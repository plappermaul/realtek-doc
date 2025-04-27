/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/vlanTable.h,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Switch core vlan table access header file.
*
* $Author: yachang $
*
* $Log: vlanTable.h,v $
* Revision 1.1.1.1  2012/02/01 07:50:52  yachang
* First working u-boot for RTL8676
*
*
* Revision 1.1.1.1  2011/06/10 08:06:31  yachang
*
*
* Revision 1.4  2006/09/15 03:53:39  ghhuang
* +: Add TFTP download support for RTL8652 FPGA
*
* Revision 1.3  2005/09/22 05:22:31  bo_zhao
* *** empty log message ***
*
* Revision 1.1.1.1  2005/09/05 12:38:24  alva
* initial import for add TFTP server
*
* Revision 1.2  2004/03/31 01:49:20  yjlou
* *: all text files are converted to UNIX format.
*
* Revision 1.1  2004/03/16 06:36:13  yjlou
* *** empty log message ***
*
* Revision 1.1.1.1  2003/09/25 08:16:56  tony
*  initial loader tree 
*
* Revision 1.1.1.1  2003/05/07 08:16:07  danwu
* no message
*
* ---------------------------------------------------------------
*/

#ifndef _VLANTABLE_H_
#define _VLANTABLE_H_



/* VLAN table access routines 
*/

/* Create vlan 
Return: EEXIST- Speicified vlan already exists.
        ENFILE- Destined slot occupied by another vlan.*/
int32 vlanTable_create(uint32 vid, rtl_vlan_param_t * param);

/* Destroy vlan 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_destroy(uint32 vid);

/* Add a member port
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_addMemberPort(uint32 vid, uint32 portNum);

/* Remove a member port 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_removeMemberPort(uint32 vid, uint32 portNum);

/* Set a member port list 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_setMemberPort(uint32 vid, uint32 portList);

/* Set ACL rule 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_setAclRule(uint32 vid, uint32 inACLStart, uint32 inACLEnd,
                                uint32 outACLStart, uint32 outACLEnd);

/* Get ACL rule 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_getAclRule(uint32 vid, uint32 *inACLStart_P, uint32 *inACLEnd_P,
                                uint32 *outACLStart_P, uint32 *outACLEnd_P);

/* Set vlan as internal interface 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_setInternal(uint32 vid);

/* Set vlan as external interface 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_setExternal(uint32 vid);

/* Enable hardware routing for this vlan 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_enableHardwareRouting(uint32 vid);

/* Disable hardware routing for this vlan 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_disableHardwareRouting(uint32 vid);

/* Set spanning tree status 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_setPortStpStatus(uint32 vid, uint32 portNum, uint32 STPStatus);

/* Get spanning tree status 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_getPortStpStatus(uint32 vid, uint32 portNum, uint32 *STPStatus_P);

/* Set spanning tree status 
Return: ENOENT- Specified vlan id does not exist.*/
int32 vlanTable_setStpStatus(uint32 vid, uint32 STPStatus);


/* Get vlan id 
Return: ENOENT- Specified slot does not exist.*/
int32 vlanTable_getVidByIndex(uint32 eidx, uint32 * vid_P);

#define CONFIG_RTL865XC 1
#ifdef CONFIG_RTL865XC
/* Hardware bit allocation of VLAN table 
*/
typedef struct {
	 /* word 0 */
	uint32	vid:12;	//CONFIG_RTL8196D
	uint32	fid:2;
	uint32     extEgressUntag  : 3;
	uint32     egressUntag : 6;
	uint32     extMemberPort   : 3;
	uint32     memberPort  : 6;

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
} vlan_table_t;

typedef struct {
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

    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} netif_table_t;


typedef struct {
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
	     uint16           reserv1:3;
	     uint16		   spaP:9;
             uint16         sMacM3_0:4;
            uint16          sMacP47_32;
	/* word 2 */
	     uint32	          reserv3:2;
            uint32          sVidM:12;
	     uint32          sVidP:12;
	     uint32		   reserv2:6;
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
	     uint16 	   vidM:12;	
            uint16          dMacM3_0:4;
            uint16          dMacP47_32;			
            /* word 2 */
	     uint32          reserv2:20;
	     uint32          vidP:12;			
            /* word 3 */
            uint32          reserv4     : 24;
            uint32          protoType   : 2;
	     uint32          reserv3:6;
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
} acl_table_t;

#endif /* RTL865XC */


#endif /*_VLANTABLE_H_*/
