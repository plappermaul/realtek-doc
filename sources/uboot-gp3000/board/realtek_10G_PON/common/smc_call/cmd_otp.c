/*
 * Copyright (C) 2020 Realtek
 */
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include "cortina_smc.h"

int do_otp_command(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd;
    char *endp;
	int ret = CMD_RET_USAGE;
	u32 i=1;
	u8 *rbuf = (u8 *)0x05000000;
	u8 min_rclean_byte = 32;

	/* need at least two arguments */
	if (argc < 3) {
        return CMD_RET_USAGE;
	}

	cmd = argv[i++];

	if (strncmp(cmd, "write", 5) == 0) {
        cmd = argv[i++];
        if (argc == 4) {
            if (strcmp(cmd, "rotpk") == 0) {
                ret = SMC_CALL_OTP_WRITE_ROTPK_HASH(simple_strtoul(argv[i++], &endp, 16));
            } else if (strcmp(cmd, "bl33key") == 0) {
                ret = SMC_CALL_OTP_WRITE_BL33_AESKEY(simple_strtoul(argv[i++], &endp, 16));
            } else if (strcmp(cmd, "aesuqekey") == 0) {
                ret = SMC_CALL_OTP_WRITE_AES_UNIQUE_KEY(simple_strtoul(argv[i++], &endp, 16));
            }
        }
    } else if(strncmp(cmd, "read", 4) == 0) {
        cmd = argv[i++];
        if (argc == 4) {
            if (strcmp(cmd, "rotpk") == 0) {
                rbuf = (u8 *)simple_strtoul(argv[i++], &endp, 16);
                ret = SMC_CALL_OTP_READ_ROTPK_HASH((unsigned long)&rbuf[0]);
                int j;
                int wc_in_line = 0;

                printf(" 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
                printf("-----------------------------------------------\n");
                for (j = 0; j< 32; j++) {
                    printf("%02x ", *rbuf++);
                    wc_in_line++;
                    if (wc_in_line >= 16) {
                        printf("\n");
                        wc_in_line = 0;
                    }
                }
                if (wc_in_line != 0) {
                    printf("\n");
                }
            }
         } else if (argc == 3) {
            memset(&rbuf[0], 0, min_rclean_byte);
            if (strcmp(cmd, "chip_mode") == 0) {
                ret = SMC_CALL_OTP_READ_CHIP_MODE((unsigned long)&rbuf[0]);
                printf("chip_mode: 0x%02x 0x%02x\n",rbuf[0],rbuf[1]);
            } else if (strcmp(cmd, "dram_model") == 0) {
                ret = SMC_CALL_OTP_READ_DRAM_MODEL((unsigned long)&rbuf[0]);
                printf("dram_model: 0x%02x 0x%02x\n",rbuf[0],rbuf[1]);
            } else if (strcmp(cmd, "bl33key") == 0) {
                ret = SMC_CALL_OTP_READ_BL33_AESKEY();
            } else if (strcmp(cmd, "aesuqekey") == 0) {
                ret = SMC_CALL_OTP_READ_AES_UNIQUE_KEY();
            }
        }
    }
    return ret;
}


U_BOOT_CMD(
    otp,  6,  1,  do_otp_command,
	"otp sub-system:",
	"write rotpk <src_addr>     => Write ROTPK assigned in <src_addr> to OTP.\n"
	"otp write bl33key <src_addr>   => Write BL33 Key assigned in <src_addr> to OTP.\n"
	"otp write aesuqekey <src_addr> => Write AES Unique Key assigned in <src_addr> to OTP.\n"
	"otp read rotpk <src_addr>      => Read back ROTPK from OTP to <src_addr>\n"
	"otp read bl33key               => Read back BL33 Key from OTP\n"
	"otp read aesuqekey             => Read back AES Key from OTP\n"
	"otp read chip_mode             => Read back chip_mode from OTP\n"
	"otp read dram_model            => Read back dram_model from OTP\n"
	"\n"
	"rotpk     => 256-bit ROTPK hash\n"
	"bl33key   => 256-bit AES KEY for u-boot decryption\n"
	"aesuqekey => 256-bit AES KEY for data encryption/decryption\n"
	"chip_mode => Chip mode number\n"
	"dram_model=> DRAM model\n"
	"src_addr  => The data source in dram, which will be written into secure OTP memory.\n"
);

