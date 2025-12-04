#include <common.h>

#include "asicregs.h"
#include "efuse.h"

uint16_t efuse_read(unsigned char regaddr)
{
    REG32(EFUSE_ADDR)   = (1<<24) | ((regaddr&0x1f)<<EFUSE_REG_OFFSET);
    REG32(EFUSE_ADDR)   = (0<<24) | ((regaddr&0x1f)<<EFUSE_REG_OFFSET);
    mdelay(10);

    return (REG32(EFUSE_ADDR1)&0x0ffff);
}

uint16_t efuse_get_phy_cap(int port)
{
    EFUSE_GPHY_CAP_T efuse_gphy_cap;
    uint16_t tmp = 0;

    efuse_gphy_cap.v = efuse_read(GPHY_INFO_EFUSE_ADDR);

    switch (port) {
    case 0:
        tmp = efuse_gphy_cap.f.port_0;
        break;

    case 1:
        tmp = efuse_gphy_cap.f.port_1;
        break;

    case 2:
        tmp = efuse_gphy_cap.f.port_2;
        break;

    case 3:
        tmp = efuse_gphy_cap.f.port_3;
        break;

    case 4:
        tmp = efuse_gphy_cap.f.port_4;
        break;
    }

    if (tmp == gphyCap_GE)
        return 0x0;
    else
        return 0xc;
}
