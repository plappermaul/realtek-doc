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
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1ad, SVLAN [VLAN Stacking]
 *
 */




/*
 * Include Files
 */
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_svlan.h>
#include <dal/apollomp/dal_apollomp_vlan.h>

/*
 * Symbol Definition
 */

#define DAL_APOLLOMP_SVLAN_SET(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {apollomp_svid_valid[vid>>3] |= (1 << (vid&7));}\
} while (0);\

#define DAL_APOLLOMP_SVLAN_CLEAR(vid) \
do {\
    if ((vid) <= RTK_VLAN_ID_MAX) {apollomp_svid_valid[vid >> 3] &= (~(1 << (vid&7)));}\
} while (0);\

#define DAL_APOLLOMP_SVLAN_IS_SET(vid) \
    (((vid) <= RTK_VLAN_ID_MAX)?((apollomp_svid_valid[vid >> 3] >> (vid&7)) & 1): 0)


#define DAL_APOLLOMP_SVLAN_ENTRY_SET(index) \
do {\
    if ((index) <= RTK_VLAN_ID_MAX) {apollomp_svidx_valid[index>>3] |= (1 << (index&7));}\
} while (0);\

#define DAL_APOLLOMP_SVLAN_ENTRY_CLEAR(index) \
do {\
    if ((index) <= RTK_VLAN_ID_MAX) {apollomp_svidx_valid[index>> 3] &= (~(1 << (index&7)));}\
} while (0);\

#define DAL_APOLLOMP_SVLAN_ENTRY_IS_SET(index) \
    (((index) <= RTK_VLAN_ID_MAX)?((apollomp_svidx_valid[index >> 3] >> (index&7)) & 1): 0)





/*
 * Data Declaration
 */
static uint32   apollomp_svlan_init = {INIT_NOT_COMPLETED};

static uint8    apollomp_svid_valid[4096/8];

static uint8	apollomp_svidx_valid[64/8];
/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : SVLAN */
static int _dal_apollomp_svlan_existCheck(rtk_vlan_t svid, uint32 *index)
{
    int32   ret;
    uint32  vidCheck;
    uint32  indexCheck;

    for(indexCheck = 0; indexCheck < APOLLOMP_SVLAN_ENTRY_NO; indexCheck ++)
    {
    	if(DAL_APOLLOMP_SVLAN_ENTRY_IS_SET(indexCheck))
    	{
	        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, indexCheck, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }
	        /*check exist configured SVID*/
	        if(vidCheck == svid)
	        {
	            *index = indexCheck;

	            return RT_ERR_OK;
	        }
    	}
    }

    return RT_ERR_SVLAN_NOT_EXIST;
}

static int _dal_apollomp_svlan_createCheck(rtk_vlan_t svid, uint32 *index)
{
    int32   ret;
    uint32  vidCheck;
    uint32  indexCheck;

    for(indexCheck = 0; indexCheck < APOLLOMP_SVLAN_ENTRY_NO; indexCheck ++)
    {
    	if(DAL_APOLLOMP_SVLAN_ENTRY_IS_SET(indexCheck))
    	{
	        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, indexCheck, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }

	        /*check exist configured SVID*/
	        if(vidCheck == svid)
	        {
	            *index = indexCheck;

	            return RT_ERR_SVLAN_EXIST;
	        }
    	}
    }

    for(indexCheck = 0; indexCheck < APOLLOMP_SVLAN_ENTRY_NO; indexCheck ++)
    {
    	if(!DAL_APOLLOMP_SVLAN_ENTRY_IS_SET(indexCheck))
    	{
            *index = indexCheck;

			DAL_APOLLOMP_SVLAN_ENTRY_SET(indexCheck);
			
            return RT_ERR_OK;
    	}
    }

    return RT_ERR_SVLAN_TABLE_FULL;
}

int32 dal_apollomp_svlan_checkAndDestroyAll(void)
{
    int32   ret;
	uint32  val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
	
	if(val != DAL_APOLLOMP_SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_OK;
	}
	
    osal_memset(apollomp_svidx_valid, 0x0, sizeof(apollomp_svidx_valid));

	return RT_ERR_OK;
}

int32 dal_apollomp_svlan_checkAndDestroy(rtk_vlan_t svid)
{
    int32   ret;
    uint32  vidCheck;
    uint32  indexCheck;
	uint32  val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
	
	if(val != DAL_APOLLOMP_SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_OK;
	}

    for(indexCheck = 0; indexCheck < APOLLOMP_SVLAN_ENTRY_NO; indexCheck ++)
    {
    	if(DAL_APOLLOMP_SVLAN_ENTRY_IS_SET(indexCheck))
    	{
	        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, indexCheck, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }
			
	        /*check exist configured SVID*/
	        if(vidCheck == svid)
	        {
	            DAL_APOLLOMP_SVLAN_ENTRY_CLEAR(svid);
				
			    val = 0;
			    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, indexCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
			    {
			        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
			        return ret;
			    }
				
				return RT_ERR_OK;
	        }
    	}
    }
	
	return RT_ERR_OK;
}

int32 dal_apollomp_svlan_checkAndCreate(rtk_vlan_t svid, uint32 *svidx)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
        {
			if((ret = dal_apollomp_vlan_create(svid)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_DAL|MOD_ACL), "");
				return ret;					
			}				
        }

		ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
	    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
	    }

		*svidx = svidxCheck;
		/*Create*/
	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }		

		return RT_ERR_OK;
	}
	else
	{
	    if(DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        if ((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }

	        *svidx = svidxCheck;

	        return RT_ERR_OK;
	    }

	    if((ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    DAL_APOLLOMP_SVLAN_SET(svid);

	    *svidx = svidxCheck;

	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    /*clear other slvan related setting*/
	    val = 0;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_UNTAGSETf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SPRf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FIDENf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FID_MSTIf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDENf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
	}
    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_checkAndCreate */


/* Function Name:
 *      dal_apollomp_svlan_init
 * Description:
 *      Initialize svlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize svlan module before calling any svlan APIs.
 */
int32
dal_apollomp_svlan_init(void)
{
    int32   ret;
    uint32  index;
    uint32  val;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    apollomp_svlan_init = INIT_COMPLETED;

	/*clear software database*/
    osal_memset(apollomp_svid_valid, 0x0, sizeof(apollomp_svid_valid));
    osal_memset(apollomp_svidx_valid, 0x0, sizeof(apollomp_svidx_valid));

    /*just clear SVID and leave other field for creating clear*/
    val = 0;
    for(index = 0; index < APOLLOMP_SVLAN_ENTRY_NO; index++)
    {
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    val = 0;
    for(index = 0; index < APOLLOMP_SVLAN_MC2S_NO; index++)
    {
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    val = 0;
    for(index = 0; index < APOLLOMP_SVLAN_SP2C_NO; index++)
    {
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    val = 0;
    for(index = 0; index < APOLLOMP_SVLAN_C2S_NO; index++)
    {
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_C2SENPMSKf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EVIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDXf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    if ((ret = dal_apollomp_svlan_lookupType_set(SVLAN_LOOKUP_S64MBRCGF)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        apollomp_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_apollomp_svlan_servicePort_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            apollomp_svlan_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if ((ret = dal_apollomp_svlan_dmacVidSelState_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            apollomp_svlan_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if ((ret = dal_apollomp_svlan_deiKeepState_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        apollomp_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_apollomp_svlan_untagAction_set(SVLAN_ACTION_DROP, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        apollomp_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if ((ret = dal_apollomp_svlan_unmatchAction_set(SVLAN_ACTION_DROP, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
        apollomp_svlan_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_init */

/* Function Name:
 *      dal_apollomp_svlan_create
 * Description:
 *      Create the svlan.
 * Input:
 *      svid - svlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_EXIST          - SVLAN entry is exist
 *      RT_ERR_SVLAN_TABLE_FULL
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_create(rtk_vlan_t svid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
	    if((ret = dal_apollomp_vlan_create(svid)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }	
	}
	else
	{
	    if(DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_EXIST;
	    }

	    if((ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    DAL_APOLLOMP_SVLAN_SET(svid);

	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    /*clear other slvan related setting*/
	    val = 0;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_UNTAGSETf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SPRf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FIDENf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FID_MSTIf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDENf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
	}
    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_create */


/* Function Name:
 *      dal_apollomp_svlan_destroy
 * Description:
 *      Destroy the svlan.
 * Input:
 *      svid - svlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_destroy(rtk_vlan_t svid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
	    if((ret = dal_apollomp_vlan_destroy(svid)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }	

	}	

	
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }

    DAL_APOLLOMP_SVLAN_CLEAR(svid);

    if ((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
	
	DAL_APOLLOMP_SVLAN_ENTRY_CLEAR(svidxCheck);
	
    val = 0;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_destroy */

/* Function Name:
 *      dal_apollomp_svlan_portSvid_get
 * Description:
 *      Get port default svlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pSvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_SVLAN_INVALID
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_portSvid_get(rtk_port_t port, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  svidx;
    uint32  vidCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);
    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
	    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
		
	    if(!DAL_APOLLOMP_SVLAN_ENTRY_IS_SET(svidx))
	    {
	        return RT_ERR_SVLAN_INVALID;
	    }
		
	    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

		if((ret = dal_apollomp_vlan_existCheck(vidCheck)) != RT_ERR_OK)
        {
            return RT_ERR_VLAN_ENTRY_NOT_FOUND;
        }
		
		*pSvid = vidCheck;
	}
	else
	{
	    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

	    if(!DAL_APOLLOMP_SVLAN_IS_SET(vidCheck))
	    {
	        return RT_ERR_SVLAN_INVALID;
	    }

	    *pSvid = vidCheck;
	}
    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_portSvid_get */


/* Function Name:
 *      dal_apollomp_svlan_portSvid_set
 * Description:
 *      Set port default svlan id.
 * Input:
 *      port - port id
 *      svid - port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_portSvid_set(rtk_port_t port, rtk_vlan_t svid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
        {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
		
		ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
	    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
	    }
		
		/*Create*/
	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }		
		/*Set Port-based SVLAN*/
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }		
		
		return RT_ERR_OK;
	}
	else
	{
	    if(DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        if ((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }

	        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
	        {
	            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }
	    }
	    else
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }
	}
    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_portSvid_set */

/* Function Name:
 *      dal_apollomp_svlan_servicePort_get
 * Description:
 *      Get service ports from the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - status of service port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_servicePort_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_UPLINK_PMSKr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_servicePort_get */


/* Function Name:
 *      dal_apollomp_svlan_servicePort_set
 * Description:
 *      Set service ports to the specified device.
 * Input:
 *      port       - port id
 *      enable     - status of service port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_servicePort_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_UPLINK_PMSKr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_servicePort_set */

/* Function Name:
 *      dal_apollomp_svlan_memberPort_set
 * Description:
 *      Replace the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
int32
dal_apollomp_svlan_memberPort_set(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_port_set(svid, pSvlanPortmask, pSvlanUntagPortmask)) != RT_ERR_OK)
		{
        	RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
			return ret;
		}
		
		return RT_ERR_OK;
	}

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pSvlanPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvlanUntagPortmask), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pSvlanPortmask), RT_ERR_PORT_MASK);
	RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pSvlanUntagPortmask), RT_ERR_PORT_MASK);

    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = pSvlanPortmask->bits[0];
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = pSvlanUntagPortmask->bits[0];
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_UNTAGSETf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_memberPort_set */



/* Function Name:
 *      dal_apollomp_svlan_memberPort_get
 * Description:
 *      Get the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
int32
dal_apollomp_svlan_memberPort_get(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_port_get(svid, pSvlanPortmask, pSvlanUntagPortmask)) != RT_ERR_OK)
		{
        	RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
			return ret;
		}
		
		return RT_ERR_OK;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pSvlanPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvlanUntagPortmask), RT_ERR_NULL_POINTER);

    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        pSvlanPortmask->bits[0] = val;

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_UNTAGSETf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        pSvlanUntagPortmask->bits[0] = val;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_memberPort_get */


/* Function Name:
 *      dal_apollomp_svlan_tpidEntry_get
 * Description:
 *      Get the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 * Output:
 *      pSvlanTagId - pointer buffer of svlan TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only support pSvlanTagId 0 in Apollo.
 */
int32
dal_apollomp_svlan_tpidEntry_get(uint32 svlanIndex, uint32 *pSvlanTagId)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pSvlanTagId), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CFGr, APOLLOMP_VS_TPIDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pSvlanTagId = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_tpidEntry_get */


/* Function Name:
 *      dal_apollomp_svlan_tpidEntry_set
 * Description:
 *      Set the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 *      svlan_tag_id - svlan TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only support pSvlan_tag_id 0 in Apollo.
 */
int32
dal_apollomp_svlan_tpidEntry_set(uint32 svlanIndex, uint32 svlan_tag_id)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((svlanIndex >= HAL_MAX_NUM_OF_SVLAN_TPID()), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ETHERTYPE_MAX < svlan_tag_id), RT_ERR_SVLAN_ETHER_TYPE);

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CFGr, APOLLOMP_VS_TPIDf, &svlan_tag_id)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_tpidEntry_set */


/* Function Name:
 *      dal_apollomp_svlan_priorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 * Input:
 *      ref - reference selection parameter.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
int32
dal_apollomp_svlan_priorityRef_set(rtk_svlan_pri_ref_t ref)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    switch(ref)
    {
        case SVLAN_PRISEL_INTERNAL_PRI:
            val = DAL_APOLLOMP_SVLAN_PRISEL_INTERNAL_PRI;

            break;
        case SVLAN_PRISEL_1QTAG_PRI:
            val = DAL_APOLLOMP_SVLAN_PRISEL_1QTAG_PRI;

            break;
        case SVLAN_PRISEL_VSPRI:
            val = DAL_APOLLOMP_SVLAN_PRISEL_VSPRI;

            break;
        case SVLAN_PRISEL_PBPRI:
            val = DAL_APOLLOMP_SVLAN_PRISEL_PBPRI;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_SPRISELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_priorityRef_set */

/* Function Name:
 *      dal_apollomp_svlan_priorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 * Input:
 *      None
 * Output:
 *      pRef - reference selection parameter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
int32
dal_apollomp_svlan_priorityRef_get(rtk_svlan_pri_ref_t *pRef)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRef), RT_ERR_NULL_POINTER);

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_SPRISELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(val)
    {
        case DAL_APOLLOMP_SVLAN_PRISEL_INTERNAL_PRI:
            *pRef = SVLAN_PRISEL_INTERNAL_PRI;

            break;
        case DAL_APOLLOMP_SVLAN_PRISEL_1QTAG_PRI:
            *pRef = SVLAN_PRISEL_1QTAG_PRI;

            break;
        case DAL_APOLLOMP_SVLAN_PRISEL_VSPRI:
            *pRef = SVLAN_PRISEL_VSPRI;

            break;
        case DAL_APOLLOMP_SVLAN_PRISEL_PBPRI:
            *pRef = SVLAN_PRISEL_PBPRI;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_priorityRef_get */

/* Function Name:
 *      dal_apollomp_svlan_memberPortEntry_set
 * Description:
 *      Configure system SVLAN member content
 * Input:
 *      psvlan_cfg - SVLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameter.
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_SVLAN_TABLE_FULL - SVLAN configuration is full.
 * Note:
 *      The API can set system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped by default setup.
 *      - rtk_svlan_memberCfg_t->svid is SVID of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->memberport is member port mask of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->fid is filtering database of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->priority is priority of SVLAN member configuration.
 */
int32
dal_apollomp_svlan_memberPortEntry_set(rtk_svlan_memberCfg_t *pSvlan_cfg)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlan_cfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(pSvlan_cfg->svid), RT_ERR_VLAN_VID);


    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_S64MBRCGF)
	{
	    if(!DAL_APOLLOMP_SVLAN_IS_SET(pSvlan_cfg->svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }
	}

    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(pSvlan_cfg->memberport), RT_ERR_PORT_MASK);
	RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID(pSvlan_cfg->untagport), RT_ERR_PORT_MASK);

	RT_PARAM_CHK((RTK_ENABLE_END <= pSvlan_cfg->fiden), RT_ERR_INPUT);
	RT_PARAM_CHK((RTK_ENABLE_END <= pSvlan_cfg->efiden), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_VLAN_FID_MAX() < pSvlan_cfg->fid), RT_ERR_FID);
    RT_PARAM_CHK((HAL_ENHANCED_FID_MAX() < pSvlan_cfg->efid), RT_ERR_EFID);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pSvlan_cfg->priority), RT_ERR_PRIORITY);


    if((ret = _dal_apollomp_svlan_existCheck(pSvlan_cfg->svid, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->memberport.bits[0];
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->untagport.bits[0];
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_UNTAGSETf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->priority;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SPRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->fiden;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FIDENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->fid;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FID_MSTIf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->efiden;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    val = (uint32)pSvlan_cfg->efid;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_memberPortEntry_set */


/* Function Name:
 *      dal_apollomp_svlan_memberPortEntry_get
 * Description:
 *      Get SVLAN member Configure.
 * Input:
 *      pSvlan_cfg - SVLAN member configuration
 * Output:
 *      pSvlan_cfg - SVLAN member configuration
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can get system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped.
 */
int32
dal_apollomp_svlan_memberPortEntry_get(rtk_svlan_memberCfg_t *pSvlan_cfg)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSvlan_cfg), RT_ERR_NULL_POINTER);
	RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(pSvlan_cfg->svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_S64MBRCGF)
	{
		if(!DAL_APOLLOMP_SVLAN_IS_SET(pSvlan_cfg->svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }
	}

    if((ret = _dal_apollomp_svlan_existCheck(pSvlan_cfg->svid, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_MBRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->memberport.bits[0] = val;

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_UNTAGSETf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->untagport.bits[0] = val;

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SPRf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->priority = val;

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FIDENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->fiden = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FID_MSTIf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->fid = val;

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->efiden = (rtk_enable_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    pSvlan_cfg->efid = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_memberPortEntry_get */


/* Function Name:
 *      dal_apollomp_svlan_ipmc2s_add
 * Description:
 *      add ip multicast address to SVLAN
 * Input:
 *      ipmc    - ip multicast address
 *      ipmcMsk - ip multicast address mask
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_SVLAN_TABLE_FULL
 * Note:
 *      The API can set IP mutlicast to SVID configuration. If upstream packet is IPv4 multicast
 *      packet and DIP is matched MC2S configuration, ASIC will assign egress SVID to the packet.
 *      There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
dal_apollomp_svlan_ipmc2s_add(ipaddr_t ipmc, ipaddr_t ipmcMsk, rtk_vlan_t svid)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  ipmcIdx;
    uint32  valid;
    uint32  format;
    ipaddr_t data;
    ipaddr_t mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
        {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
		
		ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
	    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
	    }
		
		/*Create*/
	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }		
	
	}
	else
	{
	    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }

	    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
	}
	
    for(ipmcIdx = 0; ipmcIdx < APOLLOMP_SVLAN_MC2S_NO; ipmcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(ENABLED ==  valid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(DAL_APOLLOMP_SVLAN_MC2S_FMT_IP == format)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                /*get the same IP and IP mask setting*/
                /*direct replace svid config*/
                if(data == ipmc && mask == ipmcMsk)
                {
                    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    return RT_ERR_OK;
                }
            }
        }
    }

    for(ipmcIdx = 0; ipmcIdx < APOLLOMP_SVLAN_MC2S_NO; ipmcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(DISABLED == valid)
        {

            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            data = ipmc;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            mask = ipmcMsk;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            format = DAL_APOLLOMP_SVLAN_MC2S_FMT_IP;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            valid = ENABLED;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            return RT_ERR_OK;
        }

    }

    return RT_ERR_SVLAN_TABLE_FULL;
} /* end of dal_apollomp_svlan_ipmc2s_add */

/* Function Name:
 *      dal_apollomp_svlan_ipmc2s_del
 * Description:
 *      delete ip multicast address to SVLAN
 * Input:
 *      ipmc - ip multicast address
 *      ipmcMsk - ip multicast address mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete IP mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
dal_apollomp_svlan_ipmc2s_del(ipaddr_t ipmc, ipaddr_t ipmcMsk)
{
    int32   ret;
    uint32  ipmcIdx;
    uint32  valid;
    uint32  format;
    ipaddr_t data;
    ipaddr_t mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    for(ipmcIdx = 0; ipmcIdx < APOLLOMP_SVLAN_MC2S_NO; ipmcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(ENABLED ==  valid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(DAL_APOLLOMP_SVLAN_MC2S_FMT_IP == format)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                /*get the same IP and IP mask setting*/
                /*direct replace svid config*/
                if(data == ipmc && mask == ipmcMsk)
                {
                    valid = DISABLED;
                    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    return RT_ERR_OK;
                }
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_ipmc2s_del */


/* Function Name:
 *      dal_apollomp_svlan_ipmc2s_get
 * Description:
 *      Get ip multicast address to SVLAN
 * Input:
 *      ipmc - ip multicast address
 *      ipmcMsk - ip multicast address mask
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can get IP mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
dal_apollomp_svlan_ipmc2s_get(ipaddr_t ipmc, ipaddr_t ipmcMsk, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  svidx;
    uint32  svidCheck;
    uint32  ipmcIdx;
    uint32  valid;
    uint32  format;
    ipaddr_t data;
    ipaddr_t mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);
    /* parameter check */
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    for(ipmcIdx = 0; ipmcIdx < APOLLOMP_SVLAN_MC2S_NO; ipmcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(ENABLED ==  valid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(DAL_APOLLOMP_SVLAN_MC2S_FMT_IP == format)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                /*get the same IP and IP mask setting*/
                /*direct replace svid config*/
                if(data == ipmc && mask == ipmcMsk)
                {
                    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, ipmcIdx, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    /*get the svid*/
                    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &svidCheck)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    *pSvid = (rtk_vlan_t)svidCheck;

                    return RT_ERR_OK;
                }
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_ipmc2s_get */


/* Function Name:
 *      dal_apollomp_svlan_l2mc2s_add
 * Description:
 *      Add L2 multicast address to SVLAN
 * Input:
 *      mac     - L2 multicast address
 *      macMsk  - L2 multicast address mask
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can set L2 Mutlicast to SVID configuration. If upstream packet is L2 multicast
 *      packet and DMAC is matched, ASIC will assign egress SVID to the packet. There are 8
 *      SVLAN multicast configurations for IP and L2 multicast.
 */
int32
dal_apollomp_svlan_l2mc2s_add(rtk_mac_t mac, rtk_mac_t macMsk, rtk_vlan_t svid)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  l2mcIdx;
    uint32  valid;
    uint32  format;
    uint32  data;
    uint32  mask;
    uint32  dataInput;
    uint32  maskInput;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
        {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

		ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
	    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
	    }
		
		/*Create*/
	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }		
	
	}
	else
	{
	    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }

	    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
	}
	
    dataInput = mac.octet[2];
    dataInput = (dataInput << 8) | mac.octet[3];
    dataInput = (dataInput << 8) | mac.octet[4];
    dataInput = (dataInput << 8) | mac.octet[5];

    maskInput = macMsk.octet[2];
    maskInput = (maskInput << 8) | macMsk.octet[3];
    maskInput = (maskInput << 8) | macMsk.octet[4];
    maskInput = (maskInput << 8) | macMsk.octet[5];

    for(l2mcIdx = 0; l2mcIdx < APOLLOMP_SVLAN_MC2S_NO; l2mcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(ENABLED ==  valid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(DAL_APOLLOMP_SVLAN_MC2S_FMT_MAC == format)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                /*get the same IP and IP mask setting*/
                /*direct replace svid config*/
                if(data == dataInput && mask == maskInput)
                {
                    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    return RT_ERR_OK;
                }
            }
        }
    }

    for(l2mcIdx = 0; l2mcIdx < APOLLOMP_SVLAN_MC2S_NO; l2mcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(DISABLED == valid)
        {
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            data = dataInput;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            mask = maskInput;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            format = DAL_APOLLOMP_SVLAN_MC2S_FMT_MAC;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            valid = ENABLED;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_SVLAN_TABLE_FULL;
} /* end of dal_apollomp_svlan_l2mc2s_add */


/* Function Name:
 *      dal_apollomp_svlan_l2mc2s_del
 * Description:
 *      delete L2 multicast address to SVLAN
 * Input:
 *      mac - L2 multicast address
 *      macMsk  - L2 multicast address mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete Mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
dal_apollomp_svlan_l2mc2s_del(rtk_mac_t mac, rtk_mac_t macMsk)
{
    int32   ret;
    uint32  l2mcIdx;
    uint32  valid;
    uint32  format;
    uint32  data;
    uint32  mask;
    uint32  dataInput;
    uint32  maskInput;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    dataInput = mac.octet[2];
    dataInput = (dataInput << 8) | mac.octet[3];
    dataInput = (dataInput << 8) | mac.octet[4];
    dataInput = (dataInput << 8) | mac.octet[5];

    maskInput = macMsk.octet[2];
    maskInput = (maskInput << 8) | macMsk.octet[3];
    maskInput = (maskInput << 8) | macMsk.octet[4];
    maskInput = (maskInput << 8) | macMsk.octet[5];

    for(l2mcIdx = 0; l2mcIdx < APOLLOMP_SVLAN_MC2S_NO; l2mcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(ENABLED ==  valid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(DAL_APOLLOMP_SVLAN_MC2S_FMT_MAC == format)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                /*get the same IP and IP mask setting*/
                /*direct replace svid config*/
                if(data == dataInput && mask == maskInput)
                {
                    valid = DISABLED;
                    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    return RT_ERR_OK;
                }
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_l2mc2s_del */


/* Function Name:
 *      dal_apollomp_svlan_l2mc2s_get
 * Description:
 *      Get L2 multicast address to SVLAN
 * Input:
 *      mac     - L2 multicast address
 *      macMsk  - L2 multicast address mask
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can get L2 mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
int32
dal_apollomp_svlan_l2mc2s_get(rtk_mac_t mac, rtk_mac_t macMsk, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  svidx;
    uint32  svidCheck;
    uint32  l2mcIdx;
    uint32  valid;
    uint32  format;
    uint32  data;
    uint32  mask;
    uint32  dataInput;
    uint32  maskInput;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);
    /* parameter check */
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    dataInput = mac.octet[2];
    dataInput = (dataInput << 8) | mac.octet[3];
    dataInput = (dataInput << 8) | mac.octet[4];
    dataInput = (dataInput << 8) | mac.octet[5];

    maskInput = macMsk.octet[2];
    maskInput = (maskInput << 8) | macMsk.octet[3];
    maskInput = (maskInput << 8) | macMsk.octet[4];
    maskInput = (maskInput << 8) | macMsk.octet[5];

    for(l2mcIdx = 0; l2mcIdx < APOLLOMP_SVLAN_MC2S_NO; l2mcIdx ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(ENABLED ==  valid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_FORMATf, &format)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(DAL_APOLLOMP_SVLAN_MC2S_FMT_MAC == format)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_DATAf, &data)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_MASKf, &mask)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                /*get the same IP and IP mask setting*/
                /*direct replace svid config*/
                if(data == dataInput && mask == maskInput)
                {
                    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, l2mcIdx, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    /*get the svid*/
                    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &svidCheck)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    *pSvid = (rtk_vlan_t)svidCheck;

                    return RT_ERR_OK;
                }
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_l2mc2s_get */


/* Function Name:
 *      dal_apollomp_svlan_sp2c_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      cvid        - VLAN ID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
int32
dal_apollomp_svlan_sp2c_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t cvid)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  svidx;
    uint32  valid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < cvid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
        {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

		ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
	    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
	    }
	
		/*Create*/
	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }		
	
	}
	else
	{
	    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }

	    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
	}
	
    for(sp2cIndex = 0; sp2cIndex < APOLLOMP_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidxCheck == svidx &&  port == dstPort)
            {
                val = (uint32)cvid;
                if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VIDf, &val)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    for(sp2cIndex = 0; sp2cIndex < APOLLOMP_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == DISABLED)
        {
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            val = (uint32)cvid;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VIDf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            val = (uint32)dstPort;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            valid = ENABLED;
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_SVLAN_TABLE_FULL;
} /* end of dal_apollomp_svlan_sp2c_add */


/* Function Name:
 *      dal_apollomp_svlan_sp2c_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid 	    - SVLAN VID
 *      dst_port 	- Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pCvid - VLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
int32
dal_apollomp_svlan_sp2c_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t *pCvid)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  svidx;
    uint32  valid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCvid), RT_ERR_NULL_POINTER);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_S64MBRCGF)
	{
		if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }
	}

    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    for(sp2cIndex = 0; sp2cIndex < APOLLOMP_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidxCheck == svidx &&  port == dstPort)
            {
                if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VIDf, &val)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                *pCvid = (rtk_vlan_t)val;

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_sp2c_get */

/* Function Name:
 *      dal_apollomp_svlan_sp2c_del
 * Description:
 *      Delete system SP2C configuration
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can delete SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
int32
dal_apollomp_svlan_sp2c_del(rtk_vlan_t svid, rtk_port_t dstPort)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  sp2cIndex;
    rtk_port_t port;
    uint32  svidx;
    uint32  valid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(dstPort), RT_ERR_PORT_ID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_S64MBRCGF)
	{
		if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }
	}

    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    for(sp2cIndex = 0; sp2cIndex < APOLLOMP_SVLAN_SP2C_NO; sp2cIndex ++)
    {
        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(valid == ENABLED)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &port)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(svidxCheck == svidx &&  port == dstPort)
            {
                valid = DISABLED;
                if ((ret = reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_sp2c_del */

/* Function Name:
 *      dal_apollomp_svlan_dmacVidSelState_set
 * Description:
 *      Set DMAC CVID selection status
 * Input:
 *      port    - Port
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection state
 */
int32
dal_apollomp_svlan_dmacVidSelState_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_EP_DMAC_CTRLr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_dmacVidSelState_set */

/* Function Name:
 *      dal_apollomp_svlan_dmacVidSelState_get
 * Description:
 *      Get DMAC CVID selection status
 * Input:
 *      port    - Port
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can get DMAC CVID Selection state
 */
int32
dal_apollomp_svlan_dmacVidSelState_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_SVLAN_EP_DMAC_CTRLr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_dmacVidSelState_get */

/* Function Name:
 *      dal_apollomp_svlan_unmatchAction_set
 * Description:
 *      Configure Action of downstream Unmatch packet
 * Input:
 *      action  - Action for Unmatch
 *      svid    - The SVID assigned to Unmatch packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also supported by this API. The parameter add svid is
 *      only refernced when the action is set to UNMATCH_ASSIGN
 */
int32
dal_apollomp_svlan_unmatchAction_set(rtk_svlan_action_t action, rtk_vlan_t svid)
{
    int32   ret;
    uint32  unmatchAction;
    uint32  val;
    uint32  svidxCheck;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

	if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    /* parameter check */

    switch(action)
    {
        case SVLAN_ACTION_DROP:

            unmatchAction = DAL_APOLLOMP_SVLAN_UNMATCH_DROP;
            break;
        case SVLAN_ACTION_TRAP:

            unmatchAction = DAL_APOLLOMP_SVLAN_UNMATCH_TRAP;
            break;

        case SVLAN_ACTION_SVLAN:

            unmatchAction = DAL_APOLLOMP_SVLAN_UNMATCH_ASSIGN;
            break;

        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

	svidxCheck = 0;
	
    if(action == SVLAN_ACTION_SVLAN)
    {
        RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

	    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }  
		
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNMATf, &unmatchAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }


    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNMAT_SVIDXf, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_unmatchAction_set */


/* Function Name:
 *      dal_apollomp_svlan_unmatchAction_get
 * Description:
 *      Get Action of downstream Unmatch packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for Unmatch
 *      pSvid    - The SVID assigned to Unmatch packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNMATCH_ASSIGN
 */
int32
dal_apollomp_svlan_unmatchAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  unmatchAction;
    uint32  svidx;
    uint32  vidCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

	if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNMATf, &unmatchAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }


    switch(unmatchAction)
    {
        case DAL_APOLLOMP_SVLAN_UNMATCH_DROP:

            *pAction = SVLAN_ACTION_DROP;
            break;
        case DAL_APOLLOMP_SVLAN_UNMATCH_TRAP:

            *pAction = SVLAN_ACTION_TRAP;
            break;

        case DAL_APOLLOMP_SVLAN_UNMATCH_ASSIGN:

            *pAction = SVLAN_ACTION_SVLAN;

            if((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNMAT_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            *pSvid = vidCheck;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
}/* end of dal_apollomp_svlan_unmatchAction_get */

/* Function Name:
 *      dal_apollomp_svlan_untagAction_set
 * Description:
 *      Configure Action of downstream UnStag packet
 * Input:
 *      action  - Action for UnStag
 *      svid    - The SVID assigned to UnStag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also supported by this API. The parameter of svid is
 *      only referenced when the action is set to UNTAG_ASSIGN
 */
int32
dal_apollomp_svlan_untagAction_set(rtk_svlan_action_t action, rtk_vlan_t svid)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  untagAction;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

	svidxCheck = 0;
    /* parameter check */
    switch(action)
    {
        case SVLAN_ACTION_DROP:

            untagAction = DAL_APOLLOMP_SVLAN_UNTAG_DROP;
            break;
        case SVLAN_ACTION_TRAP:

            untagAction = DAL_APOLLOMP_SVLAN_UNTAG_TRAP;
            break;

        case SVLAN_ACTION_SVLAN:

            untagAction = DAL_APOLLOMP_SVLAN_UNTAG_ASSIGN;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if(action == SVLAN_ACTION_SVLAN)
    {
        RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

	    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }

		if(val == SVLAN_LOOKUP_C4KVLAN)
		{
			if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
	        {
		        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
	        }
			ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
		    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	            return ret;
		    }
			
			/*Create*/
		    val = (uint32)svid;
		    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
		        return ret;
		    }					
		}
		else
		{
		    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
		    {
		        return RT_ERR_SVLAN_NOT_EXIST;
		    }  
			
            if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }
		}
    }

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNTAGf, &untagAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNTAG_SVIDXf, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_untagAction_set */


/* Function Name:
 *      dal_apollomp_svlan_untagAction_get
 * Description:
 *      Get Action of downstream UnStag packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for UnStag
 *      pSvid    - The SVID assigned to UnStag packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNTAG_ASSIGN
 */
int32
dal_apollomp_svlan_untagAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  svidx;
    uint32  untagAction;
    uint32  vidCheck;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNTAGf, &untagAction)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(untagAction)
    {
        case DAL_APOLLOMP_SVLAN_UNTAG_DROP:

            *pAction = SVLAN_ACTION_DROP;
            break;
        case DAL_APOLLOMP_SVLAN_UNTAG_TRAP:

            *pAction = SVLAN_ACTION_TRAP;
            break;
        case DAL_APOLLOMP_SVLAN_UNTAG_ASSIGN:

            *pAction = SVLAN_ACTION_SVLAN;

            if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_UNTAG_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &vidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            *pSvid = vidCheck;

            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_untagAction_get */

/* Function Name:
 *      dal_apollomp_svlan_c2s_add
 * Description:
 *      add CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_SVLAN_TABLE_FULL
 * Note:
 *      The API can set upstream packet CVID and ingress port to SVID configuration.
 *      There are 128 SVLAN configurations for CVID and ingress port.
 *      If CVID and SVID of configured entry are matched with configuration parameter, then
 *      different ingress port will share the same configuration entry.
 */
int32
dal_apollomp_svlan_c2s_add(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t svid)
{
    int32   ret;
    uint32  val;
    uint32  svidxCheck;
    uint32  c2sIndex;
    uint32  cvidCheck;
    rtk_portmask_t  portmask;
    uint32  svidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < cvid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		if((ret = dal_apollomp_vlan_existCheck(svid)) != RT_ERR_OK)
        {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

		ret = _dal_apollomp_svlan_createCheck(svid, &svidxCheck);
	    if(ret != RT_ERR_SVLAN_EXIST && ret != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
	    }
		
		/*Create*/
	    val = (uint32)svid;
	    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SVIDf, &val)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }			
	}
	else
	{
	    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
	    {
	        return RT_ERR_SVLAN_NOT_EXIST;
	    }

	    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
	        return ret;
	    }
	}
	
    for(c2sIndex = 0; c2sIndex < APOLLOMP_SVLAN_C2S_NO; c2sIndex ++)
    {
        if((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_EVIDf, &cvidCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        /*find the same cvid for translate*/
        if(cvidCheck == cvid)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if(RTK_PORTMASK_IS_PORT_SET(portmask, port))
            {
                /*exist configration*/
                if(svidx == svidxCheck)
                {
                     /* All the same, do nothing */
                    return RT_ERR_OK;
                }
                else
                {
                    /*remove different c2s port*/
                    RTK_PORTMASK_PORT_CLEAR(portmask, port);
                    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }
                }
            }
            else
            {
                /*exist configration*/
                if(svidx == svidxCheck)
                {
                    RTK_PORTMASK_PORT_SET(portmask, port);
                    if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                        return ret;
                    }

                    return RT_ERR_OK;
                }
            }
        }
    }


    for(c2sIndex = 0; c2sIndex < APOLLOMP_SVLAN_C2S_NO; c2sIndex ++)
    {
        if((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_EVIDf, &cvidCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        /*check valid entry for add new configuration*/
        if(0 == cvidCheck && 0 == portmask.bits[0] && 0 == svidx)
        {
            if((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_EVIDf, &cvid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            RTK_PORTMASK_PORT_SET(portmask, port);
            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_SVIDXf, &svidxCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            return RT_ERR_OK;

        }
    }

    return RT_ERR_SVLAN_TABLE_FULL;
} /* end of dal_apollomp_svlan_c2s_add */

/* Function Name:
 *      dal_apollomp_svlan_c2s_del
 * Description:
 *      delete CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The API can delet upstream packet CVID and ingress port to SVID configuration.
 */
int32
dal_apollomp_svlan_c2s_del(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t svid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  c2sIndex;
    uint32  cvidCheck;
    rtk_portmask_t  portmask;
    uint32  svidx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < cvid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);

    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }

    if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    for(c2sIndex = 0; c2sIndex < APOLLOMP_SVLAN_C2S_NO; c2sIndex ++)
    {
        if((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_EVIDf, &cvidCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if(cvidCheck == cvid && svidx == svidxCheck)
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            /*remove different c2s port*/
            RTK_PORTMASK_PORT_CLEAR(portmask, port);

            /*check if entry is still valid*/
            if(portmask.bits[0])
            {
                if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }
            }
            else
            {
                /*clear entry elements*/
                if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                cvidCheck = 0;
                if((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_EVIDf, &cvidCheck)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }

                svidx = 0;
                if ((ret = reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                    return ret;
                }
            }

            return RT_ERR_OK;
        }
    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_c2s_del */


/* Function Name:
 *      dal_apollomp_svlan_c2s_get
 * Description:
 *      Get CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_INPUT                    - Invalid input parameters.
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can delet upstream packet CVID and ingress port to SVID configuration.
 */
int32
dal_apollomp_svlan_c2s_get(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t *pSvid)
{
    int32   ret;
    uint32  c2sIndex;
    uint32  cvidCheck;
    rtk_portmask_t  portmask;
    uint32  svidx;
    uint32  svidCheck;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < cvid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSvid), RT_ERR_NULL_POINTER);

    for(c2sIndex = 0; c2sIndex < APOLLOMP_SVLAN_C2S_NO; c2sIndex ++)
    {
        if((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_EVIDf, &cvidCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_C2SENPMSKf, &portmask.bits[0])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        /*check both cvid and source port are matched*/
        if(cvidCheck == cvid && RTK_PORTMASK_IS_PORT_SET(portmask, port))
        {
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, c2sIndex, APOLLOMP_SVIDXf, &svidx)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }
            /*get the svid*/
            if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidx, APOLLOMP_SVIDf, &svidCheck)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
                return ret;
            }

            *pSvid = (rtk_vlan_t)svidCheck;

            return RT_ERR_OK;
        }

    }

    return RT_ERR_SVLAN_ENTRY_NOT_FOUND;
} /* end of dal_apollomp_svlan_c2s_get */

/* Function Name:
 *      dal_apollomp_svlan_trapPri_get
 * Description:
 *      Get svlan trap priority
 * Input:
 *      None
 * Output:
 *      pPriority - priority for trap packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_trapPri_get(rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_PRIf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pPriority = (rtk_pri_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_trapPri_get */


/* Function Name:
 *      dal_apollomp_svlan_trapPri_set
 * Description:
 *      Set svlan trap priority
 * Input:
 *      priority - priority for trap packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_trapPri_set(rtk_pri_t priority)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_INT_PRIORITY);

    val = (uint32)priority;

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_PRIf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_trapPri_set */

/* Function Name:
 *      dal_apollomp_svlan_deiKeepState_get
 * Description:
 *      Get svlan dei keep state
 * Input:
 *      None
 * Output:
 *      pEnable - state of keep dei
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_deiKeepState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_DEI_KEEPf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_deiKeepState_get */

/* Function Name:
 *      dal_apollomp_svlan_deiKeepState_set
 * Description:
 *      Set svlan dei keep state
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_deiKeepState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_DEI_KEEPf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_deiKeepState_set */

/* Function Name:
 *      dal_apollomp_svlan_lookupType_get
 * Description:
 *      Get lookup type of SVLAN
 * Input:
 *      None
 * Output:
 *      pType 		- lookup type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_lookupType_get(rtk_svlan_lookupType_t *pType)
{
    int32   ret;
    uint32  lookupType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_TYPEf, &lookupType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    switch(lookupType)
    {
        case DAL_APOLLOMP_SVLAN_LOOKUP_S64MBRCGF:

            *pType = SVLAN_LOOKUP_S64MBRCGF;
            break;
        case DAL_APOLLOMP_SVLAN_LOOKUP_C4KVLAN:

            *pType = SVLAN_LOOKUP_C4KVLAN;
            break;
        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_lookupType_get */

/* Function Name:
 *      dal_apollomp_svlan_lookupType_set
 * Description:
 *      Set lookup type of SVLAN
 * Input:
 *      type 		- lookup type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_lookupType_set(rtk_svlan_lookupType_t type)
{
    int32   ret;
    uint32  lookupType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */

    switch(type)
    {
        case SVLAN_LOOKUP_S64MBRCGF:

            lookupType = DAL_APOLLOMP_SVLAN_LOOKUP_S64MBRCGF;
            break;
        case SVLAN_LOOKUP_C4KVLAN:

            lookupType = DAL_APOLLOMP_SVLAN_LOOKUP_C4KVLAN;
            break;
        default:

            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_TYPEf, &lookupType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_lookupType_set */

/* Function Name:
 *      dal_apollomp_svlan_sp2cUnmatchCtagging_get
 * Description:
 *      Get unmatch sp2c egress ctagging state
 * Input:
 *      None
 * Output:
 *      pState     - unmatch cvlan tagging state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_sp2cUnmatchCtagging_get(rtk_enable_t *pState)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);
	
    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_SP2C_UNMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pState = (rtk_enable_t)val;
    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_sp2cUnmatchCtagging_get */

/* Function Name:
 *      dal_apollomp_svlan_sp2cUnmatchCtagging_set
 * Description:
 *      Set unmatch sp2c egress ctagging state
 * Input:
 *      state      - unmatch cvlan tagging state
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_sp2cUnmatchCtagging_set(rtk_enable_t state)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}

    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    val = (uint32)state;

    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_SP2C_UNMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_svlan_sp2cUnmatchCtagging_set */


/* Function Name:
 *      dal_apollomp_svlan_priority_get
 * Description:
 *      Get SVLAN priority for each SVID.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pPriority - priority assigned for the SVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *     None
 */
int32
dal_apollomp_svlan_priority_get(rtk_vlan_t svid, rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d",svid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SPRf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        *pPriority = val;
    }


    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_priority_get */

/* Function Name:
 *      dal_apollomp_svlan_priority_set
 * Description:
 *      Set SVLAN priority for each SVID.
 * Input:
 *      svid  - svlan id
 *      priority - priority assigned for the SVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      This API is used to set priority per SVLAN.
 */
int32
dal_apollomp_svlan_priority_set(rtk_vlan_t svid, rtk_pri_t priority)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "vid=%d,priority=%d",svid, priority);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_PRIORITY);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = priority;
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_SPRf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_priority_set */


/* Function Name:
 *      dal_apollomp_svlan_fid_get
 * Description:
 *      Get the filter id of the vlan.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pFid - pointer buffer of filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_fid_get(rtk_vlan_t svid, rtk_fid_t *pFid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d",svid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FID_MSTIf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        *pFid = val;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_fid_get */

/* Function Name:
 *      dal_apollomp_svlan_fid_set
 * Description:
 *      Set the filter id of the svlan.
 * Input:
 *      svid  - svlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The FID is effective only in VLAN SVL mode. 
 */
int32
dal_apollomp_svlan_fid_set(rtk_vlan_t svid, rtk_fid_t fid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d,fid=%d",svid, fid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((HAL_VLAN_FID_MAX() < fid), RT_ERR_FID);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = fid;
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FID_MSTIf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_fid_set */

/* Function Name:
 *      dal_apollomp_svlan_fidEnable_get
 * Description:
 *      Get svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEnable - pointer to svlan based fid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_fidEnable_get(rtk_vlan_t svid, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d",svid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FIDENf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        *pEnable = val;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_fidEnable_get */


/* Function Name:
 *      dal_apollomp_svlan_fidEnable_set
 * Description:
 *      Set svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 *      enable - svlan based fid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_fidEnable_set(rtk_vlan_t svid, rtk_enable_t enable)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d,enable=%d",svid, enable);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	} 

    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = enable;
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_FIDENf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_fidEnable_set */

/* Function Name:
 *      dal_apollomp_svlan_enhancedFid_get
 * Description:
 *      Get the enhanced filter id of the vlan.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEfid - pointer buffer of enhanced filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_enhancedFid_get(rtk_vlan_t svid, rtk_efid_t *pEfid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d",svid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pEfid), RT_ERR_NULL_POINTER);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        *pEfid = val;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_enhancedFid_get */

/* Function Name:
 *      dal_apollomp_svlan_enhancedFid_set
 * Description:
 *      Set the enhanced filter id of the svlan.
 * Input:
 *      svid  - svlan id
 *      efid  - enhanced filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The EFID is effective only in VLAN SVL mode. 
 */
int32
dal_apollomp_svlan_enhancedFid_set(rtk_vlan_t svid, rtk_efid_t efid)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d,efid=%d",svid, efid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(efid > HAL_ENHANCED_FID_MAX(), RT_ERR_INPUT);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = efid;
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_enhancedFid_set */

/* Function Name:
 *      dal_apollomp_svlan_enhancedFidEnable_get
 * Description:
 *      Get svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEnable - pointer to svlan based efid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_enhancedFidEnable_get(rtk_vlan_t svid, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d",svid);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}
    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        if ((ret = reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDENf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
        *pEnable = val;
    }

    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_enhancedFidEnable_get */


/* Function Name:
 *      dal_apollomp_svlan_enhancedFidEnable_set
 * Description:
 *      Set svlan based efid assignment status.
 * Input:
 *      svid  - svlan id
 *      enable - svlan based efid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
int32
dal_apollomp_svlan_enhancedFidEnable_set(rtk_vlan_t svid, rtk_enable_t enable)
{
    int32   ret;
    uint32  svidxCheck;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN), "svid=%d,enable=%d",svid, enable);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    if((ret = dal_apollomp_svlan_lookupType_get(&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

	if(val == SVLAN_LOOKUP_C4KVLAN)
	{
		return RT_ERR_CHIP_NOT_SUPPORTED;
	}


    /* parameter check */
    RT_PARAM_CHK(DAL_APOLLOMP_SVLAN_VID_NOT_ACCEPT(svid), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if(!DAL_APOLLOMP_SVLAN_IS_SET(svid))
    {
        return RT_ERR_SVLAN_NOT_EXIST;
    }
    else
    {
        if((ret = _dal_apollomp_svlan_existCheck(svid, &svidxCheck)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }

        val = enable;
        if ((ret = reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svidxCheck, APOLLOMP_EFIDENf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
            return ret;
        }
    }
	
    return RT_ERR_OK;
}   /* end of dal_apollomp_svlan_enhancedFidEnable_set */

/* Function Name:
 *      dal_apollomp_svlan_dmacVidSelForcedState_set
 * Description:
 *      Set forced DMAC CVID selection
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection forced state
 */
int32
dal_apollomp_svlan_dmacVidSelForcedState_set(rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_FORCED_DMACVIDSELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_dmacVidSelForcedState_get
 * Description:
 *      Get DMAC CVID selection status
 * Input:
 *      None
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      This API can get DMAC CVID Selection Forced state
 */
int32
dal_apollomp_svlan_dmacVidSelForcedState_get(rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SVLAN),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_svlan_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_FORCED_DMACVIDSELf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SVLAN), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;
    return RT_ERR_OK;
}

