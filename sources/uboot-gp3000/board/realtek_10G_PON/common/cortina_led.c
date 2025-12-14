/*
 * Driver for CA77XX memory-mapped serial LEDs
 *
 * Copyright (C) 2017 Cortina Access, Inc.
 *		http://www.cortina-access.com
 *
 * based on leds-bcm6328.c
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#include <common.h>
#include <asm/io.h>
#include <linux/compat.h>
#include <status_led.h>

#define BIT(nr) (1UL << (nr))

#define CA77XX_LED_CONTROL 0x00
#define CA77XX_LED_CONFIG_0 0x04
#define CA77XX_LED_CONFIG_1 0x08
#define CA77XX_LED_CONFIG_2 0x0c
#define CA77XX_LED_CONFIG_3 0x10
#define CA77XX_LED_CONFIG_4 0x14
#define CA77XX_LED_CONFIG_5 0x18
#define CA77XX_LED_CONFIG_6 0x1c
#define CA77XX_LED_CONFIG_7 0x20
#define CA77XX_LED_CONFIG_8 0x24
#define CA77XX_LED_CONFIG_9 0x28
#define CA77XX_LED_CONFIG_10 0x2c
#define CA77XX_LED_CONFIG_11 0x30
#define CA77XX_LED_CONFIG_12 0x34
#define CA77XX_LED_CONFIG_13 0x38
#define CA77XX_LED_CONFIG_14 0x3c
#define CA77XX_LED_CONFIG_15 0x40

#define CA77XX_LED_MAX_HW_BLINK 127
#define CA77XX_LED_MAX_COUNT CORTINA_LED_NUM
#define CA77XX_LED_MAX_PORT 8

/* LED_CONTROL fields */
#define CA77XX_LED_BLINK_RATE1_OFFSET 0
#define CA77XX_LED_BLINK_RATE1_MASK 0xFF
#define CA77XX_LED_BLINK_RATE2_OFFSET 8
#define CA77XX_LED_BLINK_RATE2_MASK 0xFF
#define CA77XX_LED_CLK_TEST BIT(16)
#define CA77XX_LED_CLK_POLARITY BIT(17)
#define CA77XX_LED_CLK_TEST_MODE BIT(16)
#define CA77XX_LED_CLK_TEST_RX_TEST BIT(30)
#define CA77XX_LED_CLK_TEST_TX_TEST BIT(31)

/* LED_CONFIG fields */
#define CA77XX_LED_EVENT_ON_OFFSET 0
#define CA77XX_LED_EVENT_ON_MASK 0x7
#define CA77XX_LED_EVENT_BLINK_OFFSET 3
#define CA77XX_LED_EVENT_BLINK_MASK 0x7
#define CA77XX_LED_EVENT_OFF_OFFSET 6
#define CA77XX_LED_EVENT_OFF_MASK 0x7
#define CA77XX_LED_OFF_ON_OFFSET 9
#define CA77XX_LED_OFF_ON_MASK 0x3
#define CA77XX_LED_PORT_OFFSET 11
#define CA77XX_LED_PORT_MASK 0x7
#define CA77XX_LED_OFF_VAL BIT(14)
#define CA77XX_LED_SW_EVENT BIT(15)
#define CA77XX_LED_BLINK_SEL BIT(16)

#define CORTINA_CA_REG_READ(off) CA_REG_READ(off)
#define CORTINA_CA_REG_WRITE(data, off) CA_REG_WRITE(data, off)

/**
 * struct ca77xx_led_cfg - configuration for LEDs
 * @mem: memory resource
 * @lock: memory lock
 * @idx: LED index number
 * @active_low: LED is active low
 * @off_event: off triggered by rx/tx/sw event
 * @blink_event: blinking triggered by rx/tx/sw event
 * @on_event: on triggered by rx/tx/sw event
 * @port: monitor port
 * @blink: haredware blink rate select
 * @enable: LED is enabled/disabled
 */
struct ca77xx_led_cfg {
	void __iomem *mem;
	spinlock_t *lock; /* protect LED resource access */
	int idx;
	bool active_low;

	int off_event;
	int blink_event;
	int on_event;
	int port;
	int blink;
	int enable;
};

/**
 * struct ca77xx_led_cfg - control for LEDs
 * @mem: memory resource
 * @lock: memory lock
 * @test_mode: enter test mode
 * @clk_low: clock polarity
 * @blink_rate1: haredware blink rate 1
 * @blink_rate2: haredware blink rate 2
 * @ca77xx_led: configuration for LEDs
 */
struct ca77xx_led_ctrl {
	void __iomem *mem;
	spinlock_t *lock; /* protect LED resource access */

	int test_mode;
	int clk_low;
	u16 blink_rate1;
	u16 blink_rate2;

	struct ca77xx_led_cfg led_cfg[CA77XX_LED_MAX_COUNT];
};

static struct ca77xx_led_ctrl glb_led_ctrl;

static void ca77xx_led_write(void __iomem *reg, unsigned long data) {
	writel(data, reg);
}

static unsigned long ca77xx_led_read(void __iomem *reg) {
	return readl(reg);
}

int ca77xx_led_sw_on(int idx, int on) {
	struct ca77xx_led_cfg *led_cfg;
	unsigned long flags;
	u32 val;

	if (idx >= CA77XX_LED_MAX_COUNT) return -EINVAL;

	led_cfg = &glb_led_ctrl.led_cfg[idx];

	spin_lock_irqsave(led_cfg->lock, flags);
	val = ca77xx_led_read(led_cfg->mem);
	if (on)
		val |= CA77XX_LED_SW_EVENT;
	else
		val &= ~CA77XX_LED_SW_EVENT;

	ca77xx_led_write(led_cfg->mem, val);
	spin_unlock_irqrestore(led_cfg->lock, flags);

	return 0;
}
EXPORT_SYMBOL(ca77xx_led_sw_on);

/**
 * ca77xx_led_enable - switch LED to off or triggered by event
 */
int ca77xx_led_enable(int idx, int enable) {
	struct ca77xx_led_cfg *led_cfg;
	unsigned long flags;
	u32 val;

	if (idx >= CA77XX_LED_MAX_COUNT) return -EINVAL;

	led_cfg = &glb_led_ctrl.led_cfg[idx];

	spin_lock_irqsave(led_cfg->lock, flags);

	val = ca77xx_led_read(led_cfg->mem);

	if (enable) {
		led_cfg->enable = 1;

		/* driven by event */
		val &= ~(CA77XX_LED_OFF_ON_MASK << CA77XX_LED_OFF_ON_OFFSET);
	} else {
		led_cfg->enable = 0;

		/* force off */
		val &= ~(CA77XX_LED_OFF_ON_MASK << CA77XX_LED_OFF_ON_OFFSET);
		val |= 0x3 << CA77XX_LED_OFF_ON_OFFSET;
	}

	ca77xx_led_write(led_cfg->mem, val);

	spin_unlock_irqrestore(led_cfg->lock, flags);

	return 0;
}
EXPORT_SYMBOL(ca77xx_led_enable);

int ca77xx_led_config(int idx, int active_low, int off_event, int blink_event,
                      int on_event, int port, int blink) {
	struct ca77xx_led_cfg *led_cfg;
	unsigned long flags;
	u32 val;

	if (idx >= CA77XX_LED_MAX_COUNT) return -EINVAL;
	if (off_event > TRIGGER_NONE) return -EINVAL;
	if (blink_event > TRIGGER_NONE) return -EINVAL;
	if (on_event > TRIGGER_NONE) return -EINVAL;
	if (port >= CA77XX_LED_MAX_PORT) return -EINVAL;
	if (blink > BLINK_RATE2) return -EINVAL;

	led_cfg = &glb_led_ctrl.led_cfg[idx];

	spin_lock_irqsave(led_cfg->lock, flags);

	val = ca77xx_led_read(led_cfg->mem);

	if (active_low) {
		led_cfg->active_low = true;
		val |= CA77XX_LED_OFF_VAL;
	} else {
		led_cfg->active_low = false;
		val &= ~CA77XX_LED_OFF_VAL;
	}

	led_cfg->off_event = off_event;
	led_cfg->blink_event = blink_event;
	led_cfg->on_event = on_event;
	led_cfg->blink = blink;

	if (blink == BLINK_RATE1)
		val &= ~CA77XX_LED_BLINK_SEL;
	else if (blink == BLINK_RATE2)
		val |= CA77XX_LED_BLINK_SEL;

	val &= ~(CA77XX_LED_EVENT_OFF_MASK << CA77XX_LED_EVENT_OFF_OFFSET);
	if (off_event != TRIGGER_NONE)
		val |= BIT(off_event) << CA77XX_LED_EVENT_OFF_OFFSET;

	val &= ~(CA77XX_LED_EVENT_BLINK_MASK << CA77XX_LED_EVENT_BLINK_OFFSET);
	if (blink_event != TRIGGER_NONE)
		val |= BIT(blink_event) << CA77XX_LED_EVENT_BLINK_OFFSET;

	val &= ~(CA77XX_LED_EVENT_ON_MASK << CA77XX_LED_EVENT_ON_OFFSET);
	if (on_event != TRIGGER_NONE)
		val |= BIT(on_event) << CA77XX_LED_EVENT_ON_OFFSET;

	led_cfg->port = port;
	val &= ~(CA77XX_LED_PORT_MASK << CA77XX_LED_PORT_OFFSET);
	val |= port << CA77XX_LED_PORT_OFFSET;

	ca77xx_led_write(led_cfg->mem, val);

	spin_unlock_irqrestore(led_cfg->lock, flags);

	ca77xx_led_enable(idx, 0);

	return 0;
}

static int ca77xx_led_init(struct ca77xx_led_ctrl *led_ctrl, u32 idx,
                           void __iomem *mem, spinlock_t *lock) {
	struct ca77xx_led_cfg *led_cfg = &led_ctrl->led_cfg[idx];

	led_cfg->idx = idx;
	led_cfg->mem = mem;
	led_cfg->lock = lock;

	return 0;
}

int ca77xx_leds_init(void __iomem *base, uint16_t rate1, uint16_t rate2) {
	spinlock_t *lock;
	unsigned long flags;
	u32 val, reg;
	int i;

	/* pinctrl */
	{
		GLOBAL_PIN_MUX_t pin_mux;

		pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
		pin_mux.bf.iomux_led_enable = 1;
		CORTINA_CA_REG_WRITE(pin_mux.wrd, GLOBAL_PIN_MUX);
	}

	glb_led_ctrl.mem = base;
	glb_led_ctrl.clk_low = 1;

	reg = 0;
	if (glb_led_ctrl.clk_low)
		reg |= CA77XX_LED_CLK_POLARITY;
	else
		reg &= ~CA77XX_LED_CLK_POLARITY;

	val = rate1 / 16 - 1;
	glb_led_ctrl.blink_rate1 =
	  val > CA77XX_LED_MAX_HW_BLINK ? CA77XX_LED_MAX_HW_BLINK : val;
	reg |= (glb_led_ctrl.blink_rate1 & CA77XX_LED_BLINK_RATE1_MASK)
	       << CA77XX_LED_BLINK_RATE1_OFFSET;

	val = rate2 / 16 - 1;
	glb_led_ctrl.blink_rate2 =
	  val > CA77XX_LED_MAX_HW_BLINK ? CA77XX_LED_MAX_HW_BLINK : val;
	reg |= (glb_led_ctrl.blink_rate2 & CA77XX_LED_BLINK_RATE2_MASK)
	       << CA77XX_LED_BLINK_RATE2_OFFSET;

	spin_lock_irqsave(lock, flags);
	ca77xx_led_write(glb_led_ctrl.mem, reg);
	spin_unlock_irqrestore(lock, flags);

	base += 4;
	for (i = 0; i < CA77XX_LED_MAX_COUNT; i++)
		ca77xx_led_init(&glb_led_ctrl, i, base + i * 4, lock);

	return 0;
}

void __led_set(led_id_t mask, int state) {
	int i;

	for (i = 0; i < CA77XX_LED_MAX_COUNT; i++) {
		if (!((0x1 << i) & mask)) continue;

		if (state == CONFIG_LED_STATUS_ON) {
			ca77xx_led_enable(i, 1);
			ca77xx_led_sw_on(i, 1);
		} else {
			ca77xx_led_enable(i, 0);
			ca77xx_led_sw_on(i, 0);
		}
	}
}

void __led_toggle(led_id_t mask) { /* TBD */
}

void __led_init(led_id_t mask, int state) {
	__led_set(mask, state);
}
