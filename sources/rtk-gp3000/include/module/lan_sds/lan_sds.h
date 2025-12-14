/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
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
 *
 * $Revision:  $
 * $Date: $
 *
 * Purpose :
 *
 * Feature :
 *
 */

#ifndef __LAN_SDS_H__
#define __LAN_SDS_H__

#ifdef CONFIG_LAN_SDS_FEATURE
extern int lan_sds_mode_set(unsigned int mode);
extern int lan_sds_mode_get(unsigned int *mode);
#endif

#ifdef CONFIG_LAN_SDS1_FEATURE
extern int lan_sds1_mode_set(unsigned int mode);
extern int lan_sds1_mode_get(unsigned int *mode);
#endif

#endif
