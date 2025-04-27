#ifndef _BSPCHIP_H_
#define _BSPCHIP_H_

#ifndef BIT
#define BIT(x) (1 << x)
#endif

#define BONDING_REG 						(0xBB0001E8)
#define STRAPING_REG						(0xBB0001EC)
#define BONDING_REG_DRAMTYPE_FREQ43_FD_S			(5)
#define BONDING_REG_DRAMTYPE_FREQ43_MASK			(3 << BONDING_REG_DRAMTYPE_FREQ43_FD_S)
#define STRAPING_REG_DRAMTYPE_FREQ210_FD_S			(22)
#define STRAPING_REG_DRAMTYPE_FREQ210_MASK			(7 << STRAPING_REG_DRAMTYPE_FREQ210_FD_S)


#define CONFIG_RTL8685_25M_XTAL

#define SYSREG_OCP0_DEFAULT_MHZ					(400)
#define SYSREG_OCP1_DEFAULT_MHZ					(500)
#define SYSREG_LX_DEFAULT_MHZ					(200)

/*SYS STATUS*/
#define SYSREG_SYSTEM_STATUS_REG				(0xB8000044)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_LX_MASK 			BIT(4)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_LX_LXPLL 			SYSREG_SYSTEM_STATUS_CF_CKSE_LX_MASK
#define SYSREG_SYSTEM_STATUS_SYS_CPU2_EN_MASK 			BIT(5)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK 			BIT(2)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP1_MASK 			BIT(3)

/*SYS PIN STATUS*/
#define SYSREG_PIN_STATUS_REG					(0xB8000100)
#define SYSREG_PIN_STATUS_CKSEL_25MHZ				(0)
#define SYSREG_PIN_STATUS_CKSEL_40MHZ				(1)
#define SYSREG_PIN_STATUS_CKSEL_MASK				BIT(5)

#define SYSREG_PINMUX0_REG					(0xB8000108)

/*SYS CLK CR*/
#define SYSREG_SYSCLK_CONTROL_REG				(0xB8000200)
#define OCP0_PLL_DIV_S						(16)
#define OCP1_PLL_DIV_S						(8)
#define SDPLL_DIV_S 						(0)

#define SYSREG_PLL_CTL0_31_REG             			(0xB8000208)
#define SYSREG_PLL_CTL32_63_REG             			(0xB800020C)

/*SYS LX PLL CR*/
#define SYSREG_PLL_CTL_REG					(0xB8000214)
#define SYSREG_LX_PLL_CTRL_LXPLL_FD_S				(0)
#define SYSREG_LX_PLL_CTRL_LXPLL_FD_MASK			(0xf << SYSREG_LX_PLL_CTRL_LXPLL_FD_S)
#define SYSREG_LX_PLL_CTRL_SPIFPLL_FD_S				(8)
#define SYSREG_LX_PLL_CTRL_SPIFPLL_FD_MASK			(0xf << SYSREG_LX_PLL_CTRL_SPIFPLL_FD_S)

#define SYSREG_DRAM_CLK_EN_MASK					BIT(0)
#define SYSREG_PIN_STATUS_CLSEL_MASK				BIT(5)
#define SYSREG_LX_DEFAULT_MHZ					(200)

#define SYSREG_ANA1_CTL_REG        				(0xB8000218)

/*SYS MCKG PHS SEL*/
#define SYSREG_MCKG_PHS_SEL_REG					(0xB8000220)
#define SYSREG_MCKG_PHS_SEL_PHS_FD_S				(0)
#define SYSREG_MCKG_PHS_SEL_PHS_MASK				(0x7 << SYSREG_MCKG_PHS_SEL_PHS_FD_S)

/*SYS MCKG FREQ DIV*/
#define SYSREG_MCKG_FREQ_DIV_REG				(0xB8000224)
#define SYSREG_MCKG_FREQ_DIV_FD_S    				(0)
#define SYSREG_MCKG_FREQ_DIV_MASK    				(0x3 << SYSREG_MCKG_FREQ_DIV_FD_S)

#define SYSREG_LX_PLL_SEL_REG					(0xB8000228)
#define SYSREG_DRAM_CLK_EN_REG					(0xB800022c)
#define SYSREG_IP_EN_CTL_REG 					(0xB8000600)

/*DDR CKODL*/
#define SYSREG_DDRCKODL_REG					(0xB800021c)
#define SYSREG_DDRCKODL_DDRCLM90_TAP_FD_S 			(16)
#define SYSREG_DDRCKODL_DDRCLM90_TAP_MASK 			(0x1f << SYSREG_DDRCKODL_DDRCLM90_TAP_FD_S)
#define SYSREG_DDRCKODL_DDRCLM_TAP_FD_S 			(8)
#define SYSREG_DDRCKODL_DDRCLM_TAP_MASK 			(0x1f << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)
#define SYSREG_DDRCKODL_DDRCK_PHS_FD_S 				(0)
#define SYSREG_DDRCKODL_DDRCK_PHS_MASK 				(0x1f << SYSREG_DDRCKODL_DDRCK_PHS_FD_S)


/*CMU regs*/
#define SYSREG_CMU_OC0_BASE 					(0xB8000380)
#define SYSREG_CMU_OC1_BASE 					(0xB8000390)
#define SYSREG_CMUGCR_OC0_REG					(SYSREG_CMU_OC0_BASE + 0x0)
#define SYSREG_CMUGCR_OC1_REG					(SYSREG_CMU_OC1_BASE + 0x0)
#define SYSREG_CMUSDCR_OC0_REG					(SYSREG_CMU_OC0_BASE + 0x4)
#define SYSREG_CMUSDCR_OC1_REG					(SYSREG_CMU_OC1_BASE + 0x4)
#define SYSREG_CMUCR_OC0_REG					(SYSREG_CMU_OC0_BASE + 0x8)
#define SYSREG_CMUCR_OC1_REG					(SYSREG_CMU_OC1_BASE + 0x8)
#define SYSREG_CMUSC_OC0_REG					(SYSREG_CMU_OC0_BASE + 0xc)
#define SYSREG_CMUSC_OC1_REG					(SYSREG_CMU_OC1_BASE + 0xc)

/*CMU GCR bits*/
#define CMU_BUSY						BIT(31)
#define MODE_MASK_S						(0)
#define MODE_MASK						(3 << SYSREG_CMUGCR_MODE_MASK_S)			
#define MODE_DISABLED_SEL					(0)
#define MODE_ENABLED_SEL					(1)
#define MODE_DYNAMIC_SEL 					(2)
#define FREQ_DIV_MASK_S					(4)
#define FREQ_DIV_MASK						(7 << SYSREG_CMUGCR_FREQ_DIV_MASK_S)
/*CMU CR bits*/				
#define AUTO_BZ							BIT(31)
#define SE_SPIF_WK						BIT(19)
#define SE_SPIF_SLP						BIT(18)
#define SE_SPIF							BIT(17)
#define SPIF_HS							BIT(16)
#define SE_DRAM_WK						BIT(11)
#define SE_DRAM_SLP						BIT(10)
#define SE_DRAM							BIT(9)
#define DRAM_HS							BIT(8)
#define DLY_MUL_MASK_S					(0)
#define DLY_MUL_MASK						(0xf << SYSREG_CMUCR_DLY_MUL_MASK)

/*LX bus slow bit CR*/
#define SYSREG_LBSBCR						(0xB80003A0)
#define LX3_FRQ_SLOWER						BIT(3)
#define LX2_FRQ_SLOWER						BIT(2)
#define LX1_FRQ_SLOWER						BIT(1)
#define LX0_FRQ_SLOWER						BIT(0)
				
				
#define SYSREG_WDCNTRR						(0xB8003260)
#define SYSREG_WDT_KICK						(1<<31)
#define SYSREG_WDTCTRLR						(0xB8003268)
#define SYSREG_WDT_E						(1<<31)
#define SYSREG_WDT_CLK_SC_MASK					(3<<29)
#define SYSREG_PH1_TO_S						(22)
#define SYSREG_PH1_TO_MASK					(0x1f << SYSREG_PH1_TO_S )
#define SYSREG_PH2_TO_S						(15)
#define SYSREG_PH2_TO_MASK					(0x1f << SYSREG_PH2_TO_S )

#define SYSREG_SFCR						(0xb8001200)
#define SYSREG_SFCR_SPI_CLK_DIV_S				(29)		
#define SYSREG_SFCR_SPI_CLK_DIV_MASK				(0x7<<SYSREG_SFCR_SPI_CLK_DIV_S)	
			
#define SYSREG_ANA_DLL_CTRL0					(0xb8000608)
#define SYSREG_ANA_DLL_CTRL1					(0xb800060c)
#define SYSREG_SWR_CTRL1					(0xb800064c)
#define SYSREG_SWR_CTRL2					(0xb8000650)
#define SYSREG_SWR_ANAREG_RDY					(1 << 31)

#define SYSREG_SFCR 					(0xb8001200)
#define SYSREG_SFCR_SPI_CLK_DIV_S		(29)		
#define SYSREG_SFCR_SPI_CLK_DIV_MASK	(0x7<<SYSREG_SFCR_SPI_CLK_DIV_S)	
			
#define SYSREG_ANA_DLL_CTRL0			(0xb8000608)
#define SYSREG_ANA_DLL_CTRL1			(0xb800060c)
#define SYSREG_SWR_CTRL1				(0xb800064c)
#define SYSREG_SWR_CTRL2				(0xb8000650)

//MEMCTRL
#define DDR_DELAY_CTRL_REG0					(0xb80015b0)
#define DDR_DELAY_CTRL_REG1					(0xb80015b4)
#define DDR_DELAY_CTRL_REG2					(0xb80015b8)
#define DDR_DELAY_CTRL_REG3					(0xb80015bc)
			
#define MEMPLL31_0						(0xb8000234)
#define MEMPLL63_32 						(MEMPLL31_0 + 0x4)
#define MEMPLL95_64 						(MEMPLL31_0 + 0x8)
#define MEMPLL127_96						(MEMPLL31_0 + 0xc)
#define MEMPLL159_128						(MEMPLL31_0 + 0x10)
#define MEMPLL191_160						(MEMPLL31_0 + 0x14)
#define MEMPLL223_192						(MEMPLL31_0 + 0x18)
			
#define DDR_DELAY_CTRL_REG0 					(0xb80015b0)
#define DDR_DELAY_CTRL_REG1 					(0xb80015b4)
#define DDR_DELAY_CTRL_REG2 					(0xb80015b8)
#define DDR_DELAY_CTRL_REG3 					(0xb80015bc)

#define DWDMOR							(0xB80015C0)
#define DWDQSOR 						(0xB80015C4)
#define DWDQOR0 						(0xB80015D0)
#define DWDQOR1 						(0xB80015D4)
#define DWDQOR2 						(0xB80015D8)
#define DWDQOR3 						(0xB80015DC)

/* * Register access macro */
#ifndef REG32
#define REG32(reg)			(*(volatile unsigned int   *)((unsigned int)reg))
#endif
#ifndef REG16
#define REG16(reg)			(*(volatile unsigned short *)((unsigned int)reg))
#endif
#ifndef REG08
#define REG08(reg)			(*(volatile unsigned char  *)((unsigned int)reg))
#endif
#ifndef REG8
#define REG8(reg)   			(*(volatile unsigned char  *)((unsigned int)reg))
#endif
#ifndef WRITE_MEM32
#define WRITE_MEM32(addr, val)   	(*(volatile unsigned int *)   (addr)) = (val)
#endif
#ifndef READ_MEM32
#define READ_MEM32(addr)         	(*(volatile unsigned int *)   (addr))
#endif
#ifndef WRITE_MEM16
#define WRITE_MEM16(addr, val)   	(*(volatile unsigned short *) (addr)) = (val)
#endif
#ifndef READ_MEM16
#define READ_MEM16(addr)         	(*(volatile unsigned short *) (addr))
#endif
#ifndef WRITE_MEM8
#define WRITE_MEM8(addr, val)    	(*(volatile unsigned char *)  (addr)) = (val)
#endif
#ifndef READ_MEM8
#define READ_MEM8(addr)          	(*(volatile unsigned char *)  (addr))
#endif

#endif //_BSPCHIP_H_
