#include <common.h>
#include <nand.h>
#include "cortina_spi_nand.h"

int do_spi_nand_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct mtd_info *mtd;
	mtd = get_nand_dev_by_index(0);
	if (!mtd)
		return 1;

	struct nand_chip *chip = mtd_to_nand(mtd);
    struct spi_nand_info *info = (struct spi_nand_info *)chip->priv;

    int ret;
    u32 vir_erase_start = info->mtd->size - info->block_size*4;
    u32 phy_erase_start = spi_nand_get_phy_offset(mtd,vir_erase_start);
    u32 input_addr, input_len;
    u32 i;
    if ( 0==(strcmp(argv[1],"erasebbt"))) {
        nand_acc_phy_offs_addr(1);
        printf("Erase BBTv ");
        for (i=0; i<4; i++) {
            // Erase block
            ret = spi_nand_block_markunbad(info, (uint64_t)phy_erase_start);
            if (ret) {
                printf("\nEE: offset 0x%x erase failed\n",phy_erase_start);
                nand_acc_phy_offs_addr(0);
                return -1;
            }
            printf("%x ",vir_erase_start+i*info->block_size);

            phy_erase_start += info->block_size;
        }
        nand_acc_phy_offs_addr(0);
     } else if ( 0==(strcmp(argv[1],"scrub"))) {
        nand_acc_phy_offs_addr(1);
        printf("Erase Physical ");
        input_addr = simple_strtoul(argv[2], NULL, 16);
        input_len = simple_strtoul(argv[3], NULL, 16);
        for (i=0; i<(input_len/info->block_size) ; i++) {
            printf("0x%x ",input_addr);
            ret = spi_nand_block_markunbad(info, (uint64_t)input_addr);
            input_addr += info->block_size;
        }
        printf("\n");
        nand_acc_phy_offs_addr(0);
     }

    return 0;
}


U_BOOT_CMD(
    spinand, 10, 1, do_spi_nand_cmd,
    "Realtek spi-nand flash command.",
    "erasebbt\n"\
    "    => Erase bbt table in flash when doing [run updev].\n" \
);
