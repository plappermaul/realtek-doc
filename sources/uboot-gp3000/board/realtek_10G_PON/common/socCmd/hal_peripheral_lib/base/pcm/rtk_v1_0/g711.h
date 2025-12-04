#ifndef G711_H
#define G711_H

// G711 header file
#if 0
#include "typedef.h"
#include "conceal_con.h"
#endif
#define G711_L_FRAME 160		// frame size
#define G711_PERIOD	  20		// 20 ms

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)			/* Number of A-law segments. */
#define	SEG_SHIFT	(4)			/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */

#define	PITCH_MIN	40		/* minimum allowed pitch, 200 Hz */
#define PITCH_MAX	120		/* maximum allowed pitch, 66 Hz */
#define	PITCHDIFF	(PITCH_MAX - PITCH_MIN)
#define	POVERLAPMAX	(PITCH_MAX >> 2)/* maximum pitch OLA window */
#define	HISTORYLEN	(PITCH_MAX * 3 + POVERLAPMAX) /* history buffer length*/
#define	NDEC		2		/* 2:1 decimation */
#define	CORRLEN		160		/* 20 ms correlation length */
#define	CORRBUFLEN	(CORRLEN + PITCH_MAX) /* correlation buffer length */
#define CORRMINPOWER	250	/* minimum power */
#define	EOVERLAPINCR	32	/* end OLA increment per frame, 4 ms */
#define	FRAMESZ		G711_L_FRAME /* 20 ms at 8 KHz */
#define ATTENFAC	6554	/* attenuation factor (0.2) per 10 ms frame */
#define	ATTENINCR	(ATTENFAC/FRAMESZ) /* attenuation per sample */
#if 0
enum    ActionLaw	{A_Law, U_Law};
#if 1 // kenny
extern enum    ActionLaw g711_WrkLaw;		// Work law (a-law u-law)
#else
enum    ActionLaw g711_WrkLaw;		// Work law (a-law u-law)
#endif

extern 	Word16 tabsqr[49];
extern 	Word32 Inv_sqrt(long L_x);
extern 	void  L_Extract(long L_32, short *hi, short *lo);
extern 	Word32 Mpy_32(short hi1, short lo1, short hi2, short lo2);

void    G711_Codec_Init();
void 	G711Dec( char *, Word16 *, Word16, Flag);
//void 	G711Enc( Word16 *, char *);
void 	G711Enc( Word16 *, char *, Word16);

void	LowcFE(void);
void	dofe(short *s);
void	addtohistory(short *s);
void	scalespeech(short *out);
void	getfespeech(short *out, int sz);
void	savespeech(short *s);
void	overlapadd(short *l, short *r, short *o, int cnt);
void	overlapaddatend(short *s, short *f, int cnt);
void	copys(short *f, short *t, int cnt);
void	zeros(short *s, int cnt);
int		findpitch(void);
#endif

extern unsigned char linear2alaw(int pcm_val);
extern int alaw2linear(unsigned char a_val);
extern unsigned char linear2ulaw(int pcm_val);
extern int ulaw2linear(unsigned char u_val);

#endif


