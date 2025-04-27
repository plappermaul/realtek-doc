/*
 * Copyright(c) Realtek Semiconductor Corporation, 2019
 * All rights reserved.
 *
 * Purpose : Related implementation of the RTL9310 board
 *
 * Feature : RTL9310 default board database for user mode LKM
 *
 */



/*
 * General mode
 */
static hwp_swDescp_t rtl9311_usermodelkm_pci_default_swDescp = {

    .chip_id                    = RTL9311_CHIP_ID,
    .swcore_supported           = TRUE,
    .swcore_access_method       = HWP_SW_ACC_PCIe,
    .swcore_spi_chip_select     = HWP_NOT_USED,
    .nic_supported              = TRUE,

    .port.descp = {
        { .mac_id = 56, .attr = HWP_CPU,   .eth = HWP_NONE, .medi = HWP_NONE,    .sds_idx = HWP_NONE,.phy_idx = HWP_NONE, .smi = HWP_NONE, .phy_addr = HWP_NONE, .led_c = HWP_NONE, .led_f = HWP_NONE,.led_layout = HWP_NONE,  },
        { .mac_id = HWP_END },
    },  /* port.descp */

    .led.descp = {
        .led_active = LED_ACTIVE_LOW,
        .led_if_sel = LED_IF_SEL_SERIAL,
        .led_definition_set[0].led[0] = HWP_LED_END,      /* None */
    },/* led.descp */

    .serdes.descp = {
        [0] = { .sds_id = HWP_END },
    }, /* serdes.descp */

    .phy.descp = {
        [0] = { .chip = HWP_END },
    }   /* .phy.descp */
};


/*
 * hardware profile
 */
static hwp_hwProfile_t rtl9311_usermodelkm_pci_default = {
    .identifier.name        = "RTL9311_USERMODELKM_PCI_DEFAULT",
    .identifier.id          = HWP_RTL9311_USERMODELKM_PCI_DEFAULT,

    .soc.swDescp_index      = 0,
    .soc.slaveInterruptPin  = HWP_NONE,

    .sw_count               = 1,
    .swDescp = {
        [0]                 = &rtl9311_usermodelkm_pci_default_swDescp,
    }

};
