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
int vlanTable_create(uint32_t vid, rtl_vlan_param_t * param);

/* Destroy vlan
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_destroy(uint32_t vid);

/* Add a member port
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_addMemberPort(uint32_t vid, uint32_t portNum);

/* Remove a member port
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_removeMemberPort(uint32_t vid, uint32_t portNum);

/* Set a member port list
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_setMemberPort(uint32_t vid, uint32_t portList);

/* Set ACL rule
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_setAclRule(uint32_t vid, uint32_t inACLStart, uint32_t inACLEnd,
                         uint32_t outACLStart, uint32_t outACLEnd);

/* Get ACL rule
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_getAclRule(uint32_t vid, uint32_t *inACLStart_P, uint32_t *inACLEnd_P,
                         uint32_t *outACLStart_P, uint32_t *outACLEnd_P);

/* Set vlan as internal interface
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_setInternal(uint32_t vid);

/* Set vlan as external interface
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_setExternal(uint32_t vid);

/* Enable hardware routing for this vlan
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_enableHardwareRouting(uint32_t vid);

/* Disable hardware routing for this vlan
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_disableHardwareRouting(uint32_t vid);

/* Set spanning tree status
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_setPortStpStatus(uint32_t vid, uint32_t portNum, uint32_t STPStatus);

/* Get spanning tree status
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_getPortStpStatus(uint32_t vid, uint32_t portNum, uint32_t *STPStatus_P);

/* Set spanning tree status
Return: ENOENT- Specified vlan id does not exist.*/
int vlanTable_setStpStatus(uint32_t vid, uint32_t STPStatus);


/* Get vlan id
Return: ENOENT- Specified slot does not exist.*/
int vlanTable_getVidByIndex(uint32_t eidx, uint32_t * vid_P);

int swCore_netifCreate(uint32_t idx, rtl_netif_param_t * param);
int swCore_netifSetMac(uint32_t idx, unsigned char *mac);

#define CONFIG_RTL865XC 1
#ifdef CONFIG_RTL865XC
/* Hardware bit allocation of VLAN table
*/
typedef struct {
    /* word 0 */
    uint32_t	vid:12;	//CONFIG_RTL8196D
    uint32_t	fid:2;
    uint32_t     extEgressUntag  : 3;
    uint32_t     egressUntag : 6;
    uint32_t     extMemberPort   : 3;
    uint32_t     memberPort  : 6;

    /* word 1 */
    uint32_t          reservw1;
    /* word 2 */
    uint32_t          reservw2;
    /* word 3 */
    uint32_t          reservw3;
    /* word 4 */
    uint32_t          reservw4;
    /* word 5 */
    uint32_t          reservw5;
    /* word 6 */
    uint32_t          reservw6;
    /* word 7 */
    uint32_t          reservw7;
} vlan_table_t;

typedef struct {
    /* word 0 */
    uint32_t          mac18_0:19;
    uint32_t          vid		 : 12;
    uint32_t          valid       : 1;
    /* word 1 */
    uint32_t         inACLStartL:2;
    uint32_t         enHWRoute : 1;
    uint32_t         mac47_19:29;

    /* word 2 */
    uint32_t         mtuL       : 3;
    uint32_t         macMask :3;
    uint32_t         outACLEnd : 7;
    uint32_t         outACLStart : 7;
    uint32_t         inACLEnd : 7;
    uint32_t         inACLStartH: 5;
    /* word 3 */
    uint32_t          reserv10   : 20;
    uint32_t          mtuH       : 12;

    /* word 4 */
    uint32_t          reservw4;
    /* word 5 */
    uint32_t          reservw5;
    /* word 6 */
    uint32_t          reservw6;
    /* word 7 */
    uint32_t          reservw7;
} netif_table_t;


typedef struct {
    union {
        struct {
            /* word 0 */
            uint16_t          dMacP31_16;
            uint16_t          dMacP15_0;
            /* word 1 */
            uint16_t          dMacM15_0;
            uint16_t          dMacP47_32;
            /* word 2 */
            uint16_t          dMacM47_32;
            uint16_t          dMacM31_16;
            /* word 3 */
            uint16_t          sMacP31_16;
            uint16_t          sMacP15_0;
            /* word 4 */
            uint16_t          sMacM15_0;
            uint16_t          sMacP47_32;
            /* word 5 */
            uint16_t          sMacM47_32;
            uint16_t          sMacM31_16;
            /* word 6 */
            uint16_t          ethTypeM;
            uint16_t          ethTypeP;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32_t          reserv1     : 24;
            uint32_t          gidxSel     : 8;
            /* word 1~6 */
            uint32_t          reserv2[6];
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
                    uint8_t           IPProtoM;
                    uint8_t           IPProtoP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint32_t          reserv0     : 20;
                    uint32_t          identSDIPM  : 1;
                    uint32_t          identSDIPP  : 1;
                    uint32_t          HTTPM       : 1;
                    uint32_t          HTTPP       : 1;
                    uint32_t          FOM         : 1;
                    uint32_t          FOP         : 1;
                    uint32_t          IPFlagM     : 3;
                    uint32_t          IPFlagP     : 3;
                    /* word 6 */
                    uint32_t          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8_t           ICMPTypeM;
                    uint8_t           ICMPTypeP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint16_t          reserv0;
                    uint8_t           ICMPCodeM;
                    uint8_t           ICMPCodeP;
                    /* word 6 */
                    uint32_t          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8_t           IGMPTypeM;
                    uint8_t           IGMPTypeP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5,6 */
                    uint32_t          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8_t           TCPFlagM;
                    uint8_t           TCPFlagP;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint16_t          TCPSPLB;
                    uint16_t          TCPSPUB;
                    /* word 6 */
                    uint16_t          TCPDPLB;
                    uint16_t          TCPDPUB;
                } TCP;
                struct {
                    /* word 4 */
                    uint16_t          reserv0;
                    uint8_t           IPTOSM;
                    uint8_t           IPTOSP;
                    /* word 5 */
                    uint16_t          UDPSPLB;
                    uint16_t          UDPSPUB;
                    /* word 6 */
                    uint16_t          UDPDPLB;
                    uint16_t          UDPDPUB;
                } UDP;
            } is;
        } L3L4;

        struct {
            /* word 0 */
            uint16_t          sMacP31_16;
            uint16_t          sMacP15_0;
            /* word 1 */
            uint16_t           reserv1:3;
            uint16_t		   spaP:9;
            uint16_t         sMacM3_0:4;
            uint16_t          sMacP47_32;
            /* word 2 */
            uint32_t	          reserv3:2;
            uint32_t          sVidM:12;
            uint32_t          sVidP:12;
            uint32_t		   reserv2:6;
            /* word 3 */
            uint32_t          reserv5     : 6;
            uint32_t          protoType   : 2;
            uint32_t          reserv4        : 24;
            /* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16_t          SPORTLB;
            uint16_t          SPORTUB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16_t          dMacP31_16;
            uint16_t          dMacP15_0;
            /* word 1 */
            uint16_t 	   vidM:12;
            uint16_t          dMacM3_0:4;
            uint16_t          dMacP47_32;
            /* word 2 */
            uint32_t          reserv2:20;
            uint32_t          vidP:12;
            /* word 3 */
            uint32_t          reserv4     : 24;
            uint32_t          protoType   : 2;
            uint32_t          reserv3:6;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16_t          DPORTLB;
            uint16_t          DPORTUB;
        } DST_FILTER;

    } is;
    /* word 7 */
    uint32_t          reserv0     : 5;
    uint32_t          pktOpApp    : 3;
    uint32_t          PPPoEIndex  : 3;
    uint32_t          vid         : 3;
    uint32_t          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32_t          actionType  : 4;
    uint32_t          ruleType    : 4;
} acl_table_t;

#endif /* RTL865XC */


#endif /*_VLANTABLE_H_*/
