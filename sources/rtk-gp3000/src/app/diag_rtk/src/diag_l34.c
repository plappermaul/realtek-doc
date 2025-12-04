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
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hal/common/halctrl.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/l34.h>
#include <dal/apollomp/raw/apollomp_raw_l34.h>


const char *ext_type_Str[L34_EXTIP_TYPE_END] = {
DIAG_STR_NAPT,
DIAG_STR_NAT,
DIAG_STR_LOCAL_PUBLIC
};



/* convert prefix 64 IPv6 address from number to string.*/
static int32
diag_ipv62str_prefix64(uint8 *str, const uint8 *ipv6)
{
    uint32  i;
    uint16  ipv6_ptr[UTIL_IPV6_TMP_BUFFER_LENGTH] = {0};

    if ((NULL == str) || (NULL == ipv6))
    {
        return RT_ERR_FAILED;
    }

    for (i = 0; i < 4 ;i++)
    {
        ipv6_ptr[i] = ipv6[i*2+1];
        ipv6_ptr[i] |=  ipv6[i*2] << 8;
    }
	
    sprintf(str, "%x:%x:%x:%x", ipv6_ptr[0], ipv6_ptr[1], ipv6_ptr[2], ipv6_ptr[3]);

    return RT_ERR_OK;
}

static void 
diag_print_napt(rtk_l34_naptOutbound_entry_t napt)
{
	diag_util_mprintf("NAPTR Table Id:\t%d\n",napt.hashIdx);
	diag_util_mprintf("Priority State:\t%s\n",(napt.priValid == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
	diag_util_mprintf("Priority Value:\t%d\n",napt.priValue);
	diag_util_mprintf("State:\t\t%s\n", (napt.valid == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
}

static void
diag_print_naptr(rtk_l34_naptInbound_entry_t naptr)
{
	char tmp[128]="";

	diag_util_mprintf("ExtIP ID:\t%d\n",naptr.extIpIdx);
#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {		
		diag_util_mprintf("ExtPortLSB:\t%d\n",naptr.extPortLSB);
	}
#endif			
#ifdef CONFIG_SDK_RTL9602C
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {		
		diag_util_mprintf("ExtPort:\t%d\n", naptr.extPortLSB | (naptr.extPortHSB << 8));
	}
#endif	
	diag_util_ip2str((uint8*)tmp, (uint32)naptr.intIp);
	diag_util_mprintf("Internal IP:\t%s\n",tmp);
	diag_util_mprintf("Internal Port:\t%d\n",naptr.intPort);
	diag_util_mprintf("IS TCP:\t\t%s\n", naptr.isTcp ? DIAG_STR_TCP : DIAG_STR_UDP);
	diag_util_mprintf("Priority State:\t%s\n", (naptr.priValid == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
	diag_util_mprintf("Priority Value:\t%d\n",naptr.priId);
	diag_util_mprintf("Remote Hash:\t%d\n",naptr.remHash);
	diag_util_mprintf("NAT mode:\t%d\n",naptr.valid);

}

void _cparser_cmd_l34_arp_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-7s ","Valid");
    diag_util_mprintf("%-6s ","NH_IDX");   
	
#ifdef CONFIG_SDK_RTL9602C
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
    	diag_util_mprintf("%-15s ","IP"); 
    }
#endif
	diag_util_mprintf("\n"); 
}


void _cparser_cmd_l34_arp_entry_display(uint32 index,rtk_l34_arp_entry_t *entry)
{
    diag_util_printf("%-5d ",index);

    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);      
        diag_util_mprintf("%-6d ",entry->nhIdx);   
#ifdef CONFIG_SDK_RTL9602C
	    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
	    {
	    	uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];

			diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)entry->ipAddr);
            diag_util_printf("%-15s ",(uint8*)ip_tmp_buf);  
	    }
#endif
		diag_util_mprintf("\n"); 
    }
    else
    {
        /*invalid*/
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
}

void _cparser_cmd_l34_external_ip_table_header_display(void)
{
    diag_util_mprintf("%-5s %-7s %-15s %-15s %-6s %-3s %-8s\n",
                                                             "Index",
                                                             "Valid",
                                                             "Ext_IP",
                                                             "Int_IP",
                                                             "TYPE",
                                                             "NHidx",
                                                             "priority");    
}    

void _cparser_cmd_l34_external_ip_table_display(uint32 index,rtk_l34_ext_intip_entry_t *entry)
{
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    
    diag_util_printf("%-5d ",index);    
    
    
    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);      

        /*external IP address*/

        diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)entry->extIpAddr);
        diag_util_printf("%-15s ",(uint8*)ip_tmp_buf);   

        diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)entry->intIpAddr);
        diag_util_printf("%-15s ",(uint8*)ip_tmp_buf);   


        /*Table Type*/
        diag_util_printf("%-6s ",ext_type_Str[entry->type]);   
        
        /*next hop index*/
        diag_util_printf("%-8d ",entry->nhIdx);   
        
        /*priority*/
        if(0x1 == entry->prival)
            diag_util_mprintf("%-8d \n",entry->pri);   
        else
            diag_util_mprintf("%-8s \n",DIAG_STR_NO);   
          
    }
    else
    {
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
    
 
}


cparser_result_t
_cparser_cmd_l34_set_external_ip_set(
    cparser_context_t *context,
    uint32_t  *table_idx_ptr,
    uint32_t  *external_ip_ptr,
    uint32_t  *internal_ip_ptr,
    uint32_t  *nh_idx_ptr,
    int32_t  pri)
{
    rtk_l34_ext_intip_entry_t entry;
    int32   ret = RT_ERR_FAILED; 
    int32   index;
    DIAG_UTIL_PARAM_CHK();
    
    /*get index*/
    index = *table_idx_ptr;
    
    /*get type*/
    if('N' == TOKEN_CHAR(6,0))
    {
        if('T' == TOKEN_CHAR(6,2))    
            entry.type = L34_EXTIP_TYPE_NAT;            
        else
            entry.type = L34_EXTIP_TYPE_NAPT;            
        
    }
    else
    {
        entry.type = L34_EXTIP_TYPE_LP;            
    }

    /*get external IP address*/
    entry.extIpAddr = *external_ip_ptr;

    
    /*get internal IP address*/
    entry.intIpAddr = *internal_ip_ptr;

    /*get nexthop index*/
    entry.nhIdx = *nh_idx_ptr;
    
    /*get priority*/
    
    
    if(-1 == pri)
    {
        entry.prival = 0;
    }
    else
    {
        entry.prival = 1;
        entry.pri = pri;
    
    }     
    
    /*set vlid bit*/
    entry.valid = 1;
    
    
    if((ret = rtk_l34_extIntIPTable_set(index,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    return CPARSER_OK;
}    /* end of _cparser_cmd_l34_set_external_ip_set */

void _cparser_cmd_l34_routing_table_header_display(void)
{
    diag_util_mprintf("%-5s %-7s %-15s %-4s %-8s %-14s %-10s\n",
                                                             "Index",
                                                             "Valid",
                                                             "IP_Addr",
                                                             "MASK",
                                                             "Internal",
                                                             "ROUTE_TYPE",
                                                             "MISC_INFO");    
}    

void _cparser_cmd_l34_routing_table_display(uint32 index, rtk_l34_routing_entry_t *entry)
{
  
    diag_util_printf("%-5d ",index);    
    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);      

        /*ip address*/
        diag_util_printf("%-15s ",diag_util_inet_ntoa(entry->ipAddr));   


        /*mask*/
        diag_util_printf("%-4d ",entry->ipMask);   
        
        /*is internal*/
        if(entry->internal)
            diag_util_printf("%-8s ",DIAG_STR_YES);  
        else
            diag_util_printf("%-8s ",DIAG_STR_NO); 
        
        /*ROUTE_TYPE*/
		diag_util_printf("%-14s ",diagStr_routeType[entry->process]); 

		/*MISC_INFO*/
		switch(entry->process)
        {
            case L34_PROCESS_ARP:
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
                if((APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE) ||
                    (APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE))
                 {
               
                    /*arp info*/
                    diag_util_printf("netif_idx(%2d) arp_start[%3d(%3d)] arp_end[%3d(%3d)]",entry->netifIdx,entry->arpStart,entry->arpStart<<2,entry->arpEnd,entry->arpEnd<<2); 
                }
#endif
#if defined(CONFIG_SDK_RTL9602C)                
                if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
                {
                    diag_util_printf("netif_idx(%2d)",entry->netifIdx);

                }
#endif
                break;
            case L34_PROCESS_NH:
                /*next hop info*/
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
                if((APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE) ||
                    (APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE))
                {
                    diag_util_printf("nh_idx(%4d) nh_num(%2d) nh_algo(%1d) ip_doman_num(%d)",entry->nhStart,entry->nhNum,entry->nhAlgo,entry->ipDomain);                
                }
#endif
#if defined(CONFIG_SDK_RTL9602C)
                if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
                {
                    diag_util_printf("nh_idx(%4d)",entry->nhNxt);                
                }                
#endif
                break;
            default:
                diag_util_printf("%-14s",DIAG_STR_INVALID); 
                break;

        }
		diag_util_mprintf("\n"); 

    }
    else
    {
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
 
  
}

void _cparser_cmd_l34_netif_table_header_display(void)
{
    diag_util_mprintf("%-5s %-7s %-7s %-4s %-8s %-17s %-4s ",
                                                             "Index",
                                                             "Valid",
                                                             "Vlan ID",
                                                             "MTU",
                                                             "EN_ROUTE",
                                                             "GATEWAY_MAC",
                                                             "MASK"); 
#ifdef CONFIG_SDK_RTL9602C
	if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
	{
		diag_util_mprintf("%-3s %-6s %-7s %-15s %-3s", 
                                                "L34", 
                                                "CtagIf",
                                                "DS-Lite",
                                                "IPaddr",
                                                "IPv6");
	}
#endif
	diag_util_mprintf("\n");
}    

void _cparser_cmd_l34_netif_table_display(uint32 index,rtk_l34_netif_entry_t *entry)
{
  
    diag_util_printf("%-5d ",index);  

    if(entry->valid == 1)
    {
	    /*valid*/
	    diag_util_printf("%-7s ",DIAG_STR_VALID);      

	    /*vlan id*/
	    diag_util_printf("%-7d ",entry->vlan_id);   


	    /*MTU*/
	    diag_util_printf("%-4d ",entry->mtu);   
	    
	    /*EN_ROUTE*/
	    if(entry->enable_rounting)
	        diag_util_printf("%-8s ",DIAG_STR_YES);  
	    else
	        diag_util_printf("%-8s ",DIAG_STR_NO); 
	    
	    /*GATEWAY_MAC*/
	    diag_util_printf("%-17s ",diag_util_inet_mactoa((uint8 *)&entry->gateway_mac));   
	    
	    /*MASK*/
	    diag_util_mprintf("%-4d ",entry->mac_mask);
#ifdef CONFIG_SDK_RTL9602C
        if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
        {
            uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
            
            diag_util_mprintf("%-3s ", diagStr_enDisplay[entry->isL34]);

            diag_util_mprintf("%-6s ", diagStr_enDisplay[entry->isCtagIf]);

            if(entry->dslite_state)
                diag_util_mprintf("%-7d ", entry->dslite_idx);
            else
                diag_util_mprintf("%-7s ", DIAG_STR_X);

            diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)entry->ipAddr);
            diag_util_printf("%-15s ",(uint8*)ip_tmp_buf);

			diag_util_mprintf("%-3s ", diagStr_enDisplay[entry->isIpv6]);
            
        }
#endif
		diag_util_mprintf("\n");
	}
    else
    {
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }

}


void _cparser_cmd_l34_nexthop_table_header_display(void)
{
    diag_util_mprintf("%-5s %-8s %-9s %-9s %-9s ",
                      "Index",
                      "TYPE",
                      "NH_L2_IDX",
                      "IF_IDX",
                      "PPPoE_IDX");    

#ifdef CONFIG_SDK_RTL9602C
	if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
		diag_util_mprintf("%-9s", "KeepPPPoE");
	}
#endif

	diag_util_mprintf("\n");
	
}    

void _cparser_cmd_l34_nexthop_table_display(uint32 index,rtk_l34_nexthop_entry_t *entry)
{
    /*index*/  
    diag_util_printf("%-5d ",index);    

    /*type*/
    diag_util_printf("%-8s ",diagStr_l34NexthopTypeStr[entry->type]);   

    /*NH_L2_IDX*/
    diag_util_printf("%-9d ",entry->nhIdx);   
        
    /*IF_IDX*/
    diag_util_printf("%-9d ",entry->ifIdx);   

    /*PPPoE_IDX*/
    diag_util_mprintf("%-9d ",entry->pppoeIdx);   

#ifdef CONFIG_SDK_RTL9602C
	if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
		diag_util_mprintf("%-9d", entry->keepPppoe);
	}
#endif   

	diag_util_mprintf("\n");
}

void _cparser_cmd_l34_pppoe_table_header_display(void)
{
    diag_util_mprintf("%-5s %-12s\n",
                      "Index",
                      "Session_ID");    
}    

void _cparser_cmd_l34_pppoe_table_display(uint32 index,rtk_l34_pppoe_entry_t *entry)
{
    /*index*/  
    diag_util_printf("%-5d ",index);    

    /*PPPoE_IDX*/
    diag_util_mprintf("%-12d \n",entry->sessionID);   
}



void _cparser_cmd_l34_route6_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-7s ","Valid");
    diag_util_printf("%-30s","IPv6 Addr");    
    diag_util_printf("%-12s","IPv6 Prefix");    
    diag_util_printf("%-10s","NHorIfId");    
    diag_util_printf("%-10s","rt2waninf");    
    diag_util_mprintf("%-6s\n","type");    
}


void _cparser_cmd_l34_route6_table_display(uint32 index,rtk_ipv6Routing_entry_t *entry)
{
    char tmp[128]="";
	
    diag_util_printf("%-5d ",index);

    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);  
        diag_util_ipv62str((uint8*)tmp, (uint8*)entry->ipv6Addr.ipv6_addr);
        diag_util_printf("%-30s",tmp);    
        diag_util_printf("%-12d",entry->ipv6PrefixLen);    
        diag_util_printf("%-10d",entry->nhOrIfidIdx);    
        diag_util_printf("%-10d",entry->rt2waninf);    
        diag_util_mprintf("%-6s\n",diagStr_routeType[entry->type]);    
    }
    else
    {
        /*invalid*/
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
}

void _cparser_cmd_l34_neigh_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-7s ","Valid");
    diag_util_printf("%-20s","IFId");    
    diag_util_printf("%-10s","RouteId");    
    diag_util_mprintf("%-8s\n","L2Id");    
   
}


void _cparser_cmd_l34_neigh_table_display(uint32 index,rtk_ipv6Neighbor_entry_t *entry)
{
	
    diag_util_printf("%-5d ",index);
    char  tmp[128]="";

    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);  
	 diag_util_eui642str(tmp, &entry->ipv6Ifid);
	 diag_util_printf("%-20s",tmp);     
        diag_util_printf("%-10d",entry->ipv6RouteIdx);    
        diag_util_mprintf("%-8d\n",entry->l2Idx);    
    }
    else
    {
        /*invalid*/
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
}


void _cparser_cmd_l34_binding_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-10s ","wanTypeIdx");
    diag_util_printf("%-10s","bindProto");    
    diag_util_printf("%-12s","extPortMask");
    diag_util_printf("%-10s","portMask");    	
    diag_util_mprintf("%-6s\n","vidLan");    
   
}


void _cparser_cmd_l34_binding_table_display(uint32 index,rtk_binding_entry_t *entry)
{

	diag_util_printf("%-5d ",index);

	/*valid*/
	diag_util_printf("%-10d ",entry->wanTypeIdx);  
	diag_util_printf("%-10d",entry->bindProto);    
	diag_util_printf("0x%-8x",entry->extPortMask.bits[0]);    
	diag_util_printf("0x%-8x",entry->portMask.bits[0]);    
	diag_util_mprintf("%-6d \n",entry->vidLan);   

}


void _cparser_cmd_l34_wanType_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-6s ","nhIdx");
    diag_util_mprintf("%-6s\n","wanType");    
   
}


void _cparser_cmd_l34_wanType_table_display(uint32 index,rtk_wanType_entry_t *entry)
{

	diag_util_printf("%-5d ",index);

	/*valid*/
	diag_util_printf("%-6d ",entry->nhIdx);  
	diag_util_mprintf("%-6d\n",entry->wanType);    

}

void _cparser_cmd_l34_dslite_table_header_display(void)
{
	diag_util_mprintf("%-5s %-7s %-30s %-30s %-13s %-4s %-10s %-9s\n",
															 "Index",
															 "Valid",
															 "AFTR_IP",
															 "B4_IP",
															 "TC_OPTION",
															 "TC",
															 "FLOW_LABEL",
															 "HOP_LIMIT"); 
}

void _cparser_cmd_l34_dslite_table_display(rtk_l34_dsliteInf_entry_t *entry)
{
	char tmp[128]="";

	diag_util_printf("%-5d ", entry->index);
	diag_util_printf("%-7s ", diagStr_valid[entry->valid]);

	diag_util_ipv62str((uint8*)tmp, (uint8*)entry->ipAftr.ipv6_addr);
	diag_util_printf("%-30s ", tmp);
	diag_util_ipv62str((uint8*)tmp, (uint8*)entry->ipB4.ipv6_addr);
	diag_util_printf("%-30s ", tmp);	

	diag_util_printf("%-13s ", diagStr_tcAssignOption[entry->tcOpt]);
	diag_util_printf("%-4d ", entry->tc);

	diag_util_printf("0x%-8x ", entry->flowLabel);
	diag_util_mprintf("%-9d\n", entry->hopLimit);
}

void _cparser_cmd_l34_dslite_mcast_table_header_display(void)
{
	diag_util_printf("%-5s %-20s %-20s ",
									 "Index",
									 "SRC_IP",
									 "SRC_MASK");
										  
	diag_util_mprintf("%-20s %-20s\n",
									 "DST_IP",
									 "DST_MASK");
}

void _cparser_cmd_l34_dslite_mcast_table_display(rtk_l34_dsliteMc_entry_t *entry)
{
	char tmp[128]="";
	
	diag_util_printf("%-5d ", entry->index);

	diag_ipv62str_prefix64((uint8*)tmp, (uint8*)entry->ipUPrefix64.ipv6_addr);
	diag_util_printf("%-20s ", tmp);
	diag_ipv62str_prefix64((uint8*)tmp, (uint8*)entry->ipUPrefix64Mask.ipv6_addr);
	diag_util_printf("%-20s ", tmp);	

	diag_ipv62str_prefix64((uint8*)tmp, (uint8*)entry->ipMPrefix64.ipv6_addr);
	diag_util_printf("%-20s ", tmp);
	diag_ipv62str_prefix64((uint8*)tmp, (uint8*)entry->ipMPrefix64Mask.ipv6_addr);
	diag_util_mprintf("%-20s\n", tmp);	
}

void _cparser_cmd_l34_flow_route_table_header_display(void)
{
	diag_util_printf("%-5s %-7s %-7s %-15s %-15s %-8s %-8s %-3s %-13s %-7s %-8s\n",
																	 "Index",
																	 "Valid",
																	 "IP_Type",
																	 "SRC_IP",
																	 "DST_IP",															 
																	 "SRC_PORT",
																	 "DST_PORT",
																	 "TCP",
																	 "ROUTE_TYPE",
																	 "NEXTHOP",
																	 "PRIORITY");							  
}

void _cparser_cmd_l34_flow_route_table_ipv6_header_display(void)
{
	diag_util_printf("%-5s %-7s %-7s %-40s %-40s ",
																	 "Index",
																	 "Valid",
																	 "IP_Type",
																	 "SRC_IP",
																	 "DST_IP");	
	diag_util_printf("%-8s %-8s %-3s %-13s %-7s %-8s\n",															 
																	 "SRC_PORT",
																	 "DST_PORT",
																	 "TCP",
																	 "ROUTE_TYPE",
																	 "NEXTHOP",
																	 "PRIORITY");		
}


void _cparser_cmd_l34_flow_route_table_display(rtk_l34_flowRoute_entry_t *entry)
{
	char tmp[128]="";
	
	diag_util_printf("%-5d ",entry->index);

	/*valid*/
	if(entry->valid == 1)
	{
    diag_util_printf("%-7s ",DIAG_STR_VALID);      

	diag_util_printf("%-7s ", (entry->isIpv4 ? DIAG_STR_IPV4 : DIAG_STR_IPV6));   

	if(entry->isIpv4 == 1)
	{
	    /*sip address*/
	    diag_util_printf("%-15s ",diag_util_inet_ntoa(entry->sip.ipv4)); 
		/*dip address*/
	    diag_util_printf("%-15s ",diag_util_inet_ntoa(entry->dip.ipv4));
	} else {
		char tmp[128]="";
		
		/*sip address*/
		diag_util_ipv62str((uint8*)tmp, (uint8*)entry->sip.ipv6.ipv6_addr);
	 	diag_util_printf("%-40s ",tmp);  
		/*dip address*/
	    diag_util_ipv62str((uint8*)tmp, (uint8*)entry->dip.ipv6.ipv6_addr);
	 	diag_util_printf("%-40s ",tmp);  
	}
	
	/*source port*/
	diag_util_printf("%-8d ",entry->sport);
	/*dst port*/
	diag_util_printf("%-8d ",entry->dport);
	/*is TCP*/
	diag_util_printf("%-3s ", (entry->isTcp ? DIAG_STR_TCP : DIAG_STR_UDP));

	/*ROUTE_TYPE*/
	switch(entry->process)
	{
	    case L34_FLOWRT_TYPE_CPU:
	        diag_util_mprintf("%-13s ",DIAG_STR_TRAP2CPU); 
	        break;
	    case L34_FLOWRT_TYPE_DROP:
	        diag_util_mprintf("%-13s ",DIAG_STR_DROP); 
	        break;

	    case L34_FLOWRT_TYPE_LOCAL:
	        diag_util_printf("%-13s ",DIAG_STR_ROUTE_TYPE_LOCAL); 
        	break;
	        
	    case L34_FLOWRT_TYPE_GLOBAL:
	        diag_util_printf("%-13s ",DIAG_STR_ROUTE_TYPE_GLOBAL); 
			break;
	    default:
	        diag_util_mprintf("%-13s ",DIAG_STR_INVALID); 
	        break;
	}
	/*nexhop index*/
	diag_util_printf("%-7d ",entry->nexthopIdx);
	}
	else 
	{
		diag_util_printf("%-7s ",DIAG_STR_INVALID);
	}

	/*priority*/
	if(entry->pri_valid == 1)
	{
		diag_util_printf("%-8d ",entry->priority);
	} else {
		diag_util_printf("%-8s ",DIAG_STR_INVALID);
	}
	
	diag_util_printf("\n");
										  
}

void _cparser_cmd_l34_ipmc_table_header_display(void)
{
	diag_util_printf("%-5s %-5s %-9s %-6s %-9s %-5s %-8s\n",
													"Index",
													"NETIF",
													"SIP_TRANS",
													"EXT_IP",
													"PPPoE_ACT",															 
													"PPPoE",
													"UNTAGMBR");		
}

void _cparser_cmd_l34_ipmc_table_display(uint32 index, rtk_l34_ipmcTrans_entry_t *entry)
{
	diag_util_printf("%-5d ", index);
	
	diag_util_printf("%-5d ", entry->netifIdx);

	diag_util_printf("%-9s ", entry->sipTransEnable == 1 ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

	diag_util_printf("%-6d ", entry->extipIdx);

	diag_util_printf("%-9s ", diagStr_ipmcPppoeAct[entry->pppoeAct]);

	diag_util_printf("%-5d ", entry->pppoeIdx);

	diag_util_printf("%-9s", diag_util_mask32tostr(entry->untagMbr.bits[0]));

	diag_util_printf("\n");
}


static int32
_diag_util_bindingAct2Str(char *str, rtk_l34_bindAct_t act)
{
	switch(act){
	case L34_BIND_ACT_DROP:
		sprintf(str,"drop\n");
	break;
	case L34_BIND_ACT_TRAP:
		sprintf(str,"trap\n");
	break;
	case L34_BIND_ACT_FORCE_L2BRIDGE:
		sprintf(str,"fore L2 bridge\n");
	break;
	case L34_BIND_ACT_PERMIT_L2BRIDGE:
		sprintf(str,"premit and L2 bridge\n");
	break;
	case L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP:
		sprintf(str,"ipv4 packet lookup and ipv6 packet trap\n");	
	break;
	case L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4:
		sprintf(str,"force L3 routing and skip lookup l4 table\n");
	break;
	case L34_BIND_ACT_FORCE_BINDL3:
		sprintf(str,"force L3 routing\n");
	break;
	case L34_BIND_ACT_NORMAL_LOOKUPL34:
		sprintf(str,"normal L34 lookup\n");
	break;
	default:
	break;
	}
	return RT_ERR_OK;
}

/*
 * l34 init
 */
cparser_result_t
cparser_cmd_l34_init(
    cparser_context_t *context)
{
	int32 ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_init(),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_lite_init */



/*
 * l34 reset table ( l3 | pppoe | nexthop | interface | external-ip | arp | naptr | napt | wan-type | binding | neighbor | l3v6 | flow )
 */
cparser_result_t
cparser_cmd_l34_reset_table_l3_pppoe_nexthop_interface_external_ip_arp_naptr_napt_wan_type_binding_neighbor_l3v6_flow(
    cparser_context_t *context)
{
	 rtk_l34_table_type_t   l34_table;
	int32   ret = RT_ERR_FAILED;
	DIAG_UTIL_PARAM_CHK();

	switch(TOKEN_STR(3)[0])
	{
		case 'l':
			if(strlen(TOKEN_STR(3)) == 2)
				l34_table = L34_ROUTING_TABLE;
			else
				l34_table = L34_IPV6_ROUTING_TABLE;
			break;	  
		case 'p':
			l34_table = L34_PPPOE_TABLE;
			break;	  
		case 'i':
			l34_table = L34_NETIF_TABLE;
			break;	  
		case 'e':
			l34_table = L34_INTIP_TABLE;
			break;	  
		case 'a':
			l34_table = L34_ARP_TABLE;
			break;	  
		case 'n':
			if(TOKEN_STR(3)[2]=='x')
				l34_table = L34_NEXTHOP_TABLE;				  
			else if(TOKEN_STR(3)[2]=='i')
				l34_table = L34_IPV6_NEIGHBOR_TABLE;	   
			else
			{
				if(strlen(TOKEN_STR(3)) == 5)
					l34_table = L34_NAPTR_TABLE;
				else
					l34_table = L34_NAPT_TABLE;
			}
			break;
		case 'w':
			l34_table = L34_WAN_TYPE_TABLE;
			break;
		case 'b':
			l34_table = L34_BINDING_TABLE;
			break;
		case 'f':
			l34_table = L34_FLOW_ROUTING_TABLE;
			break;
		default:
	        return CPARSER_NOT_OK;
	}

	if((ret = rtk_l34_table_reset(l34_table)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_table_l3_pppoe_nexthop_interface_external_ip_arp_naptr_napt */

/*
 * l34 get arp
 */
cparser_result_t
cparser_cmd_l34_get_arp(
    cparser_context_t *context)
{
	uint32 index;
	rtk_l34_arp_entry_t entry;
	int32   ret = RT_ERR_FAILED;
	  
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_cparser_cmd_l34_arp_header_display();
	for(index = 0; index <HAL_L34_ARP_ENTRY_MAX(); index++)
	{
	    if((ret = rtk_l34_arpTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
		
	    if(entry.valid)
	    	_cparser_cmd_l34_arp_entry_display(index,&entry);        
	        
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp */

/*
 * l34 get arp <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_arp_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_l34_arp_entry_t entry;
    int32   ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_l34_arpTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    _cparser_cmd_l34_arp_header_display();
    _cparser_cmd_l34_arp_entry_display(*index_ptr,&entry);

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_index */

/*
 * l34 set arp <UINT:index> next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_arp_index_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    rtk_l34_arp_entry_t entry;
    int32   ret = RT_ERR_FAILED;
  
    DIAG_UTIL_PARAM_CHK();
    if(*index_ptr >= HAL_L34_ARP_ENTRY_MAX()) 
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
	DIAG_UTIL_ERR_CHK(rtk_l34_arpTable_get(*index_ptr,&entry),ret);
    
    entry.valid = 1;
    entry.nhIdx = *nh_index_ptr;
    if((ret = rtk_l34_arpTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }        
        
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_arp_index_next_hop_table_nh_index */

/*
 * l34 set arp <UINT:index> ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_arp_index_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
    rtk_l34_arp_entry_t entry;
    int32   ret = RT_ERR_FAILED;
  
    DIAG_UTIL_PARAM_CHK();
    if(*index_ptr >= HAL_L34_ARP_ENTRY_MAX()) 
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
	DIAG_UTIL_ERR_CHK(rtk_l34_arpTable_get(*index_ptr,&entry),ret);

    entry.valid = 1;
    entry.ipAddr = *ip_ptr;
	
    DIAG_UTIL_ERR_CHK(rtk_l34_arpTable_set(*index_ptr,&entry),ret);   
        
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_arp_index_ip_ip */

/*
 * l34 del arp <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_arp_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(*index_ptr >= HAL_L34_ARP_ENTRY_MAX()) 
    {
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
        return CPARSER_NOT_OK;
    }
    if((ret = rtk_l34_arpTable_del(*index_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_arp_index */

/*
 * l34 get external-ip 
 */
cparser_result_t
cparser_cmd_l34_get_external_ip(
    cparser_context_t *context)
{
    uint32_t index; /*tble index*/
    int32   ret = RT_ERR_FAILED;
    rtk_l34_ext_intip_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_external_ip_table_header_display();
    for(index = 0; index < HAL_L34_EXTIP_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_extIntIPTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_external_ip_table_display(index,&entry);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_external_ip */

/*
 * l34 get external-ip <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_external_ip_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_l34_ext_intip_entry_t entry;
    int32   ret = RT_ERR_FAILED;    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(*index_ptr >= HAL_L34_EXTIP_ENTRY_MAX()) 
    {
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
        return CPARSER_NOT_OK;
    }
    if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    _cparser_cmd_l34_external_ip_table_header_display();
    _cparser_cmd_l34_external_ip_table_display(*index_ptr,&entry);

  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_external_ip_index */

/*
 * l34 set external-ip <UINT:index> type ( nat | napt | lp )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_type_nat_napt_lp(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	if(!osal_strcmp(TOKEN_STR(5),"nat")){
		entry.type = L34_EXTIP_TYPE_NAT;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"napt")){
		entry.type = L34_EXTIP_TYPE_NAPT;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"lp")){
		entry.type = L34_EXTIP_TYPE_LP;
	}

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_type_nat_napt_lp */

/*
 * l34 set external-ip <UINT:index> external-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_external_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.extIpAddr = *ip_ptr;

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_external_ip_ip */

/*
 * l34 set external-ip <UINT:index> internal-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_internal_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.intIpAddr = *ip_ptr;

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_internal_ip_ip */

/*
 * l34 set external-ip <UINT:index> next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.nhIdx  = *nh_index_ptr;

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_next_hop_table_nh_index */

/*
 * l34 set external-ip <UINT:index> nat-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_nat_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.prival = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.prival = ENABLED;
	}

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_nat_priority_state_disable_enable */

/*
 * l34 set external-ip <UINT:index> nat-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_nat_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.pri= *priority_ptr;

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_nat_priority_priority */

/*
 * l34 set external-ip <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}

	if((ret = rtk_l34_extIntIPTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_state_disable_enable */

/*
 * l34 del external-ip <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_external_ip_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
     int32   ret = RT_ERR_FAILED; 
    DIAG_UTIL_PARAM_CHK();

    if((ret = rtk_l34_extIntIPTable_del((uint32_t)*index_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    } 
    
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_external_ip_index */

/*
 * l34 get routing 
 */
cparser_result_t
cparser_cmd_l34_get_routing(
    cparser_context_t *context)
{
    rtk_l34_routing_entry_t entry;
    int32 ret = RT_ERR_FAILED; 
    uint32 index;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    _cparser_cmd_l34_routing_table_header_display();

    for(index =0;index < HAL_L34_ROUTING_ENTRY_MAX();index++)
    {
        osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));
        if((ret = rtk_l34_routingTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }     
        
        _cparser_cmd_l34_routing_table_display(index,&entry);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_routing */

/*
 * l34 get routing <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_routing_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_l34_routing_entry_t entry;
    int32 ret = RT_ERR_FAILED; 

        
    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((*index_ptr >= HAL_L34_ROUTING_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);   
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     

    _cparser_cmd_l34_routing_table_header_display();
    _cparser_cmd_l34_routing_table_display(*index_ptr,&entry);
    
	return CPARSER_NOT_OK;

}    /* end of cparser_cmd_l34_get_routing_index */

/*
 * l34 set routing <UINT:index> ip <IPV4ADDR:ip> ip-mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_ip_ip_ip_mask_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *mask_ptr)
{
     rtk_l34_routing_entry_t entry;
    int32   ret = RT_ERR_FAILED; 

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((*index_ptr >= HAL_L34_ROUTING_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);   
    RT_PARAM_CHK((*mask_ptr > 31), CPARSER_ERR_INVALID_PARAMS);   
    
    osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

    if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }    
   
   
    /*IP address*/
    if((ret = diag_util_str2ip((uint32 *)&entry.ipAddr,(uint8 *)TOKEN_STR(5))) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }


    /*IP Mask*/
    entry.ipMask = *mask_ptr; 
	
    /*call API to set entry*/            
    if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    
    
    return CPARSER_OK;
    
}    /* end of cparser_cmd_l34_set_routing_index_ip_ip_ip_mask_mask */

/*
 * l34 set routing <UINT:index> interface-type ( internal | external )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_interface_type_internal_external(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    

	if(!osal_strcmp(TOKEN_STR(5),"internal")){
		entry.internal = ENABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"external")){
		entry.internal = DISABLED;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_interface_type_internal_external */

/*
 * l34 set routing <UINT:index> type ( drop | trap )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_drop_trap(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    

	if(!osal_strcmp(TOKEN_STR(5),"drop")){
		entry.process = L34_PROCESS_DROP;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"trap")){
		entry.process = L34_PROCESS_CPU;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_drop_trap */

/*
 * l34 set routing <UINT:index> type local-route destination-netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_local_route_destination_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.process = L34_PROCESS_ARP;
	entry.netifIdx = *netif_index_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_local_route_destination_netif_netif_index */

/*
 * l34 set routing <UINT:index> type local-route arp-start-address <UINT:start_addr> arp-end-address <UINT:end_addr>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_local_route_arp_start_address_start_addr_arp_end_address_end_addr(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *start_addr_ptr,
    uint32_t  *end_addr_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.process = L34_PROCESS_ARP;
	entry.arpStart = *start_addr_ptr;
	entry.arpEnd = *end_addr_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_local_route_arp_start_address_start_addr_arp_end_address_end_addr */

/*
 * l34 set routing <UINT:index> type global-route next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.process = L34_PROCESS_NH;
	entry.nhNxt = *nh_index_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_table_nh_index */

/*
 * l34 set routing <UINT:index> type global-route next-hop-start <UINT:address> next-hop-number <UINT:nh_number>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_start_address_next_hop_number_nh_number(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *address_ptr,
    uint32_t  *nh_number_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.process = L34_PROCESS_NH;
	entry.nhStart = *address_ptr;
	entry.nhNum = *nh_number_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_start_address_next_hop_number_nh_number */

/*
 * l34 set routing <UINT:index> type global-route next-hop-algo ( per-packet | per-session | per-source_ip )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_algo_per_packet_per_session_per_source_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_algo_per_packet_per_session_per_source_ip */

/*
 * l34 set routing <UINT:index> type global-route ip-domain-range <UINT:range>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_ip_domain_range_range(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *range_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_ip_domain_range_range */

/*
 * l34 set routing <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_state_disable_enable */

/*
 * l34 del routing <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_routing_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();

	if((ret = rtk_l34_routingTable_del((uint32_t)*index_ptr)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_routing_index */

/*
 * l34 get netif 
 */
cparser_result_t
cparser_cmd_l34_get_netif(
    cparser_context_t *context)
{
    uint32 index;
    int32   ret = RT_ERR_FAILED;
    rtk_l34_netif_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_netif_table_header_display();
    
    for(index = 0; index < HAL_L34_NETIF_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_netifTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_netif_table_display(index,&entry);
    }    
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_netif */

/*
 * l34 get netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
     int32   ret = RT_ERR_FAILED;
    rtk_l34_netif_entry_t entry;
  

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    
    _cparser_cmd_l34_netif_table_header_display();
    _cparser_cmd_l34_netif_table_display(*index_ptr,&entry);
    
    
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_netif_index */

/*
 * l34 set netif <UINT:index> gateway-mac <MACADDR:mac> mac-mask ( no-mask | 1bit-mask | 2bit-mask | 3bit-mask )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_gateway_mac_mac_mac_mask_no_mask_1bit_mask_2bit_mask_3bit_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *mac_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	osal_memcpy(&entry.gateway_mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
	if(!osal_strcmp(TOKEN_STR(7),"no-mask")){
		entry.mac_mask = 7;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"1bit-mask")){
		entry.mac_mask = 6;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"2bit-mask")){
		entry.mac_mask = 4;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"3bit-mask")){
		entry.mac_mask = 0;
	}

	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_gateway_mac_mac_mac_mask_no_mask_1bit_mask_2bit_mask_3bit_mask */

/*
 * l34 set netif <UINT:index> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_vid_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.vlan_id = *vid_ptr;
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
		{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
		}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_vid_vid */

/*
 * l34 set netif <UINT:index> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *mtu_ptr)
{
    	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	entry.mtu = *mtu_ptr;
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_mtu_mtu */

/*
 * l34 set netif <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	
	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_state_disable_enable */

/*
 * l34 set netif <UINT:index> l3-route state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_l3_route_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	
	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.enable_rounting = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.enable_rounting = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_l3_route_state_enable_disable */

/*
 * l34 set netif <UINT:index> dslite state ( enable | disable ) { <UINT:dslite_idx> }
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_dslite_state_enable_disable_dslite_idx(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *dslite_idx_ptr)
{
    int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
	
	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.dslite_state = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.dslite_state = ENABLED;
	}

	if(dslite_idx_ptr!=NULL){
		entry.dslite_idx = *dslite_idx_ptr;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_dslite_state_enable_disable_dslite_idx */

/*
 *  l34 set netif <UINT:index> l34 state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_l34_state_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.isL34 = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.isL34 = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_netif_index_l34_state_enable_disable */

/*
 *	l34 set netif <UINT:index> ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_ip_ip(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *ip_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	
	entry.ipAddr = (rtk_ip_addr_t)*ip_ptr;
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_netif_index_ip_ip */

/*
 *	l34 set netif <UINT:index> ctag-if state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_ctag_if_state_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.isCtagIf = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.isCtagIf = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_ip_ip */

/*
 *  l34 set netif <UINT:index> ipv6 state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_ipv6_state_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.isIpv6= DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.isIpv6 = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_netif_index_ipv6_state_enable_disable */


/*
 * l34 del netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
    rtk_l34_netif_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&entry,0x0,sizeof(rtk_l34_netif_entry_t));
    
    entry.valid = 0;
    
    if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_netif_index */

/*
 * l34 get nexthop 
 */
cparser_result_t
cparser_cmd_l34_get_nexthop(
    cparser_context_t *context)
{
     uint32 index;
    int32   ret = RT_ERR_FAILED;
    rtk_l34_nexthop_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_nexthop_table_header_display();
    
    for(index = 0; index < HAL_L34_NH_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_nexthopTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_nexthop_table_display(index,&entry);		
    }    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_nexthop */

/*
 * l34 get nexthop <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_nexthop_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
    rtk_l34_nexthop_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    

    if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    _cparser_cmd_l34_nexthop_table_header_display();
        
    _cparser_cmd_l34_nexthop_table_display(*index_ptr,&entry);
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nexthop_index */

/*
 * l34 set nexthop <UINT:index> netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
	int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.ifIdx = *netif_index_ptr;
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_netif_netif_index */

/*
 * l34 set nexthop <UINT:index> l2 <UINT:l2_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_l2_l2_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l2_index_ptr)
{
	int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.nhIdx = *l2_index_ptr;
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_l2_l2_index */

/*
 * l34 set nexthop <UINT:index> type ( ethernet | pppoe )
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_type_ethernet_pppoe(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	if(!osal_strcmp(TOKEN_STR(5),"ethernet")){
		entry.type = L34_NH_ETHER;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"pppoe")){
		entry.type = L34_NH_PPPOE;
	}
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_type_ethernet_pppoe */

/*
 * l34 set nexthop <UINT:index> pppoe <UINT:pppoe_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_pppoe_pppoe_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *pppoe_index_ptr)
{
       int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	entry.pppoeIdx = *pppoe_index_ptr;
	
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_pppoe_pppoe_index */


/*
 * l34 set nexthop <UINT:index> keepPppoe { replace-by-pppIdx | original | original-tag-or-add-by-pppIdx }
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_keepPppoe_replace_by_pppIdx_original_original_tag_or_add_by_pppIdx(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	if(!osal_strcmp(TOKEN_STR(5),"replace-by-pppIdx")){
		entry.keepPppoe = L34_NH_PPPOE_REPLACE_BY_PPPIDX;
	}else if(!osal_strcmp(TOKEN_STR(5),"original")){
		entry.keepPppoe = L34_NH_PPPOE_KEEP_ORIGINAL;
	}else{
		entry.keepPppoe = L34_NH_PPPOE_KEEP_ORIGINAL_OR_ADD_BY_PPPIDX;
	}
	
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_keepPppoe_replace_by_pppIdx_original_original_tag_or_add_by_pppIdx */

/*
 * l34 get pppoe 
 */
cparser_result_t
cparser_cmd_l34_get_pppoe(
    cparser_context_t *context)
{
    uint32 index;
    int32   ret = RT_ERR_FAILED;
    rtk_l34_pppoe_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_pppoe_table_header_display();
    
    for(index = 0; index < HAL_L34_PPPOE_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_pppoeTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_pppoe_table_display(index,&entry);
    }    
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe */

/*
 * l34 get pppoe <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   uint32 index = *index_ptr;
    int32 ret = RT_ERR_FAILED;
    rtk_l34_pppoe_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    if((ret = rtk_l34_pppoeTable_get(index,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    _cparser_cmd_l34_pppoe_table_header_display();
    _cparser_cmd_l34_pppoe_table_display(index,&entry);
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_index */

/*
 * l34 set pppoe <UINT:index> session-id <UINT:session_id>
 */
cparser_result_t
cparser_cmd_l34_set_pppoe_index_session_id_session_id(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *session_id_ptr)
{
    int32   ret = RT_ERR_FAILED;
    rtk_l34_pppoe_entry_t entry;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&entry,0x0,sizeof(rtk_l34_pppoe_entry_t));
    
    /*session*/
    entry.sessionID = *session_id_ptr;

    if((ret = rtk_l34_pppoeTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_pppoe_index_session_id_session_id */

/*
 * l34 get napt 
 */
cparser_result_t
cparser_cmd_l34_get_napt(
    cparser_context_t *context)
{
	rtk_l34_naptOutbound_entry_t napt;
	int32 ret;
	uint32 i;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_NAPT_ENTRY_MAX();i++){
		DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(i,&napt),ret);
		if(napt.valid)
		{
			diag_util_mprintf("===NAPT Id:\t%d===\n",i);
			diag_print_napt(napt);
		}
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_napt */

/*
 * l34 get napt <UINT:index>
 */

cparser_result_t
cparser_cmd_l34_get_napt_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);

	diag_util_mprintf("===NAPT Id:\t%d===\n",*index_ptr);
	diag_print_napt(napt);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_napt_index */

/*
 * l34 set napt <UINT:index> hash-index <UINT:hash_index>
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_hash_index_hash_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *hash_index_ptr)
{
	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.hashIdx = *hash_index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_hash_index_hash_index */

/*
 * l34 set napt <UINT:index> napt-priority state ( disable | enable )  
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_napt_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
   	 if('d' == TOKEN_CHAR(6, 0))
   	 {
   	 	state = DISABLED;
   	 }else{
		state = ENABLED;
	 }

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.priValid = state;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_napt_priority_state_disable_enable */

/*
 * l34 set napt <UINT:index> napt-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_napt_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	uint32 priValue = *priority_ptr;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.priValue= priValue;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);
	return CPARSER_OK;

}    /* end of cparser_cmd_l34_set_napt_index_napt_priority_priority */

/*
 * l34 set napt <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
   	 if('d' == TOKEN_CHAR(5, 0))
   	 {
   	 	state = DISABLED;
   	 }else{
		state = ENABLED;
	 }

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.valid= state;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);
	return CPARSER_OK;

}    /* end of cparser_cmd_l34_set_napt_index_state_disable_enable */

/*
 * l34 del napt <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_napt_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;

	DIAG_UTIL_PARAM_CHK();
	
	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_napt_index */

/*
 * l34 get naptr 
 */
cparser_result_t
cparser_cmd_l34_get_naptr(
    cparser_context_t *context)
{
	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	uint32 i;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_NAPTR_ENTRY_MAX();i++){
		osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
		DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(i,&naptr),ret);
		if(naptr.valid)
		{
			diag_util_mprintf("===NAPTR ID:\t%d===\n",i);
			diag_print_naptr(naptr);
		}

	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_naptr */

/*
 * l34 get naptr <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_naptr_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);
	diag_util_mprintf("===NAPTR ID:\t%d===\n",*index_ptr);
	diag_print_naptr(naptr);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_naptr_index */

/*
 * l34 set naptr <UINT:index> internal-ip <IPV4ADDR:ip> internal-port <UINT:port> 
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_internal_ip_ip_internal_port_port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *port_ptr)
{
	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.intIp = (rtk_ip_addr_t)*ip_ptr;
	naptr.intPort = *port_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_internal_ip_ip_internal_port_port */

/*
 * l34 set naptr <UINT:index> protocol ( tcp | udp )
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_protocol_tcp_udp(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	 if('t' == TOKEN_CHAR(5, 0))
   	 {
		naptr.isTcp = ENABLED;
	 }else
	 {
	 	naptr.isTcp = DISABLED;
	 }
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_protocol_tcp_udp */

/*
 * l34 set naptr <UINT:index> external-ip <UINT:extip_index> ( external-port-lsb | external-port ) <UINT:export>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_external_ip_extip_index_external_port_lsb_external_port_export(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *extip_index_ptr,
    uint32_t  *export_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.extIpIdx = *extip_index_ptr;
	
	if(!osal_strcmp(TOKEN_STR(6), "external-port-lsb" )) {
		naptr.extPortLSB = *export_ptr;
	} else {
		naptr.extPortHSB = ((*export_ptr) >> 8) & 0xFF;
		naptr.extPortLSB = (*export_ptr) & 0xFF;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_external_ip_extip_index_external_port_lsb_external_port_export */

/*
 * l34 set naptr <UINT:index> naptr-priority state ( disable | enable )  
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_naptr_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	 if('d' == TOKEN_CHAR(6, 0))
   	 {
   	 	state = DISABLED;
   	 }else{
		state = ENABLED;
	 }
	 naptr.priValid = state;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_naptr_priority_state_disable_enable */

/*
 * l34 set naptr <UINT:index> naptr-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_naptr_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.priId = *priority_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_naptr_priority_priority */

/*
 * l34 set naptr <UINT:index> remote-hash-type ( none | remote_ip | remote_ip_remote_port )
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_remote_hash_type_none_remote_ip_remote_ip_remote_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	 if('n' == TOKEN_CHAR(5, 0))
   	 {
   	 	naptr.valid = 1;
   	 }else 
   	 if('_' == TOKEN_CHAR(5, 9))
   	 {
   	 	naptr.valid = 2;
	 }else
	 {
	 	naptr.valid = 3;
	 }
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_remote_hash_type_none_remote_ip_remote_ip_remote_port */

/*
 * l34 set naptr <UINT:index> hash-value <UINT:value>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_hash_value_value(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.remHash = *value_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_hash_value_value */

/*
 * l34 set naptr <UINT:index> state disable
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_state_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.valid = 0;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_state_disable */

/*
 * l34 del naptr <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_naptr_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_naptr_index */

/*
 * l34 set port ( <PORT_LIST:ports> | all ) netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_set_port_ports_all_netif_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_MAC ;
	rtk_port_t port;
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_set( type,port,*index_ptr),ret);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_port_ports_all_netif_index */

/*
 * l34 get port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_get_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_MAC ;
	rtk_port_t port;
	uint32 wanId;
	
	DIAG_UTIL_PARAM_CHK();
	diag_util_mprintf("LAN interface Multilayer-Decision-Base Control, Port Base Set!\n");

	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_get( type,port,&wanId),ret);
		diag_util_mprintf("Port %d Map to WAN %d\n",port,wanId);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_port_ports_all */

/*
 * l34 set ext ( <PORT_LIST:ext> | all ) netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_set_ext_ext_all_netif_index(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *index_ptr)
{
    	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_EXTENSION;
	rtk_port_t port;
	
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 3), ret);
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_set( type,port,*index_ptr),ret);
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_ext_ext_all_netif_index */

/*
 * l34 get ext ( <PORT_LIST:ext> | all )
 */
cparser_result_t
cparser_cmd_l34_get_ext_ext_all(
    cparser_context_t *context,
    char * *ext_ptr)
{
    	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_EXTENSION ;
	rtk_port_t port;
	uint32 wanId;
	
	DIAG_UTIL_PARAM_CHK();
	diag_util_mprintf("LAN interface Multilayer-Decision-Base Control, Port Base Set!\n");
	
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 3), ret);
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_get( type,port,&wanId),ret);
		diag_util_mprintf("Ext Port %d Map to WAN %d\n",port,wanId);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ext_ext_all */

/*
 * l34 set l4-fragment action ( trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_set_l4_fragment_action_trap_to_cpu_forward(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_FRAG2CPU_STATE ;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu")){
		state = ENABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"forward"))
	{
		state = DISABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l4_fragment_action_trap_to_cpu_forward */

/*
 * l34 get l4-fragment
 */
cparser_result_t
cparser_cmd_l34_get_l4_fragment(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_FRAG2CPU_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == ENABLED){
		    diag_util_mprintf("L4 Fragement Action: Trap to CPU\n");
	}else{
    		    diag_util_mprintf("L4 Fragement Action: Forward\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_fragment */

/*
 * l34 set l3-checksum-error action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l34_set_l3_checksum_error_action_forward_drop(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L3CHKSERRALLOW_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		state = DISABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"forward"))
	{
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l3_checksum_error_action_forward_drop */

/*
 * l34 get l3-checksum-error
 */
cparser_result_t
cparser_cmd_l34_get_l3_checksum_error(
    cparser_context_t *context)
{
   	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L3CHKSERRALLOW_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("L3 Checksum Error Action: Drop\n");
	}else{
    		    diag_util_mprintf("L3 Checksum Error Action: Forward\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l3_checksum_error */

/*
 * l34 set l4-checksum-error action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l34_set_l4_checksum_error_action_forward_drop(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L4CHKSERRALLOW_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		state = DISABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"forward"))
	{
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l4_checksum_error_action_forward_drop */

/*
 * l34 get l4-checksum-error
 */
cparser_result_t
cparser_cmd_l34_get_l4_checksum_error(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L4CHKSERRALLOW_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("L4 Checksum Error Action: Drop\n");
	}else{
    		    diag_util_mprintf("L4 Checksum Error Action: Forward\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_checksum_error */

/*
 * l34 set ttl-minus state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_ttl_minus_state_enable_disable(
    cparser_context_t *context)
{
       int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_TTLMINUS_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"disable")){
		state = DISABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"enable"))
	{
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_ttl_minus_state_enable_disable */

/*
 * l34 get ttl-minus state
 */
cparser_result_t
cparser_cmd_l34_get_ttl_minus_state(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_TTLMINUS_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("TTL Minus State: Disable\n");
	}else{
    		    diag_util_mprintf("TTL Minus State: Enable\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ttl_minus_state */

/*
 * l34 set interface-decision-mode ( vlan-based | port-based | mac-based )
 */
cparser_result_t
cparser_cmd_l34_set_interface_decision_mode_vlan_based_port_based_mac_based(
    cparser_context_t *context)
{
   	int32 ret;
	rtk_l34_lookupMode_t mode;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(3),"vlan-based")){
		mode = L34_LOOKUP_VLAN_BASE;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"port-based")){
		mode = L34_LOOKUP_PORT_BASE;
	}else{
		mode = L34_LOOKUP_MAC_BASE;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_lookupMode_set(mode),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_interface_decision_mode_vlan_based_port_based_mac_based */

/*
 * l34 get interface-decision-mode
 */
cparser_result_t
cparser_cmd_l34_get_interface_decision_mode(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_lookupMode_t mode;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_lookupMode_get(&mode),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	switch(mode){
	case L34_LOOKUP_VLAN_BASE:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: VLAN Based\n");
	break;
	case L34_LOOKUP_PORT_BASE:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: PORT Based\n");
	break;
	case L34_LOOKUP_MAC_BASE:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: MAC Based\n");
	break;
	default:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: Not support\n");
	break;
	}
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_interface_decision_mode */

/*
 * l34 set nat-attack action ( drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l34_set_nat_attack_action_drop_trap_to_cpu(
    cparser_context_t *context)
{
     	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_NAT2LOG_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		state = DISABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu"))
	{
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nat_attack_action_drop_trap_to_cpu */

/*
 * l34 get nat-attack
 */
cparser_result_t
cparser_cmd_l34_get_nat_attack(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_NAT2LOG_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state ==DISABLED){
		    diag_util_mprintf("NAT Attack Log: Disable\n");
	}else{
    		    diag_util_mprintf("NAT Attack Log: Enable\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nat_attack */

/*
 * l34 set wan-route action ( drop | trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_set_wan_route_action_drop_trap_to_cpu_forward(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_wanRouteMode_t act;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		act = L34_WANROUTE_DROP;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu")){
		act = L34_WANROUTE_FORWARD2CPU;
	}else{
		act = L34_WANROUTE_FORWARD;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_wanRoutMode_set(act),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_route_action_drop_trap_to_cpu_forward */

/*
 * l34 get wan-route
 */
cparser_result_t
cparser_cmd_l34_get_wan_route(
    cparser_context_t *context)
{
	int32 ret;
	rtk_l34_wanRouteMode_t act;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_wanRoutMode_get(&act),ret);

	diag_util_mprintf("L34 Global Setting:\n");

	switch(act){
	case L34_WANROUTE_FORWARD:
		 diag_util_mprintf("WAN Route Action: Forward\n");
	break;
	case L34_WANROUTE_DROP:
		diag_util_mprintf("WAN Route Action: DROP\n");
	break;
	case L34_WANROUTE_FORWARD2CPU:
		diag_util_mprintf("WAN Route Action: Forward to CPU\n");
	break;
	default:
		diag_util_mprintf("WAN Route Action: Not Supported\n");
	break;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_wan_route */

/*
 * l34 set route-mode ( l3 | l3-l4 | disable )
 */
cparser_result_t
cparser_cmd_l34_set_route_mode_l3_l3_l4_disable(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(3),"l3")){
		state = ENABLED;
		type = L34_GLOBAL_L3NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
		state = DISABLED;
		type = L34_GLOBAL_L4NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	}else
	if(!osal_strcmp(TOKEN_STR(3),"l3-l4")){
		state = ENABLED;
		type = L34_GLOBAL_L4NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
		state = ENABLED;
		type = L34_GLOBAL_L3NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	}else{
		state = DISABLED;
		type = L34_GLOBAL_L3NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
		state = DISABLED;
		type = L34_GLOBAL_L4NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_route_mode_l3_l3_l4_disable */

/*
 * l34 get route-mode
 */
cparser_result_t
cparser_cmd_l34_get_route_mode(
    cparser_context_t *context)
{
    int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type;
	
	DIAG_UTIL_PARAM_CHK();
	
	type = L34_GLOBAL_L34_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting: %s\n", (state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

	type = L34_GLOBAL_L3NAT_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L3 NAT STATE: %s\n", (state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
	
	type = L34_GLOBAL_L4NAT_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L4 NAT STATE: %s\n", (state == ENABLED) ? DIAG_STR_ENABLE : DIAG_STR_DISABLE);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_route_mode */

/*
 * l34 get pppoe-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	uint32 i;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_pppTrfIndicator_get(*index_ptr, &state),ret);
	diag_util_mprintf("PPP Id: %d, Indicator: %d\n",*index_ptr, state);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_traffic_indicator */

/*
 * l34 get pppoe-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_traffic_indicator(
    cparser_context_t *context)
{
	int32 ret;
	uint32 i;
	rtk_l34_ppp_trf_all_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	/*Get all*/
	DIAG_UTIL_ERR_CHK(rtk_l34_pppTrfIndicator_get_all(&state),ret);
	for(i=0;i<HAL_L34_PPPOE_ENTRY_MAX();i++)
	{
		diag_util_mprintf("PPP Id: %d, Indicator: %d\n",i, (state.trf_state[0] & (1 << i)) ? 1 : 0);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_traffic_indicator */

/*
 * l34 get arp-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_arpTrfIndicator_get(*index_ptr, &state),ret);
	diag_util_mprintf("ARP Id: %d, Indicator: %d\n",*index_ptr,state);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator_index_index */

/*
 * l34 get arp-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator(
    cparser_context_t *context)
{
    int32 ret;
	uint32 i;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_ARP_ENTRY_MAX();i++)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_arpTrfIndicator_get(i, &state),ret);
		diag_util_mprintf("ARP Id: %d, Indicator: %d\n",i,state);
	}
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator */

/*
 * l34 reset arp-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_reset_arp_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
	rtk_l34_arp_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_ARPTRF_TABLE0;
	}else
	//if(!osal_strcmp(TOKEN_STR(3),"table1"))
	{
		table = RTK_L34_ARPTRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwArpTrfWrkTbl_Clear(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_arp_traffic_indicator_table0_table1 */

/*
 * l34 select arp-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_select_arp_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    	rtk_l34_arp_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_ARPTRF_TABLE0;
	}else
	//if(!osal_strcmp(TOKEN_STR(3),"table1"))
	{
		table = RTK_L34_ARPTRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwArpTrfWrkTbl_set(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_select_arp_traffic_indicator_table0_table1 */

/*
 * l34 get l4-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_l4_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();


	DIAG_UTIL_ERR_CHK(rtk_l34_naptTrfIndicator_get(*index_ptr, &state),ret);
	diag_util_mprintf("NAPT Id: %d, Indicator: %d\n",*index_ptr,state);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_traffic_indicator_index_index */

/*
 * l34 get l4-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_l4_traffic_indicator(
    cparser_context_t *context)
{
    int32 ret;
	uint32 i;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_NAPT_ENTRY_MAX();i++)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_naptTrfIndicator_get(i, &state),ret);
		diag_util_mprintf("NAPT Id: %d, Indicator: %d\n",i,state);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_traffic_indicator */

/*
 * l34 reset l4-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_reset_l4_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    	rtk_l34_l4_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_L4TRF_TABLE0;
	}else
	//if(!osal_strcmp(TOKEN_STR(3),"table1"))
	{
		table = RTK_L34_L4TRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwL4TrfWrkTbl_Clear(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_l4_traffic_indicator_table0_table1 */

/*
 * l34 select l4-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_select_l4_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    	rtk_l34_l4_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_L4TRF_TABLE0;
	}else
	//if(!osal_strcmp(TOKEN_STR(3),"table1"))
	{
		table = RTK_L34_L4TRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwL4TrfWrkTbl_set(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_select_l4_traffic_indicator_table0_table1 */

/*
 * debug l34 set hsb l2-bridge <UINT:l2bridge> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l2_bridge_l2bridge(
    cparser_context_t *context,
    uint32_t  *l2bridge_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l2_bridge_l2bridge */

/*
 * debug l34 set hsb ip-fragments <UINT:is_fragments> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ip_fragments_is_fragments(
    cparser_context_t *context,
    uint32_t  *is_fragments_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ip_fragments_is_fragments */

/*
 * debug l34 set hsb ip-more-fragments <UINT:is_more> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ip_more_fragments_is_more(
    cparser_context_t *context,
    uint32_t  *is_more_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ip_more_fragments_is_more */

/*
 * debug l34 set hsb l4-checksum-ok <UINT:is_ok> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l4_checksum_ok_is_ok(
    cparser_context_t *context,
    uint32_t  *is_ok_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l4_checksum_ok_is_ok */

/*
 * debug l34 set hsb l3-checksum-ok <UINT:is_ok> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l3_checksum_ok_is_ok(
    cparser_context_t *context,
    uint32_t  *is_ok_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l3_checksum_ok_is_ok */

/*
 * debug l34 set hsb direct-tx <UINT:is_direct_tx> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_direct_tx_is_direct_tx(
    cparser_context_t *context,
    uint32_t  *is_direct_tx_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_direct_tx_is_direct_tx */

/*
 * debug l34 set hsb udp-no-chksum <UINT:udp_no_chk> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_udp_no_chksum_udp_no_chk(
    cparser_context_t *context,
    uint32_t  *udp_no_chk_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_udp_no_chksum_udp_no_chk */

/*
 * debug l34 set hsb parse-fail <UINT:parse_fail> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_parse_fail_parse_fail(
    cparser_context_t *context,
    uint32_t  *parse_fail_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_parse_fail_parse_fail */

/*
 * debug l34 set hsb pppoe-if <UINT:pppoe_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pppoe_if_pppoe_if(
    cparser_context_t *context,
    uint32_t  *pppoe_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pppoe_if_pppoe_if */

/*
 * debug l34 set hsb svlan-if <UINT:svlan_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_svlan_if_svlan_if(
    cparser_context_t *context,
    uint32_t  *svlan_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_svlan_if_svlan_if */

/*
 * debug l34 set hsb ttls <UINT:ttls> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ttls_ttls(
    cparser_context_t *context,
    uint32_t  *ttls_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ttls_ttls */

/*
 * debug l34 set hsb pkt-type <UINT:pkt_type> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pkt_type_pkt_type(
    cparser_context_t *context,
    uint32_t  *pkt_type_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pkt_type_pkt_type */

/*
 * debug l34 set hsb tcp-flag <UINT:tcp_flag> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_tcp_flag_tcp_flag(
    cparser_context_t *context,
    uint32_t  *tcp_flag_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_tcp_flag_tcp_flag */

/*
 * debug l34 set hsb cvlan-if <UINT:cvlan_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_cvlan_if_cvlan_if(
    cparser_context_t *context,
    uint32_t  *cvlan_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_cvlan_if_cvlan_if */

/*
 * debug l34 set hsb source-port <UINT:spa> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_source_port_spa(
    cparser_context_t *context,
    uint32_t  *spa_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_source_port_spa */

/*
 * debug l34 set hsb cvid <UINT:cvid> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_cvid_cvid(
    cparser_context_t *context,
    uint32_t  *cvid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_cvid_cvid */

/*
 * debug l34 set hsb packet-length <UINT:length> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_packet_length_length(
    cparser_context_t *context,
    uint32_t  *length_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_packet_length_length */

/*
 * debug l34 set hsb dport <UINT:dport> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dport_dport(
    cparser_context_t *context,
    uint32_t  *dport_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dport_dport */

/*
 * debug l34 set hsb pppoe-id <UINT:pppoe> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pppoe_id_pppoe(
    cparser_context_t *context,
    uint32_t  *pppoe_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pppoe_id_pppoe */

/*
 * debug l34 set hsb dip <IPV4ADDR:ip> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dip_ip */

/*
 * debug l34 set hsb sip <IPV4ADDR:ip> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_sip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_sip_ip */

/*
 * debug l34 set hsb sport <UINT:sport> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_sport_sport(
    cparser_context_t *context,
    uint32_t  *sport_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_sport_sport */

/*
 * debug l34 set hsb dmac <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dmac_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dmac_mac */

/*
 * debug l34 get hsb
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsb(
    cparser_context_t *context)
{
     int32 ret = RT_ERR_FAILED;
    rtk_l34_hsb_t hsb_data;
    uint32  ip_tmp_buf[UTIL_IPV6_TMP_BUFFER_LENGTH];
    char tmpStr[128]="";
    uint32_t spa_data;
    uint32 isIpv6 = 0;

    osal_memset(&hsb_data,0x0,sizeof(rtk_l34_hsb_t));
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /*call hsb API*/
    if((ret = rtk_l34_hsbData_get(&hsb_data)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
#if defined(CONFIG_SDK_RTL9602C)
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
	diag_util_printf("\n DA_trans            :%-4d",hsb_data.da_traslate);
	diag_util_printf("\n CFI_1               :%-4d",hsb_data.cfi_1);
	diag_util_printf("\n SA_idx              :%-4d",hsb_data.sa_idx);
	diag_util_printf("\n SA_idx_Valid        :%-4d",hsb_data.sa_idx_valid);
	diag_util_printf("\n Policy_NH_idx       :%-4d",hsb_data.policy_nh_idx);
	diag_util_printf("\n isPolicyRoute       :%-4d",hsb_data.is_policy_route);
    }
#endif
	diag_util_printf("\n isFromWan           :%-4d",hsb_data.isFromWan);
    diag_util_printf("\n l2bridbe            :%-4d",hsb_data.l2bridge);
    diag_util_printf("\n ipfrags             :%-4d",hsb_data.ipfrag_s);
    diag_util_printf("\n ipmf                :%-4d",hsb_data.ipmf);
    diag_util_printf("\n l4chk               :%-4d",hsb_data.l4_chksum_ok);
    diag_util_printf("\n l3chk               :%-4d",hsb_data.l3_chksum_ok);
    diag_util_printf("\n direct_tx           :%-4d",hsb_data.cpu_direct_tx);
    diag_util_printf("\n udp_no_chk          :%-4d",hsb_data.udp_no_chksum);
    diag_util_printf("\n parse_fail          :%-4d",hsb_data.parse_fail);
    diag_util_printf("\n pppoe_if            :%-4d",hsb_data.pppoe_if);
    diag_util_printf("\n svlan_if            :%-4d",hsb_data.svlan_if);
    diag_util_printf("\n ttls                :%-4d",hsb_data.ttls);
    diag_util_printf("\n cvlan_if            :%-4d",hsb_data.cvlan_if);
    diag_util_printf("\n SPA                 :%-4d",hsb_data.spa);   
    diag_util_printf("\n cvid                :%-4d",hsb_data.cvid);
    diag_util_printf("\n len                 :%-4d",hsb_data.len);
    diag_util_printf("\n dport_l4chksum      :%-4d",hsb_data.dport_l4chksum);
    diag_util_printf("\n pppoe_id            :%-4d",hsb_data.pppoe_id);
    diag_util_printf("\n sport_icmpid_chksum :%-4d",hsb_data.sport_icmpid_chksum);
    diag_util_printf("\n tcp_flag            :0x%-4x",hsb_data.tcp_flag);
#if defined(CONFIG_SDK_RTL9602C)
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {	
    diag_util_printf("\n IP type             :%s", (hsb_data.ip6_type_ext == 1) ? DIAG_STR_IPV6 : DIAG_STR_IPV4);
    }
#endif	
    diag_util_printf("\n type                :%s", diagStr_l34HsbType[hsb_data.type]);

#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
    if((APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE) ||
       (APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE))
    {
    	if(hsb_data.type == L34_HSB_IPV6)
			isIpv6 = 1;
    }
#endif
#if defined(CONFIG_SDK_RTL9602C)
    if(RTL9602C_CHIP_ID == DIAG_UTIL_CHIP_TYPE)
    {
    	if(hsb_data.ip6_type_ext == 1)
			isIpv6 = 1;
    }
#endif

	if(isIpv6)
	{
	    /*DIP*/    
	    diag_util_ipv62str((uint8*)ip_tmp_buf, (uint8*)hsb_data.dip.ipv6_addr);
	    diag_util_printf("\n DIP                 :%s",(uint8*)ip_tmp_buf);
	    /*SIP*/
	    diag_util_ipv62str((uint8*)ip_tmp_buf, (uint8*)hsb_data.sip.ipv6_addr);
	    diag_util_printf("\n SIP                 :%s",(uint8*)ip_tmp_buf);
	} else {
	    /*DIP*/
	    diag_util_printf("\n DIP                 :%d.%d.%d.%d",
			hsb_data.dip.ipv6_addr[12], hsb_data.dip.ipv6_addr[13],
			hsb_data.dip.ipv6_addr[14], hsb_data.dip.ipv6_addr[15]);
	    /*SIP*/
	    diag_util_printf("\n SIP                 :%d.%d.%d.%d",
			hsb_data.sip.ipv6_addr[12], hsb_data.sip.ipv6_addr[13],
			hsb_data.sip.ipv6_addr[14], hsb_data.sip.ipv6_addr[15]);
	}
	
    /*DMAC*/
    diag_util_mac2str((uint8*)tmpStr,hsb_data.dmac.octet);
    diag_util_printf("\n DMAC                :%s \n",tmpStr);   
  
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsb */

/*
 * debug l34 get hsa
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsa(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_l34_hsa_t hsa_data;
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    uint32_t spa_data;

    osal_memset(&hsa_data,0x0,sizeof(rtk_l34_hsa_t));
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /*call API*/
    if((ret = rtk_l34_hsaData_get(&hsa_data)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    
    
    diag_util_printf("\n l34trans            :%-4d",hsa_data.l34trans);    
    diag_util_printf("\n l2trans             :%-4d",hsa_data.l2trans);    
    diag_util_printf("\n interVlan_if        :%-4d",hsa_data.interVlanIf);    
    diag_util_printf("\n l4_pri_valid        :%-4d",hsa_data.l4_pri_valid);    
    diag_util_printf("\n frag                :%-4d",hsa_data.frag);    
    diag_util_printf("\n pppoe_if            :%-4d",hsa_data.pppoe_if);    
    diag_util_printf("\n action              :%-4d",hsa_data.action);    
    diag_util_printf("\n pppid_idx           :%-4d",hsa_data.pppid_idx);    
    diag_util_printf("\n difid               :%-4d",hsa_data.difid);    
    diag_util_printf("\n l4_pri_sel          :%-4d",hsa_data.l4_pri_sel);    
    diag_util_printf("\n dvid                :%-4d",hsa_data.dvid);    
    diag_util_printf("\n reason              :%-4d",hsa_data.reason);
    diag_util_printf("\n nexthop_mac_idx     :%-4d",hsa_data.nexthop_mac_idx);    
    diag_util_printf("\n l4_chksum           :0x%-6x",hsa_data.l4_chksum);    
    diag_util_printf("\n l3_chksum           :0x%-4x",hsa_data.l3_chksum);    
    diag_util_printf("\n port                :%-4d",hsa_data.port);    
    /*IP address*/
    diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)hsa_data.ip);
    diag_util_printf("\n IP                  :%s\n",(uint8*)ip_tmp_buf);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsa */

/*
 * debug l34 set hsba log-mode <UINT:mode>
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsba_log_mode_mode(
    cparser_context_t *context,
    uint32_t  *mode_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_PARAM_RANGE_CHK((*mode_ptr > 5),RT_ERR_INPUT);
    
    /*call API*/
    if((ret = rtk_l34_hsabMode_set(*mode_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsba_log_mode_mode */

/*
 * debug l34 get hsba log-mode 
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsba_log_mode(
    cparser_context_t *context)
{
     int32 ret = RT_ERR_FAILED;
    uint32_t  mode;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

   
    /*call API*/
    if((ret = rtk_l34_hsabMode_get(&mode)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    
    diag_util_printf(" hsba log mode             :%d\n",mode);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsba_log_mode */


/*
 * l34 get routing6  
 */
cparser_result_t
cparser_cmd_l34_get_routing6(
    cparser_context_t *context)
{
	uint32 index;
	int32   ret = RT_ERR_FAILED;
    	rtk_ipv6Routing_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_route6_table_header_display();

	for(index = 0; index < HAL_L34_IPV6_ROUTING_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_ipv6Routing_entry_t));
	    if((ret = rtk_l34_ipv6RoutingTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_route6_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_routing6 */

/*
 * l34 get routing6 <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_routing6_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_ipv6Routing_entry_t));
	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_route6_table_header_display();
	_cparser_cmd_l34_route6_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_routing6_index */

/*
 * l34 get neighbor  
 */
cparser_result_t
cparser_cmd_l34_get_neighbor(
    cparser_context_t *context)
{
	uint32 index;
	int32   ret = RT_ERR_FAILED;
	rtk_ipv6Neighbor_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_neigh_table_header_display();

	for(index = 0; index < HAL_L34_IPV6_NEIGHBOR_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_ipv6Neighbor_entry_t));
	    if((ret = rtk_l34_ipv6NeighborTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    if(entry.valid)
	    	_cparser_cmd_l34_neigh_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_neighbor */

/*
 * l34 get neighbor <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_neighbor_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_ipv6Neighbor_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_ipv6Neighbor_entry_t));
	if((ret = rtk_l34_ipv6NeighborTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_neigh_table_header_display();
	_cparser_cmd_l34_neigh_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_neighbor_index */

/*
 * l34 get binding  
 */
cparser_result_t
cparser_cmd_l34_get_binding(
    cparser_context_t *context)
{

    uint32 index;
	int32   ret = RT_ERR_FAILED;
	rtk_binding_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_binding_table_header_display();

	for(index = 0; index < HAL_L34_BINDING_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	    if((ret = rtk_l34_bindingTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_binding_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_binding */

/*
 * l34 get binding <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_binding_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_binding_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	if((ret = rtk_l34_bindingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_binding_table_header_display();
	_cparser_cmd_l34_binding_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_binding_index */

/*
 * l34 get wan-type  
 */
cparser_result_t
cparser_cmd_l34_get_wan_type(
    cparser_context_t *context)
{
    uint32 index;
	int32   ret = RT_ERR_FAILED;
	rtk_wanType_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_wanType_table_header_display();

	for(index = 0; index < HAL_L34_WAN_TYPE_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	    if((ret = rtk_l34_wanTypeTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_wanType_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_wan_type */

/*
 * l34 get wan-type <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_wan_type_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_wanType_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	if((ret = rtk_l34_wanTypeTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_wanType_table_header_display();
	_cparser_cmd_l34_wanType_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_wan_type_index */


/*
 * l34 set routing6 <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_state_disable_enable */

/*
 * l34 set routing6 <UINT:index> ip6 <IPV6ADDR:ip6> prefix <UINT:prefix>
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_ip6_ip6_prefix_prefix(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ip6_ptr,
    uint32_t  *prefix_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    

	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipv6Addr.ipv6_addr[0], *ip6_ptr), ret);
	entry.ipv6PrefixLen = *prefix_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing6_index_ip6_ip6_prefix_prefix */


/*
 * l34 set routing6 <UINT:index> nexthop <UINT:nexthop>
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_nexthop_nexthop(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nexthop_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	
	entry.nhOrIfidIdx = *nexthop_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing6_index_nexthop_nexthop */

/*
 * l34 set routing6 <UINT:index> type local-route destination-netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_type_local_route_destination_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	
	entry.type = L34_IPV6_ROUTE_TYPE_LOCAL;
	entry.nhOrIfidIdx = *netif_index_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing6_index_type_local_route_destination_netif_netif_index */

/*
 * l34 set routing6 <UINT:index> type global-route next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_type_global_route_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	
	entry.type = L34_IPV6_ROUTE_TYPE_GLOBAL;
	entry.nhOrIfidIdx = *nh_index_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing6_index_type_global_route_next_hop_table_nh_index */

/*
 * l34 set routing6 <UINT:index> type ( drop | trap )
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_type_drop_trap(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	if(!osal_strcmp(TOKEN_STR(5),"drop")){
		entry.type = L34_IPV6_ROUTE_TYPE_DROP;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"trap")){
		entry.type = L34_IPV6_ROUTE_TYPE_TRAP;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing6_index_type_drop_trap */


/*
 * l34 set routing6 <UINT:index> rt2wan state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_routing6_index_rt2wan_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0x0,sizeof(rtk_ipv6Routing_entry_t));

	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.rt2waninf = 1;
	} else {
		entry.rt2waninf = 0;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_ipv6RoutingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing6_index_type_drop_trap */




/*
 * l34 set neighbor <UINT:index> ifId <UINT:ifIdValue>
 */
cparser_result_t
cparser_cmd_l34_set_neighbor_index_ifId_ifIdValue(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ifIdValue_ptr)
{
    	rtk_ipv6Neighbor_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_ipv6Neighbor_entry_t));
	if((ret = rtk_l34_ipv6NeighborTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	entry.ipv6Ifid = *ifIdValue_ptr;
	if((ret = rtk_l34_ipv6NeighborTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_neighbor_index_ifid_ifidvalue */


/*
 * l34 set binding <UINT:index>  wan-type <UINT:wanTypeId>
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_wan_type_wanTypeId(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *wanTypeId_ptr)
{
	int32 ret;
	rtk_binding_entry_t entry;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	entry.wanTypeIdx = *wanTypeId_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_wan_type_wantypeid */

/*
 * l34 set binding <UINT:index> protocol ( all | ipv4-other | ipv6-other | other-only  )
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_protocol_all_ipv4_other_ipv6_other_other_only(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_binding_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	if(!osal_strcmp(TOKEN_STR(5),"all")){
		entry.bindProto = L34_BIND_PROTO_ALL;
    	}else
    	if(!osal_strcmp(TOKEN_STR(5),"ipv4-other"))
	{
		entry.bindProto = L34_BIND_PROTO_NOT_IPV6;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"ipv6-other"))
	{
		entry.bindProto = L34_BIND_PROTO_NOT_IPV4;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"other-only"))
	{
		entry.bindProto = L34_BIND_PROTO_NOT_IPV4_IPV6;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_protocol_all_ipv4_other_ipv6_other_other_only */

/*
 * l34 set binding <UINT:index> vid <UINT:vlanId>
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_vid_vlanId(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vlanId_ptr)
{
    	int32 ret;
	rtk_binding_entry_t entry;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	entry.vidLan = *vlanId_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_vid_vlanid */

/*
 * l34 set binding <UINT:index> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_port_ports_all(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
	int32 ret;
	diag_portlist_t portlist;
	rtk_binding_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	RTK_PORTMASK_FROM_UINT_PORTMASK((&(entry.portMask)), &(portlist.portmask.bits[0]));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_port_ports_all */

/*
 * l34 set binding <UINT:index> ext-port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_ext_port_ports_all(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    	int32 ret;
	diag_portlist_t portlist;
	rtk_binding_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	RTK_PORTMASK_FROM_UINT_PORTMASK((&(entry.extPortMask)), &(portlist.portmask.bits[0]));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);
	return CPARSER_OK;

}    /* end of cparser_cmd_l34_set_binding_index_ext_port_ports_all */

/*
 * l34 set wan-type <UINT:index> type ( l2-bridge | l3-routing | l34-nat-route | l34-customize )
 */
cparser_result_t
cparser_cmd_l34_set_wan_type_index_type_l2_bridge_l3_routing_l34_nat_route_l34_customize(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_wanType_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_get(*index_ptr, &entry),ret);

	if(!osal_strcmp(TOKEN_STR(5),"l2-bridge")){
		entry.wanType = L34_WAN_TYPE_L2_BRIDGE;
    	}else
    	if(!osal_strcmp(TOKEN_STR(5),"l3-routing"))
	{
		entry.wanType = L34_WAN_TYPE_L3_ROUTE;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"l34-nat-route"))
	{
		entry.wanType = L34_WAN_TYPE_L34NAT_ROUTE;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"l34-customize"))
	{
		entry.wanType = L34_WAN_TYPE_L34_CUSTOMIZED;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_type_index_type_l2_bridge_l3_routing_l34_nat_route_l34_customize */

/*
 * l34 set wan-type <UINT:index> next-hop <UINT:nexthopId>
 */
cparser_result_t
cparser_cmd_l34_set_wan_type_index_next_hop_nexthopId(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nexthopId_ptr)
{
    	int32 ret;
	rtk_wanType_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_get(*index_ptr, &entry),ret);
	entry.nhIdx = *nexthopId_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_type_index_next_hop_nexthopid */



/*
 * l34 set binding unmatch-type ( l2tol3 | l2tol34 ) act ( drop | trap | force-l2-bridge ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l2tol3_l2tol34_act_drop_trap_force_l2_bridge(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();


	if(!osal_strcmp(TOKEN_STR(4),"l2tol3")){
		type = L34_BIND_UNMATCHED_L2L3;
    	}else
    	//if(!osal_strcmp(TOKEN_STR(4),"l2tol34"))
	{
		type = L34_BIND_UNMATCHED_L2L34;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"drop")){
		act= L34_BIND_ACT_DROP;
    	}else
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    //if(!osal_strcmp(TOKEN_STR(6),"force-l2-bridge"))
	{
		act= L34_BIND_ACT_FORCE_L2BRIDGE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_unmatch_type_l2tol3_l2tol34_act_drop_trap_force_l2_bridge */

/*
 * l34 set binding unmatch-type ( l3tol2 | l34tol2 ) act ( drop | trap | permit-l2-bridge ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l3tol2_l34tol2_act_drop_trap_permit_l2_bridge(
    cparser_context_t *context)
{
   	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();


	if(!osal_strcmp(TOKEN_STR(4),"l3tol2")){
		type = L34_BIND_UNMATCHED_L3L2;
    }
    else
    	//if(!osal_strcmp(TOKEN_STR(4),"l34tol2"))
	{
		type = L34_BIND_UNMATCHED_L34L2;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"drop")){
		act= L34_BIND_ACT_DROP;
    	}else
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	//if(!osal_strcmp(TOKEN_STR(6),"permit-l2-bridge"))
	{
		act= L34_BIND_ACT_PERMIT_L2BRIDGE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}  


/*
 * l34 set binding unmatch-type  l3tol34 act ( trap | force-l4-lookup ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l3tol34_act_trap_force_l4_lookup(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_UNMATCHED_L3L34;
	
	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	//if(!osal_strcmp(TOKEN_STR(6),"force-l4-lookup"))
	{
		act= L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_unmatch_type_l3tol34_act_trap_force_l4_lookup */

/*
 * l34 set binding unmatch-type  ( l34tol3 | l3tol3 ) act ( trap | force-l3 ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l34tol3_l3tol3_act_trap_force_l3(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();


	if(!osal_strcmp(TOKEN_STR(4),"l34tol3")){
		type = L34_BIND_UNMATCHED_L34L3;
	}else
    	//if(!osal_strcmp(TOKEN_STR(4),"l3tol3"))
	{
		type = L34_BIND_UNMATCHED_L3L3;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else //if(!osal_strcmp(TOKEN_STR(6),"force-l3"))
	{
		if(type == L34_BIND_UNMATCHED_L34L3)
			act= L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
		else
			act= L34_BIND_ACT_FORCE_BINDL3;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_unmatch_type_l34tol3_l3tol3_act_trap_force_l3 */

/*
 * l34 set binding customized  l2 act ( trap | drop | permit-l2-bridge ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_customized_l2_act_trap_drop_permit_l2_bridge(
    cparser_context_t *context)
{
   	 int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_CUSTOMIZED_L2;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"drop"))
	{
		act= L34_BIND_ACT_DROP;
	}else
    //	if(!osal_strcmp(TOKEN_STR(6),"permit-l2-bridge"))
	{
		act= L34_BIND_ACT_PERMIT_L2BRIDGE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_customized_l2_act_trap_drop_permit_l2_bridge */

/*
 * l34 set binding customized  l3 act ( trap | drop | force-l3 | force-L4-lookup ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_customized_l3_act_trap_drop_force_l3_force_L4_lookup(
    cparser_context_t *context)
{
     	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_CUSTOMIZED_L3;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"drop"))
	{
		act= L34_BIND_ACT_DROP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l3"))
	{
		act= L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
	}else
    	//if(!osal_strcmp(TOKEN_STR(6),"force-L4-lookup"))
	{
		act= L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_customized_l3_act_trap_drop_force_l3_force_l4_lookup */

/*
 * l34 set binding customized  l34 act ( trap | drop | force-l3 | normal ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_customized_l34_act_trap_drop_force_l3_normal(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_CUSTOMIZED_L34;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"drop"))
	{
		act= L34_BIND_ACT_DROP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l3"))
	{
		act= L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
	}else
    	//if(!osal_strcmp(TOKEN_STR(6),"normal"))
	{
		act= L34_BIND_ACT_NORMAL_LOOKUPL34;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_customized_l34_act_trap_drop_force_l3_normal */

/*
 * l34 get binding unmatch-type (  l2tol3 | l2tol34 | l3tol2 | l3tol34 | l34tol2 | l34tol3 | l3tol3 )
 */
cparser_result_t
cparser_cmd_l34_get_binding_unmatch_type_l2tol3_l2tol34_l3tol2_l3tol34_l34tol2_l34tol3_l3tol3(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	char tmp[128]="";
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	if(!osal_strcmp(TOKEN_STR(4),"l2tol3")){
		type = L34_BIND_UNMATCHED_L2L3;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l2tol34"))
	{
		type = L34_BIND_UNMATCHED_L2L34;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3tol2"))
	{
		type = L34_BIND_UNMATCHED_L3L2;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3tol34"))
	{
		type = L34_BIND_UNMATCHED_L3L34;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l34tol2"))
	{
		type = L34_BIND_UNMATCHED_L34L2;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l34tol3"))
	{
		type = L34_BIND_UNMATCHED_L34L3;
	}else
    //	if(!osal_strcmp(TOKEN_STR(4),"l3tol3"))
	{
		type = L34_BIND_UNMATCHED_L3L3;
	}


	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_get(type,&act),ret);
	_diag_util_bindingAct2Str(tmp,act);
	
	diag_util_mprintf("Action:\t%s\n",tmp);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_binding_unmatch_type_l2tol3_l2tol34_l3tol2_l3tol34_l34tol2_l34tol3_l3tol3 */


/*
 * l34 get binding customized (  l2 | l3 | l34 )
 */
cparser_result_t
cparser_cmd_l34_get_binding_customized_l2_l3_l34(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	char tmp[128]="";
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	if(!osal_strcmp(TOKEN_STR(4),"l2")){
		type = L34_BIND_CUSTOMIZED_L2;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3"))
	{
		type = L34_BIND_CUSTOMIZED_L3;
	}else
    //	if(!osal_strcmp(TOKEN_STR(4),"l34"))
	{
		type = L34_BIND_CUSTOMIZED_L34;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_get(type,&act),ret);
	_diag_util_bindingAct2Str(tmp,act);
	
	diag_util_mprintf("Action:\t%s\n",tmp);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_binding_customized_l2_l3_l34 */

/*
 * l34 set map port <UINT:portId> wan <UINT:wanId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_port_portId_wan_wanId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *portId_ptr,
    uint32_t  *wanId_ptr)
{
	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_PORT2WAN;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *portId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_port_portid_wan_wanid_state_enable_disable */

/*
 * l34 set map ext <UINT:extId> wan <UINT:wanId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_ext_extId_wan_wanId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *extId_ptr,
    uint32_t  *wanId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_EXT2WAN;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *extId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_ext_extid_wan_wanid_state_enable_disable */

/*
 * l34 set map wan <UINT:wanId> port <UINT:portId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_wan_wanId_port_portId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *wanId_ptr,
    uint32_t  *portId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_WAN2PORT;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *portId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_wan_wanid_port_portid_state_enable_disable */

/*
 * l34 set map wan <UINT:wanId> ext <UINT:extId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_wan_wanId_ext_extId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *wanId_ptr,
    uint32_t  *extId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_WAN2EXT;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *extId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_wan_wanid_ext_extid_state_enable_disable */


/*
 * l34 get map ( port-to-wan | ext-to-wan | wan-to-port | wan-to-ext ) index <UINT:tableId>
 */
cparser_result_t
cparser_cmd_l34_get_map_port_to_wan_ext_to_wan_wan_to_port_wan_to_ext_index_tableId(
    cparser_context_t *context,
    uint32_t  *tableId_ptr)
{
    int32 ret;
	rtk_l34_portWanMapType_t type;
	rtk_l34_portWanMap_entry_t entry;
	uint32 num=0,i;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"port-to-wan")){
		 type = L34_PORTMAP_TYPE_PORT2WAN;
		 num = HAL_L34_NETIF_ENTRY_MAX();
		 diag_util_mprintf("Port %d to WAN Mapping State:\n",*tableId_ptr);
	}else
	if(!osal_strcmp(TOKEN_STR(3),"ext-to-wan"))
	{
		type = L34_PORTMAP_TYPE_EXT2WAN;
		num = HAL_L34_NETIF_ENTRY_MAX();
		diag_util_mprintf("Ext-Port %d to WAN Mapping State:\n",*tableId_ptr);

	}else
	if(!osal_strcmp(TOKEN_STR(3),"wan-to-port"))
	{
		type = L34_PORTMAP_TYPE_WAN2PORT;
		num = HAL_GET_MAX_PORT() + 1;
		diag_util_mprintf("WAN %d to Port Mapping State:\n",*tableId_ptr);
	}else
	//if(!osal_strcmp(TOKEN_STR(3),"wan-to-ext"))
	{
		type = L34_PORTMAP_TYPE_WAN2EXT;
		num = HAL_GET_MAX_EXT_PORT() + 1;
		diag_util_mprintf("WAN %d to Ext-Port Mapping State:\n",*tableId_ptr);

	}
	
	if(type == L34_PORTMAP_TYPE_WAN2EXT || type == L34_PORTMAP_TYPE_WAN2PORT)
	{
		entry.wanIdx = *tableId_ptr;
		for(i=0;i<num;i++)
		{	
			entry.port = i;
			DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_get(type,&entry),ret);
			if(entry.act==L34_PORTMAP_ACT_PERMIT){
				diag_util_mprintf("PortId:%d\t,State:Enable\n",i);
			}else
			{
				diag_util_mprintf("PortId:%d\t,State:Disable\n",i);
			}
		}
	}else
	{
		entry.port = *tableId_ptr;
		for(i=0;i<num;i++)
		{	
			entry.wanIdx= i;
			DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_get(type,&entry),ret);
		if(entry.act==L34_PORTMAP_ACT_PERMIT){
			diag_util_mprintf("WANId:%d\t,State:Enable\n",i);
		}else
		{
			diag_util_mprintf("WANId:%d\t,State:Disable\n",i);
		}
		}
	}


	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_map_port_to_wan_ext_to_wan_wan_to_port_wan_to_ext_index_tableid */


/*
 * l34 set flow-route <UINT:index> type ( drop | trap | local-route | global-route )
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_type_drop_trap_local_route_global_route(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	rtk_l34_flowRoute_type_t type;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	if(!osal_strcmp(TOKEN_STR(5),"drop")){
		type = L34_FLOWRT_TYPE_DROP;
	}
	else if(!osal_strcmp(TOKEN_STR(5),"trap")){
		type = L34_FLOWRT_TYPE_CPU;
	}
	else if(!osal_strcmp(TOKEN_STR(5),"local-route")){
		type = L34_FLOWRT_TYPE_LOCAL;
	}
	else //if(!osal_strcmp(TOKEN_STR(5),"global-route"))
	{
		type = L34_FLOWRT_TYPE_GLOBAL;
	}
	entry.process = type;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);
	
	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> nexthop <UINT:nhIdx>
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_nexthop_nhIdx(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *nhIdx_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	entry.nexthopIdx = *nhIdx_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> tcp state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_tcp_state_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.isTcp = ENABLED;
	}else{
		entry.isTcp = DISABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> sport <UINT:sport> dport <UINT:dport>
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_sport_sport_dport_dport(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *sport_ptr,
	uint32_t  *dport_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	
	entry.sport = *sport_ptr;
	entry.dport = *dport_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> ipv4 sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_ipv4_sip_sip_dip_dip(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *sip_ptr,
	uint32_t  *dip_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	entry.isIpv4 = 1;
	entry.sip.ipv4 = *sip_ptr;
	entry.dip.ipv4 = *dip_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> ipv6 sip <IPV6ADDR:sip> dip <IPV6ADDR:dip>
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_ipv6_sip_sip_dip_dip(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	char * *sip_ptr,
	char * *dip_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);
	
	entry.isIpv4 = 0;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.sip.ipv6.ipv6_addr[0], *sip_ptr), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.dip.ipv6.ipv6_addr[0], *dip_ptr), ret);
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> pri-valid ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_pri_valid_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.pri_valid = ENABLED;
	}else{
		entry.pri_valid = DISABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> priority <UINT:priority> 
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_priority_priority(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *priority_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
	
	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	entry.priority = *priority_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 set flow-route <UINT:index> state ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_index_state_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);

	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid= ENABLED;
	}else{
		entry.valid = DISABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_set(&entry), ret);

	return CPARSER_OK;
}

/*
 * l34 get flow-route <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_flow_route_index(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_flowRoute_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);
	if(entry.isIpv4 == 1)
	{
		_cparser_cmd_l34_flow_route_table_header_display();
	} else {
		_cparser_cmd_l34_flow_route_table_ipv6_header_display();
	}
	
	_cparser_cmd_l34_flow_route_table_display(&entry);

	return CPARSER_OK;
}

/*
 * l34 get flow-route valid ( ipv4 | ipv6 )
 */
cparser_result_t
cparser_cmd_l34_get_flow_route_valid_ipv4_ipv6(
	cparser_context_t *context)
{
	rtk_l34_flowRoute_entry_t entry;
	uint8 isIpv4;
	uint32 index = 0;
	int32 ret;

	if(!osal_strcmp(TOKEN_STR(4),"ipv4")){
		_cparser_cmd_l34_flow_route_table_header_display();
		isIpv4 = 1;
	} else {
		_cparser_cmd_l34_flow_route_table_ipv6_header_display();
		isIpv4 = 0;
	}
	
	while(index < HAL_L34_FLOW_ROUTE_ENTRY_MAX())
	{
	    osal_memset(&entry, 0x0, sizeof(rtk_l34_flowRoute_entry_t));
		entry.index = index;
		DIAG_UTIL_ERR_CHK(rtk_l34_flowRouteTable_get(&entry), ret);
		if((entry.valid == ENABLED) && (isIpv4 == entry.isIpv4))
		{
			_cparser_cmd_l34_flow_route_table_display(&entry);
		}
		
		if(isIpv4 == 1)
			index++;
		else 
			index+=2;
	}
	
	return CPARSER_OK;
}


/*
 * l34 set flow-route state ( ipv4 | ipv6 ) ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_flow_route_state_ipv4_ipv6_enable_disable(
cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type;

	if(!osal_strcmp(TOKEN_STR(4),"ipv4")){
		type = L34_GLOBAL_V4FLOW_RT_STATE;
	}else{
		type = L34_GLOBAL_V6FLOW_RT_STATE;
	}
	
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		state = ENABLED;
	}else{
		state = DISABLED;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type, state),ret);
	
	return CPARSER_OK;
}

/*
 * l34 get flow-route state
 */
cparser_result_t
cparser_cmd_l34_get_flow_route_state(
cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  ipv4State, ipv6State;
	rtk_l34_globalStateType_t type;

	type = L34_GLOBAL_V4FLOW_RT_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type, &ipv4State),ret);

	type = L34_GLOBAL_V6FLOW_RT_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type, &ipv6State),ret);	

	diag_util_printf("Flow Route state: ");      
	diag_util_printf("IPv4 : %s\n",diagStr_enable[ipv4State]);  
	diag_util_printf("IPv6 : %s\n",diagStr_enable[ipv6State]);     
	
	return CPARSER_OK;
}

/*
 * l34 get flow-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_flow_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_flowTrfIndicator_get(*index_ptr, &state),ret);
	diag_util_mprintf("Flow Id: %d, Indicator: %d\n",*index_ptr,state);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_flow_traffic_indicator_index_index */

/*
 * l34 set ds-lite interface <UINT:index> aftr-ip6 <IPV6ADDR:aftr_ip6> b4-ip6 <IPV6ADDR:b4_ip6> 
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_interface_index_aftr_ip6_aftr_ip6_b4_ip6_b4_ip6(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	char * *aftr_ip6_ptr,
	char * *b4_ip6_ptr)
{
	rtk_l34_dsliteInf_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_dsliteInf_entry_t));
	
	DIAG_UTIL_PARAM_CHK();

	entry.index = *index_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_get(&entry), ret);

	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipAftr.ipv6_addr[0], *aftr_ip6_ptr), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipB4.ipv6_addr[0], *b4_ip6_ptr), ret);

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_set(&entry), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_interface_index_aftr_ip6_aftr_ip6_b4_ip6_b4_ip6 */

/*
 * l34 set ds-lite interface <UINT:index> flow-label <UINT:flow_label> hop-limit <UINT:hop_limit> tc-opt ( assign | copy-egress-tos ) { <UINT:tc_value> } 
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_interface_index_flow_label_flow_label_hop_limit_hop_limit_tc_opt_assign_copy_egress_tos_tc_value(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *flow_label_ptr,
	uint32_t  *hop_limit_ptr,
	uint32_t  *tc_value_ptr)
{
	rtk_l34_dsliteInf_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_dsliteInf_entry_t));
	
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_PARAM_RANGE_CHK((0xFFFFF < *flow_label_ptr), CPARSER_ERR_INVALID_PARAMS);
	DIAG_UTIL_PARAM_RANGE_CHK((0xFF < *hop_limit_ptr), CPARSER_ERR_INVALID_PARAMS);

	entry.index = *index_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_get(&entry), ret);

	if('a' == TOKEN_CHAR(10,0))
		entry.tcOpt = RTK_L34_DSLITE_TC_OPT_ASSIGN;
	else if ('c' == TOKEN_CHAR(10,0))
		entry.tcOpt = RTK_L34_DSLITE_TC_OPT_COPY_FROM_TOS;
	
	entry.flowLabel = *flow_label_ptr;
	entry.hopLimit = *hop_limit_ptr;

	if(tc_value_ptr != NULL){
		DIAG_UTIL_PARAM_RANGE_CHK((0xFF < *tc_value_ptr), CPARSER_ERR_INVALID_PARAMS);
		entry.tc = *tc_value_ptr;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_set(&entry), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_interface_index_flow_label_flow_label_hop_limit_hop_limit_tc_opt_assign_copy_egress_tos_tc_value */

/*
 * l34 set ds-lite interface <UINT:index> state ( enable | disable ) */
cparser_result_t
cparser_cmd_l34_set_ds_lite_interface_index_state_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_dsliteInf_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_dsliteInf_entry_t));
	
	DIAG_UTIL_PARAM_CHK();

	entry.index = *index_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_get(&entry), ret);

	if('e' == TOKEN_CHAR(6,0))
		entry.valid = ENABLED;
	else if ('d' == TOKEN_CHAR(6,0))
		entry.valid = DISABLED;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_set(&entry), ret);
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_interface_index_state_enable_disable */

/*
 * l34 get ds-lite interface
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_interface(
	cparser_context_t *context)
{
	rtk_l34_dsliteInf_entry_t entry;
	uint32 index;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	/*Header*/
	_cparser_cmd_l34_dslite_table_header_display();
	
	for(index = 0; index < HAL_L34_DSLITE_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_l34_dsliteInf_entry_t));
		entry.index = index;
		DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_get(&entry), ret);
		
		_cparser_cmd_l34_dslite_table_display(&entry);
	}
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_interface */

/*
 * l34 get ds-lite interface <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_interface_index(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_dsliteInf_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_dsliteInf_entry_t));
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteInfTable_get(&entry), ret);
	
	/*Header*/
	_cparser_cmd_l34_dslite_table_header_display();
	
	_cparser_cmd_l34_dslite_table_display(&entry);
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_interface_index */

/*
 * l34 set ds-lite mcast-table <UINT:index> src-ip6 <IPV6ADDR:src_ip6> mask <IPV6ADDR:src_mask> dst-ip6 <IPV6ADDR:dst_ip6> mask <IPV6ADDR:dst_mask> 
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_mcast_table_index_src_ip6_src_ip6_mask_src_mask_dst_ip6_dst_ip6_mask_dst_mask(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	char * *src_ip6_ptr,
	char * *src_mask_ptr,
	char * *dst_ip6_ptr,
	char * *dst_mask_ptr)
{
	rtk_l34_dsliteMc_entry_t entry;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry, 0x0, sizeof(rtk_l34_dsliteMc_entry_t));
	
	entry.index = *index_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteMcTable_get(&entry), ret);
	
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipUPrefix64.ipv6_addr[0], *src_ip6_ptr), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipUPrefix64Mask.ipv6_addr[0], *src_mask_ptr), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipMPrefix64.ipv6_addr[0], *dst_ip6_ptr), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&entry.ipMPrefix64Mask.ipv6_addr[0], *dst_mask_ptr), ret);

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteMcTable_set(&entry), ret);
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_mcast_table_index_src_ip6_src_ip6_mask_src_mask_dst_ip6_dst_ip6_mask_dst_mask */

/*
 * l34 get ds-lite mcast-table
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_mcast_table(
	cparser_context_t *context)
{
	rtk_l34_dsliteMc_entry_t entry;
	uint32 index;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	/*Header*/
	_cparser_cmd_l34_dslite_mcast_table_header_display();

	for(index = 0; index < HAL_L34_DSLITE_MC_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_l34_dsliteMc_entry_t));
		entry.index = index;
		DIAG_UTIL_ERR_CHK(rtk_l34_dsliteMcTable_get(&entry), ret);
	
		_cparser_cmd_l34_dslite_mcast_table_display(&entry);
	}
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_mcast_table */

/*
 * l34 get ds-lite mcast-table <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_mcast_table_index(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_dsliteMc_entry_t entry;
	int32 ret;

	osal_memset(&entry, 0x0, sizeof(rtk_l34_dsliteMc_entry_t));
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	entry.index = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteMcTable_get(&entry), ret);
	
	/*Header*/
 	_cparser_cmd_l34_dslite_mcast_table_header_display();
	_cparser_cmd_l34_dslite_mcast_table_display(&entry);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_mcast_table_index */

/*
 * l34 set ds-lite state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_state_enable_disable(
	cparser_context_t *context)
{
	rtk_enable_t enable;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	if('e' == TOKEN_CHAR(4,0))
		enable = ENABLED;
	else //if ('d' == TOKEN_CHAR(4,0))
		enable = DISABLED;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_set(L34_DSLITE_CTRL_DSLITE_STATE, (uint32)enable), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_state_enable_disable */

/*
 * l34 get ds-lite state
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_state(
	cparser_context_t *context)
{
	rtk_enable_t enable;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_get(L34_DSLITE_CTRL_DSLITE_STATE, &enable), ret);
	
	diag_util_mprintf("DS-Lite state : %s\n", diagStr_enable[enable]);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_state */

/*
 * l34 set ds-lite mcast-unmatch-act ( drop | trap )
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_mcast_unmatch_act_drop_trap(
	cparser_context_t *context)
{
	rtk_l34_dsliteUnmatch_act_t unmatchAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	if('d' == TOKEN_CHAR(4,0))
		unmatchAct = RTK_L34_DSLITE_UNMATCH_ACT_DROP;
	else //if ('t' == TOKEN_CHAR(4,0))
		unmatchAct = RTK_L34_DSLITE_UNMATCH_ACT_TRAP;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_set(L34_DSLITE_CTRL_MC_PREFIX_UNMATCH, (uint32)unmatchAct), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_mcast_unmatch_act_drop_trap */

/*
 * l34 get ds-lite mcast-unmatch-act
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_mcast_unmatch_act(
	cparser_context_t *context)
{
	rtk_l34_dsliteUnmatch_act_t unmatchAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_get(L34_DSLITE_CTRL_MC_PREFIX_UNMATCH, &unmatchAct), ret);
	
	diag_util_mprintf("Mcast unmatch action : %s\n", diagStr_dsliteUnmatchAct[unmatchAct]);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_mcast_unmatch_act */

/*
 * l34 set ds-lite ds-unmatch-act ( drop | trap ) */
cparser_result_t
cparser_cmd_l34_set_ds_lite_ds_unmatch_act_drop_trap(
	cparser_context_t *context)
{
	rtk_l34_dsliteUnmatch_act_t unmatchAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	if('d' == TOKEN_CHAR(4,0))
		unmatchAct = RTK_L34_DSLITE_UNMATCH_ACT_DROP;
	else //if ('t' == TOKEN_CHAR(4,0))
		unmatchAct = RTK_L34_DSLITE_UNMATCH_ACT_TRAP;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_set(L34_DSLITE_CTRL_DS_UNMATCH_ACT, unmatchAct), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_ds_unmatch_act_drop_trap */

/*
 * l34 get ds-lite ds-unmatch-act */
cparser_result_t
cparser_cmd_l34_get_ds_lite_ds_unmatch_act(
	cparser_context_t *context)
{
	rtk_l34_dsliteUnmatch_act_t unmatchAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_get(L34_DSLITE_CTRL_DS_UNMATCH_ACT, &unmatchAct), ret);
	
	diag_util_mprintf("DS unicast unmatch action : %s\n", diagStr_dsliteUnmatchAct[unmatchAct]);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_ds_unmatch_act */

/*
 * l34 set ds-lite ipv6-next-header-act ( trap-extheader-over-0 | trap-extheader-over-240 )
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_ipv6_next_header_act_trap_extheader_over_0_trap_extheader_over_240(
	cparser_context_t *context)
{
	rtk_l34_dsliteNextHeader_act_t dsliteNhAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(4),"trap-extheader-over-0"))
		dsliteNhAct = RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_0;
	else //if(!osal_strcmp(TOKEN_STR(4),"trap-extheader-over-240"))
		dsliteNhAct = RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_240;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_set(L34_DSLITE_CTRL_IP6_NH_ACTION, dsliteNhAct), ret);
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_ipv6_next_header_act_trap_extheader_over_len_0_trap_extheader_over_len_254 */

/*
 * l34 get ds-lite ipv6-next-header-act
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_ipv6_next_header_act(
	cparser_context_t *context)
{
	rtk_l34_dsliteNextHeader_act_t dsliteNhAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_get(L34_DSLITE_CTRL_IP6_NH_ACTION, &dsliteNhAct), ret);
	
	diag_util_mprintf("%s\n",diagStr_dsliteNhAct[dsliteNhAct]);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_ipv6_next_header_act */

/*
 * l34 set ds-lite ipv6-frag-act ( trap | drop )
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_ipv6_frag_act_trap_drop(
	cparser_context_t *context)
{
	rtk_l34_dsliteFrag_act_t dsliteFragAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	if('d' == TOKEN_CHAR(4,0))
		dsliteFragAct = RTK_L34_DSLITE_FRAG_ACT_DROP;
	else //if ('t' == TOKEN_CHAR(4,0))
		dsliteFragAct = RTK_L34_DSLITE_FRAG_ACT_TRAP;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_set(L34_DSLITE_CTRL_IP6_FRAGMENT_ACTION, dsliteFragAct), ret);
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_ipv6_frag_act_trap_drop */

/*
 * l34 get ds-lite ipv6-frag-act
 */
cparser_result_t
cparser_cmd_l34_get_ds_lite_ipv6_frag_act(
	cparser_context_t *context)
{
	rtk_l34_dsliteFrag_act_t dsliteFragAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_get(L34_DSLITE_CTRL_IP6_FRAGMENT_ACTION, &dsliteFragAct), ret);
	
	diag_util_mprintf("%s\n",diagStr_dsliteFragAct[dsliteFragAct]);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ds_lite_ipv6_frag_act */

/*
 * l34 set ds-lite ipv4-frag-act ( trap | drop | forward )
 */
cparser_result_t
cparser_cmd_l34_set_ds_lite_ipv4_frag_act_trap_drop_forward(
	cparser_context_t *context)
{
	rtk_l34_dsliteFrag_act_t dsliteFragAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();

	if('d' == TOKEN_CHAR(4,0))
		dsliteFragAct = RTK_L34_DSLITE_FRAG_ACT_DROP;
	else if ('t' == TOKEN_CHAR(4,0))
		dsliteFragAct = RTK_L34_DSLITE_FRAG_ACT_TRAP;
	else //if ('f' == TOKEN_CHAR(4,0))
		dsliteFragAct = RTK_L34_DSLITE_FRAG_ACT_FWD;

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_set(L34_DSLITE_CTRL_IP4_FRAGMENT_ACTION, dsliteFragAct), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_ipv4_frag_act_trap_drop_forward */

/*
 * l34 get ds-lite ipv4-frag-act */
cparser_result_t
cparser_cmd_l34_get_ds_lite_ipv4_frag_act(
	cparser_context_t *context)
{
	rtk_l34_dsliteFrag_act_t dsliteFragAct;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_dsliteControl_get(L34_DSLITE_CTRL_IP4_FRAGMENT_ACTION, &dsliteFragAct), ret);
	
	diag_util_mprintf("%s\n",diagStr_dsliteFragAct[dsliteFragAct]);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ds_lite_ipv4_frag */

/*
 * l34 get conuter netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_counter_netif_index(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_mib_t mibCnts;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	mibCnts.ifIndex = *index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_mib_get(&mibCnts), ret);
	
	diag_util_mprintf("Interface %d counters:\n",mibCnts.ifIndex);
	diag_util_mprintf("In Octets    : %25llu\n",mibCnts.ifInOctets);
	diag_util_mprintf("In Ucst Pkts : %25u\n",mibCnts.ifInUcstPkts);
	diag_util_mprintf("Out Octets   : %25llu\n",mibCnts.ifOutOctets);
	diag_util_mprintf("Out Ucst Pkts: %25u\n",mibCnts.ifOutUcstPkts);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_counter_netif_index */

/*
 * l34 reset conuter netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_reset_counter_netif_index(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_mib_t mibCnts;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_mib_reset(*index_ptr), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_counter_netif_index */

/*
 *  l34 set lut-miss action ( drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l34_set_lut_miss_action_drop_trap_to_cpu(
	cparser_context_t *context)
{
	int32 ret;
	rtk_l34_lutMissAct_t lutMissAct;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if('d' == TOKEN_CHAR(4,0))
		lutMissAct = L34_LUTMISS_ACT_DROP;
	else //if ('t' == TOKEN_CHAR(4,0))
		lutMissAct = L34_LUTMISS_ACT_TRAP;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_lutLookupMiss_set(lutMissAct), ret);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_lut_miss_action_drop_trap_to_cpu */ 

/*
 *  l34 get lut-miss action
 */
cparser_result_t
cparser_cmd_l34_get_lut_miss_action(
	cparser_context_t *context)
{
	int32 ret;
	rtk_l34_lutMissAct_t lutMissAct;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	DIAG_UTIL_ERR_CHK(rtk_l34_lutLookupMiss_get(&lutMissAct), ret);

	diag_util_mprintf("L34 LUT lookup miss action :");
	
	if(lutMissAct == L34_LUTMISS_ACT_TRAP)
		diag_util_mprintf("%s\n", DIAG_STR_TRAP2CPU);
	else 
		diag_util_mprintf("%s\n", DIAG_STR_DROP);

	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_lut_miss_action_drop_trap_to_cpu */ 

/*
 *  l34 set ip-mcast-trans <UINT:index> netif <UINT:netIfIdx>
 */
cparser_result_t
cparser_cmd_l34_set_ip_mcast_trans_index_netif_netIfIdx(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *netifIdx_ptr)
{
	rtk_l34_ipmcTrans_entry_t entry;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0x0,sizeof(rtk_l34_ipmcTrans_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&entry), ret);

	entry.netifIdx = *netifIdx_ptr;
         
	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_set(*index_ptr,&entry), ret);
   
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ip_mcast_trans_index_netif_netIfIdx */

/*
 *  l34 set ip-mcast-trans <UINT:index> sip-trans ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_ip_mcast_trans_index_sip_trans_enable_disable(
	cparser_context_t *context,
	uint32_t  *index_ptr)
{
	rtk_l34_ipmcTrans_entry_t entry;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0x0,sizeof(rtk_l34_ipmcTrans_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&entry), ret);

	if('e' == TOKEN_CHAR(5,0))
		entry.sipTransEnable = ENABLED;
    else
		entry.sipTransEnable = DISABLED;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_set(*index_ptr,&entry), ret);
   
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ip_mcast_trans_index_sip_trans_enable_disable */
 
 
/*
 *  l34 set ip-mcast-trans <UINT:index> ext-ip <UINT:extIpIdx>
 */
cparser_result_t
cparser_cmd_l34_set_ip_mcast_trans_index_ext_ip_extIpIdx(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *extIpIdx)
{
	rtk_l34_ipmcTrans_entry_t entry;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0x0,sizeof(rtk_l34_ipmcTrans_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&entry), ret);

	entry.extipIdx = *extIpIdx;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_set(*index_ptr,&entry), ret);
   
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ip_mcast_trans_index_ext_ip_extIpIdx */
  
/*
 *  l34 set ip-mcast-trans <UINT:index> pppoe <UINT:pppIdx>
 */
cparser_result_t
cparser_cmd_l34_set_ip_mcast_trans_index_pppoe_pppIdx(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *pppIdx)
{
	rtk_l34_ipmcTrans_entry_t entry;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0x0,sizeof(rtk_l34_ipmcTrans_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&entry), ret);

	entry.pppoeIdx = *pppIdx;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_set(*index_ptr,&entry), ret);
   
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ip_mcast_trans_index_pppoe_pppIdx */
   
/*
 *  l34 set ip-mcast-trans <UINT:index> pppoe-act ( no-act | keep | remove | modify )
 */
cparser_result_t
cparser_cmd_l34_set_ip_mcast_trans_index_pppoe_act_no_act_keep_remove_modify(
	cparser_context_t *context,
	uint32_t  *index_ptr,
	uint32_t  *pppIdx)
{
	rtk_l34_ipmcTrans_entry_t entry;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0x0,sizeof(rtk_l34_ipmcTrans_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&entry), ret);

	if('n' == TOKEN_CHAR(5,0))
		entry.pppoeAct = L34_PPPOE_ACT_NO;
	else if('k' == TOKEN_CHAR(5,0))
		entry.pppoeAct = L34_PPPOE_ACT_KEEP;
	else if('r' == TOKEN_CHAR(5,0))
		entry.pppoeAct = L34_PPPOE_ACT_REMOVE;
	else if('m' == TOKEN_CHAR(5,0))
		entry.pppoeAct = L34_PPPOE_ACT_MODIFY;

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_set(*index_ptr,&entry), ret);
   
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ip_mcast_trans_index_pppoe_act_no_act_keep_remove_modify */
    
/*
 *  l34 set ip-mcast-trans <UINT:index> untag-member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l34_set_ip_mcast_trans_index_untag_member_ports_all_none(
	cparser_context_t *context,
	uint32_t  *index_ptr,
    char * *ports_ptr)
{
	rtk_l34_ipmcTrans_entry_t entry;
	diag_portlist_t portlist;
	int32 ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0x0,sizeof(rtk_l34_ipmcTrans_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&entry), ret);

	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    RTK_PORTMASK_ASSIGN(entry.untagMbr, portlist.portmask);
	
	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_set(*index_ptr,&entry), ret);
   
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_set_ip_mcast_trans_index_pppoe_pppIdx */
    
 
/*
 *  l34 get ip-mcast-trans 
 */
cparser_result_t
cparser_cmd_l34_get_ip_mcast_trans(
	cparser_context_t *context)
{
	int32 ret;
	rtk_l34_ipmcTrans_entry_t ipmc;
	uint32 i;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_ipmc_table_header_display();

	for(i = 0; i < HAL_L34_IPMC_TRAN_ENTRY_MAX(); i++)
	{
		osal_memset(&ipmc,0,sizeof(rtk_l34_ipmcTrans_entry_t));
		DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(i,&ipmc),ret);
    
     	_cparser_cmd_l34_ipmc_table_display(i,&ipmc);
	}
	
	return CPARSER_OK;
}	 /* end of cparser_cmd_l34_get_ip_mcast_trans */ 

/*
 * l34 get ip-mcast-trans <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_ip_mcast_trans_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_ipmcTrans_entry_t ipmc;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&ipmc,0,sizeof(rtk_l34_ipmcTrans_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_ipmcTransTable_get(*index_ptr,&ipmc),ret);

    _cparser_cmd_l34_ipmc_table_header_display();
    _cparser_cmd_l34_ipmc_table_display(*index_ptr,&ipmc);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ip_mcast_trans_index */

/*
 * l34 set arp-traffic-indicator sip ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_arp_traffic_indicator_sip_enable_disable(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if(!osal_strcmp(TOKEN_STR(4),"enable")){
		state = ENABLED;
	}else{
		state = DISABLED;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(L34_GLOBAL_SIP_ARP_TRF_STATE, state),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_arp_traffic_indicator_sip_enable_disable */

/*
 * l34 get arp-traffic-indicator sip
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator_sip(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(L34_GLOBAL_SIP_ARP_TRF_STATE, &state),ret);

    if(state == DISABLED){
		diag_util_mprintf("ARP TRF update by SIP/SMAC State: Disable\n");
	}else{
		diag_util_mprintf("ARP TRF update by SIP/SMAC State: Enable\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator_sip */

/*
 * l34 set arp-traffic-indicator dip ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_arp_traffic_indicator_dip_enable_disable(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if(!osal_strcmp(TOKEN_STR(4),"enable")){
		state = ENABLED;
	}else{
		state = DISABLED;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(L34_GLOBAL_DIP_ARP_TRF_STATE, state),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_arp_traffic_indicator_dip_enable_disable */

/*
 * l34 get arp-traffic-indicator dip
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator_dip(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(L34_GLOBAL_DIP_ARP_TRF_STATE, &state),ret);

    if(state == DISABLED){
		diag_util_mprintf("ARP TRF update by DIP State: Disable\n");
	}else{
		diag_util_mprintf("ARP TRF update by DIP State: Enable\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator_dip */
