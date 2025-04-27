#include <stdio.h>
#include <string.h>
#include "bch6.h"

#define TenBit_Mask 0x3ff

static const unsigned char gen_poly_6b[79] = {1,0,1,1,1,1,1,0,1,0,0,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,1,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,0,0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1};
static unsigned char R_6b[78] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


static inline unsigned char ToBit(unsigned short value, int bth)
{
	value = (value >> bth) & 0x1;
	return (unsigned char)value;
}


static void bch6_encode_ecc_byte(unsigned char input)
{
    unsigned char R7_6b[78], R6_6b[78], R5_6b[78], R4_6b[78], R3_6b[78], R2_6b[78], R1_6b[78], R0_6b[78];
	int i,j;

	for (i=0;i<78;i++)
	{
		if (i == 0)
			R7_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 7) ^ R_6b[77]));
		else
			R7_6b[i] = R_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 7) ^ R_6b[77]));
	}

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R6_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 6) ^ R7_6b[77]));
		else
        	R6_6b[i] = R7_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 6) ^ R7_6b[77]));
    }

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R5_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 5) ^ R6_6b[77]));
		else
        	R5_6b[i] = R6_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 5) ^ R6_6b[77]));
    }

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R4_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 4) ^ R5_6b[77]));
		else
        	R4_6b[i] = R5_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 4) ^ R5_6b[77]));
    }

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R3_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 3) ^ R4_6b[77]));
		else
        	R3_6b[i] = R4_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 3) ^ R4_6b[77]));
    }

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R2_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 2) ^ R3_6b[77]));
		else
        	R2_6b[i] = R3_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 2) ^ R3_6b[77]));
    }

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R1_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 1) ^ R2_6b[77]));
		else
        	R1_6b[i] = R2_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 1) ^ R2_6b[77]));
    }

	for (i=0;i<78;i++)
    {
        if (i == 0)
            R0_6b[0] = 0x0 ^ (gen_poly_6b[0] & (ToBit(input, 0) ^ R1_6b[77]));
		else
        	R0_6b[i] = R1_6b[i-1] ^ (gen_poly_6b[i] & (ToBit(input, 0) ^ R1_6b[77]));
    }

	//dumpR();
    unsigned short synd[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; //memset(synd, 0, sizeof(synd));

	for (i=0;i<6;i++)
		for (j=12;j>=0;j--)
			synd[i] |= (R0_6b[j+(i*13)] << j);

	for (i=0;i<6;i++)
        for (j=0;j<13;j++)
            R_6b[(i*13+j)] = ToBit(synd[i], j);

	//dumpSynd();
}

void
bch6_ecc_512B_encode(unsigned char *ecc,  // ecc: output 10 bytes of ECC code
    const unsigned char *input_buf,     // input_buf: the 512 bytes input data (BCH6_PAGE_SIZE bytes)
    const unsigned char *oob){          // oob: 6 bytes out-of-band for input (BCH6_OOB_SIZE bytes)

    // no need to init: synd, R7_6b[78], R6_6b[78], R5_6b[78], R4_6b[78], R3_6b[78], R2_6b[78], R1_6b[78], R0_6b[78];
    // should be init here: R_6b, ecc[10]
    //unsigned char ecc[10] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,};
	
	bzero(R_6b, sizeof(R_6b));
	bzero(ecc, BCH6_ECC_SIZE);
	
	int i,j;
	for ( i=0; i< BCH6_PAGE_SIZE; i++)
		bch6_encode_ecc_byte(input_buf[i]);
		
    for ( i=0; i< BCH6_OOB_SIZE; i++)
        bch6_encode_ecc_byte(oob[i]);

	for (i=0;i<9;i++)
		for (j=7;j>=0;j--)
			ecc[i] |= (R_6b[j+((8-i)*8+6)] << j);

	for (j=5;j>=0;j--)
        ecc[9] |= (R_6b[j] << (j+2));
	
	#if 0
	printf("ecc0=%x, ecc1=%x, ecc2=%x, ecc3=%x, ecc4=%x, ecc5=%x, ecc6=%x, ecc7=%x, ecc8=%x, ecc9=%x\n",
		ecc[0], ecc[1], ecc[2], ecc[3], ecc[4], ecc[5], ecc[6], ecc[7], ecc[8], ecc[9]);
    #endif
}
