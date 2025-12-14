#include <sihnah.h>
#include <asm/io.h>
#include "string.h"
#include "mmc.h"
#include "mmc_regs.h"
#include "mmc_error.h"

/***************** CARD READER Driver *****************
 * This is for eMMC
 ******************************************************/

struct mmcore_rsp_type mmc_rsp_type[] = {
    /* CLASS 1 */
    {  MCMD0_GO_IDLE_STATE,         MTYPE_BC,   MRSP_R0 , MCLASS(1) },
    {  MCMD1_SEND_OP_COND,          MTYPE_BCR,  MRSP_R3 , MCLASS(1) },
    {  MCMD2_ALL_SEND_CID,          MTYPE_BCR,  MRSP_R2 , MCLASS(1) },
    {  MCMD3_SET_RELATIVE_ADDR,     MTYPE_AC,   MRSP_R1 , MCLASS(1) },
    {  MCMD4_SET_DSR,               MTYPE_BC,   MRSP_R0 , MCLASS(1) },
    {  MCMD5_SLEEP_AWAKE,           MTYPE_AC,   MRSP_R1B, MCLASS(1) },
    {  MCMD6_SWITCH,                MTYPE_AC,   MRSP_R1B, MCLASS(1) },
    {  MCMD7_SELECT_CARD,           MTYPE_AC,   MRSP_R1,  MCLASS(1) },
    {  MCMD8_SEND_EXT_CSD,          MTYPE_ADTC, MRSP_R1,  MCLASS(1) },
    {  MCMD9_SEND_CSD,              MTYPE_AC,   MRSP_R2,  MCLASS(1) },
    {  MCMD10_SEND_CID,             MTYPE_AC,   MRSP_R2,  MCLASS(1) },
    {  MCMD12_STOP_TRANSMISSION,    MTYPE_AC,   MRSP_R1B, MCLASS(1) },
    {  MCMD13_SEND_STATUS,          MTYPE_AC,   MRSP_R1,  MCLASS(1) },
    {  MCMD14_BUSTEST_R,            MTYPE_ADTC, MRSP_R1,  MCLASS(1) },
    {  MCMD15_GO_INACTIVE_STATE,    MTYPE_AC,   MRSP_R0,  MCLASS(1) },
    {  MCMD19_BUSTEST_W,            MTYPE_ADTC, MRSP_R1,  MCLASS(1) },
    /* CLASS 2 */
    {  MCMD16_SET_BLOCKLEN,         MTYPE_AC,   MRSP_R1,  MCLASS(2) },
    {  MCMD17_READ_SINGLE_BLOCK,    MTYPE_ADTC, MRSP_R1,  MCLASS(2) },
    {  MCMD18_READ_MULTIPLE_BLOCK,  MTYPE_ADTC, MRSP_R1,  MCLASS(2) },
    {  MCMD21_SEND_TUNING_BLOCK,    MTYPE_ADTC, MRSP_R1,  MCLASS(2) },
    /* CLASS 4 */
    {  MCMD23_SET_BLOCK_COUNT,      MTYPE_AC,   MRSP_R1,  MCLASS(4) },
    {  MCMD24_WRITE_BLOCK,          MTYPE_ADTC, MRSP_R1,  MCLASS(4) },
    {  MCMD25_WRITE_MULTIPLE_BLOCK, MTYPE_ADTC, MRSP_R1,  MCLASS(4) },
    {  MCMD26_PROGRAM_CID,          MTYPE_ADTC, MRSP_R1,  MCLASS(4) },
    {  MCMD27_PROGRAM_CSD,          MTYPE_ADTC, MRSP_R1,  MCLASS(4) },
    {  MCMD49_SET_TIME,             MTYPE_ADTC, MRSP_R1,  MCLASS(4) },
    /* CLASS 6 */
    {  MCMD28_SET_WRITE_PROT,       MTYPE_AC,   MRSP_R1B, MCLASS(6) },
    {  MCMD29_CLR_WRITE_PROT,       MTYPE_AC,   MRSP_R1B, MCLASS(6) },
    {  MCMD30_SEND_WRITE_PROT,      MTYPE_ADTC, MRSP_R1,  MCLASS(6) },
    {  MCMD31_SEND_WRITE_PROT_TYPE, MTYPE_ADTC, MRSP_R1,  MCLASS(6) },
    /* CLASS 5 */
    {  MCMD35_ERASE_GROUP_START,    MTYPE_AC,   MRSP_R1,  MCLASS(5) },
    {  MCMD36_ERASE_GROUP_END,      MTYPE_AC,   MRSP_R1,  MCLASS(5) },
    {  MCMD38_ERASE,                MTYPE_AC,   MRSP_R1B, MCLASS(5) },
    /* CLASS 9 */
    {  MCMD39_FAST_IO,              MTYPE_AC,   MRSP_R4,  MCLASS(9) },
    {  MCMD40_GO_IRQ_STATE,         MTYPE_BCR,  MRSP_R5,  MCLASS(9) },
    /* CLASS 7 */
    {  MCMD42_LOCK_UNLOCK,          MTYPE_ADTC, MRSP_R1B, MCLASS(7) },
    /* CLASS 8 */
    {  MCMD55_APP_CMD,              MTYPE_AC,   MRSP_R1,  MCLASS(8) },
    {  MCMD56_GEN_CMD,              MTYPE_ADTC, MRSP_R1,  MCLASS(8) },
    /* CLASS 10 */
    {  MCMD53_PROTOCOL_RD,          MTYPE_ADTC, MRSP_R1,  MCLASS(10)},
    {  MCMD54_PROTOCOL_WR,          MTYPE_ADTC, MRSP_R1,  MCLASS(10)},
};

void mmcore_init(void)
{
    // setting here
    //RVAL(CR_SRAM_CTL) = 0x001F0060;
    writel(0x001F0060, RRVAL(CR_SRAM_CTL));
    //RVALB(CARD_STOP) = 0x4;
    writeb(0x4,RRVAL(CARD_STOP));
    //RVAL(CR_CKGEN_CTL) = 0x00002013;
    writel(0x00002013, RRVAL(CR_CKGEN_CTL));
    //RVALB(CARD_SELECT) = 0x2;
    writeb(0x2, RRVAL(CARD_SELECT));
    //RVALB(CARD_OE)     = 0x4;
    writeb(0x4, RRVAL(CARD_OE));

    // work around for DMA hang? RVALB(SD_TRANSFER) = 0
    //RVAL(SD_TRANSFER)  = 0x0;
    writel(0x0, RRVAL(SD_TRANSFER));

    // reset FIFO

}

void mmcore_set_mode(uint32_t mode)
{
    RMOD(SD_CONFIGURE1, mode_sel, mode);
}

void mmcore_set_hs_mode(uint32_t hsmode)
{
    if (hsmode > MMCORE_HSMODE_NORMAL) {
        // from DV
        if (MMCORE_HSMODE_HS400 == hsmode) {
            //RVALB(SD_AUTO_RST_FIFO) = 0x30;
            writeb(0x30, RRVAL(SD_AUTO_RST_FIFO));
        } else if (MMCORE_HSMODE_HS400ES == hsmode) {
            //RVALB(SD_AUTO_RST_FIFO) = 0xE0;
            writeb(0xE0, RRVAL(SD_AUTO_RST_FIFO));
        } else {
            printf("WARN: unknown HS type (%d)\n", hsmode);
            return;
        }
        RMOD(SD_CONFIGURE1, hs400_mode_sel, 1);
        RMOD(DS_DELAY_CFG, delay, 0x6);
        //RMOD(SD_PUSH_POINT_CTL, pp_sel_dat, 0);
        //RMOD(SD30PLLR1, phsel1, 24, phsel0, 8);
    } else {
        RMOD(SD_CONFIGURE1, hs400_mode_sel, 0);
        RMOD(DS_DELAY_CFG, delay, 0x0);
        //RVALB(SD_AUTO_RST_FIFO) = 0x0;
        //RMOD(SD30PLLR1, phsel1, 0, phsel0, 2);
    }
}

void mmcore_set_bits(uint32_t bits)
{
    uint32_t tmp_bits;

    tmp_bits = bits >> 2;
    RMOD(SD_CONFIGURE1, bus_width, tmp_bits);

}

void mmcore_set_div(uint32_t freq)
{
    //printf("%s\n", __func__);
}

static int mmcore_wait_opt(void)
{
    int time_out_count = 10000;
    while (0==RFLD(SD_TRANSFER, end)) {
        //printf("Check XFER status %x\n", RFLD(SD_TRANSFER, end));
        // work around for DMA hang? RVALB(SD_TRANSFER) = 0
        if(!time_out_count){
             printf("ERR: mmcore_wait_opt SD_TRANSFER time out!(0x%X)\n", readl(RRVAL(SD_TRANSFER)));
             break;
        }
        time_out_count--;
    }

    // check DMA error
    if (1==RFLD(SD_TRANSFER, err)) {
        printf("EE: XFER ERROR!!!\n");
        return -MMCORE_ERR_XFER_FAIL;
    }
    return 0;
}

static void mmcore_start_xfer(uint32_t cmdcode)
{
    int time_out_count = 10000;
    // wait idle
    while (0==RFLD(SD_TRANSFER, status)){
        if(!time_out_count){
            printf("ERR: mmcore_start_xfer SD_TRANSFER time out!(0x%X)\n", readl(RRVAL(SD_TRANSFER)));
            return;

        }
        time_out_count--;
    }
    RMOD(SD_TRANSFER, cmd_code, cmdcode);
    RMOD(SD_TRANSFER, start, 1);
}

static void mmcore_read_rsp(uint8_t *rsp, uint8_t *src, int len)
{
    uint32_t i, j;
    for (i=1, j=0; i<(len-1); i++, j++) {
        *(rsp+j) = *(src+i);
    }
}

int mmcore_fill_rsp_para(struct mmcore_cmd_info *cmd)
{
    uint32_t cnt = sizeof(mmc_rsp_type)/sizeof(struct mmcore_rsp_type);
    uint32_t i;
    for (i=0; i<cnt; i++) {
        if (cmd->idx == mmc_rsp_type[i].cmdidx)  {
            break;
        }
    }

    if (i==cnt) return -MMCORE_ERR_UNSUP_CMD;

    // SD_CONFIGURE1
    //SD_CONFIGURE1_T conf1 = {.v = RVALB(SD_CONFIGURE1)};
    SD_CONFIGURE1_T conf1;
    conf1.v = readb(RRVAL(SD_CONFIGURE1));
    if (mmc_rsp_type[i].cmdidx<=MCMD3_SET_RELATIVE_ADDR) {
        // init mode & clock divider, follow fOD, max 400KHz
        conf1.f.init_mode = 1;
        conf1.f.clock_divider = 1;
    } else {
        conf1.f.init_mode = 0;
        conf1.f.clock_divider = 0;
    }
    cmd->para.rsp_para1 = conf1.v;

    // SD_CONFIGURE2
    //SD_CONFIGURE2_T conf2 = {.v = RVALB(SD_CONFIGURE2)};
    SD_CONFIGURE2_T conf2;
    conf2.v = readb(RRVAL(SD_CONFIGURE2));
    if (mmc_rsp_type[i].resp == MRSP_R2) {
        conf2.f.resp_type_conf = 2;
    } else if (mmc_rsp_type[i].resp == MRSP_R0) {
        conf2.f.resp_type_conf = 0;
    } else {
        conf2.f.resp_type_conf = 1;
    }

    if (mmc_rsp_type[i].resp == MRSP_R0 ||
        mmc_rsp_type[i].resp == MRSP_R3) {
        conf2.f.crc7_check_en = 1;    // not check crc7
    } else {
        conf2.f.crc7_check_en = 0;
    }

    if (mmc_rsp_type[i].type == MTYPE_ADTC) {
        conf2.f.crc16_check_en = 0;
    } else {
        conf2.f.crc16_check_en = 1;   // not check crc16, no data xfer
    }

    if (mmc_rsp_type[i].resp == MRSP_R1B) {
        conf2.f.wait_busy_end_en = 1;
    } else {
        conf2.f.wait_busy_end_en = 0;
    }
    cmd->para.rsp_para2 = conf2.v;

    return 0;
}

int mmcore_send_cmd(struct mmcore_cmd_info *cmd,
                    struct mmcore_data_info *data)
{
    uint32_t err;
    uint32_t byte_cnt = MMCORE_DEF_BYTE_COUNT;
    uint32_t cmdcode = MMCORE_SENDCMDGETRSP;
    uint8_t rsp_17_addr[64] __attribute__ ((aligned (64)));
    uint32_t rsp_len = MMCORE_RSP_LEN_6;

    // retrieve response parameter for SD_CONFIGUREx
    err = mmcore_fill_rsp_para(cmd);
    if (err) {
        printf("EE: Unsupported Command %d\n", cmd->idx);
        goto done;
    }

#if 0
    printf("SD_CONF1 %x\n", cmd->para.rsp_para1);
    printf("SD_CONF2 %x\n", cmd->para.rsp_para2);
    printf("SD_CONF3 %x\n", cmd->para.rsp_para3);

    printf("SD_CMD0 %x\n", cmd->idx | 0x40);
    printf("SD_CMD1 %x\n", (cmd->arg>>24)&0xff);
    printf("SD_CMD2 %x\n", (cmd->arg>>16)&0xff);
    printf("SD_CMD3 %x\n", (cmd->arg>>8)&0xff);
    printf("SD_CMD4 %x\n", (cmd->arg&0xff));
    printf("SD_CMD5 %x\n", 0);
#endif

    // final setting
    //RVALB(SD_CONFIGURE1) = cmd->para.rsp_para1;
    writeb( cmd->para.rsp_para1, RRVAL(SD_CONFIGURE1));
    //RVALB(SD_CONFIGURE2) = cmd->para.rsp_para2;
    writeb( cmd->para.rsp_para2, RRVAL(SD_CONFIGURE2));
    //RVALB(SD_CONFIGURE3) = cmd->para.rsp_para3;
    writeb( cmd->para.rsp_para3, RRVAL(SD_CONFIGURE3));
    /* Byte mode is covered by ATF or UBOOT driver.
       driver will check ocr value and multiple bloc
       size when Byte mode enabled
       check the keyword
            ATF: OCR_HCS or UBOOT: OCR_HCS */

    //RVALB(SD_CMD0)       = cmd->idx | 0x40;
    writeb( cmd->idx | 0x40, RRVAL(SD_CMD0));
    //RVALB(SD_CMD1)       = (cmd->arg>>24)&0xff;
    writeb( (cmd->arg>>24)&0xff, RRVAL(SD_CMD1));
    //RVALB(SD_CMD2)       = (cmd->arg>>16)&0xff;
    writeb( (cmd->arg>>16)&0xff, RRVAL(SD_CMD2));
    //RVALB(SD_CMD3)       = (cmd->arg>>8)&0xff;
    writeb( (cmd->arg>>8)&0xff, RRVAL(SD_CMD3));
    //RVALB(SD_CMD4)       = (cmd->arg&0xff);
    writeb( (cmd->arg&0xff), RRVAL(SD_CMD4));
    //RVALB(SD_CMD5)       = 0;
    writeb( 0, RRVAL(SD_CMD5));

    SD_CONFIGURE2_T conf2 = { .v = cmd->para.rsp_para2 };

    if (MRSP_R2==conf2.f.resp_type_conf) {   // R2: length 136

        //RVALB(SD_BYTE_CNT_L)  = byte_cnt&0xFF;
        writeb( byte_cnt&0xFF, RRVAL(SD_BYTE_CNT_L));
        //RVALB(SD_BYTE_CNT_H)  = byte_cnt>>8;
        writeb( byte_cnt>>8, RRVAL(SD_BYTE_CNT_H));
        //RVALB(SD_BLOCK_CNT_L) = 1; // block count = 1
        writeb( 1, RRVAL(SD_BLOCK_CNT_L));
        //RVALB(SD_BLOCK_CNT_H) = 0;
        writeb( 0, RRVAL(SD_BLOCK_CNT_H));
        //RVAL(CR_DMA_CTL1)     = ((uint64_t)(rsp_17_addr)&0xFFFFFFFF)>>3;
        writel( ((uint64_t)(rsp_17_addr)&0xFFFFFFFF)>>3, RRVAL(CR_DMA_CTL1));
        //RVAL(CR_DMA_CTL2)     = 1;
        writel( 1, RRVAL(CR_DMA_CTL2));
        //RVAL(CR_DMA_CTL3)     = MMCORE_DMA_READ_RSP17; //read with rsp17_sel
        writel( MMCORE_DMA_READ_RSP17, RRVAL(CR_DMA_CTL3));
        rsp_len = MMCORE_RSP_LEN_17;

        DCACHE_FLUSH((uint64_t)rsp_17_addr, 64);
    } else if (data) { // with DMA data
        if ((uint64_t)data->buffer & 0x7) { // need 8 byte alignment
            err = MMCORE_ERR_UNALIGNED_DMA;
            goto done;
        }

        uint32_t dc3;
        if (data->dir == MMCORE_DATA_WRITE) {
            dc3 = MMCORE_DMA_WRITE; //write
            cmdcode = MMCORE_AUTOWRITE2;//SD_NORMALWRITE;
        } else {
            dc3 = MMCORE_DMA_READ; //read
            cmdcode = MMCORE_AUTOREAD2;//SD_NORMALREAD;
        }
#if 0
        printf("CR_DMA_CTL1 %llx\n", ((uint64_t)(data->buffer)&0xFFFFFFFF)>>3);
        printf("CR_DMA_CTL2 %x\n", data->block_count);
        printf("CR_DMA_CTL3 %x\n", dc3);
#endif
        //RVALB(SD_BYTE_CNT_L)  = byte_cnt&0xFF;
        writeb( byte_cnt&0xFF, RRVAL(SD_BYTE_CNT_L));
        //RVALB(SD_BYTE_CNT_H)  = byte_cnt>>8;
        writeb( byte_cnt>>8, RRVAL(SD_BYTE_CNT_H));
        //RVALB(SD_BLOCK_CNT_L) = data->block_count&0xFF;
        writeb( data->block_count&0xFF, RRVAL(SD_BLOCK_CNT_L));
        //RVALB(SD_BLOCK_CNT_H) = data->block_count>>8;
        writeb( data->block_count>>8, RRVAL(SD_BLOCK_CNT_H));
        //RVAL(CR_DMA_CTL1)     = ((uint64_t)(data->buffer))>>3;
        writel( ((uint64_t)(data->buffer))>>3, RRVAL(CR_DMA_CTL1));
        //RVAL(CR_DMA_CTL2)     = data->block_count;
        writel( data->block_count, RRVAL(CR_DMA_CTL2));
        //RVAL(CR_DMA_CTL3)     = dc3;
        writel( dc3, RRVAL(CR_DMA_CTL3));

        uint64_t size = data->block_count * MMCORE_DEF_BLK_SIZE;
        DCACHE_FLUSH((uint64_t)data->buffer, size);
    }

    mmcore_start_xfer(cmdcode);
    err = mmcore_wait_opt();

    if(0==err){
        if (MRSP_R2==conf2.f.resp_type_conf) {
            mmcore_read_rsp((uint8_t *)cmd->resp, rsp_17_addr, rsp_len);
        } else {
            mmcore_read_rsp((uint8_t *)cmd->resp, (uint8_t *)SD_CMD0ar, rsp_len);
        }
    }

done:
    return err;
}
