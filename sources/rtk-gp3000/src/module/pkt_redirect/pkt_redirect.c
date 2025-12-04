/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definie the device driver that communication between
 *           kernel space and user space
 *
 * Feature : Provide kernel space APIs and actual data exchange
 *           source
 *
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

#include "../../../include/rtk/switch.h"
#include "../../../include/hal/common/halctrl.h"
#include "../../../include/rtk/switch.h"
#include "../../../include/rtk/ponmac.h"
#if !defined(CONFIG_LUNA_G3_SERIES)
#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif
#endif
#include "pkt_redirect.h"

/* 
 * Symbol Definition 
 */
/* The data base typedef only used in this file */
/* For user space application */
typedef struct pr_userAppDb_s {
    unsigned short uid;        /* User ID */
    int            pid;        /* Process ID */
    unsigned short mtu;        /* Maximum data size */
    struct pr_userAppDb_s *prev;
    struct pr_userAppDb_s *next;
} pr_userAppDb_t;

/* For kernel space application */
typedef struct pr_kernelAppDb_s {
    unsigned short uid;        /* User ID - assigned by kernel/user program itself */
    void (*appCallback)(unsigned short dataLen, unsigned char *data); /* Kernel application call back function */
    struct pr_kernelAppDb_s *prev;
    struct pr_kernelAppDb_s *next;
} pr_kernelAppDb_t;

/*  
 * Data Declaration  
 */
static struct sock *session_sock;
static pr_userAppDb_t *userAppDb;
static pr_kernelAppDb_t *kernelAppDb;

/*  
 * Function Declaration  
 */

/* ------------------------------------------------------------------
 * Device driver kernel part internal APIs
 */
static int pkt_redirect_userApp_add(pr_regUserApp_t *regData)
{
    pr_userAppDb_t *userApp, *searchPtr;

    userApp = (pr_userAppDb_t *) kmalloc(sizeof(pr_userAppDb_t), GFP_KERNEL);
    if(NULL == userApp)
    {
        return -1;
    }

    userApp->uid = regData->ownUid;
    userApp->pid = regData->ownPid;
    userApp->mtu = regData->mtu;
    userApp->prev = NULL;
    userApp->next = NULL;

    if(NULL == userAppDb)
    {
        userAppDb = userApp;
        userApp->prev = NULL;
    }
    else
    {
        searchPtr = userAppDb;
        while(NULL != searchPtr)
        {
            if(searchPtr->uid == regData->ownUid)
            {
                /* Update MTU and PID */
                searchPtr->pid = regData->ownPid;
                searchPtr->mtu = regData->mtu;
                kfree(userApp);

                return 0;
            }
	     if( NULL == searchPtr->next)
	     {
	     	  break;
	     }
            searchPtr = searchPtr->next;
        }
        searchPtr->next = userApp;
        userApp->prev = searchPtr;
        userApp->next = NULL;
    }

    return 0;
}

static pr_userAppDb_t *pkt_redirect_userApp_get(unsigned short uid)
{
    pr_userAppDb_t *searchPtr;

    searchPtr = userAppDb;
    while(NULL != searchPtr)
    {
        if(searchPtr->uid == uid)
        {
            return searchPtr;
        }
        searchPtr = searchPtr->next;
    }

    return NULL;
}

static int pkt_redirect_userApp_del(pr_regUserApp_t *regData)
{
    pr_userAppDb_t *userApp, *searchPtr;

    searchPtr = userAppDb;
    while(NULL != searchPtr)
    {
        if((searchPtr->uid == regData->ownUid) &&
            (searchPtr->pid == regData->ownPid))
        {
            userApp = searchPtr;
            if(searchPtr->prev == NULL && searchPtr->next == NULL)
            {
                userAppDb = NULL;
            }
            else
            {
                if(searchPtr->prev != NULL)
                {
                    searchPtr->prev->next = searchPtr->next;
                }
                if(searchPtr->next != NULL)
                {
                    searchPtr->next->prev = searchPtr->prev;
                }
                if(userAppDb == searchPtr)
                {
                    userAppDb = searchPtr->next;
                }
                userApp->prev = NULL;
                userApp->next = NULL;
            }
            kfree(userApp);

            return 0;
        }
        searchPtr = searchPtr->next;
    }

    return -1;
}

static int pkt_redirect_userApp_delAll(void)
{
    pr_userAppDb_t *userApp, *searchPtr;

    searchPtr = userAppDb;
    while(NULL != searchPtr)
    {
        userApp = searchPtr;
        searchPtr = searchPtr->next;
        kfree(userApp);
    }
    userAppDb = NULL;

    return 0;
}

static int pkt_redirect_kernelApp_add(unsigned short uid, void (*appCallback)(unsigned short dataLen, unsigned char *data))
{
    pr_kernelAppDb_t *kernelApp, *searchPtr;

    kernelApp = (pr_kernelAppDb_t *) kmalloc(sizeof(pr_kernelAppDb_t), GFP_KERNEL);

    if(NULL == kernelApp)
    {
        return -1;
    }

    kernelApp->uid = uid;
    kernelApp->appCallback = appCallback;
    kernelApp->prev = NULL;
    kernelApp->next = NULL;
    if(NULL == kernelAppDb)
    {
        kernelAppDb = kernelApp;
        kernelApp->prev = NULL;
    }
    else
    {
        searchPtr = kernelAppDb;
        while(NULL != searchPtr)
        {
            if(searchPtr->uid == uid)
            {
                /* Update callback function only */
                searchPtr->appCallback = appCallback;
                kfree(kernelApp);

                return 0;
            }
	     if( NULL == searchPtr->next)
	     {
	     	  break;
	     }
            searchPtr = searchPtr->next;
        }
        searchPtr->next = kernelApp;
        kernelApp->prev = searchPtr;
        kernelApp->next = NULL;
    }

    return 0;
}

static pr_kernelAppDb_t *pkt_redirect_kernelApp_get(unsigned short uid)
{
    pr_kernelAppDb_t *searchPtr;

    searchPtr = kernelAppDb;
    while(NULL != searchPtr)
    {
        if(searchPtr->uid == uid)
        {
            return searchPtr;
        }
        searchPtr = searchPtr->next;
    }

    return NULL;
}

static int pkt_redirect_kernelApp_del(unsigned short uid)
{
    pr_kernelAppDb_t *kernelApp, *searchPtr;

    searchPtr = kernelAppDb;
    while(NULL != searchPtr)
    {
        if(searchPtr->uid == uid)
        {
            kernelApp = searchPtr;
            if(searchPtr->prev == NULL && searchPtr->next == NULL)
            {
                kernelAppDb = NULL;
            }
            else
            {
                if(searchPtr->prev != NULL)
                {
                    searchPtr->prev->next = searchPtr->next;
                }
                if(searchPtr->next != NULL)
                {
                    searchPtr->next->prev = searchPtr->prev;
                }
                if(kernelAppDb == searchPtr)
                {
                    kernelAppDb = searchPtr->next;
                }
                kernelApp->prev = NULL;
                kernelApp->next = NULL;
            }
            kfree(kernelApp);

            return 0;
        }
        searchPtr = searchPtr->next;
    }

    return -1;
}

static int pkt_redirect_kernelApp_delAll(void)
{
    pr_kernelAppDb_t *kernelApp, *searchPtr;

    searchPtr = kernelAppDb;
    while(NULL != searchPtr)
    {
        kernelApp = searchPtr;
        searchPtr = searchPtr->next;
        kfree(kernelApp);
    }
    kernelAppDb = NULL;

    return 0;
}

static int pkt_redirect_send2User(
    pr_userAppDb_t *userApp,
    unsigned short dataLen,
    unsigned char *payload)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int len;
    void *data;
    int ret;

    if((NULL == userApp) || (NULL == payload))
    {
        return -1;
    }

    len = NLMSG_SPACE(dataLen);
    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb)
    {
        return -3;
    }
#if defined(CONFIG_KERNEL_2_6_30)
    nlh = NLMSG_PUT(skb, userApp->pid, 0, 0, dataLen);
#else
	nlh = nlmsg_put(skb, userApp->pid, 0, 0, dataLen,0);
    
    if(nlh==NULL)
    {
        if (skb)
        {
            kfree_skb(skb);
        }
        return -2;        
    }
#endif
    nlh->nlmsg_flags = 0;
    data = NLMSG_DATA(nlh);
    memcpy(data, payload, dataLen);
#if defined(CONFIG_KERNEL_2_6_30)	
    NETLINK_CB(skb).pid = 0; /* from kernel */ 
#else
	NETLINK_CB(skb).portid = 0; /* from kernel */ 
#endif
    NETLINK_CB(skb).dst_group = 0; /* unicast */

    ret = netlink_unicast(session_sock, skb, userApp->pid, MSG_DONTWAIT);
    if (ret < 0) 
    { 
        printk("send failed %d\n", ret);
        return -4;
    } 
    return 0;
     
#if defined(CONFIG_KERNEL_2_6_30)     
    nlmsg_failure: /* Used by NLMSG_PUT */ 
    if (skb)
    {
        kfree_skb(skb);
    }
    return -5;
#endif
    
}

static void pkt_redirect_recv(struct sk_buff *skb)
{
    int ret;
    u_int uid, pid, seq, sid;
    unsigned int dataLen;
    void *data;
    struct nlmsghdr *nlh;
    pr_identifier_t *prId;
    pr_regUserApp_t *regData;
    pr_data_t *sendData;

    nlh = (struct nlmsghdr *)skb->data;
    pid = NETLINK_CREDS(skb)->pid;
#if defined(CONFIG_KERNEL_2_6_30)
    uid = NETLINK_CREDS(skb)->uid;
    sid = NETLINK_CB(skb).sid;
#else
	uid = NETLINK_CREDS(skb)->uid.val;
#endif
    seq = nlh->nlmsg_seq;
    data = NLMSG_DATA(nlh);
    dataLen = nlh->nlmsg_len;
    //printk("recv skb from user space uid:%d pid:%d seq:%d,sid:%d\n", uid, pid, seq, sid);

    prId = (pr_identifier_t *) data;
    //printk("OpType:%u pid:%d uid:%d\n", prId->prOpType, prId->pid, prId->uid);

    /* Check the packet prOpType to decide operation */
    switch(prId->prOpType)
    {
    case PKT_REDIRECT_OPTYPE_SEND_MSG:
        sendData = (pr_data_t *) data;
        //printk("PKT_REDIRECT_OPTYPE_SEND\n");
        //printk("uid: %u\n", sendData->uid);
        //printk("isUser: %d\n", sendData->isUser);
        //printk("flag: %u\n", sendData->flag);
        //printk("len: %u\n", sendData->dataLen);

        pkt_redirect_kernelApp_sendPkt(sendData->uid, sendData->isUser, sendData->dataLen, PR_DATA(sendData));
        break;
    case PKT_REDIRECT_OPTYPE_REGISTER:
        /* Register new */
        regData = (pr_regUserApp_t *) data;
        if(PKT_REDIRECT_REGTYPE_REGISTER == regData->regType)
        {
            ret = pkt_redirect_userApp_add(regData);
            //printk("PKT_REDIRECT_REGTYPE_REGISTER: %d\n", ret);
        }
        else if(PKT_REDIRECT_REGTYPE_DEREGISTER == regData->regType)
        {
            ret = pkt_redirect_userApp_del(regData);
            //printk("PKT_REDIRECT_REGTYPE_DEREGISTER: %d\n", ret);
        }

#if 0
        {
            pr_userAppDb_t *userApp, *searchPtru;
            pr_kernelAppDb_t *kernelApp, *searchPtrk;

            searchPtru = userAppDb;
            printk("userApp\n");
            while(NULL != searchPtru)
            {
                userApp = searchPtru;
                searchPtru = searchPtru->next;
                printk("uid: %u\n", userApp->uid);
                printk("pid: %d\n", userApp->pid);
                printk("mtu: %u\n", userApp->mtu);
            }

            searchPtrk = kernelAppDb;
            printk("kernelApp\n");
            while(NULL != searchPtrk)
            {
                kernelApp = searchPtrk;
                searchPtrk = searchPtrk->next;
                printk("uid: %u\n", kernelApp->uid);
                printk("CB: 0x%08x\n", (unsigned int) kernelApp->appCallback);
            }
        }
#endif
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------
 * Kernel space APIs for other kernel modules
 */
int pkt_redirect_kernelApp_reg(
    unsigned short uid,
    void (*appCallback)(unsigned short dataLen, unsigned char *data))
{
    return pkt_redirect_kernelApp_add(uid, appCallback);
}

int pkt_redirect_kernelApp_dereg(
    unsigned short uid)
{
    return pkt_redirect_kernelApp_del(uid);
}

/* Sender have to prepare the data and free after send */
int pkt_redirect_kernelApp_sendPkt(
    unsigned short dstUid,
    int            isUser,
    unsigned short dataLen,
    unsigned char *data)
{
    pr_kernelAppDb_t *kernelApp;
    pr_userAppDb_t *userApp;

    if(0 == isUser)
    {
        /* Kernel app -> kernel app */
        kernelApp = pkt_redirect_kernelApp_get(dstUid);
        if(NULL == kernelApp)
        {
            /* No such UID registered */
            return -1;
        }
        kernelApp->appCallback(dataLen, data);
    }
    else
    {
        /* Kernel app -> user app */
        userApp = pkt_redirect_userApp_get(dstUid);
        if(NULL == userApp)
        {
            /* No such UID/PID registered */
            return -1;
        }
        if(dataLen > userApp->mtu)
        {
            /* User app can't receive such large amount of data */
            return -2;
        }
        pkt_redirect_send2User(userApp, dataLen, data);
    }

    return 0;
}

/*for debug oam please change to 0x3F*/


int __init pkt_redirect_init(void)
{
    /* Create netlink for communicate between kernel and user protocol */
#if defined(CONFIG_KERNEL_2_6_30)
    session_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, 0, pkt_redirect_recv, NULL, THIS_MODULE);
#else	
	struct netlink_kernel_cfg cfg = {
	    .input = pkt_redirect_recv,
	};

	session_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, &cfg);
#endif
    pkt_redirect_userApp_delAll();
    pkt_redirect_kernelApp_delAll();
    userAppDb = NULL;
    kernelAppDb = NULL;
    if(NULL == session_sock)
    {
        printk("pkt_redirect_init failed!\n");
    }
    else
    {
        printk("pkt_redirect_init complete\n");
    }

    return 0;
}

void __exit pkt_redirect_exit(void)
{
    pkt_redirect_userApp_delAll();
    pkt_redirect_kernelApp_delAll();
    sock_release(session_sock->sk_socket);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek kernel/user packet redirect module");
MODULE_AUTHOR("Realtek");
EXPORT_SYMBOL(pkt_redirect_kernelApp_reg);
EXPORT_SYMBOL(pkt_redirect_kernelApp_dereg);
EXPORT_SYMBOL(pkt_redirect_kernelApp_sendPkt);

module_init(pkt_redirect_init);
module_exit(pkt_redirect_exit);

