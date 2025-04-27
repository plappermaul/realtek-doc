/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 */

/*
 * Include Files
 */
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/string.h>

#include <asm/bootinfo.h>
#include <asm/page.h>

#include <prom.h>
#include <platform.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */
void __init prom_meminit(void)
{
   char *ptr;
   unsigned int memsize;

   /* Check the command line first for a memsize directive */
   ptr = strstr(arcs_cmdline, "mem=");

   if (ptr)
      memsize = memparse(ptr + 4, &ptr);
   else
      memsize = 0x02000000;  /* Default to 32MB */

   /*
    * call <add_memory_region> to register boot_mem_map
    * add_memory_region(base, size, type);
    * type: BOOT_MEM_RAM, BOOT_MEM_ROM_DATA or BOOT_MEM_RESERVED
    */
   add_memory_region(0, memsize, BOOT_MEM_RAM);
}

unsigned long __init prom_free_prom_memory(void)
{
    unsigned long freed = 0;

    return freed;
}
