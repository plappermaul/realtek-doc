#include <soc.h>
#include <linux/ctype.h>
#include <misc_setting.h>


int do_otp_command (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    u32_t entry;
    u16_t key_per_entry;
    u8_t *ptr8 = (u8_t *)simple_strtoul(argv[3], NULL, 16);

    u32_t EFUSE_IND_WD, EFUSE_IND_CMD, EFUSE_IND_RD;

    if (otto_is_apro()){
        EFUSE_IND_WD = 0xBB000018;
        EFUSE_IND_CMD= 0xBB00001C;
        EFUSE_IND_RD = 0xBB000020;
    } else {
        EFUSE_IND_WD = 0xBB00001C;
        EFUSE_IND_CMD= 0xBB000020;
        EFUSE_IND_RD = 0xBB000024;
    }

    if(strcmp(argv[2],"rotpk") == 0){
        if(strcmp(argv[1],"write") == 0){
            for(entry=223 ; entry<=238 ; entry++,ptr8+=2){
                memcpy((u8_t *)&key_per_entry, ptr8, sizeof(u16_t));
                REG32(EFUSE_IND_WD) = key_per_entry;
                while(REG32(EFUSE_IND_RD)>>16 & 1);
                REG32(EFUSE_IND_CMD) = 0x30000|entry;
            }
        }else if(strcmp(argv[1],"read") == 0){
            for(entry=223 ; entry<=238 ; entry++,ptr8+=2){
                REG32(EFUSE_IND_CMD) = 0x10000|entry;
                while(REG32(EFUSE_IND_RD)>>16 & 1);
                key_per_entry = REG32(EFUSE_IND_RD)&0xFFFF;
                memcpy(ptr8, (u8_t *)&key_per_entry, sizeof(u16_t));
            }
        }else{
            printf("EE: error command\n");
            return -1;
        }
    }else {
        printf("EE: error command\n");
        return -1;
    }
    return 0;
}

U_BOOT_CMD(
    otp,    10,    0,     do_otp_command,
    "do Public Key Write/Read",
    "[write/read rotpk <ram_addr>]\n"
    "    - do Public Key Write/Read"
);


