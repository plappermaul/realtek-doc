#
# (C) Copyright 2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

ccflags-y += -I$(srctree)/board/qca/arm/ipq40xx -I$(srctree)/board/qca/arm/common 
ccflags-y += -I$(srctree)/drivers/net/ipq_common

ifeq ($(strip $(CONFIG_TP_EXT_PHY_RTL8251B)),y)
ccflags-y += -I$(srctree)/drivers/net/rtl8251b
endif

ifeq ($(strip $(CONFIG_TP_EXT_SWITCH_RTL8367_ALL)),y)
ccflags-y += -I$(srctree)/drivers/net/rtl8367_common_V1_4_2 -I$(srctree)/drivers/net/rtl8367_common_V1_4_2/dal/ -I$(srctree)/drivers/net/rtl8367_common_V1_4_2/dal/rtl8367d/ -I$(srctree)/drivers/net/rtl8367_common_V1_4_2/dal/rtl8367c/
EXTRA_CFLAGS += $(TP_SDK_CFLAGS) -DMDC_MDIO_OPERATION -DCONFIG_DAL_RTL8367D
endif

ifeq ($(strip $(CONFIG_TP_EXT_PHY_RTL8221B)),y)
ccflags-y += -I$(srctree)/drivers/net/rtl8221b
endif

ifeq ($(strip $(CONFIG_TP_EXT_SWITCH_RTL8372)),y)
ccflags-y += -I$(srctree)/drivers/net/rtl8372 -I$(srctree)/drivers/net/rtl8372/dal/ -I$(srctree)/drivers/net/rtl8372/dal/rtl8373/
EXTRA_CFLAGS += $(TP_SDK_CFLAGS) -DCONFIG_TP_EXT_SWITCH_RTL8372 -DCONFIG_TP_EXT_SWITCH  -DMDC_MDIO_OPERATION
endif

obj-$(CONFIG_PPC4xx_EMAC) += 4xx_enet.o
obj-$(CONFIG_ALTERA_TSE) += altera_tse.o
obj-$(CONFIG_ARMADA100_FEC) += armada100_fec.o
obj-$(CONFIG_DRIVER_AT91EMAC) += at91_emac.o
obj-$(CONFIG_DRIVER_AX88180) += ax88180.o
obj-$(CONFIG_BCM_SF2_ETH) += bcm-sf2-eth.o
obj-$(CONFIG_BCM_SF2_ETH_GMAC) += bcm-sf2-eth-gmac.o
obj-$(CONFIG_BFIN_MAC) += bfin_mac.o
obj-$(CONFIG_CALXEDA_XGMAC) += calxedaxgmac.o
obj-$(CONFIG_CS8900) += cs8900.o
obj-$(CONFIG_TULIP) += dc2114x.o
obj-$(CONFIG_ETH_DESIGNWARE) += designware.o
obj-$(CONFIG_DRIVER_DM9000) += dm9000x.o
obj-$(CONFIG_DNET) += dnet.o
obj-$(CONFIG_E1000) += e1000.o
obj-$(CONFIG_E1000_SPI) += e1000_spi.o
obj-$(CONFIG_EEPRO100) += eepro100.o
obj-$(CONFIG_SUNXI_EMAC) += sunxi_emac.o
obj-$(CONFIG_ENC28J60) += enc28j60.o
obj-$(CONFIG_EP93XX) += ep93xx_eth.o
obj-$(CONFIG_ETHOC) += ethoc.o
obj-$(CONFIG_FEC_MXC) += fec_mxc.o
obj-$(CONFIG_FSLDMAFEC) += fsl_mcdmafec.o mcfmii.o
obj-$(CONFIG_FTGMAC100) += ftgmac100.o
obj-$(CONFIG_FTMAC110) += ftmac110.o
obj-$(CONFIG_FTMAC100) += ftmac100.o
obj-$(CONFIG_GRETH) += greth.o
obj-$(CONFIG_DRIVER_TI_KEYSTONE_NET) += keystone_net.o
obj-$(CONFIG_KS8851_MLL) += ks8851_mll.o
obj-$(CONFIG_LAN91C96) += lan91c96.o
obj-$(CONFIG_LPC32XX_ETH) += lpc32xx_eth.o
obj-$(CONFIG_MACB) += macb.o
obj-$(CONFIG_MCFFEC) += mcffec.o mcfmii.o
obj-$(CONFIG_MPC5xxx_FEC) += mpc5xxx_fec.o
obj-$(CONFIG_MPC512x_FEC) += mpc512x_fec.o
obj-$(CONFIG_MVGBE) += mvgbe.o
obj-$(CONFIG_MVNETA) += mvneta.o
obj-$(CONFIG_NATSEMI) += natsemi.o
obj-$(CONFIG_DRIVER_NE2000) += ne2000.o ne2000_base.o
obj-$(CONFIG_DRIVER_AX88796L) += ax88796.o ne2000_base.o
obj-$(CONFIG_NETCONSOLE) += netconsole.o
obj-$(CONFIG_NS8382X) += ns8382x.o
obj-$(CONFIG_PCH_GBE) += pch_gbe.o
obj-$(CONFIG_PCNET) += pcnet.o
obj-$(CONFIG_RTL8139) += rtl8139.o
obj-$(CONFIG_RTL8169) += rtl8169.o
obj-$(CONFIG_ETH_SANDBOX) += sandbox.o
obj-$(CONFIG_ETH_SANDBOX_RAW) += sandbox-raw.o
obj-$(CONFIG_SH_ETHER) += sh_eth.o
obj-$(CONFIG_SMC91111) += smc91111.o
obj-$(CONFIG_SMC911X) += smc911x.o
obj-$(CONFIG_DRIVER_TI_EMAC) += davinci_emac.o
obj-$(CONFIG_TSEC_ENET) += tsec.o fsl_mdio.o
obj-$(CONFIG_DRIVER_TI_CPSW) += cpsw.o
obj-$(CONFIG_FMAN_ENET) += fsl_mdio.o
obj-$(CONFIG_TSI108_ETH) += tsi108_eth.o
obj-$(CONFIG_ULI526X) += uli526x.o
obj-$(CONFIG_VSC7385_ENET) += vsc7385.o
obj-$(CONFIG_XILINX_AXIEMAC) += xilinx_axi_emac.o
obj-$(CONFIG_XILINX_EMACLITE) += xilinx_emaclite.o
obj-$(CONFIG_XILINX_LL_TEMAC) += xilinx_ll_temac.o xilinx_ll_temac_mdio.o \
		xilinx_ll_temac_fifo.o xilinx_ll_temac_sdma.o
obj-$(CONFIG_ZYNQ_GEM) += zynq_gem.o
obj-$(CONFIG_FSL_MC_ENET) += fsl-mc/
obj-$(CONFIG_FSL_MC_ENET) += ldpaa_eth/
obj-$(CONFIG_FSL_MEMAC) += fm/memac_phy.o
obj-$(CONFIG_VSC9953) += vsc9953.o
obj-$(CONFIG_IPQ40XX_EDMA) += ipq40xx/ipq40xx_edma_eth.o
obj-$(CONFIG_IPQ40XX_ESS) += ipq40xx/ipq40xx_ess_sw.o
obj-$(CONFIG_IPQ_SNPS_GMAC) += ipq806x/ipq_gmac_eth.o
obj-$(CONFIG_IPQ_SWITCH_ATHRS17) += ipq806x/athrs17_phy.o
obj-$(CONFIG_IPQ_SWITCH_QCA8511) += ipq806x/qca8511.o
obj-$(CONFIG_IPQ807X_EDMA) += ipq807x/ipq807x_edma.o
obj-$(CONFIG_IPQ807X_EDMA) += ipq807x/ipq807x_ppe.o
obj-$(CONFIG_IPQ807X_EDMA) += ipq807x/ipq807x_uniphy.o
obj-$(CONFIG_IPQ6018_EDMA) += ipq6018/ipq6018_edma.o
obj-$(CONFIG_IPQ6018_EDMA) += ipq6018/ipq6018_ppe.o
obj-$(CONFIG_IPQ6018_EDMA) += ipq6018/ipq6018_uniphy.o
obj-$(CONFIG_IPQ9574_EDMA) += ipq9574/ipq9574_ppe.o
obj-$(CONFIG_IPQ9574_EDMA) += ipq9574/ipq9574_uniphy.o
obj-$(CONFIG_IPQ9574_EDMA) += ipq9574/ipq9574_edma.o
obj-$(CONFIG_IPQ5018_GMAC) += ipq5018/ipq5018_gmac.o
obj-$(CONFIG_IPQ5018_GMAC) += ipq5018/ipq5018_uniphy.o
obj-$(CONFIG_IPQ5018_MDIO) += ipq5018/ipq5018_mdio.o
obj-$(CONFIG_IPQ5018_GMAC) += ipq5018/athrs17_phy.o
obj-$(CONFIG_IPQ5332_EDMA) += ipq5332/ipq5332_edma.o
obj-$(CONFIG_IPQ5332_EDMA) += ipq5332/ipq5332_ppe.o

ifndef CONFIG_IPQ5332_RUMI
obj-$(CONFIG_IPQ5332_EDMA) += ipq5332/ipq5332_uniphy.o
endif

ifeq ($(strip $(CONFIG_TP_EXT_SWITCH_RTL8367_ALL)),y)
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/rtl8367d_asicdrv.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/rtl8367d_smi.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_acl.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_cpu.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_dot1x.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_eee.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_gpio.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_igmp.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_interrupt.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_l2.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_leaky.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_led.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_mapper.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_mirror.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_port.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_qos.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_rate.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_rldp.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_stat.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_storm.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_svlan.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_switch.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_trap.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_trunk.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/rtl8367d/dal_rtl8367d_vlan.o

obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/rtk_switch.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/port.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/vlan.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/chip.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8367_common_V1_4_2/dal/dal_mgmt.o
endif

ifeq ($(strip $(CONFIG_TP_EXT_SWITCH_RTL8372)),y)
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_switch.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_port.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_acl.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_fc.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_isolation.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/rtl8373_asicdrv.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_mirror.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_rate.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_svlan.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/rtl8373_regField_list.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_cpuTag.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_gpio.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_led.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_nic.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_rma.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_switch.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/rtl8373_reg_list.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_dos.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_hsb.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_lut.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_parser.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_rtkpp.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_trunk.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/rtl8373_smi.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_dot1x.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_i2c.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_macsec.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_sharemeter.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_vlan.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/rtl8373_tableField_list.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_drv.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_igmp.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_mapper.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_ptp.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_storm.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_wol.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/rtl8373_table_list.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_eee.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_interrupt.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_mib.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_qos.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/rtl8373/dal_rtl8373_stp.o

obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/rtk_switch.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/port.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/vlan.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/chip.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/identify.o
obj-$(CONFIG_IPQ5332_EDMA) += rtl8372/dal/dal_mgmt.o
endif

ifeq ($(strip $(CONFIG_TP_EXT_PHY_RTL8261B)),y)
obj-$(CONFIG_TP_EXT_PHY_RTL8261B) += ipq_common/rtl8261_phy.o
obj-$(CONFIG_TP_EXT_PHY_RTL8261B) += ipq_common/rtl8261_patch.o
endif

ifeq ($(strip $(CONFIG_TP_EXT_PHY_RTL8251B)),y)
obj-$(CONFIG_TP_EXT_PHY_RTL8251B) += rtl8251b/nic_rtl8251b.o
obj-$(CONFIG_TP_EXT_PHY_RTL8251B) += rtl8251b/nic_rtl8251b_init.o
endif

ifeq ($(strip $(CONFIG_TP_EXT_PHY_RTL8221B)),y)
obj-$(CONFIG_TP_EXT_PHY_RTL8221B) += rtl8221b/nic_rtl8226b.o
obj-$(CONFIG_TP_EXT_PHY_RTL8221B) += rtl8221b/nic_rtl8226b_init.o
endif

obj-$(CONFIG_IPQ_MDIO) += ipq_common/ipq_mdio.o
obj-$(CONFIG_IPQ_QTI_BIT_BANGMII) += ipq_common/ipq_bitbangmii.o
obj-$(CONFIG_GEPHY) += ipq_common/ipq_gephy.o
obj-$(CONFIG_QCA8075_PHY) += ipq_common/ipq_qca8075.o
obj-$(CONFIG_QCA8084_PHY) += ipq_common/ipq_qca8084.o
obj-$(CONFIG_QCA8084_PHY) += ipq_common/ipq_qca8084_clk.o
obj-$(CONFIG_QCA8084_PHY) += ipq_common/ipq_qca8084_interface_ctrl.o
obj-$(CONFIG_ATHRS17C_SWITCH) += ipq_common/athrs17_phy.o
obj-$(CONFIG_IPQ9574_QCA8075_PHY) += ipq9574/ipq9574_qca8075.o
obj-$(CONFIG_QCA8033_PHY) += ipq_common/ipq_qca8033.o
obj-$(CONFIG_QCA8081_PHY) += ipq_common/ipq_qca8081.o
obj-$(CONFIG_IPQ_QCA_AQUANTIA_PHY) += ipq_common/ipq_aquantia_phy.o
obj-$(CONFIG_QCA_AQUANTIA_PHY) += ipq807x/ipq807x_aquantia_phy.o
obj-$(CONFIG_IPQ6018_QCA_AQUANTIA_PHY) += ipq6018/ipq6018_aquantia_phy.o
obj-$(CONFIG_IPQ9574_QCA_AQUANTIA_PHY) += ipq9574/ipq9574_aquantia_phy.o
