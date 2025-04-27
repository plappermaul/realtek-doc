/*
 * (C) Copyright 2000
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
 */

#include <common.h>
#include <exports.h>

extern dhry_main(int argc, char *argv[]);

int dhry_test (int argc, char *argv[])
{
	int i;
	unsigned int flashdata;
	volatile unsigned int *pFlash;

	/* Print the ABI version */
	//app_startup(argv);

	printf ("Dhrystone benchmark:\n");
	clear_CP3_counter();
	set_and_start_counter(0);

	dhry_main(argc, argv);

	stop_counter();
	printf ("Hit any key to exit ... ");
	while (!tstc())
		;
	/* consume input */
	(void) getc();

	printf ("\n\n");
	return (0);
}
