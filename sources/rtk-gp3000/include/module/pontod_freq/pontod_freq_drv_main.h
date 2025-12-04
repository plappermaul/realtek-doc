/*
 * Copyright (C) 2017 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PON ToD tune Time Frequency kernel drivers
 *
 * Feature : This module install PON ToD tune Time Frequency callbacks and other 
 * kernel functions
 */

#ifndef __PONTOD_FREQ_DRV_MAIN_H__
#define __PONTOD_FREQ_DRV_MAIN_H__

#define PONTOD_FREQ_NETLINK_TYPE 31

typedef struct PonToDTimeTick_s {
    uint32 S_tick;
    uint64 S_time;
    uint32 F_tick;
    uint64 F_time;
    uint32 E_tick;
    uint64 E_time;
}PonToDTimeTick_t;

typedef struct PonToDTimeIntr_s {
    uint32 S_tick;
    uint64 S_time;
    uint32 E_tick;
    uint64 E_time;
    uint64 SS_time;
    uint64 ES_time;
    uint64 SF_time;
    uint64 EF_time;
}PonToDTimeIntr_t;

typedef struct PonToDTimeAuto_s {
    uint64 SS_time;
    uint64 ES_time;
    uint64 SF_time;
    uint64 EF_time;
}PonToDTimeAuto_t;

typedef enum PonToD_Type_e
{
    TYPE_PONTOD_PID_REG,
    TYPE_PONTOD_PID_REG_ACK,
    TYPE_PONTOD_PID_DREG,
    TYPE_PONTOD_TIME_TICK,
    TYPE_PONTOD_TIME_INTR,
    TYPE_PONTOD_TIME_AUTO,
    TYPE_PONTOD_RECOLLECT,
    TYPE_PONTOD_SET_FREQ,
} PonToD_Type_t;

typedef struct PonToDNLData_s {
    PonToD_Type_t type;
    union {
        int pid;
        PonToDTimeTick_t timeTick;
        PonToDTimeIntr_t timeIntr;
        PonToDTimeAuto_t timeAuto;
    }val;
}PonToDNLData_t;

#if defined(CONFIG_SDK_KERNEL_LINUX)
extern void pontod_freq_intr_tod_update(void);
extern void pontod_freq_tod_update_time(rtk_pon_tod_t tod_time);
extern void pontod_freq_tod_update_enable(void);
static int nl_send(char *data,uint16 dataLen);
static void nl_recv(struct sk_buff *skb);
#endif

#endif
