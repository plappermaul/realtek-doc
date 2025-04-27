#ifndef __RTK_DEBUG_H__
#define __RTK_DEBUG_H__

/*
 * Include Files
 */
#include <rtk_osal.h>


/*
 * Symbol Definition
 */
#define DBG_LEVEL   0   /*-1 means turning off debug*/


/*
 * Macro Definition
 */
#define DBG_PRINT(level, x...)  do { if (level <= DBG_LEVEL) OSAL_PRINTF("[DBG] " x); } while(0)


#endif/*__RTK_DEBUG_H__*/

