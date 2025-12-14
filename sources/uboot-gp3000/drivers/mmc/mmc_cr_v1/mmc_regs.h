/***********************************************************************
 *  mmc_reg.h
 ************************************************************************/

#ifndef MMC_REG_H
#define MMC_REG_H

#include <reg_skc35.h>
#include <asm/io.h>

#ifndef __BYTE_ORDER__
#       error "__BYTE_ORDER__ is not defined"
#endif

#if (__BYTE_ORDER__ != __ORDER_BIG_ENDIAN__) && \
    (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__)
#       error "unknown __BYTE_ORDER__ value"
#endif

//#ifndef _REG_CR_MMC
//#       define _REG_CR_MMC (0xf4400400)
//#endif
#       define _REG_CR_MMC (0xf4400400)

//typedef volatile unsigned char * regvalb;
//#define RVALB(__reg) (*((regvalb)__reg##ar))

#define SKC35_REG8_DEC(__reg, __addr, ...) \
	typedef union { \
		struct { \
			__VA_ARGS__ \
		} f; \
		unsigned char v;	\
	} __reg##_T; \
	static const unsigned long __reg##ar = __addr;

SKC35_REG_DEC(CR_SRAM_CTL, _REG_CR_MMC+0x0,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(31, 24);
               RFIELD(23, 20, albm_hprot_cfg);
               RFIELD(19, 16, ahbm_hburst_cfg);
               RF_RSV(15, 6);
               RFIELD(5, 5, map_sel);
               RFIELD(4, 4, access_en);
               RFIELD(3, 0, mem_region);
#else
               RFIELD(3, 0, mem_region);
               RFIELD(4, 4, access_en);
               RFIELD(5, 5, map_sel);
               RF_RSV(15, 6);
               RFIELD(19, 16, ahbm_hburst_cfg);
               RFIELD(23, 20, albm_hprot_cfg);
               RF_RSV(31, 24);
#endif
               );

SKC35_REG_DEC(CR_DMA_CTL1, _REG_CR_MMC+0x4,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(31, 31);
               RFIELD(30, 0, sa);
#else
               RFIELD(30, 0, sa);
               RF_RSV(31, 31);
#endif
               );

SKC35_REG_DEC(CR_DMA_CTL2, _REG_CR_MMC+0x8,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(31, 16);
               RFIELD(15, 0, dma_len);
#else
               RFIELD(15, 0, dma_len);
               RF_RSV(31, 16);
#endif
               );

SKC35_REG_DEC(CR_DMA_CTL3, _REG_CR_MMC+0xC,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(31, 7);
               RFIELD(6, 6, dma_buf_512b_mode);
               RFIELD(5, 5, dat64_sel);
               RFIELD(4, 4, rsp17_sel);
               RF_RSV(3, 2);
               RFIELD(1, 1, ddr_wr);
               RFIELD(0, 0, dma_xfer);
#else
               RFIELD(0, 0, dma_xfer);
               RFIELD(1, 1, ddr_wr);
               RF_RSV(3, 2);
               RFIELD(4, 4, rsp17_sel);
               RFIELD(5, 5, dat64_sel);
               RFIELD(6, 6, dma_buf_512b_mode);
               RF_RSV(31, 7);
#endif
               );

SKC35_REG_DEC(CR_CKGEN_CTL, _REG_CR_MMC+0x78,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(31, 19);
               RFIELD(18, 18, sd30_sample_change);
               RFIELD(17, 17, sd30_push_change);
               RFIELD(16, 16, crc_clk_change);
               RF_RSV(15, 14);
               RFIELD(13, 12, sd30_sample_clk_src);
               RF_RSV(11, 10);
               RFIELD(9, 8, sd30_push_clk_src);
               RF_RSV(7, 6);
               RFIELD(5, 4, crc_clk_src);
               RF_RSV(3, 3);
               RFIELD(2, 0, crc_div);
#else
               RFIELD(2, 0, crc_div);
               RF_RSV(3, 3);
               RFIELD(5, 4, crc_clk_src);
               RF_RSV(7, 6);
               RFIELD(9, 8, sd30_push_clk_src);
               RF_RSV(11, 10);
               RFIELD(13, 12, sd30_sample_clk_src);
               RF_RSV(15, 14);
               RFIELD(16, 16, crc_clk_change);
               RFIELD(17, 17, sd30_push_change);
               RFIELD(18, 18, sd30_sample_change);
               RF_RSV(31, 19);
#endif
               );

SKC35_REG8_DEC(CARD_STOP, _REG_CR_MMC+0x103,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(7, 6);
               RFIELD(5, 5, target_ms2);
               RFIELD(4, 4, target_sd2);
               RFIELD(3, 3, target_ms);
               RFIELD(2, 2, target_sd);
               RFIELD(1, 1, target_sm_xd);
               RFIELD(0, 0, target_cf_md);
#else
               RFIELD(0, 0, target_cf_md);
               RFIELD(1, 1, target_sm_xd);
               RFIELD(2, 2, target_sd);
               RFIELD(3, 3, target_ms);
               RFIELD(4, 4, target_sd2);
               RFIELD(5, 5, target_ms2);
               RF_RSV(7, 6);
#endif
               );

SKC35_REG8_DEC(CARD_OE, _REG_CR_MMC+0x104,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(7, 6);
               RFIELD(5, 5, target_ms2);
               RFIELD(4, 4, target_sd2);
               RFIELD(3, 3, target_ms);
               RFIELD(2, 2, target_sd);
               RFIELD(1, 1, target_sm_xd);
               RFIELD(0, 0, target_cf_md);
#else
               RFIELD(0, 0, target_cf_md);
               RFIELD(1, 1, target_sm_xd);
               RFIELD(2, 2, target_sd);
               RFIELD(3, 3, target_ms);
               RFIELD(4, 4, target_sd2);
               RFIELD(5, 5, target_ms2);
               RF_RSV(7, 6);
#endif
               );

SKC35_REG8_DEC(CARD_SELECT, _REG_CR_MMC+0x10E,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(7, 3);
               RFIELD(2, 0, card_select);
#else
               RFIELD(2, 0, card_select);
               RF_RSV(7, 3);
#endif
               );

SKC35_REG8_DEC(SD_CONFIGURE1, _REG_CR_MMC+0x180,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, init_mode);
               RFIELD(6, 6, clock_divider);
               RFIELD(5, 5, hs400_mode_sel);
               RFIELD(4, 4, sd30_async_fifo_rst_n);
               RFIELD(3, 2, mode_sel);
               RFIELD(1, 0, bus_width);
#else
               RFIELD(1, 0, bus_width);
               RFIELD(3, 2, mode_sel);
               RFIELD(4, 4, sd30_async_fifo_rst_n);
               RFIELD(5, 5, hs400_mode_sel);
               RFIELD(6, 6, clock_divider);
               RFIELD(7, 7, init_mode);
#endif
               );

SKC35_REG8_DEC(SD_CONFIGURE2, _REG_CR_MMC+0x181,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, crc7_cal_en);
               RFIELD(6, 6, crc16_check_en);
               RFIELD(5, 5, wait_wr_crc_status_time_out_en);
               RFIELD(4, 4, ignore_wr_crc_err_en);
               RFIELD(3, 3, wait_busy_end_en);
               RFIELD(2, 2, crc7_check_en);
               RFIELD(1, 0, resp_type_conf);
#else
               RFIELD(1, 0, resp_type_conf);
               RFIELD(2, 2, crc7_check_en);
               RFIELD(3, 3, wait_busy_end_en);
               RFIELD(4, 4, ignore_wr_crc_err_en);
               RFIELD(5, 5, wait_wr_crc_status_time_out_en);
               RFIELD(6, 6, crc16_check_en);
               RFIELD(7, 7, crc7_cal_en);
#endif
               );

SKC35_REG8_DEC(SD_CONFIGURE3, _REG_CR_MMC+0x182,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, stop_cmd_start_no_wait_card_busy);
               RFIELD(6, 6, cmd_start_wait_no_card_busy);
               RFIELD(5, 5, data_phase_wait_card_busy_en);
               RFIELD(4, 4, sd30_clock_stop);
               RFIELD(3, 3, sd20_clock_stop);
               RFIELD(2, 2, sd_cmd_resp_check_en);
               RFIELD(1, 1, address_mode);
               RFIELD(0, 0, sd_cmd_resp_time_out_en);
#else
               RFIELD(0, 0, sd_cmd_resp_time_out_en);
               RFIELD(1, 1, address_mode);
               RFIELD(2, 2, sd_cmd_resp_check_en);
               RFIELD(3, 3, sd20_clock_stop);
               RFIELD(4, 4, sd30_clock_stop);
               RFIELD(5, 5, data_phase_wait_card_busy_en);
               RFIELD(6, 6, cmd_start_wait_no_card_busy);
               RFIELD(7, 7, stop_cmd_start_no_wait_card_busy);
#endif
               );

SKC35_REG8_DEC(SD_STATUS1, _REG_CR_MMC+0x183,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, crc7_err);
               RFIELD(6, 6, crc16_err);
               RFIELD(5, 5, write_crc_err);
               RFIELD(4, 2, status_wr_crc);
               RFIELD(1, 1, get_wr_crc_status_timeout_err);
               RFIELD(0, 0, sd_tuning_pattern_comp_err);
#else
               RFIELD(0, 0, sd_tuning_pattern_comp_err);
               RFIELD(1, 1, get_wr_crc_status_timeout_err);
               RFIELD(4, 2, status_wr_crc);
               RFIELD(5, 5, write_crc_err);
               RFIELD(6, 6, crc16_err);
               RFIELD(7, 7, crc7_err);
#endif
               );

SKC35_REG8_DEC(SD_STATUS2, _REG_CR_MMC+0x184,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(0, 0, sd_cmd_resp_timeout_err);
               RFIELD(1, 1, sd_cmd_resp_inv);
               RFIELD(5, 2, cbw_state_machine);
               RF_RSV(7, 6);
#else
               RF_RSV(7, 6);
               RFIELD(5, 2, cbw_state_machine);
               RFIELD(1, 1, sd_cmd_resp_inv);
               RFIELD(0, 0, sd_cmd_resp_timeout_err);
#endif
               );

SKC35_REG8_DEC(SD_BUS_STATUS, _REG_CR_MMC+0x185,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, sd_clk_toggle_en);
               RFIELD(6, 6, stop_sd_clk_toggle);
               RF_RSV(5, 5);
               RFIELD(4, 4, dat3);
               RFIELD(3, 3, dat2);
               RFIELD(2, 2, dat1);
               RFIELD(1, 1, dat0);
               RFIELD(0, 0, cmd);
#else
               RFIELD(0, 0, cmd);
               RFIELD(1, 1, dat0);
               RFIELD(2, 2, dat1);
               RFIELD(3, 3, dat2);
               RFIELD(4, 4, dat3);
               RF_RSV(5, 5);
               RFIELD(6, 6, stop_sd_clk_toggle);
               RFIELD(7, 7, sd_clk_toggle_en);
#endif
               );

SKC35_REG8_DEC(SD_SAMPLE_POINT_CTL, _REG_CR_MMC+0x187,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, sp_type_dat);
               RFIELD(6, 6, sp_sel_dat);
               RFIELD(5, 5, sp_type_cmd);
               RFIELD(4, 4, sp_sel_cmd);
               RFIELD(3, 3, sp_sel_sd20_cmd_dat);
               RF_RSV(2, 0);
#else
               RF_RSV(2, 0);
               RFIELD(3, 3, sp_sel_sd20_cmd_dat);
               RFIELD(4, 4, sp_sel_cmd);
               RFIELD(5, 5, sp_type_cmd);
               RFIELD(6, 6, sp_sel_dat);
               RFIELD(7, 7, sp_type_dat);
#endif
               );

SKC35_REG8_DEC(SD_PUSH_POINT_CTL, _REG_CR_MMC+0x188,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, pp_type_cmd_dat);
               RFIELD(6, 6, pp_sel_dat);
               RFIELD(5, 5, pp_sel_cmd);
               RFIELD(4, 4, pp_sel_sd230_cmd_dat);
               RFIELD(3, 3, sample_edge);
               RFIELD(2, 1, polarity_sol);
               RFIELD(0, 0, polarity_swap);
#else
               RFIELD(0, 0, polarity_swap);
               RFIELD(2, 1, polarity_sol);
               RFIELD(3, 3, sample_edge);
               RFIELD(4, 4, pp_sel_sd230_cmd_dat);
               RFIELD(5, 5, pp_sel_cmd);
               RFIELD(6, 6, pp_sel_dat);
               RFIELD(7, 7, pp_type_cmd_dat);
#endif
               );

SKC35_REG8_DEC(SD_CMD0, _REG_CR_MMC+0x189,
               RFIELD(7, 0, command);
               );

SKC35_REG8_DEC(SD_CMD1, _REG_CR_MMC+0x18a,
               RFIELD(7, 0, command);
               );

SKC35_REG8_DEC(SD_CMD2, _REG_CR_MMC+0x18b,
               RFIELD(7, 0, command);
               );

SKC35_REG8_DEC(SD_CMD3, _REG_CR_MMC+0x18c,
               RFIELD(7, 0, command);
               );

SKC35_REG8_DEC(SD_CMD4, _REG_CR_MMC+0x18d,
               RFIELD(7, 0, command);
               );

SKC35_REG8_DEC(SD_CMD5, _REG_CR_MMC+0x18e,
               RFIELD(7, 0, command);
               );

SKC35_REG8_DEC(SD_BYTE_CNT_L, _REG_CR_MMC+0x18f,
               RFIELD(7, 0, byte_cnt);
               );

SKC35_REG8_DEC(SD_BYTE_CNT_H, _REG_CR_MMC+0x190,
               RFIELD(7, 0, byte_cnt);
               );

SKC35_REG8_DEC(SD_BLOCK_CNT_L, _REG_CR_MMC+0x191,
               RFIELD(7, 0, blk_cnt);
               );

SKC35_REG8_DEC(SD_BLOCK_CNT_H, _REG_CR_MMC+0x192,
               RFIELD(7, 0, blk_cnt);
               );

SKC35_REG8_DEC(SD_TRANSFER, _REG_CR_MMC+0x193,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, start);
               RFIELD(6, 6, end);
               RFIELD(5, 5, status);
               RFIELD(4, 4, err);
               RFIELD(3, 0, cmd_code);
#else
               RFIELD(3, 0, cmd_code);
               RFIELD(4, 4, err);
               RFIELD(5, 5, status);
               RFIELD(6, 6, end);
               RFIELD(7, 7, start);
#endif
               );
SKC35_REG8_DEC(SD_AUTO_RST_FIFO, _REG_CR_MMC+0x199,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RFIELD(7, 7, force_async_fifo_en_hs400);
               RFIELD(6, 6, force_async_fifo_strobe_hs400);
               RFIELD(5, 5, async_fifo_sample_clk_sel_dl_hs400);
               RFIELD(4, 4, async_fifo_sample_clk_sel_cl_hs400);
               RF_RSV(3, 2);
               RFIELD(1, 1, auto_rst_fifo_en_ds);
               RFIELD(0, 0, auto_rst_fifo_en);
#else
               RFIELD(0, 0, auto_rst_fifo_en);
               RFIELD(1, 1, auto_rst_fifo_en_ds);
               RF_RSV(3, 2);
               RFIELD(4, 4, async_fifo_sample_clk_sel_cl_hs400);
               RFIELD(5, 5, async_fifo_sample_clk_sel_dl_hs400);
               RFIELD(6, 6, force_async_fifo_strobe_hs400);
               RFIELD(7, 7, force_async_fifo_en_hs400);
#endif
               );

SKC35_REG8_DEC(DS_DELAY_CFG, _REG_CR_MMC+0x1B0,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(7, 6);
               RFIELD(5, 0, delay);
#else
               RFIELD(5, 0, delay);
               RF_RSV(7, 6);
#endif
               );

SKC35_REG_DEC(SD_SUPPORT_INFO, _REG_CR_MMC+0x4AC,
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
               RF_RSV(31, 4);
               RFIELD(3, 3, sd_34bits_support);
               RFIELD(2, 2, sd_cross_4Gb_support);
               RFIELD(1, 1, sd_hs200_support);
               RFIELD(0, 0, sd_hs400_support);
#else
               RFIELD(0, 0, sd_hs400_support);
               RFIELD(1, 1, sd_hs200_support);
               RFIELD(2, 2, sd_cross_4Gb_support);
               RFIELD(3, 3, sd_34bits_support);
               RF_RSV(31, 4);
#endif
               );

#if 0

#define _REG_PAD_CTRL (0x01000100)
SKC35_REG_DEC(
	SRCR, (_REG_PAD_CTRL + 0x8),
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	RF_RSV(31, 2);
	RFIELD(1, 1, pad_sd_rst_n_oe);
	RFIELD(0, 0, pad_sd_rst_n);
#else
	RFIELD(0, 0, pad_sd_rst_n);
	RFIELD(1, 1, pad_sd_rst_n_oe);
	RF_RSV(31, 2);
#endif
	);

#endif

#define _REG_PHY_CTRL (0x01000500)
SKC35_REG_DEC(
	SD30PLLR3, (_REG_PHY_CTRL + 0x5C),
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	RFIELD(31, 0, sd30_sscpll_ctrl3);
#else
	RFIELD(31, 0, sd30_sscpll_ctrl3);
#endif
	);

#if 0
#ifndef _REG_IP_ENABLE
#	define _REG_IP_ENABLE (0x01000600)
#endif

SKC35_REG_DEC(
	IPECR, (_REG_IP_ENABLE + 0x0),
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	RFIELD(31, 31, en_flash_ecc);
	RFIELD(30, 30, en_spi_nand);
	RFIELD(29, 29, en_pcie_rc);
	RFIELD(28, 28, en_pcie_ep);
	RFIELD(27, 27, en_p_nand);
	RF_RSV(26, 26);
	RFIELD(25, 25, en_gmac);
	RFIELD(24, 24, en_gdma1);
	RFIELD(23, 23, en_usb3);
	RFIELD(22, 22, en_sd30);
	RFIELD(21, 21, en_sd30_clk);
	RF_RSV(20, 4);
	RFIELD(3, 3, en_usb3phy);
	RFIELD(2, 2, en_gmac_sds);
	RFIELD(1, 1, en_usb2phy);
	RFIELD(0, 0, en_ephy);
#else
	RFIELD(0, 0, en_ephy);
	RFIELD(1, 1, en_usb2phy);
	RFIELD(2, 2, en_gmac_sds);
	RFIELD(3, 3, en_usb3phy);
	RF_RSV(20, 4);
	RFIELD(21, 21, en_sd30_clk);
	RFIELD(22, 22, en_sd30);
	RFIELD(23, 23, en_usb3);
	RFIELD(24, 24, en_gdma1);
	RFIELD(25, 25, en_gmac);
	RF_RSV(26, 26);
	RFIELD(27, 27, en_p_nand);
	RFIELD(28, 28, en_pcie_ep);
	RFIELD(29, 29, en_pcie_rc);
	RFIELD(30, 30, en_spi_nand);
	RFIELD(31, 31, en_flash_ecc);
#endif
	);

#endif


#endif //MMC_REG_H
