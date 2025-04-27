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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#if defined(__linux__)
#include <termio.h>
#endif
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>

/*
 * Symbol Definition
 */
#define MAX_MORE_LINES  20

#define UTIL_STRING_BUFFER_LENGTH       (128)
#define UTIL_PORT_MASK_BUFFER_LENGTH    (16)
#define UTIL_IP_TMP_BUFFER_LENGTH       (4)
#define UTIL_IPV6_TMP_BUFFER_LENGTH     (8)

/*
 * Data Declaration
 */
#if defined(__linux__)
static struct termios stored_settings;
#endif
static int lines = 0;
static int stopped = 0;
static int max_lines = MAX_MORE_LINES;

#define _parse_err_return_with_trkMask()  \
    do { \
        osal_memset(mask, 0, sizeof(rtk_trkmask_t)); \
        return RT_ERR_TRUNK_ID; \
    } while (0)

#define _parse_err_return_with_pMask()  \
    do { \
        osal_memset(mask, 0, sizeof(rtk_portmask_t)); \
        return RT_ERR_PORT_MASK; \
    } while (0)
#define _parse_err_return_with_pDevIDPorts()  \
    do { \
        osal_memset(pDevIDPort, 0, sizeof(*pDevIDPort)); \
        return RT_ERR_FAILED; \
    } while (0)

#define _s2m_atoi(NUM, PTR, ENDCHAR) \
    do { \
        (NUM) = 0; \
        do { \
            if (!isdigit((int) *(PTR))) \
                return RT_ERR_FAILED; \
            (NUM) = (NUM) * 10 + (int) *(PTR) - (int)'0'; \
        } while (*(++(PTR)) != (ENDCHAR)); \
    } while (0)


const char text_action[ACTION_END+1][64] =
{
    /* ACTION_FORWARD */                            "Forward",
    /* ACTION_DROP */                               "Drop",
    /* ACTION_TRAP2CPU */                           "Trap to CPU",
    /* ACTION_COPY2CPU */                           "Copy to CPU",
    /* ACTION_TRAP2MASTERCPU */                     "Trap to Master CPU",
    /* ACTION_COPY2MASTERCPU */                     "Copy to Master CPU",
    /* ACTION_FLOOD_IN_VLAN */                      "Flood in VLAN",
    /* ACTION_END */                                "ACTION_END, remove it.",// for model code check error only
};

const char text_l3_action[RTK_L3_ACT_END+1][64] =
{
    /* RTK_L3_ACT_FORWARD */                        "Forward",
    /* RTK_L3_ACT_DROP */                           "Drop",
    /* RTK_L3_ACT_TRAP2CPU */                       "Trap to CPU",
    /* RTK_L3_ACT_COPY2CPU */                       "Copy to CPU",
    /* RTK_L3_ACT_TRAP2MASTERCPU */                 "Trap to Master CPU",
    /* RTK_L3_ACT_COPY2MASTERCPU */                 "Copy to Master CPU",
    /* RTK_L3_ACT_HARD_DROP */                      "Hard Drop",
    /* RTK_L3_ACT_END */                            "RTK_L3_ACT_END, remove it.",// for model code check error only
};


const char text_state[RTK_ENABLE_END+1][32] =
{
    /* DISABLED */          "Disabled",
    /* ENABLED */           "Enabled",
    /* RTK_ENABLE_END */    "RTK_ENABLE_END, remove it.",// for model code check error only
};

/*
 * Function Declaration
 */
static int32 _diag_util_ltrkMask_clear(rtk_trkmask_t *pstLTrkMask);
static int32 _diag_util_lPortMask_clear(rtk_portmask_t *pstLPortMask);
static int32 _diag_util_trunk2LTrunkMask_set(rtk_trkmask_t *pstLTrkMask, unsigned char trunkId);
static int32 _diag_util_port2LPortMask_set(rtk_portmask_t *pstLPortMask, unsigned char ucPortId);
static int32 _diag_util_port2LPortMask_get(rtk_portmask_t *pstLPortMask, unsigned char ucPortId);
static int32 _diag_util_getnext(char *src, int32 separator, char *dest);

static int32
_diag_util_ltrkMask_clear(rtk_trkmask_t *pstLTrkMask)
{
    uint32  i = 0;

    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_TRUNK_LIST; ++i)
    {
        pstLTrkMask->bits[i] = 0;
    }

    return RT_ERR_OK;
} /* end of _diag_util_ltrkMask_clear */

static int32
_diag_util_lPortMask_clear(rtk_portmask_t *pstLPortMask)
{
    uint32  i = 0;

    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; ++i)
    {
        pstLPortMask->bits[i] = 0;
    }

    return RT_ERR_OK;
} /* end of _diag_util_lPortMask_clear */

static int32
_diag_util_port2LPortMask_set(rtk_portmask_t *pstLPortMask, unsigned char ucPortId)
{
    if (ucPortId > RTK_MAX_NUM_OF_PORTS - 1)
    {
        return RT_ERR_FAILED;
    }

    pstLPortMask->bits[ucPortId / MASK_BIT_LEN] |= (1 << (ucPortId % MASK_BIT_LEN));

    return RT_ERR_OK;
} /* end of _diag_util_port2LPortMask_set */

static int32
_diag_util_trunk2LTrunkMask_set(rtk_trkmask_t *pstLTrkMask, unsigned char trunkId)
{
    if (trunkId > RTK_MAX_TRUNK_PER_UNIT - 1)
    {
        return RT_ERR_FAILED;
    }

    pstLTrkMask->bits[trunkId / MASK_BIT_LEN] |= (1 << (trunkId % MASK_BIT_LEN));

    return RT_ERR_OK;
} /* end of _diag_util_trunk2LTrunkMask_set */


static int32
_diag_util_port2LPortMask_get(rtk_portmask_t *pstLPortMask, unsigned char ucPortId)
{
    if (ucPortId > RTK_MAX_NUM_OF_PORTS - 1)
    {
        return RT_ERR_FAILED;
    }

    if (pstLPortMask->bits[ucPortId / MASK_BIT_LEN] & (1 << (ucPortId % MASK_BIT_LEN)))
    {
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _diag_util_port2LPortMask_get */

int32
diag_util_str2LPortMask(char *str, rtk_portmask_t *mask)
{
    uint32   i = 0;
    uint32   num = 0;
    uint32   num_end = 0;
    uint8    *ptr = NULL, *p = NULL;

    if ((NULL == str) || (NULL == mask))
    {
        return RT_ERR_FAILED;
    }

    _diag_util_lPortMask_clear(mask);

    ptr = (uint8 *)strtok((char *)str, ",");
    while (NULL != ptr)
    {
        if (isdigit((int)*ptr))
        {
            p = (uint8 *)strchr((char *)ptr, '-');
            if (NULL == p)
            {   /* number only */
                _s2m_atoi(num, ptr, '\0');
                if (num >= RTK_MAX_NUM_OF_PORTS)
                    _parse_err_return_with_pMask();

                _diag_util_port2LPortMask_set(mask, num);
            }
            else
            {   /* number-number */
                _s2m_atoi(num, ptr, '-');
                ++p;
                _s2m_atoi(num_end, p, '\0');
                if (num >= RTK_MAX_NUM_OF_PORTS || num_end >= RTK_MAX_NUM_OF_PORTS)
                    _parse_err_return_with_pMask();
                if (num_end > num)
                {
                    for (i = num; i <= num_end; i++)
                    {
                        _diag_util_port2LPortMask_set(mask, i);
                    }
                }
                else
                {
                    for (i = num_end; i <= num; i++)
                    {
                        _diag_util_port2LPortMask_set(mask, i);
                    }
                }
            }
        }
        else
            _parse_err_return_with_pMask();

        ptr = (uint8 *)strtok((char *)NULL, ",");
    }
    return RT_ERR_OK;
} /* end of diag_util_str2LPortMask */

int32
diag_util_str2LTrkMask(char *str, rtk_trkmask_t *mask)
{
    uint32   i = 0;
    uint32   num = 0;
    uint32   num_end = 0;
    uint8    *ptr = NULL, *p = NULL;

    if ((NULL == str) || (NULL == mask))
    {
        return RT_ERR_FAILED;
    }

    _diag_util_ltrkMask_clear(mask);

    ptr = (uint8 *)strtok((char *)str, ",");
    while (NULL != ptr)
    {
        if (isdigit((int)*ptr))
        {
            p = (uint8 *)strchr((char *)ptr, '-');
            if (NULL == p)
            {   /* number only */
                _s2m_atoi(num, ptr, '\0');
                if (num >= RTK_MAX_TRUNK_PER_UNIT)
                    _parse_err_return_with_trkMask();

                _diag_util_trunk2LTrunkMask_set(mask, num);
            }
            else
            {   /* number-number */
                _s2m_atoi(num, ptr, '-');
                ++p;
                _s2m_atoi(num_end, p, '\0');
                if (num >= RTK_MAX_TRUNK_PER_UNIT || num_end >= RTK_MAX_TRUNK_PER_UNIT)
                    _parse_err_return_with_trkMask();
                if (num_end > num)
                {
                    for (i = num; i <= num_end; i++)
                    {
                        _diag_util_trunk2LTrunkMask_set(mask, i);
                    }
                }
                else
                {
                    for (i = num_end; i <= num; i++)
                    {
                        _diag_util_trunk2LTrunkMask_set(mask, i);
                    }
                }
            }
        }
        else
            _parse_err_return_with_trkMask();

        ptr = (uint8 *)strtok((char *)NULL, ",");
    }
    return RT_ERR_OK;
} /* end of diag_util_str2LTrkMask */


int32
diag_util_str2devPort(char *str, rtk_dev_port_t *pDev_port)
{
    uint8    *ptr = NULL;


    if ((NULL == str) || (NULL == pDev_port))
    {
        return RT_ERR_FAILED;
    }


    ptr = (uint8 *)strtok((char *)str, ":");
    _s2m_atoi(pDev_port->devID, ptr, '\0');
    ptr = (uint8 *)strtok(NULL, ":");
    _s2m_atoi(pDev_port->port, ptr, '\0');

    return RT_ERR_OK;
} /* end of diag_util_str2unitPortMask */

int32
diag_util_str2devPorts(char *str, rtk_trk_egrPort_t *pDevIDPort)
{
    uint32   i = 0, devID = 0;
    uint32   num = 0;
    uint32   num_end = 0;
    uint8    *ptr = NULL, *p = NULL;

    if ((NULL == str) || (NULL == pDevIDPort))
    {
        return RT_ERR_FAILED;
    }

    osal_memset(pDevIDPort, 0, sizeof(*pDevIDPort));

    ptr = (uint8 *)strtok((char *)str, ",");
    while (NULL != ptr)
    {
        if (isdigit((int)*ptr))
        {
            p = (uint8 *)strchr((char *)ptr, ':');
            if (NULL == p)
            {   /* not correct format */
                _parse_err_return_with_pDevIDPorts();
            }

            _s2m_atoi(devID, ptr, ':');

            ptr = p + 1;

            p = (uint8 *)strchr((char *)ptr, '-');
            if (NULL == p)
            {   /* number only */
                _s2m_atoi(num, ptr, '\0');
                if (num > RTK_MAX_NUM_OF_PORTS)
                    _parse_err_return_with_pDevIDPorts();

                pDevIDPort->egr_port[pDevIDPort->num_ports].devID = devID;
                pDevIDPort->egr_port[pDevIDPort->num_ports].port = num;
                pDevIDPort->num_ports ++;
            }
            else
            {   /* number-number */
                _s2m_atoi(num, ptr, '-');
                ++p;
                _s2m_atoi(num_end, p, '\0');

                if (num > RTK_MAX_NUM_OF_PORTS || num_end > RTK_MAX_NUM_OF_PORTS)
                    _parse_err_return_with_pDevIDPorts();
                if (num_end > num)
                {
                    for (i = num; i <= num_end; i++)
                    {
                        pDevIDPort->egr_port[pDevIDPort->num_ports].devID = devID;
                        pDevIDPort->egr_port[pDevIDPort->num_ports].port = i;
                        pDevIDPort->num_ports ++;
                    }
                }
                else
                {
                    for (i = num_end; i <= num; i++)
                    {
                        pDevIDPort->egr_port[pDevIDPort->num_ports].devID = devID;
                        pDevIDPort->egr_port[pDevIDPort->num_ports].port = i;
                        pDevIDPort->num_ports ++;
                    }
                }
            }
        }
        else
            _parse_err_return_with_pDevIDPorts();

        ptr = (uint8 *)strtok((char *)NULL, ",");
    }
    return RT_ERR_OK;
} /* end of diag_util_str2unitPortMask */

/* convert logical port mask to string, separated by ","s, string length of comma is DIAG_UTIL_PORT_MASK_STRING_LEN */
int32 diag_util_lPortMask2str (char *comma, rtk_portmask_t *pstLPortMask)
{
    int32   first = 0;
    int32   begin = 0;
    int32   end = 0;
    uint32  i = 0;
    char    buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    if ((NULL == comma) || (NULL == pstLPortMask))
    {
        return RT_ERR_FAILED;
    }

    osal_memset(buf, 0, UTIL_PORT_MASK_BUFFER_LENGTH);

    comma[0] = '\0';

    first = 1;
    begin = -1;
    end = -1;

    for (i = 0; i <= RTK_MAX_NUM_OF_PORTS; ++i)
    {
        if (RT_ERR_OK == _diag_util_port2LPortMask_get(pstLPortMask, i))
        {

            if (1 == first)
            {
                first = 0;
            }

            if (-1 == begin)
            {
                begin = end = i;
            }
            else
            {
                end = i;
            }

        }
        else
        {
            if ((0 == first) && (begin != -1))
            {
                first = -1;
            }
            else if ((-1 == first) && (begin != -1))
            {
                sprintf((char *)buf, ",");
                if ((strlen(comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
                {
                    return RT_ERR_FAILED;
                }
                strcat(comma, buf);
            }

            if ((begin != -1) && (begin == end))
            {
                sprintf(buf, "%d", begin);
                if ((strlen(comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
                {
                    return RT_ERR_FAILED;
                }
                strcat(comma, buf);
            }
            else if (begin != -1)
            {
                sprintf(buf, "%d-%d", begin, end);
                if ((strlen(comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
                {
                    return RT_ERR_FAILED;
                }
                strcat(comma, buf);
            }

            begin = -1;
            end = -1;
        }
    }

    if ((begin != -1) || (end != -1))
    {
        if (-1 == first)
        {
            sprintf(buf, ",");
            if ((strlen(comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
            {
                return RT_ERR_FAILED;
            }
            strcat(comma, buf);
        }
        if (begin == end)
        {
            sprintf(buf, "%d", begin);
            if ((strlen(comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
            {
                return RT_ERR_FAILED;
            }
            strcat(comma, buf);
        }
        else
        {
            sprintf(buf, "%d-%d", begin, end);
            if ((strlen((char *)comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
            {
                return RT_ERR_FAILED;
            }
            strcat(comma, buf);
        }
    }

    return RT_ERR_OK;
} /* end of diag_util_lPortMask2str */


void diag_util_port_min_max_get(rtk_portmask_t *pPortmask, uint32 *pMin, uint32 *pMax)
{
    int32           i;

    *pMin = RTK_MAX_NUM_OF_PORTS;
    *pMax = 0;

    for (i = 0; i < RTK_MAX_NUM_OF_PORTS; i++)
    {
        if (!RTK_PORTMASK_IS_PORT_SET(*pPortmask, i))
            continue;
        *pMin = i;
        break;
    }

    for (i = (RTK_MAX_NUM_OF_PORTS - 1); i >= 0; i--)
    {
        if (!RTK_PORTMASK_IS_PORT_SET(*pPortmask, i))
            continue;
        *pMax = i;
        break;
    }
}

int32 diag_util_extract_portlist(uint32 unit, char *pStr, diag_portlist_t *pPortlist)
{
    int32 ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    if('a' == pStr[0])  /* all */
    {
        osal_memcpy(&pPortlist->portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));
        diag_util_port_min_max_get(&pPortlist->portmask, &pPortlist->min, &pPortlist->max);
    }
    else if ('n' == pStr[0]) /* none */
    {
        osal_memset(&pPortlist->portmask, 0, sizeof(rtk_portmask_t));
        diag_util_port_min_max_get(&pPortlist->portmask, &pPortlist->min, &pPortlist->max);
    }
    else
    {
        if ((ret = diag_util_str2LPortMask(pStr, &pPortlist->portmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DIAGSHELL), "port list=%s", pStr);
            return ret;
        }
        pPortlist->min = 0;
        pPortlist->max = RTK_MAX_NUM_OF_PORTS - 1;
    }

    return ret;
}


int32 diag_util_extract_trklist(uint32 unit, char *pStr, diag_trklist_t *pTrktlist)
{
    int32 ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;
    uint32 i;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }
    pTrktlist->max = devInfo.capacityInfo.max_num_of_trunk;
    _diag_util_ltrkMask_clear(&pTrktlist->trkmask);

    if('a' == pStr[0])  /*all*/
    {
        for(i=0; i<devInfo.capacityInfo.max_num_of_trunk; i++)
            _diag_util_trunk2LTrunkMask_set(&pTrktlist->trkmask, i);
    }
    else if ('n' == pStr[0])    /*none*/
    {
        _diag_util_ltrkMask_clear(&pTrktlist->trkmask);
    }
    else
    {
        if ((ret = diag_util_str2LTrkMask(pStr, &pTrktlist->trkmask)) != RT_ERR_OK)
        {
            diag_util_printf("trk list ERROR!\n");
            RT_ERR(ret, (MOD_DIAGSHELL), "trk list=%s", pStr);
            return ret;
        }
    }

    return ret;
}

static int32
_diag_util_mask_clear(diag_bmp_t *pBmp)
{
    uint32  i = 0;

    for (i = 0; i < DIGA_BMP_WIDTH(DIAG_MASK_MAX_LEN); ++i)
    {
        pBmp->bits[i] = 0;
    }

    return RT_ERR_OK;
} /* end of _diag_util_mask_clear */

static int32
_diag_util_index2Mask_set(diag_bmp_t *pBmp, unsigned short index)
{
    if (index > DIAG_MASK_MAX_LEN - 1)
    {
        return RT_ERR_FAILED;
    }

    pBmp->bits[index / DIAG_BMP_BIT_LEN] |= (1 << (index % DIAG_BMP_BIT_LEN));

    return RT_ERR_OK;
} /* end of _diag_util_index2Mask_set */

int32 diag_util_str2Mask(char *str, diag_mask_t *pMask)
{
    uint32      i = 0;
    uint32      num = 0;
    uint32      num_end = 0;
    uint8       *ptr = NULL, *p = NULL;
    diag_bmp_t  *mask;

    if ((NULL == str) || (NULL == pMask))
    {
        return RT_ERR_FAILED;
    }

    mask = &pMask->mask;

    _diag_util_mask_clear(mask);

    ptr = (uint8 *)strtok((char *)str, ",");
    while (NULL != ptr)
    {
        if (isdigit((int)*ptr))
        {
            p = (uint8 *)strchr((char *)ptr, '-');
            if (NULL == p)
            {   /* number only */
                _s2m_atoi(num, ptr, '\0');
                if (num >= DIAG_MASK_MAX_LEN)
                    return RT_ERR_FAILED;

                _diag_util_index2Mask_set(mask, num);
            }
            else
            {   /* number-number */
                _s2m_atoi(num, ptr, '-');
                ++p;
                _s2m_atoi(num_end, p, '\0');

                if (num >= DIAG_MASK_MAX_LEN)
                    return RT_ERR_FAILED;

                if (num_end >= DIAG_MASK_MAX_LEN)
                    return RT_ERR_FAILED;

                if (num_end > num)
                {
                    for (i = num; i <= num_end; i++)
                    {
                        _diag_util_index2Mask_set(mask, i);
                    }
                }
                else
                {
                    for (i = num_end; i <= num; i++)
                    {
                        _diag_util_index2Mask_set(mask, i);
                    }
                }
            }
        }
        else
            return RT_ERR_FAILED;

        ptr = (uint8 *)strtok(NULL, ",");
    }
    return RT_ERR_OK;
} /* end of diag_util_str2Mask */

int32 diag_util_extract_mask(uint32 unit, char *pStr, uint32 type,
    diag_mask_t *pMask)
{
    uint32                  i;
    int32                   ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t    devInfo;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));

    if ((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    switch (type)
    {
        case DIAG_MASKTYPE_QUEUE:
            pMask->min = 0;
            pMask->max = devInfo.capacityInfo.max_num_of_cpuQueue - 1;
            break;
        case DIAG_MASKTYPE_DSCP:
            pMask->min = 0;
            pMask->max = RTK_VALUE_OF_DSCP_MAX;
            break;
        case DIAG_MASKTYPE_UNIT:
            pMask->min = 0;
            pMask->max = RTK_MAX_NUM_OF_UNIT - 1;
            break;
        case DIAG_MASKTYPE_VLAN:
            pMask->min = 0;
            pMask->max = 4094;
            break;
        default:
            diag_util_printf("mask type input ERROR!\n");
            return RT_ERR_FAILED;
    }

    if('a' == pStr[0])
    {
        _diag_util_mask_clear(&pMask->mask);
        for (i = pMask->min; i <= pMask->max; ++i)
        {
            _diag_util_index2Mask_set(&pMask->mask, i);
        }
    }
    else
    {
        if ((ret = diag_util_str2Mask(pStr, pMask)) != RT_ERR_OK)
        {
            diag_util_printf("mask ERROR!\n");
            RT_ERR(ret, (MOD_DIAGSHELL), "mask=%s", pStr);
            return ret;
        }
    }

    return ret;
}

/*
 * On success, the function returns the converted integral number as a unsigned long int value.
 * If no valid conversion could be performed, a zero value is returned.
 */
int32
diag_util_str2ul(uint32 *ul, const char *str)
{
    uint32 value, base= 10;

    if ((NULL == ul) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    if(('0' == str[0]) && ('X' == toupper(str[1])))
    {
        str += 2;
        base = 16;
    }

    while(isxdigit(*str) && (value = isdigit(*str) ? (*str - '0') : (toupper(*str) - 'A' + 10)) < base)
    {
        *ul = (*ul * base) + value;
        str++;
    }

    return RT_ERR_OK;
}

/*
 * getnext -- get the next token
 *
 * Parameters:
 *   src: pointer to the start of the source string
 *   separater: the symbol used to separate the token
 *   dest: destination of the next token to be placed
 *
 * Returns:
 *   length of token (-1 when failed)
 */
static int32
_diag_util_getnext (char *src, int32 separator, char *dst)
{
    int32   len = 0;
    char    *c = NULL;

    if ((NULL == src) || (NULL == dst))
    {
        return -1;
    }

    c = strchr(src, (int)separator);
    if (NULL == c)
    {
        strcpy(dst, src);
        return -1;
    }
    len = c - src;
    strncpy(dst, src, len);
    dst[len] = '\0';

    return len + 1;
} /* end of _diag_util_getnext */

/* Convert MAC address from string to unsigned char array */
int32
diag_util_str2mac (uint8 *mac, char *str)
{
    int32   len = 0;
    uint32  i = 0;
    char    *ptr = str;
    char    buf[UTIL_STRING_BUFFER_LENGTH];

    if ((NULL == mac) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    osal_memset(buf, 0, UTIL_STRING_BUFFER_LENGTH);

    for (i = 0; i < 5; ++i)
    {
        if ((len = _diag_util_getnext(ptr, ':', buf)) == -1 &&
            (len = _diag_util_getnext(ptr, '-', buf)) == -1)
        {
            return RT_ERR_FAILED; /* parse error */
        }
        mac[i] = strtol(buf, NULL, 16);
        ptr += len;
    }
    mac[5] = strtol(ptr, NULL, 16);

    return RT_ERR_OK;
} /* end of diag_util_str2mac */

int32
diag_util_mac2str (char *str, const uint8 *mac)
{
    if ((NULL == mac) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return RT_ERR_OK;
} /* end of diag_util_mac2str */

/* convert IP address from number to string. Length of ip_str must more than 15 characters*/
int32
diag_util_ip2str(char *str, uint32 ip)
{
    if (NULL == str)
    {
        return RT_ERR_FAILED;
    }

    sprintf((char *)str, "%d.%d.%d.%d", ((ip>>24)&0xff), ((ip>>16)&0xff), ((ip>>8)&0xff), (ip&0xff));

    return RT_ERR_OK;
}

/* convert IP address from string to number */
int32
diag_util_str2ip (uint32 *ip, char *str)
{
    int32   len = 0;
    uint32  i = 0;
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    char    *ptr = str;
    char    buf[UTIL_STRING_BUFFER_LENGTH];

    if ((NULL == ip) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    osal_memset(ip_tmp_buf, 0, sizeof(uint32) * UTIL_IP_TMP_BUFFER_LENGTH);
    osal_memset(buf, 0, UTIL_STRING_BUFFER_LENGTH);

    for (i = 0; i < 3; ++i)
    {
        if ((len = _diag_util_getnext(ptr, '.', buf)) == -1)
        {
            return RT_ERR_FAILED; /* parse error */
        }

        ip_tmp_buf[i] = atoi(buf);
        if (ip_tmp_buf[i] > 255)
        {
            return RT_ERR_FAILED; /* parse error */
        }

        ptr += len;
    }
    ip_tmp_buf[3] = atoi(ptr);
    if (ip_tmp_buf[3] > 255)
    {
        return RT_ERR_FAILED; /* parse error */
    }

    *ip = (ip_tmp_buf[0] << 24) + (ip_tmp_buf[1] << 16) + (ip_tmp_buf[2] << 8) + ip_tmp_buf[3];

    return RT_ERR_OK;
} /* end of diag_util_str2Ip */

/* convert IPv6 address from number to string. Length of ipv6_str must more than 39 characters*/
int32
diag_util_ipv62str(char *str, const uint8 *ipv6)
{
    uint32  i;
    uint16  ipv6_ptr[UTIL_IPV6_TMP_BUFFER_LENGTH] = {0};

    if ((NULL == str) || (NULL == ipv6))
    {
        return RT_ERR_FAILED;
    }

    for (i = 0; i < UTIL_IPV6_TMP_BUFFER_LENGTH ;i++)
    {
        ipv6_ptr[i] = ipv6[i*2+1];
        ipv6_ptr[i] |=  ipv6[i*2] << 8;
    }
    sprintf((char *)str, "%x:%x:%x:%x:%x:%x:%x:%x", ipv6_ptr[0], ipv6_ptr[1], ipv6_ptr[2], ipv6_ptr[3]
    , ipv6_ptr[4], ipv6_ptr[5], ipv6_ptr[6], ipv6_ptr[7]);

    return RT_ERR_OK;
}

/* convert IPv6 address from string to number. Length of ipv6_addr must more than 16 characters */
int32
diag_util_str2ipv6(uint8 *ipv6, const char *str)
{
#define IN6ADDRSZ 16
#define INT16SZ     2
    static const uint8 xdigits_l[] = "0123456789abcdef",
              xdigits_u[] = "0123456789ABCDEF";
    uint8 tmp[IN6ADDRSZ], *tp, *endp, *colonp;
    const uint8 *xdigits;
    int ch, saw_xdigit;
    int val;

    if ((NULL == str) || (NULL == ipv6))
    {
        return RT_ERR_FAILED;
    }

    osal_memset((tp = tmp), '\0', IN6ADDRSZ);
    endp = tp + IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*str == ':')
        if (*++str != ':')
            return (RT_ERR_FAILED);
    saw_xdigit = 0;
    val = 0;
    while ((ch = *str++) != '\0') {
        const uint8 *pch;

        if ((pch = (uint8 *)strchr((char *)(xdigits = xdigits_l), ch)) == NULL)
            pch = (uint8 *)strchr((char *)(xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (RT_ERR_FAILED);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            if (!saw_xdigit) {
                if (colonp)
                    return (RT_ERR_FAILED);
                colonp = tp;
                continue;
            }
            if (tp + INT16SZ > endp)
                return (RT_ERR_FAILED);
            *tp++ = (uint8) (val >> 8) & 0xff;
            *tp++ = (uint8) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        return (RT_ERR_FAILED);
    }
    if (saw_xdigit) {
        if (tp + INT16SZ > endp)
            return (RT_ERR_FAILED);
        *tp++ = (uint8) (val >> 8) & 0xff;
        *tp++ = (uint8) val & 0xff;
    }
    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;

        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (RT_ERR_FAILED);
    osal_memcpy(ipv6, tmp, IN6ADDRSZ);
    return (RT_ERR_OK);
}/* end of diag_util_str2Ipv6 */

int32
diag_util_ip2str_format(char *str, uint32 ip, uint32 blankWidth)
{
    uint8   i;
    uint8   len;

    if (NULL == str)
    {
        return RT_ERR_FAILED;
    }

    sprintf((char *)str, "%d.%d.%d.%d", ((ip>>24)&0xff), ((ip>>16)&0xff), ((ip>>8)&0xff), (ip&0xff));
    len = strlen((char *)str);

    if (blankWidth > len)
    {
        for (i = 0; i < (blankWidth - len);i++)
        {
            strcat((char *)str, " ");
        }
    }

    return RT_ERR_OK;
}

/* convert ACTION_xxx to string */
char *
diag_util_act2str(rtk_action_t action)
{
    switch (action)
    {
        case ACTION_FORWARD:
            return "Forward";

        case ACTION_DROP:
            return "Drop";

        case ACTION_TRAP2CPU:
            return "Trap to CPU";

        case ACTION_COPY2CPU:
            return "Copy to CPU";

        case ACTION_TRAP2MASTERCPU:
            return "Trap to Master CPU";

        case ACTION_COPY2MASTERCPU:
            return "Copy to Master CPU";
        default:
            break;
    }

    return "(Error)";
}

/* Check if the MAC address is a broadcast address or not */
int32
diag_util_isBcastMacAddr(uint8 *mac)
{
    uint32 i = 0;

    if (NULL == mac)
    {
        return FALSE;
    }

    for (i = 0; i < 6; i++)
    {
        if (0xFF == *(mac + i))
        {
            continue;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
} /* end of diag_util_isBcastMacAddr */

/* Check if the MAC address is a multicast address or not */
int32
diag_util_isMcastMacAddr(uint8 *mac)
{
    if (NULL == mac)
    {
        return FALSE;
    }

    if (0x1 == (mac[0] & 0x1))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
} /* end of diag_util_isMcastMacAddr */

static void
diag_util_set_keypress (void)
{
#if defined(__linux__)
    struct termios  new_settings;

#if defined(__linux__)
    tcgetattr(0, &stored_settings);
#endif
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
#if defined(__linux__)
    tcgetattr(0, &stored_settings);
#endif
    new_settings.c_cc[VMIN] = 1;
#if defined(__linux__)
    tcsetattr(0, TCSANOW, &new_settings);
#endif
#endif
    return;
} /* end of diag_util_set_keypress */

static void
diag_util_reset_keypress(void)
{
#if defined(__linux__)
    tcsetattr(0, TCSANOW, &stored_settings);
#endif
    return;
} /* end of diag_util_reset_keypress */

int32
diag_util_mprintf(char *fmt, ...)
{
    va_list     args;

    if (stopped)
    {
        return RT_ERR_ABORT;
    }

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    ++lines;
    if ((max_lines != 0) && (lines > max_lines))
    {
        int32    ch;

        lines = 1;
        printf("\n--More--");
        diag_util_set_keypress();
        ch = getchar();
        if (ch < 0)
            return RT_ERR_ABORT;
        else
            ch &= 0xFF;
        putchar(8);
        diag_util_reset_keypress();
        printf("\n");
        if (('Q' == ch) || ('q' == ch))
        {
            stopped = 1;
            return RT_ERR_ABORT;
        }
    }
    return RT_ERR_OK;
} /* end of diag_util_mprintf */

void
diag_util_mprintf_init (void)
{
    stopped = 0;
    lines = 0;
    return;
} /* end of diag_util_mprintf_init */



/*
 * Configure number of lines to display before "--More--" prompt.
 * Set to 0 will disable paging.
 */
void
diag_util_mprintf_paging_lines (unsigned int number)
{
    max_lines = number;
    diag_util_mprintf_init();
}

int32 diag_util_str2IntArray (uint8 *int_array, char *str, uint32 field_size)
{
    uint8  value = 0;
    uint32 str_idx, array_idx, hi_bits;

    if (!(int_array && str))
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");
        return  RT_ERR_FAILED;
    }

    if (!((strlen((char *)str) > 2) && ('0' == str[0]) && ('x' == str[1])))
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");
        return  RT_ERR_FAILED;
    }

    value = 0;
    array_idx = field_size - 1;
    hi_bits = 0;

    /* exclude 0x head */
    for (str_idx = (strlen((char *)str) - 1); str_idx >= 2; str_idx--)
    {
        if (('0' <= str[str_idx]) && ('9' >= str[str_idx]))
        {
            value = str[str_idx] - '0';
        }
        else if (('a' <= str[str_idx]) && ('f' >= str[str_idx]))
        {
            value = str[str_idx] - 'a' + 10;
        }
        else if (('A' <= str[str_idx]) && ('F' >= str[str_idx]))
        {
            value = str[str_idx] - 'A' + 10;
        }
        else
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DIAGSHELL), "");
            return  RT_ERR_FAILED;
        }

        if(hi_bits == 1)
        {
            int_array[array_idx] = int_array[array_idx] + (value  << 4);
            hi_bits = 0;
            array_idx--;
        }
        else
        {
            int_array[array_idx] = value;
            hi_bits = 1;
        }
    }

    return RT_ERR_OK;
}

int32 diag_util_ipv6IsZero_ret(rtk_ipv6_addr_t *pIp6)
{
    int32   i;

    for (i=0; i<IPV6_ADDR_LEN; i++)
    {
        if (0 != pIp6->octet[i])
            return FALSE;
    }

    return TRUE;
}



char *
diag_util_serdesMode2str(rt_serdesMode_t sdsMode)
{
    switch (sdsMode)
    {
        case RTK_MII_NONE:
            return "None";
        case RTK_MII_DISABLE:
            return "Disabled";
        case RTK_MII_10GR:
            return "10GBase-R";
        case RTK_MII_RXAUI:
            return "RXAUI";
        case RTK_MII_RXAUI_LITE:
            return "R-XAUI-Lite";
        case RTK_MII_RXAUISGMII_AUTO:
            return "R-XAUI-SGMII-AUTO";
        case RTK_MII_RXAUI1000BX_AUTO:
            return "RXAUI-1000Base-X-AUTO";
        case RTK_MII_RSGMII_PLUS:
            return "RSGMII-Plus";
        case RTK_MII_SGMII:
            return "SGMII";
        case RTK_MII_QSGMII:
            return "QSGMII";
        case RTK_MII_1000BX_FIBER:
            return "1000Base-X";
        case RTK_MII_100BX_FIBER:
            return "100Base-X";
        case RTK_MII_1000BX100BX_AUTO:
            return "1000Base-X-100Base-X-AUTO";
        case RTK_MII_10GR1000BX_AUTO:
            return "10GBase-R-1000Base-X-AUTO";
        case RTK_MII_10GRSGMII_AUTO:
            return "10GBase-R-SGMII-AUTO";
        case RTK_MII_XAUI:
            return "XAUI";
        case RTK_MII_RMII:
            return "RMII";
        case RTK_MII_SMII:
            return "SMII";
        case RTK_MII_SSSMII:
            return "SSSMII";
        case RTK_MII_RSGMII:
            return "RSGMII";
        case RTK_MII_XSMII:
            return "XSGMII";
        case RTK_MII_XSGMII:
            return "XSGMII";
        case RTK_MII_QHSGMII:
            return "QHSGMII";
        case RTK_MII_HISGMII:
            return "HISGMII";
        case RTK_MII_HISGMII_5G:
            return "HISGMII-5G";
        case RTK_MII_DUAL_HISGMII:
            return "Dual-HISGMII";
        case RTK_MII_2500Base_X:
            return "2500Base-X";
        case RTK_MII_RXAUI_PLUS:
            return "RXAUI-PLUS";
        case RTK_MII_USXGMII_10GSXGMII:
            return "USXGMII-10G-SXGMII";
        case RTK_MII_USXGMII_10GDXGMII:
            return "USXGMII-10G-DXGMII";
        case RTK_MII_USXGMII_10GQXGMII:
            return "USXGMII-10G-QXGMII";
        case RTK_MII_USXGMII_5GSXGMII:
            return "USXGMII-5G-SXGMII";
        case RTK_MII_USXGMII_5GDXGMII:
            return "USXGMII-5G-DXGMII";
        case RTK_MII_USXGMII_2_5GSXGMII:
            return "USXGMII-2.5G-SXGMII";
        case RTK_MII_USXGMII_1G:
            return "USXGMII-1G";
        case RTK_MII_USXGMII_100M:
            return "USXGMII-100M";
        case RTK_MII_USXGMII_10M:
            return "USXGMII-10M";
        case RTK_MII_5GR:
            return "5GBase-R";
        case RTK_MII_5GBASEX:
            return "5GBase-X";
        case RTK_MII_XFI_5G_ADAPT:
            return "XFI-5G-Adapt";
        case RTK_MII_XFI_2P5G_ADAPT:
            return "XFI-2.5G-Adapt";
        case RTK_MII_XFI_5G_CPRI:
            return "XFI-5G-CPRI";
        default:
            return "Unknown SerDes Type.";
    }
}

int32
diag_util_str2upper(char *str)
{
    char *p = str;
    while('\0' != *p)
    {
        *p = osal_toupper(*p);
        p++;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      diag_util_convert_mword_string_2_int32_array
 * Description:
 *      Convert a multi-word string to uint32 array
 *      string example: 0x7a808080-80808080, 7a808080-80808080
 * Input:
 *      string_ptr - multi-word string
 *      delim      - delimeter to separate multiple word
 *      cnt        - the maximum word number
 * Output:
 *      out_array  - output uint32 array
 *      cnt        - number of words
 * Return:
 *      RT_ERR_INPUT
 *      RT_ERR_OK
 */
int32 diag_util_convert_mword_string_2_int32_array(
    char *string_ptr,
    char *delim,
    uint32 *out_array,
    uint32 *cnt)
{
    char *pWord_str, *pSave=NULL;
    char *pStr = string_ptr;
    uint32 word_cnt = 0, i=0;

    if(osal_strncmp(string_ptr, "0x", 2) == 0)
    {  // bypass '0x'
        pStr = string_ptr+2;
    }

    while (((pWord_str = osal_strtok_r(pStr, delim, &pSave)) != NULL) &&
            (word_cnt < *cnt))
        {
        pStr = NULL;
        if(1 != osal_sscanf(pWord_str, "%x", &out_array[i]))
        {
            return RT_ERR_INPUT;
        }
        word_cnt++;
        i++;
        }

    *cnt = word_cnt;
    return RT_ERR_OK;
}

/* Parse input string to a range and a step value.
 * Format : start-end,step
 * Input: str
 * Output: val
 */
int32 diag_util_str2rangeStep(char *str, rt_valRangeStep_t *val)
{
    if(NULL == val)
    {
        return RT_ERR_NULL_POINTER;
    }
    if(3 == osal_sscanf(str, "%d-%d,%d", &val->start, &val->end, &val->step))
    {
        return RT_ERR_OK;
    }
    if(2 == osal_sscanf(str, "%d-%d", &val->start, &val->end))
    {
        val->step = 1;
        return RT_ERR_OK;
    }
    if(1 == osal_sscanf(str, "%d", &val->start))
    {
        val->step = 1;
        val->end = val->start;
        return RT_ERR_OK;
    }
    return RT_ERR_FAILED;
}




