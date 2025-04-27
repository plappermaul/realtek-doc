/*
* Copyright c                  Realtek Semiconductor Corporation, 2004
* All rights reserved.
* 
* Program : Forwarding Engine test module
* Abstract : 
* Creator : Edward Jin-Ru Chen (jzchen@realtek.com.tw)
* Author :  
*
* $Id: drvTest.h,v 1.19 2007-02-05 05:16:52 hyking_liu Exp $
*
* $Log: not supported by cvs2svn $
* Revision 1.18  2007/02/02 01:54:29  cw_du
* +:Define NETIF_ENTRY_CMP(entry1, entry2) for RTL865XC.
*
* Revision 1.17  2006/09/01 10:58:20  yjlou
* *: add do {} while(0) for macro
*
* Revision 1.16  2006/07/21 01:51:30  shliu
* *: when model user is defined, include tblComp.h
*
* Revision 1.15  2006/07/18 06:11:40  shliu
* *: add prototype to prevent compiling warning.
*
* Revision 1.14  2006/07/03 03:05:09  chenyl
* *: convert files from DOS format to UNIX format
*
* Revision 1.13  2006/04/11 05:23:34  yjlou
* *: Since the following two macros use IC_TYPE_MODEL, we move them into icTest.h.
*    IS_NOT_EQUAL_INT_DETAIL()
*    IS_EQUAL_INT_DETAIL()
*
* Revision 1.12  2006/04/07 05:25:00  darcy_lu
* +: add extenal compare function
*
* Revision 1.11  2006/03/31 07:02:05  rupert
* +: Add IS_EQUAL_INT_DETAIL
* *: redefine IS_EQUAL_MAC
*
* Revision 1.10  2006/03/24 08:58:51  darcy_lu
* add external routing table compare func code
*
* Revision 1.9  2005/12/21 08:50:31  hao_yu
* +: add macro ALGQOS_ENTRY_CMP for testing ALG QoS table
*
* Revision 1.8  2005/09/27 01:44:21  yjlou
* +: show heximal value for IS_EQUAL_INT() and IS_NOT_EQUAL_INT() for debug.
*
* Revision 1.7  2005/07/20 15:29:38  yjlou
* +: porting Model Code to Linux Kernel: check RTL865X_MODEL_KERNEL.
*
* Revision 1.6  2004/07/06 06:23:14  chhuang
* +: add rate limit testing code
*
* Revision 1.5  2004/05/20 06:41:33  chenyl
* *: fragment system : if (DOS)/(UDP blocking) is disabled, don't queue in-the-order packets
* *: check all dos attacks for each packet
* *: bug fix in port bouncing
* *: apply TCP state tracking in port bouncing
* *: bug fix in multicast system
*
* Revision 1.4  2004/04/23 06:55:25  chhuang
* add new testing code for multicast
*
* Revision 1.3  2004/03/04 09:45:40  chenyl
* *: move some MACROs from fwdTest.c to drvTest.h
*
* Revision 1.2  2004/03/02 08:20:46  tony
* add ALG Client test module
*
*
*/

//#if defined(RTL865X_TEST)||defined(RTL865X_MODEL_USER)
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#endif
#include "assert.h"
#include "rtl_types.h"
//#include "rtl8651_tblDrv.h"

//#ifdef RTL865X_MODEL_USER
//#include "tblComp.h"	/* Include "tblComp.h" for "table comparison function" prototype. Only for RTL865X_MODEL_USER is defined. */
//#endif


/*=======================================
  * Macro definition: Basic Unit comparation
  *=======================================*/
  
#define ASSERT(value)\
do {\
	if (!((int32)(value))) {\
		rtlglue_printf("\t%s(%d): errno=%d, fail, so abort!\n", __FUNCTION__, __LINE__, 0);\
		return FAILED; \
	}\
}while (0)
#define START_TEST_CASE()	rtlglue_printf("@Running testing case %d(%s)......\n", caseNo, __FUNCTION__);

#define IS_EQUAL_BOOL(str, v1, v2, _f_, _l_) \
do { \
	if ( (v1) != (v2) ) { \
		rtlglue_printf("\t%s(%u): %s: %s, %s\n", _f_, _l_, str, (v1==1? "TRUE": "FALSE"), (v2==1? "TRUE": "FALSE")); \
		return FAILED; \
	} \
}while(0) 
#define IS_EQUAL_MAC(str, v1, v2, _f_, _l_) \
do { \
	if (memcmp((int8 *)&(v1), (int8 *)&(v2), 6)) { \
		rtlglue_printf("\t%s(%u): %s: %x:%x:%x:%x:%x:%x, %x:%x:%x:%x:%x:%x\n", _f_, _l_, str, \
		*( (int8*)&v1+0),*( (int8*)&v1+1),*( (int8*)&v1+2),*( (int8*)&v1+3),*( (int8*)&v1+4),*( (int8*)&v1+5),\
		*( (int8*)&v2+0),*( (int8*)&v2+1),*( (int8*)&v2+2),*( (int8*)&v2+3),*( (int8*)&v2+4),*( (int8*)&v2+5));\
		return FAILED; \
	} \
} while(0)
#define IS_EQUAL_IP(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) != (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %d.%d.%d.%d, %d.%d.%d.%d\n", _f_, _l_, str, (v1>>24), (v1&0x00ff0000)>>16, (v1&0x0000ff00)>>8, (v1&0xff), (v2>>24), (v2&0x00ff0000)>>16, (v2&0x0000ff00)>>8, (v2&0xff)); \
		return FAILED; \
	} \
} while (0)



// less or equal
#define IS_LE_INT(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) > (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %d, %d\n", _f_, _l_, str, v1, v2); \
		return FAILED; \
	} \
} while(0) 

// greater or equal
#define IS_GE_INT(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) < (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %d, %d\n", _f_, _l_, str, v1, v2); \
		return FAILED; \
	} \
} while(0)

#define IS_EQUAL_INT(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) != (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %d(0x%08x), %d(0x%08x)\n", _f_, _l_, str, v1, v1, v2, v2); \
		return FAILED; \
	} \
} while(0)

#define IS_NOT_EQUAL_INT(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) == (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %d(0x%08x), %d(0x%08x)\n", _f_, _l_, str, v1, v1, v2, v2); \
		return FAILED; \
	} \
} while(0)


#define IS_EQUAL_POINTER(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) != (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %p, %p\n", _f_, _l_, str, v1, v2); \
		return FAILED; \
	} \
} while(0)
	
#define IS_NOT_EQUAL_POINTER(str, v1, v2, _f_, _l_) \
do { \
	if ((v1) == (v2)) { \
		rtlglue_printf("\t%s(%u): %s: %p, %p\n", _f_, _l_, str, v1, v2); \
		return FAILED; \
	} \
} while(0)
 
#define IS_EQUAL_STR(str, s1, s2, _f_, _l_) \
do { \
	if (strcmp(s1,s2)) {\
		rtlglue_printf("\t%s(%u): %s:\n\t%s\n\t%s\n", _f_, _l_, str, s1, s2); \
		return FAILED; \
	} \
} while(0)


/*=======================================
  * Macro definition: ASIC Table comparation
  *=======================================*/
#define MCAST_ENTRY_CMP(entry1, entry2)		if (mcast_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define L2_ENTRY_CMP(entry1, entry2) 			if (l2_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define VLAN_ENTRY_CMP(entry1, entry2)		if (vlan_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;

#ifdef CONFIG_RTL865XC
#define NETIF_ENTRY_CMP(entry1, entry2)		if (netIf_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#endif

#define L3_ENTRY_CMP(entry1, entry2)			if (l3_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define IP_ENTRY_CMP(entry1, entry2)			if (ip_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define SP_ENTRY_CMP(entry1, entry2)			if (sp_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define PPPOE_ENTRY_CMP(entry1, entry2)		if (pppoe_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define NXTHOP_ENTRY_CMP(entry1, entry2) 	if (nxthop_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ARP_ENTRY_CMP(entry1, entry2)		if (arp_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define TCPUDP_ENTRY_CMP(entry1, entry2)		if (tcpudp_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ICMP_ENTRY_CMP(entry1, entry2)		if (icmp_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ALG_ENTRY_CMP(entry1, entry2)		if (alg_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ACL_ENTRY_CMP(entry1, entry2)		if (acl_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ACL_DEFAULT_TOCPU(index)			if (acl_default_tocpu(index, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ACL_DEFAULT_DROP(index)			if (acl_default_drop(index, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define ACL_DEFAULT_PERMIT(index)			if (acl_default_permit(index, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define RATELIMIT_ENTRY_CMP(entry1, entry2)	if (ratelimit_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define EXTL3_ENTRY_CMP(entry1,entry2)          if (extl3_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#define EXTL2_ENTRY_CMP(entry1,entry2)          if (extl2_entry_cmp(entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__)==FAILED) return FAILED;
#ifdef RTL8651_ALG_QOS
#define ALGQOS_ENTRY_CMP( entry1, entry2 )	if (algQos_entry_cmp( entry1, entry2, (int8 *)__FUNCTION__, (uint32)__LINE__ ) == FAILED ) return FAILED;
#endif
#ifdef CONFIG_RTL865XC
#define NET_INTERFACE_ENTRY_CMP(entry1, entry2) if(network_interface_cmp(entry1,entry2, (int8 *) __FUNCTION__, (uint32) __LINE__) == FAILED) return FAILED;
#endif

/*=======================================
  * Global Variable:
  *=======================================*/
//extern rtl8651_tblDrvInitPara_t configPara;

