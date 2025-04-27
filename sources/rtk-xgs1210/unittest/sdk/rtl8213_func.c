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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition of SDK test APIs in the SDK
 *
 * Feature : SDK test APIs
 *
 */

#include <stdlib.h>
#include "rtl8213_func.h"
#include "rtl8213m_asicdrv.h"
#ifdef _DOT3AHOAM_
#include "./802dot3ah/rtl_1ah_oam.h"
#endif
#include "driver.h"
#include "memory.h"
#include "common.h"
#include "stdio.h"
#include "mcinfo.h"
#include <string.h>
#include "config.h"
#include <ctype.h>
#include "dhcp/netif.h"
#include "rtl8213_debug.h"

#ifdef _SNMP_
#include "./snmp/snmp_msg.h"
#endif

idata uint8 tmpBuf[6];

/* Function Name:
 *      rtl8363_setRemoteAsicReg
 * Description:
 *      Set remote Asic registers using ts1000
 * Input:
 *      phyad      - PHY address (0~2)
 *      regad       - Register address(0~31)
 *      pagtype   -  Register Page Type
 *      npage      -  Register Page Number
 *      value       -  Write value to register
 * Output:
 *      none
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      pagtype has following Macros to be chosen,
 *      MACPAG represents MAC page, set mac function
 *      UTPPAG  represents PHY page, set utp phy
 *      SERLPAG represents Serdes page, set fiber phy
 *      this function use ts1k to access remote device register,
 *      local must be center mode, remote must be terminal mode
 */
int8 rtl8363_setRemoteAsicReg(uint8 phyad, uint8 regad, uint8 pagtype,
                              uint8 npage, uint16 value)
{
    uint16 isa_page = 0;        /*ISA interface Page number */
    uint8 isa_regL = 0;         /*ISA interface 16bit-Low register address */
    int8 retval;                /*return value */
    TS1kRemoteRWPara_t xdata RmtRWPara;

    if ((phyad > 2) || (regad > 31) || (pagtype > SERHPAG) || (npage > 15))
    {
        DEBUG_RMTRW(("\nrtl8363_setRemoteAsicReg Input paraments wrong!\n"));
        return FAILED;
    }


    /*first map SMI page &reg. to ISA page reg. */
    if ((!pagtype) && (regad <= 11))
    {
        isa_page = phyad * 2;
        isa_regL = 4 + regad * 2;
    }
    else if ((!pagtype) && (regad >= 12) && (regad <= 15))
    {
        isa_page = phyad * 2 + 1;
        isa_regL = 4 + (regad - 12) * 2;
    }
    else if (regad == 31)
    {
        isa_page = 6;
        isa_regL = 4 + phyad * 2;
    }
    else if ((!pagtype) && (regad == 16))
    {
        isa_page = 7;
        isa_regL = 4 + phyad * 2;
    }
    else if ((!pagtype) && (regad >= 17) && (regad <= 30))
    {
        isa_page = 8 + phyad * 16 + npage;
        isa_regL = 4 + (regad - 17) * 2;
    }
    else if (pagtype && (phyad != 2))
    {
        if (regad <= 13)
        {
            isa_page = ((pagtype & 0x3) + 1) * 48 + phyad * 144 + npage * 3;
            isa_regL = 4 + regad * 2;
        }
        else if (regad <= 27)
        {
            isa_page = ((pagtype & 0x3) + 1) * 48 + phyad * 144 + npage * 3 + 1;
            isa_regL = 4 + (regad - 14) * 2;
        }
        else if (regad <= 30)
        {
            isa_page = ((pagtype & 0x3) + 1) * 48 + phyad * 144 + npage * 3 + 2;
            isa_regL = 4 + (regad - 28) * 2;
        }

    }
    else
    {
        return FAILED;
    }

    /*second write register value through RMON interface */

    /*select page */
    if ((isa_page >= 128) && (isa_page <= 255))
    {
        if ((retval =
             rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT,
                                                   ISA128_255)) != SUCCESS)
        {
#if DBG_RMTRW
            DEBUG_RMTRW(("\In rtl8363_setAsicTs1kOAMRemoteRWPageSel!128-255\n"));
            switch (retval)
            {
            case TS1K_DISABLED:
                DEBUG_RMTRW(("\nTS1K DISABLED!\n"));
                break;
            case TS1K_NOTRXRSP:
                DEBUG_RMTRW(("\nNot receive response!\n"));
                break;
            case TS1K_RXINVDRSP:
                DEBUG_RMTRW(("\nReceive invalid response!\n"));
                break;
            case TS1K_NOTRXMAGRSP:
                DEBUG_RMTRW(("\nNot rx mag response!\n"));
                break;
            case TS1K_MAGICNUMERR:
                DEBUG_RMTRW(("\nMagic num error!\n"));
                break;
            case TS1K_RXBUSYRSP:
                DEBUG_RMTRW(("\nRx busy response!\n"));
                break;
            case TS1K_DISREMORW:
                DEBUG_RMTRW(("\ncenter does not enable remote rw ability!\n"));
                break;
            case TS1K_NOTCEN:
                DEBUG_RMTRW(("\nnot center!\n"));
                break;
            default:
                break;
            }
#endif
            return retval;
        }
    }
    else if ((isa_page >= 256) && (isa_page <= 383))
    {
        if ((retval =
             rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT,
                                                   ISA256_383)) != SUCCESS)
        {
#if DBG_RMTRW
            DEBUG_RMTRW(("\In rtl8363_setAsicTs1kOAMRemoteRWPageSel!256-383\n"));
            switch (retval)
            {
            case TS1K_DISABLED:
                DEBUG_RMTRW(("\TS1K DISABLED!\n"));
                break;
            case TS1K_NOTRXRSP:
                DEBUG_RMTRW(("\nNot receive response!\n"));
                break;
            case TS1K_RXINVDRSP:
                DEBUG_RMTRW(("\nReceive invalid response!\n"));
                break;
            case TS1K_NOTRXMAGRSP:
                DEBUG_RMTRW(("\nNot rx mag response!\n"));
                break;
            case TS1K_MAGICNUMERR:
                DEBUG_RMTRW(("\nMagic num error!\n"));
                break;
            case TS1K_RXBUSYRSP:
                DEBUG_RMTRW(("\nRx busy response!\n"));
                break;
            case TS1K_DISREMORW:
                DEBUG_RMTRW(("\ncenter does not enable remote rw ability!\n"));
                break;
            case TS1K_NOTCEN:
                DEBUG_RMTRW(("\nnot center!\n"));
                break;
            default:
                break;
            }
#endif
            return retval;
        }
    }

    /*low 8bits */
    RmtRWPara.RTMOP = TS1K_RTMOP_WRITE;
    RmtRWPara.ISAPAG = isa_page;
    RmtRWPara.ISAREG = isa_regL;
    RmtRWPara.RMRWData = value;
    if ((retval =
         rtl8363_setAsicTs1kOAMTrigRemoteRW(FIBERPORT, &RmtRWPara)) != SUCCESS)
    {
#if DBG_RMTRW
        DEBUG_RMTRW(("\In rtl8363_setAsicTs1kOAMTrigRemoteRW!\n"));
        switch (retval)
        {
        case TS1K_DISABLED:
            DEBUG_RMTRW(("\TS1K DISABLED!\n"));
            break;
        case TS1K_NOTRXRSP:
            DEBUG_RMTRW(("\nNot receive response!\n"));
            break;
        case TS1K_RXINVDRSP:
            DEBUG_RMTRW(("\nReceive invalid response!\n"));
            break;
        case TS1K_NOTRXMAGRSP:
            DEBUG_RMTRW(("\nNot rx mag response!\n"));
            break;
        case TS1K_MAGICNUMERR:
            DEBUG_RMTRW(("\nMagic num error!\n"));
            break;
        case TS1K_RXBUSYRSP:
            DEBUG_RMTRW(("\nRx busy response!\n"));
            break;
        case TS1K_DISREMORW:
            DEBUG_RMTRW(("\ncenter does not enable remote rw ability!\n"));
            break;
        case TS1K_NOTCEN:
            DEBUG_RMTRW(("\nnot center!\n"));
            break;
        default:
            break;
        }
#endif
        return retval;
    }

    /*high 8bits */
    RmtRWPara.RTMOP = TS1K_RTMOP_WRITE;
    RmtRWPara.ISAPAG = isa_page;
    RmtRWPara.ISAREG = (isa_regL + 1);
    RmtRWPara.RMRWData = (value & 0xFF00) >> 8;
    if ((retval =
         rtl8363_setAsicTs1kOAMTrigRemoteRW(FIBERPORT, &RmtRWPara)) != SUCCESS)
    {
#if DBG_RMTRW
        DEBUG_RMTRW(("\In rtl8363_setAsicTs1kOAMTrigRemoteRW!\n"));
        switch (retval)
        {
        case TS1K_DISABLED:
            DEBUG_RMTRW(("\TS1K DISABLED!\n"));
            break;
        case TS1K_NOTRXRSP:
            DEBUG_RMTRW(("\nNot receive response!\n"));
            break;
        case TS1K_RXINVDRSP:
            DEBUG_RMTRW(("\nReceive invalid response!\n"));
            break;
        case TS1K_NOTRXMAGRSP:
            DEBUG_RMTRW(("\nNot rx mag response!\n"));
            break;
        case TS1K_MAGICNUMERR:
            DEBUG_RMTRW(("\nMagic num error!\n"));
            break;
        case TS1K_RXBUSYRSP:
            DEBUG_RMTRW(("\nRx busy response!\n"));
            break;
        case TS1K_DISREMORW:
            DEBUG_RMTRW(("\ncenter does not enable remote rw ability!\n"));
            break;
        case TS1K_NOTCEN:
            DEBUG_RMTRW(("\nnot center!\n"));
            break;
        default:
            break;
        }
#endif
        return retval;
    }

    /*return to default isa page */
    if ((isa_page >= 128) && (isa_page <= 383))
    {
        if ((retval =
             rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT,
                                                   ISA0_127)) != SUCCESS)
        {
#if DBG_RMTRW
            DEBUG_RMTRW(("\rtl8363_setAsicTs1kOAMRemoteRWPageSel!128-383\n"));
            switch (retval)
            {
            case TS1K_DISABLED:
                DEBUG_RMTRW(("\TS1K DISABLED!\n"));
                break;
            case TS1K_NOTRXRSP:
                DEBUG_RMTRW(("\nNot receive response!\n"));
                break;
            case TS1K_RXINVDRSP:
                DEBUG_RMTRW(("\nReceive invalid response!\n"));
                break;
            case TS1K_NOTRXMAGRSP:
                DEBUG_RMTRW(("\nNot rx mag response!\n"));
                break;
            case TS1K_MAGICNUMERR:
                DEBUG_RMTRW(("\nMagic num error!\n"));
                break;
            case TS1K_RXBUSYRSP:
                DEBUG_RMTRW(("\nRx busy response!\n"));
                break;
            case TS1K_DISREMORW:
                DEBUG_RMTRW(("\ncenter does not enable remote rw ability!\n"));
                break;
            case TS1K_NOTCEN:
                DEBUG_RMTRW(("\nnot center!\n"));
                break;
            default:
                break;
            }
#endif
            return retval;
        }
    }
    return SUCCESS;
}

/* Function Name:
 *      rtl8363_getRemoteAsicReg
 * Description:
 *      Get remote Asic registers using ts1000
 * Input:
 *      phyad      - PHY address (0~2)
 *      regad       - Register address(0~31)
 *      pagtype   -  Register Page Type
 *      npage      -  Register Page Number
 * Output:
*       pvalue     -  read value
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      pagtype has following Macros to be chosen,
 *      MACPAG represents MAC page, set mac function
 *      UTPPAG  represents PHY page, set utp phy
 *      SERLPAG represents Serdes page, set fiber phy
 *      this function use ts1k to access remote device register,
 *      local must be center mode, remote must be terminal mode
 */
int8 rtl8363_getRemoteAsicReg(uint8 phyad, uint8 regad, uint8 pagtype,
                              uint8 npage, uint16 * pvalue)
{

    uint16 xdata isa_page = 0;  /*ISA interface Page number */
    uint8 xdata isa_regL = 0;   /*ISA interface 16bit-Low register address */
    TS1kRemoteRWPara_t xdata RmtRWPara;
    int8 xdata retval;          /*return value */

    /*first map SMI page &reg. to ISA page reg. */
    if ((!pagtype) && (regad <= 11))
    {
        isa_page = phyad * 2;
        isa_regL = 4 + regad * 2;
    }
    else if ((!pagtype) && (regad >= 12) && (regad <= 15))
    {
        isa_page = phyad * 2 + 1;
        isa_regL = 4 + (regad - 12) * 2;
    }
    else if (regad == 31)
    {
        isa_page = 6;
        isa_regL = 4 + phyad * 2;
    }
    else if ((!pagtype) && (regad == 16))
    {
        isa_page = 7;
        isa_regL = 4 + phyad * 2;
    }
    else if ((!pagtype) && (regad >= 17) && (regad <= 30))
    {
        isa_page = 8 + phyad * 16 + npage;
        isa_regL = 4 + (regad - 17) * 2;
    }
    else if (pagtype && (phyad != 2))
    {
        if (regad <= 13)
        {
            isa_page = ((pagtype & 0x3) + 1) * 48 + phyad * 144 + npage * 3;
            isa_regL = 4 + regad * 2;
        }
        else if (regad <= 27)
        {
            isa_page = ((pagtype & 0x3) + 1) * 48 + phyad * 144 + npage * 3 + 1;
            isa_regL = 4 + (regad - 14) * 2;
        }
        else if (regad <= 30)
        {
            isa_page = ((pagtype & 0x3) + 1) * 48 + phyad * 144 + npage * 3 + 2;
            isa_regL = 4 + (regad - 28) * 2;
        }

    }
    else
    {
        return FAILED;
    }

    /*second read register value through RMON interface */

    /*select page */
    if ((isa_page >= 128) && (isa_page <= 255))
    {
        if ((retval =
             rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT,
                                                   ISA128_255)) != SUCCESS)
            return retval;
    }
    else if ((isa_page >= 256) && (isa_page <= 383))
    {
        if ((retval =
             rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT,
                                                   ISA256_383)) != SUCCESS)
            return retval;
    }

    /*low 8 bits */
    RmtRWPara.RTMOP = TS1K_RTMOP_READ;
    RmtRWPara.ISAPAG = isa_page;
    RmtRWPara.ISAREG = isa_regL;
    RmtRWPara.RMRWData = 0;
    if ((retval =
         rtl8363_setAsicTs1kOAMTrigRemoteRW(FIBERPORT, &RmtRWPara)) != SUCCESS)
    {
        return retval;
    };
    if ((retval =
         rtl8363_getAsicTs1kOAMRemoteRWinfo(FIBERPORT, &RmtRWPara)) != SUCCESS)
    {
        return retval;
    };
    *pvalue = RmtRWPara.RMRWData;
    /*high 8 bits */
    RmtRWPara.RTMOP = TS1K_RTMOP_READ;
    RmtRWPara.ISAPAG = isa_page;
    RmtRWPara.ISAREG = (isa_regL + 1);
    RmtRWPara.RMRWData = 0;
    if ((retval =
         rtl8363_setAsicTs1kOAMTrigRemoteRW(FIBERPORT, &RmtRWPara)) != SUCCESS)
    {
        return retval;
    };
    if ((retval =
         rtl8363_getAsicTs1kOAMRemoteRWinfo(FIBERPORT, &RmtRWPara)) != SUCCESS)
    {
        return retval;
    };
    *pvalue |= ((RmtRWPara.RMRWData) << 8);

    /*return to default isa page */
    if ((isa_page >= 128) && (isa_page <= 383))
    {
        if ((retval =
             rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT,
                                                   ISA0_127)) != SUCCESS)
            return retval;
    }

    return SUCCESS;
}

/* Function Name:
 *      rtl8363_setRemoteAsicRegBit
 * Description:
 *      Set remote Asic registers bit using ts1000
 * Input:
 *      phyad      - PHY address (0~2)
 *      regad       - Register address(0~31)
 *      pagtype   -  Register Page Type
 *      npage      -  Register Page Number
 *      bitpos      -  bit position
 * Output:
*       none
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      pagtype has following Macros to be chosen,
 *      MACPAG represents MAC page, set mac function
 *      UTPPAG  represents PHY page, set utp phy
 *      SERLPAG represents Serdes page, set fiber phy
 *      this function use ts1k to access remote device register,
 *      local must be center mode, remote must be terminal mode
 */
int8 rtl8363_setRemoteAsicRegBit(uint8 phyad, uint8 regad, uint8 bitpos,
                                 uint8 pagtype, uint8 npage, uint16 value)
{
    uint16 xdata rdata;

    if (rtl8363_getRemoteAsicReg(phyad, regad, pagtype, npage, &rdata) !=
        SUCCESS)
    {
        DEBUG_RMTRW(("\ngetRemoteAsicReg failed!\n"));
        return FAILED;
    }
    if (value)
    {
        if (SUCCESS !=
            rtl8363_setRemoteAsicReg(phyad, regad, pagtype, npage,
                                     rdata | (1 << bitpos)))
        {
            DEBUG_RMTRW(("\nrtl8363_setRemoteAsicReg failed!\n"));
            return FAILED;
        }
    }
    else
    {
        if (SUCCESS !=
            rtl8363_setRemoteAsicReg(phyad, regad, pagtype, npage,
                                     rdata & (~(1 << bitpos))))
        {
            DEBUG_RMTRW(("\nrtl8363_setRemoteAsicReg failed!\n"));
            return FAILED;
        }
    }
    return SUCCESS;
}

#ifdef _WEBSMART_

/* Function Name:
 *      rtl8363_getRemoteAsicRegBit
 * Description:
 *      Get remote Asic registers bit using ts1000
 * Input:
 *      phyad      - PHY address (0~2)
 *      regad       - Register address(0~31)
 *      pagtype   -  Register Page Type
 *      npage      -  Register Page Number
 *      bitpos      -  bit position
 * Output:
*       pvalue      - the bit value
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      pagtype has following Macros to be chosen,
 *      MACPAG represents MAC page, set mac function
 *      UTPPAG  represents PHY page, set utp phy
 *      SERLPAG represents Serdes page, set fiber phy
 *      this function use ts1k to access remote device register,
 *      local must be center mode, remote must be terminal mode
 */
int8 rtl8363_getRemoteAsicRegBit(uint8 phyad, uint8 regad, uint8 bitpos,
                                 uint8 pagtype, uint8 npage, uint16 * pvalue)
{
    uint16 rdata;

    if (rtl8363_getRemoteAsicReg(phyad, regad, pagtype, npage, &rdata) !=
        SUCCESS)
        return FAILED;
    if (rdata & (1 << bitpos))
        *pvalue = 1;
    else
        *pvalue = 0;

    return SUCCESS;
}
#endif

/* Function Name:
 *      dispLinkStatus
 * Description:
 *      display link status in uart console
 * Input:
 *      none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 */
int8 dispLinkStatus()
{
    uint16 xdata regval;
    uint8 xdata flag, i;
    Ts1kCfgPara_t xdata cfg;

    rtl8363_getAsicReg(1, 1, SERLPAG, 0, &regval);
    rtl8363_getAsicReg(1, 1, SERLPAG, 0, &regval);
    flag = regval & 0x4;

    /*display local information*/
    printf("\nPort\t\tStatus\n");
    printf("Local fiber:\t%s\n", flag ? "OK" : "Link Down");
    rtl8363_getAsicTs1kOAMSField(FIBERPORT, &regval);
    printf("Local UTP:\t");
    if (regval & 0x4)
    {
        printf("%s\n", "Link Down");
    }
    else
    {
        printf("%s %s\n",
               (regval & 0x80) ? "1000M" : ((regval & 0x100) ? "100M" : "10M"),
               (regval & 0x200) ? "Full" : "Half");
    }
    rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);

    /* enable ts1000 and center mode and fiber link ok, set oam interrupt mask*/
    if (cfg.EnTS1K && cfg.MCIdnt && flag)
    {
        rtl8363_setAsicRegBit(1, 24, 7, MACPAG, 7, 1);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerStatusReq);
        for (i = 0; i < 100; i++)
        {
            rtl8363_getAsicRegBit(1, 25, 7, MACPAG, 7, &regval);
            if (regval)
                break;
        }
        if (i == 100)
        {
            printf("Get Remote Link Partner Status Fail\n");
            return FAILED;
        }
        rtl8363_getAsicReg(FIBERPORT, 18, MACPAG, 6, &regval);
        printf("Remote fiber:\tOK\n");
        if (regval & 0x4)
            printf("Remote UTP:\tLink Down\n");
        else
            printf("Remote UTP:\t%s %s\n",
                   (regval & 0x80) ? "1000M" : (regval & 0x100) ? "100M" :
                   "10M", regval & 0x200 ? "Full" : "Half");

    }
    return SUCCESS;
}


/* Function Name:
 *      dispLocalStatus
 * Description:
 *      display ts1000 oam local status  in uart console
 * Input:
 *      none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 */

int8 dispLocalStatus()
{
    uint16 xdata regval;
    uint16 xdata bitval;
    Ts1kCfgPara_t xdata cfg;
    rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);
    if (cfg.EnTS1K == FALSE)
    {
        printf("\nTS1000 function is disabled.\n");
        return SUCCESS;
    }
    else
    {
        printf("\nTS1000 function is enabled.\n");
    }
    rtl8363_getAsicTs1kOAMSField(FIBERPORT, &regval);
    //printf("Debug --> S field: %x\n", regval);
    printf("TS1000 mode:\t\t%s\n", cfg.MCIdnt ? "center" : "terminal");
    if (cfg.MCIdnt == FALSE)
    {
        //terminal only
        printf("Power status:\t\t%s\n",
               regval & 0x1 ? "Power supply failure" : "Normal");
    }
    printf("OpticalRxSigStats:\t%s\n", regval & 0x2 ? "Abnormal" : "Normal");
    rtl8363_getAsicRegBit(UTPPORT, 17, 10, UTPPAG, 0, &bitval);
    printf("NetlinkStatus:\t\t%s\n",
           bitval ? "UTP Established":"UTP Unestablished or disconnected");
#if 0
    /* when force link down, regval & 0x4 still display UTP up*/
    printf("NetlinkStatus:\t\t%s\n",
           regval & 0x4 ? "UTP Unestablished or disconnected" :
           "UTP Established");
    printf("MC status:\t\t%s\n", regval&0x8?"Fail":"Normal");
#endif
    if (cfg.MCIdnt == FALSE)
    {
        printf("LossOpNotifWay:\t\t%s\n",
               (regval & 0x10) ? "with alarm FEFI" : "with OAM frame");
        printf("Operation status:\t%s\n",
               (regval & 0x20) ? "under loop back test" :
               "under ordinary operation");
        printf("MCOptionB:\t\t%s\n",
               (regval & 0x40) ? "Support" : "Not support");
        printf("Terminal Link Rate:\t%s\n",
               (regval & 0x80) ? "1000M" : (regval & 0x100) ? "100M" : "10M");
        printf("Terminal Link Duplex:\t%s\n", regval & 0x200 ? "Full" : "Half");
        printf("Terminal Link Nway:\t%s\n",
               regval & 0x400 ? "Enable" : "Disable");
        printf("NumPhyInterface:\t%s\n",
               regval & 0x800 ? "Greater than one" : "One");
    }

    return SUCCESS;
}

/* Function Name:
 *      dispLocalStatus
 * Description:
 *      display ts1000 oam remote status  in uart console
 * Input:
 *      none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *       local must be center mode, remote must be terminal mode
 */
int8 dispRemoteStatus()
{
    uint16 xdata regval, xdata i;
    Ts1kCfgPara_t xdata cfg;
    uint16 xdata bitval;


    rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);
    if (cfg.EnTS1K == FALSE)
    {
        printf("Error: TS1000 function is disabled.\n");
        return FAILED;
    }
    if (cfg.MCIdnt == FALSE)
    {
        printf("Error: Only Center can use this function.\n");
        return FAILED;
    }

    /*get remote utp link status, when force link down, s2 not right*/
    rtl8363_setAsicTs1kOAMRemoteRWEnable(FIBERPORT, TRUE);
    rtl8363_getRemoteAsicRegBit(UTPPORT, 17, 10, UTPPAG, 0, &bitval);



    rtl8363_setAsicRegBit(1, 24, 7, MACPAG, 7, 1);      //set oam interrupt mask
    rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerStatusReq);
    for (i = 0; i < RTL8363_TS1K_TIMEOUT; i++)
    {
        rtl8363_getAsicRegBit(1, 25, 7, MACPAG, 7, &regval);
        if (regval)
            break;
    }
    if (i == RTL8363_TS1K_TIMEOUT)
    {
        printf("Link Fault!\n");
        return FAILED;
    }

    rtl8363_getAsicReg(FIBERPORT, 18, MACPAG, 6, &regval);      //received OAM Frame's S Value

    printf("Remote TS1000 stauts:\n");
    printf("Power status:\t\t%s\n",
           regval & 0x1 ? "Power supply failure" : "Normal");
    printf("OpticalRxSigStats:\t%s\n", regval & 0x2 ? "Abnormal" : "Normal");
    /*
    printf("NetlinkStatus:\t\t%s\n",
           regval & 0x4 ? "UTP Unestablished or disconnected" :
           "UTP Established");
    */
    printf("NetlinkStatus:\t\t%s\n",
           bitval?  "UTP Established":"UTP Unestablished or disconnected");

    //printf("MC status:\t\t%s\n", regval&0x8?"Fail":"Normal");
    printf("LossOpNotifWay:\t\t%s\n",
           (regval & 0x10) ? "with alarm FEFI" : "with OAM frame");
    printf("Operation status:\t%s\n",
           (regval & 0x20) ? "under loop back test" :
           "under ordinary operation");
    printf("MCOptionB:\t\t%s\n", (regval & 0x40) ? "Support" : "Not support");
    if(bitval)
    {
        printf("Terminal Link Rate:\t%s\n",
               (regval & 0x80) ? "1000M" : (regval & 0x100) ? "100M" : "10M");
        printf("Terminal Link Duplex:\t%s\n", regval & 0x200 ? "Full" : "Half");
        printf("Terminal Link Nway:\t%s\n", regval & 0x400 ? "Enable" : "Disable");
        printf("NumPhyInterface:\t%s\n",
                regval & 0x800 ? "Greater than one" : "One");
    }
    else
    {
        printf("Terminal Link Rate:\t%s\n", "--");
        printf("Terminal Link Duplex:\t%s\n", "--");
        printf("Terminal Link Nway:\t%s\n", "--");
        printf("NumPhyInterface:\t%s\n", "--");
    }

#if 1
    /*get remote magic code to recognize whether it is dump Media Converter
      *enable remote magic read/write ability
      */
    if (SUCCESS != rtl8363_setAsicTs1kOAMRemoteRWEnable(FIBERPORT, TRUE))
        printf("Can't enable remote magic read/write ability\n");
    if (SUCCESS != rtl8363_getRemoteAsicReg(0, 17, MACPAG, 5, &regval))
        printf("Magic Read remote register failed!\n");
    if (regval != MAGIC_CODE)
        printf("remote is dump media converter!\n");
#endif
    return SUCCESS;
}

/* Function Name:
 *      checkLink
 * Description:
 *      check fiber link status
 * Input:
 *      none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *       local must be center mode, remote must be terminal mode, it will
 *       trigger auto loopback to check link status
 */
int8 checkLink()
{
    uint8 xdata isEnd, xdata isGood;
    Ts1kCfgPara_t xdata cfg;
    rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);
    if (cfg.EnTS1K == FALSE)
    {
        printf("\nError: TS1000 function is disabled.\n");
        return FAILED;
    }
    if (cfg.MCIdnt == FALSE)
    {
        printf("\nError: Only Center can use this function.\n");
        return FAILED;
    }
    rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerAutoLB);
    rtl8363_getAsicTs1kOAMAutoLBResult(FIBERPORT, &isEnd, &isGood);
    if (isGood == TRUE)
        printf("\nRemote Auto Loopback OK!\n");
    else
        printf("\nRemote Auto Loopback Fail!\n");
    return SUCCESS;
}

/* Function Name:
 *      setRemotePhy
 * Description:
 *      set remote phy
 * Input:
 *      mode - phy work mode
 *      type  - cmd type
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *      local must be center mode, remote must be terminal mode,
 *      thus could do remote control
 *      mode  1 1000 FULL, 2 100FULL, 3 100 half, 4 10 FULL, 5 10 HALF
 *      type   1 speed and duplex, disable nway, 2 enable nway
 */
int8 setRemotePhy(uint8 mode, uint8 type)
{
    uint16 xdata regval;
    Ts1kCfgPara_t xdata cfg;
    rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);
    if (cfg.EnTS1K == FALSE)
    {
        printf("\nError: TS1000 function is disabled.\n");
        return FAILED;
    }
    if (cfg.MCIdnt == FALSE)
    {
        printf("\nError: Only Center can use this function.\n");
        return FAILED;
    }
    rtl8363_setAsicTs1kOAMRemoteRWEnable(FIBERPORT, TRUE);      //add 2008.10.10
    if (rtl8363_getRemoteAsicReg(0, 0, UTPPAG, 0, &regval) != SUCCESS)
    {
        printf
            ("\nOperation fail. Enable function first or check link status.\n");
        return FAILED;
    }
    if (type == 1)              // speed and duplex, disable nway
    {
        if (mode == 1)          //1000 FULL
        {
            regval = (regval & ~0x3140) | 0x140;
        }
        else if (mode == 2)     //100 FULL
        {
            regval = (regval & ~0x3140) | 0x2100;
        }
        else if (mode == 3)     //100 half
        {
            regval = (regval & ~0x3140) | 0x2000;
        }
        else if (mode == 4)     //10 FULL
        {
            regval = (regval & ~0x3140) | 0x100;
        }
        else if (mode == 5)     //10 half
        {
            regval = (regval & ~0x3140) | 0x0;
        }
    }
    else if (type == 2)         //restart nway
    {
        regval = regval | 0x1200;
    }
    rtl8363_setRemoteAsicReg(0, 0, UTPPAG, 0, regval);
    printf("\nRemote configure success!\n");
    return SUCCESS;
}

/* Function Name:
 *      setFlowcontrol
 * Description:
 *      set flow control ability
 * Input:
 *      portno  - port number
 *      enable  - enable or disable flow control
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *       portno could be UTPPORT or FIBERPORT
 */
void setFlowcontrol(int portno, int enable)
{
    if (portno==UTPPORT)
    {
        rtl8363_setAsicRegBit(UTPPORT,4,10,UTPPAG,0,enable);
        rtl8363_setAsicRegBit(UTPPORT,4,11,UTPPAG,0,enable);
    }
    else
    {
        if (asicid==RTL8203M)
        {
            rtl8363_setAsicRegBit(FIBERPORT,4,10,SERLPAG,0,enable);
            rtl8363_setAsicRegBit(FIBERPORT,4,11,SERLPAG,0,enable);
        }
        else
        {
            rtl8363_setAsicRegBit(FIBERPORT,4,7,SERLPAG,0,enable);
            rtl8363_setAsicRegBit(FIBERPORT,4,8,SERLPAG,0,enable);
        }
    }
}

/* Function Name:
 *      getFlowcontrol
 * Description:
 *      get flow control ability
 * Input:
 *      portno  - port number
 * Output:
 *       none
 * Return:
 *       TRUE   - flow control is enabled
 *       FALSE  - flow control is disabled
 * Note:
 *       portno could be UTPPORT or FIBERPORT
 */
int getFlowcontrol(int portno)
{
    uint16 regval;

    if (portno==UTPPORT)
    {
        rtl8363_getAsicRegBit(UTPPORT,4,10,UTPPAG,0,&regval);
        if (regval)
            rtl8363_getAsicRegBit(UTPPORT,4,11,UTPPAG,0,&regval);
    }
    else
    {
        if (asicid==RTL8203M)
        {
            rtl8363_getAsicRegBit(FIBERPORT,4,10,SERLPAG,0,&regval);
            if (regval)
                rtl8363_getAsicRegBit(FIBERPORT,4,11,SERLPAG,0,&regval);
        }
        else
        {
            rtl8363_getAsicRegBit(FIBERPORT,4,7,SERLPAG,0,&regval);
            if (regval)
                rtl8363_getAsicRegBit(FIBERPORT,4,8,SERLPAG,0,&regval);
        }
    }
    return regval;
}

/* Function Name:
 *      setQinQ
 * Description:
 *      set QinQ ability
 * Input:
 *      cfg  - QinQ configuration
 * Output:
 *       none
 * Return:
 * Note:
 *
 */
void setQinQ(qinq_t * cfg)
{
    uint16 regval;

    if (cfg->flag & QINQ_TYPE)
    {
        rtl8363_setAsicReg(1, 17, MACPAG, 11, cfg->type);
    }
    if (cfg->flag & QINQ_VID)
    {
        rtl8363_setAsicReg(1, 18, MACPAG, 11, cfg->vid);
    }

    if ((cfg->flag & QINQ_ENABLE) || (cfg->flag & QINQ_PRI))
    {
        rtl8363_getAsicReg(1, 19, MACPAG, 11, &regval);
        if (cfg->flag & QINQ_ENABLE)
        {
            if (cfg->enable)
            {
                if (cfg->dir)
                    regval = (regval & 0xF0) | 0xB;
                else
                    regval = (regval & 0xF0) | 0x7;
                /*Wen enable qinq, enable flow control of utp and fiber.*/
                setFlowcontrol(FIBERPORT,1);
                setFlowcontrol(UTPPORT,1);
                if (asicid==RTL8203M)
                {
                /*disable en64ipg*/
                    rtl8363_setAsicRegBit(1, 29, 2, MACPAG, 10, 0);
                    rtl8363_setAsicRegBit(1, 29, 3, MACPAG, 10, 0);
                    rtl8363_setAsicRegBit(1, 29, 4, MACPAG, 10, 0);
            }

            }
            else
            {
                if (cfg->dir)
                    regval = (regval & 0xF0) | 0x8;
                else
                    regval = (regval & 0xF0) | 0x4;
            }
        }

        if (cfg->flag & QINQ_PRI)
        {
            if (cfg->pri)
                regval = regval | 0x30;
            else
                regval = regval & 0xF;
        }
        rtl8363_setAsicReg(1, 19, MACPAG, 11, regval);
    }
}

#ifdef _WEBSMART_

/* Function Name:
 *      getQinQ
 * Description:
 *      get QinQ ability
 * Input:
 *      cfg  - QinQ configuration
 * Output:
 *       none
 * Return:
 * Note:
 *
 */
void getQinQ(qinq_t * cfg)
{
    uint16 regval;
    rtl8363_getAsicReg(1, 19, MACPAG, 11, &regval);
    if ((regval & 0x3) == 0x3)
        cfg->enable = TRUE;
    else
        cfg->enable = FALSE;

    if ((regval & 0xC) == 0x4)
    {
        cfg->dir = 0;
    }
    else
        cfg->dir = 1;

    if ((regval & 0x30) == 0x30)
        cfg->pri = 1;
    else
        cfg->pri = 0;

    rtl8363_getAsicReg(1, 17, MACPAG, 11, &cfg->type);
    rtl8363_getAsicReg(1, 18, MACPAG, 11, &cfg->vid);

}
#endif

#if _USE_DIP_

/* Function Name:
 *      getDIPvalue
 * Description:
 *      get DIP value
 * Input:
 *      none
 * Output:
 *       dipval
 * Return:
 * Note:
 *     GPIO0 means input, GPIO2~4 means address select
 *     bit0 means on, bit1 means off, this feature must supported by demo board
 */
void getDIPvalue(uint8 * dipval)
{
    uint8 i;
    //set GPIO 2~4 as output pin, GPIO0 as input pin
    setReg(12, 16, 0x1C);
    *dipval = 0;
    for (i = 0; i < 8; i++)
    {
        setReg(12, 18, i << 2);
        *dipval = (*dipval << 1) | (getReg(12, 18) & 0x1);
    }
}

/* Function Name:
 *      setAsicByDIP
 * Description:
 *      configure asic according DIP value
 * Input:
 *      none
 * Output:
 *       dipval
 * Return:
 * Note:
 */
void setAsicByDIP(uint8 dipval, uint8 flag)
{
    //bit 1 : UTP Nway , on-- disable Nway
    if (flag & 0x1)
        rtl8363_setAsicRegBit(0, 0, 12, UTPPAG, 0, (dipval & 0x1) ? 1 : 0);

    //bit 2 : UPT duplex, on -- half
    if (flag & 0x2)             //Nway is disabled
    {
        rtl8363_setAsicRegBit(0, 0, 8, UTPPAG, 0, (dipval & 0x2) ? 1 : 0);
    }
    //bit3-4: speed.  bit[4:3] 10--10M, 01--100M, 11--1000M
    if (flag & 0xC)
    {

        if (((dipval >> 2) & 0x3) == 3)
        {
            rtl8363_setAsicRegBit(0, 0, 6, UTPPAG, 0, 1);
            rtl8363_setAsicRegBit(0, 0, 13, UTPPAG, 0, 0);
        }
        else if (((dipval >> 2) & 0x3) == 2)
        {
            rtl8363_setAsicRegBit(0, 0, 6, UTPPAG, 0, 0);
            rtl8363_setAsicRegBit(0, 0, 13, UTPPAG, 0, 0);
        }
        else if (((dipval >> 2) & 0x3) == 1)
        {
            rtl8363_setAsicRegBit(0, 0, 6, UTPPAG, 0, 0);
            rtl8363_setAsicRegBit(0, 0, 13, UTPPAG, 0, 1);
        }
    }
    //bit5 LFP 0-- enable
    if (flag & 0x10)
    {
        rtl8363_setAsicRegBit(0, 20, 4, MACPAG, 14, (dipval & 0x10) ? 1 : 0);
        LinkTransEn = TRUE;
    }

    //bit6 fiber Nway  0-- disable
    if (flag & 0x20)
      /****************************************************************************************************************/
        ;

    //bit7 flow control  0-- enable
    if (flag & 0x40)
    {
        rtl8363_setAsicRegBit(0, 4, 10, UTPPAG, 0, flag & 0x40);
    }

    //bit8 FEF  0--enable
    if (flag & 0x80)
        //rtl8363_setAsicRegBit(0, 20, 3, MACPAG, 14, (dipval&0x10)?0:1);
        ;
   /***********************************************************/
}

/* Function Name:
 *      dispDipConf
 * Description:
 *      display DIP setting
 * Input:
 *      dipval - DIP value
 * Output:
 *      none
 * Return:
 *      none
 * Note:
 */
void dispDipConf(uint8 dipval)
{
    printf("Bit 1: UTP Nway:\t%s\n", (dipval & 0x1) ? "ENABLE" : "DISABLE");
    printf("Bit 2: UTP Duplex:\t%s\n", (dipval & 0x2) ? "FULL" : "HALF");
    printf("Bit 3-4: UTP Speed:\t%s\n",
           (dipval & 0x4) ? ((dipval & 0x8) ? "1000M" : "100M") : "10M");
    printf("Bit 5: LFP:\t\t%s\n", (dipval & 0x10) ? "DISABLE" : "ENABLE");
    printf("Bit 6: Serdes Nway:\t%s\n", (dipval & 0x20) ? "ENABLE" : "DISABLE");
    printf("Bit 7: Flow control:\t%s\n",
           (dipval & 0x40) ? "DISABLE" : "ENABLE");
    printf("Bit 8: FEFI:\t\t%s\n", (dipval & 0x80) ? "DISABLE" : "ENABLE");
}
#endif

/* Function Name:
 *      userDefOAM
 * Description:
 *      control remote device according ts1k user defined oam
 * Input:
 *      pbuf - command word
 *      type - command type
 * Output:
 *      none
 * Return:
 *      none
 * Note:
 *      local must be center mode, remote must be terminal mode
 *      command type could be
 *      TS1K_UPDATE_MAC             - change remote mac address
 *      TS1K_UPDATE_IP                - change remote ip
 *      TS1K_READ_MAC                -  read remote mac
 *      TS1K_READ_IP                   -  read remote ip
 *      TS1K_EN_802DOT3AH         -  enable remote ieee802.3ah oam
 *      TS1K_DIS_802DOT3AH        -  disable remote ieee802.3ah oam
 *      TS1K_SET_PHY                  -  set phy
 *      TS1K_RESTORE_FACTORY    -  restore factory default configuration
 *      TS1K_SAVECON                  -  save remote configuration to flash
 *      TS1K_PORTLOOPLOCK          -  set remote port in loop/lock state
 *      TS1K_CFGMIB                     -  configure MIB counter
 *      TS1K_CFGJUMBO                 -  configure jumbo
 *      TS1K_RESET                       -  reset remote device
 *
 */
int8 userDefOAM(uint8 * pbuf, uint8 type)
{
    uint8 xdata usrOamM[6], usrRxOamFlag, i;
    uint16 xdata usrOamC, usrOamS, j;
    Ts1kCfgPara_t xdata cfg;
    uint16 xdata regval;

    rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);
    if ((!cfg.EnTS1K) || (!cfg.MCIdnt))
    {
        printf
            ("\nRemote Config Fail.Should Enable TS1000 and Set It as Center\n");
        return FAILED;
    }
    if (type == TS1K_UPDATE_MAC)
    {
        for (i = 0; i < 3; i++)
        {
            usrOamM[0] = 0x00;
            usrOamM[1] = 0xE0;
            usrOamM[2] = 0x4C;
            usrOamM[3] = pbuf[i * 2 + 1];
            usrOamM[4] = pbuf[i * 2];
            usrOamM[5] = i;
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x292, 0x0, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            for (j = 0; j < 1000; j++)
            {
                rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag,
                                                 &usrOamC, &usrOamS, usrOamM);
                if (usrRxOamFlag)
                    break;
            }
            if (j == 1000)
                return FAILED;
            if ((usrOamC & 0x1) ||
                (((usrOamC & 0xC) >> 2) != 0) ||
                ((((usrOamC & 0xF0) >> 4) != 9)) ||
                ((((usrOamC & 0x300) >> 8) != 0x3)) ||
                (usrOamM[5] != i) ||
                (usrOamM[4] != pbuf[i * 2]) || (usrOamM[3] != pbuf[i * 2 + 1]))
            {
                printf("\nRemote Update Mac Fail.\n");
                return FAILED;
            }
        }
        return SUCCESS;
    }
    else if (type == TS1K_UPDATE_IP)
    {
        for (i = 0; i < 2; i++)
        {
            usrOamM[0] = 0x00;
            usrOamM[1] = 0xE0;
            usrOamM[2] = 0x4C;
            usrOamM[3] = pbuf[i * 2 + 1];
            usrOamM[4] = pbuf[i * 2];
            usrOamM[5] = i;
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x282, 0x0, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            for (j = 0; j < 1000; j++)
            {
                rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag,
                                                 &usrOamC, &usrOamS, usrOamM);
                if (usrRxOamFlag)
                    break;
            }
            if (j == 1000)
                return FAILED;
            if ((usrOamC & 0x1) ||
                (((usrOamC & 0xC) >> 2) != 0) ||
                ((((usrOamC & 0xF0) >> 4) != 8)) ||
                ((((usrOamC & 0x300) >> 8) != 0x3)) ||
                (usrOamM[5] != i) ||
                (usrOamM[4] != pbuf[i * 2]) || (usrOamM[3] != pbuf[i * 2 + 1]))
            {
                printf("\nRemote Update IP Fail.\n");
                return FAILED;
            }
        }
        return SUCCESS;
    }
    else if (type == TS1K_READ_MAC)
    {
        for (i = 0; i < 3; i++)
        {
            usrOamM[0] = 0x00;
            usrOamM[1] = 0xE0;
            usrOamM[2] = 0x4C;
            usrOamM[5] = i;
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x092, 0x0, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            for (j = 0; j < 1000; j++)
            {
                rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag,
                                                 &usrOamC, &usrOamS, usrOamM);
                if (usrRxOamFlag)
                    break;
            }
            if (j == 1000)
                return FAILED;
            if ((usrOamC & 0x1) ||
                (((usrOamC & 0xC) >> 2) != 0) ||
                (((usrOamC & 0xF0) >> 4) != 9) ||
                (((usrOamC & 0x300) >> 8) != 0x1) || (usrOamM[5] != i))
            {
                printf("\nRemote Read Mac Fail.\n");
                return FAILED;
            }
            pbuf[i * 2] = usrOamM[4];
            pbuf[i * 2 + 1] = usrOamM[3];
        }
        return SUCCESS;
    }
    else if (type == TS1K_READ_IP)
    {
        for (i = 0; i < 2; i++)
        {
            usrOamM[0] = 0x00;
            usrOamM[1] = 0xE0;
            usrOamM[2] = 0x4C;
            usrOamM[5] = i;
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x082, 0x0, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            for (j = 0; j < 1000; j++)
            {
                rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag,
                                                 &usrOamC, &usrOamS, usrOamM);
                if (usrRxOamFlag)
                    break;
            }
            if (j == 1000)
                return FAILED;
            if ((usrOamC & 0x1) ||
                (((usrOamC & 0xC) >> 2) != 0) ||
                ((((usrOamC & 0xF0) >> 4) != 8)) ||
                ((((usrOamC & 0x300) >> 8) != 0x1)) || (usrOamM[5] != i))
            {
                printf("\nRemote Read IP Fail.\n");
                return FAILED;
            }
            pbuf[i * 2] = usrOamM[4];
            pbuf[i * 2 + 1] = usrOamM[3];
        }
        return SUCCESS;
    }
    else if ((type == TS1K_EN_802DOT3AH) || (type == TS1K_DIS_802DOT3AH))
    {
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[5] = (type == TS1K_EN_802DOT3AH) ? 0x80 : 0;
        rtl8363_setAsicTs1kOAMTxUsrFrame(0x2A2, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;
        printf("\nRemote 802.3ah function is %s\n",
               (type == TS1K_EN_802DOT3AH) ? "enabled" : "disabled");
        return SUCCESS;
    }

    if (type == TS1K_SET_PHY)
    {

        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = pbuf[0];   /*port number */
        usrOamM[4] = pbuf[1];   /*workmode */
        rtl8363_setAsicTs1kOAMTxUsrFrame(0x0212, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 4000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 4000)
            return FAILED;
        //printf("\nRemote port %bd speed/duplex/auto/workmode is set\n", pbuf[0]);
        return SUCCESS;

    }

    if (type == TS1K_RESTORE_FACTORY)
    {
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        rtl8363_setAsicTs1kOAMTxUsrFrame(0x0612, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 4000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 4000)
            return FAILED;
        return SUCCESS;
    }

    if (type == TS1K_SAVECON)
    {

        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x0A12, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 4000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 4000)
            return FAILED;
        return SUCCESS;
    }

    if (type == TS1K_PORTLOOPLOCK)
    {
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = pbuf[0];   /*port 0 or 1 */
        usrOamM[4] = pbuf[1];   /*Loop 1:loop 0:unloop */
        usrOamM[5] = pbuf[2];   /*Lock 1:lock 0:unlock */

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x0E12, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;
        return SUCCESS;
    }

    if (type == TS1K_CFGMIB)
    {
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = pbuf[0];   /*set MIB 0:relese 1:stop 2:reset */
        usrOamM[4] = 0;
        usrOamM[5] = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x1212, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;

        return SUCCESS;
    }

    if (type == TS1K_CFGJUMBO)
    {
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = pbuf[0];   /*0:disable jumbo 1:enable jumbo */
        usrOamM[4] = 0;
        usrOamM[5] = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x1612, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j != 1000)
        {
            /*configure local jumbo ability */
            rtl8363_getAsicReg(1, 30, MACPAG, 0, &regval);
            if (pbuf[0])
            {
                regval |= 0x8000;
                /*cut through forwarding mode */
                rtl8363_setAsicRegBit(1, 17, 8, MACPAG, 0, 1);
                rtl8363_setAsicRegBit(1, 17, 9, MACPAG, 0, 0);
            }
            else
            {
                regval &= 0x7fff;
                /*normal forwarding mode */
                rtl8363_setAsicRegBit(1, 17, 8, MACPAG, 0, 0);
                rtl8363_setAsicRegBit(1, 17, 9, MACPAG, 0, 0);
            }
            rtl8363_setAsicReg(1, 30, MACPAG, 0, regval);
            mcInfo.local_syscfg.enJumbo = pbuf[0];
            mcInfo.remote_syscfg.enJumbo = pbuf[0];
        }
        if (j == 1000)
            return FAILED;

        return SUCCESS;
    }


    if (type == TS1K_RESET)
    {
        /*remote reset */
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;

        usrOamM[3] = 0;
        usrOamM[4] = 0;
        usrOamM[5] = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x1A12, 0x0, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;

        return SUCCESS;
    }

    if (type == TS1K_CFGSFP)
    {
        /*remote reset */
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        /*pbuf[0] port, pbuf[1] laser_emission_control 1 : enable   0:disable */
        usrOamM[3] = (pbuf[0] ? 0x1 : 0) | (pbuf[1] ? 0x2 : 0);
        /*pbuf[2~5] int32 fiber_rx_threshold */
        usrOamM[4] = pbuf[2];
        usrOamM[5] = pbuf[3];
        usrOamS = (pbuf[4] << 8) | (pbuf[5]);

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x1E12, usrOamS, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;

        return SUCCESS;
    }

    if (type == TS1K_GETFLC)
    {
        /*get remote port flow control ability */
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = 0;
        usrOamM[4] = 0;
        usrOamM[5] = 0;
        usrOamS = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x2012, usrOamS, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;
        if (usrOamC == 0x2110)
        {
            pbuf[0] = usrOamM[3];       //port 0
            pbuf[1] = usrOamM[4];       //port 1
        }
        else
            return FAILED;

        return SUCCESS;
    }

    if (type == TS1K_SETFLC)
    {
        /*set remote port flow control ability */
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = pbuf[0];
        usrOamM[4] = pbuf[1];
        usrOamM[5] = 0;
        usrOamS = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x2212, usrOamS, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;

        return SUCCESS;
    }

    if (type == TS1K_SETRFD)
    {
        /*enable/disable remote link transparent ability */
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = pbuf[0];
        usrOamM[4] = 0;
        usrOamM[5] = 0;
        usrOamS = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0x2612, usrOamS, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;

        return SUCCESS;
    }

    if (type == TS1K_SFTVER)
    {
        //get remote software version number
        usrOamM[0] = 0x00;
        usrOamM[1] = 0xE0;
        usrOamM[2] = 0x4C;
        usrOamM[3] = 0;
        usrOamM[4] = 0;
        usrOamM[5] = 0;
        usrOamS = 0;

        rtl8363_setAsicTs1kOAMTxUsrFrame(0xB2, usrOamS, usrOamM);
        rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
        for (j = 0; j < 1000; j++)
        {
            rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                             &usrOamS, usrOamM);
            if (usrRxOamFlag)
                break;
        }
        if (j == 1000)
            return FAILED;
        if (usrOamC == 0x1B0)
        {
            pbuf[0] = usrOamM[3];
            pbuf[1] = usrOamM[4];
            pbuf[2] = usrOamM[5];
        }
        else
            return FAILED;

        return SUCCESS;
    }

    return FAILED;

}


/* Function Name:
 *      restoreFactoryDef
 * Description:
 *      restore the configuration with factory default value
 * Input:
 *       none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
int8 restoreFactoryDef()
{

#ifdef _DOT3AHOAM_
    rtl_oamUsrConfigPara_t xdata UsrCfg;
#endif
uint8 xdata i;

   mcInfo.local_syscfg.mac[0] = SYS_MAC0;
   mcInfo.local_syscfg.mac[1] = SYS_MAC1;
   mcInfo.local_syscfg.mac[2] = SYS_MAC2;
   mcInfo.local_syscfg.mac[3] = SYS_MAC3;
   mcInfo.local_syscfg.mac[4] = SYS_MAC4;
   mcInfo.local_syscfg.mac[5] = SYS_MAC5;

    mcInfo.local_syscfg.ip[0] = SYS_IP0;
    mcInfo.local_syscfg.ip[1] = SYS_IP1;
    mcInfo.local_syscfg.ip[2] = SYS_IP2;
    mcInfo.local_syscfg.ip[3] = SYS_IP3;

    mcInfo.local_syscfg.gateway[0] = SYS_GATEWAY0;
    mcInfo.local_syscfg.gateway[1] = SYS_GATEWAY1;
    mcInfo.local_syscfg.gateway[2] = SYS_GATEWAY2;
    mcInfo.local_syscfg.gateway[3] = SYS_GATEWAY3;

    mcInfo.local_syscfg.netmask[0] = SYS_NETMASK0;
    mcInfo.local_syscfg.netmask[1] = SYS_NETMASK1;
    mcInfo.local_syscfg.netmask[2] = SYS_NETMASK2;
    mcInfo.local_syscfg.netmask[3] = SYS_NETMASK3;


    //set NIC mac address
    setReg(21, 12, mcInfo.local_syscfg.mac[0]);
    setReg(21, 13, mcInfo.local_syscfg.mac[1]);
    setReg(21, 14, mcInfo.local_syscfg.mac[2]);
    setReg(21, 15, mcInfo.local_syscfg.mac[3]);
    setReg(21, 16, mcInfo.local_syscfg.mac[4]);
    setReg(21, 17, mcInfo.local_syscfg.mac[5]);


    mcInfo.local_syscfg.enDot3ah = SYS_ENDOT3AH;
    //mcInfo.local_syscfg.enTS1000 = SYS_ENTS1000;
    mcInfo.local_syscfg.mcMode = SYS_MCMODE;
    mcInfo.local_syscfg.enSuperRfd = SYS_ENSUPERRFD;
    mcInfo.local_syscfg.type = SYS_TYPE;
    mcInfo.local_syscfg.ver[0] = SYS_VER0;
    mcInfo.local_syscfg.ver[1] = SYS_VER1;
    mcInfo.local_syscfg.ver[2] = SYS_VER2;
    mcInfo.local_syscfg.isMibStop = SYS_ISMIBSTOP;

    mcInfo.local_syscfg.enQos = SYS_ENQOS;
    mcInfo.local_syscfg.enLinktransparent = SYS_ENLINKTRAS;
    mcInfo.local_syscfg.enJumbo = SYS_ENJUMBO;
    mcInfo.local_syscfg.enrfd = SYS_ENRFD;
    mcInfo.local_syscfg.enllcf = SYS_ENLLCF;
    mcInfo.local_syscfg.enfcrcerror = SYS_ENFCRCERROR;
    mcInfo.local_syscfg.enfpause = SYS_ENFPAUSE;
    mcInfo.local_syscfg.bsfilter = SYS_BSFILTER;
    mcInfo.local_syscfg.msfilter = SYS_MSFILTER;
    mcInfo.local_syscfg.undafilter = SYS_UNDAFILTER;



    mcInfo.local_Portcfg[0].loop = 0;
    mcInfo.local_Portcfg[0].lockstate = 0;
    mcInfo.local_Portcfg[0].workmode = 0;
    mcInfo.local_Portcfg[0].disFlowControl = 0;
    mcInfo.local_Portcfg[0].ingressrate = 0;
    mcInfo.local_Portcfg[0].egressrate = 0;

    mcInfo.local_Portcfg[1].loop = 0;
    mcInfo.local_Portcfg[1].lockstate = 0;
    mcInfo.local_Portcfg[1].disFlowControl = 0;
    mcInfo.local_Portcfg[1].workmode = 0;
    mcInfo.local_Portcfg[1].ingressrate = 0;
    mcInfo.local_Portcfg[1].egressrate = 0;

    memset(mcInfo.username,0x0,sizeof(mcInfo.username));
    mcInfo.username[0]='a';
    mcInfo.username[1]='d';
    mcInfo.username[2]='m';
    mcInfo.username[3]='i';
    mcInfo.username[4]='n';

    memset(mcInfo.password,0x0,sizeof(mcInfo.password));
    mcInfo.password[0]='1';
    mcInfo.password[1]='2';
    mcInfo.password[2]='3';
    mcInfo.password[3]='4';
    mcInfo.password[4]='5';
    mcInfo.password[5]='6';
    #ifdef _SNMP_
    memset(mcInfo.read_community,0x0,MAX_READ_COMMUINTY_NUMBER*COMMUNITY_LEN);
    memset(mcInfo.write_community,0x0,MAX_WRITE_COMMUINTY_NUMBER*COMMUNITY_LEN);
    mcInfo.read_community[0][0]='p';
    mcInfo.read_community[0][1]='u';
    mcInfo.read_community[0][2]='b';
    mcInfo.read_community[0][3]='l';
    mcInfo.read_community[0][4]='i';
    mcInfo.read_community[0][5]='c';

    mcInfo.write_community[0][0]='p';
    mcInfo.write_community[0][1]='r';
    mcInfo.write_community[0][2]='i';
    mcInfo.write_community[0][3]='v';
    mcInfo.write_community[0][4]='a';
    mcInfo.write_community[0][5]='t';
    mcInfo.write_community[0][5]='e';

    mcInfo.read_community_number = 1;
    mcInfo.write_community_number = 1;

    memset(&mcInfo.trap,0x0,sizeof(mcInfo.trap));

    #endif
    memset(mcInfo.devicename,0x0,sizeof(mcInfo.devicename));
    strcpy(mcInfo.devicename, "RTL8213M_DEMO");
    if (asicid == RTL8203M)
        strcpy(mcInfo.devicename, "RTL8203M_DEMO");

    mcInfo.dhcpEn = 0;
    mcInfo.snmpEn = 0;

    memset(&mcInfo.local_oamcfg,0x0,sizeof(mcInfo.local_oamcfg));
    mcInfo.local_oamcfg.en_oam = 1;
    mcInfo.local_oamcfg.en_lb = 1;
    mcInfo.local_oamcfg.en_lkevt = 1;
    mcInfo.local_oamcfg.timer_errfrm[1] = 0x2;
    mcInfo.local_oamcfg.errfrm_thr[3] = 0x1;
    mcInfo.local_oamcfg.errfrmprd_win[1] = 0x02;
    mcInfo.local_oamcfg.errfrmprd_win[2] = 0x45;
    mcInfo.local_oamcfg.errfrmprd_win[3] = 0x40;
    mcInfo.local_oamcfg.errfrmprd_thr[3] = 0x05;
    mcInfo.local_oamcfg.timer_errfrmsec[1] = 0x0a;
    mcInfo.local_oamcfg.errfrmsec_thr[1] = 0x05;

    memset(&mcInfo.vlan_group,0x0,sizeof(mcInfo.vlan_group));
    mcInfo.vlan_group.vlan_mode = 0;
    for (i=0;i<16;i++)
        mcInfo.vlan_group.vlan_id[i] = i;
    mcInfo.vlan_group.vlan_member[0] = 5;
    mcInfo.vlan_group.vlan_member[1] = 3;
    mcInfo.vlan_group.vlan_member[2] = 7;
    mcInfo.vlan_group.vlan_member[3] = 5;
    mcInfo.vlan_group.vlan_member[4] = 3;
    mcInfo.vlan_group.vlan_member[5] = 7;
    mcInfo.vlan_group.vlan_member[6] = 5;
    mcInfo.vlan_group.vlan_member[7] = 3;
    mcInfo.vlan_group.vlan_member[8] = 7;
    mcInfo.vlan_group.vlan_member[9] = 5;
    mcInfo.vlan_group.vlan_member[10] = 3;
    mcInfo.vlan_group.vlan_member[11] = 7;
    mcInfo.vlan_group.vlan_member[12] = 5;
    mcInfo.vlan_group.vlan_member[13] = 3;
    mcInfo.vlan_group.vlan_member[14] = 7;
    mcInfo.vlan_group.vlan_member[15] = 5;

    memset(&mcInfo.vlan_perport,0x0,sizeof(mcInfo.vlan_perport));
    mcInfo.vlan_perport.ingree_filter = 0;
    for (i=0;i<3;i++)
    {
        mcInfo.vlan_perport.engree_linktype[i] = 3;
        mcInfo.vlan_perport.vlan_entry[i] = i;
    }
    /*QINQ*/
    memset(&mcInfo.qinq,0x0,sizeof(mcInfo.qinq));
    mcInfo.qinq.type = 0x8100;
    /*MIRROR*/
    memset(&mcInfo.mirror,0x0,sizeof(mcInfo.mirror));
    mcInfo.mirror.mirport = 3;




    cfgSwitch();

#ifdef _DOT3AHOAM_
    /*update oampdu mac */
    UsrCfg.enoam = TRUE;
    UsrCfg.usrConf_mode = OAM_PASSIVE;  //default set passive mode
    UsrCfg.smac[0] = mcInfo.local_syscfg.mac[0];
    UsrCfg.smac[1] = mcInfo.local_syscfg.mac[1];
    UsrCfg.smac[2] = mcInfo.local_syscfg.mac[2];
    UsrCfg.smac[3] = mcInfo.local_syscfg.mac[3];
    UsrCfg.smac[4] = mcInfo.local_syscfg.mac[4];
    UsrCfg.smac[5] = mcInfo.local_syscfg.mac[5];
    UsrCfg.usrConf_var = FALSE;
    UsrCfg.usrConf_lb = TRUE;
    UsrCfg.usrConf_lkevt = TRUE;
    UsrCfg.usrConf_unidir = FALSE;
    UsrCfg.pduconfig[0] = 0x5;
    UsrCfg.pduconfig[1] = 0xEE;
    UsrCfg.oui[0] = OAM_OUI_B0;
    UsrCfg.oui[1] = OAM_OUI_B1;
    UsrCfg.oui[2] = OAM_OUI_B2;
    UsrCfg.fxportnum = FIBERPORT;

    memset(UsrCfg.errfrm_thr, 0, 4);
    UsrCfg.errfrm_thr[3] = 2;
    UsrCfg.timer_errfrm = OAM_TIMER_EVE_ERRFRM;
    memset(UsrCfg.errfrmprd_thr, 0, 4);
    UsrCfg.errfrmprd_thr[3] = 5;
    UsrCfg.errfrmprd_win[0] = 0x00;
    UsrCfg.errfrmprd_win[1] = 0x02;
    UsrCfg.errfrmprd_win[2] = 0x45;
    UsrCfg.errfrmprd_win[3] = 0x40;

    UsrCfg.timer_errfrmsec = OAM_TIMER_EVE_ERRFRMSEC;
    UsrCfg.errfrmsec_thr[0] = 0;
    UsrCfg.errfrmsec_thr[1] = 5;

    rtl_initOAM(&UsrCfg);
    mcInfo.local_syscfg.enDot3ah = TRUE;


#endif

#ifdef _DHCP_
    memcpy(mc_netif.hwaddr, mcInfo.local_syscfg.mac, 6);
    memcpy((uint8 *) & mc_netif.ip_addr.addr, mcInfo.local_syscfg.ip, 4);
    memcpy((uint8 *) & mc_netif.gw.addr, mcInfo.local_syscfg.gateway, 4);
    memcpy((uint8 *) & mc_netif.netmask.addr, mcInfo.local_syscfg.netmask, 4);
#endif

    return SUCCESS;
}

/* Function Name:
 *      userDefOAMACK
 * Description:
 *      ack ts1k user defined oam frame
 * Input:
 *       none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
int8 userDefOAMACK(void)
{
    uint8 xdata usrOamM[6], xdata usrRxOamFlag;
    uint16 xdata usrOamC, xdata usrOamS;
    phyCfg_t xdata phycfg;
    uint16 xdata regval;
    Ts1kCfgPara_t xdata cfg;
#ifdef _DOT3AHOAM_
    rtl_oamUsrConfigPara_t xdata UsrCfg;
#endif

    rtl8363_getAsicTs1kOAMRxUsrFrame(FIBERPORT, &usrRxOamFlag, &usrOamC,
                                     &usrOamS, usrOamM);
    if ((usrOamM[0] != 0) || (usrOamM[1] != 0xE0) || (usrOamM[2] != 0x4C))
        return FAILED;
    if ((usrOamC & 0xF) != 0x2)
        return FAILED;
    if ((usrOamC & 0x300) == 0x200)     //C9-C8:10 - write request
    {
        if ((usrOamC & 0xF0) == 0x80)   // C7-C4:1000 - update IP
        {
            if (usrOamM[5] > 1)
                return FAILED;
            tmpBuf[usrOamM[5] * 2] = usrOamM[4];
            tmpBuf[usrOamM[5] * 2 + 1] = usrOamM[3];
            if (usrOamM[5] == 1)        // get total IP address
            {
                changeIp(tmpBuf);
                /*
                   mcInfo.local_syscfg.ip[0] = tmpBuf[0];
                   mcInfo.local_syscfg.ip[1] = tmpBuf[1];
                   mcInfo.local_syscfg.ip[2] = tmpBuf[2];
                   mcInfo.local_syscfg.ip[3] = tmpBuf[3];
                 */
            }
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x380, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            /*
               if(usrOamM[5] == 1)  // get total IP address
               {
               saveConf();
               }
             */
            return SUCCESS;
        }
        else if ((usrOamC & 0xF0) == 0x90)      //update mac
        {
            if (usrOamM[5] > 2)
                return FAILED;
            tmpBuf[usrOamM[5] * 2] = usrOamM[4];
            tmpBuf[usrOamM[5] * 2 + 1] = usrOamM[3];
            if (usrOamM[5] == 2)        // get total mac address
            {
                changeMac(tmpBuf);
                /*
                   mcInfo.local_syscfg.mac[0] = tmpBuf[0];
                   mcInfo.local_syscfg.mac[1] = tmpBuf[1];
                   mcInfo.local_syscfg.mac[2] = tmpBuf[2];
                   mcInfo.local_syscfg.mac[3] = tmpBuf[3];
                   mcInfo.local_syscfg.mac[4] = tmpBuf[4];
                   mcInfo.local_syscfg.mac[5] = tmpBuf[5];
                 */
                //update NIC mac address
                setReg(21, 12, tmpBuf[0]);
                setReg(21, 13, tmpBuf[1]);
                setReg(21, 14, tmpBuf[2]);
                setReg(21, 15, tmpBuf[3]);
                setReg(21, 16, tmpBuf[4]);
                setReg(21, 17, tmpBuf[5]);

            }
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x390, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            /*
               if(usrOamM[5] == 2)  // get total mac address
               {
               saveConf();
               }
             */
            return SUCCESS;
        }
        else if ((usrOamC & 0xF0) == 0xA0)
        {
#ifdef _DOT3AHOAM_
            if (usrOamM[5] & 0x80)      //enable
            {
                UsrCfg.usrConf_mode = OAM_PASSIVE;
                UsrCfg.enoam = TRUE;
                UsrCfg.smac[0] = mcInfo.local_syscfg.mac[0];
                UsrCfg.smac[1] = mcInfo.local_syscfg.mac[1];
                UsrCfg.smac[2] = mcInfo.local_syscfg.mac[2];
                UsrCfg.smac[3] = mcInfo.local_syscfg.mac[3];
                UsrCfg.smac[4] = mcInfo.local_syscfg.mac[4];
                UsrCfg.smac[5] = mcInfo.local_syscfg.mac[5];
                UsrCfg.usrConf_var = mcInfo.local_oamcfg.en_var;
                UsrCfg.usrConf_lb = mcInfo.local_oamcfg.en_lb;
                UsrCfg.usrConf_lkevt = mcInfo.local_oamcfg.en_lkevt;
                UsrCfg.usrConf_unidir = mcInfo.local_oamcfg.en_unidir;
                UsrCfg.pduconfig[0] = 0x5;
                UsrCfg.pduconfig[1] = 0xEE;
                UsrCfg.oui[0] = OAM_OUI_B0;
                UsrCfg.oui[1] = OAM_OUI_B1;
                UsrCfg.oui[2] = OAM_OUI_B2;
                UsrCfg.fxportnum = FIBERPORT;

                memcpy(UsrCfg.errfrm_thr, mcInfo.local_oamcfg.errfrm_thr, 4);
                memcpy(&UsrCfg.timer_errfrm, mcInfo.local_oamcfg.timer_errfrm,
                       2);
                memcpy(UsrCfg.errfrmprd_win, mcInfo.local_oamcfg.errfrmprd_win,
                       4);
                memcpy(UsrCfg.errfrmprd_thr, mcInfo.local_oamcfg.errfrmprd_thr,
                       4);
                memcpy(&UsrCfg.timer_errfrmsec,
                       mcInfo.local_oamcfg.timer_errfrmsec, 2);
                memcpy(UsrCfg.errfrmsec_thr, mcInfo.local_oamcfg.errfrmsec_thr,
                       2);

                rtl_initOAM(&UsrCfg);
                mcInfo.local_syscfg.enDot3ah = TRUE;

            }
            else
            {
                oamPara.local_oam_enable = FALSE;
            }
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x3A0, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
#endif
            return SUCCESS;
        }

        else if ((usrOamC & 0xFCF0) == 0x0010)
        {
            /*set workmode */
            if (usrOamM[3] > 1)
                return FAILED;
            mcInfo.local_Portcfg[usrOamM[3]].workmode = usrOamM[4];

            if (usrOamM[3] == FIBERPORT)
            {
                /*fiber only could  be nway enable or force 1000MFull */
                rtl8363_setAsicRegBit(FIBERPORT, 0, 12, SERLPAG, 0,
                                      usrOamM[4] ? 0 : 1);
                /*restart nway */
                rtl8363_setAsicRegBit(FIBERPORT, 0, 9, SERLPAG, 0, 1);
                if (asicid == RTL8203M)
                    mcInfo.local_Portcfg[FIBERPORT].speed = 0x1;
                else
                    mcInfo.local_Portcfg[FIBERPORT].speed = 0x2;
                mcInfo.local_Portcfg[FIBERPORT].duplex = 0x1;
                if (usrOamM[4])
                    mcInfo.local_Portcfg[FIBERPORT].workmode = 0;
                else
                {
                    if (asicid == RTL8203M)
                        mcInfo.local_Portcfg[FIBERPORT].workmode = 2;
                    else
                        mcInfo.local_Portcfg[FIBERPORT].workmode = 1;
                }
                //mcInfo.local_Portcfg[FIBERPORT].workmode = usrOamM[4]? 0:1;
            }
            else
            {
                phycfg.FC = TRUE;
                phycfg.AsyFC = FALSE;

                if (usrOamM[4] == 0)
                {
                    /*Auto */
                    phycfg.AutoNegotiation = 1;
                    phycfg.Cap_1000Full = TRUE;
                    phycfg.Cap_100Full = TRUE;
                    phycfg.Cap_100Half = TRUE;
                    phycfg.Cap_10Full = TRUE;
                    phycfg.Cap_10Half = TRUE;
                }
                else if (usrOamM[4] == 1)
                {
                    /*1000Full */
                    phycfg.AutoNegotiation = 0;
                    phycfg.Speed = 0x2;
                    phycfg.Fullduplex = 1;
                    phycfg.Cap_1000Full = TRUE;
                    phycfg.Cap_100Full = TRUE;
                    phycfg.Cap_100Half = TRUE;
                    phycfg.Cap_10Full = TRUE;
                    phycfg.Cap_10Half = TRUE;
                }
                else if (usrOamM[4] == 2)
                {
                    /*100Full */
                    phycfg.AutoNegotiation = 0;
                    phycfg.Speed = 0x1;
                    phycfg.Fullduplex = 1;
                    phycfg.Cap_1000Full = FALSE;
                    phycfg.Cap_100Full = TRUE;
                    phycfg.Cap_100Half = TRUE;
                    phycfg.Cap_10Full = TRUE;
                    phycfg.Cap_10Half = TRUE;
                }
                else if (usrOamM[4] == 3)
                {
                    /*100Half */
                    phycfg.AutoNegotiation = 0;
                    phycfg.Speed = 0x1;
                    phycfg.Fullduplex = 0;
                    phycfg.Cap_1000Full = FALSE;
                    phycfg.Cap_100Full = TRUE;
                    phycfg.Cap_100Half = TRUE;
                    phycfg.Cap_10Full = TRUE;
                    phycfg.Cap_10Half = TRUE;
                }
                else if (usrOamM[4] == 4)
                {
                    /*10Full */
                    phycfg.AutoNegotiation = 0;
                    phycfg.Speed = 0;
                    phycfg.Fullduplex = 1;
                    phycfg.Cap_1000Full = FALSE;
                    phycfg.Cap_100Full = FALSE;
                    phycfg.Cap_100Half = FALSE;
                    phycfg.Cap_10Full = TRUE;
                    phycfg.Cap_10Half = TRUE;
                }
                else if (usrOamM[4] == 5)
                {
                    /*10Half */
                    phycfg.AutoNegotiation = 0;
                    phycfg.Speed = 0;
                    phycfg.Fullduplex = 0;
                    phycfg.Cap_1000Full = FALSE;
                    phycfg.Cap_100Full = FALSE;
                    phycfg.Cap_100Half = FALSE;
                    phycfg.Cap_10Full = FALSE;
                    phycfg.Cap_10Half = TRUE;
                }

                if (rtl8363_setAsicEthernetPHY(UTPPORT, phycfg) != SUCCESS)
                    return FAILED;

            }
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x0310, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);

            return SUCCESS;
        }
        else if ((usrOamC & 0xFCF0) == 0x0410)
        {
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x0710, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);

            /*restore factory default */
            if (restoreFactoryDef() != SUCCESS)
                return FAILED;
            saveConf();

            return SUCCESS;
        }
        else if ((usrOamC & 0xFCF0) == 0x0810)
        {
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x0B10, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            /*save configuration */
            if (saveConf() != SUCCESS)
                return FAILED;

            return SUCCESS;
        }
        else if ((usrOamC & 0xFCF0) == 0x0C10)
        {
            /*set port loop/lock state */
            if (setLocalPortLoopLock(usrOamM[3], usrOamM[4], usrOamM[5]) !=
                SUCCESS)
                return FAILED;
            mcInfo.local_Portcfg[usrOamM[3]].loop = usrOamM[4];
            mcInfo.local_Portcfg[usrOamM[3]].lockstate = usrOamM[5];
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x0F10, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);

            return SUCCESS;
        }

        else if ((usrOamC & 0xFCF0) == 0x1010)
        {
            /*set MIB 0:relese 1:stop 2:reset */
            if (rtl8363_setAsicMIB(RTL8363_ANYPORT, usrOamM[3]) != SUCCESS)
                return FAILED;
            if (usrOamM[3] == 1)
            {
                mcInfo.local_syscfg.isMibStop = TRUE;
            }
            else
            {
                mcInfo.local_syscfg.isMibStop = FALSE;
            }
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x1310, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);

            return SUCCESS;
        }

        else if ((usrOamC & 0xFCF0) == 0x1410)
        {
            /*enable/disable jumbo */
            rtl8363_getAsicReg(1, 30, MACPAG, 0, &regval);
            if (usrOamM[3])
            {
                regval |= 0x8000;
                /*cut through forwarwding mode */
                rtl8363_setAsicRegBit(1, 17, 8, MACPAG, 0, 1);
                rtl8363_setAsicRegBit(1, 17, 9, MACPAG, 0, 0);
            }
            else
            {
                regval &= 0x7fff;
                /*normal forwarwding mode */
                rtl8363_setAsicRegBit(1, 17, 8, MACPAG, 0, 0);
                rtl8363_setAsicRegBit(1, 17, 9, MACPAG, 0, 0);
            }
            rtl8363_setAsicReg(1, 30, MACPAG, 0, regval);
            mcInfo.local_syscfg.enJumbo = usrOamM[3];
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x1710, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            return SUCCESS;
        }
        else if ((usrOamC & 0xFCF0) == 0x1810)  //receives UserDefOAM frame from Center for remote reset
        {
            rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &cfg);
            if (cfg.MCIdnt)     //if local side is also Center,it will not reset
            {
                return FAILED;
            }
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x1B10, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            /*then reset hardware */
            rtl8363_setAsicRegBit(0, 16, 10, MACPAG, 0, 1);
            return SUCCESS;
        }
        else if ((usrOamC & 0xFCF0) == 0x1C10)
        {
            if (usrOamM[3] & 0x1)
            {
                mcInfo.local_sfp[1].laser_emission_control =
                    (usrOamM[3] & 0x2) ? 1 : 0;
                mcInfo.local_sfp[1].fiber_rx_threshold =
                    (int32) (((uint32) usrOamM[4]) << 24 |
                             (((uint32) usrOamM[5]) << 16) | usrOamS);
            }
            else
            {
                mcInfo.local_sfp[0].laser_emission_control =
                    (usrOamM[3] & 0x2) ? 1 : 0;
                mcInfo.local_sfp[0].fiber_rx_threshold =
                    (int32) (((uint32) usrOamM[4]) << 24 |
                             (((uint32) usrOamM[5]) << 16) | usrOamS);
            }

            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x1F10, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            return SUCCESS;
        }
        else if (usrOamC == 0x2212)
        {
            setFlowcontrol(UTPPORT,usrOamM[3 + UTPPORT]);
            setFlowcontrol(FIBERPORT,usrOamM[3 + FIBERPORT]);

            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x2310, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            /*restart nway */
            rtl8363_setAsicRegBit(UTPPORT, 0, 9, UTPPAG, 0, 1);
            rtl8363_setAsicRegBit(FIBERPORT, 0, 9, SERLPAG, 0, 1);
            return SUCCESS;
        }
        else if (usrOamC == 0x2612)
        {
            /*write response */
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x2710, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            /*set link transparence ability */
            setLinkTransparent(TRUE, usrOamM[3]);
        }

        return FAILED;
    }
    else if ((usrOamC & 0x300) == 0x0)  // read request
    {
        if ((usrOamC & 0xF0) == 0x80)   // get IP
        {
            if (usrOamM[5] < 2)
            {
                usrOamM[4] = mcInfo.local_syscfg.ip[usrOamM[5] * 2];
                usrOamM[3] = mcInfo.local_syscfg.ip[usrOamM[5] * 2 + 1];
            }
            else
                return FAILED;
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x180, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            return SUCCESS;
        }
        else if ((usrOamC & 0xF0) == 0x90)      //get mac
        {
            if (usrOamM[5] < 3)
            {
                usrOamM[4] = mcInfo.local_syscfg.mac[usrOamM[5] * 2];
                usrOamM[3] = mcInfo.local_syscfg.mac[usrOamM[5] * 2 + 1];
            }
            else
                return FAILED;
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x190, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            return SUCCESS;
        }
        else if (usrOamC == 0x2012)     // get flow control ablity
        {
            getFlowContorlAbility(&usrOamM[3]);

            rtl8363_setAsicTs1kOAMTxUsrFrame(0x2110, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            return SUCCESS;
        }
        else if (usrOamC == 0xB2)       //get software version
        {
            usrOamM[3] = mcInfo.local_syscfg.ver[0];
            usrOamM[4] = mcInfo.local_syscfg.ver[1];
            usrOamM[5] = mcInfo.local_syscfg.ver[2];
            rtl8363_setAsicTs1kOAMTxUsrFrame(0x1B0, usrOamS, usrOamM);
            rtl8363_setAsicTs1kOAMTrigFrame(FIBERPORT, TriggerUsrOAM);
            return SUCCESS;
        }

        else                    //not support
            return FAILED;
    }
    else                        //bad request
        return FAILED;
}

/* Function Name:
 *      setLinkTransparent
 * Description:
 *      enable or disable link transparent, it's can be set Local or remote ability
 * Input:
 *       isLocal   - local or remote
 *       isEnable - enable or disable
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
int8 setLinkTransparent(uint8 isLocal, uint8 isEnable)
{
    uint16 xdata regval;
    Ts1kCfgPara_t xdata ts1kcfg;

    if (isLocal)                //set local
    {
        rtl8363_getAsicReg(0, 20, MACPAG, 14, &regval);
        if (isEnable)
            regval = (regval & 0xFFE0) | 0x0009;
        else
            regval = (regval & 0xFFE0) | 0x0010;
        printf("\nLocal Link Transparent is %s\n",
               isEnable ? "Enabled" : "Disabled");
        rtl8363_setAsicReg(0, 20, MACPAG, 14, regval);
        mcInfo.local_syscfg.enLinktransparent = isEnable ? 1 : 0;

    }
    else                        //set remote
    {

        rtl8363_getAsicTs1kOAMEnable(FIBERPORT, &ts1kcfg);
        if ((!ts1kcfg.EnTS1K) || (!ts1kcfg.MCIdnt))
        {
            printf
                ("\nRemote Config Fail.Should Enable TS1000 and Set It as Center\n");
            return FAILED;
        }
#if 1
        //set remote link transparent
        rtl8363_setAsicTs1kOAMRemoteRWEnable(FIBERPORT, TRUE);  //add 2008.10.17
//         rtl8363_setAsicTs1kOAMRemoteRWPageSel(FIBERPORT, ISA0_127);
        if (rtl8363_getRemoteAsicReg(0, 20, MACPAG, 14, &regval) != SUCCESS)
        {
            printf("\nRemote Config Fail\n");
            return FAILED;
        }
        if (isEnable)
            regval = (regval & 0xFFE0) | 0x0009;
        else
            regval = (regval & 0xFFE0) | 0x0010;
        if (rtl8363_setRemoteAsicReg(0, 20, MACPAG, 14, regval) != SUCCESS)
        {
            printf("\nRemote Config Fail\n");
            return FAILED;
        }
#else

        if (userDefOAM(&isEnable, TS1K_SETRFD) != SUCCESS)
            return FAILED;
#endif

        mcInfo.remote_syscfg.enLinktransparent = isEnable ? 1 : 0;
    }
    return SUCCESS;
}

/* Function Name:
 *      rtl8363_setRemoteAsicQosPortRate
 * Description:
 *      Set remote asic port bandwidth control
 * Input:
 *       port       - Specify port number(0~2)
 *       direction - input or output port bandwidth control
 *       portRate -  Specify port rate parameter
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *     two MACRO to define direction: RTL8363_PORT_RX means input port bandwidth control, RTL8363_PORT_RX
 *     means output port bandwidth control.
 *     QosPRatePara_t describe port rate parameter
 *     enabled  - Enable or disable rate limit
 *     rate       - Rate of the port in times of 64Kbps(<=0x3FFF, 1000MHz, 0 means 64kbps)
 *
 */
int8 rtl8363_setRemoteAsicQosPortRate(uint8 port, uint8 direction,
                                      QosPRatePara_t portRate)
{
    uint16 xdata regval;

    if ((port > RTL8363_PORT2) || (portRate.rate > RTL8363_MAXRATE)
        || ((direction > RTL8363_PORT_TX)))
        return FAILED;

    rtl8363_setAsicTs1kOAMRemoteRWEnable(FIBERPORT, TRUE);      //add 2008.10.10

    /*register value 0 means 1*64kbps */
    portRate.rate--;

    if (direction == RTL8363_PORT_RX)
    {

        if (!portRate.enabled)
        {
            rtl8363_setRemoteAsicRegBit(port, 29, 15, MACPAG, 2, 1);
            mcInfo.remote_Portcfg[port].ingressrate = 0;
        }
        else
        {
            rtl8363_setRemoteAsicRegBit(port, 29, 15, MACPAG, 2, 0);
            rtl8363_getRemoteAsicReg(port, 29, MACPAG, 2, &regval);
            regval &= ~0x3FFF;
            regval |= portRate.rate & 0x3FFF;
            rtl8363_setRemoteAsicReg(port, 29, MACPAG, 2, regval);
            mcInfo.remote_Portcfg[port].ingressrate = portRate.rate & 0x3FFF;
        }

    }
    else if (direction == RTL8363_PORT_TX)
    {
        if (!portRate.enabled)
        {
            rtl8363_setRemoteAsicRegBit(port, 26, 2, MACPAG, 2, 0);
            mcInfo.remote_Portcfg[port].egressrate = 0;
        }
        else
        {
            rtl8363_setRemoteAsicRegBit(port, 26, 2, MACPAG, 2, 1);
            rtl8363_getRemoteAsicReg(port, 25, MACPAG, 2, &regval);
            regval &= ~0x3FFF;
            regval |= portRate.rate & 0x3FFF;
            rtl8363_setRemoteAsicReg(port, 25, MACPAG, 2, regval);
            mcInfo.remote_Portcfg[port].egressrate = portRate.rate & 0x3FFF;
        }

    }
    return SUCCESS;
}


/* Function Name:
 *      dumpMib
 * Description:
 *      dump mib counter in uart console
 * Input:
 *      none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
void dumpMib()
{
    uint8 MIBaddr;

    code uint8 *MIB32[] =
        { "ifInUcastPkts", "UndersizePkts", "Fragments", "Pkts64",
        "Pkts65to127", "Pkts128to255", "Pkts256to511", "Pkts512to1023",
        "Pkts1024to1518", "OversizePkts", "Jabbers", "MulticastPkts",
        "BroadcastPkts", "DropEvents", "PortInDiscards", "FCSErrors",
        "SymbolErrors", "UnkownOpcodes", "InPauseFrames",
        "ifOutUcast", "ifOutMulticast", "ifOutBroadcast", "SingleColli",
        "MultiColli", "DeferTrans", "LateColli", "ExcessColli",
        "OutPauseFrames", "StatusColli"
    };

    rtl8363_setAsicMIB(RTL8363_ANYPORT, STOP);
    printf("[MIB Counter Info]\n\n");
    printf("%-15s%-15s%-15s%-15s\n", " ", "Port0", "Port1", "Port2");
    for (MIBaddr = 0x13; MIBaddr <= 0x69; MIBaddr++)
    {
        if ((MIBaddr >= 0x13) && (MIBaddr <= 0x25))
        {
            /*MIB in counter */
            rtl8363_getAsicMIB(MIBaddr, tmp);
            printf("%-15s0X%02bX%02bX%02bX%02bX%-5s", MIB32[MIBaddr - 0x13],
                   tmp[0], tmp[1], tmp[2], tmp[3], " ");
            rtl8363_getAsicMIB(0x13 + MIBaddr, tmp);
            printf("0X%02bX%02bX%02bX%02bX%-5s", tmp[0], tmp[1], tmp[2], tmp[3],
                   " ");
            rtl8363_getAsicMIB(0x26 + MIBaddr, tmp);
            printf("0X%02bX%02bX%02bX%02bX%-5s", tmp[0], tmp[1], tmp[2], tmp[3],
                   " ");
            printf("\n");
        }
        else if ((MIBaddr >= 0x4C) && (MIBaddr <= 0X55))
        {
            /*MIB out counter */
            rtl8363_getAsicMIB(MIBaddr, tmp);
            printf("%-15s0X%02bX%02bX%02bX%02bX%-5s",
                   MIB32[MIBaddr - 0x4C + 0x13], tmp[0], tmp[1], tmp[2], tmp[3],
                   " ");
            rtl8363_getAsicMIB(0xA + MIBaddr, tmp);
            printf("0X%02bX%02bX%02bX%02bX%-5s", tmp[0], tmp[1], tmp[2], tmp[3],
                   " ");
            rtl8363_getAsicMIB(0x14 + MIBaddr, tmp);
            printf("0X%02bX%02bX%02bX%02bX%-5s", tmp[0], tmp[1], tmp[2], tmp[3],
                   " ");
            printf("\n");
        }

    }

    if (!mcInfo.local_syscfg.isMibStop)
        rtl8363_setAsicMIB(RTL8363_ANYPORT, RELEASE);

}

#ifdef _DOT3AHOAM_

/* Function Name:
 *      rtl_GetLocalInfo
 * Description:
 *      collect local information
 * Input:
 *      none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
void rtl_GetLocalInfo(void)
{
    uint8 xdata port;
    uint8 xdata buf[2];
    uint16 xdata regval;
    QosPRatePara_t xdata portRate;

    /*if 802.3ah oam is disabled, it is not required to collect local info */
    if (!oamPara.local_oam_enable)
        return;

    /*get local software version */
    sscanf(version, "%bu.%bu.%bu", &mcInfo.local_syscfg.ver[0],
           &mcInfo.local_syscfg.ver[1], &mcInfo.local_syscfg.ver[2]);
#if  _ORGOAMPDU_
    /*get MIB */
    rtl8363_setAsicMIB(RTL8363_ANYPORT, STOP);
    rtl8363_getAsicMIB(0x0, &(mcInfo.local_mib[0].ifInOctets[0]));
    rtl8363_getAsicMIB(0x1, &(mcInfo.local_mib[0].etherStatsOctets[0]));
    rtl8363_getAsicMIB(0x2, &(mcInfo.local_mib[1].ifInOctets[0]));
    rtl8363_getAsicMIB(0x3, &(mcInfo.local_mib[1].etherStatsOctets[0]));
    rtl8363_getAsicMIB(0x4, &(mcInfo.local_mib[2].ifInOctets[0]));
    rtl8363_getAsicMIB(0x5, &(mcInfo.local_mib[2].etherStatsOctets[0]));
    rtl8363_getAsicMIB(0x6, &(mcInfo.local_mib[0].ifOutOctets[0]));
    rtl8363_getAsicMIB(0x7, &(mcInfo.local_mib[1].ifOutOctets[0]));
    rtl8363_getAsicMIB(0x8, &(mcInfo.local_mib[2].ifOutOctets[0]));

    rtl8363_getAsicMIB(0x13, &(mcInfo.local_mib[0].ifInUcastPkts[0]));
    rtl8363_getAsicMIB(0x14, &(mcInfo.local_mib[0].etherStatsUndersizePkts[0]));
    rtl8363_getAsicMIB(0x15, &(mcInfo.local_mib[0].etherStatsFragments[0]));
    rtl8363_getAsicMIB(0x16, &(mcInfo.local_mib[0].EtherStatsPkts64Octets[0]));
    rtl8363_getAsicMIB(0x17,
                       &(mcInfo.local_mib[0].EtherStatsPkts65to127Octets[0]));
    rtl8363_getAsicMIB(0x18,
                       &(mcInfo.local_mib[0].EtherStatsPkts128to255Octets[0]));
    rtl8363_getAsicMIB(0x19,
                       &(mcInfo.local_mib[0].EtherStatsPkts256to511Octets[0]));
    rtl8363_getAsicMIB(0x1A,
                       &(mcInfo.local_mib[0].EtherStatsPkts512to1023Octets[0]));
    rtl8363_getAsicMIB(0x1B,
                       &(mcInfo.local_mib[0].
                         EtherStatsPkts1024to1518Octets[0]));
    rtl8363_getAsicMIB(0x1C, &(mcInfo.local_mib[0].EtherStatsOversizePkts[0]));
    rtl8363_getAsicMIB(0x1D, &(mcInfo.local_mib[0].EtherStatsJabbers[0]));
    rtl8363_getAsicMIB(0x1E, &(mcInfo.local_mib[0].EtherStatsMulticastPkts[0]));
    rtl8363_getAsicMIB(0x1F, &(mcInfo.local_mib[0].EtherStatsBroadcastPkts[0]));
    rtl8363_getAsicMIB(0x20, &(mcInfo.local_mib[0].EtherStatsDropEvents[0]));
    rtl8363_getAsicMIB(0x21, &(mcInfo.local_mib[0].dot1dTpPortInDiscards[0]));
    rtl8363_getAsicMIB(0x22, &(mcInfo.local_mib[0].dot3StatsFCSErrors[0]));
    rtl8363_getAsicMIB(0x23, &(mcInfo.local_mib[0].dot3StatsSymbolErrors[0]));
    rtl8363_getAsicMIB(0x24,
                       &(mcInfo.local_mib[0].dot3ControlInUnknownOpcodes[0]));
    rtl8363_getAsicMIB(0x25, &(mcInfo.local_mib[0].dot3InPauseFrames[0]));

    rtl8363_getAsicMIB(0x26, &(mcInfo.local_mib[1].ifInUcastPkts[0]));
    rtl8363_getAsicMIB(0x27, &(mcInfo.local_mib[1].etherStatsUndersizePkts[0]));
    rtl8363_getAsicMIB(0x28, &(mcInfo.local_mib[1].etherStatsFragments[0]));
    rtl8363_getAsicMIB(0x29, &(mcInfo.local_mib[1].EtherStatsPkts64Octets[0]));
    rtl8363_getAsicMIB(0x2A,
                       &(mcInfo.local_mib[1].EtherStatsPkts65to127Octets[0]));
    rtl8363_getAsicMIB(0x2B,
                       &(mcInfo.local_mib[1].EtherStatsPkts128to255Octets[0]));
    rtl8363_getAsicMIB(0x2C,
                       &(mcInfo.local_mib[1].EtherStatsPkts256to511Octets[0]));
    rtl8363_getAsicMIB(0x2D,
                       &(mcInfo.local_mib[1].EtherStatsPkts512to1023Octets[0]));
    rtl8363_getAsicMIB(0x2E,
                       &(mcInfo.local_mib[1].
                         EtherStatsPkts1024to1518Octets[0]));
    rtl8363_getAsicMIB(0x2F, &(mcInfo.local_mib[1].EtherStatsOversizePkts[0]));
    rtl8363_getAsicMIB(0x30, &(mcInfo.local_mib[1].EtherStatsJabbers[0]));
    rtl8363_getAsicMIB(0x31, &(mcInfo.local_mib[1].EtherStatsMulticastPkts[0]));
    rtl8363_getAsicMIB(0x32, &(mcInfo.local_mib[1].EtherStatsBroadcastPkts[0]));
    rtl8363_getAsicMIB(0x33, &(mcInfo.local_mib[1].EtherStatsDropEvents[0]));
    rtl8363_getAsicMIB(0x34, &(mcInfo.local_mib[1].dot1dTpPortInDiscards[0]));
    rtl8363_getAsicMIB(0x35, &(mcInfo.local_mib[1].dot3StatsFCSErrors[0]));
    rtl8363_getAsicMIB(0x36, &(mcInfo.local_mib[1].dot3StatsSymbolErrors[0]));
    rtl8363_getAsicMIB(0x37,
                       &(mcInfo.local_mib[1].dot3ControlInUnknownOpcodes[0]));
    rtl8363_getAsicMIB(0x38, &(mcInfo.local_mib[1].dot3InPauseFrames[0]));

    rtl8363_getAsicMIB(0x39, &(mcInfo.local_mib[2].ifInUcastPkts[0]));
    rtl8363_getAsicMIB(0x3A, &(mcInfo.local_mib[2].etherStatsUndersizePkts[0]));
    rtl8363_getAsicMIB(0x3B, &(mcInfo.local_mib[2].etherStatsFragments[0]));
    rtl8363_getAsicMIB(0x3C, &(mcInfo.local_mib[2].EtherStatsPkts64Octets[0]));
    rtl8363_getAsicMIB(0x3D,
                       &(mcInfo.local_mib[2].EtherStatsPkts65to127Octets[0]));
    rtl8363_getAsicMIB(0x3E,
                       &(mcInfo.local_mib[2].EtherStatsPkts128to255Octets[0]));
    rtl8363_getAsicMIB(0x3F,
                       &(mcInfo.local_mib[2].EtherStatsPkts256to511Octets[0]));
    rtl8363_getAsicMIB(0x40,
                       &(mcInfo.local_mib[2].EtherStatsPkts512to1023Octets[0]));
    rtl8363_getAsicMIB(0x41,
                       &(mcInfo.local_mib[2].
                         EtherStatsPkts1024to1518Octets[0]));
    rtl8363_getAsicMIB(0x42, &(mcInfo.local_mib[2].EtherStatsOversizePkts[0]));
    rtl8363_getAsicMIB(0x43, &(mcInfo.local_mib[2].EtherStatsJabbers[0]));
    rtl8363_getAsicMIB(0x44, &(mcInfo.local_mib[2].EtherStatsMulticastPkts[0]));
    rtl8363_getAsicMIB(0x45, &(mcInfo.local_mib[2].EtherStatsBroadcastPkts[0]));
    rtl8363_getAsicMIB(0x46, &(mcInfo.local_mib[2].EtherStatsDropEvents[0]));
    rtl8363_getAsicMIB(0x47, &(mcInfo.local_mib[2].dot1dTpPortInDiscards[0]));
    rtl8363_getAsicMIB(0x48, &(mcInfo.local_mib[2].dot3StatsFCSErrors[0]));
    rtl8363_getAsicMIB(0x49, &(mcInfo.local_mib[2].dot3StatsSymbolErrors[0]));
    rtl8363_getAsicMIB(0x4A,
                       &(mcInfo.local_mib[2].dot3ControlInUnknownOpcodes[0]));
    rtl8363_getAsicMIB(0x4B, &(mcInfo.local_mib[2].dot3InPauseFrames[0]));

    rtl8363_getAsicMIB(0x4C, &(mcInfo.local_mib[0].ifOutUcastPkts[0]));
    rtl8363_getAsicMIB(0x4D, &(mcInfo.local_mib[0].ifOutMulticastPkts[0]));
    rtl8363_getAsicMIB(0x4E, &(mcInfo.local_mib[0].ifOutBroadcastPkts[0]));
    rtl8363_getAsicMIB(0x4F,
                       &(mcInfo.local_mib[0].
                         dot3StatsSingleCollisionFrames[0]));
    rtl8363_getAsicMIB(0x50,
                       &(mcInfo.local_mib[0].
                         dot3StatsMultipleCollisionFrames[0]));
    rtl8363_getAsicMIB(0x51,
                       &(mcInfo.local_mib[0].
                         dot3StatsDeferredTransmissions[0]));
    rtl8363_getAsicMIB(0x52, &(mcInfo.local_mib[0].dot3StatsLateCollisions[0]));
    rtl8363_getAsicMIB(0x53,
                       &(mcInfo.local_mib[0].dot3StatsExcessiveCollisions[0]));
    rtl8363_getAsicMIB(0x54, &(mcInfo.local_mib[0].dot3OutPauseFrames[0]));
    rtl8363_getAsicMIB(0x55, &(mcInfo.local_mib[0].etherStatsCollisions[0]));

    rtl8363_getAsicMIB(0x56, &(mcInfo.local_mib[1].ifOutUcastPkts[0]));
    rtl8363_getAsicMIB(0x57, &(mcInfo.local_mib[1].ifOutMulticastPkts[0]));
    rtl8363_getAsicMIB(0x58, &(mcInfo.local_mib[1].ifOutBroadcastPkts[0]));
    rtl8363_getAsicMIB(0x59,
                       &(mcInfo.local_mib[1].
                         dot3StatsSingleCollisionFrames[0]));
    rtl8363_getAsicMIB(0x5A,
                       &(mcInfo.local_mib[1].
                         dot3StatsMultipleCollisionFrames[0]));
    rtl8363_getAsicMIB(0x5B,
                       &(mcInfo.local_mib[1].
                         dot3StatsDeferredTransmissions[0]));
    rtl8363_getAsicMIB(0x5C, &(mcInfo.local_mib[1].dot3StatsLateCollisions[0]));
    rtl8363_getAsicMIB(0x5D,
                       &(mcInfo.local_mib[1].dot3StatsExcessiveCollisions[0]));
    rtl8363_getAsicMIB(0x5E, &(mcInfo.local_mib[1].dot3OutPauseFrames[0]));
    rtl8363_getAsicMIB(0x5F, &(mcInfo.local_mib[1].etherStatsCollisions[0]));

    rtl8363_getAsicMIB(0x60, &(mcInfo.local_mib[2].ifOutUcastPkts[0]));
    rtl8363_getAsicMIB(0x61, &(mcInfo.local_mib[2].ifOutMulticastPkts[0]));
    rtl8363_getAsicMIB(0x62, &(mcInfo.local_mib[2].ifOutBroadcastPkts[0]));
    rtl8363_getAsicMIB(0x63,
                       &(mcInfo.local_mib[2].
                         dot3StatsSingleCollisionFrames[0]));
    rtl8363_getAsicMIB(0x64,
                       &(mcInfo.local_mib[2].
                         dot3StatsMultipleCollisionFrames[0]));
    rtl8363_getAsicMIB(0x65,
                       &(mcInfo.local_mib[2].
                         dot3StatsDeferredTransmissions[0]));
    rtl8363_getAsicMIB(0x66, &(mcInfo.local_mib[2].dot3StatsLateCollisions[0]));
    rtl8363_getAsicMIB(0x67,
                       &(mcInfo.local_mib[2].dot3StatsExcessiveCollisions[0]));
    rtl8363_getAsicMIB(0x68, &(mcInfo.local_mib[2].dot3OutPauseFrames[0]));
    rtl8363_getAsicMIB(0x69, &(mcInfo.local_mib[2].etherStatsCollisions[0]));


    if (!mcInfo.local_syscfg.isMibStop)
    {
        rtl8363_setAsicMIB(RTL8363_ANYPORT, RELEASE);
    }
 #endif
    rtl8363_getAsicRegBit(UTPPORT, 17, 10, UTPPAG, 0, &regval);
    mcInfo.local_Portcfg[UTPPORT].linkstatus = regval ? 1 : 0;
    if (asicid == RTL8203M)
    {
        rtl8363_getAsicRegBit(FIBERPORT, 1, 2, SERLPAG, 0, &regval);
        mcInfo.local_Portcfg[FIBERPORT].linkstatus = regval ? 1 : 0;
    }
    else
    {
        rtl8363_getAsicRegBit(FIBERPORT, 1, 2, SERLPAG, 0, &regval);
        rtl8363_getAsicRegBit(FIBERPORT, 1, 2, SERLPAG, 0, &regval);
        mcInfo.local_Portcfg[FIBERPORT].linkstatus = regval ? 1 : 0;
        rtl8363_getAsicRegBit(FIBERPORT, 6, 14, SERLPAG, 4, &regval);
        if (regval)
            mcInfo.local_Portcfg[FIBERPORT].linkstatus = 0;
    }

    rtl8363_getAsicRegBit(2, 29, 7, MACPAG, 0, &regval);
    mcInfo.local_Portcfg[UTPPORT].loop = regval ? 0 : 1;
    rtl8363_getAsicRegBit(2, 29, 15, MACPAG, 0, &regval);
    mcInfo.local_Portcfg[FIBERPORT].loop = regval ? 0 : 1;

    rtl8363_getAsicReg(UTPPORT, 17, UTPPAG, 0, &regval);
    mcInfo.local_Portcfg[UTPPORT].speed = (regval & (0x3 << 14)) >> 14;
    mcInfo.local_Portcfg[UTPPORT].duplex = (regval & (1 << 13)) ? 1 : 0;
    rtl8363_getAsicRegBit(UTPPORT, 0, 12, UTPPAG, 0, &regval);
    mcInfo.local_Portcfg[UTPPORT].autoneg = regval ? 1 : 0;

    if (asicid == RTL8203M)
    {
        //rtl8363_getAsicRegBit(FIBERPORT, 1, 14, SERLPAG, 0, &regval);
        /*RTL8203 Fiber :100M Half */
        mcInfo.local_Portcfg[FIBERPORT].speed = 0x1;
        mcInfo.local_Portcfg[FIBERPORT].duplex = 0x1;
        rtl8363_getAsicRegBit(FIBERPORT, 0, 12, SERLPAG, 0, &regval);
        mcInfo.local_Portcfg[FIBERPORT].autoneg = regval ? 1 : 0;

    }
    else
    {
        rtl8363_getAsicRegBit(FIBERPORT, 15, 15, SERLPAG, 0, &regval);
        mcInfo.local_Portcfg[FIBERPORT].speed = regval ? 0x2 : 0x1;
        mcInfo.local_Portcfg[FIBERPORT].duplex = 0x1;
        rtl8363_getAsicRegBit(FIBERPORT, 0, 12, SERLPAG, 0, &regval);
        mcInfo.local_Portcfg[FIBERPORT].autoneg = regval ? 1 : 0;

    }

    /*workmode */

    /*link transparent */
    mcInfo.local_syscfg.enDot3ah = oamPara.local_oam_enable;
    rtl8363_getAsicReg(0, 20, MACPAG, 14, &regval);
    mcInfo.local_syscfg.enLinktransparent = (regval & 0x10) ? 0 : 1;

#if _ORGOAMPDU_
    if ((oamPara.local_pdu == OAM_ANY)
        && (oamPara.local_info.conf_mode == OAM_ACTIVE))
    {
        if (mcInfo.local_syscfg.enLinktransparent != mcInfo.remote_syscfg.enRfd)
        {
            setLinkTransparent(FALSE, mcInfo.local_syscfg.enLinktransparent);
        }

    }
#endif

    /*bandwidth */
    for (port = 0; port < 2; port++)
    {
        rtl8363_getAsicQosPortRate(port, RTL8363_PORT_RX, &portRate);
        if (portRate.enabled)
            mcInfo.local_Portcfg[port].ingressrate = portRate.rate;
        else
            mcInfo.local_Portcfg[port].ingressrate = 0;

        rtl8363_getAsicQosPortRate(port, RTL8363_PORT_TX, &portRate);

        if (portRate.enabled)
            mcInfo.local_Portcfg[port].egressrate = portRate.rate;
        else
            mcInfo.local_Portcfg[port].egressrate = 0;
    }

    /*flow control ablity */
    getFlowContorlAbility(buf);
    mcInfo.local_Portcfg[0].disFlowControl = buf[0] ? 0 : 1;
    mcInfo.local_Portcfg[1].disFlowControl = buf[1] ? 0 : 1;

#if 0

    mcInfo.local_syscfg.enDot3ah = oamPara.local_oam_enable;
    rtl8363_getAsicReg(0, 20, MACPAG, 14, &regval);
    mcInfo.local_syscfg.enLinktransparent = (regval & 0x10) ? 0 : 1;

    rtl8363_getAsicReg(1, 30, MACPAG, 0, &regval);
    if ((regval & 0xC000) == 0xC000)
        mcInfo.local_syscfg.enJumbo = 1;
    else
        mcInfo.local_syscfg.enJumbo = 0;

    rtl8363_getAsicReg(0, 17, MACPAG, 0, &regval);
    mcInfo.local_Portcfg[0].lockstate = (regval & 0x2000) ? 0 : 1;
    mcInfo.local_Portcfg[1].lockstate = (regval & 0x4000) ? 0 : 1;

#endif

    /*utp port force link down? */
    rtl8363_getAsicRegBit(FIBERPORT, 22, 1, MACPAG, 6, &regval);
    if ((mcInfo.local_Portcfg[UTPPORT].linkstatus == 0) && (regval == 0))
        mcInfo.local_Portcfg[UTPPORT].isForceLinkDown = 1;
    else
        mcInfo.local_Portcfg[UTPPORT].isForceLinkDown = 0;

    /*reset timer timer_getinfo */
    timerLock();
    timer_getinfo = TIMER_GETINFO;
    timerUnlock();

    return;
}
#endif

/* Function Name:
 *      setLocalPortLoopLock
 * Description:
 *      set local port loop/lock state
 * Input:
 *      port      - port number
 *      enLoop  - enable loop back state
 *      enLock  - enable lock state
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *       if the port is locked, it could not rx/tx pkt any more, and it is still link up
 */
int8 setLocalPortLoopLock(uint8 port, uint8 enLoop, uint8 enLock)
{

    port = port ? 1 : 0;
    enLoop = enLoop ? 1 : 0;
    enLock = enLock ? 1 : 0;

    if (FIBERPORT == port)
    {
        /*set loop */
        rtl8363_setAsicRegBit(2, 29, 15, MACPAG, 0, enLoop ? 0 : 1);
        /*set lock */
        if (enLock)
        {
            rtl8363_setAsicRegBit(0, 17, 14, MACPAG, 0, 0);
            rtl8363_setAsicRegBit(0, 17, 11, MACPAG, 0, 0);
        }
        else
        {
            rtl8363_setAsicRegBit(0, 17, 14, MACPAG, 0, 1);
            rtl8363_setAsicRegBit(0, 17, 11, MACPAG, 0, 1);

        }
    }
    else
    {
        /*set loop */
        rtl8363_setAsicRegBit(2, 29, 7, MACPAG, 0, enLoop ? 0 : 1);

        /*set lock */
        if (enLock)
        {
            rtl8363_setAsicRegBit(0, 17, 13, MACPAG, 0, 0);
            rtl8363_setAsicRegBit(0, 17, 10, MACPAG, 0, 0);
        }
        else
        {
            rtl8363_setAsicRegBit(0, 17, 13, MACPAG, 0, 1);
            rtl8363_setAsicRegBit(0, 17, 10, MACPAG, 0, 1);
        }
    }
    return SUCCESS;
}

/* Function Name:
 *      cfgSwitch
 * Description:
 *      confiure switch according mcInfo
 * Input:
 *       none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
int8 cfgSwitch(void)
{
    uint8 xdata port;
    QosPRatePara_t xdata portRate;
    phyCfg_t xdata phycfg;
    uint16 xdata regval;
    stormPara_t xdata stm;
    uint8 xdata i;
    mirrorPara_t xdata mir;

    /*jumbo setting*/
    if (mcInfo.local_syscfg.enJumbo)
    {
        /*rx len 16KB*/
        rtl8363_getAsicReg(1, 30, MACPAG, 0, &regval);
        regval |= 0xC000;
        rtl8363_setAsicReg(1, 30, MACPAG, 0, regval);
        /*modified cut through mode*/
        rtl8363_setAsicRegBit(1, 17, 8, MACPAG, 0, 1);
        rtl8363_setAsicRegBit(1, 17, 9, MACPAG, 0, 0);
        /*modified cut through mode threshold 1536*/
        rtl8363_setAsicRegBit(1, 17, 12, MACPAG, 0, 1);
        rtl8363_setAsicRegBit(1, 17, 11, MACPAG, 0, 0);

    }
    else
    {
        rtl8363_getAsicReg(1, 30, MACPAG, 0, &regval);
        regval &= 0x3FFF;
        regval |= 0x4000;
        rtl8363_setAsicReg(1, 30, MACPAG, 0, regval);

        rtl8363_setAsicRegBit(1, 17, 8, MACPAG, 0, 0);
        rtl8363_setAsicRegBit(1, 17, 9, MACPAG, 0, 0);
        rtl8363_setAsicRegBit(1, 17, 12, MACPAG, 0, 0);
        rtl8363_setAsicRegBit(1, 17, 11, MACPAG, 0, 0);
    }

    /*
    printf("mcInfo.local_syscfg.enLinktransparent %bd \n",mcInfo.local_syscfg.enLinktransparent);
    printf("mcInfo.local_syscfg.enrfd %bd \n",mcInfo.local_syscfg.enrfd);
    printf("mcInfo.local_syscfg.enllcf %bd \n",mcInfo.local_syscfg.enllcf);
     */
    if (mcInfo.local_syscfg.enLinktransparent)
    {
        rtl8363_setAsicRegBit(0, 20, 4, MACPAG, 14, 0);
        LinkTransEn = TRUE;
    }
    else
    {
        /*Link transparent disable */
        rtl8363_setAsicRegBit(0, 20, 4, MACPAG, 14, 1);
        LinkTransEn = FALSE;
    }

    rtl8363_setAsicRegBit(0, 20, 3, MACPAG, 14, mcInfo.local_syscfg.enrfd);
    rtl8363_setAsicRegBit(0, 20, 0, MACPAG, 14, mcInfo.local_syscfg.enllcf);

    rtl8363_setAsicRegBit(1, 17, 13, MACPAG, 0,
                          mcInfo.local_syscfg.enfcrcerror);

    rtl8363_getAsicReg(1, 18, MACPAG, 0, &regval);
    if (!mcInfo.local_syscfg.enfpause)
    {
        rtl8363_setAsicRegBit(1, 17, 10, MACPAG, 0, 0);
        regval |= 0x3000;
    }
    else
    {
        rtl8363_setAsicRegBit(1, 17, 10, MACPAG, 0, 1);
        regval &= 0xCFFF;
    }
    rtl8363_setAsicReg(1, 18, MACPAG, 0, regval);

    stm.enBRD = mcInfo.local_syscfg.bsfilter;
    stm.enMUL = mcInfo.local_syscfg.msfilter;
    stm.enUDA = mcInfo.local_syscfg.undafilter;
    rtl8363_setAsicStormFilter(stm);


    for (port = 0; port < 2; port++)
    {

        /*set loop */
        if (FIBERPORT == port)
            rtl8363_setAsicRegBit(2, 29, 15, MACPAG, 0,
                                  mcInfo.local_Portcfg[port].loop ? 0 : 1);
        else
            rtl8363_setAsicRegBit(2, 29, 7, MACPAG, 0,
                                  mcInfo.local_Portcfg[port].loop ? 0 : 1);

        /*set lock state */
        if (mcInfo.local_Portcfg[port].lockstate)
        {
            rtl8363_setAsicRegBit(0, 17, 13 + port, MACPAG, 0, 0);
            rtl8363_setAsicRegBit(0, 17, 10 + port, MACPAG, 0, 0);
        }
        else
        {
            rtl8363_setAsicRegBit(0, 17, 13 + port, MACPAG, 0, 1);
            rtl8363_setAsicRegBit(0, 17, 10 + port, MACPAG, 0, 1);
        }

        /*set workmode */
        if (port == FIBERPORT)
        {
            /*always enable fiber nway */
            rtl8363_setAsicRegBit(FIBERPORT, 0, 12, SERLPAG, 0,
                                  1);

            /*set the flow control according to setting */

            if (mcInfo.local_Portcfg[port].disFlowControl)
            {
                setFlowcontrol(FIBERPORT,0);

            }
            else
            {
               setFlowcontrol(FIBERPORT,1);

            }

            /*restart nway */
            rtl8363_setAsicRegBit(FIBERPORT, 0, 9, SERLPAG, 0, 1);
        }
        else
        {

            phycfg.FC = (!mcInfo.local_Portcfg[port].disFlowControl);
            phycfg.AsyFC = (!mcInfo.local_Portcfg[port].disFlowControl);

            /*
            printf("mcInfo.local_Portcfg[port].disFlowControl %bd \n",mcInfo.local_Portcfg[port].disFlowControl);
            printf("mcInfo.local_Portcfg[port].workmode %bd \n",mcInfo.local_Portcfg[port].workmode);
              */
            if (mcInfo.local_Portcfg[port].workmode == 0)
            {
                /*Auto */
                phycfg.AutoNegotiation = 1;
                phycfg.Cap_1000Full = TRUE;
                phycfg.Cap_100Full = TRUE;
                phycfg.Cap_100Half = TRUE;
                phycfg.Cap_10Full = TRUE;
                phycfg.Cap_10Half = TRUE;
            }
            else if (mcInfo.local_Portcfg[port].workmode == 1)
            {
                /*1000Full */
                phycfg.AutoNegotiation = 0;
                phycfg.Speed = 0x2;
                phycfg.Fullduplex = 1;
                phycfg.Cap_1000Full = FALSE;
                phycfg.Cap_100Full = TRUE;
                phycfg.Cap_100Half = TRUE;
                phycfg.Cap_10Full = TRUE;
                phycfg.Cap_10Half = TRUE;
            }
            else if (mcInfo.local_Portcfg[port].workmode == 2)
            {
                /*100Full */
                phycfg.AutoNegotiation = 0;
                phycfg.Speed = 0x1;
                phycfg.Fullduplex = 1;
                phycfg.Cap_1000Full = FALSE;
                phycfg.Cap_100Full = TRUE;
                phycfg.Cap_100Half = TRUE;
                phycfg.Cap_10Full = TRUE;
                phycfg.Cap_10Half = TRUE;
            }
            else if (mcInfo.local_Portcfg[port].workmode == 3)
            {
                /*100Half */
                phycfg.AutoNegotiation = 0;
                phycfg.Speed = 0x1;
                phycfg.Fullduplex = 0;
                phycfg.Cap_1000Full = FALSE;
                phycfg.Cap_100Full = FALSE;
                phycfg.Cap_100Half = TRUE;
                phycfg.Cap_10Full = TRUE;
                phycfg.Cap_10Half = TRUE;
            }
            else if (mcInfo.local_Portcfg[port].workmode == 4)
            {
                /*10Full */
                phycfg.AutoNegotiation = 0;
                phycfg.Speed = 0;
                phycfg.Fullduplex = 1;
                phycfg.Cap_1000Full = FALSE;
                phycfg.Cap_100Full = FALSE;
                phycfg.Cap_100Half = FALSE;
                phycfg.Cap_10Full = TRUE;
                phycfg.Cap_10Half = TRUE;

            }
            else if (mcInfo.local_Portcfg[port].workmode == 5)
            {
                /*10Half */
                phycfg.AutoNegotiation = 0;
                phycfg.Speed = 0;
                phycfg.Fullduplex = 0;
                phycfg.Cap_1000Full = FALSE;
                phycfg.Cap_100Full = FALSE;
                phycfg.Cap_100Half = FALSE;
                phycfg.Cap_10Full = FALSE;
                phycfg.Cap_10Half = TRUE;
            }
            rtl8363_setAsicEthernetPHY(port, phycfg);

        }

        /*set bandwidth control */
        if (!mcInfo.local_Portcfg[port].ingressrate)
        {
            portRate.enabled = FALSE;
        }
        else
        {
            portRate.enabled = TRUE;
            portRate.rate = mcInfo.local_Portcfg[port].ingressrate;
        }
        rtl8363_setAsicQosPortRate(port, RTL8363_PORT_RX, portRate);

        if (!mcInfo.local_Portcfg[port].egressrate)
        {
            portRate.enabled = FALSE;
        }
        else
        {
            portRate.enabled = TRUE;
            portRate.rate = mcInfo.local_Portcfg[port].egressrate;
        }
        rtl8363_setAsicQosPortRate(port, RTL8363_PORT_TX, portRate);
        //printf("\ntx rate %d\n", mcInfo.local_Portcfg[0].egressrate);

    }

    /*vlan configuration*/
    rtl8363_setAsicVlanEnable(mcInfo.vlan_group.vlan_mode);
     for (i = 0; i < 16; i++)
        rtl8363_setAsicVlan(i, mcInfo.vlan_group.vlan_id[i], mcInfo.vlan_group.vlan_member[i]);
     rtl8363_setAsicVlanIngressFilter(mcInfo.vlan_perport.ingree_filter);
     for (i = 0; i < 3; i++)
     {
        rtl8363_setAsicVlanTagInsertRemove(i, mcInfo.vlan_perport.engree_linktype[i]);
            rtl8363_setAsicPortVlanIndex(i, mcInfo.vlan_perport.vlan_entry[i]);
     }

      /*QINQ*/
      mcInfo.qinq.flag = QINQ_TYPE;
      setQinQ(&mcInfo.qinq);
       mcInfo.qinq.flag = QINQ_VID;
      setQinQ(&mcInfo.qinq);
      mcInfo.qinq.flag = QINQ_PRI;
      setQinQ(&mcInfo.qinq);
      mcInfo.qinq.flag = QINQ_ENABLE;
      setQinQ(&mcInfo.qinq);

      /*MIRROR*/
      mir.enMirMac = FALSE;  /*VAUTION: THIS DOES NOT MEAN DISBALE MIRROR!!! SEE rtl8363_setAsicMirror*/
      mir.mirport = mcInfo.mirror.mirport;
      mir.rxport = mcInfo.mirror.rxport;
      mir.txport = mcInfo.mirror.txport;
      rtl8363_setAsicMirror(mir);;


    return SUCCESS;
}

/* Function Name:
 *      dumpMCinfo
 * Description:
 *      dump mcInfo in uart console
 * Input:
 *       none
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
void dumpMCinfo()
{
#if _ORGOAMPDU_
    uint8 xdata port;
#endif

    //printf("dump mcInfo \n");
    printf("\n");
    printf("Local Infomation(software version %c.%c.%c):\n",
           (char) (48 + mcInfo.local_syscfg.ver[0]),
           (char) (48 + mcInfo.local_syscfg.ver[1]),
           (char) (48 + mcInfo.local_syscfg.ver[2]));
    printf("mac %02bX:%02bX:%02bX:%02bX:%02bX:%02bX\n",
           mcInfo.local_syscfg.mac[0], mcInfo.local_syscfg.mac[1],
           mcInfo.local_syscfg.mac[2], mcInfo.local_syscfg.mac[3],
           mcInfo.local_syscfg.mac[4], mcInfo.local_syscfg.mac[5]);
    printf(" ip %bu.%bu.%bu.%bu\n", mcInfo.local_syscfg.ip[0],
           mcInfo.local_syscfg.ip[1], mcInfo.local_syscfg.ip[2],
           mcInfo.local_syscfg.ip[3]);
#ifdef _ARPTBL_
    printf("gateway %bu.%bu.%bu.%bu\n", mcInfo.local_syscfg.gateway[0],
           mcInfo.local_syscfg.gateway[1], mcInfo.local_syscfg.gateway[2],
           mcInfo.local_syscfg.gateway[3]);
    printf("netmask %bu.%bu.%bu.%bu\n", mcInfo.local_syscfg.netmask[0],
           mcInfo.local_syscfg.netmask[1], mcInfo.local_syscfg.netmask[2],
           mcInfo.local_syscfg.netmask[3]);
#endif
    printf("Device name: %s\n", mcInfo.devicename);

#ifdef _DHCP_
    printf("DHCP client is %s\n", mcInfo.dhcpEn ? "enabled" : "disabled");
  #if 0
       printf(" ip %bu.%bu.%bu.%bu\n",(uint8)((mc_netif.ip_addr.addr& 0xFF000000)>> 24), (uint8)((mc_netif.ip_addr.addr& 0xFF0000)>> 16),
       (uint8)((mc_netif.ip_addr.addr& 0xFF00)>> 8), (uint8)(mc_netif.ip_addr.addr& 0xFF));
       printf("gateway %bu.%bu.%bu.%bu\n", mcInfo.local_syscfg.gateway[0],mcInfo.local_syscfg.gateway[1],mcInfo.local_syscfg.gateway[2],mcInfo.local_syscfg.gateway[3]);
       printf("netmask %bu.%bu.%bu.%bu\n", mcInfo.local_syscfg.netmask[0],mcInfo.local_syscfg.netmask[1],mcInfo.local_syscfg.netmask[2],mcInfo.local_syscfg.netmask[3]);
  #endif
#endif

#if _ORGOAMPDU_
    printf
        ("enDot3ah:%bu, enLinktransparent:%bu, enJumbo:%bu, mcMode:%bu, enSuperRfd:%bu\n",
         mcInfo.local_syscfg.enDot3ah, mcInfo.local_syscfg.enLinktransparent,
         mcInfo.local_syscfg.enJumbo, mcInfo.local_syscfg.mcMode,
         mcInfo.local_syscfg.enSuperRfd);
    for (port = 0; port < 2; port++)
    {
        printf
            ("Port%bu:loop:%bu, lockstate:%bu, workmode:%bu,ingressrate:%u, egressrate:%u\n",
             port, mcInfo.local_Portcfg[port].loop,
             mcInfo.local_Portcfg[port].lockstate,
             mcInfo.local_Portcfg[port].workmode,
             mcInfo.local_Portcfg[port].ingressrate,
             mcInfo.local_Portcfg[port].egressrate);

        printf
            ("Port%bu:link:%bu, speed:%bu, duplex:%bu,auto:%bu, ForceDown:%bu, DisFlc:%bu\n",
             port, mcInfo.local_Portcfg[port].linkstatus,
             mcInfo.local_Portcfg[port].speed,
             mcInfo.local_Portcfg[port].duplex,
             mcInfo.local_Portcfg[port].autoneg,
             mcInfo.local_Portcfg[port].isForceLinkDown,
             mcInfo.local_Portcfg[port].disFlowControl);
    }

    if (!mcInfo.local_syscfg.mcMode)
        return;
    if (oamPara.local_pdu != OAM_ANY)
        return;
    printf("\n");
    printf("Remote Infomation(software version %c.%c.%c):\n",
           (char) (48 + mcInfo.remote_syscfg.ver[0]),
           (char) (48 + mcInfo.remote_syscfg.ver[1]),
           (char) (48 + mcInfo.remote_syscfg.ver[2]));
    printf("mac %02bX:%02bX:%02bX:%02bX:%02bX:%02bX",
           mcInfo.remote_syscfg.mac[0], mcInfo.remote_syscfg.mac[1],
           mcInfo.remote_syscfg.mac[2], mcInfo.remote_syscfg.mac[3],
           mcInfo.remote_syscfg.mac[4], mcInfo.remote_syscfg.mac[5]);
    printf(" ip %bu.%bu.%bu.%bu\n", mcInfo.remote_syscfg.ip[0],
           mcInfo.remote_syscfg.ip[1], mcInfo.remote_syscfg.ip[2],
           mcInfo.remote_syscfg.ip[3]);
    printf
        ("enDot3ah:%bu, enLinktransparent:%bu, enJumbo:%bu, mcMode:%bu, enSuperRfd:%bu\n",
         mcInfo.remote_syscfg.enDot3ah, mcInfo.remote_syscfg.enLinktransparent,
         mcInfo.remote_syscfg.enJumbo, mcInfo.remote_syscfg.mcMode,
         mcInfo.remote_syscfg.enSuperRfd);
    for (port = 0; port < 2; port++)
    {
        printf
            ("Port%bu:loop:%bu, lockstate:%bu, workmode:%bu,ingressrate:%u, egressrate:%u\n",
             port, mcInfo.remote_Portcfg[port].loop,
             mcInfo.remote_Portcfg[port].lockstate,
             mcInfo.remote_Portcfg[port].workmode,
             mcInfo.remote_Portcfg[port].ingressrate,
             mcInfo.remote_Portcfg[port].egressrate);

        printf
            ("Port%bu:link:%bu, speed:%bu, duplex:%bu,auto:%bu, ForceDown:%bu, DisFlc:%bu\n",
             port, mcInfo.remote_Portcfg[port].linkstatus,
             mcInfo.remote_Portcfg[port].speed,
             mcInfo.remote_Portcfg[port].duplex,
             mcInfo.remote_Portcfg[port].autoneg,
             mcInfo.remote_Portcfg[port].isForceLinkDown,
             mcInfo.remote_Portcfg[port].disFlowControl);
    }
#endif

}

void getFlowContorlAbility(uint8 * pbuf)
{
    pbuf[UTPPORT] = getFlowcontrol(UTPPORT);
    pbuf[FIBERPORT] = getFlowcontrol(FIBERPORT);

}


#if defined(_WEBSMART_) || defined(_LNKEVTOAM_)

/*Convert a long long (64bit) number to string
*It use Chinese Remainder Theorem
*/
void LLtoStr(uint8 * src, uint8 ** dst)
{
    uint16 xdata c = 0;
    uint32 xdata tmp1 = 0, xdata tmp2 = 0, xdata tmp3 = 0, xdata tmp4 = 0;
    uint16 xdata tmp5 = 0;

    tmp1 =
        7936L * src[0] + 656L * src[1] + 7776L * src[2] + 7296L * src[3] +
        7216L * src[4] + 5536L * src[5] + 256L * src[6] + src[7];

    tmp2 =
        3792L * src[0] + 7671L * src[1] + 1162L * src[2] + 9496L * src[3] +
        1677L * src[4] + 6 * src[5];
    tmp3 = 5940L * src[0] + 4749L * src[1] + 995L * src[2] + 42 * src[3];
    tmp4 = 2057L * src[0] + 281L * src[1] + src[2];
    tmp5 = 7 * src[0];

    c = tmp1 / 10000;
    tmp1 = tmp1 % 10000;

    tmp2 += c;
    c = tmp2 / 10000;
    tmp2 = tmp2 % 10000;

    tmp3 += c;
    c = tmp3 / 10000;
    tmp3 = tmp3 % 10000;

    tmp4 += c;
    c = tmp4 / 10000;
    tmp4 = tmp4 % 10000;

    tmp5 += c;

    //printf("%d%04u%04u%04u%04u\n",tmp5, (uint16)tmp4, (uint16)tmp3, (uint16)tmp2, (uint16)tmp1);
    c = sprintf(*dst, "%d%04u%04u", tmp5, (uint16) tmp4, (uint16) tmp3);
    sprintf(*dst + c, "%04u%04u\0", (uint16) tmp2, (uint16) tmp1);

    while (**dst == '0' && (*(*dst + 1) != '\0'))
    {
        (*dst)++;
    }
}
#endif

#if defined(_WEBSMART_) || defined(_LNKEVTOAM_)

/* Function Name:
 *      bigNumAdd
 * Description:
 *      Used for two 32bit number add
 * Input:
 *       p1     - addend 1
 *       p2     - addend 2
 * Output:
 *       result - result
 * Return:
 *       none
 * Note:
 *
 */
void bigNumAdd(bigNum_t * p1, bigNum_t * p2, bigNum_t * result)
{
    uint8 xdata carry;
    uint8 i;
    bigNum_t *xdata p;
    memset(result->pnum, 0, result->len);
    //big endian
    carry = 0;
    for (i = 1; (i <= p1->len) && (i <= p2->len) && (i <= result->len); i++)
    {
        result->pnum[result->len - i] =
            p1->pnum[p1->len - i] + p2->pnum[p2->len - i] + carry;
        if (p1->pnum[p1->len - i] > (0xFF - carry - p2->pnum[p2->len - i]))
            carry = 1;
        else
            carry = 0;
    }
    if (i == result->len)
        return;
    else if (p1->len >= i)
        p = p1;
    else if (p2->len >= i)
        p = p2;
    else
    {
        if (carry)
            result->pnum[result->len - i] = 1;
        return;
    }

    for (; (i <= result->len) || (i <= p->len); i++)
    {
        result->pnum[result->len - i] = p->pnum[p->len - i] + carry;
        if ((p->pnum[p->len - i] == 0xFF) & (carry == 1))
            carry = 1;
        else
            carry = 0;
    }

    if (result->len >= i)
        result->pnum[result->len - i] = 1;

}

#endif


#if defined(_LNKEVTOAM_)

/* Function Name:
 *      bigNumAdd
 * Description:
 *      Used for two big number subtration
 * Input:
 *       p1     - subtrahend 1
 *       p2     - subtrahend 2
 * Output:
 *       result - result
 * Return:
 *       none
 * Note:
 *
 */
void bigNumSub(bigNum_t * p1, bigNum_t * p2, bigNum_t * result)
{
    uint8 xdata i;

    /*big endian and result len must equal p1 len */

    if ((p1->len < p2->len) || (result->len < p1->len))
        return;

    if (p1->len == p2->len)
    {
        for (i = 0; i < p1->len; i++)
        {
            if (p1->pnum[i] > p2->pnum[i])
                break;
            else if (p1->pnum[i] == p2->pnum[i])
                continue;
            else
                return;
        }
    }

    memcpy(result->pnum, p1->pnum, p1->len);
    for (i = p1->len - p2->len; i < p1->len; i++)
    {
        if (p1->pnum[i] >= p2->pnum[i])
            result->pnum[i] = p1->pnum[i] - p2->pnum[i];
        else
        {
            result->pnum[i - 1]--;
            result->pnum[i] = 0xFF - p2->pnum[i] + p1->pnum[i] + 1;
        }

    }
    return;
}

/* Function Name:
 *      numCmp
 * Description:
 *      Used for compare two big number
 * Input:
 *       p1     - number 1
 *       p2     - number 2
 * Output:
 *       result - result
 * Return:
 *       none
 * Note:
 *
 */
void numCmp(bigNum_t * p1, bigNum_t * p2, int8 * result)
{
    uint8 xdata i;

    /*big endian and compare len at first */
    if (p1->len > p2->len)
    {
        *result = 1;
        return;
    }
    else if (p1->len < p2->len)
    {
        *result = -1;
        return;
    }

    for (i = 0; i < p1->len; i++)
    {
        if (p1->pnum[i] > p2->pnum[i])
        {
            *result = 1;
            return;
        }
        else if (p1->pnum[i] < p2->pnum[i])
        {
            *result = -1;
            return;
        }
    }
    *result = 0;
}

#endif

/* Function Name:
 *      changeMac
 * Description:
 *      change local mac address
 * Input:
 *       mac  - mac address
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
void changeMac(uint8 * mac)
{
    memcpy(mcInfo.local_syscfg.mac, mac, 6);
    memcpy(mc_netif.hwaddr, mac, 6);
}

/* Function Name:
 *      changeIp
 * Description:
 *      change local ip address
 * Input:
 *       ip  - ip address
 * Output:
 *       none
 * Return:
 *       none
 * Note:
 *
 */
void changeIp(uint8 * ip)
{
    memcpy(mcInfo.local_syscfg.ip, ip, 4);
    memcpy((uint8 *) & mc_netif.ip_addr.addr, ip, 4);
}
