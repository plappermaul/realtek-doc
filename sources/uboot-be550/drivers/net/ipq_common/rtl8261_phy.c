#include <common.h>
#include <net.h>
#include <asm-generic/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <phy.h>
#include "rtl8261_phy.h"
#include "ipq_phy.h"

extern int ipq_mdio_read(int mii_id,
		int regnum, ushort *data);
extern int ipq_mdio_write(int mii_id,
		int regnum, u16 data);

u16 rtl8261_phy_reg_read(u32 dev_id, u32 phy_id, u32 reg_id)
{   
	return ipq_mdio_read(phy_id, reg_id, NULL);
}

u16 rtl8261_phy_reg_write(u32 dev_id, u32 phy_id, u32 reg_id, u16 value)
{
	return ipq_mdio_write(phy_id, reg_id, value);
}

u8 rtl8261_phy_get_link_status(u32 dev_id, u32 phy_id)
{
	u16 phy_data;
	phy_data = rtl8261_phy_reg_read(dev_id, phy_id,
				RTL8261_REG_ADDRESS(RTL8261_MMD_AUTONEG, RTL8261_MMD_AN_STAT));
	phy_data = rtl8261_phy_reg_read(dev_id, phy_id,
				RTL8261_REG_ADDRESS(RTL8261_MMD_AUTONEG, RTL8261_MMD_AN_STAT));

	if (((phy_data >> 2) & 0x1) & PORT_LINK_UP)
		return 0;


	return 1;
}

u32 rtl8261_phy_get_duplex(u32 dev_id, u32 phy_id, fal_port_duplex_t *duplex)
{

    u16 phy_data = rtl8261_phy_reg_read(dev_id, phy_id, RTL8261_REG_ADDRESS(0x1f, 0xa434));
	*duplex = (phy_data & BIT_3) ? (FAL_FULL_DUPLEX) : (FAL_HALF_DUPLEX);

	return 0;
}

u32 rtl8261_phy_get_speed(u32 dev_id, u32 phy_id, fal_port_speed_t *speed)
{
	uint32_t rData = 0;

	rData = rtl8261_phy_reg_read(0, phy_id, RTL8261_REG_ADDRESS(0x1F, 0xa434));
	switch (rData & SPEED_MASK)
	{
	case SPEED_10M:
		*speed = FAL_SPEED_10;
		break;
	case SPEED_100M:
		*speed = FAL_SPEED_100;
		break;	
	case SPEED_1000M:
		*speed = FAL_SPEED_1000;
		break;
	case SPEED_2500M:
		*speed = FAL_SPEED_2500;
		break;
	case SPEED_5000M:
		*speed = FAL_SPEED_5000;
		break;
	case SPEED_10000M:
		*speed = FAL_SPEED_10000;
		break;
	default:
		*speed = FAL_SPEED_10;
		break;
	}
	return 0;
}


int32_t rtk_phylib_mmd_read(u32 phy_id, uint32_t mmd, uint32_t reg, uint8_t msb, uint8_t lsb, uint32_t *pData)
{
    int32_t  ret = 0;
    uint32_t rData = 0;
    uint32_t mask = 0;
    mask = UINT32_BITS_MASK(msb,lsb);

//   rData =  phy_read_mmd(phydev, mmd, reg);

    rData = rtl8261_phy_reg_read(0, phy_id, RTL8261_REG_ADDRESS(mmd, reg));

    *pData = REG32_FIELD_GET(rData, lsb, mask);
    return ret;
}

int32_t rtk_phylib_mmd_write(u32 phy_id, uint32_t mmd, uint32_t reg, uint8_t msb, uint8_t lsb, uint32_t data)
{
    int32_t  ret = 0;
    uint32_t mask = 0;
    mask = UINT32_BITS_MASK(msb,lsb);

	uint32_t rData = 0, wData = 0;

//	phy_modify_mmd(phydev, mmd, reg, mask, (data << lsb));
	if ((msb != 15) || (lsb != 0))
	{
		rData = rtl8261_phy_reg_read(0, phy_id, RTL8261_REG_ADDRESS(mmd, reg));
	}

	wData = REG32_FIELD_SET(rData, data, lsb, mask);
    
	rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(mmd, reg), wData);

    return ret;
}

/* Indirect Register Access APIs */
int rtk_phylib_826xb_sds_read(u32 phy_id, uint32_t page, uint32_t reg, uint8_t msb, uint8_t lsb, uint32_t *pData)
{
    int32_t  ret = 0;
    uint32_t rData = 0;
    uint32_t op = (page & 0x3f) | ((reg & 0x1f) << 6) | (0x8000);
    uint32_t i = 0;
    uint32_t mask = 0;
    mask = UINT32_BITS_MASK(msb,lsb);

    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 323, 15, 0, op));

    for (i = 0; i < 10; i++)
    {
        RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_read(phy_id, 30, 323, 15, 15, &rData));
        if (rData == 0)
        {
            break;
        }
        udelay(10);
    }
    if (i == 10)
    {
        return -1;
    }

    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_read(phy_id, 30, 322, 15, 0, &rData));
    *pData = REG32_FIELD_GET(rData, lsb, mask);

    return ret;
}

int rtk_phylib_826xb_sds_write(u32 phy_id, uint32_t page, uint32_t reg, uint8_t msb, uint8_t lsb, uint32_t data)
{
    int32_t  ret = 0;
    uint32_t wData = 0, rData = 0;
    uint32_t op = (page & 0x3f) | ((reg & 0x1f) << 6) | (0x8800);
    uint32_t mask = 0;
    mask = UINT32_BITS_MASK(msb,lsb);

    RTK_PHYLIB_ERR_CHK(rtk_phylib_826xb_sds_read(phy_id, page, reg, 15, 0, &rData));

    wData = REG32_FIELD_SET(rData, data, lsb, mask);

    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 321, 15, 0, wData));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 323, 15, 0, op));

    return ret;
}

int32_t phy_common_general_reg_mmd_get(u32 phy_id, u32 mmdAddr, u32 mmdReg, u32 *pData)
{
    u32 phy_data = 0;
    phy_data = rtl8261_phy_reg_read(0x0, phy_id,
			RTL8261_REG_ADDRESS(mmdAddr, mmdReg));
    *pData = (u32)phy_data;
    return 0;
}

int32_t phy_common_general_reg_mmd_set(u32 phy_id, u32 mmdAddr, u32 mmdReg, u32 data)
{
    rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(mmdAddr, mmdReg), data);
    return 0;
}

/*
int32_t rtk_phylib_time_usecs_get(ulong *pUsec)
{
    if(NULL == pUsec)
        return RTK_PHYLIB_ERR_INPUT; 
            
    *pUsec = get_timer();
    return 0;
}
*/

extern int32_t phy_rtl826xb_patch_db_init(u32 phy_id, rt_phy_patch_db_t **pPhy_patchDb);
extern int32 phy_patch(uint32 phy_id, uint8 portOffset, uint8 patch_mode);
#define PHY_PATCH_MODE_NORMAL       0
#define PHY_PATCH_MODE_CMP          1

rt_phy_patch_db_t * patch[2] = {NULL, NULL};
rt_phy_patch_db_t * get_patch_db(int32_t phy_id)
{

	return (1 == phy_id) ? patch[0]:patch[1];
}

int rtk_phylib_826xb_intr_read_clear(u32 phy_id, uint32 *status)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 rStatus = 0;

    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_read(phy_id, 31, 0xA43A, 15, 0, &rData));
    if(rData & BIT_2)
        rStatus |= RTK_PHY_INTR_NEXT_PAGE_RECV;
    if(rData & BIT_3)
        rStatus |= RTK_PHY_INTR_AN_COMPLETE;
    if(rData & BIT_4)
        rStatus |= RTK_PHY_INTR_LINK_CHANGE;
    if(rData & BIT_9)
        rStatus |= RTK_PHY_INTR_ALDPS_STATE_CHANGE;
    if(rData & BIT_11)
        rStatus |= RTK_PHY_INTR_FATAL_ERROR;
    if(rData & BIT_7)
        rStatus |= RTK_PHY_INTR_WOL;

    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_read(phy_id, 31, 0xE2, 15, 0, &rData));
    if(rData & BIT_1)
        rStatus |= RTK_PHY_INTR_RLFD;
    if(rData & BIT_3)
        rStatus |= RTK_PHY_INTR_TM_LOW;
    if(rData & BIT_4)
        rStatus |= RTK_PHY_INTR_TM_HIGH;
    if(rData & BIT_6)
        rStatus |= RTK_PHY_INTR_MACSEC;
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0xE2, 15, 0, 0xFF));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0x2DC, 15, 0, 0xFF));

    *status = rStatus;
    return ret;
}

int rtk_phylib_826xb_intr_init(u32 phy_id)
{
    int32  ret = 0;
    uint32 status = 0;

    /* Disable all IMR*/
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0xE1, 15, 0, 0));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0xE3, 15, 0, 0));

    /* source */
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0xE4, 15, 0, 0x1));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0xE0, 15, 0, 0x2F));

    /* init common link change */
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 31, 0xA424,  15,  0, 0x10));
    /* init rlfd */
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 31, 0xA442, 15, 15, 0x1));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 31, 0xA448, 7, 7, 0x1));
    /* init tm */
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0x1A0, 11, 11, 0x1));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0x19D, 11, 11, 0x1));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0x1A1, 11, 11, 0x1));
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 30, 0x19F, 11, 11, 0x1));
    /* init WOL */
    RTK_PHYLIB_ERR_CHK(rtk_phylib_mmd_write(phy_id, 31, 0xA424,  7,  7, 0x1));

    /* clear status */
    RTK_PHYLIB_ERR_CHK(rtk_phylib_826xb_intr_read_clear(phy_id, &status));

    return ret;
}

int rtl8261_phy_init(struct phy_ops **ops, u32 phy_id)
{
	u16 phy_data;
	struct phy_ops *rtl8261_ops;
    	rt_phy_patch_db_t *pPatchDb = NULL;
	int32 ret = 0;

	rtl8261_ops = (struct phy_ops *)malloc(sizeof(struct phy_ops));
	if (!rtl8261_ops)
		return -ENOMEM;
	rtl8261_ops->phy_get_link_status = rtl8261_phy_get_link_status;
	rtl8261_ops->phy_get_speed = rtl8261_phy_get_speed;
	rtl8261_ops->phy_get_duplex = rtl8261_phy_get_duplex;
	*ops = rtl8261_ops;

	if (0 == phy_id)
	{
		phy_rtl826xb_patch_db_init(phy_id, &patch[0]);
		printf ("patch address: 0x%p\n", patch[0]);
	}
	else
	{
		phy_rtl826xb_patch_db_init(phy_id, &patch[1]);
		printf ("patch address: 0x%p\n", patch[1]);
	}

    PHYPATCH_DB_GET(phy_id, pPatchDb);
	printf ("PHYPATCH_DB_GET: 0x%p\n", pPatchDb);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(RTL8261_MMD_PMAPMD, RTL8261_MMD_PMAPMD_PHY_ID1));
	printf ("PHY ID1: 0x%x\n", phy_data);
	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(RTL8261_MMD_PMAPMD, RTL8261_MMD_PMAPMD_PHY_ID2));
	printf ("PHY ID2: 0x%x\n", phy_data);

	rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(30, 0x145), 0x0001);
	rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(30, 0x145), 0x0000);
	mdelay(30);
	ret = phy_patch(phy_id, 0, PHY_PATCH_MODE_NORMAL);
	if (ret)
	{
		printf("patch failed.\n");
	}

#if 0
	ret = phy_patch(phy_id, 0, PHY_PATCH_MODE_CMP);
	if (ret)
	{
		printf("patch check failed ret 0X%X.\n",ret);
	}
	printf("patch check ok ret=%d\n", ret);
#endif

#if 0
	ret = rtk_phylib_826xb_intr_init(phy_id);
	if (ret)
	{
		printf("intrrupt init failed\n");		
	}
#endif

	rtk_phylib_826xb_sds_write(phy_id, 6, 3, 15, 0, 0x88C6);

	phy_data = rtl8261_phy_reg_read(0, phy_id, RTL8261_REG_ADDRESS(1, 0x00));
    rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(1, 0x00), phy_data&(~BIT_11) );
#if 0
	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(0x1, 0x0));
	printf ("phy_id(%d) mmd-aadr(0x1) mmd-reg(0x0) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(31, 0xa434));
	printf ("phy_id(%d) mmd-aadr(31) mmd-reg(0xa434) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x105));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x105) value = 0x%x\n", phy_id, phy_data);

	rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(30, 0x143), 0x859f);
	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x142));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x142) value = 0x%x\n", phy_id, phy_data);

	rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(30, 0x143), 0x8005);
	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x142));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x142) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0xc1));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0xc1) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(31, 0xa5d4));
	printf ("phy_id(%d) mmd-aadr(31) mmd-reg(0xa5d4) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x2d2));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x2d2) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x2d3));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x2d3) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x2d4));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x2d4) value = 0x%x\n", phy_id, phy_data);

	phy_data = rtl8261_phy_reg_read(0x0, phy_id,
				RTL8261_REG_ADDRESS(30, 0x2d5));
	printf ("phy_id(%d) mmd-aadr(30) mmd-reg(0x2d5) value = 0x%x\n", phy_id, phy_data);
#endif
	return 0;
}

static int do_rtl8261_mdio(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        char            op[2];
        unsigned int    phy_id = 0, addr = 0, reg = 0;
        unsigned int    data = 0;

        if (argc < 2)
                return CMD_RET_USAGE;

        op[0] = argv[1][0];
        if (strlen(argv[1]) > 1)
                op[1] = argv[1][1];
        else
                op[1] = '\0';

        if (argc >= 3)
                phy_id = simple_strtoul(argv[2], NULL, 16);
        if (argc >= 4)
                addr = simple_strtoul(argv[3], NULL, 16);
        if (argc >= 5)
                reg = simple_strtoul(argv[4], NULL, 16);
        if (argc >= 6)
                data = simple_strtoul(argv[5], NULL, 16);

        if (op[0] == 'r') {
                data = rtl8261_phy_reg_read(0x0, phy_id,RTL8261_REG_ADDRESS(addr, reg));
                printf("read phy_id(0x%x) mmd(0x%x) reg(0x%x) value(0x%x)\n", phy_id, addr, reg, data);
        } else if (op[0] == 'w') {
		rtl8261_phy_reg_write(0, phy_id, RTL8261_REG_ADDRESS(addr, reg), data);
                printf("write phy_id(0x%x) mmd(0x%x) reg(0x%x) value(0x%x)\n", phy_id, addr, reg, data);
        } else {
                return CMD_RET_USAGE;
        }   

        return 0;
}

U_BOOT_CMD(
        rtl8261_mdio, 6, 1, do_rtl8261_mdio,
        "rtl8261 mdio utility commands",
        "rtl8261_mdio read   <phy_id ><mmd> <reg>               - read  RTL8361 MDIO PHY <phy_id ><mmd> <reg>\n"
        "rtl8261_mdio write  <phy_id ><mmd> <reg> <value>        - write RTL8361 MDIO PHY <phy_id ><mmd> <reg> <value>\n"
        "Addr and/or reg may be ranges, e.g. 0-7."
);


