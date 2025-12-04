/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include "diag.h"
#include "rtk_hal_i2s.h"
#include "rand.h"
#include "section_config.h"
#include "rtk_i2s_demo.h"
#include "rtl_utility.h"
#include "rtk_sal_i2s.h"


s16 i2s_sine[16]={0, 12539/4, 23170/4, 30273/4, 32767/4, 30273/4, 23170/4, 12539/4,
                  0, -12539/4, -23170/4, -30273/4, -32767/4, -30273/4, -23170/4, -12539/4};

#ifdef  CONFIG_KERNEL
s16 *pDemoBuffer;
#else
s16 pDemoBuffer[768/2];
#endif
#ifdef  CONFIG_KERNEL
s16 *pDemoRxBuffer;
#else
s16 pDemoRxBuffer[768/2];
#endif

s32 I2SDemoWaveLen;
s32 I2SDemoWaveAddr;
s16 *pI2SDemoWave;
#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_2SEC
extern unsigned long I2SDemoWaveSpeech8Khz2Sec, I2SDemoWaveSpeech8Khz2Sec_end;
#endif
#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_10SEC
extern unsigned long I2SDemoWaveSpeech8Khz10Sec, I2SDemoWaveSpeech8Khz10Sec_end;
#endif
#ifdef I2S_DEMO_WAVE_COMPUTEX
extern unsigned long I2SDemoWave1, I2SDemoWave1_end;
#endif



/*I2S User Callback Struct*/
SRAM_BF_DATA_SECTION
SAL_I2S_USER_CB         I2SUserCallBack;

VOID I2SDemoTXCCallBack(
    IN  VOID    *Data
)
{
    /*I2S SAL HND Pointer*/
    PSAL_I2S_HND        pI2SDemoHnd     = (PSAL_I2S_HND)Data;
    u32  Temp;
    static u32 count=0;
    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s\n",pI2SDemoHnd->DevNum,__func__);
    count++;
    if ((count&1023) == 1023)
    {
         DBG_8195A_I2S_LVL(VERI_I2S_LVL, ",\n");
         I2SDemoWaveAddr = 0;
    }
    for (Temp = 0 ; Temp < (768/4) ; Temp++)
    {
#ifdef I2S_DEMO_SINE
        pDemoBuffer[Temp*2] = i2s_sine[Temp%16];
        pDemoBuffer[Temp*2+1] = 0;
#else
  #if (DEMO_WAVE_CHANNEL == 0)
        if (I2SDemoWaveAddr < I2SDemoWaveLen) {
            pDemoBuffer[Temp*2] = pI2SDemoWave[I2SDemoWaveAddr>>1];
            I2SDemoWaveAddr +=2;
        } else
            pDemoBuffer[Temp*2] = 0;
        pDemoBuffer[Temp*2+1] = 0;
  #else
        if (I2SDemoWaveAddr < I2SDemoWaveLen) {
            pDemoBuffer[Temp*2] = pI2SDemoWave[I2SDemoWaveAddr>>1];
            pDemoBuffer[Temp*2+1] = pI2SDemoWave[(I2SDemoWaveAddr>>1)+1];
            I2SDemoWaveAddr +=4;
        } else {
            pDemoBuffer[Temp*2] = 0;
            pDemoBuffer[Temp*2+1] = 0;
        }

  #endif	
#endif
    }
    pI2SDemoHnd->pTXBuf = &pDemoBuffer[0];
    RtkI2SSend(pI2SDemoHnd);

}

VOID I2SDemoRXCCallBack(
    IN  VOID    *Data
)
{
    /*I2S SAL HND Pointer*/
    PSAL_I2S_HND        pI2SDemoHnd     = (PSAL_I2S_HND)Data;
    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s\n",pI2SDemoHnd->DevNum,__func__);
    static u32 count=0;
    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s\n",pI2SDemoHnd->DevNum,__func__);
    count++;
    if ((count&1023) == 1023)
    {
         DBG_8195A_I2S_LVL(VERI_I2S_LVL, ".\n");
    }
    pI2SDemoHnd->pRXBuf = &pDemoRxBuffer[0];

    RtkI2SReceive(pI2SDemoHnd);
}

VOID
I2SDemoStart(
)
{
    u32 I2SIndex = I2SDEMO_INDEX;
    u32 Temp;
    
    /*I2S SAL HND Pointer*/
    PSAL_I2S_HND pI2SDemoHnd    = NULL;


    DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);

    pI2SDemoHnd = RtkI2SGetSalHnd(I2SIndex);

    pI2SDemoHnd->DevNum = I2SIndex;

    /* Load I2S default value, and set user initial value */
    RtkI2SLoadDefault(pI2SDemoHnd);

#ifdef I2S_DEMO_SINE
    pI2SDemoHnd->I2SRate         =  I2S_SR_48KHZ;
#else
  #if (DEMO_WAVE_SAMPLE_RATE == 0) //8khz sample rate
     pI2SDemoHnd->I2SRate         =  I2S_SR_8KHZ;
  #endif
#endif

    if (pI2SDemoHnd->DevNum == 0) {
        pI2SDemoHnd->PinMux = I2S0_TO_S0;

    } else {
        pI2SDemoHnd->PinMux = I2S1_TO_S1;

    }
    /* To Assign the user callback funtions*/
    pI2SDemoHnd->pUserCB                    = &I2SUserCallBack;
    pI2SDemoHnd->pUserCB->TXCCB             = I2SDemoTXCCallBack;
    pI2SDemoHnd->pUserCB->RXCCB             = I2SDemoRXCCallBack;


    DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);
    /* To Initialize I2S */
    RtkI2SInit(pI2SDemoHnd);

    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);
    //send one more tx page
#ifdef CONFIG_KERNEL
    pDemoBuffer = (s16 *) RtlZmalloc(sizeof(s16)*768/2);
    pDemoRxBuffer = (s16 *) RtlZmalloc(sizeof(s16)*768/2);
#endif

#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_2SEC //check demo wave file length   
    I2SDemoWaveLen = (unsigned long) &I2SDemoWaveSpeech8Khz2Sec_end - (unsigned long) &I2SDemoWaveSpeech8Khz2Sec;
    pI2SDemoWave = (s16 *) &I2SDemoWaveSpeech8Khz2Sec;
    I2SDemoWaveAddr = 0;
#endif

    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);

    for (Temp = 0 ; Temp < (768/4) ; Temp++)
    {
#ifdef I2S_DEMO_SINE
        pDemoBuffer[Temp*2] = i2s_sine[Temp%16];
        pDemoBuffer[Temp*2+1] = 0;
#else
  #if (DEMO_WAVE_CHANNEL == 0)
        if (I2SDemoWaveAddr < I2SDemoWaveLen) {
            pDemoBuffer[Temp*2] = pI2SDemoWave[I2SDemoWaveAddr>>1];
            I2SDemoWaveAddr +=2;
        } else
            pDemoBuffer[Temp*2] = 0;
        pDemoBuffer[Temp*2+1] = 0;
  #else
        if (I2SDemoWaveAddr < I2SDemoWaveLen) {
            pDemoBuffer[Temp*2] = pI2SDemoWave[I2SDemoWaveAddr>>1];
            pDemoBuffer[Temp*2+1] = pI2SDemoWave[(I2SDemoWaveAddr>>1)+1];
            I2SDemoWaveAddr +=4;
        } else {
            pDemoBuffer[Temp*2] = 0;
            pDemoBuffer[Temp*2+1] = 0;
        }

  #endif	
#endif
    }
    pI2SDemoHnd->pTXBuf = &pDemoBuffer[0];
    RtkI2SSend(pI2SDemoHnd);
    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);
}


VOID
I2SDemoStop(
)
{
    u32 I2SIndex = I2SDEMO_INDEX;
    
    /*I2S SAL HND Pointer*/
    PSAL_I2S_HND pI2SDemoHnd    = NULL;

    DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);


    pI2SDemoHnd = RtkI2SGetSalHnd(I2SIndex);

    pI2SDemoHnd->DevNum = I2SIndex;


    DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);
    /* To DeInitialize I2S */
    RtkI2SDeInit(pI2SDemoHnd);

    DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);
    RtlMfree(pDemoBuffer, sizeof(s16)*768/2);
    RtlMfree(pDemoRxBuffer, sizeof(s16)*768/2);
    DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s:%d\n",pI2SDemoHnd->DevNum,__func__, __LINE__);
}

