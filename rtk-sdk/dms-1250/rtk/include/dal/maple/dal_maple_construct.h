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

#ifndef __DAL_MAPLE_CONSTRUCT_H__
#define __DAL_MAPLE_CONSTRUCT_H__

void dal_maple_construct_default_init(uint32 unit);
void dal_maple_construct_phyConfig_init(uint32 unit);
void dal_maple_construct_macConfig_init(uint32 unit);
void dal_maple_construct_serdesConfig_init(uint32 unit);
void dal_maple_construct_phyExceptSerdesFiber_powerOn(uint32 unit);
void dal_maple_construct_phy_reset(uint32 unit);
void dal_maple_construct_phy_powerOff(uint32 unit);
void dal_maple_construct_pollingPhy_disable(uint32 unit);
void dal_maple_construct_pollingPhy_enable(uint32 unit);

#endif //__DAL_MAPLE_CONSTRUCT_H__


