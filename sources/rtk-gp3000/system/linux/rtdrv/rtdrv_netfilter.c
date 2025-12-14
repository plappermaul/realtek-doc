/*Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Module.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Module
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <common/rt_error.h>
#include <osal/print.h>
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <ioal/mac_debug.h>
#include <osal/memory.h>
#include <pkt_redirect.h>
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
int32 do_rtdrv_set_ctl(struct sock *sk, int cmd, void *user, unsigned int len);
int32 do_rtdrv_get_ctl(struct sock *sk, int cmd, void *user, int *len);

static struct nf_sockopt_ops rtdrv_sockopts = {
    .pf = PF_INET,
    .set_optmin = RTDRV_BASE_CTL,
    .set_optmax = RTDRV_SET_MAX+1,
    .set = do_rtdrv_set_ctl,
    .get_optmin = RTDRV_BASE_CTL,
    .get_optmax = RTDRV_GET_MAX+1,
    .get = do_rtdrv_get_ctl,
};

#if defined(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE) && defined(CONFIG_COMMON_RT_API)
static struct nf_sockopt_ops rtdrv_ext_sockopts = {
    .pf = PF_INET,
    .set_optmin = RTDRV_BASE_CTL+RTDRV_EXT_OFFSET,
    .set_optmax = RTDRV_BASE_CTL+RTDRV_EXTEND_OFFSET+1,
    .set = do_rtdrv_ext_set_ctl,
    .get_optmin = RTDRV_BASE_CTL+RTDRV_EXT_OFFSET,
    .get_optmax = RTDRV_BASE_CTL+RTDRV_EXTEND_OFFSET+1,
    .get = do_rtdrv_ext_get_ctl,
};
#endif


#if defined(CONFIG_RTK_IGMP_MLD_SNOOPING_MODULE) && defined(CONFIG_COMMON_RT_API)
#include <rtdrv_ext_igmpHook_netfilter.h>
static struct nf_sockopt_ops rtdrv_ext_igmpHook_sockopts = {
    .pf = PF_INET,
    .set_optmin = RTDRV_BASE_CTL+RTDRV_EXT_IGMPHOOK_OFFSET,
    .set_optmax = RTDRV_BASE_CTL+RTDRV_EXT_IGMPHOOKEND_OFFSET+1,
    .set = do_rtdrv_ext_igmpHook_set_ctl,
    .get_optmin = RTDRV_BASE_CTL+RTDRV_EXT_IGMPHOOK_OFFSET,
    .get_optmax = RTDRV_BASE_CTL+RTDRV_EXT_IGMPHOOKEND_OFFSET+1,
    .get = do_rtdrv_ext_igmpHook_get_ctl,
};
#endif


extern int io_mii_memory_write(uint32 memaddr,uint32 data);
extern uint32 io_mii_memory_read(uint32 memaddr);
extern int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value);
extern int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue);
extern void gpon_dbg_enable(int32 enable, uint32 level);
extern int32 sdktest_run_id(uint32 unit, uint32 start, uint32 end);


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
static int32
_classify_usrRuleField_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField)
{
    rtk_classify_field_t *field;
    rtk_classify_field_t *tmp_field;

    /* parameter check */
    RT_PARAM_CHK((NULL == pClassifyEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pClassifyField), RT_ERR_NULL_POINTER);

    field = (rtk_classify_field_t*)osal_alloc(sizeof(rtk_classify_field_t));
    osal_memcpy(field, pClassifyField, sizeof(rtk_classify_field_t));
    field->next = NULL;

    if(NULL == pClassifyEntry->field.pFieldHead)
    {
        pClassifyEntry->field.pFieldHead = field;
    }
    else
    {
        tmp_field = pClassifyEntry->field.pFieldHead;
        while(tmp_field->next != NULL)
            tmp_field = tmp_field->next;

        tmp_field->next = field;
    }

    return RT_ERR_OK;
} /* end of rtk_acl_usrRuleField_add */

static int32
_classify_usrRuleField_free(rtk_classify_cfg_t *pClassifyEntry)
{
    rtk_classify_field_t  *fieldThis;
    rtk_classify_field_t  *fieldNext;

    fieldThis = pClassifyEntry->field.pFieldHead;
    while(fieldThis != NULL)
    {
        fieldNext = fieldThis->next;
        osal_free(fieldThis);
        fieldThis = fieldNext;
    }

    pClassifyEntry->field.pFieldHead = NULL;

    return RT_ERR_OK;
}



/* Function Name:
 *      do_rtdrv_set_ctl
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
int32 do_rtdrv_set_ctl(struct sock *sk, int cmd, void *user, unsigned int len)
{
    int32                           ret = RT_ERR_FAILED;
    uint32                          *pWriteData;
    rtdrv_union_t                   buf;
    rtdrv_pktdbg_t                  *pPktDbg;
    rtk_i2c_eeprom_mirror_t         eepMirrorCfg;

    //osal_printf("do_rtdrv_set_ctl cmd=%d\n", cmd);

    switch(cmd)
    {

#ifndef CONFIG_XDSL_RG_DIAGSHELL
        /*DEBUG*/
        case RTDRV_DEBUG_ADDRESS_DUMP_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = ioal_mem32_debugMode_set(buf.unit_cfg.data);
            break;

         case RTDRV_DEBUG_EN_LOG_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_enable_set(buf.unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGLV_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_level_set(buf.unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGLVMASK_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_mask_set(buf.unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGTYPE_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_type_set(buf.unit_cfg.data);
            break;

        case RTDRV_DEBUG_LOGFORMAT_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_format_set(buf.unit_cfg.data);
            break;

        case RTDRV_DEBUG_MODMASK_SET:
            copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_unitCfg_t));
            ret = rt_log_moduleMask_set(buf.unit_cfg.data64);
            break;

        /*QoS*/
        case RTDRV_QOS_INIT:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_init();
            break;

        case RTDRV_QOS_PRISELGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priSelGroup_set(buf.qos_cfg.grpIdx, &buf.qos_cfg.weightOfPriSel);
            break;

        case RTDRV_QOS_PORTPRI_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri_set(buf.qos_cfg.port, buf.qos_cfg.intPri);
            break;

        case RTDRV_QOS_DSCPPRIREMAPGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpPriRemapGroup_set(buf.qos_cfg.grpIdx, buf.qos_cfg.dscp, buf.qos_cfg.intPri, buf.qos_cfg.dp);
            break;

        case RTDRV_QOS_1PPRIREMAPGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pPriRemapGroup_set(buf.qos_cfg.grpIdx, buf.qos_cfg.dot1pPri, buf.qos_cfg.intPri, buf.qos_cfg.dp);
            break;

        case RTDRV_QOS_PRIMAP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priMap_set(buf.qos_cfg.group, &buf.qos_cfg.pri2qid);
            break;

        case RTDRV_QOS_PORTPRIMAP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriMap_set(buf.qos_cfg.port, buf.qos_cfg.group);
            break;

        case RTDRV_QOS_1PREMARKENABLE_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarkEnable_set(buf.qos_cfg.port, buf.qos_cfg.enable);
            break;

        case RTDRV_QOS_1PREMARKGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarkGroup_set(buf.qos_cfg.grpIdx, buf.qos_cfg.intPri, buf.qos_cfg.dp, buf.qos_cfg.dot1pPri);
            break;

        case RTDRV_QOS_DSCPREMARKENABLE_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarkEnable_set(buf.qos_cfg.port, buf.qos_cfg.enable);
            break;

        case RTDRV_QOS_DSCPREMARKGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarkGroup_set(buf.qos_cfg.grpIdx, buf.qos_cfg.intPri, buf.qos_cfg.dp, buf.qos_cfg.dscp);
            break;

        case RTDRV_QOS_PORTDSCPREMARKSRCSEL_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDscpRemarkSrcSel_set(buf.qos_cfg.port, buf.qos_cfg.type);
            break;

        case RTDRV_QOS_DSCP2DSCPREMARKGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2DscpRemarkGroup_set(buf.qos_cfg.grpIdx, buf.qos_cfg.dscp, buf.qos_cfg.rmkDscp);
            break;

        case RTDRV_QOS_FWD2CPUPRIREMAP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_fwd2CpuPriRemap_set(buf.qos_cfg.intPri, buf.qos_cfg.rempPri);
            break;

        case RTDRV_QOS_SCHEDULINGQUEUE_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingQueue_set(buf.qos_cfg.port, &buf.qos_cfg.qweights);
            break;

        case RTDRV_QOS_PORTPRISELGROUP_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriSelGroup_set(buf.qos_cfg.port, buf.qos_cfg.priSelGrpIdx);
            break;

        case RTDRV_QOS_SCHEDULINGTYPE_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingType_set(buf.qos_cfg.queueType);
            break;

        case RTDRV_QOS_PORTDOT1PREMARKSRCSEL_SET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDot1pRemarkSrcSel_set(buf.qos_cfg.port, buf.qos_cfg.dot1pRmkSrc);
            break;

        /*LED*/
        case RTDRV_LED_INIT:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_init();
            break;

        case RTDRV_LED_OPERATION_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_operation_set(buf.led_cfg.opMode);
            break;

        case RTDRV_LED_SERIALMODE_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_serialMode_set(buf.led_cfg.active);
            break;

        case RTDRV_LED_BLINKRATE_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_blinkRate_set(buf.led_cfg.group, buf.led_cfg.blinkRate);
            break;

        case RTDRV_LED_CONFIG_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_config_set(buf.led_cfg.ledIdx, buf.led_cfg.type, &buf.led_cfg.config);
            break;

        case RTDRV_LED_MODEFORCE_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_modeForce_set(buf.led_cfg.ledIdx, buf.led_cfg.mode);
            break;

        case RTDRV_LED_PARALLELENABLE_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_parallelEnable_set(buf.led_cfg.ledIdx, buf.led_cfg.state);
            break;

        case RTDRV_LED_PONALARM_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_ponAlarm_set(buf.led_cfg.state);
            break;

        case RTDRV_LED_PONWARNING_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_ponWarning_set(buf.led_cfg.state);
            break;

        case RTDRV_LED_PONPORT_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_pon_port_set(buf.led_cfg.type, buf.led_cfg.mode, buf.led_cfg.blinkRate, &buf.led_cfg.config);
            break;
        case RTDRV_LED_PARALLELMODE_SET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_parallelMode_set(buf.led_cfg.ledIdx, buf.led_cfg.active);
            break;
        /*switch*/
        case RTDRV_SWITCH_INIT:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_init();
            break;

        case RTDRV_SWITCH_PORT2PORTMASK_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_port2PortMask_set(&buf.switch_cfg.portMask, buf.switch_cfg.portName);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PORT2PORTMASK_CLEAR:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_port2PortMask_clear(&buf.switch_cfg.portMask, buf.switch_cfg.portName);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_ALLPORTMASK_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_allPortMask_set(&buf.switch_cfg.portMask);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_ALLEXTPORTMASK_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_allExtPortMask_set(&buf.switch_cfg.portMask);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_MAXPKTLENLINKSPEED_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenLinkSpeed_set(buf.switch_cfg.speed, buf.switch_cfg.len);
            break;

        case RTDRV_SWITCH_MGMTMACADDR_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_mgmtMacAddr_set(&buf.switch_cfg.mac);
            break;
        case RTDRV_SWITCH_CHIP_RESET:
          copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_chip_reset();
         break;
        case RTDRV_SWITCH_MAXPKTLENBYPORT_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenByPort_set(buf.switch_cfg.portId, buf.switch_cfg.len);
            break;
        case RTDRV_SWITCH_CHANGEDUPLEX_SET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_changeDuplex_set(buf.switch_cfg.state);
            break;
        case RTDRV_SWITCH_SYSTEM_INIT:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_system_init(&buf.switch_cfg.mode);
            break;

        /*STP*/
        case RTDRV_STP_INIT:
            copy_from_user(&buf.stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_init();
            break;

        case RTDRV_STP_MSTPSTATE_SET:
            copy_from_user(&buf.stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpState_set(buf.stp_cfg.msti, buf.stp_cfg.port, buf.stp_cfg.stpState);
            break;

        /*PONMAC*/
        case RTDRV_PONMAC_INIT:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_init();
            break;

        case RTDRV_PONMAC_SWITCHBUFFER_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_switchBufferMode_set(buf.ponmac_cfg.bufferMode);
            break;


        case RTDRV_PONMAC_QUEUE_ADD:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_queue_add(&buf.ponmac_cfg.queue, &buf.ponmac_cfg.queueCfg);
            break;

        case RTDRV_PONMAC_QUEUE_DEL:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_queue_del(&buf.ponmac_cfg.queue);
            break;

        case RTDRV_PONMAC_FLOW2QUEUE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_flow2Queue_set(buf.ponmac_cfg.flow, &buf.ponmac_cfg.queue);
            break;

        case RTDRV_PONMAC_QUEUEDRAINOUT_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_queueDrainOut_set(&buf.ponmac_cfg.queue);
            break;

        case RTDRV_PONMAC_MODE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_mode_set(buf.ponmac_cfg.mode);
            break;

        case RTDRV_PONMAC_SERDESCDR_RESET:
            ret = rtk_ponmac_serdesCdr_reset();
            break;

        case RTDRV_PONMAC_BWTHRESHOLD_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_bwThreshold_set(buf.ponmac_cfg.bwThreshold, buf.ponmac_cfg.reqBwThreshold);
            break;

        case RTDRV_PONMAC_FLOWVALID_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_sidValid_set(buf.ponmac_cfg.flow, buf.ponmac_cfg.state);
            break;

        case RTDRV_PONMAC_SCHEDULINGTYPE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_schedulingType_set(buf.ponmac_cfg.queueType);
            break;

        case RTDRV_PONMAC_EGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrBandwidthCtrlRate_set(buf.ponmac_cfg.rate);
            break;

        case RTDRV_PONMAC_EGRSCHEDULEIDRATE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrScheduleIdRate_set(buf.ponmac_cfg.scheduleId, buf.ponmac_cfg.rate);
            break;

        case RTDRV_PONMAC_SYNCE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_synce_set(buf.ponmac_cfg.enable);
            break;

        case RTDRV_PONMAC_EGRBANDWIDTHCTRLINCLUDEIFG_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrBandwidthCtrlIncludeIfg_set(buf.ponmac_cfg.enable);
            break;

        case RTDRV_PONMAC_EGRSCHEDULEIDINCLUDEIFG_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrScheduleIdIncludeIfg_set(buf.ponmac_cfg.enable);
            break;

        case RTDRV_PONMAC_AGGTCONTSTATE_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_aggTcontState_set(buf.ponmac_cfg.scheduleId, buf.ponmac_cfg.state);
            break;

        case RTDRV_PONMAC_TXDISABLEGPIO_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_txDisableGpio_set(buf.ponmac_cfg.enable);
            break;

        case RTDRV_PONMAC_TXPOWERDISABLEGPIO_SET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_txPowerDisableGpio_set(buf.ponmac_cfg.enable);
            break;
		case RTDRV_PONMAC_TRANSCEIVER_TX_PWR_SCALE_SET:
			copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
			ret = rtk_ponmac_transceiver_tx_power_scale_set(buf.ponmac_cfg.data.tx_pwr_scale);
			break;

        /*VLAN*/
        case RTDRV_VLAN_INIT:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_init();
            break;

        case RTDRV_VLAN_CREATE:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_create(buf.vlan_cfg.vid);
            break;

        case RTDRV_VLAN_DESTROY:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_destroy(buf.vlan_cfg.vid);
            break;

        case RTDRV_VLAN_DESTROYALL:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_destroyAll(buf.vlan_cfg.restoreDefaultVlan);
            break;

        case RTDRV_VLAN_FID_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_fid_set(buf.vlan_cfg.vid, buf.vlan_cfg.fid);
            break;

        case RTDRV_VLAN_FIDMODE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_fidMode_set(buf.vlan_cfg.vid, buf.vlan_cfg.mode);
            break;

        case RTDRV_VLAN_PORT_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_port_set(buf.vlan_cfg.vid, &buf.vlan_cfg.member_portmask, &buf.vlan_cfg.untag_portmask);
            break;

        case RTDRV_VLAN_EXTPORT_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPort_set(buf.vlan_cfg.vid, &buf.vlan_cfg.ext_portmask);
            break;

        case RTDRV_VLAN_STG_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_stg_set(buf.vlan_cfg.vid, buf.vlan_cfg.stg);
            break;

        case RTDRV_VLAN_PRIORITY_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_priority_set(buf.vlan_cfg.vid, buf.vlan_cfg.priority);
            break;

        case RTDRV_VLAN_PRIORITYENABLE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_priorityEnable_set(buf.vlan_cfg.vid, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_POLICINGENABLE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_policingEnable_set(buf.vlan_cfg.vid, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_POLICINGMETERIDX_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_policingMeterIdx_set(buf.vlan_cfg.vid, buf.vlan_cfg.index);
            break;

        case RTDRV_VLAN_PORTACCEPTFRAMETYPE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portAcceptFrameType_set(buf.vlan_cfg.port, buf.vlan_cfg.acceptFrameType);
            break;

        case RTDRV_VLAN_VLANFUNCTIONENABLE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_vlanFunctionEnable_set(buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_PORTIGRFILTERENABLE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrFilterEnable_set(buf.vlan_cfg.port, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_LEAKY_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_leaky_set(buf.vlan_cfg.leakyType, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_PORTLEAKY_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portLeaky_set(buf.vlan_cfg.port, buf.vlan_cfg.leakyType, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_KEEPTYPE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_keepType_set(buf.vlan_cfg.keepType, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_PORTPVID_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPvid_set(buf.vlan_cfg.port, buf.vlan_cfg.pvid);
            break;

        case RTDRV_VLAN_EXTPORTPVID_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortPvid_set(buf.vlan_cfg.extPort, buf.vlan_cfg.pvid);
            break;

        case RTDRV_VLAN_PROTOGROUP_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_protoGroup_set(buf.vlan_cfg.protoGroupIdx, &buf.vlan_cfg.protoGroup);
            break;

        case RTDRV_VLAN_PORTPROTOVLAN_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portProtoVlan_set(buf.vlan_cfg.port, buf.vlan_cfg.protoGroupIdx, &buf.vlan_cfg.vlanCfg);
            break;

        case RTDRV_VLAN_TAGMODE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tagMode_set(buf.vlan_cfg.port, buf.vlan_cfg.tagMode);
            break;

        case RTDRV_VLAN_PORTFID_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portFid_set(buf.vlan_cfg.port, buf.vlan_cfg.enable, buf.vlan_cfg.fid);
            break;

        case RTDRV_VLAN_PORTPRIORITY_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPriority_set(buf.vlan_cfg.port, buf.vlan_cfg.priority);
            break;

        case RTDRV_VLAN_PORTEGRTAGKEEPTYPE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTagKeepType_set(buf.vlan_cfg.egr_port, &buf.vlan_cfg.igr_portmask, buf.vlan_cfg.type);
            break;

        case RTDRV_VLAN_TRANSPARENTENABLE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_transparentEnable_set(buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_CFIKEEPENABLE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_cfiKeepEnable_set(buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_RESERVEDVIDACTION_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_reservedVidAction_set(buf.vlan_cfg.action_vid0, buf.vlan_cfg.action_vid4095);
            break;

        case RTDRV_VLAN_TAGMODEIP4MC_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tagModeIp4mc_set(buf.vlan_cfg.port, buf.vlan_cfg.tagModeIpmc);
            break;

        case RTDRV_VLAN_TAGMODEIP6MC_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tagModeIp6mc_set(buf.vlan_cfg.port, buf.vlan_cfg.tagModeIpmc);
            break;

        case RTDRV_VLAN_LUTSVLANHASHSTATE_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_lutSvlanHashState_set(buf.vlan_cfg.vid, buf.vlan_cfg.enable);
            break;

        case RTDRV_VLAN_EXTPORTPROTOVLAN_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortProtoVlan_set(buf.vlan_cfg.extPort, buf.vlan_cfg.protoGroupIdx, &buf.vlan_cfg.vlanCfg);
            break;

        case RTDRV_VLAN_EXTPORTMASKINDEX_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortmaskIndex_set(buf.vlan_cfg.vid, buf.vlan_cfg.index);
            break;

        case RTDRV_VLAN_EXTPORTMASKCFG_SET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortmaskCfg_set(buf.vlan_cfg.index, &buf.vlan_cfg.ext_portmask);
            break;

        /* Port */
        case RTDRV_PORT_INIT:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_init();
            break;

        case RTDRV_PORT_PHYAUTONEGOENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyAutoNegoEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_PHYAUTONEGOABILITY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyAutoNegoAbility_set(buf.port_cfg.port, &buf.port_cfg.ability);
            break;

        case RTDRV_PORT_PHYFORCEMODEABILITY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyForceModeAbility_set(buf.port_cfg.port, buf.port_cfg.speed, buf.port_cfg.duplex, buf.port_cfg.flowControl);
            break;

        case RTDRV_PORT_PHYREG_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyReg_set(buf.port_cfg.port, buf.port_cfg.page, buf.port_cfg.reg, buf.port_cfg.data);
            break;

        case RTDRV_PORT_PHYMASTERSLAVE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyMasterSlave_set(buf.port_cfg.port, buf.port_cfg.masterSlave);
            break;

        case RTDRV_PORT_PHYTESTMODE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyTestMode_set(buf.port_cfg.port, buf.port_cfg.testMode);
            break;

        case RTDRV_PORT_ISOLATION_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolation_set(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            break;

        case RTDRV_PORT_ISOLATIONEXT_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationExt_set(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            break;

        case RTDRV_PORT_ISOLATIONL34_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationL34_set(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            break;

        case RTDRV_PORT_ISOLATIONEXTL34_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationExtL34_set(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            break;

        case RTDRV_PORT_ISOLATIONENTRY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationEntry_set(buf.port_cfg.mode, buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            break;

        case RTDRV_PORT_ISOLATIONENTRYEXT_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationEntryExt_set(buf.port_cfg.mode, buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            break;

        case RTDRV_PORT_ISOLATIONCTAGPKTCONFIG_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationCtagPktConfig_set(buf.port_cfg.mode);
            break;

        case RTDRV_PORT_ISOLATIONL34PKTCONFIG_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationL34PktConfig_set(buf.port_cfg.mode);
            break;

        case RTDRV_PORT_ISOLATIONIPMCLEAKY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationIpmcLeaky_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_ISOLATIONPORTLEAKY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationPortLeaky_set(buf.port_cfg.port, buf.port_cfg.type, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_ISOLATIONLEAKY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationLeaky_set(buf.port_cfg.type, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_MACREMOTELOOPBACKENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macRemoteLoopbackEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_MACLOCALLOOPBACKENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macLocalLoopbackEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_ADMINENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_adminEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_SPECIALCONGEST_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_specialCongest_set(buf.port_cfg.port, buf.port_cfg.second);
            break;

        case RTDRV_PORT_SPECIALCONGESTSTATUS_CLEAR:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_specialCongestStatus_clear(buf.port_cfg.port);
            break;

        case RTDRV_PORT_GREENENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_greenEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_PHYCROSSOVERMODE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyCrossOverMode_set(buf.port_cfg.port, buf.port_cfg.crossoverMode);
            break;

        case RTDRV_PORT_PHYPOWERDOWN_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyPowerDown_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_PHYPOWERDOWN_TRYSET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyPowerDown_tryset(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_ENHANCEDFID_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_enhancedFid_set(buf.port_cfg.port, buf.port_cfg.efid);
            break;
        case RTDRV_PORT_MACFORCEABILITY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macForceAbility_set(buf.port_cfg.port, buf.port_cfg.macAbility);
            break;
        case RTDRV_PORT_MACFORCEABILITYSTATE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macForceAbilityState_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;
        case RTDRV_PORT_MACEXTMODE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macExtMode_set(buf.port_cfg.port, buf.port_cfg.ext_mode);
            break;
        case RTDRV_PORT_MACEXTRGMIIDELAY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macExtRgmiiDelay_set(buf.port_cfg.port, buf.port_cfg.txDelay, buf.port_cfg.rxDelay);
            break;
        case RTDRV_PORT_GIGALITEENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_gigaLiteEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;
        case RTDRV_PORT_EEEENABLE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_eeeEnable_set(buf.port_cfg.port, buf.port_cfg.enable);
            break;

        case RTDRV_PORT_SERDESMODE_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesMode_set(buf.port_cfg.num, buf.port_cfg.cfg);
            break;

        case RTDRV_PORT_SERDESMODESPEED_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesModeSpeed_set(buf.port_cfg.num, buf.port_cfg.cfg, buf.port_cfg.speed);
            break;

        case RTDRV_PORT_SERDESNWAY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesNWay_set(buf.port_cfg.num, buf.port_cfg.cfg);
            break;

        case RTDRV_PORT_SERDESTXRXPOLARITY_SET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesTxRxPolarity_set(buf.port_cfg.num, buf.port_cfg.txPolarity, buf.port_cfg.rxPolarity);
            break;

        /*ACL*/
        case RTDRV_ACL_INIT:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_init();

            break;

        case RTDRV_ACL_TEMPLATE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_template_set(&buf.acl_cfg.aclTemplate);
            break;

        case RTDRV_ACL_FIELDSELECT_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_fieldSelect_set(&buf.acl_cfg.fieldEntry);
            break;

        case RTDRV_ACL_IGRRULEFIELD_ADD:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_usrRuleField_add(&buf.acl_cfg.aclRule, &buf.acl_cfg.aclField);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));

            break;

        case RTDRV_ACL_IGRRULEENTRY_ADD:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrRuleEntry_add(&buf.acl_cfg.aclRule);
            rtk_acl_usrRuleField_free(&buf.acl_cfg.aclRule);
            break;

        case RTDRV_ACL_IGRRULEENTRY_DEL:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrRuleEntry_del(buf.acl_cfg.index);
            break;

        case RTDRV_ACL_IGRRULEENTRY_DELALL:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrRuleEntry_delAll();
            break;

        case RTDRV_ACL_IGRUNMATCHACTION_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrUnmatchAction_set(buf.acl_cfg.port, buf.acl_cfg.action);
            break;

        case RTDRV_ACL_IGRSTATE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrState_set(buf.acl_cfg.port, buf.acl_cfg.state);
            break;

        case RTDRV_ACL_IPRANGE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ipRange_set(&buf.acl_cfg.ipRangeEntry);
            break;

        case RTDRV_ACL_VIDRANGE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_vidRange_set(&buf.acl_cfg.vidRangeEntry);
            break;

        case RTDRV_ACL_PORTRANGE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_portRange_set(&buf.acl_cfg.l4PortRangeEntry);
            break;

        case RTDRV_ACL_PACKETLENGTHRANGE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_packetLengthRange_set(&buf.acl_cfg.pktLenRangeEntry);
            break;

        case RTDRV_ACL_IGRRULEMODE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrRuleMode_set(buf.acl_cfg.mode);
            break;

        case RTDRV_ACL_IGRPERMITSTATE_SET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrPermitState_set(buf.acl_cfg.port, buf.acl_cfg.state);
            break;

    /** ADDR **/
        case RTDRV_REG_ADDRESS_SET:

            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));

#if defined(LINUX_KERNEL_MDIO_IO)
            io_mii_memory_write(buf.addr_cfg.address, buf.addr_cfg.value);
#else
            MEM32_WRITE(buf.addr_cfg.address, buf.addr_cfg.value);
#endif
            ret = RT_ERR_OK;
            break;


        case RTDRV_PHY_REG_SET:

            copy_from_user(&buf.phy_cfg, user, sizeof(rtdrv_phyCfg_t));

#if defined(LINUX_KERNEL_MDIO_IO)
            io_mii_phy_reg_write(buf.phy_cfg.phy_id,buf.phy_cfg.reg,buf.phy_cfg.value);
#endif
            ret = RT_ERR_OK;
            break;

        case RTDRV_SOC_ADDRESS_SET:
            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));
            MEM32_WRITE(buf.addr_cfg.address, buf.addr_cfg.value);
            ret = RT_ERR_OK;
            break;

#if (CONFIG_GPON_VERSION < 2)
        case RTDRV_GPON_SN_SET:
            copy_from_user(&buf.sn_cfg, user, sizeof(rtk_gpon_serialNumber_t));
            ret = rtk_gpon_serialNumber_set(&buf.sn_cfg);
            break;

        case RTDRV_GPON_PWD_SET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gpon_password_set(&buf.pwd_cfg);
            break;

        case RTDRV_GPON_PARA_SET:
            copy_from_user(&buf.pon_cfg, user, sizeof(rtdrv_ponPara_t));
            ret = rtk_gpon_parameter_set(buf.pon_cfg.type, &buf.pon_cfg.para);
            break;

        case RTDRV_GPON_ACTIVE:
            copy_from_user(&buf.init_state_cfg, user, sizeof(rtk_gpon_initialState_t));
            ret = rtk_gpon_activate(buf.init_state_cfg);
            break;

        case RTDRV_GPON_DEACTIVE:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_deActivate();
            break;

        case RTDRV_GPON_TCONT_ADD:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gpon_tcont_create(&buf.tcont_cfg.ind, &buf.tcont_cfg.attr);
            copy_to_user(user, &buf.tcont_cfg, sizeof(rtdrv_tcont_t));
            break;

        case RTDRV_GPON_TCONT_DEL:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gpon_tcont_destroy(&buf.tcont_cfg.ind);
            break;

        case RTDRV_GPON_DS_FLOW_ADD:
            copy_from_user(&buf.ds_flow_cfg, user, sizeof(rtdrv_dsFlow_t));
            ret = rtk_gpon_dsFlow_set(buf.ds_flow_cfg.flowId, &buf.ds_flow_cfg.attr);
            break;

        case RTDRV_GPON_US_FLOW_ADD:
            copy_from_user(&buf.us_flow_cfg, user, sizeof(rtdrv_usFlow_t));
            ret = rtk_gpon_usFlow_set(buf.us_flow_cfg.flowId, &buf.us_flow_cfg.attr);
            break;

        case RTDRV_GPON_BC_PASS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_broadcastPass_set(buf.state);
            break;

        case RTDRV_GPON_NON_MC_PASS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_nonMcastPass_set(buf.state);
            break;

        case RTDRV_GPON_IP_PTN_SET:
            copy_from_user(&buf.ip_ptn, user, sizeof(rtdrv_ip_ptn_t));
            ret = rtk_gpon_multicastAddrCheck_set(buf.ip_ptn.ipv4_pattern, buf.ip_ptn.ipv6_pattern);
            break;

        case RTDRV_GPON_FLT_MODE_SET:
            copy_from_user(&buf.filter_mode, user, sizeof(rtk_gpon_macTable_exclude_mode_t));
            ret = rtk_gpon_macFilterMode_set(buf.filter_mode);
            break;

        case RTDRV_GPON_FS_MODE_SET:
            copy_from_user(&buf.fs_mode, user, sizeof(rtdrv_fs_mode_t));
            ret = rtk_gpon_mcForceMode_set(buf.fs_mode.ipv4, buf.fs_mode.ipv6);
            break;

        case RTDRV_GPON_MAC_ENTRY_ADD:
            copy_from_user(&buf.mac_entry.entry, user, sizeof(rtk_gpon_macTable_entry_t));
            ret = rtk_gpon_macEntry_add(&buf.mac_entry.entry);
            break;

        case RTDRV_GPON_MAC_ENTRY_DEL:
            copy_from_user(&buf.mac_entry.entry, user, sizeof(rtk_gpon_macTable_entry_t));
            ret = rtk_gpon_macEntry_del(&buf.mac_entry.entry);
            break;

        case RTDRV_GPON_RDI_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_rdi_set(buf.state);
            break;

        case RTDRV_GPON_PWR_LEVEL_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_powerLevel_set((uint8)(buf.state));
            break;

        case RTDRV_GPON_TX_LASER_SET:
            copy_from_user(&buf.tx_laser, user, sizeof(rtk_gpon_laser_status_t));
            ret = rtk_gpon_txForceLaser_set(buf.state);
            break;

        case RTDRV_GPON_FS_IDLE_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForceIdle_set(buf.state);
            break;
#if 0
        case RTDRV_GPON_FS_PRBS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForcePRBS_set(buf.state);
            break;
#endif
        case RTDRV_GPON_INITIAL:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = rtk_gpon_driver_initialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_driver_initialize err: 0x%x\n\r",ret);
                return ret;
            }

            ret = rtk_gpon_device_initialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_device_initialize err: 0x%x\n\r",ret);
                return ret;
            }
            /*remove it, because if init irq will implict other irq such as rldp, ptp, ...etc*/
            #if 0
            ret = rtk_switch_irq_init(buf.index);
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_irq_reg err: 0x%x\n\r",ret);
                return ret;
            }
            #endif
            break;

        case RTDRV_GPON_DEINITIAL:

            /*remove it, because if init irq will implict other irq such as rldp, ptp, ...etc*/
            /*
                        ret = rtk_switch_irq_exit();
            */
            ret = rtk_gpon_device_deInitialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_device_deInitialize err: 0x%x\n\r",ret);
                return ret;
            }

            ret = rtk_gpon_driver_deInitialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_driver_deInitialize err: 0x%x\n\r",ret);
                return ret;
            }

            break;

        case RTDRV_GPON_DEBUG_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            gpon_dbg_enable(buf.state);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_UNIT_TEST:
            copy_from_user(&buf.index, user, sizeof(uint32));
            /*sdktest_run_id(0, buf.index, buf.index);*/
            ret = RT_ERR_OK;
            break;

#if defined(OLD_FPGA_DEFINED)
        case RTDRV_GPON_PKTGEN_CFG:
            copy_from_user(&buf.pkt_gen, user, sizeof(rtdrv_pktGen_t));
            ret = rtk_gpon_pktGen_cfg_set(buf.pkt_gen.item, buf.pkt_gen.tcont, buf.pkt_gen.buf_len, buf.pkt_gen.gem, buf.pkt_gen.omci);
            break;

        case RTDRV_GPON_PKTGEN_BUF:
            copy_from_user(&buf.pkt_gen, user, sizeof(rtdrv_pktGen_t));
            ret = rtk_gpon_pktGen_buf_set(buf.pkt_gen.item, buf.pkt_gen.buf, buf.pkt_gen.buf_len);
            break;

#endif
        case RTDRV_GPON_OMCI_TX:
            copy_from_user(&buf.omci, user, sizeof(rtk_gpon_omci_msg_t));
            ret = rtk_gpon_omci_tx(&buf.omci);
            break;

        case RTDRV_GPON_AUTO_TCONT_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoTcont_set(buf.state);
            break;

        case RTDRV_GPON_AUTO_BOH_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoBoh_set(buf.state);
            break;

        case RTDRV_GPON_EQD_OFFSET_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_eqdOffset_set(buf.state);
            break;
#else /*gponv2 version*/
#if defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_LUNA_G3_SERIES)
/*for user usage*/
    case RTDRV_GPON_SN_SET:
            copy_from_user(&buf.sn_cfg, user, sizeof(rtk_gpon_serialNumber_t));
            ret = rtk_gponapp_serialNumber_set(&buf.sn_cfg);
            break;

        case RTDRV_GPON_PWD_SET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gponapp_password_set(&buf.pwd_cfg);
            break;

        case RTDRV_GPON_PARA_SET:
            copy_from_user(&buf.pon_cfg, user, sizeof(rtdrv_ponPara_t));
            ret = rtk_gponapp_parameter_set(buf.pon_cfg.type, &buf.pon_cfg.para);
            break;

        case RTDRV_GPON_ACTIVE:
            copy_from_user(&buf.init_state_cfg, user, sizeof(rtk_gpon_initialState_t));
            ret = rtk_gponapp_activate(buf.init_state_cfg);
            break;

        case RTDRV_GPON_DEACTIVE:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_deActivate();
            break;

        case RTDRV_GPON_TCONT_ADD:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gponapp_tcont_create(&buf.tcont_cfg.ind, &buf.tcont_cfg.attr);
            copy_to_user(user, &buf.tcont_cfg, sizeof(rtdrv_tcont_t));
            break;

        case RTDRV_GPON_TCONT_DEL:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gponapp_tcont_destroy(&buf.tcont_cfg.ind);
            break;

        case RTDRV_GPON_DS_FLOW_ADD:
            copy_from_user(&buf.ds_flow_cfg, user, sizeof(rtdrv_dsFlow_t));
            ret = rtk_gponapp_dsFlow_set(buf.ds_flow_cfg.flowId, &buf.ds_flow_cfg.attr);
            break;

        case RTDRV_GPON_US_FLOW_ADD:
            copy_from_user(&buf.us_flow_cfg, user, sizeof(rtdrv_usFlow_t));
            ret = rtk_gponapp_usFlow_set(buf.us_flow_cfg.flowId, &buf.us_flow_cfg.attr);
            break;

        case RTDRV_GPON_BC_PASS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_broadcastPass_set(buf.state);
            break;

        case RTDRV_GPON_NON_MC_PASS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_nonMcastPass_set(buf.state);
            break;

        case RTDRV_GPON_IP_PTN_SET:
            copy_from_user(&buf.ip_ptn, user, sizeof(rtdrv_ip_ptn_t));
            ret = rtk_gponapp_multicastAddrCheck_set(buf.ip_ptn.ipv4_pattern, buf.ip_ptn.ipv6_pattern);
            break;

        case RTDRV_GPON_FLT_MODE_SET:
            copy_from_user(&buf.filter_mode, user, sizeof(rtk_gpon_macTable_exclude_mode_t));
            ret = rtk_gponapp_macFilterMode_set(buf.filter_mode);
            break;

        case RTDRV_GPON_FS_MODE_SET:
            copy_from_user(&buf.fs_mode, user, sizeof(rtdrv_fs_mode_t));
            ret = rtk_gponapp_mcForceMode_set(buf.fs_mode.ipv4, buf.fs_mode.ipv6);
            break;

        case RTDRV_GPON_MAC_ENTRY_ADD:
            copy_from_user(&buf.mac_entry.entry, user, sizeof(rtk_gpon_macTable_entry_t));
            ret = rtk_gponapp_macEntry_add(&buf.mac_entry.entry);
            break;

        case RTDRV_GPON_MAC_ENTRY_DEL:
            copy_from_user(&buf.mac_entry.entry, user, sizeof(rtk_gpon_macTable_entry_t));
            ret = rtk_gponapp_macEntry_del(&buf.mac_entry.entry);
            break;

        case RTDRV_GPON_RDI_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_rdi_set(buf.state);
            break;

        case RTDRV_GPON_PWR_LEVEL_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_powerLevel_set((uint8)(buf.state));
            break;

        case RTDRV_GPON_TX_LASER_SET:
            copy_from_user(&buf.tx_laser, user, sizeof(rtk_gpon_laser_status_t));
            ret = rtk_gponapp_txForceLaser_set(buf.state);
            break;

        case RTDRV_GPON_FS_IDLE_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_txForceIdle_set(buf.state);
            break;

        case RTDRV_GPON_FS_PRBS_SET:
            copy_from_user(&buf.prbs_cfg, user, sizeof(rtk_gpon_prbs_t));
            ret = rtk_gponapp_txForcePRBS_set(buf.prbs_cfg);
            break;

        case RTDRV_GPON_INITIAL:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = rtk_gponapp_driver_initialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gponapp_driver_initialize err: 0x%x\n\r",ret);
                return ret;
            }

            ret = rtk_gponapp_device_initialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gponapp_device_initialize err: 0x%x\n\r",ret);
                return ret;
            }

            break;

        case RTDRV_GPON_DEINITIAL:
            ret = rtk_gponapp_device_deInitialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_device_deInitialize err: 0x%x\n\r",ret);
                return ret;
            }

            ret = rtk_gponapp_driver_deInitialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_driver_deInitialize err: 0x%x\n\r",ret);
                return ret;
            }

            break;

        case RTDRV_GPON_DEBUG_SET:
            copy_from_user(&buf.log_cfg, user, sizeof(rtdrv_logCfg_t));
            gpon_dbg_enable(buf.log_cfg.log_format, buf.log_cfg.log_level);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_UNIT_TEST:
            copy_from_user(&buf.index, user, sizeof(uint32));
            /*sdktest_run_id(0, buf.index, buf.index);*/
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_OMCI_TX:
            copy_from_user(&buf.omci, user, sizeof(rtdrv_omci_msg_t));
            ret = rtk_gponapp_omci_tx((rtk_gpon_omci_msg_t *)&buf.omci);
            break;

        case RTDRV_GPON_AUTO_TCONT_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_autoTcont_set(buf.state);
            break;

        case RTDRV_GPON_AUTO_BOH_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_autoBoh_set(buf.state);
            break;

        case RTDRV_GPON_EQD_OFFSET_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_eqdOffset_set(buf.state);
            break;
        case RTDRV_GPON_AUTODISTX_SET:
            copy_from_user(&buf.state,user, sizeof(int32));
            ret = rtk_gpon_autoDisTx_set(buf.state);
            break;
        case RTDRV_GPON_EXTMSG_SET:
            copy_from_user(&buf.gponExt_cfg, user, sizeof(rtk_gpon_extMsg_t));
            ret = rtk_gponapp_extMsg_set(&buf.gponExt_cfg);
            break;
        case RTDRV_GPON_RGIGMP_CFGMSG_SET:
            copy_from_user(&buf.rgIgmp_cfg, user, sizeof(rtk_gpon_rgIgmp_cfg_msg_t));
            if(0 != pkt_redirect_kernelApp_sendPkt(PR_KERNEL_UID_TR142,
                0, sizeof(rtk_gpon_rgIgmp_cfg_msg_t), &buf.rgIgmp_cfg))
                //printk("%s() %d: send registered command failed !! \n", __FUNCTION__, __LINE__);
            ret = RT_ERR_OK;
            break;
        case RTDRV_GPON_DBRU_BLOCK_SIZE_SET:
            copy_from_user(&buf.state,user, sizeof(int32));
            ret = rtk_gponapp_dbruBlockSize_set(buf.state);
            break;
        case RTDRV_GPON_ROGUE_ONT_SET:
            copy_from_user(&buf.state,user, sizeof(int32));
            ret = rtk_gponapp_rogueOnt_set(buf.state);
            break;
        case RTDRV_GPON_BER_TIMER_ENABLE_SET:
            copy_from_user(&buf.state,user, sizeof(int32));
            ret = rtk_gponapp_berTimerEnable_set(buf.state);
            break;
#endif
#if defined(CONFIG_LUNA_G3_SERIES)
        case RTDRV_GPON_ACTIVE:
            copy_from_user(&buf.init_state_cfg, user, sizeof(rtk_gpon_initialState_t));
            ret = rtk_gpon_onuState_set(buf.init_state_cfg);
            break;

        case RTDRV_GPON_DEACTIVE:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_onuState_set(RTK_GPONMAC_INIT_STATE_O7);
            break;

        case RTDRV_GPON_PWD_SET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gpon_password_set(&buf.pwd_cfg);
            break;

#endif
#endif
        case RTDRV_PKT_RXDUMP_ENABLE:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = mac_debug_rxPkt_enable (buf.index);
            break;
#if defined(CONFIG_SDK_RTL9607C)
        case RTDRV_PKT_RXGMAC_SET:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = mac_debug_rxGmac_set (buf.index);
            break;
#endif
        case RTDRV_PKT_SEND:
            pPktDbg = (rtdrv_pktdbg_t *) kmalloc(sizeof(rtdrv_pktdbg_t), GFP_KERNEL);
            if(NULL == pPktDbg)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(pPktDbg, user, sizeof(rtdrv_pktdbg_t));
            ret = mac_debug_txPkt_send(&pPktDbg->buf[0], pPktDbg->length, &pPktDbg->tx_info, (pPktDbg->tx_count >= 0xffffffff) ? 0xffffffff : pPktDbg->tx_count);
            kfree(pPktDbg);
            break;

        case RTDRV_PKT_CONTINUS_SEND:
            pPktDbg = (rtdrv_pktdbg_t *) kmalloc(sizeof(rtdrv_pktdbg_t), GFP_KERNEL);
            if(NULL == pPktDbg)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(pPktDbg, user, sizeof(rtdrv_pktdbg_t));
            ret = mac_debug_txPktCont_send(&pPktDbg->buf[0], (pPktDbg->length >= 2048) ? 2048 : pPktDbg->length, &pPktDbg->tx_info,pPktDbg->tx_enable);
            kfree(pPktDbg);
            break;

        case RTDRV_PKT_CONTINUSCNT_SET:
            pPktDbg = (rtdrv_pktdbg_t *) kmalloc(sizeof(rtdrv_pktdbg_t), GFP_KERNEL);
            if(NULL == pPktDbg)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(&pPktDbg->tx_count, user, sizeof(uint32));
            ret = mac_debug_txPktContCnt_set(pPktDbg->tx_count);
            kfree(pPktDbg);
            break;

        case RTDRV_PKT_RXDUMP_CLEAR:
            ret = mac_debug_rxPkt_clear();
            break;

        /*rate*/
        case RTDRV_RATE_INIT:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_init();
            break;

        case RTDRV_RATE_PORTIGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthCtrlRate_set(buf.rate_cfg.port, buf.rate_cfg.rate);
            break;

        case RTDRV_RATE_PORTIGRBANDWIDTHCTRLINCLUDEIFG_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(buf.rate_cfg.port, buf.rate_cfg.ifgInclude);
            break;

        case RTDRV_RATE_PORTEGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBandwidthCtrlRate_set(buf.rate_cfg.port, buf.rate_cfg.rate);
            break;

        case RTDRV_RATE_EGRBANDWIDTHCTRLINCLUDEIFG_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_set(buf.rate_cfg.ifgInclude);
            break;

        case RTDRV_RATE_PORTEGRBANDWIDTHCTRLINCLUDEIFG_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBandwidthCtrlIncludeIfg_set(buf.rate_cfg.port, buf.rate_cfg.ifgInclude);
            break;

        case RTDRV_RATE_EGRQUEUEBWCTRLENABLE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlEnable_set(buf.rate_cfg.port, buf.rate_cfg.queue, buf.rate_cfg.enable);
            break;

        case RTDRV_RATE_EGRQUEUEBWCTRLMETERIDX_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlMeterIdx_set(buf.rate_cfg.port, buf.rate_cfg.queue, buf.rate_cfg.meterIndex);
            break;

        case RTDRV_RATE_EGRQUEUEBWCTRLRATE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlRate_set(buf.rate_cfg.port, buf.rate_cfg.queue, buf.rate_cfg.rate);
            break;

        case RTDRV_RATE_STORMCONTROLMETERIDX_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlMeterIdx_set(buf.rate_cfg.port, buf.rate_cfg.stormType, buf.rate_cfg.index);
            break;

        case RTDRV_RATE_STORMCONTROLPORTENABLE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlPortEnable_set(buf.rate_cfg.port, buf.rate_cfg.stormType, buf.rate_cfg.enable);
            break;

        case RTDRV_RATE_STORMCONTROLENABLE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlEnable_set(&buf.rate_cfg.stormCtrl);
            break;

        case RTDRV_RATE_STORMBYPASS_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormBypass_set(buf.rate_cfg.type, buf.rate_cfg.enable);
            break;

        case RTDRV_RATE_SHAREMETER_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeter_set(buf.rate_cfg.index, buf.rate_cfg.rate, buf.rate_cfg.ifgInclude);
            break;

        case RTDRV_RATE_SHAREMETERBUCKET_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeterBucket_set(buf.rate_cfg.index, buf.rate_cfg.bucketSize);
            break;

        case RTDRV_RATE_SHAREMETEREXCEED_CLEAR:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeterExceed_clear(buf.rate_cfg.index);
            break;
        case RTDRV_RATE_SHAREMETERMODE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeterMode_set(buf.rate_cfg.index, buf.rate_cfg.meterMode);
            break;
        case RTDRV_RATE_HOSTIGRBWCTRLSTATE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostIgrBwCtrlState_set(buf.rate_cfg.index, buf.rate_cfg.enable);
            break;
        case RTDRV_RATE_HOSTEGRBWCTRLSTATE_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostEgrBwCtrlState_set(buf.rate_cfg.index, buf.rate_cfg.enable);
            break;
        case RTDRV_RATE_HOSTBWCTRLMETERIDX_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostBwCtrlMeterIdx_set(buf.rate_cfg.index, buf.rate_cfg.meterIndex);
            break;
        case RTDRV_RATE_HOSTMACADDR_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostMacAddr_set(buf.rate_cfg.index, &buf.rate_cfg.mac);
            break;
        case RTDRV_RATE_HOSTIGRBWCTRLMETERIDX_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostIgrBwCtrlMeterIdx_set(buf.rate_cfg.index, buf.rate_cfg.meterIndex);
            break;
        case RTDRV_RATE_HOSTEGRBWCTRLMETERIDX_SET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostEgrBwCtrlMeterIdx_set(buf.rate_cfg.index, buf.rate_cfg.meterIndex);
            break;



       case RTDRV_GPIO_INIT:
          copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_init();
            break;
         case RTDRV_GPIO_STATE_SET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_state_set(buf.gpio_cfg.gpioId, buf.gpio_cfg.enable);
            break;
        case RTDRV_GPIO_MODE_SET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_mode_set(buf.gpio_cfg.gpioId, buf.gpio_cfg.mode);
            break;
        case RTDRV_GPIO_DATABIT_SET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_databit_set(buf.gpio_cfg.gpioId, buf.gpio_cfg.data);
            break;
        case RTDRV_GPIO_IMS_SET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_intrStatus_clean(buf.gpio_cfg.gpioId);
            break;
        case RTDRV_GPIO_INTR_SET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_intr_set(buf.gpio_cfg.gpioId,buf.gpio_cfg.intrMode);
            break;
        case RTDRV_I2C_INIT:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_init(buf.i2c_cfg.port);
            break;

        case RTDRV_I2C_ENABLE_SET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_enable_set(buf.i2c_cfg.port, buf.i2c_cfg.enable);
            break;

        case RTDRV_I2C_WIDTH_SET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_width_set(buf.i2c_cfg.port, buf.i2c_cfg.width);
            break;

        case RTDRV_I2C_WRITE:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_write(buf.i2c_cfg.port, buf.i2c_cfg.devID, buf.i2c_cfg.regAddr, buf.i2c_cfg.data);
            break;

        case RTDRV_I2C_NOWAIT_WRITE:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_noWait_write(buf.i2c_cfg.port, buf.i2c_cfg.devID, buf.i2c_cfg.regAddr, buf.i2c_cfg.data);
            break;

        case RTDRV_I2C_SEQ_WRITE:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            pWriteData = (uint32 *) kmalloc(sizeof(uint32) * (buf.i2c_cfg.seqCnt >= 256 ? 256 : buf.i2c_cfg.seqCnt), GFP_KERNEL);
            if(NULL == pWriteData)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(pWriteData, buf.i2c_cfg.pSeqData, sizeof(uint32) * (buf.i2c_cfg.seqCnt >= 256 ? 256 : buf.i2c_cfg.seqCnt));
            ret = rtk_i2c_seq_write(buf.i2c_cfg.port, buf.i2c_cfg.devID, buf.i2c_cfg.regAddr, pWriteData, buf.i2c_cfg.seqCnt);
            kfree(pWriteData);
            break;

        case RTDRV_I2C_CLOCK_SET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_clock_set(buf.i2c_cfg.port, buf.i2c_cfg.clock);
            break;

        case RTDRV_I2C_EEPMIRROR_SET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_eepMirror_write(buf.i2c_cfg.regAddr, buf.i2c_cfg.data);
            eepMirrorCfg.devId = buf.i2c_cfg.devID;
            eepMirrorCfg.addrWidth = buf.i2c_cfg.width;
            eepMirrorCfg.addrExtMode = buf.i2c_cfg.addrExtMode;
            ret = rtk_i2c_eepMirror_set(eepMirrorCfg);
            break;

        case RTDRV_I2C_EEPMIRROR_WRITE:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_eepMirror_write(buf.i2c_cfg.regAddr, buf.i2c_cfg.data);
            break;

        case RTDRV_I2C_DATAWIDTH_SET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_dataWidth_set(buf.i2c_cfg.port, buf.i2c_cfg.width);
            break;

        case RTDRV_I2C_ADDRWIDTH_SET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_addrWidth_set(buf.i2c_cfg.port, buf.i2c_cfg.width);
            break;

        case RTDRV_INTR_INIT:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_init();
            break;

        case RTDRV_INTR_POLARITY_SET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_polarity_set(buf.intr_cfg.polar);
            break;

        case RTDRV_INTR_IMR_SET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_imr_set(buf.intr_cfg.intr, buf.intr_cfg.enable);
            break;

        case RTDRV_INTR_IMS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_ims_clear(buf.intr_cfg.intr);
            break;

        case RTDRV_INTR_SPEEDCHANGESTATUS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_speedChangeStatus_clear();
            break;

        case RTDRV_INTR_LINKUPSTATUS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_linkupStatus_clear();
            break;

        case RTDRV_INTR_LINKUPPORTSTATUS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_linkupPortStatus_clear(buf.intr_cfg.portMask);
            break;

        case RTDRV_INTR_LINKDOWNSTATUS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_linkdownStatus_clear();
            break;

        case RTDRV_INTR_LINKDOWNPORTSTATUS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_linkdownPortStatus_clear(buf.intr_cfg.portMask);
            break;

        case RTDRV_INTR_GPHYSTATUS_CLEAR:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_gphyStatus_clear();
            break;
        case RTDRV_INTR_IMR_RESTORE:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_imr_restore(buf.intr_cfg.imrValue);
            break;

        case RTDRV_INTR_ISR_SET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_isr_set(buf.intr_cfg.type, buf.intr_cfg.enable);
            break;

        case RTDRV_SVLAN_INIT:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_init();
            break;

        case RTDRV_SVLAN_CREATE:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_create(buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_DESTROY:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_destroy(buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_PORTSVID_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_portSvid_set(buf.svlan_cfg.port, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_SERVICEPORT_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_servicePort_set(buf.svlan_cfg.port, buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_MEMBERPORT_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_memberPort_set(buf.svlan_cfg.svid, &buf.svlan_cfg.svlanPortmask, &buf.svlan_cfg.svlanUntagPortmask);
            break;

        case RTDRV_SVLAN_TPIDENTRY_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_tpidEntry_set(buf.svlan_cfg.svlanIndex, buf.svlan_cfg.svlan_tag_id);
            break;

        case RTDRV_SVLAN_PRIORITYREF_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_priorityRef_set(buf.svlan_cfg.ref);
            break;

        case RTDRV_SVLAN_MEMBERPORTENTRY_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_memberPortEntry_set(&buf.svlan_cfg.svlan_cfg);
            break;

        case RTDRV_SVLAN_IPMC2S_ADD:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_ipmc2s_add(buf.svlan_cfg.ipmc, buf.svlan_cfg.ipmcMsk, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_IPMC2S_DEL:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_ipmc2s_del(buf.svlan_cfg.ipmc, buf.svlan_cfg.ipmcMsk);
            break;

        case RTDRV_SVLAN_L2MC2S_ADD:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_l2mc2s_add(buf.svlan_cfg.mac, buf.svlan_cfg.macMsk, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_L2MC2S_DEL:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_l2mc2s_del(buf.svlan_cfg.mac, buf.svlan_cfg.macMsk);
            break;

        case RTDRV_SVLAN_SP2C_ADD:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2c_add(buf.svlan_cfg.svid, buf.svlan_cfg.dstPort, buf.svlan_cfg.cvid);
            break;
        case RTDRV_SVLAN_SP2CPRIORITY_ADD:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2cPriority_add(buf.svlan_cfg.svid, buf.svlan_cfg.dstPort, buf.svlan_cfg.priority);
            break;

        case RTDRV_SVLAN_SP2C_DEL:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2c_del(buf.svlan_cfg.svid, buf.svlan_cfg.dstPort);
            break;

        case RTDRV_SVLAN_DMACVIDSELSTATE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_dmacVidSelState_set(buf.svlan_cfg.port, buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_UNMATCHACTION_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_unmatchAction_set(buf.svlan_cfg.action, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_UNTAGACTION_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_untagAction_set(buf.svlan_cfg.action, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_C2S_ADD:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_c2s_add(buf.svlan_cfg.cvid, buf.svlan_cfg.port, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_C2S_DEL:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_c2s_del(buf.svlan_cfg.cvid, buf.svlan_cfg.port, buf.svlan_cfg.svid);
            break;

        case RTDRV_SVLAN_TRAPPRI_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_trapPri_set(buf.svlan_cfg.priority);
            break;

        case RTDRV_SVLAN_DEIKEEPSTATE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_deiKeepState_set(buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_LOOKUPTYPE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_lookupType_set(buf.svlan_cfg.type);
            break;

        case RTDRV_SVLAN_SP2CUNMATCHCTAGGING_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2cUnmatchCtagging_set(buf.svlan_cfg.state);
            break;

        case RTDRV_SVLAN_PRIORITY_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_priority_set(buf.svlan_cfg.svid, buf.svlan_cfg.priority);
            break;

        case RTDRV_SVLAN_FID_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_fid_set(buf.svlan_cfg.svid, buf.svlan_cfg.fid);
            break;

        case RTDRV_SVLAN_FIDENABLE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_fidEnable_set(buf.svlan_cfg.svid, buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_ENHANCEDFID_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_enhancedFid_set(buf.svlan_cfg.svid, buf.svlan_cfg.efid);
            break;

        case RTDRV_SVLAN_ENHANCEDFIDENABLE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_enhancedFidEnable_set(buf.svlan_cfg.svid, buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_DMACVIDSELFORCEDSTATE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_dmacVidSelForcedState_set(buf.svlan_cfg.enable);
            break;
        case RTDRV_SVLAN_SVLANFUNCTIONENABLE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_svlanFunctionEnable_set(buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_TPIDENABLE_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_tpidEnable_set(buf.svlan_cfg.svlanIndex, buf.svlan_cfg.enable);
            break;

        case RTDRV_SVLAN_EXTPORTPVID_SET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_extPortPvid_set(buf.svlan_cfg.extPort, buf.svlan_cfg.pvid);
            break;

        /* RLDP */
        case RTDRV_RLDP_INIT:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_init();
            break;

        case RTDRV_RLDP_CONFIG_SET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_config_set(&buf.rldp_cfg.config);
            break;

        case RTDRV_RLDP_PORTCONFIG_SET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_portConfig_set(buf.rldp_cfg.port, &buf.rldp_cfg.portConfig);
            break;

        case RTDRV_RLDP_PORTSTATUS_CLEAR:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_portStatus_clear(buf.rldp_cfg.port, &buf.rldp_cfg.portStatus);
            break;

        case RTDRV_RLPP_INIT:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rlpp_init();
            break;

        case RTDRV_RLPP_TRAPTYPE_SET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rlpp_trapType_set(buf.rldp_cfg.type);
            break;

        /* Classify */
        case RTDRV_CLASSIFY_INIT:
            ret = rtk_classify_init();
            break;

        case RTDRV_CLASSIFY_CFGENTRY_ADD:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfgEntry_add(&buf.classify_cfg.classifyCfg);
            _classify_usrRuleField_free(&buf.classify_cfg.classifyCfg);
            break;

        case RTDRV_CLASSIFY_CFGENTRY_DEL:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfgEntry_del(buf.classify_cfg.entryIdx);
            break;

        case RTDRV_CLASSIFY_FIELD_ADD:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = _classify_usrRuleField_add(&buf.classify_cfg.classifyCfg, &buf.classify_cfg.classifyField);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_UNMATCHACTION_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_unmatchAction_set(buf.classify_cfg.action);
            break;

        case RTDRV_CLASSIFY_UNMATCHACTION_DS_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_unmatchAction_ds_set(buf.classify_cfg.dsaction);
            break;

        case RTDRV_CLASSIFY_PORTRANGE_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_portRange_set(&buf.classify_cfg.rangeEntry);
            break;

        case RTDRV_CLASSIFY_IPRANGE_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_ipRange_set(&buf.classify_cfg.ipRangeEntry);
            break;

        case RTDRV_CLASSIFY_CFSEL_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfSel_set(buf.classify_cfg.port, buf.classify_cfg.cfSel);
            break;

        case RTDRV_CLASSIFY_CFPRI2DSCP_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfPri2Dscp_set(buf.classify_cfg.pri, buf.classify_cfg.dscp);
            break;

        case RTDRV_CLASSIFY_PERMIT_SEL_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_permit_sel_set(buf.classify_cfg.permitSel);
            break;

        case RTDRV_CLASSIFY_US1PREMARK_PRIOR_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_us1pRemarkPrior_set(buf.classify_cfg.prior);
            break;

        case RTDRV_CLASSIFY_TEMPLATECFGPATTERN0_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_templateCfgPattern0_set(&buf.classify_cfg.p0TemplateCfg);
            break;

        case RTDRV_CLASSIFY_ENTRYNUMPATTERN1_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_entryNumPattern1_set(buf.classify_cfg.entryNum);
            break;

        case RTDRV_CLASSIFY_DEFAULTWANIF_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_defaultWanIf_set(&buf.classify_cfg.defaultWanIf);
            break;
    case RTDRV_CLASSIFY_TRAPPRI_SET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_trapPri_set(buf.classify_cfg.trapPri);
            break;

        /* L2 */
        case RTDRV_L2_INIT:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_init();
            break;

        case RTDRV_L2_FLUSHLINKDOWNPORTADDRENABLE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_flushLinkDownPortAddrEnable_set(buf.l2_cfg.enable);
            break;

        case RTDRV_L2_TABLE_CLEAR:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_table_clear();
            break;

        case RTDRV_L2_LIMITLEARNINGOVERSTATUS_CLEAR:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningOverStatus_clear();
            break;

        case RTDRV_L2_LIMITLEARNINGCNT_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningCnt_set(buf.l2_cfg.macCnt);
            break;

        case RTDRV_L2_LIMITLEARNINGCNTACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningCntAction_set(buf.l2_cfg.learningAction);
            break;

        case RTDRV_L2_LIMITLEARNINGENTRYACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningEntryAction_set(buf.l2_cfg.learningEntryAction);
            break;

        case RTDRV_L2_LIMITLEARNINGPORTMASK_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningPortMask_set(buf.l2_cfg.portmask);
            break;

        case RTDRV_L2_PORTLIMITLEARNINGOVERSTATUS_CLEAR:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLimitLearningOverStatus_clear(buf.l2_cfg.port);
            break;

        case RTDRV_L2_PORTLIMITLEARNINGCNT_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLimitLearningCnt_set(buf.l2_cfg.port, buf.l2_cfg.macCnt);
            break;

        case RTDRV_L2_PORTLIMITLEARNINGCNTACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLimitLearningCntAction_set(buf.l2_cfg.port, buf.l2_cfg.learningAction);
            break;

        case RTDRV_L2_AGING_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_aging_set(buf.l2_cfg.agingTime);
            break;

        case RTDRV_L2_PORTAGINGENABLE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portAgingEnable_set(buf.l2_cfg.port, buf.l2_cfg.enable);
            break;

        case RTDRV_L2_LOOKUPMISSACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissAction_set(buf.l2_cfg.type, buf.l2_cfg.action);
            break;

        case RTDRV_L2_PORTLOOKUPMISSACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLookupMissAction_set(buf.l2_cfg.port, buf.l2_cfg.type, buf.l2_cfg.action);
            break;

        case RTDRV_L2_LOOKUPMISSFLOODPORTMASK_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_set(buf.l2_cfg.type, &buf.l2_cfg.flood_portmask);
            break;

        case RTDRV_L2_LOOKUPMISSFLOODPORTMASK_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_add(buf.l2_cfg.type, buf.l2_cfg.flood_port);
            break;

        case RTDRV_L2_LOOKUPMISSFLOODPORTMASK_DEL:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_del(buf.l2_cfg.type, buf.l2_cfg.flood_port);
            break;

        case RTDRV_L2_NEWMACOP_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_newMacOp_set(buf.l2_cfg.port, buf.l2_cfg.lrnMode, buf.l2_cfg.fwdAction);
            break;

        case RTDRV_L2_ADDR_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_add(&buf.l2_cfg.l2Addr);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_DEL:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_del(&buf.l2_cfg.l2Addr);
            break;

        case RTDRV_L2_ADDR_DELALL:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_delAll(buf.l2_cfg.includeStatic);
            break;

        case RTDRV_L2_MCASTADDR_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_add(&buf.l2_cfg.mcastAddr);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCASTADDR_DEL:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_del(&buf.l2_cfg.mcastAddr);
            break;

        case RTDRV_L2_EXTMEMBERCONFIG_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_extMemberConfig_set(buf.l2_cfg.index, buf.l2_cfg.portmask);
            break;

        case RTDRV_L2_ILLEGALPORTMOVEACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_illegalPortMoveAction_set(buf.l2_cfg.port, buf.l2_cfg.fwdAction);
            break;

        case RTDRV_L2_VIDUNMATCHACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_vidUnmatchAction_set(buf.l2_cfg.port, buf.l2_cfg.fwdAction);
            break;

        case RTDRV_L2_IPMCMODE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcMode_set(buf.l2_cfg.mode);
            break;

        case RTDRV_L2_IPMCVLANMODE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcVlanMode_set(buf.l2_cfg.mode);
            break;

        case RTDRV_L2_IPV6MCMODE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipv6mcMode_set(buf.l2_cfg.mode);
            break;

        case RTDRV_L2_IPMCGROUPLOOKUPMISSHASH_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroupLookupMissHash_set(buf.l2_cfg.ipmcHash);
            break;

        case RTDRV_L2_IPMCGROUP_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroup_add(buf.l2_cfg.ip, &buf.l2_cfg.portmask);
            break;

        case RTDRV_L2_IPMCGROUPEXT_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroupExtPortmask_add(buf.l2_cfg.ip, &buf.l2_cfg.portmask);
            break;

        case RTDRV_L2_IPMCGROUPEXTIDX_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroupExtMemberIdx_add(buf.l2_cfg.ip, buf.l2_cfg.index);
            break;

        case RTDRV_L2_IPMCGROUP_DEL:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroup_del(buf.l2_cfg.ip);
            break;

        case RTDRV_L2_PORTIPMCACTION_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portIpmcAction_set(buf.l2_cfg.port, buf.l2_cfg.action);
            break;

        case RTDRV_L2_IPMCASTADDR_ADD:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipMcastAddr_add(&buf.l2_cfg.ipmcastAddr);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCASTADDR_DEL:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipMcastAddr_del(&buf.l2_cfg.ipmcastAddr);
            break;

        case RTDRV_L2_SRCPORTEGRFILTERMASK_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_srcPortEgrFilterMask_set(&buf.l2_cfg.filter_portmask);
            break;

        case RTDRV_L2_EXTPORTEGRFILTERMASK_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_extPortEgrFilterMask_set(&buf.l2_cfg.ext_portmask);
            break;

        case RTDRV_L2_CAM_STATE_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_camState_set(buf.l2_cfg.enable);
            break;

        case RTDRV_L2_IPMCSIPFILTER_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcSipFilter_set(buf.l2_cfg.index, buf.l2_cfg.ip);
            break;
        case RTDRV_L2_IP6MCRSVADDR_SET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ip6mcReservedAddrEnable_set(buf.l2_cfg.addrRsvIp6Mc, buf.l2_cfg.enable);
            break;

      /*L34 */
        case RTDRV_L34_INIT:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_init();
            break;

        case RTDRV_L34_NETIFTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_netifTable_set(buf.l34_cfg.idx, &buf.l34_cfg.netifEntry);
            break;

        case RTDRV_L34_ARPTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_arpTable_set(buf.l34_cfg.idx, &buf.l34_cfg.arpEntry);
            break;

        case RTDRV_L34_ARPTABLE_DEL:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_arpTable_del(buf.l34_cfg.idx);
            break;

        case RTDRV_L34_PPPOETABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_pppoeTable_set(buf.l34_cfg.idx, &buf.l34_cfg.ppEntry);
            break;

        case RTDRV_L34_ROUTINGTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_routingTable_set(buf.l34_cfg.idx, &buf.l34_cfg.routEntry);
            break;

        case RTDRV_L34_ROUTINGTABLE_DEL:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_routingTable_del(buf.l34_cfg.idx);
            break;

        case RTDRV_L34_NEXTHOPTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_nexthopTable_set(buf.l34_cfg.idx, &buf.l34_cfg.nextHopEntry);
            break;

        case RTDRV_L34_EXTINTIPTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_extIntIPTable_set(buf.l34_cfg.idx, &buf.l34_cfg.extIpEntry);
            break;

        case RTDRV_L34_EXTINTIPTABLE_DEL:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_extIntIPTable_del(buf.l34_cfg.idx);
            break;

        case RTDRV_L34_NAPTINBOUNDTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_naptInboundTable_set(buf.l34_cfg.forced, buf.l34_cfg.idx, &buf.l34_cfg.naptrEntry);
            break;

        case RTDRV_L34_NAPTOUTBOUNDTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_naptOutboundTable_set(buf.l34_cfg.forced, buf.l34_cfg.idx, &buf.l34_cfg.naptEntry);
            break;

        case RTDRV_L34_IPMCTRANSTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ipmcTransTable_set(buf.l34_cfg.idx, &buf.l34_cfg.ipmcEntry);
            break;

        case RTDRV_L34_TABLE_RESET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_table_reset(buf.l34_cfg.type);
            break;

        case RTDRV_L34_BINDINGTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_bindingTable_set(buf.l34_cfg.idx, &buf.l34_cfg.bindEntry);
            break;

        case RTDRV_L34_BINDINGACTION_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_bindingAction_set(buf.l34_cfg.bindType, buf.l34_cfg.bindAction);
            break;

        case RTDRV_L34_WANTYPETABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_wanTypeTable_set(buf.l34_cfg.idx, &buf.l34_cfg.wanTypeEntry);
            break;

        case RTDRV_L34_IPV6ROUTINGTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ipv6RoutingTable_set(buf.l34_cfg.idx, &buf.l34_cfg.ipv6RoutEntry);
            break;

        case RTDRV_L34_IPV6NEIGHBORTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ipv6NeighborTable_set(buf.l34_cfg.idx, &buf.l34_cfg.ipv6NeighborEntry);
            break;
        case RTDRV_L34_HSABMODE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hsabMode_set(buf.l34_cfg.hsabMode);
            break;
        case RTDRV_L34_PORTWANMAP_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_portWanMap_set(buf.l34_cfg.portWanMapType, buf.l34_cfg.portWanMapEntry);
            break;
     case RTDRV_L34_GLOBALSTATE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_globalState_set(buf.l34_cfg.stateType, buf.l34_cfg.state);
            break;

        case RTDRV_L34_LOOKUPMODE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_lookupMode_set(buf.l34_cfg.lookupMode);
            break;

        case RTDRV_L34_LOOKUPPORTMAP_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_lookupPortMap_set(buf.l34_cfg.portType, buf.l34_cfg.portId, buf.l34_cfg.wanIdx);
            break;

        case RTDRV_L34_WANROUTMODE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_wanRoutMode_set(buf.l34_cfg.wanRouteMode);
            break;
     case RTDRV_L34_HSDSTATE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hsdState_set(buf.l34_cfg.hsdState);
            break;

        case RTDRV_L34_HWL4TRFWRKTBL_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hwL4TrfWrkTbl_set(buf.l34_cfg.l4TrfTable);
            break;

        case RTDRV_L34_HWARPTRFWRKTBL_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hwArpTrfWrkTbl_set(buf.l34_cfg.arpTrfTable);
            break;
        case RTDRV_L34_IPV6MCROUTINGTRANSIDX_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ip6mcRoutingTransIdx_set(buf.l34_cfg.idx, buf.l34_cfg.portId, buf.l34_cfg.index);
            break;
        case RTDRV_L34_FLOWROUTE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_flowRouteTable_set(&buf.l34_cfg.flowRouteEntry);
            break;
        case RTDRV_L34_DSLITEINFTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_dsliteInfTable_set(&buf.l34_cfg.dsliteInfEntry);
            break;
        case RTDRV_L34_DSLITEMCTABLE_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_dsliteMcTable_set(&buf.l34_cfg.dsliteMcEntry);
            break;
        case RTDRV_L34_DSLITECONTROL_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_dsliteControl_set(buf.l34_cfg.dsliteCtrlType, buf.l34_cfg.dsliteCtrlAct);
            break;
        case RTDRV_L34_MIB_RESET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_mib_reset(buf.l34_cfg.idx);
            break;
        case RTDRV_L34_LUTMISS_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_lutLookupMiss_set(buf.l34_cfg.lutMissAct);
            break;
#if 0 /*not support L34 lite*/
      /*L34 lite*/
      case RTDRV_L34_LITE_INIT:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_lite_init();
            break;

        case RTDRV_L34_NETIF_CREATE:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netif_create(buf.l34lite_cfg.netifId, buf.l34lite_cfg.netifType, buf.l34lite_cfg.ifmac);
            break;

        case RTDRV_L34_NETIFPPPOE_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifPPPoE_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.pppState, buf.l34lite_cfg.sessionId, buf.l34lite_cfg.passThroughState);
            break;

        case RTDRV_L34_NETIFVLAN_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifVlan_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.vid, buf.l34lite_cfg.defaultPri);
            break;

        case RTDRV_L34_NETIFROUTINGSTATE_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifRoutingState_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.routingState);
            break;

        case RTDRV_L34_NETIFMTU_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifMtu_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.mtu);
            break;

        case RTDRV_L34_NETIFIPADDR_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifIpaddr_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.ipaddr, buf.l34lite_cfg.ipmask);
            break;

        case RTDRV_L34_NETIFNAT_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifNat_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.natType, buf.l34lite_cfg.internalp);
            break;

        case RTDRV_L34_NETIFSTATE_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifState_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.ifState);
            break;

        case RTDRV_L34_NETIF_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netif_del(buf.l34lite_cfg.netifId);
            break;

        case RTDRV_L34_ARP_ADD:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_arp_add(buf.l34lite_cfg.ipaddr, buf.l34lite_cfg.mac);
            break;

        case RTDRV_L34_ARP_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_arp_del(buf.l34lite_cfg.ipaddr);
            break;

        case RTDRV_L34_ROUTE_ADD:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_route_add(buf.l34lite_cfg.routeType, buf.l34lite_cfg.routeTable);
            break;

        case RTDRV_L34_ROUTE_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_route_del(buf.l34lite_cfg.ipaddr, buf.l34lite_cfg.ipmask);
            break;

        case RTDRV_L34_CONNECTTRACK_ADD:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_connectTrack_add(buf.l34lite_cfg.dir, buf.l34lite_cfg.tuple, buf.l34lite_cfg.natInfo);
            break;

        case RTDRV_L34_CONNECTTRACK_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_connectTrack_del(buf.l34lite_cfg.tuple);
            break;

        case RTDRV_L34_ROUTE6_ADD:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_route6_add(buf.l34lite_cfg.route6Type, buf.l34lite_cfg.route6Table);
            break;

        case RTDRV_L34_ROUTE6_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_route6_del(buf.l34lite_cfg.ip6addr, buf.l34lite_cfg.prefixLen);
            break;

        case RTDRV_L34_NEIGH6_ADD:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_neigh6_add(buf.l34lite_cfg.ip6addr, buf.l34lite_cfg.mac);
            break;

        case RTDRV_L34_NEIGH6_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_neigh6_del(buf.l34lite_cfg.ip6addr);
            break;

        case RTDRV_L34_NETIFIP6ADDR_ADD:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifIp6addr_add(buf.l34lite_cfg.netifId, buf.l34lite_cfg.ip6addr, buf.l34lite_cfg.prefixLen);
            break;

        case RTDRV_L34_NETIFIP6ADDR_DEL:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifIp6addr_del(buf.l34lite_cfg.netifId, buf.l34lite_cfg.ip6addr, buf.l34lite_cfg.prefixLen);
            break;

        case RTDRV_L34_NETIFGATEWAY_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netifGateway_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.gatewayMac);
            break;

        case RTDRV_L34_NETIF_SET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netif_set(buf.l34lite_cfg.netifId, &buf.l34lite_cfg.netifInfo);
            break;
        case RTDRV_L34_NETIFMAC_SET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_netifMac_set(buf.l34lite_cfg.netifId, buf.l34lite_cfg.hwAddr);
            break;
#endif /*if 0*/ 

        /* Trunk */
        case RTDRV_TRUNK_INIT:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_init();
            break;

        case RTDRV_TRUNK_DISTRIBUTIONALGORITHM_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithm_set(buf.trunk_cfg.trkGid, buf.trunk_cfg.algoBitmask);
            break;

        case RTDRV_TRUNK_PORT_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_port_set(buf.trunk_cfg.trkGid, &buf.trunk_cfg.trunkMemberPortmask);
            break;

        case RTDRV_TRUNK_HASHMAPPINGTABLE_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_hashMappingTable_set(buf.trunk_cfg.trk_gid, &buf.trunk_cfg.hash2Port_array);
            break;

        case RTDRV_TRUNK_MODE_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_mode_set(buf.trunk_cfg.mode);
            break;

        case RTDRV_TRUNK_TRAFFICSEPARATE_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparate_set(buf.trunk_cfg.trk_gid, buf.trunk_cfg.separateType);
            break;

        case RTDRV_TRUNK_TRAFFICPAUSE_SET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficPause_set(buf.trunk_cfg.trk_gid, buf.trunk_cfg.enable);
            break;

        /* Mirror */
        case RTDRV_MIRROR_INIT:
            copy_from_user(&buf.mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_init();
            break;

        case RTDRV_MIRROR_PORTBASED_SET:
            copy_from_user(&buf.mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_portBased_set(buf.mirror_cfg.mirroringPort, &buf.mirror_cfg.mirroredRxPortmask, &buf.mirror_cfg.mirroredTxPortmask);
            break;

        case RTDRV_MIRROR_PORTISO_SET:
            copy_from_user(&buf.mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_portIso_set(buf.mirror_cfg.enable);
            break;

        /* dot1X */
        case RTDRV_DOT1X_INIT:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_init();
            break;

        case RTDRV_DOT1X_UNAUTHPACKETOPER_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_unauthPacketOper_set(buf.dot1x_cfg.port, buf.dot1x_cfg.unauthAction);
            break;

        case RTDRV_DOT1X_PORTBASEDENABLE_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_portBasedEnable_set(buf.dot1x_cfg.port, buf.dot1x_cfg.enable);
            break;

        case RTDRV_DOT1X_PORTBASEDAUTHSTATUS_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_portBasedAuthStatus_set(buf.dot1x_cfg.port, buf.dot1x_cfg.port_auth);
            break;

        case RTDRV_DOT1X_PORTBASEDDIRECTION_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_portBasedDirection_set(buf.dot1x_cfg.port, buf.dot1x_cfg.port_direction);
            break;

        case RTDRV_DOT1X_MACBASEDENABLE_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_macBasedEnable_set(buf.dot1x_cfg.port, buf.dot1x_cfg.enable);
            break;

        case RTDRV_DOT1X_MACBASEDDIRECTION_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_macBasedDirection_set(buf.dot1x_cfg.mac_direction);
            break;

        case RTDRV_DOT1X_GUESTVLAN_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_guestVlan_set(buf.dot1x_cfg.guest_vlan);
            break;

        case RTDRV_DOT1X_GUESTVLANBEHAVIOR_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_guestVlanBehavior_set(buf.dot1x_cfg.behavior);
            break;

        case RTDRV_DOT1X_TRAPPRI_SET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_trapPri_set(buf.dot1x_cfg.priority);
            break;


        /* CPU */
        case RTDRV_CPU_INIT:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_init();
            break;

        case RTDRV_CPU_AWAREPORTMASK_SET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_awarePortMask_set(buf.cpu_cfg.port_mask);
            break;

        case RTDRV_CPU_TAGFORMAT_SET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_tagFormat_set(buf.cpu_cfg.mode);
            break;

        case RTDRV_CPU_TRAPINSERTTAG_SET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_trapInsertTag_set(buf.cpu_cfg.state);
            break;

        case RTDRV_CPU_TAGAWARE_SET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_tagAware_set(buf.cpu_cfg.state);
            break;

        case RTDRV_CPU_TRAPINSERTTAGBYPORT_SET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_trapInsertTagByPort_set(buf.cpu_cfg.port, buf.cpu_cfg.state);
            break;

        case RTDRV_CPU_TAGAWAREBYPORT_SET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_tagAwareByPort_set(buf.cpu_cfg.port, buf.cpu_cfg.state);
            break;

        /* Trap */
        case RTDRV_TRAP_INIT:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_init();
            break;

        case RTDRV_TRAP_REASONTRAPTOCPUPRIORITY_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_reasonTrapToCpuPriority_set(buf.trap_cfg.type, buf.trap_cfg.priority);
            break;

        case RTDRV_TRAP_IGMPCTRLPKT2CPUENABLE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_igmpCtrlPkt2CpuEnable_set(buf.trap_cfg.enable);
            break;

        case RTDRV_TRAP_MLDCTRLPKT2CPUENABLE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_mldCtrlPkt2CpuEnable_set(buf.trap_cfg.enable);
            break;

        case RTDRV_TRAP_PORTIGMPMLDCTRLPKTACTION_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_portIgmpMldCtrlPktAction_set(buf.trap_cfg.port, buf.trap_cfg.igmpMldType, buf.trap_cfg.action);
            break;

        case RTDRV_TRAP_IPMCASTPKT2CPUENABLE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_ipMcastPkt2CpuEnable_set(buf.trap_cfg.enable);
            break;

        case RTDRV_TRAP_L2MCASTPKT2CPUENABLE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_l2McastPkt2CpuEnable_set(buf.trap_cfg.enable);
            break;

        case RTDRV_TRAP_RMAACTION_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaAction_set(&buf.trap_cfg.rmaFrame, buf.trap_cfg.rmaAction);
            break;

        case RTDRV_TRAP_RMAPRI_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaPri_set(buf.trap_cfg.priority);
            break;

        case RTDRV_TRAP_OAMPDUACTION_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_oamPduAction_set(buf.trap_cfg.action);
            break;

        case RTDRV_TRAP_PORTOAMPDUACTION_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_portOamPduAction_set(buf.trap_cfg.port, buf.trap_cfg.action);
            break;

        case RTDRV_TRAP_OAMPDUPRI_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_oamPduPri_set(buf.trap_cfg.priority);
            break;

        case RTDRV_TRAP_UNITRAPPRIORITYENABLE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_uniTrapPriorityEnable_set(buf.trap_cfg.enable);
            break;

        case RTDRV_TRAP_UNITRAPPRIORITYPRIORITY_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_uniTrapPriorityPriority_set(buf.trap_cfg.priority);
            break;

        case RTDRV_TRAP_CPUTRAPHASHMASK_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapHashMask_set(buf.trap_cfg.hashType, buf.trap_cfg.enable);
            break;

        case RTDRV_TRAP_CPUTRAPHASHPORT_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapHashPort_set(buf.trap_cfg.hashValue, buf.trap_cfg.port);
            break;

        case RTDRV_TRAP_CPUTRAPHASHSTATE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapHashState_set(buf.trap_cfg.state);
            break;

        case RTDRV_TRAP_CPUTRAPINICHASHSTATE_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapInicHashState_set(buf.trap_cfg.state);
            break;

        case RTDRV_TRAP_CPUTRAPINICHASHSELECT_SET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapInicHashSelect_set(buf.trap_cfg.select);
            break;


#endif /* !CONFIG_XDSL_RG_DIAGSHELL */

#ifndef CONFIG_XDSL_RG_DIAGSHELL
        /*OAM*/
        case RTDRV_OAM_INIT:
            copy_from_user(&buf.oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_init();
            break;

        case RTDRV_OAM_PARSERACTION_SET:
            copy_from_user(&buf.oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_parserAction_set(buf.oam_cfg.port, buf.oam_cfg.action);
            break;

        case RTDRV_OAM_MULTIPLEXERACTION_SET:
            copy_from_user(&buf.oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_multiplexerAction_set(buf.oam_cfg.port, buf.oam_cfg.multAction);
            break;

        /*EPON*/
#if defined(CONFIG_EPON_FEATURE)
        case RTDRV_EPON_INIT:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_init();
            break;

        case RTDRV_EPON_INTRMASK_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_intrMask_set(buf.epon_cfg.intrType, buf.epon_cfg.state);
            break;

        case RTDRV_EPON_INTR_DISABLEALL:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_intr_disableAll();
            break;

        case RTDRV_EPON_LLID_ENTRY_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_llid_entry_set(&buf.epon_cfg.llidEntry);
            break;

        case RTDRV_EPON_FORCELASERSTATE_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_forceLaserState_set(buf.epon_cfg.laserStatus);
            break;

        case RTDRV_EPON_LASERTIME_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_laserTime_set(buf.epon_cfg.lasetOnTime, buf.epon_cfg.lasetOffTime);
            break;

        case RTDRV_EPON_REGISTERREQ_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_registerReq_set(&buf.epon_cfg.regEntry);
            break;

        case RTDRV_EPON_CHURNINGKEY_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_churningKey_set(&buf.epon_cfg.entry);
            break;

        case RTDRV_EPON_USFECSTATE_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_usFecState_set(buf.epon_cfg.state);
            break;

        case RTDRV_EPON_DSFECSTATE_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_dsFecState_set(buf.epon_cfg.state);
            break;

        case RTDRV_EPON_MIBGLOBAL_RESET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_mibGlobal_reset();
            break;

        case RTDRV_EPON_MIBLLIDIDX_RESET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_mibLlidIdx_reset(buf.epon_cfg.llidIdx);
            break;
        case RTDRV_EPON_MPCPTIMEOUTVAL_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_mpcpTimeoutVal_set(buf.epon_cfg.timeVal);
            break;

        case RTDRV_EPON_OPTICALPOLARITY_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_opticalPolarity_set(buf.epon_cfg.polarity);
            break;

        case RTDRV_EPON_FECSTATE_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_fecState_set(buf.epon_cfg.state);
            break;

        case RTDRV_EPON_THRESHOLDREPORT_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_thresholdReport_set(buf.epon_cfg.llidIdx, &buf.epon_cfg.thresholdRpt);
            break;

        case RTDRV_EPON_FORCEPRBS_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_forcePRBS_set(buf.epon_cfg.prbsCfg);
            break;

        case RTDRV_EPON_MULTILLIDMODE_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_multiLlidMode_set(buf.epon_cfg.mode);
            break;
         case RTDRV_EPON_REPORTQUEUESET_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_reportQueueSet_set(buf.epon_cfg.queueSetNum);
            break;

        case RTDRV_EPON_QUEUETHRESHOLDREPORT_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_queueThresholdReport_set(buf.epon_cfg.queue, &buf.epon_cfg.thresholdRpt);
            break;

        case RTDRV_EPON_AUTOPOWERSAVINGSTATE_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_autoPowerSavingState_set(buf.epon_cfg.enable);
            break;

        case RTDRV_EPON_AUTOPOWERSAVINGOERECOVERTIME_SET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_autoPowerSavingOeRecoverTime_set(buf.epon_cfg.recoverTime);
            break;



#endif

        /*stat*/
        case RTDRV_STAT_INIT:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_init();
            break;

        case RTDRV_STAT_GLOBAL_RESET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_global_reset();
            break;

        case RTDRV_STAT_PORT_RESET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_port_reset(buf.stat_cfg.port);
            break;

        case RTDRV_STAT_LOG_RESET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_log_reset(buf.stat_cfg.index);
            break;

        case RTDRV_STAT_HOSTCNT_RESET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_hostCnt_reset(buf.stat_cfg.index);
            break;
        case RTDRV_STAT_HOSTSTATE_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_hostState_set(buf.stat_cfg.index, buf.stat_cfg.enState);
            break;


        case RTDRV_STAT_RSTCNTVALUE_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_rstCntValue_set(buf.stat_cfg.rstValue);
            break;

        case RTDRV_STAT_LOGCTRL_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_logCtrl_set(buf.stat_cfg.index, buf.stat_cfg.ctrl);
            break;

        case RTDRV_STAT_MIBCNTMODE_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibCntMode_set(buf.stat_cfg.cnt_mode);
            break;

        case RTDRV_STAT_MIBLATCHTIMER_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibLatchTimer_set(buf.stat_cfg.timer);
            break;

        case RTDRV_STAT_MIBSYNCMODE_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibSyncMode_set(buf.stat_cfg.sync_mode);
            break;

        case RTDRV_STAT_MIBCNTTAGLEN_SET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibCntTagLen_set(buf.stat_cfg.direction, buf.stat_cfg.state);
            break;

        /*SEC*/
        case RTDRV_SEC_INIT:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_init();
            break;

        case RTDRV_SEC_PORTATTACKPREVENTSTATE_SET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portAttackPreventState_set(buf.sec_cfg.port, buf.sec_cfg.enable);
            break;

        case RTDRV_SEC_ATTACKPREVENT_SET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackPrevent_set(buf.sec_cfg.attackType, buf.sec_cfg.action);
            break;

        case RTDRV_SEC_ATTACKFLOODTHRESH_SET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackFloodThresh_set(buf.sec_cfg.type, buf.sec_cfg.floodThresh);
            break;

        case RTDRV_SEC_ATTACKFLOODTHRESHUNIT_SET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackFloodThreshUnit_set(buf.sec_cfg.type, buf.sec_cfg.floodThreshUnit);
            break;

        /*time*/
        case RTDRV_TIME_PORTTRANSPARENTENABLE_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portTransparentEnable_set(buf.time_cfg.port, buf.time_cfg.enable);
            break;

        case RTDRV_TIME_INIT:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_init();
            break;

        case RTDRV_TIME_PORTPTPENABLE_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpEnable_set(buf.time_cfg.port, buf.time_cfg.enable);
            break;

        case RTDRV_TIME_REFTIME_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_refTime_set(buf.time_cfg.sign, buf.time_cfg.ponTod.timeStamp);
            break;

        case RTDRV_TIME_FREQUENCY_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_frequency_set(buf.time_cfg.freq);
            break;

        case RTDRV_TIME_PTPIGRMSGACTION_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ptpIgrMsgAction_set(buf.time_cfg.type, buf.time_cfg.igr_action);
            break;

        case RTDRV_TIME_PTPEGRMSGACTION_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ptpEgrMsgAction_set(buf.time_cfg.type, buf.time_cfg.egr_action);
            break;

        case RTDRV_TIME_MEANPATHDELAY_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_meanPathDelay_set(buf.time_cfg.delay);
            break;

        case RTDRV_TIME_RXTIME_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_rxTime_set(buf.time_cfg.ponTod.timeStamp);
            break;

        case RTDRV_TIME_PONTOD_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ponTodTime_set(buf.time_cfg.ponTod);
            break;

        case RTDRV_TIME_TODENABLE_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_todEnable_set(buf.time_cfg.enable);
            break;

        case RTDRV_TIME_PPSENABLE_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ppsEnable_set(buf.time_cfg.enable);
            break;

        case RTDRV_TIME_PPSMODE_SET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ppsMode_set(buf.time_cfg.mode);
            break;

        case RTDRV_PPSTOD_INIT:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_init();
            break;

        case RTDRV_PPSTOD_DELAY_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_delay_set(buf.ppstod_cfg.delay);
            break;

        case RTDRV_PPSTOD_PULSEWIDTH_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_pulseWidth_set(buf.ppstod_cfg.width);
            break;

        case RTDRV_PPSTOD_MODE_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_mode_set(buf.ppstod_cfg.mode);
            break;

        case RTDRV_PPSTOD_SARPGPSWEEK_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_sarpGpsWeek_set(buf.ppstod_cfg.week);
            break;

        case RTDRV_PPSTOD_SARPUTCSEC_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_sarpUtcSec_set(buf.ppstod_cfg.sec);
            break;

        case RTDRV_PPSTOD_FRAMELEN_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_frameLen_set(buf.ppstod_cfg.len);
            break;

        case RTDRV_PPSTOD_FRAMEDATA_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_frameData_set(buf.ppstod_cfg.offset, buf.ppstod_cfg.val);
            break;

        case RTDRV_PPSTOD_BAUDRATE_SET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_baudrate_set(buf.ppstod_cfg.rate);
            break;

        case RTDRV_PONLED_SPECTYPE_SET:
            copy_from_user(&buf.ponLed, user, sizeof(rtdrv_ponLed_t));
            ret = rtk_pon_led_SpecType_set(buf.ponLed.specType);
            break;

        case RTDRV_PONLED_STATUS_SET:
            copy_from_user(&buf.ponLed, user, sizeof(rtdrv_ponLed_t));
            ret = rtk_pon_led_status_set(buf.ponLed.ponMode, buf.ponLed.ponStatus);
            break;

        case RTDRV_PONLED_GROUP_SET:
            copy_from_user(&buf.ponLed, user, sizeof(rtdrv_ponLed_t));
            ret = rtk_pon_led_group_set(buf.ponLed.ponLedGroup, buf.ponLed.ledGroupIdx);
            break;

        case RTDRV_PBO_INIT:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_pbo_init(buf.pbo_cfg.initParam);
            break;

        case RTDRV_SWPBO_INIT:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_init(buf.pbo_cfg.swPboInitParam);
            break;

        case RTDRV_SWPBO_PORTSTATE_SET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portState_set(buf.pbo_cfg.port, buf.pbo_cfg.enable);
            break;

        case RTDRV_SWPBO_PORTAUTOCONF_SET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portAutoConf_set(buf.pbo_cfg.port, buf.pbo_cfg.enable);
            break;

        case RTDRV_SWPBO_PORTTHRESHOLD_SET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portThreshold_set(buf.pbo_cfg.port, buf.pbo_cfg.threshold);
            break;

        case RTDRV_SWPBO_QUEUETHRESHOLD_SET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_queueThreshold_set(buf.pbo_cfg.port, buf.pbo_cfg.qid, buf.pbo_cfg.threshold);
            break;

        case RTDRV_SWPBO_PORTUSEDPAGE_CLEAR:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portUsedPageCount_clear(buf.pbo_cfg.port);
            break;

        case RTDRV_SWPBO_QUEUEUSEDPAGE_CLEAR:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_queueUsedPageCount_clear(buf.pbo_cfg.port, buf.pbo_cfg.qid);
            break;

        case RTDRV_SWPBO_PORTDSC_CLEAR:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portDscCount_clear(buf.pbo_cfg.port);
            break;

        case RTDRV_MDIO_INIT:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_init();
            break;

        case RTDRV_MDIO_CFG_SET:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_cfg_set(buf.mdio_cfg.set, buf.mdio_cfg.port, buf.mdio_cfg.phyid, buf.mdio_cfg.fmt);
            break;

        case RTDRV_MDIO_C22_WRITE:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_c22_write(buf.mdio_cfg.c22_reg, buf.mdio_cfg.data);
            break;

        case RTDRV_MDIO_C45_WRITE:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_c45_write(buf.mdio_cfg.dev, buf.mdio_cfg.c45_reg, buf.mdio_cfg.data);
            break;
#if defined(CONFIG_COMMON_RT_API)
        case RTDRV_MDIO_WRITE:
        {
            rt_mdio_info_t mdio_info;

            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            memset(&mdio_info, 0, sizeof(rt_mdio_info_t));
            mdio_info.format = buf.mdio_cfg.fmt;
            mdio_info.set = buf.mdio_cfg.set;
            mdio_info.phyAddr = buf.mdio_cfg.phyid;
            mdio_info.dev = buf.mdio_cfg.dev;
            mdio_info.reg = buf.mdio_cfg.c45_reg;
            ret = rt_mdio_write(&mdio_info, buf.mdio_cfg.data);
            break;
        }
#endif

#if defined(CONFIG_COMMON_RT_API)
        case RTDRV_RT_SWITCH_INIT:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_init();
            break;

        case RTDRV_RT_SWITCH_MAXPKTLENBYPORT_SET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_maxPktLenByPort_set(buf.rt_switch_cfg.port, buf.rt_switch_cfg.len);
            break;

        case RTDRV_RT_SWITCH_MODE_SET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_mode_set(buf.rt_switch_cfg.mode);
            break;
#ifdef CONFIG_GPON_FEATURE
        /*gpon*/
        case RTDRV_RT_GPON_INIT:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_init();
            break;

        case RTDRV_RT_GPON_SERIALNUMBER_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_serialNumber_set(&buf.rt_gpon_cfg.sN);
            break;

        case RTDRV_RT_GPON_REGISTRATIONID_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_registrationId_set(&buf.rt_gpon_cfg.regId);
            break;

        case RTDRV_RT_GPON_TCONT_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_tcont_set(&buf.rt_gpon_cfg.tcontId, buf.rt_gpon_cfg.allocId);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_TCONT_DEL:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_tcont_del(buf.rt_gpon_cfg.tcontId);
            break;

        case RTDRV_RT_GPON_USFLOW_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_usFlow_set(buf.rt_gpon_cfg.usFlowId, &buf.rt_gpon_cfg.usFlow);
            break;

        case RTDRV_RT_GPON_USFLOW_DEL:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_usFlow_del(buf.rt_gpon_cfg.usFlowId);
            break;

        case RTDRV_RT_GPON_USFLOW_DELALL:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_usFlow_delAll();
            break;

        case RTDRV_RT_GPON_DSFLOW_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_dsFlow_set(buf.rt_gpon_cfg.dsFlowId, &buf.rt_gpon_cfg.dsFlow);
            break;

        case RTDRV_RT_GPON_DSFLOW_DEL:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_dsFlow_del(buf.rt_gpon_cfg.dsFlowId);
            break;

        case RTDRV_RT_GPON_DSFLOW_DELALL:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_dsFlow_delAll();
            break;

        case RTDRV_RT_GPON_LOOPGEM_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_loop_gemport_set(buf.rt_gpon_cfg.loopGemPortId, buf.rt_gpon_cfg.loopGemEn);
            break;

        case RTDRV_RT_GPON_PONQUEUE_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_ponQueue_set(buf.rt_gpon_cfg.tcontId, buf.rt_gpon_cfg.tcQueueId, &buf.rt_gpon_cfg.queuecfg);
            break;

        case RTDRV_RT_GPON_PONQUEUE_DEL:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_ponQueue_del(buf.rt_gpon_cfg.tcontId, buf.rt_gpon_cfg.tcQueueId);
            break;

        case RTDRV_RT_GPON_MSK_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_msk_set(&buf.rt_gpon_cfg.msk);
            break;

        case RTDRV_RT_GPON_MCKEY_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_mcKey_set(&buf.rt_gpon_cfg.mcKey);
            break;

        case RTDRV_RT_GPON_EGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_egrBandwidthCtrlRate_set(buf.rt_gpon_cfg.rate);
            break;

        case RTDRV_RT_GPON_ATTRIBUTE_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_attribute_set(buf.rt_gpon_cfg.attributeType, &buf.rt_gpon_cfg.attributeValue);
            break;

        case RTDRV_RT_GPON_OMCI_MIRROR_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_omci_mirror_set(buf.rt_gpon_cfg.mirrorEn, buf.rt_gpon_cfg.mirroringPort);
            break;

		case RTDRV_RT_GPON_EMERGENCY_STOP_SET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_forceEmergencyStop_set(buf.rt_gpon_cfg.emergencyStop);
            break;
#endif
        /* Statistic */
        case RTDRV_RT_STAT_INIT:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_init();
            break;

        case RTDRV_RT_STAT_PORT_RESET:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_port_reset(buf.rt_stat_cfg.port);
            break;

        case RTDRV_RT_STAT_VLANCNT_FILTER_SET:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_vlanCnt_filter_set(buf.rt_stat_cfg.index, buf.rt_stat_cfg.filter);
            break;

        case RTDRV_RT_STAT_VLANCNT_MIB_RESET:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_vlanCnt_mib_reset(buf.rt_stat_cfg.index);
            break;

        /* Rate */
        case RTDRV_RT_RATE_INIT:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_init();
            break;

        case RTDRV_RT_RATE_PORTIGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_portIgrBandwidthCtrlRate_set(buf.rt_rate_cfg.port, buf.rt_rate_cfg.rate);
            break;

        case RTDRV_RT_RATE_PORTEGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_portEgrBandwidthCtrlRate_set(buf.rt_rate_cfg.port, buf.rt_rate_cfg.rate);
            break;

        case RTDRV_RT_RATE_STORMCONTROLMETERIDX_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_stormControlMeterIdx_set(buf.rt_rate_cfg.port, buf.rt_rate_cfg.stormType, buf.rt_rate_cfg.index);
            break;

        case RTDRV_RT_RATE_STORMCONTROLPORTENABLE_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_stormControlPortEnable_set(buf.rt_rate_cfg.port, buf.rt_rate_cfg.stormType, buf.rt_rate_cfg.enable);
            break;

        case RTDRV_RT_RATE_SHAREMETERTYPE_DEL:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterType_del(buf.rt_rate_cfg.index);
            break;

        case RTDRV_RT_RATE_SHAREMETERRATE_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterRate_set(buf.rt_rate_cfg.index, buf.rt_rate_cfg.rate);
            break;

        case RTDRV_RT_RATE_SHAREMETERBUCKET_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterBucket_set(buf.rt_rate_cfg.index, buf.rt_rate_cfg.bucketSize);
            break;

        case RTDRV_RT_RATE_SHAREMETERIFGINCLUDE_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterIfgInclude_set(buf.rt_rate_cfg.index, buf.rt_rate_cfg.ifgInclude);
            break;


        case RTDRV_RT_RATE_SHAREMETERMODE_SET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterMode_set(buf.rt_rate_cfg.index, buf.rt_rate_cfg.mode);
            break;


        /* QoS */
        case RTDRV_RT_QOS_INIT:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_init();
            break;

        case RTDRV_RT_QOS_SCHEDULINGQUEUE_SET:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_schedulingQueue_set(buf.rt_qos_cfg.port, &buf.rt_qos_cfg.qweights);
            break;

        case RTDRV_RT_QOS_DCSP2PBIT_SET:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_dscp2pbit_set(&buf.rt_qos_cfg.dscp2Pbit);
            break;

        case RTDRV_RT_QOS_INGRESS_QOS_SET:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_ingress_qos_set(buf.rt_qos_cfg.index, &buf.rt_qos_cfg.igrQos);
            break;

        case RTDRV_RT_QOS_INGRESS_QOS_DEL:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_ingress_qos_del(buf.rt_qos_cfg.index);
            break;

        /* Interrupt */
        case RTDRV_RT_INTR_INIT:
            copy_from_user(&buf.rt_intr_cfg, user, sizeof(rtdrv_rt_intrCfg_t));
            ret = rt_intr_init();
            break;

        case RTDRV_RT_INTR_IMR_SET:
            copy_from_user(&buf.rt_intr_cfg, user, sizeof(rtdrv_rt_intrCfg_t));
            ret = rt_intr_imr_set(buf.rt_intr_cfg.intr, buf.rt_intr_cfg.enable);
            break;


        /* Security */
        case RTDRV_RT_SEC_INIT:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_init();
            break;

        case RTDRV_RT_SEC_PORTATTACKPREVENTSTATE_SET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_portAttackPreventState_set(buf.rt_sec_cfg.port, buf.rt_sec_cfg.enable);
            break;

        case RTDRV_RT_SEC_ATTACKPREVENT_SET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_attackPrevent_set(buf.rt_sec_cfg.attackType, buf.rt_sec_cfg.action);
            break;

        case RTDRV_RT_SEC_ATTACKFLOODTHRESH_SET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_attackFloodThresh_set(buf.rt_sec_cfg.type, buf.rt_sec_cfg.floodThresh);
            break;

        case RTDRV_RT_SEC_ATTACKFLOODTHRESHUNIT_SET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_attackFloodThreshUnit_set(buf.rt_sec_cfg.type, buf.rt_sec_cfg.floodThreshUnit);
            break;

        /* Trap */
        case RTDRV_RT_TRAP_INIT:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_init();
            break;

        case RTDRV_RT_TRAP_PORTIGMPMLDCTRLPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portIgmpMldCtrlPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.igmpMldType, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_OAMPDUACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_oamPduAction_set(buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_OMCIACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_omciAction_set(buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTDHCPPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portDhcpPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTPPPOEPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portPppoePktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTSTPPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portStpPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTHOSTPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portHostPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTETHERTYPEPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portEthertypePktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTARPPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portArpPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTLOOPDETECTPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portLoopDetectPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PORTDOT1XPKTACTION_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portDot1xPktAction_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.action);
            break;

        case RTDRV_RT_TRAP_PATTERN_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_pattern_set(buf.rt_trap_cfg.type, buf.rt_trap_cfg.pattern);
            break;

        case RTDRV_RT_TRAP_PRIORITY_SET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_priority_set(buf.rt_trap_cfg.port, buf.rt_trap_cfg.type, buf.rt_trap_cfg.pri);
            break;

        /* I2C */
        case RTDRV_RT_I2C_INIT:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_init(buf.rt_i2c_cfg.i2cPort);
            break;

        case RTDRV_RT_I2C_ENABLE_SET:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_enable_set(buf.rt_i2c_cfg.i2cPort, buf.rt_i2c_cfg.enable);
            break;

        case RTDRV_RT_I2C_WRITE:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_write(buf.rt_i2c_cfg.i2cPort, buf.rt_i2c_cfg.devID, buf.rt_i2c_cfg.regAddr, buf.rt_i2c_cfg.data);
            break;

        case RTDRV_RT_I2C_SEQ_WRITE:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            pWriteData = (uint32 *) kmalloc(sizeof(uint32) * (buf.rt_i2c_cfg.seqCnt >= 256 ? 256 : buf.rt_i2c_cfg.seqCnt), GFP_KERNEL);
            if(NULL == pWriteData)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(pWriteData, buf.rt_i2c_cfg.pSeqData, sizeof(uint32) * (buf.rt_i2c_cfg.seqCnt >= 256 ? 256 : buf.rt_i2c_cfg.seqCnt));
            ret = rt_i2c_seq_write(buf.rt_i2c_cfg.i2cPort, buf.rt_i2c_cfg.devID, buf.rt_i2c_cfg.regAddr, pWriteData, buf.rt_i2c_cfg.seqCnt);
            kfree(pWriteData);
            break;

        case RTDRV_RT_I2C_CLOCK_SET:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_clock_set(buf.rt_i2c_cfg.i2cPort, buf.rt_i2c_cfg.clock);
            break;

        /*Port */
        case RTDRV_RT_PORT_INIT:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_init();
            break;

        case RTDRV_RT_PORT_PHYAUTONEGOENABLE_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyAutoNegoEnable_set(buf.rt_port_cfg.port, buf.rt_port_cfg.enable);
            break;

        case RTDRV_RT_PORT_PHYAUTONEGOABILITY_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyAutoNegoAbility_set(buf.rt_port_cfg.port, &buf.rt_port_cfg.ability);
            break;

        case RTDRV_RT_PORT_PHYFORCEMODEABILITY_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyForceModeAbility_set(buf.rt_port_cfg.port, buf.rt_port_cfg.speed, buf.rt_port_cfg.duplex, buf.rt_port_cfg.flowControl);
            break;

        case RTDRV_RT_PORT_PHYREG_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyReg_set(buf.rt_port_cfg.port, buf.rt_port_cfg.page, buf.rt_port_cfg.reg, buf.rt_port_cfg.data);
            break;

        case RTDRV_RT_PORT_PHYPOWERDOWN_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyPowerDown_set(buf.rt_port_cfg.port, buf.rt_port_cfg.enable);
            break;

        case RTDRV_RT_PORT_ADMINENABLE_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_adminEnable_set(buf.rt_port_cfg.port, buf.rt_port_cfg.enable);
            break;

        case RTDRV_RT_PORT_ISOLATIONENTRY_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_isolationEntry_set(buf.rt_port_cfg.mode, buf.rt_port_cfg.port, &buf.rt_port_cfg.portmask, &buf.rt_port_cfg.extPortmask);
            break;

        case RTDRV_RT_PORT_UNITYPE_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_uniType_set(buf.rt_port_cfg.port, buf.rt_port_cfg.uniType);
            break;

        case RTDRV_RT_PORT_MACFORCEABILITY_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_macForceAbility_set(buf.rt_port_cfg.port, buf.rt_port_cfg.macAbility);
            break;

        case RTDRV_RT_PORT_MACFORCEABILITYSTATE_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_macForceAbilityState_set(buf.rt_port_cfg.port, buf.rt_port_cfg.enable);
            break;

        case RTDRV_RT_PORT_EEEENABLE_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_eeeEnable_set(buf.rt_port_cfg.port, buf.rt_port_cfg.enable);
            break;

        case RTDRV_RT_PORT_MACLOOPBACKENABLE_SET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_macLoopbackEnable_set(buf.rt_port_cfg.port, buf.rt_port_cfg.enable, buf.rt_port_cfg.swapEnable);
            break;

        /* L2 */
        case RTDRV_RT_L2_INIT:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_init();
            break;

        case RTDRV_RT_L2_PORTLIMITLEARNINGCNT_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_portLimitLearningCnt_set(buf.rt_l2_cfg.port, buf.rt_l2_cfg.macCnt);
            break;

        case RTDRV_RT_L2_PORTLIMITLEARNINGCNTACTION_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_portLimitLearningCntAction_set(buf.rt_l2_cfg.port, buf.rt_l2_cfg.learnCntAct);
            break;

        case RTDRV_RT_L2_NEWMACOP_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_newMacOp_set(buf.rt_l2_cfg.port, buf.rt_l2_cfg.fwdAction);
            break;

        case RTDRV_RT_L2_ADDR_ADD:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_addr_add(&buf.rt_l2_cfg.l2UcastData);
            break;

        case RTDRV_RT_L2_ADDR_DEL:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_addr_del(&buf.rt_l2_cfg.l2UcastData);
            break;

        case RTDRV_RT_L2_MCASTADDR_ADD:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_mcastAddr_add(&buf.rt_l2_cfg.mcastAddr);
            break;

        case RTDRV_RT_L2_MCASTADDR_DEL:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_mcastAddr_del(&buf.rt_l2_cfg.mcastAddr);
            break;

        case RTDRV_RT_L2_ILLEGALPORTMOVEACTION_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_illegalPortMoveAction_set(buf.rt_l2_cfg.port, buf.rt_l2_cfg.fwdAction);
            break;

        case RTDRV_RT_L2_AGETIME_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_ageTime_set(buf.rt_l2_cfg.ageTime);
            break;

        case RTDRV_RT_L2_IVLSVL_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_ivlSvl_set(buf.rt_l2_cfg.ivlEn);
            break;

        case RTDRV_RT_L2_FWDGROUPID_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_fwdGroupId_set(buf.rt_l2_cfg.idx, buf.rt_l2_cfg.mask);
            break;

        case RTDRV_RT_L2_FWDGROUPACTION_SET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_fwdGroupAction_set(buf.rt_l2_cfg.idx, buf.rt_l2_cfg.portAct);
            break;

#ifdef CONFIG_EPON_FEATURE
        case RTDRV_RT_EPON_INIT:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_init();
            break;

        case RTDRV_RT_EPON_LLID_ENTRY_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_llid_entry_set(&buf.rt_epon_cfg.llidEntry);
            break;

        case RTDRV_RT_EPON_REGISTERREQ_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_registerReq_set(&buf.rt_epon_cfg.regEntry);
            break;

        case RTDRV_RT_EPON_CHURNINGKEY_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_churningKey_set(&buf.rt_epon_cfg.entry);
            break;

        case RTDRV_RT_EPON_USFECSTATE_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_usFecState_set(buf.rt_epon_cfg.state);
            break;

        case RTDRV_RT_EPON_DSFECSTATE_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_dsFecState_set(buf.rt_epon_cfg.state);
            break;

        case RTDRV_RT_EPON_PONQUEUE_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_ponQueue_set(buf.rt_epon_cfg.llid, buf.rt_epon_cfg.queueId, &buf.rt_epon_cfg.queuecfg);
            break;

        case RTDRV_RT_EPON_PONQUEUE_DEL:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_ponQueue_del(buf.rt_epon_cfg.llid, buf.rt_epon_cfg.queueId);
            break;

        case RTDRV_RT_EPON_EGRBANDWIDTHCTRLRATE_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_egrBandwidthCtrlRate_set(buf.rt_epon_cfg.rate);
            break;

        case RTDRV_RT_EPON_MPCP_QUEUE_THRESHOLD_SET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_mpcp_queue_threshold_set(&buf.rt_epon_cfg.thresholdRpt);
            break;

#endif
        case RTDRV_RT_CLS_INIT:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_init();
            break;

        case RTDRV_RT_CLS_RULE_ADD:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_rule_add(&buf.rt_cls_cfg.clsRule);
            break;

        case RTDRV_RT_CLS_RULE_DELETE:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_rule_delete(buf.rt_cls_cfg.index);
            break;

        case RTDRV_RT_CLS_EXTEND_RULE_ADD:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_extend_rule_add(&buf.rt_cls_cfg.extClsRule);
            break;

        case RTDRV_RT_CLS_EXTEND_RULE_DELETE:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_extend_rule_delete(buf.rt_cls_cfg.index);
            break;

        case RTDRV_RT_CLS_FWDPORT_SET:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_fwdPort_set(buf.rt_cls_cfg.port, buf.rt_cls_cfg.fwdPort);
            break;

        case RTDRV_RT_CLS_VLAN_PASER_SET:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_vlan_paser_set(buf.rt_cls_cfg.port, buf.rt_cls_cfg.type, buf.rt_cls_cfg.data);
            break;

#ifdef CONFIG_COMMON_RT_PONMISC
        case RTDRV_RT_PONMISC_INIT:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_init();
            break;

        case RTDRV_RT_PONMISC_MODESPEED_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_modeSpeed_set(buf.rt_ponmisc_cfg.ponMode, buf.rt_ponmisc_cfg.ponSpeed);
            break;

        case RTDRV_RT_PONMISC_SID_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_sid_set(buf.rt_ponmisc_cfg.sid, buf.rt_ponmisc_cfg.sidInfo);
            break;

        case RTDRV_RT_PONMISC_BURSTPOLARITYREVERSE_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_burstPolarityReverse_set(buf.rt_ponmisc_cfg.burstPolarity);
            break;

        case RTDRV_RT_PONMISC_RXLOSPOLARITYREVERSE_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_rxlosPolarityReverse_set(buf.rt_ponmisc_cfg.rxlosPolarity);
            break;

        case RTDRV_RT_PONMISC_FORCELASER_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_forceLaserState_set(buf.rt_ponmisc_cfg.laserStatus);
            break;

        case RTDRV_RT_PONMISC_FORCEPRBS_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_forcePRBS_set(buf.rt_ponmisc_cfg.prbs);
            break;

        case RTDRV_RT_PONMISC_FORCEPRBS_RX_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_forcePRBS_rx_set(buf.rt_ponmisc_cfg.prbs_rx);
            break;
		case RTDRV_RT_PONMISC_TRANSCEIVER_TX_PWR_SCALE_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_transceiver_tx_power_scale_set(buf.rt_ponmisc_cfg.trxData.tx_pwr_scale);
            break;

        case RTDRV_RT_PONMISC_SELFROGUE_CFG_SET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_selfRogue_cfg_set(buf.rt_ponmisc_cfg.cfg);
            break;

        case RTDRV_RT_PONMISC_SELFROGUE_CNT_CLEAR:
            ret = rt_ponmisc_selfRogue_count_clear();
            break;

        case RTDRV_RT_PONMISC_DYINGGASP_FORCE_TRIGGER:
            ret = rt_ponmisc_dying_gasp_force_trigger();
            break;
#endif

        case RTDRV_RT_MIRROR_INIT:
            copy_from_user(&buf.rt_mirror_cfg, user, sizeof(rtdrv_rt_mirrorCfg_t));
            ret = rt_mirror_init();
            break;

        case RTDRV_RT_MIRROR_PORTBASED_SET:
            copy_from_user(&buf.rt_mirror_cfg, user, sizeof(rtdrv_rt_mirrorCfg_t));
            ret = rt_mirror_portBased_set(buf.rt_mirror_cfg.mirroringPort, &buf.rt_mirror_cfg.mirroredRxPortmask, &buf.rt_mirror_cfg.mirroredTxPortmask);
            break;

        case RTDRV_RT_TIME_INIT:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_init();
            break;

        case RTDRV_RT_TIME_OFFSETTIME_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_offsetTime_set(buf.rt_time_cfg.sign,buf.rt_time_cfg.timeStamp);
            break;

        case RTDRV_RT_TIME_FREQUENCY_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_frequency_set(buf.rt_time_cfg.freq);
            break;

        case RTDRV_RT_TIME_PTPPORTENABLE_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ptpPortEnable_set(buf.rt_time_cfg.port,buf.rt_time_cfg.enable);
            break;

        case RTDRV_RT_TIME_PTPIGRACTION_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ptpIgrAction_set(buf.rt_time_cfg.type,buf.rt_time_cfg.igrAction);
            break;

        case RTDRV_RT_TIME_PTPEGRACTION_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ptpEgrAction_set(buf.rt_time_cfg.egrAction);
            break;

        case RTDRV_RT_TIME_PONTODTIME_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ponTodTime_set(buf.rt_time_cfg.ponTod);
            break;

        case RTDRV_RT_TIME_PONTODTIMEENABLE_SET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ponTodTimeEnable_set(buf.rt_time_cfg.enable);
            break;
#endif

#if defined(CONFIG_EUROPA_FEATURE) || defined(CONFIG_RTL8290C_FEATURE) || defined(CONFIG_RTL8291_FEATURE)
        case RTDRV_LDD_I2C_INIT:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_i2c_init(buf.ldd_cfg.i2cPort);
            break;

        case RTDRV_LDD_GPIO_INIT:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_gpio_init();
            break;

        case RTDRV_LDD_RESET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_reset(buf.ldd_cfg.reset);
            break;

        case RTDRV_LDD_CALIBRATION_STATE_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_calibration_state_set(buf.ldd_cfg.state);
            break;

        case RTDRV_LDD_PARAMETER_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_parameter_set(buf.ldd_cfg.rset_code, buf.ldd_cfg.ldo_code, buf.ldd_cfg.flash_data);
            break;

        case RTDRV_LDD_LOOP_MODE_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_loopMode_set(buf.ldd_cfg.loop_mode);
            break;

        case RTDRV_LDD_LASER_LUT_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_laserLut_set(buf.ldd_cfg.loop_mode, buf.ldd_cfg.flash_data);
            break;

        case RTDRV_LDD_APD_LUT_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apdLut_set(buf.ldd_cfg.flash_data);
            break;

        case RTDRV_LDD_APC_ENABLE_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcEnableFlow_set(buf.ldd_cfg.apc_func, buf.ldd_cfg.loop_mode);
            break;

        case RTDRV_LDD_TX_ENABLE_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_txEnableFlow_set(buf.ldd_cfg.tx_func, buf.ldd_cfg.loop_mode, buf.ldd_cfg.flash_data);
            break;

        case RTDRV_LDD_RX_ENABLE_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rxEnableFlow_set(buf.ldd_cfg.rx_func);
            break;

        case RTDRV_LDD_TX_BIAS_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_bias_set(buf.ldd_cfg.tx_bias);
            break;

        case RTDRV_LDD_TX_MOD_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_mod_set(buf.ldd_cfg.tx_mod);
            break;

        case RTDRV_LDD_STEERING_MODE_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_steering_mode_set();
             break;

        case RTDRV_LDD_INTEGRATION_MODE_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_integration_mode_set();
             break;

        case RTDRV_LDD_STEERING_MODE_FIXUP:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_steering_mode_fixup();
             break;

        case RTDRV_LDD_INTEGRATION_MODE_FIXUP:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_integration_mode_fixup();
             break;

        case RTDRV_LDD_CONFIG_REFRESH:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_config_refresh();
             break;

        case RTDRV_LDD_TXCROSS_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_txCross_set(buf.ldd_cfg.state, buf.ldd_cfg.count, buf.ldd_cfg.temperature);
             break;

        case RTDRV_LDD_APC_IAVG_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_apcIavg_set(buf.ldd_cfg.count);
             break;

        case RTDRV_LDD_APC_ER_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_apcEr_set(buf.ldd_cfg.count);
             break;

        case RTDRV_LDD_APC_ERTRIM_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_apcErTrim_set(buf.ldd_cfg.count);
             break;

        case RTDRV_LDD_RXLOS_REFDAC_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_rxlosRefDac_set(buf.ldd_cfg.count);
             break;

        case RTDRV_LDD_RXLOS_HYSTSEL_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_rxlosHystSel_set(buf.ldd_cfg.count);
             break;

        case RTDRV_LDD_CHIP_INIT:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_chip_init();
             break;

        case RTDRV_LDD_API_TEST:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_api_test(buf.ldd_cfg.flash_data);
             break;

        case RTDRV_LDD_RESET2:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_reset2(buf.ldd_cfg.devId, buf.ldd_cfg.reset);
            break;

        case RTDRV_LDD_PARAMETER2_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_parameter2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
            break;

        case RTDRV_LDD_LOOP_MODE2_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_loopMode2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code);

        case RTDRV_LDD_LASER_LUT2_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_laserLut2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code, buf.ldd_cfg.half_vdd_code);
            break;

        case RTDRV_LDD_APD_LUT2_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apdLut2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);

        case RTDRV_LDD_TX_BIAS2_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_bias2_set(buf.ldd_cfg.devId, buf.ldd_cfg.tx_bias);
            break;

        case RTDRV_LDD_TX_MOD2_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_mod2_set(buf.ldd_cfg.devId, buf.ldd_cfg.tx_mod);
            break;

        case RTDRV_LDD_TXCROSS2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_txCross2_set(buf.ldd_cfg.devId, buf.ldd_cfg.state, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
             break;

        case RTDRV_LDD_APC_IAVG2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_apcIavg2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code);
             break;

        case RTDRV_LDD_APC_ER2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_apcEr2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code);
             break;

        case RTDRV_LDD_APC_ERTRIM2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_apcErTrim2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code);
             break;

        case RTDRV_LDD_RXLOS_REFDAC2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_rxlosRefDac2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code);
             break;

        case RTDRV_LDD_RXLOS_HYSTSEL2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_rxlosHystSel2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code);
             break;
             
        case RTDRV_LDD_EFUSE2_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_efuse2_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
             break;

        case RTDRV_LDD_REGBIT_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_regBit_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.half_vdd_code, buf.ldd_cfg.gnd_code);
             break;
             
        case RTDRV_LDD_REGBITS_SET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_regBits_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.half_vdd_code, buf.ldd_cfg.gnd_code);
             break;

        case RTDRV_LDD_TXES_CONFIG_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_txEsConfig_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
            break;

        case RTDRV_LDD_DAC_SET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
			//osal_printf("\n%s: %d  dac_offset = 0x%x dac_code = 0x%x \n",__FUNCTION__, __LINE__, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
            ret = rtk_ldd_dac_set(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
            break;

        case RTDRV_LDD_CONFIG2_REFRESH:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_config2_refresh(buf.ldd_cfg.devId);
            break;

        case RTDRV_LDD_CHIP_INIT2:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_chip_init2(buf.ldd_cfg.devId);

#endif /*CONFIG_EUROPA_FEATURE  || CONFIG_RTL8290C_FEATURE || CONFIG_RTL8291_FEATURE || ONFIG_RTL8291_FEATURE*/
#endif /* !CONFIG_XDSL_RG_DIAGSHELL */


        default:
            break;
    }

    return ret;
}




/* Function Name:
 *      do_rtdrv_get_ctl
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
int32 do_rtdrv_get_ctl(struct sock *sk, int cmd, void *user, int *len)
{
    int32               ret = RT_ERR_FAILED;
    uint32              *pReadData;
    rtdrv_union_t       buf;
    rtdrv_pktdbg_t      *pPktDbg;
    rtdrv_pktfifodbg_t  *pPktFifoDbg;
    rtk_i2c_eeprom_mirror_t eepMirrorCfg;

    //osal_printf("do_rtdrv_set_ctl cmd=%d\n", cmd);
    switch(cmd)
    {

    /** DEBUG **/
        case RTDRV_DEBUG_EN_LOG_GET:
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

#ifndef CONFIG_XDSL_RG_DIAGSHELL
        case RTDRV_DEBUG_LOGLV_GET:
            ret = rt_log_level_get(&buf.unit_cfg.data);
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGLVMASK_GET:
            ret = rt_log_mask_get(&buf.unit_cfg.data);
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGTYPE_GET:
            ret = rt_log_type_get(&buf.unit_cfg.data);
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGFORMAT_GET:
            ret = rt_log_format_get(&buf.unit_cfg.data);
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_MODMASK_GET:
            ret = rt_log_moduleMask_get(&buf.unit_cfg.data64);
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_unitCfg_t));
            break;

        case RTDRV_DEBUG_LOGCFG_GET:
            ret = rt_log_config_get((uint32 *)&buf.log_cfg);
            copy_to_user(user, &buf.log_cfg, sizeof(rtdrv_logCfg_t));
            break;

        /*QoS*/
        case RTDRV_QOS_PRISELGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priSelGroup_get(buf.qos_cfg.grpIdx, &buf.qos_cfg.weightOfPriSel);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORTPRI_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPri_get(buf.qos_cfg.port, &buf.qos_cfg.intPri);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCPPRIREMAPGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpPriRemapGroup_get(buf.qos_cfg.grpIdx, buf.qos_cfg.dscp, &buf.qos_cfg.intPri, &buf.qos_cfg.dp);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1PPRIREMAPGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pPriRemapGroup_get(buf.qos_cfg.grpIdx, buf.qos_cfg.dot1pPri, &buf.qos_cfg.intPri, &buf.qos_cfg.dp);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PRIMAP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_priMap_get(buf.qos_cfg.group, &buf.qos_cfg.pri2qid);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORTPRIMAP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriMap_get(buf.qos_cfg.port, &buf.qos_cfg.group);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1PREMARKENABLE_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarkEnable_get(buf.qos_cfg.port, &buf.qos_cfg.enable);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_1PREMARKGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_1pRemarkGroup_get(buf.qos_cfg.grpIdx, buf.qos_cfg.intPri, buf.qos_cfg.dp, &buf.qos_cfg.dot1pPri);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCPREMARKENABLE_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarkEnable_get(buf.qos_cfg.port, &buf.qos_cfg.enable);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCPREMARKGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscpRemarkGroup_get(buf.qos_cfg.grpIdx, buf.qos_cfg.intPri, buf.qos_cfg.dp, &buf.qos_cfg.dscp);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORTDSCPREMARKSRCSEL_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDscpRemarkSrcSel_get(buf.qos_cfg.port, &buf.qos_cfg.type);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_DSCP2DSCPREMARKGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_dscp2DscpRemarkGroup_get(buf.qos_cfg.grpIdx, buf.qos_cfg.dscp, &buf.qos_cfg.dscp);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_FWD2CPUPRIREMAP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_fwd2CpuPriRemap_get(buf.qos_cfg.intPri, &buf.qos_cfg.rempPri);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_SCHEDULINGQUEUE_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingQueue_get(buf.qos_cfg.port, &buf.qos_cfg.qweights);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_PORTPRISELGROUP_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portPriSelGroup_get(buf.qos_cfg.port, &buf.qos_cfg.priSelGrpIdx);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;

        case RTDRV_QOS_SCHEDULINGTYPE_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_schedulingType_get(&buf.qos_cfg.queueType);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;
        case RTDRV_QOS_PORTDOT1PREMARKSRCSEL_GET:
            copy_from_user(&buf.qos_cfg, user, sizeof(rtdrv_qosCfg_t));
            ret = rtk_qos_portDot1pRemarkSrcSel_get(buf.qos_cfg.port, &buf.qos_cfg.dot1pRmkSrc);
            copy_to_user(user, &buf.qos_cfg, sizeof(rtdrv_qosCfg_t));
            break;


        /*LED*/
        case RTDRV_LED_OPERATION_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_operation_get(&buf.led_cfg.opMode);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_SERIALMODE_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_serialMode_get(&buf.led_cfg.active);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_BLINKRATE_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_blinkRate_get(buf.led_cfg.group, &buf.led_cfg.blinkRate);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_CONFIG_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_config_get(buf.led_cfg.ledIdx, &buf.led_cfg.type, &buf.led_cfg.config);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_MODEFORCE_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_modeForce_get(buf.led_cfg.ledIdx, &buf.led_cfg.mode);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_PARALLELENABLE_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_parallelEnable_get(buf.led_cfg.ledIdx, &buf.led_cfg.state);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_PONALARM_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_ponAlarm_get(&buf.led_cfg.state);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        case RTDRV_LED_PONWARNING_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_ponWarning_get(&buf.led_cfg.state);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;
        case RTDRV_LED_PARALLELMODE_GET:
            copy_from_user(&buf.led_cfg, user, sizeof(rtdrv_ledCfg_t));
            ret = rtk_led_parallelMode_get(buf.led_cfg.ledIdx, &buf.led_cfg.active);
            copy_to_user(user, &buf.led_cfg, sizeof(rtdrv_ledCfg_t));
            break;

        /*switch*/
        case RTDRV_SWITCH_DEVICEINFO_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_deviceInfo_get(&buf.switch_cfg.devInfo);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PHYPORTID_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_phyPortId_get(buf.switch_cfg.portName, &buf.switch_cfg.portId);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_LOGICALPORT_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_logicalPort_get(buf.switch_cfg.portId, &buf.switch_cfg.portName);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PORTIDINMASK_CHECK:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_portIdInMask_check(&buf.switch_cfg.portMask, buf.switch_cfg.portName);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PORTMASK_CLEAR:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_portMask_Clear(&buf.switch_cfg.portMask);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_NEXTPORTINMASK_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_nextPortInMask_get(&buf.switch_cfg.portMask, &buf.switch_cfg.portId);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_MAXPKTLENLINKSPEED_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenLinkSpeed_get(buf.switch_cfg.speed, &buf.switch_cfg.len);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_MGMTMACADDR_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_mgmtMacAddr_get(&buf.switch_cfg.mac);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_VERSION_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_version_get(&buf.switch_cfg.chipId, &buf.switch_cfg.rev, &buf.switch_cfg.subtype);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_PATCH_INFO_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_patch_info_get(buf.switch_cfg.patchIdx, &buf.switch_cfg.patchData);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_CSEXTID_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_csExtId_get(&buf.switch_cfg.csExtId);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;
        case RTDRV_SWITCH_MAXPKTLENBYPORT_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_maxPktLenByPort_get(buf.switch_cfg.portId, &buf.switch_cfg.len);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;
        case RTDRV_SWITCH_CHANGEDUPLEX_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_changeDuplex_get(&buf.switch_cfg.state);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        case RTDRV_SWITCH_THERMAL_GET:
            copy_from_user(&buf.switch_cfg, user, sizeof(rtdrv_switchCfg_t));
            ret = rtk_switch_thermal_get(&buf.switch_cfg.thermalIntger, &buf.switch_cfg.thermalDecimal);
            copy_to_user(user, &buf.switch_cfg, sizeof(rtdrv_switchCfg_t));
            break;

        /*STP*/
        case RTDRV_STP_MSTPSTATE_GET:
            copy_from_user(&buf.stp_cfg, user, sizeof(rtdrv_stpCfg_t));
            ret = rtk_stp_mstpState_get(buf.stp_cfg.msti, buf.stp_cfg.port, &buf.stp_cfg.stpState);
            copy_to_user(user, &buf.stp_cfg, sizeof(rtdrv_stpCfg_t));
            break;

        /*PONMAC*/
        case RTDRV_PONMAC_QUEUE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_queue_get(&buf.ponmac_cfg.queue, &buf.ponmac_cfg.queueCfg);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_FLOW2QUEUE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_flow2Queue_get(buf.ponmac_cfg.flow, &buf.ponmac_cfg.queue);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_TRANSCEIVER_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_transceiver_get(buf.ponmac_cfg.type, &buf.ponmac_cfg.data);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_MODE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_mode_get(&buf.ponmac_cfg.mode);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_LINKSTATE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_linkState_get(buf.ponmac_cfg.mode, &buf.ponmac_cfg.sdState, &buf.ponmac_cfg.syncState);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;
        case RTDRV_PONMAC_BWTHRESHOLD_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_bwThreshold_get(&buf.ponmac_cfg.bwThreshold, &buf.ponmac_cfg.reqBwThreshold);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_FLOWVALID_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_sidValid_get(buf.ponmac_cfg.flow, &buf.ponmac_cfg.state);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_LOSSTATE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_losState_get(&buf.ponmac_cfg.state);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_SCHEDULINGTYPE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_schedulingType_get(&buf.ponmac_cfg.queueType);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_EGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrBandwidthCtrlRate_get(&buf.ponmac_cfg.rate);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_EGRSCHEDULEIDRATE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrScheduleIdRate_get(buf.ponmac_cfg.scheduleId, &buf.ponmac_cfg.rate);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_SYNCE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_synce_get(&buf.ponmac_cfg.enable);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_EGRBANDWIDTHCTRLINCLUDEIFG_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrBandwidthCtrlIncludeIfg_get(&buf.ponmac_cfg.enable);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_EGRSCHEDULEIDINCLUDEIFG_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_egrScheduleIdIncludeIfg_get(&buf.ponmac_cfg.enable);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_TXDISGPIOID_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_txDisGpioId_get(&buf.ponmac_cfg.gpioPinId);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_TXPOWERGPIOID_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_txPowerGpioId_get(&buf.ponmac_cfg.gpioPinId);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_AGGTCONTSTATE_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_aggTcontState_get(buf.ponmac_cfg.scheduleId, &buf.ponmac_cfg.state);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_TXDISABLEGPIO_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_txDisableGpio_get(&buf.ponmac_cfg.enable);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        case RTDRV_PONMAC_TXPOWERDISABLEGPIO_GET:
            copy_from_user(&buf.ponmac_cfg, user, sizeof(rtdrv_ponmacCfg_t));
            ret = rtk_ponmac_txPowerDisableGpio_get(&buf.ponmac_cfg.enable);
            copy_to_user(user, &buf.ponmac_cfg, sizeof(rtdrv_ponmacCfg_t));
            break;

        /*VLAN*/
        case RTDRV_VLAN_FID_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_fid_get(buf.vlan_cfg.vid, &buf.vlan_cfg.fid);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_FIDMODE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_fidMode_get(buf.vlan_cfg.vid, &buf.vlan_cfg.mode);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORT_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_port_get(buf.vlan_cfg.vid, &buf.vlan_cfg.memberPortmask, &buf.vlan_cfg.untagPortmask);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EXTPORT_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPort_get(buf.vlan_cfg.vid, &buf.vlan_cfg.ext_portmask);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_STG_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_stg_get(buf.vlan_cfg.vid, &buf.vlan_cfg.stg);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PRIORITY_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_priority_get(buf.vlan_cfg.vid, &buf.vlan_cfg.priority);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PRIORITYENABLE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_priorityEnable_get(buf.vlan_cfg.vid, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_POLICINGENABLE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_policingEnable_get(buf.vlan_cfg.vid, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_POLICINGMETERIDX_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_policingMeterIdx_get(buf.vlan_cfg.vid, &buf.vlan_cfg.index);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTACCEPTFRAMETYPE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portAcceptFrameType_get(buf.vlan_cfg.port, &buf.vlan_cfg.acceptFrameType);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_VLANFUNCTIONENABLE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_vlanFunctionEnable_get(&buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTIGRFILTERENABLE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portIgrFilterEnable_get(buf.vlan_cfg.port, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_LEAKY_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_leaky_get(buf.vlan_cfg.leakyType, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTLEAKY_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portLeaky_get(buf.vlan_cfg.port, buf.vlan_cfg.leakyType, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_KEEPTYPE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_keepType_get(buf.vlan_cfg.keepType, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTPVID_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPvid_get(buf.vlan_cfg.port, &buf.vlan_cfg.pvid);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EXTPORTPVID_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortPvid_get(buf.vlan_cfg.extPort, &buf.vlan_cfg.pvid);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PROTOGROUP_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_protoGroup_get(buf.vlan_cfg.protoGroupIdx, &buf.vlan_cfg.protoGroup);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTPROTOVLAN_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portProtoVlan_get(buf.vlan_cfg.port, buf.vlan_cfg.protoGroupIdx, &buf.vlan_cfg.vlanCfg);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TAGMODE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tagMode_get(buf.vlan_cfg.port, &buf.vlan_cfg.tagMode);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTFID_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portFid_get(buf.vlan_cfg.port, &buf.vlan_cfg.enable, &buf.vlan_cfg.fid);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTPRIORITY_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portPriority_get(buf.vlan_cfg.port, &buf.vlan_cfg.priority);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_PORTEGRTAGKEEPTYPE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_portEgrTagKeepType_get(buf.vlan_cfg.egr_port, &buf.vlan_cfg.igr_portmask, &buf.vlan_cfg.type);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TRANSPARENTENABLE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_transparentEnable_get(&buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_CFIKEEPENABLE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_cfiKeepEnable_get(&buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_RESERVEDVIDACTION_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_reservedVidAction_get(&buf.vlan_cfg.action_vid0, &buf.vlan_cfg.action_vid4095);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TAGMODEIP4MC_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tagModeIp4mc_get(buf.vlan_cfg.port, &buf.vlan_cfg.tagModeIpmc);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_TAGMODEIP6MC_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_tagModeIp6mc_get(buf.vlan_cfg.port, &buf.vlan_cfg.tagModeIpmc);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;
        case RTDRV_VLAN_LUTSVLANHASHSTATE_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_lutSvlanHashState_get(buf.vlan_cfg.vid, &buf.vlan_cfg.enable);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        case RTDRV_VLAN_EXTPORTPROTOVLAN_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortProtoVlan_get(buf.vlan_cfg.extPort, buf.vlan_cfg.protoGroupIdx, &buf.vlan_cfg.vlanCfg);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;
        case RTDRV_VLAN_EXTPORTMASKINDEX_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortmaskIndex_get(buf.vlan_cfg.vid, &buf.vlan_cfg.index);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;
        case RTDRV_VLAN_EXTPORTMASKCFG_GET:
            copy_from_user(&buf.vlan_cfg, user, sizeof(rtdrv_vlanCfg_t));
            ret = rtk_vlan_extPortmaskCfg_get(buf.vlan_cfg.index, &buf.vlan_cfg.ext_portmask);
            copy_to_user(user, &buf.vlan_cfg, sizeof(rtdrv_vlanCfg_t));
            break;

        /* Port */
        case RTDRV_PORT_LINK_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_link_get(buf.port_cfg.port, &buf.port_cfg.linkStatus);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SPEEDDUPLEX_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_speedDuplex_get(buf.port_cfg.port, &buf.port_cfg.speed, &buf.port_cfg.duplex);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_FLOWCTRL_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_flowctrl_get(buf.port_cfg.port, &buf.port_cfg.txStatus, &buf.port_cfg.rxStatus);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYAUTONEGOENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyAutoNegoEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYAUTONEGOABILITY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyAutoNegoAbility_get(buf.port_cfg.port, &buf.port_cfg.ability);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYFORCEMODEABILITY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyForceModeAbility_get(buf.port_cfg.port, &buf.port_cfg.speed, &buf.port_cfg.duplex, &buf.port_cfg.flowControl);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYREG_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyReg_get(buf.port_cfg.port, buf.port_cfg.page, buf.port_cfg.reg, &buf.port_cfg.data);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYMASTERSLAVE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyMasterSlave_get(buf.port_cfg.port, &buf.port_cfg.masterSlave);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYTESTMODE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyTestMode_get(buf.port_cfg.port, &buf.port_cfg.testMode);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_CPUPORTID_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_cpuPortId_get(&buf.port_cfg.port);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATION_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolation_get(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONEXT_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationExt_get(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONL34_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationL34_get(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONEXTL34_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationExtL34_get(buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONENTRY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationEntry_get(buf.port_cfg.mode, buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONENTRYEXT_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationEntryExt_get(buf.port_cfg.mode, buf.port_cfg.port, &buf.port_cfg.portmask, &buf.port_cfg.extPortmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONCTAGPKTCONFIG_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationCtagPktConfig_get(&buf.port_cfg.mode);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONL34PKTCONFIG_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationL34PktConfig_get(&buf.port_cfg.mode);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONIPMCLEAKY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationIpmcLeaky_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONPORTLEAKY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationPortLeaky_get(buf.port_cfg.port, buf.port_cfg.type, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ISOLATIONLEAKY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_isolationLeaky_get(buf.port_cfg.type, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_MACREMOTELOOPBACKENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macRemoteLoopbackEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_MACLOCALLOOPBACKENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macLocalLoopbackEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ADMINENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_adminEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SPECIALCONGEST_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_specialCongest_get(buf.port_cfg.port, &buf.port_cfg.second);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SPECIALCONGESTSTATUS_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_specialCongestStatus_get(buf.port_cfg.port, &buf.port_cfg.status);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_GREENENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_greenEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYCROSSOVERMODE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyCrossOverMode_get(buf.port_cfg.port, &buf.port_cfg.crossoverMode);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_PHYPOWERDOWN_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_phyPowerDown_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_ENHANCEDFID_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_enhancedFid_get(buf.port_cfg.port, &buf.port_cfg.efid);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_RTCTRESULT_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_rtctResult_get(buf.port_cfg.port, &buf.port_cfg.result);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_RTCT_START:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_rtct_start(&buf.port_cfg.portmask);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_MACFORCEABILITY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macForceAbility_get(buf.port_cfg.port,&buf.port_cfg.macAbility);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_MACFORCEABILITYSTATE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macForceAbilityState_get(buf.port_cfg.port,&buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_MACEXTMODE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macExtMode_get(buf.port_cfg.port, &buf.port_cfg.ext_mode);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_MACEXTRGMIIDELAY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_macExtRgmiiDelay_get(buf.port_cfg.port, &buf.port_cfg.txDelay, &buf.port_cfg.rxDelay);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_GIGALITEENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_gigaLiteEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;
        case RTDRV_PORT_EEEENABLE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_eeeEnable_get(buf.port_cfg.port, &buf.port_cfg.enable);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SERDESMODE_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesMode_get(buf.port_cfg.num, &buf.port_cfg.cfg);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SERDESMODESPEED_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesModeSpeed_get(buf.port_cfg.num, (rtk_port_sds_mode_t *)&buf.port_cfg.cfg, &buf.port_cfg.speed);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SERDESNWAY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesNWay_get(buf.port_cfg.num, &buf.port_cfg.cfg);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SERDESTXRXPOLARITY_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesTxRxPolarity_get(buf.port_cfg.num, &buf.port_cfg.txPolarity, &buf.port_cfg.rxPolarity);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        case RTDRV_PORT_SERDESSTATUS_GET:
            copy_from_user(&buf.port_cfg, user, sizeof(rtdrv_portCfg_t));
            ret = rtk_port_serdesStatus_get(buf.port_cfg.num, &buf.port_cfg.sdsStatus);
            copy_to_user(user, &buf.port_cfg, sizeof(rtdrv_portCfg_t));
            break;

        /*ACL*/
        case RTDRV_ACL_TEMPLATE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_template_get(&buf.acl_cfg.aclTemplate);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_FIELDSELECT_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_fieldSelect_get(&buf.acl_cfg.fieldEntry);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_IGRRULEENTRY_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrRuleEntry_get(&buf.acl_cfg.aclRule);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_IGRUNMATCHACTION_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrUnmatchAction_get(buf.acl_cfg.port, &buf.acl_cfg.action);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_IGRSTATE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrState_get(buf.acl_cfg.port, &buf.acl_cfg.state);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_IPRANGE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_ipRange_get(&buf.acl_cfg.ipRangeEntry);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_VIDRANGE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_vidRange_get(&buf.acl_cfg.vidRangeEntry);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_PORTRANGE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_portRange_get(&buf.acl_cfg.l4PortRangeEntry);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_PACKETLENGTHRANGE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_packetLengthRange_get(&buf.acl_cfg.pktLenRangeEntry);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_IGRRULEMODE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrRuleMode_get(&buf.acl_cfg.mode);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;

        case RTDRV_ACL_IGRPERMITSTATE_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_igrPermitState_get(buf.acl_cfg.port, &buf.acl_cfg.state);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;
        case RTDRV_ACL_DBGINFO_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_dbgInfo_get(&buf.acl_cfg.dbgCnt);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;
        case RTDRV_ACL_HITREASON_GET:
            copy_from_user(&buf.acl_cfg, user, sizeof(rtdrv_aclCfg_t));
            ret = rtk_acl_dbgHitReason_get(&buf.acl_cfg.hitReason);
            copy_to_user(user, &buf.acl_cfg, sizeof(rtdrv_aclCfg_t));
            break;


    /** ADDR **/
        case RTDRV_REG_ADDRESS_GET:
            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));

#if defined(LINUX_KERNEL_MDIO_IO)
            buf.addr_cfg.value = io_mii_memory_read(buf.addr_cfg.address);
#else
            buf.addr_cfg.value = MEM32_READ(buf.addr_cfg.address);
#endif

            copy_to_user(user, &buf.addr_cfg, sizeof(rtdrv_addrCfg_t));
            ret = RT_ERR_OK;
            break;

        case RTDRV_PHY_REG_GET:
            copy_from_user(&buf.phy_cfg, user, sizeof(rtdrv_phyCfg_t));
#if defined(LINUX_KERNEL_MDIO_IO)
            io_mii_phy_reg_read(buf.phy_cfg.phy_id,buf.phy_cfg.reg,&buf.phy_cfg.value);
#endif
            copy_to_user(user, &buf.phy_cfg, sizeof(rtdrv_phyCfg_t));
            ret = RT_ERR_OK;
            break;

        case RTDRV_SOC_ADDRESS_GET:
            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));
            buf.addr_cfg.value = MEM32_READ(buf.addr_cfg.address);
            copy_to_user(user, &buf.addr_cfg, sizeof(rtdrv_addrCfg_t));
            ret = RT_ERR_OK;
            break;

#if (CONFIG_GPON_VERSION < 2)
        case RTDRV_GPON_SN_GET:
            copy_from_user(&buf.sn_cfg, user, sizeof(rtk_gpon_serialNumber_t));
            ret = rtk_gpon_serialNumber_get(&buf.sn_cfg);
            copy_to_user(user, &buf.sn_cfg, sizeof(rtk_gpon_serialNumber_t));
            break;

        case RTDRV_GPON_PWD_GET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gpon_password_get(&buf.pwd_cfg);
            copy_to_user(user, &buf.pwd_cfg, sizeof(rtk_gpon_password_t));
            break;

        case RTDRV_GPON_PARA_GET:
            copy_from_user(&buf.pon_cfg, user, sizeof(rtdrv_ponPara_t));
            osal_memset(&buf.pon_cfg.para, 0x0, sizeof(buf.pon_cfg.para));
            ret = rtk_gpon_parameter_get(buf.pon_cfg.type, &buf.pon_cfg.para);
            copy_to_user(user, &buf.pon_cfg, sizeof(rtdrv_ponPara_t));
            break;

        case RTDRV_GPON_STATE_GET:
            copy_from_user(&buf.pon_state, user, sizeof(rtk_gpon_fsm_status_t));
            ret = rtk_gpon_ponStatus_get(&buf.pon_state);
            copy_to_user(user, &buf.pon_state, sizeof(rtk_gpon_fsm_status_t));
            break;

        case RTDRV_GPON_TCONT_GET:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gpon_tcont_get(&buf.tcont_cfg.ind, &buf.tcont_cfg.attr);
            copy_to_user(user, &buf.tcont_cfg, sizeof(rtdrv_tcont_t));
            break;

        case RTDRV_GPON_DS_FLOW_GET:
            copy_from_user(&buf.ds_flow_cfg, user, sizeof(rtdrv_dsFlow_t));
            osal_memset(&buf.ds_flow_cfg.attr, 0x0, sizeof(buf.ds_flow_cfg.attr));
            ret = rtk_gpon_dsFlow_get(buf.ds_flow_cfg.flowId, &buf.ds_flow_cfg.attr);
            copy_to_user(user, &buf.ds_flow_cfg, sizeof(rtdrv_dsFlow_t));
            break;

        case RTDRV_GPON_US_FLOW_GET:
            copy_from_user(&buf.us_flow_cfg, user, sizeof(rtdrv_usFlow_t));
            osal_memset(&buf.us_flow_cfg.attr, 0x0, sizeof(buf.us_flow_cfg.attr));
            ret = rtk_gpon_usFlow_get(buf.us_flow_cfg.flowId, &buf.us_flow_cfg.attr);
            copy_to_user(user, &buf.us_flow_cfg, sizeof(rtdrv_usFlow_t));
            break;

        case RTDRV_GPON_BC_PASS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_broadcastPass_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_NON_MC_PASS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_nonMcastPass_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_IP_PTN_GET:
            copy_from_user(&buf.ip_ptn, user, sizeof(rtdrv_ip_ptn_t));
            ret = rtk_gpon_multicastAddrCheck_get(&buf.ip_ptn.ipv4_pattern, &buf.ip_ptn.ipv6_pattern);
            copy_to_user(user, &buf.ip_ptn, sizeof(rtdrv_ip_ptn_t));
            break;

        case RTDRV_GPON_FLT_MODE_GET:
            copy_from_user(&buf.filter_mode, user, sizeof(rtk_gpon_macTable_exclude_mode_t));
            ret = rtk_gpon_macFilterMode_get(&buf.filter_mode);
            copy_to_user(user, &buf.filter_mode, sizeof(rtk_gpon_macTable_exclude_mode_t));
            break;

        case RTDRV_GPON_FS_MODE_GET:
            copy_from_user(&buf.fs_mode, user, sizeof(rtdrv_fs_mode_t));
            ret = rtk_gpon_mcForceMode_get(&buf.fs_mode.ipv4, &buf.fs_mode.ipv6);
            copy_to_user(user, &buf.fs_mode, sizeof(rtdrv_fs_mode_t));
            break;

        case RTDRV_GPON_MAC_ENTRY_GET:
            copy_from_user(&buf.mac_entry, user, sizeof(rtdrv_mac_entry_t));
            ret = rtk_gpon_macEntry_get(buf.mac_entry.index, &buf.mac_entry.entry);
            copy_to_user(user, &buf.mac_entry, sizeof(rtdrv_mac_entry_t));
            break;

        case RTDRV_GPON_RDI_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_rdi_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_PWR_LEVEL_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            {
                uint8 level;
                ret = rtk_gpon_powerLevel_get(&level);
                buf.state = level;
            }
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_ALARM_GET:
            copy_from_user(&buf.alarm_state, user, sizeof(rtdrv_ponAlarm_t));
            ret = rtk_gpon_alarmStatus_get(buf.alarm_state.type, &buf.alarm_state.status);
            copy_to_user(user, &buf.alarm_state, sizeof(rtdrv_ponAlarm_t));
            break;

        case RTDRV_GPON_TX_LASER_GET:
            copy_from_user(&buf.tx_laser, user, sizeof(rtk_gpon_laser_status_t));
            ret = rtk_gpon_txForceLaser_get(&buf.tx_laser);
            copy_to_user(user, &buf.tx_laser, sizeof(rtk_gpon_laser_status_t));
            break;

        case RTDRV_GPON_FS_IDLE_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForceIdle_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
#if 0
        case RTDRV_GPON_FS_PRBS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForcePRBS_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
#endif
        case RTDRV_GPON_DS_FEC_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_dsFecSts_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_VERSION_SHOW:
            rtk_gpon_version_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_DEV_SHOW:
            rtk_gpon_devInfo_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GTC_SHOW:
            rtk_gpon_gtc_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_TCONT_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gpon_tcont_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_DS_FLOW_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gpon_dsFlow_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_US_FLOW_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gpon_usFlow_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_MAC_TABLE_SHOW:
            rtk_gpon_macTable_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GLB_CNT_SHOW:
            copy_from_user(&buf.glb_pm.type, user, sizeof(rtk_gpon_global_performance_type_t));
            rtk_gpon_globalCounter_show(buf.glb_pm.type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_TCONT_CNT_SHOW:
            copy_from_user(&buf.tcont_cnt, user, sizeof(rtdrv_tcont_cnt_t));
            rtk_gpon_tcontCounter_show(buf.tcont_cnt.tcont, buf.tcont_cnt.cnt_type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_FLOW_CNT_SHOW:
            copy_from_user(&buf.flow_cnt, user, sizeof(rtdrv_flow_cnt_t));
            rtk_gpon_flowCounter_show(buf.flow_cnt.flow, buf.flow_cnt.cnt_type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_AUTO_TCONT_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoTcont_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_AUTO_BOH_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoBoh_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_EQD_OFFSET_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_eqdOffset_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
#else
#if defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_LUNA_G3_SERIES)
/*for gpon usage*/
   case RTDRV_GPON_SN_GET:
            copy_from_user(&buf.sn_cfg, user, sizeof(rtk_gpon_serialNumber_t));
            ret = rtk_gponapp_serialNumber_get(&buf.sn_cfg);
            copy_to_user(user, &buf.sn_cfg, sizeof(rtk_gpon_serialNumber_t));
            break;

        case RTDRV_GPON_PWD_GET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gponapp_password_get(&buf.pwd_cfg);
            copy_to_user(user, &buf.pwd_cfg, sizeof(rtk_gpon_password_t));
            break;

        case RTDRV_GPON_PARA_GET:
            copy_from_user(&buf.pon_cfg, user, sizeof(rtdrv_ponPara_t));
            osal_memset(&buf.pon_cfg.para, 0x0, sizeof(buf.pon_cfg.para));
            ret = rtk_gponapp_parameter_get(buf.pon_cfg.type, &buf.pon_cfg.para);
            copy_to_user(user, &buf.pon_cfg, sizeof(rtdrv_ponPara_t));
            break;

        case RTDRV_GPON_STATE_GET:
            copy_from_user(&buf.pon_state, user, sizeof(rtk_gpon_fsm_status_t));
            ret = rtk_gponapp_ponStatus_get(&buf.pon_state);
            copy_to_user(user, &buf.pon_state, sizeof(rtk_gpon_fsm_status_t));
            break;

        case RTDRV_GPON_TCONT_GET:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gponapp_tcont_get(&buf.tcont_cfg.ind, &buf.tcont_cfg.attr);
            copy_to_user(user, &buf.tcont_cfg, sizeof(rtdrv_tcont_t));
            break;

        case RTDRV_GPON_DS_FLOW_GET:
            copy_from_user(&buf.ds_flow_cfg, user, sizeof(rtdrv_dsFlow_t));
            osal_memset(&buf.ds_flow_cfg.attr, 0x0, sizeof(buf.ds_flow_cfg.attr));
            ret = rtk_gponapp_dsFlow_get(buf.ds_flow_cfg.flowId, &buf.ds_flow_cfg.attr);
            copy_to_user(user, &buf.ds_flow_cfg, sizeof(rtdrv_dsFlow_t));
            break;

        case RTDRV_GPON_US_FLOW_GET:
            copy_from_user(&buf.us_flow_cfg, user, sizeof(rtdrv_usFlow_t));
            osal_memset(&buf.us_flow_cfg.attr, 0x0, sizeof(buf.us_flow_cfg.attr));
            ret = rtk_gponapp_usFlow_get(buf.us_flow_cfg.flowId, &buf.us_flow_cfg.attr);
            copy_to_user(user, &buf.us_flow_cfg, sizeof(rtdrv_usFlow_t));
            break;

        case RTDRV_GPON_BC_PASS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_broadcastPass_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_NON_MC_PASS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_nonMcastPass_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_IP_PTN_GET:
            copy_from_user(&buf.ip_ptn, user, sizeof(rtdrv_ip_ptn_t));
            ret = rtk_gponapp_multicastAddrCheck_get(&buf.ip_ptn.ipv4_pattern, &buf.ip_ptn.ipv6_pattern);
            copy_to_user(user, &buf.ip_ptn, sizeof(rtdrv_ip_ptn_t));
            break;

        case RTDRV_GPON_FLT_MODE_GET:
            copy_from_user(&buf.filter_mode, user, sizeof(rtk_gpon_macTable_exclude_mode_t));
            ret = rtk_gponapp_macFilterMode_get(&buf.filter_mode);
            copy_to_user(user, &buf.filter_mode, sizeof(rtk_gpon_macTable_exclude_mode_t));
            break;

        case RTDRV_GPON_FS_MODE_GET:
            copy_from_user(&buf.fs_mode, user, sizeof(rtdrv_fs_mode_t));
            ret = rtk_gponapp_mcForceMode_get(&buf.fs_mode.ipv4, &buf.fs_mode.ipv6);
            copy_to_user(user, &buf.fs_mode, sizeof(rtdrv_fs_mode_t));
            break;

        case RTDRV_GPON_MAC_ENTRY_GET:
            copy_from_user(&buf.mac_entry, user, sizeof(rtdrv_mac_entry_t));
            ret = rtk_gponapp_macEntry_get(buf.mac_entry.index, &buf.mac_entry.entry);
            copy_to_user(user, &buf.mac_entry, sizeof(rtdrv_mac_entry_t));
            break;

        case RTDRV_GPON_RDI_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_rdi_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_PWR_LEVEL_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            {
                uint8 level;
                ret = rtk_gponapp_powerLevel_get(&level);
                buf.state = level;
            }
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_ALARM_GET:
            copy_from_user(&buf.alarm_state, user, sizeof(rtdrv_ponAlarm_t));
            ret = rtk_gponapp_alarmStatus_get(buf.alarm_state.type, &buf.alarm_state.status);
            copy_to_user(user, &buf.alarm_state, sizeof(rtdrv_ponAlarm_t));
            break;

        case RTDRV_GPON_TX_LASER_GET:
            copy_from_user(&buf.tx_laser, user, sizeof(rtk_gpon_laser_status_t));
            ret = rtk_gponapp_txForceLaser_get(&buf.tx_laser);
            copy_to_user(user, &buf.tx_laser, sizeof(rtk_gpon_laser_status_t));
            break;

        case RTDRV_GPON_FS_IDLE_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_txForceIdle_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_FS_PRBS_GET:
            copy_from_user(&buf.prbs_cfg, user, sizeof(rtk_gpon_prbs_t));
            ret = rtk_gponapp_txForcePRBS_get(&buf.prbs_cfg);
            copy_to_user(user, &buf.prbs_cfg, sizeof(rtk_gpon_prbs_t));
            break;

        case RTDRV_GPON_DS_FEC_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_dsFecSts_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_US_FEC_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_usFecSts_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_VERSION_SHOW:
            rtk_gponapp_version_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_DEV_SHOW:
            rtk_gponapp_devInfo_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GTC_SHOW:
            rtk_gponapp_gtc_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_TCONT_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gponapp_tcont_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_DS_FLOW_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gponapp_dsFlow_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_US_FLOW_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gponapp_usFlow_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_MAC_TABLE_SHOW:
            rtk_gponapp_macTable_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GLB_CNT_SHOW:
            copy_from_user(&buf.glb_pm.type, user, sizeof(rtk_gpon_global_performance_type_t));
            rtk_gponapp_globalCounter_show(buf.glb_pm.type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GLB_CNT_GET:
            copy_from_user(&buf.glb_pm, user, sizeof(rtdrv_glbPmPara_t));
            osal_memset(&buf.glb_pm.para, 0x0, sizeof(buf.glb_pm.para));
            ret = rtk_gponapp_globalCounter_get(buf.glb_pm.type, &buf.glb_pm.para);
            copy_to_user(user, &buf.glb_pm, sizeof(rtdrv_glbPmPara_t));
            break;

        case RTDRV_GPON_TCONT_CNT_SHOW:
            copy_from_user(&buf.tcont_pm, user, sizeof(rtdrv_tcontPmPara_t));
            rtk_gponapp_tcontCounter_show(buf.tcont_pm.tcontId, buf.tcont_pm.type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_TCONT_CNT_GET:
            copy_from_user(&buf.tcont_pm, user, sizeof(rtdrv_tcontPmPara_t));
            osal_memset(&buf.tcont_pm.para, 0x0, sizeof(buf.tcont_pm.para));
            ret = rtk_gponapp_tcontCounter_get(buf.tcont_pm.tcontId, buf.tcont_pm.type, &buf.tcont_pm.para);
            copy_to_user(user, &buf.tcont_pm, sizeof(rtdrv_tcontPmPara_t));
            break;

        case RTDRV_GPON_FLOW_CNT_SHOW:
            copy_from_user(&buf.flow_pm, user, sizeof(rtdrv_flowPmPara_t));
            rtk_gponapp_flowCounter_show(buf.flow_pm.flowId, buf.flow_pm.type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_FLOW_CNT_GET:
            copy_from_user(&buf.flow_pm, user, sizeof(rtdrv_flowPmPara_t));
            osal_memset(&buf.flow_pm.para, 0x0, sizeof(buf.flow_pm.para));
            ret = rtk_gponapp_flowCounter_get(buf.flow_pm.flowId, buf.flow_pm.type, &buf.flow_pm.para);
            copy_to_user(user, &buf.flow_pm, sizeof(rtdrv_flowPmPara_t));
            break;

        case RTDRV_GPON_AUTO_TCONT_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_autoTcont_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_AUTO_BOH_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_autoBoh_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_EQD_OFFSET_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_eqdOffset_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
        case RTDRV_GPON_EXTMSG_GET:
            copy_from_user(&buf.gponExt_cfg, user, sizeof(rtk_gpon_extMsg_t));
            ret = rtk_gponapp_extMsg_get(&buf.gponExt_cfg);
            copy_to_user(user, &buf.gponExt_cfg, sizeof(rtk_gpon_extMsg_t));
            break;
        case RTDRV_GPON_AES_KEY_SWITCH_GET:
            copy_from_user(&buf.state, user, sizeof(uint32));
            ret = rtk_gponapp_aesKeySwitch_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(uint32));
            break;
        case RTDRV_GPON_BER_INTERVAL_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_berInterval_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
        case RTDRV_GPON_DBRU_BLOCK_SIZE_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gponapp_dbruBlockSize_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
        case RTDRV_GPON_ROGUE_SD_CNT_GET:
            copy_from_user(&buf.state, user, sizeof(rtk_gpon_rogue_sd_cnt_t));
            ret = rtk_gponapp_rogueSdCnt_get(&buf.rogue_sd_cnt);
            copy_to_user(user, &buf.rogue_sd_cnt, sizeof(rtk_gpon_rogue_sd_cnt_t));
            break;
        case RTDRV_GPON_PPS_CNT_GET:
            ret = rtk_gponapp_ppsCnt_readClear(&buf.index);
            copy_to_user(user, &buf.index, sizeof(uint32));
            break;
        case RTDRV_GPON_FEC_GET:
            ret = rtk_gponapp_fec_get(&buf.fec);
            copy_to_user(user, &buf.fec, sizeof(rtk_gpon_fec_status_t));
            break;
        case RTDRV_GPON_EQD_GET:
            ret = rtk_gponapp_eqd_get(&buf.eqd);
            copy_to_user(user, &buf.eqd, sizeof(uint32));
            break;
#endif
#if defined(CONFIG_LUNA_G3_SERIES)
        case RTDRV_GPON_STATE_GET:
            copy_from_user(&buf.pon_state, user, sizeof(rtk_gpon_fsm_status_t));
            ret = rtk_gpon_onuState_get(&buf.pon_state);
            copy_to_user(user, &buf.pon_state, sizeof(rtk_gpon_fsm_status_t));
            break;
#endif
#endif
        case RTDRV_PKT_RXDUMP_GET:
            pPktDbg = (rtdrv_pktdbg_t *) kmalloc(sizeof(rtdrv_pktdbg_t), GFP_KERNEL);
            if(NULL == pPktDbg)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(pPktDbg, user, sizeof(rtdrv_pktdbg_t));
            ret = RT_ERR_OK;
            mac_debug_rxPkt_get(&pPktDbg->buf[0], pPktDbg->length,
                (void *)&pPktDbg->rx_length, &pPktDbg->rx_info,
                &pPktDbg->enable);
            copy_to_user(user, pPktDbg, sizeof(rtdrv_pktdbg_t));
            kfree(pPktDbg);
            break;

        case RTDRV_PKTFIFO_RXDUMP_GET:
            pPktFifoDbg = (rtdrv_pktfifodbg_t *) kmalloc(sizeof(rtdrv_pktfifodbg_t), GFP_KERNEL);
            if(NULL == pPktFifoDbg)
            {
                return RT_ERR_NULL_POINTER;
            }
            copy_from_user(pPktFifoDbg, user, sizeof(rtdrv_pktfifodbg_t));
            ret = mac_debug_rxPktFifo_get(pPktFifoDbg->fifo_idx, &pPktFifoDbg->buf[0], sizeof(pPktFifoDbg->buf));
            if(0 == ret)
            {
                pPktFifoDbg->valid = 1;
            }
            else
            {
                pPktFifoDbg->valid = 0;
            }
            ret = RT_ERR_OK;
            copy_to_user(user, pPktFifoDbg, sizeof(rtdrv_pktfifodbg_t));
            kfree(pPktFifoDbg);
            break;

        case RTDRV_PKT_CONTINUSCNT_GET:
            pPktDbg = (rtdrv_pktdbg_t *) kmalloc(sizeof(rtdrv_pktdbg_t), GFP_KERNEL);
            if(NULL == pPktDbg)
            {
                return RT_ERR_NULL_POINTER;
            }
            ret = RT_ERR_OK;
            copy_from_user(&pPktDbg->tx_count, user, sizeof(uint32));
            mac_debug_txPktContCnt_get(&pPktDbg->tx_count);
            copy_to_user(user, &pPktDbg->tx_count, sizeof(uint32));
            kfree(pPktDbg);
            break;

#if defined(CONFIG_SDK_RTL9607C)
        case RTDRV_PKT_RXGMAC_GET:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = RT_ERR_OK;
            mac_debug_rxGmac_get(&buf.index);
            copy_to_user(user, &buf.index, sizeof(uint32));
            break;
#endif
        /*Rate*/
        case RTDRV_RATE_PORTIGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthCtrlRate_get(buf.rate_cfg.port, &buf.rate_cfg.rate);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORTIGRBANDWIDTHCTRLINCLUDEIFG_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(buf.rate_cfg.port, &buf.rate_cfg.ifgInclude);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORTEGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBandwidthCtrlRate_get(buf.rate_cfg.port, &buf.rate_cfg.rate);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGRBANDWIDTHCTRLINCLUDEIFG_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrBandwidthCtrlIncludeIfg_get(&buf.rate_cfg.ifgInclude);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_PORTEGRBANDWIDTHCTRLINCLUDEIFG_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_portEgrBandwidthCtrlIncludeIfg_get(buf.rate_cfg.port, &buf.rate_cfg.ifgInclude);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGRQUEUEBWCTRLENABLE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlEnable_get(buf.rate_cfg.port, buf.rate_cfg.queue, &buf.rate_cfg.enable);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGRQUEUEBWCTRLMETERIDX_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlMeterIdx_get(buf.rate_cfg.port, buf.rate_cfg.queue, &buf.rate_cfg.meterIndex);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_EGRQUEUEBWCTRLRATE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_egrQueueBwCtrlRate_get(buf.rate_cfg.port, buf.rate_cfg.queue, &buf.rate_cfg.rate);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORMCONTROLMETERIDX_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlMeterIdx_get(buf.rate_cfg.port, buf.rate_cfg.stormType, &buf.rate_cfg.index);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORMCONTROLPORTENABLE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlPortEnable_get(buf.rate_cfg.port, buf.rate_cfg.stormType, &buf.rate_cfg.enable);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORMCONTROLENABLE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormControlEnable_get(&buf.rate_cfg.stormCtrl);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_STORMBYPASS_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_stormBypass_get(buf.rate_cfg.type, &buf.rate_cfg.enable);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_SHAREMETER_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeter_get(buf.rate_cfg.index, &buf.rate_cfg.rate, &buf.rate_cfg.ifgInclude);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_SHAREMETERBUCKET_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeterBucket_get(buf.rate_cfg.index, &buf.rate_cfg.bucketSize);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_SHAREMETEREXCEED_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeterExceed_get(buf.rate_cfg.index, &buf.rate_cfg.isExceed);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_SHAREMETERMODE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_shareMeterMode_get(buf.rate_cfg.index, &buf.rate_cfg.meterMode);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
        case RTDRV_RATE_HOSTIGRBWCTRLSTATE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostIgrBwCtrlState_get(buf.rate_cfg.index, &buf.rate_cfg.enable);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
        case RTDRV_RATE_HOSTEGRBWCTRLSTATE_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostEgrBwCtrlState_get(buf.rate_cfg.index, &buf.rate_cfg.enable);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
        case RTDRV_RATE_HOSTBWCTRLMETERIDX_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostBwCtrlMeterIdx_get(buf.rate_cfg.index, &buf.rate_cfg.meterIndex);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
        case RTDRV_RATE_HOSTMACADDR_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostMacAddr_get(buf.rate_cfg.index, &buf.rate_cfg.mac);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;
        case RTDRV_RATE_HOSTIGRBWCTRLMETERIDX_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostIgrBwCtrlMeterIdx_get(buf.rate_cfg.index, &buf.rate_cfg.meterIndex);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_RATE_HOSTEGRBWCTRLMETERIDX_GET:
            copy_from_user(&buf.rate_cfg, user, sizeof(rtdrv_rateCfg_t));
            ret = rtk_rate_hostEgrBwCtrlMeterIdx_get(buf.rate_cfg.index, &buf.rate_cfg.meterIndex);
            copy_to_user(user, &buf.rate_cfg, sizeof(rtdrv_rateCfg_t));
            break;

        case RTDRV_GPIO_STATE_GET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_state_get(buf.gpio_cfg.gpioId, &buf.gpio_cfg.enable);
            copy_to_user(user, &buf.gpio_cfg, sizeof(rtdrv_gpioCfg_t));
            break;
        case RTDRV_GPIO_MODE_GET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_mode_get(buf.gpio_cfg.gpioId, &buf.gpio_cfg.mode);
            copy_to_user(user, &buf.gpio_cfg, sizeof(rtdrv_gpioCfg_t));
            break;

        case RTDRV_GPIO_DATABIT_GET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_databit_get(buf.gpio_cfg.gpioId, &buf.gpio_cfg.data);
            copy_to_user(user, &buf.gpio_cfg, sizeof(rtdrv_gpioCfg_t));
            break;
        case RTDRV_GPIO_IMS_GET:
              copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
              ret = rtk_gpio_intrStatus_get(buf.gpio_cfg.gpioId,&buf.gpio_cfg.enable);
              copy_to_user(user, &buf.gpio_cfg, sizeof(rtdrv_gpioCfg_t));
              break;
      case RTDRV_GPIO_INTR_GET:
            copy_from_user(&buf.gpio_cfg, user, sizeof(rtdrv_gpioCfg_t));
            ret = rtk_gpio_intr_get(buf.gpio_cfg.gpioId,&buf.gpio_cfg.intrMode);
            copy_to_user(user, &buf.gpio_cfg, sizeof(rtdrv_gpioCfg_t));
            break;

        case RTDRV_I2C_ENABLE_GET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_enable_get(buf.i2c_cfg.port, &buf.i2c_cfg.enable);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_WIDTH_GET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_width_get(buf.i2c_cfg.port, &buf.i2c_cfg.width);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_READ:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_read(buf.i2c_cfg.port, buf.i2c_cfg.devID, buf.i2c_cfg.regAddr, &buf.i2c_cfg.data);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_SEQ_READ:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            pReadData = (uint32 *) kmalloc(sizeof(uint32) * buf.i2c_cfg.seqCnt, GFP_KERNEL);
            if(NULL == pReadData)
            {
                return RT_ERR_NULL_POINTER;
            }
            ret = rtk_i2c_seq_read(buf.i2c_cfg.port, buf.i2c_cfg.devID, buf.i2c_cfg.regAddr, pReadData, buf.i2c_cfg.seqCnt);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            copy_to_user(buf.i2c_cfg.pSeqData, pReadData, sizeof(uint32) * buf.i2c_cfg.seqCnt);
            kfree(pReadData);
            break;

        case RTDRV_I2C_CURR_READ:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_curr_read(buf.i2c_cfg.port, buf.i2c_cfg.devID, &buf.i2c_cfg.data);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_SEQCURR_READ:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            pReadData = (uint32 *) kmalloc(sizeof(uint32) * buf.i2c_cfg.seqCnt, GFP_KERNEL);
            if(NULL == pReadData)
            {
                return RT_ERR_NULL_POINTER;
            }
            ret = rtk_i2c_seqCurr_read(buf.i2c_cfg.port, buf.i2c_cfg.devID, pReadData, buf.i2c_cfg.seqCnt);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            copy_to_user(buf.i2c_cfg.pSeqData, pReadData, sizeof(uint32) * buf.i2c_cfg.seqCnt);
            kfree(pReadData);
            break;

        case RTDRV_I2C_CLOCK_GET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_clock_get(buf.i2c_cfg.port, &buf.i2c_cfg.clock);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_EEPMIRROR_GET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            rtk_i2c_eepMirror_get(&eepMirrorCfg);
            buf.i2c_cfg.devID = eepMirrorCfg.devId;
            buf.i2c_cfg.width = eepMirrorCfg.addrWidth;
            buf.i2c_cfg.addrExtMode = eepMirrorCfg.addrExtMode;
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_EEPMIRROR_READ:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_eepMirror_read(buf.i2c_cfg.regAddr, &buf.i2c_cfg.data);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;
        case RTDRV_I2C_DATAWIDTH_GET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_dataWidth_get(buf.i2c_cfg.port, &buf.i2c_cfg.width);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_I2C_ADDRWIDTH_GET:
            copy_from_user(&buf.i2c_cfg, user, sizeof(rtdrv_i2cCfg_t));
            ret = rtk_i2c_addrWidth_get(buf.i2c_cfg.port, &buf.i2c_cfg.width);
            copy_to_user(user, &buf.i2c_cfg, sizeof(rtdrv_i2cCfg_t));
            break;

        case RTDRV_INTR_POLARITY_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_polarity_get(&buf.intr_cfg.polar);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;
        case RTDRV_INTR_IMR_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_imr_get(buf.intr_cfg.intr, &buf.intr_cfg.enable);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;
        case RTDRV_INTR_IMS_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_ims_get(buf.intr_cfg.intr, &buf.intr_cfg.status);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_INTR_SPEEDCHANGESTATUS_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_speedChangeStatus_get(&buf.intr_cfg.portMask);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_INTR_LINKUPSTATUS_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_linkupStatus_get(&buf.intr_cfg.portMask);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_INTR_LINKDOWNSTATUS_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_linkdownStatus_get(&buf.intr_cfg.portMask);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_INTR_GPHYSTATUS_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_gphyStatus_get(&buf.intr_cfg.portMask);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_INTR_ISR_GET:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_isr_get(buf.intr_cfg.type, &buf.intr_cfg.enable);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_INTR_ISR_COUNTER_DUMP:
            copy_from_user(&buf.intr_cfg, user, sizeof(rtdrv_intrCfg_t));
            ret = rtk_intr_isr_counter_dump(buf.intr_cfg.type, &buf.intr_cfg.counter);
            copy_to_user(user, &buf.intr_cfg, sizeof(rtdrv_intrCfg_t));
            break;

        case RTDRV_SVLAN_PORTSVID_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_portSvid_get(buf.svlan_cfg.port, &buf.svlan_cfg.svid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_SERVICEPORT_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_servicePort_get(buf.svlan_cfg.port, &buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_MEMBERPORT_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_memberPort_get(buf.svlan_cfg.svid, &buf.svlan_cfg.svlanPortmask, &buf.svlan_cfg.svlanUntagPortmask);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_TPIDENTRY_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_tpidEntry_get(buf.svlan_cfg.svlanIndex, &buf.svlan_cfg.svlanTagId);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_PRIORITYREF_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_priorityRef_get(&buf.svlan_cfg.ref);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_MEMBERPORTENTRY_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_memberPortEntry_get(&buf.svlan_cfg.svlan_cfg);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_IPMC2S_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_ipmc2s_get(buf.svlan_cfg.ipmc, buf.svlan_cfg.ipmcMsk, &buf.svlan_cfg.svid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_L2MC2S_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_l2mc2s_get(buf.svlan_cfg.mac, buf.svlan_cfg.macMsk, &buf.svlan_cfg.svid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_SP2C_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2c_get(buf.svlan_cfg.svid, buf.svlan_cfg.dstPort, &buf.svlan_cfg.cvid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_SP2CPRIORITY_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2cPriority_get(buf.svlan_cfg.svid, buf.svlan_cfg.dstPort, &buf.svlan_cfg.priority);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_DMACVIDSELSTATE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_dmacVidSelState_get(buf.svlan_cfg.port, &buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_UNMATCHACTION_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_unmatchAction_get(&buf.svlan_cfg.action, &buf.svlan_cfg.svid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_UNTAGACTION_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_untagAction_get(&buf.svlan_cfg.action, &buf.svlan_cfg.svid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_C2S_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_c2s_get(buf.svlan_cfg.cvid, buf.svlan_cfg.port, &buf.svlan_cfg.svid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_TRAPPRI_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_trapPri_get(&buf.svlan_cfg.priority);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_DEIKEEPSTATE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_deiKeepState_get(&buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_LOOKUPTYPE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_lookupType_get(&buf.svlan_cfg.type);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_SP2CUNMATCHCTAGGING_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_sp2cUnmatchCtagging_get(&buf.svlan_cfg.state);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_PRIORITY_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_priority_get(buf.svlan_cfg.svid, &buf.svlan_cfg.priority);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_FID_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_fid_get(buf.svlan_cfg.svid, &buf.svlan_cfg.fid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_FIDENABLE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_fidEnable_get(buf.svlan_cfg.svid, &buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_ENHANCEDFID_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_enhancedFid_get(buf.svlan_cfg.svid, &buf.svlan_cfg.efid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_ENHANCEDFIDENABLE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_enhancedFidEnable_get(buf.svlan_cfg.svid, &buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_DMACVIDSELFORCEDSTATE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_dmacVidSelForcedState_get(&buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;
        case RTDRV_SVLAN_SVLANFUNCTIONENABLE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_svlanFunctionEnable_get(&buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_TPIDENABLE_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_tpidEnable_get(buf.svlan_cfg.svlanIndex, &buf.svlan_cfg.enable);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        case RTDRV_SVLAN_EXTPORTPVID_GET:
            copy_from_user(&buf.svlan_cfg, user, sizeof(rtdrv_svlanCfg_t));
            ret = rtk_svlan_extPortPvid_get(buf.svlan_cfg.extPort, &buf.svlan_cfg.pvid);
            copy_to_user(user, &buf.svlan_cfg, sizeof(rtdrv_svlanCfg_t));
            break;

        /* RLDP */
        case RTDRV_RLDP_CONFIG_GET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_config_get(&buf.rldp_cfg.config);
            copy_to_user(user, &buf.rldp_cfg, sizeof(rtdrv_rldpCfg_t));
            break;

        case RTDRV_RLDP_PORTCONFIG_GET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_portConfig_get(buf.rldp_cfg.port, &buf.rldp_cfg.portConfig);
            copy_to_user(user, &buf.rldp_cfg, sizeof(rtdrv_rldpCfg_t));
            break;

        case RTDRV_RLDP_STATUS_GET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_status_get(&buf.rldp_cfg.status);
            copy_to_user(user, &buf.rldp_cfg, sizeof(rtdrv_rldpCfg_t));
            break;

        case RTDRV_RLDP_PORTSTATUS_GET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rldp_portStatus_get(buf.rldp_cfg.port, &buf.rldp_cfg.portStatus);
            copy_to_user(user, &buf.rldp_cfg, sizeof(rtdrv_rldpCfg_t));
            break;

        case RTDRV_RLPP_TRAPTYPE_GET:
            copy_from_user(&buf.rldp_cfg, user, sizeof(rtdrv_rldpCfg_t));
            ret = rtk_rlpp_trapType_get(&buf.rldp_cfg.type);
            copy_to_user(user, &buf.rldp_cfg, sizeof(rtdrv_rldpCfg_t));
            break;


        /* Classify */
        case RTDRV_CLASSIFY_CFGENTRY_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfgEntry_get(&buf.classify_cfg.classifyCfg);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_UNMATCHACTION_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_unmatchAction_get(&buf.classify_cfg.action);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_UNMATCHACTION_DS_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_unmatchAction_ds_get(&buf.classify_cfg.dsaction);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_PORTRANGE_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_portRange_get(&buf.classify_cfg.rangeEntry);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_IPRANGE_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_ipRange_get(&buf.classify_cfg.ipRangeEntry);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_CFSEL_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfSel_get(buf.classify_cfg.port, &buf.classify_cfg.cfSel);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_CFPRI2DSCP_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_cfPri2Dscp_get(buf.classify_cfg.pri, &buf.classify_cfg.dscp);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_PERMIT_SEL_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_permit_sel_get(&buf.classify_cfg.permitSel);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_US1PREMARK_PRIOR_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_us1pRemarkPrior_get(&buf.classify_cfg.prior);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_TEMPLATECFGPATTERN0_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_templateCfgPattern0_get(&buf.classify_cfg.p0TemplateCfg);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_ENTRYNUMPATTERN1_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_entryNumPattern1_get(&buf.classify_cfg.entryNum);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        case RTDRV_CLASSIFY_DEFAULTWANIF_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_defaultWanIf_get(&buf.classify_cfg.defaultWanIf);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;
        case RTDRV_CLASSIFY_TRAPPRI_GET:
            copy_from_user(&buf.classify_cfg, user, sizeof(rtdrv_classifyCfg_t));
            ret = rtk_classify_trapPri_get(&buf.classify_cfg.trapPri);
            copy_to_user(user, &buf.classify_cfg, sizeof(rtdrv_classifyCfg_t));
            break;

        /* L2 */
        case RTDRV_L2_FLUSHLINKDOWNPORTADDRENABLE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_flushLinkDownPortAddrEnable_get(&buf.l2_cfg.enable);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_UCASTADDR_FLUSH:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ucastAddr_flush(&buf.l2_cfg.config);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LIMITLEARNINGOVERSTATUS_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningOverStatus_get(&buf.l2_cfg.status);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LEARNINGCNT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_learningCnt_get(&buf.l2_cfg.macCnt);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LIMITLEARNINGCNT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningCnt_get(&buf.l2_cfg.macCnt);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LIMITLEARNINGCNTACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningCntAction_get(&buf.l2_cfg.learningAction);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LIMITLEARNINGENTRYACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningEntryAction_get(&buf.l2_cfg.learningEntryAction);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LIMITLEARNINGPORTMASK_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_limitLearningPortMask_get(&buf.l2_cfg.portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTLIMITLEARNINGOVERSTATUS_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLimitLearningOverStatus_get(buf.l2_cfg.port, &buf.l2_cfg.status);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTLEARNINGCNT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLearningCnt_get(buf.l2_cfg.port, &buf.l2_cfg.macCnt);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTLIMITLEARNINGCNT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLimitLearningCnt_get(buf.l2_cfg.port, &buf.l2_cfg.macCnt);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTLIMITLEARNINGCNTACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLimitLearningCntAction_get(buf.l2_cfg.port, &buf.l2_cfg.learningAction);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_AGING_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_aging_get(&buf.l2_cfg.agingTime);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTAGINGENABLE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portAgingEnable_get(buf.l2_cfg.port, &buf.l2_cfg.enable);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LOOKUPMISSACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissAction_get(buf.l2_cfg.type, &buf.l2_cfg.action);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTLOOKUPMISSACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portLookupMissAction_get(buf.l2_cfg.port, buf.l2_cfg.type, &buf.l2_cfg.action);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_LOOKUPMISSFLOODPORTMASK_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_lookupMissFloodPortMask_get(buf.l2_cfg.type, &buf.l2_cfg.flood_portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEWMACOP_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_newMacOp_get(buf.l2_cfg.port, &buf.l2_cfg.lrnMode, &buf.l2_cfg.fwdAction);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXTVALIDADDR_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidAddr_get(&buf.l2_cfg.scanIdx, &buf.l2_cfg.l2UcastData);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXTVALIDADDRONPORT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidAddrOnPort_get(buf.l2_cfg.port, &buf.l2_cfg.scanIdx, &buf.l2_cfg.l2UcastData);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXTVALIDMCASTADDR_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidMcastAddr_get(&buf.l2_cfg.scanIdx, &buf.l2_cfg.l2McastData);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXTVALIDIPMCASTADDR_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidIpMcastAddr_get(&buf.l2_cfg.scanIdx, &buf.l2_cfg.ipMcastData);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_NEXTVALIDENTRY_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_nextValidEntry_get(&buf.l2_cfg.scanIdx, &buf.l2_cfg.l2Entry);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ADDR_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_addr_get(&buf.l2_cfg.l2Addr);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_MCASTADDR_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_mcastAddr_get(&buf.l2_cfg.mcastAddr);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_EXTMEMBERCONFIG_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_extMemberConfig_get(buf.l2_cfg.index, &buf.l2_cfg.portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_ILLEGALPORTMOVEACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_illegalPortMoveAction_get(buf.l2_cfg.port, &buf.l2_cfg.fwdAction);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_VIDUNMATCHACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_vidUnmatchAction_get(buf.l2_cfg.port, &buf.l2_cfg.fwdAction);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCMODE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcMode_get(&buf.l2_cfg.mode);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCVLANMODE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcVlanMode_get(&buf.l2_cfg.mode);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPV6MCMODE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipv6mcMode_get(&buf.l2_cfg.mode);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCGROUPLOOKUPMISSHASH_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroupLookupMissHash_get(&buf.l2_cfg.ipmcHash);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCGROUP_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroup_get(buf.l2_cfg.ip, &buf.l2_cfg.portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCGROUPEXT_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroupExtPortmask_get(buf.l2_cfg.ip, &buf.l2_cfg.portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCGROUPEXTIDX_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcGroupExtMemberIdx_get(buf.l2_cfg.ip, &buf.l2_cfg.index);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_PORTIPMCACTION_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_portIpmcAction_get(buf.l2_cfg.port, &buf.l2_cfg.action);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCASTADDR_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipMcastAddr_get(&buf.l2_cfg.ipmcastAddr);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_SRCPORTEGRFILTERMASK_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_srcPortEgrFilterMask_get(&buf.l2_cfg.filter_portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_EXTPORTEGRFILTERMASK_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_extPortEgrFilterMask_get(&buf.l2_cfg.ext_portmask);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_CAM_STATE_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_camState_get(&buf.l2_cfg.enable);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

        case RTDRV_L2_IPMCSIPFILTER_GET:
            copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
            ret = rtk_l2_ipmcSipFilter_get(buf.l2_cfg.index, &buf.l2_cfg.ip);
            copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
            break;

      case RTDRV_L2_IP6MCRSVADDR_GET:
          copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
          ret = rtk_l2_ip6mcReservedAddrEnable_get(buf.l2_cfg.addrRsvIp6Mc, &buf.l2_cfg.enable);
          copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
          break;

      case RTDRV_L2_HASHVALUE_GET:
          copy_from_user(&buf.l2_cfg, user, sizeof(rtdrv_l2Cfg_t));
          ret = rtk_l2_hashValue_get(buf.l2_cfg.hashType, buf.l2_cfg.hashData, &buf.l2_cfg.hashValue);
          copy_to_user(user, &buf.l2_cfg, sizeof(rtdrv_l2Cfg_t));
          break;

      /*L34 */
        case RTDRV_L34_NETIFTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_netifTable_get(buf.l34_cfg.idx, &buf.l34_cfg.netifEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_ARPTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_arpTable_get(buf.l34_cfg.idx, &buf.l34_cfg.arpEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_PPPOETABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_pppoeTable_get(buf.l34_cfg.idx, &buf.l34_cfg.ppEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_ROUTINGTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_routingTable_get(buf.l34_cfg.idx, &buf.l34_cfg.routEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_NEXTHOPTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_nexthopTable_get(buf.l34_cfg.idx, &buf.l34_cfg.nextHopEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_EXTINTIPTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_extIntIPTable_get(buf.l34_cfg.idx, &buf.l34_cfg.extIpEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_NAPTINBOUNDTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_naptInboundTable_get(buf.l34_cfg.idx, &buf.l34_cfg.naptrEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_NAPTOUTBOUNDTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_naptOutboundTable_get(buf.l34_cfg.idx, &buf.l34_cfg.naptEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_IPMCTRANSTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ipmcTransTable_get(buf.l34_cfg.idx, &buf.l34_cfg.ipmcEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_BINDINGTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_bindingTable_get(buf.l34_cfg.idx, &buf.l34_cfg.bindEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_BINDINGACTION_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_bindingAction_get(buf.l34_cfg.bindType, &buf.l34_cfg.bindAction);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_WANTYPETABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_wanTypeTable_get(buf.l34_cfg.idx, &buf.l34_cfg.wanTypeEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_IPV6ROUTINGTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ipv6RoutingTable_get(buf.l34_cfg.idx, &buf.l34_cfg.ipv6RoutEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_IPV6NEIGHBORTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ipv6NeighborTable_get(buf.l34_cfg.idx, &buf.l34_cfg.ipv6NeighborEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
     case RTDRV_L34_HSABMODE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hsabMode_get(&buf.l34_cfg.hsabMode);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_HSADATA_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hsaData_get(&buf.l34_cfg.hsaData);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_HSBDATA_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hsbData_get(&buf.l34_cfg.hsbData);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
      case RTDRV_L34_PORTWANMAP_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_portWanMap_get(buf.l34_cfg.portWanMapType, &buf.l34_cfg.portWanMapEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
      case RTDRV_L34_GLOBALSTATE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_globalState_get(buf.l34_cfg.stateType, &buf.l34_cfg.state);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_LOOKUPMODE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_lookupMode_get(&buf.l34_cfg.lookupMode);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_LOOKUPPORTMAP_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_lookupPortMap_get(buf.l34_cfg.portType, buf.l34_cfg.portId, &buf.l34_cfg.wanIdx);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_WANROUTMODE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_wanRoutMode_get(&buf.l34_cfg.wanRouteMode);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
     case RTDRV_L34_ARPTRFINDICATOR_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_arpTrfIndicator_get(buf.l34_cfg.index, &buf.l34_cfg.arpIndicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_NAPTTRFINDICATOR_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_naptTrfIndicator_get(buf.l34_cfg.index, &buf.l34_cfg.naptIndicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_PPPTRFINDICATOR_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_pppTrfIndicator_get(buf.l34_cfg.index, &buf.l34_cfg.pppIndicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_NEIGHTRFINDICATOR_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_neighTrfIndicator_get(buf.l34_cfg.index, &buf.l34_cfg.neighIndicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
       case RTDRV_L34_HSDSTATE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hsdState_get(&buf.l34_cfg.hsdState);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_HWL4TRFWRKTBL_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hwL4TrfWrkTbl_get(&buf.l34_cfg.l4TrfTable);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_L4TRFTB_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_l4TrfTb_get(buf.l34_cfg.l4TrfTable, buf.l34_cfg.l4EntryIndex, &buf.l34_cfg.indicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_HWL4TRFWRKTBL_CLEAR:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hwL4TrfWrkTbl_Clear(buf.l34_cfg.l4TrfTable);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_HWARPTRFWRKTBL_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hwArpTrfWrkTbl_get(&buf.l34_cfg.arpTrfTable);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_ARPTRFTB_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_arpTrfTb_get(buf.l34_cfg.arpTrfTable, buf.l34_cfg.arpEntryIndex, &buf.l34_cfg.indicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_HWARPTRFWRKTBL_CLEAR:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_hwArpTrfWrkTbl_Clear(buf.l34_cfg.arpTrfTable);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_NAPTTRFINDICATOR_GET_ALL:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_naptTrfIndicator_get_all(&buf.l34_cfg.naptMaps);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;

        case RTDRV_L34_ARPTRFINDICATOR_GET_ALL:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_arpTrfIndicator_get_all(&buf.l34_cfg.arpMaps);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_PPPTRFINDICATOR_GET_ALL:
            ret = rtk_l34_pppTrfIndicator_get_all(&buf.l34_cfg.pppTrfAll);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_IPV6MCROUTINGTRANSIDX_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_ip6mcRoutingTransIdx_get(buf.l34_cfg.idx, buf.l34_cfg.portId, &buf.l34_cfg.index);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_FLOWROUTE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_flowRouteTable_get(&buf.l34_cfg.flowRouteEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_FLOWTRFINDICATOR_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_flowTrfIndicator_get(buf.l34_cfg.index, &buf.l34_cfg.flowIndicator);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_DSLITEINFTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_dsliteInfTable_get(&buf.l34_cfg.dsliteInfEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_DSLITEMCTABLE_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_dsliteMcTable_get(&buf.l34_cfg.dsliteMcEntry);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_DSLITECONTROL_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_dsliteControl_get(buf.l34_cfg.dsliteCtrlType, &buf.l34_cfg.dsliteCtrlAct);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_MIB_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_mib_get(&buf.l34_cfg.l34Cnts);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
        case RTDRV_L34_LUTMISS_GET:
            copy_from_user(&buf.l34_cfg, user, sizeof(rtdrv_l34Cfg_t));
            ret = rtk_l34_lutLookupMiss_get(&buf.l34_cfg.lutMissAct);
            copy_to_user(user, &buf.l34_cfg, sizeof(rtdrv_l34Cfg_t));
            break;
#if 0 /*not support L34 lite*/
     /*L34 lite*/
               case RTDRV_L34_NETIF_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_netif_get(buf.l34lite_cfg.netifId, &buf.l34lite_cfg.netifInfo);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;

        case RTDRV_L34_ARP_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_arp_get(buf.l34lite_cfg.ipaddr, &buf.l34lite_cfg.arpInfo);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;

        case RTDRV_L34_ROUTE_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_route_get(buf.l34lite_cfg.routeIndex, &buf.l34lite_cfg.routeInfo);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;

        case RTDRV_L34_CONNECTTRACK_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_connectTrack_get(buf.l34lite_cfg.tuple, &buf.l34lite_cfg.connectInfo);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;

        case RTDRV_L34_GLOBALCFG_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_globalCfg_get(&buf.l34lite_cfg.globalCfg);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;

        case RTDRV_L34_ROUTE6_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_route6_get(buf.l34lite_cfg.index, &buf.l34lite_cfg.route6Info);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;

        case RTDRV_L34_NEIGH6_GET:
            copy_from_user(&buf.l34lite_cfg, user, sizeof(rtdrv_l34liteCfg_t));
            ret = rtk_l34_neigh6_get(buf.l34lite_cfg.ip6addr, &buf.l34lite_cfg.neigh6Info);
            copy_to_user(user, &buf.l34lite_cfg, sizeof(rtdrv_l34liteCfg_t));
            break;
#endif
        /* Trunk */
        case RTDRV_TRUNK_DISTRIBUTIONALGORITHM_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_distributionAlgorithm_get(buf.trunk_cfg.trkGid, &buf.trunk_cfg.algoBitmask);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_PORT_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_port_get(buf.trunk_cfg.trkGid, &buf.trunk_cfg.trunkMemberPortmask);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_HASHMAPPINGTABLE_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_hashMappingTable_get(buf.trunk_cfg.trk_gid, &buf.trunk_cfg.hash2Port_array);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_MODE_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_mode_get(&buf.trunk_cfg.mode);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_TRAFFICSEPARATE_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficSeparate_get(buf.trunk_cfg.trk_gid, &buf.trunk_cfg.separateType);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_PORTQUEUEEMPTY_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_portQueueEmpty_get(&buf.trunk_cfg.empty_portmask);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        case RTDRV_TRUNK_TRAFFICPAUSE_GET:
            copy_from_user(&buf.trunk_cfg, user, sizeof(rtdrv_trunkCfg_t));
            ret = rtk_trunk_trafficPause_get(buf.trunk_cfg.trk_gid, &buf.trunk_cfg.enable);
            copy_to_user(user, &buf.trunk_cfg, sizeof(rtdrv_trunkCfg_t));
            break;

        /* Mirror */
        case RTDRV_MIRROR_PORTBASED_GET:
            copy_from_user(&buf.mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_portBased_get(&buf.mirror_cfg.mirroringPort, &buf.mirror_cfg.mirroredRxPortmask, &buf.mirror_cfg.mirroredTxPortmask);
            copy_to_user(user, &buf.mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        case RTDRV_MIRROR_PORTISO_GET:
            copy_from_user(&buf.mirror_cfg, user, sizeof(rtdrv_mirrorCfg_t));
            ret = rtk_mirror_portIso_get(&buf.mirror_cfg.enable);
            copy_to_user(user, &buf.mirror_cfg, sizeof(rtdrv_mirrorCfg_t));
            break;

        /* dot1x */
        case RTDRV_DOT1X_UNAUTHPACKETOPER_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_unauthPacketOper_get(buf.dot1x_cfg.port, &buf.dot1x_cfg.unauthAction);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_PORTBASEDENABLE_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_portBasedEnable_get(buf.dot1x_cfg.port, &buf.dot1x_cfg.enable);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_PORTBASEDAUTHSTATUS_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_portBasedAuthStatus_get(buf.dot1x_cfg.port, &buf.dot1x_cfg.port_auth);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_PORTBASEDDIRECTION_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_portBasedDirection_get(buf.dot1x_cfg.port, &buf.dot1x_cfg.port_direction);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_MACBASEDENABLE_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_macBasedEnable_get(buf.dot1x_cfg.port, &buf.dot1x_cfg.enable);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_MACBASEDDIRECTION_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_macBasedDirection_get(&buf.dot1x_cfg.mac_direction);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_GUESTVLAN_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_guestVlan_get(&buf.dot1x_cfg.guest_vlan);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_GUESTVLANBEHAVIOR_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_guestVlanBehavior_get(&buf.dot1x_cfg.behavior);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;

        case RTDRV_DOT1X_TRAPPRI_GET:
            copy_from_user(&buf.dot1x_cfg, user, sizeof(rtdrv_dot1xCfg_t));
            ret = rtk_dot1x_trapPri_get(&buf.dot1x_cfg.priority);
            copy_to_user(user, &buf.dot1x_cfg, sizeof(rtdrv_dot1xCfg_t));
            break;


        /* CPU */
        case RTDRV_CPU_AWAREPORTMASK_GET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_awarePortMask_get(&buf.cpu_cfg.port_mask);
            copy_to_user(user, &buf.cpu_cfg, sizeof(rtdrv_cpuCfg_t));
            break;

        case RTDRV_CPU_TAGFORMAT_GET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_tagFormat_get(&buf.cpu_cfg.mode);
            copy_to_user(user, &buf.cpu_cfg, sizeof(rtdrv_cpuCfg_t));
            break;

        case RTDRV_CPU_TRAPINSERTTAG_GET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_trapInsertTag_get(&buf.cpu_cfg.state);
            copy_to_user(user, &buf.cpu_cfg, sizeof(rtdrv_cpuCfg_t));
            break;

        case RTDRV_CPU_TAGAWARE_GET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_tagAware_get(&buf.cpu_cfg.state);
            copy_to_user(user, &buf.cpu_cfg, sizeof(rtdrv_cpuCfg_t));
            break;

        case RTDRV_CPU_TRAPINSERTTAGBYPORT_GET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_trapInsertTagByPort_get(buf.cpu_cfg.port, &buf.cpu_cfg.state);
            copy_to_user(user, &buf.cpu_cfg, sizeof(rtdrv_cpuCfg_t));
            break;

        case RTDRV_CPU_TAGAWAREBYPORT_GET:
            copy_from_user(&buf.cpu_cfg, user, sizeof(rtdrv_cpuCfg_t));
            ret = rtk_cpu_tagAwareByPort_get(buf.cpu_cfg.port, &buf.cpu_cfg.state);
            copy_to_user(user, &buf.cpu_cfg, sizeof(rtdrv_cpuCfg_t));
            break;

        /* Trap */
        case RTDRV_TRAP_REASONTRAPTOCPUPRIORITY_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_reasonTrapToCpuPriority_get(buf.trap_cfg.type, &buf.trap_cfg.priority);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_IGMPCTRLPKT2CPUENABLE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_igmpCtrlPkt2CpuEnable_get(&buf.trap_cfg.enable);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_MLDCTRLPKT2CPUENABLE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_mldCtrlPkt2CpuEnable_get(&buf.trap_cfg.enable);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_PORTIGMPMLDCTRLPKTACTION_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_portIgmpMldCtrlPktAction_get(buf.trap_cfg.port, buf.trap_cfg.igmpMldType, &buf.trap_cfg.action);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_IPMCASTPKT2CPUENABLE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_ipMcastPkt2CpuEnable_get(&buf.trap_cfg.enable);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_L2MCASTPKT2CPUENABLE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_l2McastPkt2CpuEnable_get(&buf.trap_cfg.enable);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_RMAACTION_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaAction_get(&buf.trap_cfg.rmaFrame, &buf.trap_cfg.rmaAction);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_RMAPRI_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_rmaPri_get(&buf.trap_cfg.priority);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_OAMPDUACTION_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_oamPduAction_get(&buf.trap_cfg.action);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_PORTOAMPDUACTION_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_portOamPduAction_get(buf.trap_cfg.port, &buf.trap_cfg.action);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_OAMPDUPRI_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_oamPduPri_get(&buf.trap_cfg.priority);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_UNITRAPPRIORITYENABLE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_uniTrapPriorityEnable_get(&buf.trap_cfg.enable);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_UNITRAPPRIORITYPRIORITY_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_uniTrapPriorityPriority_get(&buf.trap_cfg.priority);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_CPUTRAPHASHMASK_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapHashMask_get(buf.trap_cfg.hashType, &buf.trap_cfg.enable);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_CPUTRAPHASHPORT_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapHashPort_get(buf.trap_cfg.hashValue, &buf.trap_cfg.port);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_CPUTRAPHASHSTATE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapHashState_get(&buf.trap_cfg.state);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_CPUTRAPINICHASHSTATE_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapInicHashState_get(&buf.trap_cfg.state);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

        case RTDRV_TRAP_CPUTRAPINICHASHSELECT_GET:
            copy_from_user(&buf.trap_cfg, user, sizeof(rtdrv_trapCfg_t));
            ret = rtk_trap_cpuTrapInicHashSelect_get(&buf.trap_cfg.select);
            copy_to_user(user, &buf.trap_cfg, sizeof(rtdrv_trapCfg_t));
            break;

#endif /* !CONFIG_XDSL_RG_DIAGSHELL */


#if defined(CONFIG_EPON_FEATURE)
        /* EPON API*/
        case RTDRV_EPON_INTRMASK_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_intrMask_get(buf.epon_cfg.intrType, &buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_INTR_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_intr_get(buf.epon_cfg.intrType, &buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;


        case RTDRV_EPON_LLID_ENTRY_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_llid_entry_get(&buf.epon_cfg.llidEntry);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_FORCELASERSTATE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_forceLaserState_get(&buf.epon_cfg.laserStatus);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_LASERTIME_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_laserTime_get(&buf.epon_cfg.lasetOnTime, &buf.epon_cfg.lasetOffTime);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_SYNCTIME_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_syncTime_get(&buf.epon_cfg.syncTime);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_REGISTERREQ_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_registerReq_get(&buf.epon_cfg.regEntry);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_CHURNINGKEY_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_churningKey_get(&buf.epon_cfg.entry);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_USFECSTATE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_usFecState_get(&buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_DSFECSTATE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_dsFecState_get(&buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_MIBCOUNTER_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_mibCounter_get(&buf.epon_cfg.counter);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_LOSSTATE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_losState_get(&buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;
        case RTDRV_EPON_MPCPTIMEOUTVAL_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_mpcpTimeoutVal_get(&buf.epon_cfg.timeVal);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_OPTICALPOLARITY_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_opticalPolarity_get(&buf.epon_cfg.polarity);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_FECSTATE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_fecState_get(&buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_LLIDENTRYNUM_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_llidEntryNum_get(&buf.epon_cfg.num);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_THRESHOLDREPORT_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_thresholdReport_get(buf.epon_cfg.llidIdx, &buf.epon_cfg.thresholdRpt);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_DBGINFO_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_dbgInfo_get(&buf.epon_cfg.dbgCnt);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;
        case RTDRV_EPON_CHURNINGSTATUS_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_churningStatus_get(&buf.epon_cfg.state);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_FORCEPRBS_GET:
            ret = rtk_epon_forcePRBS_get(&buf.epon_cfg.prbsCfg);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_MULTILLIDMODE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_multiLlidMode_get(&buf.epon_cfg.mode);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_REPORTQUEUESET_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_reportQueueSet_get(&buf.epon_cfg.queueSetNum);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_QUEUETHRESHOLDREPORT_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_queueThresholdReport_get(buf.epon_cfg.queue, &buf.epon_cfg.thresholdRpt);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_AUTOPOWERSAVINGSTATE_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_autoPowerSavingState_get(&buf.epon_cfg.enable);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;

        case RTDRV_EPON_AUTOPOWERSAVINGOERECOVERTIME_GET:
            copy_from_user(&buf.epon_cfg, user, sizeof(rtdrv_eponCfg_t));
            ret = rtk_epon_autoPowerSavingOeRecoverTime_get(&buf.epon_cfg.recoverTime);
            copy_to_user(user, &buf.epon_cfg, sizeof(rtdrv_eponCfg_t));
            break;



#endif
#ifndef CONFIG_XDSL_RG_DIAGSHELL
        /*oam*/
        case RTDRV_OAM_PARSERACTION_GET:
            copy_from_user(&buf.oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_parserAction_get(buf.oam_cfg.port, &buf.oam_cfg.action);
            copy_to_user(user, &buf.oam_cfg, sizeof(rtdrv_oamCfg_t));
            break;

        case RTDRV_OAM_MULTIPLEXERACTION_GET:
            copy_from_user(&buf.oam_cfg, user, sizeof(rtdrv_oamCfg_t));
            ret = rtk_oam_multiplexerAction_get(buf.oam_cfg.port, &buf.oam_cfg.multAction);
            copy_to_user(user, &buf.oam_cfg, sizeof(rtdrv_oamCfg_t));
            break;

        /*stat*/
        case RTDRV_STAT_RSTCNTVALUE_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_rstCntValue_get(&buf.stat_cfg.rstValue);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_GLOBAL_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_global_get(buf.stat_cfg.cntrIdx, &buf.stat_cfg.cntr);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_GLOBAL_GETALL:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_global_getAll(&buf.stat_cfg.globalCntrs);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_PORT_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_port_get(buf.stat_cfg.port, buf.stat_cfg.portCntrIdx, &buf.stat_cfg.cntr);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_PORT_GETALL:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_port_getAll(buf.stat_cfg.port, &buf.stat_cfg.portCntrs);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;
        case RTDRV_STAT_LOG_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_log_get(buf.stat_cfg.index, &buf.stat_cfg.cntr);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_LOGCTRL_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_logCtrl_get(buf.stat_cfg.index, &buf.stat_cfg.ctrl);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_MIBCNTMODE_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibCntMode_get(&buf.stat_cfg.cnt_mode);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_MIBLATCHTIMER_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibLatchTimer_get(&buf.stat_cfg.timer);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_MIBSYNCMODE_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibSyncMode_get(&buf.stat_cfg.sync_mode);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_MIBCNTTAGLEN_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_mibCntTagLen_get(buf.stat_cfg.direction, &buf.stat_cfg.state);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;

        case RTDRV_STAT_PKTINFO_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_pktInfo_get(buf.stat_cfg.port, &buf.stat_cfg.code);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;
        case RTDRV_STAT_HOSTCNT_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_hostCnt_get(buf.stat_cfg.index, buf.stat_cfg.hostCntrsIdx, &buf.stat_cfg.cntr);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;
        case RTDRV_STAT_HOSTSTATE_GET:
            copy_from_user(&buf.stat_cfg, user, sizeof(rtdrv_statCfg_t));
            ret = rtk_stat_hostState_get(buf.stat_cfg.index, &buf.stat_cfg.enState);
            copy_to_user(user, &buf.stat_cfg, sizeof(rtdrv_statCfg_t));
            break;


        /*SEC*/
        case RTDRV_SEC_PORTATTACKPREVENTSTATE_GET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_portAttackPreventState_get(buf.sec_cfg.port, &buf.sec_cfg.enable);
            copy_to_user(user, &buf.sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_ATTACKPREVENT_GET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackPrevent_get(buf.sec_cfg.attackType, &buf.sec_cfg.action);
            copy_to_user(user, &buf.sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_ATTACKFLOODTHRESH_GET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackFloodThresh_get(buf.sec_cfg.type, &buf.sec_cfg.floodThresh);
            copy_to_user(user, &buf.sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        case RTDRV_SEC_ATTACKFLOODTHRESHUNIT_GET:
            copy_from_user(&buf.sec_cfg, user, sizeof(rtdrv_secCfg_t));
            ret = rtk_sec_attackFloodThreshUnit_get(buf.sec_cfg.type, &buf.sec_cfg.floodThreshUnit);
            copy_to_user(user, &buf.sec_cfg, sizeof(rtdrv_secCfg_t));
            break;

        /*time*/
        case RTDRV_TIME_PORTTRANSPARENTENABLE_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portTransparentEnable_get(buf.time_cfg.port, &buf.time_cfg.enable);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORTPTPENABLE_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpEnable_get(buf.time_cfg.port, &buf.time_cfg.enable);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_CURTIME_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_curTime_get(&buf.time_cfg.ponTod.timeStamp);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_CURTIME_LATCH:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_curTime_latch();
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_REFTIME_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_refTime_get(&buf.time_cfg.sign, &buf.time_cfg.ponTod.timeStamp);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_FREQUENCY_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_frequency_get(&buf.time_cfg.freq);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PTPIGRMSGACTION_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ptpIgrMsgAction_get(buf.time_cfg.type, &buf.time_cfg.igr_action);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PTPEGRMSGACTION_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ptpEgrMsgAction_get(buf.time_cfg.type, &buf.time_cfg.egr_action);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_MEANPATHDELAY_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_meanPathDelay_get(&buf.time_cfg.delay);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_RXTIME_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_rxTime_get(&buf.time_cfg.ponTod.timeStamp);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PONTOD_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ponTodTime_get(&buf.time_cfg.ponTod);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PORTPTPTXINDICATOR_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_portPtpTxIndicator_get(buf.time_cfg.port, &buf.time_cfg.enable);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_TODENABLE_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_todEnable_get(&buf.time_cfg.enable);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PPSENABLE_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ppsEnable_get(&buf.time_cfg.enable);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_PPSMODE_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_ppsMode_get(&buf.time_cfg.mode);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_TIME_FREETIME_GET:
            copy_from_user(&buf.time_cfg, user, sizeof(rtdrv_timeCfg_t));
            ret = rtk_time_freeTime_get(&buf.time_cfg.freeTime);
            copy_to_user(user, &buf.time_cfg, sizeof(rtdrv_timeCfg_t));
            break;

        case RTDRV_PPSTOD_DELAY_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_delay_get(&buf.ppstod_cfg.delay);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_PULSEWIDTH_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_pulseWidth_get(&buf.ppstod_cfg.width);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_MODE_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_mode_get(&buf.ppstod_cfg.mode);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_SARPGPSWEEK_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_sarpGpsWeek_get(&buf.ppstod_cfg.week);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_SARPUTCSEC_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_sarpUtcSec_get(&buf.ppstod_cfg.sec);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_FRAMELEN_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_frameLen_get(&buf.ppstod_cfg.len);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_FRAMEDATA_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_frameData_get(buf.ppstod_cfg.offset, &buf.ppstod_cfg.val);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PPSTOD_BAUDRATE_GET:
            copy_from_user(&buf.ppstod_cfg, user, sizeof(rtdrv_ppstodCfg_t));
            ret = rtk_ppstod_baudrate_get(&buf.ppstod_cfg.rate);
            copy_to_user(user, &buf.ppstod_cfg, sizeof(rtdrv_ppstodCfg_t));
            break;

        case RTDRV_PONLED_GROUP_GET:
            copy_from_user(&buf.ponLed, user, sizeof(rtdrv_ponLed_t));
            ret = rtk_pon_led_group_get(buf.ponLed.ponLedGroup, &buf.ponLed.ledGroupIdx);
            copy_to_user(user, &buf.ponLed, sizeof(rtdrv_ponLed_t));
            break;

        case RTDRV_SWPBO_PORTSTATE_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portState_get(buf.pbo_cfg.port, &buf.pbo_cfg.enable);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_SWPBO_PORTAUTOCONF_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portAutoConf_get(buf.pbo_cfg.port, &buf.pbo_cfg.enable);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_SWPBO_PORTTHRESHOLD_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portThreshold_get(buf.pbo_cfg.port, &buf.pbo_cfg.threshold);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_SWPBO_QUEUETHRESHOLD_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_queueThreshold_get(buf.pbo_cfg.port, buf.pbo_cfg.qid, &buf.pbo_cfg.threshold);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_SWPBO_PORTUSEDPAGE_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portUsedPageCount_get(buf.pbo_cfg.port, &buf.pbo_cfg.currPage, &buf.pbo_cfg.maxPage);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_SWPBO_QUEUEUSEDPAGE_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_queueUsedPageCount_get(buf.pbo_cfg.port, buf.pbo_cfg.qid, &buf.pbo_cfg.currPage, &buf.pbo_cfg.maxPage);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_SWPBO_PORTDSC_GET:
            copy_from_user(&buf.pbo_cfg, user, sizeof(rtdrv_pboCfg_t));
            ret = rtk_swPbo_portDscCount_get(buf.pbo_cfg.port, &buf.pbo_cfg.currDscSram, &buf.pbo_cfg.currDscDram, &buf.pbo_cfg.maxDscSram, &buf.pbo_cfg.maxDscDram);
            copy_to_user(user, &buf.pbo_cfg, sizeof(rtdrv_pboCfg_t));
            break;

        case RTDRV_MDIO_CFG_GET:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_cfg_get(&buf.mdio_cfg.set, &buf.mdio_cfg.port, &buf.mdio_cfg.phyid, &buf.mdio_cfg.fmt);
            copy_to_user(user, &buf.mdio_cfg, sizeof(rtdrv_mdioCfg_t));
            break;

        case RTDRV_MDIO_C22_READ:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_c22_read(buf.mdio_cfg.c22_reg, &buf.mdio_cfg.data);
            copy_to_user(user, &buf.mdio_cfg, sizeof(rtdrv_mdioCfg_t));
            break;

        case RTDRV_MDIO_C45_READ:
            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            ret = rtk_mdio_c45_read(buf.mdio_cfg.dev, buf.mdio_cfg.c45_reg, &buf.mdio_cfg.data);
            copy_to_user(user, &buf.mdio_cfg, sizeof(rtdrv_mdioCfg_t));
            break;
#if defined(CONFIG_COMMON_RT_API)
        case RTDRV_MDIO_READ:
        {
            rt_mdio_info_t mdio_info;

            copy_from_user(&buf.mdio_cfg, user, sizeof(rtdrv_mdioCfg_t));
            memset(&mdio_info, 0, sizeof(rt_mdio_info_t));
            mdio_info.format = buf.mdio_cfg.fmt;
            mdio_info.set = buf.mdio_cfg.set;
            mdio_info.phyAddr = buf.mdio_cfg.phyid;
            mdio_info.dev = buf.mdio_cfg.dev;
            mdio_info.reg = buf.mdio_cfg.c45_reg;
            ret = rt_mdio_read(&mdio_info, &buf.mdio_cfg.data);
            copy_to_user(user, &buf.mdio_cfg, sizeof(rtdrv_mdioCfg_t));
            break;
        }
#endif

#if defined(CONFIG_COMMON_RT_API)
        case RTDRV_RT_SWITCH_PHYPORTID_GET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_phyPortId_get(buf.rt_switch_cfg.portName, &buf.rt_switch_cfg.portId);
            copy_to_user(user, &buf.rt_switch_cfg, sizeof(rtdrv_rt_switchCfg_t));
            break;

        case RTDRV_RT_SWITCH_VERSION_GET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_version_get(&buf.rt_switch_cfg.chipId, &buf.rt_switch_cfg.rev, &buf.rt_switch_cfg.subtype);
            copy_to_user(user, &buf.rt_switch_cfg, sizeof(rtdrv_rt_switchCfg_t));
            break;

        case RTDRV_RT_SWITCH_MAXPKTLENBYPORT_GET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_maxPktLenByPort_get(buf.rt_switch_cfg.port, &buf.rt_switch_cfg.len);
            copy_to_user(user, &buf.rt_switch_cfg, sizeof(rtdrv_rt_switchCfg_t));
            break;

        case RTDRV_RT_SWITCH_DEVICEINFO_GET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_deviceInfo_get(&buf.rt_switch_cfg.devInfo);
            copy_to_user(user, &buf.rt_switch_cfg, sizeof(rtdrv_rt_switchCfg_t));
            break;

        case RTDRV_RT_SWITCH_MODE_GET:
            copy_from_user(&buf.rt_switch_cfg, user, sizeof(rtdrv_rt_switchCfg_t));
            ret = rt_switch_mode_get(&buf.rt_switch_cfg.mode);
            copy_to_user(user, &buf.rt_switch_cfg, sizeof(rtdrv_rt_switchCfg_t));
            break;

#ifdef CONFIG_GPON_FEATURE
        /*gpon*/
        case RTDRV_RT_GPON_ACTIVATE:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_activate(buf.rt_gpon_cfg.initState);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_DEACTIVATE:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_deactivate();
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_ONUSTATE_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_onuState_get(&buf.rt_gpon_cfg.onuState);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_TCONT_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_tcont_get(buf.rt_gpon_cfg.tcontId, &buf.rt_gpon_cfg.allocId);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_USFLOW_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_usFlow_get(buf.rt_gpon_cfg.usFlowId, &buf.rt_gpon_cfg.usFlow);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_DSFLOW_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_dsFlow_get(buf.rt_gpon_cfg.dsFlowId, &buf.rt_gpon_cfg.dsFlow);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_LOOPGEM_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_loop_gemport_get(buf.rt_gpon_cfg.loopGemPortId, &buf.rt_gpon_cfg.loopGemEn);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_PONQUEUE_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_ponQueue_get(buf.rt_gpon_cfg.tcontId, buf.rt_gpon_cfg.tcQueueId, &buf.rt_gpon_cfg.queuecfg);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_SCHEINFO_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_scheInfo_get(&buf.rt_gpon_cfg.scheInfo);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_FLOWCOUNTER_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_flowCounter_get(buf.rt_gpon_cfg.flowId, buf.rt_gpon_cfg.flowCntType, &buf.rt_gpon_cfg.flowCnt);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_PMCOUNTER_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_pmCounter_get(buf.rt_gpon_cfg.pmCntType, &buf.rt_gpon_cfg.pmCnt);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_PONTAG_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_ponTag_get(&buf.rt_gpon_cfg.ponTag);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_OMCI_MIC_GENERATE:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_omci_mic_generate(buf.rt_gpon_cfg.dir, &buf.rt_gpon_cfg.msg, buf.rt_gpon_cfg.msgLen, &buf.rt_gpon_cfg.mic);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_EGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_egrBandwidthCtrlRate_get(&buf.rt_gpon_cfg.rate);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_ATTRIBUTE_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_attribute_get(buf.rt_gpon_cfg.attributeType, &buf.rt_gpon_cfg.attributeValue);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_OMCC_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_omcc_get( &buf.rt_gpon_cfg.omcc);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_FEC_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_fec_get( &buf.rt_gpon_cfg.fec);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_REGISTRATIONID_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_registrationId_get(&buf.rt_gpon_cfg.regId);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_TCONTCOUNTER_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_tcontCounter_get(&buf.rt_gpon_cfg.tcontCnt);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;

        case RTDRV_RT_GPON_SERIALNUMBER_GET:
            copy_from_user(&buf.rt_gpon_cfg, user, sizeof(rtdrv_rt_gponCfg_t));
            ret = rt_gpon_serialNumber_get(&buf.rt_gpon_cfg.sN);
            copy_to_user(user, &buf.rt_gpon_cfg, sizeof(rtdrv_rt_gponCfg_t));
            break;
#endif
        /* Statistic */
        case RTDRV_RT_STAT_PORT_GET:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_port_get(buf.rt_stat_cfg.port, buf.rt_stat_cfg.cntrIdx, &buf.rt_stat_cfg.cntr);
            copy_to_user(user, &buf.rt_stat_cfg, sizeof(rtdrv_rt_statCfg_t));
            break;

        case RTDRV_RT_STAT_PORT_GETALL:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_port_getAll(buf.rt_stat_cfg.port, &buf.rt_stat_cfg.portCntrs);
            copy_to_user(user, &buf.rt_stat_cfg, sizeof(rtdrv_rt_statCfg_t));
            break;

        case RTDRV_RT_STAT_VLANCNT_FILTER_GET:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_vlanCnt_filter_get(buf.rt_stat_cfg.index, &buf.rt_stat_cfg.filter);
            copy_to_user(user, &buf.rt_stat_cfg, sizeof(rtdrv_rt_statCfg_t));
            break;

        case RTDRV_RT_STAT_VLANCNT_MIB_GET:
            copy_from_user(&buf.rt_stat_cfg, user, sizeof(rtdrv_rt_statCfg_t));
            ret = rt_stat_vlanCnt_mib_get(buf.rt_stat_cfg.index, &buf.rt_stat_cfg.portCntrs);
            copy_to_user(user, &buf.rt_stat_cfg, sizeof(rtdrv_rt_statCfg_t));
            break;

        /* Rate */
        case RTDRV_RT_RATE_PORTIGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_portIgrBandwidthCtrlRate_get(buf.rt_rate_cfg.port, &buf.rt_rate_cfg.rate);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_PORTEGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_portEgrBandwidthCtrlRate_get(buf.rt_rate_cfg.port, &buf.rt_rate_cfg.rate);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_STORMCONTROLMETERIDX_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_stormControlMeterIdx_get(buf.rt_rate_cfg.port, buf.rt_rate_cfg.stormType, &buf.rt_rate_cfg.index);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_STORMCONTROLPORTENABLE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_stormControlPortEnable_get(buf.rt_rate_cfg.port, buf.rt_rate_cfg.stormType, &buf.rt_rate_cfg.enable);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERTYPE_ADD:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterType_add(buf.rt_rate_cfg.type, &buf.rt_rate_cfg.index);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERTYPE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterType_get(buf.rt_rate_cfg.index, &buf.rt_rate_cfg.type);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERRATE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterRate_get(buf.rt_rate_cfg.index, &buf.rt_rate_cfg.rate);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERMAPPINGHW_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterMappingHw_get(buf.rt_rate_cfg.v_index, &buf.rt_rate_cfg.meterMap);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERMAPPINGVIRTUAL_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterMappingVirtual_get(buf.rt_rate_cfg.meterMap, &buf.rt_rate_cfg.v_index);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERBUCKET_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterBucket_get(buf.rt_rate_cfg.index, &buf.rt_rate_cfg.bucketSize);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERIFGINCLUDE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterIfgInclude_get(buf.rt_rate_cfg.index, &buf.rt_rate_cfg.ifgInclude);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        case RTDRV_RT_RATE_SHAREMETERMODE_GET:
            copy_from_user(&buf.rt_rate_cfg, user, sizeof(rtdrv_rt_rateCfg_t));
            ret = rt_rate_shareMeterMode_get(buf.rt_rate_cfg.index, &buf.rt_rate_cfg.mode);
            copy_to_user(user, &buf.rt_rate_cfg, sizeof(rtdrv_rt_rateCfg_t));
            break;

        /* QoS */
        case RTDRV_RT_QOS_SCHEDULINGQUEUE_GET:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_schedulingQueue_get(buf.rt_qos_cfg.port, &buf.rt_qos_cfg.qweights);
            copy_to_user(user, &buf.rt_qos_cfg, sizeof(rtdrv_rt_qosCfg_t));
            break;

        case RTDRV_RT_QOS_DSCP2PBIT_GET:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_dscp2pbit_get(&buf.rt_qos_cfg.dscp2Pbit);
            copy_to_user(user, &buf.rt_qos_cfg, sizeof(rtdrv_rt_qosCfg_t));
            break;

        case RTDRV_RT_QOS_INGRESS_QOS_GET:
            copy_from_user(&buf.rt_qos_cfg, user, sizeof(rtdrv_rt_qosCfg_t));
            ret = rt_qos_ingress_qos_get(buf.rt_qos_cfg.index, &buf.rt_qos_cfg.igrQos);
            copy_to_user(user, &buf.rt_qos_cfg, sizeof(rtdrv_rt_qosCfg_t));
            break;

        /* Interrupt */
        case RTDRV_RT_INTR_IMR_GET:
            copy_from_user(&buf.rt_intr_cfg, user, sizeof(rtdrv_rt_intrCfg_t));
            ret = rt_intr_imr_get(buf.rt_intr_cfg.intr, &buf.rt_intr_cfg.enable);
            copy_to_user(user, &buf.rt_intr_cfg, sizeof(rtdrv_rt_intrCfg_t));
            break;

        /* Security */
        case RTDRV_RT_SEC_PORTATTACKPREVENTSTATE_GET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_portAttackPreventState_get(buf.rt_sec_cfg.port, &buf.rt_sec_cfg.enable);
            copy_to_user(user, &buf.rt_sec_cfg, sizeof(rtdrv_rt_secCfg_t));
            break;

        case RTDRV_RT_SEC_ATTACKPREVENT_GET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_attackPrevent_get(buf.rt_sec_cfg.attackType, &buf.rt_sec_cfg.action);
            copy_to_user(user, &buf.rt_sec_cfg, sizeof(rtdrv_rt_secCfg_t));
            break;

        case RTDRV_RT_SEC_ATTACKFLOODTHRESH_GET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_attackFloodThresh_get(buf.rt_sec_cfg.type, &buf.rt_sec_cfg.floodThresh);
            copy_to_user(user, &buf.rt_sec_cfg, sizeof(rtdrv_rt_secCfg_t));
            break;

        case RTDRV_RT_SEC_ATTACKFLOODTHRESHUNIT_GET:
            copy_from_user(&buf.rt_sec_cfg, user, sizeof(rtdrv_rt_secCfg_t));
            ret = rt_sec_attackFloodThreshUnit_get(buf.rt_sec_cfg.type, &buf.rt_sec_cfg.floodThreshUnit);
            copy_to_user(user, &buf.rt_sec_cfg, sizeof(rtdrv_rt_secCfg_t));
            break;

        /* Trap */
        case RTDRV_RT_TRAP_PORTIGMPMLDCTRLPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portIgmpMldCtrlPktAction_get(buf.rt_trap_cfg.port, buf.rt_trap_cfg.igmpMldType, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_OAMPDUACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_oamPduAction_get(&buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_OMCIACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_omciAction_get(&buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTDHCPPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portDhcpPktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTPPPOEPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portPppoePktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTSTPPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portStpPktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTHOSTPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portHostPktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTETHERTYPEPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portEthertypePktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTARPPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portArpPktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTLOOPDETECTPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portLoopDetectPktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PORTDOT1XPKTACTION_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_portDot1xPktAction_get(buf.rt_trap_cfg.port, &buf.rt_trap_cfg.action);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PATTERN_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_pattern_get(buf.rt_trap_cfg.type, &buf.rt_trap_cfg.attern);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        case RTDRV_RT_TRAP_PRIORITY_GET:
            copy_from_user(&buf.rt_trap_cfg, user, sizeof(rtdrv_rt_trapCfg_t));
            ret = rt_trap_priority_get(buf.rt_trap_cfg.port, buf.rt_trap_cfg.type, &buf.rt_trap_cfg.pri);
            copy_to_user(user, &buf.rt_trap_cfg, sizeof(rtdrv_rt_trapCfg_t));
            break;

        /* I2C */
        case RTDRV_RT_I2C_ENABLE_GET:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_enable_get(buf.rt_i2c_cfg.i2cPort, &buf.rt_i2c_cfg.enable);
            copy_to_user(user, &buf.rt_i2c_cfg, sizeof(rtdrv_rt_i2cCfg_t));
            break;

        case RTDRV_RT_I2C_READ:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_read(buf.rt_i2c_cfg.i2cPort, buf.rt_i2c_cfg.devID, buf.rt_i2c_cfg.regAddr, &buf.rt_i2c_cfg.data);
            copy_to_user(user, &buf.rt_i2c_cfg, sizeof(rtdrv_rt_i2cCfg_t));
            break;

        case RTDRV_RT_I2C_SEQ_READ:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            pReadData = (uint32 *) kmalloc(sizeof(uint32) * buf.rt_i2c_cfg.seqCnt, GFP_KERNEL);
            if(NULL == pReadData)
            {
                return RT_ERR_NULL_POINTER;
            }
            ret = rt_i2c_seq_read(buf.rt_i2c_cfg.i2cPort, buf.rt_i2c_cfg.devID, buf.rt_i2c_cfg.regAddr, pReadData, buf.rt_i2c_cfg.seqCnt);
            copy_to_user(user, &buf.rt_i2c_cfg, sizeof(rtdrv_rt_i2cCfg_t));
            copy_to_user(buf.rt_i2c_cfg.pSeqData, pReadData, sizeof(uint32) * buf.rt_i2c_cfg.seqCnt);
            kfree(pReadData);
            break;

        case RTDRV_RT_I2C_CLOCK_GET:
            copy_from_user(&buf.rt_i2c_cfg, user, sizeof(rtdrv_rt_i2cCfg_t));
            ret = rt_i2c_clock_get(buf.rt_i2c_cfg.i2cPort, &buf.rt_i2c_cfg.clock);
            copy_to_user(user, &buf.rt_i2c_cfg, sizeof(rtdrv_rt_i2cCfg_t));
            break;
        /*Port */
        case RTDRV_RT_PORT_LINK_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_link_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.linkStatus);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_SPEEDDUPLEX_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_speedDuplex_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.speed, &buf.rt_port_cfg.duplex);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_FLOWCTRL_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_flowctrl_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.txStatus, &buf.rt_port_cfg.rxStatus);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_PHYAUTONEGOENABLE_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyAutoNegoEnable_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.enable);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_PHYAUTONEGOABILITY_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyAutoNegoAbility_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.ability);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_PHYFORCEMODEABILITY_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyForceModeAbility_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.speed, &buf.rt_port_cfg.duplex, &buf.rt_port_cfg.flowControl);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_PHYREG_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyReg_get(buf.rt_port_cfg.port, buf.rt_port_cfg.page, buf.rt_port_cfg.reg, &buf.rt_port_cfg.data);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_PHYPOWERDOWN_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_phyPowerDown_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.enable);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_ADMINENABLE_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_adminEnable_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.enable);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_ISOLATIONENTRY_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_isolationEntry_get(buf.rt_port_cfg.mode, buf.rt_port_cfg.port, &buf.rt_port_cfg.portmask, &buf.rt_port_cfg.extPortmask);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_UNITYPE_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_uniType_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.uniType);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_MACFORCEABILITY_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_macForceAbility_get(buf.rt_port_cfg.port,&buf.rt_port_cfg.macAbility);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_MACFORCEABILITYSTATE_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_macForceAbilityState_get(buf.rt_port_cfg.port,&buf.rt_port_cfg.enable);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_EEEENABLE_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_eeeEnable_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.enable);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_MACLOOPBACKENABLE_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            ret = rt_port_macLoopbackEnable_get(buf.rt_port_cfg.port, &buf.rt_port_cfg.enable, &buf.rt_port_cfg.swapEnable);
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_PORT_PORTDEF_GET:
            copy_from_user(&buf.rt_port_cfg, user, sizeof(rtdrv_rt_portCfg_t));
            buf.rt_port_cfg.rt_port0 = rt_port0;
            buf.rt_port_cfg.rt_port1 = rt_port1;
            buf.rt_port_cfg.rt_port2 = rt_port2;
            buf.rt_port_cfg.rt_port3 = rt_port3;
            buf.rt_port_cfg.rt_port4 = rt_port4;
            buf.rt_port_cfg.rt_port5 = rt_port5;
            buf.rt_port_cfg.rt_port6 = rt_port6;
            buf.rt_port_cfg.rt_port7 = rt_port7;
            buf.rt_port_cfg.rt_port_pon = rt_port_pon;
            buf.rt_port_cfg.rt_port_cpu0 = rt_port_cpu0;
            buf.rt_port_cfg.rt_port_cpu1 = rt_port_cpu1;
            buf.rt_port_cfg.rt_port_cpu2 = rt_port_cpu2;
            buf.rt_port_cfg.rt_port_cpu3 = rt_port_cpu3;
            buf.rt_port_cfg.rt_port_cpu4 = rt_port_cpu4;
            buf.rt_port_cfg.rt_port_cpu5 = rt_port_cpu5;
            buf.rt_port_cfg.rt_port_cpu6 = rt_port_cpu6;
            buf.rt_port_cfg.rt_port_cpu7 = rt_port_cpu7;
            buf.rt_port_cfg.rt_port_max = rt_port_max;
            ret = RT_ERR_OK;
            copy_to_user(user, &buf.rt_port_cfg, sizeof(rtdrv_rt_portCfg_t));
            break;

        case RTDRV_RT_L2_PORTLIMITLEARNINGCNT_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_portLimitLearningCnt_get(buf.rt_l2_cfg.port, &buf.rt_l2_cfg.macCnt);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_PORTLIMITLEARNINGCNTACTION_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_portLimitLearningCntAction_get(buf.rt_l2_cfg.port, &buf.rt_l2_cfg.learnCntAct);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_NEWMACOP_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_newMacOp_get(buf.rt_l2_cfg.port, &buf.rt_l2_cfg.fwdAction);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_ADDR_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_addr_get(&buf.rt_l2_cfg.l2UcastData);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_MCASTADDR_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_mcastAddr_get(&buf.rt_l2_cfg.mcastAddr);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_NEXTVALIDADDR_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_nextValidAddr_get(&buf.rt_l2_cfg.scanIdx, &buf.rt_l2_cfg.l2UcastData);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_NEXTVALIDADDRONPORT_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_nextValidAddrOnPort_get(buf.rt_l2_cfg.port, &buf.rt_l2_cfg.scanIdx, &buf.rt_l2_cfg.l2UcastData);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_ILLEGALPORTMOVEACTION_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_illegalPortMoveAction_get(buf.rt_l2_cfg.port, &buf.rt_l2_cfg.fwdAction);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));

            break;

        case RTDRV_RT_L2_AGETIME_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_ageTime_get( &buf.rt_l2_cfg.ageTime);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_IVLSVL_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_ivlSvl_get(&buf.rt_l2_cfg.ivlEn);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_FWDGROUPID_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_fwdGroupId_get(buf.rt_l2_cfg.idx, &buf.rt_l2_cfg.mask);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

        case RTDRV_RT_L2_FWDGROUPACTION_GET:
            copy_from_user(&buf.rt_l2_cfg, user, sizeof(rtdrv_rt_l2Cfg_t));
            ret = rt_l2_fwdGroupAction_get(buf.rt_l2_cfg.idx, &buf.rt_l2_cfg.portAct);
            copy_to_user(user, &buf.rt_l2_cfg, sizeof(rtdrv_rt_l2Cfg_t));
            break;

#ifdef CONFIG_EPON_FEATURE
        case RTDRV_RT_EPON_LLID_ENTRY_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_llid_entry_get(&buf.rt_epon_cfg.llidEntry);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_REGISTERREQ_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_registerReq_get(&buf.rt_epon_cfg.regEntry);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_CHURNINGKEY_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_churningKey_get(&buf.rt_epon_cfg.entry);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_CHURNING_STATE_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_churningStatus_get(&buf.rt_epon_cfg.state);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;


        case RTDRV_RT_EPON_USFECSTATE_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_usFecState_get(&buf.rt_epon_cfg.state);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_DSFECSTATE_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_dsFecState_get(&buf.rt_epon_cfg.state);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_MIBCOUNTER_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_mibCounter_get(&buf.rt_epon_cfg.counter);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_LOSSTATE_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_losState_get(&buf.rt_epon_cfg.state);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_PONQUEUE_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_ponQueue_get(buf.rt_epon_cfg.llid, buf.rt_epon_cfg.queueId, &buf.rt_epon_cfg.queuecfg);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_EGRBANDWIDTHCTRLRATE_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_egrBandwidthCtrlRate_get(&buf.rt_epon_cfg.rate);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;
        case RTDRV_RT_EPON_MPCP_INFO_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_mpcp_info_get(&buf.rt_epon_cfg.info);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;

        case RTDRV_RT_EPON_MPCP_QUEUE_THRESHOLD_GET:
            copy_from_user(&buf.rt_epon_cfg, user, sizeof(rtdrv_rt_eponCfg_t));
            ret = rt_epon_mpcp_queue_threshold_get(&buf.rt_epon_cfg.thresholdRpt);
            copy_to_user(user, &buf.rt_epon_cfg, sizeof(rtdrv_rt_eponCfg_t));
            break;
#endif
#ifdef CONFIG_COMMON_RT_PONMISC
        case RTDRV_RT_PONMISC_MODESPEED_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_modeSpeed_get(&buf.rt_ponmisc_cfg.ponMode, &buf.rt_ponmisc_cfg.ponSpeed);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_SID_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_sid_get(buf.rt_ponmisc_cfg.sid, &buf.rt_ponmisc_cfg.sidInfo);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_TRX_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_transceiver_get(buf.rt_ponmisc_cfg.trxDataType, &buf.rt_ponmisc_cfg.trxData);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_BURSTPOLARITYREVERSE_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_burstPolarityReverse_get(&buf.rt_ponmisc_cfg.burstPolarity);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_RXLOSPOLARITYREVERSE_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_rxlosPolarityReverse_get(&buf.rt_ponmisc_cfg.rxlosPolarity);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_FORCELASER_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_forceLaserState_get(&buf.rt_ponmisc_cfg.laserStatus);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_FORCEPRBS_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_forcePRBS_get(&buf.rt_ponmisc_cfg.prbs);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_FORCEPRBS_RX_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_forcePRBS_rx_get(&buf.rt_ponmisc_cfg.prbs_rx);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_PRBS_RX_STATUS_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_prbsRxStatus_get(&buf.rt_ponmisc_cfg.prbs_rx_status);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_SELFROGUE_CFG_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_selfRogue_cfg_get(&buf.rt_ponmisc_cfg.cfg);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

        case RTDRV_RT_PONMISC_SELFROGUE_CNT_GET:
            copy_from_user(&buf.rt_ponmisc_cfg, user, sizeof(rtdrv_rt_ponmiscCfg_t));
            ret = rt_ponmisc_selfRogue_count_get(&buf.rt_ponmisc_cfg.cnt);
            copy_to_user(user, &buf.rt_ponmisc_cfg, sizeof(rtdrv_rt_ponmiscCfg_t));
            break;

#endif
        case RTDRV_RT_CLS_RULE_GET:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_rule_get(buf.rt_cls_cfg.op, buf.rt_cls_cfg.index, buf.rt_cls_cfg.dir, &buf.rt_cls_cfg.clsRuleInfo);
            copy_to_user(user, &buf.rt_cls_cfg, sizeof(rtdrv_rt_clsCfg_t));
            break;

        case RTDRV_RT_CLS_EXTEND_RULE_GET:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_extend_rule_get(buf.rt_cls_cfg.index, &buf.rt_cls_cfg.extClsRule);
            copy_to_user(user, &buf.rt_cls_cfg, sizeof(rtdrv_rt_clsCfg_t));
            break;

        case RTDRV_RT_CLS_VLAN_PASER_GET:
            copy_from_user(&buf.rt_cls_cfg, user, sizeof(rtdrv_rt_clsCfg_t));
            ret = rt_cls_vlan_paser_get(buf.rt_cls_cfg.port, buf.rt_cls_cfg.type, &buf.rt_cls_cfg.data);
            copy_to_user(user, &buf.rt_cls_cfg, sizeof(rtdrv_rt_clsCfg_t));
            break;

        case RTDRV_RT_MIRROR_PORTBASED_GET:
            copy_from_user(&buf.rt_mirror_cfg, user, sizeof(rtdrv_rt_mirrorCfg_t));
            ret = rt_mirror_portBased_get(&buf.rt_mirror_cfg.mirroringPort, &buf.rt_mirror_cfg.mirroredRxPortmask, &buf.rt_mirror_cfg.mirroredTxPortmask);
            copy_to_user(user, &buf.rt_mirror_cfg, sizeof(rtdrv_rt_mirrorCfg_t));
            break;

        case RTDRV_RT_TIME_CURTIME_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_curTime_get(&buf.rt_time_cfg.timeStamp);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;

        case RTDRV_RT_TIME_FREQUENCY_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_frequency_get(&buf.rt_time_cfg.freq);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;

        case RTDRV_RT_TIME_PTPPORTENABLE_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ptpPortEnable_get(buf.rt_time_cfg.port,&buf.rt_time_cfg.enable);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;

        case RTDRV_RT_TIME_PTPIGRACTION_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ptpIgrAction_get(buf.rt_time_cfg.type,&buf.rt_time_cfg.igrAction);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;

        case RTDRV_RT_TIME_PTPEGRACTION_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ptpEgrAction_get(&buf.rt_time_cfg.egrAction);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;

        case RTDRV_RT_TIME_PONTODTIME_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ponTodTime_get(&buf.rt_time_cfg.ponTod);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;

        case RTDRV_RT_TIME_PONTODTIMEENABLE_GET:
            copy_from_user(&buf.rt_time_cfg, user, sizeof(rtdrv_rt_timeCfg_t));
            ret = rt_time_ponTodTimeEnable_get(&buf.rt_time_cfg.enable);
            copy_to_user(user, &buf.rt_time_cfg, sizeof(rtdrv_rt_timeCfg_t));
            break;
#endif

#if defined(CONFIG_EUROPA_FEATURE) || defined(CONFIG_RTL8290C_FEATURE) || defined(CONFIG_RTL8291_FEATURE)
        case RTDRV_LDD_PARAMETER_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_parameter_get(buf.ldd_cfg.rset_code, buf.ldd_cfg.ldo_code, buf.ldd_cfg.flash_data);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_POWER_ON_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_powerOnStatus_get(&buf.ldd_cfg.adc_gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TX_POWER_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_power_get(buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code, &buf.ldd_cfg.half_vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_RX_POWER_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rx_power_get(buf.ldd_cfg.rssi_v0, &buf.ldd_cfg.rssi_voltage, &buf.ldd_cfg.rssi_voltage2);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_RSSI_VOLTAGE_GET:
#if 0
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtdrv_lddCfg_t));
            ret = rtk_ldd_rssiVoltage_get(&buf.ldd_cfg.rssi_voltage);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtdrv_lddCfg_t));
#endif
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rssiVoltage_get(&buf.ldd_cfg);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_RSSI_V0_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rssiV0_get(&buf.ldd_cfg.rssi_v0);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_VDD_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_vdd_get(&buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_MPD0_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_mpd0_get(buf.ldd_cfg.count, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TEMPERATURE_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_temperature_get(&buf.ldd_cfg.temperature);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TX_BIAS_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_bias_get(&buf.ldd_cfg.tx_bias);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TX_MOD_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_mod_get(&buf.ldd_cfg.tx_mod);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_DRIVER_VERSION_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_driver_version_get(&buf.ldd_cfg.driver_version);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TXCROSS_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_txCross_get(&buf.ldd_cfg.state, &buf.ldd_cfg.count, &buf.ldd_cfg.temperature);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_APC_IAVG_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcIavg_get(&buf.ldd_cfg.count);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_APC_ER_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcEr_get(&buf.ldd_cfg.count);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_APC_ERTRIM_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcErTrim_get(&buf.ldd_cfg.count);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_RXLOS_REFDAC_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rxlosRefDac_get(&buf.ldd_cfg.count);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_RXLOS_HYSTSEL_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rxlosHystSel_get(&buf.ldd_cfg.count);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_RX_POWER2_GET:
            //osal_printf("\n%s: %d\n",__FUNCTION__, __LINE__);             
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rx_power2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.rxp);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));           
            break;

        case RTDRV_LDD_TX_POWER2_GET:
            //osal_printf("\n%s: %d\n",__FUNCTION__, __LINE__);             
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_power2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.txp);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));           
            break;

        case RTDRV_LDD_PARAMETER2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_parameter2_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_MPD02_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_mpd02_get(buf.ldd_cfg.devId, buf.ldd_cfg.count, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TEMPERATURE2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_temperature2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TX_BIAS2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_bias2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.tx_bias);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TX_MOD2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_tx_mod2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.tx_mod);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TXCROSS2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_txCross2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.state, &buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));            
            break;

        case RTDRV_LDD_APC_IAVG2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcIavg2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));           
            break;
    
        case RTDRV_LDD_APC_ER2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcEr2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));           
            break;
    
        case RTDRV_LDD_APC_ERTRIM2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apcErTrim2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));           
            break;
    
        case RTDRV_LDD_RXLOS_REFDAC2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rxlosRefDac2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));           
            break;
    
        case RTDRV_LDD_RXLOS_HYSTSEL2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_rxlosHystSel2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));  
            break;

        case RTDRV_LDD_SDADC_CODE2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_sdadc_code2_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));  
            break;
        case RTDRV_LDD_LASER_LUT2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_laserLut2_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code, &buf.ldd_cfg.half_vdd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));  
            break;

        case RTDRV_LDD_APD_LUT2_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_apdLut2_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));  
            break;

        case RTDRV_LDD_EFUSE2_GET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_efuse2_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
             copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t)); 
             break;
			 
        case RTDRV_LDD_VDD2_GET:
             copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
             ret = rtk_ldd_vdd2_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code);
             copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
             break;

        case RTDRV_LDD_REGBIT_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_regBit_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.half_vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            //osal_printf("\n%s: %d  ret = %d value = 0x%x\n",__FUNCTION__, __LINE__, ret, buf.ldd_cfg.gnd_code);  			
            break;
                  
        case RTDRV_LDD_REGBITS_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_regBits_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, buf.ldd_cfg.half_vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));
            break;

        case RTDRV_LDD_TXES_CONFIG_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_txEsConfig_get(buf.ldd_cfg.devId, buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));  
            break;

        case RTDRV_LDD_DAC_GET:
            copy_from_user(&buf.ldd_cfg, user, sizeof(rtk_ldd_cfg_t));
            ret = rtk_ldd_dac_get(buf.ldd_cfg.devId, &buf.ldd_cfg.vdd_code, &buf.ldd_cfg.gnd_code);
            copy_to_user(user, &buf.ldd_cfg, sizeof(rtk_ldd_cfg_t));  
			//osal_printf("\n%s: %d  dac_offset = 0x%x dac_code = 0x%x \n",__FUNCTION__, __LINE__, buf.ldd_cfg.vdd_code, buf.ldd_cfg.gnd_code);
            break;

#endif /*CONFIG_EUROPA_FEATURE || CONFIG_RTL8290C_FEATURE || CONFIG_RTL8291_FEATURE*/
#endif /* CONFIG_XDSL_RG_DIAGSHELL */
        default:
            break;
    }

    return ret;
}




/* Function Name:
 *      rtdrv_init
 * Description:
 *      Init driver and register netfilter socket option
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 __init rtdrv_init(void)
{
    printk("\n rtdrv_init !!!! for netfilter\n");

    /* register netfilter socket option */
    if (nf_register_sockopt(&rtdrv_sockopts))
    {
        osal_printf("[%s]: nf_register_sockopt failed.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }
#if defined(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE) && defined(CONFIG_COMMON_RT_API)
    if (nf_register_sockopt(&rtdrv_ext_sockopts))
    {
        osal_printf("[%s]: nf_register_sockopt failed for rtdrv_ext.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }
#endif

	printk("[%s]: rtdrv_ext_igmpHook_sockopts init.\n", __FUNCTION__);

#if defined(CONFIG_RTK_IGMP_MLD_SNOOPING_MODULE) && defined(CONFIG_COMMON_RT_API)
    if (nf_register_sockopt(&rtdrv_ext_igmpHook_sockopts))
    {
        printk("[%s]: nf_register_sockopt failed for rtdrv_ext_igmpHook_sockopts.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }
	else
	{
		printk("[%s]: rtdrv_ext_igmpHook_sockopts success.\n", __FUNCTION__);
	}
#endif


#if defined(__RTDRV_MODULE__)
    osal_printf("Init RTDRV Driver Module....OK\n");
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtdrv_exit
 * Description:
 *      Exit driver and unregister netfilter socket option
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void __exit rtdrv_exit(void)
{
    nf_unregister_sockopt(&rtdrv_sockopts);
#if defined(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE) && defined(CONFIG_COMMON_RT_API)
    nf_unregister_sockopt(&rtdrv_ext_sockopts);
#endif

#if defined(CONFIG_RTK_IGMP_MLD_SNOOPING_MODULE) && defined(CONFIG_COMMON_RT_API)
    nf_unregister_sockopt(&rtdrv_ext_igmpHook_sockopts);
#endif

#if defined(__RTDRV_MODULE__)
    osal_printf("Exit RTDRV Driver Module....OK\n");
#endif

}


module_init(rtdrv_init);
module_exit(rtdrv_exit);

MODULE_DESCRIPTION ("Switch SDK User/Kernel Driver Module");
