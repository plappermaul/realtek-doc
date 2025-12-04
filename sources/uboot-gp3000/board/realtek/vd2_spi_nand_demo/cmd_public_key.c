#include <soc.h>
#include <linux/ctype.h>



int do_otp_command (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    u32_t entry;
    u16_t key_per_entry;
    u8_t *ptr8 = (u8_t *)simple_strtoul(argv[3], NULL, 16);

    if(strcmp(argv[2],"rotpk") == 0){
        if(strcmp(argv[1],"write") == 0){
            for(entry=40 ; entry<=55 ; entry++,ptr8+=2){
                memcpy((u8_t *)&key_per_entry, ptr8, sizeof(u16_t));
            	REG32(0xb8000644)	= (06<<24) | (entry<<16) | key_per_entry;
            	REG32(0xb8000644)	= (07<<24) | (entry<<16) | key_per_entry; 	
            	REG32(0xb8000648)	= (04<<24);
            	udelay(1000);
            }
        }else if(strcmp(argv[1],"read") == 0){
            for(entry=40 ; entry<=55 ; entry++,ptr8+=2){
                REG32(0xb8000644) = (0x01<<24) | (entry<<16);
                REG32(0xb8000644) = (0x00<<24) | (entry<<16);
                udelay(1000);
                key_per_entry = REG32(0xb8000648)&0xFFFF;
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


