#include <common.h>

#include "rtk_crypto_base_common.h"
#include "cryptoSim_cipher.h"

/*************** des.c ***************/

/* Weak and semi week keys as take from
 * %A D.W. Davies
 * %A W.L. Price
 * %T Security for Computer Networks
 * %I John Wiley & Sons
 * %D 1984
 * Many thanks to smb@ulysses.att.com (Steven Bellovin) for the reference
 * (and actual cblock values).
 */
#define NUM_WEAK_KEY    16
static int8_t weak_keys[NUM_WEAK_KEY][8]={
    /* weak keys */
    {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
    {0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE},
    {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F},
    {0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0},
    /* semi-weak keys */
    {0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE},
    {0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01},
    {0x1F,0xE0,0x1F,0xE0,0x0E,0xF1,0x0E,0xF1},
    {0xE0,0x1F,0xE0,0x1F,0xF1,0x0E,0xF1,0x0E},
    {0x01,0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1},
    {0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1,0x01},
    {0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E,0xFE},
    {0xFE,0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E},
    {0x01,0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E},
    {0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E,0x01},
    {0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1,0xFE},
    {0xFE,0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1}};


int32_t des_check_key=0;

void des_set_odd_parity(uint8_t *key) {
    int32_t i;

    for (i=0; i<DES_KEY_SZ; i++)
        key[i]=odd_parity[key[i]];
}

static int32_t check_parity(const uint8_t *key) {
    int32_t i;

    for (i=0; i<DES_KEY_SZ; i++) {
        if (key[i] != odd_parity[key[i]])
            return(0);
    }
    return(1);
}


int des_is_weak_key(const u8 *key) {
    int32_t i;

    for (i=0; i<NUM_WEAK_KEY; i++)
        /* Added == 0 to comparision, I obviously don't run
         * this section very often :-(, thanks to
         * engineering@MorningStar.Com for the fix
         * eay 93/06/29 */
        if ( rtlc_memcmp((const u8*)(weak_keys[i]),key,sizeof(*key)) == 0) return(1);
    return(0);
}

/* NOW DEFINED IN des_local.h
 * See ecb_encrypt.c for a pseudo description of these macros.
 * #define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
 *  (b)^=(t),\
 *  (a)=((a)^((t)<<(n))))
 */

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
    (a)=(a)^(t)^(t>>(16-(n))))

/* return 0 if key parity is odd (correct),
 * return -1 if key parity error,
 * return -2 if illegal weak key.
 */
int des_set_key(const u8 * key, des_key_schedule schedule) {
    static int32_t shifts2[16]={0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0};
    DES_LONG c,d,t,s;
    uint8_t *in;
    DES_LONG *k;
    int32_t i;

    if (des_check_key)
        {
        if (!check_parity(key))
            return(-1);

        if (des_is_weak_key(key))
            return(-2);
        }

    k=(DES_LONG *)schedule;
    in=(uint8_t *)key;

    c2l(in,c);
    c2l(in,d);

    /* do PC1 in 60 simple operations */
/*  PERM_OP(d,c,t,4,0x0f0f0f0fL);
    HPERM_OP(c,t,-2, 0xcccc0000L);
    HPERM_OP(c,t,-1, 0xaaaa0000L);
    HPERM_OP(c,t, 8, 0x00ff0000L);
    HPERM_OP(c,t,-1, 0xaaaa0000L);
    HPERM_OP(d,t,-8, 0xff000000L);
    HPERM_OP(d,t, 8, 0x00ff0000L);
    HPERM_OP(d,t, 2, 0x33330000L);
    d=((d&0x00aa00aaL)<<7L)|((d&0x55005500L)>>7L)|(d&0xaa55aa55L);
    d=(d>>8)|((c&0xf0000000L)>>4);
    c&=0x0fffffffL; */

    /* I now do it in 47 simple operations :-)
     * Thanks to John Fletcher (john_fletcher@lccmail.ocf.llnl.gov)
     * for the inspiration. :-) */
    PERM_OP (d,c,t,4,0x0f0f0f0fL);
    HPERM_OP(c,t,-2,0xcccc0000L);
    HPERM_OP(d,t,-2,0xcccc0000L);
    PERM_OP (d,c,t,1,0x55555555L);
    PERM_OP (c,d,t,8,0x00ff00ffL);
    PERM_OP (d,c,t,1,0x55555555L);
    d=  (((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
         ((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
    c&=0x0fffffffL;

    for (i=0; i<ITERATIONS; i++)
        {
        if (shifts2[i])
            { c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L)); }
        else
            { c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L)); }
        c&=0x0fffffffL;
        d&=0x0fffffffL;
        /* could be a few less shifts but I am to lazy at this
         * point in time to investigate */
        s=  des_skb[0][ (c    )&0x3f                ]|
            des_skb[1][((c>> 6)&0x03)|((c>> 7L)&0x3c)]|
            des_skb[2][((c>>13)&0x0f)|((c>>14L)&0x30)]|
            des_skb[3][((c>>20)&0x01)|((c>>21L)&0x06) |
                          ((c>>22L)&0x38)];
        t=  des_skb[4][ (d    )&0x3f                ]|
            des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|
            des_skb[6][ (d>>15L)&0x3f                ]|
            des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];

        /* table contained 0213 4657 */
        *(k++)=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;
        s=     ((s>>16L)|(t&0xffff0000L));
        
        s=(s<<4L)|(s>>28L);
        *(k++)=s&0xffffffffL;
        }
    return(0);
    }

int des_key_sched(u8 * key, des_key_schedule schedule) {
    return(des_set_key(key,schedule));
}


// ---------------------------------------------------

char *libdes_version="libdes v 3.24 - 20-Apr-1996 - eay";
char *DES_version="DES part of SSLeay 0.6.4 30-Aug-1996";

char *des_options()
    {
#ifdef DES_PTR
    if (sizeof(DES_LONG) != sizeof(long))
        return("des(ptr,int32_t)");
    else
        return("des(ptr,long)");
#else
    if (sizeof(DES_LONG) != sizeof(long))
        return("des(idx,int32_t)");
    else
        return("des(idx,long)");
#endif
    }
        

void des_ecb_encrypt(input, output, ks, encrypt)
des_cblock (*input);
des_cblock (*output);
des_key_schedule ks;
int32_t encrypt;
    {
    DES_LONG l;
    uint8_t *in,*out;
    DES_LONG ll[2];

    in=(uint8_t *)input;
    out=(uint8_t *)output;
    c2l(in,l); ll[0]=l;
    c2l(in,l); ll[1]=l;
    des_encrypt(ll,ks,encrypt);
    l=ll[0]; l2c(l,out);
    l=ll[1]; l2c(l,out);
    l=ll[0]=ll[1]=0;
    }

void des_encrypt(data, ks, encrypt)
DES_LONG *data;
des_key_schedule ks;
int32_t encrypt;
    {
    DES_LONG l,r,t,u;
#ifdef DES_PTR
    uint8_t *des_SP=(uint8_t *)des_SPtrans;
#endif
#ifdef undef
    union fudge {
        DES_LONG  l;
        uint16 s[2];
        uint8_t  c[4];
        } U,T;
#endif
    int32_t i;
    DES_LONG *s;

    u=data[0];
    r=data[1];

    IP(u,r);
    /* Things have been modified so that the initial rotate is
     * done outside the loop.  This required the
     * des_SPtrans values in sp.h to be rotated 1 bit to the right.
     * One perl script later and things have a 5% speed up on a sparc2.
     * Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
     * for pointing this out. */
    l=(r<<1)|(r>>31);
    r=(u<<1)|(u>>31);

    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    s=(DES_LONG *)ks;
    /* I don't know if it is worth the effort of loop unrolling the
     * inner loop
     */
    if (encrypt)
        {
        for (i=0; i<32; i+=8)
            {
            D_ENCRYPT(l,r,i+0); /*  1 */
            D_ENCRYPT(r,l,i+2); /*  2 */
            D_ENCRYPT(l,r,i+4); /*  3 */
            D_ENCRYPT(r,l,i+6); /*  4 */
            }
        }
    else
        {
        for (i=30; i>0; i-=8)
            {
            D_ENCRYPT(l,r,i-0); /* 16 */
            D_ENCRYPT(r,l,i-2); /* 15 */
            D_ENCRYPT(l,r,i-4); /* 14 */
            D_ENCRYPT(r,l,i-6); /* 13 */
            }
        }
    l=(l>>1)|(l<<31);
    r=(r>>1)|(r<<31);
    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    FP(r,l);
    data[0]=l;
    data[1]=r;
    l=r=t=u=0;
    }

void des_encrypt2(data, ks, encrypt)
DES_LONG *data;
des_key_schedule ks;
int32_t encrypt;
    {
    DES_LONG l,r,t,u;
#ifdef DES_PTR
    uint8_t *des_SP=(uint8_t *)des_SPtrans;
#endif
#ifdef undef
    union fudge {
        DES_LONG  l;
        uint16 s[2];
        uint8_t  c[4];
        } U,T;
#endif
    int32_t i;
    DES_LONG *s;

    u=data[0];
    r=data[1];

    /* Things have been modified so that the initial rotate is
     * done outside the loop.  This required the
     * des_SPtrans values in sp.h to be rotated 1 bit to the right.
     * One perl script later and things have a 5% speed up on a sparc2.
     * Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
     * for pointing this out. */
    l=(r<<1)|(r>>31);
    r=(u<<1)|(u>>31);

    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    s=(DES_LONG *)ks;
    /* I don't know if it is worth the effort of loop unrolling the
     * inner loop */
    if (encrypt)
        {
        for (i=0; i<32; i+=8)
            {
            D_ENCRYPT(l,r,i+0); /*  1 */
            D_ENCRYPT(r,l,i+2); /*  2 */
            D_ENCRYPT(l,r,i+4); /*  3 */
            D_ENCRYPT(r,l,i+6); /*  4 */
            }
        }
    else
        {
        for (i=30; i>0; i-=8)
            {
            D_ENCRYPT(l,r,i-0); /* 16 */
            D_ENCRYPT(r,l,i-2); /* 15 */
            D_ENCRYPT(l,r,i-4); /* 14 */
            D_ENCRYPT(r,l,i-6); /* 13 */
            }
        }
    l=(l>>1)|(l<<31);
    r=(r>>1)|(r<<31);
    /* clear the top bits on machines with 8byte longs */
    l&=0xffffffffL;
    r&=0xffffffffL;

    data[0]=l;
    data[1]=r;
    l=r=t=u=0;
    }

void DES_encrypt3(DES_LONG *data, des_key_schedule *ks1,
          des_key_schedule *ks2, des_key_schedule *ks3)
    {
    register DES_LONG l,r;

    l=data[0];
    r=data[1];
    IP(l,r);
    data[0]=l;
    data[1]=r;
    des_encrypt2((DES_LONG *)data,*ks1,DES_ENCRYPT);
    des_encrypt2((DES_LONG *)data,*ks2,DES_DECRYPT);
    des_encrypt2((DES_LONG *)data,*ks3,DES_ENCRYPT);
    l=data[0];
    r=data[1];
    FP(r,l);
    data[0]=l;
    data[1]=r;
}

void DES_decrypt3(DES_LONG *data, des_key_schedule *ks1,
          des_key_schedule *ks2, des_key_schedule *ks3)
    {
    register DES_LONG l,r;

    l=data[0];
    r=data[1];
    IP(l,r);
    data[0]=l;
    data[1]=r;
    des_encrypt2((DES_LONG *)data,*ks3,DES_DECRYPT);
    des_encrypt2((DES_LONG *)data,*ks2,DES_ENCRYPT);
    des_encrypt2((DES_LONG *)data,*ks1,DES_DECRYPT);
    l=data[0];
    r=data[1];
    FP(r,l);
    data[0]=l;
    data[1]=r;
}

void des_ecb3_encrypt(des_cblock *input, des_cblock *output,
              des_key_schedule *ks1, des_key_schedule *ks2,
              des_key_schedule *ks3,
         int enc)
    {
    register DES_LONG l0,l1;
    DES_LONG ll[2];
    const unsigned char *in = &(*input)[0];
    unsigned char *out = &(*output)[0];

    c2l(in,l0);
    c2l(in,l1);
    ll[0]=l0;
    ll[1]=l1;
    if (enc)
        DES_encrypt3(ll,ks1,ks2,ks3);
    else
        DES_decrypt3(ll,ks1,ks2,ks3);
    l0=ll[0];
    l1=ll[1];
    l2c(l0,out);
    l2c(l1,out);
}

void des_ncbc_encrypt(const u8 *in, unsigned char *out, long length,
             des_key_schedule *_schedule, des_cblock *ivec, int enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv;

    iv = &(*ivec)[0];

    if (enc)
        {
        c2l(iv,tout0);
        c2l(iv,tout1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0; tin[0]=tin0;
            tin1^=tout1; tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_ENCRYPT);
            tout0=tin[0]; l2c(tout0,out);
            tout1=tin[1]; l2c(tout1,out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,l+8);
            tin0^=tout0; tin[0]=tin0;
            tin1^=tout1; tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_ENCRYPT);
            tout0=tin[0]; l2c(tout0,out);
            tout1=tin[1]; l2c(tout1,out);
            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        l2c(tout1,iv);
        }
    else
        {
        c2l(iv,xor0);
        c2l(iv,xor1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0); tin[0]=tin0;
            c2l(in,tin1); tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_DECRYPT);
            tout0=tin[0]^xor0;
            tout1=tin[1]^xor1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=tin0;
            xor1=tin1;
            }
        if (l != -8)
            {
            c2l(in,tin0); tin[0]=tin0;
            c2l(in,tin1); tin[1]=tin1;
            des_encrypt((DES_LONG *)tin,*_schedule,DES_DECRYPT);
            tout0=tin[0]^xor0;
            tout1=tin[1]^xor1;
            l2cn(tout0,tout1,out,l+8);
            xor0=tin0;
            xor1=tin1;
            }
        iv = &(*ivec)[0];
        l2c(xor0,iv);
        l2c(xor1,iv);
        }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0;
}

 
unsigned char 
desx_white_in2out[256]={
    0xBD,0x56,0xEA,0xF2,0xA2,0xF1,0xAC,0x2A,0xB0,0x93,0xD1,0x9C,0x1B,0x33,0xFD,0xD0,
    0x30,0x04,0xB6,0xDC,0x7D,0xDF,0x32,0x4B,0xF7,0xCB,0x45,0x9B,0x31,0xBB,0x21,0x5A,
    0x41,0x9F,0xE1,0xD9,0x4A,0x4D,0x9E,0xDA,0xA0,0x68,0x2C,0xC3,0x27,0x5F,0x80,0x36,
    0x3E,0xEE,0xFB,0x95,0x1A,0xFE,0xCE,0xA8,0x34,0xA9,0x13,0xF0,0xA6,0x3F,0xD8,0x0C,
    0x78,0x24,0xAF,0x23,0x52,0xC1,0x67,0x17,0xF5,0x66,0x90,0xE7,0xE8,0x07,0xB8,0x60,
    0x48,0xE6,0x1E,0x53,0xF3,0x92,0xA4,0x72,0x8C,0x08,0x15,0x6E,0x86,0x00,0x84,0xFA,
    0xF4,0x7F,0x8A,0x42,0x19,0xF6,0xDB,0xCD,0x14,0x8D,0x50,0x12,0xBA,0x3C,0x06,0x4E,
    0xEC,0xB3,0x35,0x11,0xA1,0x88,0x8E,0x2B,0x94,0x99,0xB7,0x71,0x74,0xD3,0xE4,0xBF,
    0x3A,0xDE,0x96,0x0E,0xBC,0x0A,0xED,0x77,0xFC,0x37,0x6B,0x03,0x79,0x89,0x62,0xC6,
    0xD7,0xC0,0xD2,0x7C,0x6A,0x8B,0x22,0xA3,0x5B,0x05,0x5D,0x02,0x75,0xD5,0x61,0xE3,
    0x18,0x8F,0x55,0x51,0xAD,0x1F,0x0B,0x5E,0x85,0xE5,0xC2,0x57,0x63,0xCA,0x3D,0x6C,
    0xB4,0xC5,0xCC,0x70,0xB2,0x91,0x59,0x0D,0x47,0x20,0xC8,0x4F,0x58,0xE0,0x01,0xE2,
    0x16,0x38,0xC4,0x6F,0x3B,0x0F,0x65,0x46,0xBE,0x7E,0x2D,0x7B,0x82,0xF9,0x40,0xB5,
    0x1D,0x73,0xF8,0xEB,0x26,0xC7,0x87,0x97,0x25,0x54,0xB1,0x28,0xAA,0x98,0x9D,0xA5,
    0x64,0x6D,0x7A,0xD4,0x10,0x81,0x44,0xEF,0x49,0xD6,0xAE,0x2E,0xDD,0x76,0x5C,0x2F,
    0xA7,0x1C,0xC9,0x09,0x69,0x9A,0x83,0xCF,0x29,0x39,0xB9,0xE9,0x4C,0xFF,0x43,0xAB,
};

void des_xwhite_in2out(const des_cblock *des_key, const des_cblock *in_white,
         des_cblock *out_white)
    {
    int out0,out1;
    int i;
    const unsigned char *key = &(*des_key)[0];
    const unsigned char *in = &(*in_white)[0];
    unsigned char *out = &(*out_white)[0];

    out[0]=out[1]=out[2]=out[3]=out[4]=out[5]=out[6]=out[7]=0;
    out0=out1=0;
    for (i=0; i<8; i++)
        {
        out[i]=key[i]^desx_white_in2out[out0^out1];
        out0=out1;
        out1=(int)out[i&0x07];
        }

    out0=out[0];
    out1=out[i];
    for (i=0; i<8; i++)
        {
        out[i]=in[i]^desx_white_in2out[out0^out1];
        out0=out1;
        out1=(int)out[i&0x07];
        }
    }

#if 0
void des_xcbc_encrypt(des_cblock *_in, des_cblock *_out,
              long length, des_key_schedule schedule,
              des_cblock *ivec, des_cblock *inw,
              des_cblock *outw, int enc)
    {
    unsigned char *in = (unsigned char *) _in;
    unsigned char *out = (unsigned char *) _out;
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register DES_LONG inW0,inW1,outW0,outW1;
    register const unsigned char *in2;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv;

    in2 = &(*inw)[0];
    c2l(in2,inW0);
    c2l(in2,inW1);
    in2 = &(*outw)[0];
    c2l(in2,outW0);
    c2l(in2,outW1);

    iv = &(*ivec)[0];

    if (enc)
        {
        c2l(iv,tout0);
        c2l(iv,tout1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0^inW0; tin[0]=tin0;
            tin1^=tout1^inW1; tin[1]=tin1;
            des_encrypt(tin,schedule,DES_ENCRYPT);
            tout0=tin[0]^outW0; l2c(tout0, out);
            tout1=tin[1]^outW1; l2c(tout1, out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,l+8);
            tin0^=tout0^inW0; tin[0]=tin0;
            tin1^=tout1^inW1; tin[1]=tin1;
            des_encrypt(tin,schedule,DES_ENCRYPT);
            tout0=tin[0]^outW0; l2c(tout0,out);
            tout1=tin[1]^outW1; l2c(tout1,out);
            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        l2c(tout1,iv);
        }
    else
        {
        c2l(iv,xor0);
        c2l(iv,xor1);
        for (l-=8; l>0; l-=8)
            {
            c2l(in,tin0); tin[0]=tin0^outW0;
            c2l(in,tin1); tin[1]=tin1^outW1;
            des_encrypt(tin,schedule,DES_DECRYPT);
            tout0=tin[0]^xor0^inW0;
            tout1=tin[1]^xor1^inW1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=tin0;
            xor1=tin1;
            }
        if (l != -8)
            {
            c2l(in,tin0); tin[0]=tin0^outW0;
            c2l(in,tin1); tin[1]=tin1^outW1;
            des_encrypt(tin,schedule,DES_DECRYPT);
            tout0=tin[0]^xor0^inW0;
            tout1=tin[1]^xor1^inW1;
            l2cn(tout0,tout1,out,l+8);
            xor0=tin0;
            xor1=tin1;
            }

        iv = &(*ivec)[0];
        l2c(xor0,iv);
        l2c(xor1,iv);
        }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    inW0=inW1=outW0=outW1=0;
    tin[0]=tin[1]=0;
    }
#endif

void des_ede3_cbc_encrypt(const unsigned char *input, unsigned char *output,
              long length, des_key_schedule *ks1,
              des_key_schedule *ks2, des_key_schedule *ks3,
              des_cblock *ivec, int enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register const unsigned char *in;
    unsigned char *out;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv;


    in=input;
    out=output;
    iv = &(*ivec)[0];


    if (enc)
        {
        c2l(iv,tout0);
        c2l(iv,tout1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0;
            tin1^=tout1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_encrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            l2c(tout0,out);
            l2c(tout1,out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,l+8);
            tin0^=tout0;
            tin1^=tout1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_encrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            l2c(tout0,out);
            l2c(tout1,out);
            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        l2c(tout1,iv);
        }
    else
        {
        register DES_LONG t0,t1;

        c2l(iv,xor0);
        c2l(iv,xor1);
        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);

            t0=tin0;
            t1=tin1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_decrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            tout0^=xor0;
            tout1^=xor1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=t0;
            xor1=t1;
            }
        if (l != -8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            
            t0=tin0;
            t1=tin1;

            tin[0]=tin0;
            tin[1]=tin1;
            DES_decrypt3((DES_LONG *)tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];
        
            tout0^=xor0;
            tout1^=xor1;
            l2cn(tout0,tout1,out,l+8);
            xor0=t0;
            xor1=t1;
            }

        iv = &(*ivec)[0];
        l2c(xor0,iv);
        l2c(xor1,iv);
        }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0;
    }

#if 0
void des_ede3_cbcm_encrypt(const unsigned char *in, unsigned char *out,
         long length, des_key_schedule ks1, des_key_schedule ks2,
         des_key_schedule ks3, des_cblock *ivec1, des_cblock *ivec2,
         int enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1,m0,m1;
    register long l=length;
    DES_LONG tin[2];
    unsigned char *iv1,*iv2;

    iv1 = &(*ivec1)[0];
    iv2 = &(*ivec2)[0];

    if (enc)
    {
    c2l(iv1,m0);
    c2l(iv1,m1);
    c2l(iv2,tout0);
    c2l(iv2,tout1);
    for (l-=8; l>=-7; l-=8)
        {
        tin[0]=m0;
        tin[1]=m1;
        des_encrypt(tin,ks3,1);
        m0=tin[0];
        m1=tin[1];

        if(l < 0)
        {
        c2ln(in,tin0,tin1,l+8);
        }
        else
        {
        c2l(in,tin0);
        c2l(in,tin1);
        }
        tin0^=tout0;
        tin1^=tout1;

        tin[0]=tin0;
        tin[1]=tin1;
        des_encrypt(tin,ks1,1);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks2,0);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks1,1);
        tout0=tin[0];
        tout1=tin[1];

        l2c(tout0,out);
        l2c(tout1,out);
        }
    iv1=&(*ivec1)[0];
    l2c(m0,iv1);
    l2c(m1,iv1);

    iv2=&(*ivec2)[0];
    l2c(tout0,iv2);
    l2c(tout1,iv2);
    }
    else
    {
    register DES_LONG t0,t1;

    c2l(iv1,m0);
    c2l(iv1,m1);
    c2l(iv2,xor0);
    c2l(iv2,xor1);
    for (l-=8; l>=-7; l-=8)
        {
        tin[0]=m0;
        tin[1]=m1;
        des_encrypt(tin,ks3,1);
        m0=tin[0];
        m1=tin[1];

        c2l(in,tin0);
        c2l(in,tin1);

        t0=tin0;
        t1=tin1;

        tin[0]=tin0;
        tin[1]=tin1;
        des_encrypt(tin,ks1,0);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks2,1);
        tin[0]^=m0;
        tin[1]^=m1;
        des_encrypt(tin,ks1,0);
        tout0=tin[0];
        tout1=tin[1];

        tout0^=xor0;
        tout1^=xor1;
        if(l < 0)
        {
        l2cn(tout0,tout1,out,l+8);
        }
        else
        {
        l2c(tout0,out);
        l2c(tout1,out);
        }
        xor0=t0;
        xor1=t1;
        }

    iv1=&(*ivec1)[0];
    l2c(m0,iv1);
    l2c(m1,iv1);

    iv2=&(*ivec2)[0];
    l2c(xor0,iv2);
    l2c(xor1,iv2);
    }
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0;
}

#endif


// --------------------------------------------------


int desSim_ecb_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 encrypt) {
    u32 idx;
    des_cblock in,out;
    des_key_schedule ks;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAIL;
    
    for(idx = 0; idx<len; idx+=8) {
        des_set_key(key, ks);
        rtlc_memcpy((u8*)in, (const u8*)(&input[idx]), 8);
        rtlc_memset((u8*)out, 0,8);
        des_ecb_encrypt(&in,&out,ks,encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
        rtlc_memcpy((u8*)(&output[idx]), out, 8);
    }
    return SUCCESS;
}

int desSim_ede_ecb_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 encrypt) {
    u32 idx;
    des_cblock in,out;
    des_key_schedule ks, ks2, ks3;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAIL;

    for(idx = 0; idx<len; idx+=8) {
        if (des_set_key(&key[0], ks) != 0 || des_set_key(&key[8], ks2) != 0 || des_set_key(&key[16], ks3) != 0)
            return FAIL;
    
        rtlc_memcpy((u8*)in, &input[idx], 8);
        rtlc_memset((u8*)out, 0, 8);
        des_ecb3_encrypt((des_cblock *)&in,&out,&ks,&ks2, &ks3, encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
        rtlc_memcpy((u8*)(&output[idx]), out, 8);
    }
    return SUCCESS;
}

int desSim_cbc_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 * iv, const u8 encrypt) {
    des_cblock iv3;
    des_key_schedule ks;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAIL;

    if (des_set_key(&key[0], ks) != 0)
        return FAIL;
    
    rtlc_memcpy((u8*)iv3, iv, 8);
    des_ncbc_encrypt(input, output, len, &ks, &iv3, encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
    
    return SUCCESS;
}

int desSim_ede_cbc_encrypt(const u8 *input, u8 * output, const u32 len, const u8 * key, const u8 * iv, const u8 encrypt) {
    des_cblock iv3;
    des_key_schedule ks, ks2, ks3;

    if(encrypt != TRUE && encrypt != FALSE) //Only support either encrypt or decrypt operation
        return FAIL;

    if (des_set_key(&key[0], ks) != 0 || des_set_key(&key[8], ks2) != 0 || des_set_key(&key[16], ks3) != 0)
        return FAIL;

    rtlc_memcpy((u8*)iv3, iv, 8);
    
    des_ede3_cbc_encrypt(input, output, len, &ks, &ks2, &ks3, &iv3, encrypt==TRUE? DES_ENCRYPT : DES_DECRYPT);
    
    return SUCCESS;
}

/*************** ??? ***************/

/* Implementation that should never be optimized out by the compiler */

void mbedtls_zeroize(void *v, u32 n)
{
    volatile u8 *p = (u8 *)v; while (n--) *p++ = 0;
}

/*
 * 32-bit s32eger manipulation macros (little endian)
 */
#ifndef GET_UINT32_LE
#define GET_UINT32_LE(n,b,i)                            \
{                                                       \
    (n) = ( (u32) (b)[(i)    ]       )             \
        | ( (u32) (b)[(i) + 1] <<  8 )             \
        | ( (u32) (b)[(i) + 2] << 16 )             \
        | ( (u32) (b)[(i) + 3] << 24 );            \
}
#endif

#ifndef PUT_UINT32_LE
#define PUT_UINT32_LE(n,b,i)                                    \
{                                                               \
    (b)[(i)    ] = (u8) ( ( (n)       ) & 0xFF );    \
    (b)[(i) + 1] = (u8) ( ( (n) >>  8 ) & 0xFF );    \
    (b)[(i) + 2] = (u8) ( ( (n) >> 16 ) & 0xFF );    \
    (b)[(i) + 3] = (u8) ( ( (n) >> 24 ) & 0xFF );    \
}
#endif

#if 0
#if defined(MBEDTLS_PADLOCK_C) &&                      \
    ( defined(MBEDTLS_HAVE_X86) || defined(MBEDTLS_PADLOCK_ALIGN16) )
static s32 aes_padlock_ace = -1;
#endif
#endif

#define MBEDTLS_AES_ROM_TABLES
#if defined(MBEDTLS_AES_ROM_TABLES)

/*
 * Forward S-box
 */
const u8 FSb[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5,
    0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0,
    0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC,
    0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A,
    0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0,
    0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B,
    0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85,
    0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
    0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17,
    0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88,
    0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C,
    0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9,
    0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6,
    0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E,
    0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94,
    0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68,
    0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/*
 * Forward tables
 */
#define FT \
\
    V(A5,63,63,C6), V(84,7C,7C,F8), V(99,77,77,EE), V(8D,7B,7B,F6), \
    V(0D,F2,F2,FF), V(BD,6B,6B,D6), V(B1,6F,6F,DE), V(54,C5,C5,91), \
    V(50,30,30,60), V(03,01,01,02), V(A9,67,67,CE), V(7D,2B,2B,56), \
    V(19,FE,FE,E7), V(62,D7,D7,B5), V(E6,AB,AB,4D), V(9A,76,76,EC), \
    V(45,CA,CA,8F), V(9D,82,82,1F), V(40,C9,C9,89), V(87,7D,7D,FA), \
    V(15,FA,FA,EF), V(EB,59,59,B2), V(C9,47,47,8E), V(0B,F0,F0,FB), \
    V(EC,AD,AD,41), V(67,D4,D4,B3), V(FD,A2,A2,5F), V(EA,AF,AF,45), \
    V(BF,9C,9C,23), V(F7,A4,A4,53), V(96,72,72,E4), V(5B,C0,C0,9B), \
    V(C2,B7,B7,75), V(1C,FD,FD,E1), V(AE,93,93,3D), V(6A,26,26,4C), \
    V(5A,36,36,6C), V(41,3F,3F,7E), V(02,F7,F7,F5), V(4F,CC,CC,83), \
    V(5C,34,34,68), V(F4,A5,A5,51), V(34,E5,E5,D1), V(08,F1,F1,F9), \
    V(93,71,71,E2), V(73,D8,D8,AB), V(53,31,31,62), V(3F,15,15,2A), \
    V(0C,04,04,08), V(52,C7,C7,95), V(65,23,23,46), V(5E,C3,C3,9D), \
    V(28,18,18,30), V(A1,96,96,37), V(0F,05,05,0A), V(B5,9A,9A,2F), \
    V(09,07,07,0E), V(36,12,12,24), V(9B,80,80,1B), V(3D,E2,E2,DF), \
    V(26,EB,EB,CD), V(69,27,27,4E), V(CD,B2,B2,7F), V(9F,75,75,EA), \
    V(1B,09,09,12), V(9E,83,83,1D), V(74,2C,2C,58), V(2E,1A,1A,34), \
    V(2D,1B,1B,36), V(B2,6E,6E,DC), V(EE,5A,5A,B4), V(FB,A0,A0,5B), \
    V(F6,52,52,A4), V(4D,3B,3B,76), V(61,D6,D6,B7), V(CE,B3,B3,7D), \
    V(7B,29,29,52), V(3E,E3,E3,DD), V(71,2F,2F,5E), V(97,84,84,13), \
    V(F5,53,53,A6), V(68,D1,D1,B9), V(00,00,00,00), V(2C,ED,ED,C1), \
    V(60,20,20,40), V(1F,FC,FC,E3), V(C8,B1,B1,79), V(ED,5B,5B,B6), \
    V(BE,6A,6A,D4), V(46,CB,CB,8D), V(D9,BE,BE,67), V(4B,39,39,72), \
    V(DE,4A,4A,94), V(D4,4C,4C,98), V(E8,58,58,B0), V(4A,CF,CF,85), \
    V(6B,D0,D0,BB), V(2A,EF,EF,C5), V(E5,AA,AA,4F), V(16,FB,FB,ED), \
    V(C5,43,43,86), V(D7,4D,4D,9A), V(55,33,33,66), V(94,85,85,11), \
    V(CF,45,45,8A), V(10,F9,F9,E9), V(06,02,02,04), V(81,7F,7F,FE), \
    V(F0,50,50,A0), V(44,3C,3C,78), V(BA,9F,9F,25), V(E3,A8,A8,4B), \
    V(F3,51,51,A2), V(FE,A3,A3,5D), V(C0,40,40,80), V(8A,8F,8F,05), \
    V(AD,92,92,3F), V(BC,9D,9D,21), V(48,38,38,70), V(04,F5,F5,F1), \
    V(DF,BC,BC,63), V(C1,B6,B6,77), V(75,DA,DA,AF), V(63,21,21,42), \
    V(30,10,10,20), V(1A,FF,FF,E5), V(0E,F3,F3,FD), V(6D,D2,D2,BF), \
    V(4C,CD,CD,81), V(14,0C,0C,18), V(35,13,13,26), V(2F,EC,EC,C3), \
    V(E1,5F,5F,BE), V(A2,97,97,35), V(CC,44,44,88), V(39,17,17,2E), \
    V(57,C4,C4,93), V(F2,A7,A7,55), V(82,7E,7E,FC), V(47,3D,3D,7A), \
    V(AC,64,64,C8), V(E7,5D,5D,BA), V(2B,19,19,32), V(95,73,73,E6), \
    V(A0,60,60,C0), V(98,81,81,19), V(D1,4F,4F,9E), V(7F,DC,DC,A3), \
    V(66,22,22,44), V(7E,2A,2A,54), V(AB,90,90,3B), V(83,88,88,0B), \
    V(CA,46,46,8C), V(29,EE,EE,C7), V(D3,B8,B8,6B), V(3C,14,14,28), \
    V(79,DE,DE,A7), V(E2,5E,5E,BC), V(1D,0B,0B,16), V(76,DB,DB,AD), \
    V(3B,E0,E0,DB), V(56,32,32,64), V(4E,3A,3A,74), V(1E,0A,0A,14), \
    V(DB,49,49,92), V(0A,06,06,0C), V(6C,24,24,48), V(E4,5C,5C,B8), \
    V(5D,C2,C2,9F), V(6E,D3,D3,BD), V(EF,AC,AC,43), V(A6,62,62,C4), \
    V(A8,91,91,39), V(A4,95,95,31), V(37,E4,E4,D3), V(8B,79,79,F2), \
    V(32,E7,E7,D5), V(43,C8,C8,8B), V(59,37,37,6E), V(B7,6D,6D,DA), \
    V(8C,8D,8D,01), V(64,D5,D5,B1), V(D2,4E,4E,9C), V(E0,A9,A9,49), \
    V(B4,6C,6C,D8), V(FA,56,56,AC), V(07,F4,F4,F3), V(25,EA,EA,CF), \
    V(AF,65,65,CA), V(8E,7A,7A,F4), V(E9,AE,AE,47), V(18,08,08,10), \
    V(D5,BA,BA,6F), V(88,78,78,F0), V(6F,25,25,4A), V(72,2E,2E,5C), \
    V(24,1C,1C,38), V(F1,A6,A6,57), V(C7,B4,B4,73), V(51,C6,C6,97), \
    V(23,E8,E8,CB), V(7C,DD,DD,A1), V(9C,74,74,E8), V(21,1F,1F,3E), \
    V(DD,4B,4B,96), V(DC,BD,BD,61), V(86,8B,8B,0D), V(85,8A,8A,0F), \
    V(90,70,70,E0), V(42,3E,3E,7C), V(C4,B5,B5,71), V(AA,66,66,CC), \
    V(D8,48,48,90), V(05,03,03,06), V(01,F6,F6,F7), V(12,0E,0E,1C), \
    V(A3,61,61,C2), V(5F,35,35,6A), V(F9,57,57,AE), V(D0,B9,B9,69), \
    V(91,86,86,17), V(58,C1,C1,99), V(27,1D,1D,3A), V(B9,9E,9E,27), \
    V(38,E1,E1,D9), V(13,F8,F8,EB), V(B3,98,98,2B), V(33,11,11,22), \
    V(BB,69,69,D2), V(70,D9,D9,A9), V(89,8E,8E,07), V(A7,94,94,33), \
    V(B6,9B,9B,2D), V(22,1E,1E,3C), V(92,87,87,15), V(20,E9,E9,C9), \
    V(49,CE,CE,87), V(FF,55,55,AA), V(78,28,28,50), V(7A,DF,DF,A5), \
    V(8F,8C,8C,03), V(F8,A1,A1,59), V(80,89,89,09), V(17,0D,0D,1A), \
    V(DA,BF,BF,65), V(31,E6,E6,D7), V(C6,42,42,84), V(B8,68,68,D0), \
    V(C3,41,41,82), V(B0,99,99,29), V(77,2D,2D,5A), V(11,0F,0F,1E), \
    V(CB,B0,B0,7B), V(FC,54,54,A8), V(D6,BB,BB,6D), V(3A,16,16,2C)

#define V(a,b,c,d) 0x##a##b##c##d
const u32 FT0[256] = { FT };
#undef V

#define V(a,b,c,d) 0x##b##c##d##a
const u32 FT1[256] = { FT };
#undef V

#define V(a,b,c,d) 0x##c##d##a##b
const u32 FT2[256] = { FT };
#undef V

#define V(a,b,c,d) 0x##d##a##b##c
const u32 FT3[256] = { FT };
#undef V

#undef FT

/*
 * Reverse S-box
 */
const u8 RSb[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38,
    0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87,
    0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D,
    0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2,
    0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA,
    0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A,
    0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02,
    0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA,
    0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85,
    0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89,
    0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20,
    0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31,
    0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D,
    0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0,
    0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26,
    0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

/*
 * Reverse tables
 */
#define RT \
\
    V(50,A7,F4,51), V(53,65,41,7E), V(C3,A4,17,1A), V(96,5E,27,3A), \
    V(CB,6B,AB,3B), V(F1,45,9D,1F), V(AB,58,FA,AC), V(93,03,E3,4B), \
    V(55,FA,30,20), V(F6,6D,76,AD), V(91,76,CC,88), V(25,4C,02,F5), \
    V(FC,D7,E5,4F), V(D7,CB,2A,C5), V(80,44,35,26), V(8F,A3,62,B5), \
    V(49,5A,B1,DE), V(67,1B,BA,25), V(98,0E,EA,45), V(E1,C0,FE,5D), \
    V(02,75,2F,C3), V(12,F0,4C,81), V(A3,97,46,8D), V(C6,F9,D3,6B), \
    V(E7,5F,8F,03), V(95,9C,92,15), V(EB,7A,6D,BF), V(DA,59,52,95), \
    V(2D,83,BE,D4), V(D3,21,74,58), V(29,69,E0,49), V(44,C8,C9,8E), \
    V(6A,89,C2,75), V(78,79,8E,F4), V(6B,3E,58,99), V(DD,71,B9,27), \
    V(B6,4F,E1,BE), V(17,AD,88,F0), V(66,AC,20,C9), V(B4,3A,CE,7D), \
    V(18,4A,DF,63), V(82,31,1A,E5), V(60,33,51,97), V(45,7F,53,62), \
    V(E0,77,64,B1), V(84,AE,6B,BB), V(1C,A0,81,FE), V(94,2B,08,F9), \
    V(58,68,48,70), V(19,FD,45,8F), V(87,6C,DE,94), V(B7,F8,7B,52), \
    V(23,D3,73,AB), V(E2,02,4B,72), V(57,8F,1F,E3), V(2A,AB,55,66), \
    V(07,28,EB,B2), V(03,C2,B5,2F), V(9A,7B,C5,86), V(A5,08,37,D3), \
    V(F2,87,28,30), V(B2,A5,BF,23), V(BA,6A,03,02), V(5C,82,16,ED), \
    V(2B,1C,CF,8A), V(92,B4,79,A7), V(F0,F2,07,F3), V(A1,E2,69,4E), \
    V(CD,F4,DA,65), V(D5,BE,05,06), V(1F,62,34,D1), V(8A,FE,A6,C4), \
    V(9D,53,2E,34), V(A0,55,F3,A2), V(32,E1,8A,05), V(75,EB,F6,A4), \
    V(39,EC,83,0B), V(AA,EF,60,40), V(06,9F,71,5E), V(51,10,6E,BD), \
    V(F9,8A,21,3E), V(3D,06,DD,96), V(AE,05,3E,DD), V(46,BD,E6,4D), \
    V(B5,8D,54,91), V(05,5D,C4,71), V(6F,D4,06,04), V(FF,15,50,60), \
    V(24,FB,98,19), V(97,E9,BD,D6), V(CC,43,40,89), V(77,9E,D9,67), \
    V(BD,42,E8,B0), V(88,8B,89,07), V(38,5B,19,E7), V(DB,EE,C8,79), \
    V(47,0A,7C,A1), V(E9,0F,42,7C), V(C9,1E,84,F8), V(00,00,00,00), \
    V(83,86,80,09), V(48,ED,2B,32), V(AC,70,11,1E), V(4E,72,5A,6C), \
    V(FB,FF,0E,FD), V(56,38,85,0F), V(1E,D5,AE,3D), V(27,39,2D,36), \
    V(64,D9,0F,0A), V(21,A6,5C,68), V(D1,54,5B,9B), V(3A,2E,36,24), \
    V(B1,67,0A,0C), V(0F,E7,57,93), V(D2,96,EE,B4), V(9E,91,9B,1B), \
    V(4F,C5,C0,80), V(A2,20,DC,61), V(69,4B,77,5A), V(16,1A,12,1C), \
    V(0A,BA,93,E2), V(E5,2A,A0,C0), V(43,E0,22,3C), V(1D,17,1B,12), \
    V(0B,0D,09,0E), V(AD,C7,8B,F2), V(B9,A8,B6,2D), V(C8,A9,1E,14), \
    V(85,19,F1,57), V(4C,07,75,AF), V(BB,DD,99,EE), V(FD,60,7F,A3), \
    V(9F,26,01,F7), V(BC,F5,72,5C), V(C5,3B,66,44), V(34,7E,FB,5B), \
    V(76,29,43,8B), V(DC,C6,23,CB), V(68,FC,ED,B6), V(63,F1,E4,B8), \
    V(CA,DC,31,D7), V(10,85,63,42), V(40,22,97,13), V(20,11,C6,84), \
    V(7D,24,4A,85), V(F8,3D,BB,D2), V(11,32,F9,AE), V(6D,A1,29,C7), \
    V(4B,2F,9E,1D), V(F3,30,B2,DC), V(EC,52,86,0D), V(D0,E3,C1,77), \
    V(6C,16,B3,2B), V(99,B9,70,A9), V(FA,48,94,11), V(22,64,E9,47), \
    V(C4,8C,FC,A8), V(1A,3F,F0,A0), V(D8,2C,7D,56), V(EF,90,33,22), \
    V(C7,4E,49,87), V(C1,D1,38,D9), V(FE,A2,CA,8C), V(36,0B,D4,98), \
    V(CF,81,F5,A6), V(28,DE,7A,A5), V(26,8E,B7,DA), V(A4,BF,AD,3F), \
    V(E4,9D,3A,2C), V(0D,92,78,50), V(9B,CC,5F,6A), V(62,46,7E,54), \
    V(C2,13,8D,F6), V(E8,B8,D8,90), V(5E,F7,39,2E), V(F5,AF,C3,82), \
    V(BE,80,5D,9F), V(7C,93,D0,69), V(A9,2D,D5,6F), V(B3,12,25,CF), \
    V(3B,99,AC,C8), V(A7,7D,18,10), V(6E,63,9C,E8), V(7B,BB,3B,DB), \
    V(09,78,26,CD), V(F4,18,59,6E), V(01,B7,9A,EC), V(A8,9A,4F,83), \
    V(65,6E,95,E6), V(7E,E6,FF,AA), V(08,CF,BC,21), V(E6,E8,15,EF), \
    V(D9,9B,E7,BA), V(CE,36,6F,4A), V(D4,09,9F,EA), V(D6,7C,B0,29), \
    V(AF,B2,A4,31), V(31,23,3F,2A), V(30,94,A5,C6), V(C0,66,A2,35), \
    V(37,BC,4E,74), V(A6,CA,82,FC), V(B0,D0,90,E0), V(15,D8,A7,33), \
    V(4A,98,04,F1), V(F7,DA,EC,41), V(0E,50,CD,7F), V(2F,F6,91,17), \
    V(8D,D6,4D,76), V(4D,B0,EF,43), V(54,4D,AA,CC), V(DF,04,96,E4), \
    V(E3,B5,D1,9E), V(1B,88,6A,4C), V(B8,1F,2C,C1), V(7F,51,65,46), \
    V(04,EA,5E,9D), V(5D,35,8C,01), V(73,74,87,FA), V(2E,41,0B,FB), \
    V(5A,1D,67,B3), V(52,D2,DB,92), V(33,56,10,E9), V(13,47,D6,6D), \
    V(8C,61,D7,9A), V(7A,0C,A1,37), V(8E,14,F8,59), V(89,3C,13,EB), \
    V(EE,27,A9,CE), V(35,C9,61,B7), V(ED,E5,1C,E1), V(3C,B1,47,7A), \
    V(59,DF,D2,9C), V(3F,73,F2,55), V(79,CE,14,18), V(BF,37,C7,73), \
    V(EA,CD,F7,53), V(5B,AA,FD,5F), V(14,6F,3D,DF), V(86,DB,44,78), \
    V(81,F3,AF,CA), V(3E,C4,68,B9), V(2C,34,24,38), V(5F,40,A3,C2), \
    V(72,C3,1D,16), V(0C,25,E2,BC), V(8B,49,3C,28), V(41,95,0D,FF), \
    V(71,01,A8,39), V(DE,B3,0C,08), V(9C,E4,B4,D8), V(90,C1,56,64), \
    V(61,84,CB,7B), V(70,B6,32,D5), V(74,5C,6C,48), V(42,57,B8,D0)

#define V(a,b,c,d) 0x##a##b##c##d
const u32 RT0[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##b##c##d##a
const u32 RT1[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##c##d##a##b
const u32 RT2[256] = { RT };
#undef V

#define V(a,b,c,d) 0x##d##a##b##c
const u32 RT3[256] = { RT };
#undef V

#undef RT

/*
 * Round constants
 */
const u32 RCON[10] = {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x0000001B, 0x00000036
};

#endif

#if 0
//#else

/*
 * Forward S-box & tables
 */
static u8 FSb[256];
static u32 FT0[256];
static u32 FT1[256];
static u32 FT2[256];
static u32 FT3[256];

/*
 * Reverse S-box & tables
 */
static u8 RSb[256];
static u32 RT0[256];
static u32 RT1[256];
static u32 RT2[256];
static u32 RT3[256];

/*
 * Round constants
 */
static u32 RCON[10];

/*
 * Tables generation code
 */
#define ROTL8(x) ( ( x << 8 ) & 0xFFFFFFFF ) | ( x >> 24 )
#define XTIME(x) ( ( x << 1 ) ^ ( ( x & 0x80 ) ? 0x1B : 0x00 ) )
#define MUL(x,y) ( ( x && y ) ? pow[(log[x]+log[y]) % 255] : 0 )

static s32 aes_init_done = 0;

static void aes_gen_tables(void)
{
    s32 i, x, y, z;
    s32 pow[256];
    s32 log[256];

    /*
     * compute pow and log tables over GF(2^8)
     */
    for (i = 0, x = 1; i < 256; i++) {
        pow[i] = x;
        log[x] = i;
        x = (x ^ XTIME(x)) & 0xFF;
    }

    /*
     * calculate the round constants
     */
    for (i = 0, x = 1; i < 10; i++) {
        RCON[i] = (u32) x;
        x = XTIME(x) & 0xFF;
    }

    /*
     * generate the forward and reverse S-boxes
     */
    FSb[0x00] = 0x63;
    RSb[0x63] = 0x00;

    for (i = 1; i < 256; i++) {
        x = pow[255 - log[i]];

        y  = x; y = ((y << 1) | (y >> 7)) & 0xFF;
        x ^= y; y = ((y << 1) | (y >> 7)) & 0xFF;
        x ^= y; y = ((y << 1) | (y >> 7)) & 0xFF;
        x ^= y; y = ((y << 1) | (y >> 7)) & 0xFF;
        x ^= y ^ 0x63;

        FSb[i] = (u8) x;
        RSb[x] = (u8) i;
    }

    /*
     * generate the forward and reverse tables
     */
    for (i = 0; i < 256; i++) {
        x = FSb[i];
        y = XTIME(x) & 0xFF;
        z = (y ^ x) & 0xFF;

        FT0[i] = ((u32) y) ^
                 ((u32) x <<  8) ^
                 ((u32) x << 16) ^
                 ((u32) z << 24);

        FT1[i] = ROTL8(FT0[i]);
        FT2[i] = ROTL8(FT1[i]);
        FT3[i] = ROTL8(FT2[i]);

        x = RSb[i];

        RT0[i] = ((u32) MUL(0x0E, x)) ^
                 ((u32) MUL(0x09, x) <<  8) ^
                 ((u32) MUL(0x0D, x) << 16) ^
                 ((u32) MUL(0x0B, x) << 24);

        RT1[i] = ROTL8(RT0[i]);
        RT2[i] = ROTL8(RT1[i]);
        RT3[i] = ROTL8(RT2[i]);
    }
}
#endif

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    rtlc_memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx == NULL)
        return;

    mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

/*
 * AES key schedule (encryption)
 */
s32 mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const u8 *key,
                           u32 keybits)
{
    u32 i;
    u32 *RK;

    switch (keybits) {
    case 128: ctx->nr = 10; break;
    case 192: ctx->nr = 12; break;
    case 256: ctx->nr = 14; break;
    default : return (MBEDTLS_ERR_AES_INVALID_KEY_LENGTH);
    }
#if 0
#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_PADLOCK_ALIGN16)
    if (aes_padlock_ace == -1)
        aes_padlock_ace = mbedtls_padlock_has_support(MBEDTLS_PADLOCK_ACE);

    if (aes_padlock_ace)
        ctx->rk = RK = MBEDTLS_PADLOCK_ALIGN16(ctx->buf);
    else
#endif
#endif

        ctx->rk = RK = ctx->buf;

    for (i = 0; i < (keybits >> 5); i++) {
        GET_UINT32_LE(RK[i], key, i << 2);
    }

    switch (ctx->nr) {
    case 10:

        for (i = 0; i < 10; i++, RK += 4) {
            RK[4]  = RK[0] ^ RCON[i] ^
                     ((u32) FSb[(RK[3] >>  8) & 0xFF ]) ^
                     ((u32) FSb[(RK[3] >> 16) & 0xFF ] <<  8) ^
                     ((u32) FSb[(RK[3] >> 24) & 0xFF ] << 16) ^
                     ((u32) FSb[(RK[3]) & 0xFF ] << 24);

            RK[5]  = RK[1] ^ RK[4];
            RK[6]  = RK[2] ^ RK[5];
            RK[7]  = RK[3] ^ RK[6];
        }
        break;

    case 12:

        for (i = 0; i < 8; i++, RK += 6) {
            RK[6]  = RK[0] ^ RCON[i] ^
                     ((u32) FSb[(RK[5] >>  8) & 0xFF ]) ^
                     ((u32) FSb[(RK[5] >> 16) & 0xFF ] <<  8) ^
                     ((u32) FSb[(RK[5] >> 24) & 0xFF ] << 16) ^
                     ((u32) FSb[(RK[5]) & 0xFF ] << 24);

            RK[7]  = RK[1] ^ RK[6];
            RK[8]  = RK[2] ^ RK[7];
            RK[9]  = RK[3] ^ RK[8];
            RK[10] = RK[4] ^ RK[9];
            RK[11] = RK[5] ^ RK[10];
        }
        break;

    case 14:

        for (i = 0; i < 7; i++, RK += 8) {
            RK[8]  = RK[0] ^ RCON[i] ^
                     ((u32) FSb[(RK[7] >>  8) & 0xFF ]) ^
                     ((u32) FSb[(RK[7] >> 16) & 0xFF ] <<  8) ^
                     ((u32) FSb[(RK[7] >> 24) & 0xFF ] << 16) ^
                     ((u32) FSb[(RK[7]) & 0xFF ] << 24);

            RK[9]  = RK[1] ^ RK[8];
            RK[10] = RK[2] ^ RK[9];
            RK[11] = RK[3] ^ RK[10];

            RK[12] = RK[4] ^
                     ((u32) FSb[(RK[11]) & 0xFF ]) ^
                     ((u32) FSb[(RK[11] >>  8) & 0xFF ] <<  8) ^
                     ((u32) FSb[(RK[11] >> 16) & 0xFF ] << 16) ^
                     ((u32) FSb[(RK[11] >> 24) & 0xFF ] << 24);

            RK[13] = RK[5] ^ RK[12];
            RK[14] = RK[6] ^ RK[13];
            RK[15] = RK[7] ^ RK[14];
        }
        break;
    }

    return (0);
}

/*
 * AES key schedule (decryption)
 */
s32 mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const u8 *key,
                           u32 keybits)
{
    s32 i, j, ret;
    mbedtls_aes_context cty;
    u32 *RK;
    u32 *SK;

    mbedtls_aes_init(&cty);

#if 0
#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_PADLOCK_ALIGN16)
    if (aes_padlock_ace == -1)
        aes_padlock_ace = mbedtls_padlock_has_support(MBEDTLS_PADLOCK_ACE);

    if (aes_padlock_ace)
        ctx->rk = RK = MBEDTLS_PADLOCK_ALIGN16(ctx->buf);
    else
#endif
#endif
        ctx->rk = RK = ctx->buf;

    /* Also checks keybits */
    if ((ret = mbedtls_aes_setkey_enc(&cty, key, keybits)) != 0)
        goto exit;

    ctx->nr = cty.nr;

    SK = cty.rk + cty.nr * 4;

    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;

    for (i = ctx->nr - 1, SK -= 8; i > 0; i--, SK -= 8) {
        for (j = 0; j < 4; j++, SK++) {
            *RK++ = RT0[ FSb[(*SK) & 0xFF ] ] ^
                    RT1[ FSb[(*SK >>  8) & 0xFF ] ] ^
                    RT2[ FSb[(*SK >> 16) & 0xFF ] ] ^
                    RT3[ FSb[(*SK >> 24) & 0xFF ] ];
        }
    }

    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;

exit:
    mbedtls_aes_free(&cty);

    return (ret);
}

#define AES_FROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)     \
{                                               \
    X0 = *RK++ ^ FT0[ ( Y0       ) & 0xFF ] ^   \
                 FT1[ ( Y1 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y2 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y3 >> 24 ) & 0xFF ];    \
                                                \
    X1 = *RK++ ^ FT0[ ( Y1       ) & 0xFF ] ^   \
                 FT1[ ( Y2 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y3 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y0 >> 24 ) & 0xFF ];    \
                                                \
    X2 = *RK++ ^ FT0[ ( Y2       ) & 0xFF ] ^   \
                 FT1[ ( Y3 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y0 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y1 >> 24 ) & 0xFF ];    \
                                                \
    X3 = *RK++ ^ FT0[ ( Y3       ) & 0xFF ] ^   \
                 FT1[ ( Y0 >>  8 ) & 0xFF ] ^   \
                 FT2[ ( Y1 >> 16 ) & 0xFF ] ^   \
                 FT3[ ( Y2 >> 24 ) & 0xFF ];    \
}

#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)     \
{                                               \
    X0 = *RK++ ^ RT0[ ( Y0       ) & 0xFF ] ^   \
                 RT1[ ( Y3 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y2 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y1 >> 24 ) & 0xFF ];    \
                                                \
    X1 = *RK++ ^ RT0[ ( Y1       ) & 0xFF ] ^   \
                 RT1[ ( Y0 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y3 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y2 >> 24 ) & 0xFF ];    \
                                                \
    X2 = *RK++ ^ RT0[ ( Y2       ) & 0xFF ] ^   \
                 RT1[ ( Y1 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y0 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y3 >> 24 ) & 0xFF ];    \
                                                \
    X3 = *RK++ ^ RT0[ ( Y3       ) & 0xFF ] ^   \
                 RT1[ ( Y2 >>  8 ) & 0xFF ] ^   \
                 RT2[ ( Y1 >> 16 ) & 0xFF ] ^   \
                 RT3[ ( Y0 >> 24 ) & 0xFF ];    \
}

/*
 * AES-ECB block encryption
 */
s32 mbedtls_internal_aes_encrypt(mbedtls_aes_context *ctx,
                                 const u8 input[16],
                                 u8 output[16])
{
    s32 i;
    u32 *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

    RK = ctx->rk;

    GET_UINT32_LE(X0, input,  0); X0 ^= *RK++;
    GET_UINT32_LE(X1, input,  4); X1 ^= *RK++;
    GET_UINT32_LE(X2, input,  8); X2 ^= *RK++;
    GET_UINT32_LE(X3, input, 12); X3 ^= *RK++;

    for (i = (ctx->nr >> 1) - 1; i > 0; i--) {
        AES_FROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);
        AES_FROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3);
    }

    AES_FROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);

    X0 = *RK++ ^ \
         ((u32) FSb[(Y0) & 0xFF ]) ^
         ((u32) FSb[(Y1 >>  8) & 0xFF ] <<  8) ^
         ((u32) FSb[(Y2 >> 16) & 0xFF ] << 16) ^
         ((u32) FSb[(Y3 >> 24) & 0xFF ] << 24);

    X1 = *RK++ ^ \
         ((u32) FSb[(Y1) & 0xFF ]) ^
         ((u32) FSb[(Y2 >>  8) & 0xFF ] <<  8) ^
         ((u32) FSb[(Y3 >> 16) & 0xFF ] << 16) ^
         ((u32) FSb[(Y0 >> 24) & 0xFF ] << 24);

    X2 = *RK++ ^ \
         ((u32) FSb[(Y2) & 0xFF ]) ^
         ((u32) FSb[(Y3 >>  8) & 0xFF ] <<  8) ^
         ((u32) FSb[(Y0 >> 16) & 0xFF ] << 16) ^
         ((u32) FSb[(Y1 >> 24) & 0xFF ] << 24);

    X3 = *RK++ ^ \
         ((u32) FSb[(Y3) & 0xFF ]) ^
         ((u32) FSb[(Y0 >>  8) & 0xFF ] <<  8) ^
         ((u32) FSb[(Y1 >> 16) & 0xFF ] << 16) ^
         ((u32) FSb[(Y2 >> 24) & 0xFF ] << 24);

    PUT_UINT32_LE(X0, output,  0);
    PUT_UINT32_LE(X1, output,  4);
    PUT_UINT32_LE(X2, output,  8);
    PUT_UINT32_LE(X3, output, 12);

    return (0);
}

/*
 * AES-ECB block decryption
 */
s32 mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx,
                                 const u8 input[16],
                                 u8 output[16])
{
    s32 i;
    u32 *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

    RK = ctx->rk;

    GET_UINT32_LE(X0, input,  0); X0 ^= *RK++;
    GET_UINT32_LE(X1, input,  4); X1 ^= *RK++;
    GET_UINT32_LE(X2, input,  8); X2 ^= *RK++;
    GET_UINT32_LE(X3, input, 12); X3 ^= *RK++;

    for (i = (ctx->nr >> 1) - 1; i > 0; i--) {
        AES_RROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);
        AES_RROUND(X0, X1, X2, X3, Y0, Y1, Y2, Y3);
    }

    AES_RROUND(Y0, Y1, Y2, Y3, X0, X1, X2, X3);

    X0 = *RK++ ^ \
         ((u32) RSb[(Y0) & 0xFF ]) ^
         ((u32) RSb[(Y3 >>  8) & 0xFF ] <<  8) ^
         ((u32) RSb[(Y2 >> 16) & 0xFF ] << 16) ^
         ((u32) RSb[(Y1 >> 24) & 0xFF ] << 24);

    X1 = *RK++ ^ \
         ((u32) RSb[(Y1) & 0xFF ]) ^
         ((u32) RSb[(Y0 >>  8) & 0xFF ] <<  8) ^
         ((u32) RSb[(Y3 >> 16) & 0xFF ] << 16) ^
         ((u32) RSb[(Y2 >> 24) & 0xFF ] << 24);

    X2 = *RK++ ^ \
         ((u32) RSb[(Y2) & 0xFF ]) ^
         ((u32) RSb[(Y1 >>  8) & 0xFF ] <<  8) ^
         ((u32) RSb[(Y0 >> 16) & 0xFF ] << 16) ^
         ((u32) RSb[(Y3 >> 24) & 0xFF ] << 24);

    X3 = *RK++ ^ \
         ((u32) RSb[(Y3) & 0xFF ]) ^
         ((u32) RSb[(Y2 >>  8) & 0xFF ] <<  8) ^
         ((u32) RSb[(Y1 >> 16) & 0xFF ] << 16) ^
         ((u32) RSb[(Y0 >> 24) & 0xFF ] << 24);

    PUT_UINT32_LE(X0, output,  0);
    PUT_UINT32_LE(X1, output,  4);
    PUT_UINT32_LE(X2, output,  8);
    PUT_UINT32_LE(X3, output, 12);

    return (0);
}

/*
 * AES-ECB block encryption/decryption
 */
s32 mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx,
                          s32 mode,
                          const u8 input[16],
                          u8 output[16])
{

#if 0
#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_HAVE_X86)
    if (aes_padlock_ace) {
        if (mbedtls_padlock_xcryptecb(ctx, mode, input, output) == 0)
            return (0);

        // If padlock data misaligned, we just fall back to
        // unaccelerated mode
        //
    }
#endif
#endif

    if (mode == MBEDTLS_AES_ENCRYPT)
        return (mbedtls_internal_aes_encrypt(ctx, input, output));
    else
        return (mbedtls_internal_aes_decrypt(ctx, input, output));
}

/*
 * AES-CBC buffer encryption/decryption
 */
s32 mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                          s32 mode,
                          u32 length,
                          u8 iv[16],
                          const u8 *input,
                          u8 *output)
{
    s32 i;
    u8 temp[16];

    if (length % 16)
        return (MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH);

#if 0
#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_HAVE_X86)
    if (aes_padlock_ace) {
        if (mbedtls_padlock_xcryptcbc(ctx, mode, length, iv, input, output) == 0)
            return (0);

        // If padlock data misaligned, we just fall back to
        // unaccelerated mode
        //
    }
#endif
#endif

    if (mode == MBEDTLS_AES_DECRYPT) {
        while (length > 0) {
            rtlc_memcpy(temp, input, 16);
            mbedtls_aes_crypt_ecb(ctx, mode, input, output);

            for (i = 0; i < 16; i++)
                output[i] = (u8)(output[i] ^ iv[i]);

            rtlc_memcpy(iv, temp, 16);

            input  += 16;
            output += 16;
            length -= 16;
        }
    } else {
        while (length > 0) {
            for (i = 0; i < 16; i++)
                output[i] = (u8)(input[i] ^ iv[i]);

            mbedtls_aes_crypt_ecb(ctx, mode, output, output);
            rtlc_memcpy(iv, output, 16);

            input  += 16;
            output += 16;
            length -= 16;
        }
    }

    return (0);
}

/*
 * AES-CFB128 buffer encryption/decryption
 */
s32 mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
                             s32 mode,
                             size_t length,
                             size_t *iv_off,
                             u8 iv[16],
                             const u8 *input,
                             u8 *output)
{
    s32 c;
    size_t n = *iv_off;

    if (mode == MBEDTLS_AES_DECRYPT) {
        while (length--) {
            if (n == 0)
                mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);

            c = *input++;
            *output++ = (u8)(c ^ iv[n]);
            iv[n] = (u8) c;

            n = (n + 1) & 0x0F;
        }
    } else {
        while (length--) {
            if (n == 0)
                mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);

            iv[n] = *output++ = (u8)(iv[n] ^ *input++);

            n = (n + 1) & 0x0F;
        }
    }

    *iv_off = n;

    return (0);
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
#if 0
s32 mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx,
                           s32 mode,
                           u32 length,
                           u8 iv[16],
                           const u8 *input,
                           u8 *output)
{
    u8 c;
    u8 ov[17];

    while (length--) {
        rtlc_memcpy(ov, iv, 16);
        mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);

        if (mode == MBEDTLS_AES_DECRYPT)
            ov[16] = *input;

        c = *output++ = (u8)(iv[0] ^ *input++);

        if (mode == MBEDTLS_AES_ENCRYPT)
            ov[16] = c;

        rtlc_memcpy(iv, ov + 1, 16);
    }

    return (0);
}
#endif

/*
 * AES-OFB128 buffer encryption/decryption
 */
s32 mbedtls_aes_crypt_ofb128(mbedtls_aes_context *ctx,
                             u32 length,
                             size_t *iv_off,
                             u8 iv1[16],
                             const u8 *input,
                             u8 *output)
{
    u32 n = *iv_off;
    u8 iv[16];

    rtlc_memcpy(iv, iv1, 16);

    {
        while (length --) {
            if (n == 0) {
                mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
            }
            *output++ = (u8)(iv[n] ^ *input++);
            n = (n + 1) & 0x0F;

        }

    }

    *iv_off = n;

    return (0);

}

/*
 * AES-CTR buffer encryption/decryption
 */
s32 mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          u8 nonce_counter[16],
                          u8 stream_block[16],
                          const u8 *input,
                          u8 *output)
{
    s32 c, i;
    size_t n = *nc_off;

    while (length--) {
        if (n == 0) {
            mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block);

            for (i = 16; i > 0; i--)
                if (++nonce_counter[i - 1] != 0)
                    break;
        }
        c = *input++;
        *output++ = (u8)(c ^ stream_block[n]);

        n = (n + 1) & 0x0F;
    }

    *nc_off = n;

    return (0);
}

/*
 * AES test vectors from:
 *
 * http://csrc.nist.gov/archive/aes/rijndael/rijndael-vals.zip
 */
#if 0
static const u8 aes_test_ecb_dec[3][16] = {
    {
        0x44, 0x41, 0x6A, 0xC2, 0xD1, 0xF5, 0x3C, 0x58,
        0x33, 0x03, 0x91, 0x7E, 0x6B, 0xE9, 0xEB, 0xE0
    },
    {
        0x48, 0xE3, 0x1E, 0x9E, 0x25, 0x67, 0x18, 0xF2,
        0x92, 0x29, 0x31, 0x9C, 0x19, 0xF1, 0x5B, 0xA4
    },
    {
        0x05, 0x8C, 0xCF, 0xFD, 0xBB, 0xCB, 0x38, 0x2D,
        0x1F, 0x6F, 0x56, 0x58, 0x5D, 0x8A, 0x4A, 0xDE
    }
};

static const u8 aes_test_ecb_enc[3][16] = {
    {
        0xC3, 0x4C, 0x05, 0x2C, 0xC0, 0xDA, 0x8D, 0x73,
        0x45, 0x1A, 0xFE, 0x5F, 0x03, 0xBE, 0x29, 0x7F
    },
    {
        0xF3, 0xF6, 0x75, 0x2A, 0xE8, 0xD7, 0x83, 0x11,
        0x38, 0xF0, 0x41, 0x56, 0x06, 0x31, 0xB1, 0x14
    },
    {
        0x8B, 0x79, 0xEE, 0xCC, 0x93, 0xA0, 0xEE, 0x5D,
        0xFF, 0x30, 0xB4, 0xEA, 0x21, 0x63, 0x6D, 0xA4
    }
};

static const u8 aes_test_cbc_dec[3][16] = {
    {
        0xFA, 0xCA, 0x37, 0xE0, 0xB0, 0xC8, 0x53, 0x73,
        0xDF, 0x70, 0x6E, 0x73, 0xF7, 0xC9, 0xAF, 0x86
    },
    {
        0x5D, 0xF6, 0x78, 0xDD, 0x17, 0xBA, 0x4E, 0x75,
        0xB6, 0x17, 0x68, 0xC6, 0xAD, 0xEF, 0x7C, 0x7B
    },
    {
        0x48, 0x04, 0xE1, 0x81, 0x8F, 0xE6, 0x29, 0x75,
        0x19, 0xA3, 0xE8, 0x8C, 0x57, 0x31, 0x04, 0x13
    }
};

static const u8 aes_test_cbc_enc[3][16] = {
    {
        0x8A, 0x05, 0xFC, 0x5E, 0x09, 0x5A, 0xF4, 0x84,
        0x8A, 0x08, 0xD3, 0x28, 0xD3, 0x68, 0x8E, 0x3D
    },
    {
        0x7B, 0xD9, 0x66, 0xD5, 0x3A, 0xD8, 0xC1, 0xBB,
        0x85, 0xD2, 0xAD, 0xFA, 0xE8, 0x7B, 0xB1, 0x04
    },
    {
        0xFE, 0x3C, 0x53, 0x65, 0x3E, 0x2F, 0x45, 0xB5,
        0x6F, 0xCD, 0x88, 0xB2, 0xCC, 0x89, 0x8F, 0xF0
    }
};

/*
 * AES-CFB128 test vectors from:
 *
 * http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 */

static const u8 aes_test_cfb128_key[3][32] = {
    {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    },
    {
        0x8E, 0x73, 0xB0, 0xF7, 0xDA, 0x0E, 0x64, 0x52,
        0xC8, 0x10, 0xF3, 0x2B, 0x80, 0x90, 0x79, 0xE5,
        0x62, 0xF8, 0xEA, 0xD2, 0x52, 0x2C, 0x6B, 0x7B
    },
    {
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    }
};

static const u8 aes_test_cfb128_iv[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

static const u8 aes_test_cfb128_pt[64] = {
    0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
    0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
    0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C,
    0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
    0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11,
    0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF,
    0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17,
    0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10
};

static const u8 aes_test_cfb128_ct[3][64] = {
    {
        0x3B, 0x3F, 0xD9, 0x2E, 0xB7, 0x2D, 0xAD, 0x20,
        0x33, 0x34, 0x49, 0xF8, 0xE8, 0x3C, 0xFB, 0x4A,
        0xC8, 0xA6, 0x45, 0x37, 0xA0, 0xB3, 0xA9, 0x3F,
        0xCD, 0xE3, 0xCD, 0xAD, 0x9F, 0x1C, 0xE5, 0x8B,
        0x26, 0x75, 0x1F, 0x67, 0xA3, 0xCB, 0xB1, 0x40,
        0xB1, 0x80, 0x8C, 0xF1, 0x87, 0xA4, 0xF4, 0xDF,
        0xC0, 0x4B, 0x05, 0x35, 0x7C, 0x5D, 0x1C, 0x0E,
        0xEA, 0xC4, 0xC6, 0x6F, 0x9F, 0xF7, 0xF2, 0xE6
    },
    {
        0xCD, 0xC8, 0x0D, 0x6F, 0xDD, 0xF1, 0x8C, 0xAB,
        0x34, 0xC2, 0x59, 0x09, 0xC9, 0x9A, 0x41, 0x74,
        0x67, 0xCE, 0x7F, 0x7F, 0x81, 0x17, 0x36, 0x21,
        0x96, 0x1A, 0x2B, 0x70, 0x17, 0x1D, 0x3D, 0x7A,
        0x2E, 0x1E, 0x8A, 0x1D, 0xD5, 0x9B, 0x88, 0xB1,
        0xC8, 0xE6, 0x0F, 0xED, 0x1E, 0xFA, 0xC4, 0xC9,
        0xC0, 0x5F, 0x9F, 0x9C, 0xA9, 0x83, 0x4F, 0xA0,
        0x42, 0xAE, 0x8F, 0xBA, 0x58, 0x4B, 0x09, 0xFF
    },
    {
        0xDC, 0x7E, 0x84, 0xBF, 0xDA, 0x79, 0x16, 0x4B,
        0x7E, 0xCD, 0x84, 0x86, 0x98, 0x5D, 0x38, 0x60,
        0x39, 0xFF, 0xED, 0x14, 0x3B, 0x28, 0xB1, 0xC8,
        0x32, 0x11, 0x3C, 0x63, 0x31, 0xE5, 0x40, 0x7B,
        0xDF, 0x10, 0x13, 0x24, 0x15, 0xE5, 0x4B, 0x92,
        0xA1, 0x3E, 0xD0, 0xA8, 0x26, 0x7A, 0xE2, 0xF9,
        0x75, 0xA3, 0x85, 0x74, 0x1A, 0xB9, 0xCE, 0xF8,
        0x20, 0x31, 0x62, 0x3D, 0x55, 0xB1, 0xE4, 0x71
    }
};

/*
 * AES-CTR test vectors from:
 *
 * http://www.faqs.org/rfcs/rfc3686.html
 */

static const u8 aes_test_ctr_key[3][16] = {
    {
        0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67, 0xCC,
        0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E
    },
    {
        0x7E, 0x24, 0x06, 0x78, 0x17, 0xFA, 0xE0, 0xD7,
        0x43, 0xD6, 0xCE, 0x1F, 0x32, 0x53, 0x91, 0x63
    },
    {
        0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8,
        0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC
    }
};

static const u8 aes_test_ctr_nonce_counter[3][16] = {
    {
        0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
    },
    {
        0x00, 0x6C, 0xB6, 0xDB, 0xC0, 0x54, 0x3B, 0x59,
        0xDA, 0x48, 0xD9, 0x0B, 0x00, 0x00, 0x00, 0x01
    },
    {
        0x00, 0xE0, 0x01, 0x7B, 0x27, 0x77, 0x7F, 0x3F,
        0x4A, 0x17, 0x86, 0xF0, 0x00, 0x00, 0x00, 0x01
    }
};

static const u8 aes_test_ctr_pt[3][48] = {
    {
        0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
        0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    },

    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    },

    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23
    }
};

static const u8 aes_test_ctr_ct[3][48] = {
    {
        0xE4, 0x09, 0x5D, 0x4F, 0xB7, 0xA7, 0xB3, 0x79,
        0x2D, 0x61, 0x75, 0xA3, 0x26, 0x13, 0x11, 0xB8
    },
    {
        0x51, 0x04, 0xA1, 0x06, 0x16, 0x8A, 0x72, 0xD9,
        0x79, 0x0D, 0x41, 0xEE, 0x8E, 0xDA, 0xD3, 0x88,
        0xEB, 0x2E, 0x1E, 0xFC, 0x46, 0xDA, 0x57, 0xC8,
        0xFC, 0xE6, 0x30, 0xDF, 0x91, 0x41, 0xBE, 0x28
    },
    {
        0xC1, 0xCF, 0x48, 0xA8, 0x9F, 0x2F, 0xFD, 0xD9,
        0xCF, 0x46, 0x52, 0xE9, 0xEF, 0xDB, 0x72, 0xD7,
        0x45, 0x40, 0xA4, 0x2B, 0xDE, 0x6D, 0x78, 0x36,
        0xD5, 0x9A, 0x5C, 0xEA, 0xAE, 0xF3, 0x10, 0x53,
        0x25, 0xB2, 0x07, 0x2F
    }
};

static const s32 aes_test_ctr_len[3] =
{ 16, 32, 36 };
#endif
#if 0
/*
 * Checkup routine
 */
s32 mbedtls_aes_self_test(s32 verbose)
{
    s32 ret = 0, i, j, u, v;
    u8 key[32];
    u8 buf[64];
    u8 iv[16];
    u8 prv[16];
    u32 offset;
    s32 len;
    u8 nonce_counter[16];
    u8 stream_block[16];
    mbedtls_aes_context ctx;

    rtlc_memset(key, 0, 32);
    mbedtls_aes_init(&ctx);

    /*
     * ECB mode
     */
    for (i = 0; i < 6; i++) {
        u = i >> 1;
        v = i  & 1;

        if (verbose != 0)
            mbedtls_prs32f("  AES-ECB-%3d (%s): ", 128 + u * 64,
                           (v == MBEDTLS_AES_DECRYPT) ? "dec" : "enc");

        rtlc_memset(buf, 0, 16);

        if (v == MBEDTLS_AES_DECRYPT) {
            mbedtls_aes_setkey_dec(&ctx, key, 128 + u * 64);

            for (j = 0; j < 10000; j++)
                mbedtls_aes_crypt_ecb(&ctx, v, buf, buf);

            if (crypto_self_memcmp(buf, aes_test_ecb_dec[u], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        } else {
            mbedtls_aes_setkey_enc(&ctx, key, 128 + u * 64);

            for (j = 0; j < 10000; j++)
                mbedtls_aes_crypt_ecb(&ctx, v, buf, buf);

            if (crypto_self_memcmp(buf, aes_test_ecb_enc[u], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        }

        if (verbose != 0)
            mbedtls_prs32f("passed\n");
    }

    if (verbose != 0)
        mbedtls_prs32f("\n");

    /*
     * CBC mode
     */
    for (i = 0; i < 6; i++) {
        u = i >> 1;
        v = i  & 1;

        if (verbose != 0)
            mbedtls_prs32f("  AES-CBC-%3d (%s): ", 128 + u * 64,
                           (v == MBEDTLS_AES_DECRYPT) ? "dec" : "enc");

        rtlc_memset(iv, 0, 16);
        rtlc_memset(prv, 0, 16);
        rtlc_memset(buf, 0, 16);

        if (v == MBEDTLS_AES_DECRYPT) {
            mbedtls_aes_setkey_dec(&ctx, key, 128 + u * 64);

            for (j = 0; j < 10000; j++)
                mbedtls_aes_crypt_cbc(&ctx, v, 16, iv, buf, buf);

            if (crypto_self_memcmp(buf, aes_test_cbc_dec[u], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        } else {
            mbedtls_aes_setkey_enc(&ctx, key, 128 + u * 64);

            for (j = 0; j < 10000; j++) {
                u8 tmp[16];

                mbedtls_aes_crypt_cbc(&ctx, v, 16, iv, buf, buf);

                rtlc_memcpy(tmp, prv, 16);
                rtlc_memcpy(prv, buf, 16);
                rtlc_memcpy(buf, tmp, 16);
            }

            if (crypto_self_memcmp(prv, aes_test_cbc_enc[u], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        }

        if (verbose != 0)
            mbedtls_prs32f("passed\n");
    }

    if (verbose != 0)
        mbedtls_prs32f("\n");
    /*
     * CFB128 mode
     */
    for (i = 0; i < 6; i++) {
        u = i >> 1;
        v = i  & 1;

        if (verbose != 0)
            mbedtls_prs32f("  AES-CFB128-%3d (%s): ", 128 + u * 64,
                           (v == MBEDTLS_AES_DECRYPT) ? "dec" : "enc");

        rtlc_memcpy(iv,  aes_test_cfb128_iv, 16);
        rtlc_memcpy(key, aes_test_cfb128_key[u], 16 + u * 8);

        offset = 0;
        mbedtls_aes_setkey_enc(&ctx, key, 128 + u * 64);

        if (v == MBEDTLS_AES_DECRYPT) {
            rtlc_memcpy(buf, aes_test_cfb128_ct[u], 64);
            mbedtls_aes_crypt_cfb128(&ctx, v, 64, &offset, iv, buf, buf);

            if (crypto_self_memcmp(buf, aes_test_cfb128_pt, 64) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        } else {
            rtlc_memcpy(buf, aes_test_cfb128_pt, 64);
            mbedtls_aes_crypt_cfb128(&ctx, v, 64, &offset, iv, buf, buf);

            if (crypto_self_memcmp(buf, aes_test_cfb128_ct[u], 64) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        }

        if (verbose != 0)
            mbedtls_prs32f("passed\n");
    }

    if (verbose != 0)
        mbedtls_prs32f("\n");
    /*
     * CTR mode
     */
    for (i = 0; i < 6; i++) {
        u = i >> 1;
        v = i  & 1;

        if (verbose != 0)
            mbedtls_prs32f("  AES-CTR-128 (%s): ",
                           (v == MBEDTLS_AES_DECRYPT) ? "dec" : "enc");

        rtlc_memcpy(nonce_counter, aes_test_ctr_nonce_counter[u], 16);
        rtlc_memcpy(key, aes_test_ctr_key[u], 16);

        offset = 0;
        mbedtls_aes_setkey_enc(&ctx, key, 128);

        if (v == MBEDTLS_AES_DECRYPT) {
            len = aes_test_ctr_len[u];
            rtlc_memcpy(buf, aes_test_ctr_ct[u], len);

            mbedtls_aes_crypt_ctr(&ctx, len, &offset, nonce_counter, stream_block,
                                  buf, buf);

            if (crypto_self_memcmp(buf, aes_test_ctr_pt[u], len) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        } else {
            len = aes_test_ctr_len[u];
            rtlc_memcpy(buf, aes_test_ctr_pt[u], len);

            mbedtls_aes_crypt_ctr(&ctx, len, &offset, nonce_counter, stream_block,
                                  buf, buf);

            if (crypto_self_memcmp(buf, aes_test_ctr_ct[u], len) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                ret = 1;
                goto exit;
            }
        }

        if (verbose != 0)
            mbedtls_prs32f("passed\n");
    }

    if (verbose != 0)
        mbedtls_prs32f("\n");

    ret = 0;

exit:
    mbedtls_aes_free(&ctx);

    return (ret);
}
#endif

/* ------------------------------ GCM ------------------------------ */
/*
 * 32-bit s32eger manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (u32) (b)[(i)    ] << 24 )             \
        | ( (u32) (b)[(i) + 1] << 16 )             \
        | ( (u32) (b)[(i) + 2] <<  8 )             \
        | ( (u32) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (u8) ( (n) >> 24 );       \
    (b)[(i) + 1] = (u8) ( (n) >> 16 );       \
    (b)[(i) + 2] = (u8) ( (n) >>  8 );       \
    (b)[(i) + 3] = (u8) ( (n)       );       \
}
#endif

/* Implementation that should never be optimized out by the compiler */
/*
void mbedtls_zeroize( void *v, u32 n ) {
    volatile u8 *p = v; while( n-- ) *p++ = 0;
}
*/
/*
 * Initialize a context
 */
void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
    rtlc_memset(ctx, 0, sizeof(mbedtls_gcm_context));
}

/*
 * Precompute small multiples of H, that is set
 *      HH[i] || HL[i] = H times i,
 * where i is seen as a field element as in [MGV], ie high-order bits
 * correspond to low powers of P. The result is stored in the same way, that
 * is the high-order bit of HH corresponds to P^0 and the low-order bit of HL
 * corresponds to P^127.
 */
s32 gcm_gen_table(mbedtls_gcm_context *ctx)
{
    s32 ret, i, j;
    u64 hi, lo;
    u64 vl, vh;
    u8 h[16];
    u32 olen = 0;

    rtlc_memset(h, 0, 16);
    if ((ret = mbedtls_cipher_update(&ctx->cipher_ctx, h, 16, h, &olen)) != 0)
        return (ret);

    /* pack h as two 64-bits s32s, big-endian */
    GET_UINT32_BE(hi, h,  0);
    GET_UINT32_BE(lo, h,  4);
    vh = (u64) hi << 32 | lo;

    GET_UINT32_BE(hi, h,  8);
    GET_UINT32_BE(lo, h,  12);
    vl = (u64) hi << 32 | lo;

    /* 8 = 1000 corresponds to 1 in GF(2^128) */
    ctx->HL[8] = vl;
    ctx->HH[8] = vh;

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    /* With CLMUL support, we need only h, not the rest of the table */
    if (mbedtls_aesni_has_support(MBEDTLS_AESNI_CLMUL))
        return (0);
#endif

    /* 0 corresponds to 0 in GF(2^128) */
    ctx->HH[0] = 0;
    ctx->HL[0] = 0;

    for (i = 4; i > 0; i >>= 1) {
        u32 T = (vl & 1) * 0xe1000000U;
        vl  = (vh << 63) | (vl >> 1);
        vh  = (vh >> 1) ^ ((u64) T << 32);

        ctx->HL[i] = vl;
        ctx->HH[i] = vh;
    }

    for (i = 2; i <= 8; i *= 2) {
        u64 *HiL = ctx->HL + i, *HiH = ctx->HH + i;
        vh = *HiH;
        vl = *HiL;
        for (j = 1; j < i; j++) {
            HiH[j] = vh ^ ctx->HH[j];
            HiL[j] = vl ^ ctx->HL[j];
        }
    }

    return (0);
}

s32 mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const u8 *key,
                       u32 keybits)
{
    s32 ret;
    const mbedtls_cipher_info_t *cipher_info;

    //__dbg_printf("start\r\n");
    cipher_info = mbedtls_cipher_info_from_values(cipher, keybits, MBEDTLS_MODE_ECB);
    //cipher_info = mbedtls_cipher_info_from_values( cipher, keybits, MBEDTLS_MODE_GCM );
    if (cipher_info == NULL) {
        __dbg_printf("mbedtls_cipher_info_from_values error\r\n");

        return (MBEDTLS_ERR_GCM_BAD_INPUT);
    }

    //prs32f("from_values ok\r\n");

    if (cipher_info->block_size != 16)
        return (MBEDTLS_ERR_GCM_BAD_INPUT);

    mbedtls_cipher_free(&ctx->cipher_ctx);
    //__dbg_printf("free\r\n");

    if ((ret = mbedtls_cipher_setup(&ctx->cipher_ctx, cipher_info)) != 0)
        return (ret);
    //__dbg_printf("setupok\r\n");

    if ((ret = mbedtls_cipher_setkey(&ctx->cipher_ctx, key, keybits,
                                     MBEDTLS_ENCRYPT)) != 0) {
        return (ret);
    }
    //__dbg_printf("setkey ok\r\n");

    if ((ret = gcm_gen_table(ctx)) != 0)
        return (ret);
    //__dbg_printf("gen table ok\r\n");

    return (0);
}

/*
 * Shoup's method for multiplication use this table with
 *      last4[x] = x times P^128
 * where x and last4[x] are seen as elements of GF(2^128) as in [MGV]
 */
const u64 last4[16] = {
    0x0000, 0x1c20, 0x3840, 0x2460,
    0x7080, 0x6ca0, 0x48c0, 0x54e0,
    0xe100, 0xfd20, 0xd940, 0xc560,
    0x9180, 0x8da0, 0xa9c0, 0xb5e0
};

/*
 * Sets output to x times H using the precomputed tables.
 * x and output are seen as elements of GF(2^128) as in [MGV].
 */
void gcm_mult(mbedtls_gcm_context *ctx, const u8 x[16],
              u8 output[16])
{
    s32 i = 0;
    u8 lo, hi, rem;
    u64 zh, zl;

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    if (mbedtls_aesni_has_support(MBEDTLS_AESNI_CLMUL)) {
        u8 h[16];

        PUT_UINT32_BE(ctx->HH[8] >> 32, h,  0);
        PUT_UINT32_BE(ctx->HH[8],       h,  4);
        PUT_UINT32_BE(ctx->HL[8] >> 32, h,  8);
        PUT_UINT32_BE(ctx->HL[8],       h, 12);

        mbedtls_aesni_gcm_mult(output, x, h);
        return;
    }
#endif /* MBEDTLS_AESNI_C && MBEDTLS_HAVE_X86_64 */

    lo = x[15] & 0xf;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    for (i = 15; i >= 0; i--) {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        if (i != 15) {
            rem = (u8) zl & 0xf;
            zl = (zh << 60) | (zl >> 4);
            zh = (zh >> 4);
            zh ^= (u64) last4[rem] << 48;
            zh ^= ctx->HH[lo];
            zl ^= ctx->HL[lo];

        }

        rem = (u8) zl & 0xf;
        zl = (zh << 60) | (zl >> 4);
        zh = (zh >> 4);
        zh ^= (u64) last4[rem] << 48;
        zh ^= ctx->HH[hi];
        zl ^= ctx->HL[hi];
    }

    PUT_UINT32_BE(zh >> 32, output, 0);
    PUT_UINT32_BE(zh, output, 4);
    PUT_UINT32_BE(zl >> 32, output, 8);
    PUT_UINT32_BE(zl, output, 12);
}

s32 mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
                       s32 mode,
                       const u8 *iv,
                       u32 iv_len,
                       const u8 *add,
                       u32 add_len)
{
    s32 ret;
    u8 work_buf[16];
    u32 i;
    const u8 *p;
    u32 use_len, olen = 0;

    /* IV and AD are limited to 2^64 bits, so 2^61 bytes */
    /* IV is not allowed to be zero length */
    if (iv_len == 0 ||
        ((u64) iv_len) >> 61 != 0 ||
        ((u64) add_len) >> 61 != 0) {
        __dbg_printf("%s : MBEDTLS_ERR_GCM_BAD_INPUT \r\n", __FUNCTION__);
        return (MBEDTLS_ERR_GCM_BAD_INPUT);
    }

    rtlc_memset(ctx->y, 0x00, sizeof(ctx->y));
    rtlc_memset(ctx->buf, 0x00, sizeof(ctx->buf));

    ctx->mode = mode;
    ctx->len = 0;
    ctx->add_len = 0;

    if (iv_len == 12) {
        rtlc_memcpy(ctx->y, iv, iv_len);
        ctx->y[15] = 1;
    } else {
        rtlc_memset(work_buf, 0x00, 16);
        PUT_UINT32_BE(iv_len * 8, work_buf, 12);

        p = iv;
        while (iv_len > 0) {
            use_len = (iv_len < 16) ? iv_len : 16;

            for (i = 0; i < use_len; i++)
                ctx->y[i] ^= p[i];

            gcm_mult(ctx, ctx->y, ctx->y);

            iv_len -= use_len;
            p += use_len;
        }

        for (i = 0; i < 16; i++)
            ctx->y[i] ^= work_buf[i];

        gcm_mult(ctx, ctx->y, ctx->y);
    }

    if ((ret = mbedtls_cipher_update(&ctx->cipher_ctx, ctx->y, 16, ctx->base_ectr,
                                     &olen)) != 0) {
        __dbg_printf("%s : mbedtls_cipher_update, ret=0x%x \r\n", __FUNCTION__, ret);
        return (ret);
    }

    ctx->add_len = add_len;
    p = add;
    while (add_len > 0) {
        use_len = (add_len < 16) ? add_len : 16;

        for (i = 0; i < use_len; i++)
            ctx->buf[i] ^= p[i];

        gcm_mult(ctx, ctx->buf, ctx->buf);

        add_len -= use_len;
        p += use_len;
    }
    return (0);
}

s32 mbedtls_gcm_update(mbedtls_gcm_context *ctx,
                       u32 length,
                       const u8 *input,
                       u8 *output)
{
    s32 ret;
    u8 ectr[16];
    u32 i;
    const u8 *p;
    u8 *out_p = output;
    u32 use_len, olen = 0;

    if (output > input && (u32)(output - input) < length)
        return (MBEDTLS_ERR_GCM_BAD_INPUT);

    /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
     * Also check for possible overflow */
    if (ctx->len + length < ctx->len ||
        (u64) ctx->len + length > 0xFFFFFFFE0ull) {
        return (MBEDTLS_ERR_GCM_BAD_INPUT);
    }

    ctx->len += length;

    p = input;
    while (length > 0) {
        use_len = (length < 16) ? length : 16;

        for (i = 16; i > 12; i--)
            if (++ctx->y[i - 1] != 0)
                break;

        if ((ret = mbedtls_cipher_update(&ctx->cipher_ctx, ctx->y, 16, ectr,
                                         &olen)) != 0) {
            return (ret);
        }

        for (i = 0; i < use_len; i++) {
            if (ctx->mode == MBEDTLS_GCM_DECRYPT)
                ctx->buf[i] ^= p[i];
            out_p[i] = ectr[i] ^ p[i];
            if (ctx->mode == MBEDTLS_GCM_ENCRYPT)
                ctx->buf[i] ^= out_p[i];
        }

        gcm_mult(ctx, ctx->buf, ctx->buf);

        length -= use_len;
        p += use_len;
        out_p += use_len;
    }

    return (0);
}

s32 mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
                       u8 *tag,
                       u32 tag_len)
{
    u8 work_buf[16];
    u32 i;
    u64 orig_len = ctx->len * 8;
    u64 orig_add_len = ctx->add_len * 8;


    if (tag_len > 16 || tag_len < 4)
        return (MBEDTLS_ERR_GCM_BAD_INPUT);

    /*TODO!!!*/
    if (tag_len != 0) {
        //__dbg_printf(" tag = %p ctx->base_ectr = %p\r\n",tag,(ctx->base_ectr));
        rtlc_memcpy(tag, ctx->base_ectr, tag_len);   //this line memory dymp!
    }

    if (orig_len || orig_add_len) {
        rtlc_memset(work_buf, 0x00, 16);

        PUT_UINT32_BE((orig_add_len >> 32), work_buf, 0);
        PUT_UINT32_BE((orig_add_len), work_buf, 4);
        PUT_UINT32_BE((orig_len     >> 32), work_buf, 8);
        PUT_UINT32_BE((orig_len), work_buf, 12);

        for (i = 0; i < 16; i++)
            ctx->buf[i] ^= work_buf[i];

        gcm_mult(ctx, ctx->buf, ctx->buf);

        for (i = 0; i < tag_len; i++)
            tag[i] ^= ctx->buf[i];
    }

    return (0);
}

s32 mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                              s32 mode,
                              u32 length,
                              const u8 *iv,
                              u32 iv_len,
                              const u8 *add,
                              u32 add_len,
                              const u8 *input,
                              u8 *output,
                              u32 tag_len,
                              u8 *tag)
{
    s32 ret;

    if ((ret = mbedtls_gcm_starts(ctx, mode, iv, iv_len, add, add_len)) != 0) {
        __dbg_printf("mbedtls_gcm_starts : ret = 0x%x \r\n", ret);
        return (ret);
    }

    if ((ret = mbedtls_gcm_update(ctx, length, input, output)) != 0) {

        __dbg_printf("mbedtls_gcm_update : ret = 0x%x \r\n", ret);
        return (ret);
    }

    if ((ret = mbedtls_gcm_finish(ctx, tag, tag_len)) != 0) {
        __dbg_printf("mbedtls_gcm_finish : ret = 0x%x \r\n", ret);

        return (ret);
    }

    return (0);
}

#if 0
s32 mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
                             u32 length,
                             const u8 *iv,
                             u32 iv_len,
                             const u8 *add,
                             u32 add_len,
                             const u8 *tag,
                             u32 tag_len,
                             const u8 *input,
                             u8 *output)
{
    s32 ret;
    u8 check_tag[16];
    u32 i;
    s32 diff;

    if ((ret = mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_DECRYPT, length,
                                         iv, iv_len, add, add_len,
                                         input, output, tag_len, check_tag)) != 0) {
        return (ret);
    }

    /* Check tag in "constant-time" */
    for (diff = 0, i = 0; i < tag_len; i++)
        diff |= tag[i] ^ check_tag[i];

    if (diff != 0) {
        mbedtls_zeroize(output, length);
        return (MBEDTLS_ERR_GCM_AUTH_FAILED);
    }

    return (0);
}
#endif

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
    //__dbg_printf("%s\r\n",__FUNCTION__);
    mbedtls_cipher_free(&ctx->cipher_ctx);
    mbedtls_zeroize(ctx, sizeof(mbedtls_gcm_context));
}
#if 0
#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
/*
 * AES-GCM test vectors from:
 *
 * http://csrc.nist.gov/groups/STM/cavp/documents/mac/gcmtestvectors.zip
 */
#define MAX_TESTS   6

static const s32 key_index[MAX_TESTS] =
{ 0, 0, 1, 1, 1, 1 };

static const u8 key[MAX_TESTS][32] = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
    },
};

static const u32 iv_len[MAX_TESTS] =
{ 12, 12, 12, 12, 8, 60 };

static const s32 iv_index[MAX_TESTS] =
{ 0, 0, 1, 1, 1, 2 };

static const u8 iv[MAX_TESTS][64] = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },
    {
        0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
        0xde, 0xca, 0xf8, 0x88
    },
    {
        0x93, 0x13, 0x22, 0x5d, 0xf8, 0x84, 0x06, 0xe5,
        0x55, 0x90, 0x9c, 0x5a, 0xff, 0x52, 0x69, 0xaa,
        0x6a, 0x7a, 0x95, 0x38, 0x53, 0x4f, 0x7d, 0xa1,
        0xe4, 0xc3, 0x03, 0xd2, 0xa3, 0x18, 0xa7, 0x28,
        0xc3, 0xc0, 0xc9, 0x51, 0x56, 0x80, 0x95, 0x39,
        0xfc, 0xf0, 0xe2, 0x42, 0x9a, 0x6b, 0x52, 0x54,
        0x16, 0xae, 0xdb, 0xf5, 0xa0, 0xde, 0x6a, 0x57,
        0xa6, 0x37, 0xb3, 0x9b
    },
};

static const u32 add_len[MAX_TESTS] =
{ 0, 0, 0, 20, 20, 20 };

static const s32 add_index[MAX_TESTS] =
{ 0, 0, 0, 1, 1, 1 };

static const u8 additional[MAX_TESTS][64] = {
    { 0x00 },
    {
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xab, 0xad, 0xda, 0xd2
    },
};

static const u32 pt_len[MAX_TESTS] =
{ 0, 16, 64, 60, 60, 60 };

static const s32 pt_index[MAX_TESTS] =
{ 0, 0, 1, 1, 1, 1 };

static const u8 pt[MAX_TESTS][64] = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
        0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
        0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
        0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
        0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
        0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
        0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
        0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55
    },
};

static const u8 ct[MAX_TESTS * 3][64] = {
    { 0x00 },
    {
        0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92,
        0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78
    },
    {
        0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
        0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
        0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
        0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
        0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
        0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
        0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
        0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85
    },
    {
        0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
        0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
        0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
        0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
        0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
        0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
        0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
        0x3d, 0x58, 0xe0, 0x91
    },
    {
        0x61, 0x35, 0x3b, 0x4c, 0x28, 0x06, 0x93, 0x4a,
        0x77, 0x7f, 0xf5, 0x1f, 0xa2, 0x2a, 0x47, 0x55,
        0x69, 0x9b, 0x2a, 0x71, 0x4f, 0xcd, 0xc6, 0xf8,
        0x37, 0x66, 0xe5, 0xf9, 0x7b, 0x6c, 0x74, 0x23,
        0x73, 0x80, 0x69, 0x00, 0xe4, 0x9f, 0x24, 0xb2,
        0x2b, 0x09, 0x75, 0x44, 0xd4, 0x89, 0x6b, 0x42,
        0x49, 0x89, 0xb5, 0xe1, 0xeb, 0xac, 0x0f, 0x07,
        0xc2, 0x3f, 0x45, 0x98
    },
    {
        0x8c, 0xe2, 0x49, 0x98, 0x62, 0x56, 0x15, 0xb6,
        0x03, 0xa0, 0x33, 0xac, 0xa1, 0x3f, 0xb8, 0x94,
        0xbe, 0x91, 0x12, 0xa5, 0xc3, 0xa2, 0x11, 0xa8,
        0xba, 0x26, 0x2a, 0x3c, 0xca, 0x7e, 0x2c, 0xa7,
        0x01, 0xe4, 0xa9, 0xa4, 0xfb, 0xa4, 0x3c, 0x90,
        0xcc, 0xdc, 0xb2, 0x81, 0xd4, 0x8c, 0x7c, 0x6f,
        0xd6, 0x28, 0x75, 0xd2, 0xac, 0xa4, 0x17, 0x03,
        0x4c, 0x34, 0xae, 0xe5
    },
    { 0x00 },
    {
        0x98, 0xe7, 0x24, 0x7c, 0x07, 0xf0, 0xfe, 0x41,
        0x1c, 0x26, 0x7e, 0x43, 0x84, 0xb0, 0xf6, 0x00
    },
    {
        0x39, 0x80, 0xca, 0x0b, 0x3c, 0x00, 0xe8, 0x41,
        0xeb, 0x06, 0xfa, 0xc4, 0x87, 0x2a, 0x27, 0x57,
        0x85, 0x9e, 0x1c, 0xea, 0xa6, 0xef, 0xd9, 0x84,
        0x62, 0x85, 0x93, 0xb4, 0x0c, 0xa1, 0xe1, 0x9c,
        0x7d, 0x77, 0x3d, 0x00, 0xc1, 0x44, 0xc5, 0x25,
        0xac, 0x61, 0x9d, 0x18, 0xc8, 0x4a, 0x3f, 0x47,
        0x18, 0xe2, 0x44, 0x8b, 0x2f, 0xe3, 0x24, 0xd9,
        0xcc, 0xda, 0x27, 0x10, 0xac, 0xad, 0xe2, 0x56
    },
    {
        0x39, 0x80, 0xca, 0x0b, 0x3c, 0x00, 0xe8, 0x41,
        0xeb, 0x06, 0xfa, 0xc4, 0x87, 0x2a, 0x27, 0x57,
        0x85, 0x9e, 0x1c, 0xea, 0xa6, 0xef, 0xd9, 0x84,
        0x62, 0x85, 0x93, 0xb4, 0x0c, 0xa1, 0xe1, 0x9c,
        0x7d, 0x77, 0x3d, 0x00, 0xc1, 0x44, 0xc5, 0x25,
        0xac, 0x61, 0x9d, 0x18, 0xc8, 0x4a, 0x3f, 0x47,
        0x18, 0xe2, 0x44, 0x8b, 0x2f, 0xe3, 0x24, 0xd9,
        0xcc, 0xda, 0x27, 0x10
    },
    {
        0x0f, 0x10, 0xf5, 0x99, 0xae, 0x14, 0xa1, 0x54,
        0xed, 0x24, 0xb3, 0x6e, 0x25, 0x32, 0x4d, 0xb8,
        0xc5, 0x66, 0x63, 0x2e, 0xf2, 0xbb, 0xb3, 0x4f,
        0x83, 0x47, 0x28, 0x0f, 0xc4, 0x50, 0x70, 0x57,
        0xfd, 0xdc, 0x29, 0xdf, 0x9a, 0x47, 0x1f, 0x75,
        0xc6, 0x65, 0x41, 0xd4, 0xd4, 0xda, 0xd1, 0xc9,
        0xe9, 0x3a, 0x19, 0xa5, 0x8e, 0x8b, 0x47, 0x3f,
        0xa0, 0xf0, 0x62, 0xf7
    },
    {
        0xd2, 0x7e, 0x88, 0x68, 0x1c, 0xe3, 0x24, 0x3c,
        0x48, 0x30, 0x16, 0x5a, 0x8f, 0xdc, 0xf9, 0xff,
        0x1d, 0xe9, 0xa1, 0xd8, 0xe6, 0xb4, 0x47, 0xef,
        0x6e, 0xf7, 0xb7, 0x98, 0x28, 0x66, 0x6e, 0x45,
        0x81, 0xe7, 0x90, 0x12, 0xaf, 0x34, 0xdd, 0xd9,
        0xe2, 0xf0, 0x37, 0x58, 0x9b, 0x29, 0x2d, 0xb3,
        0xe6, 0x7c, 0x03, 0x67, 0x45, 0xfa, 0x22, 0xe7,
        0xe9, 0xb7, 0x37, 0x3b
    },
    { 0x00 },
    {
        0xce, 0xa7, 0x40, 0x3d, 0x4d, 0x60, 0x6b, 0x6e,
        0x07, 0x4e, 0xc5, 0xd3, 0xba, 0xf3, 0x9d, 0x18
    },
    {
        0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07,
        0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,
        0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9,
        0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa,
        0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d,
        0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38,
        0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a,
        0xbc, 0xc9, 0xf6, 0x62, 0x89, 0x80, 0x15, 0xad
    },
    {
        0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07,
        0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,
        0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9,
        0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa,
        0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d,
        0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38,
        0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a,
        0xbc, 0xc9, 0xf6, 0x62
    },
    {
        0xc3, 0x76, 0x2d, 0xf1, 0xca, 0x78, 0x7d, 0x32,
        0xae, 0x47, 0xc1, 0x3b, 0xf1, 0x98, 0x44, 0xcb,
        0xaf, 0x1a, 0xe1, 0x4d, 0x0b, 0x97, 0x6a, 0xfa,
        0xc5, 0x2f, 0xf7, 0xd7, 0x9b, 0xba, 0x9d, 0xe0,
        0xfe, 0xb5, 0x82, 0xd3, 0x39, 0x34, 0xa4, 0xf0,
        0x95, 0x4c, 0xc2, 0x36, 0x3b, 0xc7, 0x3f, 0x78,
        0x62, 0xac, 0x43, 0x0e, 0x64, 0xab, 0xe4, 0x99,
        0xf4, 0x7c, 0x9b, 0x1f
    },
    {
        0x5a, 0x8d, 0xef, 0x2f, 0x0c, 0x9e, 0x53, 0xf1,
        0xf7, 0x5d, 0x78, 0x53, 0x65, 0x9e, 0x2a, 0x20,
        0xee, 0xb2, 0xb2, 0x2a, 0xaf, 0xde, 0x64, 0x19,
        0xa0, 0x58, 0xab, 0x4f, 0x6f, 0x74, 0x6b, 0xf4,
        0x0f, 0xc0, 0xc3, 0xb7, 0x80, 0xf2, 0x44, 0x45,
        0x2d, 0xa3, 0xeb, 0xf1, 0xc5, 0xd8, 0x2c, 0xde,
        0xa2, 0x41, 0x89, 0x97, 0x20, 0x0e, 0xf8, 0x2e,
        0x44, 0xae, 0x7e, 0x3f
    },
};

static const u8 tag[MAX_TESTS * 3][16] = {
    {
        0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61,
        0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a
    },
    {
        0xab, 0x6e, 0x47, 0xd4, 0x2c, 0xec, 0x13, 0xbd,
        0xf5, 0x3a, 0x67, 0xb2, 0x12, 0x57, 0xbd, 0xdf
    },
    {
        0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6,
        0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4
    },
    {
        0x5b, 0xc9, 0x4f, 0xbc, 0x32, 0x21, 0xa5, 0xdb,
        0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47
    },
    {
        0x36, 0x12, 0xd2, 0xe7, 0x9e, 0x3b, 0x07, 0x85,
        0x56, 0x1b, 0xe1, 0x4a, 0xac, 0xa2, 0xfc, 0xcb
    },
    {
        0x61, 0x9c, 0xc5, 0xae, 0xff, 0xfe, 0x0b, 0xfa,
        0x46, 0x2a, 0xf4, 0x3c, 0x16, 0x99, 0xd0, 0x50
    },
    {
        0xcd, 0x33, 0xb2, 0x8a, 0xc7, 0x73, 0xf7, 0x4b,
        0xa0, 0x0e, 0xd1, 0xf3, 0x12, 0x57, 0x24, 0x35
    },
    {
        0x2f, 0xf5, 0x8d, 0x80, 0x03, 0x39, 0x27, 0xab,
        0x8e, 0xf4, 0xd4, 0x58, 0x75, 0x14, 0xf0, 0xfb
    },
    {
        0x99, 0x24, 0xa7, 0xc8, 0x58, 0x73, 0x36, 0xbf,
        0xb1, 0x18, 0x02, 0x4d, 0xb8, 0x67, 0x4a, 0x14
    },
    {
        0x25, 0x19, 0x49, 0x8e, 0x80, 0xf1, 0x47, 0x8f,
        0x37, 0xba, 0x55, 0xbd, 0x6d, 0x27, 0x61, 0x8c
    },
    {
        0x65, 0xdc, 0xc5, 0x7f, 0xcf, 0x62, 0x3a, 0x24,
        0x09, 0x4f, 0xcc, 0xa4, 0x0d, 0x35, 0x33, 0xf8
    },
    {
        0xdc, 0xf5, 0x66, 0xff, 0x29, 0x1c, 0x25, 0xbb,
        0xb8, 0x56, 0x8f, 0xc3, 0xd3, 0x76, 0xa6, 0xd9
    },
    {
        0x53, 0x0f, 0x8a, 0xfb, 0xc7, 0x45, 0x36, 0xb9,
        0xa9, 0x63, 0xb4, 0xf1, 0xc4, 0xcb, 0x73, 0x8b
    },
    {
        0xd0, 0xd1, 0xc8, 0xa7, 0x99, 0x99, 0x6b, 0xf0,
        0x26, 0x5b, 0x98, 0xb5, 0xd4, 0x8a, 0xb9, 0x19
    },
    {
        0xb0, 0x94, 0xda, 0xc5, 0xd9, 0x34, 0x71, 0xbd,
        0xec, 0x1a, 0x50, 0x22, 0x70, 0xe3, 0xcc, 0x6c
    },
    {
        0x76, 0xfc, 0x6e, 0xce, 0x0f, 0x4e, 0x17, 0x68,
        0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b
    },
    {
        0x3a, 0x33, 0x7d, 0xbf, 0x46, 0xa7, 0x92, 0xc4,
        0x5e, 0x45, 0x49, 0x13, 0xfe, 0x2e, 0xa8, 0xf2
    },
    {
        0xa4, 0x4a, 0x82, 0x66, 0xee, 0x1c, 0x8e, 0xb0,
        0xc8, 0xb5, 0xd4, 0xcf, 0x5a, 0xe9, 0xf1, 0x9a
    },
};
#endif
#if 0
s32 mbedtls_gcm_self_test(s32 verbose)
{
    mbedtls_gcm_context ctx;
    u8 buf[64];
    u8 tag_buf[16];
    s32 i, j, ret;
    mbedtls_cipher_id_t cipher = MBEDTLS_CIPHER_ID_AES;

    mbedtls_gcm_init(&ctx);

    for (j = 0; j < 3; j++) {
        s32 key_len = 128 + 64 * j;

        for (i = 0; i < MAX_TESTS; i++) {
            if (verbose != 0)
                mbedtls_prs32f("  AES-GCM-%3d #%d (%s): ",
                               key_len, i, "enc");

            mbedtls_gcm_setkey(&ctx, cipher, key[key_index[i]], key_len);

            ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT,
                                            pt_len[i],
                                            iv[iv_index[i]], iv_len[i],
                                            additional[add_index[i]], add_len[i],
                                            pt[pt_index[i]], buf, 16, tag_buf);

            if (ret != 0 ||
                crypto_self_memcmp(buf, ct[j * 6 + i], pt_len[i]) != 0 ||
                crypto_self_memcmp(tag_buf, tag[j * 6 + i], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                return (1);
            }

            mbedtls_gcm_free(&ctx);

            if (verbose != 0)
                mbedtls_prs32f("passed\n");

            if (verbose != 0)
                mbedtls_prs32f("  AES-GCM-%3d #%d (%s): ",
                               key_len, i, "dec");

            mbedtls_gcm_setkey(&ctx, cipher, key[key_index[i]], key_len);

            ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_DECRYPT,
                                            pt_len[i],
                                            iv[iv_index[i]], iv_len[i],
                                            additional[add_index[i]], add_len[i],
                                            ct[j * 6 + i], buf, 16, tag_buf);

            if (ret != 0 ||
                crypto_self_memcmp(buf, pt[pt_index[i]], pt_len[i]) != 0 ||
                crypto_self_memcmp(tag_buf, tag[j * 6 + i], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                return (1);
            }

            mbedtls_gcm_free(&ctx);

            if (verbose != 0)
                mbedtls_prs32f("passed\n");

            if (verbose != 0)
                mbedtls_prs32f("  AES-GCM-%3d #%d split (%s): ",
                               key_len, i, "enc");

            mbedtls_gcm_setkey(&ctx, cipher, key[key_index[i]], key_len);

            ret = mbedtls_gcm_starts(&ctx, MBEDTLS_GCM_ENCRYPT,
                                     iv[iv_index[i]], iv_len[i],
                                     additional[add_index[i]], add_len[i]);
            if (ret != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                return (1);
            }

            if (pt_len[i] > 32) {
                u32 rest_len = pt_len[i] - 32;
                ret = mbedtls_gcm_update(&ctx, 32, pt[pt_index[i]], buf);
                if (ret != 0) {
                    if (verbose != 0)
                        mbedtls_prs32f("failed\n");

                    return (1);
                }

                ret = mbedtls_gcm_update(&ctx, rest_len, pt[pt_index[i]] + 32,
                                         buf + 32);
                if (ret != 0) {
                    if (verbose != 0)
                        mbedtls_prs32f("failed\n");

                    return (1);
                }
            } else {
                ret = mbedtls_gcm_update(&ctx, pt_len[i], pt[pt_index[i]], buf);
                if (ret != 0) {
                    if (verbose != 0)
                        mbedtls_prs32f("failed\n");

                    return (1);
                }
            }

            ret = mbedtls_gcm_finish(&ctx, tag_buf, 16);
            if (ret != 0 ||
                crypto_self_memcmp(buf, ct[j * 6 + i], pt_len[i]) != 0 ||
                crypto_self_memcmp(tag_buf, tag[j * 6 + i], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                return (1);
            }

            mbedtls_gcm_free(&ctx);

            if (verbose != 0)
                mbedtls_prs32f("passed\n");

            if (verbose != 0)
                mbedtls_prs32f("  AES-GCM-%3d #%d split (%s): ",
                               key_len, i, "dec");

            mbedtls_gcm_setkey(&ctx, cipher, key[key_index[i]], key_len);

            ret = mbedtls_gcm_starts(&ctx, MBEDTLS_GCM_DECRYPT,
                                     iv[iv_index[i]], iv_len[i],
                                     additional[add_index[i]], add_len[i]);
            if (ret != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                return (1);
            }

            if (pt_len[i] > 32) {
                u32 rest_len = pt_len[i] - 32;
                ret = mbedtls_gcm_update(&ctx, 32, ct[j * 6 + i], buf);
                if (ret != 0) {
                    if (verbose != 0)
                        mbedtls_prs32f("failed\n");

                    return (1);
                }

                ret = mbedtls_gcm_update(&ctx, rest_len, ct[j * 6 + i] + 32,
                                         buf + 32);
                if (ret != 0) {
                    if (verbose != 0)
                        mbedtls_prs32f("failed\n");

                    return (1);
                }
            } else {
                ret = mbedtls_gcm_update(&ctx, pt_len[i], ct[j * 6 + i], buf);
                if (ret != 0) {
                    if (verbose != 0)
                        mbedtls_prs32f("failed\n");

                    return (1);
                }
            }

            ret = mbedtls_gcm_finish(&ctx, tag_buf, 16);
            if (ret != 0 ||
                crypto_self_memcmp(buf, pt[pt_index[i]], pt_len[i]) != 0 ||
                crypto_self_memcmp(tag_buf, tag[j * 6 + i], 16) != 0) {
                if (verbose != 0)
                    mbedtls_prs32f("failed\n");

                return (1);
            }

            mbedtls_gcm_free(&ctx);

            if (verbose != 0)
                mbedtls_prs32f("passed\n");

        }
    }

    if (verbose != 0)
        mbedtls_prs32f("\n");

    return (0);
}
#endif
#endif /* MBEDTLS_SELF_TEST && MBEDTLS_AES_C */


/* ------------------------------ Overall ------------------------------ */
#if 0
s32 mbedtls_aes_swvrf(const u32 mode, const u8 *input, const s32 len,
                      const u8 *key, const s32 lenKey, const u8 *iv,
                      const u8 *aad, const s32 lenAAD, u8 *output, u8 *tag)
{
    //char _iv[16]; /* Since soft_AES_cbc mode will modify iv,we need to allocate one for it */
    char nonce_counter[16];
    char stream_block[16];
    s32 done;
    s32 ret = SUCCESS;
    u32 offset = 0;
    mbedtls_gcm_context ctx_gcm;    /* GCM need*/
    mbedtls_cipher_id_t cipher;      /* GCM need*/

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode) || (SWDECRYPT_GCTR_AES == mode) ||
        (SWDECRYPT_GCM_AES == mode)) {
//#if SWVRF_CRITICAL
//            sw_critical_enter();
//#endif
        if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode)) {
            /* GCM Init*/
            mbedtls_gcm_init(&ctx_gcm);
            cipher = MBEDTLS_CIPHER_ID_AES;
        }
    }
    switch (mode) {

    /*
     * DECRYPTION
     * */
    case SWDECRYPT_ECB_AES:
        ret = mbedtls_aes_setkey_dec(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        for (done = 0; done < len; done += 16) {
            // Handle 1byte(8bits), total=16bytes(128bits)
            ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, &input[done], &output[done]);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
        }

        break;
    case SWDECRYPT_CBC_AES:
        ret = mbedtls_aes_setkey_dec(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, len, (u8 *)iv, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWDECRYPT_CFB_AES:
        offset = 0;
        // set key of drcryption in CFB mode is same as encryption
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_DECRYPT, len, &offset, (u8 *)iv, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWDECRYPT_OFB_AES:
        offset = 0;
        // set key of drcryption in OFB mode is same as encryption
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_ofb128(&ctx, len, &offset, (u8 *)iv, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWDECRYPT_CTR_AES:
        rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));
        rtlc_memset(stream_block, 0, sizeof(stream_block));
        offset = 0;
        // set key of drcryption in CTR mode is same as encryption
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_ctr(&ctx, len, &offset, (u8 *)nonce_counter, (u8 *)stream_block, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;
#if 1
    case SWDECRYPT_GCTR_AES:
        rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));
        ret = aes_gctr_swvrf(key, lenKey, (u8 *)nonce_counter, input, len, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWDECRYPT_GCM_AES:
        // need cipher_type(can choose other crypto_algorithms)
        ret = mbedtls_gcm_setkey(&ctx_gcm, cipher, key, lenKey * 8);
        if (SUCCESS != ret) {
            __dbg_printf("gcm_setkey decrypt fail\r\n");
            goto mbedtls_aes_end;
        } else {
            // ivlen = 12 bytes(96bits), taglen = 16 bytes(128bits)
            ret = mbedtls_gcm_crypt_and_tag(&ctx_gcm, MBEDTLS_GCM_DECRYPT, len,
                                            (u8 *)iv, 12,
                                            aad, lenAAD,
                                            input, output, 16, tag);
            if (SUCCESS != ret) {
                __dbg_printf("gcm_crypt_and_tag decrypt fail\r\n");
                goto mbedtls_aes_end;
            }
        }
        break;
#endif
    /*
     * ENCRYPTION
     * */
    case SWENCRYPT_ECB_AES:
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        for (done = 0; done < len; done += 16) {
            // Handle 1byte(8bits), total=16bytes(128bits)
            ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, &input[done], &output[done]);
            if (SUCCESS != ret) {
                goto mbedtls_aes_end;
            }
        }
        break;

    case SWENCRYPT_CBC_AES:
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, len, (u8 *)iv, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWENCRYPT_CFB_AES:
        offset = 0;
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_ENCRYPT, len, &offset, (u8 *)iv, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWENCRYPT_OFB_AES:
        offset = 0;
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_ofb128(&ctx, len, &offset, (u8 *)iv, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWENCRYPT_CTR_AES:
        rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));
        rtlc_memset(stream_block, 0, sizeof(stream_block));
        offset = 0;
        // set key of drcryption in CTR mode is same as encryption
        ret = mbedtls_aes_setkey_enc(&ctx, key, lenKey * 8);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        ret = mbedtls_aes_crypt_ctr(&ctx, len, &offset, (u8 *)nonce_counter, (u8 *)stream_block, input, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;
#if 1
    case SWENCRYPT_GCTR_AES:
        rtlc_memcpy(nonce_counter, iv, sizeof(nonce_counter));
        ret = aes_gctr_swvrf(key, lenKey, (u8 *)nonce_counter, input, len, output);
        if (SUCCESS != ret) {
            goto mbedtls_aes_end;
        }
        break;

    case SWENCRYPT_GCM_AES:
        // need cipher_type(can choose other crypto_algorithms)
        //__dbg_printf("lenkey = %d\r\n",lenKey);
        ret = mbedtls_gcm_setkey(&ctx_gcm, cipher, key, lenKey * 8);
        if (SUCCESS != ret) {
            __dbg_printf("gcm_setkey encrypt fail\r\n");
            goto mbedtls_aes_end;
        } else {
            // ivlen = 12 bytes(96bits), taglen = 16 bytes(128bits)
            ret = mbedtls_gcm_crypt_and_tag(&ctx_gcm, MBEDTLS_GCM_ENCRYPT, len,
                                            (u8 *)iv, 12,
                                            aad, lenAAD,
                                            input, output, 16, tag);
            if (SUCCESS != ret) {
                __dbg_printf("gcm_crypt_and_tag encrypt fail\r\n");
                goto mbedtls_aes_end;
            }
        }
        break;
#endif
    }

mbedtls_aes_end:
    mbedtls_aes_free(&ctx);
    if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode)) {
        mbedtls_gcm_free(&ctx_gcm);  //!!important
    }
    if ((SWDECRYPT_GCM_AES == mode) || (SWENCRYPT_GCM_AES == mode) || (SWDECRYPT_GCTR_AES == mode) ||
        (SWDECRYPT_GCM_AES == mode)) {
//#if SWVRF_CRITICAL
//        sw_critical_leave();
//#endif
    }
    return (ret);
}

s32 rtl_crypto_cipher_swvrf(const u32 cipher_type,
                            const u8 *pKey, const s32 keylen, const u8 *pIv, const s32 ivLen, const s32 init_count,
                            const u8 *pAAD, const s32 aadlen, const u8 *pText, const s32 textlen,
                            u8 *pResult, u8 *pTag)
{

    s32 ret = SUCCESS;
    ret =  mbedtls_aes_swvrf(cipher_type, pText, textlen, pKey, keylen, pIv, pAAD, aadlen, pResult, pTag);
    return (ret);
}
#endif


static inline u32 mbedtls_cipher_get_block_size(const mbedtls_cipher_context_t *ctx)
{
    if (NULL == ctx || NULL == ctx->cipher_info)
        return 0;

    return ctx->cipher_info->block_size;
}

//#define mbedtls_free    free

#if defined(MBEDTLS_ARC4_C) || defined(MBEDTLS_CIPHER_NULL_CIPHER)
#define MBEDTLS_CIPHER_MODE_STREAM
#endif

/* Implementation that should never be optimized out by the compiler */
/*
void mbedtls_zeroize( void *v, u32 n ) {
    volatile u8 *p = (u8*)v; while( n-- ) *p++ = 0;
}
*/
#if 0
static s32 supported_init = 0;

const s32 *mbedtls_cipher_list(void)
{
    const mbedtls_cipher_definition_t *def;
    s32 *type;

    if (! supported_init) {
        def = mbedtls_cipher_definitions;
        type = mbedtls_cipher_supported;

        while (def->type != 0)
            *type++ = (*def++).type;

        *type = 0;

        supported_init = 1;
    }

    return (mbedtls_cipher_supported);
}

const mbedtls_cipher_info_t *mbedtls_cipher_info_from_type(const mbedtls_cipher_type_t cipher_type)
{
    const mbedtls_cipher_definition_t *def;

    for (def = mbedtls_cipher_definitions; def->info != NULL; def++)
        if (def->type == cipher_type)
            return (def->info);

    return (NULL);
}

const mbedtls_cipher_info_t *mbedtls_cipher_info_from_string(const char *cipher_name)
{
    const mbedtls_cipher_definition_t *def;

    if (NULL == cipher_name)
        return (NULL);

    for (def = mbedtls_cipher_definitions; def->info != NULL; def++)
        if (!  strcmp(def->info->name, cipher_name))
            return (def->info);

    return (NULL);
}
#endif

//#
const mbedtls_cipher_info_t *mbedtls_cipher_info_from_values(const mbedtls_cipher_id_t cipher_id,
        s32 key_bitlen,
        const mbedtls_cipher_mode_t mode)
{
    const mbedtls_cipher_definition_t *def;

    //__dbg_printf("id = 0x%x,keybit_len = %d, mode = 0x%x\r\n",cipher_id,key_bitlen,mode);
    for (def = mbedtls_cipher_definitions; def->info != NULL; def++) {
        //__dbg_printf("type = 0x%x :cipher_id = 0x%x, keybitlen = %d,mode = 0x%x\r\n",def->type,def->info->base->cipher,def->info->key_bitlen,def->info->mode);
        if (def->info->base->cipher == cipher_id &&
            def->info->key_bitlen == (unsigned) key_bitlen &&
            def->info->mode == mode) {

            //__dbg_printf("find ciphertype = 0x%x\r\n",def->type);
            return (def->info);
        }
    }
    //__dbg_printf("not find\r\n");

    return (NULL);
}

#if 0
void mbedtls_cipher_init(mbedtls_cipher_context_t *ctx)
{
    rtlc_memset(ctx, 0, sizeof(mbedtls_cipher_context_t));
}
#endif

//#
void mbedtls_cipher_free(mbedtls_cipher_context_t *ctx)
{
    if (ctx == NULL)
        return;

#if defined(MBEDTLS_CMAC_C)
    if (ctx->cmac_ctx) {
        mbedtls_zeroize(ctx->cmac_ctx, sizeof(mbedtls_cmac_context_t));
        mbedtls_free(ctx->cmac_ctx);
    }
#endif

    if (ctx->cipher_ctx) {
        ctx->cipher_info->base->ctx_free_func(ctx->cipher_ctx);
    }
    mbedtls_zeroize(ctx, sizeof(mbedtls_cipher_context_t));
}

//#
s32 mbedtls_cipher_setup(mbedtls_cipher_context_t *ctx, const mbedtls_cipher_info_t *cipher_info)
{
    if (NULL == cipher_info || NULL == ctx)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    rtlc_memset(ctx, 0, sizeof(mbedtls_cipher_context_t));
    //__dbg_printf("setup\r\n");
    if (NULL == (ctx->cipher_ctx = cipher_info->base->ctx_alloc_func())) {
        //__dbg_printf("alloc fail\r\n");
        return (MBEDTLS_ERR_CIPHER_ALLOC_FAILED);
    }

    ctx->cipher_info = cipher_info;

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    /*
     * Ignore possible errors caused by a cipher mode that doesn't use padding
     */
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
    (void) mbedtls_cipher_set_padding_mode(ctx, MBEDTLS_PADDING_PKCS7);
#else
    (void) mbedtls_cipher_set_padding_mode(ctx, MBEDTLS_PADDING_NONE);
#endif
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

    return (0);
}

//#
s32 mbedtls_cipher_setkey(mbedtls_cipher_context_t *ctx, const u8 *key,
                          s32 key_bitlen, const mbedtls_operation_t operation)
{
    if (NULL == ctx || NULL == ctx->cipher_info)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    if ((ctx->cipher_info->flags & MBEDTLS_CIPHER_VARIABLE_KEY_LEN) == 0 &&
        (s32) ctx->cipher_info->key_bitlen != key_bitlen) {
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

    ctx->key_bitlen = key_bitlen;
    ctx->operation = operation;

    /*
     * For CFB and CTR mode always use the encryption key schedule
     */
    if (MBEDTLS_ENCRYPT == operation ||
        MBEDTLS_MODE_CFB == ctx->cipher_info->mode ||
        MBEDTLS_MODE_CTR == ctx->cipher_info->mode) {
        return ctx->cipher_info->base->setkey_enc_func(ctx->cipher_ctx, key,
                ctx->key_bitlen);
    }

    if (MBEDTLS_DECRYPT == operation)
        return ctx->cipher_info->base->setkey_dec_func(ctx->cipher_ctx, key,
                ctx->key_bitlen);

    return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
}

#if 0
s32 mbedtls_cipher_set_iv(mbedtls_cipher_context_t *ctx,
                          const u8 *iv, u32 iv_len)
{
    u32 actual_iv_size;

    if (NULL == ctx || NULL == ctx->cipher_info || NULL == iv)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    /* avoid buffer overflow in ctx->iv */
    if (iv_len > MBEDTLS_MAX_IV_LENGTH)
        return (MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);

    if ((ctx->cipher_info->flags & MBEDTLS_CIPHER_VARIABLE_IV_LEN) != 0)
        actual_iv_size = iv_len;
    else {
        actual_iv_size = ctx->cipher_info->iv_size;

        /* avoid reading past the end of input buffer */
        if (actual_iv_size > iv_len)
            return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

    rtlc_memcpy(ctx->iv, iv, actual_iv_size);
    ctx->iv_size = actual_iv_size;

    return (0);
}

s32 mbedtls_cipher_reset(mbedtls_cipher_context_t *ctx)
{
    if (NULL == ctx || NULL == ctx->cipher_info)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    ctx->unprocessed_len = 0;

    return (0);
}

#if defined(MBEDTLS_GCM_C)
s32 mbedtls_cipher_update_ad(mbedtls_cipher_context_t *ctx,
                             const u8 *ad, u32 ad_len)
{
    if (NULL == ctx || NULL == ctx->cipher_info)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    if (MBEDTLS_MODE_GCM == ctx->cipher_info->mode) {
        return mbedtls_gcm_starts((mbedtls_gcm_context *) ctx->cipher_ctx, ctx->operation,
                                  ctx->iv, ctx->iv_size, ad, ad_len);
    }

    return (0);
}
#endif /* MBEDTLS_GCM_C */
#endif

//#
s32 mbedtls_cipher_update(mbedtls_cipher_context_t *ctx, const u8 *input,
                          u32 ilen, u8 *output, u32 *olen)
{
    s32 ret;
    u32 block_size = 0;

    if (NULL == ctx || NULL == ctx->cipher_info || NULL == olen) {
        __dbg_printf(" %s : return -1 \r\n", __FUNCTION__);
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

    *olen = 0;
    block_size = mbedtls_cipher_get_block_size(ctx);

    if (ctx->cipher_info->mode == MBEDTLS_MODE_ECB) {
        if (ilen != block_size)
            return (MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);

        *olen = ilen;

        if (0 != (ret = ctx->cipher_info->base->ecb_func(ctx->cipher_ctx,
                        ctx->operation, input, output))) {
            __dbg_printf("%s: return -2 \r\n", __FUNCTION__);
            return (ret);
        }

        return (0);
    }

#if defined(MBEDTLS_GCM_C)
    if (ctx->cipher_info->mode == MBEDTLS_MODE_GCM) {
        *olen = ilen;
        ret = mbedtls_gcm_update((mbedtls_gcm_context *) ctx->cipher_ctx, ilen, input,
                                 output);
        __dbg_printf("%s : return ret = 0x%x\r\n", __FUNCTION__, ret);
        return ret;
    }
#endif

    if (0 == block_size) {
        return MBEDTLS_ERR_CIPHER_INVALID_CONTEXT;
    }

    if (input == output &&
        (ctx->unprocessed_len != 0 || ilen % block_size)) {
        __dbg_printf("%s : return =4 \r\n", __FUNCTION__);
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    if (ctx->cipher_info->mode == MBEDTLS_MODE_CBC) {
        u32 copy_len = 0;

        /*
         * If there is not enough data for a full block, cache it.
         */
        if ((ctx->operation == MBEDTLS_DECRYPT &&
             ilen <= block_size - ctx->unprocessed_len) ||
            (ctx->operation == MBEDTLS_ENCRYPT &&
             ilen < block_size - ctx->unprocessed_len)) {
            rtlc_memcpy(&(ctx->unprocessed_data[ctx->unprocessed_len]), input,
                     ilen);

            ctx->unprocessed_len += ilen;
            return (0);
        }

        /*
         * Process cached data first
         */
        if (0 != ctx->unprocessed_len) {
            copy_len = block_size - ctx->unprocessed_len;

            rtlc_memcpy(&(ctx->unprocessed_data[ctx->unprocessed_len]), input,
                     copy_len);

            if (0 != (ret = ctx->cipher_info->base->cbc_func(ctx->cipher_ctx,
                            ctx->operation, block_size, ctx->iv,
                            ctx->unprocessed_data, output))) {
                return (ret);
            }

            *olen += block_size;
            output += block_size;
            ctx->unprocessed_len = 0;

            input += copy_len;
            ilen -= copy_len;
        }

        /*
         * Cache final, incomplete block
         */
        if (0 != ilen) {
            if (0 == block_size) {
                return MBEDTLS_ERR_CIPHER_INVALID_CONTEXT;
            }

            copy_len = ilen % block_size;
            if (copy_len == 0 && ctx->operation == MBEDTLS_DECRYPT)
                copy_len = block_size;

            rtlc_memcpy(ctx->unprocessed_data, &(input[ilen - copy_len]),
                     copy_len);

            ctx->unprocessed_len += copy_len;
            ilen -= copy_len;
        }

        /*
         * Process remaining full blocks
         */
        if (ilen) {
            if (0 != (ret = ctx->cipher_info->base->cbc_func(ctx->cipher_ctx,
                            ctx->operation, ilen, ctx->iv, input, output))) {
                return (ret);
            }

            *olen += ilen;
        }

        return (0);
    }
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
    if (ctx->cipher_info->mode == MBEDTLS_MODE_CFB) {
        if (0 != (ret = ctx->cipher_info->base->cfb_func(ctx->cipher_ctx,
                        ctx->operation, ilen, &ctx->unprocessed_len, ctx->iv,
                        input, output))) {
            return (ret);
        }

        *olen = ilen;

        return (0);
    }
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
    if (ctx->cipher_info->mode == MBEDTLS_MODE_CTR) {
        if (0 != (ret = ctx->cipher_info->base->ctr_func(ctx->cipher_ctx,
                        ilen, &ctx->unprocessed_len, ctx->iv,
                        ctx->unprocessed_data, input, output))) {
            return (ret);
        }

        *olen = ilen;

        return (0);
    }
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    if (ctx->cipher_info->mode == MBEDTLS_MODE_STREAM) {
        if (0 != (ret = ctx->cipher_info->base->stream_func(ctx->cipher_ctx,
                        ilen, input, output))) {
            return (ret);
        }

        *olen = ilen;

        return (0);
    }
#endif /* MBEDTLS_CIPHER_MODE_STREAM */

    return (MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);
}

#if 0
#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
/*
 * PKCS7 (and PKCS5) padding: fill with ll bytes, with ll = padding_len
 */
static void add_pkcs_padding(u8 *output, u32 output_len,
                             u32 data_len)
{
    u32 padding_len = output_len - data_len;
    u8 i;

    for (i = 0; i < padding_len; i++)
        output[data_len + i] = (u8) padding_len;
}

static s32 get_pkcs_padding(u8 *input, u32 input_len,
                            u32 *data_len)
{
    u32 i, pad_idx;
    u8 padding_len, bad = 0;

    if (NULL == input || NULL == data_len)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    padding_len = input[input_len - 1];
    *data_len = input_len - padding_len;

    /* Avoid logical || since it results in a branch */
    bad |= padding_len > input_len;
    bad |= padding_len == 0;

    /* The number of bytes checked must be independent of padding_len,
     * so pick input_len, which is usually 8 or 16 (one block) */
    pad_idx = input_len - padding_len;
    for (i = 0; i < input_len; i++)
        bad |= (input[i] ^ padding_len) * (i >= pad_idx);

    return (MBEDTLS_ERR_CIPHER_INVALID_PADDING * (bad != 0));
}
#endif /* MBEDTLS_CIPHER_PADDING_PKCS7 */

#if defined(MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS)
/*
 * One and zeros padding: fill with 80 00 ... 00
 */
static void add_one_and_zeros_padding(u8 *output,
                                      u32 output_len, u32 data_len)
{
    u32 padding_len = output_len - data_len;
    u8 i = 0;

    output[data_len] = 0x80;
    for (i = 1; i < padding_len; i++)
        output[data_len + i] = 0x00;
}

static s32 get_one_and_zeros_padding(u8 *input, u32 input_len,
                                     u32 *data_len)
{
    u32 i;
    u8 done = 0, prev_done, bad;

    if (NULL == input || NULL == data_len)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    bad = 0xFF;
    *data_len = 0;
    for (i = input_len; i > 0; i--) {
        prev_done = done;
        done |= (input[i - 1] != 0);
        *data_len |= (i - 1) * (done != prev_done);
        bad &= (input[i - 1] ^ 0x80) | (done == prev_done);
    }

    return (MBEDTLS_ERR_CIPHER_INVALID_PADDING * (bad != 0));

}
#endif /* MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS */

#if defined(MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN)
/*
 * Zeros and len padding: fill with 00 ... 00 ll, where ll is padding length
 */
static void add_zeros_and_len_padding(u8 *output,
                                      u32 output_len, u32 data_len)
{
    u32 padding_len = output_len - data_len;
    u8 i = 0;

    for (i = 1; i < padding_len; i++)
        output[data_len + i - 1] = 0x00;
    output[output_len - 1] = (u8) padding_len;
}

static s32 get_zeros_and_len_padding(u8 *input, u32 input_len,
                                     u32 *data_len)
{
    u32 i, pad_idx;
    u8 padding_len, bad = 0;

    if (NULL == input || NULL == data_len)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    padding_len = input[input_len - 1];
    *data_len = input_len - padding_len;

    /* Avoid logical || since it results in a branch */
    bad |= padding_len > input_len;
    bad |= padding_len == 0;

    /* The number of bytes checked must be independent of padding_len */
    pad_idx = input_len - padding_len;
    for (i = 0; i < input_len - 1; i++)
        bad |= input[i] * (i >= pad_idx);

    return (MBEDTLS_ERR_CIPHER_INVALID_PADDING * (bad != 0));
}
#endif /* MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN */

#if defined(MBEDTLS_CIPHER_PADDING_ZEROS)
/*
 * Zero padding: fill with 00 ... 00
 */
static void add_zeros_padding(u8 *output,
                              u32 output_len, u32 data_len)
{
    u32 i;

    for (i = data_len; i < output_len; i++)
        output[i] = 0x00;
}

static s32 get_zeros_padding(u8 *input, u32 input_len,
                             u32 *data_len)
{
    u32 i;
    u8 done = 0, prev_done;

    if (NULL == input || NULL == data_len)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    *data_len = 0;
    for (i = input_len; i > 0; i--) {
        prev_done = done;
        done |= (input[i - 1] != 0);
        *data_len |= i * (done != prev_done);
    }

    return (0);
}
#endif /* MBEDTLS_CIPHER_PADDING_ZEROS */

/*
 * No padding: don't pad :)
 *
 * There is no add_padding function (check for NULL in mbedtls_cipher_finish)
 * but a trivial get_padding function
 */
static s32 get_no_padding(u8 *input, u32 input_len,
                          u32 *data_len)
{
    if (NULL == input || NULL == data_len)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    *data_len = input_len;

    return (0);
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

s32 mbedtls_cipher_finish(mbedtls_cipher_context_t *ctx,
                          u8 *output, u32 *olen)
{
    if (NULL == ctx || NULL == ctx->cipher_info || NULL == olen)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    *olen = 0;

    if (MBEDTLS_MODE_CFB == ctx->cipher_info->mode ||
        MBEDTLS_MODE_CTR == ctx->cipher_info->mode ||
        MBEDTLS_MODE_GCM == ctx->cipher_info->mode ||
        MBEDTLS_MODE_STREAM == ctx->cipher_info->mode) {
        return (0);
    }

    if (MBEDTLS_MODE_ECB == ctx->cipher_info->mode) {
        if (ctx->unprocessed_len != 0)
            return (MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);

        return (0);
    }

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    if (MBEDTLS_MODE_CBC == ctx->cipher_info->mode) {
        s32 ret = 0;

        if (MBEDTLS_ENCRYPT == ctx->operation) {
            /* check for 'no padding' mode */
            if (NULL == ctx->add_padding) {
                if (0 != ctx->unprocessed_len)
                    return (MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);

                return (0);
            }

            ctx->add_padding(ctx->unprocessed_data, mbedtls_cipher_get_iv_size(ctx),
                             ctx->unprocessed_len);
        } else if (mbedtls_cipher_get_block_size(ctx) != ctx->unprocessed_len) {
            /*
             * For decrypt operations, expect a full block,
             * or an empty block if no padding
             */
            if (NULL == ctx->add_padding && 0 == ctx->unprocessed_len)
                return (0);

            return (MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED);
        }

        /* cipher block */
        if (0 != (ret = ctx->cipher_info->base->cbc_func(ctx->cipher_ctx,
                        ctx->operation, mbedtls_cipher_get_block_size(ctx), ctx->iv,
                        ctx->unprocessed_data, output))) {
            return (ret);
        }

        /* Set output size for decryption */
        if (MBEDTLS_DECRYPT == ctx->operation)
            return ctx->get_padding(output, mbedtls_cipher_get_block_size(ctx),
                                    olen);

        /* Set output size for encryption */
        *olen = mbedtls_cipher_get_block_size(ctx);
        return (0);
    }
#else
    ((void) output);
#endif /* MBEDTLS_CIPHER_MODE_CBC */

    return (MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);
}

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
s32 mbedtls_cipher_set_padding_mode(mbedtls_cipher_context_t *ctx, mbedtls_cipher_padding_t mode)
{
    if (NULL == ctx ||
        MBEDTLS_MODE_CBC != ctx->cipher_info->mode) {
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

    switch (mode) {
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
    case MBEDTLS_PADDING_PKCS7:
        ctx->add_padding = add_pkcs_padding;
        ctx->get_padding = get_pkcs_padding;
        break;
#endif
#if defined(MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS)
    case MBEDTLS_PADDING_ONE_AND_ZEROS:
        ctx->add_padding = add_one_and_zeros_padding;
        ctx->get_padding = get_one_and_zeros_padding;
        break;
#endif
#if defined(MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN)
    case MBEDTLS_PADDING_ZEROS_AND_LEN:
        ctx->add_padding = add_zeros_and_len_padding;
        ctx->get_padding = get_zeros_and_len_padding;
        break;
#endif
#if defined(MBEDTLS_CIPHER_PADDING_ZEROS)
    case MBEDTLS_PADDING_ZEROS:
        ctx->add_padding = add_zeros_padding;
        ctx->get_padding = get_zeros_padding;
        break;
#endif
    case MBEDTLS_PADDING_NONE:
        ctx->add_padding = NULL;
        ctx->get_padding = get_no_padding;
        break;

    default:
        return (MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);
    }

    return (0);
}
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

#if defined(MBEDTLS_GCM_C)
s32 mbedtls_cipher_write_tag(mbedtls_cipher_context_t *ctx,
                             u8 *tag, u32 tag_len)
{
    if (NULL == ctx || NULL == ctx->cipher_info || NULL == tag)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    if (MBEDTLS_ENCRYPT != ctx->operation)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    if (MBEDTLS_MODE_GCM == ctx->cipher_info->mode)
        return mbedtls_gcm_finish((mbedtls_gcm_context *) ctx->cipher_ctx, tag, tag_len);

    return (0);
}

s32 mbedtls_cipher_check_tag(mbedtls_cipher_context_t *ctx,
                             const u8 *tag, u32 tag_len)
{
    s32 ret;

    if (NULL == ctx || NULL == ctx->cipher_info ||
        MBEDTLS_DECRYPT != ctx->operation) {
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);
    }

    if (MBEDTLS_MODE_GCM == ctx->cipher_info->mode) {
        u8 check_tag[16];
        u32 i;
        s32 diff;

        if (tag_len > sizeof(check_tag))
            return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

        if (0 != (ret = mbedtls_gcm_finish((mbedtls_gcm_context *) ctx->cipher_ctx,
                                           check_tag, tag_len))) {
            return (ret);
        }

        /* Check the tag in "constant-time" */
        for (diff = 0, i = 0; i < tag_len; i++)
            diff |= tag[i] ^ check_tag[i];

        if (diff != 0)
            return (MBEDTLS_ERR_CIPHER_AUTH_FAILED);

        return (0);
    }

    return (0);
}
#endif /* MBEDTLS_GCM_C */

/*
 * Packet-oriented wrapper for non-AEAD modes
 */
s32 mbedtls_cipher_crypt(mbedtls_cipher_context_t *ctx,
                         const u8 *iv, u32 iv_len,
                         const u8 *input, u32 ilen,
                         u8 *output, u32 *olen)
{
    s32 ret;
    u32 finish_olen;

    if ((ret = mbedtls_cipher_set_iv(ctx, iv, iv_len)) != 0)
        return (ret);

    if ((ret = mbedtls_cipher_reset(ctx)) != 0)
        return (ret);

    if ((ret = mbedtls_cipher_update(ctx, input, ilen, output, olen)) != 0)
        return (ret);

    if ((ret = mbedtls_cipher_finish(ctx, output + *olen, &finish_olen)) != 0)
        return (ret);

    *olen += finish_olen;

    return (0);
}

#if defined(MBEDTLS_CIPHER_MODE_AEAD)
/*
 * Packet-oriented encryption for AEAD modes
 */
s32 mbedtls_cipher_auth_encrypt(mbedtls_cipher_context_t *ctx,
                                const u8 *iv, u32 iv_len,
                                const u8 *ad, u32 ad_len,
                                const u8 *input, u32 ilen,
                                u8 *output, u32 *olen,
                                u8 *tag, u32 tag_len)
{
#if defined(MBEDTLS_GCM_C)
    if (MBEDTLS_MODE_GCM == ctx->cipher_info->mode) {
        *olen = ilen;
        return (mbedtls_gcm_crypt_and_tag(ctx->cipher_ctx, MBEDTLS_GCM_ENCRYPT, ilen,
                                          iv, iv_len, ad, ad_len, input, output,
                                          tag_len, tag));
    }
#endif /* MBEDTLS_GCM_C */
#if defined(MBEDTLS_CCM_C)
    if (MBEDTLS_MODE_CCM == ctx->cipher_info->mode) {
        *olen = ilen;
        return (mbedtls_ccm_encrypt_and_tag(ctx->cipher_ctx, ilen,
                                            iv, iv_len, ad, ad_len, input, output,
                                            tag, tag_len));
    }
#endif /* MBEDTLS_CCM_C */

    return (MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);
}

/*
 * Packet-oriented decryption for AEAD modes
 */
s32 mbedtls_cipher_auth_decrypt(mbedtls_cipher_context_t *ctx,
                                const u8 *iv, u32 iv_len,
                                const u8 *ad, u32 ad_len,
                                const u8 *input, u32 ilen,
                                u8 *output, u32 *olen,
                                const u8 *tag, u32 tag_len)
{
#if defined(MBEDTLS_GCM_C)
    if (MBEDTLS_MODE_GCM == ctx->cipher_info->mode) {
        s32 ret;

        *olen = ilen;
        ret = mbedtls_gcm_auth_decrypt(ctx->cipher_ctx, ilen,
                                       iv, iv_len, ad, ad_len,
                                       tag, tag_len, input, output);

        if (ret == MBEDTLS_ERR_GCM_AUTH_FAILED)
            ret = MBEDTLS_ERR_CIPHER_AUTH_FAILED;

        return (ret);
    }
#endif /* MBEDTLS_GCM_C */
#if defined(MBEDTLS_CCM_C)
    if (MBEDTLS_MODE_CCM == ctx->cipher_info->mode) {
        s32 ret;

        *olen = ilen;
        ret = mbedtls_ccm_auth_decrypt(ctx->cipher_ctx, ilen,
                                       iv, iv_len, ad, ad_len,
                                       input, output, tag, tag_len);

        if (ret == MBEDTLS_ERR_CCM_AUTH_FAILED)
            ret = MBEDTLS_ERR_CIPHER_AUTH_FAILED;

        return (ret);
    }
#endif /* MBEDTLS_CCM_C */

    return (MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE);
}
#endif /* MBEDTLS_CIPHER_MODE_AEAD */
#endif





u8 aes_ctx_buf[280];
volatile aes_gcm_alloc_ctx aes_gcm_ctx = {.used = 0, .aes_ctx = (mbedtls_aes_context *) &aes_ctx_buf[0]};


#define mbedtls_free    free
#undef free
#define free            aes_gcm_free

#undef malloc
#define malloc          aes_gcm_malloc

/*
void mbedtls_zeroize( void *v, u32 n ) {
    volatile u8 *p = v; while( n-- ) *p++ = 0;
}
*/
#if defined(MBEDTLS_AES_GCM_MPOOL_ALLOC) && (MBEDTLS_AES_GCM_MPOOL_ALLOC == 1)
void *aes_gcm_malloc(u32 size)
{

    mbedtls_aes_context *ctx = NULL;
    if (size > sizeof(mbedtls_aes_context)) {
        __dbg_printf("mpool aes_gcm_malloc fail! over size\r\n");
        return NULL;
    } else {
        ctx = (mbedtls_aes_context *)osPoolAlloc(mpool_aes_gcm_id);
        if (ctx == NULL) {
            __dbg_printf("mpool aes_gcm_malloc fail!\r\n");
            return NULL;
        }
        __dbg_printf("mpool alloc aes_gcm_ctx success\r\n");
        return (ctx);
    }
}

void aes_gcm_free(void *ctx)
{
    if (ctx == NULL)
        return;
    osPoolFree(mpool_aes_gcm_id, (mbedtls_aes_context *)ctx);
    __dbg_printf("mpool free aes_gcm_ctx success\r\n");
}


#else
void *aes_gcm_malloc(u32 size)
{

    if (size > sizeof(mbedtls_aes_context)) {
        __dbg_printf("aes_gcm_malloc fail!\r\n");
        return NULL;
    } else {
        if (aes_gcm_ctx.used == 0) {
            aes_gcm_ctx.used = 1;
            rtlc_memset(aes_gcm_ctx.aes_ctx, 0, sizeof(mbedtls_aes_context));
            __dbg_printf("alloc aes_gcm_ctx success\r\n");
            return (aes_gcm_ctx.aes_ctx);
        } else {
            __dbg_printf("aes_gcm_ctx be used,alloc fail!\r\n");
            return NULL;
        }
    }
}

void aes_gcm_free(void *ctx)
{

    if (ctx == NULL) {
        return;
    }
    aes_gcm_ctx.used = 0;
    __dbg_printf("free aes_gcm_ctx success\r\n");
}
#endif

//s32 count_malloc = 0;

#if 0
#if defined(MBEDTLS_GCM_C)
/* shared by all GCM ciphers */
static void *gcm_ctx_alloc(void)
{
    //void *ctx = mbedtls_calloc( 1, sizeof( mbedtls_gcm_context ) );
    void *ctx = malloc(1 * sizeof(mbedtls_gcm_context));

    if (ctx != NULL) {
        mbedtls_gcm_init((mbedtls_gcm_context *) ctx);
    }
    return (ctx);
}

static void gcm_ctx_free(void *ctx)
{
    mbedtls_gcm_free(ctx);
    mbedtls_free(ctx);
}
#endif /* MBEDTLS_GCM_C */

#if defined(MBEDTLS_CCM_C)
/* shared by all CCM ciphers */
static void *ccm_ctx_alloc(void)
{
    void *ctx = mbedtls_calloc(1, sizeof(mbedtls_ccm_context));

    if (ctx != NULL)
        mbedtls_ccm_init((mbedtls_ccm_context *) ctx);

    return (ctx);
}

static void ccm_ctx_free(void *ctx)
{
    mbedtls_ccm_free(ctx);
    mbedtls_free(ctx);
}
#endif /* MBEDTLS_CCM_C */
#endif

#if defined(MBEDTLS_AES_C)

s32 aes_crypt_ecb_wrap(void *ctx, mbedtls_operation_t operation,
                       const u8 *input, u8 *output)
{
    return mbedtls_aes_crypt_ecb((mbedtls_aes_context *) ctx, operation, input, output);
}
#if 0
#if defined(MBEDTLS_CIPHER_MODE_CBC)
static s32 aes_crypt_cbc_wrap(void *ctx, mbedtls_operation_t operation, u32 length,
                              u8 *iv, const u8 *input, u8 *output)
{
    return mbedtls_aes_crypt_cbc((mbedtls_aes_context *) ctx, operation, length, iv, input,
                                 output);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
static s32 aes_crypt_cfb128_wrap(void *ctx, mbedtls_operation_t operation,
                                 u32 length, size_t *iv_off, u8 *iv,
                                 const u8 *input, u8 *output)
{
    return mbedtls_aes_crypt_cfb128((mbedtls_aes_context *) ctx, operation, length, iv_off, iv,
                                    input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
static s32 aes_crypt_ctr_wrap(void *ctx, u32 length, u32 *nc_off,
                              u8 *nonce_counter, u8 *stream_block,
                              const u8 *input, u8 *output)
{
    return mbedtls_aes_crypt_ctr((mbedtls_aes_context *) ctx, length, nc_off, nonce_counter,
                                 stream_block, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */
#endif

s32 aes_setkey_dec_wrap(void *ctx, const u8 *key,
                        u32 key_bitlen)
{
    return mbedtls_aes_setkey_dec((mbedtls_aes_context *) ctx, key, key_bitlen);
}

s32 aes_setkey_enc_wrap(void *ctx, const u8 *key,
                        u32 key_bitlen)
{
    return mbedtls_aes_setkey_enc((mbedtls_aes_context *) ctx, key, key_bitlen);
}
#if 1
void *aes_ctx_alloc(void)
{
    //mbedtls_aes_context *aes = mbedtls_calloc( 1, sizeof( mbedtls_aes_context ) );
    // mbedtls_aes_context is 280bytes
    mbedtls_aes_context *aes = (mbedtls_aes_context *)malloc(1 * sizeof(mbedtls_aes_context));

    if (aes == NULL) {
        return (NULL);
    } else {

        //++count_malloc;
        //__dbg_printf("sizeof(mbedtls_aes_context) = %d count = %d\r\n",sizeof(mbedtls_aes_context),count_malloc);
        mbedtls_aes_init(aes);
    }
    return (aes);
}

void aes_ctx_free(void *ctx)
{
    mbedtls_aes_free((mbedtls_aes_context *) ctx);
    mbedtls_free(ctx);
}
#endif

const mbedtls_cipher_base_t aes_info = {
    MBEDTLS_CIPHER_ID_AES,
    aes_crypt_ecb_wrap,
#if 0
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    aes_crypt_cbc_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    aes_crypt_cfb128_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    aes_crypt_ctr_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
#endif
    aes_setkey_enc_wrap,
    aes_setkey_dec_wrap,
    aes_ctx_alloc,
    aes_ctx_free
};

const mbedtls_cipher_info_t aes_128_ecb_info = {
    MBEDTLS_CIPHER_AES_128_ECB,
    MBEDTLS_MODE_ECB,
    128,
    "AES-128-ECB",
    16,
    0,
    16,
    &aes_info
};

const mbedtls_cipher_info_t aes_192_ecb_info = {
    MBEDTLS_CIPHER_AES_192_ECB,
    MBEDTLS_MODE_ECB,
    192,
    "AES-192-ECB",
    16,
    0,
    16,
    &aes_info
};

const mbedtls_cipher_info_t aes_256_ecb_info = {
    MBEDTLS_CIPHER_AES_256_ECB,
    MBEDTLS_MODE_ECB,
    256,
    "AES-256-ECB",
    16,
    0,
    16,
    &aes_info
};

#if 0
#if defined(MBEDTLS_CIPHER_MODE_CBC)
static const mbedtls_cipher_info_t aes_128_cbc_info = {
    MBEDTLS_CIPHER_AES_128_CBC,
    MBEDTLS_MODE_CBC,
    128,
    "AES-128-CBC",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_192_cbc_info = {
    MBEDTLS_CIPHER_AES_192_CBC,
    MBEDTLS_MODE_CBC,
    192,
    "AES-192-CBC",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_256_cbc_info = {
    MBEDTLS_CIPHER_AES_256_CBC,
    MBEDTLS_MODE_CBC,
    256,
    "AES-256-CBC",
    16,
    0,
    16,
    &aes_info
};
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
static const mbedtls_cipher_info_t aes_128_cfb128_info = {
    MBEDTLS_CIPHER_AES_128_CFB128,
    MBEDTLS_MODE_CFB,
    128,
    "AES-128-CFB128",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_192_cfb128_info = {
    MBEDTLS_CIPHER_AES_192_CFB128,
    MBEDTLS_MODE_CFB,
    192,
    "AES-192-CFB128",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_256_cfb128_info = {
    MBEDTLS_CIPHER_AES_256_CFB128,
    MBEDTLS_MODE_CFB,
    256,
    "AES-256-CFB128",
    16,
    0,
    16,
    &aes_info
};
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
static const mbedtls_cipher_info_t aes_128_ctr_info = {
    MBEDTLS_CIPHER_AES_128_CTR,
    MBEDTLS_MODE_CTR,
    128,
    "AES-128-CTR",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_192_ctr_info = {
    MBEDTLS_CIPHER_AES_192_CTR,
    MBEDTLS_MODE_CTR,
    192,
    "AES-192-CTR",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_256_ctr_info = {
    MBEDTLS_CIPHER_AES_256_CTR,
    MBEDTLS_MODE_CTR,
    256,
    "AES-256-CTR",
    16,
    0,
    16,
    &aes_info
};
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#if defined(MBEDTLS_GCM_C)
static s32 gcm_aes_setkey_wrap(void *ctx, const u8 *key,
                               u32 key_bitlen)
{
    return mbedtls_gcm_setkey((mbedtls_gcm_context *) ctx, MBEDTLS_CIPHER_ID_AES,
                              key, key_bitlen);
}

static const mbedtls_cipher_base_t gcm_aes_info = {
    MBEDTLS_CIPHER_ID_AES,
    NULL,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    gcm_aes_setkey_wrap,
    gcm_aes_setkey_wrap,
    NULL,
    NULL
    //gcm_ctx_alloc,
    //gcm_ctx_free
};

static const mbedtls_cipher_info_t aes_128_gcm_info = {
    MBEDTLS_CIPHER_AES_128_GCM,
    MBEDTLS_MODE_GCM,
    128,
    "AES-128-GCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &gcm_aes_info
};

static const mbedtls_cipher_info_t aes_192_gcm_info = {
    MBEDTLS_CIPHER_AES_192_GCM,
    MBEDTLS_MODE_GCM,
    192,
    "AES-192-GCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &gcm_aes_info
};

static const mbedtls_cipher_info_t aes_256_gcm_info = {
    MBEDTLS_CIPHER_AES_256_GCM,
    MBEDTLS_MODE_GCM,
    256,
    "AES-256-GCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &gcm_aes_info
};
#endif /* MBEDTLS_GCM_C */

#if defined(MBEDTLS_CCM_C)
static s32 ccm_aes_setkey_wrap(void *ctx, const u8 *key,
                               u32 key_bitlen)
{
    return mbedtls_ccm_setkey((mbedtls_ccm_context *) ctx, MBEDTLS_CIPHER_ID_AES,
                              key, key_bitlen);
}

static const mbedtls_cipher_base_t ccm_aes_info = {
    MBEDTLS_CIPHER_ID_AES,
    NULL,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    ccm_aes_setkey_wrap,
    ccm_aes_setkey_wrap,
    ccm_ctx_alloc,
    ccm_ctx_free,
};

static const mbedtls_cipher_info_t aes_128_ccm_info = {
    MBEDTLS_CIPHER_AES_128_CCM,
    MBEDTLS_MODE_CCM,
    128,
    "AES-128-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_aes_info
};

static const mbedtls_cipher_info_t aes_192_ccm_info = {
    MBEDTLS_CIPHER_AES_192_CCM,
    MBEDTLS_MODE_CCM,
    192,
    "AES-192-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_aes_info
};

static const mbedtls_cipher_info_t aes_256_ccm_info = {
    MBEDTLS_CIPHER_AES_256_CCM,
    MBEDTLS_MODE_CCM,
    256,
    "AES-256-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_aes_info
};
#endif /* MBEDTLS_CCM_C */
#endif

#endif /* MBEDTLS_AES_C */

#if 0
#if defined(MBEDTLS_CAMELLIA_C)

static s32 camellia_crypt_ecb_wrap(void *ctx, mbedtls_operation_t operation,
                                   const u8 *input, u8 *output)
{
    return mbedtls_camellia_crypt_ecb((mbedtls_camellia_context *) ctx, operation, input,
                                      output);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static s32 camellia_crypt_cbc_wrap(void *ctx, mbedtls_operation_t operation,
                                   u32 length, u8 *iv,
                                   const u8 *input, u8 *output)
{
    return mbedtls_camellia_crypt_cbc((mbedtls_camellia_context *) ctx, operation, length, iv,
                                      input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
static s32 camellia_crypt_cfb128_wrap(void *ctx, mbedtls_operation_t operation,
                                      u32 length, size_t *iv_off, u8 *iv,
                                      const u8 *input, u8 *output)
{
    return mbedtls_camellia_crypt_cfb128((mbedtls_camellia_context *) ctx, operation, length,
                                         iv_off, iv, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
static s32 camellia_crypt_ctr_wrap(void *ctx, u32 length, u32 *nc_off,
                                   u8 *nonce_counter, u8 *stream_block,
                                   const u8 *input, u8 *output)
{
    return mbedtls_camellia_crypt_ctr((mbedtls_camellia_context *) ctx, length, nc_off,
                                      nonce_counter, stream_block, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

static s32 camellia_setkey_dec_wrap(void *ctx, const u8 *key,
                                    u32 key_bitlen)
{
    return mbedtls_camellia_setkey_dec((mbedtls_camellia_context *) ctx, key, key_bitlen);
}

static s32 camellia_setkey_enc_wrap(void *ctx, const u8 *key,
                                    u32 key_bitlen)
{
    return mbedtls_camellia_setkey_enc((mbedtls_camellia_context *) ctx, key, key_bitlen);
}

static void *camellia_ctx_alloc(void)
{
    mbedtls_camellia_context *ctx;
    ctx = mbedtls_calloc(1, sizeof(mbedtls_camellia_context));

    if (ctx == NULL)
        return (NULL);

    mbedtls_camellia_init(ctx);

    return (ctx);
}

static void camellia_ctx_free(void *ctx)
{
    mbedtls_camellia_free((mbedtls_camellia_context *) ctx);
    mbedtls_free(ctx);
}

static const mbedtls_cipher_base_t camellia_info = {
    MBEDTLS_CIPHER_ID_CAMELLIA,
    camellia_crypt_ecb_wrap,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    camellia_crypt_cbc_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    camellia_crypt_cfb128_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    camellia_crypt_ctr_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    camellia_setkey_enc_wrap,
    camellia_setkey_dec_wrap,
    camellia_ctx_alloc,
    camellia_ctx_free
};

static const mbedtls_cipher_info_t camellia_128_ecb_info = {
    MBEDTLS_CIPHER_CAMELLIA_128_ECB,
    MBEDTLS_MODE_ECB,
    128,
    "CAMELLIA-128-ECB",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_192_ecb_info = {
    MBEDTLS_CIPHER_CAMELLIA_192_ECB,
    MBEDTLS_MODE_ECB,
    192,
    "CAMELLIA-192-ECB",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_256_ecb_info = {
    MBEDTLS_CIPHER_CAMELLIA_256_ECB,
    MBEDTLS_MODE_ECB,
    256,
    "CAMELLIA-256-ECB",
    16,
    0,
    16,
    &camellia_info
};

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static const mbedtls_cipher_info_t camellia_128_cbc_info = {
    MBEDTLS_CIPHER_CAMELLIA_128_CBC,
    MBEDTLS_MODE_CBC,
    128,
    "CAMELLIA-128-CBC",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_192_cbc_info = {
    MBEDTLS_CIPHER_CAMELLIA_192_CBC,
    MBEDTLS_MODE_CBC,
    192,
    "CAMELLIA-192-CBC",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_256_cbc_info = {
    MBEDTLS_CIPHER_CAMELLIA_256_CBC,
    MBEDTLS_MODE_CBC,
    256,
    "CAMELLIA-256-CBC",
    16,
    0,
    16,
    &camellia_info
};
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
static const mbedtls_cipher_info_t camellia_128_cfb128_info = {
    MBEDTLS_CIPHER_CAMELLIA_128_CFB128,
    MBEDTLS_MODE_CFB,
    128,
    "CAMELLIA-128-CFB128",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_192_cfb128_info = {
    MBEDTLS_CIPHER_CAMELLIA_192_CFB128,
    MBEDTLS_MODE_CFB,
    192,
    "CAMELLIA-192-CFB128",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_256_cfb128_info = {
    MBEDTLS_CIPHER_CAMELLIA_256_CFB128,
    MBEDTLS_MODE_CFB,
    256,
    "CAMELLIA-256-CFB128",
    16,
    0,
    16,
    &camellia_info
};
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
static const mbedtls_cipher_info_t camellia_128_ctr_info = {
    MBEDTLS_CIPHER_CAMELLIA_128_CTR,
    MBEDTLS_MODE_CTR,
    128,
    "CAMELLIA-128-CTR",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_192_ctr_info = {
    MBEDTLS_CIPHER_CAMELLIA_192_CTR,
    MBEDTLS_MODE_CTR,
    192,
    "CAMELLIA-192-CTR",
    16,
    0,
    16,
    &camellia_info
};

static const mbedtls_cipher_info_t camellia_256_ctr_info = {
    MBEDTLS_CIPHER_CAMELLIA_256_CTR,
    MBEDTLS_MODE_CTR,
    256,
    "CAMELLIA-256-CTR",
    16,
    0,
    16,
    &camellia_info
};
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#if defined(MBEDTLS_GCM_C)
static s32 gcm_camellia_setkey_wrap(void *ctx, const u8 *key,
                                    u32 key_bitlen)
{
    return mbedtls_gcm_setkey((mbedtls_gcm_context *) ctx, MBEDTLS_CIPHER_ID_CAMELLIA,
                              key, key_bitlen);
}

static const mbedtls_cipher_base_t gcm_camellia_info = {
    MBEDTLS_CIPHER_ID_CAMELLIA,
    NULL,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    gcm_camellia_setkey_wrap,
    gcm_camellia_setkey_wrap,
    NULL,
    NULL
    //gcm_ctx_alloc,
    //gcm_ctx_free,
};

static const mbedtls_cipher_info_t camellia_128_gcm_info = {
    MBEDTLS_CIPHER_CAMELLIA_128_GCM,
    MBEDTLS_MODE_GCM,
    128,
    "CAMELLIA-128-GCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &gcm_camellia_info
};

static const mbedtls_cipher_info_t camellia_192_gcm_info = {
    MBEDTLS_CIPHER_CAMELLIA_192_GCM,
    MBEDTLS_MODE_GCM,
    192,
    "CAMELLIA-192-GCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &gcm_camellia_info
};

static const mbedtls_cipher_info_t camellia_256_gcm_info = {
    MBEDTLS_CIPHER_CAMELLIA_256_GCM,
    MBEDTLS_MODE_GCM,
    256,
    "CAMELLIA-256-GCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &gcm_camellia_info
};
#endif /* MBEDTLS_GCM_C */

#if defined(MBEDTLS_CCM_C)
static s32 ccm_camellia_setkey_wrap(void *ctx, const u8 *key,
                                    u32 key_bitlen)
{
    return mbedtls_ccm_setkey((mbedtls_ccm_context *) ctx, MBEDTLS_CIPHER_ID_CAMELLIA,
                              key, key_bitlen);
}

static const mbedtls_cipher_base_t ccm_camellia_info = {
    MBEDTLS_CIPHER_ID_CAMELLIA,
    NULL,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    ccm_camellia_setkey_wrap,
    ccm_camellia_setkey_wrap,
    ccm_ctx_alloc,
    ccm_ctx_free,
};

static const mbedtls_cipher_info_t camellia_128_ccm_info = {
    MBEDTLS_CIPHER_CAMELLIA_128_CCM,
    MBEDTLS_MODE_CCM,
    128,
    "CAMELLIA-128-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_camellia_info
};

static const mbedtls_cipher_info_t camellia_192_ccm_info = {
    MBEDTLS_CIPHER_CAMELLIA_192_CCM,
    MBEDTLS_MODE_CCM,
    192,
    "CAMELLIA-192-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_camellia_info
};

static const mbedtls_cipher_info_t camellia_256_ccm_info = {
    MBEDTLS_CIPHER_CAMELLIA_256_CCM,
    MBEDTLS_MODE_CCM,
    256,
    "CAMELLIA-256-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_camellia_info
};
#endif /* MBEDTLS_CCM_C */

#endif /* MBEDTLS_CAMELLIA_C */

#if defined(MBEDTLS_DES_C)

static s32 des_crypt_ecb_wrap(void *ctx, mbedtls_operation_t operation,
                              const u8 *input, u8 *output)
{
    ((void) operation);
    return mbedtls_des_crypt_ecb((mbedtls_des_context *) ctx, input, output);
}

static s32 des3_crypt_ecb_wrap(void *ctx, mbedtls_operation_t operation,
                               const u8 *input, u8 *output)
{
    ((void) operation);
    return mbedtls_des3_crypt_ecb((mbedtls_des3_context *) ctx, input, output);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static s32 des_crypt_cbc_wrap(void *ctx, mbedtls_operation_t operation, u32 length,
                              u8 *iv, const u8 *input, u8 *output)
{
    return mbedtls_des_crypt_cbc((mbedtls_des_context *) ctx, operation, length, iv, input,
                                 output);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static s32 des3_crypt_cbc_wrap(void *ctx, mbedtls_operation_t operation, u32 length,
                               u8 *iv, const u8 *input, u8 *output)
{
    return mbedtls_des3_crypt_cbc((mbedtls_des3_context *) ctx, operation, length, iv, input,
                                  output);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

static s32 des_setkey_dec_wrap(void *ctx, const u8 *key,
                               u32 key_bitlen)
{
    ((void) key_bitlen);

    return mbedtls_des_setkey_dec((mbedtls_des_context *) ctx, key);
}

static s32 des_setkey_enc_wrap(void *ctx, const u8 *key,
                               u32 key_bitlen)
{
    ((void) key_bitlen);

    return mbedtls_des_setkey_enc((mbedtls_des_context *) ctx, key);
}

static s32 des3_set2key_dec_wrap(void *ctx, const u8 *key,
                                 u32 key_bitlen)
{
    ((void) key_bitlen);

    return mbedtls_des3_set2key_dec((mbedtls_des3_context *) ctx, key);
}

static s32 des3_set2key_enc_wrap(void *ctx, const u8 *key,
                                 u32 key_bitlen)
{
    ((void) key_bitlen);

    return mbedtls_des3_set2key_enc((mbedtls_des3_context *) ctx, key);
}

static s32 des3_set3key_dec_wrap(void *ctx, const u8 *key,
                                 u32 key_bitlen)
{
    ((void) key_bitlen);

    return mbedtls_des3_set3key_dec((mbedtls_des3_context *) ctx, key);
}

static s32 des3_set3key_enc_wrap(void *ctx, const u8 *key,
                                 u32 key_bitlen)
{
    ((void) key_bitlen);

    return mbedtls_des3_set3key_enc((mbedtls_des3_context *) ctx, key);
}

static void *des_ctx_alloc(void)
{
    mbedtls_des_context *des = mbedtls_calloc(1, sizeof(mbedtls_des_context));

    if (des == NULL)
        return (NULL);

    mbedtls_des_init(des);

    return (des);
}

static void des_ctx_free(void *ctx)
{
    mbedtls_des_free((mbedtls_des_context *) ctx);
    mbedtls_free(ctx);
}

static void *des3_ctx_alloc(void)
{
    mbedtls_des3_context *des3;
    des3 = mbedtls_calloc(1, sizeof(mbedtls_des3_context));

    if (des3 == NULL)
        return (NULL);

    mbedtls_des3_init(des3);

    return (des3);
}

static void des3_ctx_free(void *ctx)
{
    mbedtls_des3_free((mbedtls_des3_context *) ctx);
    mbedtls_free(ctx);
}

static const mbedtls_cipher_base_t des_info = {
    MBEDTLS_CIPHER_ID_DES,
    des_crypt_ecb_wrap,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    des_crypt_cbc_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    des_setkey_enc_wrap,
    des_setkey_dec_wrap,
    des_ctx_alloc,
    des_ctx_free
};

static const mbedtls_cipher_info_t des_ecb_info = {
    MBEDTLS_CIPHER_DES_ECB,
    MBEDTLS_MODE_ECB,
    MBEDTLS_KEY_LENGTH_DES,
    "DES-ECB",
    8,
    0,
    8,
    &des_info
};

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static const mbedtls_cipher_info_t des_cbc_info = {
    MBEDTLS_CIPHER_DES_CBC,
    MBEDTLS_MODE_CBC,
    MBEDTLS_KEY_LENGTH_DES,
    "DES-CBC",
    8,
    0,
    8,
    &des_info
};
#endif /* MBEDTLS_CIPHER_MODE_CBC */

static const mbedtls_cipher_base_t des_ede_info = {
    MBEDTLS_CIPHER_ID_DES,
    des3_crypt_ecb_wrap,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    des3_crypt_cbc_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    des3_set2key_enc_wrap,
    des3_set2key_dec_wrap,
    des3_ctx_alloc,
    des3_ctx_free
};

static const mbedtls_cipher_info_t des_ede_ecb_info = {
    MBEDTLS_CIPHER_DES_EDE_ECB,
    MBEDTLS_MODE_ECB,
    MBEDTLS_KEY_LENGTH_DES_EDE,
    "DES-EDE-ECB",
    8,
    0,
    8,
    &des_ede_info
};

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static const mbedtls_cipher_info_t des_ede_cbc_info = {
    MBEDTLS_CIPHER_DES_EDE_CBC,
    MBEDTLS_MODE_CBC,
    MBEDTLS_KEY_LENGTH_DES_EDE,
    "DES-EDE-CBC",
    8,
    0,
    8,
    &des_ede_info
};
#endif /* MBEDTLS_CIPHER_MODE_CBC */

static const mbedtls_cipher_base_t des_ede3_info = {
    MBEDTLS_CIPHER_ID_3DES,
    des3_crypt_ecb_wrap,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    des3_crypt_cbc_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    des3_set3key_enc_wrap,
    des3_set3key_dec_wrap,
    des3_ctx_alloc,
    des3_ctx_free
};

static const mbedtls_cipher_info_t des_ede3_ecb_info = {
    MBEDTLS_CIPHER_DES_EDE3_ECB,
    MBEDTLS_MODE_ECB,
    MBEDTLS_KEY_LENGTH_DES_EDE3,
    "DES-EDE3-ECB",
    8,
    0,
    8,
    &des_ede3_info
};
#if defined(MBEDTLS_CIPHER_MODE_CBC)
static const mbedtls_cipher_info_t des_ede3_cbc_info = {
    MBEDTLS_CIPHER_DES_EDE3_CBC,
    MBEDTLS_MODE_CBC,
    MBEDTLS_KEY_LENGTH_DES_EDE3,
    "DES-EDE3-CBC",
    8,
    0,
    8,
    &des_ede3_info
};
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#endif /* MBEDTLS_DES_C */

#if defined(MBEDTLS_BLOWFISH_C)

static s32 blowfish_crypt_ecb_wrap(void *ctx, mbedtls_operation_t operation,
                                   const u8 *input, u8 *output)
{
    return mbedtls_blowfish_crypt_ecb((mbedtls_blowfish_context *) ctx, operation, input,
                                      output);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static s32 blowfish_crypt_cbc_wrap(void *ctx, mbedtls_operation_t operation,
                                   u32 length, u8 *iv, const u8 *input,
                                   u8 *output)
{
    return mbedtls_blowfish_crypt_cbc((mbedtls_blowfish_context *) ctx, operation, length, iv,
                                      input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
static s32 blowfish_crypt_cfb64_wrap(void *ctx, mbedtls_operation_t operation,
                                     u32 length, size_t *iv_off, u8 *iv,
                                     const u8 *input, u8 *output)
{
    return mbedtls_blowfish_crypt_cfb64((mbedtls_blowfish_context *) ctx, operation, length,
                                        iv_off, iv, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
static s32 blowfish_crypt_ctr_wrap(void *ctx, u32 length, u32 *nc_off,
                                   u8 *nonce_counter, u8 *stream_block,
                                   const u8 *input, u8 *output)
{
    return mbedtls_blowfish_crypt_ctr((mbedtls_blowfish_context *) ctx, length, nc_off,
                                      nonce_counter, stream_block, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

static s32 blowfish_setkey_wrap(void *ctx, const u8 *key,
                                u32 key_bitlen)
{
    return mbedtls_blowfish_setkey((mbedtls_blowfish_context *) ctx, key, key_bitlen);
}

static void *blowfish_ctx_alloc(void)
{
    mbedtls_blowfish_context *ctx;
    ctx = mbedtls_calloc(1, sizeof(mbedtls_blowfish_context));

    if (ctx == NULL)
        return (NULL);

    mbedtls_blowfish_init(ctx);

    return (ctx);
}

static void blowfish_ctx_free(void *ctx)
{
    mbedtls_blowfish_free((mbedtls_blowfish_context *) ctx);
    mbedtls_free(ctx);
}

static const mbedtls_cipher_base_t blowfish_info = {
    MBEDTLS_CIPHER_ID_BLOWFISH,
    blowfish_crypt_ecb_wrap,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    blowfish_crypt_cbc_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    blowfish_crypt_cfb64_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    blowfish_crypt_ctr_wrap,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    NULL,
#endif
    blowfish_setkey_wrap,
    blowfish_setkey_wrap,
    blowfish_ctx_alloc,
    blowfish_ctx_free
};

static const mbedtls_cipher_info_t blowfish_ecb_info = {
    MBEDTLS_CIPHER_BLOWFISH_ECB,
    MBEDTLS_MODE_ECB,
    128,
    "BLOWFISH-ECB",
    8,
    MBEDTLS_CIPHER_VARIABLE_KEY_LEN,
    8,
    &blowfish_info
};

#if defined(MBEDTLS_CIPHER_MODE_CBC)
static const mbedtls_cipher_info_t blowfish_cbc_info = {
    MBEDTLS_CIPHER_BLOWFISH_CBC,
    MBEDTLS_MODE_CBC,
    128,
    "BLOWFISH-CBC",
    8,
    MBEDTLS_CIPHER_VARIABLE_KEY_LEN,
    8,
    &blowfish_info
};
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
static const mbedtls_cipher_info_t blowfish_cfb64_info = {
    MBEDTLS_CIPHER_BLOWFISH_CFB64,
    MBEDTLS_MODE_CFB,
    128,
    "BLOWFISH-CFB64",
    8,
    MBEDTLS_CIPHER_VARIABLE_KEY_LEN,
    8,
    &blowfish_info
};
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
static const mbedtls_cipher_info_t blowfish_ctr_info = {
    MBEDTLS_CIPHER_BLOWFISH_CTR,
    MBEDTLS_MODE_CTR,
    128,
    "BLOWFISH-CTR",
    8,
    MBEDTLS_CIPHER_VARIABLE_KEY_LEN,
    8,
    &blowfish_info
};
#endif /* MBEDTLS_CIPHER_MODE_CTR */
#endif /* MBEDTLS_BLOWFISH_C */

#if defined(MBEDTLS_ARC4_C)
static s32 arc4_crypt_stream_wrap(void *ctx, u32 length,
                                  const u8 *input,
                                  u8 *output)
{
    return (mbedtls_arc4_crypt((mbedtls_arc4_context *) ctx, length, input, output));
}

static s32 arc4_setkey_wrap(void *ctx, const u8 *key,
                            u32 key_bitlen)
{
    /* we get key_bitlen in bits, arc4 expects it in bytes */
    if (key_bitlen % 8 != 0)
        return (MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA);

    mbedtls_arc4_setup((mbedtls_arc4_context *) ctx, key, key_bitlen / 8);
    return (0);
}

static void *arc4_ctx_alloc(void)
{
    mbedtls_arc4_context *ctx;
    ctx = mbedtls_calloc(1, sizeof(mbedtls_arc4_context));

    if (ctx == NULL)
        return (NULL);

    mbedtls_arc4_init(ctx);

    return (ctx);
}

static void arc4_ctx_free(void *ctx)
{
    mbedtls_arc4_free((mbedtls_arc4_context *) ctx);
    mbedtls_free(ctx);
}

static const mbedtls_cipher_base_t arc4_base_info = {
    MBEDTLS_CIPHER_ID_ARC4,
    NULL,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    arc4_crypt_stream_wrap,
#endif
    arc4_setkey_wrap,
    arc4_setkey_wrap,
    arc4_ctx_alloc,
    arc4_ctx_free
};

static const mbedtls_cipher_info_t arc4_128_info = {
    MBEDTLS_CIPHER_ARC4_128,
    MBEDTLS_MODE_STREAM,
    128,
    "ARC4-128",
    0,
    0,
    1,
    &arc4_base_info
};
#endif /* MBEDTLS_ARC4_C */

#if defined(MBEDTLS_CIPHER_NULL_CIPHER)
static s32 null_crypt_stream(void *ctx, u32 length,
                             const u8 *input,
                             u8 *output)
{
    ((void) ctx);
    memmove(output, input, length);
    return (0);
}

static s32 null_setkey(void *ctx, const u8 *key,
                       u32 key_bitlen)
{
    ((void) ctx);
    ((void) key);
    ((void) key_bitlen);

    return (0);
}

static void *null_ctx_alloc(void)
{
    return ((void *) 1);
}

static void null_ctx_free(void *ctx)
{
    ((void) ctx);
}

static const mbedtls_cipher_base_t null_base_info = {
    MBEDTLS_CIPHER_ID_NULL,
    NULL,
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    NULL,
#endif
#if defined(MBEDTLS_CIPHER_MODE_STREAM)
    null_crypt_stream,
#endif
    null_setkey,
    null_setkey,
    null_ctx_alloc,
    null_ctx_free
};

static const mbedtls_cipher_info_t null_cipher_info = {
    MBEDTLS_CIPHER_NULL,
    MBEDTLS_MODE_STREAM,
    0,
    "NULL",
    0,
    0,
    1,
    &null_base_info
};
#endif /* defined(MBEDTLS_CIPHER_NULL_CIPHER) */
#endif

const mbedtls_cipher_definition_t mbedtls_cipher_definitions[] = {
#if defined(MBEDTLS_AES_C)
    { MBEDTLS_CIPHER_AES_128_ECB,          &aes_128_ecb_info },
    { MBEDTLS_CIPHER_AES_192_ECB,          &aes_192_ecb_info },
    { MBEDTLS_CIPHER_AES_256_ECB,          &aes_256_ecb_info },
#if 0
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    { MBEDTLS_CIPHER_AES_128_CBC,          &aes_128_cbc_info },
    { MBEDTLS_CIPHER_AES_192_CBC,          &aes_192_cbc_info },
    { MBEDTLS_CIPHER_AES_256_CBC,          &aes_256_cbc_info },
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    { MBEDTLS_CIPHER_AES_128_CFB128,       &aes_128_cfb128_info },
    { MBEDTLS_CIPHER_AES_192_CFB128,       &aes_192_cfb128_info },
    { MBEDTLS_CIPHER_AES_256_CFB128,       &aes_256_cfb128_info },
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    { MBEDTLS_CIPHER_AES_128_CTR,          &aes_128_ctr_info },
    { MBEDTLS_CIPHER_AES_192_CTR,          &aes_192_ctr_info },
    { MBEDTLS_CIPHER_AES_256_CTR,          &aes_256_ctr_info },
#endif
#if defined(MBEDTLS_GCM_C)
    { MBEDTLS_CIPHER_AES_128_GCM,          &aes_128_gcm_info },
    { MBEDTLS_CIPHER_AES_192_GCM,          &aes_192_gcm_info },
    { MBEDTLS_CIPHER_AES_256_GCM,          &aes_256_gcm_info },
#endif
#if defined(MBEDTLS_CCM_C)
    { MBEDTLS_CIPHER_AES_128_CCM,          &aes_128_ccm_info },
    { MBEDTLS_CIPHER_AES_192_CCM,          &aes_192_ccm_info },
    { MBEDTLS_CIPHER_AES_256_CCM,          &aes_256_ccm_info },
#endif

#endif
#endif /* MBEDTLS_AES_C */

#if 0
#if defined(MBEDTLS_ARC4_C)
    { MBEDTLS_CIPHER_ARC4_128,             &arc4_128_info },
#endif

#if defined(MBEDTLS_BLOWFISH_C)
    { MBEDTLS_CIPHER_BLOWFISH_ECB,         &blowfish_ecb_info },
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    { MBEDTLS_CIPHER_BLOWFISH_CBC,         &blowfish_cbc_info },
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    { MBEDTLS_CIPHER_BLOWFISH_CFB64,       &blowfish_cfb64_info },
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    { MBEDTLS_CIPHER_BLOWFISH_CTR,         &blowfish_ctr_info },
#endif
#endif /* MBEDTLS_BLOWFISH_C */

#if defined(MBEDTLS_CAMELLIA_C)
    { MBEDTLS_CIPHER_CAMELLIA_128_ECB,     &camellia_128_ecb_info },
    { MBEDTLS_CIPHER_CAMELLIA_192_ECB,     &camellia_192_ecb_info },
    { MBEDTLS_CIPHER_CAMELLIA_256_ECB,     &camellia_256_ecb_info },
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    { MBEDTLS_CIPHER_CAMELLIA_128_CBC,     &camellia_128_cbc_info },
    { MBEDTLS_CIPHER_CAMELLIA_192_CBC,     &camellia_192_cbc_info },
    { MBEDTLS_CIPHER_CAMELLIA_256_CBC,     &camellia_256_cbc_info },
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    { MBEDTLS_CIPHER_CAMELLIA_128_CFB128,  &camellia_128_cfb128_info },
    { MBEDTLS_CIPHER_CAMELLIA_192_CFB128,  &camellia_192_cfb128_info },
    { MBEDTLS_CIPHER_CAMELLIA_256_CFB128,  &camellia_256_cfb128_info },
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    { MBEDTLS_CIPHER_CAMELLIA_128_CTR,     &camellia_128_ctr_info },
    { MBEDTLS_CIPHER_CAMELLIA_192_CTR,     &camellia_192_ctr_info },
    { MBEDTLS_CIPHER_CAMELLIA_256_CTR,     &camellia_256_ctr_info },
#endif
#if defined(MBEDTLS_GCM_C)
    { MBEDTLS_CIPHER_CAMELLIA_128_GCM,     &camellia_128_gcm_info },
    { MBEDTLS_CIPHER_CAMELLIA_192_GCM,     &camellia_192_gcm_info },
    { MBEDTLS_CIPHER_CAMELLIA_256_GCM,     &camellia_256_gcm_info },
#endif
#if defined(MBEDTLS_CCM_C)
    { MBEDTLS_CIPHER_CAMELLIA_128_CCM,     &camellia_128_ccm_info },
    { MBEDTLS_CIPHER_CAMELLIA_192_CCM,     &camellia_192_ccm_info },
    { MBEDTLS_CIPHER_CAMELLIA_256_CCM,     &camellia_256_ccm_info },
#endif
#endif /* MBEDTLS_CAMELLIA_C */

#if defined(MBEDTLS_DES_C)
    { MBEDTLS_CIPHER_DES_ECB,              &des_ecb_info },
    { MBEDTLS_CIPHER_DES_EDE_ECB,          &des_ede_ecb_info },
    { MBEDTLS_CIPHER_DES_EDE3_ECB,         &des_ede3_ecb_info },
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    { MBEDTLS_CIPHER_DES_CBC,              &des_cbc_info },
    { MBEDTLS_CIPHER_DES_EDE_CBC,          &des_ede_cbc_info },
    { MBEDTLS_CIPHER_DES_EDE3_CBC,         &des_ede3_cbc_info },
#endif
#endif /* MBEDTLS_DES_C */

#if defined(MBEDTLS_CIPHER_NULL_CIPHER)
    { MBEDTLS_CIPHER_NULL,                 &null_cipher_info },
#endif /* MBEDTLS_CIPHER_NULL_CIPHER */
#endif
    { MBEDTLS_CIPHER_NONE, NULL }
};

#define NUM_CIPHERS sizeof mbedtls_cipher_definitions / sizeof mbedtls_cipher_definitions[0]
s32 mbedtls_cipher_supported[NUM_CIPHERS];

//#endif /* MBEDTLS_CIPHER_C */







// #define AES_BLOCK_SIZE 16

/*
 * AES (Rijndael) cipher
 * Copyright (c) 2003-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

/* #define FULL_UNROLL */
#define AES_SMALL_TABLES

extern const u32 Te0[256];
extern const u32 Te1[256];
extern const u32 Te2[256];
extern const u32 Te3[256];
extern const u32 Te4[256];
extern const u32 Td0[256];
extern const u32 Td1[256];
extern const u32 Td2[256];
extern const u32 Td3[256];
extern const u32 Td4[256];
extern const u32 rcon[10];
extern const u8 Td4s[256];
extern const u8 rcons[10];

#ifndef AES_SMALL_TABLES

#define RCON(i) rcon[(i)]

#define TE0(i) Te0[((i) >> 24) & 0xff]
#define TE1(i) Te1[((i) >> 16) & 0xff]
#define TE2(i) Te2[((i) >> 8) & 0xff]
#define TE3(i) Te3[(i) & 0xff]
#define TE41(i) (Te4[((i) >> 24) & 0xff] & 0xff000000)
#define TE42(i) (Te4[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE43(i) (Te4[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE44(i) (Te4[(i) & 0xff] & 0x000000ff)
#define TE421(i) (Te4[((i) >> 16) & 0xff] & 0xff000000)
#define TE432(i) (Te4[((i) >> 8) & 0xff] & 0x00ff0000)
#define TE443(i) (Te4[(i) & 0xff] & 0x0000ff00)
#define TE414(i) (Te4[((i) >> 24) & 0xff] & 0x000000ff)
#define TE411(i) (Te4[((i) >> 24) & 0xff] & 0xff000000)
#define TE422(i) (Te4[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE433(i) (Te4[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE444(i) (Te4[(i) & 0xff] & 0x000000ff)
#define TE4(i) (Te4[(i)] & 0x000000ff)

#define TD0(i) Td0[((i) >> 24) & 0xff]
#define TD1(i) Td1[((i) >> 16) & 0xff]
#define TD2(i) Td2[((i) >> 8) & 0xff]
#define TD3(i) Td3[(i) & 0xff]
#define TD41(i) (Td4[((i) >> 24) & 0xff] & 0xff000000)
#define TD42(i) (Td4[((i) >> 16) & 0xff] & 0x00ff0000)
#define TD43(i) (Td4[((i) >> 8) & 0xff] & 0x0000ff00)
#define TD44(i) (Td4[(i) & 0xff] & 0x000000ff)
#define TD0_(i) Td0[(i) & 0xff]
#define TD1_(i) Td1[(i) & 0xff]
#define TD2_(i) Td2[(i) & 0xff]
#define TD3_(i) Td3[(i) & 0xff]

#else /* AES_SMALL_TABLES */

#define RCON(i) (rcons[(i)] << 24)

static inline u32 rotr(u32 val, s32 bits)
{
    return (val >> bits) | (val << (32 - bits));
}

#define TE0(i) Te0[((i) >> 24) & 0xff]
#define TE1(i) rotr(Te0[((i) >> 16) & 0xff], 8)
#define TE2(i) rotr(Te0[((i) >> 8) & 0xff], 16)
#define TE3(i) rotr(Te0[(i) & 0xff], 24)
#define TE41(i) ((Te0[((i) >> 24) & 0xff] << 8) & 0xff000000)
#define TE42(i) (Te0[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE43(i) (Te0[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE44(i) ((Te0[(i) & 0xff] >> 8) & 0x000000ff)
#define TE421(i) ((Te0[((i) >> 16) & 0xff] << 8) & 0xff000000)
#define TE432(i) (Te0[((i) >> 8) & 0xff] & 0x00ff0000)
#define TE443(i) (Te0[(i) & 0xff] & 0x0000ff00)
#define TE414(i) ((Te0[((i) >> 24) & 0xff] >> 8) & 0x000000ff)
#define TE411(i) ((Te0[((i) >> 24) & 0xff] << 8) & 0xff000000)
#define TE422(i) (Te0[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE433(i) (Te0[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE444(i) ((Te0[(i) & 0xff] >> 8) & 0x000000ff)
#define TE4(i) ((Te0[(i)] >> 8) & 0x000000ff)

#define TD0(i) Td0[((i) >> 24) & 0xff]
#define TD1(i) rotr(Td0[((i) >> 16) & 0xff], 8)
#define TD2(i) rotr(Td0[((i) >> 8) & 0xff], 16)
#define TD3(i) rotr(Td0[(i) & 0xff], 24)
#define TD41(i) (Td4s[((i) >> 24) & 0xff] << 24)
#define TD42(i) (Td4s[((i) >> 16) & 0xff] << 16)
#define TD43(i) (Td4s[((i) >> 8) & 0xff] << 8)
#define TD44(i) (Td4s[(i) & 0xff])
#define TD0_(i) Td0[(i) & 0xff]
#define TD1_(i) rotr(Td0[(i) & 0xff], 8)
#define TD2_(i) rotr(Td0[(i) & 0xff], 16)
#define TD3_(i) rotr(Td0[(i) & 0xff], 24)

#endif /* AES_SMALL_TABLES */

#ifdef _MSC_VER
#define SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
#define GETU32(p) SWAP(*((u32 *)(p)))
#define PUTU32(ct, st) { *((u32 *)(ct)) = SWAP((st)); }
#else
#define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ \
((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#define PUTU32(ct, st) { \
(ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); \
(ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
#endif

#define AES_PRIV_SIZE (4 * 4 * 15 + 4)
#define AES_PRIV_NR_POS (4 * 15)

#define WPA_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
             (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define WPA_PUT_BE32(a, val)                    \
    do {                            \
        (a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);   \
        (a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);   \
        (a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);    \
        (a)[3] = (u8) (((u32) (val)) & 0xff);       \
    } while (0)

#define WPA_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
                 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
                 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
                 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define WPA_PUT_BE64(a, val)                \
        do {                        \
            (a)[0] = (u8) (((u64) (val)) >> 56);    \
            (a)[1] = (u8) (((u64) (val)) >> 48);    \
            (a)[2] = (u8) (((u64) (val)) >> 40);    \
            (a)[3] = (u8) (((u64) (val)) >> 32);    \
            (a)[4] = (u8) (((u64) (val)) >> 24);    \
            (a)[5] = (u8) (((u64) (val)) >> 16);    \
            (a)[6] = (u8) (((u64) (val)) >> 8); \
            (a)[7] = (u8) (((u64) (val)) & 0xff);   \
        } while (0)


#ifndef BIT
#define BIT(x) (1 << (x))
#endif

const u8 rcons[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
    /* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};

const u32 Te0[256] = {
    0xc66363a5U, 0xf87c7c84U, 0xee777799U, 0xf67b7b8dU,
    0xfff2f20dU, 0xd66b6bbdU, 0xde6f6fb1U, 0x91c5c554U,
    0x60303050U, 0x02010103U, 0xce6767a9U, 0x562b2b7dU,
    0xe7fefe19U, 0xb5d7d762U, 0x4dababe6U, 0xec76769aU,
    0x8fcaca45U, 0x1f82829dU, 0x89c9c940U, 0xfa7d7d87U,
    0xeffafa15U, 0xb25959ebU, 0x8e4747c9U, 0xfbf0f00bU,
    0x41adadecU, 0xb3d4d467U, 0x5fa2a2fdU, 0x45afafeaU,
    0x239c9cbfU, 0x53a4a4f7U, 0xe4727296U, 0x9bc0c05bU,
    0x75b7b7c2U, 0xe1fdfd1cU, 0x3d9393aeU, 0x4c26266aU,
    0x6c36365aU, 0x7e3f3f41U, 0xf5f7f702U, 0x83cccc4fU,
    0x6834345cU, 0x51a5a5f4U, 0xd1e5e534U, 0xf9f1f108U,
    0xe2717193U, 0xabd8d873U, 0x62313153U, 0x2a15153fU,
    0x0804040cU, 0x95c7c752U, 0x46232365U, 0x9dc3c35eU,
    0x30181828U, 0x379696a1U, 0x0a05050fU, 0x2f9a9ab5U,
    0x0e070709U, 0x24121236U, 0x1b80809bU, 0xdfe2e23dU,
    0xcdebeb26U, 0x4e272769U, 0x7fb2b2cdU, 0xea75759fU,
    0x1209091bU, 0x1d83839eU, 0x582c2c74U, 0x341a1a2eU,
    0x361b1b2dU, 0xdc6e6eb2U, 0xb45a5aeeU, 0x5ba0a0fbU,
    0xa45252f6U, 0x763b3b4dU, 0xb7d6d661U, 0x7db3b3ceU,
    0x5229297bU, 0xdde3e33eU, 0x5e2f2f71U, 0x13848497U,
    0xa65353f5U, 0xb9d1d168U, 0x00000000U, 0xc1eded2cU,
    0x40202060U, 0xe3fcfc1fU, 0x79b1b1c8U, 0xb65b5bedU,
    0xd46a6abeU, 0x8dcbcb46U, 0x67bebed9U, 0x7239394bU,
    0x944a4adeU, 0x984c4cd4U, 0xb05858e8U, 0x85cfcf4aU,
    0xbbd0d06bU, 0xc5efef2aU, 0x4faaaae5U, 0xedfbfb16U,
    0x864343c5U, 0x9a4d4dd7U, 0x66333355U, 0x11858594U,
    0x8a4545cfU, 0xe9f9f910U, 0x04020206U, 0xfe7f7f81U,
    0xa05050f0U, 0x783c3c44U, 0x259f9fbaU, 0x4ba8a8e3U,
    0xa25151f3U, 0x5da3a3feU, 0x804040c0U, 0x058f8f8aU,
    0x3f9292adU, 0x219d9dbcU, 0x70383848U, 0xf1f5f504U,
    0x63bcbcdfU, 0x77b6b6c1U, 0xafdada75U, 0x42212163U,
    0x20101030U, 0xe5ffff1aU, 0xfdf3f30eU, 0xbfd2d26dU,
    0x81cdcd4cU, 0x180c0c14U, 0x26131335U, 0xc3ecec2fU,
    0xbe5f5fe1U, 0x359797a2U, 0x884444ccU, 0x2e171739U,
    0x93c4c457U, 0x55a7a7f2U, 0xfc7e7e82U, 0x7a3d3d47U,
    0xc86464acU, 0xba5d5de7U, 0x3219192bU, 0xe6737395U,
    0xc06060a0U, 0x19818198U, 0x9e4f4fd1U, 0xa3dcdc7fU,
    0x44222266U, 0x542a2a7eU, 0x3b9090abU, 0x0b888883U,
    0x8c4646caU, 0xc7eeee29U, 0x6bb8b8d3U, 0x2814143cU,
    0xa7dede79U, 0xbc5e5ee2U, 0x160b0b1dU, 0xaddbdb76U,
    0xdbe0e03bU, 0x64323256U, 0x743a3a4eU, 0x140a0a1eU,
    0x924949dbU, 0x0c06060aU, 0x4824246cU, 0xb85c5ce4U,
    0x9fc2c25dU, 0xbdd3d36eU, 0x43acacefU, 0xc46262a6U,
    0x399191a8U, 0x319595a4U, 0xd3e4e437U, 0xf279798bU,
    0xd5e7e732U, 0x8bc8c843U, 0x6e373759U, 0xda6d6db7U,
    0x018d8d8cU, 0xb1d5d564U, 0x9c4e4ed2U, 0x49a9a9e0U,
    0xd86c6cb4U, 0xac5656faU, 0xf3f4f407U, 0xcfeaea25U,
    0xca6565afU, 0xf47a7a8eU, 0x47aeaee9U, 0x10080818U,
    0x6fbabad5U, 0xf0787888U, 0x4a25256fU, 0x5c2e2e72U,
    0x381c1c24U, 0x57a6a6f1U, 0x73b4b4c7U, 0x97c6c651U,
    0xcbe8e823U, 0xa1dddd7cU, 0xe874749cU, 0x3e1f1f21U,
    0x964b4bddU, 0x61bdbddcU, 0x0d8b8b86U, 0x0f8a8a85U,
    0xe0707090U, 0x7c3e3e42U, 0x71b5b5c4U, 0xcc6666aaU,
    0x904848d8U, 0x06030305U, 0xf7f6f601U, 0x1c0e0e12U,
    0xc26161a3U, 0x6a35355fU, 0xae5757f9U, 0x69b9b9d0U,
    0x17868691U, 0x99c1c158U, 0x3a1d1d27U, 0x279e9eb9U,
    0xd9e1e138U, 0xebf8f813U, 0x2b9898b3U, 0x22111133U,
    0xd26969bbU, 0xa9d9d970U, 0x078e8e89U, 0x339494a7U,
    0x2d9b9bb6U, 0x3c1e1e22U, 0x15878792U, 0xc9e9e920U,
    0x87cece49U, 0xaa5555ffU, 0x50282878U, 0xa5dfdf7aU,
    0x038c8c8fU, 0x59a1a1f8U, 0x09898980U, 0x1a0d0d17U,
    0x65bfbfdaU, 0xd7e6e631U, 0x844242c6U, 0xd06868b8U,
    0x824141c3U, 0x299999b0U, 0x5a2d2d77U, 0x1e0f0f11U,
    0x7bb0b0cbU, 0xa85454fcU, 0x6dbbbbd6U, 0x2c16163aU,
};


typedef struct {
    u8 used;
    u8 *aes_ctx;
} aes_extend_alloc_ctx;

u8 aes_ctx_extend_buf[AES_PRIV_SIZE];

volatile aes_extend_alloc_ctx aes_extend_ctx = {.used = 0, .aes_ctx = &aes_ctx_extend_buf[0]};

void *aes_extend_malloc(u32 size)
{

    if (size > AES_PRIV_SIZE) {
        __dbg_printf("aes_extend_malloc fail!\r\n");
        return NULL;
    } else {
        if (aes_extend_ctx.used == 0) {
            aes_extend_ctx.used = 1;
            rtlc_memset(aes_extend_ctx.aes_ctx, 0, AES_PRIV_SIZE);
            //__dbg_printf("alloc aes_extend_ctx success\r\n");
            return (aes_extend_ctx.aes_ctx);
        } else {
            __dbg_printf("aes_extend_ctx be used,alloc fail!\r\n");
            return NULL;
        }
    }

#if 0
    if (size > AES_PRIV_SIZE) {
        __dbg_printf("aes_extend_malloc fail!\r\n");
        return NULL;
    } else {
        if (aes_extend_ctx.used == 0) {
            rtlc_memset(aes_extend_ctx.aes_ctx_buf, 0, AES_PRIV_SIZE);
            aes_extend_ctx.used = 1;
            __dbg_printf("alloc aes_extend_ctx success\r\n");
            return (aes_extend_ctx.aes_ctx_buf);
        } else {
            __dbg_printf("aes_extend_ctx be used,alloc fail!\r\n");
            return NULL;
        }
    }
#endif
}

void aes_extend_free(void *ctx)
{

    if (ctx == NULL) {
        return;
    }
    aes_extend_ctx.used = 0;
    //__dbg_printf("free aes_extend_ctx success\r\n");
}
void aes_gctr(void *aes, const u8 *icb, const u8 *x, u32 xlen, u8 *y);

/**
 * Expand the cipher key s32o the encryption key schedule.
 *
 * @return  the number of rounds for the given cipher key size.
 */
s32 rijndaelKeySetupEnc(u32 rk[], const u8 cipherKey[], s32 keyBits)
{
    s32 i;
    u32 temp;

    rk[0] = GETU32(cipherKey);
    rk[1] = GETU32(cipherKey +  4);
    rk[2] = GETU32(cipherKey +  8);
    rk[3] = GETU32(cipherKey + 12);

    if (keyBits == 128) {
        for (i = 0; i < 10; i++) {
            temp  = rk[3];
            rk[4] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
                    TE443(temp) ^ TE414(temp) ^ RCON(i);
            rk[5] = rk[1] ^ rk[4];
            rk[6] = rk[2] ^ rk[5];
            rk[7] = rk[3] ^ rk[6];
            rk += 4;
        }
        return 10;
    }

    rk[4] = GETU32(cipherKey + 16);
    rk[5] = GETU32(cipherKey + 20);

    if (keyBits == 192) {
        for (i = 0; i < 8; i++) {
            temp  = rk[5];
            rk[6] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
                    TE443(temp) ^ TE414(temp) ^ RCON(i);
            rk[7] = rk[1] ^ rk[6];
            rk[8] = rk[2] ^ rk[7];
            rk[9] = rk[3] ^ rk[8];
            if (i == 7)
                return 12;
            rk[10] = rk[4] ^ rk[9];
            rk[11] = rk[5] ^ rk[10];
            rk += 6;
        }
    }

    rk[6] = GETU32(cipherKey + 24);
    rk[7] = GETU32(cipherKey + 28);

    if (keyBits == 256) {
        for (i = 0; i < 7; i++) {
            temp  = rk[7];
            rk[8] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
                    TE443(temp) ^ TE414(temp) ^ RCON(i);
            rk[9] = rk[1] ^ rk[8];
            rk[10] = rk[2] ^ rk[9];
            rk[11] = rk[3] ^ rk[10];
            if (i == 6)
                return 14;
            temp  = rk[11];
            rk[12] = rk[4] ^ TE411(temp) ^ TE422(temp) ^
                     TE433(temp) ^ TE444(temp);
            rk[13] = rk[5] ^ rk[12];
            rk[14] = rk[6] ^ rk[13];
            rk[15] = rk[7] ^ rk[14];
            rk += 8;
        }
    }

    return -1;
}


void rijndaelEncrypt(const u32 rk[], s32 Nr, const u8 pt[16], u8 ct[16])
{
    u32 s0, s1, s2, s3, t0, t1, t2, t3;
#ifndef FULL_UNROLL
    s32 r;
#endif /* ?FULL_UNROLL */

    /*
     * map byte array block to cipher state
     * and add initial round key:
     */
    s0 = GETU32(pt) ^ rk[0];
    s1 = GETU32(pt +  4) ^ rk[1];
    s2 = GETU32(pt +  8) ^ rk[2];
    s3 = GETU32(pt + 12) ^ rk[3];

#define _ROUND(i,d,s) \
d##0 = TE0(s##0) ^ TE1(s##1) ^ TE2(s##2) ^ TE3(s##3) ^ rk[4 * i]; \
d##1 = TE0(s##1) ^ TE1(s##2) ^ TE2(s##3) ^ TE3(s##0) ^ rk[4 * i + 1]; \
d##2 = TE0(s##2) ^ TE1(s##3) ^ TE2(s##0) ^ TE3(s##1) ^ rk[4 * i + 2]; \
d##3 = TE0(s##3) ^ TE1(s##0) ^ TE2(s##1) ^ TE3(s##2) ^ rk[4 * i + 3]

#ifdef FULL_UNROLL

    _ROUND(1, t, s);
    _ROUND(2, s, t);
    _ROUND(3, t, s);
    _ROUND(4, s, t);
    _ROUND(5, t, s);
    _ROUND(6, s, t);
    _ROUND(7, t, s);
    _ROUND(8, s, t);
    _ROUND(9, t, s);
    if (Nr > 10) {
        _ROUND(10, s, t);
        _ROUND(11, t, s);
        if (Nr > 12) {
            _ROUND(12, s, t);
            _ROUND(13, t, s);
        }
    }

    rk += Nr << 2;

#else  /* !FULL_UNROLL */

    /* Nr - 1 full rounds: */
    r = Nr >> 1;
    for (;;) {
        _ROUND(1, t, s);
        rk += 8;
        if (--r == 0)
            break;
        _ROUND(0, s, t);
    }

#endif /* ?FULL_UNROLL */

#undef _ROUND

    /*
     * apply last round and
     * map cipher state to byte array block:
     */
    s0 = TE41(t0) ^ TE42(t1) ^ TE43(t2) ^ TE44(t3) ^ rk[0];
    PUTU32(ct, s0);
    s1 = TE41(t1) ^ TE42(t2) ^ TE43(t3) ^ TE44(t0) ^ rk[1];
    PUTU32(ct +  4, s1);
    s2 = TE41(t2) ^ TE42(t3) ^ TE43(t0) ^ TE44(t1) ^ rk[2];
    PUTU32(ct +  8, s2);
    s3 = TE41(t3) ^ TE42(t0) ^ TE43(t1) ^ TE44(t2) ^ rk[3];
    PUTU32(ct + 12, s3);
}


void *aes_encrypt_init(const u8 *key, u32 len)
{
    u32 *rk;
    s32 res;
    rk = aes_extend_malloc(AES_PRIV_SIZE);
    if (rk == NULL)
        return NULL;
    res = rijndaelKeySetupEnc(rk, key, len * 8);
    if (res < 0) {
        aes_extend_free(rk);
        return NULL;
    }
    rk[AES_PRIV_NR_POS] = res;
    return rk;
}


void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
    u32 *rk = ctx;
    rijndaelEncrypt(ctx, rk[AES_PRIV_NR_POS], plain, crypt);
}


void aes_encrypt_deinit(void *ctx)
{
    rtlc_memset(ctx, 0, AES_PRIV_SIZE);
    aes_extend_free(ctx);
}


void xor_block(u8 *dst, const u8 *src)
{
    u32 *d = (u32 *) dst;
    u32 *s = (u32 *) src;
    *d++ ^= *s++;
    *d++ ^= *s++;
    *d++ ^= *s++;
    *d++ ^= *s++;
}


void shift_right_block(u8 *v)
{
    u32 val;

    val = WPA_GET_BE32(v + 12);
    val >>= 1;
    if (v[11] & 0x01)
        val |= 0x80000000;
    WPA_PUT_BE32(v + 12, val);

    val = WPA_GET_BE32(v + 8);
    val >>= 1;
    if (v[7] & 0x01)
        val |= 0x80000000;
    WPA_PUT_BE32(v + 8, val);

    val = WPA_GET_BE32(v + 4);
    val >>= 1;
    if (v[3] & 0x01)
        val |= 0x80000000;
    WPA_PUT_BE32(v + 4, val);

    val = WPA_GET_BE32(v);
    val >>= 1;
    WPA_PUT_BE32(v, val);
}


/* Multiplication in GF(2^128) */
void gf_mult(const u8 *x, const u8 *y, u8 *z)
{
    u8 v[16];
    s32 i, j;

    rtlc_memset(z, 0, 16); /* Z_0 = 0^128 */
    rtlc_memcpy(v, y, 16); /* V_0 = Y */

    for (i = 0; i < 16; i++) {
        for (j = 0; j < 8; j++) {
            if (x[i] & BIT(7 - j)) {
                /* Z_(i + 1) = Z_i XOR V_i */
                xor_block(z, v);
            } else {
                /* Z_(i + 1) = Z_i */
            }

            if (v[15] & 0x01) {
                /* V_(i + 1) = (V_i >> 1) XOR R */
                shift_right_block(v);
                /* R = 11100001 || 0^120 */
                v[0] ^= 0xe1;
            } else {
                /* V_(i + 1) = V_i >> 1 */
                shift_right_block(v);
            }
        }
    }
}


void ghash_start(u8 *y)
{
    /* Y_0 = 0^128 */
    rtlc_memset(y, 0, 16);
}


static void ghash(const u8 *h, const u8 *x, u32 xlen, u8 *y)
{
    u32 m, i;
    const u8 *xpos = x;
    u8 tmp[16];

    m = xlen / 16;

    for (i = 0; i < m; i++) {
        /* Y_i = (Y^(i-1) XOR X_i) dot H */
        xor_block(y, xpos);
        xpos += 16;

        /* dot operation:
         * multiplication operation for binary Galois (finite) field of
         * 2^128 elements */
        gf_mult(y, h, tmp);
        rtlc_memcpy(y, tmp, 16);
    }

    if (x + xlen > xpos) {
        /* Add zero padded last block */
        u32 last = x + xlen - xpos;
        rtlc_memcpy(tmp, xpos, last);
        rtlc_memset(tmp + last, 0, sizeof(tmp) - last);

        /* Y_i = (Y^(i-1) XOR X_i) dot H */
        xor_block(y, tmp);

        /* dot operation:
         * multiplication operation for binary Galois (finite) field of
         * 2^128 elements */
        gf_mult(y, h, tmp);
        rtlc_memcpy(y, tmp, 16);
    }

    /* Return Y_m */
}


void *aes_gcm_init_hash_subkey(const u8 *key, u32 key_len, u8 *H)
{
    void *aes;

    aes = aes_encrypt_init(key, key_len);
    if (aes == NULL) {
        return NULL;
    }
    /* Generate hash subkey H = AES_K(0^128) */
    rtlc_memset(H, 0, AES_BLOCK_SIZE);
    aes_encrypt(aes, H, H);
    /*
    wpa_hexdump_key(MSG_EXCESSIVE, "Hash subkey H for GHASH",
            H, AES_BLOCK_SIZE);
    */
    //__crypto_mem_dump(H,AES_BLOCK_SIZE,"Hash subkey H for GHASH");
    return aes;
}
#if 0
void aes_gcm_ghash(const u8 *H, const u8 *aad, u32 aad_len,
                   const u8 *crypt, u32 crypt_len, u8 *S)
{
    u8 len_buf[16];

    /*
     * u = 128 * ceil[len(C)/128] - len(C)
     * v = 128 * ceil[len(A)/128] - len(A)
     * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]64 || [len(C)]64)
     * (i.e., zero padded to block size A || C and lengths of each in bits)
     */
    ghash_start(S);
    ghash(H, aad, aad_len, S);
    ghash(H, crypt, crypt_len, S);
    WPA_PUT_BE64(len_buf, aad_len * 8);
    WPA_PUT_BE64(len_buf + 8, crypt_len * 8);
    ghash(H, len_buf, sizeof(len_buf), S);

    //wpa_hexdump_key(MSG_EXCESSIVE, "S = GHASH_H(...)", S, 16);
    __crypto_mem_dump(S, 16, "S = GHASH_H(...)");
}
#endif

void aes_ghash(const u8 *H, const u8 *crypt, u32 crypt_len, u8 *S)
{

    /*
     * u = 128 * ceil[len(C)/128] - len(C)
     * v = 128 * ceil[len(A)/128] - len(A)
     * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]64 || [len(C)]64)
     * (i.e., zero padded to block size A || C and lengths of each in bits)
     */
    ghash_start(S);
    ghash(H, crypt, crypt_len, S);
    //wpa_hexdump_key(MSG_EXCESSIVE, "S = GHASH_H(...)", S, 16);
    //__crypto_mem_dump(S,16,"S = GHASH_H(...)");
}

s32 aes_ghash_swvrf(const u8 *key, u32 key_len, const u8 *plain, u32 plain_len, u8 *tag)
{
    u8 H[AES_BLOCK_SIZE];
    u8 S[16];
    void *aes;
    if (AES_BLOCK_SIZE != key_len) {
        return -1;
    }
    aes = aes_gcm_init_hash_subkey(key, key_len, H);
    if (aes == NULL) {
        return -1;
    }
    aes_ghash(H, plain, plain_len, S);
    aes_encrypt_deinit(aes);
    rtlc_memcpy(tag, S, AES_BLOCK_SIZE);

    return 0;
}

void aes_gcm_ghash(const u8 *H, const u8 *aad, u32 aad_len,
                   const u8 *crypt, u32 crypt_len, u8 *S)
{
    u8 len_buf[16];

    /*
     * u = 128 * ceil[len(C)/128] - len(C)
     * v = 128 * ceil[len(A)/128] - len(A)
     * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]64 || [len(C)]64)
     * (i.e., zero padded to block size A || C and lengths of each in bits)
     */
    ghash_start(S);
    ghash(H, aad, aad_len, S);
    ghash(H, crypt, crypt_len, S);
    WPA_PUT_BE64(len_buf, aad_len * 8);
    WPA_PUT_BE64(len_buf + 8, crypt_len * 8);
    ghash(H, len_buf, sizeof(len_buf), S);

    //wpa_hexdump_key(MSG_EXCESSIVE, "S = GHASH_H(...)", S, 16);
    //__crypto_mem_dump(S,16,"S = GHASH_H(...)");
}

void aes_gcm_prepare_j0(const u8 *iv, u32 iv_len, const u8 *H, u8 *J0)
{
    //u8 len_buf[16];

    if (iv_len == 12) {
        /* Prepare block J_0 = IV || 0^31 || 1 [len(IV) = 96] */
        rtlc_memcpy(J0, iv, iv_len);
        rtlc_memset(J0 + iv_len, 0, AES_BLOCK_SIZE - iv_len);
        J0[AES_BLOCK_SIZE - 1] = 0x01;
    }
#if 0
    else {
        /*
         * s = 128 * ceil(len(IV)/128) - len(IV)
         * J_0 = GHASH_H(IV || 0^(s+64) || [len(IV)]_64)
         */
        ghash_start(J0);
        ghash(H, iv, iv_len, J0);
        WPA_PUT_BE64(len_buf, 0);
        WPA_PUT_BE64(len_buf + 8, iv_len * 8);
        ghash(H, len_buf, sizeof(len_buf), J0);
    }
#endif
}


s32 aes_gmac_swvrf(const u8 *key, u32 key_len, const u8 *iv,
                   const u8 *plain, u32 plain_len,
                   const u8 *aad, u32 aad_len, u8 *tag)
{

    u8 H[AES_BLOCK_SIZE];
    u8 J0[AES_BLOCK_SIZE];
    u8 S[16];
    void *aes;

    aes = aes_gcm_init_hash_subkey(key, key_len, H);
    if (aes == NULL) {
        return -1;
    }
    aes_gcm_prepare_j0(iv, 12, H, J0);
    aes_gcm_ghash(H, aad, aad_len, plain, plain_len, S);
    /* T = MSB_t(GCTR_K(J_0, S)) */
    aes_gctr(aes, J0, S, sizeof(S), tag);
    aes_encrypt_deinit(aes);
    //__crypto_mem_dump(tag, AES_BLOCK_SIZE, "sw gmac tag: ");

    return 0;
}


void *aes_gctr_init_key(const u8 *key, u32 key_len)
{
    void *aes;

    aes = aes_encrypt_init(key, key_len);
    if (aes == NULL) {
        return NULL;
    }

    return aes;
}

void aes_gctr_prepare_nonce(const u8 *iv, u32 iv_len, u8 *J0)
{
    //u8 len_buf[16];

    if (iv_len == 12) {
        /* Prepare block J_0 = IV || 0^31 || 1 [len(IV) = 96] */
        rtlc_memcpy(J0, iv, iv_len);
        rtlc_memset(J0 + iv_len, 0, AES_BLOCK_SIZE - iv_len);
        J0[AES_BLOCK_SIZE - 1] = 0x01;
    }
#if 0
    else { // if iv_len > 12 condition
        /*
         * s = 128 * ceil(len(IV)/128) - len(IV)
         * J_0 = GHASH_H(IV || 0^(s+64) || [len(IV)]_64)
         */
        ghash_start(J0);
        ghash(H, iv, iv_len, J0);
        WPA_PUT_BE64(len_buf, 0);
        WPA_PUT_BE64(len_buf + 8, iv_len * 8);
        ghash(H, len_buf, sizeof(len_buf), J0);
    }
#endif
}

void inc32(u8 *block)
{
    u32 val;
    val = WPA_GET_BE32(block + AES_BLOCK_SIZE - 4);
    val++;
    WPA_PUT_BE32(block + AES_BLOCK_SIZE - 4, val);
}

void aes_gctr(void *aes, const u8 *icb, const u8 *x, u32 xlen, u8 *y)
{
    u32 i, n, last;
    u8 cb[AES_BLOCK_SIZE], tmp[AES_BLOCK_SIZE];
    const u8 *xpos = x;
    u8 *ypos = y;

    if (xlen == 0) {
        return;
    }

    n = xlen / 16;

    rtlc_memcpy(cb, icb, AES_BLOCK_SIZE);
    /* Full blocks */
    for (i = 0; i < n; i++) {
        aes_encrypt(aes, cb, ypos);
        xor_block(ypos, xpos);
        xpos += AES_BLOCK_SIZE;
        ypos += AES_BLOCK_SIZE;
        inc32(cb);
    }

    last = x + xlen - xpos;
    if (last) {
        /* Last, partial block */
        aes_encrypt(aes, cb, tmp);
        for (i = 0; i < last; i++)
            *ypos++ = *xpos++ ^ tmp[i];
    }
}

s32 aes_gctr_swvrf(const u8 *key, u32 key_len, const u8 *iv,
                   const u8 *plain, u32 plain_len, u8 *crypt)
{
    u8 J0[AES_BLOCK_SIZE];
    void *aes;

    aes = aes_gctr_init_key(key, key_len);
    if (aes == NULL) {
        return -1;
    }
    aes_gctr_prepare_nonce(iv, 12, J0);
    inc32(J0);
    /* C = GCTR_K(inc_32(J_0), P) */
    aes_gctr(aes, J0, plain, plain_len, crypt);

    aes_encrypt_deinit(aes);
    //__crypto_mem_dump(crypt, (plain_len+4), "socTest pcie_8852a 3 0 1 128584sw gctr msg: ");
    return 0;
}


#if 0
s32 aes_gcm_ae(const u8 *key, u32 key_len, const u8 *iv, u32 iv_len,
               const u8 *plain, u32 plain_len,
               const u8 *aad, u32 aad_len, u8 *crypt, u8 *tag)
{
    u8 H[AES_BLOCK_SIZE];
    u8 J0[AES_BLOCK_SIZE];
    u8 S[16];
    void *aes;

    aes = aes_gcm_init_hash_subkey(key, key_len, H);
    if (aes == NULL) {
        return -1;
    }
    //aes_gcm_ghash(H, aad, aad_len, crypt, plain_len, S);
    aes_gcm_ghash(H, aad, aad_len, plain, plain_len, S);
    /* Return (C, T) */

    aes_encrypt_deinit(aes);

    return 0;
}
#endif