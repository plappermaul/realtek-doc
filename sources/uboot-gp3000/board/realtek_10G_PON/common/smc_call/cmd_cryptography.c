/*
 * Copyright (C) 2022 Realtek
 */
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <linux/arm-smccc.h>
#include "cortina_smc.h"

#define MBEDTLS_AES_ENCRYPT     1
#define MBEDTLS_AES_DECRYPT     0


unsigned long aes256_cbc_crypt(u32 enc, unsigned long src_len, unsigned long *src_buf, unsigned long *dst_len, unsigned long *dst_buf)
{
    struct arm_smccc_res res;
    arm_smccc_smc(RTK_SVC_AES256_CBC, enc, src_len, (unsigned long)src_buf, 0, 0, (unsigned long)dst_len, (unsigned long)dst_buf, &res);
    return res.a0;
}


int do_aes256_command(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char *mode;
	const char *cmd;
    char *endp;
	unsigned long *src_buf;
	unsigned long *dst_buf;
	unsigned long src_len;
	unsigned long dst_len = 0;
	int ret = 0;

	if (argc < 6) goto usage;

	mode = argv[1];
	cmd = argv[2];
    src_len = simple_strtoul(argv[3], NULL, 16);
    src_buf = (unsigned long *)simple_strtoul(argv[4], &endp, 16);
    dst_buf = (unsigned long *)simple_strtoul(argv[5], &endp, 16);

	if (strncmp(mode, "cbc", 3) == 0) {
        if (strncmp(cmd, "enc", 3) == 0) {
            ret = aes256_cbc_crypt(MBEDTLS_AES_ENCRYPT, src_len, src_buf, &dst_len, dst_buf);
        } else if(strncmp(cmd, "dec", 4) == 0) {
            ret = aes256_cbc_crypt(MBEDTLS_AES_DECRYPT, src_len, src_buf, &dst_len, dst_buf);
        } else {
            goto usage;
        }

        if (ret == 0) {
            printf("AES256-CBC: %d bytes is %s to 0x%lx\n", (uint32_t)dst_len, (strncmp(cmd, "enc", 3) == 0)?"encrypted":"decrypted", (uintptr_t)dst_buf);
        } else if (ret == -34) {
            printf("AES256-CBC: Error %s input length\n", (strncmp(cmd, "enc", 3) == 0)?"encryption":"decryption");
        } else {
            printf("AES256-CBC: Unexpected %s error\n", (strncmp(cmd, "enc", 3) == 0)?"encryption":"decryption");
        }
	} else {
        printf("Error: Unsupported mode\n");
        goto usage;
	}

    return ret;

usage:
	return CMD_RET_USAGE;
}


U_BOOT_CMD(
    aes256, 6, 1,  do_aes256_command,
	"aes256 sub-system:",
	"cbc enc <src_len> <src_addr> <dst_addr>  => Using AES256-CBC to encrypt <src_len> bytes data from <src_addr> to <dst_addr>.\n"
	"aes256 cbc dec <src_len> <src_addr> <dst_addr>  => Using AES256-CBC to decrypt <src_len> bytes data from <src_addr> to <dst_addr>.\n"
	"\n"
	"src_len   => HEX.\n"
	"src_addr  => HEX.\n"
	"dst_addr  => HEX.\n"
);

