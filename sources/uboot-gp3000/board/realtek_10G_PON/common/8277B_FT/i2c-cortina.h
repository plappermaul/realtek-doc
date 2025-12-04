/*
 # (C) Copyright 2018
 * Cortina-Access Taiwan Limited.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CA_I2C_H_
#define __CA_I2C_H_

struct i2c_regs {
	u32 i2c_cfg;
	u32 i2c_ctrl;
	u32 i2c_txr;
	u32 i2c_rxr;
	u32 i2c_ack;
	u32 i2c_ie0;
	u32 i2c_int0;
	u32 i2c_ie1;
	u32 i2c_int1;
	u32 i2c_stat;
};

#if !defined(IC_CLK)
#define IC_CLK			166
#endif
#define NANO_TO_MICRO		1000

#define I2C_CMD_WT			0
#define I2C_CMD_RD			1
/* CA77XX_BIW_CTRL BIT */
#define BIW_CTRL_DONE		1
#define BIW_CTRL_ACK_IN		(1<<3)
#define BIW_CTRL_WRITE		(1<<4)
#define BIW_CTRL_READ		(1<<5)
#define BIW_CTRL_STOP		(1<<6)
#define BIW_CTRL_START		(1<<7)


/* Worst case timeout for 1 byte is kept as 2ms */
#define I2C_BYTE_TO		(CONFIG_SYS_HZ/500)
#define I2C_STOPDET_TO		(CONFIG_SYS_HZ/500)
#define I2C_BYTE_TO_BB		(10)

/* Speed Selection */
#define IC_SPEED_MODE_STANDARD	1
#define IC_SPEED_MODE_FAST	2
#define IC_SPEED_MODE_MAX	3

#define I2C_MAX_SPEED		1000000
#define I2C_FAST_SPEED		400000
#define I2C_STANDARD_SPEED	100000

#endif							/* __CA_I2C_H_ */
