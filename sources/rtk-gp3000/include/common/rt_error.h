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
 * Purpose : Definition the error number in the SDK.
 *
 * Feature : error definition
 *
 */

#ifndef __COMMON_RT_ERROR_H__
#define __COMMON_RT_ERROR_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/error.h>

/*
 * Data Type Declaration
 */
typedef enum rt_error_code_e
{
    /*
     * 0x0000xxxx for common error code
     * symbols defined at rt_error_common_t
     */

    /* 0x0001xxxx for vlan */
    RT_ERR_VLAN_VID = 0x00010000,                   /* 0x00010000, invalid vid                                                          */
    RT_ERR_VLAN_PRIORITY,                           /* 0x00010001, invalid 1p priority                                                  */
    RT_ERR_VLAN_EMPTY_ENTRY,                        /* 0x00010002, emtpy entry of vlan table                                            */
    RT_ERR_VLAN_ACCEPT_FRAME_TYPE,                  /* 0x00010003, invalid accept frame type                                            */
    RT_ERR_VLAN_EXIST,                              /* 0x00010004, vlan is exist                                                        */
    RT_ERR_VLAN_ENTRY_NOT_FOUND,                    /* 0x00010005, specified vlan entry not found                                       */
    RT_ERR_VLAN_PORT_MBR_EXIST,                     /* 0x00010006, member port exist in the specified vlan                              */
    RT_ERR_VLAN_FRAME_TYPE,                         /* 0x00010007, Error frame type                                                     */
    RT_ERR_VLAN_PROTO_AND_PORT,                     /* 0x00010008, Envalid protocol base group database index                           */


    /* 0x0002xxxx for svlan */
    RT_ERR_SVLAN_ENTRY_INDEX = 0x00020000,          /* 0x00020000, invalid svid entry no                                                */
    RT_ERR_SVLAN_ETHER_TYPE,                        /* 0x00020001, invalid SVLAN ether type                                             */
    RT_ERR_SVLAN_TABLE_FULL,                        /* 0x00020002, no empty entry in SVLAN table                                        */
    RT_ERR_SVLAN_ENTRY_NOT_FOUND,                   /* 0x00020003, specified svlan entry not found                                      */
    RT_ERR_SVLAN_EXIST,                             /* 0x00020004, SVLAN entry is exist                                                 */
    RT_ERR_SVLAN_VID,                               /* 0x00020005, invalid svid                                                         */
    RT_ERR_SVLAN_EFID,                              /* 0x00020006, invalid EFID                                                        */
    RT_ERR_SVLAN_EVID,                              /* 0x00020007, invalid EVID                                                        */
    RT_ERR_SVLAN_NOT_EXIST,                         /* 0x00020008, SVLAN entry is not exist                                                 */
    RT_ERR_SVLAN_INVALID,                           /* 0x00020009, SVLAN related config is invalid                                                  */

    /* 0x0003xxxx for MSTP */
    RT_ERR_MSTI = 0x00030000,                       /* 0x00030000, invalid msti                                                         */
    RT_ERR_MSTP_STATE,                              /* 0x00030001, invalid spanning tree status                                         */
    RT_ERR_MSTI_EXIST,                              /* 0x00030002, MSTI exist                                                           */
    RT_ERR_MSTI_NOT_EXIST,                          /* 0x00030003, MSTI not exist                                                       */

    /* 0x0004xxxx for BUCKET */
    RT_ERR_TIMESLOT = 0x00040000,                   /* 0x00040000, invalid time slot                                                    */
    RT_ERR_TOKEN,                                   /* 0x00040001, invalid token amount                                                 */
    RT_ERR_RATE,                                    /* 0x00040002, invalid rate                                                         */

    /* 0x0005xxxx for RMA */
    RT_ERR_RMA_ADDR = 0x00050000,                   /* 0x00050000, invalid rma mac address                                              */
    RT_ERR_RMA_ACTION,                              /* 0x00050001, invalid rma action                                                   */
    RT_ERR_RMA_MGMT_TYPE,                           /* 0x00050002, invalid type of management frame                                     */

    /* 0x0006xxxx for L2 */
    RT_ERR_L2_HASH_KEY = 0x00060000,                /* 0x00060000, invalid L2 Hash key                                                  */
    RT_ERR_L2_HASH_INDEX,                           /* 0x00060001, invalid L2 Hash index                                                */
    RT_ERR_L2_CAM_INDEX,                            /* 0x00060002, invalid L2 CAM index                                                 */
    RT_ERR_L2_ENRTYSEL,                             /* 0x00060003, invalid EntrySel                                                     */
    RT_ERR_L2_INDEXTABLE_INDEX,                     /* 0x00060004, invalid L2 index table(=portMask table) index                        */
    RT_ERR_LIMITED_L2ENTRY_NUM,                     /* 0x00060005, invalid limited L2 entry number                                      */
    RT_ERR_L2_AGGREG_PORT,                          /* 0x00060006, this aggregated port is not the lowest physical
                                                                   port of its aggregation group                                        */
    RT_ERR_L2_FID,                                  /* 0x00060007, invalid fid                                                          */
    RT_ERR_L2_RVID,                                 /* 0x00060008, invalid cvid                                                         */
    RT_ERR_L2_NO_EMPTY_ENTRY,                       /* 0x00060009, no empty entry in L2 table                                           */
    RT_ERR_L2_ENTRY_NOTFOUND,                       /* 0x0006000a, specified entry not found                                            */
    RT_ERR_L2_INDEXTBL_FULL,                        /* 0x0006000b, the L2 index table is full                                           */
    RT_ERR_L2_INVALID_FLOWTYPE,                     /* 0x0006000c, invalid L2 flow type                                                 */
    RT_ERR_L2_L2UNI_PARAM,                          /* 0x0006000d, invalid L2 unicast parameter                                         */
    RT_ERR_L2_L2MULTI_PARAM,                        /* 0x0006000e, invalid L2 multicast parameter                                       */
    RT_ERR_L2_IPMULTI_PARAM,                        /* 0x0006000f, invalid L2 ip multicast parameter                                    */
    RT_ERR_L2_PARTIAL_HASH_KEY,                     /* 0x00060010, invalid L2 partial Hash key                                          */
    RT_ERR_L2_EMPTY_ENTRY,                          /* 0x00060011, the entry is empty(invalid)                                          */
    RT_ERR_L2_FLUSH_TYPE,                           /* 0x00060012, the flush type is invalid                                            */
    RT_ERR_L2_NO_CPU_PORT,                          /* 0x00060013, CPU port not exist                                                   */
    RT_ERR_L2_MULTI_FWD_INDEX,                      /* 0x00060014, invalid index of multicast forwarding portmask                       */
    RT_ERR_L2_ENTRY_EXIST,                          /* 0x00060015, entry already exist                                                  */
    RT_ERR_L2_EXCEPT_ADDR_TYPE,                     /* 0x00060016, Invalid exception address type                                       */
    RT_ERR_L2_MCAST_FWD_ENTRY_EXIST,                /* 0x00060017, mcast forwarding entry already exist                                 */
    RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST,            /* 0x00060018, mcast forwarding entry not exist                                     */
    RT_ERR_L2_EFID,                                 /* 0x00060019, Invalid Enhanced FID                                                 */
    RT_ERR_L2_SIP_INDEX,                            /* 0x0006001a, Invalid SIP filter table index                                       */

    /* 0x0007xxxx for FILTER (PIE) */
    RT_ERR_FILTER_BLOCKNUM = 0x00070000,            /* 0x00070000, invalid block number                                                 */
    RT_ERR_FILTER_ENTRYIDX,                         /* 0x00070001, invalid entry index                                                  */
    RT_ERR_FILTER_CUTLINE,                          /* 0x00070002, invalid cutline value                                                */
    RT_ERR_FILTER_FLOWTBLBLOCK,                     /* 0x00070003, block belongs to flow table                                          */
    RT_ERR_FILTER_INACLBLOCK,                       /* 0x00070004, block belongs to ingress ACL                                         */
    RT_ERR_FILTER_ACTION,                           /* 0x00070005, action doesn't consist to entry type                                 */
    RT_ERR_FILTER_INACL_RULENUM,                    /* 0x00070006, invalid ACL rulenum                                                  */
    RT_ERR_FILTER_INACL_TYPE,                       /* 0x00070007, entry type isn't an ingress ACL rule                                 */
    RT_ERR_FILTER_INACL_EXIST,                      /* 0x00070008, ACL entry is already exit                                            */
    RT_ERR_FILTER_INACL_EMPTY,                      /* 0x00070009, ACL entry is empty                                                   */
    RT_ERR_FILTER_FLOWTBL_TYPE,                     /* 0x0007000a, entry type isn't an flow table rule                                  */
    RT_ERR_FILTER_FLOWTBL_RULENUM,                  /* 0x0007000b, invalid flow table rulenum                                           */
    RT_ERR_FILTER_FLOWTBL_EMPTY,                    /* 0x0007000c, flow table entry is empty                                            */
    RT_ERR_FILTER_FLOWTBL_EXIST,                    /* 0x0007000d, flow table entry is already exist                                    */
    RT_ERR_FILTER_METER_ID,                         /* 0x0007000e, invalid metering id                                                  */
    RT_ERR_FILTER_LOG_ID,                           /* 0x0007000f, invalid log id                                                       */
    RT_ERR_PIE_FIELD_TYPE,                          /* 0x00070010, invalid pie field type                                               */
    RT_ERR_PIE_PHASE,                               /* 0x00070011, invalid pie phase                                                    */
    RT_ERR_PIE_PHASE_NOT_SUPPORTED,                 /* 0x00070012, pie phase not supported in the chip                                  */

    /* 0x0008xxxx for ACL Rate Limit */
    RT_ERR_ACLRL_HTHR = 0x00080000,                 /* 0x00080000, invalid high threshold                                               */
    RT_ERR_ACLRL_TIMESLOT,                          /* 0x00080001, invalid time slot                                                    */
    RT_ERR_ACLRL_TOKEN,                             /* 0x00080002, invalid token amount                                                 */
    RT_ERR_ACLRL_RATE,                              /* 0x00080003, invalid rate                                                         */

    /* 0x0009xxxx for Link aggregation */
    RT_ERR_LA_CPUPORT = 0x00090000,                 /* 0x00090000, CPU port can not be aggregated port                                  */
    RT_ERR_LA_TRUNK_ID,                             /* 0x00090001, invalid trunk id                                                     */
    RT_ERR_LA_PORTMASK,                             /* 0x00090002, invalid port mask                                                    */
    RT_ERR_LA_HASHMASK,                             /* 0x00090003, invalid hash mask                                                    */
    RT_ERR_LA_DUMB,                                 /* 0x00090004, this API should be used in 802.1ad dumb mode                         */
    RT_ERR_LA_PORTNUM_DUMB,                         /* 0x00090005, it can only aggregate at most four ports when 802.1ad dumb mode      */
    RT_ERR_LA_PORTNUM_NORMAL,                       /* 0x00090006, it can only aggregate at most eight ports when 802.1ad normal mode   */
    RT_ERR_LA_MEMBER_OVERLAP,                       /* 0x00090007, the specified port mask is overlapped with other group               */
    RT_ERR_LA_NOT_MEMBER_PORT,                      /* 0x00090008, the port is not a member port of the trunk                           */
    RT_ERR_LA_TRUNK_NOT_EXIST,                      /* 0x00090009, the trunk doesn't exist                                              */

    /* 0x000axxxx for storm filter */
    RT_ERR_SFC_TICK_PERIOD = 0x000a0000,            /* 0x000a0000, invalid SFC tick period                                              */
    RT_ERR_SFC_UNKNOWN_GROUP,                       /* 0x000a0001, Unknown Storm filter group                                           */

    /* 0x000bxxxx for pattern match */
    RT_ERR_PM_MASK = 0x000b0000,                    /* 0x000b0000, invalid pattern length. Pattern length should be 8                   */
    RT_ERR_PM_LENGTH,                               /* 0x000b0001, invalid pattern match mask, first byte must care                     */
    RT_ERR_PM_MODE,                                 /* 0x000b0002, invalid pattern match mode                                           */

    /* 0x000cxxxx for input bandwidth control */
    RT_ERR_INBW_TICK_PERIOD = 0x000c0000,           /* 0x000c0000, invalid tick period for input bandwidth control                      */
    RT_ERR_INBW_TOKEN_AMOUNT,                       /* 0x000c0001, invalid amount of token for input bandwidth control                  */
    RT_ERR_INBW_FCON_VALUE,                         /* 0x000c0002, invalid flow control ON threshold value for input bandwidth control  */
    RT_ERR_INBW_FCOFF_VALUE,                        /* 0x000c0003, invalid flow control OFF threshold value for input bandwidth control */
    RT_ERR_INBW_FC_ALLOWANCE,                       /* 0x000c0004, invalid allowance of incomming packet for input bandwidth control    */
    RT_ERR_INBW_RATE,                               /* 0x000c0005, invalid input bandwidth                                              */

    /* 0x000dxxxx for QoS */
    RT_ERR_QOS_1P_PRIORITY = 0x000d0000,            /* 0x000d0000, invalid 802.1P priority                                              */
    RT_ERR_QOS_DSCP_VALUE,                          /* 0x000d0001, invalid DSCP value                                                   */
    RT_ERR_QOS_INT_PRIORITY,                        /* 0x000d0002, invalid internal priority                                            */
    RT_ERR_QOS_SEL_DSCP_PRI,                        /* 0x000d0003, invalid DSCP selection priority                                      */
    RT_ERR_QOS_SEL_PORT_PRI,                        /* 0x000d0004, invalid port selection priority                                      */
    RT_ERR_QOS_SEL_IN_ACL_PRI,                      /* 0x000d0005, invalid ingress ACL selection priority                               */
    RT_ERR_QOS_SEL_CLASS_PRI,                       /* 0x000d0006, invalid classifier selection priority                                */
    RT_ERR_QOS_EBW_RATE,                            /* 0x000d0007, invalid egress bandwidth rate                                        */
    RT_ERR_QOS_SCHE_TYPE,                           /* 0x000d0008, invalid QoS scheduling type                                          */
    RT_ERR_QOS_QUEUE_WEIGHT,                        /* 0x000d0009, invalid Queue weight                                                 */

    /* 0x000exxxx for port ability */
    RT_ERR_PHY_PAGE_ID = 0x000e0000,                /* 0x000e0000, invalid PHY page id                                                  */
    RT_ERR_PHY_REG_ID,                              /* 0x000e0001, invalid PHY reg id                                                   */
    RT_ERR_PHY_DATAMASK,                            /* 0x000e0002, invalid PHY data mask                                                */
    RT_ERR_PHY_AUTO_NEGO_MODE,                      /* 0x000e0003, invalid PHY auto-negotiation mode*/
    RT_ERR_PHY_SPEED,                               /* 0x000e0004, invalid PHY speed setting                                            */
    RT_ERR_PHY_DUPLEX,                              /* 0x000e0005, invalid PHY duplex setting                                           */
    RT_ERR_PHY_FORCE_ABILITY,                       /* 0x000e0006, invalid PHY force mode ability parameter                             */
    RT_ERR_PHY_FORCE_1000,                          /* 0x000e0007, invalid PHY force mode 1G speed setting                              */
    RT_ERR_PHY_TXRX,                                /* 0x000e0008, invalid PHY tx/rx                                                    */
    RT_ERR_PHY_RTCT_NOT_FINISH,                     /* 0x000e0009, PHY RTCT in progress                                                 */
    RT_ERR_PHY_RTCT_TIMEOUT,                        /* 0x000e000a, PHY RTCT timeout                                                     */
    RT_ERR_PHY_AUTO_ABILITY,                        /* 0x000e000b, invalid PHY auto mode ability parameter                              */
    RT_ERR_PHY_FIBER_LINKUP,                        /* 0x000e000c, Access copper PHY is not permit when fiber is linkup                 */

    /* 0x000fxxxx for mirror */
    RT_ERR_MIRROR_DIRECTION = 0x000f0000,           /* 0x000f0000, invalid error mirror direction                                       */
    RT_ERR_MIRROR_SESSION_FULL,                     /* 0x000f0001, mirroring session is full                                            */
    RT_ERR_MIRROR_SESSION_NOEXIST,                  /* 0x000f0002, mirroring session not exist                                          */
    RT_ERR_MIRROR_PORT_EXIST,                       /* 0x000f0003, mirroring port already exists                                        */
    RT_ERR_MIRROR_PORT_NOT_EXIST,                   /* 0x000f0004, mirroring port does not exists                                       */
    RT_ERR_MIRROR_PORT_FULL,                        /* 0x000f0005, Exceeds maximum number of supported mirroring port                   */
    RT_ERR_MIRROR_ID,                               /* 0x000f0006, Invalid mirror ID                                                    */

    /* 0x0010xxxx for stat */
    RT_ERR_STAT_INVALID_GLOBAL_CNTR = 0x00100000,   /* 0x00100000, Invalid Global Counter                                               */
    RT_ERR_STAT_INVALID_PORT_CNTR,                  /* 0x00100001, Invalid Port Counter                                                 */
    RT_ERR_STAT_GLOBAL_CNTR_FAIL,                   /* 0x00100002, Could not retrieve/reset Global Counter                              */
    RT_ERR_STAT_PORT_CNTR_FAIL,                     /* 0x00100003, Could not retrieve/reset Port Counter                                */
    RT_ERR_STAT_INVALID_SMON_CNTR,                  /* 0x00100004, Invalid SMON Counter                                                 */
    RT_ERR_STAT_SMON_CNTR_FAIL,                     /* 0x00100005, Could not retrieve/reset SMON Counter                                */

    /* 0x0011xxxx for dot1x */
    RT_ERR_DOT1X_INVALID_DIRECTION = 0x00110000,    /* 0x00110000, Invalid Authentication Direction                                     */
    RT_ERR_DOT1X_PROC,                              /* 0x00110001, Invalid process type                                                 */
    RT_ERR_DOT1X_GVLANIDX,                          /* 0x00110002, Invalid cvid index                                                   */

    /* 0x0012xxxx for GPON */
    RT_ERR_GPON_INITIAL_FAIL = 0x00120000,          /* 0x00120000, GPON driver initialize fail                                          */
    RT_ERR_GPON_INVALID_HANDLE,                     /* 0x00120001, GPON driver handle error                                             */
    RT_ERR_GPON_DRV_NOT_STARTED,                    /* 0x00120002, GPON driver is not started                                           */
    RT_ERR_GPON_DRV_ALREADY_STARTED,                /* 0x00120003, GPON driver is already started                                       */
    RT_ERR_GPON_ONU_ALREADY_REGISTERED,             /* 0x00120004, ONU is already registered                                            */
    RT_ERR_GPON_ONU_NOT_REGISTERED,                 /* 0x00120005, ONU is not registered                                                */
    RT_ERR_GPON_ONU_ALREADY_ACTIVATED,              /* 0x00120006, GPON Device has already been activeted                               */
    RT_ERR_GPON_ONU_NOT_ACTIVATED,                  /* 0x00120007, GPON Device has not been activeted                                   */
    RT_ERR_GPON_EXCEED_MAX_TCONT,                   /* 0x00120008, Exceed maximum number of the tcont                                   */
    RT_ERR_GPON_EXCEED_MAX_FLOW,                    /* 0x00120009, Exceed maximum number of the flow                                    */
    RT_ERR_GPON_PLOAM_QUEUE_FULL,                   /* 0x0012000a, PLOAM queue is full                                                  */
    RT_ERR_GPON_PLOAM_QUEUE_EMPTY,                  /* 0x0012000b, PLOAM queue is empty                                                 */
    RT_ERR_GPON_OMCI_QUEUE_FULL,                    /* 0x0012000c, OMCI queue is full                                                   */
    RT_ERR_GPON_OMCI_QUEUE_EMPTY,                   /* 0x0012000d, OMCI queue is empty                                                  */
    RT_ERR_GPON_TABLE_ENTRY_NOT_FOUND,              /* 0x0012000e, The entry is not found in table                                      */
    RT_ERR_GPON_MAC_FILTER_TABLE_FULL,              /* 0x0012000f, The entry is not found in table                                      */
    RT_ERR_GPON_MAC_FILTER_TABLE_ALLOC_FAIL,        /* 0x00120010, The entry is allocate fail                                           */
    RT_ERR_GPON_FORBID_TCONT_ID,                    /* 0x00120011, The input tcont id is forbid tcont for aggregate tcont               */

    /* 0x0013xxxx for DSL or EXT */
    RT_ERR_DSL_VC = 0x00130000,                     /* 0x00130000, invalid DSL VC                                          */
    RT_ERR_EXT_PORT_ID,                             /* 0x00130001, invalid EXT port id                                          */

    RT_ERR_END                                      /* The symbol is the latest symbol                                                  */
} rt_error_code_t;

/*
 * Macro Definition
 */
#define RT_ERR_XLATE_RET(ret)\
do {\
    if (ret) {\
        return rtk_error_translate(ret);\
    }\
} while (0)

/*
 * Function Declaration
 */
extern int32
rtk_error_translate(int32 errCode);

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

extern const uint8 *rt_error_numToStr(int32 err_num);

#endif /* __COMMON_RT_ERROR_H__ */

