/*
* Copyright (C) 2010 Realtek Semiconductor Corp.
* All Rights Reserved.
*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
*
* $Revision: 10455 $
* $Date: 2010-06-25 18:27:53 +0800 (Fri, 25 Jun 2010) $
*
* Purpose : Export the public APIs in lower layer module
*
* Feature : Export the public APIs in lower layer module
*
*/

/*
* Include Files
*/
#include <igmp/inc/proto_igmp.h>
#include <igmp/inc/proto_igmp_tx.h>

/*
* Symbol Definition
*/
#define IGMP_QUERY_PDU_LEN      46 /* MAC(12 + 4+2) + IP(20) + IGMP(8) */
#define IGMP_QUERY_V3_PDU_LEN   50 /* MAC(12 +4+2) + IP(20) + IGMP(8) */

#define MLD_QUERY_V1_PDU_LEN    90
#define MLD_QUERY_V2_PDU_LEN    94

static uint8                    gNetworkEnabled = FALSE;
static igmp_querier_db_t        querier_db[MULTICAST_TYPE_END];
static igmp_querier_entry_t     *p_tmp_querier_array;

static void mcast_code_convert(uint32 code, uint8 *pResult)
{
    uint8 tmp;

    if (!pResult)
    {
        return;
    }

    if (code < 128)
    {
        *pResult = code;
    }
    else
    {
        for (tmp = 15; tmp > 7; tmp--)
        {
            if ((code) & (0x1 << tmp))
                break;
        }
        *pResult = (((code) >> (tmp - 4)) & 0xf) /*bit0-bit3*/
            |(((tmp - 4 - 3) << 4) & 0x70) /*bit4-bit6*/
            | 0x80;    /*bit 7*/
    }
    return;
}

int32 mcast_querier_array_search(uint16 search, multicast_ipType_t ipType, uint16 *idx, igmp_querier_entry_t **entry)
{
    int                     low = 0, mid, high;
    igmp_querier_entry_t    *sortedArray = NULL;

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((idx == NULL || entry == NULL), SYS_ERR_FAILED);

    high = querier_db[ipType].entry_num - 1;
    sortedArray = querier_db[ipType].p_querier_entry;

    while (low <= high)
    {
        mid = (low + high) / 2;

        if (sortedArray[mid].vid == 0)   /* this case occurs when sorted array is empty */
        {
            *entry = NULL;
            *idx = mid;
            return SYS_ERR_OK;
        }

        if (sortedArray[mid].vid == search)
        {
            *entry = &sortedArray[mid];
            *idx = mid;
            return SYS_ERR_OK;
        }
        else if (sortedArray[mid].vid > search)
        {
            high = mid - 1;
        }
        else if (sortedArray[mid].vid < search)
        {
            low = mid + 1;
        }
    }

    *entry  = NULL;
    *idx    = low;
    return SYS_ERR_OK;
}

int32 mcast_querier_array_ins(uint16 sortedIdx, multicast_ipType_t ipType, igmp_querier_entry_t *entry)
{
    SYS_ERR_CHK((entry == NULL), SYS_ERR_FAILED);

    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((sortedIdx > querier_db[ipType].entry_num), SYS_ERR_FAILED);

    SYS_ERR_CHK((querier_db[ipType].entry_num >= MAX_ROUTER_VLAN), SYS_ERR_FAILED);

    osal_memcpy(p_tmp_querier_array, querier_db[ipType].p_querier_entry + sortedIdx, sizeof(igmp_querier_entry_t) *
                (querier_db[ipType].entry_num - sortedIdx));

    osal_memcpy(querier_db[ipType].p_querier_entry + sortedIdx, entry, sizeof(igmp_querier_entry_t));

    osal_memcpy(querier_db[ipType].p_querier_entry + sortedIdx + 1, p_tmp_querier_array, sizeof(igmp_querier_entry_t) *
                (querier_db[ipType].entry_num - sortedIdx));

    querier_db[ipType].entry_num++;

    return SYS_ERR_OK;
}

int32 mcast_querier_array_remove(uint16 sortedIdx, multicast_ipType_t ipType)
{
    SYS_PARAM_CHK((ipType < MULTICAST_TYPE_IPV4 || ipType > MULTICAST_TYPE_END), SYS_ERR_MCAST_IPTYPE);

    SYS_ERR_CHK((sortedIdx > querier_db[ipType].entry_num), SYS_ERR_FAILED);

    osal_memcpy(p_tmp_querier_array, querier_db[ipType].p_querier_entry + sortedIdx + 1, sizeof(igmp_querier_entry_t) *
                (querier_db[ipType].entry_num - sortedIdx));

    osal_memcpy(querier_db[ipType].p_querier_entry + sortedIdx, p_tmp_querier_array, sizeof(igmp_querier_entry_t) *
                (querier_db[ipType].entry_num - sortedIdx));

    querier_db[ipType].entry_num--;

    osal_memset(querier_db[ipType].p_querier_entry + querier_db[ipType].entry_num, 0, sizeof(igmp_querier_entry_t));

    return SYS_ERR_OK;
}

int32 mcast_querier_db_init(void)
{
    multicast_ipType_t ipType;

    for (ipType = MULTICAST_TYPE_IPV4; ipType < MULTICAST_TYPE_END; ipType++)
    {
        osal_memset(&querier_db[ipType], 0, sizeof(querier_db[ipType]));

        querier_db[ipType].p_querier_entry = (igmp_querier_entry_t *)osal_alloc(sizeof(igmp_querier_entry_t) * MAX_ROUTER_VLAN);

        SYS_ERR_CHK((!querier_db[ipType].p_querier_entry), SYS_ERR_FAILED);

        osal_memset(querier_db[ipType].p_querier_entry, 0, sizeof(igmp_querier_entry_t) * MAX_ROUTER_VLAN);
    }

    SYS_ERR_CHK((!(p_tmp_querier_array = (igmp_querier_entry_t *)osal_alloc(sizeof(igmp_querier_entry_t) * (MAX_ROUTER_VLAN - 1)))), SYS_ERR_FAILED);

    osal_memset(p_tmp_querier_array, 0, sizeof(igmp_querier_entry_t) * (MAX_ROUTER_VLAN - 1));

    return SYS_ERR_OK;
}

int32 mcast_querier_db_add(uint16 vid, multicast_ipType_t ipType)
{
    uint16                  sortedIdx;
    igmp_querier_entry_t    *entry = NULL, newEntry;

    mcast_querier_array_search(vid, ipType, &sortedIdx, &entry);

    if (!entry)
    {
        osal_memset(&newEntry, 0, sizeof(igmp_querier_entry_t));
        newEntry.vid    = vid;
        newEntry.ipType = ipType;
        return mcast_querier_array_ins(sortedIdx, ipType, &newEntry);
    }

    return SYS_ERR_OK;
}

int32 mcast_querier_db_get(uint16 vid,  multicast_ipType_t ipType, igmp_querier_entry_t **ppEntry)
{
    uint16                  sortedIdx;

    SYS_PARAM_CHK(NULL == ppEntry, SYS_ERR_NULL_POINTER);

    mcast_querier_array_search(vid, ipType, &sortedIdx, ppEntry);

    return SYS_ERR_OK;
}

int32 mcast_querier_db_del(uint16 vid, multicast_ipType_t ipType)
{
    uint16                  sortedIdx;
    igmp_querier_entry_t    *entry;

    mcast_querier_array_search(vid, ipType, &sortedIdx, &entry);   /* Only compare dip */

    if (entry)    /* further, compare dip + sip*/
    {
        return mcast_querier_array_remove(sortedIdx,ipType);
    }

    return SYS_ERR_FAILED;
}

void mcast_igmp_build_general_query(sys_nic_pkt_t *pkt, sys_vid_t vid, uint16 pktLen, uint8 version, uint32 queryEnable,
                                    uint32 gdaddr, uint8 query_type)
{
    uint8                   igmpv3hd[IGMPV3_HEAD_LEN];  /*s_rqv 1byte, qqic 1byte, src_num 16*/
    uint8                   qqic, code;
    uint16                  tot_len, posite, igmpHdrCsm, cvid;
    uint32                  saddr;
    iphdr_t                 *iph;
    igmp_hdr_t              *igmph;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
    uint32 daddr            = htonl(0xE0000001L);
    uint32 gda              = htonl(0x00000000L);
    uint16 ctag             = htons(0x8100);
    uint16 type             = htons(0x0800);
    uint16 id               = htons(0x0000);
    uint16 frag_off         = htons(0x0000);
    uint16 csum             = htons(0x00);
    uint8 lv                = htons(0x45);
    uint8 tos               = htons(0x00);
    uint8 ttl               = htons(0x01);
    uint8 protocol          = htons(2);     /* 2: IPPROTO_IGMP */
    uint8 itype             = htons(IGMP_HOST_MEMBERSHIP_QUERY);
    uint8 query_dst_mac[]   = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x01};
    uint8 options[]         = {0x94, 0x04, 0x00, 0x00};

    if (IGMP_GENERAL_QUERY == query_type)
    {
        daddr =  htonl(0xE0000001L);
        gda    = htonl(0x00000000L);
    }
    else if (IGMP_SPECIFIC_GROUP_QUERY == query_type)
    {
        daddr = htonl(gdaddr);
        gda    = htonl(gdaddr);
        query_dst_mac[3] = (gdaddr >> 16) & 0x7f;
        query_dst_mac[4] = (gdaddr >> 8) & 0xff;
        query_dst_mac[5] =  gdaddr & 0xff;
    }

    mcast_vlan_db_get(vid,ipType, &pMcastVlan);

    if (!pMcastVlan)
    {
        return;
    }

    cvid = (uint16)htons(vid);

    if (IGMP_QUERY_V3 == version)
    {
        mcast_code_convert(pMcastVlan->responseTime * 10, &code);

        code = htons(code);
        //should equel 1, then the VLC test can pass.
        //code = htons(1);
        tot_len =  htons ((sizeof(iphdr_t)) + sizeof(igmp_hdr_t));
    }
    else
    {
        code = pMcastVlan->responseTime * 10;

        tot_len =  htons ((sizeof(iphdr_t)) + sizeof(igmp_hdr_t) - sizeof(iph->v3) - sizeof(igmph->v3));
    }

    saddr = (ENABLED == queryEnable ? htonl(igmpCtrl.sys_ip) : htonl(0x00));

    /* ethernet II header */
    IGMP_FILL_QUERY(pkt, &query_dst_mac, 0, 6);
    IGMP_FILL_QUERY(pkt, &igmpCtrl.sys_mac, 6, 6);
    IGMP_FILL_QUERY(pkt, &ctag, 12, 2);
    IGMP_FILL_QUERY(pkt, &cvid, 14, 2);
    IGMP_FILL_QUERY(pkt, &type, 16, 2);

    /* ip header */
    iph = (iphdr_t *)(&pkt->data[18]);

    if (IGMP_QUERY_V2 == version)
    {
        IGMP_FILL_QUERY(pkt, &lv, 18, 1);
    }
    else
    {
        lv = htons(0x46);
        IGMP_FILL_QUERY(pkt, &lv, 18, 1);
    }

    IGMP_FILL_QUERY(pkt, &tos, 19, 1);
    IGMP_FILL_QUERY(pkt, &tot_len, 20, 2);
    IGMP_FILL_QUERY(pkt, &id, 22, 2);

    if (IGMP_QUERY_V2 == version)
    {
        IGMP_FILL_QUERY(pkt, &frag_off, 24, 2);
    }
    else
    {
        frag_off = htons(0x4000);
        IGMP_FILL_QUERY(pkt, &frag_off, 24, 2);
    }

    IGMP_FILL_QUERY(pkt, &ttl, 26, 1);
    IGMP_FILL_QUERY(pkt, &protocol, 27, 1);
    IGMP_FILL_QUERY(pkt, &csum, 28, 2);
    IGMP_FILL_QUERY(pkt, &saddr, 30, 4);
    IGMP_FILL_QUERY(pkt, &daddr, 34, 4);

    posite = 38;
    if (IGMP_QUERY_V2 == version)
    {
        /* Minus 4 byte options */
        igmp_util_checksum_get((uint16 *)iph, sizeof(iphdr_t) - sizeof(iph->v3), &csum);
    }
    else
    {
        IGMP_FILL_QUERY(pkt, options, posite, 4);
        posite += 4;
        igmp_util_checksum_get((uint16 *)iph, sizeof(iphdr_t), &csum);
    }

    csum = htons(csum);
    IGMP_FILL_QUERY(pkt, &csum, 28, 2);

    /* igmp header */
    csum = htons(0x0000);
    igmph = (igmp_hdr_t *)(&pkt->data[posite]);

    IGMP_FILL_QUERY(pkt, &itype, posite, 1);
    posite += 1;
    IGMP_FILL_QUERY(pkt, &code, posite, 1);
    posite += 1;
    igmpHdrCsm = posite;
    IGMP_FILL_QUERY(pkt, &csum, posite, 2);
    posite += 2;
    IGMP_FILL_QUERY(pkt, &gda, posite, 4);
    posite += 4;

    if (IGMP_QUERY_V3 == version)
    {
        if (IGMP_GENERAL_QUERY == query_type)
        {
            mcast_code_convert(pMcastVlan->queryIntv, &qqic);
            igmpv3hd[0] = htons(pMcastVlan->robustnessVar);
        }
        else
        {
            mcast_code_convert(pMcastVlan->grpSpecificQueryIntv, &qqic);
            igmpv3hd[0] = htons(pMcastVlan->lastMmbrQueryCnt);
        }

        igmpv3hd[1] = htons(qqic);
        igmpv3hd[2] = htons(0x0);
        igmpv3hd[3] = htons(0x0);

        IGMP_FILL_QUERY(pkt, igmpv3hd, posite, 4);
    }

    /* calculate cksum */
    tot_len = (IGMP_QUERY_V3 == version ? sizeof(igmp_hdr_t) : (sizeof(igmp_hdr_t) - sizeof(igmph->v3)));

    igmp_util_checksum_get((uint16 *)igmph, tot_len, &csum);
    csum = htons(csum);

    IGMP_FILL_QUERY(pkt, &csum, igmpHdrCsm, 2);

    pkt->as_txtag = TRUE;
    pkt->length = pktLen;
}

void mcast_igmp_build_gs_query(sys_nic_pkt_t * pkt, sys_vid_t vid, uint16 pktLen, uint32 gdaddr, uint16 version)
{
    uint8                   qqic, code;
    uint8                   igmpv3hd[IGMPV3_HEAD_LEN];  /*s_rqv 1byte, qqic 1byte, src_num 16*/
    uint16                  tot_len, cvid;
    uint32                  saddr;
    iphdr_t                 *iph;
    igmp_hdr_t              *igmph;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      *pMcastVlan = NULL;
    uint32 daddr            = htonl(gdaddr);
    uint32 gda              = htonl(gdaddr);
    uint16 ctag             = htons(0x8100);
    uint16 type             = htons(0x0800);
    uint16 id               = htons(0x0000);
    uint16 frag_off         = htons(0x0000);
    uint16 csum             = htons(0x0000);
    uint8 lv                = htons(0x46);
    uint8 tos               = htons(0x00);
    uint8 ttl               = htons(0x01);
    uint8 protocol          = htons(2);     /* 2: IPPROTO_IGMP */
    uint8 query_dst_mac[]   = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x01};
    uint8 options[]         = {0x94, 0x04, 0x00, 0x00};
    uint8 itype             = htons(IGMP_HOST_MEMBERSHIP_QUERY);

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);

    if (!pMcastVlan)
    {
        return;
    }

    cvid = (uint16)htons(vid);
    code = (uint8)htons(pMcastVlan->grpSpecificQueryIntv * 10);

    tot_len = (uint16)htons(pktLen-18);  /*18 is the l2 heaer*/

    saddr = htonl(igmpCtrl.sys_ip);

    query_dst_mac[3] = (gdaddr >> 16) & 0x7f;
    query_dst_mac[4] = (gdaddr >> 8) & 0xff;
    query_dst_mac[5] =  gdaddr & 0xff;

    /* ethernet II header */
    IGMP_FILL_QUERY(pkt, &query_dst_mac, 0, 6);
    IGMP_FILL_QUERY(pkt, &igmpCtrl.sys_mac, 6, 6);
    IGMP_FILL_QUERY(pkt, &ctag, 12, 2);
    IGMP_FILL_QUERY(pkt, &cvid, 14, 2);
    IGMP_FILL_QUERY(pkt, &type, 16, 2);

    /* ip header */
    iph = (iphdr_t *)(&pkt->data[18]);
    IGMP_FILL_QUERY(pkt, &lv, 18, 1);
    IGMP_FILL_QUERY(pkt, &tos, 19, 1);
    IGMP_FILL_QUERY(pkt, &tot_len, 20, 2);
    IGMP_FILL_QUERY(pkt, &id, 22, 2);
    IGMP_FILL_QUERY(pkt, &frag_off, 24, 2);
    IGMP_FILL_QUERY(pkt, &ttl, 26, 1);
    IGMP_FILL_QUERY(pkt, &protocol, 27, 1);
    IGMP_FILL_QUERY(pkt, &csum, 28, 2);
    IGMP_FILL_QUERY(pkt, &saddr, 30, 4);
    IGMP_FILL_QUERY(pkt, &daddr, 34, 4);
    IGMP_FILL_QUERY(pkt, &options, 38, 4);

    /* calculate ip header cksum */
    igmp_util_checksum_get((uint16 *)iph, sizeof(iphdr_t), &csum);

    csum = htons(csum);
    IGMP_FILL_QUERY(pkt, &csum, 28, 2);

    /* igmp header */
    igmph = (igmp_hdr_t *)(&pkt->data[42]);
    csum = 0;

    IGMP_FILL_QUERY(pkt, &itype, 42, 1);
    IGMP_FILL_QUERY(pkt, &code, 43, 1);
    IGMP_FILL_QUERY(pkt, &csum, 44, 2);
    IGMP_FILL_QUERY(pkt, &gda, 46, 4);

    if (IGMP_QUERY_V3 == version)
    {
        mcast_code_convert(pMcastVlan->grpSpecificQueryIntv, &qqic);
        igmpv3hd[0] = htons(pMcastVlan->lastMmbrQueryCnt);
        igmpv3hd[1] = htons(qqic);
        igmpv3hd[2] = htons(0x0);
        igmpv3hd[3] = htons(0x0);

        IGMP_FILL_QUERY(pkt, igmpv3hd, 50, 4);

        /* calculate cksum */
        igmp_util_checksum_get((uint16 *)igmph, sizeof(igmp_hdr_t), &csum);
    }
    else
    {
        igmp_util_checksum_get((uint16 *)igmph, sizeof(igmp_hdr_t) - sizeof(igmph->v3), &csum);
    }

    csum = htons(csum);

    IGMP_FILL_QUERY(pkt, &csum, 44, 2);
    pkt->as_txtag = TRUE;
    pkt->length = pktLen;
}

void mcast_igmp_build_gss_query(sys_nic_pkt_t *pkt, sys_vid_t vid, uint32 gdaddr, uint32 *pSip, uint16 numSrc, uint16 pktlen)
{
    uint8               dst_mac[6], code; /*specific group address  hosts */
    uint16              i, cvid;
    uint32              saddr, sip, tmp = 0;
    iphdr_t             *iph;
    igmp_hdr_t          *igmph;
    multicast_ipType_t  ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t  *pMcastVlan = NULL;
    uint32 daddr        = htonl(gdaddr);
    uint32 gda          = htonl(gdaddr);
    uint16 ctag         = htons(0x8100);
    uint16 type         = htons(0x0800);
    uint16 tot_len      = htons (pktlen - 14);
    uint16 id           = htons(0x0000);
    uint16 frag_off     = htons(0x0000);
    uint16 csum         = htons(0x0000);
    uint16 srcNum       = htons(numSrc);
    uint8 lv            = htons(0x45);
    uint8 tos           = htons(0x00);
    uint8 ttl           = htons(0x01);
    uint8 protocol      = htons(2);     /* 2: IPPROTO_IGMP */
    uint8 qqic          = htons(DFLT_IGMP_QUERY_INTV);
    uint8 itype         = htons(IGMP_HOST_MEMBERSHIP_QUERY);
    uint8 rsv           = htons(0);

    osal_memset(dst_mac, 0, sizeof(dst_mac));

    dst_mac[0] = 0x01;
    dst_mac[1] = 0x00;
    dst_mac[2] = 0x5e;
    dst_mac[3] = (gdaddr >> 16 & 0x07f);
    dst_mac[4] = (gdaddr >> 8 & 0x0ff);
    dst_mac[5] = (gdaddr & 0x0ff);

    saddr = htonl(igmpCtrl.sys_ip);

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);

    if (!pMcastVlan)
    {
        return;
    }

    cvid = (uint16)htons(vid);

    if (pMcastVlan->grpSpecificQueryIntv * 10 < 128)
    {
        code = pMcastVlan->grpSpecificQueryIntv * 10;
    }
    else
    {
        for (tmp = 15; tmp > 7; tmp--)
        {
            if ((pMcastVlan->grpSpecificQueryIntv * 10) & (0x1 << tmp))
                break;
        }
        code = (((pMcastVlan->grpSpecificQueryIntv * 10) >> (tmp - 4)) & 0xf) /*bit0-bit3*/
            |(((tmp - 4 - 3) << 4) & 0x70) /*bit4-bit6*/
            | 0x80;    /*bit 7*/
    }

    code = htons(code);

    /* ethernet II header */
    IGMP_FILL_QUERY(pkt, &dst_mac, 0, 6);
    IGMP_FILL_QUERY(pkt, &igmpCtrl.sys_mac, 6, 6);
    IGMP_FILL_QUERY(pkt, &ctag, 12, 2);
    IGMP_FILL_QUERY(pkt, &cvid, 14, 2);
    IGMP_FILL_QUERY(pkt, &type, 16, 2);

    /* ip header */
    iph = (iphdr_t *)(&pkt->data[18]);
    IGMP_FILL_QUERY(pkt, &lv, 18, 1);
    IGMP_FILL_QUERY(pkt, &tos, 19, 1);
    IGMP_FILL_QUERY(pkt, &tot_len, 20, 2);
    IGMP_FILL_QUERY(pkt, &id, 22, 2);
    IGMP_FILL_QUERY(pkt, &frag_off, 24, 2);
    IGMP_FILL_QUERY(pkt, &ttl, 26, 1);
    IGMP_FILL_QUERY(pkt, &protocol, 27, 1);
    IGMP_FILL_QUERY(pkt, &csum, 28, 2);
    IGMP_FILL_QUERY(pkt, &saddr, 30, 4);
    IGMP_FILL_QUERY(pkt, &daddr, 34, 4);

    /* calculate ip header cksum */
    igmp_util_checksum_get((uint16*)iph, sizeof(iphdr_t), &csum);
    csum = htons(csum);
    IGMP_FILL_QUERY(pkt, &csum, 28, 2);

    /* igmp header */
    igmph = (igmp_hdr_t *)(&pkt->data[38]);
    csum = 0;

    IGMP_FILL_QUERY(pkt, &itype, 38, 1);
    IGMP_FILL_QUERY(pkt, &code, 39, 1);
    IGMP_FILL_QUERY(pkt, &csum, 40, 2);
    IGMP_FILL_QUERY(pkt, &gda, 42, 4);

    IGMP_FILL_QUERY(pkt, &rsv, 46, 1);
    IGMP_FILL_QUERY(pkt, &qqic, 47, 1);
    IGMP_FILL_QUERY(pkt, &srcNum, 48, 2);

    for (i = 0; i < numSrc; i++)
    {
        sip = ntohl(*(pSip + i));
        IGMP_FILL_QUERY(pkt, &sip, 50 + 4 * i, 4);
    }

    /* calculate cksum */
    igmp_util_checksum_get((uint16*)igmph, pktlen - 38, &csum);
    csum = htons(csum);

    IGMP_FILL_QUERY(pkt, &csum, 40, 2);

    pkt->as_txtag = TRUE;
    pkt->length = pktlen;

}

void mcast_igmp_send_general_query(igmp_querier_entry_t* qryPtr, uint8 igmp_query_version)
{
    sys_nic_pkt_t           *pkt = NULL;
    sys_logic_portmask_t    txPmsk;
    int32                   queryPduLen;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      mcastVlanEntry;

    if (DISABLED == p_igmp_stats->igmpsnp_enable)
        return;

    queryPduLen = (igmp_query_version == IGMP_QUERY_V3 ?
                        IGMP_QUERY_PDU_LEN + IGMPV3_HEAD_LEN + IGMPV3_OPTIONS_LEN :
                        IGMP_QUERY_PDU_LEN);

    osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
    mcastVlanEntry.vid      = qryPtr->vid;
    mcastVlanEntry.ipType   = ipType;
    mcast_vlan_get(&mcastVlanEntry);

    if (qryPtr->vid != mcastVlanEntry.vid)
        return;

    if ((ENABLED == mcastVlanEntry.enable) && (ENABLED == qryPtr->enabled))
    {
        if (NULL == (pkt = (sys_nic_pkt_t *)osal_alloc(sizeof(sys_nic_pkt_t))))
            goto alloc_fail;

        osal_memset(pkt, 0, sizeof(sys_nic_pkt_t));

        if (NULL == (pkt->data = (uint8 *)osal_alloc(queryPduLen)))
            goto alloc_fail;

        osal_memset(pkt->data, 0, queryPduLen);

        mcast_igmp_build_general_query(pkt, qryPtr->vid, queryPduLen, igmp_query_version, ENABLED, 0, IGMP_GENERAL_QUERY);

        LOGIC_PORTMASK_SET_ALL(txPmsk);

        SYS_DBG(LOG_DBG_IGMP, "%s() %d Send General Query in VLAN=%d to txPmsk=%x\n",
            __FUNCTION__, __LINE__, qryPtr->vid, *(txPmsk.bits));

        mcast_snooping_tx(pkt, qryPtr->vid, pkt->length, &txPmsk);

        if (IGMP_QUERY_V3 == igmp_query_version)
        {
            p_igmp_stats->v3.g_queryV3_xmit++;      /* General query */
        }
        else
        {
            p_igmp_stats->g_query_xmit++;      /* General query */
        }
        mcastVlanEntry.operRespTime = mcastVlanEntry.responseTime;
        mcastVlanEntry.operQueryIntv = mcastVlanEntry.queryIntv;
        mcastVlanEntry.operRobNessVar = mcastVlanEntry.robustnessVar;
        mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.robustnessVar;
        mcast_vlan_set(&mcastVlanEntry);
        osal_free(pkt->data);
        osal_free(pkt);
    }
    return;
alloc_fail:
    if (pkt)
        osal_free(pkt);
    SYS_DBG(LOG_DBG_IGMP, "igmp_snooping_send_general_query: dev_alloc_skb() failed.\n");
    return;
}

void mcast_igmp_send_grp_specific_query(uint16 vid, uint32 dip, sys_logic_port_t port)
{
    sys_nic_pkt_t           *pkt = NULL;
    sys_logic_portmask_t    txPmsk;
    uint16                  pktlen = 0, sortedIdx;
    igmp_querier_entry_t    *pEntry = NULL;
    igmp_group_entry_t      *groupHead = NULL,  groupEntry;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      mcastVlanEntry;

    if (DISABLED == p_igmp_stats->igmpsnp_enable)
        return;

    LOGIC_PORTMASK_CLEAR_ALL(txPmsk);
    LOGIC_PORTMASK_SET_PORT(txPmsk, port);

    mcast_querier_db_get(vid, ipType, &pEntry);

    if (!pEntry)
        return;

    pktlen =  (IGMP_QUERY_V2 == pEntry->version ?
                IGMP_QUERY_PDU_LEN + IGMPV3_OPTIONS_LEN :
                IGMP_QUERY_V3_PDU_LEN + IGMPV3_OPTIONS_LEN);

    #if 0
    mcast_router_db_get(vid, ipType, &pRouterEntry);
    if (NULL != pRouterEntry)
    {
        LOGIC_PORTMASK_OR(txPmsk,txPmsk,pRouterEntry->router_pmsk);
    }
   #endif

    mcast_build_groupEntryKey(vid, ipType, dip, NULL, &groupEntry);
    /* Only compare dip */
    mcast_group_sortedArray_search(mcast_group_sortKey_ret(&groupEntry), &sortedIdx, &groupHead);
    if (NULL == groupHead)
    {
        //LOGIC_PORTMASK_OR(txPmsk, txPmsk, groupHead->mbr);
        return ;
    }

    if (!IS_LOGIC_PORTMASK_PORTSET(groupHead->mbr, port))
        return;

#if 0
    mcast_igmp_operVersion_get(&version);

    if (IGMP_VERSION_V3_FULL == version)
    {
        pGroupEntry = groupHead;
        while(pGroupEntry)
        {
            LOGIC_PORTMASK_OR(txPmsk,txPmsk,pGroupEntry->mbr);
            pGroupEntry = pGroupEntry->next_subgroup;
        }
    }

    if (IS_LOGIC_PORTMASK_CLEAR(txPmsk))
     return;
#endif

    if (NULL == (pkt = (sys_nic_pkt_t *)osal_alloc(sizeof(sys_nic_pkt_t))))
        goto alloc_fail;

    osal_memset(pkt,0,sizeof(sys_nic_pkt_t));

    if (NULL == (pkt->data = (uint8 *)osal_alloc(pktlen)))
        goto alloc_fail;

    osal_memset(pkt->data,0,pktlen);

    mcast_igmp_build_gs_query(pkt, vid,pktlen, dip, pEntry->version);

    if (SYS_ERR_OK != mcast_snooping_tx(pkt, vid, pktlen, &txPmsk))
    {
        SYS_DBG(LOG_DBG_IGMP, "mcast_snooping_tx() failed! \n");
    }
    else
    {
        if (IGMP_QUERY_V3 == pEntry->version)
        {
            p_igmp_stats->v3.gs_queryV3_xmit++;
        }
        else
        {
            p_igmp_stats->gs_query_xmit++;
        }

        SYS_DBG(LOG_DBG_IGMP, "Send Group Specific Query ("IPADDR_PRINT") in VLAN-%d .\n",
            IPADDR_PRINT_ARG(dip), vid);

        osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
        mcastVlanEntry.vid = vid;
        mcastVlanEntry.ipType = ipType;
        mcast_vlan_get(&mcastVlanEntry);

        if (vid == mcastVlanEntry.vid)
        {
            mcastVlanEntry.operGsqueryIntv = mcastVlanEntry.grpSpecificQueryIntv;
            mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.lastMmbrQueryCnt;
            mcast_vlan_set(&mcastVlanEntry);
        }
    }
    osal_free(pkt->data);
    osal_free(pkt);
    return;
alloc_fail:
    if (pkt)
        osal_free(pkt);
    SYS_DBG(LOG_DBG_IGMP, "igmp_snooping_send_general_query: dev_alloc_skb() failed.\n");
    return;
}


void mcast_igmp_send_grp_src_specific_query(uint16 vid, uint32 dip, uint32 *sip, uint16 numSrc, sys_logic_port_t port)
{
    sys_nic_pkt_t           *pkt = NULL;
    sys_logic_portmask_t    txPmsk;
    uint16                  pktLen = 0;
    int                     i;
    igmp_querier_entry_t    *pEntry = NULL;
    igmp_router_entry_t     *pRouterEntry = NULL;
    igmp_group_entry_t      *pGroup = NULL, groupEntry;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      mcastVlanEntry;

    if (DISABLED == p_igmp_stats->igmpsnp_enable)
        return;

    LOGIC_PORTMASK_CLEAR_ALL(txPmsk);

    mcast_querier_db_get(vid, ipType, &pEntry);

    if (!pEntry)
        return;

    mcast_router_db_get(vid, ipType, &pRouterEntry);

    if (pRouterEntry)
    {
        LOGIC_PORTMASK_OR(txPmsk, txPmsk, pRouterEntry->router_pmsk);
    }

    mcast_build_groupEntryKey(vid, ipType, dip, NULL, &groupEntry);

    for (i = 0; i < numSrc; i++)
    {
        groupEntry.sip = sip[i];
        pGroup = mcast_group_get(&groupEntry);
        if (NULL != pGroup)
        {
            LOGIC_PORTMASK_OR(txPmsk, txPmsk, pGroup->mbr);
        }
    }

    if (IS_LOGIC_PORTMASK_CLEAR(txPmsk))
        return;

    pktLen =  IGMP_QUERY_PDU_LEN + 1 + 1  + 2 + numSrc * 4; /*rserver + SQRV+QQIC+sourceNum + sourceList*/

    if (NULL == (pkt = (sys_nic_pkt_t *)osal_alloc(sizeof(sys_nic_pkt_t))))
        goto alloc_fail;

    osal_memset(pkt, 0, sizeof(sys_nic_pkt_t));

    if (NULL == (pkt->data = (uint8 *)osal_alloc(pktLen)))
        goto alloc_fail;

    osal_memset(pkt->data, 0, pktLen);

    mcast_igmp_build_gss_query(pkt, vid, dip, sip, numSrc, pktLen);

    if (SYS_ERR_OK != mcast_snooping_tx(pkt, vid, pktLen, &txPmsk))
    {
        SYS_DBG(LOG_DBG_IGMP, "mcast_snooping_tx() failed!  \n");
    }
    else
    {
        p_igmp_stats->v3.gss_queryV3_xmit++;

        SYS_DBG(LOG_DBG_IGMP, "Send Group Specific Source Specific Query ("IPADDR_PRINT") in VLAN-%d. \n",
            IPADDR_PRINT_ARG(dip), vid);

        osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
        mcastVlanEntry.vid = vid;
        mcastVlanEntry.ipType = ipType;
        mcast_vlan_get(&mcastVlanEntry);

        if (vid == mcastVlanEntry.vid)
        {
            mcastVlanEntry.operGsqueryIntv = mcastVlanEntry.grpSpecificQueryIntv;
            mcast_vlan_set(&mcastVlanEntry);
        }
    }
    osal_free(pkt->data);
    osal_free(pkt);
    return;
alloc_fail:
    if (pkt)
        osal_free(pkt);
    SYS_DBG(LOG_DBG_IGMP, "mcast_igmp_send_grp_src_specific_query: dev_alloc_skb() failed.\n");
    return;
}

int32 mcast_igmp_querier_check(uint16 vid, uint32 sip, uint8 qver)
{
    igmp_querier_entry_t    *pEntry;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV4;
    mcast_vlan_entry_t      *pMcastVlan = NULL;

    SYS_PARAM_CHK(IS_VID_INVALID(vid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)), SYS_ERR_VLAN_ID);

    IGMP_DB_SEM_LOCK();

    mcast_vlan_db_get(vid, ipType, &pMcastVlan);
    if (!pMcastVlan)
    {
        IGMP_DB_SEM_UNLOCK();
        return SYS_ERR_FAILED;
    }

    mcast_querier_db_get(vid, ipType, &pEntry);
    if (pEntry && pEntry->enabled)
    {
        if (sip < pEntry->ip || (pEntry->ip == 0 && sip < igmpCtrl.sys_ip))
        {
            pEntry->ip = sip;
            pEntry->timer = pMcastVlan->otherQuerierPresentInterval;
            pEntry->status = IGMP_NON_QUERIER;
            pEntry->version = qver;
        }
        else if ((igmpCtrl.sys_ip <= pEntry->ip && igmpCtrl.sys_ip < sip) ||
            (pEntry->ip == 0 && igmpCtrl.sys_ip < sip))
        {
            pEntry->ip = igmpCtrl.sys_ip;
            pEntry->status = IGMP_QUERIER;
        }
        else if (pEntry->ip == sip)
        {
            pEntry->timer = pMcastVlan->otherQuerierPresentInterval;
            pEntry->status = IGMP_NON_QUERIER;
            pEntry->version = qver;
        }
    }
    else if (pEntry == NULL)
    {
        SYS_DBG(LOG_DBG_IGMP, "%s():%d  Warring! Querier for VLAN-%d doesn't exist!\n", __FUNCTION__, __LINE__, vid);
    }
    IGMP_DB_SEM_UNLOCK();

    return SYS_ERR_OK;
}

void mcast_igmp_querier_timer(void)
{
    uint16                  i, time;
    igmp_querier_entry_t    *sortedArray ;
    static uint32           cnt;
    multicast_ipType_t      ipType;
    mcast_vlan_entry_t      *pMcstVlan = NULL;

    /* Periodically update sys_ip */
    cnt++;
    if (cnt == 30)
        cnt = 0;

    IGMP_DB_SEM_LOCK();
    for (ipType = MULTICAST_TYPE_IPV4; ipType < MULTICAST_TYPE_END; ipType++)
    {
        sortedArray = querier_db[ipType].p_querier_entry;

        for (i = 0; i < querier_db[ipType].entry_num; i++)
        {
            if (sortedArray[i].enabled)
            {
                mcast_vlan_db_get(sortedArray[i].vid, ipType, &pMcstVlan);

                if (!pMcstVlan)
                {
                    continue;
                }

                if (gNetworkEnabled && sortedArray[i].startupQueryCnt <= pMcstVlan->robustnessVar)
                {
                    /* Tx querier pkt */
                    //mcast_igmp_send_general_query(&sortedArray[i], sortedArray[i].version);
                    mcast_send_general_query(&sortedArray[i], sortedArray[i].version);
                    sortedArray[i].startupQueryCnt++;
                }

                time = sortedArray[i].timer;
                if (time != 0)
                {
                    if (time > PASS_SECONDS)
                    {
                        /* If user change the time period, update it */
                        if (time > pMcstVlan->otherQuerierPresentInterval)
                        {
                            sortedArray[i].timer = pMcstVlan->otherQuerierPresentInterval;
                        }
                        sortedArray[i].timer -= PASS_SECONDS;
                    }
                    else
                    {
                        /* Tx querier pkt */
                        //mcast_igmp_send_general_query(&sortedArray[i], sortedArray[i].version);
                        mcast_send_general_query(&sortedArray[i], sortedArray[i].version);

                        //received query. than clear the suppre flag.
                        mcast_suppreFlag_clear(sortedArray[i].vid, ipType);

                        /* If non-Querier switch doesn't receive any query packet before a
                        p_igmp_stats->otherQuerierPresentInterval time, should clear ip */
                        sortedArray[i].ip = igmpCtrl.sys_ip;
                        sortedArray[i].timer = pMcstVlan->queryIntv;
                        sortedArray[i].status = IGMP_QUERIER;
                        osal_time_usleep(10 * 1000); /* sleep for 0.01 sec */
                    }
                }

            }
        }
    }
    IGMP_DB_SEM_UNLOCK();
}

void mcast_mld_build_general_query (sys_nic_pkt_t * pkt, sys_vid_t vid, uint16 pktLen, uint8 version)
{
    uint8               sflag_rob = 0x02, qqi = 0x14, bufcsm[32 + 4 + 4 + 28]; /*dip +sip, len ,nex_head,mld*/
    uint8               saddr[IPV6_ADDR_LEN];
    uint16              cvid, offset = 0, mld_offset = 0, tot_len, max_response, mldHdrCsm = 0;
    multicast_ipType_t  ipType       = MULTICAST_TYPE_IPV6;
    mcast_vlan_entry_t  *pMcastVlan  = NULL;
    sys_ipv6_addr_t     ipv6Addr;
    uint16              ctag         = htons(0x8100);
    uint16              type         = htons(0x86dd);
    uint16              vtf          = htons(0x6000);
    uint16              flowlabel    = htons(0x0000);
    uint16              padn         = htons(0x0100);
    uint16              rev          = htons(0x0000);
    uint16              numSrc       = htons(0x0000);
    uint8               ipv6_nextHrd = htons(0x00);
    uint8               hopLimit     = htons(0x01);
    uint8               hop_nextHrd  = htons(0x3a);
    uint8               hop_len      = htons(0x0);
    uint8               routerAlert[]= {0x05, 0x02, 0x00, 0x00};
    uint8               reportType   = htons(0x82);
    uint8               code         = htons(0x0);
    uint8               dst_mac[]    = {0x33, 0x33, 0x00, 0x00, 0x00, 0x01};
    uint8               daddr[IPV6_ADDR_LEN] =
                            {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00,0x01};

    mcast_vlan_db_get(vid,ipType, &pMcastVlan);

    if (!pMcastVlan)
    {
        return;
    }

    tot_len = (MLD_QUERY_V1 == version ? 32 : 36);

    tot_len = htons(tot_len);
    cvid = (uint16)htons(vid);
    max_response = pMcastVlan->responseTime * 1000;
    max_response = htons(max_response);
    osal_memset(bufcsm,0,sizeof(bufcsm));

    /* ethernet II header */
    IGMP_FILL_QUERY(pkt, &dst_mac, offset, 6);
    offset += 6;
    IGMP_FILL_QUERY(pkt, &igmpCtrl.sys_mac, offset, 6);
    offset += 6;
    IGMP_FILL_QUERY(pkt, &ctag, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &cvid, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &type, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &vtf, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &flowlabel, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &tot_len, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &ipv6_nextHrd, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &hopLimit, offset, 1);
    offset += 1;
    osal_memset(saddr, 0, sizeof(saddr));
    osal_memset(ipv6Addr.ipv6_addr, 0 , IPV6_ADDR_LEN);//rsd_sys_ipv6Addr_get(&ipv6Addr);
    osal_memcpy(saddr, ipv6Addr.ipv6_addr, IPV6_ADDR_LEN);
    IGMP_FILL_QUERY(pkt, saddr, offset, 16);
    offset += 16;
    IGMP_FILL_QUERY(pkt, daddr, offset, 16);
    offset += 16;
    IGMP_FILL_QUERY(pkt, &hop_nextHrd, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &hop_len, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, routerAlert, offset, 4);
    offset += 4;
    IGMP_FILL_QUERY(pkt, &padn, offset, 2);
    offset += 2;
    mld_offset = offset;
    IGMP_FILL_QUERY(pkt, &reportType, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &code, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &mldHdrCsm, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &max_response, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &rev, offset, 2);
    offset += 2;

    osal_memset(saddr,0,sizeof(saddr));
    IGMP_FILL_QUERY(pkt, saddr, offset, 16);
    offset += 16;

    osal_memcpy(bufcsm, &pkt->data[26], 32);

    if (MLD_QUERY_V2 == version)
    {
        sflag_rob = (pMcastVlan->robustnessVar) & 0x7;
        mcast_code_convert(pMcastVlan->queryIntv, &qqi);

        IGMP_FILL_QUERY(pkt, &sflag_rob, offset, 1);
        offset += 1;
        IGMP_FILL_QUERY(pkt, &qqi, offset, 1);
        offset += 1;
        IGMP_FILL_QUERY(pkt, &numSrc, offset, 2);
        offset += 2;

        bufcsm[35] = 28;
        bufcsm[39] = 0x3a;
        osal_memcpy(&bufcsm[40], &pkt->data[mld_offset], 28);
        igmp_util_checksum_get((uint16 *)bufcsm, sizeof(bufcsm), &mldHdrCsm);
    }
    else
    {
        bufcsm[35] = 24;
        bufcsm[39] = 0x3a;
        osal_memcpy(&bufcsm[40], &pkt->data[mld_offset], 24);
        igmp_util_checksum_get((uint16 *)bufcsm, sizeof(bufcsm) - 4, &mldHdrCsm);
    }

    pkt->as_txtag = TRUE;
    pkt->length = pktLen;
}

void mcast_mld_build_gs_query(sys_nic_pkt_t * pkt, sys_vid_t vid, uint16 pktLen, uint8 *pIpv6Addr, uint16 version)
{
    uint8               sflag_rob = 0x02, qqi = 0x14, bufcsm[32 + 4 + 4 + 28];  /*dip +sip, len ,nex_head,mld*/
    uint8               saddr[IPV6_ADDR_LEN], daddr[IPV6_ADDR_LEN];
    uint16              cvid, tot_len, max_response, mldHdrCsm = 0, offset = 0, mld_offset = 0;
    multicast_ipType_t  ipType = MULTICAST_TYPE_IPV6;
    mcast_vlan_entry_t  *pMcastVlan = NULL;
    sys_ipv6_addr_t     ipv6Addr;
    uint16              ctag         = htons(0x8100);
    uint16              type         = htons(0x86dd);
    uint16              vtf          = htons(0x6000);
    uint16              flowlabel    = htons(0x0000);
    uint16              numSrc       = htons(0x0000);
    uint16              rev          = htons(0x0000);
    uint16              padn         = htons(0x0100);
    uint8               ipv6_nextHrd = htons(0x00);
    uint8               hopLimit     = htons(0x01);
    uint8               hop_nextHrd  = htons(0x3a);
    uint8               hop_len      = htons(0x0);
    uint8               routerAlert[]= {0x05, 0x02, 0x00, 0x00};
    uint8               reportType   = htons(0x82);
    uint8               code         = htons(0x0);
    uint8               dst_mac[6]   = {0x33, 0x33, 0x00, 0x00, 0x00, 0x00};

    mcast_vlan_db_get(vid,ipType, &pMcastVlan);

    if (!pMcastVlan)
    {
        return;
    }

    tot_len = (MLD_QUERY_V1 == version ? 32 : 36);

    tot_len = htons(tot_len);
    cvid = (uint16)htons(vid);
    max_response = pMcastVlan->responseTime * 1000;
    max_response = htons(max_response);
    osal_memset(bufcsm,0,sizeof(bufcsm));

    osal_memcpy(dst_mac + 2, pIpv6Addr+12, 4);
    osal_memcpy(daddr, pIpv6Addr, IPV6_ADDR_LEN);

    /* ethernet II header */
    IGMP_FILL_QUERY(pkt, &dst_mac, offset, 6);
    offset += 6;
    IGMP_FILL_QUERY(pkt, &igmpCtrl.sys_mac, offset, 6);
    offset += 6;
    IGMP_FILL_QUERY(pkt, &ctag, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &cvid, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &type, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &vtf, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &flowlabel, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &tot_len, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &ipv6_nextHrd, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &hopLimit, offset, 1);
    offset += 1;

    osal_memset(saddr, 0, sizeof(saddr));
    osal_memset(ipv6Addr.ipv6_addr, 0, IPV6_ADDR_LEN);//rsd_sys_ipv6Addr_get(&ipv6Addr);
    osal_memcpy(saddr, ipv6Addr.ipv6_addr, IPV6_ADDR_LEN);
    IGMP_FILL_QUERY(pkt, saddr, offset, 16);
    offset += 16;
    IGMP_FILL_QUERY(pkt, daddr, offset, 16);
    offset += 16;
    IGMP_FILL_QUERY(pkt, &hop_nextHrd, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &hop_len, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, routerAlert, offset, 4);
    offset += 4;
    IGMP_FILL_QUERY(pkt, &padn, offset, 2);
    offset += 2;
    mld_offset = offset;
    IGMP_FILL_QUERY(pkt, &reportType, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &code, offset, 1);
    offset += 1;
    IGMP_FILL_QUERY(pkt, &mldHdrCsm, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &max_response, offset, 2);
    offset += 2;
    IGMP_FILL_QUERY(pkt, &rev, offset, 2);
    offset += 2;

    osal_memset(saddr,0,sizeof(saddr));
    IGMP_FILL_QUERY(pkt, saddr, offset, 16);
    offset += 16;

    osal_memcpy(bufcsm, &pkt->data[26], 32);

    if (MLD_QUERY_V2 == version)
    {
        sflag_rob = (pMcastVlan->lastMmbrQueryCnt) & 0x7;
        mcast_code_convert(pMcastVlan->grpSpecificQueryIntv, &qqi);

        IGMP_FILL_QUERY(pkt, &sflag_rob, offset, 1);
        offset += 1;
        IGMP_FILL_QUERY(pkt, &qqi, offset, 1);
        offset += 1;
        IGMP_FILL_QUERY(pkt, &numSrc, offset, 2);
        offset += 2;

        bufcsm[35] = 28;
        bufcsm[39] = 0x3a;
        osal_memcpy(&bufcsm[40], &pkt->data[mld_offset], 28);
        igmp_util_checksum_get((uint16 *)bufcsm, sizeof(bufcsm), &mldHdrCsm);
    }
    else
    {
        bufcsm[35] = 24;
        bufcsm[39] = 0x3a;
        osal_memcpy(&bufcsm[40], &pkt->data[mld_offset], 24);
        igmp_util_checksum_get((uint16 *)bufcsm, sizeof(bufcsm) - 4, &mldHdrCsm);
    }

    pkt->as_txtag = TRUE;
    pkt->length = pktLen;
}

void mcast_mld_send_general_query(igmp_querier_entry_t* qryPtr, uint8 query_version)
{
    sys_nic_pkt_t           *pkt;
    sys_logic_portmask_t    txPmsk;
    int32                   queryPduLen;
    multicast_ipType_t      ipType = MULTICAST_TYPE_IPV6;
    mcast_vlan_entry_t      mcastVlanEntry;

    if (DISABLED == p_mld_stats->mldsnp_enable)
        return;

    queryPduLen = (query_version == MLD_QUERY_V1 ? MLD_QUERY_V1_PDU_LEN : MLD_QUERY_V2_PDU_LEN);

    osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
    mcastVlanEntry.vid      = qryPtr->vid;
    mcastVlanEntry.ipType   = ipType;

    mcast_vlan_get(&mcastVlanEntry);

    if (qryPtr->vid != mcastVlanEntry.vid)
        return;

    if ((ENABLED == mcastVlanEntry.enable) && (ENABLED == qryPtr->enabled))
    {
        if (NULL == (pkt = (sys_nic_pkt_t *)osal_alloc(sizeof(sys_nic_pkt_t))))
            goto alloc_fail;

        osal_memset(pkt, 0, sizeof(sys_nic_pkt_t));

        if (NULL == (pkt->data = (uint8 *)osal_alloc(queryPduLen)))
            goto alloc_fail;

        osal_memset(pkt->data, 0, queryPduLen);

        mcast_mld_build_general_query(pkt, qryPtr->vid, queryPduLen, query_version);

        LOGIC_PORTMASK_SET_ALL(txPmsk);

        SYS_DBG(LOG_DBG_IGMP, "%s() %d Send MLD General Query in VLAN=%d txPmsk=%x\n",
            __FUNCTION__, __LINE__, qryPtr->vid, *(txPmsk.bits));

        mcast_snooping_tx(pkt, qryPtr->vid, pkt->length, &txPmsk);

        p_mld_stats->g_query_xmit++;

        mcastVlanEntry.operRespTime = mcastVlanEntry.responseTime;
        mcastVlanEntry.operQueryIntv = mcastVlanEntry.queryIntv;
        mcastVlanEntry.operRobNessVar = mcastVlanEntry.robustnessVar;
        mcastVlanEntry.operLastMmbrQueryCnt = mcastVlanEntry.robustnessVar;
        mcast_vlan_set(&mcastVlanEntry);
        osal_free(pkt->data);
        osal_free(pkt);
    }
    return;
alloc_fail:
    if (pkt)
        osal_free(pkt);

    SYS_DBG(LOG_DBG_MLD, "mcast_mld_send_general_query: dev_alloc_skb() failed.\n");
    return;
}

void mcast_mld_send_grp_specific_query(igmp_group_entry_t *pGroup, sys_logic_port_t port)
{
    sys_nic_pkt_t           *pkt            = NULL;
    sys_logic_portmask_t    txPmsk;
    uint16                  pktlen          = 0, sortedIdx;
    igmp_querier_entry_t    *pEntry         = NULL;
    igmp_group_entry_t      *groupHead      = NULL;
    igmp_router_entry_t     *pRouterEntry   = NULL;
    multicast_ipType_t      ipType          = MULTICAST_TYPE_IPV6;
    mcast_vlan_entry_t      mcastVlanEntry;

    if (DISABLED == p_mld_stats->mldsnp_enable)
        return;

    LOGIC_PORTMASK_CLEAR_ALL(txPmsk);

    mcast_querier_db_get(pGroup->vid, ipType, &pEntry);

    if (!pEntry)
        return;

    pktlen = (MLD_QUERY_V1 == pEntry->version ? MLD_QUERY_V1_PDU_LEN : MLD_QUERY_V2_PDU_LEN);

    mcast_router_db_get(pGroup->vid, ipType, &pRouterEntry);

    if (pRouterEntry)
    {
        LOGIC_PORTMASK_OR(txPmsk, txPmsk, pRouterEntry->router_pmsk);
    }

    /* Only compare dip */
    mcast_group_sortedArray_search(mcast_group_sortKey_ret(pGroup), &sortedIdx, &groupHead);

    if (groupHead)
    {
        LOGIC_PORTMASK_OR(txPmsk, txPmsk, groupHead->mbr);
    }

#if 0
    mcast_igmp_operVersion_get(&version);

    if (IGMP_VERSION_V3_FULL == version)
    {
        pGroupEntry = groupHead;
        while(pGroupEntry)
        {
            LOGIC_PORTMASK_OR(txPmsk,txPmsk,pGroupEntry->mbr);
            pGroupEntry = pGroupEntry->next_subgroup;
        }
    }
#endif

    if (IS_LOGIC_PORTMASK_CLEAR(txPmsk))
        return;

    if (NULL == (pkt = (sys_nic_pkt_t *)osal_alloc(sizeof(sys_nic_pkt_t))))
        goto alloc_fail;

    osal_memset(pkt,0,sizeof(sys_nic_pkt_t));

    if (NULL == (pkt->data = (uint8 *)osal_alloc(pktlen)))
        goto alloc_fail;

    osal_memset(pkt->data, 0, pktlen);

    mcast_mld_build_gs_query(pkt, pGroup->vid, pktlen, pGroup->dipv6.addr, pEntry->version);

    if (SYS_ERR_OK != mcast_snooping_tx(pkt, pGroup->vid, pktlen, &txPmsk))
    {
        SYS_DBG(LOG_DBG_MLD, "mcast_snooping_tx() failed!\n");
    }
    else
    {
        p_mld_stats->gs_query_xmit++;

        SYS_DBG(LOG_DBG_MLD, "Send Group Specific MLD Query ("IPADDR_PRINT") in VLAN-%d .\n",
            IPADDR_PRINT_ARG(pGroup->dip), pGroup->vid);

        osal_memset(&mcastVlanEntry, 0, sizeof(mcastVlanEntry));
        mcastVlanEntry.vid      = pGroup->vid;
        mcastVlanEntry.ipType   = ipType;

        mcast_vlan_get(&mcastVlanEntry);

        if (pGroup->vid == mcastVlanEntry.vid)
        {
            mcastVlanEntry.operGsqueryIntv = mcastVlanEntry.grpSpecificQueryIntv;
            mcast_vlan_set(&mcastVlanEntry);
        }
    }
    osal_free(pkt->data);
    osal_free(pkt);
    return;
alloc_fail:
    if (pkt)
        osal_free(pkt);
    SYS_DBG(LOG_DBG_IGMP, "igmp_snooping_send_general_query: dev_alloc_skb() failed.\n");
    return;
}

void mcast_send_general_query( igmp_querier_entry_t *qryPtr, uint8 version)
{
    if (!qryPtr)
        return;

    if (MULTICAST_TYPE_IPV4 == qryPtr->ipType)
    {
        mcast_igmp_send_general_query(qryPtr, version);
    }
    else
    {
        mcast_mld_send_general_query(qryPtr, version);
    }
    return;
}

void mcast_send_gs_query(igmp_group_entry_t *pGroup, sys_logic_port_t lport)
{
    if (!pGroup)
        return;

    if (MULTICAST_TYPE_IPV4 == pGroup->ipType)
    {
        mcast_igmp_send_grp_specific_query(pGroup->vid, pGroup->dip, lport);
    }
    else
    {
        mcast_mld_send_grp_specific_query(pGroup, lport);
    }
    return;
}

