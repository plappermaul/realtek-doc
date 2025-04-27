#ifndef _MEMCTRL_REG_H_
#define _MEMCTRL_REG_H_

typedef union {
    struct {
        unsigned int cr_update:1; //0
        unsigned int mbz_0:22; //0
        unsigned int flush_fifo:1; //0
        unsigned int mbz_1:3; //0
        unsigned int srxt:1;  //0
        unsigned int dpit:1; //0
        unsigned int btt:1; //1
        unsigned int mbz_2:1; //0
        unsigned int init:1; //0
    } f;
    unsigned int v;
} CCR_T;
#define CCRar           (0xB8143000)
#define CCRdv           (0x0000000C)
#define CCRrv           RVAL(CCR)
#define RMOD_CCR(...)   RMOD(CCR, __VA_ARGS__)
#define RIZS_CCR(...)   RIZS(CCR, __VA_ARGS__)
#define RFLD_CCR(fld)   RFLD(CCR, fld)

typedef union {
    struct {
        unsigned int mbz_0:1; //0
        unsigned int zqc:1; //0
        unsigned int mbz_1:8; //0
        unsigned int bg2:1; //0
        unsigned int mbz_2:1; //0
        unsigned int gear_down:1; //0
        unsigned int par:1; //0
        unsigned int dbi:1; //1
        unsigned int mbz_3:4; //0
        unsigned int rank2:1; //0
        unsigned int mbz_4:1; //0
        unsigned int dfi_rate:3; //0
        unsigned int mbz_5:1; //0
        unsigned int hybr_dq:1; //0
        unsigned int half_dq:1; //0
        unsigned int dq_32:1; //0
        unsigned int ddr:4; //0
    } f;
    unsigned int v;
} DCR_T;
#define DCRar           (0xB8143004)
#define DCRdv           (0x00000000)
#define DCRrv           RVAL(DCR)
#define RMOD_DCR(...)   RMOD(DCR, __VA_ARGS__)
#define RIZS_DCR(...)   RIZS(DCR, __VA_ARGS__)
#define RFLD_DCR(fld)   RFLD(DCR, fld)

typedef union {
    struct {
        unsigned int fpga_dfi:1; //0
        unsigned int fpga_lp:1; //0
        unsigned int mbz_0:5; //0
        unsigned int thpy_rdata:5; //0
        unsigned int tphy_wlat:3; //0
        unsigned int tphy_wdata:5; //1
        unsigned int mbz_1:1; //0
        unsigned int rd_pipe:3; //0
        unsigned int mbz_2:1; //1
        unsigned int half_cs_n:1; //0
        unsigned int trtw_2t_dis:1; //0
        unsigned int two_n_pre_en:1; //1
        unsigned int stc_odt:1; //0
        unsigned int stc_cke:1; //0
        unsigned int mbz_3:2; //0
    } f;
    unsigned int v;
} IOCR_T;
#define IOCRar           (0xB8143008)
#define IOCRdv           (0x00101010)
#define IOCRrv           RVAL(IOCR)
#define RMOD_IOCR(...)   RMOD(IOCR, __VA_ARGS__)
#define RIZS_IOCR(...)   RIZS(IOCR, __VA_ARGS__)
#define RFLD_IOCR(fld)   RFLD(IOCR, fld)

typedef union {
    struct {
        unsigned int mbz_0:21; //0
        unsigned int bstc_idle:1; //0
        unsigned int mbo_0:1; //1
        unsigned int mem_idle:1; //1
        unsigned int mbz_1:8; //0
    } f;
    unsigned int v;
} CSR_T;
#define CSRar           (0xB814300C)
#define CSRdv           (0x00000600)
#define CSRrv           RVAL(CSR)
#define RMOD_CSR(...)   RMOD(CSR, __VA_ARGS__)
#define RIZS_CSR(...)   RIZS(CSR, __VA_ARGS__)
#define RFLD_CSR(fld)   RFLD(CSR, fld)

typedef union {
    struct {
        unsigned int zqcl_inv:3; //0
        unsigned int ref_dis:1; //0
        unsigned int ref_num:4; //0x8
        unsigned int tref:16; //0xF00
        unsigned int trfc:8; //0x72
    } f;
    unsigned int v;
}DRR_T;
#define DRRar           (0xB8143010)
#define DRRdv           (0x080F0072)
#define DRRrv           RVAL(DRR)
#define RMOD_DRR(...)   RMOD(DRR, __VA_ARGS__)
#define RIZS_DRR(...)   RIZS(DRR, __VA_ARGS__)
#define RFLD_DRR(fld)   RFLD(DRR, fld)

typedef union {
    struct {
        unsigned int tzqcs:8; //0
        unsigned int txp:1; //0
        unsigned int tcke:6; //0
        unsigned int trtp:4; //3
        unsigned int twr:4; //4
        unsigned int tras:5; //5
        unsigned int trp:4; //6
    } f;
    unsigned int v;
}TPR0_T;
#define TPR0ar           (0xB8143014)
#define TPR0dv           (0x00006856)
#define TPR0rv           RVAL(TPR0)
#define RMOD_TPR0(...)   RMOD(TPR0, __VA_ARGS__)
#define RIZS_TPR0(...)   RIZS(TPR0, __VA_ARGS__)
#define RFLD_TPR0(fld)   RFLD(TPR0, fld)

typedef union {
    struct {
        unsigned int mbz_0:3; //0
        unsigned int tfaw:5; //0
        unsigned int trtw:4; //0
        unsigned int twtr:3; //3
        unsigned int tccd:3; //1
        unsigned int trcd:4; //F
        unsigned int trc:6; //8
        unsigned int trrd:4; //2
    } f;
    unsigned int v;
}TPR1_T;
#define TPR1ar           (0xB8143018)
#define TPR1dv           (0x00067C82)
#define TPR1rv           RVAL(TPR1)
#define RMOD_TPR1(...)   RMOD(TPR1, __VA_ARGS__)
#define RIZS_TPR1(...)   RIZS(TPR1, __VA_ARGS__)
#define RFLD_TPR1(fld)   RFLD(TPR1, fld)

typedef union {
    struct {
        unsigned int rst_tus:10; //0x80
        unsigned int tus:10; //0x80
        unsigned int tns:4; //0x2
        unsigned int tmrd:4; //0x2
        unsigned int init_ref_num:4; //0x2
    } f;
    unsigned int v;
}TPR2_T;
#define TPR2ar           (0xB814301C)
#define TPR2dv           (0xC31E8341)
#define TPR2rv           RVAL(TPR2)
#define RMOD_TPR2(...)   RMOD(TPR2, __VA_ARGS__)
#define RIZS_TPR2(...)   RIZS(TPR2, __VA_ARGS__)
#define RFLD_TPR2(fld)   RFLD(TPR2, fld)

typedef union {
    struct {
        unsigned int tccd_r:3; //0
        unsigned int mbz_0:9; //0
        unsigned int twtr_s:3; //0
        unsigned int tccd_s:3; //0
        unsigned int mbz_1:7; //0
        unsigned int dpin_cmd_lat:3; //0
        unsigned int mbz_2:4; //0
    } f;
    unsigned int v;
}TPR3_T;
#define TPR3ar           (0xB8143020)
#define TPR3dv           (0x00000000)
#define TPR3rv           RVAL(TPR3)
#define RMOD_TPR3(...)   RMOD(TPR3, __VA_ARGS__)
#define RIZS_TPR3(...)   RIZS(TPR3, __VA_ARGS__)
#define RFLD_TPR3(fld)   RFLD(TPR3, fld)

typedef union {
    struct {
        unsigned int cs_1:1; //1
        unsigned int mbz_0:4; //0
        unsigned int parity:1; //0
        unsigned int act_n:1; //1
        unsigned int cs_n:1; //1
        unsigned int ras_n:1; //1
        unsigned int cas_n:1; //1
        unsigned int we_n:1; //1
        unsigned int ba:4; //0
        unsigned int addr:17; //0
    } f;
    unsigned int v;
}CMD_DPIN_NDGE_T;
#define CMD_DPIN_NDGEar           (0xB8143024)
#define CMD_DPIN_NDGEdv           (0x83E00000)
#define CMD_DPIN_NDGErv           RVAL(CMD_DPIN_NDGE)
#define RMOD_CMD_DPIN_NDGE(...)   RMOD(CMD_DPIN_NDGE, __VA_ARGS__)
#define RIZS_CMD_DPIN_NDGE(...)   RIZS(CMD_DPIN_NDGE, __VA_ARGS__)
#define RFLD_CMD_DPIN_NDGE(fld)   RFLD(CMD_DPIN_NDGE, fld)

typedef union {
    struct {
        unsigned int cs_1:1; //1
        unsigned int mbz_0:4; //0
        unsigned int parity:1; //0
        unsigned int act_n:1; //1
        unsigned int cs_n:1; //1
        unsigned int ras_n:1; //1
        unsigned int cas_n:1; //1
        unsigned int we_n:1; //1
        unsigned int ba:4; //0
        unsigned int addr:17; //0
    } f;
    unsigned int v;
}CMD_DPIN_T;
#define CMD_DPINar           (0xB8143028)
#define CMD_DPINdv           (0x83E00000)
#define CMD_DPINrv           RVAL(CMD_DPIN)
#define RMOD_CMD_DPIN(...)   RMOD(CMD_DPIN, __VA_ARGS__)
#define RIZS_CMD_DPIN(...)   RIZS(CMD_DPIN, __VA_ARGS__)
#define RFLD_CMD_DPIN(fld)   RFLD(CMD_DPIN, fld)

typedef union {
    struct {
        unsigned int odt_1:1; //0
        unsigned int cke_1:1; //1
        unsigned int rst_n_1:1; //1
        unsigned int mbz_0:24; //0
        unsigned int alert_n:1; //1
        unsigned int ten:1; //0
        unsigned int odt:1; //0
        unsigned int cke:1; //1
        unsigned int rst_n:1; //1
    } f;
    unsigned int v;
}TIE_DPIN_T;
#define TIE_DPINar           (0xB814302C)
#define TIE_DPINdv           (0x60000013)
#define TIE_DPINrv           RVAL(TIE_DPIN)
#define RMOD_TIE_DPIN(...)   RMOD(TIE_DPIN, __VA_ARGS__)
#define RIZS_TIE_DPIN(...)   RIZS(TIE_DPIN, __VA_ARGS__)
#define RFLD_TIE_DPIN(fld)   RFLD(TIE_DPIN, fld)

typedef union {
    struct {
        unsigned int mbz_0:17; //0
        unsigned int add_lat:5; //1
        unsigned int rd_lat:5; //1
        unsigned int wr_lat:5; //0
    } f;
    unsigned int v;
}MR_INFO_T;
#define MR_INFOar           (0xB8143030)
#define MR_INFOdv           (0x00000000)
#define MR_INFOrv           RVAL(MR_INFO)
#define RMOD_MR_INFO(...)   RMOD(MR_INFO, __VA_ARGS__)
#define RIZS_MR_INFO(...)   RIZS(MR_INFO, __VA_ARGS__)
#define RFLD_MR_INFO(fld)   RFLD(MR_INFO, fld)

typedef union {
    struct {
        unsigned int mr0:32; //0
    } f;
    unsigned int v;
}MR0_T;
#define MR0ar           (0xB8143034)
#define MR0dv           (0x00000000)        // default, DLL on and fast exit, turn off test mode
#define MR0rv           RVAL(MR0)
#define RMOD_MR0(...)   RMOD(MR0, __VA_ARGS__)
#define RIZS_MR0(...)   RIZS(MR0, __VA_ARGS__)
#define RFLD_MR0(fld)   RFLD(MR0, fld)

typedef union {
    struct {
        unsigned int mr1:32; //0
    } f;
    unsigned int v;
}MR1_T;
#define MR1ar           (0xB8143038)
#define MR1dv           (0x00000000)
#define MR1rv           RVAL(MR1)
#define RMOD_MR1(...)   RMOD(MR1, __VA_ARGS__)
#define RIZS_MR1(...)   RIZS(MR1, __VA_ARGS__)
#define RFLD_MR1(fld)   RFLD(MR1, fld)

typedef union {
    struct {
        unsigned int mr2:32; //0
        //unsigned int mbz0:21;
        //unsigned int rtt_wr:2;
        //unsigned int mbz1:1;
        //unsigned int srt:1;
        //unsigned int asr:1;
        //unsigned int cwl:3;
        //unsigned int pasr:3;
    } f;
    unsigned int v;
}MR2_T;
#define MR2ar           (0xB814303C)
#define MR2dv           (0x00000000)
#define MR2rv           RVAL(MR2)
#define RMOD_MR2(...)   RMOD(MR2, __VA_ARGS__)
#define RIZS_MR2(...)   RIZS(MR2, __VA_ARGS__)
#define RFLD_MR2(fld)   RFLD(MR2, fld)

typedef union {
    struct {
        unsigned int mr3:32; //0
    } f;
    unsigned int v;
}MR3_T;
#define MR3ar           (0xB8143040)
#define MR3dv           (0x00000000)
#define MR3rv           RVAL(MR3)
#define RMOD_MR3(...)   RMOD(MR3, __VA_ARGS__)
#define RIZS_MR3(...)   RIZS(MR3, __VA_ARGS__)
#define RFLD_MR3(fld)   RFLD(MR3, fld)

typedef union {
    struct {
        unsigned int mr4:32; //0
    } f;
    unsigned int v;
}MR4_T;
#define MR4ar           (0xB8143044)
#define MR4dv           (0x00000000)
#define MR4rv           RVAL(MR4)
#define RMOD_MR4(...)   RMOD(MR4, __VA_ARGS__)
#define RIZS_MR4(...)   RIZS(MR4, __VA_ARGS__)
#define RFLD_MR4(fld)   RFLD(MR4, fld)

typedef union {
    struct {
        unsigned int mr5:32; //0
    } f;
    unsigned int v;
}MR5_T;
#define MR5ar           (0xB8143048)
#define MR5dv           (0x00000400)
#define MR5rv           RVAL(MR5)
#define RMOD_MR5(...)   RMOD(MR5, __VA_ARGS__)
#define RIZS_MR5(...)   RIZS(MR5, __VA_ARGS__)
#define RFLD_MR5(fld)   RFLD(MR5, fld)

typedef union {
    struct {
        unsigned int mr6:32; //0
    } f;
    unsigned int v;
}MR6_T;
#define MR6ar           (0xB814304C)
#define MR6dv           (0x00000000)
#define MR6rv           RVAL(MR6)
#define RMOD_MR6(...)   RMOD(MR6, __VA_ARGS__)
#define RIZS_MR6(...)   RIZS(MR6, __VA_ARGS__)
#define RFLD_MR6(fld)   RFLD(MR6, fld)

// BSTC Function
typedef union {
    struct {
        unsigned int mbz_0:2; //0
        unsigned int rd_ex_cnt:14; //0
        unsigned int mbz_1:1; //0
        unsigned int reload_sram:3; //0
        unsigned int fus_rd:1; //0
        unsigned int fus_rg:1; //0
        unsigned int fus_wd:1; //0
        unsigned int fus_cmd:1; //0
        unsigned int mbz_2:2; //0
        unsigned int crr:1; //0
        unsigned int at_err_stop:1; //0
        unsigned int dis_msk:1; //0
        unsigned int loop:1; //0
        unsigned int cmp:1; //0
        unsigned int stop:1; //0
    } f;
    unsigned int v;
}BCR_T;

#define BCRar           (0xB81430D0)
#define BCRdv           (0x00000000)
#define BCRrv           RVAL(BCR)
#define RMOD_BCR(...)   RMOD(BCR, __VA_ARGS__)
#define RIZS_BCR(...)   RIZS(BCR, __VA_ARGS__)
#define RFLD_BCR(fld)   RFLD(BCR, fld)

typedef union {
    struct {
        unsigned int mbz_0:16; //0
        unsigned int loop_cnt:16; //0
    } f;
    unsigned int v;
}BCT_T;
#define BCTar           (0xB81430D4)
#define BCTdv           (0x00000000)
#define BCTrv           RVAL(BCT)
#define RMOD_BCT(...)   RMOD(BCT, __VA_ARGS__)
#define RIZS_BCT(...)   RIZS(BCT, __VA_ARGS__)
#define RFLD_BCT(fld)   RFLD(BCT, fld)

typedef union {
    struct {
        unsigned int cmp_mbit:32; //0
    } f;
    unsigned int v;
}BCM_T;
#define BCMar           (0xB81430D8)
#define BCMdv           (0x00000000)
#define BCMrv           RVAL(BCM)
#define RMOD_BCM(...)   RMOD(BCM, __VA_ARGS__)
#define RIZS_BCM(...)   RIZS(BCM, __VA_ARGS__)
#define RFLD_BCM(fld)   RFLD(BCM, fld)

typedef union {
    struct {
        unsigned int err_cnt:16; //0
        unsigned int rd_in_st:1; //0
        unsigned int mbz_0:1; //0
        unsigned int err_fst_th:14; //0
    } f;
    unsigned int v;
}BST_T;
#define BSTar           (0xB81430DC)
#define BSTdv           (0x00000000)
#define BSTrv           RVAL(BST)
#define RMOD_BST(...)   RMOD(BST, __VA_ARGS__)
#define RIZS_BST(...)   RIZS(BST, __VA_ARGS__)
#define RFLD_BST(fld)   RFLD(BST, fld)

typedef union {
    struct {
        unsigned int mbz_0:2; //0
        unsigned int wd_cnt:14; //0
        unsigned int mbz_1:4; //0
        unsigned int cmd_cnt:12; //0
    } f;
    unsigned int v;
}BSRAM0_T;
#define BSRAM0ar           (0xB81430E0)
#define BSRAM0dv           (0x00000000)
#define BSRAM0rv           RVAL(BSRAM0)
#define RMOD_BSRAM0(...)   RMOD(BSRAM0, __VA_ARGS__)
#define RIZS_BSRAM0(...)   RIZS(BSRAM0, __VA_ARGS__)
#define RFLD_BSRAM0(fld)   RFLD(BSRAM0, fld)

typedef union {
    struct {
        unsigned int mbz_0:2; //0
        unsigned int rd_cnt:14; //0
        unsigned int mbz_1:1; //0
        unsigned int rg_cnt:15; //0
    } f;
    unsigned int v;
}BSRAM1_T;
#define BSRAM1ar           (0xB81430E4)
#define BSRAM1dv           (0x00000000)
#define BSRAM1rv           RVAL(BSRAM1)
#define RMOD_BSRAM1(...)   RMOD(BSRAM1, __VA_ARGS__)
#define RIZS_BSRAM1(...)   RIZS(BSRAM1, __VA_ARGS__)
#define RFLD_BSRAM1(fld)   RFLD(BSRAM1, fld)

typedef union {
    struct {
        unsigned int err_bit:32; //0
    } f;
    unsigned int v;
}BER_T;
#define BERar           (0xB81430E8)
#define BERdv           (0x00000000)
#define BERrv           RVAL(BER)
#define RMOD_BER(...)   RMOD(BER, __VA_ARGS__)
#define RIZS_BER(...)   RIZS(BER, __VA_ARGS__)
#define RFLD_BER(fld)   RFLD(BER, fld)

typedef union {
    struct {
        unsigned int svn_num:16; //0
        unsigned int release_date:16; //0
    } f;
    unsigned int v;
}PCTL_SVN_ID_T;
#define PCTL_SVN_IDar           (0xB81430F4)
#define PCTL_SVN_IDdv           (0x81050719)
#define PCTL_SVN_IDrv           RVAL(PCTL_SVN_ID)
#define RMOD_PCTL_SVN_ID(...)   RMOD(PCTL_SVN_ID, __VA_ARGS__)
#define RIZS_PCTL_SVN_ID(...)   RIZS(PCTL_SVN_ID, __VA_ARGS__)
#define RFLD_PCTL_SVN_ID(fld)   RFLD(PCTL_SVN_ID, fld)

typedef union {
    struct {
        unsigned int cr_ver:16; //0
        unsigned int cr_pctl_def:16; //0
    } f;
    unsigned int v;
}PCTL_IDR_T;
#define PCTL_IDRar           (0xB81430F8)
#define PCTL_IDRdv           (0x00023103)
#define PCTL_IDRrv           RVAL(PCTL_IDR)
#define RMOD_PCTL_IDR(...)   RMOD(PCTL_IDR, __VA_ARGS__)
#define RIZS_PCTL_IDR(...)   RIZS(PCTL_IDR, __VA_ARGS__)
#define RFLD_PCTL_IDR(fld)   RFLD(PCTL_IDR, fld)

// PHY
typedef union {
    struct {
        unsigned int dly0:32; //0
    } f;
    unsigned int v;
}DLY0_T;
#define DLY0ar           (0xB8143100)
#define DLY0dv           (0x00000000)
#define DLY0rv           RVAL(DLY0)
#define RMOD_DLY0(...)   RMOD(DLY0, __VA_ARGS__)
#define RIZS_DLY0(...)   RIZS(DLY0, __VA_ARGS__)
#define RFLD_DLY0(fld)   RFLD(DLY0, fld)

typedef union {
    struct {
        unsigned int dly1:32; //0
    } f;
    unsigned int v;
}DLY1_T;
#define DLY1ar           (0xB8143104)
#define DLY1dv           (0x00000000)
#define DLY1rv           RVAL(DLY1)
#define RMOD_DLY1(...)   RMOD(DLY1, __VA_ARGS__)
#define RIZS_DLY1(...)   RIZS(DLY1, __VA_ARGS__)
#define RFLD_DLY1(fld)   RFLD(DLY1, fld)

typedef union {
    struct {
        unsigned int dly_clk:32; //0
    } f;
    unsigned int v;
}DLY_CLK_T;
#define DLY_CLKar           (0xB8143108)
#define DLY_CLKdv           (0x00000000)
#define DLY_CLKrv           RVAL(DLY_CLK)
#define RMOD_DLY_CLK(...)   RMOD(DLY_CLK, __VA_ARGS__)
#define RIZS_DLY_CLK(...)   RIZS(DLY_CLK, __VA_ARGS__)
#define RFLD_DLY_CLK(fld)   RFLD(DLY_CLK, fld)

typedef union {
    struct {
        unsigned int dly_st:32; //0
    } f;
    unsigned int v;
}DLY_ST_T;
#define DLY_STar           (0xB8143108)
#define DLY_STdv           (0x00000000)
#define DLY_STrv           RVAL(DLY_ST)
#define RMOD_DLY_ST(...)   RMOD(DLY_ST, __VA_ARGS__)
#define RIZS_DLY_ST(...)   RIZS(DLY_ST, __VA_ARGS__)
#define RFLD_DLY_ST(fld)   RFLD(DLY_ST, fld)


typedef union {
    struct {
        unsigned int mbz_0:20; //0
        unsigned int hybr_size:2; //0
        unsigned int mbz_1:2; //0
        unsigned int bst_size:2; //0
        unsigned int bank_size:2; //0
        unsigned int page_size:4; //0
    } f;
    unsigned int v;
}MISC_T;
#define MISCar           (0xB8143224)
#define MISCdv           (0x00000002)
#define MISCrv           RVAL(MISC)
#define RMOD_MISC(...)   RMOD(MISC, __VA_ARGS__)
#define RIZS_MISC(...)   RIZS(MISC, __VA_ARGS__)
#define RFLD_MISC(fld)   RFLD(MISC, fld)

typedef union {
    struct {
        unsigned int cr_wrap_idr:16; //0x2
        unsigned int cr_wrap_def:16; //0x310
    } f;
    unsigned int v;
}OCP_WRAP_IDR_T;
#define OCP_WRAP_IDRar           (0xB81432A0)
#define OCP_WRAP_IDRdv           (0x00020310)
#define OCP_WRAP_IDRrv           RVAL(OCP_WRAP_IDR)
#define RMOD_OCP_WRAP_IDR(...)   RMOD(OCP_WRAP_IDR, __VA_ARGS__)
#define RIZS_OCP_WRAP_IDR(...)   RIZS(OCP_WRAP_IDR, __VA_ARGS__)
#define RFLD_OCP_WRAP_IDR(fld)   RFLD(OCP_WRAP_IDR, fld)

typedef union {
    struct {
        unsigned int svn_num:16; //conf
        unsigned int release_date:16; //conf
    } f;
    unsigned int v;
}OCP_WRAP_VERSION_T;
#define OCP_WRAP_VERSIONar           (0xB81432A4)
#define OCP_WRAP_VERSIONdv           (0x81050719)
#define OCP_WRAP_VERSIONrv           RVAL(OCP_WRAP_VERSION)
#define RMOD_OCP_WRAP_VERSION(...)   RMOD(OCP_WRAP_VERSION, __VA_ARGS__)
#define RIZS_OCP_WRAP_VERSION(...)   RIZS(OCP_WRAP_VERSION, __VA_ARGS__)
#define RFLD_OCP_WRAP_VERSION(fld)   RFLD(OCP_WRAP_VERSION, fld)

// schedule
// profile
#endif  // _MEMCTRL_REG_H_
