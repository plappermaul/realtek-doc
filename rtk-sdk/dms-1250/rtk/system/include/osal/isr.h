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
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : ISR (interrupt service routine) relative API
 *
 */

#ifndef __OSAL_ISR_H__
#define __OSAL_ISR_H__


/*
 * Include Files
 */
#include <common/type.h>


/*
 * Symbol Definition
 */
#define OSAL_INT_HANDLED    (0)
#define OSAL_INT_NONE       (1)

typedef int32 osal_isrret_t;
typedef osal_isrret_t (*osal_isr_t)(void *);

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_isr_register
 * Description:
 *      SDK register receive packet function.
 * Input:
 *      dev_id       - The device id defined in dev_config.h
 *      fIsr         - The interrupt service routine function.
 *                     It is required to be called with 'pIsr_param' argument.
 *      pIsr_param   - The argument passed to 'fIsr' interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 *      RT_ERR_FAILED - fail to register interrupt service routine.
 *      RT_ERR_INPUT  - invalid input parameters.
 * Note:
 *      None
 */
extern int32
osal_isr_register(uint32 dev_id, osal_isr_t fIsr, void *pIsr_param);


/* Function Name:
 *      osal_isr_unregister
 * Description:
 *      SDK unregister receive packet function.
 * Input:
 *      dev_id - The device id defined in dev_config.h
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK    - isr unregister success.
 *      RT_ERR_INPUT - invalid input parameters.
 * Note:
 *      None
 */
extern int32
osal_isr_unregister(uint32 dev_id);

/* Function Name:
 *      osal_isr_disable_irq
 * Description:
 *      Disable IRQ interrupt.
 * Input:
 *      dev_id        - The device id defined in dev_config.h
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 *      RT_ERR_INPUT  - invalid input parameters.
 * Note:
 *      None
 */
extern int32
osal_isr_disable_irq(uint32 dev_id);

/* Function Name:
 *      osal_isr_enable_irq
 * Description:
 *      Enable IRQ interrupt.
 * Input:
 *      dev_id        - The device id defined in dev_config.h
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 *      RT_ERR_INPUT  - invalid input parameters.
 * Note:
 *      None
 */
extern int32
osal_isr_enable_irq(uint32 dev_id);

/* Function Name:
 *      osal_isr_disable_interrupt
 * Description:
 *      Disable Interrupt.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 *      RT_ERR_INPUT  - invalid input parameters.
 * Note:
 *      None
 */
extern int32
osal_isr_disable_interrupt(void);

/* Function Name:
 *      osal_isr_enable_interrupt
 * Description:
 *      Enable interrupt.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 *      RT_ERR_INPUT  - invalid input parameters.
 * Note:
 *      None
 */
extern int32
osal_isr_enable_interrupt(void);

/* Function Name:
 *      osal_isr_disable_interrupt_save
 * Description:
 *      Disable interrupt and save flags.
 * Input:
 *      flag - save interrupt status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 * Note:
 *      None
 */
extern int32
osal_isr_disable_interrupt_save(unsigned long flag);

/* Function Name:
 *      osal_isr_enable_interrupt_restore
 * Description:
 *      Enable interrupt and restore flags.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 * Note:
 *      None
 */
extern int32
osal_isr_enable_interrupt_restore(unsigned long flag);


#endif /* __OSAL_ISR_H__ */

