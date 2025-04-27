#include <util.h>

typedef union {
	struct {
		unsigned int mbz:6; //0
		unsigned int data_in:20; //0
		unsigned int wire_sel:1; //0
		unsigned int ro_sel:3; //0
		unsigned int dss_en:1; //0
		unsigned int dss_rst_n:1; //0
	} f;
	unsigned int v;
} DSSCTRL_T;

typedef union {
	struct {
		unsigned int mbz:1; //0
		unsigned int ready:1; //0
		unsigned int wsort_go:1; //0
		unsigned int mbz2:9; //0
		unsigned int count_out:20; //0
	} f;
	unsigned int v;
} DSSDATA_T;

typedef union {
	struct {
		unsigned int mbz0:16; //0
		unsigned int dss_dbgo:16; //0
	} f;
	unsigned int v;
} DSSDBG_T;

#define DSS0_CTRL   (0xB8000250)
#define DSS0_DATA   (0xB8000260)
#define DSS0_DEBG   (0xB8000264)
#define DSS1_CTRL   (0xB800025C)
#define DSS1_DATA   (0xB8000268)//(0xB8000280)    // original 0xB8000268
#define DSS1_DEBG   (0xB800026C)//(0xB8000284)    // original 0xB800026C

void dss_test(void)
{
    unsigned int ws, rs;
    DSSCTRL_T dctrl;
    DSSDATA_T d0data, d1data;
    //DSSDBG_T  d0dbg, d1dbg;
    
    for (ws=0; ws<2; ws++) {
        for (rs=1; rs<6; rs++) {
            printf("DD: ro_sel=%d, wire_sel=%d\n", rs, ws);
            // init: dss_en is driven low and activate reset
            REG32(DSS1_CTRL)=0x0;
            REG32(DSS1_CTRL)=0x0;
            REG32(DSS1_DATA)=0x0;   // work around
            
            dctrl.v = 0;
            // config dss and enable dss
            dctrl.f.data_in = 0xfff;
            dctrl.f.wire_sel = ws;
            dctrl.f.ro_sel = rs;
            dctrl.f.dss_en = 1;
            dctrl.f.dss_rst_n = 1;
            REG32(DSS0_CTRL) = dctrl.v;
            REG32(DSS1_CTRL) = dctrl.v;
            
            unsigned int cnt=0;
            // wait ready
            while (1!=(REG32(DSS1_DATA)>>30)) {
                if(++cnt>0x1000) {
                    puts("WW: DSS polling timeout!!\n");
                    return;
                }
            } 
            
            d0data.v = REG32(DSS0_DATA);
            d1data.v = REG32(DSS1_DATA);
            //d0dbg.v  = REG32(DSS0_DEBG);
            //d1dbg.v  = REG32(DSS1_DEBG);
            
            printf("DD: count_out, dss0=%6d, dss1=%6d\n", d0data.f.count_out, d1data.f.count_out);
            //printf("DD: wsort_go , dss0=%6d, dss1=%6d\n", d0data.f.wsort_go, d1data.f.wsort_go);
            //printf("DD: dss_dbgo , dss0=%6d, dss1=%6d\n", d0dbg.f.dss_dbgo, d1dbg.f.dss_dbgo);
        }
    }
}

REG_INIT_FUNC(dss_test, 22);
