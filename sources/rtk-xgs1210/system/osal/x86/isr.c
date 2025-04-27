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
 * $Revision: 72462 $
 * $Date: 2016-10-15 14:55:08 +0800 (Sat, 15 Oct 2016) $
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
#include <osal/isr.h>
#include <dev_config.h>
#include <linux/interrupt.h>


/*
 * Macro Definition
 */
#define IS_VALID_DEVICE_ID(_id) ((_id >= 0) && (_id < RTK_DEV_MAX))


/*
 * Function Declaration
 */

static irqreturn_t
_osal_linux_isr(int irq, void *dev)
{
    rtk_dev_t *_dev = (rtk_dev_t *) dev;
    osal_isr_t _isr = (osal_isr_t)_dev->fIsr;

    if (_dev && _isr)
    {
        if(OSAL_INT_HANDLED == _isr(_dev->pIsr_param))
            return IRQ_HANDLED;
        else
            return IRQ_NONE;
    }
    return IRQ_NONE;
} /* _osal_linux_isr */

/* Function Name:
 *      osal_isr_register
 * Description:
 *      SDK register receive packet function.
 * Input:
 *      dev_id      - The device id defined in dev_config.h
 *      isr         - The intrrupt service routine function.
 *                    It is required to be called with 'isr_param' argument.
 *      isr_param   - The argument passed to 'isr' interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 osal_isr_register(uint32 dev_id, osal_isr_t isr, void *isr_param)
{
    rtk_dev_t *_dev;

    if (!IS_VALID_DEVICE_ID(dev_id))
    {
        osal_printf(" osal_isr_register: Invalid device index %u\n", dev_id);
        return RT_ERR_FAILED;
    }

    _dev = &rtk_dev[dev_id];
    _dev->fIsr = isr;
    _dev->pIsr_param = isr_param;

    if ( _dev->irq != -1)
    {
        if(request_irq( _dev->irq, _osal_linux_isr, SA_INTERRUPT | SA_SHIRQ, _dev->pName, _dev ) < 0 )
        {
            osal_printf("could not request irq %d for device %d (%s)\n", _dev->irq, _dev->dev_id, _dev->pName);
            _dev->fIsr = NULL;
            _dev->pIsr_param = NULL;
            return RT_ERR_FAILED;
        }
    }
    else
    {
        osal_printf("invalid irq number %d\n", _dev->irq);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* osal_isr_register */

/* Function Name:
 *      osal_isr_unregister
 * Description:
 *      SDK unregister receive packet function.
 * Input:
 *      dev_id - The device id defined in device.h
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 osal_isr_unregister(uint32 dev_id)
{

    rtk_dev_t* _dev;

    if (!IS_VALID_DEVICE_ID(dev_id)) return FAILED;

    _dev = &rtk_dev[dev_id];

    if (_dev->fIsr != NULL)
    {
        free_irq(_dev->irq, _dev);
        _dev->fIsr = NULL;
        _dev->pIsr_param = NULL;
    }

    return SUCCESS;
} /* osal_isr_unregister */

