#ifndef __MEMCTL_H__
#define __MEMCTL_H__
#ifndef __ASM__
#include <common.h>
//#include "rtk_soc_common.h"
#endif

//#define MEMCTL_DDR_CALI_DEBUG
//#define MEMCTL_DDR_CALI_DEBUG_L2


#define MEMCTL_DRAM_MIN_WIDTH	(8)
#define MEMCTL_DRAM_MAX_WIDTH	(16)
#define MEMCTL_DRAM_MIN_ROWS	(2*1024)
#define MEMCTL_DRAM_MAX_ROWS	(64*1024)
#define MEMCTL_DRAM_MIN_COLS	(256)
#define MEMCTL_DRAM_MAX_COLS	(4*1024)
#define MEMCTL_DRAM_MIN_BKS	(2)
#define MEMCTL_DRAM_MAX_BKS	(8)
#define MEMCTL_DRAM_MIN_CS	(1)
#define MEMCTL_DRAM_MAX_CS	(2)
//#define _memctl_debug_printf printf
#define _memctl_debug_printf(...)
/* Memory Controller Registers Definition */
#define MEMCTL_DTR_ADDR 	(0xb8001008)
#define MEMCTL_DTR1_ADDR 	(0xb800100c)
#define MEMCTL_DTR2_ADDR 	(0xb8001010)
#define MCR			(0xB8001000)
#define DCR			(0xB8001004)
#define DTR			(0xB8001008)
#define DTR0			(0xB8001008)
#define DTR1			(0xB800100c)
#define DTR2			(0xB8001010)
#define DMCR			(0xB800101C)
#define SOCPNR			(0xB80010FC)

#define DACCR			(0xB8001500)
#define DACSPCR			(DACCR+0x4)
#define DACDQF			(DACCR+0x50)
#define DACDQR			(DACCR+0x10)
#define DCDQMR			(DACCR+0x90)

#define DDCR			(0xB8001050)
#define DCDR			(0xB8001060)

#define DACDQR_DQR_PHASE_SHIFT_90_FD_S (24)
#define DACDQR_DQR_PHASE_SHIFT_90_MASK (0x1f << DACDQR_DQR_PHASE_SHIFT_90_FD_S)
#define DACDQR_DQR_MAX_TAP_FD_S (16)
#define DACDQR_DQR_MAX_TAP_MASK (0x1f << DACDQR_DQR_MAX_TAP_FD_S)
#define DACDQR_DQR_MIN_TAP_FD_S (0)
#define DACDQR_DQR_MIN_TAP_MASK (0x1f << DACDQR_DQR_MIN_TAP_FD_S)

#define DCDQMR_DQM0_PHASE_SHIFT_90_FD_S (24)
#define DCDQMR_DQM0_PHASE_SHIFT_90_MASK (0x1f << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S)
#define DCDQMR_DQM1_PHASE_SHIFT_90_FD_S (16)
#define DCDQMR_DQM1_PHASE_SHIFT_90_MASK (0x1f << DCDQMR_DQM1_PHASE_SHIFT_90_FD_S)

/* Field start bit definition */
#define MCR_D_INIT_TRIG_FD_S	(14)
#define DTR0_CAS_FD_S		(28)
#define DTR0_WR_FD_S 		(24)
#define DTR0_CWL_FD_S 		(20)
#define DTR0_RTP_FD_S 		(16)
#define DTR0_WTR_FD_S 		(12)
#define DTR0_REFI_FD_S 		(8)
#define DTR0_REFI_UNIT_FD_S 	(4)
#define DTR1_RP_FD_S 		(24)
#define DTR1_RCD_FD_S 		(16)
#define DTR1_RRD_FD_S 		(8)
#define DTR1_FAWG_FD_S 		(0)
#define DTR2_RFC_FD_S 		(20)
#define DTR2_RAS_FD_S 		(12)

#define MCR_D_INIT_TRIG_MASK	(0x1 << MCR_D_INIT_TRIG_FD_S)
#define DTR0_REFI_MASK 		(0xF << DTR0_REFI_FD_S)
#define DTR0_REFI_UNIT_MASK 	(0xF << DTR0_REFI_UNIT_FD_S)
#define DTR0_CAS_MASK		(0xF << DTR0_CAS_FD_S)
#define DTR0_WR_MASK 		(0xF << DTR0_WR_FD_S)
#define DTR0_CWL_MASK 		(0xF << DTR0_CWL_FD_S)
#define DTR0_RTP_MASK 		(0xF << DTR0_RTP_FD_S)
#define DTR0_WTR_MASK 		(0xF << DTR0_WTR_FD_S)
#define DTR1_RP_MASK 		(0xFF << DTR1_RP_FD_S)
#define DTR1_RCD_MASK 		(0xFF << DTR1_RCD_FD_S)
#define DTR1_RRD_MASK 		(0xFF << DTR1_RRD_FD_S)
#define DTR1_FAWG_MASK 		(0xFF << DTR1_FAWG_FD_S)
#define DTR2_RFC_MASK 		(0xFFF << DTR2_RFC_FD_S)
#define DTR2_RAS_MASK 		(0x3F << DTR2_RAS_FD_S)


#define MEMCTL_DRAM_MIN_BUSWIDTH 	(8)
#define MEMCTL_DRAM_MAX_BUSWIDTH 	(16)
#define MEMCTL_DRAM_BUSWIDTH_UNIT 	(8)
#define MEMCTL_DRAM_MIN_CHIP_NUM 	(1)
#define MEMCTL_DRAM_MAX_CHIP_NUM 	(2)
#define MEMCTL_DRAM_DDR_MIN_BANK_NUM 	(2)
#define MEMCTL_DRAM_DDR_MAX_BANK_NUM 	(4)
#define MEMCTL_DRAM_DDR2_MIN_BANK_NUM 	(4)
#define MEMCTL_DRAM_DDR2_MAX_BANK_NUM 	(8)
#define MEMCTL_DRAM_DDR3_MIN_BANK_NUM 	(4)
#define MEMCTL_DRAM_DDR3_MAX_BANK_NUM 	(8)
#define MEMCTL_DRAM_MIN_ROW_NUM 	(2*1024)
#define MEMCTL_DRAM_MAX_ROW_NUM 	(16*1024)
#define MEMCTL_DRAM_MIN_COL_NUM 	(256)
#define MEMCTL_DRAM_MAX_COL_NUM 	(4*1024)
#define MEMCTL_DRAM_PARAM_OK	(0)
#define MEMCTL_DRAM_PARAM_ERR	(1)
#define DCR_BANKCNT_FD_S	(28)
#define DCR_DBUSWID_FD_S	(24)
#define DCR_ROWCNT_FD_S		(20)
#define DCR_COLCNT_FD_S		(16)
#define DCR_DCHIPSEL_FD_S	(15)
#define DCR_BANKCNT_MASK	(0xF << DCR_BANKCNT_FD_S)
#define DCR_DBUSWID_MASK	(0xF << DCR_DBUSWID_FD_S)
#define DCR_ROWCNT_MASK		(0xF << DCR_ROWCNT_FD_S)
#define DCR_COLCNT_MASK		(0xF << DCR_COLCNT_FD_S)
#define DCR_DCHIPSEL_MASK	(1 << DCR_DCHIPSEL_FD_S)


#define DDCR_DQS0_TAP_FD_S 	(25)
#define DDCR_DQS1_TAP_FD_S 	(20)
#define DDCR_CAL_MODE_FD_S 	(31)
#define DCDR_PHASE_SHIFT_FD_S 	(17)


/* The values of field in the registers related to the memory controller. */
#define DLL_MODE 		(0)
#define DIGITAL_MODE 		(1)
#define MCR_DRAMTYPE_MASK	(0xF0000000)
#define MCR_DRAMTYPE_DDR 	(0x00000000)
#define MCR_DRAMTYPE_DDR2 	(0x10000000)
#define MCR_DRAMTYPE_DDR3 	(0x20000000)

#define MCR_PREFETCH_ENABLE_INS (0x00800000)
#define MCR_PREFETCH_MODE_IOLD (0x00000000)
#define MCR_PREFETCH_MODE_INEW (0x08000000)
#define MCR_PREFETCH_ENABLE_DATA (0x00400000)
#define MCR_PREFETCH_MODE_DOLD (0x00000000)
#define MCR_PREFETCH_MODE_DNEW (0x04000000)
#define MCR_PREFETCH_INS_SIDE  (0x2)
#define MCR_PREFETCH_DATA_SIDE (0x1)
#define MCR_PREFETCH_OLD_MECH	(0)
#define MCR_PREFETCH_NEW_MECH	(1)

/* Field masks used for extracting the value in the corresponding fields. */
//#define MCR_DRAMTYPE_MASK 	(0x80000000)
#define MCR_PREFETCH_DIS_IMASK (0xFF7FFFFF)
#define MCR_PREFETCH_DIS_DMASK (0xEFBFFFFF)
#define MCR_PREFETCH_MODE_IMASK (0xFF7FFFFF)
#define MCR_PREFETCH_MODE_DMASK (0xFFBFFFFF)
#define DDCR_DQS0_TAP_MASK 	(31 << DDCR_DQS0_TAP_FD_S)
#define DDCR_DQS1_TAP_MASK 	(31 << DDCR_DQS1_TAP_FD_S)
#define DDCR_CAL_MODE_MASK 	(1 << DDCR_CAL_MODE_FD_S)
#define DDCR_CAL_MODE_DLL 	(0 << DDCR_CAL_MODE_FD_S)
#define DDCR_CAL_MODE_DIG 	(1 << DDCR_CAL_MODE_FD_S)
#define DCDR_PHASE_SHIFT_MASK 	(31 << DCDR_PHASE_SHIFT_FD_S)

/* Timing constraints definition used for DRAM diagnosis. */
#define DDR_CAS2_MAX_MHZ	(143)
#define DDR_CAS25_MAX_MHZ	(167)
#define DDR_CAS3_MAX_MHZ	(200)
#define DDR_STD_REF_MS 		(64)
#define DDR_STD_REFI_NS		CONFIG_DRAM_REFI_NS
#define DDR_STD_WR_NS 		(15)
#define DDR_STD_RP_NS 		(20)
#define DDR_STD_RCD_NS 		(20)
#define DDR_STD_RAS_NS 		(45)
#define DDR_STD_RFC_NS		(75)
#define DDR_STD_RRD_NS 		(15)
#define DDR_STD_RTP_NS 		(0)
#define DDR_STD_FAWG_NS		(0)
#define DDR_STD_WTR_NS 		(10)


#define DDR2_CAS2_MAX_MHZ	(143)
#define DDR2_CAS3_MAX_MHZ	(200)
#define DDR2_CAS4_MAX_MHZ	(266)
#define DDR2_CAS5_MAX_MHZ	(400)
#define DDR2_CAS6_MAX_MHZ	(400)
#define DDR2_STD_REF_MS		CONFIG_DRAM_REFI_MS
#define DDR2_STD_REFI_NS	CONFIG_DRAM_REFI_NS
#define DDR2_STD_RCD_NS 	CONFIG_DRAM_RCD_NS
#define DDR2_STD_RP_NS 		CONFIG_DRAM_RP_NS
#define DDR2_STD_RAS_NS 	CONFIG_DRAM_RAS_NS
#define DDR2_STD_RRD_NS 	CONFIG_DRAM_RRD_NS
#define DDR2_STD_FAWG_NS	CONFIG_DRAM_FAWG_NS
#define DDR2_STD_RTP_NS 	CONFIG_DRAM_RTP_NS
#define DDR2_STD_WTR_NS 	CONFIG_DRAM_WTR_NS
#define DDR2_STD_WR_NS 		CONFIG_DRAM_WR_NS
#define DDR2_STD_RFC_NS         CONFIG_DRAM_RFC_NS
#define DDR2_STD_RFC_32MB_NS	(75)
#define DDR2_STD_RFC_64MB_NS	(105)
#define DDR2_STD_RFC_128MB_NS	(128)
#define DDR2_STD_RFC_256MB_NS	(195)
#define DDR2_STD_RFC_512MB_NS	(328)


#define DDR3_CAS5_MAX_MHZ	(400)
#define DDR3_CAS6_MAX_MHZ	(400)
#define DDR3_CAS7_MAX_MHZ	(533)
#define DDR3_CAS8_MAX_MHZ	(533)
#define DDR3_CAS9_MAX_MHZ	(666)
#define DDR3_CAS10_MAX_MHZ	(666)
#define DDR3_CAS11_MAX_MHZ	(800)
#define DDR3_CWL5_MAX_MHZ	(400)
#define DDR3_CWL6_MAX_MHZ	(533)
#define DDR3_CWL7_MAX_MHZ	(666)
#define DDR3_CWL8_MAX_MHZ	(800)
#if 0
#define DDR3_STD_REF_MS		(64)
#define DDR3_STD_RCD_NS 	(15)
#define DDR3_STD_RP_NS 		(15)
#define DDR3_STD_RAS_NS 	(38)
#define DDR3_STD_WTR_CK 	(4)
#define DDR3_STD_WTR_NS 	(8)
#define DDR3_STD_WR_NS 		(15)
#define DDR3_STD_FAWG_NS	(50)
#define DDR3_STD_RTP_NS 	(8)
#define DDR3_STD_RTP_CK 	(4)
#define DDR3_STD_RRD_NS 	(10)
#define DDR3_STD_RRD_CK 	(4)
#define DDR3_STD_RFC_NS         CONFIG_DRAM_RFC_NS
#else
#define DDR3_STD_REF_MS		CONFIG_DRAM_REFI_MS
#define DDR3_STD_REFI_NS	CONFIG_DRAM_REFI_NS
#define DDR3_STD_RCD_NS 	CONFIG_DRAM_RCD_NS
#define DDR3_STD_RP_NS 		CONFIG_DRAM_RP_NS
#define DDR3_STD_RAS_NS 	CONFIG_DRAM_RAS_NS
#define DDR3_STD_WTR_CK 	CONFIG_DRAM_WTR_NS
#define DDR3_STD_WTR_NS 	CONFIG_DRAM_WTR_NS
#define DDR3_STD_WR_NS 		CONFIG_DRAM_WR_NS
#define DDR3_STD_FAWG_NS	CONFIG_DRAM_FAWG_NS
#define DDR3_STD_RTP_NS 	CONFIG_DRAM_RTP_NS
#define DDR3_STD_RTP_CK 	(4)
#define DDR3_STD_RRD_NS 	CONFIG_DRAM_RRD_NS
#define DDR3_STD_RRD_CK 	(4)
#define DDR3_STD_RFC_NS         CONFIG_DRAM_RFC_NS
#endif

#define DDR3_STD_RFC_64MB_NS	(110)
#define DDR3_STD_RFC_128MB_NS	(110)
#define DDR3_STD_RFC_256MB_NS	(160)
#define DDR3_STD_RFC_512MB_NS	(300)
#define DDR3_STD_RFC_1GB_NS	(350)


#define DMCR_MRS_BUSY		(0x80000000)
#define DMCR_MR_MODE_EN		(0x00100000)
#define DMCR_MRS_MODE_MR	(0x00000000)
#define DMCR_MRS_MODE_EMR1	(0x00010000)
#define DMCR_MRS_MODE_EMR2	(0x00020000)
#define DMCR_MRS_MODE_EMR3	(0x00030000)
#define DMCR_DIS_DRAM_REF_FD_S	(24)
#define DMCR_DIS_DRAM_REF_MASK	(1 << DMCR_DIS_DRAM_REF_FD_S)
#define DMCR_MR_MODE_EN_FD_S	(20)
#define DMCR_MR_MODE_EN_MASK	(1 << DMCR_MR_MODE_EN_FD_S)


/* DDR Mode register related definition */
#define DDR1_MR_BURST_2		(0x00000001)
#define DDR1_MR_BURST_4		(0x00000002)
#define DDR1_MR_BURST_8		(0x00000003)
#define DDR1_MR_BURST_SEQ	(0x00000000)
#define DDR1_MR_BURST_INTER	(0x00000008)
#define DDR1_MR_CAS_2 		(0x00000020)
#define DDR1_MR_CAS_3 		(0x00000030)
#define DDR1_MR_CAS_25 		(0x00000060)
#define DDR1_MR_OP_NOR		(0x00000000)
#define DDR1_MR_OP_RST_DLL	(0x00000100)
#define DDR1_MR_OP_TEST		(0x00000080)

#define DDR1_EMR1_DLL_EN	(0x00000000)
#define DDR1_EMR1_DLL_DIS	(0x00000001)
#define DDR1_EMR1_DRV_NOR	(0x00000000)
#define DDR1_EMR1_DRV_WEAK	(0x00000002)

/* DDR2 Mode register related definition */
#define DDR2_MR_BURST_4 	(0x00000002)
#define DDR2_MR_BURST_8 	(0x00000003)
#define DDR2_MR_BURST_SEQ 	(0x00000000)
#define DDR2_MR_BURST_INTER 	(0x00000008)
#define DDR2_MR_CAS_2 		(0x00000020)
#define DDR2_MR_CAS_3 		(0x00000030)
#define DDR2_MR_CAS_4 		(0x00000040)
#define DDR2_MR_CAS_5 		(0x00000050)
#define DDR2_MR_CAS_6 		(0x00000060)
#define DDR2_MR_TM_NOR 		(0x00000000)
#define DDR2_MR_TM_TEST		(0x00000080)
#define DDR2_MR_DLL_RESET_YES	(0x00000100)
#define DDR2_MR_DLL_RESET_NO	(0x00000000)
#define DDR2_MR_WR_2		(0x00000200)
#define DDR2_MR_WR_3		(0x00000400)
#define DDR2_MR_WR_4		(0x00000600)
#define DDR2_MR_WR_5		(0x00000800)
#define DDR2_MR_WR_6		(0x00000A00)
#define DDR2_MR_WR_7		(0x00000C00)
#define DDR2_MR_PD_FAST		(0x00000000)
#define DDR2_MR_PD_SLOW		(0x00001000)
#define DDR2_EMR1_DLL_EN	(0x00000000)
#define DDR2_EMR1_DLL_DIS	(0x00000001)
#define DDR2_EMR1_DIC_FULL	(0x00000000)
#define DDR2_EMR1_DIC_REDUCE	(0x00000002)
#define DDR2_EMR1_RTT_DIS	(0x00000000)
#define DDR2_EMR1_RTT_75	(0x00000004)
#define DDR2_EMR1_RTT_150	(0x00000040)
#define DDR2_EMR1_RTT_50	(0x00000044)
#define DDR2_EMR1_ADD_0		(0x00000000)
#define DDR2_EMR1_ADD_1		(0x00000008)
#define DDR2_EMR1_ADD_2		(0x00000010)
#define DDR2_EMR1_ADD_3		(0x00000018)
#define DDR2_EMR1_ADD_4		(0x00000020)
#define DDR2_EMR1_ADD_5		(0x00000028)
#define DDR2_EMR1_OCD_EX	(0x00000000)
#define DDR2_EMR1_OCD_D1	(0x00000080)
#define DDR2_EMR1_OCD_D0	(0x00000100)
#define DDR2_EMR1_OCD_AD	(0x00000200)
#define DDR2_EMR1_OCD_DEF	(0x00000380)
#define DDR2_EMR1_QOFF_EN	(0x00000000)
#define DDR2_EMR1_QOFF_DIS	(0x00001000)
#define DDR2_EMR1_NDQS_EN	(0x00000000)
#define DDR2_EMR1_NDQS_DIS	(0x00000400)
#define DDR2_EMR1_RDQS_EN	(0x00000800)
#define DDR2_EMR1_RDQS_DIS	(0x00000000)
#define DDR2_EMR2_HTREF_EN	(0x00000080)
#define DDR2_EMR2_HTREF_DIS	(0x00000000)
#define DDR2_EMR2_DCC_DIS	(0x00000000)
#define DDR2_EMR2_DCC_EN	(0x00000008)
#define DDR2_EMR2_PASELF_FULL	(0x00000000)


/* DDR3 Mode register related definition */
#define DDR3_MR_BURST_8 		(0x00000000)
#define DDR3_MR_BURST_BC4OR8 		(0x00000001)
#define DDR3_MR_BURST_BC4 		(0x00000002)
#define DDR3_MR_READ_BURST_NIBBLE 	(0x00000000)
#define DDR3_MR_READ_BURST_INTER 	(0x00000008)
#define DDR3_MR_CAS_5 			(0x00000010)
#define DDR3_MR_CAS_6	 		(0x00000020)
#define DDR3_MR_CAS_7	 		(0x00000030)
#define DDR3_MR_CAS_8	 		(0x00000040)
#define DDR3_MR_CAS_9	 		(0x00000050)
#define DDR3_MR_CAS_10	 		(0x00000060)
#define DDR3_MR_CAS_11	 		(0x00000070)
#define DDR3_MR_TM_NOR	 		(0x00000000)
#define DDR3_MR_TM_TEST			(0x00000080)
#define DDR3_MR_DLL_RESET_YES		(0x00000100)
#define DDR3_MR_DLL_RESET_NO		(0x00000000)
#define DDR3_MR_WR_5			(0x00000200)
#define DDR3_MR_WR_6			(0x00000400)
#define DDR3_MR_WR_7			(0x00000600)
#define DDR3_MR_WR_8			(0x00000800)
#define DDR3_MR_WR_9			(0x00000A00)
#define DDR3_MR_WR_10			(0x00000C00)
#define DDR3_MR_WR_12			(0x00000E00)
#define DDR3_MR_PD_FAST			(0x00001000)
#define DDR3_MR_PD_SLOW			(0x00000000)
#define DDR3_EMR1_DLL_EN		(0x00000000)
#define DDR3_EMR1_DLL_DIS		(0x00000001)
#define DDR3_EMR1_DIC_RZQ_DIV_6		(0x00000000)
#define DDR3_EMR1_DIC_RZQ_DIV_7		(0x00000002)
#define DDR3_EMR1_RTT_NOM_DIS		(0x00000000)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV4	(0x00000004)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV2	(0x00000040)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV6	(0x00000044)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV12	(0x00000200)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV8	(0x00000204)
#define DDR3_EMR1_ADD_0			(0x00000000)
#define DDR3_EMR1_ADD_CL_RD1		(0x00000008)
#define DDR3_EMR1_ADD_CL_RD2		(0x00000010)
#define DDR3_EMR1_WRITE_LEVEL_DIS	(0x00000000)
#define DDR3_EMR1_WRITE_LEVEL_EN	(0x00000080)
#define DDR3_EMR1_TDQS_DIS		(0x00000000)
#define DDR3_EMR1_TDQS_EN		(0x00000800)
#define DDR3_EMR1_QOFF_DIS		(0x00001000)
#define DDR3_EMR1_QOFF_EN		(0x00000000)
#define DDR3_EMR2_PASR_FULL		(0x00000000)
#define DDR3_EMR2_PASR_HALF_L		(0x00000001)
#define DDR3_EMR2_PASR_QUA		(0x00000002)
#define DDR3_EMR2_PASR_8TH_L		(0x00000003)
#define DDR3_EMR2_PASR_3_QUA		(0x00000004)
#define DDR3_EMR2_PASR_HALF_H		(0x00000005)
#define DDR3_EMR2_PASR_8TH_H		(0x00000007)
#define DDR3_EMR2_CWL_5			(0x00000000)
#define DDR3_EMR2_CWL_6			(0x00000008)
#define DDR3_EMR2_CWL_7			(0x00000010)
#define DDR3_EMR2_CWL_8			(0x00000018)
#define DDR3_EMR2_ASR_EN		(0x00000040)
#define DDR3_EMR2_ASR_DIS		(0x00000000)
#define DDR3_EMR2_SRT_NOR		(0x00000000)
#define DDR3_EMR2_SRT_EXT		(0x00000080)
#define DDR3_EMR2_RTT_WR_DIS		(0x00000000)
#define DDR3_EMR2_RTT_WR_RZQ_DIV_4	(0x00000200)
#define DDR3_EMR2_RTT_WR_RZQ_DIV_2	(0x00000400)
#define DDR3_EMR3_MPR_LOC_PRE_PAT	(0x00000000)
#define DDR3_EMR3_MPR_OP_NOR		(0x00000000)
#define DDR3_EMR3_MPR_OP_DATA		(0x00000004)



/* DDR Calibration */
#define MEMCTL_DDR_DQS_SE		(1)
#define MEMCTL_DDR_DQS_DIFF		(0)
#define MEMCTL_DDR_DLL_MODE		(0)
#define MEMCTL_DDR_DIG_DELAY_MODE	(1)

#define MEMCTL_DDRTYPE_DDRI	(1)
#define MEMCTL_DDRTYPE_DDRII	(2)
#define MEMCTL_DDRTYPE_DDRIII	(3)

#define MEMCTL_DACCR_AC_MODE_DLL (0<<31)
#define MEMCTL_DACCR_AC_MODE_DIG (1<<31)
#define MEMCTL_DACCR_AC_MODE_MASK (~(1<<31))
#define MEMCTL_DACCR_DQS_MODE_SE (0<<31)
#define MEMCTL_DACCR_DQS_MODE_DIF (1<<31)
#define MEMCTL_DACCR_DQS_MODE_MASK (~(1<<31))
#define MEMCTL_DACCR_DQS0_GRUP_TAP_SHIFT (16)
#define MEMCTL_DACCR_DQS1_GRUP_TAP_SHIFT (8)
#define MEMCTL_DACSPCR_PERIOD_EN_MASK	(~(1 << 31))

#define _MEMCTL_CALI_FAIL	      (0xffff)
#define _MEMCTL_CALI_PASS	      (0)
#define _MEMCTL_CALI_STATUS_LOOKING_A (1)
#define _MEMCTL_CALI_STATUS_LOOKING_B (2)
#define _MEMCTL_CALI_STATUS_LOOKING_C (3)
#define _MEMCTL_CALI_SATAUS_DONE      (4)
#define _MEMCTL_CALI_SATAUS_FAIL      (5)
#define _MEMCTL_CALI_SATAUS_OVERFLOW  (6)
#define _MEMCTL_CALI_SATAUS_OK	      (7)
#define _MEMCTL_CALI_PHASE_A_OVERFLOW (8)
#define _MEMCTL_CALI_PHASE_B_OVERFLOW (9)
#define _MEMCTL_CALI_PHASE_C_OVERFLOW (10)
#define _MEMCTL_CALI_PHASE_A_SATAUS_OK   (11)
#define _MEMCTL_CALI_STATUS_UNKNOWN   (12)
#define MEMCTL_CALI_FAIL	      (-1)
#define MEMCTL_CALI_PASS	      (0)

#define MIN_READ_DELAY_WINDOW  (1)
#define MIN_WRITE_DELAY_WINDOW (1)
#define MIN_WRITE_MASK_DELAY_WINDOW (3)


/* STATUS defintiion */
#define CALI_FAIL_DCDR_VALUE    (0xFFFE0000)
#define CALI_FAIL_DDCR_VALUE    (0xFFFFFC00)

/* Memory Unmapping */
#define MEMCTL_UNMAP_REG_SET_SIZE	(0x10)
#define MEMCTL_UNMAP_REG_SET_NUM	(4)
#define MEMCTL_UNMAP_REG_BASE_ADDR	(0xB8001300)

#define C0UMSAR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x0)
#define C0UMSSR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x4)

#define C1UMSAR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x40)
#define C1UMSSR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x44)

#define MEMCTL_UNMAP_SEG_ENABLE		(0x1)

#define MEMCTL_UNMAP_SIZE_256B		(0x1)
#define MEMCTL_UNMAP_SIZE_512B		(0x2)
#define MEMCTL_UNMAP_SIZE_1KB		(0x3)
#define MEMCTL_UNMAP_SIZE_2KB		(0x4)
#define MEMCTL_UNMAP_SIZE_4KB		(0x5)
#define MEMCTL_UNMAP_SIZE_8KB		(0x6)
#define MEMCTL_UNMAP_SIZE_16KB		(0x7)
#define MEMCTL_UNMAP_SIZE_32KB		(0x8)
#define MEMCTL_UNMAP_SIZE_64KB		(0x9)
#define MEMCTL_UNMAP_SIZE_128KB		(0xA)
#define MEMCTL_UNMAP_SIZE_256KB		(0xB)
#define MEMCTL_UNMAP_SIZE_512KB		(0xC)
#define MEMCTL_UNMAP_SIZE_1MB		(0xD)

#ifndef __ASSEMBLY__

/* Data structure used for storing tap statics of the DDR calibration experiences. */
typedef struct tap_info {
		unsigned int mode;
		unsigned int mhz;
		unsigned int tap_min;
		unsigned int tap_max;
	} tap_info_t;


/* C function */
int memctlc_init_dram(void);
unsigned int memctlc_dram_size(void);
//void memctlc_check_DRAM_config(void);
void memctlc_check_DTR(unsigned int dram_freq_mhz);
void memctlc_check_DQS_range(void);
void memctlc_check_90phase_range(void);
void memctlc_check_ZQ(void);
int  memctlc_ummap_en(unsigned int segNo, unsigned int cpu_addr, \
                    unsigned int size_in_byte);
int  memctlc_ummap_dis(unsigned int segNo);
void memctlc_show_ummapinfo(void);
void memctlc_clk_rev_check(void);
unsigned int memctlc_is_DDR(void);
unsigned int memctlc_is_DDR2(void);
unsigned int memctlc_is_DDR3(void);
unsigned int memctlc_dram_size_detect(void);

/* Assembly function */
unsigned int memctls_is_DDR(void);
unsigned int memctls_is_DDR2(void);
unsigned int memctls_is_DDR3(void);
unsigned int memctls_ddr_calibration(void);
unsigned int memctls_dram_auto_detect(void);
void memctls_ddr1_dll_reset(void);
void memctls_ddr2_dll_reset(void);
void memctls_enable_iprefeatch(void);
void memctls_enable_dprefeatch(void);
void memctlc_dqs_calibration_expansion(unsigned int dq, unsigned int buswidth);
void memctlc_dqm_calibration(void);
int memctlc_dqs_calibration(unsigned int dram_type, unsigned int ac_mode,\
                            unsigned int dqs_mode, unsigned int buswidth,\
                            unsigned int test_addr, unsigned int word_size);
unsigned int memctlc_DRAM_size_detection(void);
int memctlc_ddr_pin_detection(void);
int memctlc_hw_auto_calibration( unsigned int buswidth, unsigned int test_addr);
int memctl_unmapping(unsigned int segNo, unsigned int cpu_addr, \
		   unsigned int unmap_size_no);
int memctl_unmapping_disable(unsigned int segNo);
#endif


#endif //end of __MEMCTL_H__


