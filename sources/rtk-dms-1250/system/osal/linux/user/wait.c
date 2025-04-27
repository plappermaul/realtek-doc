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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : thread relative API
 *
 */

/*
 * Include Files
 */
#include <common/error.h>
#include <osal/wait.h>
#include <osal/sem.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      osal_wait_event
 * Description:
 *      Wait a event that osal_wake_up() sends.
 * Input:
 *      event      -
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
int32
osal_wait_event(osal_event_t event)
{
    osal_sem_take(event, OSAL_SEM_WAIT_FOREVER);
    return RT_ERR_OK;
}

/* Function Name:
 *      osal_wait_event
 * Description:
 *      Wake up a event queue.
 * Input:
 *      event      -
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
int32
osal_wake_up(osal_event_t event)
{
    osal_sem_give(event);
    return RT_ERR_OK;
}

/* Function Name:
 *      osal_wait_module_create
 * Description:
 *      Create a event structure.
 * Input:
 *      None
 * Output:
 *      event_id
 * Return:
 *
 * Note:
 *
 */
int32
osal_wait_module_create(osal_event_t *event_id)
{
    *event_id = osal_sem_create(0);
    return RT_ERR_OK;
}

/* Function Name:
 *      osal_wait_module_destroy
 * Description:
 *      Destroy the existed event structure.
 * Input:
 *      event_id
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
int32
osal_wait_module_destroy(osal_event_t event_id)
{
    osal_sem_destroy(event_id);
    return RT_ERR_OK;
}

/* Function Name:
 *      osal_wait_module_list
 * Description:
 *      List all event structure.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
void
osal_wait_module_list(void)
{
    return;
}


