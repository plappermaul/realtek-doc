/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * 
 * * 
 * $Revision$
 * $Date$
 *
 * Purpose : the basic head files of the system
 *
 * Feature : Provide the GPON Mac Device Driver the basic head files of the system
 *
 */
#ifndef _GPON_BASIC_H_
#define _GPON_BASIC_H_

#ifdef __LINUX_KERNEL__
#include <dal/apollo/gpon/gpon_basic_kernel.h>
#endif

#ifdef __LINUX_USER__
#include <dal/apollo/gpon/gpon_basic_user.h>
#endif

#include <stdarg.h>
#endif /* _GPON_BASIC_H_ */
