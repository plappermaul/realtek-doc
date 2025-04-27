/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Module.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Module
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <linux/version.h>

#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <common/rt_error.h>
#include <common/debug/mem.h>
#include <osal/print.h>
#include <hal/mac/mem.h>
#include <ioal/mem32.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_util.h>
#include <drv/watchdog/watchdog.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_9300.h>
#if (defined(CONFIG_SDK_DRIVER_TEST) || defined(CONFIG_SDK_DRIVER_TEST_MODULE))
#include <sdk/sdk_test.h>
#endif
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <rtk/eee.h>
#ifdef CONFIG_SDK_MODEL_MODE
#include <model_comm.h>
#include <tc.h>
#include <virtualmac/vmac_target.h>
#include <osal/time.h>
#endif

#include <osal/memory.h>
#include <osal/time.h>
#include <hal/phy/phy_common.h>
//#include <linux/math64.h>
#include <dal/rtrpc/rtrpc_msg.h>


/*
 * Symbol Definition
 */

#define EXT_PKTGEN_EN               1


#define EXT_PACKET_HDR_LEN          14    //dmac+smac+ethtype
#define PKTGEN_PORT_STREAM_NUM      2
#define PKTGEN_PORT_NUM             28

#define PKT_STREAM_IDX(_idx)      (_idx%PKTGEN_PORT_STREAM_NUM)

#define PKT_PAGE_CELL_NUM         4
#define PKT_CELL_BYTE             90
#define PKT_STREAM_IDX_MAX        55
#define PKT_STREAM_PAGE_BASE      0x800


#define REG_SPG_GLOBAL_INDEX_CTRL0          0xdcfc
#define REG_SPG_PB_ACCESS_CTRL2             0xdcf8
#define REG_SPG_PB_ACCESS_CTRL1             0xdcf4
#define REG_CHK_FAIL_NUM                    0x0230

/* TODO change max port num using HWP */
#define LOOPBACK_TEST_PORT_MAX_NUM         28
#define LOOPBACK_TEST_UNIT_MAX_NUM         2 // array can't use HWP_SW_COUNT

#define TRAP_RSN_RMA_UDF0    20 // follow TRAP_RSN_RMA_UDF0 in model.h

uint32  page_base = PKT_STREAM_PAGE_BASE;

/*
 * Data Declaration
 */
rtdrv_ext_pkt_t *pDiagExtPacket;

uint8 pDiagExtPacketHdr[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM][EXT_PACKET_HDR_LEN];	/* enough to comprise ethernet, IP and TCP header */
uint8 include_vlan_header[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint8 include_svlan_header[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint8 include_ethtype_header[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint8 spg_payload_type[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint32 spg_payload_pattern[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint32 oTag[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint32 iTag[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};
uint32 pktLen[PKTGEN_PORT_NUM][PKTGEN_PORT_STREAM_NUM] = {{0, 0}};

uint32 i2c_cal_brd[] = {1, 1, 2, 2, 3, 3, 4, 4};

uint32 rc_thd_target_short[] = {7403, 7394, 7441, 7425, 7143, 7684, 7563, 7837};
uint32 rc_thd_target_long[] = {10235, 10103, 10420, 10098, 9912, 10653, 10381, 10601};
uint32 *rc_thd_target[] = {rc_thd_target_short, rc_thd_target_long};

int32 ampOfsA[] = {-40, -53, -20, -32, -16, -41, -16, -33};
int32 ampOfsB[] = {-16, -30, -15, 10, -23, -40, -25, -41};
int32 ampOfsC[] = {-28, -42, -18, -11, -20, -40, -20, -37};
int32 ampOfsD[] = {-28, -42, -18, -11, -20, -40, -20, -37};
int32 *ampOf[] = {ampOfsA, ampOfsB, ampOfsC, ampOfsD};

uint32 R_OFS_A[] = {-1, -2, -1, -1, -2, -2, -2, -2};
uint32 R_OFS_B[] = {-2, -2, -2, -2, -1, -1, -1, -1};
uint32 R_OFS_C[] = {-1, -1, -1, -1, -1, -1, -1, -1};
uint32 R_OFS_D[] = {-1, -1, -1, -1, -1, -1, -1, -1};
uint32 *R_OFS[] = {R_OFS_A, R_OFS_B, R_OFS_C, R_OFS_D};

uint32 RC_CAL[2][8];
uint32 NEW_R_CAL[8];
uint32 ADC_OFS_CAL[8];
uint32 AMP_CAL_EXT[8];

static uint8 loopbackTestPktPattern1[] = {
                    0x00, 0xE0, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x55, 0xAA, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                };  //without VLAN tag

static uint32 loopback_test_cpu_pkt_cnt[LOOPBACK_TEST_UNIT_MAX_NUM][LOOPBACK_TEST_PORT_MAX_NUM] = {{0}};
static uint32 loopback_test_nic_max_depth = 30; // this value is get from experiment

/*
 * Macro Declaration
 */
typedef enum rtk_lanSwitchMode_e
{
    SWITCH_MODE_INIT,
    SWITCH_MODE_AMP,
    SWITCH_MODE_DC,
    SWITCH_MODE_SHORT,
    SWITCH_MODE_OPEN,
    SWITCH_MODE_END
} rtk_lanSwitchMode_t;

typedef enum rtk_phyDCMode_e
{
    DC_MODE_NA,
    DC_MODE_POS_1,
    DC_MODE_NEG_1,
    DC_MODE_END
} rtk_phyDCMode_t;

typedef enum rtk_voltMeas_e
{
    VOLT_MEAS_VTX,
    VOLT_MEAS_VA,
    VOLT_MEAS_VB,
    VOLT_MEAS_END
} rtk_voltMeas_t;

/*
 * Function Declaration
 */
static int32
_sdk_tc_reset(uint32 unit)
{
    int32 ret = RT_ERR_OK;
    int32 base;
    uint32 value;
    uint32 idx;
    rtk_mcast_group_t mcast_group;
    rtk_portmask_t emptyPm;
    rtk_trk_egrPort_t trk_egr_ports;
    rtk_mirror_entry_t  mirrorEntry;
    rtk_mirror_rspanTag_t tag;
    rtk_trunk_distAlgoShift_t shift;
    rtk_l2_flushCfg_t   l2FlushCfg;
    rtk_dev_port_t unitPort;
    rtk_port_t port;
    rtk_pie_rangeCheck_ip_t         ipRng;
    rtk_pie_rangeCheck_t            range;
    rtk_acl_templateIdx_t           tmplateInfo;
    rtk_acl_clear_t                 clearInfo;
    rtk_qos_1pRmkVal_t  priVal;
    rtk_qos_outer1pRmkVal_t  oPriVal;
    int32 scan_idx;
    rtk_l2_mcastAddr_t  mcast_data;

    /* reset H/W table, register */
    //osal_printf("Reset table (by using init reg)\n");
    value = 0xffffffff;
    osal_memset(&l2FlushCfg, 0, sizeof(rtk_l2_flushCfg_t));
#if 0
    reg_write(unit, LONGAN_MEM_ALE_INIT_0r, &value);
    reg_write(unit, LONGAN_MEM_ALE_INIT_1r, &value);
    reg_write(unit, LONGAN_MEM_ALE_INIT_2r, &value);
#endif

    //osal_printf("Reset extra HSA memory\n");
    value = 1;
#if 0
    reg_field_write(unit, LONGAN_MEM_EGR_CTRLr, LONGAN_LINK_INITf, &value);
#endif
    //osal_time_mdelay(100);  /* wait for 100mS */

    /* reset QOS */
    for(port = 0; port < 28; port ++)
    {
        rtk_rate_portEgrBwCtrlEnable_set(unit, port, DISABLED);
        for(idx = 0; idx < 8; idx ++)
        {
            rtk_rate_portEgrQueueBwCtrlEnable_set(unit,  port, idx, DISABLED);
            rtk_rate_portEgrQueueAssuredBwCtrlEnable_set(unit,  port, idx, DISABLED);
            rtk_rate_portEgrQueueAssuredBwCtrlMode_set(unit,  port, idx, ASSURED_MODE_SHARE);
        }
        rtk_qos_port1pRemarkEnable_set(unit, port, DISABLED);
        rtk_qos_portOut1pRemarkEnable_set(unit, port, DISABLED);
        rtk_qos_portDeiRemarkEnable_set(unit, port, DISABLED);
        rtk_qos_portDscpRemarkEnable_set(unit, port, DISABLED);
    }

    for(idx = 0; idx < 8; idx ++)
    {
        priVal.pri.val = idx;
        oPriVal.pri.val = idx;
        rtk_qos_pri2QidMap_set(unit, idx, 0);
        rtk_qos_1pRemarking_set(unit, PRI_SRC_INNER_USER_PRI, priVal, 0);
        rtk_qos_1pRemarking_set(unit, PRI_SRC_OUTER_USER_PRI, priVal, 0);
        rtk_qos_outer1pRemarking_set(unit, PRI_SRC_INNER_USER_PRI, oPriVal, 0);
        rtk_qos_outer1pRemarking_set(unit, PRI_SRC_OUTER_USER_PRI, oPriVal, 0);
    }

    /* reset L2 */

    /* reset IPMC */
    //osal_printf("Reset IPMC module\n");
    ret = rtk_ipmc_addr_delAll(unit, RTK_IPMC_FLAG_NONE);   /* IPv4 */
    if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);
    ret = rtk_ipmc_addr_delAll(unit, RTK_IPMC_FLAG_IPV6);   /* IPv6 */
    if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);

    /* reset MCAST */
    //osal_printf("Reset MCAST module\n");
    base = -1;
    ret = rtk_mcast_group_getNext(unit, RTK_MCAST_TYPE_IP, &base, &mcast_group);
    if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);
    while (base != -1)
    {
        ret = rtk_mcast_egrIf_delAll(unit, mcast_group);
        if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);
        ret = rtk_mcast_group_destroy(unit, mcast_group);
        if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);

        /* get next */
        ret = rtk_mcast_group_getNext(unit, RTK_MCAST_TYPE_IP, &base, &mcast_group);
        if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);
    }

    /* reset L3 */
    //osal_printf("Reset L3 module\n");
    #if 0
    ret = rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_NONE, 0);
    if (RT_ERR_OK != ret) osal_printf("%s():%d - ret = 0x%x\n", __FUNCTION__, __LINE__, ret);
    #endif

    /* reset BPE (802.1BR) */

    /* reset L2 */
    //osal_printf("Reset L2 module\n");
    l2FlushCfg.act = FLUSH_ACT_FLUSH_ALL_UC;
    ret = rtk_l2_ucastAddr_flush(unit, &l2FlushCfg);
    if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);
    /* remove L2 MC entry */
  #if 0
    value = 0x80000070;
    reg_write(unit, LONGAN_MEM_ALE_INIT_0r, &value);
    value = 0x00000060;
    reg_write(unit, LONGAN_MEM_ALE_INIT_2r, &value);
  #endif
    /* reset learning counter */
    value = 0;
    reg_field_write(unit, LONGAN_L2_LRN_CONSTRT_CNTr, LONGAN_LRN_CNTf, &value);
    HWP_ETHER_PORT_TRAVS(unit, port)
        reg_array_field_write(unit, LONGAN_L2_LRN_PORT_CONSTRT_CNTr, port, REG_ARRAY_INDEX_NONE, LONGAN_LRN_CNTf, &value);
    for (idx = 0; idx < HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit); idx++)
        reg_array_field_write(unit, LONGAN_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, idx, LONGAN_LRN_CNTf, &value);

    scan_idx = -1; /* get the first entry */

    while (1)
    {
        if ((ret = rtk_l2_nextValidMcastAddr_get(unit, &scan_idx, &mcast_data)) == RT_ERR_OK)
        {
            rtk_l2_mcastAddr_del(unit, mcast_data.rvid, &mcast_data.mac);
        }
        else
            break;
    }

    /* reset STP */

    /* reset VLAN */
    //osal_printf("Reset VLAN module\n");
    value = 0x0;
    RT_ERR_CHK(reg_write(unit, LONGAN_VLAN_CTRLr, &value), ret);
    RT_ERR_CHK(reg_write(unit, LONGAN_VLAN_APP_PKT_CTRLr, &value), ret);
    ret = rtk_vlan_destroyAll(unit, 1);
    for(port = 0; port < 28; port ++)
    {
        /* initial default value first */
        value = 0x8006001;
        //RT_ERR_CHK(reg_array_write(unit, LONGAN_VLAN_PORT_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, &value), ret);
        value = 0xC;
        RT_ERR_CHK(reg_array_write(unit, LONGAN_VLAN_PORT_FWD_CTRLr, port, REG_ARRAY_INDEX_NONE, &value), ret);
        value = 0x1;
        RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PORT_IGR_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_IGR_FLTR_ACTf, &value), ret);
        value = 0x1;
        RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PORT_EGR_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_EGR_FLTR_ENf, &value), ret);
        value = 0x3C9;
        //RT_ERR_CHK(reg_array_write(unit, LONGAN_VLAN_PORT_TAG_CTRLr, port, REG_ARRAY_INDEX_NONE, &value), ret);

        ret = rtk_vlan_portIgrVlanCnvtEnable_set(unit, port, DISABLED);
        if (ret == RT_ERR_PORT_ID)
            continue;

        rtk_vlan_portEgrVlanCnvtEnable_set(unit, port, DISABLED);
        rtk_vlan_portIgrVlanTransparentEnable_set(unit, port, INNER_VLAN, DISABLED);
        rtk_vlan_portIgrVlanTransparentEnable_set(unit, port, OUTER_VLAN, DISABLED);

        rtk_vlan_portPvid_set(unit, port, INNER_VLAN, 1);
        rtk_vlan_portPvid_set(unit, port, OUTER_VLAN, 1);

        rtk_vlan_portIgrTpid_set(unit, port, INNER_VLAN, 0x1);
        rtk_vlan_portIgrTpid_set(unit, port, OUTER_VLAN, 0x0);
        rtk_vlan_portEgrTpid_set(unit, port, INNER_VLAN, 0x0);
        rtk_vlan_portEgrTpid_set(unit, port, OUTER_VLAN, 0x0);

        /* PVID mode */
        rtk_vlan_portPvidMode_set(unit, port, INNER_VLAN, PBVLAN_MODE_UNTAG_AND_PRITAG);
        rtk_vlan_portPvidMode_set(unit, port, OUTER_VLAN, PBVLAN_MODE_UNTAG_AND_PRITAG);

        /* Accept-frame-type */
        rtk_vlan_portAcceptFrameType_set(unit, port, INNER_VLAN, ACCEPT_FRAME_TYPE_ALL);
        rtk_vlan_portAcceptFrameType_set(unit, port, OUTER_VLAN, ACCEPT_FRAME_TYPE_ALL);

        rtk_vlan_portIgrFilter_set(unit, port, ACTION_DROP);
        rtk_vlan_portEgrFilterEnable_set(unit, port, ENABLED);
    }

  #if 0
    /* VLAN Profile */
    {
        uint32 idx;
        uint32 data;

        for (idx=0; idx<16; idx++)
        {
            data = 0x0;
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_L2_LRN_ENf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_L2_NEW_SA_ACTf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IP4_MC_BDG_MODEf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IP6_MC_BDG_MODEf, &data), ret);

            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_L2MC_BRIDGE_LU_MIS_ACTf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IPMC_BRIDGE_LU_MIS_ACTf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IP6MC_BRIDGE_LU_MIS_ACTf, &data), ret);

            data = 0x0FFFFFF;
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_L2MC_UNKN_FLD_PMSK_52_32f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IPMC_UNKN_FLD_PMSK_52_32f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IP6MC_UNKN_FLD_PMSK_52_32f, &data), ret);

            data = 0xFFFFFFFF;
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_L2MC_UNKN_FLD_PMSK_31_0f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IPMC_UNKN_FLD_PMSK_31_0f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, LONGAN_IP6MC_UNKN_FLD_PMSK_31_0f, &data), ret);

        }
    }
  #endif
    /* TPID */
    {
        uint32 tpid_idx;

        for (tpid_idx=0; tpid_idx < 4; tpid_idx++)
        {
            RT_ERR_CHK(rtk_vlan_tpidEntry_set(unit, VLAN_TAG_TYPE_OUTER, tpid_idx, 0x88A8), ret);
            RT_ERR_CHK(rtk_vlan_tpidEntry_set(unit, VLAN_TAG_TYPE_INNER, tpid_idx, 0x8100), ret);
        }
        RT_ERR_CHK(rtk_vlan_tpidEntry_set(unit, VLAN_TAG_TYPE_EXTRA, 0, 0x8100), ret);
    }

    /* IVC */
    {
        uint32 ivcEntry[7];
        uint32 idx;
        uint32 value = 0x1;

        osal_memset(&ivcEntry, 0x00, sizeof(ivcEntry));

        for (idx=0; idx<2048; idx++)
        {
            //RT_ERR_CHK(table_write(unit, LONGAN_LAGt, idx, &ivcEntry[0]), ret);

            /* clear hit-bit */
            reg_array_field_write(unit, LONGAN_VLAN_IVC_ENTRY_INDICATIONr, REG_ARRAY_INDEX_NONE, idx, LONGAN_HITf, &value);
        }
    }

    /* EVC */
    {
        uint32 evcEntry[6];
        uint32 idx;
        uint32 value = 0x1;

        osal_memset(&evcEntry, 0x00, sizeof(evcEntry));

        for (idx=0; idx<1024; idx++)
        {
            //RT_ERR_CHK(table_write(unit, LONGAN_LAGt, idx, &evcEntry[0]), ret);

            /* clear hit-bit */
            reg_array_field_write(unit, LONGAN_VLAN_EVC_ENTRY_INDICATIONr, REG_ARRAY_INDEX_NONE, idx, LONGAN_HITf, &value);
        }
    }

    /* reset LAG (trunk) */
    //osal_printf("Reset Trunk module\n");
    osal_memset(&emptyPm, 0, sizeof(emptyPm));
    osal_memset(&trk_egr_ports, 0, sizeof(trk_egr_ports));

    for(idx = 0; idx < 128; idx ++)
    {
        rtk_trunk_localPort_set (unit, idx, &emptyPm);
        rtk_trunk_egrPort_set(unit, idx, &trk_egr_ports);
        rtk_trunk_distributionAlgorithmTypeBind_set(unit, idx, BIND_TYPE_L2, 0);
        rtk_trunk_distributionAlgorithmTypeBind_set(unit, idx, BIND_TYPE_IPV4, 0);
        rtk_trunk_distributionAlgorithmTypeBind_set(unit, idx, BIND_TYPE_IPV6, 0);
        rtk_trunk_trafficSeparateEnable_set(unit, idx, SEPARATE_KNOWN_MULTI, DISABLED);
        rtk_trunk_trafficSeparateEnable_set(unit, idx, SEPARATE_FLOOD, DISABLED);
    }

    rtk_trunk_mode_set(unit, TRUNK_MODE_STANDALONE);

    for(idx = 0; idx < 8; idx ++)
    {
        rtk_trunk_stkTrkPort_set(unit, idx, &emptyPm);
        rtk_trunk_stkDistributionAlgorithmTypeBind_set(unit, idx, BIND_TYPE_L2, 0);
        rtk_trunk_stkDistributionAlgorithmTypeBind_set(unit, idx, BIND_TYPE_IPV4, 0);
        rtk_trunk_stkDistributionAlgorithmTypeBind_set(unit, idx, BIND_TYPE_IPV6, 0);
    }
    rtk_trunk_stkTrkHash_set(unit, STACK_TRK_HASH_RECALCULATE);

    rtk_trunk_localFirst_set(unit, DISABLED);

    osal_memset(&shift, 0, sizeof(shift));
    rtk_trunk_distributionAlgorithmShiftGbl_set(unit, &shift);

    for (port = 0; port <= RTK_MAX_NUM_OF_PORTS; ++port)
    {
        unitPort.devID = 0;
        unitPort.port = port;
        rtk_trunk_srcPortMap_set(unit, unitPort, 0, 0);
    }

    /* reset Mirror */
    //osal_printf("Reset Mirror module\n");
    osal_memset(&mirrorEntry, 0, sizeof(mirrorEntry));
    osal_memset(&tag, 0, sizeof(tag));

    for(idx = 0; idx < 4; idx ++)
    {
        rtk_mirror_group_set(unit, idx, &mirrorEntry);
        rtk_mirror_rspanEgrMode_set(unit, idx, RSPAN_EGR_NO_MODIFY);
        rtk_mirror_rspanTag_set(unit, idx, &tag);
        rtk_mirror_sflowMirrorSampleRate_set(unit, idx, 0);
    }
    rtk_mirror_egrQueue_set(unit, DISABLED, 0);
    rtk_trap_rmaCancelMirror_set(unit, DISABLED);

    /*reset stacking */
    //osal_printf("Reset Stacking module\n");
    rtk_stack_port_set (unit, &emptyPm);
    rtk_stack_devId_set(unit, 0);
    rtk_stack_masterDevId_set(unit, 0);
    rtk_stack_loopGuard_set(unit, ENABLED);
    for(idx = 0; idx < 16; idx ++)
    {
        rtk_stack_devPortMap_set (unit, idx, &emptyPm);
        rtk_stack_nonUcastBlockPort_set (unit, idx, &emptyPm);
    }

    /* reset ACL */
    //osal_printf("Reset ACL module\n");
    for (idx = 0; idx < HAL_MAX_NUM_OF_PIE_BLOCK(unit); ++idx)
    {
        /* block lookup enable */
        rtk_pie_blockLookupEnable_set(unit, idx, ENABLED);

        /* block default phase is ingress */
        rtk_pie_phase_set(unit, idx, PIE_PHASE_VACL);
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, idx)
    {
        rtk_acl_portPhaseLookupEnable_set(unit, idx, ACL_PHASE_VACL, ENABLED);
    }

    /* Range Check */
    osal_memset(&range, 0, sizeof(rtk_pie_rangeCheck_t));
    for (idx = 0; idx < HAL_MAX_NUM_OF_RANGE_CHECK(unit); ++idx)
        rtk_pie_rangeCheck_set(unit, idx, &range);

    /* IPv4 or IPv6 SIP/DIP */
    osal_memset(&ipRng, 0, sizeof(rtk_pie_rangeCheck_ip_t));
    for (idx = 0; idx < HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit); ++idx)
        rtk_pie_rangeCheckIp_set(unit, idx, &ipRng);

    osal_memset(&tmplateInfo, 0, sizeof(rtk_acl_templateIdx_t));
    for (idx = 0; idx < HAL_MAX_NUM_OF_PIE_BLOCK(unit); ++idx)
    {
        rtk_acl_templateSelector_set(unit, idx, tmplateInfo);
    }

    clearInfo.start_idx = 0;
    clearInfo.end_idx = (HAL_MAX_NUM_OF_PIE_BLOCK(unit) *
            HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)) - 1;
    rtk_acl_rule_del(unit, ACL_PHASE_VACL, &clearInfo);

    /*EthernetAV*/
    for (port = 0; port <= RTK_MAX_NUM_OF_PORTS; ++port)
    {
        rtk_qos_portAvbStreamReservationClassEnable_set(unit, port,
            AVB_SR_CLASS_A, DISABLED);
        rtk_qos_portAvbStreamReservationClassEnable_set(unit, port,
            AVB_SR_CLASS_B, DISABLED);
    }


    osal_printf("SDK reset ok!\n");

    return ret;
}


uint32
_lan_switch_select(uint32 unit, rtk_lanSwitchMode_t mode)
{
    gpioID gpioId_LP = 20, gpioId_SEL = 21 ;
    uint32 ret;


    ret = drv_gpio_pin_init(unit, gpioId_LP, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);
    ret = drv_gpio_pin_init(unit, gpioId_SEL, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);


    switch(mode)
    {
        case SWITCH_MODE_AMP:
            drv_gpio_dataBit_set(unit, gpioId_LP, 0);
            drv_gpio_dataBit_set(unit, gpioId_SEL, 0);
            break;
        case SWITCH_MODE_DC:
            drv_gpio_dataBit_set(unit, gpioId_LP, 0);
            drv_gpio_dataBit_set(unit, gpioId_SEL, 1);
            break;
        case SWITCH_MODE_SHORT:
            drv_gpio_dataBit_set(unit, gpioId_LP, 1);
            drv_gpio_dataBit_set(unit, gpioId_SEL, 0);
            break;
        case SWITCH_MODE_OPEN:
            drv_gpio_dataBit_set(unit, gpioId_LP, 1);
            drv_gpio_dataBit_set(unit, gpioId_SEL, 1);
            break;
        default:

            break;
    }
    osal_time_mdelay(100);

    return RT_ERR_OK;
}

int32
phy_field_chk(uint32 unit, uint32 port, uint32 page, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 chkVal)
{
    uint32 data = 0;
    uint32 cnt = 0;

    data = 0;
    cnt = 0;
    do {
        phy_field_read(unit, port, page, reg, endBit, startBit, &data);
        cnt ++;
        if(cnt == 1000)
            break;
    }while(data != chkVal);

    if(data != chkVal)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}   /* end of phy_field_read */

uint32
_int_ado_mean(uint32 unit, uint32 port, uint32 channel, uint32 *pMean)
{
    uint32 data[] = {0x80BF, 0x80c1, 0x80c3, 0x80c5};

    phy_field_write(unit, port, 0xa47, 17, 7, 7, 0x1);
    osal_time_mdelay(10);
    if(phy_field_chk(unit, port, 0xa47, 17, 7, 7, 0) != RT_ERR_OK)
    {
        osal_printf("- adcbias_cal_en fail.");
        return RT_ERR_FAILED;
    }

    phy_field_write(unit, port, 0xa43, 27, 15, 0, data[channel]);
    phy_field_read(unit, port, 0xa43, 28, 8, 0, pMean);

    return RT_ERR_OK;
}

uint32
_round(uint32 val)
{
    if((val % 10) >= 5)
        return val - (val % 10) + 10;
    else
        return val - (val % 10);
}

uint32
_phy_en(uint32 unit, uint32 port)
{
    uint32 data;
    osal_printf("%s %d port %d\n", __FUNCTION__, __LINE__, port);

    phy_field_write(unit, port, 0, 30, 31, 0, 0x1);
    phy_field_write(unit, port, 0xa40, 0, 11, 11, 0);
    phy_field_write(unit, port, 0xa40, 0, 9, 9, 1);

    phy_field_read(0, 0, 0xa40, 0, 11, 11, &data);
                osal_printf("--- %s %d data %d ---\n", __FUNCTION__, __LINE__, data);

    osal_printf(" Port %d reset \n", port);
    return RT_ERR_OK;
}

uint32
_phy_check_on(uint32 unit, uint32 port)
{
    uint32 status;

    phy_field_write(unit, port, 0, 30, 15, 0, 0x1);
    phy_field_read(unit, port, 0xa46, 21, 10, 8, &status);

    if(status != 3)
    {
        osal_printf("Port %d Status %d != 3\n", port, status);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

uint32
_adc_ofs_cal(uint32 unit, rtk_portmask_t portmask)
{
    uint32 channel;
    uint32 phyId;
    uint32 chnlOfsetBitEnd[] = {3,7,11,15};
    uint32 chnlOfsetBitStart[] = {0,4,8,12};
    uint32 step_ado = 10;
    uint32 spec_ado = step_ado/2;
    uint32 loop = 1;
    uint32 mean;
    int32 meanInt, meanDev;
    uint32 offset, offsetCal;
    int32 step_ioffset_cnt, offsetD;
    rtk_port_t port;

    uint32 chnlOfsetA[] = {8, 8, 9, 8, 8, 8, 8, 8};
    uint32 chnlOfsetB[] = {8, 8, 8, 8, 6, 6, 6, 6};
    uint32 chnlOfsetC[] = {8, 8, 8, 8, 8, 8, 8, 8};
    uint32 chnlOfsetD[] = {8, 8, 8, 8, 6, 6, 6, 6};
    uint32 *chnlOfset[] = {chnlOfsetA, chnlOfsetB, chnlOfsetC, chnlOfsetD};

    osal_printf("=== %s ===\n", __FUNCTION__);

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        phyId = port % 8;

        _phy_check_on(unit, port);

        phy_field_write(unit, port, 0xa43, 27, 15, 0, 0x8010);
        phy_field_write(unit, port, 0xa43, 28, 11, 11, 0x0);
        phy_field_write(unit, port, 0xb82, 16, 4, 4, 0x1);

        if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 1) != RT_ERR_OK)
        {
            osal_printf("%s %d FAIL patch_rdy fail.", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        phy_field_write(unit, port, 0xa40, 0, 12, 12, 0x0);
        phy_field_write(unit, port, 0xa4A, 19, 7, 6, 0x2);
        phy_field_write(unit, port, 0xa44, 20, 2, 2, 0x1);
        phy_field_write(unit, port, 0xa81, 16, 9, 6, 0xF);
        phy_field_write(unit, port, 0xa81, 16, 4, 0, 0x0);
        phy_field_write(unit, port, 0xb82, 16, 4, 4, 0x0);

        if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 0) != RT_ERR_OK)
        {
            osal_printf("%s %d FAIL patch_rdy fail.\n", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        if(phy_field_chk(unit, port, 0xa60, 16, 7, 0, 0x41) != RT_ERR_OK)
        {
            osal_printf("%s %d FAIL pcs_state fail.\n", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        for(channel = 0; channel < 4; channel ++)
        {
            osal_printf("Channel %c\n", channel + 'A');
            loop = 1;


            phy_field_write(unit, port, 0xbcf, 22, chnlOfsetBitEnd[channel], chnlOfsetBitStart[channel], chnlOfset[channel][phyId]);
            while (1)
            {
                phy_field_read(unit, port, 0xbcf, 22, chnlOfsetBitEnd[channel], chnlOfsetBitStart[channel], &offset);
                osal_printf("   - adc_ioffset: %d\n", offset);
                _int_ado_mean(unit, port, channel, &mean);
                meanInt = mean;
                if(meanInt > 0x100)
                    meanInt -= 0x200;
                if(meanInt < 0)
                    meanDev = - meanInt;
                else
                    meanDev = meanInt;
                osal_printf("     - ado_mean: %d\n", meanInt);
                //check adc_ioffset in specification or not
                if(meanDev <= spec_ado || loop == 5)
                {
                    offsetCal = offset;
                    osal_printf("   - adc_ioffset_cal: 0x%x\n", offsetCal);
                    break;
                }
                //Adjust adc_ioffset, here is 10 times
                step_ioffset_cnt = meanDev * 10 / step_ado;
                if(meanDev % step_ado > spec_ado)
                    step_ioffset_cnt += 10;

                if(meanInt > 0)
                    step_ioffset_cnt *= -1;


                offsetD = offset + step_ioffset_cnt / 10;
                if(offsetD < 0x2)
                {
                    osal_printf("=== PHY_%d, CH_%d, ADC_OFS_CAL result = %d is less than 0x2 !!!  ===\n",
                        port, channel, offsetD);
                    return RT_ERR_FAILED;
                }
                else if(offsetD > 0xD)
                {
                    osal_printf("=== PHY_%d, CH_%d, ADC_OFS_CAL result = %d is greater than 0xD !!!  ===\n",
                        port, channel, offsetD);
                    return RT_ERR_FAILED;
                }
                else
                {
                    offset = offsetD;
                    phy_field_write(unit, port, 0xbcf, 22, chnlOfsetBitEnd[channel], chnlOfsetBitStart[channel], offset);
                }
                loop ++;
            }

            phy_field_write(unit, port, 0xbcf, 22, chnlOfsetBitEnd[channel], chnlOfsetBitStart[channel], offsetCal);

        }

        //Enable giga force mode
        phy_field_write(unit, port, 0xa43, 27, 15, 0, 0x8010);
        phy_field_write(unit, port, 0xa43, 28, 11, 11, 0x1);

        //Set patch_req = 1
        phy_field_write(unit, port, 0xb82, 16, 4, 4, 0x1);

        //Poll patch_rdy = 10x80100x
        if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 1) != RT_ERR_OK)
        {
            osal_printf("%s %d FAIL patch_rdy fail.", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        //Enable NWay
        phy_field_write(unit, port, 0xa40, 0, 12, 12, 0x1);

        //Release force slave
        phy_field_write(unit, port, 0xa44, 19, 7, 6, 0x0);

        //Release force TP1
        phy_field_write(unit, port, 0xa44, 20, 2, 2, 0x0);

        //Set aagc_frc_r = 0x0
        phy_field_write(unit, port, 0xa81, 16, 9, 6, 0x0);

        //Set patch_req = 0
        phy_field_write(unit, port, 0xb82, 16, 4, 4, 0x0);

        //Poll patch_rdy = 0
        if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 0) != RT_ERR_OK)
        {
            osal_printf("%s %d FAIL patch_rdy fail.", __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        //Check adc_ioffset
        phy_field_read(unit, port, 0xbcf, 22, 15, 0, &offset);
        osal_printf("PHY %d Offset 0x%x\n", port, offset);

        ADC_OFS_CAL[port % 8] = offset;
    }

    return RT_ERR_OK;
}

uint32 _rc_thd(uint32 unit, uint32 port, uint32 isWrite, uint32 *pVal)
{
    osal_printf("- Execute \'rccal_nyq_thd_h,l\' access procedure.\n");

    phy_field_write(unit, port, 0xa43, 27, 15, 0, 0x8116);

    if(isWrite == 0)
    {
        phy_field_read(unit, port, 0xa43, 28, 15, 0, pVal);
        osal_printf("   - Read rccal_nyq_thd_h,l = %d\n", *pVal);
    }
    else if(isWrite == 1)
    {
        phy_field_write(unit, port, 0xa43, 28, 15, 0, *pVal);
        osal_printf("   - Write rccal_nyq_thd_h,l = %d\n", *pVal);
    }
    else
    {
        osal_printf("===Parameter Error===\n");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

uint32
_rc_cal(uint32 unit, uint32 shortCali, uint32 longCali, rtk_portmask_t portmask)
{
    uint32 len;
    uint32 rlen;
    uint32 phyId;
    rtk_port_t port;
    uint32 caliIdx;

    osal_printf("=== %s ===\n", __FUNCTION__);

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    for(caliIdx = 0; caliIdx < 2; caliIdx ++)
    {

        if(((caliIdx+1) & (longCali << 1 | shortCali)) == 0)
            continue;

        for(port = 0; port < 64; port ++)
        {
            if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
                continue;
            phyId = port % 8;

            if(_phy_check_on(unit, port) != RT_ERR_OK)
                return RT_ERR_FAILED;

            //Update rccal_nyq_thd
            _rc_thd(unit, port, 1, &rc_thd_target[caliIdx][phyId]);

            //Enable RC calibration function
            phy_field_write(unit, port, 0xa47, 17, 9, 9, 0x1);
            osal_time_mdelay(100);
            if(phy_field_chk(unit, port, 0xa47, 17, 9, 9, 0) != RT_ERR_OK)
            {
                osal_printf("FAIL RC_CAL_EN fail.");
                return RT_ERR_FAILED;
            }

            //Check RC calibration done
            phy_field_write(unit, port, 0xa43, 27, 15, 0, 0x8020);
            if(phy_field_chk(unit, port, 0xa43, 28, 8, 8, 1) != RT_ERR_OK)
            {
                osal_printf("FAIL RC_CAL_EN fail.");
                return RT_ERR_FAILED;
            }


            //Check RC calibration result
            phy_field_read(unit, port, 0xbcd, 22, 15, 0, &len);
            phy_field_read(unit, port, 0xbcd, 23, 15, 0, &rlen);

            if (len < 0x1111)
            {
                osal_printf("FAILED!  PHY_%d, RC_CAL result = %d is less than 0x1111 !!!\n", port, len);
            }
            else if (rlen < 0x1111)
            {
                osal_printf("FAILED!   PHY_%d, RC_CAL result = %d is less than 0x1111 !!!\n", port, rlen);
            }
            else if (len > 0xEEEE)
            {
                osal_printf("FAILED!   PHY_%d, RC_CAL result = %d is larger than 0xEEEE !!!n", port, len);
            }
            else if (rlen > 0xEEEE)
            {
                osal_printf("FAILED!   PHY_%d, RC_CAL result = %d is larger than 0xEEEE !!!\n", port, rlen);
            }
            else
            {
                osal_printf("-  PHY_%d, len=0x%x, rlen=0x%x\n", port, len, rlen);
            }

            RC_CAL[caliIdx][port%8] = len;
        }
    }

    return RT_ERR_OK;
}

uint32 _chk_lock_main_uc(uint32 unit, uint32 port)
{
    if(phy_field_chk(unit, port, 0xa60, 16, 7, 0, 1) != RT_ERR_OK)
    {
        osal_printf("FAIL Lock PCS NCTL to main for UC fail.");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

uint32
_phy_dc_mode(uint32 unit, uint32 port, rtk_phyDCMode_t mode)
{
    uint32 val;

    //Set nctl lock main
    phy_field_write(unit, port, 0xa46, 21, 1, 1, 0x1);

    //check nctl in main
    if(_chk_lock_main_uc(unit, port) != RT_ERR_OK)
        return RT_ERR_FAILED;

    //set DAC test mode
    if(mode == DC_MODE_NA)
        phy_field_write(unit, port, 0xa58, 16, 12, 12, 0x0); //disable DAC test mode
    else
        phy_field_write(unit, port, 0xa58, 16, 12, 10, 0x4); //enable DAC test mode as DC

    //release nctl lock main
    phy_field_write(unit, port, 0xa46, 21, 1, 1, 0x0);

    //set DAC output voltage = 0.125V * (rg_dactst_val - 8)
    switch(mode)
    {
        case DC_MODE_POS_1:
            phy_field_write(unit, port, 0xa58, 16, 4, 0, 0x10);
            break;
        case DC_MODE_NEG_1:
            phy_field_write(unit, port, 0xa58, 16, 4, 0, 0x0);
            break;
        default:
            break;
    }

    phy_field_read(unit, port, 0xa58, 16, 15, 0, &val);

    osal_time_mdelay(10);

    return RT_ERR_OK;
}

uint32
_adc1015_get(uint32 unit, uint32 port, uint32 i2c_channel, uint32 phyChannel, uint32 pole, uint32 *pMVolt)
{
    uint32 scale = 4096;
    uint32 phy_id_r = port %2;
    uint32 i2c_addr = 0x48 + (phyChannel / 2) + (phy_id_r * 2);
    uint32 adc_get;
    uint16 data, adc_cfg;
    i2c_devConf_t i2c_dev;
    uint32 volt, voltFrag;
    uint32 ret;

    osal_memset(&i2c_dev, 0, sizeof(i2c_dev));

    switch(phyChannel)
    {
        case 0:
        case 2:
            if(pole == 0) //P
                adc_cfg = 0xC383;
            else //N
                adc_cfg = 0xD383;
            break;
        case 1:
        case 3:
            if(pole == 0) //P
                adc_cfg = 0xF383;
            else //N
                adc_cfg = 0xE383;
            break;
        default:
            break;
    }



    i2c_dev.device_id = I2C_DEV_ID0;                  /* I2C driver device structure ID */
                                                                /* The I2C driver device structure is a software structure*/
                                                                /* For RTL839x/8x, this is mapped to SMI */

    i2c_dev.i2c_interface_id = 1 - (port/4);           /* Chip I2C master interface ID, port 0,1,2,3 use ID 0, port 4,5,6,7 use ID 1 */
    i2c_dev.dev_addr = i2c_addr;                   /* device address of the slave device*/
    i2c_dev.clk_freq = I2C_CLK_STD_MODE;                   /* Serial clock frequency */
    i2c_dev.scl_delay = 0;                  /* For RTL839x/8x software simulation of I2C clock delay time */
    i2c_dev.mem_addr_width = 1;             /* slave memory address/Regitser index field width */
    i2c_dev.data_width = 2;                 /* slave data field width */
    i2c_dev.read_type = 0;                  /* Select Read Type : 0(Random Read) / 1(Sequential Read) */
    i2c_dev.scl_pin_id = 8 + 9 * (port/4);                 /* SCL pin ID, port 0,1,2,3 use pin 8, port 4,5,6,7 use pin 17 */
    i2c_dev.sda_pin_id = port /2;                 /* SDA pin ID */

    ret = drv_i2c_dev_init(unit, &i2c_dev);
    if(ret != RT_ERR_OK)
    {
        osal_printf("FAIL %x drv_i2c_dev_init\n", ret);
    }

    //configure ADC
    ret = drv_i2c_write(unit, i2c_dev.device_id, 1, (uint8 *)&adc_cfg);

    if(ret != RT_ERR_OK)
    {
        osal_printf("FAIL %x drv_i2c_write\n", ret);
    }
    osal_time_mdelay(10);

    //get voltage
    ret = drv_i2c_read(unit, i2c_dev.device_id, 0, (uint8 *)&data);


    if(ret != RT_ERR_OK)
    {
        osal_printf("FAIL %x drv_i2c_read\n", ret);
    }

    adc_get = data;

    //high byte and low byte inverse
    //adc_get = ((adc_get & 0xFF) << 8) + ((adc_get & 0xFF00) >> 8);

    //D[3:0] no use, shift >> 4
    adc_get = adc_get >> 4;

    //return ADC get voltage value
    volt = ((adc_get * scale) / 1000) / 2048;
    voltFrag = (((adc_get * scale) / 2048) % 1000);

    *pMVolt = volt * 1000 + voltFrag;
    osal_printf("%s REG32(0xbb000374)= %x adc_get: 0x%x pMVolt: %d\n", __FUNCTION__, REG32(0xbb000374), adc_get, *pMVolt);

    return RT_ERR_OK;
}

uint32
_amp_cal_ext(uint32 unit, rtk_portmask_t portmask)
{
    uint32 channel;
    uint32 amp_base = 2030;
    uint32 phyId;
    uint32 bit_ibadj_end[] = {3,7,11,15};
    uint32 bit_ibadj_start[] = {0,4,8,12};
    rtk_port_t  port;

    int32 mAmpTarget;
    uint32 step_ibadj_dco = 20;
    uint32 spec_ibadj_dco = step_ibadj_dco / 2;
    int32 step_ibadj_cnt;
    uint32 loop = 1, break_index = 0;
    int32 ibadj, ibadj_cal;
    uint32 mvoltP, mvoltN;
    int32 mvoltPos, mvoltNeg, mvoltDiff;
    int32 volt_dc_target, volt_dc_target_dev;

    osal_printf("=== %s ===\n", __FUNCTION__);

    drv_gpio_pin_init(unit, 8, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);
    drv_gpio_pin_init(unit, 9, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);
    drv_gpio_pin_init(unit, 10, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);
    drv_gpio_pin_init(unit, 11, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);
    drv_gpio_pin_init(unit, 12, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        phyId = port % 8;

        if(_phy_check_on(unit, port) != RT_ERR_OK)
            return RT_ERR_FAILED;

        for (channel = 0; channel < 4; channel ++)
        {
            osal_printf("Channel %c\n", channel + 'A');
            loop = 1;
            break_index = 0;

            mAmpTarget = amp_base + ampOf[channel][phyId];
            osal_printf("   mAmpTarget %d\n", mAmpTarget);

            while(1)
            {
                phy_field_read(unit, port, 0xbca, 22, bit_ibadj_end[channel], bit_ibadj_start[channel], &ibadj);
                osal_printf("   ibadj 0x%x\n", ibadj);

                _phy_dc_mode(unit, port, DC_MODE_POS_1);
                _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 0, &mvoltP);
                _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 1, &mvoltN);
                mvoltPos = mvoltP - mvoltN;

                _phy_dc_mode(unit, port, DC_MODE_NEG_1);
                _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 0, &mvoltP);
                _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 1, &mvoltN);

                mvoltNeg = mvoltP - mvoltN;
                mvoltDiff = (mvoltPos - mvoltNeg) / 2;

                osal_printf("   mvoltDiff: %d mvoltPos: %d mvoltNeg: %d\n", mvoltDiff, mvoltPos, mvoltNeg);

                //Check ibadj in specification or not
                volt_dc_target = mvoltDiff - mAmpTarget;
                volt_dc_target_dev = (volt_dc_target < 0) ? -volt_dc_target: volt_dc_target;

                if(volt_dc_target_dev <= spec_ibadj_dco || break_index == 1 || loop == 5)
                {
                    ibadj_cal = ibadj;
                    osal_printf("ibadj_cal: %d volt_dc_target_dev: %d break_index: %d loop: %d\n",
                        ibadj_cal, volt_dc_target_dev, break_index, loop);
                    break;
                }

                //Adjust ibadj
                step_ibadj_cnt = (volt_dc_target_dev * 100) / step_ibadj_dco;
                if ((volt_dc_target_dev % step_ibadj_dco) > spec_ibadj_dco)
                    step_ibadj_cnt += 100;

                if (volt_dc_target > 0)
                    step_ibadj_cnt *= -1;

                //Set ibadj
                ibadj += step_ibadj_cnt / 100;

                if(ibadj <= 0)
                {
                    phy_field_write(unit, port, 0xBCA, 22, bit_ibadj_end[channel], bit_ibadj_start[channel], 0x0);
                    break_index = 1;
                }
                else if (ibadj >= 0xF)
                {
                    phy_field_write(unit, port, 0xBCA, 22, bit_ibadj_end[channel], bit_ibadj_start[channel], 0xF);
                    break_index = 1;
                }
                else
                    phy_field_write(unit, port, 0xBCA, 22, bit_ibadj_end[channel], bit_ibadj_start[channel], ibadj);

                loop ++;
            }

            //Write back ibadj_cal
            phy_field_write(unit, port, 0xBCA, 22, bit_ibadj_end[channel], bit_ibadj_start[channel], ibadj_cal);
        }

        //Check ibadj
        phy_field_read(unit, port, 0xBCA, 22, 15, 0, &ibadj_cal);
        osal_printf("%s port %d ibadj 0x%x\n", __FUNCTION__, port, ibadj_cal);

        _phy_dc_mode(unit, port, DC_MODE_NA);

        AMP_CAL_EXT[port] = ibadj_cal;
    }

    return RT_ERR_OK;
}


uint32
_frc_pga_gain(uint32 unit, uint32 port, uint32 gain_cfg)
{
    //set nctl lock main
    phy_field_write(unit, port, 0xA46, 21, 1, 1, 1);

    //check nctl in main
    if(_chk_lock_main_uc(unit, port) != RT_ERR_OK)
        return RT_ERR_FAILED;


    if(gain_cfg == 0)
    {
        //disable DAC test mode
        phy_field_write(unit, port, 0xA58, 16, 15, 0, 0);

        //disable inrx by rg_inrx_enable
        phy_field_write(unit, port, 0xC40, 17, 13, 12, 0);

        //set agc_upd = 0
        phy_field_write(unit, port, 0xBCD, 19, 5, 5, 0);

        //set rg_agc_frc_en = 0
        phy_field_write(unit, port, 0xBCE, 21, 4, 4, 0);
    }
    else
    {
        //set DAC test mode
        phy_field_write(unit, port, 0xA58, 16, 15, 0, 0x1000);
        //enable inrx by rg_inrx_enable
        phy_field_write(unit, port, 0xC40, 17, 13, 12, 3);
        //set agc_upd = 1
        phy_field_write(unit, port, 0xBCD, 19, 5, 5, 1);
        //set rg_agc_frc_en = 1
        phy_field_write(unit, port, 0xBCE, 21, 4, 4, 1);
        //set rg_agc_value
        phy_field_write(unit, port, 0xBCE, 21, 3, 0, gain_cfg);
    }

    //release nctl lock main
    phy_field_write(unit, port, 0xA46, 21, 1, 1, 0);
    return RT_ERR_OK;
}



uint32
_new_r_cal(uint32 unit, rtk_portmask_t portmask)
{
    uint32 volt_meas_pga_gain;
    uint32 channel;
    uint32 bit_tapbin_end[] = {3,7,11,15};
    uint32 bit_tapbin_start[] = {0,4,8,12};
    int32 step_tapbin_ado = 50;//10 times of real value
    int32 spec_tapbin_ado = step_tapbin_ado / 2; //10 times of real value
    uint32 loop;
    uint32 tapbin;
    int32 ado;
    rtk_voltMeas_t volt_meas;
    int32 ado_volt_meas[VOLT_MEAS_END][2];
    uint32 pole;
    int32 vtx, va, vb, volt_calc;
    int32 volt_calc_dev, step_tapbin_cnt;
    int32 tapbin_cal;
    uint32 low_limit = 0x1, high_limit = 0xE;
    uint32 phyId;
    uint64 tmp;
    rtk_port_t port;

    #if 1 //maybe too large for uint32 after calculate
    uint32 gain_factor[] = {
        112518519,
        113038450,
        108401639,
        114285714,
        114841648,
        115818686,
        116144578,
        116425505,
        166444740,
        116662031,
        116360294,
        116075157,
        115537941,
        114850829,
        113988001,
        112946429
        };
    #else
    uint32 gain_factor[] = {
        1125185,
        1130384,
        1084016,
        1142857,
        1148416,
        1158186,
        1161445,
        1164255,
        1664447,
        1166620,
        1163602,
        1160751,
        1155379,
        1148508,
        1139880,
        1129464
        }
    #endif

    osal_printf("=== %s ===\n", __FUNCTION__);

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        phyId = port % 8;

        //Check PHY status
        if(_phy_check_on(unit, port) != RT_ERR_OK)
            return RT_ERR_FAILED;

        //Third step: calibration resistor
        //Force PGA gain, Jim_Chan: target range 200~220
        volt_meas_pga_gain = 1;

        _frc_pga_gain(unit, port, volt_meas_pga_gain);

         for (channel = 0; channel < 4; channel ++)
        {
            osal_printf("Channel %c\n", channel + 'A');

            //Force TX/RX on/off
            phy_field_write(unit, port, 0xBC0, 19, 4, 4, 1); //GETPOWCTRL_L

            switch(channel)
            {
                case 0:
                    phy_field_write(unit, port, 0xBC0, 18, 3, 0, 1); //FORCEGIGATX_L
                    phy_field_write(unit, port, 0xBC0, 18, 7, 4, 1); //FORCEGIGARX_L
                    phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0xE); //EN_KADC_L
                    break;
                case 1:
                    phy_field_write(unit, port, 0xBC0, 18, 3, 0, 2); //FORCEGIGATX_L
                    phy_field_write(unit, port, 0xBC0, 18, 7, 4, 2); //FORCEGIGARX_L
                    phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0xD); //EN_KADC_L
                    break;
                case 2:
                    phy_field_write(unit, port, 0xBC0, 18, 3, 0, 4); //FORCEGIGATX_L
                    phy_field_write(unit, port, 0xBC0, 18, 7, 4, 4); //FORCEGIGARX_L
                    phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0xB); //EN_KADC_L
                    break;
                case 3:
                    phy_field_write(unit, port, 0xBC0, 18, 3, 0, 8); //FORCEGIGATX_L
                    phy_field_write(unit, port, 0xBC0, 18, 7, 4, 8); //FORCEGIGARX_L
                    phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0x7); //EN_KADC_L
                    break;
            }

            //Calibrate tapbin by interpolation search
            loop = 1;

            while(1)
            {
                //Get tapbin
                phy_field_read(unit, port, 0xBCE, 16, bit_tapbin_end[channel], bit_tapbin_start[channel], &tapbin);
                osal_printf("   - tapbin: %x\n", tapbin);

                //Get ADO value
                for(volt_meas = 0; volt_meas < VOLT_MEAS_END; volt_meas ++)
                {
                    switch(volt_meas)
                    {
                        case VOLT_MEAS_VTX:
                            _lan_switch_select(unit, SWITCH_MODE_DC);
                            phy_field_write(unit, port, 0xBC0, 17, 3, 0, 0xF); //EN_HYBRID_TX_L
                            phy_field_write(unit, port, 0xBC0, 17, 7, 4, 0x0); //EN_HYBRID_TXRX_L
                            phy_field_write(unit, port, 0xBC0, 17, 11, 8, 0x0); //EN_HYBRID_ABSHORT_L
                            break;
                        case VOLT_MEAS_VA:
                            _lan_switch_select(unit, SWITCH_MODE_DC);
                            phy_field_write(unit, port, 0xBC0, 17, 3, 0, 0x0); //EN_HYBRID_TX_L
                            phy_field_write(unit, port, 0xBC0, 17, 7, 4, 0xF); //EN_HYBRID_TXRX_L
                            phy_field_write(unit, port, 0xBC0, 17, 11, 8, 0x0); //EN_HYBRID_ABSHORT_L
                            break;
                        case VOLT_MEAS_VB:
                            _lan_switch_select(unit, SWITCH_MODE_SHORT);
                            phy_field_write(unit, port, 0xBC0, 17, 3, 0, 0x0); //EN_HYBRID_TX_L
                            phy_field_write(unit, port, 0xBC0, 17, 7, 4, 0x0); //EN_HYBRID_TXRX_L
                            phy_field_write(unit, port, 0xBC0, 17, 11, 8, 0xF); //EN_HYBRID_ABSHORT_L
                            break;
                        default:
                            break;
                    }

                    for(pole = DC_MODE_POS_1; pole < DC_MODE_END; pole ++)
                    {
                        //Set TX test mode DC output
                        _phy_dc_mode(unit, port, pole);


                        //Get ADO value
                       _int_ado_mean(unit, port, channel, &ado);

                        if(ado < 0x100)
                        {
                            ado_volt_meas[volt_meas][pole - 1] = ado;
                        }
                        else
                        {
                            ado_volt_meas[volt_meas][pole - 1] = ado - 0x200;
                        }
                        osal_printf("ado_volt_meas[%d][%d]:%d\n", volt_meas, pole - 1, ado_volt_meas[volt_meas][pole - 1]);
                    }
                }

                // 2 times than usual
                vtx = (ado_volt_meas[VOLT_MEAS_VTX][DC_MODE_POS_1 - 1] - ado_volt_meas[VOLT_MEAS_VTX][DC_MODE_NEG_1 - 1]);
                if(vtx < 0)
                    vtx *= -1;
                va = (ado_volt_meas[VOLT_MEAS_VA][DC_MODE_POS_1 - 1] - ado_volt_meas[VOLT_MEAS_VA][DC_MODE_NEG_1 - 1]);
                if(va < 0)
                    va *= -1;
                vb = (ado_volt_meas[VOLT_MEAS_VB][DC_MODE_POS_1 - 1] - ado_volt_meas[VOLT_MEAS_VB][DC_MODE_NEG_1 - 1]);
                if(vb < 0)
                    vb *= -1;

                tmp = (uint64)vtx * (uint64)100000000;

                do_div(tmp, gain_factor[1]);

                vtx = tmp;

                volt_calc = (vtx * 2 - va - vb)/2;

                //Check tapbin in specification or not
                if(volt_calc > 0)
                    volt_calc_dev = volt_calc;
                else
                    volt_calc_dev = volt_calc * -1;

                //spec_tapbin_ado is 10 times than usual
                if(volt_calc_dev * 10 <= spec_tapbin_ado || loop == 5)
                {
                    tapbin_cal = tapbin;
                    osal_printf("tapbin_cal : %d\n", tapbin_cal);

                    break;
                }

                //Adjust tapbin
                //10 times larger
                step_tapbin_cnt = (volt_calc_dev * 100) / step_tapbin_ado;
                if((volt_calc_dev % step_tapbin_ado) > spec_tapbin_ado)
                    step_tapbin_cnt += 10;

                if(volt_calc < 0)
                    step_tapbin_cnt *= -1;

                osal_printf("step_tapbin_cnt: %d\n", step_tapbin_cnt);

                //Set tapbin
                tapbin += step_tapbin_cnt / 10;

                if(tapbin < low_limit)
                {
                    osal_printf("PHY_%d, CH_%d, NEW_R_CAL result = %d is less than %d !!!\n", port, channel, tapbin, low_limit);
                    return RT_ERR_FAILED;
                }
                else if(tapbin > high_limit)
                {
                    osal_printf("PHY_%d, CH_%d, NEW_R_CAL result = %d is greater than %d !!!\n", port, channel, tapbin, high_limit);
                    return RT_ERR_FAILED;
                }
                else
                {
                    phy_field_write(unit, port, 0xBCE, 16, bit_tapbin_end[channel], bit_tapbin_start[channel], tapbin); //tapbin
                    phy_field_write(unit, port, 0xBCE, 17, bit_tapbin_end[channel], bit_tapbin_start[channel], tapbin); //tapbin_pm
                }
                loop ++;
            }

            //Offset tapbin
            tapbin_cal += R_OFS[channel][phyId];
            osal_printf("   - tapbin offset:  R_OFS[%d][%d]:%d tapbin_cal: %d\n", channel, phyId, R_OFS[channel][phyId], tapbin_cal);
            //Write back tapbin_cal
            if(tapbin < low_limit)
            {
                osal_printf("PHY_%d, CH_%d, NEW_R_CAL result = %d is less than %d !!!\n", port, channel, tapbin, low_limit);
                return RT_ERR_FAILED;
            }
            else if(tapbin > high_limit)
            {
                osal_printf("PHY_%d, CH_%d, NEW_R_CAL result = %d is greater than %d !!!\n", port, channel, tapbin, high_limit);
                return RT_ERR_FAILED;
            }
            else
            {
                phy_field_write(unit, port, 0xBCE, 16, bit_tapbin_end[channel], bit_tapbin_start[channel], tapbin); //tapbin
                phy_field_write(unit, port, 0xBCE, 17, bit_tapbin_end[channel], bit_tapbin_start[channel], tapbin); //tapbin_pm
            }

        }
    }

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        //Release power control
        phy_field_write(unit, port, 0xBC0, 19, 4, 4, 0); //GETPOWCTRL_L
        phy_field_write(unit, port, 0xBC0, 18, 3, 0, 0xF); //FORCEGIGATX_L
        phy_field_write(unit, port, 0xBC0, 18, 7, 4, 0xF); //FORCEGIGARX_L

        //Back to default signal path
        phy_field_write(unit, port, 0xBC0, 17, 3, 0, 0xF); //EN_HYBRID_TX_L
        phy_field_write(unit, port, 0xBC0, 17, 7, 4, 0xF); //EN_HYBRID_TXRX_L
        phy_field_write(unit, port, 0xBC0, 17, 11, 8, 0x0); //EN_HYBRID_ABSHORT_L
        phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0x0); //EN_KADC_L

        //Release PGA gain
        _frc_pga_gain(unit, port, 0);

        //Disable TX test mode DC output
        _phy_dc_mode(unit, port, DC_MODE_NA);

        phy_field_read(unit, port, 0xBCE, 16, 15, 0, &tapbin); //tapbin
        NEW_R_CAL[port] = tapbin;
    }

    return RT_ERR_OK;
}

uint32
_amp_chk_ext(uint32 unit, rtk_portmask_t portmask)
{
    uint32 channel;
    uint32 mvoltP, mvoltN;
    int32 mvoltPos, mvoltNeg, mvoltDiff;
    uint32 phyId;
    rtk_port_t port;

    osal_printf("=== %s ===\n", __FUNCTION__);

    _lan_switch_select(unit, SWITCH_MODE_DC);

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        phyId = port % 8;

        for(channel = 0; channel < 4; channel ++)
        {
            _phy_dc_mode(unit, port, DC_MODE_POS_1);

            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 0, &mvoltP);
            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 1, &mvoltN);

            mvoltPos = mvoltP - mvoltN;

            osal_printf("port %d channel %d mvoltPos %d\n", port, channel, mvoltPos);

            _phy_dc_mode(unit, port, DC_MODE_NEG_1);

            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 0, &mvoltP);
            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 1, &mvoltN);

            mvoltNeg = mvoltP - mvoltN;

            osal_printf("port %d channel %d mvoltNeg %d\n", port, channel, mvoltNeg);

            mvoltDiff = (mvoltPos - mvoltNeg) / 2;

            osal_printf("port %d channel %d mvoltDiff %d\n", port, channel, mvoltDiff);

            if(mvoltDiff < 1024)
            {
                osal_printf("PHY_%d, CH_%d, AMP_CHK_EXT result = %d is less than 1.0241(V)\n", port, channel, mvoltDiff);
                _phy_dc_mode(unit, port, DC_MODE_NA);
                _lan_switch_select(unit, SWITCH_MODE_OPEN);
                return RT_ERR_FAILED;
            }
            else if (mvoltDiff > 1097)
            {
                osal_printf("PHY_%d, CH_%d, AMP_CHK_EXT result = %d is larger than 1.09725(V)\n", port, channel, mvoltDiff);
                _phy_dc_mode(unit, port, DC_MODE_NA);
                _lan_switch_select(unit, SWITCH_MODE_OPEN);
                return RT_ERR_FAILED;
            }
        }

        _phy_dc_mode(unit, port, DC_MODE_NA);
    }

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    return RT_ERR_OK;

}

uint32
_cal_para_rd(uint32 unit, rtk_portmask_t portmask, char *pPara)
{
    uint32 adc_ioffset, len, rlen, tapbin, tapbin_pm, ibadj;
    rtk_port_t port;

    osal_printf("=== %s ===\n", __FUNCTION__);

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        //adc_ioffset
        if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "adc_ioffset") == 0)
        {
            phy_field_write(unit, port, 0, 30, 15, 0, 1);
            phy_field_read(unit, port, 0xBCF, 22, 15, 0, &adc_ioffset);

            osal_printf("port %d adc_ioffset: 0x%x\n", port, adc_ioffset);
        }


        //len
        if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "len") == 0)
        {
            phy_field_write(unit, port, 0, 30, 15, 0, 1);
            phy_field_read(unit, port, 0xBCD, 22, 15, 0, &len);

            osal_printf("port %d len: 0x%x\n", port, len);
        }


        //rlen
        if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "rlen") == 0)
        {
            phy_field_write(unit, port, 0, 30, 15, 0, 1);
            phy_field_read(unit, port, 0xBCD, 23, 15, 0, &rlen);

            osal_printf("port %d rlen: 0x%x\n", port, rlen);
        }

        //tapbin
        if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "tapbin") == 0)
        {
            phy_field_write(unit, port, 0, 30, 15, 0, 1);
            phy_field_read(unit, port, 0xBCE, 16, 15, 0, &tapbin);

            osal_printf("port %d tapbin: 0x%x\n", port, tapbin);
        }

        //tapbin_pm
        if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "tapbin_pm") == 0)
        {
            phy_field_write(unit, port, 0, 30, 15, 0, 1);
            phy_field_read(unit, port, 0xBCE, 17, 15, 0, &tapbin_pm);

            osal_printf("port %d tapbin_pm: 0x%x\n", port, tapbin_pm);
        }


        //ibadj
        if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "ibadj") == 0)
        {
            phy_field_write(unit, port, 0, 30, 15, 0, 1);
            phy_field_read(unit, port, 0xBCA, 22, 15, 0, &ibadj);

            osal_printf("port %d ibadj: 0x%x\n", port, ibadj);
        }
    }
    return RT_ERR_OK;
}

uint32
_efuse_chk_busy(uint32 unit, uint32 port)
{
    uint32 i = 1, busy;

    do
    {
        phy_field_read(unit, port, 0x26E, 16, 2, 2, &busy);

        if(busy == 1)
        {
            if(i == 20)
            {
                osal_printf("FAIL E-Fuse busy bit is not return to 0.\n");
                return RT_ERR_FAILED;
            }
            i ++;
        }
        else
            break;
    } while(1);

    return RT_ERR_OK;
}

uint32
_efuse_read(uint32 unit, uint32 port, uint32 entry, uint32 *pData)
{
    uint32 ret;

    phy_field_write(unit, port, 0, 30, 15, 0, 8); /*configure page_addr:30 as top block:8*/
    phy_field_write(unit, port, 0x26E, 17, 15, 0, entry); /*efuse_adr_reg, block:8, page:0x26e, reg:17*/
    phy_field_write(unit, port, 0x26E, 16, 0, 0, 0); /*efuse_cmd_wr,  block:8, page:0x26e, reg:16, bit:1*/
    ret = _efuse_chk_busy(unit, port);
    if(ret != RT_ERR_OK)
        return ret;
    phy_field_read(unit, port, 0x26E, 19, 15, 0, pData);

    return RT_ERR_OK;
}

uint32
_efuse_write(uint32 unit, uint32 port, uint32 entry, uint32 data)
{
    uint32 ret, tmp;

    phy_field_write(unit, port, 0, 30, 15, 0, 8); /*configure page_addr:30 as top block:8*/
    phy_field_write(unit, port, 0x26E, 17, 15, 0, entry); /*efuse_adr_reg, block:8, page:0x26e, reg:17*/
    phy_field_write(unit, port, 0x26E, 18, 15, 0, data); /*efuse_adr_reg, block:8, page:0x26e, reg:18*/
    phy_field_write(unit, port, 0x26E, 16, 0, 0, 1); /*efuse_cmd_wr,  block:8, page:0x26e, reg:16, bit:1*/
    ret = _efuse_chk_busy(unit, port);
    if(ret != RT_ERR_OK)
        return ret;
    phy_field_write(unit, port, 0x26E, 16, 0, 0, 0); /*efuse_cmd_wr,  block:8, page:0x26e, reg:16, bit:1*/
    ret = _efuse_chk_busy(unit, port);
    if(ret != RT_ERR_OK)
        return ret;
    phy_field_read(unit, port, 0x26E, 19, 15, 0, &tmp);
    if(tmp != data)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

uint32
_cal_para_efuse_write(uint32 unit, rtk_portmask_t portmask)
{
    uint32 ret;
    uint32 entry;
    rtk_port_t port, phyid;

    osal_printf("Execute CAL_PARA_WR_EFUSE procedure.\n");

    RT_ERR_CHK(_efuse_write(unit, 0, 0, 0xFF00), ret);

    entry = 2;

    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        phyid = port / 8;

        RT_ERR_CHK(_efuse_write(unit, phyid, entry, RC_CAL[0][port % 8]), ret); //len_short
        entry ++;
        RT_ERR_CHK(_efuse_write(unit, phyid, entry, RC_CAL[0][port % 8]), ret); //rlen_short
        entry ++;
        RT_ERR_CHK(_efuse_write(unit, phyid, entry, NEW_R_CAL[port % 8]), ret); //tapbin
        entry ++;
        RT_ERR_CHK(_efuse_write(unit, phyid, entry, NEW_R_CAL[port % 8]), ret); //tapbin_pm
        entry ++;
        RT_ERR_CHK(_efuse_write(unit, phyid, entry, AMP_CAL_EXT[port % 8]), ret); //ibadj
        entry ++;
        RT_ERR_CHK(_efuse_write(unit, phyid, entry, ADC_OFS_CAL[port % 8]), ret); //adc_ioffset
        entry ++;
    }

    entry = 235;
    RT_ERR_CHK(_efuse_write(unit, 0, entry, 0x46), ret); //adc_ioffset

    entry = 236;
    for(port = 0; port < 64; port ++)
    {
        if(!RTK_PORTMASK_IS_PORT_SET(portmask, port))
            continue;

        phyid = port / 8;

        RT_ERR_CHK(_efuse_write(unit, phyid, entry, RC_CAL[1][port % 8]), ret); //len_long
        entry ++;
        RT_ERR_CHK(_efuse_write(unit, phyid, entry, RC_CAL[1][port % 8]), ret); //rlen_long
        entry ++;
    }

    RT_ERR_CHK(_efuse_write(unit, 0, 252, 0x1000), ret);
    RT_ERR_CHK(_efuse_write(unit, 0, 253, 0x0231), ret);


    return RT_ERR_OK;

}

int32 _pkt_buf_alloc(rtdrv_ext_pkt_t **pPkt, uint32 len)
{
    *pPkt = osal_alloc(sizeof(rtdrv_ext_pkt_t));
    if (NULL == *pPkt)
    {
        osal_printf("Alloc memery fail !\n");
        return RT_ERR_FAILED;
    }

    (*pPkt)->data = osal_alloc(len + 4);

    if (NULL == (*pPkt)->data)
    {
        osal_free(*pPkt);
        osal_printf("Alloc packet buffer fail !\n");
        return RT_ERR_FAILED;
    }

    (*pPkt)->length = len;

    return RT_ERR_OK;
}

int32 _pkt_buf_free(rtdrv_ext_pkt_t *pPkt)
{
    osal_free(pPkt->data);
    osal_free(pPkt);

    return RT_ERR_OK;
}

int32 _uint2buf(uint32 value, uint8 *pBuf)
{
    if (NULL == pBuf)
    {
        return RT_ERR_FAILED;
    }

    pBuf[0] = (value & 0xff000000) >> 24;
    pBuf[1] = (value & 0x00ff0000) >> 16;
    pBuf[2] = (value & 0x0000ff00) >> 8;
    pBuf[3] = (value & 0x000000ff) >> 0;

    return RT_ERR_OK;
}

int32 _dal_longan_packetGen_buf_oper(uint32 unit, uint32 cell, uint32 byte, uint32 isWr, uint32 *pVal)
{
    int32   ret = RT_ERR_OK;
    uint32 value =0;
    const char *pRWstr[] = {
        "Read",
        "Write",
        };

    #if EXT_SRAM_PKT_TEST
    if(cell >= PKT_STREAM_LAST_PORT_PAGE_NUM * PKT_PAGE_CELL_NUM)
    #else
    if(cell >= PKT_STREAM_PAGE_NUM * PKT_PAGE_CELL_NUM)
    #endif
    {
            osal_printf("%s packet cell is out of range: ERROR!  \n", pRWstr[isWr]);
            return RT_ERR_OUT_OF_RANGE;
    }

    if(byte >= PKT_CELL_BYTE)
    {
            osal_printf("%s packet cell byte is out of range: ERROR!  \n", pRWstr[isWr]);
            return RT_ERR_OUT_OF_RANGE;
    }

    //byte==88, only set 16bit value
    if (0 != byte%4)
    {
        osal_printf("%s packet cell byte position: ERROR!  \n", pRWstr[isWr]);
        return RT_ERR_INPUT;
    }

    #if EXT_SRAM_PKT_TEST
    value = (byte & 0xff) | ((cell & 0xff) << 8) | ((isWr&0x1)<<30);
    RT_ERR_CHK(ioal_mem32_write(unit, REG_SPG_PB_ACCESS_CTRL2, value), ret);
    #else
    RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL2r,LONGAN_PB_TYPEf, &isWr), ret);
    RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL2r,LONGAN_PB_CELL_INDEXf, &cell), ret);
    RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL2r,LONGAN_PB_BYTE_INDEXf, &byte), ret);
    #endif

    if (isWr)
    {
        #if EXT_SRAM_PKT_TEST
        RT_ERR_CHK(ioal_mem32_write(unit, REG_SPG_PB_ACCESS_CTRL1, *pVal), ret);
        value = value | (1 << 31);
        RT_ERR_CHK(ioal_mem32_write(unit, REG_SPG_PB_ACCESS_CTRL2, value), ret);
        #else
        value = *pVal;
        RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL1r,LONGAN_ACCESS_DATAf, &value), ret);
        value = 1;
        RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL2r,LONGAN_PB_TRIGf, &value), ret);
        #endif
    }
    else
    {
        #if EXT_SRAM_PKT_TEST
        value = value | (1 << 31);
        RT_ERR_CHK(ioal_mem32_write(unit, REG_SPG_PB_ACCESS_CTRL2, value), ret);
        RT_ERR_CHK(ioal_mem32_read(unit, REG_SPG_PB_ACCESS_CTRL1, &value), ret);
        #else
        value = 1;
        RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL2r,LONGAN_PB_TRIGf, &value), ret);
        RT_ERR_CHK(reg_field_read(unit, LONGAN_SPG_PB_ACCESS_CTRL1r,LONGAN_ACCESS_DATAf, &value), ret);
        #endif
        *pVal = value;
    }

    return ret;
}

//streamIdx = 0~55
// user pkt offset : 0,  2,  6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82,86     4n-2(n>=1)
// asic  byte        : 88, 84 80, 76, 72, 68, 64,60, 56, 52, 48,44, 40, 36, 32, 28, 24, 20, 16, 12, 8,  4, 0
// note : if offset != 0, the len should less than 90
int32 _pkt_stream_offset_oper(uint32 unit, uint32 streamIdx, uint32 offset, uint8 *pktBuf, uint32 len, uint32 isWrite)
{
    int32   ret = RT_ERR_OK;
    uint32  oper = 0;
    uint32  value;
    uint32 reg, regVal;
    uint32  pbCellIdx, pbByteIndex = 0;
    int32   i, pktIdx=0;
    uint32  cellCnt = 0, startCell = 0, startIdx = 0;
    int32   rwByte = 0, byteIdx, baseShift = 0, lastCellByte = 0, cellByteOffset = 0;
    int32   begin2Byte, OffsetByteIdx=0, OffsetRdByte=0;
    uint32  aligByte = 0;

    typedef struct pgn_byte_idx_s{
        uint8 usrIdx;
        uint8 asicIdx;
    }pgn_byte_idx_t;

    pgn_byte_idx_t idxArry[23];
    uint32 rdValMask[] = {0xff000000,0xffff0000,0xffffff00};
    uint32 rmHValMask[] = {0x00ffffff,0x0000ffff,0x000000ff};

    if (len == 0) // unsigned int32
        return RT_ERR_OK;

    #if EXT_SRAM_PKT_TEST
    if ((offset+len) > PKTGEN_LAST_PORT_MAX_LEN)
    #else
    if ((offset+len) > PKTGEN_MAX_LEN)
    #endif
    {
        osal_printf("Packet offset is out of range max :%d  \n", PKTGEN_MAX_LEN);
        return RT_ERR_OUT_OF_RANGE;
    }

   if (len < 4)
   {
        osal_printf("Packet wite length less than 4 \n");
        return RT_ERR_INPUT;
    }

    if (streamIdx > PKT_STREAM_IDX_MAX)
        return RT_ERR_OUT_OF_RANGE;

    if (isWrite)
        oper = 1;

    value = streamIdx;
    RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_PB_ACCESS_CTRL0r,LONGAN_PB_INDEXf, &value), ret);

    reg = REG_SPG_GLOBAL_INDEX_CTRL0 + (streamIdx/2) * 4;
    value = page_base + streamIdx * PKT_STREAM_PAGE_NUM;

    #if EXT_SRAM_PKT_TEST
    if (PKT_STREAM_IDX_MAX == streamIdx)
        value = page_base + streamIdx * PKT_STREAM_PAGE_NUM + (PKT_STREAM_LAST_PORT_PAGE_NUM - PKT_STREAM_PAGE_NUM);
    #endif

    if (0 == streamIdx%2)
    {
        RT_ERR_CHK(ioal_mem32_read(unit, reg, &regVal), ret);
        regVal = (regVal & 0xffff0000) | value;
        RT_ERR_CHK(ioal_mem32_write(unit, reg, regVal), ret);
    }
    else
    {
        RT_ERR_CHK(ioal_mem32_read(unit, reg, &regVal), ret);
        regVal = (regVal & 0x0000ffff) | (value << 16);
        RT_ERR_CHK(ioal_mem32_write(unit, reg, regVal), ret);
    }

    idxArry[0].usrIdx = 0;
    idxArry[0].asicIdx = 88;

    for(i = 1; i < sizeof(idxArry)/sizeof(pgn_byte_idx_t);i++)
    {
        idxArry[i].usrIdx = 4 * i -2;
        idxArry[i].asicIdx = 86 - idxArry[i].usrIdx;
    }

    cellCnt = ((offset + len) + 89)/PKT_CELL_BYTE;
    startCell = offset/PKT_CELL_BYTE;
    startIdx = offset%PKT_CELL_BYTE;

    for (i = 0; i < sizeof(idxArry)/sizeof(pgn_byte_idx_t);i++)
    {
       if (startIdx <=  idxArry[i].usrIdx)
       {
            if (startIdx ==  idxArry[i].usrIdx)
            {
                OffsetByteIdx = i;
                startIdx = idxArry[OffsetByteIdx].usrIdx;
            }
            else
            {
                OffsetByteIdx = i-1;
                OffsetRdByte = idxArry[i].usrIdx - startIdx;
            }

            pbByteIndex = idxArry[OffsetByteIdx].asicIdx;

            cellByteOffset = startIdx;
            break;
        }
    }

    if (i == sizeof(idxArry)/sizeof(pgn_byte_idx_t))
    {
        if (0 != startIdx)
        {
            lastCellByte = 1;
            cellByteOffset = 0;
            OffsetRdByte = 90 - startIdx;
            pbByteIndex = 0; //last byte of cell packet bufer
        }
    }

   if (0 != OffsetRdByte)
   {
        RT_ERR_CHK(_dal_longan_packetGen_buf_oper(unit,startCell,pbByteIndex,FALSE,&value),ret);
        value = value & rdValMask[4-1-OffsetRdByte];
        for (byteIdx = 0; byteIdx < OffsetRdByte; byteIdx++)
        {
            value |= (pktBuf[pktIdx + byteIdx] & 0xff) << ((OffsetRdByte-1)*8 - byteIdx*8);
        }

        RT_ERR_CHK(_dal_longan_packetGen_buf_oper(unit,startCell,pbByteIndex,TRUE,&value),ret);

        pktIdx+=OffsetRdByte;
        if (lastCellByte)
        {
            startIdx = 0;
            startCell += 1;
        }
        else
        {
            startIdx = idxArry[OffsetByteIdx+1].usrIdx;  //The algin positation
        }
   }

    for (pbCellIdx = startCell; pbCellIdx < cellCnt; pbCellIdx++)
    {
        for (i = 0; i < PKT_CELL_BYTE; )
        {
            if (pktIdx >= len)
                goto exit;

            if (0 != cellByteOffset)
            {
                if ((i < startIdx) && (pbCellIdx == startCell))
                {
                    i+=1;
                    continue;
                }
                else
                {
                    if ((i != 0) && (!aligByte))
                    {
                        i+=2;
                        aligByte = 1;
                    }
                }
            }

            if (0 == i)
                begin2Byte = TRUE;
            else
                begin2Byte = FALSE;

            OffsetRdByte = 0;
            if (TRUE == begin2Byte)
            {
                pbByteIndex = 90-2;
                rwByte = (len - pktIdx) >= 2 ? 2 : (len - pktIdx);
                baseShift = 8;
                if (rwByte < 2)
                    OffsetRdByte = (len - pktIdx);
            }
            else
            {
                pbByteIndex = 88-i%PKT_CELL_BYTE;
                rwByte = (len - pktIdx) >= 4 ? 4 : (len - pktIdx);
                baseShift = 24;
                if (rwByte < 4)
                {
                    OffsetRdByte = (len - pktIdx);
                }
            }

            //osal_printf("pktIdx = %d, len = %d, begin2Byte = %d, rwByte = %d, i = %d,  offset = %d ,  cell = %d, pbByteIndex = %d \n",
            //                  pktIdx, len ,begin2Byte, rwByte, i, offset, pbCellIdx, pbByteIndex);

            if (isWrite)
            {
                value = 0;
                if (0 != OffsetRdByte)
                {
                    RT_ERR_CHK(_dal_longan_packetGen_buf_oper(unit,pbCellIdx,pbByteIndex,FALSE,&value),ret);

                    if (begin2Byte)
                       value = value & 0x00ff;
                    else
                        value = value & rmHValMask[rwByte-1];
                }

                for (byteIdx = 0; byteIdx < rwByte; byteIdx++)
                {
                    //mask the value;
                    value |= (pktBuf[pktIdx + byteIdx] & 0xff) << (baseShift - byteIdx*8);
                    //if (0 != OffsetRdByte)
                        //osal_printf("Write value = 0x%8x \n", value);
                }
                RT_ERR_CHK(_dal_longan_packetGen_buf_oper(unit,pbCellIdx,pbByteIndex,oper,&value),ret);
            }
            else
            {
                RT_ERR_CHK(_dal_longan_packetGen_buf_oper(unit,pbCellIdx,pbByteIndex,oper,&value),ret);
                for (byteIdx = 0; byteIdx < rwByte; byteIdx++)
                {
                    pktBuf[pktIdx+byteIdx] = (value & (0xff << (baseShift -byteIdx*8))) >> (baseShift -byteIdx*8) ;
                }
            }

            if ((TRUE == begin2Byte))
                pktIdx+=2;
            else
                pktIdx+=4;

            i+=4;
        }
    }


exit:
    return ret;
}

//for test pkt buf
int32 _pkt_pattern_generate(uint32 port, uint32 streamIdx, uint8 *pktBuf, uint32 offset, uint32 len, rtdrv_ext_spg_payload_type_t type)
{
    int32 i;
    uint32 *ptr;

    RT_PARAM_CHK((NULL == pktBuf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(0, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(streamIdx >= PKTGEN_PORT_STREAM_NUM, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(type > RTDRV_EXT_SPG_PAYLOAD_REPEAT, RT_ERR_INPUT);
    RT_PARAM_CHK((offset+len) >= PKTGEN_MAX_LEN, RT_ERR_INPUT);

    if (len == 0) // unsigned int32
        return RT_ERR_OK;

    switch (type)
        {
            case RTDRV_EXT_SPG_PAYLOAD_ZERO:
                break;

            case RTDRV_EXT_SPG_PAYLOAD_FIX:
                ptr = (uint32*)&pktBuf[offset];
                *ptr = spg_payload_pattern[port][streamIdx];
                break;

            case RTDRV_EXT_SPG_PAYLOAD_REPEAT:
                {
                    ptr = (uint32*)&pktBuf[offset];
                    for (i = 0; i < len/4; i++)
                    {
                        *ptr = spg_payload_pattern[port][streamIdx];
                        ptr++;
                    }

                    if (0 != len%4)
                    {
                        for (i = 0; i < len%4; i++)
                        {
                            pktBuf[offset + (len/4)*4 + i] =  (*ptr >> (24 - 8 * i)) & 0xff;
                        }
                    }
                }
                break;

            case RTDRV_EXT_SPG_PAYLOAD_INCR:
                {
                    for(i = 0; i < len; i++)
                    {
                        pktBuf[offset+i] = (i & 0xff);
                    }
                }
                break;

            case RTDRV_EXT_SPG_PAYLOAD_DECR:
                {
                   for(i = 0; i < len; i++)
                    {
                        pktBuf[offset+i] = ((unsigned char)0xff - (i & 0xff));
                    }
                }
                break;

            default:
                break;
        }

    return RT_ERR_OK;
}

int32 pkt_stream_read(uint32 unit, uint32 streamIdx, uint32 offset, uint32 len, uint8 *pktBuf)
{
    return _pkt_stream_offset_oper(unit, streamIdx, offset, pktBuf, len, FALSE);
}

int32 pkt_stream_write(uint32 unit, uint32 streamIdx, uint32 offset, uint32 len, uint8 *pktBuf)
{
    return _pkt_stream_offset_oper(unit, streamIdx, offset, pktBuf, len, TRUE);
}


#if EXT_SRAM_PKT_TEST
int32 pkt_strem_buffer_section_set(uint32 unit, uint32 isLast2k)
{
    int32 ret = RT_ERR_OK;
    rtk_portmask_t pmsk;
    uint32 index0, index1;
    rtk_port_t port;

    if (isLast2k)
        page_base = 0x0800;
    else
        page_base = 0x0;

     //set the port stream index
    pmsk.bits[0] = 0x0fffffff;
    pmsk.bits[1] = 0;
    RTK_PORTMASK_SCAN(pmsk, port)
    {
        index0 = page_base + (port * 2 + 0) * PKT_STREAM_PAGE_NUM;
        index1 = page_base + (port * 2 + 1) * PKT_STREAM_PAGE_NUM;
        ret += reg_array_field_write(unit, LONGAN_SPG_PORT_INDEX_CTRL0r,
                    port, REG_ARRAY_INDEX_NONE, LONGAN_PORT0_S1_INDEXf, &index1);
        ret += reg_array_field_write(unit, LONGAN_SPG_PORT_INDEX_CTRL0r,
                    port, REG_ARRAY_INDEX_NONE, LONGAN_PORT0_S0_INDEXf, &index0);
   }

    return ret;
}

int32 pkt_stream_rw_time_test(uint32 unit)
{
    int32 ret = RT_ERR_OK;
    uint32 testCnt =  10;
    uint32 startTime =0, endTime = 0;
    uint32 i, j, strIdx;
    uint32 sectBuf[] = {TRUE, FALSE};
    uint32 pktLen = 12288;
    uint32 value;

    _pkt_buf_alloc(&pDiagExtPacket, pktLen);
    osal_memset(pDiagExtPacket->data, 0xff, pktLen);

    value = ENABLED;
    RT_ERR_CHK(reg_field_write(unit, LONGAN_SPG_GLB_CTRLr,LONGAN_SPG_MODEf, &value), ret);
    RT_ERR_CHK(reg_field_write(unit, LONGAN_PKB_ACC_DEBUG_CTRLr,LONGAN_DBG_ACC_PKB_ENf, &value), ret);

    RT_ERR_CHK(osal_time_usecs_get(&startTime), ret);
    for (j = 0; j < 2; j++)
    {
        RT_ERR_CHK(pkt_strem_buffer_section_set(unit, sectBuf[j]), ret);
        for (i = 0; i < testCnt; i++)
        {
            for (strIdx = 0; strIdx < 28*2; strIdx++)
            {
                RT_ERR_CHK(pkt_stream_write(unit, strIdx, 0, pktLen, pDiagExtPacket->data), ret);
            }
        }
    }
    RT_ERR_CHK(osal_time_usecs_get(&endTime), ret);
    osal_printf("write test %d times use time %u us, all page write avg time is: %d ms \n", testCnt, (endTime-startTime), (endTime-startTime)/(1000*testCnt));

    RT_ERR_CHK(osal_time_usecs_get(&startTime), ret);

    for (j = 0; j < 2; j++)
    {
        RT_ERR_CHK(pkt_strem_buffer_section_set(unit, sectBuf[j]), ret);
        for (i = 0; i < testCnt; i++)
        {
            for (strIdx = 0; strIdx < 28*2; strIdx++)
            {
                RT_ERR_CHK(pkt_stream_read(unit, strIdx, 0, pktLen, pDiagExtPacket->data), ret);
            }
        }
    }
    RT_ERR_CHK(osal_time_usecs_get(&endTime), ret);
    osal_printf("read test %d times use time %u us, all page write avg time is: %d ms \n", testCnt, (endTime-startTime), (endTime-startTime)/(1000*testCnt));

    _pkt_buf_free(pDiagExtPacket);

    //restore
    RT_ERR_CHK(pkt_strem_buffer_section_set(unit, FALSE), ret);
    value = DISABLED;
    RT_ERR_CHK(reg_field_write(unit, LONGAN_PKB_ACC_DEBUG_CTRLr,LONGAN_DBG_ACC_PKB_ENf, &value), ret);
    page_base = PKT_STREAM_PAGE_BASE;

    return RT_ERR_OK;
}


int32 pktIndex2cellByte(uint32 pktIdx, uint32 *pPageIdx, uint32 *pCell, uint32 *pByte)
{
    uint32 byteOffset = 0;

    RT_PARAM_CHK(pktIdx >= PKTGEN_LAST_PORT_MAX_LEN, RT_ERR_INPUT);
    *pPageIdx = pktIdx/360;
    *pCell = (pktIdx%360)/90;

    byteOffset = pktIdx%90;
    if (byteOffset%2)
        return RT_ERR_INPUT;

    if (0 == byteOffset)
        *pByte = 88;
    else
        *pByte = 86-byteOffset;

    return RT_ERR_OK;
}

int32 pktbuf_rw_test(uint32 unit, rtdrv_ext_bufferTest_t opter)
{
    int32 ret;
    uint32 pktLen = PKTGEN_LAST_PORT_MAX_LEN;
    int32 j, i;
    uint32 sectBuf[] = {FALSE, TRUE};
    uint32 value;
    uint32 strIdx;
    static  int32 wrValue = -1;
    uint32 pageNum =  PKT_STREAM_PAGE_NUM;

    RT_PARAM_CHK(opter >= RTDRV_EXT_BUF_TEST_END, RT_ERR_INPUT);

    _pkt_buf_alloc(&pDiagExtPacket, pktLen);
    osal_memset(pDiagExtPacket->data, 0x0, pktLen);

    value = ENABLED;
    RT_ERR_HDL(reg_field_write(unit, LONGAN_SPG_GLB_CTRLr,LONGAN_SPG_MODEf, &value), exit, ret);
    RT_ERR_HDL(reg_field_write(unit, LONGAN_PKB_ACC_DEBUG_CTRLr,LONGAN_DBG_ACC_PKB_ENf, &value), exit, ret);

    switch (opter)
        {
            case RTDRV_EXT_BUF_TEST_WR0:
            case RTDRV_EXT_BUF_TEST_WR1:
                {
                    if (RTDRV_EXT_BUF_TEST_WR0 == opter)
                    {
                        wrValue = 0;
                        osal_memset(pDiagExtPacket->data, 0x0, pktLen);
                    }
                    else
                    {
                        wrValue = 1;
                        osal_memset(pDiagExtPacket->data, 0xff, pktLen);
                    }

                    for (j = 0; j < 2; j++)
                    {
                        RT_ERR_HDL(pkt_strem_buffer_section_set(unit, sectBuf[j]), exit,  ret);
                        for (strIdx = 0; strIdx < 28*2; strIdx++)
                        {
                            if (strIdx < 27 * 2)
                                pktLen = PKTGEN_MAX_LEN;
                            else
                                pktLen = PKTGEN_LAST_PORT_MAX_LEN;
                            RT_ERR_HDL(pkt_stream_write(unit, strIdx, 0, pktLen, pDiagExtPacket->data), exit, ret);
                        }
                    }
                    osal_printf("Write packet buffer successful !\n\n");
                    break;
                }
            case RTDRV_EXT_BUF_TEST_READONLY:
                {
                    uint32 pageBaseIdx = 0;
                    for (j = 0; j < 2; j++)
                    {
                        RT_ERR_HDL(pkt_strem_buffer_section_set(unit, sectBuf[j]), exit,  ret);
                        for (strIdx = 0; strIdx < 28*2; strIdx++)
                        {
                            if (strIdx < 27 * 2)
                            {
                                pktLen = PKTGEN_MAX_LEN;
                                pageNum = PKT_STREAM_PAGE_NUM;
                                pageBaseIdx = j * 2048 + strIdx*PKT_STREAM_PAGE_NUM;
                                osal_printf("Packet buffer page for %d to %d :\n", pageBaseIdx,  pageBaseIdx + pageNum-1);
                            }
                            else
                            {
                                pktLen = PKTGEN_LAST_PORT_MAX_LEN;
                                pageNum = PKT_STREAM_LAST_PORT_PAGE_NUM;
                                pageBaseIdx = j * 2048 + 27*2*PKT_STREAM_PAGE_NUM;

                                if (PKT_STREAM_IDX_MAX == strIdx)
                                    osal_printf("Packet buffer page for %d to %d :\n",  pageBaseIdx+pageNum,  pageBaseIdx + 2*pageNum-1);
                                else
                                    osal_printf("Packet buffer page for %d to %d :\n",  pageBaseIdx,  pageBaseIdx + pageNum -1);
                            }

                            RT_ERR_HDL(pkt_stream_read(unit, strIdx, 0, pktLen, pDiagExtPacket->data), exit, ret);

                            #if 0
                            pktLen = 200; //read 100Byte
                            for (i = 0; i < pktLen; i++)
                            {
                                if ((0 == i%16) || (0 == i%360))
                                    osal_printf("0x%04x ", i);

                                osal_printf("%02x ", pDiagExtPacket->data[i]);

                                //per cell put \n
                                if (((i+1)%16 == 0) ||((i+1)%360 == 0))
                                    osal_printf("\n");
                            }
                            osal_printf("\n\n");
                            #endif
                        }
                    }
                    osal_printf("Read packet buffer successful !\n\n");
                    break;
                }
            case RTDRV_EXT_BUF_TEST_READCHECK:
            case RTDRV_EXT_BUF_TEST_READFAIL:
                {
                    uint32 pageOffset = 0, cell = 0, byte = 0;
                    uint32 last, pktData;
                    uint32 errFlag = 0;
                    uint32 errCnt = 0;
                    uint32 pageIdx = 0;
                    uint32 pageBaseIdx = 0;
                    //uint32 *ptr;

                    if (-1 == wrValue)
                    {
                        osal_printf("Have Not write packet buffer. \n");
                        goto exit;
                    }

                    for (j = 0; j < 2; j++)
                    {
                        RT_ERR_HDL(pkt_strem_buffer_section_set(unit, sectBuf[j]), exit,  ret);
                        for (strIdx = 0; strIdx < 28*2; strIdx++)
                        {
                            if (strIdx < 27 * 2)
                            {
                                pageBaseIdx = page_base + strIdx*PKT_STREAM_PAGE_NUM;
                                pktLen = PKTGEN_MAX_LEN;
                            }
                            else
                            {
                                pktLen = PKTGEN_LAST_PORT_MAX_LEN;
                                pageBaseIdx = page_base + 27*2*PKT_STREAM_PAGE_NUM;
                            }

                            osal_memset(pDiagExtPacket->data, 0, pktLen);
                            RT_ERR_HDL(pkt_stream_read(unit, strIdx, 0, pktLen, pDiagExtPacket->data), exit, ret);

                            for (i = 0; i < pktLen; )
                            {
                                last = 0;
                                pktData = pDiagExtPacket->data[i] << 24 |   \
                                                pDiagExtPacket->data[i+1] << 16 |   \
                                                pDiagExtPacket->data[i+2] << 8 |    \
                                                pDiagExtPacket->data[i+3] << 0 ;
                                //ptr = (uint32*)&(pDiagExtPacket->data[i]);
                                //pktData = *ptr;

                                if ((i + 3) > pktLen)
                                {
                                    last = 1;
                                }

                                if (0 == wrValue)
                                {
                                    if ( 0x0 != pktData)
                                    {
                                        errFlag = 1;
                                        errCnt++;
                                        if (RT_ERR_OK != pktIndex2cellByte(i, &pageOffset, &cell, &byte))
                                            osal_printf("Error input packet buffer offset!! \n");

                                        if (PKT_STREAM_IDX_MAX == strIdx)
                                            pageIdx = (pageBaseIdx + PKT_STREAM_LAST_PORT_PAGE_NUM + pageOffset);
                                        else
                                            pageIdx = (pageBaseIdx + pageOffset);

                                        if (RTDRV_EXT_BUF_TEST_READCHECK == opter)
                                            osal_printf("Packet buffer check error! 0x0 !=0x%08x, SW packet Idx: %d,  ASIC page: %d, cell:%d, byte:%d \n",
                                                                                            pktData, i, pageIdx, cell, byte);
                                    }
                                }
                                else
                                {
                                    if (((0xffffffff != pktData) && (0 == last)) || ((last) && (0xffff0000 != pktData)) )
                                    {
                                        errFlag = 1;
                                        errCnt++;
                                        if (RT_ERR_OK != pktIndex2cellByte(i, &pageOffset, &cell, &byte))
                                            osal_printf("Error input packet buffer offset!! \n");

                                        if (PKT_STREAM_IDX_MAX == strIdx)
                                            pageIdx = (pageBaseIdx + PKT_STREAM_LAST_PORT_PAGE_NUM + pageOffset);
                                        else
                                            pageIdx = (pageBaseIdx + pageOffset);

                                        if (RTDRV_EXT_BUF_TEST_READCHECK == opter)
                                            osal_printf("Packet buffer check error! 0xffffffff !=0x%08x, SW packet Idx: %d, ASIC page: %u, cell:%u, byte:%u \n",
                                                                                                   pktData, i, pageIdx, cell, byte);
                                    }
                                }

                                if (0 == i%90)
                                    i +=2;
                                else
                                    i +=4;

                                if (i >= pktLen)
                                    break;

                                #if 0
                                if (errCnt > 100)
                                {
                                    osal_printf("Check packet buffer failure !\n\n");
                                    goto exit;
                                }
                                #endif
                            }

                        }
                    }

                    if (RTDRV_EXT_BUF_TEST_READCHECK == opter)
                    {
                        if (errFlag)
                            osal_printf("Check packet buffer failure !\n\n");
                        else
                            osal_printf("Check packet buffer successful !\n\n");
                    }
                    else
                    {
                        RT_ERR_HDL(ioal_mem32_write(unit, REG_CHK_FAIL_NUM, errCnt), exit, ret);
                    }
                    break;
                }

            default:
                break;
        }


exit:
    _pkt_buf_free(pDiagExtPacket);
    return RT_ERR_OK;
}

#endif


static drv_nic_rx_t _loopback_test_rx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    uint32  ret;

    if (NULL == pPacket)
    {
        osal_printf("Error: pPacket is NULL at %s():%d\n", __FUNCTION__, __LINE__);
        goto _exit;
    }

    if (pPacket->rx_tag.reason == TRAP_RSN_RMA_UDF0)
    {
        if (pPacket->rx_tag.source_port < LOOPBACK_TEST_PORT_MAX_NUM && pPacket->rx_tag.dev_id < LOOPBACK_TEST_UNIT_MAX_NUM)
        {
            ret = osal_memcmp(pPacket->data, loopbackTestPktPattern1, 60);
            if (0 == ret)
                loopback_test_cpu_pkt_cnt[pPacket->rx_tag.dev_id][pPacket->rx_tag.source_port]++;
        }
    }

#if 0
    RT_ERR_HDL(drv_nic_pkt_free(unit, pPacket), _exit, ret);

    return NIC_RX_HANDLED_OWNED;
#else
    return drv_l2ntfy_pkt_handler(unit, pPacket, pCookie);
#endif

_exit:
    return NIC_RX_NOT_HANDLED;
}


static int32 _loopback_test_packet_send(rtk_mac_t dmac, rtk_mac_t smac, uint32 pktNum, uint32 len, rtk_portmask_t portmask, uint32 gapTime)
{
    int32  ret = RT_ERR_OK;
    uint32 unit;
    uint32 localUnit = HWP_MY_UNIT_ID();
    uint32 i;
    uint32 sentPktNum = 0;
    uint32 pktNumInQueue;
    uint32 nicPktLen = len - 4; // not include CRC

    rtk_port_t port;

    drv_nic_pkt_t *pPktBuf;

    if(pktNum > loopback_test_nic_max_depth)
        pktNumInQueue = loopback_test_nic_max_depth;
    else
        pktNumInQueue = pktNum;


    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        /* CPU send packets to each port */
        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {
                /* send pktNum packets */
                sentPktNum = 0;
                while (sentPktNum < pktNum)
                {
                    for (i = 0; i < pktNumInQueue; i++)
                    {
                        ret = drv_nic_pkt_alloc(localUnit, nicPktLen, 0, &pPktBuf);
                        if (RT_ERR_OK != ret)
                        {
                            osal_printf("alloc packet failure!!!\n");
                            return ret;
                        }

                        /* Packet DA is Switch Mac */
                        osal_memcpy(&loopbackTestPktPattern1[0], dmac.octet, 6);
                        /* SMAC */
                        osal_memcpy(&loopbackTestPktPattern1[6], smac.octet, 6);

                        osal_memset(pPktBuf->data, 0, nicPktLen);
                        if (nicPktLen >= sizeof(loopbackTestPktPattern1))
                            osal_memcpy(pPktBuf->data, loopbackTestPktPattern1, sizeof(loopbackTestPktPattern1));
                        else
                            osal_memcpy(pPktBuf->data, loopbackTestPktPattern1, nicPktLen);

                        pPktBuf->length = nicPktLen;
                        pPktBuf->tail   += pPktBuf->length;
                        pPktBuf->end    += pPktBuf->length;
                        pPktBuf->as_txtag = TRUE;
                        pPktBuf->tx_tag.fwd_type = NIC_FWD_TYPE_LOGICAL;
                        pPktBuf->tx_tag.dev_id = unit;
                        pPktBuf->tx_tag.dst_port_mask = ((uint32)1 << port) & 0xffffffff;
                        pPktBuf->tx_tag.dst_port_mask_1 = 0;

                        ret = drv_nic_pkt_tx(localUnit, pPktBuf, NULL, NULL);
                        if (RT_ERR_OK != ret)
                        {
                            drv_nic_pkt_free(localUnit, pPktBuf);
                            osal_printf( "nic tx fail\n");
                        }
                        sentPktNum++;
                    }
                    osal_time_udelay(gapTime);
                }
            }
        }
    }

    return ret;
}

static int32 _loopback_test_int_loopback_cfg(rtk_portmask_t portmask, rtk_enable_t enable)
{
    uint32 unit;
    int32 ret = RT_ERR_OK;
    rtk_port_t port;
    //rtk_port_phyPolarCtrl_t     polarCtrl;

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        /* Set port internal loopback */
        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {
                if(HWP_10GE_PORT(unit, port))
                {
#if 0 // polar set for zyxel
                    if (ENABLED == enable)
                    {
                        RT_ERR_CHK(rtk_port_phyPolar_get(unit, port, &polarCtrl), ret);
                        /* must set phy polarity to normal in loopback mode, if default value is not normal */
                        polarCtrl.phy_polar_rx = PHY_POLARITY_NORMAL;
                        RT_ERR_CHK(rtk_port_phyPolar_set(unit, port, &polarCtrl), ret);
                    }
                    else
                    {
                        RT_ERR_CHK(rtk_port_phyPolar_get(unit, port, &polarCtrl), ret);
                        /* zyxel default value is inverse */
                        polarCtrl.phy_polar_rx = PHY_POLARITY_NORMAL;
                        RT_ERR_CHK(rtk_port_phyPolar_set(unit, port, &polarCtrl), ret);
                    }
#endif
                }

                /* loopback init */
                RT_ERR_CHK(rtk_port_phyLoopBackEnable_set(unit, port, enable), ret);
            }
        }
        /* wait port cfg */
        osal_time_udelay(100000);
    }

    return ret;
}

static int32 _loopback_test_speed_set(rtk_port_speed_t speed, rtk_port_duplex_t duplex, rtk_port_10gMedia_t media,\
        rtk_enable_t flowControl, rtk_portmask_t portmask)
{
    int32  ret = RT_ERR_OK;
    uint32 unit;

    rtk_port_t port;

    rtk_port_phy_ability_t  ability;

    osal_memset(&ability, 0, sizeof(rtk_port_phy_ability_t));

    if(flowControl == ENABLED)
    {
        ability.FC = ABILITY_BIT_ON;
    }
    if(speed == PORT_SPEED_10M && duplex == PORT_HALF_DUPLEX)
    {
        ability.Half_10 = ABILITY_BIT_ON;
    }
    if(speed == PORT_SPEED_10M && duplex == PORT_FULL_DUPLEX)
    {
        ability.Full_10 = ABILITY_BIT_ON;
    }
    if(speed == PORT_SPEED_100M && duplex == PORT_HALF_DUPLEX)
    {
        ability.Half_100 = ABILITY_BIT_ON;
    }
    if(speed == PORT_SPEED_100M && duplex == PORT_FULL_DUPLEX)
    {
        ability.Full_100 = ABILITY_BIT_ON;
    }
    if(speed == PORT_SPEED_1000M && duplex == PORT_FULL_DUPLEX)
    {
        ability.Full_1000 = ABILITY_BIT_ON;
    }


    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        /* switch port duplex and speed */
        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {

                if (!HWP_10GE_PORT(unit, port))
                {
                    // phy force can't change duplex mode, use auto nego instead
                    RT_ERR_CHK(rtk_port_phyAutoNegoAbility_set(unit, port, &ability), ret);
                }
                else
                {
                    RT_ERR_CHK(rtk_port_10gMedia_set(unit, port,\
                                media), ret);
                }
            }
        }
    }

    return ret;
}

static int32 _loopback_test_wait_linkup(rtk_portmask_t portmask)
{
    int32  ret = RT_ERR_OK;
    uint32 unit;
    uint32 retry = 0;

    rtk_port_t port;
    rtk_port_linkStatus_t link_status = 0;


    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        /* Check phy link up, due to phy occasional slow link up */
        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {
                retry = 0;
                /* phy link up could eat up tens of secs, polling the status */
                do
                {
                    RT_ERR_CHK(rtk_port_link_get(unit, port, &link_status), ret);
                    if (PORT_LINKUP != link_status)
                    {
                        osal_printf("  WARNING: unit: %u, port: %u, link: %d, retry: %u \n", unit, port, link_status, retry);
                        osal_time_sleep(1);
                    }
                    retry++;
                } while (PORT_LINKUP != link_status && retry < 5);

                if (PORT_LINKUP != link_status)
                    osal_printf("  ERROR: unit: %u, port: %u, link: %d, retry: %u \n", unit, port, link_status, retry);

                /* speed changing will lead to rx mac disacards in the training procedure */
                RT_ERR_CHK(rtk_stat_port_reset(unit, port), ret);

            }
        }
    }

    return ret;
}

static int32 _loopback_test_init_cfg(rtk_portmask_t portmask, rtk_mac_t dmac, rtk_mac_t smac)
{
    uint32 unit;
    int32 ret = RT_ERR_OK;
    rtk_port_t port;
    rtk_trap_userDefinedRma_t userRma;

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        /* Set port Enable */
        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {
                /* enable mac RX TX */
                RT_ERR_CHK(rtk_port_adminEnable_set(unit, port, ENABLED), ret);
            }
        }
        /* wait port enable */
        osal_time_sleep(3);

        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {
                RT_ERR_CHK(rtk_stat_port_reset(unit, port), ret);
                RT_ERR_CHK(rtk_vlan_portPvidMode_set(unit, port, INNER_VLAN, PBVLAN_MODE_ALL_PKT), ret);
                RT_ERR_CHK(rtk_vlan_portPvidMode_set(unit, port, OUTER_VLAN, PBVLAN_MODE_ALL_PKT), ret);
            }
        }

        /* reset CPU port MIB counter */
        RT_ERR_CHK(rtk_stat_port_reset(unit, HWP_CPU_MACID(unit)), ret);
        RT_ERR_CHK(rtk_stat_global_reset(unit), ret);

        /* User-defined rma - trap to CPU */
        osal_memset(&userRma, 0, sizeof(rtk_trap_userDefinedRma_t));
        osal_memcpy(userRma.mac.octet, dmac.octet, ETHER_ADDR_LEN);
        osal_memcpy(userRma.mac_min.octet, dmac.octet, ETHER_ADDR_LEN);
        osal_memcpy(userRma.mac_max.octet, dmac.octet, ETHER_ADDR_LEN);
        userRma.cmpType = RMA_CMP_TYPE_MAC;

        RT_ERR_CHK(rtk_trap_userDefineRmaEnable_set(unit, 0, ENABLED), ret);
        RT_ERR_CHK(rtk_trap_userDefineRma_set(unit, 0, &userRma), ret);
        if(HWP_IS_CPU_UNIT(unit))
            RT_ERR_CHK(rtk_trap_userDefineRmaAction_set(unit, 0, MGMT_ACTION_TRAP2CPU), ret);
        else
            RT_ERR_CHK(rtk_trap_userDefineRmaAction_set(unit, 0, MGMT_ACTION_TRAP2MASTERCPU), ret);

    }

    return ret;
}

static int32 _loopback_test_check(rtk_portmask_t portmask, uint32 pktCnt, uint32 *checkFailed)
{
    int32 ret = RT_ERR_OK;
    uint32 unit;
    uint64 mibVal = 0;
    uint64 nicRxPktCnt = 0;
    rtk_port_t port;

    *checkFailed = 0;

    HWP_UNIT_TRAVS_LOCAL(unit)
    {
        RTK_PORTMASK_SCAN(portmask, port)
        {
            if(!HWP_CASCADE_PORT(unit, port))
            {
                RT_ERR_CHK(rtk_stat_port_get(unit, port, IF_HC_IN_UCAST_PKTS_INDEX, &mibVal), ret);
                if (mibVal != pktCnt)
                {
                    osal_printf("  FAIL: unit %u, port %u, expected %u, mib pkts cnt %llu\n", unit, port, pktCnt, mibVal);
                    *checkFailed = 1;
                }
                if (port < LOOPBACK_TEST_PORT_MAX_NUM)
                    nicRxPktCnt = loopback_test_cpu_pkt_cnt[unit][port];
                else
                    nicRxPktCnt = 0;

                if (nicRxPktCnt != pktCnt)
                {
                    osal_printf("  FAIL: unit %u, port %u, expected %u, nic pkts cnt %llu\n", unit, port, pktCnt, nicRxPktCnt);
                    *checkFailed = 1;
                }

                /* check errors in mib counter */
                RT_ERR_CHK(rtk_stat_port_get(unit, port, RX_MAC_DISCARDS_INDEX, &mibVal), ret);
                if (mibVal != 0)
                {
                    osal_printf("  FAIL: port %u, rx mac discards %llu\n", port, mibVal);
                    *checkFailed = 1;
                }
                RT_ERR_CHK(rtk_stat_port_get(unit, port, RX_MAC_IPGSHORTDROP_INDEX, &mibVal), ret);
                if (mibVal != 0)
                {
                    osal_printf("  FAIL: port %u, rx mac ipg short %llu\n", port, mibVal);
                    *checkFailed = 1;
                }
            }
        }
    }

    return ret;
}



/* Function Name:
 *      do_rtdrv_ext_set_ctl
 * Description:
 *      This function is called whenever a process tries to do setsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_ext_set_ctl(struct sock *sk, int cmd, void *user_in, unsigned int len)
{
    void            *user = (char *)user_in + sizeof(rtdrv_msgHdr_t);
    int32   ret = RT_ERR_FAILED;
    int32   i = 0, payload_offset = 0;

    uint32  reg, field;
    uint32  value;
//    uint32  *ptr;
//    rtk_port_linkStatus_t   link_status = PORT_LINKDOWN;
//    rtk_port_t basePortId;
    rtdrv_ext_union_t   buf;

    if (user); /* to avoid compile warning */

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
    /** PORT **/
    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
    /** TRUNK **/
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
    /** SVLAN **/
    /** SWITCH **/
    /** NIC **/
    /** MPLS **/
    /** EEE **/

    /** IOL **/
    /** MODEL TEST **/
    /** packet generation */

    /**testing cases**/
#ifdef CONFIG_SDK_MODEL_MODE
        case RTDRV_EXT_MODEL_TEST_SET:
            copy_from_user(&buf.model_cfg, user, sizeof(rtdrv_ext_modelCfg_t));
            vmac_setCaredICType(CARE_TYPE_REAL);
            ret = tc_exec(buf.model_cfg.startID, buf.model_cfg.endID);
            break;

        case RTDRV_EXT_MODEL_TARGET_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_ext_unitCfg_t));
            vmac_setCaredICType(CARE_TYPE_REAL);
            ret = vmac_setTarget(buf.unit_cfg.data);
            break;

        case RTDRV_EXT_MODEL_REG_ACCESS_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_ext_unitCfg_t));
            vmac_setCaredICType(CARE_TYPE_REAL);
            vmac_setRegAccessType(buf.unit_cfg.data);
            ret = RT_ERR_OK;
            break;
#endif

#if EXT_PKTGEN_EN
     /*** packet generation ***/
        case RTDRV_EXT_PKTGEN_TX_CMD_SET:
            {
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

                ret = reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr, LONGAN_GRP_TX_CMDf, &buf.pktgen_cfg.value);
            }
            break;
        case RTDRV_EXT_PKTGEN_STATE_SET:
            {
                uint32 index0, index1, enable;
                rtk_port_t port;
                rtk_portmask_t pmsk;

                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                ret = reg_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr,
                        LONGAN_SPG_MODEf, &enable);
                ret += reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr,
                        LONGAN_SPG_MODEf, &buf.pktgen_cfg.enable);

                //set the port stream index
                if (ENABLED == buf.pktgen_cfg.enable && DISABLED == enable)
                {
                    pmsk.bits[0] = 0x0fffffff;
                    pmsk.bits[1] = 0;
                    RTK_PORTMASK_SCAN(pmsk, port)
                    {
                        index0 = page_base + (port * 2 + 0) * PKT_STREAM_PAGE_NUM;
                        index1 = page_base + (port * 2 + 1) * PKT_STREAM_PAGE_NUM;
                        ret += reg_array_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_INDEX_CTRL0r,
                                    port, REG_ARRAY_INDEX_NONE, LONGAN_PORT0_S1_INDEXf, &index1);
                        ret += reg_array_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_INDEX_CTRL0r,
                                    port, REG_ARRAY_INDEX_NONE, LONGAN_PORT0_S0_INDEXf, &index0);
                   }
                }
            }
            break;
        case RTDRV_EXT_PKTGEN_DBG_ACC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_write(buf.pktgen_cfg.unit, LONGAN_PKB_ACC_DEBUG_CTRLr,
                    LONGAN_DBG_ACC_PKB_ENf, &buf.pktgen_cfg.enable);
            break;
        case RTDRV_EXT_PKTGEN_IPG_COMP_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_write(buf.pktgen_cfg.unit, LONGAN_MAC_L2_GLOBAL_CTRL0r,
                    LONGAN_IPG_1G_100M_10M_COMPS_ENf, &buf.pktgen_cfg.enable);
            ret += reg_field_write(buf.pktgen_cfg.unit, LONGAN_MAC_L2_GLOBAL_CTRL0r,
                    LONGAN_IPG_2P5G_COMPS_ENf, &buf.pktgen_cfg.enable);
            ret += reg_field_write(buf.pktgen_cfg.unit, LONGAN_MAC_L2_GLOBAL_CTRL0r,
                    LONGAN_IPG_10G_COMPS_ENf, &buf.pktgen_cfg.enable);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STATE_SET:
            {
                uint32 value = 0;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

                /* MUST set TX_CMD to NOP before change GRP_TX_PORT. Changing GRP_TX_PORT will run current TX_CMD */
                value = RTDRV_EXT_SPG_TXCMD_NOP;
                ret = reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr, LONGAN_GRP_TX_CMDf, &value);
                /* read current GRP_TX_PORT */
                ret += reg_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_TX_GRP_CTRLr,
                        LONGAN_GRP_TX_PORTf, &value);

                if (ENABLED == buf.pktgen_cfg.enable)
                    value = (1 << buf.pktgen_cfg.port) | value;
                else
                    value = (~(1 << buf.pktgen_cfg.port)) & value;
                ret += reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_TX_GRP_CTRLr,
                        LONGAN_GRP_TX_PORTf, &value);
            }
            break;
        case RTDRV_EXT_PKTGEN_PORT_IPG_LEN_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            buf.pktgen_cfg.value = buf.pktgen_cfg.value & 0xFFFFF;
            ret = reg_array_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_IPG_CTRLr,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_IPG_LENf, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_PKT_CNT_Hr,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_PKT_CNT_HIGHf, &buf.pktgen_cfg.pktlen_end);
            ret += reg_array_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_PKT_CNT_Lr,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_PKT_CNT_LOWf, &buf.pktgen_cfg.pktlen_start);
            break;
        case RTDRV_EXT_PKTGEN_PORT_TXCMD_SET:
            {
                uint32 myPort = 0;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

                /* MUST set TX_CMD to NOP before change GRP_TX_PORT. Changing GRP_TX_PORT will run current TX_CMD */
                value = RTDRV_EXT_SPG_TXCMD_NOP;
                ret = reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr, LONGAN_GRP_TX_CMDf, &value);
                myPort = (0x1 << buf.pktgen_cfg.port);
                ret += reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_TX_GRP_CTRLr, LONGAN_GRP_TX_PORTf, &myPort);
                ret += reg_field_write(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr, LONGAN_GRP_TX_CMDf, &buf.pktgen_cfg.value);
            }
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_BADCRC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_BAD_CRC_EN_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_BAD_CRC_EN_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.enable);

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_DA_MOD_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_DA_MOD_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_SA_MOD_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_SA_MOD_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_LEN_TYPE_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_LEN_TYPE_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_CONTENT_OFFSET_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_CONTENT_OFFSET_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_MODE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_CONTENT_MOD_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_CONTENT_MOD_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_REPEAT_CONTENT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL4r;
                field = LONGAN_STREAM_REPEAT_CONTENT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL4r;
                field = LONGAN_STREAM_REPEAT_CONTENT_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL1r;
                field = LONGAN_STREAM_PKT_CNT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL1r;
                field = LONGAN_STREAM_PKT_CNT_1f;
            }

            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_START_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_START_1f;
            }
            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.pktlen_start);
            if (RT_ERR_OK != ret)
                goto FAIL_EXIT;

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_END_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_END_1f;
            }
            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.pktlen_end);
            if (RT_ERR_OK != ret)
                goto FAIL_EXIT;

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_LEN_TYPE_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_LEN_TYPE_1f;
            }
            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            /* if frame len is not fixed, use end len as pktLen */
            if (0 != buf.pktgen_cfg.value)
                pktLen[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx] = buf.pktgen_cfg.pktlen_end;
            else
                pktLen[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx] = buf.pktgen_cfg.pktlen_start;
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL3r;
                field = LONGAN_STREAM_SA_REPEAT_CNT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL3r;
                field = LONGAN_STREAM_SA_REPEAT_CNT_1f;
            }
            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL3r;
                field = LONGAN_STREAM_DA_REPEAT_CNT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL3r;
                field = LONGAN_STREAM_DA_REPEAT_CNT_1f;
            }
            ret = reg_array_field_write(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_SA_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&pDiagExtPacketHdr[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)][6], buf.pktgen_cfg.sa.octet, ETHER_ADDR_LEN);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_DA_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&pDiagExtPacketHdr[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)][0], buf.pktgen_cfg.da.octet, ETHER_ADDR_LEN);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ETHTYPE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&pDiagExtPacketHdr[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)][12], &buf.pktgen_cfg.etherType, 2);
            ret = RT_ERR_OK;
            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ETHTYPE_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            include_ethtype_header[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)] = buf.pktgen_cfg.enable;
            ret = RT_ERR_OK;
            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ITAG_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&iTag[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)], &buf.pktgen_cfg.vlanHdr, 4);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ITAG_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            include_vlan_header[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)] = buf.pktgen_cfg.enable;
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&oTag[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)], &buf.pktgen_cfg.vlanHdr, 4);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            include_svlan_header[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)] = buf.pktgen_cfg.enable;
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_TYPE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            spg_payload_type[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)] = buf.pktgen_cfg.patternType;
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_PATTERN_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            spg_payload_pattern[buf.pktgen_cfg.port][PKT_STREAM_IDX(buf.pktgen_cfg.stream_idx)] = buf.pktgen_cfg.pattern;
            ret = RT_ERR_OK;

            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_OFFSET_PAYLOAD_SET:
            {
                uint8 pktbuf[4];
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                _uint2buf(buf.pktgen_cfg.pattern, pktbuf);
                ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + buf.pktgen_cfg.stream_idx),
                         buf.pktgen_cfg.len, 4, pktbuf);
            }
            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_OFFSET_PAYLOAD8B_SET:
            {
                uint8 pktbuf[8];
                int32 i;
                uint32 value;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                for (i = 0; i < 2; i++)
                {
                    value = (uint32)(buf.pktgen_cfg.pattern8Btye[0] >> (32*(1-i)) & 0xffffffff);
                    _uint2buf(value, &pktbuf[4*i]);
                }
                ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + buf.pktgen_cfg.stream_idx),
                         buf.pktgen_cfg.len, 8, pktbuf);
            }
            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_OFFSET_PAYLOAD16B_SET:
            {
                uint8 pktbuf[16];
                int32 i, j;
                uint32 value;
                uint64 pattern;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                for (j = 0; j < 2; j++)
                {
                    pattern = buf.pktgen_cfg.pattern8Btye[j];
                    for (i = 0; i < 2; i++)
                    {
                        value = (uint32)(pattern >> (32*(1-i)) & 0xffffffff);
                        _uint2buf(value, &pktbuf[8*j + 4*i]);
                    }
                }
                ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + buf.pktgen_cfg.stream_idx),
                         buf.pktgen_cfg.len, 16, pktbuf);
            }
            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_OFFSET_PAYLOAD32B_SET:
            {
                uint8 pktbuf[32];
                int32 i, j;
                uint32 value;
                uint64 pattern;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                for (j = 0; j < 4; j++)
                {
                    pattern = buf.pktgen_cfg.pattern8Btye[j];
                    for (i = 0; i < 2; i++)
                    {
                        value = (uint32)(pattern >> (32*(1-i)) & 0xffffffff);
                        _uint2buf(value, &pktbuf[8*j + 4*i]);
                    }
                }
                ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + buf.pktgen_cfg.stream_idx),
                         buf.pktgen_cfg.len, 32, pktbuf);
            }
            break;

        case RTDRV_EXT_PKTGEN_PORT_STREAM_PAYLOAD_SET:
            {
                rtdrv_ext_pktGenCfg_t        *pUser = user;
                uint8 *pBuf;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                pBuf = osal_alloc(buf.pktgen_cfg.len);
                if (NULL == pBuf)
                {
                    ret = RT_ERR_FAILED;
                }
                else
                {
                    copy_from_user(pBuf, pUser->pdata, buf.pktgen_cfg.len);
                    ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + buf.pktgen_cfg.stream_idx),
                             buf.pktgen_cfg.value, buf.pktgen_cfg.len, pBuf);  //buf.pktgen_cfg.value is offset
                    osal_free(pBuf);
                }
            }
            break;

        case RTDRV_EXT_PKTGEN_PORT_TX:
            {
                uint32 txLen = 0;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

                /* Tx two stream */
               for (i = 0; i < PKTGEN_PORT_STREAM_NUM; i++)
               {
                    // if the stream not send, set the len = 0
                    if (0 == pktLen[buf.pktgen_cfg.port][i])
                        continue;

                    _pkt_buf_alloc(&pDiagExtPacket, PKTGEN_MAX_LEN);
                    osal_memset(pDiagExtPacket->data, 0, pDiagExtPacket->length);

                    osal_memcpy(pDiagExtPacket->data, &pDiagExtPacketHdr[buf.pktgen_cfg.port][i][0], 12);
                    payload_offset = 12;

                    if (include_svlan_header[buf.pktgen_cfg.port][i])
                    {
                        osal_memcpy(pDiagExtPacket->data + payload_offset, &oTag[buf.pktgen_cfg.port][i], 4);
                        payload_offset += 4;
                    }

                    if (include_vlan_header[buf.pktgen_cfg.port][i])
                    {
                        osal_memcpy(pDiagExtPacket->data + payload_offset, &iTag[buf.pktgen_cfg.port][i], 4);
                        payload_offset += 4;
                    }

                    if (include_ethtype_header[buf.pktgen_cfg.port][i])
                    {
                        osal_memcpy(pDiagExtPacket->data + payload_offset, &pDiagExtPacketHdr[buf.pktgen_cfg.port][i][12], 2);
                        payload_offset += 2;
                    }

                    txLen = pktLen[buf.pktgen_cfg.port][i];
                    if (pktLen[buf.pktgen_cfg.port][i] > PKTGEN_MAX_LEN)
                        txLen = PKTGEN_MAX_LEN;

                    _pkt_pattern_generate(buf.pktgen_cfg.port, i, pDiagExtPacket->data, payload_offset, txLen-payload_offset,
                        spg_payload_type[buf.pktgen_cfg.port][i]);
                    ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + i), 0, txLen, pDiagExtPacket->data);

                    _pkt_buf_free(pDiagExtPacket);
                }
            }
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_DYING_GASP:
                {
                    uint32 txLen = 0;
                    _pkt_buf_alloc(&pDiagExtPacket, PKTGEN_MAX_LEN);
                    osal_memset(pDiagExtPacket->data, 0, pDiagExtPacket->length);

                    osal_memcpy(pDiagExtPacket->data, &pDiagExtPacketHdr[buf.pktgen_cfg.port][i][0], 12);
                    payload_offset = 12;

                     /* Set OAM header */
                    pDiagExtPacket->data[12] = 0x88;
                    pDiagExtPacket->data[13] = 0x09;
                    pDiagExtPacket->data[14] = 0x03;
                    pDiagExtPacket->data[15] = 0x00;
                    pDiagExtPacket->data[16] = 0x52;
                    pDiagExtPacket->data[17] = 0x00;
                    payload_offset = 18;

                    txLen = pktLen[buf.pktgen_cfg.port][0];
                    if (pktLen[buf.pktgen_cfg.port][0] > PKTGEN_MAX_LEN)
                        txLen = PKTGEN_MAX_LEN;

                    _pkt_pattern_generate(buf.pktgen_cfg.port, 0, pDiagExtPacket->data, payload_offset, txLen-payload_offset,
                        spg_payload_type[buf.pktgen_cfg.port][0]);
                    ret = pkt_stream_write(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + 0), 0, txLen, pDiagExtPacket->data);

                    _pkt_buf_free(pDiagExtPacket);
                }
            ret = RT_ERR_OK;
            break;

        case RTDRV_EXT_PKTGEN_PACKET_BUFFER_TEST:
                {
                    #if EXT_SRAM_PKT_TEST
                    rtdrv_ext_bufferTest_t  testItem;
                    copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                    testItem = buf.pktgen_cfg.value;
                    pktbuf_rw_test(buf.pktgen_cfg.unit, testItem);
                    #endif
                }
            ret = RT_ERR_OK;
            break;

       case RTDRV_EXT_PKTGEN_PACKET_BUFFER_SECT_SET:
            {
                #if EXT_SRAM_PKT_TEST
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                ret = pkt_strem_buffer_section_set(buf.pktgen_cfg.unit, buf.pktgen_cfg.value);
                #endif
            }
            break;
#endif


        case RTDRV_EXT_PORT_PHYCALI_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));

            {
                rtk_port_t port;

                //REG32(0xBB000208) = 0xFFFFFF;
                //REG32(0xBB00020C) = 0;

                _lan_switch_select(buf.port_cfg.unit, SWITCH_MODE_INIT);
#if 0
                drv_gpio_pin_init(buf.port_cfg.unit, 22, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);

                drv_gpio_dataBit_set(buf.port_cfg.unit, 22, 0); // reset PHY
            osal_printf("%s %d REG32(0xb8003308): 0x%x REG32(0xb800330C): 0x%x\n", __FUNCTION__, __LINE__,
                    REG32(0xb8003308), REG32(0xb800330C));


                osal_time_mdelay(500);
                drv_gpio_dataBit_set(buf.port_cfg.unit, 22, 1); // disable reset PHY

            osal_printf("%s %d REG32(0xb8003308): 0x%x REG32(0xb800330C): 0x%x\n", __FUNCTION__, __LINE__,
                    REG32(0xb8003308), REG32(0xb800330C));

                osal_printf("buf.port_cfg.portmask %x %x\n", buf.port_cfg.portmask.bits[1], buf.port_cfg.portmask.bits[0]);

                osal_time_mdelay(4000);
#endif
                for(port = 0; port <64; port ++)
                {
                    if(RTK_PORTMASK_IS_PORT_SET(buf.port_cfg.portmask, port))
                    {
                        osal_printf("port %d set\n", port);
                        ret = _phy_en(buf.port_cfg.unit, port);
                        if(ret != RT_ERR_OK)
                            break;
                    }
                }

                phy_field_write(0, 0, 0xa40, 0, 11, 11, 0);

                osal_printf(" Waiting 5 seconds for N-Way \n");

                osal_time_mdelay(5000);

                ret = _adc_ofs_cal(buf.port_cfg.unit, buf.port_cfg.portmask);
                if(ret != RT_ERR_OK)
                    break;
                ret = _rc_cal(buf.port_cfg.unit, 1, 1, buf.port_cfg.portmask);
                if(ret != RT_ERR_OK)
                    break;
                ret = _amp_cal_ext(buf.port_cfg.unit, buf.port_cfg.portmask);
                if(ret != RT_ERR_OK)
                    break;

                ret = _new_r_cal(buf.port_cfg.unit, buf.port_cfg.portmask);
                if(ret != RT_ERR_OK)
                    break;
                ret = _amp_chk_ext(buf.port_cfg.unit, buf.port_cfg.portmask);
                if(ret != RT_ERR_OK)
                    break;
                ret = _cal_para_rd(buf.port_cfg.unit, buf.port_cfg.portmask, "");
                if(ret != RT_ERR_OK)
                    break;

                if(buf.port_cfg.writeEfuse)
                {
                    _cal_para_efuse_write(buf.port_cfg.unit, buf.port_cfg.portmask);
                }

            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_INIT_CFG_SET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                ret = _loopback_test_init_cfg(
                        buf.loopbackTest_cfg.portmask,
                        buf.loopbackTest_cfg.da,
                        buf.loopbackTest_cfg.sa
                        );
            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_SPEED_CHANGE_SET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                ret = _loopback_test_speed_set(
                        buf.loopbackTest_cfg.speed,
                        buf.loopbackTest_cfg.duplex,
                        buf.loopbackTest_cfg.media,
                        buf.loopbackTest_cfg.flowControl,
                        buf.loopbackTest_cfg.portmask
                        );
                if(ret != RT_ERR_OK)
                    break;

                if(buf.loopbackTest_cfg.waitLinkup)
                {
                    /* wait phy auto nego done */
                    osal_time_sleep(3);
                    ret = _loopback_test_wait_linkup(buf.loopbackTest_cfg.portmask);
                }
            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_INT_LOOPBACK_SET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                ret = _loopback_test_int_loopback_cfg(
                        buf.loopbackTest_cfg.portmask,
                        buf.loopbackTest_cfg.phyLoopback
                        );
            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_PKT_TX_SET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                ret = _loopback_test_packet_send(
                        buf.loopbackTest_cfg.da,
                        buf.loopbackTest_cfg.sa,
                        buf.loopbackTest_cfg.pktNum,
                        buf.loopbackTest_cfg.len,
                        buf.loopbackTest_cfg.portmask,
                        buf.loopbackTest_cfg.gapTime
                        );
            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_RX_CB_REG_SET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                osal_memset(loopback_test_cpu_pkt_cnt, 0, sizeof(loopback_test_cpu_pkt_cnt));
                ret = drv_nic_rx_register(buf.loopbackTest_cfg.unit, buf.loopbackTest_cfg.value, _loopback_test_rx_callback, NULL, 0);
                /* ugly workaround if l2notify register priority 0 */
                if (RT_ERR_FAILED == ret)
                {
                    drv_nic_rx_unregister(buf.loopbackTest_cfg.unit, buf.loopbackTest_cfg.value, drv_l2ntfy_pkt_handler);
                    ret = drv_nic_rx_register(buf.loopbackTest_cfg.unit, buf.loopbackTest_cfg.value, _loopback_test_rx_callback, NULL, 0);
                }
            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_RX_CB_UNREG_SET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                rtk_l2ntfy_dst_t now_dst;
                ret = drv_nic_rx_unregister(buf.loopbackTest_cfg.unit, buf.loopbackTest_cfg.value, _loopback_test_rx_callback);
                /* ugly workaround if l2notify register priority 0 */
                if (RT_ERR_OK == ret)
                {
                    /* recover l2notify rx register */
                    drv_l2ntfy_dst_get(buf.loopbackTest_cfg.unit, &now_dst);
                    if((L2NTFY_DST_PKT_TO_LOCAL == now_dst) || (L2NTFY_DST_PKT_TO_MASTER == now_dst))
                        ret = drv_nic_rx_register(buf.loopbackTest_cfg.unit, L2NTFY_RX_HANDLER_PRIORITY, drv_l2ntfy_pkt_handler, NULL, 0);
                }
            }
            break;

        case RTDRV_EXT_PORT_PHYCALI_RCCAL_NYQ_THD_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));

            if(1 == buf.port_cfg.isShort)
            {
                rc_thd_target[0][0] = buf.port_cfg.thd_phy0;
                rc_thd_target[0][1] = buf.port_cfg.thd_phy1;
                rc_thd_target[0][2] = buf.port_cfg.thd_phy2;
                rc_thd_target[0][3] = buf.port_cfg.thd_phy3;
                rc_thd_target[0][4] = buf.port_cfg.thd_phy4;
                rc_thd_target[0][5] = buf.port_cfg.thd_phy5;
                rc_thd_target[0][6] = buf.port_cfg.thd_phy6;
                rc_thd_target[0][7] = buf.port_cfg.thd_phy7;
            }
            else
            {
                rc_thd_target[1][0] = buf.port_cfg.thd_phy0;
                rc_thd_target[1][1] = buf.port_cfg.thd_phy1;
                rc_thd_target[1][2] = buf.port_cfg.thd_phy2;
                rc_thd_target[1][3] = buf.port_cfg.thd_phy3;
                rc_thd_target[1][4] = buf.port_cfg.thd_phy4;
                rc_thd_target[1][5] = buf.port_cfg.thd_phy5;
                rc_thd_target[1][6] = buf.port_cfg.thd_phy6;
                rc_thd_target[1][7] = buf.port_cfg.thd_phy7;
            }
            break;

        case RTDRV_EXT_PORT_PHYCALI_AMP_CAL_OFFSET_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ampOfsA[buf.port_cfg.port] = buf.port_cfg.ofs_chA;
            ampOfsB[buf.port_cfg.port] = buf.port_cfg.ofs_chB;
            ampOfsC[buf.port_cfg.port] = buf.port_cfg.ofs_chC;
            ampOfsD[buf.port_cfg.port] = buf.port_cfg.ofs_chD;

            break;
        case RTDRV_EXT_PORT_PHYCALI_R_CAL_OFFSET_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            R_OFS_A[buf.port_cfg.port] = buf.port_cfg.ofs_chA;
            R_OFS_B[buf.port_cfg.port] = buf.port_cfg.ofs_chB;
            R_OFS_C[buf.port_cfg.port] = buf.port_cfg.ofs_chC;
            R_OFS_D[buf.port_cfg.port] = buf.port_cfg.ofs_chD;
            break;

        case RTDRV_EXT_SDK_TC_RESET:    /* sdk reset tc - reset DUT to default configuration */
            copy_from_user(&buf.sdk_cfg, user, sizeof(rtdrv_ext_sdkCfg_t));
            ret = _sdk_tc_reset(buf.sdk_cfg.unit);
            break;

        case RTDRV_EXT_REG_DUMP_SET:    /* dump all reg */
            copy_from_user(&buf.sdk_cfg, user, sizeof(rtdrv_ext_sdkCfg_t));
            {
              #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
                uint32 val;
                uint32 unit;
                uint32 reg_idx;
                uint32 field_idx;
                uint32 cpu_port;
                uint32 rtk_reg_num;
                rtk_regField_t *p_fields;
                rtk_reg_t **pReg_list;

                unit = buf.sdk_cfg.unit;
                rtk_reg_num = HAL_GET_MAX_REG_IDX(unit);
                pReg_list = HAL_GET_MAC_DRIVER(unit)->pReg_list;
                cpu_port = HWP_CPU_MACID(unit);

                for(reg_idx = 0; reg_idx < rtk_reg_num; reg_idx++)
                {
                    p_fields = pReg_list[reg_idx]->pFields;
                    osal_printf("%s\n", pReg_list[reg_idx]->name);

                    for(field_idx = 0; field_idx < pReg_list[reg_idx]->field_num; field_idx++)
                    {
                        ret = reg_field_read(unit, reg_idx, p_fields[field_idx].name, &val);
                        if(RT_ERR_OK == ret)
                            osal_printf("  %s - %s = 0x%08x\n", pReg_list[reg_idx]->name, \
                                    p_fields[field_idx].field_name, val);
                        else
                            osal_printf("  %s - %s = fail\n", pReg_list[reg_idx]->name, \
                                    p_fields[field_idx].field_name);
                    }

                    if(pReg_list[reg_idx]->hport == cpu_port)
                    {
                        for(field_idx = 0; field_idx < pReg_list[reg_idx]->field_num; field_idx++)
                        {
                            ret = reg_array_field_read(unit, reg_idx, cpu_port, REG_ARRAY_INDEX_NONE, \
                                    p_fields[field_idx].name, &val);
                            if(RT_ERR_OK == ret)
                                osal_printf("  %s - %s[cpuport] = 0x%08x\n", pReg_list[reg_idx]->name, \
                                        p_fields[field_idx].field_name, val);
                            else
                                osal_printf("  %s - %s[cpuport] = fail\n", pReg_list[reg_idx]->name, \
                                        p_fields[field_idx].field_name);
                        }
                    }
                }
              #else
                ret = RT_ERR_OK;
              #endif
            }
            break;

        default:
            break;
    }

FAIL_EXIT:
    copy_to_user(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    return RT_ERR_OK;
}

/* Function Name:
 *      do_rtdrv_ext_get_ctl
 * Description:
 *      This function is called whenever a process tries to do getsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 * Output:
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_ext_get_ctl(struct sock *sk, int cmd, void *user_in, int *len)
{
    void                *user = (char *)user_in + sizeof(rtdrv_msgHdr_t);
    int32   ret = RT_ERR_FAILED;
    rtdrv_ext_union_t   buf;
    uint32  reg, field;
    #ifdef CONFIG_SDK_MODEL_MODE
    uint32  value;
    #endif

    if (user); /* to avoid compile warning */

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
    /** PORT **/

    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
    /** TRUNK **/
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
    /** SVLAN **/
    /** SWITCH **/

    /** NIC **/
    /** MPLS **/
    /** EEE **/
    /** IOL **/
    /** MODEL TEST **/
#ifdef CONFIG_SDK_MODEL_MODE
        case RTDRV_EXT_MODEL_TARGET_GET:
            ret = vmac_getTarget(&value);
            buf.unit_cfg.data = value;
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_ext_unitCfg_t));
            break;
        case RTDRV_EXT_MODEL_REG_ACCESS_GET:
             vmac_getRegAccessType(&value);
            buf.unit_cfg.data = value;
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_ext_unitCfg_t));
            ret = RT_ERR_OK;
            break;
#endif

        /*** packet generation ***/
        case RTDRV_EXT_PKTGEN_TX_CMD_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr,
                    LONGAN_GRP_TX_CMDf, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_GLB_CTRLr,
                    LONGAN_SPG_MODEf, &buf.pktgen_cfg.enable);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_DBG_ACC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, LONGAN_PKB_ACC_DEBUG_CTRLr,
                    LONGAN_DBG_ACC_PKB_ENf,&buf.pktgen_cfg.enable);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_TX_GRP_CTRLr,
                    LONGAN_GRP_TX_PORTf,&buf.pktgen_cfg.enable);
            if (buf.pktgen_cfg.enable & ((0x1) << buf.pktgen_cfg.port))
                buf.pktgen_cfg.enable = ENABLED;
            else
                buf.pktgen_cfg.enable = DISABLED;
            #if 0
            ret = reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_TX_GRP_CTRLr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_GRP_TX_PORTf,&buf.pktgen_cfg.enable);
            #endif
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_DONE_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_GLOBAL_STSr,
                    LONGAN_TX_DONE_PORTf,&buf.pktgen_cfg.value);
            if (buf.pktgen_cfg.value & ((0x1) << buf.pktgen_cfg.port))
                buf.pktgen_cfg.value = 1;
            else
                buf.pktgen_cfg.value = 0;

            #if 0
            ret = reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_GLOBAL_STSr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_TX_DONE_PORTf,&buf.pktgen_cfg.value);
            #endif
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_IPG_LEN_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_IPG_CTRLr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_IPG_LENf,&buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_PKT_CNT_Hr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_PKT_CNT_HIGHf,&buf.pktgen_cfg.pktlen_end);
            ret += reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_PKT_CNT_Lr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_PKT_CNT_LOWf,&buf.pktgen_cfg.pktlen_start);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_PKT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_PKT_CNT_DBG_Hr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_PKT_CNT_DBG_HIGHf,&buf.pktgen_cfg.pktlen_end);
            ret += reg_array_field_read(buf.pktgen_cfg.unit, LONGAN_SPG_PORT_PKT_CNT_DBG_Lr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, LONGAN_PKT_CNT_DBG_LOWf,&buf.pktgen_cfg.pktlen_start);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_BADCRC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_BAD_CRC_EN_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_BAD_CRC_EN_1f;
            }
            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.enable);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_SA_MOD_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_SA_MOD_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_DA_MOD_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_DA_MOD_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_LEN_TYPE_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_LEN_TYPE_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_CONTENT_OFFSET_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_CONTENT_OFFSET_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_MODE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL0r;
                field = LONGAN_STREAM_CONTENT_MOD_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL0r;
                field = LONGAN_STREAM_CONTENT_MOD_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_REPEAT_CONTENT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL4r;
                field = LONGAN_STREAM_REPEAT_CONTENT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL4r;
                field = LONGAN_STREAM_REPEAT_CONTENT_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL1r;
                field = LONGAN_STREAM_PKT_CNT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL1r;
                field = LONGAN_STREAM_PKT_CNT_1f;
            }

            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_START_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_START_1f;
            }
            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.pktlen_start);
            if (RT_ERR_OK != ret)
                goto FAIL_EXIT;

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_END_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL2r;
                field = LONGAN_STREAM_LEN_RNG_END_1f;
            }
            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.pktlen_end);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL3r;
                field = LONGAN_STREAM_SA_REPEAT_CNT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL3r;
                field = LONGAN_STREAM_SA_REPEAT_CNT_1f;
            }
            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
            {
                ret = RT_ERR_OUT_OF_RANGE;
                goto FAIL_EXIT;
            }

            if (0 == buf.pktgen_cfg.stream_idx)
            {
                reg = LONGAN_SPG_PORT_STREAM0_CTRL3r;
                field = LONGAN_STREAM_DA_REPEAT_CNT_0f;
            }
            else
            {
                reg = LONGAN_SPG_PORT_STREAM1_CTRL3r;
                field = LONGAN_STREAM_DA_REPEAT_CNT_1f;
            }
            ret = reg_array_field_read(buf.pktgen_cfg.unit, reg,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, field, &buf.pktgen_cfg.value);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_STREAM_GET:
           {
                uint8 *pPktBuf = NULL;
                int32 i;
                copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
                if (buf.pktgen_cfg.stream_idx >= PKTGEN_PORT_STREAM_NUM)
                {
                    ret = RT_ERR_OUT_OF_RANGE;
                    goto FAIL_EXIT;
                }

                pPktBuf = osal_alloc(buf.pktgen_cfg.len);
                if (NULL==pPktBuf)
                {
                    osal_printf("Alloc packet buffer fail !\n");
                    return RT_ERR_FAILED;
                }
                ret = pkt_stream_read(buf.pktgen_cfg.unit, (buf.pktgen_cfg.port * 2 + buf.pktgen_cfg.stream_idx), 0, buf.pktgen_cfg.len, pPktBuf);
                osal_printf("Stream index %d data:\n", buf.pktgen_cfg.stream_idx);
                for (i = 0; i < buf.pktgen_cfg.len; i++)
                {
                    if (0 == i%16)
                        osal_printf("0x%04x ", i);

                    osal_printf("%02x ", pPktBuf[i]);

                    if ((i+1)%16 == 0)
                        osal_printf("\n");

                    if ((i+1)%256 == 0)
                        osal_printf("\n");
                }
                osal_printf("\n\n");
                osal_free(pPktBuf);
            }
            break;
        case RTDRV_EXT_LOOPBACK_TEST_CHK_GET:
            copy_from_user(&buf.loopbackTest_cfg, user, sizeof(rtdrv_ext_loopbackTestCfg_t));
            {
                ret = _loopback_test_check(
                        buf.loopbackTest_cfg.portmask,
                        buf.loopbackTest_cfg.pktNum,
                        &(buf.loopbackTest_cfg.value)
                        );
            }
            copy_to_user(user, &buf.loopbackTest_cfg, sizeof(rtdrv_ext_loopbackTestCfg_t));
            break;
        case RTDRV_EXT_PORT_10G_ABILITY_PORT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if (HWP_10GE_PORT(buf.pktgen_cfg.unit, buf.pktgen_cfg.port))
                buf.pktgen_cfg.value = 1;
            else
                buf.pktgen_cfg.value = 0;
            ret = RT_ERR_OK;
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;

        case RTDRV_EXT_PORT_PHYCALI_GET:
            {
                rtk_port_t port;

                copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));

                for(port = 0; port < 8; port ++)
                {
                    osal_printf("RCCAL_NYQ_THD[S][%d] = %d\n", port, rc_thd_target[0][port]);
                }

                for(port = 0; port < 8; port ++)
                {
                    osal_printf("RCCAL_NYQ_THD[L][%d] = %d\n", port, rc_thd_target[1][port]);
                }

                for(port = 0; port < 8; port ++)
                {
                    osal_printf("AMP_CAL_OFFSET[%d] = %d %d %d %d\n", port, ampOfsA[port],  ampOfsB[port],  ampOfsC[port],  ampOfsD[port]);
                }

                for(port = 0; port < 8; port ++)
                {
                    osal_printf("R_CAL_OFFSET[%d] = %d %d %d %d\n", port, R_OFS_A[port],  R_OFS_B[port],  R_OFS_C[port],  R_OFS_D[port]);
                }

                copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));

                ret = RT_ERR_OK;

                break;
            }
        default:
            break;
    }

FAIL_EXIT:
    copy_to_user(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    return 0;
}

struct nf_sockopt_ops rtdrv_ext_sockopts = {
    .pf         = PF_INET,
    .set_optmin = RTDRV_EXT_BASE_CTL,
    .set_optmax = RTDRV_EXT_SET_MAX+1,
    .set        = do_rtdrv_ext_set_ctl,
    .get_optmin = RTDRV_EXT_BASE_CTL,
    .get_optmax = RTDRV_EXT_GET_MAX+1,
    .get        = do_rtdrv_ext_get_ctl,
};
