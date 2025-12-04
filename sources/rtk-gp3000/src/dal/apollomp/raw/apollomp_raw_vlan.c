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
 * Purpose : switch asic-level VLAN API
 * Feature : VLAN related functions
 *
 */

#include <dal/apollomp/raw/apollomp_raw_vlan.h>


#define APOLLOMP_REG_VLAN_MBR_CFG_WORD    2

/* Function Name:
 *      apollomp_raw_vlan_memberConfig_set
 * Description:
 *      Set 32 VLAN member configurations
 * Input:
 *      pVlanCg - VLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 *      RT_ERR_L2_FID  				- Invalid FID
 *      RT_ERR_PORT_MASK  			- Invalid portmask
 *      RT_ERR_FILTER_METER_ID  	- Invalid meter
 *      RT_ERR_QOS_INT_PRIORITY  	- Invalid priority
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - Invalid VLAN member configuration index
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_memberConfig_set(apollomp_raw_vlanconfig_t *pVlanCg)
{
    uint32       vlan_mbr_cfg[APOLLOMP_REG_VLAN_MBR_CFG_WORD];
    int32        ret;
    uint32       set_val;
    uint32       index;


    /* Error Checking  */
    RT_PARAM_CHK((NULL == pVlanCg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlanCg->mbr)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!(HAL_IS_EXTPORTMASK_VALID(pVlanCg->exMbr)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pVlanCg->envlanpol >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pVlanCg->vbpen >= RTK_ENABLE_END), RT_ERR_INPUT);

    index = pVlanCg->index;

	if(index > APOLLOMP_CVIDXMAX)
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    if(pVlanCg->evid > APOLLOMP_EVIDMAX)
        return RT_ERR_INPUT;

    if(pVlanCg->fid_msti > HAL_VLAN_FID_MAX())
        return RT_ERR_L2_FID;

    if(pVlanCg->meteridx >= HAL_MAX_NUM_OF_METERING())
        return RT_ERR_FILTER_METER_ID;

    if(pVlanCg->vbpri > HAL_INTERNAL_PRIORITY_MAX())
        return RT_ERR_QOS_INT_PRIORITY;

    /*DSL_MBR EXT_MBR check TBD!!!*/

    osal_memset(vlan_mbr_cfg, 0, sizeof(vlan_mbr_cfg));
    /*set field data to entry*/
    /*EVID*/
    set_val = pVlanCg->evid;
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_EVIDf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*MBR*/
    set_val = pVlanCg->mbr.bits[0];
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_MBRf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*METERIDX*/
    set_val = pVlanCg->meteridx;
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_METERIDXf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*ENVLANPOL*/
    set_val = pVlanCg->envlanpol;
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_ENVLANPOLf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*EXT_MBR*/
    set_val = pVlanCg->exMbr.bits[0];
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_EXT_MBRf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*FID_MSTI*/
    set_val = pVlanCg->fid_msti;
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_FID_MSTIf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*VBPEN*/
    set_val = pVlanCg->vbpen;
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_VBPENf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*VBPRI*/
    set_val = pVlanCg->vbpri;
    if ((ret = reg_field_set(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_VBPRIf, &set_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    /* write entry to VLAN_MBR_CFG by index*/
    if ((ret = reg_array_write(APOLLOMP_VLAN_MBR_CFGr,REG_ARRAY_INDEX_NONE,index,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }



    return RT_ERR_OK;
}/*end of apollomp_raw_vlan_memberConfig_set*/


/* Function Name:
 *      apollomp_raw_vlan_memberConfig_get
 * Description:
 *      Get 32 VLAN member configurations
 * Input:
 *      pVlanCg - VLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - Invalid VLAN member configuration index
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_memberConfig_get(apollomp_raw_vlanconfig_t *pVlanCg)
{
    uint32       vlan_mbr_cfg[APOLLOMP_REG_VLAN_MBR_CFG_WORD];
    int32        ret;
    uint32       get_val;
    uint32       index;

    RT_PARAM_CHK((NULL == pVlanCg), RT_ERR_NULL_POINTER);
    index = pVlanCg->index;
    RT_PARAM_CHK((index > APOLLOMP_CVIDXMAX), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(vlan_mbr_cfg, 0, sizeof(vlan_mbr_cfg));

    /* get entry from VLAN_MBR_CFG by index*/
    if ((ret = reg_array_read(APOLLOMP_VLAN_MBR_CFGr,REG_ARRAY_INDEX_NONE,index,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    /*get field data from entry*/
    /*EVID*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_EVIDf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->evid = get_val;

    /*MBR*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_MBRf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->mbr.bits[0] = get_val;

    /*EXT_MBR*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_EXT_MBRf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->exMbr.bits[0] = get_val;

    /*METERIDX*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_METERIDXf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->meteridx = get_val;

    /*ENVLANPOL*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_ENVLANPOLf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->envlanpol = get_val;

    /*VBPRI*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_VBPRIf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->vbpri = get_val;

    /*VBPEN*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_VBPENf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->vbpen = get_val;

    /*FID_MSTI*/
    if ((ret = reg_field_get(APOLLOMP_VLAN_MBR_CFGr,APOLLOMP_FID_MSTIf, &get_val,vlan_mbr_cfg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlanCg->fid_msti = get_val;


    return RT_ERR_OK;
}/*end of apollomp_raw_vlan_memberConfig_get*/





 /* Function Name:
 *      apollomp_raw_vlan_4kEntry_set
 * Description:
 *      Set VID mapped entry to 4K VLAN table
 * Input:
 *      pVlan4kEntry - 4K VLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 *      RT_ERR_L2_FID  				- Invalid FID
 *      RT_ERR_VLAN_VID 			- Invalid VID parameter (0~4095)
 *      RT_ERR_PORT_MASK  			- Invalid portmask
 *      RT_ERR_FILTER_METER_ID  	- Invalid meter
 *      RT_ERR_QOS_INT_PRIORITY  	- Invalid priority
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_4kEntry_set(apollomp_raw_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    apollomp_vlan_entry_t vlan_entry;
    RT_PARAM_CHK((NULL == pVlan4kEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlan4kEntry->mbr)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!(HAL_IS_PORTMASK_VALID(pVlan4kEntry->untag)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!(HAL_IS_EXTPORTMASK_VALID(pVlan4kEntry->exMbr)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK((pVlan4kEntry->fid_msti > HAL_VLAN_FID_MAX()), RT_ERR_L2_FID);
    RT_PARAM_CHK((pVlan4kEntry->meteridx >= HAL_MAX_NUM_OF_METERING()), RT_ERR_FILTER_METER_ID);
    RT_PARAM_CHK((pVlan4kEntry->vbpri > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((pVlan4kEntry->envlanpol >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pVlan4kEntry->vbpen >= RTK_ENABLE_END), RT_ERR_INPUT);


    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    /*MBR*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*EXT_MBR*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_EXT_MBRtf, (uint32 *)&pVlan4kEntry->exMbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*FID_MSTI*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_FID_MSTItf, (uint32 *)&pVlan4kEntry->fid_msti, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*METERIDX*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_METERIDXtf, (uint32 *)&pVlan4kEntry->meteridx, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*POLICING*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_POLICINGtf, (uint32 *)&pVlan4kEntry->envlanpol, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*VB_EN*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_VB_ENtf, (uint32 *)&pVlan4kEntry->vbpen, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*VBPRI*/
    if ((ret = table_field_set(APOLLOMP_VLANt, APOLLOMP_VLAN_VBPRItf, (uint32 *)&pVlan4kEntry->vbpri, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_vlan_4kEntry_get
 * Description:
 *      Get VID mapped entry to 4K VLAN table
 * Input:
 *      pVlan4kEntry - 4K VLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_VLAN_VID 		- Invalid VID parameter (0~4095)
 *      RT_ERR_BUSYWAIT_TIMEOUT - LUT is busy at retrieving
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_4kEntry_get(apollomp_raw_vlan4kentry_t *pVlan4kEntry )
{
	int32 ret;
    apollomp_vlan_entry_t vlan_entry;

	/*parameter check*/
    RT_PARAM_CHK((pVlan4kEntry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pVlan4kEntry->vid > APOLLOMP_VIDMAX), RT_ERR_VLAN_VID);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    if ((ret = table_read(APOLLOMP_VLANt, pVlan4kEntry->vid, (uint32 *)&vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    /*MBR*/
        /*get from vlan table*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_MBRtf, (uint32 *)&pVlan4kEntry->mbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*UNTAG*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_UNTAGtf, (uint32 *)&pVlan4kEntry->untag.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*EXT_MBR*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_EXT_MBRtf, (uint32 *)&pVlan4kEntry->exMbr.bits[0], (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*IVL_SVL*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_IVL_SVLtf, (uint32 *)&pVlan4kEntry->ivl_svl, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*FID_MSTI*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_FID_MSTItf, (uint32 *)&pVlan4kEntry->fid_msti, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*METERIDX*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_METERIDXtf, (uint32 *)&pVlan4kEntry->meteridx, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*POLICING*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_POLICINGtf, (uint32 *)&pVlan4kEntry->envlanpol, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*VB_EN*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_VB_ENtf, (uint32 *)&pVlan4kEntry->vbpen, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    /*VBPRI*/
    if ((ret = table_field_get(APOLLOMP_VLANt, APOLLOMP_VLAN_VBPRItf, (uint32 *)&pVlan4kEntry->vbpri, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_portBasedVID_set
 * Description:
 *      Set port based VID which is indexed to 32 VLAN member configurations
 * Input:
 *      port 	- Physical port number (0~7)
 *      index 	- Index to VLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_PORT_ID  			- Invalid port number
 *      RT_ERR_QOS_INT_PRIORITY  	- Invalid priority
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - Invalid VLAN member configuration index
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portBasedVID_set(rtk_port_t port, uint32 index)
{
    uint32  ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    if(index > APOLLOMP_CVIDXMAX)
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PB_VIDXr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_VIDXf, &index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of apollomp_raw_vlan_portBasedVID_set*/


/* Function Name:
 *      apollomp_raw_vlan_portBasedVID_get
 * Description:
 *      Get port based VID which is indexed to 32 VLAN member configurations
 * Input:
 *      port 	- Physical port number (0~7)
 *      pIndex 	- Index to VLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portBasedVID_get(rtk_port_t port, uint32 *pIndex)
{
    uint32  ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* get pvid index entry from CHIP*/
    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PB_VIDXr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_VIDXf, pIndex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}/*end of apollomp_raw_vlan_portBasedVID_get*/



/* Function Name:
 *      apollomp_raw_vlan_protoAndPortBasedEntry
 * Description:
 *      Set protocol and port based VLAN configuration
 * Input:
 *      port 		- Physical port number (0~7)
 *      pPpbCfg 	- Protocol and port based VLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 *      RT_ERR_PORT_ID  			- Invalid port number
 *      RT_ERR_QOS_INT_PRIORITY  	- Invalid priority
 *      RT_ERR_VLAN_PROTO_AND_PORT  - Invalid protocol base group database index
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - Invalid VLAN member configuration index
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_protoAndPortBasedEntry_set(rtk_port_t port, apollomp_raw_protoVlanCfg_t *pPpbCfg)
{

	uint32 reg_value,set_val;
    uint32 index;
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((pPpbCfg  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < pPpbCfg->index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < pPpbCfg->priority), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((pPpbCfg->vlan_idx > HAL_MAX_VLAN_MBRCFG_IDX()), RT_ERR_VLAN_ENTRY_NOT_FOUND);
    RT_PARAM_CHK((pPpbCfg->valid !=0 && pPpbCfg->valid !=1), RT_ERR_INPUT);

    index = pPpbCfg->index;
	reg_value = 0;
    /* Valid bit */
    set_val = pPpbCfg->valid;
    if ((ret = reg_field_set(APOLLOMP_VLAN_PORT_PPB_VLANr,APOLLOMP_VALIDf, &set_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* CVLAN index */
    set_val = pPpbCfg->vlan_idx;
    if ((ret = reg_field_set(APOLLOMP_VLAN_PORT_PPB_VLANr,APOLLOMP_PPB_VIDXf, &set_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* priority */
    set_val = pPpbCfg->priority;
    if ((ret = reg_field_set(APOLLOMP_VLAN_PORT_PPB_VLANr,APOLLOMP_PPB_PRIf, &set_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_write(APOLLOMP_VLAN_PORT_PPB_VLANr,port,index,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_vlan_portAndProtoBasedEntry_get
 * Description:
 *      Get protocol and port based VLAN configuration
 * Input:
 *      port 		- Physical port number (0~7)
 *      pPpbCfg 	- Protocol and port based VLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 *      RT_ERR_PORT_ID  			- Invalid port number
 *      RT_ERR_VLAN_PROTO_AND_PORT  - Invalid protocol base group database index
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portAndProtoBasedEntry_get(rtk_port_t port, apollomp_raw_protoVlanCfg_t *pPpbCfg)
{
	uint32 reg_value,get_val;
    uint32 index;
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((pPpbCfg  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < pPpbCfg->index), RT_ERR_ENTRY_INDEX);

    index = pPpbCfg->index;

    if ((ret = reg_array_read(APOLLOMP_VLAN_PORT_PPB_VLANr,port,index,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

	/* Valid bit */
    if ((ret = reg_field_get(APOLLOMP_VLAN_PORT_PPB_VLANr,APOLLOMP_VALIDf, &get_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPpbCfg->valid = get_val;

	/* CVLAN id */
    if ((ret = reg_field_get(APOLLOMP_VLAN_PORT_PPB_VLANr,APOLLOMP_PPB_VIDXf, &get_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPpbCfg->vlan_idx = get_val;


	/* priority */
    if ((ret = reg_field_get(APOLLOMP_VLAN_PORT_PPB_VLANr,APOLLOMP_PPB_PRIf, &get_val,&reg_value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPpbCfg->priority = get_val;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_vlan_cfiKeepMode_get
 * Description:
 *      Get VLAN CFI keep mode configuration
 * Input:
 *      none
 * Output:
 *      mode        - cfi keep mode
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_cfiKeepMode_get(apollomp_raw_vlan_cfiKeepMode_t *mode)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((mode  == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_VLAN_CTRLr,APOLLOMP_CFI_KEEPf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *mode = tmpVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_cfiKeepMode_set
 * Description:
 *      Set VLAN CFI keep mode configuration
 * Input:
 *      mode        - cfi keep mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_cfiKeepMode_set(apollomp_raw_vlan_cfiKeepMode_t mode)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((APOLLOMP_CFI_KEEP_TYPE_END <= mode), RT_ERR_INPUT);

    tmpVal = mode;
    if ((ret = reg_field_write(APOLLOMP_VLAN_CTRLr,APOLLOMP_CFI_KEEPf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_acceptFrameType_set
 * Description:
 *      Per port set VLAN accept frame type
 * Input:
 *      port        - port number
 *      type        - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_acceptFrameType_set(rtk_port_t port, apollomp_raw_vlanAcpFrameType_t type)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((APOLLOMP_VLAN_ACCEPT_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = type;

    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PORT_ACCEPT_FRAME_TYPEr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_FRAME_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_acceptFrameType_get
 * Description:
 *      Per port get VLAN accept frame type
 * Input:
 *      port        - port number
 * Output:
 *      type        - accept frame type
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_acceptFrameType_get(rtk_port_t port, apollomp_raw_vlanAcpFrameType_t *type)
{
	uint32  tmpVal;
	int32   ret;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type  == NULL), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PORT_ACCEPT_FRAME_TYPEr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_FRAME_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    *type = tmpVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_vlan_igrFlterEnable_set
 * Description:
 *      Per port enable/disable vlan ingress filter
 * Input:
 *      port        - port number
 *      enable      - enable/disable vlan ingress filter
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_igrFlterEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = enable;

    if ((ret = reg_array_field_write(APOLLOMP_VLAN_INGRESSr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_INGRESSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_igrFlterEnable_get
 * Description:
 *      Per port get vlan ingress filter mode
 * Input:
 *      port        - port number
 * Output:
 *      enable      - enable/disable vlan ingress filter
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_igrFlterEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_INGRESSr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_INGRESSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_vlan_egrTagMode_set
 * Description:
 *      Per port set vlan egress tag mode
 * Input:
 *      port        - port number
 *      mode        - egress tag mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_egrTagMode_set(rtk_port_t port, apollomp_raw_vlan_egrTagMode_t mode)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((APOLLOMP_VLAN_EGR_TAG_MODE_END <= mode), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = mode;

    if ((ret = reg_array_field_write(APOLLOMP_VLAN_EGRESS_TAGr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_EGRESS_MODEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_egrTagMode_set
 * Description:
 *      Per port get vlan egress tag mode
 * Input:
 *      port        - port number
 *      mode        - egress tag mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_egrTagMode_get(rtk_port_t port, apollomp_raw_vlan_egrTagMode_t *pMode)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pMode==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);


    if ((ret = reg_array_field_read(APOLLOMP_VLAN_EGRESS_TAGr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_EGRESS_MODEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if(tmpVal == 3)
       tmpVal = APOLLOMP_VLAN_EGR_TAG_MODE_KEEP;
    *pMode = tmpVal;
    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_vid0TagType_set
 * Description:
 *      set vlan 0 tag type
 * Input:
 *      type        - tag type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_vid0TagType_set(apollomp_raw_vlanTag_t type)
{
	uint32  tmpVal;
	int32   ret;


    RT_PARAM_CHK((APOLLOMP_VLAN_TAG_TYPE_END <= type), RT_ERR_INPUT);

    tmpVal = type;

    if ((ret = reg_field_write(APOLLOMP_VLAN_CTRLr,APOLLOMP_VID_0_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_vid4095TagType_set
 * Description:
 *      set vlan 0 tag type
 * Input:
 *      type        - tag type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_vid4095TagType_set(apollomp_raw_vlanTag_t type)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((APOLLOMP_VLAN_TAG_TYPE_END <= type), RT_ERR_INPUT);

    tmpVal = type;

    if ((ret = reg_field_write(APOLLOMP_VLAN_CTRLr,APOLLOMP_VID_4095_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_vid0TagType_get
 * Description:
 *      get vlan 0 tag type
 * Input:
 *      none
 * Output:
 *      type        - tag type
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_vid0TagType_get(apollomp_raw_vlanTag_t *type)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((type  == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_VLAN_CTRLr,APOLLOMP_VID_0_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *type = tmpVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_vid4095TagType_get
 * Description:
 *      get vlan 4095 tag type
 * Input:
 *      none
 * Output:
 *      type        - tag type
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_vid4095TagType_get(apollomp_raw_vlanTag_t *type)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((type  == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_VLAN_CTRLr,APOLLOMP_VID_4095_TYPEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *type = tmpVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_transparentEnable_set
 * Description:
 *      set vlan transparent enable
 * Input:
 *      enable      - enable/disable
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_transparentEnable_set(rtk_enable_t enable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    tmpVal = enable;

    if ((ret = reg_field_write(APOLLOMP_VLAN_CTRLr,APOLLOMP_TRANSPARENT_ENf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_transparentEnable_get
 * Description:
 *      get vlan transparent enable
 * Input:
 *      none
 * Output:
 *      enable      - enable/disable
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_transparentEnable_get(rtk_enable_t *pEnable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_VLAN_CTRLr,APOLLOMP_TRANSPARENT_ENf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_egrIgrFilterEnable_set
 * Description:
 *      set vlan egress ingress filter enable
 * Input:
 *      enable      - enable/disable
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_egrIgrFilterEnable_set(rtk_enable_t enable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    tmpVal = enable;

    if ((ret = reg_field_write(APOLLOMP_VLAN_CTRLr,APOLLOMP_VLAN_FILTERINGf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_egrIgrFilterEnable_get
 * Description:
 *      get vlan egress ingress filter enable
 * Input:
 *      none
 * Output:
 *      enable      - enable/disable
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_egrIgrFilterEnable_get(rtk_enable_t *pEnable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);


    if ((ret = reg_field_read(APOLLOMP_VLAN_CTRLr,APOLLOMP_VLAN_FILTERINGf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_vlanPortBasedEfidEnable_set
 * Description:
 *      set  vlan port based FID enable
 * Input:
 *      port 		- port number
 *      enable      - enable/disable
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portBasedFidEnable_set(rtk_port_t port, rtk_enable_t enable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    tmpVal = enable;
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PB_FIDENr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_PBFIDENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlan_portBasedFidEnable_get
 * Description:
 *      get vlan port based FID enable
 * Input:
 *      port 		- port number
 * Output:
 *      enable      - enable/disable
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portBasedFidEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PB_FIDENr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_PBFIDENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pEnable = tmpVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_vlan_portBasedFid_get
 * Description:
 *      get vlan port based FID
 * Input:
 *      port 		- port number
 * Output:
 *      fid         - fid
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portBasedFid_get(rtk_port_t port, uint32 *pFid)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pFid==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PB_FIDr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_PBFIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    *pFid = tmpVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_vlan_portBasedFid_set
 * Description:
 *      set  vlan port based FID
 * Input:
 *      port 		- port number
 *      fid         - fid
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portBasedFid_set(rtk_port_t port, uint32 fid)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_VLAN_FID_MAX() < fid), RT_ERR_INPUT);

    tmpVal = fid;
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PB_FIDr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_PBFIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_vlan_portToMbrCfgIdx_set
 * Description:
 *      set vlan port based cvlan menber configuration index
 * Input:
 *      port 		- port number
 *      mbrCfgIdx   - index to cvlan member configuration table
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portToMbrCfgIdx_set(rtk_port_t port, uint32 mbrCfgIdx)
{
	uint32 tmpVal;
	int32 ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_CVIDXMAX < mbrCfgIdx), RT_ERR_INPUT);

    tmpVal = mbrCfgIdx;
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PB_VIDXr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_VIDXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_portToMbrCfgIdx_get
 * Description:
 *      set  vlan port based FID
 * Input:
 *      port 		- port number
 * Output:
 *      pri         - priority
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_portToMbrCfgIdx_get(rtk_port_t port, uint32 *pMbrCfgIdx)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pMbrCfgIdx==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PB_VIDXr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_VIDXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    *pMbrCfgIdx = tmpVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_egrKeepPmsk_set
 * Description:
 *      per port set egress keep port mask
 * Input:
 *      port 		- port number
 *      mbrCfgIdx   - index to cvlan member configuration table
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_egrKeepPmsk_set(rtk_port_t port, rtk_portmask_t portMsk)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((!HAL_IS_PORTMASK_VALID(portMsk)), RT_ERR_PORT_MASK);

    tmpVal = portMsk.bits[0];
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_EGRESS_KEEPr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_MBRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_egrKeepPmsk_get
 * Description:
 *      per port get egress keep port mask
 * Input:
 *      port 		- port number
 * Output:
 *      pMbrCfgIdx  - index to cvlan member configuration table
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_egrKeepPmsk_get(rtk_port_t port, rtk_portmask_t *pPortMsk)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pPortMsk==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_EGRESS_KEEPr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_MBRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pPortMsk->bits[0] = tmpVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_extPortToMbrCfgIdx_set
 * Description:
 *      set vlan extention port based cvlan menber configuration index
 * Input:
 *      port 		- extension port number
 *      mbrCfgIdx   - index to cvlan member configuration table
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_extPortToMbrCfgIdx_set(rtk_port_t port, uint32 mbrCfgIdx)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((APOLLOMP_EXTPORTNOMAX < port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_CVIDXMAX < mbrCfgIdx), RT_ERR_INPUT);

    tmpVal = mbrCfgIdx;
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_EXT_VIDXr,REG_ARRAY_INDEX_NONE,port,APOLLOMP_VIDXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlan_extPortToMbrCfgIdx_get
 * Description:
 *      get extention port based cvlan menber configuration index
 * Input:
 *      port 		- extension port number
 * Output:
 *      pMbrCfgIdx  - index to cvlan member configuration table
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_extPortToMbrCfgIdx_get(rtk_port_t port, uint32 *pMbrCfgIdx)
{
	uint32  tmpVal;
	int32   ret;

    RT_PARAM_CHK((APOLLOMP_EXTPORTNOMAX < port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pMbrCfgIdx==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_EXT_VIDXr,REG_ARRAY_INDEX_NONE,port,APOLLOMP_VIDXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    *pMbrCfgIdx = tmpVal;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_vlanProtoAndPortBasedEntry_set
 * Description:
 *      set vlan protocal entry
 * Input:
 *      entry
 * Output:
 *      entry
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlanProtoAndPortBasedEntry_set(apollomp_raw_protoValnEntryCfg_t *entry)
{
	uint32  tmpVal;
	int32   ret;
	/*parameter check*/
    RT_PARAM_CHK((entry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < entry->index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((APOLLOMP_PPVLAN_FRAME_TYPE_END <= entry->frameType), RT_ERR_INPUT);
    RT_PARAM_CHK((0xFFFF < entry->etherType), RT_ERR_INPUT);

    tmpVal = entry->frameType;
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,APOLLOMP_FRAME_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry->etherType;
    if ((ret = reg_array_field_write(APOLLOMP_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,APOLLOMP_ETHER_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_vlanProtoAndPortBasedEntry_get
 * Description:
 *      get vlan protocal entry
 * Input:
 *      entry
 * Output:
 *      entry
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 apollomp_raw_vlanProtoAndPortBasedEntry_get(apollomp_raw_protoValnEntryCfg_t *entry)
{
	uint32  tmpVal;
	int32   ret;
	/*parameter check*/
    RT_PARAM_CHK((entry  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_PROTOCOL_VLAN_IDX_MAX() < entry->index), RT_ERR_ENTRY_INDEX);


    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,APOLLOMP_FRAME_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    entry->frameType = tmpVal;

    if ((ret = reg_array_field_read(APOLLOMP_VLAN_PPB_VLAN_VALr,REG_ARRAY_INDEX_NONE,entry->index,APOLLOMP_ETHER_TYPEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    entry->etherType = tmpVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_raw_vlan_ipmcastLeaky_set
 * Description:
 *      Get IP mulicast vlan leaky
 * Input:
 *      port 		- port number
 *      enable 		- enable status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_ipmcastLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
	RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_L2_IPMC_VLAN_LEAKYr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_ENf,&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}/* end of apollomp_raw_vlan_ipmcastLeaky_set */

/* Function Name:
 *      apollomp_raw_vlan_ipmcastLeaky_get
 * Description:
 *      Get IP mulicast vlan leaky
 * Input:
 *      port 		- port number
 * Output:
 *      pEnable 		- enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_vlan_ipmcastLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
	int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pEnable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_L2_IPMC_VLAN_LEAKYr,port,REG_ARRAY_INDEX_NONE,APOLLOMP_ENf,pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
}/* end of apollomp_raw_vlan_ipmcastLeaky_get */


