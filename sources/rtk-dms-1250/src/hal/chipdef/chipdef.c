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
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <ioal/mem32.h>
#include <hal/chipdef/chipdef.h>
#include <hal/common/halctrl.h>
#if defined (CONFIG_SDK_RTL8390)
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#endif
#if defined (CONFIG_SDK_RTL8380)
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#endif
#include <hal/mac/reg.h>
#if defined(CONFIG_SDK_RTL8380)
#include <private/drv/swcore/swcore_rtl8380.h>
#endif
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <osal/memory.h>
#include <osal/print.h>


/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_RTL8390)

/* Normal 8351 Chip Port Information */
static rt_portinfo_t rtl8351_port_info =
{
    {
        RT_FE_PORT   /*P0 */, RT_FE_PORT   /*P1 */, RT_FE_PORT  /*P2 */, RT_FE_PORT  /*P3 */, RT_FE_PORT   /*P4 */,
        RT_FE_PORT   /*P5 */, RT_FE_PORT   /*P6 */, RT_FE_PORT  /*P7 */, RT_FE_PORT  /*P8 */, RT_FE_PORT   /*P9 */,
        RT_FE_PORT   /*P10*/, RT_FE_PORT   /*P11*/, RT_FE_PORT  /*P12*/, RT_FE_PORT  /*P13*/, RT_FE_PORT   /*P14*/,
        RT_FE_PORT   /*P15*/, RT_FE_PORT   /*P16*/, RT_FE_PORT  /*P17*/, RT_FE_PORT  /*P18*/, RT_FE_PORT   /*P19*/,
        RT_FE_PORT   /*P20*/, RT_FE_PORT   /*P21*/, RT_FE_PORT  /*P22*/, RT_FE_PORT  /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT  /*P27*/, RT_PORT_NONE/*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_PORT_NONE/*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/, RT_PORT_NONE /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8351_port_info */

/* Normal 8352 Chip Port Information */
static rt_portinfo_t rtl8352_port_info =
{
    {
        RT_FE_PORT   /*P0 */, RT_FE_PORT   /*P1 */, RT_FE_PORT  /*P2 */, RT_FE_PORT  /*P3 */, RT_FE_PORT   /*P4 */,
        RT_FE_PORT   /*P5 */, RT_FE_PORT   /*P6 */, RT_FE_PORT  /*P7 */, RT_FE_PORT  /*P8 */, RT_FE_PORT   /*P9 */,
        RT_FE_PORT   /*P10*/, RT_FE_PORT   /*P11*/, RT_FE_PORT  /*P12*/, RT_FE_PORT  /*P13*/, RT_FE_PORT   /*P14*/,
        RT_FE_PORT   /*P15*/, RT_FE_PORT   /*P16*/, RT_FE_PORT  /*P17*/, RT_FE_PORT  /*P18*/, RT_FE_PORT   /*P19*/,
        RT_FE_PORT   /*P20*/, RT_FE_PORT   /*P21*/, RT_FE_PORT  /*P22*/, RT_FE_PORT  /*P23*/, RT_PORT_NONE /*P24*/,
        RT_PORT_NONE /*P25*/, RT_PORT_NONE /*P26*/, RT_PORT_NONE/*P27*/, RT_PORT_NONE/*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_GE_PORT  /*P48*/, RT_GE_PORT   /*P49*/,
        RT_GE_PORT   /*P50*/, RT_GE_PORT   /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8352_port_info */

/* Normal 8353 Chip Port Information */
static rt_portinfo_t rtl8353_port_info =
{
    {
        RT_FE_PORT   /*P0 */, RT_FE_PORT   /*P1 */, RT_FE_PORT  /*P2 */, RT_FE_PORT  /*P3 */, RT_FE_PORT   /*P4 */,
        RT_FE_PORT   /*P5 */, RT_FE_PORT   /*P6 */, RT_FE_PORT  /*P7 */, RT_FE_PORT  /*P8 */, RT_FE_PORT   /*P9 */,
        RT_FE_PORT   /*P10*/, RT_FE_PORT   /*P11*/, RT_FE_PORT  /*P12*/, RT_FE_PORT  /*P13*/, RT_FE_PORT   /*P14*/,
        RT_FE_PORT   /*P15*/, RT_FE_PORT   /*P16*/, RT_FE_PORT  /*P17*/, RT_FE_PORT  /*P18*/, RT_FE_PORT   /*P19*/,
        RT_FE_PORT   /*P20*/, RT_FE_PORT   /*P21*/, RT_FE_PORT  /*P22*/, RT_FE_PORT  /*P23*/, RT_FE_PORT   /*P24*/,
        RT_FE_PORT   /*P25*/, RT_FE_PORT   /*P26*/, RT_FE_PORT  /*P27*/, RT_FE_PORT  /*P28*/, RT_FE_PORT   /*P29*/,
        RT_FE_PORT   /*P30*/, RT_FE_PORT   /*P31*/, RT_FE_PORT  /*P32*/, RT_FE_PORT  /*P33*/, RT_FE_PORT   /*P34*/,
        RT_FE_PORT   /*P35*/, RT_FE_PORT   /*P36*/, RT_FE_PORT  /*P37*/, RT_FE_PORT  /*P38*/, RT_FE_PORT   /*P39*/,
        RT_FE_PORT   /*P40*/, RT_FE_PORT   /*P41*/, RT_FE_PORT  /*P42*/, RT_FE_PORT  /*P43*/, RT_FE_PORT   /*P44*/,
        RT_FE_PORT   /*P45*/, RT_FE_PORT   /*P46*/, RT_FE_PORT  /*P47*/, RT_GE_PORT  /*P48*/, RT_GE_PORT   /*P49*/,
        RT_GE_PORT   /*P50*/, RT_GE_PORT   /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8353_port_info */

/* Normal 8391 Chip Port Information */
static rt_portinfo_t rtl8391_port_info =
{
    {
        RT_GE_PORT   /*P0 */, RT_GE_PORT   /*P1 */, RT_GE_PORT  /*P2 */, RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */, RT_GE_PORT   /*P6 */, RT_GE_PORT  /*P7 */, RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/, RT_GE_PORT   /*P11*/, RT_GE_PORT  /*P12*/, RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/, RT_GE_PORT   /*P16*/, RT_GE_PORT  /*P17*/, RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/, RT_GE_PORT   /*P21*/, RT_GE_PORT  /*P22*/, RT_GE_PORT  /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT  /*P27*/, RT_PORT_NONE/*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_PORT_NONE/*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/, RT_PORT_NONE /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8391_port_info */

/* Normal 8392 Chip Port Information */
static rt_portinfo_t rtl8392_port_info =
{
    {
        RT_GE_PORT   /*P0 */, RT_GE_PORT   /*P1 */, RT_GE_PORT  /*P2 */, RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */, RT_GE_PORT   /*P6 */, RT_GE_PORT  /*P7 */, RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/, RT_GE_PORT   /*P11*/, RT_GE_PORT  /*P12*/, RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/, RT_GE_PORT   /*P16*/, RT_GE_PORT  /*P17*/, RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/, RT_GE_PORT   /*P21*/, RT_GE_PORT  /*P22*/, RT_GE_PORT  /*P23*/, RT_PORT_NONE /*P24*/,
        RT_PORT_NONE /*P25*/, RT_PORT_NONE /*P26*/, RT_PORT_NONE/*P27*/, RT_PORT_NONE/*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_GE_PORT  /*P48*/, RT_GE_PORT   /*P49*/,
        RT_GE_PORT   /*P50*/, RT_GE_PORT   /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8392_port_info */

/* Normal 8393 Chip Port Information */
static rt_portinfo_t rtl8393_port_info =
{
    /* Normal 8393 Chip Port Information */
    {
        RT_GE_PORT   /*P0 */, RT_GE_PORT   /*P1 */, RT_GE_PORT  /*P2 */, RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */, RT_GE_PORT   /*P6 */, RT_GE_PORT  /*P7 */, RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/, RT_GE_PORT   /*P11*/, RT_GE_PORT  /*P12*/, RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/, RT_GE_PORT   /*P16*/, RT_GE_PORT  /*P17*/, RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/, RT_GE_PORT   /*P21*/, RT_GE_PORT  /*P22*/, RT_GE_PORT  /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT  /*P27*/, RT_GE_PORT  /*P28*/, RT_GE_PORT   /*P29*/,
        RT_GE_PORT   /*P30*/, RT_GE_PORT   /*P31*/, RT_GE_PORT  /*P32*/, RT_GE_PORT  /*P33*/, RT_GE_PORT   /*P34*/,
        RT_GE_PORT   /*P35*/, RT_GE_PORT   /*P36*/, RT_GE_PORT  /*P37*/, RT_GE_PORT  /*P38*/, RT_GE_PORT   /*P39*/,
        RT_GE_PORT   /*P40*/, RT_GE_PORT   /*P41*/, RT_GE_PORT  /*P42*/, RT_GE_PORT  /*P43*/, RT_GE_PORT   /*P44*/,
        RT_GE_PORT   /*P45*/, RT_GE_PORT   /*P46*/, RT_GE_PORT  /*P47*/, RT_GE_PORT  /*P48*/, RT_GE_PORT   /*P49*/,
        RT_GE_PORT   /*P50*/, RT_GE_PORT   /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8393_port_info */

/* Normal 8396 Chip Port Information */
static rt_portinfo_t rtl8396_port_info =
{
    /* Normal 8396 Chip Port Information */
    {
        RT_GE_PORT   /*P0 */, RT_GE_PORT   /*P1 */, RT_GE_PORT  /*P2 */, RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */, RT_GE_PORT   /*P6 */, RT_GE_PORT  /*P7 */, RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/, RT_GE_PORT   /*P11*/, RT_GE_PORT  /*P12*/, RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/, RT_GE_PORT   /*P16*/, RT_GE_PORT  /*P17*/, RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/, RT_GE_PORT   /*P21*/, RT_GE_PORT  /*P22*/, RT_GE_PORT  /*P23*/, RT_10GE_PORT /*P24*/,
        RT_PORT_NONE /*P25*/, RT_PORT_NONE /*P26*/, RT_PORT_NONE/*P27*/, RT_PORT_NONE/*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_10GE_PORT /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_GE_PORT  /*P48*/, RT_GE_PORT   /*P49*/,
        RT_GE_PORT   /*P50*/, RT_GE_PORT   /*P51*/, RT_CPU_PORT /*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
    },
}; /* end of rtl8396_port_info */


/* Normal 8390 Chip Port Information */
static rt_register_capacity_t rtl8390m_capacityInfo =
{
    .max_num_of_mirror              = 4,
    .max_num_of_trunk               = 16,
    .max_num_of_trunkMember         = 8,
    .max_num_of_trunk_algo          = 4,
    .trunk_algo_shift_max           = 4,
    .max_num_of_dumb_trunkMember    = 8,
    .max_num_of_trunkHashVal        = 32,
    .max_num_of_msti                = 256,
    .max_num_of_metering            = 512,
    .max_num_of_meter_block         = 16,
    .max_num_of_pie_block           = 18,
    .max_num_of_pie_block_templateSelector  = 2,
    .max_num_of_pie_blockSize       = 128,
    .max_num_of_pie_counter         = 1024,
    .max_num_of_pie_template        = 8,
    .pie_user_template_id_min       = 5,
    .pie_user_template_id_max       = 7,
    .max_num_of_field_selector      = 12,
    .max_num_of_range_check_srcPort = 16,
    .max_num_of_range_check_dstPort = 16,
    .max_num_of_range_check_ip      = 8,
    .max_num_of_range_check_vid     = 32,
    .max_num_of_range_check_l4Port  = 8,
    .max_num_of_range_check_pktLen  = 8,
    .max_num_of_l2_hashdepth        = 4,
    .max_num_of_cpuQueue            = 8,
    .max_num_of_queue               = 8,
    .min_num_of_queue               = 1,
    .max_num_of_svl                 = 0,
    .max_num_of_cvlan_tpid          = 4,
    .max_num_of_svlan_tpid          = 4,
    .max_num_of_evlan_tpid          = 1,
    .max_num_of_route_host_addr     = 2048,
    .max_num_of_route_switch_addr   = 16,
    .tpid_entry_idx_max             = 3,
    .tpid_entry_mask_max            = 0xf,
    .protocol_vlan_idx_max          = 7,
    .vlan_fid_max                   = 255,
    .flowctrl_thresh_max            = 0xFFF,
    .flowctrl_pauseOn_page_packet_max       = 0xF,
    .flowctrl_pauseOn_page_packet_len_max   = 0xFFFF,
    .pri_of_selection_max           = 7,
    .pri_of_selection_min           = 0,
    .pri_sel_group_index_max        = 3,
    .queue_weight_max               = 1023,
    .rate_of_bandwidth_max          = 0xFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max   = 3,
    .thresh_of_igr_bw_flowctrl_min  = 22,                   //[FIXED ME]
    .thresh_of_igr_bw_flowctrl_max  = 0xFFFF,
    .max_num_of_fastPath_of_rate    = 3,
    .rate_of_storm_control_max      = 0xFFFFF,
    .rate_of_storm_proto_control_max= 0xFF,
    .busrt_of_igr_bandwidth_max     = 0xFFFF,
    .busrt_of_bandwidth_max         = 0xFFFF,
    .burst_rate_of_storm_control_min= 1700,
    .burst_rate_of_storm_control_max= 0xFFFF,
    .burst_rate_of_10ge_storm_control_min = 2650,
    .burst_rate_of_10ge_storm_control_max = 0xFFFFF,
    .burst_size_of_acl_meter_min    = 17,
    .burst_size_of_acl_meter_max    = 0xFFFF,
    .internal_priority_max          = 7,
    .drop_precedence_max            = 2,
    .priority_remap_group_idx_max   = 3,
    .priority_remark_group_idx_max  = 0,
    .wred_weight_max                = 0x3FF,
    .wred_mpd_max                   = 0xF,
    .wred_drop_probability_max      = 0xFF,
    .acl_rate_max                   = 0xFFFF,
    .max_num_of_l2_hash_algo        = 2,
    .l2_learn_limit_cnt_max         = 0x4040,
    .l2_learn_limit_cnt_wo_cam_max  = 0x4000,
    .l2_learn_limit_cnt_disable     = 0x7FFF,
    .l2_fid_learn_limit_entry_max   = 32,
    .l2_ntfy_bp_thr_max             = 1024,
    .max_num_of_vrf                 = 0,
    .max_num_of_intf                = 0,
    .max_num_of_intf_mtu            = 0,
    .max_num_of_router_mac          = 0,
    .max_num_of_l3_ecmp             = 0,
    .max_num_of_l3_nexthop          = 0,
    .max_num_of_l3_host             = 0,
    .max_num_of_l3_route            = 0,
    .max_num_of_tunnel              = 0,
    .max_num_of_tunnel_qos_profile  = 0,
    .eee_queue_thresh_max           = 0xFF,
    .sec_minIpv6FragLen_max         = 0xFFFF,
    .sec_maxPingLen_max             = 0xFFFF,
    .sec_smurfNetmaskLen_max        = 32,
    .max_num_of_ip_mac_bind_entry   = 0,
    .sflow_rate_max                 = 0xFFFF,
    .rate_of_bandwidth_max_fe_port  = 0x186A,
    .rate_of_bandwidth_max_ge_port  = 0xF424,
    .rate_of_bandwidth_max_10ge_port= 0x98968,
    .max_num_of_c2sc_entry          = 1024,
    .max_num_of_c2sc_blk_entry      = 256,
    .max_num_of_c2sc_blk            = 4,
    .max_num_of_sc2c_entry          = 1024,
    .max_num_of_vlan_prof           = 8,
    .max_num_of_vlan_group          = 8,
    .max_frame_len                  = 12288,
    .max_num_of_mcast_entry         = 4096,
    .max_num_of_vlan_port_iso_entry = 16,
    .max_num_of_mpls_lib            = 256,
    .max_num_of_led_entity          = 3,
    .miim_page_id_max               = 8191,
    .max_num_of_dying_gasp_pkt_cnt  = 7,
    .dying_gasp_sustain_time_max    = 0xFFFF,
    .max_num_of_rma_user_defined    = 2,
};

/* Normal 8390 Chip PER_PORT block information */
static rt_macPpInfo_t rtl8390_macPpInfo[] =
{
    {
      /* lowerbound_addr */ 0x8000,
      /* upperbound_addr */ 0x9A7C,
      /* interval */          0x80,
    },
};

/* Definition Structure & Supported Mode Lists */ // add
typedef enum rt8390_supported_mode_e
{
    RT8390_MODE_48G_4G = 0,
    RT8390_MODE_48G_4G_APP_48G,
    RT8390_MODE_48G_4G_APP_44G_4G,
    RT8390_MODE_48G_4G_APP_24G_4G,
    RT8390_MODE_48G_4G_APP_20G_4G,
    RT8390_MODE_48G_2G = 8,
    RT8390_MODE_48G_2G_APP_24G_2G,
    RT8390_MODE_24G_2TG_RXAUI = 16,
    RT8390_MODE_24G_2TG_RXAUI_APP_20G_4G_2TG_RXAUI,
    RT8390_MODE_24G_2TG_XFP = 24,
    RT8390_MODE_24G_2TG_XFP_APP_20G_4G_2TG_XFP,
    RT8390_MODE_24G_2TG_XFP_APP_24FB_2TG_XFP,
    RT8390_MODE_48FE_4G = 32,
    RT8390_MODE_48FE_4G_APP_24FE_4G,
    RT8390_MODE_48FE_2G = 40,
    RT8390_MODE_48FE_2G_APP_24FE_2G,
}rt8390_supported_mode_t;
#endif  /* end of defined(CONFIG_SDK_RTL8390) */

/******************************************************/
/************Add CONFIG_SDK_RTL8380 in here****************/
/*****************************************************/
#if defined(CONFIG_SDK_RTL8380)
/* Normal 8380 Chip Port Information */
static rt_portinfo_t rtl8380_port_info_24G_4BX =
{
    {
        RT_GE_PORT   /*P0 */, RT_GE_PORT   /*P1 */, RT_GE_PORT  /*P2 */, RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */, RT_GE_PORT   /*P6 */, RT_GE_PORT  /*P7 */, RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/, RT_GE_PORT   /*P11*/, RT_GE_PORT  /*P12*/, RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/, RT_GE_PORT   /*P16*/, RT_GE_PORT  /*P17*/, RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/, RT_GE_PORT   /*P21*/, RT_GE_PORT  /*P22*/, RT_GE_PORT  /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT  /*P27*/, RT_CPU_PORT /*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_PORT_NONE/*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/, RT_PORT_NONE /*P51*/, RT_PORT_NONE/*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl8380_port_info_16G_4BX =
{
    {
        RT_PORT_NONE /*P0 */, RT_PORT_NONE /*P1 */, RT_PORT_NONE /*P2 */, RT_PORT_NONE /*P3 */, RT_PORT_NONE /*P4 */,
        RT_PORT_NONE /*P5 */, RT_PORT_NONE /*P6 */, RT_PORT_NONE /*P7 */, RT_GE_PORT   /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/, RT_GE_PORT   /*P11*/, RT_GE_PORT   /*P12*/, RT_GE_PORT   /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/, RT_GE_PORT   /*P16*/, RT_GE_PORT   /*P17*/, RT_GE_PORT   /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/, RT_GE_PORT   /*P21*/, RT_GE_PORT   /*P22*/, RT_GE_PORT   /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT   /*P27*/, RT_CPU_PORT  /*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE /*P32*/, RT_PORT_NONE /*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE /*P37*/, RT_PORT_NONE /*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE /*P42*/, RT_PORT_NONE /*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE /*P47*/, RT_PORT_NONE /*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/, RT_PORT_NONE /*P51*/, RT_PORT_NONE /*P52*/, RT_PORT_NONE /*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE /*P57*/, RT_PORT_NONE /*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE /*P62*/, RT_PORT_NONE /*P63*/
     },
};

static rt_portinfo_t rtl8380_port_info_24FE_4G =
{
    {
        RT_FE_PORT   /*P0 */, RT_FE_PORT   /*P1 */, RT_FE_PORT  /*P2 */, RT_FE_PORT  /*P3 */, RT_FE_PORT   /*P4 */,
        RT_FE_PORT   /*P5 */, RT_FE_PORT   /*P6 */, RT_FE_PORT  /*P7 */, RT_FE_PORT  /*P8 */, RT_FE_PORT   /*P9 */,
        RT_FE_PORT   /*P10*/, RT_FE_PORT   /*P11*/, RT_FE_PORT  /*P12*/, RT_FE_PORT  /*P13*/, RT_FE_PORT   /*P14*/,
        RT_FE_PORT   /*P15*/, RT_FE_PORT   /*P16*/, RT_FE_PORT  /*P17*/, RT_FE_PORT  /*P18*/, RT_FE_PORT   /*P19*/,
        RT_FE_PORT   /*P20*/, RT_FE_PORT   /*P21*/, RT_FE_PORT  /*P22*/, RT_FE_PORT  /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT  /*P27*/, RT_CPU_PORT /*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE/*P32*/, RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE/*P37*/, RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE/*P42*/, RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE/*P47*/, RT_PORT_NONE/*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/, RT_PORT_NONE /*P51*/, RT_PORT_NONE/*P52*/, RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE/*P57*/, RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE/*P62*/, RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl8380_port_info_16FE_4G =
{
    {
        RT_PORT_NONE /*P0 */, RT_PORT_NONE /*P1 */, RT_PORT_NONE /*P2 */, RT_PORT_NONE /*P3 */, RT_PORT_NONE /*P4 */,
        RT_PORT_NONE /*P5 */, RT_PORT_NONE /*P6 */, RT_PORT_NONE /*P7 */, RT_FE_PORT   /*P8 */, RT_FE_PORT   /*P9 */,
        RT_FE_PORT   /*P10*/, RT_FE_PORT   /*P11*/, RT_FE_PORT   /*P12*/, RT_FE_PORT   /*P13*/, RT_FE_PORT   /*P14*/,
        RT_FE_PORT   /*P15*/, RT_FE_PORT   /*P16*/, RT_FE_PORT   /*P17*/, RT_FE_PORT   /*P18*/, RT_FE_PORT   /*P19*/,
        RT_FE_PORT   /*P20*/, RT_FE_PORT   /*P21*/, RT_FE_PORT   /*P22*/, RT_FE_PORT   /*P23*/, RT_GE_PORT   /*P24*/,
        RT_GE_PORT   /*P25*/, RT_GE_PORT   /*P26*/, RT_GE_PORT   /*P27*/, RT_CPU_PORT  /*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/, RT_PORT_NONE /*P31*/, RT_PORT_NONE /*P32*/, RT_PORT_NONE /*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/, RT_PORT_NONE /*P36*/, RT_PORT_NONE /*P37*/, RT_PORT_NONE /*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/, RT_PORT_NONE /*P41*/, RT_PORT_NONE /*P42*/, RT_PORT_NONE /*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/, RT_PORT_NONE /*P46*/, RT_PORT_NONE /*P47*/, RT_PORT_NONE /*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/, RT_PORT_NONE /*P51*/, RT_PORT_NONE /*P52*/, RT_PORT_NONE /*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/, RT_PORT_NONE /*P56*/, RT_PORT_NONE /*P57*/, RT_PORT_NONE /*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/, RT_PORT_NONE /*P61*/, RT_PORT_NONE /*P62*/, RT_PORT_NONE /*P63*/
     },
};

/* Normal 8380 Chip Port Information */
static rt_register_capacity_t rtl8380m_capacityInfo =
{
    .max_num_of_mirror              = 4,
    .max_num_of_trunk               = 8,
    .max_num_of_trunkMember         = 8,
    .max_num_of_trunk_algo          = 2,
    .trunk_algo_shift_max           = 3,
    .max_num_of_dumb_trunkMember    = 8,
    .max_num_of_trunkHashVal        = 16,
    .max_num_of_msti                = 64,
    .max_num_of_metering            = 256,
    .burst_size_of_acl_meter_max = 0xffff,
    .max_num_of_pie_block           = 12,
    .max_num_of_pie_block_templateSelector  = 3,
    .max_num_of_pie_blockSize       = 128,
    .max_num_of_pie_counter         = 128,
    .max_num_of_pie_template        = 8,
    .max_num_of_pie_template_field  = 12,
    .pie_user_template_id_min       = 5 ,
    .pie_user_template_id_max       = 7 ,
    .max_num_of_field_selector      = 4,
    .max_num_of_range_check_srcPort = 16,
    .max_num_of_range_check_dstPort = 0,
    .max_num_of_range_check_ip      = 8,
    .max_num_of_range_check_vid     = 16,
    .max_num_of_range_check_l4Port  = 16,
    .max_num_of_range_check_pktLen  = 16,
    .max_num_of_l2_hashdepth        = 4,
    .max_num_of_cpuQueue            = 8,
    .max_num_of_queue               = 8,
    .min_num_of_queue               = 1,
    .max_num_of_igrQueue            = 4,
    .min_num_of_igrQueue            = 1,
    .max_num_of_svl                 = 0,
    .max_num_of_cvlan_tpid          = 4,
    .max_num_of_svlan_tpid          = 4,
    .max_num_of_evlan_tpid          = 1,
    .max_num_of_route_host_addr 	= 512,
    .tpid_entry_idx_max             = 3,
    .tpid_entry_mask_max            = 0xf,
    .protocol_vlan_idx_max          = 0,
    .vlan_fid_max                   = 63,
    .flowctrl_thresh_max            = 0x7FF,
    .flowctrl_pauseOn_page_packet_max   = 0x1FF,
    .pri_of_selection_max           = 7,
    .pri_of_selection_min           = 0,
    .pri_sel_group_index_max        = 3,
    .queue_weight_max               = 127,
    .rate_of_bandwidth_max          = 0x3FFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max   = 3,
    .thresh_of_igr_bw_flowctrl_max  = 0x3FF,
    .max_num_of_fastPath_of_rate    = 0,
    .rate_of_storm_control_max      = 0x3FFFF,
    .burst_rate_of_storm_control_max= 0xFFFF,
    .internal_priority_max          = 7,
    .drop_precedence_max            = 0,
    .priority_remap_group_idx_max   = 0,
    .priority_remark_group_idx_max  = 0,
    .wred_weight_max                = 0,
    .wred_mpd_max                   = 0,
    .acl_rate_max                   = 0x3FFFF,
    .max_num_of_l2_hash_algo        = 2,
    .l2_learn_limit_cnt_max         = 0x2040,
    .l2_learn_limit_cnt_wo_cam_max  = 0x2000,
    .l2_learn_limit_cnt_disable     = 0x3FFF,
    .l2_fid_learn_limit_entry_max   = 8,
    .eee_queue_thresh_max           = 0xFFFF,
    .sec_minIpv6FragLen_max         = 0xFFFF,
    .sec_maxPingLen_max             = 0xFFFF,
    .sec_smurfNetmaskLen_max        = 32,
    .max_num_of_ip_mac_bind_entry   = 0,
    .sflow_rate_max                 = 0xFFFF,
    .rate_of_bandwidth_max_fe_port  = 0x186A,
    .rate_of_bandwidth_max_ge_port  = 0xF424,
    .rate_of_bandwidth_max_10ge_port= 0,
    .max_num_of_mcast_entry         = 512,
    .max_num_of_vlan_port_iso_entry = 16,
    .max_num_of_sc2c_entry          = 128,
    .max_num_of_vlan_prof           = 8,
    .max_num_of_vlan_group          = 8,
    .max_frame_len                  = 10240,
    .max_num_of_led_entity          = 3,
    .miim_page_id_max               = 0xfff,
    .busrt_of_igr_bandwidth_max     = 0xFFFF,
    .busrt_of_bandwidth_max         = 0xFFFF,
};

/* Normal 8380 Chip PER_PORT block information */
static rt_macPpInfo_t rtl8380_macPpInfo[] =
{
    {
      /* lowerbound_addr */ 0xd560,
      /* upperbound_addr */ 0xe3df,
      /* interval */         0x80,
    },
    {
      /* lowerbound_addr */ 0xbfe0,
      /* upperbound_addr */ 0xce5f,
      /* interval */         0x80,
    },
};
#endif  /* end of defined(CONFIG_SDK_RTL8380) */

#if defined(CONFIG_SDK_RTL9310)
/* Normal 9310 Chip Port Information */
static rt_register_capacity_t rtl9310_capacityInfo =
{
#if defined(CONFIG_SDK_FPGA_PLATFORM)
    .max_num_of_mirror              = 4,
    .max_num_of_trunk               = 26,
    .max_num_of_trunk_stacking_mode = 128,
    .max_num_of_local_trunk       = 52,
    .max_num_of_stack_trunk      = 8,
    .max_num_of_trunkMember         = 8,
    .max_num_of_trunk_algo          = 2,
    .trunk_algo_shift_max           = 5,
    .max_num_of_dumb_trunkMember    = 8,
    .max_num_of_trunkHashVal        = 64,
    .max_num_of_msti                = 128,
    /* Meter */
    .max_num_of_metering            = 512,
    .max_num_of_meter_block         = 16,
    .rate_of_meter_max              = 0xFFFFF,
    .burst_size_of_acl_meter_max    = 0x1FFFF,
    /* PIE */
    .max_num_of_pie_block           = 3,
    .max_num_of_pie_blockSize       = 128,
    .max_num_of_pie_template        = 10,
    .pie_user_template_id_min       = 5,
    .pie_user_template_id_max       = 9,
    .max_num_of_field_selector      = 2,
    .max_offset_of_field_selector   = 100,
    .max_num_of_pie_block_templateSelector  = 2,
    .max_num_of_pie_block_group     = 3,
    .max_num_of_pie_block_logic     = 32,
    .max_num_of_pie_template_field  = 14,
    /* Range check */
    .max_num_of_range_check_ip      = 8,
    .max_num_of_range_check         = 16,
    /* ACL */
    .max_num_of_metaData            = 0xFFF,
    .max_num_of_l2_hashdepth        = 2,
    .max_num_of_l2_hashwidth        = 4,
    .max_num_of_queue               = 4,
    .min_num_of_queue               = 1,
    .max_num_of_cpuQueue            = 32,
    .max_num_of_stackQueue          = 12,
    .max_num_of_igrQueue            = 3,
    .min_num_of_igrQueue            = 1,
    .max_num_of_svl                 = 4096,
    .max_num_of_cvlan_tpid          = 4,
    .max_num_of_svlan_tpid          = 4,
    .max_num_of_evlan_tpid          = 1,
    .max_num_of_route_host_addr     = 2048,
    .max_num_of_route_switch_addr   = 16,
    .tpid_entry_idx_max             = 3,
    .tpid_entry_mask_max            = 0xf,
    .protocol_vlan_idx_max          = 7,
    .vlan_fid_max                   = 255,
    /* flow control */
    .flowctrl_thresh_max            = 5840,
    .flowctrl_pauseOn_page_packet_max       = 5840,
    .flowctrl_pauseOn_page_packet_len_max   = 0xFFFF,
    .flowctrl_egr_queue_drop_group_idx_max  = 2,
    /* QoS */
    .dp_of_selection_max            = 3,
    .dp_of_selection_min            = 0,
    .pri_of_selection_max           = 12,
    .pri_of_selection_min           = 0,
    .pri_sel_group_index_max        = 3,
    .queue_weight_max               = 127,
    .queue_weight_min               = 1,
    .igr_queue_weight_max           = 31,
    .max_num_of_fastPath_of_rate    = 3,
    /* bandwidth */
    .rate_of_egr_bandwidth_min      = 0x1,
    .rate_of_bandwidth_max          = 0xFFFFF,
    .rate_of_bandwidth_max_fe_port  = 0x186A,
    .rate_of_bandwidth_max_ge_port  = 0xF424,
    .rate_of_bandwidth_max_10ge_port= 0x98968,
    .busrt_of_igr_bandwidth_max     = 0xFFFF,
    .busrt_of_bandwidth_max         = 0xFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max   = 3,
    .thresh_of_igr_bw_flowctrl_min  = 22,
    .thresh_of_igr_bw_flowctrl_max  = 0xFFFF,
    /* Storm control */
    .rate_of_storm_control_max      = 0xFFFFFF,
    .rate_of_storm_proto_control_max= 0x1FF,
    .burst_rate_of_storm_control_min= 1700,
    .burst_rate_of_storm_control_max= 0xFFFF,
    .burst_rate_of_10ge_storm_control_min= 2650,
    .burst_rate_of_10ge_storm_control_max    = 0xFFFFF,
    .burst_rate_of_storm_proto_control_max= 0xFF,
    .internal_priority_max          = 7,
    .drop_precedence_max            = 2,
    .priority_remap_group_idx_max   = 3,
    .priority_remark_group_idx_max  = 0,
    .wred_weight_max                = 0x3FF,
    .wred_mpd_max                   = 0xF,
    .wred_drop_probability_max      = 0xFF,
    .max_num_of_l2_hash_algo        = 2,
    .l2_learn_limit_cnt_max         = 0x4002,
    .l2_learn_limit_cnt_wo_cam_max  = 0x4000,
    .l2_learn_limit_cnt_disable     = 0xFFFF,
    .l2_fid_learn_limit_entry_max   = 32,
    .l2_ntfy_bp_thr_max             = 2048,
    .max_num_of_vrf                 = 256,
    .max_num_of_intf                = 1024,
    .max_num_of_intf_mtu            = 16,
    .max_num_of_intf_mtu_value      = (16384 - 1),  /* 0 ~ 16383 */
    .max_num_of_router_mac          = 1024,
    .max_num_of_l3_ecmp             = 256,
    .max_num_of_l3_ecmp_hash_idx    = 16,
    .max_num_of_l3_ecmp_nexthop     = 8,
    .max_num_of_l3_nexthop          = 8192,
    .max_num_of_l3_host             = 12288,
    .max_num_of_l3_route            = 12288,
    .max_num_of_l3_conflict_host    = 16,
    .max_num_of_mcast_group_nexthop = 1024 + 2048 + 2, //1k intf + 2k BBSID + 1 bridge nexthop info + 1 l3 stacking forwad portmask
    .max_num_of_tunnel              = 384,
    .max_num_of_tunnel_qos_profile  = 64,
    .max_num_of_mcast_entry         = 4096,
    .eee_queue_thresh_max           = 0xFF,
    .sec_minIpv6FragLen_max         = 0xFFFF,
    .sec_maxPingLen_max             = 0xFFFF,
    .sec_smurfNetmaskLen_max        = 32,
    .max_num_of_ip_mac_bind_entry   = (1024 + 2048),
    .sflow_rate_max                 = 0xFFFF,
    .max_num_of_c2sc_entry          = 384,
    .max_num_of_c2sc_blk_entry      = 128,
    .max_num_of_c2sc_blk            = 3,
    .max_num_of_c2sc_range_check_vid = 32,
    .max_num_of_c2sc_range_check_set = 1,
    .max_num_of_sc2c_entry          = 4,
    .max_num_of_sc2c_range_check_vid = 32,
    .max_num_of_sc2c_range_check_set = 1,
    .max_num_of_vlan_prof           = 16,
    .max_num_of_vlan_group          = 16,
    .max_frame_len                  = 12288,
    .max_num_of_mcast_entry         = 4096,
    .max_num_of_vlan_port_iso_entry = 32,
    /* MPLS */
    .max_num_of_mpls_encap          = 2048,
    .max_num_of_mpls_decap          = 2048,
    .max_num_of_mpls_decap_cam      = 64,
    .max_num_of_mpls_nextHop        = 8192,
    .max_num_of_mpls_hashdepth      = 8,
    .max_num_of_led_entity          = 4,
    .miim_page_id_max               = 8191,
    .max_num_of_dying_gasp_pkt_cnt  = 7,
    .dying_gasp_sustain_time_max    = 0xFFFF,
    .max_num_of_rma_user_defined    = 4,
    .time_freq_max                   = 0xFFFFFFF,
    .max_num_of_ethdm_rx_timestamp  = 64,
    /* OpenFlow */
    .byte_cntr_th_max               = 0x3FFFFFFFFFFULL,
    .pkt_cntr_th_max                = 0xFFFFFFFFFULL,
    .max_num_of_igr_flowtbl         = 4,
    .max_num_of_virtual_igr_flowtbl = 256,
    .loopback_time_max              = 63,
    .max_num_of_group_entry         = 2048,
    .max_num_of_group_entry_bucket  = 128,
    .max_num_of_action_bucket       = 8192,
    .max_num_of_dmac_entry          = 1024,
    .max_num_of_bpe_pvid_hashdepth  = 8,
    /* L2 tunnel */
    .max_num_of_l2tnl_lst           = 32,
    .max_num_of_l2tnl_node          = 2048,
    /* stacking */
    .max_num_of_stack_port          = 16,
    .max_num_of_stack_dev       = 16,
#else
    .max_num_of_mirror              = 4,
    .max_num_of_trunk               = 128,
    .max_num_of_trunk_stacking_mode = 128,
    .max_num_of_local_trunk       = 52,
    .max_num_of_stack_trunk      = 8,
    .max_num_of_trunkMember         = 8,
    .max_num_of_trunk_algo          = 2,
    .trunk_algo_shift_max           = 5,
    .max_num_of_dumb_trunkMember    = 8,
    .max_num_of_trunkHashVal        = 64,
    .max_num_of_msti                = 128,
    /* Meter */
    .max_num_of_metering            = 512,
    .max_num_of_meter_block         = 16,
    .rate_of_meter_max              = 0xFFFFF,
    .burst_size_of_acl_meter_max    = 0x1FFFF,
    /* PIE */
    .max_num_of_pie_block           = 32,
    .max_num_of_pie_blockSize       = 128,
    .max_num_of_pie_counter         = 4096,
    .max_num_of_pie_template        = 10,
    .pie_user_template_id_min       = 5,
    .pie_user_template_id_max       = 9,
    .max_num_of_field_selector      = 14,
    .max_offset_of_field_selector   = 100,
    .max_num_of_pie_block_templateSelector  = 2,
    .max_num_of_pie_block_group     = 16,
    .max_num_of_pie_block_logic     = 32,
    .max_num_of_pie_template_field  = 14,
    /* Range check */
    .max_num_of_range_check_ip      = 8,
    .max_num_of_range_check         = 16,
    /* ACL */
    .max_num_of_metaData            = 0xFFF,
    .max_num_of_l2_hashdepth        = 4,
    .max_num_of_l2_hashwidth        = 4,
    .max_num_of_queue               = 8,
    .min_num_of_queue               = 1,
    .max_num_of_cpuQueue            = 32,
    .max_num_of_stackQueue          = 12,
    .max_num_of_igrQueue            = 3,
    .min_num_of_igrQueue            = 1,
    .max_num_of_svl                 = 4096,
    .max_num_of_cvlan_tpid          = 4,
    .max_num_of_svlan_tpid          = 4,
    .max_num_of_evlan_tpid          = 1,
    .max_num_of_route_host_addr     = 2048,
    .max_num_of_route_switch_addr   = 16,
    .tpid_entry_idx_max             = 3,
    .tpid_entry_mask_max            = 0xf,
    .protocol_vlan_idx_max          = 7,
    .vlan_fid_max                   = 255,
    /* flow control */
    .flowctrl_thresh_max            = 5840,
    .flowctrl_pauseOn_page_packet_max       = 5840,
    .flowctrl_pauseOn_page_packet_len_max   = 0xFFFF,
    .flowctrl_egr_queue_drop_group_idx_max  = 2,
    /* QoS */
    .dp_of_selection_max            = 3,
    .dp_of_selection_min            = 0,
    .pri_of_selection_max           = 12,
    .pri_of_selection_min           = 0,
    .pri_sel_group_index_max        = 3,
    .queue_weight_max               = 127,
    .queue_weight_min               = 1,
    .igr_queue_weight_max           = 31,
    .max_num_of_fastPath_of_rate    = 3,
    /* bandwidth */
    .rate_of_egr_bandwidth_min      = 0x1,
    .rate_of_bandwidth_max          = 0xFFFFF,
    .rate_of_bandwidth_max_fe_port  = 0x186A,
    .rate_of_bandwidth_max_ge_port  = 0xF424,
    .rate_of_bandwidth_max_10ge_port= 0x98968,
    .busrt_of_igr_bandwidth_max     = 0xFFFF,
    .busrt_of_bandwidth_max         = 0xFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max   = 3,
    .thresh_of_igr_bw_flowctrl_min  = 22,
    .thresh_of_igr_bw_flowctrl_max  = 0xFFFF,
    /* Storm control */
    .rate_of_storm_control_max      = 0xFFFFFF,
    .rate_of_storm_proto_control_max= 0x1FF,
    .burst_rate_of_storm_control_min= 1700,
    .burst_rate_of_storm_control_max= 0xFFFF,
    .burst_rate_of_10ge_storm_control_min = 2650,
    .burst_rate_of_10ge_storm_control_max = 0xFFFFF,
    .burst_rate_of_storm_proto_control_max= 0xFF,
    .internal_priority_max          = 7,
    .drop_precedence_max            = 2,
    .priority_remap_group_idx_max   = 3,
    .priority_remark_group_idx_max  = 0,
    .wred_weight_max                = 0x3FF,
    .wred_mpd_max                   = 0xF,
    .wred_drop_probability_max      = 0xFF,
    .max_num_of_l2_hash_algo        = 2,
    .l2_learn_limit_cnt_max         = 0x8080,
    .l2_learn_limit_cnt_wo_cam_max  = 0x8000,
    .l2_learn_limit_cnt_disable     = 0xFFFF,
    .l2_fid_learn_limit_entry_max   = 32,
    .l2_ntfy_bp_thr_max             = 2048,
    .max_num_of_vrf                 = 256,
    .max_num_of_intf                = 1024,
    .max_num_of_intf_mtu            = 16,
    .max_num_of_intf_mtu_value      = (16384 - 1),  /* 0 ~ 16383 */
    .max_num_of_router_mac          = 1024,
    .max_num_of_l3_ecmp             = 256,
    .max_num_of_l3_ecmp_hash_idx    = 16,
    .max_num_of_l3_ecmp_nexthop     = 8,
    .max_num_of_l3_nexthop          = 8192,
    .max_num_of_l3_host             = 12288,
    .max_num_of_l3_route            = 12288,
    .max_num_of_l3_conflict_host    = 16,
    .max_num_of_mcast_group_nexthop = 1024 + 2048 + 2, //1k intf + 2k BBSID + 1 bridge nexthop info + 1 l3 stacking forwad portmask
    .max_num_of_tunnel              = 384,
    .max_num_of_tunnel_qos_profile  = 64,
    .max_num_of_mcast_entry         = 4096,
    .eee_queue_thresh_max           = 0xFF,
    .sec_minIpv6FragLen_max         = 0xFFFF,
    .sec_maxPingLen_max             = 0xFFFF,
    .sec_smurfNetmaskLen_max        = 32,
    .max_num_of_ip_mac_bind_entry   = (1024 + 2048),
    .sflow_rate_max                 = 0xFFFF,
    .max_num_of_c2sc_entry          = 2048,
    .max_num_of_c2sc_blk_entry      = 128,
    .max_num_of_c2sc_blk            = 16,
    .max_num_of_c2sc_range_check_vid = 32,
    .max_num_of_c2sc_range_check_set = 4,
    .max_num_of_sc2c_entry          = 1024,
    .max_num_of_sc2c_range_check_vid = 32,
    .max_num_of_sc2c_range_check_set = 4,
    .max_num_of_vlan_prof           = 16,
    .max_num_of_vlan_group          = 16,
    .max_frame_len                  = 12288,
    .max_num_of_mcast_entry         = 4096,
    .max_num_of_vlan_port_iso_entry = 32,
    .max_num_of_mpls_encap          = 2048,
    .max_num_of_mpls_decap          = 2048,
    .max_num_of_mpls_decap_cam      = 64,
    .max_num_of_mpls_nextHop        = 8192,
    .max_num_of_mpls_hashdepth      = 8,
    .max_num_of_led_entity          = 4,
    .miim_page_id_max               = 8191,
    .max_num_of_dying_gasp_pkt_cnt  = 7,
    .dying_gasp_sustain_time_max    = 0xFFFF,
    .max_num_of_rma_user_defined    = 4,
    .time_freq_max                   = 0xFFFFFFF,
    .max_num_of_ethdm_rx_timestamp  = 64,
    /* OpenFlow */
    .byte_cntr_th_max               = 0x3FFFFFFFFFFULL,
    .pkt_cntr_th_max                = 0xFFFFFFFFFULL,
    .max_num_of_igr_flowtbl         = 4,
    .max_num_of_virtual_igr_flowtbl = 256,
    .loopback_time_max              = 63,
    .max_num_of_group_entry         = 2048,
    .max_num_of_group_entry_bucket  = 128,
    .max_num_of_action_bucket       = 8192,
    .max_num_of_dmac_entry          = 1024,
    .max_num_of_bpe_pvid_hashdepth  = 8,
    /* L2 tunnel */
    .max_num_of_l2tnl_lst           = 8192,
    .max_num_of_l2tnl_node          = 2048,
    /* stacking */
    .max_num_of_stack_port          = 16,
    .max_num_of_stack_dev       = 16,
#endif
};

/* Normal 9310 Chip PER_PORT block information */
static rt_macPpInfo_t rtl9310_macPpInfo[] =
{
    {
      /* lowerbound_addr */ 0x6000,
      /* upperbound_addr */ 0x7C7C,
      /* interval */          0x80,
    },
};

#endif  /* end of defined(CONFIG_SDK_RTL9310) */

#if defined(CONFIG_SDK_RTL9300)


static rt_portinfo_t rtl9300_port_info_24G_4XG =
{
    {
        RT_GE_PORT   /*P0 */,          RT_GE_PORT   /*P1 */,        RT_GE_PORT  /*P2 */,        RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */,          RT_GE_PORT   /*P6 */,        RT_GE_PORT  /*P7 */,        RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/,          RT_GE_PORT   /*P11*/,        RT_GE_PORT  /*P12*/,        RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/,          RT_GE_PORT   /*P16*/,        RT_GE_PORT  /*P17*/,        RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/,          RT_GE_PORT   /*P21*/,        RT_GE_PORT  /*P22*/,        RT_GE_PORT  /*P23*/, RT_10GE_SERDES_PORT /*P24*/,
        RT_10GE_SERDES_PORT /*P25*/,   RT_10GE_SERDES_PORT /*P26*/, RT_10GE_SERDES_PORT/*P27*/, RT_CPU_PORT /*P28*/, RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,          RT_PORT_NONE /*P31*/,        RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/, RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,          RT_PORT_NONE /*P36*/,        RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/, RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,          RT_PORT_NONE /*P41*/,        RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/, RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,          RT_PORT_NONE /*P46*/,        RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/, RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,          RT_PORT_NONE /*P51*/,        RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/, RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,          RT_PORT_NONE /*P56*/,        RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/, RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,          RT_PORT_NONE /*P61*/,        RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_8XG =
{
    {
        RT_10GE_SERDES_PORT /*P0 */, RT_PORT_NONE /*P1 */,         RT_PORT_NONE/*P2 */,        RT_PORT_NONE/*P3 */,         RT_PORT_NONE /*P4 */,
        RT_PORT_NONE /*P5 */,        RT_PORT_NONE /*P6 */,         RT_PORT_NONE/*P7 */,        RT_10GE_SERDES_PORT/*P8 */,  RT_PORT_NONE /*P9 */,
        RT_PORT_NONE /*P10*/,        RT_PORT_NONE /*P11*/,         RT_PORT_NONE/*P12*/,        RT_PORT_NONE/*P13*/,         RT_PORT_NONE /*P14*/,
        RT_PORT_NONE /*P15*/,        RT_10GE_SERDES_PORT /*P16*/,  RT_PORT_NONE/*P17*/,        RT_PORT_NONE/*P18*/,         RT_PORT_NONE /*P19*/,
        RT_10GE_SERDES_PORT /*P20*/, RT_PORT_NONE /*P21*/,         RT_PORT_NONE/*P22*/,        RT_PORT_NONE/*P23*/,         RT_10GE_SERDES_PORT /*P24*/,
        RT_10GE_SERDES_PORT /*P25*/, RT_10GE_SERDES_PORT /*P26*/,  RT_10GE_SERDES_PORT/*P27*/, RT_CPU_PORT /*P28*/,         RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_24G_2_5GE =
{
    {
        RT_GE_PORT   /*P0 */,          RT_GE_PORT   /*P1 */,        RT_GE_PORT  /*P2 */,        RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */,          RT_GE_PORT   /*P6 */,        RT_GE_PORT  /*P7 */,        RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/,          RT_GE_PORT   /*P11*/,        RT_GE_PORT  /*P12*/,        RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/,          RT_GE_PORT   /*P16*/,        RT_GE_PORT  /*P17*/,        RT_GE_PORT  /*P18*/, RT_GE_PORT   /*P19*/,
        RT_GE_PORT   /*P20*/,          RT_GE_PORT   /*P21*/,        RT_GE_PORT  /*P22*/,        RT_GE_PORT  /*P23*/, RT_2_5GE_FIBER_PORT /*P24*/,
        RT_2_5GE_FIBER_PORT /*P25*/, RT_2_5GE_FIBER_PORT /*P26*/,  RT_2_5GE_FIBER_PORT/*P27*/, RT_CPU_PORT /*P28*/,         RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_8G_2_5GE_4XG =
{
    {
        RT_GE_PORT   /*P0 */,          RT_GE_PORT   /*P1 */,        RT_GE_PORT  /*P2 */,        RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */,          RT_GE_PORT   /*P6 */,        RT_GE_PORT  /*P7 */,        RT_2_5GE_SERDES_PORT  /*P8 */, RT_2_5GE_SERDES_PORT   /*P9 */,
        RT_2_5GE_SERDES_PORT   /*P10*/,  RT_2_5GE_SERDES_PORT   /*P11*/,        RT_PORT_NONE  /*P12*/,                  RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE   /*P15*/,          RT_2_5GE_SERDES_PORT   /*P16*/,        RT_2_5GE_SERDES_PORT  /*P17*/,          RT_2_5GE_SERDES_PORT  /*P18*/, RT_2_5GE_SERDES_PORT   /*P19*/,
        RT_2_5GE_SERDES_PORT   /*P20*/,  RT_2_5GE_SERDES_PORT   /*P21*/,        RT_2_5GE_SERDES_PORT  /*P22*/,          RT_2_5GE_SERDES_PORT  /*P23*/, RT_10GE_SERDES_PORT /*P24*/,
        RT_10GE_SERDES_PORT /*P25*/,   RT_10GE_SERDES_PORT /*P26*/, RT_10GE_SERDES_PORT/*P27*/, RT_CPU_PORT /*P28*/,    RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_16G_2_5GE_4XG =
{
    {
        RT_GE_PORT   /*P0 */,          RT_GE_PORT   /*P1 */,        RT_GE_PORT  /*P2 */,        RT_GE_PORT  /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT   /*P5 */,          RT_GE_PORT   /*P6 */,        RT_GE_PORT  /*P7 */,        RT_GE_PORT  /*P8 */, RT_GE_PORT   /*P9 */,
        RT_GE_PORT   /*P10*/,          RT_GE_PORT   /*P11*/,        RT_GE_PORT  /*P12*/,        RT_GE_PORT  /*P13*/, RT_GE_PORT   /*P14*/,
        RT_GE_PORT   /*P15*/,          RT_2_5GE_SERDES_PORT   /*P16*/,        RT_2_5GE_SERDES_PORT  /*P17*/,          RT_2_5GE_SERDES_PORT  /*P18*/, RT_2_5GE_SERDES_PORT   /*P19*/,
        RT_2_5GE_SERDES_PORT   /*P20*/,  RT_2_5GE_SERDES_PORT   /*P21*/,        RT_2_5GE_SERDES_PORT  /*P22*/,          RT_2_5GE_SERDES_PORT  /*P23*/, RT_10GE_SERDES_PORT /*P24*/,
        RT_10GE_SERDES_PORT /*P25*/,   RT_10GE_SERDES_PORT /*P26*/, RT_10GE_SERDES_PORT/*P27*/, RT_CPU_PORT /*P28*/,    RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_2_5GE_4XG =
{
    {
        RT_2_5GE_SERDES_PORT   /*P0 */, RT_2_5GE_SERDES_PORT   /*P1 */, RT_2_5GE_SERDES_PORT  /*P2 */,  RT_2_5GE_SERDES_PORT  /*P3 */, RT_PORT_NONE   /*P4 */,
        RT_PORT_NONE   /*P5 */, RT_PORT_NONE   /*P6 */, RT_PORT_NONE  /*P7 */,  RT_2_5GE_SERDES_PORT  /*P8 */, RT_2_5GE_SERDES_PORT   /*P9 */,
        RT_2_5GE_SERDES_PORT   /*P10*/, RT_2_5GE_SERDES_PORT   /*P11*/, RT_PORT_NONE  /*P12*/,  RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE   /*P15*/, RT_2_5GE_SERDES_PORT   /*P16*/, RT_2_5GE_SERDES_PORT  /*P17*/,  RT_2_5GE_SERDES_PORT  /*P18*/, RT_2_5GE_SERDES_PORT   /*P19*/,
        RT_2_5GE_SERDES_PORT   /*P20*/, RT_2_5GE_SERDES_PORT   /*P21*/, RT_2_5GE_SERDES_PORT  /*P22*/,  RT_2_5GE_SERDES_PORT  /*P23*/, RT_10GE_SERDES_PORT /*P24*/,
        RT_10GE_SERDES_PORT /*P25*/,    RT_10GE_SERDES_PORT /*P26*/, RT_10GE_SERDES_PORT/*P27*/, RT_CPU_PORT /*P28*/,    RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_2_5GE_2XG =
{
    {
        RT_2_5GE_SERDES_PORT   /*P0 */, RT_2_5GE_SERDES_PORT   /*P1 */, RT_2_5GE_SERDES_PORT  /*P2 */,  RT_2_5GE_SERDES_PORT  /*P3 */, RT_2_5GE_SERDES_PORT   /*P4 */,
        RT_2_5GE_SERDES_PORT   /*P5 */, RT_2_5GE_SERDES_PORT   /*P6 */, RT_2_5GE_SERDES_PORT  /*P7 */,  RT_2_5GE_SERDES_PORT  /*P8 */, RT_2_5GE_SERDES_PORT   /*P9 */,
        RT_2_5GE_SERDES_PORT   /*P10*/, RT_2_5GE_SERDES_PORT   /*P11*/, RT_2_5GE_SERDES_PORT  /*P12*/,  RT_2_5GE_SERDES_PORT  /*P13*/, RT_2_5GE_SERDES_PORT   /*P14*/,
        RT_2_5GE_SERDES_PORT   /*P15*/, RT_2_5GE_SERDES_PORT   /*P16*/, RT_2_5GE_SERDES_PORT  /*P17*/,  RT_2_5GE_SERDES_PORT  /*P18*/, RT_2_5GE_SERDES_PORT   /*P19*/,
        RT_2_5GE_SERDES_PORT   /*P20*/, RT_2_5GE_SERDES_PORT   /*P21*/, RT_2_5GE_SERDES_PORT  /*P22*/,  RT_2_5GE_SERDES_PORT  /*P23*/, RT_PORT_NONE /*P24*/,
        RT_PORT_NONE /*P25*/,           RT_10GE_SERDES_PORT /*P26*/, RT_10GE_SERDES_PORT/*P27*/, RT_CPU_PORT /*P28*/,    RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};

static rt_portinfo_t rtl9300_port_info_2_5GE =
{
    {
        RT_PORT_NONE /*P0 */,        RT_PORT_NONE /*P1 */,         RT_PORT_NONE/*P2 */,        RT_PORT_NONE/*P3 */,         RT_PORT_NONE /*P4 */,
        RT_PORT_NONE /*P5 */,        RT_PORT_NONE /*P6 */,         RT_PORT_NONE/*P7 */,        RT_2_5GE_SERDES_PORT/*P8 */, RT_2_5GE_SERDES_PORT /*P9 */,
        RT_2_5GE_SERDES_PORT /*P10*/,RT_2_5GE_SERDES_PORT /*P11*/, RT_2_5GE_SERDES_PORT/*P12*/,RT_2_5GE_SERDES_PORT/*P13*/, RT_2_5GE_SERDES_PORT /*P14*/,
        RT_2_5GE_SERDES_PORT /*P15*/,RT_PORT_NONE /*P16*/,         RT_PORT_NONE/*P17*/,        RT_PORT_NONE/*P18*/,         RT_PORT_NONE /*P19*/,
        RT_PORT_NONE /*P20*/,        RT_PORT_NONE /*P21*/,         RT_PORT_NONE/*P22*/,        RT_PORT_NONE/*P23*/,         RT_PORT_NONE /*P24*/,
        RT_PORT_NONE /*P25*/,        RT_PORT_NONE /*P26*/,         RT_PORT_NONE/*P27*/,        RT_CPU_PORT /*P28*/,         RT_PORT_NONE /*P29*/,
        RT_PORT_NONE /*P30*/,        RT_PORT_NONE /*P31*/,         RT_PORT_NONE/*P32*/,        RT_PORT_NONE/*P33*/,         RT_PORT_NONE /*P34*/,
        RT_PORT_NONE /*P35*/,        RT_PORT_NONE /*P36*/,         RT_PORT_NONE/*P37*/,        RT_PORT_NONE/*P38*/,         RT_PORT_NONE /*P39*/,
        RT_PORT_NONE /*P40*/,        RT_PORT_NONE /*P41*/,         RT_PORT_NONE/*P42*/,        RT_PORT_NONE/*P43*/,         RT_PORT_NONE /*P44*/,
        RT_PORT_NONE /*P45*/,        RT_PORT_NONE /*P46*/,         RT_PORT_NONE/*P47*/,        RT_PORT_NONE/*P48*/,         RT_PORT_NONE /*P49*/,
        RT_PORT_NONE /*P50*/,        RT_PORT_NONE /*P51*/,         RT_PORT_NONE/*P52*/,        RT_PORT_NONE/*P53*/,         RT_PORT_NONE /*P54*/,
        RT_PORT_NONE /*P55*/,        RT_PORT_NONE /*P56*/,         RT_PORT_NONE/*P57*/,        RT_PORT_NONE/*P58*/,         RT_PORT_NONE /*P59*/,
        RT_PORT_NONE /*P60*/,        RT_PORT_NONE /*P61*/,         RT_PORT_NONE/*P62*/,        RT_PORT_NONE/*P63*/
     },
};


/* Normal 9300 Chip Port Information */
static rt_register_capacity_t rtl9300_capacityInfo =
{
#if defined(CONFIG_SDK_FPGA_PLATFORM)
    .max_num_of_mirror              = 4,
    .max_num_of_trunk               = 64,
    .max_num_of_trunk_stacking_mode = 64,
    .max_num_of_local_trunk       = 29,
    .max_num_of_stack_trunk      = 2,
    .max_num_of_trunkMember         = 8,
    .max_num_of_trunk_algo          = 2,
    .trunk_algo_shift_max           = 5,
    .max_num_of_dumb_trunkMember    = 8,
    .max_num_of_trunkHashVal        = 64,
    .max_num_of_msti                = 64,
    .max_num_of_metering            = 256,
    .max_num_of_meter_block         = 16,
    .rate_of_meter_max              = 0xFFFFF,
    .max_num_of_pie_block           = 8,
    .max_num_of_pie_block_templateSelector  = 2,
    .max_num_of_pie_blockSize       = 128,
    .max_num_of_pie_counter         = 2048,
    .max_num_of_pie_template        = 10,
    .pie_user_template_id_min       = 5,
    .pie_user_template_id_max       = 9,
    .max_num_of_field_selector      = 12,
    .max_offset_of_field_selector   = 90,
    .max_num_of_pie_block_group     = 16,   //[FIXED ME]
    .max_num_of_pie_block_logic     = 16,    //[FIXED ME]
    .max_num_of_pie_template_field  = 12,   //[FIXED ME]
    .max_num_of_range_check_ip      = 8,
    .max_num_of_range_check         = 16,
    .max_num_of_metaData            = 256,
    .max_num_of_l2_hashdepth        = 4,
    .max_num_of_l2_hashwidth        = 0,
    .max_num_of_queue               = 8,
    .min_num_of_queue               = 1,
    .max_num_of_cpuQueue            = 32,
    .max_num_of_stackQueue     = 12,
    .max_num_of_igrQueue = 0,
    .min_num_of_igrQueue = 0,
    .max_num_of_svl                 = 1,
    .max_num_of_cvlan_tpid          = 4,
    .max_num_of_svlan_tpid          = 4,
    .max_num_of_evlan_tpid          = 1,
    .max_num_of_route_host_addr     = 2048,
    .max_num_of_route_switch_addr   = 16,
    .tpid_entry_idx_max             = 3,
    .tpid_entry_mask_max            = 0xf,
    .protocol_vlan_idx_max          = 7,
    .vlan_fid_max                   = 255,
    .flowctrl_thresh_max            = 0xFFF,
    .flowctrl_pauseOn_page_packet_max       = 0xFFF,
    .flowctrl_pauseOn_page_packet_len_max   = 0xFFFF,
    .flowctrl_egr_queue_drop_group_idx_max  = 3,
    .flowctrl_e2eRemote_port_thresh_group_idx_max = 1,
    .dp_of_selection_max            = 3,
    .dp_of_selection_min            = 0,
    .pri_of_selection_max           = 7,
    .pri_of_selection_min           = 0,
    .pri_sel_group_index_max        = 3,
    .queue_weight_max               = 127,
    .queue_weight_min               = 1,
    .rate_of_bandwidth_max          = 0xFFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max   = 3,
    .thresh_of_igr_bw_flowctrl_min  = 678,
    .thresh_of_igr_bw_flowctrl_max  = 0x7FFFFFFF,
    .max_num_of_fastPath_of_rate    = 0,
    .rate_of_storm_control_max      = 0xFFFFFF,
    .burst_of_storm_control_max= 0xFFFF,
    .rate_of_storm_proto_control_max= 0x1FF,
    .burst_rate_of_storm_control_min= 1700,
    .burst_rate_of_storm_control_max= 0xFFFF,
    .burst_rate_of_10ge_storm_control_min= 2650,
    .burst_rate_of_10ge_storm_control_max    = 0xFFFFF,
    .burst_rate_of_storm_proto_control_max= 0xFF,
    .burst_size_of_acl_meter_max    = 0x1FFFF,
    .internal_priority_max          = 7,
    .drop_precedence_max            = 2,
    .wred_weight_max                = 0x3FF,
    .wred_mpd_max                   = 0xF,
    .wred_drop_probability_max      = 0xFF,
    .max_num_of_l2_hash_algo        = 2,
    .l2_learn_limit_cnt_max         = 0x4040,
    .l2_learn_limit_cnt_wo_cam_max  = 0x4000,
    .l2_learn_limit_cnt_disable     = 0x7FFF,
    .l2_fid_learn_limit_entry_max   = 8,
    .l2_ntfy_bp_thr_max             = 1024,
    .max_num_of_vrf                 = 0,
    .max_num_of_intf                = 128,   //egress interface
    .max_num_of_intf_mtu            = 8,
    .max_num_of_intf_mtu_value      = (16384 - 1),  /* 0 ~ 16383 */
    .max_num_of_router_mac          = 64,
    .max_num_of_l3_ecmp             = 0,
    .max_num_of_l3_ecmp_hash_idx    = 0,
    .max_num_of_l3_ecmp_nexthop     = 0,
    .max_num_of_l3_nexthop          = 2048,
    .max_num_of_l3_host             = 6144,
    .max_num_of_l3_route            = 16,
    .max_num_of_l3_conflict_host    = 16,
    .max_num_of_mcast_group_nexthop = 128 + 2, //one is bridge nexthop info + one l3 stacking forwad portmask
    .max_num_of_tunnel              = 0,
    .max_num_of_tunnel_qos_profile  = 0,
    .eee_queue_thresh_max           = 0xFF,
    .sec_minIpv6FragLen_max         = 0xFFFF,
    .sec_maxPingLen_max             = 0xFFFF,
    .sec_smurfNetmaskLen_max        = 32,
    .max_num_of_ip_mac_bind_entry   = 0,
    .sflow_rate_max                 = 0xFFFFF,
    .rate_of_bandwidth_max_fe_port  = 0x186A,
    .rate_of_bandwidth_max_ge_port  = 0xF424,
    .rate_of_bandwidth_max_10ge_port= 0x98968,
    .busrt_of_igr_bandwidth_max     = 0x7FFFFFFF,
    .busrt_of_bandwidth_max     = 0xFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max = 3,
    .max_num_of_c2sc_entry          = 1024,
    .max_num_of_c2sc_blk_entry      = 128,
    .max_num_of_c2sc_blk            = 8,
    .max_num_of_c2sc_range_check_vid = 32,
    .max_num_of_c2sc_range_check_set = 2,
    .max_num_of_sc2c_entry          = 512,
    .max_num_of_sc2c_range_check_vid = 32,
    .max_num_of_sc2c_range_check_set = 2,
    .max_num_of_vlan_prof           = 8,
    .max_num_of_vlan_group          = 8,
    .max_frame_len                  = 12288,
    .max_num_of_mcast_entry         = 1024,
    .max_num_of_vlan_port_iso_entry = 16,
    //.max_num_of_mpls_lib            = 16,
    .max_num_of_led_entity          = 4,
    .miim_page_id_max               = 0xFFF,
    .max_num_of_dying_gasp_pkt_cnt  = 7,
    .dying_gasp_sustain_time_max    = 0xFFFF,
    .max_num_of_rma_user_defined    = 4,

    /* stacking */
    .max_num_of_stack_port          = 4,
    .max_num_of_dev                 = 16,
#else
    .max_num_of_mirror              = 4,
    .max_num_of_trunk               = 64,
    .max_num_of_trunk_stacking_mode = 64,
    .max_num_of_local_trunk       = 29,
    .max_num_of_stack_trunk      = 2,
    .max_num_of_trunkMember         = 8,
    .max_num_of_trunk_algo          = 2,
    .trunk_algo_shift_max           = 5,
    .max_num_of_dumb_trunkMember    = 8,
    .max_num_of_trunkHashVal        = 64,
    .max_num_of_msti                = 64,
    .max_num_of_metering            = 256,
    .max_num_of_meter_block         = 16,
    .rate_of_meter_max              = 0xFFFFF,
    .max_num_of_pie_block           = 16,
    .max_num_of_pie_block_templateSelector  = 2,
    .max_num_of_pie_blockSize       = 128,
    .max_num_of_pie_counter         = 2048,
    .max_num_of_pie_template        = 10,
    .pie_user_template_id_min       = 5,
    .pie_user_template_id_max       = 9,
    .max_num_of_field_selector      = 12,
    .max_offset_of_field_selector   = 90,
    .max_num_of_pie_block_group     = 16,
    .max_num_of_pie_block_logic     = 16,
    .max_num_of_pie_template_field  = 12,
    .max_num_of_range_check_ip      = 8,
    .max_num_of_range_check         = 16,
    .max_num_of_metaData            = 256,
    .max_num_of_l2_hashdepth        = 4,
    .max_num_of_l2_hashwidth        = 0,
    .max_num_of_mcast_fwd           = 1024,
    .max_num_of_queue               = 8,
    .min_num_of_queue               = 1,
    .max_num_of_cpuQueue            = 32,
    .max_num_of_stackQueue     = 12,
    .max_num_of_igrQueue = 0,
    .min_num_of_igrQueue = 0,
    .max_num_of_svl                 = 1,
    .max_num_of_cvlan_tpid          = 4,
    .max_num_of_svlan_tpid          = 4,
    .max_num_of_evlan_tpid          = 1,
    .max_num_of_route_host_addr     = 2048,
    .max_num_of_route_switch_addr   = 64,
    .tpid_entry_idx_max             = 3,
    .tpid_entry_mask_max            = 0xf,
    .protocol_vlan_idx_max          = 7,
    .vlan_fid_max                   = 4095,
    .flowctrl_thresh_max            = 0xFFF,
    .flowctrl_pauseOn_page_packet_max       = 0xFFF,
    .flowctrl_pauseOn_page_packet_len_max   = 0xFFFF,
    .flowctrl_egr_queue_drop_group_idx_max  = 3,
    .flowctrl_e2eRemote_port_thresh_group_idx_max = 1,
    .dp_of_selection_max            = 3,
    .dp_of_selection_min            = 0,
    .pri_of_selection_max           = 8,
    .pri_of_selection_min           = 0,
    .pri_sel_group_index_max        = 3,
    .queue_weight_max               = 127,
    .queue_weight_min               = 1,
    .rate_of_bandwidth_max          = 0xFFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max   = 3,
    .thresh_of_igr_bw_flowctrl_min  = 0,    /*this value is not applied to 9300*/
    .thresh_of_igr_bw_flowctrl_max  = 0x7FFFFFFF,
    .max_num_of_fastPath_of_rate    = 0,
    .rate_of_storm_control_max      = 0xFFFFFF,
    .burst_of_storm_control_max= 0xFFFF,
    .rate_of_storm_proto_control_max= 0x1FF,
    .burst_rate_of_storm_control_min= 5178,
    .burst_rate_of_storm_control_max= 0xFFFF,
    .burst_rate_of_10ge_storm_control_min = 5178,
    .burst_rate_of_10ge_storm_control_max = 0xFFFF,
    .burst_rate_of_storm_proto_control_max= 0xFF,
    .burst_size_of_acl_meter_min    = 3021,
    .burst_size_of_acl_meter_max    = 0x1FFFF,
    .internal_priority_max          = 7,
    .drop_precedence_max            = 2,
    .wred_weight_max                = 0,
    .wred_mpd_max                   = 0,
    .wred_drop_probability_max      = 0xFF,
    .max_num_of_l2_hash_algo        = 2,
    .l2_learn_limit_cnt_max         = 0x4040,
    .l2_learn_limit_cnt_wo_cam_max  = 0x4000,
    .l2_learn_limit_cnt_disable     = 0x7FFF,
    .l2_fid_learn_limit_entry_max   = 8,
    .l2_ntfy_bp_thr_max             = 1024,
    .max_num_of_vrf                 = 0,
    .max_num_of_intf                = 128,
    .max_num_of_intf_mtu            = 8,
    .max_num_of_intf_mtu_value      = (16384 - 1),  /* 0 ~ 16383 */
    .max_num_of_router_mac          = 64,
    .max_num_of_l3_ecmp             = 0,
    .max_num_of_l3_ecmp_hash_idx    = 0,
    .max_num_of_l3_ecmp_nexthop     = 0,
    .max_num_of_l3_nexthop          = 2048,
    .max_num_of_l3_host             = 6144,
    .max_num_of_l3_route            = 512,
    .max_num_of_l3_conflict_host    = 16,
    .max_num_of_mcast_group_nexthop = (128 + 2), //one is bridge nexthop info + one l3 stacking forwad portmask
    .max_num_of_tunnel              = 0,
    .max_num_of_tunnel_qos_profile  = 0,
    .eee_queue_thresh_max           = 0xFFF,
    .sec_minIpv6FragLen_max         = 0xFFFF,
    .sec_maxPingLen_max             = 0xFFFF,
    .sec_smurfNetmaskLen_max        = 32,
    .max_num_of_ip_mac_bind_entry   = 0,
    .sflow_rate_max                 = 0xFFFF,
    .rate_of_bandwidth_max_fe_port  = 0x186A,
    .rate_of_bandwidth_max_ge_port  = 0xF424,
    .rate_of_bandwidth_max_10ge_port= 0x98968,
    .busrt_of_igr_bandwidth_max     = 0x7FFFFFFF,
    .busrt_of_bandwidth_max     = 0xFFFF,
    .thresh_of_igr_port_pause_congest_group_idx_max = 3,
    .max_num_of_c2sc_entry          = 1024,
    .max_num_of_c2sc_blk_entry      = 128,
    .max_num_of_c2sc_blk            = 8,
    .max_num_of_c2sc_range_check_vid = 32,
    .max_num_of_c2sc_range_check_set = 2,
    .max_num_of_sc2c_entry          = 512,
    .max_num_of_sc2c_range_check_vid = 32,
    .max_num_of_sc2c_range_check_set = 2,
    .max_num_of_vlan_prof           = 8,
    .max_num_of_vlan_group          = 8,
    .max_frame_len                  = 12288,
    .max_num_of_mcast_entry         = 1024,
    .max_num_of_vlan_port_iso_entry = 16,

    .max_num_of_led_entity          = 4,
    .miim_page_id_max               = 0xFFF,
    .max_num_of_dying_gasp_pkt_cnt  = 7,
    .dying_gasp_sustain_time_max    = 0xFFFF,
    .max_num_of_rma_user_defined    = 4,

    .loopback_time_max              = 7,
     /* stacking */
    .max_num_of_stack_port          = 4,
    .max_num_of_dev                 = 16,
#endif
};

/* Normal 9300 formal Chip PER_PORT block information */
static rt_macPpInfo_t rtl9300_macPpInfo[] =
{
    {
      /* lowerbound_addr */ 0x3260,
      /* upperbound_addr */ 0x3A5C,
      /* interval */          0x40,
    },
    {
      /* lowerbound_addr */ 0x3c60,
      /* upperbound_addr */ 0x67DC,
      /* interval */          0x180,
    },
    {
      /* lowerbound_addr */ 0x67E0,
      /* upperbound_addr */ 0x765C,
      /* interval */          0x60,
    },
    {
      /* lowerbound_addr */ 0x7660,
      /* upperbound_addr */ 0x785C,
      /* interval */          0x10,
    },
    {
      /* lowerbound_addr */ 0xE300,
      /* upperbound_addr */ 0xEAFC,
      /* interval */          0x60,
    },
    {
      /* lowerbound_addr */ 0xF080,
      /* upperbound_addr */ 0xFFFC,
      /* interval */          0x80,
    },
};
#endif  /* end of defined(CONFIG_SDK_RTL9300) */


/* Supported mac chip lists */
static rt_device_t supported_devices[] =
{
#if defined(CONFIG_SDK_RTL8390)
    /* RT_DEVICE_RTL8351M_A */
    {
        RTL8351M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8350_FAMILY_ID,
        &rtl8351_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8352M_A */
    {
        RTL8352M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8350_FAMILY_ID,
        &rtl8352_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8353M_A */
    {
        RTL8353M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8350_FAMILY_ID,
        &rtl8353_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8391M_A */
    {
        RTL8391M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8391_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8392M_A */
    {
        RTL8392M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8392_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8393M_A */
    {
        RTL8393M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8393_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8396M_A */
    {
        RTL8396M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8396_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8352MES_A */
    {
        RTL8352MES_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8350_FAMILY_ID,
        &rtl8352_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8353MES_A */
    {
        RTL8353MES_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8350_FAMILY_ID,
        &rtl8353_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8392MES_A */
    {
        RTL8392MES_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8392_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8393MES_A */
    {
        RTL8393MES_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8393_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },

    /* RT_DEVICE_RTL8396MES_A */
    {
        RTL8396MES_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8390_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8390_FAMILY_ID,
        &rtl8396_port_info,
        &rtl8390m_capacityInfo,
        1,
        rtl8390_macPpInfo,
    },
#endif /* end of defined(CONFIG_SDK_RTL8390) */


    /******************************************/
    /* Now user 8382/8380/8332/8330 chip info */
    /******************************************/
#if defined(CONFIG_SDK_RTL8380)
    /* RT_DEVICE_RTL8382M_A */
    {
        RTL8382M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8380_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8380_FAMILY_ID,
        &rtl8380_port_info_24G_4BX,
        &rtl8380m_capacityInfo,
        2,
        rtl8380_macPpInfo,
    },

    /* RT_DEVICE_RTL8381M_A */
    {
        RTL8381M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8380_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8380_FAMILY_ID,
        &rtl8380_port_info_16G_4BX,  /* Use the 80M Set to avoid varify in future */
        &rtl8380m_capacityInfo,
        2,
        rtl8380_macPpInfo,
    },

    /* RT_DEVICE_RTL8380M_A */
    {
        RTL8380M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8380_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8380_FAMILY_ID,
        &rtl8380_port_info_16G_4BX,
        &rtl8380m_capacityInfo,
        2,
        rtl8380_macPpInfo,
    },

    /* RT_DEVICE_RTL8332M_A */
    {
        RTL8332M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8380_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8330_FAMILY_ID,
        &rtl8380_port_info_24FE_4G,
        &rtl8380m_capacityInfo,
        2,
        rtl8380_macPpInfo,
    },

    /* RT_DEVICE_RTL8330M_A */
    {
        RTL8330M_CHIP_ID,
        CHIP_REV_ID_A,
        RTL8380_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL8330_FAMILY_ID,
        &rtl8380_port_info_16FE_4G,
        &rtl8380m_capacityInfo,
        2,
        rtl8380_macPpInfo,
    },
#endif


#if defined(CONFIG_SDK_RTL9310)
    /* RT_DEVICE_RTL9310_A */
    {
        RTL9310_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9310_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9310_FAMILY_ID,
        NULL,
        &rtl9310_capacityInfo,
        1,
        rtl9310_macPpInfo,
    },
    {
        RTL9311_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9310_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9310_FAMILY_ID,
        NULL,
        &rtl9310_capacityInfo,
        1,
        rtl9310_macPpInfo,
    },
    {
        RTL9312_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9310_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9310_FAMILY_ID,
        NULL,
        &rtl9310_capacityInfo,
        1,
        rtl9310_macPpInfo,
    },
    {
        RTL9313_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9310_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9310_FAMILY_ID,
        NULL,
        &rtl9310_capacityInfo,
        1,
        rtl9310_macPpInfo,
    },
#endif /* end of defined(CONFIG_SDK_RTL9310) */

#if defined(CONFIG_SDK_RTL9300)
    {
        RTL9301_CHIP_ID_24G,
        CHIP_REV_ID_B,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_24G_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9303_CHIP_ID_8XG,
        CHIP_REV_ID_B,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_8XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9301H_CHIP_ID_4X2_5G,
        CHIP_REV_ID_C,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_24G_2_5GE,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302A_CHIP_ID_12X2_5G,
        CHIP_REV_ID_C,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_8G_2_5GE_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302B_CHIP_ID_8X2_5G,
        CHIP_REV_ID_C,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_16G_2_5GE_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302C_CHIP_ID_16X2_5G,
        CHIP_REV_ID_C,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_2_5GE_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302D_CHIP_ID_24X2_5G,
        CHIP_REV_ID_C,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_2_5GE_2XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302F_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_2_5GE,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9301_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_24G_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9303_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_8XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9301H_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_24G_2_5GE,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302A_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_8G_2_5GE_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302B_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_16G_2_5GE_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302C_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_2_5GE_4XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },

    {
        RTL9302D_CHIP_ID,
        CHIP_REV_ID_A,
        RTL9300_FAMILY_ID,
        CHIP_REV_ID_A,
        CHIP_AFLAG_LEXRA,
        RTL9300_FAMILY_ID,
        &rtl9300_port_info_2_5GE_2XG,
        &rtl9300_capacityInfo,
        4,
        rtl9300_macPpInfo,
    },
#endif /* end of defined(CONFIG_SDK_RTL9300) */


}; /* end of supported_devices */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      hal_clone_device
 * Description:
 *      Clone rt_device_t
 * Input:
 *      pDev        - MAC device memory structure
 * Output:
 *      None
 * Return:
 *      NULL        - clone fail
 *      Otherwise   - Pointer of mac chip structure that cloned
 * Note:
 *      The function have take care the forward compatible in revision.
 *      Return one recently revision if no extra match revision.
 */
rt_device_t *
hal_clone_device(rt_device_t *pDev)
{
    rt_device_t             *pDev_clone = NULL;
    rt_register_capacity_t  *pCapacityInfo = NULL;
    rt_macPpInfo_t          *pMacPpInfo = NULL;


    pDev_clone      = (rt_device_t *) osal_alloc(sizeof(rt_device_t));
    pCapacityInfo   = (rt_register_capacity_t *) osal_alloc(sizeof(rt_register_capacity_t));
    pMacPpInfo      = (rt_macPpInfo_t *) osal_alloc(sizeof(rt_macPpInfo_t) * pDev->macPpInfo_blockNum);

    if (  (pDev_clone == NULL)
        ||(pCapacityInfo == NULL)
        ||(pMacPpInfo == NULL))
    {
        osal_printf("%s,%d: clone rt_device_t of chip %x fail!", __FUNCTION__, __LINE__, pDev->chip_id);
        goto FAIL_EXIT;
    }

    osal_memcpy(pMacPpInfo, pDev->pMacPpInfo, (sizeof(rt_macPpInfo_t) * pDev->macPpInfo_blockNum));
    osal_memcpy(pCapacityInfo, pDev->pCapacityInfo, sizeof(rt_register_capacity_t));
    osal_memcpy(pDev_clone, pDev, sizeof(rt_device_t));

    pDev_clone->pCapacityInfo = pCapacityInfo;
    pDev_clone->pMacPpInfo = pMacPpInfo;

    return pDev_clone;


  FAIL_EXIT:
    if (pDev_clone != NULL)
        osal_free(pDev_clone);

    if (pCapacityInfo != NULL)
        osal_free(pCapacityInfo);

    if (pMacPpInfo != NULL)
        osal_free(pMacPpInfo);


    return NULL;
}

/* Function Name:
 *      hal_find_device
 * Description:
 *      Find the mac chip from SDK supported mac device lists.
 * Input:
 *      chip_id     - chip id
 *      chip_rev_id - chip revision id
 * Output:
 *      None
 * Return:
 *      NULL        - Not found
 *      Otherwise   - Pointer of mac chip structure that found
 * Note:
 *      The function have take care the forward compatible in revision.
 *      Return one recently revision if no extra match revision.
 */
rt_device_t *
hal_find_device(uint32 chip_id, uint32 chip_rev_id)
{
    uint32  dev_idx;

    RT_PARAM_CHK((chip_rev_id > CHIP_REV_ID_MAX), NULL);

    /* find out appropriate supported revision from supported_devices lists
     */
    for(dev_idx = 0; dev_idx < sizeof(supported_devices)/sizeof(rt_device_t); dev_idx++)
    {
        if(supported_devices[dev_idx].chip_family_id == FAMILY_ID(chip_id))
        {
            /* Match and return this MAC device */
            return (&supported_devices[dev_idx]);
        }
    }

    return (NULL);
} /* end of hal_find_device */


/* Function Name:
 *      hal_get_driver_id
 * Description:
 *      Get its driver of the mac chip based on chip id and chip revision id.
 * Input:
 *      chip_id        - chip id
 *      chip_rev_id    - chip revision id
 * Output:
 *      pDriver_id     - pointer buffer of driver id
 *      pDriver_rev_id - pointer buffer of driver revision id
 * Return:
 *      RT_ERR_OK             - OK
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_FOUND - The chip can not found
 * Note:
 *      None
 */
int32
hal_get_driver_id(
    uint32 chip_id,
    uint32 chip_rev_id,
    uint32 *pDriver_id,
    uint32 *pDriver_rev_id)
{
    rt_device_t *pDev = NULL;

    RT_PARAM_CHK((NULL == pDriver_id), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDriver_rev_id), RT_ERR_NULL_POINTER);

    if ((pDev = hal_find_device(chip_id, chip_rev_id)) == NULL)
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    *pDriver_id = pDev->driver_id;
    *pDriver_rev_id = pDev->driver_rev_id;

    return RT_ERR_OK;
} /* end of hal_get_driver_id */

/* Function Name:
 *      hal_isPpBlock_check
 * Description:
 *      Check the register is PER_PORT block or not?
 * Input:
 *      unit       - unit id
 *      addr       - register address
 * Output:
 *      pIsPpBlock - pointer buffer of chip is PER_PORT block?
 *      pPpBlockIdx - pointer buffer of PER_PORT block index
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
hal_isPpBlock_check(uint32 unit, uint32 addr, uint32 *pIsPpBlock, uint32 *pPpBlockIdx)
{
    uint32  i, ppBlkNum = 0;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsPpBlock), RT_ERR_NULL_POINTER);

    ppBlkNum = HAL_GET_MACPP_BLK_NUM(unit);
    for (i = 0; i < ppBlkNum; i++)
    {
        if (addr >= HAL_GET_MACPP_BLK_MIN_ADDR(unit, i) && addr <= HAL_GET_MACPP_BLK_MAX_ADDR(unit, i))
        {
            *pIsPpBlock = TRUE;
            *pPpBlockIdx = i;
            return RT_ERR_OK;
        }
    }

    if (i == ppBlkNum)
    {
        *pIsPpBlock = FALSE;
    }

    return RT_ERR_OK;
} /* end of hal_isPpBlock_check */



