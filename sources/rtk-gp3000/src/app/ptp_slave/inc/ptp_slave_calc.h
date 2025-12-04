#ifndef PTP_SLAVE_CALC_H
#define PTP_SLAVE_CALC_H

#define PTP_SLAVE_CALC_MSG_KEY 1122

#define MQ_SLAVE_DELAY_TIME 1

#define MQ_SLAVE_F_PRI_MAX -3

typedef struct ptp_delayTime_s
{
    ptp_time_t t1;
    ptp_time_t t2;
    ptp_time_t t3;
    ptp_time_t t4;
} ptp_delayTime_t;

typedef struct ptpFDMsgBuf_s
{
    long mtype;
    ptp_delayTime_t mtext;
    unsigned char reserve[16];
} ptpFDMsgBuf_t;

extern rtk_enable_t adj_freq;
extern uint8 sample_num;
extern rtk_enable_t adj_offset;

int32 ptp_delay_msg_send(ptpFDMsgBuf_t *ptpFDMsgBuf_ptr);
void *ptp_slave_calcThread(void *argu);
void *ptp_slave_ppsThread(void *argu);

#endif
