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
 * $Revision:
 * $Date:
 *
 * Purpose :
 *
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <util/inc/igmp_common_util.h>
#include "pf_vlan.h"
#include "pf_mac.h"

int32 pf_vlan_entry_create(uint32 ulVlanId)
{
	int32 ret;

    SYS_ERR_CHK((!p_vlan_ref_cnt), RT_ERR_INPUT);

    if (!VALID_VLAN_ID(ulVlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)))
    {
        return RT_ERR_INPUT;
    }

    p_vlan_ref_cnt[ulVlanId] = ((p_vlan_ref_cnt[ulVlanId] < UINT_MAX) ? p_vlan_ref_cnt[ulVlanId] + 1  : 2);

    SYS_DBG(LOG_DBG_IGMP,
            "%s() p_vlan_ref_cnt[%u]=%u\n", __FUNCTION__, ulVlanId, p_vlan_ref_cnt[ulVlanId]);

    if (1 == p_vlan_ref_cnt[ulVlanId])
    {
    	/*create vlan*/
    	if (RT_ERR_VLAN_EXIST == (ret = rtk_vlan_create(ulVlanId)))
        {
            return RT_ERR_VLAN_EXIST;
        }
    	else if ((RT_ERR_VLAN_EXIST != ret) && (RT_ERR_OK != ret))
    	{
    		return RT_ERR_FAILED;
    	}

    	SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_fidMode_set(ulVlanId, VLAN_FID_IVL)), RT_ERR_FAILED);
    }
    return RT_ERR_OK;
}

int32 pf_vlan_entry_delete(uint32 ulVlanId)
{
	int32 ret;

    SYS_ERR_CHK((!p_vlan_ref_cnt), RT_ERR_INPUT);

    if (!VALID_VLAN_ID(ulVlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)))
    {
        return RT_ERR_INPUT;
    }

    if (p_vlan_ref_cnt[ulVlanId] > 0)
    {
        p_vlan_ref_cnt[ulVlanId]--;
    }

    SYS_DBG(LOG_DBG_IGMP,
            "%s() p_vlan_ref_cnt[%u]=%u\n", __FUNCTION__, ulVlanId, p_vlan_ref_cnt[ulVlanId]);

    if (0 == p_vlan_ref_cnt[ulVlanId])
    {
    	/*delete vlan*/
    	if (RT_ERR_VLAN_ENTRY_NOT_FOUND == (ret = rtk_vlan_destroy(ulVlanId)))
        {
            return RT_ERR_VLAN_ENTRY_NOT_FOUND;
        }
    	else if ((RT_ERR_VLAN_ENTRY_NOT_FOUND != ret) && (RT_ERR_OK != ret))
    	{
    		return RT_ERR_FAILED;
    	}
    }
    return RT_ERR_OK;
}

static int32 pf_vlan_check_member_empty(uint32 ulVlanId, int32 *isempty)
{
    rtk_portmask_t stPhyMaskTmp, stPhyMaskUntagTmp;

    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }

    RTK_PORTMASK_RESET(stPhyMaskTmp);
    RTK_PORTMASK_RESET(stPhyMaskUntagTmp);

	SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_port_get(ulVlanId, &stPhyMaskTmp, &stPhyMaskUntagTmp)), RT_ERR_FAILED);

    *isempty = ((stPhyMaskTmp.bits[0] == 0) ?  FALSE : TRUE);

    return RT_ERR_OK;
}

static int32 pf_svlan_member_add(
    uint32 ulSvlanId, uint32 ulSvlanPri, rtk_portmask_t stPhyMask, rtk_portmask_t stPhyMaskUntag, uint32 ulSvlanFid)
{
	int32                   ret;
    rtk_svlan_memberCfg_t   stSvlan_cfg;

    if ((0 != ulSvlanId) && (!VALID_VLAN_ID(ulSvlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }

    osal_memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
	stSvlan_cfg.svid = ulSvlanId;

	if (RT_ERR_SVLAN_NOT_EXIST == (ret = rtk_svlan_memberPortEntry_get(&stSvlan_cfg)))
	{
		/*create svlan*/
		SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_create(ulSvlanId)), RT_ERR_FAILED);
	}
    else if ((RT_ERR_OK != ret) && (RT_ERR_SVLAN_NOT_EXIST != ret))
    {
        return RT_ERR_FAILED;
    }

    stSvlan_cfg.priority = ulSvlanPri;
	/* TBD: ignore FID
    stSvlan_cfg.fid = ulSvlanFid;*/
    stSvlan_cfg.memberport.bits[0] |= stPhyMask.bits[0];
    stSvlan_cfg.untagport.bits[0] |= stPhyMaskUntag.bits[0];
    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_memberPortEntry_set(&stSvlan_cfg)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_mc_translation_vlan_member_add(
    uint32 usrvlan, uint32 mvlan, rtk_portmask_t stPhyMask, rtk_portmask_t stPhyMaskUntag, rtk_portmask_t stPhyMasksvlanUntag)
{
	int32           ret;
    uint32          uiFid = 0;
    rtk_portmask_t  stPhyMaskTmp, stPhyMaskUntagTmp;
    //rtk_fid_t fidTmp = 0;

    if ((0 != usrvlan) && (!VALID_VLAN_ID(usrvlan, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }
	if ((0 != mvlan) && (!VALID_VLAN_ID(mvlan, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }
    /* TBD: ignore FID
    (void)pf_valid_fid_get(usrvlan, &uiFid);
    if (HAL_VLAN_FID_MAX() < uiFid)
    {
        return RT_ERR_FAILED;
    }
    */

    RTK_PORTMASK_RESET(stPhyMaskTmp);
    RTK_PORTMASK_RESET(stPhyMaskUntagTmp);

    (void)pf_mc_vlan_mem_get(usrvlan, &stPhyMaskTmp, &stPhyMaskUntagTmp);
	RTK_PORTMASK_OR(stPhyMaskTmp, stPhyMask);
	RTK_PORTMASK_OR(stPhyMaskUntagTmp, stPhyMaskUntag);

	RTK_PORTMASK_OR(stPhyMask, stPhyMaskTmp);
	RTK_PORTMASK_OR(stPhyMaskUntag, stPhyMaskUntagTmp);

	RTK_PORTMASK_PORT_SET(stPhyMask, HAL_GET_PON_PORT());

	/*create cvlan*/
	SYS_ERR_CHK(((RT_ERR_VLAN_EXIST != (ret = pf_vlan_entry_create(usrvlan))) && (RT_ERR_OK != ret)), RT_ERR_FAILED);

    //fidTmp = uiFid;
	SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_port_set(usrvlan, &stPhyMask, &stPhyMaskUntag)), RT_ERR_FAILED);

	/* TBD: ignore FID
	ret = rtk_vlan_fid_set(usrvlan, fidTmp);
	if(RT_ERR_OK != ret)
    {
        return RT_ERR_FAILED;
    }
    */
    /*set svlan entry*/
    SYS_ERR_CHK((RT_ERR_OK != pf_svlan_member_add(mvlan, 0, stPhyMask, stPhyMasksvlanUntag, uiFid)), RT_ERR_FAILED);

    /* TBD: ingore fid, no need update sw */
    /*port members exclude ctc transparent ports ,uplink and cpu ports
    (void)pf_fid_set_by_vid(usrvlan, uiFid, stPhyMaskTmp, stPhyMaskUntagTmp);*/
    return RT_ERR_OK;
}

int32 pf_mc_translation_vlan_member_remove(uint32 usrvlan,uint32 mvlan, rtk_portmask_t stPhyMask)
{
	int32 ret, bFind;
    //uint32 uiFid;
	rtk_portmask_t stPhyMaskTmp, stPhyMaskUntagTmp, zero_mask;

	RTK_PORTMASK_RESET(zero_mask);

    if ((0 != usrvlan) && (!VALID_VLAN_ID(usrvlan, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }

	if ((0 != mvlan) && (!VALID_VLAN_ID(mvlan, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
	{
	   return RT_ERR_INPUT;
	}
    /* TBD: ignore FID
    (void)pf_valid_fid_get(usrvlan, &uiFid);
    if (HAL_VLAN_FID_MAX() < uiFid)
    {
        return RT_ERR_FAILED;
    }
    */
	RTK_PORTMASK_RESET(stPhyMaskTmp);
    RTK_PORTMASK_RESET(stPhyMaskUntagTmp);

    (void)pf_mc_vlan_mem_get(usrvlan, &stPhyMaskTmp, &stPhyMaskUntagTmp);

	RTK_PORTMASK_REMOVE(stPhyMaskTmp, stPhyMask);
	RTK_PORTMASK_REMOVE(stPhyMaskUntagTmp, stPhyMask);

    if (RTK_PORTMASK_COMPARE(zero_mask, stPhyMaskTmp))
    {
        RTK_PORTMASK_PORT_SET(stPhyMaskTmp, HAL_GET_PON_PORT());
    }

	/*set cvlan entry*/
	SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_port_set(usrvlan, &stPhyMaskTmp, &stPhyMaskUntagTmp)), RT_ERR_FAILED);

    /* TBD: ignore FID
	ret = rtk_vlan_fid_set((rtk_vlan_t)usrvlan, uiFid);
	if (RT_ERR_OK != ret)
    {
        return RT_ERR_FAILED;
    }*/

	/*delete cvlan*/
	SYS_ERR_CHK( (RT_ERR_OK != pf_vlan_check_member_empty(usrvlan, &bFind)), RT_ERR_FAILED);

	RTK_PORTMASK_PORT_CLEAR(stPhyMaskTmp, HAL_GET_PON_PORT());

	if (!RTK_PORTMASK_COMPARE(zero_mask, stPhyMaskTmp))
	{
		pf_flush_mc_vlan_mac_by_vid(usrvlan);
		SYS_ERR_CHK(((RT_ERR_VLAN_ENTRY_NOT_FOUND != (ret = pf_vlan_entry_delete(usrvlan))) && (RT_ERR_OK != ret)), RT_ERR_FAILED);
	}

    /*set svlan entry*/
    rtk_svlan_memberCfg_t stSvlan_cfg;
    osal_memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
    stSvlan_cfg.svid = mvlan;
    stSvlan_cfg.priority = 0;
    //stSvlan_cfg.fid = uiFid;
    stSvlan_cfg.memberport.bits[0] |= stPhyMaskTmp.bits[0];
    stSvlan_cfg.untagport.bits[0]  |= stPhyMaskUntagTmp.bits[0];

    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_memberPortEntry_set(&stSvlan_cfg)), RT_ERR_FAILED);

	if ((0 == stSvlan_cfg.memberport.bits[0]) && (0 != mvlan))
	{
		/*delete svlan*/
		SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_destroy(mvlan)), RT_ERR_FAILED);
	}

    /* TBD: ingore fid, no need update sw */
    //port members exclude ctc transparent ports ,uplink and cpu ports
    /*
    (void)pf_mc_vlan_mem_get(usrvlan, &stPhyMaskTmp, &stPhyMaskUntagTmp);
	RTK_PORTMASK_REMOVE(stPhyMaskTmp, stPhyMask);
	RTK_PORTMASK_REMOVE(stPhyMaskUntagTmp, stPhyMask);
    (void)pf_fid_set_by_vid(usrvlan, uiFid, stPhyMaskTmp, stPhyMaskUntagTmp);*/

    return RT_ERR_OK;
}

int32 pf_mc_vlan_member_add(uint32 ulVlanId, rtk_portmask_t stPhyMask, rtk_portmask_t stPhyMaskUntag)
{
	int32           ret;
    uint32          uiFid = 0;
    rtk_portmask_t  stPhyMaskTmp, stPhyMaskUntagTmp;
    //rtk_fid_t fidTmp = 0;

    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }

    /* TBD: ignore FID
    (void)pf_valid_fid_get(ulVlanId, &uiFid);
    if (HAL_VLAN_FID_MAX() < uiFid)
    {
        return RT_ERR_FAILED;
    }
    */
    RTK_PORTMASK_RESET(stPhyMaskTmp);
    RTK_PORTMASK_RESET(stPhyMaskUntagTmp);

    (void)pf_mc_vlan_mem_get(ulVlanId, &stPhyMaskTmp, &stPhyMaskUntagTmp);

    RTK_PORTMASK_OR(stPhyMaskTmp, stPhyMask);
	RTK_PORTMASK_OR(stPhyMaskUntagTmp, stPhyMaskUntag);

    RTK_PORTMASK_OR(stPhyMask, stPhyMaskTmp);
    RTK_PORTMASK_OR(stPhyMaskUntag, stPhyMaskUntagTmp);
	RTK_PORTMASK_PORT_SET(stPhyMask, HAL_GET_CPU_PORT());
	RTK_PORTMASK_PORT_SET(stPhyMaskUntag, HAL_GET_PON_PORT());

	/*create cvlan*/
	SYS_ERR_CHK(((RT_ERR_VLAN_EXIST != (ret = pf_vlan_entry_create(ulVlanId))) && (RT_ERR_OK != ret)), RT_ERR_FAILED);

    //fidTmp = uiFid;

	SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_port_set(ulVlanId, &stPhyMask, &stPhyMaskUntag)), RT_ERR_FAILED);

    /* TBD: ignore FID
	ret = rtk_vlan_fid_set(ulVlanId, fidTmp);
	if(RT_ERR_OK != ret)
    {
        return RT_ERR_FAILED;
    }*/

    /*set svlan entry*/
    SYS_ERR_CHK((RT_ERR_OK != pf_svlan_member_add(ulVlanId, 0, stPhyMask, stPhyMaskUntag, uiFid)), RT_ERR_FAILED);

    /* TBD: ingore fid, no need update sw */
    /*port members exclude ctc transparent ports ,uplink and cpu ports
    (void)pf_fid_set_by_vid(ulVlanId, uiFid, stPhyMaskTmp, stPhyMaskUntagTmp);*/

    return RT_ERR_OK;
}

int32 pf_mc_vlan_member_remove(uint32 ulVlanId, rtk_portmask_t stPhyMask)
{
	int32 ret, bFind;
    //uint32 uiFid;
    rtk_portmask_t stPhyMaskTmp, stPhyMaskUntagTmp, zero_mask;

    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }

    RTK_PORTMASK_RESET(zero_mask);
    /* TBD: ingore FID
    (void)pf_valid_fid_get(ulVlanId, &uiFid);
    if (HAL_VLAN_FID_MAX() < uiFid)
    {
        return RT_ERR_FAILED;
    }*/

	RTK_PORTMASK_RESET(stPhyMaskTmp);
    RTK_PORTMASK_RESET(stPhyMaskUntagTmp);

    (void)pf_mc_vlan_mem_get(ulVlanId, &stPhyMaskTmp, &stPhyMaskUntagTmp);

	RTK_PORTMASK_REMOVE(stPhyMaskTmp, stPhyMask);
	RTK_PORTMASK_REMOVE(stPhyMaskUntagTmp, stPhyMask);

    if (RTK_PORTMASK_COMPARE(zero_mask, stPhyMaskTmp))
    {
		RTK_PORTMASK_PORT_SET(stPhyMaskTmp, HAL_GET_PON_PORT());
    }

    /*set cvlan entry*/
	SYS_ERR_CHK((RT_ERR_OK != rtk_vlan_port_set(ulVlanId, &stPhyMaskTmp, &stPhyMaskUntagTmp)), RT_ERR_FAILED);

    /* TBD: ignore FID
	ret = rtk_vlan_fid_set(ulVlanId, uiFid);
	if (RT_ERR_OK != ret)
    {
        return RT_ERR_FAILED;
    }*/

	/*delete cvlan*/
	SYS_ERR_CHK((RT_ERR_OK != pf_vlan_check_member_empty(ulVlanId, &bFind)), RT_ERR_FAILED);

	RTK_PORTMASK_PORT_CLEAR(stPhyMaskTmp, HAL_GET_PON_PORT());

	if (!RTK_PORTMASK_COMPARE(zero_mask, stPhyMaskTmp))
	{
		pf_flush_mc_vlan_mac_by_vid(ulVlanId);
		SYS_ERR_CHK(((RT_ERR_VLAN_ENTRY_NOT_FOUND != (ret = pf_vlan_entry_delete(ulVlanId))) && (RT_ERR_OK != ret)), RT_ERR_FAILED);
	}

    /*set svlan entry*/
    rtk_svlan_memberCfg_t stSvlan_cfg;
    osal_memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
    stSvlan_cfg.svid = ulVlanId;
    stSvlan_cfg.priority = 0;
    //TBD: ignore FID
    //stSvlan_cfg.fid = uiFid;
    stSvlan_cfg.memberport.bits[0] |= stPhyMaskTmp.bits[0];
    stSvlan_cfg.untagport.bits[0]  |= stPhyMaskUntagTmp.bits[0];

    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_memberPortEntry_set(&stSvlan_cfg)), RT_ERR_FAILED);

	if ((0 == stSvlan_cfg.memberport.bits[0]) && (0 != ulVlanId))
	{
		/*delete svlan*/
		SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_destroy(ulVlanId)), RT_ERR_FAILED);
	}
    /* TBD: ingore fid, no need update sw */
    //port members exclude ctc transparent ports ,uplink and cpu ports
    /*
    (void)pf_mc_vlan_mem_get(ulVlanId, &stPhyMaskTmp, &stPhyMaskUntagTmp);
    RTK_PORTMASK_REMOVE(stPhyMaskTmp, stPhyMask);
	RTK_PORTMASK_REMOVE(stPhyMaskUntagTmp, stPhyMask);
    (void)pf_fid_set_by_vid(ulVlanId, uiFid, stPhyMaskTmp, stPhyMaskUntagTmp);*/

    return RT_ERR_OK;
}

int32 pf_port_c2s_entry_add(uint32 uiLPort, uint32 ulCvid, uint32 ulSvid)
{
    uint32 uiPPort;

    if (((0 != ulSvid) && (!VALID_VLAN_ID(ulSvid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)))) ||
        (!VALID_VLAN_ID(ulCvid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))) ||
        (!IS_VALID_PORT_ID(uiLPort)))
    {
        return RT_ERR_INPUT;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_c2s_add(ulCvid, uiPPort, ulSvid)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_port_c2s_entry_delete(uint32 uiLPort, uint32 ulCvid, uint32 ulSvid)
{
    uint32 uiPPort;

    if ((!VALID_VLAN_ID(ulCvid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))) ||
        (!IS_VALID_PORT_ID(uiLPort)))
    {
        return RT_ERR_INPUT;
    }

    uiPPort = PortLogic2PhyID(uiLPort);

    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_c2s_del(ulCvid, uiPPort, ulSvid)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_port_sp2c_entry_add(uint32 uiLPort, uint32 ulSvid, uint32 ulCvid)
{
    uint32 uiPPort;

    if (((0 != ulSvid) && (!VALID_VLAN_ID(ulSvid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode)))) ||
        (!VALID_VLAN_ID(ulCvid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))) ||
        (!IS_VALID_PORT_ID(uiLPort)))
    {
        return RT_ERR_INPUT;
    }

    uiPPort = PortLogic2PhyID(uiLPort);

    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_sp2c_add(ulSvid, uiPPort, ulCvid)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

int32 pf_port_sp2c_entry_delete(uint32 uiLPort, uint32 ulSvid)
{
    uint32 uiPPort;

    if ((0 != ulSvid) && (!VALID_VLAN_ID(ulSvid, MIN_VLAN_OFFSET(igmpCtrl.ponMode), MAX_VLAN_OFFSET(igmpCtrl.ponMode))))
    {
        return RT_ERR_INPUT;
    }

    uiPPort = PortLogic2PhyID(uiLPort);

    SYS_ERR_CHK((RT_ERR_OK != rtk_svlan_sp2c_del(ulSvid, uiPPort)), RT_ERR_FAILED);

    return RT_ERR_OK;
}

