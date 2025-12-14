/* #########Realtek, RTK ########### */
#include <common.h>
#include <command.h>
#include <net.h>
#include <cpu_func.h>
#include <u-boot/crc.h>

#define NetSetTimeout           net_set_timeout_handler
#define NetBootFileXferSize     net_boot_file_size
/*
 * multicast upgrade support here.
 */
#define SWAP16(i)  ( ((i<<8)&0xFF00) | (((unsigned short)i >> 8)&0x00FF ))

#define SWAP32(val)     ( (unsigned int)((val & 0xff) << 24) |\
                                          (unsigned int)((val & 0xff00) << 8) |\
                                          (unsigned int)((val & 0xff0000) >> 8) |\
                                          (unsigned int)((val & 0xff000000) >> 24) )

#define RNET16(add) (unsigned short)((((unsigned short)*((unsigned short*)(add))) << 8) | \
                        (unsigned short)(*((unsigned short *)(add)+1)))

#define RNET32(add)  (((unsigned int)*((unsigned char *)(add)) << 24) | \
                ((unsigned int)*((unsigned char *)(add)+1) << 16) | \
                ((unsigned int)*((unsigned char *)(add)+2) <<  8) | \
                ((unsigned int)*((unsigned char *)(add)+3)))

#define natohs(a) RNET16(a)     //GETSHORT(x)
#define natohl(a) RNET32(a)     //GETLONG(x)


#define MUTLICAST_FRAME_LENGTH          1024
#define SEQNUM_EXIST                    0x1
#define SEQNUM_NOT_EXIST                                0x0
#define IMAGE_NOT_INTEGRITY                             0x12
#define IMAGE_INTEGRITY                                 0x13

#define MUP_DATATYPE_NOP                                0x0
#define MUP_DATATYPE_NORM                               0x1
#define MUP_DATATYPE_LAST                               0x2
#define MUP_DATATYPE_INFO                               0x4
#define MUP_DATATYPE_MASK                               0x7

#define IP_MCAST                                        0xE0000000
#define IP_MCAST_MASK                                   0xF0000000


typedef struct partition_hdr_t {
    char name[20];
    unsigned int addrstart;
    unsigned int addrend;
    unsigned char cover_flag;
    unsigned char nop[3];
} __attribute__ ((__packed__)) partition_hdr;



typedef struct partition_info_t {
    int valid;                  /*1: valid, 0: invalid */
    int type;                   /*boot/image/config */
#define PARTI_TYPE_BOOT 0x1
#define PARTI_TYPE_IMG  0x2
#define PARTI_TYPE_CONF 0x4
    unsigned int addrstart;
    unsigned int addrend;
    unsigned char cover_flag;
} partition_info;

typedef struct {
    unsigned int hl;
    unsigned char dataseqnum[4];
    unsigned char datacrc32[4];
    unsigned char imagelength[4];
    unsigned char imagecrc32[4];
    unsigned char package_id[22];
    unsigned char product_id[20];
    unsigned char image_data[1024];
#define MUP_DATA_ALIVE_ON 1
#define MUP_DATA_ALIVE_OFF 2
} __attribute__ ((__packed__)) MUTICAST_FRAME, *MUTICAST_FRAME_Pt;


//image size should not over 64MB, one packet size if 1024
#define FWSIZE 51200
unsigned char seqNumFlag[FWSIZE]= {0};

#define MAX_PARTITION   4
partition_info partiton_info_array[MAX_PARTITION];


__weak void led_multicastUP_rx_data(void) { return; }
__weak void led_multicastUP_rx_drop(void) { return; }
__weak void led_multicastUP_updating(void) { return; }
__weak void led_multicastUP_error_hang(void) { return; }
__weak void led_multicastUP_done(void) { return; }


static int multicast_frame_start = 0;
static int multicast_frame_alive = 0;
static int multicast_frame_stop = 0;
static unsigned int total_crc32 = 0;
static unsigned int total_len = 0;
static int multicast_frame_finish = 0;
static int multicast_upgrade_begin = 0;
static ulong mcastUpgradeTimeoutMSecs = 10000UL;
static int mcastUpgradeTimeoutCount = 0;
static int mcastUpgradeTimeoutCountMax = 120;
static ulong ImgLoadAddr = 0x8000000;   //Mem 128MB Address


static int multicastListAdd(unsigned char *data, unsigned int seq)
{
    char *load_buf = (char *) ImgLoadAddr;
    if((seq & 0xf)==0xf){
        printf("#");
        led_multicastUP_rx_data();
    }

    memcpy((load_buf + MUTLICAST_FRAME_LENGTH * (seq - 1)), data,
           MUTLICAST_FRAME_LENGTH);
    seqNumFlag[seq] = 1;
    return 1;
}

static void multicastListFree(void)
{
    memset(seqNumFlag, 0, sizeof(seqNumFlag));
    return;
}

static int multicastListFind(unsigned int seq)
{
    if (seqNumFlag[seq])
        return SEQNUM_EXIST;

    return SEQNUM_NOT_EXIST;
}

static void mcastUpgradeTimeout(void)
{
    if ((mcastUpgradeTimeoutCount++ < mcastUpgradeTimeoutCountMax) &&
        multicast_upgrade_begin &&
        !multicast_frame_stop &&
        (multicast_frame_start || multicast_frame_alive)) {
        multicast_upgrade_begin = 0;
        NetSetTimeout(mcastUpgradeTimeoutMSecs, mcastUpgradeTimeout);
    } else {
        printf("mcast upgrade timeout.(%d %d %d %d %d)\n",
               mcastUpgradeTimeoutCount, multicast_upgrade_begin,
               multicast_frame_stop, multicast_frame_start,
               multicast_frame_alive);
        multicastListFree();

        eth_halt();
        net_set_state(NETLOOP_FAIL);

        return;
    }
}

static int PrepareMulticastFrameLoadBuf(void)
{
    char *load_buf = (char *) ImgLoadAddr;
    unsigned int maxImageSeq;
    int ret = IMAGE_INTEGRITY;
    int lostCnt = 0;
    int index;

    maxImageSeq = total_len / MUTLICAST_FRAME_LENGTH;
    if (total_len % MUTLICAST_FRAME_LENGTH)
        maxImageSeq += 1;
    printf("\nLoad Address: %p total_len %d  maxImageSeq %d\n",
           load_buf, total_len, maxImageSeq);
    for (index = 1; index <= maxImageSeq; index++) {
        if (multicastListFind(index) == SEQNUM_NOT_EXIST) {
            printf("image not integrity: %d lost\n", index);
            lostCnt++;
            ret = IMAGE_NOT_INTEGRITY;
        }
    }
    if (IMAGE_NOT_INTEGRITY == ret)
        printf("image not integrity:Lost(%d)/Total(%d)\n", lostCnt,
               maxImageSeq);

    return ret;
}

static int processMultcastUpgrade(MUTICAST_FRAME_Pt p)
{
    unsigned int seqnum;
    unsigned int packet_imagelen;
    unsigned int packet_imagecrc;
    unsigned int packet_datacrc;
    unsigned int crc;

    seqnum = natohl(p->dataseqnum);
//    printf("[%s][%d] seq %d\n", __func__, __LINE__, seqnum);

    static int msg_flag = 1;
    if (seqnum > FWSIZE) {
        if (msg_flag && ++msg_flag > 10){
            msg_flag = 0;
            printf("\nPacket seqnum is ignored by exceeding the FWSIZE limit %d\n", FWSIZE);
        }
        //printf("Multicast pkt seq %d over Max %d.\n", seqnum, seqMax);
        return 0;
    }
    if(msg_flag && msg_flag > 1){
        msg_flag--;
    }
    packet_imagecrc = natohl(p->imagecrc32);
    packet_datacrc = natohl(p->datacrc32);
    packet_imagelen = natohl(p->imagelength);
//      printf("receive firmware packet image_len=0x%x image_crc=0x%x seq=%d total_crc32=0x%d total_len=0x%x type=%x len=0x%x\n",
//            packet_imagelen,packet_imagecrc,seqnum,total_crc32,total_len, p->datatype, p->datalen);
//      printf("receive firmware packet image_len=0x%x image_crc=0x%x seq=%d total_crc32=0x%x total_len=0x%x type=%x len=0x%x hl=0x%x\n",
//            packet_imagelen,packet_imagecrc,seqnum,total_crc32,total_len, p->hl >> 29, p->hl & 0x1fffffff, p->hl);

    //receive a mcast upgrade packet.
    multicast_upgrade_begin = 1;

    if ((ntohl(p->hl) & MUP_DATATYPE_MASK << 29) == MUP_DATATYPE_NORM << 29
        || (ntohl(p->hl) & MUP_DATATYPE_MASK << 29) ==
        MUP_DATATYPE_LAST << 29) {
        if (multicastListFind(seqnum) != SEQNUM_NOT_EXIST) {
            printf(".");
            led_multicastUP_rx_drop();
            return 0;
        }
    }

    crc = crc32(0, p->image_data, 1024);
    if (crc != packet_datacrc) {
        //printf("packet crc error: head crc32 %x calc crc32 %x. dropped!\n", packet_datacrc, crc);
        return 0;
    }
    if ((ntohl(p->hl) & MUP_DATATYPE_MASK << 29) ==
        MUP_DATATYPE_INFO << 29) {
        //printf("\nrecv header\n");
        if (!multicast_frame_start)
            multicast_frame_start = 1;

        if ((total_len != 0) && (total_crc32 != 0)) {
            if ((packet_imagecrc == total_crc32)
                && (total_len == packet_imagelen)) {
                printf("\nthe same packet receive .....\n");
                multicast_frame_finish = 1;
            }
        } else {
            //printf("=========================================================================================\n");
            total_len = packet_imagelen;
            total_crc32 = packet_imagecrc;
#if 0
            printf("\ntotal_len=%x total_crc32=%x\n", total_len,
                   total_crc32);
            printf("packet_imagelen=%x packet_imagecrc=%x\n",
                   packet_imagelen, packet_imagecrc);
#endif
            //printf("=========================================================================================\n");
        }
    }

    if (multicast_frame_finish) {
        multicast_frame_finish = 0;

        if (PrepareMulticastFrameLoadBuf() == IMAGE_INTEGRITY) {
            printf("complete multicast data frame ....\n");

            NetSetTimeout(0, mcastUpgradeTimeout);
            NetBootFileXferSize = total_len;
            multicastListFree();

            net_set_state(NETLOOP_SUCCESS);
            return 0;
        }

        return 0;
    }

    if ((ntohl(p->hl) & MUP_DATATYPE_MASK << 29) ==
        MUP_DATATYPE_INFO << 29) {
        int i = 0;
        partition_hdr *phdr = (partition_hdr *) p->image_data;

        multicast_frame_start = 1;
        /*parse the first pkt */
        memset(partiton_info_array, 0, sizeof(partiton_info_array));

        for (i = 0; i < MAX_PARTITION; i++) {
            if (!phdr->name[0]) {
                partiton_info_array[i].valid = 0;
                continue;
            }

//            printf("%s recved\n", phdr->name);
            partiton_info_array[i].valid = 1;
            if (strcmp(phdr->name, "bootloader") == 0)
                partiton_info_array[i].type |= PARTI_TYPE_BOOT;
            else if (strcmp(phdr->name, "kernel") == 0)
                partiton_info_array[i].type |= PARTI_TYPE_IMG;
            else if (strcmp(phdr->name, "conf") == 0)
                partiton_info_array[i].type |= PARTI_TYPE_CONF;
            //strcpy(partiton_info_array[i].name, phdr->name);
            partiton_info_array[i].addrstart = ntohl(phdr->addrstart);
            partiton_info_array[i].addrend = ntohl(phdr->addrend);
            partiton_info_array[i].cover_flag = phdr->cover_flag;
            phdr++;
        }
    }

    if ((ntohl(p->hl) & MUP_DATATYPE_MASK << 29) == MUP_DATATYPE_NORM << 29
        || (ntohl(p->hl) & MUP_DATATYPE_MASK << 29) ==
        MUP_DATATYPE_LAST << 29) {
        //printf("add seq %d\n", seqnum);
        multicastListAdd(p->image_data, seqnum);
        //multicast_frame_start =1;
    }

    if ((ntohl(p->hl) & MUP_DATATYPE_MASK << 29) == MUP_DATATYPE_NOP << 29) {   /*alive pkt */
        //printf("alive packet recv\n");
        //printf("image_data[0]=%d\n", p->image_data[0]);
        if (p->image_data[0] == MUP_DATA_ALIVE_ON) {
            multicast_frame_alive = 1;
            multicast_frame_stop = 0;
        } else if (p->image_data[0] == MUP_DATA_ALIVE_OFF) {
            multicast_frame_alive = 0;
            multicast_frame_stop = 1;
        }
    }

    return 0;
}

static void mcastUpgradeHandler(uchar * pkt, unsigned dest,
                                struct in_addr src_ip, unsigned src,
                                unsigned len)
{
    struct ip_udp_hdr *ip;
    int ret;
    struct in_addr dst_ip;
    ip = (struct ip_udp_hdr *) (pkt - IP_UDP_HDR_SIZE);
    //dst_ip = NetReadIP(&ip->ip_dst);
    dst_ip = net_read_ip((void *)&ip->ip_dst);
    if ((ntohl(dst_ip.s_addr) & IP_MCAST_MASK) != IP_MCAST) {
        //printf("%s dst ip is %#x\n", __func__, ntohl(dst_ip));
        return;
    }

    ret = processMultcastUpgrade((MUTICAST_FRAME_Pt) pkt);
    if (ret != 0)
        printf("%s ret != 0\n", __func__);
}

extern void ni_enter_promiscuous_mode(void);
extern int check_img_tar(unsigned char *buffer, unsigned int size);
void multicast_start(void)
{
    char *num;
    mcastUpgradeTimeoutCount = 0;
    multicast_frame_start = 0;
    multicast_frame_alive = 0;
    multicast_frame_stop = 0;
    multicast_frame_finish = 0;
    total_len = 0;
    total_crc32 = 0;
    ni_enter_promiscuous_mode();

    num = env_get("mcastUpgradeTimeoutMSecs");
    mcastUpgradeTimeoutMSecs =
        num ? simple_strtol(num, NULL, 10) : mcastUpgradeTimeoutMSecs;
    num = env_get("mcastUpgradeTimeoutCountMax");
    mcastUpgradeTimeoutCountMax =
        num ? simple_strtol(num, NULL, 10) : mcastUpgradeTimeoutCountMax;

    //memset(seqNumFlag, 0, sizeof(seqNumFlag));
    printf("%s Using %s device, mcastUpgradeTimeoutMSecs(%lu)\n", __func__, eth_get_name(), mcastUpgradeTimeoutMSecs/1000);
    //we should wait eth up first.
    NetSetTimeout(mcastUpgradeTimeoutMSecs, mcastUpgradeTimeout);
    net_set_udp_handler(mcastUpgradeHandler);
}
__weak int check_vm_img(unsigned char * buf, u32 total_len){
    return 0;
}
void multicast_stop(void)
{
    unsigned char *load_buf = (unsigned char *) ImgLoadAddr;
    char cmd_buf[256];
    unsigned int crc32_tmp;
    int i = 0;

    led_multicastUP_updating();
    printf("\nmulticast upgrade tool receive image OK ...\n");

    /* flush cache */
    flush_cache(ImgLoadAddr, total_len);

    crc32_tmp = crc32(0, load_buf, total_len);

    if (total_crc32 != crc32_tmp) {
        printf("crc32 error %x \n", crc32_tmp);
        led_multicastUP_error_hang();
    } else {
        total_crc32 = 0;

        printf("Writing file...\n\r");
        /*
         * "conf" is always behind image, so when amd29lvEraseSector will not erase "conf".
         */
        for (i = 0; i < MAX_PARTITION; i++) {
            if (!partiton_info_array[i].valid)
                continue;

            if (partiton_info_array[i].type == PARTI_TYPE_IMG) {
                printf("Going to write DTB/kernel/rootfs to flash\n");
                if (check_img_tar(load_buf, total_len))
                    sprintf(cmd_buf, "upimgtar 0x%lx %x", ImgLoadAddr,
                            total_len);
                else if (check_vm_img(load_buf, total_len))
                    sprintf(cmd_buf, "upvmimg 0x%lx", ImgLoadAddr);
                else {
                    printf("Unknown Image Format\n");
                    led_multicastUP_error_hang();
                    return;
                }
                run_command(cmd_buf, 0);
            } else if (partiton_info_array[i].type == PARTI_TYPE_BOOT) {
                printf("WARNING: NOT READY YET\n");
                run_command(cmd_buf, 0);
            } else if (partiton_info_array[i].type == PARTI_TYPE_CONF) {
                printf("Don't support config upgrade now!\n");
            }
        }
    }
    printf("Multicast Upgrade Done\n");
   led_multicastUP_done();
    return;
}
int multi_upgrade_enable(void){
    char *men;
    int mupgrade_en = 1;
    men = env_get("mupgrade_en");
    //if mupgrade_en not exist, enable multicase upgrade
    mupgrade_en = men ? simple_strtol(men, NULL, 10) : 1;
    return mupgrade_en;
}
