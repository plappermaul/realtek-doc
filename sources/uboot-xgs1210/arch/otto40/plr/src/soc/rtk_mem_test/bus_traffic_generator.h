#ifndef BUS_TRAFFIC_GENERATOR_H
#define BUS_TRAFFIC_GENERATOR_H

#define GDMACRo   0x00
#define GDMAIMRo  0x04
#define GDMAISRo  0x08

#define BTGCRo   0x00
#define BTGIRo   0x04
#define BTGRTRo  0x08
#define BTGPRo   0x0C
#define BTGBARo  0x10
#define BTGAMRo  0x14
#define BTGGRo   0x18
#define BTGCARo  0x1C
#define BTGRFARo 0x20
#define BTGPFARo 0x24
#define BTGVFARo 0x28
#define BTGMRTRo 0x2c

typedef struct {
	const u32_t gbase;
	const u32_t lxid:2;
	const u32_t feat:1;/* BTG_WRITE & BTG_READ */
	u32_t rec_latest:1;
	u32_t precise:1;
	u32_t rchkmode:2; /* 0: NONE; 1: data gen.; 2: checksum; 3: undefined. */
	u32_t burstlen:3; /* 0: 16B; 1: 32B, 2: 64B, 3: 128B */
	u32_t pktlen:14;  /* from 32B ~ 8160B in a unit of 32B. */
	u32_t iter;       /* 0: infinite test */
	u32_t resp_time;  /* in cycles. */
	u32_t perid_time; /* in cycles. */
	u32_t addr_base;
	u32_t addr_mask;
	s32_t  addr_gap;
} btg_para_v3_t;

void btg_verification(void);

#endif
