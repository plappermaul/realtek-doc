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
#include <common/error.h>

#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>

#include <rtk/svlan.h>

#ifdef CONFIG_SDK_APOLLO
#include <dal/apollo/dal_apollo.h>
#include <dal/apollo/dal_apollo_svlan.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_svlan.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#endif

typedef struct diag_svlan_mbrCfg_s
{
    uint32              idx;
    rtk_vlan_t          svid;
    rtk_portmask_t      mbr;
    rtk_portmask_t	    untagset;
    rtk_pri_t           spri;
    rtk_enable_t        fid_en;
    rtk_fid_t           fid_msti;
    rtk_enable_t        efid_en;
    uint32              efid;
	
} diag_svlan_mbrCfg_t;


typedef enum diag_svlan_mc2sfmt_e
{
    DIAG_SVLAN_MC2S_FMT_MAC  = 0,
    DIAG_SVLAN_MC2S_FMT_IP,
    DIAG_SVLAN_MC2S_FMT_END,    

} diag_svlan_mc2sfmt_t;


typedef struct diag_svlan_mc2sCfg_s
{
    uint32                  idx;
    uint32                  svidx;
    uint32                  format;
    uint32                  data;
    uint32                  mask;
    rtk_enable_t            valid;
	
} diag_svlan_mc2sCfg_t;


#ifndef CONFIG_SDK_RTL9601B

void _diag_svlan_c2s_display(uint32 index, uint32 svidx, uint32 evid, rtk_portmask_t pmsk)
{
    diag_util_printf("%-6d", index);
    diag_util_printf("%-6d", svidx);
    diag_util_printf("%-5d", evid);
    diag_util_mprintf("%s\n", diag_util_mask32tostr(pmsk.bits[0]));
}

void _diag_svlan_entry_display(diag_svlan_mbrCfg_t *pMbrCfg)
{
    diag_util_printf("%-6d", pMbrCfg->idx);
    diag_util_printf("%-5d", pMbrCfg->svid);

    diag_util_printf("%-8s", diag_util_mask32tostr(pMbrCfg->mbr.bits[0]));

    diag_util_printf("%-8s", diag_util_mask32tostr((~pMbrCfg->untagset.bits[0])&0x7F));

    diag_util_printf("%-5d", pMbrCfg->spri);

    diag_util_printf("%-8s", diagStr_enable[pMbrCfg->fid_en]);
    diag_util_printf("%-4d",pMbrCfg->fid_msti);  

    diag_util_printf("%-8s", diagStr_enable[pMbrCfg->efid_en]);
    diag_util_mprintf("%d\n",pMbrCfg->efid);  

}

void _diag_svlan_mc2s_display(diag_svlan_mc2sCfg_t *pMc2sCfg)
{
    uint8  macStr[19];
    rtk_mac_t mac;
    
    diag_util_printf("%-6d", pMc2sCfg->idx);
    diag_util_printf("%-6d", pMc2sCfg->svidx);
    diag_util_printf("%-10s", diagStr_svlanFmtStr[pMc2sCfg->format]);

    if(DIAG_SVLAN_MC2S_FMT_MAC == pMc2sCfg->format)
    {
        mac.octet[0] = 0;
        mac.octet[1] = 0;
        mac.octet[2] = (pMc2sCfg->data>>24)&0xFF;
        mac.octet[3] = (pMc2sCfg->data>>16)&0xFF;
        mac.octet[4] = (pMc2sCfg->data>>8)&0xFF;
        mac.octet[5] = pMc2sCfg->data&0xFF;
        
        diag_util_mac2str(macStr,mac.octet);
        macStr[0] = 'x';
        macStr[1] = 'x';
        macStr[3] = 'x';
        macStr[4] = 'x';        
        diag_util_printf("%-18s", macStr);

        mac.octet[2] = (pMc2sCfg->mask>>24)&0xFF;
        mac.octet[3] = (pMc2sCfg->mask>>16)&0xFF;
        mac.octet[4] = (pMc2sCfg->mask>>8)&0xFF;
        mac.octet[5] = pMc2sCfg->mask&0xFF;
        diag_util_mac2str(macStr,mac.octet);
        macStr[0] = 'x';
        macStr[1] = 'x';
        macStr[3] = 'x';
        macStr[4] = 'x';        
        diag_util_printf("%-18s", macStr);
    }
    else
    {
        diag_util_printf("%-18s", diag_util_inet_ntoa(pMc2sCfg->data));
        diag_util_printf("%-18s", diag_util_inet_ntoa(pMc2sCfg->mask));
    }
    
    diag_util_mprintf("%s\n", diagStr_enable[pMc2sCfg->valid]);
}
#endif

/*
 * svlan init
 */
cparser_result_t
cparser_cmd_svlan_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_svlan_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_init */

/*
 * svlan create svlan-table svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_create_svlan_table_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_t svid;

    DIAG_UTIL_PARAM_CHK();

    svid = *svid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_create(svid), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_create_svlan_table_svid_svid */

/*
 * svlan destroy svlan-table all
 */
cparser_result_t
cparser_cmd_svlan_destroy_svlan_table_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_t svid;
    rtk_svlan_lookupType_t type;
    
    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
		    for(svid = 0; svid <= 4095; svid ++)
		    {
		        rtk_svlan_destroy(svid);
		    }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
		    DIAG_UTIL_ERR_CHK(rtk_svlan_lookupType_get(&type), ret);

			if(type == SVLAN_LOOKUP_C4KVLAN)
			{
			    diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            	return CPARSER_NOT_OK;
			}
			
		    for(svid = 0; svid <= 4095; svid ++)
		    {
		        rtk_svlan_destroy(svid);
		    }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
	
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_destroy_svlan_table_all */

/*
 * svlan destroy svlan-table svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_destroy_svlan_table_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_vlan_t svid;

    DIAG_UTIL_PARAM_CHK();

    svid = *svid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_destroy(svid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_destroy_svlan_table_svid_svid */

/*
 * svlan get priority-source 
 */
cparser_result_t
cparser_cmd_svlan_get_priority_source(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_pri_ref_t ref;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_priorityRef_get(&ref), ret);

    diag_util_mprintf("Priority source: %s\n", diagStr_svlanSpriSrc[ref]);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_priority_source */

/*
 * svlan set priority-source ( internal-priority | dot1q-priority | svlan-member-config | port-based-priority | outer-tag-priority ) 
 */
cparser_result_t
cparser_cmd_svlan_set_priority_source_internal_priority_dot1q_priority_svlan_member_config_port_based_priority_outer_tag_priority(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_pri_ref_t ref = REF_INTERNAL_PRI;
    
    DIAG_UTIL_PARAM_CHK();

    if ('i' == TOKEN_CHAR(3,0))
    {
        ref = REF_INTERNAL_PRI;
    }
    else if ('d' == TOKEN_CHAR(3,0))
    {
        ref = REF_CTAG_PRI;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        ref = REF_SVLAN_PRI;
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        ref = REF_PB;
    }
    else if ('o' == TOKEN_CHAR(3,0))
    {
        ref = REF_OUTER_TAG;
    }   
    DIAG_UTIL_ERR_CHK(rtk_svlan_priorityRef_set(ref), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_priority_source_internal_priority_dot1q_priority_svlan_member_config_port_based_priority_outer_tag_priority */

/*
 * svlan get service-port
 */
cparser_result_t
cparser_cmd_svlan_get_service_port(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port = 0;
    rtk_enable_t enable;
    rtk_portmask_t lPortMask;
    uint8  port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    lPortMask.bits[0] = 0;
    HAL_SCAN_ALL_PORT(port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_servicePort_get(port, &enable), ret);
        if( ENABLED == enable)
            lPortMask.bits[0] |= (1 << port);
    }

    diag_util_lPortMask2str(port_list, &lPortMask);
    diag_util_mprintf("Server Ports: %s\n",port_list);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_service_port */

/*
 * svlan set service-port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_svlan_set_service_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;
	rtk_portmask_t all_portmask, mask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

	if(RTK_PORTMASK_GET_PORT_COUNT(portlist.portmask) != 0)
	{
	    enable = ENABLED;
	    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	    {
			if(HAL_IS_PORT_EXIST(port))
			{
	        	DIAG_UTIL_ERR_CHK(rtk_svlan_servicePort_set(port, enable), ret);
			}
	    }
		
   		enable = DISABLED;
	    DIAG_UTIL_PORTMASK_UNSETSCAN(portlist, port)
	    {
			if(HAL_IS_PORT_EXIST(port))
			{
	        	DIAG_UTIL_ERR_CHK(rtk_svlan_servicePort_set(port, enable), ret);
			}
	    }
		
	}
    else
   	{
   		enable = DISABLED;
	    DIAG_UTIL_PORTMASK_UNSETSCAN(portlist, port)
	    {
			if(HAL_IS_PORT_EXIST(port))
			{
	        	DIAG_UTIL_ERR_CHK(rtk_svlan_servicePort_set(port, enable), ret);
			}
	    }
   	}

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_service_port_ports_all_none */

/*
 * svlan get port ( <PORT_LIST:ports> | all ) svid
 */
cparser_result_t
cparser_cmd_svlan_get_port_ports_all_svid(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Port SVID\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_portSvid_get(port, &svid), ret);
        diag_util_mprintf("%-5d%d\n", port, svid);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_port_ports_all_svid */


/*
 * svlan set port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_set_port_ports_all_svid_svid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    svid = *svid_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_portSvid_set(port, svid), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_port_ports_all_svid_svid */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * svlan get port ( <PORT_LIST:ports> | all ) svlan-index
 */
cparser_result_t
cparser_cmd_svlan_get_port_ports_all_svlan_index(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    uint32 svidx;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Port SVID\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, SVIDXf, &svidx), ret);
                diag_util_mprintf("%-5d%d\n", port, svidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &svidx), ret);
                diag_util_mprintf("%-5d%d\n", port, svidx);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }

    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_port_ports_all_svlan_index */

/*
 * svlan set port ( <PORT_LIST:ports> | all ) svlan-index <UINT:svidx>
 */
cparser_result_t
cparser_cmd_svlan_set_port_ports_all_svlan_index_svidx(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *svidx_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    uint32 svidx;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    svidx = *svidx_ptr;
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, SVIDXf, &svidx), ret);

                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_P_SVIDXr, port , REG_ARRAY_INDEX_NONE, APOLLOMP_SVIDXf, &svidx), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_port_ports_all_svlan_index_svidx */


/*
 * svlan set entry <UINT:index> svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_svid_svid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 svid;
    uint32 index;

    DIAG_UTIL_PARAM_CHK();

    svid = *svid_ptr;
    index = *index_ptr;
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_VLAN_ID_MAX < svid), ret);


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, SVIDf, &svid), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svid), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_svid_svid */

/*
 * svlan set entry <UINT:index> member ( <PORT_LIST:ports> | all | none ) 
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  

    DIAG_UTIL_PARAM_CHK();

    index = *index_ptr;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, MBRf, &portlist.portmask.bits[0]), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_MBRf, &portlist.portmask.bits[0]), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.memberport.bits[0] = portlist.portmask.bits[0];
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_member_ports_all_none */

/*
 * svlan set entry <UINT:index> ( tag-member | untag-member ) ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_tag_member_untag_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  

    DIAG_UTIL_PARAM_CHK();

    index = *index_ptr;
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('t' == TOKEN_CHAR(4,0))
    {
        portlist.portmask.bits[0] = (~portlist.portmask.bits[0])&0x7F;
    }


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, UNTAGSETf, &portlist.portmask.bits[0]), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_UNTAGSETf, &portlist.portmask.bits[0]), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.untagport.bits[0] = portlist.portmask.bits[0];
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_tag_member_untag_member_ports_all_none */

/*
 * svlan set entry <UINT:index> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 priority;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  

    DIAG_UTIL_PARAM_CHK();

    priority = *priority_ptr;
    index = *index_ptr;
    DIAG_UTIL_PARAM_RANGE_CHK((RTK_DOT1P_PRIORITY_MAX < priority), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, SPRf, &priority), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SPRf, &priority), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.priority = priority;
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_priority_priority */

/*
 * svlan set entry <UINT:index> fid-msti state ( disable | enable )
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_fid_msti_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  

    index = *index_ptr;

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE,index, FIDENf, &enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE,index, APOLLOMP_FIDENf, &enable), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.fiden = enable;
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_fid_msti_state_disable_enable */

/*
 * svlan set entry <UINT:index> fid-msti <UINT:fid_msti>
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_fid_msti_fid_msti(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *fid_msti_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 fid;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  

    DIAG_UTIL_PARAM_CHK();

    fid = *fid_msti_ptr;
    index = *index_ptr;
    DIAG_UTIL_PARAM_RANGE_CHK((HAL_VLAN_FID_MAX() < fid), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, FID_MSTIf, &fid), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_FID_MSTIf, &fid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.fid = fid;
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_fid_msti_fid_msti */

/*
 * svlan set entry <UINT:index> enhanced-fid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_enhanced_fid_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  
	
    index = *index_ptr;

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE,index, EFIDENf, &enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE,index, APOLLOMP_EFIDENf, &enable), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.efiden = enable;
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_enhanced_fid_state_disable_enable */

/*
 * svlan set entry <UINT:index> enhanced-fid <UINT:efid>
 */
cparser_result_t
cparser_cmd_svlan_set_entry_index_enhanced_fid_efid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 efid;
    uint32 index;
	rtk_svlan_memberCfg_t svlanRtkcfg;  

    DIAG_UTIL_PARAM_CHK();

    efid = *efid_ptr;
    index = *index_ptr;
    DIAG_UTIL_PARAM_RANGE_CHK((HAL_ENHANCED_FID_MAX() < efid), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= index), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, EFIDf, &efid), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= index), ret);
			
            //DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EFIDf, &efid), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDf, &svlanRtkcfg.svid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);
			svlanRtkcfg.efid = efid;
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_set(&svlanRtkcfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_entry_index_enhanced_fid_efid */

/*
 * svlan get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_svlan_get_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    diag_svlan_mbrCfg_t   svlanCfg;
	rtk_svlan_memberCfg_t svlanRtkcfg;  
		
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    svlanCfg.idx = *index_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= svlanCfg.idx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, SVIDf, &svlanCfg.svid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, MBRf, &svlanCfg.mbr.bits[0]), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, UNTAGSETf, &svlanCfg.untagset.bits[0]), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, SPRf, &svlanCfg.spri), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, FIDENf, &svlanCfg.fid_en), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, FID_MSTIf, &svlanCfg.fid_msti), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, EFIDENf, &svlanCfg.efid_en), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, EFIDf, &svlanCfg.efid), ret);

            diag_util_mprintf("Index SVID Member  TagSet  Spri FidEn   FID EfidEn  Efid\n");
            _diag_svlan_entry_display(&svlanCfg);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= svlanCfg.idx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, APOLLOMP_SVIDf, &svlanCfg.svid), ret);

			svlanRtkcfg.svid = svlanCfg.svid;
			DIAG_UTIL_ERR_CHK(rtk_svlan_memberPortEntry_get(&svlanRtkcfg), ret);

			svlanCfg.mbr.bits[0] = svlanRtkcfg.memberport.bits[0];
			svlanCfg.untagset.bits[0] = svlanRtkcfg.untagport.bits[0];
			svlanCfg.spri = svlanRtkcfg.priority;
			svlanCfg.fid_en = svlanRtkcfg.fiden;
			svlanCfg.fid_msti = svlanRtkcfg.fid;
			svlanCfg.efid_en = svlanRtkcfg.efiden;
			svlanCfg.efid = svlanRtkcfg.efid;
            diag_util_mprintf("Index SVID Member  TagSet  Spri FidEn   FID EfidEn  Efid\n");
            _diag_svlan_entry_display(&svlanCfg);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_entry_index */

/*
 * svlan get entry all
 */
cparser_result_t
cparser_cmd_svlan_get_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    diag_svlan_mbrCfg_t   svlanCfg;
	rtk_svlan_memberCfg_t svlanRtkcfg;  
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Index SVID Member  TagSet  Spri FidEn   FID EfidEn  Efid\n");

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            for(svlanCfg.idx = 0; svlanCfg.idx < APOLLO_DAL_SVLAN_ENTRY_NO; svlanCfg.idx ++)
            {
                DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= svlanCfg.idx), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, SVIDf, &svlanCfg.svid), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, MBRf, &svlanCfg.mbr.bits[0]), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, UNTAGSETf, &svlanCfg.untagset.bits[0]), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, SPRf, &svlanCfg.spri), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, FIDENf, &svlanCfg.fid_en), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, FID_MSTIf, &svlanCfg.fid_msti), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, EFIDENf, &svlanCfg.efid_en), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, EFIDf, &svlanCfg.efid), ret);
            
                _diag_svlan_entry_display(&svlanCfg);
            
            }

            break;
#endif


#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            for(svlanCfg.idx = 0;  svlanCfg.idx < APOLLOMP_SVLAN_ENTRY_NO; svlanCfg.idx ++)
            {
	            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= svlanCfg.idx), ret);

	            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MBRCFGr, REG_ARRAY_INDEX_NONE, svlanCfg.idx, APOLLOMP_SVIDf, &svlanCfg.svid), ret);

				svlanRtkcfg.svid = svlanCfg.svid;
				if(rtk_svlan_memberPortEntry_get(&svlanRtkcfg) == 0)
				{
					svlanCfg.mbr.bits[0] = svlanRtkcfg.memberport.bits[0];
					svlanCfg.untagset.bits[0] = svlanRtkcfg.untagport.bits[0];
					svlanCfg.spri = svlanRtkcfg.priority;
					svlanCfg.fid_en = svlanRtkcfg.fiden;
					svlanCfg.fid_msti = svlanRtkcfg.fid;
					svlanCfg.efid_en = svlanRtkcfg.efiden;
					svlanCfg.efid = svlanRtkcfg.efid;
	            
	                _diag_svlan_entry_display(&svlanCfg);
				}            
            }

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_entry_all */
/*
 * svlan get vlan-conversion c2s entry <UINT:index> 
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_c2s_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 svidx;
    uint32 evid;
    rtk_portmask_t pmsk;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    index = *index_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, SVIDXf, &svidx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, EVIDf, &evid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, C2SENPMSKf, &pmsk.bits[0]), ret);
            diag_util_mprintf("Index Svidx Evid Port\n");
            _diag_svlan_c2s_display(index, svidx, evid, pmsk);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDXf, &svidx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EVIDf, &evid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_C2SENPMSKf, &pmsk.bits[0]), ret);
            diag_util_mprintf("Index Svidx Evid Port\n");
            _diag_svlan_c2s_display(index, svidx, evid, pmsk);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_c2s_entry_index */

/*
 * svlan get vlan-conversion c2s entry all
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_c2s_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 svidx;
    uint32 evid;
    rtk_portmask_t pmsk;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Index Svidx Evid Port\n");

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            for(index = 0; index < APOLLO_DAL_SVLAN_C2S_NO; index ++)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, SVIDXf, &svidx), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, EVIDf, &evid), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, C2SENPMSKf, &pmsk.bits[0]), ret);

                _diag_svlan_c2s_display(index, svidx, evid, pmsk);
            }

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            for(index = 0; index < APOLLOMP_SVLAN_C2S_NO; index ++)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDXf, &svidx), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EVIDf, &evid), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_C2SENPMSKf, &pmsk.bits[0]), ret);

                _diag_svlan_c2s_display(index, svidx, evid, pmsk);
            }

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_c2s_entry_all */

/*
 * svlan set vlan-conversion c2s entry <UINT:index> enhanced-vid <UINT:evid>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_c2s_entry_index_enhanced_vid_evid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *evid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 evid;
    
    DIAG_UTIL_PARAM_CHK();
    index = *index_ptr;
    evid = *evid_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_MAX_ENHANCE_VID < evid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, EVIDf, &evid), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_MAX_ENHANCE_VID < evid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_EVIDf, &evid), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_c2s_entry_index_enhanced_vid_evid */

/*
 * svlan set vlan-conversion c2s entry <UINT:index> member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_c2s_entry_index_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 svidx;
    uint32 evid;
    rtk_portmask_t pmsk;
    diag_portlist_t portlist;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7), ret);
    
    index = *index_ptr;
    pmsk.bits[0] = portlist.portmask.bits[0];;


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, C2SENPMSKf, &pmsk.bits[0]), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_C2SENPMSKf, &pmsk.bits[0]), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_c2s_entry_index_member_ports_all_none */

/*
 * svlan set vlan-conversion c2s entry <UINT:index> svlan-index <UINT:svidx>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_c2s_entry_index_svlan_index_svidx(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *svidx_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 svidx;
    uint32 evid;
    rtk_portmask_t pmsk;

    DIAG_UTIL_PARAM_CHK();

    index = *index_ptr;
    svidx = *svidx_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= svidx), RT_ERR_SVLAN_ENTRY_INDEX);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, SVIDXf, &svidx), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_C2S_NO <= index), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= svidx), RT_ERR_SVLAN_ENTRY_INDEX);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_C2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDXf, &svidx), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_c2s_entry_index_svlan_index_svidx */

/*
 * svlan del vlan-conversion c2s vid <UINT:vid> port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_del_vlan_conversion_c2s_vid_vid_port_ports_all_svid_svid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ports_ptr,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_vlan_t cvid;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7), ret);

    cvid = *vid_ptr;
    svid = *svid_ptr;
    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_c2s_del(cvid, port, svid), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_del_vlan_conversion_c2s_vid_vid_port_ports_all_svid_svid */

/*
 * svlan add vlan-conversion c2s vid <UINT:vid> port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_add_vlan_conversion_c2s_vid_vid_port_ports_all_svid_svid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ports_ptr,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_vlan_t cvid;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7), ret);

    cvid = *vid_ptr;
    svid = *svid_ptr;
    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_c2s_add(cvid, port, svid), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_add_vlan_conversion_c2s_vid_vid_port_ports_all_svid_svid */

/*
 * svlan get vlan-conversion c2s vid <UINT:vid> port ( <PORT_LIST:ports> | all ) 
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_c2s_vid_vid_port_ports_all(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_vlan_t cvid;
    rtk_vlan_t svid;

    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 7), ret);

    cvid = *vid_ptr;

    diag_util_mprintf("Vid Port Svid\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
		ret = rtk_svlan_c2s_get(cvid, port, &svid);

		if(ret == RT_ERR_OK)
		{		
			diag_util_mprintf("%-3d %-4d %-d\n",cvid, port, svid); 
		}
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_c2s_vid_vid_port_ports_all */

/*
 * svlan get vlan-conversion mc2s entry <UINT:index>
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_mc2s_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_svlan_mc2sCfg_t mc2sCfg;
    uint32 val;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    mc2sCfg.idx = *index_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_MC2S_NO <= mc2sCfg.idx), ret);
            
            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, SVIDXf, &val), ret);
            mc2sCfg.svidx = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, FORMATf, &val), ret);
            mc2sCfg.format = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, DATAf, &val), ret);
            mc2sCfg.data = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, MASKf, &val), ret);
            mc2sCfg.mask = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, VALIDf, &val), ret);
            mc2sCfg.valid = val;
            diag_util_mprintf("Index SVIDX Format    Data              Mask              Valid\n");
            _diag_svlan_mc2s_display(&mc2sCfg);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_MC2S_NO <= mc2sCfg.idx), ret);
            
            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_SVIDXf, &val), ret);
            mc2sCfg.svidx = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_FORMATf, &val), ret);
            mc2sCfg.format = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_DATAf, &val), ret);
            mc2sCfg.data = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_MASKf, &val), ret);
            mc2sCfg.mask = val;

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_VALIDf, &val), ret);
            mc2sCfg.valid = val;
            diag_util_mprintf("Index SVIDX Format    Data              Mask              Valid\n");
            _diag_svlan_mc2s_display(&mc2sCfg);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_mc2s_entry_index */

/*
 * svlan get vlan-conversion mc2s entry all
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_mc2s_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    diag_svlan_mc2sCfg_t mc2sCfg;
    uint32 val;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    diag_util_mprintf("Index SVIDX Format    Data              Mask              Valid\n");

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            for(mc2sCfg.idx = 0; mc2sCfg.idx < APOLLO_DAL_SVLAN_MC2S_NO; mc2sCfg.idx ++)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, SVIDXf, &val), ret);
                mc2sCfg.svidx = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, FORMATf, &val), ret);
                mc2sCfg.format = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, DATAf, &val), ret);
                mc2sCfg.data = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, MASKf, &val), ret);
                mc2sCfg.mask = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, VALIDf, &val), ret);
                mc2sCfg.valid = val;

                _diag_svlan_mc2s_display(&mc2sCfg);
            }


            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            for(mc2sCfg.idx = 0; mc2sCfg.idx < APOLLOMP_SVLAN_MC2S_NO; mc2sCfg.idx ++)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_SVIDXf, &val), ret);
                mc2sCfg.svidx = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_FORMATf, &val), ret);
                mc2sCfg.format = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_DATAf, &val), ret);
                mc2sCfg.data = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_MASKf, &val), ret);
                mc2sCfg.mask = val;

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, mc2sCfg.idx, APOLLOMP_VALIDf, &val), ret);
                mc2sCfg.valid = val;

                _diag_svlan_mc2s_display(&mc2sCfg);
            }


            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_mc2s_entry_all */

/*
 * svlan set vlan-conversion mc2s entry <UINT:index> state ( invalid | valid )
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_state_invalid_valid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_enable_t valid;
    
    DIAG_UTIL_PARAM_CHK();
    index = *index_ptr;

    if ('i' == TOKEN_CHAR(7,0))
    {
        valid = DISABLED;
    }
    else if ('v' == TOKEN_CHAR(7,0))
    {
        valid = ENABLED;
    }
    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, VALIDf, &valid), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_VALIDf, &valid), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_state_invalid_valid */

/*
 * svlan set vlan-conversion mc2s entry <UINT:index> format ( dip | dmac )
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_format_dip_dmac(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    
    DIAG_UTIL_PARAM_CHK();
    index = *index_ptr;

    if ('i' == TOKEN_CHAR(7,1))
    {
        format = DIAG_SVLAN_MC2S_FMT_MAC;
    }
    else if ('m' == TOKEN_CHAR(7,1))
    {
        format = DIAG_SVLAN_MC2S_FMT_IP;
    }
    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, FORMATf, &format), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_FORMATf, &format), ret);


            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_format_dip_dmac */

/*
 * svlan set vlan-conversion mc2s entry <UINT:index> ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_ip_ip_ip_mask_ip_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *ip_mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    uint32 data;
    uint32 mask;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == ip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == ip_mask_ptr), CPARSER_ERR_INVALID_PARAMS);

    index = *index_ptr;

    data = *ip_ptr;
    mask = *ip_mask_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, DATAf, &data), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, MASKf, &mask), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_DATAf, &data), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_MASKf, &mask), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_ip_ip_ip_mask_ip_mask */

/*
 * svlan set vlan-conversion mc2s entry <UINT:index> mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_mac_address_mac_mac_mask_mac_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *mac_ptr,
    cparser_macaddr_t  *mac_mask_ptr)
{
    int32     ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    uint32 data;
    uint32 mask;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == mac_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == mac_mask_ptr), CPARSER_ERR_INVALID_PARAMS);

    index = *index_ptr;

    data = mac_ptr->octet[2];
    data = (data << 8) | mac_ptr->octet[3];
    data = (data << 8) | mac_ptr->octet[4];
    data = (data << 8) | mac_ptr->octet[5];

    mask = mac_mask_ptr->octet[2];
    mask = (mask << 8) | mac_mask_ptr->octet[3];
    mask = (mask << 8) | mac_mask_ptr->octet[4];
    mask = (mask << 8) | mac_mask_ptr->octet[5];


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, DATAf, &data), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, MASKf, &mask), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_DATAf, &data), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_MASKf, &mask), ret);


            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_mac_address_mac_mac_mask_mac_mask */

/*
 * svlan set vlan-conversion mc2s entry <UINT:index> svlan-index <UINT:svidx>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_svlan_index_svidx(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *svidx_ptr)
{
    int32     ret = RT_ERR_FAILED;
    uint32 index;
    uint32 svidx;

    DIAG_UTIL_PARAM_CHK();

    index = *index_ptr;
    svidx = *svidx_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= svidx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, SVIDXf, &svidx), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_MC2S_NO <= index), ret);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= svidx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_MC2Sr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_SVIDXf, &svidx), ret);


            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_mc2s_entry_index_svlan_index_svidx */

/*
 * svlan add vlan-conversion mc2s ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask> svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_add_vlan_conversion_mc2s_ip_ip_ip_mask_ip_mask_svid_svid(
    cparser_context_t *context,
    uint32_t  *ip_ptr,
    uint32_t  *ip_mask_ptr,
    uint32_t  *svid_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 format;
    uint32 data;
    uint32 mask;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == ip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == ip_mask_ptr), CPARSER_ERR_INVALID_PARAMS);


    data = *ip_ptr;
    mask = *ip_mask_ptr;
    svid = *svid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_ipmc2s_add(data, mask, svid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_add_vlan_conversion_mc2s_ip_ip_ip_mask_ip_mask_svid_svid */

/*
 * svlan del vlan-conversion mc2s ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask>
 */
cparser_result_t
cparser_cmd_svlan_del_vlan_conversion_mc2s_ip_ip_ip_mask_ip_mask(
    cparser_context_t *context,
    uint32_t  *ip_ptr,
    uint32_t  *ip_mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    uint32 data;
    uint32 mask;
    
    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == ip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == ip_mask_ptr), CPARSER_ERR_INVALID_PARAMS);

    data = *ip_ptr;
    mask = *ip_mask_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_ipmc2s_del(data, mask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_del_vlan_conversion_mc2s_ip_ip_ip_mask_ip_mask */

/*
 * svlan get vlan-conversion mc2s ip <IPV4ADDR:ip> ip-mask <IPV4ADDR:ip_mask>
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_mc2s_ip_ip_ip_mask_ip_mask(
    cparser_context_t *context,
    uint32_t  *ip_ptr,
    uint32_t  *ip_mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    uint32 data;
    uint32 mask;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    RT_PARAM_CHK((NULL == ip_ptr), CPARSER_ERR_INVALID_PARAMS);
    RT_PARAM_CHK((NULL == ip_mask_ptr), CPARSER_ERR_INVALID_PARAMS);

    data = *ip_ptr;
    mask = *ip_mask_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_ipmc2s_get(data, mask, &svid), ret);

    diag_util_mprintf("SVID %d\n", svid);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_mc2s_ip_ip_ip_mask_ip_mask */

/*
 * svlan add vlan-conversion mc2s mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask> svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_add_vlan_conversion_mc2s_mac_address_mac_mac_mask_mac_mask_svid_svid(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    cparser_macaddr_t  *mac_mask_ptr,
    uint32_t  *svid_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    rtk_mac_t data;
    rtk_mac_t mask;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();
    osal_memcpy(&data.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    osal_memcpy(&mask.octet, mac_mask_ptr->octet, ETHER_ADDR_LEN);    
    svid = *svid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_l2mc2s_add(data, mask, svid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_add_vlan_conversion_mc2s_mac_address_mac_mac_mask_mac_mask_svid_svid */

/*
 * svlan del vlan-conversion mc2s mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask>
 */
cparser_result_t
cparser_cmd_svlan_del_vlan_conversion_mc2s_mac_address_mac_mac_mask_mac_mask(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    cparser_macaddr_t  *mac_mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    rtk_mac_t data;
    rtk_mac_t mask;
    
    DIAG_UTIL_PARAM_CHK();
    osal_memcpy(&data.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    osal_memcpy(&mask.octet, mac_mask_ptr->octet, ETHER_ADDR_LEN);    

    DIAG_UTIL_ERR_CHK(rtk_svlan_l2mc2s_del(data, mask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_del_vlan_conversion_mc2s_mac_address_mac_mac_mask_mac_mask */

/*
 * svlan get vlan-conversion mc2s mac-address <MACADDR:mac> mac-mask <MACADDR:mac_mask>
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_mc2s_mac_address_mac_mac_mask_mac_mask(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    cparser_macaddr_t  *mac_mask_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 index;
    uint32 format;
    rtk_mac_t data;
    rtk_mac_t mask;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    osal_memcpy(&data.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    osal_memcpy(&mask.octet, mac_mask_ptr->octet, ETHER_ADDR_LEN);    

    DIAG_UTIL_ERR_CHK(rtk_svlan_l2mc2s_get(data, mask, &svid), ret);

    diag_util_mprintf("SVID %d\n", svid);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_mc2s_mac_address_mac_mac_mask_mac_mask */

/*
 * svlan get vlan-conversion sp2c entry <UINT:index> 
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_sp2c_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sp2cIndex;
    uint32 cvid;
    uint32 egressPort;
    uint32 svidx;
    uint32 valid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    sp2cIndex = *index_ptr;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, SVIDXf, &svidx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, VIDf, &cvid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, DST_PORTf, &egressPort), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, VALIDf, &valid), ret);
            diag_util_mprintf("Index VID  EgPort Svidx Valid\n");
            diag_util_mprintf("%-6d%-5d%-7d%-6d%s\n", 
                                    sp2cIndex,
                                    cvid,
                                    egressPort,
                                    svidx,
                                    diagStr_enable[valid]);            
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidx), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VIDf, &cvid), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &egressPort), ret);

            DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid), ret);

	
            diag_util_mprintf("Index VID  EgPort Svidx Valid\n");
            diag_util_mprintf("%-6d%-5d%-7d%-6d%s\n", 
                                    sp2cIndex,
                                    cvid,
                                    egressPort,
                                    svidx,
                                    diagStr_enable[valid]);            
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_sp2c_entry_index */

/*
 * svlan get vlan-conversion sp2c entry all
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_sp2c_entry_all(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sp2cIndex;
    uint32 cvid;
    uint32 egressPort;
    uint32 svidx;
    uint32 valid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    diag_util_mprintf("Index VID EgPort Svidx Valid\n");
    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            for(sp2cIndex = 0; sp2cIndex < APOLLO_DAL_SVLAN_SP2C_NO; sp2cIndex ++)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, SVIDXf, &svidx), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, VIDf, &cvid), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, DST_PORTf, &egressPort), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, VALIDf, &valid), ret);

                diag_util_mprintf("%-6d%-5d%-7d%-6d%s\n", 
                                        sp2cIndex,
                                        cvid,
                                        egressPort,
                                        svidx,
                                        diagStr_enable[valid]);
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            for(sp2cIndex = 0; sp2cIndex < APOLLOMP_SVLAN_SP2C_NO; sp2cIndex ++)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidx), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VIDf, &cvid), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &egressPort), ret);

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid), ret);

                diag_util_mprintf("%-6d%-5d%-7d%-6d%s\n", 
                                        sp2cIndex,
                                        cvid,
                                        egressPort,
                                        svidx,
                                        diagStr_enable[valid]);
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    	
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_sp2c_entry_all */

/*
 * svlan set vlan-conversion sp2c entry <UINT:index> state ( invalid | valid )
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_state_invalid_valid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sp2cIndex;
    uint32 valid;
    
    DIAG_UTIL_PARAM_CHK();
    
    sp2cIndex = *index_ptr;

    if ('i' == TOKEN_CHAR(7,0))
    {
        valid = DISABLED;
    }
    else if ('v' == TOKEN_CHAR(7,0))
    {
        valid = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, VALIDf, &valid), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VALIDf, &valid), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_state_invalid_valid */

/*
 * svlan set vlan-conversion sp2c entry <UINT:index> svlan-index <UINT:svidx>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_svlan_index_svidx(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *svidx_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sp2cIndex;
    uint32 svidx;
    
    DIAG_UTIL_PARAM_CHK();
    
    sp2cIndex = *index_ptr;
    svidx = *svidx_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_ENTRY_NO <= svidx), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, SVIDXf, &svidx), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_ENTRY_NO <= svidx), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_SVIDXf, &svidx), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_svlan_index_svidx */

/*
 * svlan set vlan-conversion sp2c entry <UINT:index> port <UINT:port>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_port_port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sp2cIndex;
    uint32 egressPort;
   
    DIAG_UTIL_PARAM_CHK();

    sp2cIndex = *index_ptr;
    egressPort = *port_ptr;
    DIAG_UTIL_PARAM_RANGE_CHK(!HAL_IS_PORT_EXIST(egressPort), RT_ERR_PORT_ID);
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, DST_PORTf, &egressPort), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_DST_PORTf, &egressPort), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_port_port */

/*
 * svlan set vlan-conversion sp2c entry <UINT:index> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_vid_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 sp2cIndex;
    uint32 cvid;
   
    DIAG_UTIL_PARAM_CHK();

    sp2cIndex = *index_ptr;
    cvid = *vid_ptr;


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLO_DAL_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, VIDf, &cvid), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_PARAM_RANGE_CHK((APOLLOMP_SVLAN_SP2C_NO <= sp2cIndex), RT_ERR_SVLAN_ENTRY_INDEX);
            DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_SVLAN_SP2Cr, REG_ARRAY_INDEX_NONE, sp2cIndex, APOLLOMP_VIDf, &cvid), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_sp2c_entry_index_vid_vid */

/*
 * svlan add vlan-conversion sp2c svid <UINT:svid> port <UINT:port> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_svlan_add_vlan_conversion_sp2c_svid_svid_port_port_vid_vid(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *port_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 svid;
    uint32 port;
    uint32 vid;

    DIAG_UTIL_PARAM_CHK();
    svid = *svid_ptr;
    port = *port_ptr;
    vid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_sp2c_add(svid, port, vid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_add_vlan_conversion_sp2c_svid_svid_port_port_vid_vid */

/*
 * svlan del vlan-conversion sp2c svid <UINT:svid> port <UINT:port>
 */
cparser_result_t
cparser_cmd_svlan_del_vlan_conversion_sp2c_svid_svid_port_port(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 svid;
    uint32 port;

    DIAG_UTIL_PARAM_CHK();
    svid = *svid_ptr;
    port = *port_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_sp2c_del(svid, port), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_del_vlan_conversion_sp2c_svid_svid_port_port */

/*
 * svlan get vlan-conversion sp2c svid <UINT:svid> port <UINT:port>
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_sp2c_svid_svid_port_port(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 svid;
    uint32 port;
    uint32 vid;
    uint32 priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    svid = *svid_ptr;
    port = *port_ptr;
	
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_RTL9602C)
		case RTL9602C_CHIP_ID: 		
			diag_util_mprintf("Svid Port Cvid Prioty\n");
			
			DIAG_UTIL_ERR_CHK(rtk_svlan_sp2c_get(svid, port, &vid), ret);
			
			diag_util_mprintf("%-4d %-4d %-4d ",svid, port, vid);

			DIAG_UTIL_ERR_CHK(rtk_svlan_sp2cPriority_get(svid, port, &priority), ret);
			
			diag_util_mprintf("%-d\n",priority);
		 break;
#endif		
		default:
		
			priority = 0;
		    diag_util_mprintf("Svid Port Cvid\n");

		    DIAG_UTIL_ERR_CHK(rtk_svlan_sp2c_get(svid, port, &vid), ret);

		    diag_util_mprintf("%-4d %-4d %-d\n",svid, port, vid);
		break;
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_sp2c_svid_svid_port_port */

/*
 * svlan add vlan-conversion sp2c svid <UINT:svid> port <UINT:port> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_svlan_add_vlan_conversion_sp2c_svid_svid_port_port_priority_priority(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *port_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 svid;
    uint32 port;
    uint32 priority;

    DIAG_UTIL_PARAM_CHK();
    svid = *svid_ptr;
    port = *port_ptr;
    priority = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_sp2cPriority_add(svid, port, priority), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_add_vlan_conversion_sp2c_svid_svid_port_port_priority_priority */

#endif
/*
 * svlan get svlan-table svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_get_svlan_table_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t portmask;
    rtk_portmask_t untagPortmask;
    uint32 svid;
    uint32 val;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    svid = *svid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_get(svid, &portmask, &untagPortmask), ret);

    diag_util_mprintf("SVID Member  UntagSet Spri FidEn   FID EfidEn  Efid\n");

    diag_util_printf("%-5d", svid);

    diag_util_printf("%-8s", diag_util_mask32tostr(portmask.bits[0]));

    diag_util_printf("%-9s", diag_util_mask32tostr(untagPortmask.bits[0]));

	if(rtk_svlan_priority_get(svid, &val) == RT_ERR_OK)
		diag_util_printf("%-5d", val);
	else
		diag_util_printf("x    ");
	
	if(rtk_svlan_fidEnable_get(svid, &val) == RT_ERR_OK)
		diag_util_printf("%-8s", diagStr_enable[val]);
	else
		diag_util_printf("x       ");

	if(rtk_svlan_fid_get(svid, &val) == RT_ERR_OK)
		diag_util_printf("%-4d", val);
	else
		diag_util_printf("x   ");

	if(rtk_svlan_enhancedFidEnable_get(svid, &val) == RT_ERR_OK)
		diag_util_printf("%-8s", diagStr_enable[val]);
	else
		diag_util_printf("x       ");

	if(rtk_svlan_enhancedFid_get(svid, &val) == RT_ERR_OK)
		diag_util_printf("%d\n", val);
	else
		diag_util_printf("x\n");
	
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_svlan_table_svid_svid */

/*
 * svlan set svlan-table svid <UINT:svid> member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t portmask;
    rtk_portmask_t untagPortmask;
    uint32 svid;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    svid = *svid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_get(svid, &portmask, &untagPortmask), ret);

    RTK_PORTMASK_ASSIGN(portmask, portlist.portmask);

    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_set(svid, &portmask, &untagPortmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_member_ports_all_none */

/*
 * svlan set svlan-table svid <UINT:svid> tag-member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_tag_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t portmask;
    rtk_portmask_t untagPortmask;
    uint32 svid;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    svid = *svid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_get(svid, &portmask, &untagPortmask), ret);

    RTK_PORTMASK_ASSIGN(untagPortmask, portlist.portmask);
    HAL_SCAN_ALL_PORT(port)
    {
        if(RTK_PORTMASK_IS_PORT_SET(untagPortmask, port))
        {
            RTK_PORTMASK_PORT_CLEAR(untagPortmask, port);
        }
        else
        {
            RTK_PORTMASK_PORT_SET(untagPortmask, port);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_set(svid, &portmask, &untagPortmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_tag_member_ports_all_none */

/*
 * svlan set svlan-table svid <UINT:svid> untag-member ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_untag_member_ports_all_none(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t portmask;
    rtk_portmask_t untagPortmask;
    uint32 svid;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    svid = *svid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_get(svid, &portmask, &untagPortmask), ret);

    RTK_PORTMASK_ASSIGN(untagPortmask, portlist.portmask);

    DIAG_UTIL_ERR_CHK(rtk_svlan_memberPort_set(svid, &portmask, &untagPortmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_untag_member_ports_all_none */

/*
 * svlan set svlan-table svid <UINT:svid> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_priority_priority(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 priority;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();

	priority = *priority_ptr;
	svid = *svid_ptr;
	
    DIAG_UTIL_ERR_CHK(rtk_svlan_priority_set(svid, priority), ret);	

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_priority_priority */

/*
 * svlan set svlan-table svid <UINT:svid> fid-msti state ( disable | enable )
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_fid_msti_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 enable = DISABLED;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();

	svid = *svid_ptr;
	
    if ('d' == TOKEN_CHAR(7,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(7,0))
    {
        enable = ENABLED;
    }
	
    DIAG_UTIL_ERR_CHK(rtk_svlan_fidEnable_set(svid, enable), ret);	


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_fid_msti_state_disable_enable */

/*
 * svlan set svlan-table svid <UINT:svid> fid-msti <UINT:fid_msti>
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_fid_msti_fid_msti(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *fid_msti_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 fid;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();

	fid = *fid_msti_ptr;
	svid = *svid_ptr;
	
    DIAG_UTIL_ERR_CHK(rtk_svlan_fid_set(svid, fid), ret);	

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_fid_msti_fid_msti */

/*
 * svlan set svlan-table svid <UINT:svid> enhanced-fid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_enhanced_fid_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 enable = DISABLED;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();

	svid = *svid_ptr;
	
    if ('d' == TOKEN_CHAR(7,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(7,0))
    {
        enable = ENABLED;
    }
	
    DIAG_UTIL_ERR_CHK(rtk_svlan_enhancedFidEnable_set(svid, enable), ret);	

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_enhanced_fid_state_disable_enable */

/*
 * svlan set svlan-table svid <UINT:svid> enhanced-fid <UINT:efid>
 */
cparser_result_t
cparser_cmd_svlan_set_svlan_table_svid_svid_enhanced_fid_efid(
    cparser_context_t *context,
    uint32_t  *svid_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 efid;
    uint32 svid;
    
    DIAG_UTIL_PARAM_CHK();

	efid = *efid_ptr;
	svid = *svid_ptr;
	
    DIAG_UTIL_ERR_CHK(rtk_svlan_enhancedFid_set(svid, efid), ret);
	
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_svlan_table_svid_svid_enhanced_fid_efid */

/*
 * svlan get tpid
 */
cparser_result_t
cparser_cmd_svlan_get_tpid(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable = ENABLED;
    uint32 tpid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_RTL9607C)
		case RTL9607C_CHIP_ID:		
			DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEntry_get(0, &tpid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEnable_get(0, &enable), ret);
			
			diag_util_mprintf("TPID[0]: 0x%4.4x State: %s\n",tpid,diagStr_enable[enable]);
			DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEntry_get(1, &tpid), ret);
			DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEnable_get(1, &enable), ret);
			
			diag_util_mprintf("TPID[1]: 0x%4.4x State: %s\n",tpid,diagStr_enable[enable]);
		 break;
#endif		
		default:
		
			DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEntry_get(0, &tpid), ret);
			
			diag_util_mprintf("TPID: 0x%4.4x\n",tpid);
		break;
	}


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_tpid */

/*
 * svlan get tpid entry <UINT:index> tpid
 */
cparser_result_t
cparser_cmd_svlan_get_tpid_entry_index_tpid(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 tpid;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEntry_get(*index_ptr, &tpid), ret);
    
    diag_util_mprintf("Index: %d TPID: 0x%4.4x\n",*index_ptr,tpid);
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_tpid_entry_index_tpid */

/*
 * svlan get tpid entry <UINT:index> state
 */
cparser_result_t
cparser_cmd_svlan_get_tpid_entry_index_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEnable_get(*index_ptr, &enable), ret);

    diag_util_mprintf("Index: %d Status: %s\n",*index_ptr,diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_tpid_entry_index_state */

/*
 * svlan set tpid <UINT:tpid>
 */
cparser_result_t
cparser_cmd_svlan_set_tpid_tpid(
    cparser_context_t *context,
    uint32_t  *tpid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEntry_set(0, *tpid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_tpid_tpid */

/*
 * svlan set tpid entry <UINT:index> tpid <UINT:tpid>
 */
cparser_result_t
cparser_cmd_svlan_set_tpid_entry_index_tpid_tpid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *tpid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 tpid;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEntry_set(*index_ptr, *tpid_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_tpid_entry_index_tpid_tpid */

/*
 * svlan set tpid entry <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_svlan_set_tpid_entry_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;
    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_tpidEnable_set(*index_ptr, enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_tpid_entry_index_state_disable_enable */


/*
 * svlan get trap-priority
 */
cparser_result_t
cparser_cmd_svlan_get_trap_priority(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_trapPri_get(&priority), ret);

    diag_util_mprintf("Trap priority: %d\n", priority);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_trap_priority */

/*
 * svlan set trap-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_svlan_set_trap_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 priority;
    
    DIAG_UTIL_PARAM_CHK();
    
    priority = *priority_ptr;

    DIAG_UTIL_ERR_CHK(rtk_svlan_trapPri_set(priority), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_trap_priority_priority */

/*
 * svlan get state
 */
cparser_result_t
cparser_cmd_svlan_get_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK((rtk_svlan_svlanFunctionEnable_get(&enable)), ret);

    diag_util_mprintf("svlan state: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_state */

/*
 * svlan set state ( enable | disable )
 */
cparser_result_t
cparser_cmd_svlan_set_state_enable_disable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(3,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK((rtk_svlan_svlanFunctionEnable_set(enable)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_state_enable_disable */

/*
 * svlan set dei-keep state ( disable | enable ) 
 */
cparser_result_t
cparser_cmd_svlan_set_dei_keep_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t en = DISABLED;
    
    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        en = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        en = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_deiKeepState_set(en), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_dei_keep_state_disable_enable */

/*
 * svlan get dei-keep state 
 */
cparser_result_t
cparser_cmd_svlan_get_dei_keep_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t en;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_deiKeepState_get(&en), ret);

    diag_util_mprintf("Keep DEI state: %s\n",diagStr_enable[en]);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_dei_keep_state */

/*
 * svlan get untag
 */
cparser_result_t
cparser_cmd_svlan_get_untag(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_action_t action;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_untagAction_get(&action, &svid), ret);

    diag_util_mprintf("Action: %s ", diagStr_svlanAction[action]);
    if(action == SVLAN_ACTION_SVLAN)
    {
        diag_util_mprintf("SVID: %d\n", svid);
    }
    else
    {
        diag_util_mprintf("\n");
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_untag */

/*
 * svlan set untag ( drop | trap-to-cpu | port-based-svid )
) 
 */
cparser_result_t
cparser_cmd_svlan_set_untag_drop_trap_to_cpu_port_based_svid(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_action_t action = SVLAN_ACTION_DROP;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();

    svid = 1;
    if ('d' == TOKEN_CHAR(3,0))
    {
        action = SVLAN_ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        action = SVLAN_ACTION_TRAP;
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        action = SVLAN_ACTION_PSVID;
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_untagAction_set(action, svid), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_untag_drop_trap_to_cpu */

/*
 * svlan set untag assign-svlan svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_set_untag_assign_svlan_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_action_t action;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();

    svid = *svid_ptr;
    action = SVLAN_ACTION_SVLAN;

    DIAG_UTIL_ERR_CHK(rtk_svlan_untagAction_set(action, svid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_untag_assign_svlan_svid_svid */

#if (!defined(CONFIG_REDUCED_DIAG))

/*
 * svlan get unmatch
 */
cparser_result_t
cparser_cmd_svlan_get_unmatch(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_action_t action;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_unmatchAction_get(&action, &svid), ret);

    diag_util_mprintf("Action: %s ", diagStr_svlanAction[action]);
    if(action == SVLAN_ACTION_SVLAN)
    {
        diag_util_mprintf("SVID: %d\n", svid);
    }
    else
    {
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_unmatch */

/*
 * svlan set unmatch ( drop | trap-to-cpu ) 
 */
cparser_result_t
cparser_cmd_svlan_set_unmatch_drop_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_action_t action = SVLAN_ACTION_DROP;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();

    svid = 1;
    if ('d' == TOKEN_CHAR(3,0))
    {
        action = SVLAN_ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        action = SVLAN_ACTION_TRAP;
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_unmatchAction_set(action, svid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_unmatch_drop_trap_to_cpu */

/*
 * svlan set unmatch assign-svlan svid <UINT:svid> 
 */
cparser_result_t
cparser_cmd_svlan_set_unmatch_assign_svlan_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_action_t action;
    rtk_vlan_t svid;
    
    DIAG_UTIL_PARAM_CHK();

    svid = *svid_ptr;
    action = SVLAN_ACTION_SVLAN;

    DIAG_UTIL_ERR_CHK(rtk_svlan_unmatchAction_set(action, svid), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_unmatch_assign_svlan_svid_svid */


/*
 * svlan get vlan-aggregation port ( <PORT_LIST:ports> | all ) state 
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_aggregation_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Status\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_dmacVidSelState_get(port, &enable), ret);
        diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_aggregation_port_ports_all_state */

/*
 * svlan set vlan-aggregation port ( <PORT_LIST:ports> | all ) state ( disable | enable ) 
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_aggregation_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_dmacVidSelState_set(port, enable), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_aggregation_port_ports_all_state_disable_enable */


/*
 * svlan set lookup-type ( svlan-table | vlan-4k-table ) 
 */
cparser_result_t
cparser_cmd_svlan_set_lookup_type_svlan_table_vlan_4k_table(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_lookupType_t type = SVLAN_LOOKUP_C4KVLAN;
    
    DIAG_UTIL_PARAM_CHK();

    if ('s' == TOKEN_CHAR(3,0))
    {
        type = SVLAN_LOOKUP_S64MBRCGF;
    }
    else if ('v' == TOKEN_CHAR(3,0))
    {
        type = SVLAN_LOOKUP_C4KVLAN;
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_lookupType_set(type), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_lookup_type_svlan_table_vlan_4k_table */

/*
 * svlan get lookup-type 
 */
cparser_result_t
cparser_cmd_svlan_get_lookup_type(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_svlan_lookupType_t type;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_lookupType_get(&type), ret);

    diag_util_mprintf("Lookup type: %s\n", diagStr_svlanLookup[type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_lookup_type */

/*
 * svlan get vlan-conversion sp2c unmatch-action
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_conversion_sp2c_unmatch_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t type;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_svlan_sp2cUnmatchCtagging_get(&type), ret);

    diag_util_mprintf("SP2C unmatching C-tag format: %s\n", diagStr_svlanSp2cUnmatch[type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_conversion_sp2c_unmatch_action */

/*
 * svlan set vlan-conversion sp2c unmatch-action ( untag | ctag ) 
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_conversion_sp2c_unmatch_action_untag_ctag(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t type = DISABLED;

    if ('u' == TOKEN_CHAR(5,0))
    {
        type = DISABLED;
    }
    else if ('c' == TOKEN_CHAR(5,0))
    {
        type = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_svlan_sp2cUnmatchCtagging_set(type), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_conversion_sp2c_unmatch_action_untag_ctag */

/*
 * svlan get vlan-aggregation forced-state 
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_aggregation_forced_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


	DIAG_UTIL_ERR_CHK(rtk_svlan_dmacVidSelForcedState_get(&enable), ret);
	diag_util_mprintf("%s\n", diagStr_enable[enable]);
#if 0
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_FORCED_DMACVIDSELf, &enable), ret);
		    diag_util_mprintf("%s\n", diagStr_enable[enable]);
			break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_aggregation_forced_state */

/*
 * svlan set vlan-aggregation forced-state ( disable | enable ) 
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_aggregation_forced_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
	
    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

	DIAG_UTIL_ERR_CHK(rtk_svlan_dmacVidSelForcedState_set(enable), ret);
	
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_aggregation_forced_state_disable_enable */

/*
 * svlan get vlan-aggregation vlan-type
 */
cparser_result_t
cparser_cmd_svlan_get_vlan_aggregation_vlan_type(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t enable;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
			case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_DMAC_CVID_CTRLf, &enable), ret);
            diag_util_mprintf("DMAC to cvid function for %s\n",enable?"SVL only":"both IVL and SVL");

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_vlan_aggregation_vlan_type */

/*
 * svlan set vlan-aggregation vlan-type ( svl-only | ivl-and-svl ) 
 */
cparser_result_t
cparser_cmd_svlan_set_vlan_aggregation_vlan_type_svl_only_ivl_and_svl(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t enable;
	DIAG_UTIL_PARAM_CHK();
	
	if ('i' == TOKEN_CHAR(4,0))
	{
		enable = DISABLED;
	}
	else if ('s' == TOKEN_CHAR(4,0))
	{
		enable = ENABLED;
	}
	
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
		case APOLLOMP_CHIP_ID:
				
			DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_DMAC_CVID_CTRLf, &enable), ret);

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_vlan_aggregation_vlan_type_svl_only_ivl_and_svl */

/*
 * svlan get ds-unctag-cvid 
 */
cparser_result_t
cparser_cmd_svlan_get_ds_unctag_cvid(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t enable;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
			case APOLLOMP_CHIP_ID:
			
			DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_DS_IGRCVIDf, &enable), ret);
            diag_util_mprintf("%s\n",enable?"As Ingress CVID":"CVLAN unawared");
			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_ds_unctag_cvid */

/*
 * svlan set ds-unctag-cvid ( no-cvlan | ingress-cvid ) 
 */
cparser_result_t
cparser_cmd_svlan_set_ds_unctag_cvid_no_cvlan_ingress_cvid(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t enable;
	DIAG_UTIL_PARAM_CHK();
	
	if ('n' == TOKEN_CHAR(3,0))
	{
		enable = DISABLED;
	}
	else if ('i' == TOKEN_CHAR(3,0))
	{
		enable = ENABLED;
	}
	
	switch(DIAG_UTIL_CHIP_TYPE)
	{
#ifdef CONFIG_SDK_APOLLOMP 
		case APOLLOMP_CHIP_ID:
				
			DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_SVLAN_CTRLr, APOLLOMP_VS_DS_IGRCVIDf, &enable), ret);

			break;
#endif    
		default:
			diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
			return CPARSER_NOT_OK;		  
			break;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_ds_unctag_cvid_no_cvlan_ingress_cvid */

/*
 * svlan get ext-pvid port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_svlan_get_ext_pvid_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_vlan_t pvid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_extPortPvid_get(port, &pvid), ret);
        diag_util_printf("\n EXT Port %d PVID: %d", port, pvid);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_get_ext_pvid_port_ports_all */

/*
 * svlan set ext-pvid port ( <PORT_LIST:ports> | all ) svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_svlan_set_ext_pvid_port_ports_all_svid_svid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *svid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_EXTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_svlan_extPortPvid_set(port, (rtk_vlan_t)*svid_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_set_ext_pvid_port_ports_all_svid_svid */


#endif

