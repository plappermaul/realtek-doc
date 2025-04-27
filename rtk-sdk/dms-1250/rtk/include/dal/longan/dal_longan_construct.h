/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : The data for setting MAC Serdes parameters
 *
 * Feature : The data struct of  MAC Serdes parameters
 *
 */

#ifndef __DAL_LONGAN_CONSTRUCT_H__
#define __DAL_LONGAN_CONSTRUCT_H__

#include <dal/dal_construct.h>

void dal_longan_construct_default_init(uint32 unit);
void dal_longan_construct_phy_reset(uint32 unit);
void dal_longan_construct_macConfig_init(uint32 unit);
void dal_longan_construct_pollingPhy_enable(uint32 unit);
void dal_longan_construct_pollingPhy_disable(uint32 unit);
void dal_longan_construct_serdes_on(uint32 unit);
void dal_longan_construct_serdes_off(uint32 unit);
void dal_longan_construct_phyConfig_init(uint32 unit);
void dal_longan_construct_serdesConfig_init(uint32 unit);
void dal_longan_construct_MacPollingPhy_init(uint32 unit);
void dal_longan_construct_miscConfig_init(uint32 unit);

#endif //__DAL_LONGAN_CONSTRUCT_H__


