
#ifndef __NIC_RTL8251B_INIT_H__
#define __NIC_RTL8251B_INIT_H__

BOOLEAN
Rtl8251b_phy_init(
    IN HANDLE hDevice,
    IN PHY_LINK_ABILITY *pphylinkability,
    IN BOOL singlephy
    );

#endif




