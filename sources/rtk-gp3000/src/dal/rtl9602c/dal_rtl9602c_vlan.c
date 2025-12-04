/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 46639 $
 * $Date: 2014-02-26 16:48:44 +0800 (週三, 26 二月 2014) $
 *
 * Purpose : Definition of VLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Vlan table configure and modification
 *           (2) Accept frame type
 *           (3) Vlan ingress/egress filter
 *           (4) Port based and protocol based vlan
 *           (5) TPID configuration
 *           (6) Ingress tag handling
 *           (7) Tag format handling
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/vlan.h>
#include <dal/rtl9602c/dal_rtl9602c_vlan.h>
#include <hal/mac/drv.h>

/*
 * Symbol Definition
 */

static uint32   vlan_init = INIT_NOT_COMPLETED;

static uint32   vlan_valid[(RTK_VLAN_ID_MAX + 1) / 32];

/*
 * Macro Declaration
 */
#define DAL_RTL9602C_VLAN_SET(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {vlan_valid[vid >> 5] |= (1 << (vid&31));}\
} while (0);\

#define DAL_RTL9602C_VLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {vlan_valid[vid >> 5] &= (~(1 << (vid&31)));}\
} while (0);\

#define DAL_RTL9602C_VLAN_IS_SET(vid) \
    (((vid) <= RTK_VLAN_ID_MAX)?((vlan_valid[vid >> 5] >> (vid&31)) & 1): 0)


/*
 * Function Declaration
 */

static int32 _dal_rtl9602c_vlan_4kEntry_set(rtl9602c_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    rtl9602c_vlan_entry_t vlan_entry;
    RT_PARAM_CHK((NULL == pVlan4kEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlan4kEntry->mbr)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlan4kEntry->untag)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!(HAL_IS_EXTPORTMASK_VALID(pVlan4kEntry->exMbr)), RT_ERR_PORT_MASK);


    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    /*MBR*/
    if ((ret = table_field_set(RTL9602C_VLANt, RTL9602C_VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_set(RTL9602C_VLANt, RTL9602C_VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_set(RTL9602C_VLANt, RTL9602C_VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    /*svlna hash mode*/
    if ((ret = table_field_set(RTL9602C_VLANt, RTL9602C_VLAN_SVLAN_CHK_IVL_SVLtf, (uint32 *)&pVlan4kEntry->svlanHash, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*FID_MSTI*/
    if ((ret = table_field_set(RTL9602C_VLANt, RTL9602C_VLAN_FID_MSTItf, (uint32 *)&pVlan4kEntry->fid_msti, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*EXTMBR*/
    if ((ret = table_field_set(RTL9602C_VLANt, RTL9602C_VLAN_EXT_PORT_MASKtf, (uint32 *)&pVlan4kEntry->exMbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	if ((ret = table_write(RTL9602C_VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}
static int32 _dal_rtl9602c_vlan_4kEntry_get(rtl9602c_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    rtl9602c_vlan_entry_t vlan_entry;

	/*parameter check*/
    RT_PARAM_CHK((pVlan4kEntry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > RTL9602C_VIDMAX), RT_ERR_VLAN_VID);
    
    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    if ((ret = table_read(RTL9602C_VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*MBR*/
        /*get from vlan table*/
    if ((ret = table_field_get(RTL9602C_VLANt, RTL9602C_VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_get(RTL9602C_VLANt, RTL9602C_VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_get(RTL9602C_VLANt, RTL9602C_VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*SVLAN Hash*/
    if ((ret = table_field_get(RTL9602C_VLANt, RTL9602C_VLAN_SVLAN_CHK_IVL_SVLtf, (uint32 *)&pVlan4kEntry->svlanHash, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*FID_MSTI*/
    if ((ret = table_field_get(RTL9602C_VLANt, RTL9602C_VLAN_FID_MSTItf, (uint32 *)&pVlan4kEntry->fid_msti, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*EXTMBR*/
    if ((ret = table_field_get(RTL9602C_VLANt, RTL9602C_VLAN_EXT_PORT_MASKtf, (uint32 *)&pVlan4kEntry->exMbr, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9602c_vlan_existCheck
 * Description:
 *      Check VLAN ID exist or not
 * Input:
 *      vid     - VLAN ID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_FULL
 * Note:
 *      Check vid 0~4095 vlan is existed or not. This API is for SVLAN creating while svlan looukp
 *      type is configured as SVLAN_LOOKUP_C4KVLAN
 */
int32
dal_rtl9602c_vlan_existCheck(rtk_vlan_t vid)
{
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

	if(DAL_RTL9602C_VLAN_IS_SET(vid))
	{
		return RT_ERR_OK;
	}

	return RT_ERR_VLAN_ENTRY_NOT_FOUND;
}
/* Function Name:
 *      dal_rtl9602c_vlan_init
 * Description:
 *      Initialize vlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
int32
dal_rtl9602c_vlan_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_portmask_t allPortMask;
    rtk_portmask_t allExtPortMask;
    rtl9602c_vlan4kentry_t vlan;
    rtk_leaky_type_t leaky_type;
    rtk_vlan_keep_type_t keep_type;
    uint32 idx;
    rtk_vlan_protoGroup_t protoGroup;
    rtk_vlan_protoVlanCfg_t protoVlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    osal_memset(vlan_valid, 0x00, sizeof(uint32) * ((RTK_VLAN_ID_MAX + 1) / 32));

    vlan_init = INIT_COMPLETED;

    /* Reset VLAN Table */
    if((ret = dal_rtl9602c_vlan_destroyAll(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* VLAN filter */
    if((ret = dal_rtl9602c_vlan_vlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        /* VLAN Ingress filter */
        if((ret = dal_rtl9602c_vlan_portIgrFilterEnable_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /* Egress Tag Mode */
        if((ret = dal_rtl9602c_vlan_tagMode_set(port, VLAN_TAG_MODE_ORIGINAL)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /* Create Default VLAN */
    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = DAL_RTL9602C_DEFAULT_VLAN_ID;

    HAL_GET_ALL_PORTMASK(allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.mbr, allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
    HAL_GET_ALL_EXT_PORTMASK(allExtPortMask);
    RTK_PORTMASK_ASSIGN(vlan.exMbr, allExtPortMask);
    if((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }


    /* Database update */
    DAL_RTL9602C_VLAN_SET(DAL_RTL9602C_DEFAULT_VLAN_ID)

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl9602c_vlan_portAcceptFrameType_set(port, ACCEPT_FRAME_TYPE_ALL )) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9602c_vlan_portIgrFilterEnable_set(port, ENABLED )) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9602c_vlan_portPvid_set(port, DAL_RTL9602C_DEFAULT_VLAN_ID)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }	
    }

    if((ret = dal_rtl9602c_vlan_vlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* VLAN leaky - system setting */
    for(leaky_type = LEAKY_BRG_GROUP; leaky_type < LEAKY_END; leaky_type++)
    {
        ret = dal_rtl9602c_vlan_leaky_set(leaky_type, DISABLED);
        if( ret == RT_ERR_CHIP_NOT_SUPPORTED)
            continue;

        if( ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /* VLAN leaky - port-based setting */
    for(leaky_type = LEAKY_BRG_GROUP; leaky_type < LEAKY_END; leaky_type++)
    {
        HAL_SCAN_ALL_PORT(port)
        {
            ret = dal_rtl9602c_vlan_portLeaky_set(port, leaky_type, DISABLED);
            if( ret == RT_ERR_CHIP_NOT_SUPPORTED)
                continue;

            if( ret != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                vlan_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    for(keep_type = KEEP_BRG_GROUP; keep_type < KEEP_END; keep_type++)
    {
        ret = dal_rtl9602c_vlan_keepType_set(keep_type, DISABLED);
        if( ret == RT_ERR_CHIP_NOT_SUPPORTED)
            continue;

        if( ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    osal_memset(&protoGroup, 0x00, sizeof(rtk_vlan_protoGroup_t));
    for(idx = 0; idx <= HAL_PROTOCOL_VLAN_IDX_MAX(); idx++)
    {
        if((ret = dal_rtl9602c_vlan_protoGroup_set(idx, &protoGroup)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    osal_memset(&protoVlan, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    protoVlan.vid = DAL_RTL9602C_DEFAULT_VLAN_ID;
    HAL_SCAN_ALL_PORT(port)
    {
    	for(idx = 0; idx <= HAL_PROTOCOL_VLAN_IDX_MAX(); idx++)
    	{
	        if((ret = dal_rtl9602c_vlan_portProtoVlan_set(port, idx, &protoVlan)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
	            vlan_init = INIT_NOT_COMPLETED;

	            return ret;
	        }
    	}
    }


    if((ret = dal_rtl9602c_vlan_cfiKeepEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9602c_vlan_reservedVidAction_set(RESVID_ACTION_UNTAG, RESVID_ACTION_UNTAG)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_init */

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      dal_rtl9602c_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Note:
 */
int32
dal_rtl9602c_vlan_create(rtk_vlan_t vid)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_EXIST);

    DAL_RTL9602C_VLAN_SET(vid);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_create */

/* Function Name:
 *      dal_rtl9602c_vlan_destroy
 * Description:
 *      Destroy the vlan.
 * Input:
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_destroy(rtk_vlan_t vid)
{
    int32 ret;
    rtl9602c_vlan4kentry_t vlan;
    rtk_portmask_t allPortMask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    /* Set untag mask to VLAN */
    HAL_GET_ALL_PORTMASK(allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
    if((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    /* Clear Database */
    DAL_RTL9602C_VLAN_CLEAR(vid);

	return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_destroy */


/* Function Name:
 *      dal_rtl9602c_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan.
 * Input:
 *      restore_default_vlan - keep and restore default vlan id or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
dal_rtl9602c_vlan_destroyAll(uint32 restoreDefaultVlan)
{
    int32   ret;
    uint32 vidx;
    rtl9602c_vlan4kentry_t vlan;
    rtk_port_t port;
    rtk_portmask_t allPortMask;
    rtk_portmask_t allExtPortMask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= restoreDefaultVlan), RT_ERR_INPUT);

    /* Clear 4K VLAN */
    if((ret = table_clear(RTL9602C_VLANt, RTK_VLAN_ID_MIN, RTK_VLAN_ID_MAX)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
		return ret;
    }

    /* Update database */
    osal_memset(vlan_valid, 0x00, sizeof(uint32) * (RTK_VLAN_ID_MAX + 1) / 32);

    if(ENABLED == restoreDefaultVlan)
    {
        /* Create Default VLAN */
        osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
        vlan.vid = DAL_RTL9602C_DEFAULT_VLAN_ID;

        HAL_GET_ALL_PORTMASK(allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.mbr, allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
        HAL_GET_ALL_EXT_PORTMASK(allExtPortMask);
        RTK_PORTMASK_ASSIGN(vlan.exMbr, allExtPortMask);
        if((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        /* Database update */
        DAL_RTL9602C_VLAN_SET(DAL_RTL9602C_DEFAULT_VLAN_ID)

		/* Set PVID */
		HAL_SCAN_ALL_PORT(port)
		{
			if((ret = dal_rtl9602c_vlan_portPvid_set(port, DAL_RTL9602C_DEFAULT_VLAN_ID)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
				return ret;
			}
		}

	}

    /* Set untag mask to all VLANs */
    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    for( vidx = 0 ; vidx <= RTK_VLAN_ID_MAX ; vidx ++) 
    {
        /* Create Default VLAN */
        vlan.vid = vidx;

        HAL_GET_ALL_PORTMASK(allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
        if((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
	}

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_destroyAll */

/* Function Name:
 *      dal_rtl9602c_vlan_fid_get
 * Description:
 *      Get the filter id of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pFid - pointer buffer of filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) In IVL mode, vid is equal with fid after vlan create.
 *      (2) You don't need to care fid when you use the IVL mode.
 *      (3) The API should be used for SVL mode.
 */
int32
dal_rtl9602c_vlan_fid_get(rtk_vlan_t vid, rtk_fid_t *pFid)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pFid = (rtk_fid_t)vlan.fid_msti;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_fid_get */


/* Function Name:
 *      dal_rtl9602c_vlan_fid_set
 * Description:
 *      Set the filter id of the vlan.
 * Input:
 *      vid  - vlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 */
int32
dal_rtl9602c_vlan_fid_set(rtk_vlan_t vid, rtk_fid_t fid)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_VLAN_FID_MAX() < fid), RT_ERR_FID);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    vlan.fid_msti = (uint32)fid;

    if ((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
	
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_fid_set */

/* Function Name:
 *      dal_rtl9602c_vlan_fidMode_get
 * Description:
 *      Get the filter id mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pMode - pointer buffer of filter id mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_rtl9602c_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pMode = (rtk_fidMode_t)((RTL9602C_VLAN_HASH_IVL == vlan.ivl_svl) ? VLAN_FID_IVL : VLAN_FID_SVL);


    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_fidMode_get */

/* Function Name:
 *      dal_rtl9602c_vlan_fidMode_set
 * Description:
 *      Set the filter id mode of the vlan.
 * Input:
 *      vid   - vlan id
 *      mode  - filter id mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
dal_rtl9602c_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_FID_MODE_END <= mode), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    vlan.ivl_svl = (rtl9602c_vlan_l2HashType_t)((VLAN_FID_IVL == mode) ? RTL9602C_VLAN_HASH_IVL : RTL9602C_VLAN_HASH_SVL);

    if ((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_fidMode_set */

/* Function Name:
 *      dal_rtl9602c_vlan_port_get
 * Description:
 *      Get the vlan members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMemberPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntagPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    RTK_PORTMASK_ASSIGN((*pMemberPortmask), vlan.mbr);
    RTK_PORTMASK_ASSIGN((*pUntagPortmask), vlan.untag);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_port_get */


/* Function Name:
 *      dal_rtl9602c_vlan_port_set
 * Description:
 *      Replace the vlan members.
 * Input:
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 */
int32
dal_rtl9602c_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMember_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntag_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMember_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pUntag_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    RTK_PORTMASK_ASSIGN(vlan.mbr, (*pMember_portmask));
    RTK_PORTMASK_ASSIGN(vlan.untag, (*pUntag_portmask));

    if ((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_port_set */

/* Function Name:
 *      dal_rtl9602c_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan from the specified device.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pStg - pointer buffer of spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_stg_get(rtk_vlan_t vid, rtk_stg_t *pStg)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pStg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pStg = (rtk_stg_t)vlan.fid_msti;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_stg_get */


/* Function Name:
 *      dal_rtl9602c_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan.
 * Input:
 *      vid  - vlan id
 *      stg  - spanning tree group instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_MSTI                 - invalid msti
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_stg_set(rtk_vlan_t vid, rtk_stg_t stg)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_MSTI() <= stg), RT_ERR_MSTI);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    vlan.fid_msti = (uint32)stg;

    if ((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_stg_set */

/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      dal_rtl9602c_vlan_portAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port.
 * Input:
 *      port               - port id
 * Output:
 *      pAcceptFrameType   - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *          - ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY
 *
 */
int32
dal_rtl9602c_vlan_portAcceptFrameType_get(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAcceptFrameType)
{
    int32   ret;
    rtl9602c_dal_vlanAcpFrameType_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAcceptFrameType), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_VLAN_PORT_ACCEPT_FRAME_TYPEr, port , REG_ARRAY_INDEX_NONE, RTL9602C_FRAME_TYPEf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    switch(type)
    {
        case RTL9602C_DAL_VLAN_ACCEPT_ALL:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_ALL;
            break;
        case RTL9602C_DAL_VLAN_ACCEPT_TAGGED_ONLY:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_TAG_ONLY;
            break;
        case RTL9602C_DAL_VLAN_ACCEPT_UNTAGGED_ONLY:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
            break;
        case RTL9602C_DAL_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY:
            *pAcceptFrameType = ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portAcceptFrameType_get */


/* Function Name:
 *      dal_rtl9602c_vlan_portAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port.
 * Input:
 *      port              - port id
 *      acceptFrameType   - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 *      RT_ERR_CHIP_NOT_SUPPORTED     - functions not supported by this chip model
 * Note:
 *      The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
int32
dal_rtl9602c_vlan_portAcceptFrameType_set(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t acceptFrameType)
{
    int32   ret;
    rtl9602c_dal_vlanAcpFrameType_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= acceptFrameType), RT_ERR_INPUT);

    switch(acceptFrameType)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            type = RTL9602C_DAL_VLAN_ACCEPT_ALL;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            type = RTL9602C_DAL_VLAN_ACCEPT_TAGGED_ONLY;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            type = RTL9602C_DAL_VLAN_ACCEPT_UNTAGGED_ONLY;
            break;
        case ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY:
            type = RTL9602C_DAL_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if ((ret = reg_array_field_write(RTL9602C_VLAN_PORT_ACCEPT_FRAME_TYPEr, port , REG_ARRAY_INDEX_NONE, RTL9602C_FRAME_TYPEf, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
	
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portAcceptFrameType_set */

/* Function Name:
 *      dal_rtl9602c_vlan_vlanFunctionEnable_get
 * Description:
 *      Get the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_VLAN_CTRLr, RTL9602C_VLAN_FILTERINGf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_vlanFunctionEnable_get */


/* Function Name:
 *      dal_rtl9602c_vlan_vlanFunctionEnable_set
 * Description:
 *      Set the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_vlan_vlanFunctionEnable_set(rtk_enable_t enable)
{
    int32   ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    tmpVal = enable;

    if ((ret = reg_field_write(RTL9602C_VLAN_CTRLr,RTL9602C_VLAN_FILTERINGf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_vlanFunctionEnable_set */

/* Module Name    : Vlan                       */
/* Sub-module Name: Vlan ingress/egress filter */

/* Function Name:
 *      dal_rtl9602c_vlan_portIgrFilterEnable_get
 * Description:
 *      Get vlan ingress filter status of the port.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - pointer buffer of ingress filter status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_rtl9602c_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_VLAN_INGRESSr, port , REG_ARRAY_INDEX_NONE, RTL9602C_INGRESSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portIgrFilterEnable_get */


/* Function Name:
 *      dal_rtl9602c_vlan_portIgrFilterEnable_set
 * Description:
 *      Set vlan ingress filter status of the port to the specified device.
 * Input:
 *      port       - port id
 *      enable     - ingress filter configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of vlan ingress filter is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    tmpVal = enable;

    if ((ret = reg_array_field_write(RTL9602C_VLAN_INGRESSr, port , REG_ARRAY_INDEX_NONE, RTL9602C_INGRESSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portIgrFilterEnable_set */

/* Function Name:
 *      dal_rtl9602c_vlan_leaky_get
 * Description:
 *      Get VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      This API can get VLAN leaky status for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_CDP,
 *      - LEAKY_SSTP,
 */
int32
dal_rtl9602c_vlan_leaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((LEAKY_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case LEAKY_BRG_GROUP:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL00r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_FD_PAUSE:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL01r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_SP_MCAST:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL02r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_1X_PAE:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL03r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:
        case LEAKY_UNDEF_BRG_0F:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL04r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL08r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL0Dr, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_8021AB:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL0Er, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_BRG_MNGEMENT:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL10r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_11:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL11r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_12:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL12r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
        case LEAKY_UNDEFINED_19:
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL13r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_18:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL18r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_1A:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL1Ar, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_GMRP:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL20r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_GVRP:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL21r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
			if ((ret = reg_field_read(RTL9602C_RMA_CTRL22r, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
			if ((ret = reg_field_read(RTL9602C_IGMP_GLB_CTRLr, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
				return ret;
			}			
			
            break;
        case LEAKY_CDP:
            if ((ret = reg_field_read(RTL9602C_RMA_CTRL_CDPr, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_SSTP:
            if ((ret = reg_field_read(RTL9602C_RMA_CTRL_SSTPr, RTL9602C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_leaky_get */

/* Function Name:
 *      dal_rtl9602c_vlan_leaky_set
 * Description:
 *      Set VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_CDP,
 *      - LEAKY_SSTP,
 */
int32
dal_rtl9602c_vlan_leaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((LEAKY_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    switch(type)
    {
        case LEAKY_BRG_GROUP:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL00r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_FD_PAUSE:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL01r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_SP_MCAST:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL02r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_1X_PAE:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL03r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:
        case LEAKY_UNDEF_BRG_0F:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL04r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL08r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL0Dr, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_8021AB:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL0Er, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_BRG_MNGEMENT:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL10r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_11:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL11r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_12:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL12r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
        case LEAKY_UNDEFINED_19:
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL13r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_18:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL18r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEFINED_1A:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL1Ar, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_GMRP:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL20r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_GVRP:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL21r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
			if ((ret = reg_field_write(RTL9602C_RMA_CTRL22r, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_IGMP:
			if ((ret = reg_field_write(RTL9602C_IGMP_GLB_CTRLr, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
				return ret;
			}
			
            break;
        case LEAKY_CDP:
            if ((ret = reg_field_write(RTL9602C_RMA_CTRL_CDPr, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        case LEAKY_SSTP:
            if ((ret = reg_field_write(RTL9602C_RMA_CTRL_SSTPr, RTL9602C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_leaky_set */

/* Function Name:
 *      dal_rtl9602c_vlan_portLeaky_get
 * Description:
 *      Get VLAN port-based leaky.
 * Input:
 *      port - port ID
 *      type - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 *      RT_ERR_NULL_POINTER - NULL Pointer
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_IPMULTICAST
 */
int32
dal_rtl9602c_vlan_portLeaky_get(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= LEAKY_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case LEAKY_IPMULTICAST:

			if ((ret = reg_array_field_read(RTL9602C_L2_IPMC_VLAN_LEAKYr, port,REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &tmpVal)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
				return ret;
			}

            *pEnable = ((1 == tmpVal) ? ENABLED : DISABLED);
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portLeaky_get */

/* Function Name:
 *      dal_rtl9602c_vlan_portLeaky_set
 * Description:
 *      Set VLAN port-based leaky.
 * Input:
 *      port - port ID
 *      type - Packet type for VLAN leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_IPMULTICAST
 */
int32
dal_rtl9602c_vlan_portLeaky_set(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t enable)
{
    int32   ret;
	int32   tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= LEAKY_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch(type)
    {
        case LEAKY_IPMULTICAST:
			tmpVal = enable;
			
			if ((ret = reg_array_field_write(RTL9602C_L2_IPMC_VLAN_LEAKYr, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &tmpVal)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
				return ret;
			}

			
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portLeaky_set */

/* Function Name:
 *      dal_rtl9602c_vlan_keepType_get
 * Description:
 *      Get VLAN keep format setting.
 * Input:
 *      type - Packet type for VLAN keep format.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9602c_vlan_keepType_get(rtk_vlan_keep_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((type >= KEEP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case KEEP_BRG_GROUP:
            regAddr = RTL9602C_RMA_CTRL00r;
            break;
        case KEEP_FD_PAUSE:
            regAddr = RTL9602C_RMA_CTRL01r;
            break;
        case KEEP_SP_MCAST:
            regAddr = RTL9602C_RMA_CTRL02r;
            break;
        case KEEP_1X_PAE:
            regAddr = RTL9602C_RMA_CTRL03r;
            break;
        case KEEP_UNDEF_BRG_04:
        case KEEP_UNDEF_BRG_05:
        case KEEP_UNDEF_BRG_06:
        case KEEP_UNDEF_BRG_07:
        case KEEP_UNDEF_BRG_09:
        case KEEP_UNDEF_BRG_0A:
        case KEEP_UNDEF_BRG_0B:
        case KEEP_UNDEF_BRG_0C:
        case KEEP_UNDEF_BRG_0F:
            regAddr = RTL9602C_RMA_CTRL04r;
            break;
        case KEEP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            regAddr = RTL9602C_RMA_CTRL08r;
            break;
        case KEEP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            regAddr = RTL9602C_RMA_CTRL0Dr;
            break;
        case KEEP_8021AB:
            regAddr = RTL9602C_RMA_CTRL0Er;
            break;
        case KEEP_BRG_MNGEMENT:
            regAddr = RTL9602C_RMA_CTRL10r;
            break;
        case KEEP_UNDEFINED_11:
            regAddr = RTL9602C_RMA_CTRL11r;
            break;
        case KEEP_UNDEFINED_12:
            regAddr = RTL9602C_RMA_CTRL12r;
            break;
        case KEEP_UNDEFINED_13:
        case KEEP_UNDEFINED_14:
        case KEEP_UNDEFINED_15:
        case KEEP_UNDEFINED_16:
        case KEEP_UNDEFINED_17:
        case KEEP_UNDEFINED_19:
        case KEEP_UNDEFINED_1B:
        case KEEP_UNDEFINED_1C:
        case KEEP_UNDEFINED_1D:
        case KEEP_UNDEFINED_1E:
        case KEEP_UNDEFINED_1F:
            regAddr = RTL9602C_RMA_CTRL13r;
            break;
        case KEEP_UNDEFINED_18:
            regAddr = RTL9602C_RMA_CTRL18r;
            break;
        case KEEP_UNDEFINED_1A:
            regAddr = RTL9602C_RMA_CTRL1Ar;
            break;
        case KEEP_GMRP:
            regAddr = RTL9602C_RMA_CTRL20r;
            break;
        case KEEP_GVRP:
            regAddr = RTL9602C_RMA_CTRL21r;
            break;
        case KEEP_UNDEF_GARP_22:
        case KEEP_UNDEF_GARP_23:
        case KEEP_UNDEF_GARP_24:
        case KEEP_UNDEF_GARP_25:
        case KEEP_UNDEF_GARP_26:
        case KEEP_UNDEF_GARP_27:
        case KEEP_UNDEF_GARP_28:
        case KEEP_UNDEF_GARP_29:
        case KEEP_UNDEF_GARP_2A:
        case KEEP_UNDEF_GARP_2B:
        case KEEP_UNDEF_GARP_2C:
        case KEEP_UNDEF_GARP_2D:
        case KEEP_UNDEF_GARP_2E:
        case KEEP_UNDEF_GARP_2F:
            regAddr = RTL9602C_RMA_CTRL22r;
            break;
        case KEEP_CDP:
            regAddr = RTL9602C_RMA_CTRL_CDPr;
            break;
        case KEEP_SSTP:
            regAddr = RTL9602C_RMA_CTRL_SSTPr;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_read(regAddr, RTL9602C_KEEP_FORMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_keepType_get */

/* Function Name:
 *      dal_rtl9602c_vlan_keepType_set
 * Description:
 *      Set VLAN keep format setting.
 * Input:
 *      type - Packet type for VLAN keep format.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32
dal_rtl9602c_vlan_keepType_set(rtk_vlan_keep_type_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((type >= KEEP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch (type)
    {
        case KEEP_BRG_GROUP:
            regAddr = RTL9602C_RMA_CTRL00r;
            break;
        case KEEP_FD_PAUSE:
            regAddr = RTL9602C_RMA_CTRL01r;
            break;
        case KEEP_SP_MCAST:
            regAddr = RTL9602C_RMA_CTRL02r;
            break;
        case KEEP_1X_PAE:
            regAddr = RTL9602C_RMA_CTRL03r;
            break;
        case KEEP_UNDEF_BRG_04:
        case KEEP_UNDEF_BRG_05:
        case KEEP_UNDEF_BRG_06:
        case KEEP_UNDEF_BRG_07:
        case KEEP_UNDEF_BRG_09:
        case KEEP_UNDEF_BRG_0A:
        case KEEP_UNDEF_BRG_0B:
        case KEEP_UNDEF_BRG_0C:
        case KEEP_UNDEF_BRG_0F:
            regAddr = RTL9602C_RMA_CTRL04r;
            break;
        case KEEP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            regAddr = RTL9602C_RMA_CTRL08r;
            break;
        case KEEP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            regAddr = RTL9602C_RMA_CTRL0Dr;
            break;
        case KEEP_8021AB:
            regAddr = RTL9602C_RMA_CTRL0Er;
            break;
        case KEEP_BRG_MNGEMENT:
            regAddr = RTL9602C_RMA_CTRL10r;
            break;
        case KEEP_UNDEFINED_11:
            regAddr = RTL9602C_RMA_CTRL11r;
            break;
        case KEEP_UNDEFINED_12:
            regAddr = RTL9602C_RMA_CTRL12r;
            break;
        case KEEP_UNDEFINED_13:
        case KEEP_UNDEFINED_14:
        case KEEP_UNDEFINED_15:
        case KEEP_UNDEFINED_16:
        case KEEP_UNDEFINED_17:
        case KEEP_UNDEFINED_19:
        case KEEP_UNDEFINED_1B:
        case KEEP_UNDEFINED_1C:
        case KEEP_UNDEFINED_1D:
        case KEEP_UNDEFINED_1E:
        case KEEP_UNDEFINED_1F:
            regAddr = RTL9602C_RMA_CTRL13r;
            break;
        case KEEP_UNDEFINED_18:
            regAddr = RTL9602C_RMA_CTRL18r;
            break;
        case KEEP_UNDEFINED_1A:
            regAddr = RTL9602C_RMA_CTRL1Ar;
            break;
        case KEEP_GMRP:
            regAddr = RTL9602C_RMA_CTRL20r;
            break;
        case KEEP_GVRP:
            regAddr = RTL9602C_RMA_CTRL21r;
            break;
        case KEEP_UNDEF_GARP_22:
        case KEEP_UNDEF_GARP_23:
        case KEEP_UNDEF_GARP_24:
        case KEEP_UNDEF_GARP_25:
        case KEEP_UNDEF_GARP_26:
        case KEEP_UNDEF_GARP_27:
        case KEEP_UNDEF_GARP_28:
        case KEEP_UNDEF_GARP_29:
        case KEEP_UNDEF_GARP_2A:
        case KEEP_UNDEF_GARP_2B:
        case KEEP_UNDEF_GARP_2C:
        case KEEP_UNDEF_GARP_2D:
        case KEEP_UNDEF_GARP_2E:
        case KEEP_UNDEF_GARP_2F:
            regAddr = RTL9602C_RMA_CTRL22r;
            break;
        case KEEP_CDP:
            regAddr = RTL9602C_RMA_CTRL_CDPr;
            break;
        case KEEP_SSTP:
            regAddr = RTL9602C_RMA_CTRL_SSTPr;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    val = (uint32)enable;
    if ((ret = reg_field_write(regAddr, RTL9602C_KEEP_FORMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_keepType_set */

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      dal_rtl9602c_vlan_portPvid_get
 * Description:
 *      Get port default vlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid)
{
    int32  	ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_VLAN_PB_VIDr, port, REG_ARRAY_INDEX_NONE, RTL9602C_VIDf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pPvid = tmpVal;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portPvid_get */


/* Function Name:
 *      dal_rtl9602c_vlan_portPvid_set
 * Description:
 *      Set port default vlan id.
 * Input:
 *      port - port id
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_portPvid_set(rtk_port_t port, uint32 pvid)
{
    int32  ret;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < pvid), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(pvid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

	tmpVal = pvid;
    if ((ret = reg_array_field_write(RTL9602C_VLAN_PB_VIDr, port, REG_ARRAY_INDEX_NONE, RTL9602C_VIDf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portPvid_set */

/* Function Name:
 *      dal_rtl9602c_vlan_protoGroup_get
 * Description:
 *      Get protocol group for protocol based vlan.
 * Input:
 *      protoGroupIdx  - protocol group index
 * Output:
 *      pProtoGroup    - pointer to protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_protoGroup_get(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    rtl9602c_dal_protoValnEntryCfg_t entry;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);

    entry.index = protoGroupIdx;

    if ((ret = reg_array_field_read(RTL9602C_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry.index,RTL9602C_FRAME_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    entry.frameType = tmpVal;

    if ((ret = reg_array_field_read(RTL9602C_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry.index,RTL9602C_ETHER_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    entry.etherType = tmpVal;

    switch(entry.frameType)
    {
        case RTL9602C_DAL_PPVLAN_FRAME_TYPE_ETHERNET:
            pProtoGroup->frametype = FRAME_TYPE_ETHERNET;
            break;
        case RTL9602C_DAL_PPVLAN_FRAME_TYPE_LLC:
            pProtoGroup->frametype = FRAME_TYPE_LLCOTHER;
            break;
        case RTL9602C_DAL_PPVLAN_FRAME_TYPE_RFC1042:
            pProtoGroup->frametype = FRAME_TYPE_RFC1042;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    pProtoGroup->framevalue = entry.etherType;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_protoGroup_get */

/* Function Name:
 *      dal_rtl9602c_vlan_protoGroup_set
 * Description:
 *      Set protocol group for protocol based vlan.
 * Input:
 *      protoGroupIdx  - protocol group index
 *      protoGroup     - protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_VLAN_FRAME_TYPE - invalid frame type
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      Frame type is as following:
 *      - FRAME_TYPE_ETHERNET
 *      - FRAME_TYPE_RFC1042 (SNAP)
 *      - FRAME_TYPE_LLCOTHER
 */
int32
dal_rtl9602c_vlan_protoGroup_set(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    rtl9602c_dal_protoValnEntryCfg_t entry;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pProtoGroup->frametype >= FRAME_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pProtoGroup->frametype == FRAME_TYPE_SNAP8021H), RT_ERR_INPUT);
    RT_PARAM_CHK((pProtoGroup->frametype == FRAME_TYPE_SNAPOTHER), RT_ERR_INPUT);
    RT_PARAM_CHK((pProtoGroup->framevalue > 0x0000FFFF), RT_ERR_INPUT);

    entry.index = protoGroupIdx;
    entry.etherType = pProtoGroup->framevalue;

    switch(pProtoGroup->frametype)
    {
        case FRAME_TYPE_ETHERNET:
            entry.frameType = RTL9602C_DAL_PPVLAN_FRAME_TYPE_ETHERNET;
            break;
        case FRAME_TYPE_LLCOTHER:
            entry.frameType = RTL9602C_DAL_PPVLAN_FRAME_TYPE_LLC;
            break;
        case FRAME_TYPE_RFC1042:
            entry.frameType = RTL9602C_DAL_PPVLAN_FRAME_TYPE_RFC1042;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    tmpVal = entry.frameType;
    if ((ret = reg_array_field_write(RTL9602C_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE, entry.index, RTL9602C_FRAME_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry.etherType;
    if ((ret = reg_array_field_write(RTL9602C_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE, entry.index, RTL9602C_ETHER_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_protoGroup_set */

/* Function Name:
 *      dal_rtl9602c_vlan_portProtoVlan_get
 * Description:
 *      Get vlan of specificed protocol group on specified port.
 * Input:
 *      port           - port id
 *      protoGroup_idx - protocol group index
 * Output:
 *      pVlan_cfg      - pointer to vlan configuration of protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_portProtoVlan_get(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    rtl9602c_dal_protoVlanCfg_t cfg;
	uint32 	reg_value;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanCfg), RT_ERR_NULL_POINTER);

    cfg.index = protoGroupIdx;
    if ((ret = reg_array_read(RTL9602C_VLAN_PORT_PPB_VLANr, port, cfg.index, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

	/* Valid bit */
    if ((ret = reg_field_get(RTL9602C_VLAN_PORT_PPB_VLANr,RTL9602C_VALIDf, &tmpVal, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    cfg.valid = tmpVal;

	/* CVLAN id */
    if ((ret = reg_field_get(RTL9602C_VLAN_PORT_PPB_VLANr, RTL9602C_PPB_VIDf, &tmpVal, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    cfg.vid = tmpVal;


	/* priority */
    if ((ret = reg_field_get(RTL9602C_VLAN_PORT_PPB_VLANr, RTL9602C_PPB_PRIf, &tmpVal, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    cfg.priority = tmpVal;

    pVlanCfg->valid = cfg.valid;
    pVlanCfg->dei   = 0;
    pVlanCfg->vid   = cfg.vid;
    pVlanCfg->pri   = cfg.priority;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portProtoVlan_get */

/* Function Name:
 *      dal_rtl9602c_vlan_portProtoVlan_set
 * Description:
 *      Set vlan of specificed protocol group on specified port.
 * Input:
 *      port           - port id
 *      protoGroupIdx  - protocol group index
 *      pVlanCfg       - vlan configuration of protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_portProtoVlan_set(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    rtl9602c_dal_protoVlanCfg_t cfg;
	uint32 	reg_value;
    uint32 	tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pVlanCfg->dei), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pVlanCfg->valid), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < pVlanCfg->vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pVlanCfg->pri), RT_ERR_INPUT);

    cfg.index = protoGroupIdx;
    cfg.valid = pVlanCfg->valid;
    cfg.vid = pVlanCfg->vid;
    cfg.priority = pVlanCfg->pri;

	reg_value = 0;
	/* Valid bit */
	tmpVal = cfg.valid;
    if ((ret = reg_field_set(RTL9602C_VLAN_PORT_PPB_VLANr, RTL9602C_VALIDf, &tmpVal, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* CVLAN*/
	tmpVal = cfg.vid;
    if ((ret = reg_field_set(RTL9602C_VLAN_PORT_PPB_VLANr, RTL9602C_PPB_VIDf, &tmpVal, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* priority */
	tmpVal = cfg.priority;
    if ((ret = reg_field_set(RTL9602C_VLAN_PORT_PPB_VLANr, RTL9602C_PPB_PRIf, &tmpVal, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_write(RTL9602C_VLAN_PORT_PPB_VLANr, port, cfg.index, &reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_portProtoVlan_set */

/* Module Name    : Vlan                */
/* Sub-module Name: Tag format handling */

/* Function Name:
 *      dal_rtl9602c_vlan_tagMode_get
 * Description:
 *      Get vlan tagged mode of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The vlan tagged mode as following:
 *      - VLAN_TAG_MODE_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_PRI             (always priority tag out)
 */
int32
dal_rtl9602c_vlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTagMode)
{
    int32   ret;
    rtl9602c_dal_vlan_egrTagMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTagMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_VLAN_EGRESS_TAGr, port , REG_ARRAY_INDEX_NONE, RTL9602C_EGRESS_MODEf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch (mode)
    {
        case RTL9602C_DAL_VLAN_EGR_TAG_MODE_ORI:
            *pTagMode = VLAN_TAG_MODE_ORIGINAL;
            break;
        case RTL9602C_DAL_VLAN_EGR_TAG_MODE_KEEP:
            *pTagMode = VLAN_TAG_MODE_KEEP_FORMAT;
            break;
        case RTL9602C_DAL_VLAN_EGR_TAG_MODE_PRI_TAG:
            *pTagMode = VLAN_TAG_MODE_PRI;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_tagMode_get */


/* Function Name:
 *      dal_rtl9602c_vlan_tagMode_set
 * Description:
 *      Set vlan tagged mode of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The vlan tagged mode as following:
 *      - VLAN_TAG_MODE_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_PRI             (always priority tag out)
 */
int32
dal_rtl9602c_vlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tagMode)
{
    int32   ret;
    rtl9602c_dal_vlan_egrTagMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TAG_MODE_END <= tagMode), RT_ERR_INPUT);

    switch (tagMode)
    {
        case VLAN_TAG_MODE_ORIGINAL:
            mode = RTL9602C_DAL_VLAN_EGR_TAG_MODE_ORI;
            break;
        case VLAN_TAG_MODE_KEEP_FORMAT:
            mode = RTL9602C_DAL_VLAN_EGR_TAG_MODE_KEEP;
            break;
        case VLAN_TAG_MODE_PRI:
            mode = RTL9602C_DAL_VLAN_EGR_TAG_MODE_PRI_TAG;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if ((ret = reg_array_field_write(RTL9602C_VLAN_EGRESS_TAGr, port , REG_ARRAY_INDEX_NONE, RTL9602C_EGRESS_MODEf, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_tagMode_set */

/* Function Name:
 *      dal_rtl9602c_vlan_cfiKeepEnable_get
 * Description:
 *      Get state of CFI keep
 * Input:
 *      None
 * Output:
 *      pEnable - Pointer of CFI Keep
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      ENABLED: Keep original CFI value
 *      DISABLED: Always output VLAN tag with CFI = 0
 */
int32
dal_rtl9602c_vlan_cfiKeepEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    rtl9602c_dal_vlan_cfiKeepMode_t mode;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_VLAN_CTRLr,RTL9602C_CFI_KEEPf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    mode = tmpVal;

    *pEnable = ((RTL9602C_DAL_CFI_KEEP_INGRESS == mode) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_cfiKeepEnable_get */

/* Function Name:
 *      dal_rtl9602c_vlan_cfiKeepEnable_set
 * Description:
 *      Set state of CFI keep
 * Input:
 *      enable  - state of CFI KEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      ENABLED: Keep original CFI value
 *      DISABLED: Always output VLAN tag with CFI = 0
 */
int32
dal_rtl9602c_vlan_cfiKeepEnable_set(rtk_enable_t enable)
{
    int32   ret;
    rtl9602c_dal_vlan_cfiKeepMode_t mode;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    mode = ((ENABLED == enable) ? RTL9602C_DAL_CFI_KEEP_INGRESS : RTL9602C_DAL_CFI_ALWAYS_0);
    tmpVal = mode;
    if ((ret = reg_field_write(RTL9602C_VLAN_CTRLr,RTL9602C_CFI_KEEPf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_cfiKeepEnable_set */

/* Function Name:
 *      dal_rtl9602c_vlan_reservedVidAction_get
 * Description:
 *      Get the action of VID 0 and VID 4095 packet
 * Input:
 *      None
 * Output:
 *      pAction_vid0    - Pointer of VID 0 action
 *      pAction_vid4095 - Pointer of VID 4095 action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as untagged packets
 *      RESVID_ACTION_TAG: VID 0 or VID 4095 tagged packets will be treated as tagged packets
 */
int32
dal_rtl9602c_vlan_reservedVidAction_get(rtk_vlan_resVidAction_t *pAction_vid0, rtk_vlan_resVidAction_t *pAction_vid4095)
{
    int32   ret;
    rtl9602c_dal_vlanTag_t vid0_type;
    rtl9602c_dal_vlanTag_t vid4095_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction_vid0), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAction_vid4095), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9602C_VLAN_CTRLr,RTL9602C_VID_0_TYPEf, (uint32 *)&vid0_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9602C_VLAN_CTRLr,RTL9602C_VID_4095_TYPEf, (uint32 *)&vid4095_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    *pAction_vid0 = ((RTL9602C_DAL_VLAN_UNTAG == vid0_type) ? RESVID_ACTION_UNTAG : RESVID_ACTION_TAG);
    *pAction_vid4095 = ((RTL9602C_DAL_VLAN_UNTAG == vid4095_type) ? RESVID_ACTION_UNTAG : RESVID_ACTION_TAG);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_reservedVidAction_get */

/* Function Name:
 *      dal_rtl9602c_vlan_reservedVidAction_set
 * Description:
 *      Set the action of VID 0 and VID 4095 packet
 * Input:
 *      None
 * Output:
 *      action_vid0     - Pointer of VID 0 action
 *      action_vid4095  - Pointer of VID 4095 action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as untagged packets
 *      RESVID_ACTION_TAG: VID 0 or VID 4095 tagged packets will be treated as tagged packets
 */
int32
dal_rtl9602c_vlan_reservedVidAction_set(rtk_vlan_resVidAction_t action_vid0, rtk_vlan_resVidAction_t action_vid4095)
{
    int32   ret;
    rtl9602c_dal_vlanTag_t vid0_type;
    rtl9602c_dal_vlanTag_t vid4095_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((action_vid0 >= RESVID_ACTION_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action_vid4095 >= RESVID_ACTION_END), RT_ERR_INPUT);

    vid0_type = ((RESVID_ACTION_UNTAG == action_vid0) ? RTL9602C_DAL_VLAN_UNTAG : RTL9602C_DAL_VLAN_TAG);
    vid4095_type = ((RESVID_ACTION_UNTAG == action_vid4095) ? RTL9602C_DAL_VLAN_UNTAG : RTL9602C_DAL_VLAN_TAG);

    if ((ret = reg_field_write(RTL9602C_VLAN_CTRLr,RTL9602C_VID_0_TYPEf, (uint32 *)&vid0_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
	
    if ((ret = reg_field_write(RTL9602C_VLAN_CTRLr,RTL9602C_VID_4095_TYPEf, (uint32 *)&vid4095_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_reservedVidAction_set */

/* Function Name:
 *      dal_rtl9602c_vlan_tagModeIp4mc_get
 * Description:
 *      Get vlan tagged mode for ipv4 multicast packet of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
dal_rtl9602c_vlan_tagModeIp4mc_get(rtk_port_t port, rtk_vlan_tagModeIpmc_t *pTagMode)
{
    int32   ret;
    uint32 	val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTagMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_IP4MC_EGRESS_MODEr, port , REG_ARRAY_INDEX_NONE, RTL9602C_VLAN_IP4MC_EGRESS_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
	*pTagMode = val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_tagModeIp4mc_get */


/* Function Name:
 *      dal_rtl9602c_vlan_tagModeIp4mc_set
 * Description:
 *      Set vlan tagged mode of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
dal_rtl9602c_vlan_tagModeIp4mc_set(rtk_port_t port, rtk_vlan_tagModeIpmc_t tagMode)
{
    int32   ret;
    uint32 	val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TAG_MODE_IPMC_END <= tagMode), RT_ERR_INPUT);

	val = tagMode;

    if ((ret = reg_array_field_write(RTL9602C_IP4MC_EGRESS_MODEr, port , REG_ARRAY_INDEX_NONE, RTL9602C_VLAN_IP4MC_EGRESS_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
			
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_tagModeIp4mc_set */

/* Function Name:
 *      dal_rtl9602c_vlan_tagModeIp6mc_get
 * Description:
 *      Get vlan tagged mode for ipv6 multicast packet of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The ipv6 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
dal_rtl9602c_vlan_tagModeIp6mc_get(rtk_port_t port, rtk_vlan_tagModeIpmc_t *pTagMode)
{
    int32   ret;
    uint32 	val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTagMode), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_IP6MC_EGRESS_MODEr, port , REG_ARRAY_INDEX_NONE, RTL9602C_VLAN_IP6MC_EGRESS_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	*pTagMode = val;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_tagModeIp6mc_get */


/* Function Name:
 *      dal_rtl9602c_vlan_tagModeIp6mc_set
 * Description:
 *      Set vlan tagged mode of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
dal_rtl9602c_vlan_tagModeIp6mc_set(rtk_port_t port, rtk_vlan_tagModeIpmc_t tagMode)
{
    int32   ret;
    uint32 	val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TAG_MODE_IPMC_END <= tagMode), RT_ERR_INPUT);

	val = tagMode;

    if ((ret = reg_array_field_write(RTL9602C_IP6MC_EGRESS_MODEr, port , REG_ARRAY_INDEX_NONE, RTL9602C_VLAN_IP6MC_EGRESS_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_tagModeIp6mc_set */

/* Function Name:
 *      dal_rtl9602c_vlan_lutSvlanHashState_set
 * Description:
 *      Packet from SVLAN aware port will hash by SVLAN id.
 * Input:
 *      vid              - vlan id
 *      enable           - state for svlan aware port lut hash force using svlan  
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9602c_vlan_lutSvlanHashState_set(
    rtk_vlan_t     vid,
    rtk_enable_t enable)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;
   
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "vid=%d,enable=%d",vid, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    vlan.svlanHash = enable;

    if ((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_vlan_lutSvlanHashState_set */


/* Function Name:
 *      dal_rtl9602c_vlan_lutSvlanHashState_get
 * Description:
 *      Packet from SVLAN aware port will hash by SVLAN id.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pEnable          - pointer to svlan aware port SVLAN lut hash status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9602c_vlan_lutSvlanHashState_get(
    rtk_vlan_t     vid,
    rtk_enable_t *pEnable)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "vid=%d",vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
	
    *pEnable = vlan.svlanHash;
	
    return RT_ERR_OK;
}   /* end of dal_rtl9602c_vlan_lutSvlanHashState_get */

/* Function Name:
 *      dal_rtl9602c_vlan_extPort_get
 * Description:
 *      Get the vlan extension members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pExt_portmask    - pointer buffer of extension member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_extPort_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    RTK_PORTMASK_ASSIGN((*pExt_portmask), vlan.exMbr);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_extPort_get */


/* Function Name:
 *      dal_rtl9602c_vlan_extPort_set
 * Description:
 *      Replace the vlan extension members.
 * Input:
 *      vid              - vlan id
 *      pExt_portmask    - extension member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 */
int32
dal_rtl9602c_vlan_extPort_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    rtl9602c_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pExt_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExt_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan, 0x00, sizeof(rtl9602c_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9602c_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    RTK_PORTMASK_ASSIGN(vlan.exMbr, (*pExt_portmask));

    if ((ret = _dal_rtl9602c_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_extPort_set */

/* Function Name:
 *      dal_rtl9602c_vlan_extPortPvid_get
 * Description:
 *      Get extension port default vlan id.
 * Input:
 *      extPort - Extension port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_extPortPvid_get(uint32 extPort, uint32 *pPvid)
{
    int32   ret;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    if(HAL_GET_EXT_CPU_PORT() == extPort)
    {
		if ((ret = reg_array_field_read(RTL9602C_VLAN_PB_VIDr, HAL_GET_CPU_PORT(), REG_ARRAY_INDEX_NONE, RTL9602C_VIDf, &tmpVal)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
			return ret;
		}
    }
    else
    {
		if ((ret = reg_array_field_read(RTL9602C_VLAN_EXT_VIDr, (extPort - 1), REG_ARRAY_INDEX_NONE, RTL9602C_VIDf, &tmpVal)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
			return ret;
		}
    }

    *pPvid = tmpVal;
    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_extPortPvid_get */


/* Function Name:
 *      dal_rtl9602c_vlan_extPortPvid_set
 * Description:
 *      Set extension port default vlan id.
 * Input:
 *      extPort - extension port id
 *      pvid    - extension port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_rtl9602c_vlan_extPortPvid_set(uint32 extPort, uint32 pvid)
{
    int32   ret;
    uint32 tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX <= pvid), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_RTL9602C_VLAN_IS_SET(pvid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

	tmpVal = pvid;
	
    if(HAL_GET_EXT_CPU_PORT() == extPort)
    {
		if ((ret = reg_array_field_write(RTL9602C_VLAN_PB_VIDr, HAL_GET_CPU_PORT(), REG_ARRAY_INDEX_NONE, RTL9602C_VIDf, &tmpVal)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
			return ret;
		}
    }
    else
    {
		if ((ret = reg_array_field_write(RTL9602C_VLAN_EXT_VIDr, (extPort - 1), REG_ARRAY_INDEX_NONE, RTL9602C_VIDf, &tmpVal)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
			return ret;
		}
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_vlan_extPortPvid_set */




