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
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <rtk/vlan.h>
#include <dal/rtl9601b/dal_rtl9601b_vlan.h>
#include <hal/mac/drv.h>

/*
 * Symbol Definition
 */

static uint32   vlan_init = INIT_NOT_COMPLETED;
static uint32       vlan_valid[(RTK_EXT_VLAN_ID_MAX + 1) / 32];

/*
 * Macro Declaration
 */
#define DAL_RTL9601B_VLAN_SET(vid) \
do {\
    if ((vid) <= RTK_EXT_VLAN_ID_MAX) {vlan_valid[vid >> 5] |= (1 << (vid&31));}\
} while (0);\

#define DAL_RTL9601B_VLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_EXT_VLAN_ID_MAX) {vlan_valid[vid >> 5] &= (~(1 << (vid&31)));}\
} while (0);\

#define DAL_RTL9601B_VLAN_IS_SET(vid) \
    (((vid) <= RTK_EXT_VLAN_ID_MAX)?((vlan_valid[vid >> 5] >> (vid&31)) & 1): 0)


/*
 * Function Declaration
 */

static int32 _dal_rtl9601b_vlanProtoAndPortBasedEntry_set(rtl9601b_raw_protoValnEntryCfg_t *entry)
{
	uint32  tmpVal;
	int32   ret;
	/*parameter check*/
    RT_PARAM_CHK((entry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < entry->index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RTL9601B_PPVLAN_FRAME_TYPE_END <= entry->frameType), RT_ERR_INPUT);
    RT_PARAM_CHK((0xFFFF < entry->etherType), RT_ERR_INPUT);

    tmpVal = entry->frameType;
    if ((ret = reg_array_field_write(RTL9601B_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,RTL9601B_FRAME_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry->etherType;
    if ((ret = reg_array_field_write(RTL9601B_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,RTL9601B_ETHER_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_vlanProtoAndPortBasedEntry_get(rtl9601b_raw_protoValnEntryCfg_t *entry)
{
	uint32  tmpVal;
	int32   ret;
	/*parameter check*/
    RT_PARAM_CHK((entry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < entry->index), RT_ERR_ENTRY_INDEX);


    if ((ret = reg_array_field_read(RTL9601B_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,RTL9601B_FRAME_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    entry->frameType = tmpVal;

    if ((ret = reg_array_field_read(RTL9601B_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,RTL9601B_ETHER_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    entry->etherType = tmpVal;

    return RT_ERR_OK;
}

static int32 _dal_rtl9601b_vlan_protoAndPortBasedEntry_set(rtk_port_t port, rtl9601b_raw_protoVlanCfg_t *pPpbCfg)
{

	uint32 reg_value,set_val;
    uint32 index;
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((pPpbCfg  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < pPpbCfg->index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pPpbCfg->priority), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((pPpbCfg->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_ENTRY_NOT_FOUND);
    RT_PARAM_CHK((pPpbCfg->valid !=0 && pPpbCfg->valid !=1), RT_ERR_INPUT);

    index = pPpbCfg->index;
	reg_value = 0;

    /* Valid bit */
    set_val = pPpbCfg->valid;
    if ((ret = reg_field_set(RTL9601B_VLAN_PORT_PPB_VLANr,RTL9601B_VALIDf, &set_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* CVLAN index */
    set_val = pPpbCfg->vid;
    if ((ret = reg_field_set(RTL9601B_VLAN_PORT_PPB_VLANr,RTL9601B_PPB_VIDf, &set_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* priority */
    set_val = pPpbCfg->priority;
    if ((ret = reg_field_set(RTL9601B_VLAN_PORT_PPB_VLANr,RTL9601B_PPB_PRIf, &set_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_write(RTL9601B_VLAN_PORT_PPB_VLANr,port,index,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_vlan_portAndProtoBasedEntry_get(rtk_port_t port, rtl9601b_raw_protoVlanCfg_t *pPpbCfg)
{
	uint32 reg_value,get_val;
    uint32 index;
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((pPpbCfg  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < pPpbCfg->index), RT_ERR_ENTRY_INDEX);

    index = pPpbCfg->index;

    if ((ret = reg_array_read(RTL9601B_VLAN_PORT_PPB_VLANr,port,index,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

	/* Valid bit */
    if ((ret = reg_field_get(RTL9601B_VLAN_PORT_PPB_VLANr,RTL9601B_VALIDf, &get_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPpbCfg->valid = get_val;

	/* CVLAN id */
    if ((ret = reg_field_get(RTL9601B_VLAN_PORT_PPB_VLANr,RTL9601B_PPB_VIDf, &get_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPpbCfg->vid = get_val;


	/* priority */
    if ((ret = reg_field_get(RTL9601B_VLAN_PORT_PPB_VLANr,RTL9601B_PPB_PRIf, &get_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPpbCfg->priority = get_val;

    return RT_ERR_OK;

}



static int32 _dal_rtl9601b_vlan_acceptFrameType_set(rtk_port_t port, rtl9601b_raw_vlanAcpFrameType_t type)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTL9601B_VLAN_ACCEPT_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = type;

    if ((ret = reg_array_field_write(RTL9601B_VLAN_PORT_ACCEPT_FRAME_TYPEr, port , REG_ARRAY_INDEX_NONE, RTL9601B_FRAME_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32 _dal_rtl9601b_vlan_acceptFrameType_get(rtk_port_t port, rtl9601b_raw_vlanAcpFrameType_t *type)
{
	uint32  tmpVal;
	int32   ret;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type  == NULL), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9601B_VLAN_PORT_ACCEPT_FRAME_TYPEr, port , REG_ARRAY_INDEX_NONE, RTL9601B_FRAME_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    *type = tmpVal;

    return RT_ERR_OK;
}





static int32 _dal_rtl9601b_vlan_egrTagMode_set(rtk_port_t port, rtl9601b_raw_vlan_egrTagMode_t mode)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTL9601B_VLAN_EGR_TAG_MODE_END <= mode), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = mode;

    if ((ret = reg_array_field_write(RTL9601B_VLAN_EGRESS_TAGr, port , REG_ARRAY_INDEX_NONE, RTL9601B_EGRESS_MODEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_vlan_egrTagMode_get(rtk_port_t port, rtl9601b_raw_vlan_egrTagMode_t *pMode)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pMode==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);


    if ((ret = reg_array_field_read(RTL9601B_VLAN_EGRESS_TAGr, port , REG_ARRAY_INDEX_NONE, RTL9601B_EGRESS_MODEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if(tmpVal == 3)
       tmpVal = RTL9601B_VLAN_EGR_TAG_MODE_KEEP;
    *pMode = tmpVal;
    return RT_ERR_OK;
}



static int32 _dal_rtl9601b_vlan_igrFlterEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = enable;

    if ((ret = reg_array_field_write(RTL9601B_VLAN_INGRESSr, port , REG_ARRAY_INDEX_NONE, RTL9601B_INGRESSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32 _dal_rtl9601b_vlan_igrFlterEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(RTL9601B_VLAN_INGRESSr, port , REG_ARRAY_INDEX_NONE, RTL9601B_INGRESSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_vlan_egrIgrFilterEnable_set(rtk_enable_t enable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    tmpVal = enable;

    if ((ret = reg_field_write(RTL9601B_VLAN_CTRLr,RTL9601B_VLAN_FILTERINGf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}



static int32 _dal_rtl9601b_vlan_egrIgrFilterEnable_get(rtk_enable_t *pEnable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);


    if ((ret = reg_field_read(RTL9601B_VLAN_CTRLr,RTL9601B_VLAN_FILTERINGf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
}


static int32 _dal_rtl9601b_vlan_portBasedVID_set(rtk_port_t port, uint32 vid)
{
    uint32  ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    if(vid > RTK_VLAN_ID_MAX)
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    if ((ret = reg_array_field_write(RTL9601B_VLAN_PB_VIDr, port, REG_ARRAY_INDEX_NONE, RTL9601B_VIDf, &vid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of _dal_rtl9601b_vlan_portBasedVID_set*/

static int32 _dal_rtl9601b_vlan_portBasedVID_get(rtk_port_t port, uint32 *pVid)
{
    uint32  ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pVid), RT_ERR_NULL_POINTER);

    /* get pvid from CHIP*/
    if ((ret = reg_array_field_read(RTL9601B_VLAN_PB_VIDr, port, REG_ARRAY_INDEX_NONE, RTL9601B_VIDf, pVid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of _dal_rtl9601b_vlan_portBasedVID_get*/



static int32 _dal_rtl9601b_vlan_4kEntry_set(rtl9601b_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    rtl9601b_vlan_entry_t vlan_entry;
    RT_PARAM_CHK((NULL == pVlan4kEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlan4kEntry->mbr)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlan4kEntry->untag)), RT_ERR_PORT_MASK);


    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    /*MBR*/
    if ((ret = table_field_set(RTL9601B_VLANt, RTL9601B_VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_set(RTL9601B_VLANt, RTL9601B_VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_set(RTL9601B_VLANt, RTL9601B_VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    /*svlna hash mode*/
    if ((ret = table_field_set(RTL9601B_VLANt, RTL9601B_VLAN_ENSVLANHASHtf, (uint32 *)&pVlan4kEntry->svlanHash, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    if ((ret = table_write(RTL9601B_VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}
static int32 _dal_rtl9601b_vlan_4kEntry_get(rtl9601b_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    rtl9601b_vlan_entry_t vlan_entry;

	/*parameter check*/
    RT_PARAM_CHK((pVlan4kEntry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > RTL9601B_VIDMAX), RT_ERR_VLAN_VID);
    
    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    if ((ret = table_read(RTL9601B_VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*MBR*/
        /*get from vlan table*/
    if ((ret = table_field_get(RTL9601B_VLANt, RTL9601B_VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_get(RTL9601B_VLANt, RTL9601B_VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_get(RTL9601B_VLANt, RTL9601B_VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*SVLAN Hash*/
    if ((ret = table_field_get(RTL9601B_VLANt, RTL9601B_VLAN_ENSVLANHASHtf, (uint32 *)&pVlan4kEntry->svlanHash, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_vlan_existCheck
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
dal_rtl9601b_vlan_existCheck(rtk_vlan_t vid)
{
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

	if(DAL_RTL9601B_VLAN_IS_SET(vid))
	{
		return RT_ERR_OK;
	}
	return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_rtl9601b_vlan_init
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
dal_rtl9601b_vlan_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_portmask_t allPortMask;
    rtl9601b_vlan4kentry_t vlan;
    uint32 idx;
    rtk_vlan_protoGroup_t protoGroup;
    rtk_vlan_protoVlanCfg_t protoVlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    osal_memset(vlan_valid, 0x00, sizeof(uint32) * ((RTK_EXT_VLAN_ID_MAX + 1) / 32));
    vlan_init = INIT_COMPLETED;

    /* Reset VLAN Table */
    if((ret = dal_rtl9601b_vlan_destroyAll(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* VLAN filter */
    if((ret = _dal_rtl9601b_vlan_egrIgrFilterEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        /* VLAN Ingress filter */
        if((ret = _dal_rtl9601b_vlan_igrFlterEnable_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /* Egress Tag Mode */
        if((ret = _dal_rtl9601b_vlan_egrTagMode_set(port, RTL9601B_VLAN_EGR_TAG_MODE_ORI)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    /* Create Default VLAN */
    osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
    vlan.vid = DAL_RTL9601B_DEFAULT_VLAN_ID;

    HAL_GET_ALL_PORTMASK(allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.mbr, allPortMask);
    RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
    if((ret = _dal_rtl9601b_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Database update */
    DAL_RTL9601B_VLAN_SET(DAL_RTL9601B_DEFAULT_VLAN_ID)

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_rtl9601b_vlan_portAcceptFrameType_set(port, ACCEPT_FRAME_TYPE_ALL )) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_rtl9601b_vlan_portIgrFilterEnable_set(port, ENABLED )) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
        
        /*set all port port based VID to 1*/
        if((ret = dal_rtl9601b_vlan_portPvid_set(port, 1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

    }

    if((ret = dal_rtl9601b_vlan_vlanFunctionEnable_set(ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    osal_memset(&protoGroup, 0x00, sizeof(rtk_vlan_protoGroup_t));
    for(idx = 0; idx <= HAL_PROTOCOL_VLAN_IDX_MAX(); idx++)
    {
        if((ret = dal_rtl9601b_vlan_protoGroup_set(idx, &protoGroup)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            vlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    osal_memset(&protoVlan, 0x00, sizeof(rtk_vlan_protoVlanCfg_t));
    protoVlan.vid = DAL_RTL9601B_DEFAULT_VLAN_ID;
    HAL_SCAN_ALL_PORT(port)
    {
    	for(idx = 0; idx <= HAL_PROTOCOL_VLAN_IDX_MAX(); idx++)
    	{
	        if((ret = dal_rtl9601b_vlan_portProtoVlan_set(port, idx, &protoVlan)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
	            vlan_init = INIT_NOT_COMPLETED;

	            return ret;
	        }
    	}
    }


    if((ret = dal_rtl9601b_vlan_cfiKeepEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl9601b_vlan_reservedVidAction_set(RESVID_ACTION_UNTAG, RESVID_ACTION_UNTAG)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        vlan_init = INIT_NOT_COMPLETED;
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_init */

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      dal_rtl9601b_vlan_create
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
dal_rtl9601b_vlan_create(rtk_vlan_t vid)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((DAL_RTL9601B_VLAN_IS_SET(vid)), RT_ERR_VLAN_EXIST);

    DAL_RTL9601B_VLAN_SET(vid);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_create */

/* Function Name:
 *      dal_rtl9601b_vlan_destroy
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
dal_rtl9601b_vlan_destroy(rtk_vlan_t vid)
{
    int32 ret;
    rtl9601b_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((!DAL_RTL9601B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    /* Clear 4K Entry */
    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
        vlan.vid = vid;
        if((ret = _dal_rtl9601b_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }

    /* Clear Database */
    DAL_RTL9601B_VLAN_CLEAR(vid);
	return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_destroy */


/* Function Name:
 *      dal_rtl9601b_vlan_destroyAll
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
dal_rtl9601b_vlan_destroyAll(uint32 restoreDefaultVlan)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;
    rtk_port_t port;
    rtk_portmask_t allPortMask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= restoreDefaultVlan), RT_ERR_INPUT);

    /* Clear 4K VLAN */
#if 0
    if((ret = rtl9601b_l2_table_clear(RTL9601B_VLANt, RTK_VLAN_ID_MIN, RTK_VLAN_ID_MAX)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
		return ret;
    }
#endif

    /* Update database */
    osal_memset(vlan_valid, 0x00, sizeof(uint32) * (RTK_EXT_VLAN_ID_MAX + 1) / 32);

    if(ENABLED == restoreDefaultVlan)
    {
        /* Create Default VLAN */
        osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
        vlan.vid = DAL_RTL9601B_DEFAULT_VLAN_ID;

        HAL_GET_ALL_PORTMASK(allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.mbr, allPortMask);
        RTK_PORTMASK_ASSIGN(vlan.untag, allPortMask);
        if((ret = _dal_rtl9601b_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        /* Database update */
        DAL_RTL9601B_VLAN_SET(DAL_RTL9601B_DEFAULT_VLAN_ID)
    }

    /* Set PVID */
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = _dal_rtl9601b_vlan_portBasedVID_set(port, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_destroyAll */


/* Function Name:
 *      dal_rtl9601b_vlan_fidMode_get
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
 *      mode can be: -VLAN_FID_IVL
 *                   -VLAN_FID_SVL
 */
int32
dal_rtl9601b_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9601B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = _dal_rtl9601b_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        *pMode = (rtk_fidMode_t)((RTL9601B_VLAN_HASH_IVL == vlan.ivl_svl) ? VLAN_FID_IVL : VLAN_FID_SVL);
    }
    else
    {
        *pMode = VLAN_FID_SVL;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_fidMode_get */

/* Function Name:
 *      dal_rtl9601b_vlan_fidMode_set
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
dal_rtl9601b_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_FID_MODE_END <= mode), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_RTL9601B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);


    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = _dal_rtl9601b_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        vlan.ivl_svl = (rtl9601b_vlan_l2HashType_t)((VLAN_FID_IVL == mode) ? RTL9601B_VLAN_HASH_IVL : RTL9601B_VLAN_HASH_SVL);

        if ((ret = _dal_rtl9601b_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if(mode == VLAN_FID_IVL)
        {
            return RT_ERR_INPUT;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_fidMode_set */

/* Function Name:
 *      dal_rtl9601b_vlan_port_get
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
dal_rtl9601b_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMemberPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntagPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_RTL9601B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = _dal_rtl9601b_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        RTK_PORTMASK_ASSIGN((*pMemberPortmask), vlan.mbr);
        RTK_PORTMASK_ASSIGN((*pUntagPortmask), vlan.untag);
    }
    else
    {
        return RT_ERR_VLAN_VID;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_port_get */


/* Function Name:
 *      dal_rtl9601b_vlan_port_set
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
dal_rtl9601b_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < vid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMember_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pUntag_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pMember_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pUntag_portmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((!DAL_RTL9601B_VLAN_IS_SET(vid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    /* VID > 4K, no untag_portmask */
    if( (vid > RTK_VLAN_ID_MAX) && (RTK_PORTMASK_GET_PORT_COUNT((*pUntag_portmask)) != 0) )
        return RT_ERR_PORT_MASK;

    if(vid <= RTK_VLAN_ID_MAX)
    {
        osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
        vlan.vid = vid;
        if ((ret = _dal_rtl9601b_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }

        RTK_PORTMASK_ASSIGN(vlan.mbr, (*pMember_portmask));
        RTK_PORTMASK_ASSIGN(vlan.untag, (*pUntag_portmask));

        if ((ret = _dal_rtl9601b_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        return RT_ERR_VLAN_VID;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_port_set */


/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      dal_rtl9601b_vlan_portAcceptFrameType_get
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
dal_rtl9601b_vlan_portAcceptFrameType_get(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAcceptFrameType)
{
    int32   ret;
    rtl9601b_raw_vlanAcpFrameType_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAcceptFrameType), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9601b_vlan_acceptFrameType_get(port, &type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    switch(type)
    {
        case RTL9601B_VLAN_ACCEPT_ALL:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_ALL;
            break;
        case RTL9601B_VLAN_ACCEPT_TAGGED_ONLY:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_TAG_ONLY;
            break;
        case RTL9601B_VLAN_ACCEPT_UNTAGGED_ONLY:
            *pAcceptFrameType = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
            break;
        case RTL9601B_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY:
            *pAcceptFrameType = ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portAcceptFrameType_get */


/* Function Name:
 *      dal_rtl9601b_vlan_portAcceptFrameType_set
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
dal_rtl9601b_vlan_portAcceptFrameType_set(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t acceptFrameType)
{
    int32   ret;
    rtl9601b_raw_vlanAcpFrameType_t type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= acceptFrameType), RT_ERR_INPUT);

    switch(acceptFrameType)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            type = RTL9601B_VLAN_ACCEPT_ALL;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            type = RTL9601B_VLAN_ACCEPT_TAGGED_ONLY;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            type = RTL9601B_VLAN_ACCEPT_UNTAGGED_ONLY;
            break;
        case ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY:
            type = RTL9601B_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if ((ret = _dal_rtl9601b_vlan_acceptFrameType_set(port, type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portAcceptFrameType_set */

/* Function Name:
 *      dal_rtl9601b_vlan_vlanFunctionEnable_get
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
dal_rtl9601b_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable)
{
   int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9601b_vlan_egrIgrFilterEnable_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_vlanFunctionEnable_get */


/* Function Name:
 *      dal_rtl9601b_vlan_vlanFunctionEnable_set
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
dal_rtl9601b_vlan_vlanFunctionEnable_set(rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = _dal_rtl9601b_vlan_egrIgrFilterEnable_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_vlanFunctionEnable_set */

/* Module Name    : Vlan                       */
/* Sub-module Name: Vlan ingress/egress filter */

/* Function Name:
 *      dal_rtl9601b_vlan_portIgrFilterEnable_get
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
dal_rtl9601b_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9601b_vlan_igrFlterEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portIgrFilterEnable_get */


/* Function Name:
 *      dal_rtl9601b_vlan_portIgrFilterEnable_set
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
dal_rtl9601b_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = _dal_rtl9601b_vlan_igrFlterEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portIgrFilterEnable_set */

/* Function Name:
 *      dal_rtl9601b_vlan_keepType_get
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
dal_rtl9601b_vlan_keepType_get(rtk_vlan_keep_type_t type, rtk_enable_t *pEnable)
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
            regAddr = RTL9601B_RMA_CTRL00r;
            break;
        case KEEP_FD_PAUSE:
            regAddr = RTL9601B_RMA_CTRL01r;
            break;
        case KEEP_SP_MCAST:
            regAddr = RTL9601B_RMA_CTRL02r;
            break;
        case KEEP_1X_PAE:
            regAddr = RTL9601B_RMA_CTRL03r;
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
            regAddr = RTL9601B_RMA_CTRL04r;
            break;
        case KEEP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            regAddr = RTL9601B_RMA_CTRL08r;
            break;
        case KEEP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            regAddr = RTL9601B_RMA_CTRL0Dr;
            break;
        case KEEP_8021AB:
            regAddr = RTL9601B_RMA_CTRL0Er;
            break;
        case KEEP_BRG_MNGEMENT:
            regAddr = RTL9601B_RMA_CTRL10r;
            break;
        case KEEP_UNDEFINED_11:
            regAddr = RTL9601B_RMA_CTRL11r;
            break;
        case KEEP_UNDEFINED_12:
            regAddr = RTL9601B_RMA_CTRL12r;
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
            regAddr = RTL9601B_RMA_CTRL13r;
            break;
        case KEEP_UNDEFINED_18:
            regAddr = RTL9601B_RMA_CTRL18r;
            break;
        case KEEP_UNDEFINED_1A:
            regAddr = RTL9601B_RMA_CTRL1Ar;
            break;
        case KEEP_GMRP:
            regAddr = RTL9601B_RMA_CTRL20r;
            break;
        case KEEP_GVRP:
            regAddr = RTL9601B_RMA_CTRL21r;
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
            regAddr = RTL9601B_RMA_CTRL22r;
            break;
        case KEEP_CDP:
            regAddr = RTL9601B_RMA_CTRL_CDPr;
            break;
        case KEEP_SSTP:
            regAddr = RTL9601B_RMA_CTRL_SSTPr;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_read(regAddr, RTL9601B_KEEP_FORMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_keepType_get */

/* Function Name:
 *      dal_rtl9601b_vlan_keepType_set
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
dal_rtl9601b_vlan_keepType_set(rtk_vlan_keep_type_t type, rtk_enable_t enable)
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
            regAddr = RTL9601B_RMA_CTRL00r;
            break;
        case KEEP_FD_PAUSE:
            regAddr = RTL9601B_RMA_CTRL01r;
            break;
        case KEEP_SP_MCAST:
            regAddr = RTL9601B_RMA_CTRL02r;
            break;
        case KEEP_1X_PAE:
            regAddr = RTL9601B_RMA_CTRL03r;
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
            regAddr = RTL9601B_RMA_CTRL04r;
            break;
        case KEEP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            regAddr = RTL9601B_RMA_CTRL08r;
            break;
        case KEEP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            regAddr = RTL9601B_RMA_CTRL0Dr;
            break;
        case KEEP_8021AB:
            regAddr = RTL9601B_RMA_CTRL0Er;
            break;
        case KEEP_BRG_MNGEMENT:
            regAddr = RTL9601B_RMA_CTRL10r;
            break;
        case KEEP_UNDEFINED_11:
            regAddr = RTL9601B_RMA_CTRL11r;
            break;
        case KEEP_UNDEFINED_12:
            regAddr = RTL9601B_RMA_CTRL12r;
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
            regAddr = RTL9601B_RMA_CTRL13r;
            break;
        case KEEP_UNDEFINED_18:
            regAddr = RTL9601B_RMA_CTRL18r;
            break;
        case KEEP_UNDEFINED_1A:
            regAddr = RTL9601B_RMA_CTRL1Ar;
            break;
        case KEEP_GMRP:
            regAddr = RTL9601B_RMA_CTRL20r;
            break;
        case KEEP_GVRP:
            regAddr = RTL9601B_RMA_CTRL21r;
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
            regAddr = RTL9601B_RMA_CTRL22r;
            break;
        case KEEP_CDP:
            regAddr = RTL9601B_RMA_CTRL_CDPr;
            break;
        case KEEP_SSTP:
            regAddr = RTL9601B_RMA_CTRL_SSTPr;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    val = (uint32)enable;
    if ((ret = reg_field_write(regAddr, RTL9601B_KEEP_FORMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_keepType_set */

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      dal_rtl9601b_vlan_portPvid_get
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
dal_rtl9601b_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid)
{
    int32  ret;
    uint32 vid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    if((ret = _dal_rtl9601b_vlan_portBasedVID_get(port, &vid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    *pPvid = vid;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portPvid_get */


/* Function Name:
 *      dal_rtl9601b_vlan_portPvid_set
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
dal_rtl9601b_vlan_portPvid_set(rtk_port_t port, uint32 pvid)
{
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_EXT_VLAN_ID_MAX < pvid), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_RTL9601B_VLAN_IS_SET(pvid)), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    if((ret = _dal_rtl9601b_vlan_portBasedVID_set(port, pvid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portPvid_set */


/* Function Name:
 *      dal_rtl9601b_vlan_protoGroup_get
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
dal_rtl9601b_vlan_protoGroup_get(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    rtl9601b_raw_protoValnEntryCfg_t entry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);

    entry.index = protoGroupIdx;
    if((ret = _dal_rtl9601b_vlanProtoAndPortBasedEntry_get(&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    switch(entry.frameType)
    {
        case RTL9601B_PPVLAN_FRAME_TYPE_ETHERNET:
            pProtoGroup->frametype = FRAME_TYPE_ETHERNET;
            break;
        case RTL9601B_PPVLAN_FRAME_TYPE_LLC:
            pProtoGroup->frametype = FRAME_TYPE_LLCOTHER;
            break;
        case RTL9601B_PPVLAN_FRAME_TYPE_RFC1042:
            pProtoGroup->frametype = FRAME_TYPE_RFC1042;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    pProtoGroup->framevalue = entry.etherType;
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_protoGroup_get */

/* Function Name:
 *      dal_rtl9601b_vlan_protoGroup_set
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
dal_rtl9601b_vlan_protoGroup_set(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    rtl9601b_raw_protoValnEntryCfg_t entry;

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
            entry.frameType = RTL9601B_PPVLAN_FRAME_TYPE_ETHERNET;
            break;
        case FRAME_TYPE_LLCOTHER:
            entry.frameType = RTL9601B_PPVLAN_FRAME_TYPE_LLC;
            break;
        case FRAME_TYPE_RFC1042:
            entry.frameType = RTL9601B_PPVLAN_FRAME_TYPE_RFC1042;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = _dal_rtl9601b_vlanProtoAndPortBasedEntry_set(&entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_protoGroup_set */

/* Function Name:
 *      dal_rtl9601b_vlan_portProtoVlan_get
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
dal_rtl9601b_vlan_portProtoVlan_get(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    rtl9601b_raw_protoVlanCfg_t cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < protoGroupIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanCfg), RT_ERR_NULL_POINTER);

    cfg.index = protoGroupIdx;
    if((ret = _dal_rtl9601b_vlan_portAndProtoBasedEntry_get(port, &cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    pVlanCfg->valid = cfg.valid;
    pVlanCfg->dei   = 0;
    pVlanCfg->vid   = cfg.vid;
    pVlanCfg->pri   = cfg.priority;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portProtoVlan_get */

/* Function Name:
 *      dal_rtl9601b_vlan_portProtoVlan_set
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
dal_rtl9601b_vlan_portProtoVlan_set(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    rtl9601b_raw_protoVlanCfg_t cfg;

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

    if((ret = _dal_rtl9601b_vlan_protoAndPortBasedEntry_set(port, &cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_portProtoVlan_set */

/* Module Name    : Vlan                */
/* Sub-module Name: Tag format handling */

/* Function Name:
 *      dal_rtl9601b_vlan_tagMode_get
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
dal_rtl9601b_vlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTagMode)
{
    int32   ret;
    rtl9601b_raw_vlan_egrTagMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTagMode), RT_ERR_NULL_POINTER);

    if((ret = _dal_rtl9601b_vlan_egrTagMode_get(port, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    switch (mode)
    {
        case RTL9601B_VLAN_EGR_TAG_MODE_ORI:
            *pTagMode = VLAN_TAG_MODE_ORIGINAL;
            break;
        case RTL9601B_VLAN_EGR_TAG_MODE_KEEP:
            *pTagMode = VLAN_TAG_MODE_KEEP_FORMAT;
            break;
        case RTL9601B_VLAN_EGR_TAG_MODE_PRI_TAG:
            *pTagMode = VLAN_TAG_MODE_PRI;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_tagMode_get */


/* Function Name:
 *      dal_rtl9601b_vlan_tagMode_set
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
dal_rtl9601b_vlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tagMode)
{
    int32   ret;
    rtl9601b_raw_vlan_egrTagMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TAG_MODE_END <= tagMode), RT_ERR_INPUT);

    switch (tagMode)
    {
        case VLAN_TAG_MODE_ORIGINAL:
            mode = RTL9601B_VLAN_EGR_TAG_MODE_ORI;
            break;
        case VLAN_TAG_MODE_KEEP_FORMAT:
            mode = RTL9601B_VLAN_EGR_TAG_MODE_KEEP;
            break;
        case VLAN_TAG_MODE_PRI:
            mode = RTL9601B_VLAN_EGR_TAG_MODE_PRI_TAG;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    if((ret = _dal_rtl9601b_vlan_egrTagMode_set(port, mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_tagMode_set */



/* Function Name:
 *      dal_rtl9601b_vlan_cfiKeepEnable_get
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
dal_rtl9601b_vlan_cfiKeepEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    rtl9601b_raw_vlan_cfiKeepMode_t mode;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_VLAN_CTRLr,RTL9601B_CFI_KEEPf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    mode = tmpVal;

    *pEnable = ((RTL9601B_CFI_KEEP_INGRESS == mode) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_cfiKeepEnable_get */

/* Function Name:
 *      dal_rtl9601b_vlan_cfiKeepEnable_set
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
dal_rtl9601b_vlan_cfiKeepEnable_set(rtk_enable_t enable)
{
    int32   ret;
    rtl9601b_raw_vlan_cfiKeepMode_t mode;
    uint32 tmpVal;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    mode = ((ENABLED == enable) ? RTL9601B_CFI_KEEP_INGRESS : RTL9601B_CFI_ALWAYS_0);
    tmpVal = mode;
    if ((ret = reg_field_write(RTL9601B_VLAN_CTRLr,RTL9601B_CFI_KEEPf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_cfiKeepEnable_set */

/* Function Name:
 *      dal_rtl9601b_vlan_reservedVidAction_get
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
dal_rtl9601b_vlan_reservedVidAction_get(rtk_vlan_resVidAction_t *pAction_vid0, rtk_vlan_resVidAction_t *pAction_vid4095)
{
    int32   ret;
    rtl9601b_raw_vlanTag_t vid0_type;
    rtl9601b_raw_vlanTag_t vid4095_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction_vid0), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAction_vid4095), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(RTL9601B_VLAN_CTRLr,RTL9601B_VID_0_TYPEf, (uint32 *)&vid0_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_VLAN_CTRLr,RTL9601B_VID_4095_TYPEf, (uint32 *)&vid4095_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pAction_vid0 = ((RTL9601B_VLAN_UNTAG == vid0_type) ? RESVID_ACTION_UNTAG : RESVID_ACTION_TAG);
    *pAction_vid4095 = ((RTL9601B_VLAN_UNTAG == vid4095_type) ? RESVID_ACTION_UNTAG : RESVID_ACTION_TAG);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_reservedVidAction_get */

/* Function Name:
 *      dal_rtl9601b_vlan_reservedVidAction_set
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
dal_rtl9601b_vlan_reservedVidAction_set(rtk_vlan_resVidAction_t action_vid0, rtk_vlan_resVidAction_t action_vid4095)
{
    int32   ret;
    rtl9601b_raw_vlanTag_t vid0_type;
    rtl9601b_raw_vlanTag_t vid4095_type;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((action_vid0 >= RESVID_ACTION_END), RT_ERR_INPUT);
    RT_PARAM_CHK((action_vid4095 >= RESVID_ACTION_END), RT_ERR_INPUT);

    vid0_type = ((RESVID_ACTION_UNTAG == action_vid0) ? RTL9601B_VLAN_UNTAG : RTL9601B_VLAN_TAG);
    vid4095_type = ((RESVID_ACTION_UNTAG == action_vid4095) ? RTL9601B_VLAN_UNTAG : RTL9601B_VLAN_TAG);

    if ((ret = reg_field_write(RTL9601B_VLAN_CTRLr,RTL9601B_VID_0_TYPEf, (uint32 *)&vid0_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_VLAN_CTRLr,RTL9601B_VID_4095_TYPEf, (uint32 *)&vid4095_type)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_vlan_reservedVidAction_set */


/* Function Name:
 *      dal_rtl9601b_vlan_lutSvlanHashState_set
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
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9601b_vlan_lutSvlanHashState_set(
    rtk_vlan_t     vid,
    rtk_enable_t enable)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;
   
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "vid=%d,enable=%d",vid, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);


    osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9601b_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    vlan.svlanHash = enable;

    if ((ret = _dal_rtl9601b_vlan_4kEntry_set(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_vlan_lutSvlanHashState_set */


/* Function Name:
 *      dal_rtl9601b_vlan_lutSvlanHashState_get
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
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9601b_vlan_lutSvlanHashState_get(
    rtk_vlan_t     vid,
    rtk_enable_t *pEnable)
{
    int32   ret;
    rtl9601b_vlan4kentry_t vlan;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "vid=%d",vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&vlan, 0x00, sizeof(rtl9601b_vlan4kentry_t));
    vlan.vid = vid;
    if ((ret = _dal_rtl9601b_vlan_4kEntry_get(&vlan)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN | MOD_DAL), "");
        return ret;
    }
    *pEnable = vlan.svlanHash;
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_vlan_lutSvlanHashState_get */
