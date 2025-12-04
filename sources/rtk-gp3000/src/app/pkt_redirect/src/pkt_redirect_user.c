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
 * Purpose : 
 *
 * Feature : 
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/socket.h> 
#include <linux/netlink.h> 
#include <errno.h>

#include "pkt_redirect_user.h"
#include <sys/syscall.h>

/*  
 * Function Declaration  
 */ 

/* ------------------------------------------------------------------
 * Device driver user space part internal APIs
 */
static int pkt_redirect_appMsg_send(
    int sock_fd,
    struct nlmsghdr *nlh)
{
    int ret;
    struct sockaddr_nl dest_addr;
    struct msghdr msg;
    struct iovec iov;

    memset(&msg,0,sizeof(struct msghdr));
    memset(&iov,0,sizeof(struct iovec));
    memset((unsigned char *)&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;    /* For Linux Kernel */ 
    dest_addr.nl_groups = 0; /* unicast */ 

    iov.iov_base = (void *) nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *) &dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    ret = sendmsg(sock_fd, &msg, 0);
    if(ret < 0)
    {
        printf("ret %d\n", ret);
        printf("Error sendmsg: %d %s\n", errno, strerror( errno ));
        return -2;
    }

    return ret;
}

/* ------------------------------------------------------------------
 * User space APIs
 */
int ptk_redirect_userApp_reg(
    int sock_fd,
    unsigned short uid,
    unsigned short mtu)
{
    int ret;
    //int pid = getpid();
    int pid = (int)syscall(SYS_gettid);
    struct nlmsghdr *nlh = NULL;
    pr_regUserApp_t *prRegData;
    struct sockaddr_nl nladdr;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(pr_regUserApp_t)));
    if(NULL == nlh)
    {
        return -1;
    }

    //bind thread id to sock_fd
    nladdr.nl_family = PF_NETLINK;
    nladdr.nl_pad = 0;
    nladdr.nl_pid = pid;
    nladdr.nl_groups =0;
    ret = bind(sock_fd, (struct sockaddr*)&nladdr, sizeof(nladdr));
    if(ret<0)
    {
        printf("%s %d Error on binding\n",__FUNCTION__,__LINE__);    
    }

    /* Fill the netlink message header */ 
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(pr_regUserApp_t));
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 0;

    /* Fill in the netlink message payload */ 
    prRegData = (pr_regUserApp_t *) NLMSG_DATA(nlh);
    prRegData->prOpType = PKT_REDIRECT_OPTYPE_REGISTER;
    prRegData->ownUid = uid;
    prRegData->ownPid = pid;
    prRegData->mtu = mtu;
    prRegData->regType = PKT_REDIRECT_REGTYPE_REGISTER;

    ret = pkt_redirect_appMsg_send(sock_fd, nlh);
    free(nlh);

    return ret;
}

int ptk_redirect_userApp_dereg(
    int sock_fd,
    unsigned short uid)
{
    int ret;
    //int pid = getpid();
	int pid = (int)syscall(SYS_gettid);
    struct nlmsghdr *nlh = NULL;
    pr_regUserApp_t *prRegData;
    
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(pr_regUserApp_t)));
    if(NULL == nlh)
    {
        return -1;
    }

    /* Fill the netlink message header */ 
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(pr_regUserApp_t));
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 0;

    /* Fill in the netlink message payload */ 
    prRegData = (pr_regUserApp_t *) NLMSG_DATA(nlh);
    prRegData->prOpType = PKT_REDIRECT_OPTYPE_REGISTER;
    prRegData->ownUid = uid;
    prRegData->ownPid = pid;
    prRegData->regType = PKT_REDIRECT_REGTYPE_DEREGISTER;
    /* Don't care fields */
    prRegData->mtu = 0;

    ret = pkt_redirect_appMsg_send(sock_fd, nlh);
    free(nlh);

    return ret;
}

int ptk_redirect_userApp_sendPkt(
    int sock_fd,
    unsigned short uid,
    unsigned int   flag,
    unsigned short dataLen,
    unsigned char *data)
{
    int ret;
    int netlinkLen;
    //int pid = getpid();
    int pid = (int)syscall(SYS_gettid);
    struct nlmsghdr *nlh = NULL;
    pr_data_t *prData;

    netlinkLen = NLMSG_SPACE(sizeof(pr_data_t) + dataLen);
    nlh = (struct nlmsghdr *)malloc(netlinkLen);
    if(NULL == nlh)
    {
        return -1;
    }

    /* Fill the netlink message header */ 
    nlh->nlmsg_len = netlinkLen;
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 0;

    /* Fill in the netlink message payload */ 
    prData = (pr_data_t *) NLMSG_DATA(nlh);
    prData->prOpType = PKT_REDIRECT_OPTYPE_SEND_MSG;
    prData->uid = uid;
    prData->isUser = 0; /* User app can only send to kernel module */
    prData->flag = flag;
    prData->dataLen = dataLen;
    memcpy(PR_DATA(prData), data, dataLen);

    ret = pkt_redirect_appMsg_send(sock_fd, nlh);
    free(nlh);

    return ret;
}

int ptk_redirect_userApp_recvPkt(
    int sock_fd,
    unsigned short mtu,
    unsigned short *dataLen,
    unsigned char *payload)
{
    int ret;
    int netlinkLen;
    //int pid = getpid();
    int pid = (int)syscall(SYS_gettid);
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;
    unsigned char *recvData;

    netlinkLen = NLMSG_SPACE(mtu);
    nlh = (struct nlmsghdr *)malloc(netlinkLen);
    if(NULL == nlh)
    {
        return -1;
    }

    /* Clear all structures */
    memset(nlh, 0x0, sizeof(netlinkLen));
    memset(&dest_addr, 0x0, sizeof(struct sockaddr_nl));
    memset(&iov, 0x0, sizeof(struct iovec));
    memset(&msg, 0x0, sizeof(struct msghdr));

    /* Fill the netlink message header */ 
    nlh->nlmsg_len = netlinkLen;
    nlh->nlmsg_pid = pid;
    nlh->nlmsg_flags = 0;

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    ret = recvmsg(sock_fd, &msg, 0);
    if((ret > 0) && NLMSG_OK(nlh, ret) && (nlh->nlmsg_len > NLMSG_HDRLEN))
    {
        recvData = NLMSG_DATA(nlh);
        memcpy(payload, recvData, nlh->nlmsg_len - NLMSG_HDRLEN);
        *dataLen = (nlh->nlmsg_len - NLMSG_HDRLEN);
    }
    free(nlh);

    return ret;
}

