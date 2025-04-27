

#ifndef RTL865XC_TBLASICDRV_H
#define RTL865XC_TBLASICDRV_H

//#ifndef CONFIG_RTL865X_LIGHT_ROMEDRV
#include "rtl_types.h"
//#include "rtl8651_layer2.h"
//#if defined(CONFIG_RTL865X_MULTILAYER_BSP)
//#include "rtl8651_tblDrv.h"
//#endif
//#endif

//#ifdef CONFIG_RTL865X_LIGHT_ROMEDRV
//#define RTL8651_TBLDRV_LOCAL_H
//#include "rtl8651_aclLocal.h"
//#define RTL8651_MAC_NUMBER				6
//#define RTL8651_ACL_PRIORITY				0x0f
//#endif

//#include "rtl8651_hwPatch.h"		/* define for chip related spec */

#if 0
/* ======================================================
	ASIC Driver initiation parameters
    ====================================================== */
typedef struct rtl8651_tblAsic_InitPara_s {

/*			Add Parameters for ASIC initiation                  */
/* ===================================== */
	uint32	externalPHYProperty;
	uint32	externalPHYId[RTL8651_MAC_NUMBER];
/* ===================================== */

} rtl8651_tblAsic_InitPara_t;

/* Definitions for ASIC driver initial parameters */
/* ---------------------------------------------- */

/*	external PHY Property :
	Because there really has many different external PHY, so we just simply define for each MODEL. */
#define RTL8651_TBLASIC_EXTPHYPROPERTY_PORT1234_RTL8212		(1 << 0)
#define RTL8651_TBLASIC_EXTPHYPROPERTY_PORT5_RTL8211B			(1 << 1)

/* Initiation related MACROS */
/* ---------------------------------------------- */

#define ASICDRV_ASSERT(expr) do {\
	if(!(expr)){\
		rtlglue_printf("Error >>> initialize failed at function %s line %d!!!\n", __FUNCTION__, __LINE__);\
			return FAILED;\
	}\
}while(0)

#define ASICDRV_INIT_CHECK(expr) do {\
	if((expr)!=SUCCESS){\
		rtlglue_printf("Error >>> initialize failed at function %s line %d!!!\n", __FUNCTION__, __LINE__);\
			return FAILED;\
	}\
}while(0)

#define ASICDRV_MDCMDIO_READ(phyId, miiRegAddr, varToRead)									\
	do {																							\
		WRITE_MEM32(	MDCIOCR,	(COMMAND_READ & COMMAND_MASK) |							\
									(((phyId) << PHYADD_OFFSET) & PHYADD_MASK) |				\
									(((miiRegAddr) << REGADD_OFFSET) & REGADD_MASK) |			\
									((0 << WRDATA_OFFSET) & WRDATA_MASK)		);				\
		while ((READ_MEM32(MDCIOSR) & STATUS) != 0);		/* Wait for OK */						\
		varToRead = (READ_MEM32(MDCIOSR) & RDATA_MASK) >> RDATA_OFFSET;						\
	} while(0)


#define ASICDRV_MDCMDIO_WRITE(phyId, miiRegAddr, dataToWrite)									\
	do {																							\
		WRITE_MEM32(	MDCIOCR,	(COMMAND_WRITE & COMMAND_MASK) |							\
									(((phyId) << PHYADD_OFFSET) & PHYADD_MASK) |				\
									(((miiRegAddr) << REGADD_OFFSET) & REGADD_MASK) |			\
									(((dataToWrite) << WRDATA_OFFSET) & WRDATA_MASK)		);	\
		while ((READ_MEM32(MDCIOSR) & STATUS) != 0);		/* Wait for OK */						\
	} while(0)

/* ========================================================================== */


/* chip version information */
extern int8 RtkHomeGatewayChipName[16];
extern int32 RtkHomeGatewayChipRevisionID;
extern int32 RtkHomeGatewayChipNameID;
extern rtl8651_tblAsic_InitPara_t rtl8651_tblAsicDrvPara;

#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
void rtl8651_setChipVersion(int8 *name, int32 *rev);
#endif /* RTL865X_TEST */

int32 rtl8651_getChipVersion(int8 *name,uint32 size, int32 *rev);
int32 rtl8651_getChipNameID(int32 *id);

// ASIC specification part
#define RTL8651_PPPOE_NUMBER				8
#define RTL8651_ROUTINGTBL_SIZE			8
#define RTL8651_ARPTBL_SIZE				512
#define RTL8651_PPPOETBL_SIZE				8
#define RTL8651_TCPUDPTBL_SIZE			1024
#define RTL8651_TCPUDPTBL_BITS				10
#define RTL8651_ICMPTBL_SIZE				32
#define RTL8651_ICMPTBL_BITS				5
//#ifdef CONFIG_RTL865XB
#define RTL8651_IPTABLE_SIZE				16
#define RTL8651_SERVERPORTTBL_SIZE			16
#define RTL8651_NXTHOPTBL_SIZE			32
#define RTL8651_RATELIMITTBL_SIZE		32
#define RTL8651_OUTPUTQUEUE_SIZE		6
#define	TOTAL_VLAN_PRIORITY_NUM	8
//#else
//#define RTL8651_IPTABLE_SIZE				8
//#define RTL8651_SERVERPORTTBL_SIZE			8
//#endif /* CONFIG_RTL865XB */
#define RTL865XC_ALGTBL_SIZE				48
/* in order to be consistent to RTL865xB's definition */
#define RTL8651_ALGTBL_SIZE	RTL865XC_ALGTBL_SIZE
#define RTL8651_MULTICASTTBL_SIZE			64
#define RTL8651_IPMULTICASTTBL_SIZE		64
#define RTL8651_NEXTHOPTBL_SIZE			32
#define RTL8651_RATELIMITTBL_SIZE			32
#define RTL8651_MACTBL_SIZE			1024
#define RTL8651_PROTOTRAPTBL_SIZE		8
#define RTL8651_VLANTBL_SIZE			8
#define RTL865XC_NETIFTBL_SIZE			8
#define RTL8651_ACLTBL_SIZE			125
#define ALG_PROTOCOL_TCP1    0x0
#define ALG_PROTOCOL_TCP2    0x1
#define ALG_PROTOCOL_UDP     0x2
#define ALG_PROTOCOL_BOTH    0x3
#define ALG_DIRECTION_INVALID   0
#define ALG_DIRECTION_LANTOWAN  1
#define ALG_DIRECTION_WANTOLAN  2
#define ALG_DIRECTION_BOTH      3
#define GRE_DIRECTION_LANTOWAN  1
#define GRE_DIRECTION_WANTOLAN  2
#define GRE_DIRECTION_BOTH      3
/* Memory mapping of tables 
*/
#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
#define RTL8651_ASICTABLE_BASE_OF_ALL_TABLES pVirtualSWTable
#else
#define RTL8651_ASICTABLE_BASE_OF_ALL_TABLES REAL_SWTBL_BASE
#endif /* RTL865X_TEST */

#define RTL8651_ASICTABLE_ENTRY_LENGTH (8 * sizeof(uint32))
enum {
    TYPE_L2_SWITCH_TABLE = 0,
    TYPE_ARP_TABLE,
    TYPE_L3_ROUTING_TABLE,
    TYPE_MULTICAST_TABLE,
    TYPE_NETINTERFACE_TABLE,
    TYPE_EXT_INT_IP_TABLE,    
    TYPE_VLAN_TABLE,
    TYPE_VLAN1_TABLE,    
    TYPE_SERVER_PORT_TABLE,
    TYPE_L4_TCP_UDP_TABLE,
    TYPE_L4_ICMP_TABLE,
    TYPE_PPPOE_TABLE,
    TYPE_ACL_RULE_TABLE,
    TYPE_NEXT_HOP_TABLE,
    TYPE_RATE_LIMIT_TABLE,
    TYPE_ALG_TABLE,
};

/*#define rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + 0x10000 * (type)) */
#define rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + ((type)<<16) )


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          mac31_16;
    uint16          mac15_0;
    /* word 1 */
    uint16          inACLStartH:1;
    uint16         isDMA	: 1;
    uint16          enHWRoute  : 1;
     uint16          vid        : 12;
    uint16          valid       : 1;
    uint16          mac47_32;
    /* word 2 */
    uint8           reserv5     : 1;
    uint8           outACLEnd   : 7;
    uint8           reserv4     : 1;
    uint8           outACLStart : 7;
    uint8           reserv3     : 1;
    uint8           inACLEnd    : 7;
    uint8           reserv2     : 1;
    uint8           inACLStartL  : 6;
    /* word 3 */
   uint32		reservw3;
    /* word 4 */
	uint32	reservw4;
	/* FIXME: the above variables are define in LITTLE ENDIAN, however not defined in BIG ENDIAN. */
    uint32          STPStatus   : 12;
    uint32          macNotExist     : 1;
    uint32          macMask     : 2;
    uint32          mtuH        : 3;
    uint32          mtuL        : 8;
    uint32          isDMZ           : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint16          mac15_0;
    uint16          mac31_16;
    /* word 1 */
    uint16          mac47_32;
    uint16          vid		 : 12;
    uint16          valid       : 1;
    uint16          enHWRoute       : 1;
	uint16      inACLStartH:1;
    /* word 2 */
    uint8           inACLStartL  : 6;
    uint8           reserv2     : 1;
    uint8           inACLEnd    : 7;
    uint8           reserv3     : 1;
    uint8           outACLStart : 7;
    uint8           reserv4     : 1;
    uint8           outACLEnd   : 7;
    uint8           reserv5     : 1;
    /* word 3 */
    uint32          reserv6  : 1;
    uint32          enHWRoute1   : 1;
    uint32          STPStatus   : 12;
    uint32          reserv7  : 1;
    uint32          reserv8 : 1;
    uint32          reser9 : 6;
    uint32          macMask     : 2;
    uint32          mtuL        : 8;
    /* word 4 */
    uint32          mtuH        : 3;
    uint32          reserv10   : 3;
    uint32          reserv11  : 3;
    uint32          reserv12    : 6;
    uint32          isDMZ           : 1;
    uint32          macNotExist     : 1;
    uint32          reserv13         : 15;
#endif /*_LITTLE_ENDIAN*/
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_vlanTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint32          mac18_0:19;
    uint32          vid		 : 12;
    uint32          valid       : 1;	
    /* word 1 */
    uint32         inACLStartL:2;	
    uint32         enHWRoute : 1;	
    uint32         mac47_19:29;

    /* word 2 */
    uint32         mtuL       : 3;
    uint32         macMask :3;	
    uint32         outACLEnd : 7;	
    uint32         outACLStart : 7;	
    uint32         inACLEnd : 7;	
    uint32         inACLStartH: 5;	
    /* word 3 */
    uint32          reserv10   : 20;
    uint32          mtuH       : 12;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint32          valid       : 1;	
    uint32          vid		 : 12;
    uint32          mac18_0:19;

    /* word 1 */
    uint32         mac47_19:29;
    uint32          enHWRoute      : 1;	
    uint32          inACLStartL:2;	


    /* word 2 */
    uint32         inACLStartH : 5;	
    uint32         inACLEnd : 7;	
    uint32         outACLStart : 7;
    uint32         outACLEnd : 7;	
    uint32         macMask :3;
    uint32         mtuL       : 3;


    /* word 3 */
    uint32          mtuH       : 12;
    uint32          reserv10   : 20;

#endif /*_LITTLE_ENDIAN*/
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_netifTable_t;



typedef struct {
#ifndef _LITTLE_ENDIAN
	 /* word 0 */
	uint32	reserved1:12;
	uint32	fid:2;
	uint32     extEgressUntag  : 3;
	uint32     egressUntag : 6;
	uint32     extMemberPort   : 3;
	uint32     memberPort  : 6;
#else /*_LITTLE_ENDIAN*/
	/* word 0 */
	
	uint32     memberPort  : 6;
	uint32     extMemberPort   : 3;
	uint32     egressUntag : 6;
	uint32     extEgressUntag  : 3;
	uint32	fid:2;
	uint32	reserved1:12;

#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_vlanTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          mac39_24;
    uint16          mac23_8;

    /* word 1 */
    uint32          reserv0: 6;
    uint32          auth: 1;
    uint32          fid:2;
    uint32          nxtHostFlag : 1;
    uint32          srcBlock    : 1;
    uint32          agingTime   : 2;
    uint32          isStatic    : 1;
    uint32          toCPU       : 1;
    uint32          extMemberPort   : 3;
    uint32          memberPort : 6;
    uint32          mac47_40    : 8;

#else /*LITTLE_ENDIAN*/
    /* word 0 */
    uint16          mac23_8;
    uint16          mac39_24;
		
    /* word 1 */
    uint32          mac47_40    : 8;
    uint32          memberPort : 6;
    uint32          extMemberPort   : 3;
    uint32          toCPU       : 1;
    uint32          isStatic    : 1;
    uint32          agingTime   : 2;
    uint32          srcBlock    : 1;
    uint32          nxtHostFlag : 1;
    uint32          fid:2;
    uint32          auth:1;	
    uint32          reserv0:6;	

#endif /*LITTLE_ENDIAN*/
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_l2Table_t;



typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        internalIP;
    /* word 1 */
    ipaddr_t        externalIP;
    /* word 2 */
    uint32          reserv0     : 24;
    uint32          nextHop     : 5;
    uint32          isLocalPublic   : 1;
    uint32          isOne2One       : 1;
    uint32          valid       : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        internalIP;
    /* word 1 */
    ipaddr_t        externalIP;
    /* word 2 */
    uint32          valid       : 1;
    uint32          isOne2One       : 1;
    uint32          isLocalPublic   : 1;
    uint32          nextHop     : 5;
    uint32          reserv0     : 24;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_extIpTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        internalIP;
    /* word 1 */
    ipaddr_t        externalIP;
    /* word 2 */
    uint16          externalPort;
    uint16          internalPort;
    /* word 3 */
    uint32          reserv0     : 25;
    uint32          isPortRange : 1;
    uint32          nextHop     : 5;
    uint32          valid       : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        internalIP;
    /* word 1 */
    ipaddr_t        externalIP;
    /* word 2 */
    uint16          internalPort;
    uint16          externalPort;
    /* word 3 */
    uint32          valid       : 1;
    uint32          nextHop     : 5;
    uint32          isPortRange : 1;
    uint32          reserv0     : 25;
#endif /*_LITTLE_ENDIAN*/
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_srvPortTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        internalIP;
    /* word 1 */
    ipaddr_t        externalIP;
    /* word 2 */
    uint16          externalPort;
    uint16          internalPort;
    /* word 3 */
    uint32          reserv0     : 24;
    uint32	        traffic:1;
    uint32			PID:3;
    uint32		 	PValid:1;
    uint32			protocol:2;
    uint32			isPortRange : 1;

#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        internalIP;
    /* word 1 */
    ipaddr_t        externalIP;
    /* word 2 */
    uint16          internalPort;
    uint16          externalPort;
    /* word 3 */
    uint32          isPortRange : 1;
    uint32		 protocol:2;
    uint32		 PValid:1;
    uint32		 PID:3;
    uint32	        traffic:1;
    uint32          reserv0     : 24;
#endif /*_LITTLE_ENDIAN*/
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_srvPortTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          reserv      : 15;
    uint16          valid       : 1;
    uint16          L4Port;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint16          L4Port;
    uint16          valid       : 1;
    uint16          reserv      : 15;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_algTable_t;



typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          reserv      : 12;
    uint16          direction       : 2;
    uint16          protocol     : 2;
    uint16          L4Port;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint16          L4Port;    
    uint16       	  protocol:2;
    uint16          direction       : 2;
    uint16          reserv      : 12;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_algTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        IPAddr;
    /* word 1 */
    union {
        struct {
            uint32          reserv0     : 5;
            uint32 			ARPIpIdx	: 2;
            uint32          ARPEnd      : 6;
            uint32          ARPStart    : 6;
            uint32          IPMask      : 5;
            uint32          vid         : 3;
            uint32          hPriority   : 1;
            uint32          process     : 3;
            uint32          valid       : 1;
        } ARPEntry;
        struct {
            uint32          reserv0     : 9;
            uint32          nextHop     : 10;
            uint32          IPMask      : 5;
            uint32          vid         : 3;
            uint32          hPriority   : 1;
            uint32          process     : 3;
            uint32          valid       : 1;
        } L2Entry;
        struct {
            uint32          reserv0     : 6;
            uint32          PPPoEIndex  : 3;
            uint32          nextHop     : 10;
            uint32          IPMask      : 5;
            uint32          vid         : 3;
            uint32          hPriority   : 1;
            uint32          process     : 3;
            uint32          valid       : 1;        
        } PPPoEEntry;
        struct {
            uint32          reserv0     : 5;
            uint32          IPDomain    : 3;
            uint32          nhAlgo      : 2;
            uint32          nhNxt       : 5;
            uint32          nhStart     : 4;
            uint32          IPMask      : 5;
            uint32          nhNum       : 3;
            uint32          hPriority   : 1;
            uint32          process     : 3;
            uint32          valid       : 1;
        } NxtHopEntry;
    } linkTo;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        IPAddr;
    /* word 1 */
    union {
        struct {
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          hPriority   : 1;
            uint32          vid         : 3;
            uint32          IPMask      : 5;
            uint32          ARPStart    : 6;
            uint32          ARPEnd      : 6;
            uint32			ARPIpIdx	: 2;
            uint32          reserv0     : 5;
        } ARPEntry;
        struct {
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          hPriority   : 1;
            uint32          vid         : 3;
            uint32          IPMask      : 5;
            uint32          nextHop     : 10;
            uint32          reserv0     : 9;
        } L2Entry;
        struct {
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          hPriority   : 1;
            uint32          vid         : 3;
            uint32          IPMask      : 5;
            uint32          nextHop     : 10;
            uint32          PPPoEIndex  : 3;
            uint32          reserv0     : 6;
        } PPPoEEntry;
        struct {
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          hPriority   : 1;
            uint32          nhNum       : 3;
            uint32          IPMask      : 5;
            uint32          nhStart     : 4;
            uint32          nhNxt       : 5;
            uint32          nhAlgo      : 2;
            uint32          IPDomain    : 3;
            uint32          reserv0     : 5;
        } NxtHopEntry;
    } linkTo;
#endif /*_LITTLE_ENDIAN*/
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_l3RouteTable_t;



typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        IPAddr;

    /* word 1 */
    union {
        struct {
            uint32          reserv0     : 3;
            uint32		  ARPIpIdx	: 3;
            uint32          ARPEnd      : 6;			
            uint32          ARPStart    : 6;
            uint32          netif         : 3;
            uint32          isDMZ      : 1;			
            uint32          internal   : 1;
            uint32          process     : 3;
            uint32          valid       : 1;			
            uint32          IPMask      : 5;
        } ARPEntry;
        struct {
            uint32          reserv0     : 8;
            uint32          nextHop     : 10;
            uint32          netif         : 3;
            uint32          isDMZ      : 1;
            uint32          internal   : 1;
            uint32          process     : 3;			
            uint32          valid       : 1;
            uint32          IPMask      : 5;
        } L2Entry;
        struct {

            uint32          reserv0     : 5;
            uint32          PPPoEIndex  : 3;
            uint32          nextHop     : 10;
	     uint32          netif         : 3;						
            uint32          isDMZ      : 1;
            uint32          internal   : 1;
            uint32          process     : 3;
            uint32          valid       : 1;
            uint32          IPMask      : 5;
        } PPPoEEntry;
        struct {
            uint32          reserv0     : 4;
            uint32          IPDomain    : 3;
            uint32          nhAlgo      : 2;
            uint32          nhNxt       : 5;
            uint32          nhStart     : 4;
            uint32          nhNum       : 3;
	     uint32          isDMZ      : 1;
	     uint32          internal   : 1;
	     uint32          process     : 3;
	     uint32          valid       : 1;
            uint32          IPMask      : 5;			
        } NxtHopEntry;

    } linkTo;
#else /*_LITTLE_ENDIAN*/

    /* word 0 */
    ipaddr_t        IPAddr;
    /* word 1 */
    union {
        struct {
            uint32          IPMask      : 5;
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          internal   : 1;
            uint32          isDMZ      : 1;
			
            uint32          netif         : 3;
            uint32          ARPStart    : 6;
            uint32          ARPEnd      : 6;
            uint32		  ARPIpIdx	: 3;
            uint32          reserv0     : 3;
        } ARPEntry;
        struct {
            uint32          IPMask      : 5;
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          internal   : 1;
            uint32          isDMZ      : 1;

            uint32          netif         : 3;			
            uint32          nextHop     : 10;
            uint32          reserv0     : 8;
        } L2Entry;
        struct {
            uint32          IPMask      : 5;
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          internal   : 1;
            uint32          isDMZ      : 1;
			
	     uint32          netif         : 3;			
            uint32          nextHop     : 10;
            uint32          PPPoEIndex  : 3;
            uint32          reserv0     : 5;
        } PPPoEEntry;
        struct {
            uint32          IPMask      : 5;
            uint32          valid       : 1;
            uint32          process     : 3;
            uint32          internal   : 1;
            uint32          isDMZ      : 1;

		
            uint32          nhNum       : 3;
            uint32          nhStart     : 4;
            uint32          nhNxt       : 5;
            uint32          nhAlgo      : 2;
            uint32          IPDomain    : 3;
            uint32          reserv0     : 4;
        } NxtHopEntry;
    } linkTo;
#endif /*_LITTLE_ENDIAN*/
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_l3RouteTable_t;



typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          reserv0     : 13;
    uint16          ageTime     : 3;
    uint16          sessionID;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint16          sessionID;
    uint16          ageTime     : 3;
    uint16          reserv0     : 13;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_pppoeTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint16          mac39_24;
    uint16          mac23_8;
    /* word 1 */
    uint16          reserv0     : 8;
    uint16          extMemberPort   : 3;
    uint16          nxtHostFlag : 1;
    uint16          srcBlock    : 1;
    uint16          agingTime   : 2;
    uint16          isStatic    : 1;
    uint16          toCPU       : 1;
    uint16          hPriority   : 1;
    uint16          memberPort  : 6;
    uint16          mac47_40    : 8;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint16          mac23_8;
    uint16          mac39_24;
    /* word 1 */
    uint16          mac47_40    : 8;
    uint16          memberPort  : 6;
    uint16          hPriority   : 1;
    uint16          toCPU       : 1;
    uint16          isStatic    : 1;
    uint16          agingTime   : 2;
    uint16          srcBlock    : 1;
    uint16          nxtHostFlag : 1;
    uint16          extMemberPort   : 3;
    uint16          reserv0     : 8;
#endif /*_LITTLE_ENDIAN*/
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_l2Table_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint32          reserv0     : 21;
    uint32          nextHop     : 10;
    uint32          valid       : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint32          valid       : 1;
    uint32          nextHop     : 10;
    uint32          reserv0     : 21;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_arpTable_t;
typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint32          reserv0     : 16;
    uint32          aging:5;
    uint32          nextHop     : 10;
    uint32          valid       : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    uint32          valid       : 1;
    uint32          nextHop     : 10;
    uint32		  aging:5;
    uint32          reserv0     : 21;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_arpTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          reserv0     : 1;
    uint32          selEIdx     : 10;
    uint32          selIPIdx    : 4;
    uint32          isStatic    : 1;
    uint32          dedicate    : 1;
    uint32          collision2  : 1;
    uint32          offset      : 6;
    uint32          agingTime   : 6;
    uint32          collision   : 1;
    uint32          valid       : 1;

    /* word 2 */
    uint32          reserv2     : 12;
    uint32          isTCP       : 1;
    uint32          TCPFlag     : 3;
    uint32          intPort     : 16;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          valid       : 1;
    uint32          collision   : 1;
    uint32          agingTime   : 6;
    uint32          offset      : 6;
    uint32          collision2  : 1;
    uint32          dedicate    : 1;
    uint32          isStatic    : 1;
    uint32          selIPIdx    : 4;
    uint32          selEIdx     : 10;
    uint32          reserv0     : 1;

    /* word 2 */
    uint32          intPort     : 16;
    uint32          TCPFlag     : 3;
    uint32          isTCP       : 1;
    uint32          reserv2     : 12;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_naptTcpUdpTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          reserv0     : 1;
    uint32          selEIdx     : 10;
    uint32          selIPIdx    : 4;
    uint32          isStatic    : 1;
    uint32          dedicate    : 1;
    uint32          collision2  : 1;
    uint32          offset      : 6;
    uint32          agingTime   : 6;
    uint32          collision   : 1;
    uint32          valid       : 1;

    /* word 2 */
    uint32          reserv2     : 8;
    uint32		  priority     :3;
    uint32          priValid	   :1;
    uint32          isTCP       : 1;
    uint32          TCPFlag     : 3;
    uint32          intPort     : 16;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          valid       : 1;
    uint32          collision   : 1;
    uint32          agingTime   : 6;
    uint32          offset      : 6;
    uint32          collision2  : 1;
    uint32          dedicate    : 1;
    uint32          isStatic    : 1;
    uint32          selIPIdx    : 4;
    uint32          selEIdx     : 10;
    uint32          reserv0     : 1;

    /* word 2 */
    uint32          intPort     : 16;
    uint32          TCPFlag     : 3;
    uint32          isTCP       : 1;
    uint32          priValid	   :1;
    uint32		  priority	   :3;
    uint32          reserv2     : 8;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_naptTcpUdpTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32		  intPortL:15;
    uint32          isStatic    : 1;
    uint32          dedicate    : 1;
    uint32          collision2  : 1;
    uint32          offset      : 6;
    uint32          agingTime   : 6;
    uint32          collision   : 1;
    uint32          valid       : 1;

    /* word 2 */
    uint32          reserv2     : 9;
    uint32		    priority    : 3;
    uint32          pvaild      : 1;
    uint32          selEIdx     : 10;
    uint32          selIPIdx    : 4;
    uint32          isTCP       : 1;
    uint32          TCPFlag     : 3;
    uint32          intPortH    : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          valid       : 1;
    uint32          collision   : 1;
    uint32          agingTime   : 6;
    uint32          offset      : 6;
    uint32          collision2  : 1;
    uint32          dedicate    : 1;
    uint32          isStatic    : 1;
    uint32          intPortL    : 15;
	
    /* word 2 */
    uint32          intPortH    : 1;
    uint32          TCPFlag     : 3;
    uint32          isTCP       : 1;
    uint32          selIPIdx    : 4;
    uint32          selEIdx     : 10;
    uint32          pvaild	    : 1;
    uint32		  	priority    : 3;
    uint32          reserv2     : 9;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xcFpga_tblAsic_naptTcpUdpTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          ICMPIDL     : 15;
    uint32          isStatic    : 1;
    uint32          type        : 2;
    uint32          offsetH     : 6;
    uint32          agingTime   : 6;
    uint32          collision   : 1;
    uint32          valid       : 1;
    /* word 2 */
    uint32          reserv2     : 3;
    uint32	        dir         : 2;
    uint32	        offsetL     : 10;	
    uint32          count       : 16;
    uint32          ICMPIDH     : 1;
#else /*_LITTLE_ENDIAN*/
    /* word 0 */
    ipaddr_t        intIPAddr;
    /* word 1 */
    uint32          valid       : 1;
    uint32          collision   : 1;
    uint32          agingTime   : 6;
    uint32          offsetH     : 6;
    uint32          type        : 2;
    uint32          isStatic    : 1;
    uint32          ICMPIDL     : 15;
    /* word 2 */
    uint32          ICMPIDH     : 1;
    uint32          count       : 16;
    uint32          offsetL     : 10;
    uint32          dir 	    : 2;
    uint32          reserv2     : 3;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_naptIcmpTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    union {
        struct {
            /* word 0 */
            uint16          dMacP31_16;
            uint16          dMacP15_0;
            /* word 1 */
            uint16          dMacM15_0;
            uint16          dMacP47_32;
            /* word 2 */
            uint16          dMacM47_32;
            uint16          dMacM31_16;
            /* word 3 */
            uint16          sMacP31_16;
            uint16          sMacP15_0;
            /* word 4 */
            uint16          sMacM15_0;
            uint16          sMacP47_32;
            /* word 5 */
            uint16          sMacM47_32;
            uint16          sMacM31_16;
            /* word 6 */
            uint16          ethTypeM;
            uint16          ethTypeP;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32          reserv1     : 24;
            uint32          gidxSel     : 8;
            /* word 1~6 */
            uint32          reserv2[6];
        } IFSEL;
        struct {
            /* word 0 */
            ipaddr_t        sIPP;
            /* word 1 */
            ipaddr_t        sIPM;
            /* word 2 */
            ipaddr_t        dIPP;
            /* word 3 */
            ipaddr_t        dIPM;
            union {
                struct {
                    /* word 4 */
                    uint8           IPProtoM;
                    uint8           IPProtoP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint32          reserv0     : 20;
                    uint32          identSDIPM  : 1;
                    uint32          identSDIPP  : 1;
                    uint32          HTTPM       : 1;
                    uint32          HTTPP       : 1;
                    uint32          FOM         : 1;
                    uint32          FOP         : 1;
                    uint32          IPFlagM     : 3;
                    uint32          IPFlagP     : 3;
                    /* word 6 */
                    uint32          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8           ICMPTypeM;
                    uint8           ICMPTypeP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          reserv0;
                    uint8           ICMPCodeM;
                    uint8           ICMPCodeP;
                    /* word 6 */
                    uint32          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8           IGMPTypeM;
                    uint8           IGMPTypeP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5,6 */
                    uint32          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8           TCPFlagM;
                    uint8           TCPFlagP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          TCPSPLB;
                    uint16          TCPSPUB;
                    /* word 6 */
                    uint16          TCPDPLB;
                    uint16          TCPDPUB;
                } TCP;
                struct {
                    /* word 4 */
                    uint16          reserv0;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          UDPSPLB;
                    uint16          UDPSPUB;
                    /* word 6 */
                    uint16          UDPDPLB;
                    uint16          UDPDPUB;
                } UDP;
            } is;
        } L3L4;
        struct {
            /* word 0 */
            uint16          sMacP31_16;
            uint16          sMacP15_0;
            /* word 1 */
            uint16          sMacM15_0;
            uint16          sMacP47_32;
            /* word 2 */
            uint16          sMacM47_32;
            uint16          sMacM31_16;
            /* word 3 */
            uint32          reserv2     : 6;
            uint32          protoType   : 2;
            uint32          sVidxM      : 3;
            uint32          sVidxP      : 3;
            uint32          spaM        : 9;
            uint32          spaP        : 9;
            /* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16          SPORTLB;
            uint16          SPORTUB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16          dMacP31_16;
            uint16          dMacP15_0;
            /* word 1 */
            uint16          dMacM15_0;
            uint16          dMacP47_32;
            /* word 2 */
            uint16          dMacM47_32;
            uint16          dMacM31_16;
            /* word 3 */
            uint32          reserv2     : 24;
            uint32          protoType   : 2;
            uint32          vidxM      : 3;
            uint32          vidxP      : 3;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16          DPORTLB;
            uint16          DPORTUB;
        } DST_FILTER;
    } is;
    /* word 7 */
    uint32          pktOpApp    : 3;
    uint32          reserv0     : 4;
    uint32          PPPoEIndex  : 3;
    uint32          vid         : 3;
    uint32          hPriority   : 1;
    uint32          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32          actionType  : 4;
    uint32          ruleType    : 4;
#else
    union {
        struct {
            /* word 0 */
            uint16          dMacP15_0;
            uint16          dMacP31_16;
            /* word 1 */
            uint16          dMacP47_32;
            uint16          dMacM15_0;
            /* word 2 */
            uint16          dMacM31_16;
            uint16          dMacM47_32;
            /* word 3 */
            uint16          sMacP15_0;
            uint16          sMacP31_16;
            /* word 4 */
            uint16          sMacP47_32;
            uint16          sMacM15_0;
            /* word 5 */
            uint16          sMacM31_16;
            uint16          sMacM47_32;
            /* word 6 */
            uint16          ethTypeP;
            uint16          ethTypeM;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32          gidxSel     : 8;
            uint32          reserv1     : 24;
            /* word 1~6 */
            uint32          reserv2[6];
        } IFSEL;
        struct {
            /* word 0 */
            ipaddr_t        sIPP;
            /* word 1 */
            ipaddr_t        sIPM;
            /* word 2 */
            ipaddr_t        dIPP;
            /* word 3 */
            ipaddr_t        dIPM;
            union {
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           IPProtoP;
                    uint8           IPProtoM;
                    /* word 5 */
                    uint32          IPFlagP     : 3;
                    uint32          IPFlagM     : 3;
                    uint32          FOP         : 1;
                    uint32          FOM         : 1;
                    uint32          HTTPP       : 1;
                    uint32          HTTPM       : 1;
                    uint32          identSDIPP  : 1;
                    uint32          identSDIPM  : 1;
                    uint32          reserv0     : 20;

                    /* word 6 */
                    uint32          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           ICMPTypeP;
                    uint8           ICMPTypeM;
                    /* word 5 */
                    uint8           ICMPCodeP;
                    uint8           ICMPCodeM;
                    uint16          reserv0;
                    /* word 6 */
                    uint32          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           IGMPTypeP;
                    uint8           IGMPTypeM;
                    /* word 5,6 */
                    uint32          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           TCPFlagP;
                    uint8           TCPFlagM;
                    /* word 5 */
                    uint16          TCPSPUB;
                    uint16          TCPSPLB;
                    /* word 6 */
                    uint16          TCPDPUB;
                    uint16          TCPDPLB;
                } TCP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint16          reserv0;
                    /* word 5 */
                    uint16          UDPSPUB;
                    uint16          UDPSPLB;
                    /* word 6 */
                    uint16          UDPDPUB;
                    uint16          UDPDPLB;
                } UDP;
            } is;
        } L3L4;
        struct {
            /* word 0 */
            uint16          sMacP15_0;
            uint16          sMacP31_16;
            /* word 1 */
            uint16          sMacP47_32;
            uint16          sMacM15_0;
            /* word 2 */
            uint16          sMacM31_16;
            uint16          sMacM47_32;
            /* word 3 */
            uint32          spaP        : 9;
            uint32          spaM        : 9;
            uint32          sVidxP      : 3;
            uint32          sVidxM      : 3;
            uint32          protoType   : 2;
            uint32          reserv2     : 6;
            /* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16          SPORTUB;
            uint16          SPORTLB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16          dMacP15_0;
            uint16          dMacP31_16;
            /* word 1 */
            uint16          dMacP47_32;
            uint16          dMacM15_0;
            /* word 2 */
            uint16          dMacM31_16;
            uint16          dMacM47_32;
            /* word 3 */
            uint32          vidxP      : 3;
            uint32          vidxM      : 3;
            uint32          protoType   : 2;
            uint32          reserv2     : 24;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16          DPORTUB;
            uint16          DPORTLB;
        } DST_FILTER;
    } is;
    /* word 7 */
    uint32          ruleType    : 4;
    uint32          actionType  : 4;
    uint32          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32          hPriority   : 1;
    uint32          vid         : 3;
    uint32          PPPoEIndex  : 3;
    uint32          reserv0     : 4;
    uint32          pktOpApp    : 3;
#endif /*_LITTLE_ENDIAN*/
} rtl8651_tblAsic_aclTable_t;



typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        srcIPAddr;
    /* word 1 */
    uint32          srcPortL    : 1;
    uint32          srcVid      : 3;
    uint32          destIPAddrLsbs : 28;
    /* word 2*/
    uint32          reserv0     : 11;
    uint32  	  extIPIndexH : 1;
    uint32          ageTime     : 3;
    uint32          extPortList : 3;
    uint32          srcPortExt  : 1;
    uint32          toCPU       : 1;
    uint32          valid       : 1;
    uint32          extIPIndex  : 3;
    uint32          portList    : 6;
    uint32          srcPortH    : 2;
#else
    /* word 0 */
    ipaddr_t        srcIPAddr;
    /* word 1 */
    uint32          destIPAddrLsbs : 28;
    uint32          srcVid      : 3;
    uint32          srcPortL    : 1;
    /* word 2*/
    uint32          srcPortH    : 2;
    uint32          portList    : 6;
    uint32          extIPIndex  : 3;
    uint32          valid       : 1;
    uint32          toCPU       : 1;
    uint32          srcPortExt  : 1;
    uint32          extPortList : 3;
    uint32          ageTime     : 3;
    uint32  	   	extIPIndexH : 1;
    uint32          reserv0     : 11;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_ipMulticastTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint32          reserv0     : 11;
    uint32          nextHop     : 10;
    uint32          PPPoEIndex  : 3;
    uint32          dstVid      : 3;
    uint32          IPIndex     : 4;
    uint32          type        : 1;
#else
    /* word 0 */
    uint32          type        : 1;
    uint32          IPIndex     : 4;
    uint32          dstVid      : 3;
    uint32          PPPoEIndex  : 3;
    uint32          nextHop     : 10;
    uint32          reserv0     : 11;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_nextHopTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    union {
        struct {
            /* word 0 */
            uint16          dMacP31_16;
            uint16          dMacP15_0;
            /* word 1 */
            uint16          dMacM15_0;
            uint16          dMacP47_32;
            /* word 2 */
            uint16          dMacM47_32;
            uint16          dMacM31_16;
            /* word 3 */
            uint16          sMacP31_16;
            uint16          sMacP15_0;
            /* word 4 */
            uint16          sMacM15_0;
            uint16          sMacP47_32;
            /* word 5 */
            uint16          sMacM47_32;
            uint16          sMacM31_16;
            /* word 6 */
            uint16          ethTypeM;
            uint16          ethTypeP;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32          reserv1     : 24;
            uint32          gidxSel     : 8;
            /* word 1~6 */
            uint32          reserv2[6];
        } IFSEL;
        struct {
            /* word 0 */
            ipaddr_t        sIPP;
            /* word 1 */
            ipaddr_t        sIPM;
            /* word 2 */
            ipaddr_t        dIPP;
            /* word 3 */
            ipaddr_t        dIPM;
            union {
                struct {
                    /* word 4 */
                    uint8           IPProtoM;
                    uint8           IPProtoP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint32          reserv0     : 20;
                    uint32          identSDIPM  : 1;
                    uint32          identSDIPP  : 1;
                    uint32          HTTPM       : 1;
                    uint32          HTTPP       : 1;
                    uint32          FOM         : 1;
                    uint32          FOP         : 1;
                    uint32          IPFlagM     : 3;
                    uint32          IPFlagP     : 3;
                    /* word 6 */
                    uint32          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8           ICMPTypeM;
                    uint8           ICMPTypeP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          reserv0;
                    uint8           ICMPCodeM;
                    uint8           ICMPCodeP;
                    /* word 6 */
                    uint32          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8           IGMPTypeM;
                    uint8           IGMPTypeP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5,6 */
                    uint32          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8           TCPFlagM;
                    uint8           TCPFlagP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          TCPSPLB;
                    uint16          TCPSPUB;
                    /* word 6 */
                    uint16          TCPDPLB;
                    uint16          TCPDPUB;
                } TCP;
                struct {
                    /* word 4 */
                    uint16          reserv0;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          UDPSPLB;
                    uint16          UDPSPUB;
                    /* word 6 */
                    uint16          UDPDPLB;
                    uint16          UDPDPUB;
                } UDP;
            } is;
        } L3L4;

        struct {
            /* word 0 */
            uint16          sMacP31_16;
            uint16          sMacP15_0;
            /* word 1 */
            uint16          reserv1:3;
            uint16          spaP:9;
            uint16          sMacM3_0:4;
            uint16          sMacP47_32;
			/* word 2 */
		    uint32	        reserv3:2;
            uint32          sVidM:12;
		    uint32          sVidP:12;
		    uint32		    reserv2:6;
            /* word 3 */
            uint32          reserv5     : 6;
            uint32          protoType   : 2;
     	    uint32          reserv4        : 24;
			/* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16          SPORTLB;
            uint16          SPORTUB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16          dMacP31_16;
            uint16          dMacP15_0;
            /* word 1 */
		    uint16 	        vidP:12;	
            uint16          dMacM3_0:4;
            uint16          dMacP47_32;			
            /* word 2 */
		    uint32          reserv2:20;
		    uint32          vidM:12;			
            /* word 3 */
            uint32          reserv4     : 24;
            uint32          protoType   : 2;
		     uint32         reserv3:6;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16          DPORTLB;
            uint16          DPORTUB;
        } DST_FILTER;

    } is;
    /* word 7 */
    uint32          reserv0     : 5;
    uint32          pktOpApp    : 3;
    uint32          PPPoEIndex  : 3;
    uint32          vid         : 3;
    uint32          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32          actionType  : 4;
    uint32          ruleType    : 4;
#else /* littlen endian*/
    union {
        struct {
            /* word 0 */
            uint16          dMacP15_0;
            uint16          dMacP31_16;
            /* word 1 */
            uint16          dMacP47_32;
            uint16          dMacM15_0;
            /* word 2 */
            uint16          dMacM31_16;
            uint16          dMacM47_32;
            /* word 3 */
            uint16          sMacP15_0;
            uint16          sMacP31_16;
            /* word 4 */
            uint16          sMacP47_32;
            uint16          sMacM15_0;
            /* word 5 */
            uint16          sMacM31_16;
            uint16          sMacM47_32;
            /* word 6 */
            uint16          ethTypeP;
            uint16          ethTypeM;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32          gidxSel     : 8;
            uint32          reserv1     : 24;
            /* word 1~6 */
            uint32          reserv2[6];
        } IFSEL;
        struct {
            /* word 0 */
            ipaddr_t        sIPP;
            /* word 1 */
            ipaddr_t        sIPM;
            /* word 2 */
            ipaddr_t        dIPP;
            /* word 3 */
            ipaddr_t        dIPM;
            union {
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           IPProtoP;
                    uint8           IPProtoM;
                    /* word 5 */
                    uint32          IPFlagP     : 3;
                    uint32          IPFlagM     : 3;
                    uint32          FOP         : 1;
                    uint32          FOM         : 1;
                    uint32          HTTPP       : 1;
                    uint32          HTTPM       : 1;
                    uint32          identSDIPP  : 1;
                    uint32          identSDIPM  : 1;
                    uint32          reserv0     : 20;

                    /* word 6 */
                    uint32          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           ICMPTypeP;
                    uint8           ICMPTypeM;
                    /* word 5 */
                    uint8           ICMPCodeP;
                    uint8           ICMPCodeM;
                    uint16          reserv0;
                    /* word 6 */
                    uint32          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           IGMPTypeP;
                    uint8           IGMPTypeM;
                    /* word 5,6 */
                    uint32          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint8           TCPFlagP;
                    uint8           TCPFlagM;
                    /* word 5 */
                    uint16          TCPSPUB;
                    uint16          TCPSPLB;
                    /* word 6 */
                    uint16          TCPDPUB;
                    uint16          TCPDPLB;
                } TCP;
                struct {
                    /* word 4 */
                    uint8           IPTOSP;
                    uint8           IPTOSM;
                    uint16          reserv0;
                    /* word 5 */
                    uint16          UDPSPUB;
                    uint16          UDPSPLB;
                    /* word 6 */
                    uint16          UDPDPUB;
                    uint16          UDPDPLB;
                } UDP;
            } is;
        } L3L4;
        struct {
            /* word 0 */
            uint16          sMacP15_0;
            uint16          sMacP31_16;
            /* word 1 */
            uint16          sMacP47_32;
            uint16          sMacM3_0:4;
	     uint16		   spaP:9;
	     uint16           reserv1:3;
            /* word 2 */
	     uint32		   reserv2:6;
	     uint32          sVidP:12;
            uint32          sVidM:12;
	     uint32	          reserv3:2;
	     
            /* word 3 */
            uint32          reserv4        : 24;
            uint32          protoType   : 2;
            uint32          reserv5     : 6;
            /* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16          SPORTUB;
            uint16          SPORTLB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16          dMacP15_0;
            uint16          dMacP31_16;

            /* word 1 */
            uint16          dMacP47_32;			
            uint16          dMacM3_0:4;
	     uint16 	   vidP:12;	
            /* word 2 */
	     uint32          vidM:12;			
	     uint32          reserv2:20;
            /* word 3 */
	     uint32          reserv3:6;
            uint32          protoType   : 2;
            uint32          reserv4     : 24;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16          DPORTUB;
            uint16          DPORTLB;
        } DST_FILTER;
    } is;
    /* word 7 */

    uint32          ruleType    : 4;
    uint32          actionType  : 4;
    uint32          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32          vid         : 3;
    uint32          PPPoEIndex  : 3;
    uint32          pktOpApp    : 3;
    uint32          reserv0     : 5;

#endif /*_LITTLE_ENDIAN*/
} rtl865xc_tblAsic_aclTable_t;

typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    ipaddr_t        srcIPAddr;
    /* word 1 */
    uint32          srcVidL      : 4;
    uint32          destIPAddrLsbs : 28;

    /* word 2*/
    uint32          reserv0     : 2;
    uint32  	  extIPIndexH : 1;
    uint32          ageTime     : 3;
    uint32          extPortList : 3;
    uint32          srcPortExt  : 1;
    uint32          toCPU       : 1;
    uint32          valid       : 1;
    uint32          extIPIndex  : 3;
    uint32          portList    : 6;
    uint32          srcPort    : 3;
    uint32		  srcVidH: 8;
#else
    /* word 0 */
    ipaddr_t        srcIPAddr;
    /* word 1 */
    uint32          destIPAddrLsbs : 28;
    uint32          srcVidL      :4 ;
    /* word 2*/
    uint32		  srcVidH:8;
    uint32          srcPort   : 3;
    uint32          portList   : 6;
    uint32          extIPIndex  : 3;
    uint32          valid       : 1;
    uint32          toCPU       : 1;
    uint32          srcPortExt  : 1;
    uint32          extPortList : 3;
    uint32          ageTime     : 3;
    uint32  	  extIPIndexH : 1;
    uint32          reserv0     : 2;

#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_ipMulticastTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint32          reserv0     : 11;
    uint32          nextHop     : 10;
    uint32          PPPoEIndex  : 3;
    uint32          dstnetif     : 3;
    uint32          IPIndex     : 4;
    uint32          type        : 1;
#else
    /* word 0 */
    uint32          type        : 1;
    uint32          IPIndex     : 4;
    uint32          dstnetif      : 3;
    uint32          PPPoEIndex  : 3;
    uint32          nextHop     : 10;
    uint32          reserv0     : 11;
#endif /*_LITTLE_ENDIAN*/
    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_nextHopTable_t;


typedef struct {
#ifndef _LITTLE_ENDIAN
    /* word 0 */
    uint32          reserv0     : 2;
    uint32          refillRemainTime    : 6;
    uint32          token       : 24;
    /* word 1 */
    uint32          reserv1     : 2;
    uint32          refillTime  : 6;
    uint32          maxToken    : 24;
    /* word 2 */
    uint32          reserv2     : 8;
    uint32          refill      : 24;
#else
    /* word 0 */
    uint32          token       : 24;
    uint32          refillRemainTime    : 6;
    uint32          reserv0     : 2;
    /* word 1 */
    uint32          maxToken    : 24;
    uint32          refillTime  : 6;
    uint32          reserv1     : 2;
    /* word 2 */
    uint32          refill      : 24;
    uint32          reserv2     : 8;
#endif /*_LITTLE_ENDIAN*/
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl8651_tblAsic_rateLimitTable_t;

typedef struct _rtl8651_tblAsic_psr_s
{
	uint8 valid;
	uint32 nwayEnable;
	uint32 rxPauseAbility;
	uint32 txPauseAbility;
	uint32 linkUp;
	uint32 duplex;
	uint32 linkSpeed;
} _rtl8651_tblAsic_psr_t;




int32 _rtl8651_addAsicEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 _rtl8651_forceAddAsicEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 _rtl8651_readAsicEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 _rtl8651_delAsicEntry(uint32 tableType, uint32 startEidx, uint32 endEidx);

uint32 _rtl8651_NaptAgingToSec(uint32 value);
uint32 _rtl8651_NaptAgingToUnit(uint32 sec);
uint32 rtl8651_filterDbIndex(ether_addr_t * macAddr,uint16 fid);
uint32 rtl8651_naptTcpUdpTableIndex(int8 isTCP, ipaddr_t srcAddr, uint16 srcPort, ipaddr_t destAddr, uint16 destPort);
#define HASH_UDP      0 /* flag for hash UDP */
#define HASH_TCP      1 /* flag for hash TCP */
#define HASH_FOR_TRAN 0 /* flag for hash hash1 and hash2 index */
#define HASH_FOR_VERI 2 /* flag for hash verification value of enhanced hash1 */

uint32 rtl8651_naptIcmpTableIndex(ipaddr_t srcAddr, uint16 icmpId, ipaddr_t destAddr, uint32 * tblIdx);
uint32 rtl8651_ipMulticastTableIndex(ipaddr_t srcAddr, ipaddr_t dstAddr);

void rtl8651_clearRegister(void);
int32 rtl8651_clearAsicAllTable(void);
#endif


int32 _rtl8651_mapToVirtualRegSpace( void );
int32 _rtl8651_mapToRealRegSpace( void );

#if 0
int32 rtl8651_initAsic(rtl8651_tblAsic_InitPara_t *para);

int32 rtl865xC_setAsicPortPauseFlowControl(uint32 port, uint8 rxEn, uint8 txEn);
int32 rtl8651_setAsicOperationLayer(uint32 layer);
int32 rtl8651_getAsicOperationLayer(void);
int32 rtl8651_setAsicSpanningEnable(int8 spanningTreeEnabled);
int32 rtl8651_getAsicSpanningEnable(int8 *spanningTreeEnabled);
int32 rtl8651_regLinkChangeCallBackFun(void (*callBackFun)(uint32, int8));
void rtl8651_syncLinkStatus( void );
int32 rtl8651_updateLinkStatus(void);
int32 rtl8651_restartAsicEthernetPHYNway(uint32 port);
int32 rtl8651_setAsicEthernetPHYPowerDown( uint32 port, uint32 pwrDown );
int32 rtl8651_setAsicEthernetLinkStatus(uint32 port, int8 linkUp);
int32 rtl8651_getAsicEthernetLinkStatus(uint32 port, int8 *linkUp);
int32 rtl865xC_setAsicEthernetForceModeRegs(uint32 port, uint32 enForceMode, uint32 forceLink, uint32 forceSpeed, uint32 forceDuplex);
int32 rtl8651_setAsicEthernetPHY(uint32 port, int8 autoNegotiation, uint32 advCapability, uint32 speed, int8 fullDuplex);
int32 rtl8651_getAsicEthernetPHY(uint32 port, int8 *autoNegotiation, uint32 *advCapability, uint32 *speed, int8 *fullDuplex);
int32 rtl8651_setAsicEthernetBandwidthControl(uint32 port, int8 input, uint32 rate);
int32 rtl8651_getAsicEthernetBandwidthControl(uint32 port, int8 input, uint32 *rate);
int32 rtl8651_setAsicEthernetBandwidthControlX4(int8 enable);
int32 rtl8651_getAsicEthernetBandwidthControlX4(int8 *enable);
int32 rtl8651_setAsicEthernetBandwidthControlX8(int8 enable);
int32 rtl8651_getAsicEthernetBandwidthControlX8(int8 *enable);
int32 rtl865xC_setAsicEthernetMIIMode(uint32 port, uint32 mode);
int32 rtl865xC_setAsicEthernetRGMIITiming(uint32 port, uint32 Tcomp, uint32 Rcomp);
int32 rtl8651_setAsicEthernetMII(uint32 phyAddress, int32 mode, int32 enabled);
int32 rtl8651_getAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 *rData);
int32 rtl8651_setAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 wData);
int32 rtl8651_setAsicEthernetPHYLoopback(uint32 port, int32 enabled);
int32 rtl8651_getAsicEthernetPHYLoopback(uint32 port, int32 *flag);

int32 rtl8651_setAsicMulticastEnable(uint32 enable);
int32 rtl8651_getAsicMulticastEnable(uint32 *enable);

/* Only exist in RTL865xC */
int32 rtl865xC_setAsicSpanningTreePortState(uint32 port, uint32 portState);
int32 rtl865xC_getAsicSpanningTreePortState(uint32 port, uint32 *portState);

int32 rtl8651_setAsicMulticastSpanningTreePortState(uint32 port, uint32 portState);
int32 rtl8651_getAsicMulticastSpanningTreePortState(uint32 port, uint32 *portState);
int32 rtl8651_setAsicMulticastPortInternal(uint32 port, int8 isInternal);
int32 rtl8651_getAsicMulticastPortInternal(uint32 port, int8 *isInternal);
int32 rtl8651_setAsicMulticastMTU(uint32 mcastMTU);
int32 rtl8651_getAsicMulticastMTU(uint32 *mcastMTU);

typedef struct rtl865x_tblAsicDrv_l2Param_s {
	ether_addr_t	macAddr;
	uint32 		memberPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32 		ageSec;
	uint32	 	cpu:1,
				srcBlk:1,
				isStatic:1,				
				nhFlag:1,
				fid:2,
				auth:1;

} rtl865x_tblAsicDrv_l2Param_t;

int32 rtl8651_setAsicL2Table(uint32 row, uint32 column, rtl865x_tblAsicDrv_l2Param_t *l2p);
int32 rtl8651_delAsicL2Table(uint32 row, uint32 column);
unsigned int rtl8651_asicL2DAlookup(uint8 *dmac);
int32 rtl8651_getAsicL2Table(uint32 row, uint32 column, rtl865x_tblAsicDrv_l2Param_t *l2p);
int32  rtl8651_updateAsicLinkAggregatorLMPR(int32 portmask);
int32 rtl8651_setAsicLinkAggregator(uint32 portMask);
int32 rtl8651_getAsicLinkAggregator(uint32 * portMask, uint32 *mapping);
int32 rtl8651_turnOnHardwiredProtoTrap(uint8 protoType, uint16 protoContent);
int32 rtl8651_turnOffHardwiredProtoTrap(uint8 protoType, uint16 protoContent);
int32 rtl8651_getHardwiredProtoTrap(uint8 protoType, uint16 protoContent, int8 *isEnable);
typedef struct rtl865x_tblAsicDrv_protoTrapParam_s {
	uint8 type;
	uint16 content;
} rtl865x_tblAsicDrv_protoTrapParam_t;
int32 rtl8651_setAsicProtoTrap(uint32 index, rtl865x_tblAsicDrv_protoTrapParam_t *protoTrapp);
int32 rtl8651_delAsicProtoTrap(uint32 index);
int32 rtl8651_getAsicProtoTrap(uint32 index, rtl865x_tblAsicDrv_protoTrapParam_t *protoTrapp);
int32 rtl8651_clearAsicPvid(void);
int32 rtl8651_setAsicPvid(uint32 port, uint32 pvidx);
#if defined(CONFIG_DEFAULTS_KERNEL_2_6)
int32 rtl8651_getAsicPVlanId(uint32 port, uint32 *pvid);
#else
int32 rtl8651_getAsicPvid(uint32 port, uint32 *pvidx);
#endif


typedef struct rtl865x_tblAsicDrv_vlanParam_s {
	uint32 	memberPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32 	untagPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32  fid:2;
} rtl865x_tblAsicDrv_vlanParam_t;

typedef struct rtl865x_tblAsicDrv_intfParam_s {
	ether_addr_t macAddr;
	uint16 	macAddrNumber;
	uint16 	vid;
	uint32 	inAclStart, inAclEnd, outAclStart, outAclEnd;
	uint32 	mtu;
	uint32 	enableRoute:1,
			valid:1;
} rtl865x_tblAsicDrv_intfParam_t;

int32 rtl8651_setAsicVlan(uint16 vid, rtl865x_tblAsicDrv_vlanParam_t *vlanp);
int32 rtl8651_delAsicVlan(uint16 vid);
int32 rtl8651_getAsicVlan(uint16 vid, rtl865x_tblAsicDrv_vlanParam_t *vlanp);
int32 rtl8651_getAsicNetInterface(uint32 idx, rtl865x_tblAsicDrv_intfParam_t *intfp);
int32 rtl8651_setAsicNetInterface(uint32 idx, rtl865x_tblAsicDrv_intfParam_t *intfp);
int32 rtl865x_delNetInterfaceByVid(uint16 vid);
int32 rtl865xc_setPortBasedNetif(uint32 port,uint32 netif) ;
typedef struct rtl865x_tblAsicDrv_pppoeParam_s {
	uint16 sessionId;
	uint16 age;
} rtl865x_tblAsicDrv_pppoeParam_t;
int32 rtl8651_setAsicPppoe(uint32 index, rtl865x_tblAsicDrv_pppoeParam_t *pppoep);
int32 rtl8651_getAsicPppoe(uint32 index, rtl865x_tblAsicDrv_pppoeParam_t *pppoep);
typedef struct rtl865x_tblAsicDrv_routingParam_s {
	    ipaddr_t ipAddr;
	    ipaddr_t ipMask;
	    uint32 process; //0: pppoe, 1:direct, 2:indirect, 4:Strong CPU, 5:napt nexthop
	    uint32 vidx;
	    uint32 arpStart;
	    uint32 arpEnd;
	    uint32 arpIpIdx; /* for RTL8650B C Version Only */
	    uint32 nextHopRow;
	    uint32 nextHopColumn;
	    uint32 pppoeIdx;
	    uint32 nhStart; //exact index
	    uint32 nhNum; //exact number
	    uint32 nhNxt;
	    uint32 nhAlgo;
	    uint32 ipDomain;
	    uint16 	internal:1,
		DMZFlag:1;
		
 	    uint32 netif;
} rtl865x_tblAsicDrv_routingParam_t;
int32 rtl8651_setAsicRouting(uint32 index, rtl865x_tblAsicDrv_routingParam_t *routingp);
int32 rtl8651_delAsicRouting(uint32 index);
int32 rtl8651_getAsicRouting(uint32 index, rtl865x_tblAsicDrv_routingParam_t *routingp);
typedef struct rtl865x_tblAsicDrv_arpParam_s {
	uint32 nextHopRow;
	uint32 nextHopColumn;
	uint32 aging;	
} rtl865x_tblAsicDrv_arpParam_t;
int32 rtl8651_setAsicArp(uint32 index, rtl865x_tblAsicDrv_arpParam_t *arpp);
int32 rtl8651_delAsicArp(uint32 index);
int32 rtl8651_getAsicArp(uint32 index, rtl865x_tblAsicDrv_arpParam_t *arpp);
typedef struct rtl865x_tblAsicDrv_extIntIpParam_s {
	    ipaddr_t 	extIpAddr;
	    ipaddr_t 	intIpAddr;
	    uint32 		nhIndex; //index of next hop table
	    uint32 		localPublic:1,
	           		nat:1;
} rtl865x_tblAsicDrv_extIntIpParam_t;
int32 rtl8651_setAsicExtIntIpTable(uint32 index, rtl865x_tblAsicDrv_extIntIpParam_t *extIntIpp);
int32 rtl8651_delAsicExtIntIpTable(uint32 index);
int32 rtl8651_getAsicExtIntIpTable(uint32 index, rtl865x_tblAsicDrv_extIntIpParam_t *extIntIpp);
typedef struct rtl865x_tblAsicDrv_serverPortParam_s {
	ipaddr_t extIpAddr;
	ipaddr_t intIpAddr;
	uint16 extPort;
	uint16 intPort;
	uint32 nhIndex; //index of next hop table
	uint32 portRange:1;
	uint32 pid;
	uint32 pvaild;
	uint32 protocol;
	uint32 traffic;
	uint32 valid:1;
} rtl865x_tblAsicDrv_serverPortParam_t;
int32 rtl8651_setAsicServerPortTable(uint32 index, rtl865x_tblAsicDrv_serverPortParam_t *serverPortp);
int32 rtl8651_delAsicServerPortTable(uint32 index);
int32 rtl8651_getAsicServerPortTable(uint32 index, rtl865x_tblAsicDrv_serverPortParam_t *serverPortp);
int32 rtl8651_setAsicAgingFunction(int8 l2Enable, int8 l4Enable);
int32 rtl8651_getAsicAgingFunction(int8 * l2Enable, int8 * l4Enable);
int32 rtl8651_setAsicNaptAutoAddDelete(int8 autoAdd, int8 autoDelete);
int32 rtl8651_getAsicNaptAutoAddDelete(int8 *autoAdd, int8 *autoDelete);
int32 rtl8651_setAsicNaptIcmpTimeout(uint32 timeout);
int32 rtl8651_getAsicNaptIcmpTimeout(uint32 *timeout);
int32 rtl8651_setAsicNaptUdpTimeout(uint32 timeout);
int32 rtl8651_getAsicNaptUdpTimeout(uint32 *timeout);
int32 rtl8651_setAsicNaptTcpLongTimeout(uint32 timeout);
int32 rtl8651_getAsicNaptTcpLongTimeout(uint32 *timeout);
int32 rtl8651_setAsicNaptTcpMediumTimeout(uint32 timeout);
int32 rtl8651_getAsicNaptTcpMediumTimeout(uint32 *timeout);
int32 rtl8651_setAsicNaptTcpFastTimeout(uint32 timeout);
int32 rtl8651_getAsicNaptTcpFastTimeout(uint32 *timeout);
//Mirror Port
int32 rtl8651_setAsicPortMirror(uint32 mTxMask, uint32 mRxMask, uint32 mPortMask);
int32 rtl8651_getAsicPortMirror(uint32 *mRxMask, uint32 *mTxMask, uint32 *mPortMask);

typedef struct rtl865x_tblAsicDrv_nextHopParam_s {
	uint32 nextHopRow;
	uint32 nextHopColumn;
	uint32 pppoeIdx;
	uint32 dvid;	//note: dvid means DVID index here! hyking
	uint32 extIntIpIdx;
	uint32 isPppoe:1;
} rtl865x_tblAsicDrv_nextHopParam_t;
int32 rtl8651_setAsicNextHopTable(uint32 index, rtl865x_tblAsicDrv_nextHopParam_t *nextHopp);
int32 rtl8651_getAsicNextHopTable(uint32 index, rtl865x_tblAsicDrv_nextHopParam_t *nextHopp);



//NAPT entryType definitions

#define RTL8651_DYNAMIC_NAPT_ENTRY		(0x0<<0)
#define RTL8651_STATIC_NAPT_ENTRY			(0x1<<0)
#define 	RTL8651_LIBERAL_NAPT_ENTRY			(0x2<<0)

//In RTL8651B, all 3 bits in TCPFlag field is reused if entry is a RTL8651_LIBERAL_NAPT_ENTRY. 
//Don't change these values!!!
#define RTL8651_NAPT_OUTBOUND_FLOW				(1<<2) 	//exact value in ASIC
#define RTL8651_NAPT_INBOUND_FLOW				(0<<2)	//exact value in ASIC
#define RTL8651_NAPT_UNIDIRECTIONAL_FLOW		(2<<2)	//exact value in ASIC
#define RTL8651_NAPT_SYNFIN_QUIET				(4<<2)	//exact value in ASIC 
#define RTL8651_NAPT_CHKAUTOLEARN				(1<<5)
//In RTL8651, TCPFlag field records current state of entry
#define RTL8651_TCPNAPT_WAIT4FIN			(0x4 <<2) //exact value in ASIC
#define RTL8651_TCPNAPT_WAITINBOUND			(0x2 <<2) //exact value in ASIC
#define RTL8651_TCPNAPT_WAITOUTBOUND		(0x1 <<2) //exact value in ASIC


typedef struct rtl865x_tblAsicDrv_naptTcpUdpParam_s {
	ipaddr_t 	insideLocalIpAddr;
	uint16 	insideLocalPort;
	uint32 	ageSec;
	uint8 	tcpFlag;
	uint8 	offset;
	uint8 	selExtIPIdx;
	uint16 	selEIdx;
	uint32 	isTcp:1,
			isCollision:1,
			isStatic:1,
			isCollision2:1,
			isDedicated:1,
			isValid:1,
			priValid:1,
			priority:3;
			
} rtl865x_tblAsicDrv_naptTcpUdpParam_t;
int32 rtl8651_setAsicNaptTcpUdpTable(int8 forced, uint32 index, rtl865x_tblAsicDrv_naptTcpUdpParam_t *naptTcpUdpp);
int32 rtl8651_getAsicNaptTcpUdpTable(uint32 index, rtl865x_tblAsicDrv_naptTcpUdpParam_t *naptTcpUdpp);
int32 rtl8651_delAsicNaptTcpUdpTable(uint32 start, uint32 end);
int32 _rtl8651_findAsicExtIpTableIdx(ipaddr_t extIp);
int32 rtl8651_setAsicRawNaptTable(uint32 index, void * entry, int8 forced);
int32 rtl8651_getAsicRawNaptTable(uint32 index, void  *entry);
int32 rtl8651_setAsicLiberalNaptTcpUdpTable(int8 forced, uint16 index, ipaddr_t insideLocalIpAddr, uint16 insideLocalPort, int8 selExtIPIdx, uint16 insideGlobalPort, uint32 ageSec, int8 entryType, int8 isTcp, int8 isCollision, int8 isCollision2, int8 isValid);

typedef struct rtl865x_tblAsicDrv_naptIcmpParam_s {
	ipaddr_t 	insideLocalIpAddr;
	uint16 	insideLocalId;
	uint16 	ageSec;
	uint16 	offset;
	uint16 	direction;
	uint32 	isStatic:1,
			isCollision:1,
			isSpi:1,
			isPptp:1,
			isValid:1;
	uint16	count;
} rtl865x_tblAsicDrv_naptIcmpParam_t;
int32 rtl8651_setAsicNaptIcmpTable(int8 forced, uint32 index, rtl865x_tblAsicDrv_naptIcmpParam_t *naptIcmpp);
int32 rtl8651_getAsicNaptIcmpTable(uint32 index, rtl865x_tblAsicDrv_naptIcmpParam_t *naptIcmpp);
int32 rtl8651_setAsicL4Offset(uint16 start, uint16 end);
int32 rtl8651_getAsicL4Offset(uint16 *start, uint16 *end);

typedef struct rtl865x_tblAsicDrv_algParam_s {
	uint8 direction;
	uint8 protocol;
	uint16 port;
} rtl865x_tblAsicDrv_algParam_t;
int32 rtl8651_setAsicAlg(uint32 index, rtl865x_tblAsicDrv_algParam_t *algp);
int32 rtl8651_delAsicAlg(uint32 index);
int32 rtl8651_getAsicAlg(uint32 index, rtl865x_tblAsicDrv_algParam_t *algp);
int32 rtl8651_getAsicNaptTcpUdpOffset(uint16 index, uint16 * offset, int8 * isValid);
int32 rtl8651_getAsicNaptIcmpOffset(uint16 index, uint16 * offset, int8 * isValid);

typedef struct rtl865x_tblAsicDrv_multiCastParam_s {
	ipaddr_t	sip;
	ipaddr_t	dip;
	uint16	svid;
	uint16	port;
	uint32	mbr;
	uint16	age;
	uint16	cpu;
	uint16	extIdx;
} rtl865x_tblAsicDrv_multiCastParam_t;

int32 rtl8651_setAsicIpMulticastTable(rtl865x_tblAsicDrv_multiCastParam_t *mCast_t);
int32 rtl8651_delAsicIpMulticastTable(uint32 index);
int32 rtl8651_getAsicIpMulticastTable(uint32 index, rtl865x_tblAsicDrv_multiCastParam_t *mCast_t);
//Counter

typedef struct rtl865x_tblAsicDrv_basicCounterParam_s {
	uint32	mbr;
	uint32	txPackets;
	uint32	txBytes;
	uint32	rxPackets;
	uint32	rxBytes;
	uint32	rxErrors;
	uint32	drops;
	uint32	cpus;
} rtl865x_tblAsicDrv_basicCounterParam_t;


#define ASIC_IN_COUNTERS 					0x01
#define ASIC_OUT_COUNTERS 					0x02
#define ASIC_WHOLE_SYSTEM_COUNTERS 		0x04

typedef struct rtl865x_tblAsicDrv_simpleCounterParam_s 
{
	uint64	rxBytes;
	uint32	rxPkts;
	uint32	rxPausePkts;
	
	uint32	drops;
	
	uint64	txBytes;
	uint32	txPkts;
	uint32	txPausePkts;
	
} rtl865x_tblAsicDrv_simpleCounterParam_t;


typedef struct rtl865x_tblAsicDrv_advancedCounterParam_s 
{
	/*here is in counters  definition*/
	uint64 ifInOctets;
	uint32 ifInUcastPkts;
	uint64 etherStatsOctets;
	uint32 etherStatsUndersizePkts;
	uint32 etherStatsFraments;
	uint32 etherStatsPkts64Octets;
	uint32 etherStatsPkts65to127Octets;
	uint32 etherStatsPkts128to255Octets;
	uint32 etherStatsPkts256to511Octets;
	uint32 etherStatsPkts512to1023Octets;
	uint32 etherStatsPkts1024to1518Octets;
	uint32 etherStatsOversizePkts;
	uint32 etherStatsJabbers;
	uint32 etherStatsMulticastPkts;
	uint32 etherStatsBroadcastPkts;
	uint32 dot1dTpPortInDiscards;
	uint32 etherStatusDropEvents;
	uint32 dot3FCSErrors;
	uint32 dot3StatsSymbolErrors;
	uint32 dot3ControlInUnknownOpcodes;
	uint32 dot3InPauseFrames;

	/*here is out counters  definition*/
	uint64 ifOutOctets;
	uint32 ifOutUcastPkts;
	uint32 ifOutMulticastPkts;
	uint32 ifOutBroadcastPkts;
	uint32 ifOutDiscards;
	uint32 dot3StatsSingleCollisionFrames;
	uint32 dot3StatsMultipleCollisionFrames;
	uint32 dot3StatsDefferedTransmissions;
	uint32 dot3StatsLateCollisions;
	uint32 dot3StatsExcessiveCollisions;
	uint32 dot3OutPauseFrames;
	uint32 dot1dBasePortDelayExceededDiscards;
	uint32 etherStatsCollisions;

	/*here is whole system couters definition*/
	uint32 dot1dTpLearnedEntryDiscards;
	uint32 etherStatsCpuEventPkts;
	
}rtl865x_tblAsicDrv_advancedCounterParam_t;

int32 rtl8651_resetAsicMIBCounter(uint32 port, uint32 inCounterReset, uint32 outCounterReset );
int32 rtl8651_getSimpleAsicMIBCounter(uint32 port, rtl865x_tblAsicDrv_simpleCounterParam_t * simpleCounter);

/*available three kind asic counter type:
ASIC_IN_COUNTERS 					0x01
ASIC_OUT_COUNTERS 					0x02
ASIC_WHOLE_SYSTEM_COUNTERS 		0x04
*/
int32 rtl8651_getAdvancedMIBCounter(uint32 port, uint32 asicCounterType, rtl865x_tblAsicDrv_advancedCounterParam_t * advancedCounter);

int32 rtl8651_returnAsicCounter(uint32 offset);//Backward compatable, deprecated
int32 rtl8651_clearAsicCounter(void);//Backward compatable, deprecated

int32 rtl8651_clearAsicSpecifiedCounter(uint32 counterIdx);//Clear specified ASIC counter
int32 rtl8651_resetAsicCounterMemberPort(uint32 counterIdx);//Clear the specified ASIC counter member port to null set
int32 rtl8651_addAsicCounterMemberPort(uint32 counterIdx, uint32 port);//Add the specified physical port into counter monitor set
int32 rtl8651_delAsicCounterMemberPort(uint32 counterIdx, uint32 port);//Delete the specified physical port into counter monitor set
int32 rtl8651_getAsicCounter(uint32 counterIdx, rtl865x_tblAsicDrv_basicCounterParam_t * basicCounter);


//Rate Limit
typedef struct rtl865x_tblAsicDrv_rateLimitParam_s {
	uint32 	token;
	uint32	maxToken;
	uint32	t_remainUnit;
	uint32 	t_intervalUnit;
	uint32	refill_number;
} rtl865x_tblAsicDrv_rateLimitParam_t;

int32 rtl8651_setAsicRateLimitTable(uint32 index, rtl865x_tblAsicDrv_rateLimitParam_t *rateLimit_t);
int32 rtl8651_delAsicRateLimitTable(uint32 index);
int32 rtl8651_getAsicRateLimitTable(uint32 index, rtl865x_tblAsicDrv_rateLimitParam_t *rateLimit_t);


//Misc.
#define RTL865XC_MAXALLOWED_BYTECOUNT	30360	/* Used for BSCR in RTL865xC. Means max allowable byte count for 10Mbps port */
int32 rtl865xC_setBrdcstStormCtrlRate(uint32 percentage);
int32 rtl8651_setBroadCastStormReg(int8 enable);
int32 rtl8651_getBroadCastSTormReg(int8 *enable);
int32 rtl8651_testAsicDrv(void);

#define RTL865X_PER_PORT_STORM_CONTROL
	
#ifdef RTL865X_PER_PORT_STORM_CONTROL
#define	MULTICAST_STORM_CONTROL	1
#define	BROADCAST_STORM_CONTROL	2
int32 rtl8651_perPortStormControl(uint32 type, uint32 portNum, uint32 enable);
#endif

typedef struct {
    uint32 spa;
    uint32 bc;
    uint32 vid;
    uint32 vlan;
    uint32 pppoe;
    uint8  sip[4];
    uint32 sprt;
                       
    uint8  dip[4];
    uint32 dprt;
    
    uint32 ipptl;
    uint32 ipflg;
    uint32 iptos;
    uint32 tcpflg;
    uint32 type;
    uint32 prtnmat;
	uint32 ethrtype;
    uint8  da[6];
    uint8  pad1[2];
    uint8  sa[6];
    uint8  pad2[2];
    uint32 hp;
    uint32 llc;
    uint32 udp_nocs;
    uint32 ttlst;
    uint32 pktend;
    uint32 dirtx;
    uint32 l4crcok;
    uint32 l3crcok;
    uint32 ipfragif;
    uint32 dp ;
    uint32 hp2;
	uint16	ipLen;
	uint8	L2only;
	
} rtl8651_tblAsic_hsb_param_watch_t;

typedef struct {

	uint8  mac[6];
	uint8  pad1[2];
	uint8  ip[4];
	uint32 prt;
	uint32 l3cs;
	uint32 l4cs;
	uint32 egress;
	uint32 l2act;
	uint32 l34act;
	uint32 dirtx;
	uint32 type;
	uint32 llc;
	uint32 vlan;
	uint32 dvid;
	uint32 pppoe;
	uint32 pppid;
	uint32 ttl_1;
	uint32 dpc;									
	uint32 bc;
	uint32 pktend;
	uint32 mulcst;
	uint32 svid;
	uint32 cpursn;
	uint32 spa;
	uint32 lastfrag;
	uint32 frag;
	uint32 l4csok;
	uint32 l3csok;
	uint32 bc10_5;
	uint32 extSrcPortNum;
	uint32 extDstPortMask;
	uint32 cpuacl;
	uint32 extTTL_1;
} rtl8651_tblAsic_hsa_param_watch_t;










void rtl8651_updateLinkChangePendingCount(void);

int32 rtl8651_getAsicHsB(rtl8651_tblAsic_hsb_param_watch_t * hsbWatch);
int32 rtl8651_getAsicHsA(rtl8651_tblAsic_hsa_param_watch_t * hsaWatch);

extern int8 rtl8651_tblAsicDrv_Id[];

int32 rtl8651_setAsicPortPatternMatch(uint32 port, uint32 pattern, uint32 patternMask, int32 operation);
int32 rtl865xC_dump_flowCtrlRegs(void);
int32 rtl8651_getAsicFlowControlRegister(uint32 port, uint32 *enable);
int32 rtl8651_setAsicFlowControlRegister(uint32 port, uint32 enable);
int32 rtl8651_setAsicHLQueueWeight(uint32 weight);
int32 rtl8651_getAsicQoSControlRegister(uint32 *qoscr);
int32 rtl8651_setAsicDiffServReg(uint32 dscp, int8 highPriority);
int32 rtl8651_getAsicDiffServReg(uint32 *dscr);
int32 rtl8651_getAsicDiffServ(uint32 dscp, int8 *highPriority);

int32 rtl8651_asicEthernetCableMeter(uint32 port, int32 *rxStatus, int32 *txStatus);
int32 rtl8651_getAsicEthernetMII(uint32 *phyAddress);
int32 rtl8651_queryProtocolBasedVLAN( uint32 ruleNo, uint8* ProtocolType, uint16* ProtocolValue );


/*===============================================
 * ASIC DRIVER DEFINITION: Protocol-based VLAN
 *==============================================*/
#define RTL8651_PBV_RULE_IPX				1	/* Protocol-based VLAN rule 1: IPX */
#define RTL8651_PBV_RULE_NETBIOS			2	/* Protocol-based VLAN rule 2: NetBIOS */
#define RTL8651_PBV_RULE_PPPOE_CONTROL		3	/* Protocol-based VLAN rule 3: PPPoE Control */
#define RTL8651_PBV_RULE_PPPOE_SESSION		4	/* Protocol-based VLAN rule 4: PPPoE Session */
#define RTL8651_PBV_RULE_USR1				5	/* Protocol-based VLAN rule 5: user-defined 1 */
#define RTL8651_PBV_RULE_USR2				6	/* Protocol-based VLAN rule 6: user-defined 2 */
#define RTL8651_PBV_RULE_MAX				7

int32 rtl8651_defineProtocolBasedVLAN( uint32 ruleNo, uint8 ProtocolType, uint16 ProtocolValue );
int32 rtl8651_setProtocolBasedVLAN( uint32 ruleNo, uint32 port, uint8 valid, uint16 vlanId);
int32 rtl8651_getProtocolBasedVLAN( uint32 ruleNo, uint32 port, uint8* valid, uint32* vlanIdx );

int32 rtl8651_autoMdiMdix(uint32 port, uint32 isEnable);
int32 rtl8651_getAutoMdiMdix(uint32 port, uint32 *isEnable);
int32 rtl8651_selectMdiMdix(uint32 port, uint32 isMdi);
int32 rtl8651_getSelectMdiMdix(uint32 port, uint32 *isMdi);



/* enum for port ID */
enum PORTID
{
	PHY0 = 0,
	PHY1 = 1,
	PHY2 = 2,
	PHY3 = 3,
	PHY4 = 4,
	PHY5 = 5,
	CPU = 6,
	EXT1 = 7,
	EXT2 = 8,
	EXT3 = 9,
	MULTEXT = 10,
};

/* enum for queue ID */
enum QUEUEID
{
	QUEUE0 = 0,
	QUEUE1,
	QUEUE2,
	QUEUE3,
	QUEUE4,
	QUEUE5,
};

/* enum for queue type */
enum QUEUETYPE
{
	STR_PRIO = 0,
	WFQ_PRIO,
};

/* enum for output queue number */
enum QUEUENUM
{
	QNUM1 = 1,
	QNUM2,
	QNUM3,
	QNUM4,
	QNUM5,
	QNUM6,
};

/* enum for priority value type */
enum PRIORITYVALUE
{
	PRI0 = 0,
	PRI1,
	PRI2,
	PRI3,
	PRI4,
	PRI5,
	PRI6,
	PRI7,
};


/*=========================================
  * ASIC DRIVER API: Packet Scheduling Control Register 
  *=========================================*/
int32 rtl8651_setAsicLBParameter( uint32 token, uint32 tick, uint32 hiThreshold );
int32 rtl8651_getAsicLBParameter( uint32* pToken, uint32* pTick, uint32* pHiThreshold );
int32 rtl8651_setAsicQueueRate( enum PORTID port, enum QUEUEID queueid, uint32 pprTime, uint32 aprBurstSize, uint32 apr );
int32 rtl8651_getAsicQueueRate( enum PORTID port, enum QUEUEID queueid, uint32* pPprTime, uint32* pAprBurstSize, uint32* pApr );
int32 rtl8651_setAsicPortIngressBandwidth( enum PORTID port, uint32 bandwidth );
int32 rtl8651_getAsicPortIngressBandwidth( enum PORTID port, uint32* pBandwidth );
int32 rtl8651_setAsicPortEgressBandwidth( enum PORTID port, uint32 bandwidth );
int32 rtl8651_getAsicPortEgressBandwidth( enum PORTID port, uint32* pBandwidth );
int32 rtl8651_setAsicQueueWeight( enum PORTID port, enum QUEUEID queueid, enum QUEUETYPE queueType, uint32 weight );
int32 rtl8651_getAsicQueueWeight( enum PORTID port, enum QUEUEID queueid, enum QUEUETYPE *pQueueType, uint32 *pWeight );


/*=========================================
  * ASIC DRIVER API: Remarking Control Register 
  *=========================================*/
int32 rtl8651_setAsicDot1pRemarkingAbility( enum PORTID port, uint32 isEnable );
int32 rtl8651_getAsicDot1pRemarkingAbility( enum PORTID port, uint32* isEnable );
int32 rtl8651_setAsicDot1pRemarkingParameter( enum PRIORITYVALUE priority, enum PRIORITYVALUE newpriority );
int32 rtl8651_getAsicDot1pRemarkingParameter( enum PRIORITYVALUE priority, enum PRIORITYVALUE *pNewpriority );
int32 rtl8651_setAsicDscpRemarkingAbility( enum PORTID port, uint32 isEnable );
int32 rtl8651_getAsicDscpRemarkingAbility( enum PORTID port, uint32* isEnable );
int32 rtl8651_setAsicDscpRemarkingParameter( enum PRIORITYVALUE priority, uint32 newdscp );
int32 rtl8651_getAsicDscpRemarkingParameter( enum PRIORITYVALUE priority, uint32* pNewdscp );


/*=========================================
  * ASIC DRIVER API: Priority Assignment Control Register 
  *=========================================*/
int32 rtl8651_setAsicPriorityDecision( uint32 portpri, uint32 dot1qpri, uint32 dscppri, uint32 aclpri, uint32 natpri );
int32 rtl8651_getAsicPriorityDecision( uint32* pPortpri, uint32* pDot1qpri, uint32* pDscppri, uint32* pAclpri, uint32* pNatpri );
int32 rtl8651_setAsicPortPriority( enum PORTID port, enum PRIORITYVALUE priority );
int32 rtl8651_getAsicPortPriority( enum PORTID port, enum PRIORITYVALUE *pPriority );
int32 rtl8651_setAsicDot1qAbsolutelyPriority( enum PRIORITYVALUE srcpriority, enum PRIORITYVALUE priority );
int32 rtl8651_getAsicDot1qAbsolutelyPriority( enum PRIORITYVALUE srcpriority, enum PRIORITYVALUE *pPriority );
int32 rtl8651_setAsicDscpPriority( uint32 dscp, enum PRIORITYVALUE priority );
int32 rtl8651_getAsicDscpPriority( uint32 dscp, enum PRIORITYVALUE *pPriority );
int32 rtl8651_setAsicPriorityToQIDMappingTable( enum QUEUENUM qnum, enum PRIORITYVALUE priority, enum QUEUEID qid );
int32 rtl8651_getAsicPriorityToQIDMappingTable( enum QUEUENUM qnum, enum PRIORITYVALUE priority, enum QUEUEID* pQid );
int32 rtl8651_getAsicCPUPriorityToQIDMappingTable( enum PORTID port, enum PRIORITYVALUE priority, enum QUEUEID *pQid );
int32 rtl8651_setAsicCPUPriorityToQIDMappingTable( enum PORTID port, enum PRIORITYVALUE priority, enum QUEUEID qid );
int32 rtl8651_setAsicOutputQueueNumber( enum PORTID port, enum QUEUENUM qnum );
int32 rtl8651_getAsicOutputQueueNumber( enum PORTID port, enum QUEUENUM *qnum );




/*=========================================
  * ASIC DRIVER API: Output Queue Flow Control Register 
  *=========================================*/
  

int32 rtl8651_setAsicSystemInputFlowControlRegister(uint32 fcON, uint32 fcOFF);
int32 rtl8651_getAsicSystemInputFlowControlRegister(uint32 *fcON, uint32 *fcOFF);
int32 rtl8651_setAsicQueueFlowControlConfigureRegister(enum PORTID port, enum QUEUEID queue, uint32 enable);
int32 rtl8651_getAsicQueueFlowControlConfigureRegister(enum PORTID port, enum QUEUEID queue, uint32 *enable);
int32 rtl8651_getAsicSystemBasedFlowControlRegister(uint32 *sharedON, uint32 *sharedOFF, uint32 *fcON, uint32 *fcOFF, uint32 *drop);
int32 rtl8651_setAsicSystemBasedFlowControlRegister(uint32 sharedON, uint32 sharedOFF, uint32 fcON, uint32 fcOFF, uint32 drop);
int32 rtl8651_getAsicQueueDescriptorBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 *fcON, uint32 *fcOFF);
int32 rtl8651_setAsicQueueDescriptorBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 fcON, uint32 fcOFF);
int32 rtl8651_getAsicQueuePacketBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 *fcON, uint32 *fcOFF);
int32 rtl8651_setAsicQueuePacketBasedFlowControlRegister(enum PORTID port, enum QUEUEID queue, uint32 fcON, uint32 fcOFF);
int32 rtl8651_getAsicPortBasedFlowControlRegister(enum PORTID port, uint32 *fcON, uint32 *fcOFF);
int32 rtl8651_setAsicPortBasedFlowControlRegister(enum PORTID port, uint32 fcON, uint32 fcOFF);
int32 rtl8651_getAsicPerQueuePhysicalLengthGapRegister(uint32 *gap);
int32 rtl8651_setAsicPerQueuePhysicalLengthGapRegister(uint32 gap);
int32 rtl8651_resetAsicOutputQueue(void);
int32 rtl865xC_lockSWCore(void);
int32 rtl865xC_unLockSWCore(void);
int32 rtl865xC_waitForOutputQueueEmpty(void);





uint32 _Is4WayHashEnabled( void );
int32 _set4WayHash( int32 enable );
int32 _rtl8651_enableEnhancedHash1(void);
int32 _rtl8651_disableEnhancedHash1(void);
/* Note: 	!IS_ENTRY_VALID() != IS_ENTRY_INVALID() when "v" is meaningful */
#define IS_ENTRY_VALID(entry)		((entry)->valid & (entry)->v)
#define IS_ENTRY_INVALID(entry)		((entry)->valid==0 && (entry)->v==1)



enum ENUM_NETDEC_POLICY
{
	NETIF_VLAN_BASED = 0x0,   /* Net interface Multilayer-Decision-Based Control by VLAN Based. */
	NETIF_PORT_BASED = 0x1,   /* Net interface Multilayer-Decision-Based Control by PORT Based. */
	NETIF_MAC_BASED = 0x2,    /* Net interface Multilayer-Decision-Based Control by MAC Based. */
};
int32 rtl865xC_setNetDecisionPolicy( enum ENUM_NETDEC_POLICY policy );

/* TTL config */
int32 rtl8651_setAsicTtlMinusStatus(int32 enable);
int32 rtl8651_getAsicTtlMinusStatus(int32 *enable);

int32 rtl865xC_setDefaultACLReg(uint32 isIngress, uint32 start, uint32 end);

int32 rtl8651_setAsic802D1xMacBaseAbility( enum PORTID port, uint32 isEnable );
int32 rtl8651_setAsic802D1xMacBaseDirection(int32 dir);
int32 rtl8651_setAsicGuestVlanProcessControl(uint32 process);

#ifdef CONFIG_RTL865X_LIGHT_ROMEDRV
//ACL
int32 rtl8651_setAsicAclRule(uint32 index, _rtl8651_tblDrvAclRule_t *rule);
int32 rtl8651_getAsicAclRule(uint32 index, _rtl8651_tblDrvAclRule_t *rule);
#endif

int32 rtl8651_initQoSParameter(void);
int32 rtl8651_flushAsicDot1qAbsolutelyPriority(void);
int32 rtl8651_flushAsicDscpRemarkingParameter(void);
int32 rtl8651_setAsicDot1pRemarkingParameter( enum PRIORITYVALUE priority, enum PRIORITYVALUE newpriority );
int32 rtl8651_getAsicDot1pRemarkingParameter( enum PRIORITYVALUE priority, enum PRIORITYVALUE *pNewpriority );
int32 rtl8651_flushAsicDot1pRemarkingParameter(void);
int32 rtl8651_flowContrlThreshold(uint32 a, uint32 value);
int32 rtl8651_flowContrlPrimeThreshold(uint32 a, uint32 value);
int32 rtl865xC_dumpAsicDiagCounter(void);
int32 rtl865xC_dumpAsicCounter(void);
int32 rtl8651_resetSwitchCoreStore(void);
int32 rtl8651_resetSwitchCoreActionAndConfigure(void);
int32 rtl865xC_configAsicEthernetMIIPHYMode(uint32 port, uint32 Tcomp, uint32 Rcomp, 
											uint32 enForceMode, uint32 forceLink, uint32 forceSpeed, uint32 forceDuplex);

#endif
#endif
