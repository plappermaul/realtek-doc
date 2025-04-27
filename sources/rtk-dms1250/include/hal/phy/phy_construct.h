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
 * Purpose : Set the parameters of PHY/MAC to chips.
 *
 * Feature : Structure and APIs for parameters of PHY/MAC settings
 *
 */

#ifndef __PHY_CONSTRUCT_H__
#define __PHY_CONSTRUCT_H__

void rtl8208_rtl8380_config(uint32 unit, uint8 phyid);
void rtl8214fb_rtl8380_config(uint32 unit, uint8 phyid);
void rtl8208_rtl8390_config(uint32 unit, uint8 phyid);
void rtl8214fb_rtl8390_config(uint32 unit, uint8 phyid);
void rtl8214fc_rtl8380_config(uint32 unit, uint8 phyid);
void rtl8214fc_rtl8390_config(uint32 unit, uint8 macId);
void rtl8214fc_rtl8390_post_config(uint32 unit);
void rtl8214fc_rtl9300_config(uint32 unit, uint8 phyid);
void rtl8218b_rtl8380_config(uint32 unit, uint8 phyid);
void rtl8218d_rtl8380_config(uint32 unit, uint8 phyid);
void rtl8218b_rtl8390_config(uint32 unit, uint8 macId);
void rtl8218fb_rtl8380_config(uint32 unit, uint8 phyid);
void rtl8218fb_rtl8390_config(uint32 unit, uint8 macId);
void construct_rtl8214fb_phyPowerOn(uint32 unit, rtk_port_t portid);
void construct_rtl8214fb_phyPowerOff(uint32 unit,rtk_port_t portid);
void rtl8214c_config(uint32 unit, uint8 macId);
void rtl8218b_rtl9300_config(uint32 unit, uint8 phyid);
void rtl8218d_config(uint32 unit, uint8 phyid);
void rtl8295R_config(uint32 unit, uint8 baseMacId);
void rtl8214QF_config(uint32 unit, uint8 baseMacId);
void rtl8224QF_config(uint32 unit, uint8 baseMacId);
void rtl8218e_config(uint32 unit, uint8 phyid);
void rtl8218e_perPort_config(uint32 unit, uint8 phyBaseMacId, rtk_portmask_t *pPortmask);
void phy_construct_config_init(uint32 unit);
void phy_construct_reset(uint32 unit);

#endif  /* __PHY_CONSTRUCT_H__ */

