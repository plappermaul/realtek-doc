/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * $Date:
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __SYS_DEF_H__
#define __SYS_DEF_H__

#include <stdarg.h>
#include <osal/lib.h>
#include <osal/print.h>

#include <osal/thread.h>
#include <osal/sem.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <osal/lib.h>

#include "sys_bitmap.h"
#include "sys_portmask.h"

#include <hal/common/halctrl.h>
#include <common/rt_error.h>
#include <rtk/acl.h>
#include <rtk/switch.h>
#include <rtk/irq.h>

#define SYS_MCAST_MAX_GROUP_NUM         512

#define SYS_IGMP_MAX_GROUP_NUM          256
#define SYS_MLD_MAX_GROUP_NUM           256

#define SYS_IGMP_PORT_LIMIT_ENTRY_NUM   64
#define SYS_MLD_PORT_LIMIT_ENTRY_NUM    256

#define MAX_ROUTER_VLAN                 512

#define MAC_ADDR_LEN                    6
#define IPV6_ADDR_LEN                   16

#define VLAN_BUF_LEN                    512

//#define RTK_PORT_MASK_WORD_MAX SYS_BITS_TO_LONGS(32)
#define RTK_PORT_MASK_WORD_MAX 1 

#define PRINT_LINE "---------------------------------\n"

typedef rtk_enable_t            sys_enable_t;
typedef rtk_mac_t               sys_mac_t;
typedef uint32                  sys_ipv4_addr_t;
typedef uint32                  sys_logic_port_t;
typedef int32                   sys_pri_t;
typedef uint8                   vid_list_t[VLAN_BUF_LEN];
typedef vid_list_t              sys_vlanmask_t;
typedef uint16                  sys_vid_t;

typedef 
enum {
    RTK_PKT_RLDP,
    RTK_PKT_TYPE_NUM
} rtk_pkt_type_t;
 
typedef 
enum {
    S_UPSTREAM   = 0,
    S_DOWNSTREAM,
    S_BOTH,
} rtk_pkt_dir_t; 
 
typedef enum {
    DST_FE   = 0,
    DST_DROP,
    DST_CPU,
} rtk_pkt_action_t; 


typedef enum sys_thread_pri_s
{
    SYS_THREAD_PRI_IGMP_TIMER           = 1,
    SYS_THREAD_PRI_IGMP_RATE            = 2,
    SYS_THREAD_PRI_IGMP_PREVIEW_TIMER   = 50,
    SYS_THREAD_PRI_IGMP_CFG             = 77
} sys_thread_pri_e;

typedef enum sys_err_code_s
{
    SYS_ERR_OK                          = 0,
    SYS_ERR_FAILED,
    SYS_ERR_NULL_POINTER,
    SYS_ERR_VLAN_ID,
    SYS_ERR_PORT_ID,
    SYS_ERR_L2_MAC_IS_EXIST,
    SYS_ERR_L2_MAC_FULL,
    SYS_ERR_MCAST_IPTYPE,
    SYS_ERR_INPUT,
    SYS_ERR_IGMP_GROUP_ENTRY_NOT_EXIST,
    SYS_ERR_IGMP_QUERIER_CHANGE,
    SYS_ERR_IGMP_PROFILE_ID,
    SYS_ERR_IGMP_PROFILE_NOT_EXIST,
    SYS_ERR_MCAST_DATABASE_FULL,
    SYS_ERR_MLD_PROFILE_NOT_EXIST,
    SYS_ERR_IPV4_ADDR,
    SYS_ERR_MCAST_GROUP_TYPE,
    SYS_ERR_IGMP_REMOVE_PORT_OK,
    SYS_ERR_IGMP_RATE_OVER
} sys_err_code_e;

typedef enum op_code_s
{
    SYS_OP_ADD,
    SYS_OP_UPDATE,
    SYS_OP_DEL
}op_code_t;

typedef struct sys_ipv6_addr_s
{
    uint8   ipv6_addr[IPV6_ADDR_LEN];
} sys_ipv6_addr_t;

typedef struct sys_logic_portmask_s
{
    uint32 bits[RTK_PORT_MASK_WORD_MAX];
} sys_logic_portmask_t;

typedef enum ponMode_s
{
    MODE_UNKOWN = 0,
    MODE_GPON,
    MODE_EPON,
    MODE_FIBER,
    MODE_END
}ponMode_t;

typedef struct igmp_mld_control_s
{
    uint32                      igmpMode;
    sys_logic_portmask_t        leaveMode; /* per port fast leave enable or disable */
    uint32                      controlType;
    ponMode_t                   ponMode;
    uint8                       logEnable;

    //sys_enable_t                igmpMldTrap_enable;
    sys_logic_portmask_t        igmpMldTrap_enable; /* per port trap contrl packet  */
    sys_enable_t                igmpv1Drop_enable;
    sys_enable_t                gmacMcastFilter_enable;
    sys_enable_t                igmpTagBehavior_enable;
    sys_enable_t                igmpVlanTag_type;

    sys_mac_t                   sys_mac; /* system MAC address */
    uint32                      sys_ip; /* system IP address */
    uint32                      sys_max_port_num;
} igmp_mld_control_t;

//extern igmp_mld_control_t  igmpCtrl;

#define DUMMY do {} while (0)

#define SYS_CPRINTF(fmt, args...) \
        do \
        { \
            FILE *con_fp = fopen("/dev/console", "w"); \
            if (con_fp) \
            { \
                fprintf(con_fp, fmt, ##args); \
                fclose(con_fp); \
            } \
        }while (0)

#ifdef  IGMP_DEBUG_SWITCH
#define SYS_PRINTF SYS_CPRINTF
#else
#define SYS_PRINTF(fmt, arg...)  \
        do {SYS_CPRINTF(fmt, #arg);} while (0)
// do { if (igmpCtrl.logEnable) { SYS_CPRINTF(fmt, ##arg);  } else { DUMMY; } } while (0)
#endif

#define LOG_DBG_IGMP IGMP_ALL_DEBUG_OFF
#define LOG_DBG_MLD  IGMP_ALL_DEBUG_OFF

#define SYS_DBG(module, fmt, arg...)\
        do {\
            {\
              SYS_PRINTF(fmt,##arg);\
            }\
        } while (0)

#define SYS_PARAM_CHK(expr, errCode)\
        do {\
            if ((int32)(expr)) {\
                SYS_PRINTF("\n SYS_PARAM_CHK %s %d\n",__FUNCTION__,__LINE__);\
                return errCode; \
            }\
        } while (0)

#define SYS_ERR_CHK(expr, errCode)\
        do {\
            if ((int32)(expr)) {\
                SYS_PRINTF("\n SYS_ERR_CHK %s %d\n",__FUNCTION__,__LINE__);\
                return errCode;\
            }\
        } while(0)

#define SYS_ERR_CONTINUE(ret) if(ret) continue

typedef struct sys_nic_pkt_s
{
    uint8* head;        /* pointer to the head of the packet data buffer */
    uint8* data;        /* pointer to the base address of the packet */
    uint8* tail;        /* pointer to the end address of the packet */
    uint8* end;         /* pointer to the end of the packet data buffer */
    uint32 length;      /* packet length when the packet is valid (not a empty data buffer) */
    void* buf_id;       /* pointer to the user-defined packet descriptor */

    uint8  as_txtag;    /* 0: without tx-tag, 1: with tx-tag */

    union {
        /* Notice:
           SOC layer use the field's name to do the filed copy, DO NOT change the field name
           or reverse the order of fields.
           The fields declared in struct rx/tx must be byte alignment.
        */

        /* Reception information */
        struct {
            uint8   source_port:6;      /* Source port number */
            uint8   priority:3;         /* Assigned priority */

            uint16  reason;             /* Reason to CPU */

            uint8   l2_error:1;         /* L2 CRC error. */
            uint8   l3_error:1;         /* IPv4 checksum error */
            uint8   l4_error:1;         /* TCP,UDP,ICMP,IGMP checksum error */
            uint8   pppoe:1;            /* Whether PPPoE header exists */
            uint8   svid_tagged:1;      /* Whether the SVID is tagged */
            uint8   cvid_tagged:1;      /* Whether the CVID is tagged (ethertype==0x8100) */
            uint8   drop_precedence:2;  /* Assigned drop precedence (RTL8389 unsupported) */

            uint8   l2_format:4;        /* Layer 2 format (RTL8389 unsupported) */
            uint8   l3_l4_format:4;     /* Layer 3/4 format (RTL8389 unsupported) */
            uint16  extra_tag:1;        /* Extra tag exist */
            uint16  outer_pri:3;        /* priority of outer tag */
            uint16  outer_vid:12;       /* vid of outer tag */
            uint16  inner_pri:3;        /* priority of inner tag */
            uint16  inner_vid:12;       /* vid of inner tag */
#if defined(CONFIG_SDK_SOFTWARE_RX_CPU_TAG)
            uint16  sw_valid:1;         /* 0: invalid, 1: valid */
            uint16  sw_index_type:4;    /* valid when sw_valid = 1, index type */
#define SW_TYPE_ACL     0
#define SW_TYPE_RMA     1
            uint16  sw_index:11;        /* valid when sw_valid = 1, mean acl or rma index */
#endif
        } rx_tag;
        /* Transmission information */
        struct {
            uint32  dst_port_mask:28;   /* DPM: When ADSPM is 1, DPM is the packet destination port
                                           mask. */
            uint32  as_dst_port_mask:1; /* ASDPM: Assign the destination port directly. */
            uint32  as_priority:1;      /* Assign priority and drop precedence directly. */
            uint32  as_port_remark:1;   /* ASPRMK: When ASPRMK is 1, the packet will be remarked
                                           based on port remarking setting. */
            uint32  l2_recalculate:1;   /* Recalculate the layer2 CRC */

            uint8   l3_recalculate:1;   /* Recalculate the IPv4 checksum (RTL8328 unsupported) */
            uint8   l4_recalculate:1;   /* Recalculate the TCP,UDP,ICMP,IGMP checksum (RTL8328 unsupported),
                                           Note: The checksum in TCP/UDP/ICMP/IGMP must be filled
                                           with 0x0000 if l4_recalculate is enabled*/
            uint8   drop_precedence:2;  /* Assigned drop precedence (RTL8389 unsupported) */
            uint8   priority:3;         /* Assigned priority */
            uint8   l2_learning:1;      /* Decide whether proceed L2 learning (RTL8328 unsupported) */
            uint32  flags;              /* Module enable flag (RTL8389 unsupported) */
        } tx_tag;
    };

    struct sys_nic_pkt_s* next;         /* pointer to next packet struct if it exist */
} sys_nic_pkt_t;

#endif
