/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_ERR_H_
#define _MAC_AX_ERR_H_

#define MACPFCB		1  /* Callback of platform is null */
#define MACPFED		2  /* Endian of platform error */
#define MACBADDR	3  /* Invalid base address */
#define MACLSUS		4  /* Leave suspend error */
#define MACNPTR		5  /* Pointer is null */
#define MACCHIPID	6  /* Chip ID is undefined */
#define MACADAPTER	7  /* Can not get MAC adapter */
#define MACSTCAL	8  /* Unexpected structure alignment */
#define MACNOBUF	9  /* Buffer space is not enough */
#define MACBUFSZ	10 /* Buffer size error */
#define MACNOITEM	11 /* Invalid item */
#define MACPOLLTO	12 /* Polling timeout */
#define MACPWRSW	13 /* Power switch fail */
#define MACBUFALLOC	14 /* Buffer allocation fail */
#define MACWQBUSY	15 /* Work queue is busy */
#define MACCMP		16 /* Failed compare result */
#define MACINTF		17 /* Wrong interface */
#define MACFWBIN	18 /* Incorrect FW bin file */
#define MACFFCFG	19 /* Wrong FIFO configuration */
#define MACSAMACID	20 /* Same MACID */
#define MACMACIDFL	21 /* MACID full */
#define MACNOFW		22 /* There is no FW */
#define MACPROCBUSY	23 /* Process is busy */
#define MACPROCERR	24 /* state machine error */
#define MACEFUSEBANK	25 /* switch efuse bank fail */
#define MACEFUSEREAD	26 /* read efuse fail */
#define MACEFUSEWRITE	27 /* write efuse fail */
#define MACEFUSESIZE	28 /* efuse size error */
#define MACEFUSEPARSE	29 /* eeprom parsing fail */
#define MACEFUSECMP	30 /* compare efuse fail */
#define MACSECUREON	31 /* secure on, no host indirect access */
#define MACTXCHDMA	32 /* invalid tx dma channel */

#define PLTFM_MSG_ALWAYS(...)                                                  \
	adapter->pltfm_cb->msg_print(adapter->drv_adapter,  __VA_ARGS__)

#if MAC_AX_DBG_MSG_EN

/* Enable debug msg depends on  HALMAC_MSG_LEVEL */
#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_ERR)
#define PLTFM_MSG_ERR(...)                                                     \
	adapter->pltfm_cb->msg_print(adapter->drv_adapter,  __VA_ARGS__)
#else
#define PLTFM_MSG_ERR(...)	do {} while (0)
#endif

#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_WARNING)
#define PLTFM_MSG_WARN(...)                                                    \
	adapter->pltfm_cb->msg_print(adapter->drv_adapter, __VA_ARGS__)
#else
#define PLTFM_MSG_WARN(...)	do {} while (0)
#endif

#if (MAC_AX_MSG_LEVEL >= MAC_AX_MSG_LEVEL_TRACE)
#define PLTFM_MSG_TRACE(...)                                                   \
	adapter->pltfm_cb->msg_print(adapter->drv_adapter, __VA_ARGS__)
#else
#define PLTFM_MSG_TRACE(...)	do {} while (0)
#endif

#else

/* Disable debug msg  */
#define PLTFM_MSG_ERR(...)	do {} while (0)
#define PLTFM_MSG_WARN(...)	do {} while (0)
#define PLTFM_MSG_TRACE(...)	do {} while (0)

#endif

#endif
