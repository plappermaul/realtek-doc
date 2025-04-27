#ifndef _REG_MAP_IPSEC_H
#define _REG_MAP_IPSEC_H

/*-----------------------------------------------------
 Extraced from file_IPSEC_CRYPTO.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int sdsa_sdca:32; //0x9FC0FFB0
	} f;
	unsigned int v;
} IPSEC_SRC_DESC_ADDR_T;
#define IPSEC_SRC_DESC_ADDRar (0xB800C000)
#define IPSEC_SRC_DESC_ADDRdv (0x9FC0FFB0)
#define IPSEC_SRC_DESC_ADDRrv RVAL(IPSEC_SRC_DESC_ADDR)
#define RMOD_IPSEC_SRC_DESC_ADDR(...) RMOD(IPSEC_SRC_DESC_ADDR, __VA_ARGS__)
#define RIZS_IPSEC_SRC_DESC_ADDR(...) RIZS(IPSEC_SRC_DESC_ADDR, __VA_ARGS__)
#define RFLD_IPSEC_SRC_DESC_ADDR(fld) RFLD(IPSEC_SRC_DESC_ADDR, fld)

typedef union {
	struct {
		unsigned int ddsa_ddca:32; //0x9FC0FFE0
	} f;
	unsigned int v;
} IPSEC_DST_DESC_ADDR_T;
#define IPSEC_DST_DESC_ADDRar (0xB800C004)
#define IPSEC_DST_DESC_ADDRdv (0x9FC0FFE0)
#define IPSEC_DST_DESC_ADDRrv RVAL(IPSEC_DST_DESC_ADDR)
#define RMOD_IPSEC_DST_DESC_ADDR(...) RMOD(IPSEC_DST_DESC_ADDR, __VA_ARGS__)
#define RIZS_IPSEC_DST_DESC_ADDR(...) RIZS(IPSEC_DST_DESC_ADDR, __VA_ARGS__)
#define RFLD_IPSEC_DST_DESC_ADDR(fld) RFLD(IPSEC_DST_DESC_ADDR, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int sdue:1; //0x0
		unsigned int sdle:1; //0x0
		unsigned int ddue:1; //0x0
		unsigned int ddok:1; //0x0
		unsigned int dabf:1; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int dbgs:1; //0x0
		unsigned int mbz_2:6; //0x0
		unsigned int poll:1; //0x0
		unsigned int srst:1; //0x0
	} f;
	unsigned int v;
} IPSEC_CMD_STS_T;
#define IPSEC_CMD_STSar (0xB800C008)
#define IPSEC_CMD_STSdv (0x00000000)
#define IPSEC_CMD_STSrv RVAL(IPSEC_CMD_STS)
#define RMOD_IPSEC_CMD_STS(...) RMOD(IPSEC_CMD_STS, __VA_ARGS__)
#define RIZS_IPSEC_CMD_STS(...) RIZS(IPSEC_CMD_STS, __VA_ARGS__)
#define RFLD_IPSEC_CMD_STS(fld) RFLD(IPSEC_CMD_STS, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int sduem:1; //0x0
		unsigned int sdlem:1; //0x0
		unsigned int dduem:1; //0x0
		unsigned int ddokm:1; //0x1
		unsigned int dabfm:1; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int lbkm:1; //0x0
		unsigned int sawb:1; //0x1
		unsigned int cke:1; //0x0
		unsigned int dmbs:3; //0x2
		unsigned int smbs:3; //0x2
	} f;
	unsigned int v;
} IPSEC_CTRL_T;
#define IPSEC_CTRLar (0xB800C00C)
#define IPSEC_CTRLdv (0x00001092)
#define IPSEC_CTRLrv RVAL(IPSEC_CTRL)
#define RMOD_IPSEC_CTRL(...) RMOD(IPSEC_CTRL, __VA_ARGS__)
#define RIZS_IPSEC_CTRL(...) RIZS(IPSEC_CTRL, __VA_ARGS__)
#define RFLD_IPSEC_CTRL(fld) RFLD(IPSEC_CTRL, fld)

#endif // _REG_MAP_IPSEC_H
