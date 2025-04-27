/* Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 83734 $
 * $Date: 2017-11-24 18:52:53 +0800 (Fri, 24 Nov 2017) $
 *
 * Purpose : Definition of Interrupt control API
 *
 * Feature : The file includes the following modules
 *           (1) SWCORE
 *           (2) NIC
 *
 */
#ifndef __DRV_INTR_H__
#define __DRV_INTR_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/type.h>
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
typedef enum intr_controller_e
{
#if defined(CONFIG_SDK_RTL8390)
    INTR_R8390,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INTR_R8380,
#endif
#if defined(CONFIG_SDK_RTL9300)
        INTR_R9300,
#endif
#if defined(CONFIG_SDK_RTL9310)
        INTR_R9310,
#endif
    INTR_CTRL_END
} intr_controller_t;

typedef enum drv_intr_source_e
{
    LINK_CHANGE_INTR = 0x1,
    MEDIA_CHANGE_INTR,
    SPEED_CHANGE_INTR,
    DUPLEX_CHANGE_INTR,
    INTR_SOURCE_END
} drv_intr_source_t;

typedef struct drv_intr_mapper_operation_s
{
    int32   (*enable_set)(uint32 , drv_intr_source_t);
    void    (*swcore_handler)(void *);
} drv_intr_mapper_operation_t;

typedef void (*drv_intr_hdl_f)(uint32 unit, void *pHandler_param);

#define INTR_CTRL(unit)      intr_ops[intr_if[unit]]

/*
 * Data Declaration
 */
extern drv_intr_mapper_operation_t intr_ops[INTR_CTRL_END];
extern uint32 intr_if[RTK_MAX_NUM_OF_UNIT];
/*
 * Function Declaration
 */
/* Function Name:
 *      drv_intr_init
 * Description:
 *      Initialize intr module of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Must initialize nic module before calling any nic APIs.
 */
extern int32
drv_intr_init(uint32 unit);

/* Function Name:
 *      drv_intr_enable_set
 * Description:
 *      Set the interrupt enable status of the specified source.
 * Input:
 *      unit     - unit id
 *      intr_source - interrupt source that is going to be enabled.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      Must initialize interrupt module before calling any nic APIs.
 */
extern int32
drv_intr_enable_set(uint32 unit, drv_intr_source_t intr_source);

/* Function Name:
 *      drv_intr_swcore_handler
 * Description:
 *      Common swcore interrupt handler function.
 * Input:
 *      pParam - The argument passed to interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
drv_intr_swcore_handler(void *pParam);


/* Function Name:
 *      drv_intr_link_stat_register
 * Description:
 *      Register the callback function for handling link state change interrupt handler.
 * Input:
 *      fLinkStatCb - pointer to a handler of link state change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
drv_intr_link_stat_register(drv_intr_hdl_f fLinkStatCb);

/* Function Name:
 *      drv_intr_link_stat_unregister
 * Description:
 *      Unregister the callback function for handling link state change interrupt handler.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 * Note:
 *      None
 */
int32
drv_intr_link_stat_unregister(uint32 unit);

#endif /* __DRV_INTR_H__ */
