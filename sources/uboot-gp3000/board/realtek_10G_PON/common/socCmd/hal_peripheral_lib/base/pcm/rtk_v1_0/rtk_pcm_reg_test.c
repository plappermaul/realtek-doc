


#include "basic_types.h"
#include "diag.h"
//#include "rtk_hal_pcm.h"
#include "section_config.h"
#include "peripheral.h"






typedef struct reg_entry {
	char * name;
	unsigned int reg_addr;
	unsigned int reg_val_na;	// We don't care init value at this bit. 
								// test function will fix this field according to RESV bit

	unsigned int reg_ro_mask;	// MUST be a correct value
	unsigned int reg_resv_mask; // MIST be a correct value
	unsigned int reg_rw_mask;	// test function will fix this field
} REG_ENTRY_T;

REG_ENTRY_T pcm_reg_tbl[] = {
/*   name           reg_addr    don't care  ro_mask     resv_mask   rw_mask     */
	{"REG_PCM_CTL",		REG_PCM_CTL,		0x000000FF,	0x00000000,	0xFFFC0C00,	0x00000000	},
	{"REG_PCM_CHCNR03",	REG_PCM_CHCNR03,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_TSR03",	REG_PCM_TSR03,	0x00000000,	0x00000000,	0xE0E0E0E0,	0x00000000	},
	{"REG_PCM_BSIZE03",	REG_PCM_BSIZE03,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH0TXBSA",	REG_PCM_CH0TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH1TXBSA",	REG_PCM_CH1TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH2TXBSA",	REG_PCM_CH2TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH3TXBSA",	REG_PCM_CH3TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH0RXBSA",	REG_PCM_CH0RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH1RXBSA",	REG_PCM_CH1RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH2RXBSA",	REG_PCM_CH2RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH3RXBSA",	REG_PCM_CH3RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_IMR03",	REG_PCM_IMR03,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_ISR03",	REG_PCM_ISR03,	0x00000000,	0xffffffff,	0x00000000,	0x00000000	},
	{"REG_PCM_CHCNR47",	REG_PCM_CHCNR47,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_TSR47",	REG_PCM_TSR47,	0x00000000,	0x00000000,	0xE0E0E0E0,	0x00000000	},
	{"REG_PCM_BSIZE47",	REG_PCM_BSIZE47,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH4TXBSA",	REG_PCM_CH4TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH5TXBSA",	REG_PCM_CH5TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH6TXBSA",	REG_PCM_CH6TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH7TXBSA",	REG_PCM_CH7TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH4RXBSA",	REG_PCM_CH4RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH5RXBSA",	REG_PCM_CH5RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH6RXBSA",	REG_PCM_CH6RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH7RXBSA",	REG_PCM_CH7RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_IMR47",	REG_PCM_IMR47,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_ISR47",	REG_PCM_ISR47,	0x00000000,	0xffffffff,	0x00000000,	0x00000000	},
	{"REG_PCM_CHCNR811",	REG_PCM_CHCNR811,	0x00000000,	0x00000000,	0x08080808,	0x00000000	},
	{"REG_PCM_TSR811",	REG_PCM_TSR811,	0x00000000,	0x00000000,	0xE0E0E0E0,	0x00000000	},
	{"REG_PCM_BSIZE811",	REG_PCM_BSIZE811,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH8TXBSA",	REG_PCM_CH8TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH9TXBSA",	REG_PCM_CH9TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH10TXBSA",	REG_PCM_CH10TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH11TXBSA",	REG_PCM_CH11TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH8RXBSA",	REG_PCM_CH8RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH9RXBSA",	REG_PCM_CH9RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH10RXBSA",	REG_PCM_CH10RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH11RXBSA",	REG_PCM_CH11RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_IMR811",	REG_PCM_IMR811,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_ISR811",	REG_PCM_ISR811,	0x00000000,	0xffffffff,	0x00000000,	0x00000000	},
	{"REG_PCM_CHCNR1215",	REG_PCM_CHCNR1215,	0x00000000,	0x00000000,	0x08080808,	0x00000000	},
	{"REG_PCM_TSR1215",	REG_PCM_TSR1215,	0x00000000,	0x00000000,	0xE0E0E0E0,	0x00000000	},
	{"REG_PCM_BSIZE1215",	REG_PCM_BSIZE1215,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH12TXBSA",	REG_PCM_CH12TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH13TXBSA",	REG_PCM_CH13TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH14TXBSA",	REG_PCM_CH14TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH15TXBSA",	REG_PCM_CH15TXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH12RXBSA",	REG_PCM_CH12RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH13RXBSA",	REG_PCM_CH13RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH14RXBSA",	REG_PCM_CH14RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_CH15RXBSA",	REG_PCM_CH15RXBSA,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_IMR1215",	REG_PCM_IMR1215,	0x00000000,	0x00000000,	0x00000000,	0x00000000	},
	{"REG_PCM_ISR1215",	REG_PCM_ISR1215,	0x00000000,	0xffffffff,	0x00000000,	0x00000000	},
	{"REG_PCM_INTMAP",	REG_PCM_INTMAP,	0x00000000,	0x00000000,	0xFFFF0000,	0x00000000	},
	{"REG_PCM_WTSR03",	REG_PCM_WTSR03,	0x00000000,	0x00000000,	0xE0E0E0E0,	0x00000000	},
	{"REG_PCM_WTSR47",	REG_PCM_WTSR47,	0x00000000,	0x00000000,	0xE0E0E0E0,	0x00000000	},
	{"REG_PCM_RX_BUFOW",	REG_PCM_RX_BUFOW,	0x00000000,	0x0000FFFF,	0xFFFF0000,	0x00000000	},
};


int reset_pcm(int pcm_index)
{
    int i;

    // REG_PCM_CTL bit 12 1 -> 0 reset all logic and registers to initial state
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CTL, 1<<12);
    i=1000000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CTL, 0);
    i=1000000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CTL, 1<<12);
    i=1000000;
    while (i) {
        i--;
        asm volatile ("nop\n\t");
    }

    //clear buff address value
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH0TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH1TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH2TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH3TXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH0RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH1RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH2RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH3RXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH4TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH5TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH6TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH7TXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH4RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH5RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH6RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH7RXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH8TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH9TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH10TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH11TXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH8RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH9RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH10RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH11RXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH12TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH13TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH14TXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH15TXBSA, 0);

    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH12RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH13RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH14RXBSA, 0);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_CH15RXBSA, 0);

    //wirte 1 clear isr status
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_ISR03, 0xffffffff);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_ISR47, 0xffffffff);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_ISR811, 0xffffffff);
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_ISR1215, 0xffffffff);

    //clear interrupt map
    HAL_PCMX_WRITE32(pcm_index, REG_PCM_INTMAP, 0);
    return 0;

}

void print_pcm_regs_tbl(void)
{
    int reg;

    DiagPrintf("REG ADDR    Don't care  RO MASK     RESV MASK   RW MASK\n");
    DiagPrintf("===========================================================\n");
    for (reg=0; reg<sizeof(pcm_reg_tbl)/sizeof(REG_ENTRY_T);reg++) {

        DiagPrintf("0x%x  ",pcm_reg_tbl[reg].reg_addr);
        DiagPrintf("0x%x  ",pcm_reg_tbl[reg].reg_val_na);
        DiagPrintf("0x%x  ",pcm_reg_tbl[reg].reg_ro_mask);
        DiagPrintf("0x%x  ",pcm_reg_tbl[reg].reg_resv_mask);
        DiagPrintf("0x%x\n",pcm_reg_tbl[reg].reg_rw_mask);
    }
    DiagPrintf("\n");
}

int check_pcm_reg(int TestTimes)
{
    int i,j,reg, err_cnt;
    unsigned int rw,R,W,org;
    int pcm_index=0;

    DiagPrintf( "=== Verify init value =========\n\n");
    DiagPrintf( "%s(%d) Total %d registers\n",
                 __FUNCTION__,__LINE__,sizeof(pcm_reg_tbl)/sizeof(REG_ENTRY_T));

    reset_pcm(pcm_index);

    // RST bit should be dont't care
    pcm_reg_tbl[0].reg_val_na |= 0x1000;

    for (reg=0; reg<sizeof(pcm_reg_tbl)/sizeof(REG_ENTRY_T);reg++) {
        /* modify reg_val_na mask. 
         * reserve bit is also don't care bit
         */
        pcm_reg_tbl[reg].reg_val_na |= pcm_reg_tbl[reg].reg_resv_mask;

        /* modify reg_rw_mask mask. 
         * exclude RO bit and reserved bit
         */
        pcm_reg_tbl[reg].reg_rw_mask |= 
            ~(pcm_reg_tbl[reg].reg_resv_mask | pcm_reg_tbl[reg].reg_ro_mask | pcm_reg_tbl[reg].reg_val_na);

    }

    //print_pcm_regs();

    err_cnt = 0;
    // verify init value
    for (reg=0; reg<sizeof(pcm_reg_tbl)/sizeof(REG_ENTRY_T);reg++) {

        rw = HAL_PCMX_READ32(pcm_index, pcm_reg_tbl[reg].reg_addr);

        DiagPrintf( "Checking 0x%x = 0x%x (0x%x)\n",
            pcm_reg_tbl[reg].reg_addr, rw, ~(pcm_reg_tbl[reg].reg_val_na));

        rw &= ~(pcm_reg_tbl[reg].reg_val_na);

        if (rw != 0) {
            // incorrect
            err_cnt++;
            DiagPrintf ("\nError! Incorrect init value:   %s = 0x%x != 0x0\n\n", 
                pcm_reg_tbl[reg].name, rw);
        }
    }

    if (err_cnt!=0) {
        DiagPrintf("\nREG init value test fail. Error count : %d\n\n",err_cnt);
    } else {
        DiagPrintf("\nREG init value test OK\n\n");
    }

    /* Register Read/Write test */
    DiagPrintf( "=== Verify reg r/w =========\n\n");
    unsigned int val, max, MAX, mask, t_val, tmp, round;

    reset_pcm(pcm_index);

    for (reg=0; reg<sizeof(pcm_reg_tbl)/sizeof(REG_ENTRY_T);reg++) {
        /* Do NOT write RST pin */
        if (pcm_reg_tbl[reg].reg_addr == REG_PCM_CTL ) {
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x1000);
        }

        /* TX/RX enable should be ignored. 
         * Otherwise pcm logic will influence register value 
         */

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH0TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH1TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH2TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH3TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH0RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH1RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH2RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH3RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH4TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH5TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH6TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH7TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH4RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH5RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH6RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH7RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH8TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH9TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH10TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH11TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH8RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH9RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH10RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH11RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH12TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH13TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH14TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH15TXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH12RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH13RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH14RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);
        if (pcm_reg_tbl[reg].reg_addr ==  REG_PCM_CH15RXBSA)
            pcm_reg_tbl[reg].reg_rw_mask &= ~(0x03);

    }

    round = 0; 
    err_cnt = 0;

REG_RW:

    for (reg=0; reg<sizeof(pcm_reg_tbl)/sizeof(REG_ENTRY_T);reg++) {
        mask = pcm_reg_tbl[reg].reg_rw_mask;
        DiagPrintf( "Checking 0x%x RW mask 0x%x\n", 
            pcm_reg_tbl[reg].reg_addr,mask);

        MAX = 0;

        for (i=0 ; i<32 ; i++) {
            max = 0;

            for (j=0 ; j<32 ; j++) {
                if (round%2 == 0) 
                    val = (1<<j); 			/* LSB to MSB */
                else
                    val = (0x10000000>>j); 	/* MSB to LSB*/

                if (val > mask) break;

                tmp = val&mask;
                if (tmp != 0) {
                    /* do test (MAX | j) */
                    t_val = (MAX | val);

                    /* write val and read it back */
                    rw = HAL_PCMX_READ32(pcm_index, pcm_reg_tbl[reg].reg_addr);
                    rw = (rw&(~pcm_reg_tbl[reg].reg_rw_mask)) | (pcm_reg_tbl[reg].reg_rw_mask&t_val);
                    HAL_PCMX_WRITE32(pcm_index, pcm_reg_tbl[reg].reg_addr, rw);
                    rw = HAL_PCMX_READ32(pcm_index, pcm_reg_tbl[reg].reg_addr);

                    W = t_val & pcm_reg_tbl[reg].reg_rw_mask;
                    R = rw    & pcm_reg_tbl[reg].reg_rw_mask;
                    //DiagPrintf( "W=%x, R=%x\n",W,R);

                    if (W != R) {
                        DiagPrintf( "ERROR: %s r/w\n", pcm_reg_tbl[reg].name);
                        DiagPrintf("Original  w=0x%x, r=0x%x  rw_mask=0x%x\n",
                            t_val, rw, pcm_reg_tbl[reg].reg_rw_mask);
                        DiagPrintf( "Masked    W=0x%x, R=0x%x\n\n",W,R);

                        err_cnt++;
                    }
                    max=val;
                }
            } /* for (j) */
            /* found max value */
            MAX |= max;

            /* remove max bit */
            mask &= ~max;
            if (MAX == mask)
                break;

        } /* for (i) */
    } /* for (reg) */

    if (err_cnt!=0) {
        if (round%2 == 0) DiagPrintf("\nMSB to LSB round %d",round);
        else DiagPrintf("\nLSB to MSB round %d",round);
        DiagPrintf("\nREG Read/Write test fail. Error count : %d\n\n",err_cnt);
    } else {
        if (round%2 == 0) DiagPrintf("\nMSB to LSB round %d",round);
        else DiagPrintf("\nLSB to MSB round %d",round);
        DiagPrintf("\nREG Read/Write test OK\n\n");
    }

    if (round++ < TestTimes) {
        goto REG_RW;
    }

}

