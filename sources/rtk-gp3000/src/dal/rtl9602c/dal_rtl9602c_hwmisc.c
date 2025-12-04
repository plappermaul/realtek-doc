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
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_hwmisc.h>
#include <dal/rtl9602c/dal_rtl9602c_l2.h>
#include <rtk/switch.h>
#include <ioal/mem32.h>
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/delay.h>
#define REG_DELAY 10
#endif


/* Function Name:
 *      rtl9602c_virtualMacMode_Set(
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
int32 rtl9602c_virtualMacMode_Set(rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 tmpVal;
    
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable>=RTK_ENABLE_END, RT_ERR_INPUT);


    /* enable/disable VM mac*/
    tmpVal = enable;
    if ((ret = reg_array_field_write(RTL9602C_PORT_VM_ENr,port,REG_ARRAY_INDEX_NONE,RTL9602C_PORT_VM_ENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return ret;
    }
    
	return RT_ERR_OK;    
}


/* Function Name:
 *      rtl9602c_virtualMac_Input(
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
int32 rtl9602c_virtualMac_Input(rtk_port_t fromPort, uint8 *pPkt, uint32 len)
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
    for(index =0; index < RTL9602C_VIR_MAC_DUMY_CYCLE ;index++)    
    {
        if ((ret = reg_array_write(RTL9602C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
    }

    /*start send packet contant*/
    /*preamble*/
    for(index =0; index < 8 ;index++)    
    {
        tmpVal = 1;
        regData = 0;
        if ((ret = reg_field_set(RTL9602C_PORT_VM_RXr, RTL9602C_PORT_VM_RXDVf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if(7 == index)
            tmpVal = 0xd5;
        else
            tmpVal = 0x55;
        if ((ret = reg_field_set(RTL9602C_PORT_VM_RXr, RTL9602C_PORT_VM_RXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_array_write(RTL9602C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif     
    }
    
    /*data*/
    pPktPtr = pPkt;
    for(index =0; index < len ;index++)    
    {
        tmpVal = 1;
        if ((ret = reg_field_set(RTL9602C_PORT_VM_RXr, RTL9602C_PORT_VM_RXDVf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        tmpVal = *pPktPtr;
        if ((ret = reg_field_set(RTL9602C_PORT_VM_RXr,RTL9602C_PORT_VM_RXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_array_write(RTL9602C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        pPktPtr++;    
    }

   
    /*send dumy*/
    /* general dumy data*/
    regData = 0;

    for(index =0; index < RTL9602C_VIR_MAC_DUMY_CYCLE ;index++)    
    {
        if ((ret = reg_array_write(RTL9602C_PORT_VM_RXr,fromPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
    }
    

	return RT_ERR_OK;
}



/* Function Name:
 *      rtl9602c_virtualMac_Output(
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
int32 rtl9602c_virtualMac_Output(rtk_port_t toPort, uint8 *pPkt, uint32 *plen)
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
    for(index =0; index < RTL9602C_VIR_MAC_TX_MAX_CNT ;index++)    
    {
        if ((ret = reg_array_read(RTL9602C_PORT_VM_TXr,toPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_field_get(RTL9602C_PORT_VM_TXr, RTL9602C_PORT_VM_TXENf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif		
        if(0 == tmpVal)
            continue;

        if ((ret = reg_field_get(RTL9602C_PORT_VM_TXr, RTL9602C_PORT_VM_TXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        } 
#if defined(CONFIG_SDK_KERNEL_LINUX)
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
        if ((ret = reg_array_read(RTL9602C_PORT_VM_TXr,toPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_field_get(RTL9602C_PORT_VM_TXr, RTL9602C_PORT_VM_TXENf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if(0 == tmpVal)
            break;

        if ((ret = reg_field_get(RTL9602C_PORT_VM_TXr, RTL9602C_PORT_VM_TXDf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
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
     for(index =0; index < RTL9602C_VIR_MAC_DUMY_CYCLE ;index++)    
    {
        if ((ret = reg_array_read(RTL9602C_PORT_VM_TXr,toPort,REG_ARRAY_INDEX_NONE,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        if ((ret = reg_field_get(RTL9602C_PORT_VM_TXr, RTL9602C_PORT_VM_TXENf,&tmpVal,&regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
#if defined(CONFIG_SDK_KERNEL_LINUX)
		udelay(REG_DELAY);
#endif
        /*get next packt stop dummy read*/
        if(1 == tmpVal)
            break;
    }

 
	return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_hsbData_get
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
int32 rtl9602c_hsbData_get(rtk_hsb_t *hsbData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtk_mac_t tmp_da;
    rtk_mac_t tmp_sa;
	rtk_ipv6_addr_t tmp_ip6;
    rtl9602c_hsb_entry_t hsb_entry2;
    rtl9602c_hsb_entry_t hsb_entry;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsb_entry2;
    for(index=0 ; index< (sizeof(rtl9602c_hsb_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9602C_HSB_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9602c_hsb_entry_t)/4) ; index++)
    {
        hsb_entry.entry_data[sizeof(rtl9602c_hsb_entry_t)/4 - 1 - index] = hsb_entry2.entry_data[index];
    }

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->spa = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_15tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_15 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_15tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_15 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_15tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_15 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_14tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_14 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_13tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_13 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_12tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_12 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_11tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_11 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_10tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_10 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_9tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_9 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_8tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_8 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_7tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_7 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_6tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_6 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_5 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_4 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_3 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_2 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_1 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_field_0 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->user_valid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_LEN_OF_NHStf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->len_of_nhs = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_L3PROTOCOL_NHtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->l3proto_nh = tmp_val;

    if ((ret = table_field_byte_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_DIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsbData->dip6.ipv6_addr[0],&tmp_ip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));

    if ((ret = table_field_byte_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsbData->sip6.ipv6_addr[0],&tmp_ip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));


    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_IP6_HN_RGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ip6_nh_rg = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CKS_OK_L4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cks_ok_l4 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CKS_OK_L3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cks_ok_l3 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TTL_GT1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ttl_gt1 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TTL_GT5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ttl_gt5 = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_L4_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->l4_type = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PPPOE_SESSIONtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pppoe_session = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_DIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->dip = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->sip = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->tc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TOStf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->tos_dscp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_IP6_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ip6_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_IP4_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ip4_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PTP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ptp_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_OAMPDUtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->oampdu = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_RLPP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rlpp_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_RLDP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->rldp_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_LLC_OTHERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->llc_other = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pppoe_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SNAP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->snap_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_ETHER_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ether_type = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ctag = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->ctag_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_STAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->stag = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_STAG_TPIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->stag_tpid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->stag_if = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_pon_sid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PPPOE_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_pppoe_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_pppoe_act = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_EXTSPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_extspa  = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_L34KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_l34keep = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_SBtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_sb = tmp_val;
    
    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PSELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_psel = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_DISLRNtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_dislrn = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_keep = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_pri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PRISELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_prisel = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_TXPMSKtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_txpmsk = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->cputag_if = tmp_val;

    if ((ret = table_field_byte_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SAtf, (uint8 *)&tmp_sa.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsbData->sa.octet[0],&tmp_sa.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_byte_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_DAtf, (uint8 *)&tmp_da.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsbData->da.octet[0],&tmp_da.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PON_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pon_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PKT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->pkt_len = tmp_val;

    return RT_ERR_OK;
} /* end of rtl9602c_hsbData_get */


/* Function Name:
 *      rtl9602c_hsaData_get
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
int32 rtl9602c_hsaData_get(rtk_hsa_t *hsaData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    uint8 tmp_buf[9];
    rtl9602c_hsa_entry_t hsa_entry;
    rtl9602c_hsa_entry_t hsa_entry2;
    rtk_mac_t tmp_mac;

    RT_PARAM_CHK((hsaData==NULL), RT_ERR_NULL_POINTER);

    /*read data from register*/
    tmp_val_ptr = (uint32 *) &hsa_entry2;
    for(index=0 ; index<(sizeof(rtl9602c_hsa_entry_t)/4) ; index++)
    {
        if ((ret = reg_array_read(RTL9602C_HSA_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }

    for(index=0 ; index< (sizeof(rtl9602c_hsa_entry_t)/4) ; index++)
    {
        hsa_entry.entry_data[sizeof(rtl9602c_hsa_entry_t)/4 - 1 - index] = hsa_entry2.entry_data[index];

    }

//DATA_TABLE

	if ((ret = reg_field_read(RTL9602C_MODEL_NAME_INFOr, RTL9602C_MODEL_CHAR_1STf,&tmp_val)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
		return ret;
	}	

	if(2 == tmp_val)
	{
	    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLE_OLDt, RTL9602C_HSA_DATA_TABLE_OLD_RNG_HSAB_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->rng_nhsab_endsc = tmp_val;

	    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLE_OLDt, RTL9602C_HSA_DATA_TABLE_OLD_RNG_HSAB_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->rng_nhsab_bgdsc = tmp_val;

		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLE_OLDt, RTL9602C_HSA_DATA_TABLE_OLD_RNG_HSAB_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->rng_nhsab_floodpkt = tmp_val;

	    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLE_OLDt, RTL9602C_HSA_DATA_TABLE_OLD_RNG_HSAB_QIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	        return RT_ERR_FAILED;
	    }
	    hsaData->rng_nhsab_qid = tmp_val;
	}
	else
	{
		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_ENDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_endsc = tmp_val;
	
		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_BGDSCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_bgdsc = tmp_val;
	
		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_FLOODPKTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_floodpkt = tmp_val;
	
		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_QIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_qid = tmp_val;

		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_HOSTDAMIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_hostmibEn = tmp_val;

		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_HOSTDAINFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_hostinf = tmp_val;

		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_34MIB_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_l34mibEn= tmp_val;
		
		if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_34INFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return RT_ERR_FAILED;
		}
		hsaData->rng_nhsab_l34inf = tmp_val;

	}
		
	

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_ORGtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_org = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_L3Rtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_l3r = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_EXTMASKtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_ext_mask = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_CPUPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_cpupri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_ISSBtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_issb = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_IPMCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_ipmc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_FWDRSNtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_fwdrsn = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_pon_sid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_DPMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_dpm = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_DMA_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_dma_spa = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_spa = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAB_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsab_pktlen = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_IG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ctag_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_UNTAGSETtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_untagset = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_PONACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ctag_ponact = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_ctag_act = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_PRI_REM_PON_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_1p_rem_pon_en = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_PRI_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_1p_rem_en = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_PONPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	{
	   RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	   return RT_ERR_FAILED;
	}
	hsaData->rng_nhsac_ponpri = tmp_val;
	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_PONVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	{
	    RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
	    return RT_ERR_FAILED;
	}
	hsaData->rng_nhsac_ponvid = tmp_val;
	
  	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_pri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_VIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_vid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_tagpri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_TAGCFItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_tagcfi = tmp_val;
	
  	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_tagpri = tmp_val;

  	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAC_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsac_tagvid = tmp_val;
    
    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_IG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_stag_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_UNTAGSETtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_untagset = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_PONACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_ponact = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_PONPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_ponspri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_PONVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_ponsvid = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_spri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_VIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_svid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_SP2CVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_sp2cvid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_SP2CACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_sp2cact = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_TPID_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_tpid_type = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_TAGPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_tagpri = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_TAGDEItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_tagdei = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAS_TAGVIDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsas_tagvid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAM_INTPRItf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_intpri = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAM_DSCP_REM_PONtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem_pon = tmp_val;

	if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAM_DSCP_REM_PON_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem_en_pon = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAM_DSCP_REMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAM_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsam_dscp_rem_en = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_REGEN_CRCtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_regen_crc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_CPUKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_cpukeep = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_OTHERKEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_keep = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_PTPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_ptp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_TCPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_tcp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_UDPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_udp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_IPV4tf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_ipv4 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_IPV6tf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_ipv6 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_RFC1042tf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_rfc1042 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAF_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsaf_pppoe_if = tmp_val;


	if ((ret = table_field_get(RTL9602C_HSA_DATA_PTPt, RTL9602C_HSA_DATA_PTP_RNG_HSAP_PTP_IDtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
		return RT_ERR_FAILED;
	}
	hsaData->rng_nhsap_ptp_id = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_PTPt, RTL9602C_HSA_DATA_PTP_RNG_HSAP_PTP_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsap_ptp_act = tmp_val;
    
    if ((ret = table_field_byte_get(RTL9602C_HSA_DATA_PTPt, RTL9602C_HSA_DATA_PTP_RNG_HSAP_PTP_SECtf, (uint8 *)&tmp_buf[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsaData->rng_nhsap_ptp_sec[0],&tmp_buf[0],6);

    if ((ret = table_field_get(RTL9602C_HSA_DATA_PTPt, RTL9602C_HSA_DATA_PTP_RNG_HSAP_PTP_NSECtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }    
    hsaData->rng_nhsap_ptp_nsec = tmp_val;

//NAT
    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_DSLITE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_dslite_act= tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_DSLITE_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_dslite_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_WANSAtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_wansa = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_L2TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_l2trans = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_L34TRANStf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_l34trans = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_SRC_MODtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_src_mode = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_PPPOE_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_pppoe_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_pppoe_act = tmp_val;
    
    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_SMAC_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_smac_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_L3CKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_l3chsum = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_L4CKSUMtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_l4chsum = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_NATMCt, RTL9602C_HSA_DATA_NATMC_RNG_HSAN_L3TRIDXtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_l3tridx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_newip = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_NEWPORTtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_newport = tmp_val;

    if ((ret = table_field_byte_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_NEWDMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsaData->rng_nhsan_newmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_get(RTL9602C_HSA_DATA_TABLEt, RTL9602C_HSA_DATA_TABLE_RNG_HSAN_TTLM1_PMASKtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_ttlm1_pmask = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSA_DATA_OMCIt, RTL9602C_HSA_DATA_OMCI_RNG_HSAN_OMCI_MSG_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_omci_msg_type  = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DATA_OMCIt, RTL9602C_HSA_DATA_OMCI_RNG_HSAN_OMCI_CONT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsa_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsaData->rng_nhsan_omci_cont_len = tmp_val;

  
    return RT_ERR_OK;
} /* end of apollo_raw_hsaData_get */


/* Function Name:
 *      rtl9602c_hsdData_get
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
int32 rtl9602c_hsdData_get(rtk_hsa_debug_t *hsdData)
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
	
	sizeTb = rtk_rtl9602c_table_list[INT_RTL9602C_HSA_DEBUG_DATA_RTL9602C].datareg_num;

	hsd_entry2 = osal_alloc(sizeTb*4);

    /*read data from register*/
    tmp_val_ptr = hsd_entry2;
	
	for(index=0; index < sizeTb; index++)
    {
        if ((ret = reg_array_read(RTL9602C_HSD_DATAr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
        {
			osal_free(hsd_entry2);	
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
        tmp_val_ptr ++;
    }
    
	hsd_entry = osal_alloc(sizeTb*4);

	for(index=0; index < sizeTb; index++)
    {
		*(hsd_entry + sizeTb - 1 - index) = *(hsd_entry2 + index);		
	}
    
    osal_free(hsd_entry2);  

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_TXPADtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_txpad = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_STDSCtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_stdsc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_DMA_SPAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dmaspa = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_SPAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_spa = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_DSCP_REM_PRItf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dscp_rem_pri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_DSCP_REM_ENtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dscp_rem_en = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_REGENCRCtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_regencrc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PKTLEN_PLAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pktlen_pla = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PKTLEN_DMAtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pktlen_dma = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_DPCtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dpc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_ISSBtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_issb = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_TTLPMSKtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ttlpmsk = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_TTLEXMSKtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ttlexmsk = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_TRPRSNtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_trprsn = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_EXTMSKtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_extmsk = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PONSIDtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ponsid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_L3Rtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_l3r = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_CPUPRItf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_cpupri = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_QIDtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_qid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_ORGtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_org = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PTPNSECtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
        osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ptpnsec = tmp_val;

    if ((ret = table_field_byte_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PTPSECtf, (uint8 *)&tmp_buf[0], (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsdData->hsa_debug_ptpsec[0],&tmp_buf[0],6);

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PTPIDtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ptpid = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PTPACTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ptpact = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_PTPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ptp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_CTAGtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ctag = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_CINStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_cins = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_STAGtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_stag = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_STYPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_styp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD0_SINStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_sins = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_EGR_PORTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_egr_port = tmp_val;


    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_DSLITE_IDXtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dslite_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_DSLITE_ACTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_dslite_act = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_SRC_MODtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_src_mod = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pppoe_act = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_NEWPRTtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_newprt = tmp_val;

    if ((ret = table_field_byte_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_NEWDMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsdData->hsa_debug_newdmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));


    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_NEWIPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_newip = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_L4CKSUMtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_l4cksum = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_L3CKSUMtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_l3cksum = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_L34TRANStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_l34trans = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_L2TRANStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_l2trans = tmp_val;

    if ((ret = table_field_byte_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_NEWSMACtf, (uint8 *)&tmp_mac.octet[0], (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    osal_memcpy(&hsdData->hsa_debug_newsmac.octet[0],&tmp_mac.octet[0],sizeof(rtk_mac_t));

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_34PPPOEtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_l34pppoe = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_UDPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_udp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_IPV6tf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ipv6= tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_IPV4tf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_ipv4= tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_TCPtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_tcp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_RFC1042tf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_rfc1042 = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_PPPOEtf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_pppoe = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSA_DEBUG_DATAt, RTL9602C_HSA_DEBUG_DATA_RNG_HSD1_CPUINStf, (uint32 *)&tmp_val, (uint32 *) hsd_entry)) != RT_ERR_OK)
    {
    	osal_free(hsd_entry);
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsdData->hsa_debug_cpuins = tmp_val;

	osal_free(hsd_entry);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9602c_hsbData_set
 * Description:
 *      Get HSB data
 * Input:
 *      hsbData
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
int32 rtl9602c_hsbData_set(rtk_hsb_t *hsbData)
{
	int32 ret,index;
    uint32 tmp_val,*tmp_val_ptr;
    rtk_mac_t tmp_da;
    rtk_mac_t tmp_sa;
	rtk_ipv6_addr_t tmp_ip6;
    rtl9602c_hsb_entry_t hsb_entry2;
    rtl9602c_hsb_entry_t hsb_entry;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);
	osal_memset(&hsb_entry, 0, sizeof(rtl9602c_hsb_entry_t));
	osal_memset(&hsb_entry2, 0, sizeof(rtl9602c_hsb_entry_t));

	tmp_val = hsbData->spa;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SPAtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	tmp_val = hsbData->user_field_15;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_15tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	tmp_val = hsbData->user_field_14;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_14tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	tmp_val = hsbData->user_field_13;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_13tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_12;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_12tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_11;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_11tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_10;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_10tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_0;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_9tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_8;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_8tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_7;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_7tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_6;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_6tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_5;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_4;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_3;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_2;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_2tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_1;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_field_0;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_FIELD_0tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->user_valid;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_USER_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->len_of_nhs;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_LEN_OF_NHStf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->l3proto_nh;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_L3PROTOCOL_NHtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&tmp_ip6.ipv6_addr[0],&hsbData->dip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));
    if ((ret = table_field_byte_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_DIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&tmp_ip6.ipv6_addr[0],&hsbData->sip6.ipv6_addr[0],sizeof(rtk_ipv6_addr_t));
    if ((ret = table_field_byte_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SIP6tf, (uint8 *)&tmp_ip6.ipv6_addr[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ip6_nh_rg;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_IP6_HN_RGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cks_ok_l4;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CKS_OK_L4tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cks_ok_l3;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CKS_OK_L3tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ttl_gt1;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TTL_GT1tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ttl_gt5;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TTL_GT5tf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->l4_type;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_L4_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pppoe_session;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PPPOE_SESSIONtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->dip;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_DIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->sip;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SIPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->tos_dscp;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_TCtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ip6_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_IP6_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ip4_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_IP4_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ptp_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PTP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->oampdu;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_OAMPDUtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->rlpp_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_RLPP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->rldp_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_RLDP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->llc_other;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_LLC_OTHERtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pppoe_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PPPOE_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->snap_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SNAP_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ether_type;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_ETHER_TYPEtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ctag;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CTAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->ctag_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->stag;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_STAGtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->stag_tpid;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_STAG_TPIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->stag_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_STAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_pon_sid;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PON_SIDtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*tmp_val = hsbData->cputag_l2br;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_L2BRtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }*/

    tmp_val = hsbData->cputag_pppoe_idx;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PPPOE_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_pppoe_act;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PPPOE_ACTtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_l34keep;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_L34KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_sb;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_SBtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    tmp_val = hsbData->cputag_psel;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PSELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_dislrn;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_DISLRNtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_keep;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_KEEPtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_pri;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PRItf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_prisel;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_PRISELtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_txpmsk;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_TXPMSKtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    /*tmp_val = hsbData->cputag_l4c;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_L4Ctf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->cputag_l3c;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_L3Ctf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }*/

    tmp_val = hsbData->cputag_if;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_CPUTAG_IFtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&tmp_sa.octet[0],&hsbData->sa.octet[0],sizeof(rtk_mac_t));
    if ((ret = table_field_byte_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_SAtf, (uint8 *)&tmp_sa.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    osal_memcpy(&tmp_da.octet[0],&hsbData->da.octet[0],sizeof(rtk_mac_t));
    if ((ret = table_field_byte_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_DAtf, (uint8 *)&tmp_da.octet[0], (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pon_idx;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PON_IDXtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    tmp_val = hsbData->pkt_len;
    if ((ret = table_field_set(RTL9602C_HSB_DATA_TABLEt, RTL9602C_HSB_DATA_TABLE_PKT_LENtf, (uint32 *)&tmp_val, (uint32 *) &hsb_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

	for(index=0 ; index< (sizeof(rtl9602c_hsb_entry_t)/4) ; index++)
	{
		hsb_entry2.entry_data[sizeof(rtl9602c_hsb_entry_t)/4 - 1 - index] = hsb_entry.entry_data[index];
	}

	/*write data from register*/
	tmp_val_ptr = (uint32 *) &hsb_entry2;
	for(index=0 ; index< (sizeof(rtl9602c_hsb_entry_t)/4) ; index++)
	{
		if ((ret = reg_array_write(RTL9602C_HSB_DATA_DBGr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
			return ret;
		}

		tmp_val_ptr ++;
	}
	
    return RT_ERR_OK;
} /* end of rtl9602c_hsbData_set */



/* Function Name:
 *      rtl9602c_hsdPar_get
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
int32 rtl9602c_hsbPar_get(rtk_hsb_t *hsbData)
{
	int32 ret,index;
	uint32 tmp_val;
	uint8 sizeTb;
	uint32 *bug_entry;
	uint32 *bug_entry2;
	uint32 *tmp_val_ptr;

    RT_PARAM_CHK((hsbData==NULL), RT_ERR_NULL_POINTER);
	
	sizeTb = rtk_rtl9602c_table_list[INT_RTL9602C_HSB_PAR_TABLE_RTL9602C].datareg_num;

	bug_entry = osal_alloc(sizeTb*4);
	bug_entry2 = osal_alloc(sizeTb*4);

    /*read data from register*/
    tmp_val_ptr = bug_entry2;
	
	for(index=0; index < sizeTb; index++)
    {
        if ((ret = reg_array_read(RTL9602C_HSB_PARSERr,REG_ARRAY_INDEX_NONE,index,tmp_val_ptr)) != RT_ERR_OK)
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

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_UNItf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_uni = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_mlt = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_BRDtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_brd = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_L2MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_l2mlt = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_IPV4MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_ipv4mlt = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_IPV6MLTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_ipv6mlt = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_FRM_PROVIDERtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_frm_provider = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_FRM_WANtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_frm_wan = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_TCP_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_tcp_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_UDP_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_udp_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_IGMP_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_igmp_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_MLD_IFtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_mld_if = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_L34PKTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_l34pkt = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_RMA_TYPEtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_rma_type = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_FLD00_VLDtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_fld00_vld = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_SW_PKTtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_sw_pkt = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_UTP_PMSKtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_utp_pmsk = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_EXT_PMSKtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_ext_pmsk = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_CPU_PSELtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_cpu_sel = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_IPV6_RESV_MCtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_ipv6_resv_mc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_IPV4_RESV_MCtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_ipv4_resv_mc = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_PRI_CTAGtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_pri_ctag = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_DSLITE_MATCH_IDXtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_dslite_match_idx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_MYRLDPtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_myrldp = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_PONIDXtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_ponidx = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_MPCP_OMCItf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_mpcp_omci = tmp_val;

    if ((ret = table_field_get(RTL9602C_HSB_PAR_TABLEt, RTL9602C_HSB_PAR_TABLE_RNG_DBGPARSOR_RX_PKTLENtf, (uint32 *)&tmp_val, (uint32 *) bug_entry)) != RT_ERR_OK)
    {
		osal_free(bug_entry);
		osal_free(bug_entry2);	
        RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    hsbData->par_rx_pktlen = tmp_val;

	osal_free(bug_entry);
	osal_free(bug_entry2);	
	
    return RT_ERR_OK;
}

#define REG_CONFIG_NUM 47

typedef struct reg_config_s {
    char MB[32];
    char regName[32];
    uint32 regAddr;
    uint32 testValue;
}reg_config_t;

reg_config_t reg_config[REG_CONFIG_NUM]={
    {"GLB_CTRL"            ,"GPHY_IND_WD"           ,RTL9602C_GPHY_IND_WDr           ,0x0000abcd},
    {"CHIP_INFO"           ,"BOND_INFO"             ,RTL9602C_BOND_INFOr             ,0xa0000000},
    {"ALE_GLB"             ,"MAX_LENGTH_CFG1"       ,RTL9602C_MAX_LENGTH_CFG1r       ,0x00003abc},
//  {"ALE_TABLE"           ,""                      ,                                ,0x00000000},
    {"ALE_CVLAN"           ,"VLAN_CTRL"             ,RTL9602C_VLAN_CTRLr             ,0x0000001c},
    {"ALE_SVLAN"           ,"SVLAN_CTRL"            ,RTL9602C_SVLAN_CTRLr            ,0x0008000d},
    {"ALE_ACL"             ,"ACL_TEMPLATE_CTRL"     ,RTL9602C_ACL_TEMPLATE_CTRLr     ,0x0bcdabcd},
    {"ALE_CF"              ,"CF_CFG"                ,RTL9602C_CF_CFGr                ,0x000abc6d},
    {"ALE_L2"              ,"LUT_CFG"               ,RTL9602C_LUT_CFGr               ,0x20cdabcd},
    {"ALE_MLTVLAN"         ,"RSVD_ALE_MLTVLAN"      ,RTL9602C_RSVD_ALE_MLTVLANr      ,0xabcdabcd},
    {"ALE_RLDP"            ,"RLDP_MAGIC_NUM"        ,RTL9602C_RLDP_MAGIC_NUMr        ,0x0000abcd},
    {"ALE_EAV_AFBK"        ,"PTP_TIME_OFFSET_SEC"   ,RTL9602C_PTP_TIME_OFFSET_SECr   ,0x0000abcd},
    {"ALE_DPM"             ,"UNKN_MC_CFG"           ,RTL9602C_UNKN_MC_CFGr           ,0x00003acd},
    {"INTR"                ,"INTR_DBGO_POS"         ,RTL9602C_INTR_DBGO_POSr         ,0xabcdabcd},
    {"LED"                 ,"LED_EN"                ,RTL9602C_LED_ENr                ,0x000dabcc},
    {"PHY_IP"              ,"PHY_RG0X_CEN"          ,RTL9602C_PHY_RG0X_CENr          ,0x0000abcd},
    {"PER_PORT_MAC"        ,"P_MISC"                ,RTL9602C_P_MISCr                ,0x03cdabcd},
//  {"RESERVED"            ,""                      ,                                ,0x00000000},
    {"HSG_SDSREG"          ,"WSDS_HSG_ANA_00"       ,RTL9602C_WSDS_HSG_ANA_00r       ,0xbad0bad0},//need double check
    {"HSG_SDS_IP"          ,"HSG_SDS_REG0"          ,RTL9602C_HSG_SDS_REG0r          ,0xbad0bad0},//need double check
    {"SDSREG"              ,"WSDS_ANA_01"           ,RTL9602C_WSDS_ANA_01r           ,0xabcdabcd},
//  {"SDS_ANA_MISC"        ,""                      ,                                ,0x00000000},
//  {"SDS_ANA_COM"         ,""                      ,                                ,0x00000000},
//  {"SDS_ANA_SPD"         ,""                      ,                                ,0x00000000},
//  {"SDS_ANA_SPD_1P25G"   ,""                      ,                                ,0x00000000},
//  {"SDS_ANA_SPD_GPON"    ,""                      ,                                ,0x00000000},
//  {"SDS_ANA_SPD_EPON"    ,""                      ,                                ,0x00000000},
    {"SDS_IP"              ,"SDS_REG0"              ,RTL9602C_SDS_REG0r              ,0x08001403},
    {"SWCORE"              ,"I2C_CONFIG"            ,RTL9602C_I2C_CONFIGr            ,0x0004abcd},
//  {"RESERVED"            ,""                      ,                                ,0x00000000},
    {"ALE_METER"           ,"METER_TB_CTRL"         ,RTL9602C_METER_TB_CTRLr         ,0x0001abcd},
    {"ALE_RMA_ATTACK"      ,"DOS_CFG"               ,RTL9602C_DOS_CFGr               ,0x00000000},
    {"ALE_PISO"            ,"PISO_PORT"             ,RTL9602C_PISO_PORTr             ,0x00000000},
    {"ALE_HSB"             ,"HSB_CTRL"              ,RTL9602C_HSB_CTRLr              ,0x002dabcc},
//  {"ALE_HSM"             ,""                      ,                                ,0x00000000},
    {"ALE_HSA"             ,"SVLAN_SP2C"            ,RTL9602C_SVLAN_SP2Cr            ,0x2bcdabcd},
//  {"ALE_DEBUG"           ,""                      ,                                ,0x00000000},
//  {"ALE_PKTGEN"          ,""                      ,                                ,0x00000000},
    {"EGR_OUTQ"            ,"LOW_QUEUE_TH"          ,RTL9602C_LOW_QUEUE_THr          ,0x000001cd},
    {"EGR_SCH"             ,"WFQ_CTRL"              ,RTL9602C_WFQ_CTRLr              ,0x0001abcd},
//  {"PKT_ENCAP"           ,""                      ,                                ,0x00000000},
//  {"PKT_PARSER"          ,""                      ,                                ,0x00000000},
//  {"CFM_GEN"             ,""                      ,                                ,0x00000000},
    {"BIST_CTRL"           ,"SW_BIST_CFG_1"         ,RTL9602C_SW_BIST_CFG_1r         ,0x000013cd},
//  {"MIB_DATA"            ,""                      ,                                ,0x00000000},
    {"MIB_CTRL"            ,"STAT_CTRL"             ,RTL9602C_STAT_CTRLr             ,0x00000403},
//  {"RESERVED"            ,""                      ,                                ,0x00000000},
    {"EPON_CTRL"           ,"EPON_FEC_CONFIG"       ,RTL9602C_EPON_FEC_CONFIGr       ,0x000dabc9},
//  {"RESERVED"            ,""                      ,                                ,0x00000000},
//  {"RESERVED"            ,""                      ,                                ,0x00000000},
    {"GPON_PAGE0"          ,"GPON_TEST"             ,RTL9602C_GPON_TESTr             ,0xabcdabcd},
    {"GPON_PAGE1"          ,"GPON_GTC_DS_INTR_MASK" ,RTL9602C_GPON_GTC_DS_INTR_MASKr ,0x00000b0d},
    {"GPON_PAGE2"          ,"GPON_BWMAP_CTRL"       ,RTL9602C_GPON_BWMAP_CTRLr       ,0x000080cd},
    {"GPON_PAGE3"          ,"GPON_AES_INTR_MASK"    ,RTL9602C_GPON_AES_INTR_MASKr    ,0x00000003},
    {"GPON_PAGE4"          ,"GPON_GEM_DS_MC_CFG"    ,RTL9602C_GPON_GEM_DS_MC_CFGr    ,0x0000024d},
    {"GPON_PAGE5"          ,"GPON_GTC_US_CFG"       ,RTL9602C_GPON_GTC_US_CFGr       ,0x00000c18},
    {"GPON_PAGE6"          ,"GPON_GEM_US_INTR_MASK" ,RTL9602C_GPON_GEM_US_INTR_MASKr ,0x000003cd},
    {"NAT_CTRL"            ,"SWTCR0"                ,RTL9602C_SWTCR0r                ,0xa0802bc0},
//  {"NAT_INDIR"           ,""                      ,                                ,0x00000000},
    {"NAT_HSBA"            ,"HSBA_CTRL"             ,RTL9602C_HSBA_CTRLr             ,0x0000000c},
//  {"RESERVED"            ,""                      ,                                ,0x00000000},
//  {"NAT_L4_TRF"          ,""                      ,                                ,0x00000000},
    {"PONIP_GLB"           ,"PON_LX_MON_SAMPLE"     ,RTL9602C_PON_LX_MON_SAMPLEr     ,0x0001abcd},
    {"PONIP_US"            ,"US_BIST_CFG_1"         ,RTL9602C_US_BIST_CFG_1r         ,0x000053cd},
    {"PONIP_NICUS"         ,"TX_CFG_US"             ,RTL9602C_TX_CFG_USr             ,0x00000805},
    {"PONIP_NICIFUS"       ,"PROBE_SELECT_US"       ,RTL9602C_PROBE_SELECT_USr       ,0x000000cd},
    {"PONIP_DS"            ,"DS_BIST_CFG_1"         ,RTL9602C_DS_BIST_CFG_1r         ,0x000053cd},
    {"PONIP_NICDS"         ,"TX_CFG_DS"             ,RTL9602C_TX_CFG_DSr             ,0x00000805},
    {"PONIP_NICIFDS"       ,"PROBE_SELECT_DS"       ,RTL9602C_PROBE_SELECT_DSr       ,0x000000cd},
}; 
 

//lut_table
#define LUT_TBL_CONFIG_NUM 4

typedef struct lut_tbl_config_s {
    char tblName[32];
    uint32 regAddr;
    uint32 testEntry;
    uint32 realEntry;
}lut_tbl_config_t;

rtl9602c_lut_table_t lut_config_entry={
    .table_type=RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC,
    /*common part*/
    .l3lookup=0,
    .nosalearn=1,
    .valid=1,
    
    /*----L2----*/
    .mac.octet[0]=0x00,
    .mac.octet[1]=0x01,
    .mac.octet[2]=0x02,
    .mac.octet[3]=0x03,
    .mac.octet[4]=0x04,
    .mac.octet[5]=0x05,
    .cvid_fid=1,
    .ivl_svl=RTL9602C_RAW_L2_HASH_IVL,
    
    /*l2 uc*/
    .fid=1,
    .ctag_if=1,
    .spa=0xf,
    .age=30,
    .auth=0,
    .sa_block=1,
    .da_block=0,
    .arp_used=0,
    .ext_spa=0xf,
    
    /*----L3----*/
    .ip6=0,
    .wan_sa=1,
    
    /*----L3 MC Route----*/
    .gip=1,
    .l3_idx=1,
    .sip_idx=1,
    .sip_filter=1,
    
    /*----L3 IPv6----*/
    .dip6.ipv6_addr[0]=0x00,
    .dip6.ipv6_addr[1]=0x00,
    .dip6.ipv6_addr[2]=0x00,
    .dip6.ipv6_addr[3]=0x00,
    .dip6.ipv6_addr[4]=0x00,
    .dip6.ipv6_addr[5]=0x00,
    .dip6.ipv6_addr[6]=0x00,
    .dip6.ipv6_addr[7]=0x00,
    .dip6.ipv6_addr[8]=0x00,
    .dip6.ipv6_addr[9]=0x00,
    .dip6.ipv6_addr[10]=0x00,
    .dip6.ipv6_addr[11]=0x00,
    .dip6.ipv6_addr[12]=0x00,
    .dip6.ipv6_addr[13]=0x01,
    .dip6.ipv6_addr[14]=0x02,
    .dip6.ipv6_addr[15]=0x03,
    .l3mcr_idx=1,
    
    /*(L2 MC)(L3 MC Route)(L3 IPV6 MC)*/
    .mbr=0xf,
    .ext_mbr=0xf,
};

lut_tbl_config_t lut_tbl_config[LUT_TBL_CONFIG_NUM]={
    {"L2_UC"         ,RTL9602C_L2_UCt         ,1   ,0},
    {"L2_MC_DSL"     ,RTL9602C_L2_MC_DSLt     ,2   ,0},
    {"L3_MC_ROUTE"   ,RTL9602C_L3_MC_ROUTEt   ,3   ,0},
    {"L3_IP6MC"      ,RTL9602C_L3_IP6MCt      ,4   ,0},  
};

//l2_table
typedef struct l2_tbl_config_s {
    char tblName[32];
    uint32 regAddr;
    uint32 testEntry;
    uint32 testValue[5];
}l2_tbl_config_t;

#define L2_TBL_CONFIG_NUM 12

l2_tbl_config_t l2_tbl_config[L2_TBL_CONFIG_NUM]={   
    {"VLAN"               ,RTL9602C_VLANt               ,10   ,{0x0001abcd,0x00000000,0x00000000,0x00000000,0x00000000}},
    {"ACL_DATA"           ,RTL9602C_ACL_DATAt           ,10   ,{0x0001abcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd}},
    {"ACL_MASK"           ,RTL9602C_ACL_MASKt           ,10   ,{0x0001abcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd}},
    {"ACL_ACTION_TABLE"   ,RTL9602C_ACL_ACTION_TABLEt   ,10   ,{0x00000005,0xabcdabcd,0xabcdabcd,0x00000000,0x00000000}},
    {"CF_RULE_48_P1_T0"   ,RTL9602C_CF_RULE_48_P1_T0t   ,10   ,{0x0001abcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}}, 
    {"CF_MASK_48_P1_T0"   ,RTL9602C_CF_MASK_48_P1_T0t   ,10   ,{0x0001abcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}}, 
    {"CF_RULE_48_P1_T1"   ,RTL9602C_CF_RULE_48_P1_T1t   ,10   ,{0x0001abcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}}, 
    {"CF_MASK_48_P1_T1"   ,RTL9602C_CF_MASK_48_P1_T1t   ,10   ,{0x0001abcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}}, 
    {"CF_RULE_48_P0"      ,RTL9602C_CF_RULE_48_P0t      ,10   ,{0x0001abcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}}, 
    {"CF_MASK_48_P0"      ,RTL9602C_CF_MASK_48_P0t      ,10   ,{0x0001abcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}}, 
    {"CF_ACTION_DS"       ,RTL9602C_CF_ACTION_DSt       ,10   ,{0x00000005,0xabcdabcd,0xabcdabcd,0x00000000,0x00000000}},
    {"CF_ACTION_US"       ,RTL9602C_CF_ACTION_USt       ,10   ,{0x00000005,0xabcdabcd,0xabcdabcd,0x00000000,0x00000000}}, 
};       

//l34_table
typedef struct l34_tbl_config_s {
    char tblName[32];
    uint32 regAddr;
    uint32 testEntry;
    uint32 testValue[5];
}l34_tbl_config_t;

#define L34_TBL_CONFIG_NUM 17

l34_tbl_config_t l34_tbl_config[L34_TBL_CONFIG_NUM]={
    {"ARP_CAM_TABLE"                 ,RTL9602C_ARP_CAM_TABLEt                ,1   ,{0x00000bcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}},
    {"EXTERNAL_IP_TABLEt"            ,RTL9602C_EXTERNAL_IP_TABLEt            ,1   ,{0x000003cd,0xabcdabcd,0xabcdabcd,0x00000000,0x00000000}},
    {"L3_ROUTING_DROP_TRAP"          ,RTL9602C_L3_ROUTING_DROP_TRAPt         ,1   ,{0x00002bcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}},
    {"L3_ROUTING_GLOBAL_ROUTE"       ,RTL9602C_L3_ROUTING_GLOBAL_ROUTEt      ,1   ,{0x00002bcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}},
    {"L3_ROUTING_LOCAL_ROUTE"        ,RTL9602C_L3_ROUTING_LOCAL_ROUTEt       ,1   ,{0x00002bcd,0xabcdabcd,0x00000000,0x00000000,0x00000000}},
    {"NAPT_TABLE"                    ,RTL9602C_NAPT_TABLEt                   ,1   ,{0x0001abcd,0x00000000,0x00000000,0x00000000,0x00000000}},
    {"NAPTR_TABLE"                   ,RTL9602C_NAPTR_TABLEt                  ,1   ,{0x03cdabcd,0xabcdabcd,0xabcdabcd,0x00000000,0x00000000}},
    {"NETIF"                         ,RTL9602C_NETIFt                        ,1   ,{0x000dabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0x00000000}},
    {"NEXT_HOP_TABLE"                ,RTL9602C_NEXT_HOP_TABLEt               ,1   ,{0x000dabcd,0x00000000,0x00000000,0x00000000,0x00000000}},
    {"PPPOE_TABLE"                   ,RTL9602C_PPPOE_TABLEt                  ,1   ,{0x0000abcd,0x00000000,0x00000000,0x00000000,0x00000000}},
    {"WAN_TYPE_TABLE"                ,RTL9602C_WAN_TYPE_TABLEt               ,1   ,{0x0000000d,0x00000000,0x00000000,0x00000000,0x00000000}},
    {"NEIGHBOR_TABLE"                ,RTL9602C_NEIGHBOR_TABLEt               ,1   ,{0x0000abcd,0xabcdabcd,0xabcdabcd,0x00000000,0x00000000}},
    {"IPV6_ROUTING_TABLE"            ,RTL9602C_IPV6_ROUTING_TABLEt           ,1   ,{0x0000abcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd}},
    {"BINDING_TABLE"                 ,RTL9602C_BINDING_TABLEt                ,1   ,{0x03cdabcd,0x00000000,0x00000000,0x00000000,0x00000000}},
    {"FLOW_ROUTING_TABLE_IPV4"       ,RTL9602C_FLOW_ROUTING_TABLE_IPV4t      ,1   ,{0xabcdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0x00000000}},
    {"FLOW_ROUTING_TABLE_IPV6"       ,RTL9602C_FLOW_ROUTING_TABLE_IPV6t      ,1   ,{0x03cdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd}},
    {"FLOW_ROUTING_TABLE_IPV6_EXT"   ,RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt  ,1   ,{0x03cdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd,0xabcdabcd}},
};


/* Function Name:
 *      rtl9602c_config_set
 * Description:
 *      Set register config
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32 rtl9602c_config_set(void)
{
    int32 i,ret=RT_ERR_OK;

    //register of memory block
    for(i=0;i<REG_CONFIG_NUM;i++)
    {
        if ((ret = reg_write(reg_config[i].regAddr, &reg_config[i].testValue)) != RT_ERR_OK)
        {
            rt_log_printf("[MEM Block]: %s [Register]: %s [Addr]: 0x%x [test value]: 0x%x\n"
                ,reg_config[i].MB,reg_config[i].regName,reg_config[i].regAddr,reg_config[i].testValue);
            return ret;
        }
    }

    //lut_table
    for(i=0;i<LUT_TBL_CONFIG_NUM;i++)
    {
        switch(lut_tbl_config[i].regAddr)
        {
            case RTL9602C_L2_UCt:
                lut_config_entry.table_type=RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC;
                lut_config_entry.mac.octet[0]=0x00;
                lut_config_entry.l3lookup=0;
                lut_config_entry.ip6=0;
                break;
            case RTL9602C_L2_MC_DSLt:
                lut_config_entry.table_type=RTL9602C_RAW_LUT_ENTRY_TYPE_L2MC;
                lut_config_entry.mac.octet[0]=0x01;
                lut_config_entry.l3lookup=0;
                lut_config_entry.ip6=0;
                break;
            case RTL9602C_L3_MC_ROUTEt:
                lut_config_entry.table_type=RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
                lut_config_entry.mac.octet[0]=0x00;
                lut_config_entry.l3lookup=1;
                lut_config_entry.ip6=0;
                break;
            case RTL9602C_L3_IP6MCt:
                lut_config_entry.table_type=RTL9602C_RAW_LUT_ENTRY_TYPE_L3IP6MC;
                lut_config_entry.mac.octet[0]=0x00;
                lut_config_entry.l3lookup=1;
                lut_config_entry.ip6=1;
                break;
            default:
                rt_log_printf("wrong switch case in LUT Table Config\n");
                return ret;
        }
        if ((ret = table_write(lut_tbl_config[i].regAddr, lut_tbl_config[i].testEntry, (uint32 *)&lut_config_entry)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d\n"
                ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry );
            return ret;
        }

        lut_tbl_config[i].realEntry=lut_config_entry.address;
     
    }

    //l2_table
    for(i=0;i<L2_TBL_CONFIG_NUM;i++)
    {
        if ((ret = table_write(l2_tbl_config[i].regAddr, l2_tbl_config[i].testEntry, l2_tbl_config[i].testValue)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d [test value]: 0x%08x%08x%08x%08x%08x\n"
                ,l2_tbl_config[i].tblName,l2_tbl_config[i].regAddr,l2_tbl_config[i].testEntry
                ,l2_tbl_config[i].testValue[0],l2_tbl_config[i].testValue[1],l2_tbl_config[i].testValue[2],l2_tbl_config[i].testValue[3],l2_tbl_config[i].testValue[4] );
            return ret;
        }
    }

    //l34_table
    for(i=0;i<L34_TBL_CONFIG_NUM;i++)
    {
        if ((ret = table_write(l34_tbl_config[i].regAddr, l34_tbl_config[i].testEntry, l34_tbl_config[i].testValue)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d [test value]: 0x%08x%08x%08x%08x%08x"
                ,l34_tbl_config[i].tblName,l34_tbl_config[i].regAddr,l34_tbl_config[i].testEntry
                ,l34_tbl_config[i].testValue[0],l34_tbl_config[i].testValue[1],l34_tbl_config[i].testValue[2],l34_tbl_config[i].testValue[3],l34_tbl_config[i].testValue[4] );
            return ret;
        }
    }

    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9602c_config_get
 * Description:
 *      Get register config
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32 rtl9602c_config_get(void)
{
    int32 i,ret=RT_ERR_OK;
    uint32 data[5];
    rtl9602c_lut_table_t lut_data;
    
    //register of memory block
    for(i=0;i<REG_CONFIG_NUM;i++)
    {
        memset(data,0,5*sizeof(uint32));
        if ((ret = reg_read(reg_config[i].regAddr, data)) != RT_ERR_OK)
        {
            rt_log_printf("[MEM Block]: %s [Register]: %s [Addr]: 0x%x\n"
                ,reg_config[i].MB,reg_config[i].regName,reg_config[i].regAddr);
            return ret;
        }

        if(data[0]!=reg_config[i].testValue)
        {
            rt_log_printf("[MEM Block]: %s [Register]: %s [Addr]: 0x%x [data]: 0x%x [test value]: 0x%x\n"
                ,reg_config[i].MB,reg_config[i].regName,reg_config[i].regAddr,data[0],reg_config[i].testValue);
//            return ret;
        }

    }

    //lut_table
    for(i=0;i<LUT_TBL_CONFIG_NUM;i++)
    {
        memset(&lut_data,0,sizeof(rtl9602c_lut_table_t));

        lut_data.method = RTL9602C_RAW_LUT_READ_METHOD_ADDRESS;
#if defined(RTL_CYGWIN_EMULATE)
        if ((ret = table_read(lut_tbl_config[i].regAddr, lut_tbl_config[i].testEntry, (uint32 *)&lut_data)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d\n"
                ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry );
            return ret;
        }
            
#else
        if ((ret = table_read(lut_tbl_config[i].regAddr, lut_tbl_config[i].realEntry, (uint32 *)&lut_data)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d\n"
                ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry );
            return ret;
        }
#endif        
        switch(lut_tbl_config[i].regAddr)
        {
            case RTL9602C_L2_UCt:                
                if(lut_data.table_type!=RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC
                    ||lut_data.l3lookup!=0
                    ||lut_data.ip6!=0
                    ||memcmp(lut_config_entry.mac.octet,lut_data.mac.octet,ETHER_ADDR_LEN)
                    ||lut_config_entry.cvid_fid!=lut_data.cvid_fid
                    ||lut_config_entry.ivl_svl!=lut_data.ivl_svl
                    ||lut_config_entry.fid!=lut_data.fid
                    ||lut_config_entry.ctag_if!=lut_data.ctag_if
                    ||lut_config_entry.spa!=lut_data.spa
                    ||lut_config_entry.age!=lut_data.age
                    ||lut_config_entry.auth!=lut_data.auth
                    ||lut_config_entry.sa_block!=lut_data.sa_block
                    ||lut_config_entry.da_block!=lut_data.da_block
                    ||lut_config_entry.arp_used!=lut_data.arp_used
                    ||lut_config_entry.ext_spa!=lut_data.ext_spa)
                {
                    rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d\n"
                        ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry);
//                    return ret;
                }
                
                break;
            case RTL9602C_L2_MC_DSLt:
                if(lut_data.table_type!=RTL9602C_RAW_LUT_ENTRY_TYPE_L2MC
                    ||lut_data.mac.octet[0]!=0x01
                    ||lut_data.l3lookup!=0
                    ||lut_data.ip6!=0
                    ||memcmp(lut_config_entry.mac.octet+1,lut_data.mac.octet+1,ETHER_ADDR_LEN-1)
                    ||lut_config_entry.cvid_fid!=lut_data.cvid_fid
                    ||lut_config_entry.ivl_svl!=lut_data.ivl_svl
                    ||lut_config_entry.mbr!=lut_data.mbr
                    ||lut_config_entry.ext_mbr!=lut_data.ext_mbr)
                {
                    rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d"
                        ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry);
//                    return ret;
                }
                
                break;
            case RTL9602C_L3_MC_ROUTEt:
                if(lut_data.table_type!=RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE
                    ||lut_data.l3lookup!=1
                    ||lut_data.ip6!=0
                    ||lut_config_entry.wan_sa!=lut_data.wan_sa
                    ||lut_config_entry.gip!=lut_data.gip
                    ||lut_config_entry.l3_idx!=lut_data.l3_idx
                    ||lut_config_entry.sip_idx!=lut_data.sip_idx
                    ||lut_config_entry.sip_filter!=lut_data.sip_filter
                    ||lut_config_entry.mbr!=lut_data.mbr
                    ||lut_config_entry.ext_mbr!=lut_data.ext_mbr)
                {
                    rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d"
                        ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry);
//                    return ret;
                }
                
                break;
            case RTL9602C_L3_IP6MCt:
                if(lut_data.table_type!=RTL9602C_RAW_LUT_ENTRY_TYPE_L3IP6MC
                    ||lut_data.l3lookup!=1
                    ||lut_data.ip6!=1
                    ||lut_config_entry.wan_sa!=lut_data.wan_sa
                    ||memcmp(lut_config_entry.dip6.ipv6_addr,lut_data.dip6.ipv6_addr,IPV6_ADDR_LEN)
                    ||lut_config_entry.l3mcr_idx!=lut_data.l3mcr_idx
                    ||lut_config_entry.mbr!=lut_data.mbr
                    ||lut_config_entry.ext_mbr!=lut_data.ext_mbr)
                {
                    rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d"
                        ,lut_tbl_config[i].tblName,lut_tbl_config[i].regAddr,lut_tbl_config[i].testEntry);
//                    return ret;
                }
                
                break;
            default:
                rt_log_printf("wrong switch case in LUT Table Config");
                return ret;
        }

    }

    //l2_table
    for(i=0;i<L2_TBL_CONFIG_NUM;i++)
    {
        memset(data,0,5*sizeof(uint32));
        if ((ret = table_read(l2_tbl_config[i].regAddr, l2_tbl_config[i].testEntry, data)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d"
                ,l2_tbl_config[i].tblName,l2_tbl_config[i].regAddr,l2_tbl_config[i].testEntry );
            return ret;
        }

        if (memcmp(l2_tbl_config[i].testValue,data,5*sizeof(uint32)) != 0)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d [data]: 0x%08x%08x%08x%08x%08x [test value]: 0x%08x%08x%08x%08x%08x"
                ,l2_tbl_config[i].tblName,l2_tbl_config[i].regAddr,l2_tbl_config[i].testEntry
                ,data[0],data[1],data[2],data[3],data[4]
                ,l2_tbl_config[i].testValue[0],l2_tbl_config[i].testValue[1],l2_tbl_config[i].testValue[2],l2_tbl_config[i].testValue[3],l2_tbl_config[i].testValue[4] );
//            return ret;
        }
    }

    //l34_table
    for(i=0;i<L34_TBL_CONFIG_NUM;i++)
    {
        memset(data,0,5*sizeof(uint32));
        if ((ret = table_read(l34_tbl_config[i].regAddr, l34_tbl_config[i].testEntry, data)) != RT_ERR_OK)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d"
                ,l34_tbl_config[i].tblName,l34_tbl_config[i].regAddr,l34_tbl_config[i].testEntry );
            return ret;
        }

        if (memcmp(l34_tbl_config[i].testValue,data,5*sizeof(uint32)) != 0)
        {
            rt_log_printf("[Table]: %s [Addr]: 0x%x [Entry]: %d [data]: 0x%08x%08x%08x%08x%08x [test value]: 0x%08x%08x%08x%08x%08x"
                ,l34_tbl_config[i].tblName,l34_tbl_config[i].regAddr,l34_tbl_config[i].testEntry
                ,data[0],data[1],data[2],data[3],data[4]
                ,l34_tbl_config[i].testValue[0],l34_tbl_config[i].testValue[1],l34_tbl_config[i].testValue[2],l34_tbl_config[i].testValue[3],l2_tbl_config[i].testValue[4] );
 //           return ret;
        }
    }

    return RT_ERR_OK;
}



