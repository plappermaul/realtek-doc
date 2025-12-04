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
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/print.h>
#include <rtk/switch.h>
#include <rtk/init.h>
#include <hal/common/hal_adpt.h>
#include <hal/common/halctrl.h>
#include "parser/cparser.h"
/*
 * Symbol Definition
 */
#define DIAG_UTIL_PORT_MASK_STRING_LEN          (RTK_MAX_NUM_OF_PORTS * 2)


#define UTIL_STRING_BUFFER_LENGTH       (1024)
#define UTIL_PORT_MASK_BUFFER_LENGTH    (30)
#define UTIL_IP_TMP_BUFFER_LENGTH       (4)
#define UTIL_IPV6_TMP_BUFFER_LENGTH     (8)

#define UTIL_MAC_STR_TMP_BUFFER_LENGTH       (17)
#define UTIL_IP_STR_TMP_BUFFER_LENGTH       (15)


#define DIAG_UTIL_CHIP_TYPE             (diag_util_chip_id_get())

#if 0
    #if (defined(CONFIG_SDK_KERNEL_LINUX) && !defined(__KERNEL__))
            #define diag_util_printf(fmt, args...)    fprintf(stderr, fmt, ## args)
    #else
            #define diag_util_printf(fmt, args...)    osal_printf(fmt, ## args)
    #endif
#else
    void diag_util_printf(char *fmt, ...);
#endif

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
    diag_util_extract_portlist(context->parser->tokens[(portlist_pos)].buf, DIAG_PORTTYPE_ALL, &(portlist))

/*for DSL port list*/
#define DIAG_UTIL_EXTRACT_DSLLIST(portlist, portlist_pos) \
    diag_util_extract_portlist(context->parser->tokens[(portlist_pos)].buf, DIAG_PORTTYPE_DSL, &(portlist))

/*for extension port list*/
#define DIAG_UTIL_EXTRACT_EXTLIST(portlist, portlist_pos) \
    diag_util_extract_portlist(context->parser->tokens[(portlist_pos)].buf, DIAG_PORTTYPE_EXT, &(portlist))

/*for CPU port list*/
#define DIAG_UTIL_EXTRACT_CPULIST(portlist, portlist_pos) \
    diag_util_extract_portlist(context->parser->tokens[(portlist_pos)].buf, DIAG_PORTTYPE_CPU, &(portlist))

/*for swpbo  loop back port list*/
#define DIAG_UTIL_EXTRACT_SWPBOLIST(portlist, portlist_pos) \
    diag_util_extract_portlist(context->parser->tokens[(portlist_pos)].buf, DIAG_PORTTYPE_SWPBO, &(portlist))


#define DIAG_UTIL_PORTMASK_SCAN(portlist, port)\
    for ((port) = portlist.min; (port) <= portlist.max; (port)++)\
        if (RTK_PORTMASK_IS_PORT_SET((portlist).portmask, (port)))


#define DIAG_UTIL_PORTMASK_UNSETSCAN(portlist, port)\
    for ((port) = portlist.min; (port) <= portlist.max; (port)++)\
        if (!RTK_PORTMASK_IS_PORT_SET((portlist).portmask, (port)))


#define DIAG_UTIL_EXTRACT_QUEUEMASK(_mask, _mask_pos) \
    diag_util_extract_mask(context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_QUEUE, &(_mask))

#define DIAG_UTIL_EXTRACT_MASK8(_mask, _mask_pos) \
    diag_util_extract_mask(context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_MASK8, &(_mask))

#define DIAG_UTIL_EXTRACT_MASK16(_mask, _mask_pos) \
    diag_util_extract_mask(context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_MASK16, &(_mask))

#define DIAG_UTIL_EXTRACT_MASK32(_mask, _mask_pos) \
        diag_util_extract_mask(context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_MASK32, &(_mask))

#define DIAG_UTIL_EXTRACT_MASK64(_mask, _mask_pos) \
    diag_util_extract_mask(context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_MASK64, &(_mask))

#define DIAG_UTIL_EXTRACT_MASK128(_mask, _mask_pos) \
    diag_util_extract_mask(context->parser->tokens[(_mask_pos)].buf, DIAG_MASKTYPE_MASK128, &(_mask))


#define DIAG_UTIL_MASK_SCAN(_mask, _member)\
    for ((_member) = _mask.min; (_member) <= _mask.max; (_member)++)\
        if (RTK_LSTMASK_IS_MAMBER_SET((_mask).mask, (_member), (_mask.max)))


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



#define DIAG_UTIL_ERR_CHK_NO_RET(op, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK)\
    {\
        DIAG_ERR_PRINT(ret);\
    }\
} while(0)




#define TOKEN_STR(m)    (context->parser->tokens[m].buf)
#define TOKEN_CHAR(m,n) (context->parser->tokens[m].buf[n])
#define TOKEN_NUM()     (context->parser->cmd_tokens)

#define DIAG_UTIL_OUTPUT_INIT()    (diag_util_mprintf_init())

typedef enum diag_portType_e
{
    DIAG_PORTTYPE_FE = 0,
    DIAG_PORTTYPE_GE,
    DIAG_PORTTYPE_GE_COMBO,
    DIAG_PORTTYPE_SERDES,
    DIAG_PORTTYPE_ETHER,
    DIAG_PORTTYPE_DSL,
    DIAG_PORTTYPE_EXT,
    DIAG_PORTTYPE_CPU,
    DIAG_PORTTYPE_SWPBO,
    DIAG_PORTTYPE_ALL,
    DIAG_PORTTYPE_END
} diag_portType_t;

typedef struct diag_portlist_s
{
    uint32 min;                 /* min port  */
    uint32 max;                 /* max port  */
    rtk_portmask_t portmask;    /* portmask output value */
} diag_portlist_t;



typedef enum diag_maskType_e
{
    DIAG_MASKTYPE_QUEUE,
    DIAG_MASKTYPE_FID,
    DIAG_MASKTYPE_MASK8,
    DIAG_MASKTYPE_MASK16,
    DIAG_MASKTYPE_MASK32,
    DIAG_MASKTYPE_MASK64,
    DIAG_MASKTYPE_MASK128,
    DIAG_MASKTYPE_END
} diag_maskType_t;

typedef struct diag_mask_s
{
    uint32      min;                 /* min port  */
    uint32      max;                 /* max port  */
    rtk_bmp_t  mask;                /* mask output value */
} diag_mask_t;

/* Convert <number> and trunk<number>, separated by ","s and "-"s, to logical 64-bit mask */
int32 diag_util_str2LPortMask(uint8 *str, rtk_portmask_t *mask);
int32 diag_util_str2ul(uint32 *ul, const char *str);

int8 *diag_util_mask32tostr(uint32 mask);
void diag_util_mask2str (uint8 *str, uint32 mask);

/* convert MAC address <--> string */
int8 *diag_util_inet_mactoa (const uint8 *mac);
int32 diag_util_mac2str (uint8 *str, const uint8 *mac);
int32 diag_util_str2mac(uint8 *mac, uint8 *str);


/* convert IP address <--> string */
int8 *diag_util_inet_ntoa(ipaddr_t ina);
int8 *diag_util_inet_n6toa(const uint8 *ipv6);

int32 diag_util_ip2str(uint8 *str, uint32 ip);            /* Length of ip_str must more than 15 characters */
int32 diag_util_str2ip(uint32 *ip, uint8 *str);

/* convert IPv6 address <--> string */
int32 diag_util_ipv62str(uint8 *str, const uint8 *ipv6); /* Length of ipv6_str must more than 39 characters */
int32 diag_util_str2ipv6(uint8 *ipv6, const uint8 *str); /* Length of ipv6_addr must more than 16 characters */

/* convert logical port mask to string, separated by ","s, string length of comma is DIAG_UTIL_PORT_MASK_STRING_LEN */
int32 diag_util_lPortMask2str(uint8 *comma, rtk_portmask_t *pstLPortMask);
int32 diag_util_extract_portlist(uint8 *pStr, uint32 type, diag_portlist_t *pPortmask);

int32 diag_util_portMaskRangeCheck(rtk_portmask_t *full_mask, rtk_portmask_t *test_mask);

int32 diag_util_extract_mask(uint8 *pStr, diag_maskType_t type, diag_mask_t *pMask);

int32 diag_util_isBcastMacAddr(uint8 *mac);
int32 diag_util_isMcastMacAddr(uint8 *mac);

void diag_util_mprintf(char *fmt, ...);
void diag_util_mprintf_init(void);

int32 diag_util_reverse_portlist(diag_portType_t type,rtk_portmask_t *pPlst,rtk_portmask_t *pRevesedPlst);


uint32 diag_util_chip_id_get();

int32 diag_util_str2hex32(uint8 *str, uint32 *intOut);


int32 diag_util_eui642str (uint8 *str, const uint64 *eui64);

#endif /* end of _DIAG_UTIL_H_ */
