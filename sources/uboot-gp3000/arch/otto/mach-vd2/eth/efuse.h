#ifndef __EFUSE_H__
#define __EFUSE_H__

#define EFUSE_ADDR 				0xb8000644
#define EFUSE_ADDR1 			0xb8000648
#define EFUSE_REG_OFFSET 		(16)
#define GPHY_INFO_EFUSE_ADDR 	(7)

typedef union {
    struct {
        uint16_t resvd_15:1;
        uint16_t port_4:3;
        uint16_t port_3:3;
        uint16_t port_2:3;
        uint16_t port_1:3;
        uint16_t port_0:3;
    } f;
    uint16_t v;
} EFUSE_GPHY_CAP_T;

enum efuse_gphy_cap {
    gphyCap_GE = 0,
    gphyCap_FE = 1,
};

uint16_t efuse_read(unsigned char regaddr);
uint16_t efuse_get_phy_cap(int port);

#endif //__EFUSE_H__
