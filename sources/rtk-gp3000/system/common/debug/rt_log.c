/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : Realtek Switch SDK Debug Module 
 * 
 * Feature : The file have include the following module and sub-modules
 *           1) SDK Debug Module for Kernel Mode
 * 
 */

/*
 * Include Files
 */
#include <stdarg.h>
#include <common/debug/rt_log.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
#if 1
static uint32 log_level = LOG_WARNING;
static uint32 log_mask = LOG_MASK_DEFAULT;
static uint64 log_module_mask = MOD_MASK_DEFAULT;
#else
static uint32 log_level= 0xFFFFFFFF;
static uint32 log_mask =0xFFFFFFFF;
static uint64 log_module_mask= 0xFFFFFFFF;
#endif
static uint32 log_type;
static uint32 log_format;

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

static uint32 log_level_bak;
static uint32 log_mask_bak;

/*
 * Macro Declaration
 */

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
int32 rt_log_init(void)
{

    /* Set the default value */
    log_level = LOG_LEVEL_DEFAULT;
    log_level_bak = log_level;
    log_mask = LOG_MASK_DEFAULT;
    log_mask_bak = log_mask;
    log_type = LOG_TYPE_DEFAULT;
    log_module_mask = MOD_MASK_DEFAULT;
    log_format = LOG_FORMAT_DEFAULT;
    
#ifdef __SHOW_RTLOG_DEFAULT_VALUE__
    rt_log_printf("[%s]\nlog_type: %d\nlog_level: %d\nlog_level_mask: 0x%X\nlog_module_mask: 0x%X\n",
        __FUNCTION__, log_type, log_level, log_mask, log_module_mask);
#endif /* __SHOW_RTLOG_DEFAULT_VALUE__ */

    return RT_ERR_OK;
}

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
int32 rt_log_enable_get(uint32 *pEnable)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* check which log type is used */    
    if (LOG_TYPE_LEVEL == log_type)
    {
        if (LOG_MSG_OFF == log_level)
        {
            *pEnable = DISABLED;
        }
        else
        {
            *pEnable = ENABLED;
        }
    }    
    else if (LOG_TYPE_MASK == log_type)
    {
        if (LOG_MSG_OFF == log_mask)
        {
            *pEnable = DISABLED;
        }
        else
        {
            *pEnable = ENABLED; 
        }
    }
    else
    { 
        /* log type not support */
        return RT_ERR_FAILED;           
    }
    
    return RT_ERR_OK;        
}

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
int32 rt_log_enable_set(uint32 enable)
{
    if (ENABLED == enable)
    {
        /* restore the log level or mask */
        if (LOG_TYPE_LEVEL == log_type)
        {
            log_level = log_level_bak;    
        }    
        else if (LOG_TYPE_MASK == log_type)
        {
            log_mask = log_mask_bak;  
        }
        else
        {
            /* log type not support */ 
            return RT_ERR_FAILED;          
        }
    }
    else
    {
        /* keep the current log level or mask before turning off */
        if (LOG_TYPE_LEVEL == log_type)
        {
            log_level_bak = log_level;    
            log_level = LOG_MSG_OFF;   
        }    
        else if (LOG_TYPE_MASK == log_type)
        {
            log_mask_bak = log_mask;
            log_mask = LOG_MASK_OFF;    
        }
        else
        {
            /* log type not support */
            return RT_ERR_FAILED;           
        }
    }    
    
    return RT_ERR_OK;      
}

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
void rt_log_reset(void)
{
    rt_log_level_reset();
    rt_log_mask_reset();
    rt_log_type_reset();
    rt_log_moduleMask_reset();
    rt_log_format_reset();  
}

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
int32 rt_log_level_get(uint32 *pLv)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pLv), RT_ERR_NULL_POINTER);
            
    *pLv = log_level;  
      
    return RT_ERR_OK;    
}

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
int32 rt_log_level_set(uint32 lv)
{
    /* parameter check */
    RT_PARAM_CHK(!LOG_LEVEL_CHK(lv), RT_ERR_OUT_OF_RANGE);
    
    log_level = lv;    
    
    return RT_ERR_OK;    
}

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
void rt_log_level_reset(void)
{
    log_level = LOG_LEVEL_DEFAULT;    
}

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
int32 rt_log_mask_get(uint32 *pMask)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
                
    *pMask = log_mask;    
    
    return RT_ERR_OK;    
}

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
int32 rt_log_mask_set(uint32 mask)
{
    /* parameter check */
    RT_PARAM_CHK(!LOG_MASK_CHK(mask), RT_ERR_OUT_OF_RANGE);

    log_mask = mask;    
    
    return RT_ERR_OK;    
}

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
void rt_log_mask_reset(void)
{
    log_mask = LOG_MASK_DEFAULT;    
}

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
int32 rt_log_type_get(uint32 *pType)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);  
        
    *pType = log_type;
    
    return RT_ERR_OK;    
}

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
int32 rt_log_type_set(uint32 type)
{    
    /* parameter check */
    RT_PARAM_CHK(!LOG_TYPE_CHK(type), RT_ERR_OUT_OF_RANGE);
    
    if(log_type != type)
    {
#ifdef __TYPE_CHANGE_RESET_DEFAULT__

        /* reset to DEFAULT when changing the log type */
        if (LOG_TYPE_LEVEL == type)
        {
            rt_log_mask_reset();
        }
        else if (LOG_TYPE_MASK == type)
        {
            rt_log_level_reset();
        }
        else
        {
            /* log type not support */
            return RT_ERR_FAILED;     
        }
#endif        
        log_type = type;
    }
    
    return RT_ERR_OK;    
}

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
void rt_log_type_reset(void)
{
    log_type = LOG_TYPE_DEFAULT;   
}

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
int32 rt_log_moduleMask_get(uint64 *pMask)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);  
                
    *pMask = log_module_mask;    
    
    return RT_ERR_OK;    
}

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
int32 rt_log_moduleMask_set(uint64 mask)
{
    /* parameter check */
    RT_PARAM_CHK(!LOG_MOD_CHK(mask), RT_ERR_OUT_OF_RANGE);

    log_module_mask = mask;    
    
    return RT_ERR_OK;    
}

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
void rt_log_moduleMask_reset(void)
{
    log_module_mask = MOD_MASK_DEFAULT;
}

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
int32 rt_log_format_get(uint32 *pFormat)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pFormat), RT_ERR_NULL_POINTER);  
        
    *pFormat = log_format;    
    
    return RT_ERR_OK;    
}

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
int32 rt_log_format_set(uint32 format)
{
    /* parameter check */
    RT_PARAM_CHK(!LOG_FORMAT_CHK(format), RT_ERR_OUT_OF_RANGE);

    log_format = format;   
    
    return RT_ERR_OK;    
}

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
void rt_log_format_reset(void)
{
    log_format = LOG_FORMAT_DEFAULT;   
}

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
int32 rt_log_config_get(uint32 *pCfg)
{   
    /* parameter check */
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);    
        
    *(pCfg + 0) = log_level; 
    *(pCfg + 1) = log_mask;
    *(pCfg + 2) = log_type;
    *(pCfg + 3) = log_module_mask; 
    *(pCfg + 4) = log_format;    

    return RT_ERR_OK;   
}

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
uint8** rt_log_moduleName_get(uint64 module)
{
    uint32 i;

    /* parameter check */
    RT_PARAM_CHK(!LOG_MOD_CHK(module), (rtLogModuleName + SDK_MOD_END));

    for (i = 0; i < SDK_MOD_END; i++)
        if ((module >> i) & 0x1) break;       
        
    return (rtLogModuleName + i);
}

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
int32 rt_log(const int32 level, const int64 module, const char *format, ...)
{    
    /* determine the length of the output string */
    int32 result = RT_ERR_FAILED;
    static uint8 buf[LOG_BUFSIZE_DEFAULT];   /* init value will be given by RT_LOG_FORMATTED_OUTPUT */ 

    /* check log level and module */
    RT_LOG_PARAM_CHK(level, module);

#if 0
    printf("\n===============================");
    printf("\nlevel:%x module:%x",level,module);
    printf("\nLOG_TYPE_LEVEL:%x log_type:%x log_level:%x log_mask:%x",LOG_TYPE_LEVEL,log_type,log_level,log_mask);
    printf("\nLOG_MSG_OFF:%x LOG_TYPE_MASK:%x log_module_mask:%x",LOG_MSG_OFF,LOG_TYPE_MASK,log_module_mask);
    printf("\n===============================");
#endif

    /* formatted output conversion */     
    RT_LOG_FORMATTED_OUTPUT(buf, format, result);

    if (result < 0)
        return RT_ERR_FAILED;  
    
    /* start logging */
    rt_log_printf("[%d][%s] %s", level, *rt_log_moduleName_get(log_module_mask & module), buf);    

    return RT_ERR_OK;   
}

