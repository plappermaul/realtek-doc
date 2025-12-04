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
 * Purpose : switch asic-level L34 API
 * Feature : L34 related functions
 *
 */

#include <dal/apollomp/raw/apollomp_raw_l34.h>



static apollomp_raw_l34_arp_trf_t arp_trf_select_table = APOLLOMP_RAW_L34_ARPTRF_TABLE0;
static apollomp_raw_l34_l4_trf_t l34_trf_select_table = APOLLOMP_RAW_L34_L4TRF_TABLE0;


/* Function Name:
 *      apollomp_raw_l34_hsba_mode_get
 * Description:
 *      Get L34 hsba moe
 * Input:
 *      None
 * Output:
 *      mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsba_mode_get(apollomp_raw_l34_hsba_mode_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_HSBA_CTRLr, APOLLOMP_TST_LOG_MDf, &tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    *mode = (apollomp_raw_l34_hsba_mode_t)tmp_val;
    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_l34_hsba_mode_set
 * Description:
 *      Set L34 hsba moe
 * Input:
 *      mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsba_mode_set(apollomp_raw_l34_hsba_mode_t mode)
{
    int32 ret;

    RT_PARAM_CHK((mode>APOLLOMP_RAW_L34_HSBA_LOG_FIRST_TO_CPU), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_HSBA_CTRLr, APOLLOMP_TST_LOG_MDf, (uint32 *)&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_l34_hsbaBusyFlag_get
 * Description:
 *      Get L34 hsba busy flag
 * Input:
 *      None
 * Output:
 *      flag
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsbaBusyFlag_get(uint32 *flag)
{
    int32 ret;

    RT_PARAM_CHK((flag==NULL), RT_ERR_NULL_POINTER);
    if ((ret = reg_field_read(APOLLOMP_HSBA_CTRLr, APOLLOMP_ALE34_BZf, flag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}





/* Function Name:
 *      apollomp_raw_l34_hsbaBusyFlag_get
 * Description:
 *      Get L34 hsba active flag
 * Input:
 *      None
 * Output:
 *      flag
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsbaActiveFlag_get(uint32 *flag)
{
    int32 ret;

    RT_PARAM_CHK((flag==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_HSBA_CTRLr, APOLLOMP_HSB_ATVf, flag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_l34_hsbaBusyFlag_get
 * Description:
 *      Get L34 hsba active flag
 * Input:
 *      flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsbaActiveFlag_set(uint32 flag)
{
    int32 ret;

    if ((ret = reg_field_write(APOLLOMP_HSBA_CTRLr, APOLLOMP_HSB_ATVf, (uint32 *)&flag)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      apollomp_raw_l34_hsaData_set
 * Description:
 *      Set L34 HSA data
 * Input:
 *      hsaData
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsaData_set(apollomp_raw_l34_hsa_t *hsaData)
{
    int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    ipaddr_t tmp_ip;
    apollomp_l34_hsa_entry_t hsba_entry,tmp_hsba_entry;


    RT_PARAM_CHK((hsaData==NULL), RT_ERR_NULL_POINTER);

    memset(&hsba_entry, 0x00, sizeof(apollomp_l34_hsa_entry_t));


    tmp_val = hsaData->pppoeKeep;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PPPOE_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->bindVidTrans;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_BIND_VIDTRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    tmp_val = hsaData->interVlanIf;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_INTERNAL_VLAN_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->reason;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_REASONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->l34trans;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L3L4TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->l2trans;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L2TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    tmp_val = hsaData->action;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_ACTIONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->l4_pri_valid;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L4HP_Vtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->l4_pri_sel;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L4HPtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->frag;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_FRAGtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->difid;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_DIFIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->pppoe_if;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->pppid_idx;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PPPID_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }



    tmp_val = hsaData->nexthop_mac_idx;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_NEXTHOP_MAC_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->l4_chksum;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L4_CHECKSUM_OFFSETtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->l3_chksum;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L3_CHECKSUM_OFFSETtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->dvid;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_DVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsaData->port;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PORTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_ip = hsaData->ip;
    if ((ret = table_field_set(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_IPtf, (uint32 *)&tmp_ip, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /* prepare data for writing */
    for(index=0 ; index< (sizeof(apollomp_l34_hsa_entry_t)/4) ; index++)
    {
        tmp_hsba_entry.entry_data[sizeof(apollomp_l34_hsa_entry_t)/4 - 1 - index] = hsba_entry.entry_data[index];
    }


    /*write data to register*/
    tmp_val_ptr = (uint32 *) &tmp_hsba_entry;
    for(index=0 ; index<(sizeof(apollomp_l34_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_write((APOLLOMP_HSA_DESC_W0r + index), tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_l34_hsaData_get
 * Description:
 *      Get L34 HSA data
 * Input:
 *      None
 * Output:
 *      hsaData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsaData_get(apollomp_raw_l34_hsa_t *hsaData)
{
    int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    ipaddr_t tmp_ip;
    apollomp_l34_hsa_entry_t hsba_entry,tmp_hsba_entry;


    /* parameter check */
    RT_PARAM_CHK((hsaData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &tmp_hsba_entry;
    for(index=0 ; index<(sizeof(apollomp_l34_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_read((APOLLOMP_HSA_DESC_W0r + index), tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    /* prepare data for reading */
    for(index=0 ; index< (sizeof(apollomp_l34_hsa_entry_t)/4) ; index++)
    {
        hsba_entry.entry_data[sizeof(apollomp_l34_hsa_entry_t)/4 - 1 - index] = tmp_hsba_entry.entry_data[index];
    }


    /*get field data from hsba buffer*/
    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PPPOE_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->pppoeKeep = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_BIND_VIDTRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->bindVidTrans = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_INTERNAL_VLAN_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->interVlanIf = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_REASONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->reason = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L3L4TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->l34trans = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L2TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->l2trans = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_ACTIONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->action = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L4HP_Vtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->l4_pri_valid = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L4HPtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->l4_pri_sel = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_FRAGtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->frag = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_DIFIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->difid = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->pppoe_if = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PPPID_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->pppid_idx = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_NEXTHOP_MAC_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->nexthop_mac_idx = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L4_CHECKSUM_OFFSETtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->l4_chksum = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_L3_CHECKSUM_OFFSETtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->l3_chksum = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_DVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->dvid = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_PORTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->port = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSAt, APOLLOMP_L34_HSA_IPtf, (uint32 *)&tmp_ip, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->ip = tmp_ip ;

    return RT_ERR_OK;
}





/* Function Name:
 *      apollomp_raw_l34_hsbData_set
 * Description:
 *      Set L34 HSB data
 * Input:
 *      hsbData
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsbData_set(apollomp_raw_l34_hsb_t *hsbData)
{
    int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    ipaddr_t tmp_ip;
    apollomp_l34_hsb_entry_t hsba_entry,tmp_hsba_entry;
    rtk_ipv6_addr_t tmpIpv6Addr;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);

    memset(&hsba_entry, 0x00, sizeof(apollomp_l34_hsb_entry_t));

    tmp_val = hsbData->isFromWan;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_IS_FROM_WAN_PORTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->l2bridge;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_L2BRIDGEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ipfrag_s;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_IPFRAGOFStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ipmf;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_IPMFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->l4_chksum_ok;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_L4CSOKtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    tmp_val = hsbData->l3_chksum_ok;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_L3CSOKtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cpu_direct_tx;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_CPU_DIRECT_TXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->udp_no_chksum;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_UDP_NOCStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->parse_fail;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_PARS_FAILtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pppoe_if;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->svlan_if;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SVLAN_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    tmp_val = hsbData->ttls;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_TTLSTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->type;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->tcp_flag;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_TCP_FLAG_PROTOCOL_PARSING_FAILURE_REASONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cvlan_if;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_CVLAN_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->spa;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cvid;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_CVLANIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    tmp_val = hsbData->len;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->dport_l4chksum;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_DPORT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pppoe_id;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_PPPOE_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    memcpy(&tmpIpv6Addr,&(hsbData->dip),sizeof(rtk_ipv6_addr_t));
    if ((ret = table_field_byte_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_DIPtf, (uint8 *)&tmpIpv6Addr, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_ip = hsbData->sip;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SIPtf, (uint32 *)&tmp_ip, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->sport_icmpid_chksum;
    if ((ret = table_field_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SPORT_ICMPID_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_mac_set(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_DMACtf, (uint8 *)&hsbData->dmac, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*write data to register*/
    for(index=0 ; index< (sizeof(apollomp_l34_hsb_entry_t)/4) ; index++)
    {
        tmp_hsba_entry.entry_data[sizeof(apollomp_l34_hsb_entry_t)/4 - 1 - index] = hsba_entry.entry_data[index];
    }


    tmp_val_ptr = (uint32 *) &tmp_hsba_entry;
    for(index=0 ; index<(sizeof(apollomp_l34_hsb_entry_t)/4) ; index++)
    {
        if ((ret = reg_write((APOLLOMP_HSB_DESC_W0r + index), tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }


    /*set active flag*/
    if ((ret = apollomp_raw_l34_hsbaActiveFlag_set(1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }


    /*check busy flag*/
    index=0;
    for(;;)
    {
        if ((ret = apollomp_raw_l34_hsbaBusyFlag_get(&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        if(tmp_val == 0)
            break;
        index++;
        if(index > 10000)
        {
            osal_printf("HSB access timeout!!");
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }

    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_l34_hsbData_get
 * Description:
 *      Get L34 HSB data
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hsbData_get(apollomp_raw_l34_hsb_t *hsbData)
{
    int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    ipaddr_t tmp_ip;
    rtk_ipv6_addr_t  tmpIpv6Addr;
    apollomp_l34_hsb_entry_t hsba_entry,tmp_hsba_entry;


    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &tmp_hsba_entry;
    for(index=0 ; index<(sizeof(apollomp_l34_hsb_entry_t)/4) ; index++)
    {
        if ((ret = reg_read((APOLLOMP_HSB_DESC_W0r + index), tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    /*Prepare the data for reading*/
    for(index=0 ; index< (sizeof(apollomp_l34_hsb_entry_t)/4) ; index++)
    {
        hsba_entry.entry_data[sizeof(apollomp_l34_hsb_entry_t)/4 - 1 - index] = tmp_hsba_entry.entry_data[index];
    }


    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_IS_FROM_WAN_PORTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->isFromWan = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_L2BRIDGEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->l2bridge = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_IPFRAGOFStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ipfrag_s = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_IPMFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ipmf = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_L4CSOKtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->l4_chksum_ok = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_L3CSOKtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->l3_chksum_ok = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_CPU_DIRECT_TXtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cpu_direct_tx = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_UDP_NOCStf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->udp_no_chksum = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_PARS_FAILtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->parse_fail = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pppoe_if = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SVLAN_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->svlan_if = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_TTLSTtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ttls = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->type = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_TCP_FLAG_PROTOCOL_PARSING_FAILURE_REASONtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->tcp_flag = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_CVLAN_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cvlan_if = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->spa = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_CVLANIDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cvid = tmp_val ;


    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->len = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_DPORT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->dport_l4chksum = tmp_val ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_PPPOE_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pppoe_id = tmp_val ;


    if ((ret = table_field_byte_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_DIPtf, (uint8 *)&tmpIpv6Addr, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    memcpy(&(hsbData->dip),&tmpIpv6Addr,sizeof(rtk_ipv6_addr_t));

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SIPtf, (uint32 *)&tmp_ip, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->sip = tmp_ip ;

    if ((ret = table_field_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_SPORT_ICMPID_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->sport_icmpid_chksum = tmp_val ;


    if ((ret = table_field_mac_get(APOLLOMP_L34_HSBt, APOLLOMP_L34_HSB_DMACtf, (uint8 *)&hsbData->dmac, (uint32 *) &hsba_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}



/* Function Name:
 *      apollomp_raw_l34_pppoeTrf_get
 * Description:
 *      Get L34 PPPoE traffic indicator
 * Input:
 *      None
 * Output:
 *      pppoeTrf
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_pppoeTrf_get(uint32 *pppoeTrf)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((pppoeTrf==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PP_AGEr, APOLLOMP_PPPOE_TRF_BMPf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *pppoeTrf = tmp_val;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_l34_portIntfIdx_get
 * Description:
 *      Get port net interface index
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_portIntfIdx_get(rtk_port_t port,uint8 *idx)
{
    int32 ret;
    uint32 tmp_val;
    uint32 field_name;

    RT_PARAM_CHK((idx==NULL), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 0:
            field_name = APOLLOMP_INTP0f;
            break;
        case 1:
            field_name = APOLLOMP_INTP1f;
            break;
        case 2:
            field_name = APOLLOMP_INTP2f;
            break;
        case 3:
            field_name = APOLLOMP_INTP3f;
            break;
        case 4:
            field_name = APOLLOMP_INTP4f;
            break;
        case 5:
            field_name = APOLLOMP_INTP5f;
            break;
        default:
            return RT_ERR_PORT_ID;
    }



    if ((ret = reg_field_read(APOLLOMP_NIFPr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *idx = (uint8)tmp_val;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_l34_portIntfIdx_set
 * Description:
 *      Set port net interface index
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_portIntfIdx_set(rtk_port_t port,uint8 idx)
{
    int32 ret;
    uint32 tmp_val;
    uint32 field_name;

    RT_PARAM_CHK((idx >= HAL_L34_NETIF_ENTRY_MAX()), RT_ERR_INPUT);

    switch(port)
    {
        case 0:
            field_name = APOLLOMP_INTP0f;
            break;
        case 1:
            field_name = APOLLOMP_INTP1f;
            break;
        case 2:
            field_name = APOLLOMP_INTP2f;
            break;
        case 3:
            field_name = APOLLOMP_INTP3f;
            break;
        case 4:
            field_name = APOLLOMP_INTP4f;
            break;
        case 5:
            field_name = APOLLOMP_INTP5f;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    tmp_val = idx;
    if ((ret = reg_field_write(APOLLOMP_NIFPr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_l34_extPortIntfIdx_get
 * Description:
 *      Get extention port net interface index
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_extPortIntfIdx_get(rtk_port_t port,uint8 *idx)
{
    int32 ret;
    uint32 tmp_val;
    uint32 field_name;

    RT_PARAM_CHK((port > APOLLOMP_EXTPORTNOMAX), RT_ERR_PORT_ID);
    RT_PARAM_CHK((idx==NULL), RT_ERR_NULL_POINTER);


    field_name = APOLLOMP_INTEXTP0f - (APOLLOMP_EXTPORTNOMAX - port);

    if ((ret = reg_field_read(APOLLOMP_NIFEPr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *idx = (uint8)tmp_val;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_l34_extPortIntfIdx_set
 * Description:
 *      Set extension port net interface index
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_extPortIntfIdx_set(rtk_port_t port,uint8 idx)
{
    int32 ret;
    uint32 tmp_val;
    uint32 field_name;


    RT_PARAM_CHK((port >= APOLLOMP_EXTPORTNO), RT_ERR_PORT_ID);
    RT_PARAM_CHK((idx >= HAL_L34_NETIF_ENTRY_MAX()), RT_ERR_PORT_ID);

    field_name = APOLLOMP_INTEXTP0f  - (APOLLOMP_EXTPORTNOMAX - port);
    tmp_val = idx;
    if ((ret = reg_field_write(APOLLOMP_NIFEPr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollomp_raw_l34_portIntfIdx_get
 * Description:
 *      Get port net interface index
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_vcPortIntfIdx_get(rtk_port_t port,uint8 *idx)
{
    int32 ret;
    uint32 tmp_val;
    uint32 field_name;

    RT_PARAM_CHK((idx==NULL), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 0:
            field_name = APOLLOMP_VC0f;
            break;
        case 1:
            field_name = APOLLOMP_VC1f;
            break;
        case 2:
            field_name = APOLLOMP_VC2f;
            break;
        case 3:
            field_name = APOLLOMP_VC3f;
            break;
        case 4:
            field_name = APOLLOMP_VC4f;
            break;
        case 5:
            field_name = APOLLOMP_VC5f;
            break;
    case 6:
        field_name = APOLLOMP_VC6f;
            break;
    case 7:
        field_name = APOLLOMP_VC7f;
            break;
    case 8:
        field_name = APOLLOMP_VC8f;
            break;
    case 9:
        field_name = APOLLOMP_VC9f;
            break;
    case 10:
        field_name = APOLLOMP_VC10f;
            break;
    case 11:
        field_name = APOLLOMP_VC11f;
            break;
    case 12:
        field_name = APOLLOMP_VC12f;
            break;
    case 13:
        field_name = APOLLOMP_VC13f;
            break;
    case 14:
        field_name = APOLLOMP_VC14f;
            break;
    case 15:
        field_name = APOLLOMP_VC15f;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    if(port < 8){
        if ((ret = reg_field_read(APOLLOMP_NIFVCLr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    }else{
      if ((ret = reg_field_read(APOLLOMP_NIFVCHr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    }
    *idx = (uint8)tmp_val;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_raw_l34_portIntfIdx_set
 * Description:
 *      Set port net interface index
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_vcPortIntfIdx_set(rtk_port_t port,uint8 idx)
{
    int32 ret;
    uint32 tmp_val;
    uint32 field_name;

    RT_PARAM_CHK((idx >= HAL_L34_NETIF_ENTRY_MAX()), RT_ERR_INPUT);

      switch(port)
    {
        case 0:
            field_name = APOLLOMP_VC0f;
            break;
        case 1:
            field_name = APOLLOMP_VC1f;
            break;
        case 2:
            field_name = APOLLOMP_VC2f;
            break;
        case 3:
            field_name = APOLLOMP_VC3f;
            break;
        case 4:
            field_name = APOLLOMP_VC4f;
            break;
        case 5:
            field_name = APOLLOMP_VC5f;
            break;
    case 6:
        field_name = APOLLOMP_VC6f;
            break;
    case 7:
        field_name = APOLLOMP_VC7f;
            break;
    case 8:
        field_name = APOLLOMP_VC8f;
            break;
    case 9:
        field_name = APOLLOMP_VC9f;
            break;
    case 10:
        field_name = APOLLOMP_VC10f;
            break;
    case 11:
        field_name = APOLLOMP_VC11f;
            break;
    case 12:
        field_name = APOLLOMP_VC12f;
            break;
    case 13:
        field_name = APOLLOMP_VC13f;
            break;
    case 14:
        field_name = APOLLOMP_VC14f;
            break;
    case 15:
        field_name = APOLLOMP_VC15f;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    tmp_val = idx;

    if(port < 8){
        if ((ret = reg_field_write(APOLLOMP_NIFVCLr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    }else{
          if ((ret = reg_field_write(APOLLOMP_NIFVCHr, field_name, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      apollomp_raw_l34_debugMonitor_set
 * Description:
 *      Set debug/monitor signal select
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l34_debugMonitor_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable==DISABLED)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_MONSELf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_debugMonitor_get
 * Description:
 *      Get debug/monitor signal select
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_debugMonitor_get(rtk_enable_t *enable)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((enable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_MONSELf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(tmp_val==0)
        *enable = DISABLED;
    else
        *enable = ENABLED;

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_pppoeKeep_set
 * Description:
 *      Set keep pppoe format setting
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32 apollomp_raw_l34_pppoeKeep_set(apollomp_raw_l34_pppoeKeepMode_t mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((APOLLOMP_RAW_L34_PPPOE_END <= mode), RT_ERR_INPUT);

    if(mode==APOLLOMP_RAW_L34_PPPOE_BY_ASIC)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_KEEP_PPPOE_TAGf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_pppoeKeep_get
 * Description:
 *      Get keep pppoe format setting
 * Input:
 *      None
 * Output:
 *      hsbData
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_pppoeKeep_get(apollomp_raw_l34_pppoeKeepMode_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_KEEP_PPPOE_TAGf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(tmp_val==0)
        *mode = APOLLOMP_RAW_L34_PPPOE_BY_ASIC;
    else
        *mode = APOLLOMP_RAW_L34_PPPOE_KEEP;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_natAttack2CPU_set
 * Description:
 *      Set nat attack packet to CPU setting
 * Input:
 *      rtk_enable_t enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_natAttack2CPU_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable==DISABLED)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_ENNATT2LOGf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_natAttack2CPU_get
 * Description:
 *      Get debug/monitor signal select
 * Input:
 *      None
 * Output:
 *      rtk_enable_t *enable
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_natAttack2CPU_get(rtk_enable_t *enable)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((enable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_ENNATT2LOGf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(tmp_val==0)
        *enable = DISABLED;
    else
        *enable = ENABLED;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_wanRouteAct_set
 * Description:
 *      Set nat WAN route mode
 * Input:
 *      rtk_l34_wanroute_act_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_wanRouteAct_set(rtk_l34_wanroute_act_t act)
{
    int32 ret;
    uint32 tmp_val;

    switch(act)
    {
        case L34_WAN_ROUTE_FWD:
            tmp_val = 0;
            break;

        case L34_WAN_ROUTE_FWD_TO_CPU:
            tmp_val = 1;
            break;

        case L34_WAN_ROUTE_DROP:
            tmp_val = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }


    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_WANROUTEMODEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_wanRouteAct_get
 * Description:
 *      Get nat WAN route mode
 * Input:
 *      None
 * Output:
 *      rtk_enable_t *enable
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_wanRouteAct_get(rtk_l34_wanroute_act_t *act)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((act==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_WANROUTEMODEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }


    switch(tmp_val)
    {
        case 0:
            *act = L34_WAN_ROUTE_FWD;
            break;

        case 1:
            *act = L34_WAN_ROUTE_FWD_TO_CPU;
            break;

        case 2:
            *act = L34_WAN_ROUTE_DROP ;

            break;

        default:
            return RT_ERR_OUT_OF_RANGE;
    }


    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_LanIntfMDBC_set
 * Description:
 *      Set NAT LAN interface Mutilayer-Decision-Base Control mode.
 * Input:
 *      apollomp_raw_l34_limbc_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_LanIntfMDBC_set(apollomp_raw_l34_limbc_t mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode >= APOLLOMP_RAW_L34_LIMBC_RESERVED), RT_ERR_INPUT);

    tmp_val = mode;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_LIMDBCf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_LanIntfMDBC_get
 * Description:
 *      Get NAT LAN interface Mutilayer-Decision-Base Control mode.
 * Input:
 *      None
 * Output:
 *      apollomp_raw_l34_limbc_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_LanIntfMDBC_get(apollomp_raw_l34_limbc_t *mode)
{
    int32 ret;
    uint32 tmp_val;

     RT_PARAM_CHK((mode == NULL), RT_ERR_NULL_POINTER);


    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_LIMDBCf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    *mode = tmp_val;

    return RT_ERR_OK;

}



/* Function Name:
 *      apollomp_raw_l34_mode_set
 * Description:
 *      Set L34 mode.
 * Input:
 *      apollomp_raw_l34_mode_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_mode_set(apollomp_raw_l34_mode_t mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode > APOLLOMP_RAW_L34_L3_ENABLE_L4_ENABLE), RT_ERR_INPUT);

    tmp_val = mode;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_NATMODEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_mode_get
 * Description:
 *      Get L34 mode.
 * Input:
 *      None
 * Output:
 *      apollomp_raw_l34_mode_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_mode_get(apollomp_raw_l34_mode_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_NATMODEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    *mode = tmp_val;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_TtlOperationMode_set
 * Description:
 *      Set TTL-1 operation mode.
 * Input:
 *      rtk_enable_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_TtlOperationMode_set(rtk_enable_t mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((RTK_ENABLE_END <= mode), RT_ERR_INPUT);

    if(mode == DISABLED)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_TTL_1ENABLEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_TtlOperationMode_get
 * Description:
 *      Get TTL-1 operation mode.
 * Input:
 *      None
 * Output:
 *      rtk_enable_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_TtlOperationMode_get(rtk_enable_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_TTL_1ENABLEf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0x0 == tmp_val)
        *mode = DISABLED;
    else
        *mode = ENABLED;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_L3chksumErrAllow_set
 * Description:
 *      L3 Checksum Error Allow mode set.
 * Input:
 *      rtk_enable_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_L3chksumErrAllow_set(rtk_enable_t mode)
{
    int32 ret;
    uint32 tmp_val;
    RT_PARAM_CHK((RTK_ENABLE_END <= mode), RT_ERR_INPUT);

    if(DISABLED == mode)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_L3CHKSERRALLOWf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_L3chksumErrAllow_get
 * Description:
 *      L3 Checksum Error Allow mode get.
 * Input:
 *      None
 * Output:
 *      rtk_enable_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_L3chksumErrAllow_get(rtk_enable_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_L3CHKSERRALLOWf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0x0 == tmp_val)
        *mode = DISABLED;
    else
        *mode = ENABLED;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_L4chksumErrAllow_set
 * Description:
 *      L3 Checksum Error Allow mode set.
 * Input:
 *      rtk_enable_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_L4chksumErrAllow_set(rtk_enable_t mode)
{
    int32 ret;
    uint32 tmp_val;
    RT_PARAM_CHK((RTK_ENABLE_END <= mode), RT_ERR_INPUT);

    if(mode == DISABLED)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_L4CHKSERRALLOWf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_L4chksumErrAllow_get
 * Description:
 *      L4 Checksum Error Allow mode get.
 * Input:
 *      None
 * Output:
 *      rtk_enable_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_L4chksumErrAllow_get(rtk_enable_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_L4CHKSERRALLOWf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0x0 == tmp_val)
        *mode = DISABLED;
    else
        *mode = ENABLED;

    return RT_ERR_OK;

}



/* Function Name:
 *      apollomp_raw_l34_FragPkt2Cpu_set
 * Description:
 *      Set fragment packet trap to CPU.
 * Input:
 *      rtk_enable_t mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_FragPkt2Cpu_set(rtk_enable_t mode)
{
    int32 ret;
    uint32 tmp_val;
    RT_PARAM_CHK((RTK_ENABLE_END <= mode), RT_ERR_INPUT);

    if(mode == DISABLED)
        tmp_val = 0;
    else
        tmp_val = 1;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_FRAGMENT2CPUf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_FragPkt2Cpu_get
 * Description:
 *      Get fragment packet trap to CPU setting.
 * Input:
 *      None
 * Output:
 *      rtk_enable_t *mode
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_FragPkt2Cpu_get(rtk_enable_t *mode)
{
    int32 ret;
    uint32 tmp_val;

    RT_PARAM_CHK((mode==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_FRAGMENT2CPUf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0x0 == tmp_val)
        *mode = DISABLED;
    else
        *mode = ENABLED;

    return RT_ERR_OK;

}



/* Function Name:
 *      apollomp_raw_l34_hwL4TrfWrkTbl_set
 * Description:
 *      Set HW working table id for L4 trf.
 * Input:
 *      apollomp_raw_l34_l4_trf_t table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hwL4TrfWrkTbl_set(apollomp_raw_l34_l4_trf_t table)
{
    int32 ret,cnt=0;
    uint32 tmp_val;

    tmp_val = table;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_L4_TRF_HWWRK_SELf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    /*check HW really change complete*/
    while(1 && cnt <10)
    {
        if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_L4_TRF_CHGf, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        if(0x0 == tmp_val){
            break;
        }
      cnt ++;
    }

    l34_trf_select_table = table;
    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_hwL4TrfWrkTbl_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      apollomp_raw_l34_l4_trf_t *table
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hwL4TrfWrkTbl_get(apollomp_raw_l34_l4_trf_t *table)
{
    int32 ret;
    uint32 tmp_val;


    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_L4_TRF_HWWRK_SELf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    *table = tmp_val;

    l34_trf_select_table = tmp_val;

    return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_l4TrfTb_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      apollomp_raw_l34_l4_trf_t *table
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_l4TrfTb_get(apollomp_raw_l34_l4_trf_t table,uint32 index,rtk_enable_t *pIndicator)
{
    int32 ret;
    uint32 reg;

    if(table ==APOLLOMP_RAW_L34_L4TRF_TABLE0){
        reg = APOLLOMP_L4_TRF0r;
    }else{
        reg = APOLLOMP_L4_TRF1r;
    }
    if ((ret = reg_array_field_read(reg,REG_ARRAY_INDEX_NONE,index, APOLLOMP_TRFf, pIndicator)) != RT_ERR_OK){
         RT_ERR(ret, (MOD_L34|MOD_DAL), "");
         return ret;
    }

        return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_l34_hwArpTrfWrkTbl_set
 * Description:
 *      Set HW working table id for ARP trf.
 * Input:
 *      apollomp_raw_l34_arp_trf_t table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hwArpTrfWrkTbl_set(apollomp_raw_l34_arp_trf_t table)
{
    int32 ret;
    uint32 tmp_val;

    tmp_val = table;

    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_ARP_TRF_HWWRK_SELf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    /*check HW really change complete*/
    while(1)
    {
        if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_ARP_TRF_CHGf, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        if(0x0 == tmp_val)
            break;

    }

    arp_trf_select_table = table;

    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_hwArpTrfWrkTbl_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      None
 * Output:
 *      apollomp_raw_l34_arp_trf_t *table
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hwArpTrfWrkTbl_get(apollomp_raw_l34_arp_trf_t *table)
{
    int32 ret;
    uint32 tmp_val;


    if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_ARP_TRF_HWWRK_SELf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    *table = tmp_val;

    arp_trf_select_table = tmp_val;

    return RT_ERR_OK;

}

/* Function Name:
 *      apollomp_raw_l34_l4TrfTb_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      apollomp_raw_l34_l4_trf_t *table
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_arpTrfTb_get(apollomp_raw_l34_arp_trf_t table,uint32 index,rtk_enable_t *pIndicator)
{
    int32 ret;
    uint32 reg;

    if(table ==APOLLOMP_RAW_L34_ARPTRF_TABLE0){
        reg = APOLLOMP_ARP_TRF0r;
    }else{
        reg = APOLLOMP_ARP_TRF1r;
    }
    if ((ret = reg_array_field_read(reg,REG_ARRAY_INDEX_NONE,index, APOLLOMP_TRFf, pIndicator)) != RT_ERR_OK){
         RT_ERR(ret, (MOD_L34|MOD_DAL), "");
         return ret;
    }

        return RT_ERR_OK;

}


/* Function Name:
 *      apollomp_raw_l34_hwArpTrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      apollomp_raw_l34_arp_trf_t table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hwArpTrfWrkTbl_Clear(apollomp_raw_l34_arp_trf_t table)
{
    int32 ret;
    uint32 tmp_val;

    if(table == APOLLOMP_RAW_L34_ARPTRF_TABLE0)
        tmp_val = 1;
    else
        tmp_val = 2;


    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_ARP_TRF_EXEC_CLRf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    /*check HW really clear complete*/
    while(1)
    {
        if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_ARP_TRF_EXEC_CLRf, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        if(0x0 == tmp_val)
            break;

    }


    return RT_ERR_OK;

}




/* Function Name:
 *      apollomp_raw_l34_hwL4TrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      apollomp_raw_l34_l4_trf_t table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - Success
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      None
 */
int32 apollomp_raw_l34_hwL4TrfWrkTbl_Clear(apollomp_raw_l34_l4_trf_t table)
{
    int32 ret,cnt=0;
    uint32 tmp_val;

    if(table == APOLLOMP_RAW_L34_L4TRF_TABLE0)
        tmp_val = 1;
    else
        tmp_val = 2;


    if ((ret = reg_field_write(APOLLOMP_SWTCR0r, APOLLOMP_L4_TRF_EXEC_CLRf, (uint32 *)&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    /*check HW really clear complete*/
    while(1 && cnt <10)
    {
        if ((ret = reg_field_read(APOLLOMP_SWTCR0r, APOLLOMP_L4_TRF_EXEC_CLRf, (uint32 *)&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L34|MOD_DAL), "");
            return ret;
        }
        if( 0x0 == tmp_val){
            break;
        }
      cnt++;
    }

    return RT_ERR_OK;

}

int32
apollomp_raw_l34_portToWanAction_set(rtk_port_t port, uint32 wanIdx, rtk_action_t action)
{
    int32 ret;
    uint32 writeVal;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if(action==ACTION_FORWARD)
        writeVal = 1;
    else if(action==ACTION_DROP)
        writeVal = 0;
    else
        return RT_ERR_INPUT;

    if ((ret = reg_array_field_write(APOLLOMP_L34_PORT_TO_WANr, port, wanIdx, APOLLOMP_PORT_TO_WAN_PERMITf, &writeVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_portToWanAction_get(rtk_port_t port, uint32 wanIdx, rtk_action_t *pAction)
{
    int32 ret;
    uint32 val;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(APOLLOMP_L34_PORT_TO_WANr, port, wanIdx, APOLLOMP_PORT_TO_WAN_PERMITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(1==val)
        *pAction = ACTION_FORWARD;
    else
        *pAction = ACTION_DROP;

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_extPortToWanAction_set(rtk_port_t extPort, uint32 wanIdx, rtk_action_t action)
{
    int32 ret;
    uint32 writeVal;

    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if(action==ACTION_FORWARD)
        writeVal = 1;
    else if(action==ACTION_DROP)
        writeVal = 0;
    else
        return RT_ERR_INPUT;

    if ((ret = reg_array_field_write(APOLLOMP_L34_EXTPORT_TO_WANr, extPort, wanIdx, APOLLOMP_EXTPORT_TO_WAN_PERMITf, &writeVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_extPortToWanAction_get(rtk_port_t extPort, uint32 wanIdx, rtk_action_t *pAction)
{
    int32 ret;
    uint32 val;

    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(APOLLOMP_L34_EXTPORT_TO_WANr, extPort, wanIdx, APOLLOMP_EXTPORT_TO_WAN_PERMITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(1==val)
        *pAction = ACTION_FORWARD;
    else
        *pAction = ACTION_DROP;

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_wanToPortAction_set(uint32 wanIdx, rtk_port_t port, rtk_action_t action)
{
    int32 ret;
    uint32 writeVal;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if(action==ACTION_FORWARD)
        writeVal = 1;
    else if(action==ACTION_DROP)
        writeVal = 0;
    else
        return RT_ERR_INPUT;

    if ((ret = reg_array_field_write(APOLLOMP_L34_WAN_TO_PORTr, port, wanIdx, APOLLOMP_WAN_TO_PORT_PERMITf, &writeVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_wanToPortAction_get(uint32 wanIdx, rtk_port_t port, rtk_action_t *pAction)
{
    int32 ret;
    uint32 val;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(APOLLOMP_L34_WAN_TO_PORTr, port, wanIdx, APOLLOMP_WAN_TO_PORT_PERMITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(1==val)
        *pAction = ACTION_FORWARD;
    else
        *pAction = ACTION_DROP;

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_wanToExtPortAction_set(uint32 wanIdx, rtk_port_t extPort, rtk_action_t action)
{
    int32 ret;
    uint32 writeVal;

    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if(action==ACTION_FORWARD)
        writeVal = 1;
    else if(action==ACTION_DROP)
        writeVal = 0;
    else
        return RT_ERR_INPUT;

    if ((ret = reg_array_field_write(APOLLOMP_L34_WAN_TO_EXTPORTr, extPort, wanIdx, APOLLOMP_WAN_TO_EXTPORT_PERMITf, &writeVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_wanToExtPortAction_get(uint32 wanIdx, rtk_port_t extPort, rtk_action_t *pAction)
{
    int32 ret;
    uint32 val;

    RT_PARAM_CHK(!HAL_IS_EXT_PORT(extPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=wanIdx), RT_ERR_INPUT);

    if ((ret = reg_array_field_read(APOLLOMP_L34_WAN_TO_EXTPORTr, extPort, wanIdx, APOLLOMP_WAN_TO_EXTPORT_PERMITf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(1==val)
        *pAction = ACTION_FORWARD;
    else
        *pAction = ACTION_DROP;

    return RT_ERR_OK;
}



int32
apollomp_raw_l34_ipmcTransEntry_set(apollomp_raw_l34_ipmcTransEntry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    uint32 index;

    RT_PARAM_CHK((NULL == entry), RT_ERR_NULL_POINTER);
    index = entry->index;
    RT_PARAM_CHK((APOLLOMP_IPMC_TRANS_MAX < index), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->netifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= entry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX() <= entry->extipIdx), RT_ERR_INPUT);

    tmpVal = entry->netifIdx;
    if ((ret = reg_array_field_write(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NETIF_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry->sipTransEnable;
    if ((ret = reg_array_field_write(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EN_SIP_TRANSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry->extipIdx;
    if ((ret = reg_array_field_write(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EXT_IP_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry->pppoeAct;
    if ((ret = reg_array_field_write(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PPPOE_ACTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    tmpVal = entry->pppoeIdx;
    if ((ret = reg_array_field_write(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PPPOE_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_ipmcTransEntry_get(apollomp_raw_l34_ipmcTransEntry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    uint32 index;

    RT_PARAM_CHK((NULL == entry), RT_ERR_NULL_POINTER);
    index = entry->index;
    RT_PARAM_CHK((APOLLOMP_IPMC_TRANS_MAX < index), RT_ERR_ENTRY_INDEX);

    if ((ret = reg_array_field_read(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_NETIF_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    entry->netifIdx = tmpVal;


    if ((ret = reg_array_field_read(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EN_SIP_TRANSf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    entry->sipTransEnable = tmpVal;

    if ((ret = reg_array_field_read(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EXT_IP_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    entry->extipIdx = tmpVal;

    if ((ret = reg_array_field_read(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PPPOE_ACTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    entry->pppoeAct = tmpVal;

    if ((ret = reg_array_field_read(APOLLOMP_L34_IPMC_TRAN_TBLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_PPPOE_IDXf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    entry->pppoeIdx = tmpVal;

    return RT_ERR_OK;
}


int32
apollomp_raw_l34_ipmcTtlFunction_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpVal;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable == DISABLED)
        tmpVal = 0;
    else
        tmpVal = 1;

    if ((ret = reg_field_write(APOLLOMP_L34_IPMC_TTL_CFGr, APOLLOMP_IP_MCST_TTL_1f, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_ipmcTtlFunction_get(rtk_enable_t *enable)
{
    int32 ret;
    uint32 tmpVal;

    RT_PARAM_CHK((enable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_L34_IPMC_TTL_CFGr, APOLLOMP_IP_MCST_TTL_1f, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0x0 == tmpVal)
        *enable = DISABLED;
    else
        *enable = ENABLED;

    return RT_ERR_OK;
}




int32
apollomp_raw_l34_bindingAction_set(apollomp_l34_bindType_t type, uint32 action)
{
    int32 ret;
    uint32 tmpVal;
    uint32 field;

    RT_PARAM_CHK((APOLLOMP_L34_BIND_TYPE_END <= type), RT_ERR_INPUT);

    switch(type)
    {
        case APOLLOMP_L34_BIND_UNMATCHED_L2L3:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L2L3f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L2L34:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L2L34f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L3L2:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L3L2f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L3L34:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L3L34f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L34L2:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L34L2f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L34L3:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L34L3f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L3L3:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L3L3f;
            break;

        case APOLLOMP_L34_BIND_CUSTOMIZED_L2:
            field = APOLLOMP_WAN_BINDING_CUSTOMIZED_L2f;
            break;

        case APOLLOMP_L34_BIND_CUSTOMIZED_L3:
            field = APOLLOMP_WAN_BINDING_CUSTOMIZED_L3f;
            break;

        case APOLLOMP_L34_BIND_CUSTOMIZED_L34:
            field = APOLLOMP_WAN_BINDING_CUSTOMIZED_L34f;
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    tmpVal = action;
    if ((ret = reg_field_write(APOLLOMP_BD_CFGr, field, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


int32
apollomp_raw_l34_bindingAction_get(apollomp_l34_bindType_t type, uint32 *action)
{
    int32 ret;
    uint32 tmpVal;
    uint32 field;

    RT_PARAM_CHK((action==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_BIND_TYPE_END <= type), RT_ERR_INPUT);

    switch(type)
    {
        case APOLLOMP_L34_BIND_UNMATCHED_L2L3:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L2L3f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L2L34:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L2L34f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L3L2:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L3L2f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L3L34:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L3L34f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L34L2:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L34L2f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L34L3:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L34L3f;
            break;

        case APOLLOMP_L34_BIND_UNMATCHED_L3L3:
            field = APOLLOMP_WAN_BINDING_UNMATCHED_L3L3f;
            break;

        case APOLLOMP_L34_BIND_CUSTOMIZED_L2:
            field = APOLLOMP_WAN_BINDING_CUSTOMIZED_L2f;
            break;

        case APOLLOMP_L34_BIND_CUSTOMIZED_L3:
            field = APOLLOMP_WAN_BINDING_CUSTOMIZED_L3f;
            break;

        case APOLLOMP_L34_BIND_CUSTOMIZED_L34:
            field = APOLLOMP_WAN_BINDING_CUSTOMIZED_L34f;
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    if ((ret = reg_field_read(APOLLOMP_BD_CFGr, field, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }
    *action = tmpVal;

    return RT_ERR_OK;
}



int32
apollomp_raw_l34_bindFunction_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpVal;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable == DISABLED)
        tmpVal = 0;
    else
        tmpVal = 1;

    if ((ret = reg_field_write(APOLLOMP_V6_BD_CTLr, APOLLOMP_PB_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

int32
apollomp_raw_l34_bindFunction_get(rtk_enable_t *enable)
{
    int32 ret;
    uint32 tmpVal;

    RT_PARAM_CHK((enable==NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_V6_BD_CTLr, APOLLOMP_PB_ENf, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0x0 == tmpVal)
        *enable = DISABLED;
    else
        *enable = ENABLED;

    return RT_ERR_OK;
}


int32
apollomp_raw_l34_globalFunction_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpVal;

    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(enable == DISABLED)
        tmpVal = 0;
    else
        tmpVal = 1;

    if ((ret = reg_field_write(APOLLOMP_L34_GLB_CFGr, APOLLOMP_L34_GLOBAL_CFGf, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



int32
apollomp_raw_l34_globalFunction_get(rtk_enable_t *enable)
{
    int32 ret;
    uint32 tmpVal;

    RT_PARAM_CHK((NULL == enable), RT_ERR_NULL_POINTER);


    if ((ret = reg_field_read(APOLLOMP_L34_GLB_CFGr, APOLLOMP_L34_GLOBAL_CFGf, (uint32 *)&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L34|MOD_DAL), "");
        return ret;
    }

    if(0 == tmpVal)
        *enable = DISABLED;
    else
        *enable = ENABLED;


    return RT_ERR_OK;
}

