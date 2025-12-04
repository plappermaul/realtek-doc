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

#include <sys/socket.h>
#include <linux/netlink.h>
#include "pkt_redirect_user.h"

/*
 * Symbol Definition
 */
#define SFP_MAX_PAYLOAD 1600

#define SFP_CMD_LOID_PWD_UPDATE     0x01
#define SFP_CMD_RESTORE             0x02
#define SFP_CMD_QUERY_LOIDAUTH      0x03
#define SFP_CMD_GPONPWD_UPDATE      0x04

#define SEP_APP_GPONPWD_LENGTH      10
#define SEP_APP_LOID_LENGTH         24
#define SEP_APP_PWD_LENGTH          12
#define SEP_APP_CMD_LENGTH          1

/*
 * Data Declaration
 */
int sfpPktRedirect_sock;

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */
static void sfpApp_recv_cmd_loid_pwd(unsigned char *pLoid, unsigned char *pPwd)
{
    int idx, ptr;
    unsigned char cmd[200];
    unsigned char loid[100];
    unsigned char password[100];

    if( (pLoid == NULL) || (pPwd == NULL) )
    {
        printf("\n LOID or PASSWORD is NULL\n");
        return;
    }

    /* Check character from '0'~'9', 'a'~'z', 'A'~'Z'*/
    ptr = 0;
    loid[ptr] = '\0';
    for(idx = 0; idx < SEP_APP_LOID_LENGTH; idx++)
    {
        if( ((pLoid[idx] >= '0') && (pLoid[idx] <= '9')) || ((pLoid[idx] >= 'a') && (pLoid[idx] <= 'z')) || ((pLoid[idx] >= 'A') && (pLoid[idx] <= 'Z')) )
        {
            loid[ptr] = pLoid[idx];
            ptr++;
        }
    }
    loid[ptr] = '\0';

    ptr = 0;
    password[ptr] = '\0';
    for(idx = 0; idx < SEP_APP_PWD_LENGTH; idx++)
    {
        if( ((pPwd[idx] >= '0') && (pPwd[idx] <= '9')) || ((pPwd[idx] >= 'a') && (pPwd[idx] <= 'z')) || ((pPwd[idx] >= 'A') && (pPwd[idx] <= 'Z')) )
        {
            password[ptr] = pPwd[idx];
            ptr++;
        }
    }
    password[ptr] = '\0';

    /* Write LOID & PASSWORD to Flash */
    sprintf(cmd, "flash set LOID %s", loid);
    system(cmd);

    /* Password could be empty */
    if(pPwd[0] != '\0')
    {
        sprintf(cmd, "flash set LOID_PASSWD %s", password);
        system(cmd);
    }

    /* Pass New LOID & Password to OMCI */
    sprintf(cmd, "omcicli set loid %s %s", loid, password);
    system(cmd);
}

static void sfpApp_recv_cmd_restore(void)
{
    system("flash default cs");
}

static void sfpApp_recv_cmd_query_loidAuth(void)
{
    FILE *pp;
    unsigned char cbuffer[1000];
    unsigned char data[10];

    pp = popen("omcicli get loidauth | sed 's/Auth Status : //g'", "r");

    if(fgets(cbuffer, sizeof(cbuffer), pp) != NULL)
    {
        if(cbuffer[0] >= '0' && cbuffer[0] <= '9')
        {
            data[0] = cbuffer[0] - '0';
        }
    }

    pclose(pp);
    ptk_redirect_userApp_sendPkt(sfpPktRedirect_sock, PR_KERNEL_UID_SFPCTRL, 0, 1, data);
}

static void sfpApp_recv_cmd_gpon_pwd(unsigned char *pGponPwd)
{
    int idx, ptr;
    unsigned char cmd[200];
    unsigned char password[100];

    if(pGponPwd == NULL)
    {
        printf("\n GPON PASSWORD is NULL\n");
        return;
    }

    /* Check character from '0'~'9', 'a'~'z', 'A'~'Z'*/
    ptr = 0;
    password[ptr] = '\0';
    for(idx = 0; idx < SEP_APP_GPONPWD_LENGTH; idx++)
    {
        if( ((pGponPwd[idx] >= '0') && (pGponPwd[idx] <= '9')) || ((pGponPwd[idx] >= 'a') && (pGponPwd[idx] <= 'z')) || ((pGponPwd[idx] >= 'A') && (pGponPwd[idx] <= 'Z')) )
        {
            password[ptr] = pGponPwd[idx];
            ptr++;
        }
    }
    password[ptr] = '\0';

    /* Write GPON PASSWORD to Flash */
    sprintf(cmd, "flash set GPON_PLOAM_PASSWD %s", password);
    system(cmd);
}

static int sfpApp_recv(unsigned char *frame, unsigned int frame_len)
{
    int i;
#if 0
    printf("receive SFP packet: len = %d\n", frame_len);
    for (i = 0; i < frame_len; i++)
    {
        printf("%02x ", frame[i]);
    }
    printf("\n");
#endif
    switch (frame[0])
    {
        case SFP_CMD_LOID_PWD_UPDATE:
            if(frame_len < (SEP_APP_LOID_LENGTH + SEP_APP_PWD_LENGTH + SEP_APP_CMD_LENGTH))
                printf("\n Error!! LOID+PWD len = %d\n", frame_len);
            else
                sfpApp_recv_cmd_loid_pwd(&frame[SEP_APP_CMD_LENGTH], &frame[SEP_APP_CMD_LENGTH + SEP_APP_LOID_LENGTH]);
            break;
        case SFP_CMD_RESTORE:
            sfpApp_recv_cmd_restore();
            break;
        case SFP_CMD_QUERY_LOIDAUTH:
            sfpApp_recv_cmd_query_loidAuth();
            break;
        case SFP_CMD_GPONPWD_UPDATE:
            if(frame_len < (SEP_APP_GPONPWD_LENGTH + SEP_APP_CMD_LENGTH))
                printf("\n Error!! GPON PWD len = %d\n", frame_len);
            else
                sfpApp_recv_cmd_gpon_pwd(&frame[SEP_APP_CMD_LENGTH]);
            break;
        default:
            printf("\n Error!! Unknown SFP command!!\n");
    }

    return 0;
}

int
main(
	int argc,
	char *argv[])
{
    unsigned short dataLen;
	unsigned char *payload;

	payload = (unsigned char *)osal_alloc(SFP_MAX_PAYLOAD * sizeof(char));

    printf("SFP APP\n\r");

    /* Create sock */
    if (-1 == (sfpPktRedirect_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_USERSOCK)))
	{
		printf("Create Socket Fail!\n");
	    return -1;
	}

    /* register */
    if(0 > ptk_redirect_userApp_reg(sfpPktRedirect_sock, PR_USER_UID_SFPCTRL, SFP_MAX_PAYLOAD))
	{
		printf("Register Packet Redirect Fail.\n");
		return -1;
	}

    /* Start to receive command from kernel */
	while(1)
    {
        if(ptk_redirect_userApp_recvPkt(sfpPktRedirect_sock, SFP_MAX_PAYLOAD, &dataLen, payload) > 0)
        {
			sfpApp_recv(payload, dataLen);
		}
	}

    return 0;
}

