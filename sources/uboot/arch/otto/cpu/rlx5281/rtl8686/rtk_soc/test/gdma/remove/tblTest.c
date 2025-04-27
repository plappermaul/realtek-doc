#include "utility.h"
#include "drvTest.h"
#include "rtl_glue.h"
#include "rtl8651_layer2.h"
#include "rtl8651_layer2local.h"
#include "rtl8651_tblDrv.h"
#include "rtl865xC_tblAsicDrv.h"
#include "rtl8651_tblDrvLocal.h"
#include "rtl8651_tblDrv.h"
#include "asicRegs.h"

#define RTL8651_TESTING_DEFAULT_CONFIGURATION

static int32 default_configuration1(uint32 type1, uint32 type2)
{
	ether_addr_t mac = { { 0x00, 0x03, 0x04, 0x05, 0x06, 0x01 } };
	ipaddr_t ipaddr, mask;

	/* VLAN 1: subnet 1 (VLAN 1 has already been added in the initialization of test code. */
	ASSERT( rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS );
	ASSERT( rtl8651_addNetworkIntf("vlan1") == SUCCESS );
	ASSERT( rtl8651_specifyNetworkIntfLinkLayerType("vlan1", type1, 1) == SUCCESS );
	ipaddr = ntohl( inet_addr("192.168.1.1") );
	mask = ntohl( inet_addr("255.255.255.0") );
	ASSERT( rtl8651_addIpIntf("vlan1", ipaddr, mask) == SUCCESS );
	
	/* VLAN 2: subnet 2 */
	mac.octet[5] = 0x02;
	ASSERT( rtl8651_addVlan(2) == SUCCESS );
	ASSERT( rtl8651_addVlanPortMember( 2, 0 ) == SUCCESS );	/* Add a member port to this VLAN. Otherwise, this VLAN is invalid in ASIC. */
	ASSERT( rtl8651_assignVlanMacAddress(2, &mac, 1) == SUCCESS );
	ASSERT( rtl8651_addNetworkIntf("vlan2") == SUCCESS );
	ASSERT( rtl8651_specifyNetworkIntfLinkLayerType("vlan2", type2, 2) == SUCCESS );
	ipaddr = ntohl(inet_addr("192.168.2.1"));
	mask = ntohl(inet_addr("255.255.255.0"));
	ASSERT( rtl8651_addIpIntf("vlan2", ipaddr, mask) == SUCCESS );
	return SUCCESS;
}

int32 default_configuration3(void)
{
	ether_addr_t mac = { { 0x00, 0x03, 0x04, 0x05, 0x06, 0x01 } };
	ipaddr_t ipaddr, mask;

	/* VLAN 1: subnet 1 */	
	ASSERT(rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("pppoe") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("pppoe", RTL8651_LL_PPPOE, 1) == SUCCESS);
	ASSERT(rtl8651_addPppoeSession(1000, 1) == SUCCESS);

	/* VLAN 2: subnet 2 */
	mac.octet[5] = 0x02;
	ASSERT(rtl8651_addVlan(2) == SUCCESS);
	ASSERT( rtl8651_addVlanPortMember( 2, 0 ) == SUCCESS );	/* Add a member port to this VLAN. Otherwise, this VLAN is invalid in ASIC. */
	ASSERT(rtl8651_assignVlanMacAddress(2, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("lan") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("lan", RTL8651_LL_VLAN, 2) == SUCCESS);
	ipaddr = ntohl(inet_addr("192.168.2.1"));
	mask = ntohl(inet_addr("255.255.255.0"));
	ASSERT(rtl8651_addIpIntf("lan", ipaddr, mask) == SUCCESS);
	return SUCCESS;

}


static int32 pppoe_default_configuration(void)
{
	ether_addr_t mac = { { 0x00, 0x03, 0x04, 0x05, 0x06, 0x01 } };
	ipaddr_t ipaddr, mask;

	/* VLAN 1: subnet 1 */	
	ASSERT(rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS);
	ASSERT(rtl8651_addNetworkIntf("pppoe") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("pppoe", RTL8651_LL_PPPOE, 1) == SUCCESS);
	ASSERT(rtl8651_addExtNetworkInterface("pppoe") == SUCCESS);

	/* VLAN 2: subnet 2 */
	mac.octet[5] = 0x02;
	ASSERT(rtl8651_addVlan(2) == SUCCESS);
	ASSERT(rtl8651_assignVlanMacAddress(2, &mac, 1) == SUCCESS);
	ASSERT( rtl8651_addVlanPortMember( 2, 0 ) == SUCCESS );	/* Add a member port to this VLAN. Otherwise, this VLAN is invalid in ASIC. */
	ASSERT(rtl8651_addNetworkIntf("vlan") == SUCCESS);
	ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("vlan", RTL8651_LL_VLAN, 2) == SUCCESS);
	ipaddr = ntohl(inet_addr("192.168.2.1"));
	mask = ntohl(inet_addr("255.255.255.0"));
	ASSERT(rtl8651_addIpIntf("vlan", ipaddr, mask) == SUCCESS);		
	return SUCCESS;
}


#define RTL865X_ROUTING_TESTING_CODE

/*
@fun int32 | l3_test1 | Routing table Testing.
@comm
Test if an interface route is automatically added while a subnet is created. Contrarily, check if the interface is automatically
removed while a subnet is removed.
*/
int32 l3_test1(uint32 caseNo)
{
	rtl865x_tblAsicDrv_routingParam_t asic_rt_1, asic_rt_2;
	ipaddr_t ipaddr, mask;
	uint32 entry;

	START_TEST_CASE();

	{
		/*========================================
		  * Test: Interface route add/delete
		  *	      If a ethernet type subnet is added, a interface route
		  *	      should be added automatically.
		  *========================================*/
		default_configuration1( RTL8651_LL_VLAN, RTL8651_LL_VLAN );

		/* Verify L3 Table for VLAN 1 */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= ntohl(inet_addr("192.168.1.1")) & ntohl(inet_addr("255.255.255.0"));
		asic_rt_1.ipMask	= ntohl(inet_addr("255.255.255.0"));
		asic_rt_1.process 	= RT_ARP;
		asic_rt_1.vidx		= 0;		/* 'vidx' stands for net interface table index */
		asic_rt_1.arpStart	= 0;
		asic_rt_1.arpEnd	= 31;
		asic_rt_1.arpIpIdx	= 0;
		asic_rt_1.internal	= 1;
		asic_rt_1.DMZFlag	= 0;
		ASSERT(rtl8651_getAsicRouting(0,  &asic_rt_2) == SUCCESS);
		L3_ENTRY_CMP(&asic_rt_1, &asic_rt_2);

		/* Verify L3 Table for VLAN 2 */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= ntohl(inet_addr("192.168.2.1")) & ntohl(inet_addr("255.255.255.0"));
		asic_rt_1.ipMask	= ntohl(inet_addr("255.255.255.0"));
		asic_rt_1.process 	= RT_ARP;
		asic_rt_1.vidx		= 1;
		asic_rt_1.arpStart	= 32;
		asic_rt_1.arpEnd	= 63;
		asic_rt_1.arpIpIdx	= 0;
		asic_rt_1.internal	= 1;
		asic_rt_1.DMZFlag	= 0;
		ASSERT(rtl8651_getAsicRouting(1,  &asic_rt_2) == SUCCESS);
		L3_ENTRY_CMP(&asic_rt_1, &asic_rt_2);

		for ( entry = 2; entry < RTL8651_ROUTINGTBL_SIZE - 1; entry++ )
		{
			/* Other L3 entries should be invalid */
			ASSERT(rtl8651_getAsicRouting(entry,  &asic_rt_2) != SUCCESS);
		}

		/* Remove Subnet */
		ipaddr = ntohl(inet_addr("192.168.1.1"));
		mask = ntohl(inet_addr("255.255.255.0"));
		ASSERT(rtl8651_delIpIntf("vlan1", ipaddr, mask) == SUCCESS);
		ipaddr = ntohl(inet_addr("192.168.2.1"));
		mask = ntohl(inet_addr("255.255.255.0"));
		ASSERT(rtl8651_delIpIntf("vlan2", ipaddr, mask) == SUCCESS);

		/* Verify L3 Table */
		for ( entry = 0; entry < RTL8651_ROUTINGTBL_SIZE - 1; entry++ )
		{
			ASSERT( rtl8651_getAsicRouting(entry,  &asic_rt_2) != SUCCESS );
		}
	}

	return SUCCESS;
}



#define RTL865X_ARP_TESTING_CODE

/*
@func int32 | arp_test1 | ARP entry testing.
@comm
Check to see if a subnet is created, a broadcast arp should be added automatically.
*/
int32 arp_test1(uint32 caseNo)
{
	ether_addr_t mac = { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
	rtl865x_tblAsicDrv_arpParam_t asic_arp_1, asic_arp_2;
	rtl865x_tblAsicDrv_l2Param_t asic_l2_1, asic_l2_2;

	START_TEST_CASE();

	{
		/*===============================================
		  * Test: If a Subnet is created, a broadcast arp should be added
		  *	      automatically.
		  *===============================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		/* Verify ARP Table */
		bzero(&asic_arp_1, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		bzero(&asic_arp_2, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		asic_arp_1.nextHopRow		= 0;
		asic_arp_1.nextHopColumn	= 0;
		asic_arp_1.aging			= 31;
		ASSERT(rtl8651_getAsicArp(255,  &asic_arp_2) == SUCCESS);
		ARP_ENTRY_CMP(&asic_arp_1, &asic_arp_2);
		ASSERT(rtl8651_getAsicArp(511,  &asic_arp_2) == SUCCESS);
		ARP_ENTRY_CMP(&asic_arp_1, &asic_arp_2);

		/* Verify L2 Table */
		bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		memcpy(&asic_l2_1.macAddr, &mac, 6);
		asic_l2_1.memberPortMask	= 0x1ff;
		asic_l2_1.ageSec			= 300;
		asic_l2_1.cpu				= 0;
		asic_l2_1.srcBlk			= 0;
		asic_l2_1.isStatic			= 1;
		asic_l2_1.nhFlag			= 0;
		asic_l2_1.fid				= 0;
		asic_l2_1.auth				= 0;
		ASSERT(rtl8651_getAsicL2Table(0, 0,  &asic_l2_2) == SUCCESS);
		L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);

	}

	return SUCCESS;
}

/*
@func int32 | arp_test2 | ARP entry testing.
@comm
Add and delete a ARP entry.
*/
int32 arp_test2(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	rtl865x_tblAsicDrv_arpParam_t asic_arp_1, asic_arp_2;
	rtl865x_tblAsicDrv_l2Param_t asic_l2_1, asic_l2_2;
	ipaddr_t ipaddr;


	START_TEST_CASE();

	{
		/*==================================================
		  * Test: add/delete a ARP entry
		  *==================================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		/* (1) add ARP entry */
		ipaddr = ntohl(inet_addr("192.168.1.254"));
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0x01) == SUCCESS);

		/* Verify ARP Table */
		bzero(&asic_arp_1, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		bzero(&asic_arp_2, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		asic_arp_1.nextHopRow		= rtl8651_filterDbIndex(&mac, 0);
		asic_arp_1.nextHopColumn	= 0;
		asic_arp_1.aging			= 31;
		ASSERT(rtl8651_getAsicArp(254, &asic_arp_2) == SUCCESS);
		ARP_ENTRY_CMP(&asic_arp_1, &asic_arp_2);
	
		/* Verify L2 Table */
		bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		memcpy(&asic_l2_1.macAddr, &mac, 6);
		asic_l2_1.memberPortMask	= 0x02;
		asic_l2_1.ageSec			= 300;
		asic_l2_1.cpu				= 0;
		asic_l2_1.srcBlk			= 0;
		asic_l2_1.isStatic			= 0;	/* Currently, we always write it as dynamic entry and set */
		asic_l2_1.nhFlag			= 1;	/* nhFlag to avoid round up when writing it as static entry */
		asic_l2_1.fid				= 0;
		asic_l2_1.auth				= 0;
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) == SUCCESS);
		L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);

		/* (2) Remove the arp entry */
		ASSERT(rtl8651_delArp(ipaddr) == SUCCESS);

		/* Verify ARP Table */
		ASSERT(rtl8651_getAsicArp(254, &asic_arp_2) != SUCCESS);

		/* Verify L2 Table */
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) != SUCCESS);

	}
	return SUCCESS;
}

/*
@func int32 | arp_test3 | ARP entry testing.
@comm
ARP doesn't belong to gateway's any network interface will be forbidden.
*/
int32 arp_test3(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	ipaddr_t ipaddr;


	START_TEST_CASE();

	{
		/*==================================================
		  * Test: add a unknown arp
		  *==================================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		ipaddr = ntohl(inet_addr("192.168.9.2"));
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0x01) != SUCCESS);
	}
	return SUCCESS;
}

/*
@func int32 | arp_test4 | ARP entry testing.
@comm
Add a ARP entry and check to see if its corresponding L2 entry's member port is correct.
*/
int32 arp_test4(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	rtl865x_tblAsicDrv_l2Param_t asic_l2_1, asic_l2_2;
	ipaddr_t ipaddr;
	int32 port;
	

	START_TEST_CASE();

	{
		/*==================================================
		  * Test: (ARP) L2 entry's member port
		  *==================================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		ipaddr = ntohl(inet_addr("192.168.1.254"));
		for ( port = 0; port < RTL8651_PORT_NUMBER; port++ )
		{
			ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", port) == SUCCESS);

			/* Verify L2 Table */
			bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
			bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
			memcpy(&asic_l2_1.macAddr, &mac, 6);
			asic_l2_1.memberPortMask	= ( 1 << port );
			asic_l2_1.ageSec			= 300;
			asic_l2_1.cpu				= 0;
			asic_l2_1.srcBlk			= 0;
			asic_l2_1.isStatic			= 0;	/* Currently, we always write it as dynamic entry and set */
			asic_l2_1.nhFlag			= 1;	/* nhFlag to avoid round up when writing it as static entry */
			asic_l2_1.fid				= 0;
			asic_l2_1.auth				= 0;
			ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) == SUCCESS);
			L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);
			
			ASSERT(rtl8651_delArp(ipaddr) == SUCCESS);	
		}

		/* add Arp entry: port mask = 0xffffffff */
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0xffffffff) == SUCCESS);

		/* Verify L2 Table */
		bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		memcpy(&asic_l2_1.macAddr, &mac, 6);
		asic_l2_1.memberPortMask	= 0x1ff;
		asic_l2_1.ageSec			= 300;
		asic_l2_1.cpu				= 0;
		asic_l2_1.srcBlk			= 0;
		asic_l2_1.isStatic			= 0;	/* Currently, we always write it as dynamic entry and set */
		asic_l2_1.nhFlag			= 1;	/* nhFlag to avoid round up when writing it as static entry */
		asic_l2_1.fid				= 0;
		asic_l2_1.auth				= 0;
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) == SUCCESS);
		L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);

	}
	return SUCCESS;
}

/*
@func int32 | arp_test5 | ARP entry testing.
@comm
Make sure the duplicate arp adding (same IP but different MAC) will result in the previous arp entry be updated.
*/
int32 arp_test5(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	rtl865x_tblAsicDrv_arpParam_t asic_arp_1, asic_arp_2;
	rtl865x_tblAsicDrv_l2Param_t asic_l2_1, asic_l2_2;
	rtl8651_tblDrv_networkIntfTable_t *netif_t;
	ipaddr_t ipaddr;
	uint32 temp;
	

	START_TEST_CASE();

	{
		/*==================================================
		  * Test: Internal API: the duplicate adding will modify the previous
		  *	      arp info.
		  *==================================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		netif_t = GET_NETWORK_INTERFACE("vlan1", NULL) ;
		ASSERT(netif_t != NULL);

		/* (1) Add Arp entry */
		ipaddr = ntohl(inet_addr("192.168.1.254"));
		ASSERT(_rtl8651_addLocalArp(TRUE, ipaddr, &mac, netif_t, 0x01) == SUCCESS);

		/* Verify ARP Table */
		bzero(&asic_arp_1, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		bzero(&asic_arp_2, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		asic_arp_1.nextHopRow		= rtl8651_filterDbIndex(&mac, 0);
		asic_arp_1.nextHopColumn	= 0;
		asic_arp_1.aging			= 31;
		ASSERT(rtl8651_getAsicArp(254, &asic_arp_2) == SUCCESS);
		ARP_ENTRY_CMP(&asic_arp_1, &asic_arp_2);

		/* Verify L2 Table */
		bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		memcpy(&asic_l2_1.macAddr, &mac, 6);
		asic_l2_1.memberPortMask	= 0x02;
		asic_l2_1.ageSec			= 300;
		asic_l2_1.cpu				= 0;
		asic_l2_1.srcBlk			= 0;
		asic_l2_1.isStatic			= 0;	/* Currently, we always write it as dynamic entry and set */
		asic_l2_1.nhFlag			= 1;	/* nhFlag to avoid round up when writing it as static entry */
		asic_l2_1.fid				= 0;
		asic_l2_1.auth				= 0;
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) == SUCCESS);
		L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);

		/* (2) adding the same ARP (same Ip address) */
		temp = rtl8651_filterDbIndex(&mac, 0);
		mac.octet[5] ++;
		ASSERT(_rtl8651_addLocalArp(TRUE, ipaddr, &mac, netif_t, 0x01) == SUCCESS);

		/* Verify ARP table */
		bzero(&asic_arp_1, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		bzero(&asic_arp_2, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		asic_arp_1.nextHopRow		= rtl8651_filterDbIndex(&mac, 0);
		asic_arp_1.nextHopColumn	= 0;
		asic_arp_1.aging			= 31;
		ASSERT(rtl8651_getAsicArp(254, &asic_arp_2) == SUCCESS);
		ARP_ENTRY_CMP(&asic_arp_1, &asic_arp_2);

		/* Verify L2 Table */
		bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		memcpy(&asic_l2_1.macAddr, &mac, 6);
		asic_l2_1.memberPortMask	= 0x02;
		asic_l2_1.ageSec			= 300;
		asic_l2_1.cpu				= 0;
		asic_l2_1.srcBlk			= 0;
		asic_l2_1.isStatic			= 0;	/* Currently, we always write it as dynamic entry and set */
		asic_l2_1.nhFlag			= 1;	/* nhFlag to avoid round up when writing it as static entry */
		asic_l2_1.fid				= 0;
		asic_l2_1.auth				= 0;
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) == SUCCESS);
		L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);

		ASSERT(rtl8651_getAsicL2Table(temp, 0,  &asic_l2_2) != SUCCESS);

		/* (3) remove the arp entry */
		ASSERT(_rtl8651_delLocalArp(TRUE, ipaddr, netif_t) == SUCCESS);

		/* Verify ARP Table */
		ASSERT(rtl8651_getAsicArp(254, &asic_arp_2) != SUCCESS);

		/* Verify L2 Table */
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) != SUCCESS);
	}
	return SUCCESS;
}

/*
@func int32 | arp_test6 | ARP entry testing.
@comm
Make sure the duplicate arp adding (same IP but different MAC) will result in the previous arp entry be updated. In this case
if a route references the arp entry, the route should be updated.
*/
int32 arp_test6(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	rtl865x_tblAsicDrv_arpParam_t asic_arp_1, asic_arp_2;
	rtl865x_tblAsicDrv_routingParam_t asic_rt_2;
	rtl865x_tblAsicDrv_l2Param_t asic_l2_1, asic_l2_2;
	rtl865x_tblAsicDrv_nextHopParam_t asic_nh1, asic_nh2;
	rtl8651_tblDrv_networkIntfTable_t *netif_t;
	ipaddr_t ipaddr;
	

	START_TEST_CASE();

	{
		/*==================================================
		  * Test: Internal API: the duplicate adding will modify the previous
		  *	      arp info. In this case, if a route references the entry, the
		  *	      route should update is arp info.
		  *==================================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		netif_t = GET_NETWORK_INTERFACE("vlan1", NULL) ;
		ASSERT(netif_t != NULL);

		/* (1) Add Arp entry */
		ipaddr = ntohl(inet_addr("192.168.1.254"));
		ASSERT(_rtl8651_addLocalArp(TRUE, ipaddr, &mac, netif_t, 0x01) == SUCCESS);

		/* Verify ARP Table */
		bzero(&asic_arp_1, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		bzero(&asic_arp_2, sizeof(rtl865x_tblAsicDrv_arpParam_t));
		asic_arp_1.nextHopRow		= rtl8651_filterDbIndex(&mac, 0);
		asic_arp_1.nextHopColumn	= 0;
		asic_arp_1.aging			= 31;
		ASSERT(rtl8651_getAsicArp(254, &asic_arp_2) == SUCCESS);
		ARP_ENTRY_CMP(&asic_arp_1, &asic_arp_2);

		/* Verify L2 Table */
		bzero(&asic_l2_1, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		bzero(&asic_l2_2, sizeof(rtl865x_tblAsicDrv_l2Param_t));
		memcpy(&asic_l2_1.macAddr, &mac, 6);
		asic_l2_1.memberPortMask	= 0x02;
		asic_l2_1.ageSec			= 300;
		asic_l2_1.cpu				= 0;
		asic_l2_1.srcBlk			= 0;
		asic_l2_1.isStatic			= 0;	/* Currently, we always write it as dynamic entry and set */
		asic_l2_1.nhFlag			= 1;	/* nhFlag to avoid round up when writing it as static entry */
		asic_l2_1.fid				= 0;
		asic_l2_1.auth				= 0;
		ASSERT(rtl8651_getAsicL2Table(rtl8651_filterDbIndex(&mac, 0), 0,  &asic_l2_2) == SUCCESS);
		L2_ENTRY_CMP(&asic_l2_1, &asic_l2_2);

		/* (2) add default route */
		ASSERT(rtl8651_addRoute(0, 0, "vlan1", ipaddr) == SUCCESS);

		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh2, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= FALSE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= rtl8651_filterDbIndex(&mac, 0);
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);

		/* (3) add arp again (different MAC addr) */
		mac.octet[5] ++;
		ASSERT(_rtl8651_addLocalArp(TRUE, ipaddr, &mac, netif_t, 0x02) == SUCCESS);

		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh2, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= FALSE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= rtl8651_filterDbIndex(&mac, 0);
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);

	}
	return SUCCESS;
}

/*
@func int32 | arp_test7 | ARP entry testing.
@comm
In public API, the duplicate arp adding is forbidden.
*/
int32 arp_test7(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	ipaddr_t ipaddr;


	START_TEST_CASE();

	{
		/*==================================================
		  * Test: Released API: duplicate adding is not allowed.
		  *==================================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		ipaddr = ntohl(inet_addr("192.168.1.254"));
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0x01) == SUCCESS);
		mac.octet[5]++;
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0x01) != SUCCESS);
	}
	return SUCCESS;
}

/*
@func int32 | arp_test8 | ARP entry testing.
@comm
Test internal API, rtl8651_addArp() is correct.
*/
int32 arp_test8(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	rtl8651_tblDrv_networkIntfTable_t *netif_t;
	rtl8651_tblDrv_arpEntry_t *arp_t;
	ipaddr_t ipaddr;


	START_TEST_CASE();

	{
		/*========================================
		  * Test: Internal API: _rtl8651_getArpEntry()
		  *========================================*/
		default_configuration1(RTL8651_LL_VLAN, RTL8651_LL_VLAN);

		/* (1) add ARP entry */
		ipaddr = ntohl(inet_addr("192.168.1.250"));
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0x01) == SUCCESS);
		mac.octet[5]++;
		ipaddr = ntohl(inet_addr("192.168.1.254"));
		ASSERT(rtl8651_addArp(ipaddr, &mac, "vlan1", 0x01) == SUCCESS);

		/* (2) find the specified ARP */
		ipaddr = ntohl(inet_addr("192.168.1.254"));
		arp_t = _rtl8651_getArpEntry(NULL, ipaddr, FALSE);
		ASSERT( arp_t != NULL);
		ASSERT(arp_t->ipAddr == ipaddr);

		/* (3) find the specified ARP */
		ipaddr = ntohl(inet_addr("192.168.1.250"));
		netif_t = GET_NETWORK_INTERFACE("vlan1", NULL) ;
		ASSERT(netif_t != NULL);
		arp_t = _rtl8651_getArpEntry(netif_t, ipaddr, FALSE) ;
		ASSERT(arp_t != NULL);
		ASSERT(arp_t->ipAddr == ipaddr);
	}
	return SUCCESS;
}

#define RTL865X_IP_INTERFACE_TESTING_CODE

/* 
@func int32 | ip_interface_test_1 | IP Interface (subnet) testing.
@comm
Add and delete a IP interface (subnet).
*/
int32 ip_interface_test_1(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 } };
	ipaddr_t ipaddr, mask;
	uint32 round;

	START_TEST_CASE();

	{
		/*====================================
		  * Test: IP interface add/delete
		  *====================================*/
		/* (1) add Ip interface */
		ASSERT(rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS);
		ASSERT(rtl8651_addNetworkIntf("vlan1") == SUCCESS);
		ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("vlan1", RTL8651_LL_VLAN, 1) == SUCCESS);
		ipaddr = ntohl(inet_addr("192.168.1.1"));
		mask = ntohl(inet_addr("255.255.255.0"));

		for ( round = 0; round < 1000; round++ )
		{
			ASSERT(rtl8651_addIpIntf("vlan1", ipaddr, mask) == SUCCESS);

			/* (2) remove Ip interface */
			ASSERT(rtl8651_delIpIntf("vlan1", ipaddr,  mask) == SUCCESS);
		}
	}
	return SUCCESS;
}

#define RTL865X_PPPoE_TESTING_CODE

/*
@func int32 | pppoe_test1 | PPPoE testing.
@comm
Add and delete PPPoE session. Overflow PPPoE table and then remove it one by one.
*/
int32 pppoe_test1(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x03, 0x04, 0x05, 0x06, 0x01 } };
	uint32 session;


	START_TEST_CASE();

	{
		/*======================================= 
		  * Test: add/delete PPPoE session (old model)
		  *	      MAX session: 8
		  *=======================================*/
		ASSERT(rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS);
		ASSERT(rtl8651_addNetworkIntf("pppoe") == SUCCESS);
		ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("pppoe", RTL8651_LL_PPPOE, 1) == SUCCESS);

		/* (1) add session: up to 8 sessions */
		for ( session = 0; session < RTL8651_PPPOETBL_SIZE; session++ )
		{
			ASSERT( rtl8651_addPppoeSession( 1000 + session, 1) == SUCCESS );	/* Why we can add 8 sessions successfully ? The reason is that we set 'configPara.dialSessionNumber = 8' */
		}
		
		ASSERT( rtl8651_addPppoeSession( 2000, 1 ) != SUCCESS );

		/* (2) remove session */
		for ( session = 0; session < RTL8651_PPPOETBL_SIZE; session++ )
		{		
			ASSERT( rtl8651_delPppoeSession( 1000 + session ) == SUCCESS );
		}
		ASSERT( rtl8651_delPppoeSession(2000) != SUCCESS );
	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test2 | PPPoE testing.
@comm
Dial up PPPoE session and then hang up it.
*/
int32 pppoe_test2(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x33, 0x44, 0x55, 0x66, 0x01 } };
	rtl865x_tblAsicDrv_pppoeParam_t asic_pppoe_1, asic_pppoe_2;
	uint32 session;
	

	START_TEST_CASE();

	{
		/*==============================================
		  * Test: set/reset PPPoE property
		  *==============================================*/
		ASSERT(rtl8651_assignVlanMacAddress(1, &mac, 1) == SUCCESS);
		ASSERT(rtl8651_addNetworkIntf("pppoe") == SUCCESS);
		ASSERT(rtl8651_specifyNetworkIntfLinkLayerType("pppoe", RTL8651_LL_PPPOE, 1) == SUCCESS);
		mac.octet[5] = 0x10;

		/* (1) Add PPPoE session */
		for(session=0; session<RTL8651_PPPOETBL_SIZE; session++) {
			/* add PPPoE session and set its property */
			ASSERT(rtl8651_addPppoeSession(1000+session, 1) == SUCCESS);
			ASSERT(rtl8651_setPppoeSessionProperty(1000+session, 11111*(session+1), &mac, 0x01) == SUCCESS);

			/* Verify PPPoe Table */
			bzero(&asic_pppoe_1, sizeof(rtl865x_tblAsicDrv_pppoeParam_t));
			bzero(&asic_pppoe_1, sizeof(rtl865x_tblAsicDrv_pppoeParam_t));
			asic_pppoe_1.sessionId	= 11111*(session+1);
			ASSERT(rtl8651_getAsicPppoe(session, &asic_pppoe_2) == SUCCESS);
			PPPOE_ENTRY_CMP(&asic_pppoe_1, &asic_pppoe_2);
			mac.octet[5] ++;
		}

		/* (2) Del PPPoE Session */
		for(session=0, mac.octet[5]=0x10; session<RTL8651_PPPOETBL_SIZE; session++, mac.octet[5]++) {
			/* Reset PPPoE session */
			ASSERT(rtl8651_resetPppoeSessionProperty(session+1000) == SUCCESS);
		}
	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test3 | PPPoE testing.
@comm
Test default route when set PPPoE session.
*/
int32 pppoe_test3(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x33, 0x44, 0x55, 0x66, 0x01 } };
	rtl865x_tblAsicDrv_routingParam_t asic_rt_1, asic_rt_2;
	rtl865x_tblAsicDrv_nextHopParam_t asic_nh1, asic_nh2;
	ipaddr_t ipaddr;

	START_TEST_CASE();

	{
		/*==============================================
		  * Test: add default route before/after dial-up pppoe session.
		  *==============================================*/
		default_configuration3();

		/* (1) add default route to PPPoE */
		ASSERT(rtl8651_addRoute(0, 0, "pppoe", 0) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_NEXTHOP;
		asic_rt_1.nhStart	= 2;
		asic_rt_1.nhNum	= 2;
		asic_rt_1.nhNxt	= 2;
		asic_rt_1.nhAlgo	= 2;
		asic_rt_1.ipDomain	= 6;

		/* Verify nexthop Table */
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;		/* destination network interface */
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= TRUE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= 5; 
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);

		/* (2) dial up PPPoE session */
		ipaddr = ntohl(inet_addr("140.113.1.1"));
		ASSERT(rtl8651_setPppoeSessionProperty(1000, 11111, &mac, 0x01) == SUCCESS);
		ASSERT(rtl8651_addIpIntf("pppoe", ipaddr, 0xffffffff) == SUCCESS);
		ASSERT(rtl8651_bindSession(ipaddr, 1000) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_NEXTHOP;
		asic_rt_1.nhStart	= 2;
		asic_rt_1.nhNum	= 2;
		asic_rt_1.nhNxt	= 2;
		asic_rt_1.nhAlgo	= 2;
		asic_rt_1.ipDomain	= 6;

		/* Verify nexthop Table */
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= TRUE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= 69; 
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);
	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test10 | PPPoE testing.
@comm
Test add and delete PPPoE session (use new Model).
*/
int32 pppoe_test10(uint32 caseNo)
{
	
	START_TEST_CASE();

	{
		/*==============================================
		  * Test: add/delete PPPoE session by using new API
		  *==============================================*/
		pppoe_default_configuration();

		/* original network interface */
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1000, "pppoe", 1) == SUCCESS);
		ASSERT(_rtl8651_getPPPInfo(1000, 0) != NULL);
		ASSERT(rtl8651_delSession(1000) == SUCCESS);
		ASSERT(_rtl8651_getPPPInfo(1000, 0) == NULL);

		/* new network interface */
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1000, "pppoe1", 1) == SUCCESS);
		ASSERT(_rtl8651_getPPPInfo(1000, 0) != NULL);
		ASSERT(rtl8651_delSession(1000) == SUCCESS);
		ASSERT(_rtl8651_getPPPInfo(1000, 0) == NULL);
	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test11 | PPPoE testing.
@comm
Dial up PPPoE session and then hang up it. (use new Model)
*/
int32 pppoe_test11(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x33, 0x44, 0x55, 0x66, 0x11 } };
	rtl865x_tblAsicDrv_pppoeParam_t asic_pppoe1, asic_pppoe2;
	ipaddr_t ipaddr;

	START_TEST_CASE();

	{
		/*==============================================
		  * Test: set/resest pppoe property
		  *==============================================*/
		pppoe_default_configuration();
		/* original network interface */
		/* (1) set pppoe session property */
		ipaddr = ntohl(inet_addr("140.113.1.1"));
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1000, "pppoe", 1) == SUCCESS);
		ASSERT(rtl8651_setPppoeSessionProperty(1000, 11111, &mac, 0x01) == SUCCESS);
		ASSERT(rtl8651_addIpIntf("pppoe", ipaddr, 0xffffffff) == SUCCESS);
		ASSERT(rtl8651_bindSession(ipaddr, 1000) == SUCCESS);

		/* Verify PPPoE TAble */
		bzero(&asic_pppoe1, sizeof(rtl865x_tblAsicDrv_pppoeParam_t));
		bzero(&asic_pppoe2, sizeof(rtl865x_tblAsicDrv_pppoeParam_t));
		asic_pppoe1.age		= 0 ;
		asic_pppoe1.sessionId	= 11111;
		ASSERT(rtl8651_getAsicPppoe(0,  &asic_pppoe2) == SUCCESS);
		PPPOE_ENTRY_CMP(&asic_pppoe1, &asic_pppoe2);

		/* (2) reset pppoe session */
		ASSERT(rtl8651_resetPppoeSessionProperty(1000) == SUCCESS);
		ASSERT(rtl8651_delIpIntf("pppoe", ipaddr, 0xffffffff) == SUCCESS);

		/* new network interface */
		/* (3) set pppoe1 session property */
		mac.octet[5] ++;
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1001, "pppoe1", 1) == SUCCESS);
		ASSERT(rtl8651_setPppoeSessionProperty(1001, 22222, &mac, 0x01) == SUCCESS);
		ASSERT(rtl8651_addIpIntf("pppoe1", ipaddr, 0xffffffff) == SUCCESS);
		ASSERT(rtl8651_bindSession(ipaddr, 1001) == SUCCESS);

		/* Verify PPPoE TAble */
		bzero(&asic_pppoe1, sizeof(rtl865x_tblAsicDrv_pppoeParam_t));
		bzero(&asic_pppoe2, sizeof(rtl865x_tblAsicDrv_pppoeParam_t));
		asic_pppoe1.age		= 0 ;
		asic_pppoe1.sessionId	= 22222;
		ASSERT(rtl8651_getAsicPppoe(1,  &asic_pppoe2) == SUCCESS);
		PPPOE_ENTRY_CMP(&asic_pppoe1, &asic_pppoe2);
		
		/* (2) reset pppoe1 session */
		ASSERT(rtl8651_resetPppoeSessionProperty(1001) == SUCCESS);
		ASSERT(rtl8651_delIpIntf("pppoe1", ipaddr, 0xffffffff) == SUCCESS);
	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test12 | PPPoE testing.
@comm
Add route to PPPoE session. Check the route if correct in dial up state and hang up state separately. The route is added before
the session dial up.
*/
int32 pppoe_test12(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x33, 0x44, 0x55, 0x66, 0x11 } };
	rtl865x_tblAsicDrv_routingParam_t asic_rt_1, asic_rt_2;
	rtl865x_tblAsicDrv_nextHopParam_t asic_nh1, asic_nh2;
	int8 *ifname[] = { "pppoe", "pppoe1" };
	ipaddr_t ipaddr;
	uint32 round=0;
	

	START_TEST_CASE();

	{
		/*==============================================
		  * Test: add route to pppoe
		  *	      add route before dialing up the session
		  *==============================================*/
		pppoe_default_configuration();

again:
		/* (1) add pppoe session */
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1000, ifname[round], 1) == SUCCESS);

		/* (2) add default route to the pppoe session */
		ASSERT(rtl8651_addRoute(0, 0, ifname[round], 0) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_NEXTHOP;
		asic_rt_1.nhStart	= 2;
		asic_rt_1.nhNum	= 2;
		asic_rt_1.nhNxt	= 2;
		asic_rt_1.nhAlgo	= 2;
		asic_rt_1.ipDomain	= 6;

		/* Verify nexthop Table */
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= TRUE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= 5;
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);

		/* (3) dial up the pppoe session */
		ipaddr = ntohl(inet_addr("140.113.1.1"));
		ASSERT(rtl8651_setPppoeSessionProperty(1000, 11111, &mac, 0x01) == SUCCESS);
		ASSERT(rtl8651_addIpIntf(ifname[round], ipaddr, 0xffffffff) == SUCCESS);
		ASSERT(rtl8651_bindSession(ipaddr, 1000) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_NEXTHOP;
		asic_rt_1.nhStart	= 2;
		asic_rt_1.nhNum	= 2;
		asic_rt_1.nhNxt	= 2;
		asic_rt_1.nhAlgo	= 2;
		asic_rt_1.ipDomain	= 6;

		/* Verify nexthop Table */
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= TRUE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= 85; 
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);

		/* (4) reset pppoe session */
		ASSERT(rtl8651_resetPppoeSessionProperty(1000) == SUCCESS);
		ASSERT(rtl8651_delIpIntf(ifname[round], ipaddr, 0xffffffff) == SUCCESS);
		
		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_NEXTHOP;
		asic_rt_1.nhStart	= 2;
		asic_rt_1.nhNum	= 2;
		asic_rt_1.nhNxt	= 2;
		asic_rt_1.nhAlgo	= 2;
		asic_rt_1.ipDomain	= 6;

		/* Verify nexthop Table */
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= TRUE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= 5; 
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);

		/* (5) remove route */
		ASSERT(rtl8651_delSession(1000) != SUCCESS);
		ASSERT(rtl8651_delRoute(0, 0) == SUCCESS);
		
		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_CPU;
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		L3_ENTRY_CMP(&asic_rt_1, &asic_rt_2);

		/* (6) remove session */
		ASSERT(rtl8651_delSession(1000) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_CPU;
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		L3_ENTRY_CMP(&asic_rt_1, &asic_rt_2);

		if (round < 1) 
		{
			round ++;
			goto again;
		}
	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test13 | PPPoE testing.
@comm
Add route to PPPoE session. Check the route if correct in dial up state and hang up state separately. The route is added after
the session dial up.
*/
int32 pppoe_test13(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x33, 0x44, 0x55, 0x66, 0x11 } };
	rtl865x_tblAsicDrv_routingParam_t asic_rt_1, asic_rt_2;
	rtl865x_tblAsicDrv_nextHopParam_t asic_nh1, asic_nh2;
	int8 *ifname[] = { "pppoe", "pppoe1" };
	ipaddr_t ipaddr;
	uint32 round=0;
	

	START_TEST_CASE();

	{
		/*==============================================
		  * Test: add route to pppoe 
		  *	      add route after dialing up the session
		  *==============================================*/
		pppoe_default_configuration();

again:
		/* (1) add pppoe session */
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1000, ifname[round], 1) == SUCCESS);

		/* (2) dial up the pppoe session */
		ipaddr = ntohl(inet_addr("140.113.1.1"));
		ASSERT(rtl8651_setPppoeSessionProperty(1000, 11111, &mac, 0x01) == SUCCESS);
		ASSERT(rtl8651_addIpIntf(ifname[round], ipaddr, 0xffffffff) == SUCCESS);
		ASSERT(rtl8651_bindSession(ipaddr, 1000) == SUCCESS);

		/* (3) add default route to the pppoe session */
		ASSERT(rtl8651_addRoute(0, 0, ifname[round], 0) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_NEXTHOP;
		asic_rt_1.nhStart	= 2;
		asic_rt_1.nhNum	= 2;
		asic_rt_1.nhNxt	= 2;
		asic_rt_1.nhAlgo	= 2;
		asic_rt_1.ipDomain	= 6;

		/* Verify nexthop Table */
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		bzero(&asic_nh1, sizeof(rtl865x_tblAsicDrv_nextHopParam_t));
		ASSERT(rtl8651_getAsicNextHopTable(asic_rt_2.nhStart, &asic_nh1) == SUCCESS);
		asic_nh2.dvid				= 0;
		asic_nh2.extIntIpIdx		= 0;
		asic_nh2.isPppoe			= TRUE;
		asic_nh2.nextHopColumn	= 0;
		asic_nh2.nextHopRow		= 85; 
		asic_nh2.pppoeIdx			= 0;
		NXTHOP_ENTRY_CMP(&asic_nh1, &asic_nh2);


		/* (4) remove route */
		ASSERT(rtl8651_delSession(1000) != SUCCESS);
		ASSERT(rtl8651_delRoute(0, 0) == SUCCESS);

			
		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process 	= RT_CPU;
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		L3_ENTRY_CMP(&asic_rt_1, &asic_rt_2);

		/* (5) reset pppoe session */
		ASSERT(rtl8651_resetPppoeSessionProperty(1000) == SUCCESS);
		ASSERT(rtl8651_delIpIntf(ifname[round], ipaddr, 0xffffffff) == SUCCESS);
		
		/* (6) remove session */
		ASSERT(rtl8651_delSession(1000) == SUCCESS);

		/* Verify L3 Table */
		bzero(&asic_rt_1, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		bzero(&asic_rt_2, sizeof(rtl865x_tblAsicDrv_routingParam_t));
		asic_rt_1.ipAddr	= 0x00;
		asic_rt_1.ipMask	= 0x00;
		asic_rt_1.process	= RT_CPU;
		ASSERT(rtl8651_getAsicRouting(RTL8651_ROUTINGTBL_SIZE - 1,  &asic_rt_2) == SUCCESS);
		L3_ENTRY_CMP(&asic_rt_1, &asic_rt_2);


		if (round < 1) {
			round ++;
			goto again;
		}

	}
	return SUCCESS;
}

/*
@func int32 | pppoe_test14 | PPPoE testing.
@comm
Test API: rtl8651_getRoutingTable.
*/
int32 pppoe_test14(uint32 caseNo)
{
	ether_addr_t mac = { { 0x00, 0x33, 0x44, 0x55, 0x66, 0x11 } };
	int8 *ifname[] = { "pppoe", "pppoe1" };
	ipaddr_t ipaddr;
	uint32 round=0;
	rtl8651_extRouteTable_t *curr_rt;
       rtl8651_extRouteTable_t *next_rt;
	rtl8651_extRouteTable_t *temp_rt;
       next_rt=(rtl8651_extRouteTable_t*)malloc(sizeof (rtl8651_extRouteTable_t));
       curr_rt=(rtl8651_extRouteTable_t*)malloc(sizeof (rtl8651_extRouteTable_t));
	temp_rt=(rtl8651_extRouteTable_t*)malloc(sizeof (rtl8651_extRouteTable_t));


	 

	START_TEST_CASE();

	{
		/*==============================================
		  * Test: API: rtl8651_getRoutingTable.
		  *==============================================*/
		pppoe_default_configuration();

again:
		/* (1) add pppoe session */
		ASSERT(rtl8651_addSession(RTL8651_PPPOE, 1000, ifname[round], 1) == SUCCESS);

		/* (2) dial up the pppoe session */
		ipaddr = ntohl(inet_addr("140.113.1.1"));
		ASSERT(rtl8651_setPppoeSessionProperty(1000, 11111, &mac, 0x01) == SUCCESS);
		ASSERT(rtl8651_addIpIntf(ifname[round], ipaddr, 0xffffffff) == SUCCESS);
		ASSERT(rtl8651_bindSession(ipaddr, 1000) == SUCCESS);

		/* (3) add defaule route to the pppoe session */
		ASSERT(rtl8651_addRoute(0, 0, ifname[round], 0) == SUCCESS);

		/*(4) verify*/
             curr_rt->ipAddr   = ntohl(inet_addr("192.168.2.1"));
	      curr_rt->ipMask  = ntohl(inet_addr("255.255.255.0"));
	      curr_rt->nextHop= ntohl(inet_addr("0"));
	      ASSERT(rtl8651_getRoutingTable(curr_rt, next_rt)==SUCCESS);
	      temp_rt->ipAddr   =0;
	      temp_rt->ipMask  =0;
	      temp_rt->nextHop=0;
	      strncpy(temp_rt->ifName, ifname[round], RTL8651_NETINTF_NAME_LEN);
	      EXTL3_ENTRY_CMP(next_rt,temp_rt);

		ASSERT(rtl8651_delSession(1000) != SUCCESS);
		ASSERT(rtl8651_delRoute(0, 0) == SUCCESS);
		
              /* (5) reset pppoe session */
		ASSERT(rtl8651_resetPppoeSessionProperty(1000) == SUCCESS);
		ASSERT(rtl8651_delIpIntf(ifname[round], ipaddr, 0xffffffff) == SUCCESS);
		
		/* (6) remove session */
		ASSERT(rtl8651_delSession(1000) == SUCCESS);


		if (round < 1) {
			round ++;
			goto again;
		    }
  
		}
	       free(curr_rt);
		free(next_rt);
		free(temp_rt);
	return SUCCESS;

}

#define RTL865X_VLAN_TESTING_CODE

int32 swVlan_test1(uint32 caseNo)
{
	uint32 vid;

	/* Testing rtl8651_addSwVlan() / rtl8651_delSwVlan() API. */
	START_TEST_CASE();
	{
		/* Add an illegal vid to software VLAN table. */
		ASSERT(rtl8651_addSwVlan(0) == TBLDRV_EINVALIDVLANID);
		ASSERT(rtl8651_addSwVlan(4095) == TBLDRV_EINVALIDVLANID);

		/* Add an default vid = 1 (in ASIC) to software VLAN table. */
		ASSERT(rtl8651_addSwVlan(1) == TBLDRV_EVLANALREADYEXISTS);

		/* Add an already-existing vid (in ASIC) to software VLAN table. */
		ASSERT(rtl8651_addVlan(2) == SUCCESS);
		ASSERT(rtl8651_addSwVlan(2) == TBLDRV_EVLANALREADYEXISTS);
		ASSERT(rtl8651_delVlan(2) == SUCCESS);

		/* Add vid = 2 ~ 4094 to software VLAN table. */
		for (vid = 2; vid < 4095; vid++)
			ASSERT(rtl8651_addSwVlan(vid) == SUCCESS);

		/* Add an already-existing vid (in software VLAN) to ASIC VLAN table. */
		for (vid = 2; vid < 4095; vid++)
			ASSERT(rtl8651_addVlan(vid) == TBLDRV_EVLANALREADYEXISTS);

		/* Delete an illegal vid from software VLAN table. */
		ASSERT(rtl8651_delSwVlan(0) == TBLDRV_EINVALIDVLANID);
		ASSERT(rtl8651_delSwVlan(4095) == TBLDRV_EINVALIDVLANID);

		/* Delete an default vid = 1 (in ASIC) from software VLAN table. */
		ASSERT(rtl8651_delSwVlan(1) == TBLDRV_EINVALIDVLANID);
		
		/* Delete vid = 2 ~ 4094 from software VLAN table. */
		for (vid = 2; vid < 4095; vid++)
			ASSERT(rtl8651_delSwVlan(vid) == SUCCESS);

		return SUCCESS;
	}
}

int32 swVlan_test2(uint32 caseNo)
{
	uint32 vid, port;
	
	START_TEST_CASE();
	{
		for (vid = 2; vid < 4095; vid++) {
			ASSERT(rtl8651_addSwVlan(vid) == SUCCESS);
			/* Add each port to VLAN */
			for (port = 0; port < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; port++)
				ASSERT(rtl8651_addVlanPortMember(vid, port) == SUCCESS);
		}			

		/* Link up */
		for (port = 0; port < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; port++)
			rtl8651_setEthernetPortLinkStatus(port, TRUE);

		for (vid = 2; vid < 4095; vid++) {
			/* Delete each port to VLAN */
			for (port = 0; port < RTL8651_PORT_NUMBER + rtl8651_totalExtPortNum; port++)
				ASSERT(rtl8651_delVlanPortMember(vid, port) == SUCCESS);
			ASSERT(rtl8651_delSwVlan(vid) == SUCCESS);
		}
		
		return SUCCESS;
	}
}

