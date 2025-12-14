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
 * $Revision:
 * $Date:
 *
 * Purpose :
 *
 * Feature :
 *
 */

#include "igmp_ipc.h"

static int32 igmp_cmd_set_acl_help(void)
{
   	printf("\nUsage: igmpcli set acl {ipType, aclType, index, aniVid, sip, start_dip, end_dip, grpBw,"\
           " previewLen, previewRepeatTime, previewRepeatCnt, previewReset}\n\n");
	printf("ipType              : IPV4 or IPV6\n");
	printf("                    [ipv4 | ipv6]\n");
	printf("aclType             : static acl or dynamic acl\n");
	printf("                    [s | d]\n");
	printf("index               : Acl entry ID\n");
	printf("                    [1...n]\n");
    printf("aniVid              : ANI VID\n");
	printf("                    [0-4095]\n");
    printf("sip                 : Source IP address \n");
	printf("                    [A.B.C.D | XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX]\n");
    printf("start_dip           : Start destination IP address \n");
	printf("                    [A.B.C.D | XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX]\n");
    printf("end_dip             : End destination IP address \n");
	printf("                    [A.B.C.D | XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX]\n");
    printf("grpBw               : Imputed group bandwidth \n");
	printf("                    [0...0xFFFFFFFF]\n");
    printf("previewLen          : Preview length \n");
	printf("                    [0...65535]\n");
    printf("previewRepeatTime   : Preview repeat time \n");
	printf("                    [0...65535]\n");
    printf("previewRepeatCnt    : Preview repeat count \n");
	printf("                    [0...65535]\n");
    printf("previewReset        : Preview reset time \n");
	printf("                    [0 | 1...24 | 25...240 | 241..254 | 255]\n");
	return 0;
}

static int32 igmp_cmd_get_acl_help(void)
{
   	printf("\nUsage: igmpcli set acl {ipType, aclType}\n\n");
	printf("ipType              : IPV4 or IPV6\n");
	printf("                    [ipv4 | ipv6]\n");
	printf("aclType             : static acl or dynamic acl\n");
	printf("                    [s | d]\n");
	return 0;
}

static int32 igmp_cmd_get_mc_filter_help(void)
{
   	printf("\nUsage: igmpcli get  pon_mac_mc_filter {ipType}\n\n");
	printf("ipType              : IPV4 or IPV6\n");
	printf("                    [ipv4 | ipv6]\n");
	return 0;
}


static int igmp_cmd_set_acl(int argc,char **argv)
{
	mcast_aclEntry_t aclEntry;

	if (argc < 11)
	{
		igmp_cmd_set_acl_help();
		return 0;
	}
    memset(&aclEntry, 0, sizeof(mcast_aclEntry_t));

	if (!strcmp(argv[0], "ipv4"))
    {
		aclEntry.ipType = MULTICAST_TYPE_IPV4;
	}
    else if (!strcmp(argv[0], "ipv6"))
    {
		aclEntry.ipType = MULTICAST_TYPE_IPV6;
	}
    else
    {
		igmp_cmd_set_acl_help();
		return 0;
	}

	if (!strcmp(argv[1], "s"))
	{
	    aclEntry.aclType = IGMP_GROUP_ENTRY_STATIC;
	}
    else if (!strcmp(argv[1], "d"))
	{
	    aclEntry.aclType = IGMP_GROUP_ENTRY_DYNAMIC;
	}
    else
    {
		igmp_cmd_set_acl_help();
		return 0;
	}
    struct in_addr addr;
	aclEntry.aclEntry.id                = atoi(argv[2]);
	aclEntry.aclEntry.aniVid            = atoi(argv[3]);
    inet_aton(argv[4], &addr);
	aclEntry.aclEntry.sip.ipv4          = addr.s_addr;
    inet_aton(argv[5], &addr);
	aclEntry.aclEntry.dipStart.ipv4     = addr.s_addr;
    inet_aton(argv[6], &addr);
	aclEntry.aclEntry.dipEnd.ipv4       = addr.s_addr;
	aclEntry.aclEntry.imputedGrpBW      = atoi(argv[7]);
	aclEntry.aclEntry.previewLen        = atoi(argv[8]);
	aclEntry.aclEntry.previewRepeatTime = atoi(argv[9]);
	aclEntry.aclEntry.previewRepeatCnt  = atoi(argv[10]);
	aclEntry.aclEntry.previewReset      = atoi(argv[11]);

    /* prepare enough memory */
    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_aclEntry_t) + sizeof(unsigned int));
    char* bm = m; /* copies the pointer */

    /*puts the first value */
    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_ACLENTRY_SET;
    m += sizeof(mcast_msgType_t); /* move forward the pointer to the next element */

    /* puts the next value */
    (*(mcast_aclEntry_t **)m) = &aclEntry;
    m += sizeof(mcast_aclEntry_t *); /* move forward again*/

    /* puts the third element */
    (*(unsigned int *)m) = sizeof(mcast_aclEntry_t);
    m += sizeof(unsigned int); /* unneeded, but here for clarity. */

	//mcast_ipc_set(MCAST_MSGTYPE_ACLENTRY_SET, &aclEntry, sizeof(mcast_aclEntry_t));
	mcast_ipc_set(bm);
    free(bm);
	return 0;
}

static int igmp_cmd_get_mc_filter(int argc, char **argv)
{

	mcast_filter_mc_on_pon_entry_t filterMcEntry;

	if (argc < 1)
	{
		igmp_cmd_get_mc_filter_help();
		return 0;
	}

	if (!strcmp(argv[0], "ipv4"))
    {
		filterMcEntry.ipType = MULTICAST_TYPE_IPV4;
	}
    else if (!strcmp(argv[0], "ipv6"))
	{
	    filterMcEntry.ipType = MULTICAST_TYPE_IPV6;
	}
    else
    {
		igmp_cmd_get_mc_filter_help();
		return 0;
	}


    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_filter_mc_on_pon_entry_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_PRINT;
    m += sizeof(mcast_msgType_t);

    (*(mcast_filter_mc_on_pon_entry_t **)m) = &filterMcEntry;
    m += sizeof(mcast_aclEntry_t *);

    (*(unsigned int *)m) = sizeof(mcast_filter_mc_on_pon_entry_t);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;
}

static int igmp_cmd_get_acl(int argc,char **argv)
{
	mcast_aclEntry_t aclEntry;

	if (argc < 2)
	{
		igmp_cmd_get_acl_help();
		return 0;
	}

	if (!strcmp(argv[0], "ipv4"))
    {
		aclEntry.ipType = MULTICAST_TYPE_IPV4;
	}
    else if (!strcmp(argv[0], "ipv6"))
	{
	    aclEntry.ipType = MULTICAST_TYPE_IPV6;
	}
    else
    {
		igmp_cmd_get_acl_help();
		return 0;
	}

	if (!strcmp(argv[1], "s"))
	{
	    aclEntry.aclType = IGMP_GROUP_ENTRY_STATIC;
	}
    else if (!strcmp(argv[1], "d"))
	{
	    aclEntry.aclType = IGMP_GROUP_ENTRY_DYNAMIC;
	}
    else
    {
		igmp_cmd_get_acl_help();
		return 0;
	}


    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_aclEntry_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_ACLENTRY_PRINT;
    m += sizeof(mcast_msgType_t);

    (*(mcast_aclEntry_t **)m) = &aclEntry;
    m += sizeof(mcast_aclEntry_t *);

    (*(unsigned int *)m) = sizeof(mcast_aclEntry_t);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;
}


static int32 igmp_cmd_get_profile_help(void)
{
   	printf("\nUsage: igmpcli get pf {ipType, aclType, portId}\n\n");
	printf("ipType      : IPV4 or IPV6\n");
	printf("            [ipv4 | ipv6]\n");
	printf("aclType     : Static acl or dynamic acl\n");
	printf("            [s | d]\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	return 0;
}

static int32 igmp_cmd_set_profile_help(void)
{
   	printf("\nUsage: igmpcli set pf {ipType, aclType, portId, aclId}\n\n");
	printf("ipType      : IPV4 or IPV6\n");
	printf("            [ipv4 | ipv6]\n");
	printf("aclType     : Static acl or dynamic acl\n");
	printf("            [s | d]\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	printf("aclId       : Acl entry ID\n");
	printf("            [0...n]\n");
	return 0;
}

static int32 igmp_cmd_get_port_cfg_help(void)
{
   	printf("\nUsage: igmpcli get port_cfg {portId}\n\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	return 0;
}

static int igmp_cmd_set_profile(int argc,char **argv)
{
	mcast_mcProfile_t profile;

	if (argc < 2)
	{
		igmp_cmd_set_profile_help();
		return 0;
	}

	if (!strcmp(argv[0], "ipv4"))
	{
	    profile.ipType = MULTICAST_TYPE_IPV4;
	}
    else if (!strcmp(argv[0], "ipv6"))
	{
	    profile.ipType = MULTICAST_TYPE_IPV6;
	}
    else
    {
		igmp_cmd_set_profile_help();
		return 0;
	}

	if (!strcmp(argv[1], "s"))
	{
	    profile.aclType = IGMP_GROUP_ENTRY_STATIC;
	}
    else if (!strcmp(argv[1],"d"))
	{
	    profile.aclType = IGMP_GROUP_ENTRY_DYNAMIC;
	}
    else
	{
		igmp_cmd_set_profile_help();
		return 0;
	}
	profile.port = atoi(argv[2]);
	profile.aclEntryId = atoi(argv[3]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_mcProfile_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_PROFILE_SET;
    m += sizeof(mcast_msgType_t);

    (*(mcast_mcProfile_t **)m) = &profile;
    m += sizeof(mcast_mcProfile_t *);

    (*(unsigned int *)m) = sizeof(mcast_mcProfile_t);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;
}

static int igmp_cmd_get_profile(int argc,char **argv)
{
	mcast_mcProfile_t profile;

	if (argc < 3)
	{
		igmp_cmd_get_profile_help();
		return 0;
	}

	if (!strcmp(argv[0], "ipv4"))
	{
	    profile.ipType = MULTICAST_TYPE_IPV4;
	}
    else if (!strcmp(argv[0], "ipv6"))
	{
	    profile.ipType = MULTICAST_TYPE_IPV6;
	}
    else
    {
		igmp_cmd_get_profile_help();
		return 0;
	}

	if (!strcmp(argv[1], "s"))
	{
	    profile.aclType = IGMP_GROUP_ENTRY_STATIC;
	}
    else if(!strcmp(argv[1], "d"))
	{
	    profile.aclType = IGMP_GROUP_ENTRY_DYNAMIC;
	}
    else
	{
		igmp_cmd_get_profile_help();
		return 0;
	}

	profile.port = atoi(argv[2]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_mcProfile_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_PROFILE_PRINT;
    m += sizeof(mcast_msgType_t);

    (*(mcast_mcProfile_t **)m) = &profile;
    m += sizeof(mcast_mcProfile_t *);

    (*(unsigned int *)m) = sizeof(mcast_mcProfile_t);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;
}

static int igmp_cmd_get_port_cfg(int argc,char **argv)
{
	unsigned int port;
	if (argc < 1)
	{
		igmp_cmd_get_port_cfg_help();
		return 0;
	}
	port = atoi(argv[0]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(unsigned int) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_PORT_CFG_PRINT;
    m += sizeof(mcast_msgType_t);

    (*(unsigned int **)m) = &port;
    m += sizeof(unsigned int *);

    (*(unsigned int *)m) = sizeof(port);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);

	return 0;

}

static int32 igmp_cmd_set_rateLimit_help(void)
{
   	printf("\nUsage: igmpcli set rate {portId, rate}\n\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	printf("rate        : rate in second\n");
	printf("            [0...4294967295]\n");
	return 0;
}

static int igmp_cmd_set_rateLimit(int argc,char **argv)
{
	mcast_igmpRateLimit_t rateLimit;

	if (argc < 2)
	{
		igmp_cmd_set_rateLimit_help();
		return 0;
	}

	rateLimit.port = atoi(argv[0]);
	rateLimit.packetRate = atoi(argv[1]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_igmpRateLimit_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_IGMP_RATELIMIT_SET;
    m += sizeof(mcast_msgType_t);

    (*(mcast_igmpRateLimit_t **)m) = &rateLimit;
    m += sizeof(mcast_igmpRateLimit_t *);

    (*(unsigned int *)m) = sizeof(rateLimit);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;
}

static int32 igmp_cmd_set_fastLeave_help(void)
{
   	printf("\nUsage: igmpcli set fast_leave {portId, enable}\n\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	printf("enable      : disable or enable\n");
	printf("            [0 | 1]\n");
	return 0;
}

int igmp_cmd_set_fastLeave(int argc, char **argv)
{
	mcast_portFastLeaveMode_t fastLeaveModeVal;

	if (argc < 2)
	{
		igmp_cmd_set_fastLeave_help();
		return 0;
	}
	memset(&fastLeaveModeVal, 0, sizeof(mcast_portFastLeaveMode_t));
	//check argv[0]
	fastLeaveModeVal.port = atoi(argv[0]);
	//check argv[1]
	fastLeaveModeVal.enable = atoi(argv[1]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_portFastLeaveMode_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_FASTLEAVE_MODE_SET;
    m += sizeof(mcast_msgType_t);

    (*(mcast_portFastLeaveMode_t **)m) = &fastLeaveModeVal;
    m += sizeof(mcast_portFastLeaveMode_t *);

    (*(unsigned int *)m) = sizeof(fastLeaveModeVal);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;
}

static int32 igmp_cmd_set_log_help(void)
{
	printf("\nUsage: igmpcli set log {enable}\n\n");
	printf("enable      : turn on or turn off\n");
	printf("            [on | off]\n");
	return 0;
}

static int igmp_cmd_set_log(int argc, char **argv)
{
	uint32 logVal;

	if (argc < 1)
	{
		igmp_cmd_set_log_help();
		return 0;
	}

	if (0 == strncmp(argv[0], "off", strlen("off")))
	{
	    logVal = 0;
	}
    else if (0 == strncmp(argv[0], "on", strlen("on")))
	{
	    logVal = 1;
    }

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(uint32) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_IGMP_LOG_SET;
    m += sizeof(mcast_msgType_t);

    (*(uint32 **)m) = &logVal;
    m += sizeof(uint32 *);

    (*(unsigned int *)m) = sizeof(logVal);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);

	return 0;
}

static int32 igmp_cmd_get_allowed_preview_help(void)
{
   	printf("\nUsage: igmpcli get allowed_preview {portId}\n\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	return 0;
}

int igmp_cmd_get_allowed_preview_by_port(int argc,char **argv)
{
	unsigned int port;

	if (argc < 1)
	{
		igmp_cmd_get_allowed_preview_help();
		return 0;
	}
	port = atoi(argv[0]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(unsigned int) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_ALLOWED_PREVIEW_PRINT;
    m += sizeof(mcast_msgType_t);

    (*(unsigned int **)m) = &port;
    m += sizeof(unsigned int *);

    (*(unsigned int *)m) = sizeof(port);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;

}

static int32 igmp_cmd_set_help(void)
{
   	printf("\nUsage: igmpcli set [cmd]\n\n");
	printf("acl         : Set ACL netry of IPv4/IPV6 and static/dynamic\n");
	printf("pf          : Set acl entry to profile of IPv4/IPV6 per port\n");
	printf("rate        : Set rate limiting per port\n");
	printf("fast_leave  : Set fast leave per port\n");
	printf("log         : Set turn on or off log\n");
	return 0;
}
static int32 igmp_cmd_set(int argc,char **argv)
{
	if (argc <= 0)
	{
		igmp_cmd_set_help();
		return 0;
	}

	if (!strcmp(argv[0], "acl"))
	{
		igmp_cmd_set_acl(argc-1, &argv[1]);
	}
    else if (!strcmp(argv[0], "pf"))
	{
		igmp_cmd_set_profile(argc-1, &argv[1]);
	}
    else if (!strcmp(argv[0], "rate"))
	{
		igmp_cmd_set_rateLimit(argc-1, &argv[1]);
	}
    else if (!strcmp(argv[0], "fast_leave"))
	{
		igmp_cmd_set_fastLeave(argc-1, &argv[1]);
	}
    else if (!strcmp(argv[0], "log"))
	{
		igmp_cmd_set_log(argc-1, &argv[1]);
	}
	else
	{
		igmp_cmd_set_help();
	}
	return 0;
}

static int32 igmp_cmd_get_help(void)
{
    //cfg
   	printf("\nUsage: igmpcli get [cmd]\n\n");
	printf("global_cfg              : Get global configuration\n");
    printf("mcast_profile_cfg       : Get multicast profile information\n");
	printf("acl                     : Get ACL netry of IPv4/IPV6 and static/dynamic\n");
    printf("pf                      : Get profile of IPv4/IPV6 per port\n");
    printf("port_cfg                : Get omci configuration per port\n");
	printf("allowed_preview         : Get allowed preview configuration per port\n");
    printf("pon_mac_mc_filter       : Get multicast filter on pon mac\n");
	return 0;
}

static int32 igmp_cmd_get(int argc,char **argv)
{
	if (argc <= 0)
	{
		igmp_cmd_get_help();
		return 0;
	}

	if (!strcmp(argv[0], "acl"))
	{
		igmp_cmd_get_acl(argc - 1, &argv[1]);
	}
    else if (!strcmp(argv[0], "pf"))
	{
		igmp_cmd_get_profile(argc - 1, &argv[1]);
	}
    else if (!strcmp(argv[0], "port_cfg"))
	{
		igmp_cmd_get_port_cfg(argc - 1, &argv[1]);
	}
	else if (!strcmp(argv[0], "global_cfg"))
	{
	    void *ptr = NULL;
        char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(void *) + sizeof(unsigned int));
        char* bm = m;

        (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_GLB_CFG_PRINT;
        m += sizeof(mcast_msgType_t);

        (*(void **)m) = &ptr;
        m += sizeof(void *);

        (*(unsigned int *)m) = 0;
        m += sizeof(unsigned int);

        mcast_ipc_set(bm);
        free(bm);
	}
    else if (!strcmp(argv[0], "mcast_profile_cfg"))
	{

	    void *ptr = NULL;

        char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(void *) + sizeof(unsigned int));
        char* bm = m;

        (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_MCASTPROF_PRINT;
        m += sizeof(mcast_msgType_t);

        (*(void **)m) = &ptr;
        m += sizeof(void *);

        (*(unsigned int *)m) = 0;
        m += sizeof(unsigned int);

        mcast_ipc_set(bm);
        free(bm);
	}
	else if (!strcmp(argv[0], "allowed_preview"))
	{
		igmp_cmd_get_allowed_preview_by_port(argc - 1, &argv[1]);
	}
    else if (!strcmp(argv[0], "pon_mac_mc_filter"))
    {
        igmp_cmd_get_mc_filter(argc - 1, &argv[1]);
    }
	else
	{
		igmp_cmd_get_help();
	}
	return 0;
}

static int32 igmp_cmd_show_port_stat_help(void)
{
   	printf("\nUsage: igmpcli show port_stat {portId}\n\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	return 0;
}

static int32 igmp_cmd_show_counter_help(void)
{
	printf("\nUsage: igmpcli show cnt {portId}\n\n");
	printf("portId      : LAN port ID\n");
	printf("            [1...n]\n");
	return 0;
}

static int igmp_cmd_show_counter(int argc,char **argv)
{
	mcast_counter_t counter;

	if (argc < 1)
	{
		igmp_cmd_show_counter_help();
		return 0;
	}

	counter.port = atoi(argv[0]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(mcast_counter_t) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_COUNTER_GET;
    m += sizeof(mcast_msgType_t);


    (*(mcast_counter_t **)m) = &counter;
    m += sizeof(mcast_counter_t *);

    (*(unsigned int *)m) = sizeof(mcast_counter_t);
    m += sizeof(unsigned int);

	mcast_ipc_get(bm);
    free(bm);
    printf("%s", PRINT_LINE);
	printf("port [%d] rx counter\n", counter.port);
    printf("%s", PRINT_LINE);
	printf("US IGMP Rx              : %d\n", counter.igmpUsRx);
	printf("US IGMP Rx drop         : %d\n", counter.igmpUsRxDrop);
	return 0;
}

static int igmp_cmd_show_port_stat(int argc,char **argv)
{
	unsigned int port;

	if (argc < 1)
	{
		igmp_cmd_show_port_stat_help();
		return 0;
	}
	port = atoi(argv[0]);

    char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(unsigned int) + sizeof(unsigned int));
    char* bm = m;

    (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_PORT_STAT_PRINT;
    m += sizeof(mcast_msgType_t);

    (*(unsigned int **)m) = &port;
    m += sizeof(unsigned int *);

    (*(unsigned int *)m) = sizeof(port);
    m += sizeof(unsigned int);

	mcast_ipc_set(bm);
    free(bm);
	return 0;

}

static int32 igmp_cmd_show_help(void)
{
    //status
   	printf("\nUsage: igmpcli show [cmd]\n\n");
	printf("active_group        : Show active group information\n");
	printf("preview_timer       : Show timer file descriptor of current preview group\n");
	printf("cnt                 : Show igmp control counters per port\n");
	printf("port_stat           : Show statistics per port\n");
	return 0;
}

static int32 igmp_cmd_show(int argc,char **argv)
{
	if (argc <= 0)
	{
		igmp_cmd_show_help();
		return 0;
	}

	if (!strcmp(argv[0], "cnt"))
	{
		igmp_cmd_show_counter(argc - 1, &argv[1]);
	}
    else if (!strcmp(argv[0], "active_group"))
	{
	    void *ptr = NULL;

        char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(void *) + sizeof(unsigned int));
        char* bm = m;

        (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_ACTIVE_GRP_SHOW;
        m += sizeof(mcast_msgType_t);

        (*(void **)m) = &ptr;
        m += sizeof(void *);

        (*(unsigned int *)m) = 0;
        m += sizeof(unsigned int);

        mcast_ipc_set(bm);
        free(bm);
	}
	else if (!strcmp(argv[0], "preview_timer"))
	{
	    void *ptr = NULL;

        char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(void *) + sizeof(unsigned int));
        char* bm = m;

        (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_PREVIEW_TIMER_SHOW;
        m += sizeof(mcast_msgType_t);

        (*(void **)m) = &ptr;
        m += sizeof(void *);

        (*(unsigned int *)m) = 0;
        m += sizeof(unsigned int);

		mcast_ipc_set(bm);
        free(bm);
	}
    else if (!strcmp(argv[0], "port_stat"))
	{
		igmp_cmd_show_port_stat(argc - 1, &argv[1]);
	}
	else
	{
		igmp_cmd_show_help();
	}
	return 0;
}

static int32 igmp_cmd_debug_help(void)
{
    //internal global variables
   	printf("\nUsage: igmpcli debug [cmd]\n\n");
	printf("info        : Show debug information\n");
	return 0;
}

static int32 igmp_cmd_debug(int argc,char **argv)
{
	if (argc <= 0)
	{
		igmp_cmd_debug_help();
		return 0;
	}

	if (!strcmp(argv[0], "info"))
	{
	    void *ptr = NULL;
        char* m = (char *)malloc(sizeof(mcast_msgType_t) + sizeof(void *) + sizeof(unsigned int));
        char* bm = m;

        (*(mcast_msgType_t *)m) = MCAST_MSGTYPE_INFO_DEBUG;
        m += sizeof(mcast_msgType_t);

        (*(void **)m) = &ptr;
        m += sizeof(void *);

        (*(unsigned int *)m) = 0;
        m += sizeof(unsigned int);

        mcast_ipc_set(bm);
        free(bm);
	}
	else
	{
		igmp_cmd_debug_help();
	}
	return 0;
}


static int32 cmd_help(void)
{
   	printf("\nUsage: igmpcli [cmd]\n\n");
	printf("set             : set series command\n");
	printf("get             : get series command\n");
    printf("show            : show series command\n");
    printf("debug           : debug series command\n");
	printf("\n");
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cmd_help();
		return 0;
	}

	if (!strcmp(argv[1], "set"))
	{
		igmp_cmd_set(argc - 2, &argv[2]);
	}
	else if (!strcmp(argv[1], "get"))
	{
		igmp_cmd_get(argc - 2, &argv[2]);
	}
    else if (!strcmp(argv[1], "show"))
    {
        igmp_cmd_show(argc - 2, &argv[2]);
    }
    else if (!strcmp(argv[1], "debug"))
    {
        igmp_cmd_debug(argc - 2, &argv[2]);
    }
	else if (!strcmp(argv[1], "-v"))
	{
		#ifdef CONFIG_SVN_VERSION
			printf("IGMP SVN revision: %s\n", CONFIG_SVN_VERSION);
		#endif
	}

	return 0;
}

