#ifndef TIME_SYNC_MONITOR_H
#define TIME_SYNC_MONITOR_H

#define TIME_SYNC_MSG_KEY 1133
#define TIME_SYNC_MAX_CMD_SIZE 8

#define TIME_SYNC_MQ_MONITOR 1
#define TIME_SYNC_MQ_CMD 2
#define TIME_SYNC_MQ_PRI_MAX -5

#define TIME_SYNC_MONITOR_TIMER_SIG SIGRTMIN+5
#define TIME_SYNC_MONITOR_TIME 5

#define GPS_UTC_DIFF_SEC 315964800

/*the same as intr_bcaster.h*/

typedef struct time_sync_cmd_s
{
    unsigned short len;
    unsigned char cmd[TIME_SYNC_MAX_CMD_SIZE];
} time_sync_cmd_t;

typedef struct timeSyncMsgBuf_s
{
    long mtype;
    time_sync_cmd_t mtext;
    unsigned char reserve[16];
} timeSyncMsgBuf_t;

void monitor_timer_init(int timeSec);
void *time_sync_monitorHandle(void *argu);

#endif
