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
 * Purpose : Implementation of the uart driver
 *
 * Feature : uart driver
 *
 */

/*
 * Include Files
 */
#include <soc/type.h>
#include <drv/uart/uart.h>
#include <private/drv/uart/uart_mapper.h>
#include <osal/time.h>
#include <osal/print.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>

/*
 * Symbol Definition
 */

/* poe packet size is 15, buffer_size > 15 */
#define UART_TX_BUFFER_SIZE	(32)
#define UART_RX_BUFFER_SIZE	(32)

/*
 * Data Declaration
 */

typedef struct UART_QUE_S
{
    unsigned int	size;
    unsigned int	consumerIndex;
    unsigned int	producerIndex;
    unsigned int	boundryIndex;
    unsigned char	*buf;
} UART_QUE_T;

UART_QUE_T rx_buf;
UART_QUE_T tx_buf;

static uint8 rx_pool[UART_RX_BUFFER_SIZE];
static uint8 tx_pool[UART_TX_BUFFER_SIZE];

static osal_mutex_t uart1_sem[RTK_MAX_NUM_OF_UNIT];


/*
 * Macro Definition
 */
#define UART1_SEM_LOCK(unit)    \
    do {\
        if (osal_sem_mutex_take(uart1_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_COMMON), "semaphore lock failed");\
            return RT_ERR_SEM_LOCK_FAILED;\
        }\
    } while(0)

#define UART1_SEM_UNLOCK(unit)   \
    do {\
        if (osal_sem_mutex_give(uart1_sem[unit]) != RT_ERR_OK)\
        {\
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_COMMON), "semaphore unlock failed");\
            return RT_ERR_SEM_UNLOCK_FAILED;\
        }\
    } while(0)



/*
 * Function Declaration
 */

/* Function Name:
 *      drv_uart_QueReset
 * Description:
 *      reset the queue info to default value
 * Input:
 *      que - queue pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int drv_uart_QueReset(UART_QUE_T* que)
{
    /* check if it is a value queue */
    if(que == NULL)
        return RT_ERR_FAILED;

    /* reset the indices */
    que->consumerIndex = 1;
    que->producerIndex = 1;
    que->boundryIndex = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_uart_QueCreate
 * Description:
 *      allocate buffer to queue
 * Input:
 *      que - queue pointer
 *      buf - data buffer
 *      size - buffer size
 * Output:
 *      None
 * Return:
 *			queue pointer
 * Note:
 *      None
 */
static UART_QUE_T *drv_uart_QueCreate(UART_QUE_T *que,uint8 *buf,uint32 size)
{
    if(que != NULL)
    {
        /* allocating the buffer address right after the structure */
        que->buf = buf;
        que->size = size;

        /* reset the indeices */
        drv_uart_QueReset(que);
    }

    return que;
}

/* Function Name:
 *      drv_uart_IsQueEmpty
 * Description:
 *      check if queue empty
 * Input:
 *      que - queue pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int drv_uart_IsQueEmpty(UART_QUE_T *que)
{
    /* check if the que is valid */
    if(que == NULL)
        return RT_ERR_FAILED;

    if (que->producerIndex == que->consumerIndex)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      drv_uart_IsQueFull
 * Description:
 *      check if queue full
 * Input:
 *      que - queue pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int drv_uart_IsQueFull(UART_QUE_T *que)
{
    /* check if the que is valid */
    if(que == NULL)
        return RT_ERR_FAILED;

    if(que->producerIndex == que->boundryIndex)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      drv_uart_QueConsume
 * Description:
 *      get char(s) from queue buffer
 * Input:
 *      que - queue pointer
 *      buf - char(s) buffer pointer
 *    	length - expect input char(s) size
 * Output:
 *      None
 * Return:
 *      the success insert char(s) count
 * Note:
 *      None
 */
static int drv_uart_QueConsume(uint32 unit, UART_QUE_T *que, uint8 *buf, int32 length)
{
    int32 i;
    uint8 *source_ptr;

    /* check if the que is valid */
    if(que == NULL)
        return 0;

    source_ptr=que->buf;

    for(i = 0;i<length && drv_uart_IsQueEmpty(que) != RT_ERR_OK; i++)
    {
        UART1_SEM_LOCK(unit);
        que->boundryIndex = que->consumerIndex;
        *(buf+i)=*(source_ptr+que->consumerIndex);
        que->consumerIndex++;
        if(que->consumerIndex >= que->size)
        {
            /* circular wrap */
            que->consumerIndex = 0;
        }
        UART1_SEM_UNLOCK(unit);
    }
    return i;
}

/* Function Name:
 *      drv_uart_QueProduce
 * Description:
 *      remove char(s) from queue buffer
 * Input:
 *      que - queue pointer
 *      buf - char(s) buffer pointer
 *    	length - expect input char(s) size
 * Output:
 *      None
 * Return:
 *      the success remove char(s) count
 * Note:
 *      None
 */
static int drv_uart_QueProduce(UART_QUE_T *que, uint8 *buf, int32 length)
{

    int32 i;
    uint8 *dest_ptr;

    /* check if the que is valid */
    if(que == NULL)
        return 0;

    dest_ptr = que->buf;
    for(i = 0;i<length && drv_uart_IsQueFull(que) != RT_ERR_OK; i++)
    {
        UART1_SEM_LOCK(HWP_MY_UNIT_ID());
        *(dest_ptr+que->producerIndex) = *(buf+i);
        que->producerIndex++;

        if(que->producerIndex == que->size)
        {
            /* circular wrap */
            que->producerIndex = 0;
        }
        UART1_SEM_UNLOCK(HWP_MY_UNIT_ID());
    }
    return i;
}

/* Function Name:
 *      drv_uart_CallbackInput
 * Description:
 *      the call back routine for serial isr char in
 * Input:
 *      que - queue pointer
 *      inchar - serial in data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 drv_uart_CallbackInput(uint32 unit, uint8 inchar)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    /* store received byte to rx queue */
    if(drv_uart_QueProduce(&rx_buf, &inchar, (int32)1) != 1)
        return RT_ERR_FAILED;
    else
    {
        return RT_ERR_OK;
    }
}

/* Function Name:
 *      drv_uart_CallbackOutput
 * Description:
 *      the call back routine for serial isr char out
 * Input:
 *      que - queue pointer
 *      inch - serial out data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 drv_uart_CallbackOutput(uint32 unit, uint8 *inch)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    /* send a byte to caller (the interrupt routing) */
    if(drv_uart_QueConsume(unit, &tx_buf, (uint8 *)inch, (int32)1) != 1)
        return RT_ERR_FAILED;
    else
    {
        return RT_ERR_OK;
    }
}

/* Function Name:
 *      drv_uart_init
 * Description:
 *      Init the uart module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_init(uint32 unit)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    drv_uart_QueCreate(&rx_buf,rx_pool, UART_RX_BUFFER_SIZE);
    drv_uart_QueCreate(&tx_buf,tx_pool, UART_TX_BUFFER_SIZE);

    /* create semaphore */
    uart1_sem[unit] = osal_sem_mutex_create();
    if (0 == uart1_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_INIT), "semaphore create failed");
        return RT_ERR_FAILED;
    }



    return UART_CTRL(unit).init(unit,
    (drv_uart_rxcallback_t)drv_uart_CallbackInput,
    (drv_uart_txcallback_t)drv_uart_CallbackOutput);


} /* end of drv_uart_init */

/* Function Name:
 *      drv_uart_interface_set
 * Description:
 *      Init the common GPIO interface.
 * Input:
 *      unit - unit id
 *      interface - common pin interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
int32 drv_uart_interface_set(uint32 unit, drv_uart_interface_t interface)
{
    int32 ret;
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    UART1_SEM_LOCK(unit);
    if (RT_ERR_OK != (ret = UART_CTRL(unit).interface_set(unit, interface)))
    {
        UART1_SEM_UNLOCK(unit);
        return ret;
    }
    UART1_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_uart_clearfifo
 * Description:
 *      empty hw rx/tx fifo and reset queue information to default value
 * Input:
 *      que - queue pointer
 *      inch - serial out data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_clearfifo(uint32 unit)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    UART1_SEM_LOCK(unit);
    UART_CTRL(unit).clearfifo(unit);
    drv_uart_QueReset(&rx_buf);
    drv_uart_QueReset(&tx_buf);
    UART1_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of drv_uart_clearfifo */

/* Function Name:
 *      drv_uart_getc
 * Description:
 *      Get the character from uart interface with timeout value in the specified device
 * Input:
 *      unit    - unit id
 *      timeout - timeout value (unit: milli-second), 0 mean no timeout
 * Output:
 *      pData   - pointer buffer of character from uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_BUSYWAIT_TIMEOUT - timeout and no get the character
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_getc(uint32 unit, uint8 u8len, uint8 *pData, uint32 timeout)
{
    uint32  timeout_us;
    osal_usecs_t begin;
    osal_usecs_t end;
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    timeout_us = timeout * 1000;
    osal_time_usecs_get(&begin);
    *pData = 0;
    while (UART_CTRL(unit).tstc(unit)== RT_ERR_FAILED)
    {
        if (timeout != 0)
        {
            osal_time_usecs_get(&end);
            if (end - begin > timeout_us)
                return RT_ERR_BUSYWAIT_TIMEOUT;
            osal_time_udelay(10);
        }
    }
    return UART_CTRL(unit).poll_getc(unit, u8len, pData);
} /* end of drv_uart_getc */

/* Function Name:
 *      drv_uart_gets
 * Description:
 *      Get the character(s) from uart queue buffer with timeout value in the specified device
 * Input:
 *      unit    - unit id
 *      timeout - timeout value (unit: milli-second), 0 mean no timeout
 * Output:
 *      pData   - pointer buffer of character from uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_BUSYWAIT_TIMEOUT - timeout and no get the character
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_gets(uint32 unit, uint8 *pData, uint32 expect_size,uint32 *rev_cnt,uint32 timeout)
{
    uint32  timeout_us;
    osal_usecs_t begin;
    osal_usecs_t end;
    uint32 count;
    uint32 offset,remainder;
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    *rev_cnt=count=offset=0;
    remainder = expect_size;
    while (remainder != 0)
    {
        timeout_us = timeout * 1000;
        osal_time_usecs_get(&begin);
        count=0;
        while (count==0)
        {
            count = drv_uart_QueConsume(unit, &rx_buf, pData+offset, remainder);
            if (!count && timeout != 0)
            {
                osal_time_usecs_get(&end);
                if (end - begin > timeout_us)
                {
                    osal_printf("\n timeout,expect size %u,remainder %u",expect_size,remainder);
                    return RT_ERR_BUSYWAIT_TIMEOUT;
                }
                osal_time_usleep(10*1000);
            }
            if (count)
            {
                offset=offset+count;
                remainder=remainder-count;
            }
        }
    }

    *rev_cnt = (expect_size-remainder);

    return RT_ERR_OK;
} /* end of drv_uart_gets */

/* Function Name:
 *      drv_uart_putc
 * Description:
 *      Output the character to uart interface in the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      data - output the character to uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_putc(uint32 unit, uint8 u8len, uint8 *data)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return UART_CTRL(unit).poll_putc(unit, u8len ,data);
} /* end of drv_uart_putc */

/* Function Name:
 *      drv_uart_puts
 * Description:
 *      Output the character(s) to uart queue buffer in the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      data - output the character to uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_puts(uint32 unit, uint8 *data,uint32 size)
{
    int32 retval = RT_ERR_OK;
    uint32 out_size = 0;
    uint32   offset,remainder;
    remainder=size;
    offset=0;
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    while (remainder != 0)
    {
        out_size = drv_uart_QueProduce(&tx_buf,data+offset, remainder);
        if (out_size != 0)
        {
            UART_CTRL(unit).starttx(unit);
            osal_time_udelay(100);
        }
        remainder=remainder-out_size;
        offset=offset+out_size;
  }
  return retval;
} /* end of drv_uart_puts */

/* Function Name:
 *      drv_uart_baudrate_get
 * Description:
 *      Get the baudrate of the uart interface in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pBaudrate - pointer buffer of baudrate value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_baudrate_get(uint32 unit, drv_uart_baudrate_t *pBaudrate)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return UART_CTRL(unit).baudrate_get(unit, pBaudrate);
} /* end of drv_uart_baudrate_get */

/* Function Name:
 *      drv_uart_baudrate_set
 * Description:
 *      Configure the baudrate of the uart interface in the specified device
 * Input:
 *      unit     - unit id
 *      baudrate - baudrate value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_uart_baudrate_set(uint32 unit, drv_uart_baudrate_t baudrate)
{
    RT_PARAM_CHK(((unit > RTK_MAX_UNIT_ID) || (unit != HWP_MY_UNIT_ID())), RT_ERR_UNIT_ID);

    return UART_CTRL(unit).baudrate_set(unit, baudrate);
} /* end of drv_uart_baudrate_set */
