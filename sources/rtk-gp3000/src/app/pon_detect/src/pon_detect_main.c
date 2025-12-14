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
 * $Revision: 40647 $
 * $Date: 2013-07-01 15:36:16 +0800 (Mon, 01 Jul 2013) $
 *
 * Purpose : Main function of the EPON OAM protocol stack user application
 *           It create two additional threads for packet Rx and state control
 *
 * Feature : Start point of the EPON OAM protocol stack. Use individual threads
 *           for packet Rx and state control
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include <common/error.h>
#include <rtk/ponmac.h>
#if defined(CONFIG_PON_AUTO_SELECT_FEATURE)
#include <LINUX/mib.h>
#endif
#include <rtk/rt/rt_ponmisc.h>
#include <rtk/rt/rt_gpon.h>
#include <pkt_redirect_user.h>
#include <module/intr_bcaster/intr_bcaster.h>

#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#include <common/util/rt_util.h>
#include <rtk/utility.h>
#endif

/*
 * Symbol Definition
 */
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
typedef enum{
    RX_NORMAL,
    RX_LOW,
    RX_HIGH,
}rx_pow_state_t;
#endif

/*
 * Data Declaration
 */
rt_ponmisc_ponMode_t ponMode = RT_PONMODE_END;
rt_ponmisc_ponSpeed_t ponSpeed = RT_PONSPEED_END;
rt_ponmisc_ponMode_t swap_ponMode = RT_PONMODE_END;
rt_ponmisc_ponSpeed_t swap_ponSpeed = RT_PONSPEED_END;
unsigned int set_mode, set_speed, set_sub_mode=0, set_pon_led_spec=0;
unsigned int ctrl_flag = 0;
unsigned int gpon_mode_flag = 0;
unsigned int epon_mode_flag = 0;
unsigned int ether_mode_flag = 0;


/*
 * Macro Definition
 */
#define RESET_CDR_TIMES  3
#define RX_LOS_SWAP_TIMES  15
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#define GPON_LOG_INTERVAL 60
#endif

#define UINT_MSB	0x80000000
#define UINT_LSB	0x1

#define CTRL_FLAG_AUTO_SELECT_ENABLE			1	//enable auto select
#define CTRL_FLAG_SPEED_SELECT_INCREASE		2	//default decrease: 10G -> 2.5G -> 1G -> 100M, by the mode flag order
#define CTRL_FLAG_MODE_SELECT_REORDER		4	//default GPON->EPON->ETH, if reorder=1: GPON->ETH->EPON
#define CTRL_FLAG_RX_LOS_DETECT_ENABLE		8	//enable detect under RX LOS: only support GPON+XGPON or GPON+XGSPON

#define PON_MODE_FAMILY_GPON					1
#define PON_MODE_FAMILY_EPON					2
#define PON_MODE_FAMILY_ETHERNET				3

#define GPON_FAMILY_MODE_NONE					0
#define GPON_FAMILY_MODE_GPON				1
#define GPON_FAMILY_MODE_GPON_US_2DOT5G	2
#define GPON_FAMILY_MODE_XGPON				4
#define GPON_FAMILY_MODE_XGSPON				8

#define EPON_FAMILY_MODE_NONE					0
#define EPON_FAMILY_MODE_EPON					1
#define EPON_FAMILY_MODE_XGEPON_US_1G		2
#define EPON_FAMILY_MODE_XGEPON_US_10G		4

#define ETHERNET_FAMILY_MODE_NONE			0
#define ETHERNET_FAMILY_MODE_100BASEX		1	//Fiber-100
#define ETHERNET_FAMILY_MODE_1000BASEX		2	//Fiber-1000
#define ETHERNET_FAMILY_MODE_2500BASEX		4
#define ETHERNET_FAMILY_MODE_SGMII			8
#define ETHERNET_FAMILY_MODE_HISGMII			16
#define ETHERNET_FAMILY_MODE_USXGMII			32
#define ETHERNET_FAMILY_MODE_SFI_XFI			64

/*
 * Function Declaration
 */
int opticalPowerRange(int *low, int *high)
{
    FILE* fp=NULL;
    char tmpBuf[128];

    if((fp=fopen("/proc/exception", "r"))!=NULL)
    {
        while(fgets(tmpBuf, sizeof(tmpBuf), fp) != NULL){
            if(strstr(tmpBuf,"opticalPowerRange"))
            {
                //printf("tmpBuf %s\n",tmpBuf);
                if(sscanf(tmpBuf,"opticalPowerRange:%d,%d",low,high)!= -1){
                    //printf("low=%d high=%d\n",*low,*high);
                }
                else{
                    //printf("sscanf fail!\n");
                }
            }
        }
    }
    else
    {
        //printf("fopen fail!\n");
        return -1;
    }
        
    fclose(fp);
    return 0;
}

#if defined(CONFIG_PON_DETECT_FEATURE)
int
pon_mode_switch(void)
{
    int i, ret = 0;
    unsigned int current_speed_flag=0, current_mode=0, mode_switch=0, supported_speed_flag=0;
    unsigned int next_speed_flag=0, next_mode=0, next_mode_flags=0;
    unsigned int overflow_mask=0;

    //only support GPON&EPON in 1st stage
    if(ponMode == RT_GPON_MODE)
    {
        current_mode = PON_MODE_FAMILY_GPON;
        supported_speed_flag = gpon_mode_flag;
        if(ponSpeed == RT_1G25G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_GPON;
        else if(ponSpeed == RT_BOTH2DOT5G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_GPON_US_2DOT5G;
        else if(ponSpeed == RT_DN10G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_XGPON;
        else if(ponSpeed == RT_BOTH10G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_XGSPON;
        else {
            printf("pon auto detect: unsupported GPON speed %d\r\n", ponSpeed);
            return -1;
        }
    }
    else if(ponMode == RT_EPON_MODE)
    {
        current_mode = PON_MODE_FAMILY_EPON;
        supported_speed_flag = epon_mode_flag;
        if(ponSpeed == RT_1G25G_SPEED)
            current_speed_flag = EPON_FAMILY_MODE_EPON;
        else if(ponSpeed == RT_DN10G_SPEED)
            current_speed_flag = EPON_FAMILY_MODE_XGEPON_US_1G;
        else if(ponSpeed == RT_BOTH10G_SPEED)
            current_speed_flag = EPON_FAMILY_MODE_XGEPON_US_10G;
        else {
            printf("pon auto detect: unsupported EPON speed %d\r\n", ponSpeed);
            return -1;
        }
    }
    else
    {
        //TBD: NG-PON2, Ethernet or other modes
        printf("pon auto detect: unsupported PON mode %d\r\n", ponMode);
        return -1;
    }

    //checking for next speed
    if(supported_speed_flag & (~current_speed_flag))
    {
        if(CTRL_FLAG_SPEED_SELECT_INCREASE & ctrl_flag)
        {
            for(i=1;i<32;i++)
            {
                if(current_speed_flag == UINT_MSB)
                {
                    mode_switch = 1;
                    break;
                }
                overflow_mask=(0xFFFFFFFF >> i);
                if(((unsigned int)((current_speed_flag & overflow_mask) << i)) & supported_speed_flag)
                {
                    next_speed_flag = ((current_speed_flag & overflow_mask) << i);
                    next_mode = current_mode;
                    break;
                }
                if(((current_speed_flag & overflow_mask) <<i) == UINT_MSB)
                {
                    mode_switch = 1;
                    break;
                }
            }
        }
        else
        {
            for(i=1;i<32;i++)
            {
                if(current_speed_flag == UINT_LSB)
                {
                    mode_switch = 1;
                    break;
                }
                if((current_speed_flag >> i) & supported_speed_flag)
                {
                    next_speed_flag = (current_speed_flag >> i);
                    next_mode = current_mode;
                    break;
                }
                if((current_speed_flag >>i) == UINT_LSB)
                {
                   mode_switch = 1;
                    break;
                }
            }
        }
    }
    else
    {
        mode_switch = 1;
    }

    //swithing mode, only support GPON&EPON in 1st stage
    if(mode_switch)
    {
        if((current_mode == PON_MODE_FAMILY_GPON)&&(epon_mode_flag))
        {
            next_mode = PON_MODE_FAMILY_EPON;
            next_mode_flags = epon_mode_flag;
        }
        else if((current_mode == PON_MODE_FAMILY_GPON)&&(epon_mode_flag==0)&&((supported_speed_flag & (~current_speed_flag))))
        {  //no mode switch, but restart speed rotate
            next_mode = PON_MODE_FAMILY_GPON;
            next_mode_flags = gpon_mode_flag;
        }
        else if((current_mode == PON_MODE_FAMILY_EPON)&&(gpon_mode_flag))
        {
            next_mode = PON_MODE_FAMILY_GPON;
            next_mode_flags = gpon_mode_flag;
        }
        else if((current_mode == PON_MODE_FAMILY_EPON)&&(gpon_mode_flag==0)&&((supported_speed_flag & (~current_speed_flag))))
        {  //no mode switch, but restart speed rotate
            next_mode = PON_MODE_FAMILY_EPON;
            next_mode_flags = epon_mode_flag;
        }
        else
        {
            printf("pon auto detect: unexpected mode switch\r\n");
            return -1;
        }

        //speed decision
        if(ctrl_flag & CTRL_FLAG_SPEED_SELECT_INCREASE)
        {
            for(i=0;i<32;i++)
            {
                if((UINT_LSB << i) & next_mode_flags)
                {
                    next_speed_flag = (UINT_LSB << i);
                    break;
                }
            }
        }
        else
        {
            for(i=0;i<32;i++)
            {
                if((UINT_MSB >> i) & next_mode_flags)
                {
                    next_speed_flag = (UINT_MSB >> i);
                    break;
                }
            }
        }
    }

    //setup next PON_MODE PON_SPEED PON_SUB_MODE for MIB
    if(next_mode == PON_MODE_FAMILY_GPON)
    {
        set_mode = 1;
        set_sub_mode = 0;
        set_pon_led_spec = 0;
        switch (next_speed_flag)
        {
            case  GPON_FAMILY_MODE_GPON:
                set_speed = 0;
                break;
            case  GPON_FAMILY_MODE_GPON_US_2DOT5G:
                set_speed = 3;
                break;
            case  GPON_FAMILY_MODE_XGPON:
                set_speed = 1;
                break;
            case  GPON_FAMILY_MODE_XGSPON:
                set_speed = 2;
                break;
            default:
                printf("pon auto detect: unexpected mode to switch 0x%x\r\n", next_speed_flag);
                return -1;
                break;
        }
    }
    else if(next_mode == PON_MODE_FAMILY_EPON)
    {
        set_mode = 2;
        set_sub_mode = 0;
        set_pon_led_spec = 2;
        switch (next_speed_flag)
        {
            case  EPON_FAMILY_MODE_EPON:
                set_speed = 0;
                break;
            case  EPON_FAMILY_MODE_XGEPON_US_1G:
                set_speed = 1;
                break;
            case  EPON_FAMILY_MODE_XGEPON_US_10G:
                set_speed = 2;
                break;
            default:
                printf("pon auto detect: unexpected mode to switch 0x%x\r\n", next_speed_flag);
                return -1;
                break;
        }
    }
    else
    {
        printf("pon auto detect: unexpected mode for MIB setup \r\n");
        return -1;
    }

    return ret;
}


int
pon_mode_mib_set(rtk_enable_t reboot)
{
    unsigned char buf[32];

    memset(buf, 0, 32);
    sprintf(buf, "mib set PON_MODE %d", set_mode);
    system(buf);
    memset(buf, 0, 32);
    sprintf(buf, "mib set PON_SPEED %d", set_speed);
    system(buf);
    memset(buf, 0, 32);
    sprintf(buf, "mib set PON_SUB_MODE %d", set_sub_mode);
    system(buf);
    memset(buf, 0, 32);
    sprintf(buf, "mib set PON_LED_SPEC %d", set_pon_led_spec);
    system(buf);
    memset(buf, 0, 32);
    sprintf(buf, "mib commit");
    system(buf);

    if(reboot)
    {
        /* reboot device */
        system("reboot");
    }
    return 0;
}

int
auto_detect_mib_set()
{
    unsigned char buf[64];

    memset(buf, 0, 64);
    sprintf(buf, "mib set PON_DETECT_CONTROL_FLAG %d", ctrl_flag);
    system(buf);
    memset(buf, 0, 64);
    sprintf(buf, "mib commit");
    system(buf);

    return 0;
}

int
pon_mode_swap_set(void)
{
    rt_gpon_serialNumber_t gpon_sn;
    rt_gpon_registrationId_t gpon_pw;

    if(swap_ponMode == RT_GPON_MODE)
    {  //GPON Family
        //retrieve current GPON SN/PASSWORD
        if(rt_gpon_serialNumber_get(&gpon_sn) != RT_ERR_OK)
        {
            printf("pon auto detect: gpon get serialnumber failed \r\n");
            return -1;
        }
        if(rt_gpon_registrationId_get(&gpon_pw) != RT_ERR_OK)
        {
            printf("pon auto detect: gpon get registrationId failed \r\n");
            return -1;
        }

        //deactivate gpon
        if(rt_gpon_deactivate() != RT_ERR_OK)
        {
            printf("pon auto detect: deactivate xGPON failed \r\n");
            return -1;
        }

        //disable pon mode
        if(rt_ponmisc_modeSpeed_set(PON_MODE_DISABLE, 0) != RT_ERR_OK)
        {
            printf("pon auto detect: disable pon mode failed \r\n");
            return -1;
        }

        //set new pon mode
        if(rt_ponmisc_modeSpeed_set(swap_ponMode, swap_ponSpeed) != RT_ERR_OK)
        {
            printf("pon auto detect: set new xGPON mode failed \r\n");
            return -1;
        }

        //gpon init
        if(rt_gpon_init() != RT_ERR_OK)
        {
            printf("pon auto detect: gpon init failed \r\n");
            return -1;
        }

        //set GPON SN/PASSWORD
        if(rt_gpon_serialNumber_set(&gpon_sn) != RT_ERR_OK)
        {
            printf("pon auto detect: gpon set serialnumber failed \r\n");
            return -1;
        }

        if(rt_gpon_registrationId_set(&gpon_pw) != RT_ERR_OK)
        {
            printf("pon auto detect: gpon set registrationId failed \r\n");
            return -1;
        }

        //activate gpon with O1
        if(rt_gpon_activate(RT_GPON_ONU_INIT_STATE_O1) != RT_ERR_OK)
        {
            printf("pon auto detect: gpon activation failed \r\n");
            return -1;
        }
    }
    else if(swap_ponMode == RT_EPON_MODE)
    {  //EPON Family
        //disable pon mode
        if(rt_ponmisc_modeSpeed_set(PON_MODE_DISABLE, 0) != RT_ERR_OK)
        {
            printf("pon auto detect: disable pon mode failed \r\n");
            return -1;
        }

        //set new pon mode
        if(rt_ponmisc_modeSpeed_set(swap_ponMode, swap_ponSpeed) != RT_ERR_OK)
        {
            printf("pon auto detect: set new xEPON mode failed \r\n");
            return -1;
        }

        //epon init
        if(rt_epon_init() != RT_ERR_OK)
        {
            printf("pon auto detect: epon init failed \r\n");
            return -1;
        }
    }
    else
    {
            printf("pon_mode_swap_set: unsupported pon mode to set \r\n");
            return -1;
    }

    return 0;
}

int
pon_mode_swap(void)
{
    int i, ret = 0;
    unsigned int current_speed_flag=0, current_mode=0, supported_speed_flag=0;
    unsigned int next_speed_flag=0, next_mode=0;
    unsigned int rotate=0, overflow_mask=0;

    //only support GPON family in 1st stage
    if(ponMode == RT_GPON_MODE)
    {
        current_mode = PON_MODE_FAMILY_GPON;
        next_mode = PON_MODE_FAMILY_GPON;
        supported_speed_flag = gpon_mode_flag;
        if(ponSpeed == RT_1G25G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_GPON;
        else if(ponSpeed == RT_BOTH2DOT5G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_GPON_US_2DOT5G;
        else if(ponSpeed == RT_DN10G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_XGPON;
        else if(ponSpeed == RT_BOTH10G_SPEED)
            current_speed_flag = GPON_FAMILY_MODE_XGSPON;
        else {
            printf("pon auto detect: unsupported GPON speed %d\r\n", ponSpeed);
            return -1;
        }
    }
    else
    {
        //TBD: EPON, NG-PON2, Ethernet or other modes
        printf("pon auto detect: unsupported PON mode for swap %d\r\n", ponMode);
        return -1;
    }

    //checking for next speed
    if(supported_speed_flag & (~current_speed_flag))
    {
        if(CTRL_FLAG_SPEED_SELECT_INCREASE & ctrl_flag)
        {
            for(i=1;i<32;i++)
            {
                if(current_speed_flag == UINT_MSB)
                {
                    rotate = 1;
                }
                if(rotate)
                {
                    if((UINT_LSB << (i-rotate)) & supported_speed_flag)
                    {
                        next_speed_flag = (UINT_LSB << (i-rotate));
                        break;
                    }
                }
                else
                {
                    overflow_mask=(0xFFFFFFFF >> i);
                    if(((unsigned int)((current_speed_flag & overflow_mask) << i)) & supported_speed_flag)
                    {
                        next_speed_flag = ((current_speed_flag & overflow_mask) << i);
                        break;
                    }
                    if(((current_speed_flag & overflow_mask) << i) == UINT_MSB)
                    {
                        rotate = i+1;
                    }
                }
            }
        }
        else
        {
            for(i=1;i<32;i++)
            {
                if(current_speed_flag == UINT_LSB)
                {
                    rotate = 1;
                }
                if(rotate)
                {
                    if((UINT_MSB >> (i-rotate)) & supported_speed_flag)
                    {
                        next_speed_flag = (UINT_MSB >> (i-rotate));
                        break;
                    }
                }
                else
                {
                    if((current_speed_flag >> i) & supported_speed_flag)
                    {
                        next_speed_flag = (current_speed_flag >> i);
                        break;
                    }
                    if((current_speed_flag >> i) == UINT_LSB)
                    {
                        rotate = i+1;
                    }
                }
            }
        }
    }
    else
    {
        printf("pon auto detect: unsupported PON speed for swap, supported_speed=%d\r\n", supported_speed_flag);
        return -1;
    }

    //setup next PON_MODE PON_SPEED PON_SUB_MODE for MIB
    if(next_mode == PON_MODE_FAMILY_GPON)
    {
        set_mode = 1;
        set_sub_mode = 0;
        swap_ponMode = RT_GPON_MODE;

        switch (next_speed_flag)
        {
            case  GPON_FAMILY_MODE_GPON:
                set_speed = 0;
                swap_ponSpeed = RT_1G25G_SPEED;
                break;
            case  GPON_FAMILY_MODE_GPON_US_2DOT5G:
                set_speed = 3;
                swap_ponSpeed = RT_BOTH2DOT5G_SPEED;
                break;
            case  GPON_FAMILY_MODE_XGPON:
                set_speed = 1;
                swap_ponSpeed = RT_DN10G_SPEED;
                break;
            case  GPON_FAMILY_MODE_XGSPON:
                set_speed = 2;
                swap_ponSpeed = RT_BOTH10G_SPEED;
                break;
            default:
                printf("pon auto detect: unexpected speed to switch 0x%x\r\n", next_speed_flag);
                return -1;
                break;
        }

        if(pon_mode_swap_set()!=0)
        {
            printf("pon auto detect: pon_mode_swap_set failed \r\n");
            return -1;
        }
    }
    else
    {
        printf("pon auto detect: unexpected mode for PON mode set \r\n");
        return -1;
    }

    return ret;
}

struct thread_info {    /* Used as argument to thread_start() */
   pthread_t thread_id;        /* ID returned by pthread_create() */
   int       thread_num;       /* Application-defined thread # */
   char     *argv_string;      /* From command-line argument */
};

static int
pon_bcast_msg_recv(int fd, intrBcasterMsg_t *pMsgData)
{
    int                 recvLen;
    int                 nlmsgLen;
    struct nlmsghdr     *pNlh = NULL;
    struct iovec        iov;
    struct sockaddr_nl  da;
    struct msghdr       msg;

    if (!pMsgData)
        return -1;

    // allocate memory
    nlmsgLen = NLMSG_SPACE(MAX_BYTE_PER_INTR_BCASTER_MSG);
    pNlh = (struct nlmsghdr *)malloc(nlmsgLen);
    if (!pNlh)
        return -1;

    // fill struct nlmsghdr
    memset(pNlh, 0, nlmsgLen);

    // fill struct iovec
    memset(&iov, 0, sizeof(struct iovec));
    iov.iov_base = (void *)pNlh;
    iov.iov_len = nlmsgLen;

    // fill struct sockaddr_nl
    memset(&da, 0, sizeof(struct sockaddr_nl));
    da.nl_family = AF_NETLINK;

    // fill struct msghdr
    memset(&msg, 0x0, sizeof(struct msghdr));
    msg.msg_name = (void *)&da;
    msg.msg_namelen = sizeof(da);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // receive message
    recvLen = recvmsg(fd, &msg, 0);
    if (!NLMSG_OK(pNlh, recvLen))
    {
        free(pNlh);
        return -1;
    }

    memcpy(pMsgData, NLMSG_DATA(pNlh), MAX_BYTE_PER_INTR_BCASTER_MSG);
    free(pNlh);

    return 0;
}

int
pon_event_handler(unsigned int * data)
{
    int ret=0;
    int mode_swap=0;
    static unsigned int new_ctrl_flag = 0;
    static int gIntrSocketFd = -1;
    intrBcasterMsg_t    msgData;
    fd_set              fdSet;
    struct sockaddr_nl sa;

    //create socket for receiving broadcast messages
    if (gIntrSocketFd < 0)
    {
        if (-1 == (gIntrSocketFd = socket(PF_NETLINK, SOCK_RAW, INTR_BCASTER_NETLINK_TYPE)))
        {
            printf("pon_event_handler: Create Interrupt Socket Fail!\n");
            return -1;
        }
        printf("pon_event_handler: gIntrSocketFd=%d\r\n", gIntrSocketFd);

        memset(&sa, 0, sizeof(sa));
        sa.nl_family = AF_NETLINK;
        sa.nl_groups = (1<<MSG_TYPE_OMCI_EVENT);

        if (-1 == bind(gIntrSocketFd, (struct sockaddr*)&sa, sizeof(struct sockaddr)))
        {
            printf("pon_event_handler: Bind Interrupt Socket Fail!\n");
            return -1;
        }

        FD_ZERO(&fdSet);
        FD_SET(gIntrSocketFd, &fdSet);
    }

    while(1)
    {
        if (select(gIntrSocketFd + 1, &fdSet, NULL, NULL, NULL) < 0)
            break;

        if (!FD_ISSET(gIntrSocketFd, &fdSet))
            continue;

        memset(&msgData, 0, sizeof(intrBcasterMsg_t));
        if (0 == pon_bcast_msg_recv(gIntrSocketFd, &msgData))
        {
            if (MSG_TYPE_OMCI_EVENT == msgData.intrType)
            {
                if(OMCI_EVENT_TYPE_PON_SPEED_CHANGE == msgData.intrSubType)
                {
                    set_mode = 1;
                    set_sub_mode = 0;
                    swap_ponMode = RT_GPON_MODE;
                    new_ctrl_flag = ctrl_flag;

                    printf("pon_event_handler: Receive Interrupt: OMCI_EVENT_TYPE_PON_SPEED_CHANGE \r\n");
                    switch(msgData.intrStatus){
                        case BCAST_OMCI_PON_SPEED_AUTO:
                            printf("pon_event_handler: Receive Interrupt: BCAST_OMCI_PON_SPEED_AUTO\r\n");
                            new_ctrl_flag = ctrl_flag | CTRL_FLAG_RX_LOS_DETECT_ENABLE;
                        break;
                        case BCAST_OMCI_PON_SPEED_GPON:
                            printf("pon_event_handler: Receive Interrupt: BCAST_OMCI_PON_SPEED_GPON\r\n");
                            set_speed = 0;
                            swap_ponSpeed = RT_1G25G_SPEED;
                            mode_swap = 1;
                            new_ctrl_flag = ctrl_flag & (~CTRL_FLAG_RX_LOS_DETECT_ENABLE);
                        break;
                        case BCAST_OMCI_PON_SPEED_XGPON:
                            printf("pon_event_handler: Receive Interrupt: BCAST_OMCI_PON_SPEED_XGPON\r\n");
                            set_speed = 1;
                            swap_ponSpeed = RT_DN10G_SPEED;
                            mode_swap = 1;
                            new_ctrl_flag = ctrl_flag & (~CTRL_FLAG_RX_LOS_DETECT_ENABLE);
                        break;
                        case BCAST_OMCI_PON_SPEED_XGSPON:
                            printf("pon_event_handler: Receive Interrupt: BCAST_OMCI_PON_SPEED_XGSPON\r\n");
                            set_speed = 2;
                            swap_ponSpeed = RT_BOTH10G_SPEED;
                            mode_swap = 1;
                            new_ctrl_flag = ctrl_flag & (~CTRL_FLAG_RX_LOS_DETECT_ENABLE);
                        break;
                        default:
                            printf("pon_event_handler: Unknown PON Speed\r\n");
                        break;
                    }

                    if(mode_swap)
                    {
                        //set MIB
                        printf("pon_event_handler: set MIB PON_MODE=%d, PON_SPPED=%d, PON_SUB_MODE=%d\n\r", set_mode, set_speed, set_sub_mode);
                        pon_mode_swap_set();
                        pon_mode_mib_set(0);
                        mode_swap = 0;
                    }

                    if(new_ctrl_flag != ctrl_flag)
                    {
                        printf("pon_event_handler: set MIB ctrl_flag=%d, new_ctrl_flag=%d\n\r", ctrl_flag, new_ctrl_flag);
                        ctrl_flag = new_ctrl_flag;
                        auto_detect_mib_set();
                    }
                }
            }
        }

    }

    return ret;
}

#endif

int
main(
    int argc,
    char *argv[])
{
#if defined(CONFIG_PON_DETECT_FEATURE)
    rtk_ponmac_mode_t check_mode = PONMAC_MODE_END;
    unsigned int sd, sync;
    unsigned char buf[64];
    char *strMode=NULL;
    FILE *pFD;

    int reset_cnt=0, rx_los_cnt=0, mode_swap=0;
    int ret;
    int i=0;
    rt_ponmisc_prbs_t prbsRxCfg=0, prbsTxCfg=0;
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
    rt_transceiver_data_t data;
    int rx_h=-7, rx_l=-28;
    double rx_p=0;
    rx_pow_state_t rx_state=RX_NORMAL;
#ifdef CONFIG_LUNA_G3_SERIES
    rt_gpon_pm_counter_t counter;
    unsigned int ber=0, log_ber=0;
#endif
#endif
    struct thread_info  tinfo;

    if(argc<5)
    {
        printf("pon auto detect: input parameter number error!\r\n");
        return 0;
    }
    
    ctrl_flag = atoi(argv[1]);
    gpon_mode_flag = atoi(argv[2]);
    epon_mode_flag = atoi(argv[3]);
    ether_mode_flag = atoi(argv[4]);

    if((ret = rt_ponmisc_modeSpeed_get(&ponMode,&ponSpeed)) != RT_ERR_OK)
    {
        printf("%s:%d, ret = %d\n",__FUNCTION__, __LINE__, ret);
    }

    switch(ponMode)
    {
        case RT_GPON_MODE:
            check_mode = PONMAC_MODE_GPON;
            break;
        case RT_EPON_MODE:
            check_mode = PONMAC_MODE_EPON;
            break;
        default: //check_mode is only used for Apollo seriel
            check_mode = PONMAC_MODE_END;
            break;
    }

    //fork a thread for event handle
    tinfo.thread_num=1;
    if (0 != pthread_create(&tinfo.thread_id, NULL, (void*)pon_event_handler, (void*)&tinfo))
    {
        printf("pon auto detect: failed to create thread for event handler\n\r");
        return 0;
    }

    /* delay to detect for PON MAC ready */
    sleep(10);

    while(1)
    {
        sleep(2);
        /* check PRBS TX and RX enable */
        if((ret = rt_ponmisc_forcePRBS_get(&prbsTxCfg))!= RT_ERR_OK)
        {
            prbsTxCfg = RT_PONMISC_PRBS_OFF;
        }

        if((ret = rt_ponmisc_forcePRBS_rx_get(&prbsRxCfg))!= RT_ERR_OK)
        {
            prbsRxCfg = RT_PONMISC_PRBS_OFF;
        }

        if((prbsTxCfg != RT_PONMISC_PRBS_OFF) || (prbsRxCfg != RT_PONMISC_PRBS_OFF))
            continue;

        /* get pon link state */
        if((ret = rtk_ponmac_linkState_get(check_mode, &sd, &sync))!= RT_ERR_OK)
        {
            printf("pon auto detect: get link state failed 0x%x\n\r", ret);
            continue;
        }
        /*printf("pon auto detect: pon mode=%d, get sd=%d, sync=%d\n\r",mib_pon_mode, sd, sync);*/
        if((sd == 0) && (sync == 0)) /* no link */
        {
            /*printf("pon auto detect: pon mode detect no link \n\r");*/
            reset_cnt = 0;

            if(ctrl_flag & CTRL_FLAG_RX_LOS_DETECT_ENABLE)
            {
                if(rx_los_cnt >= RX_LOS_SWAP_TIMES)
                {
                    printf("pon auto detect: RX_LOS detected and swap mode for detect \n\r");
                    rt_ponmisc_modeSpeed_get(&ponMode,&ponSpeed);
                    if(pon_mode_swap()==0)
                    {
                        printf("pon auto detect: PON mode swaped to PON_MODE=%d, PON_SPPED=%d, PON_SUB_MODE=%d \n\r", set_mode, set_speed, set_sub_mode);
                        mode_swap = 1;
                    }
                    else
                    {
                        printf("pon auto detect: unable to determine next PON mode for swap\r\n");
                    }
                    rx_los_cnt=0;
                }
            }
            rx_los_cnt++;
        }
        else
        if((sd == 1) && (sync == 0)) /* wrong pon mode */
        {
            if(ctrl_flag & CTRL_FLAG_AUTO_SELECT_ENABLE)
            {
                if(reset_cnt == RESET_CDR_TIMES)
                {
                    printf("pon auto detect: pon mode detect wrong mode \n\r");
                    if(pon_mode_switch()==0)
                    {
                        printf("pon auto detect: set MIB PON_MODE=%d, PON_SPPED=%d, PON_SUB_MODE=%d and then reboot\n\r", set_mode, set_speed, set_sub_mode);
                        /* set pon mode */
                        pon_mode_mib_set(1);
                    }
                    else
                    {
                        printf("pon auto detect: unable to determine next PON mode for switching\r\n");
                    }

                }
            }
            /* serdes cdr reset for re-lock */
            else
            {
                if((ret = rtk_ponmac_serdesCdr_reset()) != RT_ERR_OK)
                {
                    printf("pon auto detect: serdes reset fail 0x%x\n\r", ret);
                }
            }
            printf("pon auto detect: sd=1, sync=0, reset-cdr \n\r");
            reset_cnt++;
            rx_los_cnt=0;
        }
        else
        {
            if((sd == 1) && (sync == 1)) /* correct pon mode */
            {
                /*printf("pon auto detect: pon mode detect correct mode %d \n\r", check_mode);*/
                reset_cnt = 0;
                rx_los_cnt=0;

                if(mode_swap)
                {
                    //set MIB
                    pon_mode_mib_set(0);

                    mode_swap = 0;
                }
            }
            else /* strange state */
            {
                printf("pon auto detect: pon mode detect strange!!! sd=%d, sync=%d!!\n\r", sd, sync);
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
                /* gpon log for error case */
                system("echo 10002000 > /proc/exception_log/exc_log_pon");
#endif
            }
        }

#ifdef CONFIG_RTK_GPON_EXCEP_LOG
        /* gpon log periodically */
        if(++i%(GPON_LOG_INTERVAL/2) == 0)
        {
            /* get power range threshold */
            opticalPowerRange(&rx_l, &rx_h);
            //printf("gpon_log: rx_l=%d, rx_h=%d\n\r",rx_l,rx_h);

            /* gpon log for monit rx_power */
            if(rt_ponmisc_transceiver_get(RT_TRANSCEIVER_PARA_TYPE_RX_POWER, &data) == RT_ERR_OK)
            {
                rx_p = __log10(((double)((data.buf[0] << 8) | data.buf[1])*1/10000))*10;
                //printf("gpon_log: rx_power=%lf\n\r",rx_p);
                if(rx_p > rx_h)
                {
                    if(rx_state == RX_NORMAL)
                    {
                        system("echo 10000001 > /proc/exception_log/exc_log_pon");
                        //printf("user_exc_log_add: 10000001\n\r");
                    }
                    rx_state = RX_HIGH;
                }
                else if(rx_p < rx_l)
                {
                    if(rx_state == RX_NORMAL)
                    {
                        system("echo 10000002 > /proc/exception_log/exc_log_pon");
                        //printf("user_exc_log_add: 10000002\n\r");
                    }
                    rx_state = RX_LOW;
                }
                else
                {
                    if((rx_state == RX_HIGH) || (rx_state == RX_LOW))
                    {
                        system("echo 10000003 > /proc/exception_log/exc_log_pon");
                        //printf("user_exc_log_add: 10000003\n\r");
                    }
                    rx_state = RX_NORMAL;
                }
            }
#ifdef CONFIG_LUNA_G3_SERIES
            /* gpon log for bip error */
            if(rt_gpon_pmCounter_get(RT_GPON_PM_TYPE_PHY_LOS, &counter) == RT_ERR_OK)
            {
                /*unit¡G%¡A10^-6 */
                /* (bipx100000000)/(intervalx8000x38880) */
                ber = (counter.phyLos.bip_error_count*10000)/(GPON_LOG_INTERVAL*8*3888); 
                if(ber > 999)
                    ber = 999;
                //printf("gpon_log: bip=%u, ber=%u\n\r",counter.phyLos.bip_error_count, ber);

                if(((log_ber == 0) && (ber > 0)) || 
                   ((log_ber > 0) && (ber == 0)))
                {
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "echo %u > /proc/exception_log/exc_log_pon", (10003000+ber));
                    system(buf);
                }
                    
                log_ber = ber;
            }
#endif
        }
#endif        
    }

#endif
    return 0;
}

