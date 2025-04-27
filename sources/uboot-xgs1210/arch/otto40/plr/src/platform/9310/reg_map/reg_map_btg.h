#ifndef _REG_MAP_BTG_H
#define _REG_MAP_BTG_H

/*-----------------------------------------------------
 Extraced from file_LX0_BUS_TRAF_GEN.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int enable:1; //0x0
		unsigned int poll:1; //0x0
		unsigned int bus_traf_gen:2; //0x0
		unsigned int func:4; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int swap_type:1; //0x0
		unsigned int ent_size:2; //0x0
		unsigned int mbz_1:10; //0x0
		unsigned int dis_check_ddr:1; //0x0
		unsigned int debug_sel:1; //0x0
		unsigned int burst_size:2; //0x0
		unsigned int enough_thr:2; //0x0
		unsigned int dst_low_thr:2; //0x0
		unsigned int src_low_th:2; //0x0
	} f;
	unsigned int v;
} LX0_GDMA_CTRL_T;
#define LX0_GDMA_CTRLar (0xB800A000)
#define LX0_GDMA_CTRLdv (0x00000000)
#define LX0_GDMA_CTRLrv RVAL(LX0_GDMA_CTRL)
#define RMOD_LX0_GDMA_CTRL(...) RMOD(LX0_GDMA_CTRL, __VA_ARGS__)
#define RIZS_LX0_GDMA_CTRL(...) RIZS(LX0_GDMA_CTRL, __VA_ARGS__)
#define RFLD_LX0_GDMA_CTRL(fld) RFLD(LX0_GDMA_CTRL, fld)

typedef union {
	struct {
		unsigned int comp_ie:1; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int need_cpu_ie:1; //0x0
		unsigned int rd_pkt_ie:1; //0x0
		unsigned int err_rchk_ie:1; //0x0
		unsigned int err_rprd_ie:1; //0x0
		unsigned int err_rrp_ie:1; //0x0
		unsigned int wr_pkt_ie:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int err_wprd_ie:1; //0x0
		unsigned int err_wrp_ie:1; //0x0
		unsigned int mbz_2:20; //0x0
	} f;
	unsigned int v;
} LX0_GDMA_IM_T;
#define LX0_GDMA_IMar (0xB800A004)
#define LX0_GDMA_IMdv (0x00000000)
#define LX0_GDMA_IMrv RVAL(LX0_GDMA_IM)
#define RMOD_LX0_GDMA_IM(...) RMOD(LX0_GDMA_IM, __VA_ARGS__)
#define RIZS_LX0_GDMA_IM(...) RIZS(LX0_GDMA_IM, __VA_ARGS__)
#define RFLD_LX0_GDMA_IM(fld) RFLD(LX0_GDMA_IM, fld)

typedef union {
	struct {
		unsigned int comp_ip:1; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int need_cpu_ip:1; //0x0
		unsigned int rd_pkt_ip:1; //0x0
		unsigned int err_rchk_ip:1; //0x0
		unsigned int err_rprd_ip:1; //0x0
		unsigned int err_rrp_ip:1; //0x0
		unsigned int wr_pkt_ip:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int err_wprd_ip:1; //0x0
		unsigned int err_wrp_ip:1; //0x0
		unsigned int mbz_2:20; //0x0
	} f;
	unsigned int v;
} LX0_GDMA_IS_T;
#define LX0_GDMA_ISar (0xB800A008)
#define LX0_GDMA_ISdv (0x00000000)
#define LX0_GDMA_ISrv RVAL(LX0_GDMA_IS)
#define RMOD_LX0_GDMA_IS(...) RMOD(LX0_GDMA_IS, __VA_ARGS__)
#define RIZS_LX0_GDMA_IS(...) RIZS(LX0_GDMA_IS, __VA_ARGS__)
#define RFLD_LX0_GDMA_IS(fld) RFLD(LX0_GDMA_IS, fld)

typedef union {
	struct {
		unsigned int icvl:32; //0x0
	} f;
	unsigned int v;
} LX0_GDMA_ICVL_T;
#define LX0_GDMA_ICVLar (0xB800A00C)
#define LX0_GDMA_ICVLdv (0x00000000)
#define LX0_GDMA_ICVLrv RVAL(LX0_GDMA_ICVL)
#define RMOD_LX0_GDMA_ICVL(...) RMOD(LX0_GDMA_ICVL, __VA_ARGS__)
#define RIZS_LX0_GDMA_ICVL(...) RIZS(LX0_GDMA_ICVL, __VA_ARGS__)
#define RFLD_LX0_GDMA_ICVL(fld) RFLD(LX0_GDMA_ICVL, fld)

typedef union {
	struct {
		unsigned int enable:1; //0x0
		unsigned int no_use30:8; //0x0
		unsigned int rec_err:1; //0x0
		unsigned int prec:1; //0x0
		unsigned int infinite:1; //0x0
		unsigned int no_use19:2; //0x0
		unsigned int burst_len:2; //0x0
		unsigned int no_use15:3; //0x0
		unsigned int pkt_len:8; //0x0
		unsigned int no_use04:5; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_CTRL_T;
#define LX0_BTG_WRITE_CTRLar (0xB800A100)
#define LX0_BTG_WRITE_CTRLdv (0x00000000)
#define LX0_BTG_WRITE_CTRLrv RVAL(LX0_BTG_WRITE_CTRL)
#define RMOD_LX0_BTG_WRITE_CTRL(...) RMOD(LX0_BTG_WRITE_CTRL, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_CTRL(...) RIZS(LX0_BTG_WRITE_CTRL, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_CTRL(fld) RFLD(LX0_BTG_WRITE_CTRL, fld)

typedef union {
	struct {
		unsigned int iter:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_ITER_T;
#define LX0_BTG_WRITE_ITERar (0xB800A104)
#define LX0_BTG_WRITE_ITERdv (0x00000000)
#define LX0_BTG_WRITE_ITERrv RVAL(LX0_BTG_WRITE_ITER)
#define RMOD_LX0_BTG_WRITE_ITER(...) RMOD(LX0_BTG_WRITE_ITER, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_ITER(...) RIZS(LX0_BTG_WRITE_ITER, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_ITER(fld) RFLD(LX0_BTG_WRITE_ITER, fld)

typedef union {
	struct {
		unsigned int resp_time:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_RESP_TIME_T;
#define LX0_BTG_WRITE_RESP_TIMEar (0xB800A108)
#define LX0_BTG_WRITE_RESP_TIMEdv (0x00000000)
#define LX0_BTG_WRITE_RESP_TIMErv RVAL(LX0_BTG_WRITE_RESP_TIME)
#define RMOD_LX0_BTG_WRITE_RESP_TIME(...) RMOD(LX0_BTG_WRITE_RESP_TIME, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_RESP_TIME(...) RIZS(LX0_BTG_WRITE_RESP_TIME, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_RESP_TIME(fld) RFLD(LX0_BTG_WRITE_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int period:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_PERI_T;
#define LX0_BTG_WRITE_PERIar (0xB800A10C)
#define LX0_BTG_WRITE_PERIdv (0x00000000)
#define LX0_BTG_WRITE_PERIrv RVAL(LX0_BTG_WRITE_PERI)
#define RMOD_LX0_BTG_WRITE_PERI(...) RMOD(LX0_BTG_WRITE_PERI, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_PERI(...) RIZS(LX0_BTG_WRITE_PERI, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_PERI(fld) RFLD(LX0_BTG_WRITE_PERI, fld)

typedef union {
	struct {
		unsigned int base_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_ADDR_T;
#define LX0_BTG_WRITE_ADDRar (0xB800A110)
#define LX0_BTG_WRITE_ADDRdv (0x00000000)
#define LX0_BTG_WRITE_ADDRrv RVAL(LX0_BTG_WRITE_ADDR)
#define RMOD_LX0_BTG_WRITE_ADDR(...) RMOD(LX0_BTG_WRITE_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_ADDR(...) RIZS(LX0_BTG_WRITE_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_ADDR(fld) RFLD(LX0_BTG_WRITE_ADDR, fld)

typedef union {
	struct {
		unsigned int addr_msk:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_ADDR_MASK_T;
#define LX0_BTG_WRITE_ADDR_MASKar (0xB800A114)
#define LX0_BTG_WRITE_ADDR_MASKdv (0x00000000)
#define LX0_BTG_WRITE_ADDR_MASKrv RVAL(LX0_BTG_WRITE_ADDR_MASK)
#define RMOD_LX0_BTG_WRITE_ADDR_MASK(...) RMOD(LX0_BTG_WRITE_ADDR_MASK, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_ADDR_MASK(...) RIZS(LX0_BTG_WRITE_ADDR_MASK, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_ADDR_MASK(fld) RFLD(LX0_BTG_WRITE_ADDR_MASK, fld)

typedef union {
	struct {
		unsigned int dec_gap:1; //0x0
		unsigned int no_use30:15; //0x0
		unsigned int addr_gap:16; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_GAP_T;
#define LX0_BTG_WRITE_GAPar (0xB800A118)
#define LX0_BTG_WRITE_GAPdv (0x00000000)
#define LX0_BTG_WRITE_GAPrv RVAL(LX0_BTG_WRITE_GAP)
#define RMOD_LX0_BTG_WRITE_GAP(...) RMOD(LX0_BTG_WRITE_GAP, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_GAP(...) RIZS(LX0_BTG_WRITE_GAP, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_GAP(fld) RFLD(LX0_BTG_WRITE_GAP, fld)

typedef union {
	struct {
		unsigned int cur_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_CURR_ADDR_T;
#define LX0_BTG_WRITE_CURR_ADDRar (0xB800A11C)
#define LX0_BTG_WRITE_CURR_ADDRdv (0x00000000)
#define LX0_BTG_WRITE_CURR_ADDRrv RVAL(LX0_BTG_WRITE_CURR_ADDR)
#define RMOD_LX0_BTG_WRITE_CURR_ADDR(...) RMOD(LX0_BTG_WRITE_CURR_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_CURR_ADDR(...) RIZS(LX0_BTG_WRITE_CURR_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_CURR_ADDR(fld) RFLD(LX0_BTG_WRITE_CURR_ADDR, fld)

typedef union {
	struct {
		unsigned int resp_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_RESP_FAIL_ADDR_T;
#define LX0_BTG_WRITE_RESP_FAIL_ADDRar (0xB800A120)
#define LX0_BTG_WRITE_RESP_FAIL_ADDRdv (0x00000000)
#define LX0_BTG_WRITE_RESP_FAIL_ADDRrv RVAL(LX0_BTG_WRITE_RESP_FAIL_ADDR)
#define RMOD_LX0_BTG_WRITE_RESP_FAIL_ADDR(...) RMOD(LX0_BTG_WRITE_RESP_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_RESP_FAIL_ADDR(...) RIZS(LX0_BTG_WRITE_RESP_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_RESP_FAIL_ADDR(fld) RFLD(LX0_BTG_WRITE_RESP_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int peri_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_PERI_FAIL_ADDR_T;
#define LX0_BTG_WRITE_PERI_FAIL_ADDRar (0xB800A124)
#define LX0_BTG_WRITE_PERI_FAIL_ADDRdv (0x00000000)
#define LX0_BTG_WRITE_PERI_FAIL_ADDRrv RVAL(LX0_BTG_WRITE_PERI_FAIL_ADDR)
#define RMOD_LX0_BTG_WRITE_PERI_FAIL_ADDR(...) RMOD(LX0_BTG_WRITE_PERI_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_PERI_FAIL_ADDR(...) RIZS(LX0_BTG_WRITE_PERI_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_PERI_FAIL_ADDR(fld) RFLD(LX0_BTG_WRITE_PERI_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int max_resp_time:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_WRITE_MAX_RESP_TIME_T;
#define LX0_BTG_WRITE_MAX_RESP_TIMEar (0xB800A12C)
#define LX0_BTG_WRITE_MAX_RESP_TIMEdv (0x00000000)
#define LX0_BTG_WRITE_MAX_RESP_TIMErv RVAL(LX0_BTG_WRITE_MAX_RESP_TIME)
#define RMOD_LX0_BTG_WRITE_MAX_RESP_TIME(...) RMOD(LX0_BTG_WRITE_MAX_RESP_TIME, __VA_ARGS__)
#define RIZS_LX0_BTG_WRITE_MAX_RESP_TIME(...) RIZS(LX0_BTG_WRITE_MAX_RESP_TIME, __VA_ARGS__)
#define RFLD_LX0_BTG_WRITE_MAX_RESP_TIME(fld) RFLD(LX0_BTG_WRITE_MAX_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX0_BTC_WRITE_IN_VEC_T;
#define LX0_BTC_WRITE_IN_VECar (0xB800A130)
#define LX0_BTC_WRITE_IN_VECdv (0x00000000)
#define LX0_BTC_WRITE_IN_VECrv RVAL(LX0_BTC_WRITE_IN_VEC)
#define RMOD_LX0_BTC_WRITE_IN_VEC(...) RMOD(LX0_BTC_WRITE_IN_VEC, __VA_ARGS__)
#define RIZS_LX0_BTC_WRITE_IN_VEC(...) RIZS(LX0_BTC_WRITE_IN_VEC, __VA_ARGS__)
#define RFLD_LX0_BTC_WRITE_IN_VEC(fld) RFLD(LX0_BTC_WRITE_IN_VEC, fld)

typedef union {
	struct {
		unsigned int enable:1; //0x0
		unsigned int no_use30:5; //0x0
		unsigned int chk_mode:2; //0x0
		unsigned int no_use23:1; //0x0
		unsigned int rec_err:1; //0x0
		unsigned int prec:1; //0x0
		unsigned int infinite:1; //0x0
		unsigned int no_use19:2; //0x0
		unsigned int burst_len:2; //0x0
		unsigned int no_use15:3; //0x0
		unsigned int pkt_len:8; //0x0
		unsigned int no_use04:5; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_CTRL_T;
#define LX0_BTG_READ_CTRLar (0xB800A200)
#define LX0_BTG_READ_CTRLdv (0x00000000)
#define LX0_BTG_READ_CTRLrv RVAL(LX0_BTG_READ_CTRL)
#define RMOD_LX0_BTG_READ_CTRL(...) RMOD(LX0_BTG_READ_CTRL, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_CTRL(...) RIZS(LX0_BTG_READ_CTRL, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_CTRL(fld) RFLD(LX0_BTG_READ_CTRL, fld)

typedef union {
	struct {
		unsigned int iter:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_ITER_T;
#define LX0_BTG_READ_ITERar (0xB800A204)
#define LX0_BTG_READ_ITERdv (0x00000000)
#define LX0_BTG_READ_ITERrv RVAL(LX0_BTG_READ_ITER)
#define RMOD_LX0_BTG_READ_ITER(...) RMOD(LX0_BTG_READ_ITER, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_ITER(...) RIZS(LX0_BTG_READ_ITER, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_ITER(fld) RFLD(LX0_BTG_READ_ITER, fld)

typedef union {
	struct {
		unsigned int resp_time:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_RESP_TIME_T;
#define LX0_BTG_READ_RESP_TIMEar (0xB800A208)
#define LX0_BTG_READ_RESP_TIMEdv (0x00000000)
#define LX0_BTG_READ_RESP_TIMErv RVAL(LX0_BTG_READ_RESP_TIME)
#define RMOD_LX0_BTG_READ_RESP_TIME(...) RMOD(LX0_BTG_READ_RESP_TIME, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_RESP_TIME(...) RIZS(LX0_BTG_READ_RESP_TIME, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_RESP_TIME(fld) RFLD(LX0_BTG_READ_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int period:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_PERI_T;
#define LX0_BTG_READ_PERIar (0xB800A20C)
#define LX0_BTG_READ_PERIdv (0x00000000)
#define LX0_BTG_READ_PERIrv RVAL(LX0_BTG_READ_PERI)
#define RMOD_LX0_BTG_READ_PERI(...) RMOD(LX0_BTG_READ_PERI, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_PERI(...) RIZS(LX0_BTG_READ_PERI, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_PERI(fld) RFLD(LX0_BTG_READ_PERI, fld)

typedef union {
	struct {
		unsigned int base_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_ADDR_T;
#define LX0_BTG_READ_ADDRar (0xB800A210)
#define LX0_BTG_READ_ADDRdv (0x00000000)
#define LX0_BTG_READ_ADDRrv RVAL(LX0_BTG_READ_ADDR)
#define RMOD_LX0_BTG_READ_ADDR(...) RMOD(LX0_BTG_READ_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_ADDR(...) RIZS(LX0_BTG_READ_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_ADDR(fld) RFLD(LX0_BTG_READ_ADDR, fld)

typedef union {
	struct {
		unsigned int addr_msk:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_ADDR_MASK_T;
#define LX0_BTG_READ_ADDR_MASKar (0xB800A214)
#define LX0_BTG_READ_ADDR_MASKdv (0x00000000)
#define LX0_BTG_READ_ADDR_MASKrv RVAL(LX0_BTG_READ_ADDR_MASK)
#define RMOD_LX0_BTG_READ_ADDR_MASK(...) RMOD(LX0_BTG_READ_ADDR_MASK, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_ADDR_MASK(...) RIZS(LX0_BTG_READ_ADDR_MASK, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_ADDR_MASK(fld) RFLD(LX0_BTG_READ_ADDR_MASK, fld)

typedef union {
	struct {
		unsigned int dec_gap:1; //0x0
		unsigned int no_use30:15; //0x0
		unsigned int addr_gap:16; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_GAP_T;
#define LX0_BTG_READ_GAPar (0xB800A218)
#define LX0_BTG_READ_GAPdv (0x00000000)
#define LX0_BTG_READ_GAPrv RVAL(LX0_BTG_READ_GAP)
#define RMOD_LX0_BTG_READ_GAP(...) RMOD(LX0_BTG_READ_GAP, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_GAP(...) RIZS(LX0_BTG_READ_GAP, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_GAP(fld) RFLD(LX0_BTG_READ_GAP, fld)

typedef union {
	struct {
		unsigned int cur_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_CURR_ADDR_T;
#define LX0_BTG_READ_CURR_ADDRar (0xB800A21C)
#define LX0_BTG_READ_CURR_ADDRdv (0x00000000)
#define LX0_BTG_READ_CURR_ADDRrv RVAL(LX0_BTG_READ_CURR_ADDR)
#define RMOD_LX0_BTG_READ_CURR_ADDR(...) RMOD(LX0_BTG_READ_CURR_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_CURR_ADDR(...) RIZS(LX0_BTG_READ_CURR_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_CURR_ADDR(fld) RFLD(LX0_BTG_READ_CURR_ADDR, fld)

typedef union {
	struct {
		unsigned int resp_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_RESP_FAIL_ADDR_T;
#define LX0_BTG_READ_RESP_FAIL_ADDRar (0xB800A220)
#define LX0_BTG_READ_RESP_FAIL_ADDRdv (0x00000000)
#define LX0_BTG_READ_RESP_FAIL_ADDRrv RVAL(LX0_BTG_READ_RESP_FAIL_ADDR)
#define RMOD_LX0_BTG_READ_RESP_FAIL_ADDR(...) RMOD(LX0_BTG_READ_RESP_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_RESP_FAIL_ADDR(...) RIZS(LX0_BTG_READ_RESP_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_RESP_FAIL_ADDR(fld) RFLD(LX0_BTG_READ_RESP_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int peri_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_PERI_FAIL_ADDR_T;
#define LX0_BTG_READ_PERI_FAIL_ADDRar (0xB800A224)
#define LX0_BTG_READ_PERI_FAIL_ADDRdv (0x00000000)
#define LX0_BTG_READ_PERI_FAIL_ADDRrv RVAL(LX0_BTG_READ_PERI_FAIL_ADDR)
#define RMOD_LX0_BTG_READ_PERI_FAIL_ADDR(...) RMOD(LX0_BTG_READ_PERI_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_PERI_FAIL_ADDR(...) RIZS(LX0_BTG_READ_PERI_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_PERI_FAIL_ADDR(fld) RFLD(LX0_BTG_READ_PERI_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int chk_addr:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_VERI_FAIL_ADDR_T;
#define LX0_BTG_READ_VERI_FAIL_ADDRar (0xB800A228)
#define LX0_BTG_READ_VERI_FAIL_ADDRdv (0x00000000)
#define LX0_BTG_READ_VERI_FAIL_ADDRrv RVAL(LX0_BTG_READ_VERI_FAIL_ADDR)
#define RMOD_LX0_BTG_READ_VERI_FAIL_ADDR(...) RMOD(LX0_BTG_READ_VERI_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_VERI_FAIL_ADDR(...) RIZS(LX0_BTG_READ_VERI_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_VERI_FAIL_ADDR(fld) RFLD(LX0_BTG_READ_VERI_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int max_resp_time:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_MAX_RESP_TIME_T;
#define LX0_BTG_READ_MAX_RESP_TIMEar (0xB800A22C)
#define LX0_BTG_READ_MAX_RESP_TIMEdv (0x00000000)
#define LX0_BTG_READ_MAX_RESP_TIMErv RVAL(LX0_BTG_READ_MAX_RESP_TIME)
#define RMOD_LX0_BTG_READ_MAX_RESP_TIME(...) RMOD(LX0_BTG_READ_MAX_RESP_TIME, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_MAX_RESP_TIME(...) RIZS(LX0_BTG_READ_MAX_RESP_TIME, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_MAX_RESP_TIME(fld) RFLD(LX0_BTG_READ_MAX_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_IN_VEC0_T;
#define LX0_BTG_READ_IN_VEC0ar (0xB800A230)
#define LX0_BTG_READ_IN_VEC0dv (0x00000000)
#define LX0_BTG_READ_IN_VEC0rv RVAL(LX0_BTG_READ_IN_VEC0)
#define RMOD_LX0_BTG_READ_IN_VEC0(...) RMOD(LX0_BTG_READ_IN_VEC0, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_IN_VEC0(...) RIZS(LX0_BTG_READ_IN_VEC0, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_IN_VEC0(fld) RFLD(LX0_BTG_READ_IN_VEC0, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_IN_VEC1_T;
#define LX0_BTG_READ_IN_VEC1ar (0xB800A234)
#define LX0_BTG_READ_IN_VEC1dv (0x00000000)
#define LX0_BTG_READ_IN_VEC1rv RVAL(LX0_BTG_READ_IN_VEC1)
#define RMOD_LX0_BTG_READ_IN_VEC1(...) RMOD(LX0_BTG_READ_IN_VEC1, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_IN_VEC1(...) RIZS(LX0_BTG_READ_IN_VEC1, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_IN_VEC1(fld) RFLD(LX0_BTG_READ_IN_VEC1, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_IN_VEC2_T;
#define LX0_BTG_READ_IN_VEC2ar (0xB800A238)
#define LX0_BTG_READ_IN_VEC2dv (0x00000000)
#define LX0_BTG_READ_IN_VEC2rv RVAL(LX0_BTG_READ_IN_VEC2)
#define RMOD_LX0_BTG_READ_IN_VEC2(...) RMOD(LX0_BTG_READ_IN_VEC2, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_IN_VEC2(...) RIZS(LX0_BTG_READ_IN_VEC2, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_IN_VEC2(fld) RFLD(LX0_BTG_READ_IN_VEC2, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX0_BTG_READ_IN_VEC_IDX_T;
#define LX0_BTG_READ_IN_VEC_IDXar (0xB800A23C)
#define LX0_BTG_READ_IN_VEC_IDXdv (0x00000000)
#define LX0_BTG_READ_IN_VEC_IDXrv RVAL(LX0_BTG_READ_IN_VEC_IDX)
#define RMOD_LX0_BTG_READ_IN_VEC_IDX(...) RMOD(LX0_BTG_READ_IN_VEC_IDX, __VA_ARGS__)
#define RIZS_LX0_BTG_READ_IN_VEC_IDX(...) RIZS(LX0_BTG_READ_IN_VEC_IDX, __VA_ARGS__)
#define RFLD_LX0_BTG_READ_IN_VEC_IDX(fld) RFLD(LX0_BTG_READ_IN_VEC_IDX, fld)

/*-----------------------------------------------------
 Extraced from file_LX1_BUS_TRAF_GEN.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int enable:1; //0x0
		unsigned int poll:1; //0x0
		unsigned int bus_traf_gen:2; //0x0
		unsigned int func:4; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int swap_type:1; //0x0
		unsigned int ent_size:2; //0x0
		unsigned int mbz_1:10; //0x0
		unsigned int dis_check_ddr:1; //0x0
		unsigned int debug_sel:1; //0x0
		unsigned int burst_size:2; //0x0
		unsigned int enough_thr:2; //0x0
		unsigned int dst_low_thr:2; //0x0
		unsigned int src_low_th:2; //0x0
	} f;
	unsigned int v;
} LX1_GDMA_CTRL_T;
#define LX1_GDMA_CTRLar (0xB8018000)
#define LX1_GDMA_CTRLdv (0x00000000)
#define LX1_GDMA_CTRLrv RVAL(LX1_GDMA_CTRL)
#define RMOD_LX1_GDMA_CTRL(...) RMOD(LX1_GDMA_CTRL, __VA_ARGS__)
#define RIZS_LX1_GDMA_CTRL(...) RIZS(LX1_GDMA_CTRL, __VA_ARGS__)
#define RFLD_LX1_GDMA_CTRL(fld) RFLD(LX1_GDMA_CTRL, fld)

typedef union {
	struct {
		unsigned int comp_ie:1; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int need_cpu_ie:1; //0x0
		unsigned int rd_pkt_ie:1; //0x0
		unsigned int err_rchk_ie:1; //0x0
		unsigned int err_rprd_ie:1; //0x0
		unsigned int err_rrp_ie:1; //0x0
		unsigned int wr_pkt_ie:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int err_wprd_ie:1; //0x0
		unsigned int err_wrp_ie:1; //0x0
		unsigned int mbz_2:20; //0x0
	} f;
	unsigned int v;
} LX1_GDMA_IM_T;
#define LX1_GDMA_IMar (0xB8018004)
#define LX1_GDMA_IMdv (0x00000000)
#define LX1_GDMA_IMrv RVAL(LX1_GDMA_IM)
#define RMOD_LX1_GDMA_IM(...) RMOD(LX1_GDMA_IM, __VA_ARGS__)
#define RIZS_LX1_GDMA_IM(...) RIZS(LX1_GDMA_IM, __VA_ARGS__)
#define RFLD_LX1_GDMA_IM(fld) RFLD(LX1_GDMA_IM, fld)

typedef union {
	struct {
		unsigned int comp_ip:1; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int need_cpu_ip:1; //0x0
		unsigned int rd_pkt_ip:1; //0x0
		unsigned int err_rchk_ip:1; //0x0
		unsigned int err_rprd_ip:1; //0x0
		unsigned int err_rrp_ip:1; //0x0
		unsigned int wr_pkt_ip:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int err_wprd_ip:1; //0x0
		unsigned int err_wrp_ip:1; //0x0
		unsigned int mbz_2:20; //0x0
	} f;
	unsigned int v;
} LX1_GDMA_IS_T;
#define LX1_GDMA_ISar (0xB8018008)
#define LX1_GDMA_ISdv (0x00000000)
#define LX1_GDMA_ISrv RVAL(LX1_GDMA_IS)
#define RMOD_LX1_GDMA_IS(...) RMOD(LX1_GDMA_IS, __VA_ARGS__)
#define RIZS_LX1_GDMA_IS(...) RIZS(LX1_GDMA_IS, __VA_ARGS__)
#define RFLD_LX1_GDMA_IS(fld) RFLD(LX1_GDMA_IS, fld)

typedef union {
	struct {
		unsigned int icvl:32; //0x0
	} f;
	unsigned int v;
} LX1_GDMA_ICVL_T;
#define LX1_GDMA_ICVLar (0xB801800C)
#define LX1_GDMA_ICVLdv (0x00000000)
#define LX1_GDMA_ICVLrv RVAL(LX1_GDMA_ICVL)
#define RMOD_LX1_GDMA_ICVL(...) RMOD(LX1_GDMA_ICVL, __VA_ARGS__)
#define RIZS_LX1_GDMA_ICVL(...) RIZS(LX1_GDMA_ICVL, __VA_ARGS__)
#define RFLD_LX1_GDMA_ICVL(fld) RFLD(LX1_GDMA_ICVL, fld)

typedef union {
	struct {
		unsigned int enable:1; //0x0
		unsigned int no_use30:8; //0x0
		unsigned int rec_err:1; //0x0
		unsigned int prec:1; //0x0
		unsigned int infinite:1; //0x0
		unsigned int no_use19:2; //0x0
		unsigned int burst_len:2; //0x0
		unsigned int no_use15:3; //0x0
		unsigned int pkt_len:8; //0x0
		unsigned int no_use04:5; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_CTRL_T;
#define LX1_BTG_WRITE_CTRLar (0xB8018100)
#define LX1_BTG_WRITE_CTRLdv (0x00000000)
#define LX1_BTG_WRITE_CTRLrv RVAL(LX1_BTG_WRITE_CTRL)
#define RMOD_LX1_BTG_WRITE_CTRL(...) RMOD(LX1_BTG_WRITE_CTRL, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_CTRL(...) RIZS(LX1_BTG_WRITE_CTRL, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_CTRL(fld) RFLD(LX1_BTG_WRITE_CTRL, fld)

typedef union {
	struct {
		unsigned int iter:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_ITER_T;
#define LX1_BTG_WRITE_ITERar (0xB8018104)
#define LX1_BTG_WRITE_ITERdv (0x00000000)
#define LX1_BTG_WRITE_ITERrv RVAL(LX1_BTG_WRITE_ITER)
#define RMOD_LX1_BTG_WRITE_ITER(...) RMOD(LX1_BTG_WRITE_ITER, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_ITER(...) RIZS(LX1_BTG_WRITE_ITER, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_ITER(fld) RFLD(LX1_BTG_WRITE_ITER, fld)

typedef union {
	struct {
		unsigned int resp_time:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_RESP_TIME_T;
#define LX1_BTG_WRITE_RESP_TIMEar (0xB8018108)
#define LX1_BTG_WRITE_RESP_TIMEdv (0x00000000)
#define LX1_BTG_WRITE_RESP_TIMErv RVAL(LX1_BTG_WRITE_RESP_TIME)
#define RMOD_LX1_BTG_WRITE_RESP_TIME(...) RMOD(LX1_BTG_WRITE_RESP_TIME, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_RESP_TIME(...) RIZS(LX1_BTG_WRITE_RESP_TIME, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_RESP_TIME(fld) RFLD(LX1_BTG_WRITE_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int period:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_PERI_T;
#define LX1_BTG_WRITE_PERIar (0xB801810C)
#define LX1_BTG_WRITE_PERIdv (0x00000000)
#define LX1_BTG_WRITE_PERIrv RVAL(LX1_BTG_WRITE_PERI)
#define RMOD_LX1_BTG_WRITE_PERI(...) RMOD(LX1_BTG_WRITE_PERI, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_PERI(...) RIZS(LX1_BTG_WRITE_PERI, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_PERI(fld) RFLD(LX1_BTG_WRITE_PERI, fld)

typedef union {
	struct {
		unsigned int base_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_ADDR_T;
#define LX1_BTG_WRITE_ADDRar (0xB8018110)
#define LX1_BTG_WRITE_ADDRdv (0x00000000)
#define LX1_BTG_WRITE_ADDRrv RVAL(LX1_BTG_WRITE_ADDR)
#define RMOD_LX1_BTG_WRITE_ADDR(...) RMOD(LX1_BTG_WRITE_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_ADDR(...) RIZS(LX1_BTG_WRITE_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_ADDR(fld) RFLD(LX1_BTG_WRITE_ADDR, fld)

typedef union {
	struct {
		unsigned int addr_msk:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_ADDR_MASK_T;
#define LX1_BTG_WRITE_ADDR_MASKar (0xB8018114)
#define LX1_BTG_WRITE_ADDR_MASKdv (0x00000000)
#define LX1_BTG_WRITE_ADDR_MASKrv RVAL(LX1_BTG_WRITE_ADDR_MASK)
#define RMOD_LX1_BTG_WRITE_ADDR_MASK(...) RMOD(LX1_BTG_WRITE_ADDR_MASK, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_ADDR_MASK(...) RIZS(LX1_BTG_WRITE_ADDR_MASK, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_ADDR_MASK(fld) RFLD(LX1_BTG_WRITE_ADDR_MASK, fld)

typedef union {
	struct {
		unsigned int dec_gap:1; //0x0
		unsigned int no_use30:15; //0x0
		unsigned int addr_gap:16; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_GAP_T;
#define LX1_BTG_WRITE_GAPar (0xB8018118)
#define LX1_BTG_WRITE_GAPdv (0x00000000)
#define LX1_BTG_WRITE_GAPrv RVAL(LX1_BTG_WRITE_GAP)
#define RMOD_LX1_BTG_WRITE_GAP(...) RMOD(LX1_BTG_WRITE_GAP, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_GAP(...) RIZS(LX1_BTG_WRITE_GAP, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_GAP(fld) RFLD(LX1_BTG_WRITE_GAP, fld)

typedef union {
	struct {
		unsigned int cur_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_CURR_ADDR_T;
#define LX1_BTG_WRITE_CURR_ADDRar (0xB801811C)
#define LX1_BTG_WRITE_CURR_ADDRdv (0x00000000)
#define LX1_BTG_WRITE_CURR_ADDRrv RVAL(LX1_BTG_WRITE_CURR_ADDR)
#define RMOD_LX1_BTG_WRITE_CURR_ADDR(...) RMOD(LX1_BTG_WRITE_CURR_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_CURR_ADDR(...) RIZS(LX1_BTG_WRITE_CURR_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_CURR_ADDR(fld) RFLD(LX1_BTG_WRITE_CURR_ADDR, fld)

typedef union {
	struct {
		unsigned int resp_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_RESP_FAIL_ADDR_T;
#define LX1_BTG_WRITE_RESP_FAIL_ADDRar (0xB8018120)
#define LX1_BTG_WRITE_RESP_FAIL_ADDRdv (0x00000000)
#define LX1_BTG_WRITE_RESP_FAIL_ADDRrv RVAL(LX1_BTG_WRITE_RESP_FAIL_ADDR)
#define RMOD_LX1_BTG_WRITE_RESP_FAIL_ADDR(...) RMOD(LX1_BTG_WRITE_RESP_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_RESP_FAIL_ADDR(...) RIZS(LX1_BTG_WRITE_RESP_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_RESP_FAIL_ADDR(fld) RFLD(LX1_BTG_WRITE_RESP_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int peri_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_PERI_FAIL_ADDR_T;
#define LX1_BTG_WRITE_PERI_FAIL_ADDRar (0xB8018124)
#define LX1_BTG_WRITE_PERI_FAIL_ADDRdv (0x00000000)
#define LX1_BTG_WRITE_PERI_FAIL_ADDRrv RVAL(LX1_BTG_WRITE_PERI_FAIL_ADDR)
#define RMOD_LX1_BTG_WRITE_PERI_FAIL_ADDR(...) RMOD(LX1_BTG_WRITE_PERI_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_PERI_FAIL_ADDR(...) RIZS(LX1_BTG_WRITE_PERI_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_PERI_FAIL_ADDR(fld) RFLD(LX1_BTG_WRITE_PERI_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int max_resp_time:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_WRITE_MAX_RESP_TIME_T;
#define LX1_BTG_WRITE_MAX_RESP_TIMEar (0xB801812C)
#define LX1_BTG_WRITE_MAX_RESP_TIMEdv (0x00000000)
#define LX1_BTG_WRITE_MAX_RESP_TIMErv RVAL(LX1_BTG_WRITE_MAX_RESP_TIME)
#define RMOD_LX1_BTG_WRITE_MAX_RESP_TIME(...) RMOD(LX1_BTG_WRITE_MAX_RESP_TIME, __VA_ARGS__)
#define RIZS_LX1_BTG_WRITE_MAX_RESP_TIME(...) RIZS(LX1_BTG_WRITE_MAX_RESP_TIME, __VA_ARGS__)
#define RFLD_LX1_BTG_WRITE_MAX_RESP_TIME(fld) RFLD(LX1_BTG_WRITE_MAX_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX1_BTC_WRITE_IN_VEC_T;
#define LX1_BTC_WRITE_IN_VECar (0xB8018130)
#define LX1_BTC_WRITE_IN_VECdv (0x00000000)
#define LX1_BTC_WRITE_IN_VECrv RVAL(LX1_BTC_WRITE_IN_VEC)
#define RMOD_LX1_BTC_WRITE_IN_VEC(...) RMOD(LX1_BTC_WRITE_IN_VEC, __VA_ARGS__)
#define RIZS_LX1_BTC_WRITE_IN_VEC(...) RIZS(LX1_BTC_WRITE_IN_VEC, __VA_ARGS__)
#define RFLD_LX1_BTC_WRITE_IN_VEC(fld) RFLD(LX1_BTC_WRITE_IN_VEC, fld)

typedef union {
	struct {
		unsigned int enable:1; //0x0
		unsigned int no_use30:5; //0x0
		unsigned int chk_mode:2; //0x0
		unsigned int no_use23:1; //0x0
		unsigned int rec_err:1; //0x0
		unsigned int prec:1; //0x0
		unsigned int infinite:1; //0x0
		unsigned int no_use19:2; //0x0
		unsigned int burst_len:2; //0x0
		unsigned int no_use15:3; //0x0
		unsigned int pkt_len:8; //0x0
		unsigned int no_use04:5; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_CTRL_T;
#define LX1_BTG_READ_CTRLar (0xB8018200)
#define LX1_BTG_READ_CTRLdv (0x00000000)
#define LX1_BTG_READ_CTRLrv RVAL(LX1_BTG_READ_CTRL)
#define RMOD_LX1_BTG_READ_CTRL(...) RMOD(LX1_BTG_READ_CTRL, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_CTRL(...) RIZS(LX1_BTG_READ_CTRL, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_CTRL(fld) RFLD(LX1_BTG_READ_CTRL, fld)

typedef union {
	struct {
		unsigned int iter:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_ITER_T;
#define LX1_BTG_READ_ITERar (0xB8018204)
#define LX1_BTG_READ_ITERdv (0x00000000)
#define LX1_BTG_READ_ITERrv RVAL(LX1_BTG_READ_ITER)
#define RMOD_LX1_BTG_READ_ITER(...) RMOD(LX1_BTG_READ_ITER, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_ITER(...) RIZS(LX1_BTG_READ_ITER, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_ITER(fld) RFLD(LX1_BTG_READ_ITER, fld)

typedef union {
	struct {
		unsigned int resp_time:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_RESP_TIME_T;
#define LX1_BTG_READ_RESP_TIMEar (0xB8018208)
#define LX1_BTG_READ_RESP_TIMEdv (0x00000000)
#define LX1_BTG_READ_RESP_TIMErv RVAL(LX1_BTG_READ_RESP_TIME)
#define RMOD_LX1_BTG_READ_RESP_TIME(...) RMOD(LX1_BTG_READ_RESP_TIME, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_RESP_TIME(...) RIZS(LX1_BTG_READ_RESP_TIME, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_RESP_TIME(fld) RFLD(LX1_BTG_READ_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int period:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_PERI_T;
#define LX1_BTG_READ_PERIar (0xB801820C)
#define LX1_BTG_READ_PERIdv (0x00000000)
#define LX1_BTG_READ_PERIrv RVAL(LX1_BTG_READ_PERI)
#define RMOD_LX1_BTG_READ_PERI(...) RMOD(LX1_BTG_READ_PERI, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_PERI(...) RIZS(LX1_BTG_READ_PERI, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_PERI(fld) RFLD(LX1_BTG_READ_PERI, fld)

typedef union {
	struct {
		unsigned int base_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_ADDR_T;
#define LX1_BTG_READ_ADDRar (0xB8018210)
#define LX1_BTG_READ_ADDRdv (0x00000000)
#define LX1_BTG_READ_ADDRrv RVAL(LX1_BTG_READ_ADDR)
#define RMOD_LX1_BTG_READ_ADDR(...) RMOD(LX1_BTG_READ_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_ADDR(...) RIZS(LX1_BTG_READ_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_ADDR(fld) RFLD(LX1_BTG_READ_ADDR, fld)

typedef union {
	struct {
		unsigned int addr_msk:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_ADDR_MASK_T;
#define LX1_BTG_READ_ADDR_MASKar (0xB8018214)
#define LX1_BTG_READ_ADDR_MASKdv (0x00000000)
#define LX1_BTG_READ_ADDR_MASKrv RVAL(LX1_BTG_READ_ADDR_MASK)
#define RMOD_LX1_BTG_READ_ADDR_MASK(...) RMOD(LX1_BTG_READ_ADDR_MASK, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_ADDR_MASK(...) RIZS(LX1_BTG_READ_ADDR_MASK, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_ADDR_MASK(fld) RFLD(LX1_BTG_READ_ADDR_MASK, fld)

typedef union {
	struct {
		unsigned int dec_gap:1; //0x0
		unsigned int no_use30:15; //0x0
		unsigned int addr_gap:16; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_GAP_T;
#define LX1_BTG_READ_GAPar (0xB8018218)
#define LX1_BTG_READ_GAPdv (0x00000000)
#define LX1_BTG_READ_GAPrv RVAL(LX1_BTG_READ_GAP)
#define RMOD_LX1_BTG_READ_GAP(...) RMOD(LX1_BTG_READ_GAP, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_GAP(...) RIZS(LX1_BTG_READ_GAP, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_GAP(fld) RFLD(LX1_BTG_READ_GAP, fld)

typedef union {
	struct {
		unsigned int cur_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_CURR_ADDR_T;
#define LX1_BTG_READ_CURR_ADDRar (0xB801821C)
#define LX1_BTG_READ_CURR_ADDRdv (0x00000000)
#define LX1_BTG_READ_CURR_ADDRrv RVAL(LX1_BTG_READ_CURR_ADDR)
#define RMOD_LX1_BTG_READ_CURR_ADDR(...) RMOD(LX1_BTG_READ_CURR_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_CURR_ADDR(...) RIZS(LX1_BTG_READ_CURR_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_CURR_ADDR(fld) RFLD(LX1_BTG_READ_CURR_ADDR, fld)

typedef union {
	struct {
		unsigned int resp_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_RESP_FAIL_ADDR_T;
#define LX1_BTG_READ_RESP_FAIL_ADDRar (0xB8018220)
#define LX1_BTG_READ_RESP_FAIL_ADDRdv (0x00000000)
#define LX1_BTG_READ_RESP_FAIL_ADDRrv RVAL(LX1_BTG_READ_RESP_FAIL_ADDR)
#define RMOD_LX1_BTG_READ_RESP_FAIL_ADDR(...) RMOD(LX1_BTG_READ_RESP_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_RESP_FAIL_ADDR(...) RIZS(LX1_BTG_READ_RESP_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_RESP_FAIL_ADDR(fld) RFLD(LX1_BTG_READ_RESP_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int peri_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_PERI_FAIL_ADDR_T;
#define LX1_BTG_READ_PERI_FAIL_ADDRar (0xB8018224)
#define LX1_BTG_READ_PERI_FAIL_ADDRdv (0x00000000)
#define LX1_BTG_READ_PERI_FAIL_ADDRrv RVAL(LX1_BTG_READ_PERI_FAIL_ADDR)
#define RMOD_LX1_BTG_READ_PERI_FAIL_ADDR(...) RMOD(LX1_BTG_READ_PERI_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_PERI_FAIL_ADDR(...) RIZS(LX1_BTG_READ_PERI_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_PERI_FAIL_ADDR(fld) RFLD(LX1_BTG_READ_PERI_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int chk_addr:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_VERI_FAIL_ADDR_T;
#define LX1_BTG_READ_VERI_FAIL_ADDRar (0xB8018228)
#define LX1_BTG_READ_VERI_FAIL_ADDRdv (0x00000000)
#define LX1_BTG_READ_VERI_FAIL_ADDRrv RVAL(LX1_BTG_READ_VERI_FAIL_ADDR)
#define RMOD_LX1_BTG_READ_VERI_FAIL_ADDR(...) RMOD(LX1_BTG_READ_VERI_FAIL_ADDR, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_VERI_FAIL_ADDR(...) RIZS(LX1_BTG_READ_VERI_FAIL_ADDR, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_VERI_FAIL_ADDR(fld) RFLD(LX1_BTG_READ_VERI_FAIL_ADDR, fld)

typedef union {
	struct {
		unsigned int max_resp_time:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_MAX_RESP_TIME_T;
#define LX1_BTG_READ_MAX_RESP_TIMEar (0xB801822C)
#define LX1_BTG_READ_MAX_RESP_TIMEdv (0x00000000)
#define LX1_BTG_READ_MAX_RESP_TIMErv RVAL(LX1_BTG_READ_MAX_RESP_TIME)
#define RMOD_LX1_BTG_READ_MAX_RESP_TIME(...) RMOD(LX1_BTG_READ_MAX_RESP_TIME, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_MAX_RESP_TIME(...) RIZS(LX1_BTG_READ_MAX_RESP_TIME, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_MAX_RESP_TIME(fld) RFLD(LX1_BTG_READ_MAX_RESP_TIME, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_IN_VEC0_T;
#define LX1_BTG_READ_IN_VEC0ar (0xB8018230)
#define LX1_BTG_READ_IN_VEC0dv (0x00000000)
#define LX1_BTG_READ_IN_VEC0rv RVAL(LX1_BTG_READ_IN_VEC0)
#define RMOD_LX1_BTG_READ_IN_VEC0(...) RMOD(LX1_BTG_READ_IN_VEC0, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_IN_VEC0(...) RIZS(LX1_BTG_READ_IN_VEC0, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_IN_VEC0(fld) RFLD(LX1_BTG_READ_IN_VEC0, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_IN_VEC1_T;
#define LX1_BTG_READ_IN_VEC1ar (0xB8018234)
#define LX1_BTG_READ_IN_VEC1dv (0x00000000)
#define LX1_BTG_READ_IN_VEC1rv RVAL(LX1_BTG_READ_IN_VEC1)
#define RMOD_LX1_BTG_READ_IN_VEC1(...) RMOD(LX1_BTG_READ_IN_VEC1, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_IN_VEC1(...) RIZS(LX1_BTG_READ_IN_VEC1, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_IN_VEC1(fld) RFLD(LX1_BTG_READ_IN_VEC1, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_IN_VEC2_T;
#define LX1_BTG_READ_IN_VEC2ar (0xB8018238)
#define LX1_BTG_READ_IN_VEC2dv (0x00000000)
#define LX1_BTG_READ_IN_VEC2rv RVAL(LX1_BTG_READ_IN_VEC2)
#define RMOD_LX1_BTG_READ_IN_VEC2(...) RMOD(LX1_BTG_READ_IN_VEC2, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_IN_VEC2(...) RIZS(LX1_BTG_READ_IN_VEC2, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_IN_VEC2(fld) RFLD(LX1_BTG_READ_IN_VEC2, fld)

typedef union {
	struct {
		unsigned int in_vec:32; //0x0
	} f;
	unsigned int v;
} LX1_BTG_READ_IN_VEC_IDX_T;
#define LX1_BTG_READ_IN_VEC_IDXar (0xB801823C)
#define LX1_BTG_READ_IN_VEC_IDXdv (0x00000000)
#define LX1_BTG_READ_IN_VEC_IDXrv RVAL(LX1_BTG_READ_IN_VEC_IDX)
#define RMOD_LX1_BTG_READ_IN_VEC_IDX(...) RMOD(LX1_BTG_READ_IN_VEC_IDX, __VA_ARGS__)
#define RIZS_LX1_BTG_READ_IN_VEC_IDX(...) RIZS(LX1_BTG_READ_IN_VEC_IDX, __VA_ARGS__)
#define RFLD_LX1_BTG_READ_IN_VEC_IDX(fld) RFLD(LX1_BTG_READ_IN_VEC_IDX, fld)

#endif // _REG_MAP_BTG_H
