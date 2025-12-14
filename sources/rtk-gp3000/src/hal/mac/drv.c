/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/mac/drv.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <ioal/io_mii.h>
#include <ioal/mem32.h>
#include <rtk/gpio.h>

#ifdef CONFIG_SDK_APOLLO
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <dal/apollo/raw/apollo_raw_l2.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_l2.h>
#endif

#ifdef CONFIG_SDK_RTL9601B
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_l2.h>
#endif

#ifdef CONFIG_SDK_RTL9602C
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_l2.h>
#endif

#ifdef CONFIG_SDK_RTL9607C
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_l2.h>
#include <rtk/mdio.h>
#if defined(CONFIG_COMMON_RT_API)
#include <rtk/rt/rt_mdio.h>
#endif
#include <dal/rtl9607c/dal_rtl9607c_mdio.h>

#endif

#ifdef CONFIG_SDK_RTL9603CVD
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_l2.h>
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_LUNA_G3_SERIES)
#define RTK_INTPHY_ACCESS_LOCK
#define RTK_INTPHY_ACCESS_UNLOCK
#else
#include <linux/spinlock.h>
#if defined(CONFIG_KERNEL_2_6_30)
spinlock_t rtkIntPhySpinLock = SPIN_LOCK_UNLOCKED;
#else
DEFINE_SPINLOCK(rtkIntPhySpinLock);
#endif
unsigned long rtkIntPhySpinflags;

#define RTK_INTPHY_ACCESS_LOCK      spin_lock_irqsave(&rtkIntPhySpinLock,rtkIntPhySpinflags)
#define RTK_INTPHY_ACCESS_UNLOCK    spin_unlock_irqrestore(&rtkIntPhySpinLock,rtkIntPhySpinflags)
#endif
#else
#define RTK_INTPHY_ACCESS_LOCK
#define RTK_INTPHY_ACCESS_UNLOCK
#endif


/*
 * Symbol Definition
 */
#define APOLLO_TBL_BUSY_CHECK_NO            (0xFFFF)

#ifdef CONFIG_SDK_APOLLO
    #define APOLLO_L34_TABLE_WORD  (3)
    #define APOLLO_L2_TABLE_WORD   (5)
    #define APOLLO_LUT_TABLE_WORD  (4)

    #define APOLLO_DEFAULT_MIIM_PAGE    (0xA40)

    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_TEST_VLAN_SIZE           4096
        #define SDK_ASICDRV_TEST_PPPOE_SIZE          8
        #define SDK_ASICDRV_TEST_ARP_SIZE            512
        #define SDK_ASICDRV_TEST_NETIF_SIZE          8
        #define SDK_ASICDRV_TEST_ROUTE_SIZE          8
        #define SDK_ASICDRV_TEST_NEXTHOP_SIZE        16
        #define SDK_ASICDRV_TEST_NAPT_OUTBOUND_SIZE  2048
        #define SDK_ASICDRV_TEST_NAPT_INBOUND_SIZE   1024
        #define SDK_ASICDRV_TEST_L34_EXTIP_SIZE      16


        #define SDK_ASICDRV_TEST_ACLACTION_SIZE  128
        #define SDK_ASICDRV_TEST_CFACTION_SIZE   512
        #define SDK_ASICDRV_TEST_CFTBLENTRY_SIZE   128
        #define SDK_ASICDRV_TEST_L2_SIZE         2048


        vlan_entry_t      ApolloVirtualVlanTable[SDK_ASICDRV_TEST_VLAN_SIZE + 1];

        l34_pppoe_entry_t ApolloVirtualPppoeTable[SDK_ASICDRV_TEST_PPPOE_SIZE + 1];
        l34_netif_entry_t ApolloVirtualExtipTable[SDK_ASICDRV_TEST_L34_EXTIP_SIZE + 1];
        l34_arp_entry_t ApolloVirtualArpTable[SDK_ASICDRV_TEST_ARP_SIZE + 1];
        l34_netif_entry_t ApolloVirtualNetifTable[SDK_ASICDRV_TEST_NETIF_SIZE + 1];
        l34_routing_entry_t ApolloVirtualRouteTable[SDK_ASICDRV_TEST_ROUTE_SIZE + 1];
        l34_nexthop_entry_t ApolloVirtualNexthopTable[SDK_ASICDRV_TEST_NEXTHOP_SIZE + 1];
        l34_napt_outband_entry_t ApolloVirtualNaptOutbandTable[SDK_ASICDRV_TEST_NAPT_OUTBOUND_SIZE + 1];
        l34_napt_inband_entry_t ApolloVirtualNaptInbandTable[SDK_ASICDRV_TEST_NAPT_INBOUND_SIZE + 1];


        acl_act_entry_t ApolloVirtualAclActTable[SDK_ASICDRV_TEST_ACLACTION_SIZE + 1];
        acl_rule_entry_t ApolloVirtualAclDataTable[SDK_ASICDRV_TEST_CFACTION_SIZE + 1];
        acl_rule_entry_t ApolloVirtualAclMaskTable[SDK_ASICDRV_TEST_ACLACTION_SIZE + 1];

        cf_act_entry_t ApolloVirtualCfActDsTable[SDK_ASICDRV_TEST_CFACTION_SIZE + 1];
        cf_act_entry_t ApolloVirtualCfActUsTable[SDK_ASICDRV_TEST_CFACTION_SIZE + 1];
        cf_rule_entry_t ApolloVirtualCfDataTable[SDK_ASICDRV_TEST_CFTBLENTRY_SIZE + 1];
        cf_rule_entry_t ApolloVirtualCfMaskTable[SDK_ASICDRV_TEST_CFTBLENTRY_SIZE + 1];
        l2_lut_entry_t ApolloVirtualLutTable[SDK_ASICDRV_TEST_L2_SIZE + 1];
    #endif
#endif /*CONFIG_SDK_APOLLO*/




#ifdef CONFIG_SDK_APOLLOMP
    #define APOLLOMP_L34_TABLE_WORD  (5)
    #define APOLLOMP_L2_TABLE_WORD   (5)
    #define APOLLOMP_LUT_TABLE_WORD  (4)

    #define APOLLOMP_DEFAULT_MIIM_PAGE    (0xA40)

    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_APOLLOMP_TEST_VLAN_SIZE           (RTK_VLAN_ID_MAX+1)
        #define SDK_ASICDRV_APOLLOMP_TEST_PPPOE_SIZE          APOLLOMP_L34_PPPOE_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_ARP_SIZE            APOLLOMP_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NETIF_SIZE          APOLLOMP_L34_NETIF_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_ROUTE_SIZE          APOLLOMP_L34_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NEXTHOP_SIZE        APOLLOMP_L34_NH_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NAPT_OUTBOUND_SIZE  APOLLOMP_L34_NAPT_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NAPT_INBOUND_SIZE   APOLLOMP_L34_NAPTR_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_L34_EXTIP_SIZE      APOLLOMP_L34_EXTIP_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_ACLACTION_SIZE      APOLLOMP_MAX_NUM_OF_ACL_ACTION
     #define SDK_ASICDRV_APOLLOMP_TEST_ROUTE6_SIZE      APOLLOMP_L34_IPV6_ROUTING_TABLE_MAX
     #define SDK_ASICDRV_APOLLOMP_TEST_NBR_SIZE         APOLLOMP_L34_IPV6_NBR_TABLE_MAX
     #define SDK_ASICDRV_APOLLOMP_TEST_BINDING_SIZE     APOLLOMP_L34_BINDING_TABLE_MAX
     #define SDK_ASICDRV_APOLLOMP_TEST_WAN_TYPE_SIZE    APOLLOMP_L34_WAN_TYPE_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE       512
        #define SDK_ASICDRV_APOLLOMP_TEST_CFTBLENTRY_SIZE     128
        #define SDK_ASICDRV_APOLLOMP_TEST_L2_SIZE             (2048+64)


        apollomp_vlan_entry_t      ApollompVirtualVlanTable[SDK_ASICDRV_APOLLOMP_TEST_VLAN_SIZE + 1];

        apollomp_l34_pppoe_entry_t ApollompVirtualPppoeTable[SDK_ASICDRV_APOLLOMP_TEST_PPPOE_SIZE + 1];
        apollomp_l34_netif_entry_t ApollompVirtualExtipTable[SDK_ASICDRV_APOLLOMP_TEST_L34_EXTIP_SIZE + 1];
        apollomp_l34_arp_entry_t ApollompVirtualArpTable[SDK_ASICDRV_APOLLOMP_TEST_ARP_SIZE + 1];
        apollomp_l34_netif_entry_t ApollompVirtualNetifTable[SDK_ASICDRV_APOLLOMP_TEST_NETIF_SIZE + 1];
        apollomp_l34_routing_entry_t ApollompVirtualRouteTable[SDK_ASICDRV_APOLLOMP_TEST_ROUTE_SIZE + 1];
        apollomp_l34_nexthop_entry_t ApollompVirtualNexthopTable[SDK_ASICDRV_APOLLOMP_TEST_NEXTHOP_SIZE + 1];
        apollomp_l34_napt_outband_entry_t ApollompVirtualNaptOutbandTable[SDK_ASICDRV_APOLLOMP_TEST_NAPT_OUTBOUND_SIZE + 1];
        apollomp_l34_napt_inband_entry_t ApollompVirtualNaptInbandTable[SDK_ASICDRV_APOLLOMP_TEST_NAPT_INBOUND_SIZE + 1];
        apollomp_l34_ipv6_routing_entry_t ApollompVirtualRoute6Table[SDK_ASICDRV_APOLLOMP_TEST_ROUTE6_SIZE + 1];
     apollomp_l34_ipv6_neighbor_entry_t ApollompVirtualNeighborTable[SDK_ASICDRV_APOLLOMP_TEST_NBR_SIZE + 1];
     apollomp_l34_binding_entry_t   ApollompVirtualBindingTable[SDK_ASICDRV_APOLLOMP_TEST_BINDING_SIZE + 1];
     apollomp_l34_wan_type_entry_t ApollompVirtualWanTypeTable[SDK_ASICDRV_APOLLOMP_TEST_WAN_TYPE_SIZE + 1];


        apollomp_acl_act_entry_t ApollompVirtualAclActTable[SDK_ASICDRV_APOLLOMP_TEST_ACLACTION_SIZE + 1];
        apollomp_acl_rule_entry_t ApollompVirtualAclDataTable[SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE + 1];
        apollomp_acl_rule_entry_t ApollompVirtualAclMaskTable[SDK_ASICDRV_APOLLOMP_TEST_ACLACTION_SIZE + 1];

        apollomp_cf_act_entry_t ApollompVirtualCfActDsTable[SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE + 1];
        apollomp_cf_act_entry_t ApollompVirtualCfActUsTable[SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE + 1];
        apollomp_cf_rule_entry_t ApollompVirtualCfDataTable[SDK_ASICDRV_APOLLOMP_TEST_CFTBLENTRY_SIZE + 1];
        apollomp_cf_rule_entry_t ApollompVirtualCfMaskTable[SDK_ASICDRV_APOLLOMP_TEST_CFTBLENTRY_SIZE + 1];
        apollomp_l2_lut_entry_t ApollompVirtualLutTable[SDK_ASICDRV_APOLLOMP_TEST_L2_SIZE + 1];
    #endif

#ifdef CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO
    #define APOLLOMP_MII_CLOCK_GPIO_PIN (CONFIG_EXTERNAL_PHY_MII_CLOCK_GPIO_PIN)
    #define APOLLOMP_MII_DATA_GPIO_PIN  (CONFIG_EXTERNAL_PHY_MII_DATA_GPIO_PIN)
#endif

#endif /*CONFIG_SDK_APOLLOMP*/

#ifdef CONFIG_SDK_RTL9601B
    #define RTL9601B_L34_TABLE_WORD  (5)
    #define RTL9601B_L2_TABLE_WORD   (5)
    #define RTL9601B_LUT_TABLE_WORD  (4)

    #define RTL9601B_DEFAULT_MIIM_PAGE    (0xA40)

    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_RTL9601B_TEST_VLAN_SIZE           (RTK_VLAN_ID_MAX+1)
        #define SDK_ASICDRV_RTL9601B_TEST_ACLACTION_SIZE      RTL9601B_MAX_NUM_OF_ACL_ACTION
        #define SDK_ASICDRV_RTL9601B_TEST_CFACTION_SIZE       256
        #define SDK_ASICDRV_RTL9601B_TEST_CFTBLENTRY_SIZE     64
        #define SDK_ASICDRV_RTL9601B_TEST_L2_SIZE             (256+8)


        rtl9601b_vlan_entry_t      Rtl9601bVirtualVlanTable[SDK_ASICDRV_RTL9601B_TEST_VLAN_SIZE + 1];

        rtl9601b_acl_act_entry_t Rtl9601bVirtualAclActTable[SDK_ASICDRV_RTL9601B_TEST_ACLACTION_SIZE + 1];
        rtl9601b_acl_rule_entry_t Rtl9601bVirtualAclDataTable[SDK_ASICDRV_RTL9601B_TEST_ACLACTION_SIZE + 1];
        rtl9601b_acl_rule_entry_t Rtl9601bVirtualAclMaskTable[SDK_ASICDRV_RTL9601B_TEST_ACLACTION_SIZE + 1];

        rtl9601b_cf_act_entry_t Rtl9601bVirtualCfActDsTable[SDK_ASICDRV_RTL9601B_TEST_CFACTION_SIZE + 1];
        rtl9601b_cf_act_entry_t Rtl9601bVirtualCfActUsTable[SDK_ASICDRV_RTL9601B_TEST_CFACTION_SIZE + 1];
        rtl9601b_cf_rule_entry_t Rtl9601bVirtualCfDataTable[SDK_ASICDRV_RTL9601B_TEST_CFTBLENTRY_SIZE + 1];
        rtl9601b_cf_rule_entry_t Rtl9601bVirtualCfMaskTable[SDK_ASICDRV_RTL9601B_TEST_CFTBLENTRY_SIZE + 1];
        rtl9601b_l2_lut_entry_t Rtl9601bVirtualLutTable[SDK_ASICDRV_RTL9601B_TEST_L2_SIZE + 1];
    #endif

#define RTL9601B_MII_CLOCK_GPIO_PIN (60)
#define RTL9601B_MII_DATA_GPIO_PIN  (61)

#endif /*CONFIG_SDK_RTL9601B*/


#ifdef CONFIG_SDK_RTL9602C
    #define RTL9602C_L34_TABLE_WORD  (5)
    #define RTL9602C_L2_TABLE_WORD   (5)
    #define RTL9602C_LUT_TABLE_WORD  (4)

    #define RTL9602C_DEFAULT_MIIM_PAGE    (0xA40)

    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_RTL9602C_TEST_VLAN_SIZE           (RTK_VLAN_ID_MAX+1)
        #define SDK_ASICDRV_RTL9602C_TEST_ACLACTION_SIZE      RTL9602C_MAX_NUM_OF_ACL_ACTION
        #define SDK_ASICDRV_RTL9602C_TEST_CFACTION_SIZE       256
        #define SDK_ASICDRV_RTL9602C_TEST_CFTBLENTRY_SIZE     256
        #define SDK_ASICDRV_RTL9602C_TEST_L2_SIZE             (1024+64)
        #define SDK_ASICDRV_RTL9602C_TEST_PPPOE_SIZE          RTL9602C_L34_PPPOE_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_ARP_SIZE            RTL9602C_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_NETIF_SIZE          RTL9602C_L34_NETIF_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_ROUTE_SIZE          RTL9602C_L34_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_NEXTHOP_SIZE        RTL9602C_L34_NH_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_NAPT_OUTBOUND_SIZE  RTL9602C_L34_NAPT_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_NAPT_INBOUND_SIZE   RTL9602C_L34_NAPTR_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_L34_EXTIP_SIZE      RTL9602C_L34_EXTIP_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_ACLACTION_SIZE      RTL9602C_MAX_NUM_OF_ACL_ACTION
        #define SDK_ASICDRV_RTL9602C_TEST_ROUTE6_SIZE         RTL9602C_L34_IPV6_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_NBR_SIZE            RTL9602C_L34_IPV6_NBR_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_BINDING_SIZE        RTL9602C_L34_BINDING_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_WAN_TYPE_SIZE       RTL9602C_L34_WAN_TYPE_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_ARP_SIZE            RTL9602C_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_RTL9602C_TEST_FLOW_ROUTE_SIZE       RTL9602C_L34_FLOW_ROUTE_TABLE_MAX

        rtl9602c_vlan_entry_t      Rtl9602cVirtualVlanTable[SDK_ASICDRV_RTL9602C_TEST_VLAN_SIZE + 1];

        rtl9602c_acl_act_entry_t Rtl9602cVirtualAclActTable[SDK_ASICDRV_RTL9602C_TEST_ACLACTION_SIZE + 1];
        rtl9602c_acl_rule_entry_t Rtl9602cVirtualAclDataTable[SDK_ASICDRV_RTL9602C_TEST_ACLACTION_SIZE + 1];
        rtl9602c_acl_rule_entry_t Rtl9602cVirtualAclMaskTable[SDK_ASICDRV_RTL9602C_TEST_ACLACTION_SIZE + 1];

        rtl9602c_cf_act_entry_t Rtl9602cVirtualCfActDsTable[SDK_ASICDRV_RTL9602C_TEST_CFACTION_SIZE + 1];
        rtl9602c_cf_act_entry_t Rtl9602cVirtualCfActUsTable[SDK_ASICDRV_RTL9602C_TEST_CFACTION_SIZE + 1];
        rtl9602c_cf_rule_entry_t Rtl9602cVirtualCfDataTable[SDK_ASICDRV_RTL9602C_TEST_CFTBLENTRY_SIZE + 1];
        rtl9602c_cf_rule_entry_t Rtl9602cVirtualCfMaskTable[SDK_ASICDRV_RTL9602C_TEST_CFTBLENTRY_SIZE + 1];
        rtl9602c_l2_lut_entry_t Rtl9602cVirtualLutTable[SDK_ASICDRV_RTL9602C_TEST_L2_SIZE + 1];

        rtl9602c_l34_pppoe_entry_t Rtl9602cVirtualPppoeTable[SDK_ASICDRV_RTL9602C_TEST_PPPOE_SIZE + 1];
        rtl9602c_l34_extip_entry_t Rtl9602cVirtualExtipTable[SDK_ASICDRV_RTL9602C_TEST_L34_EXTIP_SIZE + 1];
        rtl9602c_l34_netif_entry_t Rtl9602cVirtualNetifTable[SDK_ASICDRV_RTL9602C_TEST_NETIF_SIZE + 1];
        rtl9602c_l34_routing_entry_t Rtl9602cVirtualRouteTable[SDK_ASICDRV_RTL9602C_TEST_ROUTE_SIZE + 1];
        rtl9602c_l34_nexthop_entry_t Rtl9602cVirtualNexthopTable[SDK_ASICDRV_RTL9602C_TEST_NEXTHOP_SIZE + 1];
        rtl9602c_l34_napt_outbound_entry_t Rtl9602cVirtualNaptOutboundTable[SDK_ASICDRV_RTL9602C_TEST_NAPT_OUTBOUND_SIZE + 1];
        rtl9602c_l34_napt_inbound_entry_t Rtl9602cVirtualNaptInboundTable[SDK_ASICDRV_RTL9602C_TEST_NAPT_INBOUND_SIZE + 1];
        rtl9602c_l34_ipv6_routing_entry_t Rtl9602cVirtualRoute6Table[SDK_ASICDRV_RTL9602C_TEST_ROUTE6_SIZE + 1];
        rtl9602c_l34_ipv6_neighbor_entry_t Rtl9602cVirtualNeighborTable[SDK_ASICDRV_RTL9602C_TEST_NBR_SIZE + 1];
        rtl9602c_l34_binding_entry_t   Rtl9602cVirtualBindingTable[SDK_ASICDRV_RTL9602C_TEST_BINDING_SIZE + 1];
        rtl9602c_l34_wan_type_entry_t Rtl9602cVirtualWanTypeTable[SDK_ASICDRV_RTL9602C_TEST_WAN_TYPE_SIZE + 1];
        rtl9602c_l34_arp_entry_t Rtl9602cVirtualArpTable[SDK_ASICDRV_RTL9602C_TEST_ARP_SIZE + 1];
        rtl9602c_l34_flow_route_entry_t Rtl9602cVirtualFlowRouteTable[SDK_ASICDRV_RTL9602C_TEST_FLOW_ROUTE_SIZE + 1];
    #endif


#endif /*CONFIG_SDK_RTL9602C*/


#ifdef CONFIG_SDK_RTL9607C
    #define RTL9607C_L34_TABLE_WORD  (5)
    #define RTL9607C_L2_TABLE_WORD   (5)
    #define RTL9607C_LUT_TABLE_WORD  (4)

    #define RTL9607C_DEFAULT_MIIM_PAGE    (0xA40)

    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_RTL9607C_TEST_VLAN_SIZE           (RTK_VLAN_ID_MAX+1)
        #define SDK_ASICDRV_RTL9607C_TEST_ACLACTION_SIZE      128
        #define SDK_ASICDRV_RTL9607C_TEST_CFACTION_SIZE       256
        #define SDK_ASICDRV_RTL9607C_TEST_CFTBLENTRY_SIZE     256
        #define SDK_ASICDRV_RTL9607C_TEST_L2_SIZE             (2048+64)
        #define SDK_ASICDRV_RTL9607C_TEST_PPPOE_SIZE          RTL9607C_L34_PPPOE_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_ARP_SIZE            RTL9607C_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_NETIF_SIZE          RTL9607C_L34_NETIF_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_ROUTE_SIZE          RTL9607C_L34_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_NEXTHOP_SIZE        RTL9607C_L34_NH_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_NAPT_OUTBOUND_SIZE  RTL9607C_L34_NAPT_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_NAPT_INBOUND_SIZE   RTL9607C_L34_NAPTR_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_L34_EXTIP_SIZE      RTL9607C_L34_EXTIP_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_ROUTE6_SIZE         RTL9607C_L34_IPV6_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_NBR_SIZE            RTL9607C_L34_IPV6_NBR_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_BINDING_SIZE        RTL9607C_L34_BINDING_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_WAN_TYPE_SIZE       RTL9607C_L34_WAN_TYPE_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_ARP_SIZE            RTL9607C_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_RTL9607C_TEST_FLOW_ROUTE_SIZE       RTL9607C_L34_FLOW_ROUTE_TABLE_MAX

        rtl9607c_vlan_entry_t      Rtl9607cVirtualVlanTable[SDK_ASICDRV_RTL9607C_TEST_VLAN_SIZE + 1];

        rtl9607c_acl_act_entry_t Rtl9607cVirtualAclActTable[SDK_ASICDRV_RTL9607C_TEST_ACLACTION_SIZE + 1];
        rtl9607c_acl_rule_entry_t Rtl9607cVirtualAclDataTable[SDK_ASICDRV_RTL9607C_TEST_ACLACTION_SIZE + 1];
        rtl9607c_acl_rule_entry_t Rtl9607cVirtualAclMaskTable[SDK_ASICDRV_RTL9607C_TEST_ACLACTION_SIZE + 1];

        rtl9607c_cf_act_entry_t Rtl9607cVirtualCfActDsTable[SDK_ASICDRV_RTL9607C_TEST_CFACTION_SIZE + 1];
        rtl9607c_cf_act_entry_t Rtl9607cVirtualCfActUsTable[SDK_ASICDRV_RTL9607C_TEST_CFACTION_SIZE + 1];
        rtl9607c_cf_rule_entry_t Rtl9607cVirtualCfDataTable[SDK_ASICDRV_RTL9607C_TEST_CFTBLENTRY_SIZE + 1];
        rtl9607c_cf_rule_entry_t Rtl9607cVirtualCfMaskTable[SDK_ASICDRV_RTL9607C_TEST_CFTBLENTRY_SIZE + 1];
        rtl9607c_l2_lut_entry_t Rtl9607cVirtualLutTable[SDK_ASICDRV_RTL9607C_TEST_L2_SIZE + 1];

    #endif


#endif /*CONFIG_SDK_RTL9607C*/


#ifdef CONFIG_SDK_RTL9603CVD
    #define RTL9603CVD_L34_TABLE_WORD  (5)
    #define RTL9603CVD_L2_TABLE_WORD   (5)
    #define RTL9603CVD_LUT_TABLE_WORD  (4)

    #define RTL9603CVD_DEFAULT_MIIM_PAGE    (0xA40)

    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_RTL9603CVD_TEST_VLAN_SIZE           (RTK_VLAN_ID_MAX+1)
        #define SDK_ASICDRV_RTL9603CVD_TEST_ACLACTION_SIZE      128
        #define SDK_ASICDRV_RTL9603CVD_TEST_CFACTION_SIZE       256
        #define SDK_ASICDRV_RTL9603CVD_TEST_CFTBLENTRY_SIZE     256
        #define SDK_ASICDRV_RTL9603CVD_TEST_L2_SIZE             (2048+64)
        #define SDK_ASICDRV_RTL9603CVD_TEST_PPPOE_SIZE          RTL9603CVD_L34_PPPOE_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_ARP_SIZE            RTL9603CVD_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_NETIF_SIZE          RTL9603CVD_L34_NETIF_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_ROUTE_SIZE          RTL9603CVD_L34_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_NEXTHOP_SIZE        RTL9603CVD_L34_NH_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_NAPT_OUTBOUND_SIZE  RTL9603CVD_L34_NAPT_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_NAPT_INBOUND_SIZE   RTL9603CVD_L34_NAPTR_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_L34_EXTIP_SIZE      RTL9603CVD_L34_EXTIP_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_ROUTE6_SIZE         RTL9603CVD_L34_IPV6_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_NBR_SIZE            RTL9603CVD_L34_IPV6_NBR_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_BINDING_SIZE        RTL9603CVD_L34_BINDING_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_WAN_TYPE_SIZE       RTL9603CVD_L34_WAN_TYPE_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_ARP_SIZE            RTL9603CVD_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_RTL9603CVD_TEST_FLOW_ROUTE_SIZE     RTL9603CVD_L34_FLOW_ROUTE_TABLE_MAX

        rtl9603cvd_vlan_entry_t      Rtl9603cvdVirtualVlanTable[SDK_ASICDRV_RTL9603CVD_TEST_VLAN_SIZE + 1];

        rtl9603cvd_acl_act_entry_t Rtl9603cvdVirtualAclActTable[SDK_ASICDRV_RTL9603CVD_TEST_ACLACTION_SIZE + 1];
        rtl9603cvd_acl_rule_entry_t Rtl9603cvdVirtualAclDataTable[SDK_ASICDRV_RTL9603CVD_TEST_ACLACTION_SIZE + 1];
        rtl9603cvd_acl_rule_entry_t Rtl9603cvdVirtualAclMaskTable[SDK_ASICDRV_RTL9603CVD_TEST_ACLACTION_SIZE + 1];

        rtl9603cvd_cf_act_entry_t Rtl9603cvdVirtualCfActDsTable[SDK_ASICDRV_RTL9603CVD_TEST_CFACTION_SIZE + 1];
        rtl9603cvd_cf_act_entry_t Rtl9603cvdVirtualCfActUsTable[SDK_ASICDRV_RTL9603CVD_TEST_CFACTION_SIZE + 1];
        rtl9603cvd_cf_rule_entry_t Rtl9603cvdVirtualCfDataTable[SDK_ASICDRV_RTL9603CVD_TEST_CFTBLENTRY_SIZE + 1];
        rtl9603cvd_cf_rule_entry_t Rtl9603cvdVirtualCfMaskTable[SDK_ASICDRV_RTL9603CVD_TEST_CFTBLENTRY_SIZE + 1];
        rtl9603cvd_l2_lut_entry_t Rtl9603cvdVirtualLutTable[SDK_ASICDRV_RTL9603CVD_TEST_L2_SIZE + 1];

    #endif


#endif /*CONFIG_SDK_RTL9603CVD*/


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
#ifdef CONFIG_SDK_APOLLO
    int32 apollo_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollo_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollo_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollo_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollo_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 apollo_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 lutStToTblData(apollo_lut_table_t *pL2Table, uint32 *pTblData);
    int32 tblDataToLutSt(apollo_lut_table_t *pL2Table, uint32 *pTblData);

    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _apollo_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _apollo_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

#endif /*#ifdef CONFIG_SDK_APOLLO*/


#ifdef CONFIG_SDK_APOLLOMP
    int32 apollomp_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollomp_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollomp_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollomp_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollomp_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 apollomp_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 apollomp_lutStToTblData(apollomp_lut_table_t *pL2Table, uint32 *pTblData);
    int32 apollomp_tblDataToLutSt(apollomp_lut_table_t *pL2Table, uint32 *pTblData);

    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _apollomp_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _apollomp_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

static uint32 apollomp_ext_phyid = 0xFFFF;

#endif /*#ifdef CONFIG_SDK_APOLLOMP*/


#ifdef CONFIG_SDK_RTL9601B

    int32 rtl9601b_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9601b_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9601b_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 rtl9601b_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    static int32 rtl9601b_lutStToTblData(rtl9601b_lut_table_t *pL2Table, uint32 *pTblData);
    static int32 rtl9601b_tblDataToLutSt(rtl9601b_lut_table_t *pL2Table, uint32 *pTblData);
    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _rtl9601b_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _rtl9601b_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

#endif /*#ifdef CONFIG_SDK_RTL9601B*/



#ifdef CONFIG_SDK_RTL9602C
    int32 rtl9602c_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9602c_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9602c_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9602c_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9602c_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 rtl9602c_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    static int32 rtl9602c_lutStToTblData(rtl9602c_lut_table_t *pL2Table, uint32 *pTblData);
    static int32 rtl9602c_tblDataToLutSt(rtl9602c_lut_table_t *pL2Table, uint32 *pTblData);
    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _rtl9602c_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _rtl9602c_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

#endif /*#ifdef CONFIG_SDK_RTL9602C*/



#ifdef CONFIG_SDK_RTL9607C
    int32 rtl9607c_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9607c_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9607c_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9607c_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9607c_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 rtl9607c_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    static int32 rtl9607c_lutStToTblData(rtl9607c_lut_table_t *pL2Table, uint32 *pTblData);
    static int32 rtl9607c_tblDataToLutSt(rtl9607c_lut_table_t *pL2Table, uint32 *pTblData);
    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _rtl9607c_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _rtl9607c_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

    extern int32
    rtl9607c_extPhy_read(
        uint32      phyID,
        uint32      phyRegAddr,
        uint16      *pData);

    extern int32
    rtl9607c_extPhy_write(
        uint32      phyID,
        uint32      phyRegAddr,
        uint16      data);

    #ifdef CONFIG_EXTERNAL_PHY_POLLING
    static uint32 rtl9607c_ext_phyid = 0xFFFF;
    #endif
#endif /*#ifdef CONFIG_SDK_RTL9607C*/


#ifdef CONFIG_SDK_RTL9603CVD
    int32 rtl9603cvd_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9603cvd_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9603cvd_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 rtl9603cvd_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 rtl9603cvd_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 rtl9603cvd_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    static int32 rtl9603cvd_lutStToTblData(rtl9603cvd_lut_table_t *pL2Table, uint32 *pTblData);
    static int32 rtl9603cvd_tblDataToLutSt(rtl9603cvd_lut_table_t *pL2Table, uint32 *pTblData);
    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _rtl9603cvd_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _rtl9603cvd_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

#endif /*#ifdef CONFIG_SDK_RTL9603CVD*/


/* Function Name:
 *      table_find
 * Description:
 *      Find this kind of table structure in this specified chip.
 * Input:
 *      table - table index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of table structure that found
 * Note:
 *      None
 */
rtk_table_t *
table_find (uint32 table)
{

    return &(hal_ctrl.pChip_driver->pTable_list[table]);
} /* end of table_find */


#if defined(CONFIG_SDK_ASICDRV_TEST)
#ifdef CONFIG_SDK_APOLLO
int32 _apollo_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
    switch (table)
    {
        case VLANt:
            ptr = (uint32 *)&ApolloVirtualVlanTable[addr];
            break;

        case ACL_ACTIONt:
            ptr = (uint32 *)&ApolloVirtualAclActTable[addr];
            break;

        case ACL_DATAt:
        case ACL_DATA2t:
            ptr = (uint32 *)&ApolloVirtualAclDataTable[addr];
            break;

        case ACL_MASKt:
        case ACL_MASK2t:
            ptr = (uint32 *)&ApolloVirtualAclMaskTable[addr];
            break;

        case CF_MASKt:
            ptr = (uint32 *)&ApolloVirtualCfMaskTable[addr];
            break;

        case CF_RULEt:
            ptr = (uint32 *)&ApolloVirtualCfDataTable[addr];
            break;

        case CF_ACTION_DSt:
            ptr = (uint32 *)&ApolloVirtualCfActDsTable[addr];
            break;

        case CF_ACTION_USt:
            ptr = (uint32 *)&ApolloVirtualCfActUsTable[addr];
            break;

        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualLutTable[addr];
            break;
        case EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualExtipTable[addr];
            break;

        case PPPOE_TABLEt:
            ptr = (uint32 *)&ApolloVirtualPppoeTable[addr];
            break;

        case ARP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualArpTable[addr];
            break;
        case L3_ROUTING_DROP_TRAPt:
        case L3_ROUTING_GLOBAL_ROUTEt:
        case L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualRouteTable[addr];
            break;


        case NAPT_TABLEt:
            ptr =(uint32 *) &ApolloVirtualNaptOutbandTable[addr];
            break;

        case NAPTR_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNaptInbandTable[addr];
            break;

        case NETIFt:
            ptr = (uint32 *)&ApolloVirtualNetifTable[addr];
            break;

        case NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNexthopTable[addr];
            break;

        default:
            return RT_ERR_INPUT;
    }

    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;

}

int32 _apollo_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case VLANt:
            ptr = (uint32 *)&ApolloVirtualVlanTable[addr];
            break;
        case ACL_ACTIONt:
            ptr = (uint32 *)&ApolloVirtualAclActTable[addr];
            break;
        case ACL_DATAt:
        case ACL_DATA2t:
            ptr = (uint32 *)&ApolloVirtualAclDataTable[addr];
            break;
        case ACL_MASKt:
        case ACL_MASK2t:
            ptr = (uint32 *)&ApolloVirtualAclMaskTable[addr];
            break;
        case CF_MASKt:
            ptr = (uint32 *)&ApolloVirtualCfMaskTable[addr];
            break;
        case CF_RULEt:
            ptr = (uint32 *)&ApolloVirtualCfDataTable[addr];
            break;
        case CF_ACTION_DSt:
            ptr = (uint32 *)&ApolloVirtualCfActDsTable[addr];
            break;
        case CF_ACTION_USt:
            ptr = (uint32 *)&ApolloVirtualCfActUsTable[addr];
            break;
        case PPPOE_TABLEt:
            ptr = (uint32 *)&ApolloVirtualPppoeTable[addr];
            break;
        case EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualExtipTable[addr];
            break;
        case ARP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualArpTable[addr];
            break;
        case L3_ROUTING_DROP_TRAPt:
        case L3_ROUTING_GLOBAL_ROUTEt:
        case L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualRouteTable[addr];
            break;
        case NAPT_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNaptOutbandTable[addr];
            break;
        case NAPTR_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNaptInbandTable[addr];
            break;
        case NETIFt:
            ptr = (uint32 *)&ApolloVirtualNetifTable[addr];
            break;
        case NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNexthopTable[addr];
            break;
        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualLutTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));
    return RT_ERR_OK;
}

#endif /*CONFIG_SDK_APOLLO*/

#ifdef CONFIG_SDK_APOLLOMP
int32 _apollomp_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
    switch (table)
    {
        case APOLLOMP_VLANt:
            ptr = (uint32 *)&ApollompVirtualVlanTable[addr];
            break;
        case APOLLOMP_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&ApollompVirtualAclActTable[addr];
            break;
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
            ptr = (uint32 *)&ApollompVirtualAclDataTable[addr];
            break;
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
            ptr = (uint32 *)&ApollompVirtualAclMaskTable[addr];
            break;
        case APOLLOMP_CF_MASKt:
            ptr = (uint32 *)&ApollompVirtualCfMaskTable[addr];
            break;
        case APOLLOMP_CF_RULEt:
            ptr = (uint32 *)&ApollompVirtualCfDataTable[addr];
            break;
        case APOLLOMP_CF_ACTION_DSt:
            ptr = (uint32 *)&ApollompVirtualCfActDsTable[addr];
            break;
        case APOLLOMP_CF_ACTION_USt:
            ptr = (uint32 *)&ApollompVirtualCfActUsTable[addr];
            break;
        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualLutTable[addr];
            break;
        case APOLLOMP_EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualExtipTable[addr];
            break;
        case APOLLOMP_PPPOE_TABLEt:
            ptr = (uint32 *)&ApollompVirtualPppoeTable[addr];
            break;
        case APOLLOMP_ARP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualArpTable[addr];
            break;
        case APOLLOMP_L3_ROUTING_DROP_TRAPt:
        case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
        case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualRouteTable[addr];
            break;
        case APOLLOMP_NAPT_TABLEt:
            ptr =(uint32 *) &ApollompVirtualNaptOutbandTable[addr];
            break;
        case APOLLOMP_NAPTR_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNaptInbandTable[addr];
            break;
        case APOLLOMP_NETIFt:
            ptr = (uint32 *)&ApollompVirtualNetifTable[addr];
            break;
        case APOLLOMP_NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNexthopTable[addr];
            break;
        case APOLLOMP_IPV6_ROUTING_TABLEt:
        ptr = (uint32 *)&ApollompVirtualRoute6Table[addr];
        break;
        case APOLLOMP_NEIGHBOR_TABLEt:
        ptr = (uint32 *)&ApollompVirtualNeighborTable[addr];
        break;
        case APOLLOMP_BINDING_TABLEt:
        ptr = (uint32 *)&ApollompVirtualBindingTable[addr];
        break;
        case APOLLOMP_WAN_TYPE_TABLEt:
        ptr = (uint32 *)&ApollompVirtualWanTypeTable[addr];
        break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

int32 _apollomp_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case APOLLOMP_VLANt:
            ptr = (uint32 *)&ApollompVirtualVlanTable[addr];
            break;
        case APOLLOMP_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&ApollompVirtualAclActTable[addr];
            break;
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
            ptr = (uint32 *)&ApollompVirtualAclDataTable[addr];
            break;
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
            ptr = (uint32 *)&ApollompVirtualAclMaskTable[addr];
            break;
        case APOLLOMP_CF_MASKt:
            ptr = (uint32 *)&ApollompVirtualCfMaskTable[addr];
            break;
        case APOLLOMP_CF_RULEt:
            ptr = (uint32 *)&ApollompVirtualCfDataTable[addr];
            break;
        case APOLLOMP_CF_ACTION_DSt:
            ptr = (uint32 *)&ApollompVirtualCfActDsTable[addr];
            break;
        case APOLLOMP_CF_ACTION_USt:
            ptr = (uint32 *)&ApollompVirtualCfActUsTable[addr];
            break;
        case APOLLOMP_PPPOE_TABLEt:
            ptr = (uint32 *)&ApollompVirtualPppoeTable[addr];
            break;
        case APOLLOMP_EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualExtipTable[addr];
            break;
        case APOLLOMP_ARP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualArpTable[addr];
            break;
        case APOLLOMP_L3_ROUTING_DROP_TRAPt:
        case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
        case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualRouteTable[addr];
            break;
        case APOLLOMP_NAPT_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNaptOutbandTable[addr];
            break;
        case APOLLOMP_NAPTR_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNaptInbandTable[addr];
            break;
        case APOLLOMP_NETIFt:
            ptr = (uint32 *)&ApollompVirtualNetifTable[addr];
            break;
        case APOLLOMP_NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNexthopTable[addr];
            break;
        case APOLLOMP_IPV6_ROUTING_TABLEt:
        ptr = (uint32 *)&ApollompVirtualRoute6Table[addr];
        break;
        case APOLLOMP_NEIGHBOR_TABLEt:
        ptr = (uint32 *)&ApollompVirtualNeighborTable[addr];
        break;
        case APOLLOMP_BINDING_TABLEt:
        ptr = (uint32 *)&ApollompVirtualBindingTable[addr];
        break;
        case APOLLOMP_WAN_TYPE_TABLEt:
        ptr = (uint32 *)&ApollompVirtualWanTypeTable[addr];
        break;
        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualLutTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));
    return RT_ERR_OK;
}

#endif/*CONFIG_SDK_APOLLOMP*/

#ifdef CONFIG_SDK_RTL9601B
int32 _rtl9601b_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
    switch (table)
    {
        case RTL9601B_VLANt:
            ptr = (uint32 *)&Rtl9601bVirtualVlanTable[addr];
            break;
        case RTL9601B_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9601bVirtualAclActTable[addr];
            break;
        case RTL9601B_ACL_DATAt:
            ptr = (uint32 *)&Rtl9601bVirtualAclDataTable[addr];
            break;
        case RTL9601B_ACL_MASKt:
            ptr = (uint32 *)&Rtl9601bVirtualAclMaskTable[addr];
            break;
        case RTL9601B_CF_MASKt:
        case RTL9601B_CF_MASK_L34t:
            ptr = (uint32 *)&Rtl9601bVirtualCfMaskTable[addr];
            break;
        case RTL9601B_CF_RULEt:
        case RTL9601B_CF_RULE_ENHANCEDt:
            ptr = (uint32 *)&Rtl9601bVirtualCfDataTable[addr];
            break;
        case RTL9601B_CF_ACTION_DSt:
            ptr = (uint32 *)&Rtl9601bVirtualCfActDsTable[addr];
            break;
        case RTL9601B_CF_ACTION_USt:
            ptr = (uint32 *)&Rtl9601bVirtualCfActUsTable[addr];
            break;
        case RTL9601B_L2_MC_DSLt:
        case RTL9601B_L2_UCt:
        case RTL9601B_L3_MC_DSLt:
            ptr = (uint32 *)&Rtl9601bVirtualLutTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }



    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

int32 _rtl9601b_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case RTL9601B_VLANt:
            ptr = (uint32 *)&Rtl9601bVirtualVlanTable[addr];
            break;
        case RTL9601B_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9601bVirtualAclActTable[addr];
            break;
        case RTL9601B_ACL_DATAt:
            ptr = (uint32 *)&Rtl9601bVirtualAclDataTable[addr];
            break;
        case RTL9601B_ACL_MASKt:
            ptr = (uint32 *)&Rtl9601bVirtualAclMaskTable[addr];
            break;
        case RTL9601B_CF_MASKt:
        case RTL9601B_CF_MASK_L34t:
            ptr = (uint32 *)&Rtl9601bVirtualCfMaskTable[addr];
            break;
        case RTL9601B_CF_RULEt:
        case RTL9601B_CF_RULE_ENHANCEDt:
            ptr = (uint32 *)&Rtl9601bVirtualCfDataTable[addr];
            break;
        case RTL9601B_CF_ACTION_DSt:
            ptr = (uint32 *)&Rtl9601bVirtualCfActDsTable[addr];
            break;
        case RTL9601B_CF_ACTION_USt:
            ptr = (uint32 *)&Rtl9601bVirtualCfActUsTable[addr];
            break;
        case RTL9601B_L2_MC_DSLt:
        case RTL9601B_L2_UCt:
        case RTL9601B_L3_MC_DSLt:
            ptr = (uint32 *)&Rtl9601bVirtualLutTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

#endif/*CONFIG_SDK_RTL9601B*/


#ifdef CONFIG_SDK_RTL9602C
int32 _rtl9602c_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
    switch (table)
    {
        case RTL9602C_VLANt:
            ptr = (uint32 *)&Rtl9602cVirtualVlanTable[addr];
            break;
        case RTL9602C_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualAclActTable[addr];
            break;
        case RTL9602C_ACL_DATAt:
            ptr = (uint32 *)&Rtl9602cVirtualAclDataTable[addr];
            break;
        case RTL9602C_ACL_MASKt:
            ptr = (uint32 *)&Rtl9602cVirtualAclMaskTable[addr];
            break;
        case RTL9602C_CF_MASK_48_P0t:
        case RTL9602C_CF_MASK_48_P1_T0t:
        case RTL9602C_CF_MASK_48_P1_T1t:
            ptr = (uint32 *)&Rtl9602cVirtualCfMaskTable[addr];
            break;
        case RTL9602C_CF_RULE_48_P0t:
        case RTL9602C_CF_RULE_48_P1_T0t:
        case RTL9602C_CF_RULE_48_P1_T1t:
            ptr = (uint32 *)&Rtl9602cVirtualCfDataTable[addr];
            break;
        case RTL9602C_CF_ACTION_DSt:
            ptr = (uint32 *)&Rtl9602cVirtualCfActDsTable[addr];
            break;
        case RTL9602C_CF_ACTION_USt:
            ptr = (uint32 *)&Rtl9602cVirtualCfActUsTable[addr];
            break;
        case RTL9602C_L2_MC_DSLt:
        case RTL9602C_L2_UCt:
        case RTL9602C_L3_MC_ROUTEt:
        case RTL9602C_L3_IP6MCt:
            ptr = (uint32 *)&Rtl9602cVirtualLutTable[addr];
            break;
        case RTL9602C_PPPOE_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualPppoeTable[addr];
            break;
        case RTL9602C_EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualExtipTable[addr];
            break;
        case RTL9602C_ARP_CAM_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualArpTable[addr];
            break;
        case RTL9602C_L3_ROUTING_DROP_TRAPt:
        case RTL9602C_L3_ROUTING_GLOBAL_ROUTEt:
        case RTL9602C_L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&Rtl9602cVirtualRouteTable[addr];
            break;
        case RTL9602C_NAPT_TABLEt:
            ptr =(uint32 *) &Rtl9602cVirtualNaptOutboundTable[addr];
            break;
        case RTL9602C_NAPTR_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNaptInboundTable[addr];
            break;
        case RTL9602C_NETIFt:
            ptr = (uint32 *)&Rtl9602cVirtualNetifTable[addr];
            break;
        case RTL9602C_NEXT_HOP_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNexthopTable[addr];
            break;
        case RTL9602C_IPV6_ROUTING_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualRoute6Table[addr];
            break;
        case RTL9602C_NEIGHBOR_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNeighborTable[addr];
            break;
        case RTL9602C_BINDING_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualBindingTable[addr];
            break;
        case RTL9602C_WAN_TYPE_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualWanTypeTable[addr];
            break;
        case RTL9602C_FLOW_ROUTING_TABLE_IPV4t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt:
            ptr = (uint32 *)&Rtl9602cVirtualFlowRouteTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }



    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

int32 _rtl9602c_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case RTL9602C_VLANt:
            ptr = (uint32 *)&Rtl9602cVirtualVlanTable[addr];
            break;
        case RTL9602C_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualAclActTable[addr];
            break;
        case RTL9602C_ACL_DATAt:
            ptr = (uint32 *)&Rtl9602cVirtualAclDataTable[addr];
            break;
        case RTL9602C_ACL_MASKt:
            ptr = (uint32 *)&Rtl9602cVirtualAclMaskTable[addr];
            break;
        case RTL9602C_CF_MASK_48_P0t:
        case RTL9602C_CF_MASK_48_P1_T0t:
        case RTL9602C_CF_MASK_48_P1_T1t:
            ptr = (uint32 *)&Rtl9602cVirtualCfMaskTable[addr];
            break;
        case RTL9602C_CF_RULE_48_P0t:
        case RTL9602C_CF_RULE_48_P1_T0t:
        case RTL9602C_CF_RULE_48_P1_T1t:
            ptr = (uint32 *)&Rtl9602cVirtualCfDataTable[addr];
            break;
        case RTL9602C_CF_ACTION_DSt:
            ptr = (uint32 *)&Rtl9602cVirtualCfActDsTable[addr];
            break;
        case RTL9602C_CF_ACTION_USt:
            ptr = (uint32 *)&Rtl9602cVirtualCfActUsTable[addr];
            break;
        case RTL9602C_L2_MC_DSLt:
        case RTL9602C_L2_UCt:
        case RTL9602C_L3_MC_ROUTEt:
        case RTL9602C_L3_IP6MCt:
            ptr = (uint32 *)&Rtl9602cVirtualLutTable[addr];
            break;
        case RTL9602C_PPPOE_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualPppoeTable[addr];
            break;
        case RTL9602C_EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualExtipTable[addr];
            break;
        case RTL9602C_ARP_CAM_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualArpTable[addr];
            break;
        case RTL9602C_L3_ROUTING_DROP_TRAPt:
        case RTL9602C_L3_ROUTING_GLOBAL_ROUTEt:
        case RTL9602C_L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&Rtl9602cVirtualRouteTable[addr];
            break;
        case RTL9602C_NAPT_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNaptOutboundTable[addr];
            break;
        case RTL9602C_NAPTR_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNaptInboundTable[addr];
            break;
        case RTL9602C_NETIFt:
            ptr = (uint32 *)&Rtl9602cVirtualNetifTable[addr];
            break;
        case RTL9602C_NEXT_HOP_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNexthopTable[addr];
            break;
        case RTL9602C_IPV6_ROUTING_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualRoute6Table[addr];
            break;
        case RTL9602C_NEIGHBOR_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualNeighborTable[addr];
            break;
        case RTL9602C_BINDING_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualBindingTable[addr];
            break;
        case RTL9602C_WAN_TYPE_TABLEt:
            ptr = (uint32 *)&Rtl9602cVirtualWanTypeTable[addr];
            break;
        case RTL9602C_FLOW_ROUTING_TABLE_IPV4t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt:
            ptr = (uint32 *)&Rtl9602cVirtualFlowRouteTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

#endif/*CONFIG_SDK_RTL9602C*/


#ifdef CONFIG_SDK_RTL9607C
int32 _rtl9607c_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
    switch (table)
    {
        case RTL9607C_VLANt:
            ptr = (uint32 *)&Rtl9607cVirtualVlanTable[addr];
            break;
        case RTL9607C_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9607cVirtualAclActTable[addr];
            break;
        case RTL9607C_ACL_DATAt:
            ptr = (uint32 *)&Rtl9607cVirtualAclDataTable[addr];
            break;
        case RTL9607C_ACL_MASKt:
            ptr = (uint32 *)&Rtl9607cVirtualAclMaskTable[addr];
            break;
        case RTL9607C_CF_MASK_T0t:
        case RTL9607C_CF_MASK_T1t:
        case RTL9607C_CF_MASK_T2t:
            ptr = (uint32 *)&Rtl9607cVirtualCfMaskTable[addr];
            break;
        case RTL9607C_CF_RULE_T0t:
        case RTL9607C_CF_RULE_T1t:
        case RTL9607C_CF_RULE_T2t:
            ptr = (uint32 *)&Rtl9607cVirtualCfDataTable[addr];
            break;
        case RTL9607C_CF_ACTION_DSt:
            ptr = (uint32 *)&Rtl9607cVirtualCfActDsTable[addr];
            break;
        case RTL9607C_CF_ACTION_USt:
            ptr = (uint32 *)&Rtl9607cVirtualCfActUsTable[addr];
            break;
        case RTL9607C_L2_MC_DSLt:
        case RTL9607C_L2_UCt:
        case RTL9607C_L3_MCt:
            ptr = (uint32 *)&Rtl9607cVirtualLutTable[addr];
            break;

        default:
            return RT_ERR_INPUT;
    }



    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

int32 _rtl9607c_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case RTL9607C_VLANt:
            ptr = (uint32 *)&Rtl9607cVirtualVlanTable[addr];
            break;
        case RTL9607C_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9607cVirtualAclActTable[addr];
            break;
        case RTL9607C_ACL_DATAt:
            ptr = (uint32 *)&Rtl9607cVirtualAclDataTable[addr];
            break;
        case RTL9607C_ACL_MASKt:
            ptr = (uint32 *)&Rtl9607cVirtualAclMaskTable[addr];
            break;
        case RTL9607C_CF_MASK_T0t:
        case RTL9607C_CF_MASK_T1t:
        case RTL9607C_CF_MASK_T2t:
            ptr = (uint32 *)&Rtl9607cVirtualCfMaskTable[addr];
            break;
        case RTL9607C_CF_RULE_T0t:
        case RTL9607C_CF_RULE_T1t:
        case RTL9607C_CF_RULE_T2t:
            ptr = (uint32 *)&Rtl9607cVirtualCfDataTable[addr];
            break;
        case RTL9607C_CF_ACTION_DSt:
            ptr = (uint32 *)&Rtl9607cVirtualCfActDsTable[addr];
            break;
        case RTL9607C_CF_ACTION_USt:
            ptr = (uint32 *)&Rtl9607cVirtualCfActUsTable[addr];
            break;
        case RTL9607C_L2_MC_DSLt:
        case RTL9607C_L2_UCt:
        case RTL9607C_L3_MCt:
            ptr = (uint32 *)&Rtl9607cVirtualLutTable[addr];
            break;

        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

#endif/*CONFIG_SDK_RTL9607C*/


#ifdef CONFIG_SDK_RTL9603CVD
int32 _rtl9603cvd_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
    switch (table)
    {
        case RTL9603CVD_VLANt:
            ptr = (uint32 *)&Rtl9603cvdVirtualVlanTable[addr];
            break;
        case RTL9603CVD_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9603cvdVirtualAclActTable[addr];
            break;
        case RTL9603CVD_ACL_DATAt:
            ptr = (uint32 *)&Rtl9603cvdVirtualAclDataTable[addr];
            break;
        case RTL9603CVD_ACL_MASKt:
            ptr = (uint32 *)&Rtl9603cvdVirtualAclMaskTable[addr];
            break;


        case RTL9603CVD_L2_MC_DSLt:
        case RTL9603CVD_L2_UCt:
        case RTL9603CVD_L3_MCt:
            ptr = (uint32 *)&Rtl9603cvdVirtualLutTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }



    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

int32 _rtl9603cvd_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case RTL9603CVD_VLANt:
            ptr = (uint32 *)&Rtl9603cvdVirtualVlanTable[addr];
            break;
        case RTL9603CVD_ACL_ACTION_TABLEt:
            ptr = (uint32 *)&Rtl9603cvdVirtualAclActTable[addr];
            break;
        case RTL9603CVD_ACL_DATAt:
            ptr = (uint32 *)&Rtl9603cvdVirtualAclDataTable[addr];
            break;
        case RTL9603CVD_ACL_MASKt:
            ptr = (uint32 *)&Rtl9603cvdVirtualAclMaskTable[addr];
            break;

        case RTL9603CVD_L2_MC_DSLt:
        case RTL9603CVD_L2_UCt:
        case RTL9603CVD_L3_MCt:
            ptr = (uint32 *)&Rtl9603cvdVirtualLutTable[addr];
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

#endif/*CONFIG_SDK_RTL9603CVD*/


#endif /*CONFIG_SDK_ASICDRV_TEST*/


#ifdef CONFIG_SDK_APOLLO

static int32 _appolo_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(TBL_ACCESS_STSr, BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table    -  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 lutStToTblData(apollo_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);


    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_set(L2_UCt, L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_set(L2_UCt, L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_set(L2_UCt, L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(L2_UCt, L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(L2_UCt, L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*--- L2 ---*/
    if (pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L2UC || pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L2MC_DSL)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(L2_UCt, L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(L2_UCt, L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_set(L2_UCt, L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }

    /*--- L3 ---*/
    if (pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L3MC_DSL || pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L3MC_ROUTE)
    {

        /*GIP*/
        if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if (pL2Table->table_type != RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*DSL_MBR*/
        if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_DSL_MBRtf, &pL2Table->dsl_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }


    }
    switch ( pL2Table->table_type )
    {
        case RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(L2_UCt, L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_set(L2_UCt, L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_set(L2_UCt, L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(L2_UCt, L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(L2_UCt, L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_set(L2_UCt, L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(L2_UCt, L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(L2_UCt, L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_set(L2_UCt, L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(L2_UCt, L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;

        case RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            break;

        case RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*L3 Translation Index*/
            if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*EXT_FR*/
            if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*WAN_SA*/
            if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
}

/* Function Name:
 *      tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table    -  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 tblDataToLutSt(apollo_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_get(L2_UCt, L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_get(L2_UCt, L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_get(L2_UCt, L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(L2_UCt, L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(L2_UCt, L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    if (pL2Table->l3lookup==0) /*L2*/
    {

        /*MAC*/
        if ((ret = table_field_mac_get(L2_UCt, L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CVID / CVID_FID*/
        if ((ret = table_field_get(L2_UCt, L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_get(L2_UCt, L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        else
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L2UC;


    }
    else
    {/*L3*/

        /*GIP*/
        if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(pL2Table->gip_only)
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        else
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L3MC_DSL;

    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if(pL2Table->table_type != RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*DSL_MBR*/
        if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_DSL_MBRtf, &pL2Table->dsl_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }


    }
    switch(pL2Table->table_type)
    {
        case RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(L2_UCt, L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_get(L2_UCt, L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_get(L2_UCt, L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(L2_UCt, L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(L2_UCt, L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_get(L2_UCt, L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(L2_UCt, L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(L2_UCt, L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_get(L2_UCt, L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(L2_UCt, L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;


        case RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*WAN_SA*/
            if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*L3 Translation Index*/
            if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_FR*/
            if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      apollo_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollo_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    uint32      tableData[APOLLO_LUT_TABLE_WORD];
    pLutSt = (apollo_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    lutStToTblData((apollo_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }
            if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;

        case RAW_LUT_READ_METHOD_ADDRESS:
        case RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case RAW_LUT_READ_METHOD_NEXT_L2UC:
        case RAW_LUT_READ_METHOD_NEXT_L2MC:
        case RAW_LUT_READ_METHOD_NEXT_L3MC:
        case RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }

             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(TBL_ACCESS_CTRLr, SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    tblDataToLutSt((apollo_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(TBL_ACCESS_STSr, HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
        pLutSt = (apollo_lut_table_t *)pData;
        _apollo_drv_virtualTable_read(L2_UCt,pTable,addr,tableData);
        tblDataToLutSt((apollo_lut_table_t *)pData, tableData);
        pLutSt->lookup_hit = 1;
        pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of apollo_lut_table_read */


/* Function Name:
 *      apollo_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    /*for ACL and CF data and mask use the same table,but different index*/
#if 0
    if (table == ACL_MASKt || table == ACL_MASK2t || table == CF_MASKt)
        addr+= pTable->size;
#endif
    if (table == ACL_DATAt || table == ACL_DATA2t || table == CF_RULEt)
        addr+= pTable->size;



    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data =0;

    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data =pTable->type;

    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }



#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollo_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of apollo_l2_table_read */



/* Function Name:
 *      apollo_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "apollo_table_read table=%d, addr=0x%x", table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case ARP_TABLEt:
        case EXTERNAL_IP_TABLEt:
        case L3_ROUTING_DROP_TRAPt:
        case L3_ROUTING_GLOBAL_ROUTEt:
        case L3_ROUTING_LOCAL_ROUTEt:
        case NAPT_TABLEt:
        case NAPTR_TABLEt:
        case NETIFt:
        case NEXT_HOP_TABLEt:
        case PPPOE_TABLEt:
            return apollo_l34_table_read(table, pTable, addr, pData);
            break;

        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            return apollo_lut_table_read(pTable ,addr ,pData);
            break;

        case ACL_ACTIONt:
        case ACL_DATAt:
        case ACL_DATA2t:
        case ACL_MASKt:
        case ACL_MASK2t:
        case CF_ACTION_DSt:
        case CF_ACTION_USt:
        case CF_MASKt:
        case CF_RULEt:
        case VLANt:
            return apollo_l2_table_read(table, pTable ,addr ,pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
} /* end of apollo_table_read */



/* Function Name:
 *      apollo_l34_table_write
 * Description:
 *      Write one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l34_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollo_drv_virtualTable_write(table,pTable,addr,pData);
#else
    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLO_L34_TABLE_WORD];

    if(table);

    /* initialize variable */
    reg_data = 0;
    busy = 0;
    osal_memset(l34_table_data, 0, sizeof(l34_table_data));


     for (i = 0 ; i < APOLLO_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        l34_table_data[APOLLO_L34_TABLE_WORD - i -1] = pData[pTable->datareg_num-i-1];
    }

    /* Write pre-configure table data to indirect data register */
    if ((ret = reg_write(NAT_TBL_ACCESS_WRDATAr, l34_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Table access operation
     */
    field_data =1;

    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, WR_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(NAT_TBL_ACCESS_CTRLr, WR_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

#endif  /*defined(CONFIG_SDK_ASICDRV_TEST)*/

    return RT_ERR_OK;
}/* end of apollo_l34_table_write */
/* Function Name:
 *      apollo_l34_table_read
 * Description:
 *      Read one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l34_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollo_drv_virtualTable_read(table,pTable,addr,pData);
#else
    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLO_L34_TABLE_WORD];
    /* initialize variable */
    reg_data = 0;
    busy = 0;

    if(table);

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    /* Table access operation
     */
    field_data = 1;

    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, RD_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */

    if ((ret = reg_write(NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(NAT_TBL_ACCESS_CTRLr, RD_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read table data from indirect data register */
    if ((ret = reg_read(NAT_TBL_ACCESS_RDDATAr, l34_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < APOLLO_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        pData[pTable->datareg_num - i - 1]= l34_table_data[APOLLO_L34_TABLE_WORD - i - 1];
    }
#endif

    return RT_ERR_OK;
}/* end of apollo_l34_table_read */



/* Function Name:
 *      apollo_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollo_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      tableData[APOLLO_L2_TABLE_WORD];
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (apollo_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    lutStToTblData((apollo_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }


    if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(TBL_ACCESS_STSr, HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (apollo_lut_table_t *)pData;
    lutStToTblData((apollo_lut_table_t *)pData, tableData);
    _apollo_drv_virtualTable_write(L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */
    return RT_ERR_OK;
}/* end of apollo_lut_table_write */


/* Function Name:
 *      apollo_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
#if 0
    if (table == ACL_MASKt || table == ACL_MASK2t || table == CF_MASKt)
        addr+= pTable->size;
#endif
    if (table == ACL_DATAt || table == ACL_DATA2t || table == CF_RULEt)
        addr+= pTable->size;


    if ((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }
    if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

    _apollo_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of apollo_l2_table_write */


/* Function Name:
 *      apollo_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
apollo_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case ARP_TABLEt:
        case EXTERNAL_IP_TABLEt:
        case L3_ROUTING_DROP_TRAPt:
        case L3_ROUTING_GLOBAL_ROUTEt:
        case L3_ROUTING_LOCAL_ROUTEt:
        case NAPT_TABLEt:
        case NAPTR_TABLEt:
        case NETIFt:
        case NEXT_HOP_TABLEt:
        case PPPOE_TABLEt:
            return apollo_l34_table_write(table, pTable, addr, pData);
            break;

        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            return apollo_lut_table_write(pTable, addr, pData);

            break;

        case ACL_ACTIONt:
        case ACL_DATAt:
        case ACL_DATA2t:
        case ACL_MASKt:
        case ACL_MASK2t:
        case CF_ACTION_DSt:
        case CF_ACTION_USt:
        case CF_MASKt:
        case CF_RULEt:
        case VLANt:
            return apollo_l2_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;

} /* end of apollo_table_write */


/* Function Name:
 *      apollo_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
apollo_init(void)
{
#if 0 /* use command to do patch, temply. */
    /* Serdes patch */
    apollo_serdes_patch();

    /* GPHY initial */
    ioal_mem32_write(0x148, 0x8);
    ioal_mem32_write(0x94, 0x14);
#endif
    return RT_ERR_OK;
} /* end of apollo_init */




/* Function Name:
 *      apollo_table_clear
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      regData,addr;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      startAddr,endAddr;

    switch(table)
    {
        case ACL_ACTIONt:
        case ACL_DATAt:
        case ACL_DATA2t:
        case ACL_MASKt:
        case ACL_MASK2t:
        case CF_ACTION_DSt:
        case CF_ACTION_USt:
        case CF_MASKt:
        case CF_RULEt:
        case VLANt:
        case L2_UCt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);


    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == ACL_DATAt || table == ACL_DATA2t || table == CF_RULEt)
    {

        startAddr = startIdx+pTable->size;
        endAddr = endIdx+pTable->size;
    }
    else
    {
        startAddr = startIdx;
        endAddr = endIdx;
    }
    if ((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    memset(l2_table_data,0x0,sizeof(l2_table_data));

    if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set access methold -- 0b1 with specify lut address*/
    fieldData =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        /*set address*/
        fieldData = addr;
        if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        fieldData =1;
        if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*check if table access status*/
        if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

        _apollo_drv_virtualTable_write(table,pTable,fieldData,l2_table_data);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    }


    return RT_ERR_OK;
}/* end of apollo_table_write */



/* Function Name:
 *      apollo_interPhy_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollo_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;


    regData = 0;

    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);

    if ((ret = reg_field_set(GPHY_IND_CMDr, ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(GPHY_IND_CMDr, WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(GPHY_IND_CMDr, CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_read(GPHY_IND_RDr,&regData)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_field_get(GPHY_IND_RDr, BUSYf, &busy, &regData)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_get(GPHY_IND_RDr, RD_DATf ,&fieldData,&regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollo_interPhy_write
 * Description:
 *      Set PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollo_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(GPHY_IND_WDr, WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);
    if ((ret = reg_field_set(GPHY_IND_CMDr, ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(GPHY_IND_CMDr, WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(GPHY_IND_CMDr, CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(GPHY_IND_RDr, BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}


/* Function Name:
 *      apollo_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollo_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 0:
            phyid = 0;
            break;
        case 1:
            phyid = 1;
            break;
        case 3:
            phyid = 4;
            break;
        case 4:
            phyid = 2;
            break;
        case 5:
            phyid = 3;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = APOLLO_DEFAULT_MIIM_PAGE;
        else if(phyReg <= 15)
            real_page = APOLLO_DEFAULT_MIIM_PAGE + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if ((ret = apollo_interPhy_read(phyid, real_page, phyReg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = data;

    return RT_ERR_OK;
} /* end of apollo_miim_read */


/* Function Name:
 *      apollo_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollo_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    switch(port)
    {
        case 0:
            phyid = 0;
            break;
        case 1:
            phyid = 1;
            break;
        case 3:
            phyid = 4;
            break;
        case 4:
            phyid = 2;
            break;
        case 5:
            phyid = 3;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = APOLLO_DEFAULT_MIIM_PAGE;
        else if(phyReg <= 15)
            real_page = APOLLO_DEFAULT_MIIM_PAGE + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if ((ret = apollo_interPhy_write(phyid, real_page, phyReg, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollo_miim_write */



#if 0
#define PATCH_W_OFFSET 0xC000
#define PATCH_R_OFFSET 0x8000

static int32 patch_chk_busy(void)
{
    uint16 value;

    io_mii_phy_reg_read(8, 0, &value);
    while((value&0x8000))
        io_mii_phy_reg_read(8, 0, &value);

    return 0;
}

static void patch_write(uint8 phy_id, uint8 reg, uint16 value)
{
    uint16 data;

    patch_chk_busy();

    io_mii_phy_reg_write(8, 1, value);

    data = (PATCH_W_OFFSET | ((phy_id & 0x1F) << 5) |(reg & 0x1F));
    io_mii_phy_reg_write(8, 0, data);
}

void apollo_serdes_patch(void)
{
#if 0
    osal_printf("apollo_serdes_patch\n\r");
#endif
    /*
    #====================================================================================
    #initial SerDes
    #source RLE0439_TX-RX-2d488G.tcl
    #----------------------------------------------#
    #fine tune TX 16bits to 20 bits function block 622M sample 155M data position
    con_PHYReg w 0x11 0x00 0xA; #force sample clk timing"
    con_PHYReg w 0x11 0x01 0x0100;
    */
    patch_write(0x11, 0x0, 0xA);
    patch_write(0x11, 0x1, 0x0100);

    /*
    #---------------------------------------------------------------------------------
    #setting for jitter transfer---
    con_PHYReg w 0x1d 0x1a 0x0000;   #RX_filter setting(7:0)
    con_PHYReg w 0x1d 0x2 0x2d16;  #kp1==3,ki=1, TX CLK source =RX cdr,disable CMU_TX
    con_PHYReg w 0x1d 0x16 0xa8b2;  #RX_KP1_2=3
    #con_PHYReg w 0x1d 0x16 0xa801;  #Tx clock from CMU.
    con_PHYReg w 0x1d 0x3 0x6041;  #kp2=4
    con_PHYReg w 0x1d 0x18 0xdde4;   #RX_KP2_2=4
    */
    patch_write(0x1d, 0x1a, 0x0000);
    patch_write(0x1d, 0x2, 0x2d16);
    patch_write(0x1d, 0x16, 0xa8b2);
    patch_write(0x1d, 0x3, 0x6041);
    patch_write(0x1d, 0x18, 0xdde4);

    /*
    #----------------------------------------------
    # set best CMU-RX PLL parameter4
    con_PHYReg w 0x1d 0x06 0xf4f0;
    #con_PHYReg w 0x1d 0x07 0x01f7;
    con_PHYReg w 0x1d 0x05 0x4003;
    con_PHYReg w 0x1d 0x0f 0x4fe6;  #TX/RX Io=CML mode
    #con_PHYReg w 0x1d 0x0f 0x4f66;  #TX IO= LVPECL /RX Io=CML mode
    #----------------------------------------------
    #con_PHYReg w 0x1d 0x0 0x5122; # set Fiber 1000 serdes TX internal looback to serdes  RX  (bit8=1)
    #con_PHYReg w 0x1d 0x0 0x5922; # set Fiber 100 serdes TX internal looback to serdes  RX  (bit8=1)

    con_PHYReg w 0x10 1 0xc; #rxd neg edge launch data
    */
    patch_write(0x1d, 0x6, 0xf4f0);
    patch_write(0x1d, 0x5, 0x4003);
    patch_write(0x1d, 0xf, 0x4fe6);
    patch_write(0x10, 0x1, 0xc);
}
#endif


#endif/*CONFIG_SDK_APOLLO*/



#ifdef CONFIG_SDK_APOLLOMP

/*********************************************************/
/*                  APOLLO MP                            */
/*********************************************************/

/* Function Name:
 *      apollomp_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
apollomp_init(void)
{

#ifdef CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO
    uint16  phyData;
    uint32  phyid;
    int32   ret;

    uint32  data;

    /* Enable GPIO */
    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_EN_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_EN_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    /* configure GPIO to OUTPUT mode */
    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_SEL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_SEL_GPIOf, &data)) != RT_ERR_OK)
        return ret;
#endif

#ifdef CONFIG_EXTERNAL_PHY_POLLING
    uint16  phyData;
    uint32  phyid;
    int32   ret;
    uint32  data;

    /*enable MDC/MDIO function on GPIO 60/61*/
    data = 2;
    if ((ret = reg_field_write(APOLLOMP_IO_MODE_ENr, APOLLOMP_MDX_M_ENf, &data)) != RT_ERR_OK)
    {
        return ret;
    }
    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 71, APOLLOMP_EN_GPIOf, &data)) != RT_ERR_OK)
        return ret;
#endif


#if defined(CONFIG_EXTERNAL_PHY_POLLING) || defined(CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO)
    /* Polling PHY ID */
    /* unknown external PHY ID, try tp detect it. */
    for(phyid = 0; phyid <= 31; phyid++)
    {
        if ((ret = apollomp_extPhy_read(phyid, 1, &phyData)) != RT_ERR_OK)
            return ret;

        if(phyData != 0xFFFF)
        {
            apollomp_ext_phyid = phyid;
            break;
        }
    }
#endif
    return RT_ERR_OK;
} /* end of apollomp_init */



static int32 _appolomp_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table    -  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_lutStToTblData(apollomp_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);


    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*--- L2 ---*/
    if (pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC || pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }

    /*--- L3 ---*/
    if (pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL || pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE)
    {

        /*GIP*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if (pL2Table->table_type != APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch ( pL2Table->table_type )
    {
        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*L3 Translation Index*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*EXT_FR*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*WAN_SA*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
} /*apollomp_lutStToTblData*/

/* Function Name:
 *      apollomp_tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table    -  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_tblDataToLutSt(apollomp_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > APOLLOMP_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    if (pL2Table->l3lookup==0) /*L2*/
    {

        /*MAC*/
        if ((ret = table_field_mac_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CVID / CVID_FID*/
        if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        else
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC;


    }
    else
    {/*L3*/

        /*GIP*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(pL2Table->gip_only)
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        else
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL;

    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if(pL2Table->table_type != APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    switch(pL2Table->table_type)
    {
        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;


        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*WAN_SA*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*L3 Translation Index*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_FR*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/*apollomp_tblDataToLutSt*/


/* Function Name:
 *      apollomp_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollomp_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    uint32      tableData[APOLLOMP_LUT_TABLE_WORD];
    pLutSt = (apollomp_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    apollomp_lutStToTblData((apollomp_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case APOLLOMP_RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }
            if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;

        case APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UC:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2MC:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L3MC:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }

             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    apollomp_tblDataToLutSt((apollomp_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
        pLutSt = (apollomp_lut_table_t *)pData;
        _apollomp_drv_virtualTable_read(APOLLOMP_L2_UCt,pTable,addr,tableData);
        apollomp_tblDataToLutSt((apollomp_lut_table_t *)pData, tableData);
        pLutSt->lookup_hit = 1;
        pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of apollomp_lut_table_read */




/* Function Name:
 *      apollomp_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollomp_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      tableData[APOLLOMP_L2_TABLE_WORD];
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (apollomp_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    apollomp_lutStToTblData((apollomp_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }


    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (apollomp_lut_table_t *)pData;
    apollomp_lutStToTblData((apollomp_lut_table_t *)pData, tableData);
    _apollomp_drv_virtualTable_write(APOLLOMP_L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */
    return RT_ERR_OK;
}/* end of apollomp_lut_table_write */




/* Function Name:
 *      apollomp_l34_table_write
 * Description:
 *      Write one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l34_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)

    _apollomp_drv_virtualTable_write(table,pTable,addr,pData);
#else
    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLOMP_L34_TABLE_WORD];

    if(table){}

    /* initialize variable */
    reg_data = 0;
    busy = 0;
    osal_memset(l34_table_data, 0, sizeof(l34_table_data));


    for (i = 0 ; i < APOLLOMP_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        l34_table_data[i] = pData[pTable->datareg_num-i-1];
    }

    /* Write pre-configure table data to indirect data register */
    for (i = 0 ; i < APOLLOMP_L34_TABLE_WORD ; i++)
    {
        if ((ret = reg_write((APOLLOMP_NAT_TBL_ACCESS_WRDATA0r + i), &l34_table_data[i])) != RT_ERR_OK)
        {
            return ret;
        }
    }

    /* Table access operation
     */
    field_data =1;

    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_WR_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(APOLLOMP_NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_WR_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

#endif  /*defined(CONFIG_SDK_ASICDRV_TEST)*/

    return RT_ERR_OK;
}/* end of apollomp_l34_table_write */


/* Function Name:
 *      apollomp_l34_table_read
 * Description:
 *      Read one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l34_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollomp_drv_virtualTable_read(table,pTable,addr,pData);
#else
    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLOMP_L34_TABLE_WORD];
    /* initialize variable */
    reg_data = 0;
    busy = 0;

    if(table){}

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    /* Table access operation
     */
    field_data = 1;

    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_RD_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */

    if ((ret = reg_write(APOLLOMP_NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_RD_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read table data from indirect data register */
    for (i = 0 ; i < APOLLOMP_L34_TABLE_WORD ; i++)
    {
        if ((ret = reg_read((APOLLOMP_NAT_TBL_ACCESS_RDDATA0r + i), &l34_table_data[i])) != RT_ERR_OK)
        {
            return ret;
        }
    }

    for (i = 0 ; i < APOLLOMP_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        pData[pTable->datareg_num - i - 1]= l34_table_data[i];

    }

#endif

    return RT_ERR_OK;
}/* end of apollomp_l34_table_read */



/* Function Name:
 *      apollomp_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      r_data[APOLLOMP_L2_TABLE_WORD];
    uint32      retry, retry_cnt;
    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == APOLLOMP_ACL_DATAt || table == APOLLOMP_ACL_DATA2t || table == APOLLOMP_CF_RULEt)
        addr+= pTable->size;


    if ((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

    retry_cnt = 0;
    do
    {
        retry = 0;

        /*write data to TBL_ACCESS_WR_DATA*/
        /*Write table data to indirect data register */
        for (i = 0 ; i < pTable->datareg_num ; i++)
        {
            l2_table_data[pTable->datareg_num-i-1] = pData[i];
        }
        if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Read TBL_ACCESS_CTRL register */
        if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set address*/
        field_data = addr;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set access methold -- 0b1 with specify lut address*/
        field_data =1;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        field_data =1;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set table type*/
        field_data =pTable->type;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
        {
            return ret;
        }

        /*check if table access status*/
        if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

        if((ret = apollomp_l2_table_read(table,pTable,addr,r_data) != RT_ERR_OK))
           return ret;

        for (i = 0 ; i < pTable->datareg_num ; i++)
        {
            if(r_data[i] != pData[i])
                retry = 1;
        }

        retry_cnt++;
    }while((retry == 1) && (retry_cnt <= 10) &&
           ((table == APOLLOMP_CF_ACTION_DSt) || (table == APOLLOMP_CF_ACTION_USt)));

    if(retry_cnt>1)
    {
        osal_printf("apollomp_l2_table_write retry %u, table=%u, pTabke=0x%p, addr=0x%x\n\r",retry_cnt, table, pTable, addr);
        for (i = 0 ; i < pTable->datareg_num ; i++)
            osal_printf("  pData=%08x-%08x, r_data=%08x-%08x\n\r",pData[0],pData[1],r_data[0],r_data[1]);
    }

#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollomp_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of apollomp_l2_table_write */

/* Function Name:
 *      apollomp_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    /*for ACL and CF data and mask use the same table,but different index*/
    if (table == APOLLOMP_ACL_DATAt || table == APOLLOMP_ACL_DATA2t || table == APOLLOMP_CF_RULEt)
        addr+= pTable->size;

    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data =0;

    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data =pTable->type;

    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }



#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollomp_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of apollomp_l2_table_read */





/* Function Name:
 *      apollomp_table_clear
 * Description:
 *      clear L2 specified table entry by table index range.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      regData,addr;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      startAddr,endAddr;

    switch(table)
    {
        case APOLLOMP_ACL_ACTION_TABLEt:
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
        case APOLLOMP_CF_ACTION_DSt:
        case APOLLOMP_CF_ACTION_USt:
        case APOLLOMP_CF_MASKt:
        case APOLLOMP_CF_RULEt:
        case APOLLOMP_VLANt:
        case APOLLOMP_L2_UCt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);


    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == APOLLOMP_ACL_DATAt || table == APOLLOMP_ACL_DATA2t || table == APOLLOMP_CF_RULEt)
    {

        startAddr = startIdx+pTable->size;
        endAddr = endIdx+pTable->size;
    }
    else
    {
        startAddr = startIdx;
        endAddr = endIdx;
    }
    if ((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    memset(l2_table_data,0x0,sizeof(l2_table_data));

    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set access methold -- 0b1 with specify lut address*/
    fieldData =1;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        /*set address*/
        fieldData = addr;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        fieldData =1;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*check if table access status*/
        if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

        _apollomp_drv_virtualTable_write(table,pTable,fieldData,l2_table_data);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    }


    return RT_ERR_OK;
}/* end of apollomp_table_clear */





/* Function Name:
 *      apollomp_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
apollomp_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case APOLLOMP_ARP_TABLEt:
        case APOLLOMP_EXTERNAL_IP_TABLEt:
        case APOLLOMP_L3_ROUTING_DROP_TRAPt:
        case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
        case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
        case APOLLOMP_NAPT_TABLEt:
        case APOLLOMP_NAPTR_TABLEt:
        case APOLLOMP_NETIFt:
        case APOLLOMP_NEXT_HOP_TABLEt:
        case APOLLOMP_PPPOE_TABLEt:
        case APOLLOMP_WAN_TYPE_TABLEt:
        case APOLLOMP_NEIGHBOR_TABLEt:
        case APOLLOMP_IPV6_ROUTING_TABLEt:
        case APOLLOMP_BINDING_TABLEt:
            return apollomp_l34_table_write(table, pTable, addr, pData);
            break;

        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            return apollomp_lut_table_write(pTable, addr, pData);

            break;

        case APOLLOMP_ACL_ACTION_TABLEt:
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
        case APOLLOMP_CF_ACTION_DSt:
        case APOLLOMP_CF_ACTION_USt:
        case APOLLOMP_CF_MASKt:
        case APOLLOMP_CF_RULEt:
        case APOLLOMP_VLANt:
            return apollomp_l2_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}



/* Function Name:
 *      apollomp_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "apollomp_table_read table=%d, addr=0x%x", table, addr);
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case APOLLOMP_ARP_TABLEt:
        case APOLLOMP_EXTERNAL_IP_TABLEt:
        case APOLLOMP_L3_ROUTING_DROP_TRAPt:
        case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
        case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
        case APOLLOMP_NAPT_TABLEt:
        case APOLLOMP_NAPTR_TABLEt:
        case APOLLOMP_NETIFt:
        case APOLLOMP_NEXT_HOP_TABLEt:
        case APOLLOMP_PPPOE_TABLEt:
        case APOLLOMP_WAN_TYPE_TABLEt:
        case APOLLOMP_NEIGHBOR_TABLEt:
        case APOLLOMP_IPV6_ROUTING_TABLEt:
        case APOLLOMP_BINDING_TABLEt:
            return apollomp_l34_table_read(table, pTable, addr, pData);
            break;

        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            return apollomp_lut_table_read(pTable ,addr ,pData);
            break;

        case APOLLOMP_ACL_ACTION_TABLEt:
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
        case APOLLOMP_CF_ACTION_DSt:
        case APOLLOMP_CF_ACTION_USt:
        case APOLLOMP_CF_MASKt:
        case APOLLOMP_CF_RULEt:
        case APOLLOMP_CF_MASK_L34t:
        case APOLLOMP_CF_RULE_L34t:
        case APOLLOMP_VLANt:
            return apollomp_l2_table_read(table, pTable ,addr ,pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}

#ifdef CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO


static void _apollomp_extMdcMdio_wait_half_CLK(void)
{
    int32 i;

    for(i = 0; i<= 10000; i++)
    {
        /* Dummy loop */
    }
}


static int32 _apollomp_extMdcMdio_setHigh(void)
{
    uint32 data;
    int32 ret;

    /* DIR=write, data=1, clock=0 */
    data = 0;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    _apollomp_extMdcMdio_wait_half_CLK();

    /* DIR=write, data=1, clock=1 */
    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    _apollomp_extMdcMdio_wait_half_CLK();
    return RT_ERR_OK;
}

static int32 _apollomp_extMdcMdio_setLow(void)
{
    uint32 data;
    int32 ret;

    /* DIR=write, data=0, clock=0 */
    data = 0;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    data = 0;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    _apollomp_extMdcMdio_wait_half_CLK();

    /* DIR=write, data=0, clock=1 */
    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    data = 0;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;

    _apollomp_extMdcMdio_wait_half_CLK();
    return RT_ERR_OK;
}

static int32 _apollomp_extMdcMdio_writeBit(uint8 data)
{
    int32 ret;

    if(data == 0)
    {
        if((ret = _apollomp_extMdcMdio_setLow()) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if((ret = _apollomp_extMdcMdio_setHigh()) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

static int32 _apollomp_extMdcMdio_readBit(uint8 *pData)
{
    uint32 bit;
    uint32 data;
    int32 ret;

    /* Clock low & wait half clock */
    data = 0;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
        return ret;
    _apollomp_extMdcMdio_wait_half_CLK();

    /* Read data bit */
    if((ret = reg_array_field_read(APOLLOMP_GPIO_CTRL_1r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_STS_GPIOf, &bit)) != RT_ERR_OK)
        return ret;

    /* clock high & wait half clock */
    data = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_CLOCK_GPIO_PIN, APOLLOMP_CTRL_GPIOf, &data)) != RT_ERR_OK)
       return ret;
    _apollomp_extMdcMdio_wait_half_CLK();

    *pData = bit;
    return RT_ERR_OK;
}

#endif

/* Function Name:
 *      apollomp_extPhy_read
 * Description:
 *      Get external PHY registers .
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollomp_extPhy_read(
    uint32      phyID,
    uint32      phyRegAddr,
    uint16      *pData)
{
#if defined(CONFIG_EXTERNAL_PHY_POLLING) || defined(CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO)

#ifdef CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO
    int i;
    uint16 data;
    uint8 bit;
    uint32 reg_value;
    int32 ret;

    /* 32 bits preamble */
    for (i = 0; i < 32; i++)
    {
        if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
            return ret;
    }

    /* Start of command 0->1 */
    if((ret = _apollomp_extMdcMdio_writeBit(0)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
        return ret;

    /* Writing OP code 1->0 */
    if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit(0)) != RT_ERR_OK)
        return ret;

    /* output PHY address */
    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x10) >> 4)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x08) >> 3)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x04) >> 2)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x02) >> 1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x01))) != RT_ERR_OK)
        return ret;

    /* output register address */
    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x10) >> 4)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x08) >> 3)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x04) >> 2)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x02) >> 1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x01))) != RT_ERR_OK)
        return ret;

    /* Turn around */
    if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit(0)) != RT_ERR_OK)
        return ret;

    /* Change Data pin to INPUT mode */
    reg_value = 0;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_SEL_GPIOf, &reg_value)) != RT_ERR_OK)
        return ret;

    /* Read 16 bit data */
    data = 0;
    for(i = 0; i <= 15; i++)
    {
        data = data << 1;
        if((ret = _apollomp_extMdcMdio_readBit(&bit)) != RT_ERR_OK)
            return ret;
        data = data | bit;
    }

    /* Change Data pin to OUTPUT mode */
    reg_value = 1;
    if((ret = reg_array_field_write(APOLLOMP_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, APOLLOMP_MII_DATA_GPIO_PIN, APOLLOMP_SEL_GPIOf, &reg_value)) != RT_ERR_OK)
        return ret;
    *pData = data;

    return RT_ERR_OK;

#endif

#ifdef CONFIG_EXTERNAL_PHY_POLLING
    uint32 rdata;
    uint32 data;
    int32 ret;
    int i;


    /*set phy id*/
    data = phyID;
    if ((ret = reg_field_write(APOLLOMP_CFG_PHY_CTRLr, APOLLOMP_BASE_PHYADf, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*reg*/
    data = phyRegAddr;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_CMDr, APOLLOMP_ADRf, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set to read*/
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_CMDr, APOLLOMP_WRENf, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*enable command*/
    data = 1;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_CMDr, APOLLOMP_CMD_ENf, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check busy*/
    i=0;
    do{
        i++;
        if ((ret = reg_field_read(APOLLOMP_GPHY_SMI_RDr, APOLLOMP_BUSYf, &rdata)) != RT_ERR_OK)
        {
            return ret;
        }
        if(i>1000)
        {
            osal_printf("busy~~");
            break;
        }
    }while(1==rdata);

    /*read data*/
    if ((ret = reg_field_read(APOLLOMP_GPHY_SMI_RDr, APOLLOMP_RD_DATf, &rdata)) != RT_ERR_OK)
    {
        return ret;
    }
    *pData = rdata;

    /*set CFG_PHY_CTRL.BASE_PHYAD to 0*/
    data = 0;
    if ((ret = reg_field_write(APOLLOMP_CFG_PHY_CTRLr, APOLLOMP_BASE_PHYADf, &data)) != RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;

#endif

#else

    if(phyID){}
    if(phyRegAddr){}
    if(pData){}

    return RT_ERR_FAILED;

#endif





}

/* Function Name:
 *      apollomp_extPhy_write
 * Description:
 *      Set external PHY registers.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/

int32
apollomp_extPhy_write(
    uint32      phyID,
    uint32      phyRegAddr,
    uint16      data)
{

#if defined(CONFIG_EXTERNAL_PHY_POLLING) || defined(CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO)

#ifdef CONFIG_EXTERNAL_PHY_POLLING_USING_GPIO
    int i;
    uint32 bit;
    int32 ret;

    /* 32 bits preamble */
    for (i = 0; i < 32; i++)
    {
        if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
            return ret;
    }

    /* Start of command 0->1 */
    if((ret = _apollomp_extMdcMdio_writeBit(0)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
        return ret;

    /* Writing OP code 0->1 */
    if((ret = _apollomp_extMdcMdio_writeBit(0)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
        return ret;

    /* output PHY address */
    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x10) >> 4)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x08) >> 3)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x04) >> 2)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x02) >> 1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyID & 0x01))) != RT_ERR_OK)
        return ret;

    /* output register address */
    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x10) >> 4)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x08) >> 3)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x04) >> 2)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x02) >> 1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit( (phyRegAddr & 0x01))) != RT_ERR_OK)
        return ret;

    /* Turn around */
    if((ret = _apollomp_extMdcMdio_writeBit(1)) != RT_ERR_OK)
        return ret;

    if((ret = _apollomp_extMdcMdio_writeBit(0)) != RT_ERR_OK)
        return ret;

    /* output data-16bit */
    for(i = 15; i >= 0; i--)
    {
        bit = ((data>>i) & 0x1) > 0 ? 1 : 0;
        if((ret = _apollomp_extMdcMdio_writeBit((uint8)bit)) != RT_ERR_OK)
            return ret;
    }
#endif

#ifdef CONFIG_EXTERNAL_PHY_POLLING

    uint32 rdata;
    uint32 wdata;
    int32 ret;
    int i;

    /*set phy id*/
    wdata = phyID;
    if ((ret = reg_field_write(APOLLOMP_CFG_PHY_CTRLr, APOLLOMP_BASE_PHYADf, &wdata) != RT_ERR_OK))
    {
        return ret;
    }

    /*reg*/
    wdata = phyRegAddr;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_CMDr, APOLLOMP_ADRf, &wdata)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set to write*/
    wdata = 1;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_CMDr, APOLLOMP_WRENf, &wdata)) != RT_ERR_OK)
    {
        return ret;
    }

    /*write data*/
    wdata = data;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_WDr, APOLLOMP_WR_DATf, &wdata)) != RT_ERR_OK)
    {
        return ret;
    }


    /*enable command*/
    wdata = 1;
    if ((ret = reg_field_write(APOLLOMP_GPHY_SMI_CMDr, APOLLOMP_CMD_ENf, &wdata)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check busy*/
    i=0;
    do{
        i++;
        if ((ret = reg_field_read(APOLLOMP_GPHY_SMI_RDr, APOLLOMP_BUSYf, &rdata)) != RT_ERR_OK)
        {
            return ret;
        }
        if(i>1000)
        {
            osal_printf("busy~~");
            break;
        }
    }while(1==rdata);

    /*set CFG_PHY_CTRL.BASE_PHYAD to 0*/
    wdata = 0;
    if ((ret = reg_field_write(APOLLOMP_CFG_PHY_CTRLr, APOLLOMP_BASE_PHYADf, &wdata)) != RT_ERR_OK)
    {
        return ret;
    }

#endif
    return RT_ERR_OK;

#else

    if(phyID){}
    if(phyRegAddr){}
    if(data){}

    return RT_ERR_FAILED;

#endif

}

/* Function Name:
 *      apollomp_interPhy_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollomp_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /*set CFG_PHY_CTRL.BASE_PHYAD to 0*/
    regData = 0;
    if ((ret = reg_field_write(APOLLOMP_CFG_PHY_CTRLr, APOLLOMP_BASE_PHYADf, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    regData = 0;

    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);

    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(APOLLOMP_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_interPhy_write
 * Description:
 *      Set PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollomp_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /*set CFG_PHY_CTRL.BASE_PHYAD to 0*/
    regData = 0;
    if ((ret = reg_field_write(APOLLOMP_CFG_PHY_CTRLr, APOLLOMP_BASE_PHYADf, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_WDr, APOLLOMP_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(APOLLOMP_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollomp_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            phyid = port;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = APOLLOMP_DEFAULT_MIIM_PAGE;
        else if(phyReg <= 15)
            real_page = APOLLOMP_DEFAULT_MIIM_PAGE + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if(5 == phyid)
    {
        if(0xFFFF == apollomp_ext_phyid)
            return RT_ERR_PORT_ID;

        /* use external physical PHY ID to get data */
        phyid = apollomp_ext_phyid;
        if ((ret = apollomp_extPhy_read(phyid, phyReg, &data)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = apollomp_interPhy_read(phyid, real_page, phyReg, &data)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    *pData = data;

    return RT_ERR_OK;
} /* end of apollomp_miim_read */


/* Function Name:
 *      apollomp_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollomp_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            phyid = port;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = APOLLOMP_DEFAULT_MIIM_PAGE;
        else if(phyReg <= 15)
            real_page = APOLLOMP_DEFAULT_MIIM_PAGE + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if(5 == phyid)
    {
        if(0xFFFF == apollomp_ext_phyid)
            return RT_ERR_PORT_ID;

        phyid = apollomp_ext_phyid;
        if ((ret = apollomp_extPhy_write(phyid, phyReg, (uint16)data)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = apollomp_interPhy_write(phyid, real_page, phyReg, (uint16)data)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of apollomp_miim_write */

#endif/*CONFIG_SDK_APOLLOMP*/

#ifdef CONFIG_SDK_RTL9601B

/*********************************************************/
/*                  RTL9601B                            */
/*********************************************************/

/* Function Name:
 *      rtl9601b_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9601b_init(void)
{
    /* No external phy for RTL9601B */

    return RT_ERR_OK;
} /* end of rtl9601b_init */



static int32 _rtl9601b_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table    -  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9601b_lutStToTblData(rtl9601b_lut_table_t *pL2Table, uint32 *pTblData)
{



    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);


    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*IS_STATIC*/
    if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_STATICtf, &pL2Table->is_static, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*--- L2 ---*/
    if (pL2Table->table_type == RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC )
    {
        /*MAC*/
        if ((ret = table_field_mac_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_CVIDtf, &pL2Table->vid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_L2IVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CTAG_IF*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CTAG_VID*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_CTAG_VIDtf, &pL2Table->ctag_vid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*SPA*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*AGE*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }
    /*--- L2 Mucast ---*/
    if(pL2Table->table_type == RTL9601B_RAW_LUT_ENTRY_TYPE_L2MC_DSL)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(RTL9601B_L2_MC_DSLt, RTL9601B_L2_MC_DSL_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(RTL9601B_L2_MC_DSLt, RTL9601B_L2_MC_DSL_VID_FIDtf, &pL2Table->vid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
         /*IVL_SVL*/
        if ((ret = table_field_set(RTL9601B_L2_UCt, RTL9601B_L2_UC_L2IVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*MBR*/
        if ((ret = table_field_set(RTL9601B_L2_MC_DSLt, RTL9601B_L2_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    /*--- L3 ---*/

    if(pL2Table->table_type == RTL9601B_RAW_LUT_ENTRY_TYPE_L3MC_DSL)
    {

        /*GIP*/
        if ((ret = table_field_set(RTL9601B_L3_MC_DSLt, RTL9601B_L3_MC_DSL_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*VID*/
        if ((ret = table_field_set(RTL9601B_L3_MC_DSLt, RTL9601B_L3_MC_DSL_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*MBR*/
        if ((ret = table_field_set(RTL9601B_L3_MC_DSLt, RTL9601B_L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /*rtl9601b_lutStToTblData*/

/* Function Name:
 *      rtl9601b_tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table    -  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9601b_tblDataToLutSt(rtl9601b_lut_table_t *pL2Table, uint32 *pTblData)
{

    int32 ret = RT_ERR_FAILED;
    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > RTL9601B_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);


    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*VALID*/
    if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*STATIC*/
    if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_STATICtf, &pL2Table->is_static, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if (pL2Table->l3lookup==0) /*L2*/
    {

        /*MAC*/
        if ((ret = table_field_mac_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CVID / CVID_FID*/
        if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_CVIDtf, &pL2Table->vid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_L2IVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        else
            pL2Table->table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC;


    }
    else
    {/*L3*/

        /*GIP*/
        if ((ret = table_field_get(RTL9601B_L3_MC_DSLt, RTL9601B_L3_MC_DSL_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        pL2Table->table_type = RTL9601B_RAW_LUT_ENTRY_TYPE_L3MC_DSL;
    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if(pL2Table->table_type != RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_get(RTL9601B_L3_MC_DSLt, RTL9601B_L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch(pL2Table->table_type)
    {
        case RTL9601B_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*CTAG_IF*/
            if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG VID*/
            if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_CTAG_VIDtf, &pL2Table->ctag_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(RTL9601B_L2_UCt, RTL9601B_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            break;

        case RTL9601B_RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            /*no action*/
            break;


        case RTL9601B_RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_get(RTL9601B_L3_MC_DSLt, RTL9601B_L3_MC_DSL_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}/*rtl9601b_tblDataToLutSt*/


/* Function Name:
 *      rtl9601b_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9601b_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    rtl9601b_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[RTL9601B_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    uint32      tableData[RTL9601B_LUT_TABLE_WORD];

    pLutSt = (rtl9601b_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _rtl9601b_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;


    rtl9601b_lutStToTblData((rtl9601b_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {

        return ret;
    }

    switch (pLutSt->method)
    {
        case RTL9601B_RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }
            if ((ret = reg_write(RTL9601B_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;

        case RTL9601B_RAW_LUT_READ_METHOD_ADDRESS:
        case RTL9601B_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UC:
        case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2MC:
        case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L3MC:
        case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/

            field_data = addr;
            if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case RTL9601B_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }

             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _rtl9601b_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    rtl9601b_tblDataToLutSt((rtl9601b_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 8) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
        pLutSt = (rtl9601b_lut_table_t *)pData;
        _rtl9601b_drv_virtualTable_read(RTL9601B_L2_UCt,pTable,addr,tableData);
        rtl9601b_tblDataToLutSt((rtl9601b_lut_table_t *)pData, tableData);
        pLutSt->lookup_hit = 1;
        pLutSt->lookup_busy = 0;

#endif
    return RT_ERR_OK;
}/* end of rtl9601b_lut_table_read */




/* Function Name:
 *      rtl9601b_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9601b_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{


    rtl9601b_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      tableData[RTL9601B_L2_TABLE_WORD];
    uint32      l2_table_data[RTL9601B_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (rtl9601b_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _rtl9601b_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    rtl9601b_lutStToTblData((rtl9601b_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }


    if ((ret = reg_write(RTL9601B_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9601b_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_TBL_ACCESS_STSr, RTL9601B_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 8) | address;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9601b_lut_table_t *)pData;
    rtl9601b_lutStToTblData((rtl9601b_lut_table_t *)pData, tableData);
    _rtl9601b_drv_virtualTable_write(RTL9601B_L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */
    return RT_ERR_OK;
}/* end of rtl9601b_lut_table_write */



/* Function Name:
 *      rtl9601b_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9601b_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;
    uint32      l2_table_data[RTL9601B_L2_TABLE_WORD];
    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == RTL9601B_ACL_DATAt || table == RTL9601B_CF_RULE_ENHANCEDt || table == RTL9601B_CF_RULEt)
        addr+= pTable->size;


    if ((ret = _rtl9601b_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }
    if ((ret = reg_write(RTL9601B_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9601b_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

    _rtl9601b_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of rtl9601b_l2_table_write */

/* Function Name:
 *      rtl9601b_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9601b_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[RTL9601B_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));

    /*for ACL and CF data and mask use the same table,but different index*/
    if (table == RTL9601B_ACL_DATAt  || table == RTL9601B_CF_RULEt || table == RTL9601B_CF_RULE_ENHANCEDt)
        addr+= pTable->size;

    if((ret = _rtl9601b_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data =0;

    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data =pTable->type;

    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9601B_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9601b_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


#if defined(CONFIG_SDK_ASICDRV_TEST)
    _rtl9601b_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of rtl9601b_l2_table_read */





/* Function Name:
 *      rtl9601b_table_clear
 * Description:
 *      clear L2 specified table entry by table index range.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9601b_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      regData,addr;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      l2_table_data[RTL9601B_L2_TABLE_WORD];
    uint32      startAddr,endAddr;

    switch(table)
    {
        case RTL9601B_ACL_ACTION_TABLEt:
        case RTL9601B_ACL_DATAt:
        case RTL9601B_ACL_MASKt:
        case RTL9601B_CF_ACTION_DSt:
        case RTL9601B_CF_ACTION_USt:
        case RTL9601B_CF_MASKt:
        case RTL9601B_CF_RULEt:
        case RTL9601B_VLANt:
        case RTL9601B_L2_UCt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);


    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == RTL9601B_ACL_DATAt || table == RTL9601B_CF_RULE_ENHANCEDt || table == RTL9601B_CF_RULEt)
    {

        startAddr = startIdx+pTable->size;
        endAddr = endIdx+pTable->size;
    }
    else
    {
        startAddr = startIdx;
        endAddr = endIdx;
    }
    if ((ret = _rtl9601b_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    memset(l2_table_data,0x0,sizeof(l2_table_data));

    if ((ret = reg_write(RTL9601B_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9601B_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set access methold -- 0b1 with specify lut address*/
    fieldData =1;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        /*set address*/
        fieldData = addr;
        if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_ADDRf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        fieldData =1;
        if ((ret = reg_field_set(RTL9601B_TBL_ACCESS_CTRLr, RTL9601B_CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(RTL9601B_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*check if table access status*/
        if((ret = _rtl9601b_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

        _rtl9601b_drv_virtualTable_write(table,pTable,fieldData,l2_table_data);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    }


    return RT_ERR_OK;
}/* end of rtl9601b_l2_table_clear */





/* Function Name:
 *      rtl9601b_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
rtl9601b_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9601B_L2_MC_DSLt:
        case RTL9601B_L2_UCt:
        case RTL9601B_L3_MC_DSLt:
            return rtl9601b_lut_table_write(pTable, addr, pData);
            break;

        case RTL9601B_ACL_ACTION_TABLEt:
        case RTL9601B_ACL_DATAt:
        case RTL9601B_ACL_MASKt:
        case RTL9601B_CF_ACTION_DSt:
        case RTL9601B_CF_ACTION_USt:
        case RTL9601B_CF_MASKt:
        case RTL9601B_CF_MASK_L34t:
        case RTL9601B_CF_RULEt:
        case RTL9601B_CF_RULE_ENHANCEDt:
        case RTL9601B_VLANt:
            return rtl9601b_l2_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}



/* Function Name:
 *      rtl9601b_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9601b_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "rtl9601b_table_read table=%d, addr=0x%x", table, addr);
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9601B_L2_MC_DSLt:
        case RTL9601B_L2_UCt:
        case RTL9601B_L3_MC_DSLt:
            return rtl9601b_lut_table_read(pTable ,addr ,pData);
            break;

        case RTL9601B_ACL_ACTION_TABLEt:
        case RTL9601B_ACL_DATAt:
        case RTL9601B_ACL_MASKt:
        case RTL9601B_CF_ACTION_DSt:
        case RTL9601B_CF_ACTION_USt:
        case RTL9601B_CF_MASKt:
        case RTL9601B_CF_RULEt:
        case RTL9601B_CF_MASK_L34t:
        case RTL9601B_CF_RULE_ENHANCEDt:
        case RTL9601B_VLANt:
            return rtl9601b_l2_table_read(table, pTable ,addr ,pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}

/* Function Name:
 *      rtl9601b_interPhy_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9601b_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /* SMI access for RTL9601B has changed
     * Each access has two steps
     * 1. Write page to reg 31 to change page
     * 2. Read/Write date to specific reg
     */

    /* Step 1. change page */
    /*write data to write buffer*/
    regData = 0;
    fieldData = page;
    if ((ret = reg_field_write(RTL9601B_GPHY_SMI_WDr, RTL9601B_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* set phy id and reg address
     * bit 21~17 : phy id
     * bit 4~0 : reg addr
     */
    fieldData = (phyID << 17) | (31);
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9601B_GPHY_SMI_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9601B_GPHY_SMI_RDr, RTL9601B_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);


    /* Step 2. read/write date */
    /* set phy id and reg address
     * bit 21~17 : phy id
     * bit 4~0 : reg addr
     */
    regData = 0;
    fieldData = (phyID << 17) | (phyRegAddr);
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9601B_GPHY_SMI_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9601B_GPHY_SMI_RDr, RTL9601B_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(RTL9601B_GPHY_SMI_RDr, RTL9601B_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      rtl9601b_interPhy_write
 * Description:
 *      Set PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9601b_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /* SMI access for RTL9601B has changed
     * Each access has two steps
     * 1. Write page to reg 31 to change page
     * 2. Read/Write date to specific reg
     */

    /* Step 1. change page */
    /*write data to write buffer*/
    fieldData = page;
    if ((ret = reg_field_write(RTL9601B_GPHY_SMI_WDr, RTL9601B_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* set phy id and reg address
     * bit 21~17 : phy id
     * bit 4~0 : reg addr
     */
    regData = 0;
    fieldData = (phyID << 17) | (31);
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9601B_GPHY_SMI_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9601B_GPHY_SMI_RDr, RTL9601B_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);


    /* Step 2. read/write date */
    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(RTL9601B_GPHY_SMI_WDr, RTL9601B_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* set phy id and reg address
     * bit 21~17 : phy id
     * bit 4~0 : reg addr
     */
    fieldData = (phyID << 17) | (phyRegAddr);
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_SMI_CMDr, RTL9601B_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9601B_GPHY_SMI_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9601B_GPHY_SMI_RDr, RTL9601B_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9601b_ocpInterPhy_read
 * Description:
 *      Get PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9601b_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    regData = 0;

    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9601B_GPHY_IND_CMDr, RTL9601B_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(RTL9601B_GPHY_IND_CMDr, RTL9601B_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_IND_CMDr, RTL9601B_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9601B_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9601B_GPHY_IND_RDr, RTL9601B_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(RTL9601B_GPHY_IND_RDr, RTL9601B_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      rtl9601b_ocpInterPhy_write
 * Description:
 *      Set PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9601b_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data)
{
    uint32 regData = 0, fieldData;
    int32 ret;
    uint32      busy;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(RTL9601B_GPHY_IND_WDr, RTL9601B_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9601B_GPHY_IND_CMDr, RTL9601B_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_IND_CMDr, RTL9601B_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9601B_GPHY_IND_CMDr, RTL9601B_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9601B_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9601B_GPHY_IND_RDr, RTL9601B_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl9601b_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9601b_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    phyid = port;

    real_page = page;

    if ((ret = rtl9601b_interPhy_read(phyid, real_page, phyReg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = data;

    return RT_ERR_OK;
} /* end of rtl9601b_miim_read */


/* Function Name:
 *      rtl9601b_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9601b_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 real_page;

    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    phyid = port;

    real_page = page;

    if ((ret = rtl9601b_interPhy_write(phyid, real_page, phyReg, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of rtl9601b_miim_write */

#endif/*CONFIG_SDK_RTL9601B*/


#ifdef CONFIG_SDK_RTL9602C

/*********************************************************/
/*                  RTL9602C                            */
/*********************************************************/

/* Function Name:
 *      rtl9602c_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9602c_init(void)
{
    /* No external phy for RTL9602C */

    return RT_ERR_OK;
} /* end of rtl9602c_init */



static int32 _rtl9602c_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9602c_lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table    -  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9602c_lutStToTblData(rtl9602c_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    uint32 revert_dip6[4];

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);

    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    /*--- L2 ---*/
    if (pL2Table->table_type == RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC ||
        pL2Table->table_type == RTL9602C_RAW_LUT_ENTRY_TYPE_L2MC ||
        pL2Table->table_type == RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE)
    {
        /*--- L2 ---*/
        if (pL2Table->table_type != RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE)
        {
            /*MAC*/
            if ((ret = table_field_mac_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    /*--- L3 ---*/
    if (pL2Table->table_type == RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE ||
        pL2Table->table_type == RTL9602C_RAW_LUT_ENTRY_TYPE_L3IP6MC)
    {

        /*IP6*/
        if ((ret = table_field_set(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_IP6tf, &pL2Table->ip6, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*WAN_SA*/
        if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    /*(L2 MC DSL)(L3 MC ROUTE)(L3 IP6MC)*/
    if (pL2Table->table_type != RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC)
    {
        /*MBR*/
        if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch ( pL2Table->table_type )
    {
        case RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG_IF*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_SPA*/
            if ((ret = table_field_set(RTL9602C_L2_UCt, RTL9602C_L2_UC_EXT_SPAtf, &pL2Table->ext_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9602C_RAW_LUT_ENTRY_TYPE_L2MC:
            break;

        case RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*GIP*/
            if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*L3 Translation Index*/
            if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SIP_IDX*/
            if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_SIP_IDXtf, &pL2Table->sip_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SIP_FILTER_EN*/
            if ((ret = table_field_set(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_SIP_FILTER_ENtf, &pL2Table->sip_filter, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9602C_RAW_LUT_ENTRY_TYPE_L3IP6MC:
            /*DIP6[43:0]*/
            /* revert DIP6 address first */
            revert_dip6[0] =
                (((uint32) *(pL2Table->dip6.ipv6_addr + 12)) << 24) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 13)) << 16) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 14)) << 8) |
                ((uint32) *(pL2Table->dip6.ipv6_addr + 15));
            revert_dip6[1] =
                (((uint32) *(pL2Table->dip6.ipv6_addr + 8)) << 24) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 9)) << 16) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 10)) << 8) |
                ((uint32) *(pL2Table->dip6.ipv6_addr + 11));
            revert_dip6[2] =
                (((uint32) *(pL2Table->dip6.ipv6_addr + 4)) << 24) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 5)) << 16) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 6)) << 8) |
                ((uint32) *(pL2Table->dip6.ipv6_addr + 7));
            revert_dip6[3] =
                (((uint32) *(pL2Table->dip6.ipv6_addr)) << 24) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 1)) << 16) |
                (((uint32) *(pL2Table->dip6.ipv6_addr + 2)) << 8) |
                ((uint32) *(pL2Table->dip6.ipv6_addr + 3));
            if ((ret = table_field_set(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_DIP_43_0tf, revert_dip6, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DIP6[117:104]*/
            /* Retrive bit 117:104 first */
            osal_memcpy(&value, (((uint8 *) &revert_dip6) + 12), sizeof(value));
            value >>= 8;
            value &= 0x3fff;
            if ((ret = table_field_set(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_DIP_117_104tf, &value, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*IPMCR_IDX*/
            if ((ret = table_field_set(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_L3MCR_IDXtf, &pL2Table->l3mcr_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
} /*rtl9602c_lutStToTblData*/

/* Function Name:
 *      rtl9602c_tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table    -  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9602c_tblDataToLutSt(rtl9602c_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    uint32 revert_dip6[4];

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > RTL9602C_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/
    /*L3LOOKUP*/
    if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if (pL2Table->l3lookup==0) /*L2*/
    {
        /*MAC*/
        if ((ret = table_field_mac_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CVID / CVID_FID*/
        if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = RTL9602C_RAW_LUT_ENTRY_TYPE_L2MC;
        else
            pL2Table->table_type = RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC;
    }
    else
    {/*L3*/
        /*IP6*/
        if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_IP6tf, &pL2Table->ip6, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*WAN_SA*/
        if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(0 == pL2Table->ip6)
        {/*IPv4*/
            pL2Table->table_type = RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        }
        else
        {/*IPv6*/
            pL2Table->table_type = RTL9602C_RAW_LUT_ENTRY_TYPE_L3IP6MC;
        }
    }

    /*(L2 MC DSL)(L3 MC ROUTE)(L3 IP6MC)*/
    if(pL2Table->table_type != RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC)
    {
        /*MBR*/
        if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch(pL2Table->table_type)
    {
        case RTL9602C_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG_IF*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_SPA*/
            if ((ret = table_field_get(RTL9602C_L2_UCt, RTL9602C_L2_UC_EXT_SPAtf, &pL2Table->ext_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9602C_RAW_LUT_ENTRY_TYPE_L2MC:
            break;

        case RTL9602C_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*GIP*/
            if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*L3 Translation Index*/
            if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SIP_IDX*/
            if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_SIP_IDXtf, &pL2Table->sip_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CVID / CVID_FID*/
            if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*IVL_SVL*/
            if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SIP_FILTER_EN*/
            if ((ret = table_field_get(RTL9602C_L3_MC_ROUTEt, RTL9602C_L3_MC_ROUTE_SIP_FILTER_ENtf, &pL2Table->sip_filter, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9602C_RAW_LUT_ENTRY_TYPE_L3IP6MC:
            /* ---L3 IP6MC---*/
            osal_memset((uint8 *)&revert_dip6, 0, 16);
            /*DIP6[43:0]*/
            if ((ret = table_field_get(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_DIP_43_0tf, revert_dip6, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*DIP6[117:104]*/
            if ((ret = table_field_get(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_DIP_117_104tf, &value, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            value &= 0x3fff;
            value <<= 8;
            osal_memcpy((((uint8 *) &revert_dip6) + 12), &value, sizeof(value));

            osal_memset(&pL2Table->dip6, 0, sizeof(pL2Table->dip6));
            *(pL2Table->dip6.ipv6_addr + 0) = (uint8) ((revert_dip6[3] & 0xff000000) >> 24);
            *(pL2Table->dip6.ipv6_addr + 1) = (uint8) ((revert_dip6[3] & 0x00ff0000) >> 16);
            *(pL2Table->dip6.ipv6_addr + 2) = (uint8) ((revert_dip6[3] & 0x0000ff00) >> 8);
            *(pL2Table->dip6.ipv6_addr + 3) = (uint8) (revert_dip6[3] & 0x000000ff);
            *(pL2Table->dip6.ipv6_addr + 4) = (uint8) ((revert_dip6[2] & 0xff000000) >> 24);
            *(pL2Table->dip6.ipv6_addr + 5) = (uint8) ((revert_dip6[2] & 0x00ff0000) >> 16);
            *(pL2Table->dip6.ipv6_addr + 6) = (uint8) ((revert_dip6[2] & 0x0000ff00) >> 8);
            *(pL2Table->dip6.ipv6_addr + 7) = (uint8) (revert_dip6[2] & 0x000000ff);
            *(pL2Table->dip6.ipv6_addr + 8) = (uint8) ((revert_dip6[1] & 0xff000000) >> 24);
            *(pL2Table->dip6.ipv6_addr + 9) = (uint8) ((revert_dip6[1] & 0x00ff0000) >> 16);
            *(pL2Table->dip6.ipv6_addr + 10) = (uint8) ((revert_dip6[1] & 0x0000ff00) >> 8);
            *(pL2Table->dip6.ipv6_addr + 11) = (uint8) (revert_dip6[1] & 0x000000ff);
            *(pL2Table->dip6.ipv6_addr + 12) = (uint8) ((revert_dip6[0] & 0xff000000) >> 24);
            *(pL2Table->dip6.ipv6_addr + 13) = (uint8) ((revert_dip6[0] & 0x00ff0000) >> 16);
            *(pL2Table->dip6.ipv6_addr + 14) = (uint8) ((revert_dip6[0] & 0x0000ff00) >> 8);
            *(pL2Table->dip6.ipv6_addr + 15) = (uint8) (revert_dip6[0] & 0x000000ff);

            /*IPMCR_IDX*/
            if ((ret = table_field_get(RTL9602C_L3_IP6MCt, RTL9602C_L3_IP6MC_L3MCR_IDXtf, &pL2Table->l3mcr_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/*rtl9602c_tblDataToLutSt*/



/* Function Name:
 *      rtl9602c_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    rtl9602c_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[RTL9602C_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    uint32      tableData[RTL9602C_LUT_TABLE_WORD];
    pLutSt = (rtl9602c_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _rtl9602c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    rtl9602c_lutStToTblData((rtl9602c_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case RTL9602C_RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }

            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                if ((ret = reg_array_write(RTL9602C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
                    return ret;
                }
            }
            break;

        case RTL9602C_RAW_LUT_READ_METHOD_ADDRESS:
        case RTL9602C_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case RTL9602C_RAW_LUT_READ_METHOD_NEXT_L2UC:
        case RTL9602C_RAW_LUT_READ_METHOD_NEXT_L2MC:
        case RTL9602C_RAW_LUT_READ_METHOD_NEXT_L3MC:
        case RTL9602C_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case RTL9602C_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }

             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _rtl9602c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
#if 0
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_read(RTL9602C_TBL_ACCESS_RD_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    rtl9602c_tblDataToLutSt((rtl9602c_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 10) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9602c_lut_table_t *)pData;
    _rtl9602c_drv_virtualTable_read(RTL9602C_L2_UCt,pTable,addr,tableData);
    rtl9602c_tblDataToLutSt((rtl9602c_lut_table_t *)pData, tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of rtl9602c_lut_table_read */




/* Function Name:
 *      rtl9602c_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    rtl9602c_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      tableData[RTL9602C_LUT_TABLE_WORD];
    uint32      l2_table_data[RTL9602C_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (rtl9602c_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _rtl9602c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    rtl9602c_lutStToTblData((rtl9602c_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_write(RTL9602C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }
#if 0
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9602c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9602C_TBL_ACCESS_STSr, RTL9602C_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 10) | address;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9602c_lut_table_t *)pData;
    rtl9602c_lutStToTblData((rtl9602c_lut_table_t *)pData, tableData);
    _rtl9602c_drv_virtualTable_write(RTL9602C_L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */
    return RT_ERR_OK;
}/* end of rtl9602c_lut_table_write */



/* Function Name:
 *      rtl9602c_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32      l2_table_data[RTL9602C_L2_TABLE_WORD];
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == RTL9602C_ACL_DATAt)
        addr += 128;
    else if(table == RTL9602C_CF_RULE_48_P0t || table == RTL9602C_CF_RULE_48_P1_T0t || table == RTL9602C_CF_RULE_48_P1_T1t)
        addr += pTable->size;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_write(RTL9602C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }

#if 0
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9602c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

    _rtl9602c_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of rtl9602c_l2_table_write */

/* Function Name:
 *      rtl9602c_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[RTL9602C_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;

    osal_memset(l2_table_data,0,sizeof(l2_table_data));

    /*for ACL and CF data and mask use the same table,but different index*/
    if (table == RTL9602C_ACL_DATAt)
        addr += 128;
    else if(table == RTL9602C_CF_RULE_48_P0t || table == RTL9602C_CF_RULE_48_P1_T0t || table == RTL9602C_CF_RULE_48_P1_T1t)
        addr += pTable->size;

    if((ret = _rtl9602c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data =0;

    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data =pTable->type;

    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9602c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
#if 0
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_read(RTL9602C_TBL_ACCESS_RD_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }

#if defined(CONFIG_SDK_ASICDRV_TEST)
    _rtl9602c_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of rtl9602c_l2_table_read */


/* Function Name:
 *      rtl9602c_l34_table_write
 * Description:
 *      Write one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_l34_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)
    uint32      l34_table_data[RTL9602C_L34_TABLE_WORD];
    uint32      i;

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    for (i = 0 ; i < RTL9602C_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        l34_table_data[i] = pData[pTable->datareg_num-i-1];
    }

    _rtl9602c_drv_virtualTable_write(table,pTable,addr,l34_table_data);
#else

    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[RTL9602C_L34_TABLE_WORD];

    if(table){}

    /* initialize variable */
    reg_data = 0;
    busy = 0;
    osal_memset(l34_table_data, 0, sizeof(l34_table_data));


    for (i = 0 ; i < RTL9602C_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        l34_table_data[i] = pData[pTable->datareg_num-i-1];
    }

    /* Write pre-configure table data to indirect data register */
    for (i = 0 ; i < RTL9602C_L34_TABLE_WORD ; i++)
    {
        if ((ret = reg_write((RTL9602C_NAT_TBL_ACCESS_WRDATA0r + i), &l34_table_data[i])) != RT_ERR_OK)
        {
            return ret;
        }
    }

    /* Table access operation
     */
    field_data =1;

    if ((ret = reg_field_set(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_WR_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9602C_NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_WR_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

#endif  /*defined(CONFIG_SDK_ASICDRV_TEST)*/

    return RT_ERR_OK;
}/* end of rtl9602c_l34_table_write */


/* Function Name:
 *      rtl9602c_l34_table_read
 * Description:
 *      Read one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_l34_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
#if defined(CONFIG_SDK_ASICDRV_TEST)
    uint32      l34_table_data[RTL9602C_L34_TABLE_WORD];
    uint32      i;

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    _rtl9602c_drv_virtualTable_read(table,pTable,addr,l34_table_data);

    for (i = 0 ; i < RTL9602C_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        pData[pTable->datareg_num - i - 1]= l34_table_data[i];

    }
#else

    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[RTL9602C_L34_TABLE_WORD];
    /* initialize variable */
    reg_data = 0;
    busy = 0;

    if(table){}

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    /* Table access operation
     */
    field_data = 1;

    if ((ret = reg_field_set(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_RD_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */

    if ((ret = reg_write(RTL9602C_NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9602C_NAT_TBL_ACCESS_CTRLr, RTL9602C_RD_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read table data from indirect data register */
    for (i = 0 ; i < RTL9602C_L34_TABLE_WORD ; i++)
    {
        if ((ret = reg_read((RTL9602C_NAT_TBL_ACCESS_RDDATA0r + i), &l34_table_data[i])) != RT_ERR_OK)
        {
            return ret;
        }
    }

    for (i = 0 ; i < RTL9602C_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        pData[pTable->datareg_num - i - 1]= l34_table_data[i];

    }

#endif

    return RT_ERR_OK;
}/* end of rtl9602c_l34_table_read */



/* Function Name:
 *      rtl9602c_table_clear
 * Description:
 *      clear L2 specified table entry by table index range.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      regData,addr;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      l2_table_data[RTL9602C_L2_TABLE_WORD];
    uint32      startAddr,endAddr,i;

    switch(table)
    {
        case RTL9602C_ACL_ACTION_TABLEt:
        case RTL9602C_ACL_DATAt:
        case RTL9602C_ACL_MASKt:
        case RTL9602C_CF_ACTION_DSt:
        case RTL9602C_CF_ACTION_USt:
        case RTL9602C_CF_MASK_48_P0t:
        case RTL9602C_CF_MASK_48_P1_T0t:
        case RTL9602C_CF_MASK_48_P1_T1t:
        case RTL9602C_CF_RULE_48_P0t:
        case RTL9602C_CF_RULE_48_P1_T0t:
        case RTL9602C_CF_RULE_48_P1_T1t:
        case RTL9602C_VLANt:
        case RTL9602C_L2_UCt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);


    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == RTL9602C_ACL_DATAt)
    {
        startAddr = startIdx+128;
        endAddr = endIdx+128;
    }
    else if(table == RTL9602C_CF_RULE_48_P0t || table == RTL9602C_CF_RULE_48_P1_T0t || table == RTL9602C_CF_RULE_48_P1_T1t)
    {
        startAddr = startIdx + pTable->size + pTable->size;
        endAddr = endIdx + pTable->size + pTable->size;
    }
    else
    {
        startAddr = startIdx;
        endAddr = endIdx;
    }
    if ((ret = _rtl9602c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    memset(l2_table_data,0x0,sizeof(l2_table_data));

    for (i = 0 ; i < RTL9602C_L2_TABLE_WORD ; i++)
    {
        if ((ret = reg_array_write(RTL9602C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }
#if 0
    if ((ret = reg_write(RTL9602C_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9602C_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set access methold -- 0b1 with specify lut address*/
    fieldData =1;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        /*set address*/
        fieldData = addr;
        if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_ADDRf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        fieldData =1;
        if ((ret = reg_field_set(RTL9602C_TBL_ACCESS_CTRLr, RTL9602C_CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(RTL9602C_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }
        /*check if table access status*/
        if((ret = _rtl9602c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

        _rtl9602c_drv_virtualTable_write(table,pTable,addr,l2_table_data);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    }


    return RT_ERR_OK;
}/* end of rtl9602c_l2_table_clear */





/* Function Name:
 *      rtl9602c_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
rtl9602c_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9602C_L2_MC_DSLt:
        case RTL9602C_L2_UCt:
        case RTL9602C_L3_MC_ROUTEt:
        case RTL9602C_L3_IP6MCt:
            return rtl9602c_lut_table_write(pTable, addr, pData);

            break;
        case RTL9602C_ACL_ACTION_TABLEt:
        case RTL9602C_ACL_DATAt:
        case RTL9602C_ACL_MASKt:
        case RTL9602C_CF_ACTION_DSt:
        case RTL9602C_CF_ACTION_USt:
        case RTL9602C_CF_MASK_48_P0t:
        case RTL9602C_CF_RULE_48_P0t:
        case RTL9602C_CF_MASK_48_P1_T0t:
        case RTL9602C_CF_RULE_48_P1_T0t:
        case RTL9602C_CF_MASK_48_P1_T1t:
        case RTL9602C_CF_RULE_48_P1_T1t:
        case RTL9602C_VLANt:
            return rtl9602c_l2_table_write(table, pTable, addr, pData);
            break;

        case RTL9602C_ARP_CAM_TABLEt:
        case RTL9602C_EXTERNAL_IP_TABLEt:
        case RTL9602C_L3_ROUTING_DROP_TRAPt:
        case RTL9602C_L3_ROUTING_GLOBAL_ROUTEt:
        case RTL9602C_L3_ROUTING_LOCAL_ROUTEt:
        case RTL9602C_NAPT_TABLEt:
        case RTL9602C_NAPTR_TABLEt:
        case RTL9602C_NETIFt:
        case RTL9602C_NEXT_HOP_TABLEt:
        case RTL9602C_PPPOE_TABLEt:
        case RTL9602C_WAN_TYPE_TABLEt:
        case RTL9602C_NEIGHBOR_TABLEt:
        case RTL9602C_IPV6_ROUTING_TABLEt:
        case RTL9602C_BINDING_TABLEt:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV4t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt:
            return rtl9602c_l34_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}



/* Function Name:
 *      rtl9602c_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9602c_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "rtl9602c_table_read table=%d, addr=0x%x", table, addr);
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9602C_L2_MC_DSLt:
        case RTL9602C_L2_UCt:
        case RTL9602C_L3_MC_ROUTEt:
        case RTL9602C_L3_IP6MCt:
            return rtl9602c_lut_table_read(pTable ,addr ,pData);
            break;

        case RTL9602C_ACL_ACTION_TABLEt:
        case RTL9602C_ACL_DATAt:
        case RTL9602C_ACL_MASKt:
        case RTL9602C_CF_ACTION_DSt:
        case RTL9602C_CF_ACTION_USt:
        case RTL9602C_CF_MASK_48_P0t:
        case RTL9602C_CF_RULE_48_P0t:
        case RTL9602C_CF_MASK_48_P1_T0t:
        case RTL9602C_CF_RULE_48_P1_T0t:
        case RTL9602C_CF_MASK_48_P1_T1t:
        case RTL9602C_CF_RULE_48_P1_T1t:
        case RTL9602C_VLANt:
            return rtl9602c_l2_table_read(table, pTable ,addr ,pData);
            break;

        case RTL9602C_ARP_CAM_TABLEt:
        case RTL9602C_EXTERNAL_IP_TABLEt:
        case RTL9602C_L3_ROUTING_DROP_TRAPt:
        case RTL9602C_L3_ROUTING_GLOBAL_ROUTEt:
        case RTL9602C_L3_ROUTING_LOCAL_ROUTEt:
        case RTL9602C_NAPT_TABLEt:
        case RTL9602C_NAPTR_TABLEt:
        case RTL9602C_NETIFt:
        case RTL9602C_NEXT_HOP_TABLEt:
        case RTL9602C_PPPOE_TABLEt:
        case RTL9602C_WAN_TYPE_TABLEt:
        case RTL9602C_NEIGHBOR_TABLEt:
        case RTL9602C_IPV6_ROUTING_TABLEt:
        case RTL9602C_BINDING_TABLEt:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV4t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6t:
        case RTL9602C_FLOW_ROUTING_TABLE_IPV6_EXTt:
            return rtl9602c_l34_table_read(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}

/* Function Name:
 *      rtl9602c_ocpInterPhy_read
 * Description:
 *      Get PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9602c_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;
    uint16 waitCnt = APOLLO_TBL_BUSY_CHECK_NO;

    regData = 0;

    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9602C_GPHY_IND_CMDr, RTL9602C_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(RTL9602C_GPHY_IND_CMDr, RTL9602C_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9602C_GPHY_IND_CMDr, RTL9602C_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9602C_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9602C_GPHY_IND_RDr, RTL9602C_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        waitCnt --;
        if(waitCnt == 0)
        {
            return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(RTL9602C_GPHY_IND_RDr, RTL9602C_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      rtl9602c_ocpInterPhy_write
 * Description:
 *      Set PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9602c_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data)
{
    uint32 regData = 0, fieldData;
    int32 ret;
    uint32      busy;
    uint16 waitCnt = APOLLO_TBL_BUSY_CHECK_NO;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(RTL9602C_GPHY_IND_WDr, RTL9602C_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9602C_GPHY_IND_CMDr, RTL9602C_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9602C_GPHY_IND_CMDr, RTL9602C_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9602C_GPHY_IND_CMDr, RTL9602C_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9602C_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9602C_GPHY_IND_RDr, RTL9602C_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        waitCnt --;
        if(waitCnt == 0)
        {
            return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    } while (busy);

    return RT_ERR_OK;
}




/* Function Name:
 *      rtl9602c_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9602c_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 ocpAddr;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    phyid = port;

#if !defined(FPGA_DEFINED)
    if(phyReg < 8)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 16)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 24)
    {
        ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
    }
    else if(phyReg < 30)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else
    {
        return RT_ERR_PHY_REG_ID;
    }

    if ((ret = rtl9602c_ocpInterPhy_read(phyid, ocpAddr, &data)) != RT_ERR_OK)
    {
        return ret;
    }
#else
    ret= 0;
    data = 0;
#endif

    *pData = data;

    return RT_ERR_OK;
} /* end of rtl9602c_miim_read */


/* Function Name:
 *      rtl9602c_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9602c_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 ocpAddr;

    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    phyid = port;

#if !defined(FPGA_DEFINED)
    if(phyReg < 8)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 16)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 24)
    {
        ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
    }
    else if(phyReg < 30)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else
    {
        return RT_ERR_PHY_REG_ID;
    }

    if ((ret = rtl9602c_ocpInterPhy_write(phyid, ocpAddr, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }
#else
    ret= 0;
    data = data;
#endif

    return RT_ERR_OK;
} /* end of rtl9602c_miim_write */

#endif/*CONFIG_SDK_RTL9602C*/



#ifdef CONFIG_SDK_RTL9607C

/*********************************************************/
/*                  RTL9607C                            */
/*********************************************************/


/* Function Name:
 *      rtl9607c_extPhy_read
 * Description:
 *      Get external PHY registers .
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9607c_extPhy_read(
    uint32      phyID,
    uint32      phyRegAddr,
    uint16      *pData)
{
#if defined(CONFIG_EXTERNAL_PHY_POLLING)
    uint16 data;
    int32 ret;

    /*set phy id*/
    if ((ret = dal_rtl9607c_mdio_cfg_set(CONFIG_MDC_MDIO_HW_SET,0,phyID,MDIO_FMT_C22)) != RT_ERR_OK)
    {
        return ret;
    }  

    /*read*/
    data = 0;
    if ((ret = dal_rtl9607c_mdio_c22_read(phyRegAddr,&data)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = data;


    return RT_ERR_OK;
#else

    if(phyID){}
    if(phyRegAddr){}
    if(pData){}

    return RT_ERR_FAILED;

#endif
}

/* Function Name:
 *      rtl9607c_extPhy_write
 * Description:
 *      Set external PHY registers.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/

int32
rtl9607c_extPhy_write(
    uint32      phyID,
    uint32      phyRegAddr,
    uint16      data)
{

#ifdef CONFIG_EXTERNAL_PHY_POLLING
    uint16 wdata;
    int32 ret;

    /*set phy id*/
    if ((ret = dal_rtl9607c_mdio_cfg_set(CONFIG_MDC_MDIO_HW_SET,0,phyID,MDIO_FMT_C22)) != RT_ERR_OK)
    {
        return ret;
    }  

    /*write*/
    wdata = data;
    if ((ret = dal_rtl9607c_mdio_c22_write(phyRegAddr,wdata)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;

#else

    if(phyID){}
    if(phyRegAddr){}
    if(data){}

    return RT_ERR_FAILED;

#endif

}



/* Function Name:
 *      rtl9607c_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9607c_init(void)
{

#ifdef CONFIG_EXTERNAL_PHY_POLLING
    uint16  phyData;
    uint32  phyid;
    int32   ret;

    /*init mdc/mdio*/
    
    if ((ret = dal_rtl9607c_mdio_init()) != RT_ERR_OK)
    {
        return ret;
    }    

    if ((ret = dal_rtl9607c_mdio_cfg_set(CONFIG_MDC_MDIO_HW_SET,0,0,MDIO_FMT_C22)) != RT_ERR_OK)
    {
        return ret;
    }    


    /* Polling PHY ID */
    /* unknown external PHY ID, try tp detect it. */
    for(phyid = 0; phyid <= 31; phyid++)
    {

        if ((ret = dal_rtl9607c_mdio_cfg_set(CONFIG_MDC_MDIO_HW_SET,0,phyid,MDIO_FMT_C22)) != RT_ERR_OK)
        {
            return ret;
        }    

        if ((ret = rtl9607c_extPhy_read(phyid, 1, &phyData)) != RT_ERR_OK)
            return ret;

        if(phyData != 0x0000)
        {
            rtl9607c_ext_phyid = phyid;
            break;
        }
    }
    
    if(0xFFFF==rtl9607c_ext_phyid)
    {
        osal_printf("\nno ext phy detected\n");    
    }
#endif


    return RT_ERR_OK;
} /* end of rtl9607c_init */



static int32 _rtl9607c_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9607c_lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table    -  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9607c_lutStToTblData(rtl9607c_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;
    rtl9607c_raw_ipmc_type_t hashType;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);

    /*--- Common part registers configuration ---*/
    /*CVID / CVID_FID*/
    if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*L3LOOKUP*/
    if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*IVL_SVL*/
    if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    /*--- L2 ---*/
    if (pL2Table->table_type != RTL9607C_RAW_LUT_ENTRY_TYPE_L3MC)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    /*(L2 MC DSL)(L3 MC)*/
    if (pL2Table->table_type != RTL9607C_RAW_LUT_ENTRY_TYPE_L2UC)
    {
        /*MBR*/
        if ((ret = table_field_set(RTL9607C_L3_MCt, RTL9607C_L3_MC_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR_IDX*/
        if ((ret = table_field_set(RTL9607C_L3_MCt, RTL9607C_L3_MC_EXT_MBRIDXtf, &pL2Table->ext_mbr_idx, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch ( pL2Table->table_type )
    {
        case RTL9607C_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG_IF*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_SPA*/
            if ((ret = table_field_set(RTL9607C_L2_UCt, RTL9607C_L2_UC_EXT_SPAtf, &pL2Table->ext_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9607C_RAW_LUT_ENTRY_TYPE_L2MC:
            break;

        case RTL9607C_RAW_LUT_ENTRY_TYPE_L3MC:
            /* ---L3 MC---*/
            /*GIP*/
            if ((ret = table_field_set(RTL9607C_L3_MCt, RTL9607C_L3_MC_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /* Write different table/field according to current hash mode */
            if ((ret = reg_field_read(RTL9607C_LUT_CFGr, RTL9607C_LUT_IPMC_HASHf, &hashType)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if(hashType != RTL9607C_RAW_IPMC_TYPE_GIP_AND_SIP)
            {
                if(pL2Table->ivl_svl)
                {
                    /* VID */
                    if ((ret = table_field_set(RTL9607C_L3_MC_VIDt, RTL9607C_L3_MC_VID_VIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    /* FID */
                    if ((ret = table_field_set(RTL9607C_L3_MC_FIDt, RTL9607C_L3_MC_FID_FIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
            }
            else
            {
                /* SIP */
                if ((ret = table_field_set(RTL9607C_L3_MCt, RTL9607C_L3_MC_SIPtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                    return RT_ERR_FAILED;
                }
            }

            break;

        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
} /*rtl9607c_lutStToTblData*/

/* Function Name:
 *      rtl9607c_tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table    -  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9607c_tblDataToLutSt(rtl9607c_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;
    rtl9607c_raw_ipmc_type_t hashType;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);

    RT_PARAM_CHK(pL2Table->method > RTL9607C_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/
    /*CVID / CVID_FID*/
    if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*L3LOOKUP*/
    if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*IVL_SVL*/
    if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if (pL2Table->l3lookup==0) /*L2*/
    {
        /*MAC*/
        if ((ret = table_field_mac_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = RTL9607C_RAW_LUT_ENTRY_TYPE_L2MC;
        else
            pL2Table->table_type = RTL9607C_RAW_LUT_ENTRY_TYPE_L2UC;
    }
    else
    {
        /*L3*/
        pL2Table->table_type = RTL9607C_RAW_LUT_ENTRY_TYPE_L3MC;
    }

    /*(L2 MC DSL)(L3 MC)*/
    if(pL2Table->table_type != RTL9607C_RAW_LUT_ENTRY_TYPE_L2UC)
    {
        /*MBR*/
        if ((ret = table_field_get(RTL9607C_L3_MCt, RTL9607C_L3_MC_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBRIDX*/
        if ((ret = table_field_get(RTL9607C_L3_MCt, RTL9607C_L3_MC_EXT_MBRIDXtf, &pL2Table->ext_mbr_idx, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch(pL2Table->table_type)
    {
        case RTL9607C_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG_IF*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_SPA*/
            if ((ret = table_field_get(RTL9607C_L2_UCt, RTL9607C_L2_UC_EXT_SPAtf, &pL2Table->ext_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9607C_RAW_LUT_ENTRY_TYPE_L2MC:
            break;

        case RTL9607C_RAW_LUT_ENTRY_TYPE_L3MC:
            /* ---L3 MC ROUTE---*/
            /*GIP*/
            if ((ret = table_field_get(RTL9607C_L3_MCt, RTL9607C_L3_MC_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /* Read different table/field according to current hash mode */
            if ((ret = reg_field_read(RTL9607C_LUT_CFGr, RTL9607C_LUT_IPMC_HASHf, &hashType)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if(hashType != RTL9607C_RAW_IPMC_TYPE_GIP_AND_SIP)
            {
                if(pL2Table->ivl_svl)
                {
                    /* VID */
                    if ((ret = table_field_get(RTL9607C_L3_MC_VIDt, RTL9607C_L3_MC_VID_VIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    /* FID */
                    if ((ret = table_field_get(RTL9607C_L3_MC_FIDt, RTL9607C_L3_MC_FID_FIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
            }
            else
            {
                /* SIP */
                if ((ret = table_field_get(RTL9607C_L3_MCt, RTL9607C_L3_MC_SIPtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                    return RT_ERR_FAILED;
                }
            }
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/*rtl9607c_tblDataToLutSt*/



/* Function Name:
 *      rtl9607c_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9607c_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    rtl9607c_lut_table_t *pLutSt = NULL;
    uint32      tableData[RTL9607C_LUT_TABLE_WORD];

    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[RTL9607C_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    pLutSt = (rtl9607c_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _rtl9607c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    rtl9607c_lutStToTblData((rtl9607c_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case RTL9607C_RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }

            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                if ((ret = reg_array_write(RTL9607C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
                    return ret;
                }
            }
            break;

        case RTL9607C_RAW_LUT_READ_METHOD_ADDRESS:
        case RTL9607C_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case RTL9607C_RAW_LUT_READ_METHOD_NEXT_L2UC:
        case RTL9607C_RAW_LUT_READ_METHOD_NEXT_L2MC:
        case RTL9607C_RAW_LUT_READ_METHOD_NEXT_L3MC:
        case RTL9607C_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case RTL9607C_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }

             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _rtl9607c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_read(RTL9607C_TBL_ACCESS_RD_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    rtl9607c_tblDataToLutSt((rtl9607c_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9607c_lut_table_t *)pData;
    _rtl9607c_drv_virtualTable_read(RTL9607C_L2_UCt,pTable,addr,tableData);
    rtl9607c_tblDataToLutSt((rtl9607c_lut_table_t *)pData, tableData);
    pLutSt->lookup_hit = pLutSt->valid;
    pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of rtl9607c_lut_table_read */




/* Function Name:
 *      rtl9607c_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9607c_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    rtl9607c_lut_table_t *pLutSt = NULL;
    uint32      tableData[RTL9607C_LUT_TABLE_WORD];

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;

    uint32      l2_table_data[RTL9607C_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (rtl9607c_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _rtl9607c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    rtl9607c_lutStToTblData((rtl9607c_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_write(RTL9607C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9607c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9607C_TBL_ACCESS_STSr, RTL9607C_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9607c_lut_table_t *)pData;
    rtl9607c_lutStToTblData((rtl9607c_lut_table_t *)pData, tableData);
    _rtl9607c_drv_virtualTable_write(RTL9607C_L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    return RT_ERR_OK;
}/* end of rtl9607c_lut_table_write */



/* Function Name:
 *      rtl9607c_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9607c_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32      l2_table_data[RTL9607C_L2_TABLE_WORD];
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == RTL9607C_ACL_DATAt)
        addr += 128;
    else if(table == RTL9607C_CF_RULE_T0t || table == RTL9607C_CF_RULE_T1t || table == RTL9607C_CF_RULE_T2t)
        addr += 256;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_write(RTL9607C_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

#if 0
    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data = pTable->type;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9607c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    if (table == RTL9607C_ACL_DATAt)
        addr -= 128;
    else if(table == RTL9607C_CF_RULE_T0t || table == RTL9607C_CF_RULE_T1t || table == RTL9607C_CF_RULE_T2t)
        addr -= 256;

    _rtl9607c_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of rtl9607c_l2_table_write */

/* Function Name:
 *      rtl9607c_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9607c_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[RTL9607C_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;

    osal_memset(l2_table_data,0,sizeof(l2_table_data));

    /*for ACL and CF data and mask use the same table,but different index*/
    if (table == RTL9607C_ACL_DATAt)
        addr += 128;
    else if(table == RTL9607C_CF_RULE_T0t || table == RTL9607C_CF_RULE_T1t || table == RTL9607C_CF_RULE_T2t)
        addr += 256;

    if((ret = _rtl9607c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


#if 0
    /*set access methold -- 0b1 with specify lut address*/
    field_data = 1;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    /*set command type -- 0b0 read*/
    field_data = 0;

    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;

    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9607C_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9607c_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_read(RTL9607C_TBL_ACCESS_RD_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }

#if defined(CONFIG_SDK_ASICDRV_TEST)
    if (table == RTL9607C_ACL_DATAt)
        addr -= 128;
    else if(table == RTL9607C_CF_RULE_T0t || table == RTL9607C_CF_RULE_T1t || table == RTL9607C_CF_RULE_T2t)
        addr -= 256;

    _rtl9607c_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of rtl9607c_l2_table_read */

/* Function Name:
 *      rtl9607c_table_clear
 * Description:
 *      clear L2 specified table entry by table index range.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9607c_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    uint32      addr;
    uint32      l2_table_data[RTL9607C_L2_TABLE_WORD];
    uint32      startAddr,endAddr;
#else
    int32       ret = RT_ERR_FAILED;
    uint32      regData;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
#endif

    switch(table)
    {
        case RTL9607C_ACL_ACTION_TABLEt:
        case RTL9607C_ACL_DATAt:
        case RTL9607C_ACL_MASKt:
        case RTL9607C_CF_ACTION_DSt:
        case RTL9607C_CF_ACTION_USt:
        case RTL9607C_CF_MASK_T0t:
        case RTL9607C_CF_MASK_T1t:
        case RTL9607C_CF_MASK_T2t:
        case RTL9607C_CF_RULE_T0t:
        case RTL9607C_CF_RULE_T1t:
        case RTL9607C_CF_RULE_T2t:
        case RTL9607C_VLANt:
        case RTL9607C_L2_UCt:
        case RTL9607C_L2_MC_DSLt:
        case RTL9607C_L3_MCt:
        case RTL9607C_L3_MC_VIDt:
        case RTL9607C_L3_MC_FIDt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

#if defined(CONFIG_SDK_ASICDRV_TEST)
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    /*virtual tables don't need index data-mask offset check*/
    startAddr = startIdx;
    endAddr = endIdx;

    memset(l2_table_data,0x0,sizeof(l2_table_data));

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        _rtl9607c_drv_virtualTable_write(table,pTable,addr,l2_table_data);
    }

#else
    if ((ret = _rtl9607c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_read(RTL9607C_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b10 reset*/
    fieldData = 2;
    if ((ret = reg_field_set(RTL9607C_TBL_ACCESS_CTRLr, RTL9607C_CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9607C_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = _rtl9607c_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

#endif

    return RT_ERR_OK;
}/* end of rtl9607c_l2_table_clear */





/* Function Name:
 *      rtl9607c_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
rtl9607c_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9607C_L2_MC_DSLt:
        case RTL9607C_L2_UCt:
        case RTL9607C_L3_MCt:
        case RTL9607C_L3_MC_VIDt:
        case RTL9607C_L3_MC_FIDt:
            return rtl9607c_lut_table_write(pTable, addr, pData);

            break;
        case RTL9607C_ACL_ACTION_TABLEt:
        case RTL9607C_ACL_DATAt:
        case RTL9607C_ACL_MASKt:
        case RTL9607C_CF_ACTION_DSt:
        case RTL9607C_CF_ACTION_USt:
        case RTL9607C_CF_MASK_T0t:
        case RTL9607C_CF_RULE_T0t:
        case RTL9607C_CF_MASK_T1t:
        case RTL9607C_CF_RULE_T1t:
        case RTL9607C_CF_MASK_T2t:
        case RTL9607C_CF_RULE_T2t:
        case RTL9607C_VLANt:
            return rtl9607c_l2_table_write(table, pTable, addr, pData);
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}



/* Function Name:
 *      rtl9607c_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9607c_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "rtl9607c_table_read table=%d, addr=0x%x", table, addr);
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9607C_L2_MC_DSLt:
        case RTL9607C_L2_UCt:
        case RTL9607C_L3_MCt:
        case RTL9607C_L3_MC_VIDt:
        case RTL9607C_L3_MC_FIDt:
            return rtl9607c_lut_table_read(pTable ,addr ,pData);
            break;

        case RTL9607C_ACL_ACTION_TABLEt:
        case RTL9607C_ACL_DATAt:
        case RTL9607C_ACL_MASKt:
        case RTL9607C_CF_ACTION_DSt:
        case RTL9607C_CF_ACTION_USt:
        case RTL9607C_CF_MASK_T0t:
        case RTL9607C_CF_RULE_T0t:
        case RTL9607C_CF_MASK_T1t:
        case RTL9607C_CF_RULE_T1t:
        case RTL9607C_CF_MASK_T2t:
        case RTL9607C_CF_RULE_T2t:
        case RTL9607C_VLANt:
            return rtl9607c_l2_table_read(table, pTable ,addr ,pData);
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}

/* Function Name:
 *      rtl9607c_ocpInterPhy_read
 * Description:
 *      Get PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9607c_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;
    uint16 waitCnt = APOLLO_TBL_BUSY_CHECK_NO;

    RTK_INTPHY_ACCESS_LOCK;

    regData = 0;

    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9607C_GPHY_IND_CMDr, RTL9607C_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(RTL9607C_GPHY_IND_CMDr, RTL9607C_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9607C_GPHY_IND_CMDr, RTL9607C_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9607C_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9607C_GPHY_IND_RDr, RTL9607C_BUSYf, &busy)) != RT_ERR_OK)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return ret;
        }
        waitCnt --;
        if(waitCnt == 0)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(RTL9607C_GPHY_IND_RDr, RTL9607C_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }

    *pData = (uint16)fieldData;

    RTK_INTPHY_ACCESS_UNLOCK;

    return RT_ERR_OK;
}



/* Function Name:
 *      rtl9607c_ocpInterPhy_write
 * Description:
 *      Set PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9607c_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data)
{
    uint32 regData = 0, fieldData;
    int32 ret;
    uint32      busy;
    uint16 waitCnt = APOLLO_TBL_BUSY_CHECK_NO;

    RTK_INTPHY_ACCESS_LOCK;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(RTL9607C_GPHY_IND_WDr, RTL9607C_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9607C_GPHY_IND_CMDr, RTL9607C_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9607C_GPHY_IND_CMDr, RTL9607C_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9607C_GPHY_IND_CMDr, RTL9607C_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9607C_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9607C_GPHY_IND_RDr, RTL9607C_BUSYf, &busy)) != RT_ERR_OK)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return ret;
        }
        waitCnt --;
        if(waitCnt == 0)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    } while (busy);
    RTK_INTPHY_ACCESS_UNLOCK;

    return RT_ERR_OK;
}




/* Function Name:
 *      rtl9607c_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9607c_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 ocpAddr;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    phyid = port;

#if !defined(FPGA_DEFINED)
#ifdef CONFIG_EXTERNAL_PHY_POLLING
    if(CONFIG_MDC_MDIO_EXT_PORT==port)
    {
        if(0xFFFF == rtl9607c_ext_phyid)
            return RT_ERR_PORT_ID;

        if ((ret = rtl9607c_extPhy_read(rtl9607c_ext_phyid, phyReg, pData)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;        
    }
#endif    
    /* No PHY for CPU & PON & Serdes ports */
    if(HAL_IS_CPU_PORT(port) ||
       HAL_IS_PON_PORT(port) ||
       HAL_IS_SERDES_PORT(port))
    {
        return RT_ERR_OK;
    }

    if(phyReg < 8)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 16)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 24)
    {
        ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
    }
    else if(phyReg < 30)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else
    {
        return RT_ERR_PHY_REG_ID;
    }

    if ((ret = rtl9607c_ocpInterPhy_read(phyid, ocpAddr, &data)) != RT_ERR_OK)
    {
        return ret;
    }
#else
    ret= 0;
    data = 0;
#endif

    *pData = data;

    return RT_ERR_OK;
} /* end of rtl9607c_miim_read */


/* Function Name:
 *      rtl9607c_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9607c_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 ocpAddr;

    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    phyid = port;

#if !defined(FPGA_DEFINED)
#ifdef CONFIG_EXTERNAL_PHY_POLLING
    if(CONFIG_MDC_MDIO_EXT_PORT==port)
    {
        if(0xFFFF == rtl9607c_ext_phyid)
            return RT_ERR_PORT_ID;

        if ((ret = rtl9607c_extPhy_write(rtl9607c_ext_phyid, phyReg, data)) != RT_ERR_OK)
            return ret;

        return RT_ERR_OK;        
    }
#endif    

    /* No PHY for CPU & PON & Serdes ports */
    if(HAL_IS_CPU_PORT(port) ||
       HAL_IS_PON_PORT(port) ||
       HAL_IS_SERDES_PORT(port))
    {
        return RT_ERR_OK;
    }

    if(phyReg < 8)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 16)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else if(phyReg < 24)
    {
        ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
    }
    else if(phyReg < 30)
    {
        ocpAddr = 0xa400 + phyReg * 2;
    }
    else
    {
        return RT_ERR_PHY_REG_ID;
    }

    if ((ret = rtl9607c_ocpInterPhy_write(phyid, ocpAddr, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }
#else
    ret= 0;
    data = data;
#endif

    return RT_ERR_OK;
} /* end of rtl9607c_miim_write */

#endif/*CONFIG_SDK_RTL9607C*/


#ifdef CONFIG_SDK_RTL9603CVD

/*********************************************************/
/*                  RTL9603CVD                            */
/*********************************************************/

/* Function Name:
 *      rtl9603cvd_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
rtl9603cvd_init(void)
{
    /* No external phy for RTL9603CVD */

    return RT_ERR_OK;
} /* end of rtl9603cvd_init */



static int32 _rtl9603cvd_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      rtl9603cvd_lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table    -  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9603cvd_lutStToTblData(rtl9603cvd_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;
    rtl9603cvd_raw_ipmc_type_t hashType;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);

    /*--- Common part registers configuration ---*/
    /*CVID / CVID_FID*/
    if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*L3LOOKUP*/
    if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*IVL_SVL*/
    if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    /*--- L2 ---*/
    if (pL2Table->table_type != RTL9603CVD_RAW_LUT_ENTRY_TYPE_L3MC)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    /*(L2 MC DSL)(L3 MC)*/
    if (pL2Table->table_type != RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2UC)
    {
        /*MBR*/
        if ((ret = table_field_set(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR_IDX*/
        if ((ret = table_field_set(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_EXT_MBRIDXtf, &pL2Table->ext_mbr_idx, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch ( pL2Table->table_type )
    {
        case RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG_IF*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_SPA*/
            if ((ret = table_field_set(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_EXT_SPAtf, &pL2Table->ext_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2MC:
            break;

        case RTL9603CVD_RAW_LUT_ENTRY_TYPE_L3MC:
            /* ---L3 MC---*/
            /*GIP*/
            if ((ret = table_field_set(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /* Write different table/field according to current hash mode */
            if ((ret = reg_field_read(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_IPMC_HASHf, &hashType)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if(hashType != RTL9603CVD_RAW_IPMC_TYPE_GIP_AND_SIP)
            {
                if(pL2Table->ivl_svl)
                {
                    /* VID */
                    if ((ret = table_field_set(RTL9603CVD_L3_MC_VIDt, RTL9603CVD_L3_MC_VID_VIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    /* FID */
                    if ((ret = table_field_set(RTL9603CVD_L3_MC_FIDt, RTL9603CVD_L3_MC_FID_FIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
            }
            else
            {
                /* SIP */
                if ((ret = table_field_set(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_SIPtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                    return RT_ERR_FAILED;
                }
            }

            break;

        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
} /*rtl9603cvd_lutStToTblData*/

/* Function Name:
 *      rtl9603cvd_tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table    -  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
static int32 rtl9603cvd_tblDataToLutSt(rtl9603cvd_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;
    rtl9603cvd_raw_ipmc_type_t hashType;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);

    RT_PARAM_CHK(pL2Table->method > RTL9603CVD_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/
    /*CVID / CVID_FID*/
    if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*L3LOOKUP*/
    if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*IVL_SVL*/
    if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if (pL2Table->l3lookup==0) /*L2*/
    {
        /*MAC*/
        if ((ret = table_field_mac_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2MC;
        else
            pL2Table->table_type = RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2UC;
    }
    else
    {
        /*L3*/
        pL2Table->table_type = RTL9603CVD_RAW_LUT_ENTRY_TYPE_L3MC;
    }

    /*(L2 MC DSL)(L3 MC)*/
    if(pL2Table->table_type != RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2UC)
    {
        /*MBR*/
        if ((ret = table_field_get(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBRIDX*/
        if ((ret = table_field_get(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_EXT_MBRIDXtf, &pL2Table->ext_mbr_idx, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch(pL2Table->table_type)
    {
        case RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*CTAG_IF*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_CTAG_IFtf, &pL2Table->ctag_if, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_SPA*/
            if ((ret = table_field_get(RTL9603CVD_L2_UCt, RTL9603CVD_L2_UC_EXT_SPAtf, &pL2Table->ext_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RTL9603CVD_RAW_LUT_ENTRY_TYPE_L2MC:
            break;

        case RTL9603CVD_RAW_LUT_ENTRY_TYPE_L3MC:
            /* ---L3 MC ROUTE---*/
            /*GIP*/
            if ((ret = table_field_get(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /* Read different table/field according to current hash mode */
            if ((ret = reg_field_read(RTL9603CVD_LUT_CFGr, RTL9603CVD_LUT_IPMC_HASHf, &hashType)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if(hashType != RTL9603CVD_RAW_IPMC_TYPE_GIP_AND_SIP)
            {
                if(pL2Table->ivl_svl)
                {
                    /* VID */
                    if ((ret = table_field_get(RTL9603CVD_L3_MC_VIDt, RTL9603CVD_L3_MC_VID_VIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
                else
                {
                    /* FID */
                    if ((ret = table_field_get(RTL9603CVD_L3_MC_FIDt, RTL9603CVD_L3_MC_FID_FIDtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                        return RT_ERR_FAILED;
                    }
                }
            }
            else
            {
                /* SIP */
                if ((ret = table_field_get(RTL9603CVD_L3_MCt, RTL9603CVD_L3_MC_SIPtf, &pL2Table->sip_cvid_fid, pTblData)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                    return RT_ERR_FAILED;
                }
            }
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/*rtl9603cvd_tblDataToLutSt*/



/* Function Name:
 *      rtl9603cvd_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9603cvd_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    rtl9603cvd_lut_table_t *pLutSt = NULL;
    uint32      tableData[RTL9603CVD_LUT_TABLE_WORD];

    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[RTL9603CVD_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    pLutSt = (rtl9603cvd_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _rtl9603cvd_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    rtl9603cvd_lutStToTblData((rtl9603cvd_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case RTL9603CVD_RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }

            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                if ((ret = reg_array_write(RTL9603CVD_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
                    return ret;
                }
            }
            break;

        case RTL9603CVD_RAW_LUT_READ_METHOD_ADDRESS:
        case RTL9603CVD_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case RTL9603CVD_RAW_LUT_READ_METHOD_NEXT_L2UC:
        case RTL9603CVD_RAW_LUT_READ_METHOD_NEXT_L2MC:
        case RTL9603CVD_RAW_LUT_READ_METHOD_NEXT_L3MC:
        case RTL9603CVD_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case RTL9603CVD_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }

             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _rtl9603cvd_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_read(RTL9603CVD_TBL_ACCESS_RD_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    rtl9603cvd_tblDataToLutSt((rtl9603cvd_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9603cvd_lut_table_t *)pData;
    _rtl9603cvd_drv_virtualTable_read(RTL9603CVD_L2_UCt,pTable,addr,tableData);
    rtl9603cvd_tblDataToLutSt((rtl9603cvd_lut_table_t *)pData, tableData);
    pLutSt->lookup_hit = pLutSt->valid;
    pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of rtl9603cvd_lut_table_read */




/* Function Name:
 *      rtl9603cvd_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9603cvd_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    rtl9603cvd_lut_table_t *pLutSt = NULL;
    uint32      tableData[RTL9603CVD_LUT_TABLE_WORD];

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;

    uint32      l2_table_data[RTL9603CVD_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (rtl9603cvd_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _rtl9603cvd_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    rtl9603cvd_lutStToTblData((rtl9603cvd_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_write(RTL9603CVD_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9603cvd_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(RTL9603CVD_TBL_ACCESS_STSr, RTL9603CVD_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (rtl9603cvd_lut_table_t *)pData;
    rtl9603cvd_lutStToTblData((rtl9603cvd_lut_table_t *)pData, tableData);
    _rtl9603cvd_drv_virtualTable_write(RTL9603CVD_L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    return RT_ERR_OK;
}/* end of rtl9603cvd_lut_table_write */



/* Function Name:
 *      rtl9603cvd_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9603cvd_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32      l2_table_data[RTL9603CVD_L2_TABLE_WORD];
    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == RTL9603CVD_ACL_DATAt)
        addr += 128;

/*RTL9603CVD remove classify faeture*/
#if 0
    else if(table == RTL9603CVD_CF_RULE_T0t || table == RTL9603CVD_CF_RULE_T1t || table == RTL9603CVD_CF_RULE_T2t)
        addr += 256;
#endif

    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_write(RTL9603CVD_TBL_ACCESS_WR_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

#if 0
    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data = pTable->type;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9603cvd_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    if (table == RTL9603CVD_ACL_DATAt)
        addr -= 128;
#if 0 /*RTL9603CVD remove claffify feature*/
    else if(table == RTL9603CVD_CF_RULE_T0t || table == RTL9603CVD_CF_RULE_T1t || table == RTL9603CVD_CF_RULE_T2t)
        addr -= 256;
#endif
    _rtl9603cvd_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of rtl9603cvd_l2_table_write */

/* Function Name:
 *      rtl9603cvd_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9603cvd_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[RTL9603CVD_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;

    osal_memset(l2_table_data,0,sizeof(l2_table_data));

    /*for ACL and CF data and mask use the same table,but different index*/
    if (table == RTL9603CVD_ACL_DATAt)
        addr += 128;

#if 0 /*RTL9603CVD remove classify featire*/
    else if(table == RTL9603CVD_CF_RULE_T0t || table == RTL9603CVD_CF_RULE_T1t || table == RTL9603CVD_CF_RULE_T2t)
        addr += 256;
#endif
    if((ret = _rtl9603cvd_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


#if 0
    /*set access methold -- 0b1 with specify lut address*/
    field_data = 1;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
#endif
    /*set command type -- 0b0 read*/
    field_data = 0;

    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;

    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(RTL9603CVD_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _rtl9603cvd_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((ret = reg_array_read(RTL9603CVD_TBL_ACCESS_RD_DATAr,REG_ARRAY_INDEX_NONE,i,&l2_table_data[i])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HWMISC|MOD_DAL), "");
            return ret;
        }

        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }

#if defined(CONFIG_SDK_ASICDRV_TEST)
    if (table == RTL9603CVD_ACL_DATAt)
        addr -= 128;
#if 0 /*RTL9603CVD remove classify feature*/
    else if(table == RTL9603CVD_CF_RULE_T0t || table == RTL9603CVD_CF_RULE_T1t || table == RTL9603CVD_CF_RULE_T2t)
        addr -= 256;
#endif
    _rtl9603cvd_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of rtl9603cvd_l2_table_read */

/* Function Name:
 *      rtl9603cvd_table_clear
 * Description:
 *      clear L2 specified table entry by table index range.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9603cvd_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    uint32      addr;
    uint32      l2_table_data[RTL9603CVD_L2_TABLE_WORD];
    uint32      startAddr,endAddr;
#else
    int32       ret = RT_ERR_FAILED;
    uint32      regData;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
#endif

    switch(table)
    {
        case RTL9603CVD_ACL_ACTION_TABLEt:
        case RTL9603CVD_ACL_DATAt:
        case RTL9603CVD_ACL_MASKt:

#if 0 /*RTL9603CVD remove classify feature*/
        case RTL9603CVD_CF_ACTION_DSt:
        case RTL9603CVD_CF_ACTION_USt:
        case RTL9603CVD_CF_MASK_T0t:
        case RTL9603CVD_CF_MASK_T1t:
        case RTL9603CVD_CF_MASK_T2t:
        case RTL9603CVD_CF_RULE_T0t:
        case RTL9603CVD_CF_RULE_T1t:
        case RTL9603CVD_CF_RULE_T2t:
#endif
        case RTL9603CVD_VLANt:
        case RTL9603CVD_L2_UCt:
        case RTL9603CVD_L2_MC_DSLt:
        case RTL9603CVD_L3_MCt:
        case RTL9603CVD_L3_MC_VIDt:
        case RTL9603CVD_L3_MC_FIDt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

#if defined(CONFIG_SDK_ASICDRV_TEST)
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    /*virtual tables don't need index data-mask offset check*/
    startAddr = startIdx;
    endAddr = endIdx;

    memset(l2_table_data,0x0,sizeof(l2_table_data));

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        _rtl9603cvd_drv_virtualTable_write(table,pTable,addr,l2_table_data);
    }

#else
    if ((ret = _rtl9603cvd_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_read(RTL9603CVD_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b10 reset*/
    fieldData = 2;
    if ((ret = reg_field_set(RTL9603CVD_TBL_ACCESS_CTRLr, RTL9603CVD_CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(RTL9603CVD_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = _rtl9603cvd_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;

#endif

    return RT_ERR_OK;
}/* end of rtl9603cvd_l2_table_clear */





/* Function Name:
 *      rtl9603cvd_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
rtl9603cvd_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9603CVD_L2_MC_DSLt:
        case RTL9603CVD_L2_UCt:
        case RTL9603CVD_L3_MCt:
        case RTL9603CVD_L3_MC_VIDt:
        case RTL9603CVD_L3_MC_FIDt:
            return rtl9603cvd_lut_table_write(pTable, addr, pData);

            break;
        case RTL9603CVD_ACL_ACTION_TABLEt:
        case RTL9603CVD_ACL_DATAt:
        case RTL9603CVD_ACL_MASKt:
#if 0   /*RTL9603CVD remove classify feature*/
        case RTL9603CVD_CF_ACTION_DSt:
        case RTL9603CVD_CF_ACTION_USt:
        case RTL9603CVD_CF_MASK_T0t:
        case RTL9603CVD_CF_RULE_T0t:
        case RTL9603CVD_CF_MASK_T1t:
        case RTL9603CVD_CF_RULE_T1t:
        case RTL9603CVD_CF_MASK_T2t:
        case RTL9603CVD_CF_RULE_T2t:
#endif            
        case RTL9603CVD_VLANt:
            return rtl9603cvd_l2_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}



/* Function Name:
 *      rtl9603cvd_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
rtl9603cvd_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "rtl9603cvd_table_read table=%d, addr=0x%x", table, addr);
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case RTL9603CVD_L2_MC_DSLt:
        case RTL9603CVD_L2_UCt:
        case RTL9603CVD_L3_MCt:
        case RTL9603CVD_L3_MC_VIDt:
        case RTL9603CVD_L3_MC_FIDt:
            return rtl9603cvd_lut_table_read(pTable ,addr ,pData);
            break;

        case RTL9603CVD_ACL_ACTION_TABLEt:
        case RTL9603CVD_ACL_DATAt:
        case RTL9603CVD_ACL_MASKt:
#if 0 /*RTL9603CVD remove classify feature*/
        case RTL9603CVD_CF_ACTION_DSt:
        case RTL9603CVD_CF_ACTION_USt:
        case RTL9603CVD_CF_MASK_T0t:
        case RTL9603CVD_CF_RULE_T0t:
        case RTL9603CVD_CF_MASK_T1t:
        case RTL9603CVD_CF_RULE_T1t:
        case RTL9603CVD_CF_MASK_T2t:
        case RTL9603CVD_CF_RULE_T2t:
#endif
        case RTL9603CVD_VLANt:
            return rtl9603cvd_l2_table_read(table, pTable ,addr ,pData);
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}

/* Function Name:
 *      rtl9603cvd_ocpInterPhy_read
 * Description:
 *      Get PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9603cvd_ocpInterPhy_read(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      *pData)
{
#if !defined(FPGA_DEFINED)
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;
    uint16 waitCnt = APOLLO_TBL_BUSY_CHECK_NO;

    RTK_INTPHY_ACCESS_LOCK;

    regData = 0;

    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9603CVD_GPHY_IND_CMDr, RTL9603CVD_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(RTL9603CVD_GPHY_IND_CMDr, RTL9603CVD_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9603CVD_GPHY_IND_CMDr, RTL9603CVD_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9603CVD_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9603CVD_GPHY_IND_RDr, RTL9603CVD_BUSYf, &busy)) != RT_ERR_OK)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return ret;
        }
        waitCnt --;
        if(waitCnt == 0)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(RTL9603CVD_GPHY_IND_RDr, RTL9603CVD_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }

    *pData = (uint16)fieldData;

    RTK_INTPHY_ACCESS_UNLOCK;
#else
    if(phyID);
    if(ocpAddr);
    if(pData);

    *pData = 0;
#endif
    return RT_ERR_OK;
}



/* Function Name:
 *      rtl9603cvd_ocpInterPhy_write
 * Description:
 *      Set PHY registers from apollo family chips through OCP.
 * Input:
 *      phyID       - PHY id
 *      ocpAddr     - OCP address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
rtl9603cvd_ocpInterPhy_write(
    uint32      phyID,
    uint32      ocpAddr,
    uint16      data)
{

#if !defined(FPGA_DEFINED)
    uint32 regData = 0, fieldData;
    int32 ret;
    uint32      busy;
    uint16 waitCnt = APOLLO_TBL_BUSY_CHECK_NO;

    RTK_INTPHY_ACCESS_LOCK;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(RTL9603CVD_GPHY_IND_WDr, RTL9603CVD_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /* set phy id and ocp address
     * bit 20~16 : phy id
     * bit 15~0 : ocp addr
     */
    fieldData = (phyID << 16) | (ocpAddr & 0xffff);
    if ((ret = reg_field_set(RTL9603CVD_GPHY_IND_CMDr, RTL9603CVD_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9603CVD_GPHY_IND_CMDr, RTL9603CVD_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(RTL9603CVD_GPHY_IND_CMDr, RTL9603CVD_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(RTL9603CVD_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        RTK_INTPHY_ACCESS_UNLOCK;
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(RTL9603CVD_GPHY_IND_RDr, RTL9603CVD_BUSYf, &busy)) != RT_ERR_OK)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return ret;
        }
        waitCnt --;
        if(waitCnt == 0)
        {
            RTK_INTPHY_ACCESS_UNLOCK;
            return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    } while (busy);

    RTK_INTPHY_ACCESS_UNLOCK;
#else
    if(phyID);
    if(ocpAddr);
    if(data);
#endif
    return RT_ERR_OK;
}




/* Function Name:
 *      rtl9603cvd_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9603cvd_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 ocpAddr;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    phyid = port;

#if !defined(FPGA_DEFINED)
    /* No PHY for CPU & PON & Serdes ports */
    if(HAL_IS_CPU_PORT(port) ||
       HAL_IS_PON_PORT(port) ||
       (HAL_IS_SERDES_PORT(port) && !HAL_IS_ETHER_PORT(port)))
    {
        return RT_ERR_OK;
    }

    switch(port)
    {
    case 3:
        /* GPHY only reg 16~24 affect by page */
        if(phyReg >= 16 && phyReg < 24)
        {
            ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
        }
        else
        {
            ocpAddr = 0xa400 + (phyReg << 1);
        }
        break;
    default:
        /* FE PHY only care bits 1~5 */
        ocpAddr = phyReg << 1;
        break;
    }

    if ((ret = rtl9603cvd_ocpInterPhy_read(phyid, ocpAddr, &data)) != RT_ERR_OK)
    {
        return ret;
    }
#else
    ret= 0;
    data = 0;
#endif

    *pData = data;

    return RT_ERR_OK;
} /* end of rtl9603cvd_miim_read */


/* Function Name:
 *      rtl9603cvd_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
rtl9603cvd_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 ocpAddr;

    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    phyid = port;

#if !defined(FPGA_DEFINED)
    /* No PHY for CPU & PON & Serdes ports */
    if(HAL_IS_CPU_PORT(port) ||
       HAL_IS_PON_PORT(port) ||
       (HAL_IS_SERDES_PORT(port) && !HAL_IS_ETHER_PORT(port)))
    {
        return RT_ERR_OK;
    }

    switch(port)
    {
    case 3:
        /* GPHY only reg 16~24 affect by page */
        if(phyReg >= 16 && phyReg < 24)
        {
            ocpAddr = ((page & 0xfff) << 4) + (phyReg - 16) * 2;
        }
        else
        {
            ocpAddr = 0xa400 + (phyReg << 1);
        }
        break;
    default:
        /* FE PHY only care bits 1~5 */
        ocpAddr = phyReg << 1;
        break;
    }

    if ((ret = rtl9603cvd_ocpInterPhy_write(phyid, ocpAddr, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }
#else
    ret= 0;
    data = data;
#endif

    return RT_ERR_OK;
} /* end of rtl9603cvd_miim_write */

#endif/*CONFIG_SDK_RTL9603CVD*/

