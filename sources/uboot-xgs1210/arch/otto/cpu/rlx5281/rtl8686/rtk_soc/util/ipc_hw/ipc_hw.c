#include "ipc_hw.h"

uint32 hw_intr_handled(void)
{
	return (0x0 == REG(IPC_EVENT_TRIG_REG));
}

void hw_trigger_intr(void)
{
	REG(IPC_EVENT_TRIG_REG) = 0x1;
}

/* 
 * return 1: Lock
 * return 0: Lock fail
 */
int hw_mutex_lock(void)
{
	return REG(IPC_MUTEX_REG);
}

void hw_mutex_unlock(void)
{
	if( REG(IPC_MUTEX_OWNER_REG) & IPC_OWN_MASK )
		REG(IPC_MUTEX_REG) = 0;

	return;
}

