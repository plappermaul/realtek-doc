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
 * $Revision: 94631 $
 * $Date: 2019-01-04 16:39:33 +0800 (Fri, 04 Jan 2019) $
 *
 * Purpose : Definition those public uart APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) uart init
 *            2) character set & get
 */

/*
 * Include Files
 */
#include <soc/type.h>
#include <ioal/mem32.h>
#include <osal/print.h>
#include <osal/time.h>
#include <private/drv/uart/uart_common.h>
#include <private/drv/uart/uart_mapper.h>
#include <private/drv/swcore/swcore_rtl8380.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <private/drv/swcore/swcore_rtl9300.h>
#include <hal/chipdef/chip.h>
#include <hwp/hw_profile.h>
#include <osal/isr.h>
#include <dev_config.h>

/*
 * Symbol Definition
 */


/*
 * Data Type Definition
 */
static uint8 uart_init_status[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static struct serial_info_s serial_info[RTK_MAX_NUM_OF_UNIT];
/*
 * Data Declaration
 */
uart_reg_definition_t uart_reg[UART_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL9310)
    {
        .uart1_rx_tx_div_lsb_addr.reg_addr = RTL9310_UART1_PBR_THR_DLL_ADDR,
        .uart1_intr_en_div_msb_addr.reg_addr = RTL9310_UART1_IER_DLM_ADDR,
        .uart1_line_sts_addr.reg_addr = RTL9310_UART1_LSR_ADDR,
        .uart1_line_ctrl_addr.reg_addr = RTL9310_UART1_LCR_ADDR,
        .uart1_intr_id_fifo_ctrl_addr.reg_addr = RTL9310_UART1_IIR_FCR_ADDR,
        .uart1_modem_ctrl_addr.reg_addr = RTL9310_UART1_MCR_ADDR,
    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    {
        .uart1_rx_tx_div_lsb_addr.reg_addr = RTL9300_UART1_PBR_THR_DLL_ADDR,
        .uart1_intr_en_div_msb_addr.reg_addr = RTL9300_UART1_IER_DLM_ADDR,
        .uart1_intr_id_fifo_ctrl_addr.reg_addr = RTL9300_UART1_IIR_FCR_ADDR,
        .uart1_line_ctrl_addr.reg_addr = RTL9300_UART1_LCR_ADDR,
        .uart1_modem_ctrl_addr.reg_addr = RTL9300_UART1_MCR_ADDR,
        .uart1_line_sts_addr.reg_addr = RTL9300_UART1_LSR_ADDR,
    },
#endif
#if defined(CONFIG_SDK_RTL8390)
    {
        .uart1_rx_tx_div_lsb_addr.reg_addr = RTL8390_UART1_RX_TX_DIV_L_ADDR,
        .uart1_intr_en_div_msb_addr.reg_addr = RTL8390_UART1_INTR_EN_DIV_M_ADDR,
        .uart1_line_sts_addr.reg_addr = RTL8390_UART1_LINE_STS_ADDR,
        .uart1_line_ctrl_addr.reg_addr = RTL8390_UART1_LINE_CTRL_ADDR,
        .uart1_intr_id_fifo_ctrl_addr.reg_addr = RTL8390_UART1_INTR_IDEN_FIFO_CTRL_ADDR,
        .uart1_modem_ctrl_addr.reg_addr = RTL8390_UART1_MODEM_CTRL_ADDR,
    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {
        .uart1_rx_tx_div_lsb_addr.reg_addr = RTL8380_UART1_RX_TX_DIV_LSB_ADDR,
        .uart1_intr_en_div_msb_addr.reg_addr = RTL8380_UART1_INTR_EN_DIV_MSB_ADDR,
        .uart1_line_sts_addr.reg_addr = RTL8380_UART1_LINE_STS_ADDR,
        .uart1_line_ctrl_addr.reg_addr = RTL8380_UART1_LINE_CTRL_ADDR,
        .uart1_intr_id_fifo_ctrl_addr.reg_addr = RTL8380_UART1_INTR_ID_FIFO_CTRL_ADDR,
        .uart1_modem_ctrl_addr.reg_addr = RTL8380_UART1_MODEM_CTRL_ADDR,
    },
#endif

};

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
#ifdef UART1_USING_INTERRUPT
static int32 serial_intr(void *isr_param);
#endif
/* Function Name:
 *      serial_out
 * Description:
 *      out serial data
 * Input:
 *      unit  - unit id
 *      value - out data
 * Output:
 *      None
 * Return:
 * Note:
 */
static void
serial_out(uint32 unit, int reg_addr, uint8 value)
{
    ioal_soc_mem32_write(unit, reg_addr, (uint32)(((uint32)(value)) << UART_RESERVED_FILED));
    return;
}

/* Function Name:
 *      serial_in
 * Description:
 *      out serial data
 * Input:
 *      unit  - unit id
 * Output:
 * Return:
  *      serial input data
 * Note:
 */
static uint8
serial_in(uint32 unit, int reg_addr)
{
    uint32 value;

    ioal_soc_mem32_read(unit, reg_addr, &value);
    value = (value >> UART_RESERVED_FILED) & 0xFF;

    return (uint8)value;
}
/* Function Name:
 *      uart_init
 * Description:
 *      Init the uart module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
uart_init(uint32 unit, drv_uart_rxcallback_t rxcallback, drv_uart_txcallback_t txcallback)
{
    RT_INIT_REENTRY_CHK(uart_init_status[unit]);

    /* uart-1 initialize code */
    serial_info[unit].putRxChar = rxcallback;
    serial_info[unit].getTxChar = txcallback;

    while ((serial_in(unit, UART_REG(unit).uart1_line_sts_addr.reg_addr) & UART_LSR_DR) != 0)
    {
        uint8 Data;
        Data = serial_in(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr);
        if(Data);/*Ignore compile error.*/
    }

#ifdef UART1_USING_INTERRUPT
    osal_isr_register(RTK_DEV_UART1, serial_intr, (void *)unit);
    serial_out(unit, UART_IER, UART_IER_RDI);
#else
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, 0);
#endif
    uart_init_status[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of uart_init */

/* Function Name:
 *      uart_interface_set
 * Description:
 *      Init the common GPIO interface.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */

int32 uart_interface_set(uint32 unit, drv_uart_interface_t interface)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    uint32 data;
#endif
    uint32 val;

    switch (interface)
    {
        case JTAG_INTERFACE:
            val = 0x0;
            break;
        case UART1_INTERFACE:
            val = 0x1;
            break;
        case GPIO_INTERFACE:
            val = 0x2;
            break;
        default:
            return RT_ERR_INPUT;
    }

#if defined(CONFIG_SDK_RTL9300)
    if(HWP_9300_FAMILY_ID(unit))
    {
        /* select to uart-1 interface, default is E-JTAG */
        ioal_mem32_read(unit, RTL9300_JTAG_SEL_CTRL_ADDR, &data);
        data &= ~(RTL9300_JTAG_SEL_CTRL_JTAG_SEL_MASK);
        data |= (val << RTL9300_JTAG_SEL_CTRL_JTAG_SEL_OFFSET);
        ioal_mem32_write(unit, RTL9300_JTAG_SEL_CTRL_ADDR, data);
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8390)
    if(HWP_8390_50_FAMILY(unit))
    {
        /* select to uart-1 interface, default is E-JTAG */
        ioal_mem32_read(unit, RTL8390_MAC_IF_CTRL_ADDR, &data);
        data &= ~(RTL8390_MAC_IF_CTRL_JTAG_SEL_MASK);
        data |= (val << RTL8390_MAC_IF_CTRL_JTAG_SEL_OFFSET);
        ioal_mem32_write(unit, RTL8390_MAC_IF_CTRL_ADDR, data);
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(HWP_8380_30_FAMILY(unit))
    {
        if(interface != UART1_INTERFACE)
            return RT_ERR_INPUT;
        /* GMII interface select to uart-1, default is SPI slave */
        ioal_mem32_read(unit, RTL8380_GMII_INTF_SEL_ADDR, &data);
        data &= ~(RTL8380_GMII_INTF_SEL_UART1_SEL_MASK);
        data |= (val << RTL8380_GMII_INTF_SEL_UART1_SEL_OFFSET);
        ioal_mem32_write(unit, RTL8380_GMII_INTF_SEL_ADDR, data);
    }
    else
#endif
    {
        if (val); /* prevent compile warning */
        return RT_ERR_OK;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      uart_tstc
 * Description:
 *      test if serial data in
 * Input:
 *      unit  - unit id
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
uart_tstc(uint32 unit)
{

    if (serial_in(unit, UART_REG(unit).uart1_line_sts_addr.reg_addr) & UART_LSR_DR)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      uart_getc
 * Description:
 *      Get the character from uart interface
 * Input:
 *      unit    - unit id
 * Output:
 *      pData   - pointer buffer of character from uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
uart_getc(uint32 unit, uint8 *pData)
{
    if (serial_in(unit, UART_REG(unit).uart1_line_sts_addr.reg_addr) & UART_LSR_DR)
    {
        (*pData) = serial_in(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr);
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_FAILED;
    }
} /* end of uart_getc */

/* Function Name:
 *      uart_putc
 * Description:
 *      Output the character to uart interface in the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      data - output the character to uart interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
uart_putc(uint32 unit, uint8 data)
{
    while ((serial_in(unit, UART_REG(unit).uart1_line_sts_addr.reg_addr) & UART_LSR_THRE) == 0);

    serial_out(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr, data);

    return RT_ERR_OK;
} /* end of uart_putc */

/* Function Name:
 *      uart_baudrate_get
 * Description:
 *      Get the baudrate of the uart interface in the specified device
 * Input:
 *      unit      - unit id
 * Output:
 *      pBaudrate - pointer buffer of baudrate value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
uart_baudrate_get(uint32 unit, drv_uart_baudrate_t *pBaudrate)
{
    (*pBaudrate) = serial_info[unit].baudrate;
    return RT_ERR_OK;
} /* end of uart_baudrate_get */

/* Function Name:
 *      uart_baudrate_set
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
 * Note:
 *      None
 */
int32
uart_baudrate_set(uint32 unit, drv_uart_baudrate_t baudrate)
{
    uint32 divisor;
    uint32 sysClk = 200 * 1000 * 1000;
#if defined(CONFIG_SDK_RTL9300)
    if(HWP_9300_FAMILY_ID(unit))
    {
        sysClk = 175 * 1000 * 1000; /*175 MHz*/
    }
#endif
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if(HWP_9310_FAMILY_ID(unit) || HWP_8380_FAMILY_ID(unit) || HWP_8390_FAMILY_ID(unit))
    {
        sysClk = 200 * 1000 * 1000; /*200 MHz*/
    }
#endif

    switch (baudrate)
    {
        case UART_BAUDRATE_9600:
            divisor = sysClk/(9600 * 16) - 1;
            break;
        case UART_BAUDRATE_19200:
            divisor = sysClk/(19200 * 16) - 1;
            break;
        case UART_BAUDRATE_38400:
            divisor = sysClk/(38400 * 16) - 1;
            break;
        case UART_BAUDRATE_57600:
            divisor = sysClk/(57600 * 16) - 1;
            break;
        case UART_BAUDRATE_115200:
            divisor = sysClk/(115200 * 16) - 1;
            break;
        case UART_BAUDRATE_END:
        default:
            return RT_ERR_FAILED;
    }
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr,0);
    serial_out(unit, UART_REG(unit).uart1_line_ctrl_addr.reg_addr,( UART_LCR_DLAB | UART_LCR_WLEN8));
    serial_out(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr, (divisor & 0xff));
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, (divisor >> 8));
    serial_out(unit, UART_REG(unit).uart1_line_ctrl_addr.reg_addr, UART_LCR_WLEN8);
    serial_out(unit, UART_REG(unit).uart1_intr_id_fifo_ctrl_addr.reg_addr, FCRVAL);
    serial_info[unit].baudrate = baudrate;
    osal_time_sleep(1);

    return RT_ERR_OK;
} /* end of uart_baudrate_set */

/* Function Name:
 *      serial_starttx
 * Description:
 *      trigger hw to start tx.
 * Input:
 *      unit  - unit id
 * Output:
 *
 * Return:
 * Note:
 */
void
serial_starttx(uint32 unit)
{
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, (UART_IER_RDI | UART_IER_THRI) );
    return;
}

/* Function Name:
 *      serial_clearfifo
 * Description:
 *      empty hw rx/tx fifo.
 * Input:
 *      unit  - unit id
 * Output:
 *
 * Return:
 * Note:
 *      for sync io, poe should call this function before exchange message
 */
void
serial_clearfifo(uint32 unit)
{
    uint8 data;

#ifdef UART1_USING_INTERRUPT
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, 0);
#endif

    serial_out(unit, UART_REG(unit).uart1_intr_id_fifo_ctrl_addr.reg_addr,
        UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT);
    while ((serial_in(unit, UART_REG(unit).uart1_line_sts_addr.reg_addr) & UART_LSR_DR) != 0)
    {
        data = serial_in(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr);
        if(data);/*Ignore compile error.*/
    }
#ifdef UART1_USING_INTERRUPT
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, UART_IER_RDI);
#endif

    return;
}
#ifdef UART1_USING_INTERRUPT
/* Function Name:
 *      serial_intr
 * Description:
 *      serial isr
 * Input:
 *      isr_param  - unit id
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      POE uart message packet size is 15, for speed up the process,
 *      isr read/write 15 chars.
 */
static int32 serial_intr(void *isr_param)
{
    uint32 unit, cnt;
    uint8 lsr, iir, ier, data;

    unit = (uint32)isr_param;
    ier = serial_in(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr);
    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, 0);
    iir = serial_in(unit, UART_REG(unit).uart1_intr_id_fifo_ctrl_addr.reg_addr);
    if(iir);/*Ignore compile error.*/

    cnt = 0;

    while (cnt < 15)
    {
        lsr = serial_in(unit, UART_REG(unit).uart1_line_sts_addr.reg_addr);
        if (lsr & UART_LSR_DR)
        {
            data = serial_in(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr);
            serial_info[unit].putRxChar(unit, data);
        }
        if (lsr & UART_LSR_THRE)
        {
            if (serial_info[unit].getTxChar(unit, &data) != RT_ERR_OK)
            {
                ier &= ~UART_IER_THRI;
            }
            else
            {
                serial_out(unit, UART_REG(unit).uart1_rx_tx_div_lsb_addr.reg_addr, data);
            }
        }
        iir = serial_in(unit, UART_REG(unit).uart1_intr_id_fifo_ctrl_addr.reg_addr);
        cnt++;
    }

    serial_out(unit, UART_REG(unit).uart1_intr_en_div_msb_addr.reg_addr, ier);
    return RT_ERR_OK;
}
#endif
