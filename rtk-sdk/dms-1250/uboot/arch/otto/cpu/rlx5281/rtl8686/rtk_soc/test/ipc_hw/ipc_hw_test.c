#include <common.h>
#include <exports.h>
#include "ipc_hw.h"

#define IPC_POLLING_LIMIT (100)

/* Basic function test: IPC lock/unlock */
int _ipc_test1(void)
{
	int lock_status;
	uint32 lock_time;

	lock_status = IPC_UNLOCKED;
	lock_time = 0;

	printf("lock mutex");
	/* lock IPC */
	while(lock_status==IPC_UNLOCKED){
		lock_status = hw_mutex_lock();
		if(lock_time > IPC_POLLING_LIMIT){
			printf("test case %s:%d lock fail\n", __FUNCTION__, __LINE__);
			printf("IPC_MUTEX_REG(0x%08x):0x%08x\n", IPC_MUTEX_REG, REG(IPC_MUTEX_REG));
			return -1;			
		}
		lock_time++;
	}

	/* check mutex owner */
	printf("\rcheck mutex owner 1");
	if( REG(IPC_MUTEX_OWNER_REG) & IPC_OWN_MASK ){
		printf("test case %s:%d check mutex owner fail\n", __FUNCTION__, __LINE__);
		printf("IPC_MUTEX_OWNER_REG(0x%08x):0x%08x\n", IPC_MUTEX_OWNER_REG, REG(IPC_MUTEX_OWNER_REG));
		return -1;
	}
	
	/* unlock IPC */
	printf("\runlock mutex");
	hw_mutex_unlock();
	
	/* check mutex owner */
	printf("\rcheck mutex owner 2");
	if( REG(IPC_MUTEX_OWNER_REG) & IPC_OWN_MASK ){
		printf("test case %s:%d mutex unlock fail\n", __FUNCTION__, __LINE__);
		printf("IPC_MUTEX_OWNER_REG(0x%08x):0x%08x\n", IPC_MUTEX_OWNER_REG, REG(IPC_MUTEX_OWNER_REG));
		return -1;
	}

	printf("\r%s : passed\n", __FUNCTION__);
	return 0;
}

int ipc_test(int flag, int argc, char *argv[])
{
	printf("start ipc test\n");
	if(0 != _ipc_test1())
		return -1;
	
	printf("%s : passed\n", __FUNCTION__);
	return 0;
}

