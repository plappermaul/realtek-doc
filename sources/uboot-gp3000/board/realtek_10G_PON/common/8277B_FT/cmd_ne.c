#include <common.h>
#include <command.h>
#include <asm/types.h>
#include <asm/io.h>
#include <config.h>
#include "ft2.h"

#define CA_BIG_ENDIAN

typedef enum
{
	NI_MAC_SPEED_100F = 1,
	NI_MAC_SPEED_10F = 2,
	NI_MAC_SPEED_MAX
} ni_mac_speed_t;

/* Ethernet MIB operation code enumeration */
typedef enum
{
	__NI_MIB_OP_CODE_READ_ONLY      = 0, /* ONLY read MIB counter, not clear */
	__NI_MIB_OP_CODE_READ_CLEAR_MSB = 1, /* read MIB counter and clear MSB */
	__NI_MIB_OP_CODE_READ_CLEAR_ALL = 2  /* read MIB counter and clear all */

} __ni_mib_op_code_t;

typedef struct{
	ca_uint32_t drop_source_cnt_00;
	ca_uint32_t ipv4_cs_chk_drp_01;
	ca_uint32_t dpid_blackhole_drp_02;
	ca_uint32_t ilpb_stp_chk_drp_03;
	ca_uint32_t vlan_stp_chk_drp_04;
	ca_uint32_t ilpb_vlan_type_drp_05;
	ca_uint32_t igr_cls_pmt_stp_drp_06;
	ca_uint32_t vid4095_drp_07;
	ca_uint32_t unknown_vlan_drp_08;
	ca_uint32_t ve_deny_mc_flag_drp_09;
	ca_uint32_t l2e_da_deny_drp_10;
	ca_uint32_t non_std_sa_drp_11;
	ca_uint32_t sw_learn_err_drp_12;
	ca_uint32_t l2e_sa_deny_drp_13;
	ca_uint32_t sa_mis_or_sa_limit_drp_14;
	ca_uint32_t uuc_umc_bc_drp_15;
	ca_uint32_t spid_dpid_not_in_rx_vlan_mem_drp_16;
	ca_uint32_t dpid_not_in_port_mem_drp_17;
	ca_uint32_t spid_dpid_not_in_tx_vlan_mem_drp_18;
	ca_uint32_t tx_vlan_stp_drp_19;
	ca_uint32_t egr_cls_deny_drp_20;
	ca_uint32_t loopback_drp_21;
	ca_uint32_t elpb_stp_drp_22;
	ca_uint32_t drop_source_cnt_23;
	ca_uint32_t drop_source_cnt_24;
	ca_uint32_t mcfib_vlan_drp_25;
	ca_uint32_t drop_source_cnt_26;
	ca_uint32_t mcfib_dpid_equal_spid_drp_27;
	ca_uint32_t drop_source_cnt_28;
	ca_uint32_t drop_source_cnt_29;
	ca_uint32_t drop_source_cnt_30;
	ca_uint32_t drop_source_cnt_31;
}l2fe_drop_source_t;


/* Scan/Flush matching configuration table */
typedef union {
	struct {
#ifdef CA_BIG_ENDIAN
		ca_uint32_t rsvd                                    : 20;
		ca_uint32_t sa_permit                               : 1;
		ca_uint32_t da_permit                               : 1;
		ca_uint32_t static_flag                             : 1;
		ca_uint32_t in_refresh_flag                         : 1;
		ca_uint32_t in_st_mv_flag                           : 1;
		ca_uint32_t tmp_flag                                : 1;
		ca_uint32_t key_dot1p                               : 1;
		ca_uint32_t key_vid                                 : 1;
		ca_uint32_t lrned_vid                               : 1;
		ca_uint32_t aging_status                            : 1;
		ca_uint32_t mcgid                                   : 1;
		ca_uint32_t lpid                                    : 1;
#else
		ca_uint32_t lpid                                    : 1;
		ca_uint32_t mcgid                                   : 1;
		ca_uint32_t aging_status                            : 1;
		ca_uint32_t lrned_vid                               : 1;
		ca_uint32_t key_vid                                 : 1;
		ca_uint32_t key_dot1p                               : 1;
		ca_uint32_t tmp_flag                                : 1;
		ca_uint32_t in_st_mv_flag                           : 1;
		ca_uint32_t in_refresh_flag                         : 1;
		ca_uint32_t static_flag                             : 1;
		ca_uint32_t da_permit                               : 1;
		ca_uint32_t sa_permit                               : 1;
		ca_uint32_t mc_flag                                 : 1;
		ca_uint32_t key_sc_ind                              : 1;
		ca_uint32_t invalid_entry                           : 1;
		ca_uint32_t rsvd                                    : 17;
#endif
	} bf;
	ca_uint32_t     wrd ;
} l2fe_fdb_cpu_cmd_word4_msk_t;
typedef volatile union {
	struct {
#ifdef CA_BIG_ENDIAN
		ca_uint32_t rsvd                  : 2;
		ca_uint32_t sa_permit             : 1;
		ca_uint32_t da_permit             : 1;
		ca_uint32_t static_flag           : 1;
		ca_uint32_t in_refresh_flag       : 1;
		ca_uint32_t in_st_mv_flag         : 1;
		ca_uint32_t tmp_flag              : 1;
		ca_uint32_t key_vid_high          : 12;
		ca_uint32_t lrned_vid_high        : 12;
#else /* CA_LITTLE_ENDIAN */
		ca_uint32_t lrned_vid_high        : 12;
		ca_uint32_t key_vid_high          : 12;
		ca_uint32_t tmp_flag              : 1;
		ca_uint32_t in_st_mv_flag         : 1;
		ca_uint32_t in_refresh_flag       : 1;
		ca_uint32_t static_flag           : 1;
		ca_uint32_t da_permit             : 1;
		ca_uint32_t sa_permit             : 1;
		ca_uint32_t mc_flag               : 1;
		ca_uint32_t key_sc_ind            : 1;
#endif
	} bf ;
	ca_uint32_t     wrd ;
} l2fe_fdb_cpu_cmd_word3_t;

typedef volatile union {
	struct {
#ifdef CA_BIG_ENDIAN
		ca_uint32_t rsrvd1               :  8 ;
		ca_uint32_t key_vid_low          : 12 ;
		ca_uint32_t lrned_vid_low        : 12 ;
#else /* CA_LITTLE_ENDIAN */
		ca_uint32_t lrned_vid_low        : 12 ;
		ca_uint32_t key_vid_low          : 12 ;
		ca_uint32_t rsrvd1               :  8 ;
#endif
	} bf ;
	ca_uint32_t     wrd ;
} l2fe_fdb_cpu_cmd_word2_t;

typedef volatile union {
	struct {
#ifdef CA_BIG_ENDIAN
		ca_uint32_t rsrvd1                : 5;
		ca_uint32_t key_dot1p_high        : 3;
		ca_uint32_t aging_status_high     : 8;
		ca_uint32_t mcgid_high            : 10;
		ca_uint32_t lpid_high             : 6;
#else /* CA_LITTLE_ENDIAN */
		ca_uint32_t lpid_high             : 6;
		ca_uint32_t mcgid_high            : 10;
		ca_uint32_t aging_status_high     : 8;
		ca_uint32_t key_dot1p_high        : 3;
		ca_uint32_t rsrvd1                : 5;
#endif
	} bf ;
	ca_uint32_t     wrd ;
} l2fe_fdb_cpu_cmd_word1_t;

typedef volatile union {
	struct {
#ifdef CA_BIG_ENDIAN
		ca_uint32_t rsrvd1                : 5;
		ca_uint32_t key_dot1p_low         : 3;
		ca_uint32_t aging_status_low      : 8;
		ca_uint32_t mcgid_low             : 10;
		ca_uint32_t lpid_low              : 6;
#else /* CA_LITTLE_ENDIAN */
		ca_uint32_t lpid_low              : 6;
		ca_uint32_t mcgid_low             : 10;
		ca_uint32_t aging_status_low      : 8;
		ca_uint32_t key_dot1p_low         : 3;
		ca_uint32_t rsrvd1                : 5;
#endif
	} bf ;
	ca_uint32_t     wrd ;
} l2fe_fdb_cpu_cmd_word0_t;


/* when mc_flag is 0, use this structure */
typedef volatile union {
	struct {
#ifdef CA_BIG_ENDIAN
		ca_uint32_t lrned_vid            :  9 ; /* bits 31:23 */
		ca_uint32_t lrned_sc_ind         :  1 ; /* bits 22:22 */
		ca_uint32_t sa_permit            :  1 ; /* bits 21:21 */
		ca_uint32_t da_permit            :  1 ; /* bits 20:20 */
		ca_uint32_t is_static            :  1 ; /* bits 19:19 */
		ca_uint32_t aging_status         :  8 ; /* bits 18:11 */
		ca_uint32_t mc_flag              :  1 ; /* bits 10:10 */
		ca_uint32_t entry_valid          :  1 ;
		ca_uint32_t tmp_flag             :  1 ;
		ca_uint32_t in_st_move_flag      :  1 ;
		ca_uint32_t in_refresh_flag      :  1 ;
		ca_uint32_t lpid                 :  6 ;
#else /* CA_LITTLE_ENDIAN */
		ca_uint32_t lpid                 :  6 ;
		ca_uint32_t in_refresh_flag      :  1 ;
		ca_uint32_t in_st_move_flag      :  1 ;
		ca_uint32_t tmp_flag             :  1 ;
		ca_uint32_t entry_valid          :  1 ;
		ca_uint32_t mc_flag              :  1 ; /* bits 10:10 */
		ca_uint32_t aging_status         :  8 ; /* bits 18:11 */
		ca_uint32_t is_static            :  1 ; /* bits 19:19 */
		ca_uint32_t da_permit            :  1 ; /* bits 20:20 */
		ca_uint32_t sa_permit            :  1 ; /* bits 21:21 */
		ca_uint32_t lrned_sc_ind         :  1 ; /* bits 22:22 */
		ca_uint32_t lrned_vid            :  9 ; /* bits 31:23 */
#endif
	} bf ;
	ca_uint32_t     wrd ;
} l2fe_fdb_data0_t;

#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
#define CORTINA_CA_REG_READ(off)		CA_REG_READ(KSEG1_ATU_XLAT(off))
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, KSEG1_ATU_XLAT(off))
#else
#define CORTINA_CA_REG_READ(off)                CA_REG_READ(off)
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, off)
#endif

/*
 * NI ASIC table access MACROs
 */
#define  __NI_TABLE_ACCESS_TIMEOUT 5000
#define  __NI_TABLE_ACTION_WAIT(tbl_name,acc_reg) {\
	ca_uint32_t  timeout = 0;\
	do {\
		(acc_reg).wrd = CORTINA_CA_REG_READ(tbl_name);\
		if (timeout++ > __NI_TABLE_ACCESS_TIMEOUT)\
			return CA_E_TIMEOUT;\
	}while(0 != (acc_reg).bf.access);\
}while(0)

#define  __NI_MIB_TABLE_ACCESS(rw,tbl_name,port_id,mib_id,act,acc_reg) {\
	(acc_reg).wrd = 0;\
	(acc_reg).bf.access = 1;\
	(acc_reg).bf.rbw = (rw);\
	(acc_reg).bf.op_code = (act);\
	(acc_reg).bf.port_id = (port_id);\
	(acc_reg).bf.counter_id = (mib_id);\
	CORTINA_CA_REG_WRITE((acc_reg).wrd, tbl_name);\
	__NI_TABLE_ACTION_WAIT(tbl_name,acc_reg);\
}while(0)

#define FIND_INDIRCT_ADDRESS(rbw, index)    (((ca_uint32_t)1<<31)|((ca_uint32_t)(rbw)<<30)|(ca_uint32_t)(index))
#define CHECK_INDIRCT_OPERATE_STATE(dev,addr)    do{\
                                                    ca_uint32_t i=0;\
                                                    ca_uint32_t val32;\
                                                    for(i = 0; i<10000; i++){\
                                                       val32 = CORTINA_CA_REG_READ((ca_uint32_t)addr);\
                                                       if((val32 & ((ca_uint32)1 <<31)) == 0){\
                                                        break;\
                                                        }\
                                                    }\
                                                    if (i>= 10000) \
                                                        printf("%s(): access %s timeout.\n", __func__, #addr);\
                                                }while(0)


#define DO_INDIRCT_OP(dev,rbw, indx, raddr) do { CORTINA_CA_REG_WRITE(FIND_INDIRCT_ADDRESS(rbw, indx), raddr); \
                                  CHECK_INDIRCT_OPERATE_STATE(dev,raddr); } while(0)

#define WRITE_INDIRCT_REG(dev,index,addr) DO_INDIRCT_OP(dev,1,index,addr)
#define READ_INDIRCT_REG(dev,index,addr) DO_INDIRCT_OP(dev,0,index,addr)

extern uint8_t gmii_ports[];
extern uint8_t rgmii_ports[];

/******************************************
*                   NI commands
*******************************************/
ca_status_t ni_eth_port_rx_mib_get(
	ca_uint32_t port_id,
	ca_boolean_t read_clear,
	ca_uint32_t stats[__NI_MIB_RX_NUM]
)
{
	NI_HV_GLB_RXMIB_ACCESS_t  mib_access;
	NI_HV_GLB_RXMIB_DATA1_t   mib_data1;
	NI_HV_GLB_RXMIB_DATA0_t   mib_data0;
	__ni_rx_mib_id_t    mib_id;

	memset(stats, 0, __NI_MIB_RX_NUM*sizeof(ca_uint32_t));

	for (mib_id = __NI_MIB_RX_UC_PKT_CNT; mib_id < __NI_MIB_RX_BYTE_COUNT_LO; mib_id++)
	{
		__NI_MIB_TABLE_ACCESS(0, NI_HV_GLB_RXMIB_ACCESS,
			port_id, mib_id,
			read_clear ? __NI_MIB_OP_CODE_READ_CLEAR_ALL : __NI_MIB_OP_CODE_READ_ONLY,
			mib_access);

		/* data0 is cnt_val_lo, Valid for all counters;
		 * data1 is cnt_val_hi, Valid for Rx Byte count only.
		 */
		mib_data0.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMIB_DATA0);
		stats[mib_id] = mib_data0.wrd;
	}

	/* Read Rx byte counters ,
	 * note:  Access to either Lo or Hi byte counter id will update both values
	 */
	__NI_MIB_TABLE_ACCESS(0, NI_HV_GLB_RXMIB_ACCESS,
		port_id, __NI_MIB_RX_BYTE_COUNT_LO,
		read_clear ? __NI_MIB_OP_CODE_READ_CLEAR_ALL : __NI_MIB_OP_CODE_READ_ONLY,
		mib_access);

	mib_data1.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMIB_DATA1);
	mib_data0.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMIB_DATA0);

	stats[__NI_MIB_RX_BYTE_COUNT_LO] = mib_data0.wrd;
	stats[__NI_MIB_RX_BYTE_COUNT_HI] = mib_data1.wrd;

	return CA_E_OK;
}

ca_status_t ni_eth_port_tx_mib_get(
	ca_uint32_t port_id,
	ca_boolean_t read_clear,
	ca_uint32_t stats[__NI_MIB_TX_NUM]
)
{
	NI_HV_GLB_TXMIB_ACCESS_t  mib_access;
	NI_HV_GLB_TXMIB_DATA1_t   mib_data1;
	NI_HV_GLB_TXMIB_DATA0_t   mib_data0;
	__ni_tx_mib_id_t             mib_id;


	memset(stats, 0, __NI_MIB_TX_NUM*sizeof(ca_uint32_t));

	for (mib_id = __NI_MIB_TX_UC_PKT_CNT; mib_id < __NI_MIB_TX_BYTE_CNT_LO; mib_id++)
	{
		/* Skip ASIC reserved MIB index */
		if (mib_id == __NI_MIB_TX_RSVD_12 || mib_id == __NI_MIB_TX_RSVD_23) {
			stats[mib_id] = 0;
			continue;
		}

		__NI_MIB_TABLE_ACCESS(0, NI_HV_GLB_TXMIB_ACCESS,
			port_id, mib_id,
			read_clear ? __NI_MIB_OP_CODE_READ_CLEAR_ALL : __NI_MIB_OP_CODE_READ_ONLY,
			mib_access);

	   /* data0 is cnt_val_lo, Valid for all counters;
		 * data1 is cnt_val_hi, Valid for Tx Byte count only.
		 */
		mib_data0.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_TXMIB_DATA0);

		stats[mib_id] = mib_data0.wrd;
	}

	/* Read Tx byte counters ,
	 * note:  Access to either Lo or Hi byte counter id will update both values
	 */
	__NI_MIB_TABLE_ACCESS(0, NI_HV_GLB_TXMIB_ACCESS,
		port_id, __NI_MIB_TX_BYTE_CNT_LO,
		read_clear ? __NI_MIB_OP_CODE_READ_CLEAR_ALL : __NI_MIB_OP_CODE_READ_ONLY,
		mib_access);

	mib_data1.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_TXMIB_DATA1);
	mib_data0.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_TXMIB_DATA0);

	stats[__NI_MIB_TX_BYTE_CNT_LO] = mib_data0.wrd;
	stats[__NI_MIB_TX_BYTE_CNT_HI] = mib_data1.wrd;

	return CA_E_OK;
}

#if 0
/***************************************************/
int do_ni_init(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#if !defined(CONFIG_TARGET_VENUS)
#define L2FE_PLC_ELPB_DATA0_t	L2FE_PLC_ELPB_DATA_t
#define L2FE_PLC_ELPB_DATA0	L2FE_PLC_ELPB_DATA
#endif
#define L2_TM_CB_PORT_NUM    16
#define MAX_PORT_ID    16
#define L2_TM_CB_PRI_NUM    8
#define L2_LPORT_MC	0x1b
#define L2_PPORT_MC	0x0b
#define L2_LPORT_BLACKHOLE		0x1f
#define L2_PPORT_BLACKHOLE	0x0f

	ca_uint16_t i, port, pri, profile_id, index;
	GLOBAL_BLOCK_RESET_t reset;
	NI_HV_GLB_INIT_DONE_t init;
	L2FE_PP_NI_INTF_STS_t ready;
	NI_HV_GLB_NITX_MISC_CFG_t ni_tx_misc;
	NI_HV_GLB_STATIC_CFG_t cfg;
	NI_HV_PT_TXMAC_CFG_t tx_mac_cfg;
	NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG0_t tx_mac_pause_0;
	NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG1_t tx_mac_pause_1;
	NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG2_t tx_mac_pause_2;
	NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG3_t tx_mac_pause_3;
	NI_HV_PT_EEE_LPI_CFG0_t lpi_cfg;
	NI_HV_PT_PORT_STATIC_CFG_t port_cfg;
	NI_HV_PT_PORT_GLB_CFG_t port_glb;
	L2TM_L2TM_QM_EQ_CFG_t eq_cfg;
	L2TM_L2TM_QM_PORT_PRVT_PROFILE0_t port_prvt_prof;
	L2TM_L2TM_QM_GLOB_BUF_CFG_t buf_cfg;
	L2TM_L2TM_ES_CTRL_t es_ctrl;
	L2FE_PLC_DPID_FWD_CTRL_t fwd_ctrl;
	L2FE_PLC_L2_LEARNING_CTRL_t learn_ctrl;
	L2FE_PP_ILPB_DATA1_t ilpb_data;
	L2FE_PP_ILPB_DATA2_t ilpb_data_2;
	L2FE_PLC_ELPB_DATA0_t elpb_data;
	L2FE_PLE_PORT_MMSHP_CHK_DATA0_t port_mmshp;
	L2FE_PLE_MMSHP_CHK_ID_MAP_DATA_t mmshp_chk_id;
	L2FE_ARB_PDPID_MAP_TBL_DATA_t pdpid_map;
	L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA_t port_free_cnt;
	L2TM_L2TE_GLB_CTRL_t l2te_ctrl;
	L2TM_L2TE_POL_PORT_CTRL0_t pol_port;
	L2TM_L2TE_CB_SRC0_PRI_THRSH0_t src0_pri_thrsh;
	L2TM_L2TE_CB_SRC1_PRI_THRSH0_t src1_pri_thrsh;
	L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_t src_port_thrsh;
	L2TM_L2TE_CB_GLB_PRI_THRSH0_t glb_pri_thrsh;
	L2TM_L2TE_CB_GLB_THRSH_t   glb_thrsh;
	L2TM_L2TE_CB_PORT_THRSH_PROFILE0_t port_thred;
	L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1_t voq_prf1;
	L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0_t voq_prf0;
	L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_t dq_port_thrsh;
	L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA_t dqsch_voq_thrsh;
	L2FE_PLE_DFT_FWD_DATA_t def_fwd;
	L2FE_ARB_MC_FIB_TBL_DATA3_t mc_data3;
	L2FE_ARB_MC_FIB_TBL_DATA2_t mc_data2;
	L2FE_ARB_MC_FIB_TBL_DATA1_t mc_data1;
	L2FE_ARB_MC_FIB_TBL_DATA0_t mc_data0;
	NI_HV_MCE_INDX_LKUP_DATA0_t mce_indx;
	L2FE_PLE_TX_UNKWN_VLAN_DFT_2_t tx_unkwn_vlan;
	L2FE_PLE_RX_UNKWN_VLAN_DFT_t rx_unkwn_vlan;
	L2FE_PLC_VLAN_MODE_CTRL_t vlan_mode;
	L2FE_L2E_CTRL_t l2e_ctrl;

#if defined(CONFIG_TARGET_SATURN_ASIC)
	/* set gpio to enable network */
	CORTINA_CA_REG_WRITE(0x3000004, GLOBAL_PIN_MUX);
	CORTINA_CA_REG_WRITE(0x220, GLOBAL_GPIO_MUX_0);
	CORTINA_CA_REG_WRITE(0xfffffddf, PER_GPIO0_CFG);
	CORTINA_CA_REG_WRITE(0x200, PER_GPIO0_OUT);
#endif
	ca_ft2_active_gphy_network();

	/* release reset */
	reset.wrd = CORTINA_CA_REG_READ(GLOBAL_BLOCK_RESET);
	reset.bf.reset_ni = 0;
	reset.bf.reset_l2fe = 0;
	reset.bf.reset_l2tm = 0;
	reset.bf.reset_l3fe = 0;
	reset.bf.reset_sdram = 0;
	reset.bf.reset_tqm = 0;
	CORTINA_CA_REG_WRITE(reset.wrd, GLOBAL_BLOCK_RESET);
	/* check ni init done and  fe2ni ready*/
	do {
		udelay(400);
		init.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_INIT_DONE);
		ready.wrd = CORTINA_CA_REG_READ(L2FE_PP_NI_INTF_STS);
		if (init.bf.ni_init_done && ready.bf.rdy)
			break;
		i++;
	} while (i < 50);
	if (i >= 50)
		printf("NI init fail\n");

	/* release port reset */
	CORTINA_CA_REG_WRITE(0x000fffff, NI_HV_GLB_INTF_RST_CONFIG);
	udelay(500);
	CORTINA_CA_REG_WRITE(0, NI_HV_GLB_INTF_RST_CONFIG);

	/* set ni time base */
	ni_tx_misc.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_NITX_MISC_CFG);
	ni_tx_misc.bf.time_base_cnt = 33;
	CORTINA_CA_REG_WRITE(ni_tx_misc.wrd, NI_HV_GLB_NITX_MISC_CFG);
	/* all port ingress goes to L2FE */
	cfg.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_STATIC_CFG);
	cfg.bf.port_to_cpu = 0xf;
	CORTINA_CA_REG_WRITE(cfg.wrd, NI_HV_GLB_STATIC_CFG);
	/* enable QM eq1 pool */
	eq_cfg.wrd = CORTINA_CA_REG_READ(L2TM_L2TM_QM_EQ_CFG);
	eq_cfg.bf.eq0_bufnum = 0;
	eq_cfg.bf.eq0_prvt = 0;
	eq_cfg.bf.eq0_en = 0;
	eq_cfg.bf.eq1_bufnum = 4;
	eq_cfg.bf.eq1_prvt = 0x1d;
	eq_cfg.bf.eq1_en = 1;
	CORTINA_CA_REG_WRITE(eq_cfg.wrd, L2TM_L2TM_QM_EQ_CFG);
	 /* port private buffer configuration, all port selects profile 0 by default */
	 port_prvt_prof.wrd = CORTINA_CA_REG_READ(L2TM_L2TM_QM_PORT_PRVT_PROFILE0);
	 port_prvt_prof.bf.cnt_even = 0x1d;
	 port_prvt_prof.bf.cnt_odd = 0x10;
	 CORTINA_CA_REG_WRITE(port_prvt_prof.wrd, L2TM_L2TM_QM_PORT_PRVT_PROFILE0);
	/* enable QM drop and disable FE backpressure */
	buf_cfg.wrd = CORTINA_CA_REG_READ(L2TM_L2TM_QM_GLOB_BUF_CFG);
	buf_cfg.bf.nodrop = 0x21;
	buf_cfg.bf.drop_en = 1;
	buf_cfg.bf.noncong = 0x4000;
	buf_cfg.bf.fe_bp_en = 0;
	CORTINA_CA_REG_WRITE(buf_cfg.wrd, L2TM_L2TM_QM_GLOB_BUF_CFG);
	/* enable ES tx ports */
	es_ctrl.wrd = CORTINA_CA_REG_READ(L2TM_L2TM_ES_CTRL);
	es_ctrl.bf.tx_en = 1;
	es_ctrl.bf.tx_port0_en = 1;
	es_ctrl.bf.tx_port1_en = 1;
	es_ctrl.bf.tx_port2_en = 1;
	es_ctrl.bf.tx_port3_en = 1;
	es_ctrl.bf.tx_port4_en = 1;
	es_ctrl.bf.tx_port5_en = 1;
	es_ctrl.bf.tx_port6_en = 1;
	es_ctrl.bf.tx_port7_en = 1;
	es_ctrl.bf.tx_port8_en = 1;
	es_ctrl.bf.tx_port9_en = 1;
	es_ctrl.bf.tx_port10_en = 1;
	es_ctrl.bf.tx_port11_en = 1;
	es_ctrl.bf.tx_port12_en = 1;
	es_ctrl.bf.tx_port13_en = 1;
	es_ctrl.bf.tx_port15_en = 1;
	CORTINA_CA_REG_WRITE(es_ctrl.wrd, L2TM_L2TM_ES_CTRL);
	/* permit  unknown vlan packets */
	fwd_ctrl.wrd = CORTINA_CA_REG_READ(L2FE_PLC_DPID_FWD_CTRL);
	fwd_ctrl.bf.unkvlan_fwd = 1;
	CORTINA_CA_REG_WRITE(fwd_ctrl.wrd, L2FE_PLC_DPID_FWD_CTRL);
	/* learn unknown vlan packets and unknown ethertype */
	learn_ctrl.wrd = CORTINA_CA_REG_READ(L2FE_PLC_L2_LEARNING_CTRL);
	learn_ctrl.bf.unkvlan_learn = 1;
	learn_ctrl.bf.unk_ethtype_no_learn = 0;
	CORTINA_CA_REG_WRITE(learn_ctrl.wrd, L2FE_PLC_L2_LEARNING_CTRL);
	/* SVL mode */
	vlan_mode.wrd = CORTINA_CA_REG_READ(L2FE_PLC_VLAN_MODE_CTRL);
	vlan_mode.bf.l2e_glb_vlan_mode = 1;
	CORTINA_CA_REG_WRITE(vlan_mode.wrd, L2FE_PLC_VLAN_MODE_CTRL);
	/* CPU operates FDB and CNT tables mode: command mode */
	l2e_ctrl.wrd = CORTINA_CA_REG_READ(L2FE_L2E_CTRL);
	l2e_ctrl.bf.cpu_opt_mode = 1;
	CORTINA_CA_REG_WRITE(l2e_ctrl.wrd, L2FE_L2E_CTRL);

#if 0
	/* l2te init */
	l2te_ctrl.wrd = CORTINA_CA_REG_READ(L2TM_L2TE_GLB_CTRL);
	l2te_ctrl.bf.pol_en = 1;
	l2te_ctrl.bf.shp_en = 1;
	l2te_ctrl.bf.taildrop_en = 1;
	l2te_ctrl.bf.wred_en = 1;
	CORTINA_CA_REG_WRITE(l2te_ctrl.wrd, L2TM_L2TE_GLB_CTRL);

	/* set port based policer */
	for (port = 0; port < MAX_PORT_ID; port++) {
		pol_port.wrd = CORTINA_CA_REG_READ(L2TM_L2TE_POL_PORT_CTRL0 + port * 4);
		pol_port.bf.type = 1;
		CORTINA_CA_REG_WRITE(pol_port.wrd, L2TM_L2TE_POL_PORT_CTRL0 + port * 4);
	}

	/* init tm cb*/
	/* set src priority threshold */
	for(pri = 0; pri < L2_TM_CB_PRI_NUM; pri++) {
		src0_pri_thrsh.wrd = 0;
		src0_pri_thrsh.bf.hth = 0x280;
		src0_pri_thrsh.bf.lth = 0x110;
		CORTINA_CA_REG_WRITE(src0_pri_thrsh.wrd,L2TM_L2TE_CB_SRC0_PRI_THRSH0 + pri*L2TM_L2TE_CB_SRC0_PRI_THRSH0_STRIDE);
		src1_pri_thrsh.wrd = 0;
		src1_pri_thrsh.bf.hth = 0x280;
		src1_pri_thrsh.bf.lth = 0x110;
		CORTINA_CA_REG_WRITE(src1_pri_thrsh.wrd,L2TM_L2TE_CB_SRC1_PRI_THRSH0 + pri*L2TM_L2TE_CB_SRC1_PRI_THRSH0_STRIDE);
	}
	/* set src port threshold */
	for(profile_id = 0; profile_id < 4; profile_id++) {
		src_port_thrsh.wrd = 0;
		src_port_thrsh.bf.hth = 0xc00;
		src_port_thrsh.bf.lth = 0x100;
		CORTINA_CA_REG_WRITE(src_port_thrsh.wrd,L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_SRC_PORT_THRSH_PROFILE0_STRIDE);
	}
	/* set global priority threshold */
	for(pri = 0; pri<L2_TM_CB_PRI_NUM; pri++) {
		glb_pri_thrsh.wrd = 0;
		glb_pri_thrsh.bf.hth = 0x20;
		glb_pri_thrsh.bf.lth = 0x10;
		CORTINA_CA_REG_WRITE(glb_pri_thrsh.wrd, L2TM_L2TE_CB_GLB_PRI_THRSH0 + L2TM_L2TE_CB_GLB_PRI_THRSH0_STRIDE*pri);
	}
	/* set global threshold */
	glb_thrsh.wrd = 0;
	glb_thrsh.bf.hth = 0x1000;
	glb_thrsh.bf.lth = 0x20;
	CORTINA_CA_REG_WRITE(glb_thrsh.wrd, L2TM_L2TE_CB_GLB_THRSH);
	/* set l2te port free counter */
	for (port = 0; port < L2_TM_CB_PORT_NUM; port++) {
		port_free_cnt.wrd = 0;
		port_free_cnt.bf.cnt0_msb = 1;
		port_free_cnt.bf.cnt0 = 0;
		port_free_cnt.bf.cnt1_msb = 1;
		port_free_cnt.bf.cnt1 = 0xe30;
		CORTINA_CA_REG_WRITE(port_free_cnt.wrd, L2TM_L2TE_CB_PORT_FREECNT_MEM_DATA);
		WRITE_INDIRCT_REG(0, port, L2TM_L2TE_CB_PORT_FREECNT_MEM_ACCESS);
	}
	/* set port threshold profile  */
	for(profile_id = 0; profile_id < 4; profile_id++) {
		port_thred.bf.hth = 0x200;
		port_thred.bf.lth = 0x10;
		CORTINA_CA_REG_WRITE(port_thred.wrd,L2TM_L2TE_CB_PORT_THRSH_PROFILE0 + profile_id * L2TM_L2TE_CB_PORT_GRPTHRSH_PROFILE0_STRIDE);
	}
	/* set voq  threshold profile */
	for(profile_id = 0; profile_id<8; profile_id++) {
		voq_prf1.bf.grphth = 0x7fff;
		voq_prf1.bf.grplth = 0x7fff;
		voq_prf0.bf.hth = 0x200;
		voq_prf0.bf.lth = 0x80;
		CORTINA_CA_REG_WRITE(voq_prf1.wrd,L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA1);
		CORTINA_CA_REG_WRITE(voq_prf0.wrd,L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_DATA0);
		WRITE_INDIRCT_REG(0,profile_id, L2TM_L2TE_CB_VOQ_THRSH_PROFILE_MEM_ACCESS);
	}
	/* set deep queue port threshold profile  */
	for(profile_id = 0; profile_id<4; profile_id++) {
		dq_port_thrsh.wrd = 0;
		dq_port_thrsh.bf.hth = 0x520;
		dq_port_thrsh.bf.lth = 0x20;
		CORTINA_CA_REG_WRITE(dq_port_thrsh.wrd,L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0 + profile_id*L2TM_L2TE_CB_DQSCH_PORT_THRSH_PROFILE0_STRIDE);
	}
	/* set deep queue voq threshold profile */
	for(profile_id = 0; profile_id<8; profile_id++){
		dqsch_voq_thrsh.wrd = 0;
		dqsch_voq_thrsh.bf.hth = 0x160;
		dqsch_voq_thrsh.bf.lth = 0x80;
		CORTINA_CA_REG_WRITE(dqsch_voq_thrsh.wrd, L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_DATA);
		WRITE_INDIRCT_REG(device_id,profile_id,L2TM_L2TE_CB_DQSCH_VOQ_THRSH_PROFILE_MEM_ACCESS);
	}
#endif
	mce_indx.wrd = 0;
	tx_unkwn_vlan.wrd = 0;
	rx_unkwn_vlan.wrd = 0;
	/* set gmii port config */
	for (i = 0; (port = gmii_ports[i]) != INVALID_PORT; i++) {
		printf("Init GMII Port %d\n", port);
		/* set tx mac */
		tx_mac_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_TXMAC_CFG + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_cfg.bf.tx_pfc_disable = 1;
		tx_mac_cfg.bf.tx_pau_sel = 2;
		CORTINA_CA_REG_WRITE(tx_mac_cfg.wrd, (NI_HV_PT_TXMAC_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set tx mac pause timer */
		tx_mac_pause_0.wrd = CORTINA_CA_REG_READ((NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG0 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_0.bf.txpfc_time_0 = 0xffff;
		tx_mac_pause_0.bf.txpfc_time_1 = 0xffff;
		CORTINA_CA_REG_WRITE(tx_mac_pause_0.wrd, (NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG0 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_1.wrd = CORTINA_CA_REG_READ((NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG1 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_1.bf.txpfc_time_2 = 0xffff;
		tx_mac_pause_1.bf.txpfc_time_3 = 0xffff;
		CORTINA_CA_REG_WRITE(tx_mac_pause_1.wrd, (NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG1 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_2.wrd = CORTINA_CA_REG_READ((NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG2 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_2.bf.txpfc_time_4 = 0xffff;
		tx_mac_pause_2.bf.txpfc_time_5 = 0xffff;
		CORTINA_CA_REG_WRITE(tx_mac_pause_2.wrd, (NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG2 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_3.wrd = CORTINA_CA_REG_READ((NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG3 + APB0_NI_HV_PT_STRIDE * port));
		tx_mac_pause_3.bf.txpfc_time_6 = 0xffff;
		tx_mac_pause_3.bf.txpfc_time_7 = 0xffff;
		CORTINA_CA_REG_WRITE(tx_mac_pause_3.wrd, (NI_HV_PT_ETH_TXMAC_PAUSE_TIMER_CFG3 + APB0_NI_HV_PT_STRIDE * port));
		/* disable lpi */
		lpi_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_EEE_LPI_CFG0 + APB0_NI_HV_PT_STRIDE * port));
		lpi_cfg.bf.cpu_lpi_deassert = 1;
		CORTINA_CA_REG_WRITE(lpi_cfg.wrd, (NI_HV_PT_EEE_LPI_CFG0 + APB0_NI_HV_PT_STRIDE * port));
		/* set to  GMII mode */
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_cfg.bf.int_cfg = 0;
		port_cfg.bf.mac_addr6 = 0x2;
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set speed/duplex */
		port_glb.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_glb.bf.duplex = 1;
		CORTINA_CA_REG_WRITE(port_glb.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set ilpb_stp_mode */
#if defined(CONFIG_TARGET_VENUS)
		READ_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
		ilpb_data_2.wrd = CORTINA_CA_REG_READ(L2FE_PP_ILPB_DATA2);
		ilpb_data_2.bf.ilpb_stp_mode = 3;
		CORTINA_CA_REG_WRITE(ilpb_data_2.wrd, L2FE_PP_ILPB_DATA2);
		WRITE_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
#else
		READ_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
		ilpb_data.wrd = CORTINA_CA_REG_READ(L2FE_PP_ILPB_DATA1);
		ilpb_data.bf.ilpb_stp_mode = 3;
		CORTINA_CA_REG_WRITE(ilpb_data.wrd, L2FE_PP_ILPB_DATA1);
		WRITE_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
#endif
		/* set egress port stp status to forward */
		READ_INDIRCT_REG(0, port, L2FE_PLC_ELPB_ACCESS);
		elpb_data.wrd = CORTINA_CA_REG_READ(L2FE_PLC_ELPB_DATA0);
		elpb_data.bf.elpb_egr_port_stp_status = 0x1;
		CORTINA_CA_REG_WRITE(elpb_data.wrd, L2FE_PLC_ELPB_DATA0);
		WRITE_INDIRCT_REG (0, port, L2FE_PLC_ELPB_ACCESS);
		/* set port membership check*/
		port_mmshp.bf.ldpid_bm = 0xffffffff;
		CORTINA_CA_REG_WRITE(port_mmshp.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA0);
		CORTINA_CA_REG_WRITE(port_mmshp.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA1);
		WRITE_INDIRCT_REG (0, port, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
		/* set membership check id */
		mmshp_chk_id.bf.mmshp_chk_id_map_chk_id = port;
		CORTINA_CA_REG_WRITE(mmshp_chk_id.wrd, L2FE_PLE_MMSHP_CHK_ID_MAP_DATA);
		WRITE_INDIRCT_REG (0, port, L2FE_PLE_MMSHP_CHK_ID_MAP_ACCESS);
		tx_unkwn_vlan.bf.memshp_bm |= (0x1 << port);
		rx_unkwn_vlan.bf.memshp_bm |= (0x1 << port);
		/* set the physical destination port ID mapping */
		pdpid_map.wrd = 0;
		pdpid_map.bf.pdpid = port;
		CORTINA_CA_REG_WRITE(pdpid_map.wrd, L2FE_ARB_PDPID_MAP_TBL_DATA);
		WRITE_INDIRCT_REG(0, port, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
		/* set BC forwarding  to mc_group_id 0 */
		index = (port << 2) | 0x0;
		READ_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		def_fwd.wrd = CORTINA_CA_REG_READ(L2FE_PLE_DFT_FWD_DATA);
		def_fwd.bf.dft_fwd_mc_group_id_valid = 1;
		def_fwd.bf.dft_fwd_mc_group_id = 0;
		CORTINA_CA_REG_WRITE(def_fwd.wrd, L2FE_PLE_DFT_FWD_DATA);
		WRITE_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		/* set UUC forwarding  to mc_group_id 0 */
		index = (port << 2) | 0x1;
		READ_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		def_fwd.wrd = CORTINA_CA_REG_READ(L2FE_PLE_DFT_FWD_DATA);
		def_fwd.bf.dft_fwd_mc_group_id_valid = 1;
		def_fwd.bf.dft_fwd_mc_group_id = 0;
		CORTINA_CA_REG_WRITE(def_fwd.wrd, L2FE_PLE_DFT_FWD_DATA);
		WRITE_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		/* set UL2MC forwarding  to mc_group_id 0 */
		index = (port << 2) | 0x2;
		READ_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		def_fwd.wrd = CORTINA_CA_REG_READ(L2FE_PLE_DFT_FWD_DATA);
		def_fwd.bf.dft_fwd_mc_group_id_valid = 1;
		def_fwd.bf.dft_fwd_mc_group_id = 0;
		CORTINA_CA_REG_WRITE(def_fwd.wrd, L2FE_PLE_DFT_FWD_DATA);
		WRITE_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		/* set mc fib */
		mc_data3.wrd = 0;
		mc_data2.wrd = 0;
		mc_data1.wrd = 0;
		mc_data0.wrd = 0;
		mc_data3.bf.cos_cmd = 1;
		mc_data3.bf.ldpid = port;
		CORTINA_CA_REG_WRITE(mc_data3.wrd, L2FE_ARB_MC_FIB_TBL_DATA3);
		CORTINA_CA_REG_WRITE(mc_data2.wrd, L2FE_ARB_MC_FIB_TBL_DATA2);
		CORTINA_CA_REG_WRITE(mc_data1.wrd, L2FE_ARB_MC_FIB_TBL_DATA1);
		CORTINA_CA_REG_WRITE(mc_data0.wrd, L2FE_ARB_MC_FIB_TBL_DATA0);
		WRITE_INDIRCT_REG(0, port, L2FE_ARB_MC_FIB_TBL_ACCESS);
		/* set mce index */
		mce_indx.bf.mc_vec |= (0x1 << port);
	}

	/* set rgmii port config */
	for (i = 0; (port = rgmii_ports[i]) != INVALID_PORT; i++) {
		printf("Init RGMII Port %d\n", port);
		/* set to  RGMII mode */
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_cfg.bf.int_cfg = 2;
		port_cfg.bf.mac_addr6 = 0x2;
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set speed/duplex */
		port_glb.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_glb.bf.duplex = 1;
		CORTINA_CA_REG_WRITE(port_glb.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		/* set ilpb_stp_mode */
#if defined(CONFIG_TARGET_VENUS)
		READ_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
		ilpb_data_2.wrd = CORTINA_CA_REG_READ(L2FE_PP_ILPB_DATA2);
		ilpb_data_2.bf.ilpb_stp_mode = 3;
		CORTINA_CA_REG_WRITE(ilpb_data_2.wrd, L2FE_PP_ILPB_DATA2);
		WRITE_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
#else
		READ_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
		ilpb_data.wrd = CORTINA_CA_REG_READ(L2FE_PP_ILPB_DATA1);
		ilpb_data.bf.ilpb_stp_mode = 3;
		CORTINA_CA_REG_WRITE(ilpb_data.wrd, L2FE_PP_ILPB_DATA1);
		WRITE_INDIRCT_REG(0, port, L2FE_PP_ILPB_ACCESS);
#endif
		/* set egress port stp status to forward */
		READ_INDIRCT_REG(0, port, L2FE_PLC_ELPB_ACCESS);
		elpb_data.wrd = CORTINA_CA_REG_READ(L2FE_PLC_ELPB_DATA0);
		elpb_data.bf.elpb_egr_port_stp_status = 0x1;
		CORTINA_CA_REG_WRITE(elpb_data.wrd, L2FE_PLC_ELPB_DATA0);
		WRITE_INDIRCT_REG (0, port, L2FE_PLC_ELPB_ACCESS);
		/* set port membership check*/
		port_mmshp.bf.ldpid_bm = 0xffffffff;
		CORTINA_CA_REG_WRITE(port_mmshp.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA0);
		CORTINA_CA_REG_WRITE(port_mmshp.wrd, L2FE_PLE_PORT_MMSHP_CHK_DATA1);
		WRITE_INDIRCT_REG (0, port, L2FE_PLE_PORT_MMSHP_CHK_ACCESS);
		/* set membership check id */
		mmshp_chk_id.bf.mmshp_chk_id_map_chk_id = port;
		CORTINA_CA_REG_WRITE(mmshp_chk_id.wrd, L2FE_PLE_MMSHP_CHK_ID_MAP_DATA);
		WRITE_INDIRCT_REG (0, port, L2FE_PLE_MMSHP_CHK_ID_MAP_ACCESS);
		tx_unkwn_vlan.bf.memshp_bm |= (0x1 << port);
		rx_unkwn_vlan.bf.memshp_bm |= (0x1 << port);
		/* set the physical destination port ID mapping */
		pdpid_map.wrd = 0;
		pdpid_map.bf.pdpid = port;
		CORTINA_CA_REG_WRITE(pdpid_map.wrd, L2FE_ARB_PDPID_MAP_TBL_DATA);
		WRITE_INDIRCT_REG(0, port, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
		/* set BC forwarding to mc_group_id 0*/
		index = (port << 2) | 0x0;
		READ_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		def_fwd.wrd = CORTINA_CA_REG_READ(L2FE_PLE_DFT_FWD_DATA);
		def_fwd.bf.dft_fwd_mc_group_id_valid = 1;
		def_fwd.bf.dft_fwd_mc_group_id = 0;
		CORTINA_CA_REG_WRITE(def_fwd.wrd, L2FE_PLE_DFT_FWD_DATA);
		WRITE_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		/* set UUC forwarding  to mc_group_id 0 */
		index = (port << 2) | 0x1;
		READ_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		def_fwd.wrd = CORTINA_CA_REG_READ(L2FE_PLE_DFT_FWD_DATA);
		def_fwd.bf.dft_fwd_mc_group_id_valid = 1;
		def_fwd.bf.dft_fwd_mc_group_id = 0;
		CORTINA_CA_REG_WRITE(def_fwd.wrd, L2FE_PLE_DFT_FWD_DATA);
		WRITE_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		/* set UL2MC forwarding  to mc_group_id 0 */
		index = (port << 2) | 0x2;
		READ_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		def_fwd.wrd = CORTINA_CA_REG_READ(L2FE_PLE_DFT_FWD_DATA);
		def_fwd.bf.dft_fwd_mc_group_id_valid = 1;
		def_fwd.bf.dft_fwd_mc_group_id = 0;
		CORTINA_CA_REG_WRITE(def_fwd.wrd, L2FE_PLE_DFT_FWD_DATA);
		WRITE_INDIRCT_REG(0, index, L2FE_PLE_DFT_FWD_ACCESS);
		/* set mc fib */
		mc_data3.wrd = 0;
		mc_data2.wrd = 0;
		mc_data1.wrd = 0;
		mc_data0.wrd = 0;
		mc_data3.bf.cos_cmd = 1;
		mc_data3.bf.ldpid = port;
		CORTINA_CA_REG_WRITE(mc_data3.wrd, L2FE_ARB_MC_FIB_TBL_DATA3);
		CORTINA_CA_REG_WRITE(mc_data2.wrd, L2FE_ARB_MC_FIB_TBL_DATA2);
		CORTINA_CA_REG_WRITE(mc_data1.wrd, L2FE_ARB_MC_FIB_TBL_DATA1);
		CORTINA_CA_REG_WRITE(mc_data0.wrd, L2FE_ARB_MC_FIB_TBL_DATA0);
		WRITE_INDIRCT_REG(0, port, L2FE_ARB_MC_FIB_TBL_ACCESS);
		/* set mce index */
		mce_indx.bf.mc_vec |= (0x1 << port);
	}
	/* set tx vlan membership bitmap for unknown vlan packet */
	CORTINA_CA_REG_WRITE(tx_unkwn_vlan.wrd, L2FE_PLE_TX_UNKWN_VLAN_DFT_2);
	/* set rx vlan membership bitmap for unknown vlan packet */
	CORTINA_CA_REG_WRITE(rx_unkwn_vlan.wrd, L2FE_PLE_RX_UNKWN_VLAN_DFT);
	/* set mce index for mc_group_id 0*/
	CORTINA_CA_REG_WRITE(mce_indx.wrd, NI_HV_MCE_INDX_LKUP_DATA0);
	WRITE_INDIRCT_REG(0, 0, NI_HV_MCE_INDX_LKUP_ACCESS);
	/* set MC port to the physical destination port ID mapping */
	pdpid_map.wrd = 0;
	pdpid_map.bf.pdpid = L2_PPORT_MC;
	CORTINA_CA_REG_WRITE(pdpid_map.wrd, L2FE_ARB_PDPID_MAP_TBL_DATA);
	WRITE_INDIRCT_REG(0, L2_LPORT_MC, L2FE_ARB_PDPID_MAP_TBL_ACCESS);
	/* set BLACKHOLE port to the physical destination port ID mapping */
	pdpid_map.wrd = 0;
	pdpid_map.bf.pdpid = L2_PPORT_BLACKHOLE;
	CORTINA_CA_REG_WRITE(pdpid_map.wrd, L2FE_ARB_PDPID_MAP_TBL_DATA);
	WRITE_INDIRCT_REG(0, L2_LPORT_BLACKHOLE, L2FE_ARB_PDPID_MAP_TBL_ACCESS);

	return 0;
}
#endif

int do_ni_mib(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_uint32_t port_id;
	ca_boolean_t read_clear = 0;
	ca_uint32_t rx_stats[ __NI_MIB_RX_NUM];
	ca_uint32_t tx_stats[__NI_MIB_TX_NUM];
	ca_uint64 count;

	if (argc <= 1) {
		return 0;
	} else {
		port_id = simple_strtoul(argv[1], NULL, 10);
		if (NULL != argv[2])
			read_clear = simple_strtoul(argv[2], NULL, 10);
	}

	ni_eth_port_rx_mib_get(port_id, read_clear, rx_stats);
	ni_eth_port_tx_mib_get(port_id, read_clear, tx_stats);

	printf("%-30s: %10u\n", "rx_uc_pkt_cnt", rx_stats[__NI_MIB_RX_UC_PKT_CNT]);
	printf("%-30s: %10u\n", "rx_mc_frame_cnt", rx_stats[__NI_MIB_RX_MC_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_bc_frame_cnt", rx_stats[__NI_MIB_RX_BC_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_oam_frame_cnt", rx_stats[__NI_MIB_RX_OAM_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_jumbo_frame_cnt", rx_stats[__NI_MIB_RX_JUMBO_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_pause_frame_cnt", rx_stats[__NI_MIB_RX_PAUSE_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_pfc_frame_cnt", rx_stats[__NI_MIB_RX_PFC_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_unk_opcode_frame_cnt", rx_stats[__NI_MIB_RX_UNK_OPCODE_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_crc_error_frame_cnt", rx_stats[__NI_MIB_RX_CRC_ERR_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_undersize_frame_cnt", rx_stats[__NI_MIB_RX_UNDER_SIZE_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_runt_frame_cnt", rx_stats[__NI_MIB_RX_RUNT_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_ovsize_frame_cnt", rx_stats[__NI_MIB_RX_OVSIZE_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_jabber_frame_cnt", rx_stats[__NI_MIB_RX_JABBER_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_invalid_frame_cnt", rx_stats[__NI_MIB_RX_INVALID_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_invalid_flow_frame_cnt", rx_stats[__NI_MIB_RX_INVALID_FLOW_FRM_CNT]);
	printf("%-30s: %10u\n", "rx_frame_64_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_64_OCT]);
	printf("%-30s: %10u\n", "rx_frame_65to127_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_65_TO_127_OCT]);
	printf("%-30s: %10u\n", "rx_frame_128to255_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_128_TO_255_OCT]);
	printf("%-30s: %10u\n", "rx_frame_256to511_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_256_TO_511_OCT]);
	printf("%-30s: %10u\n", "rx_frame_512to1023_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_512_TO_1023_OCT]);
	printf("%-30s: %10u\n", "rx_frame_1024to1518_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_1024_TO_1518_OCT]);
	printf("%-30s: %10u\n", "rx_frame_1519to2100_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_1519_TO_2100_OCT]);
	printf("%-30s: %10u\n", "rx_frame_2101to9200_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_2101_TO_9200_OCT]);
	printf("%-30s: %10u\n", "rx_frame_9201tomax_cnt", rx_stats[__NI_MIB_RX_STATS_FRM_9201_TO_MAX_OCT]);
	count = rx_stats[__NI_MIB_RX_BYTE_COUNT_HI];
	count = count << 32| rx_stats[__NI_MIB_RX_BYTE_COUNT_LO];
	printf("%-30s: %10llu\n", "rx_byte_cnt", count);
	printf("\n");
	printf("%-30s: %10u\n", "tx_uc_pkt_cnt", tx_stats[__NI_MIB_TX_UC_PKT_CNT]);
	printf("%-30s: %10u\n", "tx_mc_frame_cnt", tx_stats[__NI_MIB_TX_MC_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_bc_frame_cnt", tx_stats[__NI_MIB_TX_BC_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_oam_frame_cnt", tx_stats[__NI_MIB_TX_OAM_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_jumbo_frame_cnt", tx_stats[__NI_MIB_TX_JUMBO_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_pause_frame_cnt", tx_stats[__NI_MIB_TX_PAUSE_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_pfc_frame_cnt", tx_stats[__NI_MIB_TX_PFC_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_crc_error_frame_cnt", tx_stats[__NI_MIB_TX_CRCERR_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_ovsize_frame_cnt", tx_stats[__NI_MIB_TX_OVSIZE_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_single_col_frame_cnt", tx_stats[__NI_MIB_TX_SINGLE_COL_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_multi_col_frame_cnt", tx_stats[__NI_MIB_TX_MULTI_COL_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_late_col_frame_cnt", tx_stats[__NI_MIB_TX_LATE_COL_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_exess_col_frame_cnt", tx_stats[__NI_MIB_TX_EXESS_COL_FRM_CNT]);
	printf("%-30s: %10u\n", "tx_frame_64_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_64_OCT]);
	printf("%-30s: %10u\n", "tx_frame_65to127_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_65_TO_127_OCT]);
	printf("%-30s: %10u\n", "tx_frame_128to255_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_128_TO_255_OCT]);
	printf("%-30s: %10u\n", "tx_frame_256to511_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_256_TO_511_OCT]);
	printf("%-30s: %10u\n", "tx_frame_512to1023_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_512_TO_1023_OCT]);
	printf("%-30s: %10u\n", "tx_frame_1024to1518_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_1024_TO_1518_OCT]);
	printf("%-30s: %10u\n", "tx_frame_1519to2100_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_1519_TO_2100_OCT]);
	printf("%-30s: %10u\n", "tx_frame_2101to9200_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_2101_TO_9200_OCT]);
	printf("%-30s: %10u\n", "tx_frame_9201tomax_cnt", tx_stats[__NI_MIB_TX_STATS_FRM_9201_TO_MAX_OCT]);
	count = tx_stats[__NI_MIB_TX_BYTE_CNT_HI];
	count = count << 32| tx_stats[__NI_MIB_TX_BYTE_CNT_LO];
	printf("%-30s: %10llu\n", "tx_byte_cnt", count);
	printf("\n");

	return 0;
}

int do_ni_mac(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_uint32_t i, port, mac_speed = NI_MAC_SPEED_MAX;
	char *speed[NI_MAC_SPEED_MAX+1] = {"None", "100F", "10F", "1000F"};
	NI_HV_PT_PORT_STATIC_CFG_t port_cfg;
	NI_HV_PT_PORT_GLB_CFG_t port_glb;

	if (argc > 1) {
		for (mac_speed = NI_MAC_SPEED_100F; mac_speed < NI_MAC_SPEED_MAX; mac_speed++) {
			if (0 == strcmp(argv[1], speed[mac_speed]))
				break;
		}
	}

	/* set gmii port config */
	for (i = 0; (port = gmii_ports[i]) != INVALID_PORT; i++) {
		printf("GMII Port %d: %s\n", port, speed[mac_speed]);
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_glb.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		switch(mac_speed) {
			case NI_MAC_SPEED_100F:
				/* set to  MII mode */
				port_cfg.bf.int_cfg = 1;
				port_glb.bf.speed = 0;
				port_glb.bf.duplex = 0;
				break;
			case NI_MAC_SPEED_10F:
				/* set to  MII mode */
				port_cfg.bf.int_cfg = 1;
				port_glb.bf.speed = 1;
				port_glb.bf.duplex = 0;
				break;
			default:
				/* set to  GMII mode */
				port_cfg.bf.int_cfg = 0;
				port_glb.bf.duplex = 1;
		}
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		CORTINA_CA_REG_WRITE(port_glb.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
	}

	/* set rgmii port config */
	for (i = 0; (port = rgmii_ports[i]) != INVALID_PORT; i++) {
		printf("RGMII Port %d: %s\n", port, speed[mac_speed]);
		port_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		port_glb.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
		switch(mac_speed) {
			case NI_MAC_SPEED_100F:
				/* set to  RGMII_FE mode */
				port_cfg.bf.int_cfg = 3;
				port_glb.bf.speed = 0;
				port_glb.bf.duplex = 0;
				break;
			case NI_MAC_SPEED_10F:
				/* set to  RGMII_FE mode */
				port_cfg.bf.int_cfg = 3;
				port_glb.bf.speed = 1;
				port_glb.bf.duplex = 0;
				break;
			default:
				/* set to  RGMII_GE mode */
				port_cfg.bf.int_cfg = 2;
				port_glb.bf.duplex = 1;
		}
		CORTINA_CA_REG_WRITE(port_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * port));
		CORTINA_CA_REG_WRITE(port_glb.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * port));
	}

	return 0;
}

int do_ni_rxmux_counter(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_uint32_t port_id;
	ca_boolean_t read_clear = 0;
	NI_HV_GLB_RXMUX_CTRL_CFG_t ctrl;
	NI_HV_GLB_RXMUX_PKTCNT_t pkt_cnt;

	if (argc <= 1) {
		return 0;
	} else {
		port_id = simple_strtoul(argv[1], NULL, 10);
		if (NULL != argv[2])
			read_clear = simple_strtoul(argv[2], NULL, 10);
	}

	ctrl.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMUX_CTRL_CFG);
	if (ctrl.bf.dbg_sel != port_id) {
		ctrl.bf.dbg_sel = port_id;
		CORTINA_CA_REG_WRITE(ctrl.wrd, NI_HV_GLB_RXMUX_CTRL_CFG);
	}

	if (read_clear) {
		CORTINA_CA_REG_WRITE(0, NI_HV_GLB_RXMUX_PKTCNT);
	}

	pkt_cnt.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMUX_PKTCNT);
	printf("%-10s: %10u\n", "rxmux_pkt_cnt", pkt_cnt.bf.dbg_pkt_cnt);

	return 0;
}

int do_ni_mce_counter(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	NI_HV_MCE_INGRESS_CNT_t in_cnt;
	NI_HV_MCE_EGRESS_CNT_t out_cnt;
	NI_HV_MCE_ZERO_BITMAP_DROP_CNT_t drop_cnt;

	in_cnt.wrd = CORTINA_CA_REG_READ(NI_HV_MCE_INGRESS_CNT);
	out_cnt.wrd = CORTINA_CA_REG_READ(NI_HV_MCE_EGRESS_CNT);
	drop_cnt.wrd = CORTINA_CA_REG_READ(NI_HV_MCE_ZERO_BITMAP_DROP_CNT);
	printf("%-18s: %10u\n", "mce_rx_pkt_cnt", in_cnt.bf.cnt);
	printf("%-18s: %10u\n", "mce_tx_pkt_cnt", out_cnt.bf.cnt);
	printf("%-18s: %10u\n", "mce_drop_cnt", drop_cnt.bf.cnt);

	return 0;
}

int do_ni_mce_in_header(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "last_in_hdr2", CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2));
	printf("%-20s: 0x%08x\n", "last_in_hdr1", CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1));
	printf("%-20s: 0x%08x\n", "last_in_hdr0", CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR0));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR2)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_IN_HDR1)>>3)&0x3f);

	return 0;
}

int do_ni_mce_out_header(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "last_in_hdr2", CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2));
	printf("%-20s: 0x%08x\n", "last_in_hdr1", CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1));
	printf("%-20s: 0x%08x\n", "last_in_hdr0", CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR0));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR2)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(NI_HV_MCE_LAST_OUT_HDR1)>>3)&0x3f);

	return 0;
}

static cmd_tbl_t cmd_ni_sub[] = {
	//U_BOOT_CMD_MKENT(init, 1, 1, do_ni_init, "", ""),
	U_BOOT_CMD_MKENT(mib, 3, 1, do_ni_mib, "", ""),
	U_BOOT_CMD_MKENT(mac, 2, 1, do_ni_mac, "", ""),
	U_BOOT_CMD_MKENT(rxmux_cnt, 3, 1, do_ni_rxmux_counter, "", ""),
	U_BOOT_CMD_MKENT(mce_cnt, 3, 1, do_ni_mce_counter, "", ""),
	U_BOOT_CMD_MKENT(mce_in_header, 1, 1, do_ni_mce_in_header, "", ""),
	U_BOOT_CMD_MKENT(mce_out_header, 1, 1, do_ni_mce_out_header, "", ""),
};

int do_ni(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'ni' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_ni_sub[0], ARRAY_SIZE(cmd_ni_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char ni_help_text[] =
	"init - Init NI\n"
	"ni mib <port> <read_clear>- Show port MIB\n"
	"ni mac <100F/10F>- Set MAC speed\n"
	"ni rxmux_cnt <port> <read_clear>- Show RX MUX packet counter\n"
	"ni mce_cnt - Show MCE ingress and egress packet counter\n"
	"ni mce_in_header - Show NI MCE last in header\n"
	"ni mce_out_header - Show NI MCE last out header\n";
#endif

U_BOOT_CMD(
	ni, 5, 1, do_ni,
	"NI utility commands",
	ni_help_text
);


/******************************************
*                   L2FE commands
*******************************************/
int do_ni_intf_counter(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_boolean_t read_clear = 0;
	NI_HV_GLB_INIT_DONE_t init;
	L2FE_PP_NI_INTF_STS_t ready;
	L2FE_PP_NI_INTF_PKT_CNT_t pkt_cnt;
	L2FE_PP_NI_INTF_DROP_CNT_t drop_cnt;

	if (argc >= 2) {
		read_clear = simple_strtoul(argv[1], NULL, 10);
	}

	init.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_INIT_DONE);
	ready.wrd = CORTINA_CA_REG_READ(L2FE_PP_NI_INTF_STS);

	if (read_clear) {
		CORTINA_CA_REG_WRITE(0, L2FE_PP_NI_INTF_PKT_CNT);
		CORTINA_CA_REG_WRITE(0, L2FE_PP_NI_INTF_DROP_CNT);
	}
	pkt_cnt.wrd = CORTINA_CA_REG_READ(L2FE_PP_NI_INTF_PKT_CNT);
	drop_cnt.wrd = CORTINA_CA_REG_READ(L2FE_PP_NI_INTF_DROP_CNT);

	printf("%-14s: %10u\n", "ni_init_done", init.bf.ni_init_done);
	printf("%-14s: %10u\n", "ni_l2fe_ready", ready.bf.rdy);

	printf("%-14s: %10u\n", "sop_cnt", pkt_cnt.bf.sop);
	printf("%-14s: %10u\n", "eop_cnt", pkt_cnt.bf.eop);
	printf("%-14s: %10u\n", "drop_cnt", drop_cnt.bf.drop);

	return 0;
}

int do_l2fe_drop_source_cnt(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	l2fe_drop_source_t drop_source_cnt;
	ca_uint8_t drop_source_id;
	ca_uint32_t *p = (ca_uint32_t*)&drop_source_cnt;

	/* clear counter */
	for(drop_source_id = 0; drop_source_id<32; drop_source_id++) {
		READ_INDIRCT_REG (0, drop_source_id, L2FE_PE_DROP_STTS_ACCESS);
		*p =  CORTINA_CA_REG_READ(L2FE_PE_DROP_STTS_DATA);
		p++;
	}
	/* get counter */
	for(drop_source_id = 0; drop_source_id<32; drop_source_id++) {
		CORTINA_CA_REG_WRITE(0,L2FE_PE_DROP_STTS_DATA);
		WRITE_INDIRCT_REG (0, drop_source_id, L2FE_PE_DROP_STTS_ACCESS);
	}
	printf("%-36s: %10u\n", "drop_source_cnt_00", drop_source_cnt.drop_source_cnt_00);
	printf("%-36s: %10u\n", "ipv4_cs_chk_drp_01", drop_source_cnt.ipv4_cs_chk_drp_01);
	printf("%-36s: %10u\n", "dpid_blackhole_drp_02", drop_source_cnt.dpid_blackhole_drp_02);
	printf("%-36s: %10u\n", "ilpb_stp_chk_drp_03", drop_source_cnt.ilpb_stp_chk_drp_03);
	printf("%-36s: %10u\n", "vlan_stp_chk_drp_04", drop_source_cnt.vlan_stp_chk_drp_04);
	printf("%-36s: %10u\n", "ilpb_vlan_type_drp_05", drop_source_cnt.ilpb_vlan_type_drp_05);
	printf("%-36s: %10u\n", "igr_cls_pmt_stp_drp_06", drop_source_cnt.igr_cls_pmt_stp_drp_06);
	printf("%-36s: %10u\n", "vid4095_drp_07", drop_source_cnt.vid4095_drp_07);
	printf("%-36s: %10u\n", "unknown_vlan_drp_08", drop_source_cnt.unknown_vlan_drp_08);
	printf("%-36s: %10u\n", "ve_deny_mc_flag_drp_09", drop_source_cnt.ve_deny_mc_flag_drp_09);
	printf("%-36s: %10u\n", "l2e_da_deny_drp_10", drop_source_cnt.l2e_da_deny_drp_10);
	printf("%-36s: %10u\n", "non_std_sa_drp_11", drop_source_cnt.non_std_sa_drp_11);
	printf("%-36s: %10u\n", "sw_learn_err_drp_12", drop_source_cnt.sw_learn_err_drp_12);
	printf("%-36s: %10u\n", "l2e_sa_deny_drp_13", drop_source_cnt.l2e_sa_deny_drp_13);
	printf("%-36s: %10u\n", "sa_mis_or_sa_limit_drp_14", drop_source_cnt.sa_mis_or_sa_limit_drp_14);
	printf("%-36s: %10u\n", "uuc_umc_bc_drp_15", drop_source_cnt.uuc_umc_bc_drp_15);
	printf("%-36s: %10u\n", "spid_dpid_not_in_rx_vlan_mem_drp_16", drop_source_cnt.spid_dpid_not_in_rx_vlan_mem_drp_16);
	printf("%-36s: %10u\n", "dpid_not_in_port_mem_drp_17", drop_source_cnt.dpid_not_in_port_mem_drp_17);
	printf("%-36s: %10u\n", "spid_dpid_not_in_tx_vlan_mem_drp_18", drop_source_cnt.spid_dpid_not_in_tx_vlan_mem_drp_18);
	printf("%-36s: %10u\n", "tx_vlan_stp_drp_19", drop_source_cnt.tx_vlan_stp_drp_19);
	printf("%-36s: %10u\n", "egr_cls_deny_drp_20", drop_source_cnt.egr_cls_deny_drp_20);
	printf("%-36s: %10u\n", "loopback_drp_21", drop_source_cnt.loopback_drp_21);
	printf("%-36s: %10u\n", "elpb_stp_drp_22", drop_source_cnt.elpb_stp_drp_22);
	printf("%-36s: %10u\n", "drop_source_cnt_23", drop_source_cnt.drop_source_cnt_23);
	printf("%-36s: %10u\n", "drop_source_cnt_24", drop_source_cnt.drop_source_cnt_24);
	printf("%-36s: %10u\n", "mcfib_vlan_drp_25", drop_source_cnt.mcfib_vlan_drp_25);
	printf("%-36s: %10u\n", "drop_source_cnt_26", drop_source_cnt.drop_source_cnt_26);
	printf("%-36s: %10u\n", "mcfib_dpid_equal_spid_drp_27", drop_source_cnt.mcfib_dpid_equal_spid_drp_27);
	printf("%-36s: %10u\n", "drop_source_cnt_28", drop_source_cnt.drop_source_cnt_28);
	printf("%-36s: %10u\n", "drop_source_cnt_29", drop_source_cnt.drop_source_cnt_29);
	printf("%-36s: %10u\n", "drop_source_cnt_30", drop_source_cnt.drop_source_cnt_30);
	printf("%-36s: %10u\n", "drop_source_cnt_31", drop_source_cnt.drop_source_cnt_31);

	return 0;
}

int do_l2fe_pp_heada(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "heada_hi", CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI));
	printf("%-20s: 0x%08x\n", "heada_mid", CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID));
	printf("%-20s: 0x%08x\n", "heada_low", CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_LOW));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type",	(CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_HI)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(L2FE_PP_HEADER_A_MID)>>3)&0x3f);

	return 0;
}

int do_l2fe_pe_heada(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "heada_hi", CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI));
	printf("%-20s: 0x%08x\n", "heada_mid", CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID));
	printf("%-20s: 0x%08x\n", "heada_low", CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_LOW));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_HI)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(L2FE_PE_HEADER_A_MID)>>3)&0x3f);

	return 0;
}

int l2fe_fdb_cpu_cmd_clean(void)
{
	l2fe_fdb_cpu_cmd_word4_msk_t word4;

	word4.wrd = 0xffff;
	CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_CPU_CMD_WORD0);
	CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_CPU_CMD_WORD1);
	CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_CPU_CMD_WORD2);
	CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_CPU_CMD_WORD3);
	CORTINA_CA_REG_WRITE(word4.wrd, L2FE_L2E_FDB_CPU_CMD_WORD4);

	return 0;
}


int do_l2fe_fdb(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	ca_uint32_t i = 0, start = 0, next = 0;
	L2FE_L2E_FDB_ACCESS_t l2_fdb_access;
	L2FE_L2E_FDB_CMD_RETURN_t cmd_ret;
	L2FE_L2E_FDB_DATA0_t fdb_0;
	L2FE_L2E_FDB_DATA1_t fdb_1;
	L2FE_L2E_FDB_DATA2_t fdb_2;
	L2FE_L2E_FDB_DATA3_t fdb_3;
	ca_uint8_t mac[6];

	/* If busy, just return error */
	l2_fdb_access.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_ACCESS);

	if (l2_fdb_access.bf.access) {
		printf("HW BUSY, wrd:0x%08x\n", l2_fdb_access.wrd);
		return 0;
	}

	l2fe_fdb_cpu_cmd_clean();

	printf("      lrn-vid	key-vid 		    static  mc  tmp	SA	DA\n");
	printf("port v (s/c)	(s/c)		MAC         flag    flag flag  permit permit age addr\n");
	printf("----   -------   -------   ----------------- ----- ---- ----- ------ ------  --- -----\n");

	while(1) {
		/* init cmd return */
		CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_CMD_RETURN);
		/* init fdb data */
		CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_DATA0);
		CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_DATA1);
		CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_DATA2);
		CORTINA_CA_REG_WRITE(0, L2FE_L2E_FDB_DATA3);
		/* set fdb access */
		/* __FDB_TBL_OPCODE_SCAN */
		l2_fdb_access.wrd = 0;
		l2_fdb_access.bf.access = 1;
		l2_fdb_access.bf.rbw = 0;
		/*__FDB_TBL_OPCODE_SCAN, and  start addr from 1 */
		l2_fdb_access.wrd |= ((start & 0x1FFF) << 8 | 0x1);
		CORTINA_CA_REG_WRITE(l2_fdb_access.wrd, L2FE_L2E_FDB_ACCESS);
		do {
			udelay(500);
			l2_fdb_access.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_ACCESS);
			if (!l2_fdb_access.bf.access)
				break;
		} while (i++ < 100);

		/* check cmd return value */
		cmd_ret.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_CMD_RETURN);
		/* CA_AAL_FDB_ACCESS_HIT */
		if (cmd_ret.bf.status == 5) {
			fdb_0.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_DATA0);
			fdb_1.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_DATA1);
			fdb_2.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_DATA2);
			fdb_3.wrd = CORTINA_CA_REG_READ(L2FE_L2E_FDB_DATA3);
			mac[0] = (fdb_3.bf.mac_addr << 5) | ((fdb_2.bf.mac_addr >> 27) & 0x1f);
			mac[1] = (fdb_2.bf.mac_addr >> 19) & 0xff;
			mac[2] = (fdb_2.bf.mac_addr >> 11) & 0xff;
			mac[3] = (fdb_2.bf.mac_addr >>  3) & 0xff;
			mac[4] = ((fdb_2.bf.mac_addr & 0x7) << 5) | ((fdb_1.bf.mac_addr >> 8) & 0x1f);
			mac[5] = fdb_1.bf.mac_addr & 0xff;
			next = cmd_ret.bf.ext_status + 1;
			printf("%-4u %u  %4u(%-1s) %4u(%-1s)    %02x:%02x:%02x:%02x:%02x:%02x %-5u %-3u   %-6u   %-4u  %-4u %-3u %u\n",
				(fdb_0.bf.mc_flag) ? fdb_0.bf.mcgid : (fdb_0.bf.mcgid & 0x3f),
#if defined(CONFIG_TARGET_VENUS)
				fdb_3.bf.lrned_tag_vld,
#else
				1,
#endif
				(fdb_1.bf.lrned_vid << 9) | fdb_0.bf.lrned_vid,
				(fdb_0.bf.lrned_scind) ? "s" : "c",
				fdb_1.bf.key_vid ,
				(fdb_1.bf.key_scind) ? "s" : "c",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
				fdb_0.bf.is_static,
				fdb_0.bf.mc_flag,
				(fdb_0.bf.mc_flag) ? 0 : ((fdb_0.bf.mcgid >> 7) & 0x1),
				fdb_0.bf.sa_permit  ,
				fdb_0.bf.da_permit  ,
				fdb_0.bf.aging_sts,
				 cmd_ret.bf.ext_status
				);
		}

		if (start == next)
			break;
		start = next;
	}

	return 0;
}

static cmd_tbl_t cmd_l2fe_sub[] = {
	U_BOOT_CMD_MKENT(ni_intf_cnt, 2, 1, do_ni_intf_counter, "", ""),
	U_BOOT_CMD_MKENT(drop_source_cnt, 1, 1, do_l2fe_drop_source_cnt, "", ""),
	U_BOOT_CMD_MKENT(pp_heada, 1, 1, do_l2fe_pp_heada, "", ""),
	U_BOOT_CMD_MKENT(pe_heada, 1, 1, do_l2fe_pe_heada, "", ""),
	U_BOOT_CMD_MKENT(fdb, 1, 1, do_l2fe_fdb, "", ""),
};

int do_l2fe(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'ni' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_l2fe_sub[0], ARRAY_SIZE(cmd_l2fe_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char l2fe_help_text[] =
	"ni_intf_cnt <read_clear>- Show NI to FE interface counter\n"
	"l2fe drop_source_cnt - Show L2FE drop source counter\n"
	"l2fe pp_heada - Show L2FE PP header_a\n"
	"l2fe pe_heada - Show L2FE PE header_a\n"
	"l2fe fdb - Show L2FE FDB table\n";
#endif

U_BOOT_CMD(
	l2fe, 5, 1, do_l2fe,
	"L2FE utility commands",
	l2fe_help_text
);

/******************************************
*                   L2TM commands
*******************************************/
int do_l2tm_bm_counter(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: %10u\n", "rx_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_PCNT));
	printf("%-20s: %10u\n", "tx_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_PCNT));
	printf("%-20s: %10u\n", "sb_drop_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_SB_DPCNT));
	printf("%-20s: %10u\n", "hdr_drop_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_HDR_DPCNT));
	printf("%-20s: %10u\n", "te_drop_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_TE_DPCNT));
	printf("%-20s: %10u\n", "err_drop_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_ERR_DPCNT));
	printf("%-20s: %10u\n", "no_buff_drop_cnt", CORTINA_CA_REG_READ(L2TM_L2TM_BM_NOBUF_DPCNT));

	return 0;
}

int do_l2tm_bm_status(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	L2TM_L2TM_BM_STS_t status;

	status.wrd = CORTINA_CA_REG_READ(L2TM_L2TM_BM_STS);

	printf("%-30s: %d\n", "fe_tm_valid", status.bf.fe_tm_vld);
	printf("%-30s: %d\n", "tm_fe_ready", status.bf.tm_fe_rdy);
	printf("%-30s: %d\n", "ni_tm_valid", status.bf.ni_tm_vld);
	printf("%-30s: %d\n", "tm_ni_ready", status.bf.tm_ni_rdy);
	printf("%-30s: %d\n", "tm_ni_valid", status.bf.tm_ni_vld);
	printf("%-30s: %d\n", "deep_q_if_fifo_empty", status.bf.dq_ff_empty);
	printf("%-30s: %d\n", "deep_q_pkt_fifo_empty", status.bf.dq_pack_ff_empty);
	printf("%-30s: %d\n", "fe_if_fifo_empty", status.bf.fe_ff_empty);
	printf("%-30s: %d\n", "pkt_fifo_empty", status.bf.pack_ff_empty);
	printf("%-30s: %d\n", "wr_data_fifo_empty", status.bf.wdat_ff_empty);
	printf("%-30s: %d\n", "en_q_buff_fifo_empty", status.bf.wcmd_ff_empty);
	printf("%-30s: %d\n", "rd_data_fifo_empty", status.bf.rdat_ff_empty);
	printf("%-30s: %d\n", "de_q_buff_fifo_empty", status.bf.rcmd_ff_empty);
	printf("%-30s: %d\n", "buff_recycling_fifo_empty", status.bf.rcyc_ff_empty);

	return 0;
}

int do_l2tm_bm_rx_fe_header(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "header_63_32", CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32));
	printf("%-20s: 0x%08x\n", "header_31_0", CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_63_32)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_FE_HEADER_31_0)>>3)&0x3f);

	return 0;
}

int do_l2tm_bm_rx_ni_header(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "header_63_32", CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32));
	printf("%-20s: 0x%08x\n", "header_31_0", CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_63_32)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_RX_NI_HEADER_31_0)>>3)&0x3f);

	return 0;
}

int do_l2tm_bm_tx_header(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("%-20s: 0x%08x\n", "header_63_32", CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32));
	printf("%-20s: 0x%08x\n", "header_31_0", CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0));
	printf("%-20s: 0x%08x\n", "cpu_flag", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>31)&1);
	printf("%-20s: 0x%08x\n", "deep_q", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>30)&1);
	printf("%-20s: 0x%08x\n", "policy_ena", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>16)&3);
	printf("%-20s: 0x%08x\n", "policy_group_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>27)&7);
	printf("%-20s: 0x%08x\n", "policy_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>18)&0x1ff);
	printf("%-20s: 0x%08x\n", "marked", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>15)&1);
	printf("%-20s: 0x%08x\n", "mirror", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>14)&1);
	printf("%-20s: 0x%08x\n", "no_drop", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>13)&1);
	printf("%-20s: 0x%08x\n", "rx_packet_type", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>11)&3);
	printf("%-20s: 0x%08x\n", "drop_code", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>8)&7);
	printf("%-20s: 0x%08x\n", "mc_group_id", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_63_32)>>0)&0xff);
	printf("%-20s: 0x%08x\n", "header_type", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0)>>30)&3);
	printf("%-20s: 0x%08x\n", "fe_bypass", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0)>>29)&1);
	printf("%-20s: 0x%08x\n", "packet_size", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0)>>15)&0x3fff);
	printf("%-20s: 0x%08x\n", "cos", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0) >> 0)&0x7);
	printf("%-20s: 0x%08x\n", "logic_spid", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0)>>9) &0x3f);
	printf("%-20s: 0x%08x\n", "logic_dpid", (CORTINA_CA_REG_READ(L2TM_L2TM_BM_TX_NI_HEADER_31_0)>>3)&0x3f);

	return 0;
}

static cmd_tbl_t cmd_l2tm_sub[] = {
	U_BOOT_CMD_MKENT(bm_cnt, 1, 1, do_l2tm_bm_counter, "", ""),
	U_BOOT_CMD_MKENT(bm_status, 1, 1, do_l2tm_bm_status, "", ""),
	U_BOOT_CMD_MKENT(bm_rx_fe_header, 1, 1, do_l2tm_bm_rx_fe_header, "", ""),
	U_BOOT_CMD_MKENT(bm_rx_ni_header, 1, 1, do_l2tm_bm_rx_ni_header, "", ""),
	U_BOOT_CMD_MKENT(bm_tx_header, 1, 1, do_l2tm_bm_tx_header, "", ""),
};

int do_l2tm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'ni' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_l2tm_sub[0], ARRAY_SIZE(cmd_l2tm_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char l2tm_help_text[] =
	"bm_cnt - Show L2TM buffer manger counter\n"
	"l2tm bm_status - Show L2TM buffer manger status\n"
	"l2tm bm_rx_fe_header - Show L2TM BM RX FE header\n"
	"l2tm bm_rx_ni_header - Show L2TM BM RX NI header\n"
	"l2tm bm_tx_header - Show L2TM BM TX NI header\n";
#endif

U_BOOT_CMD(
	l2tm, 5, 1, do_l2tm,
	"L2TM utility commands",
	l2tm_help_text
);

