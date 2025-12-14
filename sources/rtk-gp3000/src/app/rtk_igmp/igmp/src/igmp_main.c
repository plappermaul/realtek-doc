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
 * Purpose : Main function of SFU IGMP Application
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IGMP main APIs
 */

/*
* Include Files
*/
#include <igmp/inc/proto_igmp.h>
#include "pkt_redirect_user.h"
#include <rtk/init.h>

static int          igmpPktRedirect_sock;
igmp_mld_control_t  igmpCtrl;
igmp_stats_t        *p_igmp_stats = NULL;
mld_stats_t         *p_mld_stats = NULL;
uint32              gUnknown_mcast_action;

char *sdk_parameters[SDK_MODE_END][6] =
{
    {"flash", "PON_MODE", "RTK_IGMP_DBGEN", "RTK_IGMP_DROPV1", "RTK_IGMP_TAGDECISION", "RTK_IGMP_VIDTYPE"},
    {"/sbin/uci", "ponMode", "logEnable", "igmpv1Drop_enable", "igmpTagBehavior_enable", "igmpVlanTag_type" }
};


static int config_modify_by_sdk(void)
{
    char sdkMibVal[SDK_MIB_BUFFER_LEN];

    //pon_mode
    memset(sdkMibVal, 0, SDK_MIB_BUFFER_LEN);
    if (SYS_ERR_OK == igmp_util_get_sdk_mib_value_by_name(sdk_parameters[igmpCtrl.sdkMode][1], sdkMibVal))
    {
        igmpCtrl.ponMode = strtol(sdkMibVal, NULL, 0);
    }

    //log
    memset(sdkMibVal, 0, SDK_MIB_BUFFER_LEN);
    if (SYS_ERR_OK == igmp_util_get_sdk_mib_value_by_name(sdk_parameters[igmpCtrl.sdkMode][2], sdkMibVal))
    {
        igmpCtrl.logEnable = strtol(sdkMibVal, NULL, 0);
    }

    //igmp v1 behavior
    memset(sdkMibVal, 0, SDK_MIB_BUFFER_LEN);
    if (SYS_ERR_OK == igmp_util_get_sdk_mib_value_by_name(sdk_parameters[igmpCtrl.sdkMode][3], sdkMibVal))
    {
        igmpCtrl.igmpv1Drop_enable = strtol(sdkMibVal, NULL, 0);
    }

    //igmp tag search behavior
    memset(sdkMibVal, 0, SDK_MIB_BUFFER_LEN);
    if (SYS_ERR_OK == igmp_util_get_sdk_mib_value_by_name(sdk_parameters[igmpCtrl.sdkMode][4], sdkMibVal))
    {
        igmpCtrl.igmpTagBehavior_enable = strtol(sdkMibVal, NULL, 0);
    }

    //igmp query tag vid type
    memset(sdkMibVal, 0, SDK_MIB_BUFFER_LEN);
    if (SYS_ERR_OK == igmp_util_get_sdk_mib_value_by_name(sdk_parameters[igmpCtrl.sdkMode][5], sdkMibVal))
    {
        igmpCtrl.igmpVlanTag_type = strtol(sdkMibVal, NULL, 0);
    }

    return SYS_ERR_OK;
}

static void _userStatusConfigSet(char *str, int value)
{
	if (!strcmp(str, "l2LookupMode"))
	{
		p_igmp_stats->lookup_mode = (igmp_lookup_mode_t)value;
	}
    else if (!strcmp(str, "igmpOperVer"))
	{
		p_igmp_stats->oper_version = (igmp_version_t)value;
	}
    else if (!strcmp(str, "unknowIp4mcastAct"))
	{
		//p_igmp_stats->unknowIp4mcastAct = value;
	}
    else if (!strcmp(str, "gponMacTblReplaceL2MacTbl"))
	{
		igmpCtrl.ponMacReplaceL2_enable = (sys_enable_t)value;
	}
    return;
}

/*Get config from config file, and set*/
static int userStatusConfigSet(char *filename)
{
	FILE    *pFile = NULL;
	char    buff[100], *str, *pch;
	int32   value;

    pFile = fopen(filename, "r");

    SYS_ERR_CHK((NULL == pFile), SYS_ERR_FAILED);

	while (NULL != fgets(buff, sizeof(buff), pFile))
	{
		/*string start with "#" means this line is comment*/
		if (buff[0] == '#' || buff[0] == '\n')
			continue;

		pch = strtok(buff, "=");
		str = pch;

		pch = strtok(NULL, "=");
		value = atoi(pch);

		_userStatusConfigSet(str, value);
	}

    fclose(pFile);
    pFile = NULL;

	return SYS_ERR_OK;
}

static void config_init_default(void)
{
    // igmp_mld ctrl init
	igmpCtrl.igmpv1Drop_enable  = DISABLED;
    LOGIC_PORTMASK_CLEAR_ALL(igmpCtrl.igmpMldTrap_enable);

    // p_igmp_stats init
	p_igmp_stats->lookup_mode      = DFLT_IGMP_LOOKUP_MODE;
	p_igmp_stats->igmpsnp_enable   = DISABLED;
	p_igmp_stats->suppre_enable    = DFLT_IGMP_SUPPRE_EBL;
	p_igmp_stats->oper_version     = DFLT_IGMP_OPERATION_VER;

	gUnknown_mcast_action       = DFLT_IGMP_UNKNOWN_MCAST_ACTION;

    // p_mld_stats init
    p_mld_stats->lookup_mode   = DFLT_MLD_LOOKUP_MODE;
    p_mld_stats->mldsnp_enable = DISABLED;
    p_mld_stats->suppre_enable = DFLT_MLD_SUPPRE_EBL;
    p_mld_stats->oper_version  = DFLT_MLD_OPERATION_VER;

	return;
}

int32 igmp_mld_send_pkt(uint8 *pMsg, uint32 len, rtk_portmask_t portmask, uint32 sid)
{
    uint8 is_unkown = 0;
    uint8 data[MAX_PAYLOAD];
    uint32 portMask = 0, realDataLen = 0;
    rtk_portmask_t zero_mask, ether_pmk;

    SYS_ERR_CHK(((NULL == pMsg) || (0 == len) || (MAX_PAYLOAD - sizeof(uint32) - sizeof(uint32) < len)), SYS_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET(zero_mask);

    SYS_DBG(LOG_DBG_IGMP,
        "%s()@%d: origial  physical port mask=%u, sid=%u\n", __FUNCTION__, __LINE__, portmask.bits[0], sid);

    is_unkown = (((RTK_PORTMASK_COMPARE(portmask, zero_mask)) || (RTK_PORTMASK_IS_PORT_SET(portmask, HAL_GET_PON_PORT()))) ? 0 : 1);

    if (is_unkown)
    {
        RTK_PORTMASK_RESET(ether_pmk);
        HAL_GET_ETHER_PORTMASK(ether_pmk); /* include pon portmask  */
        RTK_PORTMASK_PORT_CLEAR(ether_pmk, HAL_GET_PON_PORT());
        portMask = ether_pmk.bits[0];
    }
    else
    {
        portMask = portmask.bits[0];
    }

    realDataLen = ((len + sizeof(uint32) * 2) > MAX_PAYLOAD ? MAX_PAYLOAD : (len + sizeof(uint32) * 2));
    osal_memset(data, 0, MAX_PAYLOAD);
    // portMask + sid + pMsg
    osal_memcpy(data, (unsigned char *)&portMask, sizeof(uint32));
    osal_memcpy(data + sizeof(uint32), (unsigned char *)&sid, sizeof(uint32));
    osal_memcpy(data + sizeof(uint32) * 2, pMsg, len);

    SYS_DBG(LOG_DBG_IGMP,
        "%s()@%d, is_unkown=%u pkt fwd port mask=%u, realDataLen=%u\n", __FUNCTION__, __LINE__, is_unkown, portMask, realDataLen);


    if (0 > ptk_redirect_userApp_sendPkt(igmpPktRedirect_sock, PR_KERNEL_UID_IGMPCTRL, 0, realDataLen, (unsigned char *)&data))
    {
        printf("Send packet failed by PR %s() @%d\n", __FUNCTION__, __LINE__);
        return SYS_ERR_FAILED;
    }

    return SYS_ERR_OK;
}

static int32 igmp_mld_recv_pkt(void)
{
	unsigned short dataLen  = 0;
	unsigned char *pPayload = NULL;

	pPayload = (unsigned char *)osal_alloc(MAX_PAYLOAD * sizeof(char));

    SYS_PARAM_CHK((!pPayload), SYS_ERR_NULL_POINTER);

	while (1)
    {
        if (ptk_redirect_userApp_recvPkt(igmpPktRedirect_sock, MAX_PAYLOAD, &dataLen, pPayload) > 0)
        {
			mcast_recv(pPayload, dataLen);
		}
	}
	return SYS_ERR_OK;
}

int32 igmp_mld_create_pkt_dev(void)
{
    if (-1 == (igmpPktRedirect_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_USERSOCK)))
	{
		printf("Create Socket Fail!\n");
	    return SYS_ERR_FAILED;
	}

    if (0 > ptk_redirect_userApp_reg(igmpPktRedirect_sock, PR_USER_UID_IGMPCTRL, MAX_PAYLOAD))
	{
		printf("Register Packet Redirect Fail.\n");
		return SYS_ERR_FAILED;
	}
    return SYS_ERR_OK;
}

int main(int argc, char *argv[])
{
	int cmd = 1;
	char filename[50] = ""; // for initial '\0'

	/* global control resource init */
    osal_memset(&igmpCtrl, 0, sizeof(igmp_mld_control_t));

    SYS_ERR_CHK((SYS_ERR_OK != rtk_core_init()), SYS_ERR_FAILED);

    /* TBD for max port id definition */
    igmpCtrl.sys_max_port_num = (HAL_GET_CPU_PORT() + 1);

    igmpCtrl.sys_has_ip_mc_tbl = (SYS_ERR_FAILED == rsd_ipMcGroup_get() ? DISABLED : ENABLED);

    printf("igmpd is running:  sys_max_port_num =%u .....\n", igmpCtrl.sys_max_port_num);

    /* igmp and mld statistics resource init */
    SYS_ERR_CHK((SYS_ERR_OK != mcast_igmp_mld_statistics_init()), SYS_ERR_FAILED);

    /* set default value for global configuration */
	config_init_default();

	/* parser input parameters for user setting */
	while (cmd < argc)
	{
		if (!strncmp("-f", argv[cmd], strlen(argv[cmd])))
		{
			cmd++;
            SYS_ERR_CHK((strlen(argv[cmd]) > sizeof(filename)), SYS_ERR_FAILED);
			strncpy(filename, argv[cmd], strlen(argv[cmd]));
			cmd++;
		}
        else if (!strncmp("-s", argv[cmd], strlen(argv[cmd])))
        {
			cmd++;
            SYS_ERR_CHK((strlen(argv[cmd]) > sizeof(filename)), SYS_ERR_FAILED);
            if (!strncmp("OpenWrt", argv[cmd], strlen(argv[cmd])))
            {
                igmpCtrl.sdkMode = SDK_MODE_OPENWRT;
            }
            else
            {
                igmpCtrl.sdkMode = SDK_MODE_LUNA;
            }
			cmd++;
        }
        else
		{
			cmd++;
		}
	}

    /* set configuration by BOA MIB */
    SYS_ERR_CHK((SYS_ERR_OK != config_modify_by_sdk()), SYS_ERR_FAILED);

    /* set configuration by user configuration file */
    SYS_ERR_CHK((SYS_ERR_OK != userStatusConfigSet(filename)), SYS_ERR_FAILED);

	SYS_ERR_CHK((SYS_ERR_OK != mcast_init()), SYS_ERR_FAILED);

    /* start to receive control packets */
    SYS_ERR_CHK((SYS_ERR_OK != igmp_mld_recv_pkt()), SYS_ERR_FAILED);

	return SYS_ERR_OK;
}

