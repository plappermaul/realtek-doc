
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
 *
 * Purpose : define supported command flags
 *
 * Feature : None
 *
 */

#ifndef __CMD_FLAG_EXTCPU_H__
#define __CMD_FLAG_EXTCPU_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */

#undef CMD_SWITCH_SET_TC_ID_ID_STATE_ENABLE_DISABLE
#undef CMD_SWITCH_SET_TC_ID_ID_MODE_TIMER_COUNTER
#undef CMD_SWITCH_SET_TC_ID_ID_DIV_FACTOR_FACTOR
#undef CMD_SWITCH_SET_TC_ID_ID_INIT_VALUE_VALUE
#undef CMD_SWITCH_GET_TC_ID_ID_COUNTER

#undef CMD_SWITCH_KICK_WATCHDOG

#undef CMD_SWITCH_GET_WATCHDOG_SCALE_BITS
#undef CMD_SWITCH_GET_WATCHDOG_STATE
#undef CMD_SWITCH_GET_WATCHDOG_PHASE_1_PHASE_2_THRESHOLD
#undef CMD_SWITCH_SET_WATCHDOG_SCALE_BITS_25_26_27_28
#undef CMD_SWITCH_SET_WATCHDOG_STATE_DISABLE_ENABLE
#undef CMD_SWITCH_SET_WATCHDOG_PHASE_1_PHASE_2_THRESHOLD_THRESHOLD

#undef CMD_DEBUG_GET_IMAGE_INFO
#undef CMD_DEBUG_FLASHTEST_MTD_MTD_IDX

#undef CMD_NIC_DUMP_PKTHDR_MBUF_RAW_DATA
#undef CMD_NIC_DUMP_PKTHDR_MBUF_TX_RING_IDX_RAW_DATA
#undef CMD_NIC_DUMP_PKTHDR_MBUF_RX_RING_IDX_RAW_DATA
#define CMD_NIC_GET_REG_REG16_REG8_ADDRESS
#define CMD_NIC_SET_REG_REG16_REG8_ADDRESS_VALUE

#endif /* __CMD_FLAG_EXTCPU_H__ */

