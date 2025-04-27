/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_INIT_H__
#define __RTRPC_INIT_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>

#ifdef RTUSR
  #define rtk_init                                         rtrpc_init
#endif //RTUSR
/*
 * Function Declaration
 */

extern int32
rtrpc_init(void);

#endif /* __RTRPC_INIT_H__ */
