/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include <asm/mipsregs.h>

#define  CAUSEB_DC              27
#define  CAUSEF_DC              (_ULCAST_(1)   << 27)

typedef unsigned int u32;

//test crypto interrupt.

//0:8DW, ~ 3:20DW
#define CRYPTO_DDLEN 3
//0~2
#define CRYPTO_BRUST 2


#define CRYPTO_REG_BASE			0xB800C000 // 8196f ap soc
#define REG_IPSSDAR                     (0x00)  // IPSec Source Descriptor Starting Address Register
#define REG_IPSDDAR                     (0x04)  // IPSec Destination Descriptor Starting Address Register
#define REG_IPSCSR                      (0x08)  // IPSec Command/Status Register
#define REG_IPSCTR                      (0x0C)  // IPSec Control Register

/* IPSec Command/Status Register */
#define IPS_SDUEIP              (1<<15)                         /* Source Descriptor Unavailable Error Interrupt Pending */
#define IPS_SDLEIP              (1<<14)                         /* Source Descriptor Length Error Interrupt Pending */ 
#define IPS_DDUEIP              (1<<13)                         /* Destination Descriptor Unavailable Error Interrupt Pending */
#define IPS_DDOKIP              (1<<12)                         /* Destination Descriptor OK Interrupt Pending */ 
#define IPS_DABFIP              (1<<11)                         /* Data Address Buffer Interrupt Pending */
#define IPS_POLL                (1<<1)                          /* Descriptor Polling. Set 1 to kick crypto engine to fetch source descriptor. */
#define IPS_SRST                (1<<0)                          /* Software reset, write 1 to reset */ 

/* IPSec Control Register */
#define IPS_DDLEN_20    (3<<24)                         /* Destination Descriptor Length : Length is 20*DW : for SHA-1/MD5 */
#define IPS_DDLEN_16    (2<<24)                         /* Destination Descriptor Length : Length is 16*DW : for SHA-1/MD5 */
#define IPS_DDLEN_12    (1<<24)                         /* Destination Descriptor Length : Length is 12*DW : for SHA-1/MD5 */
#define IPS_DDLEN_8             (0<<24)                         /* Destination Descriptor Length : Length is 8*DW : for SHA-1/MD5 */

#define IPS_SDUEIE              (1<<15)                         /* Source Descriptor Unavailable Error Interrupt Enable */
#define IPS_SDLEIE              (1<<14)                         /* Source Descriptor Length Error Interrupt Enable */
#define IPS_DDUEIE              (1<<13)                         /* Destination Descriptor Unavailable Error Interrupt Enable */                     
#define IPS_DDOKIE              (1<<12)                         /* Destination Descriptor OK Interrupt Enable */
#define IPS_DABFIE              (1<<11)                         /* Data Address Buffer Interrupt Enable */


#define IPS_LXLITTLE            (1<<10)                         /* Lx bus data endian*/
#define IPS_LBKM                (1<<8)                          /* Loopback mode enable */
#define IPS_SAWB                (1<<7)                          /* Source Address Write Back */
#define IPS_CKE                 (1<<6)                          /* Clock enable */
#define IPS_DMBS_16             (0x0<<3)                        /* 16 Bytes */
#define IPS_DMBS_32             (0x1<<3)                        /* 32 Bytes */
#define IPS_DMBS_64             (0x2<<3)                        /* 64 Bytes */
#define IPS_SMBS_MASK   (0x7<<0)                        /* Mask for SourceDMA Maximum Burst Size */ 
#define IPS_SMBS_16             (0x0<<0)                        /* 16 Bytes */
#define IPS_SMBS_32             (0x1<<0)                        /* 32 Bytes */
#define IPS_SMBS_64             (0x2<<0)                        /* 64 Bytes */




#define CRYPTO_IRQ     (20)

#define HAL_CRYPTO_READ32(addr)         (*((volatile unsigned int*)(CRYPTO_REG_BASE + addr)))
#define HAL_CRYPTO_WRITE32(addr, value) ((*((volatile unsigned int*)(CRYPTO_REG_BASE + addr))) = value)

// address macro
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define CPHYSADDR(a)        ((int)(a) & 0x1fffffff)
#define CKSEG0ADDR(a)       (CPHYSADDR(a) | KSEG0)
#define CKSEG1ADDR(a)       (CPHYSADDR(a) | KSEG1)
#define CKSEG2ADDR(a)       (CPHYSADDR(a) | KSEG2)
#define CKSEG3ADDR(a)       (CPHYSADDR(a) | KSEG3)

#define PHYSICAL_ADDRESS(x) CPHYSADDR(x)
#define UNCACHED_ADDRESS(x) CKSEG1ADDR(x)
#define CACHED_ADDRESS(x)       CKSEG0ADDR(x)


__attribute__ ((aligned(32)))
unsigned int crypto_src_buf[16][4];
__attribute__ ((aligned(32)))
unsigned int crypto_dst_buf[16][8+CRYPTO_DDLEN];


static const unsigned char _sha1_plaintext[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

static const unsigned char _sha1_padding[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

static const unsigned char _sha2_plaintext[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                                     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};

static const unsigned char _sha2_padding[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};


// 128 bit - CBC
__attribute__ ((aligned (4)))
static const unsigned char _aes_test_key[16] =
{                                
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c 
} ;                                                                                                          

//write 4 dummy bytes. test vector from amiba neo jou (jwsyu)
static inline 
void write_memory(u32 addr, u32 value)
{
        *((volatile u32*)(addr)) = value;

}

static const unsigned char test_key[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

// set key
#if 0
static const unsigned char src_desc1[] = {
        0xa3, 0xa1, 0x80, 0x10, 0x00, 0x00, 0x00, 0x10, 
        0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x82, 0x64
};
#else
static const unsigned int src_desc1[] = {
        0xa3a18010, 0x00000010, 
        0x00000000, 0x00008264
};

#endif


// set iv
#if 0
static const unsigned char src_desc2[] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#else
static const unsigned int src_desc2[] = {
        0x80000000, 0x00000000, 
        0x00000000, 0x00000000
};

#endif
// set message
#if 0
static const unsigned char src_desc3[] = {
        0x80, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x71, 0x68
};
#else
static const unsigned char src_desc3[] = {
        0x80000010, 0x00000000, 
        0x00000000, 0x00007168
};
#endif

static const unsigned char src_desc_none[] = {
        0x00000000, 0x00000000, 
        0x00000000, 0x00000000
};
static const unsigned char test_msg[] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

// dest descriptor
#if 0
static const unsigned char dst_desc1[] = {
        0x80, 0x00, 0x00, 0x10, 0x10, 0x01, 0xb5, 0x40, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#else
static const unsigned int dst_desc1[] = {
        0x80000010, 0x0001b540, 
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
};

#endif
static const unsigned int dst_desc_none[] = {
        0x00000000, 0x00000000, 
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
};



void issue_test1(void)
{
        printf("issue Test1 : all register setting \r\n");
        
//    write_memory(0x40000210, 0x00011107);
//    write_memory(0x40000210, 0x00111107);
//        write_memory(0x40000244, 0x00000010);
//        write_memory(0x40000244, 0x00000030);

        write_memory(0xB800C008, 0x1);
        printf("After reset\r\n");

        write_memory(0xB800C000, 0x00016b88);
        write_memory(0xB800C004, 0x00016c10);
        write_memory(0xB800C00c, 0x00000040);
        write_memory(0xB800C008, 0x0000f000);
        // debug port
//        write_memory(0x400700a0, 0x00000001);
//        write_memory(0x4007002c, 0x00fde404);


        // set result to 0xa5 first, write 24 bytes 
        rtl_memsetb(0xa001b540, 0xa5, 24);


        // set key
        memcpy(0xa0008264, test_key, sizeof(test_key));
        memcpy(0xa0007168, test_msg, sizeof(test_msg));
        
        // source descriptor
        memcpy(0xa0016b88, src_desc1, sizeof(src_desc1));
        memcpy(0xa0016b98, src_desc2, sizeof(src_desc2));
        memcpy(0xa0016ba8, src_desc3, sizeof(src_desc3));
        memcpy(0xa0016ba8, src_desc3, sizeof(src_desc3));
        memcpy(0xa0016bb8, src_desc_none, sizeof(src_desc_none));
        memcpy(0xa0016bb8, src_desc_none, sizeof(src_desc_none));
	*((volatile u32*)(0xa0016ba8)) = 0x80000010;
	*((volatile u32*)(0xa0016bac)) = 0x0;
	*((volatile u32*)(0xa0016bb0)) = 0x0;
	*((volatile u32*)(0xa0016bb4)) = 0x00007168;
	*((volatile u32*)(0xa0016bb8)) = 0x0;
	*((volatile u32*)(0xa0016bbc)) = 0x0;
	*((volatile u32*)(0xa0016bc0)) = 0x0;
	*((volatile u32*)(0xa0016bc4)) = 0x0;

        // dest descriptor
        memcpy(0xa0016c10, dst_desc1, sizeof(dst_desc1));
        memcpy(0xa0016c40, dst_desc_none, sizeof(dst_desc_none));
        memcpy(0xa0016c10, dst_desc1, sizeof(dst_desc1));

        // kick off
     //   write_memory(0xb800C008, 0x0000f800);
     //   write_memory(0xb800C008, 0x00000002);
        
        printf("end\r\n");
}


__attribute__ ((aligned (4))) 
static const unsigned char _aes_test_iv_1[16] = 
{  
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,                                      
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F                                        
};                                                                                          
                                                                                              
            
__attribute__ ((aligned (4)))                                                                
static const unsigned char _aes_test_buf[16384] =                                               
{                                                                                             
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,                                        
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,                                       
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a                                        
}; 

__attribute__ ((aligned (32)))                                                                
static unsigned char _aes_test_out_buf[320] =                                               
{                                                                                             
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                        
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                       
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                                        
}; 
//#define ISR_LESS_PRINT 1
void Crypto_ISR(void *data)
{
	unsigned int wtemp;


	wtemp = HAL_CRYPTO_READ32(REG_IPSCSR);
#ifndef ISR_LESS_PRINT
	printf("REG_IPSCSR=%x", wtemp);
#endif
	if (wtemp& IPS_POLL) {
		printf("polling.");
		wtemp = HAL_CRYPTO_READ32(REG_IPSCSR);
		printf("REG_IPSCSR=%x", wtemp);
		if (wtemp& IPS_POLL) {
			printf("polling2.");
		}
	}
	//clear interrupt as soon as possiblea
	//only clear interrupt pending event, not IPS_POLL, IPS_SRST
	wtemp &= (IPS_SDUEIP|IPS_SDLEIP|IPS_DDUEIP|IPS_DDOKIP|IPS_DABFIP); 
	HAL_CRYPTO_WRITE32(REG_IPSCSR, wtemp);
	if (wtemp&IPS_SDUEIP) {
		printf("INT:(IPS_SDUEIP)");
	}
	if (wtemp&IPS_SDLEIP) {
		printf("INT:(IPS_SDLEIP)");
	}
	if (wtemp&IPS_DDUEIP) {
		printf("INT:(IPS_DDUEIP)");
	}
	if (wtemp&IPS_DDOKIP) {
#ifndef ISR_LESS_PRINT
		printf("INT:(IPS_DDOKIP)");
#else
		printf("Oo");
#endif
	}
	if (wtemp&IPS_DABFIP) {
		printf("INT:(IPS_DABFIP)");
	}
	//HAL_CRYPTO_WRITE32(REG_IPSCSR, wtemp);

		printf("\n");
}



void ipsec_crypto_dma(void)
{
	
	unsigned int burstSize;
	unsigned int DDLEN_setting;
	int i, j;
	unsigned int *pcrypto_src_buf=(unsigned int*) UNCACHED_ADDRESS(crypto_src_buf);
	unsigned int *pcrypto_dst_buf=(unsigned int*) UNCACHED_ADDRESS(crypto_dst_buf);

	(*(volatile unsigned int*) 0xb8000050) |= 4;

	// Software Reset 
	HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SRST);

	printf(" DDLEN=%d, BRUST=%d", CRYPTO_DDLEN, CRYPTO_BRUST);
#if (CRYPTO_BRUST == 0)
	burstSize = IPS_DMBS_16 | IPS_SMBS_16;
#elif (CRYPTO_BRUST == 1)
	burstSize = IPS_DMBS_32 | IPS_SMBS_32;
#elif (CRYPTO_BRUST == 2)
	burstSize = IPS_DMBS_64 | IPS_SMBS_64;
#endif
#if (CRYPTO_DDLEN == 0)
	DDLEN_setting = IPS_DDLEN_8;
#elif (CRYPTO_DDLEN == 1)
	DDLEN_setting = IPS_DDLEN_12;
#elif (CRYPTO_DDLEN == 2)
	DDLEN_setting = IPS_DDLEN_16;
#elif (CRYPTO_DDLEN == 3)
	DDLEN_setting = IPS_DDLEN_20;
#endif

	HAL_CRYPTO_WRITE32(REG_IPSCTR, 0 | IPS_LXLITTLE | IPS_CKE | burstSize | DDLEN_setting );
	//write 1 to clear
	HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);
printf("(%s-%d)REG_IPSCTR=%x\n", __func__, __LINE__, HAL_CRYPTO_READ32(REG_IPSCTR));

	HAL_CRYPTO_WRITE32(REG_IPSSDAR, PHYSICAL_ADDRESS(&crypto_src_buf[0][0]));
	HAL_CRYPTO_WRITE32(REG_IPSDDAR, PHYSICAL_ADDRESS(&crypto_dst_buf[0][0]));

		printf("test AES IPS_DDOKIE\n");
		crypto_src_buf[0][0]=0xa3a80010;
		crypto_src_buf[0][1]=0x10;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x80000010;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_buf);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000010;
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;

flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
		        _aes_test_out_buf[7]);
		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
		        _aes_test_out_buf[15]);
	
	
}







void Start_crypto_int(int interrupt)
{
	unsigned int burstSize;
	unsigned int DDLEN_setting;
	int i, j;
	unsigned int *pcrypto_src_buf=(unsigned int*) UNCACHED_ADDRESS(crypto_src_buf);
	unsigned int *pcrypto_dst_buf=(unsigned int*) UNCACHED_ADDRESS(crypto_dst_buf);

	(*(volatile unsigned int*) 0xb8000050) |= 4;

	// Software Reset 
	HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SRST);

	printf("interrupt test=%d, DDLEN=%d, BRUST=%d",interrupt, CRYPTO_DDLEN, CRYPTO_BRUST);
#if (CRYPTO_BRUST == 0)
	burstSize = IPS_DMBS_16 | IPS_SMBS_16;
#elif (CRYPTO_BRUST == 1)
	burstSize = IPS_DMBS_32 | IPS_SMBS_32;
#elif (CRYPTO_BRUST == 2)
	burstSize = IPS_DMBS_64 | IPS_SMBS_64;
#endif
#if (CRYPTO_DDLEN == 0)
	DDLEN_setting = IPS_DDLEN_8;
#elif (CRYPTO_DDLEN == 1)
	DDLEN_setting = IPS_DDLEN_12;
#elif (CRYPTO_DDLEN == 2)
	DDLEN_setting = IPS_DDLEN_16;
#elif (CRYPTO_DDLEN == 3)
	DDLEN_setting = IPS_DDLEN_20;
#endif

	HAL_CRYPTO_WRITE32(REG_IPSCTR, 0 | IPS_LXLITTLE | IPS_CKE | burstSize | DDLEN_setting );
	//write 1 to clear
	HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);
printf("(%s-%d)REG_IPSCTR=%x\n", __func__, __LINE__, HAL_CRYPTO_READ32(REG_IPSCTR));

	irq_install_handler(CRYPTO_IRQ, Crypto_ISR, 0);
//printf("(%s-%d)\n", __func__, __LINE__);

	//enable interrupt mask
	HAL_CRYPTO_WRITE32(REG_IPSCTR, HAL_CRYPTO_READ32(REG_IPSCTR) | IPS_SDUEIE | IPS_SDLEIE | IPS_DDUEIE | IPS_DDOKIE | IPS_DABFIE );
//printf("(%s-%d)\n", __func__, __LINE__);

	HAL_CRYPTO_WRITE32(REG_IPSSDAR, PHYSICAL_ADDRESS(&crypto_src_buf[0][0]));
	HAL_CRYPTO_WRITE32(REG_IPSDDAR, PHYSICAL_ADDRESS(&crypto_dst_buf[0][0]));
//printf("(%s-%d)\n", __func__, __LINE__);

	if (interrupt == 1) {
		printf("test IPS_SDUEIE ownbit\n");
		crypto_src_buf[0][0]=0x24008000; /* 0xa4000000 -> disable ownbit */
		crypto_src_buf[0][1]=0x38;
		crypto_src_buf[0][2]=0x80000;
		crypto_src_buf[0][3]=0x0;

		crypto_src_buf[1][0]=0x80000038;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[2][0]=0x80000008;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000000;
		crypto_dst_buf[0][1]=0x0;
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;
flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);
        
		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
	}

	if (interrupt == 2) {
		printf("test IPS_SDUEIE FS\n");
		crypto_src_buf[0][0]=0x84008000; /* 0xa4000000 -> disable FS */
		crypto_src_buf[0][1]=0x38;
		crypto_src_buf[0][2]=0x80000;
		crypto_src_buf[0][3]=0x0;

		crypto_src_buf[1][0]=0x80000038;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[2][0]=0x80000008;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000000;
		crypto_dst_buf[0][1]=0x0;
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
	}

	if (interrupt == 3) {
		printf("test IPS_SDLEIE\n");
		crypto_src_buf[0][0]=0xa4008000;
		crypto_src_buf[0][1]=0x38;
		crypto_src_buf[0][2]=0x80000;
		crypto_src_buf[0][3]=0x0;

		crypto_src_buf[1][0]=0x80000030; /* 0x80000038 -> change data length */
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[2][0]=0x80000008;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000000;
		crypto_dst_buf[0][1]=0x0;
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
	}

	if (interrupt == 4) {
		printf("test IPS_DDUEIE\n");
		crypto_src_buf[0][0]=0xa4008000;
		crypto_src_buf[0][1]=0x38;
		crypto_src_buf[0][2]=0x80000;
		crypto_src_buf[0][3]=0x0;

		crypto_src_buf[1][0]=0x80000038; /* 0x80000038 */
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[2][0]=0x80000008;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x00000000; /* 0x80000000 -> change own bit */
		crypto_dst_buf[0][1]=0x0;
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
	}

	if (interrupt == 5) {
		printf("test IPS_DDOKIE\n");
		crypto_src_buf[0][0]=0xa4008000;
		crypto_src_buf[0][1]=0x38;
		crypto_src_buf[0][2]=0x80000;
		crypto_src_buf[0][3]=0x0;

		crypto_src_buf[1][0]=0x80000038;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[2][0]=0x80000008;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000000;
		crypto_dst_buf[0][1]=0x0;
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);
        //printf("bus=%x.", (*((volatile unsigned int*)(0xb81450b0))));
		//int bus1 = (*((volatile unsigned int*)(0xb81450b0)));
		//int bus2 = (*((volatile unsigned int*)(0xb81450b0)));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);
		//bus2 = (*((volatile unsigned int*)(0xb81450b0)));
		//if (bus2 != bus1)
			//printf("bus=%x.", bus2);
		
		for (i=0 ; i<1024 ; i++) {
			
		//bus2 = (*((volatile unsigned int*)(0xb81450b0)));
		//if (bus2 != bus1)
		//	printf("bus=%x.", bus2);

			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x,", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
		printf("result2=%x, %x, %x, %x\n", crypto_dst_buf[0][4], crypto_dst_buf[0][5],
		                                crypto_dst_buf[0][6], crypto_dst_buf[0][7]);
	}

	if (interrupt == 6) {
		printf("test multi IPS_DDOKIE\n");
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x\n", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		crypto_src_buf[0][0]=0xa4008000;
		crypto_src_buf[0][1]=0x38;
		crypto_src_buf[0][2]=0x80000;
		crypto_src_buf[0][3]=0x0;

		crypto_src_buf[1][0]=0x80000038;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[2][0]=0x80000008;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[3][0]=0xa4008000;
		crypto_src_buf[3][1]=0x38;
		crypto_src_buf[3][2]=0x80000;
		crypto_src_buf[3][3]=0x0;

		crypto_src_buf[4][0]=0x80000038;
		crypto_src_buf[4][1]=0x00;
		crypto_src_buf[4][2]=0x00000;
		crypto_src_buf[4][3]=PHYSICAL_ADDRESS(_sha2_plaintext);

		crypto_src_buf[5][0]=0x80000008;
		crypto_src_buf[5][1]=0x00;
		crypto_src_buf[5][2]=0x00000;
		crypto_src_buf[5][3]=PHYSICAL_ADDRESS(_sha2_padding);

		crypto_src_buf[6][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000000;
		crypto_dst_buf[0][1]=0x0;
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

		crypto_dst_buf[1][0]=0x80000000;
		crypto_dst_buf[1][1]=0x0;
		crypto_dst_buf[1][2]=0x0;
		crypto_dst_buf[1][3]=0x0;
		crypto_dst_buf[1][4]=0x0;
		crypto_dst_buf[1][5]=0x0;
		crypto_dst_buf[1][6]=0x0;
		crypto_dst_buf[1][7]=0x0;
flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);
        //printf("bus=%x.", (*((volatile unsigned int*)(0xb81450b0))));
		//int bus1 = (*((volatile unsigned int*)(0xb81450b0)));
		//int bus2 = (*((volatile unsigned int*)(0xb81450b0)));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);
		//bus2 = (*((volatile unsigned int*)(0xb81450b0)));
		//if (bus2 != bus1)
			//printf("bus=%x.", bus2);
		
		for (i=0 ; i<1024 ; i++) {
			
		//bus2 = (*((volatile unsigned int*)(0xb81450b0)));
		//if (bus2 != bus1)
		//	printf("bus=%x.", bus2);

			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x\n", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x,", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
		printf("result2=%x, %x, %x, %x\n", crypto_dst_buf[0][4], crypto_dst_buf[0][5],
		                                crypto_dst_buf[0][6], crypto_dst_buf[0][7]);
		printf("bresult=%x, %x, %x, %x,", crypto_dst_buf[1][0], crypto_dst_buf[1][1],
		                                crypto_dst_buf[1][2], crypto_dst_buf[1][3]);
		printf("bresult2=%x, %x, %x, %x\n", crypto_dst_buf[1][4], crypto_dst_buf[1][5],
		                                crypto_dst_buf[1][6], crypto_dst_buf[1][7]);
	}
	if (interrupt == 7) {
		printf("test AES IPS_DDUEIE\n");
		crypto_src_buf[0][0]=0xa3a80010;
		crypto_src_buf[0][1]=0x10;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x80000010;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_buf);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x00000010; /* 0x80000000 -> change own bit */
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;
flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));

		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
		        _aes_test_out_buf[7]);
		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
		        _aes_test_out_buf[15]);
	}
	
	if (interrupt == 8) {
		printf("test AES IPS_DDOKIE\n");
		crypto_src_buf[0][0]=0xa3a80010;
		crypto_src_buf[0][1]=0x10;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x80000010;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_buf);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000010;
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;

flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
		        _aes_test_out_buf[7]);
		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
		        _aes_test_out_buf[15]);
	}

	if (interrupt == 9) {
		printf("test AES destination data error\n");
		printf("ecb mode \n");
		crypto_src_buf[0][0]=0xa3a00010;
		crypto_src_buf[0][1]=0xA0;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x800000A0;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_buf);

		crypto_src_buf[3][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x80000050; /* 0x800000A0 -> change length */
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		//_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		//_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;
		for (i=0 ; i<320 ; i++)
			_aes_test_out_buf[i] = i;
flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("orig=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
//
//		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
//		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
//		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
//		        _aes_test_out_buf[7]);
//		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
//		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
//		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
//		        _aes_test_out_buf[15]);
		printf("aes_enc_out=");
		for (i=0 ; i<23 ; i++)
			printf("\n %x, %x, %x, %x, %x, %x, %x, %x,", _aes_test_out_buf[0+8*i], 
			        _aes_test_out_buf[1+8*i], _aes_test_out_buf[2+8*i], _aes_test_out_buf[3+8*i],
			        _aes_test_out_buf[4+8*i], _aes_test_out_buf[5+8*i], _aes_test_out_buf[6+8*i],
			        _aes_test_out_buf[7+8*i]);
		printf("\n");
	}
#if 1
	if (interrupt == 10) {
		printf("test AES (DABF) destination address buffer full-1\n");
		printf("ecb mode \n");

		HAL_CRYPTO_WRITE32(REG_IPSCTR, HAL_CRYPTO_READ32(REG_IPSCTR) | IPS_SAWB );

		crypto_src_buf[0][0]=0xa3a00010;
		crypto_src_buf[0][1]=0xA0;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x80000091;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_out_buf);

		crypto_src_buf[3][0]=0x80000001;
		crypto_src_buf[3][1]=0x00;
		crypto_src_buf[3][2]=0x00000;
		crypto_src_buf[3][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x91;

		crypto_src_buf[4][0]=0x80000001;
		crypto_src_buf[4][1]=0x00;
		crypto_src_buf[4][2]=0x00000;
		crypto_src_buf[4][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x92;

		crypto_src_buf[5][0]=0x80000002;
		crypto_src_buf[5][1]=0x00;
		crypto_src_buf[5][2]=0x00000;
		crypto_src_buf[5][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x93;

		crypto_src_buf[6][0]=0x80000002;
		crypto_src_buf[6][1]=0x00;
		crypto_src_buf[6][2]=0x00000;
		crypto_src_buf[6][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x95;

		crypto_src_buf[7][0]=0x80000002;
		crypto_src_buf[7][1]=0x00;
		crypto_src_buf[7][2]=0x00000;
		crypto_src_buf[7][3]=PHYSICAL_ADDRESS(_aes_test_buf)+0x97;

		crypto_src_buf[8][0]=0x80000007;
		crypto_src_buf[8][1]=0x00;
		crypto_src_buf[8][2]=0x00000;
		crypto_src_buf[8][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x99;

		crypto_src_buf[9][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x800000A0; /* 0x800000A0 */
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		//_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		//_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;
		for (i=0 ; i<160 ; i++)
			_aes_test_out_buf[i] = _aes_test_buf[i];
flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("orig=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
//
//		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
//		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
//		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
//		        _aes_test_out_buf[7]);
//		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
//		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
//		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
//		        _aes_test_out_buf[15]);
		printf("aes_enc_out=");
		for (i=0 ; i<23 ; i++)
			printf("\n %x, %x, %x, %x, %x, %x, %x, %x,", _aes_test_out_buf[0+8*i], 
			        _aes_test_out_buf[1+8*i], _aes_test_out_buf[2+8*i], _aes_test_out_buf[3+8*i],
			        _aes_test_out_buf[4+8*i], _aes_test_out_buf[5+8*i], _aes_test_out_buf[6+8*i],
			        _aes_test_out_buf[7+8*i]);
		printf("\n");
	}

	if (interrupt == 11) {
		printf("test AES (DABF) destination address buffer full-2\n");
		printf("ecb mode \n");

		HAL_CRYPTO_WRITE32(REG_IPSCTR, HAL_CRYPTO_READ32(REG_IPSCTR) | IPS_SAWB );

		crypto_src_buf[0][0]=0xa3a00010;
		crypto_src_buf[0][1]=0xA0;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x80000091;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_out_buf);

		crypto_src_buf[3][0]=0x80000001;
		crypto_src_buf[3][1]=0x00;
		crypto_src_buf[3][2]=0x00000;
		crypto_src_buf[3][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x91;

		crypto_src_buf[4][0]=0x80000001;
		crypto_src_buf[4][1]=0x00;
		crypto_src_buf[4][2]=0x00000;
		crypto_src_buf[4][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x92;

		crypto_src_buf[5][0]=0x80000002;
		crypto_src_buf[5][1]=0x00;
		crypto_src_buf[5][2]=0x00000;
		crypto_src_buf[5][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x93;

		crypto_src_buf[6][0]=0x80000002;
		crypto_src_buf[6][1]=0x00;
		crypto_src_buf[6][2]=0x00000;
		crypto_src_buf[6][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x95;

		crypto_src_buf[7][0]=0x80000002;
		crypto_src_buf[7][1]=0x00;
		crypto_src_buf[7][2]=0x00000;
		crypto_src_buf[7][3]=PHYSICAL_ADDRESS(_aes_test_buf)+0x97;

		crypto_src_buf[8][0]=0x80000002;
		crypto_src_buf[8][1]=0x00;
		crypto_src_buf[8][2]=0x00000;
		crypto_src_buf[8][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x99;

		crypto_src_buf[9][0]=0x80000005;
		crypto_src_buf[9][1]=0x00;
		crypto_src_buf[9][2]=0x00000;
		crypto_src_buf[9][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x9b;

		crypto_src_buf[10][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x800000A0; /* 0x800000A0 */
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		//_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		//_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;
		for (i=0 ; i<160 ; i++)
			_aes_test_out_buf[i] = _aes_test_buf[i];
flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("orig=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
//
//		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
//		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
//		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
//		        _aes_test_out_buf[7]);
//		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
//		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
//		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
//		        _aes_test_out_buf[15]);
		printf("aes_enc_out=");
		for (i=0 ; i<23 ; i++)
			printf("\n %x, %x, %x, %x, %x, %x, %x, %x,", _aes_test_out_buf[0+8*i], 
			        _aes_test_out_buf[1+8*i], _aes_test_out_buf[2+8*i], _aes_test_out_buf[3+8*i],
			        _aes_test_out_buf[4+8*i], _aes_test_out_buf[5+8*i], _aes_test_out_buf[6+8*i],
			        _aes_test_out_buf[7+8*i]);
		printf("\n");
	}
#endif
	if (interrupt == 12) {
		printf("test AES (DABF) destination address buffer full-3\n");
		printf("ecb mode \n");

		HAL_CRYPTO_WRITE32(REG_IPSCTR, HAL_CRYPTO_READ32(REG_IPSCTR) | IPS_SAWB );

		crypto_src_buf[0][0]=0xa3a00010;
		crypto_src_buf[0][1]=0xA0;
		crypto_src_buf[0][2]=0x0;
		crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);

		crypto_src_buf[1][0]=0x80000010;
		crypto_src_buf[1][1]=0x00;
		crypto_src_buf[1][2]=0x00000;
		crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);

		crypto_src_buf[2][0]=0x80000091;
		crypto_src_buf[2][1]=0x00;
		crypto_src_buf[2][2]=0x00000;
		crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_out_buf);

		crypto_src_buf[3][0]=0x80000001;
		crypto_src_buf[3][1]=0x00;
		crypto_src_buf[3][2]=0x00000;
		crypto_src_buf[3][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x91;

		crypto_src_buf[4][0]=0x80000001;
		crypto_src_buf[4][1]=0x00;
		crypto_src_buf[4][2]=0x00000;
		crypto_src_buf[4][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x92;

		crypto_src_buf[5][0]=0x80000002;
		crypto_src_buf[5][1]=0x00;
		crypto_src_buf[5][2]=0x00000;
		crypto_src_buf[5][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x93;

		crypto_src_buf[6][0]=0x80000002;
		crypto_src_buf[6][1]=0x00;
		crypto_src_buf[6][2]=0x00000;
		crypto_src_buf[6][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x95;

		crypto_src_buf[7][0]=0x80000002;
		crypto_src_buf[7][1]=0x00;
		crypto_src_buf[7][2]=0x00000;
		crypto_src_buf[7][3]=PHYSICAL_ADDRESS(_aes_test_buf)+0x97;

		crypto_src_buf[8][0]=0x80000002;
		crypto_src_buf[8][1]=0x00;
		crypto_src_buf[8][2]=0x00000;
		crypto_src_buf[8][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x99;

		crypto_src_buf[9][0]=0x80000002;
		crypto_src_buf[9][1]=0x00;
		crypto_src_buf[9][2]=0x00000;
		crypto_src_buf[9][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x9b;

		crypto_src_buf[10][0]=0x80000003;
		crypto_src_buf[10][1]=0x00;
		crypto_src_buf[10][2]=0x00000;
		crypto_src_buf[10][3]=PHYSICAL_ADDRESS(_aes_test_out_buf)+0x9d;

		crypto_src_buf[11][0]=0x0;

flush_dcache_range((u32)crypto_src_buf, ((u32)crypto_src_buf)+sizeof(crypto_src_buf));
//invalidate_dcache_range((u32crypto_src_buf), ((u32)crypto_src_buf)+sizeof(crypto_src_buf));

		crypto_dst_buf[0][0]=0x800000A0; /* 0x800000A0 */
		crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_out_buf);
		crypto_dst_buf[0][2]=0x0;
		crypto_dst_buf[0][3]=0x0;
		crypto_dst_buf[0][4]=0x0;
		crypto_dst_buf[0][5]=0x0;
		crypto_dst_buf[0][6]=0x0;
		crypto_dst_buf[0][7]=0x0;

flush_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
//invalidate_dcache_range((u32)crypto_dst_buf, ((u32)crypto_dst_buf)+sizeof(crypto_dst_buf));
		//_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		//_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;
		for (i=0 ; i<160 ; i++)
			_aes_test_out_buf[i] = _aes_test_buf[i];
flush_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
//invalidate_dcache_range((u32)_aes_test_out_buf, ((u32)_aes_test_out_buf)+sizeof(_aes_test_out_buf));
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("orig=%x, %x, %x, %x\n", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<1024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
		}
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		                                crypto_dst_buf[0][2], crypto_dst_buf[0][3]);
//
//		printf("aes_enc_out=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[0],
//		        _aes_test_out_buf[1], _aes_test_out_buf[2], _aes_test_out_buf[3],
//		        _aes_test_out_buf[4], _aes_test_out_buf[5], _aes_test_out_buf[6],
//		        _aes_test_out_buf[7]);
//		printf("aes_enc_out2=%x, %x, %x, %x, %x, %x, %x, %x\n", _aes_test_out_buf[8],
//		        _aes_test_out_buf[9], _aes_test_out_buf[10], _aes_test_out_buf[11],
//		        _aes_test_out_buf[12], _aes_test_out_buf[13], _aes_test_out_buf[14],
//		        _aes_test_out_buf[15]);
		printf("aes_enc_out=");
		for (i=0 ; i<23 ; i++)
			printf("\n %x, %x, %x, %x, %x, %x, %x, %x,", _aes_test_out_buf[0+8*i], 
			        _aes_test_out_buf[1+8*i], _aes_test_out_buf[2+8*i], _aes_test_out_buf[3+8*i],
			        _aes_test_out_buf[4+8*i], _aes_test_out_buf[5+8*i], _aes_test_out_buf[6+8*i],
			        _aes_test_out_buf[7+8*i]);
		printf("\n");
	}

	if (interrupt == 20) {
		printf("cpu cp0 count \n");
		set_c0_cause(CAUSEF_DC);
		write_c0_count(0);
		clear_c0_cause(CAUSEF_DC);
		

		i=read_c0_count();
		//0x7fffffff = 2147483647
		// cpu=66Mhz, counter = cpu/2=33Mhz, 33Mhz max wait 65sec
		// cpu=800Mhz, counter = cpu/2=400Mhz, 400Mhz max wait 5.37sec
		while (i< (33000000*4)) {
		//while (i< (400000000*5)) {
			i=read_c0_count();
		}
		printf("cpu cp0 count =%d\n", i);
	}

	if (interrupt == 21) {
		printf("test through put \n");
		set_c0_cause(CAUSEF_DC);
		write_c0_count(0);

		//disable interrupt mask, enable loopback
		//HAL_CRYPTO_WRITE32(REG_IPSCTR, IPS_LBKM | (HAL_CRYPTO_READ32(REG_IPSCTR) & 0xffff07ff ));
		HAL_CRYPTO_WRITE32(REG_IPSCTR, (HAL_CRYPTO_READ32(REG_IPSCTR) & 0xffff07ff ));
#if 1	//aes128
		printf("test AES-128 enc through put\n");
		pcrypto_src_buf[0*4+0] = 0xa3a00000;//0xa3a80010;
#elif 0
		printf("test AES-256 enc through put\n");
		pcrypto_src_buf[0*4+0] = 0xa3e00000;//0xa3a80010;
#elif 0
		//printf("test DES loopback through put\n");
		printf("test DES enc through put\n");
		pcrypto_src_buf[0*4+0] = 0xa3800000;//0xa3a80010;
#else
		printf("test 3DES enc through put\n");
		pcrypto_src_buf[0*4+0] = 0xa2900000;//0xa3a80010;
#endif
		//0xa3a00000 aes-128bit
		//0xa3800000 des
		//crypto_src_buf[0][0]=0xa3a00010;
		//crypto_src_buf[0][1]=0x3ff0;
		pcrypto_src_buf[0*4+1] = 0x3ff0;
		//crypto_src_buf[0][2]=0x0;
		pcrypto_src_buf[0*4+2] = 0x0;
		//crypto_src_buf[0][3]=PHYSICAL_ADDRESS(_aes_test_key);
		pcrypto_src_buf[0*4+3] = PHYSICAL_ADDRESS(_aes_test_key);

		//crypto_src_buf[1][0]=0x80000010;
		pcrypto_src_buf[1*4+0] = 0x80000000;//0x80000010;
		//crypto_src_buf[1][1]=0x00;
		pcrypto_src_buf[1*4+1] = 0x00;
		//crypto_src_buf[1][2]=0x00000;
		pcrypto_src_buf[1*4+2] = 0x00000;
		//crypto_src_buf[1][3]=PHYSICAL_ADDRESS(_aes_test_iv_1);
		pcrypto_src_buf[1*4+3] = PHYSICAL_ADDRESS(_aes_test_iv_1);

		//crypto_src_buf[2][0]=0x80003ff0;
		pcrypto_src_buf[2*4+0] = 0x80003ff0;
		//crypto_src_buf[2][1]=0x00;
		pcrypto_src_buf[2*4+1] = 0x00;
		//crypto_src_buf[2][2]=0x00000;
		pcrypto_src_buf[2*4+2] = 0x00000;
		//crypto_src_buf[2][3]=PHYSICAL_ADDRESS(0xbfe00000);
		pcrypto_src_buf[2*4+3] = PHYSICAL_ADDRESS(0xbfe00000);
		//crypto_src_buf[2][3]=PHYSICAL_ADDRESS(_aes_test_buf);
		//pcrypto_src_buf[2*4+3] = PHYSICAL_ADDRESS(_aes_test_buf);

		//crypto_src_buf[3][0]=0x0;
		pcrypto_src_buf[3*4+0] = 0x0;


		//crypto_dst_buf[0][0]=0x80000000; /* 0x80000000 */
		pcrypto_dst_buf[0] = 0x80000000;
		//crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(0xbfe00000);
		pcrypto_dst_buf[1] = PHYSICAL_ADDRESS(0xbfe00000);
		//crypto_dst_buf[0][1]=PHYSICAL_ADDRESS(_aes_test_buf);
		//crypto_dst_buf[0][2]=0x0;
		pcrypto_dst_buf[2] = 0;
		//crypto_dst_buf[0][3]=0x0;
		pcrypto_dst_buf[3] = 0;
		//crypto_dst_buf[0][4]=0x0;
		pcrypto_dst_buf[4] = 0;
		//crypto_dst_buf[0][5]=0x0;
		pcrypto_dst_buf[5] = 0;
		//crypto_dst_buf[0][6]=0x0;
		pcrypto_dst_buf[6] = 0;
		//crypto_dst_buf[0][7]=0x0;
		pcrypto_dst_buf[7] = 0; 

		//_aes_test_out_buf[0] = _aes_test_out_buf[2] = _aes_test_out_buf[4] = _aes_test_out_buf[6] = 0;
		//_aes_test_out_buf[8] = _aes_test_out_buf[10] = _aes_test_out_buf[12] = _aes_test_out_buf[14] = 0;

		HAL_CRYPTO_WRITE32(REG_IPSCSR, HAL_CRYPTO_READ32(REG_IPSCSR) | IPS_SDUEIP | IPS_SDLEIP | IPS_DDUEIP | IPS_DDOKIP);

		clear_c0_cause(CAUSEF_DC);

		HAL_CRYPTO_WRITE32(REG_IPSCSR, IPS_POLL);

		for (i=0 ; i<131024 ; i++) {
			j = HAL_CRYPTO_READ32(REG_IPSCSR);
			if (j & IPS_DDOKIP) {
				break;
			}
		}
		set_c0_cause(CAUSEF_DC);
		j=read_c0_count();
		printf("cpu cp0 count =%d\n", j);
		printf("REG_IPSDDAR=%x, REG_IPSSDAR=%x, REG_IPSCSR=%x, REG_IPSCTR=%x", HAL_CRYPTO_READ32(REG_IPSDDAR), HAL_CRYPTO_READ32(REG_IPSSDAR), HAL_CRYPTO_READ32(REG_IPSCSR), HAL_CRYPTO_READ32(REG_IPSCTR));
		//printf("result=%x, %x, %x, %x", crypto_dst_buf[0][0], crypto_dst_buf[0][1],
		printf("loop count =%d\n", i);

		i=read_c0_count();
		printf("cpu cp0 count =%d\n", i);
	}
}


