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
 * Purpose : Define those public diag shell utility APIs.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag shell utility
 */

/*
 * Include Files
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <termio.h>
#endif /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
#include <common/debug/rt_log.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <diag_util.h>
#include <diag_om.h>



#include <hal/mac/mac_probe.h>


/*
 * Symbol Definition
 */
#define MAX_MORE_LINES  20



extern cparser_t main_parser;
/*
 * Data Declaration
 */
#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
static struct termios stored_settings;
#endif /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
static int lines = 0;
static int stopped = 0;

#define _parse_err_return()  \
    do { \
        _diag_util_lPortMask_clear(mask); \
        return RT_ERR_FAILED; \
    } while (0)

#define _s2m_atoi(NUM, PTR, ENDCHAR) \
    do { \
        (NUM) = 0; \
        do { \
            if (!isdigit((int) *(PTR))) \
                _parse_err_return (); \
            (NUM) = (NUM) * 10 + (int) *(PTR) - (int)'0'; \
        } while (*(++(PTR)) != (ENDCHAR)); \
    } while (0)


#define _mask_parse_err_return()  \
    do { \
        _diag_util_mask_clear(mask); \
        return RT_ERR_FAILED; \
    } while (0)



#define _s2mask_atoi(NUM, PTR, ENDCHAR) \
    do { \
        (NUM) = 0; \
        do { \
            if (!isdigit((int) *(PTR))) \
                _mask_parse_err_return(); \
            (NUM) = (NUM) * 10 + (int) *(PTR) - (int)'0'; \
        } while (*(++(PTR)) != (ENDCHAR)); \
    } while (0)


#define	_ishex(c)	(isdigit(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))


/*
 * Function Declaration
 */
static int32 _diag_util_lPortMask_clear(rtk_portmask_t *pstLPortMask);
static int32 _diag_util_port2LPortMask_set(rtk_portmask_t *pstLPortMask, unsigned char ucPortId);
static int32 _diag_util_port2LPortMask_get(rtk_portmask_t *pstLPortMask, unsigned char ucPortId);
static int32 _diag_util_getnext(uint8 *src, int32 separator, uint8 *dest, uint16 bufSize);
#if 0
void diag_util_printf(char *fmt, ...);
#endif
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
diag_util_portMaskRangeCheck(rtk_portmask_t *full_mask, rtk_portmask_t *test_mask)
{
    rtk_portmask_t invert_full_mask;

    uint32  i = 0;

    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; ++i)
    {
        invert_full_mask.bits[i] = ~(full_mask->bits[i]);
    }

    for (i = 0; i < RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST; ++i)
    {
        if((invert_full_mask.bits[i] & test_mask->bits[i]) != 0)
        {
        	return RT_ERR_FAILED;
        }
    }

	return RT_ERR_OK;
}/* end of diag_util_portMaskRangeCheck */

int32
diag_util_str2LPortMask(uint8 *str, rtk_portmask_t *mask)
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

    ptr = strtok(str, ",");
    while (NULL != ptr)
    {
        if (isdigit((int)*ptr))
        {
            p = strchr(ptr, '-');
            if (NULL == p)
            {   /* number only */
                _s2m_atoi(num, ptr, '\0');
#if 0 /* for DSL port will not pass this check */
                if (num > MAX_PHY_PORT)
                    _parse_err_return();
#endif
                _diag_util_port2LPortMask_set(mask, num);
            }
            else
            {   /* number-number */
                _s2m_atoi(num, ptr, '-');
                ++p;
                _s2m_atoi(num_end, p, '\0');
#if 0 /* for DSL port will not pass this check */
                if (num > MAX_PHY_PORT || num_end > MAX_PHY_PORT)
                    _parse_err_return();
#endif
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
        else if (!strncasecmp(ptr, "trunk", 5))
        {
            ptr += 5;
            p = strchr(ptr, '-');
            if (NULL == p)
            {
                _s2m_atoi(num, ptr, '\0');
#if 0 /* for DSL port will not pass this check */
                if (num > MAX_TRK_PORT)
                    _parse_err_return ();
#endif
                _diag_util_port2LPortMask_set(mask, num + MAX_PHY_N_CPU_PORT);
            }
            else
            {
                _s2m_atoi(num, ptr, '-');
                ++p;
                _s2m_atoi(num_end, p, '\0');
#if 0 /* for DSL port will not pass this check */
                if (num > MAX_TRK_PORT || num_end > MAX_TRK_PORT)
                    _parse_err_return();
#endif
                if (num_end > num)
                {
                    for (i = num; i <= num_end; i++)
                    {
                        _diag_util_port2LPortMask_set(mask, i + MAX_PHY_N_CPU_PORT);
                    }
                }
                else
                {
                    for (i = num_end; i <= num; i++)
                    {
                        _diag_util_port2LPortMask_set(mask, i + MAX_PHY_N_CPU_PORT);
                    }
                }
            }
        }
#ifdef CONFIG_SDK_RTL9607C
        else if(!strncasecmp(ptr, "cpu0", 4))
        {
            if(rtk_switch_phyPortId_get(RTK_PORT_CPU0,&i) != RT_ERR_OK)
            {
                diag_util_printf("Error! get CPU0 port id failure! %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            _diag_util_port2LPortMask_set(mask, i);
        }
        else if(!strncasecmp(ptr, "cpu1", 4))
        {
            if(rtk_switch_phyPortId_get(RTK_PORT_CPU1,&i) != RT_ERR_OK)
            {
                diag_util_printf("Error! get CPU1 port id failure! %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            _diag_util_port2LPortMask_set(mask, i);
        }
        else if(!strncasecmp(ptr, "cpu2", 4))
        {
            if(rtk_switch_phyPortId_get(RTK_PORT_CPU2,&i) != RT_ERR_OK)
            {
                diag_util_printf("Error! get CPU2 port id failure! %s %d\n", __FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            _diag_util_port2LPortMask_set(mask, i);
        }
        else if(!strncasecmp(ptr, "cpu", 3))
        {
            if(rtk_switch_phyPortId_get(RTK_PORT_CPU0,&i) != RT_ERR_OK)
            {
                diag_util_printf("Error! get CPU port id failure! %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            _diag_util_port2LPortMask_set(mask, i);
        }
#else
        else if(!strncasecmp(ptr, "cpu", 3))
        {
            if(rtk_switch_phyPortId_get(RTK_PORT_CPU,&i) != RT_ERR_OK)
            {
                diag_util_printf("Error! get CPU port id failure! %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            _diag_util_port2LPortMask_set(mask, i);
        }
#endif
        else if(!strncasecmp(ptr, "pon", 3))
        {
            if(rtk_switch_phyPortId_get(RTK_PORT_PON,&i) != RT_ERR_OK)
            {
                diag_util_printf("Error! get PON port id failure! %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            _diag_util_port2LPortMask_set(mask, i);
        }
        else
            _parse_err_return();

        ptr = strtok(NULL, ",");
    }
    return RT_ERR_OK;
} /* end of diag_util_str2LPortMask */

/* convert logical port mask to string, separated by ","s, string length of comma is DIAG_UTIL_PORT_MASK_STRING_LEN */
int32 diag_util_lPortMask2str (uint8 *comma, rtk_portmask_t *pstLPortMask)
{
    int32   first = 0;
    int32   begin = 0;
    int32   end = 0;
    uint32  i = 0;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    if ((NULL == comma) || (NULL == pstLPortMask))
    {
        return RT_ERR_FAILED;
    }

    memset(buf, 0, UTIL_PORT_MASK_BUFFER_LENGTH);

    comma[0] = '\0';

    first = 1;
    begin = -1;
    end = -1;
#if defined(CONFIG_LUNA_G3_SERIES)
    for (i = 0; i <= RTK_MAX_PORT_ID; ++i)
#else
    for (i = 0; i <= MAX_PORT; ++i)
#endif
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
                sprintf(buf, ",");
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
            if ((strlen(comma) + strlen(buf)) > DIAG_UTIL_PORT_MASK_STRING_LEN)
            {
                return RT_ERR_FAILED;
            }
            strcat(comma, buf);
        }
    }
#if 0
    for (i = MAX_PHY_N_CPU_PORT; i < MAX_LOGIC_PORT; ++i)
    {
        if (RT_ERR_OK == _diag_util_port2LPortMask_get(pstLPortMask, i))
        {
            if (1 == first)
            {
                first = 0;
                sprintf(buf, "Trunk%d", i - MAX_PORT - 1 + 1);

            }
            else
            {
                sprintf(buf, ",Trunk%d", i - MAX_PORT - 1 + 1);
            }
            strcat(comma, buf);
        }
    }
#endif

    if(comma[0]=='\0')
        sprintf(comma,"none");
    return RT_ERR_OK;
} /* end of diag_util_lPortMask2str */


void _diag_int2Str(unsigned char* strBuffer,unsigned char* strIdx,unsigned char num)
{
    unsigned char quo,quo2,rem, rem2;

    quo = num /10;
    rem = num %10;

    if(quo)
    {
        quo2 = quo/10;
        if(quo2)
        {
            rem2 = quo%10;
            *(strBuffer+*strIdx) = '0' + quo2;
            *strIdx = *strIdx + 1;
            *(strBuffer+*strIdx) = '0' + rem2;
            *strIdx = *strIdx + 1;
        }
        else
        {
            *(strBuffer+*strIdx) = '0' + quo;
            *strIdx = *strIdx + 1;
        }
    }

    *(strBuffer+*strIdx) = '0' + rem;

    *strIdx = *strIdx + 1;

}

int8 *diag_util_mask32tostr (uint32 mask)
{
    unsigned char strIdx;
    unsigned char idx;
    int8 preNum,curNum,fromNum;
    static uint8 strBuffer[32];

    preNum = -1;
    strIdx = 0;
    fromNum = -1;


    for(idx = 0; idx <32;idx ++)
    {
        if(mask & (1<<idx))
        {
            if(preNum>=0)
            {
                curNum = idx;

                if(curNum != (preNum+1))
                {
                    if(preNum != fromNum && fromNum>=0)
                    {
                        _diag_int2Str(strBuffer,&strIdx,fromNum);

                        *(strBuffer + strIdx) = '-';
                        strIdx ++;
                    }
                    _diag_int2Str(strBuffer,&strIdx,preNum);

                    *(strBuffer + strIdx) = ',';
                    strIdx ++;

                    preNum = curNum;
                    fromNum = preNum;

                }
                else
                {
                    preNum = curNum;
                }
            }
            else
            {
                preNum = idx;
                fromNum = preNum;
            }
        }
    }

    if(preNum>=0)
    {
        if(preNum != fromNum && fromNum>=0)
        {
            _diag_int2Str(strBuffer,&strIdx,fromNum);

            *(strBuffer + strIdx) = '-';
            strIdx ++;
        }
        _diag_int2Str(strBuffer,&strIdx,preNum);
    }


    if(0 == strIdx)
    {
        osal_strcpy(strBuffer,"");
    }
    else
    {
        *(strBuffer + strIdx) = 0;
    }

    return strBuffer;
}

void diag_util_mask2str (uint8 *strBuffer,uint32 mask)
{
    unsigned char strIdx;
    unsigned char idx;
    int8 preNum,curNum,fromNum;

    preNum = -1;
    strIdx = 0;
    fromNum = -1;


    for(idx = 0; idx <32;idx ++)
    {
        if(mask & (1<<idx))
        {
            if(preNum>=0)
            {
                curNum = idx;

                if(curNum != (preNum+1))
                {
                    if(preNum != fromNum && fromNum>=0)
                    {
                        _diag_int2Str(strBuffer,&strIdx,fromNum);

                        *(strBuffer + strIdx) = '-';
                        strIdx ++;
                    }
                    _diag_int2Str(strBuffer,&strIdx,preNum);

                    *(strBuffer + strIdx) = ',';
                    strIdx ++;

                    preNum = curNum;
                    fromNum = preNum;

                }
                else
                {
                    preNum = curNum;
                }
            }
            else
            {
                preNum = idx;
                fromNum = preNum;
            }
        }
    }

    if(preNum>=0)
    {
        if(preNum != fromNum && fromNum>=0)
        {
            _diag_int2Str(strBuffer,&strIdx,fromNum);

            *(strBuffer + strIdx) = '-';
            strIdx ++;
        }
        _diag_int2Str(strBuffer,&strIdx,preNum);
    }


    if(0 == strIdx)
    {
        osal_strcpy(strBuffer,"");
    }
    else
    {
        *(strBuffer + strIdx) = 0;
    }
}


int32 diag_util_extract_portlist(uint8 *pStr, uint32 type, diag_portlist_t *pPortlist)
{
    int32 ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;
	rtk_portmask_t	check_portmask;


    memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));



    if ((ret = diag_om_get_deviceInfo(&devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    if('a' == pStr[0])
    {
        switch(type)
        {
            case DIAG_PORTTYPE_DSL:
                pPortlist->min = devInfo.dsl.min;
                pPortlist->max = devInfo.dsl.max;
                osal_memcpy(&pPortlist->portmask, &(devInfo.dsl.portmask), sizeof(rtk_portmask_t));
                break;

            case DIAG_PORTTYPE_EXT:
				pPortlist->min = devInfo.ext.min;
				pPortlist->max = devInfo.ext.max;
				osal_memcpy(&pPortlist->portmask, &(devInfo.ext.portmask), sizeof(rtk_portmask_t));
                break;

            case DIAG_PORTTYPE_CPU:
                pPortlist->min = devInfo.cpu.min;
                pPortlist->max = devInfo.cpu.max;
                osal_memcpy(&pPortlist->portmask, &(devInfo.cpu.portmask), sizeof(rtk_portmask_t));
                break;

            default:
				pPortlist->min = devInfo.ether.min;
				pPortlist->max = devInfo.ether.max;
				osal_memcpy(&pPortlist->portmask, &(devInfo.ether.portmask), sizeof(rtk_portmask_t));
                break;
        }
    }
#if !defined(CONFIG_LUNA_G3_SERIES)
    else if('d' == pStr[0])
    {
        pPortlist->min = devInfo.dsl.min;
        pPortlist->max = devInfo.dsl.max;
        osal_memcpy(&pPortlist->portmask, &(devInfo.dsl.portmask), sizeof(rtk_portmask_t));
    }
    else if('e' == pStr[0])
    {
        pPortlist->min = devInfo.ext.min;
        pPortlist->max = devInfo.ext.max;
        osal_memcpy(&pPortlist->portmask, &(devInfo.ext.portmask), sizeof(rtk_portmask_t));
    }
    else if('c' == pStr[0])
    {
        pPortlist->min = devInfo.cpu.min;
        pPortlist->max = devInfo.cpu.max;
        osal_memcpy(&pPortlist->portmask, &(devInfo.cpu.portmask), sizeof(rtk_portmask_t));
    }
    else if ('s' == pStr[0])
    {
        pPortlist->min = devInfo.swpbo_lb.min;
        pPortlist->max = devInfo.swpbo_lb.max;
        osal_memcpy(&pPortlist->portmask, &(devInfo.swpbo_lb.portmask), sizeof(rtk_portmask_t));
    }
	else if ('t' == pStr[0]) 
	{
        pPortlist->min = devInfo.all.min;
        pPortlist->max = devInfo.all.max;
        osal_memcpy(&pPortlist->portmask, &(devInfo.all.portmask), sizeof(rtk_portmask_t));
	}
    else if('n' == pStr[0])
    {/*for none key word*/
        osal_memset(&pPortlist->portmask, 0x0, sizeof(rtk_portmask_t));
        osal_memset(&check_portmask, 0x0, sizeof(rtk_portmask_t));



        switch(type)
        {
            case DIAG_PORTTYPE_FE:
                pPortlist->min = devInfo.fe.min;
                pPortlist->max = devInfo.fe.max;
                break;

            case DIAG_PORTTYPE_GE:
                pPortlist->min = devInfo.ge.min;
                pPortlist->max = devInfo.ge.max;
                break;

            case DIAG_PORTTYPE_GE_COMBO:
                pPortlist->min = devInfo.ge_combo.min;
                pPortlist->max = devInfo.ge_combo.max;
                break;

            case DIAG_PORTTYPE_SERDES:
                pPortlist->min = devInfo.serdes.min;
                pPortlist->max = devInfo.serdes.max;
                break;

            case DIAG_PORTTYPE_ETHER:
                pPortlist->min = devInfo.ether.min;
                pPortlist->max = devInfo.ether.max;
                break;

            case DIAG_PORTTYPE_DSL:
                pPortlist->min = devInfo.dsl.min;
                pPortlist->max = devInfo.dsl.max;
                break;

            case DIAG_PORTTYPE_EXT:
                pPortlist->min = devInfo.ext.min;
                pPortlist->max = devInfo.ext.max;
                break;

            case DIAG_PORTTYPE_CPU:
                pPortlist->min = devInfo.cpu.min;
                pPortlist->max = devInfo.cpu.max;
                break;

            case DIAG_PORTTYPE_SWPBO:
                pPortlist->min = devInfo.swpbo_lb.min;
                pPortlist->max = devInfo.swpbo_lb.max;
                break;

            case DIAG_PORTTYPE_ALL:
                pPortlist->min = devInfo.all.min;
                pPortlist->max = devInfo.all.max;
                break;

            default:
                diag_util_printf("port type input ERROR!\n");
                return RT_ERR_FAILED;
        }

    }
#endif
    else
    {


        switch(type)
        {
            case DIAG_PORTTYPE_FE:
                pPortlist->min = devInfo.fe.min;
                pPortlist->max = devInfo.fe.max;
                check_portmask = devInfo.fe.portmask;
                break;

            case DIAG_PORTTYPE_GE:
                pPortlist->min = devInfo.ge.min;
                pPortlist->max = devInfo.ge.max;
                check_portmask = devInfo.ge.portmask;
                break;

            case DIAG_PORTTYPE_GE_COMBO:
                pPortlist->min = devInfo.ge_combo.min;
                pPortlist->max = devInfo.ge_combo.max;
                check_portmask = devInfo.ge.portmask;
                break;

            case DIAG_PORTTYPE_SERDES:
                pPortlist->min = devInfo.serdes.min;
                pPortlist->max = devInfo.serdes.max;
                check_portmask = devInfo.serdes.portmask;
                break;

            case DIAG_PORTTYPE_ETHER:
                pPortlist->min = devInfo.ether.min;
                pPortlist->max = devInfo.ether.max;
                check_portmask = devInfo.ether.portmask;
                break;

            case DIAG_PORTTYPE_DSL:
                pPortlist->min = devInfo.dsl.min;
                pPortlist->max = devInfo.dsl.max;
                check_portmask = devInfo.dsl.portmask;
                break;

            case DIAG_PORTTYPE_EXT:
                pPortlist->min = devInfo.ext.min;
                pPortlist->max = devInfo.ext.max;
                check_portmask = devInfo.ext.portmask;
                break;

            case DIAG_PORTTYPE_CPU:
                pPortlist->min = devInfo.cpu.min;
                pPortlist->max = devInfo.cpu.max;
                check_portmask = devInfo.cpu.portmask;
                break;

            case DIAG_PORTTYPE_SWPBO:
                pPortlist->min = devInfo.swpbo_lb.min;
                pPortlist->max = devInfo.swpbo_lb.max;
                check_portmask = devInfo.swpbo_lb.portmask;
                break;

            case DIAG_PORTTYPE_ALL:
                pPortlist->min = devInfo.all.min;
                pPortlist->max = devInfo.all.max;
                check_portmask = devInfo.all.portmask;
                break;

            default:
                diag_util_printf("port type input ERROR!\n");
                return RT_ERR_FAILED;
        }


        if ((ret = diag_util_str2LPortMask(pStr, &pPortlist->portmask)) != RT_ERR_OK)
        {
            diag_util_printf("port list ERROR!\n");
            RT_ERR(ret, (MOD_DIAGSHELL), "port list=%s", pStr);
            return ret;
        }

    #if defined(CONFIG_SDK_RTL8198X)
        if (pPortlist->portmask.bits[0] & (1 << 0x18)) {
            if (pPortlist->max < 0x18)
                pPortlist->max = 0x18;

            if (!(check_portmask.bits[0] & pPortlist->portmask.bits[0]))
                check_portmask.bits[0] |= (1 << 0x18);
        }
    #endif

        /*check if given port is inside port range*/
        if ((ret = diag_util_portMaskRangeCheck(&check_portmask, &pPortlist->portmask)) != RT_ERR_OK)
        {
            diag_util_printf("port list ERROR! port is out of range!\n");
            return ret;
        }

    }


    return ret;
}




int32 diag_util_reverse_portlist(diag_portType_t type,rtk_portmask_t *pPlst,rtk_portmask_t *pRevesedPlst)
{
    int32 ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t devInfo;
	rtk_portmask_t check_portmask;
	rtk_portmask_t apply_portmask;
	int32 min;
	int32 max;
	int port;

    memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    memcpy(&check_portmask,pPlst,sizeof(rtk_portmask_t));

    if ((ret = diag_om_get_deviceInfo(&devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }


    switch(type)
    {
        case DIAG_PORTTYPE_ETHER:
            min = devInfo.ether.min;
            max = devInfo.ether.max;
            break;

        case DIAG_PORTTYPE_DSL:
            min = devInfo.dsl.min;
            max = devInfo.dsl.max;
            break;

        case DIAG_PORTTYPE_EXT:
            min = devInfo.ext.min;
            max = devInfo.ext.max;
            break;

        case DIAG_PORTTYPE_ALL:
            min = devInfo.all.min;
            max = devInfo.all.max;
            break;

        case DIAG_PORTTYPE_FE:
        case DIAG_PORTTYPE_GE:
        case DIAG_PORTTYPE_GE_COMBO:
        case DIAG_PORTTYPE_SERDES:
        default:
            diag_util_printf("port type input ERROR!\n");
            return RT_ERR_FAILED;
    }

    memset(&apply_portmask,0x0,sizeof(rtk_portmask_t));
    for (port = min; port <= max; port++)
    {
        if (!RTK_PORTMASK_IS_PORT_SET(check_portmask, port))
        {
            RTK_PORTMASK_PORT_SET(apply_portmask, port);
        }
    }


    memcpy(pRevesedPlst,&apply_portmask,sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}


static int32
_diag_util_mask_clear(rtk_bmp_t *pBmp)
{
    uint32  i = 0;

    for (i = 0; i < RTK_BMP_WIDTH(RTK_MASK_MAX_LEN); ++i)
    {
        pBmp->bits[i] = 0;
    }

    return RT_ERR_OK;
} /* end of _diag_util_mask_clear */

static int32
_diag_util_index2Mask_set(rtk_bmp_t *pBmp, unsigned char index)
{
    if (index > RTK_MASK_MAX_LEN - 1)
    {
        return RT_ERR_FAILED;
    }

    pBmp->bits[index / RTK_BMP_BIT_LEN] |= (1 << (index % RTK_BMP_BIT_LEN));

    return RT_ERR_OK;
} /* end of _diag_util_index2Mask_set */

int32 diag_util_str2Mask(uint8 *str, diag_mask_t *pMask)
{
    uint32      i = 0;
    uint32      num = 0;
    uint32      num_end = 0;
    uint8       *ptr = NULL, *p = NULL;
    rtk_bmp_t  *mask;

    if ((NULL == str) || (NULL == pMask))
    {
        return RT_ERR_FAILED;
    }

    mask = &pMask->mask;

    _diag_util_mask_clear(mask);

    ptr = strtok(str, ",");
    while (NULL != ptr)
    {
        if (isdigit((int)*ptr))
        {
            p = strchr(ptr, '-');
            if (NULL == p)
            {   /* number only */
                _s2mask_atoi(num, ptr, '\0');
                if (num < pMask->min || num > pMask->max)
                    return RT_ERR_FAILED;

                _diag_util_index2Mask_set(mask, num);
            }
            else
            {   /* number-number */
                _s2mask_atoi(num, ptr, '-');
                ++p;
                _s2mask_atoi(num_end, p, '\0');

                if (num < pMask->min || num > pMask->max)
                    return RT_ERR_FAILED;

                if (num_end < pMask->min || num_end > pMask->max)
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

        ptr = strtok(NULL, ",");
    }

    return RT_ERR_OK;
} /* end of diag_util_str2Mask */

int32 diag_util_extract_mask(
    uint8 *pStr,
    diag_maskType_t type,
    diag_mask_t *pMask)
{
    uint32                  i;
    int32                   ret = RT_ERR_FAILED;
    rtk_switch_devInfo_t    devInfo;

    memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));

    if ((ret = diag_om_get_deviceInfo(&devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return ret;
    }

    switch (type)
    {
        case DIAG_MASKTYPE_QUEUE:
            pMask->min = 0;
            pMask->max = devInfo.capacityInfo.max_num_of_queue - 1;
            break;
        case DIAG_MASKTYPE_FID:
            pMask->min = 0;
            pMask->max = devInfo.capacityInfo.vlan_fid_max;
            break;
        case DIAG_MASKTYPE_MASK8:
            pMask->min = 0;
            pMask->max = 7;
            break;
        case DIAG_MASKTYPE_MASK16:
            pMask->min = 0;
            pMask->max = 15;
            break;
        case DIAG_MASKTYPE_MASK32:
            pMask->min = 0;
            pMask->max = 31;
            break;
        case DIAG_MASKTYPE_MASK64:
            pMask->min = 0;
            pMask->max = 63;
            break;
        case DIAG_MASKTYPE_MASK128:
            pMask->min = 0;
            pMask->max = 127;
            break;
        default:
            diag_util_printf("mask type input ERROR!\n");
            return RT_ERR_FAILED;
    }
	/*none*/
	if('n' == pStr[0])
	{
		memset(&pMask->mask, 0, sizeof(rtk_bmp_t));
	}
    else if('a' == pStr[0])
    {
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
_diag_util_getnext (uint8 *src, int32 separator, uint8 *dest, uint16 bufSize)
{
    int32   len = 0;
    uint8   *c = NULL;

    if ((NULL == src) || (NULL == dest))
    {
        return -1;
    }

    c = strchr(src, separator);
    if (NULL == c)
    {
        if(strlen(src) <= bufSize)
        {
            strncpy(dest, src, strlen(src));
        }
        else
        {
            return -1;
        }
    }
    len = c - src;
    if(len >= bufSize)
        return -1;
    else
        strncpy(dest, src, len);
    dest[len] = '\0';

    return len + 1;
} /* end of _diag_util_getnext */

/* Convert MAC address from string to unsigned char array */
int32
diag_util_str2mac (uint8 *mac, uint8 *str)
{
    int32    len = 0;
    uint32   i = 0;
    uint8    *ptr = str;
    uint8    buf[UTIL_STRING_BUFFER_LENGTH];

    if ((NULL == mac) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    memset(buf, 0, UTIL_STRING_BUFFER_LENGTH);

    for (i = 0; i < 5; ++i)
    {
        if ((len = _diag_util_getnext(ptr, ':', buf, UTIL_STRING_BUFFER_LENGTH)) == -1 &&
            (len = _diag_util_getnext(ptr, '-', buf, UTIL_STRING_BUFFER_LENGTH)) == -1)
        {
            return RT_ERR_FAILED; /* parse error */
        }
        mac[i] = strtol(buf, NULL, 16);
        ptr += len;
    }
    mac[5] = strtol(ptr, NULL, 16);

    return RT_ERR_OK;
} /* end of diag_util_str2mac */

int8 *diag_util_inet_mactoa (const uint8 *mac)
{
	static int8 str[6*sizeof "123"];

    if (NULL == mac)
    {
        sprintf(str,"NULL");
        return str;
    }

    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return str;
} /* end of diag_util_mac2str */


int32
diag_util_mac2str (uint8 *str, const uint8 *mac)
{
    if ((NULL == mac) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return RT_ERR_OK;
} /* end of diag_util_mac2str */


int32
diag_util_eui642str (uint8 *str, const uint64 *eui64)
{
    char eui[8];
    if ((NULL == eui64) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    osal_memcpy(&eui,eui64,sizeof(eui));
    sprintf(str, "%02X%02X:%02X%02X:%02X%02X:%02X%02X", eui[0], eui[1], eui[2], eui[3], eui[4], eui[5],eui[6],eui[7]);

    return RT_ERR_OK;
} /* end of diag_util_mac2str */


int8 *diag_util_ui8tod( uint8 n, int8 *p )
{
	if( n > 99 ) *p++ = (n/100) + '0';
	if( n >  9 ) *p++ = ((n/10)%10) + '0';
	*p++ = (n%10) + '0';
	return p;
}

/*IPv4 address to string*/
int8 *diag_util_inet_ntoa(uint32 ina)
{
	static int8 buf[4*sizeof "123"];

    sprintf(buf, "%d.%d.%d.%d", ((ina>>24)&0xff), ((ina>>16)&0xff), ((ina>>8)&0xff), (ina&0xff));
	return (buf);
}

/*IPv6 address to string*/
int8 *diag_util_inet_n6toa(const uint8 *ipv6)
{
	static int8 buf[8*sizeof "FFFF:"];
    uint32  i;
    uint16  ipv6_ptr[UTIL_IPV6_TMP_BUFFER_LENGTH] = {0};

    for (i = 0; i < UTIL_IPV6_TMP_BUFFER_LENGTH ;i++)
    {
        ipv6_ptr[i] = ipv6[i*2+1];
        ipv6_ptr[i] |=  ipv6[i*2] << 8;
    }

    sprintf(buf, "%X:%X:%X:%X:%X:%X:%X:%X", ipv6_ptr[0], ipv6_ptr[1], ipv6_ptr[2], ipv6_ptr[3]
    , ipv6_ptr[4], ipv6_ptr[5], ipv6_ptr[6], ipv6_ptr[7]);
	return (buf);
}

/* convert IP address from number to string. Length of ip_str must more than 15 characters*/
int32
diag_util_ip2str(uint8 *str, uint32 ip)
{
    if (NULL == str)
    {
        return RT_ERR_FAILED;
    }

    sprintf(str, "%d.%d.%d.%d", ((ip>>24)&0xff), ((ip>>16)&0xff), ((ip>>8)&0xff), (ip&0xff));

    return RT_ERR_OK;
}

/* convert IP address from string to number */
int32
diag_util_str2ip (uint32 *ip, uint8 *str)
{
    int32   len = 0;
    uint32  i = 0;
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    uint8   *ptr = str;
    uint8   buf[UTIL_STRING_BUFFER_LENGTH];

    if ((NULL == ip) || (NULL == str))
    {
        return RT_ERR_FAILED;
    }

    memset(ip_tmp_buf, 0, sizeof(uint32) * UTIL_IP_TMP_BUFFER_LENGTH);
    memset(buf, 0, UTIL_STRING_BUFFER_LENGTH);

    for (i = 0; i < 3; ++i)
    {
        if ((len = _diag_util_getnext(ptr, '.', buf, UTIL_STRING_BUFFER_LENGTH)) == -1)
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
diag_util_ipv62str(uint8 *str, const uint8 *ipv6)
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
    sprintf(str, "%x:%x:%x:%x:%x:%x:%x:%x", ipv6_ptr[0], ipv6_ptr[1], ipv6_ptr[2], ipv6_ptr[3]
    , ipv6_ptr[4], ipv6_ptr[5], ipv6_ptr[6], ipv6_ptr[7]);

    return RT_ERR_OK;
}

/* convert IPv6 address from string to number. Length of ipv6_addr must more than 16 characters */
int32
diag_util_str2ipv6(uint8 *ipv6, const uint8 *str)
{
#define IN6ADDRSZ 16
#define INT16SZ     2
    static const uint8 xdigits_l[] = "0123456789abcdef",
              xdigits_u[] = "0123456789ABCDEF";
    uint8 tmp[IN6ADDRSZ], *tp, *endp, *colonp;
    const uint8 *xdigits, *curtok;
    int ch, saw_xdigit;
    int val;

    if ((NULL == str) || (NULL == ipv6))
    {
        return RT_ERR_FAILED;
    }

    memset((tp = tmp), '\0', IN6ADDRSZ);
    endp = tp + IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*str == ':')
        if (*++str != ':')
            return (RT_ERR_FAILED);
    curtok = str;
    saw_xdigit = 0;
    val = 0;
    while ((ch = *str++) != '\0') {
        const uint8 *pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (RT_ERR_FAILED);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = str;
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
#if 0
        if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
            inet_pton4(curtok, tp) > 0) {
            tp += INADDRSZ;
            saw_xdigit = 0;
            break;  /* '\0' was seen by inet_pton4(). */
        }
#endif
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
#if defined(__linux__) /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
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
#endif /* Add the line for eCos, 2010-05-07 Fixed Me!!! */
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




void
diag_util_printf(char *fmt, ...)
{
    va_list     args;
    char tmp_str[UTIL_STRING_BUFFER_LENGTH];

    va_start(args, fmt);
    vsprintf(tmp_str,fmt, args);
    va_end(args);

    main_parser.cfg.prints(&main_parser, tmp_str);

}

void
diag_util_mprintf(char *fmt, ...)
{
    va_list     args;
    char tmp_str[UTIL_STRING_BUFFER_LENGTH];
    if (stopped)
    {
        return;
    }
    va_start(args, fmt);
    vsprintf(tmp_str,fmt, args);
    va_end(args);
    main_parser.cfg.prints(&main_parser, (const char*)tmp_str);
    ++lines;

#if defined(RTL_RLX_IO) || defined(CYGWIN_MDIO_IO) || defined(FPGA_DEFINED)

#else
    if (lines > MAX_MORE_LINES)
    {
        int    ch;
        cparser_char_t type;
        lines = 1;
        main_parser.cfg.prints(&main_parser, "\n--More--");

        diag_util_set_keypress();

        main_parser.cfg.getch(&main_parser,&ch,&type);
        ch = ch & 0xFF;

        main_parser.cfg.printc(&main_parser,8);


        diag_util_reset_keypress();


        main_parser.cfg.prints(&main_parser, "\n");

        if (('Q' == ch) || ('q' == ch))
        {
            stopped = 1;
            return;
        }
    }
#endif

    return;
} /* end of diag_util_mprintf */


void
diag_util_mprintf_init (void)
{
    stopped = 0;
    lines = 0;
    return;
} /* end of diag_util_mprintf_init */


uint32 diag_util_chip_id_get(void)
{
    uint32 chipId;
    uint32 ChipRevId;
    uint32 ret;

	if (( ret = drv_swcore_cid_get(&chipId,&ChipRevId)) != RT_ERR_OK )
    {
        return 0xFFFFFFFF;
    }

    return chipId;
}


int32 diag_util_str2hex32(uint8 *str, uint32 *intOut)
{
	uint8 num;

	if(strlen(str) != 8)
		return FALSE;

	*intOut = 0;

	while((*str)!= '\0')
	{
		if(isdigit(*str))
			num = (*str) - '0';
		else if((*str) >= 'a' && (*str) <= 'f')
			num = (*str) - 'a' + 10;
		else if((*str) >= 'A' && (*str) <= 'F')
			num = (*str) - 'A' + 10;
		else
			return FALSE;

		*intOut = (*intOut * 16) + num;

		str++;
	}

	return TRUE;
}


