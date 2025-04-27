
/*
 * Copyright(c) Realtek Semiconductor Corporation, 2020
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL9303 board
 *
 * Feature : RTL9303 8XGE board database
 *
 */

/*
 *  8*10G mode, has 8 ports: 0, 8, 16, 20, 24, 25, 26 ,27
 */
static hwp_swDescp_t rtl9303_6x8254L_6xspi_swDescp0 = {

    .chip_id                    = RTL9303_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 0,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id = 0,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 20, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 4, .phy_idx = 0,        .smi = 0, .phy_addr = 0,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 25, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 5, .phy_idx = 0,        .smi = 0, .phy_addr = 1,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 6, .phy_idx = 0,        .smi = 0, .phy_addr = 2,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 27, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 7, .phy_idx = 0,        .smi = 0, .phy_addr = 3,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE, .phy_mdi_pin_swap = 0,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x20B,        /* 10G/5G/2.5G link */
        .led_definition_set[0].led[1] = 0x2A0,        /* 1G/100Mbps link */
        .led_definition_set[0].led[2] = 0xBFF,        /* link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8254L, .mac_id = 24,  .phy_max = 4 },
        [1] = { .chip = HWP_END },
    }   /* .phy.descp */
};

static hwp_swDescp_t rtl9303_6x8254L_6xspi_swDescp1 = {

    .chip_id                    = RTL9303_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 1,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id = 0,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 20, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 4, .phy_idx = 0,        .smi = 0, .phy_addr = 5,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 25, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 5, .phy_idx = 0,        .smi = 0, .phy_addr = 6,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 6, .phy_idx = 0,        .smi = 0, .phy_addr = 7,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 27, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 7, .phy_idx = 0,        .smi = 0, .phy_addr = 8,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE, .phy_mdi_pin_swap = 0,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x20B,        /* 10G/5G/2.5G link */
        .led_definition_set[0].led[1] = 0x2A0,        /* 1G/100Mbps link */
        .led_definition_set[0].led[2] = 0xBFF,        /* link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8254L, .mac_id = 24,  .phy_max = 4 },
        [1] = { .chip = HWP_END },
    }   /* .phy.descp */
};


static hwp_swDescp_t rtl9303_6x8254L_6xspi_swDescp2 = {

    .chip_id                    = RTL9303_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 2,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id = 0,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 20, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 4, .phy_idx = 0,        .smi = 0, .phy_addr = 8,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 25, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 5, .phy_idx = 0,        .smi = 0, .phy_addr = 9,        .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 6, .phy_idx = 0,        .smi = 0, .phy_addr = 10,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 27, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 7, .phy_idx = 0,        .smi = 0, .phy_addr = 11,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE, .phy_mdi_pin_swap = 0,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x20B,        /* 10G/5G/2.5G link */
        .led_definition_set[0].led[1] = 0x2A0,        /* 1G/100Mbps link */
        .led_definition_set[0].led[2] = 0xBFF,        /* link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8254L, .mac_id = 24,  .phy_max = 4 },
        [1] = { .chip = HWP_END },
    }   /* .phy.descp */
};

static hwp_swDescp_t rtl9303_6x8254L_6xspi_swDescp3 = {

    .chip_id                    = RTL9303_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 3,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id = 0,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 20, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 4, .phy_idx = 0,        .smi = 0, .phy_addr = 13,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 25, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 5, .phy_idx = 0,        .smi = 0, .phy_addr = 14,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 6, .phy_idx = 0,        .smi = 0, .phy_addr = 15,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 27, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 7, .phy_idx = 0,        .smi = 0, .phy_addr = 16,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE, .phy_mdi_pin_swap = 0,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x20B,        /* 10G/5G/2.5G link */
        .led_definition_set[0].led[1] = 0x2A0,        /* 1G/100Mbps link */
        .led_definition_set[0].led[2] = 0xBFF,        /* link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8254L, .mac_id = 24,  .phy_max = 4 },
        [1] = { .chip = HWP_END },
    }   /* .phy.descp */
};

static hwp_swDescp_t rtl9303_6x8254L_6xspi_swDescp4 = {

    .chip_id                    = RTL9303_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 4,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id = 0,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 20, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 4, .phy_idx = 0,        .smi = 0, .phy_addr = 16,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 25, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 5, .phy_idx = 0,        .smi = 0, .phy_addr = 17,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 6, .phy_idx = 0,        .smi = 0, .phy_addr = 18,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 27, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 7, .phy_idx = 0,        .smi = 0, .phy_addr = 19,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE, .phy_mdi_pin_swap = 0,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x20B,        /* 10G/5G/2.5G link */
        .led_definition_set[0].led[1] = 0x2A0,        /* 1G/100Mbps link */
        .led_definition_set[0].led[2] = 0xBFF,        /* link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8254L, .mac_id = 24,  .phy_max = 4 },
        [1] = { .chip = HWP_END },
    }   /* .phy.descp */
};


static hwp_swDescp_t rtl9303_6x8254L_6xspi_swDescp5 = {

    .chip_id                    = RTL9303_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_SPI,
    .swcore_spi_chip_select     = 5,
    .nic_supported              = FALSE,

    .port.descp = {
        { .mac_id = 0,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 0, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 8,  .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 1, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 16, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 2, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 20, .attr = HWP_ETH, .eth = HWP_5GE,  .medi = HWP_NONE,   .sds_idx = 3, .phy_idx = HWP_NONE, .smi = 1, .phy_addr = HWP_NONE, .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 24, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 4, .phy_idx = 0,        .smi = 0, .phy_addr = 20,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 25, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 5, .phy_idx = 0,        .smi = 0, .phy_addr = 21,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 26, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 6, .phy_idx = 0,        .smi = 0, .phy_addr = 22,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 27, .attr = HWP_ETH, .eth = HWP_XGE,  .medi = HWP_COPPER, .sds_idx = 7, .phy_idx = 0,        .smi = 0, .phy_addr = 23,       .led_c = 0, .led_f = 0, .led_layout = SINGLE_SET, .phy_mdi_pin_swap = 0,},
        { .mac_id = 28, .attr = HWP_CPU, .eth = HWP_NONE, .medi = HWP_NONE,   .sds_idx = HWP_NONE, .phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr =  HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE, .phy_mdi_pin_swap = 0,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = 0x20B,        /* 10G/5G/2.5G link */
        .led_definition_set[0].led[1] = 0x2A0,        /* 1G/100Mbps link */
        .led_definition_set[0].led[2] = 0xBFF,        /* link/act */
        .led_definition_set[0].led[3] = HWP_LED_END,  /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = 2, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [1] = { .sds_id = 3, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [2] = { .sds_id = 4, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [3] = { .sds_id = 5, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [4] = { .sds_id = 6, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [5] = { .sds_id = 7, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [6] = { .sds_id = 8, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [7] = { .sds_id = 9, .mode = RTK_MII_USXGMII_10GSXGMII,  .rx_polarity = SERDES_POLARITY_NORMAL, .tx_polarity = SERDES_POLARITY_NORMAL },
        [8] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = RTK_PHYTYPE_RTL8254L, .mac_id = 24,  .phy_max = 4 },
        [1] = { .chip = HWP_END },
    }   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9303_6x8254L_6xspi = {

    .identifier.name        = "RTL9303_6x8254L_6xSPI",
    .identifier.id          = HWP_RTL9303_6x8254L_6xSPI,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 6,
    .swDescp = {
        [0]                 = &rtl9303_6x8254L_6xspi_swDescp0,
        [1]                 = &rtl9303_6x8254L_6xspi_swDescp1,
        [2]                 = &rtl9303_6x8254L_6xspi_swDescp2,
        [3]                 = &rtl9303_6x8254L_6xspi_swDescp3,
        [4]                 = &rtl9303_6x8254L_6xspi_swDescp4,
        [5]                 = &rtl9303_6x8254L_6xspi_swDescp5,
    }

};
