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
#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <common/rt_error.h>
#include <common/debug/mem.h>
#include <osal/print.h>
#include <hal/mac/mem.h>
#include <ioal/mem32.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/watchdog/watchdog.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_9310.h>
#include <osal/memory.h>
#if (defined(CONFIG_SDK_DRIVER_TEST) || defined(CONFIG_SDK_DRIVER_TEST_MODULE))
#include <sdk/sdk_test.h>
#endif
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_rate.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#ifdef CONFIG_SDK_MODEL_MODE
#include <model_comm.h>
#include <tc.h>
#include <virtualmac/vmac_target.h>
#endif
#include <osal/time.h>
#include <rtk/trunk.h>
#include <private/drv/nic/nic_diag.h>
#include <hal/phy/phy_common.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <dal/rtrpc/rtrpc_msg.h>
/*
 * Symbol Definition
 */
#define EXT_PACKET_HDR_LEN  60
#define PKTGEN_STREAM_NUM   2

/*
 * Data Declaration
 */
drv_nic_pkt_t *pDiagExtPacket;
uint8 pDiagExtPacketHdr[52][PKTGEN_STREAM_NUM][EXT_PACKET_HDR_LEN];	/* enough to comprise ethernet, IP and TCP header */
uint8 include_vlan_header[52][PKTGEN_STREAM_NUM];
uint8 spg_payload_type[52][PKTGEN_STREAM_NUM];
uint32 spg_payload_pattern[52][PKTGEN_STREAM_NUM];
uint32 oTag[52][PKTGEN_STREAM_NUM];
static uint32 linkMon_wake_up;


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

static void
_nic_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    if (pPacket == NULL)
    {
        goto _exit;
    }
    osal_free(pCookie);
    osal_free(pPacket);

_exit:
    return;
}

#ifdef CONFIG_SDK_FPGA_PLATFORM
static int32
_fpga_info_get(uint32 unit, uint32 *pRtl, uint32 *pDate, uint32 *pTime, uint32 *pVersion)
{
    reg_array_field_read(unit, MANGO_ACL_RSVDr, REG_ARRAY_INDEX_NONE, 0, MANGO_DUMY_12f, pRtl);
    reg_array_field_read(unit, MANGO_ACL_RSVDr, REG_ARRAY_INDEX_NONE, 1, MANGO_DUMY_12f, pDate);
    reg_array_field_read(unit, MANGO_ACL_RSVDr, REG_ARRAY_INDEX_NONE, 2, MANGO_DUMY_12f, pTime);
    reg_array_field_read(unit, MANGO_ACL_RSVDr, REG_ARRAY_INDEX_NONE, 3, MANGO_DUMY_12f, pVersion);

    return RT_ERR_OK;
}


#define FPGA_REG_WRITE(_unit, _reg, _val)   \
do {                                    \
    uint32 __value = _val;              \
    uint32 __check;                     \
    reg_write(_unit, _reg, &__value);   \
    reg_read(_unit, _reg, &__check);    \
    if (__value != __check) osal_printf("write 0x%08X, read-back 0x%08X\n", __value, __check); \
} while (0)

#define FPGA_REG_FIELD_WRITE(_unit, _reg, _idx1, _idx2, _field, _val)   \
do {                                                                    \
    uint32 __value = _val;                                              \
    uint32 __check;                                                     \
    reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &__value); \
    reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &__check);  \
    if (__value != __check) osal_printf("write 0x%08X, read-back 0x%08X\n", __value, __check); \
} while (0)

#define FPGA_REG_SET(_unit, _reg, _idx1, _idx2, _val)   \
do {                                                        \
    uint32 __value = _val;                                  \
    uint32 __check;                                         \
    reg_array_write(_unit, _reg, _idx1, _idx2, &__value);   \
    reg_array_read(_unit, _reg, _idx1, _idx2, &__check);    \
    if (__value != __check) osal_printf("write 0x%08X, read-back 0x%08X\n", __value, __check); \
} while (0)

static int32
_fpga_init(uint32 unit, uint32 fpgaVer)
{
#if 0
uint32 i;
osal_printf("*********************_fpga_init\n");


// Flow Contrl
// for Jumbo mode
// set RX groups 3 of FC_PORT_LO_THR (ON, OFF) = (72, 35)
// set RX groups 3 of FC_PORT_FCOFF_LO_THR (ON, OFF) = (72, 35)
// set RX groups 3 of FC_PORT_GUAR_THR = (6)
for (i=3; i<=3; i++)
{
    FPGA_REG_SET(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, i, 0x480023);
    FPGA_REG_SET(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, i, 0x480023);
    FPGA_REG_SET(unit, MANGO_FC_PORT_GUAR_THRr, REG_ARRAY_INDEX_NONE, i, 0x23);
}

// Jumbo mode setting - Enable Jumbo mode, packet length threshold = 2000B, exit jumbo mode page threshold = 6 pages.
FPGA_REG_SET(unit, MANGO_FC_JUMBO_THR_ADJUSTr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x87D00006);


// set CPU port Tx/Rx enable
// sprintf 'reg set 0x7c00 0x3'
//FPGA_REG_SET(unit, MANGO_MAC_L2_PORT_CTRLr, 56, REG_ARRAY_INDEX_NONE, 0x3);
// CPU port Force link-up
// sprintf 'reg set 0xdf8 0x1AA01'
FPGA_REG_SET(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, 0x32A01);

FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 1);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 1);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 1);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 1);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 1);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, 0x2);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, 0);
FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_TX_PAUSE_ENf, 0);

// Force CPU port egress queue drop when congestion
// sprintf 'reg set 0xa9c8 0x1'
FPGA_REG_SET(unit, MANGO_FC_CPU_Q_EGR_FORCE_DROP_CTRLr, REG_ARRAY_INDEX_NONE, 0, 0x1);

// set ingress queue drop theshold of ingress queue 0~2 for all port (ON, OFF) = (220, 210)
FPGA_REG_SET(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, 0, 0xDC00D2);
FPGA_REG_SET(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, 1, 0xDC00D2);
FPGA_REG_SET(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, 2, 0xDC00D2);

// QoS Scheduling - Set queue empty threshold to be 0
FPGA_REG_SET(unit, MANGO_SCHED_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0);


osal_printf("*********************_fpga_init\n");

return RT_ERR_OK;

#else

    uint32 i, val, idx;
    rtk_port_t  port;

    /* CPU port simulation */
    if (fpgaVer == 9)
    {
        osal_printf("enable CPU port simulation\n");
        FPGA_REG_WRITE(unit, MANGO_GLB_DEBUG_SELECTr, 0x40000000);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_L2_GLOBAL_CTRL2r, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_EXT_CPU_ENf, 0x1);

        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_TX_PAUSE_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, 0x2);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_DUP_SELf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FEFI_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MSTR_SLV_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_EEE_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MEDIA_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 0x1);

        rtk_vlan_port_add(unit, 1, 56, TRUE);

        FPGA_REG_WRITE(unit, MANGO_FC_CPU_Q_EGR_FORCE_DROP_CTRLr, 0x0);

        return RT_ERR_OK;
    } else if (fpgaVer == 10)
    {
        osal_printf("disable CPU port simulation\n");
        FPGA_REG_WRITE(unit, MANGO_GLB_DEBUG_SELECTr, 0x0);

        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_TX_PAUSE_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, 0x2);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_DUP_SELf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FEFI_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MSTR_SLV_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_EEE_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MEDIA_ENf, 0x1);
        FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 0x1);

        rtk_vlan_port_del(unit, 1, 56);

        FPGA_REG_WRITE(unit, MANGO_FC_CPU_Q_EGR_FORCE_DROP_CTRLr, 0x1);

        return RT_ERR_OK;
    }

    osal_printf("Reset table (by using init reg)\n");
    FPGA_REG_FIELD_WRITE(unit, MANGO_MEM_IGR_INITr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_LINK_LIST_INITf, 0x1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MEM_ENCAP_INITr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_MEM_INITf, 0x1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MEM_EGR_INITr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_MEM_INITf, 0x1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MEM_MIB_INITr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_MEM_RSTf, 0x1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MEM_ACL_INITr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_MEM_INITf, 0x1);
    FPGA_REG_WRITE(unit, MANGO_MEM_ALE_INIT_0r, 0xffffffff);
    FPGA_REG_WRITE(unit, MANGO_MEM_ALE_INIT_1r, 0xffffffff);
    FPGA_REG_WRITE(unit, MANGO_MEM_ALE_INIT_2r, 0xffffffff);
    /* reset learning counter */
    val = 0;
    reg_field_write(unit, MANGO_L2_LRN_CONSTRT_CNTr, MANGO_LRN_CNTf, &val);
    HWP_ETHER_PORT_TRAVS(unit, port)
        reg_array_field_write(unit, MANGO_L2_LRN_PORT_CONSTRT_CNTr, port, REG_ARRAY_INDEX_NONE, MANGO_LRN_CNTf, &val);
    for (idx = 0; idx < HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit); idx++)
        reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, idx, MANGO_LRN_CNTf, &val);


    osal_printf("Reset extra HSA memory\n");
    FPGA_REG_FIELD_WRITE(unit, MANGO_MEM_EGR_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_LINK_INITf, 1);
    //osal_time_mdelay(100);  /* wait for 100mS */

    /* set EXT_CPU_EN enable, so that packet won't pile up and causing egress drop */
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_L2_GLOBAL_CTRL2r, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_EXT_CPU_ENf, 0x1);

    if (fpgaVer >= 1)
    {
        /* FE patch */
        osal_printf("FPGA patch (FE)\n");

        if (fpgaVer == 2)
        {
            osal_printf("System clock has been changed to 325Mhz\n");
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_L2_GLOBAL_CTRL2r, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_SYS_CLK_SELf, 0x1);

            // QoS T, B value (for FE version only)
            FPGA_REG_SET(unit, MANGO_IGBW_LB_CTRLr,         REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00350156);
            FPGA_REG_SET(unit, MANGO_EGBW_LB_CTRLr,         REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00350156);
            FPGA_REG_SET(unit, MANGO_EGBW_CPU_PPS_LB_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00013601);
            FPGA_REG_SET(unit, MANGO_WFQ_LB_CTRLr,          REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x080102ac);
            FPGA_REG_SET(unit, MANGO_STORM_LB_CTRLr,        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00350156);
            FPGA_REG_SET(unit, MANGO_STORM_LB_PPS_CTRLr,    REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x000004D1);
            FPGA_REG_SET(unit, MANGO_STORM_LB_PROTO_CTRLr,  REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x0009AF8E);
            FPGA_REG_SET(unit, MANGO_METER_BYTE_TB_CTRLr,   REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00350156);
            FPGA_REG_SET(unit, MANGO_METER_PKT_TB_CTRLr,    REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x01360001);
        }
        else
        {
            osal_printf("System clock has been changed to 175Mhz\n");
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_L2_GLOBAL_CTRL2r, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, MANGO_SYS_CLK_SELf, 0x2);

            // QoS T, B value (for FE version only)
            FPGA_REG_SET(unit, MANGO_IGBW_LB_CTRLr,         REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x003e02e7);
            FPGA_REG_SET(unit, MANGO_EGBW_LB_CTRLr,         REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x003e02e7);
            FPGA_REG_SET(unit, MANGO_EGBW_CPU_PPS_LB_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x0000a701);
            FPGA_REG_SET(unit, MANGO_WFQ_LB_CTRLr,          REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x08010744);
            FPGA_REG_SET(unit, MANGO_STORM_LB_CTRLr,        REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x003e02e7);
            FPGA_REG_SET(unit, MANGO_STORM_LB_PPS_CTRLr,    REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x000002a1);
            FPGA_REG_SET(unit, MANGO_STORM_LB_PROTO_CTRLr,  REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00053725);
            FPGA_REG_SET(unit, MANGO_METER_BYTE_TB_CTRLr,   REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x003e02e7);
            FPGA_REG_SET(unit, MANGO_METER_PKT_TB_CTRLr,    REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x00a70001);

            // Write Egress Port Burst Size
            for (i=0; i<=53; i++)
            {
                if ((4 <= i) && (i <= 51)) { continue; }
                FPGA_REG_FIELD_WRITE(unit, MANGO_EGBW_PORT_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, 0x1B58);
            }
        }

        // force port 0~3 FC_EN/HALF_DUPLEX/100M/LINK_UP
        // force port 1 FC_EN/HALF_DUPLEX/100M/LINK_UP
        // force port 2 FC_EN/HALF_DUPLEX/100M/LINK_UP
        // force port 3 FC_EN/HALF_DUPLEX/100M/LINK_UP
        for (i=0; i<=3; i++)
        {
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MEDIA_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_EEE_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MSTR_SLV_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FEFI_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_DUP_SELf, 0);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, 0x1);
        }

        // force port 52 FC_EN/FULL_DUPLEX/1000M/LINK_UP
        // force port 53 FC_EN/FULL_DUPLEX/1000M/LINK_UP
        for (i=52; i<=53; i++)
        {
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MEDIA_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_EEE_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MSTR_SLV_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FEFI_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 1);
        }
    }
    else
    {
        /* L2/L3 patch */
        osal_printf("FPGA patch (L2/L3)\n");

        // force port 0 FC_EN/FULL_DUPLEX/1000M/LINK_UP
        // force port 1 FC_EN/FULL_DUPLEX/1000M/LINK_UP
        // force port 2 FC_EN/FULL_DUPLEX/1000M/LINK_UP
        // force port 3 FC_EN/FULL_DUPLEX/1000M/LINK_UP
        for (i=0; i<=3; i++)
        {
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MEDIA_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_EEE_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MSTR_SLV_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FEFI_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 1);
        }

        // force port 52 FC_EN/FULL_DUPLEX/10G/LINK_UP
        // force port 53 FC_EN/FULL_DUPLEX/10G/LINK_UP
        for (i=52; i<=53; i++)
        {
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MEDIA_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_EEE_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_MSTR_SLV_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FEFI_ENf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 1);
            FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, i, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, 0x4);
        }
    }

    // VLAN
    for (i = 0; i <= 55; i++)
        rtk_vlan_port_add(unit, 1, i, TRUE);

    // Flow Contrl
    // adjust flow control threshold to fit in FPGA
    // set FC_GLB_DROP_THR, FPGA packet buffer has 730 page in total
    FPGA_REG_SET(unit, MANGO_FC_GLB_DROP_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x2da);
    // set FC_GLB_HI_THR (ON, OFF) = (434, 428)
    FPGA_REG_SET(unit, MANGO_FC_GLB_HI_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x1B201AC);
    // set FC_GLB_LO_THR (ON, OFF) = (301, 295)
    FPGA_REG_SET(unit, MANGO_FC_GLB_LO_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x12D0127);
    // set FC_GLB_FCOFF_HI_THR (ON, OFF) = (434, 428)
    FPGA_REG_SET(unit, MANGO_FC_GLB_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x1B201AC);
    // set FC_GLB_FCOFF_LO_THR (ON, OFF) = (301, 295)
    FPGA_REG_SET(unit, MANGO_FC_GLB_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x12D0127);
    // set FC_GLB_SYS_UTIL_THR
    FPGA_REG_SET(unit, MANGO_FC_GLB_SYS_UTIL_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0);

    // set FC_JUMBO_HI_THR (ON, OFF) = (434, 428)
    FPGA_REG_SET(unit, MANGO_FC_JUMBO_HI_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x1B201AC);
    // set FC_JUMBO_LO_THR (ON, OFF) = (301, 295)
    FPGA_REG_SET(unit, MANGO_FC_JUMBO_LO_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x12D0127);
    // set FC_JUMBO_FCOFF_HI_THR (ON, OFF) = (434, 428)
    FPGA_REG_SET(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x1B201AC);
    // set FC_JUMBO_FCOFF_LO_THR (ON, OFF) = (301, 295)
    FPGA_REG_SET(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x12D0127);

    // set RX groups 0~2 of FC_PORT_HI_THR (ON, OFF) = (38, 31)
    // set RX groups 0~2 of FC_PORT_LO_THR (ON, OFF) = (3, 2)
    // set RX groups 0~2 of FC_PORT_FCOFF_HI_THR (ON, OFF) = (38, 31)
    // set RX groups 0~2 of FC_PORT_FCOFF_LO_THR (ON, OFF) = (3, 2)
    // set RX groups 0~2 of FC_PORT_GUAR_THR = (3)
    for (i=0; i<=2; i++)
    {
        FPGA_REG_SET(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, i, 0x26001f);
        FPGA_REG_SET(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, i, 0x14000f);
        FPGA_REG_SET(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, i, 0x26001f);
        FPGA_REG_SET(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, i, 0x14000f);
        FPGA_REG_SET(unit, MANGO_FC_PORT_GUAR_THRr, REG_ARRAY_INDEX_NONE, i, 0x3);
    }
    // for Jumbo mode
    // set RX groups 3 of FC_PORT_HI_THR (ON, OFF) = (100, 60)
    // set RX groups 3 of FC_PORT_LO_THR (ON, OFF) = (42, 12)
    // set RX groups 3 of FC_PORT_FCOFF_HI_THR (ON, OFF) = (100, 60)
    // set RX groups 3 of FC_PORT_FCOFF_LO_THR (ON, OFF) = (42, 12)
    // set RX groups 3 of FC_PORT_GUAR_THR = (6)
    for (i=3; i<=3; i++)
    {
        FPGA_REG_SET(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, i, 0x64003c);
        FPGA_REG_SET(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, i, 0x2a000c);
        FPGA_REG_SET(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, i, 0x64003c);
        FPGA_REG_SET(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, i, 0x2a000c);
        FPGA_REG_SET(unit, MANGO_FC_PORT_GUAR_THRr, REG_ARRAY_INDEX_NONE, i, 0x6);
    }

    // set egress queue drop theshold of queue 0~3 in group 0~1 FC_Q_EGR_DROP_THR (ON, OFF) = (78, 68)
    for (i=0; i<=1; i++)
    {
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 0, i, 0x4e0044);
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 1, i, 0x4e0044);
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 2, i, 0x4e0044);
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 3, i, 0x4e0044);
    }
    // for Jumbo mode
    // set egress queue drop theshold of queue 0~3 in group 2 FC_Q_EGR_DROP_THR (ON, OFF) = (88, 58)
    for (i=2; i<=2; i++)
    {
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 0, i, 0x58003a);
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 1, i, 0x58003a);
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 2, i, 0x58003a);
        FPGA_REG_SET(unit, MANGO_FC_Q_EGR_DROP_THRr, 3, i, 0x58003a);
    }

    // set CPU port egress queue drop theshold FC_CPU_Q_EGR_DROP_THR (ON, OFF) = (78, 68)
    FPGA_REG_SET(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, 0, 0x4e0044);
    FPGA_REG_SET(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, 1, 0x4e0044);
    FPGA_REG_SET(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, 2, 0x4e0044);
    FPGA_REG_SET(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, 3, 0x4e0044);

    // Jumbo mode setting - Enable Jumbo mode, packet length threshold = 2000B, exit jumbo mode page threshold = 6 pages.
    FPGA_REG_SET(unit, MANGO_FC_JUMBO_THR_ADJUSTr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0x87D00006);

    // sprintf 'port set port all state enable'

    // set CPU port Tx/Rx enable
    // sprintf 'reg set 0x7c00 0x3'
    //FPGA_REG_SET(unit, MANGO_MAC_L2_PORT_CTRLr, 56, REG_ARRAY_INDEX_NONE, 0x3);
    // CPU port Force link-up
    // sprintf 'reg set 0xdf8 0x1AA01'
    FPGA_REG_SET(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, 0x32A01);

    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, 1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_DUP_ENf, 1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_SPD_ENf, 1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_FC_ENf, 1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, 1);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_SPD_SELf, 0x2);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, 0);
    FPGA_REG_FIELD_WRITE(unit, MANGO_MAC_FORCE_MODE_CTRLr, 56, REG_ARRAY_INDEX_NONE, MANGO_SMI_TX_PAUSE_ENf, 0);

    // Force CPU port egress queue drop when congestion
    // sprintf 'reg set 0xa9c8 0x1'
    FPGA_REG_SET(unit, MANGO_FC_CPU_Q_EGR_FORCE_DROP_CTRLr, REG_ARRAY_INDEX_NONE, 0, 0x1);

    // set ingress queue drop theshold of ingress queue 0~2 for all port (ON, OFF) = (220, 210)
    FPGA_REG_SET(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, 0, 0xDC00D2);
    FPGA_REG_SET(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, 1, 0xDC00D2);
    FPGA_REG_SET(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, 2, 0xDC00D2);

    // QoS Scheduling - Set queue empty threshold to be 0
    FPGA_REG_SET(unit, MANGO_SCHED_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, 0);

    return RT_ERR_OK;
#endif

}
#endif

uint32 _zigzag(uint32 ori, uint32 index)
{
    uint32 data = 0, mod;

    mod = index % 32;
    if (mod > 16)
        mod = 32 - mod;

    data = ((ori << 1) & (0xffff << (mod + 1))) | (ori & ((1 << mod) - 1));

    return data & 0x1ffff;
}

uint32 _zigzagReverse(uint32 zData, uint32 index)
{
    uint32 data = 0, mod;

    mod = index % 32;
    if (mod > 16)
        mod = 32 - mod;

    data = ((zData >> (mod + 1)) << mod) | (zData & ((1 << mod) - 1));

    return data & 0xffff;
}

int32 _mdx_read(uint32 unit, uint32 reg, uint32 *pValue)
{
    uint32 tmp = 0, cnt = 0, data;

    data = ((reg & 0x1f) << 7) | 0x9;
    reg_write(unit, MANGO_TEST_MDX_CTRLr, &data);
    do
    {
        reg_field_read(unit, MANGO_TEST_MDX_CTRLr, MANGO_MDX_REQf, &tmp);
        cnt++;
    } while (tmp != 0 && cnt < 1000);

    reg_field_read(unit, MANGO_TEST_MDX_CTRLr, MANGO_MDX_NO_TAf, &tmp);
    if (tmp)
        return RT_ERR_FAILED;

    reg_read(unit, MANGO_TEST_MDX_DATAr, pValue);

    return RT_ERR_OK;
}

int32 _mdx_write(uint32 unit, uint32 reg, uint32 value)
{
    uint32 tmp = 0, cnt = 0, data;

    data = value & 0xffff;
    reg_write(unit, MANGO_TEST_MDX_DATAr, &data);
    data = ((reg & 0x1f) << 7) | 0xb;
    reg_write(unit, MANGO_TEST_MDX_CTRLr, &data);
    do
    {
        reg_field_read(unit, MANGO_TEST_MDX_CTRLr, MANGO_MDX_REQf, &tmp);
        cnt++;
    } while (tmp != 0 && cnt < 1000);

    if (tmp != 0 && cnt >= 1000)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

static int32
_testio_set_ocp(uint32 unit, uint32 reg, uint32 value)
{
    int32   ret;
    uint32  data = 0, cnt;


    /* REG=0x8*/
    if ((ret = _mdx_write(unit, 0x8, value & 0xffff)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xA*/
    if ((ret = _mdx_write(unit, 0xa, value >> 16)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xC*/
    if ((ret = _mdx_write(unit, 0xc, reg)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xE*/
    if ((ret = _mdx_write(unit, 0xe, 0x800f)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    return RT_ERR_OK;
}

static int32
_testio_get_ocp(uint32 unit, uint32 reg, uint32 *pValue)
{
    int32   ret;
    uint32  data = 0, cnt;


    /* REG=0xC*/
    if ((ret = _mdx_write(unit, 0xc, reg)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xE*/
    if ((ret = _mdx_write(unit, 0xe, 0x8000)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));


    if ((ret = _mdx_read(unit, 0x8, &data)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    *pValue = data & 0xffff;

    if ((ret = _mdx_read(unit, 0xa, &data)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    *pValue |= data << 16;

    return RT_ERR_OK;
}

static int32
_testio_set_ePhy(uint32 unit, uint32 reg, uint32 value)
{
    int32   ret;
    uint32  data = 0, cnt;


    /* REG=0x8*/
    if ((ret = _mdx_write(unit, 0x8, value & 0xffff)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xA*/
    if ((ret = _mdx_write(unit, 0xa, 0x8000 | (reg & 0x7f))))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xC*/
    if ((ret = _mdx_write(unit, 0xc, 0xde20)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xE*/
    if ((ret = _mdx_write(unit, 0xe, 0x800f)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    /* REG=0xC*/
    if ((ret = _mdx_write(unit, 0xc, 0xde20)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xE*/
    if ((ret = _mdx_write(unit, 0xe, 0x8000)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xa, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);

    return RT_ERR_OK;
}

static int32
_testio_get_ePhy(uint32 unit, uint32 reg, uint32 *pValue)
{
    int32   ret;
    uint32  data = 0, cnt;


    /* REG=0x8*/
    if ((ret = _mdx_write(unit, 0x8, 0x0)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xA*/
    if ((ret = _mdx_write(unit, 0xa, reg & 0x7f)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xC*/
    if ((ret = _mdx_write(unit, 0xc, 0xde20)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xE*/
    if ((ret = _mdx_write(unit, 0xe, 0x800f)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));


    /* REG=0xC*/
    if ((ret = _mdx_write(unit, 0xc, 0xde20)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    /* REG=0xE*/
    if ((ret = _mdx_write(unit, 0xe, 0x8000)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));


    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xa, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while (((data >> 15) == 0) && (cnt < 1000));


    if ((ret = _mdx_read(unit, 0x8, pValue)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    return RT_ERR_OK;
}

static int32
_testio_set_eFuse_check(uint32 unit)
{
    uint32  data = 0, data2 = 0;

    /* Repair Margin read & Repair Check mode*/
    _testio_set_ocp(unit, 0xdd02, 0xB000);
    do
    {
        _testio_get_ocp(unit, 0xdd02, &data);
    } while (data >> 15);


    _testio_get_ocp(unit, 0xdd00, &data);
    data = data & 0x3;
    if (data == 0)
        osal_printf("OTP is good\n");
    else if (data == 1)
    {
        /* Repair Program mode*/
        _testio_set_ocp(unit, 0xdd02, 0x4800);
        do
        {
            _testio_get_ocp(unit, 0xdd02, &data);
        } while (data >> 14);

        /* Initial Margin read*/
        _testio_set_ocp(unit, 0xdd02, 0x9000);
        do
        {
            _testio_get_ocp(unit, 0xdd02, &data);
        } while (data >> 15);

        _testio_get_ocp(unit, 0xdd00, &data);
        _testio_get_ocp(unit, 0xdd02, &data2);
        if (data != 0xffff && ((data2 >> 7) & 1) != 1)
            osal_printf("OTP repair failed\n");
        else
        {
            osal_printf("OTP repair success\n");
            return RT_ERR_FAILED;
        }
    }
    else if (data == 3)
    {
        osal_printf("OTP is failure\n");
        return RT_ERR_FAILED;
    }
    else
    {
        osal_printf("Unknown result of OTP check");
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
}

static int32
_testio_set_eFuse(uint32 unit, uint32 index, uint32 reg, uint32 value)
{
    int32   ret;
    uint32  oriEfuseCfg;
    uint32  data = 0, zData = 0, cnt, cnt2 = 0;

    if (index > 0x7f)
    {
        osal_printf("index is out of range\n");
        return RT_ERR_FAILED;
    }


    /* Enable Efuse Writing */
    _testio_get_ocp(unit, 0xe85a, &oriEfuseCfg);
    _testio_set_ocp(unit, 0xe85a, oriEfuseCfg | (0x3 << 14));


    /* Set Efuse sequential program mode */
    if ((ret = _mdx_write(unit, 0x8, 0x0)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xa, 0x0)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xc, 0xdd04)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xe, 0x8003)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);



    /* Efuse write first 17-bit data */

    /* Write Efuse command and EPhy addr */
    data = (reg << 8) | 0x80bf;
    zData = _zigzag(data, index);
    if ((ret = _mdx_write(unit, 0x8, zData & 0xffff)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    data = index | 0x4000 | (((zData >> 16) & 0x1) << 7);
    if ((ret = _mdx_write(unit, 0xa, data)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xe, 0x800f)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    do
    {
        if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        if ((ret = _mdx_write(unit, 0xe, 0x8000)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt = 0;
        do
        {
            if ((ret = _mdx_read(unit, 0xe, &data)))
            {
                osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
            }
            cnt++;
        } while ((data >> 15) && (cnt < 1000));
        if (cnt >= 1000)
            osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


        if ((ret = _mdx_read(unit, 0xa, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt2++;
    } while ((data >> 14) && (cnt2 < 1000));
    if (cnt2 >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);



    /* Efuse write 2nd 17-bit data */

    /* Write Efuse data */
    data = value;
    zData = _zigzag(data, index + 1);
    if ((ret = _mdx_write(unit, 0x8, zData & 0xffff)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    data = (index + 1) | 0x4000 | ((zData >> 16) << 7);
    if ((ret = _mdx_write(unit, 0xa, data)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xe, 0x800f)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    do
    {
        if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        if ((ret = _mdx_write(unit, 0xe, 0x8000)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt = 0;
        do
        {
            if ((ret = _mdx_read(unit, 0xe, &data)))
            {
                osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
            }
            cnt++;
        } while ((data >> 15) && (cnt < 1000));
        if (cnt >= 1000)
            osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


        cnt2 = 0;
        if ((ret = _mdx_read(unit, 0xa, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 14) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    /* Disable Efuse Writing */
    _testio_set_ocp(unit, 0xe85a, oriEfuseCfg);

    return RT_ERR_OK;
}

static int32
_testio_get_eFuse(uint32 unit, uint32 index, uint32 *pReg, uint32 *pValue)
{
    int32   ret;
    uint32  data = 0, zData = 0, cnt, cnt2 = 0;


    /* Efuse read first 17-bit data */
    if ((ret = _mdx_write(unit, 0x8, 0x0)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xa, 0x8000 | index)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xe, 0x800c)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    do
    {
        if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        if ((ret = _mdx_write(unit, 0xe, 0x8000)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt = 0;
        do
        {
            if ((ret = _mdx_read(unit, 0xe, &data)))
            {
                osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
            }
            cnt++;
        } while ((data >> 15) && (cnt < 100));
        if (cnt >= 100)
            osal_printf("%s():%d  Failed!  data:%#x\n", __FUNCTION__, __LINE__, data);


        if ((ret = _mdx_read(unit, 0xa, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt2++;
    } while ((data >> 15) && (cnt2 < 100));
    if (cnt2 >= 100)
        osal_printf("%s():%d  Failed!  data:%#x\n", __FUNCTION__, __LINE__, data);

    zData = ((data >> 7) & 0x1) << 16;

    if ((ret = _mdx_read(unit, 0x8, &data)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    zData |= data;
    *pReg = (_zigzagReverse(zData, index) >> 8) & 0x7f;



    /* Efuse read 2nd 17-bit data */
    if ((ret = _mdx_write(unit, 0x8, 0x0)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xa, 0x8000 | (index + 1))))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    if ((ret = _mdx_write(unit, 0xe, 0x800c)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }
    cnt = 0;
    do
    {
        if ((ret = _mdx_read(unit, 0xe, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt++;
    } while ((data >> 15) && (cnt < 1000));
    if (cnt >= 1000)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);


    do
    {
        if ((ret = _mdx_write(unit, 0xc, 0xdd00)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        if ((ret = _mdx_write(unit, 0xe, 0x8000)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt = 0;
        do
        {
            if ((ret = _mdx_read(unit, 0xe, &data)))
            {
                osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
            }
            cnt++;
        } while ((data >> 15) && (cnt < 100));
        if (cnt >= 100)
            osal_printf("%s():%d  Failed!  data:%#x\n", __FUNCTION__, __LINE__, data);


        if ((ret = _mdx_read(unit, 0xa, &data)))
        {
            osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
        }
        cnt2++;
    } while ((data >> 15) && (cnt2 < 100));
    if (cnt2 >= 100)
        osal_printf("%s():%d  Failed\n", __FUNCTION__, __LINE__);

    zData = ((data >> 7) & 0x1) << 16;

    if ((ret = _mdx_read(unit, 0x8, &data)))
    {
        osal_printf("%s():%d Failed\n", __FUNCTION__, __LINE__);
    }

    zData |= data;
    *pValue = _zigzagReverse(zData, index + 1);

    return RT_ERR_OK;
}

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
    rtk_time_timeStamp_t timeStamp;
    rtk_qos_1pRmkVal_t  priVal;
    rtk_qos_outer1pRmkVal_t  oPriVal;
    int32 scan_idx;
    rtk_l2_mcastAddr_t  mcast_data;

    /* reset H/W table, register */
    //osal_printf("Reset table (by using init reg)\n");
    value = 0xffffffff;
    osal_memset(&l2FlushCfg, 0, sizeof(rtk_l2_flushCfg_t));
#if 0
    reg_write(unit, MANGO_MEM_ALE_INIT_0r, &value);
    reg_write(unit, MANGO_MEM_ALE_INIT_1r, &value);
    reg_write(unit, MANGO_MEM_ALE_INIT_2r, &value);
#endif

    //osal_printf("Reset extra HSA memory\n");
    value = 1;
#if 0
    reg_field_write(unit, MANGO_MEM_EGR_CTRLr, MANGO_LINK_INITf, &value);
#endif
    //osal_time_mdelay(100);  /* wait for 100mS */

    /* reset QOS */
    for(port = 0; port < 56; port ++)
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

    /* reset Tunnel */
    ret = rtk_tunnel_intf_destroyAll(unit);
    if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);

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
    ret = rtk_l3_globalCtrl_set(unit, RTK_L3_GCT_NONE, 0);
    if (RT_ERR_OK != ret) osal_printf("%s():%d - ret = 0x%x\n", __FUNCTION__, __LINE__, ret);

    /* reset BPE (802.1BR) */

    /* reset L2 */
    //osal_printf("Reset L2 module\n");
    l2FlushCfg.act = FLUSH_ACT_FLUSH_ALL_UC;
    ret = rtk_l2_ucastAddr_flush(unit, &l2FlushCfg);
    if (RT_ERR_OK != ret) osal_printf("%s():%d FATAL - ret (%d) != 0\n", __FUNCTION__, __LINE__, ret);
    /* remove L2 MC entry */
    value = 0x80000070;
    reg_write(unit, MANGO_MEM_ALE_INIT_0r, &value);
    value = 0x00000060;
    reg_write(unit, MANGO_MEM_ALE_INIT_2r, &value);
    /* reset learning counter */
    value = 0;
    reg_field_write(unit, MANGO_L2_LRN_CONSTRT_CNTr, MANGO_LRN_CNTf, &value);
    HWP_ETHER_PORT_TRAVS(unit, port)
        reg_array_field_write(unit, MANGO_L2_LRN_PORT_CONSTRT_CNTr, port, REG_ARRAY_INDEX_NONE, MANGO_LRN_CNTf, &value);
    for (idx = 0; idx < HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit); idx++)
        reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, idx, MANGO_LRN_CNTf, &value);

    scan_idx = -1;

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
    RT_ERR_CHK(reg_write(unit, MANGO_VLAN_CTRLr, &value), ret);
    RT_ERR_CHK(reg_write(unit, MANGO_VLAN_APP_PKT_CTRLr, &value), ret);
    ret = rtk_vlan_destroyAll(unit, 1);
    for(port = 0; port < 56; port ++)
    {
        /* initial default value first */
        value = 0x8006001;
        RT_ERR_CHK(reg_array_write(unit, MANGO_VLAN_PORT_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, &value), ret);
        value = 0xC;
        RT_ERR_CHK(reg_array_write(unit, MANGO_VLAN_PORT_FWD_CTRLr, port, REG_ARRAY_INDEX_NONE, &value), ret);
        value = 0x1;
        RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PORT_IGR_FLTRr, port, REG_ARRAY_INDEX_NONE, MANGO_IGR_FLTR_ACTf, &value), ret);
        value = 0x1;
        RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PORT_EGR_FLTRr, port, REG_ARRAY_INDEX_NONE, MANGO_EGR_FLTR_ENf, &value), ret);
        value = 0x3C9;
        RT_ERR_CHK(reg_array_write(unit, MANGO_VLAN_PORT_TAG_CTRLr, port, REG_ARRAY_INDEX_NONE, &value), ret);

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

    /* VLAN Profile */
    {
        uint32 idx;
        uint32 data;

        for (idx=0; idx<16; idx++)
        {
            data = 0x0;
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_L2_NEW_SA_LRNf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_L2_NEW_SA_ACTf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IPMC_BRIDGE_MODEf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IP6MC_BRIDGE_MODEf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_L2MC_BRIDGE_LU_MIS_ACTf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IPMC_BRIDGE_LU_MIS_ACTf, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IP6MC_BRIDGE_LU_MIS_ACTf, &data), ret);

            data = 0x0FFFFFF;
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_L2MC_UNKN_FLD_PMSK_52_32f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IPMC_UNKN_FLD_PMSK_52_32f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IP6MC_UNKN_FLD_PMSK_52_32f, &data), ret);

            data = 0xFFFFFFFF;
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_L2MC_UNKN_FLD_PMSK_31_0f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IPMC_UNKN_FLD_PMSK_31_0f, &data), ret);
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx, MANGO_IP6MC_UNKN_FLD_PMSK_31_0f, &data), ret);
        }
    }

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
            RT_ERR_CHK(table_write(unit, MANGO_VLAN_IVCt, idx, &ivcEntry[0]), ret);

            /* clear hit-bit */
            reg_array_field_write(unit, MANGO_VLAN_IVC_ENTRY_INDICATIONr, REG_ARRAY_INDEX_NONE, idx, MANGO_HITf, &value);
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
            RT_ERR_CHK(table_write(unit, MANGO_VLAN_EVCt, idx, &evcEntry[0]), ret);

            /* clear hit-bit */
            reg_array_field_write(unit, MANGO_VLAN_EVC_ENTRY_INDICATIONr, REG_ARRAY_INDEX_NONE, idx, MANGO_HITf, &value);
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
        rtk_acl_portPhaseLookupEnable_set(unit, idx, ACL_PHASE_EACL, ENABLED);
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

    /*Y.1731*/
    for (port = 0; port <= RTK_MAX_NUM_OF_PORTS; ++port)
        rtk_oam_cfmPortEthDmEnable_set(unit, port, DISABLED);

    /*Read to clear*/
    rtk_oam_cfmEthDmRxTimestamp_get(unit, 1, &timeStamp);
    rtk_oam_cfmEthDmRxTimestamp_get(unit, 0, &timeStamp);

    osal_printf("SDK reset ok!\n");

    return ret;
}

void
_dummy_tx_callback(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    return;
}

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

uint32 R_OFS_A[] = {-1, -2, -1, -1, -2, -2, -2, -2};
uint32 R_OFS_B[] = {-2, -2, -2, -2, -1, -1, -1, -1};
uint32 R_OFS_C[] = {-1, -1, -1, -1, -1, -1, -1, -1};
uint32 R_OFS_D[] = {-1, -1, -1, -1, -1, -1, -1, -1};
uint32 *R_OFS[] = {R_OFS_A, R_OFS_B, R_OFS_C, R_OFS_D};

uint32 i2c_cal_brd[] = {1, 1, 2, 2, 3, 3, 4, 4};

uint32
_lan_switch_select(uint32 unit, rtk_lanSwitchMode_t mode)
{
    gpioID gpioId_LP = 20, gpioId_SEL = 21 ;

    drv_gpio_pin_init(unit, gpioId_LP, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);
    drv_gpio_pin_init(unit, gpioId_SEL, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_BOTH_EDGE);

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

    phy_field_write(unit, port, 0xa43, 27, 31, 0, data[channel]);
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
_adc_ofs_cal(uint32 unit, uint32 port)
{
    uint32 channel;
    uint32 phyId = port % 8;
    uint32 chnlOfsetBitEnd[] = {3,7,11,15};
    uint32 chnlOfsetBitStart[] = {0,4,8,12};
    uint32 step_ado = 10;
    uint32 spec_ado = step_ado/2;
    uint32 loop = 1;
    uint32 mean;
    int32 meanInt, meanDev;
    uint32 offset, offsetCal;
    int32 step_ioffset_cnt, offsetD;

    uint32 chnlOfsetA[] = {8, 8, 9, 8, 8, 8, 8, 8};
    uint32 chnlOfsetB[] = {8, 8, 8, 8, 6, 6, 6, 6};
    uint32 chnlOfsetC[] = {8, 8, 8, 8, 8, 8, 8, 8};
    uint32 chnlOfsetD[] = {8, 8, 8, 8, 6, 6, 6, 6};
    uint32 *chnlOfset[] = {chnlOfsetA, chnlOfsetB, chnlOfsetC, chnlOfsetD};

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    phy_field_write(unit, port, 0xa43, 27, 15, 0, 0x8010);
    phy_field_write(unit, port, 0xa43, 28, 11, 11, 0x0);
    phy_field_write(unit, port, 0xb82, 16, 4, 4, 0x1);

    if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 1) != RT_ERR_OK)
        return RT_ERR_FAILED;

    phy_field_write(unit, port, 0xa40, 0, 12, 12, 0x0);
    phy_field_write(unit, port, 0xa4A, 19, 7, 6, 0x2);
    phy_field_write(unit, port, 0xa44, 20, 2, 2, 0x1);
    phy_field_write(unit, port, 0xa81, 16, 9, 6, 0xF);
    phy_field_write(unit, port, 0xa81, 16, 4, 0, 0x0);
    phy_field_write(unit, port, 0xa82, 16, 4, 4, 0x0);

    if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 0) != RT_ERR_OK)
    {
        osal_printf("%s FAIL patch_rdy fail.", __FUNCTION__);
        return RT_ERR_FAILED;
    }

    if(phy_field_chk(unit, port, 0xa60, 16, 7, 0, 0x41) != RT_ERR_OK)
        return RT_ERR_FAILED;

    for(channel = 0; channel < 4; channel ++)
    {
        phy_field_write(unit, port, 0xbcf, 22, chnlOfsetBitEnd[channel], chnlOfsetBitStart[channel], chnlOfset[channel][phyId]);
        while (1)
        {
            phy_field_read(unit, port, 0xbcf, 22, chnlOfsetBitEnd[channel], chnlOfsetBitStart[channel], &offset);
            _int_ado_mean(unit, port, channel, &mean);
            meanInt = mean;
            if(meanInt > 0x100)
                meanInt -= 0x200;
            if(meanInt < 0)
                meanDev = - meanInt;
            else
                meanDev = meanInt;
            osal_printf("   meanInt: %d\n", meanInt);
            //check adc_ioffset in specification or not
            if(meanDev <= spec_ado || loop == 5)
            {
                offsetCal = offset;
                osal_printf("   offsetCal: 0x%x\n", offsetCal);
                break;
            }
            //Adjust adc_ioffset
            step_ioffset_cnt = _round(meanDev) / step_ado;
            if(meanDev - (step_ado * step_ioffset_cnt) > spec_ado)
                step_ioffset_cnt += 1;

            if(meanInt > 0)
                step_ioffset_cnt *= -1;

            offsetD = offset + step_ioffset_cnt;
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
    phy_field_write(unit, port, 0xa43, 27, 31, 0, 0x8010);
    phy_field_write(unit, port, 0xa43, 28, 11, 11, 0x1);

    //Set patch_req = 1
    phy_field_write(unit, port, 0xb82, 16, 4, 4, 0x1);

    //Poll patch_rdy = 10x80100x
    if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 1) != RT_ERR_OK)
    {
        osal_printf("FAIL patch_rdy fail.");
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
    phy_field_write(unit, port, 0xa82, 16, 4, 4, 0x0);

    //Poll patch_rdy = 0
    if(phy_field_chk(unit, port, 0xb80, 16, 6, 6, 0) != RT_ERR_OK)
    {
        osal_printf("FAIL patch_rdy fail.");
        return RT_ERR_FAILED;
    }

    //Check adc_ioffset
    phy_field_read(unit, port, 0xbcf, 22, 31, 0, &offset);
    osal_printf("PHY %d Offset 0x%x\n", port, offset);

    return RT_ERR_OK;
}

uint32
_rc_cal(uint32 unit, uint32 port)
{
    uint32 len;
    uint32 rlen;

    if(_phy_check_on(unit, port) != RT_ERR_OK)
        return RT_ERR_FAILED;

    //Enable RC calibration function
    phy_field_write(unit, port, 0xa47, 17, 9, 9, 0x1);
    osal_time_mdelay(100);
    if(phy_field_chk(unit, port, 0xa47, 17, 9, 9, 0) != RT_ERR_OK)
    {
        osal_printf("FAIL RC_CAL_EN fail.");
        return RT_ERR_FAILED;
    }

    //Check RC calibration done
    phy_field_write(unit, port, 0xa43, 27, 31, 0, 0x8020);
    if(phy_field_chk(unit, port, 0xa43, 28, 8, 8, 1) != RT_ERR_OK)
    {
        osal_printf("FAIL RC_CAL_EN fail.");
        return RT_ERR_FAILED;
    }


    //Check RC calibration result
    phy_field_read(unit, port, 0xbcd, 22, 31, 0, &len);
    phy_field_read(unit, port, 0xbcd, 23, 31, 0, &rlen);

    if (len < 0x1111)
    {
        osal_printf("=====   PHY_%d, RC_CAL result = %d is less than 0x1111 !!!   =====\n", port, len);
    }
    else if (rlen < 0x1111)
    {
        osal_printf("=====   PHY_%d, RC_CAL result = %d is less than 0x1111 !!!   =====\n", port, rlen);
    }
    else if (len > 0xEEEE)
    {
        osal_printf("=====   PHY_%d, RC_CAL result = %d is larger than 0xEEEE !!!   =====\n", port, len);
    }
    else if (rlen < 0xEEEE)
    {
        osal_printf("=====   PHY_%d, RC_CAL result = %d is larger than 0xEEEE !!!   =====\n", port, rlen);
    }
    else
    {
        osal_printf("-  PHY_%d, len=%d, rlen=%d\n", port, len, rlen);
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

    osal_time_mdelay(100);

    return RT_ERR_OK;
}

uint32
_adc1015_get(uint32 unit, uint32 port, uint32 i2c_channel, uint32 phyChannel, uint32 pole, uint32 *pMVolt)
{
    uint32 scale = 4096;
    uint32 phy_id_r = port %2;
    uint32 i2c_addr = 0x90 + (i2c_channel / 2) + (phy_id_r * 2);
    uint32 adc_cfg, adc_get;
    i2c_devConf_t i2c_dev;
    uint32 volt, voltFrag;

    osal_memset(&i2c_dev, 0, sizeof(i2c_dev));

    switch(phyChannel)
    {
        case 0:
        case 2:
            if(pole == 0) //P
                adc_cfg = 0x83C3;
            else //N
                adc_cfg = 0x83D3;
            break;
        case 1:
        case 3:
            if(pole == 0) //P
                adc_cfg = 0x83F3;
            else //N
                adc_cfg = 0x83E3;
            break;
        default:
            break;
    }

    i2c_dev.device_id = i2c_addr;                  /* I2C driver device structure ID */
                                                                /* The I2C driver device structure is a software structure*/
                                                                /* For RTL839x/8x, this is mapped to SMI */

    i2c_dev.i2c_interface_id = 0;           /* Chip I2C master interface ID */
    i2c_dev.dev_addr = 0;                   /* device address of the slave device*/
    i2c_dev.clk_freq = 0;                   /* Serial clock frequency */
    i2c_dev.scl_delay = 0;                  /* For RTL839x/8x software simulation of I2C clock delay time */
    i2c_dev.mem_addr_width = 0;             /* slave memory address/Regitser index field width */
    i2c_dev.data_width = 0;                 /* slave data field width */
    i2c_dev.read_type = 0;                  /* Select Read Type : 0(Random Read) / 1(Sequential Read) */
    i2c_dev.scl_pin_id = 8;                 /* SCL pin ID */
    i2c_dev.sda_pin_id = 9;                 /* SDA pin ID */


    drv_i2c_dev_init(unit, &i2c_dev);

    //configure ADC
    drv_i2c_write(unit, i2c_addr, 1, (uint8 *)&adc_cfg);

    //get voltage
    drv_i2c_read(unit, i2c_addr, 0, (uint8 *)&adc_get);

    //high byte and low byte inverse
    adc_get = ((adc_get & 0xFF) << 8) + ((adc_get & 0xFF00) >> 8);

    //D[3:0] no use, shift >> 4
    adc_get = adc_get >> 4;

    //return ADC get voltage value
    volt = ((adc_get * scale) / 1000) & 0x7FF;
    voltFrag = ((adc_get * scale) % 1000);

    *pMVolt = volt * 1000 + voltFrag;
    return RT_ERR_OK;
}

uint32
_amp_cal_ext(uint32 unit, uint32 port)
{
    uint32 channel;
    uint32 amp_base = 2030;
    uint32 phyId = port % 8;
    uint32 bit_ibadj_end[] = {3,7,11,15};
    uint32 bit_ibadj_start[] = {0,4,8,12};
    int32 ampOfsA[] = {-40, -53, -20, -32, -16, -41, -16, -33};
    int32 ampOfsB[] = {-16, -30, -15, 10, -23, -40, -25, -41};
    int32 ampOfsC[] = {-28, -42, -18, -11, -20, -40, -20, -37};
    int32 ampOfsD[] = {-28, -42, -18, -11, -20, -40, -20, -37};
    int32 *ampOf[] = {ampOfsA, ampOfsB, ampOfsC, ampOfsD};
    int32 mAmpTarget;
    uint32 step_ibadj_dco = 20, step_ibadj_cnt;
    uint32 spec_ibadj_dco = step_ibadj_dco / 2;
    uint32 loop = 1, break_index = 0;
    int32 ibadj, ibadj_cal;
    uint32 mvoltP, mvoltN;
    int32 mvoltPos, mvoltNeg, mvoltDiff;
    int32 volt_dc_target, volt_dc_target_dev;


    if(_phy_check_on(unit, port) != RT_ERR_OK)
        return RT_ERR_FAILED;

    for (channel = 0; channel < 4; channel ++)
    {
        osal_printf("Channel %d\n", channel);

        mAmpTarget = amp_base + ampOf[channel][phyId];


        while(1)
        {
            phy_field_read(unit, port, 0xbca, 22, bit_ibadj_end[channel], bit_ibadj_start[channel], &ibadj);
            osal_printf("   ibadj %d\n", ibadj);

            _phy_dc_mode(unit, port, DC_MODE_POS_1);

            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 0, &mvoltP);
            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 1, &mvoltN);

            mvoltPos = mvoltP - mvoltN;

            _phy_dc_mode(unit, port, DC_MODE_NEG_1);

            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 0, &mvoltP);
            _adc1015_get(unit, port, i2c_cal_brd[phyId], channel, 1, &mvoltN);

            mvoltNeg = mvoltP - mvoltN;

            mvoltDiff = (mvoltPos - mvoltNeg) / 2;

            osal_printf("mvoltDiff: %d mvoltPos: %d mvoltNeg: %d\n", mvoltDiff, mvoltPos, mvoltNeg);

            //Check ibadj in specification or not
            volt_dc_target = mvoltDiff - mAmpTarget;
            volt_dc_target_dev = (mAmpTarget < 0) ? -mAmpTarget: mAmpTarget;

            if(volt_dc_target_dev <= spec_ibadj_dco || break_index == 1 || loop == 5)
            {
                ibadj_cal = ibadj;
                osal_printf("ibadj_cal: %d volt_dc_target_dev: %d break_index: %d loop: %d\n", ibadj_cal, volt_dc_target_dev, break_index, loop);
                break;
            }

            //Adjust ibadj
            step_ibadj_cnt = (_round(volt_dc_target_dev) * 10) / step_ibadj_dco;
            if ((volt_dc_target_dev * 10 - step_ibadj_dco * step_ibadj_cnt) > spec_ibadj_dco * 10)
                step_ibadj_cnt += 10;

            if (volt_dc_target > 0)
                step_ibadj_cnt *= -1;

            //Set ibadj
            ibadj += step_ibadj_cnt / 10;

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
    osal_printf("%s port %d ibadj %d\n", __FUNCTION__, port, ibadj);

    _phy_dc_mode(unit, port, DC_MODE_NA);

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
_new_r_cal(uint32 unit, uint32 port)
{
    uint32 volt_meas_pga_gain;
    uint32 channel;
    uint32 bit_tapbin_end[] = {3,7,11,15};
    uint32 bit_tapbin_start[] = {0,4,8,12};
    uint32 step_tapbin_ado = 50;//10 times of real value
    uint32 spec_tapbin_ado = step_tapbin_ado / 2; //10 times of real value
    uint32 loop;
    uint32 tapbin;
    int32 ado;
    rtk_voltMeas_t volt_meas;
    int32 ado_volt_meas[VOLT_MEAS_END][2];
    uint32 pole;
    int32 vtx, va, vb, volt_calc;
    int32 volt_calc_dev, tapbin_cal, step_tapbin_cnt;
    uint32 low_limit = 0x1, high_limit = 0xE;
    uint32 phyId = port % 8;
    #if 1 //maybe too large for uint32 after calculate
    int32 gain_factor[] = {
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

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    //Check PHY status
    if(_phy_check_on(unit, port) != RT_ERR_OK)
        return RT_ERR_FAILED;

    //Third step: calibration resistor
    //Force PGA gain, Jim_Chan: target range 200~220
    volt_meas_pga_gain = 1;

    _frc_pga_gain(unit, port, volt_meas_pga_gain);

     for (channel = 0; channel < 4; channel ++)
    {
        osal_printf("Channel %d\n", channel);

        //Force TX/RX on/off
        phy_field_write(unit, port, 0xBC0, 19, 4, 4, 1); //GETPOWCTRL_L

        switch(channel)
        {
            case 0:
                phy_field_write(unit, port, 0xBC0, 18, 3, 0, 1); //FORCEGIGATX_L
                phy_field_write(unit, port, 0xBC0, 18, 7, 4, 1); //FORCEGIGARX_L
                phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0xE); //EN_KADC_L
                break;
        }

        //Calibrate tapbin by interpolation search
        loop = 1;

        while(1)
        {
            //Get tapbin
            phy_field_read(unit, port, 0xBCE, 16, bit_tapbin_end[channel], bit_tapbin_end[channel], &tapbin);
            osal_printf("tapbin: %d\n", tapbin);

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
                    _phy_dc_mode(unit, port, DC_MODE_POS_1);


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
            va = (ado_volt_meas[VOLT_MEAS_VA][DC_MODE_POS_1 - 1] - ado_volt_meas[VOLT_MEAS_VA][DC_MODE_NEG_1 - 1]);
            vb = (ado_volt_meas[VOLT_MEAS_VB][DC_MODE_POS_1 - 1] - ado_volt_meas[VOLT_MEAS_VB][DC_MODE_NEG_1 - 1]);

            if(vtx > 20 || vtx < -20)
            {
                return RT_ERR_FAILED;
            }

            // 2 times than usual
            volt_calc = (vtx * 100000000 / gain_factor[0]) * 2 - va - vb;
            //Check tapbin in specification or not
            volt_calc_dev = (volt_calc > 0) ? volt_calc:volt_calc*-1;
            if( volt_calc_dev / 2 <= spec_tapbin_ado || loop == 5)
            {
                tapbin_cal = tapbin;
                osal_printf("tapbin_cal : %d\n", tapbin_cal);
            }

            //Adjust tapbin
            //100 times larger
            step_tapbin_cnt = _round(volt_calc_dev / 2) * 100 / step_tapbin_ado;
            if(volt_calc_dev*50 - (step_tapbin_ado*step_tapbin_cnt) > spec_tapbin_ado * 100)
                step_tapbin_cnt += 100;

            if(volt_calc < 0)
                step_tapbin_cnt *= -1;

            //Set tapbin
            tapbin += step_tapbin_cnt / 100;

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
        osal_printf(" tapbin offset:  R_OFS[%d][%d]:%d tapbin_cal: %d\n", channel, phyId, R_OFS[channel][phyId], tapbin_cal);
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

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    //Release power control
    phy_field_write(unit, port, 0xBC0, 19, 4, 4, 0); //GETPOWCTRL_L
    phy_field_write(unit, port, 0xBC0, 18, 3, 0, 0xF); //FORCEGIGATX_L
    phy_field_write(unit, port, 0xBC0, 18, 7, 4, 0xF); //FORCEGIGARX_L

    //Back to default signal path
    phy_field_write(unit, port, 0xBC0, 17, 3, 0, 0); //EN_HYBRID_TX_L
    phy_field_write(unit, port, 0xBC0, 17, 7, 4, 0xF); //EN_HYBRID_TXRX_L
    phy_field_write(unit, port, 0xBC0, 17, 11, 8, 0xF); //EN_HYBRID_ABSHORT_L
    phy_field_write(unit, port, 0xBC0, 17, 15, 12, 0xF); //EN_KADC_L

    //Release PGA gain
    _frc_pga_gain(unit, port, 0);

    //Disable TX test mode DC output
    _phy_dc_mode(unit, port, DC_MODE_NA);

    return RT_ERR_OK;
}

uint32
_amp_chk_ext(uint32 unit, uint32 port)
{
    uint32 channel;
    uint32 mvoltP, mvoltN;
    int32 mvoltPos, mvoltNeg, mvoltDiff;
    uint32 phyId = port % 8;

    _lan_switch_select(unit, SWITCH_MODE_DC);

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
            return RT_ERR_FAILED;
        }
        else if (mvoltDiff > 1097)
        {
            osal_printf("PHY_%d, CH_%d, AMP_CHK_EXT result = %d is larger than 1.09725(V)\n", port, channel, mvoltDiff);
            return RT_ERR_FAILED;
        }
    }

    _phy_dc_mode(unit, port, DC_MODE_NA);

    _lan_switch_select(unit, SWITCH_MODE_OPEN);

    return RT_ERR_OK;

}

uint32
_cal_para_rd(uint32 unit, uint32 port, char *pPara)
{
    uint32 adc_ioffset, len, rlen, tapbin, tapbin_pm, ibadj;

    //adc_ioffset
    if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "adc_ioffset") == 0)
    {
        phy_field_write(unit, port, 0, 30, 15, 0, 1);
        phy_field_read(unit, port, 0xBCF, 22, 15, 0, &adc_ioffset);

        osal_printf("port %d adc_ioffset: %d\n", port, adc_ioffset);
    }


    //len
    if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "len") == 0)
    {
        phy_field_write(unit, port, 0, 30, 15, 0, 1);
        phy_field_read(unit, port, 0xBCD, 22, 15, 0, &len);

        osal_printf("port %d len: %d\n", port, len);
    }


    //rlen
    if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "rlen") == 0)
    {
        phy_field_write(unit, port, 0, 30, 15, 0, 1);
        phy_field_read(unit, port, 0xBCD, 23, 15, 0, &rlen);

        osal_printf("port %d rlen: %d\n", port, rlen);
    }

    //tapbin
    if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "tapbin") == 0)
    {
        phy_field_write(unit, port, 0, 30, 15, 0, 1);
        phy_field_read(unit, port, 0xBCE, 16, 15, 0, &tapbin);

        osal_printf("port %d tapbin: %d\n", port, tapbin);
    }

    //tapbin_pm
    if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "tapbin_pm") == 0)
    {
        phy_field_write(unit, port, 0, 30, 15, 0, 1);
        phy_field_read(unit, port, 0xBCE, 17, 15, 0, &tapbin_pm);

        osal_printf("port %d tapbin_pm: %d\n", port, tapbin_pm);
    }


    //ibadj
    if(osal_strcmp(pPara, "") == 0 || osal_strcmp(pPara, "ibadj") == 0)
    {
        phy_field_write(unit, port, 0, 30, 15, 0, 1);
        phy_field_read(unit, port, 0xBCA, 22, 15, 0, &ibadj);

        osal_printf("port %d tapbin_pm: %d\n", port, ibadj);
    }

    return RT_ERR_OK;
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
    rtdrv_ext_union_t buf;
    int32 ret = RT_ERR_FAILED;
    uint32 idx;
    rtk_portmask_t trk_igr_ports;
    rtk_trk_egrPort_t trk_egr_ports;
    auto_rcvy_txerr_cnt_entry_t txerrCntr;
    uint32 myUnit;
    rtk_portmask_t ingressPorts;
    uint32 val;
    rtk_port_t port;
    rtk_dev_port_t unit_port;

    osal_memset(&txerrCntr, 0, sizeof(auto_rcvy_txerr_cnt_entry_t));

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
        case RTDRV_EXT_L2_AGING_UNIT_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_write(buf.l2_cfg.unit, MANGO_L2_AGE_CTRLr, MANGO_AGE_UNITf, &buf.l2_cfg.aging_time);
            break;

        case RTDRV_EXT_L2_CMA_ENABLE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_write(buf.l2_cfg.unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &buf.l2_cfg.enable);
            rtk_l2_init(buf.l2_cfg.unit);
            break;
    /** ACL **/
        case RTDRV_EXT_ACL_MIRROR_SFLOW_BYPASS_POST_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_ext_aclCfg_t));
            ret = reg_field_write(buf.acl_cfg.unit, MANGO_PIE_ENCAP_CTRLr, MANGO_EGR_MIR_SFLOW_BYPASS_POSTf, &buf.acl_cfg.enable);
            break;
    /** PORT **/
        case RTDRV_EXT_PORT_MACFORCESTATE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, &buf.port_cfg.state);
            break;

        case RTDRV_EXT_PORT_MACFORCELINK_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, &buf.port_cfg.state);
            break;

        case RTDRV_EXT_PORT_MACFORCEFLOWCTRL_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, &buf.port_cfg.state);
            ret = reg_array_field_write(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_TX_PAUSE_ENf, &buf.port_cfg.state);
            break;

        case RTDRV_EXT_PORT_PHYCALI_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));

            ret = _adc_ofs_cal(buf.port_cfg.unit, buf.port_cfg.port);
            if(ret != RT_ERR_OK)
                return ret;
            ret = _rc_cal(buf.port_cfg.unit, buf.port_cfg.port);
            if(ret != RT_ERR_OK)
                return ret;
            ret = _amp_cal_ext(buf.port_cfg.unit, buf.port_cfg.port);
            if(ret != RT_ERR_OK)
                return ret;
            ret = _new_r_cal(buf.port_cfg.unit, buf.port_cfg.port);
            if(ret != RT_ERR_OK)
                return ret;
            ret = _cal_para_rd(buf.port_cfg.unit, buf.port_cfg.port, "");
            if(ret != RT_ERR_OK)
                return ret;

            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_field_write(buf.port_cfg.unit, MANGO_SC_DRAIN_OUT_THRr, MANGO_DRAIN_OUT_THRf, &buf.port_cfg.full_th);
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_HALF_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_field_write(buf.port_cfg.unit, MANGO_SC_DRAIN_OUT_THRr, MANGO_DRAIN_OUT_THR_Hf, &buf.port_cfg.half_th);
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, MANGO_SC_PORT_TIMERr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_CNGST_SUST_TMR_LMTf, &buf.port_cfg.full_sec);
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_HALF_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, MANGO_SC_PORT_TIMERr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_CNGST_SUST_TMR_LMT_Hf, &buf.port_cfg.half_sec);
            break;

    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
        case RTDRV_EXT_STACK_DEBUG_SET:
            copy_from_user(&buf.stack_cfg, user, sizeof(rtdrv_ext_stackCfg_t));

            ret = reg_field_write(buf.port_cfg.unit, MANGO_STK_DBG_CTRLr, MANGO_STK_PORT_DEBUGf, &buf.stack_cfg.enable);

            break;
        case RTDRV_EXT_MIB_TX_ERR_CNTR_RESET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_mibCfg_t));
            ret = table_read(buf.mib_cfg.unit, MANGO_AUTO_RECOVERY_TXERR_CNTt, buf.mib_cfg.port, (uint32 *) &txerrCntr);
            ret = table_field_set(buf.mib_cfg.unit, MANGO_AUTO_RECOVERY_TXERR_CNTt,
                MANGO_AUTO_RECOVERY_TXERR_CNT_TXERR_CNTtf, &buf.mib_cfg.rst_val, (uint32 *) &txerrCntr);
            ret = table_write(buf.mib_cfg.unit, MANGO_AUTO_RECOVERY_TXERR_CNTt, buf.mib_cfg.port, (uint32 *) &txerrCntr);
            break;

    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
    /** TRUNK **/
        case RTDRV_EXT_TRUNK_RESET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_ext_trunkCfg_t));

            osal_memset(&trk_igr_ports, 0, sizeof(trk_igr_ports));
            osal_memset(&trk_egr_ports, 0, sizeof(trk_egr_ports));

            for(idx = 0; idx < 128; idx ++)
            {
                rtk_trunk_localPort_set (buf.trunk_cfg.unit, idx, &trk_igr_ports);
                rtk_trunk_egrPort_set(buf.trunk_cfg.unit, idx, &trk_egr_ports);
                rtk_trunk_distributionAlgorithmTypeBind_set(buf.trunk_cfg.unit, idx, BIND_TYPE_L2, 0);
                rtk_trunk_distributionAlgorithmTypeBind_set(buf.trunk_cfg.unit, idx, BIND_TYPE_IPV4, 0);
                rtk_trunk_distributionAlgorithmTypeBind_set(buf.trunk_cfg.unit, idx, BIND_TYPE_IPV6, 0);
                rtk_trunk_trafficSeparateEnable_set(buf.trunk_cfg.unit, idx, SEPARATE_KNOWN_MULTI, DISABLED);
                rtk_trunk_trafficSeparateEnable_set(buf.trunk_cfg.unit, idx, SEPARATE_FLOOD, DISABLED);
            }

            for(idx = 0; idx < RTK_MAX_NUM_OF_UNIT; idx ++)
            {
                HWP_PORT_TRAVS_EXCEPT_CPU(buf.trunk_cfg.unit, port)
                {
                    unit_port.devID = idx;
                    unit_port.port = port;
                    rtk_trunk_srcPortMap_set(buf.trunk_cfg.unit, unit_port, 0, 0);
                }
            }

            rtk_trunk_mode_set(buf.trunk_cfg.unit, TRUNK_MODE_STANDALONE);

            ret = RT_ERR_OK;
            break;
        case RTDRV_EXT_TRUNK_MEMBER_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_ext_trunkCfg_t));

            rtk_stack_devId_get(buf.trunk_cfg.unit, &myUnit);
            osal_memset(&ingressPorts, 0, sizeof(ingressPorts));

            for(idx = 0; idx < buf.trunk_cfg.trk_ports.num_ports; idx ++)
            {
                if(buf.trunk_cfg.trk_ports.egr_port[idx].devID == myUnit)
                {
                    ingressPorts.bits[buf.trunk_cfg.trk_ports.egr_port[idx].port/32] |= 1 << buf.trunk_cfg.trk_ports.egr_port[idx].port%32;
                }
            }

            rtk_trunk_localPort_set(buf.trunk_cfg.unit, buf.trunk_cfg.id, &ingressPorts);
            rtk_trunk_egrPort_set(buf.trunk_cfg.unit, buf.trunk_cfg.id, &buf.trunk_cfg.trk_ports);

            break;
    /** STACK **/
        case RTDRV_EXT_MIB_RST_VAL_SET:
            copy_from_user(&buf.stack_cfg, user, sizeof(rtdrv_ext_mibCfg_t));
            ret = reg_field_write(buf.mib_cfg.unit, MANGO_STAT_RSTr, MANGO_RST_MIB_VALf, &buf.mib_cfg.rst_val);
            break;
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
        case RTDRV_EXT_RATE_EGR_INCLUDE_CPU_TAG_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_EGBW_ENCAP_CTRLr,
                    MANGO_CPU_TAG_FEED_BACKf, &buf.switch_cfg.enable);
            break;
    /** SVLAN **/
    /** SWITCH **/
        case RTDRV_EXT_SWITCH_48PASS1_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_HALF_48PASS1_ENf, &buf.switch_cfg.half_48pass1);
            break;
        case RTDRV_EXT_SWITCH_MAC_48PASS1_DROP_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_MAC_48PASS1_DROP_ENf, &buf.switch_cfg.enable);
            break;
        case RTDRV_EXT_SWITCH_LIMITPAUSE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_LIMIT_PAUSE_ENf, &buf.switch_cfg.value);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_1G_100M_10M_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_1G_100M_10M_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_10G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_10G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_2P5G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_SEL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_2P5G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_5G_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_5G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_5G_SEL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_5G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            break;

        case RTDRV_EXT_SWITCH_IPGMINLEN_10M_100M_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_LIMIT_IPG_CFG_10M_100Mf, &buf.switch_cfg.min_ipg);
            break;

        case RTDRV_EXT_SWITCH_IPGMINLEN_1G_2_5G_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_LIMIT_IPG_CFG_1G_2P5Gf, &buf.switch_cfg.min_ipg);
            break;

        case RTDRV_EXT_SWITCH_BKPRES_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_BKPRES_MTHD_SELf, &buf.switch_cfg.bkpres);
            break;
        case RTDRV_EXT_SWITCH_BYPASSTXCRC_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_BYP_TX_CRCf, &buf.switch_cfg.bypass_tx_crc);
            break;
        case RTDRV_EXT_SWITCH_PASSALLMODE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_PASS_ALL_MODE_ENf, &buf.switch_cfg.pass_all_mode);
            break;
#if 0   /* removed, not support */
        case RTDRV_EXT_SWITCH_RXCHECKCRC_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_RX_CHK_CRC_ENf, &buf.switch_cfg.rx_check_crc);
            break;
#endif
        case RTDRV_EXT_SWITCH_PADDINGUNDSIZE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_PADDING_UND_SIZE_ENf, &buf.switch_cfg.enable);
            break;
        case RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_ENABLE_SET:
            if (!linkMon_wake_up)
            {
                copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
                ret = rtk_port_linkMon_enable(buf.switch_cfg.unit, 10000);
                linkMon_wake_up = 1;
            }
            break;
        case RTDRV_EXT_SWITCH_INTR_LINK_CHANGE_DISABLE_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = rtk_port_linkMon_disable(buf.switch_cfg.unit);
            linkMon_wake_up = 0;
            break;

        case RTDRV_EXT_SWITCH_PADDINCONTENT_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_MAC_L2_PADDING_SELr,
                    MANGO_PADDING_SELf, &buf.switch_cfg.value);
            break;

        case RTDRV_EXT_SWITCH_SW_QUERE_RESET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, MANGO_RST_GLB_CTRLr,
                    MANGO_SW_Q_RSTf, &buf.switch_cfg.value);
            break;

    /** NIC **/
        case RTDRV_EXT_NIC_PKT_SEND_SET:
            copy_from_user(&buf.nicSend_cfg, user, sizeof(rtdrv_ext_nicSendCfg_t));

        {
            uint8 pkt1[] = { /* Pause frame */
                0x01, 0x80, 0xc2, 0x00, 0x00, 0x01, 0x00, 0x1A, 0x1B, 0x33, 0x44, 0x55, 0x00, 0x00, 0x00, 0x01,
                0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            };
            uint8 *tx_buff;
            drv_nic_pkt_t *pPacket;

            pPacket = osal_alloc(sizeof(drv_nic_pkt_t));
            if (pPacket == NULL)
            {
                /* out of memory */
                ret = RT_ERR_FAILED;
                goto FAIL_EXIT;
            }

            tx_buff = osal_alloc(1600);
            if (tx_buff == NULL)
            {
                osal_free(pPacket);
                ret = RT_ERR_FAILED;
                goto FAIL_EXIT;
            }

            {
                if(buf.nicSend_cfg.isCpuTag == TRUE)
                    pPacket->as_txtag = 1;
                else
                    pPacket->as_txtag = 0;

            #if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
                if(buf.nicSend_cfg.isTrunkHash == TRUE)
                    pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_LOGICAL;
                else
                    pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_PHYISCAL;
            #endif

                pPacket->tx_tag.dst_port_mask       = buf.nicSend_cfg.txPortmask.bits[0];
                pPacket->tx_tag.dst_port_mask_1     = buf.nicSend_cfg.txPortmask.bits[1];

                pPacket->tx_tag.as_priority         = 1;
                pPacket->tx_tag.priority            = 7;
            }

            osal_memcpy(&pkt1[0], &buf.nicSend_cfg.dst_mac.octet[0], 6);
            osal_memcpy(&pkt1[6], &buf.nicSend_cfg.src_mac.octet[0], 6);


            /* raw packet */
            pPacket->buf_id = (void *)NULL;
            pPacket->head = tx_buff;
            pPacket->data = tx_buff + 2 + 12;
            pPacket->tail = tx_buff + 2 + 12 + 64;
            pPacket->end = tx_buff + 1600;
            pPacket->length = 60;
            pPacket->next = NULL;

            osal_memcpy(pPacket->data, pkt1, 60);

            if (RT_ERR_OK == drv_nic_pkt_tx(buf.nicSend_cfg.unit, pPacket, _nic_tx_callback, (void *)tx_buff))
            {

            }
            else
            {
                osal_free(pPacket);
                osal_free(tx_buff);
                ret = RT_ERR_FAILED;
                goto FAIL_EXIT;
            }
          }

            break;

        case RTDRV_EXT_NIC_PKT_SEND_PRI_SET:    /* sdk reset tc - reset DUT to default configuration */
            copy_from_user(&buf.nic_cfg, user, sizeof(rtdrv_ext_nicCfg_t));
          #if defined(CONFIG_SDK_RTL9310)
            {
                int32   ret, cnt = 0, i, j, priSize = buf.nic_cfg.end - buf.nic_cfg.start + 1;
                drv_nic_pkt_t *pPacket;

                if (RT_ERR_OK != drv_nic_pkt_alloc(buf.nic_cfg.unit, buf.nic_cfg.len, 0, &pPacket))
                {
                    osal_printf("[%s]: Alloc packet failed.\n", __FUNCTION__);
                    return RT_ERR_FAILED;
                }

                pPacket->length         = buf.nic_cfg.len;
                pPacket->tail           = pPacket->data + buf.nic_cfg.len;
                pPacket->txIncludeCRC   = TRUE;

                pPacket->as_txtag       = TRUE;

                /* Setting CPU TX tag or payload */
                pPacket->tx_tag.fwd_type        = NIC_FWD_TYPE_ALE;
                pPacket->tx_tag.dst_port_mask   = 0xFFFFFFFF;
                pPacket->tx_tag.dst_port_mask_1 = 0xFFFFFF;

                /* Setting DA/SA */
                for (i = 0; i < 12; i++)
                    pPacket->data[i] = 0;
                pPacket->data[5] = 0x01;
                pPacket->data[7] = 0xE0;
                pPacket->data[8] = 0x4C;
                pPacket->data[12] = 0x81;
                pPacket->data[13] = 0x00;
                pPacket->data[14] = 0x00;
                pPacket->data[15] = 0x01;

                for (i = 16, j = 0; i < buf.nic_cfg.len; i++, j++)
                    pPacket->data[i] = j & 0xff;


                while (1)
                {
                    pPacket->tx_tag.as_priority = 1;
                    pPacket->tx_tag.priority    = (cnt % priSize) + buf.nic_cfg.start;

                    pPacket->data[14] = (pPacket->tx_tag.priority % 8) << 5;

                    if ((ret = drv_nic_pkt_tx(buf.nic_cfg.unit, pPacket, _dummy_tx_callback, NULL)))
                    {
                        osal_printf("%s():%d  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        break;
                    }

                    if (buf.nic_cfg.num != 0)
                    {
                        cnt++;
                        if (cnt == buf.nic_cfg.num)
                            break;
                    }
                    else
                        break;
                }

                drv_nic_pkt_free(buf.nic_cfg.unit, pPacket);
            }
          #endif
            break;

        case RTDRV_EXT_NIC_RESET:
            copy_from_user(&buf.nicSend_cfg, user, sizeof(rtdrv_ext_nicSendCfg_t));
            drv_nic_reset(buf.nic_cfg.unit);
            break;

    /** Remote Access **/
        case RTDRV_EXT_REMOTE_ACCESS_REG_SET:

            copy_from_user(&buf.remoteAccess_cfg, user, sizeof(rtdrv_ext_remoteAccessCfg_t));

            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_UNITf, &buf.remoteAccess_cfg.targetUnit);
            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_ADDRf, &buf.remoteAccess_cfg.addr);
            val = 1; /*write*/
            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_RWOPf, &val);
            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_DATAr,
                MANGO_DATAf, &buf.remoteAccess_cfg.value);

            val = 1; /*exec*/
            ret = reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_EXECf, &val);

            break;

    /** Diag Register/Table access **/
        case RTDRV_EXT_DIAG_REGARRAY_SET:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = rtk_diag_regArray_set(buf.diag_cfg.unit, buf.diag_cfg.reg, buf.diag_cfg.idx1, buf.diag_cfg.idx2, (uint32 *)&buf.diag_cfg.data);
            break;

        case RTDRV_EXT_DIAG_REGARRAYFIELD_SET:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = rtk_diag_regArrayField_set(buf.diag_cfg.unit, buf.diag_cfg.reg, buf.diag_cfg.idx1, buf.diag_cfg.idx2, buf.diag_cfg.field, (uint32 *)&buf.diag_cfg.data);
            break;

        case RTDRV_EXT_DIAG_TABLEENTRY_SET:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = rtk_diag_tableEntry_set(buf.diag_cfg.unit, buf.diag_cfg.table, buf.diag_cfg.addr, (uint32 *)&buf.diag_cfg.data);
            break;

        case RTDRV_EXT_DIAG_SEND_VAL:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
          {
            drv_generalCtrlGpio_devConf_t   gpioDevCfg;
            uint32 devId = 7;
            int32 i;

            osal_memset(&gpioDevCfg, 0, sizeof(drv_generalCtrlGpio_devConf_t));
            gpioDevCfg.direction            = GPIO_DIR_OUT;
            gpioDevCfg.default_value        = 0;
            gpioDevCfg.ext_gpio.access_mode = EXT_GPIO_ACCESS_MODE_MDC;
            gpioDevCfg.ext_gpio.address     = 0; //please check the correct address of 8231.
            ret = drv_generalCtrlGPIO_dev_init(buf.diag_cfg.unit, devId, &gpioDevCfg);

            drv_generalCtrlGPIO_direction_set(buf.diag_cfg.unit, devId, 8, GPIO_DIR_OUT); //pinId assign to 8 and 9 for your case
            drv_generalCtrlGPIO_direction_set(buf.diag_cfg.unit, devId, 9, GPIO_DIR_OUT); //pinId assign to 8 and 9 for your case

            drv_generalCtrlGPIO_devEnable_set(buf.diag_cfg.unit, devId, ENABLED);
            for(i = 31; i >= 0; i --)
            {
                drv_generalCtrlGPIO_dataBit_set(buf.diag_cfg.unit, devId, 9, (buf.diag_cfg.field & (1 << i)) >> i); //data is 1 or 0
                osal_printf("send %d\n", (buf.diag_cfg.field & (1 << i)) >> i);
                drv_generalCtrlGPIO_dataBit_set(buf.diag_cfg.unit, devId, 8, 1); //data is 1 or 0
                osal_time_udelay(1);
                drv_generalCtrlGPIO_dataBit_set(buf.diag_cfg.unit, devId, 8, 0); //data is 1 or 0
                osal_time_udelay(1);
            }
          }

            break;

        case RTDRV_EXT_SDK_TC_RESET:    /* sdk reset tc - reset DUT to default configuration */
            copy_from_user(&buf.sdk_cfg, user, sizeof(rtdrv_ext_sdkCfg_t));
            ret = _sdk_tc_reset(buf.sdk_cfg.unit);
            break;

    /** MPLS **/
    /** EEE **/
    /** IOL **/
        case RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_IOL_MAX_RETRY_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_ERROR_LENGTH_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IOL_LEN_ERR_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_LATE_COLLISION_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_LATE_COLI_DROP_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_MAX_LENGTH_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IOL_MAX_LEN_ENf, &buf.iol_cfg.enable);
            break;

    /** MODEL TEST **/
#ifdef CONFIG_SDK_MODEL_MODE
        case RTDRV_EXT_MODEL_TEST_SET:
            copy_from_user(&buf.model_cfg, user, sizeof(rtdrv_ext_modelCfg_t));
            vmac_setCaredICType(buf.model_cfg.caredType);
            ret = tc_exec(buf.model_cfg.startID, buf.model_cfg.endID);
            vmac_setCaredICType(CARE_TYPE_BOTH);
            break;

        case RTDRV_EXT_MODEL_TEST_UNIT_SET:
            copy_from_user(&buf.model_cfg, user, sizeof(rtdrv_ext_modelCfg_t));
            tc_unit_set(buf.model_cfg.unit);
            vmac_setCaredICType(buf.model_cfg.caredType);
            ret = tc_exec(buf.model_cfg.startID, buf.model_cfg.endID);
            vmac_setCaredICType(CARE_TYPE_BOTH);
            break;
#endif

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

        case RTDRV_EXT_TESTIO_SET_OCP:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_set_ocp(buf.diag_cfg.unit, buf.diag_cfg.reg, buf.diag_cfg.data[0]);
            break;

        case RTDRV_EXT_TESTIO_SET_EPHY:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_set_ePhy(buf.diag_cfg.unit, buf.diag_cfg.reg, buf.diag_cfg.data[0]);
            break;

        case RTDRV_EXT_TESTIO_SET_EFUSE_CHECK:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_set_eFuse_check(buf.diag_cfg.unit);
            break;

        case RTDRV_EXT_TESTIO_SET_EFUSE:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_set_eFuse(buf.diag_cfg.unit, buf.diag_cfg.idx1, buf.diag_cfg.reg, buf.diag_cfg.data[0]);
            break;

#ifdef CONFIG_SDK_FPGA_PLATFORM
        case RTDRV_EXT_FPGA_INIT_SET:
            copy_from_user(&buf.fpga_cfg, user, sizeof(rtdrv_ext_fpgaCfg_t));
            ret = _fpga_init(buf.fpga_cfg.unit, buf.fpga_cfg.fpgaVer);
            break;

        case RTDRV_EXT_FPGA_TEST:
            copy_from_user(&buf.fpga_cfg, user, sizeof(rtdrv_ext_fpgaCfg_t));
            {
                uint32              unit = buf.fpga_cfg.unit;
                uint32              i, j, port;
                uint32              ebl[4][4], val, data[2], data1[2], mask[2], mask1[2], rate[4][4], burst[4][4];
                rtk_l2_ucastAddr_t  ucastEntry;
                rtk_mac_t           mac;
                rtk_vlan_t          vid;
                rtk_portmask_t      member_portmask, untag_portmask;
                rtk_l2_mcastAddr_t  mcastAddr;
                rtk_rate_assuredMode_t cfgMode[4][4];
                rtk_acl_phase_t     phase = ACL_PHASE_VACL;
                vlan_entry_t        vlan_entry;
                vlan_untag_entry_t  vlan_untag_entry;
                egr_qBw_entry_t     egrQEntry;
                const static uint16 egrQBwBurst_fieldidx[] = {MANGO_EGR_Q_BW_MAX_LB_BURST_Q0tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q1tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q2tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q3tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q4tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q5tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q6tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q7tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q8tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q9tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q10tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q11tf};

                /* Table access test */
                /* L2 */
                osal_memset(&ucastEntry, 0, sizeof(rtk_l2_ucastAddr_t));
                osal_memset(&member_portmask, 0, sizeof(rtk_portmask_t));
                osal_memset(&untag_portmask, 0, sizeof(rtk_portmask_t));
                mac.octet[0] = 0x1a;
                mac.octet[1] = 0x11;
                mac.octet[2] = 0x22;
                mac.octet[3] = 0x33;
                mac.octet[4] = 0x44;
                mac.octet[5] = 0x55;
                ucastEntry.vid = 25;
                ucastEntry.port = 1;
                osal_memcpy(&ucastEntry.mac, &mac, ETHER_ADDR_LEN);
                if ((ret = rtk_l2_addr_add(unit, &ucastEntry)))
                {
                    osal_printf("%s():%d  FAIL! Adding L2 entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((ret = rtk_l2_addr_get(unit, &ucastEntry)))
                {
                    osal_printf("%s():%d  FAIL! Getting L2 entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((ret = osal_memcmp(&ucastEntry.mac, &mac, ETHER_ADDR_LEN)))
                {
                    osal_printf("%s():%d  FAIL! Compareing L2 entry failed.\n", __FUNCTION__, __LINE__);
                }
                /* Power Saving Reset Test: Check whether table content exist */
                val = 1;
                if ((ret = reg_field_write(unit, MANGO_PS_SLOW_SYSCLK_CTRLr, MANGO_SLOW_DOWN_SYSCLK_ENf, &val)) != RT_ERR_OK)
                {
                    osal_printf("%s():%d  Error! Setting SLOW_DOWN_SYSCLK_EN to %d failed.  ret:%#x\n", __FUNCTION__, __LINE__, val, ret);
                }
                if ((ret = rtk_l2_addr_get(unit, &ucastEntry)))
                {
                    osal_printf("%s():%d  FAIL! Getting L2 entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((ret = osal_memcmp(&ucastEntry.mac, &mac, ETHER_ADDR_LEN)))
                {
                    osal_printf("%s():%d  FAIL! Compareing L2 entry failed.\n", __FUNCTION__, __LINE__);
                }
                val = 0;
                if ((ret = reg_field_write(unit, MANGO_PS_SLOW_SYSCLK_CTRLr, MANGO_SLOW_DOWN_SYSCLK_ENf, &val)) != RT_ERR_OK)
                {
                    osal_printf("%s():%d  Error! Setting SLOW_DOWN_SYSCLK_EN to %d failed.  ret:%#x\n", __FUNCTION__, __LINE__, val, ret);
                }
                if ((ret = rtk_l2_addr_get(unit, &ucastEntry)))
                {
                    osal_printf("%s():%d  FAIL! Getting L2 entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((ret = osal_memcmp(&ucastEntry.mac, &mac, ETHER_ADDR_LEN)))
                {
                    osal_printf("%s():%d  FAIL! Compareing L2 entry failed After switch SLOW_DOWN_SYSCLK_EN.\n", __FUNCTION__, __LINE__);
                }
                if ((ret = rtk_l2_addr_del(unit, 25, &mac)))
                {
                    osal_printf("%s():%d  FAIL! Deleting L2 entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }


                /* Multicast Portmask Table */
                osal_memset(&mcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
                mcastAddr.rvid = 1;
                mcastAddr.mac.octet[0] = 0x01;
                mcastAddr.mac.octet[1] = 0x11;
                mcastAddr.mac.octet[2] = 0x22;
                mcastAddr.mac.octet[3] = 0x33;
                mcastAddr.mac.octet[4] = 0x44;
                mcastAddr.mac.octet[5] = 0x55;
                mcastAddr.portmask.bits[0] = 0x1;
                mcastAddr.portmask.bits[1] = 0x300000;
                if ((ret = rtk_l2_mcastAddr_add(unit, &mcastAddr)))
                {
                    osal_printf("%s():%d  FAIL! Adding L2 multicast entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                mcastAddr.portmask.bits[0] = 0x0;
                mcastAddr.portmask.bits[1] = 0x0;
                if ((ret = rtk_l2_mcastAddr_get(unit, &mcastAddr)))
                {
                    osal_printf("%s():%d  FAIL! Getting L2 entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if (mcastAddr.portmask.bits[0] != 0x1 || mcastAddr.portmask.bits[1] != 0x300000)
                {
                    osal_printf("%s():%d  FAIL! Compareing Multicast Portmask Table entry failed.\n", __FUNCTION__, __LINE__);
                }
                if ((ret = rtk_l2_mcastAddr_del(unit, mcastAddr.rvid, &mcastAddr.mac)))
                {
                    osal_printf("%s():%d  FAIL! Deleting L2 multicast entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }


                /* VLAN */
                osal_memset(&vlan_entry, 0, sizeof(vlan_entry_t));
                osal_memset(&vlan_untag_entry, 0, sizeof(vlan_untag_entry_t));
                vid = 28;
                if ((ret = rtk_vlan_create(unit, vid)))
                {
                    osal_printf("%s():%d  FAIL! Creating VLAN entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                member_portmask.bits[0] = 0xffffffff;;
                member_portmask.bits[1] = 0x1ffffff;;
                untag_portmask.bits[0] = 0xacacacac;;
                untag_portmask.bits[1] = 0x1ffffff;;
                if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_MBRtf, member_portmask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                    return ret;
                }
                if ((ret = table_write(unit, MANGO_VLANt, vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, MANGO_VLAN_UNTAGt, MANGO_VLAN_UNTAG_UNTAGtf, untag_portmask.bits, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                    return ret;
                }
                if ((ret = table_write(unit, MANGO_VLAN_UNTAGt, vid, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                    return ret;
                }
                if ((ret = rtk_vlan_port_get(unit, vid, &member_portmask, &untag_portmask)))
                {
                    osal_printf("%s():%d  FAIL! Getting VLAN entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((member_portmask.bits[0] != 0xffffffff) || (member_portmask.bits[1] != 0x1ffffff) ||
                    (untag_portmask.bits[0] != 0xacacacac) || (untag_portmask.bits[1] != 0x1ffffff))
                {
                    osal_printf("%s():%d  FAIL! Compareing VLAN entry failed.\n", __FUNCTION__, __LINE__);
                }
                /* Power Saving Reset Test: Check whether table content exist */
                val = 1;
                if ((ret = reg_field_write(unit, MANGO_PS_SLOW_SYSCLK_CTRLr, MANGO_SLOW_DOWN_SYSCLK_ENf, &val)) != RT_ERR_OK)
                {
                    osal_printf("%s():%d  Error! Setting SLOW_DOWN_SYSCLK_EN to %d failed.  ret:%#x\n", __FUNCTION__, __LINE__, val, ret);
                }
                if ((ret = rtk_vlan_port_get(unit, vid, &member_portmask, &untag_portmask)))
                {
                    osal_printf("%s():%d  FAIL! Getting VLAN entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((member_portmask.bits[0] != 0xffffffff) || (member_portmask.bits[1] != 0x1ffffff) ||
                    (untag_portmask.bits[0] != 0xacacacac) || (untag_portmask.bits[1] != 0x1ffffff))
                {
                    osal_printf("%s():%d  FAIL! Compareing VLAN entry failed.\n", __FUNCTION__, __LINE__);
                }
                val = 0;
                if ((ret = reg_field_write(unit, MANGO_PS_SLOW_SYSCLK_CTRLr, MANGO_SLOW_DOWN_SYSCLK_ENf, &val)) != RT_ERR_OK)
                {
                    osal_printf("%s():%d  Error! Setting SLOW_DOWN_SYSCLK_EN to %d failed.  ret:%#x\n", __FUNCTION__, __LINE__, val, ret);
                }
                if ((ret = rtk_vlan_port_get(unit, vid, &member_portmask, &untag_portmask)))
                {
                    osal_printf("%s():%d  FAIL! Getting VLAN entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((member_portmask.bits[0] != 0xffffffff) || (member_portmask.bits[1] != 0x1ffffff) ||
                    (untag_portmask.bits[0] != 0xacacacac) || (untag_portmask.bits[1] != 0x1ffffff))
                {
                    osal_printf("%s():%d  FAIL! Compareing VLAN entry failed.\n", __FUNCTION__, __LINE__);
                }
                if ((ret = rtk_vlan_destroy(unit, vid)))
                {
                    osal_printf("%s():%d  FAIL! Creating VLAN entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }


                /* ACL */

                if ((ret = rtk_pie_phase_set(unit, 0, PIE_PHASE_VACL)))
                {
                    osal_printf("%s():%d  FAIL! Set block phase failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                osal_memset(data, 0, 2);
                osal_memset(mask, 0, 2);
                if ((ret = rtk_acl_ruleEntryField_read(unit, phase, 0, USER_FIELD_SPM, (uint8 *)data, (uint8 *)mask)))
                {
                    osal_printf("%s():%d  FAIL! Getting ACL entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                data1[0] = 0x0;
                data1[1] = 0xFFFFFFFF;
                mask1[0] = 0x0;
                mask1[1] = 0xFFFFFFFF;
                if ((ret = rtk_acl_ruleEntryField_write(unit, phase, 0, USER_FIELD_SPM, (uint8 *)data1, (uint8 *)mask1)))
                {
                    osal_printf("%s():%d  FAIL! Adding ACL entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }
                if ((ret = rtk_acl_ruleEntryField_read(unit, phase, 0, USER_FIELD_SPM, (uint8 *)data, (uint8 *)mask)))
                {
                    osal_printf("%s():%d  FAIL! Getting ACL entry failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                }

                if (0 != memcmp(data, data1, 2) || 0 != memcmp(mask, mask1, 2))
                {
                    osal_printf("%s():%d  FAIL! Compareing ACL entry failed.\n", __FUNCTION__, __LINE__);
                    osal_printf("data 0x%x 0x%x mask 0x%x 0x%x\n", data[0], data[1], mask[0], mask[1]);
                }

                /* Egr Queue */ /* test PORT 0, 52, 53*/
                for (i = 0; i < 3; i++)
                {
                    port = i;
                    if (i == 1)
                        port = 52;
                    else if (i == 2)
                        port = 53;
                    for (j = 0; j < 4; j++)
                    {
                        if ((ret = rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port, j, &ebl[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueBwCtrlEnable_set(unit, port, j, !ebl[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueBwCtrlEnable_get(unit, port, j, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if (val != (!ebl[i][j]))
                            osal_printf("%s():%d  FAIL! Compareing Egr Queue failed.\n", __FUNCTION__, __LINE__);
                        else
                        {
                            if ((ret = rtk_rate_portEgrQueueBwCtrlEnable_set(unit, port, j, ebl[i][j])))
                            {
                                osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                            }
                        }

                        if ((ret = rtk_rate_portEgrQueueBwCtrlBurstSize_get(unit, port, j, &burst[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        /* write EGR_Q_BW table */
                        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));
                        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
                        {
                            osal_printf("%s():%d  FAIL! Reading EGR_Q_BW table failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        val = !burst[i][j];
                        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwBurst_fieldidx[j],
                                        &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
                        {
                            osal_printf("%s():%d  FAIL! Writing EGR_Q_BW table failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
                        {
                            osal_printf("%s():%d  FAIL! Writing EGR_Q_BW table failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueBwCtrlBurstSize_get(unit, port, j, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if (val != (!burst[i][j]))
                            osal_printf("%s():%d  FAIL! Compareing Egr Queue failed.\n", __FUNCTION__, __LINE__);
                        else
                        {
                            if ((ret = rtk_rate_portEgrQueueBwCtrlBurstSize_set(unit, port, j, burst[i][j])))
                            {
                                osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                            }
                        }

                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_get(unit, port, j, &ebl[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_set(unit, port, j, !ebl[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_get(unit, port, j, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if (val != (!ebl[i][j]))
                            osal_printf("%s():%d  FAIL! Compareing Egr Queue failed.\n", __FUNCTION__, __LINE__);
                        else
                        {
                            if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlEnable_set(unit, port, j, ebl[i][j])))
                            {
                                osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                            }
                        }

                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlRate_get(unit, port, j, &rate[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlRate_set(unit, port, j, !rate[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlRate_get(unit, port, j, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if (val != (!rate[i][j]))
                            osal_printf("%s():%d  FAIL! Compareing Egr Queue failed.\n", __FUNCTION__, __LINE__);
                        else
                        {
                            if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlRate_set(unit, port, j, rate[i][j])))
                            {
                                osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                            }
                        }

                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_get(unit, port, j, &cfgMode[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_set(unit, port, j, !cfgMode[i][j])))
                        {
                            osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_get(unit, port, j, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                        }
                        if (val != (!cfgMode[i][j]))
                            osal_printf("%s():%d  FAIL! Compareing Egr Queue failed.\n", __FUNCTION__, __LINE__);
                        else
                        {
                            if ((ret = rtk_rate_portEgrQueueAssuredBwCtrlMode_set(unit, port, j, cfgMode[i][j])))
                            {
                                osal_printf("%s():%d  FAIL! Writing Egr Queue failed.  ret:%#x\n", __FUNCTION__, __LINE__, ret);
                            }
                        }
                    }
                }


                /* MSTI */
                for (i = 0; i < 3; i++)
                {
                    port = i;
                    if (i == 1)
                        port = 52;
                    else if (i == 2)
                        port = 53;
                    for (j = 0; j < 4; j++)
                    {
                        rtk_stp_mstpInstance_create(unit, j);
                        if ((ret = rtk_stp_mstpState_set(unit, j, port, STP_STATE_BLOCKING)))
                        {
                            osal_printf("%s():%d  FAIL! Writing MSTI failed.  ret:%#x  MSTI:%d port:%d\n", __FUNCTION__, __LINE__, ret, j, port);
                            goto FAIL_EXIT;
                        }
                        if ((ret = rtk_stp_mstpState_get(unit, j, port, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading MSTI failed.  ret:%#x  MSTI:%d port:%d\n", __FUNCTION__, __LINE__, ret, j, port);
                            goto FAIL_EXIT;
                        }
                        if (val != STP_STATE_BLOCKING)
                            osal_printf("%s():%d  FAIL! Compareing MSTI failed. MSTI[%d].state[%d]:%d != STP_STATE_BLOCKING\n", __FUNCTION__, __LINE__, j, port, val);


                        if ((ret = rtk_stp_mstpState_set(unit, j, port, STP_STATE_FORWARDING)))
                        {
                            osal_printf("%s():%d  FAIL! Writing MSTI failed.  ret:%#x  MSTI:%d port:%d\n", __FUNCTION__, __LINE__, ret, j, port);
                            goto FAIL_EXIT;
                        }
                        if ((ret = rtk_stp_mstpState_get(unit, j, port, &val)))
                        {
                            osal_printf("%s():%d  FAIL! Reading MSTI failed.  ret:%#x  MSTI:%d port:%d\n", __FUNCTION__, __LINE__, ret, j, port);
                            goto FAIL_EXIT;
                        }
                        if (val != STP_STATE_FORWARDING)
                            osal_printf("%s():%d  FAIL! Compareing MSTI failed. MSTI[%d].state[%d]:%d != STP_STATE_FORWARDING\n", __FUNCTION__, __LINE__, j, port, val);

                        rtk_stp_mstpInstance_destroy(unit, j);
                    }
                }
            }
#endif

        default:
            break;
    }


FAIL_EXIT:

    copy_to_user(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    return 0;
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
    rtdrv_ext_union_t   buf;
    int32               ret = RT_ERR_FAILED;
    auto_rcvy_txerr_cnt_entry_t txerrCntr;
    uint32              val;

    osal_memset(&txerrCntr, 0, sizeof(auto_rcvy_txerr_cnt_entry_t));

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
        case RTDRV_EXT_L2_AGING_UNIT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_read(buf.l2_cfg.unit, MANGO_L2_AGE_CTRLr, MANGO_AGE_UNITf, &buf.l2_cfg.aging_time);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_ext_l2Cfg_t));
            break;

        case RTDRV_EXT_L2_UC_SIZE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));

            ret = table_size_get(buf.l2_cfg.unit, MANGO_L2_UCt, &buf.l2_cfg.data);

            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_ext_l2Cfg_t));
            break;

        case RTDRV_EXT_L2_CMA_ENABLE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_read(buf.l2_cfg.unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &buf.l2_cfg.enable);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_ext_l2Cfg_t));
            break;

    /** ACL **/
        case RTDRV_EXT_ACL_MIRROR_SFLOW_BYPASS_POST_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_ext_aclCfg_t));
            ret = reg_field_read(buf.acl_cfg.unit, MANGO_PIE_ENCAP_CTRLr, MANGO_EGR_MIR_SFLOW_BYPASS_POSTf, &buf.acl_cfg.enable);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_ext_aclCfg_t));
            break;

    /** PORT **/
        case RTDRV_EXT_PORT_MACFORCESTATE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret =  reg_array_field_read(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINK_ENf, &buf.port_cfg.state);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));

        case RTDRV_EXT_PORT_MACFORCELINK_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_read(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_FORCE_LINKf, &buf.port_cfg.state);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;

        case RTDRV_EXT_PORT_MACFORCEFLOWCTRL_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_read(buf.port_cfg.unit, MANGO_MAC_FORCE_MODE_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_SMI_RX_PAUSE_ENf, &buf.port_cfg.state);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_field_read(buf.port_cfg.unit, MANGO_SC_DRAIN_OUT_THRr, MANGO_DRAIN_OUT_THRf, &buf.port_cfg.full_th);
            ret = reg_field_read(buf.port_cfg.unit, MANGO_SC_DRAIN_OUT_THRr, MANGO_DRAIN_OUT_THR_Hf, &buf.port_cfg.half_th);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_read(buf.port_cfg.unit, MANGO_SC_PORT_TIMERr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_CNGST_SUST_TMR_LMTf, &buf.port_cfg.full_sec);
            ret = reg_array_field_read(buf.port_cfg.unit, MANGO_SC_PORT_TIMERr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_CNGST_SUST_TMR_LMT_Hf, &buf.port_cfg.half_sec);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;
    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
        case RTDRV_EXT_MIB_RST_VAL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_mibCfg_t));
            ret = reg_field_read(buf.mib_cfg.unit, MANGO_STAT_RSTr, MANGO_RST_MIB_VALf, &buf.mib_cfg.rst_val);
            copy_to_user(user, &buf.mib_cfg, sizeof(rtdrv_ext_mibCfg_t));
            break;
        case RTDRV_EXT_MIB_TX_ERR_CNTR_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_mibCfg_t));
            ret = table_read(buf.mib_cfg.unit, MANGO_AUTO_RECOVERY_TXERR_CNTt, buf.mib_cfg.port, (uint32 *) &txerrCntr);
            ret = table_field_get(buf.mib_cfg.unit, MANGO_AUTO_RECOVERY_TXERR_CNTt,
                MANGO_AUTO_RECOVERY_TXERR_CNT_TXERR_CNTtf, &buf.mib_cfg.cntr, (uint32 *) &txerrCntr);
            copy_to_user(user, &buf.mib_cfg, sizeof(rtdrv_ext_mibCfg_t));
            break;

    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
    /** TRUNK **/
        case RTDRV_EXT_TRUNK_LOCAL_ENTRY_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_ext_trunkCfg_t));
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_NUM_TX_CANDIf, &buf.trunk_cfg.localEntry.num_tx_candi);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_L2_HASH_MSK_IDXf, &buf.trunk_cfg.localEntry.hash_msk_idx);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT7f, &buf.trunk_cfg.localEntry.trk_port7);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT6f, &buf.trunk_cfg.localEntry.trk_port6);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT5f, &buf.trunk_cfg.localEntry.trk_port5);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT4f, &buf.trunk_cfg.localEntry.trk_port4);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT3f, &buf.trunk_cfg.localEntry.trk_port3);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT2f, &buf.trunk_cfg.localEntry.trk_port2);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT1f, &buf.trunk_cfg.localEntry.trk_port1);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_TRK_PORT0f, &buf.trunk_cfg.localEntry.trk_port0);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_SEP_FLOOD_PORTf, &buf.trunk_cfg.localEntry.sep_dlf_bcast_port);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_SEP_KWN_MC_PORTf, &buf.trunk_cfg.localEntry.sep_kwn_mc_port);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_NUM_AVL_TX_CANDIf, &buf.trunk_cfg.localEntry.num_avl_tx_candi);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT7f, &buf.trunk_cfg.localEntry.avl_trk_port7);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT6f, &buf.trunk_cfg.localEntry.avl_trk_port6);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT5f, &buf.trunk_cfg.localEntry.avl_trk_port5);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT4f, &buf.trunk_cfg.localEntry.avl_trk_port4);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT3f, &buf.trunk_cfg.localEntry.avl_trk_port3);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT2f, &buf.trunk_cfg.localEntry.avl_trk_port2);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT1f, &buf.trunk_cfg.localEntry.avl_trk_port1);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_TRK_PORT0f, &buf.trunk_cfg.localEntry.avl_trk_port0);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_SEP_FLOOD_PORTf, &buf.trunk_cfg.localEntry.avl_sep_dlf_bcast_port);
            ret = reg_array_field_read(buf.trunk_cfg.unit, MANGO_TRK_LOCAL_TBLr, REG_ARRAY_INDEX_NONE,
                    buf.trunk_cfg.id, MANGO_AVL_SEP_KWN_MC_PORTf, &buf.trunk_cfg.localEntry.avl_sep_kwn_mc_port);

            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_ext_trunkCfg_t));
            break;
    /** STACK **/
        case RTDRV_EXT_STACK_DEBUG_GET:
            copy_from_user(&buf.stack_cfg, user, sizeof(rtdrv_ext_stackCfg_t));

            ret = reg_field_read(buf.port_cfg.unit, MANGO_STK_DBG_CTRLr, MANGO_STK_PORT_DEBUGf, &buf.stack_cfg.enable);

            copy_to_user(user, &buf.stack_cfg, sizeof(rtdrv_ext_stackCfg_t));

            break;
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
        case RTDRV_EXT_RATE_EGR_INCLUDE_CPU_TAG_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_EGBW_ENCAP_CTRLr,
                    MANGO_CPU_TAG_FEED_BACKf, &buf.switch_cfg.enable);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
    /** SVLAN **/
    /** SWITCH **/
        case RTDRV_EXT_SWITCH_48PASS1_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_HALF_48PASS1_ENf, &buf.switch_cfg.half_48pass1);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_LIMITPAUSE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_LIMIT_PAUSE_ENf, &buf.switch_cfg.value);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_1G_100M_10M_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_1G_100M_10M_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_10G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_10G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
         case RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_2P5G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_2_5G_SEL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_2P5G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_5G_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_5G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;

        case RTDRV_EXT_SWITCH_IPGCOMSTN_5G_SEL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IPG_5G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;

        case RTDRV_EXT_SWITCH_IPGMINLEN_10M_100M_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_LIMIT_IPG_CFG_10M_100Mf, &buf.switch_cfg.min_ipg);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;

        case RTDRV_EXT_SWITCH_IPGMINLEN_1G_2_5G_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_LIMIT_IPG_CFG_1G_2P5Gf, &buf.switch_cfg.min_ipg);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_BKPRES_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_BKPRES_MTHD_SELf, &buf.switch_cfg.bkpres);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_BYPASSTXCRC_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_BYP_TX_CRCf, &buf.switch_cfg.bypass_tx_crc);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_PASSALLMODE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_PASS_ALL_MODE_ENf, &buf.switch_cfg.pass_all_mode);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
#if 0   /* removed, not support */
        case RTDRV_EXT_SWITCH_RXCHECKCRC_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, MANGO_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_RX_CHK_CRC_ENf, &buf.switch_cfg.rx_check_crc);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
#endif
        case RTDRV_EXT_SWITCH_PADDINGUNDSIZE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, MANGO_PADDING_UND_SIZE_ENf, &buf.switch_cfg.enable);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;

        case RTDRV_EXT_SWITCH_PADDINCONTENT_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, MANGO_MAC_L2_PADDING_SELr,
                    MANGO_PADDING_SELf, &buf.switch_cfg.value);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;

    /** NIC **/
    /** MPLS **/
    /** EEE **/
        case RTDRV_EXT_EEE_PORT_STATUS_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_array_field_read(buf.eee_cfg.unit,
                          MANGO_EEE_PORT_TX_STSr,
                          buf.eee_cfg.port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_EEEP_TX_STSf,
                          &buf.eee_cfg.txState);
            ret = reg_array_field_read(buf.eee_cfg.unit,
                          MANGO_EEE_PORT_RX_STSr,
                          buf.eee_cfg.port,
                          REG_ARRAY_INDEX_NONE,
                          MANGO_EEE_EEEP_RX_STSf,
                          &buf.eee_cfg.rxState);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
    /** IOL **/
        case RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_IOL_MAX_RETRY_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_ERROR_LENGTH_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IOL_LEN_ERR_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_LATE_COLLISION_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, MANGO_MAC_GLB_CTRLr,
                    MANGO_LATE_COLI_DROP_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_MAX_LENGTH_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
                    MANGO_IOL_MAX_LEN_ENf, &buf.iol_cfg.enable);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_TESTIO_GET_OCP:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_get_ocp(buf.diag_cfg.unit, buf.diag_cfg.reg, &buf.diag_cfg.data[0]);
            copy_to_user(user, &buf.diag_cfg, sizeof(rtdrv_ext_diagCfg_t));
            break;
        case RTDRV_EXT_TESTIO_GET_EPHY:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_get_ePhy(buf.diag_cfg.unit, buf.diag_cfg.reg, &buf.diag_cfg.data[0]);
            copy_to_user(user, &buf.diag_cfg, sizeof(rtdrv_ext_diagCfg_t));
            break;
        case RTDRV_EXT_TESTIO_GET_EFUSE:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = _testio_get_eFuse(buf.diag_cfg.unit, buf.diag_cfg.idx1, &buf.diag_cfg.reg, &buf.diag_cfg.data[0]);
            copy_to_user(user, &buf.diag_cfg, sizeof(rtdrv_ext_diagCfg_t));
            break;

#ifdef CONFIG_SDK_FPGA_PLATFORM
        case RTDRV_EXT_FPGA_INFO_GET:
            copy_from_user(&buf.fpga_cfg, user, sizeof(rtdrv_ext_fpgaCfg_t));
            ret = _fpga_info_get(buf.fpga_cfg.unit, &buf.fpga_cfg.rtl_svn_rev, &buf.fpga_cfg.build_date, &buf.fpga_cfg.build_time, &buf.fpga_cfg.fpga_type_and_reg_profile_ver);
            copy_to_user(user, &buf.fpga_cfg, sizeof(rtdrv_ext_fpgaCfg_t));
            break;
#endif

    /** Remote Access **/
        case RTDRV_EXT_REMOTE_ACCESS_REG_GET:

            copy_from_user(&buf.remoteAccess_cfg, user, sizeof(rtdrv_ext_remoteAccessCfg_t));

            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_UNITf, &buf.remoteAccess_cfg.targetUnit);
            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_ADDRf, &buf.remoteAccess_cfg.addr);
            val = 0; /*read*/
            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_RWOPf, &val);

            val = 1; /*exec*/
            reg_field_write(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_EXECf, &val);

            ret = reg_field_read(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_DATAr,
                MANGO_DATAf, &buf.remoteAccess_cfg.value);
            copy_to_user(user, &buf.remoteAccess_cfg, sizeof(rtdrv_ext_remoteAccessCfg_t));

            reg_field_read(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr,
                MANGO_EXECf, &val);

            reg_read(buf.remoteAccess_cfg.unit, MANGO_RMT_REG_ACCESS_CTRLr, &val);

            break;


    /** Diag Register/Table access **/
        case RTDRV_EXT_DIAG_REGARRAY_GET:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = rtk_diag_regArray_get(buf.diag_cfg.unit, buf.diag_cfg.reg, buf.diag_cfg.idx1, buf.diag_cfg.idx2, (uint32 *)&buf.diag_cfg.data);
            copy_to_user(user, &buf.diag_cfg, sizeof(rtdrv_ext_diagCfg_t));
            break;

        case RTDRV_EXT_DIAG_REGARRAYFIELD_GET:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = rtk_diag_regArrayField_get(buf.diag_cfg.unit, buf.diag_cfg.reg, buf.diag_cfg.idx1, buf.diag_cfg.idx2, buf.diag_cfg.field, (uint32 *)&buf.diag_cfg.data);
            copy_to_user(user, &buf.diag_cfg, sizeof(rtdrv_ext_diagCfg_t));
            break;

        case RTDRV_EXT_DIAG_TABLEENTRY_GET:
            copy_from_user(&buf.diag_cfg, user, sizeof(rtdrv_ext_diagCfg_t));
            ret = rtk_diag_tableEntry_get(buf.diag_cfg.unit, buf.diag_cfg.table, buf.diag_cfg.addr, (uint32 *)&buf.diag_cfg.data);
            copy_to_user(user, &buf.diag_cfg, sizeof(rtdrv_ext_diagCfg_t));
            break;

        case RTDRV_EXT_THERMAL_CURRENT_GET:
            copy_from_user(&buf.thermal_cfg, user, sizeof(rtdrv_ext_thermalCfg_t));

            {
                uint32 tmp = 1;

                RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x00B0, 31, 0, 1), ret);
                RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8230, 31, 0, 1), ret);

                if(buf.thermal_cfg.isLow)
                {
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0080, 31, 0, 0x7CB9428), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0084, 31, 10, 0x3734CC), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0084, 2, 0, 0x1), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0088, 28, 28, 0x1), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8200, 31, 0, 0x7CB9428), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8204, 31, 10, 0x3734CC), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8204, 2, 0, 0x1), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8208, 28, 28, 0x1), ret);
                }
                else
                {
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0080, 31, 0, 0x7467000), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0084, 31, 10, 0x380FFF), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0084, 2, 0, 0x1), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x0088, 28, 28, 0x1), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8200, 31, 0, 0x7467000), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8204, 31, 10, 0x380FFF), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8204, 2, 0, 0x1), ret);
                    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(buf.thermal_cfg.unit, 0x8208, 28, 28, 0x1), ret);
                }



                reg_field_write(buf.thermal_cfg.unit, MANGO_TM0_CTRL4r,
                        MANGO_REG_RSTBf, &tmp);

                reg_field_write(buf.thermal_cfg.unit, MANGO_TM1_CTRL4r,
                        MANGO_REG_RSTBf, &tmp);
            }

            osal_time_mdelay(100);  /* wait for 100mS */

            if(0 == buf.thermal_cfg.meterId)
            {
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM0_CTRL0r,
                    MANGO_REG_Af, &buf.thermal_cfg.regA);
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM0_CTRL1r,
                    MANGO_REG_Bf, &buf.thermal_cfg.regB);
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM0_CTRL1r,
                    MANGO_REG_ADC_OSR_SELf, &buf.thermal_cfg.osrSel);
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM0_CTRL2r,
                    MANGO_REG_DIGT_ORDER_SELf, &buf.thermal_cfg.digtOdrSel);
                ret = reg_field_read(buf.thermal_cfg.unit, MANGO_TM0_RESULT0r,
                    MANGO_CT_OUTf, &buf.thermal_cfg.ct_out);
            }
            else
            {
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM1_CTRL0r,
                    MANGO_REG_Af, &buf.thermal_cfg.regA);
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM1_CTRL1r,
                    MANGO_REG_Bf, &buf.thermal_cfg.regB);
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM1_CTRL1r,
                    MANGO_REG_ADC_OSR_SELf, &buf.thermal_cfg.osrSel);
                reg_field_read(buf.thermal_cfg.unit, MANGO_TM1_CTRL2r,
                    MANGO_REG_DIGT_ORDER_SELf, &buf.thermal_cfg.digtOdrSel);
                ret = reg_field_read(buf.thermal_cfg.unit, MANGO_TM1_RESULT0r,
                    MANGO_CT_OUTf, &buf.thermal_cfg.ct_out);
            }
            copy_to_user(user, &buf.thermal_cfg, sizeof(rtdrv_ext_thermalCfg_t));
            break;
        default:
            break;
    }

    copy_to_user(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    return 0;
}

struct nf_sockopt_ops rtdrv_ext_sockopts = {
    .list = { NULL, NULL },
    .pf   = PF_INET,
    .set_optmin = RTDRV_EXT_BASE_CTL,
    .set_optmax = RTDRV_EXT_SET_MAX+1,
    .set = do_rtdrv_ext_set_ctl,
#ifdef CONFIG_COMPAT
    .compat_set = NULL,
#endif
    .get_optmin = RTDRV_EXT_BASE_CTL,
    .get_optmax = RTDRV_EXT_GET_MAX+1,
    .get = do_rtdrv_ext_get_ctl,
#ifdef CONFIG_COMPAT
    .compat_get = NULL,
#endif
    //.owner = NULL,
};

