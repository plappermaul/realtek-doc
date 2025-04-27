#include <common/rt_type.h>
#if defined(__LUNA_KERNEL__)
#include <naf_kernel_util.h>
#include <ecc_ctrl.h>
#else
#include <string.h>
#include <init_define.h>
#include <soc.h>
#endif
#include <ecc/ecc_struct.h>



#define BCH6_PAGE_SIZE     512
#define BCH6_OOB_SIZE      6
#define BCH6_ECC_SIZE      10
#define BCH6_UNIT_SIZE     (BCH6_PAGE_SIZE+BCH6_OOB_SIZE+BCH6_ECC_SIZE)



#define TenBit_Mask 0x3ff

static const unsigned char gen_poly_6b[79] SECTION_SDATA = {1,0,1,1,1,1,1,0,1,0,0,0,1,1,0,1,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,1,0,0,0,0,1,1,1,1,0,0,1,0,0,1,1,1,0,0,0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1};
static unsigned char R_6b[78] SECTION_SDATA = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


SECTION_UNS_TEXT static inline unsigned char ToBit(unsigned short value, int bth)
{
    value = (value >> bth) & 0x1;
    return (unsigned char)value;
}


SECTION_UNS_TEXT static void bch6_encode_ecc_byte(unsigned char input)
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

SECTION_UNS_TEXT void
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


SECTION_UNS_TEXT void
plr_ecc_encode_bch(u32_t ecc_ability, void *dma_addr, void *p_eccbuf)
{
    u32_t encode_addr = (u32_t)dma_addr;
    u32_t page_size = BCH_SECTOR_SIZE*BCH_SECTS_PER_2K_PAGE;
    u8_t *tag_addr = (u8_t *)(encode_addr + page_size);
    u8_t *syn_addr = (u8_t *)(tag_addr + BCH_TAG_SIZE*BCH_SECTS_PER_2K_PAGE);

    u32_t syn_size;
    if(12 == ecc_ability) syn_size = BCH12_SYNDROME_SIZE;
    else syn_size = BCH6_SYNDROME_SIZE;

    dcache_wr_inv((u32_t)dma_addr, (u32_t)(dma_addr+page_size));

    u32_t j;
    for(j=0 ; j<BCH_SECTS_PER_2K_PAGE ; j++, encode_addr+=BCH_SECTOR_SIZE, tag_addr+=BCH_TAG_SIZE, syn_addr+=syn_size){
        bch6_ecc_512B_encode((unsigned char *)(syn_addr), (unsigned char *)encode_addr, (unsigned char *)tag_addr);
        dcache_wr_inv((u32_t)tag_addr, (u32_t)(tag_addr+BCH_TAG_SIZE));
        dcache_wr_inv((u32_t)syn_addr, (u32_t)(syn_addr+syn_size));
    }
}


#if !defined(__LUNA_KERNEL__)
SECTION_RECYCLE void init_ecc_sw_patch(void)
{
    plr_spi_nand_flash_info._ecc_encode = &plr_ecc_encode_bch;
}
REG_INIT_FUNC(init_ecc_sw_patch, 5);
#endif



