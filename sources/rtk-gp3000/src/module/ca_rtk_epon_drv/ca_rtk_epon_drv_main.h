/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
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
 * $Revision: $
 * $Date: $
 *
 * Purpose : EPON kernel drivers
 *
 * Feature : This module install EPON kernel callbacks and other 
 * kernel functions
 *
 */

#ifndef __CA_RTK_EPON_DRV_MAIN_H__
#define __CA_RTK_EPON_DRV_MAIN_H__

#define MAX_S_OAM_NUM 16
#define MAX_S_OAM_SIZE 128

typedef struct s_oam_s{
    unsigned char num;
    unsigned char rx_len[MAX_S_OAM_NUM];
    unsigned char rx[MAX_S_OAM_NUM][MAX_S_OAM_SIZE];
    unsigned char tx_len[MAX_S_OAM_NUM];
    unsigned char tx[MAX_S_OAM_NUM][MAX_S_OAM_SIZE];
}s_oam_t;

void s_oam_register(s_oam_t *reg_oam);
void s_oam_unregister(void);

#endif
