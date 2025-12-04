/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Define interrupt broadcaster
 *
 * Feature : Broadcast interrupt event to kernel/user space
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>

#include "rtk/switch.h"
#include "rtk/intr.h"
#include "rtk/irq.h"

#if defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_LUNA_G3_SERIES)

#include <module/gpon/gpon.h>
#include "../../module/pkt_redirect/pkt_redirect.h"

#endif

#include <dal/rtl9607c/dal_rtl9607c.h>

#include "common/debug/rt_log.h"
#include "hal/common/halctrl.h"
#include "intr_bcaster.h"
#include "osal/time.h"
/*
 * Data Declaration
 */
typedef struct
{
    struct work_struct  work;
    intrBcasterMsg_t    msgData;
} intrBcasterWork_t;

static BLOCKING_NOTIFIER_HEAD(gNotifierHead);
#ifndef USE_GENERIC_NETLINK_INTF
static struct sock      *pgSocket;
#endif
static rtk_enable_t     gImrLinkChange;
static rtk_enable_t     gImrDyingGasp;
static int32            gIsRegEvtState;
/*
 * Function Declaration
 */
static void send_broadcast(struct work_struct *p)
{
    int                 ret;
    struct sk_buff      *pSkb;
    intrBcasterWork_t   *work;
    void                *pMsgData;
#ifdef USE_GENERIC_NETLINK_INTF
    void                *pGeNlh;
    struct nlattr       *pNla;
    unsigned int        mcGrpId;
#else
    struct nlmsghdr     *pNlh;
#endif

    work = (intrBcasterWork_t *)p;

#ifdef USE_GENERIC_NETLINK_INTF
    if (MSG_TYPE_LINK_CHANGE == work->msgData.intrType)
        mcGrpId = gIntrBcasterGenlMcGrpLinkChange.id;
    else if (MSG_TYPE_DYING_GASP == work->msgData.intrType)
        mcGrpId = gIntrBcasterGenlMcGrpDyingGasp.id;
    else
        goto call_notifier;

    // allocating a Generic Netlink message buffer
    pSkb = genlmsg_new(MAX_BYTE_PER_INTR_BCASTER_MSG, GFP_ATOMIC);
    if (!pSkb)
        goto call_notifier;

    // create the message headers
    pGeNlh = genlmsg_put(pSkb, 0, 0,
                        &gIntrBcasterGenlFamily, 0,
                        INTR_BCASTER_GENL_CMD_EVENT);
    if (!pGeNlh)
        goto genlmsg_failure;

    // add a INTR_BCASTER_GENL_ATTR_EVENT attribute
    pNla = nla_reserve(pSkb, INTR_BCASTER_GENL_ATTR_EVENT,
                        MAX_BYTE_PER_INTR_BCASTER_MSG);
    if (!pNla)
        goto genlmsg_failure;

    pMsgData = nla_data(pNla);
    if (!pMsgData)
        goto genlmsg_failure;

    memcpy(pMsgData, &work->msgData, MAX_BYTE_PER_INTR_BCASTER_MSG);

    // finalize the message
    if (genlmsg_end(pSkb, pGeNlh) < 0)
        goto genlmsg_failure;

    // sending Generic Netlink messages
    ret = genlmsg_multicast(pSkb, 0, mcGrpId, GFP_ATOMIC);
    if (ret < 0)
        printk(KERN_INFO "send interrupt broadcast to generic netlink failed\n");

    goto call_notifier;

genlmsg_failure:
    if (pSkb)
        nlmsg_free(pSkb);
#else
    if (!netlink_has_listeners(pgSocket, work->msgData.intrType + 1))
        goto call_notifier;

    pSkb = nlmsg_new(MAX_BYTE_PER_INTR_BCASTER_MSG, GFP_ATOMIC);
    if (!pSkb)
        goto call_notifier;

    pNlh = nlmsg_put(pSkb, 0, 0, 0, MAX_BYTE_PER_INTR_BCASTER_MSG, 0);
    if (!pNlh)
        goto nlmsg_failure;

    pMsgData = nlmsg_data(pNlh);
    if (!pMsgData)
        goto nlmsg_failure;

    memcpy(pMsgData, &work->msgData, MAX_BYTE_PER_INTR_BCASTER_MSG);

    ret = nlmsg_multicast(pgSocket, pSkb, 0, work->msgData.intrType + 1, GFP_ATOMIC);
    if (ret < 0)
    {
        printk(KERN_INFO "send interrupt broadcast to netlink failed, ret %d intrType %d intrStatus %d\n",
            ret, work->msgData.intrType, work->msgData.intrStatus);
    }

    goto call_notifier;

nlmsg_failure:
    if (pSkb)
        kfree_skb(pSkb);
#endif

call_notifier:
    ret = blocking_notifier_call_chain(&gNotifierHead, work->msgData.intrType, &work->msgData);
    if (NOTIFY_BAD == ret)
        printk(KERN_INFO "send interrupt broadcast to blocking notifier failed\n");

    kfree(work);
}

int queue_broadcast(bcast_msg_type_t  intrType,
                            uint32          intrSubType,
                            uint32          intrBitMask,
                            rtk_enable_t    intrStatus)
{
    intrBcasterWork_t   *work;

    work = kmalloc(sizeof(intrBcasterWork_t), GFP_ATOMIC);
    if (!work)
        return RT_ERR_FAILED;

    INIT_WORK((struct work_struct *)work, send_broadcast);

    work->msgData.intrType = intrType;
    work->msgData.intrSubType = intrSubType;
    work->msgData.intrBitMask = intrBitMask;
    work->msgData.intrStatus = intrStatus;

    return schedule_work((struct work_struct *)work);
}

EXPORT_SYMBOL (queue_broadcast);

static uint32 _linkChangeIntCnt=0;

int32 link_change_isr_counter_get(uint32 *counter)
{
    /* function body */
    *counter = _linkChangeIntCnt;

    /*reset counter*/
    _linkChangeIntCnt=0;

    return RT_ERR_OK;
}

EXPORT_SYMBOL(link_change_isr_counter_get);

static void link_change_isr(void)
{
    int32                   ret;
    int32                   portId;
    rtk_enable_t            enable;
    rtk_portmask_t          upPortMask;
    rtk_portmask_t          downPortMask;
    rtk_port_linkStatus_t   linkStatus;


    // get interrupt status
    if (RT_ERR_OK != (ret = rtk_intr_ims_get(INTR_TYPE_LINK_CHANGE, &enable)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }

    _linkChangeIntCnt ++;

    // collect link up status
    if (RT_ERR_OK != (ret = rtk_intr_linkupStatus_get(&upPortMask)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }
    // collect link down status
    if (RT_ERR_OK != (ret = rtk_intr_linkdownStatus_get(&downPortMask)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }

    // send out the interrupt event
    for (portId = 0; portId <= HAL_GET_MAX_PORT(); portId++)
    {
        if (!HAL_IS_PORT_EXIST(portId))
            continue;

        if (RTK_PORTMASK_IS_PORT_SET(upPortMask, portId) && RTK_PORTMASK_IS_PORT_SET(downPortMask, portId))
        {
            if (RT_ERR_OK != (ret = rtk_port_link_get(portId, &linkStatus)))
                continue;

            if (PORT_LINKDOWN == linkStatus)
            {
                queue_broadcast(MSG_TYPE_LINK_CHANGE, INTR_STATUS_LINKUP, portId, ENABLED);
                queue_broadcast(MSG_TYPE_LINK_CHANGE, INTR_STATUS_LINKDOWN, portId, ENABLED);
            }
            else
            {
                queue_broadcast(MSG_TYPE_LINK_CHANGE, INTR_STATUS_LINKDOWN, portId, ENABLED);
                queue_broadcast(MSG_TYPE_LINK_CHANGE, INTR_STATUS_LINKUP, portId, ENABLED);
            }
        }
        else
        {
            if (RTK_PORTMASK_IS_PORT_SET(upPortMask, portId)) {
                queue_broadcast(MSG_TYPE_LINK_CHANGE, INTR_STATUS_LINKUP, portId, ENABLED);
            }

            if (RTK_PORTMASK_IS_PORT_SET(downPortMask, portId)) {
                queue_broadcast(MSG_TYPE_LINK_CHANGE, INTR_STATUS_LINKDOWN, portId, ENABLED);
            }
        }
    }

    // clear interrupt status
    if (RT_ERR_OK != (ret = rtk_intr_linkupPortStatus_clear(upPortMask)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }
    if (RT_ERR_OK != (ret = rtk_intr_linkdownPortStatus_clear(downPortMask)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }
    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(INTR_TYPE_LINK_CHANGE)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }

}

static int32 common_intr_register(rtk_intr_type_t   intrType,
                                    void            (*isr)(void),
                                    rtk_enable_t    *pImr)
{
    int32   ret;

    if (!pImr)
        return RT_ERR_INPUT;

    if (RT_ERR_OK != (ret = rtk_intr_imr_get(intrType, pImr)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_imr_set(intrType, DISABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_irq_isr_register(intrType, isr)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(intrType)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_imr_set(intrType, ENABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    return ret;
}

static int32 common_intr_unregister(rtk_intr_type_t     intrType,
                                    rtk_enable_t        *pImr)
{
    int32   ret;

    if (!pImr)
        return RT_ERR_INPUT;

    if (RT_ERR_OK != (ret = rtk_intr_imr_set(intrType, DISABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_irq_isr_unregister(intrType)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_imr_restore(*pImr)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    return ret;
}

static int32 link_change_intr_register(void)
{
    int32   ret;

    if (RT_ERR_OK != (ret = rtk_intr_imr_get(INTR_TYPE_LINK_CHANGE, &gImrLinkChange)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_imr_set(INTR_TYPE_LINK_CHANGE, DISABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_irq_isr_register(INTR_TYPE_LINK_CHANGE, link_change_isr)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_linkupStatus_clear()))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    if (RT_ERR_OK != (ret = rtk_intr_linkdownStatus_clear()))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }
    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(INTR_TYPE_LINK_CHANGE)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_imr_set(INTR_TYPE_LINK_CHANGE, ENABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    return ret;
}

static int32 link_change_intr_unregister(void)
{
    int32   ret;

    if (RT_ERR_OK != (ret = rtk_intr_imr_set(INTR_TYPE_LINK_CHANGE, DISABLED)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_irq_isr_unregister(INTR_TYPE_LINK_CHANGE)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    if (RT_ERR_OK != (ret = rtk_intr_imr_restore(gImrLinkChange)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return ret;
    }

    return ret;
}

static uint32 _dyingGaspIntCnt=0;

int32 dying_gasp_isr_counter_get(uint32 *counter)
{
    /* function body */
    *counter = _dyingGaspIntCnt;

    /*reset counter*/
    _dyingGaspIntCnt=0;

    return RT_ERR_OK; 
}

EXPORT_SYMBOL(dying_gasp_isr_counter_get);

static void dying_gasp_isr(void)
{
    int32               ret;
    rtk_enable_t        enable;


    if (RT_ERR_OK != (ret = rtk_intr_ims_get(INTR_TYPE_DYING_GASP, &enable)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }

    // send out the event
    queue_broadcast(MSG_TYPE_DYING_GASP, 0, 0, ENABLED);

    if (RT_ERR_OK != (ret = rtk_intr_ims_clear(INTR_TYPE_DYING_GASP)))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }

    printk(KERN_NOTICE "dying gasp interrupt!\n");
    osal_time_mdelay(100);
    /* reboot ONT self to prevent the situation of low power, but ONT alive */
    if (RT_ERR_OK != (ret = rtk_switch_chip_reset()))
    {
        RT_ERR(ret, (MOD_INTR), "");
        return;
    }


}

#if defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_LUNA_G3_SERIES)

static void gpon_event_state_cb(rtk_gpon_fsm_event_t event, 
    rtk_gpon_fsm_status_t newstatus, rtk_gpon_fsm_status_t oldstatus)
{
    rtk_intr_status_t state;
    
    if (oldstatus != newstatus)
    {
        // send out the event TBD, current rtk_intr_type is fake.
        queue_broadcast(MSG_TYPE_ONU_STATE, newstatus, HAL_GET_PON_PORT(), ENABLED);
    }

    return;
}

static void register_gpon_evt_state(unsigned short dataLen, unsigned char *pRegisterB)
{
    if (1 == *pRegisterB && 
        RT_ERR_OK == rtk_gponapp_evtHdlStateChange_reg(gpon_event_state_cb))
    {
        gIsRegEvtState = 1;
        printk(KERN_ERR 
            "%s: register gpon event state Successfully !\n", __FUNCTION__);

    }
    else
    {
        gIsRegEvtState = 0;
        printk(KERN_ERR
            "%s: register gpon event state failed!!!!\n", __FUNCTION__);
    }
}

#endif

int __init intr_bcaster_init(void)
{

#ifdef USE_GENERIC_NETLINK_INTF
    if (genl_register_family(&gIntrBcasterGenlFamily))
    {
        printk(KERN_ERR
            "%s: register generic netlink family failed!\n", __FUNCTION__);

        goto err_out;
    }

    if (genl_register_mc_group(&gIntrBcasterGenlFamily, &gIntrBcasterGenlMcGrpLinkChange))
    {
        printk(KERN_ERR
            "%s: register generic netlink multicast group for link change failed!\n", __FUNCTION__);

        goto unreg_family;
    }

    if (genl_register_mc_group(&gIntrBcasterGenlFamily, &gIntrBcasterGenlMcGrpDyingGasp))
    {
        printk(KERN_ERR
            "%s: register generic netlink multicast group for dying gasp failed!\n", __FUNCTION__);

        goto unreg_family;
    }
#if !defined(CONFIG_LUNA_G3_SERIES)//link change intr register move to dal_luna_g3_intr_init
    if (RT_ERR_OK != link_change_intr_register())
    {
        printk(KERN_ERR
            "%s: register link change interrupt failed!\n", __FUNCTION__);

        goto unreg_mc_grp;
    }
#endif
#if 0
    if (RT_ERR_OK != common_intr_register(INTR_TYPE_DYING_GASP, dying_gasp_isr, &gImrDyingGasp))
    {
        printk(KERN_ERR
            "%s: register dying gasp interrupt failed!\n", __FUNCTION__);

        goto unreg_mc_grp;
    }
#endif

#else

#if defined(CONFIG_KERNEL_2_6_30)
    pgSocket = netlink_kernel_create(&init_net, INTR_BCASTER_NETLINK_TYPE,
                                        0, NULL, NULL, THIS_MODULE);
#else
    struct netlink_kernel_cfg cfg = {
        .input = NULL,
    };

    pgSocket = netlink_kernel_create(&init_net, INTR_BCASTER_NETLINK_TYPE, &cfg);
#endif 
    if (NULL == pgSocket)
    {
        printk(KERN_ERR
            "%s: create kernel netlink socket failed!\n", __FUNCTION__);

        goto err_out;
    }
#if !defined(CONFIG_LUNA_G3_SERIES) //link change intr register move to dal_luna_g3_intr_init, mark dying gasp intr 
    if (RT_ERR_OK != link_change_intr_register())
    {
        printk(KERN_ERR
            "%s: register link change interrupt failed!\n", __FUNCTION__);

        goto del_sock;
    }

    if (RT_ERR_OK != common_intr_register(INTR_TYPE_DYING_GASP, dying_gasp_isr, &gImrDyingGasp))
    {
        printk(KERN_ERR
            "%s: register dying gasp interrupt failed!\n", __FUNCTION__);

        goto del_sock;
    }
#endif
#endif

#if defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_LUNA_G3_SERIES)

    if (RT_ERR_OK != pkt_redirect_kernelApp_reg(PR_KERNEL_UID_BCASTER, register_gpon_evt_state))
    {
        printk(KERN_ERR
            "%s: register pkt_redirect failed!\n", __FUNCTION__);

        goto del_sock;
    }
#endif

    printk(KERN_NOTICE "interrupt broadcaster init ok!\n");

    return RT_ERR_OK;

#ifdef USE_GENERIC_NETLINK_INTF
unreg_mc_grp:
    genl_unregister_mc_group(&gIntrBcasterGenlFamily, &gIntrBcasterGenlMcGrpDyingGasp);
    genl_unregister_mc_group(&gIntrBcasterGenlFamily, &gIntrBcasterGenlMcGrpLinkChange);

unreg_family:
    genl_unregister_family(&gIntrBcasterGenlFamily);
#else
del_sock:
    netlink_kernel_release(pgSocket);
#endif

err_out:
    return RT_ERR_FAILED;
}

void __exit intr_bcaster_exit(void)
{
#if defined(CONFIG_GPON_FEATURE) && !defined(CONFIG_LUNA_G3_SERIES)

    if(gIsRegEvtState)
    {
        rtk_gponapp_evtHdlStateChange_dereg();
    }
    
    pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_BCASTER);
    
#endif
    
    link_change_intr_unregister();

    common_intr_unregister(INTR_TYPE_DYING_GASP, &gImrDyingGasp);

#ifdef USE_GENERIC_NETLINK_INTF
    genl_unregister_mc_group(&gIntrBcasterGenlFamily, &gIntrBcasterGenlMcGrpDyingGasp);
    genl_unregister_mc_group(&gIntrBcasterGenlFamily, &gIntrBcasterGenlMcGrpLinkChange);

    genl_unregister_family(&gIntrBcasterGenlFamily);
#else
    netlink_kernel_release(pgSocket);
#endif
}

static int intr_bcaster_notifier_dispatch_cb(struct notifier_block  *nb,
                                            unsigned long           action,
                                            void                    *data)
{
    intrBcasterNotifier_t   *pNotifier;

    if (!nb)
        return RT_ERR_FAILED;

    pNotifier = (intrBcasterNotifier_t *)(nb + 1);

    if (pNotifier->notifyType == action && pNotifier->notifierCb)
        pNotifier->notifierCb(data);

    return RT_ERR_OK;
}

int intr_bcaster_notifier_cb_register(intrBcasterNotifier_t     *pRegNotifier)
{
    struct notifier_block   *pNb;
    intrBcasterNotifier_t   *pNotifier;

    if (!pRegNotifier)
        return RT_ERR_FAILED;

    pNb = kmalloc(sizeof(struct notifier_block) +
                    sizeof(intrBcasterNotifier_t), GFP_KERNEL);
    if (!pNb)
        return RT_ERR_FAILED;

    pNotifier = (intrBcasterNotifier_t *)(pNb + 1);

    // fill data
    pNb->notifier_call = intr_bcaster_notifier_dispatch_cb;
    pNotifier->notifierCb = pRegNotifier->notifierCb;
    pNotifier->notifyType = pRegNotifier->notifyType;

    // keep nb addr
    pRegNotifier->pNbAddr = pNb;

    if (blocking_notifier_chain_register(&gNotifierHead, pNb))
    {
        kfree(pNb);

        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int intr_bcaster_notifier_cb_unregister(intrBcasterNotifier_t     *pUnregNotifier)
{
    struct notifier_block   *pNb;

    if (!pUnregNotifier)
        return RT_ERR_FAILED;

    // restore nb addr
    pNb = pUnregNotifier->pNbAddr;
    if (!pNb)
        return RT_ERR_FAILED;

    if (blocking_notifier_chain_unregister(&gNotifierHead, pNb))
        return RT_ERR_FAILED;

    // release nb
    kfree(pNb);

    return RT_ERR_OK;
}

/*
 * Symbol Definition
 */
EXPORT_SYMBOL(intr_bcaster_notifier_cb_register);
EXPORT_SYMBOL(intr_bcaster_notifier_cb_unregister);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek Interrupt Broadcasting Module");
MODULE_AUTHOR("Alvin Wu <alvinwu@realtek.com>");
module_init(intr_bcaster_init);  //TBD have to be called before gpon, re8686 and romedriver, after pkt_redirect
module_exit(intr_bcaster_exit);
