
#ifndef _DW_SPI_BASE_TEST_H_
#define _DW_SPI_BASE_TEST_H_


/* Function Prototype */
//extern void *
//memset( void *s, int c, SIZE_T n );

VOID
SsiAIrqHandle(
        IN VOID *Data
        );

VOID
SsiBIrqHandle(
        IN VOID *Data
        );

VOID
SsiIpEnable();

VOID
GdmaIpEnable();

u8
GetGdmaChannelIrq(
        u8 GdmaIndex, u8 GdmaChannel
        );

u16
GetGdmaChannelEnableValue(
        u8 GdmaChannel
        );

u32
GetSsiIrqNumber(
        u8 SsiIndex
        );

VOID
DumpSsiRegInfo(
        u8 SsiIndex
        );

BOOL
SsiDataCompare(
        IN u16 *pSrc,
        IN u16 *pDst,
        IN u32 Length
        );

VOID
SsiGenerateRandomTxData(
        IN u16 *pTxArray,
        IN u32 Length,
        IN u32 Mode,
        IN BOOL MICROWIRE
        );

#endif //_SPI_BASE_TEST_H_
