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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/classify.h>
#include <hal/common/halctrl.h>
#include <osal/memory.h>


/* variable */
static rtk_classify_cfg_t diag_classfRule;

/* local function */
static uint32 _classf_show(rtk_classify_cfg_t *pCfg)
{




#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
   if((APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE) ||
       (APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE))
    {
        diag_util_printf("%15s: (%d,0x%x)\n", "TagVID",
        (pCfg->field.readField.dataFieldRaw[1] & 0x7f) << 5 | (pCfg->field.readField.dataFieldRaw[0] >> 11 & 0x1f),
        (pCfg->field.readField.careFieldRaw[1] & 0x7f) << 5 | (pCfg->field.readField.careFieldRaw[0] >> 11 & 0x1f));

        diag_util_printf("%15s: (%d,0x%x)\n", "TagPri",
        pCfg->field.readField.dataFieldRaw[0] >> 8 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 8 & 0x7);

        diag_util_printf("%15s: (%d,0x%x)\n", "IntrPri",
        pCfg->field.readField.dataFieldRaw[0] >> 5 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 5 & 0x7);

        diag_util_printf("%15s: (%d,0x%x)\n", "S Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 4 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 4 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "C Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 3 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 3 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "UNI",
        pCfg->field.readField.dataFieldRaw[0] & 0x7, pCfg->field.readField.careFieldRaw[0] & 0x7);

        if(pCfg->index > 63)
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "TOS/TC/GEMIDX",
           (pCfg->field.readField.dataFieldRaw[1] >> 7) & 0xff,(pCfg->field.readField.careFieldRaw[1] >> 7) & 0xff);

            diag_util_printf("%15s: (0x%x,0x%x)\n", "EtherType", pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }
        else
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "DEI",
            pCfg->field.readField.dataFieldRaw[1] >> 7 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 7 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IGMP",
            pCfg->field.readField.dataFieldRaw[1] >> 8 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 8 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "MLD",
            pCfg->field.readField.dataFieldRaw[1] >> 9 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 9 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IP4 MC",
            pCfg->field.readField.dataFieldRaw[1] >> 10 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 10 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IP6 MC",
            pCfg->field.readField.dataFieldRaw[1] >> 11 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 11 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "WAN IF",
            pCfg->field.readField.dataFieldRaw[1] >> 12 & 0x7, pCfg->field.readField.careFieldRaw[1] >> 12 & 0x7);
            diag_util_printf("%15s: (%d,0x%x)\n", "ACLHitLatchIndex",
            pCfg->field.readField.dataFieldRaw[2] & 0xff, pCfg->field.readField.careFieldRaw[2] & 0xff);
            diag_util_printf("%15s: (%d,0x%x)\n", "IP Range",
            pCfg->field.readField.dataFieldRaw[2] >> 8 & 0xf, pCfg->field.readField.careFieldRaw[2] >> 8 & 0xf);
            diag_util_printf("%15s: (%d,0x%x)\n", "PortRange",
            pCfg->field.readField.dataFieldRaw[2] >> 12 & 0xf, pCfg->field.readField.careFieldRaw[2] >> 12 & 0xf);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9601B)
    if(RTL9601B_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("%15s: (%d,0x%x)\n", "TagVID",
        (pCfg->field.readField.dataFieldRaw[1] & 0x7f) << 5 | (pCfg->field.readField.dataFieldRaw[0] >> 11 & 0x1f),
        (pCfg->field.readField.careFieldRaw[1] & 0x7f) << 5 | (pCfg->field.readField.careFieldRaw[0] >> 11 & 0x1f));

        diag_util_printf("%15s: (%d,0x%x)\n", "TagPri",
        pCfg->field.readField.dataFieldRaw[0] >> 8 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 8 & 0x7);

        diag_util_printf("%15s: (%d,0x%x)\n", "IntrPri",
        pCfg->field.readField.dataFieldRaw[0] >> 5 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 5 & 0x7);

        diag_util_printf("%15s: (%d,0x%x)\n", "S Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 4 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 4 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "C Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 3 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 3 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "UNI",
        pCfg->field.readField.dataFieldRaw[0] & 0x7, pCfg->field.readField.careFieldRaw[0] & 0x7);

        if(pCfg->index > 31)
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "TOS/TC/GEMIDX",
           (pCfg->field.readField.dataFieldRaw[1] >> 7) & 0xff,(pCfg->field.readField.careFieldRaw[1] >> 7) & 0xff);

            diag_util_printf("%15s: (0x%x,0x%x)\n", "EtherType", pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }
        else
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "DEI",
            pCfg->field.readField.dataFieldRaw[1] >> 7 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 7 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "ACLHitLatchIndex",
            pCfg->field.readField.dataFieldRaw[1] >> 8 & 0x7f, pCfg->field.readField.careFieldRaw[1] >> 8 & 0x7f);
            diag_util_printf("%15s: (%d,0x%x)\n", "Inner Tag PRI",
            pCfg->field.readField.dataFieldRaw[2] >> 13 & 0x7, pCfg->field.readField.careFieldRaw[2] >> 13 & 0x7);
            diag_util_printf("%15s: (%d,0x%x)\n", "Inner Tag CFI",
            pCfg->field.readField.dataFieldRaw[2] >> 12 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 12 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "Inner Tag VID",
            pCfg->field.readField.dataFieldRaw[2] & 0xfff, pCfg->field.readField.careFieldRaw[2] & 0xfff);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9602C)
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("%15s: (%d,0x%x)\n", "TagVID",
        (pCfg->field.readField.dataFieldRaw[1] & 0x7f) << 5 | (pCfg->field.readField.dataFieldRaw[0] >> 11 & 0x1f),
        (pCfg->field.readField.careFieldRaw[1] & 0x7f) << 5 | (pCfg->field.readField.careFieldRaw[0] >> 11 & 0x1f));

        diag_util_printf("%15s: (%d,0x%x)\n", "TagPri",
        pCfg->field.readField.dataFieldRaw[0] >> 8 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 8 & 0x7);



        if(0 == pCfg->patternIdx)
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "PriCfg",
            pCfg->field.readField.dataFieldRaw[0] >> 5 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 5 & 0x7);
        }
        else if(1 == pCfg->patternIdx && 0 == pCfg->templateIdx)
        {
        diag_util_printf("%15s: (%d,0x%x)\n", "IntrPri",
        pCfg->field.readField.dataFieldRaw[0] >> 5 & 0x7, pCfg->field.readField.careFieldRaw[0] >> 5 & 0x7);

        }



        diag_util_printf("%15s: (%d,0x%x)\n", "S Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 4 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 4 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "C Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 3 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 3 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "UNI",
        pCfg->field.readField.dataFieldRaw[0] & 0x7, pCfg->field.readField.careFieldRaw[0] & 0x7);

        if(pCfg->patternIdx > 0)
        {
            if(pCfg->templateIdx == 1)
            {
            diag_util_printf("%15s: (0x%x,0x%x)\n", "STPID Sel",
            pCfg->field.readField.dataFieldRaw[0] >> 6 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 6 & 0x1);
            diag_util_printf("%15s: (0x%x,0x%x)\n", "DEI",
            pCfg->field.readField.dataFieldRaw[0] >> 7 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 7 & 0x1);
            }
            diag_util_printf("%15s: (%d,0x%x)\n", "TOS/TC/GEMIDX",
           (pCfg->field.readField.dataFieldRaw[1] >> 7) & 0xff,(pCfg->field.readField.careFieldRaw[1] >> 7) & 0xff);
            diag_util_printf("%15s: (0x%x,0x%x)\n", "EtherType/Ctag",
            pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }
        else
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "CFACLHitIndex",
            pCfg->field.readField.dataFieldRaw[1] >> 7 & 0x7f, pCfg->field.readField.careFieldRaw[1] >> 7 & 0x7f);
            diag_util_printf("%15s: (%d,0x%x)\n", "IntVidTagIf",
            pCfg->field.readField.dataFieldRaw[1] >> 14 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 14 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "ACLHitLatchIndex",
            pCfg->field.readField.dataFieldRaw[2] & 0x7f, pCfg->field.readField.careFieldRaw[2] & 0x7f);
            diag_util_printf("%15s: (%d,0x%x)\n", "IGMP/MLD",
            pCfg->field.readField.dataFieldRaw[2] >> 7 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 7 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPMC",
            pCfg->field.readField.dataFieldRaw[2] >> 8 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 8 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPv6",
            pCfg->field.readField.dataFieldRaw[2] >> 9 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 9 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPv4",
            pCfg->field.readField.dataFieldRaw[2] >> 10 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 10 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "PPPOE",
            pCfg->field.readField.dataFieldRaw[2] >> 11 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 11 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "WAN IF",
            pCfg->field.readField.dataFieldRaw[2] >> 12 & 0xf, pCfg->field.readField.careFieldRaw[2] >> 12 & 0xf);
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9607C)
    if(RTL9607C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        if(0 == pCfg->templateIdx)
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "IntrPri",
            pCfg->field.readField.dataFieldRaw[2] >> 13 & 0x7, pCfg->field.readField.careFieldRaw[2] >> 13 & 0x7);
            diag_util_printf("%15s: (%d,0x%x)\n", "PPPOE",
            pCfg->field.readField.dataFieldRaw[2] >> 12 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 12 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPv4",
            pCfg->field.readField.dataFieldRaw[2] >> 11 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 11 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPv6",
            pCfg->field.readField.dataFieldRaw[2] >> 10 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 10 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPMC",
            pCfg->field.readField.dataFieldRaw[2] >> 9 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 9 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IGMP/MLD",
            pCfg->field.readField.dataFieldRaw[2] >> 8 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 8 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "ACLHitLatchIndex",
            pCfg->field.readField.dataFieldRaw[2] & 0xff, pCfg->field.readField.careFieldRaw[2] & 0xff);
        }
        else if(1 == pCfg->templateIdx)
        {
            diag_util_printf("%15s: (0x%x,0x%x)\n", "EtherType",
            pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }
        else
        {
            diag_util_printf("%15s: (0x%x,0x%x)\n", "Ctag",
            pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }

        diag_util_printf("%15s: (%d,0x%x)\n", "TOS/TC/GEMIDX",
        (pCfg->field.readField.dataFieldRaw[1] >> 7) & 0xff,(pCfg->field.readField.careFieldRaw[1] >> 7) & 0xff);

        diag_util_printf("%15s: (%d,0x%x)\n", "TagVID",
        (pCfg->field.readField.dataFieldRaw[1] & 0x7) << 9 | (pCfg->field.readField.dataFieldRaw[0] >> 7 & 0x1ff),
        (pCfg->field.readField.careFieldRaw[1] & 0x7) << 9 | (pCfg->field.readField.careFieldRaw[0] >> 7 & 0x1ff));

        diag_util_printf("%15s: (%d,0x%x)\n", "TagDEI",
        pCfg->field.readField.dataFieldRaw[1] >> 3 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 3 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "TagPri",
        pCfg->field.readField.dataFieldRaw[1] >> 4 & 0x7, pCfg->field.readField.careFieldRaw[1] >> 4 & 0x7);

        diag_util_printf("%15s: (%d,0x%x)\n", "STPID Sel",
        pCfg->field.readField.dataFieldRaw[0] >> 6 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 6 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "S Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 5 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 5 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "C Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 4 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 4 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "UNI",
        pCfg->field.readField.dataFieldRaw[0] & 0xf, pCfg->field.readField.careFieldRaw[0] & 0xf);
    }
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
    if(RTL9603CVD_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        if(0 == pCfg->templateIdx)
        {
            diag_util_printf("%15s: (%d,0x%x)\n", "IntrPri",
            pCfg->field.readField.dataFieldRaw[2] >> 13 & 0x7, pCfg->field.readField.careFieldRaw[2] >> 13 & 0x7);
            diag_util_printf("%15s: (%d,0x%x)\n", "PPPOE",
            pCfg->field.readField.dataFieldRaw[2] >> 12 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 12 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPv4",
            pCfg->field.readField.dataFieldRaw[2] >> 11 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 11 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPv6",
            pCfg->field.readField.dataFieldRaw[2] >> 10 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 10 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IPMC",
            pCfg->field.readField.dataFieldRaw[2] >> 9 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 9 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "IGMP/MLD",
            pCfg->field.readField.dataFieldRaw[2] >> 8 & 0x1, pCfg->field.readField.careFieldRaw[2] >> 8 & 0x1);
            diag_util_printf("%15s: (%d,0x%x)\n", "ACLHitLatchIndex",
            pCfg->field.readField.dataFieldRaw[2] & 0xff, pCfg->field.readField.careFieldRaw[2] & 0xff);
        }
        else if(1 == pCfg->templateIdx)
        {
            diag_util_printf("%15s: (0x%x,0x%x)\n", "EtherType",
            pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }
        else
        {
            diag_util_printf("%15s: (0x%x,0x%x)\n", "Ctag",
            pCfg->field.readField.dataFieldRaw[2], pCfg->field.readField.careFieldRaw[2]);
        }

        diag_util_printf("%15s: (%d,0x%x)\n", "TOS/TC/GEMIDX",
        (pCfg->field.readField.dataFieldRaw[1] >> 7) & 0xff,(pCfg->field.readField.careFieldRaw[1] >> 7) & 0xff);

        diag_util_printf("%15s: (%d,0x%x)\n", "TagVID",
        (pCfg->field.readField.dataFieldRaw[1] & 0x7) << 9 | (pCfg->field.readField.dataFieldRaw[0] >> 7 & 0x1ff),
        (pCfg->field.readField.careFieldRaw[1] & 0x7) << 9 | (pCfg->field.readField.careFieldRaw[0] >> 7 & 0x1ff));

        diag_util_printf("%15s: (%d,0x%x)\n", "TagDEI",
        pCfg->field.readField.dataFieldRaw[1] >> 3 & 0x1, pCfg->field.readField.careFieldRaw[1] >> 3 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "TagPri",
        pCfg->field.readField.dataFieldRaw[1] >> 4 & 0x7, pCfg->field.readField.careFieldRaw[1] >> 4 & 0x7);

        diag_util_printf("%15s: (%d,0x%x)\n", "STPID Sel",
        pCfg->field.readField.dataFieldRaw[0] >> 6 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 6 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "S Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 5 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 5 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "C Bit",
        pCfg->field.readField.dataFieldRaw[0] >> 4 & 0x1, pCfg->field.readField.careFieldRaw[0] >> 4 & 0x1);

        diag_util_printf("%15s: (%d,0x%x)\n", "UNI",
        pCfg->field.readField.dataFieldRaw[0] & 0x7, pCfg->field.readField.careFieldRaw[0] & 0x7);
    }
#endif

    return RT_ERR_OK;
}

static uint32 _diag_classf_show_field(rtk_classify_field_t *pField)
{
    switch(pField->fieldType)
    {
        case CLASSIFY_FIELD_ETHERTYPE:
            diag_util_printf("ether type data: 0x%04x\n", pField->classify_pattern.etherType.value);
            diag_util_printf("           mask: 0x%x\n", pField->classify_pattern.etherType.mask);
        break;
        case CLASSIFY_FIELD_TOS_DSIDX:
            diag_util_printf("tos/sid data: 0x%x\n", pField->classify_pattern.tosDsidx.value);
            diag_util_printf("        mask: 0x%x\n", pField->classify_pattern.tosDsidx.mask);
        break;
        case CLASSIFY_FIELD_TAG_VID:
            diag_util_printf("tag vid data: %d\n", pField->classify_pattern.tagVid.value);
            diag_util_printf("        mask: 0x%x\n", pField->classify_pattern.tagVid.mask);
        break;
        case CLASSIFY_FIELD_TAG_PRI:
            diag_util_printf("tag priority data: %d\n", pField->classify_pattern.tagPri.value);
            diag_util_printf("             mask: 0x%x\n", pField->classify_pattern.tagPri.mask);
        break;
        case CLASSIFY_FIELD_INTER_PRI:
            diag_util_printf("internal priority data: %d\n", pField->classify_pattern.interPri.value);
            diag_util_printf("                  mask: 0x%x\n", pField->classify_pattern.interPri.mask);
        break;
        case CLASSIFY_FIELD_IS_CTAG:
            diag_util_printf("c-bit data: %d\n", pField->classify_pattern.isCtag.value);
            diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.isCtag.mask);
        break;
        case CLASSIFY_FIELD_IS_STAG:
            diag_util_printf("s-bit data: %d\n", pField->classify_pattern.isStag.value);
            diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.isStag.mask);
        break;
        case CLASSIFY_FIELD_UNI:
            diag_util_printf("UNI data: %d\n", pField->classify_pattern.uni.value);
            diag_util_printf("    mask: 0x%x\n", pField->classify_pattern.uni.mask);
        break;
        case CLASSIFY_FIELD_ACL_HIT:
                diag_util_printf("ACL hit data: %d\n", pField->classify_pattern.aclHit.value);
                diag_util_printf("        mask: 0x%x\n", pField->classify_pattern.aclHit.mask);
        break;
        case CLASSIFY_FIELD_DEI:
                diag_util_printf("DEI  data: %d\n", pField->classify_pattern.dei.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.dei.mask);
        break;
        default:
        break;
    }

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        switch(pField->fieldType)
        {
            case CLASSIFY_FIELD_PORT_RANGE:
                diag_util_printf("L4-port range data: %d\n", pField->classify_pattern.portRange.value);
                diag_util_printf("              mask: 0x%x\n", pField->classify_pattern.portRange.mask);
            break;
            case CLASSIFY_FIELD_IP_RANGE:
                diag_util_printf("IP range data: %d\n", pField->classify_pattern.ipRange.value);
                diag_util_printf("         mask: 0x%x\n", pField->classify_pattern.ipRange.mask);
            break;
            case CLASSIFY_FIELD_WAN_IF:
                diag_util_printf("WAN interface data: %d\n", pField->classify_pattern.wanIf.value);
                diag_util_printf("              mask: 0x%x\n", pField->classify_pattern.wanIf.mask);
            break;
            case CLASSIFY_FIELD_IP6_MC:
                diag_util_printf("IPv6 multicast data: %d\n", pField->classify_pattern.ip6Mc.value);
                diag_util_printf("               mask: 0x%x\n", pField->classify_pattern.ip6Mc.mask);
            break;
            case CLASSIFY_FIELD_IP4_MC:
                diag_util_printf("IPv4 multicast data: %d\n", pField->classify_pattern.ip4Mc.value);
                diag_util_printf("               mask: 0x%x\n", pField->classify_pattern.ip4Mc.mask);
            break;
            case CLASSIFY_FIELD_MLD:
                diag_util_printf("MLD data: %d\n", pField->classify_pattern.mld.value);
                diag_util_printf("    mask: 0x%x\n", pField->classify_pattern.mld.mask);
            break;
            case CLASSIFY_FIELD_IGMP:
                diag_util_printf("IGMP data: %d\n", pField->classify_pattern.igmp.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.igmp.mask);
            break;
            default:
            break;
        }
    }
#endif

#ifdef CONFIG_SDK_RTL9601B
        if(RTL9601B_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
        {
            switch(pField->fieldType)
            {
            case CLASSIFY_FIELD_INNER_VLAN:
                diag_util_printf("INNER VLAN  data: 0x%x\n", pField->classify_pattern.innerVlan.value);
                diag_util_printf("            mask: 0x%x\n", pField->classify_pattern.innerVlan.mask);
            break;
            default:
            break;
            }
        }
#endif

#ifdef CONFIG_SDK_RTL9602C
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        switch(pField->fieldType)
        {
            case CLASSIFY_FIELD_WAN_IF:
                diag_util_printf("WAN interface data: %d\n", pField->classify_pattern.wanIf.value);
                diag_util_printf("              mask: 0x%x\n", pField->classify_pattern.wanIf.mask);
            break;
            case CLASSIFY_FIELD_IGMP_MLD:
                diag_util_printf("IGMP/MLD data: %d\n", pField->classify_pattern.igmp_mld.value);
                diag_util_printf("         mask: 0x%x\n", pField->classify_pattern.igmp_mld.mask);
            break;
            case CLASSIFY_FIELD_PPPOE:
                diag_util_printf("PPPOE data: %d\n", pField->classify_pattern.pppoe.value);
                diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.pppoe.mask);
            break;
            case CLASSIFY_FIELD_IPV4:
                diag_util_printf("IPv4 data: %d\n", pField->classify_pattern.ipv4.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.ipv4.mask);
            break;
            case CLASSIFY_FIELD_IPV6:
                diag_util_printf("IPv6 data: %d\n", pField->classify_pattern.ipv6.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.ipv6.mask);
            break;
            case CLASSIFY_FIELD_IPMC:
                diag_util_printf("IPMC/IP6MC data: %d\n", pField->classify_pattern.ipmc.value);
                diag_util_printf("           mask: 0x%x\n", pField->classify_pattern.ipmc.mask);
            break;
            case CLASSIFY_FIELD_INTERNAL_VID_TAG_IF:
                diag_util_printf("Int Vid Tag If  data: %d\n", pField->classify_pattern.intVidTagIf.value);
                diag_util_printf("                mask: 0x%x\n", pField->classify_pattern.intVidTagIf.mask);
            break;
            case CLASSIFY_FIELD_CF_ACL_HIT:
                diag_util_printf("CF ALC hit data: %d\n", pField->classify_pattern.cfAclHit.value);
                diag_util_printf("           mask: 0x%x\n", pField->classify_pattern.cfAclHit.mask);
            break;
            case CLASSIFY_FIELD_STPID_SEL:
                diag_util_printf("STPID Sel data: %d\n", pField->classify_pattern.stpidSel.value);
                diag_util_printf("          mask: 0x%x\n", pField->classify_pattern.stpidSel.mask);
            break;
            case CLASSIFY_FIELD_INNER_VLAN:
                diag_util_printf("INNER VLAN  data: 0x%x\n", pField->classify_pattern.innerVlan.value);
                diag_util_printf("            mask: 0x%x\n", pField->classify_pattern.innerVlan.mask);
            break;
       default:
            break;
        }
    }
#endif

#ifdef CONFIG_SDK_RTL9607C
    if(RTL9607C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        switch(pField->fieldType)
        {
            case CLASSIFY_FIELD_IGMP_MLD:
                diag_util_printf("IGMP/MLD data: %d\n", pField->classify_pattern.igmp_mld.value);
                diag_util_printf("         mask: 0x%x\n", pField->classify_pattern.igmp_mld.mask);
            break;
            case CLASSIFY_FIELD_PPPOE:
                diag_util_printf("PPPOE data: %d\n", pField->classify_pattern.pppoe.value);
                diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.pppoe.mask);
            break;
            case CLASSIFY_FIELD_IPV4:
                diag_util_printf("IPv4 data: %d\n", pField->classify_pattern.ipv4.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.ipv4.mask);
            break;
            case CLASSIFY_FIELD_IPV6:
                diag_util_printf("IPv6 data: %d\n", pField->classify_pattern.ipv6.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.ipv6.mask);
            break;
            case CLASSIFY_FIELD_IPMC:
                diag_util_printf("IPMC/IP6MC data: %d\n", pField->classify_pattern.ipmc.value);
                diag_util_printf("           mask: 0x%x\n", pField->classify_pattern.ipmc.mask);
            break;
            case CLASSIFY_FIELD_STPID_SEL:
                diag_util_printf("STPID Sel data: %d\n", pField->classify_pattern.stpidSel.value);
                diag_util_printf("          mask: 0x%x\n", pField->classify_pattern.stpidSel.mask);
            break;
            case CLASSIFY_FIELD_INNER_VLAN:
                diag_util_printf("INNER VLAN  data: 0x%x\n", pField->classify_pattern.innerVlan.value);
                diag_util_printf("            mask: 0x%x\n", pField->classify_pattern.innerVlan.mask);
            break;
       default:
            break;
        }
    }
#endif

#ifdef CONFIG_SDK_RTL9603CVD
    if(RTL9603CVD_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        switch(pField->fieldType)
        {
            case CLASSIFY_FIELD_IGMP_MLD:
                diag_util_printf("IGMP/MLD data: %d\n", pField->classify_pattern.igmp_mld.value);
                diag_util_printf("         mask: 0x%x\n", pField->classify_pattern.igmp_mld.mask);
            break;
            case CLASSIFY_FIELD_PPPOE:
                diag_util_printf("PPPOE data: %d\n", pField->classify_pattern.pppoe.value);
                diag_util_printf("      mask: 0x%x\n", pField->classify_pattern.pppoe.mask);
            break;
            case CLASSIFY_FIELD_IPV4:
                diag_util_printf("IPv4 data: %d\n", pField->classify_pattern.ipv4.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.ipv4.mask);
            break;
            case CLASSIFY_FIELD_IPV6:
                diag_util_printf("IPv6 data: %d\n", pField->classify_pattern.ipv6.value);
                diag_util_printf("     mask: 0x%x\n", pField->classify_pattern.ipv6.mask);
            break;
            case CLASSIFY_FIELD_IPMC:
                diag_util_printf("IPMC/IP6MC data: %d\n", pField->classify_pattern.ipmc.value);
                diag_util_printf("           mask: 0x%x\n", pField->classify_pattern.ipmc.mask);
            break;
            case CLASSIFY_FIELD_STPID_SEL:
                diag_util_printf("STPID Sel data: %d\n", pField->classify_pattern.stpidSel.value);
                diag_util_printf("          mask: 0x%x\n", pField->classify_pattern.stpidSel.mask);
            break;
            case CLASSIFY_FIELD_INNER_VLAN:
                diag_util_printf("INNER VLAN  data: 0x%x\n", pField->classify_pattern.innerVlan.value);
                diag_util_printf("            mask: 0x%x\n", pField->classify_pattern.innerVlan.mask);
            break;
       default:
            break;
        }
    }
#endif
    return RT_ERR_OK;
}

static uint32 _diag_classf_show_dsAction(rtk_classify_ds_act_t *pAct)
{
    rtk_portmask_t portlist;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    diag_util_printf("Stag action: %s\n", diagStr_dsCStagAction[pAct->csAct]);
    if(CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID == pAct->csAct ||
       CLASSIFY_DS_CSACT_ADD_TAG_8100 == pAct->csAct ||
       CLASSIFY_DS_CSACT_ADD_TAG_STAG_TPID == pAct->csAct ||
       CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2 == pAct->csAct)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                diag_util_printf("Tag VID: %d\n", pAct->cTagVid);
                diag_util_printf("Tag RPI: %d\n", pAct->cTagPri);
            break;
#endif
            default:
                diag_util_printf("Stag VID action: %s\n", diagStr_dsVidAction[pAct->csVidAct]);
                /*if(CLASSIFY_DS_VID_ACT_ASSIGN == pAct->csVidAct)*/
                    diag_util_printf("Stag VID: %d\n", pAct->sTagVid);

                diag_util_printf("Stag PRI action: %s\n", diagStr_dsPriAction[pAct->csPriAct]);
                /*if(CLASSIFY_DS_PRI_ACT_ASSIGN == pAct->csPriAct)*/
                    diag_util_printf("Stag PRI: %d\n", pAct->sTagPri);
            break;
        }

    }

    diag_util_printf("Ctag action: %s\n", diagStr_dsCtagAction[pAct->cAct]);
    if(CLASSIFY_DS_CACT_ADD_CTAG_8100 == pAct->cAct)
    {
        diag_util_printf("Ctag VID action: %s\n", diagStr_dsVidAction[pAct->cVidAct]);
        /*if(CLASSIFY_DS_VID_ACT_ASSIGN == pAct->cVidAct)*/
            diag_util_printf("Ctag VID: %d\n", pAct->cTagVid);

        diag_util_printf("Ctag PRI action: %s\n", diagStr_dsPriAction[pAct->cPriAct]);
        /*if(CLASSIFY_DS_PRI_ACT_ASSIGN == pAct->cPriAct)*/
            diag_util_printf("Ctag PRI: %d\n", pAct->cTagPri);
    }

    diag_util_printf("Classf PRI action: %s\n", diagStr_cfpriAction[pAct->interPriAct]);
    if(CLASSIFY_CF_PRI_ACT_ASSIGN == pAct->interPriAct)
        diag_util_printf("CF PRI: %d\n", pAct->cfPri);

    diag_util_printf("UNI action: %s\n", diagStr_dsUniAction[pAct->uniAct]);
    diag_util_lPortMask2str(buf,&pAct->uniMask);
    diag_util_printf("UNI ports: %s\n", buf);

#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B)
    if((APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE) ||
       (RTL9601B_CHIP_ID == DIAG_UTIL_CHIP_TYPE))
    {
        diag_util_printf("DSCP remarking action: %s\n", diagStr_enable[pAct->dscp]);
    }
#endif

#if defined(CONFIG_SDK_RTL9602C)
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("DSCP remarking action: %s\n", diagStr_dscpRemarkAction[pAct->dscp]);

        if (CLASSIFY_DSCP_ACT_ENABLE == pAct->dscp)
            diag_util_printf("DSCP value: %d\n", pAct->dscpVal);
    }
#endif
    return RT_ERR_OK;
}

uint32 _diag_classf_show_usAction(rtk_classify_us_act_t *pAct)
{
    rtk_portmask_t portlist;
    uint8   buf[UTIL_PORT_MASK_BUFFER_LENGTH];

    diag_util_printf("Stag action: %s\n", diagStr_usCStagAction[pAct->csAct]);
    if(CLASSIFY_US_CSACT_ADD_TAG_VS_TPID == pAct->csAct ||
       CLASSIFY_US_CSACT_ADD_TAG_8100 == pAct->csAct ||
       CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID == pAct->csAct ||
       CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2 == pAct->csAct)
    {
        diag_util_printf("Stag VID action: %s\n", diagStr_usVidAction[pAct->csVidAct]);
       /* if(CLASSIFY_US_VID_ACT_ASSIGN == pAct->csVidAct)*/
            diag_util_printf("Stag VID: %d\n", pAct->sTagVid);

        diag_util_printf("Stag PRI action: %s\n", diagStr_usPriAction[pAct->csPriAct]);
        /*if(CLASSIFY_US_PRI_ACT_ASSIGN == pAct->csPriAct)*/
            diag_util_printf("Stag PRI: %d\n", pAct->sTagPri);
    }

    diag_util_printf("Ctag action: %s\n", diagStr_usCtagAction[pAct->cAct]);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
    case APOLLO_CHIP_ID:
        break;
    default:
        if(CLASSIFY_US_CACT_ADD_CTAG_8100 == pAct->cAct)
        {
            diag_util_printf("Ctag VID action: %s\n", diagStr_usVidAction[pAct->cVidAct]);
           /* if(CLASSIFY_US_VID_ACT_ASSIGN == pAct->cVidAct)*/
                diag_util_printf("Ctag VID: %d\n", pAct->cTagVid);

            diag_util_printf("Ctag PRI action: %s\n", diagStr_usPriAction[pAct->cPriAct]);
            /*if(CLASSIFY_US_PRI_ACT_ASSIGN == pAct->csPriAct)*/
                diag_util_printf("Ctag PRI: %d\n", pAct->cTagPri);
        }
        break;
    }

    diag_util_printf("SID action: %s\n", diagStr_usSidAction[pAct->sidQidAct]);
    diag_util_printf("Assign ID: %d\n", pAct->sidQid);

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Classf PRI action: %s\n", diagStr_cfpriAction[pAct->interPriAct]);
        if(CLASSIFY_CF_PRI_ACT_ASSIGN == pAct->interPriAct)
            diag_util_printf("CF PRI: %d\n", pAct->cfPri);

        diag_util_printf("DSCP remarking action: %s\n", diagStr_enable[pAct->dscp]);
        diag_util_printf("Drop action: %s\n", diagStr_enable[pAct->drop]);
        diag_util_printf("logging action: %s\n", diagStr_enable[pAct->log]);
        if(CLASSIFY_US_LOG_ACT_ENABLE == pAct->log)
            diag_util_printf("logging index: %d\n", pAct->logCntIdx);
    }
#endif

#if defined(CONFIG_SDK_RTL9601B)
    if(RTL9601B_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Classf PRI action: %s\n", diagStr_cfpriAction[pAct->interPriAct]);
        if(CLASSIFY_CF_PRI_ACT_ASSIGN == pAct->interPriAct)
            diag_util_printf("CF PRI: %d\n", pAct->cfPri);

        diag_util_printf("DSCP remarking action: %s\n", diagStr_enable[pAct->dscp]);

        diag_util_printf("Drop action: %s\n", diagStr_usFwdAction[pAct->drop]);
    }
#endif

#if defined(CONFIG_SDK_RTL9602C)
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Classf PRI action: %s\n", diagStr_cfpriAction[pAct->interPriAct]);
        if(CLASSIFY_CF_PRI_ACT_ASSIGN == pAct->interPriAct)
            diag_util_printf("CF PRI: %d\n", pAct->cfPri);

        diag_util_printf("DSCP remarking action: %s\n", diagStr_dscpRemarkAction[pAct->dscp]);

        if (CLASSIFY_DSCP_ACT_ENABLE == pAct->dscp)
            diag_util_printf("DSCP value: %d\n", pAct->dscpVal);

        diag_util_printf("Drop action: %s\n", diagStr_usFwdAction[pAct->drop]);
    }
#endif

#if defined(CONFIG_SDK_RTL9607C)
    if(RTL9607C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Drop action: %s\n", diagStr_usFwdAction[pAct->drop]);
    }
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
    if(RTL9603CVD_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Drop action: %s\n", diagStr_usFwdAction[pAct->drop]);
    }
#endif

    return RT_ERR_OK;
}

/*
 * classf show
 */
cparser_result_t
cparser_cmd_classf_show(
    cparser_context_t *context)
{
    rtk_classify_field_t *cf_field;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Not: %s\n", diagStr_enable[diag_classfRule.invert]);
    diag_util_printf("direction: %s\n",diagStr_direction[diag_classfRule.direction]);
    diag_util_printf("templateIdx: %d\n",diag_classfRule.templateIdx);

    cf_field = diag_classfRule.field.pFieldHead;
    while(cf_field != NULL)
    {
        diag_util_printf("Rule: \n");
        _diag_classf_show_field(cf_field);
        cf_field = cf_field->next;
    }

    if(CLASSIFY_DIRECTION_DS == diag_classfRule.direction)
    {
        diag_util_printf("Downstream action: \n");
        _diag_classf_show_dsAction(&diag_classfRule.act.dsAct);
    }
    else
    {
        diag_util_printf("Upstream action: \n");
        _diag_classf_show_usAction(&diag_classfRule.act.usAct);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_show_rule */

/*
 * classf clear
 */
cparser_result_t
cparser_cmd_classf_clear(
    cparser_context_t *context)
{
    rtk_classify_field_t *fieldNext, *fieldThis;

    DIAG_UTIL_PARAM_CHK();

    fieldThis = diag_classfRule.field.pFieldHead;
    while(fieldThis != NULL)
    {
        fieldNext = fieldThis->next;
        osal_free(fieldThis);
        fieldThis = fieldNext;
    }

    diag_classfRule.field.pFieldHead = NULL;

    osal_memset(&diag_classfRule, 0x0, sizeof(rtk_classify_cfg_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_clear */

/*
 * classf init
 */
cparser_result_t
cparser_cmd_classf_init(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();


    DIAG_UTIL_ERR_CHK(rtk_classify_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_clear */

/*
 * classf add entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_add_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_field_t *fieldNext, *fieldThis;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.index = *index_ptr;
    diag_classfRule.valid = 1;

    if((ret = rtk_classify_cfgEntry_add(&diag_classfRule)) != RT_ERR_OK)
        DIAG_ERR_PRINT(ret);\

    fieldThis = diag_classfRule.field.pFieldHead;
    while(fieldThis != NULL)
    {
        fieldNext = fieldThis->next;
        osal_free(fieldThis);
        fieldThis = fieldNext;
    }

    diag_classfRule.field.pFieldHead = NULL;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_add_entry_index */

/*
 * classf del entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_del_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_classify_cfgEntry_del(*index_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_del_entry_index */

/*
 * classf del entry all
 */
cparser_result_t
cparser_cmd_classf_del_entry_all(
    cparser_context_t *context)
{
    uint32 start_entry, total_entry;
    uint32 idx;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    start_entry = 0;

    total_entry = HAL_CLASSIFY_ENTRY_MAX();

    for(idx=start_entry; idx < (start_entry + total_entry); idx++)
    {
        DIAG_UTIL_ERR_CHK(rtk_classify_cfgEntry_del(idx), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_del_entry_all */

/*
 * classf get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_cfg_t entry;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    entry.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_cfgEntry_get(&entry), ret);

    diag_util_printf("Valid: %s\n", diagStr_valid[entry.valid]);
    diag_util_printf("Not: %s\n", diagStr_enable[entry.invert]);
    diag_util_printf("direction: %s\n",diagStr_direction[entry.direction]);
    diag_util_printf("templateIdx: %d\n", entry.templateIdx);

    diag_util_printf("databit: 0x%04x-%04x-%04x\n", entry.field.readField.dataFieldRaw[2],
                                                    entry.field.readField.dataFieldRaw[1],
                                                    entry.field.readField.dataFieldRaw[0]);

    diag_util_printf("carebit: 0x%04x-%04x-%04x\n", entry.field.readField.careFieldRaw[2],
                                                    entry.field.readField.careFieldRaw[1],
                                                    entry.field.readField.careFieldRaw[0]);

    if (ENABLED == entry.valid)
    {
        _classf_show(&entry);
    }

    if(CLASSIFY_DIRECTION_DS == entry.direction)
    {
        diag_util_printf("Downstream action: \n");
        _diag_classf_show_dsAction(&entry.act.dsAct);
    }
    else
    {
        diag_util_printf("Upstream action: \n");
        _diag_classf_show_usAction(&entry.act.usAct);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_entry_index */

/*
 * classf get entry valid
 */
cparser_result_t
cparser_cmd_classf_get_entry_valid(
    cparser_context_t *context)
{
    rtk_classify_cfg_t entry;
    uint32 index = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&entry, 0x0, sizeof(rtk_classify_cfg_t));
    entry.index = index;
    while (RT_ERR_OK == (rtk_classify_cfgEntry_get(&entry)))
    {
        if (ENABLED == entry.valid)
        {
            diag_util_printf("****************************************************************\n");
            diag_util_printf("Index: %u\n", entry.index);
            diag_util_printf("Valid: %s\n", diagStr_valid[entry.valid]);
            diag_util_printf("Not: %s\n", diagStr_enable[entry.invert]);
            diag_util_printf("direction: %s\n",diagStr_direction[entry.direction]);
            diag_util_printf("templateIdx: %d\n", entry.templateIdx);

            diag_util_printf("databit: 0x%04x-%04x-%04x\n", entry.field.readField.dataFieldRaw[2],
                                                            entry.field.readField.dataFieldRaw[1],
                                                            entry.field.readField.dataFieldRaw[0]);

            diag_util_printf("carebit: 0x%04x-%04x-%04x\n", entry.field.readField.careFieldRaw[2],
                                                            entry.field.readField.careFieldRaw[1],
                                                            entry.field.readField.careFieldRaw[0]);

            _classf_show(&entry);

            if(CLASSIFY_DIRECTION_DS == entry.direction)
            {
                diag_util_printf("Downstream action: \n");
                _diag_classf_show_dsAction(&entry.act.dsAct);
            }
            else
            {
                diag_util_printf("Upstream action: \n");
                _diag_classf_show_usAction(&entry.act.usAct);
            }

        }
        osal_memset(&entry, 0x0, sizeof(rtk_classify_cfg_t));
        index++;
        entry.index = index;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_entry_valid */


/*
 * classf set rule direction ( upstream | downstream )
 */
cparser_result_t
cparser_cmd_classf_set_rule_direction_upstream_downstream(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if ('u' == TOKEN_CHAR(4,0))
        diag_classfRule.direction = CLASSIFY_DIRECTION_US;
    else
        diag_classfRule.direction = CLASSIFY_DIRECTION_DS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_direction_upstream_downstream */

/*
 * classf set rule tos-sid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_tos_sid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(CLASSIFY_DIRECTION_DS == diag_classfRule.direction)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0x7F), CPARSER_ERR_INVALID_PARAMS);
    }
    else
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0xFF), CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0xFF), CPARSER_ERR_INVALID_PARAMS);
    }

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_TOS_DSIDX;
    fieldHead->classify_pattern.tosDsidx.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.tosDsidx.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_tos_sid_data_data_mask_mask */

/*
 * classf set rule tag-vid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_tag_vid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_TAG_VID;
    fieldHead->classify_pattern.tagVid.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.tagVid.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_tag_vid_data_data_mask_mask */

/*
 * classf set rule tag-priority data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_tag_priority_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_TAG_PRI;
    fieldHead->classify_pattern.tagPri.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.tagPri.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_tag_priority_data_data_mask_mask */

/*
 * classf set rule internal-priority data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_internal_priority_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_INTER_PRI;
    fieldHead->classify_pattern.interPri.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.interPri.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_internal_priority_data_data_mask_mask */

/*
 * classf set rule svlan-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_svlan_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IS_STAG;
    fieldHead->classify_pattern.isStag.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.isStag.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_svlan_bit_data_data_mask_mask */

/*
 * classf set rule cvlan-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_cvlan_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IS_CTAG;
    fieldHead->classify_pattern.isCtag.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.isCtag.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_cvlan_bit_data_data_mask_mask */

/*
 * classf set rule uni data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_uni_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
#ifdef CONFIG_SDK_RTL9607C
    if(RTL9607C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0xf), CPARSER_ERR_INVALID_PARAMS);
        DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0xf), CPARSER_ERR_INVALID_PARAMS);
    }
#else
    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0x7), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0x7), CPARSER_ERR_INVALID_PARAMS);
#endif
    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_UNI;
    fieldHead->classify_pattern.uni.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.uni.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_uni_data_data_mask_mask */

/*
 * classf set rule ether-type data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ether_type_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_ETHERTYPE;
    fieldHead->classify_pattern.etherType.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.etherType.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ether_type_data_data_mask_mask */

/*
 * classf set rule hit-acl data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_hit_acl_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x80|(HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x80|(HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_ACL_HIT;
    fieldHead->classify_pattern.aclHit.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.aclHit.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_hit_acl_data_data_mask_mask */

/*
 * classf set rule inner-vlan data <UINT:data> mask <UINT:mask> */
cparser_result_t
cparser_cmd_classf_set_rule_inner_vlan_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 0xffff), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_INNER_VLAN;
    fieldHead->classify_pattern.innerVlan.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.innerVlan.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_inner_vlan_data_data_mask_mask */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * classf set rule range-l4port data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_range_l4port_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x8|(HAL_CLASSIFY_L4PORT_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x8|(HAL_CLASSIFY_L4PORT_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_PORT_RANGE;
    fieldHead->classify_pattern.portRange.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.portRange.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_range_l4port_data_data_mask_mask */

/*
 * classf set rule range-ip data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_range_ip_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x8|(HAL_CLASSIFY_IP_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x8|(HAL_CLASSIFY_IP_RANGE_NUM()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IP_RANGE;
    fieldHead->classify_pattern.ipRange.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ipRange.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_range_ip_data_data_mask_mask */

/*
 * classf set rule wan-if data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_wan_if_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(*data_ptr >= (HAL_L34_NETIF_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK(*mask_ptr >= (HAL_L34_NETIF_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_WAN_IF;
    fieldHead->classify_pattern.wanIf.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.wanIf.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_wan_if_data_data_mask_mask */

/*
 * classf set rule ipmc-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ipmc_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IP4_MC;
    fieldHead->classify_pattern.ip4Mc.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ip4Mc.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ipmc_bit_data_data_mask_mask */

/*
 * classf set rule ip6mc-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ip6mc_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IP6_MC;
    fieldHead->classify_pattern.ip6Mc.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ip6Mc.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ip6mc_bit_data_data_mask_mask */

/*
 * classf set rule ipmc-ip6mc-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ipmc_ip6mc_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IPMC;
    fieldHead->classify_pattern.ipmc.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ipmc.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ipmc_ip6mc_bit_data_data_mask_mask */


/*
 * classf set rule igmp-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_igmp_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IGMP;
    fieldHead->classify_pattern.igmp.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.igmp.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_igmp_bit_data_data_mask_mask */

/*
 * classf set rule mld-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_mld_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));

    fieldHead->fieldType = CLASSIFY_FIELD_MLD;
    fieldHead->classify_pattern.mld.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.mld.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_mld_bit_data_data_mask_mask */

/*
 * classf set rule igmp-mld-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_igmp_mld_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));

    fieldHead->fieldType = CLASSIFY_FIELD_IGMP_MLD;
    fieldHead->classify_pattern.igmp_mld.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.igmp_mld.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_igmp_mld_bit_data_data_mask_mask */


/*
 * classf set rule dei-cfi data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_dei_cfi_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_DEI;
    fieldHead->classify_pattern.dei.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.dei.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_dei_bit_data_data_mask_mask */




/*
 * classf set rule pppoe-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_pppoe_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_PPPOE;
    fieldHead->classify_pattern.pppoe.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.pppoe.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_pppoe_bit_data_data_mask_mask */

/*
 * classf set rule ipv4-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ipv4_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IPV4;
    fieldHead->classify_pattern.ipv4.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ipv4.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ipv4_bit_data_data_mask_mask */

/*
 * classf set rule ipv6-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_ipv6_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_IPV6;
    fieldHead->classify_pattern.ipv6.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.ipv6.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_ipv6_bit_data_data_mask_mask */

/*
 * classf set rule intVidTagIf-bit data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_intVidTagIf_bit_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_INTERNAL_VID_TAG_IF;
    fieldHead->classify_pattern.intVidTagIf.value= (uint16)*data_ptr;
    fieldHead->classify_pattern.intVidTagIf.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_intVlanTagIf_bit_data_data_mask_mask */

/*
 * classf set rule hit-cf-acl data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_hit_cf_acl_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > (0x80|(HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1))), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > (0x80|(HAL_MAX_NUM_OF_ACL_RULE_ENTRY()-1))), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_CF_ACL_HIT;
    fieldHead->classify_pattern.cfAclHit.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.cfAclHit.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_hit_cf_acl_data_data_mask_mask */


/*
 * classf set rule stpid-sel data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_classf_set_rule_stpid_sel_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_classify_field_t *fieldHead;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*data_ptr > 1), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*mask_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    fieldHead = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    fieldHead->fieldType = CLASSIFY_FIELD_STPID_SEL;
    fieldHead->classify_pattern.stpidSel.value = (uint16)*data_ptr;
    fieldHead->classify_pattern.stpidSel.mask = (uint16)*mask_ptr;
    fieldHead->next = NULL;

    DIAG_UTIL_ERR_CHK(rtk_classify_field_add(&diag_classfRule, fieldHead), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_stpid_sel_data_data_mask_mask */


/*
 * classf set rule template entry <UINT:entry>
 */
cparser_result_t
cparser_cmd_classf_set_rule_template_entry_entry(
    cparser_context_t *context,
    uint32_t  *entry_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*entry_ptr > 2), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.templateIdx = *entry_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_rule_template_entry_entry */


#endif
/*
 * classf set operation entry <UINT:index> ( upstream | downstream ) ( hit | not )
 */
cparser_result_t
cparser_cmd_classf_set_operation_entry_index_upstream_downstream_hit_not(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);

    if ('h' == TOKEN_CHAR(6,0))
        diag_classfRule.invert = CLASSIFY_INVERT_DISABLE;
    else if ('n' == TOKEN_CHAR(6,0))
        diag_classfRule.invert = CLASSIFY_INVERT_ENABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_operation_entry_index_upstream_downstream_hit_not */

/*
 * classf set upstream-action svlan-act ( nop | vs-tpid | c-tpid | del | transparent | stag-tpid | act-disable | vs-tpid2 )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent_stag_tpid_act_disable_vs_tpid2(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_NOP;
    else if(0 == osal_strcmp("vs-tpid", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID;
    else if('c' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_8100;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_DEL_STAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_TRANSPARENT;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_STAG_TPID;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ACTCTRL_DISABLE;
    else if(0 == osal_strcmp("vs-tpid2", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csAct = CLASSIFY_US_CSACT_ADD_TAG_VS_TPID2;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent */

/*
 * classf set upstream-action cvlan-act ( nop | c-tag | c2s | del | transparent | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_act_nop_c_tag_c2s_del_transparent_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_NOP;
    else if('c' == TOKEN_CHAR(4,0) && '-' == TOKEN_CHAR(4,1))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
    else if('c' == TOKEN_CHAR(4,0) && '2' == TOKEN_CHAR(4,1))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_TRANSLATION_C2S;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_DEL_CTAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_TRANSPARENT;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cAct = CLASSIFY_US_CACT_ACTCTRL_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_act_nop_c_tag_c2s_del_transparent */

/*
 * classf set upstream-action svlan-id-act assign <UINT:vid>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_id_act_assign_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
    diag_classfRule.act.usAct.sTagVid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_id_act_assign_vid */


/*
 * classf set upstream-action svlan-id-act ( copy-outer | copy-inner ) { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_id_act_copy_outer_copy_inner_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{

    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if(vid_ptr!=NULL){
    diag_classfRule.act.usAct.sTagVid = *vid_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_id_act_copy_outer_copy_inner */

/*
 * classf set upstream-action svlan-id-act nop
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_id_act_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.act.usAct.csVidAct = CLASSIFY_US_VID_ACT_NOP;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_id_act_nop */

/*
 * classf set upstream-action svlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
    diag_classfRule.act.usAct.sTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_priority_act_assign_priority */

/*
 * classf set upstream-action svlan-priority-act ( copy-outer | copy-inner | internal-priority | dscp-priority ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority_dscp_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)

{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_FROM_DSCP;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if(priority_ptr!=NULL){
        diag_classfRule.act.usAct.sTagPri = *priority_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority */

/*
 * classf set upstream-action svlan-priority-act nop
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_svlan_priority_act_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.act.usAct.csPriAct = CLASSIFY_US_PRI_ACT_NOP;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_svlan_priority_act_nop */


/*
 * classf set upstream-action cvlan-id-act assign <UINT:vid>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_id_act_assign_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_ASSIGN;
    diag_classfRule.act.usAct.cTagVid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_id_act_assign_vid */

/*
 * classf set upstream-action cvlan-id-act ( copy-outer | copy-inner | internal-vid )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_id_act_copy_outer_copy_inner_internal_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if(vid_ptr!=NULL){
    diag_classfRule.act.usAct.cTagVid = *vid_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_id_act_copy_outer_copy_inner_internal_vid */

/*
 * classf set upstream-action cvlan-id-act nop
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_id_act_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_NOP;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_id_act_nop */


/*
 * classf set upstream-action cvlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
    diag_classfRule.act.usAct.cTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_priority_act_assign_priority */

/*
 * classf set upstream-action cvlan-priority-act ( copy-outer | copy-inner | internal-priority | dscp-priority ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_priority_act_copy_outer_copy_inner_internal_priority_dscp_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_1ST_TAG;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_FROM_DSCP;

    else
        return CPARSER_ERR_INVALID_PARAMS;

    if(priority_ptr!=NULL){
        diag_classfRule.act.usAct.cTagPri = *priority_ptr;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_priority_act_copy_outer_copy_inner_internal_priority */

/*
 * classf set upstream-action cvlan-priority-act nop
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_cvlan_priority_act_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_classfRule.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_NOP;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_cvlan_priority_act_nop */


/*
 * classf set upstream-action sid-act ( sid | qid ) <UINT:id>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_sid_act_sid_qid_id(
    cparser_context_t *context,
    uint32_t  *id_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*id_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);

    if('q' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_QID;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_classfRule.act.usAct.sidQid = *id_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_sid_act_sid_qid_id */

/*
 * classf set upstream-action sid-act ( nop | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_sid_act_nop_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
    {
        diag_classfRule.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_NOP;
        diag_classfRule.act.usAct.sidQid = 0;
    }
    else if ('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ACTCTRL_DISABLE;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_sid_act_nop_act_disable */

/*
 * classf set upstream-action priority-act ( follow-swcore | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_priority_act_follow_swcore_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_NOP;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_priority_act_follow_swcore_act_disable */

/*
 * classf set upstream-action priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
    diag_classfRule.act.usAct.cfPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_priority_act_assign_priority */

/*
 * classf set upstream-action remark-dscp ( enable | disable | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_remark_dscp_enable_disable_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.dscp = CLASSIFY_DSCP_ACT_DISABLE;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.dscp = CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_remark_dscp_enable_disable_act_disable */

/*
 * classf set upstream-action remark-dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_remark_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.dscpVal = (uint8)*dscp_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_remark_dscp_dscp */

/*
 * classf set upstream-action drop ( enable | disable | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_drop_enable_disable_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_NONE;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_ACTCTRL_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_drop_enable_disable_act_disable */

/*
 * classf set upstream-action ( trap | drop-pon )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_trap_drop_pon(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('t' == TOKEN_CHAR(3,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_TRAP;
    if('d' == TOKEN_CHAR(3,0))
        diag_classfRule.act.usAct.drop = CLASSIFY_DROP_ACT_DROP_PON;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_trap */

/*
 * classf set upstream-action statistic <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_set_upstream_action_statistic_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.usAct.log = CLASSIFY_US_LOG_ACT_ENABLE;
    diag_classfRule.act.usAct.logCntIdx = (uint8)*index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_action_statistic_index */

/*
 * classf set downstream-action svlan-act ( nop | vs-tpid | c-tpid | del | transparent | sp2c | stag-tpid | act-disable | vs-tpid2 )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent_sp2c_stag_tpid_act_disable_vs_tpid2(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_NOP;
    else if(0 == osal_strcmp("vs-tpid", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID;
    else if('c' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_8100;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_DEL_STAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_TRANSPARENT;
    else if(0 == osal_strcmp("sp2c", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_SP2C;
    else if(0 == osal_strcmp("stag-tpid", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_STAG_TPID;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ACTCTRL_DISABLE;
    else if(0 == osal_strcmp("vs-tpid2", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csAct = CLASSIFY_DS_CSACT_ADD_TAG_VS_TPID2;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_act_nop_vs_tpid_c_tpid_del_transparent_sp2c_act_disable */

/*
 * classf set downstream-action svlan-id-act assign <UINT:vid>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_id_act_assign_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
    diag_classfRule.act.dsAct.sTagVid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_id_act_assign_vid */

/*
 * classf set downstream-action svlan-id-act ( copy-outer | copy-inner ) { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_id_act_copy_outer_copy_inner_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if(vid_ptr!=NULL){
    diag_classfRule.act.dsAct.sTagVid = *vid_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_id_act_copy_outer_copy_inner_vid */

/*
 * classf set downstream-action svlan-id-act ( sp2c | nop )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_id_act_sp2c_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_TRANSLATION_SP2C;
    else if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csVidAct = CLASSIFY_DS_VID_ACT_NOP;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_id_act_sp2c_nop */


/*
 * classf set downstream-action svlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
    diag_classfRule.act.dsAct.sTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_priority_act_assign_priority */

/*
 * classf set downstream-action svlan-priority-act ( copy-outer | copy-inner | internal-priority | dscp-priority ) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority_dscp_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();


    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if(priority_ptr!=NULL){
        diag_classfRule.act.dsAct.sTagPri = *priority_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_priority_act_copy_outer_copy_inner_internal_priority_priority */

/*
 * classf set downstream-action svlan-priority-act ( sp2c | nop )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_svlan_priority_act_sp2c_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_TRANSLATION_SP2C;
    else if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.csPriAct = CLASSIFY_DS_PRI_ACT_NOP;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_svlan_priority_act_sp2c_nop */

/*
 * classf set downstream-action cvlan-act ( nop | c-tag | sp2c | del | transparent | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_act_nop_c_tag_sp2c_del_transparent_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_NOP;
    else if('c' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_TRANSLATION_SP2C;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_DEL_CTAG;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_TRANSPARENT;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cAct = CLASSIFY_DS_CACT_ACTCTRL_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_act_nop_c_tag_sp2c_del_transparent */

/*
 * classf set downstream-action cvlan-id-act ( copy-outer | copy-inner | lookup-table )  { <UINT:vid> }
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_id_act_copy_outer_copy_inner_lookup_table_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('l' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_LUT;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_1ST_TAG;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_2ND_TAG;
    else
        return CPARSER_ERR_INVALID_PARAMS;

   if(vid_ptr!=NULL){
    diag_classfRule.act.dsAct.cTagVid = *vid_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_id_act_follow_swcore_copy_outer_copy_inner_lookup_table_sp2c_vid */

/*
 * classf set downstream-action cvlan-id-act ( nop | sp2c )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_id_act_nop_sp2c(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_NOP;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_TRANSLATION_SP2C;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_id_act_follow_swcore_copy_outer_copy_inner_lookup_table_sp2c_vid */


/*
 * classf set downstream-action cvlan-id-act assign <UINT:cvid>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_id_act_assign_cvid(
    cparser_context_t *context,
    uint32_t  *cvid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*cvid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_ASSIGN;
    diag_classfRule.act.dsAct.cTagVid = *cvid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_id_act_assign_cvid */


/*
 * classf set downstream-action cvlan-priority-act ( copy-outer | copy-inner | internal-priority | dscp-priority) { <UINT:priority> }
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_priority_act_copy_outer_copy_inner_internal_priority_dscp_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
    else if(0 == osal_strcmp("copy-inner", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_2ND_TAG;
    else if(0 == osal_strcmp("copy-outer", TOKEN_STR(4)))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_1ST_TAG;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_FROM_DSCP;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if(priority_ptr!=NULL){
        diag_classfRule.act.dsAct.cTagPri = *priority_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_priority_act_follow_swcore_copy_outer_copy_inner_internal_priority_sp2c_priority */

/*
 * classf set downstream-action cvlan-priority-act ( nop | sp2c )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_priority_act_nop_sp2c(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_NOP;
    else if('s' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_TRANSLATION_SP2C;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_priority_act_follow_swcore_copy_outer_copy_inner_internal_priority_sp2c_priority */

/*
 * classf set downstream-action cvlan-priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_cvlan_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
    diag_classfRule.act.dsAct.cTagPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_cvlan_priority_act_assign_priority */


/*
 * classf set downstream-action priority-act ( follow-swcore | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_priority_act_follow_swcore_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_NOP;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_ACTCTRL_DISABLE;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_priority_act_follow_swcore */

/*
 * classf set downstream-action priority-act assign <UINT:priority>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_priority_act_assign_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
    diag_classfRule.act.dsAct.cfPri = (uint8)*priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_priority_act_assign_priority */

/*
 * classf set downstream-action uni-forward-act ( flood | forced ) port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_uni_forward_act_flood_forced_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    if(0 == osal_strcmp(TOKEN_STR(4),"flood"))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_MASK_BY_UNIMASK;
    else if(0 == osal_strcmp(TOKEN_STR(4),"forced"))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

#if 0
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_classfRule.act.dsAct.uniMask.bits[0] = portlist.portmask.bits[0];
    }
    else if(APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        /* bit 0-2:MAC0-2, bit 3-4:MAC4-5  bit5:MAC6(aka CPU) */
        diag_classfRule.act.dsAct.uniMask.bits[0] = ( (portlist.portmask.bits[0] & 0x7) |
                                             ((portlist.portmask.bits[0] & 0x70) >> 1) );
    }
#else
    diag_classfRule.act.dsAct.uniMask = portlist.portmask;
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_uni_forward_act_flood_forced_port_ports_all_none */

/*
 * classf set downstream-action uni-forward-act ( nop | trap | act-disable ) */
cparser_result_t
cparser_cmd_classf_set_downstream_action_uni_forward_act_nop_trap_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('n' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_NOP;
    else if('t' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_TRAP;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_ACTCTRL_DISABLE;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_uni_forward_act_nop */

/*
 * classf set downstream-action remark-dscp ( enable | disable | act-disable )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_remark_dscp_enable_disable_act_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.dscp = CLASSIFY_DSCP_ACT_DISABLE;
    else if('a' == TOKEN_CHAR(4,0))
        diag_classfRule.act.dsAct.dscp = CLASSIFY_DSCP_ACT_ACTCTRL_DISABLE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_remark_dscp */


/*
 * classf set downstream-action remark-dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_classf_set_downstream_action_remark_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), CPARSER_ERR_INVALID_PARAMS);

    diag_classfRule.act.dsAct.dscpVal = (uint8)*dscp_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_downstream_action_remark_dscp_dscp */

/*
 * classf set upstream-unmatch-act ( drop | permit | permit-without-pon )
 */
cparser_result_t
cparser_cmd_classf_set_upstream_unmatch_act_drop_permit_permit_without_pon(
    cparser_context_t *context)
{
    rtk_classify_unmatch_action_t action=CLASSIFY_UNMATCH_DROP;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp(TOKEN_STR(3),"drop"))
        action = CLASSIFY_UNMATCH_DROP;
    else if(0 == osal_strcmp(TOKEN_STR(3),"permit"))
        action = CLASSIFY_UNMATCH_PERMIT;
    else if(0 == osal_strcmp(TOKEN_STR(3),"permit-without-pon"))
        action = CLASSIFY_UNMATCH_PERMIT_WITHOUT_PON;

    DIAG_UTIL_ERR_CHK(rtk_classify_unmatchAction_set(action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_unmatch_act_drop_permit_permit_without_pon */

/*
 * classf get upstream-unmatch-act
 */
cparser_result_t
cparser_cmd_classf_get_upstream_unmatch_act(
    cparser_context_t *context)
{
    rtk_classify_unmatch_action_t action;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_unmatchAction_get(&action), ret);

    diag_util_printf("Upstream un-match action: %s\n\r", diagStr_cfUnmatchAct[action]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_upstream_unmatch_act */

/*
 * classf set downstream-unmatch-act ( drop | permit )
 */
cparser_result_t
cparser_cmd_classf_set_downstream_unmatch_act_drop_permit(
    cparser_context_t *context)
{
    rtk_classify_unmatch_action_ds_t action=CLASSIFY_UNMATCH_DS_DROP;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(0 == osal_strcmp(TOKEN_STR(3),"drop"))
        action = CLASSIFY_UNMATCH_DS_DROP;
    else if(0 == osal_strcmp(TOKEN_STR(3),"permit"))
        action = CLASSIFY_UNMATCH_DS_PERMIT;

    DIAG_UTIL_ERR_CHK(rtk_classify_unmatchAction_ds_set(action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_upstream_unmatch_act_drop_permit_permit_without_pon */

/*
 * classf get downstream-unmatch-act
 */
cparser_result_t
cparser_cmd_classf_get_downstream_unmatch_act(
    cparser_context_t *context)
{
    rtk_classify_unmatch_action_ds_t action=CLASSIFY_UNMATCH_DS_DROP;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_unmatchAction_ds_get(&action), ret);

    diag_util_printf("Downstream un-match action: %s\n\r", diagStr_cfUnmatchActDs[action]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_upstream_unmatch_act */

/*
 * classf set remarking dscp priority <UINT:priority> dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_classf_set_remarking_dscp_priority_priority_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *priority_ptr,
    uint32_t  *dscp_ptr)
{
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_classify_cfPri2Dscp_set(*priority_ptr, *dscp_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_remarking_dscp_priority_priority_dscp_dscp */

/*
 * classf get remarking dscp
 */
cparser_result_t
cparser_cmd_classf_get_remarking_dscp(
    cparser_context_t *context)
{
    rtk_pri_t   pri;
    rtk_dscp_t  dscp;
    int32       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("CF_priority  DSCP");
    for(pri=0; pri<=RTK_DOT1P_PRIORITY_MAX; pri++)
    {
        DIAG_UTIL_ERR_CHK(rtk_classify_cfPri2Dscp_get(pri, &dscp), ret);
        diag_util_printf("%11d  %4d\n\r",pri,dscp);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_remarking_dscp */


#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * classf set cf-sel-port ( pon | rg ) ( enable | disable )
 */
cparser_result_t
cparser_cmd_classf_set_cf_sel_port_pon_rg_enable_disable(
    cparser_context_t *context)
{
    rtk_port_t              port=0;
    rtk_classify_cf_sel_t   cfSel=CLASSIFY_CF_SEL_DISABLE;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();

    if('p' == TOKEN_CHAR(3,0))
        port = HAL_GET_PON_PORT();
    else if('r' == TOKEN_CHAR(3,0))
        port = HAL_GET_RGMII_PORT();

    if('e' == TOKEN_CHAR(4,0))
        cfSel = CLASSIFY_CF_SEL_ENABLE;
    else if('d' == TOKEN_CHAR(4,0))
        cfSel = CLASSIFY_CF_SEL_DISABLE;

    DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_set(port, cfSel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_cf_sel_port_pon_rg_enable_disable */

/*
 * classf set cf-sel-port port ( <PORT_LIST:ports> | all ) ( enable | disable )
 */
cparser_result_t
cparser_cmd_classf_set_cf_sel_port_port_ports_all_enable_disable(
    cparser_context_t *context)
{
    rtk_port_t              port;
    diag_portlist_t         portlist;
    rtk_classify_cf_sel_t   cfSel=CLASSIFY_CF_SEL_DISABLE;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5,0))
        cfSel = CLASSIFY_CF_SEL_ENABLE;
    else if('d' == TOKEN_CHAR(5,0))
        cfSel = CLASSIFY_CF_SEL_DISABLE;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_set(port, cfSel), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_cf_sel_port_port_ports_all_enable_disable */

/*
 * classf get cf-sel-port
 */
cparser_result_t
cparser_cmd_classf_get_cf_sel_port(
    cparser_context_t *context)
{
    rtk_port_t              port;
    rtk_classify_cf_sel_t   cfSel;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    port = HAL_GET_PON_PORT();
    DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_get(port, &cfSel), ret);
    diag_util_printf("Cf select port: PON %s\n", diagStr_enable[cfSel]);

#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        port = HAL_GET_RGMII_PORT();
        DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_get(port, &cfSel), ret);
        diag_util_printf("Cf select port: RGMII %s\n", diagStr_enable[cfSel]);
    }
#endif

#ifdef CONFIG_SDK_RTL9607C
    if(RTL9607C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Cf select enable port: ");
        HAL_SCAN_ALL_PORT(port)
        {
            DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_get(port, &cfSel), ret);
            if(CLASSIFY_CF_SEL_ENABLE == cfSel)
            {
                diag_util_printf("%d ", port);
            }
        }
        diag_util_printf("\n");
    }
#endif

#ifdef CONFIG_SDK_RTL9603CVD
    if(RTL9603CVD_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
        diag_util_printf("Cf select enable port: ");
        HAL_SCAN_ALL_PORT(port)
        {
            DIAG_UTIL_ERR_CHK(rtk_classify_cfSel_get(port, &cfSel), ret);
            if(CLASSIFY_CF_SEL_ENABLE == cfSel)
            {
                diag_util_printf("%d ", port);
            }
        }
        diag_util_printf("\n");
    }
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_cf_sel_port */

/*
 * classf get range-ip entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_range_ip_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_ip_t ipRange;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_IP_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_get(&ipRange), ret);
    diag_util_printf("Range check of IP address\n");
    diag_util_printf("Index: %d Upper: %s ",
                                                    ipRange.index,
                                                    diag_util_inet_ntoa(ipRange.upperIp));
    diag_util_printf("Lower: %s Type: %s\n",
                                                    diag_util_inet_ntoa(ipRange.lowerIp),
                                                    diagStr_cfRangeCheckIpTypeStr[ipRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_range_ip_entry_index */

/*
 * classf set range-ip entry <UINT:index> type ( sip | dip )
 */
cparser_result_t
cparser_cmd_classf_set_range_ip_entry_index_type_sip_dip(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_ip_t    ipRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_IP_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_get(&ipRange), ret);

    if('s' == TOKEN_CHAR(6,0))
        ipRange.type = CLASSIFY_IPRANGE_IPV4_SIP;
    else if('d' == TOKEN_CHAR(6,0))
        ipRange.type = CLASSIFY_IPRANGE_IPV4_DIP;

    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_ip_entry_index_type_sip_dip */

/*
 * classf set range-ip entry <UINT:index> low-bound <IPV4ADDR:low_bound_ip> up-bound <IPV4ADDR:up_bound_ip>
 */
cparser_result_t
cparser_cmd_classf_set_range_ip_entry_index_low_bound_low_bound_ip_up_bound_up_bound_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *low_bound_ip_ptr,
    uint32_t  *up_bound_ip_ptr)
{
    rtk_classify_rangeCheck_ip_t    ipRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_IP_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_get(&ipRange), ret);

    ipRange.lowerIp = *low_bound_ip_ptr;
    ipRange.upperIp = *up_bound_ip_ptr;

    DIAG_UTIL_ERR_CHK(rtk_classify_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_ip_entry_index_low_bound_low_bound_ip_up_bound_up_bound_ip */

/*
 * classf get range-l4port entry <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_range_l4port_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_l4Port_t portRange;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_L4PORT_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_get(&portRange), ret);

    diag_util_printf("Range check of L4 port\n");
    diag_util_printf("Index: %d Upper: %d Lower: %d Type: %s\n",
                                                    portRange.index,
                                                    portRange.upperPort,
                                                    portRange.lowerPort,
                                                    diagStr_cfRangeCheckPortTypeStr[portRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_range_l4port_entry_index */

/*
 * classf set range-l4port entry <UINT:index> type ( src-port | dst-port )
 */
cparser_result_t
cparser_cmd_classf_set_range_l4port_entry_index_type_src_port_dst_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_classify_rangeCheck_l4Port_t portRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_L4PORT_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_get(&portRange), ret);

    if('s' == TOKEN_CHAR(6,0))
        portRange.type = CLASSIFY_PORTRANGE_SPORT;
    else if('d' == TOKEN_CHAR(6,0))
        portRange.type = CLASSIFY_PORTRANGE_DPORT;

    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_set(&portRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_l4port_entry_index_type_src_port_dst_port */

/*
 * classf set range-l4port entry <UINT:index> low-bound <UINT:l4lport> up-bound <UINT:l4uport>
 */
cparser_result_t
cparser_cmd_classf_set_range_l4port_entry_index_low_bound_l4lport_up_bound_l4uport(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l4lport_ptr,
    uint32_t  *l4uport_ptr)
{
    rtk_classify_rangeCheck_l4Port_t portRange;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_CLASSIFY_L4PORT_RANGE_NUM()), CPARSER_ERR_INVALID_PARAMS);

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_get(&portRange), ret);

    portRange.lowerPort = (uint16)*l4lport_ptr;
    portRange.upperPort = (uint16)*l4uport_ptr;

    DIAG_UTIL_ERR_CHK(rtk_classify_portRange_set(&portRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_range_l4port_entry_index_low_bound_l4lport_up_bound_l4uport */


/*
 * classf set permit-sel-range ( 0to511 | 64to511 )
 */
cparser_result_t
cparser_cmd_classf_set_permit_sel_range_0to511_64to511(
    cparser_context_t *context)
{
    int32 ret;
    rtk_classify_permit_sel_t permitSel=CLASSIFY_CF_SEL_FROM_0;

    DIAG_UTIL_PARAM_CHK();

    if('0' == TOKEN_CHAR(3,0))
        permitSel = CLASSIFY_CF_SEL_FROM_0;
    else if('6' == TOKEN_CHAR(3,0))
        permitSel = CLASSIFY_CF_SEL_FROM_64;

    DIAG_UTIL_ERR_CHK(rtk_classify_permit_sel_set(permitSel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_permit_sel_range_0to511_64to511 */

/*
 * classf get permit-sel-range */
cparser_result_t
cparser_cmd_classf_get_permit_sel_range(
    cparser_context_t *context)
{

    int32 ret;
    rtk_classify_permit_sel_t permitSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_permit_sel_get(&permitSel), ret);

    if(permitSel == CLASSIFY_CF_SEL_FROM_0){
         diag_util_printf("Permit range from 0 to 511\n\r");
    }else
    {
         diag_util_printf("Permit range from 64 to 511\n\r");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_permit_sel_range */


/*
 * classf set us-1p-remark-prior ( cf | acl )
 */
cparser_result_t
cparser_cmd_classf_set_us_1p_remark_prior_cf_acl(
    cparser_context_t *context)
{
    int32 ret;
    rtk_classify_us_1premark_prior_t prior=CLASSIFY_US_1PREMARK_PRIOR_ACL;

    DIAG_UTIL_PARAM_CHK();

    if('c' == TOKEN_CHAR(3,0))
        prior = CLASSIFY_US_1PREMARK_PRIOR_CF;
    else if('a' == TOKEN_CHAR(3,0))
        prior = CLASSIFY_US_1PREMARK_PRIOR_ACL;

    DIAG_UTIL_ERR_CHK(rtk_classify_us1pRemarkPrior_set(prior), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_permit_sel_range_0to511_64to511 */

/*
 * classf get us-1p-remark-prior */
cparser_result_t
cparser_cmd_classf_get_us_1p_remark_prior(
    cparser_context_t *context)
{
    int32 ret;
    rtk_classify_us_1premark_prior_t prior;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_us1pRemarkPrior_get(&prior), ret);

    if(prior == CLASSIFY_US_1PREMARK_PRIOR_CF){
         diag_util_printf("CF prior than ACL\n\r");
    }else
    {
         diag_util_printf("ACL prior than CF\n\r");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_permit_sel_range */



/*
 * classf set template-pattern0 ( us | ds ) <UINT:index> vid-cfg ( ingress-cvid | ingress-outer-vid | internal-cvid ) pri-cfg ( ingress-cpri | internal-pri | remark-cpri )
 */
cparser_result_t
cparser_cmd_classf_set_template_pattern0_us_ds_index_vid_cfg_ingress_cvid_ingress_outer_vid_internal_cvid_pri_cfg_ingress_cpri_internal_pri_remark_cpri(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_classify_template_cfg_pattern0_t templateCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr > 1), CPARSER_ERR_INVALID_PARAMS);

    templateCfg.index = *index_ptr;

    if (0 == osal_strcmp("us", TOKEN_STR(3)))
        templateCfg.direction = CLASSIFY_DIRECTION_US;
    else
        templateCfg.direction = CLASSIFY_DIRECTION_DS;

    if (0 == osal_strcmp("ingress-cvid", TOKEN_STR(6)))
        templateCfg.vidCfg = CLASSIFY_TEMPLATE_VID_CFG_INGRESS_CVID;
    else if (0 == osal_strcmp("ingress-outer-vid", TOKEN_STR(6)))
        templateCfg.vidCfg = CLASSIFY_TEMPLATE_VID_CFG_INGRESS_OUTERTAG_VID;
    else
        templateCfg.vidCfg = CLASSIFY_TEMPLATE_VID_CFG_INTERNAL_CVID;

    if (0 == osal_strcmp("ingress-cpri", TOKEN_STR(8)))
        templateCfg.priCfg = CLASSIFY_TEMPLATE_PRI_CFG_INGRESS_CPRI;
    else if (0 == osal_strcmp("internal-pri", TOKEN_STR(8)))
        templateCfg.priCfg = CLASSIFY_TEMPLATE_PRI_CFG_INTERNAL_PRI;
    else
        templateCfg.priCfg = CLASSIFY_TEMPLATE_PRI_CFG_REMARK_CPRI;

    DIAG_UTIL_ERR_CHK(rtk_classify_templateCfgPattern0_set(&templateCfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_template_pattern0_us_ds_index_vid_cfg_ingress_cvid_ingress_outer_vid_internal_cvid_pri_cfg_ingress_cpri_internal_pri_remark_cpri */
/*
 * classf get template-pattern0 ( us | ds ) <UINT:index>
 */
cparser_result_t
cparser_cmd_classf_get_template_pattern0_us_ds_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_classify_template_cfg_pattern0_t templateCfg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    templateCfg.index = *index_ptr;
    if (0 == osal_strcmp("us", TOKEN_STR(3)))
        templateCfg.direction = CLASSIFY_DIRECTION_US;
    else
        templateCfg.direction = CLASSIFY_DIRECTION_DS;

    DIAG_UTIL_ERR_CHK(rtk_classify_templateCfgPattern0_get(&templateCfg), ret);
    diag_util_mprintf("Template %s index %d cfg: VID: %s, PRI: %s\n",
        diagStr_direction[templateCfg.direction], templateCfg.index,
        diagStr_cfTemplatePtn0VidCfg[templateCfg.vidCfg],
        diagStr_cfTemplatePtn0PriCfg[templateCfg.priCfg]);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_template_pattern0_us_ds_index */

/*
 * classf set num-pattern1 <UINT:num>
 */
cparser_result_t
cparser_cmd_classf_set_num_pattern1_num(
    cparser_context_t *context,
    uint32_t  *num_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*num_ptr > 255), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_classify_entryNumPattern1_set(*num_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_num_pattern1_num */

/*
 * classf get num-pattern1
 */
cparser_result_t
cparser_cmd_classf_get_num_pattern1(
    cparser_context_t *context)
{
    int32 ret;
    uint32 num_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_entryNumPattern1_get(&num_ptr), ret);

    diag_util_mprintf("Entry num of Pattern 1 : %d \n", num_ptr);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_num_pattern1 */


/*
 *classf set default-wan-if l2 <UINT:l2wan>
 */
cparser_result_t
cparser_cmd_classf_set_default_wan_if_l2_l2wan(
    cparser_context_t *context,
    uint32_t  *l2wan_ptr)
{
    int32 ret;
    rtk_classify_default_wanIf_t wanIf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*l2wan_ptr >= HAL_L34_NETIF_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_classify_defaultWanIf_get(&wanIf), ret);
    wanIf.l2WanIf = *l2wan_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_defaultWanIf_set(&wanIf), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_default_wan_if_l2_l2wan_mcast_mcastwan */

/*
 *classf set default-wan-if mcast <UINT:mcastwan>
 */
cparser_result_t
cparser_cmd_classf_set_default_wan_if_mcast_mcastwan(
    cparser_context_t *context,
    uint32_t  *mcastwan_ptr)
{
    int32 ret;
    rtk_classify_default_wanIf_t wanIf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*mcastwan_ptr >= HAL_L34_NETIF_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_ERR_CHK(rtk_classify_defaultWanIf_get(&wanIf), ret);
    wanIf.mcastWanIf = *mcastwan_ptr;
    DIAG_UTIL_ERR_CHK(rtk_classify_defaultWanIf_set(&wanIf), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_default_wan_if_l2_l2wan_mcast_mcastwan */


/*
 *classf get default-wan-if
 */
cparser_result_t
cparser_cmd_classf_get_default_wan_if(
    cparser_context_t *context)
{
    int32 ret;
    rtk_classify_default_wanIf_t wanIf;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_defaultWanIf_get(&wanIf), ret);

    diag_util_mprintf("Default WAN interface index for L2 packet: %d \n", wanIf.l2WanIf);
    diag_util_mprintf("Default WAN interface index for Multicast packet: %d \n", wanIf.mcastWanIf);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_default_wan_if_l2_l2wan_mcast_mcastwan */

/*
 *classf set trap-pri <UINT:pri>
 */
cparser_result_t
cparser_cmd_classf_set_trap_pri_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_trapPri_set(*pri_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_set_trap_pri_pri */


/*
 *classf get trap-pri
 */
cparser_result_t
cparser_cmd_classf_get_trap_pri(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pri_t pri;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_classify_trapPri_get(&pri), ret);

    diag_util_mprintf("CF Trap Priority: %d \n", pri);

    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_trap_pri */
#endif
