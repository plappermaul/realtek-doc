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
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_l2.h>
#include <dal/apollomp/raw/apollomp_raw_l2.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

static uint32    l2_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
static int32
_dal_apollomp_rawtoL2uc(rtk_l2_ucastAddr_t *pL2Data, apollomp_lut_table_t *pLut_entry)
{
    pL2Data->vid            = pLut_entry->cvid_fid;
    pL2Data->mac.octet[0]   = pLut_entry->mac.octet[0];
    pL2Data->mac.octet[1]   = pLut_entry->mac.octet[1];
    pL2Data->mac.octet[2]   = pLut_entry->mac.octet[2];
    pL2Data->mac.octet[3]   = pLut_entry->mac.octet[3];
    pL2Data->mac.octet[4]   = pLut_entry->mac.octet[4];
    pL2Data->mac.octet[5]   = pLut_entry->mac.octet[5];
    pL2Data->fid            = pLut_entry->fid;
    pL2Data->efid           = pLut_entry->efid;
    pL2Data->port           = pLut_entry->spa;
    pL2Data->ext_port       = pLut_entry->ext_dsl_spa;
    pL2Data->age            = pLut_entry->age;
    pL2Data->priority       = pLut_entry->lut_pri;
    pL2Data->auth           = pLut_entry->auth;
    pL2Data->index          = pLut_entry->address;

    if(ENABLED == pLut_entry->sa_block)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;

    if(ENABLED == pLut_entry->da_block)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;

    if(ENABLED == pLut_entry->nosalearn)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_STATIC;

    if(ENABLED == pLut_entry->arp_used)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_ARP_USED;

    if(ENABLED == pLut_entry->fwdpri_en)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_FWD_PRI;

    if(ENABLED == pLut_entry->sapri_en)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_LOOKUP_PRI;

    if(ENABLED == pLut_entry->ivl_svl)
        pL2Data->flags |= RTK_L2_UCAST_FLAG_IVL;

    return RT_ERR_OK;
}

static int32
_dal_apollomp_l2uctoRaw(apollomp_lut_table_t *pLut_entry, rtk_l2_ucastAddr_t *pL2Data)
{
    pLut_entry->cvid_fid    = pL2Data->vid;
    pLut_entry->mac.octet[0]= pL2Data->mac.octet[0];
    pLut_entry->mac.octet[1]= pL2Data->mac.octet[1];
    pLut_entry->mac.octet[2]= pL2Data->mac.octet[2];
    pLut_entry->mac.octet[3]= pL2Data->mac.octet[3];
    pLut_entry->mac.octet[4]= pL2Data->mac.octet[4];
    pLut_entry->mac.octet[5]= pL2Data->mac.octet[5];
    pLut_entry->fid         = pL2Data->fid;
    pLut_entry->efid        = pL2Data->efid;
    pLut_entry->spa         = pL2Data->port;
    pLut_entry->ext_dsl_spa = pL2Data->ext_port;
    pLut_entry->age         = pL2Data->age;
    pLut_entry->lut_pri     = pL2Data->priority;
    pLut_entry->auth        = pL2Data->auth;
    pLut_entry->address     = pL2Data->index;


    if (pL2Data->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)
        pLut_entry->sa_block = ENABLED;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)
        pLut_entry->da_block = ENABLED;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_STATIC)
        pLut_entry->nosalearn = ENABLED;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_ARP_USED)
        pLut_entry->arp_used = ENABLED;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_FWD_PRI)
        pLut_entry->fwdpri_en = ENABLED;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_LOOKUP_PRI)
        pLut_entry->sapri_en = ENABLED;

    if (pL2Data->flags & RTK_L2_UCAST_FLAG_IVL)
        pLut_entry->ivl_svl = ENABLED;

    return RT_ERR_OK;
}

static int32
_dal_apollomp_rawtoL2mc(rtk_l2_mcastAddr_t *pL2Data, apollomp_lut_table_t *pLut_entry)
{

    pL2Data->vid            = pLut_entry->cvid_fid;
    pL2Data->mac.octet[0]   = pLut_entry->mac.octet[0];
    pL2Data->mac.octet[1]   = pLut_entry->mac.octet[1];
    pL2Data->mac.octet[2]   = pLut_entry->mac.octet[2];
    pL2Data->mac.octet[3]   = pLut_entry->mac.octet[3];
    pL2Data->mac.octet[4]   = pLut_entry->mac.octet[4];
    pL2Data->mac.octet[5]   = pLut_entry->mac.octet[5];
    pL2Data->priority       = pLut_entry->lut_pri;
    pL2Data->index          = pLut_entry->address;
    pL2Data->portmask.bits[0] = pLut_entry->mbr;
    pL2Data->ext_portmask.bits[0]   = pLut_entry->ext_mbr;

    if(ENABLED == pLut_entry->fwdpri_en)
        pL2Data->flags |= RTK_L2_MCAST_FLAG_FWD_PRI;

    if(ENABLED == pLut_entry->ivl_svl)
    {
        pL2Data->flags |= RTK_L2_MCAST_FLAG_IVL;
        pL2Data->vid   = pLut_entry->cvid_fid;
    }
    else
        pL2Data->fid   = pLut_entry->cvid_fid;

    return RT_ERR_OK;
}

static int32
_dal_apollomp_l2mctoRaw(apollomp_lut_table_t *pLut_entry, rtk_l2_mcastAddr_t *pL2Data)
{
    pLut_entry->mac.octet[0] = pL2Data->mac.octet[0];
    pLut_entry->mac.octet[1] = pL2Data->mac.octet[1];
    pLut_entry->mac.octet[2] = pL2Data->mac.octet[2];
    pLut_entry->mac.octet[3] = pL2Data->mac.octet[3];
    pLut_entry->mac.octet[4] = pL2Data->mac.octet[4];
    pLut_entry->mac.octet[5] = pL2Data->mac.octet[5];
    pLut_entry->ext_mbr      = pL2Data->ext_portmask.bits[0];
    pLut_entry->lut_pri      = pL2Data->priority;
    pLut_entry->address      = pL2Data->index;
    pLut_entry->mbr          = pL2Data->portmask.bits[0];

    if (pL2Data->flags & RTK_L2_MCAST_FLAG_FWD_PRI)
        pLut_entry->fwdpri_en = ENABLED;

    if (pL2Data->flags & RTK_L2_MCAST_FLAG_IVL)
    {
        pLut_entry->cvid_fid = pL2Data->vid;
        pLut_entry->ivl_svl  = ENABLED;
    }
    else
        pLut_entry->cvid_fid = pL2Data->fid;

    pLut_entry->spa = HAL_GET_MIN_PORT();

    return RT_ERR_OK;
}

static int32
_dal_apollomp_rawtoIpmc(rtk_l2_ipMcastAddr_t *pL2Data, apollomp_lut_table_t *pLut_entry)
{
    int32 ret;
    rtk_l2_ipmcMode_t   mode;

    if((ret = dal_apollomp_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    pL2Data->dip            = (pLut_entry->gip | 0xE0000000);

    if(pLut_entry->gip_only)
    {
        pL2Data->flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        pL2Data->l3_trans_index = pLut_entry->l3_idx;

        if(pLut_entry->ext_fr)
            pL2Data->flags |= RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE;

        if(pLut_entry->wan_sa)
            pL2Data->flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    }
    else
    {
        if(LOOKUP_ON_DIP_AND_VID == mode)
        {
            pL2Data->vid = pLut_entry->sip_vid;
        }
        else
            pL2Data->sip = pLut_entry->sip_vid;
    }

    if(pLut_entry->fwdpri_en)
        pL2Data->flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;

    if(pLut_entry->nosalearn)
        pL2Data->flags |= RTK_L2_IPMCAST_FLAG_STATIC;

    pL2Data->portmask.bits[0]     = pLut_entry->mbr;
    pL2Data->ext_portmask.bits[0] = pLut_entry->ext_mbr;
    pL2Data->priority             = pLut_entry->lut_pri;
    pL2Data->index                = pLut_entry->address;

    return RT_ERR_OK;
}

static int32
_dal_apollomp_IpmctoRaw(apollomp_lut_table_t *pLut_entry, rtk_l2_ipMcastAddr_t *pL2Data)
{
    int32 ret;
    rtk_l2_ipmcMode_t   mode;

    if((ret = dal_apollomp_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    pLut_entry->gip =   pL2Data->dip;

    if(pL2Data->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY)
    {
        pLut_entry->gip_only = ENABLED;
        pLut_entry->l3_idx   = pL2Data->l3_trans_index;

        if (pL2Data->flags & RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE)
            pLut_entry->ext_fr = ENABLED;

        if (pL2Data->flags & RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN)
            pLut_entry->wan_sa = ENABLED;
    }
    else
    {
        if(LOOKUP_ON_DIP_AND_VID == mode)
            pLut_entry->sip_vid = pL2Data->vid;
        else
            pLut_entry->sip_vid = pL2Data->sip;
    }

    if(pL2Data->flags & RTK_L2_IPMCAST_FLAG_STATIC)
        pLut_entry->nosalearn = 1;

    pLut_entry->fwdpri_en = ((pL2Data->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? ENABLED : DISABLED);

    pLut_entry->mbr      = pL2Data->portmask.bits[0];
    pLut_entry->ext_mbr  = pL2Data->ext_portmask.bits[0];
    pLut_entry->lut_pri  = pL2Data->priority;
    pLut_entry->address  = pL2Data->index;

    pLut_entry->spa      = HAL_GET_MIN_PORT();

    return RT_ERR_OK;
}

static int _dal_apollomp_l2UcSvlHash(rtk_mac_t mac, uint32 fid, uint32 efid, uint32 *pHashValue)
{
    unsigned char index[9];

    index[0] = 
        ((mac.octet[5] & (1<<0))>>0) ^
        ((mac.octet[4] & (1<<1))>>1) ^
        ((mac.octet[3] & (1<<2))>>2) ^
        ((mac.octet[2] & (1<<3))>>3) ^
        ((mac.octet[1] & (1<<4))>>4) ^
        ((mac.octet[0] & (1<<5))>>5);
    index[1] = 
        ((mac.octet[5] & (1<<1))>>1) ^
        ((mac.octet[4] & (1<<2))>>2) ^
        ((mac.octet[3] & (1<<3))>>3) ^
        ((mac.octet[2] & (1<<4))>>4) ^
        ((mac.octet[1] & (1<<5))>>5) ^
        ((mac.octet[0] & (1<<6))>>6);
    index[2] = 
        ((mac.octet[5] & (1<<2))>>2) ^
        ((mac.octet[4] & (1<<3))>>3) ^
        ((mac.octet[3] & (1<<4))>>4) ^
        ((mac.octet[2] & (1<<5))>>5) ^
        ((mac.octet[1] & (1<<6))>>6) ^
        ((mac.octet[0] & (1<<7))>>7);
    index[3] = 
        ((mac.octet[5] & (1<<3))>>3) ^
        ((mac.octet[4] & (1<<4))>>4) ^
        ((mac.octet[3] & (1<<5))>>5) ^
        ((mac.octet[2] & (1<<6))>>6) ^
        ((mac.octet[1] & (1<<7))>>7) ^
        ((fid & (1<<0))>>0);
    index[4] = 
        ((mac.octet[5] & (1<<4))>>4) ^
        ((mac.octet[4] & (1<<5))>>5) ^
        ((mac.octet[3] & (1<<6))>>6) ^
        ((mac.octet[2] & (1<<7))>>7) ^
        ((mac.octet[0] & (1<<0))>>0) ^
        ((fid & (1<<1))>>1);
    index[5] = 
        ((mac.octet[5] & (1<<5))>>5) ^
        ((mac.octet[4] & (1<<6))>>6) ^
        ((mac.octet[3] & (1<<7))>>7) ^
        ((mac.octet[1] & (1<<0))>>0) ^
        ((mac.octet[0] & (1<<1))>>1) ^
        ((fid & (1<<2))>>2);
    index[6] = 
        ((mac.octet[5] & (1<<6))>>6) ^
        ((mac.octet[4] & (1<<7))>>7) ^
        ((mac.octet[2] & (1<<0))>>0) ^
        ((mac.octet[1] & (1<<1))>>1) ^
        ((mac.octet[0] & (1<<2))>>2) ^
        ((fid & (1<<3))>>3) ^
        ((efid & (1<<0))>>0);
    index[7] = 
        ((mac.octet[5] & (1<<7))>>7) ^
        ((mac.octet[3] & (1<<0))>>0) ^
        ((mac.octet[2] & (1<<1))>>1) ^
        ((mac.octet[1] & (1<<2))>>2) ^
        ((mac.octet[0] & (1<<3))>>3) ^
        ((efid & (1<<1))>>1);
    index[8] = 
        ((mac.octet[4] & (1<<0))>>0) ^
        ((mac.octet[3] & (1<<1))>>1) ^
        ((mac.octet[2] & (1<<2))>>2) ^
        ((mac.octet[1] & (1<<3))>>3) ^
        ((mac.octet[0] & (1<<4))>>4) ^
        ((efid & (1<<2))>>2);

    *pHashValue = 
        (index[8] << 8) | (index[7] << 7) | (index[6] << 6) | (index[5] << 5) |
        (index[4] << 4) | (index[3] << 3) | (index[2] << 2) | (index[1] << 1) |
        (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_apollomp_l2McSvlHash(rtk_mac_t mac, uint32 fid, uint32 *pHashValue)
{
    unsigned char index[9];

    index[0] = 
        ((mac.octet[5] & (1<<0))>>0) ^
        ((mac.octet[4] & (1<<1))>>1) ^
        ((mac.octet[3] & (1<<2))>>2) ^
        ((mac.octet[2] & (1<<3))>>3) ^
        ((mac.octet[1] & (1<<4))>>4) ^
        ((mac.octet[0] & (1<<5))>>5);
    index[1] = 
        ((mac.octet[5] & (1<<1))>>1) ^
        ((mac.octet[4] & (1<<2))>>2) ^
        ((mac.octet[3] & (1<<3))>>3) ^
        ((mac.octet[2] & (1<<4))>>4) ^
        ((mac.octet[1] & (1<<5))>>5) ^
        ((mac.octet[0] & (1<<6))>>6);
    index[2] = 
        ((mac.octet[5] & (1<<2))>>2) ^
        ((mac.octet[4] & (1<<3))>>3) ^
        ((mac.octet[3] & (1<<4))>>4) ^
        ((mac.octet[2] & (1<<5))>>5) ^
        ((mac.octet[1] & (1<<6))>>6) ^
        ((mac.octet[0] & (1<<7))>>7);
    index[3] = 
        ((mac.octet[5] & (1<<3))>>3) ^
        ((mac.octet[4] & (1<<4))>>4) ^
        ((mac.octet[3] & (1<<5))>>5) ^
        ((mac.octet[2] & (1<<6))>>6) ^
        ((mac.octet[1] & (1<<7))>>7) ^
        ((fid & (1<<0))>>0);
    index[4] = 
        ((mac.octet[5] & (1<<4))>>4) ^
        ((mac.octet[4] & (1<<5))>>5) ^
        ((mac.octet[3] & (1<<6))>>6) ^
        ((mac.octet[2] & (1<<7))>>7) ^
        ((mac.octet[0] & (1<<0))>>0) ^
        ((fid & (1<<1))>>1);
    index[5] = 
        ((mac.octet[5] & (1<<5))>>5) ^
        ((mac.octet[4] & (1<<6))>>6) ^
        ((mac.octet[3] & (1<<7))>>7) ^
        ((mac.octet[1] & (1<<0))>>0) ^
        ((mac.octet[0] & (1<<1))>>1) ^
        ((fid & (1<<2))>>2);
    index[6] = 
        ((mac.octet[5] & (1<<6))>>6) ^
        ((mac.octet[4] & (1<<7))>>7) ^
        ((mac.octet[2] & (1<<0))>>0) ^
        ((mac.octet[1] & (1<<1))>>1) ^
        ((mac.octet[0] & (1<<2))>>2) ^
        ((fid & (1<<3))>>3);
    index[7] = 
        ((mac.octet[5] & (1<<7))>>7) ^
        ((mac.octet[3] & (1<<0))>>0) ^
        ((mac.octet[2] & (1<<1))>>1) ^
        ((mac.octet[1] & (1<<2))>>2) ^
        ((mac.octet[0] & (1<<3))>>3);
    index[8] = 
        ((mac.octet[4] & (1<<0))>>0) ^
        ((mac.octet[3] & (1<<1))>>1) ^
        ((mac.octet[2] & (1<<2))>>2) ^
        ((mac.octet[1] & (1<<3))>>3) ^
        ((mac.octet[0] & (1<<4))>>4);

    *pHashValue = 
        (index[8] << 8) | (index[7] << 7) | (index[6] << 6) | (index[5] << 5) |
        (index[4] << 4) | (index[3] << 3) | (index[2] << 2) | (index[1] << 1) |
        (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_apollomp_l2UcIvlHash(rtk_mac_t mac, uint32 vid, uint32 efid, uint32 *pHashValue)
{
    unsigned char index[9];

    index[0] = 
        ((mac.octet[5] & (1<<0))>>0) ^
        ((mac.octet[4] & (1<<1))>>1) ^
        ((mac.octet[3] & (1<<2))>>2) ^
        ((mac.octet[2] & (1<<3))>>3) ^
        ((mac.octet[1] & (1<<4))>>4) ^
        ((mac.octet[0] & (1<<5))>>5) ^
        ((vid & (1<<6))>>6);
    index[1] = 
        ((mac.octet[5] & (1<<1))>>1) ^
        ((mac.octet[4] & (1<<2))>>2) ^
        ((mac.octet[3] & (1<<3))>>3) ^
        ((mac.octet[2] & (1<<4))>>4) ^
        ((mac.octet[1] & (1<<5))>>5) ^
        ((mac.octet[0] & (1<<6))>>6) ^
        ((vid & (1<<7))>>7);
    index[2] = 
        ((mac.octet[5] & (1<<2))>>2) ^
        ((mac.octet[4] & (1<<3))>>3) ^
        ((mac.octet[3] & (1<<4))>>4) ^
        ((mac.octet[2] & (1<<5))>>5) ^
        ((mac.octet[1] & (1<<6))>>6) ^
        ((mac.octet[0] & (1<<7))>>7) ^
        ((vid & (1<<8))>>8);
    index[3] = 
        ((mac.octet[5] & (1<<3))>>3) ^
        ((mac.octet[4] & (1<<4))>>4) ^
        ((mac.octet[3] & (1<<5))>>5) ^
        ((mac.octet[2] & (1<<6))>>6) ^
        ((mac.octet[1] & (1<<7))>>7) ^
        ((vid & (1<<0))>>0) ^
        ((vid & (1<<9))>>9);
    index[4] = 
        ((mac.octet[5] & (1<<4))>>4) ^
        ((mac.octet[4] & (1<<5))>>5) ^
        ((mac.octet[3] & (1<<6))>>6) ^
        ((mac.octet[2] & (1<<7))>>7) ^
        ((mac.octet[0] & (1<<0))>>0) ^
        ((vid & (1<<1))>>1) ^
        ((vid & (1<<10))>>10);
    index[5] = 
        ((mac.octet[5] & (1<<5))>>5) ^
        ((mac.octet[4] & (1<<6))>>6) ^
        ((mac.octet[3] & (1<<7))>>7) ^
        ((mac.octet[1] & (1<<0))>>0) ^
        ((mac.octet[0] & (1<<1))>>1) ^
        ((vid & (1<<2))>>2) ^
        ((vid & (1<<11))>>11);
    index[6] = 
        ((mac.octet[5] & (1<<6))>>6) ^
        ((mac.octet[4] & (1<<7))>>7) ^
        ((mac.octet[2] & (1<<0))>>0) ^
        ((mac.octet[1] & (1<<1))>>1) ^
        ((mac.octet[0] & (1<<2))>>2) ^
        ((vid & (1<<3))>>3) ^
        ((efid & (1<<0))>>0);
    index[7] = 
        ((mac.octet[5] & (1<<7))>>7) ^
        ((mac.octet[3] & (1<<0))>>0) ^
        ((mac.octet[2] & (1<<1))>>1) ^
        ((mac.octet[1] & (1<<2))>>2) ^
        ((mac.octet[0] & (1<<3))>>3) ^
        ((vid & (1<<4))>>4) ^
        ((efid & (1<<1))>>1);
    index[8] = 
        ((mac.octet[4] & (1<<0))>>0) ^
        ((mac.octet[3] & (1<<1))>>1) ^
        ((mac.octet[2] & (1<<2))>>2) ^
        ((mac.octet[1] & (1<<3))>>3) ^
        ((mac.octet[0] & (1<<4))>>4) ^
        ((vid & (1<<5))>>5) ^
        ((efid & (1<<2))>>2);

    *pHashValue = 
        (index[8] << 8) | (index[7] << 7) | (index[6] << 6) | (index[5] << 5) |
        (index[4] << 4) | (index[3] << 3) | (index[2] << 2) | (index[1] << 1) |
        (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_apollomp_l2McIvlHash(rtk_mac_t mac, uint32 vid, uint32 *pHashValue)
{
    unsigned char index[9];

    index[0] = 
        ((mac.octet[5] & (1<<0))>>0) ^
        ((mac.octet[4] & (1<<1))>>1) ^
        ((mac.octet[3] & (1<<2))>>2) ^
        ((mac.octet[2] & (1<<3))>>3) ^
        ((mac.octet[1] & (1<<4))>>4) ^
        ((mac.octet[0] & (1<<5))>>5) ^
        ((vid & (1<<6))>>6);
    index[1] = 
        ((mac.octet[5] & (1<<1))>>1) ^
        ((mac.octet[4] & (1<<2))>>2) ^
        ((mac.octet[3] & (1<<3))>>3) ^
        ((mac.octet[2] & (1<<4))>>4) ^
        ((mac.octet[1] & (1<<5))>>5) ^
        ((mac.octet[0] & (1<<6))>>6) ^
        ((vid & (1<<7))>>7);
    index[2] = 
        ((mac.octet[5] & (1<<2))>>2) ^
        ((mac.octet[4] & (1<<3))>>3) ^
        ((mac.octet[3] & (1<<4))>>4) ^
        ((mac.octet[2] & (1<<5))>>5) ^
        ((mac.octet[1] & (1<<6))>>6) ^
        ((mac.octet[0] & (1<<7))>>7) ^
        ((vid & (1<<8))>>8);
    index[3] = 
        ((mac.octet[5] & (1<<3))>>3) ^
        ((mac.octet[4] & (1<<4))>>4) ^
        ((mac.octet[3] & (1<<5))>>5) ^
        ((mac.octet[2] & (1<<6))>>6) ^
        ((mac.octet[1] & (1<<7))>>7) ^
        ((vid & (1<<0))>>0) ^
        ((vid & (1<<9))>>9);
    index[4] = 
        ((mac.octet[5] & (1<<4))>>4) ^
        ((mac.octet[4] & (1<<5))>>5) ^
        ((mac.octet[3] & (1<<6))>>6) ^
        ((mac.octet[2] & (1<<7))>>7) ^
        ((mac.octet[0] & (1<<0))>>0) ^
        ((vid & (1<<1))>>1) ^
        ((vid & (1<<10))>>10);
    index[5] = 
        ((mac.octet[5] & (1<<5))>>5) ^
        ((mac.octet[4] & (1<<6))>>6) ^
        ((mac.octet[3] & (1<<7))>>7) ^
        ((mac.octet[1] & (1<<0))>>0) ^
        ((mac.octet[0] & (1<<1))>>1) ^
        ((vid & (1<<2))>>2) ^
        ((vid & (1<<11))>>11);
    index[6] = 
        ((mac.octet[5] & (1<<6))>>6) ^
        ((mac.octet[4] & (1<<7))>>7) ^
        ((mac.octet[2] & (1<<0))>>0) ^
        ((mac.octet[1] & (1<<1))>>1) ^
        ((mac.octet[0] & (1<<2))>>2) ^
        ((vid & (1<<3))>>3);
    index[7] = 
        ((mac.octet[5] & (1<<7))>>7) ^
        ((mac.octet[3] & (1<<0))>>0) ^
        ((mac.octet[2] & (1<<1))>>1) ^
        ((mac.octet[1] & (1<<2))>>2) ^
        ((mac.octet[0] & (1<<3))>>3) ^
        ((vid & (1<<4))>>4);
    index[8] = 
        ((mac.octet[4] & (1<<0))>>0) ^
        ((mac.octet[3] & (1<<1))>>1) ^
        ((mac.octet[2] & (1<<2))>>2) ^
        ((mac.octet[1] & (1<<3))>>3) ^
        ((mac.octet[0] & (1<<4))>>4) ^
        ((vid & (1<<5))>>5);

    *pHashValue = 
        (index[8] << 8) | (index[7] << 7) | (index[6] << 6) | (index[5] << 5) |
        (index[4] << 4) | (index[3] << 3) | (index[2] << 2) | (index[1] << 1) |
        (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_apollomp_l3SipGipHash(rtk_ip_addr_t sip, rtk_ip_addr_t gip, uint32 *pHashValue)
{
    unsigned char index[9];
    unsigned char gip_addr[4] = { ((unsigned char *)(&gip))[3], ((unsigned char *)(&gip))[2], ((unsigned char *)(&gip))[1], ((unsigned char *)(&gip))[0] };
    unsigned char sip_addr[4] = { ((unsigned char *)(&sip))[3], ((unsigned char *)(&sip))[2], ((unsigned char *)(&sip))[1], ((unsigned char *)(&sip))[0] };

    index[0] = 
        ((gip_addr[3] & (1<<0))>>0) ^
        ((gip_addr[2] & (1<<1))>>1) ^
        ((gip_addr[1] & (1<<2))>>2) ^
        ((gip_addr[0] & (1<<3))>>3) ^
        ((sip_addr[2] & (1<<0))>>0) ^
        ((sip_addr[1] & (1<<1))>>1) ^
        ((sip_addr[0] & (1<<2))>>2);
    index[1] = 
        ((gip_addr[3] & (1<<1))>>1) ^
        ((gip_addr[2] & (1<<2))>>2) ^
        ((gip_addr[1] & (1<<3))>>3) ^
        ((sip_addr[3] & (1<<0))>>0) ^
        ((sip_addr[2] & (1<<1))>>1) ^
        ((sip_addr[1] & (1<<2))>>2) ^
        ((sip_addr[0] & (1<<3))>>3);
    index[2] = 
        ((gip_addr[3] & (1<<2))>>2) ^
        ((gip_addr[2] & (1<<3))>>3) ^
        ((gip_addr[1] & (1<<4))>>4) ^
        ((sip_addr[3] & (1<<1))>>1) ^
        ((sip_addr[2] & (1<<2))>>2) ^
        ((sip_addr[1] & (1<<3))>>3) ^
        ((sip_addr[0] & (1<<4))>>4);
    index[3] = 
        ((gip_addr[3] & (1<<3))>>3) ^
        ((gip_addr[2] & (1<<4))>>4) ^
        ((gip_addr[1] & (1<<5))>>5) ^
        ((sip_addr[3] & (1<<2))>>2) ^
        ((sip_addr[2] & (1<<3))>>3) ^
        ((sip_addr[1] & (1<<4))>>4) ^
        ((sip_addr[0] & (1<<5))>>5);
    index[4] = 
        ((gip_addr[3] & (1<<4))>>4) ^
        ((gip_addr[2] & (1<<5))>>5) ^
        ((gip_addr[1] & (1<<6))>>6) ^
        ((sip_addr[3] & (1<<3))>>3) ^
        ((sip_addr[2] & (1<<4))>>4) ^
        ((sip_addr[1] & (1<<5))>>5) ^
        ((sip_addr[0] & (1<<6))>>6);
    index[5] = 
        ((gip_addr[3] & (1<<5))>>5) ^
        ((gip_addr[2] & (1<<6))>>6) ^
        ((gip_addr[1] & (1<<7))>>7) ^
        ((sip_addr[3] & (1<<4))>>4) ^
        ((sip_addr[2] & (1<<5))>>5) ^
        ((sip_addr[1] & (1<<6))>>6) ^
        ((sip_addr[0] & (1<<7))>>7);
    index[6] = 
        ((gip_addr[3] & (1<<6))>>6) ^
        ((gip_addr[2] & (1<<7))>>7) ^
        ((gip_addr[0] & (1<<0))>>0) ^
        ((sip_addr[3] & (1<<5))>>5) ^
        ((sip_addr[2] & (1<<6))>>6) ^
        ((sip_addr[1] & (1<<7))>>7);
    index[7] = 
        ((gip_addr[3] & (1<<7))>>7) ^
        ((gip_addr[1] & (1<<0))>>0) ^
        ((gip_addr[0] & (1<<1))>>1) ^
        ((sip_addr[3] & (1<<6))>>6) ^
        ((sip_addr[2] & (1<<7))>>7) ^
        ((sip_addr[0] & (1<<0))>>0);
    index[8] = 
        ((gip_addr[2] & (1<<0))>>0) ^
        ((gip_addr[1] & (1<<1))>>1) ^
        ((gip_addr[0] & (1<<2))>>2) ^
        ((sip_addr[3] & (1<<7))>>7) ^
        ((sip_addr[1] & (1<<0))>>0) ^
        ((sip_addr[0] & (1<<1))>>1);

    *pHashValue = 
        (index[8] << 8) | (index[7] << 7) | (index[6] << 6) | (index[5] << 5) |
        (index[4] << 4) | (index[3] << 3) | (index[2] << 2) | (index[1] << 1) |
        (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

static int _dal_apollomp_l3GipVidHash(rtk_ip_addr_t gip, uint32 vid, uint32 *pHashValue)
{
    unsigned char index[9];
    unsigned char gip_addr[4] = { ((unsigned char *)(&gip))[3], ((unsigned char *)(&gip))[2], ((unsigned char *)(&gip))[1], ((unsigned char *)(&gip))[0] };

    index[0] = 
        ((gip_addr[3] & (1<<0))>>0) ^
        ((gip_addr[2] & (1<<1))>>1) ^
        ((gip_addr[1] & (1<<2))>>2) ^
        ((gip_addr[0] & (1<<3))>>3) ^
        ((vid & (1<<8))>>8);
    index[1] = 
        ((gip_addr[3] & (1<<1))>>1) ^
        ((gip_addr[2] & (1<<2))>>2) ^
        ((gip_addr[1] & (1<<3))>>3) ^
        ((vid & (1<<0))>>0) ^
        ((vid & (1<<9))>>9);
    index[2] = 
        ((gip_addr[3] & (1<<2))>>2) ^
        ((gip_addr[2] & (1<<3))>>3) ^
        ((gip_addr[1] & (1<<4))>>4) ^
        ((vid & (1<<1))>>1) ^
        ((vid & (1<<10))>>10);
    index[3] = 
        ((gip_addr[3] & (1<<3))>>3) ^
        ((gip_addr[2] & (1<<4))>>4) ^
        ((gip_addr[1] & (1<<5))>>5) ^
        ((vid & (1<<2))>>2) ^
        ((vid & (1<<11))>>11);
    index[4] = 
        ((gip_addr[3] & (1<<4))>>4) ^
        ((gip_addr[2] & (1<<5))>>5) ^
        ((gip_addr[1] & (1<<6))>>6) ^
        ((vid & (1<<3))>>3);
    index[5] = 
        ((gip_addr[3] & (1<<5))>>5) ^
        ((gip_addr[2] & (1<<6))>>6) ^
        ((gip_addr[1] & (1<<7))>>7) ^
        ((vid & (1<<4))>>4);
    index[6] = 
        ((gip_addr[3] & (1<<6))>>6) ^
        ((gip_addr[2] & (1<<7))>>7) ^
        ((gip_addr[0] & (1<<0))>>0) ^
        ((vid & (1<<5))>>5);
    index[7] = 
        ((gip_addr[3] & (1<<7))>>7) ^
        ((gip_addr[1] & (1<<0))>>0) ^
        ((gip_addr[0] & (1<<1))>>1) ^
        ((vid & (1<<6))>>6);
    index[8] = 
        ((gip_addr[2] & (1<<0))>>0) ^
        ((gip_addr[1] & (1<<1))>>1) ^
        ((gip_addr[0] & (1<<2))>>2) ^
        ((vid & (1<<7))>>7);

    *pHashValue = 
        (index[8] << 8) | (index[7] << 7) | (index[6] << 6) | (index[5] << 5) |
        (index[4] << 4) | (index[3] << 3) | (index[2] << 2) | (index[1] << 1) |
        (index[0] << 0);
    *pHashValue = *pHashValue << 2;

    return RT_ERR_OK;
}

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_apollomp_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
dal_apollomp_l2_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_portmask_t all_portmask;
    rtk_portmask_t all_extPortmask;
    uint32 index;
    apollomp_raw_flush_ctrl_t apollomp_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    l2_init = INIT_COMPLETED;

    if((ret = dal_apollomp_l2_flushLinkDownPortAddrEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_table_clear()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_l2_portLimitLearningCnt_set(port, HAL_L2_LEARN_LIMIT_CNT_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_l2_portLimitLearningCntAction_set(port, LIMIT_LEARN_CNT_ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_l2_portAgingEnable_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_l2_newMacOp_set(port, HARDWARE_LEARNING, ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_l2_illegalPortMoveAction_set(port, ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_l2_limitLearningCnt_set(HAL_L2_LEARN_LIMIT_CNT_MAX())) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_aging_set(RTK_L2_DEFAULT_AGING_TIME)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_lookupMissAction_set(DLF_TYPE_IPMC, ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_lookupMissAction_set(DLF_TYPE_MCAST, ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_lookupMissAction_set(DLF_TYPE_UCAST, ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_GET_ALL_PORTMASK(all_portmask);
    if((ret = dal_apollomp_l2_lookupMissFloodPortMask_set(DLF_TYPE_IPMC, &all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST, &all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST, &all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_ipmcMode_set(LOOKUP_ON_MAC_AND_VID_FID)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_l2_ipmcGroupLookupMissHash_set(HASH_DIP_ONLY)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = apollomp_raw_l2_unknReservedMcFlood_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    for(index = 0; index <= APOLLOMP_IPMC_TABLE_IDX_MAX; index++)
    {
        if((ret = apollomp_raw_l2_igmp_Mc_table_set(index, 0xE0000000, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            l2_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_l2_srcPortEgrFilterMask_set(&all_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_GET_ALL_EXT_PORTMASK(all_extPortmask);
    RTK_PORTMASK_PORT_CLEAR(all_extPortmask, HAL_GET_EXT_CPU_PORT());
    if((ret = dal_apollomp_l2_extPortEgrFilterMask_set(&all_extPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Set flush parameter as dynamic only */
    if((ret = apollomp_raw_l2_flushCtrl_get(&apollomp_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    apollomp_cfg.flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;

    if((ret = apollomp_raw_l2_flushCtrl_set(&apollomp_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        l2_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_init */

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      dal_apollomp_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l2_flushLinkDownPortAddrEnable_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_flushLinkDownPortAddrEnable_get */


/* Function Name:
 *      dal_apollomp_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration.
 * Input:
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = apollomp_raw_l2_flushLinkDownPortAddrEnable_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_flushLinkDownPortAddrEnable_set */

/* Function Name:
 *      dal_apollomp_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_l2_ucastAddr_flush(rtk_l2_flushCfg_t *pConfig)
{
    int32   ret;
    int32   back_ret;
    apollomp_raw_flush_ctrl_t flush_ctrl;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pConfig->flushByVid >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pConfig->flushByPort >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(pConfig->port), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushByFid >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->fid > HAL_VLAN_FID_MAX()), RT_ERR_FID);
    RT_PARAM_CHK((pConfig->flushStaticAddr >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushDynamicAddr >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushAddrOnAllPorts >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* Error Configuration Check: can't support both VID and FID flush */
    if( (ENABLED == pConfig->flushByVid) && (ENABLED == pConfig->flushByFid) )
        return RT_ERR_CHIP_NOT_SUPPORTED;

    /* Error Configuration Check: must specify port(s). */
    if( (DISABLED == pConfig->flushByPort) && (DISABLED == pConfig->flushAddrOnAllPorts) )
        return RT_ERR_INPUT;

    /* Set Configuration */
    osal_memset(&flush_ctrl, 0x00, sizeof(apollomp_raw_flush_ctrl_t));
    if(ENABLED == pConfig->flushByVid)
    {
        flush_ctrl.flushMode = APOLLOMP_RAW_FLUSH_MODE_VID;
        flush_ctrl.vid = pConfig->vid;
    }
    else if(ENABLED == pConfig->flushByFid)
    {
        flush_ctrl.flushMode = APOLLOMP_RAW_FLUSH_MODE_FID;
        flush_ctrl.fid = pConfig->fid;
    }
    else if(ENABLED == pConfig->flushByPort)
    {
        flush_ctrl.flushMode = APOLLOMP_RAW_FLUSH_MODE_PORT;
    }

    if( (ENABLED == pConfig->flushStaticAddr) && (ENABLED == pConfig->flushDynamicAddr) )
        flush_ctrl.flushType = APOLLOMP_RAW_FLUSH_TYPE_BOTH;
    else if(ENABLED == pConfig->flushStaticAddr)
        flush_ctrl.flushType = APOLLOMP_RAW_FLUSH_TYPE_STATIC;
    else if(ENABLED == pConfig->flushDynamicAddr)
        flush_ctrl.flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;
    else
        return RT_ERR_INPUT;

    if((ret = apollomp_raw_l2_flushCtrl_set(&flush_ctrl)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    /* Start Flushing */
    if(ENABLED == pConfig->flushByPort)
    {
        if((ret = apollomp_raw_l2_flushEn_set(pConfig->port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        }
    }
    else if(ENABLED == pConfig->flushAddrOnAllPorts)
    {
        HAL_SCAN_ALL_PORT(port)
        {
            if((ret = apollomp_raw_l2_flushEn_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                break;
            }
        }
    }

    flush_ctrl.flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;
    if((back_ret = apollomp_raw_l2_flushCtrl_set(&flush_ctrl)) != RT_ERR_OK)
    {
        RT_ERR(back_ret, (MOD_L2 | MOD_DAL), "");
        return back_ret;
    }

    return ret;
} /* end of dal_apollomp_l2_ucastAddr_flush */

/* Function Name:
 *      dal_apollomp_l2_table_clear
 * Description:
 *      Clear entire L2 table.
 *      All the entries (static and dynamic) (L2 and L3) will be deleted.
 * Input:
 *      None.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_apollomp_l2_table_clear(void)
{
    int32   ret;
    uint32  index = 0;
    apollomp_lut_table_t lut_entry;

    do
    {
        osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
        lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS;
        lut_entry.address = index;

        if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        if(0 == lut_entry.lookup_hit)
        {
            /* table empty */
            break;
        }

        if(lut_entry.address < index)
        {
            /* all table clear and hit new-learn entry */
            break;
        }

        /* Delete this entry */
        if(lut_entry.l3lookup == 0)
        {
            /* L2 */
            if((lut_entry.mac.octet[0] & 0x01) == 0)
            {
                /* Unicast */
                lut_entry.nosalearn  = DISABLED;
                lut_entry.age        = 0;
            }
            else
            {
                /* Multicast */
                lut_entry.mbr = 0x00;
                lut_entry.nosalearn = DISABLED;
            }
        }
        else
        {
            /* L3 */
            lut_entry.nosalearn = DISABLED;
            lut_entry.l3lookup = ENABLED;
        }

        lut_entry.spa        = HAL_GET_MIN_PORT();
        if ((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        /* Assign new index */
        index = lut_entry.address + 1;
    }
    while(1);

    return RT_ERR_OK;
}   /* end of dal_apollomp_l2_table_clear */

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      dal_apollomp_l2_limitLearningOverStatus_get
 * Description:
 *      Get the system learning over status
 * Input:
 *      None.
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_apollomp_l2_limitLearningOverStatus_get(uint32 *pStatus)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_L2_SYS_LRN_OVER_STSr, APOLLOMP_LRN_OVER_INDf, pStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_l2_limitLearningOverStatus_clear
 * Description:
 *      Clear the system learning over status
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
dal_apollomp_l2_limitLearningOverStatus_clear(void)
{
    int32   ret;
    uint32 regData = 1;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    if((ret = reg_field_write(APOLLOMP_L2_SYS_LRN_OVER_STSr, APOLLOMP_LRN_OVER_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_l2_learningCnt_get
 * Description:
 *      Get the total mac learning counts of the whole specified device.
 * Input:
 *      None.
 * Output:
 *      pMacCnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_apollomp_l2_learningCnt_get(uint32 *pMacCnt)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* get value from CHIP*/
    if((ret = reg_field_read(APOLLOMP_L2_SYS_LRN_CNTr, APOLLOMP_SYS_L2_LRN_CNTf, pMacCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_learningCnt_get */

/* Function Name:
 *      dal_apollomp_l2_limitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *      address, not apply to static mac address.
 */
int32
dal_apollomp_l2_limitLearningCnt_get(uint32 *pMacCnt)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* get value from CHIP*/
    if((ret = reg_field_read(APOLLOMP_LUT_SYS_LRN_LIMITNOr, APOLLOMP_SYS_LRN_LIMITNOf, pMacCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_limitLearningCnt_get */

/* Function Name:
 *      dal_apollomp_l2_limitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the specified device.
 * Input:
 *      macCnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *      address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the system.
 */
int32
dal_apollomp_l2_limitLearningCnt_set(uint32 macCnt)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(macCnt > HAL_L2_LEARN_LIMIT_CNT_MAX(), RT_ERR_LIMITED_L2ENTRY_NUM);

    /* programming value into CHIP*/
    if((ret = reg_field_write(APOLLOMP_LUT_SYS_LRN_LIMITNOr, APOLLOMP_SYS_LRN_LIMITNOf, &macCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_limitLearningCnt_set */

/* Function Name:
 *      dal_apollomp_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
dal_apollomp_l2_limitLearningCntAction_get(rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    int32   ret;
    uint32  act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    /* get value from CHIP*/
    if((ret = reg_field_read(APOLLOMP_LUT_SYS_LRN_OVER_CTRLr, APOLLOMP_ACTf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* translate register value to action */
    switch (act)
    {
        case 0:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_FORWARD;
            break;
        case 1:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_DROP;
            break;
        case 2:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_TO_CPU;
            break;
        case 3:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_COPY_CPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_limitLearningCntAction_get */

/* Function Name:
 *      dal_apollomp_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the specified device.
 * Input:
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      (1) The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
dal_apollomp_l2_limitLearningCntAction_set(rtk_l2_limitLearnCntAction_t learningAction)
{
    int32   ret;
    uint32  act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(learningAction >= LIMIT_LEARN_CNT_ACTION_END, RT_ERR_INPUT);

    switch (learningAction)
    {
        case LIMIT_LEARN_CNT_ACTION_DROP:
            act = 1;
            break;
        case LIMIT_LEARN_CNT_ACTION_FORWARD:
            act = 0;
            break;
        case LIMIT_LEARN_CNT_ACTION_TO_CPU:
            act = 2;
            break;
        case LIMIT_LEARN_CNT_ACTION_COPY_CPU:
            act = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* programming value to CHIP*/
    if((ret = reg_field_write(APOLLOMP_LUT_SYS_LRN_OVER_CTRLr, APOLLOMP_ACTf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_limitLearningCntAction_set */

/* Function Name:
 *      dal_apollomp_l2_portLimitLearningOverStatus_get
 * Description:
 *      Get the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_apollomp_l2_portLimitLearningOverStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_L2_LRN_OVER_STSr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_LRN_OVER_INDf, pStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_l2_portLimitLearningOverStatus_clear
 * Description:
 *      Clear the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
dal_apollomp_l2_portLimitLearningOverStatus_clear(rtk_port_t port)
{
    int32   ret;
    uint32 regData = 1;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if((ret = reg_array_field_write(APOLLOMP_L2_LRN_OVER_STSr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_LRN_OVER_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_l2_portLearningCnt_get
 * Description:
 *      Get the mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt  - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_apollomp_l2_portLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l2_lrnCnt_get(port, pMacCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portLearningCnt_get */



/* Function Name:
 *      dal_apollomp_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
dal_apollomp_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l2_lrnLimitNo_get(port, pMacCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portLimitLearningCnt_get */


/* Function Name:
 *      dal_apollomp_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
int32
dal_apollomp_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX() < macCnt), RT_ERR_INPUT);

    if((ret = apollomp_raw_l2_lrnLimitNo_set(port, macCnt)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portLimitLearningCnt_set */


/* Function Name:
 *      dal_apollomp_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      port    - port id
 * Output:
 *      pLearningAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
dal_apollomp_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pLearningAction)
{
    int32   ret;
    rtk_action_t act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLearningAction), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l2_learnOverAct_get(port, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    switch(act)
    {
        case ACTION_FORWARD:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_FORWARD;
            break;
        case ACTION_DROP:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_DROP;
            break;
        case ACTION_TRAP2CPU:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_TO_CPU;
            break;
        case ACTION_COPY2CPU:
            *pLearningAction = LIMIT_LEARN_CNT_ACTION_COPY_CPU;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      dal_apollomp_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      port   - port id
 *      learningAction - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
dal_apollomp_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t learningAction)
{

    int32   ret;
    rtk_action_t act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LIMIT_LEARN_CNT_ACTION_END <= learningAction), RT_ERR_INPUT);

    switch(learningAction)
    {
        case LIMIT_LEARN_CNT_ACTION_FORWARD:
            act = ACTION_FORWARD;
            break;
        case LIMIT_LEARN_CNT_ACTION_DROP:
            act = ACTION_DROP;
            break;
        case LIMIT_LEARN_CNT_ACTION_TO_CPU:
            act = ACTION_TRAP2CPU;
            break;
        case LIMIT_LEARN_CNT_ACTION_COPY_CPU:
            act = ACTION_COPY2CPU;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    if((ret = apollomp_raw_l2_learnOverAct_set(port, act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portLimitLearningCntAction_set */



/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */

/* Function Name:
 *      dal_apollomp_l2_aging_get
 * Description:
 *      Get the dynamic address aging time.
 * Input:
 *      None
 * Output:
 *      pAgingTime - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (0.1sec)
 */
int32
dal_apollomp_l2_aging_get(uint32 *pAgingTime)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAgingTime), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l2_agingTime_get(pAgingTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_aging_get */


/* Function Name:
 *      dal_apollomp_l2_aging_set
 * Description:
 *      Set the dynamic address aging time.
 * Input:
 *      agingTime - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) RTL8329/RTL8389 aging time is not configurable.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 */
int32
dal_apollomp_l2_aging_set(uint32 agingTime)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L2_AGING_TIME_MAX() < agingTime), RT_ERR_INPUT);

    if((ret = apollomp_raw_l2_agingTime_set(agingTime)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_aging_set */


/* Function Name:
 *      dal_apollomp_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out setting of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of Age
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_l2_agingEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portAgingEnable_get */

/* Function Name:
 *      dal_apollomp_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      port    - port id
 *      enable  - enable status of Age
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = apollomp_raw_l2_agingEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_portAgingEnable_set */

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      dal_apollomp_l2_lookupMissAction_get
 * Description:
 *      Get forwarding action when destination address lookup miss.
 * Input:
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
int32
dal_apollomp_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((DLF_TYPE_BCAST == type), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case DLF_TYPE_IPMC:
            if((ret = apollomp_raw_unkn_ip4Mc_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_IP6MC:
            if((ret = apollomp_raw_unkn_ip6Mc_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if((ret = apollomp_raw_l2_unkn_ucDaCtl_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_MCAST:
            if((ret = apollomp_raw_unkn_l2Mc_get(0, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_lookupMissAction_get */

/* Function Name:
 *      dal_apollomp_l2_lookupMissAction_set
 * Description:
 *      Set forwarding action when destination address lookup miss.
 * Input:
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_BCAST
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FLOOD_IN_VLAN
 *      - ACTION_FLOOD_IN_ALL_PORT  (only for DLF_TYPE_MCAST)
 *      - ACTION_FLOOD_IN_ROUTER_PORTS (only for DLF_TYPE_IPMC)
 */
int32
dal_apollomp_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32   ret;
    rtk_action_t raw_act;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((DLF_TYPE_BCAST == type), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((ACTION_END <= action), RT_ERR_INPUT);

    switch (action)
    {
        case ACTION_FORWARD:
        case ACTION_DROP:
        case ACTION_TRAP2CPU:
            raw_act = action;
            break;
        case ACTION_DROP_EXCLUDE_RMA:
            if(DLF_TYPE_MCAST != type)
                return RT_ERR_CHIP_NOT_SUPPORTED;
            else
                raw_act = action;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch(type)
    {
        case DLF_TYPE_IPMC:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_unkn_ip4Mc_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_IP6MC:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_unkn_ip6Mc_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_UCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_l2_unkn_ucDaCtl_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_MCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_unkn_l2Mc_set(port, raw_act)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_lookupMissAction_set */

/* Function Name:
 *      dal_apollomp_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
dal_apollomp_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case DLF_TYPE_IPMC:
            if((ret = apollomp_raw_unkn_ip4Mc_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_IP6MC:
            if((ret = apollomp_raw_unkn_ip6Mc_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if((ret = apollomp_raw_l2_unkn_ucDaCtl_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_MCAST:
            if((ret = apollomp_raw_unkn_l2Mc_get(port, pAction)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;

} /* end of dal_apollomp_l2_portLookupMissAction_get */

/* Function Name:
 *      dal_apollomp_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
dal_apollomp_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32   ret;
    rtk_action_t raw_act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    switch (action)
    {
        case ACTION_FORWARD:
        case ACTION_DROP:
        case ACTION_TRAP2CPU:
            raw_act = action;
            break;
        case ACTION_DROP_EXCLUDE_RMA:
            if(type != DLF_TYPE_MCAST)
                return RT_ERR_CHIP_NOT_SUPPORTED;
            else
                raw_act = action;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    switch(type)
    {
        case DLF_TYPE_IPMC:
            if((ret = apollomp_raw_unkn_ip4Mc_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_IP6MC:
            if((ret = apollomp_raw_unkn_ip6Mc_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if((ret = apollomp_raw_l2_unkn_ucDaCtl_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_MCAST:
            if((ret = apollomp_raw_unkn_l2Mc_set(port, raw_act)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;

} /* end of dal_apollomp_l2_portLookupMissAction_set */

/* Function Name:
 *      dal_apollomp_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type   - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
dal_apollomp_l2_lookupMissFloodPortMask_get(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    rtk_port_t port;
    rtk_enable_t state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlood_portmask), RT_ERR_NULL_POINTER);

    /* Clear Portmask */
    RTK_PORTMASK_RESET((*pFlood_portmask));

    switch(type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_l2_unknMcFlood_get(port, &state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                if(ENABLED == state)
                    RTK_PORTMASK_PORT_SET((*pFlood_portmask), port);
            }
            break;
        case DLF_TYPE_UCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_l2_unknUcFlood_get(port, &state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                if(ENABLED == state)
                    RTK_PORTMASK_PORT_SET((*pFlood_portmask), port);
            }
            break;
        case DLF_TYPE_BCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                if((ret = apollomp_raw_l2_bcFlood_get(port, &state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                if(ENABLED == state)
                    RTK_PORTMASK_PORT_SET((*pFlood_portmask), port);
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_lookupMissFloodPortMask_get */

/* Function Name:
 *      dal_apollomp_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
dal_apollomp_l2_lookupMissFloodPortMask_set(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    rtk_port_t port;
    rtk_enable_t state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFlood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pFlood_portmask)), RT_ERR_PORT_MASK);

    switch(type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                state = (RTK_PORTMASK_IS_PORT_SET((*pFlood_portmask), port)) ? ENABLED : DISABLED;

                if((ret = apollomp_raw_l2_unknMcFlood_set(port, state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_UCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                state = (RTK_PORTMASK_IS_PORT_SET((*pFlood_portmask), port)) ? ENABLED : DISABLED;

                if((ret = apollomp_raw_l2_unknUcFlood_set(port, state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        case DLF_TYPE_BCAST:
            HAL_SCAN_ALL_PORT(port)
            {
                state = (RTK_PORTMASK_IS_PORT_SET((*pFlood_portmask), port)) ? ENABLED : DISABLED;

                if((ret = apollomp_raw_l2_bcFlood_set(port, state)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_lookupMissFloodPortMask_set */

/* Function Name:
 *      dal_apollomp_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
dal_apollomp_l2_lookupMissFloodPortMask_add(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(flood_port), RT_ERR_PORT_ID);

    switch (type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            if ((ret = apollomp_raw_l2_unknMcFlood_set(flood_port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if ((ret = apollomp_raw_l2_unknUcFlood_set(flood_port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_BCAST:
            if ((ret = apollomp_raw_l2_bcFlood_set(flood_port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_lookupMissFloodPortMask_add */

/* Function Name:
 *      dal_apollomp_l2_lookupMissFloodPortMask_del
 * Description:
 *      Del one port member in flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration..
 */
int32
dal_apollomp_l2_lookupMissFloodPortMask_del(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((DLF_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(flood_port), RT_ERR_PORT_ID);


    switch (type)
    {
        case DLF_TYPE_IPMC:
        case DLF_TYPE_IP6MC:
        case DLF_TYPE_MCAST:
            if ((ret = apollomp_raw_l2_unknMcFlood_set(flood_port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_UCAST:
            if ((ret = apollomp_raw_l2_unknUcFlood_set(flood_port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        case DLF_TYPE_BCAST:
            if ((ret = apollomp_raw_l2_bcFlood_set(flood_port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_lookupMissFloodPortMask_del */

/* Module Name    : L2      */
/* Sub-module Name: Unicast */
/* Function Name:
 *      dal_apollomp_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
dal_apollomp_l2_newMacOp_get(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLrnMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_l2_unkn_saCtl_get(port, pFwdAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pLrnMode = HARDWARE_LEARNING;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_newMacOp_get */

/* Function Name:
 *      dal_apollomp_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
dal_apollomp_l2_newMacOp_set(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((LEARNING_MODE_END <= lrnMode), RT_ERR_INPUT);
    RT_PARAM_CHK((HARDWARE_LEARNING != lrnMode), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= fwdAction), RT_ERR_INPUT);

    if ((ret = apollomp_raw_l2_unkn_saCtl_set(port, fwdAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_newMacOp_set */


/* Module Name    : L2              */
/* Sub-module Name: Get next entry */

/* Function Name:
 *      dal_apollomp_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2UcastData   - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
dal_apollomp_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2UcastData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((*pScanIdx >= HAL_L2_LEARN_LIMIT_CNT_MAX()), RT_ERR_L2_INDEXTABLE_INDEX);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UC;
    lut_entry.address = *pScanIdx;
    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2UcastData, 0x00, sizeof(rtk_l2_ucastAddr_t));
    if ((ret = _dal_apollomp_rawtoL2uc(pL2UcastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_nextValidAddr_get */


/* Function Name:
 *      dal_apollomp_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2UcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
dal_apollomp_l2_nextValidAddrOnPort_get(
    rtk_port_t          port,
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2UcastData)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2UcastData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((*pScanIdx >= HAL_L2_LEARN_LIMIT_CNT_MAX()), RT_ERR_L2_INDEXTABLE_INDEX);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UCSPA;
    lut_entry.address = *pScanIdx;
    lut_entry.spa = (uint32)port;
    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2UcastData, 0x00, sizeof(rtk_l2_ucastAddr_t));
    if ((ret = _dal_apollomp_rawtoL2uc(pL2UcastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_nextValidAddrOnPort_get */


/* Function Name:
 *      dal_apollomp_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pL2McastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          reply next valid L2 multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
dal_apollomp_l2_nextValidMcastAddr_get(
    int32               *pScanIdx,
    rtk_l2_mcastAddr_t  *pL2McastData)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2McastData), RT_ERR_NULL_POINTER);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2MC;
    lut_entry.address = *pScanIdx;
    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2McastData, 0x00, sizeof(rtk_l2_mcastAddr_t));
    if ((ret = _dal_apollomp_rawtoL2mc(pL2McastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_nextValidMcastAddr_get */

/* Function Name:
 *      dal_apollomp_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pIpMcastData  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and multicast entry and
 *          reply next valid L2 ip multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
dal_apollomp_l2_nextValidIpMcastAddr_get(
    int32                   *pScanIdx,
    rtk_l2_ipMcastAddr_t    *pIpMcastData)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpMcastData), RT_ERR_NULL_POINTER);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L3MC;
    lut_entry.address = *pScanIdx;
    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pIpMcastData, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    if ((ret = _dal_apollomp_rawtoIpmc(pIpMcastData, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    *pScanIdx = lut_entry.address;

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_nextValidIpMcastAddr_get */

/* Function Name:
 *      dal_apollomp_l2_nextValidEntry_get
 * Description:
 *      Get LUT next valid entry.
 * Input:
 *      pScanIdx - Index field in the structure.
 * Output:
 *      pL2Entry - entry content
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get next valid LUT entry.
 */
int32
dal_apollomp_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2Entry), RT_ERR_NULL_POINTER);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));

    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS;
    lut_entry.address = *pScanIdx;
    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

	if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2Entry, 0x00, sizeof(rtk_l2_addr_table_t));
    if(lut_entry.l3lookup == 0)
    {
        if((lut_entry.mac.octet[0] & 0x01) == 0)
        {
            pL2Entry->entryType = RTK_LUT_L2UC;
            if ((ret = _dal_apollomp_rawtoL2uc(&(pL2Entry->entry.l2UcEntry), &lut_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            pL2Entry->entryType = RTK_LUT_L2MC;
            if ((ret = _dal_apollomp_rawtoL2mc(&(pL2Entry->entry.l2McEntry), &lut_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }
        }
    }
    else
    {
        pL2Entry->entryType = RTK_LUT_L3MC;
        if ((ret = _dal_apollomp_rawtoIpmc(&(pL2Entry->entry.ipmcEntry), &lut_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
    }

    *pScanIdx = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_nextValidEntry_get */



/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      dal_apollomp_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
dal_apollomp_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    RT_PARAM_CHK((pL2Addr->fid > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);
    RT_PARAM_CHK((pL2Addr->efid > HAL_ENHANCED_FID_MAX()), RT_ERR_L2_EFID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(pL2Addr->port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(pL2Addr->ext_port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK((pL2Addr->age > HAL_L2_ENTRY_AGING_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((pL2Addr->priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_PRIORITY);
    RT_PARAM_CHK((pL2Addr->auth >= RTK_ENABLE_END), RT_ERR_INPUT);

    osal_memset(&lut_entry, 0x00, sizeof(apollomp_lut_table_t));

    if ((ret = _dal_apollomp_l2uctoRaw(&lut_entry, pL2Addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC;
    if ((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_FAILED;

    pL2Addr->index = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_addr_add */

/* Function Name:
 *      dal_apollomp_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
dal_apollomp_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    RT_PARAM_CHK((pL2Addr->efid > HAL_ENHANCED_FID_MAX()), RT_ERR_L2_EFID);
    if(pL2Addr->flags & RTK_L2_UCAST_FLAG_IVL)
        RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    else
        RT_PARAM_CHK((pL2Addr->fid > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);

    osal_memset(&lut_entry, 0x00, sizeof(apollomp_lut_table_t));

    if ((ret = _dal_apollomp_l2uctoRaw(&lut_entry, pL2Addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC;
    lut_entry.nosalearn  = DISABLED;
    lut_entry.age        = 0;
    lut_entry.spa        = HAL_GET_MIN_PORT();
    if ((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_addr_del */


/* Function Name:
 *      dal_apollomp_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
dal_apollomp_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->flags > RTK_L2_UCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK(((pL2Addr->mac.octet[0] & 0x01) != 0), RT_ERR_MAC);
    RT_PARAM_CHK((pL2Addr->efid > HAL_ENHANCED_FID_MAX()), RT_ERR_L2_EFID);
    if(pL2Addr->flags & RTK_L2_UCAST_FLAG_IVL)
        RT_PARAM_CHK((pL2Addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    else
        RT_PARAM_CHK((pL2Addr->fid > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if ((ret = _dal_apollomp_l2uctoRaw(&lut_entry, pL2Addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_MAC;
    lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC;
    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pL2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    if ((ret = _dal_apollomp_rawtoL2uc(pL2Addr, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_addr_get */


/* Function Name:
 *      dal_apollomp_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry.
 * Input:
 *      includeStatic - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_l2_addr_delAll(uint32 includeStatic)
{
    int32   ret;
    apollomp_raw_flush_ctrl_t flushCtrl, originalCtrl;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((TRUE < includeStatic), RT_ERR_INPUT);

    /* Get Current Flush Control */
    if((ret = apollomp_raw_l2_flushCtrl_get(&originalCtrl)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    /* Set Flush Control to BOTH */
    osal_memset(&flushCtrl, 0x00, sizeof(apollomp_raw_flush_ctrl_t));
    if (ENABLED == includeStatic)
        flushCtrl.flushType = APOLLOMP_RAW_FLUSH_TYPE_BOTH;
    else
        flushCtrl.flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;

    flushCtrl.flushMode = APOLLOMP_RAW_FLUSH_MODE_PORT;
    if((ret = apollomp_raw_l2_flushCtrl_set(&flushCtrl)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    /* Start Flushing */
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = apollomp_raw_l2_flushEn_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }
    }

    /* Recover to original Flush Control */
    if((ret = apollomp_raw_l2_flushCtrl_set(&originalCtrl)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_addr_delAll */

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      dal_apollomp_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
int32
dal_apollomp_l2_mcastAddr_add(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pMcastAddr->mac.octet[0] & 0x01) != 1), RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->fid > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(pMcastAddr->portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID(pMcastAddr->ext_portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pMcastAddr->flags > RTK_L2_MCAST_FLAG_ALL), RT_ERR_INPUT);
    RT_PARAM_CHK((pMcastAddr->priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_PRIORITY);

    if( (0 == RTK_PORTMASK_GET_PORT_COUNT(pMcastAddr->ext_portmask)) && (RTK_PORTMASK_IS_PORT_SET(pMcastAddr->portmask, HAL_GET_CPU_PORT())) )
    {
        /* CPU port is set to member port mask bu no extnsion port set */
        /* Set ext port 0 at extension port mask */
        RTK_PORTMASK_PORT_SET(pMcastAddr->ext_portmask, HAL_GET_MIN_EXT_PORT());
    }

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if((ret = _dal_apollomp_l2mctoRaw(&lut_entry, pMcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
    lut_entry.nosalearn = ENABLED;
    if((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_FAILED;

    pMcastAddr->index = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_mcastAddr_add */

/* Function Name:
 *      dal_apollomp_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      None
 */
int32
dal_apollomp_l2_mcastAddr_del(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pMcastAddr->mac.octet[0] & 0x01) != 1), RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->flags > RTK_L2_MCAST_FLAG_ALL), RT_ERR_INPUT);
    if(pMcastAddr->flags & RTK_L2_MCAST_FLAG_IVL)
        RT_PARAM_CHK((pMcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    else
        RT_PARAM_CHK((pMcastAddr->fid > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if((ret = _dal_apollomp_l2mctoRaw(&lut_entry, pMcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
    lut_entry.mbr = 0x00;
    lut_entry.nosalearn = DISABLED;
    if((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_mcastAddr_del */

/* Function Name:
 *      dal_apollomp_l2_mcastAddr_get
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_l2_mcastAddr_get(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pMcastAddr->mac.octet[0] & 0x01) != 1), RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->flags > RTK_L2_MCAST_FLAG_ALL), RT_ERR_INPUT);
    if(pMcastAddr->flags & RTK_L2_MCAST_FLAG_IVL)
        RT_PARAM_CHK((pMcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    else
        RT_PARAM_CHK((pMcastAddr->fid > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if ((ret = _dal_apollomp_l2mctoRaw(&lut_entry, pMcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
    lut_entry.method = APOLLOMP_RAW_LUT_READ_METHOD_MAC;

    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pMcastAddr, 0x00, sizeof(rtk_l2_mcastAddr_t));
    if ((ret = _dal_apollomp_rawtoL2mc(pMcastAddr, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_mcastAddr_get */

/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      dal_apollomp_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
dal_apollomp_l2_illegalPortMoveAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_l2_unMatched_saCtl_get(port, pFwdAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_illegalPortMoveAction_get */

/* Function Name:
 *      dal_apollomp_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
dal_apollomp_l2_illegalPortMoveAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACTION_END <= fwdAction), RT_ERR_INPUT);

    switch (fwdAction)
    {
        case ACTION_FORWARD:
        case ACTION_DROP:
        case ACTION_TRAP2CPU:
        case ACTION_COPY2CPU:
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if ((ret = apollomp_raw_l2_unMatched_saCtl_set(port, fwdAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_illegalPortMoveAction_set */


/* Module Name    : L2           */
/* Sub-module Name: IP multicast */


/* Function Name:
 *      dal_apollomp_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DPI_AND_VID
 */
int32
dal_apollomp_l2_ipmcMode_get(rtk_l2_ipmcMode_t *pMode)
{
    int32   ret;
    apollomp_raw_l2_ipMcHashType_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_l2_ipmcHashType_get(&raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    switch(raw_mode)
    {
        case APOLLOMP_RAW_LUT_IPMCHASH_TYPE_DMACFID:
            *pMode = LOOKUP_ON_MAC_AND_VID_FID;
            break;

        case APOLLOMP_RAW_LUT_IPMCHASH_TYPE_DIPSIP:
            *pMode = LOOKUP_ON_DIP_AND_SIP;
            break;

        case APOLLOMP_RAW_LUT_IPMCHASH_TYPE_GIPVID:
            *pMode = LOOKUP_ON_DIP_AND_VID;
            break;

        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipmcMode_get */

/* Function Name:
 *      dal_apollomp_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DPI_AND_VID
 */
int32
dal_apollomp_l2_ipmcMode_set(rtk_l2_ipmcMode_t mode)
{
    int32   ret;
    apollomp_raw_l2_ipMcHashType_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((IPMC_MODE_END <= mode), RT_ERR_INPUT);

    switch(mode)
    {
        case LOOKUP_ON_MAC_AND_VID_FID:
            raw_mode = APOLLOMP_RAW_LUT_IPMCHASH_TYPE_DMACFID;
            break;

        case LOOKUP_ON_DIP_AND_SIP:
            raw_mode = APOLLOMP_RAW_LUT_IPMCHASH_TYPE_DIPSIP;
            break;

        case LOOKUP_ON_DIP_AND_VID:
            raw_mode = APOLLOMP_RAW_LUT_IPMCHASH_TYPE_GIPVID;
            break;

        default:
            return RT_ERR_CHIP_NOT_FOUND;
            break;
    }

    if ((ret = apollomp_raw_l2_ipmcHashType_set(raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipmcMode_set */

/* Function Name:
 *      dal_apollomp_l2_ipmcGroupLookupMissHash_get
 * Description:
 *      Get Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      None
 * Output:
 *      pIpmcHash - pointer of Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_apollomp_l2_ipmcGroupLookupMissHash_get(rtk_l2_ipmcHashOp_t *pIpmcHash)
{
    int32   ret;
    apollomp_raw_l2_ipMcLookupOp_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcHash), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_l2_ipmcLookupOp_get(&raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    switch(raw_mode)
    {
        case APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_DIPSIP:
            *pIpmcHash = HASH_DIP_AND_SIP;
            break;
        case APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_DIP:
            *pIpmcHash = HASH_DIP_ONLY;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipmcGroupLookupMissHash_get */

/* Function Name:
 *      dal_apollomp_l2_ipmcGroupLookupMissHash_set
 * Description:
 *      Set Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      ipmcHash - Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_apollomp_l2_ipmcGroupLookupMissHash_set(rtk_l2_ipmcHashOp_t ipmcHash)
{
    int32   ret;
    apollomp_raw_l2_ipMcLookupOp_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((HASH_END <= ipmcHash), RT_ERR_INPUT);

    switch(ipmcHash)
    {
        case HASH_DIP_AND_SIP:
            raw_mode = APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_DIPSIP;
            break;
        case HASH_DIP_ONLY:
            raw_mode = APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_DIP;
            break;
        default:
            return RT_ERR_CHIP_NOT_FOUND;;
            break;
    }

    if ((ret = apollomp_raw_l2_ipmcLookupOp_set(raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipmcGroupLookupMissHash_set */

/* Function Name:
 *      dal_apollomp_l2_ipmcGroup_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip         - Group IP
 *      pPortmask   - Group member port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
int32
dal_apollomp_l2_ipmcGroup_add(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
    int32   ret;
    uint32  idx;
    rtk_ip_addr_t raw_dip;
    uint32 raw_portmask;
    uint32 empty_idx = 0xFFFFFFFF;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(( (gip > 0xEFFFFFFF) || (gip <= 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pPortmask)), RT_ERR_PORT_MASK);

    for(idx = 0; idx <= APOLLOMP_IPMC_TABLE_IDX_MAX; idx++)
    {
        if ((ret = apollomp_raw_l2_igmp_Mc_table_get(idx, &raw_dip, &raw_portmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        if (raw_dip == gip)
        {
            if ((ret = apollomp_raw_l2_igmp_Mc_table_set(idx, (rtk_ip_addr_t)gip, RTK_PORTMASK_TO_UINT_PORTMASK(pPortmask))) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                return ret;
            }

            return RT_ERR_OK;
        }
        else if( (0xE0000000 == raw_dip) && (0xFFFFFFFF == empty_idx) )
             empty_idx = idx;
    }

    if(empty_idx != 0xFFFFFFFF)
    {
        if ((ret = apollomp_raw_l2_igmp_Mc_table_set(empty_idx, (rtk_ip_addr_t)gip, RTK_PORTMASK_TO_UINT_PORTMASK(pPortmask))) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_FULL;
} /* end of dal_apollomp_l2_ipmcGroup_add */

/* Function Name:
 *      dal_apollomp_l2_ipmcGroup_del
 * Description:
 *      Delete an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
dal_apollomp_l2_ipmcGroup_del(ipaddr_t gip)
{
    int32   ret;
    uint32  idx;
    rtk_ip_addr_t raw_dip;
    uint32 raw_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(( (gip > 0xEFFFFFFF) || (gip <= 0xE0000000) ), RT_ERR_INPUT);

    for(idx = 0; idx <= APOLLOMP_IPMC_TABLE_IDX_MAX; idx++)
    {
        if ((ret = apollomp_raw_l2_igmp_Mc_table_get(idx, &raw_dip, &raw_portmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        if(raw_dip != 0xE0000000)
        {
            if (gip == raw_dip)
            {
                if ((ret = apollomp_raw_l2_igmp_Mc_table_set(idx, 0xE0000000, 0)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_ENTRY_NOTFOUND;
} /* end of dal_apollomp_l2_ipmcGroup_del */

/* Function Name:
 *      dal_apollomp_l2_ipmcGroup_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      pPortmask   - Group member port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
dal_apollomp_l2_ipmcGroup_get(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
    int32   ret;
    uint32  idx;
    rtk_ip_addr_t raw_dip;
    uint32 raw_portmask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(( (gip > 0xEFFFFFFF) || (gip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    for(idx = 0; idx <= APOLLOMP_IPMC_TABLE_IDX_MAX; idx++)
    {
        if ((ret = apollomp_raw_l2_igmp_Mc_table_get(idx, &raw_dip, &raw_portmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
            return ret;
        }

        if (raw_dip == gip)
        {
            RTK_PORTMASK_FROM_UINT_PORTMASK(pPortmask, &raw_portmask);
            return RT_ERR_OK;
        }
    }

    return RT_ERR_ENTRY_NOTFOUND;
} /* end of dal_apollomp_l2_ipmcGroup_get */

/* Function Name:
 *      dal_apollomp_l2_portIpmcAction_get
 * Description:
 *      Get the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 * Output:
 *      pAction     - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l2_portIpmcAction_get(rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2), "port=%d",port);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = apollomp_raw_l2_ipmcAction_get(port, pAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l2_portIpmcAction_get */

/* Function Name:
 *      dal_apollomp_l2_portIpmcAction_set
 * Description:
 *      Set the Action of IPMC packet per ingress port.
 * Input:
 *      port        - Ingress port number
 *      action      - IPMC packet action (ACTION_FORWARD or ACTION_DROP)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_l2_portIpmcAction_set(rtk_port_t port, rtk_action_t action)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2), "port=%d,action=%d",port, action);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(( (ACTION_FORWARD != action) && (ACTION_DROP != action) ), RT_ERR_INPUT);

    /* function body */
    if((ret = apollomp_raw_l2_ipmcAction_set(port, action)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_l2_portIpmcAction_set */

/* Function Name:
 *      dal_apollomp_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_IPV4_ADDRESS  - Invalid IPv4 address
 *      RT_ERR_VLAN_VID      - invalid vlan id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
dal_apollomp_l2_ipMcastAddr_add(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;
    rtk_l2_ipmcMode_t   mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(( (pIpmcastAddr->dip > 0xEFFFFFFF) || (pIpmcastAddr->dip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(pIpmcastAddr->portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID(pIpmcastAddr->ext_portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pIpmcastAddr->flags > RTK_L2_IPMCAST_FLAG_ALL), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pIpmcastAddr->priority > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_PRIORITY);

    if( (0 == RTK_PORTMASK_GET_PORT_COUNT(pIpmcastAddr->ext_portmask)) && (RTK_PORTMASK_IS_PORT_SET(pIpmcastAddr->portmask, HAL_GET_CPU_PORT())) )
    {
        /* CPU port is set to member port mask bu no extnsion port set */
        /* Set ext port 0 at extension port mask */
        RTK_PORTMASK_PORT_SET(pIpmcastAddr->ext_portmask, HAL_GET_MIN_EXT_PORT());
    }

    if((ret = dal_apollomp_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(LOOKUP_ON_DIP_AND_VID == mode)
        RT_PARAM_CHK((pIpmcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if ((ret = _dal_apollomp_IpmctoRaw(&lut_entry, pIpmcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(lut_entry.gip_only)
        lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
    else
        lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    lut_entry.l3lookup = ENABLED;

    if ((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_FAILED;

    pIpmcastAddr->index = lut_entry.address;
    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipMcastAddr_add */

/* Function Name:
 *      dal_apollomp_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      pIpmcastAddr  - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      (1) In vlan unaware mode (SVL), the vid will be ignore, suggest to
 *          input vid=0 in vlan unaware mode.
 *      (2) In vlan aware mode (IVL), the vid will be care.
 */
int32
dal_apollomp_l2_ipMcastAddr_del(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;
    rtk_l2_ipmcMode_t   mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(( (pIpmcastAddr->dip > 0xEFFFFFFF) || (pIpmcastAddr->dip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK((pIpmcastAddr->flags > RTK_L2_IPMCAST_FLAG_ALL), RT_ERR_PORT_MASK);

    if((ret = dal_apollomp_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(LOOKUP_ON_DIP_AND_VID == mode)
        RT_PARAM_CHK((pIpmcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if ((ret = _dal_apollomp_IpmctoRaw(&lut_entry, pIpmcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(lut_entry.gip_only)
        lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
    else
        lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    lut_entry.nosalearn = DISABLED;
    lut_entry.mbr=0;
    lut_entry.l3lookup = ENABLED;
    lut_entry.mbr = 0x0;
    lut_entry.ext_mbr = 0x0;

    if ((ret = apollomp_raw_l2_lookUpTb_set(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipMcastAddr_del */

/* Function Name:
 *      dal_apollomp_l2_ipMcastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
dal_apollomp_l2_ipMcastAddr_get(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    apollomp_lut_table_t lut_entry;
    rtk_l2_ipmcMode_t   mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcastAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(( (pIpmcastAddr->dip > 0xEFFFFFFF) || (pIpmcastAddr->dip < 0xE0000000) ), RT_ERR_INPUT);
    RT_PARAM_CHK((pIpmcastAddr->flags > RTK_L2_IPMCAST_FLAG_ALL), RT_ERR_PORT_MASK);

    if((ret = dal_apollomp_l2_ipmcMode_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(LOOKUP_ON_DIP_AND_VID == mode)
        RT_PARAM_CHK((pIpmcastAddr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&lut_entry, 0x0, sizeof(apollomp_lut_table_t));
    if ((ret = _dal_apollomp_IpmctoRaw(&lut_entry, pIpmcastAddr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(lut_entry.gip_only)
        lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
    else
        lut_entry.table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    lut_entry.l3lookup = ENABLED;

    if ((ret = apollomp_raw_l2_lookUpTb_get(&lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    if(0 == lut_entry.lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memset(pIpmcastAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    if ((ret = _dal_apollomp_rawtoIpmc(pIpmcastAddr, &lut_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2 | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_ipMcastAddr_get */


/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */


/* Function Name:
 *      dal_apollomp_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
dal_apollomp_l2_srcPortEgrFilterMask_get(rtk_portmask_t *pFilter_portmask)
{
    int32   ret;
    rtk_port_t port;
    uint32 permit;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter_portmask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET((*pFilter_portmask));

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = reg_array_field_read(APOLLOMP_L2_SRC_PORT_PERMITr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &permit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }

        if(DISABLED == permit)
            RTK_PORTMASK_PORT_SET((*pFilter_portmask), port);
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_srcPortEgrFilterMask_get */

/* Function Name:
 *      dal_apollomp_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
dal_apollomp_l2_srcPortEgrFilterMask_set(rtk_portmask_t *pFilter_portmask)
{
    int32   ret;
    rtk_port_t port;
    uint32 permit;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFilter_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pFilter_portmask), RT_ERR_INPUT);

    HAL_SCAN_ALL_PORT(port)
    {
        if(RTK_PORTMASK_IS_PORT_SET((*pFilter_portmask), port))
            permit = DISABLED;
        else
            permit = ENABLED;

        if ((ret = reg_array_field_write(APOLLOMP_L2_SRC_PORT_PERMITr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &permit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_srcPortEgrFilterMask_set */

/* Function Name:
 *      dal_apollomp_l2_extPortEgrFilterMask_get
 * Description:
 *      Get extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
dal_apollomp_l2_extPortEgrFilterMask_get(rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    rtk_port_t port;
    uint32 permit;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET((*pExt_portmask));

    HAL_SCAN_ALL_EXT_PORT(port)
    {
        if ((ret = reg_array_field_read(APOLLOMP_L2_SRC_EXT_PERMITr, REG_ARRAY_INDEX_NONE, port, APOLLOMP_ENf, &permit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }

        if(DISABLED == permit)
            RTK_PORTMASK_PORT_SET((*pExt_portmask), port);
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_extPortEgrFilterMask_get */

/* Function Name:
 *      dal_apollomp_l2_extPortEgrFilterMask_set
 * Description:
 *      Set extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
dal_apollomp_l2_extPortEgrFilterMask_set(rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    rtk_port_t port;
    uint32 permit;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(l2_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExt_portmask)), RT_ERR_PORT_MASK);

    HAL_SCAN_ALL_EXT_PORT(port)
    {
        if(RTK_PORTMASK_IS_PORT_SET((*pExt_portmask), port))
            permit = DISABLED;
        else
            permit = ENABLED;

        if ((ret = reg_array_field_write(APOLLOMP_L2_SRC_EXT_PERMITr, REG_ARRAY_INDEX_NONE, (uint32)port, APOLLOMP_ENf, &permit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l2_extPortEgrFilterMask_set */

/* Function Name:
 *      dal_apollomp_l2_hashValue_get
 * Description:
 *      Calculate the hash value for specific hash type
 * Input:
 *      type        - hash type
 *      hashData    - input data to be hashed
 * Output:
 *      pHashValue  - hash result of specified type with data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
dal_apollomp_l2_hashValue_get(rtk_l2_hashType_t type, rtk_l2_hashData_t hashData, uint32 *pHashValue)
{
	int32	ret;

	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L2),"%s",__FUNCTION__);

	/* Software only API, don't check Init status */

	/* parameter check */
	RT_PARAM_CHK((type >= LUT_HASHTYPE_END), RT_ERR_INPUT);
	RT_PARAM_CHK((NULL == pHashValue), RT_ERR_NULL_POINTER);

	switch(type)
	{
        case LUT_HASHTYPE_L2_UCSVL:
            RT_PARAM_CHK(!APOLLOMP_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_MAC | LUT_HASHFLAG_FID | LUT_HASHFLAG_EFID), RT_ERR_INPUT);
            if((ret = _dal_apollomp_l2UcSvlHash(hashData.mac, hashData.fid, hashData.efid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L2_MCSVL:
            RT_PARAM_CHK(!APOLLOMP_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_MAC | LUT_HASHFLAG_FID), RT_ERR_INPUT);
            if((ret = _dal_apollomp_l2McSvlHash(hashData.mac, hashData.fid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L2_UCIVL:
            RT_PARAM_CHK(!APOLLOMP_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_MAC | LUT_HASHFLAG_VID | LUT_HASHFLAG_EFID), RT_ERR_INPUT);
            if((ret = _dal_apollomp_l2UcIvlHash(hashData.mac, hashData.vid, hashData.efid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L2_MCIVL:
            RT_PARAM_CHK(!APOLLOMP_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_MAC | LUT_HASHFLAG_VID), RT_ERR_INPUT);
            if((ret = _dal_apollomp_l2McIvlHash(hashData.mac, hashData.vid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L3_SIPGIP:
            RT_PARAM_CHK(!APOLLOMP_HASH_FLAG_CHECK(hashData.dataFlags, 
                LUT_HASHFLAG_SIP | LUT_HASHFLAG_GIP), RT_ERR_INPUT);
            if((ret = _dal_apollomp_l3SipGipHash(hashData.sip, hashData.gip, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case LUT_HASHTYPE_L3_GIPVID:
            RT_PARAM_CHK(!APOLLOMP_HASH_FLAG_CHECK(hashData.dataFlags,
                LUT_HASHFLAG_GIP | LUT_HASHFLAG_VID), RT_ERR_INPUT);
            if((ret = _dal_apollomp_l3GipVidHash(hashData.gip, hashData.vid, pHashValue)) != RT_ERR_OK)
            {
        		RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
		default:
			return RT_ERR_CHIP_NOT_SUPPORTED;
	}

	return RT_ERR_OK;
} /* end of dal_apollomp_l2_hashValue_get */

