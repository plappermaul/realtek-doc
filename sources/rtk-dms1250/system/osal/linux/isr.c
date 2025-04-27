/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

/*
 * Include Files
 */
#include <linux/version.h>

#include <dev_config.h>
#include <linux/interrupt.h>
#include <common/debug/rt_log.h>
#include <osal/isr.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */
#define IS_VALID_DEVICE_ID(_id)     ((_id >= 0) && (_id < RTK_DEV_MAX))

/*
 * Function Declaration
 */

/* Function Name:
 *      _osal_linux_isr
 * Description:
 *      isr wrapper function.
 * Input:
 *      irq         - irq number
 *      pDev        - device data pointer
 * Output:
 *      None
 * Return:
 *      IRQ_HANDLED - irq is handled
 *      IRQ_NONE    - irq is not handled
 * Note:
 *      For internal use.
 */
static irqreturn_t
_osal_linux_isr(int irq, void *pDev)
{
    rtk_dev_t *pRtk_dev;   /* used for cast pDev */
    osal_isr_t dev_isr;    /* used for cast isr */

    pRtk_dev = (rtk_dev_t *)pDev;

    /* Check pRtk_dev value */
    RT_INTERNAL_PARAM_CHK((NULL == pRtk_dev), IRQ_NONE);

    dev_isr = (osal_isr_t)pRtk_dev->fIsr;

    /* Check isr value */
    RT_INTERNAL_PARAM_CHK((NULL == dev_isr), IRQ_NONE);

    /* interrupt is handled. */
    if (OSAL_INT_HANDLED == dev_isr(pRtk_dev->pIsr_param))
    {
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
} /* end of _osal_linux_isr */

/* Function Name:
 *      osal_isr_register
 * Description:
 *      SDK register receive packet function.
 * Input:
 *      dev_id        - The device id defined in dev_config.h
 *      fIsr          - The interrupt service routine function.
 *                      It is required to be called with 'pIsr_param' argument.
 *      pIsr_param    - The argument passed to 'fIsr' interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - isr register success.
 *      RT_ERR_FAILED - fail to register interrupt service routine.
 *      RT_ERR_INPUT  - invalid input parameters.
 * Note:
 *      None
 */
int32
osal_isr_register(uint32 dev_id, osal_isr_t fIsr, void *pIsr_param)
{
    rtk_dev_t *pDev;

    /* Check isr value. */
    RT_INTERNAL_PARAM_CHK((NULL == fIsr), RT_ERR_INPUT);

    /* Check whether the device id is valid. */
    RT_INTERNAL_PARAM_CHK(!IS_VALID_DEVICE_ID(dev_id), RT_ERR_INPUT);

    /* Check device irq value. */
    RT_INTERNAL_PARAM_CHK((0 > rtk_dev[dev_id].irq), RT_ERR_INPUT);

    /* assign device data */
    pDev = &rtk_dev[dev_id];
    pDev->fIsr = fIsr;
    pDev->pIsr_param = pIsr_param;

    /* if device irq is valid */
    if (0 <= pDev->irq)
    {
        /* register the device irq and interrupt service routine. */
        if (request_irq(pDev->irq, _osal_linux_isr, \
            IRQF_SHARED, pDev->pName, pDev) < 0)
        {
            pDev->fIsr = NULL;
            pDev->pIsr_param = NULL;

            return RT_ERR_FAILED;
        }
    }
    else
    {
        /* invalid irq number */
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of osal_isr_register */

/* Function Name:
 *      osal_isr_unregister
 * Description:
 *      SDK unregister receive packet function.
 * Input:
 *      dev_id - The device id defined in device.h
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK    - isr unregister success.
 *      RT_ERR_INPUT - invalid input parameters.
 * Note:
 *      None
 */
int32
osal_isr_unregister(uint32 dev_id)
{
    rtk_dev_t *pDev;

    /* check device id */
    RT_INTERNAL_PARAM_CHK((!IS_VALID_DEVICE_ID(dev_id)), RT_ERR_INPUT);

    pDev = &rtk_dev[dev_id];

    if (NULL != pDev->fIsr)
    {
        free_irq(pDev->irq, pDev);
        pDev->fIsr = NULL;
        pDev->pIsr_param = NULL;
    }

    return RT_ERR_OK;
} /* end of osal_isr_unregister */

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
int32
osal_isr_disable_irq(uint32 dev_id)
{
    rtk_dev_t *pDev;

    /* Check whether the device id is valid. */
    RT_INTERNAL_PARAM_CHK(!IS_VALID_DEVICE_ID(dev_id), RT_ERR_INPUT);

    /* Check device irq value. */
    RT_INTERNAL_PARAM_CHK((0 > rtk_dev[dev_id].irq), RT_ERR_INPUT);

    /* assign device data */
    pDev = &rtk_dev[dev_id];

    disable_irq(pDev->irq);

    return RT_ERR_OK;
} /* end of osal_isr_disable_irq */

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
int32
osal_isr_enable_irq(uint32 dev_id)
{
    rtk_dev_t *pDev;

    /* Check whether the device id is valid. */
    RT_INTERNAL_PARAM_CHK(!IS_VALID_DEVICE_ID(dev_id), RT_ERR_INPUT);

    /* Check device irq value. */
    RT_INTERNAL_PARAM_CHK((0 > rtk_dev[dev_id].irq), RT_ERR_INPUT);

    /* assign device data */
    pDev = &rtk_dev[dev_id];

    enable_irq(pDev->irq);

    return RT_ERR_OK;
} /* end of osal_isr_enable_irq */

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
int32
osal_isr_disable_interrupt(void)
{
    local_irq_disable();

    return RT_ERR_OK;
} /* end of osal_isr_disable_interrupt */

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
int32
osal_isr_enable_interrupt(void)
{
    local_irq_enable();

    return RT_ERR_OK;
} /* end of osal_isr_enable_interrupt */

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
int32
osal_isr_disable_interrupt_save(unsigned long flag)
{
    local_irq_save(flag);

    return RT_ERR_OK;
} /* end of osal_isr_disable_interrupt_save */

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
int32
osal_isr_enable_interrupt_restore(unsigned long flag)
{
    local_irq_restore(flag);

    return RT_ERR_OK;
} /* end of osal_isr_enable_interrupt_restore */


