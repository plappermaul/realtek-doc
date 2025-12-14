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
 *
 * $Revision$
 * $Date$
 *
 * Purpose : register service APIs in the SDK.
 *
 * Feature : register service APIs
 *
 */

#ifndef __HAL_MAC_DEBUG_H__
#define __HAL_MAC_DEBUG_H__

#include <common/rt_autoconf.h>

struct hsb_param_s
{

    union
    {
        struct
        {
            uint32  sphy:5;
            uint32  l2fmt : 4;
            uint32  pppoe : 1;
            uint32  l34fmt : 4;
            uint32  l4off : 8;
            uint32  ipfo0_n:1;
            uint32  datype : 2;
            uint32  oampdu : 1;
            uint32  cputag : 1;
            uint32  otagExist : 1;
            uint32  opri : 3;
            uint32  dei : 1;
            uint32  ovid : 12;
            uint32  itagExist : 1;
            uint32  ipri : 3;
            uint32  cfi : 1;
            uint32  frameLen: 14;
            uint32  ipv4oph: 1;
            uint32  ivid : 12;
            uint32  typeLen : 16;
            uint32  rtkPro : 3;
            uint32  lb : 1;
            uint32  l2err : 1;
            uint32  l3err : 1;
            uint32  l4err : 1;
            uint32  l2calc: 1;
            uint32  l3calc: 1;  
            uint32  l4calc: 1;  
            uint8   fs1[9];
            uint8   fs2[9];
            uint32 oamTf : 1;
            uint32 trapToCpu : 1;
            uint32 oam_lbsa : 2;
            uint32 oam_lbda : 2;
            uint32 flag : 16;
                #define FC_EN_MASK    (1<<0)
                #define INGRESS_ACL_EN_MASK   (1<<1)
                #define EGRESS_ACL_EN_MASK     (1<<2)
                #define EGRESS_VLAN_TRAN_EN_MASK  (1<<3)
                #define INGRESS_MAX1X_EN_MASK         (1<<4)
                #define EGRESS_MAX1X_EN_MASK          (1<<5)
                #define ATTACK_PREV_EN_MASK           (1<<6)
                #define TAG_STADE_EN_MASK                 (1<<7)
                #define SML_EN_MASK                               (1<<8)
                #define INGRESS_VLAN_EN_MASK              (1<<9)
                #define EGRESS_VLAN_EN_MASK               (1<<10)
                #define LINKAGGR_LOADSHARE_EN_MASK        (1<<11)
                #define EGRESS_SPT_EN_MASK                        (1<<12)
                #define EGRESS_LDP_EN_MASK                        (1<<13)
            uint32 pm;  
            uint8   dmac[6];
            uint8   smac[6]; 
            uint8   payload[47];
            uint32 drop :1;
            uint32 asdpm : 1;				  
            uint32 asdp : 1;				  
            uint32 dp : 2;				  
            uint32 pri : 3;				  
            uint32 asdprmk : 1;	
            uint32 ipv6oph : 1;
            uint32  dpm : 28;
            uint32 reason : 4;
            uint32 rxrspan : 1;
            uint32 extraTag : 1;
            uint32 dfPri :1;
            uint32 insertCpuTag:1;
        } r8328;
        struct
        {
            uint32  sel_hsb:1;
            uint32  valid_hsb : 1;
            uint32  reserved : 7;
            uint32  cfi : 1;
            uint32  patternmatch : 2;
            uint32  flowlabel:20;

            uint32  dstport:16;
            uint32  srcport : 16;

            uint32  tcpflags:8;
            uint32  ipproto : 8;
            uint32  svid:12;
            uint32  spri : 3;
            uint32  rxdrop:1;

            uint32  cputagif:1;
            uint32  cpuintpri : 3;
            uint32  cpuportmask:28;

            uint32  ethtype:16;
            uint32  ipv6mld : 1;
            uint32  cpri:3;
            uint32  cvid:12;

            uint32  dip;
            uint32  sip;

            uint8   dmac[6];
            uint8   smac[6]; 

            uint32  tos:8;
            uint32  cpuasdp : 1;
            uint32  cpuasdpm:1;
            uint32  cpuasdprmk:1;
            uint32  ipv6:1;
            uint32  ipv4:1;
            uint32  pppoe:1;
            uint32  stagif:1;
            uint32  ctagif:1;
            uint32  frametype:2;
            uint32  pktlen:14;

            uint32  l4csok:1;
            uint32  l3csok:1;
            uint32  endpage:11;
            uint32  startpage:11;
            uint32  startbank:3;
            uint32  spa:5;
        } r8389;
    };
};
typedef struct hsb_param_s hsb_param_t;


struct ppi_param_s
{
    uint32  sphy:5;			
    uint32  crsvlan:1 ;		
    uint32  keeporigivid: 1;		
    uint32  keeporigovid: 1;		
    uint32  pbivid: 12;		
    uint32  pbovid: 12;		


    uint32  pbipri: 3;		       
    uint32  pbodei:  1;		
    uint32  ipuc:1;			
    uint32  pbopri: 3; 		
    uint32  ppbivid: 12;		
    uint32  ppbovid: 12;		


    uint32  ppbipri: 3;		
    uint32  ppbodei:  1;		
    uint32  upLinkMac_0:1;	
    uint32  ppbopri: 3;		
    uint32  fcivid: 12;		
    uint32  fcovid: 12;		

    uint32  ttldec : 1;
    uint32  fcseldot1qipri: 1;	
    uint32  fcseldot1qopri: 1;	
    uint32  rrcpType: 3;		
    uint32  rrcpAuth : 1;		
    uint32  ivid: 12;			
    uint32  ovid: 12;			
    uint32  drop:1;			


    uint32  cpu: 1;			
    uint32  cputag: 1;		
    uint32  toGVlan: 1;		
    uint32  ipmcPkt: 1;		
    uint32  dot1qodp: 2;		
    uint32  sml: 1;			
    uint32  redir: 1;			
    uint32  lkmiss: 1;		
    uint32  ftidx: 10;					
    uint32  dpn: 5;  			
    uint32  pbdp: 2;			
    uint32  pbpri: 3;			
    uint32  ftIdxValid: 1;		
    uint32  dot1qidp: 2;
    uint32  dpm: 29;			
    uint32  dot1qipri: 3;
    uint32  dscppriexist: 1;	
    uint32  dscpdp: 2;		
    uint32  dscppri: 3;		
    uint32  fbpriexist: 1;		
    uint32  fbdp: 2;			
    uint32  fbpri: 3; 			
    uint32  ipriexist: 1;		
    uint32  idp: 2;			
    uint32  ipri: 3; 			
    uint32  opriexist: 1;		
    uint32  odp: 2;			
    uint32  opri: 3;			
    uint32  dp: 2;			
    uint32  pri: 3;			
    uint32  macdot1x : 1;      
    uint32  fcdfi : 1;		
    uint32  fcdfo : 1;	
    uint32  tmidx: 2;	
    uint32  vidrch : 1;
    uint32  fwd : 4;			
    uint32  mir : 4;			
    uint32  mirorg : 4;		
    uint32  spm : 16;		
    uint32  msti : 7;			
    uint32  fid: 12;	
    uint32 miriutag : 4;			
    uint32 miroutag : 4;	 
    uint32  flood: 1;			
    uint32  ppbdfi: 1;		
    uint32  ppbdfo: 1;		
    uint32  tm: 1;			
    uint32  ipmc:1;			
    uint32  copyToCpu: 1;	
    uint32  vidrc : 5;			
    uint32  reason: 16;		
    uint32  dscprmk: 1;		
    uint32  dot1qopri:3;		
    uint32  ce : 1;	
    uint32  iutagstatus: 29;		
    uint32  iuntagValid : 1;	
    uint32  ountagValid : 1;	
    uint32  outagstatus: 29;

    uint32 dscp : 6;				
    uint32 orgvid : 12;					
    uint32  dmacidx: 14;	

    uint32 rrcpRegData;			

    uint32 prc:16;					
    uint8  iprc[8];

    uint8 bpdu:1;  /*for sw use*/
    uint32 egressvid : 12;

    uint32  aclidx0 : 8;			
    uint32  aclidx1 : 8;			 


    uint32  aclidx2 : 8;			
    uint32  aclidx3 : 8;			
    uint32  aclidx4 : 8;			 
    uint32  aclidx5 : 8;			 


    uint32  aclidx6 : 8;			
    uint32  aclidx7 : 8;			
    uint32  aclidx8 : 8;			
    uint32  aclidx9 : 8;			


    uint32  aclidx10 : 8;			
    uint32  aclidx11 : 8;			
    uint32  aclidx12 : 8;			
    uint32  aclidx13 : 8;			


    uint32  aclidx14 : 8;			
    uint32  aclidx15 : 8;			

};
typedef struct ppi_param_s ppi_param_t;


struct pmi_param_s
{
    uint32 dpm: 29;			/*destination port mask*/
    uint32 l2err : 1;			/*Recalculate the layer2 CRC*/
    uint32 l3err:1; 			/*Recalculate the IP checksum*/
    uint32 pppoe : 1;		/*Whether PPPoE header exists*/

    uint32 fwd: 4;			
    uint32 mir:4; 			
    uint32 mirorg:4; 		
    uint32 pri:3; 			/*Priority*/
    uint32 rxrspan : 1;		/*rspan tag exist*/
    uint32 miriutag : 4;		
    uint32 miroutag : 4;
    uint32 stphy : 5;
    uint32 rrcpType : 2;		/*rrcp packet type*/
    uint32 rxcpuTag : 1;		/*cpu tag exist when packet is rx from cpu port*/  

    uint32 dpcnt : 5;			
    uint32 pnxt : 11;  		
    uint32 reason : 16;  		/*cpu reason*/

    uint32 framelength:14;	/*frame length,include CRC*/
    uint32 dp : 2;			/*Drop precedence*/
    uint32 lb : 1;			/*oam loopback*/
    uint32 occupy : 1;		/*occupied or not*/
    uint32 sphy : 5;			/*Source port number*/
    uint32 extraTag : 1;		/*extra tag exist*/
    uint32 asdp : 1;			/*Packet priority and drop precedence is assigned*/
    uint32 asdprmk : 1;		/*Packet priority and drop precedence is assigned and need remarking*/
    uint32 evten : 1;
    uint32 ttldec : 1;		      /*TTL decrease for routing */
    uint32 txcputag : 1;		 /*Insert CPU tag when packet is sent to CPU port*/
    uint32 dopri : 3;			/*destination outer tag priority*/		

    uint32 divid : 12;			/*Destination Inner VID*/
    uint32 l2fmt : 4;			/*Layer 2 format*/
    uint32 dovid : 12;  		/*Destination Outer VID*/
    uint32 dipri : 3;			
    uint32 l4err : 1;			 /*Recalculate the TCP, UDP, ICMP and IGMP checksum*/

    uint32 iutagStatus : 29;		
    uint32 dscprmk : 1;		/*Weather DSCP remarking or not*/
    uint32 oam_lbsa : 2;		/*oam loopback smac address operation*/

    uint32 ipuc : 1;				/*IPv4 unicast routing*/
    uint32 ipmc : 1;		  		/*ip multicast routing*/
    uint32 l34fmt : 4;			/*Layer 3/4 format*/
    uint32 dmacidx:14;			/*When perform IPv4 unicast routing, this field point to the DMAC position stored in address hash table*/
    uint32 dscp : 6;				/*DSCP*/  
    uint32 ce : 1;				/*Whether set CE bit in ip header to */
    uint32 rrcpRegData31_27: 5;	/*rrcp register data bit31-bit26*/

    uint32 outagStatus:29;				
    uint32 rrcp:1;				/*rrcp packet*/
    uint32 oam_lbda : 2;			/*oam loopback dmac address operation*/

    uint32 keeporigivid : 1;
    uint32 keeporigovid : 1;

    uint32 diprivalid:1;
    uint32 dcfivalid:1;
    uint32 dcfi:1;
    uint32 doprivalid:1;
    uint32 ddeivalid:1;
    uint32 ddei:1;
    uint32 spididxvalid:1;
    uint32 spididx:4;
    uint32 reserved:19;

    /*used for model code, not real ASIC field*/
    uint32 drop : 1; /*1:dropped by last PIE lookup phase*/
    uint32 curPortNum:5;  /*current port number*/
    uint32 egvidiuvalid:1;
    uint32 egvidouvalid:1;
    uint32 egvidkeepi:1;
    uint32 egvidkeepo:1;
};
typedef struct pmi_param_s pmi_param_t;

struct hsa_param_s
{
    uint32 hsa_busy : 1;
    uint32 reserved : 5;
    uint32 newsvid :12;
    uint32 newvid : 12;
    uint32 cputagif : 1;
    uint32 dpcnt_4 : 1;

    uint32 dpcnt_3_0 : 4;
    uint32 rvid : 12; 			
    uint32 reason : 16; 		

    uint32 intpri : 3;			
    uint32 dpmask : 29;  		

    uint32 mir1dpa : 5;
    uint32 mir0dpa : 5;
    uint32 cpuasdprmk : 1;
    uint32 ipv6 : 1;
    uint32 ipv4 : 1;
    uint32 pppoe : 1;
    uint32 stagif : 1;
    uint32 ctagif : 1;
    uint32 frametype : 2;
    uint32 pktlen : 14;

    uint32 l4csok : 1;
    uint32 l3csok : 1;
    uint32 endpage : 11;
    uint32 startpage : 11;
    uint32 startbank : 3;
    uint32 spa : 5;
};
typedef struct hsa_param_s hsa_param_t;

/* Function Name:
 *      dumpHsb
 * Description:
 *      Dump hsb paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 dumpHsb(uint32 unit);

/* Function Name:
 *      dumpPpi
 * Description:
 *      Dump ppi paramter of the specified device.
 * Input:
 *      unit - unit id
 *      index -ppi index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 dumpPpi(uint32 unit, uint32 index);

/* Function Name:
 *      dumpPmi
 * Description:
 *      Dump pmi paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 dumpPmi(uint32 unit);

#if defined(CONFIG_SDK_RTL8389)
/* Function Name:
 *      dumpHsa
 * Description:
 *      Dump hsa paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
extern int32 dumpHsa(uint32 unit);
#endif

#endif  /* __HAL_MAC_DEBUG_H__ */
