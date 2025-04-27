#set PARTS_DB_DIR "../../parts_db"   ;# set the path of parts database

## retrive NOR SPI flash database
source $PARTS_DB_DIR/parts_for_nor_spi.tcl
## Fast Read Settings
namespace eval MXIC_MX25L12835F {
    variable prefer_rd_cmd      0x0B    ;#prefered read command to be used in PIO and MMIO.
    variable prefer_rd_cmd_io   SIO
    variable prefer_rd_dummy_c  8
    variable prefer_rd_addr_io  SIO
    variable prefer_rd_data_io  SIO
}
add_model nor_spi MXIC_MX25L12835F

## DIO Read Settings
#namespace eval MXIC_MX25L12835F {
#    variable prefer_divisor     6       ;#freq. divisor of SPIF ctrl.
#    variable prefer_rd_cmd      0xBB    ;#prefered read command to be used in PIO and MMIO.
#    variable prefer_rd_cmd_io   SIO
#    variable prefer_rd_dummy_c  4
#    variable prefer_rd_addr_io  DIO
#    variable prefer_rd_data_io  DIO
#}
#add_model nor_spi MXIC_MX25L12835F


## retrive dram gen2 database
source $PARTS_DB_DIR/parts_for_dram_gen2.tcl

## DDR3 CLK_M=300MHz
#namespace eval SAMSUNG_K4B1G0846G-BCH9 {
#    variable DQS0_EN_HCLK           1
#    variable tx_clk_phs_delay       17
#    variable clkm_delay             13
#    variable clkm90_delay           10
#}
#add_model dram_gen2 SAMSUNG_K4B1G0846G-BCH9 

## DDR3 CLK_M=193MHz
#namespace eval SAMSUNG_K4B1G0846G-BCH9 {
#    variable DQS0_EN_HCLK           0
#    variable tx_clk_phs_delay       6
#    variable clkm_delay             13
#    variable clkm90_delay           10
#    variable zq_setting             0xdeaddead
#}
#add_model dram_gen2 SAMSUNG_K4B1G0846G-BCH9 

## DDR2 CLK_M=300MHz
namespace eval NT5TU128M8GE-AC {
    variable DQS0_EN_HCLK           1
    variable tx_clk_phs_delay       19
    variable clkm_delay             13
    variable clkm90_delay           10
    variable zq_setting             0xdeaddead
}
add_model dram_gen2 NT5TU128M8GE-AC 



## PLL gen3 information
# NOTE: when using "pin" in "set_by", the corresponding cpu/dsp/dram/lx clock 
#       should be correctly filled as some parameters passed to the preloader
#       are calculated respectively. (E.g., DRAM timer, UART clock divisor and etc) 
namespace eval rtk_pll_gen3 {
    variable set_by         software
    variable cpu_clock_mhz  500
    variable dram_clock_mhz 300
    variable lx_clock_mhz   200
}
add_model pll_gen3 rtk_pll_gen3

## peripheral information
namespace eval rtk_peri {
    variable uart_baudrate  115200
}
add_model peri rtk_peri

## mac info
namespace eval rtk_mac {
    variable address        "11:22:33:44:55:66"
}
add_model mac rtk_mac

## globale information
variable flash_type         spi_nor
variable preloader_image    "plr.img"
variable plr_version        0x0009
variable padding_unit       4K

##flash layout
namespace eval default_flash_layout {
    variable layout [subst {
        { follow    bootloader1 "$bootloader_image" }
        --runtime
        { 256K      env 	      -reserve 16K }
        { 1M        kernel1     -reserve 4M }
        { follow    rootfs1     -reserve 4M+512K }
        { 10M       end }
    }]
}
add_model flash_layout default_flash_layout
