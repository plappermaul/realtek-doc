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

#ifndef __DAL_CYPRESS_CONSTRUCT_H__
#define __DAL_CYPRESS_CONSTRUCT_H__

#include <dal/dal_construct.h>

void MacRegSetCheck(uint32 unit, uint32 reg,uint32 val);
void Serdes_patch_set_chk(uint32 unit, confcode_serdes_patch_t _in);
void Serdes_set(uint32 unit, uint32 _reg,uint32 _endBit,uint32 _startBit,uint32 _inVal);
void rtl839x_serdes_patch_init(uint32 unit);
void rtl839x_5x_serdes_patch_init(uint32 unit);
void rtl839x_serdesReset(const uint32 sds_num);
void rtl839x_93m_rst_sys(uint32 unit);
void rtl839x_53m_rst_sys(uint32 unit);
void rtl8390_serdes_eee_enable(uint32 unit, const int sds_num);
int  rtl8390_serdes_chk(uint32 unit, const int sdsId);
void dal_cypress_construct_default_init(uint32 unit);
void dal_cypress_construct_phyConfig_init(uint32 unit);
void dal_cypress_construct_serdesConfig_init(uint32 unit);
void rtl8390_sfp_speed_set(uint32 unit, int port, int speed);
void dal_cypress_construct_phy_reset(uint32 unit);

#endif //__DAL_CYPRESS_CONSTRUCT_H__


