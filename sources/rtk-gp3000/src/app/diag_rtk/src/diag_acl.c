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
#include <common/util/rt_util.h>
#include <diag_str.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>

#include <osal/memory.h>


#include <rtk/acl.h>
#include <rtk/stat.h>

#ifdef CONFIG_SDK_APOLLO
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <dal/apollo/dal_apollo_svlan.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#endif

#ifdef CONFIG_SDK_RTL9601B
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#endif

#ifdef CONFIG_SDK_RTL9602C
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#endif


#define DIAG_SVLAN_VID_NOT_ACCEPT(vid) \
    ((RTK_VLAN_ID_MAX < vid)? 1: 0)


typedef enum diag_acl_ruleMode_e
{
    DIAG_ACL_RULE_DISPLAY_MODE_GET = 0,
    DIAG_ACL_RULE_DISPLAY_MODE_SET,
    DIAG_ACL_RULE_DISPLAY_MODE_END
}diag_acl_ruleMode_t;

rtk_acl_template_t diag_aclTemplate;
rtk_acl_ingress_entry_t diag_aclRuleEntry;
rtk_enable_t diag_aclTemplateEn[RTK_MAX_NUM_OF_ACL_RULE_FIELD];

int32 _diag_aclTemplateAdd(rtk_acl_field_type_t type)
{
    uint32 fieldIdx;

    for(fieldIdx = 0; fieldIdx < HAL_MAX_NUM_OF_ACL_RULE_FIELD(); fieldIdx ++ )
    {
        if(diag_aclTemplateEn[fieldIdx] == DISABLED)
        {
            diag_aclTemplate.fieldType[fieldIdx] = type;
			diag_aclTemplateEn[fieldIdx] = ENABLED;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

int8 * _diag_aclGetTypeName(rtk_acl_field_type_t type)
{
    static int8 name[20];
	switch(type)
	{
		case ACL_FIELD_DMAC0: osal_strcpy(name,"DMAC0");break;
		case ACL_FIELD_DMAC1: osal_strcpy(name,"DMAC1");break;
		case ACL_FIELD_DMAC2: osal_strcpy(name,"DMAC2");break;
		case ACL_FIELD_SMAC0: osal_strcpy(name,"SMAC0");break;
		case ACL_FIELD_SMAC1: osal_strcpy(name,"SMAC1");break;
		case ACL_FIELD_SMAC2: osal_strcpy(name,"SMAC2");break;
		case ACL_FIELD_ETHERTYPE: osal_strcpy(name,"ETHERTYPE");break;
		case ACL_FIELD_STAG: osal_strcpy(name,"STAG");break;
		case ACL_FIELD_CTAG: osal_strcpy(name,"CTAG");break;
		case ACL_FIELD_GEMPORT: osal_strcpy(name,"GEM/LLID");break;
		case ACL_FIELD_FRAME_TYPE_TAGS: osal_strcpy(name,"FRAME_TAG");break;

		case ACL_FIELD_IPV4_SIP0: osal_strcpy(name,"IP4SIP0");break;
		case ACL_FIELD_IPV4_SIP1: osal_strcpy(name,"IP4SIP1");break;
		case ACL_FIELD_IPV4_DIP0: osal_strcpy(name,"IP4DIP0");break;
		case ACL_FIELD_IPV4_DIP1: osal_strcpy(name,"IP4DIP1");break;
		case ACL_FIELD_IPV4_SIP0_INNER: osal_strcpy(name,"IP4SIP0_INNER");break;
		case ACL_FIELD_IPV4_SIP1_INNER: osal_strcpy(name,"IP4SIP1_INNER");break;
		case ACL_FIELD_IPV4_DIP0_INNER: osal_strcpy(name,"IP4DIP0_INNER");break;
		case ACL_FIELD_IPV4_DIP1_INNER: osal_strcpy(name,"IP4DIP1_INNER");break;

		case ACL_FIELD_IPV6_SIP0: osal_strcpy(name,"IP6SIP0");break;
		case ACL_FIELD_IPV6_SIP1: osal_strcpy(name,"IP6SIP1");break;
		case ACL_FIELD_IPV6_SIP2: osal_strcpy(name,"IP6SIP2");break;
		case ACL_FIELD_IPV6_SIP3: osal_strcpy(name,"IP6SIP3");break;
		case ACL_FIELD_IPV6_SIP4: osal_strcpy(name,"IP6SIP4");break;
		case ACL_FIELD_IPV6_SIP5: osal_strcpy(name,"IP6SIP5");break;
		case ACL_FIELD_IPV6_SIP6: osal_strcpy(name,"IP6SIP6");break;
		case ACL_FIELD_IPV6_SIP7: osal_strcpy(name,"IP6SIP7");break;
		case ACL_FIELD_IPV6_DIP0: osal_strcpy(name,"IP6DIP0");break;
		case ACL_FIELD_IPV6_DIP1: osal_strcpy(name,"IP6DIP1");break;
		case ACL_FIELD_IPV6_DIP2: osal_strcpy(name,"IP6DIP2");break;
		case ACL_FIELD_IPV6_DIP3: osal_strcpy(name,"IP6DIP3");break;
		case ACL_FIELD_IPV6_DIP4: osal_strcpy(name,"IP6DIP4");break;
		case ACL_FIELD_IPV6_DIP5: osal_strcpy(name,"IP6DIP5");break;
		case ACL_FIELD_IPV6_DIP6: osal_strcpy(name,"IP6DIP6");break;
		case ACL_FIELD_IPV6_DIP7: osal_strcpy(name,"IP6DIP7");break;
		case ACL_FIELD_IPV6_NEXT_HEADER: osal_strcpy(name,"TC_NEXTHEADER");break;
		case ACL_FIELD_IPV4_PROTOCOL: osal_strcpy(name,"TOS_PROTO");break;
		case ACL_FIELD_IPV4_PROTOCOL_INNER: osal_strcpy(name,"TOS_PROTO_INNER");break;
 		case ACL_FIELD_TCP_DPORT: osal_strcpy(name,"TCP_DPORT");break;
 		case ACL_FIELD_TCP_SPORT: osal_strcpy(name,"TCP_SPORT");break;
 		case ACL_FIELD_UDP_DPORT: osal_strcpy(name,"UDP_DPORT");break;
 		case ACL_FIELD_UDP_SPORT: osal_strcpy(name,"UDP_SPORT");break;
 		case ACL_FIELD_VID_RANGE: osal_strcpy(name,"VIDRANGE");break;
 		case ACL_FIELD_IP_RANGE: osal_strcpy(name,"IPRANGE");break;
 		case ACL_FIELD_PORT_RANGE: osal_strcpy(name,"PORTRANGE");break;
 		case ACL_FIELD_PKT_LEN_RANGE: osal_strcpy(name,"PKTELNRANGE");break;
 		case ACL_FIELD_USER_VALID: osal_strcpy(name,"FIELDVALID");break;
 		case ACL_FIELD_EXT_PORTMASK: osal_strcpy(name,"EXTPMSK");break;
 		case ACL_FIELD_IP_RANGE_0: osal_strcpy(name,"IPRANGE0");break;
 		case ACL_FIELD_IP_RANGE_1: osal_strcpy(name,"IPRANGE1");break;
 		case ACL_FIELD_PORT_RANGE_0: osal_strcpy(name,"PORTRANGE0");break;
 		case ACL_FIELD_PORT_RANGE_1: osal_strcpy(name,"PORTRANGE1");break;
		case ACL_FIELD_USER_DEFINED00: osal_strcpy(name,"Field_Sel0");break;
		case ACL_FIELD_USER_DEFINED01: osal_strcpy(name,"Field_Sel1");break;
		case ACL_FIELD_USER_DEFINED02: osal_strcpy(name,"Field_Sel2");break;
		case ACL_FIELD_USER_DEFINED03: osal_strcpy(name,"Field_Sel3");break;
		case ACL_FIELD_USER_DEFINED04: osal_strcpy(name,"Field_Sel4");break;
		case ACL_FIELD_USER_DEFINED05: osal_strcpy(name,"Field_Sel5");break;
		case ACL_FIELD_USER_DEFINED06: osal_strcpy(name,"Field_Sel6");break;
		case ACL_FIELD_USER_DEFINED07: osal_strcpy(name,"Field_Sel7");break;
		case ACL_FIELD_USER_DEFINED08: osal_strcpy(name,"Field_Sel8");break;
		case ACL_FIELD_USER_DEFINED09: osal_strcpy(name,"Field_Sel9");break;
		case ACL_FIELD_USER_DEFINED10: osal_strcpy(name,"Field_Sel10");break;
		case ACL_FIELD_USER_DEFINED11: osal_strcpy(name,"Field_Sel11");break;
		case ACL_FIELD_USER_DEFINED12: osal_strcpy(name,"Field_Sel12");break;
		case ACL_FIELD_USER_DEFINED13: osal_strcpy(name,"Field_Sel13");break;
		case ACL_FIELD_USER_DEFINED14: osal_strcpy(name,"Field_Sel14");break;
		case ACL_FIELD_USER_DEFINED15: osal_strcpy(name,"Field_Sel15");break;
		default: osal_strcpy(name,"unknown");
	}
    return name;
}

void _diag_aclTemplateShow(rtk_acl_template_t *pAclTmeplate)
{
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Template: %d ",pAclTmeplate->index);
    diag_util_mprintf("[0] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[0]));
    diag_util_mprintf("            [1] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[1]));
    diag_util_mprintf("            [2] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[2]));
    diag_util_mprintf("            [3] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[3]));
    diag_util_mprintf("            [4] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[4]));
    diag_util_mprintf("            [5] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[5]));
    diag_util_mprintf("            [6] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[6]));
    diag_util_mprintf("            [7] %s\n",_diag_aclGetTypeName(pAclTmeplate->fieldType[7]));
}

void _diag_aclActShow(rtk_acl_ingress_entry_t *pAclRuleEntry)
{
    uint8 buf[16];
    diag_util_mprintf("Valid: %s\n", diagStr_valid[pAclRuleEntry->valid]);
    diag_util_mprintf("Not: %s\n", diagStr_enable[pAclRuleEntry->invert]);


    if(pAclRuleEntry->act.enableAct[ACL_IGR_CVLAN_ACT])
    {
        diag_util_printf("Cvlan action: %s ",diagStr_aclActCvlanStr[pAclRuleEntry->act.cvlanAct.act]);
        switch(pAclRuleEntry->act.cvlanAct.act)
        {
            case ACL_IGR_CVLAN_IGR_CVLAN_ACT:
            case ACL_IGR_CVLAN_EGR_CVLAN_ACT:
                diag_util_mprintf("vid: %d\n",pAclRuleEntry->act.cvlanAct.cvid);
                break;
            case ACL_IGR_CVLAN_POLICING_ACT:
			case ACL_IGR_CVLAN_BW_METER_ACT:
                diag_util_mprintf("meter: %d\n",pAclRuleEntry->act.cvlanAct.meter);
                break;
            case ACL_IGR_CVLAN_MIB_ACT:
                diag_util_mprintf("counter: %d\n",pAclRuleEntry->act.cvlanAct.mib);
                break;
            case ACL_IGR_CVLAN_1P_REMARK_ACT:
                diag_util_mprintf("1p-pri: %d\n",pAclRuleEntry->act.cvlanAct.dot1p);
                break;
            default:
                diag_util_mprintf("\n");
                break;
        }
    }
    else
    {
        diag_util_mprintf("Cvlan action: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_CVLAN_ACT]]);
    }


    if(pAclRuleEntry->act.enableAct[ACL_IGR_SVLAN_ACT])
    {
        diag_util_printf("Svlan action: %s ",diagStr_aclActSvlanStr[pAclRuleEntry->act.svlanAct.act]);

        switch(pAclRuleEntry->act.svlanAct.act)
        {
            case ACL_IGR_SVLAN_IGR_SVLAN_ACT:
            case ACL_IGR_SVLAN_EGR_SVLAN_ACT:
                diag_util_mprintf("svid: %d\n",pAclRuleEntry->act.svlanAct.svid);
                break;
            case ACL_IGR_SVLAN_POLICING_ACT:
			case ACL_IGR_SVLAN_BW_METER_ACT:
                diag_util_mprintf("meter: %d\n",pAclRuleEntry->act.svlanAct.meter);
                break;
            case ACL_IGR_SVLAN_MIB_ACT:
                diag_util_mprintf("counter: %d\n",pAclRuleEntry->act.svlanAct.mib);
                break;
            case ACL_IGR_SVLAN_1P_REMARK_ACT:
                diag_util_mprintf("1p-pri: %d\n",pAclRuleEntry->act.svlanAct.dot1p);
                break;
            case ACL_IGR_SVLAN_DSCP_REMARK_ACT:
                diag_util_mprintf("dscp: %d\n",pAclRuleEntry->act.svlanAct.dscp);
                break;
            case ACL_IGR_SVLAN_ROUTE_ACT:
                diag_util_mprintf("hexthop: %d\n",pAclRuleEntry->act.svlanAct.nexthop);
                break;
            default:
                diag_util_mprintf("\n");
                break;
        }
    }
    else
    {
        diag_util_mprintf("Svlan action: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_SVLAN_ACT]]);
    }

    if(pAclRuleEntry->act.enableAct[ACL_IGR_FORWARD_ACT])
    {
        if(pAclRuleEntry->act.forwardAct.act == ACL_IGR_FORWARD_TRAP_ACT ||
			pAclRuleEntry->act.forwardAct.act == ACL_IGR_FORWARD_NOP_ACT ||
			pAclRuleEntry->act.forwardAct.act == ACL_IGR_FORWARD_TRAP2SLAVE_ACT)
        {
            diag_util_mprintf("Forward: %s\n",diagStr_aclActFwdStr[pAclRuleEntry->act.forwardAct.act]);		
        }
        else
        {
            if(pAclRuleEntry->act.forwardAct.portMask.bits[0] == 0 && 
                pAclRuleEntry->act.forwardAct.act == ACL_IGR_FORWARD_REDIRECT_ACT)
            {
                diag_util_mprintf("Forward: Drop\n");
            }
            else
            {
            	switch(DIAG_UTIL_CHIP_TYPE)
				{
#if defined(CONFIG_SDK_RTL9601B)
					case RTL9601B_CHIP_ID:
						if(pAclRuleEntry->act.forwardAct.act == ACL_IGR_FORWARD_COPY_ACT)
						{
							diag_util_mprintf("Forward: %s\n",DIAG_STR_COPY2CPU);
						}	
						else if(pAclRuleEntry->act.forwardAct.act == ACL_IGR_FORWARD_NOP_ACT)
						{
							diag_util_mprintf("Forward: Forward Nop\n");
						}	
						else
						{
							diag_util_mask2str(buf,pAclRuleEntry->act.forwardAct.portMask.bits[0]);
							diag_util_mprintf("Forward: %s Ports: %s\n",
													diagStr_aclActFwdStr[pAclRuleEntry->act.forwardAct.act],
													buf);
						}	
						break;
#endif					
					default:
		                diag_util_mask2str(buf,pAclRuleEntry->act.forwardAct.portMask.bits[0]);
		                diag_util_mprintf("Forward: %s Ports: %s\n",
		                                        diagStr_aclActFwdStr[pAclRuleEntry->act.forwardAct.act],
		                                        buf);
						break;
            	}	
        	}
        }
    }
    else
    {
        diag_util_mprintf("Forward: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_FORWARD_ACT]]);
    }

    if(pAclRuleEntry->act.enableAct[ACL_IGR_LOG_ACT])
    {
        diag_util_printf("Policing: %s ",diagStr_aclActPoliceStr[pAclRuleEntry->act.logAct.act]);
        switch(pAclRuleEntry->act.logAct.act)
        {
            case ACL_IGR_LOG_POLICING_ACT:
			case ACL_IGR_LOG_BW_METER_ACT:
                diag_util_mprintf("meter: %d\n",pAclRuleEntry->act.logAct.meter);
                break;
            case ACL_IGR_LOG_MIB_ACT:
                diag_util_mprintf("counter: %d\n",pAclRuleEntry->act.logAct.mib);
                break;
			case ACL_IGR_LOG_1P_REMARK_ACT:
				diag_util_mprintf("1p-pri: %d\n",pAclRuleEntry->act.logAct.dot1p);
				break;
            default:
                diag_util_mprintf("\n");
                break;
        }

    }
    else
    {
        diag_util_mprintf("Policing: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_LOG_ACT]]);
    }

    if(pAclRuleEntry->act.enableAct[ACL_IGR_PRI_ACT])
    {
        diag_util_printf("Pri-Remark: %s ",diagStr_aclActPriStr[pAclRuleEntry->act.priAct.act]);

        switch(pAclRuleEntry->act.priAct.act)
        {
            case ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT:
                diag_util_mprintf("acl-pri: %d\n",pAclRuleEntry->act.priAct.aclPri);
                break;
            case ACL_IGR_PRI_1P_REMARK_ACT:
                diag_util_mprintf("1p-pri: %d\n",pAclRuleEntry->act.priAct.dot1p);
                break;
            case ACL_IGR_PRI_DSCP_REMARK_ACT:
                diag_util_mprintf("dscp: %d\n",pAclRuleEntry->act.priAct.dscp);
                break;
			case ACL_IGR_PRI_TOS_REMARK_ACT:
				diag_util_mprintf("tos: 0x%2.2x [mask: 0x%2.2x]\n",
									pAclRuleEntry->act.priAct.tos,
									pAclRuleEntry->act.priAct.tosMask);
				break;
            case ACL_IGR_PRI_POLICING_ACT:
			case ACL_IGR_PRI_BW_METER_ACT:
                diag_util_mprintf("meter: %d\n",pAclRuleEntry->act.priAct.meter);
                break;
            case ACL_IGR_PRI_MIB_ACT:
                diag_util_mprintf("counter: %d\n",pAclRuleEntry->act.priAct.mib);
                break;
            case ACL_IGR_PRI_ROUTE_ACT:
                diag_util_mprintf("nexthop: %d\n",pAclRuleEntry->act.priAct.nexthop);
                break;
            default:
                diag_util_mprintf("\n");
                break;
        }
    }
    else
    {
        diag_util_mprintf("Pri-Remark: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_PRI_ACT]]);
    }

    if(pAclRuleEntry->act.enableAct[ACL_IGR_INTR_ACT])
    {
#if defined(CONFIG_SDK_RTL9603CVD)
        diag_util_mprintf("Interrupt: %s\n",diagStr_enable[pAclRuleEntry->act.aclInterrupt]);
        if(pAclRuleEntry->act.extendAct.act == ACL_IGR_EXTEND_SID_ACT)
        {
            diag_util_mprintf("SID assign: %d\n",pAclRuleEntry->act.extendAct.index);
        }        
#else
        diag_util_mprintf("Interrupt: %s\n",diagStr_enable[pAclRuleEntry->act.aclInterrupt]);

        diag_util_mprintf("ACL index latch: %s\n",diagStr_enable[pAclRuleEntry->act.aclLatch]);

    #if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9602C)
		if(pAclRuleEntry->act.extendAct.act == ACL_IGR_EXTEND_NONE_ACT)
        {
            diag_util_mprintf("Classification: %s\n",diagStr_aclActCfStr[pAclRuleEntry->act.extendAct.act]);
        }
        else if(pAclRuleEntry->act.extendAct.act == ACL_IGR_EXTEND_1P_REMARK_ACT)
        {
            diag_util_mprintf("Classification: %s 1p-pri: %d\n",
                                            diagStr_aclActCfStr[pAclRuleEntry->act.extendAct.act],
                                            pAclRuleEntry->act.extendAct.dot1p);
        }
        else if(pAclRuleEntry->act.extendAct.act == ACL_IGR_EXTEND_EXT_ACT)
        {
            diag_util_mask2str(buf,pAclRuleEntry->act.extendAct.portMask.bits[0]);
            diag_util_mprintf("Classification: %s ExtMask: %s\n",
                                            diagStr_aclActCfStr[pAclRuleEntry->act.extendAct.act],
                                            buf);
        }
        else
        {
            diag_util_mprintf("Classification: %s index: %d\n",
                                            diagStr_aclActCfStr[pAclRuleEntry->act.extendAct.act],
                                            pAclRuleEntry->act.extendAct.index);
        }
    #endif
#endif		
    }
    else
    {
        diag_util_mprintf("Interrupt or Classification: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_INTR_ACT]]);
    }


	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID:
			if(pAclRuleEntry->act.enableAct[ACL_IGR_ROUTE_ACT])
			{
				diag_util_printf("Policy Route: %s ",diagStr_aclActPlStr[pAclRuleEntry->act.routeAct.act]);
				switch(pAclRuleEntry->act.svlanAct.act)
				{
					case ACL_IGR_ROUTE_1P_REMARK_ACT:
						diag_util_mprintf("1p-pri: %d\n",pAclRuleEntry->act.routeAct.dot1p);
						break;
					case ACL_IGR_ROUTE_ROUTE_ACT:
						diag_util_mprintf("hexthop: %d\n",pAclRuleEntry->act.routeAct.nexthop);
						break;
					default:
						diag_util_mprintf("\n");
						break;
				}
			}
			else
			{
				diag_util_mprintf("Policy Route: %s\n", diagStr_enable[pAclRuleEntry->act.enableAct[ACL_IGR_ROUTE_ACT]]);
			}

			break;
#endif
		default:
			break;
	}
}

int8 * _diag_aclGetTagName(rtk_acl_care_tag_index_t tag)
{
    static int8 name[20];

	switch(tag)
	{
        case ACL_CARE_TAG_PPPOE: osal_strcpy(name,"PPPoE");break;
        case ACL_CARE_TAG_CTAG: osal_strcpy(name,"Ctag");break;
        case ACL_CARE_TAG_STAG: osal_strcpy(name,"Stag");break;
        case ACL_CARE_TAG_IPV4: osal_strcpy(name,"IP4");break;
        case ACL_CARE_TAG_IPV6: osal_strcpy(name,"IP6");break;
        case ACL_CARE_TAG_TCP: osal_strcpy(name,"Tcp");break;
        case ACL_CARE_TAG_UDP: osal_strcpy(name,"Udp");break;
        default: osal_strcpy(name,"Unknown");break;
	}
    return name;
}

void _diag_aclFieldShow(rtk_acl_field_t *pAclField)
{

    switch(pAclField->fieldType)
    {
        case ACL_FIELD_DMAC:
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&pAclField->fieldUnion.mac.value.octet[0]));
            diag_util_mprintf("     mask: %s\n",diag_util_inet_mactoa(&pAclField->fieldUnion.mac.mask.octet[0]));
            break;
        case ACL_FIELD_SMAC:
            diag_util_mprintf("smac data: %s\n",diag_util_inet_mactoa(&pAclField->fieldUnion.mac.value.octet[0]));
            diag_util_mprintf("     mask: %s\n",diag_util_inet_mactoa(&pAclField->fieldUnion.mac.mask.octet[0]));
            break;
        case ACL_FIELD_IPV4_SIP:
            diag_util_mprintf("sip data: %s\n",diag_util_inet_ntoa(pAclField->fieldUnion.ip.value));
            diag_util_mprintf("    mask: %s\n",diag_util_inet_ntoa(pAclField->fieldUnion.ip.mask));
            break;
        case ACL_FIELD_IPV4_DIP:
            diag_util_mprintf("dip data: %s\n",diag_util_inet_ntoa(pAclField->fieldUnion.ip.value));
            diag_util_mprintf("    mask: %s\n",diag_util_inet_ntoa(pAclField->fieldUnion.ip.mask));
            break;
        case ACL_FIELD_IPV6_DIP:
            diag_util_mprintf("dipv6 data: %s\n",diag_util_inet_n6toa( &pAclField->fieldUnion.ip6.value.ipv6_addr[0]));
            diag_util_mprintf("      mask: %s\n",diag_util_inet_n6toa( &pAclField->fieldUnion.ip6.mask.ipv6_addr[0]));
            break;
        case ACL_FIELD_IPV6_SIP:
            diag_util_mprintf("sipv6 data: %s\n",diag_util_inet_n6toa( &pAclField->fieldUnion.ip6.value.ipv6_addr[0]));
            diag_util_mprintf("      mask: %s\n",diag_util_inet_n6toa( &pAclField->fieldUnion.ip6.mask.ipv6_addr[0]));
            break;
        case ACL_FIELD_ETHERTYPE:
            diag_util_mprintf("ethertype data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_GEMPORT:
            diag_util_mprintf("gemport data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("        mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_IPV6_NEXT_HEADER:
            diag_util_mprintf("tc-nextheader data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("              mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
		case ACL_FIELD_IPV4_PROTOCOL:
			diag_util_mprintf("tos-protocol data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
			diag_util_mprintf("             mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
			break;
        case ACL_FIELD_EXT_PORTMASK:
            diag_util_mprintf("ext-ports data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
		case ACL_FIELD_TCP_DPORT:
			diag_util_mprintf("tcp dport data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
			diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
			break;
		case ACL_FIELD_TCP_SPORT:
			diag_util_mprintf("tcp sport data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
			diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
			break;
		case ACL_FIELD_UDP_DPORT:
			diag_util_mprintf("udp dport data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
			diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
			break;
		case ACL_FIELD_UDP_SPORT:
			diag_util_mprintf("udp sport data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
			diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
			break;

        case ACL_FIELD_VID_RANGE:
            diag_util_mprintf("vid range data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
            diag_util_mprintf("          mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
            break;
        case ACL_FIELD_IP_RANGE:
            diag_util_mprintf("ip range data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
            diag_util_mprintf("         mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
            break;
		case ACL_FIELD_IP_RANGE_0:
			diag_util_mprintf("ip range[15:0] data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
			diag_util_mprintf("               mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
			break;
		case ACL_FIELD_IP_RANGE_1:
			diag_util_mprintf("ip range[31:16] data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
			diag_util_mprintf("                mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
			break;
        case ACL_FIELD_PORT_RANGE:
            diag_util_mprintf("l4 port range data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
            diag_util_mprintf("              mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
            break;
		case ACL_FIELD_PORT_RANGE_0:
			diag_util_mprintf("l4 port range[15:0] data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
			diag_util_mprintf("                    mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
			break;
		case ACL_FIELD_PORT_RANGE_1:
			diag_util_mprintf("l4 port range[31:16] data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
			diag_util_mprintf("                     mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
			break;
        case ACL_FIELD_PKT_LEN_RANGE:
            diag_util_mprintf("pkt length range data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
            diag_util_mprintf("                 mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
            break;
        case ACL_FIELD_USER_VALID:
            diag_util_mprintf("valid field data: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.value));
            diag_util_mprintf("            mask: %s\n",diag_util_mask32tostr(pAclField->fieldUnion.data.mask));
            break;
        case ACL_FIELD_USER_DEFINED00:
            diag_util_mprintf("field[00] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED01:
            diag_util_mprintf("field[01] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED02:
            diag_util_mprintf("field[02] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED03:
            diag_util_mprintf("field[03] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED04:
            diag_util_mprintf("field[04] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED05:
            diag_util_mprintf("field[05] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED06:
            diag_util_mprintf("field[06] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED07:
            diag_util_mprintf("field[07] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED08:
            diag_util_mprintf("field[08] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED09:
            diag_util_mprintf("field[09] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED10:
            diag_util_mprintf("field[10] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED11:
            diag_util_mprintf("field[11] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED12:
            diag_util_mprintf("field[12] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED13:
            diag_util_mprintf("field[13] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED14:
            diag_util_mprintf("field[14] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;
        case ACL_FIELD_USER_DEFINED15:
            diag_util_mprintf("field[15] data: 0x%4.4X\n",pAclField->fieldUnion.data.value);
            diag_util_mprintf("          mask: 0x%4.4X\n",pAclField->fieldUnion.data.mask);
            break;

        case ACL_FIELD_PATTERN_MATCH:
            diag_util_mprintf("pattern data: 0x%4.4X\n",pAclField->fieldUnion.pattern.data.value);
            diag_util_mprintf("        mask: 0x%4.4X\n",pAclField->fieldUnion.pattern.data.mask);
            diag_util_mprintf("        index:%2.2x\n",pAclField->fieldUnion.pattern.fieldIdx);
            break;
        case ACL_FIELD_CTAG:
            diag_util_mprintf("ctag data: priority: %d cfi: %d vid: %d\n",
                                                    pAclField->fieldUnion.l2tag.pri.value,
                                                    pAclField->fieldUnion.l2tag.cfi_dei.value,
                                                    pAclField->fieldUnion.l2tag.vid.value);
            diag_util_mprintf("     mask: priority: %d cfi: %d vid: %d\n",
                                                    pAclField->fieldUnion.l2tag.pri.mask,
                                                    pAclField->fieldUnion.l2tag.cfi_dei.mask,
                                                    pAclField->fieldUnion.l2tag.vid.mask);
            break;
        case ACL_FIELD_STAG:
            diag_util_mprintf("stag data: priority: %d dei: %d vid: %d\n",
                                                    pAclField->fieldUnion.l2tag.pri.value,
                                                    pAclField->fieldUnion.l2tag.cfi_dei.value,
                                                    pAclField->fieldUnion.l2tag.vid.value);
            diag_util_mprintf("     mask: priority: %d dei: %d vid: %d\n",
                                                    pAclField->fieldUnion.l2tag.pri.mask,
                                                    pAclField->fieldUnion.l2tag.cfi_dei.mask,
                                                    pAclField->fieldUnion.l2tag.vid.mask);
            break;


        default:
            diag_util_mprintf("not support\n");
            break;
    }

#if 0



    if(pAclRule->ctag_en)
    {
        diag_util_mprintf("ctag data: priority: %d cfi: %d vid: %d\n",
                                                (pAclRule->ctag>>13)&0x7,
                                                (pAclRule->ctag>>12)&0x1,
                                                pAclRule->ctag&0xFFF);
        diag_util_mprintf("     mask: priority: %d cfi: %d vid: %d\n",
                                                (pAclRule->ctag_mask>>13)&0x7,
                                                (pAclRule->ctag_mask>>12)&0x1,
                                                pAclRule->ctag_mask&0xFFF);
    }

    if(pAclRule->stag_en)
    {
        diag_util_mprintf("stag data: priority: %d dei: %d vid: %d\n",
                                                (pAclRule->stag>>13)&0x7,
                                                (pAclRule->stag>>12)&0x1,
                                                pAclRule->stag&0xFFF);
        diag_util_mprintf("     mask: priority: %d dei: %d vid: %d\n",
                                                (pAclRule->stag_mask>>13)&0x7,
                                                (pAclRule->stag_mask>>12)&0x1,
                                                pAclRule->stag_mask&0xFFF);
    }


    for( fieldIdx = 0; fieldIdx < APOLLO_RAW_ACL_FIELD_SELECTOR_NO; fieldIdx ++)
    {
        if(pAclRule->field_en[fieldIdx])
        {
            diag_util_mprintf("filed[%2d] data: 0x%4.4X mask: 0x%4.4X\n",
                                                fieldIdx,
                                                pAclRule->field[fieldIdx],
                                                pAclRule->field_mask[fieldIdx]);
        }
    }
#endif
}


void _diag_aclRuleShow(rtk_acl_ingress_entry_t *pAclRuleEntry, uint32 mode, rtk_acl_igr_rule_mode_t ruleMode)
{  
    int32 ret = RT_ERR_FAILED;
    uint32 fieldIdx;
    uint32 fieldIdxEnd;
    uint8 buf[64];
    rtk_acl_care_tag_index_t tag;
    rtk_acl_field_t *thisField;

    diag_util_lPortMask2str(buf, &pAclRuleEntry->activePorts);
    diag_util_mprintf("Active Port: %s\n",buf);

    diag_util_printf("Tags: ");
    for(tag = 0; tag < ACL_CARE_TAG_END; tag++)
    {
        if(pAclRuleEntry->careTag.tags[tag].mask) 
        {
            if(pAclRuleEntry->careTag.tags[tag].value)
                diag_util_printf("%s ",_diag_aclGetTagName(tag));
            else
                diag_util_printf("!%s ",_diag_aclGetTagName(tag));
        }        
    }
    diag_util_printf("Mask: ");
    for(tag = 0; tag < ACL_CARE_TAG_END; tag++)
    {
        if(pAclRuleEntry->careTag.tags[tag].mask)
            diag_util_printf("%s ",_diag_aclGetTagName(tag));
    }
    diag_util_mprintf("\n");

    /*display with rule get*/
    if(mode == DIAG_ACL_RULE_DISPLAY_MODE_GET)
    {
        if(ruleMode == ACL_IGR_RULE_MODE_0)
        {
            fieldIdxEnd = HAL_MAX_NUM_OF_ACL_RULE_FIELD();
        }
        else if(pAclRuleEntry->index >= HAL_MAX_NUM_OF_ACL_RULE_ENTRY())
        {
            fieldIdxEnd = 3;
        }
        else
        {
            fieldIdxEnd = 4;
        }

        for( fieldIdx = 0; fieldIdx < fieldIdxEnd; fieldIdx ++)
        {
            diag_util_mprintf("field[%d] data: 0x%4.4X mask: 0x%4.4X\n",
                                            fieldIdx,
                                            pAclRuleEntry->readField.fieldRaw[fieldIdx].value,
                                            pAclRuleEntry->readField.fieldRaw[fieldIdx].mask);
        }
    }
    else
    {
        if(pAclRuleEntry->pFieldHead != NULL)
        {
            thisField = pAclRuleEntry->pFieldHead;
             _diag_aclFieldShow(pAclRuleEntry->pFieldHead);

            while(thisField->next != NULL)            {

                thisField = thisField->next;
                _diag_aclFieldShow(thisField);
            }
        }
    }
}

void _diag_aclReasonShow(void)
{
	int32 ret = RT_ERR_FAILED;
	uint32 val;
	uint32 index;
	rtk_acl_debug_reason_t aclReason;

    (void)rtk_acl_dbgHitReason_get(&aclReason);
	
	switch(DIAG_UTIL_CHIP_TYPE)
	{

#if defined(CONFIG_SDK_RTL9601B)
		case RTL9601B_CHIP_ID:
			diag_util_printf("%-11s %-3s %d\n", "1p-remark",aclReason.hitAct[ACL_IGR_CVLAN_ACT]?"yes":"no",aclReason.index[ACL_IGR_CVLAN_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "dscp-remark",aclReason.hitAct[ACL_IGR_SVLAN_ACT]?"yes":"no",aclReason.index[ACL_IGR_SVLAN_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "priority",aclReason.hitAct[ACL_IGR_PRI_ACT]?"yes":"no",aclReason.index[ACL_IGR_PRI_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "policing",aclReason.hitAct[ACL_IGR_LOG_ACT]?"yes":"no",aclReason.index[ACL_IGR_LOG_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "forward",aclReason.hitAct[ACL_IGR_FORWARD_ACT]?"yes":"no",aclReason.index[ACL_IGR_FORWARD_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "inter-cf",aclReason.hitAct[ACL_IGR_INTR_ACT]?"yes":"no",aclReason.index[ACL_IGR_INTR_ACT]);
			
			break;
#endif
		default:

			
			diag_util_printf("%-11s %-3s %d\n", "cvlan",aclReason.hitAct[ACL_IGR_CVLAN_ACT]?"yes":"no",aclReason.index[ACL_IGR_CVLAN_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "svlan",aclReason.hitAct[ACL_IGR_SVLAN_ACT]?"yes":"no",aclReason.index[ACL_IGR_SVLAN_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "priority",aclReason.hitAct[ACL_IGR_PRI_ACT]?"yes":"no",aclReason.index[ACL_IGR_PRI_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "policing",aclReason.hitAct[ACL_IGR_LOG_ACT]?"yes":"no",aclReason.index[ACL_IGR_LOG_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "forward",aclReason.hitAct[ACL_IGR_FORWARD_ACT]?"yes":"no",aclReason.index[ACL_IGR_FORWARD_ACT]);
			diag_util_printf("%-11s %-3s %d\n", "extend",aclReason.hitAct[ACL_IGR_INTR_ACT]?"yes":"no",aclReason.index[ACL_IGR_INTR_ACT]);
			
#if defined(CONFIG_SDK_RTL9602C)
			diag_util_printf("%-11s %-3s %d\n", "route",aclReason.hitAct[ACL_IGR_ROUTE_ACT]?"yes":"no",aclReason.index[ACL_IGR_ROUTE_ACT]);

#endif
			break;
	}
}

/*
 * acl init
 */
cparser_result_t
cparser_cmd_acl_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_acl_init(), ret);

    osal_memset(&diag_aclTemplate, 0x0, sizeof(rtk_acl_template_t));
	osal_memset(&diag_aclTemplateEn, 0x0, sizeof(diag_aclTemplateEn));

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_init */


/*
 * acl show
 */
cparser_result_t
cparser_cmd_acl_show(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_template_t aclTemplate;
    rtk_acl_igr_rule_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _diag_aclActShow(&diag_aclRuleEntry);

    aclTemplate.index = diag_aclRuleEntry.templateIdx;
    DIAG_UTIL_ERR_CHK(rtk_acl_template_get(&aclTemplate), ret);

    _diag_aclTemplateShow(&aclTemplate);

    if(RT_ERR_DRIVER_NOT_FOUND == rtk_acl_igrRuleMode_get(&mode))
    {
        mode = ACL_IGR_RULE_MODE_0;
    }    

    _diag_aclRuleShow(&diag_aclRuleEntry, DIAG_ACL_RULE_DISPLAY_MODE_SET,mode);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_show */

/*
 * acl clear
 */
cparser_result_t
cparser_cmd_acl_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&diag_aclRuleEntry, 0x0, sizeof(diag_aclRuleEntry));

    diag_aclRuleEntry.pFieldHead = NULL;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_clear */

/*
 * acl show rule
 */
cparser_result_t
cparser_cmd_acl_show_rule(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_template_t aclTemplate;
    rtk_acl_igr_rule_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    aclTemplate.index = diag_aclRuleEntry.templateIdx;
    DIAG_UTIL_ERR_CHK(rtk_acl_template_get(&aclTemplate), ret);

    _diag_aclTemplateShow(&aclTemplate);

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleMode_get(&mode), ret);
    _diag_aclRuleShow(&diag_aclRuleEntry, DIAG_ACL_RULE_DISPLAY_MODE_SET,mode);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_show_rule */


/*
 * acl add entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_add_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_acl_field_t *fieldNext;
    rtk_acl_field_t *fieldThis;

    DIAG_UTIL_PARAM_CHK();
    diag_aclRuleEntry.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleEntry_add(&diag_aclRuleEntry), ret);

    if(diag_aclRuleEntry.pFieldHead != NULL)
    {
        fieldNext = diag_aclRuleEntry.pFieldHead->next;

        osal_free(diag_aclRuleEntry.pFieldHead);

        diag_aclRuleEntry.pFieldHead = NULL;

        fieldThis = fieldNext;
        while(fieldThis != NULL)
        {
            fieldNext = fieldThis->next;
            osal_free(fieldThis);
            /*fieldThis->next = NULL;*/

            fieldThis = fieldNext;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_add_entry_index */

/*
 * acl del entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_del_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;

    DIAG_UTIL_PARAM_CHK();
    index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleEntry_del(index), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_del_entry_index */

/*
 * acl del entry all
 */
cparser_result_t
cparser_cmd_acl_del_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleEntry_delAll(), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_del_entry_all */

/*
 * acl get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_ingress_entry_t aclRule;
    rtk_acl_template_t aclTemplate;
    rtk_acl_igr_rule_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
    
    aclRule.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleEntry_get(&aclRule), ret);

    _diag_aclActShow(&aclRule);
    aclTemplate.index = aclRule.templateIdx;
    DIAG_UTIL_ERR_CHK(rtk_acl_template_get(&aclTemplate), ret);

    _diag_aclTemplateShow(&aclTemplate);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLL) || defined(CONFIG_SDK_APOLLOMP)
        case APOLLO_CHIP_ID:
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleMode_get(&mode), ret);
            break;
#endif           
        default:
            mode = ACL_IGR_RULE_MODE_0;
            break;
    }

    _diag_aclRuleShow(&aclRule, DIAG_ACL_RULE_DISPLAY_MODE_GET, mode);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_entry_index */

/*
 * acl set rule ( dmac | smac ) data <MACADDR:mac> mask <MACADDR:mac_mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_dmac_smac_data_mac_mask_mac_mask(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    cparser_macaddr_t  *mac_mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_SMAC;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_DMAC;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    osal_memcpy(&fieldHead->fieldUnion.mac.value.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    osal_memcpy(&fieldHead->fieldUnion.mac.mask.octet, mac_mask_ptr->octet, ETHER_ADDR_LEN);

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_dmac_smac_data_mac_mask_mac_mask */

/*
 * acl set rule ( dmac | smac ) data <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_acl_set_rule_dmac_smac_data_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_SMAC;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_DMAC;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    osal_memcpy(&fieldHead->fieldUnion.mac.value.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    osal_memset(&fieldHead->fieldUnion.mac.mask.octet, 0xFF, ETHER_ADDR_LEN);

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_dmac_smac_data_mac */

/*
 * acl set rule ( sip | dip ) data <IPV4ADDR:ip> mask <IPV4ADDR:ip_mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_sip_dip_data_ip_mask_ip_mask(
    cparser_context_t *context,
    uint32_t  *ip_ptr,
    uint32_t  *ip_mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_SIP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_DIP;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.ip.value = *ip_ptr;
    fieldHead->fieldUnion.ip.mask = *ip_mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);

    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_sip_dip_data_ip_mask_ip_mask */

/*
 * acl set rule ( sip | dip ) data <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_acl_set_rule_sip_dip_data_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_SIP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_DIP;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.ip.value = *ip_ptr;
    fieldHead->fieldUnion.ip.mask = 0xFFFFFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);

    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_sip_dip_data_ip */

/*
 * acl set rule ( sip6 | dip6 ) data <IPV6ADDR:ip6>
 */
cparser_result_t
cparser_cmd_acl_set_rule_sip6_dip6_data_ip6(
    cparser_context_t *context,
    char * *ip6_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV6_SIP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV6_DIP;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    ret = diag_util_str2ipv6(&fieldHead->fieldUnion.ip6.value.ipv6_addr[0], TOKEN_STR(5));
    if(RT_ERR_OK != ret)
    {
        osal_free(fieldHead);
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    osal_memset(&fieldHead->fieldUnion.ip6.mask,0xFF,sizeof(rtk_ipv6_addr_t));

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_sip6_dip6_data_ip6 */

/*
 * acl set rule ( sip6 | dip6 ) data <IPV6ADDR:ip6> mask <IPV6ADDR:ip6_mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_sip6_dip6_data_ip6_mask_ip6_mask(
    cparser_context_t *context,
    char * *ip6_ptr,
    char * *ip6_mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV6_SIP;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV6_DIP;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    ret = diag_util_str2ipv6(&fieldHead->fieldUnion.ip6.value.ipv6_addr[0], TOKEN_STR(5));
    if(RT_ERR_OK != ret)
    {
        osal_free(fieldHead);
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    ret = diag_util_str2ipv6(&fieldHead->fieldUnion.ip6.mask.ipv6_addr[0], TOKEN_STR(7));
    if(RT_ERR_OK != ret)
    {
        osal_free(fieldHead);
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }  

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_sip6_dip6_data_ip6_mask_ip6_mask */

/*
 * acl set rule ( ethertype | gemport-llid | next-header | ip-proto ) data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ethertype_gemport_llid_next_header_ip_proto_data_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('e' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_ETHERTYPE;
    }
    else if ('g' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_GEMPORT;
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV6_NEXT_HEADER;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {		
        fieldHead->fieldType = ACL_FIELD_IPV4_PROTOCOL;
	}
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    
	
    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ethertype_gemport_llid_next_header_ip_proto_data_data */

/*
 * acl set rule ( ethertype | gemport-llid | next-header | ip-proto ) data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ethertype_gemport_llid_next_header_ip_proto_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('e' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_ETHERTYPE;
    }
    else if ('g' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_GEMPORT;
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV6_NEXT_HEADER;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {		
        fieldHead->fieldType = ACL_FIELD_IPV4_PROTOCOL;
	}
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ethertype_gemport_llid_next_header_ip_proto_data_data_mask_mask */

#if (!defined(CONFIG_REDUCED_DIAG))

/*
 * acl set rule ( tcp-sport | tcp-dport | udp-sport | udp-dport ) data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_tcp_sport_tcp_dport_udp_sport_udp_dport_data_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('t' == TOKEN_CHAR(3,0))
    {
    	if ('s' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_TCP_SPORT;
		else if ('d' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_TCP_DPORT;
		else
		{
		    osal_free(fieldHead);
		    return CPARSER_NOT_OK;
        }    
    }
    else if ('u' == TOKEN_CHAR(3,0))
    {
    	if ('s' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_UDP_SPORT;
		else if ('d' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_UDP_DPORT;
		else
		{
            osal_free(fieldHead);
            return CPARSER_NOT_OK;
        } 
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    
	
    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_tcp_sport_tcp_dport_udp_sport_udp_dport_data_data */

/*
 * acl set rule ( tcp-sport | tcp-dport | udp-sport | udp-dport ) data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_tcp_sport_tcp_dport_udp_sport_udp_dport_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('t' == TOKEN_CHAR(3,0))
    {
    	if ('s' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_TCP_SPORT;
		else if ('d' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_TCP_DPORT;
		else
        {      
            osal_free(fieldHead);
			return CPARSER_NOT_OK;
        }    
    }
    else if ('u' == TOKEN_CHAR(3,0))
    {
    	if ('s' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_UDP_SPORT;
		else if ('d' == TOKEN_CHAR(3,4))
        	fieldHead->fieldType = ACL_FIELD_UDP_DPORT;
		else
        {      
            osal_free(fieldHead);
			return CPARSER_NOT_OK;
        }    
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_tcp_sport_tcp_dport_udp_sport_udp_dport_data_data_mask_mask */



/*
 * acl set rule ( ip-proto-inner | frame-type-tags ) data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ip_proto_inner_frame_type_tags_data_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('i' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_PROTOCOL_INNER;
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_FRAME_TYPE_TAGS;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    
	
    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ip_proto_inner_frame_type_tag_data_data */

/*
 * acl set rule (  ip-proto-inner | frame-type-tags ) data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ip_proto_inner_frame_type_tags_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('i' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_PROTOCOL_INNER;
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_FRAME_TYPE_TAGS;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ip_proto_inner_frame_type_tags_data_data_mask_mask */



/*
 * acl set rule ( sip-inner | dip-inner ) data <IPV4ADDR:ip> mask <IPV4ADDR:ip_mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_sip_inner_dip_inner_data_ip_mask_ip_mask(
    cparser_context_t *context,
    uint32_t  *ip_ptr,
    uint32_t  *ip_mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_SIP_INNER;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_DIP_INNER;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.ip.value = *ip_ptr;
    fieldHead->fieldUnion.ip.mask = *ip_mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);

    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_sip_inner_dip_inner_data_ip_mask_ip_mask */

/*
 * acl set rule ( sip-inner | dip-inner ) data <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_acl_set_rule_sip_inner_dip_inner_data_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('s' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_SIP_INNER;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        fieldHead->fieldType = ACL_FIELD_IPV4_DIP_INNER;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.ip.value = *ip_ptr;
    fieldHead->fieldUnion.ip.mask = 0xFFFFFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_sip_inner_dip_inner_data_ip */




/*
 * acl set rule ( range-l4port-0 | range-l4port-1 | range-ip-0 | range-ip-1 ) data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_range_l4port_0_range_l4port_1_range_ip_0_range_ip_1_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if(!osal_strcmp(TOKEN_STR(3),"range-l4port-0"))
    {
        fieldHead->fieldType = ACL_FIELD_PORT_RANGE_0;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"range-l4port-1"))
    {
        fieldHead->fieldType = ACL_FIELD_PORT_RANGE_1;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"range-ip-0"))
    {
        fieldHead->fieldType = ACL_FIELD_IP_RANGE_0;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"range-ip-1"))
    {
        fieldHead->fieldType = ACL_FIELD_IP_RANGE_1;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_range_l4port_0_range_l4port_1_range_ip_0_range_ip_1_data_data_mask_mask */
#endif

/*
 * acl set rule range-l4port care-range <MASK_LIST:list>
 */
cparser_result_t
cparser_cmd_acl_set_rule_range_l4port_care_range_list(
    cparser_context_t *context,
    char * *list_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 5), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_PORT_RANGE;
    fieldHead->fieldUnion.data.value = mask.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_range_l4port_care_range_list */

/*
 * acl set rule ( range-vid | range-ip | range-length ) care-range <MASK_LIST:list>
 */
cparser_result_t
cparser_cmd_acl_set_rule_range_vid_range_ip_range_length_care_range_list(
    cparser_context_t *context,
    char * *list_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 5), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('v' == TOKEN_CHAR(3,6))
    {
        fieldHead->fieldType = ACL_FIELD_VID_RANGE;
    }
    else if ('i' == TOKEN_CHAR(3,6))
    {
        fieldHead->fieldType = ACL_FIELD_IP_RANGE;
    }
    else if ('l' == TOKEN_CHAR(3,6))
    {
        fieldHead->fieldType = ACL_FIELD_PKT_LEN_RANGE;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.data.value = mask.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_range_vid_range_ip_range_length_care_range_list */


/*
 * acl set rule range-l4port care-range ( <MASK_LIST:list> | none ) mask-range <MASK_LIST:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_range_l4port_care_range_list_none_mask_range_mask(
    cparser_context_t *context,
    char * *list_ptr,
    char * *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t data;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(data, 5), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 7), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_PORT_RANGE;
    fieldHead->fieldUnion.data.value = data.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_range_l4port_care_range_list_none_mask_range_mask */

/*
 * acl set rule ( range-vid | range-ip | range-length ) care-range ( <MASK_LIST:list> | none ) mask-range <MASK_LIST:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_range_vid_range_ip_range_length_care_range_list_none_mask_range_mask(
    cparser_context_t *context,
    char * *list_ptr,
    char * *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t data;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(data, 5), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 7), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    if ('v' == TOKEN_CHAR(3,6))
    {
        fieldHead->fieldType = ACL_FIELD_VID_RANGE;
    }
    else if ('i' == TOKEN_CHAR(3,6))
    {
        fieldHead->fieldType = ACL_FIELD_IP_RANGE;
    }
    else if ('l' == TOKEN_CHAR(3,6))
    {
        fieldHead->fieldType = ACL_FIELD_PKT_LEN_RANGE;
    }
	else
    {      
        osal_free(fieldHead);
		return CPARSER_NOT_OK;
    }    

    fieldHead->fieldUnion.data.value = data.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_range_vid_range_ip_range_length_care_range_list_none_mask_range_mask */


/*
 * acl set rule ctag data vid <UINT:vid> priority <UINT:priority> cfi <UINT:cfi>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ctag_data_vid_vid_priority_priority_cfi_cfi(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr,
    uint32_t  *cfi_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(RTK_VLAN_ID_MAX < *vid_ptr, RT_ERR_VLAN_VID);
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < *priority_ptr), RT_ERR_QOS_1P_PRIORITY);
    DIAG_UTIL_PARAM_RANGE_CHK((1 < *cfi_ptr), RT_ERR_OUT_OF_RANGE);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_CTAG ;

    fieldHead->fieldUnion.l2tag.vid.value = *vid_ptr;
    fieldHead->fieldUnion.l2tag.pri.value = *priority_ptr;
    fieldHead->fieldUnion.l2tag.cfi_dei.value = *cfi_ptr;

    fieldHead->fieldUnion.l2tag.vid.mask = RTK_VLAN_ID_MAX;
    fieldHead->fieldUnion.l2tag.pri.mask = RTK_DOT1P_PRIORITY_MAX;
    fieldHead->fieldUnion.l2tag.cfi_dei.mask = RTK_DOT1P_DEI_MAX;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ctag_data_vid_vid_priority_priority_cfi_cfi */

/*
 * acl set rule ctag data vid <UINT:vid> priority <UINT:priority> cfi <UINT:cfi> mask vid <UINT:vid_mask> priority <UINT:priority_mask> cfi <UINT:cfi_mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ctag_data_vid_vid_priority_priority_cfi_cfi_mask_vid_vid_mask_priority_priority_mask_cfi_cfi_mask(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr,
    uint32_t  *cfi_ptr,
    uint32_t  *vid_mask_ptr,
    uint32_t  *priority_mask_ptr,
    uint32_t  *cfi_mask_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(RTK_VLAN_ID_MAX < *vid_ptr, RT_ERR_VLAN_VID);
    DIAG_UTIL_PARAM_RANGE_CHK(RTK_VLAN_ID_MAX < *vid_mask_ptr, RT_ERR_VLAN_VID);
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < *priority_ptr), RT_ERR_QOS_1P_PRIORITY);
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < *priority_mask_ptr), RT_ERR_QOS_1P_PRIORITY);
    DIAG_UTIL_PARAM_RANGE_CHK((1 < *cfi_ptr), RT_ERR_OUT_OF_RANGE);
    DIAG_UTIL_PARAM_RANGE_CHK((1 < *cfi_mask_ptr), RT_ERR_OUT_OF_RANGE);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_CTAG ;

    fieldHead->fieldUnion.l2tag.vid.value = *vid_ptr;
    fieldHead->fieldUnion.l2tag.pri.value = *priority_ptr;
    fieldHead->fieldUnion.l2tag.cfi_dei.value = *cfi_ptr;

    fieldHead->fieldUnion.l2tag.vid.mask = *vid_mask_ptr;
    fieldHead->fieldUnion.l2tag.pri.mask = *priority_mask_ptr;
    fieldHead->fieldUnion.l2tag.cfi_dei.mask = *cfi_mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ctag_data_vid_vid_priority_priority_cfi_cfi_mask_vid_vid_mask_priority_priority_mask_cfi_cfi_mask */

/*
 * acl set rule stag data vid <UINT:vid> priority <UINT:priority> dei <UINT:dei>
 */
cparser_result_t
cparser_cmd_acl_set_rule_stag_data_vid_vid_priority_priority_dei_dei(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr,
    uint32_t  *dei_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(RTK_VLAN_ID_MAX < *vid_ptr, RT_ERR_VLAN_VID);
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < *priority_ptr), RT_ERR_QOS_1P_PRIORITY);
    DIAG_UTIL_PARAM_RANGE_CHK((1 < *dei_ptr), RT_ERR_OUT_OF_RANGE);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_STAG ;

    fieldHead->fieldUnion.l2tag.vid.value = *vid_ptr;
    fieldHead->fieldUnion.l2tag.pri.value = *priority_ptr;
    fieldHead->fieldUnion.l2tag.cfi_dei.value = *dei_ptr;

    fieldHead->fieldUnion.l2tag.vid.mask = RTK_VLAN_ID_MAX;
    fieldHead->fieldUnion.l2tag.pri.mask = RTK_DOT1P_PRIORITY_MAX;
    fieldHead->fieldUnion.l2tag.cfi_dei.mask = RTK_DOT1P_DEI_MAX;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_stag_data_vid_vid_priority_priority_dei_dei */

/*
 * acl set rule stag data vid <UINT:vid> priority <UINT:priority> dei <UINT:dei> mask vid <UINT:vid_mask> priority <UINT:priority_mask> dei <UINT:dei_mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_stag_data_vid_vid_priority_priority_dei_dei_mask_vid_vid_mask_priority_priority_mask_dei_dei_mask(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr,
    uint32_t  *dei_ptr,
    uint32_t  *vid_mask_ptr,
    uint32_t  *priority_mask_ptr,
    uint32_t  *dei_mask_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(RTK_VLAN_ID_MAX < *vid_ptr, RT_ERR_VLAN_VID);
    DIAG_UTIL_PARAM_RANGE_CHK(RTK_VLAN_ID_MAX < *vid_mask_ptr, RT_ERR_VLAN_VID);
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < *priority_ptr), RT_ERR_QOS_1P_PRIORITY);
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < *priority_mask_ptr), RT_ERR_QOS_1P_PRIORITY);
    DIAG_UTIL_PARAM_RANGE_CHK((1 < *dei_ptr), RT_ERR_OUT_OF_RANGE);
    DIAG_UTIL_PARAM_RANGE_CHK((1 < *dei_mask_ptr), RT_ERR_OUT_OF_RANGE);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_STAG ;

    fieldHead->fieldUnion.l2tag.vid.value = *vid_ptr;
    fieldHead->fieldUnion.l2tag.pri.value = *priority_ptr;
    fieldHead->fieldUnion.l2tag.cfi_dei.value = *dei_ptr;

    fieldHead->fieldUnion.l2tag.vid.mask = *vid_mask_ptr;
    fieldHead->fieldUnion.l2tag.pri.mask = *priority_mask_ptr;
    fieldHead->fieldUnion.l2tag.cfi_dei.mask = *dei_mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_stag_data_vid_vid_priority_priority_dei_dei_mask_vid_vid_mask_priority_priority_mask_dei_dei_mask */

/*
 * acl set rule user-field <UINT:index> data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_user_field_index_data_data(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *data_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(*index_ptr >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);
    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_USER_DEFINED00 + (*index_ptr);

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_user_field_index_data_data */

/*
 * acl set rule user-field <UINT:index> data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_user_field_index_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(*index_ptr >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_USER_DEFINED00 + (*index_ptr);

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_user_field_index_data_data_mask_mask */

/*
 * acl set rule pattern field-index <UINT:index> data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_pattern_field_index_index_data_data(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *data_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_PATTERN_MATCH;

    fieldHead->fieldUnion.pattern.fieldIdx = *index_ptr;
    fieldHead->fieldUnion.pattern.data.value = *data_ptr;
    fieldHead->fieldUnion.pattern.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_pattern_field_index_index_data_data */

/*
 * acl set rule pattern field-index <UINT:index> data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_pattern_field_index_index_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_PATTERN_MATCH;

    fieldHead->fieldUnion.pattern.fieldIdx = *index_ptr;
    fieldHead->fieldUnion.pattern.data.value = *data_ptr;
    fieldHead->fieldUnion.pattern.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_pattern_field_index_index_data_data_mask_mask */

/*
 * acl set rule field-valid care-field <MASK_LIST:list> 
 */
cparser_result_t
cparser_cmd_acl_set_rule_field_valid_care_field_list(
    cparser_context_t *context,
    char * *list_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 5), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_USER_VALID;
    fieldHead->fieldUnion.data.value = mask.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_field_valid_care_field_list */

/*
 * acl set rule field-valid care-field <MASK_LIST:list> mask-field <MASK_LIST:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_field_valid_care_field_list_mask_field_mask(
    cparser_context_t *context,
    char * *list_ptr,
    char * *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t data;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(data, 5), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 7), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_PORT_RANGE;
    fieldHead->fieldUnion.data.value = data.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);

    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_field_valid_care_field_list_mask_field_mask */

/*
 * acl set rule field-valid data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_field_valid_data_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_USER_VALID;

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_field_valid_data_data */

/*
 * acl set rule field-valid data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_field_valid_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    diag_mask_t data;
    diag_mask_t mask;
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(data, 5), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK16(mask, 7), ret);

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_PORT_RANGE;
    fieldHead->fieldUnion.data.value = data.mask.bits[0];
    fieldHead->fieldUnion.data.mask = mask.mask.bits[0];

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_field_valid_data_data_mask_mask */

#if (!defined(CONFIG_REDUCED_DIAG)) 
/*
 * acl set rule ext data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ext_data_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_EXT_PORTMASK;

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = 0xFFFF;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ext_data_data */

/*
 * acl set rule ext data <UINT:data> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_rule_ext_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t  *data_ptr,
    uint32_t  *mask_ptr)
{
    rtk_acl_field_t *fieldHead;

    DIAG_UTIL_PARAM_CHK();

    fieldHead = (rtk_acl_field_t*)osal_alloc(sizeof(rtk_acl_field_t));

    fieldHead->fieldType = ACL_FIELD_EXT_PORTMASK;

    fieldHead->fieldUnion.data.value = *data_ptr;
    fieldHead->fieldUnion.data.mask = *mask_ptr;

    fieldHead->next = NULL;
    rtk_acl_igrRuleField_add(&diag_aclRuleEntry,fieldHead);
    //osal_free(fieldHead);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_ext_data_data_mask_mask */
#endif

/*
 * acl set rule care-tags { ctag } { ip } { ipv6 } { pppoe } { stag } { tcp } { udp } { no-ctag } { no-stag }
 */
cparser_result_t
cparser_cmd_acl_set_rule_care_tags_ctag_ip_ipv6_pppoe_stag_tcp_udp_no_ctag_no_stag(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    int32 tagsIdx;
    DIAG_UTIL_PARAM_CHK();

    for(tagsIdx = 0; tagsIdx < ACL_CARE_TAG_END;tagsIdx ++)
    {
        diag_aclRuleEntry.careTag.tags[tagsIdx].value = DISABLED;
        diag_aclRuleEntry.careTag.tags[tagsIdx].mask = DISABLED;
    }

    for(tokenIdx = 4; tokenIdx <= TOKEN_NUM();tokenIdx ++)
    {
        if(!osal_strcmp(TOKEN_STR(tokenIdx),"ctag"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_CTAG].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_CTAG].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"ip"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_IPV4].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_IPV4].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"ipv6"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_IPV6].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_IPV6].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"pppoe"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_PPPOE].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_PPPOE].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"stag"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_STAG].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_STAG].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"tcp"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_TCP].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_TCP].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"udp"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_UDP].value = ENABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_UDP].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"no-ctag"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_CTAG].value = DISABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_CTAG].mask = ENABLED;
        }
        else if(!osal_strcmp(TOKEN_STR(tokenIdx),"no-stag"))
        {
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_STAG].value = DISABLED;
            diag_aclRuleEntry.careTag.tags[ACL_CARE_TAG_STAG].mask = ENABLED;
        }

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_care_tags_ctag_ip_ipv6_pppoe_stag_tcp_udp */


/*
 * acl set rule port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_acl_set_rule_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    RTK_PORTMASK_ASSIGN(diag_aclRuleEntry.activePorts, portlist.portmask);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_port_ports_all */

/*
 * acl set rule template entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_rule_template_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(*index_ptr >= HAL_MAX_NUM_OF_ACL_TEMPLATE(), RT_ERR_OUT_OF_RANGE);

    diag_aclRuleEntry.templateIdx = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_template_entry_index */

/*
 * acl set rule operation reverse-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_rule_operation_reverse_state_disable_enable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(5,0))
    {
        diag_aclRuleEntry.invert = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        diag_aclRuleEntry.invert = ENABLED;
    }
	else
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_operation_reverse_state_disable_enable */

/*
 * acl set rule state ( valid | invalid )
 */
cparser_result_t
cparser_cmd_acl_set_rule_state_valid_invalid(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if ('i' == TOKEN_CHAR(4,0))
    {
        diag_aclRuleEntry.valid = DISABLED;
    }
    else if ('v' == TOKEN_CHAR(4,0))
    {
        diag_aclRuleEntry.valid = ENABLED;
    }
	else
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_rule_state_valid_invalid */

/*
 * acl get entry <UINT:index> action
 */
cparser_result_t
cparser_cmd_acl_get_entry_index_action(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_ingress_entry_t aclRule;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
    
    aclRule.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleEntry_get(&aclRule), ret);

    _diag_aclActShow(&aclRule);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_entry_index_action */

/*
 * acl get range-vid entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_range_vid_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_vid_t vidRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    vidRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_get(&vidRange), ret);

    diag_util_mprintf("Range check of VID\n");
    diag_util_mprintf("Index: %d Upper: %d Lower: %d Type: %s\n",
                                                    vidRange.index,
                                                    vidRange.upperVid,
                                                    vidRange.lowerVid,
                                                    diagStr_aclRangeCheckVidTypeStr[vidRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_range_vid_entry_index */

/*
 * acl set range-vid entry <UINT:index> state ( invalid | valid )
 */
cparser_result_t
cparser_cmd_acl_set_range_vid_entry_index_state_invalid_valid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_vid_t vidRange;

    DIAG_UTIL_PARAM_CHK();

    vidRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_get(&vidRange), ret);

    if ('i' == TOKEN_CHAR(6,0))
    {
        vidRange.type = VIDRANGE_UNUSED;
        DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_set(&vidRange), ret);
    }
    else if ('v' == TOKEN_CHAR(6,0))
    {
        switch(vidRange.type)
        {
            case VIDRANGE_CVID:
            case VIDRANGE_SVID:
                break;
            default:      
                vidRange.type = VIDRANGE_CVID;
                DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_set(&vidRange), ret);
                break;
        }
    }
	else
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_vid_entry_index_state_invalid_valid */

/*
 * acl set range-vid entry <UINT:index> type ( svid | cvid )
 */
cparser_result_t
cparser_cmd_acl_set_range_vid_entry_index_type_svid_cvid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_vid_t vidRange;

    DIAG_UTIL_PARAM_CHK();

    vidRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_get(&vidRange), ret);

    if ('s' == TOKEN_CHAR(6,0))
    {
        vidRange.type = VIDRANGE_SVID;
    }
    else if ('c' == TOKEN_CHAR(6,0))
    {
        vidRange.type = VIDRANGE_CVID;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_set(&vidRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_vid_entry_index_type_svid_cvid */

/*
 * acl set range-vid entry <UINT:index> low-bound <UINT:vid>
 */
cparser_result_t
cparser_cmd_acl_set_range_vid_entry_index_low_bound_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_vid_t vidRange;

    DIAG_UTIL_PARAM_CHK();

    vidRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_get(&vidRange), ret);

    vidRange.lowerVid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_set(&vidRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_vid_entry_index_low_bound_vid */

/*
 * acl set range-vid entry <UINT:index> up-bound <UINT:vid>
 */
cparser_result_t
cparser_cmd_acl_set_range_vid_entry_index_up_bound_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_vid_t vidRange;

    DIAG_UTIL_PARAM_CHK();

    vidRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_get(&vidRange), ret);

    vidRange.upperVid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_vidRange_set(&vidRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_vid_entry_index_up_bound_vid */

/*
 * acl get range-ip entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_range_ip_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_ip_t ipRange;
	rtk_ipv6_addr_t upIp6Addr,lowIp6Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_get(&ipRange), ret);

    diag_util_mprintf("Range check of IP address\n");

	diag_util_mprintf("Index: %d, Type: %s\n",ipRange.index, diagStr_aclRangeCheckIpTypeStr[ipRange.type]);
	if(ipRange.type == IPRANGE_IPV4_SIP || 
		ipRange.type == IPRANGE_IPV4_DIP ||
		ipRange.type == IPRANGE_IPV4_SIP_INNER ||
		ipRange.type == IPRANGE_IPV4_DIP_INNER )
	{
	    diag_util_mprintf("Upper: %s ",diag_util_inet_ntoa(ipRange.upperIp));
	    diag_util_mprintf("Lower: %s\n",diag_util_inet_ntoa(ipRange.lowerIp));
	}
	else if(ipRange.type == IPRANGE_IPV6_SIP || ipRange.type == IPRANGE_IPV6_DIP)
	{
		osal_memset(&lowIp6Addr,0x00,sizeof(rtk_ipv6_addr_t));
		osal_memset(&upIp6Addr,0x00,sizeof(rtk_ipv6_addr_t));

		lowIp6Addr.ipv6_addr[15] = ipRange.lowerIp&0xFF;
		lowIp6Addr.ipv6_addr[14] = (ipRange.lowerIp>>8)&0xFF;
		lowIp6Addr.ipv6_addr[13] = (ipRange.lowerIp>>16)&0xFF;
		lowIp6Addr.ipv6_addr[12] = (ipRange.lowerIp>>24)&0xFF;
		lowIp6Addr.ipv6_addr[11] = ipRange.lowerIp2&0xFF;
		lowIp6Addr.ipv6_addr[10] = (ipRange.lowerIp2>>8)&0xFF;
		lowIp6Addr.ipv6_addr[9] = (ipRange.lowerIp2>>16)&0xFF;
		lowIp6Addr.ipv6_addr[8] = (ipRange.lowerIp2>>24)&0xFF;
		lowIp6Addr.ipv6_addr[7] = ipRange.lowerIp3&0xFF;
		lowIp6Addr.ipv6_addr[6] = (ipRange.lowerIp3>>8)&0xFF;
		lowIp6Addr.ipv6_addr[5] = (ipRange.lowerIp3>>16)&0xFF;
		lowIp6Addr.ipv6_addr[4] = (ipRange.lowerIp3>>24)&0xFF;
		lowIp6Addr.ipv6_addr[3] = ipRange.lowerIp4&0xFF;
		lowIp6Addr.ipv6_addr[2] = (ipRange.lowerIp4>>8)&0xFF;
		lowIp6Addr.ipv6_addr[1] = (ipRange.lowerIp4>>16)&0xFF;
		lowIp6Addr.ipv6_addr[0] = (ipRange.lowerIp4>>24)&0xFF;

		upIp6Addr.ipv6_addr[15] = ipRange.upperIp&0xFF;
		upIp6Addr.ipv6_addr[14] = (ipRange.upperIp>>8)&0xFF;
		upIp6Addr.ipv6_addr[13] = (ipRange.upperIp>>16)&0xFF;
		upIp6Addr.ipv6_addr[12] = (ipRange.upperIp>>24)&0xFF;
		upIp6Addr.ipv6_addr[11] = ipRange.upperIp2&0xFF;
		upIp6Addr.ipv6_addr[10] = (ipRange.upperIp2>>8)&0xFF;
		upIp6Addr.ipv6_addr[9] = (ipRange.upperIp2>>16)&0xFF;
		upIp6Addr.ipv6_addr[8] = (ipRange.upperIp2>>24)&0xFF;
		upIp6Addr.ipv6_addr[7] = ipRange.upperIp3&0xFF;
		upIp6Addr.ipv6_addr[6] = (ipRange.upperIp3>>8)&0xFF;
		upIp6Addr.ipv6_addr[5] = (ipRange.upperIp3>>16)&0xFF;
		upIp6Addr.ipv6_addr[4] = (ipRange.upperIp3>>24)&0xFF;
		upIp6Addr.ipv6_addr[3] = ipRange.upperIp4&0xFF;
		upIp6Addr.ipv6_addr[2] = (ipRange.upperIp4>>8)&0xFF;
		upIp6Addr.ipv6_addr[1] = (ipRange.upperIp4>>16)&0xFF;
		upIp6Addr.ipv6_addr[0] = (ipRange.upperIp4>>24)&0xFF;
		
		diag_util_mprintf("Upper: %s ",diag_util_inet_n6toa(&upIp6Addr.ipv6_addr[0]));
		diag_util_mprintf("Lower: %s\n",diag_util_inet_n6toa(&lowIp6Addr.ipv6_addr[0]));
	}	
	return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_range_ip_entry_index */

/*
 * acl set range-ip entry <UINT:index> state ( invalid | valid )
 */
cparser_result_t
cparser_cmd_acl_set_range_ip_entry_index_state_invalid_valid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_ip_t ipRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_get(&ipRange), ret);

    if ('i' == TOKEN_CHAR(6,0))
    {
        ipRange.type = IPRANGE_UNUSED;
        DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_set(&ipRange), ret);
    }
    else if ('v' == TOKEN_CHAR(6,0))
    {
        switch(ipRange.type)
        {
            case IPRANGE_IPV4_SIP:
            case IPRANGE_IPV4_DIP:
            case IPRANGE_IPV6_SIP:
            case IPRANGE_IPV6_DIP:
                break;
            default:    
                ipRange.type = IPRANGE_IPV4_SIP;
                DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_set(&ipRange), ret);
            break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_ip_entry_index_state_invalid_valid */

/*
 * acl set range-ip entry <UINT:index> type ( sip | dip | sip6 | dip6 | sip-inner | dip-inner )
 */
cparser_result_t
cparser_cmd_acl_set_range_ip_entry_index_type_sip_dip_sip6_dip6_sip_inner_dip_inner(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_ip_t ipRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_get(&ipRange), ret);

    if(!osal_strcmp(TOKEN_STR(6),"sip"))
    {
        ipRange.type = IPRANGE_IPV4_SIP;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"dip"))
    {
        ipRange.type = IPRANGE_IPV4_DIP;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"sip6"))
    {
        ipRange.type = IPRANGE_IPV6_SIP;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"dip6"))
    {
        ipRange.type = IPRANGE_IPV6_DIP;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"sip-inner"))
    {
        ipRange.type = IPRANGE_IPV4_SIP_INNER;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"dip-inner"))
    {
        ipRange.type = IPRANGE_IPV4_DIP_INNER;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_ip_entry_index_type_sip_dip_sip6_dip6_sip_inner_dip_inner */

/*
 * acl set range-ip entry <UINT:index> low-bound <IPV4ADDR:low_bound_ip> up-bound <IPV4ADDR:up_bound_ip>
 */
cparser_result_t
cparser_cmd_acl_set_range_ip_entry_index_low_bound_low_bound_ip_up_bound_up_bound_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *low_bound_ip_ptr,
    uint32_t  *up_bound_ip_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_ip_t ipRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_get(&ipRange), ret);

    ipRange.lowerIp = *low_bound_ip_ptr;
    ipRange.upperIp = *up_bound_ip_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_ip_entry_index_low_bound_low_bound_ip_up_bound_up_bound_ip */

/*
 * acl set range-ip entry <UINT:index> low-bound-ip6 <IPV6ADDR:low_ip> up-bound-ip6 <IPV6ADDR:up_ip>
 */
cparser_result_t
cparser_cmd_acl_set_range_ip_entry_index_low_bound_ip6_low_ip_up_bound_ip6_up_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *low_ip_ptr,
    char * *ip_ip_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_ip_t ipRange;
	rtk_ipv6_addr_t upIp6Addr,lowIp6Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    ipRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_get(&ipRange), ret);

	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&lowIp6Addr.ipv6_addr[0], TOKEN_STR(6)), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&upIp6Addr.ipv6_addr[0], TOKEN_STR(8)), ret);

    ipRange.lowerIp = lowIp6Addr.ipv6_addr[12];
	ipRange.lowerIp = ipRange.lowerIp<<8 | lowIp6Addr.ipv6_addr[13];
	ipRange.lowerIp = ipRange.lowerIp<<8 | lowIp6Addr.ipv6_addr[14];
	ipRange.lowerIp = ipRange.lowerIp<<8 | lowIp6Addr.ipv6_addr[15];
    ipRange.lowerIp2 = lowIp6Addr.ipv6_addr[8];
	ipRange.lowerIp2 = ipRange.lowerIp2<<8 | lowIp6Addr.ipv6_addr[9];
	ipRange.lowerIp2 = ipRange.lowerIp2<<8 | lowIp6Addr.ipv6_addr[10];
	ipRange.lowerIp2 = ipRange.lowerIp2<<8 | lowIp6Addr.ipv6_addr[11];
    ipRange.lowerIp3 = lowIp6Addr.ipv6_addr[4];
	ipRange.lowerIp3 = ipRange.lowerIp3<<8 | lowIp6Addr.ipv6_addr[5];
	ipRange.lowerIp3 = ipRange.lowerIp3<<8 | lowIp6Addr.ipv6_addr[6];
	ipRange.lowerIp3 = ipRange.lowerIp3<<8 | lowIp6Addr.ipv6_addr[7];
    ipRange.lowerIp4 = lowIp6Addr.ipv6_addr[0];
	ipRange.lowerIp4 = ipRange.lowerIp4<<8 | lowIp6Addr.ipv6_addr[1];
	ipRange.lowerIp4 = ipRange.lowerIp4<<8 | lowIp6Addr.ipv6_addr[2];
	ipRange.lowerIp4 = ipRange.lowerIp4<<8 | lowIp6Addr.ipv6_addr[3];

    ipRange.upperIp = upIp6Addr.ipv6_addr[12];
	ipRange.upperIp = ipRange.upperIp<<8 | upIp6Addr.ipv6_addr[13];
	ipRange.upperIp = ipRange.upperIp<<8 | upIp6Addr.ipv6_addr[14];
	ipRange.upperIp = ipRange.upperIp<<8 | upIp6Addr.ipv6_addr[15];
    ipRange.upperIp2 = upIp6Addr.ipv6_addr[8];
	ipRange.upperIp2 = ipRange.upperIp2<<8 | upIp6Addr.ipv6_addr[9];
	ipRange.upperIp2 = ipRange.upperIp2<<8 | upIp6Addr.ipv6_addr[10];
	ipRange.upperIp2 = ipRange.upperIp2<<8 | upIp6Addr.ipv6_addr[11];
    ipRange.upperIp3 = upIp6Addr.ipv6_addr[4];
	ipRange.upperIp3 = ipRange.upperIp3<<8 | upIp6Addr.ipv6_addr[5];
	ipRange.upperIp3 = ipRange.upperIp3<<8 | upIp6Addr.ipv6_addr[6];
	ipRange.upperIp3 = ipRange.upperIp3<<8 | upIp6Addr.ipv6_addr[7];
    ipRange.upperIp4 = upIp6Addr.ipv6_addr[0];
	ipRange.upperIp4 = ipRange.upperIp4<<8 | upIp6Addr.ipv6_addr[1];
	ipRange.upperIp4 = ipRange.upperIp4<<8 | upIp6Addr.ipv6_addr[2];
	ipRange.upperIp4 = ipRange.upperIp4<<8 | upIp6Addr.ipv6_addr[3];

    DIAG_UTIL_ERR_CHK(rtk_acl_ipRange_set(&ipRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_ip_entry_index_low_bound_ip6_low_ip_up_bound_ip6_up_ip */


/*
 * acl get range-l4port entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_range_l4port_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_l4Port_t portRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_get(&portRange), ret);

    diag_util_mprintf("Range check of L4 port\n");
    diag_util_mprintf("Index: %d Upper: %d Lower: %d Type: %s\n",
                                                    portRange.index,
                                                    portRange.upper_bound,
                                                    portRange.lower_bound,
                                                    diagStr_aclRangeCheckPortTypeStr[portRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_range_l4port_entry_index */

/*
 * acl set range-l4port entry <UINT:index> state ( invalid | valid )
 */
cparser_result_t
cparser_cmd_acl_set_range_l4port_entry_index_state_invalid_valid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_l4Port_t portRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_get(&portRange), ret);

    if ('i' == TOKEN_CHAR(6,0))
    {
        portRange.type = PORTRANGE_UNUSED;
        DIAG_UTIL_ERR_CHK(rtk_acl_portRange_set(&portRange), ret);
    }
    else if ('v' == TOKEN_CHAR(6,0))            
    {
        switch(portRange.type)
        {
            case PORTRANGE_DPORT:
            case PORTRANGE_SPORT:
                break;
            default:    
                portRange.type = PORTRANGE_SPORT;
                DIAG_UTIL_ERR_CHK(rtk_acl_portRange_set(&portRange), ret);
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_l4port_entry_index_state_invalid_valid */

/*
 * acl set range-l4port entry <UINT:index> type ( src-port | dst-port )
 */
cparser_result_t
cparser_cmd_acl_set_range_l4port_entry_index_type_src_port_dst_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_l4Port_t portRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_get(&portRange), ret);

    if ('s' == TOKEN_CHAR(6,0))
    {
        portRange.type = PORTRANGE_SPORT;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        portRange.type = PORTRANGE_DPORT;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_set(&portRange), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_l4port_entry_index_type_src_port_dst_port */

/*
 * acl set range-l4port entry <UINT:index> low-bound <UINT:l4port>
 */
cparser_result_t
cparser_cmd_acl_set_range_l4port_entry_index_low_bound_l4port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l4port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_l4Port_t portRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_get(&portRange), ret);

    portRange.lower_bound = *l4port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_set(&portRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_l4port_entry_index_low_bound_l4port */

/*
 * acl set range-l4port entry <UINT:index> up-bound <UINT:l4port>
 */
cparser_result_t
cparser_cmd_acl_set_range_l4port_entry_index_up_bound_l4port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l4port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_l4Port_t portRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    portRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_get(&portRange), ret);

    portRange.upper_bound = *l4port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_portRange_set(&portRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_l4port_entry_index_up_bound_l4port */

/*
 * acl get range-length entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_range_length_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_pktLength_t pktLenRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    pktLenRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_get(&pktLenRange), ret);

    diag_util_mprintf("Range check of packet length\n");
    diag_util_mprintf("Index: %d Upper: %d Lower: %d Type: %s\n",
                                                    pktLenRange.index,
                                                    pktLenRange.upper_bound,
                                                    pktLenRange.lower_bound,
                                                    diagStr_aclRangeCheckLenTypeStr[pktLenRange.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_range_length_entry_index */

/*
 * acl set range-length entry <UINT:index> low-bound <UINT:length>
 */
cparser_result_t
cparser_cmd_acl_set_range_length_entry_index_low_bound_length(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *length_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_pktLength_t pktLenRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    pktLenRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_get(&pktLenRange), ret);

    pktLenRange.lower_bound = *length_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_set(&pktLenRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_length_entry_index_low_bound_length */

/*
 * acl set range-length entry <UINT:index> up-bound <UINT:length>
 */
cparser_result_t
cparser_cmd_acl_set_range_length_entry_index_up_bound_length(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *length_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_pktLength_t pktLenRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    pktLenRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_get(&pktLenRange), ret);

    pktLenRange.upper_bound = *length_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_set(&pktLenRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_length_entry_index_up_bound_length */

/*
 * acl set range-length entry <UINT:index> reverse-state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_range_length_entry_index_reverse_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_rangeCheck_pktLength_t pktLenRange;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    pktLenRange.index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_get(&pktLenRange), ret);

    if ('d' == TOKEN_CHAR(6,0))
    {
        pktLenRange.type = PKTLENRANGE_NOTREVISE;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        pktLenRange.type = PKTLENRANGE_REVISE;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_packetLengthRange_set(&pktLenRange), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_range_length_entry_index_reverse_state_disable_enable */

/*
 * acl get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_acl_get_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);


    diag_util_mprintf("Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_igrState_get(port, &enable), ret);
        diag_util_mprintf("%-5d%s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_port_ports_all_state */

/*
 * acl set port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_acl_set_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_igrState_set(port, enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_port_ports_all_state_enable_disable */

/*
 * acl get port ( <PORT_LIST:ports> | all ) permit
 */
cparser_result_t
cparser_cmd_acl_get_port_ports_all_permit(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
       DIAG_UTIL_ERR_CHK(rtk_acl_igrPermitState_get(port, &enable), ret);

       diag_util_mprintf("%-5d%s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_port_ports_all_permit */

/*
 * acl set port ( <PORT_LIST:ports> | all ) permit ( enable | disable )
 */
cparser_result_t
cparser_cmd_acl_set_port_ports_all_permit_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }

        

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_igrPermitState_set(port, enable), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_port_ports_all_permit_enable_disable */

#if (!defined(CONFIG_REDUCED_DIAG))

/*
 * acl set action cvlan ingress vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_ingress_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_EXT_VLAN_ID_MAX < *vid_ptr), RT_ERR_VLAN_VID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_IGR_CVLAN_ACT;
    diag_aclRuleEntry.act.cvlanAct.cvid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_ingress_vid_vid */

/*
 * acl set action cvlan egress vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_egress_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_EXT_VLAN_ID_MAX < *vid_ptr), RT_ERR_VLAN_VID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_EGR_CVLAN_ACT;
    diag_aclRuleEntry.act.cvlanAct.cvid = *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_egress_vid_vid */

/*
 * acl set action cvlan using-svid
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_using_svid(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_DS_SVID_ACT;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_using_svid */

/*
 * acl set action cvlan meter <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_METERING()), RT_ERR_FILTER_METER_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_POLICING_ACT;
    diag_aclRuleEntry.act.cvlanAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_meter_index */

/*
 * acl set action cvlan statistic <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_statistic_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_MIB_ACT;
    diag_aclRuleEntry.act.cvlanAct.mib = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_statistic_index */

/*
 * acl set action cvlan remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_1P_REMARK_ACT;
    diag_aclRuleEntry.act.cvlanAct.dot1p = *priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_remarking_dot1p_priority */

/*
 * acl set action cvlan bandwidth-metering <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_cvlan_bandwidth_metering_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_BW_METER_ACT;
    diag_aclRuleEntry.act.cvlanAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_cvlan_bandwidth_metering_index */

/*
 * acl set action svlan ingress svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_ingress_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(DIAG_SVLAN_VID_NOT_ACCEPT(*svid_ptr), RT_ERR_VLAN_VID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_IGR_SVLAN_ACT;
    diag_aclRuleEntry.act.svlanAct.svid = *svid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_ingress_svlan_index_svidx */

/*
 * acl set action svlan egress svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_egress_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(DIAG_SVLAN_VID_NOT_ACCEPT(*svid_ptr), RT_ERR_VLAN_VID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_EGR_SVLAN_ACT;
    diag_aclRuleEntry.act.svlanAct.svid = *svid_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_egress_svid_svid */

/*
 * acl set action svlan using-cvid
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_using_cvid(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_US_CVID_ACT;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_using_cvid */

/*
 * acl set action svlan meter <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_METERING()), RT_ERR_FILTER_METER_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_POLICING_ACT;
    diag_aclRuleEntry.act.svlanAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_meter_index */

/*
 * acl set action svlan statistic <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_statistic_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_MIB_ACT;
    diag_aclRuleEntry.act.svlanAct.mib = *index_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_statistic_index */

/*
 * acl set action svlan remarking dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_remarking_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), RT_ERR_QOS_DSCP_VALUE);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
    diag_aclRuleEntry.act.svlanAct.dscp = *dscp_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_remarking_dscp_dscp */

/*
 * acl set action svlan remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_1P_REMARK_ACT;
    diag_aclRuleEntry.act.svlanAct.dot1p = *priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_remarking_dot1p_priority */

/*
 * acl set action svlan route nexthop <UINT:index> 
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_route_nexthop_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_NEXTHOP()), RT_ERR_OUT_OF_RANGE);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_ROUTE_ACT;
    diag_aclRuleEntry.act.svlanAct.nexthop = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_route_nexthop_index */

/*
 * acl set action svlan bandwidth-metering <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_svlan_bandwidth_metering_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_BW_METER_ACT;
    diag_aclRuleEntry.act.svlanAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_svlan_bandwidth_metering_index */
#endif

/*
 * acl set action priority assign-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_assign_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
    diag_aclRuleEntry.act.priAct.aclPri = *priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_assign_priority_priority */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * acl set action priority remarking dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_remarking_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), RT_ERR_QOS_DSCP_VALUE);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_DSCP_REMARK_ACT;
    diag_aclRuleEntry.act.priAct.dscp = *dscp_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_remarking_dscp_dscp */

/*
 * acl set action priority remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_1P_REMARK_ACT;
    diag_aclRuleEntry.act.priAct.dot1p = *priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_remarking_dot1p_priority */

/*
 * acl set action priority meter <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_METERING()), RT_ERR_FILTER_METER_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_POLICING_ACT;
    diag_aclRuleEntry.act.priAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_meter_index */

/*
 * acl set action priority remarking tos-tc <UINT:tos> mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_remarking_tos_tc_tos_mask_mask(
    cparser_context_t *context,
    uint32_t  *tos_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_TOS_REMARK_ACT;
    diag_aclRuleEntry.act.priAct.tos = *tos_ptr;
    diag_aclRuleEntry.act.priAct.tosMask = *mask_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_remarking_tos_tc_tos_mask_mask */


/*
 * acl set action priority statistic <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_statistic_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_MIB_ACT;
    diag_aclRuleEntry.act.priAct.mib = *index_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_statistic_index */

/*
 * acl set action priority route nexthop <UINT:index> 
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_route_nexthop_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_NEXTHOP()), RT_ERR_OUT_OF_RANGE);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_ROUTE_ACT;
    diag_aclRuleEntry.act.priAct.nexthop = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_route_nexthop_index */

/*
 * acl set action priority bandwidth-metering <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_priority_bandwidth_metering_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_PRI_ACT]= ENABLED;
    diag_aclRuleEntry.act.priAct.act = ACL_IGR_PRI_BW_METER_ACT;
    diag_aclRuleEntry.act.priAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_bandwidth_metering_index */
#endif

/*
 * acl set action remarking dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_acl_set_action_remarking_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), RT_ERR_QOS_DSCP_VALUE);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_SVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
    diag_aclRuleEntry.act.svlanAct.dscp = *dscp_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_priority_remarking_dscp_dscp */

/*
 * acl set action remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_CVLAN_ACT]= ENABLED;
    diag_aclRuleEntry.act.cvlanAct.act = ACL_IGR_CVLAN_1P_REMARK_ACT;
    diag_aclRuleEntry.act.cvlanAct.dot1p = *priority_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_remarking_dot1p_priority */



/*
 * acl set action meter <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_METERING()), RT_ERR_FILTER_METER_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_LOG_ACT]= ENABLED;
    diag_aclRuleEntry.act.logAct.act = ACL_IGR_LOG_POLICING_ACT;
    diag_aclRuleEntry.act.logAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_meter_index */

/*
 * acl set action statistic <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_statistic_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_LOG_ACT]= ENABLED;
    diag_aclRuleEntry.act.logAct.act = ACL_IGR_LOG_MIB_ACT;
    diag_aclRuleEntry.act.logAct.mib = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_statistic_index */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * acl set action bandwidth-metering <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_action_bandwidth_metering_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_METER_ID);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_LOG_ACT]= ENABLED;
    diag_aclRuleEntry.act.logAct.act = ACL_IGR_LOG_BW_METER_ACT;
    diag_aclRuleEntry.act.logAct.meter = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_bandwidth_metering_index */

/*
 * acl set action police remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_police_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_LOG_ACT]= ENABLED;
    diag_aclRuleEntry.act.logAct.act = ACL_IGR_LOG_1P_REMARK_ACT;
    diag_aclRuleEntry.act.logAct.dot1p = *priority_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_police_remarking_dot1p_priority */
#endif
/*
 * acl set action trap-to-cpu
 */
cparser_result_t
cparser_cmd_acl_set_action_trap_to_cpu(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_trap_to_cpu */

/*
 * acl set action copy to cpu
 */
cparser_result_t
cparser_cmd_acl_set_action_copy_to_cpu(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = 1 << HAL_GET_CPU_PORT();

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_copy_to_cpu */


/*
 * acl set action drop
 */
cparser_result_t
cparser_cmd_acl_set_action_drop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
			
            break;
#endif
        default:
            diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;

            break;
    }
    
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = 0;
    
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_drop */

/*
 * acl set action drop
 */
cparser_result_t
cparser_cmd_acl_set_action_forward_nop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_NOP_ACT;    
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = 0;
    
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_forward_nop */

#if (!defined(CONFIG_REDUCED_DIAG)) 
/*
 * acl set action trap-to-slave-cpu
 */
cparser_result_t
cparser_cmd_acl_set_action_trap_to_slave_cpu(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_TRAP2SLAVE_ACT;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_trap_to_slave_cpu */


/*
 * acl set action copy port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_acl_set_action_copy_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = portlist.portmask.bits[0];

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_copy_port_ports_all */

/*
 * acl set action redirect port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_acl_set_action_redirect_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = portlist.portmask.bits[0];


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_redirect_port_ports_all_none */

/*
 * acl set action mirror port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_acl_set_action_mirror_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_IGR_MIRROR_ACT;
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = portlist.portmask.bits[0];

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_mirror_port_ports_all */

/*
 * acl set action egressmask port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_acl_set_action_egressmember_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;
    diag_aclRuleEntry.act.forwardAct.act = ACL_IGR_FORWARD_EGRESSMASK_ACT;
    diag_aclRuleEntry.act.forwardAct.portMask.bits[0] = portlist.portmask.bits[0];

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_egressmember_port_ports_all */
#endif

/*
 * acl set action interrupt
 */
cparser_result_t
cparser_cmd_acl_set_action_interrupt(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.aclInterrupt = ENABLED;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_interrupt */

/*
 * acl set action latch-index
 */
cparser_result_t
cparser_cmd_acl_set_action_latch_index(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.aclLatch = ENABLED;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_latch_index */

#if (!defined(CONFIG_REDUCED_DIAG))

/*
 * acl set action classf none
 */
cparser_result_t
cparser_cmd_acl_set_action_classf_none(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.extendAct.act = ACL_IGR_EXTEND_NONE_ACT;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_classf_none */

/*
 * acl set action classf sid <UINT:sid>
 */
cparser_result_t
cparser_cmd_acl_set_action_classf_sid_sid(
    cparser_context_t *context,
    uint32_t  *sid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.extendAct.act = ACL_IGR_EXTEND_SID_ACT;
    diag_aclRuleEntry.act.extendAct.index = *sid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_classf_sid_sid */

/*
 * acl set action classf llid <UINT:llid>
 */
cparser_result_t
cparser_cmd_acl_set_action_classf_llid_llid(
    cparser_context_t *context,
    uint32_t  *llid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.extendAct.act = ACL_IGR_EXTEND_LLID_ACT;
    diag_aclRuleEntry.act.extendAct.index = *llid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_classf_llid_llid */


/*
 * acl set action classf ext-member ( <PORT_LIST:ext> | all )
 */
cparser_result_t
cparser_cmd_acl_set_action_classf_ext_member_ext_all(
    cparser_context_t *context,
    char * *ext_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.extendAct.act = ACL_IGR_EXTEND_EXT_ACT;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 5), ret);
    RTK_PORTMASK_ASSIGN(diag_aclRuleEntry.act.extendAct.portMask, portlist.portmask);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_classf_ext_vidx */

/*
 * acl set action classf remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_classf_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_INTR_ACT]= ENABLED;
    diag_aclRuleEntry.act.extendAct.act = ACL_IGR_EXTEND_1P_REMARK_ACT;
    diag_aclRuleEntry.act.extendAct.dot1p = *priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_classf_remarking_dot1p_priority */



/*
 * acl set action route nexthop <UINT:index> 
 */
cparser_result_t
cparser_cmd_acl_set_action_route_nexthop_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr >= HAL_MAX_NUM_OF_NEXTHOP()), RT_ERR_OUT_OF_RANGE);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_ROUTE_ACT]= ENABLED;
    diag_aclRuleEntry.act.routeAct.act = ACL_IGR_ROUTE_ROUTE_ACT;
    diag_aclRuleEntry.act.routeAct.nexthop = *index_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_route_nexthop_index */

/*
 * acl set action route remarking dot1p <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_action_route_remarking_dot1p_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*priority_ptr > HAL_INTERNAL_PRIORITY_MAX()), RT_ERR_QOS_INT_PRIORITY);

    diag_aclRuleEntry.act.enableAct[ACL_IGR_ROUTE_ACT]= ENABLED;
    diag_aclRuleEntry.act.routeAct.act = ACL_IGR_ROUTE_1P_REMARK_ACT;
    diag_aclRuleEntry.act.routeAct.dot1p = *priority_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_route_remarking_dot1p_priority */

#endif


/*
 * acl set action none
 */
cparser_result_t
cparser_cmd_acl_set_action_none(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&diag_aclRuleEntry.act, 0x0, sizeof(diag_aclRuleEntry.act));

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_action_none */

/*
 * acl show action
 */
cparser_result_t
cparser_cmd_acl_show_action(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _diag_aclActShow(&diag_aclRuleEntry);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_show_action */

/*
 * acl clear action
 */
cparser_result_t
cparser_cmd_acl_clear_action(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&diag_aclRuleEntry.act, 0x0, sizeof(diag_aclRuleEntry.act));

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_clear_action */

/*
 * acl show template
 */
cparser_result_t
cparser_cmd_acl_show_template(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();
    _diag_aclTemplateShow(&diag_aclTemplate);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_show_template */

/*
 * acl clear template
 */
cparser_result_t
cparser_cmd_acl_clear_template(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&diag_aclTemplate, 0x0, sizeof(rtk_acl_template_t));
	osal_memset(&diag_aclTemplateEn, 0x0, sizeof(diag_aclTemplateEn));
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_clear_template */

/*
 * acl set template ( stag | ctag | ethertype | gem-llid | ipv6-next-header | unknown | range-ip )
 */
cparser_result_t
cparser_cmd_acl_set_template_stag_ctag_ethertype_gem_llid_ipv6_next_header_unknown_range_ip(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('c' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_CTAG), ret);
    }
    else if('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_STAG), ret);
    }
    else if('e' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_ETHERTYPE), ret);
    }
    else if('g' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_GEMPORT), ret);
    }
    else if('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_NEXT_HEADER), ret);
    }
    else if('u' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_UNUSED), ret);
    }
    else if('r' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IP_RANGE), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_stag_ctag_ethertype_gem_llid_ipv6_next_header_unknown_range_ip */

/*
 * acl set template ( dip | sip | range-length )
 */
cparser_result_t
cparser_cmd_acl_set_template_dip_sip_range_length(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_DIP0), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_DIP1), ret);
    }
    else if('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_SIP0), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_SIP1), ret);
    }
    else if('r' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_PKT_LEN_RANGE), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_dip_sip_range_length */

/*
 * acl set template ( smac | dmac | range-l4port )
 */
cparser_result_t
cparser_cmd_acl_set_template_smac_dmac_range_l4port(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_DMAC0), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_DMAC1), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_DMAC2), ret);
    }
    else if('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_SMAC0), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_SMAC1), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_SMAC2), ret);
    }
    else if('r' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_PORT_RANGE), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_smac_dmac_range_l4port */

/*
 * acl set template ( sip6 | dip6 | range-vid )
 */
cparser_result_t
cparser_cmd_acl_set_template_sip6_dip6_range_vid(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3,0))
    {
		switch(DIAG_UTIL_CHIP_TYPE)
		{
#if defined(CONFIG_SDK_APOLLOMP)
			case APOLLOMP_CHIP_ID:
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP0), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP1), ret);			
				break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
			case RTL9601B_CHIP_ID:
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP0), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP1), ret);			
				break;
#endif
			default:
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP0), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP1), ret);			
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP2), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP3), ret);			
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP4), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP5), ret);			
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP6), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP7), ret);		
				break;
		}
    }
    else if('s' == TOKEN_CHAR(3,0))
    {
		switch(DIAG_UTIL_CHIP_TYPE)
		{
#if defined(CONFIG_SDK_APOLLOMP)
			case APOLLOMP_CHIP_ID:
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP0), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP1), ret);			
				break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
			case RTL9601B_CHIP_ID:
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP0), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP1), ret);			
				break;
#endif
			default:
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP0), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP1), ret);			
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP2), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP3), ret);			
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP4), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP5), ret);			
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP6), ret);
				DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP7), ret);		
				break;
		}
    }
    else if('r' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_VID_RANGE), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_sip6_dip6_range_vid */

/*
 * acl set template ( sip-15-0 | sip-31-16 | dip-15-0 | dip-31-16 | ipv4-protocol )
 */
cparser_result_t
cparser_cmd_acl_set_template_sip_15_0_sip_31_16_dip_15_0_dip_31_16_ipv4_protocol(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(3),"sip-15-0"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_SIP0), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"sip-31-16"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_SIP1), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip-15-0"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_DIP0), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip-31-16"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_DIP1), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"ipv4-protocol"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_PROTOCOL), ret);
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_sip_15_0_sip_31_16_dip_15_0_dip_31_16_ipv4_protocol */

/*
 * acl set template ( sip6-15-0 | sip6-31-16 | dip6-15-0 | dip6-31-16 )
 */
cparser_result_t
cparser_cmd_acl_set_template_sip6_15_0_sip6_31_16_dip6_15_0_dip6_31_16(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(3),"sip6-15-0"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP0), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"sip6-31-16"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP1), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-15-0"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP0), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-31-16"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP1), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_sip6_15_0_sip6_31_16_dip6_15_0_dip6_31_16 */

#if (!defined(CONFIG_REDUCED_DIAG)) 

/*
 * acl set template ( sip-inner | dip-inner | ip-proto-inner | frame-type-tags )
 */
cparser_result_t
cparser_cmd_acl_set_template_sip_inner_dip_inner_ip_proto_inner_frame_type_tags(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_DIP0_INNER), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_DIP1_INNER), ret);
    }
    else if('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_SIP0_INNER), ret);
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_SIP1_INNER), ret);
    }
    else if('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV4_PROTOCOL_INNER), ret);
    }
    else if('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_FRAME_TYPE_TAGS), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_sip_inner_dip_inner_ip_proto_inner_frame_type_tags */



/*
 * acl set template ( tcp-sport | tcp-dport )
 */
cparser_result_t
cparser_cmd_acl_set_template_tcp_sport_tcp_dport(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3,4))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_TCP_DPORT), ret);
    }
    else if('s' == TOKEN_CHAR(3,4))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_TCP_SPORT), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_tcp_sport_tcp_dport */

/*
 * acl set template ( udp-sport | udp-dport )
 */
cparser_result_t
cparser_cmd_acl_set_template_udp_sport_udp_dport(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('d' == TOKEN_CHAR(3,4))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_UDP_DPORT), ret);
    }
    else if('s' == TOKEN_CHAR(3,4))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_UDP_SPORT), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_udp_sport_udp_dport */


/*
 * acl set template ( sip6-79-64 | sip6-95-80 | sip6-111-96 | sip6-127-112 )
 */
cparser_result_t
cparser_cmd_acl_set_template_sip6_79_64_sip6_95_80_sip6_111_96_sip6_127_112(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(3),"sip6-79-64"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP4), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"sip6-95-80"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP5), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"sip6-111-96"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP6), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"sip6-127-112"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP7), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_sip6_79_64_sip6_95_80_sip6_111_96_sip6_127_112 */

/*
 * acl set template ( sip6-47-32 | sip6-63-48 | dip6-47-32 | dip6-63-48 )
 */
cparser_result_t
cparser_cmd_acl_set_template_sip6_47_32_sip6_63_48_dip6_47_32_dip6_63_48(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(3),"sip6-47-32"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP2), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"sip6-63-48"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_SIP3), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-47-32"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP2), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-63-48"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP3), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_sip6_47_32_sip6_63_48_dip6_47_32_dip6_63_48 */

/*
 * acl set template ( dip6-79-64 | dip6-95-80 | dip6-111-96 | dip6-127-112 )
 */
cparser_result_t
cparser_cmd_acl_set_template_dip6_79_64_dip6_95_80_dip6_111_96_dip6_127_112(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(3),"dip6-79-64"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP4), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-95-80"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP5), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-111-96"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP6), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dip6-127-112"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_IPV6_DIP7), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_dip6_79_64_dip6_95_80_dip6_111_96_dip6_127_112 */
#endif

/*
 * acl set template ( smac-15-0 | smac-31-16 | smac-47-32 | dmac-15-0 | dmac-31-16 | dmac-47-32 )
 */
cparser_result_t
cparser_cmd_acl_set_template_smac_15_0_smac_31_16_smac_47_32_dmac_15_0_dmac_31_16_dmac_47_32(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(3),"smac-15-0"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_SMAC0), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"smac-31-16"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_SMAC1), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"smac-47-32"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_SMAC2), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dmac-15-0"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_DMAC0), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dmac-31-16"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_DMAC1), ret);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dmac-47-32"))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_DMAC2), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_smac_15_0_smac_31_16_smac_47_32_dmac_15_0_dmac_31_16_dmac_47_32 */

/*
 * acl set template user-field <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_set_template_user_field_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_field_type_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr) >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    type = ACL_FIELD_USER_DEFINED00 + (*index_ptr);

    DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(type), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_user_field_index */

/*
 * acl set template ( field-valid | ext-pmask )
 */
cparser_result_t
cparser_cmd_acl_set_template_field_valid_ext_pmask(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_USER_VALID), ret);
    }
    else if('e' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(_diag_aclTemplateAdd(ACL_FIELD_EXT_PORTMASK), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_template_field_valid_ext_pmask */



/*
 * acl add template entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_add_template_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_aclTemplate.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_template_set(&diag_aclTemplate), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_add_template_entry_index */

/*
 * acl del template entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_del_template_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_template_t aclTemplate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&aclTemplate, ACL_FIELD_UNUSED, sizeof(rtk_acl_template_t));
    aclTemplate.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_template_set(&aclTemplate), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_del_template_entry_index */

/*
 * acl get template entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_template_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_template_t aclTemplate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    aclTemplate.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_template_get(&aclTemplate), ret);

    _diag_aclTemplateShow(&aclTemplate);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_template_entry_index */


#if (!defined(CONFIG_REDUCED_DIAG)) 
/*
 * acl set mode ( 64-entries | 128-entries )
 */
cparser_result_t
cparser_cmd_acl_set_mode_64_entries_128_entries(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_igr_rule_mode_t mode;

    DIAG_UTIL_PARAM_CHK();

    if ('6' == TOKEN_CHAR(3,0))
    {
        mode = ACL_IGR_RULE_MODE_0;
    }
    else if ('1' == TOKEN_CHAR(3,0))
    {
        mode = ACL_IGR_RULE_MODE_1;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleMode_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_mode_64_entries_128_entries */

/*
 * acl get mode
 */
cparser_result_t
cparser_cmd_acl_get_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_igr_rule_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleMode_get(&mode), ret);

    diag_util_mprintf("ACL mode: %s\n", diagStr_aclModeStr[mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_mode */

/*
 * field-selector set index <UINT:index> format ( inner-ipv4-header | inner-ipv6-header | inner-ip-payload) offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_field_selector_set_index_index_format_inner_ipv4_header_inner_ipv6_header_inner_ip_payload_offset_offset(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *offset_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_field_entry_t fieldSel;

    DIAG_UTIL_PARAM_CHK();

    fieldSel.index = *index_ptr;
    fieldSel.offset = *offset_ptr;

    if(!osal_strcmp(TOKEN_STR(5),"inner-ipv4-header"))
    {
        fieldSel.format = ACL_FORMAT_IPV4_INNER;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"inner-ipv6-header"))
    {
        fieldSel.format = ACL_FORMAT_IPV6_INNER;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"inner-ip-payload"))
    {
        fieldSel.format = ACL_FORMAT_IPPAYLOAD_INNER;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_fieldSelect_set(&fieldSel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_field_selector_set_index_index_format_inner_ipv4_header_inner_ipv6_header_inner_ip_payload_offset_offset */

/*
 * field-selector set index <UINT:index> format ( outer-ipv4-header | outer-ipv6-header | outer-ip-payload) offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_field_selector_set_index_index_format_outer_ipv4_header_outer_ipv6_header_outer_ip_payload_offset_offset(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *offset_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_field_entry_t fieldSel;

    DIAG_UTIL_PARAM_CHK();

    fieldSel.index = *index_ptr;
    fieldSel.offset = *offset_ptr;

    if(!osal_strcmp(TOKEN_STR(5),"outer-ipv4-header"))
    {
        fieldSel.format = ACL_FORMAT_IPV4_OUTER;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"outer-ipv6-header"))
    {
        fieldSel.format = ACL_FORMAT_IPV6_OUTER;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"outer-ip-payload"))
    {
        fieldSel.format = ACL_FORMAT_IPPAYLOAD_OUTER;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_fieldSelect_set(&fieldSel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_field_selector_set_index_index_format_outer_ipv4_header_outer_ipv6_header_outer_ip_payload_offset_offset */


#endif
/*
 * field-selector set index <UINT:index> format ( default | raw | llc | pppoe | arp | ipv4-header | ipv6-header | ip-payload | l4-payload ) offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_field_selector_set_index_index_format_default_raw_llc_pppoe_arp_ipv4_header_ipv6_header_ip_payload_l4_payload_offset_offset(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *offset_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_field_entry_t fieldSel;

    DIAG_UTIL_PARAM_CHK();

    fieldSel.index = *index_ptr;
    fieldSel.offset = *offset_ptr;

    if(!osal_strcmp(TOKEN_STR(5),"default"))
    {
        fieldSel.format = ACL_FORMAT_DEFAULT;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"raw"))
    {
        fieldSel.format = ACL_FORMAT_RAW;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"llc"))
    {
        fieldSel.format = ACL_FORMAT_LLC;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"pppoe"))
    {
        fieldSel.format = ACL_FORMAT_PPPOE;
    }
	else if(!osal_strcmp(TOKEN_STR(5),"arp"))
    {
        fieldSel.format = ACL_FORMAT_ARP;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"ipv4-header"))
    {
        fieldSel.format = ACL_FORMAT_IPV4;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"ipv6-header"))
    {
        fieldSel.format = ACL_FORMAT_IPV6;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"ip-payload"))
    {
        fieldSel.format = ACL_FORMAT_IPPAYLOAD;
    }
    else if(!osal_strcmp(TOKEN_STR(5),"l4-payload"))
    {
        fieldSel.format = ACL_FORMAT_L4PAYLOAD;
    }
	else
		return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_acl_fieldSelect_set(&fieldSel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_field_selector_set_index_index_format_default_raw_llc_arp_ipv4_header_ipv6_header_ip_payload_l4_payload_offset_offset */

/*
 * field-selector get index <UINT:index>
 */
cparser_result_t
cparser_cmd_field_selector_get_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_acl_field_entry_t fieldSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fieldSel.index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_fieldSelect_get(&fieldSel), ret);

    diag_util_mprintf("Index Offset Mode\n");
    diag_util_mprintf("%-5d %-6d %s\n",fieldSel.index, fieldSel.offset,  diagStr_selectorMode[fieldSel.format]);

    return CPARSER_OK;
}    /* end of cparser_cmd_field_selector_get_index_index */


/*
 * acl get reason 
 */
cparser_result_t
cparser_cmd_acl_get_reason(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	diag_util_printf("Action      Hit Index\n");
	_diag_aclReasonShow();

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_reason */

/*
 * acl get reason ( all | cvlan | svlan | priority | policing | forward | extend | route )
 */
cparser_result_t
cparser_cmd_acl_get_reason_all_cvlan_svlan_priority_policing_forward_extend_route(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 val;
    uint32 index;
	rtk_acl_debug_reason_t aclReason;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
			diag_util_printf("Action      Hit Index\n");
			
            if('a' == TOKEN_CHAR(3,0))
            {
				_diag_aclReasonShow();
                
            }
            else if('c' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 0, APOLLOMP_ACL_HIT_INFOf, &val), ret);
          
                diag_util_printf("%-11s %-3s %d\n", "cvlan",(val&0x80)?"yes":"no",val&0x7F);
            }
            else if('s' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 1, APOLLOMP_ACL_HIT_INFOf, &val), ret);

                diag_util_printf("%-11s %-3s %d\n", "svlan",(val&0x80)?"yes":"no",val&0x7F);
            }
			else if('p' == TOKEN_CHAR(3,0))
			{
	            if('r' == TOKEN_CHAR(3,1))
	            {
	                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 2, APOLLOMP_ACL_HIT_INFOf, &val), ret);
       
                    diag_util_printf("%-11s %-3s %d\n", "priority",(val&0x80)?"yes":"no",val&0x7F);
	                
	            }
	            else if('o' == TOKEN_CHAR(3,1))
	            {
	                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 3, APOLLOMP_ACL_HIT_INFOf, &val), ret);
         
	                diag_util_printf("%-11s %-3s %d\n", "policing",(val&0x80)?"yes":"no",val&0x7F);

	            }
			}
            else if('f' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 5, APOLLOMP_ACL_HIT_INFOf, &val), ret);

                diag_util_printf("%-11s %-3s %d\n", "forward",(val&0x80)?"yes":"no",val&0x7F);
                
            }
            else if('e' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 4, APOLLOMP_ACL_HIT_INFOf, &val), ret);

                diag_util_printf("%-11s %-3s %d\n", "extend",(val&0x80)?"yes":"no",val&0x7F);
            }
			else
				diag_util_printf("error!!\n");
			
            break;
#endif

#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
			diag_util_printf("Action      Hit Index\n");
			
            if('a' == TOKEN_CHAR(3,0))
            {
				_diag_aclReasonShow();
                
            }
            else if('c' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 0, RTL9601B_ACL_HIT_INFOf, &val), ret);

                diag_util_printf("%-11s %-3s %d\n", "1p-remark",(val&0x40)?"yes":"no",val&0x3F);
            }
            else if('s' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 1, RTL9601B_ACL_HIT_INFOf, &val), ret);
          
                diag_util_printf("%-11s %-3s %d\n", "dscp-remark",(val&0x40)?"yes":"no",val&0x3F);
            }
			else if('p' == TOKEN_CHAR(3,0))
			{
	            if('r' == TOKEN_CHAR(3,1))
	            {
	                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 2, RTL9601B_ACL_HIT_INFOf, &val), ret);
          
	                diag_util_printf("%-11s %-3s %d\n", "priority",(val&0x40)?"yes":"no",val&0x3F);
	                
	            }
	            else if('o' == TOKEN_CHAR(3,1))
	            {
	                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 3, RTL9601B_ACL_HIT_INFOf, &val), ret);

	                diag_util_printf("%-11s %-3s %d\n", "policing",(val&0x40)?"yes":"no",val&0x3F);

	            }
			}
            else if('f' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 5, RTL9601B_ACL_HIT_INFOf, &val), ret);

                diag_util_printf("%-11s %-3s %d\n", "forward",(val&0x40)?"yes":"no",val&0x3F);
                
            }
            else if('e' == TOKEN_CHAR(3,0))
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9601B_STAT_ACL_REASONr, REG_ARRAY_INDEX_NONE, 4, RTL9601B_ACL_HIT_INFOf, &val), ret);
        
                diag_util_printf("%-11s %-3s %d\n", "inter-cf",(val&0x40)?"yes":"no",val&0x3F);
            }
			else
				diag_util_printf("error!!\n");
			
            break;
#endif
		default:

			diag_util_printf("Action      Hit Index\n");
			
			if('a' == TOKEN_CHAR(3,0))
			{
				_diag_aclReasonShow();				
			}
			else
			{			
				DIAG_UTIL_ERR_CHK(rtk_acl_dbgHitReason_get(&aclReason), ret);
				if('c' == TOKEN_CHAR(3,0))
				{
					diag_util_printf("%-11s %-3s %d\n", "cvlan",aclReason.hitAct[ACL_IGR_CVLAN_ACT]?"yes":"no",aclReason.index[ACL_IGR_CVLAN_ACT]);
				}
				else if('s' == TOKEN_CHAR(3,0))
				{
					diag_util_printf("%-11s %-3s %d\n", "svlan",aclReason.hitAct[ACL_IGR_SVLAN_ACT]?"yes":"no",aclReason.index[ACL_IGR_SVLAN_ACT]);
				}
				else if('p' == TOKEN_CHAR(3,0))
				{
					if('r' == TOKEN_CHAR(3,1))
					{
						diag_util_printf("%-11s %-3s %d\n", "priority",aclReason.hitAct[ACL_IGR_PRI_ACT]?"yes":"no",aclReason.index[ACL_IGR_PRI_ACT]);
						
					}
					else if('o' == TOKEN_CHAR(3,1))
					{
						diag_util_printf("%-11s %-3s %d\n", "policing",aclReason.hitAct[ACL_IGR_LOG_ACT]?"yes":"no",aclReason.index[ACL_IGR_LOG_ACT]);

					}
				}
				else if('f' == TOKEN_CHAR(3,0))
				{
					diag_util_printf("%-11s %-3s %d\n", "forward",aclReason.hitAct[ACL_IGR_FORWARD_ACT]?"yes":"no",aclReason.index[ACL_IGR_FORWARD_ACT]);
				}
				else if('e' == TOKEN_CHAR(3,0))
				{
					diag_util_printf("%-11s %-3s %d\n", "extend",aclReason.hitAct[ACL_IGR_INTR_ACT]?"yes":"no",aclReason.index[ACL_IGR_INTR_ACT]);
				}
				else if('r' == TOKEN_CHAR(3,0))
				{
					diag_util_printf("%-11s %-3s %d\n", "route",aclReason.hitAct[ACL_IGR_ROUTE_ACT]?"yes":"no",aclReason.index[ACL_IGR_ROUTE_ACT]);
				}
				else
					diag_util_printf("error!!\n");
			}
			break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_reason_all_cvlan_svlan_priority_policing_forward_extend */

/*
 * acl set meter-mode ( policing-only | bandwidth-occupy )
 */
cparser_result_t
cparser_cmd_acl_set_meter_mode_policing_only_bandwidth_occupy(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int mode;

    DIAG_UTIL_PARAM_CHK();

    if ('p' == TOKEN_CHAR(3,0))
    {
        mode = 0;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        mode = 1;
    }
	else
		return CPARSER_NOT_OK;
	
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_APOLLOMP)
		case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_ACL_POLICING_MODEr, APOLLOMP_POLICING_CNTING_MODEf, &mode), ret);
 
		
			break;
#endif
		default:
			diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;
	}
		
    return CPARSER_OK;
}    /* end of cparser_cmd_acl_set_meter_mode_policing_only_bandwidth_occupy */

/*
 * acl get meter-mode 
 */
cparser_result_t
cparser_cmd_acl_get_meter_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_APOLLOMP)
		case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_ACL_POLICING_MODEr, APOLLOMP_POLICING_CNTING_MODEf, &mode), ret);
            diag_util_mprintf("Meter mode: %s\n",mode?"occupy bandwidth":"policing only");

			break;
#endif
		default:
			diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;
	}


    return CPARSER_OK;
}    /* end of cparser_cmd_acl_get_meter_mode */

/*
 * acl get entry valid
 */
cparser_result_t
cparser_cmd_acl_get_entry_valid(
    cparser_context_t *context)
{
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_template_t aclTemplate;
    rtk_acl_igr_rule_mode_t mode;
	uint32 index = 0;
	int32 ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
	aclRule.index = index;
	while (RT_ERR_OK == (rtk_acl_igrRuleEntry_get(&aclRule)))
	{
		if (ENABLED == aclRule.valid)
		{
			diag_util_printf("****************************************************************\n");
			diag_util_printf("Index: %u\n", aclRule.index);
			_diag_aclActShow(&aclRule);
			aclTemplate.index = aclRule.templateIdx;
			DIAG_UTIL_ERR_CHK(rtk_acl_template_get(&aclTemplate), ret);

			_diag_aclTemplateShow(&aclTemplate);

			switch(DIAG_UTIL_CHIP_TYPE)
			{
#if defined(CONFIG_SDK_APOLL) || defined(CONFIG_SDK_APOLLOMP)
				case APOLLO_CHIP_ID:
				case APOLLOMP_CHIP_ID:
					DIAG_UTIL_ERR_CHK(rtk_acl_igrRuleMode_get(&mode), ret);
					break;
#endif           
				default:
					mode = ACL_IGR_RULE_MODE_0;
					break;
			}

			_diag_aclRuleShow(&aclRule, DIAG_ACL_RULE_DISPLAY_MODE_GET, mode);
			
		}
		osal_memset(&aclRule, 0x0, sizeof(rtk_acl_ingress_entry_t));
		index++;
		aclRule.index = index;
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_classf_get_entry_valid */

/*
 * acl dump debug counter
 */
cparser_result_t
cparser_cmd_acl_dump_debug_counter(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    rtk_acl_dbgCnt_t dbgCnt;
    memset(&dbgCnt,0x0,sizeof(rtk_acl_dbgCnt_t));

	DIAG_UTIL_ERR_CHK(rtk_acl_dbgInfo_get(&dbgCnt), ret);

    diag_util_printf("[interrupt status]\n"); 
    diag_util_printf("isr enter number: %d\n",dbgCnt.actionIntCnt); 
	
   	return CPARSER_OK;
}    /* end of cparser_cmd_acl_dump_debug_counter */


