#include <soc.h>
#include <common.h>
#include <misc_setting.h>


/***********************************************
  * For APro_v2
  **********************************************/
void __otp_write_entry(u16_t entry, u16_t value)
{
    u32_t EFUSE_IND_WD = 0xBB000018;
    u32_t EFUSE_IND_CMD= 0xBB00001C;
    u32_t EFUSE_IND_RD = 0xBB000020;
    u32_t otp_addr = entry;

    otp_addr = otp_addr<<1;
    REG32(EFUSE_IND_WD) = (value & 0xFF);
    REG32(EFUSE_IND_CMD) = (1<<21)|(1<<17)|(1<<16)|otp_addr;
    while(REG32(EFUSE_IND_RD)>>16 & 1);

    otp_addr = otp_addr + 1;
    REG32(EFUSE_IND_WD) = ((value>>8) & 0xFF);
    REG32(EFUSE_IND_CMD) = (1<<21)|(1<<17)|(1<<16)|otp_addr;
    while(REG32(EFUSE_IND_RD)>>16 & 1);

    return;
}

u16_t __otp_read_entry(u16_t entry)
{
    u32_t EFUSE_IND_CMD= 0xBB00001C;
    u32_t EFUSE_IND_RD = 0xBB000020;
    u32_t otp_addr = entry;
    u32_t otp_val = 0;

    otp_addr = otp_addr<<1;
    REG32(EFUSE_IND_CMD)= 0x10000|otp_addr;
    //Wait OTP ready. HW busy status response need 2us
    udelay(2);
    while(REG32(EFUSE_IND_RD)>>16 & 1);
    otp_val = (REG32(EFUSE_IND_RD) & 0xFF);

    otp_addr = otp_addr + 1;
    REG32(EFUSE_IND_CMD)= 0x10000|otp_addr;
    //Wait OTP ready. HW busy status response need 2us
    udelay(2);
    while(REG32(EFUSE_IND_RD)>>16 & 1);
    otp_val |= ((REG32(EFUSE_IND_RD) & 0xFF) << 8);

    return (u16_t) otp_val;
}


/***********************************************
  * For APro & RTL9603C-VD
  **********************************************/
void __efuse_write_entry(u16_t entry, u16_t value)
{
    u32_t EFUSE_IND_WD, EFUSE_IND_CMD, EFUSE_IND_RD;

    if (OTTO_SID == PLR_SID_APRO){
        EFUSE_IND_WD = 0xBB000018;
        EFUSE_IND_CMD= 0xBB00001C;
        EFUSE_IND_RD = 0xBB000020;
    } else if (OTTO_SID == PLR_SID_9603CVD){
        EFUSE_IND_WD = 0xBB00001C;
        EFUSE_IND_CMD= 0xBB000020;
        EFUSE_IND_RD = 0xBB000024;
    } else {
        return;
    }

    REG32(EFUSE_IND_WD) = value;
    REG32(EFUSE_IND_CMD) = 0x30000|entry;
    while(REG32(EFUSE_IND_RD)>>16 & 1){;}
}

u16_t __efuse_read_entry(u16_t entry)
{
    u32_t EFUSE_IND_CMD, EFUSE_IND_RD;

    if (OTTO_SID == PLR_SID_APRO){
        EFUSE_IND_CMD= 0xBB00001C;
        EFUSE_IND_RD = 0xBB000020;
    } else if (OTTO_SID == PLR_SID_9603CVD){
        EFUSE_IND_CMD= 0xBB000020;
        EFUSE_IND_RD = 0xBB000024;
    } else {
        return 0;
    }

	REG32(EFUSE_IND_CMD) = 0x10000|entry;
    while(REG32(EFUSE_IND_RD)>>16 & 1){;}
	return (REG32(EFUSE_IND_RD) & 0xFFFF);
}


/***********************************************
  * General API
  **********************************************/
void efuse_write_entry(u16_t entry, u16_t value)
{
    switch(OTTO_SID){
        case PLR_SID_APRO:
        case PLR_SID_9603CVD:
            __efuse_write_entry(entry, value);
            break;
        case PLR_SID_APRO_GEN2:
            __otp_write_entry(entry, value);
            break;
    }
	return;
}

u16_t efuse_read_entry(u16_t entry)
{
    u16_t val = 0;

    switch(OTTO_SID){
        case PLR_SID_APRO:
        case PLR_SID_9603CVD:
            val = __efuse_read_entry(entry);
            break;
        case PLR_SID_APRO_GEN2:
            val = __otp_read_entry(entry);
            break;
    }
	return val;
}


/***********************************************
  * Command Function
  **********************************************/
int do_otp_command (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *endp;
	u32_t i = 1;
    u16_t entry;
    u16_t key_per_entry;
    u8_t *ptr8;

    if (argc < 3) goto usage;

    if (strcmp(argv[i], "read") == 0) {
        i++;
        if (strcmp(argv[i],"rotpk") == 0) {
            ptr8 = (u8_t *)simple_strtoul(argv[++i], &endp, 16);
            for(entry=223 ; entry<=238 ; entry++,ptr8+=2){
                if (argc < 4) goto usage;
                key_per_entry = efuse_read_entry(entry);
                memcpy(ptr8, (u8_t *)&key_per_entry, sizeof(u16_t));
            }
        } else if (strcmp(argv[i],"chip_mode") == 0) {
            printf("chip_mode = 0x%x 0x%x 0x%x\n",(efuse_read_entry(3)&0x1F), (efuse_read_entry(4)&0x1F), (efuse_read_entry(5)&0x1F));
        } else if (strcmp(argv[i],"entry") == 0) {
            entry = simple_strtoul(argv[++i], &endp, 10);
            printf("entry[%d] = 0x%x\n",entry, efuse_read_entry(entry));
        } else {
            goto usage;
        }
    } else if (strcmp(argv[i], "write") == 0) {
        i++;
        if (strcmp(argv[i],"rotpk") == 0) {
            if (argc < 4) goto usage;
            ptr8 = (u8_t *)simple_strtoul(argv[++i], &endp, 16);
            for(entry=223 ; entry<=238 ; entry++,ptr8+=2){
                memcpy((u8_t *)&key_per_entry, ptr8, sizeof(u16_t));
                efuse_write_entry(entry, key_per_entry);
            }
        } else {
            goto usage;
        }
    } else {
        goto usage;
    }
    return 0;

usage:
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
    otp,    6,    1,     do_otp_command,
    "otp sub-system:",
	"write rotpk <src_addr> => Write ROTPK assigned in <src_addr> to Efuse/OTP.\n"
	"otp read rotpk <src_addr>  => Read back ROTPK from Efuse/OTP to <src_addr>\n"
	"otp read chip_mode         => Read back chip_mode from Efuse/OTP\n"
);

