/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Realtek Switch SDK Error Code Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Error code for SDK
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/lib.h>

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
 *      err_num - error number
 * Output:
 *      None.
 * Return:
 *      error string
 * Note:
 *      None.
 */
const uint8 *rt_error_numToStr(int32 err_num)
{
    switch (err_num)
    {
        case RT_ERR_FAILED:
            return (uint8 *)"General Error";

        case RT_ERR_OK:
            return (uint8 *)"OK";

        /* for common error code */
        case RT_ERR_INPUT:
            return (uint8 *)"Invalid input parameter";

        case RT_ERR_UNIT_ID:
            return (uint8 *)"Invalid unit id";

        case RT_ERR_PORT_ID:
            return (uint8 *)"Invalid port id";

        case RT_ERR_TRUNK_ID:
            return (uint8 *)"Invalid trunk id";

        case RT_ERR_PORT_MASK:
            return (uint8 *)"Invalid port mask";

        case RT_ERR_PORT_LINKDOWN:
            return (uint8 *)"Link down port status";

        case RT_ERR_ENTRY_INDEX:
            return (uint8 *)"Invalid entry index";

        case RT_ERR_NULL_POINTER:
            return (uint8 *)"Input parameter is null pointer";

        case RT_ERR_QUEUE_ID:
            return (uint8 *)"Invalid queue id";

        case RT_ERR_QUEUE_NUM:
            return (uint8 *)"Invalid queue number";

        case RT_ERR_BUSYWAIT_TIMEOUT:
            return (uint8 *)"Busy watting time out";

        case RT_ERR_MAC:
            return (uint8 *)"Invalid MAC address";

        case RT_ERR_OUT_OF_RANGE:
            return (uint8 *)"Input parameter is out of range";

        case RT_ERR_CHIP_NOT_SUPPORTED:
            return (uint8 *)"Function is not supported by this chip model";

        case RT_ERR_PORT_NOT_SUPPORTED:
            return (uint8 *)"Function is not supported by this port";

        case RT_ERR_SMI:
            return (uint8 *)"SMI error";

        case RT_ERR_NOT_INIT:
            return (uint8 *)"The module is not initial";

        case RT_ERR_CHIP_NOT_FOUND:
            return (uint8 *)"The chip is not found";

        case RT_ERR_NOT_ALLOWED:
            return (uint8 *)"Action is not allowed by the function";

        case RT_ERR_DRIVER_NOT_FOUND:
            return (uint8 *)"The driver is not found";

        case RT_ERR_TBL_FULL:
            return (uint8 *)"The table is full";

        case RT_ERR_ENTRY_EXIST:
            return (uint8 *)"The entry already exists";

        case RT_ERR_ENTRY_NOTFOUND:
            return (uint8 *)"The entry is not found";

        case RT_ERR_EXCEEDS_CAPACITY:
            return (uint8 *)"Exceeds the hardware capacity";

        case RT_ERR_ENTRY_REFERRED:
            return (uint8 *)"Entry is still being referred";

        case RT_ERR_OPER_DENIED:
            return (uint8 *)"Operation denied";

        case RT_ERR_SOCKET:
            return (uint8 *)"System socket error";

        case RT_ERR_MEM_ALLOC:
            return (uint8 *)"Insufficient memory is available";

        case RT_ERR_ABORT:
            return (uint8 *)"Operation aborted";

        case RT_ERR_DEV_ID:
            return (uint8 *)"Invalid device id";

        case RT_ERR_DRIVER_NOT_SUPPORTED:
            return (uint8 *)"Function is not supported by this driver";

        case RT_ERR_NOT_SUPPORTED:
            return (uint8 *)"Function is not supported";

        /* for VLAN */
        case RT_ERR_VLAN_VID:
            return (uint8 *)"Invalid vid";

        case RT_ERR_VLAN_PRIORITY:
            return (uint8 *)"Invalid 1p priority";

        case RT_ERR_VLAN_EMPTY_ENTRY:
            return (uint8 *)"Empty entry of VLAN table";

        case RT_ERR_VLAN_ACCEPT_FRAME_TYPE:
            return (uint8 *)"Invalid accept frame type";

        case RT_ERR_VLAN_EXIST:
            return (uint8 *)"VLAN exists";

        case RT_ERR_VLAN_ENTRY_NOT_FOUND:
            return (uint8 *)"Specified VLAN entry is not found";

        case RT_ERR_VLAN_PORT_MBR_EXIST:
            return (uint8 *)"Member port exist in the specified VLAN";
        case RT_ERR_VLAN_FRAME_TYPE:
            return (uint8 *)"Invalid frame type";
        case RT_ERR_VLAN_TPID_INDEX:
            return (uint8 *)"Invalid TPID index";
        case RT_ERR_VLAN_C2SC_BLOCK_MODE:
            return (uint8 *)"Try to add a entry to a incompatiable block, change block mode first";
        case RT_ERR_VLAN_C2SC_BLOCK_DISABLED:
            return (uint8 *)"Ingress VLAN conversion block is disabled, enables it first";

        /* for MSTP */
        case RT_ERR_MSTI:
            return (uint8 *)"Invalid MSTI";

        case RT_ERR_MSTP_STATE:
            return (uint8 *)"Invalid spanning tree status";

        case RT_ERR_MSTI_EXIST:
            return (uint8 *)"MSTI exists";

        case RT_ERR_MSTI_NOT_EXIST:
            return (uint8 *)"MSTI does not exist";

        /* for BUCKET */
        case RT_ERR_TIMESLOT:
            return (uint8 *)"Invalid time slot";

        case RT_ERR_TOKEN:
            return (uint8 *)"Invalid token amount";

        case RT_ERR_RATE:
            return (uint8 *)"Invalid rate";

        /* for RMA */
        case RT_ERR_RMA_ADDR:
            return (uint8 *)"Invalid RMA MAC address";

        case RT_ERR_RMA_ACTION:
            return (uint8 *)"Invalid RMA action";

        case RT_ERR_RMA_MGMT_TYPE:
            return (uint8 *)"Invalid type of management frame";

        case RT_ERR_RMA_NOT_SUPPORT_GLOBAL:
            return (uint8 *)"This RMA is not supported in global configuration";

        case RT_ERR_RMA_NOT_SUPPORT_PORT:
            return (uint8 *)"This RMA is not supported in per-port configuration";

        /* for L2 */
        case RT_ERR_L2_HASH_KEY:
            return (uint8 *)"Invalid L2 Hash key";

        case RT_ERR_L2_HASH_INDEX:
            return (uint8 *)"Invalid L2 Hash index";

        case RT_ERR_L2_CAM_INDEX:
            return (uint8 *)"Invalid L2 CAM index";

        case RT_ERR_L2_ENRTYSEL:
            return (uint8 *)"Invalid entry select";

        case RT_ERR_L2_INDEXTABLE_INDEX:
            return (uint8 *)"Invalid L2 index table(=portMask table) index";

        case RT_ERR_LIMITED_L2ENTRY_NUM:
            return (uint8 *)"Invalid limited L2 entry number";

        case RT_ERR_L2_AGGREG_PORT:
            return (uint8 *)"This aggregated port is not the lowest physical port of its aggregation group";

        case RT_ERR_L2_FID:
            return (uint8 *)"Invalid fid";

        case RT_ERR_L2_RVID:
            return (uint8 *)"Invalid cvid";

        case RT_ERR_L2_NO_EMPTY_ENTRY:
            return (uint8 *)"No empty entry in L2 table";

        case RT_ERR_L2_ENTRY_NOTFOUND:
            return (uint8 *)"Specified entry is not found";

        case RT_ERR_L2_INDEXTBL_FULL:
            return (uint8 *)"The L2 index table is full";

        case RT_ERR_L2_INVALID_FLOWTYPE:
            return (uint8 *)"Invalid L2 flow type";

        case RT_ERR_L2_L2UNI_PARAM:
            return (uint8 *)"Invalid L2 unicast parameter";

        case RT_ERR_L2_L2MULTI_PARAM:
            return (uint8 *)"Invalid L2 multicast parameter";

        case RT_ERR_L2_IPMULTI_PARAM:
            return (uint8 *)"Invalid L2 ip multicast parameter";

        case RT_ERR_L2_PARTIAL_HASH_KEY:
            return (uint8 *)"Invalid L2 partial Hash key";

        case RT_ERR_L2_EMPTY_ENTRY:
            return (uint8 *)"The entry is empty(invalid)";

        case RT_ERR_L2_FLUSH_TYPE:
            return (uint8 *)"The flush type is invalid";

        case RT_ERR_L2_NO_CPU_PORT:
            return (uint8 *)"CPU port does not exist";
        case RT_ERR_L2_MULTI_FWD_INDEX:
            return (uint8 *)"Invalid index of multicast forwarding portmask";
        case RT_ERR_L2_ENTRY_EXIST:
            return (uint8 *)"Entry already exist";
        case RT_ERR_L2_EXCEPT_ADDR_TYPE:
            return (uint8 *)"Invalid exception address type";
        case RT_ERR_L2_MCAST_FWD_ENTRY_EXIST:
            return (uint8 *)"Mcast forwarding entry already exist";
        case RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST:
            return (uint8 *)"Mcast forwarding entry not exist";
        case RT_ERR_L2_IPMCAST_LOOKUP_MODE:
            return (uint8 *)"invalid IP multicast lookup mode";
        case RT_ERR_L2_PMSK_NOT_INIT:
            return (uint8 *)"Flooding portmask is not initialized";
        case RT_ERR_L2_IP6_CACHETBL_FULL:
            return (uint8 *)"The IPv6 cache table is full";
        case RT_ERR_L2_IP6_HASHKEY_EXIST:
            return (uint8 *)"The the same IPv6 hash key has already existed";
        case RT_ERR_L2_NEXTHOP_EXIST:
            return (uint8 *)"This is a nexthop entry";
        case RT_ERR_L2_NEXTHOP_NOT_EXIST:
            return (uint8 *)"Nexthop entry doesn't exist";
        case RT_ERR_L2_N_TO_1_NOT_ENABLED:
            return (uint8 *)"N:1 is not enabled";
        case RT_ERR_L2_PE_NOT_ENABLED:
            return (uint8 *)"Port Extension is not enabled";

        /* for FILTER/PIE/ACL */
        case RT_ERR_PIE_FIELD_TYPE:
            return (uint8 *)"invalid field type";
        case RT_ERR_PIE_PHASE:
            return (uint8 *)"Invalid pie phase";
        case RT_ERR_PIE_PHASE_NOT_SUPPORTED:
            return (uint8 *)"pie phase not supported in the chip";
        case RT_ERR_PIE_BLOCK_INDEX:
            return (uint8 *)"invalid block index";
        case RT_ERR_PIE_TEMPLATE_INDEX:
            return (uint8 *)"invalid template index";
        case RT_ERR_PIE_FIELD_LOCATION:
            return (uint8 *)"specify field type to invalid field location";
        case RT_ERR_PIE_CLEAR_INDEX:
            return (uint8 *)"end index is lower than start index";

        case RT_ERR_RANGE_CHECK_TYPE:
            return (uint8 *)"invalid Range Check type";

        case RT_ERR_ACL_FIELD_TYPE:
            return (uint8 *)"invalid ACL field type";
        case RT_ERR_ACL_TEMPLATE_INCOMPATIBLE:
            return (uint8 *)"try to map a ACL block to an incompatible template";
        case RT_ERR_ACL_CLEAR_INDEX:
            return (uint8 *)"end index is lower than start index";
        case RT_ERR_ACL_BLOCK_INDEX:
            return (uint8 *)"invalid block index";
        case RT_ERR_ACL_PHASE:
            return (uint8 *)"invalid ACL phase";

        #if defined(CONFIG_SDK_RTL8380)
            case RT_ERR_ACL_AGGR_2:
                return (uint8 *)"only support first 6 blocks(entry0~767)  ";
        #endif

      #if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380))
        case RT_ERR_ACL_BLOCK_POWER:
            return (uint8 *)"the block is power disabled";
      #endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)) */

        /* for Link aggregation */
        case RT_ERR_LA_CPUPORT:
            return (uint8 *)"CPU port can not be aggregated port";
        case RT_ERR_LA_TRUNK_ID:
            return (uint8 *)"Invalid trunk id";
        case RT_ERR_LA_PORTMASK:
            return (uint8 *)"Invalid port mask";
        case RT_ERR_LA_HASHMASK:
            return (uint8 *)"Invalid hash mask";
        case RT_ERR_LA_DUMB:
            return (uint8 *)"This API should be used in 802.1ad dumb mode";
        case RT_ERR_LA_PORTNUM_DUMB:
            return (uint8 *)"It can only aggregate at most four ports when 802.1ad dumb mode";
        case RT_ERR_LA_PORTNUM_NORMAL:
            return (uint8 *)"It can only aggregate at most eight ports when 802.1ad normal mode";
        case RT_ERR_LA_MEMBER_OVERLAP:
            return (uint8 *)"The specified port mask is overlapped with other group";
        case RT_ERR_LA_NOT_MEMBER_PORT:
            return (uint8 *)"The port is not a member port of the trunk";
        case RT_ERR_LA_TRUNK_NOT_EXIST:
            return (uint8 *)"The trunk doesn't exist";
        case RT_ERR_LA_ALGO_ID:
            return (uint8 *)"Invalid trunk algorithm id";
        case RT_ERR_LA_ALGO_SHIFT:
            return (uint8 *)"Invalid trunk algorithm shift";
        case RT_ERR_LA_STACK_TRUNK_ID:
            return (uint8 *)"Invalid stack trunk id";

        /* for storm filter */
        case RT_ERR_SFC_TICK_PERIOD:
            return (uint8 *)"Invalid SFC tick period";

        case RT_ERR_SFC_UNKNOWN_GROUP:
            return (uint8 *)"Unknown storm filter group";


        /* for pattern match */
        case RT_ERR_PM_MASK:
            return (uint8 *)"Invalid pattern length. Pattern length should be 8";

        case RT_ERR_PM_LENGTH:
            return (uint8 *)"Invalid pattern match mask, first byte must care";

        case RT_ERR_PM_MODE:
            return (uint8 *)"Invalid pattern match mode";

        /* for input bandwidth control */
        case RT_ERR_INBW_TICK_PERIOD:
            return (uint8 *)"Invalid tick period for input bandwidth control";

        case RT_ERR_INBW_TOKEN_AMOUNT:
            return (uint8 *)"Invalid amount of token for input bandwidth control";

        case RT_ERR_INBW_FCON_VALUE:
            return (uint8 *)"Invalid flow control ON threshold value for input bandwidth control";

        case RT_ERR_INBW_FCOFF_VALUE:
            return (uint8 *)"Invalid flow control OFF threshold value for input bandwidth control";

        case RT_ERR_INBW_FC_ALLOWANCE:
            return (uint8 *)"Invalid allowance of incomming packet for input bandwidth control";

        case RT_ERR_INBW_RATE:
            return (uint8 *)"Invalid input bandwidth";

        /* for QoS */
        case RT_ERR_QOS_1P_PRIORITY:
            return (uint8 *)"Invalid 802.1P priority";

        case RT_ERR_QOS_DSCP_VALUE:
            return (uint8 *)"Invalid DSCP value";

        case RT_ERR_QOS_INT_PRIORITY:
            return (uint8 *)"Invalid internal priority";

        case RT_ERR_QOS_SEL_DSCP_PRI:
            return (uint8 *)"Invalid DSCP selection priority";

        case RT_ERR_QOS_SEL_PORT_PRI:
            return (uint8 *)"Invalid port selection priority";

        case RT_ERR_QOS_SEL_IN_ACL_PRI:
            return (uint8 *)"Invalid ingress ACL selection priority";

        case RT_ERR_QOS_SEL_CLASS_PRI:
            return (uint8 *)"Invalid classifier selection priority";

        case RT_ERR_QOS_EBW_RATE:
            return (uint8 *)"Invalid egress bandwidth rate";

        case RT_ERR_QOS_SCHE_TYPE:
            return (uint8 *)"Invalid QoS scheduling type";

        case RT_ERR_QOS_QUEUE_WEIGHT:
            return (uint8 *)"Invalid queue weight";

        /* for port ability */
        case RT_ERR_PHY_PAGE_ID:
            return (uint8 *)"Invalid PHY page id";

        case RT_ERR_PHY_REG_ID:
            return (uint8 *)"Invalid PHY register id";

        case RT_ERR_PHY_DATAMASK:
            return (uint8 *)"Invalid PHY data mask";

        case RT_ERR_PHY_AUTO_NEGO_MODE:
            return (uint8 *)"Invalid PHY auto-negotiation mode";

        case RT_ERR_PHY_SPEED:
            return (uint8 *)"Invalid PHY speed setting";

        case RT_ERR_PHY_DUPLEX:
            return (uint8 *)"Invalid PHY duplex setting";

        case RT_ERR_PHY_FORCE_ABILITY:
            return (uint8 *)"Invalid PHY force mode ability parameter";

        case RT_ERR_PHY_FORCE_1000:
            return (uint8 *)"Invalid PHY force mode 1G speed setting";

        case RT_ERR_PHY_TXRX:
            return (uint8 *)"Invalid PHY tx/rx";

        case RT_ERR_PHY_RTCT_NOT_FINISH:
            return (uint8 *)"PHY RTCT in progress";

        case RT_ERR_PHY_RTCT_TIMEOUT:
            return (uint8 *)"PHY RTCT timeout";

        case RT_ERR_PHY_AUTO_ABILITY:
            return (uint8 *)"Invalid PHY auto mode ability parameter";

        case RT_ERR_PHY_FIBER_LINKUP:
            return (uint8 *)"Access copper PHY is not permit when fiber is linkup";
        case RT_ERR_PHY_ACCESS_FAIL:
            return (uint8 *)"Access PHY fail";

        /* for mirror */
        case RT_ERR_MIRROR_DIRECTION:
            return (uint8 *)"Invalid error mirror direction";

        case RT_ERR_MIRROR_SESSION_FULL:
            return (uint8 *)"Mirroring session is full";

        case RT_ERR_MIRROR_SESSION_NOEXIST:
            return (uint8 *)"Mirroring session does not exist";

        case RT_ERR_MIRROR_PORT_EXIST:
            return (uint8 *)"Mirroring port already exists";

        case RT_ERR_MIRROR_PORT_NOT_EXIST:
            return (uint8 *)"Mirroring port does not exist";

        case RT_ERR_MIRROR_PORT_FULL:
            return (uint8 *)"Exceeds maximum number of supported mirroring port";
        case RT_ERR_MIRROR_DP_IN_SPM_DPM:
            return (uint8 *)"mirroring port can not be in ingress or egress mirrored portmask of any mirroring set";

        /* for stat */
        case RT_ERR_STAT_INVALID_GLOBAL_CNTR:
            return (uint8 *)"Invalid global counter";

        case RT_ERR_STAT_INVALID_PORT_CNTR:
            return (uint8 *)"Invalid port counter";

        case RT_ERR_STAT_GLOBAL_CNTR_FAIL:
            return (uint8 *)"Can't retrieve/reset global counter";

        case RT_ERR_STAT_PORT_CNTR_FAIL:
            return (uint8 *)"Can't retrieve/reset port counter";

        /* for port related */
        case RT_ERR_PORT_VLAN_ISO_VID_EXIST_IN_OTHER_IDX:
            return (uint8 *)"VID exists in other entry";
        case RT_ERR_PORT_VLAN_ISO_VID_NOT_FOUND:
            return (uint8 *)"VID not found";
        case RT_ERR_PORT_VLAN_ISO_NO_EMPTY_ENTRY:
            return (uint8 *)"No VLAN isolation entry is empty";

        /* for L3 related */
        case RT_ERR_INTF_MTU_VARIETY_EXCEEDS:
            return (uint8 *)"Interface MTU variety exceeds the maximum";
        case RT_ERR_TTL_EXCEED:
            return (uint8 *)"TTL exceeds the maximum";

        case RT_ERR_MCAST_GROUP_NOT_FOUND:
            return (uint8 *)"Mcast group entry not found";
        case RT_ERR_MCAST_GROUP_UNBIND:
            return (uint8 *)"Mcast group entry not bind multicast ip address";
        case RT_ERR_IPMC_ADDR:
            return (uint8 *)"Invalid multicast ip address";
        case RT_ERR_INTF_NOT_EXIST:
            return (uint8 *)"Interface ID not exsit";
        case RT_ERR_NEXTHOP_NOT_EXIST:
            return (uint8 *)"Next-hop ID(Path ID) not exsit";
        case RT_ERR_MTU_EXCEED:
            return (uint8 *)"Interface MTU value is too big (over maximum)";

        /* OpenFlow */
        case RT_ERR_OF_FIELD_TYPE:
            return (uint8 *)"Invalid match field type";
        case RT_ERR_OF_FT_PHASE:
            return (uint8 *)"Invalid Flow Table phase";
        case RT_ERR_OF_SET_FIELD_ID:
            return (uint8 *)"Invalid set field ID";
        case RT_ERR_OF_SET_FIELD_TYPE:
            return (uint8 *)"Invalid set field type";
        case RT_ERR_VALID_ENTRY_EXIST:
            return (uint8 *)"Valid entry is existed";
        case RT_ERR_OF_BLOCK_PHASE:
            return (uint8 *)"The block is not belonged to Flow Table";

        /*Stacking*/
        case RT_ERR_STACK_DEV_ID:
            return (uint8 *)"Invalid stacking device ID";
        case RT_ERR_STACK_PORT_ID:
            return (uint8 *)"Invalid stacking port ID";
        case RT_ERR_STACK_PORT_NUM:
            return (uint8 *)"Number of stacking ports exceeds the maximum";
        case RT_ERR_STACK_PORT_RX_EN:
            return (uint8 *)"MAC RX disabled serdes port can't be configured as stacking port";

        /* BPE */
        case RT_ERR_BPE_NO_EMPTY_ENTRY:
            return (uint8 *)"No empty entry";

        default:
            return (uint8 *)"Unknown error code";
    }
    return RT_ERR_OK;
} /* end of rt_error_numToStr */

