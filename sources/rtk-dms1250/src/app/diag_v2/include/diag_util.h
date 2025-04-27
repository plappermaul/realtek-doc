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
 * Purpose : Define those public diag shell utility APIs.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag shell utility
 */


#ifndef _DIAG_UTIL_H_
#define _DIAG_UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <osal/print.h>
#include <rtk/switch.h>
#include <rtk/trunk.h>
#include <rtk/l3.h>
#include <flag/cmd_flag.h>

/*
 * Symbol Definition
 */
#define DIAG_UTIL_PORT_MASK_STRING_LEN          (RTK_MAX_NUM_OF_PORTS * 2)
#define diag_util_printf(fmt, args...)          osal_printf(fmt, ## args)

#define DIAG_ERR_PRINT(err_num) \
    do { \
            diag_util_printf("Error (0x%x): %s\n", err_num, rt_error_numToStr(err_num)); \
   	   } while (0)

#define CPARSER_ERR_PRINT(err_num) \
    do { \
            diag_util_printf("Cparser Error (0x%x): %s\n", err_num, cparser_error_numToStr(err_num)); \
   	   } while (0)

#define IS_PORT_IN_PORT_MARSK(port, port_mask)  ((1 << port) & (port_mask))

/* portlist_pos is the position of portlist token */
#define DIAG_UTIL_EXTRACT_PORTLIST(portlist, portlist_pos) \
    diag_util_extract_portlist(unit, context->parser->tokens[(portlist_pos)].buf, &(portlist))

#define DIAG_UTIL_EXTRACT_TRKLIST(trklist, trklist_pos) \
    diag_util_extract_trklist(unit, context->parser->tokens[(trklist_pos)].buf, &(trklist))

#define DIAG_UTIL_PORTMASK_SCAN(portlist, port)\
    for ((port) = 0; (port) < RTK_MAX_NUM_OF_PORTS; (port)++)\
        if (RTK_PORTMASK_IS_PORT_SET((portlist).portmask, (port)))

#define DIAG_UTIL_TRKMASK_SCAN(trklist, trunk)  \
    for ((trunk) = 0; (trunk) < RTK_MAX_TRUNK_PER_UNIT; (trunk)++)\
        if (RTK_TRUNKMASK_IS_TRUNK_SET((trklist).trkmask, (trunk)))

#define DIAG_UTIL_EXTRACT_MASK(_mask, _mask_pos, _type) \
    diag_util_extract_mask(unit, context->parser->tokens[(_mask_pos)].buf, _type, &(_mask))

#define DIAG_UTIL_EXTRACT_QUEUEMASK(_mask, _mask_pos) \
    diag_util_extract_mask(unit, context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_QUEUE, &(_mask))

#define DIAG_UTIL_MASK_SCAN(_mask, _member)\
    for ((_member) = 0; (_member) < DIAG_MASK_MAX_LEN; (_member)++)\
        if (BITMAP_IS_SET((_mask).mask.bits, (_member)))

#define DIAG_UTIL_DSCPMASK_SCAN(_mask, _member)\
    for ((_member) = _mask.min; (_member) <= _mask.max; (_member)++)\
        if (RTK_DSCPMASK_IS_DSCP_SET((_mask).mask, (_member)))

#define DIAG_UTIL_PARSE_ACTION(__idx,__act)  \
do{                                 \
    switch(TOKEN_CHAR(__idx, 2)) {  \
        case 'r':                   \
            __act = ACTION_FORWARD; \
            break;                  \
        case 'o':                   \
            if ('p' == TOKEN_CHAR(__idx, 3))    \
                __act = ACTION_DROP;            \
            break;                              \
        case 'a':                               \
            if ('c' == TOKEN_CHAR(__idx, 8))    \
                __act = ACTION_TRAP2CPU;        \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = ACTION_TRAP2MASTERCPU;  \
            break;                              \
        case 'p':                               \
            if ('c' == TOKEN_CHAR(__idx, 8))    \
                __act = ACTION_COPY2CPU;        \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = ACTION_COPY2MASTERCPU;  \
            break;                              \
        default:                                \
            break;                              \
    }                                           \
}while(0);

#define DIAG_UTIL_PARSE_L3_ACT(__idx,__act)         \
do{                                                 \
    switch(TOKEN_CHAR(__idx, 2)) {                  \
        case 'r':                                   \
            if ('f' == TOKEN_CHAR(__idx, 0))        \
                __act = RTK_L3_ACT_FORWARD;         \
            else if ('h' == TOKEN_CHAR(__idx, 0))   \
                __act = RTK_L3_ACT_HARD_DROP;       \
            break;                                  \
        case 'o':                                   \
            if ('p' == TOKEN_CHAR(__idx, 3))        \
                __act = RTK_L3_ACT_DROP;            \
            break;                                  \
        case 'a':                                   \
            if ('c' == TOKEN_CHAR(__idx, 8))        \
                __act = RTK_L3_ACT_TRAP2CPU;        \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = RTK_L3_ACT_TRAP2MASTERCPU;  \
            break;                                  \
        case 'p':                                   \
            if ('c' == TOKEN_CHAR(__idx, 8))        \
                __act = RTK_L3_ACT_COPY2CPU;        \
            else if ('m' == TOKEN_CHAR(__idx, 8))   \
                __act = RTK_L3_ACT_COPY2MASTERCPU;  \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \
}while(0)

#define DIAG_UTIL_PARSE_L3_HOST_ACT(__idx,__act)    \
do{                                                 \
    switch(TOKEN_CHAR(__idx, 0)) {                  \
        case 'f':                                   \
            __act = RTK_L3_HOST_ACT_FORWARD;        \
            break;                                  \
        case 'd':                                   \
            __act = RTK_L3_HOST_ACT_DROP;           \
            break;                                  \
        case 't':                                   \
            __act = RTK_L3_HOST_ACT_TRAP2CPU;       \
            break;                                  \
        case 'c':                                   \
            __act = RTK_L3_HOST_ACT_COPY2CPU;       \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \
}while(0)

#define DIAG_UTIL_PARSE_L3_ROUTE_ACT(__idx,__act)   \
do{                                                 \
    switch(TOKEN_CHAR(__idx, 0)) {                  \
        case 'f':                                   \
            __act = RTK_L3_ROUTE_ACT_FORWARD;       \
            break;                                  \
        case 'd':                                   \
            __act = RTK_L3_ROUTE_ACT_DROP;          \
            break;                                  \
        case 't':                                   \
            __act = RTK_L3_ROUTE_ACT_TRAP2CPU;      \
            break;                                  \
        case 'c':                                   \
            __act = RTK_L3_ROUTE_ACT_COPY2CPU;      \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \
}while(0)

#define DIAG_UTIL_PARSE_STATE(__idx,__state)    \
do{                                         \
    if ('e' == TOKEN_CHAR(__idx, 0))        \
        __state = ENABLED;                  \
    else if ('d' == TOKEN_CHAR(__idx, 0))   \
        __state = DISABLED;                 \
    else                                    \
        return CPARSER_ERR_INVALID_PARAMS;  \
}while(0);

#define DIAG_UTIL_FUNC_INIT(__unit)         \
do{                             \
    DIAG_UTIL_PARAM_CHK();      \
    DIAG_OM_GET_UNIT_ID(__unit);\
    DIAG_UTIL_OUTPUT_INIT();    \
}while(0);

#define DIAG_UTIL_MPRINTF   diag_util_mprintf
#define DIAG_UTIL_MAC2STR   diag_util_mac2str

#define DIAG_UTIL_PARAM_CHK()\
do {\
    if (NULL == context)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");\
        return CPARSER_NOT_OK;\
    }\
} while (0)

#define DIAG_UTIL_PARAM_RANGE_CHK(op, ret)\
do {\
    if (op)\
    {\
        CPARSER_ERR_PRINT(ret);\
        return CPARSER_NOT_OK;\
    }\
} while(0)

#define DIAG_UTIL_ERR_CHK(op, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK)\
    {\
        DIAG_ERR_PRINT(ret);\
        return CPARSER_NOT_OK;\
    }\
} while(0)

#define TOKEN_STR(m)    (context->parser->tokens[m].buf)
#define TOKEN_CHAR(m,n) (context->parser->tokens[m].buf[n])
#define TOKEN_NUM       (context->parser->cmd_tokens)

#define DIAG_UTIL_OUTPUT_INIT()    (diag_util_mprintf_init())

#define DIAG_MIN(a,b) ((a < b)?a:b);
#define DIAG_MAX(a,b) ((a < b)?b:a);

#define DIAG_UTIL_VAL_TO_BYTE_ARRAY(_val, _valbytes, _array, _start, _bytes, _i)\
do{\
    for (_i = 0; _i < _bytes; _i++)\
        _array[_start+_i] = (_val >> (8* (_valbytes - _i - 1)));\
}while(0)

extern const char text_action[ACTION_END+1][64];
extern const char text_l3_action[RTK_L3_ACT_END+1][64];
extern const char text_state[RTK_ENABLE_END+1][32];

typedef enum diag_portType_e
{
    DIAG_PORTTYPE_ETHER = 0,
    DIAG_PORTTYPE_ALL,
    DIAG_PORTTYPE_END
} diag_portType_t;

typedef struct diag_portlist_s
{
    uint32 min;                 /* min port  */
    uint32 max;                 /* max port  */
    rtk_portmask_t portmask;    /* portmask output value */
} diag_portlist_t;

typedef struct diag_trklist_s
{
    uint32 max;                 /* max trunk  */
    rtk_trkmask_t trkmask;      /* trunk output value */
} diag_trklist_t;

#define DIAG_MASK_MAX_LEN       4096

typedef uint32                  diag_bitmap_t;
#define DIAG_BMP_BIT_LEN        32

#define DIGA_BMP_WIDTH(_len)    (((_len) + DIAG_BMP_BIT_LEN - 1) / DIAG_BMP_BIT_LEN)

typedef struct diag_bmp_s
{
    diag_bitmap_t   bits[DIGA_BMP_WIDTH(DIAG_MASK_MAX_LEN)];
} diag_bmp_t;

typedef enum diag_maskType_e
{
    DIAG_MASKTYPE_QUEUE,
    DIAG_MASKTYPE_DSCP,
    DIAG_MASKTYPE_UNIT,
    DIAG_MASKTYPE_VLAN,
    DIAG_MASKTYPE_END
} diag_maskType_t;

typedef struct diag_mask_s
{
    uint32      min;                 /* min port  */
    uint32      max;                 /* max port  */
    diag_bmp_t  mask;                /* mask output value */
} diag_mask_t;

/* Convert <number> and trunk<number>, separated by ","s and "-"s, to logical 64-bit mask */
int32 diag_util_str2LPortMask(char *str, rtk_portmask_t *mask);
int32 diag_util_str2LTrkMask(char *str, rtk_trkmask_t *mask);
int32 diag_util_str2devPort(char *str, rtk_dev_port_t *pDev_port);
int32 diag_util_str2devPorts(char *str, rtk_trk_egrPort_t *pUnitPort);
int32 diag_util_str2ul(uint32 *ul, const char *str);

/* convert MAC address <--> string */
int32 diag_util_mac2str (char *str, const uint8 *mac);
int32 diag_util_str2mac(uint8 *mac, char *str);

/* convert IP address <--> string */
int32 diag_util_ip2str(char *str, uint32 ip);            /* Length of ip_str must more than 15 characters */
int32 diag_util_str2ip(uint32 *ip, char *str);
int32 diag_util_ip2str_format(char *str, uint32 ip, uint32 blankWidth);    /* Length of ip_str must more than 15 characters */

/* convert IPv6 address <--> string */
int32 diag_util_ipv62str(char *str, const uint8 *ipv6); /* Length of ipv6_str must more than 39 characters */
int32 diag_util_str2ipv6(uint8 *ipv6, const char *str); /* Length of ipv6_addr must more than 16 characters */

/* convert logical port mask to string, separated by ","s, string length of comma is DIAG_UTIL_PORT_MASK_STRING_LEN */
int32 diag_util_lPortMask2str(char *comma, rtk_portmask_t *pstLPortMask);
int32 diag_util_extract_portlist(uint32 unit, char *pStr, diag_portlist_t *pPortmask);
int32 diag_util_extract_trklist(uint32 unit, char *pStr, diag_trklist_t *pTrktlist);
void diag_util_port_min_max_get(rtk_portmask_t *pPortmask, uint32 *pMin, uint32 *pMax);

int32 diag_util_extract_mask(uint32 unit, char *pStr, uint32 type,
    diag_mask_t *pMask);

/* convert ACTION_xxx to string */
char *diag_util_act2str(rtk_action_t action);


int32 diag_util_isBcastMacAddr(uint8 *mac);
int32 diag_util_isMcastMacAddr(uint8 *mac);

int32 diag_util_mprintf(char *fmt, ...);
void diag_util_mprintf_init(void);

void diag_util_mprintf_paging_lines (unsigned int number);
int32 diag_util_str2IntArray (uint8 *int_array, char *str, uint32 field_size);

int32 diag_util_ipv6IsZero_ret(rtk_ipv6_addr_t *pIp6);
char *diag_util_serdesMode2str(rt_serdesMode_t sdsMode);
int32 diag_util_str2upper(char *str);
int32 diag_util_convert_mword_string_2_int32_array(char * string_ptr,char * delim,uint32 * out_array,uint32 * cnt);
int diag_util_str2rangeStep(char * str,rt_valRangeStep_t * val);



#endif /* end of _DIAG_UTIL_H_ */

