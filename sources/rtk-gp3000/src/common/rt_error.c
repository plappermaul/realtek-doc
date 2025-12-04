/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 * Purpose : Realtek Switch SDK Error Code Module 
 * 
 * Feature : The file have include the following module and sub-modules
 *           1) Error code for SDK
 * 
 */

/*
 * Include Files
 */
#include <common/rt_error.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rt_error_numToStr
 * Description:
 *      Convert error number to readable string.
 * Input:
 *      err_num --- error number
 * Output:
 *      None.
 * Return:
 *      error string
 * Note:
 *      None.
 */

const uint8 *rt_error_numToStr(int32 err_num)
{
    
    switch(err_num)
	{
    	case RT_ERR_FAILED:
    		return "General Error";
    		
    	case RT_ERR_OK:
    	    return "OK";
    		
    	/* for common error code */	
    	case RT_ERR_INPUT:
    		return "Invalid input parameter";

    	case RT_ERR_UNIT_ID:
    		return "Invalid unit id";

    	case RT_ERR_PORT_ID:
    		return "Invalid port id";

    	case RT_ERR_PORT_MASK:
    		return "Invalid port mask";

    	case RT_ERR_PORT_LINKDOWN:
    		return "Link down port status";

    	case RT_ERR_ENTRY_INDEX:
    		return "Invalid entry index";

    	case RT_ERR_NULL_POINTER:
    		return "Input parameter is null pointer";

    	case RT_ERR_QUEUE_ID:
    		return "Invalid queue id";

    	case RT_ERR_QUEUE_NUM:
    		return "Invalid queue number";

    	case RT_ERR_BUSYWAIT_TIMEOUT:
    		return "Busy watting time out";

    	case RT_ERR_MAC:
    		return "Invalid MAC address";

    	case RT_ERR_OUT_OF_RANGE:
    		return "Input parameter is out of range";

    	case RT_ERR_CHIP_NOT_SUPPORTED:
    		return "Function is not supported by this chip model";

    	case RT_ERR_SMI:
    		return "SMI error";

    	case RT_ERR_NOT_INIT:
    		return "The module is not initial";

    	case RT_ERR_CHIP_NOT_FOUND:
    		return "The chip is not found";

    	case RT_ERR_NOT_ALLOWED:
    		return "Action is not allowed by the function";

    	case RT_ERR_DRIVER_NOT_FOUND:
    		return "The driver is not found";

    	case RT_ERR_PRIORITY:
    		return "Invalid priority";

    	case RT_ERR_ENTRY_NOTFOUND:
    		return "No such entry";
    		
    	case RT_ERR_ENTRY_FULL:
    		return "Entry is full";

    	case RT_ERR_FEATURE_NOT_SUPPORTED:
    		return "Feature not supported";

    	case RT_ERR_PORT_LINKUP:
    		return "Link up port status";
    	
    	/* for VLAN */	
    	case RT_ERR_VLAN_VID:
    		return "Invalid vid";

    	case RT_ERR_VLAN_PRIORITY:
    		return "Invalid 1p priority";

    	case RT_ERR_VLAN_EMPTY_ENTRY:
    		return "Empty entry of VLAN table";

    	case RT_ERR_VLAN_ACCEPT_FRAME_TYPE:
    		return "Invalid accept frame type";

    	case RT_ERR_VLAN_EXIST:
    		return "VLAN exists";

    	case RT_ERR_VLAN_ENTRY_NOT_FOUND:
    		return "Specified VLAN entry is not found";

    	case RT_ERR_VLAN_PORT_MBR_EXIST:
    		return "Member port exist in the specified VLAN";

        case RT_ERR_VLAN_PROTO_AND_PORT:
            return "Envalid protocol base group database index";
            
    	/* for SVLAN */	
    	case RT_ERR_SVLAN_ENTRY_INDEX:
    		return "Invalid svid entry index";

    	case RT_ERR_SVLAN_ETHER_TYPE:
    		return "Invalid SVLAN ether type";

    	case RT_ERR_SVLAN_TABLE_FULL:
    		return "No empty entry in SVLAN table";

    	case RT_ERR_SVLAN_ENTRY_NOT_FOUND:
    		return "Specified SVLAN entry is not found";

    	case RT_ERR_SVLAN_EXIST:
    		return "SVLAN entry exists";

    	case RT_ERR_SVLAN_VID:
    		return "Invalid svid";
            
    	
    	/* for MSTP */	
    	case RT_ERR_MSTI:
    		return "Invalid MSTI";

    	case RT_ERR_MSTP_STATE:
    		return "Invalid spanning tree status";

    	case RT_ERR_MSTI_EXIST:
    		return "MSTI exists";

    	case RT_ERR_MSTI_NOT_EXIST:
    		return "MSTI does not exist";
    	
    	/* for BUCKET */	
    	case RT_ERR_TIMESLOT:
    		return "Invalid time slot";

    	case RT_ERR_TOKEN:
    		return "Invalid token amount";

    	case RT_ERR_RATE:
    		return "Invalid rate";

    	/* for RMA */	
    	case RT_ERR_RMA_ADDR:
    		return "Invalid RMA MAC address";

    	case RT_ERR_RMA_ACTION:
    		return "Invalid RMA action";
    	
    	/* for L2 */	
    	case RT_ERR_L2_HASH_KEY:
    		return "Invalid L2 Hash key";

    	case RT_ERR_L2_HASH_INDEX:
    		return "Invalid L2 Hash index";

    	case RT_ERR_L2_CAM_INDEX:
    		return "Invalid L2 CAM index";

    	case RT_ERR_L2_ENRTYSEL:
    		return "Invalid entry select";

    	case RT_ERR_L2_INDEXTABLE_INDEX:
    		return "Invalid L2 index table(=portMask table) index";

    	case RT_ERR_LIMITED_L2ENTRY_NUM:
    		return "Invalid limited L2 entry number";

    	case RT_ERR_L2_AGGREG_PORT:
    		return "This aggregated port is not the lowest physical port of its aggregation group";

    	case RT_ERR_L2_FID:
    		return "Invalid fid";

    	case RT_ERR_L2_RVID:
    		return "Invalid cvid";

    	case RT_ERR_L2_NO_EMPTY_ENTRY:
    		return "No empty entry in L2 table";

    	case RT_ERR_L2_ENTRY_NOTFOUND:
    		return "Specified entry is not found";

    	case RT_ERR_L2_INDEXTBL_FULL:
    		return "The L2 index table is full";

    	case RT_ERR_L2_INVALID_FLOWTYPE:
    		return "Invalid L2 flow type";

    	case RT_ERR_L2_L2UNI_PARAM:
    		return "Invalid L2 unicast parameter";

    	case RT_ERR_L2_L2MULTI_PARAM:
    		return "Invalid L2 multicast parameter";

    	case RT_ERR_L2_IPMULTI_PARAM:
    		return "Invalid L2 ip multicast parameter";

    	case RT_ERR_L2_PARTIAL_HASH_KEY:
    		return "Invalid L2 partial Hash key";

    	case RT_ERR_L2_EMPTY_ENTRY:
    		return "The entry is empty(invalid)";

    	case RT_ERR_L2_FLUSH_TYPE:
    		return "The flush type is invalid";

    	case RT_ERR_L2_NO_CPU_PORT:
    		return "CPU port does not exist";

        case RT_ERR_L2_SIP_INDEX:
            return "Invalid SIP filter table index";
    	
    	/* for FILTER (PIE) */	
    	case RT_ERR_FILTER_BLOCKNUM:
    		return "Invalid block number";

    	case RT_ERR_FILTER_ENTRYIDX:
    		return "Invalid entry index";

    	case RT_ERR_FILTER_CUTLINE:
    		return "Invalid cutline value";

    	case RT_ERR_FILTER_FLOWTBLBLOCK:
    		return "Block belongs to flow table";

    	case RT_ERR_FILTER_INACLBLOCK:
    		return "Block belongs to ingress ACL";

    	case RT_ERR_FILTER_ACTION:
    		return "Action doesn't consist to entry type";

    	case RT_ERR_FILTER_INACL_RULENUM:
    		return "Invalid ACL rule number";

    	case RT_ERR_FILTER_INACL_TYPE:
    		return "Entry type isn't an ingress ACL rule";

    	case RT_ERR_FILTER_INACL_EXIST:
    		return "ACL entry already exists";

    	case RT_ERR_FILTER_INACL_EMPTY:
    		return "ACL entry is empty";

    	case RT_ERR_FILTER_FLOWTBL_TYPE:
    		return "Entry type isn't an flow table rule";

    	case RT_ERR_FILTER_FLOWTBL_RULENUM:
    		return "Invalid flow table rule number";

    	case RT_ERR_FILTER_FLOWTBL_EMPTY:
    		return "Flow table entry is empty";

    	case RT_ERR_FILTER_FLOWTBL_EXIST:
    		return "Flow table entry already exists";

    	case RT_ERR_FILTER_METER_ID:
    		return "Invalid metering id";

    	case RT_ERR_FILTER_LOG_ID:
    		return "Invalid log id";
    	
    	case RT_ERR_PIE_FIELD_TYPE:
    	    return "Invalid pie field type";
    	
    	case RT_ERR_PIE_PHASE:
    	    return "Invalid pie phase";
    	    
    	case RT_ERR_PIE_PHASE_NOT_SUPPORTED:
            return "pie phase not supported in the chip";

    	/* for ACL Rate Limit */	
    	case RT_ERR_ACLRL_HTHR:
    		return "Invalid high threshold";

    	case RT_ERR_ACLRL_TIMESLOT:
    		return "Invalid time slot";

    	case RT_ERR_ACLRL_TOKEN:
    		return "Invalid token amount";

    	case RT_ERR_ACLRL_RATE:
    		return "Invalid rate";
    	
    	/* for Link aggregation */	
    	case RT_ERR_LA_CPUPORT:
    		return "CPU port can not be aggregated port";

    	case RT_ERR_LA_TRUNK_ID:
    		return "Invalid trunk id";

    	case RT_ERR_LA_PORTMASK:
    		return "Invalid port mask";

    	case RT_ERR_LA_HASHMASK:
    		return "Invalid hash mask";

    	case RT_ERR_LA_DUMB:
    		return "This API should be used in 802.1ad dumb mode";

    	case RT_ERR_LA_PORTNUM_DUMB:
    		return "It can only aggregate at most four ports when 802.1ad dumb mode";

    	case RT_ERR_LA_PORTNUM_NORMAL:
    		return "It can only aggregate at most eight ports when 802.1ad normal mode";

    	case RT_ERR_LA_MEMBER_OVERLAP:
    		return "The specified port mask is overlapped with other group";

    	case RT_ERR_LA_NOT_MEMBER_PORT:
    		return "The port is not a member port of the trunk";

    	case RT_ERR_LA_TRUNK_NOT_EXIST:
    		return "The trunk doesn't exist";

    	/* for storm filter */	
    	case RT_ERR_SFC_TICK_PERIOD:
    		return "Invalid SFC tick period";

    	case RT_ERR_SFC_UNKNOWN_GROUP:
    		return "Unknown storm filter group";

    	
    	/* for pattern match */	
    	case RT_ERR_PM_MASK:
    		return "Invalid pattern length. Pattern length should be 8";

    	case RT_ERR_PM_LENGTH:
    		return "Invalid pattern match mask, first byte must care";

    	case RT_ERR_PM_MODE:
    		return "Invalid pattern match mode";
    	
    	/* for input bandwidth control */	
    	case RT_ERR_INBW_TICK_PERIOD:
    		return "Invalid tick period for input bandwidth control";

    	case RT_ERR_INBW_TOKEN_AMOUNT:
    		return "Invalid amount of token for input bandwidth control";

    	case RT_ERR_INBW_FCON_VALUE:
    		return "Invalid flow control ON threshold value for input bandwidth control";

    	case RT_ERR_INBW_FCOFF_VALUE:
    		return "Invalid flow control OFF threshold value for input bandwidth control";

    	case RT_ERR_INBW_FC_ALLOWANCE:
    		return "Invalid allowance of incomming packet for input bandwidth control";

    	case RT_ERR_INBW_RATE:
    		return "Invalid input bandwidth";
    	
    	/* for QoS */	
    	case RT_ERR_QOS_1P_PRIORITY:
    		return "Invalid 802.1P priority";

    	case RT_ERR_QOS_DSCP_VALUE:
    		return "Invalid DSCP value";

    	case RT_ERR_QOS_INT_PRIORITY:
    		return "Invalid internal priority";

    	case RT_ERR_QOS_SEL_DSCP_PRI:
    		return "Invalid DSCP selection priority";

    	case RT_ERR_QOS_SEL_PORT_PRI:
    		return "Invalid port selection priority";

    	case RT_ERR_QOS_SEL_IN_ACL_PRI:
    		return "Invalid ingress ACL selection priority";

    	case RT_ERR_QOS_SEL_CLASS_PRI:
    		return "Invalid classifier selection priority";

    	case RT_ERR_QOS_EBW_RATE:
    		return "Invalid egress bandwidth rate";

    	case RT_ERR_QOS_SCHE_TYPE:
    		return "Invalid QoS scheduling type";

    	case RT_ERR_QOS_QUEUE_WEIGHT:
    		return "Invalid queue weight";
    	
    	/* for port ability */	
    	case RT_ERR_PHY_PAGE_ID:
    		return "Invalid PHY page id";

    	case RT_ERR_PHY_REG_ID:
    		return "Invalid PHY register id";

    	case RT_ERR_PHY_DATAMASK:
    		return "Invalid PHY data mask";

    	case RT_ERR_PHY_AUTO_NEGO_MODE:
    		return "Invalid PHY auto-negotiation mode";

    	case RT_ERR_PHY_SPEED:
    		return "Invalid PHY speed setting";

    	case RT_ERR_PHY_DUPLEX:
    		return "Invalid PHY duplex setting";

    	case RT_ERR_PHY_FORCE_ABILITY:
    		return "Invalid PHY force mode ability parameter";

    	case RT_ERR_PHY_FORCE_1000:
    		return "Invalid PHY force mode 1G speed setting";

    	case RT_ERR_PHY_TXRX:
    		return "Invalid PHY tx/rx";
    	
    	case RT_ERR_PHY_RTCT_NOT_FINISH:
    	    return "PHY RTCT in progress";

    	case RT_ERR_PHY_RTCT_TIMEOUT:
    	    return "PHY RTCT timeout";

    	case RT_ERR_PHY_AUTO_ABILITY:
    	    return "Invalid PHY auto mode ability parameter";
    	
    	case RT_ERR_PHY_FIBER_LINKUP:
    	    return "Access copper PHY is not permit when fiber is linkup";
    	
    	/* for mirror */	
    	case RT_ERR_MIRROR_DIRECTION:
    		return "Invalid error mirror direction";

    	case RT_ERR_MIRROR_SESSION_FULL:
    		return "Mirroring session is full";

    	case RT_ERR_MIRROR_SESSION_NOEXIST:
    		return "Mirroring session does not exist";

    	case RT_ERR_MIRROR_PORT_EXIST:
    		return "Mirroring port already exists";

    	case RT_ERR_MIRROR_PORT_NOT_EXIST:
    		return "Mirroring port does not exist";

    	case RT_ERR_MIRROR_PORT_FULL:
    		return "Exceeds maximum number of supported mirroring port";
    	
    	/* for stat */
    	case RT_ERR_STAT_INVALID_GLOBAL_CNTR:
    		return "Invalid global counter";

    	case RT_ERR_STAT_INVALID_PORT_CNTR:
    		return "Invalid port counter";

    	case RT_ERR_STAT_GLOBAL_CNTR_FAIL:
    		return "Can't retrieve/reset global counter";

    	case RT_ERR_STAT_PORT_CNTR_FAIL:
    		return "Can't retrieve/reset port counter";
    	
    	/* for dot1x */	
    	case RT_ERR_DOT1X_INVALID_DIRECTION:
    		return "Invalid authentication direction";

        case RT_ERR_DOT1X_PROC:
            return "Unauthorized behavior error";

        case RT_ERR_DOT1X_GVLANIDX:
            return "Invalid cvid index";

        /* for GPON */	
        case RT_ERR_GPON_INITIAL_FAIL:
                return "GPON driver initialize fail";

        case RT_ERR_GPON_INVALID_HANDLE:
    		return "GPON driver invalid handle";

        case RT_ERR_GPON_DRV_NOT_STARTED:
    		return "GPON driver is not started";

        case RT_ERR_GPON_DRV_ALREADY_STARTED:
    		return "GPON driver is already started";

        case RT_ERR_GPON_ONU_ALREADY_REGISTERED:             
    		return "ONU is already registered";

        case RT_ERR_GPON_ONU_NOT_REGISTERED:
    		return "ONU is not registered";

        case RT_ERR_GPON_ONU_ALREADY_ACTIVATED:
    		return "GPON Device has already been activeted";

        case RT_ERR_GPON_ONU_NOT_ACTIVATED:
    		return "GPON Device has not been activeted";

        case RT_ERR_GPON_EXCEED_MAX_TCONT:
    		return "Exceed maximum number of the tcont";

        case RT_ERR_GPON_EXCEED_MAX_FLOW:
    		return "Exceed maximum number of the GEM flow";

        case RT_ERR_GPON_PLOAM_QUEUE_FULL:
    		return "PLOAM queue is full";

        case RT_ERR_GPON_PLOAM_QUEUE_EMPTY:
    		return "PLOAM queue is empty";

        case RT_ERR_GPON_OMCI_QUEUE_FULL:
    		return "OMCI queue is full";

        case RT_ERR_GPON_OMCI_QUEUE_EMPTY:
    		return "OMCI queue is empty";

        case RT_ERR_GPON_TABLE_ENTRY_NOT_FOUND:
    		return "The entry is not found in GPON table";

        case RT_ERR_GPON_MAC_FILTER_TABLE_FULL:
    		return "The MAC filter table is full";

        case RT_ERR_GPON_MAC_FILTER_TABLE_ALLOC_FAIL:
    		return "The MAC filter table entry allocate fail";

    	default:
    		return "Unknown error code";
	}
}

