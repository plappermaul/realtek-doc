/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * $Revision: 8992 $
 * $Date: 2010-04-12 14:18:51 +0800 (Mon, 12 Apr 2010) $
 *
 */

#ifndef _PROM_H
#define _PROM_H

extern void prom_printf(char *fmt, ...);
extern void prom_meminit(void);
extern void prom_console_init(void);

#endif /* _PROM_H */
