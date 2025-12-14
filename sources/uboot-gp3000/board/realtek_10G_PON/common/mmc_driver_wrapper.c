
#include <common.h>
#include <dm.h>
#include <mmc.h>
#include <sihnah.h>
#include <asm/io.h>
//#include <register_map.h>
//#include <linux/mtd/mtd.h>
#include <linux/delay.h>
//#include <driver/mmc_cr_v1/mmc.h>
//#include <driver/mmc_cr_v1/mmc_regs.h>
#include <mmc_cr_v1/mmc.h>
#include <mmc_cr_v1/mmc_regs.h>

/* 8277C */
#define SSCPLL_CTRL0  (0x0)
#define SSCPLL_CTRL1  (0x4)
  #define SSCLDO_EN (0x1 << 0)
  #define SSC_DIV_F_SEL (0x1 << 17)
#define SSCPLL_CTRL2  (0x8)
#define SSCPLL_CTRL3  (0xC)
  #define SSC_RSTB  (0x1 << 0)
  #define SSC_PLL_RSTB  (0x1 << 1)
  #define SSC_PLL_POW (0x1 << 2)
  #define CLK_EN_SD30 (0x1 << 8)
#define LDO_CTRL  (0x14)
  #define LDO_EN  (0x3 << 0)
  #define LDO_DIS (0x0 << 0)
  #define TUNE_VO (0x7 << 6)
  #define LDO_3P3V  (0x7 << 6)
  #define LDO_1P8V  (0x2 << 6)
  #define LDO_1P9V  (0x3 << 6)
#define BLK_RESET (0)
  #define RESET_SD  (0x1 << 28)
#define BLK_RESET_EXT (4)
  #define EN_SD   (0x1 << 14)
#define IO_PULL_CTRL  (0x5C)
#define IOMUX_MMC 10

//emmc wrapper (base = 0x18012000)
#define EMMC_SRAM_CTL               ( 0x0   )
#define EMMC_DMA_CTL1               ( 0x004 )
#define EMMC_DMA_CTL2               ( 0x008 )
#define EMMC_DMA_CTL3               ( 0x00C )
#define EMMC_SD_ISR                 ( 0x024 )
#define EMMC_SD_ISREN               ( 0x028 )
#define EMMC_PAD_CTL                ( 0x074 )
#define EMMC_CKGEN_CTL              ( 0x078 )
#define EMMC_CPU_ACC                ( 0x080 )
#define EMMC_IP_CARD_INT_EN         ( 0x120 )
#define EMMC_CARD_PAD_DRV           ( 0x130 )
#define EMMC_CMD_PAD_DRV            ( 0x131 )
#define EMMC_DATA_PAD_DRV           ( 0x132 )
#define EMMC_SRAM_BASE_0            ( 0x200 )
#define EMMC_SRAM_BASE_1            ( 0x300 )

//sd & emmc
#define CR_CARD_STOP                ( 0x103 )
#define CR_CARD_OE                  ( 0x104 )
#define CARD_SELECT                 ( 0x10e )
#define CARD_EXIST                  ( 0x11f )
#define CARD_INT_PEND             ( 0x121 )
#define CARD_CLOCK_EN_CTL           ( 0x129 )
#define CARD_SD_CLK_PAD_DRIVE       ( 0x130 )
#define CARD_SD_CMD_PAD_DRIVE       ( 0x131 )
#define CARD_SD_DAT_PAD_DRIVE       ( 0x132 )
#define SD_CONFIGURE1               ( 0x180 )
#define SD_CONFIGURE2               ( 0x181 )
#define SD_CONFIGURE3               ( 0x182 )
#define SD_STATUS1                  ( 0x183 )
#define SD_STATUS2                  ( 0x184 )
#define SD_BUS_STATUS               ( 0x185 )
#define SD_CMD_MODE                 ( 0x186 )
#define SD_SAMPLE_POINT_CTL         ( 0x187 )
#define SD_PUSH_POINT_CTL           ( 0x188 )
#define SD_CMD0                     ( 0x189 )
#define SD_CMD1                     ( 0x18A )
#define SD_CMD2                     ( 0x18B )
#define SD_CMD3                     ( 0x18C )
#define SD_CMD4                     ( 0x18D )
#define SD_CMD5                     ( 0x18E )
#define SD_BYTE_CNT_L               ( 0x18F )
#define SD_BYTE_CNT_H               ( 0x190 )
#define SD_BLOCK_CNT_L              ( 0x191 )
#define SD_BLOCK_CNT_H              ( 0x192 )
#define SD_TRANSFER                 ( 0x193 )
#define SD_DDR_DETECT_START         ( 0x194 )
#define SD_CMD_STATE                ( 0x195 )
#define SD_DATA_STATE               ( 0x196 )
#define SD_BUS_TA_STATE             ( 0x197 )
#define SD_STOP_SDCLK_CFG           ( 0x198 )
#define SD_AUTO_RST_FIFO            ( 0x199 )
#define SD_DAT_PAD                  ( 0x19A )
#define SD_DUMMY_4                  ( 0x19B )
#define SD_DUMMY_5                  ( 0x19C )
#define SD_DUTY_CTL                 ( 0x19D )
#define SD_SEQ_RW_CTL               ( 0x19E )
#define SD_CONFIGURE4               ( 0x19F )
#define SD_ADDR_L                   ( 0x1A0 )
#define SD_ADDR_H                   ( 0x1A1 )
#define SD_START_ADDR_0             ( 0x1A2 )
#define SD_START_ADDR_1             ( 0x1A3 )
#define SD_START_ADDR_2             ( 0x1A4 )
#define SD_START_ADDR_3             ( 0x1A5 )
#define SD_RSP_MASK_1               ( 0x1A6 )
#define SD_RSP_MASK_2               ( 0x1A7 )
#define SD_RSP_MASK_3               ( 0x1A8 )
#define SD_RSP_MASK_4               ( 0x1A9 )
#define SD_RSP_DATA_1               ( 0x1AA )
#define SD_RSP_DATA_2               ( 0x1AB )
#define SD_RSP_DATA_3               ( 0x1AC )
#define SD_RSP_DATA_4               ( 0x1AD )
#define SD_WRITE_DELAY              ( 0x1AE )
#define SD_READ_DELAY               ( 0x1AF )
#define DS_DELAY_CFG                ( 0x1B0 )


 /* SRAM_CTL */
 #define MAP_SEL     (0x1 << 5)

/* PAD_CTL */
#define PAD_3P3V    (0x1)
#define PAD_1P8V    (0x0)

/* CARD_STOP */
#define STOP_SD_MMC   (0x1 << 2)

/* CARD_SELECT */
#define SD_SEL      0x2

/* CARD_OE */
#define SD_MMC      0x4


static inline u32 rtl_reg_mask(void *addr, u32 mask, u32 new_val)
{
  u32 val;

  val = readl(addr);
  val &= ~mask;
  val |= new_val & mask;
  writel(val, addr);
  val = readl(addr);  /* flush write to the hardware */
  //printf("%s:%d => addr: %x , val: %x(%d)\n", __func__, __LINE__, addr, val,val);

  return val;
}


struct orin_mmc_plat {
    struct mmc_config cfg;
    struct mmc mmc;
};

struct orin_mmc_priv {
    uint32_t input_clk;        /* Input clock to MMC controller */
};

#define BOUNDARY_4GB    (0x100000000L)

static int orin_mmc_send_cmd(struct udevice *dev, struct mmc_cmd *cmd, struct mmc_data *data)
{
    //printf("%s:%d (%s)=> idx: %x , val: %x(%d)\n", __func__, __LINE__, __FILE__ ,cmd->cmdidx, cmd->cmdarg,cmd->cmdarg);
    // work around
    if ((8 == cmd->cmdidx && 0 != cmd->cmdarg) ||
        (55 == cmd->cmdidx && 0 == cmd->cmdarg)) {
        printf("DD: direct return failed\n");
        return -ETIMEDOUT; // for SD trial flow
    }

    struct orin_mmc_plat *plat = dev_get_platdata(dev);
    struct mmc *mmc = &plat->mmc;

    int ret = 0;
    struct mmcore_cmd_info mc_cmd;
    //struct mmcore_data_info mc_data;
    mc_cmd.idx = cmd->cmdidx;
    mc_cmd.arg = cmd->cmdarg;
    mc_cmd.resp = cmd->response;
    if (data) {
        uint8_t dir;
        struct mmcore_data_info mc_data;
        uint32_t blks, blk_len;
        uint8_t *buffer;

        if (data->flags == MMC_DATA_WRITE) {
            dir = MMCORE_DATA_WRITE;
            blk_len = mmc->write_bl_len;
        } else {
            dir = MMCORE_DATA_READ;
            blk_len = mmc->read_bl_len;
        }
        mc_data.dir = dir;

        // work around: checking boundary cross
        if ((uint64_t)data->dest < BOUNDARY_4GB &&
            ((uint64_t)data->dest+data->blocks*blk_len) > BOUNDARY_4GB)
        {
            if ((uint64_t)data->dest & (blk_len-1)) {
                printf("\nERR: cross boundary and non block size alignment! \n");
                return -EFAULT;
            }

            // process sectors ahead the boudary
            blks = (BOUNDARY_4GB-(uint64_t)data->dest)/blk_len;
            buffer = (uint8_t*)data->dest;
            mc_data.buffer = buffer;        // (uint8_t*)data->dest;
            mc_data.block_count = blks;
            //printf("processing 1st blocks=%d, buffer=%p, arg=%x\n", blks, buffer, mc_cmd.arg);
            ret = mmcore_send_cmd(&mc_cmd, &mc_data);
            if (ret!=0) return -EIO;

            // process cross boundary if non-alignment
            // buffer&copy&copy, hooray! ... alignment is best

            // process sectors behind the boundary
            buffer += blks*blk_len;
            /* need to handle the byte mode or sector mode here*/
            if (mmc->high_capacity){
                mc_cmd.arg += blks;
            } else {
                mc_cmd.arg += blks*blk_len;
            }
            blks = data->blocks - blks;
            //printf("processing 2nd blocks=%d, buffer=%p, arg=%x\n", blks, buffer, mc_cmd.arg);
        } else {
            buffer = (uint8_t*)data->dest;
            blks = data->blocks;
        }
        mc_data.buffer = buffer;
        mc_data.block_count = blks;

        ret = mmcore_send_cmd(&mc_cmd, &mc_data);
    } else {
        ret = mmcore_send_cmd(&mc_cmd, NULL);
    }

    if (ret!=0) return -EIO;

    cmd->response[0] = __CPU_TO_BE32(cmd->response[0]);
    cmd->response[1] = __CPU_TO_BE32(cmd->response[1]);
    cmd->response[2] = __CPU_TO_BE32(cmd->response[2]);
    cmd->response[3] = __CPU_TO_BE32(cmd->response[3]);
#if 0
     printf("SendCMDGetRSP_Cmd response (ret %d): %x %x %x %x\n", ret,
            cmd->response[0], cmd->response[1],
            cmd->response[2], cmd->response[3]);
#endif

    return ret;
}

static int orin_mmc_set_ios(struct udevice *dev)
{
    struct orin_mmc_plat *plat = dev_get_platdata(dev);
    struct mmc *mmc = &plat->mmc;
#if 0
    printf("%s, %d, (%s) clock %d, buswidth %d, ddr_mode %d \n", __func__, __LINE__,__FILE__,  mmc->clock, mmc->bus_width, mmc->ddr_mode);
#endif
    mmcore_set_div(mmc->clock);
    mmcore_set_bits(mmc->bus_width);
    if (mmc->ddr_mode) {
        mmcore_set_mode(MMCORE_MODE_DDR);
    } else {
        mmcore_set_mode(MMCORE_MODE_SD20);
    }
    return 0;
}

static int orin_mmc_getcd(struct udevice *dev)
{
    //printf("INFO: %s, %d\n", __func__, __LINE__);
    return 1;
}

static int orin_mmc_getwp(struct udevice *dev)
{
    //printf("INFO: %s, %d\n", __func__, __LINE__);
    return 0;
}

#define PLL_BASE  (0xf4320350)
#define BLK_RESET (0xF4320098)
#define BLK_BASE  (0xf432009c)
#define EMMC_BASE (0xf4400400)
static int orin_dm_mmc_init(struct udevice *dev)
{
    // setting here

    // release SD reset
    writel(readl(BLK_RESET)&((-1)^RESET_SD), BLK_RESET);
    /* Enable LDO and fix divided fractional */
    writel(readl(PLL_BASE + SSCPLL_CTRL1) | SSCLDO_EN | SSC_DIV_F_SEL,
            PLL_BASE + SSCPLL_CTRL1);


    /* Enable SSC PLL clk */
    writel(SSC_RSTB | SSC_PLL_RSTB | SSC_PLL_POW | CLK_EN_SD30,
            PLL_BASE + SSCPLL_CTRL3);

    /* Enable SD/EMMC IP */
    rtl_reg_mask(BLK_BASE, EN_SD, EN_SD);
    writel(0x019FF100, BLK_BASE + IO_PULL_CTRL);

    /* Set voltage to 1.9v */
    rtl_reg_mask(PLL_BASE + LDO_CTRL, TUNE_VO, LDO_1P9V);
    rtl_reg_mask(PLL_BASE + LDO_CTRL, LDO_EN, LDO_EN);

    writel(PAD_1P8V, EMMC_BASE + EMMC_PAD_CTL);

    rtl_reg_mask(EMMC_BASE + EMMC_SRAM_CTL, MAP_SEL, MAP_SEL);
    writeb(STOP_SD_MMC, EMMC_BASE + CR_CARD_STOP);
    writeb(SD_SEL, EMMC_BASE + CARD_SELECT);
    writeb(SD_MMC, EMMC_BASE + CR_CARD_OE);

#if 0
    writeb(0x66, 0xf4400400 + EMMC_CARD_PAD_DRV);
    //printf("%s:%d => addr: %p , val: %x(%d) <- 0x66\n", __func__, __LINE__, (void*)( 0xf4400400 + EMMC_CARD_PAD_DRV), readb( 0xf4400400 + EMMC_CARD_PAD_DRV),readb( 0xf4400400 + EMMC_CARD_PAD_DRV));
    writeb(0x64, 0xf4400400 + EMMC_CMD_PAD_DRV);
    //printf("%s:%d => addr: %p , val: %x(%d) <- 0x64\n", __func__, __LINE__, (void*)( 0xf4400400 + EMMC_CMD_PAD_DRV), readb( 0xf4400400 + EMMC_CMD_PAD_DRV),readb( 0xf4400400 + EMMC_CMD_PAD_DRV));
    writeb(0x66, 0xf4400400 + EMMC_DATA_PAD_DRV);
    //printf("%s:%d => addr: %p , val: %x(%d) <= 0x66\n", __func__, __LINE__, (void*)( 0xf4400400 + EMMC_DATA_PAD_DRV), readb( 0xf4400400 + EMMC_DATA_PAD_DRV), readb( 0xf4400400 + EMMC_DATA_PAD_DRV));
#endif
    mmcore_init();

    // reset FIFO
    return 0;
}


static int orin_mmc_of_to_plat(struct udevice *dev)
{

    struct orin_mmc_plat *plat = dev_get_platdata(dev);
    struct mmc_config *cfg = &plat->cfg;
    struct mmc *mmc = &plat->mmc;

    cfg->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_DDR_52MHz |
                      MMC_MODE_HS200 | //MMC_MODE_HS400 |
                      MMC_MODE_4BIT | MMC_MODE_8BIT;

    cfg->name = "RTK_MMC"; // FIXME
    cfg->voltages = 0xFF8080;//MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
    cfg->b_max = MMCORE_MAX_DMA_BLOCKS;
    cfg->f_max = 200;
    cfg->f_min = 0;

    mmc->version = MMC_VERSION_UNKNOWN;
    mmc->read_bl_len = MMCORE_MAX_BLK_SIZE;
    mmc->write_bl_len = MMCORE_MAX_BLK_SIZE;
#if 0
    mmc->erase_grp_size = MMCORE_MAX_BLK_SIZE;
#if CONFIG_IS_ENABLED(MMC_HW_PARTITIONING)
    mmc->hc_wp_grp_size = MMCORE_MAX_BLK_SIZE;	/* in 512-byte sectors */
#endif
#endif
    //mmc->bus_width = 1;
    mmc->clock = 25;
    //mmc->has_init = 0;
    return 0;
}

static int orin_mmc_probe(struct udevice *dev)
{

    struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
    struct orin_mmc_plat *plat = dev_get_platdata(dev);
    //struct orin_mmc_priv *priv = dev_get_priv(dev);

    //priv->reg_base = plat->reg_base;
    //priv->input_clk = clk_get(orin_MMCSD_CLKID);
    upriv->mmc = &plat->mmc;

    return orin_dm_mmc_init(dev);
}

#if CONFIG_BLK
static int orin_mmc_bind(struct udevice *dev)
{
    struct orin_mmc_plat *plat = dev_get_platdata(dev);

    return mmc_bind(dev, &plat->mmc, &plat->cfg);
}
#endif

#ifdef MMC_SUPPORTS_TUNING
static int orin_mmc_execute_tuning(struct udevice *dev, uint32_t opcode)
{
    struct orin_mmc_plat *plat = dev_get_platdata(dev);
    struct mmc *mmc = &plat->mmc;
    uint32_t mode = mmc->selected_mode;
    uint32_t hsmode = MMCORE_HSMODE_NORMAL;
    if (MMC_HS_400 == mode) {
        hsmode = MMCORE_HSMODE_HS400;
    } else if (MMC_HS_400_ES == mode) {
        hsmode = MMCORE_HSMODE_HS400ES;
    }
    //printf("\n%s, %d mode %d, hs %d\n", __func__, __LINE__, mode, hsmode);
    mmcore_set_hs_mode(hsmode);
    // sending tuning block command
    // while(xxx) {
    //      mmc_send_tuning(mmc, opcode, NULL);
    // }
    return 0;
}
#endif


static const struct dm_mmc_ops rtk_mmc_ops = {
    //.deferred_probe =
    //.reinit =
#ifdef MMC_SUPPORTS_TUNING
    .execute_tuning = orin_mmc_execute_tuning,
#endif
    .send_cmd   = orin_mmc_send_cmd,
    .set_ios    = orin_mmc_set_ios,
    .get_cd     = orin_mmc_getcd,
    .get_wp     = orin_mmc_getwp,
};


static const struct udevice_id rtk_mmc_ids[] = {
    { .compatible = "rtk,mmc" },
    {},
};


U_BOOT_DRIVER(orin_sd_mmc) = {
        .name = "rtk_mmc",
        .id = UCLASS_MMC,
        .of_match = rtk_mmc_ids,
        .ops = &rtk_mmc_ops,
        .ofdata_to_platdata = orin_mmc_of_to_plat,
        .platdata_auto_alloc_size = sizeof(struct orin_mmc_plat),
        .priv_auto_alloc_size	= sizeof(struct orin_mmc_priv),
        .probe = orin_mmc_probe,
#if CONFIG_BLK
        .bind = orin_mmc_bind,
#endif
};
