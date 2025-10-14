/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * Unless you and Realtek execute a separate written software license
 * agreement governing use of this software, this software is licensed
 * to you under the terms of the GNU General Public License version 2,
 * available at https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 * Purpose : RTL8367C switch low-level function for access register
 * Feature : SMI related functions
 *
 */
 #include <linux/phy.h>
 #include <linux/of_mdio.h>
 #include "./rtl8226_typedef.h"
 #include <linux/spinlock.h>
 //#include "./rtl8226b_mdio.h"
 
/* default PHY ID 1 */
#define MDC_MDIO_PHY_ID (6)

#define MII_ADDR_C45 (1<<30)

extern spinlock_t reg_access;

extern int qca_mdio_read(unsigned int dev_id, unsigned int phy_addr, unsigned int reg, unsigned int *data);
extern int qca_mdio_write(unsigned int dev_id, unsigned int phy_addr, unsigned int reg, unsigned int data);

/*mii_mgr_read/mii_mgr_write is the callback API for rtl8367 driver*/
unsigned int mii_mgr_read(unsigned int phy_addr, unsigned int phy_register, unsigned int *read_data)
{
	spin_lock_bh(&reg_access);
	qca_mdio_read(1, phy_addr, phy_register, read_data);
	spin_unlock_bh(&reg_access);

	return 0;
}

unsigned int mii_mgr_write(unsigned int phy_addr,  unsigned int phy_register, unsigned int write_data)
{
	spin_lock_bh(&reg_access);
	qca_mdio_write(1, phy_addr, phy_register, write_data);
	spin_unlock_bh(&reg_access);
	return 0;
}

int rtl8226_mdio_write(unsigned short devad, int regnum, unsigned short val)
{
	regnum = MII_ADDR_C45 | ((devad & 0x1f) << 16) | (regnum & 0xffff);

	mii_mgr_write(MDC_MDIO_PHY_ID, regnum, val);
	return SUCCESS;
}

int rtl8226_mdio_read(unsigned short devad, int regnum, unsigned short *data)
{
	unsigned int temp_data = 0;
	regnum = MII_ADDR_C45 | (devad << 16) | (regnum & 0xffff);

	mii_mgr_read(MDC_MDIO_PHY_ID, regnum, &temp_data);
	if (data)
		*data = temp_data;

	return SUCCESS;
}

BOOLEAN
MmdPhyRead( IN  HANDLE hDevice, IN  UINT16 dev, IN  UINT16 addr, OUT UINT16 *data)
{
	return rtl8226_mdio_read(dev, (int)addr, data);
}

BOOLEAN
MmdPhyWrite( IN HANDLE hDevice, IN UINT16 dev, IN UINT16 addr, IN UINT16 data)
{
	return rtl8226_mdio_write(dev, addr, data);
}
