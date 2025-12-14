#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_acl_test_case.h>
#include <rtk/acl.h>
#include <rtk/vlan.h>
#include <rtk/svlan.h>

#include <common/unittest_util.h>
#include <dal/apollomp/dal_apollomp_acl.h>
//#include <dal/apollo/dal_apollo_acl.h>
#include <common/rt_type.h>
#include <rtk/port.h>

int32 dal_acl_test(uint32 caseNo)
{  
    uint32 dataR;
    uint32 dataW;
    rtk_enable_t enableR;
    rtk_enable_t enableW;
    rtk_port_t port;
    rtk_acl_template_t tempR;
    rtk_acl_template_t tempW;
    rtk_acl_field_entry_t selR;
    rtk_acl_field_entry_t selW;
    rtk_acl_igr_rule_mode_t modeR;
    rtk_acl_igr_rule_mode_t modeW;
    uint32 j;
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)	
	const rtk_acl_field_type_t fieldType[8][8] = {
		{ACL_FIELD_DMAC0,			ACL_FIELD_DMAC1,			ACL_FIELD_DMAC2,			ACL_FIELD_STAG, 			ACL_FIELD_SMAC0,			ACL_FIELD_SMAC1,			ACL_FIELD_SMAC2,			ACL_FIELD_ETHERTYPE},
		{ACL_FIELD_CTAG,			ACL_FIELD_IPV4_SIP0,		ACL_FIELD_IPV4_SIP1,		ACL_FIELD_VID_RANGE,		ACL_FIELD_IP_RANGE, 		ACL_FIELD_PORT_RANGE,		ACL_FIELD_IPV4_DIP0,		ACL_FIELD_IPV4_DIP1},
		{ACL_FIELD_USER_DEFINED00,	ACL_FIELD_USER_DEFINED01,	ACL_FIELD_USER_DEFINED02,	ACL_FIELD_USER_DEFINED03,	ACL_FIELD_USER_DEFINED04,	ACL_FIELD_USER_DEFINED05,	ACL_FIELD_USER_DEFINED06,	ACL_FIELD_USER_DEFINED07},
		{ACL_FIELD_IPV4_SIP0_INNER,	ACL_FIELD_IPV4_SIP1_INNER,	ACL_FIELD_IPV4_DIP0_INNER,	ACL_FIELD_IPV4_DIP1_INNER,	ACL_FIELD_IPV4_PROTOCOL_INNER,	ACL_FIELD_FRAME_TYPE_TAGS,	ACL_FIELD_UNUSED,	ACL_FIELD_UNUSED},
		{ACL_FIELD_IPV6_DIP0,		ACL_FIELD_IPV6_DIP1,		ACL_FIELD_IPV6_DIP2,		ACL_FIELD_IPV6_DIP3,		ACL_FIELD_IPV6_DIP4,		ACL_FIELD_IPV6_DIP5,		ACL_FIELD_IPV6_DIP6,		ACL_FIELD_IPV6_DIP7},
		{ACL_FIELD_IPV6_SIP0,		ACL_FIELD_IPV6_SIP1,		ACL_FIELD_IPV6_SIP2,		ACL_FIELD_IPV6_SIP3,		ACL_FIELD_IPV6_SIP4,		ACL_FIELD_IPV6_SIP5,		ACL_FIELD_IPV6_SIP6,		ACL_FIELD_IPV6_SIP7},
		{ACL_FIELD_GEMPORT, 		ACL_FIELD_IPV4_PROTOCOL,	ACL_FIELD_IPV6_NEXT_HEADER, ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED},
		{ACL_FIELD_EXT_PORTMASK,	ACL_FIELD_USER_VALID,		ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED},
	};

#else    
    const rtk_acl_field_type_t fieldType[8][8] = {
        {ACL_FIELD_DMAC0,     		ACL_FIELD_DMAC1,      		ACL_FIELD_DMAC2,    		ACL_FIELD_STAG,     		ACL_FIELD_SMAC0,    		ACL_FIELD_SMAC1,        	ACL_FIELD_SMAC2,    		ACL_FIELD_ETHERTYPE},
        {ACL_FIELD_CTAG,      		ACL_FIELD_IPV4_SIP0,  		ACL_FIELD_IPV4_SIP1,		ACL_FIELD_VID_RANGE,		ACL_FIELD_IP_RANGE, 		ACL_FIELD_PORT_RANGE,   	ACL_FIELD_IPV4_DIP0,		ACL_FIELD_IPV4_DIP1},
        {ACL_FIELD_USER_DEFINED00,  ACL_FIELD_USER_DEFINED01, 	ACL_FIELD_USER_DEFINED02,	ACL_FIELD_USER_DEFINED03,	ACL_FIELD_USER_DEFINED04,	ACL_FIELD_USER_DEFINED05,	ACL_FIELD_USER_DEFINED06,	ACL_FIELD_USER_DEFINED07},
        {ACL_FIELD_USER_DEFINED08,  ACL_FIELD_USER_DEFINED09,	ACL_FIELD_USER_DEFINED10,	ACL_FIELD_USER_DEFINED11,	ACL_FIELD_USER_DEFINED12,	ACL_FIELD_USER_DEFINED13,	ACL_FIELD_USER_DEFINED14,	ACL_FIELD_USER_DEFINED15},
        {ACL_FIELD_IPV6_DIP0,       ACL_FIELD_IPV6_DIP1,		ACL_FIELD_IPV6_DIP2,		ACL_FIELD_IPV6_DIP3,		ACL_FIELD_IPV6_DIP4,		ACL_FIELD_IPV6_DIP5,		ACL_FIELD_IPV6_DIP6,		ACL_FIELD_IPV6_DIP7},
		{ACL_FIELD_IPV6_SIP0,		ACL_FIELD_IPV6_SIP1,		ACL_FIELD_IPV6_SIP2,		ACL_FIELD_IPV6_SIP3,		ACL_FIELD_IPV6_SIP4,		ACL_FIELD_IPV6_SIP5,		ACL_FIELD_IPV6_SIP6,		ACL_FIELD_IPV6_SIP7},
		{ACL_FIELD_GEMPORT,			ACL_FIELD_IPV4_PROTOCOL, 	ACL_FIELD_IPV6_NEXT_HEADER,	ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED},
		{ACL_FIELD_EXT_PORTMASK,	ACL_FIELD_USER_VALID, 		ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED,			ACL_FIELD_UNUSED},
    };
#endif


    if( rtk_acl_igrState_set(HAL_GET_MAX_PORT()+1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrState_set(HAL_GET_MIN_PORT()-1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrState_get(HAL_GET_MAX_PORT()+1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrState_get(HAL_GET_MIN_PORT()-1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrPermitState_set(HAL_GET_MAX_PORT()+1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrPermitState_set(HAL_GET_MIN_PORT()-1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrPermitState_get(HAL_GET_MAX_PORT()+1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrPermitState_get(HAL_GET_MIN_PORT()-1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrRuleMode_set(ACL_IGR_RULE_MODE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&tempW, 0, sizeof(rtk_acl_template_t));
    tempW.index = HAL_MAX_NUM_OF_ACL_TEMPLATE();
    if( rtk_acl_template_set(&tempW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    tempR.index = HAL_MAX_NUM_OF_ACL_TEMPLATE();
    if( rtk_acl_template_get(&tempR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    
    osal_memset(&selW, 0, sizeof(rtk_acl_field_entry_t));
    selW.index = HAL_MAX_NUM_OF_FIELD_SELECTOR();
    if( rtk_acl_fieldSelect_set(&selW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    selR.index = HAL_MAX_NUM_OF_FIELD_SELECTOR();
    if( rtk_acl_fieldSelect_get(&selR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
#if defined(CONFIG_SDK_RTL9602C)	    
    for(tempW.index = 0; tempW.index < (HAL_MAX_NUM_OF_ACL_TEMPLATE()-1); tempW.index ++)
#else
	for(tempW.index = 0; tempW.index < HAL_MAX_NUM_OF_ACL_TEMPLATE(); tempW.index ++)
#endif	
    {
        tempR.index = tempW.index;
        for(j = 0; j < 8; j ++)
        {
			tempW.fieldType[j] = fieldType[tempW.index][j];
        }
		
        if( rtk_acl_template_set(&tempW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if( rtk_acl_template_get(&tempR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
                    
        for(j = 0; j < 8; j ++)
        {
            if(tempR.fieldType[j] != tempW.fieldType[j])
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }
    }

    for(selW.index = 0; selW.index < HAL_MAX_NUM_OF_FIELD_SELECTOR(); selW.index ++)
    {
        selR.index = selW.index;

        for(selW.format = ACL_FORMAT_DEFAULT; selW.format <= ACL_FORMAT_IPPAYLOAD_INNER; selW.format ++)
        {
			if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID || 
                UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
			{
				if(selW.format == ACL_FORMAT_DEFAULT)
					continue;
			}
			if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
				UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
				UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID)
			{
				if(selW.format == ACL_FORMAT_PPPOE)
					continue;
			}
			if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID || 
				UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
				UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
			{
				if(selW.format == ACL_FORMAT_LLC)
					continue;
			}
			if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
                UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
			{
				if(selW.format == ACL_FORMAT_ARP || 
					selW.format == ACL_FORMAT_L4PAYLOAD)
					continue;
			}
			if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
				UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
				UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID ||
				UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
			{
				if(selW.format == ACL_FORMAT_IPV4_INNER)
					continue;
				if(selW.format == ACL_FORMAT_IPV6_INNER)
					continue;
				if(selW.format == ACL_FORMAT_IPPAYLOAD_INNER)
					continue;
			}

		
            for(selW.offset = 0; selW.offset <= 0xFF; selW.offset += 0x6F)
            {
                if( rtk_acl_fieldSelect_set(&selW) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                
                if( rtk_acl_fieldSelect_get(&selR) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
                
                if(selW.offset != selR.offset || 
                    selW.format != selR.format)
                {
                    osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                    return RT_ERR_FAILED;       
                }
            }
        }
    }


    HAL_SCAN_ALL_PORT(port)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
            if( rtk_acl_igrState_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if( rtk_acl_igrState_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_acl_igrPermitState_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if( rtk_acl_igrPermitState_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

        }    
    }   
    
	if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
	{
	    for(modeW = ACL_IGR_RULE_MODE_0; modeW < ACL_IGR_RULE_MODE_END; modeW ++)
	    {
	        if( rtk_acl_igrRuleMode_set(modeW) != RT_ERR_OK)
	        {
	            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	            return RT_ERR_FAILED;       
	        }

	        if( rtk_acl_igrRuleMode_get(&modeR) != RT_ERR_OK)
	        {
	            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	            return RT_ERR_FAILED;       
	        }
	        
	        if(modeW != modeR)
	        {
	            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	            return RT_ERR_FAILED;       
	        }
	    }
	}
	
    if( rtk_acl_template_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_acl_fieldSelect_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_acl_igrState_get(port, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }            


    if( rtk_acl_igrPermitState_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
	{
		if( rtk_acl_igrRuleMode_get(NULL) == RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
	}
    
    return RT_ERR_OK;
}    




int32 dal_acl_test_ruleEntry(uint32 caseNo)
{  
	rtk_acl_field_t aclField;
	rtk_acl_field_t aclField2;
	rtk_acl_field_t aclField3;
	rtk_acl_field_t aclField4;
	rtk_acl_field_t aclField5;
	rtk_acl_field_t aclField6;
	rtk_acl_field_t aclField7;
	rtk_acl_field_t aclField8;
    rtk_acl_ingress_entry_t ruleR;
    rtk_acl_ingress_entry_t ruleW;
	uint32 index;
	uint32 index2;

	/*initial for following test*/
    if(rtk_acl_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(rtk_vlan_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if(rtk_svlan_init() != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
    {	
        if(rtk_acl_igrRuleMode_set(ACL_IGR_RULE_MODE_0) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }
    
    osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    ruleW.index = HAL_MAX_NUM_OF_ACL_RULE_ENTRY() + 1;
    if( rtk_acl_igrRuleEntry_add(&ruleW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    ruleR.index = HAL_MAX_NUM_OF_ACL_RULE_ENTRY() + 1;
    if( rtk_acl_igrRuleEntry_get(&ruleR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	/*NULL check*/
    if( rtk_acl_igrRuleEntry_add(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_igrRuleEntry_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
    {	

    	if(rtk_acl_igrRuleMode_set(ACL_IGR_RULE_MODE_1) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        } 
    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_DMAC;
    	aclField.fieldUnion.mac.value.octet[0] = 0x12;
    	aclField.fieldUnion.mac.mask.octet[0] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[1] = 0x34;
    	aclField.fieldUnion.mac.mask.octet[1] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[2] = 0x56;
    	aclField.fieldUnion.mac.mask.octet[2] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[3] = 0x78;
    	aclField.fieldUnion.mac.mask.octet[3] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[4] = 0x9a;
    	aclField.fieldUnion.mac.mask.octet[4] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[5] = 0xbc;
    	aclField.fieldUnion.mac.mask.octet[5] = 0xFF;
    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x0;
    	
    	for(ruleW.index = 64; ruleW.index < 79; ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(0x1234 != ruleR.readField.fieldRaw[2].value ||
    			0x5678 != ruleR.readField.fieldRaw[1].value|| 
    			0x9abc != ruleR.readField.fieldRaw[0].value)
    	    {
     	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
                              ruleR.readField.fieldRaw[2].value,
                              ruleR.readField.fieldRaw[1].value,
    	                      ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}

    		
    	}
    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_IPV4_SIP;
    	aclField.fieldUnion.ip.value = 0xFEDCBA98;
    	aclField.fieldUnion.ip.mask = 0xFFFFFFFF;

    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    	
    	aclField2.fieldType = ACL_FIELD_CTAG;
    	aclField2.fieldUnion.l2tag.pri.value = 0x3;
    	aclField2.fieldUnion.l2tag.pri.mask = 0x7;
    	aclField2.fieldUnion.l2tag.cfi_dei.value = 1;
    	aclField2.fieldUnion.l2tag.cfi_dei.mask = 1;
    	aclField2.fieldUnion.l2tag.vid.value = 0x123;
    	aclField2.fieldUnion.l2tag.vid.mask = 0xFFF;
    	aclField2.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }


        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x1;
    	
    	for(ruleW.index = 80; ruleW.index < 96; ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(0xFEDC != ruleR.readField.fieldRaw[2].value ||
    			0xBA98 != ruleR.readField.fieldRaw[1].value ||
    			0x7123 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
    	                          ruleR.readField.fieldRaw[2].value,
    	                          ruleR.readField.fieldRaw[1].value,
    	                          ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}		
    	}

    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_USER_DEFINED00;
    	aclField.fieldUnion.data.value = 0x1234;
    	aclField.fieldUnion.data.mask = 0xFFFF;

    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    	
    	aclField2.fieldType = ACL_FIELD_USER_DEFINED01;
    	aclField2.fieldUnion.data.value = 0x5678;
    	aclField2.fieldUnion.data.mask = 0xFFFF;

    	aclField2.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
    	
    	aclField3.fieldType = ACL_FIELD_USER_DEFINED02;
    	aclField3.fieldUnion.data.value = 0x9abc;
    	aclField3.fieldUnion.data.mask = 0xFFFF;

    	aclField3.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x2;
    	
    	for(ruleW.index = 96; ruleW.index < 112; ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(0x9abc != ruleR.readField.fieldRaw[2].value ||
    			0x5678 != ruleR.readField.fieldRaw[1].value ||
    			0x1234 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
    	                          ruleR.readField.fieldRaw[2].value,
    	                          ruleR.readField.fieldRaw[1].value,
    	                          ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}		
    	}

    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField.fieldUnion.pattern.data.value = 0x4321;
    	aclField.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField.fieldUnion.pattern.fieldIdx = 0;
    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    	
    	aclField2.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField2.fieldUnion.pattern.data.value = 0x8765;
    	aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField2.fieldUnion.pattern.fieldIdx = 1;
    	aclField2.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
    	
    	aclField3.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField3.fieldUnion.pattern.data.value = 0xcba9;
    	aclField3.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField3.fieldUnion.pattern.fieldIdx = 2;
    	aclField3.next = NULL;

    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x3;
    	
    	for(ruleW.index = 112; ruleW.index < HAL_MAX_NUM_OF_ACL_ACTION(); ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(0xcba9 != ruleR.readField.fieldRaw[2].value ||
    			0x8765 != ruleR.readField.fieldRaw[1].value ||
    			0x4321 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
    	                          ruleR.readField.fieldRaw[2].value,
    	                          ruleR.readField.fieldRaw[1].value,
    	                          ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}		
    	}

    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_SMAC;
    	aclField.fieldUnion.mac.value.octet[0] = 0x11;
    	aclField.fieldUnion.mac.mask.octet[0] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[1] = 0x22;
    	aclField.fieldUnion.mac.mask.octet[1] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[2] = 0x33;
    	aclField.fieldUnion.mac.mask.octet[2] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[3] = 0x44;
    	aclField.fieldUnion.mac.mask.octet[3] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[4] = 0x55;
    	aclField.fieldUnion.mac.mask.octet[4] = 0xFF;
    	aclField.fieldUnion.mac.value.octet[5] = 0x66;
    	aclField.fieldUnion.mac.mask.octet[5] = 0xFF;
    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x0;	
    	for(ruleW.index = 0; ruleW.index < 16; ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}
    		if(0x1122 != ruleR.readField.fieldRaw[2].value ||
    			0x3344 != ruleR.readField.fieldRaw[1].value|| 
    			0x5566 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
    	                          ruleR.readField.fieldRaw[2].value,
    	                          ruleR.readField.fieldRaw[1].value,
    	                          ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}
    	}

    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_IPV4_DIP;
    	aclField.fieldUnion.ip.value = 0x76543210;
    	aclField.fieldUnion.ip.mask = 0xFFFFFFFF;

    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
    	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    	
    	aclField2.fieldType = ACL_FIELD_PORT_RANGE;
    	aclField2.fieldUnion.data.value = 0x1234;
    	aclField2.fieldUnion.data.mask = 0xFFFF;

    	aclField2.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
    	
    	aclField3.fieldType = ACL_FIELD_IP_RANGE;
    	aclField3.fieldUnion.data.value = 0x56;
    	aclField3.fieldUnion.data.mask = 0xFF;

    	aclField3.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }


        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x1;
    	
    	for(ruleW.index = 16; ruleW.index < 32; ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(0x7654 != ruleR.readField.fieldRaw[3].value ||
    			0x3210 != ruleR.readField.fieldRaw[2].value ||
    			0x1234 != ruleR.readField.fieldRaw[1].value ||
    			0x56 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
    	                          ruleR.readField.fieldRaw[2].value,
    	                          ruleR.readField.fieldRaw[1].value,
    	                          ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}	
    	}

    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_USER_DEFINED04;
    	aclField.fieldUnion.data.value = 0x1122;
    	aclField.fieldUnion.data.mask = 0xFFFF;

    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    	
    	aclField2.fieldType = ACL_FIELD_USER_DEFINED05;
    	aclField2.fieldUnion.data.value = 0x3344;
    	aclField2.fieldUnion.data.mask = 0xFFFF;

    	aclField2.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
    	
    	aclField3.fieldType = ACL_FIELD_USER_DEFINED06;
    	aclField3.fieldUnion.data.value = 0x5566;
    	aclField3.fieldUnion.data.mask = 0xFFFF;

    	aclField3.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField4, 0, sizeof(rtk_acl_field_t));
    	
    	aclField4.fieldType = ACL_FIELD_USER_DEFINED07;
    	aclField4.fieldUnion.data.value = 0x7788;
    	aclField4.fieldUnion.data.mask = 0xFFFF;

    	aclField4.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField4) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x2;
    	
    	for(ruleW.index = 32; ruleW.index < 48; ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(	0x7788 != ruleR.readField.fieldRaw[3].value ||
    			0x5566 != ruleR.readField.fieldRaw[2].value ||
    			0x3344 != ruleR.readField.fieldRaw[1].value ||
    			0x1122 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d\n",__FUNCTION__,__LINE__,
    	                          ruleR.readField.fieldRaw[2].value,
    	                          ruleR.readField.fieldRaw[1].value,
    	                          ruleR.readField.fieldRaw[0].value);
    	        return RT_ERR_FAILED;       
    		}		
    	}

    	/*add set testing*/
    	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
    	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    	
    	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField.fieldUnion.pattern.data.value = 0x1111;
    	aclField.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField.fieldUnion.pattern.fieldIdx = 0;
    	aclField.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    	
    	aclField2.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField2.fieldUnion.pattern.data.value = 0x2222;
    	aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField2.fieldUnion.pattern.fieldIdx = 1;
    	aclField2.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
    	
    	aclField3.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField3.fieldUnion.pattern.data.value = 0x3333;
    	aclField3.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField3.fieldUnion.pattern.fieldIdx = 2;
    	aclField3.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

    	osal_memset(&aclField4, 0, sizeof(rtk_acl_field_t));
    	
    	aclField4.fieldType = ACL_FIELD_PATTERN_MATCH;
    	aclField4.fieldUnion.pattern.data.value = 0x4444;
    	aclField4.fieldUnion.pattern.data.mask = 0xFFFF;
    	aclField4.fieldUnion.pattern.fieldIdx = 3;
    	aclField4.next = NULL;
    	
    	if( rtk_acl_igrRuleField_add(&ruleW, &aclField4) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        ruleW.valid = ENABLED;
        ruleW.templateIdx = 0x3;
    	
    	for(ruleW.index = 48; ruleW.index < HAL_MAX_NUM_OF_ACL_RULE_ENTRY(); ruleW.index +=4)
    	{
    		ruleW.activePorts.bits[0] = ruleW.index;
    		
    		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }
    		ruleR.index =  ruleW.index;
    	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    	    }		
    		if(ruleW.valid != ruleR.valid ||
    			ruleW.templateIdx != ruleR.templateIdx || 
    			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
    	    {
    	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
    	        return RT_ERR_FAILED;       
    		}

    		if(	0x4444 != ruleR.readField.fieldRaw[3].value ||
    			0x3333 != ruleR.readField.fieldRaw[2].value ||
    			0x2222 != ruleR.readField.fieldRaw[1].value ||
    			0x1111 != ruleR.readField.fieldRaw[0].value)
    	    {
    	        osal_printf("\n %s %d  %d %d %d %d\n",__FUNCTION__,__LINE__
    	                                    ,ruleR.readField.fieldRaw[0].value
    	                                    ,ruleR.readField.fieldRaw[1].value
    	                                    ,ruleR.readField.fieldRaw[2].value
    	                                    ,ruleR.readField.fieldRaw[3].value);
    	        return RT_ERR_FAILED;       
    		}		
    	}

        if(rtk_acl_igrRuleMode_set(ACL_IGR_RULE_MODE_0) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }
    
	/*add set testing*/
	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
	
	aclField.fieldType = ACL_FIELD_DMAC;
	aclField.fieldUnion.mac.value.octet[0] = 0x12;
	aclField.fieldUnion.mac.mask.octet[0] = 0xFF;
	aclField.fieldUnion.mac.value.octet[1] = 0x34;
	aclField.fieldUnion.mac.mask.octet[1] = 0xFF;
	aclField.fieldUnion.mac.value.octet[2] = 0x56;
	aclField.fieldUnion.mac.mask.octet[2] = 0xFF;
	aclField.fieldUnion.mac.value.octet[3] = 0x78;
	aclField.fieldUnion.mac.mask.octet[3] = 0xFF;
	aclField.fieldUnion.mac.value.octet[4] = 0x9a;
	aclField.fieldUnion.mac.mask.octet[4] = 0xFF;
	aclField.fieldUnion.mac.value.octet[5] = 0xbc;
	aclField.fieldUnion.mac.mask.octet[5] = 0xFF;
	aclField.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
	
	aclField2.fieldType = ACL_FIELD_SMAC;
	aclField2.fieldUnion.mac.value.octet[0] = 0x11;
	aclField2.fieldUnion.mac.mask.octet[0] = 0xFF;
	aclField2.fieldUnion.mac.value.octet[1] = 0x22;
	aclField2.fieldUnion.mac.mask.octet[1] = 0xFF;
	aclField2.fieldUnion.mac.value.octet[2] = 0x33;
	aclField2.fieldUnion.mac.mask.octet[2] = 0xFF;
	aclField2.fieldUnion.mac.value.octet[3] = 0x44;
	aclField2.fieldUnion.mac.mask.octet[3] = 0xFF;
	aclField2.fieldUnion.mac.value.octet[4] = 0x55;
	aclField2.fieldUnion.mac.mask.octet[4] = 0xFF;
	aclField2.fieldUnion.mac.value.octet[5] = 0x66;
	aclField2.fieldUnion.mac.mask.octet[5] = 0xFF;
	aclField2.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    ruleW.valid = ENABLED;
    ruleW.templateIdx = 0x0;
	
#if defined(FPGA_DEFINED)
	for(ruleW.index = 0; ruleW.index < 8; ruleW.index ++)
#else	
	for(ruleW.index = 0; ruleW.index < 16; ruleW.index +=4)
#endif
	{
	    HAL_GET_ALL_PORTMASK(ruleW.activePorts);
		
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }	
        #if 0
        osal_printf("\nindex:%d\nW:%d-%d-0x%2x\nR:%d-%d-0x%2x\n",
				ruleW.index,
				ruleW.valid,ruleW.templateIdx,ruleW.activePorts.bits[0],
                ruleR.valid,ruleR.templateIdx,ruleR.activePorts.bits[0]);
        #endif    
		if(ruleW.valid != ruleR.valid ||
			ruleW.templateIdx != ruleR.templateIdx || 
			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}
		
		if( 0x1122 != ruleR.readField.fieldRaw[6].value ||
			0x3344 != ruleR.readField.fieldRaw[5].value || 
			0x5566 != ruleR.readField.fieldRaw[4].value ||
			0x1234 != ruleR.readField.fieldRaw[2].value ||
			0x5678 != ruleR.readField.fieldRaw[1].value || 
			0x9abc != ruleR.readField.fieldRaw[0].value)
	    {
		   osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);	 
		   return RT_ERR_FAILED;       
		}
	}

	/*add set testing*/
	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
	
	aclField.fieldType = ACL_FIELD_IPV4_SIP;
	aclField.fieldUnion.ip.value = 0xFEDCBA98;
	aclField.fieldUnion.ip.mask = 0xFFFFFFFF;

	aclField.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
	
	aclField2.fieldType = ACL_FIELD_CTAG;
	aclField2.fieldUnion.l2tag.pri.value = 0x3;
	aclField2.fieldUnion.l2tag.pri.mask = 0x7;
	aclField2.fieldUnion.l2tag.cfi_dei.value = 1;
	aclField2.fieldUnion.l2tag.cfi_dei.mask = 1;
	aclField2.fieldUnion.l2tag.vid.value = 0x123;
	aclField2.fieldUnion.l2tag.vid.mask = 0xFFF;
	aclField2.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
	
	aclField3.fieldType = ACL_FIELD_IPV4_DIP;
	aclField3.fieldUnion.ip.value = 0x76543210;
	aclField3.fieldUnion.ip.mask = 0xFFFFFFFF;

	aclField3.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	
	osal_memset(&aclField4, 0, sizeof(rtk_acl_field_t));
	
	aclField4.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField4.fieldUnion.pattern.data.value = 0x3333;
	aclField4.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField4.fieldUnion.pattern.fieldIdx = 3;
	aclField4.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField4) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	
	osal_memset(&aclField5, 0, sizeof(rtk_acl_field_t));
	
	aclField5.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField5.fieldUnion.pattern.data.value = 0x4444;
	aclField5.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField5.fieldUnion.pattern.fieldIdx = 4;
	aclField5.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField5) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField6, 0, sizeof(rtk_acl_field_t));
	
	aclField6.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField6.fieldUnion.pattern.data.value = 0x5555;
	aclField6.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField6.fieldUnion.pattern.fieldIdx = 5;
	aclField6.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField6) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


#if defined(FPGA_DEFINED)

#else

    ruleW.valid = ENABLED;
    ruleW.templateIdx = 0x1;
	
	for(ruleW.index = 16; ruleW.index < 32; ruleW.index +=4)
	{
	    HAL_GET_ALL_PORTMASK(ruleW.activePorts);
		
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.valid != ruleR.valid ||
			ruleW.templateIdx != ruleR.templateIdx || 
			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}

		if(0x7654 != ruleR.readField.fieldRaw[7].value ||
			0x3210 != ruleR.readField.fieldRaw[6].value ||
			0x5555 != ruleR.readField.fieldRaw[5].value ||
			0x4444 != ruleR.readField.fieldRaw[4].value ||
			0x3333 != ruleR.readField.fieldRaw[3].value ||
			0xFEDC != ruleR.readField.fieldRaw[2].value ||
			0xBA98 != ruleR.readField.fieldRaw[1].value ||
			0x7123 != ruleR.readField.fieldRaw[0].value)
	    {
		   osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);	 
		   osal_printf("%4.4x %4.4x %4.4x %4.4x %4.4x %4.4x %4.4x %4.4x\n",
		   					ruleR.readField.fieldRaw[0].value,
		   					ruleR.readField.fieldRaw[1].value,
		   					ruleR.readField.fieldRaw[2].value,
		   					ruleR.readField.fieldRaw[3].value,
		   					ruleR.readField.fieldRaw[4].value,
		   					ruleR.readField.fieldRaw[5].value,
		   					ruleR.readField.fieldRaw[6].value,
		   					ruleR.readField.fieldRaw[7].value);
	        return RT_ERR_FAILED;       
		}		
	}

	/*add set testing*/
	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
	
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.data.value = 0x1111;
	aclField.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField.fieldUnion.pattern.fieldIdx = 0;
	aclField.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
	
	aclField2.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField2.fieldUnion.pattern.data.value = 0x2222;
	aclField2.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField2.fieldUnion.pattern.fieldIdx = 1;
	aclField2.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField2) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField3, 0, sizeof(rtk_acl_field_t));
	
	aclField3.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField3.fieldUnion.pattern.data.value = 0x3333;
	aclField3.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField3.fieldUnion.pattern.fieldIdx = 2;
	aclField3.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField3) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField4, 0, sizeof(rtk_acl_field_t));
	
	aclField4.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField4.fieldUnion.pattern.data.value = 0x4444;
	aclField4.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField4.fieldUnion.pattern.fieldIdx = 3;
	aclField4.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField4) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField5, 0, sizeof(rtk_acl_field_t));
	
	aclField5.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField5.fieldUnion.pattern.data.value = 0x5555;
	aclField5.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField5.fieldUnion.pattern.fieldIdx = 4;
	aclField5.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField5) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField6, 0, sizeof(rtk_acl_field_t));
	
	aclField6.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField6.fieldUnion.pattern.data.value = 0x6666;
	aclField6.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField6.fieldUnion.pattern.fieldIdx = 5;
	aclField6.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField6) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField7, 0, sizeof(rtk_acl_field_t));
	
	aclField7.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField7.fieldUnion.pattern.data.value = 0x7777;
	aclField7.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField7.fieldUnion.pattern.fieldIdx = 6;
	aclField7.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField7) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	osal_memset(&aclField8, 0, sizeof(rtk_acl_field_t));
	
	aclField8.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField8.fieldUnion.pattern.data.value = 0x8888;
	aclField8.fieldUnion.pattern.data.mask = 0xFFFF;
	aclField8.fieldUnion.pattern.fieldIdx = 7;
	aclField8.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField8) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	
    ruleW.valid = ENABLED;
    ruleW.templateIdx = 0x2;
	
	for(ruleW.index = 32; ruleW.index < HAL_MAX_NUM_OF_ACL_RULE_ENTRY(); ruleW.index +=4)
	{
	    HAL_GET_ALL_PORTMASK(ruleW.activePorts);
		
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.valid != ruleR.valid ||
			ruleW.templateIdx != ruleR.templateIdx || 
			ruleW.activePorts.bits[0] != ruleR.activePorts.bits[0])
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}

		if(0x8888 != ruleR.readField.fieldRaw[7].value ||
			0x7777 != ruleR.readField.fieldRaw[6].value ||
			0x6666 != ruleR.readField.fieldRaw[5].value ||
			0x5555 != ruleR.readField.fieldRaw[4].value ||
			0x4444 != ruleR.readField.fieldRaw[3].value ||
			0x3333 != ruleR.readField.fieldRaw[2].value ||
			0x2222 != ruleR.readField.fieldRaw[1].value ||
			0x1111 != ruleR.readField.fieldRaw[0].value)
	    {
		   osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);	 
		   osal_printf("%4.4x %4.4x %4.4x %4.4x %4.4x %4.4x %4.4x %4.4x\n",
		   					ruleR.readField.fieldRaw[0].value,
		   					ruleR.readField.fieldRaw[1].value,
		   					ruleR.readField.fieldRaw[2].value,
		   					ruleR.readField.fieldRaw[3].value,
		   					ruleR.readField.fieldRaw[4].value,
		   					ruleR.readField.fieldRaw[5].value,
		   					ruleR.readField.fieldRaw[6].value,
		   					ruleR.readField.fieldRaw[7].value);
	        return RT_ERR_FAILED;       
		}		
	}
#endif


	/*add set testing*/
	osal_memset(&ruleW, 0, sizeof(rtk_acl_ingress_entry_t));
	osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
	
	aclField.fieldType = ACL_FIELD_DMAC;
	aclField.fieldUnion.mac.value.octet[0] = 0x01;
	aclField.fieldUnion.mac.mask.octet[0] = 0xFF;
	aclField.fieldUnion.mac.value.octet[1] = 0x23;
	aclField.fieldUnion.mac.mask.octet[1] = 0xFF;
	aclField.fieldUnion.mac.value.octet[2] = 0x45;
	aclField.fieldUnion.mac.mask.octet[2] = 0xFF;
	aclField.fieldUnion.mac.value.octet[3] = 0x67;
	aclField.fieldUnion.mac.mask.octet[3] = 0xFF;
	aclField.fieldUnion.mac.value.octet[4] = 0x89;
	aclField.fieldUnion.mac.mask.octet[4] = 0xFF;
	aclField.fieldUnion.mac.value.octet[5] = 0xab;
	aclField.fieldUnion.mac.mask.octet[5] = 0xFF;
	aclField.next = NULL;
	
	if( rtk_acl_igrRuleField_add(&ruleW, &aclField) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	ruleW.index = 0;
    HAL_GET_ALL_PORTMASK(ruleW.activePorts);
    ruleW.valid = ENABLED;
    ruleW.templateIdx = 0x0;

    /*trap packets*/
    ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
    ruleW.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
    //ruleW.act.forwardAct.portMask.bits[0] = 0;
	
    /*care tags testing*/
	for(index = 0; index < ACL_CARE_TAG_END; index ++)
	{
		if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
			UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
			UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
		{
			if(index == ACL_CARE_TAG_PPPOE)
				continue;
		}

		if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
            UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
		{
			if(index == ACL_CARE_TAG_IPV4 ||
				index == ACL_CARE_TAG_IPV6 ||
				index == ACL_CARE_TAG_TCP ||
				index == ACL_CARE_TAG_UDP)
				continue;
		}

		
		
		ruleW.careTag.tags[index].value = 1;
	    ruleW.careTag.tags[index].mask = 1;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		
		for(index2 = 0; index2 <= index; index2++)
		{   
			
			if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
				UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
				UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
			{
				if(index2 == ACL_CARE_TAG_PPPOE)
					continue;
			}
			
			if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
                UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
			{
				if(index2 == ACL_CARE_TAG_IPV4 ||
					index2 == ACL_CARE_TAG_IPV6 ||
					index2 == ACL_CARE_TAG_TCP ||
					index2 == ACL_CARE_TAG_UDP)
					continue;
			}
			#if 0
			osal_printf("\n%d W:%d R:%d\n",
				index2,
				ruleW.careTag.tags[index2].value,
				ruleR.careTag.tags[index2].value);
			#endif
			if(ruleW.careTag.tags[index2].value != ruleR.careTag.tags[index2].value)
		    {
		    	
		        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
		        return RT_ERR_FAILED;       
	    	}
		}
	}
    
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
    {	
    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
        ruleW.act.forwardAct.portMask.bits[0] = 0x11;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act || 
    		ruleW.act.forwardAct.portMask.bits[0] != ruleR.act.forwardAct.portMask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID)
    {	
    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_COPY_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {	
    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_EGRESSMASK_ACT;
        ruleW.act.forwardAct.portMask.bits[0] = 0xF;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act || 
    		ruleW.act.forwardAct.portMask.bits[0] != ruleR.act.forwardAct.portMask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }


    
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
		UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;

		if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
			ruleW.act.forwardAct.portMask.bits[0] = 0x1;
		else
        	ruleW.act.forwardAct.portMask.bits[0] = 0x22;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act || 
    		ruleW.act.forwardAct.portMask.bits[0] != ruleR.act.forwardAct.portMask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    	
    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_IGR_MIRROR_ACT;
		
		if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
			ruleW.act.forwardAct.portMask.bits[0] = 0x2;
		else
        	ruleW.act.forwardAct.portMask.bits[0] = 0x44;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act || 
    		ruleW.act.forwardAct.portMask.bits[0] != ruleR.act.forwardAct.portMask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

		ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		ruleW.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}
		ruleR.index =  ruleW.index;
		if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}		
		
		if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
			ruleR.act.forwardAct.act != ACL_IGR_FORWARD_REDIRECT_ACT ||
			ruleR.act.forwardAct.portMask.bits[0] != 0x00)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			
			return RT_ERR_FAILED;		
		}
    }

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID)
    {

    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act) 
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act) 
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    }

    
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
        UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;

        ruleW.act.forwardAct.portMask.bits[0] = 0x22;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act || 
    		ruleW.act.forwardAct.portMask.bits[0] != ruleR.act.forwardAct.portMask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        ruleW.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
    		ruleW.act.forwardAct.act != ruleR.act.forwardAct.act)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

		ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		ruleW.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}
		ruleR.index =  ruleW.index;
		if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}		
		
		if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
			ruleR.act.forwardAct.act != ACL_IGR_FORWARD_REDIRECT_ACT ||
			ruleR.act.forwardAct.portMask.bits[0] != 0x00)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			
			return RT_ERR_FAILED;		
		}

		ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
		ruleW.act.forwardAct.act = ACL_IGR_FORWARD_TRAP2SLAVE_ACT;
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}
		ruleR.index =  ruleW.index;
		if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}		
		if(ruleW.act.enableAct[ACL_IGR_FORWARD_ACT] != ruleR.act.enableAct[ACL_IGR_FORWARD_ACT] ||
			ruleW.act.forwardAct.act != ruleR.act.forwardAct.act)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}		
    }


	

    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
		UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
        ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_IGR_CVLAN_ACT;
        ruleW.act.cvlanAct.cvid = 1;
    	if(rtk_vlan_create(ruleW.act.cvlanAct.cvid) != RT_ERR_OK && rtk_vlan_create(ruleW.act.cvlanAct.cvid) != RT_ERR_VLAN_EXIST)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
    		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act || 
    		ruleW.act.cvlanAct.cvid != ruleR.act.cvlanAct.cvid)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
        ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_EGR_CVLAN_ACT;
        ruleW.act.cvlanAct.cvid = 1;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
    		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act || 
    		ruleW.act.cvlanAct.cvid != ruleR.act.cvlanAct.cvid)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
        ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_DS_SVID_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
    		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
        ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_POLICING_ACT;
        ruleW.act.cvlanAct.meter = 31;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
    		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act || 
    		ruleW.act.cvlanAct.meter != ruleR.act.cvlanAct.meter)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
        ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_MIB_ACT;
        ruleW.act.cvlanAct.mib = 29;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
    		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act || 
    		ruleW.act.cvlanAct.mib != ruleR.act.cvlanAct.mib)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
    ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_1P_REMARK_ACT;
    ruleW.act.cvlanAct.dot1p = 7;
    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	ruleR.index =  ruleW.index;
    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act || 
		ruleW.act.cvlanAct.dot1p!= ruleR.act.cvlanAct.dot1p)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
	}

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] = ENABLED;
        ruleW.act.cvlanAct.act = ACL_IGR_CVLAN_BW_METER_ACT;
		if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
	        ruleW.act.cvlanAct.meter = 31;
		else
			ruleW.act.cvlanAct.meter = 47;
		
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_CVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_CVLAN_ACT] ||
    		ruleW.act.cvlanAct.act != ruleR.act.cvlanAct.act || 
    		ruleW.act.cvlanAct.meter != ruleR.act.cvlanAct.meter)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

	}



    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
		UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_IGR_SVLAN_ACT;
        ruleW.act.svlanAct.svid = 2;
    	if(rtk_svlan_create(ruleW.act.svlanAct.svid) != RT_ERR_OK && rtk_svlan_create(ruleW.act.svlanAct.svid) != RT_ERR_SVLAN_EXIST)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
    		ruleW.act.svlanAct.svid != ruleR.act.svlanAct.svid)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_EGR_SVLAN_ACT;
        ruleW.act.svlanAct.svid = 2;
    	
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }	

    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
    		ruleW.act.svlanAct.svid != ruleR.act.svlanAct.svid)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_US_CVID_ACT;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_POLICING_ACT;
        ruleW.act.svlanAct.meter = 28;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
    		ruleW.act.svlanAct.meter != ruleR.act.svlanAct.meter)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_MIB_ACT;
        ruleW.act.svlanAct.mib = 27;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
    		ruleW.act.svlanAct.mib != ruleR.act.svlanAct.mib)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
    ruleW.act.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
    ruleW.act.svlanAct.dscp = 63;
    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	ruleR.index =  ruleW.index;
    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
		ruleW.act.svlanAct.dscp != ruleR.act.svlanAct.dscp)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
	}

	
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
		UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
		ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
	    ruleW.act.svlanAct.act = ACL_IGR_SVLAN_1P_REMARK_ACT;
	    ruleW.act.svlanAct.dot1p = 6;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
			ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
			ruleW.act.svlanAct.dot1p != ruleR.act.svlanAct.dot1p)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}
    }
	
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_ROUTE_ACT;
        ruleW.act.svlanAct.nexthop = 7;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
    		ruleW.act.svlanAct.nexthop != ruleR.act.svlanAct.nexthop)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] = ENABLED;
        ruleW.act.svlanAct.act = ACL_IGR_SVLAN_BW_METER_ACT;
        ruleW.act.svlanAct.meter = 31;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_SVLAN_ACT] != ruleR.act.enableAct[ACL_IGR_SVLAN_ACT] ||
    		ruleW.act.svlanAct.act != ruleR.act.svlanAct.act || 
    		ruleW.act.svlanAct.meter != ruleR.act.svlanAct.meter)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

	
	ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
    ruleW.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
    ruleW.act.priAct.aclPri = 6;
    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	ruleR.index =  ruleW.index;
    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
		ruleW.act.priAct.act != ruleR.act.priAct.act || 
		ruleW.act.priAct.aclPri != ruleR.act.priAct.aclPri)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
	}
    
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
		UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
	    ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
        ruleW.act.priAct.act = ACL_IGR_PRI_DSCP_REMARK_ACT;
        ruleW.act.priAct.dscp = 55;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
    		ruleW.act.priAct.act != ruleR.act.priAct.act || 
    		ruleW.act.priAct.dscp != ruleR.act.priAct.dscp)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
        ruleW.act.priAct.act = ACL_IGR_PRI_1P_REMARK_ACT;
        ruleW.act.priAct.dot1p = 5;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
    		ruleW.act.priAct.act != ruleR.act.priAct.act || 
    		ruleW.act.priAct.dot1p != ruleR.act.priAct.dot1p)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
        ruleW.act.priAct.act = ACL_IGR_PRI_POLICING_ACT;
        ruleW.act.priAct.meter = 7;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
    		ruleW.act.priAct.act != ruleR.act.priAct.act || 
    		ruleW.act.priAct.meter != ruleR.act.priAct.meter)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
        ruleW.act.priAct.act = ACL_IGR_PRI_MIB_ACT;
        ruleW.act.priAct.mib = 14;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
    		ruleW.act.priAct.act != ruleR.act.priAct.act || 
    		ruleW.act.priAct.mib != ruleR.act.priAct.mib)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

	if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
        ruleW.act.priAct.act = ACL_IGR_PRI_BW_METER_ACT;
		if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID) 
	        ruleW.act.priAct.meter = 31;
		else
			ruleW.act.priAct.meter = 47;
		
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
    		ruleW.act.priAct.act != ruleR.act.priAct.act || 
    		ruleW.act.priAct.meter != ruleR.act.priAct.meter)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }
	
	if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
        UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
        ruleW.act.priAct.act = ACL_IGR_PRI_TOS_REMARK_ACT;
	    ruleW.act.priAct.tos = 0x7F;
		
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_PRI_ACT] != ruleR.act.enableAct[ACL_IGR_PRI_ACT] ||
    		ruleW.act.priAct.act != ruleR.act.priAct.act || 
    		ruleW.act.priAct.tos != ruleR.act.priAct.tos)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

	ruleW.act.enableAct[ACL_IGR_LOG_ACT] = ENABLED;
    ruleW.act.logAct.act = ACL_IGR_LOG_POLICING_ACT;
    ruleW.act.logAct.meter = 3;
    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	ruleR.index =  ruleW.index;
    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
	if(ruleW.act.enableAct[ACL_IGR_LOG_ACT] != ruleR.act.enableAct[ACL_IGR_LOG_ACT] ||
		ruleW.act.logAct.act != ruleR.act.logAct.act || 
		ruleW.act.logAct.meter != ruleR.act.logAct.meter)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
	}

	ruleW.act.enableAct[ACL_IGR_LOG_ACT] = ENABLED;
    ruleW.act.logAct.act = ACL_IGR_LOG_MIB_ACT;
    ruleW.act.logAct.mib = 4;
    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	ruleR.index =  ruleW.index;
    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }		
	if(ruleW.act.enableAct[ACL_IGR_LOG_ACT] != ruleR.act.enableAct[ACL_IGR_LOG_ACT] ||
		ruleW.act.logAct.act != ruleR.act.logAct.act || 
		ruleW.act.logAct.mib != ruleR.act.logAct.mib)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
	}

	if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
		ruleW.act.enableAct[ACL_IGR_LOG_ACT] = ENABLED;
		ruleW.act.logAct.act = ACL_IGR_LOG_BW_METER_ACT;
		ruleW.act.logAct.meter = 5;
		if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}
		ruleR.index =  ruleW.index;
		if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}		
		if(ruleW.act.enableAct[ACL_IGR_LOG_ACT] != ruleR.act.enableAct[ACL_IGR_LOG_ACT] ||
			ruleW.act.logAct.act != ruleR.act.logAct.act || 
			ruleW.act.logAct.meter != ruleR.act.logAct.meter)
		{
			osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
			return RT_ERR_FAILED;		
		}
	}
	

    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID || 
		UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
        ruleW.act.extendAct.act = ACL_IGR_EXTEND_NONE_ACT;
    	ruleW.act.aclInterrupt = ENABLED;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
    		ruleW.act.extendAct.act != ruleR.act.extendAct.act ||
    		ruleW.act.aclInterrupt != ruleR.act.aclInterrupt )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}

    	ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
        ruleW.act.extendAct.act = ACL_IGR_EXTEND_SID_ACT;
		if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
			ruleW.act.extendAct.index = 63;
		else
    		ruleW.act.extendAct.index = 127;
    	ruleW.act.aclInterrupt = DISABLED;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
    		ruleW.act.extendAct.act != ruleR.act.extendAct.act || 
    		ruleW.act.extendAct.index != ruleR.act.extendAct.index ||
    		ruleW.act.aclInterrupt != ruleR.act.aclInterrupt )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }

    if(UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
    	ruleW.act.aclInterrupt = ENABLED;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
    		ruleW.act.aclInterrupt != ruleR.act.aclInterrupt )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }
    
    if(UNITTEST_UTIL_CHIP_TYPE == RTL9601B_CHIP_ID ||
		UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID)
    {
    	ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
    	ruleW.act.aclLatch = ENABLED;
        if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    	ruleR.index =  ruleW.index;
        if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }		
    	if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
    		ruleW.act.aclLatch != ruleR.act.aclLatch )
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
    	}
    }


    if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)
    {
		ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
	    ruleW.act.extendAct.act = ACL_IGR_EXTEND_LLID_ACT;
		ruleW.act.extendAct.index = 5;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
			ruleW.act.extendAct.act != ruleR.act.extendAct.act || 
			ruleW.act.extendAct.index != ruleR.act.extendAct.index)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}

		ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
	    ruleW.act.extendAct.act = ACL_IGR_EXTEND_EXT_ACT;
		ruleW.act.extendAct.portMask.bits[0] = 0x3F;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
			ruleW.act.extendAct.act != ruleR.act.extendAct.act || 
			ruleW.act.extendAct.portMask.bits[0] != ruleR.act.extendAct.portMask.bits[0])
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}
  	}


    if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
    {
		ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
	    ruleW.act.extendAct.act = ACL_IGR_EXTEND_LLID_ACT;
		ruleW.act.extendAct.index = 5;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
			ruleW.act.extendAct.act != ruleR.act.extendAct.act || 
			ruleW.act.extendAct.index != ruleR.act.extendAct.index)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}

		ruleW.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
	    ruleW.act.extendAct.act = ACL_IGR_EXTEND_1P_REMARK_ACT;
		ruleW.act.extendAct.dot1p = 7;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.act.enableAct[ACL_IGR_INTR_ACT] != ruleR.act.enableAct[ACL_IGR_INTR_ACT] ||
			ruleW.act.extendAct.act != ruleR.act.extendAct.act || 
			ruleW.act.extendAct.dot1p != ruleR.act.extendAct.dot1p)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}
  	}


	if(UNITTEST_UTIL_CHIP_TYPE == RTL9602C_CHIP_ID)
    {
		ruleW.act.enableAct[ACL_IGR_ROUTE_ACT] = ENABLED;
		ruleW.act.routeAct.act = ACL_IGR_ROUTE_ROUTE_ACT;
	    ruleW.act.routeAct.nexthop = 15;
	    if(rtk_acl_igrRuleEntry_add(&ruleW) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }
		ruleR.index =  ruleW.index;
	    if(rtk_acl_igrRuleEntry_get(&ruleR) != RT_ERR_OK)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
	    }		
		if(ruleW.act.enableAct[ACL_IGR_ROUTE_ACT] != ruleR.act.enableAct[ACL_IGR_ROUTE_ACT] ||
			ruleW.act.routeAct.act != ruleR.act.routeAct.act  ||
			ruleW.act.routeAct.nexthop != ruleR.act.routeAct.nexthop)
	    {
	        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
	        return RT_ERR_FAILED;       
		}
	}
	
    return RT_ERR_OK;
}

int32 dal_acl_test_rangeCheck(uint32 caseNo)
{ 
    rtk_acl_rangeCheck_ip_t ipW;
    rtk_acl_rangeCheck_vid_t vidW;
    rtk_acl_rangeCheck_l4Port_t l4PortW;
    rtk_acl_rangeCheck_pktLength_t pktLengthW;
    rtk_acl_rangeCheck_ip_t ipR;
    rtk_acl_rangeCheck_vid_t vidR;
    rtk_acl_rangeCheck_l4Port_t l4PortR;
    rtk_acl_rangeCheck_pktLength_t pktLengthR;
	rtk_acl_iprange_t ipTypeMax;


    osal_memset(&ipW, 0, sizeof(rtk_acl_rangeCheck_ip_t));
    ipW.index = HAL_MAX_NUM_OF_RANGE_CHECK_IP();
    if( rtk_acl_ipRange_set(&ipW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_ipRange_get(&ipW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&ipW, 0, sizeof(rtk_acl_rangeCheck_ip_t));
    ipW.type = IPRANGE_END;
    if( rtk_acl_ipRange_set(&ipW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&vidW, 0, sizeof(rtk_acl_rangeCheck_vid_t));
    vidW.index = HAL_MAX_NUM_OF_RANGE_CHECK_VID();
    if( rtk_acl_vidRange_set(&vidW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_vidRange_get(&vidW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&vidW, 0, sizeof(rtk_acl_rangeCheck_vid_t));
    vidW.type = VIDRANGE_END;
    if( rtk_acl_vidRange_set(&vidW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&l4PortW, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));
    l4PortW.index = HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT();
    if( rtk_acl_portRange_set(&l4PortW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_portRange_get(&l4PortW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&l4PortW, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));
    l4PortW.type = PORTRANGE_END;
    if( rtk_acl_portRange_set(&l4PortW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    osal_memset(&pktLengthW, 0, sizeof(rtk_acl_rangeCheck_pktLength_t));
    pktLengthW.index = HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN();
    if( rtk_acl_packetLengthRange_set(&pktLengthW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_packetLengthRange_get(&pktLengthW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&pktLengthW, 0, sizeof(rtk_acl_rangeCheck_pktLength_t));
    pktLengthW.type = PKTLENRANGE_END;
    if( rtk_acl_packetLengthRange_set(&pktLengthW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

	/*set get testing*/
	for(ipW.index = 0; ipW.index < HAL_MAX_NUM_OF_RANGE_CHECK_IP(); ipW.index++)
	{	
		ipR.index = ipW.index;
		if(UNITTEST_UTIL_CHIP_TYPE == RTL9607C_CHIP_ID ||
            UNITTEST_UTIL_CHIP_TYPE == RTL9603CVD_CHIP_ID)
			ipTypeMax = IPRANGE_IPV4_DIP_INNER;
		else
			ipTypeMax = IPRANGE_IPV6_DIP;

		
		for(ipW.type = 0; ipW.type <= ipTypeMax; ipW.type++)
		{
			for(ipW.upperIp = 0; ipW.upperIp < 0x9FFFFFFF; ipW.upperIp += 0x53456789)
			{
				for(ipW.lowerIp = 0; ipW.lowerIp < 0x9FFFFFFF; ipW.lowerIp += 0x5456789a)
				{
				    if( rtk_acl_ipRange_set(&ipW) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }
					
				    if( rtk_acl_ipRange_get(&ipR) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				

					if(ipW.type != ipR.type ||
						ipW.upperIp != ipR.upperIp || 
						ipW.lowerIp != ipR.lowerIp)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				
				}				
			}			
		}		
	}

	
	for(vidW.index = 0; vidW.index < HAL_MAX_NUM_OF_RANGE_CHECK_VID(); vidW.index++)
	{	
		vidR.index = vidW.index;
		
		for(vidW.type = 0; vidW.type < VIDRANGE_END; vidW.type++)
		{
			for(vidW.upperVid = 0; vidW.upperVid < 0xFFF; vidW.upperVid += 0x577)
			{
				for(vidW.lowerVid = 0; vidW.lowerVid < 0xFFF; vidW.lowerVid += 0x588)
				{
				    if( rtk_acl_vidRange_set(&vidW) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }
					
				    if( rtk_acl_vidRange_get(&vidR) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				

					if(vidW.type != vidR.type ||
						vidW.upperVid != vidR.upperVid || 
						vidW.lowerVid != vidR.lowerVid)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				
				}				
			}			
		}		
	}

	
	for(l4PortW.index = 0; l4PortW.index < HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(); l4PortW.index++)
	{	
		l4PortR.index = l4PortW.index;
		
		for(l4PortW.type = 0; l4PortW.type < PORTRANGE_END; l4PortW.type++)
		{
			for(l4PortW.upper_bound = 0; l4PortW.upper_bound < 0x9FFF; l4PortW.upper_bound += 0x5777)
			{
				for(l4PortW.lower_bound = 0; l4PortW.lower_bound < 0x9FFF; l4PortW.lower_bound += 0x5888)
				{
				    if( rtk_acl_portRange_set(&l4PortW) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }
					
				    if( rtk_acl_portRange_get(&l4PortR) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				

					if(l4PortW.type != l4PortR.type ||
						l4PortW.upper_bound != l4PortR.upper_bound || 
						l4PortW.lower_bound != l4PortR.lower_bound)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				
				}				
			}			
		}		
	}

	
	for(pktLengthW.index = 0; pktLengthW.index < HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(); pktLengthW.index++)
	{	
		pktLengthR.index = pktLengthW.index;
		
		for(pktLengthW.type = 0; pktLengthW.type < PKTLENRANGE_END; pktLengthW.type++)
		{
			for(pktLengthW.upper_bound = 0; pktLengthW.upper_bound < 0x3FFF; pktLengthW.upper_bound += 0x1FED)
			{
				for(pktLengthW.lower_bound = 0; pktLengthW.lower_bound < 0x3FFF; pktLengthW.lower_bound += 0x1234)
				{
				    if( rtk_acl_packetLengthRange_set(&pktLengthW) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }
					
				    if( rtk_acl_packetLengthRange_get(&pktLengthR) != RT_ERR_OK)
				    {
				        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
				        return RT_ERR_FAILED;       
				    }				
					if(pktLengthW.type != pktLengthR.type ||
						pktLengthW.upper_bound != pktLengthR.upper_bound || 
						pktLengthW.lower_bound != pktLengthR.lower_bound)
				    {
				        osal_printf("\n %s %d index:%d\n",__FUNCTION__,__LINE__,pktLengthR.index);
				        return RT_ERR_FAILED;       
				    }				
				}				
			}			
		}		
	}


	/*NULL check*/
    if( rtk_acl_ipRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_ipRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_vidRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_vidRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_portRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_portRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_packetLengthRange_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_acl_packetLengthRange_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
	
    return RT_ERR_OK;    
}





