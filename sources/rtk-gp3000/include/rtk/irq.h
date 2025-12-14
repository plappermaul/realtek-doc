/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition of IRQ API
 *
 * Feature : Provide the APIs to register/deregisger switch IRQ
 *
 */

#ifndef __RTK_IRQ_H__
#define __RTK_IRQ_H__


/*
 * Include Files
 */
#include <rtk/intr.h>


#define IRQ_ID_NO_CARE -1
/*structure define*/
typedef struct rtk_irq_data_s
{
    void (*isr)(void);

}rtk_irq_data_t;


/* Function Name:
 *      rtk_irq_init
 * Description:
 *      IRQ register
 * Input:
 *      irq_id      - IRQ ID
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern int32 rtk_irq_init(void);

/* Function Name:
 *      rtk_irq_exit
 * Description:
 *      IRQ deregister
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern int32 rtk_irq_exit(void);


/* Function Name:
 *      rtk_switch_irq_register
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_isr_register(rtk_intr_type_t intr, void (*fun)(void));


/* Function Name:
 *      rtk_irq_isr_unregister
 * Description:
 *     Unregister isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_isr_unregister(rtk_intr_type_t intr);


/* Function Name:
 *      rtk_irq_gpioISR_register
 * Description:
 *      Register isr handler
 * Input:
 *      intr            - interrupt type
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_gpioISR_register(uint32 gpioId, void (*fun)(void));


/* Function Name:
 *      rtk_irq_gpioISR_unregister
 * Description:
 *     Unregister isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_gpioISR_unregister(uint32 gpioId);


/* Function Name:
 *      rtk_irq_isrDgTh_register
 * Description:
 *      Register Dying gasp top half isr handler
 * Input:
 *      intr            - interrupt type
 *      fun            - function pointer of isr hander
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_isrDgTh_register(void (*fun)(void));


/* Function Name:
 *      rtk_irq_isrDgTh_unregister
 * Description:
 *      Register  Dying gasp top half isr handler
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_isrDgTh_unregister(void);


/* Function Name:
 *      rtk_irq_dgTimes_set
 * Description:
 *      Register  Dying gasp top half isr handler
 * Input:
 *      dg_times        - dying_gasp times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
rtk_irq_dgTimes_set(uint32 dg_times);

#endif  /* __RTK_IRQ_H__ */
