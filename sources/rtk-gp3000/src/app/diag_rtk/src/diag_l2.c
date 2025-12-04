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
#include <hal/common/halctrl.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/l2.h>
#include <rtk/trap.h>
#if defined(CONFIG_SDK_APOLLO)
#include <dal/apollo/raw/apollo_raw_port.h>
#include <dal/apollo/raw/apollo_raw_l2.h>
#endif

#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/raw/apollomp_raw_port.h>
#include <dal/apollomp/raw/apollomp_raw_l2.h>
#endif

#if defined(CONFIG_SDK_RTL9602C)
#include <hal/chipdef/rtl9602c/rtl9602c_def.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_table_struct.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_l2.h>
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_table_struct.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_l2.h>
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_table_struct.h>
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_l2.h>
#endif

#if defined(CONFIG_SDK_RTL9601B)
extern int32 rtl9601b_raw_l2_unknReservedMcFlood_set(rtk_enable_t state);
extern int32 rtl9601b_raw_l2_unknReservedMcFlood_get(rtk_enable_t *pState);
#endif


static uint32 efid = 0;
static rtk_l2_hashData_t hashData;

/* Static function */
#if defined(CONFIG_SDK_APOLLO)
static void
_diag_lutDisplay(apollo_lut_table_t *diag_lut)
{
	uint8 strBuffer[64];
	uint8 strDsl[64];
	uint8 strExt[64];
	uint8 strSip[64];
	uint8 strDip[64];

    apollo_raw_l2_ipMcHashType_t ipMcHashType;



	if(diag_lut->lookup_busy)
	{
		diag_util_mprintf("\nLUT Access Busy\n");
	}
    else if(!diag_lut->lookup_hit)
    {
        diag_util_mprintf("\nLUT Access Error\n");
    }
    else
    {
        diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",diag_lut->address,(diag_lut->address > (APOLLO_LUT_4WAY_NO-1))?"(64 BCAM)":"(2K LUT)");

        if(diag_lut->l3lookup)
        {
            apollo_raw_l2_ipmcHashType_get(&ipMcHashType);


            diag_lut->gip = diag_lut->gip | 0xE0000000;
            diag_util_ip2str(strSip,diag_lut->sip_vid);
            diag_util_ip2str(strDip,diag_lut->gip);
            diag_util_mask2str(strDsl, diag_lut->dsl_mbr);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

    		diag_util_mprintf("IP Multicast table:\n");

            if(ipMcHashType == RAW_LUT_IPMCHASH_TYPE_GIPVID)
            {
                diag_util_mprintf("DestinationIP   Vid  Member Fwd Pri State  Ext   Dsl   DipOnly\n");

        		diag_util_mprintf("%-15s %-4d %-6s %-3s %-3d %-6s %-5s %-5s %s\n",
                                    strDip,
                                    diag_lut->sip_vid,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    strDsl,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
            else
            {
                diag_util_mprintf("DestinationIP   SourceIP        Member Fwd Pri State  Ext   Dsl\n");

        		diag_util_mprintf("%-15s %-15s %-6s %-3s %-3d %-6s %-5s %-5s %s\n",
                                    strDip,
                                    strSip,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    strDsl,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
        }
        else if(diag_lut->mac.octet[0]&0x01)
        {
            diag_util_mask2str(strDsl, diag_lut->dsl_mbr);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

            diag_util_mprintf("L2 Multicast table:\n");
			diag_util_mprintf("MACAddress         Member Fid_Vid FwdPriEn Pri State  Hash Ext   Dsl\n");

			diag_util_mprintf("%-18s %-6s %-7d %-8s %-3d %-6s %-4s %-5s %-s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_util_mask32tostr(diag_lut->mbr),
			                    diag_lut->cvid_fid,
			                    diagStr_enDisplay[diag_lut->fwdpri_en],
			                    diag_lut->lut_pri,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl],
                                strExt,
                                strDsl);
        }
        else
        {
            diag_util_mprintf("L2 Unicast table:\n");
			diag_util_mprintf("MACAddress        Spa Fid Efid Age Vid  State  Hash\n");
 			diag_util_mprintf("%-17s %-3d %-3d %-4d %-3d %-4d %-6s %s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_lut->spa,
			                    diag_lut->fid,
			                    diag_lut->efid,
			                    diag_lut->age,
			                    diag_lut->cvid_fid,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl]);

			diag_util_mprintf("SaPriEn FwdPriEn Pri Auth DaBlock SaBlock Arp ExtDsl\n");
 			diag_util_mprintf("%-7s %-8s %-3d %-4s %-7s %-7s %-3s %d\n",
			                    diagStr_enDisplay[diag_lut->sapri_en],
                                diagStr_enDisplay[diag_lut->fwdpri_en],
                                diag_lut->lut_pri,
                                diagStr_enDisplay[diag_lut->auth],
                                diagStr_enDisplay[diag_lut->da_block],
                                diagStr_enDisplay[diag_lut->sa_block],
                                diagStr_enDisplay[diag_lut->arp_used],
                                diag_lut->ext_dsl_spa);
		}
    }
}
#endif

#if defined(CONFIG_SDK_APOLLOMP)
static void
_diag_apollomp_lutDisplay(apollomp_lut_table_t *diag_lut)
{
	uint8 strBuffer[64];
	uint8 strDsl[64];
	uint8 strExt[64];
	uint8 strSip[64];
	uint8 strDip[64];

    apollomp_raw_l2_ipMcHashType_t ipMcHashType;



	if(diag_lut->lookup_busy)
	{
		diag_util_mprintf("\nLUT Access Busy\n");
	}
    else if(!diag_lut->lookup_hit)
    {
        diag_util_mprintf("\nLUT Access Error\n");
    }
    else
    {
        diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",diag_lut->address,(diag_lut->address > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

        if(diag_lut->l3lookup)
        {
            apollomp_raw_l2_ipmcHashType_get(&ipMcHashType);


            diag_lut->gip = diag_lut->gip | 0xE0000000;
            diag_util_ip2str(strSip,diag_lut->sip_vid);
            diag_util_ip2str(strDip,diag_lut->gip);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

    		diag_util_mprintf("IP Multicast table:\n");
			if(diag_lut->gip_only)
			{
                diag_util_mprintf("DestinationIP   Member Fwd Pri State  Ext   L3Route Forced L3Index(6-0)\n");

        		diag_util_mprintf("%-15s %-6s %-3s %-3d %-6s %-5s %-7s %-6s %2d %2d %2d %2d %2d %2d %2d\n",
                                    strDip,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    diagStr_enDisplay[diag_lut->wan_sa],
                                    diagStr_enDisplay[diag_lut->ext_fr],
                                    (diag_lut->l3_idx>>24) & 0xF,
                                    (diag_lut->l3_idx>>20) & 0xF,
                                    (diag_lut->l3_idx>>16) & 0xF,
                                    (diag_lut->l3_idx>>12) & 0xF,
                                    (diag_lut->l3_idx>>8) & 0xF,
                                    (diag_lut->l3_idx>>4) & 0xF,
                                    diag_lut->l3_idx & 0xF);
			}
            else if(ipMcHashType == APOLLOMP_RAW_LUT_IPMCHASH_TYPE_GIPVID)
            {
                diag_util_mprintf("DestinationIP   Vid Member Fwd Pri State  Ext\n");

        		diag_util_mprintf("%-15s %-3d %-6s %-3s %-3d %-6s %s\n",
                                    strDip,
                                    diag_lut->sip_vid,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt);
            }
            else
            {
                diag_util_mprintf("DestinationIP   SourceIP        Member Fwd Pri State  Ext    DipOnly\n");

        		diag_util_mprintf("%-15s %-15s %-6s %-3s %-3d %-6s %-6s %-7s\n",
                                    strDip,
                                    strSip,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
        }
        else if(diag_lut->mac.octet[0]&0x01)
        {
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

            diag_util_mprintf("L2 Multicast table:\n");
			diag_util_mprintf("MACAddress         Member Fid_Vid FwdPriEn Pri State  Hash Ext   \n");

			diag_util_mprintf("%-18s %-6s %-7d %-8s %-3d %-6s %-4s %-5s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_util_mask32tostr(diag_lut->mbr),
			                    diag_lut->cvid_fid,
			                    diagStr_enDisplay[diag_lut->fwdpri_en],
			                    diag_lut->lut_pri,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl],
                                strExt);
        }
        else
        {
            diag_util_mprintf("L2 Unicast table:\n");
			diag_util_mprintf("MACAddress        Spa Fid Efid Age Vid  State  Hash\n");
 			diag_util_mprintf("%-17s %-3d %-3d %-4d %-3d %-4d %-6s %s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_lut->spa,
			                    diag_lut->fid,
			                    diag_lut->efid,
			                    diag_lut->age,
			                    diag_lut->cvid_fid,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl]);

			diag_util_mprintf("SaPriEn FwdPriEn Pri Auth DaBlock SaBlock Arp ExtDsl\n");
 			diag_util_mprintf("%-7s %-8s %-3d %-4s %-7s %-7s %-3s %d\n",
			                    diagStr_enDisplay[diag_lut->sapri_en],
                                diagStr_enDisplay[diag_lut->fwdpri_en],
                                diag_lut->lut_pri,
                                diagStr_enDisplay[diag_lut->auth],
                                diagStr_enDisplay[diag_lut->da_block],
                                diagStr_enDisplay[diag_lut->sa_block],
                                diagStr_enDisplay[diag_lut->arp_used],
                                diag_lut->ext_dsl_spa);
		}
    }
}
#endif


#if defined(CONFIG_SDK_RTL9601B)
static void
_diag_rtl9601b_lutDisplay(rtk_l2_addr_table_t *diag_lut)
{
    int32 ret = RT_ERR_FAILED;
    uint8 strBuffer[64];
	uint8 strDsl[64];
	uint8 strExt[64];
	uint8 strSip[64];
	uint8 strDip[64];

    rtk_l2_ipmcMode_t ipMcHashType;
	rtk_l2_ucastAddr_t   l2UcEntry;
	rtk_l2_mcastAddr_t   l2McEntry;
	rtk_l2_ipMcastAddr_t ipmcEntry;


    switch(diag_lut->entryType)
    {
    case RTK_LUT_L2UC:
		l2UcEntry = diag_lut->entry.l2UcEntry;
		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2UcEntry.index,(l2UcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(8 BCAM)":"(256 LUT)");
		diag_util_mprintf("L2 Unicast table:\n");
		diag_util_mprintf("MACAddress        Spa Age Vid  State  Hash\n");
		diag_util_mprintf("%-17s %-3d %-3d %-4d %-6s %s\n",
		                diag_util_inet_mactoa(&l2UcEntry.mac.octet[0]),
		                l2UcEntry.port,
		                l2UcEntry.age,
		                l2UcEntry.vid,
		                diagStr_l2LutStaticOrAutoStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC) ? 1:0],
		                diagStr_l2HashMethodStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_IVL) ? 1:0 ]);

		diag_util_mprintf("CtagIf CtagVid\n");
		diag_util_mprintf("%-6s %-7d\n",
		                diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_CTAG_IF) ? 1:0],
						l2UcEntry.ctag_vid);
	break;
	case RTK_LUT_L2MC:
		l2McEntry = diag_lut->entry.l2McEntry;
		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2McEntry.index,(l2McEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(8 BCAM)":"(256 LUT)");
		diag_util_mprintf("L2 Multicast table:\n");
		diag_util_mprintf("MACAddress         Member Vid\n");

		diag_util_mprintf("%-18s %-6s %-7d\n",
		                diag_util_inet_mactoa(&l2McEntry.mac.octet[0]),
		                diag_util_mask32tostr(l2McEntry.portmask.bits[0]),
		                l2McEntry.vid);
	break;
	case RTK_LUT_L3MC:
		ipmcEntry = diag_lut->entry.ipmcEntry;
		ret = rtk_l2_ipmcMode_get(&ipMcHashType);
        if(ret != RT_ERR_OK)
            break;
        
		ipmcEntry.dip = ipmcEntry.dip | 0xE0000000;
        diag_util_ip2str(strDip,ipmcEntry.dip);
		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcEntry.index,(ipmcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(8 BCAM)":"(256 LUT)");
		diag_util_mprintf("IP Multicast table:\n");

        if(ipMcHashType == LOOKUP_ON_DIP_AND_VID)
        {
            diag_util_mprintf("DestinationIP   Vid  Member\n");

    		diag_util_mprintf("%-15s %-4d %-6s\n",
                                strDip,
                                ipmcEntry.vid,
                                diag_util_mask32tostr(ipmcEntry.portmask.bits[0]));
        }
        else
        {
            diag_util_mprintf("DestinationIP   Member\n");

    		diag_util_mprintf("%-15s %-6s\n",
                                strDip,
                                diag_util_mask32tostr(ipmcEntry.portmask.bits[0]));
        }
	break;
	default:
	break;
    }

}
#endif

#if defined(CONFIG_SDK_RTL9602C)
static void
_diag_rtl9602c_lutDisplay(rtk_l2_addr_table_t *diag_lut, uint32 displayAddr)
{
    uint8 strDip[64];
    uint8 strMBR[64];
    uint8 strExt[64];

    //rtk_l2_ipmcMode_t ipMcHashType;
    rtk_l2_ucastAddr_t   l2UcEntry;
    rtk_l2_mcastAddr_t   l2McEntry;
    rtk_l2_ipMcastAddr_t ipmcEntry;

    switch(diag_lut->entryType)
    {
    case RTK_LUT_L2UC:
		l2UcEntry = diag_lut->entry.l2UcEntry;
        if(displayAddr)
        {
	    	diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2UcEntry.index,(l2UcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(1K LUT)");
        }
        diag_util_mprintf("L2 Unicast table:\n");
		diag_util_mprintf("MACAddress        Spa Fid Age Vid  State  Ext  Hash\n");
		diag_util_mprintf("%-17s %-3d %-3d %-3d %-4d %-6s %-4d %s\n",
            diag_util_inet_mactoa(&l2UcEntry.mac.octet[0]),
            l2UcEntry.port,
            l2UcEntry.fid,
            l2UcEntry.age,
            l2UcEntry.vid,
            diagStr_l2LutStaticOrAutoStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC) ? 1:0],
            l2UcEntry.ext_port,
            diagStr_l2HashMethodStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_IVL) ? 1:0]);

		diag_util_mprintf("CtagIf Auth DaBlock SaBlock Arp\n");
		diag_util_mprintf("%-6s %-4s %-7s %-7s %-3s\n",
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_CTAG_IF) ? 1:0],
            diagStr_enDisplay[l2UcEntry.auth],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_ARP_USED) ? 1:0]);
	break;
	case RTK_LUT_L2MC:
		l2McEntry = diag_lut->entry.l2McEntry;
        diag_util_mask2str(strMBR, l2McEntry.portmask.bits[0]);
        diag_util_mask2str(strExt, l2McEntry.ext_portmask.bits[0]);
        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2McEntry.index,(l2McEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(1K LUT)");
        }
		diag_util_mprintf("L2 Multicast table:\n");
		diag_util_mprintf("MACAddress         Member Ext    Fid_Vid Hash\n");
		diag_util_mprintf("%-18s %-6s %-6s %-7d %-4s\n",
            diag_util_inet_mactoa(&l2McEntry.mac.octet[0]),
            strMBR,
            strExt,
            (l2McEntry.flags & RTK_L2_MCAST_FLAG_IVL) ? l2McEntry.vid : l2McEntry.fid,
            diagStr_l2HashMethodStr[(l2McEntry.flags & RTK_L2_MCAST_FLAG_IVL) ? 1:0]);
	break;
	case RTK_LUT_L3MC:
		ipmcEntry = diag_lut->entry.ipmcEntry;
		//rtk_l2_ipmcMode_get(&ipMcHashType);
		ipmcEntry.dip = ipmcEntry.dip | 0xE0000000;
        diag_util_ip2str(strDip, ipmcEntry.dip);
        diag_util_mask2str(strMBR, ipmcEntry.portmask.bits[0]);
        diag_util_mask2str(strExt, ipmcEntry.ext_portmask.bits[0]);
        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcEntry.index,(ipmcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(1K LUT)");
        }
		diag_util_mprintf("IP Multicast table:\n");

        diag_util_mprintf("DestinationIP   Fid_Vid Hash Member Ext    State  L3Route L3Index(3-0)\n");
		diag_util_mprintf("%-15s %-7d %-4s %-6s %-6s %-6s %-7s %2d %2d %2d %2d\n",
            strDip,
            (ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? ipmcEntry.vid : ipmcEntry.fid,
            diagStr_l2HashMethodStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? 1:0],
            strMBR,
            strExt,
            diagStr_l2LutStaticOrAutoStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1:0],
            diagStr_enDisplay[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN) ? 1:0],
            (ipmcEntry.l3_trans_index>>12) & 0xF,
            (ipmcEntry.l3_trans_index>>8) & 0xF,
            (ipmcEntry.l3_trans_index>>4) & 0xF,
            ipmcEntry.l3_trans_index & 0xF);

		diag_util_mprintf("SIP_Filter SIP_Idx\n");
		diag_util_mprintf("%-10s %-7d\n",
            diagStr_enDisplay[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_SIP_FILTER) ? 1:0],
            ipmcEntry.sip_index);
	break;
	case RTK_LUT_L3V6MC:
		ipmcEntry = diag_lut->entry.ipmcEntry;
        diag_util_mask2str(strMBR, ipmcEntry.portmask.bits[0]);
        diag_util_mask2str(strExt, ipmcEntry.ext_portmask.bits[0]);
        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcEntry.index,(ipmcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(1K LUT)");
        }
		diag_util_mprintf("IPv6 Multicast table:\n");
		diag_util_mprintf("DestinationIP                            Member Ext    L3Route L3Index\n");
		diag_util_mprintf("%-40s %-6s %-6s %-7s %-7d\n",
            diag_util_inet_n6toa(&ipmcEntry.dip6.ipv6_addr[0]),
            strMBR,
            strExt,
            diagStr_enDisplay[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN) ? 1:0],
            ipmcEntry.l3_mcr_index);
	break;
	default:
	break;
    }
}
#endif

#if defined(CONFIG_SDK_RTL9607C)
static void
_diag_rtl9607c_lutDisplay(rtk_l2_addr_table_t *diag_lut, uint32 displayAddr)
{
    int ret;
    uint32 type;
    uint8 strDip[64];
    uint8 strSip[64];
    uint8 strMBR[64];
    uint8 strExt[64];

    //rtk_l2_ipmcMode_t ipMcHashType;
    rtk_l2_ucastAddr_t   l2UcEntry;
    rtk_l2_mcastAddr_t   l2McEntry;
    rtk_l2_ipMcastAddr_t ipmcEntry;

    switch(diag_lut->entryType)
    {
    case RTK_LUT_L2UC:
		l2UcEntry = diag_lut->entry.l2UcEntry;
        if(displayAddr)
        {
	    	diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2UcEntry.index,(l2UcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
        }
        diag_util_mprintf("L2 Unicast table:\n");
		diag_util_mprintf("MACAddress        Spa Fid Age Vid  State  Ext  Hash\n");
		diag_util_mprintf("%-17s %-3d %-3d %-3d %-4d %-6s %-4d %s\n",
            diag_util_inet_mactoa(&l2UcEntry.mac.octet[0]),
            l2UcEntry.port,
            l2UcEntry.fid,
            l2UcEntry.age,
            l2UcEntry.vid,
            diagStr_l2LutStaticOrAutoStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC) ? 1:0],
            l2UcEntry.ext_port,
            diagStr_l2HashMethodStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_IVL) ? 1:0]);

		diag_util_mprintf("CtagIf DaBlock SaBlock Arp\n");
		diag_util_mprintf("%-6s %-7s %-7s %-3s\n",
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_CTAG_IF) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_ARP_USED) ? 1:0]);
    	break;
	case RTK_LUT_L2MC:
		l2McEntry = diag_lut->entry.l2McEntry;
        diag_util_mask2str(strMBR, l2McEntry.portmask.bits[0]);
        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2McEntry.index,(l2McEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
        }
		diag_util_mprintf("L2 Multicast table:\n");
		diag_util_mprintf("MACAddress         Member ExtIdx    Fid_Vid Hash\n");
		diag_util_mprintf("%-18s %-6s %-9d %-7d %-4s\n",
            diag_util_inet_mactoa(&l2McEntry.mac.octet[0]),
            strMBR,
            l2McEntry.ext_portmask_idx,
            (l2McEntry.flags & RTK_L2_MCAST_FLAG_IVL) ? l2McEntry.vid : l2McEntry.fid,
            diagStr_l2HashMethodStr[(l2McEntry.flags & RTK_L2_MCAST_FLAG_IVL) ? 1:0]);
    	break;
	case RTK_LUT_L3MC:
		ipmcEntry = diag_lut->entry.ipmcEntry;
		//rtk_l2_ipmcMode_get(&ipMcHashType);
		ipmcEntry.dip = ipmcEntry.dip | 0xE0000000;
        diag_util_ip2str(strDip, ipmcEntry.dip);
        diag_util_ip2str(strSip, ipmcEntry.sip);
        diag_util_mask2str(strMBR, ipmcEntry.portmask.bits[0]);

        if ((ret = reg_field_read(RTL9607C_LUT_CFGr, RTL9607C_LUT_IPMC_HASHf, &type)) != RT_ERR_OK)
        {
            break;
        }

        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcEntry.index,(ipmcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
        }
		diag_util_mprintf("IP Multicast table:\n");

        if(type == 2) /* IPMC hash mode = GIP + SIP */
        {
            diag_util_mprintf("DestinationIP   SourceIP        Hash Member ExtIdx    State\n");
    		diag_util_mprintf("%-15s %-15s %-4s %-6s %-9d %-6s\n",
                strDip,
                strSip,
                diagStr_l2HashMethodStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? 1:0],
                strMBR,
                ipmcEntry.ext_portmask_idx,
                diagStr_l2LutStaticOrAutoStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1:0]);
        }
        else /* All other using GIP + VID/FID display */
        {
            diag_util_mprintf("DestinationIP   Fid_Vid Hash Member ExtIdx    State\n");
    		diag_util_mprintf("%-15s %-7d %-4s %-6s %-9d %-6s\n",
                strDip,
                (ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? ipmcEntry.vid : ipmcEntry.fid,
                diagStr_l2HashMethodStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? 1:0],
                strMBR,
                ipmcEntry.ext_portmask_idx,
                diagStr_l2LutStaticOrAutoStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1:0]);
        }
    	break;
	default:
    	break;
    }
}
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
static void
_diag_rtl9603cvd_lutDisplay(rtk_l2_addr_table_t *diag_lut, uint32 displayAddr)
{
    int ret;
    uint32 type;
    uint8 strDip[64];
    uint8 strSip[64];
    uint8 strMBR[64];
    uint8 strExt[64];

    //rtk_l2_ipmcMode_t ipMcHashType;
    rtk_l2_ucastAddr_t   l2UcEntry;
    rtk_l2_mcastAddr_t   l2McEntry;
    rtk_l2_ipMcastAddr_t ipmcEntry;

    switch(diag_lut->entryType)
    {
    case RTK_LUT_L2UC:
		l2UcEntry = diag_lut->entry.l2UcEntry;
        if(displayAddr)
        {
	    	diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2UcEntry.index,(l2UcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
        }
        diag_util_mprintf("L2 Unicast table:\n");
		diag_util_mprintf("MACAddress        Spa Fid Age Vid  State  Ext  Hash\n");
		diag_util_mprintf("%-17s %-3d %-3d %-3d %-4d %-6s %-4d %s\n",
            diag_util_inet_mactoa(&l2UcEntry.mac.octet[0]),
            l2UcEntry.port,
            l2UcEntry.fid,
            l2UcEntry.age,
            l2UcEntry.vid,
            diagStr_l2LutStaticOrAutoStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC) ? 1:0],
            l2UcEntry.ext_port,
            diagStr_l2HashMethodStr[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_IVL) ? 1:0]);

		diag_util_mprintf("CtagIf DaBlock SaBlock Arp\n");
		diag_util_mprintf("%-6s %-7s %-7s %-3s\n",
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_CTAG_IF) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1:0],
            diagStr_enDisplay[(l2UcEntry.flags & RTK_L2_UCAST_FLAG_ARP_USED) ? 1:0]);
    	break;
	case RTK_LUT_L2MC:
		l2McEntry = diag_lut->entry.l2McEntry;
        diag_util_mask2str(strMBR, l2McEntry.portmask.bits[0]);
        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2McEntry.index,(l2McEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
        }
		diag_util_mprintf("L2 Multicast table:\n");
		diag_util_mprintf("MACAddress         Member ExtIdx    Fid_Vid Hash\n");
		diag_util_mprintf("%-18s %-6s %-9d %-7d %-4s\n",
            diag_util_inet_mactoa(&l2McEntry.mac.octet[0]),
            strMBR,
            l2McEntry.ext_portmask_idx,
            (l2McEntry.flags & RTK_L2_MCAST_FLAG_IVL) ? l2McEntry.vid : l2McEntry.fid,
            diagStr_l2HashMethodStr[(l2McEntry.flags & RTK_L2_MCAST_FLAG_IVL) ? 1:0]);
    	break;
	case RTK_LUT_L3MC:
		ipmcEntry = diag_lut->entry.ipmcEntry;
		//rtk_l2_ipmcMode_get(&ipMcHashType);
		ipmcEntry.dip = ipmcEntry.dip | 0xE0000000;
        diag_util_ip2str(strDip, ipmcEntry.dip);
        diag_util_ip2str(strSip, ipmcEntry.sip);
        diag_util_mask2str(strMBR, ipmcEntry.portmask.bits[0]);

        if ((ret = reg_field_read(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_IPMC_HASHf, &type)) != RT_ERR_OK)
        {
            break;
        }

        if(displayAddr)
        {
    		diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcEntry.index,(ipmcEntry.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
        }
		diag_util_mprintf("IP Multicast table:\n");

        if(type == 2) /* IPMC hash mode = GIP + SIP */
        {
            diag_util_mprintf("DestinationIP   SourceIP        Hash Member ExtIdx    State\n");
    		diag_util_mprintf("%-15s %-15s %-4s %-6s %-9d %-6s\n",
                strDip,
                strSip,
                diagStr_l2HashMethodStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? 1:0],
                strMBR,
                ipmcEntry.ext_portmask_idx,
                diagStr_l2LutStaticOrAutoStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1:0]);
        }
        else /* All other using GIP + VID/FID display */
        {
            diag_util_mprintf("DestinationIP   Fid_Vid Hash Member ExtIdx    State\n");
    		diag_util_mprintf("%-15s %-7d %-4s %-6s %-9d %-6s\n",
                strDip,
                (ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? ipmcEntry.vid : ipmcEntry.fid,
                diagStr_l2HashMethodStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_IVL) ? 1:0],
                strMBR,
                ipmcEntry.ext_portmask_idx,
                diagStr_l2LutStaticOrAutoStr[(ipmcEntry.flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1:0]);
        }
    	break;
	default:
    	break;
    }
}
#endif


static int32
_display_ipMcast_entry(rtk_l2_ipMcastAddr_t *pEntry)
{
    int32 ret;
    uint8 strSip[64];
	uint8 strDip[64];
    uint8 strExt[64];
    rtk_l2_ipmcMode_t   mode;
#if defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD) 
    rtk_l2_addr_table_t lut_entry;
#endif

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
    case RTL9602C_CHIP_ID:
        lut_entry.entryType = (pEntry->flags & RTK_L2_IPMCAST_FLAG_IPV6) ? RTK_LUT_L3V6MC : RTK_LUT_L3MC;
        lut_entry.entry.ipmcEntry = *pEntry;
        _diag_rtl9602c_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L3MC;
        lut_entry.entry.ipmcEntry = *pEntry;
        _diag_rtl9607c_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L3MC;
        lut_entry.entry.ipmcEntry = *pEntry;
        _diag_rtl9603cvd_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
    /* For backward compatibility */
    default:
        DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_get(&mode), ret);

        if(pEntry->flags & RTK_L2_IPMCAST_FLAG_IPV6)
        {
            diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);
    		diag_util_mprintf("DestinationIP                            Member FwdPriEn Pri Ext\n");
            diag_util_mprintf("---------------------------------------- ------ -------- --- ---\n");
    		diag_util_mprintf("%-40s %-6s %-8s %-3d %-5s\n",
                diag_util_inet_n6toa(&pEntry->dip6.ipv6_addr[0]),
                diag_util_mask32tostr(pEntry->portmask.bits[0]),
                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1:0],
                pEntry->priority,
                strExt);
        }
        else if(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY)
        {
            diag_util_ip2str(strDip,pEntry->dip);
            diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);

            diag_util_mprintf("DestinationIP   Member Fwd Pri State  Ext    DipOnly ForceExt L3Route L3Idx\n");
            diag_util_mprintf("--------------- ------ --- --- ------ ------ ------- -------- ------- -----\n");

            diag_util_mprintf("%-15s %-6s %-3s %-3d %-6s %-6s %-7s %-8s %-7s %-5d\n",
                                strDip,
                                diag_util_mask32tostr(pEntry->portmask.bits[0]),
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                pEntry->priority,
                                diagStr_l2LutStaticOrAutoStr[(pEntry->flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1 : 0],
                                strExt,
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY) ? 1 : 0],
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE) ? 1 : 0],
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN) ? 1 : 0],
                                pEntry->l3_trans_index);
        }
        else if(LOOKUP_ON_DIP_AND_VID == mode)
        {
            diag_util_ip2str(strDip,pEntry->dip);
            diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);

            diag_util_mprintf("DestinationIP   Vid  Member Fwd Pri State  Ext    DipOnly \n");
            diag_util_mprintf("--------------- ---- ------ --- --- ------ ------ ------- \n");

            diag_util_mprintf("%-15s %-4d %-6s %-3s %-3d %-6s %-6s %-5s\n",
                                strDip,
                                pEntry->vid,
                                diag_util_mask32tostr(pEntry->portmask.bits[0]),
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                pEntry->priority,
                                diagStr_l2LutStaticOrAutoStr[(pEntry->flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1 : 0],
                                strExt,
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY) ? 1 : 0]);
        }
        else /* LOOKUP_ON_DIP_AND_SIP */
        {
            diag_util_ip2str(strDip,pEntry->dip);
            diag_util_ip2str(strSip,pEntry->sip);
            diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);

            diag_util_mprintf("DestinationIP   SourceIP        Member Fwd Pri State  Ext    DipOnly \n");
            diag_util_mprintf("--------------- --------------- ------ --- --- ------ ------ ------- \n");

            diag_util_mprintf("%-15s %-15s %-6s %-3s %-3d %-6s %-6s %-7s\n",
                                strDip,
                                strSip,
                                diag_util_mask32tostr(pEntry->portmask.bits[0]),
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                pEntry->priority,
                                diagStr_l2LutStaticOrAutoStr[(pEntry->flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1 : 0],
                                strExt,
                                diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY) ? 1 : 0]);
        }
        break;
    }

    return RT_ERR_OK;
}

static int32
_display_l2Mcast_entry(rtk_l2_mcastAddr_t *pMcastAddr)
{
    uint8 strExt[64];
#if defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    rtk_l2_addr_table_t lut_entry;
#endif

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
    case RTL9602C_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L2MC;
        lut_entry.entry.l2McEntry = *pMcastAddr;
        _diag_rtl9602c_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L2MC;
        lut_entry.entry.l2McEntry = *pMcastAddr;
        _diag_rtl9607c_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L2MC;
        lut_entry.entry.l2McEntry = *pMcastAddr;
        _diag_rtl9603cvd_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
    /* For backward compatibility */
    default:
        if(pMcastAddr->flags & RTK_L2_MCAST_FLAG_IVL)
        {
            diag_util_mask2str(strExt, pMcastAddr->ext_portmask.bits[0]);

         	diag_util_mprintf("MACAddress         Member VID  FwdPriEn Pri Ext   \n");
            diag_util_mprintf("------------------ ------ ---- -------- --- ------\n");
    		diag_util_mprintf("%-18s %-6s %-4d %-8s %-3d %-6s \n",
    			                    diag_util_inet_mactoa(&pMcastAddr->mac.octet[0]),
    			                    diag_util_mask32tostr(pMcastAddr->portmask.bits[0]),
                                    pMcastAddr->vid,
                                    diagStr_enDisplay[(pMcastAddr->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                    pMcastAddr->priority,
                                    strExt);
        }
        else
        {
            diag_util_mask2str(strExt, pMcastAddr->ext_portmask.bits[0]);

         	diag_util_mprintf("MACAddress         Member FID  FwdPriEn Pri Ext   \n");
            diag_util_mprintf("------------------ ------ ---- -------- --- ------\n");
    		diag_util_mprintf("%-18s %-6s %-4d %-8s %-3d %-6s \n",
    			                    diag_util_inet_mactoa(&pMcastAddr->mac.octet[0]),
    			                    diag_util_mask32tostr(pMcastAddr->portmask.bits[0]),
                                    pMcastAddr->fid,
                                    diagStr_enDisplay[(pMcastAddr->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                    pMcastAddr->priority,
                                    strExt);
        }
        break;
    }

    return RT_ERR_OK;
}

static int32
_display_l2Ucast_entry(rtk_l2_ucastAddr_t *pL2Addr)
{
#if defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    rtk_l2_addr_table_t lut_entry;
#endif

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
    case RTL9602C_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L2UC;
        lut_entry.entry.l2UcEntry = *pL2Addr;
        _diag_rtl9602c_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L2UC;
        lut_entry.entry.l2UcEntry = *pL2Addr;
        _diag_rtl9607c_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        lut_entry.entryType = RTK_LUT_L2UC;
        lut_entry.entry.l2UcEntry = *pL2Addr;
        _diag_rtl9603cvd_lutDisplay(&lut_entry, DISABLED);
        break;
#endif
    /* For backward compatibility */
    default:
        diag_util_mprintf("                  Spa/                             Priority         Block\n");
        diag_util_mprintf("MACAddress        Ext Fid Efid Age Vid  State  LRN Sa  Fwd Pri Auth Da  Sa  Arp CtagIf CtagVid\n");
        diag_util_mprintf("----------------- --- --- ---- --- ---- ------ --- --- --- --- ---- --- --- --- ------ -------\n");
        diag_util_mprintf("%-17s %d/%d %-3d %-4d %-3d %-4d %-6s %-3s %-3s %-3s %-3d %-4s %-3s %-3s %-3s %-6s %-7d\n",
    			                    diag_util_inet_mactoa(&pL2Addr->mac.octet[0]),
    			                    pL2Addr->port,
                                    pL2Addr->ext_port,
    			                    pL2Addr->fid,
    			                    pL2Addr->efid,
    			                    pL2Addr->age,
    			                    pL2Addr->vid,
    			                    diagStr_l2LutStaticOrAutoStr[(pL2Addr->flags & RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0],
    			                    diagStr_l2HashMethodStr[(pL2Addr->flags & RTK_L2_UCAST_FLAG_IVL) ? 1 : 0],
                                    diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_LOOKUP_PRI) ? 1 : 0],
                                    diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_FWD_PRI) ? 1 : 0],
                                    pL2Addr->priority,
                                    diagStr_enDisplay[pL2Addr->auth],
                                    diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0],
                                    diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0],
                                    diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_ARP_USED) ? 1 : 0],
    	 							diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_CTAG_IF) ? 1 : 0],
    	 							pL2Addr->ctag_vid);
        break;
    }

    return RT_ERR_OK;
}

/*
 * l2-table init
 */
cparser_result_t
cparser_cmd_l2_table_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    efid = 0;
    DIAG_UTIL_ERR_CHK(rtk_l2_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_init */

/*
 * l2-table del all { include-static }
 */
cparser_result_t
cparser_cmd_l2_table_del_all_include_static(
    cparser_context_t *context)
{
    int32 ret;
    uint32 includeStatic;

    DIAG_UTIL_PARAM_CHK();

    if(4 == TOKEN_NUM())
    {
        includeStatic = 1;
    }
    else
    {
        includeStatic = 0;
    }
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_delAll(includeStatic), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_all_include_static */

/*
 * l2-table del ip-mcast dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_dip_dip(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_dip_dip */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all */
/*
 * l2-table get ip-mcast dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_dip_dip(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_dip_dip */
/*
 * l2-table del ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_vid_vid_dip_dip(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_vid_vid_dip_dip */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_port_ports_all_none */
/*
 * l2-table get ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_vid_vid_dip_dip(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_vid_vid_dip_dip */
/*
 * l2-table del mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_mcast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_del(&l2Mcast), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_mcast_vid_vid_mac_address_mac */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_port_ports_all_none */

/*
 * l2-table del mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_mcast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_del(&l2Mcast), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_mcast_filter_id_fid_mac_address_mac */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_port_ports_all_none */

/*
 * l2-table get mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_mcast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_get(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Mcast_entry(&l2Mcast);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_mcast_filter_id_fid_mac_address_mac */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none ) l3-interface <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all_none_l3_interface_index(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.l3_trans_index = *index_ptr;
    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all_l3_interface_index */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_priority(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_priority */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_state_disable_enable */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_ext_ext_all_none */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_l3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> forcedl3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_forcedl3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_forcedl3routing_state_disable_enable */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_ext_port_idx_index */

/*
 * l2-table del ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_sip_sip_dip_dip(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_sip_sip_dip_dip */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_port_ports_all_none */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_priority(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    ipmcAddr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_priority */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    if('e' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_state_disable_enable */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_ext_ext_all_none */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    ipmcAddr.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_ext_port_idx_index */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_port_ports_all */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 11), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_port_ports_all */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_l3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    if('e' == TOKEN_CHAR(12, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(12, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> port ( <PORT_LIST:ports> | all | none ) l3-interface <UINT:l3index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_port_ports_all_none_l3_interface_l3index(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr,
    char * *ports_ptr,
    uint32_t  *l3index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    ipmcAddr.l3_trans_index = *l3index_ptr;
    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index_port_ports_all_l3_interface_index */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_port_ports_all */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_port_ports_all */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_ext_port_idx_index */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_l3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none ) l3-interface <UINT:l3index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_port_ports_all_none_l3_interface_l3index(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr,
    uint32_t  *l3index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.fid = (uint32)(*fid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.l3_trans_index = *l3index_ptr;
    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_filter_id_fid_dip_dip_port_ports_all_l3_interface_index */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 11), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.vid = (uint32)(*vid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_none */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.vid = (uint32)(*vid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_port_ports_all */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_l3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.vid = (uint32)(*vid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    if('e' == TOKEN_CHAR(12, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(12, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index> port ( <PORT_LIST:ports> | all | none ) l3-interface <UINT:l3index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_port_ports_all_none_l3_interface_l3index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr,
    char * *ports_ptr,
    uint32_t  *l3index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
        ipmcAddr.vid = (uint32)(*vid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip_index = (uint32)(*index_ptr);
    }

    ipmcAddr.l3_trans_index = *l3index_ptr;
    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index_port_ports_all_l3_interface_index */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_l3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.vid = (uint32)(*vid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none ) l3-interface <UINT:l3index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_port_ports_all_none_l3_interface_l3index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr,
    uint32_t  *l3index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.vid = (uint32)(*vid_ptr);
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.l3_trans_index = *l3index_ptr;
    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_port_ports_all_l3_interface_l3index */

/*
 * l2-table get ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_sip_sip_dip_dip(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_sip_sip_dip_dip */

/*
 * l2-table get ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index */

/*
 * l2-table get ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_filter_id_fid_dip_dip(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_filter_id_fid_dip_dip */

/*
 * l2-table get ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index */

/*
 * l2-table del ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_filter_id_fid_dip_dip_sip_filter_sip_index_index */

/*
 * l2-table del ip-mcast filter-id <UINT:fid> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_filter_id_fid_dip_dip(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.fid = (uint32)(*fid_ptr);
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_filter_id_fid_dip_dip */

/*
 * l2-table del ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> sip-filter sip-index <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_SIP_FILTER;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ipmcAddr.sip_index = (uint32)(*index_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_vid_vid_dip_dip_sip_filter_sip_index_index */

/*
 * l2-table add ip-mcast dip6 <IPV6ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip6_dip_port_ports_all_none(
    cparser_context_t *context,
    char * *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
    diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
        diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip6_dip_port_ports_all_none */

/*
 * l2-table add ip-mcast dip6 <IPV6ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip6_dip_ext_ext_all_none(
    cparser_context_t *context,
    char * *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
    diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
        diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip6_dip_ext_ext_all_none */

/*
 * l2-table add ip-mcast dip6 <IPV6ADDR:dip> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip6_dip_l3routing_state_disable_enable(
    cparser_context_t *context,
    char * *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
    diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
        diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip6_dip_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast dip6 <IPV6ADDR:dip> l3table <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip6_dip_l3table_index(
    cparser_context_t *context,
    char * *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
    diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
        diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    }

    ipmcAddr.l3_mcr_index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip6_dip_l3table_index */

/*
 * l2-table get ip-mcast dip6 <IPV6ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_dip6_dip(
    cparser_context_t *context,
    char * *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
    diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_dip6_dip */

/*
 * l2-table del ip-mcast dip6 <IPV6ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_dip6_dip(
    cparser_context_t *context,
    char * *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IPV6;
    diag_util_str2ipv6(&ipmcAddr.dip6.ipv6_addr[0], TOKEN_STR(4));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_dip6_dip */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    ipmcAddr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_priority */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    if('e' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_state_disable_enable */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_ext_ext_all_none */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_IVL;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    ipmcAddr.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);
    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_ext_port_idx_index */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Mcast.flags &= ~RTK_L2_MCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_ext_ext_all_none */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_ext_port_idx_index */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Mcast.flags &= ~RTK_L2_MCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_ext_ext_all_none */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_ext_ext_all_none */

/*
 * l2-table add mac-mcast mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_mac_address_mac_port_ports_all_none(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_mac_address_mac_port_ports_all_none */

/*
 * l2-table add mac-mcast mac-address <MACADDR:mac> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_mac_address_mac_ext_port_idx_index(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.ext_portmask_idx = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_mac_address_mac_ext_port_idx_index */

/*
 * l2-table get mac-mcast mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_mcast_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_get(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    _display_l2Mcast_entry(&l2Mcast);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_mcast_mac_address_mac */

/*
 * l2-table del mac-mcast mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_mcast_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_del(&l2Mcast), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_mcast_vid_vid_mac_address_mac */
#endif

/*
 * l2-table get mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_mcast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_get(&l2Mcast), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    _display_l2Mcast_entry(&l2Mcast);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_mcast_vid_vid_mac_address_mac */

/*
 * l2-table del mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_ucast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_del(&l2Addr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_ucast_vid_vid_mac_address_mac */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_spn_port(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_spn_port */


/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> age <UINT:age>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_age_age(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *age_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.age = *age_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_age_age */
/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> static state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_static_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_STATIC;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_static_state_disable_enable */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> filter-id <UINT:fid>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_filter_id_fid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.fid = *fid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_filter_id_fid */

/*
 * l2-table set mac-ucast enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_set_mac_ucast_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32 *efid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    efid = *efid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_mac_ucast_enhanced_filter_id_efid */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> sa-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_priority_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> arp-usage state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_arp_usage_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_ARP_USED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_ARP_USED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_arp_usage_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> auth state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_auth_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.auth = ENABLED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.auth = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_auth_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> da-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_da_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_DA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_da_block_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> sa-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_SA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_block_state_disable_enable */


/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> ext-spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ext_spn_port(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.ext_port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ext_spn_port */
#endif

/*
 * l2-table del mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_ucast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_del(&l2Addr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_ucast_filter_id_fid_mac_address_mac */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_spn_port(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_spn_port */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_vid_vid(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_vid_vid */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> age <UINT:age>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_age_age(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *age_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.age = *age_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_age_age */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> static state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_static_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_STATIC;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_static_state_disable_enable */


#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> sa-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_priority_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> arp-usage state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_arp_usage_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_ARP_USED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_ARP_USED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_arp_usage_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> auth state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_auth_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.auth = ENABLED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.auth = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_auth_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> da-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_da_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_DA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_da_block_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> sa-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_SA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_block_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> ext-spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ext_spn_port(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    l2Addr.ext_port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ext_spn_port */

/*
 * l2-table get mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = *efid_ptr;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac_enhanced_filter_id_efid */

/*
 * l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = *efid_ptr;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac_enhanced_filter_id_efid */
#endif
/*
 * l2-table get mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac */


/*
 * l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);
    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac */

/*
 * l2-table get aging-out port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_l2_table_get_aging_out_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portAgingEnable_get(port, &state), ret);
        diag_util_printf("\n Port %d Age state: %s", port, diagStr_enable[state]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_aging_out_port_ports_all_state */

/*
 * l2-table set aging-out port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_aging_out_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portAgingEnable_set(port, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_aging_out_port_ports_all_state_disable_enable */

/*
 * l2-table get aging-time
 */
cparser_result_t
cparser_cmd_l2_table_get_aging_time(
    cparser_context_t *context)
{
    int32 ret;
    uint32 time;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_aging_get(&time), ret);
    diag_util_printf("\n Age Time: %d", time);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_aging_time */

/*
 * l2-table set aging-time <UINT:time>
 */
cparser_result_t
cparser_cmd_l2_table_set_aging_time_time(
    cparser_context_t *context,
    uint32_t  *time_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_aging_set(*time_ptr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_aging_time_time */

/*
 * l2-table get cam state
 */
cparser_result_t
cparser_cmd_l2_table_get_cam_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_camEnable_get(&state), ret);
            diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_camEnable_get(&state), ret);
            diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtk_l2_camState_get(&state), ret);
			diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
			break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtk_l2_camState_get(&state), ret);
			diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
			break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtk_l2_camState_get(&state), ret);
			diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
			break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_camState_get(&state), ret);
            diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
            break;
#endif
        case UNKNOWN_CHIP_ID:
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_cam_state */

/*
 * l2-table set cam state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_cam_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_camEnable_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_camEnable_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtk_l2_camState_set(state), ret);
			break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtk_l2_camState_set(state), ret);
			break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtk_l2_camState_set(state), ret);
			break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_camState_set(state), ret);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_cam_state_disable_enable */

/*
 * l2-table get cam full-act
 */
cparser_result_t
cparser_cmd_l2_table_get_cam_full_act(
    cparser_context_t *context)
{
    int32 ret;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9602C
	case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_LUT_CFGr, RTL9602C_LUT_ENTRY_FULL_ACTf, &value), ret);

        diag_util_mprintf("cam full action: %s\n", (0 == value) ? "forward" : "trap to cpu");
		break;
#endif    
#ifdef CONFIG_SDK_RTL9607C
	case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_LUT_CFGr, RTL9607C_LUT_ENTRY_FULL_ACTf, &value), ret);

        diag_util_mprintf("cam full action: %s\n", (0 == value) ? "forward" : "trap to cpu");
		break;
#endif  
#ifdef CONFIG_SDK_RTL9603CVD
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_ENTRY_FULL_ACTf, &value), ret);

        diag_util_mprintf("cam full action: %s\n", (0 == value) ? "forward" : "trap to cpu");
        break;
#endif    
	default:
		diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
		return CPARSER_NOT_OK;		  
		break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_cam_full_act */

/*
 * l2-table set cam full-act ( forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_cam_full_act_forward_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9602C
	case RTL9602C_CHIP_ID:
        if('f' == TOKEN_CHAR(4, 0))
        {
            value = 0;
        }
        else if('t' == TOKEN_CHAR(4, 0))
        {
            value = 1;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_LUT_CFGr, RTL9602C_LUT_ENTRY_FULL_ACTf, &value), ret);
		break;
#endif    
#ifdef CONFIG_SDK_RTL9607C
	case RTL9607C_CHIP_ID:
        if('f' == TOKEN_CHAR(4, 0))
        {
            value = 0;
        }
        else if('t' == TOKEN_CHAR(4, 0))
        {
            value = 1;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_LUT_CFGr, RTL9607C_LUT_ENTRY_FULL_ACTf, &value), ret);
		break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
    case RTL9603CVD_CHIP_ID:
        if('f' == TOKEN_CHAR(4, 0))
        {
            value = 0;
        }
        else if('t' == TOKEN_CHAR(4, 0))
        {
            value = 1;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_ENTRY_FULL_ACTf, &value), ret);
        break;
#endif    

	default:
		diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
		return CPARSER_NOT_OK;		  
		break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_cam_full_act_forward_trap_to_cpu */

/*
 * l2-table get limit-learning action
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_limitLearnCntAction_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_get(&action), ret);

    diag_util_printf("\n System learning Over Action: %s", diagStr_actionStr[action]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_action */

/*
 * l2-table set limit-learning action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    if('c' == TOKEN_CHAR(4, 0))
        action = ACTION_COPY2CPU;
    else if('d' == TOKEN_CHAR(4, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(4, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(4, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_set((rtk_l2_limitLearnCntAction_t)action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_action_copy_to_cpu_drop_forward_trap_to_cpu */

/*
 * l2-table get limit-learning count
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_count(
    cparser_context_t *context)
{
    int32 ret;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_get(&cnt), ret);

    diag_util_printf("\n System Learning Limit: %d\n", cnt);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_count */

/*
 * l2-table set limit-learning count <UINT:count>
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_count_count(
    cparser_context_t *context,
    uint32_t  *count_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_set(*count_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_count_count */

/*
 * l2-table set limit-learning count unlimited
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_count_unlimited(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_set(HAL_L2_LEARN_LIMIT_CNT_MAX()), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_count_unlimited */

/*
 * l2-table get limit-learning entry action
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_entry_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_limitLearnEntryAction_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningEntryAction_get(&action), ret);

    diag_util_printf("\n Entry learning Over Action: %s", diagStr_actionStr[action]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_entry_action */

/*
 * l2-table set limit-learning entry action ( forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_entry_action_forward_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(5, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(5, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningEntryAction_set((rtk_l2_limitLearnEntryAction_t)action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_entry_action_forward_trap_to_cpu */

/*
 * l2-table get limit-learning include
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_include(
    cparser_context_t *context)
{
    int ret;
    rtk_portmask_t portmask;
    uint8  port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningPortMask_get(&portmask), ret);
            diag_util_lPortMask2str(port_list, &portmask);
            diag_util_printf("System learning limit include port: %s\n", port_list);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningPortMask_get(&portmask), ret);
            diag_util_lPortMask2str(port_list, &portmask);
            diag_util_printf("System learning limit include port: %s\n", port_list);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningPortMask_get(&portmask), ret);
            diag_util_lPortMask2str(port_list, &portmask);
            diag_util_printf("System learning limit include port: %s\n", port_list);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_include */

/*
 * l2-table set limit-learning include ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_include_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningPortMask_set(portlist.portmask), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningPortMask_set(portlist.portmask), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningPortMask_set(portlist.portmask), ret);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_include_ports_all */

/*
 * l2-table get limit-learning port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_limitLearnCntAction_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_get(port, &action), ret);
        diag_util_printf("\n Port %d learning limit over action: %s", port, diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_port_ports_all_action */

/*
 * l2-table set limit-learning port ( <PORT_LIST:ports> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_limitLearnCntAction_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('c' == TOKEN_CHAR(6, 0))
        action = LIMIT_LEARN_CNT_ACTION_COPY_CPU;
    else if('d' == TOKEN_CHAR(6, 0))
        action = LIMIT_LEARN_CNT_ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        action = LIMIT_LEARN_CNT_ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(6, 0))
        action = LIMIT_LEARN_CNT_ACTION_TO_CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_set(port, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu */

/*
 * l2-table get limit-learning port ( <PORT_LIST:ports> | all ) count
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_port_ports_all_count(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_get(port, &cnt), ret);
        diag_util_printf("\n Port %d learning limit: %d", port, cnt);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_port_ports_all_count */

/*
 * l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count <UINT:count>
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_count(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *count_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_set(port, *count_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_count */

/*
 * l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count unlimited
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_unlimited(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_set(port, HAL_L2_LEARN_LIMIT_CNT_MAX()), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_unlimited */

/*
 * l2-table get learning-count
 */
cparser_result_t
cparser_cmd_l2_table_get_learning_count(
    cparser_context_t *context)
{
    int32 ret;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_learningCnt_get(&cnt), ret);

    diag_util_printf("\n System Learning Counter: %d\n", cnt);


    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_learning_count */

/*
 * l2-table get learning-count port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_get_learning_count_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLearningCnt_get(port, &cnt), ret);
        diag_util_printf("\n Port %d learning counter: %d", port, cnt);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_learning_count_port_ports_all */



/*
 * l2-table get link-down-flush state
 */
cparser_result_t
cparser_cmd_l2_table_get_link_down_flush_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_get(&state), ret);
    diag_util_printf("\n Link down flush state: %s", diagStr_enable[state]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_link_down_flush_state */

/*
 * l2-table set link-down-flush state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_link_down_flush_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_set(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_link_down_flush_state_disable_enable */

/*
 * l2-table get lookup-miss multicast trap-priority
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_multicast_trap_priority(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pri_t priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_get(TRAP_REASON_MULTICASTDLF, &priority), ret);
    diag_util_printf("\n Lookup-miss multicast trap-priority: %d", priority);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_multicast_trap_priority */

/*
 * l2-table set lookup-miss multicast trap-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_multicast_trap_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_set(TRAP_REASON_MULTICASTDLF, (rtk_pri_t)*priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_multicast_trap_priority_priority */

/*
 * l2-table get lookup-miss multicast reserved-flooding
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_multicast_reserved_flooding(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_unknReservedMcFlood_get(&state), ret);
            diag_util_printf("\n Unknown reserved IP multicast Flooding: %s", diagStr_enable[state]);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtl9601b_raw_l2_unknReservedMcFlood_get(&state), ret);
			diag_util_printf("\n Unknown reserved IP multicast Flooding: %s", diagStr_enable[state]);
			break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
            rtl9602c_l2_resAct_t act;
            DIAG_UTIL_ERR_CHK(rtl9602c_l2_unknReservedIpv4McAction_get(&act), ret);
            diag_util_printf("\n Unknown reserved IPv4 multicast action: %s", diagStr_l2unknownMcAct[act]);
            DIAG_UTIL_ERR_CHK(rtl9602c_l2_unknReservedIpv6McAction_get(&act), ret);
            diag_util_printf("\n Unknown reserved IPv6 multicast action: %s", diagStr_l2unknownMcAct[act]);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
            rtl9607c_l2_resAct_t act;
            DIAG_UTIL_ERR_CHK(rtl9607c_l2_unknReservedIpv4McAction_get(&act), ret);
            diag_util_printf("\n Unknown reserved IPv4 multicast action: %s", diagStr_l2unknownMcAct[act]);
            DIAG_UTIL_ERR_CHK(rtl9607c_l2_unknReservedIpv6McAction_get(&act), ret);
            diag_util_printf("\n Unknown reserved IPv6 multicast action: %s", diagStr_l2unknownMcAct[act]);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtl9603cvd_l2_resAct_t act;
            DIAG_UTIL_ERR_CHK(rtl9603cvd_l2_unknReservedIpv4McAction_get(&act), ret);
            diag_util_printf("\n Unknown reserved IPv4 multicast action: %s", diagStr_l2unknownMcAct[act]);
            DIAG_UTIL_ERR_CHK(rtl9603cvd_l2_unknReservedIpv6McAction_get(&act), ret);
            diag_util_printf("\n Unknown reserved IPv6 multicast action: %s", diagStr_l2unknownMcAct[act]);
            break;
        }
#endif
    case UNKNOWN_CHIP_ID:
        break;
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
        break;
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_multicast_reserved_flooding */

/*
 * l2-table set lookup-miss multicast reserved-flooding ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_multicast_reserved_flooding_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else if('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else
        return CPARSER_NOT_OK;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_unknReservedMcFlood_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			DIAG_UTIL_ERR_CHK(rtl9601b_raw_l2_unknReservedMcFlood_set(state), ret);
			break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
        {
			rtk_action_t action;
            if(ENABLED == state)
                action = ACTION_FLOOD_IN_VLAN;
            else
                action = ACTION_FORWARD;

            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_RSVIPMC,action), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_RSVIP6MC,action), ret);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
        {
			rtk_action_t action;
            if(ENABLED == state)
				action = ACTION_FLOOD_IN_VLAN;
			else
				action = ACTION_FORWARD;
				
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_RSVIPMC,action), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_RSVIP6MC,action), ret);
            break;
        }
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
        {
            rtk_action_t action;
            if(ENABLED == state)
                action = ACTION_FLOOD_IN_VLAN;
            else
                action = ACTION_FORWARD;
                
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_RSVIPMC,action), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_RSVIP6MC,action), ret);
            break;
        }
#endif

    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_multicast_reserved_flooding_disable_enable */

/*
 * l2-table get lookup-miss multicast reserved-action ( ipv4 | ipv6 ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_multicast_reserved_action_ipv4_ipv6_action(
    cparser_context_t *context)
{
    int32 ret;
	rtk_l2_lookupMissType_t type;
	rtk_action_t action;
	uint32 actDal;
	
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('4' == TOKEN_CHAR(5, 3))
        type = DLF_TYPE_RSVIPMC;
    else if('6' == TOKEN_CHAR(5, 3))
        type = DLF_TYPE_RSVIP6MC;
    else
        return CPARSER_NOT_OK;

	DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_get(type,&action), ret);

	diag_util_printf("\n Unknown reserved IPv%d multicast action: ",(type==DLF_TYPE_RSVIPMC)?4:6);

	switch(action)
	{
		case ACTION_TRAP2CPU:
			actDal = 2;
			break;
		case ACTION_FLOOD_IN_VLAN:
			actDal = 1;
			break;
		case ACTION_FORWARD:
			actDal = 0;
			break;
		default:
			diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;
	}

	diag_util_printf("%s\n",diagStr_l2unknownMcAct[actDal]);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_multicast_reserved_action_ipv4_ipv6_action */

/*
 * l2-table set lookup-miss multicast reserved-action ( ipv4 | ipv6 ) action ( normal-uknown-mc | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_multicast_reserved_action_ipv4_ipv6_action_normal_uknown_mc_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret;
    uint8 ipVer;
	rtk_l2_lookupMissType_t type;
	rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    if('4' == TOKEN_CHAR(5, 3))
        type = DLF_TYPE_RSVIPMC;
    else if('6' == TOKEN_CHAR(5, 3))
        type = DLF_TYPE_RSVIP6MC;
    else
        return CPARSER_NOT_OK;

	if('n' == TOKEN_CHAR(7, 0))
		action = ACTION_FORWARD;
	else if('f' == TOKEN_CHAR(7, 0))
		action = ACTION_FLOOD_IN_VLAN;
	else if('t' == TOKEN_CHAR(7, 0))
		action = ACTION_TRAP2CPU;
	else
		return CPARSER_NOT_OK;


	DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(type,action), ret);
			

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_multicast_reserved_action_ipv4_ipv6_action_normal_uknown_mc_flood_in_vlan_trap_to_cpu */

/*
 * l2-table get lookup-miss multicast reserved-addr ipv6
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_multicast_reserved_addr_ipv6(
    cparser_context_t *context)
{
    int32 ret;
	rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_get(LUT_IP6MCADDR_RSVDOC_FF01, &enable), ret);
	diag_util_printf("\nFF01::0/112 or FF01::DB8:0:0/96 treat as: %s\n", diagStr_l2rsvdAddr[enable ? 1 : 0]);
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_get(LUT_IP6MCADDR_RSVDOC_FF02, &enable), ret);
	diag_util_printf("FF02::0/112 or FF02::DB8:0:0/96 treat as: %s\n", diagStr_l2rsvdAddr[enable ? 1 : 0]);
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_get(LUT_IP6MCADDR_RSVDOC_FF05, &enable), ret);
	diag_util_printf("FF05::0/112 or FF05::DB8:0:0/96 treat as: %s\n", diagStr_l2rsvdAddr[enable ? 1 : 0]);
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_get(LUT_IP6MCADDR_RSVDOC_FF08, &enable), ret);
	diag_util_printf("FF08::0/112 or FF08::DB8:0:0/96 treat as: %s\n", diagStr_l2rsvdAddr[enable ? 1 : 0]);
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_get(LUT_IP6MCADDR_RSVDOC_FF0E, &enable), ret);
	diag_util_printf("FF0e::0/112 or FF0e::DB8:0:0/96 treat as: %s\n", diagStr_l2rsvdAddr[enable ? 1 : 0]);
    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_get(LUT_IP6MCADDR_SOILCITED_NOTE, &enable), ret);
	diag_util_printf("solicited-node multicast address ff02::1:ff00:0/104 treat as: %s\n", diagStr_l2rsvdAddr[enable ? 1 : 0]);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_multicast_reserved_addr_ipv6 */

/*
 * l2-table set lookup-miss multicast reserved-addr ipv6 ( 1 | 2 | 5 | 8 | e | solicited-node ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_multicast_reserved_addr_ipv6_1_2_5_8_e_solicited_node_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret;
	rtk_enable_t enable;
	rtk_l2_ip6McReservedAddr_t type;

    DIAG_UTIL_PARAM_CHK();

    if('1' == TOKEN_CHAR(6, 0))
        type = LUT_IP6MCADDR_RSVDOC_FF01;
    else if('2' == TOKEN_CHAR(6, 0))
        type = LUT_IP6MCADDR_RSVDOC_FF02;
    else if('5' == TOKEN_CHAR(6, 0))
        type = LUT_IP6MCADDR_RSVDOC_FF05;
    else if('8' == TOKEN_CHAR(6, 0))
        type = LUT_IP6MCADDR_RSVDOC_FF08;
    else if('e' == TOKEN_CHAR(6, 0))
        type = LUT_IP6MCADDR_RSVDOC_FF0E;
    else if('s' == TOKEN_CHAR(6, 0))
        type = LUT_IP6MCADDR_SOILCITED_NOTE;
    else
        return CPARSER_NOT_OK;

	if('e' == TOKEN_CHAR(8, 0))
	{
		enable = ENABLED;
	}
	else if('d' == TOKEN_CHAR(8, 0))
	{
		enable = DISABLED;
	}
	else
	{
		return CPARSER_NOT_OK;
	}

    DIAG_UTIL_ERR_CHK(rtk_l2_ip6mcReservedAddrEnable_set(type, enable), ret);

    return CPARSER_OK;
}

/*
 * l2-table get lookup-miss multicast trap ( icmpv6 | dhcpv6 ) state
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_multicast_trap_icmpv6_dhcpv6_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable;
	rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	if('i' == TOKEN_CHAR(5, 0))
	{
		DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_get(DLF_TYPE_ICMP6MC,&action), ret);

		if(action == ACTION_TRAP2CPU)
			enable = ENABLED;
		else
			enable = DISABLED;
		
		diag_util_printf("\n Unknown multicast ICMPv6 trap action: %s\n", diagStr_enable[enable]);
	}
	else if('d' == TOKEN_CHAR(5, 0))
	{
		DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_get(DLF_TYPE_DHCP6MC,&action), ret);
		if(action == ACTION_TRAP2CPU)
			enable = ENABLED;
		else
			enable = DISABLED;
		
		diag_util_printf("\n Unknown multicast DHCPv6 trap action: %s\n", diagStr_enable[enable]);
	}
	else
	{
		return CPARSER_NOT_OK;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_multicast_trap_icmpv6_dhcpv6_state */

/*
 * l2-table set lookup-miss multicast trap ( icmpv6 | dhcpv6 ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_multicast_trap_icmpv6_dhcpv6_state_enable_disable(
    cparser_context_t *context)
{
    int32 ret;
	rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else if('d' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else
        return CPARSER_NOT_OK;

	if('i' == TOKEN_CHAR(5, 0))
	{
		DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_ICMP6MC,action), ret);
	}
	else if('d' == TOKEN_CHAR(5, 0))
	{
		DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissAction_set(DLF_TYPE_DHCP6MC,action), ret);
	}
	else
	{
		return CPARSER_NOT_OK;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_multicast_trap_icmpv6_dhcpv6_state_enable_disable */

/*
 * l2-table get lookup-miss ( broadcast | unicast | multicast ) flood-ports
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_broadcast_unicast_multicast_flood_ports(
    cparser_context_t *context)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('b' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_BCAST;
    else if('u' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_UCAST;
    else if('m' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_MCAST;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_get(type, &portmask), ret);
    diag_util_printf("\n %s Lookup miss flood portmask: %s", diagStr_lookupmissType[type], diag_util_mask32tostr(portmask.bits[0]));
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_broadcast_unicast_multicast_flood_ports */

/*
 * l2-table set lookup-miss ( broadcast | unicast | multicast ) flood-ports ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_broadcast_unicast_multicast_flood_ports_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if('b' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_BCAST;
    else if('u' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_UCAST;
    else if('m' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_MCAST;
    else
        return CPARSER_NOT_OK;

    RTK_PORTMASK_FROM_UINT_PORTMASK((&portmask), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_set(type, &portmask), ret);

    if(DLF_TYPE_MCAST == type)
        DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_IPMC, &portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_broadcast_unicast_multicast_flood_ports_ports_all_none */

/*
 * l2-table get lookup-miss port ( <PORT_LIST:ports> | all ) ( multicast | ip-mcast | ip6-mcast | unicast | broadcast) action
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_port_ports_all_multicast_ip_mcast_ip6_mcast_unicast_broadcast_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('l' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_MCAST;
    else if('-' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IPMC;
    else if('6' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IP6MC;
    else if('i' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_UCAST;
    else if('o' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_BCAST;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_get(port, type, &action), ret);
        if(DLF_TYPE_MCAST == type)
            diag_util_printf("\n Port %d lookup-miss multicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_IPMC == type)
            diag_util_printf("\n Port %d lookup-miss IPv4 multicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_IP6MC == type)
            diag_util_printf("\n Port %d lookup-miss IPv6 multicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_UCAST == type)
            diag_util_printf("\n Port %d lookup-miss Unicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_BCAST == type)
            diag_util_printf("\n Port %d lookup-miss Broadcast Action: %s", port, diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_port_ports_all_multicast_ip_mcast_ip6_mcast_unicast_action */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) ( ip-mcast | ip6-mcast ) action ( drop | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_ip_mcast_ip6_mcast_action_drop_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('-' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IPMC;
    else if('6' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IP6MC;
    else
        return CPARSER_NOT_OK;

    if('d' == TOKEN_CHAR(7, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, type, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_ip_mcast_ip6_mcast_action_drop_flood_in_vlan_trap_to_cpu */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) multicast action ( drop | drop-exclude-rma | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_multicast_action_drop_drop_exclude_rma_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('d' == TOKEN_CHAR(7, 0))
    {
        if(!osal_strcmp(TOKEN_STR(7),"drop-exclude-rma"))
            action = ACTION_DROP_EXCLUDE_RMA;
        else if(!osal_strcmp(TOKEN_STR(7),"drop"))
            action = ACTION_DROP;
        else
            return CPARSER_NOT_OK;
    }
    else if('f' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, DLF_TYPE_MCAST, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_multicast_action_drop_drop_exclude_rma_flood_in_vlan_trap_to_cpu */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) unicast action ( drop | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_unicast_action_drop_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('d' == TOKEN_CHAR(7, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, DLF_TYPE_UCAST, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_unicast_action_drop_flood_in_vlan_trap_to_cpu */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) broadcast action ( flood-in-vlan | trap-to-cpu | follow-fb )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_broadcast_action_flood_in_vlan_trap_to_cpu_follow_fb(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('l' == TOKEN_CHAR(7, 1))
        action = ACTION_FORWARD;
    else if('r' == TOKEN_CHAR(7, 1))
        action = ACTION_TRAP2CPU;
    else if('o' == TOKEN_CHAR(7, 1))
        action = ACTION_FOLLOW_FB;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, DLF_TYPE_BCAST, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_unicast_action_drop_flood_in_vlan_trap_to_cpu */

/*
 * l2-table get lookup-hit unicast forwarding
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_hit_unicast_forwarding(
    cparser_context_t *context)
{
    int32 ret;
    uint32 action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
	case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_LUT_CFGr, RTL9607C_LUT_L2UC_ACTf, &action), ret);
        diag_util_printf("\n forwarding decision:");
        switch(action)
        {
        case 0:
            diag_util_printf("lut");
            break;
        case 1:
            diag_util_printf("fb");
            break;
        case 2:
            diag_util_printf("fb-lut");
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            break;
        }
        diag_util_printf("\n");        
		break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_L2UC_ACTf, &action), ret);

        diag_util_printf("\n forwarding decision:");
        switch(action)
        {
        case 0:
            diag_util_printf("lut");
            break;
        case 1:
            diag_util_printf("fb");
            break;
        case 2:
            diag_util_printf("fb-lut");
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            break;
        }
        diag_util_printf("\n");        
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_hit_unicast_forwarding */

/*
 * l2-table set lookup-hit unicast forwarding ( lut | fb | fb-lut )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_hit_unicast_forwarding_lut_fb_fb_lut(
    cparser_context_t *context)
{
    int32 ret;
    uint32 action;

    DIAG_UTIL_PARAM_CHK();

    if('-' == TOKEN_CHAR(5, 2))
        action = 2;
    else if('l' == TOKEN_CHAR(5, 0))
        action = 0;
    else if('f' == TOKEN_CHAR(5, 0))
        action = 1;
    else
        return CPARSER_NOT_OK;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
	case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_LUT_CFGr, RTL9607C_LUT_L2UC_ACTf, &action), ret);
		break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_L2UC_ACTf, &action), ret);
        break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_hit_unicast_forward_lut_fb_fb_lut */

/*
 * l2-table get lookup-hit wan-ports
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_hit_wan_ports(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    rtk_port_t port = 0;
    rtk_portmask_t lPortMask;
    uint8  port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    lPortMask.bits[0] = 0;
    HAL_SCAN_ALL_PORT(port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_RTL9607C)
    	case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9607C_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &value), ret);
            if( 1 == value)
            {
                lPortMask.bits[0] |= (1 << port);
            }
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9603CVD_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf, &value), ret);
            if( 1 == value)
            {
                lPortMask.bits[0] |= (1 << port);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
        }
    }

    diag_util_lPortMask2str(port_list, &lPortMask);
    diag_util_mprintf("WAN Ports: %s\n", port_list);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_hit_wan_ports */

/*
 * l2-table set lookup-hit wan-port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_hit_wan_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

	if(RTK_PORTMASK_GET_PORT_COUNT(portlist.portmask) != 0)
	{
        value = 1;
	    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	    {
			if(HAL_IS_PORT_EXIST(port))
			{
                switch(DIAG_UTIL_CHIP_TYPE)
                {
#if defined(CONFIG_SDK_RTL9607C)
            	case RTL9607C_CHIP_ID:
                    DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &value), ret);
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf, &value), ret);
                    break;
#endif
                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                }
			}
	    }
		
        value = 0;
	    DIAG_UTIL_PORTMASK_UNSETSCAN(portlist, port)
	    {
			if(HAL_IS_PORT_EXIST(port))
			{
                switch(DIAG_UTIL_CHIP_TYPE)
                {
#if defined(CONFIG_SDK_RTL9607C)
            	case RTL9607C_CHIP_ID:
                    DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &value), ret);
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf, &value), ret);
                    break;
#endif

                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                }
			}
	    }
		
	}
    else
   	{
        value = 0;
	    DIAG_UTIL_PORTMASK_UNSETSCAN(portlist, port)
	    {
			if(HAL_IS_PORT_EXIST(port))
			{
                switch(DIAG_UTIL_CHIP_TYPE)
                {
#if defined(CONFIG_SDK_RTL9607C)
            	case RTL9607C_CHIP_ID:
                    DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ENf, &value), ret);
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_LUT_WAN_ENr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ENf, &value), ret);
                    break;
#endif
                default:
                    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                    return CPARSER_NOT_OK;
                }
			}
	    }
   	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_hit_wan_port_ports */

/*
 * l2-table set ip-mcast-mode ( dip | dip-and-sip | dip-and-vid | vid-and-mac | dip-and-cvid | dip-and-fid-vid )
 */

cparser_result_t
cparser_cmd_l2_table_set_ip_mcast_mode_dip_dip_and_sip_dip_and_vid_vid_and_mac_dip_and_cvid_dip_and_fid_vid(
	cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcMode_t mode;

    DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp("dip",TOKEN_STR(3)))
        mode = LOOKUP_ON_DIP;
	else if(!osal_strcmp("dip-and-cvid",TOKEN_STR(3)))
		mode = LOOKUP_ON_DIP_AND_CVID;
    else if('s' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_SIP;
    else if('v' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_VID;
    else if('m' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_MAC_AND_VID_FID;
    else if('f' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_VID_FID;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_ip_mcast_mode_dip_dip_and_sip_dip_and_vid_vid_and_mac_dip_and_cvid_dip_and_fid_vid */

/*
 * l2-table get ip-mcast-mode
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_mode(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_get(&mode), ret);

    diag_util_printf("\n IPMC mode : %s", diagStr_l2IpMcHashMethodStr[mode]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_mode */

/*
 * l2-table set ip-mcast-vlan-mode ( ivl-svl | no-vlan | always-vlan )
 */

cparser_result_t
cparser_cmd_l2_table_set_ip_mcast_vlan_mode_ivl_svl_no_vlan_always_vlan(
	cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcVlanMode_t mode;

    DIAG_UTIL_PARAM_CHK();

	if('i' == TOKEN_CHAR(3, 0))
        mode = LOOKUP_VLAN_BY_IVL_SVL;
	else if('n' == TOKEN_CHAR(3,0))
		mode = LOOKUP_VLAN_FORCE_NO_VLAN;
    else if('a' == TOKEN_CHAR(3, 0))
        mode = LOOKUP_VLAN_FORCE_VLAN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcVlanMode_set(mode), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_l2_table_set_ip_mcast_vlan_mode_ivl_svl_no_vlan_always_vlan */

/*
 * l2-table get ip-mcast-vlan-mode
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_vlan_mode(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcVlanMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcVlanMode_get(&mode), ret);

    diag_util_printf("\n IPMC VLAN mode : %s", diagStr_l2IpMcVlanHashMethodStr[mode]);
    diag_util_printf("\n");

    return CPARSER_OK;
} /* end of cparser_cmd_l2_table_get_ip_mcast_vlan_mode */

/*
 * l2-table set ipv6-mcast-mode ( mac-and-fid-vid | dip )
 */
cparser_result_t
cparser_cmd_l2_table_set_ipv6_mcast_mode_mac_and_fid_vid_dip(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcMode_t mode;

    DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp("dip",TOKEN_STR(3)))
        mode = LOOKUP_ON_DIP;
    else if(!osal_strcmp("mac-and-fid-vid",TOKEN_STR(3)))
        mode = LOOKUP_ON_MAC_AND_VID_FID;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipv6mcMode_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_ipv6_mcast_mode_mac_and_fid_vid_dip */

/*
 * l2-table get ipv6-mcast-mode
 */
cparser_result_t
cparser_cmd_l2_table_get_ipv6_mcast_mode(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_ipv6mcMode_get(&mode), ret);

    diag_util_printf("\n IPv6MC mode : %s", diagStr_l2IpMcHashMethodStr[mode]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ipv6_mcast_mode */

/*
 * l2-table set ext-mbr-cfg ext-port-idx <UINT:index> ext ( <PORT_LIST:exts> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_set_ext_mbr_cfg_ext_port_idx_index_ext_exts_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *exts_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 6), ret);
    RTK_PORTMASK_RESET(portmask);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        RTK_PORTMASK_PORT_SET(portmask, port);
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_extMemberConfig_set(*index_ptr, portmask), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_ext_mbr_cfg_cfg_index_index_ext_exts_all_none */

/*
 * l2-table get ext-mbr-cfg ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_get_ext_mbr_cfg_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_port_t port;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_extMemberConfig_get(*index_ptr, &portmask), ret);
    diag_util_printf("Index: %u\n", *index_ptr);
    diag_util_printf("Ext port list: %s\n", diag_util_mask32tostr(RTK_PORTMASK_TO_UINT_PORTMASK((&portmask))));

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ext_mbr_cfg_cfg_index_index */

/*
 * l2-table get vid-unmatch port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_vid_unmatch_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_vidUnmatchAction_get(port, &action), ret);
        diag_util_printf("\nPort %d Action: %s", port, diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_vid_unmatch_port_ports_all_action */

/*
 * l2-table set vid-unmatch port ( <PORT_LIST:ports> | all ) action ( forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_vid_unmatch_port_ports_all_action_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(6, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_vidUnmatchAction_set(port, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_vid_unmatch_port_ports_all_action_forward_trap_to_cpu */


/*
 * l2-table get ( port-move | unknown-sa ) port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_port_move_unknown_sa_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_newMacLrnMode_t mode;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if('p' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_illegalPortMoveAction_get(port, &action), ret);
            diag_util_printf("\n Port %d Port move Action: %s", port, diagStr_actionStr[action]);
        }
        else if('u' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_newMacOp_get(port, &mode, &action), ret);
            diag_util_printf("\n Port %d unknown SA Action: %s", port, diagStr_actionStr[action]);
        }
        else
            return CPARSER_NOT_OK;
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_port_move_unknown_sa_port_ports_all_action */

/*
 * l2-table set ( port-move | unknown-sa ) port ( <PORT_LIST:ports> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_port_move_unknown_sa_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('c' == TOKEN_CHAR(6, 0))
        action = ACTION_COPY2CPU;
    else if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(6, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if('p' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_illegalPortMoveAction_set(port, action), ret);
        }
        else if('u' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_newMacOp_set(port, HARDWARE_LEARNING, action), ret);
        }
        else
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_port_move_unknown_sa_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu */

/*
 * l2-table set port-move port ( <PORT_LIST:ports> | all ) action forward-disable-learn
 */
cparser_result_t
cparser_cmd_l2_table_set_port_move_port_ports_all_action_forward_disable_learn(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	switch(DIAG_UTIL_CHIP_TYPE)
	{

#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			action = 0x3;
			
			DIAG_UTIL_PORTMASK_SCAN(portlist, port)
			{
				DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9607C_LUT_UNMATCHED_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9607C_ACTf, &action), ret);
			}

			break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            action = 0x3;
            
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9603CVD_LUT_UNMATCHED_SA_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9603CVD_ACTf, &action), ret);
            }

            break;
#endif
		default:
			diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;
			break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_port_move_port_ports_all_action_forward_disable_learn */



/*
 * l2-table set flush mac-ucast
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast(
    cparser_context_t *context)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_flush_ctrl_t apollo_cfg;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_flush_ctrl_t apollomp_cfg;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    rtl9602c_raw_flush_ctrl_t rtl9602c_cfg;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    rtl9607c_raw_flush_ctrl_t rtl9607c_cfg;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    rtl9603cvd_raw_flush_ctrl_t rtl9603cvd_cfg;
#endif

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_get(&apollo_cfg), ret);

            apollo_cfg.flushType = RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_set(&apollo_cfg), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_get(&apollomp_cfg), ret);

            apollomp_cfg.flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_set(&apollomp_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_l2_flushCtrl_get(&rtl9602c_cfg), ret);

            rtl9602c_cfg.flushType = RTL9602C_RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_l2_flushCtrl_set(&rtl9602c_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_l2_flushCtrl_get(&rtl9607c_cfg), ret);

            rtl9607c_cfg.flushType = RTL9607C_RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_l2_flushCtrl_set(&rtl9607c_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_l2_flushCtrl_get(&rtl9603cvd_cfg), ret);

            rtl9603cvd_cfg.flushType = RTL9603CVD_RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_l2_flushCtrl_set(&rtl9603cvd_cfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast */

/*
 * l2-table set flush mac-ucast include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_include_static(
    cparser_context_t *context)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_flush_ctrl_t apollo_cfg;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_flush_ctrl_t apollomp_cfg;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    rtl9602c_raw_flush_ctrl_t rtl9602c_cfg;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    rtl9607c_raw_flush_ctrl_t rtl9607c_cfg;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    rtl9603cvd_raw_flush_ctrl_t rtl9603cvd_cfg;
#endif

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_get(&apollo_cfg), ret);

            apollo_cfg.flushType = RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_set(&apollo_cfg), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_get(&apollomp_cfg), ret);

            apollomp_cfg.flushType = APOLLOMP_RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_set(&apollomp_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_l2_flushCtrl_get(&rtl9602c_cfg), ret);

            rtl9602c_cfg.flushType = RTL9602C_RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_l2_flushCtrl_set(&rtl9602c_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_l2_flushCtrl_get(&rtl9607c_cfg), ret);

            rtl9607c_cfg.flushType = RTL9607C_RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_l2_flushCtrl_set(&rtl9607c_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_l2_flushCtrl_get(&rtl9603cvd_cfg), ret);

            rtl9603cvd_cfg.flushType = RTL9603CVD_RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_l2_flushCtrl_set(&rtl9603cvd_cfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_include_static */

/*
 * l2-table set flush mac-ucast static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_static_only(
    cparser_context_t *context)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_flush_ctrl_t apollo_cfg;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_flush_ctrl_t apollomp_cfg;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    rtl9602c_raw_flush_ctrl_t rtl9602c_cfg;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    rtl9607c_raw_flush_ctrl_t rtl9607c_cfg;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    rtl9603cvd_raw_flush_ctrl_t rtl9603cvd_cfg;
#endif

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_get(&apollo_cfg), ret);

            apollo_cfg.flushType = RAW_FLUSH_TYPE_STATIC;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_set(&apollo_cfg), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_get(&apollomp_cfg), ret);

            apollomp_cfg.flushType = APOLLOMP_RAW_FLUSH_TYPE_STATIC;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_set(&apollomp_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_raw_l2_flushCtrl_get(&rtl9602c_cfg), ret);

            rtl9602c_cfg.flushType = RTL9602C_RAW_FLUSH_TYPE_STATIC;

            DIAG_UTIL_ERR_CHK(rtl9602c_raw_l2_flushCtrl_set(&rtl9602c_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9607c_raw_l2_flushCtrl_get(&rtl9607c_cfg), ret);

            rtl9607c_cfg.flushType = RTL9607C_RAW_FLUSH_TYPE_STATIC;

            DIAG_UTIL_ERR_CHK(rtl9607c_raw_l2_flushCtrl_set(&rtl9607c_cfg), ret);

            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_l2_flushCtrl_get(&rtl9603cvd_cfg), ret);

            rtl9603cvd_cfg.flushType = RTL9603CVD_RAW_FLUSH_TYPE_STATIC;

            DIAG_UTIL_ERR_CHK(rtl9603cvd_raw_l2_flushCtrl_set(&rtl9603cvd_cfg), ret);

            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_static_only */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByFid = 1;
        cfg.fid = *fid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 0;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid> include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_include_static(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByFid = 1;
        cfg.fid = *fid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_include_static */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid> static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_static_only(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByFid = 1;
        cfg.fid = *fid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 0;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_static_only */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByVid = 1;
        cfg.vid = *vid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 0;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid> include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_include_static(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByVid = 1;
        cfg.vid = *vid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_include_static */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid> static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_static_only(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByVid = 1;
        cfg.vid = *vid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 0;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_static_only */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 0;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_include_static(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_include_static */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_static_only(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 0;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_static_only */

/*
 * l2-table set ip-mcast-data port ( <PORT_LIST:ports> | all ) action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l2_table_set_ip_mcast_data_port_ports_all_action_forward_drop(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portIpmcAction_set(port, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_ip_mcast_data_port_ports_all_action_forward_drop */

/*
 * l2-table get ip-mcast-data port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_data_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portIpmcAction_get(port, &action), ret);
        diag_util_printf("\n Port %d IPMC action: %s", port, diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_data_port_ports_all_action */

/*
 * l2-table get entry address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_entry_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_lut_table_t apollomp_diag_lut;
#endif
#if defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	rtk_l2_addr_table_t l2table;
	uint32 addr;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

            diag_lut.method = RAW_LUT_READ_METHOD_ADDRESS;
            diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_lookUpTb_get(&diag_lut), ret);

            _diag_lutDisplay(&diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

            apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS;
            apollomp_diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut), ret);

            _diag_apollomp_lutDisplay(&apollomp_diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			l2table.method = LUT_READ_METHOD_ADDRESS;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
			_diag_rtl9601b_lutDisplay(&l2table);
		break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			//l2table.method = LUT_READ_METHOD_ADDRESS;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            /* See if the nextValidEntry is equal to the address inputed */
            if(addr != *address_ptr)
            {
    			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
            }
  			_diag_rtl9602c_lutDisplay(&l2table, ENABLED);
		break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            /* See if the nextValidEntry is equal to the address inputed */
            if(addr != *address_ptr)
            {
    			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
            }
  			_diag_rtl9607c_lutDisplay(&l2table, ENABLED);
		break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
            addr = *address_ptr;
            DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            /* See if the nextValidEntry is equal to the address inputed */
            if(addr != *address_ptr)
            {
                DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
            }
            _diag_rtl9603cvd_lutDisplay(&l2table, ENABLED);
        break;
#endif
        case UNKNOWN_CHIP_ID:
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_entry_address_address */

/*
 * l2-table get next-entry address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_lut_table_t apollomp_diag_lut;
#endif
#if defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	rtk_l2_addr_table_t l2table;
	uint32 addr;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

            diag_lut.method = RAW_LUT_READ_METHOD_NEXT_ADDRESS;
            diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_lookUpTb_get(&diag_lut), ret);

            _diag_lutDisplay(&diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

            apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS;
            apollomp_diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut), ret);

            _diag_apollomp_lutDisplay(&apollomp_diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			l2table.method = LUT_READ_METHOD_NEXT_ADDRESS;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
			_diag_rtl9601b_lutDisplay(&l2table);
		break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			//l2table.method = LUT_READ_METHOD_NEXT_ADDRESS;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
			_diag_rtl9602c_lutDisplay(&l2table, ENABLED);
		break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
			_diag_rtl9607c_lutDisplay(&l2table, ENABLED);
		break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
            addr = *address_ptr;
            DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            _diag_rtl9603cvd_lutDisplay(&l2table, ENABLED);
        break;
#endif
        case UNKNOWN_CHIP_ID:
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_address_address */

/*
 * l2-table get next-entry mac-ucast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_ucastAddr_t ucastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&ucastAddr, 0x00, sizeof(rtk_l2_ucastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidAddr_get(&address, &ucastAddr), ret);
    _display_l2Ucast_entry(&ucastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address */

/*
 * l2-table get next-entry mac-ucast address <UINT:address> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address_spn_port(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_ucastAddr_t ucastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&ucastAddr, 0x00, sizeof(rtk_l2_ucastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidAddrOnPort_get(*port_ptr, &address, &ucastAddr), ret);
    _display_l2Ucast_entry(&ucastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address_spn_port */

/*
 * l2-table get next-entry l2-mcast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_l2_mcast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_mcastAddr_t mcastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&mcastAddr, 0x00, sizeof(rtk_l2_mcastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidMcastAddr_get(&address, &mcastAddr), ret);
    _display_l2Mcast_entry(&mcastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_l2_mcast_address_address */

/*
 * l2-table get next-entry ip-mcast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_ip_mcast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_ipMcastAddr_t ipMcastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&ipMcastAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidIpMcastAddr_get(&address, &ipMcastAddr), ret);
    _display_ipMcast_entry(&ipMcastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_ip_mcast_address_address */

/*
 * l2-table get next-entry l2-ip-mcast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_l2_ip_mcast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_lut_table_t apollomp_diag_lut;
#endif
#if defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	rtk_l2_addr_table_t l2table;
	uint32 addr;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

            diag_lut.method = RAW_LUT_READ_METHOD_NEXT_L2L3MC;
            diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_lookUpTb_get(&diag_lut), ret);

            _diag_lutDisplay(&diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

            apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2L3MC;
            apollomp_diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut), ret);

            _diag_apollomp_lutDisplay(&apollomp_diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			l2table.method = LUT_READ_METHOD_ADDRESS;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
			_diag_rtl9601b_lutDisplay(&l2table);
		break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			//l2table.method = LUT_READ_METHOD_ADDRESS;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            /* See if the nextValidEntry is equal to the address inputed */
            switch(l2table.entryType)
            {
            case RTK_LUT_L2UC:
                if(l2table.entry.l2UcEntry.index != addr)
                {
        			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            case RTK_LUT_L2MC:
                if(l2table.entry.l2McEntry.index != addr)
                {
        			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            case RTK_LUT_L3MC:
            case RTK_LUT_L3V6MC:
                if(l2table.entry.ipmcEntry.index != addr)
                {
        			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            default:
                break;
            }
  			_diag_rtl9602c_lutDisplay(&l2table, ENABLED);
		break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = *address_ptr;
			DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            /* See if the nextValidEntry is equal to the address inputed */
            switch(l2table.entryType)
            {
            case RTK_LUT_L2UC:
                if(l2table.entry.l2UcEntry.index != addr)
                {
        			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            case RTK_LUT_L2MC:
                if(l2table.entry.l2McEntry.index != addr)
                {
        			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            case RTK_LUT_L3MC:
                if(l2table.entry.ipmcEntry.index != addr)
                {
        			DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            default:
                break;
            }
  			_diag_rtl9607c_lutDisplay(&l2table, ENABLED);
		break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
            addr = *address_ptr;
            DIAG_UTIL_ERR_CHK(rtk_l2_nextValidEntry_get(&addr,&l2table),ret);
            /* See if the nextValidEntry is equal to the address inputed */
            switch(l2table.entryType)
            {
            case RTK_LUT_L2UC:
                if(l2table.entry.l2UcEntry.index != addr)
                {
                    DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            case RTK_LUT_L2MC:
                if(l2table.entry.l2McEntry.index != addr)
                {
                    DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            case RTK_LUT_L3MC:
                if(l2table.entry.ipmcEntry.index != addr)
                {
                    DIAG_UTIL_ERR_CHK(RT_ERR_L2_ENTRY_NOTFOUND,ret);
                }
                break;
            default:
                break;
            }
            _diag_rtl9603cvd_lutDisplay(&l2table, ENABLED);
        break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_l2_ip_mcast_address_address */

/*
 * l2-table get src-port-egress-filter port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_l2_table_get_src_port_egress_filter_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(&portmask), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(RTK_PORTMASK_IS_PORT_SET(portmask, port))
        {
            diag_util_printf("\n SRC Port %d egress filter state: %s", port, diagStr_enable[ENABLED]);
        }
        else
        {
            diag_util_printf("\n SRC Port %d egress filter state: %s", port, diagStr_enable[DISABLED]);
        }
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_src_port_egress_filter_port_ports_all_state */

/*
 * l2-table set src-port-egress-filter port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_src_port_egress_filter_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(&portmask), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(ENABLED == state)
        {
            RTK_PORTMASK_PORT_SET(portmask, port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(portmask, port);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_set(&portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_src_port_egress_filter_port_ports_all_state_disable_enable */

/*
 * l2-table get src-port-egress-filter ext <PORT_LIST:exts> state
 */
cparser_result_t
cparser_cmd_l2_table_get_src_port_egress_filter_ext_exts_state(
    cparser_context_t *context,
    char * *exts_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_extPortEgrFilterMask_get(&portmask), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(RTK_PORTMASK_IS_PORT_SET(portmask, port))
        {
            diag_util_printf("\n EXT Port %d egress filter state: %s", port, diagStr_enable[ENABLED]);
        }
        else
        {
            diag_util_printf("\n EXT Port %d egress filter state: %s", port, diagStr_enable[DISABLED]);
        }
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_src_port_egress_filter_ext_exts_state */

/*
 * l2-table set src-port-egress-filter ext <PORT_LIST:exts> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_src_port_egress_filter_ext_exts_state_disable_enable(
    cparser_context_t *context,
    char * *exts_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_extPortEgrFilterMask_get(&portmask), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(ENABLED == state)
        {
            RTK_PORTMASK_PORT_SET(portmask, port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(portmask, port);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_extPortEgrFilterMask_set(&portmask), ret);
    return CPARSER_OK;
}/* end of cparser_cmd_l2_table_set_src_port_egress_filter_ext_exts_state_disable_enable */



/*
 * l2-table get learning-exceed
 */
cparser_result_t
cparser_cmd_l2_table_get_learning_exceed(
    cparser_context_t *context)
{
	rtk_enable_t status;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningOverStatus_get(&status),ret);

	if(status==ENABLED){
		diag_util_printf("Learning-exceed = Yes\n");
	}else{
		diag_util_printf("Learning-exceed = No\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_learning_exceed */


/*
 * l2-table get learning-exceed port ( <PORT_LIST:ports> | all ) */
cparser_result_t
cparser_cmd_l2_table_get_learning_exceed_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
	int32 ret;
	rtk_port_t port;
	diag_portlist_t portlist;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

	 DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
	 	DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningOverStatus_get(port, &state), ret);
		if(state==ENABLED){
			diag_util_printf("Port Id %d, learning-exceed = Yes\n",port);
		}else{
			diag_util_printf("Port Id %d, learning-exceed = No\n",port);
		}
	}

      return CPARSER_OK;

}    /* end of cparser_cmd_l2_table_get_learning_exceed_port_ports_all */


/*
 * l2-table clear learning-exceed
 */
cparser_result_t
cparser_cmd_l2_table_clear_learning_exceed(
    cparser_context_t *context)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningOverStatus_clear(),ret);
    diag_util_printf("Learning-exceed clear\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_clear_learning_exceed */

/*
 * l2-table clear l2-table
 */
cparser_result_t
cparser_cmd_l2_table_clear_l2_table(
    cparser_context_t *context)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_l2_table_clear(),ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_clear_l2_table */


/*
 * l2-table clear learning-exceed port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_clear_learning_exceed_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
	int32 ret;
	rtk_port_t port;
	diag_portlist_t portlist;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

	 DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
	 	DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningOverStatus_clear(port), ret);
		diag_util_printf("Port Id %d, learning-exceed clear\n",port);
	}
   	return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_clear_learning_exceed_port_ports_all */


/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> ctag-if state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ctag_if_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_CTAG_IF;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_CTAG_IF;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ctag_if_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> ctag-if vid <UINT:vid> */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ctag_if_vid_cvid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *cvid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

	l2Addr.ctag_vid = *cvid_ptr;
   
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ctag_if_vid_vid */

     
 /*
  * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> ctag-if state ( disable | enable )
  */
 cparser_result_t
 cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ctag_if_state_disable_enable(
	 cparser_context_t *context,
	 uint32_t  *fid_ptr,
	 cparser_macaddr_t	*mac_ptr)
 {
	int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid= *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid= *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_CTAG_IF;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_CTAG_IF;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
 }	  /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ctag_if_state_disable_enable */
 
 /*
  * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> ctag-if vid <UINT:vid> */
 cparser_result_t
 cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ctag_if_vid_cvid(
	 cparser_context_t *context,
	 uint32_t  *fid_ptr,
	 cparser_macaddr_t	*mac_ptr,
	 uint32_t  *cvid_ptr)
 {
	int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid= *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
    }

	l2Addr.ctag_vid = *cvid_ptr;
   
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}	  /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ctag_if_vid_vid */


/*
 * l2-table get nolearn-aging state 
 */
cparser_result_t
cparser_cmd_l2_table_get_nolearn_aging_state(
    cparser_context_t *context)
{
	int32 ret;
    rtk_enable_t enable = DISABLED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
			case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_AGE_UPDATE_NOLRNf, &enable), ret);
            
            diag_util_mprintf("Always aging update state %s\n",enable?"Enable":"Disable");

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_nolearn_aging_state */

/*
 * l2-table set nolearn-aging state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_nolearn_aging_state_disable_enable(
    cparser_context_t *context)
{
	int32 ret;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
			case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_AGE_UPDATE_NOLRNf, &enable), ret);

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}


    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_nolearn_aging_state_disable_enable */

/*
 * l2-table get svl-untag-vid-learn
 */
cparser_result_t
cparser_cmd_l2_table_get_svl_untag_vid_learn(
    cparser_context_t *context)
{
	int32 ret;
    rtk_enable_t enable = DISABLED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
			case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_CVID_LEARNf, &enable), ret);
            diag_util_mprintf("Untag CVID learning to %s\n",enable?"VID 0":"Ingress VID");

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_svl_untag_vid_learn */

/*
 * l2-table set svl-untag-vid-learn ( ingress-vid | vid0 ) 
 */
cparser_result_t
cparser_cmd_l2_table_set_svl_untag_vid_learn_ingress_vid_vid0(
    cparser_context_t *context)
{
	int32 ret;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(3, 0))
        enable = 0;
    else if('v' == TOKEN_CHAR(3, 0))
        enable = 1;
    else
        return CPARSER_NOT_OK;

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
			case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_LUT_CFGr, APOLLOMP_LUT_CVID_LEARNf, &enable), ret);

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_svl_untag_vid_learn_ingress_vid_vid0 */ 
		
/*
 * l2-table get entry address valid
 */
cparser_result_t
cparser_cmd_l2_table_get_entry_address_valid(
	cparser_context_t *context)
{
	int32 ret;
	uint32 index = 0;
#if defined(CONFIG_SDK_APOLLO)
	apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
	apollomp_lut_table_t apollomp_diag_lut;
#endif
#if defined(CONFIG_SDK_RTL9601B) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
	rtk_l2_addr_table_t l2table;
	uint32 addr;
#endif

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_APOLLO)
		case APOLLO_CHIP_ID:
			osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

			diag_lut.method = RAW_LUT_READ_METHOD_ADDRESS;
			diag_lut.address = index;

			while (RT_ERR_OK == (apollo_raw_l2_lookUpTb_get(&diag_lut)))
			{
				if (!diag_lut.lookup_busy && (diag_lut.lookup_hit))
				{
					_diag_lutDisplay(&diag_lut);					
				}
				osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));
				index++;
				diag_lut.method = RAW_LUT_READ_METHOD_ADDRESS;
				diag_lut.address = index;
			}	
			break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
		case APOLLOMP_CHIP_ID:
			osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

			apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS;
			apollomp_diag_lut.address = index;

			while (RT_ERR_OK == (apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut)))
			{				
				if(!apollomp_diag_lut.lookup_busy && (apollomp_diag_lut.lookup_hit))
				{
					_diag_apollomp_lutDisplay(&apollomp_diag_lut);
				}
				osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));
				index++;
				apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS;
				apollomp_diag_lut.address = index;
			}
			break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
			addr = index;
			l2table.method = LUT_READ_METHOD_ADDRESS;
			l2table.entryType = RTK_LUT_END;

			while (index < HAL_L2_LEARN_LIMIT_CNT_MAX())
			{
				if(RT_ERR_OK == (rtk_l2_nextValidEntry_get(&addr,&l2table)))
				{
					_diag_rtl9601b_lutDisplay(&l2table);
				}
    			index++;
				osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
				addr = index;
				l2table.method = LUT_READ_METHOD_ADDRESS;
				l2table.entryType = RTK_LUT_END;
			}
			break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			addr = index;
			do
			{
				osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
				//l2table.method = LUT_READ_METHOD_NEXT_ADDRESS;
				//l2table.entryType =	RTK_LUT_END;
				index =	addr;
				if(RT_ERR_OK ==	(rtk_l2_nextValidEntry_get(&addr,&l2table)))
				{
					/* Wrap	around */
					if(addr	< index)
					{
						break;
					}

					_diag_rtl9602c_lutDisplay(&l2table, ENABLED);
					addr ++;
				}
				else
				{
					break;
				}
			} while(1);
			break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:
			addr = index;
			do
			{
				osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
				index =	addr;
				if(RT_ERR_OK ==	(rtk_l2_nextValidEntry_get(&addr,&l2table)))
				{
					/* Wrap	around */
					if(addr	< index)
					{
						break;
					}

					_diag_rtl9607c_lutDisplay(&l2table, ENABLED);
					addr ++;
				}
				else
				{
					break;
				}
			} while(1);
			break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            addr = index;
            do
            {
                osal_memset(&l2table,0x0,sizeof(rtk_l2_addr_table_t));
                index = addr;
                if(RT_ERR_OK == (rtk_l2_nextValidEntry_get(&addr,&l2table)))
                {
                    /* Wrap around */
                    if(addr < index)
                    {
                        break;
                    }

                    _diag_rtl9603cvd_lutDisplay(&l2table, ENABLED);
                    addr ++;
                }
                else
                {
                    break;
                }
            } while(1);
            break;
#endif
        case UNKNOWN_CHIP_ID:
            break;
		default:
			diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;
			break;
	}

	diag_util_printf("\n");

	return CPARSER_OK;
}	 /* end of cparser_cmd_l2_table_get_entry_address_valid */

/*
 * l2-table set arp-usage-as-known ( follow-age | always-known ) 
 */
cparser_result_t
cparser_cmd_l2_table_set_arp_usage_as_known_follow_age_always_known(
    cparser_context_t *context)
{
    int32 ret;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9602C
	case RTL9602C_CHIP_ID:
        if('f' == TOKEN_CHAR(3, 0))
        {
            value = 0;
        }
        else if('a' == TOKEN_CHAR(3, 0))
        {
            value = 1;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_LUT_CFGr, RTL9602C_LUT_L34_ARP_USAGE_AS_KNOWNf, &value), ret);
		break;
#endif    
#ifdef CONFIG_SDK_RTL9607C
	case RTL9607C_CHIP_ID:
        if('f' == TOKEN_CHAR(3, 0))
        {
            value = 0;
        }
        else if('a' == TOKEN_CHAR(3, 0))
        {
            value = 1;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_LUT_CFGr, RTL9607C_LUT_L34_ARP_USAGE_AS_KNOWNf, &value), ret);
		break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD
    case RTL9603CVD_CHIP_ID:
        if('f' == TOKEN_CHAR(3, 0))
        {
            value = 0;
        }
        else if('a' == TOKEN_CHAR(3, 0))
        {
            value = 1;
        }
        else
        {
            return CPARSER_NOT_OK;
        }
        
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_L34_ARP_USAGE_AS_KNOWNf, &value), ret);
        break;
#endif    
	default:
		diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
		return CPARSER_NOT_OK;		  
		break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_arp_usage_as_known_follow_age_always_known */

/*
 * l2-table get arp-usage-as-known
 */
cparser_result_t
cparser_cmd_l2_table_get_arp_usage_as_known(
    cparser_context_t *context)
{
    int32 ret;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_RTL9602C
	case RTL9602C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_LUT_CFGr, RTL9602C_LUT_L34_ARP_USAGE_AS_KNOWNf, &value), ret);

        diag_util_mprintf("arp usage as known: %s\n", (0 == value) ? "follow age in lut" : "always treat as known");
		break;
#endif    
#ifdef CONFIG_SDK_RTL9607C
	case RTL9607C_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_LUT_CFGr, RTL9607C_LUT_L34_ARP_USAGE_AS_KNOWNf, &value), ret);

        diag_util_mprintf("arp usage as known: %s\n", (0 == value) ? "follow age in lut" : "always treat as known");
		break;
#endif    
#ifdef CONFIG_SDK_RTL9603CVD
    case RTL9603CVD_CHIP_ID:
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_L34_ARP_USAGE_AS_KNOWNf, &value), ret);

        diag_util_mprintf("arp usage as known: %s\n", (0 == value) ? "follow age in lut" : "always treat as known");
        break;
#endif  
	default:
		diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
		return CPARSER_NOT_OK;		  
		break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_arp_usage_as_known */

/*
 * l2 set trap-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_set_trap_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_set(TRAP_REASON_LUT, *priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_set_priority_priority */

/*
 * l2 get trap-priority
 */
cparser_result_t
cparser_cmd_l2_table_get_trap_priority(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    uint32 priority;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_get(TRAP_REASON_LUT, &priority), ret);
    diag_util_mprintf("The L2 trap priority = %u\n", priority);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_get_priority */

/*
 * l2-table clear hash
 */
cparser_result_t
cparser_cmd_l2_table_clear_hash(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    osal_memset(&hashData, 0, sizeof(rtk_l2_hashData_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_clear_hash */

/*
 * l2-table set hash dmac-address <MACADDR:dmac>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_dmac_address_dmac(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    osal_memcpy(&hashData.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    hashData.dataFlags |= LUT_HASHFLAG_MAC;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_dmac_address_dmac */

/*
 * l2-table set hash filter-id <UINT:fid>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_filter_id_fid(
    cparser_context_t *context,
    uint32_t *fid_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    hashData.fid = *fid_ptr;
    hashData.dataFlags |= LUT_HASHFLAG_FID;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_filter_id_fid */

/*
 * l2-table set hash enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32_t *efid_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    hashData.efid = *efid_ptr;
    hashData.dataFlags |= LUT_HASHFLAG_EFID;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_ext_filter_id_efid */

/*
 * l2-table set hash vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_vid_vid(
    cparser_context_t *context,
    uint32_t *vid_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    hashData.vid = *vid_ptr;
    hashData.dataFlags |= LUT_HASHFLAG_VID;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_vid_vid */

/*
 * l2-table set hash gip <IPV4ADDR:gip>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_gip_gip(
    cparser_context_t *context,
    uint32_t *gip_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    hashData.gip = (rtk_ip_addr_t)*gip_ptr;
    hashData.dataFlags |= LUT_HASHFLAG_GIP;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_gip_gip */

/*
 * l2-table set hash sip <IPV4ADDR:sip>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_sip_sip(
    cparser_context_t *context,
    uint32_t *sip_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    hashData.sip = (rtk_ip_addr_t)*sip_ptr;
    hashData.dataFlags |= LUT_HASHFLAG_SIP;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_sip_sip */

/*
 * l2-table set hash dip6 <IPV6ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_dip6_dip(
    cparser_context_t *context,
    char * *dip_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_str2ipv6(&hashData.dip6.ipv6_addr[0], TOKEN_STR(4));
    hashData.dataFlags |= LUT_HASHFLAG_DIP6;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_dip6_dip */

/*
 * l2-table set hash sip-filter ( enable | disable )
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_sip_filter_enable_disable(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(4, 0))
        hashData.sip_filter_en = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        hashData.sip_filter_en = DISABLED;
    else
        return CPARSER_NOT_OK;

    hashData.dataFlags |= LUT_HASHFLAG_SIP_FILTER_EN;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_sip_filter_enable_disable */

/*
 * l2-table set hash sip-index <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_set_hash_sip_index_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    hashData.sip_filter_idx = *index_ptr;
    hashData.dataFlags |= LUT_HASHFLAG_SIP_FILTER_IDX;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_hash_sip_index_index */

/*
 * l2-table get hash ( uc-svl | uc-ivl | mc-svl | mc-ivl | mc-only | sip-gip | gip-fid | gip-vid | gip-only | gip-fid-sipfilter | gip-vid-sipfilter | dip6 )
 */
cparser_result_t
cparser_cmd_l2_table_get_hash_uc_svl_uc_ivl_mc_svl_mc_ivl_mc_only_sip_gip_gip_fid_gip_vid_gip_only_gip_fid_sipfilter_gip_vid_sipfilter_dip6(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    uint32 hashType = LUT_HASHTYPE_L2_UCSVL, hashValue;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(!osal_strcmp(TOKEN_STR(3), "uc-svl"))
    {
        hashType = LUT_HASHTYPE_L2_UCSVL;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "uc-ivl"))
    {
        hashType = LUT_HASHTYPE_L2_UCIVL;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "mc-svl"))
    {
        hashType = LUT_HASHTYPE_L2_MCSVL;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "mc-ivl"))
    {
        hashType = LUT_HASHTYPE_L2_MCIVL;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "mc-only"))
    {
        hashType = LUT_HASHTYPE_L2_MC;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "sip-gip"))
    {
        hashType = LUT_HASHTYPE_L3_SIPGIP;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "gip-fid"))
    {
        hashType = LUT_HASHTYPE_L3_GIPFID;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "gip-vid"))
    {
        hashType = LUT_HASHTYPE_L3_GIPVID;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "gip-only"))
    {
        hashType = LUT_HASHTYPE_L3_GIP;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "gip-fid-sipfilter"))
    {
        hashType = LUT_HASHTYPE_L3_GIPFIDSIPFILTER;
    }
    else if(!osal_strcmp(TOKEN_STR(3), "gip-vid-sipfilter"))
    {
        hashType = LUT_HASHTYPE_L3_GIPVIDSIPFILTER;
    }
    else //if(!osal_strcmp(TOKEN_STR(3), "dip6"))
    {
        hashType = LUT_HASHTYPE_L3_DIP6;
    }

	DIAG_UTIL_ERR_CHK(rtk_l2_hashValue_get(hashType, hashData, &hashValue), ret);
    diag_util_mprintf("hash value base = 0x%04x\n", hashValue);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_hash_uc_svl_uc_ivl_mc_svl_mc_ivl_mc_only_sip_gip_gip_fid_gip_vid_gip_only_gip_fid_sipfilter_gip_vid_sipfilter_dip6 */

/*
 * l2-table show hash
 */
cparser_result_t
cparser_cmd_l2_table_show_hash(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("hash flag = 0x%08x\n", hashData.dataFlags);
    if(hashData.dataFlags & LUT_HASHFLAG_MAC)
    {
        diag_util_mprintf("mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
            hashData.mac.octet[0],
            hashData.mac.octet[1],
            hashData.mac.octet[2],
            hashData.mac.octet[3],
            hashData.mac.octet[4],
            hashData.mac.octet[5]);
    }
    else
    {
        diag_util_mprintf("mac = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_FID)
    {
        diag_util_mprintf("filter-id = %u\n", hashData.fid);
    }
    else
    {
        diag_util_mprintf("filter-id = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_EFID)
    {
        diag_util_mprintf("enhanced-filter-id = %u\n", hashData.efid);
    }
    else
    {
        diag_util_mprintf("enhanced-filter-id = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_VID)
    {
        diag_util_mprintf("vid = %u\n", hashData.vid);
    }
    else
    {
        diag_util_mprintf("vid = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_GIP)
    {
        uint8 strGip[64];
        diag_util_ip2str(strGip,hashData.gip);
        diag_util_mprintf("gip = %s\n", strGip);
    }
    else
    {
        diag_util_mprintf("gip = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_SIP)
    {
        uint8 strSip[64];
        diag_util_ip2str(strSip,hashData.sip);
        diag_util_mprintf("sip = %s\n", strSip);
    }
    else
    {
        diag_util_mprintf("sip = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_DIP6)
    {
        diag_util_mprintf("dip6 = %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
            hashData.dip6.ipv6_addr[0],
            hashData.dip6.ipv6_addr[1],
            hashData.dip6.ipv6_addr[2],
            hashData.dip6.ipv6_addr[3],
            hashData.dip6.ipv6_addr[4],
            hashData.dip6.ipv6_addr[5],
            hashData.dip6.ipv6_addr[6],
            hashData.dip6.ipv6_addr[7],
            hashData.dip6.ipv6_addr[8],
            hashData.dip6.ipv6_addr[9],
            hashData.dip6.ipv6_addr[10],
            hashData.dip6.ipv6_addr[11],
            hashData.dip6.ipv6_addr[12],
            hashData.dip6.ipv6_addr[13],
            hashData.dip6.ipv6_addr[14],
            hashData.dip6.ipv6_addr[15]);
    }
    else
    {
        diag_util_mprintf("dip6 = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_SIP_FILTER_EN)
    {
        diag_util_mprintf("sip-filter state = %s\n", diagStr_enable[hashData.sip_filter_en]);
    }
    else
    {
        diag_util_mprintf("sip-filter state = n/a\n");
    }
    if(hashData.dataFlags & LUT_HASHFLAG_SIP_FILTER_IDX)
    {
        diag_util_mprintf("sip-filter index = %u\n", hashData.sip_filter_idx);
    }
    else
    {
        diag_util_mprintf("sip-filter index = n/a\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_show_hash */

