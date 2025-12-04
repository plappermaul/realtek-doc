/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
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
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : semphore relative API
 *
 */

/*
 * Include Files
 */
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <osal/sem.h>

osal_mutex_t
osal_sem_mutex_create(void)
{
    sem_t   *new_sem;
    int     ret;
    
    new_sem = malloc(sizeof(sem_t));
    ret = sem_init(new_sem, 0, 1);
    
    if ( 0 == ret){
	    return (osal_mutex_t) new_sem;
	} else {
	    free(new_sem);
	    return 0;
	}
}

void
osal_sem_mutex_destroy(osal_mutex_t mutex)
{
	if(mutex);
	
	return;    
}

int32
osal_sem_mutex_take(osal_mutex_t mutex, uint32 usec)
{
    
    if (mutex != 0){
        sem_wait((sem_t *)mutex);
    } else {
        return 1;
    }

    if(usec);

    return 0;
}

int32
osal_sem_mutex_give(osal_mutex_t mutex)
{
    if (mutex != 0){
        sem_post((sem_t *)mutex);
    } else {
        return 1;
    }
    return 0;
}

osal_sem_t
osal_sem_create(int32 init_num)
{
	if(init_num)
	    return 0;
	return 0;
}

int32
osal_sem_take(osal_sem_t sem, uint32 usec)
{
    if(sem || usec)
    	return 0;
    
	return 0;
}

int32
osal_sem_give(osal_sem_t sem)
{
    if(sem)
	    return 0;
	
    return 0;
	    
}
