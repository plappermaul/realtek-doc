#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <conf.h>


static unsigned int
file_length(int h) {
    struct stat stat_buf;
    fstat(h, &stat_buf);
    return (unsigned int)stat_buf.st_size;
}
static unsigned int
checksum32(char *buf, unsigned int sz) {
    unsigned int *b=(unsigned int *)buf;
    unsigned int s=0;
    sz/=sizeof(unsigned int);
    while ((sz--)>0) {
        s+=ntohl(*(b++));
    }
    return s;
}
static void
minus_checksum(unsigned int* v, unsigned int s) {
    unsigned int u=ntohl(*v);
    u-=s;
    *v=htonl(u);
}


static void
show_message(const char *n="lplr_checksum") {
    fprintf(stderr, 
    "This utility is used to pad lplr to a desired size and make the 32-bit check of this file becoming zero.\n"
    "Usage: %s -s <desired_size> -c <the_byte_offset_to_add_checksum> <lplr.img>\n"
    "    <desired_size>: should be a multiple of 4KB\n"
    "    <the_byte_offset_to_add_checksum>: where it can add 32-bit checksum, the offset should be 4-byte align\n"
    "    <lplr.img>: the image file of lplr, it is both input and output file\n"
    , n);
}
#define MINUS_ONE   0xffffffff
const char *lplr_img=NULL;
unsigned int checksum_offset=MINUS_ONE;
unsigned int lplr_size=MINUS_ONE;
int main(int argc, char *argv[]) {
    bool error_out=false;
    while (*(++argv)!=NULL) {
        if (strcmp(*argv, "-s")==0) {
            lplr_size=strtoul(*(++argv), NULL, 0);
        } else if (strcmp(*argv, "-c")==0) {
            checksum_offset=strtoul(*(++argv), NULL, 0);
        } else {
            lplr_img=*argv;
        }
    }
    if (lplr_img==NULL) {
        fprintf(stderr, "missing <lplr.img>\n");
        error_out=true;
    }
    if ((lplr_size==MINUS_ONE)||(lplr_size==0)) {
        fprintf(stderr, "missing <desired_size>\n");
        error_out=true;
    } else if ((lplr_size%4096)!=0) {
        fprintf(stderr, "<desired_size> should be a multiple of 4KB\n");
        error_out=true;
    }
    if ((checksum_offset==MINUS_ONE)||(checksum_offset==0)) {
        fprintf(stderr, "missing <the_byte_offset_to_add_checksum>\n");
        error_out=true;
    } else if ((checksum_offset&3)!=0) {
        fprintf(stderr, "<the_byte_offset_to_add_checksum> should be 4-byte align\n");
        error_out=true;
    }
    if (checksum_offset>lplr_size) {
        fprintf(stderr, "<the_byte_offset_to_add_checksum> is larger thane the <desired_size>\n");
        error_out=true;
    }
    if (error_out) {
        show_message();
        return 1;
    }
    
    // read lplr.img
    int h=open(lplr_img, O_RDONLY);
    if (h<0) {
        fprintf(stderr, "can't open '%s' as <lplr.img>\n", lplr_img);
        return 1;
    }
    unsigned l=file_length(h);
    if (l>lplr_size) {
        fprintf(stderr, "the size of '%s' has %d bytes which is larger than the desired size %d bytes\n",
            lplr_img, l, lplr_size);
        return 1;
    }
    char *buf=(char*) malloc(lplr_size);
    bzero(buf, lplr_size);
    if (read(h, buf, l)!=l) {
        fprintf(stderr, "can't read '%s' completely\n", lplr_img);
        return 1;
    }
    close(h);

    // compute check sum
    unsigned int chs=checksum32(buf, lplr_size);
    if ((chs==0)&&(l==lplr_size)) return 0;
    
    minus_checksum((unsigned int*)(buf+checksum_offset), chs);
    unsigned int vchs=checksum32(buf, lplr_size);
    if (vchs!=0) {
        fprintf(stderr, "internal error: vchs=%d (should be zero)\n", vchs);
        return 1;
    }
    
    // write out
    h=open(lplr_img, O_WRONLY | O_TRUNC);
    if (h<0) {
        fprintf(stderr, "can't open '%s' as output file\n", lplr_img);
        return 1;
    }
    write(h, buf, lplr_size);
    close(h);
    
    return 0;
}




