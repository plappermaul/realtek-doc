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
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1ad, SVLAN [VLAN Stacking] 
 *
 */


#ifndef __RTK_SVLAN_H__
#define __RTK_SVLAN_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/vlan.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */
typedef struct rtk_svlan_data_s
{
    uint32          idx;
    rtk_vlan_t      svid;
    rtk_portmask_t  member_portmask;
} rtk_svlan_data_t;

typedef struct rtk_svlan_memberCfg_s{
    uint32 svid;
    rtk_portmask_t memberport;
    rtk_portmask_t untagport;
    rtk_enable_t fiden;
    uint32 fid;
    uint32 priority;
    rtk_enable_t efiden;
    uint32 efid;
}rtk_svlan_memberCfg_t;

typedef enum rtk_svlan_pri_ref_e
{
    REF_INTERNAL_PRI = 0,
    REF_CTAG_PRI,
    REF_SVLAN_PRI,
    REF_PB,
    REF_OUTER_TAG,
    REF_PRI_END
} rtk_svlan_pri_ref_t;


typedef enum rtk_svlan_untag_action_e
{
    UNTAG_DROP = 0,
    UNTAG_TRAP,
    UNTAG_ASSIGN,
    UNTAG_END
} rtk_svlan_untag_action_t;

typedef enum rtk_svlan_unmatch_action_e
{
    UNMATCH_DROP = 0,
    UNMATCH_TRAP,
    UNMATCH_ASSIGN,
    UNMATCH_END
} rtk_svlan_unmatch_action_t;


typedef enum rtk_svlan_lookupType_e
{
    SVLAN_LOOKUP_S64MBRCGF  = 0,
    SVLAN_LOOKUP_C4KVLAN,
    SVLAN_LOOKUP_END,

} rtk_svlan_lookupType_t;

typedef enum rtk_svlan_priSel_e
{
    SVLAN_PRISEL_INTERNAL_PRI  = 0,
    SVLAN_PRISEL_1QTAG_PRI,
    SVLAN_PRISEL_VSPRI,
    SVLAN_PRISEL_PBPRI,
    SVLAN_PRISEL_OUTERTAG,
    SVLAN_PRISEL_END,    

} rtk_svlan_priSel_t;

typedef enum rtk_svlan_action_e
{
    SVLAN_ACTION_DROP  = 0,
    SVLAN_ACTION_TRAP,
    SVLAN_ACTION_SVLAN,
    SVLAN_ACTION_PSVID,    
    SVLAN_ACTION_END,    

} rtk_svlan_action_t;

typedef enum rtk_svlan_mc2sFmt_e
{
    SVLAN_MC2SFMT_DMAC  = 0,
    SVLAN_MC2SFMT_IPV4,
    SVLAN_MC2SFMT_END,

} rtk_svlan_mc2sFmt_t;

/*
 * Function Declaration
 */

/* Module Name : SVLAN */

/* Function Name:
 *      rtk_svlan_init
 * Description:
 *      Initialize svlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize svlan module before calling any svlan APIs.
 */
extern int32
rtk_svlan_init(void);

/* Function Name:
 *      rtk_svlan_create
 * Description:
 *      Create the svlan.
 * Input:
 *      svid - svlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_EXIST - SVLAN entry is exist
 * Note:
 *      None
 */
extern int32
rtk_svlan_create(rtk_vlan_t svid);


/* Function Name:
 *      rtk_svlan_destroy
 * Description:
 *      Destroy the svlan.
 * Input:
 *      svid - svlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      None
 */
extern int32
rtk_svlan_destroy(rtk_vlan_t svid);

/* Function Name:
 *      rtk_svlan_portSvid_get
 * Description:
 *      Get port default svlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pSvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_portSvid_get(rtk_port_t port, rtk_vlan_t *pSvid);


/* Function Name:
 *      rtk_svlan_portSvid_set
 * Description:
 *      Set port default svlan id.
 * Input:
 *      port - port id
 *      svid - port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
rtk_svlan_portSvid_set(rtk_port_t port, rtk_vlan_t svid);


/* Function Name:
 *      rtk_svlan_servicePort_get
 * Description:
 *      Get service ports from the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - status of service port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_servicePort_get(rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_svlan_servicePort_set
 * Description:
 *      Set service ports to the specified device.
 * Input:
 *      port       - port id
 *      enable     - status of service port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_svlan_servicePort_set(rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      rtk_svlan_memberPort_set
 * Description:
 *      Replace the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
extern int32
rtk_svlan_memberPort_set(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask);



/* Function Name:
 *      rtk_svlan_memberPort_get
 * Description:
 *      Get the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
extern int32
rtk_svlan_memberPort_get(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask);


/* Function Name:
 *      rtk_svlan_tpidEntry_get
 * Description:
 *      Get the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 * Output:
 *      pSvlanTagId - pointer buffer of svlan TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only support pSvlanTagId 0 in Apollo.
 */
extern int32
rtk_svlan_tpidEntry_get(uint32 svlanIndex, uint32 *pSvlanTagId);


/* Function Name:
 *      rtk_svlan_tpidEntry_set
 * Description:
 *      Set the svlan TPID.
 * Input:
 *      svlanIndex   - index of tpid entry
 *      svlan_tag_id - svlan TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only support pSvlan_tag_id 0 in Apollo.
 */
extern int32
rtk_svlan_tpidEntry_set(uint32 svlanIndex, uint32 svlan_tag_id);


/* Function Name:
 *      rtk_svlan_priorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 * Input:
 *      ref - reference selection parameter.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
extern int32 
rtk_svlan_priorityRef_set(rtk_svlan_pri_ref_t ref);

/* Function Name:
 *      rtk_svlan_priorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 * Input:
 *      None
 * Output:
 *      pRef - reference selection parameter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
extern int32 
rtk_svlan_priorityRef_get(rtk_svlan_pri_ref_t *pRef);

/* Function Name:
 *      rtk_svlan_memberPortEntry_set
 * Description:
 *      Configure system SVLAN member content
 * Input:
 *      psvlan_cfg - SVLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameter.
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_SVLAN_TABLE_FULL - SVLAN configuration is full.
 * Note:
 *      The API can set system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped by default setup.
 *      - rtk_svlan_memberCfg_t->svid is SVID of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->memberport is member port mask of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->fid is filtering database of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->priority is priority of SVLAN member configuration.
 */
extern int32 
rtk_svlan_memberPortEntry_set(rtk_svlan_memberCfg_t *pSvlan_cfg);


/* Function Name:
 *      rtk_svlan_memberPortEntry_get
 * Description:
 *      Get SVLAN member Configure.
 * Input:
 *      pSvlan_cfg - SVLAN member configuration
 * Output:
 *      pSvlan_cfg - SVLAN member configuration
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can get system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped.
 */
extern int32 
rtk_svlan_memberPortEntry_get(rtk_svlan_memberCfg_t *pSvlan_cfg);


/* Function Name:
 *      rtk_svlan_ipmc2s_add
 * Description:
 *      add ip multicast address to SVLAN
 * Input:
 *      ipmc    - ip multicast address
 *      ipmcMsk - ip multicast address mask
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can set IP mutlicast to SVID configuration. If upstream packet is IPv4 multicast
 *      packet and DIP is matched MC2S configuration, ASIC will assign egress SVID to the packet.
 *      There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
extern int32 
rtk_svlan_ipmc2s_add(ipaddr_t ipmc, ipaddr_t ipmcMsk, rtk_vlan_t svid);

/* Function Name:
 *      rtk_svlan_ipmc2s_del
 * Description:
 *      delete ip multicast address to SVLAN
 * Input:
 *      ipmc - ip multicast address
 *      ipmcMsk - ip multicast address mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete IP mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
extern int32 
rtk_svlan_ipmc2s_del(ipaddr_t ipmc, ipaddr_t ipmcMsk);


/* Function Name:
 *      rtk_svlan_ipmc2s_get
 * Description:
 *      Get ip multicast address to SVLAN
 * Input:
 *      ipmc - ip multicast address
 *      ipmcMsk - ip multicast address mask
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can get IP mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
extern int32 
rtk_svlan_ipmc2s_get(ipaddr_t ipmc, ipaddr_t ipmcMsk, rtk_vlan_t *pSvid);


/* Function Name:
 *      rtk_svlan_l2mc2s_add
 * Description:
 *      Add L2 multicast address to SVLAN
 * Input:
 *      mac - L2 multicast address
 *      macMsk  - L2 multicast address
 *      svid - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can set L2 Mutlicast to SVID configuration. If upstream packet is L2 multicast
 *      packet and DMAC is matched, ASIC will assign egress SVID to the packet. There are 32
 *      SVLAN multicast configurations for IP and L2 multicast.
 */
extern int32 
rtk_svlan_l2mc2s_add(rtk_mac_t mac, rtk_mac_t macMsk, rtk_vlan_t svid);


/* Function Name:
 *      rtk_svlan_l2mc2s_del
 * Description:
 *      delete L2 multicast address to SVLAN
 * Input:
 *      mac     - L2 multicast address
 *      macMsk  - L2 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can delete Mutlicast to SVID configuration. There are 8 SVLAN multicast configurations for IP and L2 multicast.
 */
extern int32 
rtk_svlan_l2mc2s_del(rtk_mac_t mac, rtk_mac_t macMsk);


/* Function Name:
 *      rtk_svlan_l2mc2s_get
 * Description:
 *      Get L2 multicast address to SVLAN
 * Input:
 *      mac - L2 multicast address
 *      macMsk  - L2 multicast address
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can get L2 mutlicast to SVID configuration. There are 32 SVLAN multicast configurations for IP and L2 multicast.
 */
extern int32 
rtk_svlan_l2mc2s_get(rtk_mac_t mac, rtk_mac_t macMsk, rtk_vlan_t *pSvid);


/* Function Name:
 *      rtk_svlan_sp2c_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      cvid        - VLAN ID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
extern int32  
rtk_svlan_sp2c_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t cvid);


/* Function Name:
 *      rtk_svlan_sp2c_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid 	    - SVLAN VID
 *      dst_port 	- Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pCvid - VLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
extern int32  
rtk_svlan_sp2c_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_vlan_t *pCvid);

/* Function Name:
 *      rtk_svlan_sp2c_del
 * Description:
 *      Delete system SP2C configuration
 * Input:
 *      svid        - SVLAN VID
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 * Note:
 *      The API can delete SVID & Destination Port to CVLAN configuration. There are 128 SP2C configurations.
 */
extern int32 
rtk_svlan_sp2c_del(rtk_vlan_t svid, rtk_port_t dstPort);

/* Function Name:
 *      rtk_svlan_sp2cPriority_add
 * Description:
 *      Add system SP2C configuration
 * Input:
 *      dst_port    - Destination port of SVLAN to CVLAN configuration
 *      svid        - SVLAN VID
 *      priority    - Priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can add SVID & Destination Port to CVLAN configuration. The downstream frames with assigned
 *      SVID will be add C-tag with assigned CVID if the output port is the assigned destination port.
 *      There are 128 SP2C configurations.
 */
extern int32  
rtk_svlan_sp2cPriority_add(rtk_vlan_t svid, rtk_port_t dstPort, rtk_pri_t priority);

/* Function Name:
 *      rtk_svlan_sp2cPriority_get
 * Description:
 *      Get configure system SP2C content
 * Input:
 *      svid 	    - SVLAN VID
 *      dst_port 	- Destination port of SVLAN to CVLAN configuration
 * Output:
 *      pPriority 	- Priority
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE - input out of range.
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_SVLAN_VID    - Invalid SVLAN VID parameter.
 * Note:
 *     The API can get SVID & Destination Port to CVLAN configuration. 
 */
extern int32  
rtk_svlan_sp2cPriority_get(rtk_vlan_t svid, rtk_port_t dstPort, rtk_pri_t *pPriority);



/* Function Name:
 *      rtk_svlan_dmacVidSelState_set
 * Description:
 *      Set DMAC CVID selection status
 * Input:
 *      port    - Port
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_PORT_ID      			- Invalid port id.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection state
 */
extern int32 
rtk_svlan_dmacVidSelState_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_dmacVidSelState_get
 * Description:
 *      Get DMAC CVID selection status
 * Input:
 *      port    - Port
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_PORT_ID      			- Invalid port id.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can get DMAC CVID Selection state
 */
extern int32 
rtk_svlan_dmacVidSelState_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_svlan_unmatchAction_set
 * Description:
 *      Configure Action of downstream Unmatch packet
 * Input:
 *      action  - Action for Unmatch
 *      svid    - The SVID assigned to Unmatch packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also supported by this API. The parameter add svid is
 *      only refernced when the action is set to UNMATCH_ASSIGN
 */
extern int32 
rtk_svlan_unmatchAction_set(rtk_svlan_action_t action, rtk_vlan_t svid);


/* Function Name:
 *      rtk_svlan_unmatchAction_get
 * Description:
 *      Get Action of downstream Unmatch packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for Unmatch
 *      pSvid    - The SVID assigned to Unmatch packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-match packet. A SVID assigned
 *      to the un-match is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNMATCH_ASSIGN
 */
extern int32 
rtk_svlan_unmatchAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid);


/* Function Name:
 *      rtk_svlan_untagAction_set
 * Description:
 *      Configure Action of downstream UnStag packet
 * Input:
 *      action  - Action for UnStag
 *      svid    - The SVID assigned to UnStag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also supported by this API. The parameter of svid is
 *      only referenced when the action is set to UNTAG_ASSIGN
 */
extern int32 
rtk_svlan_untagAction_set(rtk_svlan_action_t action, rtk_vlan_t svid);


/* Function Name:
 *      rtk_svlan_untagAction_get
 * Description:
 *      Get Action of downstream UnStag packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for UnStag
 *      pSvid    - The SVID assigned to UnStag packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNTAG_ASSIGN
 */
extern int32 
rtk_svlan_untagAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid);

/* Function Name:
 *      rtk_svlan_c2s_add
 * Description:
 *      add CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can set upstream packet CVID and ingress port to SVID configuration.
 *      There are 128 SVLAN configurations for CVID and ingress port.
 *      If CVID and SVID of configured entry are matched with configuration parameter, then 
 *      different ingress port will share the same configuration entry. 
 */
extern int32 
rtk_svlan_c2s_add(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t svid);

/* Function Name:
 *      rtk_svlan_c2s_del
 * Description:
 *      delete CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 *      svid    - SVLAN VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can delet upstream packet CVID and ingress port to SVID configuration. 
 */
extern int32 
rtk_svlan_c2s_del(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t svid);


/* Function Name:
 *      rtk_svlan_c2s_get
 * Description:
 *      Get CVID and ingress Port to SVLAN
 * Input:
 *      cvid    - CVLAN VID
 *      port    - port id
 * Output:
 *      pSvid - SVLAN VID
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_INPUT                    - Invalid input parameters.
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 * Note:
 *      The API can delet upstream packet CVID and ingress port to SVID configuration. 
 */
extern int32 
rtk_svlan_c2s_get(rtk_vlan_t cvid, rtk_port_t port, rtk_vlan_t *pSvid);

/* Function Name:
 *      rtk_svlan_trapPri_get
 * Description:
 *      Get svlan trap priority
 * Input:
 *      None
 * Output:
 *      pPriority - priority for trap packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_trapPri_get(rtk_pri_t *pPriority);


/* Function Name:
 *      rtk_svlan_trapPri_set
 * Description:
 *      Set svlan trap priority
 * Input:
 *      priority - priority for trap packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY 
 * Note:
 *      None
 */
extern int32
rtk_svlan_trapPri_set(rtk_pri_t priority);

/* Function Name:
 *      rtk_svlan_deiKeepState_get
 * Description:
 *      Get svlan dei keep state
 * Input:
 *      None
 * Output:
 *      pEnable - state of keep dei 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_deiKeepState_get(rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_svlan_deiKeepState_set
 * Description:
 *      Set svlan dei keep state
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
rtk_svlan_deiKeepState_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_lookupType_get
 * Description:
 *      Get lookup type of SVLAN
 * Input:
 *      None
 * Output:
 *      pType 		- lookup type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
extern int32 
rtk_svlan_lookupType_get(rtk_svlan_lookupType_t *pType);

/* Function Name:
 *      rtk_svlan_lookupType_set
 * Description:
 *      Set lookup type of SVLAN
 * Input:
 *      type 		- lookup type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      Must call this API after rtl_svlan_init. Otherwise, there will be some unexpected switch behaviors.
 *      Set lookup type to SVLAN_LOOKUP_C4KVLAN and must create vlan by using API rtk_vlan_create. In the
 *		SVLAN_LOOKUP_C4KVLAN config, rtk_svlan_create and rtk_svlan_destroy will return failed.
 */
extern int32 
rtk_svlan_lookupType_set(rtk_svlan_lookupType_t type);

/* Function Name:
 *      rtk_svlan_sp2cUnmatchCtagging_get
 * Description:
 *      Get unmatch sp2c egress ctagging state
 * Input:
 *      None
 * Output:
 *      pState     - unmatch cvlan tagging state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 
rtk_svlan_sp2cUnmatchCtagging_get(rtk_enable_t *pState);

/* Function Name:
 *      rtk_svlan_sp2cUnmatchCtagging_set
 * Description:
 *      Set unmatch sp2c egress ctagging state
 * Input:
 *      state      - unmatch cvlan tagging state
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32 
rtk_svlan_sp2cUnmatchCtagging_set(rtk_enable_t state);

/* Function Name:
 *      rtk_svlan_priority_get
 * Description:
 *      Get SVLAN priority for each SVID.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pPriority - priority assigned for the SVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *     None
 */
extern int32
rtk_svlan_priority_get(rtk_vlan_t svid, rtk_pri_t *pPriority);

/* Function Name:
 *      rtk_svlan_priority_set
 * Description:
 *      Set SVLAN priority for each SVID.
 * Input:
 *      svid  - svlan id
 *      priority - priority assigned for the SVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      This API is used to set priority per SVLAN.
 */
extern int32
rtk_svlan_priority_set(rtk_vlan_t svid, rtk_pri_t priority);


/* Function Name:
 *      rtk_svlan_fid_get
 * Description:
 *      Get the filter id of the vlan.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pFid - pointer buffer of filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_fid_get(rtk_vlan_t svid, rtk_fid_t *pFid);

/* Function Name:
 *      rtk_svlan_fid_set
 * Description:
 *      Set the filter id of the svlan.
 * Input:
 *      svid  - svlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The FID is effective only in VLAN SVL mode. 
 */
extern int32
rtk_svlan_fid_set(rtk_vlan_t svid, rtk_fid_t fid);

/* Function Name:
 *      rtk_svlan_fidEnable_get
 * Description:
 *      Get svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEnable - pointer to svlan based fid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_fidEnable_get(rtk_vlan_t svid, rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_svlan_fidEnable_set
 * Description:
 *      Set svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 *      enable - svlan based fid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
extern int32
rtk_svlan_fidEnable_set(rtk_vlan_t svid, rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_enhancedFid_get
 * Description:
 *      Get the enhanced filter id of the vlan.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEfid - pointer buffer of enhanced filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_enhancedFid_get(rtk_vlan_t svid, rtk_efid_t *pEfid);

/* Function Name:
 *      rtk_svlan_enhancedFid_set
 * Description:
 *      Set the enhanced filter id of the svlan.
 * Input:
 *      svid  - svlan id
 *      efid  - enhanced filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      The EFID is effective only in VLAN SVL mode. 
 */
extern int32
rtk_svlan_enhancedFid_set(rtk_vlan_t svid, rtk_efid_t efid);

/* Function Name:
 *      rtk_svlan_enhancedFidEnable_get
 * Description:
 *      Get svlan based fid assignment status.
 * Input:
 *      svid  - svlan id
 * Output:
 *      pEnable - pointer to svlan based efid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_svlan_enhancedFidEnable_get(rtk_vlan_t svid, rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_svlan_enhancedFidEnable_set
 * Description:
 *      Set svlan based efid assignment status.
 * Input:
 *      svid  - svlan id
 *      enable - svlan based efid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SVLAN_NOT_EXIST
 * Note:
 *      None
 */
extern int32
rtk_svlan_enhancedFidEnable_set(rtk_vlan_t svid, rtk_enable_t enable);


/* Function Name:
 *      rtk_svlan_dmacVidSelForcedState_set
 * Description:
 *      Set DMAC CVID selection status
 * Input:
 *      port    - Port
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      This API can set DMAC CVID Selection forced state
 */
extern int32 
rtk_svlan_dmacVidSelForcedState_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_dmacVidSelForcedState_get
 * Description:
 *      Get DMAC CVID selection forced status
 * Input:
 *      None
 * Output:
 *      pEnable - state of DMAC CVID Selection
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      This API can get DMAC CVID Selection forced state
 */
extern int32 
rtk_svlan_dmacVidSelForcedState_get(rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_svlan_svlanFunctionEnable_get
 * Description:
 *      Get the SVLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of svlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
rtk_svlan_svlanFunctionEnable_get(rtk_enable_t *pEnable);


/* Function Name:
 *      rtk_svlan_svlanFunctionEnable_set
 * Description:
 *      Set the SVLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of svlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
rtk_svlan_svlanFunctionEnable_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_tpidEnable_get
 * Description:
 *      Get the svlan TPID enable status.
 * Input:
 *      svlanIndex  - index of tpid entry
 * Output:
 *      pEnable - pointer to svlan tpid assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
rtk_svlan_tpidEnable_get(uint32 svlanIndex, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_svlan_tpidEnable_set
 * Description:
 *      Set the svlan TPID enable status.
 * Input:
 *      svlanIndex  - index of tpid entry
 *      enable - svlan tpid assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 */
extern int32
rtk_svlan_tpidEnable_set(uint32 svlanIndex, rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_extPortPvid_get
 * Description:
 *      Get extension port default svlan id.
 * Input:
 *      extPort - Extension port id
 * Output:
 *      pPvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Configuration on EXT port 0 will be get from physical port 6.
 */
extern int32
rtk_svlan_extPortPvid_get(uint32 extPort, uint32 *pPvid);


/* Function Name:
 *      rtk_svlan_extPortPvid_set
 * Description:
 *      Set extension port default svlan id.
 * Input:
 *      extPort - extension port id
 *      pvid    - extension port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *	  None    
 */
extern int32
rtk_svlan_extPortPvid_set(uint32 extPort, uint32 pvid);

#endif /* __RTK_SVLAN_H__ */
