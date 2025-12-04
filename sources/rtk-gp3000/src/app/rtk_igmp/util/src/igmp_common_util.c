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

/*
 * Include Files
 */
#include <util/inc/igmp_common_util.h>

static uint32 getip(char *ifname)
{
    int                 sid;
    struct ifreq        ifr;
    struct sockaddr_in  *ina = NULL;
    int                 i;
    uint32              ip = 0;

    SYS_PARAM_CHK((strlen(ifname) > sizeof(ifr.ifr_name)), 0);

    sid = socket (AF_INET, SOCK_DGRAM, 0);

    if (sid < 0)
    {
        return 0;
    }

    for (i = 0; i < IFNAMSIZ; i++)
    {
        ifr.ifr_name[i] = 0;
    }

    ina = (struct sockaddr_in *)&(ifr.ifr_ifru.ifru_addr);
    strncpy(ifr.ifr_name, ifname, strlen(ifname));
    memset(ina, 0, sizeof(*ina));
    ina->sin_family = AF_INET;

    if (-1 == ioctl(sid, SIOCGIFADDR, &ifr))
    {
        ip = 0;
    }
    else
    {
        ip = (long)(ina->sin_addr.s_addr);
    }

    close(sid);
    return ip;

}
static uint32 getnetmask(char *ifname)
{
    int                 sid = 0;
    struct ifreq        ifr;
    struct sockaddr_in  *ina = NULL;
    int                 i;
    unsigned int        mask = 0;

    SYS_PARAM_CHK((strlen(ifname) > sizeof(ifr.ifr_name)), 0);

    sid = socket (AF_INET, SOCK_DGRAM, 0);

    if (sid < 0)
    {
        return 0;
    }

    for (i = 0; i < IFNAMSIZ; i++)
    {
        ifr.ifr_name[i] = 0;
    }
    ina = (struct sockaddr_in *)&(ifr.ifr_ifru.ifru_netmask);
    strncpy(ifr.ifr_name, ifname, strlen(ifname));
    memset(ina, 0, sizeof(*ina));
    ina->sin_family = AF_INET;

    if (-1 == ioctl(sid, SIOCGIFNETMASK, &ifr))
    {
        mask = 0;
    }
    else
    {
        mask = (long)(ina->sin_addr.s_addr);
    }
    close(sid);
    return mask;
}

int32 igmp_util_ipAddr_get(char *devName, sys_ipv4_addr_t *pIp, sys_ipv4_addr_t *pNetmask)
{
    SYS_PARAM_CHK(((NULL == devName) || (NULL == pIp) ||
        (NULL == pNetmask)), SYS_ERR_NULL_POINTER);

    *pIp        = getip(devName);
    *pNetmask   = getnetmask(devName);

	return SYS_ERR_OK;
}

int32 igmp_util_mac_addr_get(char *devName, sys_mac_t *pMac)
{
    struct ifreq ifreq;
    int32 sock;

    SYS_PARAM_CHK(((NULL == devName) || (NULL == pMac)), SYS_ERR_NULL_POINTER);
    SYS_PARAM_CHK((strlen(devName) > sizeof(ifreq.ifr_name)), SYS_ERR_INPUT);

    memset(&ifreq, 0, sizeof(struct ifreq));

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return SYS_ERR_FAILED;
    }
    strncpy(ifreq.ifr_name, &devName[0], strlen(devName));

    if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        close(sock);
        return SYS_ERR_FAILED;
    }
    pMac->octet[0] = ifreq.ifr_hwaddr.sa_data[0];
    pMac->octet[1] = ifreq.ifr_hwaddr.sa_data[1];
    pMac->octet[2] = ifreq.ifr_hwaddr.sa_data[2];
    pMac->octet[3] = ifreq.ifr_hwaddr.sa_data[3];
    pMac->octet[4] = ifreq.ifr_hwaddr.sa_data[4];
    pMac->octet[5] = ifreq.ifr_hwaddr.sa_data[5];
    close(sock);
    return SYS_ERR_OK;
}

uint16 igmp_util_checksum_get(uint16 *addr, int32 len, uint16* pChksum)
{
    /* Compute Internet Checksum for "count" bytes
     *         beginning at location "addr".
     */
    register int32 sum = 0;
    uint16 *source = (uint16 *) addr;

    while (len > 1)
    {
        /*  This is the inner loop */
        sum += *source++;
        len -= 2;
    }

    /*  Add left-over byte, if any */
    if (len > 0)
    {
        /* Make sure that the left-over byte is added correctly both
         * with little and big endian hosts */
        uint16 tmp = 0;
        *(uint8 *) (&tmp) = * (uint8 *) source;
        sum += tmp;
    }
    /*  Fold 32-bit sum to 16 bits */
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    *pChksum = ~sum;

    return SYS_ERR_OK;
}

uint32 igmp_util_get_sdk_mib_value_by_name(char *pName, char *pVal)
{
	char				buffer[SDK_MIB_BUFFER_LEN];
	char				cmd[SDK_MIB_BUFFER_LEN];
    char                *p = NULL;
	FILE				*pFd = NULL;

	if (!pName)
		return SYS_ERR_INPUT;

	memset(buffer, 0, SDK_MIB_BUFFER_LEN);
	memset(cmd, 0, SDK_MIB_BUFFER_LEN);

    if (SDK_MODE_LUNA == igmpCtrl.sdkMode)
    {
        snprintf(cmd, sizeof(cmd), "%s get %s", sdk_parameters[igmpCtrl.sdkMode][0], pName);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "%s get rtkigmp.config.%s", sdk_parameters[igmpCtrl.sdkMode][0], pName);
    }

    if (NULL != (pFd = popen(cmd, "r")))
    {
        if (fgets(buffer, sizeof(buffer), pFd))
        {
            if (SDK_MODE_LUNA == igmpCtrl.sdkMode)
            {
                if (!(p = strstr(buffer, "=")))
                {
                    pclose(pFd);
                    return SYS_ERR_FAILED;
                }
                p++;
                strncpy(pVal, p, strlen(p));
                pVal[strlen(p) - 1] = '\0';
            }
            else
            {
                strncpy(pVal, buffer, strlen(buffer));
                pVal[strlen(buffer) - 1] = '\0';
            }
            pclose(pFd);
            return SYS_ERR_OK;
        }
        pclose(pFd);
    }
    return SYS_ERR_FAILED;
}

