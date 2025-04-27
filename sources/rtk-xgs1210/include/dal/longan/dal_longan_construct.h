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
 * $Revision: 96385 $
 * $Date: 2019-04-17 15:33:41 +0800 (Wed, 17 Apr 2019) $
 *
 * Purpose : The data for setting MAC Serdes parameters
 *
 * Feature : The data struct of  MAC Serdes parameters
 *
 */

#ifndef __DAL_LONGAN_CONSTRUCT_H__
#define __DAL_LONGAN_CONSTRUCT_H__

#include <dal/dal_construct.h>

typedef struct regActCtrl_s
{
	uint32	 reg;
	uint32	 field;
	uint32	 actValue;
}regActCtrl_t;

typedef struct fcRegActCtrl_s
{
    uint32   reg;
    uint32   field;
    uint32   actValue[3];   /*0 for 24G+4*10G, 1 for 8*10G, 2 for 48G stacking*/
}fcRegActCtrl_t;

typedef struct fcRegIdxActCtrl_s
{
    uint32  idx;
    fcRegActCtrl_t    regAct;
}fcRegIdxActCtrl_t;


void dal_longan_construct_default_init(uint32 unit);
void dal_longan_construct_phy_reset(uint32 unit);
void dal_longan_construct_macConfig_init(uint32 unit);
void dal_longan_construct_pollingPhy_enable(uint32 unit);
void dal_longan_construct_pollingPhy_disable(uint32 unit);
void dal_longan_construct_serdes_on(uint32 unit);
void dal_longan_construct_serdes_off(uint32 unit);
void dal_longan_construct_phyConfig_init(uint32 unit);
void dal_longan_construct_serdesConfig_init(uint32 unit);
void dal_longan_construct_miscConfig_init(uint32 unit);

#endif //__DAL_LONGAN_CONSTRUCT_H__


