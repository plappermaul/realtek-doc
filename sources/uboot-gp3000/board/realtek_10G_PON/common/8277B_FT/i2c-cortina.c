/*
 # (C) Copyright 2018
 * Cortina-Access Taiwan Limited.
 * Based on designware_i2c.c
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <asm/io.h>
#include "i2c-cortina.h"

/*
 * set_speed - Set the i2c speed mode (standard, high, fast)
 * @i2c_spd:	required i2c speed mode
 *
 * Set the i2c speed mode (standard, high, fast)
 */
static void set_speed(struct i2c_adapter *adap, int i2c_spd)
{
	PER_BIW_CFG_t i2c_cfg;
	/* Disable I2C before set speed */
	i2c_cfg.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_CFG));
	i2c_cfg.bf.core_en = 0;
	writel(i2c_cfg.wrd, KSEG1_ATU_XLAT(PER_BIW_CFG));

	switch (i2c_spd) {
		case IC_SPEED_MODE_MAX:
			//i2c_cfg.bf.prer = CONFIG_PER_IO_FREQ / (5 * I2C_MAX_SPEED) - 1;
			i2c_cfg.bf.prer = CORTINA_PER_IO_FREQ / (5 * I2C_MAX_SPEED) - 1;
			break;

		case IC_SPEED_MODE_STANDARD:
			i2c_cfg.bf.prer = CORTINA_PER_IO_FREQ / (5 * I2C_STANDARD_SPEED) - 1;
			break;

		case IC_SPEED_MODE_FAST:
		default:
			i2c_cfg.bf.prer = CORTINA_PER_IO_FREQ / (5 * I2C_FAST_SPEED) - 1;
			break;
	}

	/* Enable back i2c now speed set */
	i2c_cfg.bf.core_en = 1;
	writel(i2c_cfg.wrd, KSEG1_ATU_XLAT(PER_BIW_CFG));
}

/*
 * i2c_set_bus_speed - Set the i2c speed
 * @speed:	required i2c speed
 *
 * Set the i2c speed.
 */
static unsigned int ca_i2c_set_bus_speed(struct i2c_adapter *adap,
										 unsigned int speed)
{
	int i2c_spd;
	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_SPEED_MODE_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_SPEED_MODE_FAST;
	else
		i2c_spd = IC_SPEED_MODE_STANDARD;

	set_speed(adap, i2c_spd);
	adap->speed = speed;
	return 0;
}

/*
 * i2c_init - Init function
 * @speed:	required i2c speed
 * @slaveaddr:	slave address for the device
 *
 * Initialization function.
 */
static void ca_i2c_init(struct i2c_adapter *adap, int speed, int slaveaddr)
{
	PER_BIW_CFG_t i2c_cfg;
	/* Disable i2c */
	i2c_cfg.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_CFG));
	i2c_cfg.bf.core_en = 0;
	i2c_cfg.bf.biw_soft_reset = 1;
	writel(i2c_cfg.wrd, KSEG1_ATU_XLAT(PER_BIW_CFG));
	mdelay(10);
	i2c_cfg.bf.biw_soft_reset = 0;
	writel(i2c_cfg.wrd, KSEG1_ATU_XLAT(PER_BIW_CFG));

	/* Set I2C clock speed */
	ca_i2c_set_bus_speed(adap, speed);

	/* Enable i2c */
	i2c_cfg.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_CFG));
	i2c_cfg.bf.core_en = 1;
	writel(i2c_cfg.wrd, KSEG1_ATU_XLAT(PER_BIW_CFG));
}

static int i2c_wait_complete(void)
{
	PER_BIW_CTRL_t i2c_ctrl;
	unsigned long start_time_bb = get_timer(0);
	i2c_ctrl.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_CTRL));
	while (i2c_ctrl.bf.biwdone == 0) {
		i2c_ctrl.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_CTRL));

		/* Evaluate timeout */
		if (get_timer(start_time_bb) > (unsigned long)(I2C_BYTE_TO_BB)) {
			printf("%s not done!!!\n", __func__);
			return 1;
		}
	}

	/* Clear done bit */
	writel(i2c_ctrl.wrd, KSEG1_ATU_XLAT(PER_BIW_CTRL));
	/* Clear Interrupt */
	writel(1, KSEG1_ATU_XLAT(PER_BIW_INT_0));

	return 0;
}

/*
 * i2c_setaddress - Sets the target slave address
 * @i2c_addr:	target i2c address
 *
 * Sets the target slave address.
 */
static void i2c_setaddress(struct i2c_adapter *adap, unsigned int i2c_addr,
						   int write_read)
{
	/* Fill Address */
	writel(i2c_addr|write_read, KSEG1_ATU_XLAT(PER_BIW_TXR));

	/* Send out */
	writel(BIW_CTRL_START|BIW_CTRL_WRITE, KSEG1_ATU_XLAT(PER_BIW_CTRL));

	/* Polling until complete */
	i2c_wait_complete();
}

/*
 * i2c_wait_for_bb - Waits for bus busy
 *
 * Waits for bus busy
 */
static int i2c_wait_for_bb(struct i2c_adapter *adap)
{

	PER_BIW_ACK_t i2c_ack;
	unsigned long start_time_bb = get_timer(0);
	i2c_ack.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_ACK));
	while (i2c_ack.bf.biw_busy) {
		i2c_ack.wrd = readl(KSEG1_ATU_XLAT(PER_BIW_ACK));

		/* Evaluate timeout */
		if (get_timer(start_time_bb) > (unsigned long)(I2C_BYTE_TO_BB)){
			printf("%s: timeout!\n", __func__);
			return 1;
		}
	}
	return 0;
}

static int i2c_xfer_init(struct i2c_adapter *adap, uchar chip, uint addr,
						 int alen, int write_read)
{
	int addr_len = alen;
	if (i2c_wait_for_bb(adap))
		return 1;
	/* First cycle must write addr + offset */
	chip = ((chip & 0x7F) << 1);
	if((alen==0) && (write_read==I2C_CMD_RD))
		i2c_setaddress(adap, chip, I2C_CMD_RD);
	else
		i2c_setaddress(adap, chip, I2C_CMD_WT);

	while (alen) {
		alen--;
		writel(addr, KSEG1_ATU_XLAT(PER_BIW_TXR));
		if(write_read == I2C_CMD_RD)
			writel(BIW_CTRL_WRITE|BIW_CTRL_STOP, KSEG1_ATU_XLAT(PER_BIW_CTRL));
		else
			writel(BIW_CTRL_WRITE, KSEG1_ATU_XLAT(PER_BIW_CTRL));
		i2c_wait_complete();
	}

	/* Send address again with Read flag if it's read command */
	if ((write_read == I2C_CMD_RD) && (addr_len > 0)) {
		i2c_setaddress(adap, chip, I2C_CMD_RD);
	}
	return 0;
}

static int i2c_xfer_finish(struct i2c_adapter *adap)
{
	/* Dummy read makes bus free */
	writel(BIW_CTRL_READ | BIW_CTRL_STOP, KSEG1_ATU_XLAT(PER_BIW_CTRL));
	i2c_wait_complete();

	if (i2c_wait_for_bb(adap)) {
		printf("Timed out waiting for bus\n");
		return 1;
	}

	return 0;
}

/*
 * i2c_read - Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
static int ca_i2c_read(struct i2c_adapter *adap, u8 dev, uint addr,
					   int alen, u8 * buffer, int len)
{
	unsigned long start_time_rx;
	int rc = 0;

	if (i2c_xfer_init(adap, dev, addr, alen, I2C_CMD_RD))
		return 1;

	start_time_rx = get_timer(0);
	while (len) {
		/* ACK_IN is ack value to send during read.
		 * ack high only on the very last byte!
		 */
		if (len == 1)			/* Stop if this is the last byte */
			writel(BIW_CTRL_READ | BIW_CTRL_ACK_IN | BIW_CTRL_STOP, KSEG1_ATU_XLAT(PER_BIW_CTRL));
		else
			writel(BIW_CTRL_READ, KSEG1_ATU_XLAT(PER_BIW_CTRL));

		rc = i2c_wait_complete();
		udelay(1);

		if (rc == 0) {
			*buffer++ = (uchar) readl(KSEG1_ATU_XLAT(PER_BIW_RXR));
			len--;
			start_time_rx = get_timer(0);

		} else if (get_timer(start_time_rx) > I2C_BYTE_TO) {
			return 1;
		}
	}
	i2c_xfer_finish(adap);
	return rc;
}

/*
 * i2c_write - Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
static int ca_i2c_write(struct i2c_adapter *adap, u8 dev, uint addr,
						int alen, u8 * buffer, int len)
{
	int rc, nb = len;
	unsigned long start_time_tx;
	if (i2c_xfer_init(adap, dev, addr, alen, I2C_CMD_WT))
		return 1;

	start_time_tx = get_timer(0);
	while (len) {
		writel(*buffer, KSEG1_ATU_XLAT(PER_BIW_TXR));
		if (len == 1)			/* Stop if this is the last byte */
			writel(BIW_CTRL_WRITE | BIW_CTRL_STOP, KSEG1_ATU_XLAT(PER_BIW_CTRL));
		else
			writel(BIW_CTRL_WRITE, KSEG1_ATU_XLAT(PER_BIW_CTRL));

		rc = i2c_wait_complete();

		if (rc == 0) {
			len--;
			buffer++;
			start_time_tx = get_timer(0);
		} else if (get_timer(start_time_tx) > (nb * I2C_BYTE_TO)) {
			printf("Timed out. i2c write Failed\n");
			return 1;
		}
	}
	return 0;
}

/*
 * i2c_probe - Probe the i2c chip
 */
static int ca_i2c_probe(struct i2c_adapter *adap, u8 dev)
{
	u32 tmp;
	int ret;

	/*
	 * Try to read the first location of the chip.
	 */
	ret = ca_i2c_read(adap, dev, 0, 1, (uchar *) & tmp, 1);
	if (ret)
		ca_i2c_init(adap, adap->speed, adap->slaveaddr);

	return ret;
}

U_BOOT_I2C_ADAP_COMPLETE(ca_0, ca_i2c_init, ca_i2c_probe, ca_i2c_read,
						 ca_i2c_write, ca_i2c_set_bus_speed,
						 CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE, 0)
