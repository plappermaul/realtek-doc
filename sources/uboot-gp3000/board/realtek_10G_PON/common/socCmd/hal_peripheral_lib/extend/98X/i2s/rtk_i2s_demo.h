/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_I2S_DEMO_H_
#define _RTK_I2S_DEMO_H_

//select wave data in bfram 262144 or sdram 2M
#define I2S_DEMO_WAVE_IN_BFRAM	1


//select wave data
#define I2S_DEMO_SINE				1
//#define I2S_DEMO_WAVE_SPEECH_8KHZ_2SEC	1
//#define I2S_DEMO_WAVE_SPEECH_8KHZ_10SEC	1
//#define I2S_DEMO_WAVE_SPEECH_8KHZ_32SEC	1

#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_2SEC
#define DEMO_WAVE_SAMPLE_RATE 0
#define DEMO_WAVE_CHANNEL 0
#endif

#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_10SEC
#define DEMO_WAVE_SAMPLE_RATE 0
#define DEMO_WAVE_CHANNEL 0
#endif

#ifdef I2S_DEMO_WAVE_COMPUTEX
// 0:8khz, 1:16khz, 2:24khz, 3:32khz, 5:48khz, 6:96khz
#define DEMO_WAVE_SAMPLE_RATE 5
#define DEMO_WAVE_CHANNEL 1
#endif /* I2S_DEMO_WAVE_COMPUTEX  */

//select i2s0 or i2s1 for demo
#define I2SDEMO_INDEX 1


#endif

