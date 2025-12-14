/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:
 * $Date: 2014-10-31 18:27:53 +0800 (Fri, 31 Oct 2014) $
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __PROTO_IGMP_PREVIEW_TIMER_H__
#define __PROTO_IGMP_PREVIEW_TIMER_H__


#include <sys_def.h>


#define MAX_TIMER 4096
typedef struct Timer *Timer_t;
typedef void (*timer_callback)(Timer_t, void*);
typedef struct TimerMgr *TimerMgr_t;
extern TimerMgr_t CreateTimerMgr();
extern void       DestroyTimerMgr(TimerMgr_t*); //Input argument is double pointer
//extern void       RunTimerMgr(TimerMgr_t);
extern void       RunTimerMgr(void *arg);
extern void       TerminateTimerMgr(TimerMgr_t); //Input argument is single pointer
extern int        AddTimer(TimerMgr_t, Timer_t);
extern int        RemoveTimer(TimerMgr_t, Timer_t);
extern Timer_t    CreateTimer(struct itimerspec*, timer_callback, void *arg);
extern void       DestroyTimer(Timer_t*);
extern void       DefaultInit(struct itimerspec*, long interval);
void previewTimerCB(Timer_t t, void *arg);
extern int32 igmp_mld_preview_timer_mgr_init(void);

#ifndef DEFAULT_TIMER
#define DEFAULT_TIMER(INTERVAL,CALLBACK,ARG) \
	({Timer_t __ret; \
		struct itimerspec __spec;\
		DefaultInit(&__spec,INTERVAL);\
		__ret = CreateTimer(&__spec,CALLBACK,ARG);\
		__ret;})
#endif

struct TimerMgr
{
	int epollfd;
	volatile int terminated;
};

struct Timer
{
	int   fd;
	void *arg;
	timer_callback callback;
};

typedef struct igmp_preview_timer_entry_s
{
	Timer_t pEntry;
	LIST_ENTRY(igmp_preview_timer_entry_s) entries;
}igmp_preview_timer_entry_t;


typedef struct timer_arg_s
{
	//TimerMgr_t mTimer;
	unsigned int id;
	unsigned long watchTimes;
	struct timespec nextPreviewTime;
	unsigned short previewReset;
}timer_arg_t;

LIST_HEAD(previewHead, igmp_preview_timer_entry_s);

extern struct previewHead previewTimerHead;

#endif /* __PROTO_IGMP_PREVIEW_TIMER_H__ */

