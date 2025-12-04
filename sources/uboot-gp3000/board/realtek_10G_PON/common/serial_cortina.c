/*
 * (C) Copyright 2015
 * Cortina-Access Ltd.
 * Jason Li, <jason.li@cortina-access.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* Simple U-Boot driver for the PrimeCell PL010/PL011 UARTs */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <watchdog.h>
#include <asm/io.h>
#include <serial.h>
#include <linux/compiler.h>
#include "serial_cortina.h"

#define IO_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define IO_READ(addr) (*(volatile unsigned int *)(addr))

#ifndef CONFIG_DM_SERIAL

static volatile unsigned char *port[] = {(void *)CONFIG_SYS_SERIAL0,
                                         (void *)CONFIG_SYS_SERIAL1};

#	define NUM_PORTS (sizeof(port) / sizeof(port[0]))
DECLARE_GLOBAL_DATA_PTR;
#endif

#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS)
__weak void ca77xx_ni_scan_phy_link(void) {
	return;
}
#endif

static int cortina_putc(int portnum, char c) {
	volatile unsigned int status;

	/* Retry if TX FIFO full */
	status = IO_READ(port[portnum] + UINFO);
	if (status & UINFO_TX_FIFO_FULL) return -EAGAIN;

	/* Send the character */
	IO_WRITE(port[portnum] + UTX_DATA, c);

#if 0
	/* Wait for Tx FIFO empty */
	do {
		status = IO_READ( port[portnum] + UINFO);
	}
	while (!(status & UINFO_TX_FIFO_EMPTY));
#endif

	return 0;
}

static int cortina_getc(int portnum) {
	unsigned int data;

	/* Wait until there is data in the FIFO */
	while (IO_READ(port[portnum] + UINFO) & UINFO_RX_FIFO_EMPTY) {
		WATCHDOG_RESET();
#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS)
		/* check the GPHY link status */
		ca77xx_ni_scan_phy_link();
#endif
	}

	data = IO_READ(port[portnum] + URX_DATA) & 0xFF;

	/* Dual boot for failover (from G2) */
	// restore_gs2_flag();

	return data;
}

static int cortina_tstc(int portnum) {
	unsigned int status;

#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS)
	/* check the GPHY link status */
	ca77xx_ni_scan_phy_link();
#endif

	status = IO_READ(port[portnum] + UINFO);

	if (status & UINFO_RX_FIFO_EMPTY) /* empty */
		return 0;
	else /* something in RX FIFO */
		return 1;
}

static int cortina_generic_setbrg(int portnum, int baudrate) {
	unsigned int temp;
	unsigned int sample = 0;
	unsigned int apb_clk = CONFIG_UART_CLOCK;

	/* Wait short delay to consume message from last bootloader */
	udelay(3000);

	/* serial_base = gd->bd->serial_base; */
	// port[0] = (void *)CONFIG_SYS_SERIAL0;
	// port[1] = (void *)CONFIG_SYS_SERIAL1;

	/*
	** First, disable everything.
	*/
	IO_WRITE(port[CONFIG_CONS_INDEX] + UCFG, 0x0);

	/* get APB clock rate */
	// get_platform_clk(&cpu_clk, &apb_clk, &axi_clk);

	/*
	** Set baud rate, 8 bits, 1 stop bit, no parity
	**
	*/

	/* temp = UART_CLOCK/baudrate; */
	temp = apb_clk / baudrate;
	temp = (temp << 8) | UCFG_EN | UCFG_TX_EN | UCFG_RX_EN | UCFG_CHAR_8;
	IO_WRITE(port[CONFIG_CONS_INDEX] + UCFG, temp);

	/* sample = (UART_CLOCK/baudrate)/2; */
	sample = (apb_clk / baudrate) / 2;
	sample = (sample < 7) ? 7 : sample;
	IO_WRITE(port[CONFIG_CONS_INDEX] + URX_SAMPLE, sample);

	return 0;
}

#ifndef CONFIG_DM_SERIAL
static void cortina_serial_init_baud(int baudrate) {
	port[0] = (unsigned char *)UART0_BASE_ADDR;
	port[1] = (unsigned char *)UART1_BASE_ADDR;
	cortina_generic_setbrg(CONFIG_CONS_INDEX, baudrate);
}

/*
 * Integrator AP has two UARTs, we use the first one, at 38400-8-N-1
 * Integrator CP has two UARTs, use the first one, at 38400-8-N-1
 * Versatile PB has four UARTs.
 */
int cortina_serial_init(void) {
	cortina_serial_init_baud(CONFIG_BAUDRATE);

	return 0;
}

void cortina_serial_putc(const char c) {
	if (c == '\n')
		while (cortina_putc(CONFIG_CONS_INDEX, '\r') == -EAGAIN)
			;

	while (cortina_putc(CONFIG_CONS_INDEX, c) == -EAGAIN)
		;
}

static int cortina_serial_getc(void) {
	while (1) {
		int ch = cortina_getc(CONFIG_CONS_INDEX);

		if (ch == -EAGAIN) {
			WATCHDOG_RESET();
			continue;
		}
		return ch;
	}
}

static int cortina_serial_tstc(void) {
	return cortina_tstc(CONFIG_CONS_INDEX);
}

static void cortina_serial_setbrg(void) {
	cortina_serial_init_baud(gd->baudrate);
}

static struct serial_device cortina_serial_drv = {
  .name = "cortina_serial",
  .start = cortina_serial_init,
  .stop = NULL,
  .setbrg = cortina_serial_setbrg,
  .putc = cortina_serial_putc,
  .puts = default_serial_puts,
  .getc = cortina_serial_getc,
  .tstc = cortina_serial_tstc,
};

void cortina_serial_initialize(void) {
	serial_register(&cortina_serial_drv);
}

__weak struct serial_device *default_serial_console(void) {
	return &cortina_serial_drv;
}

#endif /* nCONFIG_DM_SERIAL */

#ifdef CONFIG_DM_SERIAL

struct pl01x_priv {
	struct pl01x_regs *regs;
	enum pl01x_type type;
};

static int cortina_serial_setbrg(struct udevice *dev, int baudrate) {
	struct pl01x_serial_platdata *plat = dev_get_platdata(dev);
	struct pl01x_priv *priv = dev_get_priv(dev);

	cortina_generic_setbrg(priv->regs, priv->type, plat->clock, baudrate);

	return 0;
}

static int pl01x_serial_probe(struct udevice *dev) {
	struct pl01x_serial_platdata *plat = dev_get_platdata(dev);
	struct pl01x_priv *priv = dev_get_priv(dev);

	priv->regs = (struct pl01x_regs *)plat->base;
	priv->type = plat->type;
	return cortina_generic_serial_init(priv->regs, priv->type);
}

static int cortina_serial_getc(struct udevice *dev) {
	struct pl01x_priv *priv = dev_get_priv(dev);

	return cortina_getc(CONFIG_CONS_INDEX);
}

int cortina_serial_putc(struct udevice *dev, const char ch) {
	struct pl01x_priv *priv = dev_get_priv(dev);

	return cortina_putc(CONFIG_CONS_INDEX, ch);
}

static int pl01x_serial_pending(struct udevice *dev, bool input) {
	struct pl01x_priv *priv = dev_get_priv(dev);
	unsigned int fr = readl(&priv->regs->fr);

	if (input)
		return cortina_tstc(priv->regs);
	else
		return fr & UART_PL01x_FR_TXFF ? 0 : 1;
}

static const struct dm_serial_ops pl01x_serial_ops = {
  .putc = cortina_serial_putc,
  .pending = pl01x_serial_pending,
  .getc = cortina_serial_getc,
  .setbrg = cortina_serial_setbrg,
};

U_BOOT_DRIVER(serial_pl01x) = {
  .name = "serial_pl01x",
  .id = UCLASS_SERIAL,
  .probe = pl01x_serial_probe,
  .ops = &pl01x_serial_ops,
  .flags = DM_FLAG_PRE_RELOC,
  .priv_auto_alloc_size = sizeof(struct pl01x_priv),
};

#endif
