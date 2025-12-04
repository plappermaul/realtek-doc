/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <command.h>
extern int rtk_cortex_freq_set(uint target_idx);
extern int rtk_cortex_freq_get(void);
extern void rtk_cortex_freq_tbl_show(void);

static int cpu_clk_daemon(int argc, char *const argv[])
{
    uint speed_tlb_idx = 0;
    unsigned long current_rate = 0;



    if (!strcmp(argv[2], "set")) {
        if (argc >= 4) {
            speed_tlb_idx = simple_strtoul(argv[3], NULL, 10);
            rtk_cortex_freq_set(speed_tlb_idx);
        } else {
            rtk_cortex_freq_tbl_show();
        }
    } else if (!strcmp(argv[2], "get")) {
        current_rate = rtk_cortex_freq_get();
        printf("CPU Freq: %lu\n", current_rate);
    } else {
        rtk_cortex_freq_tbl_show();
    }
    return 0;
}



static int do_pll_clk(cmd_tbl_t * cmdtp, int flag, int argc,
                      char *const argv[])
{
    if (strcmp(argv[1], "cpu") == 0) {
        cpu_clk_daemon(argc, argv);
    } else {
        return cmd_usage(cmdtp);
    }
    return 0;
}

U_BOOT_CMD(pll_clk, CONFIG_SYS_MAXARGS, 2, do_pll_clk,
           "pll_clk [cpu] [set|get} [target_freq_idx]",
           "\npll_clk cpu set   - Set up CPU Frequency.\n"
           "pll_clk cpu get   - Print CPU Frequency.");
