#ifndef _REG_MAP_BUS_CTRL_H
#define _REG_MAP_BUS_CTRL_H

/*-----------------------------------------------------
 Extraced from file_BUS_CTRL.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int nane:32; //0x52300
	} f;
	unsigned int v;
} NAME_T;
#define NAMEar (0xB8150000)
#define NAMEdv (0x00052300)
#define NAMErv RVAL(NAME)
#define RMOD_NAME(...) RMOD(NAME, __VA_ARGS__)
#define RIZS_NAME(...) RIZS(NAME, __VA_ARGS__)
#define RFLD_NAME(fld) RFLD(NAME, fld)

typedef union {
	struct {
		unsigned int ver:32; //0x300
	} f;
	unsigned int v;
} VER_T;
#define VERar (0xB8150004)
#define VERdv (0x00000300)
#define VERrv RVAL(VER)
#define RMOD_VER(...) RMOD(VER, __VA_ARGS__)
#define RIZS_VER(...) RIZS(VER, __VA_ARGS__)
#define RFLD_VER(fld) RFLD(VER, fld)

typedef union {
	struct {
		unsigned int rev:32; //0x0
	} f;
	unsigned int v;
} REV_T;
#define REVar (0xB8150008)
#define REVdv (0x00000000)
#define REVrv RVAL(REV)
#define RMOD_REV(...) RMOD(REV, __VA_ARGS__)
#define RIZS_REV(...) RIZS(REV, __VA_ARGS__)
#define RFLD_REV(fld) RFLD(REV, fld)

typedef union {
	struct {
		unsigned int inst:32; //0x0
	} f;
	unsigned int v;
} INST_T;
#define INSTar (0xB815000C)
#define INSTdv (0x00000000)
#define INSTrv RVAL(INST)
#define RMOD_INST(...) RMOD(INST, __VA_ARGS__)
#define RIZS_INST(...) RIZS(INST, __VA_ARGS__)
#define RFLD_INST(fld) RFLD(INST, fld)

typedef union {
	struct {
		unsigned int impl_y:32; //0x2016
	} f;
	unsigned int v;
} IMPL_Y_T;
#define IMPL_Yar (0xB8150010)
#define IMPL_Ydv (0x00002016)
#define IMPL_Yrv RVAL(IMPL_Y)
#define RMOD_IMPL_Y(...) RMOD(IMPL_Y, __VA_ARGS__)
#define RIZS_IMPL_Y(...) RIZS(IMPL_Y, __VA_ARGS__)
#define RFLD_IMPL_Y(fld) RFLD(IMPL_Y, fld)

typedef union {
	struct {
		unsigned int impl_d:32; //0x6141952
	} f;
	unsigned int v;
} IMPL_D_T;
#define IMPL_Dar (0xB8150014)
#define IMPL_Ddv (0x06141952)
#define IMPL_Drv RVAL(IMPL_D)
#define RMOD_IMPL_D(...) RMOD(IMPL_D, __VA_ARGS__)
#define RIZS_IMPL_D(...) RIZS(IMPL_D, __VA_ARGS__)
#define RFLD_IMPL_D(fld) RFLD(IMPL_D, fld)

typedef union {
	struct {
		unsigned int dev:32; //0x521439
	} f;
	unsigned int v;
} DEV_T;
#define DEVar (0xB8150018)
#define DEVdv (0x00521439)
#define DEVrv RVAL(DEV)
#define RMOD_DEV(...) RMOD(DEV, __VA_ARGS__)
#define RIZS_DEV(...) RIZS(DEV, __VA_ARGS__)
#define RFLD_DEV(fld) RFLD(DEV, fld)

typedef union {
	struct {
		unsigned int prod_num:32; //0x9310
	} f;
	unsigned int v;
} PROD_NUM_T;
#define PROD_NUMar (0xB815001C)
#define PROD_NUMdv (0x00009310)
#define PROD_NUMrv RVAL(PROD_NUM)
#define RMOD_PROD_NUM(...) RMOD(PROD_NUM, __VA_ARGS__)
#define RIZS_PROD_NUM(...) RIZS(PROD_NUM, __VA_ARGS__)
#define RFLD_PROD_NUM(fld) RFLD(PROD_NUM, fld)

/*-----------------------------------------------------
 Extraced from file_BUS_CTRL_ERR.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int err_bindex:8; //0x0
		unsigned int err_blen:8; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int err_byte:3; //0x0
		unsigned int err_cmd:3; //0x0
		unsigned int err_src:8; //0x0
	} f;
	unsigned int v;
} ERR_PLD0_T;
#define ERR_PLD0ar (0xB8150200)
#define ERR_PLD0dv (0x00000000)
#define ERR_PLD0rv RVAL(ERR_PLD0)
#define RMOD_ERR_PLD0(...) RMOD(ERR_PLD0, __VA_ARGS__)
#define RIZS_ERR_PLD0(...) RIZS(ERR_PLD0, __VA_ARGS__)
#define RFLD_ERR_PLD0(fld) RFLD(ERR_PLD0, fld)

typedef union {
	struct {
		unsigned int err_reqinfo:9; //0x0
		unsigned int mbz_0:4; //0x0
		unsigned int err_size:3; //0x0
		unsigned int err_byteen:16; //0x0
	} f;
	unsigned int v;
} ERR_PLD1_T;
#define ERR_PLD1ar (0xB8150204)
#define ERR_PLD1dv (0x00000000)
#define ERR_PLD1rv RVAL(ERR_PLD1)
#define RMOD_ERR_PLD1(...) RMOD(ERR_PLD1, __VA_ARGS__)
#define RIZS_ERR_PLD1(...) RIZS(ERR_PLD1, __VA_ARGS__)
#define RFLD_ERR_PLD1(fld) RFLD(ERR_PLD1, fld)

typedef union {
	struct {
		unsigned int err_tag:32; //0x0
	} f;
	unsigned int v;
} ERR_TAG_T;
#define ERR_TAGar (0xB8150208)
#define ERR_TAGdv (0x00000000)
#define ERR_TAGrv RVAL(ERR_TAG)
#define RMOD_ERR_TAG(...) RMOD(ERR_TAG, __VA_ARGS__)
#define RIZS_ERR_TAG(...) RIZS(ERR_TAG, __VA_ARGS__)
#define RFLD_ERR_TAG(fld) RFLD(ERR_TAG, fld)

typedef union {
	struct {
		unsigned int err_addr0:32; //0x0
	} f;
	unsigned int v;
} ERR_ADDR0_T;
#define ERR_ADDR0ar (0xB815020C)
#define ERR_ADDR0dv (0x00000000)
#define ERR_ADDR0rv RVAL(ERR_ADDR0)
#define RMOD_ERR_ADDR0(...) RMOD(ERR_ADDR0, __VA_ARGS__)
#define RIZS_ERR_ADDR0(...) RIZS(ERR_ADDR0, __VA_ARGS__)
#define RFLD_ERR_ADDR0(fld) RFLD(ERR_ADDR0, fld)

typedef union {
	struct {
		unsigned int err_addr1:32; //0x0
	} f;
	unsigned int v;
} ERR_ADDR1_T;
#define ERR_ADDR1ar (0xB8150210)
#define ERR_ADDR1dv (0x00000000)
#define ERR_ADDR1rv RVAL(ERR_ADDR1)
#define RMOD_ERR_ADDR1(...) RMOD(ERR_ADDR1, __VA_ARGS__)
#define RIZS_ERR_ADDR1(...) RIZS(ERR_ADDR1, __VA_ARGS__)
#define RFLD_ERR_ADDR1(fld) RFLD(ERR_ADDR1, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int err_code:8; //0x0
	} f;
	unsigned int v;
} ERR_CODE_T;
#define ERR_CODEar (0xB8150230)
#define ERR_CODEdv (0x00000000)
#define ERR_CODErv RVAL(ERR_CODE)
#define RMOD_ERR_CODE(...) RMOD(ERR_CODE, __VA_ARGS__)
#define RIZS_ERR_CODE(...) RIZS(ERR_CODE, __VA_ARGS__)
#define RFLD_ERR_CODE(fld) RFLD(ERR_CODE, fld)

typedef union {
	struct {
		unsigned int mbz_0:31; //0x0
		unsigned int intr_clr:1; //0x0
	} f;
	unsigned int v;
} INTR_CLR_T;
#define INTR_CLRar (0xB815023C)
#define INTR_CLRdv (0x00000000)
#define INTR_CLRrv RVAL(INTR_CLR)
#define RMOD_INTR_CLR(...) RMOD(INTR_CLR, __VA_ARGS__)
#define RIZS_INTR_CLR(...) RIZS(INTR_CLR, __VA_ARGS__)
#define RFLD_INTR_CLR(fld) RFLD(INTR_CLR, fld)

/*-----------------------------------------------------
 Extraced from file_BUS_CTRL_MISC.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int slot_0_owner:2; //0x0
		unsigned int slot_1_owner:2; //0x2
		unsigned int slot_2_owner:2; //0x1
		unsigned int slot_3_owner:2; //0x0
		unsigned int slot_4_owner:2; //0x0
		unsigned int slot_5_owner:2; //0x2
		unsigned int slot_6_owner:2; //0x1
		unsigned int slot_7_owner:2; //0x0
	} f;
	unsigned int v;
} WRR_CTRL_T;
#define WRR_CTRLar (0xB8150400)
#define WRR_CTRLdv (0x00002424)
#define WRR_CTRLrv RVAL(WRR_CTRL)
#define RMOD_WRR_CTRL(...) RMOD(WRR_CTRL, __VA_ARGS__)
#define RIZS_WRR_CTRL(...) RIZS(WRR_CTRL, __VA_ARGS__)
#define RFLD_WRR_CTRL(fld) RFLD(WRR_CTRL, fld)

typedef union {
	struct {
		unsigned int base_addr:32; //0x1F000000
	} f;
	unsigned int v;
} SRAM_REMAP_T;
#define SRAM_REMAPar (0xB8150404)
#define SRAM_REMAPdv (0x1F000000)
#define SRAM_REMAPrv RVAL(SRAM_REMAP)
#define RMOD_SRAM_REMAP(...) RMOD(SRAM_REMAP, __VA_ARGS__)
#define RIZS_SRAM_REMAP(...) RIZS(SRAM_REMAP, __VA_ARGS__)
#define RFLD_SRAM_REMAP(fld) RFLD(SRAM_REMAP, fld)

#endif //_REG_MAP_BUS_CTRL_H
