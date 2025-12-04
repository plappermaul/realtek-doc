#ifndef TIME_SYNC__INTR_H
#define TIME_SYNC__INTR_H

#define TIME_SYNC_TOD_UPDATE_TIMER_SIG SIGRTMIN+7
#define TIME_SYNC_TOD_UPDATE_TIME 5

void *time_sync_intrHandle(void *argu);
#endif
