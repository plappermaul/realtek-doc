/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Setup MAC Serdes parameters
 *
 * Feature : Setup MAC Serdes parameters functions
 *
 */
#include <common/error.h>
#include <hwp/hw_profile.h>
#include <hal/chipdef/driver.h>
#include <hal/mac/mem.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <hal/phy/phy_construct.h>
#include <hal/phy/phy_rtl9310.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hwp_util.h>
#include <dal/dal_construct.h>
#include <dal/mango/dal_mango_construct.h>
#include <dal/mango/dal_mango_sds.h>
#include <rtk/default.h>

#define DAL_MANGO_SDS_MAX   14
#define DAL_MANGO_SMI_MAX   4


sds_config dal_mango_construct_ana_common[] =
{
  {0x21, 0x00, 0x1800}, {0x21, 0x01, 0x0060}, {0x21, 0x02, 0x3000}, {0x21, 0x03, 0xFFFF},
  {0x21, 0x04, 0x0603}, {0x21, 0x05, 0x1104}, {0x21, 0x06, 0x4444}, {0x21, 0x07, 0x7044},
  {0x21, 0x08, 0xF104}, {0x21, 0x09, 0xF104}, {0x21, 0x0A, 0xF104}, {0x21, 0x0B, 0x0003},
  {0x21, 0x0C, 0x007F}, {0x21, 0x0D, 0x3FE4}, {0x21, 0x0E, 0x31F9}, {0x21, 0x0F, 0x0618},
  {0x21, 0x10, 0x1FF8}, {0x21, 0x11, 0x7C9F}, {0x21, 0x12, 0x7C9F}, {0x21, 0x13, 0x13FF},
  {0x21, 0x14, 0x001F}, {0x21, 0x15, 0x01F0}, {0x21, 0x16, 0x1067}, {0x21, 0x17, 0x8AF1},
  {0x21, 0x18, 0x210A}, {0x21, 0x19, 0xF0F0}
};

sds_config dal_mango_construct_ana_10p3125g[] =
{
  {0x2E, 0x00, 0x0107}, {0x2E, 0x01, 0x0200}, {0x2E, 0x02, 0x6A24}, {0x2E, 0x03, 0xD10D},
  {0x2E, 0x04, 0xD550}, {0x2E, 0x05, 0xA95E}, {0x2E, 0x06, 0xE31D}, {0x2E, 0x07, 0x000E},
  {0x2E, 0x08, 0x0294}, {0x2E, 0x09, 0x0CE4}, {0x2E, 0x0a, 0x7FC8}, {0x2E, 0x0b, 0xE0E7},
  {0x2E, 0x0c, 0x0200}, {0x2E, 0x0d, 0xDF80}, {0x2E, 0x0e, 0x0000}, {0x2E, 0x0f, 0x1FC4},
  {0x2E, 0x10, 0x0C3F}, {0x2E, 0x11, 0x0000}, {0x2E, 0x12, 0x27C0}, {0x2E, 0x13, 0x7F1C},
  {0x2E, 0x14, 0x1300}, {0x2E, 0x15, 0x003F}, {0x2E, 0x16, 0xBE7F}, {0x2E, 0x17, 0x0090},
  {0x2E, 0x18, 0x0000}, {0x2E, 0x19, 0x4000}, {0x2E, 0x1a, 0x0000}, {0x2E, 0x1b, 0x8000},
  {0x2E, 0x1c, 0x011E}, {0x2E, 0x1d, 0x0000}, {0x2E, 0x1e, 0xC8FF}, {0x2E, 0x1f, 0x0000},
  {0x2F, 0x00, 0xC000}, {0x2F, 0x01, 0xF000}, {0x2F, 0x02, 0x6010},
  {0x2F, 0x12, 0x0EEE}, {0x2F, 0x13, 0x0000}, {0x6 , 0x0 , 0x0000}
};

sds_config dal_mango_construct_ana_10p3125g_cmu[] =
{
  {0x2F, 0x03, 0x4210}, {0x2F, 0x04, 0x0000}, {0x2F, 0x05, 0x3FD9}, {0x2F, 0x06, 0x58A6},
  {0x2F, 0x07, 0x2990}, {0x2F, 0x08, 0xFFF4}, {0x2F, 0x09, 0x1F08}, {0x2F, 0x0A, 0x0000},
  {0x2F, 0x0B, 0x8000}, {0x2F, 0x0C, 0x4224}, {0x2F, 0x0D, 0x0000}, {0x2F, 0x0E, 0x0400},
  {0x2F, 0x0F, 0xA464}, {0x2F, 0x10, 0x8000}, {0x2F, 0x11, 0x0165}, {0x20, 0x11, 0x000D},
  {0x20, 0x12, 0x510F}, {0x20, 0x00, 0x0030}
};

sds_config dal_mango_construct_ana_5g[] =
{
  {0x2A, 0x00, 0x0104}, {0x2A, 0x01, 0x0200}, {0x2A, 0x02, 0x2A24}, {0x2A, 0x03, 0xD10D},
  {0x2A, 0x04, 0xD550}, {0x2A, 0x05, 0xA95E}, {0x2A, 0x06, 0xE31D}, {0x2A, 0x07, 0x800E},
  {0x2A, 0x08, 0x0294}, {0x2A, 0x09, 0x28E4}, {0x2A, 0x0A, 0x7FC8}, {0x2A, 0x0B, 0xE0E7},
  {0x2A, 0x0C, 0x0200}, {0x2A, 0x0D, 0x9F80}, {0x2A, 0x0E, 0x0800}, {0x2A, 0x0F, 0x1FC8},
  {0x2A, 0x10, 0x0C3F}, {0x2A, 0x11, 0x0000}, {0x2A, 0x12, 0x27C0}, {0x2A, 0x13, 0x7F1C},
  {0x2A, 0x14, 0x1300}, {0x2A, 0x15, 0x003F}, {0x2A, 0x16, 0xBE7F}, {0x2A, 0x17, 0x0090},
  {0x2A, 0x18, 0x0000}, {0x2A, 0x19, 0x407F}, {0x2A, 0x1A, 0x0000}, {0x2A, 0x1B, 0x8000},
  {0x2A, 0x1C, 0x011E}, {0x2A, 0x1D, 0x0000}, {0x2A, 0x1E, 0xC8FF}, {0x2A, 0x1F, 0x0000},
  {0x2B, 0x00, 0xC000}, {0x2B, 0x01, 0xF000}, {0x2B, 0x02, 0x6010},
  {0x2B, 0x12, 0x0EEE}, {0x2B, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_5g_cmu[] =
{
  {0x2B, 0x03, 0x5010}, {0x2B, 0x04, 0x0000}, {0x2B, 0x05, 0x27D9}, {0x2B, 0x06, 0x58A6},
  {0x2B, 0x07, 0x2990}, {0x2B, 0x08, 0xFFF4}, {0x2B, 0x09, 0x2682}, {0x2B, 0x0A, 0x0000},
  {0x2B, 0x0B, 0x8000}, {0x2B, 0x0C, 0x5024}, {0x2B, 0x0D, 0x0000}, {0x2B, 0x0E, 0x0000},
  {0x2B, 0x0F, 0xA470}, {0x2B, 0x10, 0x8000}, {0x2B, 0x11, 0x0362}
};

sds_config dal_mango_construct_ana_3p125g[] =
{
  {0x28, 0x00, 0x0104}, {0x28, 0x01, 0x0200}, {0x28, 0x02, 0x2A24}, {0x28, 0x03, 0xD10D},
  {0x28, 0x04, 0xD550}, {0x28, 0x05, 0xA95E}, {0x28, 0x06, 0xE31D}, {0x28, 0x07, 0x000E},
  {0x28, 0x08, 0x0294}, {0x28, 0x09, 0x04E4}, {0x28, 0x0A, 0x7FC8}, {0x28, 0x0B, 0xE0E7},
  {0x28, 0x0C, 0x0200}, {0x28, 0x0D, 0xDF80}, {0x28, 0x0E, 0x0800}, {0x28, 0x0F, 0x1FD8},
  {0x28, 0x10, 0x0C3F}, {0x28, 0x11, 0x0000}, {0x28, 0x12, 0x27C0}, {0x28, 0x13, 0x7F1C},
  {0x28, 0x14, 0x1300}, {0x28, 0x15, 0x003F}, {0x28, 0x16, 0xBE7F}, {0x28, 0x17, 0x0090},
  {0x28, 0x18, 0x0000}, {0x28, 0x19, 0x407F}, {0x28, 0x1A, 0x0000}, {0x28, 0x1B, 0x8000},
  {0x28, 0x1C, 0x011E}, {0x28, 0x1D, 0x0000}, {0x28, 0x1E, 0xC8FF}, {0x28, 0x1F, 0x0000},
  {0x29, 0x00, 0xC000}, {0x29, 0x01, 0xF000}, {0x29, 0x02, 0x6010},
  {0x29, 0x12, 0x0EEE}, {0x29, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_3p125g_cmu[] =
{
  {0x2D, 0x03, 0x6410}, {0x2D, 0x04, 0x0000}, {0x2D, 0x05, 0x27D9}, {0x2D, 0x06, 0x58A6},
  {0x2D, 0x07, 0x2990}, {0x2D, 0x08, 0xFFF4}, {0x2D, 0x09, 0x3082}, {0x2D, 0x0A, 0x0000},
  {0x2D, 0x0B, 0x8000}, {0x2D, 0x0C, 0x6424}, {0x2D, 0x0D, 0x0000}, {0x2D, 0x0E, 0x0000},
  {0x2D, 0x0F, 0xA470}, {0x2D, 0x10, 0x8000}, {0x2D, 0x11, 0x037B}
};

sds_config dal_mango_construct_ana_2p5g[] =
{
  {0x26, 0x00, 0x0104}, {0x26, 0x01, 0x0200}, {0x26, 0x02, 0x2A24}, {0x26, 0x03, 0xD10D},
  {0x26, 0x04, 0xD550}, {0x26, 0x05, 0xA95E}, {0x26, 0x06, 0xE31D}, {0x26, 0x07, 0x000E},
  {0x26, 0x08, 0x0294}, {0x26, 0x09, 0x04E4}, {0x26, 0x0A, 0x7FC8}, {0x26, 0x0B, 0xE0E7},
  {0x26, 0x0C, 0x0200}, {0x26, 0x0D, 0xDF80}, {0x26, 0x0E, 0x0800}, {0x26, 0x0F, 0x1FD8},
  {0x26, 0x10, 0x0C3F}, {0x26, 0x11, 0x0000}, {0x26, 0x12, 0x27C0}, {0x26, 0x13, 0x7F1C},
  {0x26, 0x14, 0x1300}, {0x26, 0x15, 0x003F}, {0x26, 0x16, 0xBE7F}, {0x26, 0x17, 0x0090},
  {0x26, 0x18, 0x0000}, {0x26, 0x19, 0x407F}, {0x26, 0x1A, 0x0000}, {0x26, 0x1B, 0x8000},
  {0x26, 0x1C, 0x011E}, {0x26, 0x1D, 0x0000}, {0x26, 0x1E, 0xC8FF}, {0x26, 0x1F, 0x0000},
  {0x27, 0x00, 0xC000}, {0x27, 0x01, 0xF000}, {0x27, 0x02, 0x6010}, {0x27, 0x03, 0x6410},
  {0x27, 0x05, 0x27D9}, {0x27, 0x07, 0x2990}, {0x27, 0x08, 0xFFF4}, {0x27, 0x09, 0x3082},
  {0x27, 0x0C, 0x6424}, {0x27, 0x11, 0x037B}, {0x27, 0x12, 0x0EEE}, {0x27, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_1p25g[] =
{
  {0x24, 0x00, 0x0104}, {0x24, 0x01, 0x0200}, {0x24, 0x02, 0x2A24}, {0x24, 0x03, 0xD10D},
  {0x24, 0x04, 0xD550}, {0x24, 0x05, 0xA95E}, {0x24, 0x06, 0xE31D}, {0x24, 0x07, 0x800E},
  {0x24, 0x08, 0x0294}, {0x24, 0x09, 0x04E4}, {0x24, 0x0A, 0x7FC8}, {0x24, 0x0B, 0xE0E7},
  {0x24, 0x0C, 0x0200}, {0x24, 0x0D, 0x9F80}, {0x24, 0x0E, 0x0000}, {0x24, 0x0F, 0x1FF0},
  {0x24, 0x10, 0x0C3F}, {0x24, 0x11, 0x0000}, {0x24, 0x12, 0x27C0}, {0x24, 0x13, 0x7F1C},
  {0x24, 0x14, 0x1300}, {0x24, 0x15, 0x003F}, {0x24, 0x16, 0xBE7F}, {0x24, 0x17, 0x0090},
  {0x24, 0x18, 0x0000}, {0x24, 0x19, 0x407F}, {0x24, 0x1A, 0x0000}, {0x24, 0x1B, 0x8000},
  {0x24, 0x1C, 0x011E}, {0x24, 0x1D, 0x0000}, {0x24, 0x1E, 0xC8FF}, {0x24, 0x1F, 0x0000},
  {0x25, 0x00, 0xC000}, {0x25, 0x01, 0xF000}, {0x25, 0x02, 0x6010},
  {0x25, 0x12, 0x0EEE}, {0x25, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana2[] =
{
     {0x20, 0x12, 0x150F},
     {0x2E, 0x7 , 0x800E},
     {0x2A, 0x7 , 0x800E},
     {0x24, 0x7 , 0x000E},
     {0x26, 0x7 , 0x000E},
     {0x28, 0x7 , 0x000E},

     {0x2F, 0x12, 0x0AAA},

     {0x2A, 0x12, 0x2740},
     {0x2B, 0x0 , 0x0   },
     {0x2B, 0x2 , 0x2010},

     {0x2F, 0x3 , 0x84A0},
     {0x2F, 0xC , 0x84A4},

     {0x24, 0xD , 0xDF80},
     {0x2A, 0xD , 0xDF80},

     {0x2F, 0x5 , 0x2FD9},
     {0x2F, 0x5 , 0x3FD9},
     {0x21, 0x16, 0x1065},
     {0x21, 0x16, 0x1067},

     {0x21, 0x19, 0xF0A5},
};

sds_config dal_mango_construct_ana_common_type1[] =
{
    {0x21, 0x00, 0x1800}, {0x21, 0x01, 0x0060}, {0x21, 0x02, 0x3000}, {0x21, 0x03, 0xFFFF},
    {0x21, 0x04, 0x0603}, {0x21, 0x05, 0x1104}, {0x21, 0x06, 0x4444}, {0x21, 0x07, 0x7044},
    {0x21, 0x08, 0xF104}, {0x21, 0x09, 0xF104}, {0x21, 0x0A, 0xF104}, {0x21, 0x0B, 0x0003},
    {0x21, 0x0C, 0x007F}, {0x21, 0x0D, 0x3FE4}, {0x21, 0x0E, 0x31F9}, {0x21, 0x0F, 0x0618},
    {0x21, 0x10, 0x1FF8}, {0x21, 0x11, 0x7C9F}, {0x21, 0x12, 0x7C9F}, {0x21, 0x13, 0x13FF},
    {0x21, 0x14, 0x001F}, {0x21, 0x15, 0x01F0}, {0x21, 0x16, 0x1064}, {0x21, 0x17, 0x8AF1},
    {0x21, 0x18, 0x210A}, {0x21, 0x19, 0xF0F1}
};

sds_config dal_mango_construct_ana_10p3125g_type1[] =
{
    {0x2E, 0x00, 0x0107}, {0x2E, 0x01, 0x01A3}, {0x2E, 0x02, 0x6A24}, {0x2E, 0x03, 0xD10D},
    {0x2E, 0x04, 0x8000}, {0x2E, 0x05, 0xA17E}, {0x2E, 0x06, 0xE31D}, {0x2E, 0x07, 0x800E},
    {0x2E, 0x08, 0x0294}, {0x2E, 0x09, 0x0CE4}, {0x2E, 0x0A, 0x7FC8}, {0x2E, 0x0B, 0xE0E7},
    {0x2E, 0x0C, 0x0200}, {0x2E, 0x0D, 0xDF80}, {0x2E, 0x0E, 0x0000}, {0x2E, 0x0F, 0x1FC2},
    {0x2E, 0x10, 0x0C3F}, {0x2E, 0x11, 0x0000}, {0x2E, 0x12, 0x27C0}, {0x2E, 0x13, 0x7E1D},
    {0x2E, 0x14, 0x1300}, {0x2E, 0x15, 0x003F}, {0x2E, 0x16, 0xBE7F}, {0x2E, 0x17, 0x0090},
    {0x2E, 0x18, 0x0000}, {0x2E, 0x19, 0x4000}, {0x2E, 0x1A, 0x0000}, {0x2E, 0x1B, 0x8000},
    {0x2E, 0x1C, 0x011F}, {0x2E, 0x1D, 0x0000}, {0x2E, 0x1E, 0xC8FF}, {0x2E, 0x1F, 0x0000},
    {0x2F, 0x00, 0xC000}, {0x2F, 0x01, 0xF000}, {0x2F, 0x02, 0x6010},
    {0x2F, 0x12, 0x0EE7}, {0x2F, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_10p3125g_cmu_type1[] =
{
    {0x2F, 0x03, 0x4210}, {0x2F, 0x04, 0x0000}, {0x2F, 0x05, 0x0019}, {0x2F, 0x06, 0x18A6},
    {0x2F, 0x07, 0x2990}, {0x2F, 0x08, 0xFFF4}, {0x2F, 0x09, 0x1F08}, {0x2F, 0x0A, 0x0000},
    {0x2F, 0x0B, 0x8000}, {0x2F, 0x0C, 0x4224}, {0x2F, 0x0D, 0x0000}, {0x2F, 0x0E, 0x0000},
    {0x2F, 0x0F, 0xA470}, {0x2F, 0x10, 0x8000}, {0x2F, 0x11, 0x037B}
};

sds_config dal_mango_construct_ana_5g_type1[] =
{
    {0x2A, 0x00, 0xF904}, {0x2A, 0x01, 0x0200}, {0x2A, 0x02, 0x2A20}, {0x2A, 0x03, 0xD10D},
    {0x2A, 0x04, 0x8000}, {0x2A, 0x05, 0xA17E}, {0x2A, 0x06, 0xE115}, {0x2A, 0x07, 0x000E},
    {0x2A, 0x08, 0x0294}, {0x2A, 0x09, 0x28E4}, {0x2A, 0x0A, 0x7FC8}, {0x2A, 0x0B, 0xE0E7},
    {0x2A, 0x0C, 0x0200}, {0x2A, 0x0D, 0xDF80}, {0x2A, 0x0E, 0x0000}, {0x2A, 0x0F, 0x1FC8},
    {0x2A, 0x10, 0x0C3F}, {0x2A, 0x11, 0x0000}, {0x2A, 0x12, 0x27C0}, {0x2A, 0x13, 0x7E1D},
    {0x2A, 0x14, 0x1300}, {0x2A, 0x15, 0x003F}, {0x2A, 0x16, 0xBE7F}, {0x2A, 0x17, 0x0090},
    {0x2A, 0x18, 0x0000}, {0x2A, 0x19, 0x407F}, {0x2A, 0x1A, 0x0000}, {0x2A, 0x1B, 0x8000},
    {0x2A, 0x1C, 0x011E}, {0x2A, 0x1D, 0x0000}, {0x2A, 0x1E, 0xC8FF}, {0x2A, 0x1F, 0x0000},
    {0x2B, 0x00, 0xC000}, {0x2B, 0x01, 0xF000}, {0x2B, 0x02, 0x6010},
    {0x2B, 0x12, 0x0EE7}, {0x2B, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_5g_cmu_type1[] =
{
    {0x2B, 0x03, 0x5010}, {0x2B, 0x04, 0x0000}, {0x2B, 0x05, 0x0019}, {0x2B, 0x06, 0x18A6},
    {0x2B, 0x07, 0x2990}, {0x2B, 0x08, 0xFF84}, {0x2B, 0x09, 0x2682}, {0x2B, 0x0A, 0x0000},
    {0x2B, 0x0B, 0x8000}, {0x2B, 0x0C, 0x5024}, {0x2B, 0x0D, 0x0000}, {0x2B, 0x0E, 0x0000},
    {0x2B, 0x0F, 0xA470}, {0x2B, 0x10, 0x8000}, {0x2B, 0x11, 0x0362}
};

sds_config dal_mango_construct_ana_3p125g_type1[] =
{
    {0x28, 0x00, 0xF904}, {0x28, 0x01, 0x0200}, {0x28, 0x02, 0x2A20}, {0x28, 0x03, 0xD10D},
    {0x28, 0x04, 0x8000}, {0x28, 0x05, 0xA17E}, {0x28, 0x06, 0xE115}, {0x28, 0x07, 0x000E},
    {0x28, 0x08, 0x0294}, {0x28, 0x09, 0x04E4}, {0x28, 0x0A, 0x7FC8}, {0x28, 0x0B, 0xE0E7},
    {0x28, 0x0C, 0x0200}, {0x28, 0x0D, 0xDF80}, {0x28, 0x0E, 0x0000}, {0x28, 0x0F, 0x1FD8},
    {0x28, 0x10, 0x0C3F}, {0x28, 0x11, 0x0000}, {0x28, 0x12, 0x27C0}, {0x28, 0x13, 0x7E1D},
    {0x28, 0x14, 0x1300}, {0x28, 0x15, 0x003F}, {0x28, 0x16, 0xBE7F}, {0x28, 0x17, 0x0090},
    {0x28, 0x18, 0x0000}, {0x28, 0x19, 0x407F}, {0x28, 0x1A, 0x0000}, {0x28, 0x1B, 0x8000},
    {0x28, 0x1C, 0x011E}, {0x28, 0x1D, 0x0000}, {0x28, 0x1E, 0xC8FF}, {0x28, 0x1F, 0x0000},
    {0x29, 0x00, 0xC000}, {0x29, 0x01, 0xF000}, {0x29, 0x02, 0x6010},
    {0x29, 0x12, 0x0EE7}, {0x29, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_3p125g_cmu_type1[] =
{
    {0x2D, 0x03, 0x6410}, {0x2D, 0x04, 0x0000}, {0x2D, 0x05, 0x0019}, {0x2D, 0x06, 0x18A6},
    {0x2D, 0x07, 0x2990}, {0x2D, 0x08, 0xFF84}, {0x2D, 0x09, 0x3082}, {0x2D, 0x0A, 0x0000},
    {0x2D, 0x0B, 0x8000}, {0x2D, 0x0C, 0x6424}, {0x2D, 0x0D, 0x0000}, {0x2D, 0x0E, 0x0000},
    {0x2D, 0x0F, 0xA470}, {0x2D, 0x10, 0x8000}, {0x2D, 0x11, 0x037B}
};

sds_config dal_mango_construct_ana_2p5g_type1[] =
{
    {0x26, 0x00, 0xF904}, {0x26, 0x01, 0x0200}, {0x26, 0x02, 0x2A20}, {0x26, 0x03, 0xD10D},
    {0x26, 0x04, 0x8000}, {0x26, 0x05, 0xA17E}, {0x26, 0x06, 0xE115}, {0x26, 0x07, 0x000E},
    {0x26, 0x08, 0x0294}, {0x26, 0x09, 0x04E4}, {0x26, 0x0A, 0x7FC8}, {0x26, 0x0B, 0xE0E7},
    {0x26, 0x0C, 0x0200}, {0x26, 0x0D, 0xDF80}, {0x26, 0x0E, 0x0000}, {0x26, 0x0F, 0x1FE0},
    {0x26, 0x10, 0x0C3F}, {0x26, 0x11, 0x0000}, {0x26, 0x12, 0x27C0}, {0x26, 0x13, 0x7E1D},
    {0x26, 0x14, 0x1300}, {0x26, 0x15, 0x003F}, {0x26, 0x16, 0xBE7F}, {0x26, 0x17, 0x0090},
    {0x26, 0x18, 0x0000}, {0x26, 0x19, 0x407F}, {0x26, 0x1A, 0x0000}, {0x26, 0x1B, 0x8000},
    {0x26, 0x1C, 0x011E}, {0x26, 0x1D, 0x0000}, {0x26, 0x1E, 0xC8FF}, {0x26, 0x1F, 0x0000},
    {0x27, 0x00, 0xC000}, {0x27, 0x01, 0xF000}, {0x27, 0x02, 0x6010},
    {0x27, 0x12, 0x0EE7}, {0x27, 0x13, 0x0000}
};

sds_config dal_mango_construct_ana_1p25g_type1[] =
{
    {0x24, 0x00, 0xF904}, {0x24, 0x01, 0x0200}, {0x24, 0x02, 0x2A20}, {0x24, 0x03, 0xD10D},
    {0x24, 0x04, 0x8000}, {0x24, 0x05, 0xA17E}, {0x24, 0x06, 0xE115}, {0x24, 0x07, 0x000E},
    {0x24, 0x08, 0x0294}, {0x24, 0x09, 0x84E4}, {0x24, 0x0A, 0x7FC8}, {0x24, 0x0B, 0xE0E7},
    {0x24, 0x0C, 0x0200}, {0x24, 0x0D, 0xDF80}, {0x24, 0x0E, 0x0000}, {0x24, 0x0F, 0x1FF0},
    {0x24, 0x10, 0x0C3F}, {0x24, 0x11, 0x0000}, {0x24, 0x12, 0x27C0}, {0x24, 0x13, 0x7E1D},
    {0x24, 0x14, 0x1300}, {0x24, 0x15, 0x003F}, {0x24, 0x16, 0xBE7F}, {0x24, 0x17, 0x0090},
    {0x24, 0x18, 0x0000}, {0x24, 0x19, 0x407F}, {0x24, 0x1A, 0x0000}, {0x24, 0x1B, 0x8000},
    {0x24, 0x1C, 0x011E}, {0x24, 0x1D, 0x0000}, {0x24, 0x1E, 0xC8FF}, {0x24, 0x1F, 0x0000},
    {0x25, 0x00, 0xC000}, {0x25, 0x01, 0xF000}, {0x25, 0x02, 0x6010},
    {0x25, 0x12, 0x0EE7}, {0x25, 0x13, 0x0000}
};

const static uint16 egrQBwBurst_fieldidx[] = {MANGO_EGR_Q_BW_MAX_LB_BURST_Q0tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q1tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q2tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q3tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q4tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q5tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q6tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q7tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q8tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q9tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q10tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q11tf};

const static uint16 egrAssureQBwBurst_fieldidx[] = {MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q0tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q1tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q2tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q3tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q4tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q5tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q6tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q7tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q8tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q9tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q10tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q11tf};

static uint32 _actIpRouteCtrlNhAgeOut[] = {   /* MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_AGE_OUT_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static int32
_dal_mango_vlan_init_default(uint32 unit)
{
    int32       ret;
    rtk_portmask_t  member_portmask;
    rtk_portmask_t  untag_portmask;
    vlan_entry_t        vlan_entry;
    vlan_untag_entry_t  vlan_untag_entry;
    uint32  temp_var, value;
    rtk_port_t  port;

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));
    osal_memset(&vlan_untag_entry, 0, sizeof(vlan_untag_entry));

    /*** VLAN table ***/
    /* set  fid */
    temp_var = 0;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L2_TNL_LST_IDXtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set  msti */
    temp_var = RTK_DEFAULT_MSTI;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_MSTItf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set unicast lookup mode */
    temp_var = VLAN_L2_LOOKUP_MODE_VID;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L2_HKEY_UCASTtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set mulicast lookup mode */
    temp_var = VLAN_L2_LOOKUP_MODE_VID;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L2_HKEY_MCASTtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set profile index */
    temp_var = 0;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_VLAN_PROFILEtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set member set */
    HWP_GET_ALL_PORTMASK(unit, member_portmask);
    RTK_PORTMASK_PORT_CLEAR(member_portmask, HWP_CPU_MACID(unit));
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_MBRtf, member_portmask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set group mask */
    temp_var = 0;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_GROUP_MASKtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set L3 interface ID */
    temp_var = 0;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L3_INTF_IDtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* set BSSID List Valid */
    temp_var = 0;
    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L2_TNL_LST_VALIDtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* programming vlan entry to chip (RTK_DEFAULT_VLAN_ID) */
    if ((ret = table_write(unit, MANGO_VLANt, RTK_DEFAULT_VLAN_ID, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /*** VLAN untag member table ***/
    /* set untagged member set */
    HWP_GET_ALL_PORTMASK(unit, untag_portmask);
    if ((ret = table_field_set(unit, MANGO_VLAN_UNTAGt, MANGO_VLAN_UNTAG_UNTAGtf, untag_portmask.bits, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* programming vlan untag entry to chip (RTK_DEFAULT_VLAN_ID) */
    if ((ret = table_write(unit, MANGO_VLAN_UNTAGt, RTK_DEFAULT_VLAN_ID, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Port-based default value */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        value = 0;  /* OPVID_FMT, IPVID_FMT */
        if ((ret = reg_array_field_write(unit, MANGO_VLAN_PORT_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_OPVID_FMTf, &value)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = reg_array_field_write(unit, MANGO_VLAN_PORT_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_IPVID_FMTf, &value)) != RT_ERR_OK)
        {
            return ret;
        }

        temp_var = RTK_DEFAULT_VLAN_SA_LRN_MODE;

        if (VLAN_SALRN_MODE_LEARN_ALL_END == temp_var)
        {
            value = 0;
        } else if (VLAN_SALRN_MODE_LEARN_BY_VLAN_MEMBER == temp_var) {
            value = 1;
        } else {
            value = 1; /* default: sa learn by vlan member */
        }

        if ((ret = reg_array_field_write(unit, MANGO_VLAN_PORT_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SA_LRN_MODEf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_trunk_init_config(uint32 unit)
{
    int32   ret;
    uint32 algo_idx;
    uint32 val;

    for (algo_idx = 0; algo_idx < 2; algo_idx ++)
    {
        val = RTK_DEFAULT_TRUNK_DISTRIBUTION_L2_ALGORITHM;

        if ((ret = reg_array_field_write(unit,
                          MANGO_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MANGO_L2_HASH_MSKf,
                          &val)) != RT_ERR_OK)
        {
            return ret;
        }

        val = RTK_DEFAULT_TRUNK_DISTRIBUTION_L3_ALGORITHM;

        if ((ret = reg_array_field_write(unit,
                          MANGO_TRK_HASH_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          algo_idx,
                          MANGO_L3_HASH_MSKf,
                          &val)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_mango_trunk_init_config */


static int32
_dal_mango_rate_init_config(uint32 unit)
{
    int32   ret;
    uint32  storm_burst = RTK_DEFAULT_STORM_CONTROL_BURST_SIZE;
    uint32  sysClk = 0;
    uint32  igrBwTick, egrBwTick, egrBwPPSTickCPU, stormTick, stormPPSTick, stormProtoTick;
    uint32  igrBwTkn, egrBwTkn, egrBwPPSTknCPU, stormTkn, stormPPSTkn;
    uint32  meterBpsTick, meterBpsTkn, meterPpsTick, meterPpsTkn;
    /* [MANGO-3439]-START - Only test chip need it. */
    rtk_port_t port;
    rtk_qid_t  queue;
    uint32  val = 0;
    egr_qBw_entry_t egrQEntry;

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    /* Set Port 52~55 Assured BW default value to be 0 */
    for (port = 52; port <= 55; port++)
    {
        val = 0;
        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, MANGO_EGR_Q_BW_ASSURED_BW_Q8tf, &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, MANGO_EGR_Q_BW_ASSURED_BW_Q9tf, &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, MANGO_EGR_Q_BW_ASSURED_BW_Q10tf, &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, MANGO_EGR_Q_BW_ASSURED_BW_Q11tf, &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
    }
    /* [MANGO-3439]-END */

    if ((ret = reg_field_read(unit, MANGO_MAC_L2_GLOBAL_CTRL2r, MANGO_SYS_CLK_SELf, &sysClk)) != RT_ERR_OK)
    {
        return ret;
    }

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));
    val = RTK_DEFAULT_EGR_BANDWIDTH_PORT_BURST;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &val)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
        for (queue = 0; queue < HAL_MAX_NUM_OF_STACK_QUEUE(unit); queue++)
        {
            if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwBurst_fieldidx[queue],
                            &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
            {
                return ret;
            }
            if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwBurst_fieldidx[queue],
                            &val, (uint32 *) &egrQEntry)) != RT_ERR_OK)
            {
                return ret;
            }
        }
        if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    port = HWP_CPU_MACID(unit);
    val = RTK_DEFAULT_EGR_BANDWIDTH_CPU_PORT_BURST;
    if ((ret = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &val)) != RT_ERR_OK)
    {
        return ret;
    }
    for (queue = 0; queue < HAL_MAX_NUM_OF_CPU_QUEUE(unit); queue++)
    {
        if ((ret = reg_array_field_write(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, MANGO_BURSTf, &val)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    if (sysClk == RATE_SYSCLK_325) /* 325MHz */
    {
        igrBwTick = MANGO_IGRBW_LB_TICK_325M;
        igrBwTkn = MANGO_IGRBW_LB_TKN_325M;
        egrBwTick = MANGO_EGRBW_LB_TICK_325M;
        egrBwTkn = MANGO_EGRBW_LB_TKN_325M;
        egrBwPPSTickCPU = MANGO_EGRBW_LB_CPU_PPS_TICK_325M;
        egrBwPPSTknCPU = MANGO_EGRBW_LB_CPU_PPS_TKN_325M;
        stormTick = MANGO_STORM_LB_TICK_325M;
        stormTkn = MANGO_STORM_LB_TKN_325M;
        stormPPSTick = MANGO_STORM_LB_PPS_TICK_325M;
        stormPPSTkn = MANGO_STORM_LB_PPS_TKN_325M;
        stormProtoTick = MANGO_STORM_LB_PROTO_TICK_325M;

        meterBpsTick = MANGO_METER_BPS_TICK_325M;
        meterBpsTkn = MANGO_METER_BPS_TKN_325M;
        meterPpsTick = MANGO_METER_PPS_TICK_325M;
        meterPpsTkn = MANGO_METER_PPS_TKN_325M;
    }
    else if (sysClk == RATE_SYSCLK_175) /* 175MHz */
    {
        igrBwTick = MANGO_IGRBW_LB_TICK_175M;
        igrBwTkn = MANGO_IGRBW_LB_TKN_175M;
        egrBwTick = MANGO_EGRBW_LB_TICK_175M;
        egrBwTkn = MANGO_EGRBW_LB_TKN_175M;
        egrBwPPSTickCPU = MANGO_EGRBW_LB_CPU_PPS_TICK_175M;
        egrBwPPSTknCPU = MANGO_EGRBW_LB_CPU_PPS_TKN_175M;
        stormTick = MANGO_STORM_LB_TICK_175M;
        stormTkn = MANGO_STORM_LB_TKN_175M;
        stormPPSTick = MANGO_STORM_LB_PPS_TICK_175M;
        stormPPSTkn = MANGO_STORM_LB_PPS_TKN_175M;
        stormProtoTick = MANGO_STORM_LB_PROTO_TICK_175M;

        meterBpsTick = MANGO_METER_BPS_TICK_175M;
        meterBpsTkn = MANGO_METER_BPS_TKN_175M;
        meterPpsTick = MANGO_METER_PPS_TICK_175M;
        meterPpsTkn = MANGO_METER_PPS_TKN_175M;
    }
    else /* 650MHz */
    {
        igrBwTick = MANGO_IGRBW_LB_TICK_650M;
        igrBwTkn = MANGO_IGRBW_LB_TKN_650M;
        egrBwTick = MANGO_EGRBW_LB_TICK_650M;
        egrBwTkn = MANGO_EGRBW_LB_TKN_650M;
        egrBwPPSTickCPU = MANGO_EGRBW_LB_CPU_PPS_TICK_650M;
        egrBwPPSTknCPU = MANGO_EGRBW_LB_CPU_PPS_TKN_650M;
        stormTick = MANGO_STORM_LB_TICK_650M;
        stormTkn = MANGO_STORM_LB_TKN_650M;
        stormPPSTick = MANGO_STORM_LB_PPS_TICK_650M;
        stormPPSTkn = MANGO_STORM_LB_PPS_TKN_650M;
        stormProtoTick = MANGO_STORM_LB_PROTO_TICK_650M;

        meterBpsTick = MANGO_METER_BPS_TICK_650M;
        meterBpsTkn = MANGO_METER_BPS_TKN_650M;
        meterPpsTick = MANGO_METER_PPS_TICK_650M;
        meterPpsTkn = MANGO_METER_PPS_TKN_650M;
    }

    if ((ret = reg_field_write(unit, MANGO_IGBW_LB_CTRLr, MANGO_TICKf, &igrBwTick)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_IGBW_LB_CTRLr, MANGO_TKNf, &igrBwTkn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_EGBW_LB_CTRLr, MANGO_TICKf, &egrBwTick)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_EGBW_LB_CTRLr, MANGO_TKNf, &egrBwTkn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_EGBW_CPU_PPS_LB_CTRLr, MANGO_TICKf, &egrBwPPSTickCPU)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_EGBW_CPU_PPS_LB_CTRLr, MANGO_TKNf, &egrBwPPSTknCPU)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_STORM_LB_CTRLr, MANGO_TICKf, &stormTick)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_STORM_LB_CTRLr, MANGO_TKNf, &stormTkn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_STORM_LB_PPS_CTRLr, MANGO_TICKf, &stormPPSTick)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_STORM_LB_PPS_CTRLr, MANGO_TKNf, &stormPPSTkn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_STORM_LB_PROTO_CTRLr, MANGO_TICKf, &stormProtoTick)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_ERR_CHK(reg_field_write(unit, MANGO_METER_BYTE_TB_CTRLr, MANGO_TICK_PERIODf,
            &meterBpsTick), ret);

    RT_ERR_CHK(reg_field_write(unit, MANGO_METER_BYTE_TB_CTRLr, MANGO_TKNf,
            &meterBpsTkn), ret);


    RT_ERR_CHK(reg_field_write(unit, MANGO_METER_PKT_TB_CTRLr, MANGO_TICK_PERIODf,
            &meterPpsTick), ret);

    RT_ERR_CHK(reg_field_write(unit, MANGO_METER_PKT_TB_CTRLr, MANGO_TKNf,
            &meterPpsTkn), ret);


    RT_ERR_CHK(reg_field_write(unit, MANGO_METER_PKT_TB_CTRLr, MANGO_TKNf,
            &meterPpsTkn), ret);

    HWP_PORT_TRAVS(unit, port)
    {
        if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_UC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &storm_burst)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_MC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &storm_burst)) != RT_ERR_OK)
        {
            return ret;
        }

        if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_BC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &storm_burst)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_mango_rate_init_config */

static int32
_dal_mango_swred_init_config(uint32 unit)
{
    uint32      maxThresh, minThresh, probability;
    rtk_qid_t   qid;
    int32       ret;

    for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
    {
        maxThresh = MANGO_SWRED_QUEUE_DP0_HIGH_THRESH;
        minThresh = MANGO_SWRED_QUEUE_DP0_LOW_THRESH;
        probability = MANGO_SWRED_QUEUE_DP0_DROP_RATE;

        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_DROP_RATEr, qid, 0, MANGO_RATEf, &probability)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,  qid, 0, MANGO_MAXf, &maxThresh)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,  qid, 0, MANGO_MINf, &minThresh)) != RT_ERR_OK)
        {
            return ret;
        }

        maxThresh = MANGO_SWRED_QUEUE_DP1_HIGH_THRESH;
        minThresh = MANGO_SWRED_QUEUE_DP1_LOW_THRESH;
        probability = MANGO_SWRED_QUEUE_DP1_DROP_RATE;
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_DROP_RATEr, qid, 1, MANGO_RATEf, &probability)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,  qid, 1, MANGO_MAXf, &maxThresh)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,  qid, 1, MANGO_MINf, &minThresh)) != RT_ERR_OK)
        {
            return ret;
        }

        maxThresh = MANGO_SWRED_QUEUE_DP2_HIGH_THRESH;
        minThresh = MANGO_SWRED_QUEUE_DP2_LOW_THRESH;
        probability = MANGO_SWRED_QUEUE_DP2_DROP_RATE;
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_DROP_RATEr, qid, 2, MANGO_RATEf, &probability)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,  qid, 2, MANGO_MAXf, &maxThresh)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_SWRED_Q_THRr,  qid, 2, MANGO_MINf, &minThresh)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_mango_swred_init_config */

static int32
_dal_mango_flowctrl_init_config(uint32 unit)
{
    int32       ret;
    uint32      idx, value, highOn, highOff, lowOn, lowOff, high, low, guar;
    rtk_qid_t   qid, maxQueue;
    rtk_port_t  port;

    /* Set Ingress Queue Drop Threshold */
    high = MANGO_IGBW_Q_DROP_THR_H;
    low = MANGO_IGBW_Q_DROP_THR_L;
    maxQueue = HAL_MAX_NUM_OF_IGR_QUEUE(unit);
    for (qid = 0; qid < maxQueue; qid++)
    {
        if ((ret = reg_array_field_write(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, qid, MANGO_HIGHf, &high)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE, qid, MANGO_LOWf, &low)) != RT_ERR_OK)
        {
            return ret;
        }
    }

#if 0
    /* Set System Threshold */
    highOff = MANGO_FC_S_THR_H_OFF;
    if ((ret = reg_field_write(unit, MANGO_FC_GLB_HI_THRr, MANGO_OFFf, &highOff)) != RT_ERR_OK)
    {
        return ret;
    }
#endif

    /* Set Jumbo-mode Egress Queue Drop Threshold */
    high = MANGO_JUMBO_EGBW_Q_DROP_THR_H;
    low = MANGO_JUMBO_EGBW_Q_DROP_THR_L;
    maxQueue = HAL_MAX_NUM_OF_QUEUE(unit);
    for (qid = 0; qid < maxQueue; qid++)
    {
        if ((ret = reg_array_field_write(unit, MANGO_FC_Q_EGR_DROP_THRr, qid, MANGO_JUMBO_EGBW_Q_THR_IDX, MANGO_ONf, &high)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_FC_Q_EGR_DROP_THRr, qid, MANGO_JUMBO_EGBW_Q_THR_IDX, MANGO_OFFf, &low)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    /* Set Jumbo-mode System Threshold */
    highOn = MANGO_FC_JUMBO_S_THR_H_ON;
    highOff = MANGO_FC_JUMBO_S_THR_H_OFF;
    lowOn = MANGO_FC_JUMBO_S_THR_L_ON;
    lowOff = MANGO_FC_JUMBO_S_THR_L_OFF;
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_HI_THRr, MANGO_ONf, &highOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_HI_THRr, MANGO_OFFf, &highOff)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_LO_THRr, MANGO_ONf, &lowOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_LO_THRr, MANGO_OFFf, &lowOff)) != RT_ERR_OK)
    {
        return ret;
    }
    highOn = MANGO_FC_OFF_JUMBO_S_THR_H_ON;
    highOff = MANGO_FC_OFF_JUMBO_S_THR_H_OFF;
    lowOn = MANGO_FC_OFF_JUMBO_S_THR_L_ON;
    lowOff = MANGO_FC_OFF_JUMBO_S_THR_L_OFF;
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, MANGO_ONf, &highOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, MANGO_OFFf, &highOff)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, MANGO_ONf, &lowOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, MANGO_OFFf, &lowOff)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Set Jumbo-mode Port based Threshold */
    highOn = MANGO_FC_JUMBO_P_THR_H_ON;
    highOff = MANGO_FC_JUMBO_P_THR_H_OFF;
    lowOn = MANGO_FC_JUMBO_P_THR_L_ON;
    lowOff = MANGO_FC_JUMBO_P_THR_L_OFF;
    guar = MANGO_FC_JUMBO_P_THR_GUAR;
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_ONf, &highOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_OFFf, &highOff)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_ONf, &lowOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_OFFf, &lowOff)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_GUAR_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_THRf, &guar)) != RT_ERR_OK)
    {
        return ret;
    }

    highOn = MANGO_FC_OFF_JUMBO_P_THR_H_ON;
    highOff = MANGO_FC_OFF_JUMBO_P_THR_H_OFF;
    lowOn = MANGO_FC_OFF_JUMBO_P_THR_L_ON;
    lowOff = MANGO_FC_OFF_JUMBO_P_THR_L_OFF;
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_ONf, &highOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_OFFf, &highOff)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_ONf, &lowOn)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, MANGO_FC_JUMBO_P_THR_IDX,
                            MANGO_OFFf, &lowOff)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Adjust Port based Low-On Threshold */
    lowOn = MANGO_FC_P_THR_L_ON;
    for (idx = 0; idx < MANGO_FC_JUMBO_P_THR_IDX; idx++)
    {
        if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, idx, MANGO_ONf, &lowOn)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, idx, MANGO_ONf, &lowOn)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    /* Set Replication Queue Drop Theshold */
    value = MANGO_FC_REPCT_Q_DROP_THR_H;
    if ((ret = reg_field_write(unit, MANGO_FC_REPCT_FCOFF_THRr, MANGO_DROP_ONf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    value = MANGO_FC_REPCT_Q_DROP_THR_L;
    if ((ret = reg_field_write(unit, MANGO_FC_REPCT_FCOFF_THRr, MANGO_DROP_OFFf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_FE_PORT(unit, port))
        {
            continue;
        }
        value = 40;
        if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_ACT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ALLOW_PAGE_CNTf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    /* HOL Preventation Packet Type Status */
    value = RTK_DEFAULT_FC_HOL_PKT_BC_STATUS;
    if ((ret = reg_field_write(unit, MANGO_FC_HOL_PRVNT_CTRLr, MANGO_BC_ENf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value = RTK_DEFAULT_FC_HOL_PKT_L2_MC_STATUS;
    if ((ret = reg_field_write(unit, MANGO_FC_HOL_PRVNT_CTRLr, MANGO_L2_MC_ENf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value = RTK_DEFAULT_FC_HOL_PKT_IP_MC_STATUS;
    if ((ret = reg_field_write(unit, MANGO_FC_HOL_PRVNT_CTRLr, MANGO_IP_MC_ENf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    value = RTK_DEFAULT_FC_HOL_PKT_UNKN_UC_STATUS;
    if ((ret = reg_field_write(unit, MANGO_FC_HOL_PRVNT_CTRLr, MANGO_UNKN_UC_ENf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_mango_flowctrl_init_config */

static int32
_dal_mango_qos_init_config(uint32 unit)
{
    int32       ret;
    uint32      value;
    rtk_port_t  port;
    rtk_qid_t   queue;
    rtk_pri_t priority;

    value = 0;
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_OPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_RMK_CTRLr, MANGO_IPRI_DFLT_CFGf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Outer tag to be DEI remarking target */
    value = RTK_DEFAULT_QOS_REMARK_DEI_SOURCE;
    HWP_PORT_TRAVS(unit, port)
    {
        if ((ret = reg_array_field_write(unit, MANGO_RMK_PORT_CTRLr
                            , port, REG_ARRAY_INDEX_NONE, MANGO_DEI_RMK_TAG_SELf, &value)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    /* Internal-priority to queue mapping */
    value = 0xfac688;
    if ((ret = reg_write(unit, MANGO_QM_INTPRI2QID_CTRLr, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Port-based default value */
    value = 1;  /* strict priority = enabled */
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        for (queue = 0; queue < HAL_MAX_NUM_OF_IGR_QUEUE(unit); queue++)
        {
            if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_SCHEDr, port, queue, MANGO_SPf, &value)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }

    port = HWP_CPU_MACID(unit);
    value = RTK_DEFAULT_QOS_SCHED_CPU_ALGORITHM;
    if ((ret = reg_array_field_write(unit, MANGO_SCHED_PORT_ALGO_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_SCHED_TYPEf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    priority = RTK_DEFAULT_QOS_AVB_NONA_RMK_PRI;
    if ((ret = reg_field_write(unit, MANGO_AVB_CTRL_MACr, MANGO_CLASS_NON_A_RMK_PRIf, &priority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    priority = RTK_DEFAULT_QOS_AVB_NONB_RMK_PRI;
    if ((ret = reg_field_write(unit, MANGO_AVB_CTRL_MACr, MANGO_CLASS_NON_B_RMK_PRIf, &priority)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    /*SS-1347*/
    for (queue = 0; queue < HAL_MAX_NUM_OF_QUEUE(unit); queue++)
    {
        RT_ERR_CHK(reg_array_field_write(unit, MANGO_QM_CPUQID2QID_CTRLr, REG_ARRAY_INDEX_NONE, queue, MANGO_QIDf, &queue), ret);
    }

    for (queue = 0; queue < HAL_MAX_NUM_OF_STACK_QUEUE(unit); queue++)
    {
        RT_ERR_CHK(reg_array_field_write(unit, MANGO_QM_CPUQID2XGSQID_CTRLr, REG_ARRAY_INDEX_NONE, queue, MANGO_QIDf, &queue), ret);
    }


    return RT_ERR_OK;
}

static int32
_dal_mango_l2_init_config(uint32 unit)
{
    int32           ret;
    uint32          val, idx;
    rtk_port_t      port;
    rtk_portmask_t  portmask;

    /* reset learning counter */
    val = 0;
    reg_field_write(unit, MANGO_L2_LRN_CONSTRT_CNTr, MANGO_LRN_CNTf, &val);
    HWP_ETHER_PORT_TRAVS(unit, port)
        reg_array_field_write(unit, MANGO_L2_LRN_PORT_CONSTRT_CNTr, port, REG_ARRAY_INDEX_NONE, MANGO_LRN_CNTf, &val);
    for (idx = 0; idx < HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit); idx++)
        reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, idx, MANGO_LRN_CNTf, &val);


    HWP_GET_ALL_PORTMASK(unit, portmask);
    if ((ret = reg_field_write(unit, MANGO_L2_UNKN_UC_FLD_PMSKr, MANGO_PMSK_0f, &portmask.bits[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_L2_UNKN_UC_FLD_PMSKr, MANGO_PMSK_1f, &portmask.bits[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    if ((ret = reg_field_write(unit, MANGO_L2_BC_FLD_PMSKr, MANGO_PMSK_0f, &portmask.bits[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MANGO_L2_BC_FLD_PMSKr, MANGO_PMSK_1f, &portmask.bits[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    val = RTK_DEFAULT_L2_DA_BLOCK_PORT_STATE;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        reg_array_field_write(unit, MANGO_L2_PORT_DABLK_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &val);
    val = RTK_DEFAULT_L2_SA_BLOCK_PORT_STATE;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        reg_array_field_write(unit, MANGO_L2_PORT_SABLK_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &val);

    return RT_ERR_OK;
}

static int32
_dal_mango_l3_init_config(uint32 unit)
{
    int32       ret = RT_ERR_OK;
    uint32      value;

    if ((ret = rt_util_actListIndex_get(_actIpRouteCtrlNhAgeOut, (sizeof(_actIpRouteCtrlNhAgeOut)/sizeof(uint32)), &(value), RTK_DEFAULT_L3_NH_AGEOUT_ACT)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_AGE_OUT_ACTf, &value)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_switch_init_config(uint32 unit)
{
    int32           ret;
    uint32          val;

    val = RTK_DEFAULT_SWITCH_CPU_PORT_TX_MAX_LEN;
    if ((ret = reg_field_write(unit, MANGO_MAC_L2_CPU_MAX_LEN_CTRLr, MANGO_CPU_PORT_TX_MAX_LENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_construct_eee_init(uint32 unit)
{
    uint32  val;
    int32   ret;

    val = 0x1E;
    RT_ERR_CHK(reg_field_write(unit, MANGO_EEE_TX_TIMER_2P5G_5G_LITE_CTRLr,
            MANGO_TX_WAKE_TIMER_5G_LITEf, &val), ret);

    return ret;
}   /* end of _dal_mango_construct_eee_init */

/* Function Name:
 *      dal_mango_construct_default_init
 * Description:
 *      Init chip default value
 * Input:
 *      unit - which unit
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_mango_construct_default_init(uint32 unit)
{
    uint32 val;
    uint32 chk;

    /* memory reset */
    val = 1;
    reg_field_write(unit, MANGO_MEM_ENCAP_INITr, MANGO_MEM_INITf, &val);
    if (!IF_CHIP_TYPE_1(unit))
    {
        /* currently, cannot check it (it won't be cleared) */
        do { reg_field_read(unit, MANGO_MEM_ENCAP_INITr, MANGO_MEM_INITf, &chk); } while ((chk & val) != 0);
    }

    reg_field_write(unit, MANGO_MEM_MIB_INITr, MANGO_MEM_RSTf, &val);
    do { reg_field_read(unit, MANGO_MEM_MIB_INITr, MANGO_MEM_RSTf, &chk); } while ((chk & val) != 0);
    reg_field_write(unit, MANGO_MEM_ACL_INITr, MANGO_MEM_INITf, &val);
    do { reg_field_read(unit, MANGO_MEM_ACL_INITr, MANGO_MEM_INITf, &chk); } while ((chk & val) != 0);

    val = 0xFFFFFFFF;
    reg_write(unit, MANGO_MEM_ALE_INIT_0r, &val);
    do { reg_read(unit, MANGO_MEM_ALE_INIT_0r, &chk); } while ((chk & val) != 0);
    val = 0x7F;
    reg_write(unit, MANGO_MEM_ALE_INIT_1r, &val);
#if !defined(CONFIG_SDK_FPGA_PLATFORM)
    /* On FPGA platform, it may not be ready (ignore it) */
    do { reg_read(unit, MANGO_MEM_ALE_INIT_1r, &chk); } while ((chk & val) != 0);
#endif
    val = 0x7FF;
    reg_write(unit, MANGO_MEM_ALE_INIT_2r, &val);
    do { reg_read(unit, MANGO_MEM_ALE_INIT_2r, &chk); } while ((chk & val) != 0);

    /* set ESD auto recovery status reg to be level trigger */
    val = 1;
    reg_write(unit, MANGO_MDX_CTRL_RSVDr, &val);

    /* switch */
    _dal_mango_switch_init_config(unit);

    /* VLAN default entry */
    _dal_mango_vlan_init_default(unit);

    /* Dying Gasp Polarity */
    val = 1;
    reg_write(unit, MANGO_DYING_GASP_POLARITY_CTRLr, &val);
    reg_field_write(unit, MANGO_ISR_MISCr, MANGO_ISR_OAM_DYGASPf, &val);

    /* LINK Delay */
    val = 1;
    reg_field_write(unit, MANGO_LINK_DELAY_CTRLr, MANGO_LNKDN_FRC_DISf, &val);

    /* BPE*/
    val = 1;
    reg_array_field_write(unit, MANGO_PE_ETAG_RMK_CTRLr, REG_ARRAY_INDEX_NONE, 0, MANGO_PRIf, &val);
    val = 0;
    reg_array_field_write(unit, MANGO_PE_ETAG_RMK_CTRLr, REG_ARRAY_INDEX_NONE, 1, MANGO_PRIf, &val);

    /* RMA */
    val = 0;
    reg_field_write(unit, MANGO_RMA_CTRL_0r, MANGO_RMA_02_ACTf, &val);

    /* IPG 4N Byte Compensation */
    val = 1;
    reg_field_write(unit, MANGO_MAC_L2_GLOBAL_CTRL1r, MANGO_IPG_10G_4NBYTE_COMPS_ENf, &val);
    reg_field_write(unit, MANGO_MAC_L2_GLOBAL_CTRL1r, MANGO_IPG_5G_4NBYTE_COMPS_ENf, &val);

    /* Parser */
    val = 1; /* PPPoE-IP-parse enable */
    reg_field_write(unit, MANGO_PARSER_CTRLr, MANGO_PPPOE_PARSE_ENf, &val);
    val = 0; /* Disable CAPWAP parsing */
    reg_field_write(unit, MANGO_CAPWAP_UDP_PORTr, MANGO_CTRLf, &val);
    reg_field_write(unit, MANGO_CAPWAP_UDP_PORTr, MANGO_DATAf, &val);

    /* Disable limit-pause */
    val = 0;
    reg_field_write(unit, MANGO_MAC_L2_GLOBAL_CTRL1r, MANGO_LIMIT_PAUSE_ENf, &val);

    /* Fiber unidirectional */
    val = 0;
    reg_field_write(unit, MANGO_MAC_L2_GLOBAL_CTRL1r, MANGO_FIB_UNIDIR_ONLY_CPUTX_ENf, &val);

    /* Disable MAC_GLB_CTRL.MAC_48PASS1_DROP_EN */
    val = 0;
    reg_field_write(unit, MANGO_MAC_GLB_CTRLr, MANGO_MAC_48PASS1_DROP_ENf, &val);

    /* Trunk */
    _dal_mango_trunk_init_config(unit);

    /* Rate Leaky Bucket default config */
    _dal_mango_rate_init_config(unit);

    /* SWRED threshold */
    _dal_mango_swred_init_config(unit);

    /* Flow Control threshold */
    _dal_mango_flowctrl_init_config(unit);

    /* QoS */
    _dal_mango_qos_init_config(unit);

    /* L2 */
    _dal_mango_l2_init_config(unit);

    /* L3 */
    _dal_mango_l3_init_config(unit);

    /* EEE */
    _dal_mango_construct_eee_init(unit);

    //MAC drain out packet while remote fault received
    reg_read(unit, MANGO_MAC_CTRL_RSVDr, &val);
    val |= (1 << 15);
    reg_write(unit, MANGO_MAC_CTRL_RSVDr, &val);

    return;
}

static int32 _dal_mango_construct_mac_usxgmii_10gs(uint32 unit, uint32 sds)
{
    uint32  val = 1;
    int32   ret;

    switch (sds)
    {
        case 2:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_1G_ENf, &val), ret);
            break;
        case 3:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_1G_ENf, &val), ret);
            break;
        case 4:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_1G_ENf, &val), ret);
            break;
        case 5:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_1G_ENf, &val), ret);
            break;
        case 6:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_1G_ENf, &val), ret);
            break;
        case 7:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_1G_ENf, &val), ret);
            break;
        case 8:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP6_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP6_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP6_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP6_MAC0_1G_ENf, &val), ret);
            break;
        case 9:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP7_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP7_MAC0_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP7_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP7_MAC0_1G_ENf, &val), ret);
            break;
        case 10:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP8_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP8_MAC_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP8_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP8_MAC_1G_ENf, &val), ret);
            break;
        case 11:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP9_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP9_MAC_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP9_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP9_MAC_1G_ENf, &val), ret);
            break;
        case 12:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP10_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP10_MAC_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP10_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP10_MAC_1G_ENf, &val), ret);
            break;
        case 13:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP11_MAC_10G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP11_MAC_5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP11_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP11_MAC_1G_ENf, &val), ret);
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}   /* end of _dal_mango_construct_mac_usxgmii_10gs */

static int32 _dal_mango_construct_mac_usxgmii_10gq(uint32 unit, uint32 sds)
{
    uint32  val;
    int32   ret;

    val = 2;
    RT_ERR_CHK(reg_array_field_write(unit, MANGO_USXGMII_SUBMODE_CTRLr,
            REG_ARRAY_INDEX_NONE, sds, MANGO_USXGMII_SUBMODEf, &val), ret);

    val = 1;
    switch (sds)
    {
        case 2:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC1_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC1_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC2_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC2_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC3_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC3_1G_ENf, &val), ret);

            if (HWP_PORT_SDSID(unit, 4) == sds || HWP_PORT_SDSID(unit, 5) == sds)
            {
                val = 2;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P4_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES2_2_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P5_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES2_3_GMII_SELf, &val), ret);
            }
            break;
        case 3:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC1_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC1_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC2_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC2_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC3_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC3_1G_ENf, &val), ret);

            if (HWP_PORT_SDSID(unit, 12) == sds || HWP_PORT_SDSID(unit, 13) == sds)
            {
                val = 2;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P12_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES3_2_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P13_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES3_3_GMII_SELf, &val), ret);
            }
            break;
        case 4:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC1_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC1_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC2_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC2_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC3_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC3_1G_ENf, &val), ret);

            if (HWP_PORT_SDSID(unit, 20) == sds || HWP_PORT_SDSID(unit, 21) == sds)
            {
                val = 1;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P20_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES4_2_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P21_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES4_3_GMII_SELf, &val), ret);
            }
            break;
        case 5:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC1_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC1_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC2_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC2_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC3_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC3_1G_ENf, &val), ret);

            if (HWP_PORT_SDSID(unit, 28) == sds || HWP_PORT_SDSID(unit, 29) == sds)
            {
                val = 1;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P28_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES5_2_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P29_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES5_3_GMII_SELf, &val), ret);
            }
            break;
        case 6:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC1_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC1_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC2_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC2_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC3_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC3_1G_ENf, &val), ret);

            if (HWP_PORT_SDSID(unit, 36) == sds || HWP_PORT_SDSID(unit, 37) == sds)
            {
                val = 1;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P36_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES6_2_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P37_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES6_3_GMII_SELf, &val), ret);
            }
            break;
        case 7:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC1_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC1_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC2_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC2_1G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC3_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC3_1G_ENf, &val), ret);

            if (HWP_PORT_SDSID(unit, 44) == sds || HWP_PORT_SDSID(unit, 45) == sds)
            {
                val = 1;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P44_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES7_2_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P45_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES7_3_GMII_SELf, &val), ret);
            }
            break;
        case 9:
            if (HWP_PORT_SDSID(unit, 42) == sds || HWP_PORT_SDSID(unit, 43) == sds ||
                    HWP_PORT_SDSID(unit, 46) == sds || HWP_PORT_SDSID(unit, 47) == sds)
            {
                val = 1;
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P42_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P43_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P46_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_0r, MANGO_P47_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_P50_GMII_SELf, &val), ret);
                RT_ERR_CHK(reg_field_write(unit, MANGO_PROT_SERDSE_MUX_CTRL_1r, MANGO_SERDES9_0_GMII_SELf, &val), ret);
            }
            break;
        default:
            return RT_ERR_OK;
    }

    return ret;
}   /* end of _dal_mango_construct_mac_usxgmii_10gq */

static int32 _dal_mango_construct_mac_hisgmii(uint32 unit, uint32 sds)
{
    uint32  val = 1;
    int32   ret;

    switch (sds)
    {
        case 2:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP0_MAC0_1G_ENf, &val), ret);
            break;
        case 3:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP0_1_CTRLr, MANGO_GROUP1_MAC0_1G_ENf, &val), ret);
            break;
        case 4:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP2_MAC0_1G_ENf, &val), ret);
            break;
        case 5:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP2_3_CTRLr, MANGO_GROUP3_MAC0_1G_ENf, &val), ret);
            break;
        case 6:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP4_CTRLr, MANGO_GROUP4_MAC0_1G_ENf, &val), ret);
            break;
        case 7:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP5_CTRLr, MANGO_GROUP5_MAC0_1G_ENf, &val), ret);
            break;
        case 8:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP6_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP6_MAC0_1G_ENf, &val), ret);
            break;
        case 9:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP7_MAC0_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP6_7_CTRLr, MANGO_GROUP7_MAC0_1G_ENf, &val), ret);
            break;
        case 10:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP8_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP8_MAC_1G_ENf, &val), ret);
            break;
        case 11:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP9_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP9_MAC_1G_ENf, &val), ret);
            break;
        case 12:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP10_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP10_MAC_1G_ENf, &val), ret);
            break;
        case 13:
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP11_MAC_2P5G_ENf, &val), ret);
            RT_ERR_CHK(reg_field_write(unit, MANGO_MAC_GROUP8_11_CTRLr, MANGO_GROUP11_MAC_1G_ENf, &val), ret);
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}   /* end of _dal_mango_construct_mac_hisgmii */

/* Function Name:
 *      dal_mango_construct_macConfig_init
 * Description:
 *      MAC Configuration
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
int32 dal_mango_construct_macConfig_init(uint32 unit)
{
    uint32  port, sds, val;
    uint32  aSds;
    uint32  xsg_sdsid_0, xsg_sdsid_1;
    int32   ret;

    CNSTRT_PRINT("%s()\n", __func__);

    HWP_SDS_TRAVS(unit, sds)
    {
        RT_ERR_CHK(drv_rtl9310_sds2AnaSds_get(unit, sds, &aSds), ret);
        RT_ERR_CHK(drv_rtl9310_sds2XsgmSds_get(unit, sds, &xsg_sdsid_0), ret);
        xsg_sdsid_1 = xsg_sdsid_0 + 1;

        if (SERDES_POLARITY_CHANGE == HWP_SDS_RX_POLARITY(unit, sds))
        {
            // 10gr_rx_inv
            RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x6, 0x2, 13, 13, 1), ret);

            // xsg_rx_inv
            RT_ERR_CHK(SDS_FIELD_W(unit, xsg_sdsid_0, 0x0, 0x0, 9, 9, 1), ret);
            RT_ERR_CHK(SDS_FIELD_W(unit, xsg_sdsid_1, 0x0, 0x0, 9, 9, 1), ret);
        }

        if (SERDES_POLARITY_CHANGE == HWP_SDS_TX_POLARITY(unit, sds))
        {
            // 10gr_tx_inv
            RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x6, 0x2, 14, 14, 1), ret);

            // xsg_tx_inv
            RT_ERR_CHK(SDS_FIELD_W(unit, xsg_sdsid_0, 0x0, 0x0, 8, 8, 1), ret);
            RT_ERR_CHK(SDS_FIELD_W(unit, xsg_sdsid_1, 0x0, 0x0, 8, 8, 1), ret);
        }

        port = HWP_SDS_ID2MACID(unit, sds);
        if (HWP_SERDES_PORT(unit, port))
        {
            val = 0x2;
            RT_ERR_CHK(reg_array_field_write(unit, MANGO_SMI_PHY_ABLTY_GET_SELr,
                    port, REG_ARRAY_INDEX_NONE, MANGO_PHY_ABLTY_GET_SELf, &val), ret);
        }

        switch (HWP_SDS_MODE(unit, sds))
        {
            case RTK_MII_USXGMII_10GSXGMII:
                RT_ERR_CHK(_dal_mango_construct_mac_usxgmii_10gs(unit, sds), ret);
                break;
            case RTK_MII_USXGMII_10GQXGMII:
                RT_ERR_CHK(_dal_mango_construct_mac_usxgmii_10gq(unit, sds), ret);
                break;
            case RTK_MII_HISGMII:
                RT_ERR_CHK(_dal_mango_construct_mac_hisgmii(unit, sds), ret);
                break;
            default:
                break;
        }
    }

    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x0080, 31, 0, 0x7CB9428), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x0084, 31, 10, 0x3734CC), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x0084, 2, 0, 0x1), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x0088, 28, 28, 0x1), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x8200, 31, 0, 0x7CB9428), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x8204, 31, 10, 0x3734CC), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x8204, 2, 0, 0x1), ret);
    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x8208, 28, 28, 0x1), ret);

    RT_ERR_CHK(RTL9310_REGADDR_FIELD_W(unit, 0x5730, 14, 14, 0x1), ret);

    return RT_ERR_OK;
}   /* end of dal_mango_construct_macConfig_init */

/* Function Name:
 *      dal_mango_construct_serdes_off
 * Description:
 *      Turn off SerDes mode
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_serdes_off(uint32 unit)
{
    uint32  sds;

    for (sds = 2; sds < DAL_MANGO_SDS_MAX; ++sds)
    {
        phy_rtl9310_sds_mode_set(unit, sds, RTK_MII_DISABLE);
    }
}   /* end of dal_mango_construct_serdes_off */

/* Function Name:
 *      dal_mango_construct_phy_reset
 * Description:
 *      Reset PHY.
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      None
 */
void dal_mango_construct_phy_reset(uint32 unit)
{
    CNSTRT_PRINT("%s()\n", __func__);

    phy_construct_reset(unit);

    return;
}   /* end of dal_mango_construct_phy_reset */

/* Function Name:
 *      dal_mango_construct_phyConfig_init
 * Description:
 *      PHY Configuration
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_mango_construct_phyConfig_init(uint32 unit)
{
    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    phy_construct_config_init(unit);

    return ;
}   /* end of dal_mango_construct_phyConfig_init */

static int32 _dal_mango_construct_sds_patch_set(uint32 unit, uint32 sds, sds_config *cfg, uint32 size)
{
    int32   ret;

    while(size)
    {
        RT_ERR_CHK(hal_serdes_reg_set(unit, sds, cfg->page, cfg->reg, cfg->data), ret);
        ++cfg;
        --size;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_sds_patch_set */

static int32 _dal_mango_construct_10g_sds_ana_patch(uint32 unit, uint32 sds)
{
    uint32  val;
    int32   ret;

    if (IF_CHIP_TYPE_1(unit))
    {
        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_common_type1,
                sizeof(dal_mango_construct_ana_common_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_10p3125g_type1,
                sizeof(dal_mango_construct_ana_10p3125g_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_10p3125g_cmu_type1,
                sizeof(dal_mango_construct_ana_10p3125g_cmu_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_5g_type1,
                sizeof(dal_mango_construct_ana_5g_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_5g_cmu_type1,
                sizeof(dal_mango_construct_ana_5g_cmu_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_3p125g_type1,
                sizeof(dal_mango_construct_ana_3p125g_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_3p125g_cmu_type1,
                sizeof(dal_mango_construct_ana_3p125g_cmu_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_2p5g_type1,
                sizeof(dal_mango_construct_ana_2p5g_type1)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_1p25g_type1,
                sizeof(dal_mango_construct_ana_1p25g_type1)/sizeof(sds_config)), ret);
    }
    else
    {
        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_common,
                sizeof(dal_mango_construct_ana_common)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_10p3125g,
                sizeof(dal_mango_construct_ana_10p3125g)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_10p3125g_cmu,
                sizeof(dal_mango_construct_ana_10p3125g_cmu)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_5g,
                sizeof(dal_mango_construct_ana_5g)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_5g_cmu,
                sizeof(dal_mango_construct_ana_5g_cmu)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_3p125g,
                sizeof(dal_mango_construct_ana_3p125g)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_3p125g_cmu,
                sizeof(dal_mango_construct_ana_3p125g_cmu)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_2p5g,
                sizeof(dal_mango_construct_ana_2p5g)/sizeof(sds_config)), ret);

        RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                dal_mango_construct_ana_1p25g,
                sizeof(dal_mango_construct_ana_1p25g)/sizeof(sds_config)), ret);

        val = 0xa0000;
        RT_ERR_CHK(ioal_mem32_write(unit, 0x8, val), ret);
        RT_ERR_CHK(ioal_mem32_read(unit, 0x8, &val), ret);
        if (1 == (val >> 28))
        {
            RT_ERR_CHK(_dal_mango_construct_sds_patch_set(unit, sds,
                    dal_mango_construct_ana2,
                    sizeof(dal_mango_construct_ana2)/sizeof(sds_config)), ret);
        }
        val = 0;
        RT_ERR_CHK(ioal_mem32_write(unit, 0x8, val), ret);

        SDS_FIELD_W(unit, 10, 0x2f, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 10, 0x2f, 0x5, 15, 0, 0x3FD7);
        SDS_FIELD_W(unit, 14, 0x2f, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 14, 0x2f, 0x5, 15, 0, 0x3FD7);
        SDS_FIELD_W(unit, 6, 0x2f, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 6, 0x2f, 0x5, 15, 0, 0x3FD7);
        SDS_FIELD_W(unit, 18, 0x2f, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 18, 0x2f, 0x5, 15, 0, 0x3FD7);
        SDS_FIELD_W(unit, 2, 0x2f, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x2f, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x2f, 0x5, 15, 0, 0x3FD7);
        SDS_FIELD_W(unit, 10, 0x2d, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 10, 0x2d, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 14, 0x2d, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 14, 0x2d, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 6 , 0x2d, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 6 , 0x2d, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 18, 0x2d, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 18, 0x2d, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 2 , 0x2d, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x2d, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x2d, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 10, 0x2b, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 10, 0x2b, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 14, 0x2b, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 14, 0x2b, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 6 , 0x2b, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 6 , 0x2b, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 18, 0x2b, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 18, 0x2b, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 2 , 0x2b, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x2b, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x2b, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 10, 0x25, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 10, 0x25, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 14, 0x25, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 14, 0x25, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 6 , 0x25, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 6 , 0x25, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 18, 0x25, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 18, 0x25, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 2 , 0x25, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x25, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x25, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 10, 0x27, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 10, 0x27, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 14, 0x27, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 14, 0x27, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 6 , 0x27, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 6 , 0x27, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 18, 0x27, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 18, 0x27, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 2 , 0x27, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x27, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x27, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 10, 0x29, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 10, 0x29, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 14, 0x29, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 14, 0x29, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 6 , 0x29, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 6 , 0x29, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 18, 0x29, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 18, 0x29, 0x5, 15, 0, 0x27D7);
        SDS_FIELD_W(unit, 2 , 0x29, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x29, 0x6, 15, 0, 0x5826);
        SDS_FIELD_W(unit, 22, 0x29, 0x5, 15, 0, 0x27D7);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_10g_sds_ana_patch */

static int32 _dal_mango_construct_init_10gr(uint32 unit, uint32 sds, uint32 aSds)
{
    int32   ret;

    //configure 10GR fiber mode=1
    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x1f, 0xb, 1, 1, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_10gr */

static int32 _dal_mango_construct_xsg_fifo_clk_inv(uint32 unit, uint32 dSds)
{
    uint32  xsg_sdsid_1;
    int32   ret;

    xsg_sdsid_1 = dSds + 1;

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x1, 0x1, 7, 4, 0xf), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x1, 0x1, 3, 0, 0xf), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, xsg_sdsid_1, 0x1, 0x1, 7, 4, 0xf), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, xsg_sdsid_1, 0x1, 0x1, 3, 0, 0xf), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_xsg_fifo_clk_inv */

static int32 _dal_mango_construct_init_xsgmii(uint32 unit, uint32 dSds)
{
    int32   ret;

    if (IF_CHIP_TYPE_1(unit))
    {
        RT_ERR_CHK(_dal_mango_construct_xsg_fifo_clk_inv(unit, dSds), ret);
    }

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x0, 0xE, 12, 12, 1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds + 1, 0x0, 0xE, 12, 12, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_xsgmii */

static int32 _dal_mango_construct_init_usxgmii(uint32 unit, uint32 sds, uint32 aSds)
{
    uint32  i, evenSds;
    int32   ret;
    uint32  op_code = 0x6003;

    if (IF_CHIP_TYPE_1(unit))
    {
        RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x6, 0x2, 12, 12, 1), ret);

        for (i = 0; i < sizeof(dal_mango_construct_ana_10p3125g_type1)/sizeof(sds_config); ++i)
        {
            RT_ERR_CHK(hal_serdes_reg_set(unit, aSds,
                    dal_mango_construct_ana_10p3125g_type1[i].page - 0x4,
                    dal_mango_construct_ana_10p3125g_type1[i].reg,
                    dal_mango_construct_ana_10p3125g_type1[i].data), ret);
        }

        evenSds = aSds - (aSds % 2);

        for (i = 0; i < sizeof(dal_mango_construct_ana_10p3125g_cmu_type1)/sizeof(sds_config); ++i)
        {
            RT_ERR_CHK(hal_serdes_reg_set(unit, evenSds,
                    dal_mango_construct_ana_10p3125g_cmu_type1[i].page - 0x4,
                    dal_mango_construct_ana_10p3125g_cmu_type1[i].reg,
                    dal_mango_construct_ana_10p3125g_cmu_type1[i].data), ret);
        }

        RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x6, 0x2, 12, 12, 0), ret);
    }
    else
    {
        RT_ERR_CHK(phy_rtl9310_init_leq_dfe(unit, sds), ret);
        RT_ERR_CHK(hal_mac_serdes_rst(unit, sds), ret);

        switch (HWP_SDS_ID2PHYMODEL(unit, sds))
        {
            case RTK_PHYTYPE_RTL8224QF:
                op_code = 0x60AA;
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x1d, 0x0A00), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x12, 0x0606), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x13, 0x0000), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x14, 0x0000), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x15, 0x0000), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x16, 0x0000), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x17, 0x0000), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x18, 0x0000), ret);
                break;
            default:
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x1d, 0x0600), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x13, 0x68c1), ret);
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0x14, 0xf021), ret);
        }
        RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x7, 0x10, op_code), ret);
        RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x7, 0x6, 0x1401), ret);
        RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x7, 0x8, 0x1401), ret);
        RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x7, 0xA, 0x1401), ret);
        RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x7, 0xC, 0x1401), ret);
        RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x6, 0xe, 0x055A), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x6, 0x3, 15, 15, 1), ret);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_usxgmii */

static int32 _dal_mango_construct_init_fiber1g(uint32 unit, uint32 dSds, uint32 aSds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x3, 0x13, 15, 14, 0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 12, 12, 1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 6, 6, 1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 13, 13, 0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x0, 0x4, 2, 2, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_fiber1g */

static int32 _dal_mango_construct_init_fiber10g_1g_auto(uint32 unit, uint32 sds, uint32 dSds, uint32 aSds)
{
    int32   ret;

    RT_ERR_CHK(_dal_mango_construct_init_10gr(unit, sds, aSds), ret);
    RT_ERR_CHK(_dal_mango_construct_init_fiber1g(unit, dSds, aSds), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x1f, 13, 15, 0, 0x109e), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x1f, 0x6, 14, 10, 0x8), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x1f, 0x7, 10, 4, 0x7f), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_fiber10g_1g_auto */

static int32 _dal_mango_construct_init_qsgmii(uint32 unit, uint32 sds)
{
    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_qsgmii */

static int32 _dal_mango_construct_init_hisgmii(uint32 unit, uint32 dSds, uint32 aSds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x20, 0x12, 15, 0, 0x3105), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x24, 0x7, 15, 15, 0x1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x1, 0x14, 8, 8, 1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x0, 0xE, 12, 12, 1), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x20, 0x12, 15, 0, 0x310F), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_hisgmii */

static int32 _dal_mango_construct_init_xsmii(uint32 unit, uint32 sds)
{
    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_xsmii */

static int32 _dal_mango_construct_init_fiber100(uint32 unit, uint32 dSds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x3, 0x13, 15, 14, 0), ret);

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 12, 12, 0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 6, 6, 0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 13, 13, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_fiber100 */

static int32 _dal_mango_construct_postInit_fiber100(uint32 unit, uint32 dSds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 12, 12, 0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 6, 6, 0), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x2, 0x0, 13, 13, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_postInit_fiber100 */

static int32 _dal_mango_construct_init_fiber1g_100_auto(uint32 unit, uint32 dSds, uint32 aSds)
{
    int32   ret;

    RT_ERR_CHK(_dal_mango_construct_init_fiber1g(unit, dSds, aSds), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_fiber1g_100_auto */

static int32 _dal_mango_construct_init_fiber2p5g(uint32 unit, uint32 dSds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x1, 0x14, 8, 8, 1), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_fiber1g_100_auto */

static int32 _dal_mango_construct_init_sgmii(uint32 unit, uint32 sds)
{
    int32   ret;

    RT_ERR_CHK(SDS_FIELD_W(unit, sds, 0x24, 0x9, 15, 15, 0), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_init_sgmii */

static int32
_dal_mango_construct_cmuType_set(uint32 unit, uint32 aSds, rtk_serdesMode_t mode)
{
    dal_mango_sds_cmutype_t cmuType = DAL_MANGO_SDS_CMU_END;
    uint32                  cmuPage = 0;
    uint32                  frc_cmu_spd;
    uint32                  evenSds;
    uint32                  lane, frc_lc_mode_bitnum, frc_lc_mode_val_bitnum;
    int32                   ret;

    switch (mode)
    {
        case RTK_MII_DISABLE:
        case RTK_MII_10GR:
        case RTK_MII_XSGMII:
        //case RTK_MII_USXGMII:
        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_10GQXGMII:
        case RTK_MII_USXGMII_5GSXGMII:
        case RTK_MII_USXGMII_5GDXGMII:
        case RTK_MII_USXGMII_2_5GSXGMII:
        case RTK_MII_USXGMII_1G:
        case RTK_MII_USXGMII_100M:
        case RTK_MII_USXGMII_10M:
            return RT_ERR_OK;
        case RTK_MII_10GR1000BX_AUTO:
            if (IF_CHIP_TYPE_1(unit))
            {
                RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x24, 0xd, 14, 14, 0), ret);
            }
            return RT_ERR_OK;
        case RTK_MII_QSGMII:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x2a;
            frc_cmu_spd = 0;
            break;
        case RTK_MII_HISGMII:
            cmuType = DAL_MANGO_SDS_CMU_END;
            cmuPage = 0x28;
            frc_cmu_spd = 1;
            break;
        case RTK_MII_XSMII:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x26;
            frc_cmu_spd = 0;
            break;
        case RTK_MII_1000BX_FIBER:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x24;
            frc_cmu_spd = 0;
            break;
        case RTK_MII_100BX_FIBER:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x24;
            frc_cmu_spd = 0;
            break;
        case RTK_MII_1000BX100BX_AUTO:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x24;
            frc_cmu_spd = 0;
            break;
        case RTK_MII_SGMII:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x24;
            frc_cmu_spd = 0;
            break;
        case RTK_MII_2500Base_X:
            cmuType = DAL_MANGO_SDS_CMU_RING;
            cmuPage = 0x28;
            frc_cmu_spd = 1;
            break;
        default:
            osal_printf("SerDes %d mode is invalid\n", aSds);
            return RT_ERR_FAILED;
    }

    lane = aSds % 2;

    if (0 == lane)
    {
        frc_lc_mode_bitnum = 4;
        frc_lc_mode_val_bitnum = 5;
    }
    else
    {
        frc_lc_mode_bitnum = 6;
        frc_lc_mode_val_bitnum = 7;
    }

    evenSds = aSds - lane;
    if (DAL_MANGO_SDS_CMU_RING == cmuType)
    {
        RT_ERR_CHK(SDS_FIELD_W(unit, aSds, cmuPage, 0x7, 15, 15, 0), ret);
        if (IF_CHIP_TYPE_1(unit))
        {
            RT_ERR_CHK(SDS_FIELD_W(unit, aSds, cmuPage, 0xd, 14, 14, 0), ret);
        }

        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, 3, 2, 0x3), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, frc_lc_mode_bitnum, frc_lc_mode_bitnum, 1), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, frc_lc_mode_val_bitnum, frc_lc_mode_val_bitnum, 0), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, 12, 12, 1), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, 15, 13, frc_cmu_spd), ret);
    }
    else if (DAL_MANGO_SDS_CMU_LC == cmuType)
    {
        RT_ERR_CHK(SDS_FIELD_W(unit, aSds, cmuPage, 0x7, 15, 15, 1), ret);
        if (IF_CHIP_TYPE_1(unit))
        {
            RT_ERR_CHK(SDS_FIELD_W(unit, aSds, cmuPage, 0xd, 14, 14, 1), ret);
        }

        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, 1, 0, 0x3), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, frc_lc_mode_bitnum, frc_lc_mode_bitnum, 1), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, frc_lc_mode_val_bitnum, frc_lc_mode_val_bitnum, 1), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, 8, 8, 1), ret);
        RT_ERR_CHK(SDS_FIELD_W(unit, evenSds, 0x20, 0x12, 11, 9, frc_cmu_spd), ret);
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_cmuType_set */

int32
_dal_mango_construct_sdsMode_set(uint32 unit, uint32 sds, rtk_serdesMode_t mode)
{
    uint32  board_sds_tx_type1[] = {0x1C3, 0x1C3, 0x1C3, 0x1A3, 0x1A3,
            0x1A3, 0x143, 0x143, 0x143, 0x143, 0x163, 0x163};

    uint32  board_sds_tx[] = {0x2A0, 0x2A0, 0x200, 0x200, 0x200,
            0x200, 0x1A3, 0x1A3, 0x1A3, 0x1A3, 0x1E3, 0x1E3};

    uint32  board_sds_tx2[] = {0xDC0, 0x1C0, 0x200, 0x180, 0x160,
            0x123, 0x123, 0x163, 0x1A3, 0x1A0, 0x1C3, 0x9C3};

    uint32  aSds, dSds, val, ori;
    int32   ret;

    if (DAL_MANGO_SDS_MAX <= sds)
        return RT_ERR_FAILED;

    RT_ERR_CHK(drv_rtl9310_sds2XsgmSds_get(unit, sds, &dSds), ret);
    RT_ERR_CHK(drv_rtl9310_sds2AnaSds_get(unit, sds, &aSds), ret);

    ori = 0;
    RT_ERR_CHK(reg_read(unit, MANGO_PS_SERDES_OFF_MODE_CTRLr, &ori), ret);
    val = ori | (1 << sds);
    RT_ERR_CHK(reg_write(unit, MANGO_PS_SERDES_OFF_MODE_CTRLr, &val), ret);

    switch (mode)
    {
        case RTK_MII_DISABLE:
            break;
        case RTK_MII_XSGMII:
            RT_ERR_CHK(_dal_mango_construct_init_xsgmii(unit, dSds), ret);
            break;
        //case RTK_MII_USXGMII:
        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_10GQXGMII:
        //case RTK_MII_USXGMII_5GSXGMII:
        //case RTK_MII_USXGMII_5GDXGMII:
        //case RTK_MII_USXGMII_2_5GSXGMII:
            RT_ERR_CHK(_dal_mango_construct_init_usxgmii(unit, sds, aSds), ret);
            break;
        case RTK_MII_10GR:
        case RTK_MII_10GR1000BX_AUTO:
            RT_ERR_CHK(_dal_mango_construct_init_fiber10g_1g_auto(unit, sds, dSds, aSds), ret);
            break;
        case RTK_MII_QSGMII:
            RT_ERR_CHK(_dal_mango_construct_init_qsgmii(unit, aSds), ret);
            break;
        case RTK_MII_HISGMII:
            RT_ERR_CHK(_dal_mango_construct_init_hisgmii(unit, dSds, aSds), ret);
            break;
        case RTK_MII_XSMII:
            RT_ERR_CHK(_dal_mango_construct_init_xsmii(unit, aSds), ret);
            break;
        case RTK_MII_1000BX_FIBER:
            RT_ERR_CHK(_dal_mango_construct_init_fiber1g(unit, dSds, aSds), ret);
            break;
        case RTK_MII_100BX_FIBER:
            RT_ERR_CHK(_dal_mango_construct_init_fiber100(unit, dSds), ret);
            break;
        case RTK_MII_1000BX100BX_AUTO:
            RT_ERR_CHK(_dal_mango_construct_init_fiber1g_100_auto(unit, dSds, aSds), ret);
            break;
        case RTK_MII_SGMII:
            RT_ERR_CHK(_dal_mango_construct_init_sgmii(unit, aSds), ret);
            break;
        case RTK_MII_2500Base_X:
            RT_ERR_CHK(_dal_mango_construct_init_fiber2p5g(unit, dSds), ret);
            break;
        default:
            osal_printf("SerDes %d mode is invalid\n", sds);
            return RT_ERR_FAILED;
    }

    RT_ERR_CHK(_dal_mango_construct_cmuType_set(unit, aSds, mode), ret);

    if (sds >= 2 && sds <= 13)
    {
        if (IF_CHIP_TYPE_1(unit))
            RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x2E, 0x1, board_sds_tx_type1[sds - 2]), ret);
        else
        {
            val = 0xa0000;
            RT_ERR_CHK(ioal_mem32_write(unit, 0x8, val), ret);
            RT_ERR_CHK(ioal_mem32_read(unit, 0x8, &val), ret);
            if (1 == (val >> 28) && RTL9313_CHIP_ID == HWP_CHIP_ID(unit))
            {
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x2E, 0x1, board_sds_tx2[sds - 2]), ret);
            }
            else
            {
                RT_ERR_CHK(hal_serdes_reg_set(unit, aSds, 0x2E, 0x1, board_sds_tx[sds - 2]), ret);
            }
            val = 0;
            RT_ERR_CHK(ioal_mem32_write(unit, 0x8, val), ret);
        }
    }

    val = ori & ~(1 << sds);
    RT_ERR_CHK(reg_write(unit, MANGO_PS_SERDES_OFF_MODE_CTRLr, &val), ret);

    switch (mode)
    {
        case RTK_MII_XSGMII:
        case RTK_MII_QSGMII:
        case RTK_MII_XSMII:
        case RTK_MII_SGMII:
        case RTK_MII_USXGMII_10GSXGMII:
        case RTK_MII_USXGMII_10GQXGMII:
            RT_ERR_CHK(phy_rtl9310_sds_mode_set(unit, sds, mode), ret);
            break;
        case RTK_MII_HISGMII:
            RT_ERR_CHK(SDS_FIELD_W(unit, aSds, 0x2A, 0x7, 15, 15, 0), ret);
            RT_ERR_CHK(phy_rtl9310_sds_mode_set(unit, sds, mode), ret);
            break;
        case RTK_MII_100BX_FIBER:
            RT_ERR_CHK(_dal_mango_construct_postInit_fiber100(unit, dSds), ret);
            break;
        default:
            if (RTK_PHYTYPE_SERDES != HWP_SDS_ID2PHYMODEL(unit, sds))
                RT_ERR_CHK(phy_rtl9310_sds_mode_set(unit, sds, mode), ret);
            break;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_sdsMode_set */

/* Function Name:
 *      _dal_mango_construct_pcb_init
 * Description:
 *      PCB init
 * Input:
 *      unit    - unit id
 *      sds     - SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_dal_mango_construct_pcb_init(uint32 unit, uint32 sds)
{
    rtk_sds_rxCaliConf_t    conf;
    int32                   ret;

    conf.dfeTap1_4Enable = ENABLED;
    conf.dfeAuto = ENABLED;
    conf.leqAuto = ENABLED;
    conf.ofst = 0;
    RT_ERR_CHK(phy_rtl9310_sdsRxCaliConf_set(unit, sds, conf), ret);

    return RT_ERR_OK;
} /* end of _dal_mango_construct_pcb_init */

/* Function Name:
 *      _dal_mango_construct_pcb_cali
 * Description:
 *      PCB calibration
 * Input:
 *      unit    - unit id
 *      sds     - SerDes id
 *      asds    - Analog SerDes id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_dal_mango_construct_pcb_cali(uint32 unit, uint32 sds, uint32 asds)
{
    uint32                  dSds;
    uint32                  i, max_cali_num = 10;
    uint32                  dis_xsgmii_nway;
    int32                   ret;
    uint8                   linkSts;

    RT_ERR_CHK(drv_rtl9310_sds2XsgmSds_get(unit, sds, &dSds), ret);

    switch (HWP_SDS_ID2PHYMODEL(unit, sds))
    {
        case RTK_PHYTYPE_RTL8218D_NMP:
        case RTK_PHYTYPE_RTL8218D:
        case RTK_PHYTYPE_RTL8218E:
            dis_xsgmii_nway = 1;
            break;
        default:
            dis_xsgmii_nway = 0;
    }

    RT_ERR_CHK(SDS_FIELD_W(unit, dSds, 0x0, 0x2, 9, 8, dis_xsgmii_nway), ret);
    RT_ERR_CHK(SDS_FIELD_W(unit, dSds + 1, 0x0, 0x2, 9, 8, dis_xsgmii_nway), ret);

    RT_ERR_CHK(phy_rtl9310_sdsRxCaliEnable_set(unit, sds, ENABLED), ret);

    for (i = 0; i < max_cali_num; ++i)
    {
        RT_ERR_CHK(phy_rtl9310_rxCali(unit, sds), ret);
        RT_ERR_CHK(phy_rtl9310_linkSts_chk(unit, sds, &linkSts), ret);

        if (1 == linkSts)
            break;

        osal_time_mdelay(100);
    }

    //if (0 == linkSts)
    //    osal_printf("SDS %u link fail\n", sds);

    RT_ERR_CHK(phy_rtl9310_pcb_adapt(unit, sds), ret);

    return RT_ERR_OK;
}   /* end of _dal_mango_construct_pcb_cali */

/* Function Name:
 *      dal_mango_construct_serdesConfig_init
 * Description:
 *      Serdes Configuration
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_mango_construct_serdesConfig_init(uint32 unit)
{
    uint32  sds, aSds;
    uint32  val;

    CNSTRT_PRINT("%s()\n", __func__);

    dal_mango_sds_init(unit);

    val = 0;
    for (sds = 2; sds < DAL_MANGO_SDS_MAX; ++sds)
    {
        drv_rtl9310_sds2AnaSds_get(unit, sds, &aSds);
        val |= (1 << aSds);
    }

    reg_field_write(unit, MANGO_SERDES_BC_CTRLr, MANGO_SERDES_BC_ENf, &val);
    sds = 0x1F;
    reg_field_write(unit, MANGO_SERDES_BC_CTRLr, MANGO_SERDES_BC_IDf, &sds);

    val = 0xFFFFFFFF;
    reg_write(unit, MANGO_FRC_RXDV_Hr, &val);
    reg_write(unit, MANGO_FRC_RXDV_Lr, &val);

    _dal_mango_construct_10g_sds_ana_patch(unit, sds);

    HWP_SDS_TRAVS(unit, sds)
    {
        _dal_mango_construct_sdsMode_set(unit, sds, HWP_SDS_MODE(unit, sds));
        osal_time_mdelay(10);
        SDS_FIELD_W(unit, aSds, 0x20, 0x0, 11, 10, 0x1);
        SDS_FIELD_W(unit, aSds, 0x20, 0x0, 11, 10, 0x3);
    }

    osal_time_mdelay(1000);

    HWP_SDS_TRAVS(unit, sds)
    {
        if (RTK_PHYTYPE_SERDES != HWP_SDS_ID2PHYMODEL(unit, sds))
            _dal_mango_construct_pcb_init(unit, sds);

        switch(HWP_SDS_MODE(unit, sds))
        {
            case RTK_MII_XSGMII:
            case RTK_MII_USXGMII_10GSXGMII:
            case RTK_MII_USXGMII_10GQXGMII:
                _dal_mango_construct_pcb_cali(unit, sds, aSds);
                break;
            default:
                break;
        }

        phy_rtl9310_sdsCustConfig_init(unit, sds, PORT_10GMEDIA_NONE);
    }

    return ;
}   /* end of dal_mango_construct_serdesConfig_init */

/* Function Name:
 *      dal_mango_construct_pollingConfig_init
 * Description:
 *      Polling Configuration
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_pollingConfig_init(uint32 unit)
{
    uint32  val, port;

    val = 0x1;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (HWP_PHY_EXIST(unit, port) || HWP_SERDES_PORT(unit, port))
        {
            /* set polling portmask, must after SerDes patch */
            reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val);
        }
    }

    if (IF_CHIP_TYPE_1(unit))
    {
        if (RTL9313_CHIP_ID == HWP_CHIP_ID(unit))
        {
            val = 0x1;
            for (port = 0; port < 48; ++port)
            {
                reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_CTRLr,
                        port, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val);
            }

            val = 0x0;
            for (port = 48; port < 56; ++port)
            {
                reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_CTRLr,
                        port, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val);
            }
        }
    }

    return ;
}   /* end of dal_mango_construct_pollingConfig_init */


/* Function Name:
 *      dal_mango_construct_pollingConfig_disable
 * Description:
 *      Disable polling configuration
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_pollingConfig_disable(uint32 unit)
{
    uint32  val, port;

    val = 0x0;
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        /* set polling portmask, must after SerDes patch */
        reg_array_field_write(unit, MANGO_SMI_PORT_POLLING_CTRLr,
                port, REG_ARRAY_INDEX_NONE, MANGO_SMI_POLLING_PMSKf, &val);
    }
}


/* Function Name:
 *      dal_mango_construct_macPollingPhy_init
 * Description:
 *      MAC Polling PHY config initialization
 * Input:
 *      unit                - unit id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
dal_mango_construct_macPollingPhy_init(uint32 unit)
{
    uint32      port, smi_id, value;
    uint32      is_10g = 0;
    uint32      addr_reg0_bit = 0, addr_reg0_dev = 0, addr_reg0_addr = 0;
    uint32      addr_reg9_bit = 0, addr_reg9_dev = 0, addr_reg9_addr = 0;
    uint32      addr_reg10_bit = 0, addr_reg10_dev = 0, addr_reg10_addr = 0;
    uint32      smiPrvte0_val[DAL_MANGO_SMI_MAX] = { 0 };
    uint32      smiPrvte1_val[DAL_MANGO_SMI_MAX] = { 0 };
    uint32      smiFmtSel_val[DAL_MANGO_SMI_MAX] = { 0 };
    uint32      smiPrvte0_field[] = {
                MANGO_SMI_SET0_PRVTE0_POLLINGf, MANGO_SMI_SET1_PRVTE0_POLLINGf, MANGO_SMI_SET2_PRVTE0_POLLINGf, MANGO_SMI_SET3_PRVTE0_POLLINGf };
    uint32      smiPrvte1_field[] = {
                MANGO_SMI_SET0_PRVTE1_POLLINGf, MANGO_SMI_SET1_PRVTE1_POLLINGf, MANGO_SMI_SET2_PRVTE1_POLLINGf, MANGO_SMI_SET3_PRVTE1_POLLINGf };
    uint32      smiPark_field[] = {
                MANGO_SMI_SET0_POLLING_PARK_SELf, MANGO_SMI_SET1_POLLING_PARK_SELf, MANGO_SMI_SET2_POLLING_PARK_SELf, MANGO_SMI_SET3_POLLING_PARK_SELf };
    uint32      smiFmtSel_field[] = {
                MANGO_SMI_SET0_FMT_SELf, MANGO_SMI_SET1_FMT_SELf, MANGO_SMI_SET2_FMT_SELf, MANGO_SMI_SET3_FMT_SELf };

    rt_phyInfo_t    phyInfo;


    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((smi_id = (uint32)HWP_PORT_SMI(unit, port)) != HWP_NONE)
        {
            if (smi_id >= DAL_MANGO_SMI_MAX)
            {
                continue;
            }

            phy_info_get(unit, port, &phyInfo);

            if ((phyInfo.flags & RTK_PHYINFO_FLAG_NO_RES_REG) == 0)
            {
                smiPrvte0_val[smi_id] = 1;
            }

            if (phyInfo.flags & RTK_PHYINFO_FLAG_FORCE_RES)
            {
                smiFmtSel_val[smi_id] = 1;
                smiPrvte1_val[smi_id] = 1;
            }

            if ((phyInfo.flags & RTK_PHYINFO_FLAG_1G_MMD_CFG) != 0)
            {
                is_10g = 1;

                addr_reg0_bit       = phyInfo.xGePhyLocalDuplexAbilityBit;
                addr_reg0_dev       = phyInfo.xGePhyLocalDuplexAbilityDev;
                addr_reg0_addr      = phyInfo.xGePhyLocalDuplexAbilityAddr;

                addr_reg9_bit       = phyInfo.xGePhyLocal1000MSpeedAbilityBit;
                addr_reg9_dev       = phyInfo.xGePhyLocal1000MSpeedAbilityDev;
                addr_reg9_addr      = phyInfo.xGePhyLocal1000MSpeedAbilityAddr;

                addr_reg10_bit      = phyInfo.xGePhyLinkPartner1000MSpeedAbilityBit;
                addr_reg10_dev      = phyInfo.xGePhyLinkPartner1000MSpeedAbilityDev;
                addr_reg10_addr     = phyInfo.xGePhyLinkPartner1000MSpeedAbilityAddr;
            }
        }

        if (HWP_PHY_MODEL_BY_PORT(unit, port) == RTK_PHYTYPE_RTL8224QF)
        {
            value = 0x2;
            reg_array_field_write(unit, MANGO_SMI_PHY_ABLTY_GET_SELr,
                    port, REG_ARRAY_INDEX_NONE, MANGO_PHY_ABLTY_GET_SELf, &value);
        }
    }

    for (smi_id = 0; smi_id < DAL_MANGO_SMI_MAX; smi_id++)
    {
        //PRVT0_POLLING
        if (smiPrvte0_val[smi_id] == 1)
            value = 1;
        else
            value = 0;

        reg_field_write(unit, MANGO_SMI_GLB_CTRL0r, smiPrvte0_field[smi_id], &value);
        reg_field_write(unit, MANGO_SMI_GLB_CTRL0r, smiPark_field[smi_id], &value);

        //PRVT1_POLLING
        if (smiPrvte1_val[smi_id] == 1)
            value = 1;
        else
            value = 0;

        reg_field_write(unit, MANGO_SMI_GLB_CTRL0r, smiPrvte1_field[smi_id], &value);

        //FMT_SEL
        reg_field_read(unit, MANGO_SMI_GLB_CTRL1r, smiFmtSel_field[smi_id], &value);
        if (smiFmtSel_val[smi_id] == 1)
        {
            // force proprietary
            value |= 0x1;
        }
        else
        {
            // standard reg
            value &= 0x2;
        }
        reg_field_write(unit, MANGO_SMI_GLB_CTRL1r, smiFmtSel_field[smi_id], &value);
    }

    /* register address at 10G phy */
    if (is_10g == 1)
    {
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL2r, MANGO_REG0_BIT_POLLING_10GPHYf, &addr_reg0_bit);
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL2r, MANGO_REG0_DEV_POLLING_10GPHYf, &addr_reg0_dev);
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL2r, MANGO_REG0_REG_POLLING_10GPHYf, &addr_reg0_addr);

        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL3r, MANGO_REG9_BIT_POLLING_10GPHYf, &addr_reg9_bit);
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL3r, MANGO_REG9_DEV_POLLING_10GPHYf, &addr_reg9_dev);
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL3r, MANGO_REG9_REG_POLLING_10GPHYf, &addr_reg9_addr);

        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL4r, MANGO_REG10_BIT_POLLING_10GPHYf, &addr_reg10_bit);
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL4r, MANGO_REG10_DEV_POLLING_10GPHYf, &addr_reg10_dev);
        reg_field_write(unit, MANGO_SMI_10GPHY_POLLING_SEL4r, MANGO_REG10_REG_POLLING_10GPHYf, &addr_reg10_addr);
    }
    return ;
}   /* end of dal_mango_construct_macPollingPhy_init */


