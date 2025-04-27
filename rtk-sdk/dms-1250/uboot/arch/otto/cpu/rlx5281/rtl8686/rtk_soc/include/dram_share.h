#ifndef __DRAM_SHARE_H__
#define __DRAM_SHARE_H__

#define DRAM_SHARE_REG_BASE	(0xB8001700)
#define C0DOR0			(DRAM_SHARE_REG_BASE + 0x00) //CPU0 DRAM Offset Register of Zone 0
#define C0DMAR0 		(DRAM_SHARE_REG_BASE + 0x04) //CPU0 DRAM Maximal Address Register of Zone 0
#define C0DOR1			(DRAM_SHARE_REG_BASE + 0x10) //CPU0 DRAM Offset Register of Zone 1
#define C0DMAR1 		(DRAM_SHARE_REG_BASE + 0x14) //CPU0 DRAM Maximal Address Register of Zone 1
#define C0DOR2			(DRAM_SHARE_REG_BASE + 0x20) //CPU0 DRAM Offset Register of Zone 2
#define C0DMAR2 		(DRAM_SHARE_REG_BASE + 0x24) //CPU0 DRAM Maximal Address Register of Zone 2
#define C0RCR			(DRAM_SHARE_REG_BASE + 0x30) //CPU0 Range Control Register
#define C0ILAR			(DRAM_SHARE_REG_BASE + 0x34) //CPU0 Illegal Logical Address Register

#define C1DOR0			(DRAM_SHARE_REG_BASE + 0x40) //CPU1 DRAM Offset Register of Zone 0
#define C1DMAR0 		(DRAM_SHARE_REG_BASE + 0x44) //CPU1 DRAM Maximal Address Register of Zone 0
#define C1DOR1			(DRAM_SHARE_REG_BASE + 0x50) //CPU1 DRAM Offset Register of Zone 1
#define C1DMAR1 		(DRAM_SHARE_REG_BASE + 0x54) //CPU1 DRAM Maximal Address Register of Zone 1
#define C1DOR2			(DRAM_SHARE_REG_BASE + 0x60) //CPU1 DRAM Offset Register of Zone 2
#define C1DMAR2 		(DRAM_SHARE_REG_BASE + 0x64) //CPU1 DRAM Maximal Address Register of Zone 2
#define C1RCR			(DRAM_SHARE_REG_BASE + 0x70) //CPU1 Range Control Register
#define C1ILAR			(DRAM_SHARE_REG_BASE + 0x74) //CPU1 Illegal Logical Address Register

#define ZONE0_BASE		(0x00000000)
#define ZONE1_BASE		(0x10000000)
#define ZONE2_BASE		(0x20000000)
#define ZONE_MAX		(0x7FFFFFFF)
#define PHYSICAL_SPACE_SIZE	(0x80000000)

unsigned int _DS_vir_to_bus_addr(unsigned int vir_addr);
unsigned int _DS_bus_to_phy_addr(unsigned int bus_addr);
unsigned int _tlb_trans(unsigned int vir_addr);
int is_Master_cpu(void);
#endif //end of __DRAM_SHARE_H__
