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

#ifndef _INT_SETUP_H
#define _INT_SETUP_H

typedef enum target_cpu_id_e
{
    TARGET_CPU_ID0 = 0,
    TARGET_CPU_ID1,
    TARGET_CPU_ID2,
    TARGET_CPU_ID3,
    TARGET_CPU_ID4,
    TARGET_CPU_ID5,
    TARGET_CPU_ID6,
    TARGET_CPU_ID7,
    TARGET_CPU_ID_END
} target_cpu_id_t;

typedef enum cpu_ip_id_e
{
    CPU_INT0 = 0,
    CPU_INT1,
    CPU_INT2,
    CPU_INT3,
    CPU_INT4,
    CPU_INT5,
    CPU_INT6,
    CPU_INT7,
    CPU_INT_END
} cpu_ip_id_t;

typedef enum interrupt_type_e
{
    INTR_TYPE_DEFAULT = 0,
    INTR_TYPE_HIGH_LEVEL,
    INTR_TYPE_LOW_LEVEL,
    INTR_TYPE_FALLING_EDGE,
    INTR_TYPE_RISING_EDGE,
    INTR_TYPE_END
} interrupt_type_t;

typedef enum interrupt_default_set_e
{
    INTR_DEFAULT_DISABLE = 0,
    INTR_DEFAULT_ENABLE,
    INTR_DEFAULTE_END
} interrupt_default_set_t;


#define BSP_UNUSED_FIELD        0xFFFFFFFF

typedef struct interrupt_mapping_conf_s
{
    target_cpu_id_t             target_cpu_id;          /* Handle INTR CPU */
    cpu_ip_id_t                 cpu_ip_id;
    interrupt_type_t            intr_type;
    unsigned int                intr_src_id;                 /* SCL pin ID; For Software simulation of I2C, this is GPIO ID */
    interrupt_default_set_t     intr_defalut;
    unsigned int                mapped_system_irq;
} interrupt_mapping_conf_t;

#endif /* _INT_SETUP_H */
