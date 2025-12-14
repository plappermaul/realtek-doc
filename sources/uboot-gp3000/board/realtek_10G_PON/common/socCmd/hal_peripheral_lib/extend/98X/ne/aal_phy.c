
#include <generated/ca_ne_autoconf.h>
#include "ca_types.h"
#include "osal_common.h"
#include "aal_common.h"
#include "aal_phy.h"
#include "aal_ni_l2.h"

#if 1//sd1 uboot for 98f
//#include <linux/delay.h> /* for mdelay() */
#include <common.h>     /* for mdelay() */

#define ca_udelay(usecs)      udelay(usecs)
#define ca_mdelay(msecs)      mdelay(msecs)
#endif

#define __MDIO_CLOCK_MIN     (1)
#define __MDIO_CLOCK_MAX     (20000)
#define __MDIO_CLOCK_DEF     (2000)

#define __PER_REG_READ(dev,addr)         CA_PER_MDIO_REG_READ(addr)
#define __PER_REG_WRITE(dev,addr,data)   CA_PER_MDIO_REG_WRITE(data, addr)

#define __DELAY_US(usecs)      ca_udelay(usecs)
#define __DELAY_MS(msecs)      ca_mdelay(msecs)

/* for 98f fe-phy (+) */
#define __FE_AUTO_NEG_MAX_CNT   (300)
#define __FE_AUTO_NEG_MDLAY   (10)

#define __FE_FM_MAX_CNT   (400)
#define __FE_FM_MDLAY   (10)

#define __FE_RST_MDLAY   (200)
/* for 98f fe-phy (-) */

/* for 8211f ge nic (+) */
#define __GE_AUTO_NEG_MAX_CNT   (300)
#define __GE_AUTO_NEG_MDLAY   (10)

#define __GE_FM_MAX_CNT   (400)
#define __GE_FM_MDLAY   (10)

#define __GE_RST_MDLAY   (400)
#define REG(reg)  (*(volatile unsigned int *)(reg))
/* for 8211f ge nic (-) */

#ifndef CONFIG_CORTINA_BOARD_FPGA
/*
Init from ASIC:
#phy address 1
mw.l f43380fc 0bc6
#md.l f43380c0 8
mw.l f43380c0 0053
mw.l f43380c8 4003
mw.l f43380d8 7e01
#md.l f43380c0 8
mw.l f43380fc 0a42
#md.l f43380c0 8
mw.l f43380fc 0a40
#md.l f4338080 31
mw.l f4338080 1140
#md.l f4338080 31
#
#Phy address 2
mw.l f433817c 0bc6
#md.l f4338140 8
mw.l f4338140 0053
mw.l f4338148 4003
mw.l f4338158 7e01
#md.l f4338140 8
mw.l f433817c 0a42
#md.l f4338140 8
mw.l f433817c 0a40
#md.l f4338100 31
mw.l f4338100 1140
#md.l f4338100 31
#
#Phy address 3
mw.l f43381fc 0bc6
#md.l f43381c0 8
mw.l f43381c0 0053
mw.l f43381c8 4003
mw.l f43381d8 7e01
#md.l f43381c0 8
mw.l f43381fc 0a42
#md.l f43381c0 8
mw.l f43381fc 0a40
#md.l f4338180 31
mw.l f4338180 1140
#md.l f4338180 31
#
#Phy Addres 4
mw.l f433827c 0bc6
#md.l f4338240 8
mw.l f4338240 0053
mw.l f4338248 4003
mw.l f4338258 7e01
#md.l f4338240 8
mw.l f433827c 0a42
#md.l f4338240 8
mw.l f433827c 0a40
#md.l f4338200 31
mw.l f4338200 1140
#md.l f4338200 31
    Internally to our chip the MDIO interface of the GPHY is memory mapped into
    the chip register address space. As a result they will do a direct memory
    write for each of the writes that they put into that file.

    The mapping works like this:

    Basic GPHY offset for all memory reads/writes: 0xF433_8000

    The address for accessing a particular register inside a particular phy is:

    Address[31:12] = F4338
    Address[11:7] = PHYADDR (could be 0x1, 0x2, 0x3, 0x4 for our design)
    Address[6:2] = REGADDR
    Addresss[1:0] = 00
    ---------------------------------

*/

#define ASIC_PHY_REG_BASE                0xF4338000

#define ASIC_PHY_MDIO_READ(phy,reg)      \
    CA_NE_PHY_REG_READ(ASIC_PHY_REG_BASE+(((phy)&0x1F)<<7)+(((reg)&0x1F)<<2))
#define ASIC_PHY_MDIO_WRITE(phy,reg,data) \
    CA_NE_PHY_REG_WRITE(data,ASIC_PHY_REG_BASE+(((phy)&0x1F)<<7)+(((reg)&0x1F)<<2))

#endif /* CONFIG_CORTINA_BOARD_FPGA */

/**************************************************************/
/*           MDIO Access implementation definition            */
/**************************************************************/

typedef struct {
    ca_uint aal_mdio_lock;
    ca_uint32_t init_flag;
} aal_mdio_drv_t;

static aal_mdio_drv_t g_mdio_drv;
ca_status_t aal_mdio_global_init(ca_device_id_t device_id)
{
    if(g_mdio_drv.init_flag==1)
            return CA_E_OK;
    memset(&g_mdio_drv, 0, sizeof(g_mdio_drv));

#if 0//yocto
    ca_spin_lock_init(&g_mdio_drv.aal_mdio_lock, SPINLOCK_BH);
#else//sd1 uboot for 98f
#endif

    g_mdio_drv.init_flag = 1;

    return CA_E_OK;
}
EXPORT_SYMBOL(aal_mdio_global_init);

ca_status_t aal_mdio_global_exit(ca_device_id_t device_id)
{
    if (g_mdio_drv.aal_mdio_lock)
        ca_spin_lock_destroy(g_mdio_drv.aal_mdio_lock);
    g_mdio_drv.aal_mdio_lock = 0;
    g_mdio_drv.init_flag = 0;
    return CA_E_OK;
}
EXPORT_SYMBOL(aal_mdio_global_exit);

#if 0//yocto
#define AAL_MDIO_LOCK(device_id)        {aal_mdio_global_init(device_id);ca_spin_lock(g_mdio_drv.aal_mdio_lock);}
#define AAL_MDIO_UNLOCK(device_id)      ca_spin_unlock(g_mdio_drv.aal_mdio_lock)
#else//sd1 uboot for 98f
#endif

ca_status_t aal_mdio_speed_set (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint32_t  addr,
    CA_IN  ca_uint32_t  clock)

{
    ca_uint16_t    prer = 0;
    PER_MDIO_CFG_t cfg;

    if (clock < __MDIO_CLOCK_MIN ||
        clock > __MDIO_CLOCK_MAX ) {
        return CA_E_PARAM;
    }

#if 0//yocto
    AAL_MDIO_LOCK(device_id);
#else//sd1 uboot for 98f
#endif

    cfg.wrd = __PER_REG_READ(device_id, PER_MDIO_CFG);

    /* Caculate pre scale for MDIO clock adjust */
    prer = ( CA_AAL_MDIO_PER_CLK / (clock << 1)) - 1;
    if (( CA_AAL_MDIO_PER_CLK % (clock << 1)) > clock) {
        prer += 1;
    }

    cfg.bf.mdio_pre_scale = prer;
    cfg.bf.mdio_manual    = 1;

    __PER_REG_WRITE(device_id, PER_MDIO_CFG, cfg.wrd);

#if 0//yocto
    AAL_MDIO_UNLOCK(device_id);
#else//sd1 uboot for 98f
#endif

    return CA_E_OK;

}


ca_status_t aal_mdio_speed_get (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_uint32_t         addr,
    CA_OUT      ca_uint32_t        *clock)
{
    PER_MDIO_CFG_t cfg;

    CA_ASSERT_RET(clock, CA_E_PARAM);

    AAL_MDIO_LOCK(device_id);

    cfg.wrd = __PER_REG_READ(device_id, PER_MDIO_CFG);

    *clock = CA_AAL_MDIO_PER_CLK / (cfg.bf.mdio_pre_scale + 1) >> 1;

    AAL_MDIO_UNLOCK(device_id);

    return CA_E_OK;

}

static ca_status_t aal_mdio_write_direct(
   CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_addr,
    CA_IN      ca_uint16_t              data)
{
    PER_MDIO_ADDR_t  mdio_addr;
    PER_MDIO_CTRL_t  mdio_ctrl;
    ca_uint32_t      loop_wait = CA_AAL_MDIO_ACCESS_TIMEOUT;

    mdio_addr.wrd            = 0;
    mdio_addr.bf.mdio_addr   = phy_port_addr;
    mdio_addr.bf.mdio_op     = 1;  /* type of indirect access */
    mdio_addr.bf.mdio_offset = reg_addr;                  /* offset */
    mdio_addr.bf.mdio_rd_wr  = CA_AAL_MDIO_WR_FLAG;
    mdio_addr.bf.mdio_st     = 0;  /* indirect/direct access */

    __PER_REG_WRITE(device_id, PER_MDIO_ADDR, mdio_addr.wrd);
    __PER_REG_WRITE(device_id, PER_MDIO_WRDATA, data);

    mdio_ctrl.wrd          = 0;
    mdio_ctrl.bf.mdiostart = 1;
    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);

    do {
        mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
        if (mdio_ctrl.bf.mdiodone) {
            /* Clear MDIO done */
             mdio_ctrl.bf.mdiodone = 1;
            __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);
            return CA_E_OK;
        }
    } while(--loop_wait);
    return CA_E_TIMEOUT;
}

ca_status_t aal_mdio_write_indirect(
   CA_IN       ca_device_id_t             device_id,
   CA_IN       ca_uint8_t             phy_port_addr,
   CA_IN       ca_uint8_t             dev_addr,
   CA_IN       ca_uint16_t             addr,
   CA_IN       ca_uint16_t             data)
{
    PER_MDIO_ADDR_t  mdio_addr;
    PER_MDIO_CTRL_t  mdio_ctrl;
    ca_uint32_t      loop_wait = CA_AAL_MDIO_ACCESS_TIMEOUT;
    ca_uint32_t      tmp_data;

    mdio_addr.wrd            = 0;
    mdio_addr.bf.mdio_addr   = phy_port_addr;
    mdio_addr.bf.mdio_op     = 0;                       /* address write */
    mdio_addr.bf.mdio_offset = dev_addr;                  /* region area */
    mdio_addr.bf.mdio_rd_wr  = CA_AAL_MDIO_WR_FLAG;
    mdio_addr.bf.mdio_st     = 1;                       /* indirect access */
    __PER_REG_WRITE(device_id, PER_MDIO_ADDR, mdio_addr.wrd);
    __PER_REG_WRITE(device_id, PER_MDIO_WRDATA, addr);

    tmp_data = __PER_REG_READ(device_id, PER_MDIO_ADDR);
    printf("%s: PER_MDIO_ADDR=0x%x\n", __func__, tmp_data);

    mdio_ctrl.wrd          = 0;
    mdio_ctrl.bf.mdiostart = 1;
    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);

    do {
            mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
            if (mdio_ctrl.bf.mdiodone) {
                    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);
                    break;
            }
            mdelay(1);
    } while(--loop_wait);
    if (loop_wait == 0) {
            printf("%s: indirect write address timeout!!\n", __func__);
    }

    mdio_addr.wrd            = 0;
    mdio_addr.bf.mdio_addr   = phy_port_addr;
    mdio_addr.bf.mdio_offset = dev_addr;
    mdio_addr.bf.mdio_op     = 1;           /* indriect WRITE */
    mdio_addr.bf.mdio_st     = 1;	    /* indirect access */

    mdio_addr.bf.mdio_rd_wr  = CA_AAL_MDIO_WR_FLAG;
    __PER_REG_WRITE(device_id, PER_MDIO_ADDR, mdio_addr.wrd);
    __PER_REG_WRITE(device_id, PER_MDIO_WRDATA, data);

    mdio_ctrl.wrd          = 0;
    mdio_ctrl.bf.mdiostart = 1;
    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);

    do {
        mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
        if (mdio_ctrl.bf.mdiodone) {
            /* Clear MDIO done */
             mdio_ctrl.bf.mdiodone = 1;
            __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);
            return CA_E_OK;
        }
    } while(--loop_wait);
        return CA_E_TIMEOUT;
}
EXPORT_SYMBOL(aal_mdio_write_indirect);
/*Because of the ASIC_PHY_MDIO_WRITE, phy_port_addr and reg_dev_addr should be ca_uint32_t*/
ca_status_t aal_mdio_write (
    CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              st_code,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_dev_addr,
    CA_IN      ca_uint16_t              addr,
    CA_IN      ca_uint16_t              data)
{
    ca_status_t ret = 0;

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    if (phy_port_addr > 1 && phy_port_addr < 5) {
        return CA_E_NOT_FOUND;
    }
#endif

#if 0//yocto
    AAL_MDIO_LOCK(device_id);
#else//sd1 uboot for 98f
#endif

#ifdef CONFIG_CORTINA_BOARD_FPGA
    ret = aal_mdio_write_direct(device_id, phy_port_addr, reg_dev_addr, data);
#else

    if (phy_port_addr >= 5) {
        /*1 for IEEE 802.3 clause 22.2.4.5.3 (direct access),
          0 for IEEE 802.3 clause 45.3.3  (indirect access) */
        if (st_code == 0){
            ret = aal_mdio_write_indirect(device_id, phy_port_addr, reg_dev_addr, addr, data);
        }
        else{
            ret = aal_mdio_write_direct(device_id, phy_port_addr, reg_dev_addr, data);
        }

#if 0//yocto
        AAL_MDIO_UNLOCK(device_id);
#else//sd1 uboot for 98f
#endif

        return ret;
    }

    ASIC_PHY_MDIO_WRITE(phy_port_addr, reg_dev_addr, data);
#endif

#if 0//yocto
    AAL_MDIO_UNLOCK(device_id);
#else//sd1 uboot for 98f
#endif

    return ret;
}
EXPORT_SYMBOL(aal_mdio_write);

#if 0//yocto
#else//sd1 uboot for 98f - davidlu need cmd for phy access

#define	CA_PER_MDIO_PAGE_MAX	0xffff
#define	PER_MDIO_DEFAULT_PAGE	0x0
#define	PER_MDIO_PAGE_SEL_REG	0x1f

#define	__PER_MDIO_PAGE_CHK2(page, max) do{ \
	if( (max < page)) { \
		printf("%s: 'page' is out of range!!! (0x0 ~ 0x%x)\n", \
			__func__, \
			max); \
		return CA_E_PARAM; \
	} \
} while(0)

void fe_page_read(
    CA_IN	ca_uint16_t	page,
    CA_IN	ca_uint32_t	phy_ad,
    CA_IN	ca_uint32_t	reg_ad,
    CA_OUT	ca_uint16_t	*data)
{
    /* support PER_MDIO_PAGE range: 0~65535,
        see comments at MAX_PER_MDIO_PAGE's definition */
    __PER_MDIO_PAGE_CHK2(page, CA_PER_MDIO_PAGE_MAX);

    memset(data, 0, sizeof(*data));

    /* write page[15:0] to reg.0x31 to select page */
    ASIC_PHY_MDIO_WRITE(phy_ad, PER_MDIO_PAGE_SEL_REG, page);

    /* read data[15:0] from reg_ad */
    *data = ASIC_PHY_MDIO_READ(phy_ad, reg_ad);

    /* move back to page[15:0]=0x0, after above action has been done */
    ASIC_PHY_MDIO_WRITE(phy_ad, PER_MDIO_PAGE_SEL_REG, PER_MDIO_DEFAULT_PAGE);
}

void fe_page_write(
    CA_IN	ca_uint16_t	page,
    CA_IN	ca_uint32_t	phy_ad,
    CA_IN	ca_uint32_t	reg_ad,
    CA_IN	ca_uint16_t	data)
{
    /* support PER_MDIO_PAGE range: 0~65535,
        see comments at MAX_PER_MDIO_PAGE's definition */
    __PER_MDIO_PAGE_CHK2(page, CA_PER_MDIO_PAGE_MAX);

    /* write page[15:0] to reg.0x31 to select page */
    ASIC_PHY_MDIO_WRITE(phy_ad, PER_MDIO_PAGE_SEL_REG, page);

    /* write data[15:0] to config reg_ad */
    ASIC_PHY_MDIO_WRITE(phy_ad, reg_ad, data);

    /* move back to page[15:0]=0x0, after above action has been done */
    ASIC_PHY_MDIO_WRITE(phy_ad, PER_MDIO_PAGE_SEL_REG, PER_MDIO_DEFAULT_PAGE);

#if 0
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") with " \
        LIGHT_BLUE  "data: %#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, data);
#endif
}

extern ca_status_t ca_per_mdio_write (
    CA_IN ca_uint32_t addr,
    CA_IN ca_uint32_t offset,
    CA_IN ca_uint16_t data);

extern ca_status_t ca_per_mdio_read (
    CA_IN ca_uint32_t addr,
    CA_IN ca_uint32_t offset,
    CA_OUT ca_uint16_t *data);

void rt_mdio_read(
/*    CA_IN	ca_uint16_t page, */
    CA_IN	ca_uint32_t phy_ad,
    CA_IN	ca_uint32_t reg_ad,
    CA_OUT ca_uint16_t *data)
{
    memset(data, 0x0, sizeof(ca_uint16_t));

    /* read data[15:0] from reg_ad */
    ca_per_mdio_read(phy_ad, reg_ad, data);
}

void mdio_page_read(
    CA_IN	ca_uint16_t page,
    CA_IN	ca_uint32_t phy_ad,
    CA_IN	ca_uint32_t reg_ad,
    CA_OUT ca_uint16_t *data)
{
    /* support PER_MDIO_PAGE range: 0~65535,
        see comments at MAX_PER_MDIO_PAGE's definition */
    __PER_MDIO_PAGE_CHK2(page, CA_PER_MDIO_PAGE_MAX);

    memset(data, 0x0, sizeof(ca_uint16_t));

    /* write page[15:0] to reg.0x31 to select page */
    ca_per_mdio_write(phy_ad, PER_MDIO_PAGE_SEL_REG, page);

    /* read data[15:0] from reg_ad */
    ca_per_mdio_read(phy_ad, reg_ad, data);

    /* move back to page[15:0]=0x0, after above action has been done */
    ca_per_mdio_write(phy_ad, PER_MDIO_PAGE_SEL_REG, PER_MDIO_DEFAULT_PAGE);

#if 0
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") = " \
        LIGHT_BLUE  "%#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, *data);
#endif
}

void rt_mdio_write(
/*    CA_IN	ca_uint16_t page,*/
    CA_IN	ca_uint32_t phy_ad,
    CA_IN	ca_uint32_t reg_ad,
    CA_IN	ca_uint16_t data)
{
    /* write data[15:0] to config reg_ad */
    ca_per_mdio_write(phy_ad, reg_ad, data);
}

void mdio_page_write(
    CA_IN	ca_uint16_t page,
    CA_IN	ca_uint32_t phy_ad,
    CA_IN	ca_uint32_t reg_ad,
    CA_IN	ca_uint16_t data)
{
    /* support PER_MDIO_PAGE range: 0~65535,
        see comments at MAX_PER_MDIO_PAGE's definition */
    __PER_MDIO_PAGE_CHK2(page, CA_PER_MDIO_PAGE_MAX);

    /* write page[15:0] to reg.0x31 to select page */
    ca_per_mdio_write(phy_ad, PER_MDIO_PAGE_SEL_REG, page);

    /* write data[15:0] to config reg_ad */
    ca_per_mdio_write(phy_ad, reg_ad, data);

    /* move back to page[15:0]=0x0, after above action has been done */
    ca_per_mdio_write(phy_ad, PER_MDIO_PAGE_SEL_REG, PER_MDIO_DEFAULT_PAGE);

#if 0
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") with " \
        LIGHT_BLUE  "data: %#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, data);
#endif
}

#endif//sd1 uboot for 98f - davidlu need cmd for phy access

static ca_status_t aal_mdio_read_direct (
    CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_addr,
    CA_OUT      ca_uint16_t             *data)
{
    PER_MDIO_ADDR_t    mdio_addr;
    PER_MDIO_CTRL_t    mdio_ctrl;
    PER_MDIO_RDDATA_t  read_data;
    ca_uint32_t        loop_wait = CA_AAL_MDIO_ACCESS_TIMEOUT;


    CA_ASSERT_RET(data, CA_E_PARAM);

    mdio_addr.wrd            = 0;
    mdio_addr.bf.mdio_addr   = phy_port_addr;
    mdio_addr.bf.mdio_op     = 3;
    mdio_addr.bf.mdio_offset = reg_addr;
    mdio_addr.bf.mdio_st     = 0;
    mdio_addr.bf.mdio_rd_wr  = CA_AAL_MDIO_RD_FLAG;

#if 1//yocto
#else//sd1 uboot
    printf("\t%s(%d) %s ====(+)==== \n", __func__, __LINE__, __FILE__);
    printf("\tmdio_addr.bf.mdio_addr: %#x\n", mdio_addr.bf.mdio_addr);
    printf("\tmdio_addr.bf.mdio_op: %#x\n", mdio_addr.bf.mdio_op);
    printf("\tmdio_addr.bf.mdio_offset: %#x\n", mdio_addr.bf.mdio_offset);
    printf("\tmdio_addr.bf.mdio_rd_wr: %#x\n", mdio_addr.bf.mdio_rd_wr);
    printf("\tmdio_addr.bf.mdio_st: %#x\n", mdio_addr.bf.mdio_st);
    printf("\t%s(%d) %s ====(-)==== \n", __func__, __LINE__, __FILE__);
#endif

    __PER_REG_WRITE(device_id, PER_MDIO_ADDR, mdio_addr.wrd);

    mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
    mdio_ctrl.bf.mdiostart = 1;
    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);

    do {
        mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
        if (mdio_ctrl.bf.mdiodone) {
            /* Read MDIO data */
            read_data.wrd = __PER_REG_READ(device_id, PER_MDIO_RDDATA);
            *data = read_data.bf.mdio_rddata;
            /* Clear MDIO done */
             mdio_ctrl.bf.mdiodone = 1;
            __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);
            return CA_E_OK;
        }
    } while(--loop_wait);
    return CA_E_TIMEOUT;
}

ca_status_t aal_mdio_read_indirect (
    CA_IN       ca_device_id_t             device_id,
    CA_IN       ca_uint8_t             phy_port_addr,
    CA_IN       ca_uint8_t             dev_type,
    CA_IN       ca_uint16_t             addr,
    CA_OUT      ca_uint16_t             *data)
{
    PER_MDIO_ADDR_t    mdio_addr;
    PER_MDIO_CTRL_t    mdio_ctrl;
    PER_MDIO_RDDATA_t  read_data;
    ca_uint32_t        loop_wait = CA_AAL_MDIO_ACCESS_TIMEOUT;
    ca_uint32_t	       tmp_data;

    CA_ASSERT_RET(data, CA_E_PARAM);

    mdio_addr.wrd            = 0;
    mdio_addr.bf.mdio_addr   = phy_port_addr;
    mdio_addr.bf.mdio_op     = 0;			/* address write */
    mdio_addr.bf.mdio_offset = dev_type;                  /* region area */
    mdio_addr.bf.mdio_rd_wr  = CA_AAL_MDIO_WR_FLAG;
    mdio_addr.bf.mdio_st     = 1;			/* indirect access */

#if 0//yocto
#else//sd1 uboot
    printf("\t%s(%d) %s ====(+)==== \n", __func__, __LINE__, __FILE__);
    printf("\tPER_MDIO_CFG: %#x\n", CA_REG_READ(PER_MDIO_CFG) );
    printf("\tGLOBAL_PINMUX_CTRL_04: %#x\n", CA_REG_READ(GLOBAL_PINMUX_CTRL_04) );
    printf("\tGLOBAL_PINMUX_CTRL_05: %#x\n", CA_REG_READ(GLOBAL_PINMUX_CTRL_05) );
    printf("\tGLOBAL_PINMUX_CTRL_06: %#x\n", CA_REG_READ(GLOBAL_PINMUX_CTRL_06) );

    printf("\tmdio_addr.bf.mdio_addr: %#x\n", mdio_addr.bf.mdio_addr);
    printf("\tmdio_addr.bf.mdio_op: %#x\n", mdio_addr.bf.mdio_op);
    printf("\tmdio_addr.bf.mdio_offset: %#x\n", mdio_addr.bf.mdio_offset);
    printf("\tmdio_addr.bf.mdio_rd_wr: %#x\n", mdio_addr.bf.mdio_rd_wr);
    printf("\tmdio_addr.bf.mdio_st: %#x\n", mdio_addr.bf.mdio_st);
    printf("\t%s(%d) %s ====(-)==== \n", __func__, __LINE__, __FILE__);
#endif

    __PER_REG_WRITE(device_id, PER_MDIO_ADDR, mdio_addr.wrd);
    __PER_REG_WRITE(device_id, PER_MDIO_WRDATA, addr);

    tmp_data = __PER_REG_READ(device_id, PER_MDIO_ADDR);
    /*printf("%s: PER_MDIO_ADDR=0x%x\n", __func__, tmp_data);*/

    mdio_ctrl.wrd          = 0;
    mdio_ctrl.bf.mdiostart = 1;
    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);

    do {
	    mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
	    if (mdio_ctrl.bf.mdiodone) {
		    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);
		    break;
	    }
    } while(--loop_wait);
    if (loop_wait == 0) {
	    printf("%s: indirect write address timeout!!\n", __func__);
    }

    mdio_addr.wrd            = 0;
    mdio_addr.bf.mdio_addr   = phy_port_addr;
    mdio_addr.bf.mdio_op     = 3;		      /* indirect read */
    mdio_addr.bf.mdio_offset = dev_type;		      /* region area */
    mdio_addr.bf.mdio_st     = 1;		      /* indirect access */
    mdio_addr.bf.mdio_rd_wr  = CA_AAL_MDIO_RD_FLAG;
    __PER_REG_WRITE(device_id, PER_MDIO_ADDR, mdio_addr.wrd);

    mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
    mdio_ctrl.bf.mdiostart = 1;
    __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);

    do {
        mdio_ctrl.wrd = __PER_REG_READ(device_id, PER_MDIO_CTRL);
        if (mdio_ctrl.bf.mdiodone) {
            /* Read MDIO data */
            read_data.wrd = __PER_REG_READ(device_id, PER_MDIO_RDDATA);
            *data = read_data.bf.mdio_rddata;
            /* Clear MDIO done */
             mdio_ctrl.bf.mdiodone = 1;
            __PER_REG_WRITE(device_id, PER_MDIO_CTRL, mdio_ctrl.wrd);
            return CA_E_OK;
        }
    } while(--loop_wait);
        return CA_E_TIMEOUT;
}
EXPORT_SYMBOL(aal_mdio_read_indirect);

/*Because of the ASIC_PHY_MDIO_READ, phy_port_addr and reg_dev_addr should be ca_uint32_t*/
ca_status_t aal_mdio_read (
    CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              st_code,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_dev_addr,
    CA_IN      ca_uint16_t              addr,
    CA_OUT   ca_uint16_t             *data)
{
    ca_status_t ret = 0;


#ifdef CONFIG_CORTINA_BOARD_FPGA
    ret = aal_mdio_read_direct(device_id, phy_port_addr, reg_dev_addr, data);
#else

#if 1//yocto
    if (((phy_port_addr >= 5 || phy_port_addr == 0)) && (phy_port_addr != 8)) {
#else//sd1 uboot for 98f dbg
    if (((phy_port_addr >= 5 || phy_port_addr == 4)) && (phy_port_addr != 8)) {
#endif

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d): st_code: %#x, ", __func__, __LINE__, st_code);
        ca_printf("device_id: %#x, ", device_id);
        ca_printf("phy_port_addr: %#x, ", phy_port_addr);
        ca_printf("reg_dev_addr: %#x, ", reg_dev_addr);
        ca_printf("addr: %#x,", addr);
        ca_printf("*data: %#x\r\n", *data);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        /*1 for IEEE 802.3 clause 22.2.4.5.3 (direct access),
          0 for IEEE 802.3 clause 45.3.3  (indirect access) */
        if (st_code == 0){
            ret = aal_mdio_read_indirect(device_id, phy_port_addr, reg_dev_addr, addr, data);
        }
        else{
            ret = aal_mdio_read_direct(device_id, phy_port_addr, reg_dev_addr, data);
        }

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d): st_code: %#x, ", __func__, __LINE__, st_code);
        ca_printf("phy_port_addr: %#x, ", phy_port_addr);
        ca_printf("device_id: %#x, ", device_id);
        ca_printf("reg_dev_addr: %#x, ", reg_dev_addr);
        ca_printf("addr: %#x,", addr);
        ca_printf("*data: %#x\r\n", *data);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        return ret;
    }

    /* for 98f P0 ~ P4 */
    *data = ASIC_PHY_MDIO_READ(phy_port_addr, reg_dev_addr);

#endif

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d): st_code: %#x, ", __func__, __LINE__, st_code);
    ca_printf("device_id: %#x, ", device_id);
    ca_printf("phy_port_addr: %#x, ", phy_port_addr);
    ca_printf("reg_dev_addr: %#x, ", reg_dev_addr);
    ca_printf("addr: %#x,", addr);
    ca_printf("*data: %#x\r\n", *data);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return CA_E_OK;
}
EXPORT_SYMBOL(aal_mdio_read);


#define PHY_REG_MAX_LEN        (16)

/*Organizationally Unique Identifier (OUI) */
#define PHY_OUI_BCM5461                          (0x000818)
#define PHY_OUI_BCM54612                         (0x00d897)
#define PHY_OUI_RTK                              (0x000732)
#define PHY_OUI_VTS                              (0x0001c1)
#define PHY_OUI_AR                               (0x001374)

/*Manufacturer's Model Number  */
#define PHY_MODEL_BCM5461                        (0x000c)
#define PHY_MODEL_BCM54612                       (0x0026)
#define PHY_MODEL_RTL8211E                       (0x0010)
#define PHY_MODEL_RTL8211                        (0x0011)
#define PHY_MODEL_RTL8211_ASIC                   (0x0018)
#define PHY_MODEL_RTL8201F                       (0x0001)
#define PHY_MODEL_RTL8214                        (0x0026)
#define PHY_MODEL_RTL8214C                       (0x0014)
#define PHY_MODEL_VTS8641                        (0x0003)
#define PHY_MODEL_AR8305                         (0x0007)
#define PHY_MODEL_RTL8198F                       (0x0008)

/**************************************************************/
/*       Etherent PHY MII Standard Registers definition       */
/**************************************************************/

#define PHY_REG_CTRL                             (0)
#define PHY_REG_STATUS                           (1)
#define PHY_REG_ID_1                             (2)
#define PHY_REG_ID_2                             (3)
#define PHY_REG_AUTONEG_ADV                      (4)
#define PHY_REG_LP_ABILITY_BASE                  (5)
#define PHY_REG_AN_EXT                           (6)
#define PHY_REG_AUTONEG_NEXT_PAGE                (7)
#define PHY_REG_LP_NEXT_PAGE                     (8)
#define PHY_REG_1000BASET_CTRL                   (9)
#define PHY_REG_1000BASET_STATUS                 (10)
#define PHY_REG_RSVD1                            (11)
#define PHY_REG_RSVD2                            (12)
#define PHY_REG_RSVD3                            (13)
#define PHY_REG_RSVD4                            (14)
#define PHY_REG_1000BSET_STATUS_EXT              (15)
#define PHY_REG_SPECIFIC_STATUS                  (17)

#if 1//for 98FH MP
#define GE_NIC_8211EG_PHY_SR_PAGE        (0x0)
#define GE_NIC_8211EG_PHY_SR                 (0x11)
#endif

#if 1//for 98FH RD development
#define GE_NIC_8211F_PHY_SR_PAGE        (0xa43)
#define GE_NIC_8211F_PHY_SR                 (0x1a)
#endif

/* Control register bits */
#define PHY_REG_CTRL_BIT_SPEED_SEL_MSB           (6)
#define PHY_REG_CTRL_BIT_COLLISTION_TEST         (7)
#define PHY_REG_CTRL_BIT_DUPLEX                  (8)
#define PHY_REG_CTRL_BIT_AN_RESTART              (9)
#define PHY_REG_CTRL_BIT_ISOLATE                 (10)
#define PHY_REG_CTRL_BIT_PWR_DOWN                (11)
#define PHY_REG_CTRL_BIT_AN_CTRL                 (12)
#define PHY_REG_CTRL_BIT_SPEED_SEL_LSB           (13)
#define PHY_REG_CTRL_BIT_LPBK                    (14)
#define PHY_REG_CTRL_BIT_RESET                   (15)


/* Status register bits */
#define PHY_REG_STATUS_BIT_EXT_CAP               (0)
#define PHY_REG_STATUS_BIT_JBRD                  (1)
#define PHY_REG_STATUS_BIT_LNK                   (2)
#define PHY_REG_STATUS_BIT_AN_CAPABLE            (3)
#define PHY_REG_STATUS_BIT_RMT_FAULT             (4)
#define PHY_REG_STATUS_BIT_AN_DONE               (5)
#define PHY_REG_STATUS_BIT_EXT_STATUS            (8)
#define PHY_REG_STATUS_BIT_100BT2_HD_SUPP        (9)
#define PHY_REG_STATUS_BIT_100BT2_FD_SUPP        (10)
#define PHY_REG_STATUS_BIT_10_HD_SUPP            (11)
#define PHY_REG_STATUS_BIT_10_FD_SUPP            (12)
#define PHY_REG_STATUS_BIT_100BX_HD_SUPP         (13)
#define PHY_REG_STATUS_BIT_100BX_FD_SUPP         (14)
#define PHY_REG_STATUS_BIT_100BT4_SUPP           (15)

/* Advertisement control register bits */
#define PHY_REG_AN_ADVR_BIT_CSMA                 (0)
#define PHY_REG_AN_ADVR_BIT_10BT_HD              (5)
#define PHY_REG_AN_ADVR_BIT_10BT_FD              (6)
#define PHY_REG_AN_ADVR_BIT_100BX_HD             (7)
#define PHY_REG_AN_ADVR_BIT_100BX_FD             (8)
#define PHY_REG_AN_ADVR_BIT_100BT4_CAP           (9)
#define PHY_REG_AN_ADVR_BIT_PAUSE_CAP            (10)
#define PHY_REG_AN_ADVR_BIT_PAUSE_ASYM           (11)
#define PHY_REG_AN_ADVR_BIT_RMT_FAULT            (13)
#define PHY_REG_AN_ADVR_BIT_NPAGE                (15)

/* Link partner ability register bits */
#define PHY_REG_LP_ABLITY_BIT_10BT_HD            (5)
#define PHY_REG_LP_ABLITY_BIT_10BT_FD            (6)
#define PHY_REG_LP_ABLITY_BIT_100BX_HD           (7)
#define PHY_REG_LP_ABLITY_BIT_100BX_FD           (8)
#define PHY_REG_LP_ABLITY_BIT_100BT4_CAP         (9)
#define PHY_REG_LP_ABLITY_BIT_PAUSE_CAP          (10)
#define PHY_REG_LP_ABLITY_BIT_PAUSE_ASYM         (11)
#define PHY_REG_LP_ABLITY_BIT_RESV               (12)
#define PHY_REG_LP_ABLITY_BIT_RMT_FAULT          (13)
#define PHY_REG_LP_ABLITY_BIT_LP_ACK             (14)
#define PHY_REG_LP_ABLITY_BIT_NP                 (15)

/* Auto-negotiation Expansion register bits */
 #define PHY_REG_AN_EXT_BIT_LP_AN_CAP            (0)
 #define PHY_REG_AN_EXT_BIT_LP_NP_RX             (1)
 #define PHY_REG_AN_EXT_BIT_NP_CAP               (2)
 #define PHY_REG_AN_EXT_BIT_LP_NP_CAP            (3)
 #define PHY_REG_AN_EXT_BIT_P_FAULT              (4)

/* 1000BASE-T Control register bits */
#define PHY_REG_1000BT_CTRL_BIT_ADVR_1000_HD     (8)
#define PHY_REG_1000BT_CTRL_BIT_ADVR_1000_FD     (9)
#define PHY_REG_1000BT_CTRL_BIT_MASTER_CFG       (10)
#define PHY_REG_1000BT_CTRL_BIT_MMODE_CFG_ENBL   (11)

/* 1000BASE-T Status register bits */
#define PHY_REG_1000BT_STATUS_BIT_LP_1000_HD     (10)
#define PHY_REG_1000BT_STATUS_BIT_LP_1000_FD     (11)
#define PHY_REG_1000BT_STATUS_BIT_RMTRX_OK       (12)
#define PHY_REG_1000BT_STATUS_BIT_LCLRX_OK       (13)

/* Interrupts Definitions */
#define PHY_INTR_LINK_CHANGE           0x0001 /* Link Change Interrupt   */
#define PHY_INTR_SPEED_CHANGE          0x0002 /* Speed Change Interrupt  */
#define PHY_INTR_DUPLEX_CHANGE         0x0004 /* Duplex Change Interrupt */
#define PHY_INTR_AN_DONE               0x0008 /* AN Done Interrupt       */
#define PHY_INTR_AN_FAIL               0x0010 /* AN Fail Interrupt       */

#define ETH_PHY_NUM_MAX  32

/* Ethernet PHY driver object */
typedef struct {
    ca_status_t (*init)(ca_uint8_t);
    ca_status_t (*reset)(ca_uint8_t );
    ca_status_t (*auto_neg_restart)(ca_uint8_t );
    ca_status_t (*power_set)(ca_uint8_t, ca_boolean_t );
    ca_status_t (*power_get)(ca_uint8_t, ca_boolean_t*);
    ca_status_t (*auto_neg_set)(ca_uint8_t, ca_boolean_t );
    ca_status_t (*auto_neg_get)(ca_uint8_t, ca_boolean_t*);
    ca_status_t (*ability_adv_set)(ca_uint8_t, aal_phy_autoneg_adv_t );
    ca_status_t (*ability_adv_get)(ca_uint8_t, aal_phy_autoneg_adv_t*);
    ca_status_t (*partner_ability_adv_get)(ca_uint8_t, aal_phy_autoneg_adv_t*);
    ca_status_t (*link_status_get)(ca_uint8_t, aal_phy_link_status_t* );
    ca_status_t (*speed_set)(ca_uint8_t, aal_phy_speed_mode_t );
    ca_status_t (*speed_get)(ca_uint8_t, aal_phy_speed_mode_t*);
    ca_status_t (*duplex_set)(ca_uint8_t, ca_boolean_t );
    ca_status_t (*duplex_get)(ca_uint8_t, ca_boolean_t*);
    ca_status_t (*flow_ctrl_set)(ca_uint8_t, ca_boolean_t );
    ca_status_t (*flow_ctrl_get)(ca_uint8_t, ca_boolean_t*);
    ca_status_t (*mdix_set)(ca_uint8_t, aal_phy_mdi_mode_t );
    ca_status_t (*mdix_get)(ca_uint8_t, aal_phy_mdi_mode_t*);
    ca_status_t (*loopback_set)(ca_uint8_t, aal_phy_lpbk_mode_t);
    ca_status_t (*loopback_get)(ca_uint8_t, aal_phy_lpbk_mode_t*);
    ca_status_t (*int_mask_all)(ca_uint8_t);
    ca_status_t (*int_en_set)(ca_uint8_t, ca_uint16_t);
    ca_status_t (*int_en_get)(ca_uint8_t, ca_uint16_t*);
    ca_status_t (*int_status_get)(ca_uint8_t, ca_uint16_t*);
    ca_status_t (*int_status_clr)(ca_uint8_t);
}phy_drv_t;

typedef struct {
    char        name[32];
    ca_uint32_t oui;
    ca_uint32_t model;
    phy_drv_t   drv;
}phy_dev_t;

/**************************************************************/
/*       Internal Etherent PHY register access functions      */
/**************************************************************/
#if 0//yocto
#else//sd1 uboot for 98f MP test

static ca_status_t __fe_phy_regfield_read
(
    ca_uint8_t   phy_dev,
    ca_uint8_t   reg,
    ca_uint8_t   offset,
    ca_uint8_t   length,
    ca_uint16_t *p_val16
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint16_t data = 0;

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("%s(%d) phy_dev: %#x, reg: %#x, offset: %#x, length: %#x, p_val16: %#x\n", \
    __func__, __LINE__, phy_dev, reg, offset, length, p_val16);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if (length == 0
        || offset >= PHY_REG_MAX_LEN
        || length > (PHY_REG_MAX_LEN - offset)
        || !p_val16) {
        return CA_E_PARAM;
    }

#if 0//yocto
    ret = aal_mdio_read(0, 1, phy_dev, reg, 0, &data);
#else//sd1 uboot for 98f MP test

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t\t%s(%d) phy_dev: %#x, reg: %#x, offset: %#x, length: %#x, p_val16: %#x\n", \
    __func__, __LINE__, phy_dev, reg, offset, length, p_val16);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

data = ASIC_PHY_MDIO_READ(phy_dev, reg);

#endif//sd1 uboot for 98f MP test

    if ((0 == offset) && (16 == length)) {
        *p_val16 = data & 0xffff;
    }
    else {
        *p_val16 = (data >> offset) & ((0x1 << length) -1);
    }

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t\t\t%s(%d) data: %#x\n", \
    __func__, __LINE__, data);

printf("\t\t\t%s(%d) *p_val16: %#x\n", \
    __func__, __LINE__, *p_val16);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

end:
    return ret;

}

static ca_status_t __fe_phy_regfield_write
(
    ca_uint8_t   phy_dev,
    ca_uint8_t   reg,
    ca_uint8_t   offset,
    ca_uint8_t   length,
    ca_uint16_t  val16
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint16_t data = 0;
    ca_uint16_t data16 = 0;

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t\t%s(%d) phy_dev: %#x, reg: %#x, offset: %#x, length: %#x, val16: %#x\n", \
    __func__, __LINE__, phy_dev, reg, offset, length, val16);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if (length == 0
        || offset >= PHY_REG_MAX_LEN
        || length > (PHY_REG_MAX_LEN - offset)){
        return CA_E_PARAM;
    }

#if 0//yocto
    ret = aal_mdio_read(0, 1, phy_dev, reg, 0, &data);
#else//sd1 uboot for 98f MP test

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t\t%s(%d) phy_dev: %#x, reg: %#x, offset: %#x, length: %#x, val16: %#x\n", \
    __func__, __LINE__, phy_dev, reg, offset, length, val16);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    data = ASIC_PHY_MDIO_READ(phy_dev, reg);
#endif//sd1 uboot for 98f MP test

    data16 = (data & ~(((0x1 << length) -1) << offset))
                | (val16 << offset);

    if ((data & 0xffff) == data16) {
        goto end;
    }

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t\t\t%s(%d) data16: %#x\n", \
    __func__, __LINE__, data16);

printf("\t\t\t%s(%d) data: %#x\n", \
    __func__, __LINE__, data);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if 0//yocto
    ret = aal_mdio_write(0, 1, phy_dev, reg, 0, data16);
#else//sd1 uboot for 98f MP test
    ASIC_PHY_MDIO_WRITE(phy_dev, reg, data);
#endif//sd1 uboot for 98f MP test

end:
    return ret;

}

void __fe_phy_regfield_mask_write
(
    ca_uint8_t   phy_dev,
    ca_uint8_t   reg,
    ca_uint16_t  mask,
    ca_uint16_t  val16
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint16_t data = 0;
    ca_uint16_t data16 = val16;

    if (mask != 0xffff) {
        aal_mdio_read(0, 1, phy_dev, reg, 0, &data);

        data &= ~mask;  /*mask the bits want to be overwrited */
        data16 = (data & 0xffff) | (val16 & mask); /*overwrite the data */
    }

    aal_mdio_write(0, 1, phy_dev, reg, 0, data16);

end:

    return ret;
}

#endif//sd1 uboot for 98f MP test

static ca_status_t __eth_phy_regfield_read
(
    ca_uint8_t   phy_dev,
    ca_uint8_t   reg,
    ca_uint8_t   offset,
    ca_uint8_t   length,
    ca_uint16_t *p_val16
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint16_t data = 0;

    if (length == 0
        || offset >= PHY_REG_MAX_LEN
        || length > (PHY_REG_MAX_LEN - offset)
        || !p_val16) {
        return CA_E_PARAM;
    }

    ret = aal_mdio_read(0, 1, phy_dev, reg, 0, &data);
    if (ret != CA_E_OK) {
        goto end;
    }

    if ((0 == offset) && (16 == length)) {
        *p_val16 = data & 0xffff;
    }
    else {
        *p_val16 = (data >> offset) & ((0x1 << length) -1);
    }

end:
    return ret;

}

static ca_status_t __eth_phy_regfield_write
(
    ca_uint8_t   phy_dev,
    ca_uint8_t   reg,
    ca_uint8_t   offset,
    ca_uint8_t   length,
    ca_uint16_t  val16
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint16_t data = 0;
    ca_uint16_t data16 = 0;

    if (length == 0
        || offset >= PHY_REG_MAX_LEN
        || length > (PHY_REG_MAX_LEN - offset)){
        return CA_E_PARAM;
    }

    ret = aal_mdio_read(0, 1, phy_dev, reg, 0, &data);
    if (ret != CA_E_OK ) {
        goto end;
    }

    data16 = (data & ~(((0x1 << length) -1) << offset))
                | (val16 << offset);

    if ((data & 0xffff) == data16) {
        goto end;
    }

    ret = aal_mdio_write(0, 1, phy_dev, reg, 0, data16);
    if (ret != CA_E_OK ) {
        goto end;
    }

end:
    return ret;

}

static ca_status_t __eth_phy_regfield_mask_write
(
    ca_uint8_t   phy_dev,
    ca_uint8_t   reg,
    ca_uint16_t  mask,
    ca_uint16_t  val16
)
{
    ca_status_t ret = CA_E_OK;
    ca_uint16_t data = 0;
    ca_uint16_t data16 = val16;

    if (mask != 0xffff) {
        ret = aal_mdio_read(0, 1, phy_dev, reg, 0, &data);
        if (ret != CA_E_OK ) {
            goto end;
        }

        data &= ~mask;  /*mask the bits want to be overwrited */
        data16 = (data & 0xffff) | (val16 & mask); /*overwrite the data */
    }

    ret = aal_mdio_write(0, 1, phy_dev, reg, 0, data16);
    if (ret != CA_E_OK ) {
        goto end;
    }

end:

    return ret;
}

/**************************************************************/
/*             Common Etherent PHY APIs                       */
/**************************************************************/
#if 0//yocto
#else//sd1 uboot for 98f MP test

static ca_status_t  __fe_phy_reset(ca_uint8_t phy_dev)
{
    return __fe_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_RESET, 1, 1);
}

static ca_status_t  __fe_phy_auto_neg_restart(ca_uint8_t phy_dev)
{
    return __fe_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_AN_RESTART, 1, 1);
}

static ca_status_t  __fe_phy_auto_neg_set(ca_uint8_t phy_dev, ca_boolean_t enable)
{
    return __fe_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_AN_CTRL, 1, enable ? 1 : 0);
}

ca_status_t  __fe_phy_link_status_get(ca_uint8_t phy_dev, aal_phy_link_status_t *link_status)
{
    ca_uint16 data16 = 0;

    __fe_phy_regfield_read(phy_dev, PHY_REG_STATUS, 0, 16, &data16);
    __fe_phy_regfield_read(phy_dev, PHY_REG_STATUS, 0, 16, &data16);

    if (data16 & (0x1 << 2)) { /* for 98f fe-phy, link-status[2] */
        link_status->link_up = TRUE;

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t%s(%d)\t\tdata16: %#x\n", __func__, __LINE__, data16);
printf("\t%s(%d)\t\tlink_status->link_up: %#x\n", __func__, __LINE__, link_status->link_up);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    }
    else {
        link_status->link_up  = FALSE;
        link_status->speed    = AAL_PHY_SPEED_10;
        link_status->duplex   = FALSE;

#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t%s(%d)\t\tdata16: %#x\n", __func__, __LINE__, data16);
printf("\t%s(%d)\t\tlink_status->link_up: %#x\n", __func__, __LINE__, link_status->link_up);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        return CA_E_OK;
    }

#if 0//yocto
    if (data16 & (0x1 << PHY_REG_CTRL_BIT_SPEED_SEL_LSB)) {
        link_status->speed = CA_SPEED_100MBPS;
    }
    else {
        link_status->speed = CA_SPEED_10MBPS;
    }

    if (data16 & (0x1 << PHY_REG_CTRL_BIT_DUPLEX)) {
        link_status->duplex = CA_DUPLEX_FULL;
    }
    else {
        link_status->duplex = CA_DUPLEX_HALF;
    }
#else
    /* get phy's actual speed and duplex from status register directily*/
    ca_uint16 data = 0;
    data16 = 0;

    /* Read capability: reg4.8:5 */
    __fe_phy_regfield_read(phy_dev, PHY_REG_AUTONEG_ADV, 0, 16, &data16);

    /* Link partner ability: reg5.8:5 */
    __fe_phy_regfield_read(phy_dev, PHY_REG_LP_ABILITY_BASE, 0, 16, &data);

    data16 &= data;

    if(data16 & BIT(8))
    {
        link_status->speed = CA_SPEED_100MBPS;
        link_status->duplex = CA_DUPLEX_FULL;
    }
    else if(data16 & BIT(7))
    {
        link_status->speed = CA_SPEED_100MBPS;
        link_status->duplex = CA_DUPLEX_HALF;
    }
    else if(data16 & BIT(6))
    {
        link_status->speed = CA_SPEED_10MBPS;
        link_status->duplex = CA_DUPLEX_FULL;
    }
    else
    {
        link_status->speed = CA_SPEED_10MBPS;
        link_status->duplex = CA_DUPLEX_HALF;
    }
#endif

    return CA_E_OK;
}

void ca_fe_phy_link_status_get(
    ca_port_id_t port_id,
    ca_boolean_t link_up,
    ca_uint8_t speed,
    ca_boolean_t duplex
)
{
    aal_phy_link_status_t link_status;
    ca_uint8_t phy_addr;

    GLOBAL_PCS_PHY_CONTROL_0_t global_pcs_phy_control_0;
    global_pcs_phy_control_0.wrd = CA_NE_REG_READ(GLOBAL_PCS_PHY_CONTROL_0);
    phy_addr = ( (global_pcs_phy_control_0.wrd >> (port_id * 5)) & 0x1f);

    /* config speed & duplex */
    memset(&link_status, 0x0, sizeof(aal_phy_link_status_t));
    __fe_phy_link_status_get(phy_addr, &link_status);

    link_up = link_status.link_up;
    speed = (ca_uint8_t *) (&(link_status.speed));
    duplex = link_status.duplex;

    return;
}

void __fe_phy_speed_get(ca_uint8_t phy_dev, aal_phy_speed_mode_t *speed)
{
    ca_uint16 data16 = 0;
    aal_phy_link_status_t link_status;

    memset(&link_status, 0x0, sizeof(aal_phy_link_status_t));

    __fe_phy_link_status_get(phy_dev, &link_status);

    *speed = link_status.speed;

#if 0
    if(link_status->link_up){
        printf("phy dev %#x with speed = %s\n", phy_dev, link_status->speed? "100Mbps" : "10Mbps");
    }else{
        printf("The target port to be inspected is currently link down...\n");
    }
#endif

    return;
}

void  __fe_phy_duplex_get(ca_uint8_t phy_dev, ca_boolean_t *duplex)
{
    ca_uint16 data16 = 0;
    aal_phy_link_status_t link_status;

    memset(&link_status, 0x0, sizeof(aal_phy_link_status_t));

    __fe_phy_link_status_get(phy_dev, &link_status);

    *duplex = link_status.duplex;

#if 0
    if(link_status->link_up){
        printf("phy dev %#x with duplex = %s\n", phy_dev, link_status->duplex? "Full" : "Half");
    }else{
        printf("The target port to be inspected is currently link down...\n");
    }
#endif

    return;
}

static ca_status_t  __ge_nic_link_status_get(ca_uint8_t phy_dev, aal_phy_link_status_t *link_status)
{
    ca_uint32_t reg_ad;
    ca_uint16_t page, data;

    /* get phy's actual speed and duplex from status register directily*/
    /* chk PHYSR */
    /* QA board ref. spec.: http://dtdinfo/svn/CN/CN-WLNIC-FW/trunk/Work/SoC/8198F/Switch/14.phy_switch_ic_data/00.phy_8211/RTL8211F(D)(I)-CG_DataSheet_1.7.pdf */
    /* MP board ref. spec.: http://dtdinfo/svn/CN/CN-WLNIC-FW/trunk/Work/SoC/8198F/Switch/14.phy_switch_ic_data/00.phy_8211/RTL8211E(G)-VB(VL)-CG_DataSheet_1.8.pdf */
#ifndef CONFIG_98F_8211F_QAB /* 98FH MP board */
/* chk PHYSR for link status */
    page = GE_NIC_8211EG_PHY_SR_PAGE;
    reg_ad = GE_NIC_8211EG_PHY_SR;

    memset(&data, 0x0, sizeof(ca_uint16_t));
    mdio_page_read(page, phy_dev, reg_ad, &data);

    if (data & (0x1 << 10)) { /* for {8211eg} ge nic, link-status[10] */
        link_status->link_up = TRUE;
    } else {
        link_status->link_up  = FALSE;
        link_status->speed    = AAL_PHY_SPEED_10;
        link_status->duplex   = FALSE;
        return CA_E_NOT_FOUND;
    }

    memset(&data, 0x0, sizeof(ca_uint16_t));
    mdio_page_read(page, phy_dev, reg_ad, &data);

    /* PHYSR - 17.15:14 speed */
    if(data & (1<<15)) /* 1000M or reserved */
    {
            if(data & (1<<14))
            {
#ifdef CONFIG_98FH_MP_ENABLE
   //                 printf("{F005}\n");
  //                  REG(0xf4320734)++;
   //            printf("\t%s:the GE NIC speed is out of range! (%d)\n", __func__, __LINE__);
#else
                    printf("\t%s:the GE NIC speed is out of range! (%d)\n", __func__, __LINE__);
#endif
                    return CA_E_PARAM;
            }
            else
            {
                    link_status->speed = AAL_GE_NIC_SPEED_1000;
            }
    }
    else  /* 100M or 10M */
    {
            if(data & (1<<14))
            {
                    link_status->speed = AAL_GE_NIC_SPEED_100;
            }
            else
            {
                    link_status->speed = AAL_GE_NIC_SPEED_10;
            }
    }

    /* PHYSR - 17.13 duplex */
    if(data & (1<<13))
    {
            link_status->duplex = AAL_GE_NIC_FULL_DUPLEX;
    }
    else
    {
            link_status->duplex = AAL_GE_NIC_HALF_DUPLEX;
    }

#else /* 98FH QA board */
    /* chk PHYSR for link status */
    page = GE_NIC_8211F_PHY_SR_PAGE;
    reg_ad = GE_NIC_8211F_PHY_SR;

    memset(&data, 0x0, sizeof(ca_uint16_t));
    mdio_page_read(page, phy_dev, reg_ad, &data);

    if (data & (0x1 << 2)) { /* for {8211f} ge nic, link-status[2] */
        link_status->link_up = TRUE;

    } else {
        link_status->link_up  = FALSE;
        link_status->speed    = AAL_PHY_SPEED_10;
        link_status->duplex   = FALSE;

        return CA_E_NOT_FOUND;
    }

    memset(&data, 0x0, sizeof(ca_uint16_t));
    mdio_page_read(page, phy_dev, reg_ad, &data);
    /* PHYSR - 26.5:4 speed */
    if(data & (1<<5)) /* 1000M or reserved */
    {
            if(data & (1<<4))
            {
#ifdef CONFIG_98FH_MP_ENABLE
    //                 printf("{F005}\n");
    //                 REG(0xf4320734)++;
     //             printf("\t%s:the GE NIC speed is out of range! (%d)\n", __func__, __LINE__);
#else
                    printf("\t%s:the GE NIC speed is out of range! (%d)\n", __func__, __LINE__);
#endif
                    return CA_E_PARAM;
            }
            else
            {
                    link_status->speed = AAL_GE_NIC_SPEED_1000;
            }
    }
    else  /* 100M or 10M */
    {
            if(data & (1<<4))
            {
                    link_status->speed = AAL_GE_NIC_SPEED_100;
            }
            else
            {
                    link_status->speed = AAL_GE_NIC_SPEED_10;
            }
    }

    /* PHYSR - 26.3 duplex */
    if(data & (1<<3))
    {
            link_status->duplex = AAL_GE_NIC_FULL_DUPLEX;
    }
    else
    {
            link_status->duplex = AAL_GE_NIC_HALF_DUPLEX;
    }
#endif /* CONFIG_98FH_MP_ENABLE */

    return CA_E_OK;
}

#define DBG__MDIOACCESSEXTDEVPHYID      0
static ca_status_t  _mdioAccessExtDevPhyID(
    ca_uint32_t ExtDev_PAGE,
    ca_uint32_t ExtDev_PHY_ADDR,
    ca_uint32_t ExtDev_PHYID1_REG,
    ca_uint32_t ExtDev_PHYID2_REG,
    ca_uint32_t ExtDev_PHYID1,
    ca_uint32_t ExtDev_PHYID1_bm,
    ca_uint32_t ExtDev_PHYID2,
    ca_uint32_t ExtDev_PHYID2_bm
){
    ca_uint32_t reg_ad1, reg_ad2;
    ca_uint16_t page, data1, data2, expData1, expData2, actualData1, actualData2;
    ca_uint8_t phy_ad;

#if DBG__MDIOACCESSEXTDEVPHYID
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PAGE", ExtDev_PAGE, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHY_ADDR", ExtDev_PHY_ADDR, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1_REG", ExtDev_PHYID1_REG, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2_REG", ExtDev_PHYID2_REG, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1", ExtDev_PHYID1, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1_bm", ExtDev_PHYID1_bm, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2", ExtDev_PHYID2, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2_bm", ExtDev_PHYID2_bm, __func__, __LINE__);
#endif /* DBG__MDIOACCESSEXTDEVPHYID */

    page = ExtDev_PAGE;
    phy_ad = ExtDev_PHY_ADDR;
    reg_ad1 = ExtDev_PHYID1_REG;
    reg_ad2 = ExtDev_PHYID2_REG;

    /* chk PHYID1 for external device */
    memset(&data1, 0x0, sizeof(ca_uint16_t));
    mdio_page_read(page, phy_ad, reg_ad1, &data1);

    /* chk PHYID2 for external device */
    memset(&data2, 0x0, sizeof(ca_uint16_t));
    mdio_page_read(page, phy_ad, reg_ad2, &data2);

    expData1 = ExtDev_PHYID1 & ExtDev_PHYID1_bm & CA_AAL_MDIO_DATA_MASK; /* take 16LSB */
    expData2 = ExtDev_PHYID2 & ExtDev_PHYID2_bm & CA_AAL_MDIO_DATA_MASK; /* take 16LSB */

    actualData1 = data1 & ExtDev_PHYID1_bm & CA_AAL_MDIO_DATA_MASK; /* take 16LSB */
    actualData2 = data2 & ExtDev_PHYID2_bm & CA_AAL_MDIO_DATA_MASK; /* take 16LSB */

#if DBG__MDIOACCESSEXTDEVPHYID
    printf("%s(0x%08x)\t[%s(%d)]\n", "ExtDev_PHYID1", ExtDev_PHYID1, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "ExtDev_PHYID2", ExtDev_PHYID2, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "data1", data1, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "data2", data2, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "ExtDev_PHYID1_bm", ExtDev_PHYID1_bm, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "ExtDev_PHYID2_bm", ExtDev_PHYID2_bm, __func__, __LINE__);

    printf("%s(0x%08x)\t[%s(%d)]\n", "ExtDev_PHYID1 & ExtDev_PHYID1_bm", expData1, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "ExtDev_PHYID2 & ExtDev_PHYID2_bm", expData2, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "data1 & ExtDev_PHYID1_bm", actualData1, __func__, __LINE__);
    printf("%s(0x%08x)\t[%s(%d)]\n", "data2 & ExtDev_PHYID2_bm", actualData2, __func__, __LINE__);
#endif /* DBG__MDIOACCESSEXTDEVPHYID */

    if( (expData1 == actualData1) && (expData2 == actualData2) )
    {
        return CA_E_OK;
    } else {
        return CA_E_ERROR;
    }

}

#endif//sd1 uboot for 98f MP test

static ca_status_t  __eth_phy_reset(ca_uint8_t phy_dev)
{
    return __eth_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_RESET, 1, 1);
}

static ca_status_t  __eth_phy_auto_neg_restart(ca_uint8_t phy_dev)
{
    return __eth_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_AN_RESTART, 1, 1);
}

static ca_status_t  __eth_phy_power_set(ca_uint8_t phy_dev, ca_boolean_t power_up)
{
    return __eth_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_PWR_DOWN, 1, power_up ? FALSE : TRUE);
}

static ca_status_t  __eth_phy_power_get(ca_uint8_t phy_dev, ca_boolean_t *power_up)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_PWR_DOWN, 1, &data16);
    *power_up = data16 ? FALSE : TRUE;

    return CA_E_OK;
}

static ca_status_t  __eth_phy_auto_neg_set(ca_uint8_t phy_dev, ca_boolean_t enable)
{
    return __eth_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_AN_CTRL, 1, enable ? 1 : 0);
}

static ca_status_t  __eth_phy_auto_neg_get(ca_uint8_t phy_dev, ca_boolean_t *enable)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_AN_CTRL, 1, &data16);
    *enable = data16 ? TRUE : FALSE;

    return CA_E_OK;
}

static ca_status_t  __eth_phy_ability_adv_set(ca_uint8_t phy_dev, aal_phy_autoneg_adv_t adv)
{
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_10BT_HD,    1, adv.adv_10base_t_half);
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_10BT_FD,    1, adv.adv_10base_t_full);
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_100BX_HD,   1, adv.adv_100base_tx_half);
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_100BX_FD,   1, adv.adv_100base_tx_full);
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_100BT4_CAP, 1, adv.adv_100base_t4);
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_PAUSE_CAP,  1, adv.adv_pause);
    __eth_phy_regfield_write(phy_dev, PHY_REG_AUTONEG_ADV, PHY_REG_AN_ADVR_BIT_PAUSE_ASYM, 1, adv.adv_asym_pause);
    __eth_phy_regfield_write(phy_dev, PHY_REG_1000BASET_CTRL, PHY_REG_1000BT_CTRL_BIT_ADVR_1000_FD, 1, adv.adv_1000base_t_full);
    __eth_phy_regfield_write(phy_dev, PHY_REG_1000BASET_CTRL, PHY_REG_1000BT_CTRL_BIT_ADVR_1000_HD, 1, adv.adv_1000base_t_half);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_ability_adv_get(ca_uint8_t phy_dev, aal_phy_autoneg_adv_t *adv)
{
    ca_uint16 val16 = 0;
    ca_status_t ret = CA_E_OK;

    ret = __eth_phy_regfield_read(phy_dev, PHY_REG_AUTONEG_ADV, 0, 16, &val16);
    if (CA_E_OK != ret)
        return ret;

    adv->adv_asym_pause       = (val16 >> PHY_REG_AN_ADVR_BIT_PAUSE_ASYM) & 0x1;
    adv->adv_pause            = (val16 >> PHY_REG_AN_ADVR_BIT_PAUSE_CAP) & 0x1;
    adv->adv_100base_t4       = (val16 >> PHY_REG_AN_ADVR_BIT_100BT4_CAP) & 0x1;
    adv->adv_100base_tx_full  = (val16 >> PHY_REG_AN_ADVR_BIT_100BX_FD) & 0x1;
    adv->adv_100base_tx_half  = (val16 >> PHY_REG_AN_ADVR_BIT_100BX_HD) & 0x1;
    adv->adv_10base_t_full    = (val16 >> PHY_REG_AN_ADVR_BIT_10BT_FD) & 0x1;
    adv->adv_10base_t_half    = (val16 >> PHY_REG_AN_ADVR_BIT_10BT_HD) & 0x1;

    ret = __eth_phy_regfield_read(phy_dev, PHY_REG_1000BASET_CTRL, 0, 16, &val16);
    if (CA_E_OK != ret)
        return ret;

    adv->adv_1000base_t_full  = (val16 >> PHY_REG_1000BT_CTRL_BIT_ADVR_1000_FD) & 0x1;
    adv->adv_1000base_t_half  = (val16 >> PHY_REG_1000BT_CTRL_BIT_ADVR_1000_HD) & 0x1;

    return CA_E_OK;
}

static ca_status_t  __eth_phy_partner_ability_adv_get(ca_uint8_t phy_dev, aal_phy_autoneg_adv_t *adv)
{
    ca_uint16 val16 = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_LP_ABILITY_BASE, 0, 16, &val16);

    adv->adv_asym_pause       = (val16 >> PHY_REG_AN_ADVR_BIT_PAUSE_ASYM) & 0x1;
    adv->adv_pause            = (val16 >> PHY_REG_AN_ADVR_BIT_PAUSE_CAP) & 0x1;
    adv->adv_100base_t4       = (val16 >> PHY_REG_AN_ADVR_BIT_100BT4_CAP) & 0x1;
    adv->adv_100base_tx_full  = (val16 >> PHY_REG_AN_ADVR_BIT_100BX_FD) & 0x1;
    adv->adv_100base_tx_half  = (val16 >> PHY_REG_AN_ADVR_BIT_100BX_HD) & 0x1;
    adv->adv_10base_t_full    = (val16 >> PHY_REG_AN_ADVR_BIT_10BT_FD) & 0x1;
    adv->adv_10base_t_half    = (val16 >> PHY_REG_AN_ADVR_BIT_10BT_HD) & 0x1;

    return CA_E_OK;

}

static ca_status_t  __eth_phy_link_status_get(ca_uint8_t phy_dev, aal_phy_link_status_t *link_status)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_STATUS, 0, 16, &data16);
    __eth_phy_regfield_read(phy_dev, PHY_REG_STATUS, 0, 16, &data16);

    if (data16 & (0x1 << 1)) {
        link_status->link_up = TRUE;

printf("\t%s(%d)\t\tdata16: %#x\n", __func__, __LINE__, data16);
printf("\t%s(%d)\t\tlink_status->link_up: %#x\n", __func__, __LINE__, link_status->link_up);
    }
    else {
        link_status->link_up  = FALSE;
        link_status->speed    = AAL_PHY_SPEED_10;
        link_status->duplex   = FALSE;

printf("\t%s(%d)\t\tdata16: %#x\n", __func__, __LINE__, data16);
printf("\t%s(%d)\t\tlink_status->link_up: %#x\n", __func__, __LINE__, link_status->link_up);
        return CA_E_OK;
    }

    /* phy's actual speed and duplex can be got from ctrl register directily*/
    data16 = 0;
    __eth_phy_regfield_read(phy_dev, PHY_REG_CTRL, 0, 16, &data16);
    if (data16 & (0x1 << PHY_REG_CTRL_BIT_SPEED_SEL_LSB)) {
        link_status->speed = CA_SPEED_100MBPS;
    }
    else {
        link_status->speed = CA_SPEED_10MBPS;
    }

    if (data16 & (0x1 << PHY_REG_CTRL_BIT_DUPLEX)) {
        link_status->duplex = CA_DUPLEX_FULL;
    }
    else {
        link_status->duplex = CA_DUPLEX_HALF;
    }

printf("\t%s(%d)\t\t%#x\n", __func__, __LINE__, data16);
    return CA_E_OK;
}

static ca_status_t  __eth_phy_speed_set(ca_uint8_t phy_dev, aal_phy_speed_mode_t speed)
{
    ca_status_t ret    = CA_E_OK;
    ca_uint16 data16 = 0, ge_data = 0;

    switch(speed){
        case AAL_PHY_SPEED_10:
            data16 = 0;
            ge_data = 0x0;
            break;
        case AAL_PHY_SPEED_100:
            data16 = 0x2000;
            ge_data = 0x0;
            break;
        case AAL_PHY_SPEED_1000:
            data16 = 0x40;
            ge_data = 0x300;
            break;
        default:
            return CA_E_PARAM;
    }

    ret =  __eth_phy_regfield_mask_write(phy_dev, PHY_REG_CTRL,
        0x2040, data16);

    ret |=  __eth_phy_regfield_mask_write(phy_dev, PHY_REG_1000BASET_CTRL,
        0x300, ge_data);

    return ret;

}

static ca_status_t  __eth_phy_speed_get(ca_uint8_t phy_dev, aal_phy_speed_mode_t *speed)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_CTRL, 0, 16, &data16);

    data16 &= 0x2040;
    switch(data16){
        case 0:
            *speed = AAL_PHY_SPEED_10;
            break;
        case 0x2000:
            *speed = AAL_PHY_SPEED_100;
            break;
        case 0x40:
            *speed = AAL_PHY_SPEED_1000;
            break;
        default:
            return CA_E_ERROR;
    }

    return CA_E_OK;
}


static ca_status_t  __eth_phy_duplex_set(ca_uint8_t phy_dev, ca_boolean_t is_full)
{
    return __eth_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_DUPLEX, 1, is_full ? 1 : 0);
}

static ca_status_t  __eth_phy_duplex_get(ca_uint8_t phy_dev, ca_boolean_t *duplex)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_DUPLEX, 1, &data16);

    *duplex = (data16 != 0);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_flow_ctrl_set(ca_uint8_t phy_dev, ca_boolean_t enable)
{
    return CA_E_NOT_SUPPORT;
}

static ca_status_t  __eth_phy_flow_ctrl_get(ca_uint8_t phy_dev, ca_boolean_t *enable)
{
    return CA_E_NOT_SUPPORT;
}

static ca_status_t  __eth_phy_mdix_set(ca_uint8_t phy_dev, aal_phy_mdi_mode_t mdix_mode)
{
    return CA_E_NOT_SUPPORT;
}

static ca_status_t  __eth_phy_mdix_get(ca_uint8_t phy_dev, aal_phy_mdi_mode_t *mdix_mode)
{
    return CA_E_NOT_SUPPORT;
}

static ca_status_t  __eth_phy_loopback_set(ca_uint8_t phy_dev, aal_phy_lpbk_mode_t mode)
{
    if (mode.rx2tx_enable) {
        return CA_E_PARAM;
    }

    __eth_phy_regfield_write(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_LPBK, 1, mode.tx2rx_enable ? 1 : 0);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_loopback_get(ca_uint8_t phy_dev, aal_phy_lpbk_mode_t *mode)
{
    ca_uint16  data = 0;

    __eth_phy_regfield_read(phy_dev, PHY_REG_CTRL, PHY_REG_CTRL_BIT_LPBK,  1, &data);

    mode->tx2rx_enable = data;
    mode->rx2tx_enable = 0;

    return CA_E_OK;
}


/**************************************************************/
/*             Private Etherent PHY APIs                      */
/**************************************************************/
/* Fiber Mode and Loopback */
#define __RTL8021F_REG_FM_LPKT                        (28)
#define __RTL8021F_REG_FM_LPKT_BIT_FIBER_MODE         (5)
#define __RTL8021F_REG_FM_LPKT_BIT_AUTO_MDIX          (2)
#define __RTL8021F_REG_FM_LPKT_BIT_FORCE_MDI          (1)

/*Int mask Register */
#define __RTL8201F_REG_INTR_EN                        (0x13)
#define __RTL8201F_REG_INTR_EN_BIT_AN_ERR             (11)
#define __RTL8201F_REG_INTR_EN_BIT_SPEED
#define __RTL8201F_REG_INTR_EN_BIT_DUPLEX             (12)
#define __RTL8201F_REG_INTR_EN_BIT_LINK               (13)

/*MDI mode Register*/
#define __RTL8201F_REG_MDI_MODE                       (0x1c)
#define __RTL8201F_REG_MDI_MODE_BIT_MDI_AUTO          (2)
#define __RTL8201F_REG_MDI_MODE_BIT_MDI_FORCE         (1)

/*Int Status Register*/
#define __RTL8201F_REG_INTR_STATUS                    (0x1e)
#define __RTL8201F_REG_INTR_STATUS_BIT_AN_ERR         (15)
#define __RTL8201F_REG_INTR_STATUS_BIT_SPEED          (14)
#define __RTL8201F_REG_INTR_STATUS_BIT_DUPLEX         (13)
#define __RTL8201F_REG_INTR_STATUS_BIT_LINK           (11)

 /* Page select register */
#define __RTL8201F_REG_PAGE_SEL                       (0x1f)
#define __RTL8201F_REG_PAGE_INTR_EN                   (0x7)

#define  __RTL8201F_POWER_DOWN_CYCLE_TIME                 (10)
#define  __RTL8201F_POWER_UP_CYCLE_TIME                   (500)

static ca_status_t __eth_phy_rtl8201f_init(ca_uint8_t phy_dev)
{
    /*
      * RGMII mode select
      */
    /* REG31 write 0x0007, set to extension page 7*/
    __eth_phy_regfield_write(phy_dev, 31, 0, 16, 0x0007);

    /* REG16 write 0x1FFB, set an external clock input to CKXTAL2 */
    __eth_phy_regfield_write(phy_dev, 16, 0, 16, 0x7FFB);

    /* REG31 write 0X0000, back to page0 */
    __eth_phy_regfield_write(phy_dev, 31, 0, 16, 0);

    /*
      * LED mode select
      */
    /* REG31 write 0x0007, set to extension page 7*/
    __eth_phy_regfield_write(phy_dev, 31, 0, 16, 0x0007);

    /* Write MII Phy 2 Reg 19 Data = 0xc03c */
    __eth_phy_regfield_write(phy_dev, 19, 0, 16, 0xc03c);
    __eth_phy_regfield_write(phy_dev, 17, 0, 16, 0x00bb);

    /* REG31 write 0X0000, back to page0 */
    __eth_phy_regfield_write(phy_dev, 31, 0, 16, 0);

    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8201f_int_mask_all(ca_uint8 phy_dev)
{
    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, __RTL8201F_REG_PAGE_INTR_EN);
    __eth_phy_regfield_write(phy_dev, __RTL8201F_REG_INTR_EN, 0, 16, 0);
    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, 0);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8201f_int_en_set(ca_uint8 phy_dev, ca_uint16_t data)
{

    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, __RTL8201F_REG_PAGE_INTR_EN);

    if (data & PHY_INTR_LINK_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_LINK,
                           1,
                           1);
    }

    if (data & PHY_INTR_DUPLEX_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_DUPLEX,
                           1,
                           1);
    }

    if (data & PHY_INTR_SPEED_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_LINK,
                           1,
                           1);
    }

    if (data & PHY_INTR_AN_FAIL) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_AN_ERR,
                           1,
                           1);
    }

    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, 0);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8201f_int_en_get(ca_uint8 phy_dev, ca_uint16_t *data)
{
    ca_uint16_t data16 = 0;

    __eth_phy_regfield_write(phy_dev, __RTL8201F_REG_PAGE_SEL, 0, 16, __RTL8201F_REG_PAGE_INTR_EN);
    __eth_phy_regfield_read(phy_dev, __RTL8201F_REG_INTR_EN, 0, 16, &data16);

    if (data16 & (0x1 << __RTL8201F_REG_INTR_EN_BIT_LINK)) {
        *data |= PHY_INTR_LINK_CHANGE;
        *data |= PHY_INTR_SPEED_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_EN_BIT_DUPLEX)) {
        *data |= PHY_INTR_DUPLEX_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_EN_BIT_AN_ERR)) {
        *data |= PHY_INTR_AN_FAIL;
    }

    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, 0);

    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8201f_int_status_get(ca_uint8 phy_dev, ca_uint16_t *data)
{
    ca_uint16_t data16 = 0;

    __eth_phy_regfield_read(phy_dev, __RTL8201F_REG_INTR_STATUS, 0, 16, &data16);

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_LINK)) {
        *data |= PHY_INTR_LINK_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_SPEED)) {
        *data |= PHY_INTR_SPEED_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_DUPLEX)) {
        *data |= PHY_INTR_DUPLEX_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_AN_ERR)) {
        *data |= PHY_INTR_AN_FAIL;
    }

    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8201f_int_status_clr(ca_uint8 phy_dev)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, __RTL8201F_REG_INTR_STATUS, 0, 16, &data16);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8201f_mdix_set(ca_uint8_t phy_dev, aal_phy_mdi_mode_t mdix_mode)
{
    ca_status_t ret = CA_E_OK;

    if (AAL_PHY_MDI_AUTO == mdix_mode) {
        ret = __eth_phy_regfield_write(phy_dev, __RTL8021F_REG_FM_LPKT, __RTL8021F_REG_FM_LPKT_BIT_AUTO_MDIX, 1, 0x1);
    }
    else if (AAL_PHY_MDI_MDI == mdix_mode) {
        ret = __eth_phy_regfield_mask_write(phy_dev, __RTL8021F_REG_FM_LPKT, 0x6, 0x1);
    }
    else {
        ret = __eth_phy_regfield_mask_write(phy_dev, __RTL8021F_REG_FM_LPKT, 0x6, 0x0);
    }

    return ret;
}

static ca_status_t  __eth_phy_rtl8201f_mdix_get(ca_uint8_t phy_dev, aal_phy_mdi_mode_t *mdix_mode)
{
    ca_uint16 data16 = 0;

    if (CA_E_OK != __eth_phy_regfield_read(phy_dev, __RTL8021F_REG_FM_LPKT, 0, 16, &data16)) {
        return CA_E_INTERNAL;
    }

    if (data16 & (1 << __RTL8021F_REG_FM_LPKT_BIT_AUTO_MDIX)) {
        *mdix_mode = AAL_PHY_MDI_AUTO;
    }
    else if (data16 & (1 << __RTL8021F_REG_FM_LPKT_BIT_FORCE_MDI)){
        *mdix_mode = AAL_PHY_MDI_MDI;
    }
    else {
        *mdix_mode = AAL_PHY_MDI_MDIX;

    }

    return CA_E_NOT_SUPPORT;
}

/* Specific Register */
#define __RTL8211F_REG_SPEC_PHYCR1                   (0x18)
#define __RTL8211F_REG_SPEC_PHYCR2                   (0x19)
#define __RTL8211F_REG_SPEC_PHYSR                    (0x1A)

/*Int mask Register */
#define __RTL8211_REG_INTR_EN                        (0x12)
#define __RTL8211_REG_INTR_EN_BIT_AN_ERR              (0)
#define __RTL8211_REG_INTR_EN_BIT_PAGE_RCVD           (2)
#define __RTL8211_REG_INTR_EN_BIT_AN_DONE             (3)
#define __RTL8211_REG_INTR_EN_BIT_LINK                (4)
#define __RTL8211_REG_INTR_EN_BIT_PME                 (7)
#define __RTL8211_REG_INTR_EN_BIT_ALDPS               (9)
#define __RTL8211_REG_INTR_EN_BIT_JABBER              (10)

/*MDI mode Register*/
#define __RTL8211_REG_MDI_MODE                       (0x18)
#define __RTL8211_REG_MDI_MODE_BIT_MDI_AUTO          (9)
#define __RTL8211_REG_MDI_MODE_BIT_MDI_FORCE         (8)

/*Int Status Register*/
#define __RTL8211_REG_INTR_STATUS                    (0x1d)

#define __RTL8211_REG_INTR_STATUS_BIT_AN_ERR         (0)
#define __RTL8211_REG_INTR_STATUS_BIT_PAGE_RCVD      (2)
#define __RTL8211_REG_INTR_STATUS_BIT_AN_DONE        (3)
#define __RTL8211_REG_INTR_STATUS_BIT_LINK           (4)
#define __RTL8211_REG_INTR_STATUS_BIT_PME            (7)
#define __RTL8211_REG_INTR_STATUS_BIT_ALDPS          (9)
#define __RTL8211_REG_INTR_STATUS_BIT_JABBER         (10)


 /* Page select register */
#define __RTL8211_REG_PAGE_SEL                       (0x1f)
#define __RTL8211_REG_PAGE_INTR_EN                   (0x0)
#define __RTL8211_REG_PAGE_LCR                       (0xd04)
#define __RTL8211_REG_PAGE_INTBCR                    (0xd40)


#define  __RTL8211_POWER_DOWN_CYCLE_TIME              (10)
#define  __RTL8211_POWER_UP_CYCLE_TIME                (500)



static ca_status_t  __eth_phy_rtl8211_int_mask_all(ca_uint8 phy_dev)
{
    __eth_phy_regfield_write(phy_dev,__RTL8211_REG_PAGE_SEL, 0, 16, __RTL8211_REG_PAGE_INTR_EN);
    __eth_phy_regfield_write(phy_dev, __RTL8211_REG_INTR_EN, 0, 16, 0);
    __eth_phy_regfield_write(phy_dev,__RTL8211_REG_PAGE_SEL, 0, 16, 0);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8211_int_en_set(ca_uint8 phy_dev, ca_uint16_t data)
{

    __eth_phy_regfield_write(phy_dev,__RTL8211_REG_PAGE_SEL, 0, 16, __RTL8211_REG_PAGE_INTR_EN);

    if (data & PHY_INTR_LINK_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8211_REG_INTR_EN,
                           __RTL8211_REG_INTR_EN_BIT_LINK,
                           1,
                           1);
    }
#if 0
    if (data & PHY_INTR_DUPLEX_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8211_REG_INTR_EN,
                           __RTL8211_REG_INTR_EN_BIT_DUPLEX,
                           1,
                           1);
    }

    if (data & PHY_INTR_SPEED_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8211_REG_INTR_EN,
                           __RTL8211_REG_INTR_EN_BIT_LINK,
                           1,
                           1);
    }
#endif

    if (data & PHY_INTR_AN_FAIL) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8211_REG_INTR_EN,
                           __RTL8211_REG_INTR_EN_BIT_AN_ERR,
                           1,
                           1);
    }

    __eth_phy_regfield_write(phy_dev,__RTL8211_REG_PAGE_SEL, 0, 16, 0);

    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8211_int_en_get(ca_uint8 phy_dev, ca_uint16_t *data)
{
    ca_uint16_t data16 = 0;

    __eth_phy_regfield_write(phy_dev, __RTL8211_REG_PAGE_SEL, 0, 16, __RTL8211_REG_PAGE_INTR_EN);
    __eth_phy_regfield_read(phy_dev, __RTL8211_REG_INTR_EN, 0, 16, &data16);

    if (data16 & (0x1 << __RTL8211_REG_INTR_EN_BIT_LINK)) {
        *data |= PHY_INTR_LINK_CHANGE;
        *data |= PHY_INTR_SPEED_CHANGE;
    }
#if 0
    if (data16 & (0x1 << __RTL8211_REG_INTR_EN_BIT_DUPLEX)) {
        *data |= PHY_INTR_DUPLEX_CHANGE;
    }
#endif
    if (data16 & (0x1 << __RTL8211_REG_INTR_EN_BIT_AN_ERR)) {
        *data |= PHY_INTR_AN_FAIL;
    }

    __eth_phy_regfield_write(phy_dev,__RTL8211_REG_PAGE_SEL, 0, 16, 0);

    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8211_int_status_get(ca_uint8 phy_dev, ca_uint16_t *data)
{
    ca_uint16_t data16 = 0;

    __eth_phy_regfield_read(phy_dev, __RTL8211_REG_INTR_STATUS, 0, 16, &data16);

    if (data16 & (0x1 << __RTL8211_REG_INTR_STATUS_BIT_LINK)) {
        *data |= PHY_INTR_LINK_CHANGE;
    }
#if 0
    if (data16 & (0x1 << __RTL8211_REG_INTR_STATUS_BIT_SPEED)) {
        *data |= PHY_INTR_SPEED_CHANGE;
    }

    if (data16 & (0x1 << __RTL8211_REG_INTR_STATUS_BIT_DUPLEX)) {
        *data |= PHY_INTR_DUPLEX_CHANGE;
    }
#endif
    if (data16 & (0x1 << __RTL8211_REG_INTR_STATUS_BIT_AN_ERR)) {
        *data |= PHY_INTR_AN_FAIL;
    }

    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8211_int_status_clr(ca_uint8 phy_dev)
{
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, __RTL8211_REG_INTR_STATUS, 0, 16, &data16);

    return CA_E_OK;
}
static ca_status_t  __eth_phy_rtl8211_link_status_get(ca_uint8_t phy_dev, aal_phy_link_status_t *link_status)
{
        ca_uint16   data16 = 0;
        ca_status_t ret    = CA_E_OK;
        aal_phy_reg_GBCR_t      reg_GBCR_val;
        aal_phy_reg_GBSR_t      reg_GBSR_val;
        aal_phy_reg_ANAR_t      reg_ANAR_val;
        aal_phy_reg_ANLPAR_t reg_ANLPAR_val;


        ret = __eth_phy_regfield_read(phy_dev, __RTL8211F_REG_SPEC_PHYSR, 0, 16, &data16);
        if (CA_E_OK != ret)
                return ret;

        /*
         * __RTL8211F_REG_SPEC_PHYSR_BIT_LNK             2
         * __RTL8211F_REG_SPEC_PHYSR_BIT_DUPLEX          3
         * __RTL8211F_REG_SPEC_PHYSR_BIT_SPEED           4
         */
        link_status->link_up = (data16 & (0x0004)) ? TRUE: FALSE;

        if (!link_status->link_up) {
                link_status->speed    = AAL_PHY_SPEED_10;
                link_status->duplex   = FALSE;
                return CA_E_OK;
        }

        __eth_phy_regfield_read(phy_dev, PHY_REG_1000BASET_CTRL, 0, 16, &reg_GBCR_val.wrd);
        __eth_phy_regfield_read(phy_dev, PHY_REG_1000BASET_STATUS, 0, 16, &reg_GBSR_val.wrd);
        //printf("%s: reg_GBCR_val.wrd=0x%x, reg_GBSR_val.wrd=0x%x\n", __func__, reg_GBCR_val.wrd, reg_GBSR_val.wrd);

        /* check 1000Mbps connection */
        if (reg_GBCR_val.bf.adv_1000base_t_full && reg_GBSR_val.bf.lp_1000base_t_full) {
                //printf("%s: Link at 1000Mbps Full Duplex\n", __func__);
                link_status->speed = CA_SPEED_1000MBPS;
                link_status->duplex = CA_DUPLEX_FULL;
                return CA_E_OK;
        }

        __eth_phy_regfield_read(phy_dev, PHY_REG_AUTONEG_ADV, 0, 16, &reg_ANAR_val.wrd);
        __eth_phy_regfield_read(phy_dev, PHY_REG_LP_ABILITY_BASE, 0, 16, &reg_ANLPAR_val.wrd);
        //printf("%s: reg_ANAR_val.wrd=0x%x, reg_ANLPAR_val.wrd=0x%x\n", __func__, reg_ANAR_val.wrd, reg_ANLPAR_val.wrd);
        if (reg_ANAR_val.bf.adv_100base_tx_full && reg_ANLPAR_val.bf.adv_100base_tx_full) {
                //printf("%s: Link at 100Mbps Full Duplex\n", __func__);
                link_status->speed = CA_SPEED_100MBPS;
                link_status->duplex = CA_DUPLEX_FULL;
                return CA_E_OK;
        }

        if (reg_ANAR_val.bf.adv_100base_tx_half && reg_ANLPAR_val.bf.adv_100base_tx_half) {
                //printf("%s: Link at 100Mbps Half Duplex\n", __func__);
                link_status->speed = CA_SPEED_100MBPS;
                link_status->duplex = CA_DUPLEX_HALF;
                return CA_E_OK;
                                                                                                      }

        if (reg_ANAR_val.bf.adv_10base_t_full && reg_ANLPAR_val.bf.adv_10base_t_full) {
                //printf("%s: Link at 10Mbps Full Duplex\n", __func__);
                link_status->speed = CA_SPEED_10MBPS;
                link_status->duplex = CA_DUPLEX_FULL;
                return CA_E_OK;
        }

        if (reg_ANAR_val.bf.adv_10base_t_half && reg_ANLPAR_val.bf.adv_10base_t_half) {
                //printf("%s: Link at 10Mbps Half Duplex\n", __func__);
                link_status->speed = CA_SPEED_10MBPS;
                link_status->duplex = CA_DUPLEX_HALF;
                return CA_E_OK;
        }

        return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8211_mdix_set(ca_uint8_t phy_dev, aal_phy_mdi_mode_t mdix_mode)
{
    ca_status_t ret = CA_E_OK;

    if (AAL_PHY_MDI_AUTO == mdix_mode) {
        ret = __eth_phy_regfield_write(phy_dev, __RTL8211_REG_MDI_MODE, __RTL8211_REG_MDI_MODE_BIT_MDI_AUTO, 1, 0);
    }
    else if (AAL_PHY_MDI_MDI == mdix_mode) {
        ret = __eth_phy_regfield_mask_write(phy_dev, __RTL8211_REG_MDI_MODE, 0x300, 0x300);
    }
    else {
        ret = __eth_phy_regfield_mask_write(phy_dev, __RTL8211_REG_MDI_MODE, 0x300, 0x200);
    }

    /* Should perform reset for RTL8211 PHY after change MDIX */
    if (CA_E_OK == ret) {
        __eth_phy_reset(phy_dev);
    }

    return ret;
}

static ca_status_t  __eth_phy_rtl8211_mdix_get(ca_uint8_t phy_dev, aal_phy_mdi_mode_t *mdix_mode)
{
    ca_uint16 data16 = 0;

    if (CA_E_OK != __eth_phy_regfield_read(phy_dev, __RTL8211_REG_MDI_MODE, 0, 16, &data16)) {
        return CA_E_INTERNAL;
    }

    if (data16 & (1 << __RTL8211_REG_MDI_MODE_BIT_MDI_AUTO)) {/* Manual config */
        if (data16 & (1 << __RTL8211_REG_MDI_MODE_BIT_MDI_FORCE))
            *mdix_mode = AAL_PHY_MDI_MDI;
        else
            *mdix_mode = AAL_PHY_MDI_MDIX;
    }
    else {
        *mdix_mode = AAL_PHY_MDI_AUTO;
    }

    return CA_E_OK;
}

//#define __RTL8211E_REG_MDI_MODE                       (0x10)
//#define __RTL8211E_REG_MDI_MODE_BIT_MDI_AUTO          (6)
//#define __RTL8211E_REG_MDI_MODE_BIT_MDI_FORCE         (5)

static ca_status_t __eth_phy_rtl8211_init(ca_uint8_t phy_dev)
{
    ca_status_t ret    = CA_E_OK;

    ret =__eth_phy_rtl8211_int_mask_all(phy_dev);
    if (CA_E_OK != ret) {
        return CA_E_ERROR;
    }

    ret =__eth_phy_rtl8211_int_status_clr(phy_dev);
    if (CA_E_OK != ret) {
        return CA_E_ERROR;
    }

    ret = __eth_phy_rtl8211_int_en_set(phy_dev, PHY_INTR_LINK_CHANGE);
    if (CA_E_OK != ret) {
        return CA_E_ERROR;
    }

#if 0//98f ne mp: this will affect 98f EEE function
    /* select extension page */
    __eth_phy_regfield_write(phy_dev, 31, 0, 16, __RTL8211_REG_PAGE_LCR);

    __eth_phy_regfield_write(phy_dev, 0x10, 0, 16, 0x6251); /* Link Act LED */
    __eth_phy_regfield_write(phy_dev, 0x11, 0, 16, 0x0); /* disable EEE LED */

    /* back to page 0 */
    __eth_phy_regfield_write(phy_dev, 31, 0, 16, 0);
#endif//98f ne mp: this will affect 98f EEE function

    return CA_E_OK;

}


static ca_status_t  __eth_phy_rtl8214_int_mask_all(ca_uint8 phy_dev)
{
/*
    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, __RTL8201F_REG_PAGE_INTR_EN);
    __eth_phy_regfield_write(phy_dev, __RTL8201F_REG_INTR_EN, 0, 16, 0);
    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, 0);
*/
    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8214_int_en_set(ca_uint8 phy_dev, ca_uint16_t data)
{
/*
    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, __RTL8201F_REG_PAGE_INTR_EN);

    if (data & PHY_INTR_LINK_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_LINK,
                           1,
                           1);
    }

    if (data & PHY_INTR_DUPLEX_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_DUPLEX,
                           1,
                           1);
    }

    if (data & PHY_INTR_SPEED_CHANGE) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_LINK,
                           1,
                           1);
    }

    if (data & PHY_INTR_AN_FAIL) {
        __eth_phy_regfield_write(phy_dev,
                           __RTL8201F_REG_INTR_EN,
                           __RTL8201F_REG_INTR_EN_BIT_AN_ERR,
                           1,
                           1);
    }

    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, 0);
*/
    return CA_E_OK;
}

static ca_status_t  __eth_phy_rtl8214_int_en_get(ca_uint8 phy_dev, ca_uint16_t *data)
{
    /*
    ca_uint16_t data16 = 0;

    __eth_phy_regfield_write(phy_dev, __RTL8201F_REG_PAGE_SEL, 0, 16, __RTL8201F_REG_PAGE_INTR_EN);
    __eth_phy_regfield_read(phy_dev, __RTL8201F_REG_INTR_EN, 0, 16, &data16);

    if (data16 & (0x1 << __RTL8201F_REG_INTR_EN_BIT_LINK)) {
        *data |= PHY_INTR_LINK_CHANGE;
        *data |= PHY_INTR_SPEED_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_EN_BIT_DUPLEX)) {
        *data |= PHY_INTR_DUPLEX_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_EN_BIT_AN_ERR)) {
        *data |= PHY_INTR_AN_FAIL;
    }

    __eth_phy_regfield_write(phy_dev,__RTL8201F_REG_PAGE_SEL, 0, 16, 0);
    */
    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8214_int_status_get(ca_uint8 phy_dev, ca_uint16_t *data)
{
    /*
    ca_uint16_t data16 = 0;

    __eth_phy_regfield_read(phy_dev, __RTL8201F_REG_INTR_STATUS, 0, 16, &data16);

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_LINK)) {
        *data |= PHY_INTR_LINK_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_SPEED)) {
        *data |= PHY_INTR_SPEED_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_DUPLEX)) {
        *data |= PHY_INTR_DUPLEX_CHANGE;
    }

    if (data16 & (0x1 << __RTL8201F_REG_INTR_STATUS_BIT_AN_ERR)) {
        *data |= PHY_INTR_AN_FAIL;
    }
    */
    return CA_E_OK;

}

static ca_status_t  __eth_phy_rtl8214_int_status_clr(ca_uint8 phy_dev)
{
    /*
    ca_uint16 data16 = 0;

    __eth_phy_regfield_read(phy_dev, __RTL8201F_REG_INTR_STATUS, 0, 16, &data16);
    */
    return CA_E_OK;
}

ca_status_t __eth_phy_rtl8214_init(ca_uint8_t phy_dev)
{
    /* From TaiWan team and Realtek
    Reg 29: 0x6602
    Reg 30: 0x84D7
    Reg 29: 0x6601
    Reg 30: 0x0540
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0xF994
    Reg 29: 0x6601
    Reg 30: 0x0541
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0x2DA3
    Reg 29: 0x6601
    Reg 30: 0x0542
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0x3960
    Reg 29: 0x6601
    Reg 30: 0x0543
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0x9728
    Reg 29: 0x6601
    Reg 30: 0x0544
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0xF83F
    Reg 29: 0x6601
    Reg 30: 0x0545
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0x9D85
    Reg 29: 0x6601
    Reg 30: 0x0423
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x6602
    Reg 30: 0xD810
    Reg 29: 0x6601
    Reg 30: 0x0424
    Reg 29: 0x6600
    Reg 30: 0x00C0

    Reg 29: 0x1D11
    Reg 30: 0x1506

    Reg 29: 0x1D12
    Reg 30: 0x0800

    Reg 29: 0x6602
    Reg 30: 0xC3FA
    Reg 29: 0x6601
    Reg 30: 0x002E
    Reg 29: 0x6600
    Reg 30: 0x00C0
    */
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x84D7);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0540);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);

    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0xF994);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0541);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);

    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x2DA3);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0542);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);

    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x3960);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0543);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);

    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x9728);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0544);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);

    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0xF83F);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0545);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x9D85);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 0 , 0, 16, 0x0423);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0xD810);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0424);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x1D11);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x1506);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x1D12);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x0800);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6602);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0xC3FA);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6601);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x002E);
    __eth_phy_regfield_write(phy_dev, 29, 0, 16, 0x6600);
    __eth_phy_regfield_write(phy_dev, 30, 0, 16, 0x00C0);

    return CA_E_OK;
}
/*******************************************************************************/
/*                 Ethernet PHY Driver control block                            */
/*******************************************************************************/

phy_dev_t  *p_eth_phy_dev[ETH_PHY_NUM_MAX] = {0};
phy_dev_t __phy_rtl_8201f_dev ={
    "RTL8201F",
    PHY_OUI_RTK,
    PHY_MODEL_RTL8201F,
    {
         __eth_phy_rtl8201f_init,
         __eth_phy_reset,
         __eth_phy_auto_neg_restart,
         __eth_phy_power_set,
         __eth_phy_power_get,
         __eth_phy_auto_neg_set,
         __eth_phy_auto_neg_get,
         __eth_phy_ability_adv_set,
         __eth_phy_ability_adv_get,
         __eth_phy_partner_ability_adv_get,
         __eth_phy_link_status_get,
         __eth_phy_speed_set,
         __eth_phy_speed_get,
         __eth_phy_duplex_set,
         __eth_phy_duplex_get,
         __eth_phy_flow_ctrl_set,
         __eth_phy_flow_ctrl_get,
         __eth_phy_rtl8201f_mdix_set,
         __eth_phy_rtl8201f_mdix_get,
         __eth_phy_loopback_set,
         __eth_phy_loopback_get,
         __eth_phy_rtl8201f_int_mask_all,
         __eth_phy_rtl8201f_int_en_set,
         __eth_phy_rtl8201f_int_en_get,
         __eth_phy_rtl8201f_int_status_get,
         __eth_phy_rtl8201f_int_status_clr
    }
};

phy_dev_t __phy_rtl_8211_dev ={
    "RTL8211",
    PHY_OUI_RTK,
    PHY_MODEL_RTL8211,
    {
         __eth_phy_rtl8211_init,
         __eth_phy_reset,
         __eth_phy_auto_neg_restart,
         __eth_phy_power_set,
         __eth_phy_power_get,
         __eth_phy_auto_neg_set,
         __eth_phy_auto_neg_get,
         __eth_phy_ability_adv_set,
         __eth_phy_ability_adv_get,
         __eth_phy_partner_ability_adv_get,
         __eth_phy_rtl8211_link_status_get,
         __eth_phy_speed_set,
         __eth_phy_speed_get,
         __eth_phy_duplex_set,
         __eth_phy_duplex_get,
         __eth_phy_flow_ctrl_set,
         __eth_phy_flow_ctrl_get,
         __eth_phy_rtl8211_mdix_set,
         __eth_phy_rtl8211_mdix_get,
         __eth_phy_loopback_set,
         __eth_phy_loopback_get,
         __eth_phy_rtl8211_int_mask_all,
         __eth_phy_rtl8211_int_en_set,
         __eth_phy_rtl8211_int_en_get,
         __eth_phy_rtl8211_int_status_get,
         __eth_phy_rtl8211_int_status_clr
    }
};
phy_dev_t __phy_rtl_8214_dev ={
    "RTL8214",
    PHY_OUI_RTK,
    PHY_MODEL_RTL8214,
    {
         __eth_phy_rtl8214_init,
         __eth_phy_reset,
         __eth_phy_auto_neg_restart,
         __eth_phy_power_set,
         __eth_phy_power_get,
         __eth_phy_auto_neg_set,
         __eth_phy_auto_neg_get,
         __eth_phy_ability_adv_set,
         __eth_phy_ability_adv_get,
         __eth_phy_partner_ability_adv_get,
         __eth_phy_link_status_get,
         __eth_phy_speed_set,
         __eth_phy_speed_get,
         __eth_phy_duplex_set,
         __eth_phy_duplex_get,
         __eth_phy_flow_ctrl_set,
         __eth_phy_flow_ctrl_get,
         __eth_phy_mdix_set,
         __eth_phy_mdix_get,
         __eth_phy_loopback_set,
         __eth_phy_loopback_get,
         __eth_phy_rtl8214_int_mask_all,
         __eth_phy_rtl8214_int_en_set,
         __eth_phy_rtl8214_int_en_get,
         __eth_phy_rtl8214_int_status_get,
         __eth_phy_rtl8214_int_status_clr
    }
};
/**************************************************************/
/*             Export Etherent PHY APIs                       */
/**************************************************************/

ca_status_t aal_eth_phy_init(ca_device_id_t  device_id, ca_uint8_t phy_addr)
{
    ca_uint16_t  phy_id1   = 0;
    ca_uint16_t  phy_id2   = 0;
    ca_uint16_t  phy_oui   = 0;
    ca_uint16_t  phy_model = 0;
    ca_status_t  ret       = CA_E_OK;

#if 0//CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d) %s\n", __func__, __LINE__, __FILE__);
    ca_printf("\tphy_addr(%#x)\tETH_PHY_NUM_MAX(%#x)\n", phy_addr, ETH_PHY_NUM_MAX);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if (phy_addr >= ETH_PHY_NUM_MAX) {
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d) %s\n", __func__, __LINE__, __FILE__);
        ca_printf("\tphy_addr(%#x) >= ETH_PHY_NUM_MAX(%#x)\n", phy_addr, ETH_PHY_NUM_MAX);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        return CA_E_PARAM;
    }

    (void)aal_mdio_speed_set(device_id, phy_addr, __MDIO_CLOCK_DEF);

    ret = __eth_phy_regfield_read(phy_addr, PHY_REG_ID_1, 0, 16, &phy_id1);

    if (ret != CA_E_OK) {
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d) %s\n", __func__, __LINE__, __FILE__);
        ca_printf("\tread phy_addr = %#x for PHY_REG_ID_1 fail (phy_id1: %#x)\n", phy_addr, phy_id1);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        return CA_E_ERROR;
    }

    ret = __eth_phy_regfield_read(phy_addr, PHY_REG_ID_2, 0, 16, &phy_id2);

    if (ret != CA_E_OK) {
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d) %s\n", __func__, __LINE__, __FILE__);
        ca_printf("\tread phy_addr = %#x for PHY_REG_ID_2 fail (phy_id2: %#x)\n", phy_addr, phy_id2);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        return CA_E_ERROR;
    }

    if ((phy_id1 == 0xFFFF) && (phy_id2 == 0xFFFF)) {
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d) %s\n", __func__, __LINE__, __FILE__);
        ca_printf("\tread phy_addr = 0x%x for PHY_REG_ID_1 fail (phy_id1: %#x)\n", phy_addr, phy_id1);
        ca_printf("\tread phy_addr = 0x%x for PHY_REG_ID_2 fail (phy_id2: %#x)\n", phy_addr, phy_id2);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        return CA_E_NOT_FOUND;
    }

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d) %s\n", __func__, __LINE__, __FILE__);
        ca_printf("\t\tread phy_addr = 0x%x for PHY_REG_ID_1 success (phy_id1: %#x)\n", phy_addr, phy_id1);
        ca_printf("\t\tread phy_addr = 0x%x for PHY_REG_ID_2 success (phy_id2: %#x)\n", phy_addr, phy_id2);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    phy_oui   = (((phy_id1 & 0x3ff) << 6)|((phy_id2 & 0xfc00) >> 10));
    phy_model = ((phy_id2 & 0x03f0) >> 4);
#if 0//CONFIG_98F_UBOOT_NE_DBG//#ifdef DEBUG
    printf("eth PHY read PHY %u IDs(%#x %#x): phy_oui %#x, phy_model %#x .\n",
        phy_addr, phy_id1, phy_id2, phy_oui, phy_model);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if(phy_oui == PHY_OUI_RTK) { /* RealTek Ethernet PHY */
        switch (phy_model) {
            case PHY_MODEL_RTL8201F:
                p_eth_phy_dev[phy_addr] = &__phy_rtl_8201f_dev;
                printf("detect Eth PHY RTL8201F at PHY addr %u\n", phy_addr);
                break;

            case PHY_MODEL_RTL8211:
            case PHY_MODEL_RTL8211E:
            case PHY_MODEL_RTL8211_ASIC:
                p_eth_phy_dev[phy_addr] = &__phy_rtl_8211_dev;
                printf("detect Eth PHY RTL8211 at PHY addr %u\n", phy_addr);
                break;

            case PHY_MODEL_RTL8198F: 
                p_eth_phy_dev[phy_addr] = &__phy_rtl_8211_dev; /* temp */
#if CONFIG_98F_UBOOT_NE_DBG
                printf("detect Eth PHY RTL8198F at PHY addr %u\n", phy_addr);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
                break;

            case PHY_MODEL_RTL8214:
            case PHY_MODEL_RTL8214C:
                p_eth_phy_dev[phy_addr] = &__phy_rtl_8214_dev;
                printf("detect Eth PHY RTL8214(C) at PHY addr %u\n", phy_addr);
                break;

            default:
                p_eth_phy_dev[phy_addr] = NULL;
                printf("!!!No driver support for PHY at addr %u\n", phy_addr);
                return CA_E_NOT_SUPPORT;
        }
    }else {
        // TODO: support other vendor's Eth PHY here

        printf("Detect unkown eth PHY at addr %u: ID(%#x %#x), phy_oui %#x, phy_model %#x .\n",
            phy_addr, phy_id1, phy_id2, phy_oui, phy_model);

        return CA_E_NOT_SUPPORT;
    }

////the pcs driver here that changes the page.4, reg.16 & reg.17
    if (p_eth_phy_dev[phy_addr] && p_eth_phy_dev[phy_addr]->drv.init)
        p_eth_phy_dev[phy_addr]->drv.init(phy_addr);

    return CA_E_OK;

}

#ifndef CONFIG_CORTINA_BOARD_FPGA

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
#define __FOR_ALL_INTERNAL_PHY_DO(phy_addr) \
    for (phy_addr = 1; phy_addr < 5; phy_addr++)
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define __FOR_ALL_INTERNAL_PHY_DO(phy_addr) \
    for (phy_addr = 1; phy_addr <= 1; phy_addr++)
#elif defined(CONFIG_ARCH_RTL8198F)
#define __FOR_ALL_INTERNAL_PHY_DO(port_id) \
    for (port_id = 0; port_id < 5; port_id++)
#else
#define __FOR_ALL_INTERNAL_PHY_DO(phy_addr) \
    for (phy_addr = 1; phy_addr < 1; phy_addr++)
#endif

ca_status_t aal_internal_phy_init(ca_device_id_t  device_id)
{
#if defined(CONFIG_ARCH_RTL8198F)
    ca_uint8_t  port_id = 0, phy_addr;
    ca_uint32_t default_mdio[] = {
        8, 1, 2, 3, 4, 31, 31, 31
    };

    __FOR_ALL_INTERNAL_PHY_DO(port_id) {
        phy_addr = default_mdio[port_id];

        /* add internal phy patch here */
    }
#else
#if 0//98f: make sure not enter this part of G3 G-PHY patch
    ca_uint8_t  phy_addr = 0;
    ca_uint32_t data     = 0;
	int reg;

    __FOR_ALL_INTERNAL_PHY_DO(phy_addr) {
        ASIC_PHY_MDIO_WRITE(phy_addr, 31, 0x0BC6);
        data = ASIC_PHY_MDIO_READ(phy_addr, 31);
        ca_printf("phy %d reg 31 = %#x\r\n", phy_addr, data);

        ASIC_PHY_MDIO_WRITE(phy_addr, 16, 0x0053);
        data = ASIC_PHY_MDIO_READ(phy_addr, 16);
        ca_printf("phy %d reg 16 = %#x\r\n", phy_addr, data);

        ASIC_PHY_MDIO_WRITE(phy_addr, 18, 0x4003);
        data = ASIC_PHY_MDIO_READ(phy_addr, 18);
        ca_printf("phy %d reg 18 = %#x\r\n", phy_addr, data);

        ASIC_PHY_MDIO_WRITE(phy_addr, 22, 0x7e01);
        data = ASIC_PHY_MDIO_READ(phy_addr, 22);
        ca_printf("phy %d reg 22 = %#x\r\n", phy_addr, data);

        ASIC_PHY_MDIO_WRITE(phy_addr, 31, 0x0A42);
        data = ASIC_PHY_MDIO_READ(phy_addr, 31);
        ca_printf("phy %d reg 31 = %#x\r\n", phy_addr, data);

        ASIC_PHY_MDIO_WRITE(phy_addr, 31, 0x0A40);
        data = ASIC_PHY_MDIO_READ(phy_addr, 31);
        ca_printf("phy %d reg 31 = %#x\r\n", phy_addr, data);

        ASIC_PHY_MDIO_WRITE(phy_addr,  0, 0x1140);
        data = ASIC_PHY_MDIO_READ(phy_addr, 0);
        ca_printf("phy %d reg 0 = %#x\r\n", phy_addr, data);
    }

#if 0//yocto
    __DELAY_MS(100);

    /* From: ASIC Team Joe Liu:
        For the GPHY work around, please check the following GPHY register (apply to all 4 ports),
        1.	Page 0xc40, rg18, bit[0], if it's 1 then read the following interrupt register, if it's then then you don't need to do anything.
        2.	Page 0xb90, rg19, you should see 0x10, this bit will be automatically cleared upon the read,
        please read it again to confirm it's 0x0 then the corresponding port should have the clock switched back to the normal clock and start to work.
    */
#else//sd1 uboot for 98f
    ca_mdelay(100);
#endif

    __FOR_ALL_INTERNAL_PHY_DO(phy_addr) {
        /* Check clk_fail_sys */
        ASIC_PHY_MDIO_WRITE(phy_addr, 31, 0xC40);
        data = ASIC_PHY_MDIO_READ(phy_addr, 18);
        if (data & 1) {
            ca_printf("%s: phy_addr=%d, GPHY register 18 bit[0] is not 0, need work-around!\n", __func__, phy_addr);
            /* Clear clock fail interrupt */
            ASIC_PHY_MDIO_WRITE(phy_addr, 31, 0xB90);
            data = ASIC_PHY_MDIO_READ(phy_addr, 19);
            ca_printf("%s: phy_addr=%d, read register 19, value=0x%x\n", __func__, phy_addr);
        }
    }

    /* Recover to Page0 */
    ASIC_PHY_MDIO_WRITE(phy_addr, 31, 0xa40);
#endif//98f: make sure not enter this part of G3 G-PHY patch

#endif

    return CA_E_OK;
}

#if 0
/* initialize port 6/7 XFI interfaces */
#define XFI_STRIDE	0x1000
#define SXGMII_STRIDE	0x800
#define XGE_MISC_STRIDE	0x100
void aal_reset_xfi_pcs(aal_phy_sxgmii_pcs_control_index_t pcs_index)
{
	NI_HV_GLB_INTF_RST_CONFIG_t	intf_rst_config;

        /* reset interface again */
        intf_rst_config.wrd = CA_NE_REG_READ(NI_HV_GLB_INTF_RST_CONFIG);
        if (pcs_index == AAL_SXGMII_PCS_CONTROL_INDEX_0)
                intf_rst_config.bf.pcs_rst_p6 = 1;
#if defined(CONFIG_ARCH_CORTINA_G3)
        else
                intf_rst_config.bf.pcs_rst_p7 = 1;
#endif
        CA_NE_REG_WRITE(intf_rst_config.wrd, NI_HV_GLB_INTF_RST_CONFIG);

        mdelay(10);

        intf_rst_config.wrd = CA_NE_REG_READ(NI_HV_GLB_INTF_RST_CONFIG);
        if (pcs_index == AAL_SXGMII_PCS_CONTROL_INDEX_0)
                intf_rst_config.bf.pcs_rst_p6 = 0;
#if defined(CONFIG_ARCH_CORTINA_G3)
        else
                intf_rst_config.bf.pcs_rst_p7 = 0;
#endif
        CA_NE_REG_WRITE(intf_rst_config.wrd, NI_HV_GLB_INTF_RST_CONFIG);

        mdelay(1);
}

/* DFE adaption */
static void aal_xfi_do_dfe_adaption(aal_phy_sxgmii_pcs_control_index_t pcs_index)
{
	int xfi_offset, sxgmii_offset;
	ca_uint32_t tap0, vthp, vthn, tap1_even, tap1_odd;
	XFI_RG13_t	rg13;
	XFI_RG43_t	rg43;
	XFI_RG0F_t	rg0f;
	XFI_RG4A_t	rg4a;
	XFI_RG20_t	rg20;
	XFI_RG2E_t	rg2e;
	XFI_RG23_t	rg23;
	XFI_RG46_t	rg46;
	NI_HV_SXGMII_PCS_PCSREG32770_t	pcs_reg32770;

	xfi_offset = pcs_index * XFI_STRIDE;
	sxgmii_offset = pcs_index * SXGMII_STRIDE;

	/* enable tap0/vth/tap1 adaption */
	rg13.wrd = CA_NE_XFI_REG_READ(XFI_RG13 + xfi_offset);
	rg13.bf.reg0_load_in_init = 0x3c;
	CA_NE_XFI_REG_WRITE(rg13.wrd, XFI_RG13 + xfi_offset);

	rg43.wrd = CA_NE_XFI_REG_READ(XFI_RG43 + xfi_offset);
#if defined(CONFIG_ARCH_CORTINA_G3)
	rg43.bf.reg0_dfe_adapt_eqen = 1;
#endif
	CA_NE_XFI_REG_WRITE(rg43.wrd, XFI_RG43 + xfi_offset);

	/* wait 1ms */
	mdelay(1);

	/* read initial value of tap0/vth/tap1 */
	rg0f.wrd = CA_NE_XFI_REG_READ(XFI_RG0F + xfi_offset);
	rg0f.bf.reg0_coef_sel = 0;
	CA_NE_XFI_REG_WRITE(rg0f.wrd, XFI_RG0F + xfi_offset);

	rg4a.wrd = CA_NE_XFI_REG_READ(XFI_RG4A + xfi_offset);
	tap0 = rg4a.bf.S0_coef_data;

	rg0f.wrd = CA_NE_XFI_REG_READ(XFI_RG0F + xfi_offset);
	rg0f.bf.reg0_coef_sel = 0xc;
	CA_NE_XFI_REG_WRITE(rg0f.wrd, XFI_RG0F + xfi_offset);

	rg4a.wrd = CA_NE_XFI_REG_READ(XFI_RG4A + xfi_offset);
	vthp = rg4a.bf.S0_coef_data & 0x7;
	vthn = (rg4a.bf.S0_coef_data & 0x38) >> 3;

	rg0f.wrd = CA_NE_XFI_REG_READ(XFI_RG0F + xfi_offset);
	rg0f.bf.reg0_coef_sel = 1;
	CA_NE_XFI_REG_WRITE(rg0f.wrd, XFI_RG0F + xfi_offset);

	rg4a.wrd = CA_NE_XFI_REG_READ(XFI_RG4A + xfi_offset);
	tap1_even = rg4a.bf.S0_coef_data & 0x7f;

	rg0f.wrd = CA_NE_XFI_REG_READ(XFI_RG0F + xfi_offset);
	rg0f.bf.reg0_coef_sel = 6;
	CA_NE_XFI_REG_WRITE(rg0f.wrd, XFI_RG0F + xfi_offset);

	rg4a.wrd = CA_NE_XFI_REG_READ(XFI_RG4A + xfi_offset);
	tap1_odd = rg4a.bf.S0_coef_data & 0x7f;

	printf("%s: tap0=0x%x, vthp=0x%x, vthn=0x%x, tap1_even=0x%x, tap1_odd=0x%x\n",
		__func__, tap0, vthp, vthn, tap1_even, tap1_odd);

	/* write initial values to tap0/vth/tap1 init */
	rg20.wrd = CA_NE_XFI_REG_READ(XFI_RG20 + xfi_offset);
	rg20.bf.reg0_tap0_init = tap0;
	CA_NE_XFI_REG_WRITE(rg20.wrd, XFI_RG20 + xfi_offset);

	rg2e.wrd = CA_NE_XFI_REG_READ(XFI_RG2E + xfi_offset);
	rg2e.bf.reg0_vthp_init = vthp;
	rg2e.bf.reg0_vthn_init = vthn;
	CA_NE_XFI_REG_WRITE(rg2e.wrd, XFI_RG2E + xfi_offset);

	rg23.wrd = CA_NE_XFI_REG_READ(XFI_RG23 + xfi_offset);
	rg23.bf.reg0_tap1_init_even = tap1_even;
	rg23.bf.reg0_tap1_init_odd = tap1_odd;
	CA_NE_XFI_REG_WRITE(rg23.wrd, XFI_RG23 + xfi_offset);

	/* disable tap0/vth/tap1 adaption */
	rg13.wrd = CA_NE_XFI_REG_READ(XFI_RG13 + xfi_offset);
	rg13.bf.reg0_load_in_init = 0x7f;
	CA_NE_XFI_REG_WRITE(rg13.wrd, XFI_RG13 + xfi_offset);

	rg43.wrd = CA_NE_XFI_REG_READ(XFI_RG43 + xfi_offset);
#if defined(CONFIG_ARCH_CORTINA_G3)
	rg43.bf.reg0_dfe_adapt_eqen = 0;
#endif
	CA_NE_XFI_REG_WRITE(rg43.wrd, XFI_RG43 + xfi_offset);

	mdelay(1);

	/* toggle RX_EN to perform foreground & DCVS offset calibration again */
	rg46.wrd = CA_NE_XFI_REG_READ(XFI_RG46 + xfi_offset);
	rg46.bf.RX_EN = 0;
	CA_NE_XFI_REG_WRITE(rg46.wrd, XFI_RG46 + xfi_offset);

	mdelay(10);

	rg46.wrd = CA_NE_XFI_REG_READ(XFI_RG46 + xfi_offset);
	rg46.bf.RX_EN = 1;
	CA_NE_XFI_REG_WRITE(rg46.wrd, XFI_RG46 + xfi_offset);

	mdelay(10);

	/* do PCS tx/rx reset */
	pcs_reg32770.wrd = CA_NE_REG_READ(NI_HV_SXGMII_PCS_PCSREG32770 + sxgmii_offset);
	pcs_reg32770.wrd |= 0x1000;
	CA_NE_REG_WRITE(pcs_reg32770.wrd, NI_HV_SXGMII_PCS_PCSREG32770 + sxgmii_offset);
	mdelay(10);

	pcs_reg32770.wrd &= 0xffffefff;
	CA_NE_REG_WRITE(pcs_reg32770.wrd, NI_HV_SXGMII_PCS_PCSREG32770 + sxgmii_offset);
	mdelay(10);
}

ca_status_t aal_xfi_set_speed(ca_device_id_t device_id, aal_phy_sxgmii_pcs_control_index_t pcs_index, aal_phy_speed_mode_t speed)
{
	int i, xfi_offset, sxgmii_offset, xge_offset;
	NI_HV_SXGMII_PCS_PCSSTS1_t	pcs_sts;
	NI_HV_SXGMII_SFP_PCS_STATUS_t	sfp_sts;
	NI_HV_GLB_INTF_RST_CONFIG_t     intf_rst_config;


	printf("%s: pcs_index=%d, speed=%d\n", __func__, pcs_index, speed);

	if (speed != AAL_PHY_SPEED_10000 && speed != AAL_PHY_SPEED_1000) {
		return CA_E_PARAM;
	}

	xfi_offset = pcs_index * XFI_STRIDE;
	sxgmii_offset = pcs_index * SXGMII_STRIDE;
	xge_offset = pcs_index * XGE_MISC_STRIDE;

	/* for 1Gbps */
	intf_rst_config.wrd = CA_NE_REG_READ(NI_HV_GLB_INTF_RST_CONFIG);
	if (pcs_index == AAL_SXGMII_PCS_CONTROL_INDEX_0)
		intf_rst_config.bf.pcs_rst_p6 = 0;
#if defined(CONFIG_ARCH_CORTINA_G3)
	else
		intf_rst_config.bf.pcs_rst_p7 = 0;
#endif
	CA_NE_REG_WRITE(intf_rst_config.wrd, NI_HV_GLB_INTF_RST_CONFIG);

	CA_NE_REG_WRITE(0x00001800, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
	CA_NE_REG_WRITE(0x00003800, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);

	if (speed == AAL_PHY_SPEED_10000) {
		CA_NE_REG_WRITE(0x000000ff, NI_HV_XGE_MISC_CONFIG + xge_offset);
		CA_NE_REG_WRITE(0x80000000, NI_HV_SXGMII_SXGRA_PCSCTL + sxgmii_offset);
		udelay(100);
		CA_NE_REG_WRITE(0x00000000, NI_HV_SXGMII_SXGRA_PCSCTL + sxgmii_offset);
	}
	else {
		CA_NE_REG_WRITE(0x800000ff, NI_HV_XGE_MISC_CONFIG + xge_offset);
		CA_NE_REG_WRITE(0x80000001, NI_HV_SXGMII_SXGRA_PCSCTL + sxgmii_offset);
		udelay(100);
		CA_NE_REG_WRITE(0x00000001, NI_HV_SXGMII_SXGRA_PCSCTL + sxgmii_offset);
	}

	/* XFI0/1 ANx */
	CA_NE_XFI_REG_WRITE(0x0000FC20, XFI_RG01 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00001C47, XFI_RG02 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000C71D, XFI_RG03 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000AAA8, XFI_RG04 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00005523, XFI_RG05 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00006003, XFI_RG06 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000FFFF, XFI_RG07 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000326A, XFI_RG08 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00008B40, XFI_RG09 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000180, XFI_RG0A + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00004003, XFI_RG0B + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000017F, XFI_RG0C + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000087A6, XFI_RG0E + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000003CD, XFI_RG0F + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000021A, XFI_RG10 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000C00, XFI_RG11 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000FC48, XFI_RG12 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000000FF, XFI_RG13 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG14 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000444, XFI_RG15 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00004444, XFI_RG16 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000082F, XFI_RG17 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000082F, XFI_RG18 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000082F, XFI_RG19 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000B80, XFI_RG1A + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG1B + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG1C + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000430, XFI_RG1D + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000401, XFI_RG1E + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG1F + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000001F, XFI_RG20 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00001F24, XFI_RG21 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000F960, XFI_RG22 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG23 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00001F93, XFI_RG24 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000F400, XFI_RG25 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000003FF, XFI_RG26 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000027D0, XFI_RG27 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000000F, XFI_RG28 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000FC9F, XFI_RG29 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00004000, XFI_RG2A + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00003FF2, XFI_RG2B + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00007D8E, XFI_RG2C + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00004010, XFI_RG2D + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000E1EC, XFI_RG2E + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000FE00, XFI_RG2F + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00008924, XFI_RG30 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000C190, XFI_RG31 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000007EB, XFI_RG32 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x000028B8, XFI_RG34 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000001, XFI_RG35 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000002, XFI_RG36 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00001299, XFI_RG37 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000000D, XFI_RG3A + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0C001000, XFI_RG40 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x09C00001, XFI_RG41 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x0000003F, XFI_RG42 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000405, XFI_RG43 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00007E00, XFI_RG44 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG45 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x003F0908, XFI_RG46 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0xB8AA00BE, XFI_RG47 + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000003, XFI_RG48 + xfi_offset);
#if defined(CONFIG_ARCH_CORTINA_G3)
	CA_NE_XFI_REG_WRITE(0x0000FFFF, XFI_RG49 + xfi_offset);
#endif

	CA_NE_XFI_REG_WRITE(0x000000C8, XFI_RG4A + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000000, XFI_RG4B + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00005007, XFI_RG4C + xfi_offset);
	CA_NE_XFI_REG_WRITE(0x00000005, XFI_RG4D + xfi_offset);

        /* change XGRA settings */
	if (speed == AAL_PHY_SPEED_10000) {
		CA_NE_XFI_REG_WRITE(0x000060F0, XFI_RG00 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00004806, XFI_RG01 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00001447, XFI_RG02 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00003044, XFI_RG0D + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x000002FA, XFI_RG10 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x0000BC00, XFI_RG11 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00007C48, XFI_RG12 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x0000FF24, XFI_RG21 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x0000C5EC, XFI_RG2E + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00001F08, XFI_RG33 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00004210, XFI_RG38 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00004210, XFI_RG39 + xfi_offset);
	}
	else {
		CA_NE_XFI_REG_WRITE(0x000020F0, XFI_RG00 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00000060, XFI_RG0D + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00002682, XFI_RG33 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00005010, XFI_RG38 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00005010, XFI_RG39 + xfi_offset);
	}
	udelay(100);

	/* change XGRA settings */
	if (speed == AAL_PHY_SPEED_10000) {
		CA_NE_REG_WRITE(0x00001d00, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
		CA_NE_REG_WRITE(0x00003d00, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
		CA_NE_REG_WRITE(0x00003f00, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
		udelay(100);

		CA_NE_XFI_REG_WRITE(0x00007f00, XFI_RG44 + xfi_offset);
		udelay(10);
		CA_NE_XFI_REG_WRITE(0x002f0908, XFI_RG46 + xfi_offset);
		udelay(10);
		CA_NE_XFI_REG_WRITE(0x00270908, XFI_RG46 + xfi_offset);
		udelay(10);
		CA_NE_XFI_REG_WRITE(0x00230908, XFI_RG46 + xfi_offset);
		udelay(10);
		CA_NE_XFI_REG_WRITE(0x002b0908, XFI_RG46 + xfi_offset);
		udelay(10);
		CA_NE_XFI_REG_WRITE(0x002f0908, XFI_RG46 + xfi_offset);
		udelay(10);
		CA_NE_XFI_REG_WRITE(0x003f0908, XFI_RG46 + xfi_offset);
		udelay(100);
	}
	else {

		CA_NE_REG_WRITE(0x00001d00, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
		CA_NE_XFI_REG_WRITE(0x003e8108, XFI_RG46 + xfi_offset);
		CA_NE_REG_WRITE(0x00003d00, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
		CA_NE_XFI_REG_WRITE(0x00007f00, XFI_RG44 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x003e8108, XFI_RG46 + xfi_offset);
		CA_NE_REG_WRITE(0x00003f00, NI_HV_SXGMII_SXGRA_PCSSDSMISCCTL + sxgmii_offset);
		CA_NE_XFI_REG_WRITE(0x003e8108, XFI_RG46 + xfi_offset);
		udelay(100);

		CA_NE_XFI_REG_WRITE(0x003f0908, XFI_RG46 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00370908, XFI_RG46 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x00330908, XFI_RG46 + xfi_offset);
		CA_NE_XFI_REG_WRITE(0x003b0908, XFI_RG46 + xfi_offset);
		udelay(100);
		CA_NE_XFI_REG_WRITE(0x003f0908, XFI_RG46 + xfi_offset);
	}

	/* reset interface again */
	aal_reset_xfi_pcs(pcs_index);
	mdelay(1);

	if (speed == AAL_PHY_SPEED_10000) {

		/* do DFE adaption */
		aal_xfi_do_dfe_adaption(pcs_index);
		mdelay(10);

		CA_NE_REG_WRITE(0x80000000, NI_HV_SXGMII_SXGRA_PCSCTL + sxgmii_offset);
		udelay(100);

		CA_NE_REG_WRITE(0xc010, NI_HV_XGE_MAC_CFG_COM + xge_offset);
		CA_NE_REG_WRITE(0x0010, NI_HV_XGE_MAC_CFG_COM + xge_offset);

		mdelay(100);

		/* check pcs link status */
		pcs_sts.wrd = CA_NE_REG_READ(NI_HV_SXGMII_PCS_PCSSTS1 + sxgmii_offset);
		printf("%s: 10G Link status PCS %d, pcs_sts.wrd=0x%x\n", __func__, pcs_index, pcs_sts.wrd);
	}
	else {
		CA_NE_REG_WRITE(0x1, NI_HV_SXGMII_SXGRA_PCSCTL + sxgmii_offset);
		udelay(100);

		CA_NE_REG_WRITE(0xc010, NI_HV_XGE_MAC_CFG_COM + xge_offset);
		CA_NE_REG_WRITE(0x0010, NI_HV_XGE_MAC_CFG_COM + xge_offset);
		mdelay(100);

		sfp_sts.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SFP_PCS_STATUS + sxgmii_offset);
		printf("%s: 1G Link status SFP %d, sfp_sts.wrd=0x%x\n", __func__, pcs_index, sfp_sts.wrd);
	}

	return CA_E_OK;
}

ca_status_t aal_xfi_do_hw_autoneg(ca_device_id_t device_id, aal_phy_sxgmii_pcs_control_index_t pcs_index)
{
	int i, sxgmii_offset;
	NI_HV_XGE_MAC_CFG_COM_t			mac_cfg_com;
	NI_HV_SXGMII_SXGRA_SXGHWANCTL_t		sxg_hw_an_ctl;
	NI_HV_SXGMII_SXGRA_SXGMISCCTL_t		sxg_misc_ctl;
	NI_HV_SXGMII_SXGRA_LINKSPEEDCTL_t	link_speed;
	NI_HV_SXGMII_SXGRA_SXG_INTENABLE_t	sxg_inte;
	NI_HV_SXGMII_SXGRA_SXG_INTERRUPT_t	sxg_int;

	/* set up XFI to 10G first */
	aal_xfi_set_speed(0, pcs_index, AAL_PHY_SPEED_10000);

	sxgmii_offset = pcs_index * SXGMII_STRIDE;

	printf("%s: Link status checking...\n", __func__);
	sxg_inte.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTENABLE + sxgmii_offset);
	sxg_inte.bf.linkup_intE = 1;
	CA_NE_REG_WRITE(sxg_inte.wrd, NI_HV_SXGMII_SXGRA_SXG_INTENABLE + sxgmii_offset);
	for (i = 0; i < 100; i++) {
		sxg_int.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
		if (sxg_int.bf.linkup_intI) {
			CA_NE_REG_WRITE(sxg_int.wrd, NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
			printf("%s: Link up detected!!!\n", __func__);
			break;
		}
		mdelay(100);
	}
	if (i == 100) {
		printf("%s: Link down!!!\n", __func__);
		return CA_E_ERROR;
	}

	printf("%s: Check Auto nego. request..\n", __func__);
	sxg_inte.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTENABLE + sxgmii_offset);
	sxg_inte.bf.ancfgwddet_intE = 1;
	CA_NE_REG_WRITE(sxg_inte.wrd, NI_HV_SXGMII_SXGRA_SXG_INTENABLE + sxgmii_offset);
        for (i = 0; i < 100; i++) {
                sxg_int.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
                if (sxg_int.bf.ancfgwddet_intI) {
			CA_NE_REG_WRITE(sxg_int.wrd, NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
                        printf("%s: Received Auto nego. request!!!\n", __func__);
                        break;
                }
                mdelay(100);
        }
	if (i == 100) {
		printf("%s: There is no Auto nego. request received!!!\n", __func__);
		//return CA_E_ERROR;
	}

	/* disable data path */
	mac_cfg_com.wrd = CA_NE_REG_READ(NI_HV_XGE_MAC_CFG_COM + sxgmii_offset);
	mac_cfg_com.bf.tx_rst = 1;
	mac_cfg_com.bf.rx_rst = 1;
	CA_NE_REG_WRITE(mac_cfg_com.wrd, NI_HV_XGE_MAC_CFG_COM + sxgmii_offset);

	printf("%s: Do HW Auto nego...\n", __func__);
	sxg_inte.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTENABLE + sxgmii_offset);
        sxg_inte.bf.hwancomplete_intE = 1;
	CA_NE_REG_WRITE(sxg_inte.wrd, NI_HV_SXGMII_SXGRA_SXG_INTENABLE + sxgmii_offset);

	sxg_hw_an_ctl.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXGHWANCTL + sxgmii_offset);
	sxg_hw_an_ctl.bf.linktmr = 0x4c4b4;
	sxg_hw_an_ctl.bf.hwanstart = 1;
	sxg_hw_an_ctl.bf.hwanenable = 1;
	CA_NE_REG_WRITE(sxg_hw_an_ctl.wrd, NI_HV_SXGMII_SXGRA_SXGHWANCTL + sxgmii_offset);

	for (i = 0; i < 100; i++) {
		sxg_int.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
		if (sxg_int.bf.hwancomplete_intI) {
			CA_NE_REG_WRITE(sxg_int.wrd, NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
			///TODO: debug
			printf("%s: before write sxg_int.wrd=0x%x\n", __func__, sxg_int.wrd);
			sxg_int.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXG_INTERRUPT + sxgmii_offset);
			printf("%s: after write sxg_int.wrd=0x%x\n", __func__, sxg_int.wrd);

			/* keep HW auto nego. enabled always */
			//sxg_hw_an_ctl.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXGHWANCTL + sxgmii_offset);
			//sxg_hw_an_ctl.bf.hwanenable = 0;
			//CA_NE_REG_WRITE(sxg_hw_an_ctl.wrd, NI_HV_SXGMII_SXGRA_SXGHWANCTL + sxgmii_offset);

			/* read link speed */
			link_speed.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_LINKSPEEDCTL + sxgmii_offset);
			printf("%s: auto neg complete=%d, link speed=%d\n", __func__,
					link_speed.bf.autoneg_complete, link_speed.bf.linkspeed);

			/* recenter the USXGMII elastic buffers */
			sxg_misc_ctl.wrd = CA_NE_REG_READ(NI_HV_SXGMII_SXGRA_SXGMISCCTL + sxgmii_offset);
			sxg_misc_ctl.bf.txgmfreinit = 1;
			sxg_misc_ctl.bf.txxgmfreinit = 1;
			sxg_misc_ctl.bf.rxgmfreinit = 1;
			sxg_misc_ctl.bf.rxxgmfreinit = 1;
			CA_NE_REG_WRITE(sxg_misc_ctl.wrd, NI_HV_SXGMII_SXGRA_SXGMISCCTL + sxgmii_offset);

			/* enable data path */
			mac_cfg_com.wrd = CA_NE_REG_READ(NI_HV_XGE_MAC_CFG_COM + sxgmii_offset);
			mac_cfg_com.bf.tx_rst = 0;
			mac_cfg_com.bf.rx_rst = 0;
			CA_NE_REG_WRITE(mac_cfg_com.wrd, NI_HV_XGE_MAC_CFG_COM + sxgmii_offset);

			printf("%s: HW Auto nego. completed!!!\n", __func__);
			break;
		}
		mdelay(100);
	}

	if (i == 100) {
		printf("%s: Auto nego. no completed!!!\n", __func__);

		//TODO: debug
		/* enable data path */
		mac_cfg_com.wrd = CA_NE_REG_READ(NI_HV_XGE_MAC_CFG_COM + sxgmii_offset);
		mac_cfg_com.bf.tx_rst = 0;
		mac_cfg_com.bf.rx_rst = 0;
		CA_NE_REG_WRITE(mac_cfg_com.wrd, NI_HV_XGE_MAC_CFG_COM + sxgmii_offset);

		return CA_E_ERROR;
	}

	return CA_E_OK;
}
EXPORT_SYMBOL(aal_xfi_do_hw_autoneg);

ca_status_t aal_xfi_init(ca_device_id_t  device_id)
{
	aal_xfi_set_speed(0, AAL_SXGMII_PCS_CONTROL_INDEX_0, AAL_PHY_SPEED_10000);
	aal_xfi_set_speed(0, AAL_SXGMII_PCS_CONTROL_INDEX_1, AAL_PHY_SPEED_10000);

    return CA_E_OK;
}
#endif

#endif /* CONFIG_CORTINA_BOARD_FPGA */


ca_status_t  aal_eth_phy_reset(
		ca_device_id_t  device_id,
    ca_uint8_t   phy_addr)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.reset(phy_addr);

    return CA_E_NOT_SUPPORT;
}

ca_status_t  aal_eth_phy_auto_neg_restart(
    ca_device_id_t  device_id,
    ca_uint8_t   phy_addr)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.auto_neg_restart(phy_addr);

    return CA_E_NOT_SUPPORT;
}

ca_status_t  aal_eth_phy_power_set(
    ca_device_id_t  device_id,
    ca_uint8_t   phy_addr,
    ca_boolean_t power_up)
{
    ca_status_t ret = CA_E_OK;

    if (p_eth_phy_dev[phy_addr])
        ret = p_eth_phy_dev[phy_addr]->drv.power_set(phy_addr, power_up);
    else
        return CA_E_OK;


    if (power_up) {
        __DELAY_MS(100);
    } else {
        __DELAY_MS(50);
    }

    return ret;
}

ca_status_t  aal_eth_phy_power_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    ca_boolean_t *power_up)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.power_get(phy_addr, power_up);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_auto_neg_set(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    ca_boolean_t  enable)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.auto_neg_set(phy_addr, enable);

    return CA_E_NOT_SUPPORT;
}

ca_status_t  aal_eth_phy_auto_neg_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    ca_boolean_t *enable)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.auto_neg_get(phy_addr, enable);

    return CA_E_NOT_SUPPORT;
}

ca_status_t  aal_eth_phy_ability_adv_set(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    aal_phy_autoneg_adv_t adv)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.ability_adv_set(phy_addr, adv);

    return CA_E_NOT_SUPPORT;
}

ca_status_t  aal_eth_phy_ability_adv_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    aal_phy_autoneg_adv_t *adv)
{

    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.ability_adv_get(phy_addr, adv);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_partner_ability_adv_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    aal_phy_autoneg_adv_t *adv)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.partner_ability_adv_get(phy_addr, adv);

    return CA_E_NOT_SUPPORT;

}


ca_status_t  aal_eth_phy_link_status_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    aal_phy_link_status_t *link_status)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.link_status_get(phy_addr, link_status);

    return CA_E_NOT_SUPPORT;

}


ca_status_t  aal_eth_phy_speed_set(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    aal_phy_speed_mode_t speed)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.speed_set(phy_addr, speed);

    return CA_E_NOT_SUPPORT;
}


ca_status_t  aal_eth_phy_speed_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    aal_phy_speed_mode_t *speed)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.speed_get(phy_addr, speed);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_duplex_set(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    ca_boolean_t  is_full)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.duplex_set(phy_addr, is_full);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_duplex_get(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    ca_boolean_t *is_full)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.duplex_get(phy_addr, is_full);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_flow_ctrl_set(
    ca_device_id_t   device_id,
    ca_uint8_t    phy_addr,
    ca_boolean_t  enable)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.flow_ctrl_set(phy_addr, enable);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_flow_ctrl_get(
    ca_device_id_t    device_id,
    ca_uint8_t     phy_addr,
    ca_boolean_t  *enable)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.flow_ctrl_get(phy_addr, enable);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_mdix_set(
    ca_device_id_t    device_id,
    ca_uint8_t     phy_addr,
    aal_phy_mdi_mode_t mdix_mode)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.mdix_set(phy_addr, mdix_mode);

    return CA_E_NOT_SUPPORT;

}

ca_status_t  aal_eth_phy_mdix_get(
    ca_device_id_t    device_id,
    ca_uint8_t     phy_addr,
    aal_phy_mdi_mode_t *mdix_mode)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.mdix_get(phy_addr, mdix_mode);

    return CA_E_NOT_SUPPORT;

}


ca_status_t  aal_eth_phy_loopback_set(
    ca_device_id_t    device_id,
    ca_uint8_t     phy_addr,
    aal_phy_lpbk_mode_t mode)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.loopback_set(phy_addr, mode);

    return CA_E_NOT_SUPPORT;
}

ca_status_t  aal_eth_phy_loopback_get(
    ca_device_id_t    device_id,
    ca_uint8_t     phy_addr,
    aal_phy_lpbk_mode_t *mode)
{
    if (p_eth_phy_dev[phy_addr])
        return p_eth_phy_dev[phy_addr]->drv.loopback_get(phy_addr, mode);

    return CA_E_NOT_SUPPORT;
}



#define CA_PORT_MAX_NUM_FE  5
#define __FOR_ALL_FE_PORTS_DO(port)   \
                       for((port)=0; (port)<CA_PORT_MAX_NUM_FE; ++(port))

void aal_fe_phy_auto_neg(void)
{
    ca_port_id_t port_id = 0;
    ca_uint16_t page = 0;
    ca_uint32_t fe_an_cnt = __FE_AUTO_NEG_MAX_CNT, reg_ad;
    aal_phy_link_status_t link_status[5];
    ca_uint8_t phy_addr[5], link_up_cnt = 0, link_up_bm[5];

    GLOBAL_PCS_PHY_CONTROL_0_t global_pcs_phy_control_0;
    global_pcs_phy_control_0.wrd = CA_NE_REG_READ(GLOBAL_PCS_PHY_CONTROL_0);
    __FOR_ALL_FE_PORTS_DO(port_id){
        phy_addr[port_id] = ( (global_pcs_phy_control_0.wrd >> (port_id * 5)) & 0x1f);
    }

    __FOR_ALL_FE_PORTS_DO(port_id){
        memset(&link_status[port_id], 0x0, sizeof(aal_phy_link_status_t));
//        __fe_phy_reset(phy_addr[port_id]);
        reg_ad = PHY_REG_CTRL;
        fe_page_write(page, phy_addr[port_id], reg_ad, 0x8000);
    }
    mdelay(__FE_RST_MDLAY);

    __FOR_ALL_FE_PORTS_DO(port_id){
        __fe_phy_auto_neg_set(phy_addr[port_id], ENABLE);
        __fe_phy_auto_neg_restart(phy_addr[port_id]);
    }
    mdelay(__FE_AUTO_NEG_MDLAY);

    __FOR_ALL_FE_PORTS_DO(port_id){
        __fe_phy_link_status_get(phy_addr[port_id], &link_status[port_id]);
        link_up_bm[port_id] = 0;
    }

    do{
        link_up_cnt = 0;
        mdelay(__FE_AUTO_NEG_MDLAY);

        if( !(fe_an_cnt % 20) )
#ifndef CONFIG_98FH_MP_ENABLE     
            printf(".");
#endif
        __FOR_ALL_FE_PORTS_DO(port_id){
            __fe_phy_link_status_get(phy_addr[port_id], &link_status[port_id]);
            
            if( (link_status[port_id].link_up) && (0 == link_up_bm[port_id]) ){
#if 0//CONFIG_98F_UBOOT_NE_DBG
printf("\t%s(%d)\t\tlink_status[port_id].link_up: %#x\n", __func__, __LINE__, link_status[port_id].link_up);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

                link_up_bm[port_id] = 1;
#ifndef CONFIG_98FH_MP_ENABLE
//printf("{P20%x}\n",port_id);
//#else 
                printf("\tport %#x (phy_ad: %#x) is link up! {speed: %s, duplex: %s}\n", \
                    port_id, \
                    phy_addr[port_id], \
                    link_status[port_id].speed ? "100Mbps" : "10Mbps", \
                    link_status[port_id].duplex ? "Full-duplex" : "Half-duplex");
#endif
                link_up_cnt++;
            }
        }

        if(5 == link_up_cnt)
            break;

        fe_an_cnt--;
    }
    while(fe_an_cnt);
    printf("\n");

    __FOR_ALL_FE_PORTS_DO(port_id){
        if( 0 == link_up_bm[port_id] ){
         #ifdef CONFIG_98FH_MP_ENABLE
REG(0xf4320730)++; ///printf("{F20%x}\n",port_id);
#else 
                
            printf("\tport %#x (phy_ad: %#x) is still link down...\n", port_id, phy_addr[port_id]);
#endif
        }
    }

    return;
}

void aal_fe_phy_force_mode(ca_port_id_t port_id, int fe_speed, bool is_full)
{
    ca_uint8_t phy_addr[5], ca_port_idx;
    int link10MPASS=0;

    GLOBAL_PCS_PHY_CONTROL_0_t global_pcs_phy_control_0;
    global_pcs_phy_control_0.wrd = CA_NE_REG_READ(GLOBAL_PCS_PHY_CONTROL_0);
    __FOR_ALL_FE_PORTS_DO(ca_port_idx){
        phy_addr[ca_port_idx] = ( (global_pcs_phy_control_0.wrd >> (ca_port_idx * 5)) & 0x1f);
    }

    ca_uint16_t page = 0, data;
    ca_uint32_t reg_ad;

    ca_uint32_t fe_an_cnt = __FE_FM_MAX_CNT;
    aal_phy_link_status_t link_status;

    memset(&data, 0x0, sizeof(ca_uint16_t));

    aal_phy_speed_mode_t speed;

    if(10 == fe_speed){
        speed = AAL_PHY_SPEED_10;
    }else if(100 == fe_speed){
        speed = AAL_PHY_SPEED_100;
    }else{ /* err handling */
        printf("%s: the input fe-phy speed should be '10' or '100'\n", __func__);
        return;
    }

    /* disable N-way, use force-mode */
    switch (speed) {
        default:
        case AAL_PHY_SPEED_10:
            if (is_full)
            {       /* force fe-phy at 10Mbps, Full-Duplex */
                data = 0x0100;
            }
            else    /* force fe-phy at 10Mbps, Half-Duplex */
            {
                data = 0x0000;
            }
            break;

        case AAL_PHY_SPEED_100:
            if (is_full)
            {       /* force fe-phy at 100Mbps, Full-Duplex */
                data = 0x2100;
            }
            else    /* force fe-phy at 100Mbps, Half-Duplex */
            {
                data = 0x2000;
            }
            break;
    }

    /* reset pcs, then force mode, to drive its link speed change */
    reg_ad = PHY_REG_CTRL;
    fe_page_write(page, phy_addr[port_id], reg_ad, 0x8000);
    mdelay(__FE_RST_MDLAY);
    fe_page_write(page, phy_addr[port_id], reg_ad, data);

    memset(&link_status, 0x0, sizeof(aal_phy_link_status_t));

    do{
        mdelay(__FE_FM_MDLAY);

        if( !(fe_an_cnt % 20) )
        {
#ifndef CONFIG_98FH_MP_ENABLE 
            printf(".");
#endif
        }

        __fe_phy_link_status_get(phy_addr[port_id], &link_status);
      
        if(link_status.link_up){
                
#ifndef CONFIG_98FH_MP_ENABLE
//printf("{P21%x}\n",port_id);
//#else  
            printf("\tport %#x (phy_ad: %#x) is link up! {speed: %s, duplex: %s}\n", \
                port_id, \
                phy_addr[port_id], \
                link_status.speed ? "100Mbps" : "10Mbps", \
                link_status.duplex ? "Full-duplex" : "Half-duplex");
#endif
                return CA_E_OK;
        }
        
        fe_an_cnt--;
    }
    while(fe_an_cnt);


#ifdef CONFIG_98FH_MP_ENABLE
       REG(0xf4320720)++;  //printf("{F21%x}\n",port_id);
#endif

    return CA_E_NOT_FOUND;
}

#if 1
void aal_set_8211f(ca_uint8_t phy_addr, ca_uint8_t nic_mode, int ge_speed, bool is_full)
{
    ca_uint16_t page = 0, data;
    ca_uint32_t reg_ad, fe_an_cnt = __GE_FM_MAX_CNT;
    aal_phy_link_status_t link_status;
    aal_phy_speed_mode_t speed;
    ca_port_id_t port_id = 4;

#if 0
printf("%s  %#x  %#x  %#x  %#x\n", __func__, \
    phy_addr, nic_mode, ge_speed, is_full);
#endif

    memset(&data, 0x0, sizeof(ca_uint16_t));

    switch(nic_mode) {
        default:
        case AAL_GE_NIC_OP_AN:
            /* en-AN && restart-AN */
            data = 0x1200;
            goto set_pcs;
            break;

        case AAL_GE_NIC_OP_FM:
            if(10 == ge_speed){
                speed = AAL_PHY_SPEED_10;
            }else if(100 == ge_speed){
                speed = AAL_PHY_SPEED_100;
            }else if(1000 == ge_speed){
                speed = AAL_PHY_SPEED_1000;
            }else{ /* err handling */
                printf("%s: the input ge-phy speed should be '10', '100', or '1000'\n", __func__);
                return;
            }

            /* disable N-way, use force-mode */
            switch (speed) {
                default:
                case AAL_PHY_SPEED_10:
                    if (is_full)
                    {       /* force ge-phy at 10Mbps, Full-Duplex */
                        data = 0x0100;
                    }
                    else    /* force ge-phy at 10Mbps, Half-Duplex */
                    {
                        data = 0x0000;
                    }
                    break;

                case AAL_PHY_SPEED_100:
                    if (is_full)
                    {       /* force ge-phy at 100Mbps, Full-Duplex */
                        data = 0x2100;
                    }
                    else    /* force ge-phy at 100Mbps, Half-Duplex */
                    {
                        data = 0x2000;
                    }
                    break;

                case AAL_PHY_SPEED_1000:
                    /* force ge-phy at 1000Mbps, always Full-Duplex */
                    data = 0x140;
            }
            goto set_pcs;
            break;

        case AAL_GE_NIC_OP_LPBK:
            /* speed[6,13]=2'b10=1Gbps && en-loopback */
            data = 0x4040;
            goto set_pcs;
            break;
    }

set_pcs:
            /* reset pcs, then force mode, to drive its link speed change */
            reg_ad = PHY_REG_CTRL;
            mdio_page_write(page, phy_addr, reg_ad, 0x8000);
            mdelay(__GE_RST_MDLAY);
            mdio_page_write(page, phy_addr, reg_ad, data);

            if(AAL_GE_NIC_OP_LPBK == nic_mode){
#ifndef CONFIG_98FH_MP_ENABLE
                
                printf("\tConfig ext. GE NIC to be loopback mode!\n");
#endif
                goto end;
            }else if(AAL_GE_NIC_OP_AN == nic_mode){
#ifndef CONFIG_98FH_MP_ENABLE            
                printf("\tConfig ext. GE NIC to be AN mode!\n");
#endif
            }else if(AAL_GE_NIC_OP_FM == nic_mode){
#ifndef CONFIG_98FH_MP_ENABLE               
                printf("\tConfig ext. GE NIC to be Force mode!\n");
#endif
            }else{
#ifndef CONFIG_98FH_MP_ENABLE        
                printf("\tConfig ext. GE NIC to be wrong op mode!\n");
#endif
                goto end;
            }

            memset(&link_status, 0x0, sizeof(aal_phy_link_status_t));

            do{
                mdelay(__GE_FM_MDLAY);

                if( !(fe_an_cnt % 20) ){
#ifndef CONFIG_98FH_MP_ENABLE
                    printf(".");
#endif
                }

              __ge_nic_link_status_get(phy_addr, &link_status) ;

                if(link_status.link_up){

                    if(link_status.speed < AAL_GE_NIC_SPEED_1000){

#ifdef CONFIG_98FH_MP_ENABLE
                        printf("{P005}\n");
                        REG(0xf4320738)++;
                        //printf("\t%s%d", __func__, __LINE__);
#else
                        printf("\tport %#x (phy_ad: %#x) is link up! {speed: %s, duplex: %s}\n", \
                            port_id, \
                            phy_addr, \
                            link_status.speed ? "100Mbps" : "10Mbps", \
                            link_status.duplex ? "Full-duplex" : "Half-duplex");
#endif
                    }else if(link_status.speed == AAL_GE_NIC_SPEED_1000)
                    {
#ifdef CONFIG_98FH_MP_ENABLE
                        printf("{P005}\n");
                        REG(0xf4320738)++;
                         printf("\t%s%d", __func__, __LINE__);
#else                      
                        printf("\tport %#x (phy_ad: %#x) is link up! {speed: %s, duplex: %s}\n", \
                            port_id, phy_addr, "1000Mbps", "Full-duplex");
#endif
                    }else
                    {
#ifdef CONFIG_98FH_MP_ENABLE
                        printf("{F005}\n");
                        REG(0xf4320734)++;
                         printf("\t%s%d", __func__, __LINE__);
                   
#else                     
                        printf("\tThe speed is out of range, please check the return code!(link speed = %d)\n", link_status.speed);
#endif
                        return;
                    }

                    break;
                }              
                fe_an_cnt--;
            }
            while(fe_an_cnt);

 //printf("\tfe_an_cnt= %d\n", fe_an_cnt);
if (!fe_an_cnt){

#ifdef CONFIG_98FH_MP_ENABLE
printf("{F005}\n");
REG(0xf4320734)++; 

#endif
}
            
            goto end;

end:
    return;
}

#define DBG_AAL_MDIOACCESSEXTDEVPHYID   0
ca_status_t  aal_mdioAccessExtDevPhyID(
    ca_uint32_t ExtDev_PAGE,
    ca_uint32_t ExtDev_PHY_ADDR,
    ca_uint32_t ExtDev_PHYID1_REG,
    ca_uint32_t ExtDev_PHYID2_REG,
    ca_uint32_t ExtDev_PHYID1,
    ca_uint32_t ExtDev_PHYID1_bm,
    ca_uint32_t ExtDev_PHYID2,
    ca_uint32_t ExtDev_PHYID2_bm
){
    ca_uint32_t ret;

#if DBG_AAL_MDIOACCESSEXTDEVPHYID
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PAGE", ExtDev_PAGE, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHY_ADDR", ExtDev_PHY_ADDR, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1_REG", ExtDev_PHYID1_REG, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2_REG", ExtDev_PHYID2_REG, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1", ExtDev_PHYID1, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID1_bm", ExtDev_PHYID1_bm, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2", ExtDev_PHYID2, __func__, __LINE__);
    printf("%s(0x%x)\t[%s(%d)]\n", "ExtDev_PHYID2_bm", ExtDev_PHYID2_bm, __func__, __LINE__);
#endif /* DBG_AAL_MDIOACCESSEXTDEVPHYID */

    ret = _mdioAccessExtDevPhyID(ExtDev_PAGE, ExtDev_PHY_ADDR, ExtDev_PHYID1_REG, ExtDev_PHYID2_REG, ExtDev_PHYID1, ExtDev_PHYID1_bm, ExtDev_PHYID2, ExtDev_PHYID2_bm);

    return ret;
}

#endif

