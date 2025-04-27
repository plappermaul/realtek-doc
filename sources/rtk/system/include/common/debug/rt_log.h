/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Debug Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SDK Debug Module
 *
 */

#ifndef __RT_LOG_H__
#define __RT_LOG_H__

/*
 * Include Files
 */
#ifdef __BOOTLOADER__

#else
#include <common/rt_autoconf.h>
#endif
#include <common/error.h>
#include <osal/print.h>

/*
 * Symbol Definition
 */
#if (defined(CONFIG_SDK_KERNEL_LINUX) && !defined(__KERNEL__))
        #define rt_log_printf(fmt, args...)    fprintf(stderr, fmt "\n", ## args)
#else
        #define rt_log_printf(fmt, args...)    osal_printf(fmt "\n", ## args)
#endif

#define RT_LOG_PLINE                           rt_log_printf("%s(%d):\n",__FUNCTION__,__LINE__);

/***************************************************************************************
 *  Two-dimension for debug information
 *
 *  Through the following two dimension, level and module, we can control the debug
 *  information at run time.
 *
 *  \    Level  |                          LOG LEVEL/MASK
 *   \________  |
 *   Module   \ | FATAL | MAJOR | MINOR | WARN | EVENT | INFO | FUNC | DEBUG | TRACE
 *  ___________\|_______|_______|_______|______|_______|______|______|_______|_______
 *              |       |       |       |      |       |      |      |       |
 *       VLAN   |       |       |       |      |       |      |      |       |
 *      --------+-------+-------+-------+------+-------+------+------+-------+-------
 *   M   L2     |       |       |       |      |       |      |      |       |
 *   O  --------+-------+-------+-------+------+-------+------+------+-------+-------
 *   D   ACL    |       |       |       |      |       |      |      |       |
 *      --------+-------+-------+-------+------+-------+------+------+-------+-------
 *   M   PHY    |       |       |       |      |       |      |      |       |
 *   A  --------+-------+-------+-------+------+-------+------+------+-------+-------
 *   S   QoS    |       |       |       |      |       |      |      |       |
 *   K  --------+-------+-------+-------+------+-------+------+------+-------+-------
 *       NIC    |       |       |       |      |       |      |      |       |
 *
 *  (1)LOG_LEVEL MODE
 *     For those enabled module, any message which level is smaller than LOG_LEVEL will
 *     be logged to the console. The larger we set the level, the more we could see.
 *     It's more like what Linux does.
 *
 *  (2)LOG_MASK MODE
 *     The table follows 'AND' operation: only the debug message with both log mask and
 *     module mask are 1 will be logged to the console.
 *
 ***************************************************************************************/

typedef enum log_lv_e
{
    LOG_FATAL_ERR = 0,          /* <0> Fatal error */
    LOG_MAJOR_ERR,              /* <1> Major unexpected event */
    LOG_MINOR_ERR,              /* <2> Minor unexpected event */
    LOG_WARNING,                /* <3> Warning message */
    LOG_EVENT,                  /* <4> Normal event */
    LOG_INFO,                   /* <5> Information */
    LOG_FUNC_ENTER,             /* <6> Entering function */
    LOG_DEBUG,                  /* <7> Debug message */
    LOG_TRACE,                  /* <8> Trace message for more detailed debug */
    LOG_MSG_OFF,                /* Level to turn off log information */
    LOG_LV_END
}log_lv_t;

#ifdef CONFIG_SDK_DEBUG_LOG_LEVEL
#define LOG_LEVEL_DEFAULT       CONFIG_SDK_DEBUG_LOG_LEVEL
#else
#define LOG_LEVEL_DEFAULT       LOG_WARNING
#endif

#define DBG_LEVEL_DEFAULT       LOG_DEBUG
#define ERR_LEVEL_DEFAULT       LOG_MINOR_ERR

#define LOG_MASK_OFF            0
#define LOG_MASK_ALL            ((1 << LOG_MSG_OFF) - 1)

#ifdef CONFIG_SDK_DEBUG_LOG_LEVEL_MASK
#define LOG_MASK_DEFAULT        CONFIG_SDK_DEBUG_LOG_LEVEL_MASK
#else
#define LOG_MASK_DEFAULT        ((1 << LOG_LEVEL_DEFAULT) - 1)
#endif

typedef enum log_type_e
{
    LOG_TYPE_LEVEL = 0,
    LOG_TYPE_MASK,
    LOG_TYPE_END
}log_type_t;

#ifdef CONFIG_SDK_DEBUG_LOG_TYPE_LEVEL_MASK
#define LOG_TYPE_DEFAULT        LOG_TYPE_MASK
#else
#define LOG_TYPE_DEFAULT        LOG_TYPE_LEVEL
#endif

typedef enum log_mod_e
{
    /* type by function */
    SDK_MOD_GENERAL = 0,        /* 0 */
    SDK_MOD_FLOWCTRL,           /* 1 */
    SDK_MOD_INIT,               /* 2 */
    SDK_MOD_L2,                 /* 3 */
    SDK_MOD_MIRROR,             /* 4 */
    SDK_MOD_NIC,                /* 5 */
    SDK_MOD_PORT,               /* 6 */
    SDK_MOD_QOS,                /* 7 */
    SDK_MOD_RATE,               /* 8 */
    SDK_MOD_STAT,               /* 9 */
    SDK_MOD_STP,                /* 10 */
    SDK_MOD_SWITCH,             /* 11 */
    SDK_MOD_TRAP,               /* 12 */
    SDK_MOD_TRUNK,              /* 13 */
    SDK_MOD_VLAN,               /* 14 */
    SDK_MOD_PIE,                /* 15 */

    /* type by component */
    SDK_MOD_HAL,                /* 16 */
    SDK_MOD_DAL,                /* 17 */
    SDK_MOD_RTDRV,              /* 18 */
    SDK_MOD_RTUSR,              /* 19 */
    SDK_MOD_DIAGSHELL,          /* 20 */
    SDK_MOD_UNITTEST,           /* 21 */

    SDK_MOD_OAM,                /* 22 */
    SDK_MOD_L3,                 /* 23 */
    SDK_MOD_RTCORE,             /* 24 */
    SDK_MOD_EEE,                /* 25 */
    SDK_MOD_SEC,                /* 26 */
    SDK_MOD_LED,                /* 27 */
    SDK_MOD_TIME,               /* 28 */
    SDK_MOD_ACL,                /* 29 */
    SDK_MOD_MPLS,               /* 30 */
    SDK_MOD_DIAG,               /* 31 */
    SDK_MOD_STACK,              /* 32 */
    SDK_MOD_COMMON,             /* 33 */
    SDK_MOD_TUNNEL,             /* 34 */
    SDK_MOD_OPENFLOW,           /* 35 */
    SDK_MOD_CAPWAP,             /* 36 */
    SDK_MOD_MCAST,              /* 37 */
    SDK_MOD_IPMCAST,            /* 38 */
    SDK_MOD_BPE,                /* 39 */
    SDK_MOD_BSP,                /* 40 */
    SDK_MOD_PHY,                /* 41 */
    SDK_MOD_VXLAN,              /* 42 */
    SDK_MOD_SDS,                /* 43 */

    /* end of module type */
    SDK_MOD_END
}log_mod_t;

#define MOD_GENERAL             (1ULL << SDK_MOD_GENERAL)       /* 0x00000001 */
#define MOD_FLOWCTRL            (1ULL << SDK_MOD_FLOWCTRL)      /* 0x00000002 */
#define MOD_INIT                (1ULL << SDK_MOD_INIT)          /* 0x00000004 */
#define MOD_L2                  (1ULL << SDK_MOD_L2)            /* 0x00000008 */
#define MOD_MIRROR              (1ULL << SDK_MOD_MIRROR)        /* 0x00000010 */
#define MOD_NIC                 (1ULL << SDK_MOD_NIC)           /* 0x00000020 */
#define MOD_PORT                (1ULL << SDK_MOD_PORT)          /* 0x00000040 */
#define MOD_QOS                 (1ULL << SDK_MOD_QOS)           /* 0x00000080 */
#define MOD_RATE                (1ULL << SDK_MOD_RATE)          /* 0x00000100 */
#define MOD_STAT                (1ULL << SDK_MOD_STAT)          /* 0x00000200 */
#define MOD_STP                 (1ULL << SDK_MOD_STP)           /* 0x00000400 */
#define MOD_SWITCH              (1ULL << SDK_MOD_SWITCH)        /* 0x00000800 */
#define MOD_TRAP                (1ULL << SDK_MOD_TRAP)          /* 0x00001000 */
#define MOD_TRUNK               (1ULL << SDK_MOD_TRUNK)         /* 0x00002000 */
#define MOD_VLAN                (1ULL << SDK_MOD_VLAN)          /* 0x00004000 */
#define MOD_PIE                 (1ULL << SDK_MOD_PIE)           /* 0x00008000 */
#define MOD_HAL                 (1ULL << SDK_MOD_HAL)           /* 0x00010000 */
#define MOD_DAL                 (1ULL << SDK_MOD_DAL)           /* 0x00020000 */
#define MOD_RTDRV               (1ULL << SDK_MOD_RTDRV)         /* 0x00040000 */
#define MOD_RTUSR               (1ULL << SDK_MOD_RTUSR)         /* 0x00080000 */
#define MOD_DIAGSHELL           (1ULL << SDK_MOD_DIAGSHELL)     /* 0x00100000 */
#define MOD_UNITTEST            (1ULL << SDK_MOD_UNITTEST)      /* 0x00200000 */
#define MOD_OAM                 (1ULL << SDK_MOD_OAM)           /* 0x00400000 */
#define MOD_L3                  (1ULL << SDK_MOD_L3)            /* 0x00800000 */
#define MOD_RTCORE              (1ULL << SDK_MOD_RTCORE)        /* 0x01000000 */
#define MOD_EEE                 (1ULL << SDK_MOD_EEE)           /* 0x02000000 */
#define MOD_SEC                 (1ULL << SDK_MOD_SEC)           /* 0x04000000 */
#define MOD_LED                 (1ULL << SDK_MOD_LED)           /* 0x08000000 */
#define MOD_TIME                (1ULL << SDK_MOD_TIME)          /* 0x10000000 */
#define MOD_ACL                 (1ULL << SDK_MOD_ACL)           /* 0x20000000 */
#define MOD_MPLS                (1ULL << SDK_MOD_MPLS)          /* 0x40000000 */
#define MOD_DIAG                (1ULL << SDK_MOD_DIAG)          /* 0x80000000 */
#define MOD_STACK               (1ULL << SDK_MOD_STACK)         /* 0x00000001-00000000 */
#define MOD_COMMON              (1ULL << SDK_MOD_COMMON)        /* 0x00000002-00000000 */
#define MOD_TUNNEL              (1ULL << SDK_MOD_TUNNEL)        /* 0x00000004-00000000 */
#define MOD_OPENFLOW            (1ULL << SDK_MOD_OPENFLOW)      /* 0x00000008-00000000 */
#define MOD_CAPWAP              (1ULL << SDK_MOD_CAPWAP)        /* 0x00000010-00000000 */
#define MOD_MCAST               (1ULL << SDK_MOD_MCAST)         /* 0x00000020-00000000 */
#define MOD_IPMCAST             (1ULL << SDK_MOD_IPMCAST)       /* 0x00000040-00000000 */
#define MOD_BPE                 (1ULL << SDK_MOD_BPE)           /* 0x00000080-00000000 */
#define MOD_BSP                 (1ULL << SDK_MOD_BSP)           /* 0x00000100-00000000 */
#define MOD_PHY                 (1ULL << SDK_MOD_PHY)           /* 0x00000200-00000000 */
#define MOD_VXLAN               (1ULL << SDK_MOD_VXLAN)         /* 0x00000400-00000000 */
#define MOD_SDS                 (1ULL << SDK_MOD_SDS)           /* 0x00000800-00000000 */

#define MOD_OFF                 0
#define MOD_ALL                 ((((uint64)1 << SDK_MOD_END)) - 1)

#ifdef CONFIG_SDK_DEBUG_LOG_MOD_MASK
#define MOD_MASK_DEFAULT        CONFIG_SDK_DEBUG_LOG_MOD_MASK
#else
#define MOD_MASK_DEFAULT        MOD_ALL
#endif

#define STR_MOD_GENERAL         "general"       /* SDK_MOD_GENERAL */
#define STR_MOD_FLOWCTRL        "flowctrl"      /* SDK_MOD_FLOWCTRL */
#define STR_MOD_INIT            "init"          /* SDK_MOD_INIT */
#define STR_MOD_L2              "l2"            /* SDK_MOD_L2 */
#define STR_MOD_MIRROR          "mirror"        /* SDK_MOD_MIRROR */
#define STR_MOD_NIC             "nic"           /* SDK_MOD_NIC */
#define STR_MOD_PORT            "port"          /* SDK_MOD_PORT */
#define STR_MOD_QOS             "qos"           /* SDK_MOD_QOS */
#define STR_MOD_RATE            "rate"          /* SDK_MOD_RATE */
#define STR_MOD_STAT            "stat"          /* SDK_MOD_STAT */
#define STR_MOD_STP             "stp"           /* SDK_MOD_STP */
#define STR_MOD_SWITCH          "switch"        /* SDK_MOD_SWITCH */
#define STR_MOD_TRAP            "trap"          /* SDK_MOD_TRAP */
#define STR_MOD_TRUNK           "trunk"         /* SDK_MOD_TRUNK */
#define STR_MOD_VLAN            "vlan"          /* SDK_MOD_VLAN */
#define STR_MOD_PIE             "pie"           /* SDK_MOD_PIE */
#define STR_MOD_HAL             "hal"           /* SDK_MOD_HAL */
#define STR_MOD_DAL             "dal"           /* SDK_MOD_DAL */
#define STR_MOD_RTDRV           "rtdrv"         /* SDK_MOD_RTDRV */
#define STR_MOD_RTUSR           "rtusr"         /* SDK_MOD_RTUSR */
#define STR_MOD_DIAGSHELL       "diagshell"     /* SDK_MOD_DIAGSHELL */
#define STR_MOD_UNITTEST        "unittest"      /* SDK_MOD_UNITTEST */
#define STR_MOD_OAM             "oam"           /* SDK_MOD_OAM */
#define STR_MOD_L3              "l3"            /* SDK_MOD_L3 */
#define STR_MOD_RTCORE          "rtcore"        /* SDK_MOD_RTCORE */
#define STR_MOD_EEE             "eee"           /* SDK_MOD_EEE */
#define STR_MOD_SEC             "sec"           /* SDK_MOD_SEC */
#define STR_MOD_LED             "led"           /* SDK_MOD_LED */
#define STR_MOD_TIME            "time"          /* SDK_MOD_TIME */
#define STR_MOD_ACL             "acl"           /* SDK_MOD_ACL */
#define STR_MOD_MPLS            "mpls"          /* SDK_MOD_MPLS */
#define STR_MOD_DIAG            "diag"          /* SDK_MOD_DIAG */
#define STR_MOD_STACK           "stack"         /* SDK_MOD_STACK */
#define STR_MOD_COMMON          "common"        /* SDK_MOD_COMMON */
#define STR_MOD_TUNNEL          "tunnel"        /* SDK_MOD_TUNNEL */
#define STR_MOD_OPENFLOW        "openflow"      /* SDK_MOD_OPENFLOW */
#define STR_MOD_CAPWAP          "capwap"        /* SDK_MOD_CAPWAP */
#define STR_MOD_MCAST           "mcast"         /* SDK_MOD_MCAST */
#define STR_MOD_IPMCAST         "ipmcast"       /* SDK_MOD_IPMCAST */
#define STR_MOD_BPE             "bpe"           /* SDK_MOD_BPE */
#define STR_MOD_BSP             "bsp"           /* SDK_MOD_BSP */
#define STR_MOD_PHY             "phy"           /* SDK_MOD_PHY */
#define STR_MOD_VXLAN           "vxlan"         /* SDK_MOD_VXLAN */
#define STR_MOD_SDS             "sds"           /* SDK_MOD_SDS */

#define STR_MOD_END             ""              /* SDK_MOD_END */


typedef enum log_format_e
{
    LOG_FORMAT_NORMAL = 0,
    LOG_FORMAT_DETAILED,
    LOG_FORMAT_END
}log_format_t;

#define LOG_FORMAT_DEFAULT      LOG_FORMAT_NORMAL

#define LOG_BUFSIZE_DEFAULT     1024

/*
 * Data Declaration
 */
extern uint32 log_initErrEn;
extern uint32 log_initMsgEn;


/*
 * Macro Declaration
 */

/* Undefine the macro */
#undef RT_LOG
#undef RT_ERR
#undef _RT_LOG
#undef _RT_DBG
#undef _RT_ERR

#if defined(__BOOTLOADER__)

  #define RT_LOG(level, module, args...)                    do {} while(0)
  #define RT_ERR(error_code, module, fmt, args...)          do { osal_printf("Err:0x%x:"fmt, error_code, ##args); } while (0)
  #define RT_INIT_ERR(error_code, module, fmt, args...)     do { osal_printf(fmt, ##args); } while (0)
  #define RT_INIT_MSG(fmt, args...)                         do { osal_printf(fmt, ##args); } while (0)

#elif defined(CONFIG_SDK_DEBUG)
  /* Macro for all layer debugging */
  #define RT_LOG(level, module, args...) \
      do { rt_log(level, module, ## args); } while (0)

  #if defined(__MODEL_USER__) || defined(CONFIG_X86_I2C)
      #define RT_ERR(error_code, module, args...) \
           do { if(RT_ERR_OK != error_code)\
              printf("*** [RT_ERR] %s:%d: In function '%s'\n             Error Code: 0x%X\n", \
                    __FILE__, __LINE__, __FUNCTION__, error_code); } while (0)
  #else
      #define RT_ERR(error_code, module, fmt, args...) \
           do { if(RT_ERR_OK != error_code)\
              if(RT_ERR_OK == rt_log(ERR_LEVEL_DEFAULT, module, "!rtkErr: "fmt, ## args)) \
                  { rt_log_printf("*** [RT_ERR] %s:%d: In function '%s'\n             Error Code: 0x%X\n", \
                   __FILE__, __LINE__, __FUNCTION__, error_code); } } while (0)
  #endif /* defined(__MODEL_USER__) || defined(CONFIG_X86_I2C) */

  #define RT_INIT_ERR(error_code, module, fmt, args...)   do { if (log_initErrEn) {RT_ERR(error_code, (MOD_INIT|module), fmt, ##args);} } while(0)
  #define RT_INIT_MSG(fmt, args...)                       do { if (log_initMsgEn) { osal_printf(fmt, ##args); } } while (0)
#else

  /* Turn off all messages for reducing code size at compile time */
  #define RT_LOG(level, module, args...)                  do {} while(0)
  #define RT_ERR(error_code, module, fmt, args...)        do {} while(0)
  #define RT_INIT_ERR(error_code, module, fmt, args...)   do {} while(0)
  #define RT_INIT_MSG(fmt, args...)                       do {} while(0)

#endif

/* Cancel individual debug message */
#define _RT_LOG(level, module, args...)                 do {} while(0)
#define _RT_DBG(level, module, args...)                 do {} while(0)
#define _RT_ERR(error_code, module, args...)            do {} while(0)

/* Macro for Internal Development */
#ifdef CONFIG_SDK_INTERNAL_PARAM_CHK_
#define RT_INTERNAL_PARAM_CHK(expr, errCode)            RT_PARAM_CHK(expr, errCode)
#else
#define RT_INTERNAL_PARAM_CHK(expr, errCode)            do {} while(0)
#endif

#define RT_CHIP_ID_DISPLAY(id)                          (id >> 16)

/* Macro for log module */
#define LOG_LEVEL_CHK(level)    ((level >= LOG_FATAL_ERR) && (level <= LOG_TRACE))
#define LOG_MASK_CHK(mask)      (mask <= LOG_MASK_ALL)
#define LOG_TYPE_CHK(type)      (type < LOG_TYPE_END)
#define LOG_MOD_CHK(mod_mask)   ((uint64)mod_mask <= MOD_ALL)
#define LOG_FORMAT_CHK(format)  ((format >= 0) && (format < LOG_FORMAT_END))

#define RT_LOG_PARAM_CHK(level, module)             \
do {                                                \
    if (log_init == INIT_NOT_COMPLETED)             \
        break;                                      \
    if (LOG_TYPE_LEVEL == log_type)                 \
    {                                               \
        if (LOG_MSG_OFF == log_level)               \
            return RT_ERR_NOT_ALLOWED;              \
        if ((LOG_MSG_OFF <= level) ||               \
                (log_level < level))                \
            return RT_ERR_NOT_ALLOWED;              \
    }                                               \
    else if (LOG_TYPE_MASK == log_type)             \
    {                                               \
        if (!(log_mask & (1 << level)))             \
            return RT_ERR_NOT_ALLOWED;              \
    }                                               \
    else                                            \
        return RT_ERR_FAILED;                       \
    if (!(log_module_mask & module))                \
        return RT_ERR_NOT_ALLOWED;                  \
} while(0)

#define RT_LOG_FORMATTED_OUTPUT(buf, format, result)\
do {                                                \
    va_list va;                                     \
    va_start(va, format);                           \
    result = osal_vsnprintf((char *)buf, sizeof(buf)-1,  \
                       format, va);                 \
    va_end(va);                                     \
} while(0)

#define RT_LOG_MODULE_STRING               \
    (uint8 *)STR_MOD_GENERAL,               /* SDK_MOD_GENERAL */           \
    (uint8 *)STR_MOD_FLOWCTRL,              /* SDK_MOD_FLOWCTRL */          \
    (uint8 *)STR_MOD_INIT,                  /* SDK_MOD_INIT */              \
    (uint8 *)STR_MOD_L2,                    /* SDK_MOD_L2 */                \
    (uint8 *)STR_MOD_MIRROR,                /* SDK_MOD_MIRROR */            \
    (uint8 *)STR_MOD_NIC,                   /* SDK_MOD_NIC */               \
    (uint8 *)STR_MOD_PORT,                  /* SDK_MOD_PORT */              \
    (uint8 *)STR_MOD_QOS,                   /* SDK_MOD_QOS */               \
    (uint8 *)STR_MOD_RATE,                  /* SDK_MOD_RATE */              \
    (uint8 *)STR_MOD_STAT,                  /* SDK_MOD_STAT */              \
    (uint8 *)STR_MOD_STP,                   /* SDK_MOD_STP */               \
    (uint8 *)STR_MOD_SWITCH,                /* SDK_MOD_SWITCH */            \
    (uint8 *)STR_MOD_TRAP,                  /* SDK_MOD_TRAP */              \
    (uint8 *)STR_MOD_TRUNK,                 /* SDK_MOD_TRUNK */             \
    (uint8 *)STR_MOD_VLAN,                  /* SDK_MOD_VLAN */              \
    (uint8 *)STR_MOD_PIE,                   /* SDK_MOD_PIE */               \
    (uint8 *)STR_MOD_HAL,                   /* SDK_MOD_HAL */               \
    (uint8 *)STR_MOD_DAL,                   /* SDK_MOD_DAL */               \
    (uint8 *)STR_MOD_RTDRV,                 /* SDK_MOD_RTDRV */             \
    (uint8 *)STR_MOD_RTUSR,                 /* SDK_MOD_RTUSR */             \
    (uint8 *)STR_MOD_DIAGSHELL,             /* SDK_MOD_DIAGSHELL */         \
    (uint8 *)STR_MOD_UNITTEST,              /* SDK_MOD_UNITTEST */          \
    (uint8 *)STR_MOD_OAM,                   /* SDK_MOD_OAM */               \
    (uint8 *)STR_MOD_L3,                    /* SDK_MOD_L3 */                \
    (uint8 *)STR_MOD_RTCORE,                /* SDK_MOD_RTCORE */            \
    (uint8 *)STR_MOD_EEE,                   /* SDK_MOD_EEE */               \
    (uint8 *)STR_MOD_SEC,                   /* SDK_MOD_SEC */               \
    (uint8 *)STR_MOD_LED,                   /* SDK_MOD_LED */               \
    (uint8 *)STR_MOD_TIME,                  /* SDK_MOD_TIME */              \
    (uint8 *)STR_MOD_ACL,                   /* SDK_MOD_ACL */               \
    (uint8 *)STR_MOD_MPLS,                  /* SDK_MOD_MPLS */              \
    (uint8 *)STR_MOD_DIAG,                  /* SDK_MOD_DIAG */              \
    (uint8 *)STR_MOD_STACK,                 /* SDK_MOD_STACK */             \
    (uint8 *)STR_MOD_COMMON,                /* SDK_MOD_COMMON */            \
    (uint8 *)STR_MOD_TUNNEL,                /* SDK_MOD_TUNNEL */            \
    (uint8 *)STR_MOD_OPENFLOW,              /* SDK_MOD_OPENFLOW */          \
    (uint8 *)STR_MOD_CAPWAP,                /* SDK_MOD_CAPWAP */            \
    (uint8 *)STR_MOD_MCAST,                 /* SDK_MOD_MCAST */             \
    (uint8 *)STR_MOD_IPMCAST,               /* SDK_MOD_IPMCAST */           \
    (uint8 *)STR_MOD_BPE,                   /* SDK_MOD_BPE */               \
    (uint8 *)STR_MOD_BSP,                   /* SDK_MOD_BSP */               \
    (uint8 *)STR_MOD_PHY,                   /* SDK_MOD_PHY */               \
    (uint8 *)STR_MOD_VXLAN,                 /* SDK_MOD_VXLAN */             \
    (uint8 *)STR_MOD_SDS,                   /* SDK_MOD_SDS */               \
    (uint8 *)STR_MOD_END                    /* SDK_MOD_END */


 /*
 * Function Declaration
 */

/* Function Name:
 *      rt_log_bootMsg_set
 * Description:
 *      Enable or disable the booting message or its error message
 * Input:
 *      initMsgEn   - enable/disable booting message
 *      initErrEn   - enable/disable booting error message
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *
 */
int32 rt_log_bootMsgEnable_set(rtk_enable_t initMsgEn, rtk_enable_t initErrEn);


/* Function Name:
 *      rt_log_init
 * Description:
 *      Initialize common log module
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Will be called from the init process of RTK module
 */
extern int32 rt_log_init(void);

/* Function Name:
 *      rt_log_enable_get
 * Description:
 *      Get the enable status of the log module
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of the enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_enable_get(uint32 *pEnable);

/* Function Name:
 *      rt_log_enable_set
 * Description:
 *      Set the enable status of the log module
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32 rt_log_enable_set(uint32 enable);

/* Function Name:
 *      rt_log_reset
 * Description:
 *      Reset the log module
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      Used to reset all configuration levels to the default values
 */
extern void rt_log_reset(void);

/* Function Name:
 *      rt_log_level_get
 * Description:
 *      Get the log level of the module
 * Input:
 *      None
 * Output:
 *      pLv - pointer buffer of the log level
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_level_get(uint32 *pLv);

/* Function Name:
 *      rt_log_level_set
 * Description:
 *      Set the log level of the module
 * Input:
 *      lv - log level
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
extern int32 rt_log_level_set(uint32 lv);

/* Function Name:
 *      rt_log_level_reset
 * Description:
 *      Reset the log level to default
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void rt_log_level_reset(void);

/* Function Name:
 *      rt_log_mask_get
 * Description:
 *      Get the log level mask of the module
 * Input:
 *      None
 * Output:
 *      pMask - pointer buffer of the log level mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_mask_get(uint32 *pMask);

/* Function Name:
 *      rt_log_mask_set
 * Description:
 *      Set the log level mask of the module
 * Input:
 *      mask - log level mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
extern int32 rt_log_mask_set(uint32 mask);

/* Function Name:
 *      rt_log_mask_reset
 * Description:
 *      Reset the log level mask to default
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void rt_log_mask_reset(void);

/* Function Name:
 *      rt_log_type_get
 * Description:
 *      Get the log type of the module
 * Input:
 *      None
 * Output:
 *      pType - pointer buffer of the log type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_type_get(uint32 *pType);

/* Function Name:
 *      rt_log_type_set
 * Description:
 *      Set the log type of the module
 * Input:
 *      type - log type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
extern int32 rt_log_type_set(uint32 type);

/* Function Name:
 *      rt_log_type_reset
 * Description:
 *      Reset the log type to default
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void rt_log_type_reset(void);

/* Function Name:
 *      rt_log_moduleMask_get
 * Description:
 *      Get the log module mask of the module
 * Input:
 *      None
 * Output:
 *      pMask - pointer buffer of the log module mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_moduleMask_get(uint64 *pMask);

/* Function Name:
 *      rt_log_moduleMask_set
 * Description:
 *      Set the log module mask of the module
 * Input:
 *      mask - log module mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
extern int32 rt_log_moduleMask_set(uint64 mask);

/* Function Name:
 *      rt_log_moduleMask_reset
 * Description:
 *      Reset the log module mask to default
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void rt_log_moduleMask_reset(void);

/* Function Name:
 *      rt_log_format_get
 * Description:
 *      Get the log format of the module
 * Input:
 *      None
 * Output:
 *      pFormat - pointer buffer of the log format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_format_get(uint32 *pFormat);

/* Function Name:
 *      rt_log_format_set
 * Description:
 *      Set the log format of the module
 * Input:
 *      format - log format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
extern int32 rt_log_format_set(uint32 format);

/* Function Name:
 *      rt_log_format_reset
 * Description:
 *      Reset the log format to default
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void rt_log_format_reset(void);

/* Function Name:
 *      rt_log_config_get
 * Description:
 *      Get the log config settings of the module
 * Input:
 *      None
 * Output:
 *      pCfg - pointer buffer of the log config settings
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 rt_log_config_get(uint32 *pCfg);

/* Function Name:
 *      rt_log_moduleName_get
 * Description:
 *      Get the string of the log module name
 * Input:
 *      module - module name
 * Output:
 *      None
 * Return:
 *      pointer buffer of the name string
 * Note:
 *      None
 */
extern uint8** rt_log_moduleName_get(uint64 module);

/* Function Name:
 *      rt_log
 * Description:
 *      Public API for all layers logging
 * Input:
 *      level   - log level
 *      module  - module name
 *      format - pointer buffer of the logging information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_ALLOWED - actions not allowed by the function
 * Note:
 *      None
 */
extern int32 rt_log(const int32 level, const int64 module, const char *format, ...);

#endif /* __RT_LOG_H__ */

