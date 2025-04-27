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
#include <linux/version.h>

#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/mem.h>
#include <osal/print.h>
#include <hal/mac/mem.h>
#include <ioal/mem32.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/watchdog/watchdog.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_8390.h>
#include <osal/memory.h>
#if (defined(CONFIG_SDK_DRIVER_TEST) || defined(CONFIG_SDK_DRIVER_TEST_MODULE))
#include <sdk/sdk_test.h>
#endif
#include <dal/cypress/dal_cypress_l2.h>
#include <hal/mac/reg.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#ifdef CONFIG_SDK_MODEL_MODE
#include <model_comm.h>
#include <tc.h>
#include <virtualmac/vmac_target.h>
#include <osal/time.h>
#endif

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
    int32   ret = RT_ERR_FAILED, i = 0, j, payload_offset = 0;
#ifdef CONFIG_SDK_RTL8390
    rtdrv_ext_union_t   buf;
    spg_port_entry_t    spg;
    uint32  field, value;
    uint32  *ptr;
    sched_entry_t schedEntry;
    rtk_port_linkStatus_t   link_status = PORT_LINKDOWN;
    rtk_port_t basePortId;

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
     /** PORT **/
        case RTDRV_EXT_PORT_MAC_STATE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));

            if(buf.port_cfg.state == 0x1)
            {
                field = 1;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_TX_ENf, &field);
                field = 0;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_RX_ENf, &field);
            }
            else if(buf.port_cfg.state == 0x2)
            {
                field = 0;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_TX_ENf, &field);
                field = 1;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_RX_ENf, &field);
            }
            else if(buf.port_cfg.state == 0x3)
            {
                field = 1;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_TX_ENf, &field);
                field = 1;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_RX_ENf, &field);
            }
            else
            {
                field = 0;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_TX_ENf, &field);
                field = 0;
                ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_RX_ENf, &field);
            }
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_field_write(buf.port_cfg.unit, CYPRESS_SC_P_CTRLr, CYPRESS_DRAIN_OUT_THRf, &buf.port_cfg.full_th);
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_HALF_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_field_write(buf.port_cfg.unit, CYPRESS_SC_P_CTRLr, CYPRESS_DRAIN_OUT_THR_Hf, &buf.port_cfg.half_th);
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_SC_P_ENr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMTf, &buf.port_cfg.full_sec);
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_SUSTAIN_TIMER_HALF_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_SC_P_ENr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMT_Hf, &buf.port_cfg.half_sec);
            break;
        case RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_write(buf.port_cfg.unit, CYPRESS_SMI_PORT_POLLING_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_SMI_POLLING_PMSKf, &buf.port_cfg.state);
            break;
    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
        case RTDRV_EXT_MIB_RST_VAL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_mibCfg_t));
            ret = reg_field_write(buf.mib_cfg.unit, CYPRESS_STAT_RSTr, CYPRESS_RST_MIB_VALf, &buf.mib_cfg.rst_val);
            break;

    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
        case RTDRV_EXT_QOS_PORT_QUEUE_NUM_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_qosCfg_t));
            ret = reg_array_field_write(buf.qos_cfg.unit, CYPRESS_QM_PORT_QNUMr, buf.qos_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_QNUMf, &buf.qos_cfg.qnum);
            break;

        case RTDRV_EXT_QOS_PORT_RATE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_qosCfg_t));
            osal_memset(&schedEntry, 0, sizeof(sched_entry_t));
            /* program value to CHIP */
            ret = table_read(buf.qos_cfg.unit, CYPRESS_SCHEDt, buf.qos_cfg.port, (uint32 *) &schedEntry);
            ret = table_field_set(buf.qos_cfg.unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf, &buf.qos_cfg.data, (uint32 *) &schedEntry);
            if ((ret = table_write(buf.qos_cfg.unit, CYPRESS_SCHEDt, buf.qos_cfg.port, (uint32 *) &schedEntry)) != RT_ERR_OK)
                break;
            break;

    /** TRUNK **/
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
    /** SVLAN **/
    /** SWITCH **/
        case RTDRV_EXT_SWITCH_48PASS1_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_HALF_48PASS1_ENf, &buf.switch_cfg.half_48pass1);
            break;
        case RTDRV_EXT_SWITCH_LIMITPAUSE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_LIMIT_PAUSE_ENf, &buf.switch_cfg.limit_pause);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_1G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_1G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_10G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_10G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            break;
        case RTDRV_EXT_SWITCH_IPGMINLEN_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_IPG_MIN_RX_SELf, &buf.switch_cfg.data);
            break;
        case RTDRV_EXT_SWITCH_BKPRES_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_write(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_BKPRES_MTHD_SELf, &buf.switch_cfg.bkpres);
            break;
        case RTDRV_EXT_SWITCH_BYPASSTXCRC_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_BYP_TX_CRCf, &buf.switch_cfg.bypass_tx_crc);
            break;
        case RTDRV_EXT_SWITCH_PASSALLMODE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_PASS_ALL_MODE_ENf, &buf.switch_cfg.pass_all_mode);
            break;
        case RTDRV_EXT_SWITCH_RXCHECKCRC_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_RX_CHK_CRC_ENf, &buf.switch_cfg.rx_check_crc);
            break;
        case RTDRV_EXT_SWITCH_PADDINGUNDSIZE_SET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_write(buf.switch_cfg.unit, CYPRESS_MAC_PADDING_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_PADDING_UND_SIZE_ENf, &buf.switch_cfg.enable);
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

    /** NIC **/
    /** MPLS **/
    /** EEE **/
        case RTDRV_EXT_EEE_TX_WAKE_MODE_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEE_TX_CTRLr,
                          CYPRESS_TX_WAKE_SELf,
                          &buf.eee_cfg.wakeMode);
            break;
        case RTDRV_EXT_EEE_LINK_UP_DELAY_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_write(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_LINK_UP_DELAYf, &buf.eee_cfg.linkUpDelay);
            break;
        case RTDRV_EXT_EEE_MULTI_WAKE_STATE_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_write(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_MULTIWAKE_ENf, &buf.eee_cfg.enable);
            break;
        case RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_write(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_MULTIWAKE_INTLVf, &buf.eee_cfg.interval);
            break;
        case RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_write(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_MULTIWAKE_PORTSf, &buf.eee_cfg.portNum);
            break;
        case RTDRV_EXT_EEEP_TX_SLEEP_RATE_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            {
                uint32 rateTimer, rateThr;
                rateTimer = 192;
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_100M_CTRLr,
                          CYPRESS_TX_RATE_TIMER_100Mf,
                              &rateTimer);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_500M_CTRLr,
                          CYPRESS_TX_RATE_TIMER_500Mf,
                              &rateTimer);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_GIGA_CTRLr,
                          CYPRESS_TX_RATE_TIMER_GIGAf,
                          &rateTimer);
                rateThr = buf.eee_cfg.rate * rateTimer / 16;
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_100M_CTRLr,
                          CYPRESS_TX_RATE_THR_100Mf,
                          &rateThr);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_500M_CTRLr,
                          CYPRESS_TX_RATE_THR_500Mf,
                          &rateThr);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_GIGA_CTRLr,
                          CYPRESS_TX_RATE_THR_GIGAf,
                          &rateThr);

            }

            break;
        case RTDRV_EXT_EEEP_RX_SLEEP_RATE_SET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            {
                uint32 rateTimer, rateThr;
                rateTimer = 192;
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_100M_CTRLr,
                          CYPRESS_RX_RATE_TIMER_100Mf,
                          &rateTimer);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_500M_CTRLr,
                          CYPRESS_RX_RATE_TIMER_500Mf,
                          &rateTimer);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_GIGA_CTRLr,
                          CYPRESS_RX_RATE_TIMER_GIGAf,
                          &rateTimer);
                rateThr = buf.eee_cfg.rate * rateTimer / 16;
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_100M_CTRLr,
                          CYPRESS_RX_RATE_THR_100Mf,
                          &rateThr);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_500M_CTRLr,
                          CYPRESS_RX_RATE_THR_500Mf,
                          &rateThr);
                ret = reg_field_write(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_GIGA_CTRLr,
                          CYPRESS_RX_RATE_THR_GIGAf,
                          &rateThr);
            }
            break;

    /** IOL **/
        case RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IOL_MAX_RETRY_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_ERROR_LENGTH_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IOL_LEN_ERR_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_INVALID_PAUSE_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IOL_PAUSE_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_LATE_COLLISION_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_LATE_COLI_DROP_ENf, &buf.iol_cfg.action);
            break;
        case RTDRV_EXT_IOL_MAX_LENGTH_SET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_write(buf.iol_cfg.unit, CYPRESS_MAC_MAX_LEN_CTRLr,
                    CYPRESS_MAX_LEN_TAG_INCf, &buf.iol_cfg.enable);
            break;
    /** MODEL TEST **/
#ifdef CONFIG_SDK_MODEL_MODE
        case RTDRV_EXT_MODEL_TEST_SET:
            copy_from_user(&buf.model_cfg, user, sizeof(rtdrv_ext_modelCfg_t));
            vmac_setCaredICType(buf.model_cfg.caredType);
            ret = tc_exec(buf.model_cfg.startID, buf.model_cfg.endID);
            vmac_setCaredICType(CARE_TYPE_BOTH);
            break;
#endif
        /*** packet generation ***/
        case RTDRV_EXT_PKTGEN_TX_CMD_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            reg_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_GRP_TX_CMDf, &value);
            if (RTDRV_EXT_SPG_TXCMD_START == value && RTDRV_EXT_SPG_TXCMD_START == buf.pktgen_cfg.status)
            {
                /* Only finished, can we go on */
                HWP_PORT_TRAVS_EXCEPT_CPU(buf.pktgen_cfg.unit, i)
                {
                    rtk_port_link_get(buf.pktgen_cfg.unit, i, &link_status);
                    if (PORT_LINKDOWN == link_status)
                        continue;
                    ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_STSr,
                    i, REG_ARRAY_INDEX_NONE, CYPRESS_TX_DONE_PORTf,
                        &value);
                    if (value == 0)
                    {
                        ret = RT_ERR_FAILED;
                        goto FAIL_EXIT;
                    }
                }

                value = RTDRV_EXT_SPG_TXCMD_NOP;
                reg_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_GRP_TX_CMDf, &value);
            }
            ret = reg_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_GRP_TX_CMDf, &buf.pktgen_cfg.status);
            reg_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_GRP_TX_CMDf, &buf.pktgen_cfg.status);
            break;
        case RTDRV_EXT_PKTGEN_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_SPG_MODEf, &buf.pktgen_cfg.enable);
            break;
        case RTDRV_EXT_PKTGEN_PORT_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_TX_GRP_CTRLr,
                    buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_GRP_TX_PORTf, &buf.pktgen_cfg.enable);
            break;
        case RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_FRAG_PKT_TRAPf, &buf.pktgen_cfg.action);
            break;
        case RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_OVER_SIZE_TRAPf, &buf.pktgen_cfg.action);
            break;
        case RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_UNDER_SIZE_TRAPf, &buf.pktgen_cfg.action);
            break;
        case RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_BAD_CRC_TRAPf, &buf.pktgen_cfg.action);
            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    CYPRESS_SPG_PORT_PKT_CNTtf, &buf.pktgen_cfg.status,
                    (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_BAD_CRC_EN_0tf;
            else
                field = CYPRESS_SPG_PORT_BAD_CRC_EN_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status,
                    (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_LEN_TYPE_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_LEN_TYPE_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_RNDM_EN_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_RNDM_EN_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_RNDM_OFFSET_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_RNDM_OFFSET_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_SA_INC_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_SA_INC_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_DA_INC_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_DA_INC_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_PKT_CNT_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_PKT_CNT_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            /* packet length range start */
            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_START_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_START_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.pktlen_start, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            /* packet length range end */
            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_END_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_END_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.pktlen_end, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_SA_REPEAT_CNT_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_SA_REPEAT_CNT_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_DA_REPEAT_CNT_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_DA_REPEAT_CNT_1tf;

            ret = table_field_set(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            if (ret != RT_ERR_OK)
                break;

            if ((ret = table_write(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_SA_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&pDiagExtPacketHdr[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx][6], buf.pktgen_cfg.sa.octet, ETHER_ADDR_LEN);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_DA_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&pDiagExtPacketHdr[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx][0], buf.pktgen_cfg.da.octet, ETHER_ADDR_LEN);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_ETHTYPE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&pDiagExtPacketHdr[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx][12], &buf.pktgen_cfg.etherType, 2);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            osal_memcpy(&oTag[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx], &buf.pktgen_cfg.vlanHdr, 4);
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_OTAG_STATE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            include_vlan_header[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx] = buf.pktgen_cfg.status;
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_TYPE_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            spg_payload_type[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx] = buf.pktgen_cfg.patternType;
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_FIELD_PAYLOAD_PATTERN_SET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            spg_payload_pattern[buf.pktgen_cfg.port][buf.pktgen_cfg.stream_idx] = buf.pktgen_cfg.pattern;
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_TX:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            /* Tx two stream */
            for (i = 0; i < 2; i++)
            {
                if (RT_ERR_OK != drv_nic_pkt_alloc(buf.pktgen_cfg.unit, 1518, 0, &pDiagExtPacket))
                {
                    ret = RT_ERR_FAILED;   /* Failed */
                    goto FAIL_EXIT;
                }

                osal_memset(pDiagExtPacket->data, 0, pDiagExtPacket->length);

                if (include_vlan_header[buf.pktgen_cfg.port][i])
                {
                    osal_memcpy(pDiagExtPacket->data, &pDiagExtPacketHdr[buf.pktgen_cfg.port][i][0], 12);
                    osal_memcpy(pDiagExtPacket->data + 12, &oTag[buf.pktgen_cfg.port][i], 4);
                    osal_memcpy(pDiagExtPacket->data + 16, &pDiagExtPacketHdr[buf.pktgen_cfg.port][i][12], EXT_PACKET_HDR_LEN - 12);
                    payload_offset = 16;
                }
                else
                {
                    osal_memcpy(pDiagExtPacket->data, &pDiagExtPacketHdr[buf.pktgen_cfg.port][i][0], EXT_PACKET_HDR_LEN);
                    payload_offset = 14;
                }


                /* Set payload content */
                if (RTDRV_EXT_SPG_PAYLOAD_INCR == spg_payload_type[buf.pktgen_cfg.port][i])
                {
                    for (j = 0; j < pDiagExtPacket->length - payload_offset; j++)
                        pDiagExtPacket->data[payload_offset + j] = j;
                }
                else if (RTDRV_EXT_SPG_PAYLOAD_FIX == spg_payload_type[buf.pktgen_cfg.port][i])
                {
                    ptr = (uint32*)&pDiagExtPacket->data[payload_offset];
                    *ptr = spg_payload_pattern[buf.pktgen_cfg.port][i];
                }
                else if (RTDRV_EXT_SPG_PAYLOAD_REPEAT == spg_payload_type[buf.pktgen_cfg.port][i])
                {
                    ptr = (uint32*)&pDiagExtPacket->data[payload_offset];
                    for (j = 0; j < (pDiagExtPacket->length - payload_offset) / 4; j++)
                    {
                        *ptr = spg_payload_pattern[buf.pktgen_cfg.port][i];
                        ptr++;
                    }
                }

                pDiagExtPacket->as_txtag = TRUE;
                pDiagExtPacket->tx_tag.fwd_type         = NIC_FWD_TYPE_LOGICAL;
                pDiagExtPacket->tx_tag.dst_port_mask	= ((uint64)1 << buf.pktgen_cfg.port) & 0xffffffff;
                pDiagExtPacket->tx_tag.dst_port_mask_1  = ((uint64)1 << buf.pktgen_cfg.port) >> 32;
                drv_nic_pkt_tx(buf.nicSend_cfg.unit, pDiagExtPacket, NULL, NULL);
            }
            ret = RT_ERR_OK;

            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_DYING_GASP:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));

            if (RT_ERR_OK != drv_nic_pkt_alloc(buf.pktgen_cfg.unit, 1518, 0, &pDiagExtPacket))
            {
                ret = RT_ERR_FAILED;   /* Failed */
                goto FAIL_EXIT;
            }

            osal_memset(pDiagExtPacket->data, 0, pDiagExtPacket->length);

            /* Only tx stream 0 as dying gasp */
            osal_memcpy(pDiagExtPacket->data, &pDiagExtPacketHdr[buf.pktgen_cfg.port][0][0], EXT_PACKET_HDR_LEN);

            /* Set OAM header */
            pDiagExtPacket->data[12] = 0x88;
            pDiagExtPacket->data[13] = 0x09;
            pDiagExtPacket->data[14] = 0x03;
            pDiagExtPacket->data[15] = 0x00;
            pDiagExtPacket->data[16] = 0x52;
            pDiagExtPacket->data[17] = 0x00;

            /* Set payload content */
            payload_offset = 18;
            if (RTDRV_EXT_SPG_PAYLOAD_INCR == spg_payload_type[buf.pktgen_cfg.port][0])
            {
                for (j = 0; j < pDiagExtPacket->length - payload_offset; j++)
                    pDiagExtPacket->data[payload_offset + j] = j;
            }
            else if (RTDRV_EXT_SPG_PAYLOAD_FIX == spg_payload_type[buf.pktgen_cfg.port][0])
            {
                ptr = (uint32*)&pDiagExtPacket->data[payload_offset];
                *ptr = spg_payload_pattern[buf.pktgen_cfg.port][0];
            }
            else if (RTDRV_EXT_SPG_PAYLOAD_REPEAT == spg_payload_type[buf.pktgen_cfg.port][0])
            {
                ptr = (uint32*)&pDiagExtPacket->data[payload_offset];
                for (j = 0; j < (pDiagExtPacket->length - payload_offset) / 4; j++)
                {
                    *ptr = spg_payload_pattern[buf.pktgen_cfg.port][0];
                    ptr++;
                }
            }

            pDiagExtPacket->length = buf.pktgen_cfg.len;
            pDiagExtPacket->as_txtag = TRUE;
            pDiagExtPacket->tx_tag.dg_pkt = TRUE;
            pDiagExtPacket->tx_tag.fwd_type         = NIC_FWD_TYPE_LOGICAL;
            pDiagExtPacket->tx_tag.dst_port_mask	= ((uint64)1 << buf.pktgen_cfg.port) & 0xffffffff;
            pDiagExtPacket->tx_tag.dst_port_mask_1  = ((uint64)1 << buf.pktgen_cfg.port) >> 32;
            drv_nic_pkt_tx(buf.nicSend_cfg.unit, pDiagExtPacket, NULL, NULL);
            ret = RT_ERR_OK;
            break;

        case RTDRV_EXT_L2_AGING_UNIT_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_write(buf.l2_cfg.unit, CYPRESS_L2_CTRL_1r,
                    CYPRESS_AGE_UNITf, &buf.l2_cfg.aging_time);
            break;

        case RTDRV_EXT_L2_TBL_CLEAR:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            field = 1;
            ret = reg_field_write(buf.l2_cfg.unit, CYPRESS_RST_GLB_CTRLr,
                    CYPRESS_TBL_L2_RSTf, &field);
            rtk_l2_init(0);
            break;

        case RTDRV_EXT_L2_CMA_ENABLE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_write(buf.l2_cfg.unit, CYPRESS_L2_CTRL_0r, CYPRESS_LUTCAM_ENf, &buf.l2_cfg.enable);
            rtk_l2_init(0);
            break;

        case RTDRV_EXT_ACL_METER_COUNTER_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_ext_aclCfg_t));
            ret = reg_field_write(buf.acl_cfg.unit, CYPRESS_METER_CNTR_CTRLr,
                    CYPRESS_UNITf, &buf.acl_cfg.counterUnit);
            ret = reg_field_write(buf.acl_cfg.unit, CYPRESS_METER_CNTR_CTRLr,
                    CYPRESS_CNT_METER_IDXf, &buf.acl_cfg.index);
            ret = reg_field_write(buf.acl_cfg.unit, CYPRESS_METER_CNTR_CTRLr,
                    CYPRESS_CNTR_CLRf, &buf.acl_cfg.clear);
            break;
        case RTDRV_EXT_ACL_METER_COUNTER_RESET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_ext_aclCfg_t));
            ret = reg_field_write(buf.acl_cfg.unit, CYPRESS_METER_CNTR_CTRLr,
                    CYPRESS_CNTR_CLRf, &buf.acl_cfg.clear);
            break;
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
                ret = RT_ERR_FAILED;   /* Failed */
                goto FAIL_EXIT;
            }

            tx_buff = osal_alloc(1600);
            if (tx_buff == NULL)
            {
                osal_free(pPacket);
                ret = RT_ERR_FAILED;   /* Failed */
                goto FAIL_EXIT;
            }

            {
                if(buf.nicSend_cfg.isCpuTag == TRUE)
                    pPacket->as_txtag = 1;
                else
                    pPacket->as_txtag = 0;


                if(buf.nicSend_cfg.isTrunkHash == TRUE)
                    pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_LOGICAL;
                else
                    pPacket->tx_tag.fwd_type = NIC_FWD_TYPE_PHYISCAL;

                pPacket->tx_tag.dst_port_mask       = buf.nicSend_cfg.txPortmask.bits[0];
                pPacket->tx_tag.dst_port_mask_1     = buf.nicSend_cfg.txPortmask.bits[1];

                pPacket->tx_tag.as_priority         = 1;
                pPacket->tx_tag.priority            = 7;
                pPacket->tx_tag.l2_learning         = 0;
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
                ret = RT_ERR_FAILED;   /* Failed */
                goto FAIL_EXIT;
            }
          }

            break;
        case RTDRV_EXT_TIME_PORT_TPID_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpIgrTpid_set(buf.time_cfg.unit, basePortId, buf.time_cfg.vlanType, buf.time_cfg.tpid_idx, buf.time_cfg.tpid);
            break;
        case RTDRV_EXT_TIME_PORT_MAC_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpSwitchMacAddr_set(buf.time_cfg.unit, basePortId, &buf.time_cfg.mac);
            ret = phy_ptpSwitchMacRange_set(buf.time_cfg.unit, basePortId, buf.time_cfg.range);
            break;

        case RTDRV_EXT_TIME_PORT_REF_TIME_FREQ_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpRefTimeFreq_set(buf.time_cfg.unit, basePortId, buf.time_cfg.freq);

            break;

        case RTDRV_EXT_TIME_PORT_INTERRUPT_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpInterruptEnable_set(buf.time_cfg.unit, basePortId, buf.time_cfg.interruptEbl);

            break;

        default:
            break;
    }
#endif /*CONFIG_SDK_RTL8390*/

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
    void            *user = (char *)user_in + sizeof(rtdrv_msgHdr_t);
    int32   ret = RT_ERR_FAILED;
#ifdef CONFIG_SDK_RTL8390
    rtdrv_ext_union_t   buf;
    spg_port_entry_t    spg;
    uint32  field;
    sched_entry_t schedEntry;
    rtk_port_t basePortId;

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
    /** PORT **/
        case RTDRV_EXT_PORT_MAC_STATE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            buf.port_cfg.state = 0;
            ret = reg_array_field_read(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_TX_ENf, &field);
            buf.port_cfg.state |= field;
            ret = reg_array_field_read(buf.port_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_RX_ENf, &field);
            buf.port_cfg.state |= field << 1;
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_DRAIN_OUT_THRESH_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_field_read(buf.port_cfg.unit, CYPRESS_SC_P_CTRLr, CYPRESS_DRAIN_OUT_THRf, &buf.port_cfg.full_th);
            ret = reg_field_read(buf.port_cfg.unit, CYPRESS_SC_P_CTRLr, CYPRESS_DRAIN_OUT_THR_Hf, &buf.port_cfg.half_th);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;

        case RTDRV_EXT_PORT_SPECIAL_CONGEST_PORT_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_read(buf.port_cfg.unit, CYPRESS_SC_P_ENr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMTf, &buf.port_cfg.full_sec);
            ret = reg_array_field_read(buf.port_cfg.unit, CYPRESS_SC_P_ENr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_CNGST_SUST_TMR_LMT_Hf, &buf.port_cfg.half_sec);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;
        case RTDRV_EXT_PORT_MACPOLLINGPHYSTATE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_ext_portCfg_t));
            ret = reg_array_field_read(buf.port_cfg.unit, CYPRESS_SMI_PORT_POLLING_CTRLr, buf.port_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_SMI_POLLING_PMSKf, &buf.port_cfg.state);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_ext_portCfg_t));
            break;
    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
        case RTDRV_EXT_MIB_RST_VAL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_mibCfg_t));
            ret = reg_field_read(buf.mib_cfg.unit, CYPRESS_STAT_RSTr, CYPRESS_RST_MIB_VALf, &buf.mib_cfg.rst_val);
            copy_to_user(user, &buf.mib_cfg, sizeof(rtdrv_ext_mibCfg_t));

            break;

    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
        case RTDRV_EXT_QOS_PORT_QUEUE_NUM_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_qosCfg_t));
            ret = reg_array_field_read(buf.qos_cfg.unit, CYPRESS_QM_PORT_QNUMr, buf.qos_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_QNUMf, &buf.qos_cfg.qnum);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_ext_qosCfg_t));
            break;

        case RTDRV_EXT_QOS_PORT_RATE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_qosCfg_t));
            osal_memset(&schedEntry, 0, sizeof(sched_entry_t));
            /* program value to CHIP */
            ret = table_read(buf.qos_cfg.unit, CYPRESS_SCHEDt, buf.qos_cfg.port, (uint32 *) &schedEntry);
            ret = table_field_get(buf.qos_cfg.unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf, &buf.qos_cfg.data, (uint32 *) &schedEntry);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_ext_qosCfg_t));
            break;

    /** TRUNK **/
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
    /** SVLAN **/
    /** SWITCH **/
        case RTDRV_EXT_SWITCH_48PASS1_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_HALF_48PASS1_ENf, &buf.switch_cfg.half_48pass1);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_LIMITPAUSE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_LIMIT_PAUSE_ENf, &buf.switch_cfg.limit_pause);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_INFO_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_ext_switchCfg_t));

            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MODEL_NAME_INFOr,
                    CYPRESS_MODEL_CHAR_1STf, &buf.switch_cfg.model_char_1st);
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MODEL_NAME_INFOr,
                    CYPRESS_MODEL_CHAR_2NDf, &buf.switch_cfg.model_char_2nd);
#if 0
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MODEL_NAME_INFOr,
                    CYPRESS_MODEL_CHAR_3RDf, &buf.switch_cfg.model_char_3rd);
#endif
            ret = ioal_mem32_read(buf.switch_cfg.unit, 0x0804, &buf.switch_cfg.data);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_1G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_SEL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_1G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_10G_COMPS_ENf, &buf.switch_cfg.ipg_cmpstn);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGCOMSTN_10G_SEL_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IPG_10G_SELf, &buf.switch_cfg.ipg_cmpstn_sel);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_IPGMINLEN_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_IPG_MIN_RX_SELf, &buf.switch_cfg.data);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_BKPRES_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_field_read(buf.switch_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_BKPRES_MTHD_SELf, &buf.switch_cfg.bkpres);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_BYPASSTXCRC_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_BYP_TX_CRCf, &buf.switch_cfg.bypass_tx_crc);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_PASSALLMODE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_PASS_ALL_MODE_ENf, &buf.switch_cfg.pass_all_mode);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_RXCHECKCRC_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, CYPRESS_MAC_PORT_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_RX_CHK_CRC_ENf, &buf.switch_cfg.rx_check_crc);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;
        case RTDRV_EXT_SWITCH_PADDINGUNDSIZE_GET:
            copy_from_user(&buf.mib_cfg, user, sizeof(rtdrv_ext_switchCfg_t));
            ret = reg_array_field_read(buf.switch_cfg.unit, CYPRESS_MAC_PADDING_CTRLr, buf.switch_cfg.port,
                    REG_ARRAY_INDEX_NONE, CYPRESS_PADDING_UND_SIZE_ENf, &buf.switch_cfg.enable);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_ext_switchCfg_t));
            break;

    /** NIC **/
    /** MPLS **/
    /** EEE **/
        case RTDRV_EXT_EEE_TX_WAKE_MODE_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEE_TX_CTRLr,
                          CYPRESS_TX_WAKE_SELf,
                          &buf.eee_cfg.wakeMode);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEE_PORT_STATUS_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_array_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEE_EEEP_PORT_TX_STSr,
                          buf.eee_cfg.port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_EEE_EEEP_TX_STSf,
                          &buf.eee_cfg.txState);
            ret = reg_array_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEE_EEEP_PORT_RX_STSr,
                          buf.eee_cfg.port,
                          REG_ARRAY_INDEX_NONE,
                          CYPRESS_EEE_EEEP_RX_STSf,
                          &buf.eee_cfg.rxState);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEE_LINK_UP_DELAY_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_read(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_LINK_UP_DELAYf, &buf.eee_cfg.linkUpDelay);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEE_MULTI_WAKE_STATE_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_read(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_MULTIWAKE_ENf, &buf.eee_cfg.enable);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEE_MULTI_WAKE_INTERVAL_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_read(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_MULTIWAKE_INTLVf, &buf.eee_cfg.interval);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEE_MULTI_WAKE_PORT_NUM_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            ret = reg_field_read(buf.eee_cfg.unit, CYPRESS_EEE_TX_CTRLr,
                    CYPRESS_MULTIWAKE_PORTSf, &buf.eee_cfg.portNum);
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEEP_TX_SLEEP_RATE_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            {
                uint32 rateTimer, rateThr;
                ret = reg_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_100M_CTRLr,
                          CYPRESS_TX_RATE_TIMER_100Mf,
                          &rateTimer);
                ret = reg_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEEP_TX_100M_CTRLr,
                          CYPRESS_TX_RATE_THR_100Mf,
                          &rateThr);
                if(0 == rateTimer)
                    buf.eee_cfg.rate = 0;
                else
                    buf.eee_cfg.rate = rateThr * 16 / rateTimer;
            }
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
        case RTDRV_EXT_EEEP_RX_SLEEP_RATE_GET:
            copy_from_user(&buf.eee_cfg, user, sizeof(rtdrv_ext_eeeCfg_t));
            {
                uint32 rateTimer, rateThr;
                ret = reg_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_100M_CTRLr,
                          CYPRESS_RX_RATE_TIMER_100Mf,
                          &rateTimer);
                ret = reg_field_read(buf.eee_cfg.unit,
                          CYPRESS_EEEP_RX_RATE_100M_CTRLr,
                          CYPRESS_RX_RATE_THR_100Mf,
                          &rateThr);
                if(0 == rateTimer)
                    buf.eee_cfg.rate = 0;
                else
                    buf.eee_cfg.rate = rateThr * 16 / rateTimer;
            }
            copy_to_user(user, &buf.eee_cfg, sizeof(rtdrv_ext_eeeCfg_t));
            break;
    /** IOL **/
        case RTDRV_EXT_IOL_COLLISION_MAX_ATTEMPT_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IOL_MAX_RETRY_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_ERROR_LENGTH_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IOL_LEN_ERR_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_INVALID_PAUSE_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_IOL_PAUSE_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_LATE_COLLISION_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, CYPRESS_MAC_GLB_CTRLr,
                    CYPRESS_LATE_COLI_DROP_ENf, &buf.iol_cfg.action);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        case RTDRV_EXT_IOL_MAX_LENGTH_GET:
            copy_from_user(&buf.iol_cfg, user, sizeof(rtdrv_ext_iolCfg_t));
            ret = reg_field_read(buf.iol_cfg.unit, CYPRESS_MAC_MAX_LEN_CTRLr,
                    CYPRESS_MAX_LEN_TAG_INCf, &buf.iol_cfg.enable);
            copy_to_user(user, &buf.iol_cfg, sizeof(rtdrv_ext_iolCfg_t));
            break;
        /*** packet generation ***/
        case RTDRV_EXT_PKTGEN_TX_CMD_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_GRP_TX_CMDf, &buf.pktgen_cfg.status);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_GLB_CTRLr,
                    CYPRESS_SPG_MODEf, &buf.pktgen_cfg.enable);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_TX_GRP_CTRLr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_GRP_TX_PORTf,
                    &buf.pktgen_cfg.enable);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_DONE_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_STSr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_TX_DONE_PORTf,
                    &buf.pktgen_cfg.enable);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_FRAG_PKT_ACTION_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_FRAG_PKT_TRAPf,
                    &buf.pktgen_cfg.action);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_OVERSIZE_PKT_ACTION_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_OVER_SIZE_TRAPf,
                    &buf.pktgen_cfg.action);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_UNDERSIZE_PKT_ACTION_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_UNDER_SIZE_TRAPf,
                    &buf.pktgen_cfg.action);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_BADCRC_PKT_ACTION_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            ret = reg_array_field_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORT_PKT_TRAPr,
                buf.pktgen_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_BAD_CRC_TRAPf,
                    &buf.pktgen_cfg.action);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_TX_PKT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;
            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    CYPRESS_SPG_PORT_PKT_CNTtf, &buf.pktgen_cfg.status,
                    (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_BADCRC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_BAD_CRC_EN_0tf;
            else
                field = CYPRESS_SPG_PORT_BAD_CRC_EN_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status,
                    (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_LENTYPE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_LEN_TYPE_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_LEN_TYPE_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_CONTENT_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_RNDM_EN_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_RNDM_EN_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_RANDOM_OFFSET_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_RNDM_OFFSET_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_RNDM_OFFSET_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_INC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_SA_INC_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_SA_INC_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_INC_STATE_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_DA_INC_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_DA_INC_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_TX_PKT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_PKT_CNT_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_PKT_CNT_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_PKT_LEN_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            /* packet length range start */
            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_START_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_START_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.pktlen_start, (uint32 *) &spg);

            /* packet length range end */
            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_END_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_LEN_RNG_END_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.pktlen_end, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_SA_REPEAT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_SA_REPEAT_CNT_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_SA_REPEAT_CNT_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_PKTGEN_PORT_STREAM_DA_REPEAT_CNT_GET:
            copy_from_user(&buf.pktgen_cfg, user, sizeof(rtdrv_ext_pktGenCfg_t));
            if ((ret = table_read(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    buf.pktgen_cfg.port, (uint32 *) &spg)) != RT_ERR_OK)
                break;

            if (0 == buf.pktgen_cfg.stream_idx)
                field = CYPRESS_SPG_PORT_STREAM_DA_REPEAT_CNT_0tf;
            else
                field = CYPRESS_SPG_PORT_STREAM_DA_REPEAT_CNT_1tf;

            ret = table_field_get(buf.pktgen_cfg.unit, CYPRESS_SPG_PORTt,
                    field, &buf.pktgen_cfg.status, (uint32 *) &spg);
            copy_to_user(user, &buf.pktgen_cfg, sizeof(rtdrv_ext_pktGenCfg_t));
            break;
        case RTDRV_EXT_L2_AGING_UNIT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));

            ret = reg_field_read(buf.l2_cfg.unit, CYPRESS_L2_CTRL_1r, CYPRESS_AGE_UNITf,
                    &buf.l2_cfg.aging_time);

            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_ext_l2Cfg_t));
            break;
        case RTDRV_EXT_L2_UC_SIZE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));

            ret = table_size_get(buf.l2_cfg.unit, CYPRESS_L2_UCt, &buf.l2_cfg.data);

            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_ext_l2Cfg_t));
            break;
        case RTDRV_EXT_L2_CMA_ENABLE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_ext_l2Cfg_t));
            ret = reg_field_read(buf.l2_cfg.unit, CYPRESS_L2_CTRL_0r, CYPRESS_LUTCAM_ENf, &buf.l2_cfg.enable);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_ext_l2Cfg_t));
            break;
        case RTDRV_EXT_ACL_METER_COUNTER_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_ext_aclCfg_t));

            ret = reg_field_read(buf.acl_cfg.unit, CYPRESS_METER_CNTR_CTRLr,
                    CYPRESS_UNITf, &buf.acl_cfg.counterUnit);
            ret = reg_field_read(buf.acl_cfg.unit, CYPRESS_METER_CNTR_CTRLr,
                    CYPRESS_CNT_METER_IDXf, &buf.acl_cfg.index);
            ret = reg_field_read(buf.acl_cfg.unit, CYPRESS_METER_GREEN_CNTR_STSr,
                    CYPRESS_GREEN_CNTRf, &buf.acl_cfg.greenCounter);
            ret = reg_field_read(buf.acl_cfg.unit, CYPRESS_METER_YELLOW_CNTR_STSr,
                    CYPRESS_YELLOW_CNTRf, &buf.acl_cfg.yellowCounter);
            ret = reg_field_read(buf.acl_cfg.unit, CYPRESS_METER_RED_CNTR_STSr,
                    CYPRESS_RED_CNTRf, &buf.acl_cfg.redCounter);
            ret = reg_field_read(buf.acl_cfg.unit, CYPRESS_METER_TOTAL_CNTR_STSr,
                    CYPRESS_TOTAL_CNTRf, &buf.acl_cfg.totalCounter);

            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_ext_aclCfg_t));
            break;
        case RTDRV_EXT_TIME_PORT_PTP_TIMESTAMP_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            if (0 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SYNC_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else if (1 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_DELAY_REQ_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else if (2 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_PDELAY_REQ_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else if (3 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_PDELAY_RESP_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_RX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else if (4 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SYNC_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else if (5 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_DELAY_REQ_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else if (6 == buf.time_cfg.type)
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_PDELAY_REQ_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }
            else
            {
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_PDELAY_RESP_SEQ_IDf,
                    &buf.time_cfg.sid);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_SECf,
                    &buf.time_cfg.sec);
                ret = reg_array_field_read(buf.time_cfg.unit, CYPRESS_PTP_PORT_TX_TIMEr,
                    buf.time_cfg.port, REG_ARRAY_INDEX_NONE, CYPRESS_NSECf,
                    &buf.time_cfg.nsec);
            }

            buf.time_cfg.nsec = (buf.time_cfg.nsec << 3);

            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_ext_timeCfg_t));
            break;

        case RTDRV_EXT_TIME_PORT_REF_TIME_GET:

            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpRefTime_get(buf.time_cfg.unit, basePortId, &buf.time_cfg.timeStamp);

            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_ext_timeCfg_t));
            break;

        case RTDRV_EXT_TIME_PORT_TPID_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpIgrTpid_get(buf.time_cfg.unit, basePortId, buf.time_cfg.vlanType, buf.time_cfg.tpid_idx, &buf.time_cfg.tpid);

            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_ext_timeCfg_t));
            break;
        case RTDRV_EXT_TIME_PORT_MAC_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpSwitchMacAddr_get(buf.time_cfg.unit, basePortId, &buf.time_cfg.mac);
            ret = phy_ptpSwitchMacRange_get(buf.time_cfg.unit, basePortId, &buf.time_cfg.range);

            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_ext_timeCfg_t));
            break;
        case RTDRV_EXT_TIME_PORT_REF_TIME_FREQ_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpRefTimeFreq_get(buf.time_cfg.unit, basePortId, &buf.time_cfg.freq);

            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_ext_timeCfg_t));
            break;
        case RTDRV_EXT_TIME_PORT_INTERRUPT_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_ext_timeCfg_t));

            basePortId = HWP_PHY_BASE_MACID(buf.time_cfg.unit, buf.time_cfg.port);

            ret = phy_ptpInterruptEnable_get(buf.time_cfg.unit, basePortId, &buf.time_cfg.interruptEbl);
            ret = phy_ptpInterruptStatus_get(buf.time_cfg.unit, buf.time_cfg.port, &buf.time_cfg.interruptTrigger, &buf.time_cfg.interruptSts);

            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_ext_timeCfg_t));
            break;
        default:
            break;
    }
#endif /*CONFIG_SDK_RTL8390*/

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
