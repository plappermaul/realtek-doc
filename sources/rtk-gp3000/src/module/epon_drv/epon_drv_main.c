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
 *
 * $Revision: 51194 $
 * $Date: 2014-09-10 16:36:56 +0800 (Wed, 10 Sep 2014) $
 *
 * Purpose : EPON kernel drivers
 *
 * Feature : This module install EPON kernel callbacks and other 
 * kernel functions
 *
 */

#ifdef CONFIG_EPON_FEATURE
/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#include <net/sock.h> 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
#include <linux/proc_fs.h>
#endif
#include <linux/timekeeping.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/epon.h>
#include <rtk/port.h>
#include <rtk/switch.h>
#include <rtk/trap.h>
#include <pkt_redirect.h>

#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif

#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b.h>
#endif

#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c.h>
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c.h>
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>
#endif

#ifdef CONFIG_COMMON_RT_API
#include "rtk/rt/rt_ponmisc.h"
#include "rtk/rt/rt_epon.h"
#endif


#if (!defined(CONFIG_RTK_L34_ENABLE) || defined(CONFIG_HYBRID_MODE)) && (defined(CONFIG_RTL9607C_SERIES) || defined(CONFIG_RTL9602C_SERIES))
#include <common/rt_type.h>
#include <rtk/svlan.h>
#include <common/util/rt_util.h>
#endif

#include "module/intr_bcaster/intr_bcaster.h"

#define MAX_OAM_EVENT_SIZE (sizeof(rtk_epon_oam_event_t) + 2) /* magic key size 2 */

#define VTAG2VLANTCI(v) (( (((v) & 0xff00)>>8) | (((v) & 0x00ff)<<8) ) + 1) 

#if defined(CONFIG_HYBRID_MODE)
int epon_devmode = 1;	// 1 router   0 bridge
#endif

void skb_push_qtag(struct sk_buff *pSkb, unsigned short usVid, unsigned char ucPriority)
{
    if(usVid){
        /*push switch header*/
        skb_push(pSkb, 4); /*pSkb->data -= 4; pSkb->len += 4*/
        memmove(pSkb->data, 
                pSkb->data + 4, 
                (2 * 6 /* MAC_ADDR_LEN */));
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */)) = 0x81;
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */) + 1) = 0x00;
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */) + 2) = (unsigned char)(((usVid >> 8) & 0xF) | (ucPriority << 5));
        *(pSkb->data + (2 * 6 /* MAC_ADDR_LEN */) + 3) = usVid & 0xFF;
    }    
}

#if (!defined(CONFIG_RTK_L34_ENABLE) || defined(CONFIG_HYBRID_MODE)) && (defined(CONFIG_RTL9607C_SERIES) || defined(CONFIG_RTL9602C_SERIES))
static int32 epon_svlan_member_add(uint32 ulSvlanId, rtk_portmask_t stPhyMask, rtk_portmask_t stPhyMaskUntag)
{
	int32 ret;
    rtk_portmask_t stPortmask, stUntagPortmask;

    ret = rtk_svlan_memberPort_get(ulSvlanId, &stPortmask, &stUntagPortmask);

	if(RT_ERR_SVLAN_NOT_EXIST == ret || RT_ERR_SVLAN_ENTRY_NOT_FOUND == ret 
		|| RT_ERR_VLAN_ENTRY_NOT_FOUND == ret)
	{
		/*try create svlan if not exist*/
		ret = rtk_svlan_create(ulSvlanId);
		if(RT_ERR_OK != ret)
    	{
        	return RT_ERR_FAILED;
    	}
		RTK_PORTMASK_RESET(stPortmask);
		RTK_PORTMASK_RESET(stUntagPortmask);
	}
    else if (RT_ERR_OK != ret)
    {
        return RT_ERR_FAILED;
    }

    stPortmask.bits[0] |= stPhyMask.bits[0];
    stUntagPortmask.bits[0] |= stPhyMaskUntag.bits[0];
    ret = rtk_svlan_memberPort_set(ulSvlanId, &stPortmask, &stUntagPortmask);
    if(RT_ERR_OK != ret)
    {
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

static int32 epon_svlan_member_remove(uint32 ulSvlanId, rtk_portmask_t stPhyMask)
{
	int32 ret;    
    rtk_portmask_t stPortmask, stUntagPortmask;
	uint32 uiFid;

   	ret = rtk_svlan_memberPort_get(ulSvlanId, &stPortmask, &stUntagPortmask);
	/*because svlan and cvlan share same entry in vlan table,
	we assume the specified vlan entry is deleted successfully if return value is faile*/
	if (RT_ERR_OK != ret)
    {
        return RT_ERR_OK;
    }

    stPortmask.bits[0] &= ~(stPhyMask.bits[0]);

	ret = rtk_svlan_memberPort_set(ulSvlanId, &stPortmask, &stUntagPortmask);
	if (RT_ERR_OK != ret)
	{
		return RT_ERR_FAILED;
	}
    return RT_ERR_OK;
}

static int32 epon_maintain_vlan_table(unsigned char isAdd, unsigned char uiPPort)
{
	int32 ret;
    rtk_portmask_t stPhyMask, stPhyMaskUntag;
	uint16 vid;

	RTK_PORTMASK_RESET(stPhyMask);
    RTK_PORTMASK_RESET(stPhyMaskUntag);

	RTK_PORTMASK_PORT_SET(stPhyMask, uiPPort);
	if(isAdd)		
		rtk_switch_port2PortMask_set(&stPhyMask, RTK_PORT_PON);	

	/* Set all VLAN configuration */
	for(vid = 1; vid <= 4094; vid++)
	{
		if(isAdd)
		{
			ret = epon_svlan_member_add(vid, stPhyMask, stPhyMaskUntag);
		    if (RT_ERR_OK != ret)
		    {
		    	printk("%s %d: vid[%d] fail ret[%d]\n", __FUNCTION__, __LINE__, vid, ret);
		        return RT_ERR_FAILED;
		    }
		}
		else
		{
			ret = epon_svlan_member_remove(vid, stPhyMask);
		    if (RT_ERR_OK != ret)
		    {
		    	printk("%s %d: vid[%d] fail ret[%d]\n", __FUNCTION__, __LINE__, vid, ret);
		        return RT_ERR_FAILED;
		    }
		}
	}
	return RT_ERR_OK;
}
#endif

static unsigned int rtl960x_pon_port_mask;
static unsigned int rtl960x_all_port_mask;

#define SUPPORT_OAM_KEEPLIVE
#ifdef SUPPORT_OAM_KEEPLIVE
/* siyuan 2017-08-07: oam keeplive mode: we parse the received standard oam discovery complete info packet,
   then generate oam info reply packet and send it to olt directly to avoid the case that eponoamd has no chance to send it in time
   when cpu is busy, otherwise olt will send a mpcp flag 2 to onu which causes onu offline. 
   also drop the standard oam info reply packet send by eponoamd.
 */
#define MAX_OAM_INFO_SIZE 					128
#define EPON_OAM_INFO_COMPLETE_FLAG1 		0x00
#define EPON_OAM_INFO_COMPLETE_FLAG2 		0x50
#define EPON_OAM_SUPPORT_LLID_NUM       	1    /* FIXME: only support llid 0 now */
#define EPON_OAM_OUI_LENGTH                 0x03
/* Info OAMPDU related */
#define EPON_OAMPDU_CODE_INFO               0x00
#define EPON_INFO_OAMPDU_INFO_LEN           0x10
#define EPON_INFO_HEADER_LEN           		0x12
#define EPON_INFO_OAMPDU_VENDER_SPEC_LEN    0x04
/* Info OAMPDU TLV types */
#define EPON_INFO_OAMPDU_TYPE_END           0x00
#define EPON_INFO_OAMPDU_TYPE_LOCAL         0x01
#define EPON_INFO_OAMPDU_TYPE_REMOTE        0x02
#define EPON_INFO_OAMPDU_TYPE_ORG_SPEC      0xFE
#define EPON_INFO_OAMPDU_TYPE_RESERVED      0xFF
#define EPON_INFO_OAMPDU_ORGSPEC_MIN_LEN    0x05 /* Type(1) + Length(1) + OUI(3) */

typedef struct oam_oamInfo_s {
	unsigned char valid;			/* If the OAM info valid */
	unsigned char oamVer;			/* OAM version defined in 802.3 section 57 */
	unsigned short revision;		/* Revision defined in 802.3 section 57 */
	unsigned char state;			/* State defined in 802.3 section 57 */
	unsigned char oamConfig;		/* OAM config defined in 802.3 section 57 */
	unsigned short oamPduConfig;	/* OAMPDU config defined in 802.3 section 57 */
	unsigned char oui[3];			/* OUI version defined in 802.3 section 57 */
	unsigned char venderSpecInfo[4];/* Vender Specific Info defined in 802.3 section 57 */
} oam_oamInfo_t;

typedef struct oam_config_s {
   unsigned char macAddr[6];        /* MAC address through out all OAM protocol */
} oam_config_t;

static unsigned char oam_info_reply[MAX_OAM_INFO_SIZE];
static unsigned char oam_info_keeplive_enable;
static oam_oamInfo_t localOamInfo[EPON_OAM_SUPPORT_LLID_NUM];
static oam_oamInfo_t remoteOamInfo[EPON_OAM_SUPPORT_LLID_NUM];
static oam_config_t  oamConfig[EPON_OAM_SUPPORT_LLID_NUM];
static long long oamrxtime[EPON_OAM_SUPPORT_LLID_NUM];

static unsigned char infoOrgSpecDb[][3] = {
	{ 0x11, 0x11, 0x11 }  /* CTC_OAM_OUI */
};

static void epon_oam_info_stdHdr_gen(unsigned short llidIdx, unsigned char *pFrameBuf)
{
    const unsigned char oamDstMac[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};
    unsigned short oamPduFlag;

    /* Already check the size at caller side */
    memcpy(pFrameBuf, oamDstMac, sizeof(oamDstMac));
    pFrameBuf += sizeof(oamDstMac);
    memcpy(pFrameBuf, oamConfig[llidIdx].macAddr, sizeof(oamConfig[llidIdx].macAddr));
    pFrameBuf += sizeof(oamConfig[llidIdx].macAddr);
    /* EtherType */
    *pFrameBuf = 0x88;
    pFrameBuf ++;
    *pFrameBuf = 0x09;
    pFrameBuf ++;
    /* SubType */
    *pFrameBuf = 0x03;
    pFrameBuf ++;
    /* Flag */
    *pFrameBuf = EPON_OAM_INFO_COMPLETE_FLAG1;
    pFrameBuf ++;
    *pFrameBuf = EPON_OAM_INFO_COMPLETE_FLAG2;
    pFrameBuf ++;
    /* Code */
    *pFrameBuf = EPON_OAMPDU_CODE_INFO;
}

static int epon_oam_localInfo_get(unsigned char llidIdx, oam_oamInfo_t *pOamInfo)
{
    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return -1;
    }	
	if(NULL == pOamInfo)
	{
		return -1;
	}
    *pOamInfo = localOamInfo[llidIdx];
	return 0;
}

static int epon_oam_remoteInfo_get(unsigned char llidIdx, oam_oamInfo_t *pOamInfo)
{
    /* Parameter check */
    if(llidIdx >= EPON_OAM_SUPPORT_LLID_NUM)
    {
        return -1;
    }
    if(NULL == pOamInfo)
    {
        return -1;
    }
    *pOamInfo = remoteOamInfo[llidIdx];
    return 0;
}

static int epon_oam_stdInfo_gen(
    unsigned char llidIdx,
    unsigned char *pBuf,
    unsigned short bufLen,
    unsigned short *pPushedLen)
{
    oam_oamInfo_t localInfo, remoteInfo;

    memset(&localInfo, 0x0, sizeof(oam_oamInfo_t));
    memset(&remoteInfo, 0x0, sizeof(oam_oamInfo_t));

    epon_oam_localInfo_get(llidIdx, &localInfo);
    if(bufLen > EPON_INFO_OAMPDU_INFO_LEN)
    {
        /* Add local info oam */
        pBuf[0] = EPON_INFO_OAMPDU_TYPE_LOCAL;
        pBuf[1] = EPON_INFO_OAMPDU_INFO_LEN;
        pBuf[2] = localInfo.oamVer;
        pBuf[3] = ((unsigned char *)(&localInfo.revision))[0];
        pBuf[4] = ((unsigned char *)(&localInfo.revision))[1];
        pBuf[5] = localInfo.state;
        pBuf[6] = localInfo.oamConfig;
        pBuf[7] = ((unsigned char *)(&localInfo.oamPduConfig))[0];
        pBuf[8] = ((unsigned char *)(&localInfo.oamPduConfig))[1];
        pBuf[9] = localInfo.oui[0];
        pBuf[10] = localInfo.oui[1];
        pBuf[11] = localInfo.oui[2];
        pBuf[12] = localInfo.venderSpecInfo[0];
        pBuf[13] = localInfo.venderSpecInfo[1];
        pBuf[14] = localInfo.venderSpecInfo[2];
        pBuf[15] = localInfo.venderSpecInfo[3];

        pBuf += EPON_INFO_OAMPDU_INFO_LEN;
        *pPushedLen += EPON_INFO_OAMPDU_INFO_LEN;

        epon_oam_remoteInfo_get(llidIdx, &remoteInfo);
        if(bufLen > (EPON_INFO_OAMPDU_INFO_LEN * 2))
        {
            /* Add remote info oam */
            pBuf[0] = EPON_INFO_OAMPDU_TYPE_REMOTE;
            pBuf[1] = EPON_INFO_OAMPDU_INFO_LEN;
            pBuf[2] = remoteInfo.oamVer;
            pBuf[3] = ((unsigned char *)(&remoteInfo.revision))[0];
            pBuf[4] = ((unsigned char *)(&remoteInfo.revision))[1];
            pBuf[5] = remoteInfo.state;
            pBuf[6] = remoteInfo.oamConfig;
            pBuf[7] = ((unsigned char *)(&remoteInfo.oamPduConfig))[0];
            pBuf[8] = ((unsigned char *)(&remoteInfo.oamPduConfig))[1];
            pBuf[9] = remoteInfo.oui[0];
            pBuf[10] = remoteInfo.oui[1];
            pBuf[11] = remoteInfo.oui[2];
            pBuf[12] = remoteInfo.venderSpecInfo[0];
            pBuf[13] = remoteInfo.venderSpecInfo[1];
            pBuf[14] = remoteInfo.venderSpecInfo[2];
            pBuf[15] = remoteInfo.venderSpecInfo[3];
            pBuf += EPON_INFO_OAMPDU_INFO_LEN;
            *pPushedLen += EPON_INFO_OAMPDU_INFO_LEN;

			/* Add end type for reply OAMPDU */
   	 		pBuf[0] = EPON_INFO_OAMPDU_TYPE_END;
    		*pPushedLen += 1;
        }
    }
    return 0;
}

static int epon_oam_stdInfo_parser(
    oam_oamInfo_t *pOamInfo,
    unsigned char *pFrame,
    unsigned short length,
    unsigned short *pExtractLen)
{
    unsigned char *pParsePtr;

    /* Minimum length requirement check */
    if(EPON_INFO_OAMPDU_INFO_LEN > length)
    {
        /* Remain length is not enough for parsing
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }

    pParsePtr = pFrame + 1; /* Skip information type */
    if(EPON_INFO_OAMPDU_INFO_LEN != *pParsePtr)
    {
        /* Wrong length for local/remote info OAMPDU
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }

    pParsePtr += 1;
    pOamInfo->oamVer = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->revision))[0] = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->revision))[1] = *pParsePtr;
    pParsePtr += 1;
    pOamInfo->state = *pParsePtr;
    pParsePtr += 1;
    pOamInfo->oamConfig = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->oamPduConfig))[0] = *pParsePtr;
    pParsePtr += 1;
    ((unsigned char *)(&pOamInfo->oamPduConfig))[1] = *pParsePtr;
    pParsePtr += 1;
    memcpy(pOamInfo->oui, pParsePtr, EPON_OAM_OUI_LENGTH);
    pParsePtr += EPON_OAM_OUI_LENGTH;
    memcpy(pOamInfo->venderSpecInfo, pParsePtr, EPON_INFO_OAMPDU_VENDER_SPEC_LEN);
    pParsePtr += EPON_INFO_OAMPDU_VENDER_SPEC_LEN;
    *pExtractLen = (pParsePtr - pFrame);

    return 0;
}

static int epon_oam_orgSpecInfo_find(unsigned char *pOui)
{
	int i;
	for(i = 0; i < sizeof(infoOrgSpecDb)/sizeof(infoOrgSpecDb[0]);i++)
	{
        if((infoOrgSpecDb[i][0] == pOui[0]) &&
           (infoOrgSpecDb[i][1] == pOui[1]) &&
           (infoOrgSpecDb[i][2] == pOui[2]))
        {
            return 0;
        }
    }
    return -1;
}

/* return value: 0:support, -1: not support */
static int epon_oam_orgSpecInfo_parser(
    unsigned char *pFrame,
    unsigned short length,
    unsigned short *pExtractLen)
{
    int ret;
    unsigned char *pParsePtr;
    unsigned short cbExtractLen, orgSpecLen;
	unsigned char oui[EPON_OAM_OUI_LENGTH];
	
    /* 1. Parsing the length and OUI first
     * 2. Check if it is supported by our oam via OUI
     */

    /* Minimum length requirement check */
    if( EPON_INFO_OAMPDU_ORGSPEC_MIN_LEN > length)
    {
        /* Remain length is not enough for parsing
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }

    pParsePtr = pFrame + 1; /* Skip information type */
    orgSpecLen = *pParsePtr;
    if(length < orgSpecLen)
    {
        /* Remain length is not enough for parsing
         * Skip the TLV
         */
        *pExtractLen = length;
        return -1;
    }
	*pExtractLen = orgSpecLen;

    pParsePtr += 1; /* length */
    memcpy(oui, pParsePtr, EPON_OAM_OUI_LENGTH);
	
	ret = epon_oam_orgSpecInfo_find(oui);
	//printk("%s oui[%x%x%x] ret[%d]\n",__func__, oui[0],oui[1],oui[2], ret);
	return ret;
}

static int epon_oam_stdInfo_process(
	unsigned char llidIdx,
	unsigned char * data,
	unsigned short dataLen,
	int isRx)
{
    int ret = 0; /* ret vlaue 0: a standard oam info; -1:parse error; -2: contains more info content */
    unsigned char isEnd;
    unsigned char *pProcPtr;
    unsigned short remainLen, extractLen;
	oam_oamInfo_t oamInfo;
	
    isEnd = 0;
    pProcPtr = data;
    remainLen = dataLen;

    memset((unsigned char *) &oamInfo, 0x0, sizeof(oam_oamInfo_t));
    
    if(llidIdx>=EPON_OAM_SUPPORT_LLID_NUM)
        return -1;

    while((remainLen >= 1) && (1 != isEnd) && (0 == ret))
    {
        switch(*pProcPtr)
        {
        case EPON_INFO_OAMPDU_TYPE_END:
            /* OAM PDU ended */
            isEnd = 1;
            remainLen --;
            pProcPtr ++;
            break;
        case EPON_INFO_OAMPDU_TYPE_LOCAL:
            /* Receive Remote device's "LOCAL" TLV */
			if(isRx)
			{
	            ret = epon_oam_stdInfo_parser(&remoteOamInfo[llidIdx], pProcPtr, remainLen, &extractLen);
	            if(0 != ret)
	                break;
			}
			else
			{
				extractLen = EPON_INFO_OAMPDU_INFO_LEN;
			}
            remainLen -= extractLen;
            pProcPtr += extractLen;
            break;
        case EPON_INFO_OAMPDU_TYPE_REMOTE:
            /* Receive Remote device's "REMOTE" TLV */
			if(isRx)
			{
	            ret = epon_oam_stdInfo_parser(&oamInfo, pProcPtr, remainLen, &extractLen);
	            if(0 != ret)
	                break;

				memcpy(&localOamInfo[llidIdx], &oamInfo, sizeof(oam_oamInfo_t));
			}
			else
			{
				extractLen = EPON_INFO_OAMPDU_INFO_LEN;
			}
            remainLen -= extractLen;
            pProcPtr += extractLen;
            break;
        case EPON_INFO_OAMPDU_TYPE_ORG_SPEC:
			ret = epon_oam_orgSpecInfo_parser(pProcPtr, remainLen, &extractLen);
			if(ret == 0)
			{	/* supported org info, not reply in kernel */				
				ret = -2;
			}
			else
			{	/* unsupported org info, cotinue to parse */
				remainLen -= extractLen;
            	pProcPtr += extractLen;
				ret = 0;
			}
			break;
        case EPON_INFO_OAMPDU_TYPE_RESERVED:
        default:
			ret = -2; /* contains more info content */
            break;
        }
    }
	return ret;
}

void epon_oam_pkt_tx(unsigned short dataLen, unsigned char *data);
void epon_oam_pkt_tx_for_keeplive(unsigned short dataLen, unsigned char *data)
{
	if(oam_info_keeplive_enable && (dataLen > MAX_OAM_EVENT_SIZE))
	{
		unsigned char llidIdx;
		llidIdx = *((unsigned char *) (data + (dataLen - sizeof(unsigned char))));
		if(llidIdx < EPON_OAM_SUPPORT_LLID_NUM )
		{
			if((data[15] == EPON_OAM_INFO_COMPLETE_FLAG1) && (data[16] == EPON_OAM_INFO_COMPLETE_FLAG2) && (data[17] == EPON_OAMPDU_CODE_INFO))  
			{
				//send a oam discover complete info packet	
				if(remoteOamInfo[llidIdx].valid == 0)
				{
					memcpy(oamConfig[llidIdx].macAddr, &data[6] , 6);
					remoteOamInfo[llidIdx].valid = 1;
					printk("%s llid[%d] valid mac[0x%x%x:%x%x:%x%x]\n",__func__,llidIdx,
						oamConfig[llidIdx].macAddr[0],oamConfig[llidIdx].macAddr[1],
						oamConfig[llidIdx].macAddr[2],oamConfig[llidIdx].macAddr[3],
						oamConfig[llidIdx].macAddr[4],oamConfig[llidIdx].macAddr[5]);
				}
				else
				{
					if(0 == epon_oam_stdInfo_process(llidIdx, data+EPON_INFO_HEADER_LEN, (dataLen-sizeof(unsigned char)-EPON_INFO_HEADER_LEN), 0))
					{
						//it is a standard oam info packet, not send it
						//printk("[%s:%d] llid[%d] len[%d] drop\n", __func__, __LINE__,llidIdx,dataLen);
						return;
					}
				}
			}
			else if(((data[16]&0xff) != EPON_OAM_INFO_COMPLETE_FLAG2) && (data[17] == EPON_OAMPDU_CODE_INFO))
			{	
				//send a oam discover not complete info packet
				remoteOamInfo[llidIdx].valid = 0;
				printk("%s llid[%d] unvalid\n",__func__,llidIdx);
			}
		}
	}
	epon_oam_pkt_tx(dataLen,data);
}

static void epon_oam_reset_info(void)
{
	/* Init local/remote info database */
    memset((unsigned char *) &localOamInfo, 0x0, sizeof(oam_oamInfo_t) * EPON_OAM_SUPPORT_LLID_NUM);
    memset((unsigned char *) &remoteOamInfo, 0x0, sizeof(oam_oamInfo_t) * EPON_OAM_SUPPORT_LLID_NUM);
}

int epon_oam_keeplive_write_proc(struct file *file, const char __user *buffer, size_t len, loff_t *ppos)
{	
	unsigned char tmpBuf[16] = {0};
	int count = (len > 15) ? 15 : len;

	if (buffer && !copy_from_user(tmpBuf, buffer, count))
	{
		oam_info_keeplive_enable = simple_strtoul(tmpBuf, NULL, 16);
		switch(oam_info_keeplive_enable) 
		{
			case 0:
				epon_oam_reset_info();
				printk("Turn off kernel oam keeplive mode\n");
				break;
			case 1:
				printk("Turn on kernel oam keeplive mode\n");
				break;
		}
		return count;
	}	
	return -EFAULT;
}

static int epon_oam_keeplive_read_proc(struct seq_file *seq, void *v)
{
	int i;
	seq_printf(seq, "Kernel Oam Keeplive Mode:%s\n", (oam_info_keeplive_enable != 0)?"ON":"OFF");
	if(oam_info_keeplive_enable)
	{
		for(i=0; i<EPON_OAM_SUPPORT_LLID_NUM; i++)
		{
			seq_printf(seq, "%d:%lld\n", i, oamrxtime[i]);
		}
	}
	return 0;
}

static int proc_oam_keeplive_open(struct inode *inode, struct file *file)
{
	return single_open(file, epon_oam_keeplive_read_proc, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops oamKeeplive_ops = {
        .proc_open      = proc_oam_keeplive_open,
        .proc_read      = seq_read,
        .proc_write     = epon_oam_keeplive_write_proc,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations oamKeeplive_ops = {
        .owner          = THIS_MODULE,
        .open           = proc_oam_keeplive_open,
        .read           = seq_read,
        .write          = epon_oam_keeplive_write_proc,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

static void epon_oam_keeplive_init(void)
{
	struct proc_dir_entry *entry=NULL;
	entry = proc_create_data("oamKeeplive", 0644, NULL, &oamKeeplive_ops,NULL);
	if (entry == NULL)
	{
		printk("%s create proc oamKeeplive fail!\n",__func__);	
		remove_proc_entry("oamKeeplive", NULL);
		return;
	}

	oam_info_keeplive_enable = 0; /* disable kepplive by default */

	epon_oam_reset_info();
}
#endif

int epon_oam_handle_event(unsigned short dataLen, unsigned char *data)
{
	rtk_epon_oam_event_t event;
	unsigned char eventKey[] = {
        0x42, 0x15, /* Magic key */
    };

	if(memcmp(data,eventKey,sizeof(eventKey)))
		return -1;

	memcpy(&event, data+sizeof(eventKey), sizeof(rtk_epon_oam_event_t));
	printk("[%s] eventType[%d]\n", __func__, event.eventType);
	switch(event.eventType)
	{
		case RTK_EPON_OAM_STATE:
		{
			rtk_epon_fsm_status_t state = event.eventData[0];
			if((state <= RTK_EPON_STATE_OAM_AUTH_SUCC) && (state > RTK_EPON_STATE_UNKNOWN))
				queue_broadcast(MSG_TYPE_EPON_STATE, state, 0, ENABLED);
		}
		break;
		case RTK_EPON_OAM_EVENT:
		{
			unsigned int subType = event.eventData[0];
			if(subType == 0)
			{	/* tr069 wan vlan config */
				unsigned int config;
				memcpy(&config, event.eventData+1, sizeof(config));
				queue_broadcast(MSG_TYPE_EPON_EVENT, subType, config, ENABLED);
			}
		}
		break;
	}
	return 0;
}

void epon_oam_pkt_tx(unsigned short dataLen, unsigned char *data)
{
    unsigned char llidIdx;
    uint32 chipId, rev, subtype;
    struct tx_info txInfo;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);
    int ret;

	if(dataLen <= MAX_OAM_EVENT_SIZE)
	{
#if (!defined(CONFIG_RTK_L34_ENABLE) || defined(CONFIG_HYBRID_MODE)) && (defined(CONFIG_RTL9607C_SERIES) || defined(CONFIG_RTL9602C_SERIES))
		if(dataLen < 10)
		{
			/* Check Magic */
			if(data[0] == 0x42)
			{
				unsigned char action = data[1];
				unsigned char port = data[2];
				//printk("%s %d: action[%d] port[%d] len[%d]\n", __FUNCTION__, __LINE__, action, port, dataLen);
				epon_maintain_vlan_table(action,port);				
			}	
			return;
		}
#endif
		epon_oam_handle_event(dataLen,data);
		return;
	}
	
    llidIdx = *((unsigned char *) (data + (dataLen - sizeof(unsigned char))));

#ifdef CONFIG_COMMON_RT_API
    if ((ret = rt_epon_oam_tx(dataLen - sizeof(unsigned char), data, llidIdx)) != RT_ERR_OK)
    {
        printk("%s:%d tx OAM failed %d!\n", __FILE__, __LINE__,ret);
    }
#else
    memset(&txInfo,0x0,sizeof(struct tx_info));

    /*keep format, switch do not modify packet format ex:vlan tag*/
    GMAC_TXINFO_KEEP(&txInfo)=1;
    GMAC_TXINFO_CPUTAG(&txInfo)=1;
    GMAC_TXINFO_TX_PMASK(&txInfo)=rtl960x_pon_port_mask;

    GMAC_TXINFO_ASPRI(&txInfo)=1;
    GMAC_TXINFO_CPUTAG_PRI(&txInfo)=7;

    /* Specified the output queue according to the LLId index */
    GMAC_TXINFO_CPUTAG_PSEL(&txInfo)=1;

    ret = rtk_switch_version_get(&chipId, &rev, &subtype);
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
    }     
    switch(chipId)
    {    
        case RTL9601B_CHIP_ID:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo)= 0x08 + (llidIdx * 16);
            break;

        case RTL9602C_CHIP_ID:
        case RTL9607C_CHIP_ID:
            switch(llidIdx)
            {
                case 0:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 8;
                    break;
                case 1:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 10;
                    break;
                case 2:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 24;
                    break;
                case 3:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 26;
                    break;
                case 4:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 40;
                    break;
                case 5:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 42;
                    break;
                case 6:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 56;
                    break;
                case 7:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 58;
                    break;
                        
            }
            break;
        case RTL9603CVD_CHIP_ID:
            switch(llidIdx)
            {
                case 0:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 8;
                    break;
                case 1:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 24;
                    break;
                case 2:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 40;
                    break;
                case 3:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 56;
                    break;
                case 4:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 72;
                    break;
                case 5:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 88;
                    break;
                case 6:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 104;
                    break;
                case 7:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 120;
                    break;
                        
            }
            break;
        case APOLLOMP_CHIP_ID:
        default:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 7;
           break;
        
    } 
       
    re8686_tx_with_Info(data, dataLen - sizeof(unsigned char), &txInfo);
#endif
}

void epon_oam_dyingGasp_tx(unsigned short dataLen, unsigned char *data)
{
    unsigned char llidIdx;
    uint32 chipId, rev, subtype;
    struct tx_info txInfo;
    int ret;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

    llidIdx = *((unsigned char *) (data + (dataLen - sizeof(unsigned char))));

#ifdef CONFIG_COMMON_RT_API
    if ((ret = rt_epon_dyinggasp_set(dataLen - sizeof(unsigned char), data)) != RT_ERR_OK)
    {
        printk("%s:%d tx OAM failed %d!\n", __FILE__, __LINE__,ret);
    }
#else
    memset(&txInfo,0x0,sizeof(struct tx_info));
    
    
    GMAC_TXINFO_KEEP(&txInfo) = 1;
    GMAC_TXINFO_CPUTAG_PSEL(&txInfo) = 1;
    GMAC_TXINFO_CPUTAG(&txInfo) = 1;

    ret=rtk_switch_version_get(&chipId, &rev, &subtype);
 
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
    } 
 
    switch(chipId)
    {    
        case RTL9601B_CHIP_ID:
        case RTL9602C_CHIP_ID:
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 0x1f;
            break;
        case APOLLOMP_CHIP_ID:
        default:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 0x7f;
            break;
    }   
    
    GMAC_TXINFO_TX_PMASK(&txInfo) = rtl960x_pon_port_mask;
    re8686_tx_with_Info(data, dataLen - sizeof(unsigned char), &txInfo);
#endif
}

#ifdef CONFIG_COMMON_RT_API
uint32 rt_oam_rx(uint32 msgLen,uint8 *pMsg, uint8 llidIdx)
{
    int ret;
    uint8 *oam_ptr = NULL;
    uint32_t oam_len = msgLen;

    if(llidIdx == 0x08 || llidIdx == 0x0A || llidIdx == 0x0B)
        llidIdx = 0;

    oam_ptr=(uint8 *)kmalloc((msgLen+8)*sizeof(uint8),GFP_ATOMIC);
    if (!oam_ptr)
    {
        printk("kmalloc (%d) fail\n", PR_USER_UID_EPONOAM);
        return 0;
    }

    memcpy(oam_ptr,pMsg,msgLen);
    *((uint8 *)(oam_ptr + oam_len)) = llidIdx;
    
    ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, oam_len +  sizeof(llidIdx), oam_ptr);
    if(ret)
    {
        printk("send to user app (%d) fail (%d)\n", PR_USER_UID_EPONOAM, ret);
    }

    if(oam_ptr != NULL)
        kfree(oam_ptr);

    return 0;
}
#endif

int epon_oam_pkt_rx(
    struct re_private *cp,
    struct sk_buff *skb,
    struct rx_info *pRxInfo)
{
    int ret;
    unsigned char chLlidIdx; 
    
#ifdef CONFIG_EPON_LOS_RECOVER
    uint32 chipId, rev, subtype;
    uint32 data;
#endif    
    
    //printk("\n epon_oam_pkt_rx 0x%x 0x%x  0x%x\n",skb->data[12],skb->data[13],skb->data[14]);
    
    /* Filter only spa = PON and OAM frames */
    if((skb->data[12] == 0x88) && (skb->data[13] == 0x09) && (skb->data[14] == 0x03))
    {
        /* concate the LLID index to the end of the packet buffer */
        chLlidIdx = GMAC_RXINFO_PON_STREAM_ID(pRxInfo);
        
        chLlidIdx = chLlidIdx & 0xf;
        //printk("\n chLlidIdx:%d \n",chLlidIdx);
		/* llid index of boardcast oam packet is 0x08, 0x0A(10) or 0x0B(11), it must be changed to llid 0 */
		if(chLlidIdx == 0x08 || chLlidIdx == 0x0A || chLlidIdx == 0x0B)
			chLlidIdx = 0;
		
        skb_put(skb, sizeof(chLlidIdx));
        *((unsigned char *)(skb->data + (skb->len - sizeof(chLlidIdx)))) = chLlidIdx;
        ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, skb->len, skb->data);
        if(ret)
        {
            printk("send to user app (%d) fail (%d)\n", PR_USER_UID_EPONOAM, ret);
        }

#ifdef SUPPORT_OAM_KEEPLIVE
		if(oam_info_keeplive_enable && (chLlidIdx < EPON_OAM_SUPPORT_LLID_NUM))
		{
			if((skb->data[15] == EPON_OAM_INFO_COMPLETE_FLAG1) && (skb->data[16] == EPON_OAM_INFO_COMPLETE_FLAG2) && (skb->data[17] == EPON_OAMPDU_CODE_INFO)
				&& remoteOamInfo[chLlidIdx].valid )
			{	//receive a oam discover complete info packet
				unsigned char * data;
				unsigned short length;

				data = &skb->data[EPON_INFO_HEADER_LEN];
				length = skb->len - sizeof(chLlidIdx) - (data - skb->data);
				
				if(0 == epon_oam_stdInfo_process(chLlidIdx, data, length, 1))
				{
					//it is a standard oam info packet, send it directly				
					memset(oam_info_reply, 0, MAX_OAM_INFO_SIZE);
					epon_oam_info_stdHdr_gen(chLlidIdx, oam_info_reply);
					data = &oam_info_reply[EPON_INFO_HEADER_LEN];
					length = EPON_INFO_HEADER_LEN;
					
					epon_oam_stdInfo_gen(chLlidIdx, data, MAX_OAM_INFO_SIZE - EPON_INFO_HEADER_LEN, &length);
                    
                    if(length>=(MAX_OAM_INFO_SIZE-1))
                    {
                        printk("data length exceed!!\n");
                        goto oam_rx_exit;      
                    }
                    
					oam_info_reply[length++] = ((unsigned char) chLlidIdx);

					//printk("%s llid[%d] send len[%d]\n",__func__,chLlidIdx, length);
					epon_oam_pkt_tx(length, oam_info_reply);
					oamrxtime[chLlidIdx] = ktime_to_ms(ktime_get_boottime());
					
				}
			}
		}
#endif

#ifdef CONFIG_EPON_LOS_RECOVER
        ret=rtk_switch_version_get(&chipId, &rev, &subtype);
        /*enable stop sync local time when start rx oam packet*/    
        switch(chipId)
        {    
            case RTL9601B_CHIP_ID:
            #if defined(CONFIG_SDK_RTL9601B)
                if ((ret = reg_array_field_read(RTL9601B_RSVD_EPON_CTRLr, REG_ARRAY_INDEX_NONE, 0, RTL9601B_RSVD_MEMf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                    goto oam_rx_exit;  
                } 
                data = data | (1<<5);
                /*data = 0x55555575; enable stop sync local time set bit 5 to 1*/
                if ((ret = reg_array_field_write(RTL9601B_RSVD_EPON_CTRLr, REG_ARRAY_INDEX_NONE, 0, RTL9601B_RSVD_MEMf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                    goto oam_rx_exit;  
                }
            #endif
                break;
            case RTL9602C_CHIP_ID:
            #if defined(CONFIG_SDK_RTL9602C)
            data = 1;
            if ((ret = reg_field_write(RTL9602C_EPON_ASIC_OPTI1r,RTL9602C_STOP_SYNCf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            } 
            #endif
                break;
            case RTL9607C_CHIP_ID:
            #if defined(CONFIG_SDK_RTL9607C)
            data = 1;
            if ((ret = reg_field_write(RTL9607C_EPON_ASIC_OPTI1r,RTL9607C_STOP_SYNCf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            } 
            #endif
                break;
            case RTL9603CVD_CHIP_ID:
            #if defined(CONFIG_SDK_RTL9603CVD)
            data = 1;
            if ((ret = reg_field_write(RTL9603CVD_EPON_ASIC_OPTI1r,RTL9603CVD_STOP_SYNCf,&data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                return ret;
            } 
            #endif
                break;

            case APOLLOMP_CHIP_ID:
            default:
                break;
        } 
#endif /*CONFIG_EPON_LOS_RECOVER*/
        /* Left the skb to be free by caller */
oam_rx_exit:
        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;
}

#if defined(CONFIG_RTK_OAM_V2)
int epon_muticast_pkt_rx(
    struct re_private *cp,
    struct sk_buff *skb,
    struct rx_info *pRxInfo)
{
    int ret;

    /* Filter only spa = All Ports and Muticast frames */
    if((skb->data[0] == 0x01) && (skb->data[1] == 0x00) && (skb->data[2] == 0x5e))
    {
    	skb->vlan_tci = GMAC_RXINFO_CTAGVA(pRxInfo) ? VTAG2VLANTCI(GMAC_RXINFO_CVLAN_TAG(pRxInfo)) : 0;
    	skb_push_qtag(skb, skb->vlan_tci & 0xfff /* VLAN_VID_MASK */, 0);
    	skb_push(skb, 3);    					
		skb->data[0] = GMAC_RXINFO_SRC_PORT_NUM(pRxInfo)+1;
		skb->data[1] = ((skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) >> 8) & 0xF;
		skb->data[2] = (skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) & 0xFF;
		//printk("%s %d 0x%x 0x%x 0x%x %d\n", __FUNCTION__, __LINE__, skb->data[0], skb->data[1], skb->data[2], skb->len);
        ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, skb->len, skb->data);
        if(ret)
        {
            //printk("send to user app (%d) for muticast frames fail (%d)\n", PR_USER_UID_EPONOAM, ret);
        }
        /* Left the skb to be free by caller */
        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;
}
#elif defined(CONFIG_RTK_OAM_V1)
typedef struct ipv4hdr_s 
{
#if !defined(CONFIG_CPU_BIG_ENDIAN)
    uint8   ihl:4,
            version:4;
#else
    uint8   version:4,
            ihl:4;
#endif            
	uint8	typeOfService;			
	uint16	length;			/* total length */
	uint16	identification;	/* identification */
	uint16	offset;			
	uint8	ttl;			/* time to live */
	uint8	protocol;			
	uint16	checksum;			
	uint32 	sip;
	uint32 	dip;
}ipv4hdr_t;

typedef struct ipv6hdr_s
{
	uint32  vtf;            /*version(4bits),  traffic class(8bits), and flow label(20bits)*/
	uint16	payloadLenth;	/* payload length */
	uint8	nextHeader;		/* next header */
	uint8	hopLimit;		/* hop limit*/
	uint32  sip[4];	    	/*source address*/
	uint32  dip[4];			/* destination address */
} ipv6hdr_t;

typedef struct igmpv3_record_s
{
	uint8	type;				/* Record Type */
	uint8	auxLen;			    /* auxiliary data length, in uints of 32 bit words*/
	uint16	numOfSrc;			/* number of sources */
	uint32	groupAddr;			/* group address being reported */
	uint32	srcList[0];			/* first entry of source list */	
} igmpv3_record_t;

typedef struct igmpv3_report_s
{
	uint8	type;				/* Report Type */
	uint8  reserved1;             
	uint16 checkSum;			/*IGMP checksum*/
	uint16 reserved2;
	uint16	numOfRecords;		/* number of Group records */
	igmpv3_record_t recordList[0];  /*first entry of group record list*/
} igmpv3_report_t;


#ifndef IGMP_PROTOCOL
#define IGMP_PROTOCOL     2
#endif

#define IGMPV3_TYPE_MEMBERSHIP_REPORT 0x22

static int epon_is_pppoe_igmp_packet(uint8 * data)
{
	uint8 *ptr;
	ipv4hdr_t *iph;
	
	ptr = data;
	ptr += 12;

	if ((ptr[0] == 0x88) && (ptr[1] == 0xA8))
	{
		ptr += 4;
	}
	if ((ptr[0] == 0x81) && (ptr[1] == 0x00))
	{
		ptr += 4;
	}
	if((ptr[0] == 0x88) && (ptr[1] == 0x64)) /* pppoe session */
	{
		ptr += 2 + 6; /* PPPoE header: 6 bytes */
		if((ptr[0] == 0x00) && (ptr[1] == 0x21)) /* ipv4 */
		{
			ptr += 2;
		}
		else if(ptr[0] == 0x21) /* compressed ipv4 */
		{
			ptr += 1;
		}
		else
			return 0;
	}
	else
		return 0;

	iph = (ipv4hdr_t *)(ptr);
	if(iph->protocol == IGMP_PROTOCOL)
	{
		return 1;
	}
	return 0;
}

static int epon_is_igmp_packet(uint8 * data)
{
	uint8 *ptr;
	ipv4hdr_t *iph;
	
	ptr = data;
	ptr += 12;

	if ((ptr[0] == 0x88) && (ptr[1] == 0xA8))
	{
		ptr += 4;
	}
	if ((ptr[0] == 0x81) && (ptr[1] == 0x00))
	{
		ptr += 4;
	}
	if ((ptr[0] == 0x08) && (ptr[1] == 0x00)) /* ipv4 */
	{
		ptr += 2;
	}
	else if((ptr[0] == 0x88) && (ptr[1] == 0x64)) /* pppoe session */
	{
		ptr += 2 + 6; /* PPPoE header: 6 bytes */
		if((ptr[0] == 0x00) && (ptr[1] == 0x21)) /* ipv4 */
		{
			ptr += 2;
		}
		else if(ptr[0] == 0x21) /* compressed ipv4 */
		{
			ptr += 1;
		}
		else
			return 0;
	}
	else
	{
		return 0;
	}

	iph = (ipv4hdr_t *)(ptr);
	if(iph->protocol == IGMP_PROTOCOL)
	{
		igmpv3_report_t *report;

		/*239.255.255.250 igmpv2 packet must return to protocol stack*/
		if(ntohl(iph->dip) == 0xEFFFFFFA)
		{
			return 0;
		}

		report = (igmpv3_report_t*)((char*)iph + (iph->ihl<<2));
		if(report->type == IGMPV3_TYPE_MEMBERSHIP_REPORT)
		{
			igmpv3_record_t *grec, *ptr;
			uint32 size1, size2;
			int i;
			grec = report->recordList;
			size1 = sizeof(igmpv3_record_t);
			
			for (i = 0; i < report->numOfRecords; i++)
			{
				//if we get not 239.255.255.250 report, send to oam
				if(ntohl(grec->groupAddr) != 0xEFFFFFFA)
				{
					return 1;
				}
				size2 = grec->numOfSrc * 4;
				ptr = (igmpv3_record_t*)((char*) grec + size1 + size2);
				grec = ptr;
			}
			return 0;
		}
		return 1;
	}
	return 0;
}

#define IPV6_HEADER_LENGTH 40
#define HOP_BY_HOP_OPTIONS_HEADER 	 0
#define ROUTING_HEADER 				43
#define FRAGMENT_HEADER 			44
#define DESTINATION_OPTION_HEADER 	60
#define NO_NEXT_HEADER 				59
#define ICMPV6_PROTOCOL    58

#define MLD_TYPE_MEMBERSHIP_QUERY      130
#define MLD_TYPE_MEMBERSHIP_REPORT     131  
#define MLD_TYPE_MEMBERSHIP_DONE       132
#define MLDV2_TYPE_MEMBERSHIP_REPORT   143 

static int epon_is_pppoe_ipv6(uint8 * data)
{
	uint8 *ptr;
	
	ptr = data;
	ptr += 12;

	if ((ptr[0] == 0x88) && (ptr[1] == 0xA8))
	{
		ptr += 4;
	}
	if ((ptr[0] == 0x81) && (ptr[1] == 0x00))
	{
		ptr += 4;
	}
	if((ptr[0] == 0x88) && (ptr[1] == 0x64)) /* pppoe session */
	{		
		ptr += 2 + 6; /* PPPoE header: 6 bytes */
		if((ptr[0] == 0x00) && (ptr[1] == 0x57)) /* ipv6 */
		{
			ptr += 2;
			return 1;
		}
		else if(ptr[0] == 0x57) /* compressed ipv6 */
		{
			ptr += 1;
			return 1;
		}
	}
	return 0;
}

static int epon_is_mld_packet(uint8 * data)
{
	uint8 *ptr;
	ipv6hdr_t *iph6;
	uint8  nextHeader=0;
	uint8 * playloadPtr;
	uint16 extensionHdrLen = 0;
	int ret = 0;
	
	ptr = data;
	ptr += 12;

	if ((ptr[0] == 0x88) && (ptr[1] == 0xA8))
	{
		ptr += 4;
	}
	if ((ptr[0] == 0x81) && (ptr[1] == 0x00))
	{
		ptr += 4;
	}
	if ((ptr[0] == 0x86) && (ptr[1] == 0xDD)) /* ipv6 */
	{
		ptr += 2;
	}
	else if((ptr[0] == 0x88) && (ptr[1] == 0x64)) /* pppoe session */
	{
		ptr += 2 + 6; /* PPPoE header: 6 bytes */
		if((ptr[0] == 0x00) && (ptr[1] == 0x57)) /* ipv6 */
		{
			ptr += 2;
		}
		else if(ptr[0] == 0x57) /* compressed ipv6 */
		{
			ptr += 1;
		}
		else
			return 0;
	}
	else
	{
		return 0;
	}

	iph6 = (ipv6hdr_t *)(ptr);
	nextHeader = iph6->nextHeader;
	ptr = ptr + IPV6_HEADER_LENGTH;
	playloadPtr = ptr;

	while(((ptr - playloadPtr) < ntohs(iph6->payloadLenth)) 
		&& (NO_NEXT_HEADER != nextHeader))
	{
		switch(nextHeader) 
		{
			case HOP_BY_HOP_OPTIONS_HEADER:
			case ROUTING_HEADER:
			case DESTINATION_OPTION_HEADER:
				nextHeader=ptr[0];
				extensionHdrLen=((uint16)(ptr[1])+1)*8;
				ptr=ptr+extensionHdrLen;
				break;
			case FRAGMENT_HEADER:
				nextHeader=ptr[0];
				ptr=ptr+8;
				break;
			case ICMPV6_PROTOCOL:
				nextHeader=NO_NEXT_HEADER;
				/* check icmpv6 type in order to process igmp packet only */
				if((ptr[0] == MLD_TYPE_MEMBERSHIP_QUERY) || (ptr[0] == MLD_TYPE_MEMBERSHIP_REPORT) ||
					(ptr[0] == MLD_TYPE_MEMBERSHIP_DONE) || (ptr[0] == MLDV2_TYPE_MEMBERSHIP_REPORT))
				{
					ret = 1;
				}
				break;
			default:
				nextHeader=NO_NEXT_HEADER;
				break;
		}
	}
	return ret;
}

#define SUPPORT_IGMP_RATE_LIMIT
#ifdef SUPPORT_IGMP_RATE_LIMIT	
#define IGMP_RATE_LIMIT_CLEAR_INTERVAL 	(CONFIG_HZ>>4) 	/* interval time:(1/16)second */
static struct timer_list igmpRateLimitTimer;
static int igmpByteCount = 0;
static unsigned int igmpRateLimit = 0; /* unit:kbps, value 0: disable rate limit */

static void epon_igmp_rate_limit_timer(unsigned long task_priv)
{
	//clear the accumulated pkt counter
	igmpByteCount = 0;
	if(igmpRateLimit)
		mod_timer(&igmpRateLimitTimer, jiffies + IGMP_RATE_LIMIT_CLEAR_INTERVAL); 
}

static int epon_igmp_rate_limit_write_proc(struct file *file, const char __user *buffer, size_t len, loff_t *ppos)
{	
	unsigned char tmpBuf[16] = {0};
	int count = (len > 15) ? 15 : len;

	if (buffer && !copy_from_user(tmpBuf, buffer, count))
	{
		igmpRateLimit = simple_strtoul(tmpBuf, NULL, 10);
		if(igmpRateLimit == 0)
		{
			printk("Disable epon igmp rate limit\n");			
		}
		else
		{
			mod_timer(&igmpRateLimitTimer, jiffies + IGMP_RATE_LIMIT_CLEAR_INTERVAL);
			printk("set epon igmp rate limit to %d kbps\n", igmpRateLimit);
		}
		return count;
	}	
	return -EFAULT;
}

static int epon_igmp_rate_limit_read_proc(struct seq_file *seq, void *v)
{
	seq_printf(seq, "epon igmp rate limit:%d kpbs (0 is disable)\n", igmpRateLimit);
	return 0;
}

static int proc_igmp_rate_limit_open(struct inode *inode, struct file *file)
{
	return single_open(file, epon_igmp_rate_limit_read_proc, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops oamIgmpRateLimit_ops = {
        .proc_open      = proc_igmp_rate_limit_open,
        .proc_read      = seq_read,
        .proc_write     = epon_igmp_rate_limit_write_proc,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations oamIgmpRateLimit_ops = {
        .owner          = THIS_MODULE,
        .open           = proc_igmp_rate_limit_open,
        .read           = seq_read,
        .write          = epon_igmp_rate_limit_write_proc,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

static void epon_igmp_rate_limit_init(void)
{
	struct proc_dir_entry *entry=NULL;
	entry = proc_create_data("oamIgmpRateLimit", 0644, NULL, &oamIgmpRateLimit_ops,NULL);
	if (entry == NULL)
	{
		printk("%s create proc oamIgmpRateLimit fail!\n",__func__);	
		remove_proc_entry("oamIgmpRateLimit", NULL);
		return;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
	init_timer(&igmpRateLimitTimer);
	igmpRateLimitTimer.function = epon_igmp_rate_limit_timer;
#else
    timer_setup(&igmpRateLimitTimer, epon_igmp_rate_limit_timer, 0);
#endif
	
	if(igmpRateLimit)
		mod_timer(&igmpRateLimitTimer, jiffies + IGMP_RATE_LIMIT_CLEAR_INTERVAL);
}

/* return value 1: over rate limit, should drop, 0: can process */
int epon_igmp_rate_limit_drop(int len)
{
	if(igmpRateLimit)
	{
		igmpByteCount += len;
		if((igmpByteCount*8)/*flow bits in time period*/ > (igmpRateLimit * 1024 * IGMP_RATE_LIMIT_CLEAR_INTERVAL)/*rate limit bits in time period*/){		
			printk("[Drop] igmp packet rate higher than rateLimit[%d kbps]\n",igmpRateLimit);
			return 1;
		}
	}
	return 0;
}
#endif
int epon_muticast_pkt_rx(
    struct re_private *cp,
    struct sk_buff *skb,
    struct rx_info *pRxInfo)
{
#if defined(CONFIG_HYBRID_MODE)
	if (epon_devmode)//hgu
		return RE8670_RX_CONTINUE;
#endif

    /* Filter only spa = All Ports and Muticast frames */
    if(((skb->data[0] == 0x01) && (skb->data[1] == 0x00) && (skb->data[2] == 0x5e)) ||
		epon_is_pppoe_igmp_packet(skb->data))
    {
		if(0 == epon_is_igmp_packet(skb->data))
		{
			return RE8670_RX_CONTINUE;
		}

#ifdef SUPPORT_IGMP_RATE_LIMIT
		if(epon_igmp_rate_limit_drop(skb->len))
			return RE8670_RX_STOP;
#endif

		skb->vlan_tci = (pRxInfo->opts2.bit.ctagva) ? VTAG2VLANTCI(pRxInfo->opts2.bit.cvlan_tag) : 0;
    	skb_push_qtag(skb, skb->vlan_tci & 0xfff /* VLAN_VID_MASK */, 0);
    	skb_push(skb, 4); /* make sure data 4 bytes align */   					
		skb->data[0] = pRxInfo->opts3.bit.src_port_num+1;
		skb->data[1] = ((skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) >> 8) & 0xF;
		skb->data[2] = (skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) & 0xFF;
		//printk("%s %d 0x%x 0x%x 0x%x %d\n", __FUNCTION__, __LINE__, skb->data[0], skb->data[1], skb->data[2], skb->len);
		
		pkt_redirect_kernelApp_sendPkt(PR_USER_UID_IGMPMLD, 1, skb->len, skb->data);     
        /* Left the skb to be free by caller */
        return RE8670_RX_STOP;
    }
	else if(((skb->data[0] == 0x33) && (skb->data[1] == 0x33) && (skb->data[2] != 0xff)) ||
		epon_is_pppoe_ipv6(skb->data))
	{
		if(0 == epon_is_mld_packet(skb->data))
		{
			return RE8670_RX_CONTINUE;
		}
#ifdef SUPPORT_IGMP_RATE_LIMIT
		if(epon_igmp_rate_limit_drop(skb->len))
			return RE8670_RX_STOP;
#endif
		skb->vlan_tci = (pRxInfo->opts2.bit.ctagva) ? VTAG2VLANTCI(pRxInfo->opts2.bit.cvlan_tag) : 0;
    	skb_push_qtag(skb, skb->vlan_tci & 0xfff /* VLAN_VID_MASK */, 0);
    	skb_push(skb, 4); /* make sure data 4 bytes align */   					
		skb->data[0] = pRxInfo->opts3.bit.src_port_num+1;
		skb->data[1] = ((skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) >> 8) & 0xF;
		skb->data[2] = (skb->vlan_tci & 0xfff /* VLAN_VID_MASK */) & 0xFF;
		//printk("%s %d 0x%x 0x%x 0x%x %d\n", __FUNCTION__, __LINE__, skb->data[0], skb->data[1], skb->data[2], skb->len);
		
		pkt_redirect_kernelApp_sendPkt(PR_USER_UID_IGMPMLD, 1, skb->len, skb->data);     
        /* Left the skb to be free by caller */
        return RE8670_RX_STOP;
	}
    return RE8670_RX_CONTINUE;
}

void epon_muticast_pkt_tx(unsigned short dataLen, unsigned char *data)
{
	unsigned char llidIdx;
    uint32 chipId, rev, subtype;
    struct tx_info txInfo;
	uint32 fwdPortMask;
	int ret;
	
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

    llidIdx = *((unsigned char *) (data + (dataLen - 2)));
	fwdPortMask = *((uint8*)(data + (dataLen - 1)));

	//printk("%s %d fwd[0x%x] llidIdx[%d],len[%d]\n", __FUNCTION__, __LINE__, fwdPortMask, llidIdx,dataLen);
    memset(&txInfo,0x0,sizeof(struct tx_info));

    /*keep format, switch do not modify packet format ex:vlan tag*/
    GMAC_TXINFO_KEEP(&txInfo) = 1;

	/*disable switch l2 learning for this packet*/
    GMAC_TXINFO_DISLRN(&txInfo) = 1;
    GMAC_TXINFO_CPUTAG(&txInfo) = 1;

    GMAC_TXINFO_TX_PMASK(&txInfo) = fwdPortMask;

    GMAC_TXINFO_ASPRI(&txInfo) = 1;
    GMAC_TXINFO_CPUTAG_PRI(&txInfo) = 7;

    /* Specified the output queue according to the LLId index */
    GMAC_TXINFO_CPUTAG_PSEL(&txInfo) = 1;

    ret = rtk_switch_version_get(&chipId, &rev, &subtype);
                    
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
    }
    
    switch(chipId)
    {    
        case RTL9601B_CHIP_ID:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 0x08 + (llidIdx * 16);
            break;

        case RTL9602C_CHIP_ID:
            switch(llidIdx)
            {
                case 0:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 8;
                    break;
                case 1:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 10;
                    break;
                case 2:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 24;
                    break;
                case 3:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 26;
                    break;
                case 4:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 40;
                    break;
                case 5:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 42;
                    break;
                case 6:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 56;
                    break;
                case 7:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 58;
                    break;
                        
            }
            
            break;
        case APOLLOMP_CHIP_ID:
        default:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 0x07;
            break;
        
    }    
    re8686_tx_with_Info(data, dataLen - 2, &txInfo);
}
#endif

#if defined(CONFIG_HYBRID_MODE)
static int proc_devmode_fops_read(struct seq_file *seq, void *v)
{
	int err = 0;
	
	seq_printf(seq, "DevMode: ");
	switch(epon_devmode){
		case 1: seq_printf(seq, "router\n"); break;
		case 0: seq_printf(seq, "bridge\n"); break;
		default: seq_printf(seq, "unknown\n"); break;
	}
	return err;
}
static int proc_devmode_fops_write(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	int mode = -1;
	char tmpbuf[24] = {0};
	
	if(buf && count < (sizeof(tmpbuf)-1) && 
		!copy_from_user(tmpbuf, buf, count))
	{
		sscanf(tmpbuf, "%d", &mode);
		switch(mode){
			case 1:
			case 0: 
				epon_devmode = mode;
				break;
			default:
				printk("Errordevmode !!!!\n"); 
		}
	}
	
	return count;
}

static int proc_devmode_fops_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_devmode_fops_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops devmode_fops = {
        .proc_open      = proc_devmode_fops_open,
        .proc_read      = seq_read,
        .proc_write     = proc_devmode_fops_write,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations devmode_fops = {
        .owner          = THIS_MODULE,
        .open           = proc_devmode_fops_open,
        .read           = seq_read,
        .write          = proc_devmode_fops_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif
#endif


int __init epon_drv_init(void)
{
    extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
    uint32 chipId, rev, subtype;
    rtk_portmask_t portMask;
    int ret;
    
    rtk_switch_portMask_Clear(&portMask);
    rtk_switch_port2PortMask_set(&portMask,RTK_PORT_PON);
    rtl960x_pon_port_mask = portMask.bits[0];

    ret = rtk_switch_version_get(&chipId, &rev, &subtype);
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
    }
    switch(chipId)
    {    
        case RTL9601B_CHIP_ID:
            rtl960x_all_port_mask = 0x3;
            break;
        case RTL9602C_CHIP_ID:
            rtl960x_all_port_mask = 0x7;
            break;
        case APOLLOMP_CHIP_ID:
            rtl960x_all_port_mask = 0x3f;
            break;
        case RTL9607C_CHIP_ID:
            rtl960x_all_port_mask = 0x7ff;
            break;
        case RTL9603CVD_CHIP_ID:
            rtl960x_all_port_mask = 0x3f;
            break;

        default:
            rtl960x_all_port_mask = 0xff;
            break;
    }    

    printk("Register EPON for pkt_redirect module\n");
    /* For packet redirect to user space protocol */
#ifdef SUPPORT_OAM_KEEPLIVE
	epon_oam_keeplive_init();
	pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GMAC, epon_oam_pkt_tx_for_keeplive);
#else
    pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GMAC, epon_oam_pkt_tx);
#endif
    pkt_redirect_kernelApp_reg(PR_KERNEL_UID_EPONDYINGGASP, epon_oam_dyingGasp_tx);
    /* Hook on PON port only */
#ifdef CONFIG_COMMON_RT_API
    rt_epon_oam_rx_callback_register(rt_oam_rx);
#else
    #if defined(FPGA_DEFINED)
    drv_nic_register_rxhook(0xFF, RE8686_RXPRI_OAM, epon_oam_pkt_rx);
    #else
    drv_nic_register_rxhook(rtl960x_pon_port_mask, RE8686_RXPRI_OAM, epon_oam_pkt_rx);
    #endif
#endif
#ifndef CONFIG_EPON_OAM_DUMMY_MODE
#if !defined(CONFIG_RTK_L34_ENABLE) || defined(CONFIG_HYBRID_MODE)
	/* This feature should be mutually exclusive with RG */
	#if defined(CONFIG_RTK_OAM_V2)
    drv_nic_register_rxhook(0x0F, RE8686_RXPRI_MUTICAST, epon_muticast_pkt_rx);
	#endif
#endif

/* siyuan 2016-08-03: epon oam handle igmp if not use RG */
#if defined(CONFIG_RTK_OAM_V1) && (!defined(CONFIG_RTK_L34_ENABLE) && !defined(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE) || defined(CONFIG_HYBRID_MODE))
	ret = rtk_trap_igmpCtrlPkt2CpuEnable_set(ENABLED);
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_trap_igmpCtrlPkt2CpuEnable_set failed %d!\n", __FILE__, __LINE__,ret);
    }	
	ret = rtk_trap_mldCtrlPkt2CpuEnable_set(ENABLED);
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_trap_mldCtrlPkt2CpuEnable_set failed %d!\n", __FILE__, __LINE__,ret);
    }
    
  	drv_nic_register_rxhook(rtl960x_all_port_mask, RE8686_RXPRI_MUTICAST, epon_muticast_pkt_rx);
  	pkt_redirect_kernelApp_reg(PR_KERNEL_UID_IGMPMLD, epon_muticast_pkt_tx);
#ifdef SUPPORT_IGMP_RATE_LIMIT
	epon_igmp_rate_limit_init();
#endif
#endif
#endif

#if defined(CONFIG_HYBRID_MODE)
	proc_create("devmode", 0644, NULL, &devmode_fops);
#endif

    return 0;
}

void __exit epon_drv_exit(void)
{
    extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);

    pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_GMAC);
    pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_EPONDYINGGASP);

#ifndef CONFIG_COMMON_RT_API
    #if defined(FPGA_DEFINED)
    drv_nic_unregister_rxhook(0xFF, RE8686_RXPRI_OAM, epon_oam_pkt_rx);
    #else
    drv_nic_unregister_rxhook(rtl960x_pon_port_mask, RE8686_RXPRI_OAM, epon_oam_pkt_rx);
    #endif
#endif

#if !defined(CONFIG_RTK_L34_ENABLE) || defined(CONFIG_HYBRID_MODE)
	#if defined(CONFIG_RTK_OAM_V2)
    /* This feature should be mutually exclusive with RG */
    drv_nic_unregister_rxhook(0x0F, RE8686_RXPRI_MUTICAST, epon_muticast_pkt_rx);
	#endif
#endif
/* siyuan 2016-08-03: epon oam handle igmp if not use RG */
#if defined(CONFIG_RTK_OAM_V1) && (!defined(CONFIG_RTK_L34_ENABLE) && !defined(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE) || defined(CONFIG_HYBRID_MODE))
	drv_nic_unregister_rxhook(rtl960x_all_port_mask, RE8686_RXPRI_MUTICAST, epon_muticast_pkt_rx);
	pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_IGMPMLD);
#endif

#if defined(CONFIG_HYBRID_MODE)
	remove_proc_entry("devmode", NULL);
#endif

}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek EPON drive module");
MODULE_AUTHOR("Realtek");
module_init(epon_drv_init);
module_exit(epon_drv_exit);
#endif

