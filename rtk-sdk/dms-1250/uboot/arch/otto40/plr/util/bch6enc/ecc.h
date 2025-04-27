#ifndef ECC_H
#define ECC_H

class ecc_encoder_t {
    protected:
        unsigned int page_size;
        unsigned int oob_size;
        unsigned int ecc_size;
    public:
        ecc_encoder_t(unsigned int page_sz, unsigned int oob_sz, unsigned int ecc_sz):
            page_size(page_sz), oob_size(oob_sz), ecc_size(ecc_sz) {}
        unsigned int get_page_size() const {return page_size;}
        unsigned int get_oob_size() const {return oob_size;}
        unsigned int get_ecc_size() const {return ecc_size;}
        unsigned int get_unit_size() const {return page_size+oob_size+ecc_size;}
        virtual void encode_512B(unsigned char *ecc,    // ecc: output 10 bytes of ECC code
            const unsigned char *input_buf,             // input_buf: the 512 bytes input data (RS_V2_PAGE_SIZE bytes)
            const unsigned char *oob)=0;                // oob: 6 bytes out-of-band for input (RS_V2_OOB_SIZE bytes)
};

#endif // ECC_H


