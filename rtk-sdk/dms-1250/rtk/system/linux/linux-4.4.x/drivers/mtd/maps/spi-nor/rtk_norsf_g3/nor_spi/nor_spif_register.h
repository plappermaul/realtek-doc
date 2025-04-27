#ifndef _REGISTER_MAP_NOR_G2_H_
#define _REGISTER_MAP_NOR_G2_H_

#include <reg_map_util.h>

#define reg_soc_write(ADDR, PV) do { \
        *((volatile u32_t *)(ADDR)) = *((volatile u32_t *)PV); \
    } while (0)

#define reg_soc_read(ADDR, PV) do { \
        *((volatile u32_t *)PV) = *((volatile u32_t *)(ADDR)); \
    } while (0)

#define NOR_G2_SFCRr  (0xB8001200)
#define NOR_G2_SFCR2r (0xB8001204)
#define NOR_G2_SFCSRr (0xB8001208)
#define NOR_G2_SFDRr  (0xB800120C)
#define NOR_G2_SFDR2r (0xB8001210)
#define NOR_G2_SFRDRr (0xB8001218)
#define NOR_G2_SFPPRr (0xB800121C)

/*-----------------------------------------------------
 Extraced from file_MEM_SPI_FLASH.xml
-----------------------------------------------------*/
typedef union {
    struct {
        unsigned int spi_clk_div:3; //7
        unsigned int rbo:1; //1
        unsigned int wbo:1; //1
        unsigned int spi_tcs:5; //31
        unsigned int no_use21:1; //0
        unsigned int mbz_0:8; //0
        unsigned int ocp0_frq_slower:1; //0
        unsigned int mbz_1:2; //0
        unsigned int hw_reset_en:1; //1
        unsigned int rst_cmd_dis:1; //1
        unsigned int spi_tchsh:4; //4
        unsigned int spi_tslch:4; //4
    } f;
    unsigned int v;
} SFCR_T;
#define SFCRrv (*((regval)0xb8001200))
#define SFCRdv (0xffc00344)
#define RMOD_SFCR(...) rset(SFCR, SFCRrv, __VA_ARGS__)
#define RIZS_SFCR(...) rset(SFCR, 0, __VA_ARGS__)
#define RFLD_SFCR(fld) (*((const volatile SFCR_T *)0xb8001200)).f.fld

typedef union {
    struct {
        unsigned int sfcmd:8; //3
        unsigned int sfsize:3; //7
        unsigned int rd_opt:1; //0
        unsigned int cmd_io:2; //0
        unsigned int addr_io:2; //0
        unsigned int dummy_cycles:3; //0
        unsigned int data_io:2; //0
        unsigned int hold_till_sfdr2:1; //0
        unsigned int mmio_4b_addr_en:1; //0
        unsigned int mbz_1:8; //0
        unsigned int pio_en:1; //0
    } f;
    unsigned int v;
} SFCR2_T;
#define SFCR2rv (*((regval)0xb8001204))
#define SFCR2dv (0x03e00000)
#define RMOD_SFCR2(...) rset(SFCR2, SFCR2rv, __VA_ARGS__)
#define RIZS_SFCR2(...) rset(SFCR2, 0, __VA_ARGS__)
#define RFLD_SFCR2(fld) (*((const volatile SFCR2_T *)0xb8001204)).f.fld

typedef union {
    struct {
        unsigned int spi_csb0:1; //1
        unsigned int spi_csb1:1; //1
        unsigned int len:2; //3
        unsigned int spi_rdy:1; //1
        unsigned int io_width:2; //0
        unsigned int chip_sel:1; //0
        unsigned int cmd_byte:8; //0
        unsigned int no_use15:4; //0
        unsigned int spi_csb0_sts:1; //1
        unsigned int spi_csb1_sts:1; //1
        unsigned int no_use09:5; //0
        unsigned int spi_idle:1; //0
        unsigned int mbz_0:3; //0
        unsigned int mio_pio_err:1; //0
    } f;
    unsigned int v;
} SFCSR_T;
#define SFCSRrv (*((regval)0xb8001208))
#define SFCSRdv (0xf8000c00)
#define RMOD_SFCSR(...) rset(SFCSR, SFCSRrv, __VA_ARGS__)
#define RIZS_SFCSR(...) rset(SFCSR, 0, __VA_ARGS__)
#define RFLD_SFCSR(fld) (*((const volatile SFCSR_T *)0xb8001208)).f.fld

typedef union {
    struct {
        unsigned int data3:8; //0
        unsigned int data2:8; //0
        unsigned int data1:8; //0
        unsigned int data0:8; //0
    } f;
    unsigned int v;
} SFDR_T;
#define SFDRrv (*((regval)0xb800120c))
#define SFDRdv (0x00000000)
#define RMOD_SFDR(...) rset(SFDR, SFDRrv, __VA_ARGS__)
#define RIZS_SFDR(...) rset(SFDR, 0, __VA_ARGS__)
#define RFLD_SFDR(fld) (*((const volatile SFDR_T *)0xb800120c)).f.fld

typedef union {
    struct {
        unsigned int data3:8; //0
        unsigned int data2:8; //0
        unsigned int data1:8; //0
        unsigned int data0:8; //0
    } f;
    unsigned int v;
} SFDR2_T;
#define SFDR2rv (*((regval)0xb8001210))
#define SFDR2dv (0x00000000)
#define RMOD_SFDR2(...) rset(SFDR2, SFDR2rv, __VA_ARGS__)
#define RIZS_SFDR2(...) rset(SFDR2, 0, __VA_ARGS__)
#define RFLD_SFDR2(fld) (*((const volatile SFDR2_T *)0xb8001210)).f.fld

typedef union {
    struct {
        unsigned int mbz_0:3; //0
        unsigned int io3_delay:5; //0
        unsigned int mbz_1:3; //0
        unsigned int io2_delay:5; //0
        unsigned int mbz_2:3; //0
        unsigned int io1_delay:5; //0
        unsigned int mbz_3:3; //0
        unsigned int io0_delay:5; //0
    } f;
    unsigned int v;
} SFRDR_T;
#define SFRDRrv (*((regval)0xb8001218))
#define SFRDRdv (0x00000000)
#define RMOD_SFRDR(...) rset(SFRDR, SFRDRrv, __VA_ARGS__)
#define RIZS_SFRDR(...) rset(SFRDR, 0, __VA_ARGS__)
#define RFLD_SFRDR(fld) (*((const volatile SFRDR_T *)0xb8001218)).f.fld

typedef union {
    struct {
        unsigned int mbz_0:28; //0
        unsigned int rst_rcv_time:4; //11
    } f;
    unsigned int v;
} SFPPR_T;
#define SFPPRrv (*((regval)0xb800121c))
#define SFPPRdv (0x0000000b)
#define RMOD_SFPPR(...) rset(SFPPR, SFPPRrv, __VA_ARGS__)
#define RIZS_SFPPR(...) rset(SFPPR, 0, __VA_ARGS__)
#define RFLD_SFPPR(fld) (*((const volatile SFPPR_T *)0xb800121c)).f.fld

#endif
