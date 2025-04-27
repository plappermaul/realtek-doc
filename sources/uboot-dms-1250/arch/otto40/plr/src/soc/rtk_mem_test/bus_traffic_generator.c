#include <soc.h>
#include "bus_traffic_generator.h"

#define BTG_WRITE 0
#define BTG_READ  1

#define BTG_RCHK_NONE 0
#define BTG_RCHK_DGEN 1
#define BTG_RCHK_CSUM 2

#define BTG_REC_FIRST  0
#define BTG_REC_LATEST 1

#define BTG_IMPRECISE 0
#define BTG_PRECISE   1

#define BTG_FINITE   0
#define BTG_INFINITE 1

#define BTG_BLEN_16  0
#define BTG_BLEN_32  1
#define BTG_BLEN_64  2
#define BTG_BLEN_128 3

#define GDMAREG(req, oft) (*((volatile u32_t *)(gdma_get_base(req) + oft)))
#define BTGREG(req, oft)  (*((volatile u32_t *)(btg_get_base(req) + oft)))
#define BTGIVEC(req, n)   (*((volatile u32_t *)(btg_get_base(req) + 0x30 + (n*4))))

/* Provided outside. */
#define BTGLX1_BASE (0xb800a000)
#define BTGLX2_BASE (0xb8018000)


btg_para_v3_t btg_fpga[] = {
    {
		.gbase = BTGLX1_BASE + 0x100,
		.lxid  = 1,
		.feat  = BTG_WRITE,
        .burstlen = 1, /* 0: 16B; 1: 32B, 2: 64B, 3: 128B */
        .pktlen = 0x20,  /* from 32B ~ 8160B in a unit of 32B. */
        .resp_time = 0x80,  /* in cycles. */
        .perid_time = 0x80, /* in cycles. */
        .addr_base = BTG_LX1_ADDR_PHY,
        .addr_mask = BTG_SIZE_W,
        .addr_gap = 0x20,
	}, 
	{
		.gbase = BTGLX1_BASE + 0x200,
		.lxid  = 1,
		.feat  = BTG_READ,
        .rchkmode = 1, /* 0: NONE; 1: data gen.; 2: checksum; 3: undefined. */
        .burstlen = 1, /* 0: 16B; 1: 32B, 2: 64B, 3: 128B */
        .pktlen = 0x20,  /* from 32B ~ 8160B in a unit of 32B. */
        .resp_time = 0x80,  /* in cycles. */
        .perid_time = 0x80, /* in cycles. */
        .addr_base = BTG_LX1_ADDR_PHY,
        .addr_mask = BTG_SIZE_R,
        .addr_gap = 0x20,
	},
	{
		.gbase = BTGLX2_BASE + 0x100,
		.lxid  = 2,
		.feat  = BTG_WRITE,
        .burstlen = 1, /* 0: 16B; 1: 32B, 2: 64B, 3: 128B */
        .pktlen = 0x20,  /* from 32B ~ 8160B in a unit of 32B. */
        .resp_time = 0x80,  /* in cycles. */
        .perid_time = 0x80, /* in cycles. */
        .addr_base = BTG_LX2_ADDR_PHY,
        .addr_mask = BTG_SIZE_W,
        .addr_gap = 0x20,
	}, 
	{
		.gbase = BTGLX2_BASE + 0x200,
		.lxid  = 2,
		.feat  = BTG_READ,
        .rchkmode = 1, /* 0: NONE; 1: data gen.; 2: checksum; 3: undefined. */
        .burstlen = 1, /* 0: 16B; 1: 32B, 2: 64B, 3: 128B */
        .pktlen = 0x20,  /* from 32B ~ 8160B in a unit of 32B. */
        .resp_time = 0x80,  /* in cycles. */
        .perid_time = 0x80, /* in cycles. */
        .addr_base = BTG_LX2_ADDR_PHY,
        .addr_mask = BTG_SIZE_R,
        .addr_gap = 0x20,
	},
    {
		/* Sentinal entry guards end of list. */
		.gbase = 0xffff,
	}
};


inline static u32_t gdma_get_base(btg_para_v3_t *req) {
	return (req->gbase & 0xfffff000);
}

inline static u32_t btg_get_base(btg_para_v3_t *req) {
	return req->gbase;
}

void _btg_setup(btg_para_v3_t *req) {
	u32_t infinite = 0;
	u32_t pktlen = req->pktlen;

	if (pktlen > 8160) {
		puts("WW: packet length is capped to 8160B\n");
		pktlen = 8160;
		req->pktlen = pktlen;
	}

	/* if (pktlen & 0x1F) { */
	/* 	puts("WW: packet length is changed to 32B-alignment\n"); */
	/* 	pktlen &= (~(0x1F)); */
		req->pktlen = pktlen;
	/* } */

	if (req->iter == 0) {
		infinite = 1;
	}

	BTGREG(req, BTGCRo)  = ((req->rchkmode << 24) | (req->rec_latest << 22) |
	                        (req->precise  << 21) | (infinite << 20)        |
	                        (req->burstlen << 16) | pktlen);
	BTGREG(req, BTGIRo)  = req->iter;
	BTGREG(req, BTGRTRo) = req->resp_time;
	BTGREG(req, BTGPRo)  = req->perid_time;
	BTGREG(req, BTGBARo) = req->addr_base & (~(0xF0000000)); /* get physical address. */
	BTGREG(req, BTGAMRo) = req->addr_mask;

	if (req->addr_gap < 0) {
		req->addr_gap *= -1;
		BTGREG(req, BTGGRo) = 0x80000000 | req->addr_gap;
	} else  {
		BTGREG(req, BTGGRo) = req->addr_gap;
	}

	/* Input Vector setup */
	BTGIVEC(req, 0)  = 0xa5a55a5a;
	BTGIVEC(req, 1)  = 0xffff0000;
	BTGIVEC(req, 2)  = 0x0000ffff;
	BTGIVEC(req, 3)  = 0xff00ff00;
	BTGIVEC(req, 4)  = 0x00ff00ff;
	BTGIVEC(req, 5)  = 0x5a5aa5a5;
	BTGIVEC(req, 6)  = 0x01234567;
	BTGIVEC(req, 7)  = 0x89abcdef;
	BTGIVEC(req, 8)  = 0xaaaa5555;
	BTGIVEC(req, 9)  = 0x5555aaaa;
	BTGIVEC(req, 10) = 0xa5a55a5a;
	BTGIVEC(req, 11) = 0xffff0000;
	BTGIVEC(req, 12) = 0x0000ffff;
	BTGIVEC(req, 13) = 0xff00ff00;
	BTGIVEC(req, 14) = 0x00ff00ff;
	BTGIVEC(req, 15) = 0x5a5aa5a5;

	return;
}


#define ISBTGAVAL(x) ((x->gbase & 0xb8000000) == 0xb8000000)
#define ISBTGEND(x)  (x->gbase == 0xffff)

static void _btg_start(btg_para_v3_t *req) {
	volatile u32_t *region;

	printf("II: BTG-LX%d(%s) ",
	       req->lxid,
	       (req->feat == BTG_WRITE) ? "WR" : "RD");

	if (!ISBTGAVAL(req)) {
		puts("is NOT avaiable\n");
	} else {
		printf("@ 0x%08x: 0x%08x ~ 0x%08x... ",
		       req->gbase, req->addr_base, (req->addr_base | req->addr_mask));

		if (req->feat == BTG_WRITE) {
			region = (u32_t *)(req->addr_base | 0xa0000000);
			while ((u32_t)region < (req->addr_base | req->addr_mask | 0xa0000000)) {
				*region++ = 0x77777777;
			}
		}

		BTGREG(req, BTGCRo) |= 0x80000000;
		puts("enabled\n");
	}

	return;
}

static void _btg_stop(btg_para_v3_t *req) {
	printf("II: BTG-LX%d(%s) ",
	       req->lxid,
	       (req->feat == BTG_WRITE) ? "WR" : "RD");

	if (!ISBTGAVAL(req)) {
		puts("is NOT avaiable\n");
	} else {
		printf("@ 0x%08x: 0x%08x ~ 0x%08x... ",
		       req->gbase, req->addr_base, (req->addr_base | req->addr_mask));
		BTGREG(req, BTGCRo) &= 0x7FFFFFFF;
		puts("stopped\n");
	}

	return;
}

static void _btg_show_reg(btg_para_v3_t *req) {
	volatile u32_t *btgreg = (volatile u32_t *)req->gbase;

	u32_t i = 0;

	if (!ISBTGAVAL(req)) {
		printf("II: BTG-LX%d(%s) is NOT available\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD");
	} else {
		printf("II: BTG-LX%d(%s) @ 0x%08x:\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD",
		       req->gbase);
		while (i < 28) {
			printf("II:   %p: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			       btgreg,
			       *(btgreg+0), *(btgreg+1), *(btgreg+2), *(btgreg+3));
			btgreg += 4;
			i += 4;
		}
	}
	return;
}

static void _btg_para_decode(btg_para_v3_t *req) {
	u32_t isr, isr_offset;

	if (!ISBTGAVAL(req)) {
		printf("II: BTG-LX%d(%s) is NOT available\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD");
	} else {
		isr = GDMAREG(req, GDMAISRo);
		isr_offset = (req->feat == BTG_WRITE) ? 0 : 4;

		printf("II: BTG-LX%d(%s) @ 0x%08x Configuration:=========\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD",
		       req->gbase);
		printf("II:   rec_latest: %d\tprecise: %d\n", req->rec_latest, req->precise);
		printf("II:   rchkmode:  %d\tburstlen: %d B\n", req->rchkmode, 1 << (4+req->burstlen));
		printf("II:   pktlen: %d B\titer: %d\n", req->pktlen, req->iter);
		printf("II:   resp_time: %d C\tperid_time: %d C\n", req->resp_time, req->perid_time);
		printf("II:   addr_base: %08x\taddr_mask: %08x\n", req->addr_base, req->addr_mask);
		printf("II:   addr_gap: %08x\n", req->addr_gap);
		printf("II:   covered range: %08x ~ %08x\n",
		       req->addr_base, (req->addr_base | req->addr_mask));
		puts("II: Status:\n");
		printf("II:   Int. Status Reg.: %08x\n", isr);
		printf("II:   In progress? %c\n", (BTGREG(req, BTGCRo) & 0x80000000) ? 'Y' : 'N');
		printf("II:   Failed response time: Intr.(%d);  Addr.(0x%08x)\n",
		       (isr >> (20 + isr_offset)) & 0x1, BTGREG(req, BTGRFARo));
		printf("II:   Failed period       : Intr.(%d);  Addr.(0x%08x)\n",
		       (isr >> (21 + isr_offset)) & 0x1, BTGREG(req, BTGPFARo));
		if (req->feat == BTG_READ) {
		    printf("II:   Failed verification : Intr.(%d);  Addr.(0x%08x)\n",
			       (isr >> 26) & 0x1, BTGREG(req, BTGVFARo));
		}
		printf("II:   Max response time: %d(0x%x) cycles\n\n",
		       BTGREG(req, BTGMRTRo), BTGREG(req, BTGMRTRo));
	}
	return;
}

void static inline _btg_reset(void) {
	btg_para_v3_t *btg_entry = &btg_fpga[0];

	printf("II: Resetting ");
	while (btg_entry->gbase != 0xffff) {
		printf("BTG-LX%d... ", btg_entry->lxid);
		if ((btg_entry->gbase & 0xfffff000) == 0) {
			btg_entry += 2;
			puts("N/A; ");
			continue;
		}

		/* Reset GDMA and BTG */
		GDMAREG(btg_entry, GDMACRo) = 0x00000000;
		GDMAREG(btg_entry, GDMACRo) = 0x10000000;
		GDMAREG(btg_entry, GDMACRo) = 0x90000000;

		/* Reset Interrupt bits */
		GDMAREG(btg_entry, GDMAIMRo) = 0x07300000;
		GDMAREG(btg_entry, GDMAISRo) = 0x07300000;
		GDMAREG(btg_entry, GDMAISRo) = 0x00000000;

		BTGREG(btg_entry, BTGCRo) = 0;
		btg_entry++;
		BTGREG(btg_entry, BTGCRo) = 0;
		puts("done; ");
		btg_entry++;
	}

	puts("\n");

	return;
}

void btg_controller_init(void)
{
    _btg_reset();
    
    btg_para_v3_t *btg_entry = &btg_fpga[0];
	while (!ISBTGEND(btg_entry)) {
        _btg_setup(btg_entry);
		btg_entry++;
    }
    #if 0    
    btg_entry = &btg_fpga[0];
	while (!ISBTGEND(btg_entry)) {
        _btg_show_reg(btg_entry);
		btg_entry++;
    }
    #endif
}


void btg_status(void)
{
    btg_para_v3_t *btg_entry = &btg_fpga[0];
    while (!ISBTGEND(btg_entry)) {
        _btg_para_decode(btg_entry);
		btg_entry++;
    }   
}
void lx1_btg_start(void)
{
    btg_para_v3_t *btg_entry = &btg_fpga[0];
        _btg_start(btg_entry);
		btg_entry++;
    _btg_start(btg_entry);
    }

void lx2_btg_start(void)
{
    btg_para_v3_t *btg_entry = &btg_fpga[2];
    while (!ISBTGEND(btg_entry)) {
        _btg_start(btg_entry);
		btg_entry++;
}
}


void lx1_btg_stop(void)
{
    btg_para_v3_t *btg_entry = &btg_fpga[0];
    _btg_stop(btg_entry);
    btg_entry++;
    _btg_stop(btg_entry);
}

void lx2_btg_stop(void)
{
    btg_para_v3_t *btg_entry = &btg_fpga[2];
    while (!ISBTGEND(btg_entry)) {
        _btg_stop(btg_entry);
        btg_entry++;
    }
}

extern void ddr_memory_test(void);
void btg_test()
{   
    printf("II: Start %s\n");

    u32_t bst_len,i;

    for(bst_len=0; bst_len<=3; bst_len++){
        for(i=0 ; i<4 ; i++){
            btg_fpga[i].burstlen = bst_len;
        }

        printf("    [PLR] BTG Initial Status\n");
        btg_controller_init();
        btg_status();
        
        printf("    [PLR] Enable BTG ...................\n");
        lx1_btg_start();
//        lx2_btg_start();
        
        ddr_memory_test();
        
        btg_status();
        lx1_btg_stop();
//        lx2_btg_stop();
    }

    printf("\n\n    [PLR] Finish bus_traffic_generator.c\n");
    return;
}
REG_INIT_FUNC(btg_test, 35);
REG_INIT_FUNC(btg_status, 36);


