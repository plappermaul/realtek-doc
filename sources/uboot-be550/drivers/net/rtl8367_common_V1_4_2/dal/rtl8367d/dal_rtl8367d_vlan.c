/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
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
 * Purpose : RTK switch high-level API for RTL8367/RTL8367D
 * Feature : Here is a list of all functions and variables in VLAN module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8367d/dal_rtl8367d_vlan.h>
#include <dal/rtl8367d/rtl8367d_asicdrv.h>
#include <linux/string.h>


#if defined(CONFIG_RTL8367D_ASICDRV_TEST)
dal_rtl8367d_user_vlan4kentry Rtl8367dVirtualVlanTable[RTL8367D_VIDMAX + 1];
#endif

static void _dal_rtl8367d_Vlan4kStUser2Smi(dal_rtl8367d_user_vlan4kentry *pUserVlan4kEntry, rtk_uint16 *pSmiVlan4kEntry)
{
    pSmiVlan4kEntry[0] |= (pUserVlan4kEntry->mbr & 0x00FF);
    pSmiVlan4kEntry[0] |= (pUserVlan4kEntry->untag & 0x00FF) << 8;

    pSmiVlan4kEntry[1] |= (pUserVlan4kEntry->fid_msti & 0x0003);
    pSmiVlan4kEntry[1] |= (pUserVlan4kEntry->svlan_chk_ivl_svl & 0x0001) << 2;
    pSmiVlan4kEntry[1] |= (pUserVlan4kEntry->ivl_svl & 0x0001) << 3;
}


static void _dal_rtl8367d_Vlan4kStSmi2User(rtk_uint16 *pSmiVlan4kEntry, dal_rtl8367d_user_vlan4kentry *pUserVlan4kEntry)
{
    pUserVlan4kEntry->mbr               = (pSmiVlan4kEntry[0] & 0x00FF);
    pUserVlan4kEntry->untag             = (pSmiVlan4kEntry[0] & 0xFF00) >> 8;

    pUserVlan4kEntry->fid_msti          = (pSmiVlan4kEntry[1] & 0x0003);
    pUserVlan4kEntry->svlan_chk_ivl_svl = (pSmiVlan4kEntry[1] & 0x0004) >> 2;
    pUserVlan4kEntry->ivl_svl           = (pSmiVlan4kEntry[1] & 0x0008) >> 3;
}
ret_t _dal_rtl8367d_setAsicVlan4kEntry(dal_rtl8367d_user_vlan4kentry *pVlan4kEntry )
{
    rtk_uint16              vlan_4k_entry[RTL8367D_VLAN_4KTABLE_LEN];
    rtk_uint32              page_idx;
    rtk_uint16              *tableAddr;
    ret_t                   retVal;
    rtk_uint32              regData;

    if(pVlan4kEntry->vid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    if(pVlan4kEntry->mbr > RTL8367D_PORTMASK)
        return RT_ERR_PORT_MASK;

    if(pVlan4kEntry->untag > RTL8367D_PORTMASK)
        return RT_ERR_PORT_MASK;

    if(pVlan4kEntry->fid_msti > RTL8367D_FIDMAX)
        return RT_ERR_L2_FID;

    if(pVlan4kEntry->ivl_svl> RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if(pVlan4kEntry->svlan_chk_ivl_svl> RTK_ENABLE_END)
        return RT_ERR_INPUT;

    memset(vlan_4k_entry, 0x00, sizeof(rtk_uint16) * RTL8367D_VLAN_4KTABLE_LEN);
    _dal_rtl8367d_Vlan4kStUser2Smi(pVlan4kEntry, vlan_4k_entry);
    //PRINT("\n %s %d\n",__FUNCTION__,__LINE__);

    /* Prepare Data */
    tableAddr = vlan_4k_entry;
    for(page_idx = 0; page_idx < RTL8367D_VLAN_4KTABLE_LEN; page_idx++)
    {
        regData = *tableAddr;
        retVal = rtl8367d_setAsicReg(RTL8367D_REG_TABLE_WRITE_DATA0 + page_idx, regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        tableAddr++;
    }

    /* Write Address (VLAN_ID) */
    regData = pVlan4kEntry->vid;
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_TABLE_ACCESS_ADDR, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Write Command */
    retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_TABLE_ACCESS_CTRL, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_WRITE,RTL8367D_TB_TARGET_CVLAN));
    if(retVal != RT_ERR_OK)
        return retVal;

#if defined(CONFIG_RTL8367D_ASICDRV_TEST)
    memcpy(&Rtl8367dVirtualVlanTable[pVlan4kEntry->vid], pVlan4kEntry, sizeof(dal_rtl8367d_user_vlan4kentry));
#endif

    return RT_ERR_OK;
}


ret_t _dal_rtl8367d_getAsicVlan4kEntry(dal_rtl8367d_user_vlan4kentry *pVlan4kEntry )
{
    rtk_uint16                  vlan_4k_entry[RTL8367D_VLAN_4KTABLE_LEN];
    rtk_uint32                  page_idx;
    rtk_uint16                  *tableAddr;
    ret_t                       retVal;
    rtk_uint32                  regData;
    rtk_uint32                  busyCounter;

    if(pVlan4kEntry->vid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Polling status */
    busyCounter = RTL8367D_VLAN_BUSY_CHECK_NO;
    while(busyCounter)
    {
        retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_TABLE_LUT_ADDR, RTL8367D_TABLE_LUT_ADDR_BUSY_FLAG_OFFSET,&regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        if(regData == 0)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Write Address (VLAN_ID) */
    regData = pVlan4kEntry->vid;
    retVal = rtl8367d_setAsicReg(RTL8367D_REG_TABLE_ACCESS_ADDR, regData);
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Read Command */
    retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_TABLE_ACCESS_CTRL, RTL8367D_TABLE_TYPE_MASK | RTL8367D_COMMAND_TYPE_MASK, RTL8367D_TABLE_ACCESS_REG_DATA(RTL8367D_TB_OP_READ,RTL8367D_TB_TARGET_CVLAN));
    if(retVal != RT_ERR_OK)
        return retVal;

    /* Polling status */
    busyCounter = RTL8367D_VLAN_BUSY_CHECK_NO;
    while(busyCounter)
    {
        retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_TABLE_LUT_ADDR, RTL8367D_TABLE_LUT_ADDR_BUSY_FLAG_OFFSET,&regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        if(regData == 0)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Read VLAN data from register */
    tableAddr = vlan_4k_entry;
    for(page_idx = 0; page_idx < RTL8367D_VLAN_4KTABLE_LEN; page_idx++)
    {
        retVal = rtl8367d_getAsicReg(RTL8367D_REG_TABLE_READ_DATA0 + page_idx, &regData);
        if(retVal != RT_ERR_OK)
            return retVal;

        *tableAddr = regData;
        tableAddr++;
    }

    _dal_rtl8367d_Vlan4kStSmi2User(vlan_4k_entry, pVlan4kEntry);

#if defined(CONFIG_RTL8367D_ASICDRV_TEST)
    memcpy(pVlan4kEntry, &Rtl8367dVirtualVlanTable[pVlan4kEntry->vid], sizeof(dal_rtl8367d_user_vlan4kentry));
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_vlan_init
 * Description:
 *      Initialize VLAN.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      VLAN is disabled by default. User has to call this API to enable VLAN before
 *      using it. And It will set a default VLAN(vid 1) including all ports and set
 *      all ports PVID to the default VLAN.
 */
rtk_api_ret_t dal_rtl8367d_vlan_init(void)
{
    rtk_api_ret_t retVal;
    rtk_uint32 i;
    dal_rtl8367d_user_vlan4kentry vlan4K;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Set a default VLAN with vid 1 to 4K table for all ports */
    memset(&vlan4K, 0, sizeof(dal_rtl8367d_user_vlan4kentry));
    vlan4K.vid = 1;
    vlan4K.mbr = RTK_PHY_PORTMASK_ALL;
    vlan4K.untag = RTK_PHY_PORTMASK_ALL;
    vlan4K.fid_msti = 0;
    if ((retVal = _dal_rtl8367d_setAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    /* Set all ports PVID to default VLAN and tag-mode to original */
    RTK_SCAN_ALL_LOG_PORT(i)
    {
        if ((retVal = dal_rtl8367d_vlan_portPvid_set(i, 1, 0)) != RT_ERR_OK)
            return retVal;
        if ((retVal = dal_rtl8367d_vlan_tagMode_set(i, VLAN_TAG_MODE_ORIGINAL)) != RT_ERR_OK)
            return retVal;
    }

    /* Enable Ingress filter */
    RTK_SCAN_ALL_LOG_PORT(i)
    {
        if ((retVal = dal_rtl8367d_vlan_portIgrFilterEnable_set(i, ENABLED)) != RT_ERR_OK)
            return retVal;
    }

    /* enable VLAN */
    if ((retVal = dal_rtl8367d_vlan_egrFilterEnable_set(ENABLED)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_set
 * Description:
 *      Set a VLAN entry.
 * Input:
 *      vid - VLAN ID to configure.
 *      pVlanCfg - VLAN Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_L2_FID               - Invalid FID.
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - Invalid member port mask.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_set(rtk_vlan_t vid, rtk_vlan_cfg_t *pVlanCfg)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phyMbrPmask;
    rtk_uint32 phyUntagPmask;
    dal_rtl8367d_user_vlan4kentry vlan4K;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* vid must be 0~8191 */
    if (vid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Null pointer check */
    if(NULL == pVlanCfg)
        return RT_ERR_NULL_POINTER;

    /* Check port mask valid */
    RTK_CHK_PORTMASK_VALID(&(pVlanCfg->mbr));

    /* Check untag port mask valid */
    RTK_CHK_PORTMASK_VALID(&(pVlanCfg->untag));

    /* IVL_EN */
    if(pVlanCfg->ivl_en >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    /* fid must be 0~3 */
    if(pVlanCfg->fid_msti > RTL8367D_FIDMAX)
        return RT_ERR_L2_FID;

    /* Policing,  Meter ID ,  VLAN based priority ar not supported in RTL8367D*/
    if((pVlanCfg->envlanpol != 0)||(pVlanCfg->meteridx != 0) || (pVlanCfg->vbpen != 0) ||(pVlanCfg->vbpri != 0))
        return RT_ERR_INPUT;

    /* Get physical port mask */
    if(rtk_switch_portmask_L2P_get(&(pVlanCfg->mbr), &phyMbrPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if(rtk_switch_portmask_L2P_get(&(pVlanCfg->untag), &phyUntagPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    /* update 4K table */
    memset(&vlan4K, 0, sizeof(dal_rtl8367d_user_vlan4kentry));
    vlan4K.vid = vid;

    vlan4K.mbr    = (phyMbrPmask & 0xFFFF);
    vlan4K.untag  = (phyUntagPmask & 0xFFFF);

    vlan4K.ivl_svl      = pVlanCfg->ivl_en;
    vlan4K.fid_msti     = pVlanCfg->fid_msti;

    if ((retVal = _dal_rtl8367d_setAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_get
 * Description:
 *      Get a VLAN entry.
 * Input:
 *      vid - VLAN ID to configure.
 * Output:
 *      pVlanCfg - VLAN Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_get(rtk_vlan_t vid, rtk_vlan_cfg_t *pVlanCfg)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phyMbrPmask;
    rtk_uint32 phyUntagPmask;
    dal_rtl8367d_user_vlan4kentry vlan4K;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* vid must be 0~8191 */
    if (vid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* Null pointer check */
    if(NULL == pVlanCfg)
        return RT_ERR_NULL_POINTER;

    vlan4K.vid = vid;

    if ((retVal = _dal_rtl8367d_getAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    phyMbrPmask   = vlan4K.mbr;
    phyUntagPmask = vlan4K.untag;
    if(rtk_switch_portmask_P2L_get(phyMbrPmask, &(pVlanCfg->mbr)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if(rtk_switch_portmask_P2L_get(phyUntagPmask, &(pVlanCfg->untag)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    pVlanCfg->ivl_en    = vlan4K.ivl_svl;
    pVlanCfg->fid_msti  = vlan4K.fid_msti;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_egrFilterEnable_set
 * Description:
 *      Set VLAN egress filter.
 * Input:
 *      egrFilter - Egress filtering
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid input parameters.
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_egrFilterEnable_set(rtk_enable_t egrFilter)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(egrFilter >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    /* enable VLAN */
    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_VLAN_CTRL, RTL8367D_VLAN_CTRL_OFFSET, egrFilter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_egrFilterEnable_get
 * Description:
 *      Get VLAN egress filter.
 * Input:
 *      pEgrFilter - Egress filtering
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - NULL Pointer.
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_egrFilterEnable_get(rtk_enable_t *pEgrFilter)
{
    rtk_api_ret_t retVal;
    rtk_uint32 state;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pEgrFilter)
        return RT_ERR_NULL_POINTER;

    /* enable VLAN */
    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_VLAN_CTRL, RTL8367D_VLAN_CTRL_OFFSET, &state)) != RT_ERR_OK)
        return retVal;

    *pEgrFilter = (rtk_enable_t)state;
    return RT_ERR_OK;
}

/* Function Name:
 *     dal_rtl8367d_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID).
 * Input:
 *      port - Port id.
 *      pvid - Specified VLAN ID.
 *      priority - 802.1p priority for the PVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_VLAN_PRIORITY        - Invalid priority.
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - VLAN entry not found.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *       The API is used for Port-based VLAN. The untagged frame received from the
 *       port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtk_api_ret_t dal_rtl8367d_vlan_portPvid_set(rtk_port_t port, rtk_vlan_t pvid, rtk_pri_t priority)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    /* vid must be 0~8191 */
    if (pvid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* priority is not supported in RTL8367D */
    if (priority > 0)
        return RT_ERR_INPUT;

    retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_VLAN_PVID_CTRL0 + rtk_switch_port_L2P_get(port), RTL8367D_VLAN_PVID_CTRL0_MASK, pvid);
    if(retVal != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port.
 * Input:
 *      port - Port id.
 * Output:
 *      pPvid - Specified VLAN ID.
 *      pPriority - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can get the PVID and 802.1p priority for the PVID of Port-based VLAN.
 */
rtk_api_ret_t dal_rtl8367d_vlan_portPvid_get(rtk_port_t port, rtk_vlan_t *pPvid, rtk_pri_t *pPriority)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pPvid)
        return RT_ERR_NULL_POINTER;

    if(NULL == pPriority)
        return RT_ERR_NULL_POINTER;

    *pPriority = 0;

    retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_VLAN_PVID_CTRL0 + rtk_switch_port_L2P_get(port), RTL8367D_VLAN_PVID_CTRL0_MASK, pPvid);
    if(retVal != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port.
 * Input:
 *      port - Port id.
 *      igr_filter - VLAN ingress function enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The status of vlan ingress filter is as following:
 *      - DISABLED
 *      - ENABLED
 *      While VLAN function is enabled, ASIC will decide VLAN ID for each received frame and get belonged member
 *      ports from VLAN table. If received port is not belonged to VLAN member ports, ASIC will drop received frame if VLAN ingress function is enabled.
 */
rtk_api_ret_t dal_rtl8367d_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t igr_filter)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (igr_filter >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_VLAN_INGRESS, rtk_switch_port_L2P_get(port), igr_filter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portIgrFilterEnable_get
 * Description:
 *      Get VLAN Ingress Filter
 * Input:
 *      port        - Port id.
 * Output:
 *      pIgr_filter - VLAN ingress function enable status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can Get the VLAN ingress filter status.
 *     The status of vlan ingress filter is as following:
 *     - DISABLED
 *     - ENABLED
 */
rtk_api_ret_t dal_rtl8367d_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pIgr_filter)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pIgr_filter)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_VLAN_INGRESS, rtk_switch_port_L2P_get(port), pIgr_filter)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN accept_frame_type
 * Input:
 *      port                - Port id.
 *      accept_frame_type   - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_PORT_ID                  - Invalid port number.
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE   - Invalid frame type.
 * Note:
 *      The API is used for checking 802.1Q tagged frames.
 *      The accept frame type as following:
 *      - ACCEPT_FRAME_TYPE_ALL
 *      - ACCEPT_FRAME_TYPE_TAG_ONLY
 *      - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtk_api_ret_t dal_rtl8367d_vlan_portAcceptFrameType_set(rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    rtk_api_ret_t retVal;
    rtk_uint32 mask;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (accept_frame_type >= ACCEPT_FRAME_TYPE_END)
        return RT_ERR_VLAN_ACCEPT_FRAME_TYPE;

    mask = RTL8367D_PORT0_FRAME_TYPE_MASK << ((rtk_switch_port_L2P_get(port) & 0x7) << 1);
    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_VLAN_ACCEPT_FRAME_TYPE_CTRL0, mask, accept_frame_type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN accept_frame_type
 * Input:
 *      port - Port id.
 * Output:
 *      pAccept_frame_type - accept frame type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can Get the VLAN ingress filter.
 *     The accept frame type as following:
 *     - ACCEPT_FRAME_TYPE_ALL
 *     - ACCEPT_FRAME_TYPE_TAG_ONLY
 *     - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtk_api_ret_t dal_rtl8367d_vlan_portAcceptFrameType_get(rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    rtk_api_ret_t retVal;
    rtk_uint32 mask;
    rtk_uint32 type;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pAccept_frame_type)
        return RT_ERR_NULL_POINTER;

    mask = RTL8367D_PORT0_FRAME_TYPE_MASK << ((rtk_switch_port_L2P_get(port) & 0x7) << 1);
    if((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_VLAN_ACCEPT_FRAME_TYPE_CTRL0, mask, &type)) != RT_ERR_OK)
        return retVal;

    *pAccept_frame_type = (rtk_vlan_acceptFrameType_t)type;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_tagMode_set
 * Description:
 *      Set CVLAN egress tag mode
 * Input:
 *      port        - Port id.
 *      tag_mode    - The egress tag mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set Egress tag mode. There are 4 mode for egress tag:
 *      - VLAN_TAG_MODE_ORIGINAL,
 *      - VLAN_TAG_MODE_KEEP_FORMAT,
 *      - VLAN_TAG_MODE_PRI.
 *      - VLAN_TAG_MODE_REAL_KEEP_FORMAT,
 */
rtk_api_ret_t dal_rtl8367d_vlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tag_mode)
{
    rtk_api_ret_t retVal;
    rtk_uint32 addr;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (tag_mode >= VLAN_TAG_MODE_END)
        return RT_ERR_PORT_ID;

    addr = (RTL8367D_REG_PORT0_MISC_CFG + (rtk_switch_port_L2P_get(port) << 5));
    if ((retVal = rtl8367d_setAsicRegBits(addr, RTL8367D_PORT0_MISC_CFG_VLAN_EGRESS_MODE_MASK, tag_mode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_tagMode_get
 * Description:
 *      Get CVLAN egress tag mode
 * Input:
 *      port - Port id.
 * Output:
 *      pTag_mode - The egress tag mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get Egress tag mode. There are 4 mode for egress tag:
 *      - VLAN_TAG_MODE_ORIGINAL,
 *      - VLAN_TAG_MODE_KEEP_FORMAT,
 *      - VLAN_TAG_MODE_PRI.
 *      - VLAN_TAG_MODE_REAL_KEEP_FORMAT,
 */
rtk_api_ret_t dal_rtl8367d_vlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTag_mode)
{
    rtk_api_ret_t retVal;
    rtk_uint32  mode;
    rtk_uint32 addr;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pTag_mode)
        return RT_ERR_NULL_POINTER;

    addr = (RTL8367D_REG_PORT0_MISC_CFG + (rtk_switch_port_L2P_get(port) << 5));
    if ((retVal = rtl8367d_getAsicRegBits(addr, RTL8367D_PORT0_MISC_CFG_VLAN_EGRESS_MODE_MASK, &mode)) != RT_ERR_OK)
        return retVal;

    *pTag_mode = (rtk_vlan_tagMode_t)mode;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_transparent_set
 * Description:
 *      Set VLAN transparent mode
 * Input:
 *      egr_port        - Egress Port id.
 *      pIgr_pmask      - Ingress Port Mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_vlan_transparent_set(rtk_port_t egr_port, rtk_portmask_t *pIgr_pmask)
{
     rtk_api_ret_t retVal;
     rtk_uint32    pmask;

     /* Check initialization state */
     RTK_CHK_INIT_STATE();

     /* Check Port Valid */
     RTK_CHK_PORT_VALID(egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     RTK_CHK_PORTMASK_VALID(pIgr_pmask);

     if(rtk_switch_portmask_L2P_get(pIgr_pmask, &pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

     if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_VLAN_EGRESS_TRANS_CTRL0 + rtk_switch_port_L2P_get(egr_port), RTL8367D_VLAN_EGRESS_TRANS_CTRL0_MASK, pmask)) != RT_ERR_OK)
         return retVal;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_transparent_get
 * Description:
 *      Get VLAN transparent mode
 * Input:
 *      egr_port        - Egress Port id.
 * Output:
 *      pIgr_pmask      - Ingress Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_vlan_transparent_get(rtk_port_t egr_port, rtk_portmask_t *pIgr_pmask)
{
     rtk_api_ret_t retVal;
     rtk_uint32    pmask;

     /* Check initialization state */
     RTK_CHK_INIT_STATE();

     /* Check Port Valid */
     RTK_CHK_PORT_VALID(egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_VLAN_EGRESS_TRANS_CTRL0 + rtk_switch_port_L2P_get(egr_port), RTL8367D_VLAN_EGRESS_TRANS_CTRL0_MASK, &pmask)) != RT_ERR_OK)
         return retVal;

     if(rtk_switch_portmask_P2L_get(pmask, pIgr_pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_keep_set
 * Description:
 *      Set VLAN egress keep mode
 * Input:
 *      egr_port        - Egress Port id.
 *      pIgr_pmask      - Ingress Port Mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_vlan_keep_set(rtk_port_t egr_port, rtk_portmask_t *pIgr_pmask)
{
     rtk_api_ret_t retVal;
     rtk_uint32    pmask;
     rtk_uint32    regAddr, bitMask;

     /* Check initialization state */
     RTK_CHK_INIT_STATE();

     /* Check Port Valid */
     RTK_CHK_PORT_VALID(egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     RTK_CHK_PORTMASK_VALID(pIgr_pmask);

     if(rtk_switch_portmask_L2P_get(pIgr_pmask, &pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

     regAddr = RTL8367D_REG_VLAN_EGRESS_KEEP_CTRL0 + (rtk_switch_port_L2P_get(egr_port)>>1);
     bitMask = RTL8367D_PORT0_VLAN_KEEP_MASK_MASK<<((rtk_switch_port_L2P_get(egr_port)&1)*8);
     retVal = rtl8367d_setAsicRegBits(regAddr, bitMask, pmask);
     if(retVal != RT_ERR_OK)
         return retVal;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_keep_get
 * Description:
 *      Get VLAN egress keep mode
 * Input:
 *      egr_port        - Egress Port id.
 * Output:
 *      pIgr_pmask      - Ingress Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtk_api_ret_t dal_rtl8367d_vlan_keep_get(rtk_port_t egr_port, rtk_portmask_t *pIgr_pmask)
{
     rtk_api_ret_t retVal;
     rtk_uint32    pmask;
     rtk_uint32    regAddr, bitMask;

     /* Check initialization state */
     RTK_CHK_INIT_STATE();

     /* Check Port Valid */
     RTK_CHK_PORT_VALID(egr_port);

     if(NULL == pIgr_pmask)
        return RT_ERR_NULL_POINTER;

     regAddr = RTL8367D_REG_VLAN_EGRESS_KEEP_CTRL0 + (rtk_switch_port_L2P_get(egr_port)>>1);
     bitMask = RTL8367D_PORT0_VLAN_KEEP_MASK_MASK<<((rtk_switch_port_L2P_get(egr_port)&1)*8);
     retVal = rtl8367d_getAsicRegBits(regAddr, bitMask, &pmask);
     if(retVal != RT_ERR_OK)
         return retVal;

     if(rtk_switch_portmask_P2L_get(pmask, pIgr_pmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      vid - Specified VLAN ID.
 *      stg - spanning tree group instance.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_MSTI         - Invalid msti parameter
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *      The API can set spanning tree group instance of the vlan to the specified device.
 */
rtk_api_ret_t dal_rtl8367d_vlan_stg_set(rtk_vlan_t vid, rtk_stp_msti_id_t stg)
{
    rtk_api_ret_t retVal;
    dal_rtl8367d_user_vlan4kentry vlan4K;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* vid must be 0~4095 */
    if (vid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* stg must be 0~3 */
    if (stg > RTL8367D_MSTIMAX)
        return RT_ERR_MSTI;

    /* update 4K table */
    vlan4K.vid = vid;
    if ((retVal = _dal_rtl8367d_getAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    vlan4K.fid_msti= stg;
    if ((retVal = _dal_rtl8367d_setAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan to the specified device
 * Input:
 *      vid - Specified VLAN ID.
 * Output:
 *      pStg - spanning tree group instance.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *      The API can get spanning tree group instance of the vlan to the specified device.
 */
rtk_api_ret_t dal_rtl8367d_vlan_stg_get(rtk_vlan_t vid, rtk_stp_msti_id_t *pStg)
{
    rtk_api_ret_t retVal;
    dal_rtl8367d_user_vlan4kentry vlan4K;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* vid must be 0~4095 */
    if (vid > RTL8367D_VIDMAX)
        return RT_ERR_VLAN_VID;

    if(NULL == pStg)
        return RT_ERR_NULL_POINTER;

    /* update 4K table */
    vlan4K.vid = vid;
    if ((retVal = _dal_rtl8367d_getAsicVlan4kEntry(&vlan4K)) != RT_ERR_OK)
        return retVal;

    *pStg = vlan4K.fid_msti;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portFid_set
 * Description:
 *      Set port-based filtering database
 * Input:
 *      port - Port id.
 *      enable - ebable port-based FID
 *      fid - Specified filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_L2_FID - Invalid fid.
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can set port-based filtering database. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
rtk_api_ret_t dal_rtl8367d_vlan_portFid_set(rtk_port_t port, rtk_enable_t enable, rtk_fid_t fid)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (enable>=RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    /* fid must be 0~3 */
    if (fid > RTL8367D_FIDMAX)
        return RT_ERR_L2_FID;

    if ((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_PORT_PBFIDEN, rtk_switch_port_L2P_get(port), enable))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_setAsicReg(RTL8367D_REG_PORT0_PBFID + rtk_switch_port_L2P_get(port), fid))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_portFid_get
 * Description:
 *      Get port-based filtering database
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - ebable port-based FID
 *      pFid - Specified filtering database.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can get port-based filtering database status. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
rtk_api_ret_t dal_rtl8367d_vlan_portFid_get(rtk_port_t port, rtk_enable_t *pEnable, rtk_fid_t *pFid)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if(NULL == pFid)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_PORT_PBFIDEN, rtk_switch_port_L2P_get(port), pEnable))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367d_getAsicReg(RTL8367D_REG_PORT0_PBFID + rtk_switch_port_L2P_get(port), pFid))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/*Spanning Tree*/
/* Function Name:
 *      dal_rtl8367d_stp_mstpState_set
 * Description:
 *      Configure spanning tree state per each port.
 * Input:
 *      port - Port id
 *      msti - Multiple spanning tree instance.
 *      stp_state - Spanning tree state for msti
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_MSTI         - Invalid msti parameter.
 *      RT_ERR_MSTP_STATE   - Invalid STP state.
 * Note:
 *      System supports per-port multiple spanning tree state for each msti.
 *      There are four states supported by ASIC.
 *      - STP_STATE_DISABLED
 *      - STP_STATE_BLOCKING
 *      - STP_STATE_LEARNING
 *      - STP_STATE_FORWARDING
 */
rtk_api_ret_t dal_rtl8367d_stp_mstpState_set(rtk_stp_msti_id_t msti, rtk_port_t port, rtk_stp_state_t stp_state)
{
    rtk_api_ret_t retVal;
    rtk_uint32 bitMask;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (msti > RTL8367D_MSTIMAX)
        return RT_ERR_MSTI;

    if (stp_state >= STP_STATE_END)
        return RT_ERR_MSTP_STATE;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    bitMask = RTL8367D_VLAN_MSTI0_CTRL0_PORT0_STATE_MASK << (phyPort * 2);
    if ((retVal = rtl8367d_setAsicRegBits(RTL8367D_REG_VLAN_MSTI0_CTRL0 + (msti * 2), bitMask, stp_state)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_stp_mstpState_get
 * Description:
 *      Get spanning tree state per each port.
 * Input:
 *      port - Port id.
 *      msti - Multiple spanning tree instance.
 * Output:
 *      pStp_state - Spanning tree state for msti
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_MSTI         - Invalid msti parameter.
 * Note:
 *      System supports per-port multiple spanning tree state for each msti.
 *      There are four states supported by ASIC.
 *      - STP_STATE_DISABLED
 *      - STP_STATE_BLOCKING
 *      - STP_STATE_LEARNING
 *      - STP_STATE_FORWARDING
 */
rtk_api_ret_t dal_rtl8367d_stp_mstpState_get(rtk_stp_msti_id_t msti, rtk_port_t port, rtk_stp_state_t *pStp_state)
{
    rtk_api_ret_t retVal;
    rtk_uint32 bitMask;
    rtk_uint32 phyPort;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(port);

    if (msti > RTL8367D_MSTIMAX)
        return RT_ERR_MSTI;

    if(NULL == pStp_state)
        return RT_ERR_NULL_POINTER;

    phyPort = rtk_switch_port_L2P_get(port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    bitMask = RTL8367D_VLAN_MSTI0_CTRL0_PORT0_STATE_MASK << (phyPort * 2);
    if ((retVal = rtl8367d_getAsicRegBits(RTL8367D_REG_VLAN_MSTI0_CTRL0 + (msti * 2), bitMask, pStp_state)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_vlan_reservedVidAction_set
 * Description:
 *      Set Action of VLAN ID = 0 & 4095 tagged packet
 * Input:
 *      action_vid0     - Action for VID 0.
 *      action_vid4095  - Action for VID 4095.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_reservedVidAction_set(rtk_vlan_resVidAction_t action_vid0, rtk_vlan_resVidAction_t action_vid4095)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(action_vid0 >= RESVID_ACTION_END)
        return RT_ERR_INPUT;

    if(action_vid4095 >= RESVID_ACTION_END)
        return RT_ERR_INPUT;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL, RTL8367D_VLAN_VID0_TYPE_OFFSET, action_vid0)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL, RTL8367D_VLAN_VID4095_TYPE_OFFSET, action_vid4095)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_reservedVidAction_get
 * Description:
 *      Get Action of VLAN ID = 0 & 4095 tagged packet
 * Input:
 *      pAction_vid0     - Action for VID 0.
 *      pAction_vid4095  - Action for VID 4095.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - NULL Pointer
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_reservedVidAction_get(rtk_vlan_resVidAction_t *pAction_vid0, rtk_vlan_resVidAction_t *pAction_vid4095)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(pAction_vid0 == NULL)
        return RT_ERR_NULL_POINTER;

    if(pAction_vid4095 == NULL)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL, RTL8367D_VLAN_VID0_TYPE_OFFSET, (rtk_uint32 *)pAction_vid0)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL, RTL8367D_VLAN_VID4095_TYPE_OFFSET, (rtk_uint32 *)pAction_vid4095)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_realKeepRemarkEnable_set
 * Description:
 *      Set Real keep 1p remarking feature
 * Input:
 *      enabled     - State of 1p remarking at real keep packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_realKeepRemarkEnable_set(rtk_enable_t enabled)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(enabled >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL, RTL8367D_VLAN_1P_REMARK_BYPASS_REALKEEP_OFFSET, enabled)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8367d_vlan_realKeepRemarkEnable_get
 * Description:
 *      Get Real keep 1p remarking feature
 * Input:
 *      None.
 * Output:
 *      pEnabled     - State of 1p remarking at real keep packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_realKeepRemarkEnable_get(rtk_enable_t *pEnabled)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if(NULL == pEnabled)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8367d_getAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL, RTL8367D_VLAN_1P_REMARK_BYPASS_REALKEEP_OFFSET, (rtk_uint32 *)pEnabled)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8367d_vlan_reset
 * Description:
 *      Reset VLAN
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtk_api_ret_t dal_rtl8367d_vlan_reset(void)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    if((retVal = rtl8367d_setAsicRegBit(RTL8367D_REG_VLAN_EXT_CTRL2, RTL8367D_VLAN_EXT_CTRL2_OFFSET, 1)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}



