#ifndef _UART_V2_BAUDRATE_H_
#define _UART_V2_BAUDRATE_H_

u32 RTKBAUDRATE[] = {
       1200,    9600,   14400,   19200,
      28800,   38400,   57600,   76800,
     115200,  128000,  153600,  230400,
     460800,  500000,  921600, 1000000,
    1382400, 1444400, 1500000, 1843200,
    2000000, 2100000, 2764800, 3000000,
    3250000, 3692300, 3750000, 4000000,
    6000000
};
#if 0 //no need
u32 RTKOVSR[] = {
    11, 20, 10, 16,
    10, 10, 14, 10,
    14, 15, 10, 14,
    15, 10, 15, 10,
    14, 13, 11, 18,
    10, 15, 18, 11,
    15, 13, 13, 12,
    16
};

u32 RTKDIV[] = {
    7326, 499, 646, 323,
     323, 251, 124, 124,
      62,  50,  62,  31,
      14,  20,   7,  10,
       5,   5,   6,   3,
       5,   3,   2,   3,
       2,   2,   2,   2,
       1
};

u32 RTKOVSR_ADJ[] = {
    0x24A, 0x7EF, 0x5DD,  0x10,
    0x5DD, 0x24A,   0x0, 0x555,
      0x0, 0x5AD, 0x555,   0x0,
    0x555,   0x0, 0x555,   0x0,
    0x2AA, 0x5F7,  0x10,  0x20,
      0x0, 0x7F7,  0x20,  0x10,
    0x24A, 0x555, 0x492, 0x555,
    0x36D
};
#endif //no need
u8 RTKTEST_DATA[] = {
    0xFF, 0x5A, 0xA5, 0x00
};

typedef struct _RUART_SPEED_SETTING_ {
    u32 BaudRate;
    u32 Ovsr;
    u32 Div;
    u32 Ovsr_adj;
}RUART_SPEED_SETTING, *PRUART_SPEED_SETTING;

#endif
