/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
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
    SDK_MOD_DOT1X,              /* 1 */
    SDK_MOD_FILTER,             /* 2 */
    SDK_MOD_FLOWCTRL,           /* 3 */
    SDK_MOD_INIT,               /* 4 */
    SDK_MOD_L2,                 /* 5 */
    SDK_MOD_MIRROR,             /* 6 */
    SDK_MOD_NIC,                /* 7 */
    SDK_MOD_PORT,               /* 8 */
    SDK_MOD_QOS,                /* 9 */
    SDK_MOD_RATE,               /* 10 */
    SDK_MOD_STAT,               /* 11 */ 
    SDK_MOD_STP,                /* 12 */ 
    SDK_MOD_SVLAN,              /* 13 */ 
    SDK_MOD_SWITCH,             /* 14 */ 
    SDK_MOD_TRAP,               /* 15 */ 
    SDK_MOD_TRUNK,              /* 16 */ 
    SDK_MOD_VLAN,               /* 17 */
    SDK_MOD_ACL,                /* 18 */
    
    /* type by component */
    SDK_MOD_HAL,                /* 19 */
    SDK_MOD_DAL,                /* 20 */
    SDK_MOD_RTDRV,              /* 21 */
    SDK_MOD_RTUSR,              /* 22 */
    SDK_MOD_DIAGSHELL,          /* 23 */
    SDK_MOD_UNITTEST,           /* 24 */

    SDK_MOD_OAM,                /* 25 */
    SDK_MOD_L3,                 /* 26 */
    SDK_MOD_RTCORE,             /* 27 */
    SDK_MOD_EEE,                /* 28 */
    SDK_MOD_SEC,                /* 29 */
    SDK_MOD_LED,                /* 30 */
    SDK_MOD_RSVD_001,           /* 31, SDK_MOD_RSVD_001 */
    SDK_MOD_RSVD_002,           /* 32, SDK_MOD_RSVD_002 */
    SDK_MOD_RSVD_003,           /* 33, SDK_MOD_RSVD_003 */
    SDK_MOD_L34,                /* 34 */
    SDK_MOD_GPON,               /* 35 */
    SDK_MOD_EPON,               /* 36 */
    SDK_MOD_HWMISC,             /* 37 */
    SDK_MOD_CPU,                /* 38 */
    SDK_MOD_INTR,               /* 39 */
    SDK_MOD_PONMAC,             /* 40 */
    SDK_MOD_RLDP,               /* 41 */    
    SDK_MOD_CLASSIFY,           /* 42 */
    SDK_MOD_GPIO,               /* 43 */
    SDK_MOD_PTP,                /* 44 */
    SDK_MOD_PPSTOD,             /* 45 */
    SDK_MOD_PBO,                /* 46 */
    SDK_MOD_MDIO,               /* 47 */
    /* end of module type */
    SDK_MOD_END              
}log_mod_t;                  
                             
#define MOD_GENERAL             (1 << SDK_MOD_GENERAL)      /* 0x00000001 */        
#define MOD_DOT1X               (1 << SDK_MOD_DOT1X)        /* 0x00000002 */
#define MOD_FILTER              (1 << SDK_MOD_FILTER)       /* 0x00000004 */
#define MOD_FLOWCTRL            (1 << SDK_MOD_FLOWCTRL)     /* 0x00000008 */
#define MOD_INIT                (1 << SDK_MOD_INIT)         /* 0x00000010 */
#define MOD_L2                  (1 << SDK_MOD_L2)           /* 0x00000020 */
#define MOD_MIRROR              (1 << SDK_MOD_MIRROR)       /* 0x00000040 */
#define MOD_NIC                 (1 << SDK_MOD_NIC)          /* 0x00000080 */
#define MOD_PORT                (1 << SDK_MOD_PORT)         /* 0x00000100 */
#define MOD_QOS                 (1 << SDK_MOD_QOS)          /* 0x00000200 */
#define MOD_RATE                (1 << SDK_MOD_RATE)         /* 0x00000400 */
#define MOD_STAT                (1 << SDK_MOD_STAT)         /* 0x00000800 */
#define MOD_STP                 (1 << SDK_MOD_STP)          /* 0x00001000 */
#define MOD_SVLAN               (1 << SDK_MOD_SVLAN)        /* 0x00002000 */
#define MOD_SWITCH              (1 << SDK_MOD_SWITCH)       /* 0x00004000 */
#define MOD_TRAP                (1 << SDK_MOD_TRAP)         /* 0x00008000 */
#define MOD_TRUNK               (1 << SDK_MOD_TRUNK)        /* 0x00010000 */
#define MOD_VLAN                (1 << SDK_MOD_VLAN)         /* 0x00020000 */
#define MOD_ACL                 (1 << SDK_MOD_ACL)          /* 0x00040000 */
#define MOD_HAL                 (1 << SDK_MOD_HAL)          /* 0x00080000 */
#define MOD_DAL                 (1 << SDK_MOD_DAL)          /* 0x00100000 */
#define MOD_RTDRV               (1 << SDK_MOD_RTDRV)        /* 0x00200000 */
#define MOD_RTUSR               (1 << SDK_MOD_RTUSR)        /* 0x00400000 */
#define MOD_DIAGSHELL           (1 << SDK_MOD_DIAGSHELL)    /* 0x00800000 */
#define MOD_UNITTEST            (1 << SDK_MOD_UNITTEST)     /* 0x01000000 */
#define MOD_OAM                 (1 << SDK_MOD_OAM)          /* 0x02000000 */
#define MOD_L3                  (1 << SDK_MOD_L3)           /* 0x04000000 */
#define MOD_RTCORE              (1 << SDK_MOD_RTCORE)       /* 0x08000000 */
#define MOD_EEE                 (1 << SDK_MOD_EEE)          /* 0x10000000 */
#define MOD_SEC                 (1 << SDK_MOD_SEC)          /* 0x20000000 */
#define MOD_LED                 (1 << SDK_MOD_LED)          /* 0x40000000 */
#define MOD_RSVD_001            (1 << SDK_MOD_RSVD_001)     /* 0x80000000 */
#define MOD_RSVD_002            ((uint64)1 << SDK_MOD_RSVD_002) /* 0x00000001-00000000 */
#define MOD_RSVD_003            ((uint64)1 << SDK_MOD_RSVD_003) /* 0x00000002-00000000 */
#define MOD_L34                 ((uint64)1 << SDK_MOD_L34)      /* 0x00000004-00000000 */
#define MOD_GPON                ((uint64)1 << SDK_MOD_GPON)     /* 0x00000008-00000000 */
#define MOD_EPON                ((uint64)1 << SDK_MOD_EPON)     /* 0x00000010-00000000 */
#define MOD_HWMISC              ((uint64)1 << SDK_MOD_HWMISC)   /* 0x00000020-00000000 */
#define MOD_CPU                 ((uint64)1 << SDK_MOD_CPU)      /* 0x00000040-00000000 */
#define MOD_INTR                ((uint64)1 << SDK_MOD_INTR)     /* 0x00000080-00000000 */
#define MOD_PONMAC              ((uint64)1 << SDK_MOD_PONMAC)   /* 0x00000100-00000000 */
#define MOD_RLDP                ((uint64)1 << SDK_MOD_RLDP)     /* 0x00000200-00000000 */
#define MOD_CLASSIFY            ((uint64)1 << SDK_MOD_CLASSIFY) /* 0x00000400-00000000 */
#define MOD_GPIO		   ((uint64)1 << SDK_MOD_GPIO) /* 0x00000800-00000000 */
#define MOD_PTP                 ((uint64)1 << SDK_MOD_PTP)      /* 0x00001000-00000000 */
#define MOD_PPSTOD              ((uint64)1 << SDK_MOD_PPSTOD)   /* 0x00002000-00000000 */
#define MOD_PBO                 ((uint64)1 << SDK_MOD_PBO)      /* 0x00004000-00000000 */
#define MOD_MDIO                ((uint64)1 << SDK_MOD_MDIO)     /* 0x00008000-00000000 */
#define MOD_OFF                 0
#define MOD_ALL                 ((((uint64)1 << SDK_MOD_END)) - 1)





#ifdef CONFIG_SDK_DEBUG_LOG_MOD_MASK
#define MOD_MASK_DEFAULT        CONFIG_SDK_DEBUG_LOG_MOD_MASK
#else
#define MOD_MASK_DEFAULT        MOD_ALL
#endif

#define STR_MOD_GENERAL         "general"                     
#define STR_MOD_DOT1X           "dot1x"    
#define STR_MOD_FILTER          "filter"    
#define STR_MOD_FLOWCTRL        "flowctrl"    
#define STR_MOD_INIT            "init"    
#define STR_MOD_L2              "l2"    
#define STR_MOD_MIRROR          "mirror"    
#define STR_MOD_NIC             "nic"    
#define STR_MOD_PORT            "port"    
#define STR_MOD_QOS             "qos"    
#define STR_MOD_RATE            "rate"    
#define STR_MOD_STAT            "stat"    
#define STR_MOD_STP             "stp"    
#define STR_MOD_SVLAN           "svlan"    
#define STR_MOD_SWITCH          "switch"    
#define STR_MOD_TRAP            "trap"    
#define STR_MOD_TRUNK           "trunk"    
#define STR_MOD_VLAN            "vlan"    
#define STR_MOD_ACL             "acl"
#define STR_MOD_HAL             "hal"    
#define STR_MOD_DAL             "dal"    
#define STR_MOD_RTDRV           "rtdrv"    
#define STR_MOD_RTUSR           "rtusr"    
#define STR_MOD_DIAGSHELL       "diagshell"
#define STR_MOD_UNITTEST        "unittest"
#define STR_MOD_OAM             "oam"
#define STR_MOD_L3              "l3"
#define STR_MOD_RTCORE          "rtcore"
#define STR_MOD_EEE             "eee"
#define STR_MOD_SEC             "sec"
#define STR_MOD_LED             "led"
#define STR_MOD_RSVD_001        "rsvd_001"
#define STR_MOD_RSVD_002        "rsvd_002"
#define STR_MOD_RSVD_003        "rsvd_003"
#define STR_MOD_L34             "L34"
#define STR_MOD_GPON            "GPON"
#define STR_MOD_EPON            "EPON"
#define STR_MOD_HWMISC          "hw misc"
#define STR_MOD_CPU             "CPU"
#define STR_MOD_INTR            "interrupt"
#define STR_MOD_PONMAC          "PON MAC"
#define STR_MOD_RLDP            "rldp"
#define STR_MOD_CLASSIFY        "classify"
#define STR_MOD_GPIO		    "gpio"
#define STR_MOD_PTP		    "PTP"
#define STR_MOD_PPSTOD		    "ppstod"
#define STR_MOD_PBO		    "pbo"
#define STR_MOD_MDIO		    "mdio"
#define STR_MOD_END             ""

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

/*
 * Macro Declaration
 */
 
/* Undefine the macro */
#undef RT_LOG
#undef RT_DBG 
#undef RT_ERR    
#undef _RT_LOG
#undef _RT_DBG
#undef _RT_ERR   

#ifdef CONFIG_SDK_DEBUG

/* Macro for all layer debugging */    
#define RT_LOG(level, module, args...) \
    do { rt_log(level, module, ## args); } while (0)       
   
#define RT_DBG(level, module, args...) \
    do { if(RT_ERR_OK == rt_log(level, module, ## args)) \
            { rt_log_printf("*** [RT_DBG] %s:%d: In function '%s'\n", __FILE__, \
             __LINE__, __FUNCTION__); } } while (0)

#define RT_ERR(error_code, module, args...) \
    do { if(RT_ERR_OK == rt_log(ERR_LEVEL_DEFAULT, module, ## args)) \
            { rt_log_printf("*** [RT_ERR] %s:%d: In function '%s'\n             Error Code: 0x%X\n", \
             __FILE__, __LINE__, __FUNCTION__, error_code); } } while (0)
#else

#if defined(CONFIG_LUNA_G3_SERIES) && defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/kernel.h>
#include <linux/string.h>
extern uint32 ca_rtk_debug;
#define RT_LOG(level, module, args...) if(ca_rtk_debug & 0x200) {printk("\033[1;31m RT_LOG %s %d \033[m\n",__FUNCTION__,__LINE__);}
#define RT_DBG(level, module, args...) if(ca_rtk_debug & 0x100) {printk("\033[1;31m RT_DBG %s %d \033[m\n",__FUNCTION__,__LINE__);}
#define RT_ERR(error_code, module, args...) if(ca_rtk_debug & 0x80) {printk("\033[1;31m RT_ERR %s %d \033[m\n",__FUNCTION__,__LINE__);}
#else
/* Turn off all messages for reducing code size at compile time */
#define RT_LOG(level, module, args...) do {} while(0)
#define RT_DBG(level, module, args...) do {} while(0)
#define RT_ERR(error_code, module, args...) do {} while(0)
#endif

#endif

/* Cancel individual debug message */
#define _RT_LOG(level, module, args...) do {} while(0)   
#define _RT_DBG(level, module, args...) do {} while(0)
#define _RT_ERR(error_code, module, args...) do {} while(0)     

/* Macro for Internal Development */
#ifdef CONFIG_SDK_INTERNAL_PARAM_CHK_
#define RT_INTERNAL_PARAM_CHK(expr, errCode)    RT_PARAM_CHK(expr, errCode)
#else
#define RT_INTERNAL_PARAM_CHK(expr, errCode)    do {} while(0)
#endif

/* Macro for log module */
#define LOG_LEVEL_CHK(level)    (level <= LOG_TRACE)
#define LOG_MASK_CHK(mask)      (mask <= LOG_MASK_ALL)
#define LOG_TYPE_CHK(type)      (type < LOG_TYPE_END)
#define LOG_MOD_CHK(mod_mask)   ((uint64)mod_mask <= MOD_ALL)
#define LOG_FORMAT_CHK(format)  (format < LOG_FORMAT_END)

#define RT_LOG_PARAM_CHK(level, module)             \
do {                                                \
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
    result = vsnprintf((char *)buf, (size_t)(sizeof(buf)-1),          \
                       format, va);                 \
    va_end(va);                                     \
} while(0)

 /*
 * Function Declaration
 */
 
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

