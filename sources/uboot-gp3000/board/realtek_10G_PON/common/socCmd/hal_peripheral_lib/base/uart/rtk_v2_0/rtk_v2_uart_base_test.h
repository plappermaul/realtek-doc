
#ifndef _UART_v2_BASE_TEST_H_
#define _UART_v2_BASE_TEST_H_


u32 RTK_BR_TABLE[] = {
       1200,    9600,   14400,   19200,
      28800,   38400,   57600,   76800,
     115200,  128000,  153600,  230400,
     460800,  500000,  921600, 1000000,
    1382400, 1444400, 1500000, 1843200,
    2000000, 2100000, 2764800, 3000000,
    3250000, 3692300, 3750000, 4000000,
    6000000
};

typedef struct _RUART_VERIFY_PARAMETER_ {
    u32  TestPara0;  // Command
    u32  TestPara1;  // VerifyItem
    u32  TestPara2;  // VerifyLoop
    u32  TestPara3;  // RUART_A ID
    u32  TestPara4;  // RUART_B ID
    u32  TestPara5;  // Baudrate
}RUART_VERIFY_PARAMETER, *PRUART_VERIFY_PARAMETER;

#define ExecuteInfo_num 10
//volatile u8 ExecuteInfo[ExecuteInfo_num];
typedef struct _EXE_BUF_ {
    u8  ExecuteInfo[ExecuteInfo_num];
}EXE_BUF, *PEXE_BUF;

/*ExecuteInfo record each event trigger count
[0]:Received Data Available Interrupt
[1]:Timeout indication interrupt
[2]:Modem Status Interrupts : Clear to Send(transmitter)
[3]:Modem Status Interrupts : Data Set Ready(receiver)
[4]:Modem Status Interrupts : Data Carrier Detect(receiver)
[5]:Tx FIFO empty interrupt
[6]:Receiver Line Status Interrupt: Overrun Error
[7]:Tx DMA burst transfer times
[8]:Rx DMA burst transfer times
[9]:compare data result, 1:success, 0: fail
*/
typedef enum _UART_EXE_RESULT_ {
	DATA_OK_INT = 0,
	DATA_TIMEOUT = 1,
	CTS 		= 2,
	DSR	  		= 3,
	DCD	  		= 4,
	TXFIFO_EMP	= 5,
	OV_ERR		= 6,
	RLS_INT		= 7,
	TX_DMA		= 8,
	RX_DMA		= 9,
	RTKUART_COMPARE_GOOD = 10
}UART_EXE_RESULT, *PUART_EXE_RESULT;

#endif //_UART_BASE_TEST_H_
