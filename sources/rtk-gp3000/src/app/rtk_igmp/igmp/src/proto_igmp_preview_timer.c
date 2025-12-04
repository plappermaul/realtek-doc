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
#include <igmp/inc/proto_igmp.h>

struct previewHead previewTimerHead;
TimerMgr_t tMgr;

TimerMgr_t CreateTimerMgr(void)
{
	//int epollfd = TEMP_FAILURE_RETRY(epoll_create(MAX_TIMER));
	int epollfd = epoll_create(MAX_TIMER);

	if (epollfd >= 0)
	{
		TimerMgr_t t = osal_alloc(sizeof(*t));
		t->epollfd = epollfd;
		return t;
	}

	return NULL;
}

void DestroyTimerMgr(TimerMgr_t *t)
{
	close((*t)->epollfd);
	osal_free(*t);
	*t = 0;
}

void TerminateTimerMgr(TimerMgr_t t)
{
	t->terminated = 1;
}

//void RunTimerMgr(TimerMgr_t t){
void RunTimerMgr(void *arg)
{
    long long           tmp;
    struct epoll_event  *events;
    timer_arg_t         *pTimerArg = NULL;
	char                *name = "IGMP_PREVIEW";

    TimerMgr_t t = (TimerMgr_t)arg;
	prctl(PR_SET_NAME, (unsigned long)name);

	events = osal_alloc (sizeof (struct epoll_event) * MAX_TIMER);
	t->terminated = 0;

	while (!t->terminated)
	{
		//int nfds = TEMP_FAILURE_RETRY(epoll_wait(t->epollfd,events,MAX_TIMER,-1));
		int nfds = epoll_wait(t->epollfd, events, MAX_TIMER, -1);

        if (nfds < 0)
		{
			t->terminated = 1;
			break;
		}

		int i;

		for (i = 0 ; i < nfds; ++i)
		{
			Timer_t _timer = (Timer_t)events[i].data.ptr;

            pTimerArg = (timer_arg_t *)(_timer->arg);

            printf("%s() nfds=%d, id=%x\n", __FUNCTION__, nfds, pTimerArg->id);

            if ( -1 == (read(_timer->fd,&tmp,sizeof(tmp))))
                printf("read error: %s\n", strerror(errno));

            if (_timer->callback)
            {
				_timer->callback(_timer,_timer->arg);
            }
        }
    }
}

int AddTimer(TimerMgr_t t, Timer_t _timer)
{
	struct epoll_event  ev;
	ev.data.ptr = _timer;
 	//ev.data.fd = _timer->fd;
	ev.events =  EPOLLIN | EPOLLOUT;
	//TEMP_FAILURE_RETRY(ret = epoll_ctl(t->epollfd,EPOLL_CTL_ADD,_timer->fd,&ev));
 	if (0 != epoll_ctl(t->epollfd, EPOLL_CTL_ADD, _timer->fd, &ev))
    {
 		return -1;
    }
    return 0;
}

int RemoveTimer(TimerMgr_t t, Timer_t _timer)
{
	struct epoll_event ev;
	//TEMP_FAILURE_RETRY(ret = epoll_ctl(t->epollfd,EPOLL_CTL_DEL,_timer->fd,&ev));
	if (0 != epoll_ctl(t->epollfd, EPOLL_CTL_DEL, _timer->fd, &ev))
    {
		return -1;
    }
    return 0;
}

void DefaultInit(struct itimerspec *new_value, long interval)
{
	//it_value and it_interval are zero is stop timer
	//struct timespec now;
	//clock_gettime(/*CLOCK_REALTIME*/CLOCK_MONOTONIC, &now);
	int sec = interval / 1000;
	int ms = interval % 1000;
	//long long nosec = (now.tv_sec + sec) * 1000 * 1000 * 1000 + now.tv_nsec + ms * 1000 * 1000;
	//TBD: the first timeout time = get current time + timeout time
	new_value->it_value.tv_sec = sec;
	new_value->it_value.tv_nsec = ms * 1000* 1000;
	//periodical timeout time
	new_value->it_interval.tv_sec = sec;
	new_value->it_interval.tv_nsec = ms * 1000* 1000;
}

Timer_t CreateTimer(struct itimerspec *spec, timer_callback callback, void *arg)
{
	int fd = timerfd_create(/*CLOCK_REALTIME*/CLOCK_MONOTONIC, 0);

    if (fd < 0)
    {
		return NULL;
    }

    Timer_t t = osal_alloc(sizeof(*t));

    if (!t)
	{
		close(fd);
		return NULL;
	}

    t->callback = callback;
	t->fd       = fd;
	t->arg      = arg;

    timerfd_settime(fd, TFD_TIMER_ABSTIME, spec, 0);

	return t;
}

void DestroyTimer(Timer_t *t)
{
	(*t)->callback = NULL;
	osal_free((*t)->arg);
	close((*t)->fd);
	osal_free(*t);
	*t = 0;
}

void previewTimerCB(Timer_t pTimer, void *arg)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    printf("%s() %u\n", __FUNCTION__, __LINE__);

    timer_arg_t *pArg = (timer_arg_t *)arg;

	if (0 ==  pArg->previewReset || 255 == pArg->previewReset)
	{
		//Do not reset the preview repeat counter automatically
	}
	else
	{
		pArg->watchTimes = 0;
	}
}

int32 igmp_mld_preview_timer_mgr_init(void)
{
    SYS_ERR_CHK((!(tMgr = CreateTimerMgr())), SYS_ERR_FAILED);

    if ((osal_thread_t)NULL == (igmp_thread_create("IGMP preview timer Thread", STACK_SIZE,
        SYS_THREAD_PRI_IGMP_PREVIEW_TIMER, (void *)RunTimerMgr, (void *)tMgr)))
    {
        SYS_PRINTF("IGMP Tick Thread create failed\n");
        return SYS_ERR_FAILED;
    }
    return SYS_ERR_OK;
}
