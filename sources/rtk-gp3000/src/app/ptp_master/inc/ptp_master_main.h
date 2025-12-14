#ifndef PTP_MASTER_MAIN_H
#define PTP_MASTER_MAIN_H

#define PTP_MASTER_MSG_KEY 1120
#define PTP_MAX_PKT_SIZE 256

#define MQ_ANNOUNCE_TIMEOUT 1
#define MQ_SYNC_TIMEOUT 2
#define MQ_RX_PACKET 3

#define MQ_PRI_MAX -5

//PTP message type
#define PTP_Sync                   0x0
#define PTP_Delay_Req              0x1
#define PTP_Pdelay_Req             0x2
#define PTP_Pdelay_Resp            0x3
#define PTP_Follow_Up              0x8
#define PTP_Delay_Resp             0x9
#define PTP_Pdelay_Resp_Follow_Up  0xA
#define PTP_Announce               0xB
#define PTP_Rx_Mirror              0xff

//PTP control message
#define PTP_CTRL_Sync        00
#define PTP_CTRL_Delay_Req   01
#define PTP_CTRL_Follow_Up   02
#define PTP_CTRL_Delay_Resp  03
#define PTP_CTRL_Management  04
#define PTP_CTRL_Others      05

//PTP packet offset without CPU tag
#define PTP_DA_offset         0
#define PTP_SA_offset         6
#define PTP_ETHERTYPE_offset  12
#define PTP_MTYPE_offset      14
#define PTP_VER_offset        15
#define PTP_MLEN_offset       16
#define PTP_DMAIN_offset      18
#define PTP_FLAG_offset       20
#define PTP_CFNS_offset       22
#define PTP_CFSUBNS_offset    28
#define PTP_CLOCKID_offset    34
#define PTP_SRCPORTID_offset  42
#define PTP_SQID_offset       44
#define PTP_CTRL_offset       46
#define PTP_MGPERIOD_offset   47
#define PTP_TSSEC_offset      48
#define PTP_TSNSEC_offset     54
#define PTP_REQCLOCKID_offset 58
#define PTP_REQPORTID_offset  66

//PTP announce offset without CPU tag
#define PTP_GM_PRI1_offset    61
#define PTP_GM_CQ_offset      62
#define PTP_GM_PRI2_offset    66
#define PTP_GM_ID_offset      67
#define PTP_GM_STEPRM_offset  75
#define PTP_GM_TIMESRC_offset 77

#define ANNOUNCE_LEN              78
#define SYN_LEN                   60
#define FOLLOW_UP_LEN             60
#define DELAY_REQ_LEN             58
#define DELAY_RESP_LEN            68
#define PDELAY_REQ_LEN            58
#define PDELAY_RESP_LEN           68
#define PDELAY_RESP_FOLLOW_UP_LEN 68

#define ANNOUNCE_INTER_TIME 3
#define SYNC_INTER_TIME 1

typedef enum 
{
    PTP_MODE_ONE_STEP,
    PTP_MODE_TWO_STEP,
}PTP_MODE_t;

typedef struct ptp_packet_s
{
    unsigned short pkt_len;
    unsigned char pkt[PTP_MAX_PKT_SIZE];
} ptp_packet_t;

typedef struct ptpMsgBuf_s
{
    long mtype;
    ptp_packet_t mtext;
    unsigned char reserve[16];
} ptpMsgBuf_t;

typedef struct ptpTimeMsgBuf_s
{
    long mtype;
    char mtext[4];
    unsigned char reserve[16];
} ptpTimeMsgBuf_t;


typedef struct ptp_pkt_cputag_s
{
    uint16 etherType;
    uint8 protocol;
    uint8 reason;
    uint16 word;
    uint8 sec[6];
    uint8 nanosec[4];
} ptp_pkt_cputag_t;


extern int debug;

#define DBG_PRINT(lvl,fmt,arg...)\
    do {\
        {\
          if(debug>=lvl) \
          printf(fmt,##arg);\
        }\
    } while (0)

#define RTK_API_ERR_CHK(op, ret)\
    do {\
        if ((ret = (op)) != RT_ERR_OK)\
        {\
            printf("[%s:%d] Rtk API Error! ret=%d \n", __FUNCTION__, __LINE__,ret); \
            return ret;\
        }\
    } while(0)

#define PROC_TRAP_WRITE(p1,p2)\
    do {\
        struct stat sb;\
        char cmd[64];\
        FILE *fp = NULL;\
        if (stat("/proc/ptp_drv",&sb) == 0 && S_ISDIR(sb.st_mode))\
        {\
            sprintf(cmd,"echo %d %d >> /proc/ptp_drv/rx_trap_uid",p1,p2);\
            fp = popen(cmd,"w");\
            if(fp == NULL)\
            { \
                printf("[%s:%d] unable to write on /proc/ptp_drv/rx_trap_uid \n", __FUNCTION__, __LINE__);\
                return RT_ERR_NULL_POINTER;\
            } \
            pclose(fp);\
        }\
        else\
        {\
            printf("[%s:%d] /proc/ptp_drv not exist \n", __FUNCTION__, __LINE__);\
            return RT_ERR_NULL_POINTER; \
        }\
    } while(0)

int32 ptp_announce_timeout_msg_send(void);
int32 ptp_sync_timeout_msg_send(void);
int32 ptp_rx_pkt_msg_send(ptpMsgBuf_t *pmsg_ptr);

#endif
