#ifndef _REG_MAP_DINT_H
#define _REG_MAP_DINT_H

/*-----------------------------------------------------
 Extraced from file_DLY_INTC.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int iti_trig:1; //0x0
		unsigned int mbz_0:27; //0x0
		unsigned int delayed_ip_sel:4; //0x0
	} f;
	unsigned int v;
} TC4_DLY_INTR_T;
#define TC4_DLY_INTRar (0xB8003090)
#define TC4_DLY_INTRdv (0x00000000)
#define TC4_DLY_INTRrv RVAL(TC4_DLY_INTR)
#define RMOD_TC4_DLY_INTR(...) RMOD(TC4_DLY_INTR, __VA_ARGS__)
#define RIZS_TC4_DLY_INTR(...) RIZS(TC4_DLY_INTR, __VA_ARGS__)
#define RFLD_TC4_DLY_INTR(fld) RFLD(TC4_DLY_INTR, fld)

typedef union {
	struct {
		unsigned int iti_trig:1; //0x0
		unsigned int mbz_0:27; //0x0
		unsigned int delayed_ip_sel:4; //0x0
	} f;
	unsigned int v;
} TC5_DLY_INTR_T;
#define TC5_DLY_INTRar (0xB8003094)
#define TC5_DLY_INTRdv (0x00000000)
#define TC5_DLY_INTRrv RVAL(TC5_DLY_INTR)
#define RMOD_TC5_DLY_INTR(...) RMOD(TC5_DLY_INTR, __VA_ARGS__)
#define RIZS_TC5_DLY_INTR(...) RIZS(TC5_DLY_INTR, __VA_ARGS__)
#define RFLD_TC5_DLY_INTR(fld) RFLD(TC5_DLY_INTR, fld)

typedef union {
	struct {
		unsigned int iti_trig:1; //0x0
		unsigned int mbz_0:27; //0x0
		unsigned int delayed_ip_sel:4; //0x0
	} f;
	unsigned int v;
} TC6_DLY_INTR_T;
#define TC6_DLY_INTRar (0xB8003098)
#define TC6_DLY_INTRdv (0x00000000)
#define TC6_DLY_INTRrv RVAL(TC6_DLY_INTR)
#define RMOD_TC6_DLY_INTR(...) RMOD(TC6_DLY_INTR, __VA_ARGS__)
#define RIZS_TC6_DLY_INTR(...) RIZS(TC6_DLY_INTR, __VA_ARGS__)
#define RFLD_TC6_DLY_INTR(fld) RFLD(TC6_DLY_INTR, fld)

#endif // _REG_MAP_DINT_H
