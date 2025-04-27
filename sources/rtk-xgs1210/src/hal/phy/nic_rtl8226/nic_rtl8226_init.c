
/* SW_SDK: include files */
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <osal/time.h>
#include <osal/phy_osal.h>
#include <hal/phy/nic_rtl8226/nic_rtl8226.h>


static const MMD_REG Rtl8226_n0_ramcode[] =
{
    { 31, 0xA436, 0xA016, },
    { 31, 0xA438, 0x0000, },
    { 31, 0xA436, 0xA012, },
    { 31, 0xA438, 0x0000, },
    { 31, 0xA436, 0xA014, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x8010, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x808b, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x808f, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x8093, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x8097, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x809d, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x80a1, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x80aa, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x607b, },
    { 31, 0xA438, 0x40da, },
    { 31, 0xA438, 0xf00e, },
    { 31, 0xA438, 0x42da, },
    { 31, 0xA438, 0xf01e, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x615b, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1456, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14a4, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14bc, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x5f2e, },
    { 31, 0xA438, 0xf01c, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1456, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14a4, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14bc, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x5f2e, },
    { 31, 0xA438, 0xf024, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1456, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14a4, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14bc, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x5f2e, },
    { 31, 0xA438, 0xf02c, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1456, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14a4, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x14bc, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x5f2e, },
    { 31, 0xA438, 0xf034, },
    { 31, 0xA438, 0xd719, },
    { 31, 0xA438, 0x4118, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac11, },
    { 31, 0xA438, 0xd501, },
    { 31, 0xA438, 0xce01, },
    { 31, 0xA438, 0xa410, },
    { 31, 0xA438, 0xce00, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x4779, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac0f, },
    { 31, 0xA438, 0xae01, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1444, },
    { 31, 0xA438, 0xf034, },
    { 31, 0xA438, 0xd719, },
    { 31, 0xA438, 0x4118, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac22, },
    { 31, 0xA438, 0xd501, },
    { 31, 0xA438, 0xce01, },
    { 31, 0xA438, 0xa420, },
    { 31, 0xA438, 0xce00, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x4559, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac0f, },
    { 31, 0xA438, 0xae01, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1444, },
    { 31, 0xA438, 0xf023, },
    { 31, 0xA438, 0xd719, },
    { 31, 0xA438, 0x4118, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac44, },
    { 31, 0xA438, 0xd501, },
    { 31, 0xA438, 0xce01, },
    { 31, 0xA438, 0xa440, },
    { 31, 0xA438, 0xce00, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x4339, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac0f, },
    { 31, 0xA438, 0xae01, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1444, },
    { 31, 0xA438, 0xf012, },
    { 31, 0xA438, 0xd719, },
    { 31, 0xA438, 0x4118, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac88, },
    { 31, 0xA438, 0xd501, },
    { 31, 0xA438, 0xce01, },
    { 31, 0xA438, 0xa480, },
    { 31, 0xA438, 0xce00, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x4119, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0xac0f, },
    { 31, 0xA438, 0xae01, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1444, },
    { 31, 0xA438, 0xf001, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x1456, },
    { 31, 0xA438, 0xd718, },
    { 31, 0xA438, 0x5fac, },
    { 31, 0xA438, 0xc48f, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x141b, },
    { 31, 0xA438, 0xd504, },
    { 31, 0xA438, 0x8010, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x121a, },
    { 31, 0xA438, 0xd0b4, },
    { 31, 0xA438, 0xd1bb, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x0898, },
    { 31, 0xA438, 0xd0b4, },
    { 31, 0xA438, 0xd1bb, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x0a0e, },
    { 31, 0xA438, 0xd064, },
    { 31, 0xA438, 0xd18a, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x0b7e, },
    { 31, 0xA438, 0x401c, },
    { 31, 0xA438, 0xd501, },
    { 31, 0xA438, 0xa804, },
    { 31, 0xA438, 0x8804, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x053b, },
    { 31, 0xA438, 0xd500, },
    { 31, 0xA438, 0xa301, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x0648, },
    { 31, 0xA438, 0xc520, },
    { 31, 0xA438, 0xa201, },
    { 31, 0xA438, 0xd701, },
    { 31, 0xA438, 0x252d, },
    { 31, 0xA438, 0x1646, },
    { 31, 0xA438, 0xd708, },
    { 31, 0xA438, 0x4006, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x1646, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x0308, },
    { 31, 0xA436, 0xA026, },
    { 31, 0xA438, 0x0307, },
    { 31, 0xA436, 0xA024, },
    { 31, 0xA438, 0x1645, },
    { 31, 0xA436, 0xA022, },
    { 31, 0xA438, 0x0647, },
    { 31, 0xA436, 0xA020, },
    { 31, 0xA438, 0x053a, },
    { 31, 0xA436, 0xA006, },
    { 31, 0xA438, 0x0b7c, },
    { 31, 0xA436, 0xA004, },
    { 31, 0xA438, 0x0a0c, },
    { 31, 0xA436, 0xA002, },
    { 31, 0xA438, 0x0896, },
    { 31, 0xA436, 0xA000, },
    { 31, 0xA438, 0x11a1, },
    { 31, 0xA436, 0xA008, },
    { 31, 0xA438, 0xff00, },

};


static const MMD_REG Rtl8226_n1_ramcode[] =
{
    { 31, 0xA436, 0xA016, },
    { 31, 0xA438, 0x0010, },
    { 31, 0xA436, 0xA012, },
    { 31, 0xA438, 0x0000, },
    { 31, 0xA436, 0xA014, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x8010, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x8015, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x801a, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x801a, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x801a, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x801a, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x801a, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x801a, },
    { 31, 0xA438, 0xad02, },
    { 31, 0xA438, 0x1000, },
    { 31, 0xA438, 0x02d7, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x00ed, },
    { 31, 0xA438, 0x0c0f, },
    { 31, 0xA438, 0x0509, },
    { 31, 0xA438, 0xc100, },
    { 31, 0xA438, 0x1800, },
    { 31, 0xA438, 0x008f, },
    { 31, 0xA436, 0xA08E, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xA08C, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xA08A, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xA088, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xA086, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xA084, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xA082, },
    { 31, 0xA438, 0x008d, },
    { 31, 0xA436, 0xA080, },
    { 31, 0xA438, 0x00eb, },
    { 31, 0xA436, 0xA090, },
    { 31, 0xA438, 0x0103, },
};


static const MMD_REG Rtl8226_n2_ramcode[] =
{
    { 31, 0xa436, 0xA016, },
    { 31, 0xa438, 0x0020, },
    { 31, 0xa436, 0xA012, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa436, 0xA014, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8010, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8014, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8018, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8024, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8051, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8055, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x8072, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x80dc, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0xfffd, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0xfffd, },
    { 31, 0xa438, 0x8301, },
    { 31, 0xa438, 0x800a, },
    { 31, 0xa438, 0x8190, },
    { 31, 0xa438, 0x82a0, },
    { 31, 0xa438, 0x8404, },
    { 31, 0xa438, 0xa70c, },
    { 31, 0xa438, 0x9402, },
    { 31, 0xa438, 0x890c, },
    { 31, 0xa438, 0x8840, },
    { 31, 0xa438, 0xa380, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x066e, },
    { 31, 0xa438, 0xcb91, },
    { 31, 0xa438, 0xd700, },
    { 31, 0xa438, 0x4063, },
    { 31, 0xa438, 0xd139, },
    { 31, 0xa438, 0xf002, },
    { 31, 0xa438, 0xd140, },
    { 31, 0xa438, 0xd040, },
    { 31, 0xa438, 0xb404, },
    { 31, 0xa438, 0x0c0f, },
    { 31, 0xa438, 0x0d00, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07e0, },
    { 31, 0xa438, 0xa610, },
    { 31, 0xa438, 0xa110, },
    { 31, 0xa438, 0xa2a0, },
    { 31, 0xa438, 0xa404, },
    { 31, 0xa438, 0xd704, },
    { 31, 0xa438, 0x4085, },
    { 31, 0xa438, 0xa180, },
    { 31, 0xa438, 0xa404, },
    { 31, 0xa438, 0x8280, },
    { 31, 0xa438, 0xd704, },
    { 31, 0xa438, 0x405d, },
    { 31, 0xa438, 0xa720, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x0743, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07f0, },
    { 31, 0xa438, 0xd700, },
    { 31, 0xa438, 0x5f74, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x0743, },
    { 31, 0xa438, 0xd702, },
    { 31, 0xa438, 0x7fb6, },
    { 31, 0xa438, 0x8190, },
    { 31, 0xa438, 0x82a0, },
    { 31, 0xa438, 0x8404, },
    { 31, 0xa438, 0x8610, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa438, 0x0c0f, },
    { 31, 0xa438, 0x0d01, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07e0, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x066e, },
    { 31, 0xa438, 0xd158, },
    { 31, 0xa438, 0xd04d, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x03d4, },
    { 31, 0xa438, 0x94bc, },
    { 31, 0xa438, 0x870c, },
    { 31, 0xa438, 0x8380, },
    { 31, 0xa438, 0xd10d, },
    { 31, 0xa438, 0xd040, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07c4, },
    { 31, 0xa438, 0xd700, },
    { 31, 0xa438, 0x5fb4, },
    { 31, 0xa438, 0xa190, },
    { 31, 0xa438, 0xa00a, },
    { 31, 0xa438, 0xa280, },
    { 31, 0xa438, 0xa404, },
    { 31, 0xa438, 0xa220, },
    { 31, 0xa438, 0xd130, },
    { 31, 0xa438, 0xd040, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07c4, },
    { 31, 0xa438, 0xd700, },
    { 31, 0xa438, 0x5fb4, },
    { 31, 0xa438, 0xbb80, },
    { 31, 0xa438, 0xd1c4, },
    { 31, 0xa438, 0xd074, },
    { 31, 0xa438, 0xa301, },
    { 31, 0xa438, 0xd704, },
    { 31, 0xa438, 0x604b, },
    { 31, 0xa438, 0xa90c, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x0556, },
    { 31, 0xa438, 0xcb92, },
    { 31, 0xa438, 0xd700, },
    { 31, 0xa438, 0x4063, },
    { 31, 0xa438, 0xd116, },
    { 31, 0xa438, 0xf002, },
    { 31, 0xa438, 0xd119, },
    { 31, 0xa438, 0xd040, },
    { 31, 0xa438, 0xd703, },
    { 31, 0xa438, 0x60a0, },
    { 31, 0xa438, 0x6241, },
    { 31, 0xa438, 0x63e2, },
    { 31, 0xa438, 0x6583, },
    { 31, 0xa438, 0xf054, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x611e, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x40da, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d10, },
    { 31, 0xa438, 0xa010, },
    { 31, 0xa438, 0x8740, },
    { 31, 0xa438, 0xf02f, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d50, },
    { 31, 0xa438, 0x8010, },
    { 31, 0xa438, 0xa740, },
    { 31, 0xa438, 0xf02a, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x611e, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x40da, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d20, },
    { 31, 0xa438, 0xa010, },
    { 31, 0xa438, 0x8740, },
    { 31, 0xa438, 0xf021, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d60, },
    { 31, 0xa438, 0x8010, },
    { 31, 0xa438, 0xa740, },
    { 31, 0xa438, 0xf01c, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x611e, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x40da, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d30, },
    { 31, 0xa438, 0xa010, },
    { 31, 0xa438, 0x8740, },
    { 31, 0xa438, 0xf013, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d70, },
    { 31, 0xa438, 0x8010, },
    { 31, 0xa438, 0xa740, },
    { 31, 0xa438, 0xf00e, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x611e, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x40da, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d40, },
    { 31, 0xa438, 0xa010, },
    { 31, 0xa438, 0x8740, },
    { 31, 0xa438, 0xf005, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d80, },
    { 31, 0xa438, 0x8010, },
    { 31, 0xa438, 0xa740, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07e8, },
    { 31, 0xa438, 0xa610, },
    { 31, 0xa438, 0xd704, },
    { 31, 0xa438, 0x405d, },
    { 31, 0xa438, 0xa720, },
    { 31, 0xa438, 0xd700, },
    { 31, 0xa438, 0x5ff4, },
    { 31, 0xa438, 0xa008, },
    { 31, 0xa438, 0xd704, },
    { 31, 0xa438, 0x4046, },
    { 31, 0xa438, 0xa002, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x0743, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07fb, },
    { 31, 0xa438, 0xd703, },
    { 31, 0xa438, 0x7f6f, },
    { 31, 0xa438, 0x7f4e, },
    { 31, 0xa438, 0x7f2d, },
    { 31, 0xa438, 0x7f0c, },
    { 31, 0xa438, 0x800a, },
    { 31, 0xa438, 0x0cf0, },
    { 31, 0xa438, 0x0d00, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x07e8, },
    { 31, 0xa438, 0x8010, },
    { 31, 0xa438, 0xa740, },
    { 31, 0xa438, 0x1000, },
    { 31, 0xa438, 0x0743, },
    { 31, 0xa438, 0xd702, },
    { 31, 0xa438, 0x7fb5, },
    { 31, 0xa438, 0xd701, },
    { 31, 0xa438, 0x3ad4, },
    { 31, 0xa438, 0x0556, },
    { 31, 0xa438, 0x8610, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x066e, },
    { 31, 0xa438, 0xd1f5, },
    { 31, 0xa438, 0xd049, },
    { 31, 0xa438, 0x1800, },
    { 31, 0xa438, 0x01ec, },
    { 31, 0xa436, 0xA10E, },
    { 31, 0xa438, 0x01ea, },
    { 31, 0xa436, 0xA10C, },
    { 31, 0xa438, 0x06a9, },
    { 31, 0xa436, 0xA10A, },
    { 31, 0xa438, 0x078a, },
    { 31, 0xa436, 0xA108, },
    { 31, 0xa438, 0x03d2, },
    { 31, 0xa436, 0xA106, },
    { 31, 0xa438, 0x067f, },
    { 31, 0xa436, 0xA104, },
    { 31, 0xa438, 0x0665, },
    { 31, 0xa436, 0xA102, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa436, 0xA100, },
    { 31, 0xa438, 0x0000, },
    { 31, 0xa436, 0xA110, },
    { 31, 0xa438, 0x00fc, },
};

static const MMD_REG Rtl8226_uc2_ramcode[] =
{
    { 31, 0xA436, 0xb87c, },
    { 31, 0xA438, 0x8530, },
    { 31, 0xA436, 0xb87e, },
    { 31, 0xA438, 0xaf85, },
    { 31, 0xA438, 0x3caf, },
    { 31, 0xA438, 0x8545, },
    { 31, 0xA438, 0xaf85, },
    { 31, 0xA438, 0x45af, },
    { 31, 0xA438, 0x8545, },
    { 31, 0xA438, 0xee82, },
    { 31, 0xA438, 0xf900, },
    { 31, 0xA438, 0x0103, },
    { 31, 0xA438, 0xaf03, },
    { 31, 0xA438, 0xb7f8, },
    { 31, 0xA438, 0xe0a6, },
    { 31, 0xA438, 0x00e1, },
    { 31, 0xA438, 0xa601, },
    { 31, 0xA438, 0xef01, },
    { 31, 0xA438, 0x58f0, },
    { 31, 0xA438, 0xa080, },
    { 31, 0xA438, 0x37a1, },
    { 31, 0xA438, 0x8402, },
    { 31, 0xA438, 0xae16, },
    { 31, 0xA438, 0xa185, },
    { 31, 0xA438, 0x02ae, },
    { 31, 0xA438, 0x11a1, },
    { 31, 0xA438, 0x8702, },
    { 31, 0xA438, 0xae0c, },
    { 31, 0xA438, 0xa188, },
    { 31, 0xA438, 0x02ae, },
    { 31, 0xA438, 0x07a1, },
    { 31, 0xA438, 0x8902, },
    { 31, 0xA438, 0xae02, },
    { 31, 0xA438, 0xae1c, },
    { 31, 0xA438, 0xe0b4, },
    { 31, 0xA438, 0x62e1, },
    { 31, 0xA438, 0xb463, },
    { 31, 0xA438, 0x6901, },
    { 31, 0xA438, 0xe4b4, },
    { 31, 0xA438, 0x62e5, },
    { 31, 0xA438, 0xb463, },
    { 31, 0xA438, 0xe0b4, },
    { 31, 0xA438, 0x62e1, },
    { 31, 0xA438, 0xb463, },
    { 31, 0xA438, 0x6901, },
    { 31, 0xA438, 0xe4b4, },
    { 31, 0xA438, 0x62e5, },
    { 31, 0xA438, 0xb463, },
    { 31, 0xA438, 0xfc04, },
    { 31, 0xA436, 0xb85e, },
    { 31, 0xA438, 0x03b3, },
    { 31, 0xA436, 0xb860, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xb862, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xb864, },
    { 31, 0xA438, 0xffff, },
    { 31, 0xA436, 0xb878, },
    { 31, 0xA438, 0x0001, },

};

static const MMD_REG Rtl8226_uc_ramcode[] =
{
    { 31, 0xa436, 0x8513, },
    { 31, 0xa438, 0xaf85, },
    { 31, 0xa438, 0x1faf, },
    { 31, 0xa438, 0x852a, },
    { 31, 0xa438, 0xaf85, },
    { 31, 0xa438, 0x2aaf, },
    { 31, 0xa438, 0x852a, },
    { 31, 0xa438, 0xa107, },
    { 31, 0xa438, 0x05ec, },
    { 31, 0xa438, 0x80af, },
    { 31, 0xa438, 0x3e78, },
    { 31, 0xa438, 0xaf3e, },
    { 31, 0xa438, 0x7600, },
    { 31, 0xa436, 0xb818, },
    { 31, 0xa438, 0x3e6f, },
    { 31, 0xa436, 0xb81a, },
    { 31, 0xa438, 0xffff, },
    { 31, 0xa436, 0xb81c, },
    { 31, 0xa438, 0xffff, },
    { 31, 0xa436, 0xb81e, },
    { 31, 0xa438, 0xffff, },
    { 31, 0xa436, 0xb832, },
    { 31, 0xa438, 0x0001, },
};

static const MMD_REG Rtl8226_data_ramcode[] =
{
    { 31, 0xB88E, 0xC091, },
    { 31, 0xB890, 0x6E12, },
    { 31, 0xB88E, 0xC092, },
    { 31, 0xB890, 0x1214, },
    { 31, 0xB88E, 0xC094, },
    { 31, 0xB890, 0x1516, },
    { 31, 0xB88E, 0xC096, },
    { 31, 0xB890, 0x171B, },
    { 31, 0xB88E, 0xC098, },
    { 31, 0xB890, 0x1B1C, },
    { 31, 0xB88E, 0xC09A, },
    { 31, 0xB890, 0x1F1F, },
    { 31, 0xB88E, 0xC09C, },
    { 31, 0xB890, 0x2021, },
    { 31, 0xB88E, 0xC09E, },
    { 31, 0xB890, 0x2224, },
    { 31, 0xB88E, 0xC0A0, },
    { 31, 0xB890, 0x2424, },
    { 31, 0xB88E, 0xC0A2, },
    { 31, 0xB890, 0x2424, },
    { 31, 0xB88E, 0xC0A4, },
    { 31, 0xB890, 0x2424, },
    { 31, 0xB88E, 0xC018, },
    { 31, 0xB890, 0x0AF2, },
    { 31, 0xB88E, 0xC01A, },
    { 31, 0xB890, 0x0D4A, },
    { 31, 0xB88E, 0xC01C, },
    { 31, 0xB890, 0x0F26, },
    { 31, 0xB88E, 0xC01E, },
    { 31, 0xB890, 0x118D, },
    { 31, 0xB88E, 0xC020, },
    { 31, 0xB890, 0x14F3, },
    { 31, 0xB88E, 0xC022, },
    { 31, 0xB890, 0x175A, },
    { 31, 0xB88E, 0xC024, },
    { 31, 0xB890, 0x19C0, },
    { 31, 0xB88E, 0xC026, },
    { 31, 0xB890, 0x1C26, },
    { 31, 0xB88E, 0xC089, },
    { 31, 0xB890, 0x6050, },
    { 31, 0xB88E, 0xC08A, },
    { 31, 0xB890, 0x5F6E, },
    { 31, 0xB88E, 0xC08C, },
    { 31, 0xB890, 0x6E6E, },
    { 31, 0xB88E, 0xC08E, },
    { 31, 0xB890, 0x6E6E, },
    { 31, 0xB88E, 0xC090, },
    { 31, 0xB890, 0x6E12, },
};

static const MMD_REG Rtl8226_isram_patch[] =
{
    { 30, 0x7223, 0x0000, },
    { 30, 0x7C00, 0xE008, },
    { 30, 0x7C01, 0xE01E, },
    { 30, 0x7C02, 0xE02C, },
    { 30, 0x7C03, 0xE052, },
    { 30, 0x7C04, 0xE055, },
    { 30, 0x7C05, 0xE057, },
    { 30, 0x7C06, 0xE059, },
    { 30, 0x7C07, 0xE05B, },
    { 30, 0x7C08, 0x9996, },
    { 30, 0x7C09, 0x49D1, },
    { 30, 0x7C0A, 0xF005, },
    { 30, 0x7C0B, 0x49D4, },
    { 30, 0x7C0C, 0xF10A, },
    { 30, 0x7C0D, 0x49D8, },
    { 30, 0x7C0E, 0xF108, },
    { 30, 0x7C0F, 0xC00F, },
    { 30, 0x7C10, 0x7100, },
    { 30, 0x7C11, 0x209C, },
    { 30, 0x7C12, 0x249C, },
    { 30, 0x7C13, 0xC009, },
    { 30, 0x7C14, 0x9900, },
    { 30, 0x7C15, 0xE004, },
    { 30, 0x7C16, 0xC006, },
    { 30, 0x7C17, 0x1900, },
    { 30, 0x7C18, 0x9900, },
    { 30, 0x7C19, 0xC602, },
    { 30, 0x7C1A, 0xBE00, },
    { 30, 0x7C1B, 0x5A48, },
    { 30, 0x7C1C, 0xE0C2, },
    { 30, 0x7C1D, 0x0004, },
    { 30, 0x7C1E, 0xE10A, },
    { 30, 0x7C1F, 0xC60D, },
    { 30, 0x7C20, 0x73C4, },
    { 30, 0x7C21, 0x49B3, },
    { 30, 0x7C22, 0xF105, },
    { 30, 0x7C23, 0x73C2, },
    { 30, 0x7C24, 0xC609, },
    { 30, 0x7C25, 0xC005, },
    { 30, 0x7C26, 0xB800, },
    { 30, 0x7C27, 0xC606, },
    { 30, 0x7C28, 0xC003, },
    { 30, 0x7C29, 0xB800, },
    { 30, 0x7C2A, 0x0544, },
    { 30, 0x7C2B, 0x0568, },
    { 30, 0x7C2C, 0xE906, },
    { 30, 0x7C2D, 0xCDE8, },
    { 30, 0x7C2E, 0xC724, },
    { 30, 0x7C2F, 0xC624, },
    { 30, 0x7C30, 0x9EE2, },
    { 30, 0x7C31, 0x1E01, },
    { 30, 0x7C32, 0x9EE0, },
    { 30, 0x7C33, 0x76E0, },
    { 30, 0x7C34, 0x49E0, },
    { 30, 0x7C35, 0xF1FE, },
    { 30, 0x7C36, 0x76E6, },
    { 30, 0x7C37, 0x486D, },
    { 30, 0x7C38, 0x4868, },
    { 30, 0x7C39, 0x9EE4, },
    { 30, 0x7C3A, 0x1E03, },
    { 30, 0x7C3B, 0x9EE0, },
    { 30, 0x7C3C, 0x76E0, },
    { 30, 0x7C3D, 0x49E0, },
    { 30, 0x7C3E, 0xF1FE, },
    { 30, 0x7C3F, 0xC615, },
    { 30, 0x7C40, 0x9EE2, },
    { 30, 0x7C41, 0x1E01, },
    { 30, 0x7C42, 0x9EE0, },
    { 30, 0x7C43, 0x76E0, },
    { 30, 0x7C44, 0x49E0, },
    { 30, 0x7C45, 0xF1FE, },
    { 30, 0x7C46, 0x76E6, },
    { 30, 0x7C47, 0x486F, },
    { 30, 0x7C48, 0x9EE4, },
    { 30, 0x7C49, 0x1E03, },
    { 30, 0x7C4A, 0x9EE0, },
    { 30, 0x7C4B, 0x76E0, },
    { 30, 0x7C4C, 0x49E0, },
    { 30, 0x7C4D, 0xF1FE, },
    { 30, 0x7C4E, 0x7196, },
    { 30, 0x7C4F, 0xC702, },
    { 30, 0x7C50, 0xBF00, },
    { 30, 0x7C51, 0x5A44, },
    { 30, 0x7C52, 0xEB0E, },
    { 30, 0x7C53, 0x0070, },
    { 30, 0x7C54, 0x00C3, },
    { 30, 0x7C55, 0x1BC0, },
    { 30, 0x7C56, 0xC602, },
    { 30, 0x7C57, 0xBE00, },
    { 30, 0x7C58, 0x0E26, },
    { 30, 0x7C59, 0xC602, },
    { 30, 0x7C5A, 0xBE00, },
    { 30, 0x7C5B, 0x0000, },
    { 30, 0x7C5C, 0xC602, },
    { 30, 0x7C5D, 0xBE00, },
    { 30, 0x7C5E, 0x0000, },
    { 30, 0x7C5F, 0xC602, },
    { 30, 0x7C60, 0xBE00, },
    { 30, 0x7C61, 0x0000, },
    { 30, 0x7C62, 0xC602, },
    { 30, 0x7C63, 0xBE00, },
    { 30, 0x7C64, 0x0000, },

    { 30, 0x7E13, 0x8000, },    // BP_BA
    { 30, 0x7E14, 0x5A46, },    // BP_0
    { 30, 0x7E16, 0x5A42, },    // BP_2
    { 30, 0x7E24, 0x0005, },    // BP_EN
};

static BOOL
Rtl8226_wait_for_bit(
    IN HANDLE hDevice,
    IN UINT16 dev,
    IN UINT16 addr,
    IN UINT16 mask,
    IN BOOL   set,
    IN UINT16 timeoutms)
{
    BOOL status = FAILURE;
    UINT16 phydata = 0;

    while (--timeoutms) {
        status = MmdPhyRead(hDevice, MMD_VEND2, addr, &phydata);
        if (!status)
            goto exit;

        if (!set)
            phydata = ~phydata;

        if ((phydata & mask) == mask)
            return 1;

        Sleep(1);
    }

    phy_osal_printf("Timeout (dev=%02x addr=0x%02x mask=0x%02x timeout=%d)\n",
          dev, addr, mask, timeoutms);

exit:
    return 0;
}

BOOLEAN
Rtl8226_phy_init(
    IN HANDLE hDevice,
    IN PHY_LINK_ABILITY *pphylinkability,
    IN BOOL singlephy
    )
{
    BOOL status = FAILURE;
    UINT16 i = 0;   /* SW_SDK: use UINT16 instead of UINT8, for MMD_REG array may over 255 entries */
    UINT16 phydata = 0;
    const UINT16 patchver = 0x15, patchaddr = 0x8024;

    // Polling PHY Status
    status = Rtl8226_wait_for_bit(hDevice, MMD_VEND2, 0xA420, 0x3, 1, 100);
    if (status != SUCCESS)
        goto exit;

    // MMD 31.0xA436[15:0] = 0x801E
    status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, 0x801E);
    if (status != SUCCESS)
        goto exit;

    status = MmdPhyRead(hDevice, MMD_VEND2, 0xA438, &phydata);
    if (status != SUCCESS)
        goto exit;

    // Already patched.
    if (phydata == patchver)
    {
        status = 1;
        goto exit;
    }
    else
    {
        // Patch request & wait patch_rdy (for normal patch flow - Driver Initialize)
        // MMD 31.0xB820[4] = 1'b1     //(patch request)
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB820, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_4;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB820, phydata);
        if (status != SUCCESS)
            goto exit;

        //wait for patch ready = 1 (MMD 31.0xB800[6])
        status = Rtl8226_wait_for_bit(hDevice, MMD_VEND2, 0xB800, BIT_6, 1, 100);
        if (status != SUCCESS)
            goto exit;

        //Set patch_key & patch_lock
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, patchaddr);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA438[15:0] = 0x8601
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA438, 0x8601);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA436[15:0] = 0xB82E
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, 0xB82E);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA438[15:0] = 0x0001
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA438, 0x0001);
        if (status != SUCCESS)
            goto exit;

        // NC & UC patch
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB820, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_7;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB820, phydata);
        if (status != SUCCESS)
            goto exit;

        // patch nc0
        for(i=0; i<sizeof(Rtl8226_n0_ramcode)/sizeof(MMD_REG); i++)
        {
            status = MmdPhyWrite(hDevice, Rtl8226_n0_ramcode[i].dev, Rtl8226_n0_ramcode[i].addr, Rtl8226_n0_ramcode[i].value);
            if (status != SUCCESS)
                goto exit;
        }

        // patch nc1
        for(i=0; i<sizeof(Rtl8226_n1_ramcode)/sizeof(MMD_REG); i++)
        {
            status = MmdPhyWrite(hDevice, Rtl8226_n1_ramcode[i].dev, Rtl8226_n1_ramcode[i].addr, Rtl8226_n1_ramcode[i].value);
            if (status != SUCCESS)
                goto exit;
        }


        // patch nc2
        for(i=0; i<sizeof(Rtl8226_n2_ramcode)/sizeof(MMD_REG); i++)
        {
            status = MmdPhyWrite(hDevice, Rtl8226_n2_ramcode[i].dev, Rtl8226_n2_ramcode[i].addr, Rtl8226_n2_ramcode[i].value);
            if (status != SUCCESS)
                goto exit;
        }

        // patch uc2
        for(i=0; i<sizeof(Rtl8226_uc2_ramcode)/sizeof(MMD_REG); i++)
        {
            status = MmdPhyWrite(hDevice, Rtl8226_uc2_ramcode[i].dev, Rtl8226_uc2_ramcode[i].addr, Rtl8226_uc2_ramcode[i].value);
            if (status != SUCCESS)
                goto exit;
        }

        // MMD 31.0xB820[7] = 1'b0
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB820, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_7);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB820, phydata);
        if (status != SUCCESS)
            goto exit;

        // patch uc
        for(i=0; i<sizeof(Rtl8226_uc_ramcode)/sizeof(MMD_REG); i++)
        {
            status = MmdPhyWrite(hDevice, Rtl8226_uc_ramcode[i].dev, Rtl8226_uc_ramcode[i].addr, Rtl8226_uc_ramcode[i].value);
            if (status != SUCCESS)
                goto exit;
        }

        // GPHY OCP 0xB896 bit[0] = 0x0
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB896, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_0);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB896, phydata);
        if (status != SUCCESS)
            goto exit;

        // GPHY OCP 0xB892 bit[15:8] = 0x0
        phydata = 0;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB892, phydata);
        if (status != SUCCESS)
            goto exit;

        // patch ram code
        for(i=0; i<sizeof(Rtl8226_data_ramcode)/sizeof(MMD_REG); i++)
        {
            status = MmdPhyWrite(hDevice, Rtl8226_data_ramcode[i].dev, Rtl8226_data_ramcode[i].addr, Rtl8226_data_ramcode[i].value);
            if (status != SUCCESS)
                goto exit;
        }

        // GPHY OCP 0xB896 bit[0] = 0x1
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB896, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_0;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB896, phydata);
        if (status != SUCCESS)
            goto exit;

        // Clear patch_key & patch_lock
        phydata = 0x0;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, phydata);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA438[15:0] = 0x0000
        phydata = 0x0;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA438, phydata);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xB82E[0] = 1'b0
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB82E, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_0);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB82E, phydata);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA436[15:0] = patch_key_addr
        phydata = patchaddr;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, phydata);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA438[15:0] = 0x0000
        phydata = 0x0;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA438, phydata);
        if (status != SUCCESS)
            goto exit;

        // Release patch request
        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB820, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_4);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB820, phydata);
        if (status != SUCCESS)
            goto exit;

        status = Rtl8226_wait_for_bit(hDevice, MMD_VEND2, 0xB800, BIT_6, 0, 100);
        if (status != SUCCESS)
            goto exit;

        // Update patch version
        phydata = 0x801E;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, phydata);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xA438[15:0] =  driver_note_ver
        phydata = patchver;
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA438, phydata);
        if (status != SUCCESS)
            goto exit;


        // MMD 31.0xa436[15:0] = 0x81B3
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa436, 0x81B3);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0043
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0043);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00A7
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00A7);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00D6
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00D6);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00EC
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00EC);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00F6
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00F6);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00FB
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00FB);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00FD
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00FD);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00FF
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00FF);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x00BB
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x00BB);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0058
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0058);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0029
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0029);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0013
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0013);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0009
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0009);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0004
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0004);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0002
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0002);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0000
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0000);
        if (status != SUCCESS)
            goto exit;

        // MMD 31.0xa438[15:0] = 0x0000
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0000);
        if (status != SUCCESS)
            goto exit;

        for (i=17; i<39; i++)
        {
            status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x0000);
            if (status != SUCCESS)
                goto exit;
        }


        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa436, 0x80ea);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xa438, 0x7843);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA436, 0x8257);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA438, 0x020F);
        if (status != SUCCESS)
            goto exit;



        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAD4E, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_4;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD4E, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAD16, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= 0x3FF;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD16, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAD32, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5));
        phydata |= 0x6;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD32, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAC08, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_12);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC08, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAC08, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_8);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC08, phydata);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyRead(hDevice, MMD_VEND2, 0xACC0, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_0 | BIT_1));
        phydata |= BIT_1;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xACC0, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAD40, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_5 | BIT_6 | BIT_7));
        phydata |= BIT_6;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD40, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAD40, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_0 | BIT_1 | BIT_2));
        phydata |= BIT_2;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD40, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAC14, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_7);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC14, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAC80, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_8 | BIT_9));

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC80, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAC5E, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_0 | BIT_1 | BIT_2));
        phydata |= BIT_1;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC5E, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD4C, 0x00A8);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC5C, 0x01FF);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xAC8A, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_4 | BIT_5 | BIT_6 | BIT_7));
        phydata |= (BIT_4 | BIT_5);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAC8A, phydata);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB87C, 0x80A2);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB87E, 0x0153);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB87C, 0x809C);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB87E, 0x0153);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyRead(hDevice, MMD_VEND2, 0xD068, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_13;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xD068, phydata);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyRead(hDevice, MMD_VEND2, 0xA442, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_11;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA442, phydata);
        if (status != SUCCESS)
            goto exit;

        status = MmdPhyRead(hDevice, MMD_VEND2, 0xA432, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_4;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA432, phydata);
        if (status != SUCCESS)
            goto exit;


        status = MmdPhyRead(hDevice, MMD_VEND2, 0xB54C, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~(BIT_8 | BIT_9 | BIT_10 | BIT_11 | BIT_12 | BIT_13 | BIT_14 | BIT_15));
        phydata |= 0xDB00;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xB54C, phydata);
        if (status != SUCCESS)
            goto exit;


                status = MmdPhyRead(hDevice, MMD_VEND2, 0xA5D4, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata |= BIT_5;

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA5D4, phydata);
        if (status != SUCCESS)
            goto exit;


                status = MmdPhyRead(hDevice, MMD_VEND2, 0xAD4E, &phydata);
        if (status != SUCCESS)
            goto exit;

        phydata &= (~BIT_4);

        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xAD4E, phydata);
        if (status != SUCCESS)
            goto exit;


        if (singlephy)
        {
            for(i=0; i<sizeof(Rtl8226_isram_patch)/sizeof(MMD_REG); i++)
            {
                status = MmdPhyWrite(hDevice, Rtl8226_isram_patch[i].dev, Rtl8226_isram_patch[i].addr, Rtl8226_isram_patch[i].value);
                if (status != SUCCESS)
                    goto exit;
            }
        }
#if 0
        // Set initial speed as 2.5G, enable all speed cap.
        {
            PHY_LINK_ABILITY phylinkability;
            memset(&phylinkability, 0x0, sizeof(phylinkability));

            phylinkability.Half_10 = TRUE;
            phylinkability.Full_10 = TRUE;

            phylinkability.Half_100 = TRUE;
            phylinkability.Full_100 = TRUE;

            phylinkability.Full_1000 = TRUE;

            phylinkability.adv_2_5G = TRUE;

            phylinkability.FC = TRUE;
            phylinkability.AsyFC = TRUE;

            status = Rtl8226_autoNegoAbility_set(hDevice, &phylinkability);
            if (status != SUCCESS)
                goto exit;
        }

#else
        status = Rtl8226_autoNegoAbility_set(hDevice, pphylinkability);
        if (status != SUCCESS)
            goto exit;
#endif
        // PHYRST & Restart Nway
        status = MmdPhyWrite(hDevice, MMD_VEND2, 0xA400, 0x9200);
        if (status != SUCCESS)
            goto exit;
    }

exit:
    return status;
}

