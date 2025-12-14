/*
 * Copyright (C) 2010 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 65149 $                                                           
 * $Date: 2016-01-15 16:09:00 +0800 (15 Jan 2016) $ 
 *
 * Purpose : RLDP
 *
 * Feature : RLDP
 *
 */
#ifndef __PROTO_RLDP_H__
#define __PROTO_RLDP_H__
#include "sys_def.h"
#include <common/rt_type.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <module/intr_bcaster/intr_bcaster.h>

#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif


/*Protocol version */
#define RLDP_PROTOCOL_ID              0x23

#define MAX_PAYLOAD                             (1600)

typedef
enum
rldp_status_s {
    RLDP_STS_LOOP_REMOVED = 0,
    RLDP_STS_LOOP_DETECTED,
    RLDP_STS_END
} rldp_status_t; 

typedef 
struct 
rldp_frame_s
{
    uint8 da[ETHER_ADDR_LEN];
    uint8 sa[ETHER_ADDR_LEN];
    uint8 eth_type[2];
    uint8 proto_id;
    uint8 random_id[6];
    uint8 user_id[6];
    uint8 tx_port;
    uint8 padding[36];
} rldp_frame_t;

typedef 
enum 
rldp_action_e
{
    RLDP_ACTION_LOG = 0,
    RLDP_ACTION_LED,
    RLDP_ACTION_BOTH,
    RLDP_ACTION_NONE,
} rldp_action_t;

typedef 
struct 
rldp_cfg_s
{
    rtk_enable_t enable;
    uint16 tx_interval;
    uint16 recovery_interval;
} rldp_cfg_t;


typedef 
struct 
rldp_port_sts_s
{
    osal_sem_t lock;
    rtk_port_t port;
    rtk_enable_t  enable;
    rldp_action_t action;
    uint32 tx_count;
    uint32 rx_count;
    uint32 loop_count;
    int32 timer;
    uint8 loop_status;
    uint8 isLinkup;
} rldp_port_sts_t;

typedef
struct
    cxn_tbl_s {
    int client_sock;
    struct cxn_tbl_s* next;
} cxn_tbl_t; 

int32
rldp_set (
    rldp_cfg_t* rldp );

int32 
__init
rldp_init (
    void);


#endif

