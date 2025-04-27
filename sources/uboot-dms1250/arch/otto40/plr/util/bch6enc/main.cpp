#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "bch6.h"
#include <conf.h>

static void
show_message(const char *n="bch6enc") {
    fprintf(stderr, 
        "This is bch6 encoder including ecc and oob.\n"
        "usage: %s -o <out.img> [-cpb <num-chunk-per-block>] [-csz <bytes-per-chunk>] [-plr <plr.bin> [num-plr-copy]] [-skip_plr_signature [default-size]] \
[-uboot <uboot.bin> [num-uboot-copy]] [-uboot_from <uboot-start-block-num>] \
[-uboot_magic/-umagic <magic-number>] [-uboot_load_addr/-ula <load_addr>] \
[-uboot_entry_addr/-uea <entry-addr>] [-uboot_size/-usize <image-size>] [-uboot_comp/-ucomp <compress-type>] \
[-uboot_cont/-ucont] [-uboot_lzma/-ulzma [lzma_tool_path]] [-no_uboot_header|-no_hdr]\n"
        "\t-o <out.img>: the file name of output file;\n"
        "\t[-cpb <num-chunk-per-block>]: the number of chuncks per block, default is 64;\n"
        "\t[-csz <bytes-per-chunk>]: the number of bytes in a chunk, now 2048 supported only;\n"
        "\t[-plr <plr.bin> [num-plr-copy]]:\n"
        "\t\t<plr.bin> is preloader image file;\n"
        "\t\t[num-plr-copy] is the number of copies of preloader will be stored, default 2;\n"
        "\t[-skip_plr_signature [default-first-load-size]]:\n"
        "\t\tto skip the check of the preloader signature;\n"
        "\t\t[default-first-load-size] when preloader signature is wrong, the number bytes to load for preloader.\n"
        "\t[-uboot <uboot.bin> [num-uboot-copy] [-uboot_from <uboot-start-block-num>]]:\n"
        "\t\t<uboot.bin> is the uboot image file;\n"
        "\t\t[num-uboot-copy] is the number of copies of uboot to store, default 2;\n"
        "\t\t[-uboot_from <uboot-start-block-num>] is the first block to store uboot, default block is the block followed by plr;\n"
        "\t[-uboot_magic|-umagic <magic-number>]  is to set the uboot header of 'magic number';\n"
        "\t[-uboot_load_addr|-ula <load_addr>]    is to set the uboot header of 'loading address', default=0x80000000;\n"
        "\t[-uboot_entry_addr|-uea <entry-addr>]  is to set the uboot header of 'uboot entry address', default=0x80000000;\n"
        "\t[-uboot_size|-usize <image-size>]      is to set the uboot header of 'uncompressed image size';\n"
        "\t[-uboot_comp|-ucomp <compress-type>]   is to set the uboot header of 'compression type', lzma=3, default=uncompress=0;\n"
        "\t[-uboot_cont|-ucont] is to put the copies of uboot continuously, but the first one is decided by '-uboot_from';\n"
        "\t[-uboot_lzma|-ulzma] is to lzma <uboot.bin>, and then ecc the compressed image into <out.img>;\n"
        "\t[lzma_tool_path] is the path that lzma tool put;\n"       
        "\t[-no_uboot_header|-no_hdr] is not to add header for uboot in <out.img>;"
        "\n"
        , n);
}
#define MINUS_ONE 0xffffffff
unsigned int chunk_size=2048;   // current the only 2048 supported
unsigned int chunk_per_block=64;
unsigned int page_size=0;
unsigned int oob_size=0;
unsigned int ecc_size=0;
unsigned int unit_size=0;
unsigned int unit_per_chunk=0;
const char *output_file_name=NULL;
const char *uboot_file_name=NULL;
const char *plr_file_name=NULL;
const char *lzma_tool_path="./lzma";
unsigned int num_plr=2;
unsigned int num_uboot=2;
unsigned int uboot_from=MINUS_ONE;
unsigned int uboot_cont   = 0; //0: Put next uboot to block alignment
unsigned int uboot_magic      = UIMG_MAGIC_NUM;
unsigned int uboot_load_addr  = 0x80000000;
unsigned int uboot_entry_addr = 0x80000000;
unsigned int uboot_image_size = 0; //The uncompressed image size
unsigned char uboot_comp_type = UIH_COMP_NONE;
unsigned char uboot_use_hdr   = 1; //0:NOT use header; 1: Use header



int output_handle=0;
bool check_plr_signature=true;
unsigned int default_first_load_byte=8192;

ecc_encoder_t *ecc_encoder=NULL;

#define round_to_chunk(n) (((n)+chunk_size-1)/chunk_size)
#define round_to_block(n) (((n)+chunk_per_block-1)/chunk_per_block)
static unsigned int num_chunck_written=0;
#define num_block_written (num_chunck_written/chunk_per_block)
#define num_block_writing round_to_block(num_chunck_written)

static void
append_ecc_one_chunk(const unsigned char *chunk_data, unsigned int len, const unsigned char *spare) {
    unsigned char local_chunk_data[unit_per_chunk*unit_size];
    unsigned char local_spare[unit_per_chunk*oob_size];
    unsigned char ecc[unit_per_chunk*ecc_size];
    
    ++num_chunck_written;
    
    // copy source into local buffer
    memset(local_chunk_data, 0xff, sizeof(local_chunk_data));
    
    if ((chunk_data==NULL) && (spare==NULL)) {
        write(output_handle, local_chunk_data, unit_per_chunk*unit_size);
        return;
    }

    if (len>chunk_size) len=chunk_size;
    if (chunk_data!=NULL) {
        memcpy(local_chunk_data, chunk_data, len);
    }
    if (spare==NULL) {
        memset(local_spare, 0xff, sizeof(local_spare));
        spare=local_spare;
    }
	
    // compute ECC and write out
    // write out one chunk
    for (unsigned i=0,j=0,k=0; i<chunk_size; i+=page_size, j+=oob_size, k+=ecc_size) {
        ecc_encoder->encode_512B(ecc+k, local_chunk_data+i, spare+j);
    }
    write(output_handle, local_chunk_data, chunk_size);
    write(output_handle, spare, unit_per_chunk*oob_size);
    write(output_handle, ecc, unit_per_chunk*ecc_size);
}

typedef oob6t_t oob_t;

static unsigned int
file_length(int h) {
    struct stat stat_buf;
    fstat(h, &stat_buf);
    return (unsigned int)stat_buf.st_size;
}
static int 
write_out_plr(){
    unsigned char buf[chunk_size];
    
    oob_t oob;
    memset ((void*)&oob, 0xff, sizeof(oob));
    
    if (plr_file_name==NULL) return 0;
    int h=open(plr_file_name, O_RDONLY);
    if (h<0) {
        fprintf(stderr, "unable to open '%s' as preloader image\n", plr_file_name);
        return -1;
    }

    // set of oob info
    oob.bbi=0xffff;
    oob.reserved=0xffff;
    oob.signature=htonl(SIGNATURE_PLR);
    oob.num_copy=htonl(num_plr);
    oob.total_chunk=htonl(round_to_chunk(file_length(h)));
    read(h, buf, chunk_size);
    soc_t *soc=(soc_t *)(buf+OTTO_HEADER_OFFSET);
    
    unsigned int plr_signature=ntohl(soc->bios.header.signature);
    bool plr_signature_is_correct=true;
    if (plr_signature!=SIGNATURE_PLR) {
        fprintf(stderr, "'%s' is not recognized preloader, wrong signature.\n", plr_file_name);
        if (check_plr_signature) return -1;
        plr_signature_is_correct=false;
    }
    unsigned int size_of_plr_load_firstly=plr_signature_is_correct?
        ntohl(soc->bios.size_of_plr_load_firstly):default_first_load_byte;
    oob.startup_num=htonl(round_to_chunk(size_of_plr_load_firstly));

    // write content
    for (unsigned int r=0;r<num_plr;++r) {
        lseek(h, 0, SEEK_SET);
        int c, cn=(r<<16);
        while ((c=read(h, buf, chunk_size))>0) {
            oob.chunk_num=htonl(cn); ++cn;
            append_ecc_one_chunk(buf, c, (unsigned char*)&oob);
            if ((unsigned)c<chunk_size) break;
        }
    }
    
    // padding to block align
    while ((num_chunck_written%chunk_per_block))
        append_ecc_one_chunk(NULL, 0, NULL);
    
    close(h);
    return 0;
} 


static int 
write_out_uboot(){
    unsigned char buf[chunk_size];   
    oob_t oob;
    uimage_header_t uhdr;
    unsigned int hdr_size = sizeof(uimage_header_t);

    memset ((void*)&oob, 0xff, sizeof(oob));

    if (uboot_file_name==NULL) return 0;
    int h=open(uboot_file_name, O_RDONLY);
    if (h<0) {
        fprintf(stderr, "unable to open '%s' as uboot image\n", uboot_file_name);
        return -1;
    }

    if(uboot_use_hdr){
        memset ((void*)&uhdr, 0xff, hdr_size);
        if(0==uboot_image_size) uboot_image_size = lseek(h, 0, SEEK_END);
        uhdr.ih_magic = htonl(uboot_magic);
        uhdr.ih_size = htonl(uboot_image_size);
        uhdr.ih_load = htonl(uboot_load_addr);
        uhdr.ih_ep = htonl(uboot_entry_addr);
        uhdr.ih_comp = uboot_comp_type;
    }

    // skip several blocks if uboot_from was defined
    if (uboot_from!=MINUS_ONE) {
        if (uboot_from<num_block_writing) {
            fprintf(stderr, "preloader occupied %d block(s), but uboot wants to start at %d-th block\n",num_block_writing,uboot_from);
            return -1;
        }
        while (uboot_from>=num_block_written) 
            append_ecc_one_chunk(NULL, 0, NULL);
    }

    // set of oob info
    oob.bbi=0xffff;
    oob.num_copy=htonl(num_uboot);
    oob.signature=htonl(SIGNATURE_UBOOT);
    oob.total_chunk=htonl(round_to_chunk(file_length(h)));
    oob.startup_num=MINUS_ONE;

    // write content
    for (unsigned int r=0;r<num_uboot;++r) {
		lseek(h, 0, SEEK_SET);
		int c, cn=(r<<16);
        if(uboot_use_hdr){			
            memcpy(buf, &uhdr, hdr_size);
            if((c=read(h,(buf+hdr_size), (chunk_size-hdr_size)))>0){
                oob.chunk_num=htonl(cn); ++cn;
                append_ecc_one_chunk(buf, (c+hdr_size), (unsigned char*)&oob);
            }
        }
		
        while ((c=read(h, buf, chunk_size))>0) {
            oob.chunk_num=htonl(cn); ++cn;
            append_ecc_one_chunk(buf, c, (unsigned char*)&oob);
            if ((unsigned)c<chunk_size) break;
        }
        if(!uboot_cont){
        // padding to block align
        while ((num_chunck_written%chunk_per_block))
            append_ecc_one_chunk(NULL, 0, NULL);
        }
    }
    
    close(h);
    return 0;
}

int
main(int argc, char *argv[]) {
    bool error_out=false;
    bool lzma_uboot=false;
    
    if (argc==1) {
        show_message();
        return 0;
    }
    while (*(++argv)!=NULL) {
        if (strcmp(*argv, "-o")==0) {
            output_file_name=*(++argv);
        } else if ((strcmp(*argv, "--chunk-size")==0)||(strcmp(*argv, "-csz")==0)) {
            chunk_size=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "--chunk-per-block")==0)||(strcmp(*argv, "-cpb")==0)) {
            chunk_per_block=strtoul(*(++argv), NULL, 0);
        } else if (strcmp(*argv, "-plr")==0) {
            plr_file_name=*(++argv);
            const char *nxt=argv[1];
            if ((nxt!=NULL) && (nxt[0]!='-')) {
                num_plr=strtoul(nxt, NULL, 0);
                ++argv;
            }
        } else if (strcmp(*argv, "-uboot")==0) {
            uboot_file_name=*(++argv);
            const char *nxt=argv[1];
            if ((nxt!=NULL) && (nxt[0]!='-')) {
                num_uboot=strtoul(nxt, NULL, 0);
                ++argv;
            }
        } else if (strcmp(*argv, "-uboot_from")==0) {
            uboot_from=strtoul(*(++argv), NULL, 0);
        } else if (strcmp(*argv, "-skip_plr_signature")==0) {
            check_plr_signature=false;
            const char *nxt=argv[1];
            if ((nxt!=NULL) && (nxt[0]!='-')) {
                default_first_load_byte=strtoul(nxt, NULL, 0);
                ++argv;
            }
        } else if ((strcmp(*argv, "-uboot_cont")==0)||(strcmp(*argv, "-uc")==0)) {
            uboot_cont = 1;
        } else if ((strcmp(*argv, "-uboot_magic")==0)||(strcmp(*argv, "-umagic")==0)) {
            uboot_magic=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "-uboot_load_addr")==0)||(strcmp(*argv, "-ula")==0)) {
            uboot_load_addr=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "-uboot_entry_addr")==0)||(strcmp(*argv, "-uea")==0)) {
            uboot_entry_addr=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "-uboot_size")==0)||(strcmp(*argv, "-usize")==0)) {
            uboot_image_size=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "-uboot_compress")==0)||(strcmp(*argv, "-ucomp")==0)) {
            uboot_comp_type=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "-uboot_lzma")==0)||(strcmp(*argv, "-ulzma")==0)) {
            lzma_uboot = true;
            const char *nxt=argv[1];
            if ((nxt!=NULL) && (nxt[0]!='-')) lzma_tool_path=*(++argv);
        } else if ((strcmp(*argv, "-no_uboot_header")==0)||(strcmp(*argv, "-no_hdr")==0)) {
            uboot_use_hdr = 0;
        } else {
            fprintf(stderr, "unknown options '%s'\n", *argv);
        }
    }

    if ((plr_file_name==NULL) && (uboot_file_name==NULL)) {
        fprintf(stderr, "Either -plr nor -uboot was defined\n");
        error_out=true;
    }
    if (output_file_name==NULL) {
        fprintf(stderr, "output file name was not defined\n");
        error_out=true;
    } else if ((output_handle=open(output_file_name, O_WRONLY|O_TRUNC|O_CREAT, 0666))<0) {
        fprintf(stderr, "output file '%s' unable to open\n", output_file_name);
        error_out=true;
    }
    switch (chunk_size) {
        case 2048:
            break;
        default:
            fprintf(stderr, "chunk size of %d bytes is not supported\n", chunk_size);
            error_out=true;
            break;
    }
    if(true == lzma_uboot){
        char tmp[200];
        sprintf(tmp,"%s e %s %s.lzma",lzma_tool_path,uboot_file_name,uboot_file_name);
        int ret=system(tmp);
        if(ret < 0){
            fprintf(stderr, "Doing lzma fail!!\n");
            error_out=true;
        }
        sprintf(tmp,"%s.lzma",uboot_file_name);
        uboot_file_name=tmp;
        uboot_comp_type=UIH_COMP_LZMA;
        if(0==uboot_image_size){
            int h=open(uboot_file_name, O_RDONLY);
            if(h<0){
                fprintf(stderr, "unable to open '%s' as uboot image\n", uboot_file_name);
                error_out=true;
            }
            uboot_image_size = lseek(h, 0, SEEK_END);
            close(h);
        }
    }

    if (error_out) {
        show_message();
        return 1;
    }
    // select bch6 by default
    ecc_encoder=new ecc_bch6_encode_t();
    page_size=ecc_encoder->get_page_size();
    oob_size=ecc_encoder->get_oob_size();
    ecc_size=ecc_encoder->get_ecc_size();
    unit_size=ecc_encoder->get_unit_size();
    unit_per_chunk=chunk_size/page_size;

    if (unit_per_chunk*oob_size!=sizeof(oob_t)) {
        fprintf(stderr, "unmach structure size of sizeof(oob_t)=%u, and oob-size=%u\n", 
            (unsigned int)sizeof(oob_t), unit_per_chunk*oob_size);
        return 1;
    }

    
    return write_out_plr() || write_out_uboot() || close(output_handle);
}
