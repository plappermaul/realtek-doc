/*-----------------------------------------------------
 Extraced from file_MEM_GLB.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int dram_type:4; //1
		unsigned int boot_sel:4; //0
		unsigned int ip_ref:1; //0
		unsigned int dp_ref:1; //0
		unsigned int eeprom_type:1; //1
		unsigned int d_signal:1; //0
		unsigned int flash_map0_dis:1; //0
		unsigned int flash_map1_dis:1; //1
		unsigned int mbz_0:3; //0
		unsigned int d_init_trig:1; //0
		unsigned int ocp1_frq_slower:1; //0
		unsigned int lx1_frq_slower:1; //0
		unsigned int lx2_frq_slower:1; //0
		unsigned int lx3_frq_slower:1; //0
		unsigned int ocp0_frq_slower:1; //0
		unsigned int ocp1_rbf_mask_en:1; //0
		unsigned int ocp0_rbf_mask_en:1; //0
		unsigned int ocp1_rbf_f_dis:1; //1
		unsigned int ocp0_rbf_f_dis:1; //1
		unsigned int sync_lx0_dram:1; //0
		unsigned int sync_lx1_dram:1; //0
		unsigned int sync_lx2_dram:1; //0
		unsigned int sync_lx3_dram:1; //0
		unsigned int sync_ocp0_dram:1; //0
	} f;
	unsigned int v;
} MCR_T;
#define MCRrv (*((regval)0xb8001000))
#define MCRdv (0x10200060)
#define RMOD_MCR(...) rset(MCR, MCRrv, __VA_ARGS__)
#define RIZS_MCR(...) rset(MCR, 0, __VA_ARGS__)
#define RFLD_MCR(fld) (*((const volatile MCR_T *)0xb8001000)).f.fld

typedef union {
	struct {
		unsigned int no_use31:2; //0
		unsigned int bankcnt:2; //1
		unsigned int no_use27:2; //0
		unsigned int dbuswid:2; //1
		unsigned int rowcnt:4; //0
		unsigned int colcnt:4; //0
		unsigned int dchipsel:1; //1
		unsigned int fast_rx:1; //0
		unsigned int bstref:1; //0
        unsigned int prl_bank_act_en:1; //0
		unsigned int no_use12:12; //0
	} f;
	unsigned int v;
} DCR_T;
#define DCRrv (*((regval)0xb8001004))
#define DCRdv (0x11008000)
#define RMOD_DCR(...) rset(DCR, DCRrv, __VA_ARGS__)
#define RIZS_DCR(...) rset(DCR, 0, __VA_ARGS__)
#define RFLD_DCR(fld) (*((const volatile DCR_T *)0xb8001004)).f.fld

typedef union {
	struct {
		unsigned int t_cas:4; //2
		unsigned int t_wr:4; //3
		unsigned int t_cwl:4; //0
		unsigned int t_rtp:4; //15
		unsigned int t_wtr:4; //15
		unsigned int t_refi:4; //0
		unsigned int t_refi_unit:4; //4
		unsigned int t_cas_phy:4; //2
	} f;
	unsigned int v;
} DTR0_T;
#define DTR0rv (*((regval)0xb8001008))
#define DTR0dv (0x230ff042)
#define RMOD_DTR0(...) rset(DTR0, DTR0rv, __VA_ARGS__)
#define RIZS_DTR0(...) rset(DTR0, 0, __VA_ARGS__)
#define RFLD_DTR0(fld) (*((const volatile DTR0_T *)0xb8001008)).f.fld

typedef union {
	struct {
		unsigned int no_use31:3; //0
		unsigned int t_rp:5; //31
		unsigned int no_use23:3; //0
		unsigned int t_rcd:5; //31
		unsigned int no_use15:3; //0
		unsigned int t_rrd:5; //31
		unsigned int no_use07:3; //0
		unsigned int t_fawg:5; //31
	} f;
	unsigned int v;
} DTR1_T;
#define DTR1rv (*((regval)0xb800100c))
#define DTR1dv (0x1f1f1f1f)
#define RMOD_DTR1(...) rset(DTR1, DTR1rv, __VA_ARGS__)
#define RIZS_DTR1(...) rset(DTR1, 0, __VA_ARGS__)
#define RFLD_DTR1(fld) (*((const volatile DTR1_T *)0xb800100c)).f.fld

typedef union {
	struct {
		unsigned int no_use31:4; //0
		unsigned int t_rfc:8; //255
		unsigned int no_use19:2; //0
		unsigned int t_ras:6; //63
		unsigned int no_use11:12; //0
	} f;
	unsigned int v;
} DTR2_T;
#define DTR2rv (*((regval)0xb8001010))
#define DTR2dv (0x0ff3f000)
#define RMOD_DTR2(...) rset(DTR2, DTR2rv, __VA_ARGS__)
#define RIZS_DTR2(...) rset(DTR2, 0, __VA_ARGS__)
#define RFLD_DTR2(fld) (*((const volatile DTR2_T *)0xb8001010)).f.fld

typedef union {
	struct {
		unsigned int dtr_up_busy_mrs_busy:1; //0
		unsigned int mbz_0:5; //0
		unsigned int en_wr_leveling:1; //0
		unsigned int dis_dram_ref:1; //0
		unsigned int no_use23:3; //0
		unsigned int mr_mode_en:1; //0
		unsigned int no_use19:2; //0
		unsigned int mr_mode:2; //0
		unsigned int no_use15:2; //0
		unsigned int mr_data:14; //0
	} f;
	unsigned int v;
} DMCR_T;
#define DMCRrv (*((regval)0xb800101c))
#define DMCRdv (0x00000000)
#define RMOD_DMCR(...) rset(DMCR, DMCRrv, __VA_ARGS__)
#define RIZS_DMCR(...) rset(DMCR, 0, __VA_ARGS__)
#define RFLD_DMCR(fld) (*((const volatile DMCR_T *)0xb800101c)).f.fld

typedef union {
	struct {
		unsigned int ocp_req:32; //2863311530
	} f;
	unsigned int v;
} GIAR0_T;
#define GIAR0rv (*((regval)0xb8001020))
#define GIAR0dv (0xaaaaaaaa)
#define RMOD_GIAR0(...) rset(GIAR0, GIAR0rv, __VA_ARGS__)
#define RIZS_GIAR0(...) rset(GIAR0, 0, __VA_ARGS__)
#define RFLD_GIAR0(fld) (*((const volatile GIAR0_T *)0xb8001020)).f.fld

typedef union {
	struct {
		unsigned int lx_req:32; //1431655765
	} f;
	unsigned int v;
} GIAR1_T;
#define GIAR1rv (*((regval)0xb8001024))
#define GIAR1dv (0x55555555)
#define RMOD_GIAR1(...) rset(GIAR1, GIAR1rv, __VA_ARGS__)
#define RIZS_GIAR1(...) rset(GIAR1, 0, __VA_ARGS__)
#define RFLD_GIAR1(fld) (*((const volatile GIAR1_T *)0xb8001024)).f.fld

typedef union {
	struct {
		unsigned int lx0_req:32; //3991793133
	} f;
	unsigned int v;
} LXIAR0_T;
#define LXIAR0rv (*((regval)0xb8001028))
#define LXIAR0dv (0xedededed)
#define RMOD_LXIAR0(...) rset(LXIAR0, LXIAR0rv, __VA_ARGS__)
#define RIZS_LXIAR0(...) rset(LXIAR0, 0, __VA_ARGS__)
#define RFLD_LXIAR0(fld) (*((const volatile LXIAR0_T *)0xb8001028)).f.fld

typedef union {
	struct {
		unsigned int lx1_req:32; //3082270647
	} f;
	unsigned int v;
} LXIAR1_T;
#define LXIAR1rv (*((regval)0xb800102c))
#define LXIAR1dv (0xb7b7b7b7)
#define RMOD_LXIAR1(...) rset(LXIAR1, LXIAR1rv, __VA_ARGS__)
#define RIZS_LXIAR1(...) rset(LXIAR1, 0, __VA_ARGS__)
#define RFLD_LXIAR1(fld) (*((const volatile LXIAR1_T *)0xb800102c)).f.fld

typedef union {
	struct {
		unsigned int lx2_req:32; //3739147998
	} f;
	unsigned int v;
} LXIAR2_T;
#define LXIAR2rv (*((regval)0xb8001030))
#define LXIAR2dv (0xdededede)
#define RMOD_LXIAR2(...) rset(LXIAR2, LXIAR2rv, __VA_ARGS__)
#define RIZS_LXIAR2(...) rset(LXIAR2, 0, __VA_ARGS__)
#define RFLD_LXIAR2(fld) (*((const volatile LXIAR2_T *)0xb8001030)).f.fld

typedef union {
	struct {
		unsigned int lx3_req:32; //2071690107
	} f;
	unsigned int v;
} LXIAR3_T;
#define LXIAR3rv (*((regval)0xb8001034))
#define LXIAR3dv (0x7b7b7b7b)
#define RMOD_LXIAR3(...) rset(LXIAR3, LXIAR3rv, __VA_ARGS__)
#define RIZS_LXIAR3(...) rset(LXIAR3, 0, __VA_ARGS__)
#define RFLD_LXIAR3(fld) (*((const volatile LXIAR3_T *)0xb8001034)).f.fld

typedef union {
	struct {
		unsigned int flush_ocp_cmd:1; //0
		unsigned int dram_cmd_going:1; //1
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} MSRR_T;
#define MSRRrv (*((regval)0xb8001038))
#define MSRRdv (0x40000000)
#define RMOD_MSRR(...) rset(MSRR, MSRRrv, __VA_ARGS__)
#define RIZS_MSRR(...) rset(MSRR, 0, __VA_ARGS__)
#define RFLD_MSRR(fld) (*((const volatile MSRR_T *)0xb8001038)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int dbg_mem_ctrl_sel:4; //0
	} f;
	unsigned int v;
} MCDSR_T;
#define MCDSRrv (*((regval)0xb800103c))
#define MCDSRdv (0x00000000)
#define RMOD_MCDSR(...) rset(MCDSR, MCDSRrv, __VA_ARGS__)
#define RIZS_MCDSR(...) rset(MCDSR, 0, __VA_ARGS__)
#define RFLD_MCDSR(fld) (*((const volatile MCDSR_T *)0xb800103c)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:2; //0
		unsigned int pm_mode:2; //0
		unsigned int t_cke:4; //15
		unsigned int mbz_1:2; //0
		unsigned int trsd:10; //1023
		unsigned int mbz_2:2; //0
		unsigned int t_xsref:10; //1023
	} f;
	unsigned int v;
} MPMR0_T;
#define MPMR0rv (*((regval)0xb8001040))
#define MPMR0dv (0x0f3ff3ff)
#define RMOD_MPMR0(...) rset(MPMR0, MPMR0rv, __VA_ARGS__)
#define RIZS_MPMR0(...) rset(MPMR0, 0, __VA_ARGS__)
#define RFLD_MPMR0(fld) (*((const volatile MPMR0_T *)0xb8001040)).f.fld

typedef union {
	struct {
		unsigned int t_xard:4; //15
		unsigned int t_axpd:4; //15
		unsigned int mbz_0:24; //0
	} f;
	unsigned int v;
} MPMR1_T;
#define MPMR1rv (*((regval)0xb8001044))
#define MPMR1dv (0xff000000)
#define RMOD_MPMR1(...) rset(MPMR1, MPMR1rv, __VA_ARGS__)
#define RIZS_MPMR1(...) rset(MPMR1, 0, __VA_ARGS__)
#define RFLD_MPMR1(fld) (*((const volatile MPMR1_T *)0xb8001044)).f.fld

typedef union {
	struct {
		unsigned int dqs0_en_hclk:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqs0_en_tap:5; //0
		unsigned int dqs1_en_hclk:1; //0
		unsigned int mbz_1:2; //0
		unsigned int dqs1_en_tap:5; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} DIDER_T;
#define DIDERrv (*((regval)0xb8001050))
#define DIDERdv (0x00000000)
#define RMOD_DIDER(...) rset(DIDER, DIDERrv, __VA_ARGS__)
#define RIZS_DIDER(...) rset(DIDER, 0, __VA_ARGS__)
#define RFLD_DIDER(fld) (*((const volatile DIDER_T *)0xb8001050)).f.fld

typedef union {
	struct {
		unsigned int dqs0_pupd_det_en:1;		//0
		unsigned int mbz_0:6; 					//0
		unsigned int dqs0_pupd_det_delay:5; 	//0b01111
		unsigned int mbz_1:3; 					//0
		unsigned int dqs0_pupd_mask_delay:5; 	//0b01111
		unsigned int dqs0_pupd_start_det:4; 	//0b0010
		unsigned int mbz_2:8; 					//0
	} f;
	unsigned int v;
} DDPDR0_T;
#define DDPDR0rv (*((regval)0xb8001054))
#define DDPDR0dv (0x00000000)
#define RMOD_DDPDR0(...) rset(DDPDR0, DDPDR0rv, __VA_ARGS__)
#define RIZS_DDPDR0(...) rset(DDPDR0, 0, __VA_ARGS__)
#define RFLD_DDPDR0(fld) (*((const volatile DDPDR0_T *)0xb8001054)).f.fld

typedef union {
	struct {
		unsigned int dqs1_pupd_det_en:1;		//0
		unsigned int mbz_0:6; 					//0
		unsigned int dqs1_pupd_det_delay:5; 	//0b01111
		unsigned int mbz_1:3; 					//0
		unsigned int dqs1_pupd_mask_delay:5; 	//0b01111
		unsigned int dqs1_pupd_start_det:4; 	//0b0010
		unsigned int mbz_2:8; 					//0
	} f;
	unsigned int v;
} DDPDR1_T;
#define DDPDR1rv (*((regval)0xb8001058))
#define DDPDR1dv (0x00000000)
#define RMOD_DDPDR1(...) rset(DDPDR1, DDPDR1rv, __VA_ARGS__)
#define RIZS_DDPDR1(...) rset(DDPDR1, 0, __VA_ARGS__)
#define RFLD_DDPDR1(fld) (*((const volatile DDPDR1_T *)0xb8001058)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:18;					//0
		unsigned int dqs1_wr_phy_ph_adj:2;		//0b00
		unsigned int mbz_1:2;					//0
		unsigned int dqs0_wr_phy_ph_adj:2;		//0b00
		unsigned int mbz_2:8;					//0
	} f;
	unsigned int v;
} DCPER_T;
#define DCPERrv (*((regval)0xb8001064))
#define DCPERdv (0x00000000)
#define RMOD_DCPER(...) rset(DCPER, DCPERrv, __VA_ARGS__)
#define RIZS_DCPER(...) rset(DCPER, 0, __VA_ARGS__)
#define RFLD_DCPER(fld) (*((const volatile DCPER_T *)0xb8001064)).f.fld

typedef union {
	struct {
		unsigned int ocd_dt1:16; //0
		unsigned int ocd_dt0:16; //0
	} f;
	unsigned int v;
} D2ODR0_T;
#define D2ODR0rv (*((regval)0xb8001074))
#define D2ODR0dv (0x00000000)
#define RMOD_D2ODR0(...) rset(D2ODR0, D2ODR0rv, __VA_ARGS__)
#define RIZS_D2ODR0(...) rset(D2ODR0, 0, __VA_ARGS__)
#define RFLD_D2ODR0(fld) (*((const volatile D2ODR0_T *)0xb8001074)).f.fld

typedef union {
	struct {
		unsigned int ocd_dt3:16; //0
		unsigned int ocd_dt2:16; //0
	} f;
	unsigned int v;
} D2ODR1_T;
#define D2ODR1rv (*((regval)0xb8001078))
#define D2ODR1dv (0x00000000)
#define RMOD_D2ODR1(...) rset(D2ODR1, D2ODR1rv, __VA_ARGS__)
#define RIZS_D2ODR1(...) rset(D2ODR1, 0, __VA_ARGS__)
#define RFLD_D2ODR1(fld) (*((const volatile D2ODR1_T *)0xb8001078)).f.fld

typedef union {
	struct {
		unsigned int odt_always_on:1; //0
		unsigned int te_alwyas_on:1; //0
		unsigned int mbz_0:30; //0
	} f;
	unsigned int v;
} D23OSCR_T;
#define D23OSCRrv (*((regval)0xb800107c))
#define D23OSCRdv (0x00000000)
#define RMOD_D23OSCR(...) rset(D23OSCR, D23OSCRrv, __VA_ARGS__)
#define RIZS_D23OSCR(...) rset(D23OSCR, 0, __VA_ARGS__)
#define RFLD_D23OSCR(fld) (*((const volatile D23OSCR_T *)0xb800107c)).f.fld

typedef union {
	struct {
		unsigned int zq_long_tri:1; //0
		unsigned int zq_short_en:1; //0
		unsigned int mbz_0:7; //0
		unsigned int t_zqcs:7; //127
		unsigned int mbz_1:16; //0
	} f;
	unsigned int v;
} D3ZQCCR_T;
#define D3ZQCCRrv (*((regval)0xb8001080))
#define D3ZQCCRdv (0x007f0000)
#define RMOD_D3ZQCCR(...) rset(D3ZQCCR, D3ZQCCRrv, __VA_ARGS__)
#define RIZS_D3ZQCCR(...) rset(D3ZQCCR, 0, __VA_ARGS__)
#define RFLD_D3ZQCCR(fld) (*((const volatile D3ZQCCR_T *)0xb8001080)).f.fld

typedef union {
	struct {
		unsigned int zq_pad_force_on:1; //0
		unsigned int mbz_0:2; //0
		unsigned int zctrl_clk_sel:1; //1
		unsigned int trim_mode:4; //15
		unsigned int mbz_1:24; //0
	} f;
	unsigned int v;
} DDZQPR_T;
#define DDZQPRrv (*((regval)0xb8001090))
#define DDZQPRdv (0x1f000000)
#define RMOD_DDZQPR(...) rset(DDZQPR, DDZQPRrv, __VA_ARGS__)
#define RIZS_DDZQPR(...) rset(DDZQPR, 0, __VA_ARGS__)
#define RFLD_DDZQPR(fld) (*((const volatile DDZQPR_T *)0xb8001090)).f.fld

typedef union {
	struct {
		unsigned int zctrl_start:1; //0
		unsigned int zq_pad_period_cali_en:1; //0
		unsigned int mbz_0:9; //0
		unsigned int zprog:21; //0
	} f;
	unsigned int v;
} DDZQPCR_T;
#define DDZQPCRrv (*((regval)0xb8001094))
#define DDZQPCRdv (0x00000000)
#define RMOD_DDZQPCR(...) rset(DDZQPCR, DDZQPCRrv, __VA_ARGS__)
#define RIZS_DDZQPCR(...) rset(DDZQPCR, 0, __VA_ARGS__)
#define RFLD_DDZQPCR(fld) (*((const volatile DDZQPCR_T *)0xb8001094)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:1; //0
		unsigned int zctrl_status:31; //0
	} f;
	unsigned int v;
} DDZQPSR_T;
#define DDZQPSRrv (*((regval)0xb8001098))
#define DDZQPSRdv (0x00000000)
#define RMOD_DDZQPSR(...) rset(DDZQPSR, DDZQPSRrv, __VA_ARGS__)
#define RIZS_DDZQPSR(...) rset(DDZQPSR, 0, __VA_ARGS__)
#define RFLD_DDZQPSR(fld) (*((const volatile DDZQPSR_T *)0xb8001098)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int err_flag:4; //0
	} f;
	unsigned int v;
} MCESR0_T;
#define MCESR0rv (*((regval)0xb80010a0))
#define MCESR0dv (0x00000000)
#define RMOD_MCESR0(...) rset(MCESR0, MCESR0rv, __VA_ARGS__)
#define RIZS_MCESR0(...) rset(MCESR0, 0, __VA_ARGS__)
#define RFLD_MCESR0(fld) (*((const volatile MCESR0_T *)0xb80010a0)).f.fld

typedef union {
	struct {
		unsigned int err1_dq15r_flag:1; //0
		unsigned int err1_dq14r_flag:1; //0
		unsigned int err1_dq13r_flag:1; //0
		unsigned int err1_dq12r_flag:1; //0
		unsigned int err1_dq11r_flag:1; //0
		unsigned int err1_dq10r_flag:1; //0
		unsigned int err1_dq9r_flag:1; //0
		unsigned int err1_dq8r_flag:1; //0
		unsigned int err1_dq7r_flag:1; //0
		unsigned int err1_dq6r_flag:1; //0
		unsigned int err1_dq5r_flag:1; //0
		unsigned int err1_dq4r_flag:1; //0
		unsigned int err1_dq3r_flag:1; //0
		unsigned int err1_dq2r_flag:1; //0
		unsigned int err1_dq1r_flag:1; //0
		unsigned int err1_dq0r_flag:1; //0
		unsigned int err1_dq15f_flag:1; //0
		unsigned int err1_dq14f_flag:1; //0
		unsigned int err1_dq13f_flag:1; //0
		unsigned int err1_dq12f_flag:1; //0
		unsigned int err1_dq11f_flag:1; //0
		unsigned int err1_dq10f_flag:1; //0
		unsigned int err1_dq9f_flag:1; //0
		unsigned int err1_dq8f_flag:1; //0
		unsigned int err1_dq7f_flag:1; //0
		unsigned int err1_dq6f_flag:1; //0
		unsigned int err1_dq5f_flag:1; //0
		unsigned int err1_dq4f_flag:1; //0
		unsigned int err1_dq3f_flag:1; //0
		unsigned int err1_dq2f_flag:1; //0
		unsigned int err1_dq1f_flag:1; //0
		unsigned int err1_dq0f_flag:1; //0
	} f;
	unsigned int v;
} MCESR1_T;
#define MCESR1rv (*((regval)0xb80010a4))
#define MCESR1dv (0x00000000)
#define RMOD_MCESR1(...) rset(MCESR1, MCESR1rv, __VA_ARGS__)
#define RIZS_MCESR1(...) rset(MCESR1, 0, __VA_ARGS__)
#define RFLD_MCESR1(fld) (*((const volatile MCESR1_T *)0xb80010a4)).f.fld

typedef union {
	struct {
		unsigned int err2_dq15r_flag:1; //0
		unsigned int err2_dq14r_flag:1; //0
		unsigned int err2_dq13r_flag:1; //0
		unsigned int err2_dq12r_flag:1; //0
		unsigned int err2_dq11r_flag:1; //0
		unsigned int err2_dq10r_flag:1; //0
		unsigned int err2_dq9r_flag:1; //0
		unsigned int err2_dq8r_flag:1; //0
		unsigned int err2_dq7r_flag:1; //0
		unsigned int err2_dq6r_flag:1; //0
		unsigned int err2_dq5r_flag:1; //0
		unsigned int err2_dq4r_flag:1; //0
		unsigned int err2_dq3r_flag:1; //0
		unsigned int err2_dq2r_flag:1; //0
		unsigned int err2_dq1r_flag:1; //0
		unsigned int err2_dq0r_flag:1; //0
		unsigned int err2_dq15f_flag:1; //0
		unsigned int err2_dq14f_flag:1; //0
		unsigned int err2_dq13f_flag:1; //0
		unsigned int err2_dq12f_flag:1; //0
		unsigned int err2_dq11f_flag:1; //0
		unsigned int err2_dq10f_flag:1; //0
		unsigned int err2_dq9f_flag:1; //0
		unsigned int err2_dq8f_flag:1; //0
		unsigned int err2_dq7f_flag:1; //0
		unsigned int err2_dq6f_flag:1; //0
		unsigned int err2_dq5f_flag:1; //0
		unsigned int err2_dq4f_flag:1; //0
		unsigned int err2_dq3f_flag:1; //0
		unsigned int err2_dq2f_flag:1; //0
		unsigned int err2_dq1f_flag:1; //0
		unsigned int err2_dq0f_flag:1; //0
	} f;
	unsigned int v;
} MCESR2_T;
#define MCESR2rv (*((regval)0xb80010a8))
#define MCESR2dv (0x00000000)
#define RMOD_MCESR2(...) rset(MCESR2, MCESR2rv, __VA_ARGS__)
#define RIZS_MCESR2(...) rset(MCESR2, 0, __VA_ARGS__)
#define RFLD_MCESR2(fld) (*((const volatile MCESR2_T *)0xb80010a8)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:16; //0
		unsigned int mbz_1:2; //0
		unsigned int lx3_jt_num:2; //0
		unsigned int mbz_2:2; //0
		unsigned int lx2_jt_num:2; //0
		unsigned int mbz_3:2; //0
		unsigned int lx1_jt_num:2; //0
		unsigned int mbz_4:2; //0
		unsigned int lx0_jt_num:2; //0
	} f;
	unsigned int v;
} MCLJTCR_T;
#define MCLJTCRrv (*((regval)0xb80010b0))
#define MCLJTCRdv (0x00000000)
#define RMOD_MCLJTCR(...) rset(MCLJTCR, MCLJTCRrv, __VA_ARGS__)
#define RIZS_MCLJTCR(...) rset(MCLJTCR, 0, __VA_ARGS__)
#define RFLD_MCLJTCR(fld) (*((const volatile MCLJTCR_T *)0xb80010b0)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:19; //0
		unsigned int lx3_bpre_dis:1; //0
		unsigned int mbz_1:3; //0
		unsigned int lx2_bpre_dis:1; //0
		unsigned int mbz_2:3; //0
		unsigned int lx1_bpre_dis:1; //0
		unsigned int mbz_3:3; //0
		unsigned int lx0_bpre_dis:1; //0
	} f;
	unsigned int v;
} MCLPBRCR_T;
#define MCLPBRCRrv (*((regval)0xb80010b4))
#define MCLPBRCRdv (0x00000000)
#define RMOD_MCLPBRCR(...) rset(MCLPBRCR, MCLPBRCRrv, __VA_ARGS__)
#define RIZS_MCLPBRCR(...) rset(MCLPBRCR, 0, __VA_ARGS__)
#define RFLD_MCLPBRCR(fld) (*((const volatile MCLPBRCR_T *)0xb80010b4)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int ocp0_raw_dis:1; //0
		unsigned int mbz_1:3; //0
		unsigned int ocp1_raw_dis:1; //0
		unsigned int mbz_2:3; //0
		unsigned int lx0_raw_dis:1; //0
		unsigned int mbz_3:3; //0
		unsigned int lx1_raw_dis:1; //0
		unsigned int mbz_4:3; //0
		unsigned int lx2_raw_dis:1; //0
		unsigned int mbz_5:3; //0
		unsigned int lx3_raw_dis:1; //0
		unsigned int mbz_6:3; //0
		unsigned int ocp0_ocp1_raw_en:1; //0
		unsigned int mbz_7:4; //0
	} f;
	unsigned int v;
} MCERAWCR_T;
#define MCERAWCRrv (*((regval)0xb80010c0))
#define MCERAWCRdv (0x00000000)
#define RMOD_MCERAWCR(...) rset(MCERAWCR, MCERAWCRrv, __VA_ARGS__)
#define RIZS_MCERAWCR(...) rset(MCERAWCR, 0, __VA_ARGS__)
#define RFLD_MCERAWCR(fld) (*((const volatile MCERAWCR_T *)0xb80010c0)).f.fld

/*-----------------------------------------------------
 Extraced from file_MEM_VER.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mcv_year:8; //20
		unsigned int mcv_date:16; //532
		unsigned int mcv_hour:8; //0
	} f;
	unsigned int v;
} MCVR_T;
#define MCVRrv (*((regval)0xb80010f8))
#define MCVRdv (0x14021400)
#define RMOD_MCVR(...) rset(MCVR, MCVRrv, __VA_ARGS__)
#define RIZS_MCVR(...) rset(MCVR, 0, __VA_ARGS__)
#define RFLD_MCVR(fld) (*((const volatile MCVR_T *)0xb80010f8)).f.fld

typedef union {
	struct {
		unsigned int soc_plat_num:16; //1593
		unsigned int soc_plat_cut_num:4; //1
		unsigned int mbz_0:12; //0
	} f;
	unsigned int v;
} SOCPNR_T;
#define SOCPNRrv (*((regval)0xb80010fc))
#define SOCPNRdv (0x06391000)
#define RMOD_SOCPNR(...) rset(SOCPNR, SOCPNRrv, __VA_ARGS__)
#define RIZS_SOCPNR(...) rset(SOCPNR, 0, __VA_ARGS__)
#define RFLD_SOCPNR(fld) (*((const volatile SOCPNR_T *)0xb80010fc)).f.fld

/*-----------------------------------------------------
 Extraced from file_MEM_UNMAP.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int no_use31:1; //0
		unsigned int addr:23; //0
		unsigned int no_use07:7; //0
		unsigned int unmap_en:1; //0
	} f;
	unsigned int v;
} UMSAR0_T;
#define UMSAR0rv (*((regval)0xb8001300))
#define UMSAR0dv (0x00000000)
#define RMOD_UMSAR0(...) rset(UMSAR0, UMSAR0rv, __VA_ARGS__)
#define RIZS_UMSAR0(...) rset(UMSAR0, 0, __VA_ARGS__)
#define RFLD_UMSAR0(fld) (*((const volatile UMSAR0_T *)0xb8001300)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //8
	} f;
	unsigned int v;
} UMSSR0_T;
#define UMSSR0rv (*((regval)0xb8001304))
#define UMSSR0dv (0x00000008)
#define RMOD_UMSSR0(...) rset(UMSSR0, UMSSR0rv, __VA_ARGS__)
#define RIZS_UMSSR0(...) rset(UMSSR0, 0, __VA_ARGS__)
#define RFLD_UMSSR0(fld) (*((const volatile UMSSR0_T *)0xb8001304)).f.fld

typedef union {
	struct {
		unsigned int no_use31:1; //0
		unsigned int addr:23; //0
		unsigned int no_use07:7; //0
		unsigned int unmap_en:1; //0
	} f;
	unsigned int v;
} UMSAR1_T;
#define UMSAR1rv (*((regval)0xb8001310))
#define UMSAR1dv (0x00000000)
#define RMOD_UMSAR1(...) rset(UMSAR1, UMSAR1rv, __VA_ARGS__)
#define RIZS_UMSAR1(...) rset(UMSAR1, 0, __VA_ARGS__)
#define RFLD_UMSAR1(fld) (*((const volatile UMSAR1_T *)0xb8001310)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //8
	} f;
	unsigned int v;
} UMSSR1_T;
#define UMSSR1rv (*((regval)0xb8001314))
#define UMSSR1dv (0x00000008)
#define RMOD_UMSSR1(...) rset(UMSSR1, UMSSR1rv, __VA_ARGS__)
#define RIZS_UMSSR1(...) rset(UMSSR1, 0, __VA_ARGS__)
#define RFLD_UMSSR1(fld) (*((const volatile UMSSR1_T *)0xb8001314)).f.fld

typedef union {
	struct {
		unsigned int no_use31:1; //0
		unsigned int addr:23; //0
		unsigned int no_use07:7; //0
		unsigned int unmap_en:1; //0
	} f;
	unsigned int v;
} UMSAR2_T;
#define UMSAR2rv (*((regval)0xb8001320))
#define UMSAR2dv (0x00000000)
#define RMOD_UMSAR2(...) rset(UMSAR2, UMSAR2rv, __VA_ARGS__)
#define RIZS_UMSAR2(...) rset(UMSAR2, 0, __VA_ARGS__)
#define RFLD_UMSAR2(fld) (*((const volatile UMSAR2_T *)0xb8001320)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //8
	} f;
	unsigned int v;
} UMSSR2_T;
#define UMSSR2rv (*((regval)0xb8001324))
#define UMSSR2dv (0x00000008)
#define RMOD_UMSSR2(...) rset(UMSSR2, UMSSR2rv, __VA_ARGS__)
#define RIZS_UMSSR2(...) rset(UMSSR2, 0, __VA_ARGS__)
#define RFLD_UMSSR2(fld) (*((const volatile UMSSR2_T *)0xb8001324)).f.fld

typedef union {
	struct {
		unsigned int no_use31:1; //0
		unsigned int addr:23; //0
		unsigned int no_use07:7; //0
		unsigned int enunmap:1; //0
	} f;
	unsigned int v;
} UMSAR3_T;
#define UMSAR3rv (*((regval)0xb8001330))
#define UMSAR3dv (0x00000000)
#define RMOD_UMSAR3(...) rset(UMSAR3, UMSAR3rv, __VA_ARGS__)
#define RIZS_UMSAR3(...) rset(UMSAR3, 0, __VA_ARGS__)
#define RFLD_UMSAR3(fld) (*((const volatile UMSAR3_T *)0xb8001330)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int size:4; //8
	} f;
	unsigned int v;
} UMSSR3_T;
#define UMSSR3rv (*((regval)0xb8001334))
#define UMSSR3dv (0x00000008)
#define RMOD_UMSSR3(...) rset(UMSSR3, UMSSR3rv, __VA_ARGS__)
#define RIZS_UMSSR3(...) rset(UMSSR3, 0, __VA_ARGS__)
#define RFLD_UMSSR3(fld) (*((const volatile UMSSR3_T *)0xb8001334)).f.fld

/*-----------------------------------------------------
 Extraced from file_MEM_DDR_PHY.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int ac_mode:1; //1
		unsigned int dqs_se:1; //0
		unsigned int mbz_0:9; //0
		unsigned int dqs0_group_tap:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqs1_group_tap:5; //0
		unsigned int mbz_2:2; //0
		unsigned int ac_dyn_bptr_clr_en:1; //0
		unsigned int ac_bptr_clear:1; //0
		unsigned int ac_debug_sel:4; //0
	} f;
	unsigned int v;
} DACCR_T;
#define DACCRrv (*((regval)0xb8001500))
#define DACCRdv (0x80000000)
#define RMOD_DACCR(...) rset(DACCR, DACCRrv, __VA_ARGS__)
#define RIZS_DACCR(...) rset(DACCR, 0, __VA_ARGS__)
#define RFLD_DACCR(fld) (*((const volatile DACCR_T *)0xb8001500)).f.fld

typedef union {
	struct {
		unsigned int ac_silen_period_en:1; //0
		unsigned int ac_silen_gen_en:1; //0
		unsigned int mbz_0:9; //0
		unsigned int ac_silen_trig:1; //0
		unsigned int ac_silen_period_unit:4; //0
		unsigned int ac_silen_period:8; //0
		unsigned int ac_silen_len:8; //0
	} f;
	unsigned int v;
} DACSPCR_T;
#define DACSPCRrv (*((regval)0xb8001504))
#define DACSPCRdv (0x00000000)
#define RMOD_DACSPCR(...) rset(DACSPCR, DACSPCRrv, __VA_ARGS__)
#define RIZS_DACSPCR(...) rset(DACSPCR, 0, __VA_ARGS__)
#define RFLD_DACSPCR(fld) (*((const volatile DACSPCR_T *)0xb8001504)).f.fld

typedef union {
	struct {
		unsigned int ac_silen_addr:32; //0
	} f;
	unsigned int v;
} DACSPAR_T;
#define DACSPARrv (*((regval)0xb8001508))
#define DACSPARdv (0x00000000)
#define RMOD_DACSPAR(...) rset(DACSPAR, DACSPARrv, __VA_ARGS__)
#define RIZS_DACSPAR(...) rset(DACSPAR, 0, __VA_ARGS__)
#define RFLD_DACSPAR(fld) (*((const volatile DACSPAR_T *)0xb8001508)).f.fld

typedef union {
	struct {
		unsigned int ac_sps_dq15r:1; //0
		unsigned int ac_sps_dq14r:1; //0
		unsigned int ac_sps_dq13r:1; //0
		unsigned int ac_sps_dq12r:1; //0
		unsigned int ac_sps_dq11r:1; //0
		unsigned int ac_sps_dq10r:1; //0
		unsigned int ac_sps_dq9r:1; //0
		unsigned int ac_sps_dq8r:1; //0
		unsigned int ac_sps_dq7r:1; //0
		unsigned int ac_sps_dq6r:1; //0
		unsigned int ac_sps_dq5r:1; //0
		unsigned int ac_sps_dq4r:1; //0
		unsigned int ac_sps_dq3r:1; //0
		unsigned int ac_sps_dq2r:1; //0
		unsigned int ac_sps_dq1r:1; //0
		unsigned int ac_sps_dq0r:1; //0
		unsigned int ac_sps_dq15f:1; //0
		unsigned int ac_sps_dq14f:1; //0
		unsigned int ac_sps_dq13f:1; //0
		unsigned int ac_sps_dq12f:1; //0
		unsigned int ac_sps_dq11f:1; //0
		unsigned int ac_sps_dq10f:1; //0
		unsigned int ac_sps_dq9f:1; //0
		unsigned int ac_sps_dq8f:1; //0
		unsigned int ac_sps_dq7f:1; //0
		unsigned int ac_sps_dq6f:1; //0
		unsigned int ac_sps_dq5f:1; //0
		unsigned int ac_sps_dq4f:1; //0
		unsigned int ac_sps_dq3f:1; //0
		unsigned int ac_sps_dq2f:1; //0
		unsigned int ac_sps_dq1f:1; //0
		unsigned int ac_sps_dq0f:1; //0
	} f;
	unsigned int v;
} DACSPSR_T;
#define DACSPSRrv (*((regval)0xb800150c))
#define DACSPSRdv (0x00000000)
#define RMOD_DACSPSR(...) rset(DACSPSR, DACSPSRrv, __VA_ARGS__)
#define RIZS_DACSPSR(...) rset(DACSPSR, 0, __VA_ARGS__)
#define RFLD_DACSPSR(fld) (*((const volatile DACSPSR_T *)0xb800150c)).f.fld

typedef union {
	struct {
		unsigned int dqn_r_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int dqn_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqn_r_ac_max_tap:5; //31
		unsigned int mbz_2:3; //0
		unsigned int dqn_r_ac_cur_tap:5; //15
		unsigned int mbz_3:3; //0
		unsigned int dqn_r_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ_IDX_RR_T;
#define DACDQ_IDX_RRrv (*((regval)0xb8001510))
#define DACDQ_IDX_RRdv (0x001f0f00)
#define RMOD_DACDQ_IDX_RR(...) rset(DACDQ_IDX_RR, DACDQ_IDX_RRrv, __VA_ARGS__)
#define RIZS_DACDQ_IDX_RR(...) rset(DACDQ_IDX_RR, 0, __VA_ARGS__)
#define RFLD_DACDQ_IDX_RR(fld) (*((const volatile DACDQ_IDX_RR_T *)0xb8001510)).f.fld

typedef union {
	struct {
		unsigned int dqn_f_ac_en:1; //0
		unsigned int mbz_0:2; //0
		unsigned int mbz_1:5; //0
		unsigned int mbz_2:3; //0
		unsigned int dqn_f_ac_max_tap:5; //31
		unsigned int mbz_3:3; //0
		unsigned int dqn_f_ac_cur_tap:5; //15
		unsigned int mbz_4:3; //0
		unsigned int dqn_f_ac_min_tap:5; //0
	} f;
	unsigned int v;
} DACDQ_IDX_FR_T;
#define DACDQ_IDX_FRrv (*((regval)0xb8001550))
#define DACDQ_IDX_FRdv (0x001f0f00)
#define RMOD_DACDQ_IDX_FR(...) rset(DACDQ_IDX_FR, DACDQ_IDX_FRrv, __VA_ARGS__)
#define RIZS_DACDQ_IDX_FR(...) rset(DACDQ_IDX_FR, 0, __VA_ARGS__)
#define RFLD_DACDQ_IDX_FR(fld) (*((const volatile DACDQ_IDX_FR_T *)0xb8001550)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int dqm0_phase_shift_90:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqm1_phase_shift_90:5; //0
		unsigned int mbz_2:16; //0
	} f;
	unsigned int v;
} DCDQMR_T;
#define DCDQMRrv (*((regval)0xb8001590))
#define DCDQMRdv (0x00000000)
#define RMOD_DCDQMR(...) rset(DCDQMR, DCDQMRrv, __VA_ARGS__)
#define RIZS_DCDQMR(...) rset(DCDQMR, 0, __VA_ARGS__)
#define RFLD_DCDQMR(fld) (*((const volatile DCDQMR_T *)0xb8001590)).f.fld

typedef union {
	struct {
		unsigned int dq7r_fifo_d:4; //0
		unsigned int dq6r_fifo_d:4; //0
		unsigned int dq5r_fifo_d:4; //0
		unsigned int dq4r_fifo_d:4; //0
		unsigned int dq3r_fifo_d:4; //0
		unsigned int dq2r_fifo_d:3; //0
		unsigned int dq1r_fifo_d:5; //0
		unsigned int dq0r_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDRR0_T;
#define DCFDRR0rv (*((regval)0xb8001594))
#define DCFDRR0dv (0x00000000)
#define RMOD_DCFDRR0(...) rset(DCFDRR0, DCFDRR0rv, __VA_ARGS__)
#define RIZS_DCFDRR0(...) rset(DCFDRR0, 0, __VA_ARGS__)
#define RFLD_DCFDRR0(fld) (*((const volatile DCFDRR0_T *)0xb8001594)).f.fld

typedef union {
	struct {
		unsigned int dq15r_fifo_d:4; //0
		unsigned int dq14r_fifo_d:4; //0
		unsigned int dq13r_fifo_d:4; //0
		unsigned int dq12r_fifo_d:4; //0
		unsigned int dq11r_fifo_d:4; //0
		unsigned int dq10r_fifo_d:3; //0
		unsigned int dq9r_fifo_d:5; //0
		unsigned int dq8r_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDRR1_T;
#define DCFDRR1rv (*((regval)0xb8001598))
#define DCFDRR1dv (0x00000000)
#define RMOD_DCFDRR1(...) rset(DCFDRR1, DCFDRR1rv, __VA_ARGS__)
#define RIZS_DCFDRR1(...) rset(DCFDRR1, 0, __VA_ARGS__)
#define RFLD_DCFDRR1(fld) (*((const volatile DCFDRR1_T *)0xb8001598)).f.fld

typedef union {
	struct {
		unsigned int dq7f_fifo_d:4; //0
		unsigned int dq6f_fifo_d:4; //0
		unsigned int dq5f_fifo_d:4; //0
		unsigned int dq4f_fifo_d:4; //0
		unsigned int dq3f_fifo_d:4; //0
		unsigned int dq2f_fifo_d:3; //0
		unsigned int dq1f_fifo_d:5; //0
		unsigned int dq0f_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDFR0_T;
#define DCFDFR0rv (*((regval)0xb800159c))
#define DCFDFR0dv (0x00000000)
#define RMOD_DCFDFR0(...) rset(DCFDFR0, DCFDFR0rv, __VA_ARGS__)
#define RIZS_DCFDFR0(...) rset(DCFDFR0, 0, __VA_ARGS__)
#define RFLD_DCFDFR0(fld) (*((const volatile DCFDFR0_T *)0xb800159c)).f.fld

typedef union {
	struct {
		unsigned int dq15f_fifo_d:4; //0
		unsigned int dq14f_fifo_d:4; //0
		unsigned int dq13f_fifo_d:4; //0
		unsigned int dq12f_fifo_d:4; //0
		unsigned int dq11f_fifo_d:4; //0
		unsigned int dq10f_fifo_d:3; //0
		unsigned int dq9f_fifo_d:5; //0
		unsigned int dq8f_fifo_d:4; //0
	} f;
	unsigned int v;
} DCFDFR1_T;
#define DCFDFR1rv (*((regval)0xb80015a0))
#define DCFDFR1dv (0x00000000)
#define RMOD_DCFDFR1(...) rset(DCFDFR1, DCFDFR1rv, __VA_ARGS__)
#define RIZS_DCFDFR1(...) rset(DCFDFR1, 0, __VA_ARGS__)
#define RFLD_DCFDFR1(fld) (*((const volatile DCFDFR1_T *)0xb80015a0)).f.fld

typedef union {
	struct {
		unsigned int fifo_d_dbg_en:1; //0
		unsigned int mbz_0:30; //0
		unsigned int fifo_d_sel:1; //0
	} f;
	unsigned int v;
} DCFDDCR_T;
#define DCFDDCRrv (*((regval)0xb80015a4))
#define DCFDDCRdv (0x00000000)
#define RMOD_DCFDDCR(...) rset(DCFDDCR, DCFDDCRrv, __VA_ARGS__)
#define RIZS_DCFDDCR(...) rset(DCFDDCR, 0, __VA_ARGS__)
#define RFLD_DCFDDCR(fld) (*((const volatile DCFDDCR_T *)0xb80015a4)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:4; //0
		unsigned int cke_dly_tap:4; //0
		unsigned int cs1_dly_tap:4; //0
		unsigned int cs0_dly_tap:4; //0
		unsigned int odt_dly_tap:4; //0
		unsigned int ras_dly_tap:4; //0
		unsigned int cas_dly_tap:4; //0
		unsigned int we_dly_tap:4; //0
	} f;
	unsigned int v;
} DCSDCR0_T;
#define DCSDCR0rv (*((regval)0xb80015b0))
#define DCSDCR0dv (0x00000000)
#define RMOD_DCSDCR0(...) rset(DCSDCR0, DCSDCR0rv, __VA_ARGS__)
#define RIZS_DCSDCR0(...) rset(DCSDCR0, 0, __VA_ARGS__)
#define RFLD_DCSDCR0(fld) (*((const volatile DCSDCR0_T *)0xb80015b0)).f.fld

typedef union {
	struct {
		unsigned int mbz_0:3; //0
		unsigned int dqs1_dly_tap:5; //0
		unsigned int mbz_1:3; //0
		unsigned int dqs0_dly_tap:5; //0
		unsigned int mbz_2:4; //0
		unsigned int ba2_dly_tap:4; //0
		unsigned int ba1_dly_tap:4; //0
		unsigned int ba0_dly_tap:4; //0
	} f;
	unsigned int v;
} DCSDCR1_T;
#define DCSDCR1rv (*((regval)0xb80015b4))
#define DCSDCR1dv (0x00000000)
#define RMOD_DCSDCR1(...) rset(DCSDCR1, DCSDCR1rv, __VA_ARGS__)
#define RIZS_DCSDCR1(...) rset(DCSDCR1, 0, __VA_ARGS__)
#define RFLD_DCSDCR1(fld) (*((const volatile DCSDCR1_T *)0xb80015b4)).f.fld

typedef union {
	struct {
		unsigned int a7_dly_tap:4; //0
		unsigned int a6_dly_tap:4; //0
		unsigned int a5_dly_tap:4; //0
		unsigned int a4_dly_tap:4; //0
		unsigned int a3_dly_tap:4; //0
		unsigned int a2_dly_tap:4; //0
		unsigned int a1_dly_tap:4; //0
		unsigned int a0_dly_tap:4; //0
	} f;
	unsigned int v;
} DCSDCR2_T;
#define DCSDCR2rv (*((regval)0xb80015b8))
#define DCSDCR2dv (0x00000000)
#define RMOD_DCSDCR2(...) rset(DCSDCR2, DCSDCR2rv, __VA_ARGS__)
#define RIZS_DCSDCR2(...) rset(DCSDCR2, 0, __VA_ARGS__)
#define RFLD_DCSDCR2(fld) (*((const volatile DCSDCR2_T *)0xb80015b8)).f.fld

typedef union {
	struct {
		unsigned int a15_dly_tap:4; //0
		unsigned int a14_dly_tap:4; //0
		unsigned int a13_dly_tap:4; //0
		unsigned int a12_dly_tap:4; //0
		unsigned int a11_dly_tap:4; //0
		unsigned int a10_dly_tap:4; //0
		unsigned int a9_dly_tap:4; //0
		unsigned int a8_dly_tap:4; //0
	} f;
	unsigned int v;
} DCSDCR3_T;
#define DCSDCR3rv (*((regval)0xb80015bc))
#define DCSDCR3dv (0x00000000)
#define RMOD_DCSDCR3(...) rset(DCSDCR3, DCSDCR3rv, __VA_ARGS__)
#define RIZS_DCSDCR3(...) rset(DCSDCR3, 0, __VA_ARGS__)
#define RFLD_DCSDCR3(fld) (*((const volatile DCSDCR3_T *)0xb80015bc)).f.fld

typedef union {
	struct {
	    unsigned int mbz_0:3;				//0
		unsigned int dqm1_out_en_delay:5;	//0
		unsigned int mbz_1:3;				//0
		unsigned int dqm0_out_en_delay:5;	//0
		unsigned int mbz_2:16;				//0
	} f;
	unsigned int v;
} DWDMOR_T;
#define DWDMORrv (*((regval)0xb80015c0))
#define DWDMORdv (0x00000000)
#define RMOD_DWDMOR(...) rset(DWDMOR, DWDMORrv, __VA_ARGS__)
#define RIZS_DWDMOR(...) rset(DWDMOR, 0, __VA_ARGS__)
#define RFLD_DWDMOR(fld) (*((const volatile DWDMOR_T *)0xb80015c0)).f.fld

typedef union {
	struct {
	    unsigned int mbz_0:3;				//0
		unsigned int dqm1_out_en_delay:5;	//0
		unsigned int mbz_1:3;				//0
		unsigned int dqm0_out_en_delay:5;	//0
		unsigned int mbz_2:16;				//0
	} f;
	unsigned int v;
} DWDQSOR_T;
#define DWDQSORrv (*((regval)0xb80015c4))
#define DWDQSORdv (0x00000000)
#define RMOD_DWDQSOR(...) rset(DWDQSOR, DWDQSORrv, __VA_ARGS__)
#define RIZS_DWDQSOR(...) rset(DWDQSOR, 0, __VA_ARGS__)
#define RFLD_DWDQSOR(fld) (*((const volatile DWDQSOR_T *)0xb80015c4)).f.fld