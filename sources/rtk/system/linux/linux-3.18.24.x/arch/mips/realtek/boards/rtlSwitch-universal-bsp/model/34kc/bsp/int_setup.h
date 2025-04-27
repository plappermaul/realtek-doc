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

typedef enum cpu_src_timer_id_e
{
    CPU_TIMER0 = 0,
    CPU_TIMER1,
    CPU_TIMER2,
    CPU_TIMER3,
    CPU_TIMER4,
    CPU_TIMER5,
    CPU_TIMER6,
    CPU_TIMER_END
} cpu_src_timer_id_t;


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


typedef enum intr_src_idx_e
{
    INTR_SRC_IDX_0 = 0,
    INTR_SRC_IDX_1,
    INTR_SRC_IDX_2,
    INTR_SRC_IDX_3,
    INTR_SRC_IDX_4,
    INTR_SRC_IDX_5,
    INTR_SRC_IDX_6,
    INTR_SRC_IDX_7,
    INTR_SRC_IDX_8,
    INTR_SRC_IDX_9,
    INTR_SRC_IDX_10,
    INTR_SRC_IDX_11,
    INTR_SRC_IDX_12,
    INTR_SRC_IDX_13,
    INTR_SRC_IDX_14,
    INTR_SRC_IDX_15,
    INTR_SRC_IDX_16,
    INTR_SRC_IDX_17,
    INTR_SRC_IDX_18,
    INTR_SRC_IDX_19,
    INTR_SRC_IDX_20,
    INTR_SRC_IDX_21,
    INTR_SRC_IDX_22,
    INTR_SRC_IDX_23,
    INTR_SRC_IDX_24,
    INTR_SRC_IDX_25,
    INTR_SRC_IDX_26,
    INTR_SRC_IDX_27,
    INTR_SRC_IDX_28,
    INTR_SRC_IDX_29,
    INTR_SRC_IDX_30,
    INTR_SRC_IDX_31,
    INTR_SRC_IDX_32,
    INTR_SRC_IDX_33,
    INTR_SRC_IDX_34,
    INTR_SRC_IDX_35,
    INTR_SRC_IDX_36,
    INTR_SRC_IDX_37,
    INTR_SRC_IDX_38,
    INTR_SRC_IDX_39,
    INTR_SRC_IDX_40,
    INTR_SRC_IDX_41,
    INTR_SRC_IDX_42,
    INTR_SRC_IDX_43,
    INTR_SRC_IDX_44,
    INTR_SRC_IDX_45,
    INTR_SRC_IDX_46,
    INTR_SRC_IDX_47,
    INTR_SRC_IDX_END
} intr_src_idx_t;


#define BSP_UNUSED_FIELD        0xFFFFFFFF

typedef struct interrupt_mapping_conf_s
{
    target_cpu_id_t             target_cpu_id;
    cpu_ip_id_t                 cpu_ip_id;
    interrupt_type_t            intr_type;
    unsigned int                intr_src_id;
    interrupt_default_set_t     intr_defalut;
    unsigned int                mapped_system_irq;
} interrupt_mapping_conf_t;

typedef struct cpu_timer_source_mapping_conf_s
{
    unsigned int                timer_id;
    cpu_ip_id_t                 cpu_ip_id;          /* interrupt_mapping_conf_t.cpu_ip_id */
    intr_src_idx_t              intr_src_id;        /* interrupt_mapping_conf_t.intr_src_id */
    interrupt_type_t            intr_type;          /* interrupt_mapping_conf_t.intr_type */
    interrupt_default_set_t     intr_default;       /* interrupt_mapping_conf_t.intr_default */
    unsigned int                mapped_system_irq;  /* interrupt_mapping_conf_t.mapped_system_irq */
    unsigned int                gic_cpu_ip;         /* gic_intr_map.pin */
    unsigned int                flags;              /* gic_intr_map.flags */
} cpu_timer_source_mapping_conf_t;

typedef struct cpu_timerId_irq_s
{
    unsigned int                timer_id;
    unsigned int                mapped_system_irq;
} cpu_timerId_irq_t;


extern cpu_timer_source_mapping_conf_t rtl9310_cpu_timer_mapping[];
extern cpu_timer_source_mapping_conf_t rtl9300_cpu_timer_mapping[];
extern cpu_timer_source_mapping_conf_t rtl839X8X_cpu_timer_mapping[];

extern cpu_timerId_irq_t bsp_cpu_timerId_irq[];

extern void rtk_intr_func_init(void);
extern void bsp_interrupt_irq_mapping_setup(void);

#endif /* _INT_SETUP_H */
