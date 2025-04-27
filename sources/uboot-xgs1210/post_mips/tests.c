/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Be sure to mark tests to be run before relocation as such with the
 * CONFIG_SYS_POST_PREREL flag so that logging is done correctly if the
 * logbuffer support is enabled.
 */

#include <common.h>

#include <post.h>
extern int ocm_post_test (int flags);
extern int cache_post_test (int flags);
extern int watchdog_post_test (int flags);
extern int i2c_post_test (int flags);
extern int rtc_post_test (int flags);
extern int memory_post_test (int flags);
extern int uart_post_test (int flags);
extern int ether_post_test (int flags);
extern int spi_post_test (int flags);
extern int usb_post_test (int flags);
extern int spr_post_test (int flags);
extern int sysmon_post_test (int flags);
extern int dsp_post_test (int flags);
extern int codec_post_test (int flags);
extern int ecc_post_test (int flags);
extern int flash_post_test(int flags);

extern int dspic_init_post_test (int flags);
extern int dspic_post_test (int flags);
extern int gdc_post_test (int flags);
extern int fpga_post_test (int flags);
extern int lwmon5_watchdog_post_test(int flags);
extern int sysmon1_post_test(int flags);
extern int coprocessor_post_test(int flags);
extern int led_post_test(int flags);
extern int button_post_test(int flags);
extern int memory_regions_post_test(int flags);

extern int sysmon_init_f (void);

extern void sysmon_reloc (void);

ulong post_word_load (void)
{
	ulong postmode;
	postmode=getenv_ulong("postword",16,0);
	return postmode;
}

void post_word_store (ulong value)
{
	char tmp[30];
	sprintf(tmp,"0x%x", (u32_t)value);
  setenv("postword", tmp);
	return;
}


struct post_test post_list[] =
{
#if CONFIG_POST & CONFIG_SYS_POST_I2C
    {
	"I2C test",
	"i2c",
	"This test verifies the I2C operation.",
	POST_RAM | POST_ALWAYS,
	&i2c_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_I2C
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEMORY
    {
	"Memory test",
	"memory",
	"This test checks RAM.",
	POST_RAM | POST_NORMAL | POST_NORMAL2 | POST_ROM | POST_POWERON | POST_SLOWTEST | POST_MANUAL,  //POST_ROM | POST_POWERON | POST_SLOWTEST | POST_PREREL,
	&memory_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEMORY
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_UART
  {
	"UART test",
	"uart",
	"This test verifies the UART operation.",
	POST_RAM | POST_NORMAL2 | POST_SLOWTEST | POST_MANUAL,
	&uart_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_UART
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC1
    {
	"Led test",
	"led",
	"This test verifies LEDs on the board.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&led_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_BSPEC1
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_ETHER
    {
	"ETHERNET test",
	"ethernet",
	"This test verifies the ETHERNET operation.",
	POST_RAM | POST_ALWAYS | POST_MANUAL,
	&ether_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_ETHER
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPI
    {
	"SPI test",
	"spi",
	"This test verifies the SPI operation.",
	POST_RAM | POST_ALWAYS | POST_MANUAL,
	&spi_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_SPI
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPR
    {
	"SPR test",
	"spr",
	"This test checks SPR contents.",
	POST_RAM | POST_ALWAYS,
	&spr_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_SPR
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SYSMON
    {
	"SYSMON test",
	"sysmon",
	"This test monitors system hardware.",
	POST_RAM | POST_ALWAYS,
	&sysmon_post_test,
	&sysmon_init_f,
	&sysmon_reloc,
	CONFIG_SYS_POST_SYSMON
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_FLASH
    {
	"Serial NOR flash test",
	"flash",
	"This test verifies Serial flash operations.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&flash_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_FLASH
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEM_REGIONS
    {
	"Memory regions test",
	"mem_regions",
	"This test checks regularly placed regions of the RAM.",
	POST_ROM | POST_SLOWTEST | POST_PREREL,
	&memory_regions_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEM_REGIONS
    },
#endif
};

unsigned int post_list_size = ARRAY_SIZE(post_list);
