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
#include "feature_export_api.h"

uint32 mcast_ipc_set(va_list argp)
{
	int                 sockfd;
	struct sockaddr_un  addr;
	ipcMsg_t            msg;

    mcast_msgType_t  msgType = va_arg(argp, mcast_msgType_t);
    void *pVal = va_arg(argp, void *);
    unsigned int valLen = va_arg(argp, unsigned int);
    va_end( argp);

	memset(&msg, 0, sizeof(ipcMsg_t));

	msg.msgType  = msgType;

    if (pVal)
    {
		memcpy(msg.data, pVal, valLen);
    }

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		printf("socket create error: %s\n", strerror(errno));
		return SYS_ERR_FAILED;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
	{
		//printf("connect error: %s\n", strerror(errno));
		close(sockfd);
		return SYS_ERR_FAILED;
	}

	if ( -1 == (send(sockfd, &msg, sizeof(ipcMsg_t), 0)))
    {
        printf("send error: %s\n", strerror(errno));
    }

	close(sockfd);
	return SYS_ERR_OK;
}


uint32 mcast_ipc_get(va_list argp)
{
	int                 sockfd;
	struct sockaddr_un  addr;
	ipcMsg_t            msg;

    mcast_msgType_t  msgType = va_arg(argp, mcast_msgType_t);
    void *pVal = va_arg(argp, void *);
    unsigned int valLen = va_arg(argp, unsigned int);
    va_end( argp);

	memset(&msg, 0, sizeof(ipcMsg_t));

	msg.msgType  = msgType;
	memcpy(msg.data, pVal, valLen);

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		printf("socket create error: %s\n", strerror(errno));
		return SYS_ERR_FAILED;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);

	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
	{
		//printf("connect error: %s\n", strerror(errno));
		close(sockfd);
		return SYS_ERR_FAILED;
	}

	if ( -1 == (send(sockfd, &msg, sizeof(ipcMsg_t), 0)))
        printf("socket send error: %s\n", strerror(errno));

	if (-1 == (recv(sockfd, &msg, sizeof(ipcMsg_t), 0)))
        printf("socket recv error: %s\n", strerror(errno));

	memcpy(pVal, msg.data, valLen);

	close(sockfd);
	return SYS_ERR_OK;
}

uint32 mcast_shm_get(va_list argp)
{
	key_t   shmKey;
	size_t  shmSize;

    mcast_msgType_t  msgType = va_arg(argp, mcast_msgType_t);
    void **shm = va_arg(argp, void **);
    int *shmId = va_arg(argp, int *);
    va_end( argp);

	switch (msgType)
	{
		case MCAST_MSGTYPE_GROUPENTRY_GET:
			shmKey = SHM_KEY_GROUP_ARRAY;
			shmSize = SHM_SIZE_2048;
		    break;
		case MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET:
			shmKey = SHM_KEY_GROUP_ARRAY + MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET;
			shmSize = SHM_MAX_SIZE;
		    break;
		default:
			printf("not supported\n");
			return SYS_ERR_FAILED;
	}

	if ((*shmId = shmget(shmKey, shmSize, 0666)) < 0)
    {
        printf("shmget failed\n");
  		return SYS_ERR_FAILED;
    }

    if ((*shm = shmat(*shmId, (void *)0, 0)) == (void *)-1)
    {
        printf("shmat failed\n");
  		return SYS_ERR_FAILED;
    }

    return SYS_ERR_OK;
}

uint32 mcast_shm_destroy(va_list argp)
{
    int shmId = va_arg(argp, int);
    void *shm = va_arg(argp, void *);
    va_end( argp);
	if (-1 == shmdt(shm))
	{
		printf("shmdt failed\n");
  		return SYS_ERR_FAILED;
	}

    if (-1 == shmctl(shmId, IPC_RMID, NULL))
    {
		printf("shmctl failed\n");
  		return SYS_ERR_FAILED;
	}

	return SYS_ERR_OK;
}

/* each shared library need to define feature_module_init if APIs in the shared library */
uint32 feature_module_init(uint32 moduleID)
{
	if (FAL_OK != feature_api_register(FEATURE_API_MCAST_RTK_IPC_SET, moduleID, &mcast_ipc_set))
        return FAL_FAIL;
	if (FAL_OK != feature_api_register(FEATURE_API_MCAST_RTK_IPC_GET, moduleID, &mcast_ipc_get))
        return FAL_FAIL;
	if (FAL_OK != feature_api_register(FEATURE_API_MCAST_RTK_IPC_CREATE,  moduleID, &mcast_shm_get))
        return FAL_FAIL;
	if (FAL_OK != feature_api_register(FEATURE_API_MCAST_RTK_IPC_DESTROY, moduleID, &mcast_shm_destroy))
        return FAL_FAIL;

    return FAL_OK;
}

uint32 feature_module_exit(void)
{
	feature_api_unregister(FEATURE_API_MCAST_RTK_IPC_SET);
	feature_api_unregister(FEATURE_API_MCAST_RTK_IPC_GET);
	feature_api_unregister(FEATURE_API_MCAST_RTK_IPC_CREATE);
	feature_api_unregister(FEATURE_API_MCAST_RTK_IPC_DESTROY);

    return FAL_OK;
}


