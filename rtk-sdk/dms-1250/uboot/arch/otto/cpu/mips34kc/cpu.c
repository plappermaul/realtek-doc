/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
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
 */

#include <common.h>
#include <soc.h>
#include <pblr.h>

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	void (*f)(void) = (void *) 0xbfc00000;
	f();
	fprintf(stderr, "*** reset failed ***\n");
	return 0;
}

void flush_cache(ulong start_addr, ulong size) {
	parameters._dcache_writeback_invalidate_all();
	parameters._icache_invalidate_all();
}

#define ENABLE_BEV(status)  do { \
		status = (status | (1 << 22)); \
	} while(0)
#define DISABLE_BEV(status) do { \
		status = (status & (~(1 << 22))); \
	} while(0)
void set_exception_base(u32_t ebase) {
	u32_t tmp;

	/* Get CP0_STATUS ($12) and set BEV to 1. */
	__asm__ __volatile__ ("mfc0 %0, $12;"
	                      :"=r" (tmp));
	ENABLE_BEV(tmp);
	__asm__ __volatile__ ("mtc0 %0, $12;"
	                      ::"r" (tmp));

	/* Change EBASE ($15, 1). */
	__asm__ __volatile__ ("mtc0 %0, $15, 1;\n"
	                      ::"r" (ebase));

	/* Change BEV of CP0_STATUS to 0 to enable EBASE. */
	DISABLE_BEV(tmp);
	__asm__ __volatile__ ("mtc0 %0, $12;"
	                      ::"r" (tmp));

	return;
}
