/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Debug Module 
 * 
 * Feature : The file have include the following module and sub-modules
 *           1) SDK Debug Module for Linux User Mode
 * 
 */

/*
 * Include Files
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <rtk/rtusr/include/rtusr_util.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
uint8 *rtLogModuleName[] = 
{
    (uint8 *)STR_MOD_GENERAL,               /*  SDK_MOD_GENERAL,            0 */                    
    (uint8 *)STR_MOD_DOT1X,                 /*  SDK_MOD_DOT1X,              1 */                    
    (uint8 *)STR_MOD_FILTER,                /*  SDK_MOD_FILTER,             2 */                    
    (uint8 *)STR_MOD_FLOWCTRL,              /*  SDK_MOD_FLOWCTRL,           3 */                    
    (uint8 *)STR_MOD_INIT,                  /*  SDK_MOD_INIT,               4 */                    
    (uint8 *)STR_MOD_L2,                    /*  SDK_MOD_L2,                 5 */                    
    (uint8 *)STR_MOD_MIRROR,                /*  SDK_MOD_MIRROR,             6 */                    
    (uint8 *)STR_MOD_NIC,                   /*  SDK_MOD_NIC,                7 */                    
    (uint8 *)STR_MOD_PORT,                  /*  SDK_MOD_PORT,               8 */                    
    (uint8 *)STR_MOD_QOS,                   /*  SDK_MOD_QOS,                9 */                    
    (uint8 *)STR_MOD_RATE,                  /*  SDK_MOD_RATE,               10 */                   
    (uint8 *)STR_MOD_STAT,                  /*  SDK_MOD_STAT,               11 */                   
    (uint8 *)STR_MOD_STP,                   /*  SDK_MOD_STP,                12 */                   
    (uint8 *)STR_MOD_SVLAN,                 /*  SDK_MOD_SVLAN,              13 */                   
    (uint8 *)STR_MOD_SWITCH,                /*  SDK_MOD_SWITCH,             14 */                   
    (uint8 *)STR_MOD_TRAP,                  /*  SDK_MOD_TRAP,               15 */                   
    (uint8 *)STR_MOD_TRUNK,                 /*  SDK_MOD_TRUNK,              16 */                   
    (uint8 *)STR_MOD_VLAN,                  /*  SDK_MOD_VLAN,               17 */                   
    (uint8 *)STR_MOD_ACL,                   /*  SDK_MOD_ACL,                18 */                   
    (uint8 *)STR_MOD_HAL,                   /*  SDK_MOD_HAL,                19 */                   
    (uint8 *)STR_MOD_DAL,                   /*  SDK_MOD_DAL,                20 */                   
    (uint8 *)STR_MOD_RTDRV,                 /*  SDK_MOD_RTDRV,              21 */                   
    (uint8 *)STR_MOD_RTUSR,                 /*  SDK_MOD_RTUSR,              22 */                   
    (uint8 *)STR_MOD_DIAGSHELL,             /*  SDK_MOD_DIAGSHELL,          23 */                   
    (uint8 *)STR_MOD_UNITTEST,              /*  SDK_MOD_UNITTEST,           24 */                   
    (uint8 *)STR_MOD_OAM,                   /*  SDK_MOD_OAM,                25 */                   
    (uint8 *)STR_MOD_L3,                    /*  SDK_MOD_L3,                 26 */                   
    (uint8 *)STR_MOD_RTCORE,                /*  SDK_MOD_RTCORE,             27 */                   
    (uint8 *)STR_MOD_EEE,                   /*  SDK_MOD_EEE,                28 */                   
    (uint8 *)STR_MOD_SEC,                   /*  SDK_MOD_SEC,                29 */                   
    (uint8 *)STR_MOD_LED,                   /*  SDK_MOD_LED,                30 */                   
    (uint8 *)STR_MOD_RSVD_001,              /*  SDK_MOD_RSVD_001,           31, SDK_MOD_RSVD_001 */ 
    (uint8 *)STR_MOD_RSVD_002,              /*  SDK_MOD_RSVD_002,           32, SDK_MOD_RSVD_002 */ 
    (uint8 *)STR_MOD_RSVD_003,              /*  SDK_MOD_RSVD_003,           33, SDK_MOD_RSVD_003 */ 
    (uint8 *)STR_MOD_L34,                   /*  SDK_MOD_L34,                34 */                   
    (uint8 *)STR_MOD_GPON,                  /*  SDK_MOD_GPON,               35 */                   
    (uint8 *)STR_MOD_EPON,                  /*  SDK_MOD_EPON,               36 */                   
    (uint8 *)STR_MOD_HWMISC,                /*  SDK_MOD_HWMISC,             37 */                   
    (uint8 *)STR_MOD_CPU,                   /*  SDK_MOD_CPU,                38 */                   
    (uint8 *)STR_MOD_INTR,                  /*  SDK_MOD_INTR,               39 */                   
    (uint8 *)STR_MOD_PONMAC,                /*  SDK_MOD_PONMAC,             40 */                   
    (uint8 *)STR_MOD_RLDP,                  /*  SDK_MOD_RLDP,               41 */                   
    (uint8 *)STR_MOD_CLASSIFY,              /*  SDK_MOD_CLASSIFY,           42 */                   
    (uint8 *)STR_MOD_GPIO,                 /*  SDK_MOD_GPIO,               43 */                   
    (uint8 *)STR_MOD_PTP,                  /*  SDK_MOD_PTP,                44 */                   
    (uint8 *)STR_MOD_PPSTOD,               /*  SDK_MOD_PPSTOD,             45 */                   
    (uint8 *)STR_MOD_PBO,                  /*  SDK_MOD_PBO,                46 */                   
    (uint8 *)STR_MOD_MDIO,                 /*  SDK_MOD_MDIO,               47 */                   
    (uint8 *)STR_MOD_END                    /*  SDK_MOD_END */
};


/*
 * Macro Declaration
 */
#define RT_LOG_CONFIG_GET(type, lv, lv_mask,                    \
                          mod_mask, format)                     \
do {                                                            \
    rtdrv_logCfg_t cfg;                                         \
    if(RT_ERR_OK == rt_log_config_get((uint32 *) &cfg))         \
    {                                                           \
        type = cfg.log_level_type;                              \
        lv = cfg.log_level;                                     \
        lv_mask = cfg.log_level_mask;                           \
        mod_mask = cfg.log_module_mask;                         \
        format = cfg.log_format;                                \
    }                                                           \
} while(0)

/*
 * Function Declaration
 */
int32 rt_log_enable_get(rtk_enable_t *pEnabled)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_EN_LOG_GET, &unit_cfg, rtdrv_unitCfg_t, 1);    

    *pEnabled = unit_cfg.data;
    
    return RT_ERR_OK;    
}

int32 rt_log_enable_set(rtk_enable_t enabled)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = enabled;
    SETSOCKOPT(RTDRV_DEBUG_EN_LOG_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rt_log_level_get(uint32 *pLv)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));
     
    GETSOCKOPT(RTDRV_DEBUG_LOGLV_GET, &unit_cfg, rtdrv_unitCfg_t, 1);    
    *pLv = unit_cfg.data;
    
    return RT_ERR_OK;    
}

int32 rt_log_level_set(uint32 lv)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = lv;
    SETSOCKOPT(RTDRV_DEBUG_LOGLV_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rt_log_mask_get(uint32 *pMask)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));
 
    GETSOCKOPT(RTDRV_DEBUG_LOGLVMASK_GET, &unit_cfg, rtdrv_unitCfg_t, 1);    
    *pMask = unit_cfg.data;
    
    return RT_ERR_OK;    
}

int32 rt_log_mask_set(uint32 mask)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = mask;
    SETSOCKOPT(RTDRV_DEBUG_LOGLVMASK_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rt_log_type_get(uint32 *pType)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));
   
    GETSOCKOPT(RTDRV_DEBUG_LOGTYPE_GET, &unit_cfg, rtdrv_unitCfg_t, 1);    
    *pType = unit_cfg.data;
    
    return RT_ERR_OK;    
}

int32 rt_log_type_set(uint32 type)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = type;
    SETSOCKOPT(RTDRV_DEBUG_LOGTYPE_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rt_log_format_get(uint32 *pFormat)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));
 
    GETSOCKOPT(RTDRV_DEBUG_LOGFORMAT_GET, &unit_cfg, rtdrv_unitCfg_t, 1);    
    *pFormat = unit_cfg.data;
    
    return RT_ERR_OK;    
}

int32 rt_log_format_set(uint32 format)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data = format;
    SETSOCKOPT(RTDRV_DEBUG_LOGFORMAT_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rt_log_moduleMask_get(uint64 *pMask)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    GETSOCKOPT(RTDRV_DEBUG_MODMASK_GET, &unit_cfg, rtdrv_unitCfg_t, 1);    
    *pMask = unit_cfg.data64;
    
    return RT_ERR_OK;    
}

int32 rt_log_moduleMask_set(uint64 mask)
{
    rtdrv_unitCfg_t unit_cfg;
    osal_memset(&unit_cfg, 0x0, sizeof(rtdrv_unitCfg_t));

    unit_cfg.data64 = mask;
    SETSOCKOPT(RTDRV_DEBUG_MODMASK_SET, &unit_cfg, rtdrv_unitCfg_t, 1); 

    return RT_ERR_OK;    
}

int32 rt_log_config_get(uint32 *pCfg)
{   
    GETSOCKOPT(RTDRV_DEBUG_LOGCFG_GET, pCfg, rtdrv_logCfg_t, 1);   
    
    return RT_ERR_OK;    
}

uint8** rt_log_moduleName_get(uint64 module)
{
    uint32 i;

    /* parameter check */
    RT_PARAM_CHK(!LOG_MOD_CHK(module), RT_ERR_INPUT);

    for (i = 0; i < SDK_MOD_END; i++)
        if ((module >> i) & 0x1) break;       
        
    return (rtLogModuleName + i);
}

int32 rt_log(const int32 level, const int64 module, const char *format, ...)
{    
    /* start logging, determine the length of the output string */
    uint32 log_type = 0, log_level = 0, log_mask = 0, log_module_mask = 0, log_format = 0;    
    int32  result = RT_ERR_FAILED;
    static uint8  buf[LOG_BUFSIZE_DEFAULT]; /* init value will be given by RT_LOG_FORMATTED_OUTPUT */    
   
    /* get the log config setting from the module */
    RT_LOG_CONFIG_GET(log_type, log_level, log_mask, log_module_mask, log_format);

    /* check log level and module */
    RT_LOG_PARAM_CHK(level, module);

    /* formatted output conversion */     
    RT_LOG_FORMATTED_OUTPUT(buf, format, result);

    if (result < 0)
        return RT_ERR_FAILED;  
    
    /* get the current time */
    time_t t = time(0);
    struct tm *pLt = localtime(&t);	    
    
    /* if that worked, print to console */
    if (LOG_FORMAT_DETAILED == log_format)
    {
        rt_log_printf("* ------------------------------------------------------------\n");
        rt_log_printf("* Level : %d\n", level);
        rt_log_printf("* Module: %s\n", *rt_log_moduleName_get(log_module_mask & module));			    
        rt_log_printf("* Date  : %04d-%02d-%02d\n", pLt->tm_year + 1900, pLt->tm_mon + 1, pLt->tm_mday);
        rt_log_printf("* Time  : %02d:%02d:%02d\n", pLt->tm_hour, pLt->tm_min, pLt->tm_sec);                
        rt_log_printf("* Log   : %s\n", buf);			    
    }
    else
    {
        rt_log_printf("[%d][%s] %02d:%02d:%02d %s", level, *rt_log_moduleName_get(log_module_mask & module),
                      pLt->tm_hour, pLt->tm_min, pLt->tm_sec, buf);            
    }    	 

    return RT_ERR_OK;
}

