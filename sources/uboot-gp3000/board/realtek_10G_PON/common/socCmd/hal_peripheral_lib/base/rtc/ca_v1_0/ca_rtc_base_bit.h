/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTC_BASE_BIT_H_
#define _RTC_BASE_BIT_H_

// RTC Write Enable register
#define BIT_SHIFT_WRITE_ENABLE                  0
#define BIT_MASK_WRITE_ENABLE                   0x1
#define BIT_SET_WRITE_ENABLE(x)                 (((x) & BIT_MASK_WRITE_ENABLE) << BIT_SHIFT_WRITE_ENABLE)
#define BIT_GET_WRITE_ENABLE(x)                 (((x) >> BIT_SHIFT_WRITE_ENABLE) & BIT_MASK_WRITE_ENABLE)

// Control Register
#define BIT_WRITE_ENABLE                        BIT(0)
#define BIT_SEL_WAKEUP_INT                      BIT(1)
#define BIT_WRITE_ENABLE_DEBUG                  BIT(8)
#define BIT_LOAD_TIME_CHECK_DEBUG               BIT(9)

// Changing Register
#define BIT_CHANGING_TIME                       BIT(0)

// BCD Register
#define BIT_SHIFT_BCD_YEAR                      0
#define BIT_MASK_BCD_YEAR                       0xFF
#define BIT_GET_BCD_YEAR(x)                     (((x) >> BIT_SHIFT_BCD_YEAR) & BIT_MASK_BCD_YEAR)

#define BIT_SHIFT_BCD_MONTH                     0
#define BIT_MASK_BCD_MONTH                      0x1F
#define BIT_GET_BCD_MONTH(x)                    (((x) >> BIT_SHIFT_BCD_MONTH) & BIT_MASK_BCD_MONTH)

#define BIT_SHIFT_BCD_WEEK                      0
#define BIT_MASK_BCD_WEEK                       0x7
#define BIT_GET_BCD_WEEK(x)                     (((x) >> BIT_SHIFT_BCD_DAY) & BIT_MASK_BCD_DAY)

#define BIT_SHIFT_BCD_DAY                       0
#define BIT_MASK_BCD_DAY                        0x3F
#define BIT_GET_BCD_DAY(x)                      (((x) >> BIT_SHIFT_BCD_DAY) & BIT_MASK_BCD_DAY)

#define BIT_SHIFT_BCD_HOUR                      0
#define BIT_MASK_BCD_HOUR                       0x3F
#define BIT_GET_BCD_HOUR(x)                     (((x) >> BIT_SHIFT_BCD_HOUR) & BIT_MASK_BCD_HOUR)

#define BIT_SHIFT_BCD_MIN                      0
#define BIT_MASK_BCD_MIN                       0x7F
#define BIT_GET_BCD_MIN(x)                     (((x) >> BIT_SHIFT_BCD_MIN) & BIT_MASK_BCD_MIN)

#define BIT_SHIFT_BCD_SEC                      0
#define BIT_MASK_BCD_SEC                       0x7F
#define BIT_GET_BCD_SEC(x)                     (((x) >> BIT_SHIFT_BCD_SEC) & BIT_MASK_BCD_SEC)

#define BIT_SHIFT_CHANGING                      0
#define BIT_MASK_CHANGING                       0x1
#define BIT_GET_CHANGING(x)                     (((x) >> BIT_SHIFT_CHANGING) & BIT_MASK_CHANGING)

// Interrupt register mask
#define BIT_INT_SECONDLY                        BIT(0)
#define BIT_INT_MINUTELY                        BIT(1)
#define BIT_INT_HOURLY                          BIT(2)
#define BIT_INT_WEEKLY                          BIT(3)
#define BIT_INT_MONTHLY                         BIT(4)
#define BIT_INT_YEARLY                          BIT(5)
#define BIT_INT_SPECIFIC_DATE                   BIT(6)
#define BIT_INT_WAKEUP                          BIT(7)

// Compensation register
#define BIT_COMPENSATION_EN                     BIT(15)
#define BIT_CPMPENSATION_PROGRAM_TIME           BIT(14)
#define BIT_SHIFT_COMPENSATION                  0
#define BIT_MASK_COMPENSATION                   0x3FFF
#define BIT_GET_COMPENSATION(x)                 (((x) >> BIT_SHIFT_COMPENSATION) & BIT_MASK_COMPENSATION)
#define BIT_SET_COMPENSATION(x)                 (((x) & BIT_MASK_COMPENSATION) << BIT_SHIFT_COMPENSATION)

// Ripple counter register
#define BIT_RIPPLE_COUNTER_READ_EN              BIT(15)
#define BIT_SHIFT_RIPPLE_COUNTER                0
#define BIT_MASK_RIPPLE_COUNTER                 0x7FFF
#define BIT_GET_RIPPLE_COUNTER(x)               (((x) >> BIT_SHIFT_RIPPLE_COUNTER) & BIT_MASK_RIPPLE_COUNTER)
#define BIT_SET_RIPPLE_COUNTER(x)               (((x) & BIT_MASK_RIPPLE_COUNTER) << BIT_SHIFT_RIPPLE_COUNTER)


#endif //_RTC_BASE_BIT_H_

