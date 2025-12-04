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
 * Purpose : switch asic-level HW misc API
 * Feature : HW misc related functions
 *
 */
#include <rtk/debug.h>
#include <rtk/switch.h>
#include <dal/rtl9601b/dal_rtl9601b_hwmisc.h>
#include <ioal/mem32.h>


/* Function Name:
 *      rtl9601b_hsbData_get
 * Description:
 *      Get HSB data
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 rtl9601b_hsbData_get(rtk_hsb_t *hsbData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtk_mac_t tmp_da;
    rtk_mac_t tmp_sa;
    rtl9601b_hsb_entry_t hsb_entry2;
    rtl9601b_hsb_entry_t hsb_entry;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsb_entry2;
    for(index=0 ; index< (sizeof(rtl9601b_hsb_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9601B_HSB_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9601b_hsb_entry_t)/4) ; index++)
    {
        hsb_entry.entry_data[sizeof(rtl9601b_hsb_entry_t)/4 - 1 - index] = hsb_entry2.entry_data[index];
    }

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->spa = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_15tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_15 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_14tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_14 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_13tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_13 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_12tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_12 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_11tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_11 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_10tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_10 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_9tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_9 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_8tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_8 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_7tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_7 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_6tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_6 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_5 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_4 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_3 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_2 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_1 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_0 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_valid = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_IGMP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->igmp_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_UDP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->udp_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_TCP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->tcp_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_DIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->dip = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->sip = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_TOS_DSCPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->tos_dscp = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_IP_RSV_MC_ADDRtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ip_rsv_mc_addr = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_IP_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ip_type = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_OMAPDUtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->oampdu = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_LLC_OTHERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->llc_other = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pppoe_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SNAP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->snap_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_ETHER_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ether_type = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ctag = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ctag_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_STAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->stag = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->stag_if = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_pon_sid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PSELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_psel = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_DISLRNtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_dislrn = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_keep = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_pri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PRISELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_prisel = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_TXPMSKtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_txpmsk = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_if = tmp_val ;

    if ((ret = table_field_byte_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SAtf, (uint8 *)&tmp_sa.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsbData->sa.octet[0],&tmp_sa.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_byte_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_DAtf, (uint8 *)&tmp_da.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsbData->da.octet[0],&tmp_da.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_PON_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pon_idx = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_PKT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pkt_len = tmp_val ;

    return RT_ERR_OK;
} /* end of rtl9601b_hsbData_get */


/* Function Name:
 *      rtl9601b_hsaData_get
 * Description:
 *      Get HSA data
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 rtl9601b_hsaData_get(rtk_hsa_t *hsaData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtl9601b_hsa_entry_t hsa_entry;
    rtl9601b_hsa_entry_t hsa_entry2;
    uint32  chipId, rev, subType,regData;
    RT_PARAM_CHK((hsaData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsa_entry2;
    for(index=0 ; index<(sizeof(rtl9601b_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9601B_HSA_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9601b_hsa_entry_t)/4) ; index++)
    {
        hsa_entry.entry_data[sizeof(rtl9601b_hsa_entry_t)/4 - 1 - index] = hsa_entry2.entry_data[index];
    }
	
    if ((ret = rtk_switch_version_get(&chipId, &rev, &subType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

	if(rev == CHIP_REV_ID_0)
	{
		if ((ret = ioal_mem32_read(0x280d8, &regData)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		
		hsaData->rng_nhsab_omci_pktlen = (regData>>19) & 0x3F;
		hsaData->rng_nhsab_endsc = (regData>>10) & 0x1FF;
		hsaData->rng_nhsab_bgdsc = (regData>>1) & 0x1FF;		
	}
	else
	{		
		if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_OMCI_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_omci_pktlen = tmp_val;
		
		if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_endsc = tmp_val ;
		
		if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_bgdsc = tmp_val ;
	}

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_QIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_qid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_floodpkt = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_IPMCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_ipmc = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_CPUPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_cpupri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_FWDRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_fwdrsn = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_pon_sid = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_DPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_dpm = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_spa = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAB_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_pktlen = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_ORG_CVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_org_cvid= tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_ORG_CPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_org_cpri= tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_ORG_CFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_org_cfi= tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_CACT_TAGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_cact_tag= tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_UNTAGSETtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_untagset = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_CTAG_PONACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ctag_ponact = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_CTAG_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ctag_act = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_VIDZEROtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_vidzero = tmp_val ;
    
    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_PRITAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_pritag_if = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ctag_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_PONVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ponvid = tmp_val ;
	
    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_VIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_vid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_CACT_NOPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_cact_nop = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_PONPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ponpri = tmp_val ;
	
    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAC_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_pri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_MDY_SVID_PONtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_mdy_svid_pon = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_MDY_SVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_mdy_svid = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_stag_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_DEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_dei = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_UNTAGSETtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_untagset = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_STAG_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_stag_type = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_FRCTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_frctag = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_FRCTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_frctag_if= tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_PONSVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_ponsvid = tmp_val ;
	
    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_SVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_svid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_PKT_SPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_pkt_spri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_PONSPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_ponspri = tmp_val ;
	
    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAS_SPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_spri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_cputag_if = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_USER_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_user_pri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_1P_REM_PONtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_1p_rem_pon = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_1P_REM_EN_PONtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_1p_rem_en_pon = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_DSCP_REM_PONtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem_pon = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_DSCP_REM_EN_PONtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem_en_pon = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_1P_REMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_1p_rem = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_1P_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_1p_rem_en = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_DSCP_REMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAM_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem_en = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_regen_crc = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_CPUKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_cpukeep = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_keep = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_TCPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_tcp = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_UDPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_udp = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_IPV4tf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_ipv4 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_IPV6tf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_ipv6 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_RFC1042tf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_rfc1042 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DATA_TABLEt, RTL9601B_HSA_DATA_TABLE_RNG_NHSAF_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_pppoe_if = tmp_val ;

  
    return RT_ERR_OK;
} /* end of rtl9601b_hsaData_get */


/* Function Name:
 *      rtl9601b_hsdData_get
 * Description:
 *      Get HSA debug data  
 * Input:
 *      None
 * Output:
 *      hsdData
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 rtl9601b_hsdData_get(rtk_hsa_debug_t *hsdData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtl9601b_hsd_entry_t hsd_entry;
    rtl9601b_hsd_entry_t hsd_entry2;

    RT_PARAM_CHK((hsdData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsd_entry2;
    for(index=0 ; index<(sizeof(rtl9601b_hsd_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9601B_HSA_TX_DBGr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9601b_hsd_entry_t)/4) ; index++)
    {
        hsd_entry.entry_data[sizeof(rtl9601b_hsd_entry_t)/4 - 1 - index] = hsd_entry2.entry_data[index];

    }

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_PKT_CHANGEDtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pkt_change = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_TCPtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_tcp = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_UDPtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_udp = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_PADGtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_padg = tmp_val ;


    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_SV_DEItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_sv_dei = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_STYPtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_styp = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_PKTLEN_ORItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pktlen_ori = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_QIDtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_qid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_STDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_stdsc = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_CPUPRItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_cpupri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_SPRItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_spri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_TXINStf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_txins = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_CVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_cvid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_CFItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_cfi = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_REGENCRCtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_regencrc = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_PPPOEtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pppoe = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_RFC1042tf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_rfc1042 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_IPV6tf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ipv6 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA1_IPV4tf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ipv4 = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_REMDSCP_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_remdscp_pri = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_CTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ctag_pri= tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_REMDSCP_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_remdscp_en = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_SVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_svid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_STAG_INStf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_instag = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_PKTLENGTH_MDYtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pktlen = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_spa = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_DPCtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dpc = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ponsid = tmp_val ;

    if ((ret = table_field_get(RTL9601B_HSA_DEBUG_DATAt, RTL9601B_HSA_DEBUG_DATA_TXHSA0_TRPRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsd_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_trprsn = tmp_val ;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_hsbData_set
 * Description:
 *      Set HSB data
 * Input:
 *      hsbData
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 rtl9601b_hsbData_set(rtk_hsb_t *hsbData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtk_mac_t tmp_da;
    rtk_mac_t tmp_sa;
    rtl9601b_hsb_entry_t hsb_entry2;
    rtl9601b_hsb_entry_t hsb_entry;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);
	osal_memset(&hsb_entry, 0, sizeof(rtl9601b_hsb_entry_t));
	osal_memset(&hsb_entry2, 0, sizeof(rtl9601b_hsb_entry_t));


    tmp_val = hsbData->spa;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_15;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_15tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_14;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_14tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_13;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_13tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_12;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_12tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_11;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_11tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_10;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_10tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_9;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_9tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_8;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_8tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_7;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_7tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_6;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_6tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_5;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_4;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_3;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_2;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_1;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_0;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_FIELD_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_valid;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_USER_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->igmp_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_IGMP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->udp_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_UDP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->tcp_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_TCP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->dip;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_DIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->sip;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->tos_dscp;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_TOS_DSCPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ip_rsv_mc_addr;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_IP_RSV_MC_ADDRtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ip_type;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_IP_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->oampdu;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_OMAPDUtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->llc_other;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_LLC_OTHERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pppoe_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->snap_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SNAP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ether_type;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_ETHER_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ctag;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ctag_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->stag;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_STAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->stag_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
	
    tmp_val = hsbData->cputag_pon_sid;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_psel;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PSELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_dislrn;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_DISLRNtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_keep;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_pri;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_prisel;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_PRISELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_txpmsk;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_TXPMSKtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_if;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&tmp_sa.octet[0],&hsbData->sa.octet[0],sizeof(rtk_mac_t));
    if ((ret = table_field_byte_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_SAtf, (uint8 *)&tmp_sa.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&tmp_da.octet[0],&hsbData->da.octet[0],sizeof(rtk_mac_t));
    if ((ret = table_field_byte_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_DAtf, (uint8 *)&tmp_da.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pon_idx;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_PON_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pkt_len;
    if ((ret = table_field_set(RTL9601B_HSB_DATA_TABLEt, RTL9601B_HSB_DATA_TABLE_PKT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    for(index=0 ; index< (sizeof(rtl9601b_hsb_entry_t)/4) ; index++)
    {
        hsb_entry2.entry_data[sizeof(rtl9601b_hsb_entry_t)/4 - 1 - index] = hsb_entry.entry_data[index];
    }

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsb_entry2;
    for(index=0 ; index< (sizeof(rtl9601b_hsb_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_write(RTL9601B_HSB_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tmp_val_ptr ++;
    }


    return RT_ERR_OK;
} /* end of rtl9601b_hsbData_set */


