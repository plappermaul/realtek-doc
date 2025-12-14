/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Test L34 API
 *
 * Feature : L34 API test 
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <rtk/l34.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <dal/dal_l34_test_case.h>

/*
 * Function Declaration
 */
int32 dal_l34_netifTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_netif_entry_t wEntry;
    rtk_l34_netif_entry_t rEntry;
    uint32 cnt;
    
    /*error input check*/
    /*out of range*/
    index = HAL_L34_NETIF_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));
    if( rtk_l34_netifTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }    

    index = HAL_L34_NETIF_ENTRY_MAX();    
    if( rtk_l34_netifTable_get(index, &rEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }    
    
    index = 0;
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));
    wEntry.mtu = 16384;
    if( rtk_l34_netifTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));
    wEntry.enable_rounting = RTK_ENABLE_END;
    if( rtk_l34_netifTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));    
    wEntry.valid = 2;
    if( rtk_l34_netifTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));
    wEntry.vlan_id = RTK_VLAN_ID_MAX+1;
    if( rtk_l34_netifTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));
    wEntry.mac_mask = 8;
    if( rtk_l34_netifTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_netif_entry_t));
    cnt = 0;
    for(index = 0 ; index <HAL_L34_NETIF_ENTRY_MAX() ; index++)
    {
        for(wEntry.vlan_id = 0 ; wEntry.vlan_id<=RTK_VLAN_ID_MAX ; wEntry.vlan_id += 0x1F)
        {
            for(wEntry.mtu = 0 ; wEntry.mtu<16384 ; wEntry.mtu+= 0x1F3)
            {   
     
                if(cnt%3 == 0)
                    wEntry.enable_rounting = DISABLED;
                else    
                    wEntry.enable_rounting = ENABLED;

                if(cnt%2 == 0)
                    wEntry.valid = 0;
                else    
                    wEntry.valid = 1;

                wEntry.gateway_mac.octet[0]=(cnt%0xFF);
                wEntry.gateway_mac.octet[1]=(cnt+1%0xFF);
                wEntry.gateway_mac.octet[2]=(cnt+2%0xFF);
                wEntry.gateway_mac.octet[3]=(cnt+3%0xFF);
                wEntry.gateway_mac.octet[4]=(cnt+4%0xFF);
                wEntry.gateway_mac.octet[5]=(cnt+5%0xFF);

                if(cnt%4 == 0)
                    wEntry.mac_mask = 0;
                else if(cnt%4 == 1)   
                    wEntry.mac_mask = 4;
                else if(cnt%4 == 2)   
                    wEntry.mac_mask = 6;
                else   
                    wEntry.mac_mask = 7;


                if(rtk_l34_netifTable_set(index, &wEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);

                    osal_printf("write: index:%d %5d %2d %2d %4d %2d mac[%2x:%2x:%2x:%2x:%2x:%2x]\n",
                                  index,
                                  wEntry.mtu,
                                  wEntry.enable_rounting,
                                  wEntry.mac_mask,
                                  wEntry.vlan_id,
                                  wEntry.valid,
                                  wEntry.gateway_mac.octet[0],
                                  wEntry.gateway_mac.octet[1],
                                  wEntry.gateway_mac.octet[2],
                                  wEntry.gateway_mac.octet[3],
                                  wEntry.gateway_mac.octet[4],
                                  wEntry.gateway_mac.octet[5]);

                    return RT_ERR_FAILED;
                }

                osal_memset(&rEntry,0x0, sizeof(rtk_l34_netif_entry_t));

                if( rtk_l34_netifTable_get(index, &rEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_netif_entry_t))!=0)
                {
                #if 1    
                    osal_printf("write: index:%d %5d %2d %2d %4d %2d mac[%2x:%2x:%2x:%2x:%2x:%2x]\n",
                                  index,
                                  wEntry.mtu,
                                  wEntry.enable_rounting,
                                  wEntry.mac_mask,
                                  wEntry.vlan_id,
                                  wEntry.valid,
                                  wEntry.gateway_mac.octet[0],
                                  wEntry.gateway_mac.octet[1],
                                  wEntry.gateway_mac.octet[2],
                                  wEntry.gateway_mac.octet[3],
                                  wEntry.gateway_mac.octet[4],
                                  wEntry.gateway_mac.octet[5]);

                    osal_printf("read: index:%d %5d %2d %2d %4d %2d mac[%2x:%2x:%2x:%2x:%2x:%2x]\n",
                                  index,
                                  rEntry.mtu,
                                  rEntry.enable_rounting,
                                  rEntry.mac_mask,
                                  rEntry.vlan_id,
                                  rEntry.valid,
                                  rEntry.gateway_mac.octet[0],
                                  rEntry.gateway_mac.octet[1],
                                  rEntry.gateway_mac.octet[2],
                                  rEntry.gateway_mac.octet[3],
                                  rEntry.gateway_mac.octet[4],
                                  rEntry.gateway_mac.octet[5]);
                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }

                cnt++;    
            }
        }
    }


    /*null pointer*/
    if(rtk_l34_netifTable_set(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_netifTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    
    return RT_ERR_OK;
}

int32 dal_l34_arpTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_arp_entry_t wEntry;
    rtk_l34_arp_entry_t rEntry;

    /*error input check*/
    /*out of range*/
    index = HAL_L34_ARP_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_arp_entry_t));
    if( rtk_l34_arpTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    index = HAL_L34_ARP_ENTRY_MAX();    
    if( rtk_l34_arpTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    index = 0;    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_arp_entry_t));
    wEntry.valid = 2;
    if( rtk_l34_arpTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

   
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_arp_entry_t));
    wEntry.nhIdx = HAL_L34_NH_ENTRY_MAX();
    if( rtk_l34_arpTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    

    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_arp_entry_t));
    for(index = 0 ; index < HAL_L34_ARP_ENTRY_MAX() ; index++)
    {
        for(wEntry.nhIdx = 0 ; wEntry.nhIdx < HAL_L34_NH_ENTRY_MAX() ; wEntry.nhIdx ++)
        {
            for(wEntry.valid = 0 ; wEntry.valid<2 ; wEntry.valid++)
            {   

                if(rtk_l34_arpTable_set(index, &wEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    osal_printf("write: index:%d %5d %2d\n",
                                  index,
                                  wEntry.nhIdx,
                                  wEntry.valid);

                    return RT_ERR_FAILED;
                }

                osal_memset(&rEntry,0x0, sizeof(rtk_l34_arp_entry_t));

                if( rtk_l34_arpTable_get(index, &rEntry) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;
                }

                if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_arp_entry_t))!=0)
                {
                #if 1    
                    osal_printf("write: index:%d %5d %2d\n",
                                  index,
                                  wEntry.nhIdx,
                                  wEntry.valid);

                    osal_printf("read: index:%d %5d %2d\n",
                                  index,
                                  rEntry.nhIdx,
                                  rEntry.valid);
                #endif                  
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    /*null pointer*/
    if(rtk_l34_arpTable_set(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_arpTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
        
    return RT_ERR_OK;
}

int32 dal_l34_pppoeTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_pppoe_entry_t wEntry;
    rtk_l34_pppoe_entry_t rEntry;
    
    /*error input check*/
    /*out of range*/
    index = HAL_L34_PPPOE_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_pppoe_entry_t));
    if( rtk_l34_pppoeTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    index = HAL_L34_PPPOE_ENTRY_MAX();    
    if( rtk_l34_pppoeTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    index = 0;    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_pppoe_entry_t));
    wEntry.sessionID = 0x10000;
    if( rtk_l34_pppoeTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_pppoe_entry_t));
    for(index = 0 ; index < HAL_L34_PPPOE_ENTRY_MAX() ; index++)
    {
        for(wEntry.sessionID = 0 ; wEntry.sessionID < 0x10000 ; wEntry.sessionID+=0x100)
        {
            if(rtk_l34_pppoeTable_set(index, &wEntry) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                osal_printf("write: index:%d %d \n",
                              index,
                              wEntry.sessionID);
    
                return RT_ERR_FAILED;
            }
    
            osal_memset(&rEntry,0x0, sizeof(rtk_l34_pppoe_entry_t));
    
            if( rtk_l34_pppoeTable_get(index, &rEntry) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
    
            if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_pppoe_entry_t))!=0)
            {
            #if 1    
                osal_printf("write: index:%d %d \n",
                              index,
                              wEntry.sessionID);
    
                osal_printf("read: index:%d %d \n",
                              index,
                              rEntry.sessionID);
            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;            
            }
        }
    }        

    /*null pointer*/
    if(rtk_l34_pppoeTable_set(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_pppoeTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }     
    return RT_ERR_OK;
}

int32 dal_l34_routingTable_test(uint32 testcase)
{
    uint32 index;
    uint32 cnt;
    rtk_l34_routing_entry_t wEntry;
    rtk_l34_routing_entry_t rEntry;

    /*error input check*/
    /*out of range*/
    index = HAL_L34_ROUTING_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    index = HAL_L34_ROUTING_ENTRY_MAX();    
    if( rtk_l34_routingTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    index = 0;    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.ipMask = 32;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.valid = 2;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.internal = 2;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.process = L34_PROCESS_END;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.netifIdx = HAL_L34_NETIF_ENTRY_MAX();
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.arpEnd = 128;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.arpStart = 128;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.nhStart = HAL_L34_NH_ENTRY_MAX();
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.nhNum = 5;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.nhAlgo = 3;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    wEntry.ipDomain = 4;
    if( rtk_l34_routingTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    
    cnt = 0;
    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_routing_entry_t));
    for(index = 0 ; index < HAL_L34_ROUTING_ENTRY_MAX() ; index++)
    {
        for(wEntry.netifIdx = 0 ; wEntry.netifIdx < HAL_L34_NETIF_ENTRY_MAX() ; wEntry.netifIdx ++)
        {
            for(wEntry.process = L34_PROCESS_CPU ; wEntry.process<L34_PROCESS_END ; wEntry.process++)
            {   
                for(wEntry.ipMask = 0; wEntry.ipMask <32; wEntry.ipMask++)
                {
                    for(wEntry.ipAddr = 0x00000000; wEntry.ipAddr <0xFFFFFFFF; wEntry.ipAddr +=0x11111111)
                    {
                        if(cnt % 2 == 0)
                            wEntry.valid = 0;
                        else
                            wEntry.valid = 1;
        
                        if(cnt % 3 == 0)
                            wEntry.internal = 0;
                        else
                            wEntry.internal = 1;
        
                        wEntry.netifIdx = cnt % HAL_L34_NETIF_ENTRY_MAX();
                        wEntry.arpStart = cnt % 128;
                        wEntry.arpEnd = cnt % 128;
                        wEntry.nhStart = cnt % HAL_L34_NH_ENTRY_MAX();
                        wEntry.nhNum = cnt % 5;
                        wEntry.nhAlgo = cnt % 3;
                        wEntry.ipDomain = cnt % 4;
                        if(rtk_l34_routingTable_set(index, &wEntry) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            osal_printf("write: index:%d 0x%8x %d %d %d %d %d %d %d %d %d %d %d\n",
                                          index,
                                          wEntry.ipAddr,
                                          wEntry.ipMask,
                                          wEntry.process,
                                          wEntry.valid,
                                          wEntry.internal,
                                          wEntry.netifIdx,
                                          wEntry.arpStart,
                                          wEntry.arpEnd,
                                          wEntry.nhStart,
                                          wEntry.nhNum,
                                          wEntry.nhAlgo,
                                          wEntry.ipDomain);
        
                            return RT_ERR_FAILED;
                        }

                        osal_memset(&rEntry,0x0, sizeof(rtk_l34_routing_entry_t));
        
                        if( rtk_l34_routingTable_get(index, &rEntry) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }
        
                        if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_routing_entry_t))!=0)
                        {
                        #if 1    
                            osal_printf("write: index:%d 0x%8x %d %d %d %d %d %d %d %d %d %d %d\n",
                                          index,
                                          wEntry.ipAddr,
                                          wEntry.ipMask,
                                          wEntry.process,
                                          wEntry.valid,
                                          wEntry.internal,
                                          wEntry.netifIdx,
                                          wEntry.arpStart,
                                          wEntry.arpEnd,
                                          wEntry.nhStart,
                                          wEntry.nhNum,
                                          wEntry.nhAlgo,
                                          wEntry.ipDomain);

                            osal_printf("read: index:%d 0x%8x %d %d %d %d %d %d %d %d %d %d %d\n",
                                          index,
                                          rEntry.ipAddr,
                                          rEntry.ipMask,
                                          rEntry.process,
                                          rEntry.valid,
                                          rEntry.internal,
                                          rEntry.netifIdx,
                                          rEntry.arpStart,
                                          rEntry.arpEnd,
                                          rEntry.nhStart,
                                          rEntry.nhNum,
                                          rEntry.nhAlgo,
                                          rEntry.ipDomain);
        

                        #endif                  
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            
                            return RT_ERR_FAILED;
                        }
                        cnt++;
                    }
                }    
            }
        }
    }
    
        
    /*null pointer*/
    if(rtk_l34_routingTable_set(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_routingTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 
    
    return RT_ERR_OK;
}

int32 dal_l34_nexthopTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_nexthop_entry_t wEntry;
    rtk_l34_nexthop_entry_t rEntry;

    /*error input check*/
    /*out of range*/
    index = HAL_L34_NH_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    if( rtk_l34_nexthopTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    index = HAL_L34_NH_ENTRY_MAX();    
    if( rtk_l34_nexthopTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    index = 0;    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    wEntry.type = L34_NH_END;
    if( rtk_l34_nexthopTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

   
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    wEntry.ifIdx = HAL_L34_NETIF_ENTRY_MAX();
    if( rtk_l34_nexthopTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    wEntry.pppoeIdx = HAL_L34_PPPOE_ENTRY_MAX();
    if( rtk_l34_nexthopTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    wEntry.nhIdx = 2048;
    if( rtk_l34_nexthopTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }    
    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    for(index = 0 ; index < HAL_L34_NH_ENTRY_MAX() ; index++)
    {
        for(wEntry.ifIdx = 0 ; wEntry.ifIdx < HAL_L34_NETIF_ENTRY_MAX() ; wEntry.ifIdx ++)
        {
            for(wEntry.pppoeIdx = 0 ; wEntry.pppoeIdx<HAL_L34_PPPOE_ENTRY_MAX() ; wEntry.pppoeIdx++)
            {   
                for(wEntry.nhIdx = 0 ; wEntry.nhIdx<2048; wEntry.nhIdx+=128)
                {   
    
                    if(rtk_l34_nexthopTable_set(index, &wEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        osal_printf("write: index:%d %d %2d %2d %4d\n",
                                      index,
                                      wEntry.type,
                                      wEntry.ifIdx,
                                      wEntry.pppoeIdx,
                                      wEntry.nhIdx);
    
                        return RT_ERR_FAILED;
                    }
    
                    osal_memset(&rEntry,0x0, sizeof(rtk_l34_nexthop_entry_t));
    
                    if( rtk_l34_nexthopTable_get(index, &rEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
    
                    if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_nexthop_entry_t))!=0)
                    {
                    #if 1    
                        osal_printf("write: index:%d %d %2d %2d %4d\n",
                                      index,
                                      wEntry.type,
                                      wEntry.ifIdx,
                                      wEntry.pppoeIdx,
                                      wEntry.nhIdx);
    
                        osal_printf("read: index:%d %d %2d %2d %4d\n",
                                      index,
                                      rEntry.type,
                                      rEntry.ifIdx,
                                      rEntry.pppoeIdx,
                                      rEntry.nhIdx);
                    #endif                  
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        
                        return RT_ERR_FAILED;
                    }
                }
            }
        }
    }

    /*null pointer*/
    if(rtk_l34_nexthopTable_set(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_nexthopTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
        
    return RT_ERR_OK;
}


int32 dal_l34_extIntIPTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_ext_intip_entry_t wEntry;
    rtk_l34_ext_intip_entry_t rEntry;
    uint32 cnt;

    /*error input check*/
    /*out of range*/
    index = HAL_L34_EXTIP_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    if( rtk_l34_extIntIPTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    index = HAL_L34_EXTIP_ENTRY_MAX();    
    if( rtk_l34_extIntIPTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    index = 0; 
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    wEntry.prival = 2;
    if( rtk_l34_extIntIPTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    wEntry.valid = 2;
    if( rtk_l34_extIntIPTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    wEntry.pri = RTK_MAX_NUM_OF_PRIORITY;
    if( rtk_l34_extIntIPTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    wEntry.nhIdx = HAL_L34_NH_ENTRY_MAX();
    if( rtk_l34_extIntIPTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    wEntry.type = L34_EXTIP_TYPE_END;
    if( rtk_l34_extIntIPTable_set(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }    
    
    cnt = 0;
    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
    for(index = 0 ; index < HAL_L34_EXTIP_ENTRY_MAX() ; index++)
    {
        for(wEntry.nhIdx = 0 ; wEntry.nhIdx < HAL_L34_NH_ENTRY_MAX() ; wEntry.nhIdx ++)
        {
            for(wEntry.type = L34_EXTIP_TYPE_NAPT ; wEntry.type<L34_EXTIP_TYPE_END ; wEntry.type++)
            {   
                for(wEntry.extIpAddr = 0x00000000; wEntry.extIpAddr <0xFFFFFFFF; wEntry.extIpAddr +=0x11111111)
                {
                    for(wEntry.intIpAddr = 0x00000000; wEntry.intIpAddr <0xFFFFFFFF; wEntry.intIpAddr +=0x11111111)
                    {
                        if(cnt % 2 == 0)
                            wEntry.valid = 0;
                        else
                            wEntry.valid = 1;
        
                        if(cnt % 3 == 0)
                            wEntry.prival = 0;
                        else
                            wEntry.prival = 1;
        
                        wEntry.pri = cnt % RTK_MAX_NUM_OF_PRIORITY;
        
                                        
                        if(rtk_l34_extIntIPTable_set(index, &wEntry) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            osal_printf("write: index:%d 0x%8x 0x%8x %d %d %2d %d %d\n",
                                          index,
                                          wEntry.extIpAddr,
                                          wEntry.intIpAddr,
                                          wEntry.valid,
                                          wEntry.type,
                                          wEntry.nhIdx,
                                          wEntry.prival,
                                          wEntry.pri);
        
                            return RT_ERR_FAILED;
                        }
        
                        osal_memset(&rEntry,0x0, sizeof(rtk_l34_ext_intip_entry_t));
        
                        if( rtk_l34_extIntIPTable_get(index, &rEntry) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;
                        }
        
                        if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_ext_intip_entry_t))!=0)
                        {
                        #if 1    
                            osal_printf("write: index:%d 0x%8x 0x%8x %d %d %2d %d %d\n",
                                          index,
                                          wEntry.extIpAddr,
                                          wEntry.intIpAddr,
                                          wEntry.valid,
                                          wEntry.type,
                                          wEntry.nhIdx,
                                          wEntry.prival,
                                          wEntry.pri);

                            osal_printf("read: index:%d 0x%8x 0x%8x %d %d %2d %d %d\n",
                                          index,
                                          rEntry.extIpAddr,
                                          rEntry.intIpAddr,
                                          rEntry.valid,
                                          rEntry.type,
                                          rEntry.nhIdx,
                                          rEntry.prival,
                                          rEntry.pri);        

                        #endif                  
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            
                            return RT_ERR_FAILED;
                        }
                        cnt++;
                    }
                }    
            }
        }
    }

    /*null pointer*/
    if(rtk_l34_extIntIPTable_set(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_extIntIPTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    return RT_ERR_OK;
}

int32 dal_l34_naptInboundTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_naptInbound_entry_t wEntry;
    rtk_l34_naptInbound_entry_t rEntry;
    uint32 cnt;

    /*error input check*/
    /*out of range*/
    index = HAL_L34_NAPTR_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    if(rtk_l34_naptInboundTable_set(1,index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    index = HAL_L34_NAPTR_ENTRY_MAX();    
    if( rtk_l34_naptInboundTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    index = 0;    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    wEntry.valid = 4;
    if( rtk_l34_naptInboundTable_set(1,index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    wEntry.priValid = 2;
    if( rtk_l34_naptInboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    wEntry.priId = RTK_MAX_NUM_OF_PRIORITY;
    if( rtk_l34_naptInboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    wEntry.isTcp = 2;
    if( rtk_l34_naptInboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    wEntry.extIpIdx = HAL_L34_EXTIP_ENTRY_MAX();
    if( rtk_l34_naptInboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
   
    
    cnt = 0;
    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    for(index = 0 ; index < HAL_L34_NAPTR_ENTRY_MAX() ; index++)
    {
        for(wEntry.extIpIdx = 0 ; wEntry.extIpIdx < HAL_L34_EXTIP_ENTRY_MAX() ; wEntry.extIpIdx ++)
        {
            for(wEntry.valid = 0 ; wEntry.valid<4 ; wEntry.valid++)
            {   
                for(wEntry.intIp = 0x0; wEntry.intIp <0xFFFFFFFF; wEntry.intIp +=0x11111111)
                {
                    wEntry.intPort = wEntry.intIp %0xFFFF;
                    
                    if(cnt % 2 == 0)
                        wEntry.priValid = 0;
                    else
                        wEntry.priValid = 1;
    
                    wEntry.priId = cnt % RTK_MAX_NUM_OF_PRIORITY;
    
                    wEntry.remHash = wEntry.intPort;
                    wEntry.extPortLSB = wEntry.intPort%0xFF;
                                    
                    if(rtk_l34_naptInboundTable_set(1, index, &wEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        osal_printf("write: index:%d 0x%8x 0x%4x 0x%4x %2d 0x%4x %d %d %d %d\n",
                                      index,
                                      wEntry.intIp,
                                      wEntry.intPort,
                                      wEntry.remHash,
                                      wEntry.extIpIdx,
                                      wEntry.extPortLSB,
                                      wEntry.isTcp,
                                      wEntry.valid,
                                      wEntry.priValid,
                                      wEntry.priId);
    
                        return RT_ERR_FAILED;
                    }
    
                    osal_memset(&rEntry,0x0, sizeof(rtk_l34_naptInbound_entry_t));
    
                    if( rtk_l34_naptInboundTable_get(index, &rEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
    
                    if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_naptInbound_entry_t))!=0)
                    {
                    #if 1    
                        osal_printf("write: index:%d 0x%8x 0x%4x 0x%4x %2d 0x%4x %d %d %d %d\n",
                                      index,
                                      wEntry.intIp,
                                      wEntry.intPort,
                                      wEntry.remHash,
                                      wEntry.extIpIdx,
                                      wEntry.extPortLSB,
                                      wEntry.isTcp,
                                      wEntry.valid,
                                      wEntry.priValid,
                                      wEntry.priId);

                        osal_printf("read: index:%d 0x%8x 0x%4x 0x%4x %2d 0x%4x %d %d %d %d\n",
                                      index,
                                      rEntry.intIp,
                                      rEntry.intPort,
                                      rEntry.remHash,
                                      rEntry.extIpIdx,
                                      rEntry.extPortLSB,
                                      rEntry.isTcp,
                                      rEntry.valid,
                                      rEntry.priValid,
                                      rEntry.priId);
 

                    #endif                  
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        
                        return RT_ERR_FAILED;
                    }
                    cnt++;
                }    
            }
        }
    }

    /*null pointer*/
    if(rtk_l34_naptInboundTable_set(1, 0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_naptInboundTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}

int32 dal_l34_naptOutboundTable_test(uint32 testcase)
{
    uint32 index;
    rtk_l34_naptOutbound_entry_t wEntry;
    rtk_l34_naptOutbound_entry_t rEntry;
    uint32 cnt;

    /*error input check*/
    /*out of range*/
    index = HAL_L34_NAPT_ENTRY_MAX();    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    if(rtk_l34_naptOutboundTable_set(1,index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    index = HAL_L34_NAPT_ENTRY_MAX();    
    if( rtk_l34_naptOutboundTable_get(index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    index = 0;    
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    wEntry.hashIdx = 1024;
    if( rtk_l34_naptOutboundTable_set(1,index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    wEntry.valid = 2;
    if( rtk_l34_naptOutboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    wEntry.priValue = RTK_MAX_NUM_OF_PRIORITY;
    if( rtk_l34_naptOutboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    wEntry.priValid = 2;
    if( rtk_l34_naptOutboundTable_set(1, index, &wEntry) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    osal_memset(&wEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    for(index = 0 ; index < HAL_L34_NAPT_ENTRY_MAX() ; index++)
    {
        for(wEntry.hashIdx = 0 ; wEntry.hashIdx < 1024; wEntry.hashIdx+= 128)
        {
            for(wEntry.valid = 0 ; wEntry.valid<2 ; wEntry.valid++)
            {   
                for(wEntry.priValid = 0 ; wEntry.priValid<2 ; wEntry.priValid++)
                {
                    wEntry.priValue = cnt % RTK_MAX_NUM_OF_PRIORITY;
    
                    if(rtk_l34_naptOutboundTable_set(1, index, &wEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        osal_printf("write: index:%d %4d %d %d %d\n",
                                      index,
                                      wEntry.hashIdx,
                                      wEntry.valid,
                                      wEntry.priValid,
                                      wEntry.priValue);
    
                        return RT_ERR_FAILED;
                    }
    
                    osal_memset(&rEntry,0x0, sizeof(rtk_l34_naptOutbound_entry_t));
    
                    if( rtk_l34_naptOutboundTable_get(index, &rEntry) != RT_ERR_OK)
                    {
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        return RT_ERR_FAILED;
                    }
    
                    if(memcmp(&wEntry,&rEntry,sizeof(rtk_l34_naptOutbound_entry_t))!=0)
                    {
                    #if 1    
                        osal_printf("write: index:%d %4d %d %d %d\n",
                                      index,
                                      wEntry.hashIdx,
                                      wEntry.valid,
                                      wEntry.priValid,
                                      wEntry.priValue);

                        osal_printf("reaf: index:%d %4d %d %d %d\n",
                                      index,
                                      rEntry.hashIdx,
                                      rEntry.valid,
                                      rEntry.priValid,
                                      rEntry.priValue);
 

                    #endif                  
                        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                        
                        return RT_ERR_FAILED;
                    }
                    cnt++;
                }    
            }
        }
    }

    /*null pointer*/
    if(rtk_l34_naptOutboundTable_set(1, 0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_l34_naptOutboundTable_get(0,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}

