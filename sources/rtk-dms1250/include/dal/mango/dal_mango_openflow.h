/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2015
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) OpenFlow
 *
 */

#ifndef __DAL_MANGO_OPENFLOW_H__
#define __DAL_MANGO_OPENFLOW_H__

/*
 * Include Files
 */
#include <rtk/openflow.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

#define DAL_MANGO_OF_DMAC_ENTRY_MAX         (1024)  /* max. DMAC entry */

typedef enum dal_mango_of_getMethod_e
{
    MANGO_OF_GETMETHOD_EXIST = 0,
    MANGO_OF_GETMETHOD_FREE,
    MANGO_OF_GETMETHOD_EXIST_FIRST,/* if entry existed, the entry index is returned. Or free index is returned */
    MANGO_OF_GETMETHOD_END
} dal_mango_of_getMethod_t;

typedef enum dal_mango_of_memType_e
{
    MANGO_OF_MEMTYPE_SRAM = 0,
    MANGO_OF_MEMTYPE_CAM,
    MANGO_OF_MEMTYPE_END
} dal_mango_of_memType_t;

typedef enum dal_mango_of_l2EntryType_e
{
    MANGO_OF_L2ENTRY_TYPE_SA = 0,
    MANGO_OF_L2ENTRY_TYPE_DA,
    MANGO_OF_L2ENTRY_TYPE_SA_DA,
    MANGO_OF_L2ENTRY_TYPE_FIVE_TUPLE,
    OF_L2ENTRY_TYPE_END
} dal_mango_of_l2EntryType_t;

typedef enum dal_mango_of_l2SFType0_e
{
    MANGO_OF_L2_FIELD_TYPE0_NONE = 0,
    MANGO_OF_L2_FIELD_TYPE0_VLAN_ID,
    MANGO_OF_L2_FIELD_TYPE0_IP_DSCP,
    MANGO_OF_L2_FIELD_TYPE0_END
} dal_mango_of_l2SFType0_t;

typedef enum dal_mango_of_l2SFType1_e
{
    MANGO_OF_L2_FIELD_TYPE1_NONE = 0,
    MANGO_OF_L2_FIELD_TYPE1_VLAN_PRI,
    MANGO_OF_L2_FIELD_TYPE1_END
} dal_mango_of_l2SFType1_t;

typedef enum dal_mango_of_l2OutputType_e
{
    MANGO_OF_L2_OUTPUT_TYPE_DISABLE = 0,
    MANGO_OF_L2_OUTPUT_TYPE_PHY_PORT,
    MANGO_OF_L2_OUTPUT_TYPE_TRK_PORT,
    MANGO_OF_L2_OUTPUT_TYPE_MULTI_EGR_PORT,
    MANGO_OF_L2_OUTPUT_TYPE_IN_PORT,
    MANGO_OF_L2_OUTPUT_TYPE_FLOOD,
    MANGO_OF_L2_OUTPUT_TYPE_LB,
    MANGO_OF_L2_OUTPUT_TYPE_TUNNEL,
    MANGO_OF_L2_OUTPUT_TYPE_END
} dal_mango_of_l2OutputType_t;

typedef enum dal_mango_of_gotoTblType_e
{
    MANGO_OF_GOTOTBL_NORMAL = 0,
    MANGO_OF_GOTOTBL_APPLY_AND_LB,
    MANGO_OF_GOTOTBL_LB,
    MANGO_OF_GOTOTBL_END
} dal_mango_of_gotoTblType_t;

typedef enum dal_mango_of_l3IpVer_e
{
    MANGO_OF_L3_IPVER_IP4 = 0,
    MANGO_OF_L3_IPVER_IP6,
    MANGO_OF_L3_IPVER_END
} dal_mango_of_l3IpVer_t;

typedef enum dal_mango_of_l3EntryType_e
{
    MANGO_OF_L3ENTRY_TYPE_SIP = 0,
    MANGO_OF_L3ENTRY_TYPE_DIP,
    MANGO_OF_L3ENTRY_TYPE_SIP_DIP,
    MANGO_OF_L3ENTRY_TYPE_END
} dal_mango_of_l3EntryType_t;

typedef enum dal_mango_of_l3SFType0_e
{
    MANGO_OF_L3_FIELD_TYPE0_NONE = 0,
    MANGO_OF_L3_FIELD_TYPE0_SA,
    MANGO_OF_L3_FIELD_TYPE0_VLAN_PRI,
    MANGO_OF_L3_FIELD_TYPE0_IP_DSCP,
    MANGO_OF_L3_FIELD_TYPE0_END
} dal_mango_of_l3SFType0_t;

typedef enum dal_mango_of_l3SFType1_e
{
    MANGO_OF_L3_FIELD_TYPE1_NONE = 0,
    MANGO_OF_L3_FIELD_TYPE1_DA,
    MANGO_OF_L3_FIELD_TYPE1_VLAN_PRI,
    MANGO_OF_L3_FIELD_TYPE1_IP_DSCP,
    MANGO_OF_L3_FIELD_TYPE1_END
} dal_mango_of_l3SFType1_t;

typedef enum dal_mango_of_l3SFType2_e
{
    MANGO_OF_L3_FIELD_TYPE2_NONE = 0,
    MANGO_OF_L3_FIELD_TYPE2_VLAN_ID,
    MANGO_OF_L3_FIELD_TYPE2_VLAN_PRI,
    MANGO_OF_L3_FIELD_TYPE2_IP_DSCP,
    MANGO_OF_L3_FIELD_TYPE2_END
} dal_mango_of_l3SFType2_t;

typedef enum dal_mango_of_outputType_e
{
    MANGO_OF_OUTPUT_TYPE_DISABLE = 0,
    MANGO_OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT,
    MANGO_OF_OUTPUT_TYPE_PHY_PORT,
    MANGO_OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT,
    MANGO_OF_OUTPUT_TYPE_TRK_PORT,
    MANGO_OF_OUTPUT_TYPE_MULTI_EGR_PORT,
    MANGO_OF_OUTPUT_TYPE_IN_PORT,
    MANGO_OF_OUTPUT_TYPE_FLOOD,
    MANGO_OF_OUTPUT_TYPE_LB,
    MANGO_OF_OUTPUT_TYPE_TUNNEL,
    MANGO_OF_OUTPUT_TYPE_FAILOVER,
    MANGO_OF_OUTPUT_TYPE_END
} dal_mango_of_igrOutputType_t, dal_mango_of_l3OutputType_t, dal_mango_of_actBktOutputType_t;

typedef enum dal_mango_of_popMplsType_e
{
    MANGO_OF_POP_MPLS_TYPE_OUTERMOST,
    MANGO_OF_POP_MPLS_TYPE_DOUBLE,
    MANGO_OF_POP_MPLS_TYPE_END
} dal_mango_of_popMplsType_t;

typedef enum dal_mango_of_pushMplsMode_e
{
    MANGO_OF_PUSH_MPLS_MODE_NORMAL,
    MANGO_OF_PUSH_MPLS_MODE_ENCAPTBL,
    MANGO_OF_PUSH_MPLS_MODE_END
} dal_mango_of_pushMplsMode_t;

typedef enum dal_mango_of_pushMplsVpnType_e
{
    MANGO_OF_PUSH_MPLS_VPN_TYPE_L3,
    MANGO_OF_PUSH_MPLS_VPN_TYPE_L2,
    MANGO_OF_PUSH_MPLS_VPN_TYPE_END
} dal_mango_of_pushMplsVpnType_t;

typedef enum dal_mango_of_meterAct_e
{
    MANGO_OF_METER_ACT_DROP = 0,
    MANGO_OF_METER_ACT_DSCP_REMARK,
    MANGO_OF_METER_ACT_END
} dal_mango_of_meterAct_t;

typedef enum dal_mango_of_sfType0_e
{
    MANGO_OF_SF_TYPE0_NONE = 0,
    MANGO_OF_SF_TYPE0_SA,
    MANGO_OF_SF_TYPE0_VLAN_PRI,
    MANGO_OF_SF_TYPE0_MPLS_TC,
    MANGO_OF_SF_TYPE0_MPLS_TTL,
    MANGO_OF_SF_TYPE0_IP_DSCP,
    MANGO_OF_SF_TYPE0_IP_TTL,
    MANGO_OF_SF_TYPE0_IP_FLAG_RSVD,
    MANGO_OF_SF_TYPE0_END
} dal_mango_of_igrSFType0_t, dal_mango_of_actBktSFType0_t;

typedef enum dal_mango_of_sfType1_e
{
    MANGO_OF_SF_TYPE1_NONE = 0,
    MANGO_OF_SF_TYPE1_DA,
    MANGO_OF_SF_TYPE1_VLAN_PRI,
    MANGO_OF_SF_TYPE1_MPLS_LABEL,
    MANGO_OF_SF_TYPE1_MPLS_TC,
    MANGO_OF_SF_TYPE1_MPLS_TTL,
    MANGO_OF_SF_TYPE1_IP_DSCP,
    MANGO_OF_SF_TYPE1_IP_TTL,
    MANGO_OF_SF_TYPE1_END
} dal_mango_of_igrSFType1_t, dal_mango_of_actBktSFType1_t;

typedef enum dal_mango_of_sfType2_e
{
    MANGO_OF_SF_TYPE2_NONE = 0,
    MANGO_OF_SF_TYPE2_VLAN_ID,
    MANGO_OF_SF_TYPE2_VLAN_PRI,
    MANGO_OF_SF_TYPE2_MPLS_LABEL,
    MANGO_OF_SF_TYPE2_MPLS_TC,
    MANGO_OF_SF_TYPE2_MPLS_TTL,
    MANGO_OF_SF_TYPE2_IP_DSCP,
    MANGO_OF_SF_TYPE2_IP_TTL,
    MANGO_OF_SF_TYPE2_END
} dal_mango_of_igrSFType2_t, dal_mango_of_actBktSFType2_t;

typedef enum dal_mango_of_igrSFType3_t
{
    MANGO_OF_IGR_SF_TYPE3_NONE = 0,
    MANGO_OF_IGR_SF_TYPE3_SIP,
    MANGO_OF_IGR_SF_TYPE3_DIP,
    MANGO_OF_IGR_SF_TYPE3_VLAN_PRI,
    MANGO_OF_IGR_SF_TYPE3_MPLS_LABEL,
    MANGO_OF_IGR_SF_TYPE3_MPLS_TC,
    MANGO_OF_IGR_SF_TYPE3_MPLS_TTL,
    MANGO_OF_IGR_SF_TYPE3_IP_DSCP,
    MANGO_OF_IGR_SF_TYPE3_END
} dal_mango_of_igrSFType3_t;

typedef enum dal_mango_of_igrSFType4_t
{
    MANGO_OF_IGR_SF_TYPE4_NONE = 0,
    MANGO_OF_IGR_SF_TYPE4_SPORT,
    MANGO_OF_IGR_SF_TYPE4_DPORT,
    MANGO_OF_IGR_SF_TYPE4_VLAN_PRI,
    MANGO_OF_IGR_SF_TYPE4_MPLS_LABEL,
    MANGO_OF_IGR_SF_TYPE4_MPLS_TC,
    MANGO_OF_IGR_SF_TYPE4_MPLS_TTL,
    MANGO_OF_IGR_SF_TYPE4_IP_DSCP,
    MANGO_OF_IGR_SF_TYPE4_END
} dal_mango_of_igrSFType4_t;

typedef enum dal_mango_of_egrSFType0_e
{
    MANGO_OF_EGR_SFTYPE0_NONE = 0,
    MANGO_OF_EGR_SFTYPE0_VLAN_PRI,
    MANGO_OF_EGR_SFTYPE0_IP_DSCP,
    MANGO_OF_EGR_SFTYPE0_END
} dal_mango_of_egrSFType0_t;


typedef enum dal_mango_of_egrSFType1_e
{
    MANGO_OF_EGR_SFTYPE1_NONE = 0,
    MANGO_OF_EGR_SFTYPE1_VLAN_ID,
    MANGO_OF_EGR_SFTYPE1_END
} dal_mango_of_egrSFType1_t;


/*
 * Data Declaration
 */

typedef struct dal_mango_of_drvDb_s
{
    /* OpenFlow DMAC table */
    struct
    {
        rtk_bitmap_t    used[BITMAP_ARRAY_CNT(DAL_MANGO_OF_DMAC_ENTRY_MAX)];
        uint32          used_count;
    } dmac_tbl;
} dal_mango_of_drvDb_t;

typedef struct dal_mango_of_dmacEntry_s
{
    rtk_mac_t mac;
} dal_mango_of_dmacEntry_t;

typedef struct dal_mango_of_igrFtIns_s
{
    rtk_enable_t                    meter_en;
    rtk_enable_t                    clearAct_en;
    rtk_enable_t                    writeAct_en;
    rtk_enable_t                    writeMd_en;
    rtk_enable_t                    gotoTbl_en;

    uint32                          meterIdx;
    dal_mango_of_meterAct_t         yellow_act;
    dal_mango_of_meterAct_t         red_act;

    rtk_enable_t 				    cp_ttl_inward;

    rtk_enable_t 				    pop_vlan;

    rtk_enable_t 				    pop_mpls;
    dal_mango_of_popMplsType_t      pop_mpls_type;

    rtk_enable_t 				    push_mpls;
    dal_mango_of_pushMplsMode_t     push_mpls_mode;
    dal_mango_of_pushMplsVpnType_t  push_mpls_vpnType;
    uint32	                        push_mpls_libIdx;
    uint32	                        push_mpls_ethType;


    rtk_enable_t 	                push_vlan;
    uint32	                        etherType_idx;

    rtk_enable_t 				    cp_ttl_outward;

    rtk_enable_t 	                dec_mpls_ttl;

    rtk_enable_t 	                dec_ip_ttl;

    dal_mango_of_igrSFType0_t       field0_type;
    uint32			                field0_data;
    dal_mango_of_igrSFType1_t       field1_type;
    uint32			                field1_data;
    dal_mango_of_igrSFType2_t       field2_type;
    uint32			                field2_data;
    dal_mango_of_igrSFType3_t       field3_type;
    uint32			                field3_data;
    dal_mango_of_igrSFType4_t       field4_type;
    uint32			                field4_data;

    rtk_enable_t 	                set_queue;
    uint32			                qid;

    rtk_enable_t 	                group;
    uint32			                gid;

    dal_mango_of_igrOutputType_t    output_type;
    uint32                          output_data;

    uint32                          metadata;
    uint32                          metadata_mask;

    dal_mango_of_gotoTblType_t      tbl_act;
    uint32                          tbl_id;
    uint32                          tbl_lb_time;
} dal_mango_of_igrFtIns_t;

typedef struct dal_mango_of_egrFtIns_s
{
    rtk_enable_t                    meter_en;
    rtk_enable_t                    writeAct_en;

    uint32                          meterIdx;
    dal_mango_of_meterAct_t         yellow_act;
    dal_mango_of_meterAct_t         red_act;

    rtk_enable_t 				    pop_vlan;

    rtk_enable_t 	                push_vlan;
    uint32	                        etherType_idx;

    dal_mango_of_egrSFType0_t       field0_type;
    uint32			                field0_data;
    dal_mango_of_egrSFType1_t       field1_type;
    uint32			                field1_data;

    rtk_enable_t                    drop;
} dal_mango_of_egrFtIns_t;

typedef struct dal_mango_of_bucketIdx_s
{
    uint32 bucket0_hash_idx;/* bucket 0 hash index */
    uint32 bucket1_hash_idx;/* bucket 1 hash index */
    uint32 bucket0_idx;         /* bucket 0 table access index */
    uint32 bucket1_idx;         /* bucket 1 table access index */
} dal_mango_of_bucketIdx_t;

typedef struct dal_mango_of_getResult_s
{
    uint32 isFree;                  /* found free entry */
    uint32 isExist;                 /* found exist entry */
    uint32 idx;                     /* free or existed entry index */
    dal_mango_of_memType_t memType; /* located memory. not used for L3. */
} dal_mango_of_getResult_t;

typedef struct dal_mango_of_l2Ins_s
{
    rtk_enable_t                clearAct_en;
    rtk_enable_t                writeAct_en;
    rtk_enable_t                writeMd_en;
    rtk_enable_t                gotoTbl_en;

    rtk_enable_t 	            push_vlan;
    uint32	                    etherType_idx;

    dal_mango_of_l2SFType0_t    field0_type;
    uint32			            field0_data;
    dal_mango_of_l2SFType1_t    field1_type;
    uint32			            field1_data;

    rtk_enable_t 	            set_queue;
    uint32			            qid;

    dal_mango_of_l2OutputType_t output_type;
    uint32                      output_data;

    uint32                      metadata;
    uint32                      metadata_mask;

    dal_mango_of_gotoTblType_t  tbl_act;
    uint32                      tbl_id;
    uint32                      tbl_lb_time;
} dal_mango_of_l2Ins_t;

typedef struct dal_mango_of_l2Entry_s
{
    uint32                      valid;
    uint32                      fmt;/* 0: Ethernet/WLAN, 1: OpenFlow */
    dal_mango_of_l2EntryType_t  type;

    struct
    {
        struct
        {
            rtk_vlan_t      vid;
            rtk_mac_t       mac0;
            rtk_mac_t       mac1;/* only valid for type MANGO_OF_L2ENTRY_TYPE_SA_DA */
            uint32          md_cmp;
            uint32          metadata;
        } vidMac;

        struct
        {
            uint32          type;/* 0: IP Protocol, 1: VID */
            struct
            {
                rtk_vlan_t  vid;
                uint32      ipproto;
            } fifthData;

            rtk_ip_addr_t   sip;
            rtk_ip_addr_t   dip;
            uint32          l4_sport;
            uint32          l4_dport;
        } fiveTp;
    } matchfd;

    dal_mango_of_l2Ins_t        ins;
    uint32                      hit;
} dal_mango_of_l2Entry_t;

typedef struct dal_mango_of_l3Ins_s
{
    rtk_enable_t                clearAct_en;
    rtk_enable_t                writeAct_en;
    rtk_enable_t                writeMd_en;
    rtk_enable_t                gotoTbl_en;

    rtk_enable_t 	            push_vlan;
    uint32	                    etherType_idx;

    rtk_enable_t 	            dec_ip_ttl;

    dal_mango_of_l3SFType0_t    field0_type;
    uint32			            field0_data;
    dal_mango_of_l3SFType1_t    field1_type;
    uint32			            field1_data;
    dal_mango_of_l3SFType2_t    field2_type;
    uint32			            field2_data;

    rtk_enable_t 	            set_queue;
    uint32			            qid;

    rtk_enable_t 	            group;
    uint32			            gid;

    dal_mango_of_l3OutputType_t output_type;
    uint32                      output_data;

    uint32                      metadata;
    uint32                      metadata_mask;

    dal_mango_of_gotoTblType_t  tbl_act;
    uint32                      tbl_id;
    uint32                      tbl_lb_time;
} dal_mango_of_l3Ins_t;

typedef struct dal_mango_of_l3CamEntry_s
{
    uint32                      valid;
    uint32                      fmt; /* 0: L3, 1: OpenFlow */
    dal_mango_of_l3EntryType_t  type;
    dal_mango_of_l3IpVer_t      ip_ver;

    union
    {
        rtk_ip_addr_t   ipv4;
        rtk_ipv6_addr_t ipv6;
    } ip0;

    union
    {
        rtk_ip_addr_t   ipv4;
        rtk_ipv6_addr_t ipv6;
    } ip0_msk;

    union
    {
        rtk_ip_addr_t   ipv4;
        rtk_ipv6_addr_t ipv6;
    } ip1;

    union
    {
        rtk_ip_addr_t   ipv4;
        rtk_ipv6_addr_t ipv6;
    } ip1_msk;

    uint32                      metadata;
    uint32                      metadata_msk;

    dal_mango_of_l3Ins_t        ins;
    uint32                      hit;
} dal_mango_of_l3CamEntry_t;

typedef struct dal_mango_of_l3HashEntry_s
{
    uint32                      valid;
    uint32                      fmt; /* 0: L3, 1: OpenFlow */
    dal_mango_of_l3EntryType_t  type;
    dal_mango_of_l3IpVer_t      ip_ver;

    union
    {
        rtk_ip_addr_t   ipv4;
        rtk_ipv6_addr_t ipv6;
    } ip0;

    union
    {
        rtk_ip_addr_t   ipv4;
        rtk_ipv6_addr_t ipv6;
    } ip1;

    rtk_enable_t                md_cmp;
    uint32                      metadata;

    dal_mango_of_l3Ins_t        ins;
    uint32                      hit;
} dal_mango_of_l3HashEntry_t;

typedef struct dal_mango_of_actBucket_s
{
    rtk_enable_t 				    cp_ttl_inward;

    rtk_enable_t 				    pop_vlan;

    rtk_enable_t 				    pop_mpls;
    dal_mango_of_popMplsType_t      pop_mpls_type;

    rtk_enable_t 				    push_mpls;
    dal_mango_of_pushMplsMode_t     push_mpls_mode;
    dal_mango_of_pushMplsVpnType_t  push_mpls_vpnType;
    uint32	                        push_mpls_libIdx;
    uint32	                        push_mpls_ethType;

    rtk_enable_t 	                push_vlan;
    uint32	                        vlan_ethType_idx;

    rtk_enable_t 				    cp_ttl_outward;

    rtk_enable_t 	                dec_mpls_ttl;

    rtk_enable_t 	                dec_ip_ttl;

    dal_mango_of_actBktSFType0_t    field0_type;
    uint32			                field0_data;
    dal_mango_of_actBktSFType1_t    field1_type;
    uint32			                field1_data;
    dal_mango_of_actBktSFType2_t    field2_type;
    uint32			                field2_data;

    rtk_enable_t 	                set_queue;
    uint32			                qid;

    rtk_enable_t 	                group;
    uint32			                gid;

    dal_mango_of_actBktOutputType_t output_type;
    uint32                          output_data;

    uint32                          metadata;
    uint32                          metadata_mask;

    dal_mango_of_gotoTblType_t      tbl_act;
    uint32                          tbl_id;
} dal_mango_of_actBucket_t;

/*
 * Macro Declaration
 */
#define EXTRACT_BITS_BY_LEN(_val, _lsb, _len)   (((_val) & (((1 << (_len)) - 1) << (_lsb))) >> (_lsb))
#define DAL_MANGO_OF_ENTRY_IDX_TO_ADDR(_idx)    ((((_idx)/6)*8) + ((_idx)%6))

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_ofMapper_init
 * Description:
 *      Hook of module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook of module before calling any of APIs.
 */
extern int32
dal_mango_ofMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_of_init
 * Description:
 *      Initialize OpenFlow module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize OpenFlow module before calling any OpenFlow APIs.
 */
extern int32
dal_mango_of_init(uint32 unit);

/* Function Name:
 *      dal_mango_of_classifier_get
 * Description:
 *      Get OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 * Output:
 *      pData 	- classifier data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_INPUT    	- invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 */
extern int32
dal_mango_of_classifier_get(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t *pData);

/* Function Name:
 *      dal_mango_of_classifier_set
 * Description:
 *      Set OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 *      data 	- classifier data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 */
extern int32
dal_mango_of_classifier_set(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t data);

/* Function Name:
 *      dal_mango_of_flowMatchFieldSize_get
 * Description:
 *      Get the match field size of flow entry.
 * Input:
 *      unit        - unit id
 *      type        - type of entry field
 * Output:
 *      pField_size - match field size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The unit of size is bit.
 */
extern int32
dal_mango_of_flowMatchFieldSize_get(uint32 unit, rtk_of_matchfieldType_t type, uint32 *pField_size);

/* Function Name:
 *      dal_mango_of_flowEntrySize_get
 * Description:
 *      Get the flow entry size
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 * Output:
 *      pEntry_size - flow entry size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The unit of size is byte.
 */
extern int32
dal_mango_of_flowEntrySize_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 *pEntry_size);

/* Function Name:
 *      dal_mango_of_flowEntryValidate_get
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pValid    - pointer buffer of valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryValidate_get(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		*pValid);

/* Function Name:
 *      dal_mango_of_flowEntryValidate_set
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      valid     - valid state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    	- The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryValidate_set(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		valid);

#if 0
/* Function Name:
 *      dal_mango_of_flowEntry_read
 * Description:
 *      Read match fields data from specified flow entry.
 * Input:
 *      unit          - unit id
 *      phase         - Flow Table phase
 *      entry_idx     - entry index
 * Output:
 *      pEntry_buffer - data buffer of flow entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntry_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint8               		*pEntry_buffer);
#endif

/* Function Name:
 *      dal_mango_of_flowEntryFieldList_get
 * Description:
 *      Get supported match field list of the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OF_FT_PHASE    - invalid Flow Table phase
 * Note:
 *      None
 */
extern int32
dal_mango_of_flowEntryFieldList_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldList_t  *list);

/* Function Name:
 *      dal_mango_of_flowEntryField_check
 * Description:
 *      Check whether the match field type is supported on the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OF_FT_PHASE    - invalid Flow Table phase
 *      RT_ERR_OF_FIELD_TYPE  - invalid match field type
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldType_t  type);

/* Function Name:
 *      dal_mango_of_flowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified phase and field ID.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

/* Function Name:
 *      dal_mango_of_flowEntryField_read
 * Description:
 *      Read match field data from specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 * Output:
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE		- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryField_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask);

/* Function Name:
 *      dal_mango_of_flowEntryField_write
 * Description:
 *      Write match field data to specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryField_write(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask);

/* Function Name:
 *      dal_mango_of_flowEntryOperation_get
 * Description:
 *      Get flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 * Output:
 *      pOperation - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 *      (6) Egress flow table 0 doesn't support aggr_1 operation.
 */
extern int32
dal_mango_of_flowEntryOperation_get(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation);

/* Function Name:
 *      dal_mango_of_flowEntryOperation_set
 * Description:
 *      Set flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 *      pOperation - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 *      (6) Egress flow table 0 doesn't support aggr_1 operation.
 */
extern int32
dal_mango_of_flowEntryOperation_set(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation);

/* Function Name:
 *      dal_mango_of_flowEntryInstruction_get
 * Description:
 *      Get the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 * Output:
 *      pAction   - instruction mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryInstruction_get(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData);

/* Function Name:
 *      dal_mango_of_flowEntryInstruction_set
 * Description:
 *      Set the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      pData     - instruction mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_flowEntryInstruction_set(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData);

/* Function Name:
 *      dal_mango_of_flowEntryHitSts_get
 * Description:
 *      Get the flow entry hit status.
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The hit status can be cleared by configuring parameter 'reset' to (1)
 */
extern int32
dal_mango_of_flowEntryHitSts_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint32                   reset,
    uint32                   *pIsHit);

/* Function Name:
 *      dal_mango_of_flowEntry_del
 * Description:
 *      Delete the specified flow entries.
 * Input:
 *      unit    - unit id
 *      phase   - Flow Table phase
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OF_FT_PHASE     - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_OF_CLEAR_INDEX  - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all cleared.
 */
extern int32
dal_mango_of_flowEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx);

/* Function Name:
 *      dal_mango_of_flowEntry_move
 * Description:
 *      Move the specified flow entries.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      pData - movement info
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all moved.
 *      (3) The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      (4) (move_from + length) and (move_to + length) must <= the number of flow entries for the specified phase.
 */
extern int32
dal_mango_of_flowEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pData);

/* Function Name:
 *      dal_mango_of_ftTemplateSelector_get
 * Description:
 *      Get the mapping template of specific Flow Table block.
 * Input:
 *      unit          - unit id
 *      phase         - Flow Table phase
 *      block_idx     - block index
 * Output:
 *      pTemplate_idx - template index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_OF_BLOCK_PHASE   - The block is not belonged to Flow Table
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_ftTemplateSelector_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   *pTemplate_idx);

/* Function Name:
 *      dal_mango_of_ftTemplateSelector_set
 * Description:
 *      Set the mapping template of specific Flow Table block.
 * Input:
 *      unit         - unit id
 *      phase        - Flow Table phase
 *      block_idx    - block index
 *      template_idx - template index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_OF_BLOCK_PHASE       - The block is not belonged to Flow Table
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 */
extern int32
dal_mango_of_ftTemplateSelector_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   template_idx);

/* Function Name:
 *      dal_mango_of_flowCntMode_get
 * Description:
 *      Get counter mode for specific flow counters.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pMode     - counter mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 */
extern int32
dal_mango_of_flowCntMode_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     *pMode);

/* Function Name:
 *      dal_mango_of_flowCntMode_set
 * Description:
 *      Set counter mode for specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      mode      - counter mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 */
extern int32
dal_mango_of_flowCntMode_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     mode);

/* Function Name:
 *      dal_mango_of_flowCnt_get
 * Description:
 *      Get packet counter or byte counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      pCnt      - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 */
extern int32
dal_mango_of_flowCnt_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type,
    uint64                   *pCnt);

/* Function Name:
 *      dal_mango_of_flowCnt_clear
 * Description:
 *      Clear counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_OF_FT_PHASE - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 *      RT_ERR_INPUT       - invalid input parameter
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 */
extern int32
dal_mango_of_flowCnt_clear(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type);

/* Function Name:
 *      dal_mango_of_flowCntThresh_get
 * Description:
 *      Get packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 * Output:
 *      pThreshold - pointer buffer of interrupt trigger threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 */
extern int32
dal_mango_of_flowCntThresh_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   *pThreshold);

/* Function Name:
 *      dal_mango_of_flowCntThresh_set
 * Description:
 *      Set packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 *      threshold  - interrupt trigger threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 */
extern int32
dal_mango_of_flowCntThresh_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   threshold);

/* Function Name:
 *      dal_mango_of_ttlExcpt_get
 * Description:
 *      Get action for invalid IP/MPLS TTL.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_TRAP.
 */
extern int32
dal_mango_of_ttlExcpt_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_of_ttlExcpt_set
 * Description:
 *      Set action for invalid IP/MPLS TTL.
 * Input:
 *      unit   - unit id
 *      action - action to take
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_TRAP.
 */
extern int32
dal_mango_of_ttlExcpt_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_mango_of_maxLoopback_get
 * Description:
 *      Get maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 * Output:
 *      pTimes - pointer buffer of loopback times
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 */
extern int32
dal_mango_of_maxLoopback_get(uint32 unit, uint32 *pTimes);

/* Function Name:
 *      dal_mango_of_maxLoopback_set
 * Description:
 *      Set maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 *      times  - loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 */
extern int32
dal_mango_of_maxLoopback_set(uint32 unit, uint32 times);

/* Function Name:
 *      dal_mango_of_l2FlowTblMatchField_get
 * Description:
 *      Get match field of L2 flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L2 flow table can lookup twice for a certain packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 */
extern int32
dal_mango_of_l2FlowTblMatchField_get(uint32 unit, rtk_of_l2FlowTblMatchField_t *pField);

/* Function Name:
 *      dal_mango_of_l2FlowTblMatchField_set
 * Description:
 *      Set match field of L2 flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L2 flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 */
extern int32
dal_mango_of_l2FlowTblMatchField_set(uint32 unit, rtk_of_l2FlowTblMatchField_t field);

/* Function Name:
 *      dal_mango_of_l2FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Note:
 *      None
 */
extern int32
dal_mango_of_l2FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

/* Function Name:
 *      dal_mango_of_l2Entry_get
 * Description:
 *      Get L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L2 flow entry is 6 bit width.
 */
extern int32
dal_mango_of_l2FlowEntry_get(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l2FlowEntryNextValid_get
 * Description:
 *      Get next valid L2 flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l2 flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Note:
 *      (1) Input -1 for getting the first valid entry of l2 flow table.
 *      (2) The pScan_idx is both the input and output argument.
 *      (3) The pScan_idx must be multiple of 2.
 */
extern int32
dal_mango_of_l2FlowEntryNextValid_get(
    uint32               unit,
    int32                *pScan_idx,
    rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l2Entry_add
 * Description:
 *      Add a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 *      (3) The metadata in L2 flow entry is 6 bit width.
 */
extern int32
dal_mango_of_l2FlowEntry_add(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l2Entry_del
 * Description:
 *      Delete a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      None
 */
extern int32
dal_mango_of_l2FlowEntry_del(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l2Entry_delAll
 * Description:
 *      Delete all L2 flow entry.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_of_l2FlowEntry_delAll(uint32 unit);

/* Function Name:
 *      dal_mango_of_l2FlowEntryHitSts_get
 * Description:
 *      Get the L2 flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 */
extern int32
dal_mango_of_l2FlowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l2FlowEntry_t     *pEntry,
    uint32                   reset,
    uint32                   *pIsHit);

/* Function Name:
 *      dal_mango_of_l2FlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 */
extern int32
dal_mango_of_l2FlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo);

/* Function Name:
 *      dal_mango_of_l2FlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 */
extern int32
dal_mango_of_l2FlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo);

/* Function Name:
 *      dal_mango_of_l3FlowTblPri_get
 * Description:
 *      Get precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 * Output:
 *      pTable  - pointer to target table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 */
extern int32
dal_mango_of_l3FlowTblPri_get(uint32 unit, rtk_of_l3FlowTblList_t *pTable);

/* Function Name:
 *      dal_mango_of_l3FlowTblPri_set
 * Description:
 *      Set precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 *      table   - target table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 */
extern int32
dal_mango_of_l3FlowTblPri_set(uint32 unit, rtk_of_l3FlowTblList_t table);

/* Function Name:
 *      dal_mango_of_l3CamFlowTblMatchField_get
 * Description:
 *      Get match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
extern int32
dal_mango_of_l3CamFlowTblMatchField_get(uint32 unit, rtk_of_l3CamFlowTblMatchField_t *pField);

/* Function Name:
 *      dal_mango_of_l3CamFlowTblMatchField_set
 * Description:
 *      Set match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
extern int32
dal_mango_of_l3CamFlowTblMatchField_set(uint32 unit, rtk_of_l3CamFlowTblMatchField_t field);

/* Function Name:
 *      dal_mango_of_l3HashFlowTblMatchField_get
 * Description:
 *      Get match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
extern int32
dal_mango_of_l3HashFlowTblMatchField_get(uint32 unit, rtk_of_l3HashFlowTblMatchField_t *pField);

/* Function Name:
 *      dal_mango_of_l3HashFlowTblMatchField_set
 * Description:
 *      Set match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 */
extern int32
dal_mango_of_l3HashFlowTblMatchField_set(uint32 unit, rtk_of_l3HashFlowTblMatchField_t field);

/* Function Name:
 *      dal_mango_of_l3HashFlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 */
extern int32
dal_mango_of_l3HashFlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo);

/* Function Name:
 *      dal_mango_of_l3HashFlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 */
extern int32
dal_mango_of_l3HashFlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo);

/* Function Name:
 *      dal_mango_of_l3FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Note:
 *      None
 */
extern int32
dal_mango_of_l3FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_get
 * Description:
 *      Get a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 */
extern int32
dal_mango_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_add
 * Description:
 *      Add a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VALID_ENTRY_EXIST- Valid entry is existed
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 *      (4) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 */
extern int32
dal_mango_of_l3CamFlowEntry_add(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_del
 * Description:
 *      Delete the specified L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *      To delete an IPv4 SIP/DIP entry, valid index is 2N.
 *      To delete an IPv4 SIP+DIP entry, valid index is 2N.
 *      To delete an IPv6 SIP/DIP entry, valid index is 3N.
 *      To delete an IPv6 SIP+DIP entry, valid index is 6N.
 */
extern int32
dal_mango_of_l3CamFlowEntry_del(uint32 unit, uint32 entry_idx);

/* Function Name:
 *      dal_mango_of_l3CamFlowEntry_move
 * Description:
 *      Move the specified L3 CAM-based flow entries.
 * Input:
 *      unit    - unit id
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Note:
 *		In addition to entry data, hit status is also moved.
 */
extern int32
dal_mango_of_l3CamFlowEntry_move(uint32 unit, rtk_of_flowMove_t *pClrIdx);

/* Function Name:
 *      dal_mango_of_l3CamflowEntryHitSts_get
 * Description:
 *      Get the L3 Cam-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 */
extern int32
dal_mango_of_l3CamflowEntryHitSts_get(
    uint32    unit,
    uint32    entry_idx,
    uint32    reset,
    uint32    *pIsHit);

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_get
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L3 flow entry is 12 bit width.
 */
extern int32
dal_mango_of_l3HashFlowEntry_get(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l3HashFlowEntryNextValid_get
 * Description:
 *      Get next valid L3 Hash-based flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l3 hash-based flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Note:
 *      (1) Input -1 for getting the first valid entry of l3 hash-based flow table.
 *      (2) The pScan_idx is both the input and output argument.
 */
extern int32
dal_mango_of_l3HashFlowEntryNextValid_get(
    uint32                   unit,
    int32                    *pScan_idx,
    rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_add
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_EXIST  - entry existed
 *      RT_ERR_TBL_FULL     - The table is full
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries.
 *          An IPv4 SIP+DIP entry occupies two physical entries.
 *          An IPv6 SIP/DIP entry occupies three physical entries.
 *          An IPv6 SIP+DIP entry occupies six physical entries.
 *      (2) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *          The metadata in L3 flow entry is 12 bit width.
 *      (3) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 */
extern int32
dal_mango_of_l3HashFlowEntry_add(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_del
 * Description:
 *      Delete a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Note:
 *      None
 */
extern int32
dal_mango_of_l3HashFlowEntry_del(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_l3HashFlowEntry_delAll
 * Description:
 *      Delete all L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_of_l3HashFlowEntry_delAll(uint32 unit);

/* Function Name:
 *      dal_mango_of_l3HashflowEntryHitSts_get
 * Description:
 *      Get the L3 Hash-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 */
extern int32
dal_mango_of_l3HashflowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l3HashFlowEntry_t *pEntry,
    uint32                   reset,
    uint32                   *pIsHit);

/* Function Name:
 *      dal_mango_of_groupEntry_get
 * Description:
 *      Get a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type OF_GROUP_TYPE_INDIRECT, 'bucket_num' is not used.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.
 */
extern int32
dal_mango_of_groupEntry_get(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_groupEntry_set
 * Description:
 *      Set a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type INDIRECT, use OF_GROUP_TYPE_ALL with bucket_num=1.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.  Refer to rtk_of_actionBucket_set.
 */
extern int32
dal_mango_of_groupEntry_set(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry);

/* Function Name:
 *      dal_mango_of_groupTblHashPara_get
 * Description:
 *      Get parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 * Output:
 *      para    - pointer buffer of hash parameters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Each parameter can be included/excluded separately.
 */
extern int32
dal_mango_of_groupTblHashPara_get(uint32 unit, rtk_of_groupTblHashPara_t *para);

/* Function Name:
 *      dal_mango_of_groupTblHashPara_set
 * Description:
 *      Set parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 *      para    - pointer buffer of hash parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Each parameter can be included/excluded separately.
 */
extern int32
dal_mango_of_groupTblHashPara_set(uint32 unit, rtk_of_groupTblHashPara_t *para);

/* Function Name:
 *      dal_mango_of_actionBucket_get
 * Description:
 *      Get a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 */
extern int32
dal_mango_of_actionBucket_get(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry);

/* Function Name:
 *      dal_mango_of_actionBucket_set
 * Description:
 *      Set a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 */
extern int32
dal_mango_of_actionBucket_set(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry);

/* Function Name:
 *      dal_mango_of_trapTarget_get
 * Description:
 *      Get target device for trap packet.
 * Input:
 *      unit	- unit id
 * Output:
 *      pTarget - pointer to target device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 */
extern int32
dal_mango_of_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      dal_mango_of_trapTarget_set
 * Description:
 *      Set target device for trap packet.
 * Input:
 *      unit	- unit id
 *      target 	- target device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 */
extern int32
dal_mango_of_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      dal_mango_of_tblMissAction_get
 * Description:
 *      Get table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 * Output:
 *      pAct    - pointer to table miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *		None
 */
extern int32
dal_mango_of_tblMissAction_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t *pAct);

/* Function Name:
 *      dal_mango_of_tblMissAction_set
 * Description:
 *      Set table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 *      act     - table miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *		(1) OF_TBLMISS_ACT_FORWARD_NEXT_TBL isn't applicable for the last ingress flow table.
 *		(2) OF_TBLMISS_ACT_TRAP and OF_TBLMISS_ACT_FORWARD_NEXT_TBL aren't applicable for the egress flow table.
 */
extern int32
dal_mango_of_tblMissAction_set(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t act);

/* Function Name:
 *      dal_mango_of_flowCnt_get
 * Description:
 *      Get lookup and matched packet counters of flow tables.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      type     - counter type
 * Output:
 *      pCnt     - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The counter is cleared after reading.
 */
extern int32
dal_mango_of_flowTblCnt_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowTblCntType_t type, uint32 *pCnt);

#endif /* __DAL_MANGO_OPENFLOW_H__ */

