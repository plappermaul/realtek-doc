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
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_hwmisc.h>
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#include <rtk/switch.h>
#include <ioal/mem32.h>

#if defined(__KERNEL__)
#include <linux/delay.h>
#define REG_DELAY 30
#endif

/* Function Name:
 *      rtl9607c_virtualMacMode_Set(
 * Description:
 *      set virtual MAC mode
 * Input:
 *      port     - port number
 *      enable   - enable virtual mac on this port
 * Output:
 *      mode
 * Return:
 *      RT_ERR_OK 				- Success
 * Note:
 *      None
 */
int32 rtl9607c_virtualMacMode_Set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpVal;
    
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable>=RTK_ENABLE_END, RT_ERR_INPUT);


    /* enable/disable VM mac*/
    tmpVal = enable;
    if ((ret = reg_array_field_write(RTL9607C_PORT_VM_ENr,port,REG_ARRAY_INDEX_NONE,RTL9607C_PORT_VM_ENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return ret;
    }
    
	return RT_ERR_OK;    
}


/* Function Name:
 *      rtl9607c_virtualMac_Input(
 * Description:
 *      virtual MAC input
 * Input:
 *      fromPort - input packet to indicate port number
 *      pPkt     - input packet buffer, this buffer must include FCS
 *      len      - input packet length, include FCS length
 * Output:
 *      mode
 * Return:
 *      RT_ERR_OK 				- Success
 * Note:
 *      None
 */
int32 rtl9607c_virtualMac_Input(rtk_port_t fromPort, uint8 *pPkt, uint32 len)
{
    int32 ret;
    uint32 tmpVal;
    uint32 regData;
    uint32 index;
    uint8 *pPktPtr;
    
    RT_PARAM_CHK((pPkt==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(fromPort), RT_ERR_PORT_ID);
    
    /* general dumy data*/
    regData = 0;
    for(index =0; index < RTL9607C_VIR_MAC_DUMMY_CYCLE ;index++)    
    {
        if ((ret = reg_array_write(RTL9607C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
    }

    /*start send packet contant*/
    /*preamble*/
    for(index =0; index < 8 ;index++)    
    {
        tmpVal = 1;
        regData = 0;
        if ((ret = reg_field_set(RTL9607C_PORT_VM_RXr, RTL9607C_PORT_VM_RXDVf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if(7 == index)
            tmpVal = 0xd5;
        else
            tmpVal = 0x55;
        if ((ret = reg_field_set(RTL9607C_PORT_VM_RXr, RTL9607C_PORT_VM_RXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif

        if ((ret = reg_array_write(RTL9607C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
            
    }
    
    /*data*/
    pPktPtr = pPkt;
    for(index =0; index < len ;index++)    
    {
        tmpVal = 1;
        if ((ret = reg_field_set(RTL9607C_PORT_VM_RXr, RTL9607C_PORT_VM_RXDVf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        tmpVal = *pPktPtr;
        if ((ret = reg_field_set(RTL9607C_PORT_VM_RXr,RTL9607C_PORT_VM_RXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_array_write(RTL9607C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        pPktPtr++;    
    }

   
    /*send dumy*/
    /* general dumy data*/
    regData = 0;

    for(index =0; index < RTL9607C_VIR_MAC_DUMMY_CYCLE ;index++)    
    {
        if ((ret = reg_array_write(RTL9607C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
    }
    

	return RT_ERR_OK;
}



/* Function Name:
 *      rtl9607c_virtualMac_Output(
 * Description:
 *      virtual MAC outpit
 * Input:
 *      toPort   - Get output packet from indicate port number
 * Output:
 *      pPkt     - Get output packet buffer, this buffer include FCS
 *      len      - input packet length, include FCS length
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_TIMEOUT
 * Note:
 *      None
 */
int32 rtl9607c_virtualMac_Output(rtk_port_t toPort, uint8 *pPkt, uint32 *plen)
{
    int32 ret;
    uint32 tmpVal;
    uint32 regData;
    uint32 index;
    uint8 *pPktPtr;
    uint32 isGetPkt;
    uint32 pktlen;
    uint32 bufLen;
    
    RT_PARAM_CHK((pPkt==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((plen==NULL), RT_ERR_NULL_POINTER);    
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(toPort), RT_ERR_PORT_ID);

    /*init packet length*/
    bufLen = *plen;
    *plen = 0;
    
    /* get dumy data and preamble*/
    regData = 0;
    isGetPkt = FALSE;
    for(index =0; index < RTL9607C_VIR_MAC_TX_MAX_CNT ;index++)    
    {
        if ((ret = reg_array_read(RTL9607C_PORT_VM_TXr,toPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif

        if ((ret = reg_field_get(RTL9607C_PORT_VM_TXr, RTL9607C_PORT_VM_TXENf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if(0 == tmpVal)
            continue;

        if ((ret = reg_field_get(RTL9607C_PORT_VM_TXr, RTL9607C_PORT_VM_TXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        } 
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        /*get start of frame*/       
        if(0xd5 == tmpVal)
        {
            isGetPkt = TRUE;
            break;
        }
    }

    if(isGetPkt != TRUE)
        return RT_ERR_TIMEOUT;
    
    pktlen=0;
    pPktPtr = pPkt;
    /*start get packet contant*/
    do   
    {
        if ((ret = reg_array_read(RTL9607C_PORT_VM_TXr,toPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_field_get(RTL9607C_PORT_VM_TXr, RTL9607C_PORT_VM_TXENf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if(0 == tmpVal)
            break;

        if ((ret = reg_field_get(RTL9607C_PORT_VM_TXr, RTL9607C_PORT_VM_TXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if(pktlen < bufLen)
        {
            *pPktPtr=tmpVal;
            pPktPtr++;
        }
        pktlen++;
            
        
    }while(1);
    
    *plen = pktlen;
    
    /*dummy read*/
     for(index =0; index < RTL9607C_VIR_MAC_DUMMY_CYCLE ;index++)    
    {
        if ((ret = reg_array_read(RTL9607C_PORT_VM_TXr,toPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_field_get(RTL9607C_PORT_VM_TXr, RTL9607C_PORT_VM_TXENf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(__KERNEL__)
		udelay(REG_DELAY);
#endif
        /*get next packt stop dummy read*/
        if(1 == tmpVal)
            break;
    }

 
	return RT_ERR_OK;
}

/* Function Name:
 *      rtl9607c_hsbData_get
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
int32 rtl9607c_hsbData_get(rtk_hsb_t *hsbData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtk_mac_t tmp_da;
    rtk_mac_t tmp_sa;
	rtk_ipv6_addr_t tmp_ip6;
    rtl9607c_hsb_entry_t hsb_entry2;
    rtl9607c_hsb_entry_t hsb_entry;
	uint32 chipid, rev, dmy_type;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsb_entry2;
    for(index=0 ; index< (sizeof(rtl9607c_hsb_entry_t)/4) ; index++)
    {
		if ((ret = reg_array_read(RTL9607C_HSB_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9607c_hsb_entry_t)/4) ; index++)
    {
        hsb_entry.entry_data[sizeof(rtl9607c_hsb_entry_t)/4 - 1 - index] = hsb_entry2.entry_data[index];
    }

	if((ret = dal_rtl9607c_switch_version_get(&chipid, &rev, &dmy_type) != RT_ERR_OK))
	{
		return RT_ERR_FAILED;	
	}
	
	if(CHIP_REV_ID_B <= rev)
	{
		
		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->bgdsc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->endsc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_7tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_7 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_6tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_6 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_5 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_4 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_3 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_2 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_1 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_FIELD_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_field_0 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_USER_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->user_valid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DEFAULT_FILED_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->default_field_3 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DEFAULT_FILED_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->default_field_2 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DEFAULT_FILED_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->default_field_1 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DEFAULT_FILED_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->default_field_0 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_GRE_SEQ_FLAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->if_gre_seq = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SESSION_SEQNOtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->pptp_l2tp_seq_session = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_L3PROTOCOL_NHtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ip_proto_nh = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsbData->dip6.ipv6_addr[0],&tmp_ip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));

		if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsbData->sip6.ipv6_addr[0],&tmp_ip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));

		if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->dip2 = tmp_ip6.ipv6_addr[12];
	    hsbData->dip2 = (hsbData->dip2 << 8 ) | tmp_ip6.ipv6_addr[13];
	    hsbData->dip2 = (hsbData->dip2 << 8 ) | tmp_ip6.ipv6_addr[14];
	    hsbData->dip2 = (hsbData->dip2 << 8 ) | tmp_ip6.ipv6_addr[15];

		if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->sip2 = tmp_ip6.ipv6_addr[12];
	    hsbData->sip2 = (hsbData->sip2 << 8 ) | tmp_ip6.ipv6_addr[13];
	    hsbData->sip2 = (hsbData->sip2 << 8 ) | tmp_ip6.ipv6_addr[14];
	    hsbData->sip2 = (hsbData->sip2 << 8 ) | tmp_ip6.ipv6_addr[15];

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_IP6_HN_RGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ip6_nh_rg = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CKS_OK_L4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cks_ok_l4 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CKS_OK_L3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cks_ok_l3 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_TTL_GT1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ttl_gt1 = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_IHL_GT5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ihl_gt5 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CKS_OK_L4_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cks_ok_l4_inner = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CKS_OK_L3_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cks_ok_l3_inner = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_TTL_GT1_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ttl_gt1_inner = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_IHL_GT5_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ihl_gt5_inner = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_L4_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->l4_type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_PPTP_L2TP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->pptp_l2tp_id = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_PPPOE_SESSIONtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->pppoe_session = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->dip = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->sip = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_TCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->tos_tc_inner = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_TOStf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->tos_tc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_IP6_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ip6_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_IP2_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ip4_inner_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_IP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ip4_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_PTP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ptp_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_OAMPDUtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->oampdu = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_RLDP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->rldp_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_LLC_OTHERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->llc_other = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DUAL_IPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->dual_ip = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->pppoe_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SNAP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->snap_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_ETHER_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ether_type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ctag = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->ctag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_STAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->stag = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_STAG_TPIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->stag_tpid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->stag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_pon_sid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PPPOE_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_pppoe_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_pppoe_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_extspa	= tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_DIRTXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_dirtx = tmp_val;
	  
		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PSELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_psel = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_DISLRNtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_dislrn = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_keep = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PRISELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_prisel = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PMSK_10_8tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_pmsk_10_8 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_PMSK_7_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_pmsk_7_0 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->cputag_if = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_SAtf, (uint8 *)&tmp_sa.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsbData->sa.octet[0],&tmp_sa.octet[0],sizeof(rtk_mac_t));

		if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_DAtf, (uint8 *)&tmp_da.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsbData->da.octet[0],&tmp_da.octet[0],sizeof(rtk_mac_t));

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_PON_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->pon_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLE2t, RTL9607C_HSB_DATA_TABLE2_PKT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsbData->pkt_len = tmp_val;

	}
	else 
	{

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->bgdsc = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->endsc = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->spa = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_7tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_7 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_6tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_6 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_5 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_4 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_3 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_2 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_1 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_FIELD_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_field_0 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_USER_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->user_valid = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DEFAULT_FILED_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->default_field_3 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DEFAULT_FILED_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->default_field_2 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DEFAULT_FILED_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->default_field_1 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DEFAULT_FILED_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->default_field_0 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_GRE_SEQ_FLAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->if_gre_seq = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SESSION_SEQNOtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->pptp_l2tp_seq_session = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_L3PROTOCOL_NHtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ip_proto_nh = tmp_val;

	    if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    osal_memcpy(&hsbData->dip6.ipv6_addr[0],&tmp_ip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));

	    if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    osal_memcpy(&hsbData->sip6.ipv6_addr[0],&tmp_ip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));

	    if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->dip2 = tmp_ip6.ipv6_addr[12];
	    hsbData->dip2 = (hsbData->dip2 << 8 ) | tmp_ip6.ipv6_addr[13];
	    hsbData->dip2 = (hsbData->dip2 << 8 ) | tmp_ip6.ipv6_addr[14];
	    hsbData->dip2 = (hsbData->dip2 << 8 ) | tmp_ip6.ipv6_addr[15];

	    if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->sip2 = tmp_ip6.ipv6_addr[12];
	    hsbData->sip2 = (hsbData->sip2 << 8 ) | tmp_ip6.ipv6_addr[13];
	    hsbData->sip2 = (hsbData->sip2 << 8 ) | tmp_ip6.ipv6_addr[14];
	    hsbData->sip2 = (hsbData->sip2 << 8 ) | tmp_ip6.ipv6_addr[15];

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_IP6_HN_RGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ip6_nh_rg = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CKS_OK_L4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cks_ok_l4 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CKS_OK_L3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cks_ok_l3 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_TTL_GT1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ttl_gt1 = tmp_val;

#if 0 /* Field not exist */
	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_IHL_GT5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ihl_gt5 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CKS_OK_L4_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cks_ok_l4_inner = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CKS_OK_L3_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cks_ok_l3_inner = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_TTL_GT1_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ttl_gt1_inner = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_IHL_GT5_INNERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ihl_gt5_inner = tmp_val;
#endif


	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_L4_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->l4_type = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_PPTP_L2TP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->pptp_l2tp_id = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_PPPOE_SESSIONtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->pppoe_session = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->dip = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->sip = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_TCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->tos_tc_inner = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_TOStf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->tos_tc = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_IP6_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ip6_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_IP2_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ip4_inner_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_IP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ip4_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_PTP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ptp_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_OAMPDUtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->oampdu = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_RLDP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->rldp_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_LLC_OTHERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->llc_other = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DUAL_IPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->dual_ip = tmp_val;


	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->pppoe_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SNAP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->snap_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_ETHER_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ether_type = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ctag = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->ctag_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_STAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->stag = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_STAG_TPIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->stag_tpid = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->stag_if = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_pon_sid = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PPPOE_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_pppoe_idx = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_pppoe_act = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_extspa  = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_DIRTXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_dirtx = tmp_val;
	  
	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PSELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_psel = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_DISLRNtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_dislrn = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_keep = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_pri = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PRISELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_prisel = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PMSK_10_8tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_pmsk_10_8 = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_PMSK_7_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_pmsk_7_0 = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->cputag_if = tmp_val;

	    if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_SAtf, (uint8 *)&tmp_sa.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    osal_memcpy(&hsbData->sa.octet[0],&tmp_sa.octet[0],sizeof(rtk_mac_t));

	    if ((ret = table_field_byte_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_DAtf, (uint8 *)&tmp_da.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    osal_memcpy(&hsbData->da.octet[0],&tmp_da.octet[0],sizeof(rtk_mac_t));

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_PON_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->pon_idx = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSB_DATA_TABLEt, RTL9607C_HSB_DATA_TABLE_PKT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsbData->pkt_len = tmp_val;
	}
    return RT_ERR_OK;
} /* end of rtl9602c_hsbData_get */

/* Function Name:
 *      rtl9607c_hsaData_get
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
int32 rtl9607c_hsaData_get(rtk_hsa_t *hsaData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    uint8 tmp_buf[9];
    rtl9607c_hsa_entry_t hsa_entry;
    rtl9607c_hsa_entry_t hsa_entry2;
    rtk_mac_t tmp_mac;
	uint32 chipid, rev, dmy_type;

    RT_PARAM_CHK((hsaData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsa_entry2;
    for(index=0 ; index<(sizeof(rtl9607c_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9607C_HSA_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9607c_hsa_entry_t)/4) ; index++)
    {
        hsa_entry.entry_data[sizeof(rtl9607c_hsa_entry_t)/4 - 1 - index] = hsa_entry2.entry_data[index];

    }
	
	if((ret = dal_rtl9607c_switch_version_get(&chipid, &rev, &dmy_type) != RT_ERR_OK))
	{
		return RT_ERR_FAILED;	
	}
		
	if(CHIP_REV_ID_B <= rev)
	{
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_bgdsc = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_DSPBOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_dspbo = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_endsc = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_pktlen = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_sid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_floodpkt = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_QIDtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}

		hsaData->dbghsa_oq_qid[0] = tmp_buf[4] & 0x7;
		hsaData->dbghsa_oq_qid[1] = (tmp_buf[4] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[2] = ((tmp_buf[3] << 2) & 0x4) | ((tmp_buf[4] >> 6) & 0x3);
		hsaData->dbghsa_oq_qid[3] = (tmp_buf[3] >> 1) & 0x7;
		hsaData->dbghsa_oq_qid[4] = (tmp_buf[3] >> 4) & 0x7;
		hsaData->dbghsa_oq_qid[5] = ((tmp_buf[2] << 1) & 0x6) | ((tmp_buf[3] >> 7) & 0x1);
		hsaData->dbghsa_oq_qid[6] = (tmp_buf[2] >> 2) & 0x7;
		hsaData->dbghsa_oq_qid[7] = (tmp_buf[2] >> 5) & 0x7;
		hsaData->dbghsa_oq_qid[8] = tmp_buf[1] & 0x7;
		hsaData->dbghsa_oq_qid[9] = (tmp_buf[1] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[10] = ((tmp_buf[0] << 2) & 0x4) | ((tmp_buf[1] >> 6) & 0x3);

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_DPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_dpm = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_fb = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_DRPPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_drppkt = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_MTR_DA_DRPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_mtr_da_drp = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_SAMIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_samib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_SAMIB_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_samib_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_CPU_FORCE_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_cpu_forced_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_PON_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_PON_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_endsc = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_TRAP_HASHtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_trap_hash = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_1PREM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_1PREM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_tagcfi = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_PISO_LEAKYtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_piso_leaky = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_USERPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_userpri = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PBODPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pbodmp = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PBOQIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}

		hsaData->dbghsa_epcom_pboqid[0] = tmp_val & 0x7;
		hsaData->dbghsa_epcom_pboqid[1] = (tmp_val >> 3) & 0x7;
		hsaData->dbghsa_epcom_pboqid[2] = (tmp_val >> 6) & 0x7;
		hsaData->dbghsa_epcom_pboqid[3] = (tmp_val >> 9) & 0x7;
		hsaData->dbghsa_epcom_pboqid[4] = (tmp_val >> 12) & 0x7;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FLOW_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_flow_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FLOW_MIB_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_flow_mib_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SINGLEIP_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_singleip_chksum = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SINGLEIP_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_singleip_chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_keep = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_CPUKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cpukeep = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FRM_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_frm_fb = tmp_val; 

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PPPOE_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_sid = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsaData->dbghsa_epcom_dmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_NOCPUTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_nocputag = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DUALHDR_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DUALHDR_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DA_HOST_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DA_HOST_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagcfi= tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SP2C_SRCVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_sp2c_srcvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DPCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DMABCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmabc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_ponsid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_CPUTAG_ORGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_org = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extspa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FWDRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_fwdrsn = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_EXTMASK_HASH_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extmask_hash_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DPM_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpm_type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_EPTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_eptype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_svlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_ptp_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_ptp_id = tmp_val;


		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_SECtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsaData->dbghsa_ptp_sec[0],&tmp_buf[0],6);

		if ((ret = table_field_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_NSECtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_ptp_nsec = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCI2t, RTL9607C_HSA_DATA_OMCI2_DBGHSA_OMCI_DEV_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_omci_dev_id = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCI2t, RTL9607C_HSA_DATA_OMCI2_DBGHSA_OMCI_CONT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_omci_cont_len = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_svlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_SRCMODtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_srcmod = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_L3TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l4chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_L3CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_NEWPORTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newport = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newip = tmp_val;

	}
	else
	{
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_bgdsc = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_endsc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_pktlen = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_sid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_floodpkt = tmp_val;

	    if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_QIDtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }

		hsaData->dbghsa_oq_qid[0] = tmp_buf[4] & 0x7;
		hsaData->dbghsa_oq_qid[1] = (tmp_buf[4] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[2] = ((tmp_buf[3] << 2) & 0x4) | ((tmp_buf[4] >> 6) & 0x3);
		hsaData->dbghsa_oq_qid[3] = (tmp_buf[3] >> 1) & 0x7;
		hsaData->dbghsa_oq_qid[4] = (tmp_buf[3] >> 4) & 0x7;
		hsaData->dbghsa_oq_qid[5] = ((tmp_buf[2] << 1) & 0x6) | ((tmp_buf[3] >> 7) & 0x1);
		hsaData->dbghsa_oq_qid[6] = (tmp_buf[2] >> 2) & 0x7;
		hsaData->dbghsa_oq_qid[7] = (tmp_buf[2] >> 5) & 0x7;
		hsaData->dbghsa_oq_qid[8] = tmp_buf[1] & 0x7;
		hsaData->dbghsa_oq_qid[9] = (tmp_buf[1] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[10] = ((tmp_buf[0] << 2) & 0x4) | ((tmp_buf[1] >> 6) & 0x3);

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_DPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_dpm = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_fb = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_DRPPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_drppkt = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_PON_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_PON_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_endsc = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_TRAP_HASHtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_trap_hash = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_1PREM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_1PREM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_tagcfi = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_PISO_LEAKYtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_piso_leaky = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_USERPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_userpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_SINGLEIP_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_singleip_chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_keep = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cpukeep = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_FRM_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_frm_fb = tmp_val; 

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_PPPOE_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_sid = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsaData->dbghsa_epcom_dmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_NOCPUTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_nocputag = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DUALHDR_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DUALHDR_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DA_HOST_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DA_HOST_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagcfi= tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_SP2C_SRCVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_sp2c_srcvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DPCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DMABCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmabc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_ponsid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_pri = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUTAG_L3Rtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_l3r = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUTAG_ORGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_org = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extspa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_FWDRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_fwdrsn = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_EXTMASK_HASH_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extmask_hash_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DPM_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpm_type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_EPTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_eptype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_svlan_info = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_ptp_act = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_ptp_id = tmp_val;


	    if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_SECtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    osal_memcpy(&hsaData->dbghsa_ptp_sec[0],&tmp_buf[0],6);

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_NSECtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_ptp_nsec = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCIt, RTL9607C_HSA_DATA_OMCI_DBGHSA_OMCI_DEV_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_omci_dev_id = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCIt, RTL9607C_HSA_DATA_OMCI_DBGHSA_OMCI_CONT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_omci_cont_len = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_svlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_SRCMODtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_srcmod = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_L3TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l4chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_L3CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_NEWPORTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newport = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newip = tmp_val;
	}		
	  
    return RT_ERR_OK;
} /* end of rtl9607c_hsaData_get */


/* Function Name:
 *      rtl9607c_fbData_get
 * Description:
 *      Get FB HSA data
 * Input:
 *      None
 * Output:
 *      hsaData
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 rtl9607c_fbData_get(rtk_hsa_t *hsaData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    uint8 tmp_buf[9];
    rtl9607c_hsa_entry_t hsa_entry;
    rtl9607c_hsa_entry_t hsa_entry2;
    rtk_mac_t tmp_mac;
	uint32 chipid, rev, dmy_type;

    RT_PARAM_CHK((hsaData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsa_entry2;
    for(index=0 ; index<(sizeof(rtl9607c_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9607C_FBHSA_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9607c_hsa_entry_t)/4) ; index++)
    {
        hsa_entry.entry_data[sizeof(rtl9607c_hsa_entry_t)/4 - 1 - index] = hsa_entry2.entry_data[index];

    }

	
	if((ret = dal_rtl9607c_switch_version_get(&chipid, &rev, &dmy_type) != RT_ERR_OK))
	{
		return RT_ERR_FAILED;	
	}
		
	if(CHIP_REV_ID_B <= rev)
	{
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_bgdsc = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_DSPBOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_dspbo = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_endsc = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_pktlen = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_sid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_floodpkt = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_QIDtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}

		hsaData->dbghsa_oq_qid[0] = tmp_buf[4] & 0x7;
		hsaData->dbghsa_oq_qid[1] = (tmp_buf[4] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[2] = ((tmp_buf[3] << 2) & 0x4) | ((tmp_buf[4] >> 6) & 0x3);
		hsaData->dbghsa_oq_qid[3] = (tmp_buf[3] >> 1) & 0x7;
		hsaData->dbghsa_oq_qid[4] = (tmp_buf[3] >> 4) & 0x7;
		hsaData->dbghsa_oq_qid[5] = ((tmp_buf[2] << 1) & 0x6) | ((tmp_buf[3] >> 7) & 0x1);
		hsaData->dbghsa_oq_qid[6] = (tmp_buf[2] >> 2) & 0x7;
		hsaData->dbghsa_oq_qid[7] = (tmp_buf[2] >> 5) & 0x7;
		hsaData->dbghsa_oq_qid[8] = tmp_buf[1] & 0x7;
		hsaData->dbghsa_oq_qid[9] = (tmp_buf[1] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[10] = ((tmp_buf[0] << 2) & 0x4) | ((tmp_buf[1] >> 6) & 0x3);

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_DPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_dpm = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_fb = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_OQ_DRPPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_drppkt = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_MTR_DA_DRPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_mtr_da_drp = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_SAMIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_samib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_SAMIB_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_samib_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_CPU_FORCE_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_cpu_forced_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_PON_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_PON_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_endsc = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_TRAP_HASHtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_trap_hash = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_1PREM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_1PREM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_tagcfi = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_PISO_LEAKYtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_piso_leaky = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FB2t, RTL9607C_HSA_DATA_FB2_DBGHSA_L2HSA_USERPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_userpri = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PBODPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pbodmp = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PBOQIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}

		hsaData->dbghsa_epcom_pboqid[0] = tmp_val & 0x7;
		hsaData->dbghsa_epcom_pboqid[1] = (tmp_val >> 3) & 0x7;
		hsaData->dbghsa_epcom_pboqid[2] = (tmp_val >> 6) & 0x7;
		hsaData->dbghsa_epcom_pboqid[3] = (tmp_val >> 9) & 0x7;
		hsaData->dbghsa_epcom_pboqid[4] = (tmp_val >> 12) & 0x7;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FLOW_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_flow_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FLOW_MIB_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_flow_mib_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SINGLEIP_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_singleip_chksum = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SINGLEIP_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_singleip_chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_keep = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_CPUKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cpukeep = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FRM_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_frm_fb = tmp_val; 

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PPPOE_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_sid = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsaData->dbghsa_epcom_dmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_NOCPUTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_nocputag = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DUALHDR_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DUALHDR_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DA_HOST_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DA_HOST_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_S_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagcfi= tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_C_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SP2C_SRCVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_sp2c_srcvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DPCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DMABCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmabc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_ponsid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_CPUTAG_ORGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_org = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extspa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_FWDRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_fwdrsn = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_EXTMASK_HASH_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extmask_hash_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_DPM_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpm_type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_EPCOM_EPTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_eptype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP0_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_svlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_ptp_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_ptp_id = tmp_val;


		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_SECtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsaData->dbghsa_ptp_sec[0],&tmp_buf[0],6);

		if ((ret = table_field_get(RTL9607C_HSA_DATA_PTP2t, RTL9607C_HSA_DATA_PTP2_DBGHSA_PTP_NSECtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_ptp_nsec = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCI2t, RTL9607C_HSA_DATA_OMCI2_DBGHSA_OMCI_DEV_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_omci_dev_id = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCI2t, RTL9607C_HSA_DATA_OMCI2_DBGHSA_OMCI_CONT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_omci_cont_len = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMAL2t, RTL9607C_HSA_DATA_NORMAL2_DBGHSA_GRP1_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_svlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_SRCMODtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_srcmod = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_L3TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l4chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_L3CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_NEWPORTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newport = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NAT2t, RTL9607C_HSA_DATA_NAT2_DBGHSA_NAT_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newip = tmp_val;

	}
	else
	{
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_bgdsc = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_endsc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_pktlen = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_sid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_floodpkt = tmp_val;

	    if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_QIDtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }

		hsaData->dbghsa_oq_qid[0] = tmp_buf[4] & 0x7;
		hsaData->dbghsa_oq_qid[1] = (tmp_buf[4] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[2] = ((tmp_buf[3] << 2) & 0x4) | ((tmp_buf[4] >> 6) & 0x3);
		hsaData->dbghsa_oq_qid[3] = (tmp_buf[3] >> 1) & 0x7;
		hsaData->dbghsa_oq_qid[4] = (tmp_buf[3] >> 4) & 0x7;
		hsaData->dbghsa_oq_qid[5] = ((tmp_buf[2] << 1) & 0x6) | ((tmp_buf[3] >> 7) & 0x1);
		hsaData->dbghsa_oq_qid[6] = (tmp_buf[2] >> 2) & 0x7;
		hsaData->dbghsa_oq_qid[7] = (tmp_buf[2] >> 5) & 0x7;
		hsaData->dbghsa_oq_qid[8] = tmp_buf[1] & 0x7;
		hsaData->dbghsa_oq_qid[9] = (tmp_buf[1] >> 3) & 0x7;
		hsaData->dbghsa_oq_qid[10] = ((tmp_buf[0] << 2) & 0x4) | ((tmp_buf[1] >> 6) & 0x3);

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_DPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_dpm = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_fb = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_OQ_DRPPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_oq_drppkt = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_PON_C_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_c_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_PON_S_VLANINFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_pon_s_vlaninfo = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_endsc = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_TRAP_HASHtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_trap_hash = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_1PREM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_1PREM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_1prem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_tagcfi = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_C_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_c_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_cputag_pri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_PISO_LEAKYtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_piso_leaky = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_FBt, RTL9607C_HSA_DATA_FB_DBGHSA_L2HSA_USERPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_l2hsa_userpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_SINGLEIP_CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_singleip_chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_keep = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cpukeep = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_FRM_FBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_frm_fb = tmp_val; 

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_PPPOE_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_pppoe_sid = tmp_val;

		if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		osal_memcpy(&hsaData->dbghsa_epcom_dmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_NOCPUTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_nocputag = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_regen_crc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DUALHDR_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_act = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DUALHDR_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dualhdr_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DA_HOST_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DA_HOST_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_da_host_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_L4TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l4type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l3type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_L2TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_l2type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DATYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_datype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_MCTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_mctype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagdei = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_S_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_s_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tag_if = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagcfi= tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_C_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_c_tagvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_SP2C_SRCVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_sp2c_srcvid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_intpri = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DPCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DMABCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dmabc = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_ponsid = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_pri = tmp_val;


		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUTAG_L3Rtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_l3r = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_CPUTAG_ORGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_cputag_org = tmp_val;
		
		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extspa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_spa = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_FWDRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_fwdrsn = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_EXTMASK_HASH_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_extmask_hash_idx = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_DPM_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_dpm_type = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_EPCOM_EPTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_epcom_eptype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP0_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp0_svlan_info = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_ptp_act = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_ptp_id = tmp_val;


	    if ((ret = table_field_byte_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_SECtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    osal_memcpy(&hsaData->dbghsa_ptp_sec[0],&tmp_buf[0],6);

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_PTPt, RTL9607C_HSA_DATA_PTP_DBGHSA_PTP_NSECtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_ptp_nsec = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCIt, RTL9607C_HSA_DATA_OMCI_DBGHSA_OMCI_DEV_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_omci_dev_id = tmp_val;

	    if ((ret = table_field_get(RTL9607C_HSA_DATA_OMCIt, RTL9607C_HSA_DATA_OMCI_DBGHSA_OMCI_CONT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->dbghsa_omci_cont_len = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_O_INF_MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf_mib_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_O_INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_o_inf = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_SMAC_TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_smac_trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_DSCP_REM_VALtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_dscp_rem_val = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_C_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_c_rem_en = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_S_TPIDTYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_s_tpidtype = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_CVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_cvlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NORMALt, RTL9607C_HSA_DATA_NORMAL_DBGHSA_GRP1_SVLAN_INFOtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_grp1_svlan_info = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_SRCMODtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_srcmod = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_L3TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3trans = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_L4CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l4chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_L3CHKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_l3chksum = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_NEWPORTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newport = tmp_val;

		if ((ret = table_field_get(RTL9607C_HSA_DATA_NATt, RTL9607C_HSA_DATA_NAT_DBGHSA_NAT_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->dbghsa_nat_newip = tmp_val;
	}		
  
    return RT_ERR_OK;
} /* end of rtl9607c_hsaData_get */



/* Function Name:
 *      rtl9607c_hsdData_get
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
int32 rtl9607c_hsdData_get(rtk_hsa_debug_t *hsdData)
{
	int32 ret,index;
	uint32 tmp_val;
    uint8 tmp_buf[9];
    rtk_mac_t tmp_mac;
	uint8 sizeTb;
	uint32 *hsd_entry;
	uint32 *hsd_entry2;
	uint32 *tmp_val_ptr;

    RT_PARAM_CHK((hsdData==NULL), RT_ERR_NULL_POINTER);
	
	sizeTb = rtk_rtl9607c_table_list[INT_RTL9607C_HSA_DEBUG_DATA_RTL9607C].datareg_num;

	hsd_entry = osal_alloc(sizeTb*4);
	hsd_entry2 = osal_alloc(sizeTb*4);

    /*read data from register*/
    tmp_val_ptr = hsd_entry2;
	
	for(index=0; index < sizeTb; index++)
    {
        if ((ret = reg_array_read(RTL9607C_HSD_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            osal_free(hsd_entry);
            osal_free(hsd_entry2);
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");            
            return ret;
        }
        tmp_val_ptr ++;
    }

	for(index=0; index < sizeTb; index++)
    {

		*(hsd_entry + sizeTb - 1 - index) = *(hsd_entry2 + index);		
	}
    osal_free(hsd_entry2);
    

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_EGR_PORTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->egr_port = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_CPUINStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_cpuins = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PPPOEtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_pppoe = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_TCPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_tcp = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_L3TYPEtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_l3type = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_UDPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_udp = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_34PPPOEtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_34pppoe = tmp_val;

    if ((ret = table_field_byte_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_NEWSMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsdData->rng_hsd_newsmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_SMACTRANStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_smactrans = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_DMACTRANStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_dmactrans = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_L34TRANStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_l34trans = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_SINGLEIP_CKSUMtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_singleip_cksum = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_L3CKSUMtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_l3cksum = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_L4CKSUMtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_l4cksum = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_newip = tmp_val;

    if ((ret = table_field_byte_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_NEWDMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsdData->rng_hsd_newdmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_NEWPRTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_newprt = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_pppoe_act = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_SRC_MODtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_src_mod = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_DUALHDR_ACTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_dualhdr_act = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_DUALHDR_IDXtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_dualhdr_idx = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_SINStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_sins = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_STYPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_styp = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_STAGtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_stag = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_CINStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_cins = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_CTAGtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ctag = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PTPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ptp = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PTPACTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ptpact = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PTPIDtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ptpid = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PTPNSECtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ptpnsec = tmp_val;

    if ((ret = table_field_byte_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PTPSECtf, (uint8 *)&tmp_buf[0], (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsdData->rng_hsd_ptpsec[0],&tmp_buf[0],6);

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_ORGtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_org = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_QIDtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_qid = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_CPUPRItf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_cpupri = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_L3Rtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_l3r = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_EXTMASK_HASHIDXtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_extmask_hashidx = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ponsid = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_TRPRSNtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_trprsn = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_TTLPMSKtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_ttlpmsk = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_DPCtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_dpc = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PKTLEN_DMAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_pktlen_dma = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_PKTLEN_PLAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_pktlen_pla = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_REGENCRCtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_regencrc = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_dscp_rem_en = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_DSCP_REM_PRItf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_dscp_rem_pri = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_SPAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_spa = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_STDSCtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_stdsc = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSA_DEBUG_DATAt, RTL9607C_HSA_DEBUG_DATA_RNG_HSD_TXPADtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->rng_hsd_txpad = tmp_val;

	osal_free(hsd_entry);
    
    return RT_ERR_OK;
}




/* Function Name:
 *      rtl9607c_hsdPar_get
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
int32 rtl9607c_hsbPar_get(rtk_hsb_t *hsbData)
{
	int32 ret,index;
	uint32 tmp_val;
	uint8 sizeTb;
	uint32 *bug_entry;
	uint32 *bug_entry2;
	uint32 *tmp_val_ptr;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);
	
	sizeTb = rtk_rtl9607c_table_list[INT_RTL9607C_HSB_PAR_TABLE_RTL9607C].datareg_num;

	bug_entry = osal_alloc(sizeTb*4);
	bug_entry2 = osal_alloc(sizeTb*4);

    /*read data from register*/
    tmp_val_ptr = bug_entry2;
	
	for(index=0; index < sizeTb; index++)
    {
        if ((ret = reg_array_read(RTL9607C_HSB_PARSERr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            osal_free(bug_entry);
            osal_free(bug_entry2);
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

	for(index=0; index < sizeTb; index++)
    {

		*(bug_entry + sizeTb - 1 - index) = *(bug_entry2 + index);		
	}
    osal_free(bug_entry2);

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_SEL_DMACtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_sel_dmac = tmp_val;


    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_RX_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_rx_pktlen = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_MPCP_OMCItf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_mpcp_omci = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_PONIDXtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_ponidx = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_GATEWAYMACtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_gatewaymac = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_MYRLDPtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_myrldp = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_PRI_CTAGtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_pri_ctag = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_IPV4_RESV_MCtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_ipv4_resv_mc = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_IPV6_RESV_MCtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_ipv6_resv_mc = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_EXT_PMSKtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_ext_pmsk = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_PHY_PMSKtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_phy_pmsk = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_CISCO_RMA_TYPEtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_cisco_rma_type = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_RMA_TYPEtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_rma_type = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_ICMP_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_icmp_if = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_IGMP_MLD_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_igmp_mld_if = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_UDP_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_udp_if = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_TCP_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_tcp_if = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_FRM_WANtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_frm_wan = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_FRM_PROVIDERtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_frm_provider = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_FRM_EXTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_frm_ext = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_FRM_DSPBOtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_frm_dspbo = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_DUALIP_ERRtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_dualip_err = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_BRDtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_brd = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_IPV6MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_ipv6mlt = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_IPV4MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_ipv4mlt = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_L2MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_l2mlt = tmp_val;

    if ((ret = table_field_get(RTL9607C_HSB_PAR_TABLEt, RTL9607C_HSB_PAR_TABLE_RNG_PARSER_UNItf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
        osal_free(bug_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rng_parser_uni = tmp_val;

	osal_free(bug_entry);

    return RT_ERR_OK;
}

