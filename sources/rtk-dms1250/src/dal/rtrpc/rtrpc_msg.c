/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * $Revision: 76201 $
 * $Date: 2017-03-07 11:20:06 +0800 (Tue, 07 Mar 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RTK API user and kernel communication utility
 *
 */

#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <osal/print.h>
#include <osal/time.h>
#include <osal/memory.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <dal/rtrpc/rtrpc_msg.h>

#define RTUSR_MSG_VARPTR(_p)        (char *)_p + sizeof(rtdrv_msgHdr_t)

rtrpc_pktTx_f               rtrpc_pkt_tx=NULL;
rtrpc_unitId2remoteUnit_f   rtrpc_unitId2remoteUnit_translate=NULL;

void
rtrpc_txFunc_reg(rtrpc_pktTx_f f)
{
    rtrpc_pkt_tx = f;
}

void
rtrpc_unitIdTranslateFunc_reg(rtrpc_unitId2remoteUnit_f f)
{
    rtrpc_unitId2remoteUnit_translate = f;
}


#ifdef RTUSR
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


int
rtrpc_msg_set(uint32 unit, unsigned int optid, void *varptr, unsigned int size)
{
    int    ret=RT_ERR_OK;

    int             sockfd;
    int             len;
    char            *p;
    rtdrv_msgHdr_t  *pHdr;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_RTUSR, "socket error(%d) %s", errno, strerror(errno));
        return RT_ERR_SOCKET;
    }

    len = sizeof(rtdrv_msgHdr_t) + size;
    if ((p = osal_alloc(len)) == NULL)
    {
        close(sockfd);
        return RT_ERR_MEM_ALLOC;
    }
    pHdr = (rtdrv_msgHdr_t *)p;
    osal_memset(pHdr, 0, sizeof(rtdrv_msgHdr_t));
    osal_memcpy(RTUSR_MSG_VARPTR(p), varptr, size);
    if ((ret = setsockopt(sockfd, IPPROTO_IP, optid, (void *)p, (socklen_t)len)) != 0)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_RTUSR, "setsockopt error(%d) %s", errno, strerror(errno));
        osal_free(p);
        close(sockfd);
        return RT_ERR_SOCKET;
    }
    close(sockfd);

    /* some set operation will return values, so, copy the data back to varptr */
    osal_memcpy(varptr, RTUSR_MSG_VARPTR(p), size);
    ret = pHdr->ret_code;
    osal_free(p);

    return ret;
}


int
rtrpc_msg_get(uint32 unit, unsigned int optid, void *varptr, unsigned int size)
{
    int    ret=RT_ERR_OK;
    int             sockfd;
    int             len;
    char            *p;
    rtdrv_msgHdr_t  *pHdr;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_RTUSR, "socket error(%d) %s", errno, strerror(errno));
        return RT_ERR_SOCKET;
    }

    len = sizeof(rtdrv_msgHdr_t) + size;
    if ((p = osal_alloc(len)) == NULL)
    {
        close(sockfd);
        return RT_ERR_MEM_ALLOC;
    }
    pHdr = (rtdrv_msgHdr_t *)p;
    osal_memset(pHdr, 0, sizeof(rtdrv_msgHdr_t));
    osal_memcpy(RTUSR_MSG_VARPTR(p), varptr, size);
    if ((ret = getsockopt(sockfd, IPPROTO_IP, optid, (void *)p, (socklen_t *)&len)) != 0)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_RTUSR, "getsockopt error(%d) %s", errno, strerror(errno));
        osal_free(p);
        close(sockfd);
        return RT_ERR_SOCKET;
    }
    close(sockfd);

    osal_memcpy(varptr, RTUSR_MSG_VARPTR(p), size);
    ret = pHdr->ret_code;
    osal_free(p);

    return ret;
}

#else //RTUSR

int32
rtrpc_msg_set(uint32 unit, unsigned int optid, void *varptr, unsigned int size)
{
    int             ret=RT_ERR_OK;
    int             len;
    char            *p;
    rtdrv_msgHdr_t  *pHdr;

    len = sizeof(rtdrv_msgHdr_t) + size;
    if ((p = osal_alloc(len)) == NULL)
    {
        return RT_ERR_MEM_ALLOC;
    }
    pHdr = (rtdrv_msgHdr_t *)p;
    osal_memset(pHdr, 0, sizeof(rtdrv_msgHdr_t));
    osal_memcpy(RTUSR_MSG_VARPTR(p), varptr, size);

    pHdr->flag |= RTDRV_MSG_FLAG_SET;
    pHdr->optid = optid;
    pHdr->data_size = size;

    if(rtrpc_pkt_tx!=NULL)
    {
        if ((ret = rtrpc_pkt_tx(unit, p, len) != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, MOD_RTUSR, "rtrpc_pkt_tx error!");
            osal_free(p);
            return RT_ERR_SOCKET;
        }
    }

    /* some set operation will return values, so, copy the data back to varptr */
    osal_memcpy(varptr, RTUSR_MSG_VARPTR(p), size);
    ret = pHdr->ret_code;
    osal_free(p);

    return ret;
}


int32
rtrpc_msg_get(uint32 unit, unsigned int optid, void *varptr, unsigned int size)
{
    int    ret=RT_ERR_OK;
    int             len;
    char            *p;
    rtdrv_msgHdr_t  *pHdr;

    len = sizeof(rtdrv_msgHdr_t) + size;
    if ((p = osal_alloc(len)) == NULL)
    {
        return RT_ERR_MEM_ALLOC;
    }
    pHdr = (rtdrv_msgHdr_t *)p;
    osal_memset(pHdr, 0, sizeof(rtdrv_msgHdr_t));
    osal_memcpy(RTUSR_MSG_VARPTR(p), varptr, size);

    {
#ifdef USING_RTSTK_PKT_AS_RAIL

        pHdr->flag &= (~RTDRV_MSG_FLAG_SET);
        pHdr->optid = optid;

        if(rtrpc_pkt_tx!=NULL)
        {
            if ((ret = rtrpc_pkt_tx(unit, p, len) != RT_ERR_OK))
            {
                RT_LOG(LOG_MAJOR_ERR, MOD_RTUSR, "rtrpc_pkt_tx error!");
                osal_free(p);
                return RT_ERR_FAILED;
            }
        }
#else
        rail_cpuId_t pCpuId;
        uint8 pkt[100]={1};

        rtrpc_pkt_tx(&pCpuId,pkt,100,0,0);
#endif
    }

    osal_memcpy(varptr, RTUSR_MSG_VARPTR(p), size);
    ret = pHdr->ret_code;
    osal_free(p);

    return ret;

}

#endif //RTUSR

