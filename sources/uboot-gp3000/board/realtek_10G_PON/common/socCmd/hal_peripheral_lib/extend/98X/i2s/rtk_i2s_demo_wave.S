#include "i2s_demo.h"




#ifdef I2S_DEMO_WAVE_IN_BFRAM
	.section	".bfsram.data"
#else
	.section	".sdram.data"
#endif

#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_2SEC	
	.globl	I2SDemoWaveSpeech8Khz2Sec
	.globl	I2SDemoWaveSpeech8Khz2Sec_end
I2SDemoWaveSpeech8Khz2Sec:
	.incbin	"SPEECH_8khz_mono_16bit_LE_2sec.pcm"
I2SDemoWaveSpeech8Khz2Sec_end:
#endif

#ifdef I2S_DEMO_WAVE_SPEECH_8KHZ_10SEC	
	.globl	I2SDemoWaveSpeech8Khz10Sec
I2SDemoWaveSpeech8Khz10Sec:
	.incbin	"SPEECH_8khz_mono_16bit_LE_10sec.pcm"
I2SDemoWaveSpeech8Khz10Sec_end:
#endif




