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
 *
 *
 * $Revision: 40672 $
 * $Date: 2013-07-02 10:10:54 +0800 (Tue, 02 Jul 2013) $
 *
 * Purpose : Fiber mode init
 *
 * Feature : Initial for fiber access
 *
 */

#ifdef CONFIG_DYING_GASP_FEATURE

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/sock.h>
#include <linux/netdevice.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <rtk/qos.h>
#include <rtk/ponmac.h>
#include <rtk/irq.h>
#if 0
#include <module/intr_bcaster/intr_bcaster.h>
#endif

#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif

/*
 * Symbol Definition
 */
#define EPON_OAM_FLAG_DYING_GASP            0x0002
#define EPON_OAM_FLAG_CRITICAL_EVENT        0x0004
#define EPON_OAMPDU_CODE_INFO               0x00
#define EPON_INFO_OAMPDU_TYPE_LOCAL         0x01
#define EPON_INFO_OAMPDU_INFO_LEN           0x10


/*
 * Data Declaration
 */
static unsigned int rtl960x_fiber_port_mask;
static struct proc_dir_entry *dyinggasp_proc_dir = NULL;
static struct proc_dir_entry *dyinggasp_entry = NULL;
static unsigned int dyinggasp_packet_num=10;
static unsigned int use_hw_dyinggasp=0;

static rtk_ponmac_queue_t dyinggasp_que;
static rtk_ponmac_queueCfg_t dyinggasp_que_cfg;

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);
static int _dyinggasp_packet_prepare(unsigned int pkt_cnt);
static int32 _dyinggasp_handler(void);

static int32 _dyinggasp_handler(void)
{
    if(use_hw_dyinggasp)
    {
        rtk_ponmac_queue_add(&dyinggasp_que, &dyinggasp_que_cfg);
    }
    else
    {
        _dyinggasp_packet_prepare(dyinggasp_packet_num);
    }

    printk("dyingpasp_handler: send %d pkts\n\r", dyinggasp_packet_num);

    return 0;

    //rtk_switch_chip_reset();

}

static int _dyinggasp_find_eth_addr(unsigned char *pMac)
{
    struct socket * sock = NULL;
    struct net_device *dev = NULL;
    struct net *net = NULL;
    int ret;

    ret = sock_create(AF_INET, SOCK_STREAM, 0, &sock);
    if((net = sock_net(sock->sk)) == NULL)
    {
        printk("\n\r sock net fail!");
        return -1;
    }

    if ((dev = dev_get_by_name(net,"nas0_0")) != NULL)
    {
        memcpy(pMac,dev->dev_addr,6);
    }
    else
    {
        printk("\n\r get dev fail!");
        return -1;
    }

    sock_release(sock);

    return ret;
}

static int
_dyinggasp_packet_prepare(unsigned int pkt_cnt)
{
    struct tx_info txInfo;
    unsigned short data_len=60;
    unsigned char data[data_len];
    unsigned char *pPtr;
    const unsigned char oamDstMac[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};
    unsigned char oamSrcMac[6];
    unsigned short oamPduFlag;
    int i;

    /* dying gasp packet content */
    memset(data, 0x0, data_len);
    pPtr = &data[0];
    /* Add standard header */
    memcpy(pPtr, oamDstMac, sizeof(oamDstMac));
    pPtr += sizeof(oamDstMac);

    _dyinggasp_find_eth_addr(oamSrcMac);
    memcpy(pPtr, oamSrcMac, sizeof(oamSrcMac));
    pPtr += sizeof(oamSrcMac);

    /* EtherType */
    *pPtr = 0x88;
    pPtr ++;
    *pPtr = 0x09;
    pPtr ++;
    /* SubType */
    *pPtr = 0x03;
    pPtr ++;
    /* Flag: Critical event and Dying gasp bit */
    oamPduFlag = EPON_OAM_FLAG_CRITICAL_EVENT | EPON_OAM_FLAG_DYING_GASP;
    *pPtr = ((unsigned char *)(&oamPduFlag))[0];
    pPtr ++;
    *pPtr = ((unsigned char *)(&oamPduFlag))[1];
    pPtr ++;
    /* Code: Information */
    *pPtr = EPON_OAMPDU_CODE_INFO;
    pPtr ++;
    /* Type: Local */
    *pPtr = EPON_INFO_OAMPDU_TYPE_LOCAL;
    pPtr ++;
    /* Length: 0x10 */
    *pPtr = EPON_INFO_OAMPDU_INFO_LEN;
    pPtr ++;
    /* OAM version: 0x01 */
    *pPtr = 0x01;
    pPtr ++;
    /* Revision:0x1010 */
    *pPtr = 0x10;
    pPtr ++;
    *pPtr = 0x10;
    pPtr ++;
    /* State: Forwarding 0x0 */
    *pPtr = 0x0;
    pPtr ++;
    /* OAM configuration 0x0 */
    *pPtr = 0x0;
    pPtr ++;
    /* MAX PDU size 0x0110 */
    *pPtr = 0x01;
    pPtr ++;
    *pPtr = 0x10;
    pPtr ++;

    /* dying gasp packet send to queue 31 */
    memset(&txInfo,0x0,sizeof(struct tx_info));
    /*txInfo.opts1.bit.cputag_psel = 1;*/
    //txInfo.opts1.bit.keep = 1;
    txInfo.opts2.bit.cputag = 1;
    txInfo.opts2.bit.aspri = 1;
    txInfo.opts2.bit.cputag_pri = 7;
    /*txInfo.opts3.bit.tx_dst_stream_id = 0x1f;*/
    txInfo.opts3.bit.tx_portmask = rtl960x_fiber_port_mask;

    for(i=0; i< pkt_cnt; i++)
    {
        re8686_tx_with_Info(data, data_len, &txInfo);
    }

    return 0;
}

static int dyinggasp_ctrl_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "use_hw_dyinggasp=%d, dyinggasp_packet_num=%d\n", use_hw_dyinggasp, dyinggasp_packet_num);

	return len;
}

static int dyinggasp_ctrl_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;
	char *strptr, *method_ptr, *cnt_ptr;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
        tmpBuf[len] = '\0';
        strptr=tmpBuf;
		method_ptr = strsep(&strptr," ");
		if (method_ptr==NULL)
			goto errout;

        if (memcmp(method_ptr, "hw", 2)==0)
            use_hw_dyinggasp=1;
        else
            use_hw_dyinggasp=0;

        cnt_ptr = strsep(&strptr," ");
		if (cnt_ptr==NULL)
			goto errout;

		dyinggasp_packet_num = simple_strtoul(cnt_ptr, NULL, 10);
		printk("write use_hw_dyinggasp=%d, dyinggasp_packet_num=%d\n", use_hw_dyinggasp, dyinggasp_packet_num);
        /* if packet_num > 0, it use queu block method to send dying gasp */
        if(use_hw_dyinggasp)
        {
            rtk_ponmac_queue_del(&dyinggasp_que);
            _dyinggasp_packet_prepare(dyinggasp_packet_num);
        }
        else /* if packet_num = 0, use pure SW to send out packet */
        {
            rtk_ponmac_queue_add(&dyinggasp_que, &dyinggasp_que_cfg);
        }

        return count;
	}
    else
    {
errout:
        printk("format:	\"sw 100\"\n");
        printk("format:	\"hw 100\"\n");
    }

	return -EFAULT;
}

int __init dyinggasp_init(void)
{
    rtk_portmask_t portMask;
#if 0
    intrBcasterNotifier_t dyinggasp_Notifier;
#endif
    int fiber_port;
    int ret;

    /* Create proc dyinggasp commands */
    if(NULL == dyinggasp_proc_dir)
    {
        dyinggasp_proc_dir = proc_mkdir("dyinggasp", NULL);
    }
    if(dyinggasp_proc_dir)
    {
    	dyinggasp_entry = create_proc_entry("dyinggasp_ctrl", 0, dyinggasp_proc_dir);
    	if(dyinggasp_entry){
    		dyinggasp_entry->read_proc = dyinggasp_ctrl_read;
    		dyinggasp_entry->write_proc = dyinggasp_ctrl_write;
    	}
    }

    rtk_switch_portMask_Clear(&portMask);
    rtk_switch_port2PortMask_set(&portMask,RTK_PORT_FIBER);
    rtl960x_fiber_port_mask = portMask.bits[0];

#if 0
    memset(&dyinggasp_Notifier, 0x0, sizeof(intrBcasterNotifier_t));
    dyinggasp_Notifier.notifyType = INTR_TYPE_DYING_GASP;
    dyinggasp_Notifier.notifierCb = _dyinggasp_handler;
    /* register fiber dying gasp interrupt handler */
    if((ret = intr_bcaster_notifier_cb_register(&dyinggasp_Notifier)) != RT_ERR_OK)
    {
        printk("%s, %d: intr_bcaster_notifier_cb_register fail 0x%x!\n", __FILE__, __LINE__, ret);
        return 0;
    }
#endif
    /*register dying gasp isr for top half*/
	if((ret = rtk_irq_isrDgTh_register(_dyinggasp_handler)) != RT_ERR_OK)
	{
		printk("%s, %d: rtk_irq_isrDgTh_register fail 0x%x!\n", __FILE__, __LINE__, ret);
		return ret;
	}

    /* set qos priority to queue mapping to table 0 in port 4 */
    if((ret = rtk_switch_phyPortId_get(RTK_PORT_FIBER,&fiber_port)) != RT_ERR_OK)
    {
        printk("%s, %d: rtk_switch_phyPortId_get fail 0x%x!\n", __FILE__, __LINE__, ret);
        return 0;
    }

    /* set the internal priority to queue mapping */
    if ((ret = rtk_qos_portPriMap_set(fiber_port, 0)) != RT_ERR_OK)
    {
        printk("%s, %d: rtk_switch_phyPortId_get fail 0x%x!\n", __FILE__, __LINE__, ret);
        return 0;
    }

    /* set queue 7 no scheduling for not send out */
    dyinggasp_que.schedulerId = 0;
    dyinggasp_que.queueId = 7;
    rtk_ponmac_queue_get(&dyinggasp_que, &dyinggasp_que_cfg);

    printk("dyinggasp init complete!\n");

    return 0;
}

void __exit dyinggasp_exit(void)
{
    /* Remove proc debug commands */
    if(dyinggasp_entry)
    {
    	remove_proc_entry("dyinggasp_ctrl", dyinggasp_proc_dir);
        dyinggasp_entry = NULL;
    }
    if(dyinggasp_proc_dir)
    {
        remove_proc_entry("dyinggasp", NULL);
        dyinggasp_proc_dir = NULL;
    }
    return;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek dying_gasp module");
MODULE_AUTHOR("Scott Lin <scottlin@realtek.com>");
module_init(dyinggasp_init);
module_exit(dyinggasp_exit);

#endif /* CONFIG_DYING_GASP_FEATURE */
