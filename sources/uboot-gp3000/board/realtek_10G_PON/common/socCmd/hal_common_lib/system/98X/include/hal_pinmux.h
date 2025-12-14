#ifndef _HAL_PINMUX_
#define _HAL_PINMUX_


//Function Index
#define UART0       0
#define UART1       1
#define UART2       2
#define SPI0        8
#define SPI1        9
#define SPI2        10
#define SPI3        11
#define SPI0_MCS    15
#define I2C0        16
#define I2C1        17
#define I2C2        18
#define I2C3        19
#define I2S0        24
#define I2S1        25
#define PCM0        28
#define PCM1        29
#define SDIOD       64
#define SDIOH       65
#define WL_LED      96
#define WL_ANT0     104
#define WL_ANT1     105
#define WL_BTCOEX   108
#define WL_BTCMD    109
#define SPI_FLASH   196
#define SDR         200
#define JTAG        216
#define TRACE       217
#define LOG_UART    220
#define LOG_UART_IR 221
#define SIC         224
#define EEPROM      225
#define DEBUG       226

//Location Index
#define DW_MUX_0    0
#define DW_MUX_1    1
#define DW_MUX_2    2
#define DW_MUX_3    3

#ifndef _TRUE
#define _TRUE	    1
#endif

#ifndef _FALSE
#define _FALSE	    0
#endif

#ifndef ON
#define ON          1
#endif

#ifndef OFF
#define OFF         0
#endif

u8 
HalPinCtrlRtl8195A(
    IN u32  Function, 
    IN u32  PinLocation, 
    IN BOOL   Operation);
#endif   //_HAL_PINMUX_
