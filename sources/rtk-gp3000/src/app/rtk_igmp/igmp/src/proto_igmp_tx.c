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

int32 mcast_snooping_tx(sys_nic_pkt_t * pkt, uint16 vid, uint32 length, sys_logic_portmask_t *portmask)
{
    int             ret = 0;
	uint8           tagsts;
	uint16          userVid = 0, mvid = 0;
	uint32          port = 0, uiPPort;
	rtk_portmask_t  portMsk;

    IGMP_PKT_SEM_LOCK();

	if (IS_LOGIC_PORTMASK_PORTSET(*portmask, (HAL_GET_PON_PORT() + 1)))
	{
		if (IGMP_MODE_SNOOPING == igmpCtrl.igmpMode ||
            IGMP_MODE_SPR == igmpCtrl.igmpMode)
		{
			SYS_DBG(LOG_DBG_IGMP, "%s() %d, vid=%u, sid=%u, length=%u, length=%u\n",
                __FUNCTION__, __LINE__, vid, pkt->tx_tag.dst_port_mask, length, pkt->length);

			if (MODE_GPON != igmpCtrl.ponMode)
			{
				if (TRUE == mcast_ismvlan(vid))
				{
					if (TRUE == pkt->rx_tag.cvid_tagged)
					{
						memmove(&(pkt->data[12]), &(pkt->data[16]), pkt->length - 16);

						pkt->length -= 4;
						length -= 4;
						pkt->rx_tag.cvid_tagged = FALSE;
					}
					vid = 0;
				}
				else if (TRUE == mcast_mcVlanExist(vid))
				{
					//
				}
				else if (SYS_ERR_OK == mcast_igmpTagTranslationTableGetbyUservid(pkt->rx_tag.source_port, vid, &mvid))
				{
					pkt->data[14] |=(uint8)((mvid >> 8) & 0xF);
					pkt->data[15] = mvid & 0xFF;
					vid = mvid;
				}
			}
		}
		else
		{
			if (TRUE == pkt->rx_tag.cvid_tagged)
			{
				pkt->data[14] |= (uint8)((pkt->rx_tag.inner_vid >> 8) & 0xF);
				pkt->data[15] = pkt->rx_tag.inner_vid & 0xFF;
			}
			else
			{
				memmove(&(pkt->data[16]), &(pkt->data[12]), pkt->length-12);
				pkt->data[12] = 0x81;
				pkt->data[13] = 0x00;
				pkt->data[14] = (uint8)((pkt->rx_tag.inner_vid >> 8) & 0xF);
				pkt->data[15] = pkt->rx_tag.inner_vid & 0xFF;
				pkt->length += 4;
				length += 4;
			}
			//vid = pkt->rx_tag.source_port;
			SYS_DBG(LOG_DBG_IGMP, "%s() %d	packet with inner vid=%02x", __FUNCTION__, __LINE__, pkt->rx_tag.inner_vid);
		}

		RTK_PORTMASK_RESET(portMsk);
		RTK_PORTMASK_PORT_SET(portMsk, HAL_GET_PON_PORT());

        ret = igmp_mld_send_pkt(pkt->data, length, portMsk, pkt->tx_tag.dst_port_mask);
	}
	else
	{
		SYS_DBG(LOG_DBG_IGMP, "%s() %d	packet is send to UTP ports portMaks=%02x, length=%u, length=%u\n",
            __FUNCTION__, __LINE__, *(portmask->bits), length, pkt->length);

        RTK_PORTMASK_RESET(portMsk);
		//TBD: SQ use normal is FAIL on GPON mode
		//	FOR_EACH_NORMAL_PORT_IN_LOGIC_PORTMASK(port,*portmask)
		FOR_EACH_PORT_IN_LOGIC_PORTMASK(port, *portmask)
		{
			if (MODE_GPON != igmpCtrl.ponMode)
			{
				mcast_igmpMcTagstripGet(port, &tagsts);

				if (TAG_OPER_MODE_STRIP == tagsts)
				{
					if (TRUE == pkt->rx_tag.cvid_tagged)
				 	{
				 		memmove(&(pkt->data[12]), &(pkt->data[16]), pkt->length - 16);
						pkt->length -= 4;
						length -= 4;
						pkt->rx_tag.cvid_tagged = FALSE;
						vid = 0;
				 	}
				}
				else if (TAG_OPER_MODE_TRANSLATION == tagsts)
				{
					ret = mcast_igmpTagTranslationTableGet(port, vid, &userVid);
					/*if find it ,replace the vlan to uservid,else the vlan has been translated by aisc*/
					if (SYS_ERR_OK == ret)
					{
						pkt->data[14] |=(uint8)((userVid>> 8) & 0xF) ;
						pkt->data[15] = userVid & 0xFF;
						vid = userVid;
					}
					else
					{
					    /*just keep*/
						pkt->data[14] |= (uint8)((vid >> 8) & 0xF) ;
						pkt->data[15] = vid & 0xFF;
					}

				}
				else //transparent
				{

				}
			}

            if (INVALID_PORT != (uiPPort = PortLogic2PhyID(port)))
            {
    			RTK_PORTMASK_PORT_SET(portMsk, uiPPort);
            }
		}
        ret = igmp_mld_send_pkt(pkt->data, length, portMsk, pkt->tx_tag.dst_port_mask);
	}

    IGMP_PKT_SEM_UNLOCK();

    return ret;
}

