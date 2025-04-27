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

#include <dal/dal_construct.h>
#include <dal/cypress/dal_cypress_construct.h>
#include <dal/cypress/dal_cypress_port.h>
#include <hal/phy/phy_construct.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <ioal/phy_reset.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/phy/phy_rtl8390.h>

#define Serdes_set              dal_cypress_construct_serdesPatch_set

void dal_cypress_construct_serdesPatch_set(uint32 unit, uint32 reg,
    uint32 endBit, uint32 startBit, uint32 val)
{
    uint32  configVal, len, mask;
    uint32  i;
    int32   ret;

    len = endBit - startBit + 1;

    if (32 == len)
        configVal = val;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        ret = ioal_mem32_read(unit, reg, &configVal);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }

        configVal &= ~(mask);
        configVal |= (val << startBit);
    }

    ret = ioal_mem32_write(unit, reg, configVal);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }

    return;
}

void MacRegSetCheck(uint32 unit, uint32 reg,uint32 val)
{
    uint32 data;
    int32   ret;

    ret = ioal_mem32_write(unit, reg, val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }

    osal_time_mdelay(1);

    if ((reg != 0x0ff4) &&
        (reg != 0x0014) &&
        (reg != 0x1180))
    {

        ret = ioal_mem32_read(unit, reg, &data);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }

        if ( data != val){
            osal_printf("WARN: Write(0x%08X) to Reg 0x%08X,but read as 0x%08X\n",val, SWCORE_VIRT_BASE + reg, data);
        }
    }

    return;
}

void Serdes_patch_set(uint32 unit, confcode_serdes_patch_t in)
{
    uint32  reg, val, len, mask;
    uint32  i, startBit, endBit;
    int32   ret;

    reg = in.reg + in.offset;
    startBit = in.startBit;
    endBit = in.endBit;
    len = endBit - startBit + 1;

    if (32 == len)
        val = in.val;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        ret = ioal_mem32_read(unit, reg, &val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }

        val &= ~(mask);
        val |= (in.val << startBit);
    }
    ret = ioal_mem32_write(unit, reg, val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }
}

void Serdes_patch_set_chk(uint32 unit, confcode_serdes_patch_t in)
{
    uint32  reg, val, len, mask;
    uint32  i, startBit, endBit;
    int32   ret;

    reg = in.reg + in.offset;
    startBit = in.startBit;
    endBit = in.endBit;
    len = endBit - startBit + 1;

    if (32 == len)
        val = in.val;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        ret = ioal_mem32_read(unit, reg, &val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
        val &= ~(mask);
        val |= (in.val << startBit);
    }

    MacRegSetCheck(unit, reg, val);
}

void rtl839x_serdes_reset(uint32 unit, const uint32 sds_num)
{
    uint32  sdsReg[] = {0xA328, 0xA728, 0xAB28, 0xAF28, 0xB320, 0xB728, 0xBB20};
    uint32 addr_ofst = 0x400;
    uint32  ofst, sdsAddr;

    ofst = addr_ofst*(sds_num/2);
    sdsAddr = sdsReg[sds_num/2] + (0x80 * (sds_num % 2));
    if (sds_num < 8 || sds_num == 10 || sds_num == 11) {
        Serdes_set(unit, 0xa3c0 + ofst,  31 , 16 , 0x0050);
        Serdes_set(unit, 0xa3c0 + ofst,  31 , 16 , 0x00f0);
        Serdes_set(unit, 0xa3c0 + ofst,  31 , 16 , 0x0);

        Serdes_set(unit, sdsAddr,  0 , 0 , 0x0);
        Serdes_set(unit, sdsAddr,  9 , 9 , 0x1);
        osal_time_mdelay(100);
        Serdes_set(unit, sdsAddr,  9 , 9 , 0x0);
    } else if (sds_num == 8 || sds_num == 9) {
        Serdes_set(unit, 0xb3f8,  19 , 16 , 0x5);
        osal_time_mdelay(500);
        Serdes_set(unit, 0xb3f8,  19 , 16 , 0xf);
        Serdes_set(unit, 0xb3f8,  19 , 16 , 0x0);

        Serdes_set(unit, 0xb320,  3 , 3 , 0x0);
        Serdes_set(unit, 0xb340,  15 , 15 , 0x1);
        osal_time_mdelay(100);
        Serdes_set(unit, 0xb340,  15 , 15 , 0x0);
    } else if (sds_num == 12 || sds_num == 13) {
        Serdes_set(unit, 0xbbf8,  19 , 16 , 0x5);
        osal_time_mdelay(500);
        Serdes_set(unit, 0xbbf8,  19 , 16 , 0xf);
        Serdes_set(unit, 0xbbf8,  19 , 16 , 0x0);

        Serdes_set(unit, 0xbb20,  3 , 3 , 0x0);
        Serdes_set(unit, 0xbb40,  15 , 15 , 0x1);
        osal_time_mdelay(100);
        Serdes_set(unit, 0xbb40,  15 , 15 , 0x0);
    } else {
        osal_printf( "sds number doesn't exist");
        return;
    }
    Serdes_set(unit, 0xa004 + ofst,  31 , 16 , 0x7146);
    osal_time_mdelay(100);
    Serdes_set(unit, 0xa004 + ofst,  31 , 16 , 0x7106);
    Serdes_set(unit, 0xa004 + ofst + 0x100,  31 , 16 , 0x7146);
    osal_time_mdelay(100);
    Serdes_set(unit, 0xa004 + ofst + 0x100,  31 , 16 , 0x7106);
}   /* end of rtl839x_serdes_reset */

void rtl839x_serdes_patch_init(uint32 unit)
{
    uint32  ofst_list[] = {0,0x800};
    uint32  ofst_list1[] = {0x80,0x880};
    uint32  ofst_list2[] = {0,0x80,0x400,0x480,0x800,0x880,0xc00,0xc80,0x1400,0x1480};
    uint32  i, ofst;

    Serdes_set(unit, 0xb300,  15 , 0  , 0x5800);
    Serdes_set(unit, 0xb300,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xb304,  15 , 0  , 0x5400);
    Serdes_set(unit, 0xb304,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb308,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb308,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xb30c,  15 , 0  , 0x4000);
    Serdes_set(unit, 0xb30c,  31 , 16 , 0xffff);
    Serdes_set(unit, 0xb310,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xb310,  31 , 16 , 0x806f);
    Serdes_set(unit, 0xb314,  15 , 0  , 0x0004);
    Serdes_set(unit, 0xb314,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb318,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb318,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb31c,  15 , 0  , 0x0a00);
    Serdes_set(unit, 0xb31c,  31 , 16 , 0x2000);
    Serdes_set(unit, 0xb320,  15 , 0  , 0xf00e);
    Serdes_set(unit, 0xb320,  31 , 16 , 0xf04a);
    Serdes_set(unit, 0xb324,  15 , 0  , 0x97b3);
    Serdes_set(unit, 0xb324,  31 , 16 , 0x5318);
    Serdes_set(unit, 0xb328,  15 , 0  , 0x0f03);
    Serdes_set(unit, 0xb328,  31 , 16 , 0x0);
    Serdes_set(unit, 0xb32c,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb32c,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb330,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb330,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb334,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xb334,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb338,  15 , 0  , 0x1203);
    Serdes_set(unit, 0xb338,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb33c,  15 , 0  , 0xa052);
    Serdes_set(unit, 0xb33c,  31 , 16 , 0x9a00);
    Serdes_set(unit, 0xb340,  15 , 0  , 0x00f5);
    Serdes_set(unit, 0xb340,  31 , 16 , 0xf000);
    Serdes_set(unit, 0xb344,  15 , 0  , 0x41ff);
    Serdes_set(unit, 0xb344,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb348,  15 , 0  , 0x39ff);
    Serdes_set(unit, 0xb348,  31 , 16 , 0x3340);
    Serdes_set(unit, 0xb34c,  15 , 0  , 0x40aa);
    Serdes_set(unit, 0xb34c,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb350,  15 , 0  , 0x801f);
    Serdes_set(unit, 0xb350,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb354,  15 , 0  , 0x619c);
    Serdes_set(unit, 0xb354,  31 , 16 , 0xffed);
    Serdes_set(unit, 0xb358,  15 , 0  , 0x29ff);
    Serdes_set(unit, 0xb358,  31 , 16 , 0x29ff);
    Serdes_set(unit, 0xb35c,  15 , 0  , 0x4e10);
    Serdes_set(unit, 0xb35c,  31 , 16 , 0x4e10);
    Serdes_set(unit, 0xb360,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb360,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb380,  15 , 0  , 0x5800);
    Serdes_set(unit, 0xb380,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xb384,  15 , 0  , 0x5000);
    Serdes_set(unit, 0xb384,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb388,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb388,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xb38c,  15 , 0  , 0x4000);
    Serdes_set(unit, 0xb38c,  31 , 16 , 0xffff);
    Serdes_set(unit, 0xb390,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xb390,  31 , 16 , 0x806f);
    Serdes_set(unit, 0xb394,  15 , 0  , 0x0004);
    Serdes_set(unit, 0xb394,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb398,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb398,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb39c,  15 , 0  , 0x0a00);
    Serdes_set(unit, 0xb39c,  31 , 16 , 0x2000);
    Serdes_set(unit, 0xb3a0,  15 , 0  , 0xf00e);
    Serdes_set(unit, 0xb3a0,  31 , 16 , 0xfdab);
    Serdes_set(unit, 0xb3a4,  15 , 0  , 0x96ea);
    Serdes_set(unit, 0xb3a4,  31 , 16 , 0x5318);
    Serdes_set(unit, 0xb3a8,  15 , 0  , 0x0f03);
    Serdes_set(unit, 0xb3a8,  31 , 16 , 0);
    Serdes_set(unit, 0xb3ac,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb3ac,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3b0,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb3b0,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3b4,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xb3b4,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3b8,  15 , 0  , 0x1203);
    Serdes_set(unit, 0xb3b8,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3bc,  15 , 0  , 0xa052);
    Serdes_set(unit, 0xb3bc,  31 , 16 , 0x9a00);
    Serdes_set(unit, 0xb3c0,  15 , 0  , 0x00f5);
    Serdes_set(unit, 0xb3c0,  31 , 16 , 0xf000);
    Serdes_set(unit, 0xb3c4,  15 , 0  , 0x4079);
    Serdes_set(unit, 0xb3c4,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3c8,  15 , 0  , 0x93fa);
    Serdes_set(unit, 0xb3c8,  31 , 16 , 0x3340);
    Serdes_set(unit, 0xb3cc,  15 , 0  , 0x4280);
    Serdes_set(unit, 0xb3cc,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3d0,  15 , 0  , 0x801f);
    Serdes_set(unit, 0xb3d0,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb3d4,  15 , 0  , 0x619c);
    Serdes_set(unit, 0xb3d4,  31 , 16 , 0xffed);
    Serdes_set(unit, 0xb3d8,  15 , 0  , 0x29ff);
    Serdes_set(unit, 0xb3d8,  31 , 16 , 0x29ff);
    Serdes_set(unit, 0xb3dc,  15 , 0  , 0x4c50);
    Serdes_set(unit, 0xb3dc,  31 , 16 , 0x4c50);
    Serdes_set(unit, 0xb3e0,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xb3e0,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb00,  15 , 0  , 0x5800);
    Serdes_set(unit, 0xbb00,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xbb04,  15 , 0  , 0x5400);
    Serdes_set(unit, 0xbb04,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb08,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb08,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xbb0c,  15 , 0  , 0x4000);
    Serdes_set(unit, 0xbb0c,  31 , 16 , 0xffff);
    Serdes_set(unit, 0xbb10,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xbb10,  31 , 16 , 0x806f);
    Serdes_set(unit, 0xbb14,  15 , 0  , 0x0004);
    Serdes_set(unit, 0xbb14,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb18,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb18,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb1c,  15 , 0  , 0x0a00);
    Serdes_set(unit, 0xbb1c,  31 , 16 , 0x2000);
    Serdes_set(unit, 0xbb20,  15 , 0  , 0xf00e);
    Serdes_set(unit, 0xbb20,  31 , 16 , 0xf04a);
    Serdes_set(unit, 0xbb24,  15 , 0  , 0x97b3);
    Serdes_set(unit, 0xbb24,  31 , 16 , 0x5318);
    Serdes_set(unit, 0xbb28,  15 , 0  , 0x0f03);
    Serdes_set(unit, 0xbb28,  31 , 16 , 0x0);
    Serdes_set(unit, 0xbb2c,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb2c,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb30,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb30,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb34,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xbb34,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb38,  15 , 0  , 0x1203);
    Serdes_set(unit, 0xbb38,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb3c,  15 , 0  , 0xa052);
    Serdes_set(unit, 0xbb3c,  31 , 16 , 0x9a00);
    Serdes_set(unit, 0xbb40,  15 , 0  , 0x00f5);
    Serdes_set(unit, 0xbb40,  31 , 16 , 0xf000);
    Serdes_set(unit, 0xbb44,  15 , 0  , 0x41ff);
    Serdes_set(unit, 0xbb44,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb48,  15 , 0  , 0x39ff);
    Serdes_set(unit, 0xbb48,  31 , 16 , 0x3340);
    Serdes_set(unit, 0xbb4c,  15 , 0  , 0x40aa);
    Serdes_set(unit, 0xbb4c,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb50,  15 , 0  , 0x801f);
    Serdes_set(unit, 0xbb50,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb54,  15 , 0  , 0x619c);
    Serdes_set(unit, 0xbb54,  31 , 16 , 0xffed);
    Serdes_set(unit, 0xbb58,  15 , 0  , 0x29ff);
    Serdes_set(unit, 0xbb58,  31 , 16 , 0x29ff);
    Serdes_set(unit, 0xbb5c,  15 , 0  , 0x4e10);
    Serdes_set(unit, 0xbb5c,  31 , 16 , 0x4e10);
    Serdes_set(unit, 0xbb60,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb60,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb80,  15 , 0  , 0x5800);
    Serdes_set(unit, 0xbb80,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xbb84,  15 , 0  , 0x5000);
    Serdes_set(unit, 0xbb84,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb88,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb88,  31 , 16 , 0x4000);
    Serdes_set(unit, 0xbb8c,  15 , 0  , 0x4000);
    Serdes_set(unit, 0xbb8c,  31 , 16 , 0xffff);
    Serdes_set(unit, 0xbb90,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xbb90,  31 , 16 , 0x806f);
    Serdes_set(unit, 0xbb94,  15 , 0  , 0x0004);
    Serdes_set(unit, 0xbb94,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb98,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbb98,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbb9c,  15 , 0  , 0x0a00);
    Serdes_set(unit, 0xbb9c,  31 , 16 , 0x2000);
    Serdes_set(unit, 0xbba0,  15 , 0  , 0xf00e);
    Serdes_set(unit, 0xbba0,  31 , 16 , 0xfdab);
    Serdes_set(unit, 0xbba4,  15 , 0  , 0x96ea);
    Serdes_set(unit, 0xbba4,  31 , 16 , 0x5318);
    Serdes_set(unit, 0xbba8,  15 , 0  , 0x0f03);
    Serdes_set(unit, 0xbba8,  31 , 16 , 0);
    Serdes_set(unit, 0xbbac,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbbac,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbb0,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbbb0,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbb4,  15 , 0  , 0xffff);
    Serdes_set(unit, 0xbbb4,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbb8,  15 , 0  , 0x1203);
    Serdes_set(unit, 0xbbb8,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbbc,  15 , 0  , 0xa052);
    Serdes_set(unit, 0xbbbc,  31 , 16 , 0x9a00);
    Serdes_set(unit, 0xbbc0,  15 , 0  , 0x00f5);
    Serdes_set(unit, 0xbbc0,  31 , 16 , 0xf000);
    Serdes_set(unit, 0xbbc4,  15 , 0  , 0x4079);
    Serdes_set(unit, 0xbbc4,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbc8,  15 , 0  , 0x93fa);
    Serdes_set(unit, 0xbbc8,  31 , 16 , 0x3340);
    Serdes_set(unit, 0xbbcc,  15 , 0  , 0x4280);
    Serdes_set(unit, 0xbbcc,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbd0,  15 , 0  , 0x801f);
    Serdes_set(unit, 0xbbd0,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xbbd4,  15 , 0  , 0x619c);
    Serdes_set(unit, 0xbbd4,  31 , 16 , 0xffed);
    Serdes_set(unit, 0xbbd8,  15 , 0  , 0x29ff);
    Serdes_set(unit, 0xbbd8,  31 , 16 , 0x29ff);
    Serdes_set(unit, 0xbbdc,  15 , 0  , 0x4c50);
    Serdes_set(unit, 0xbbdc,  31 , 16 , 0x4c50);
    Serdes_set(unit, 0xbbe0,  15 , 0  , 0x0000);
    Serdes_set(unit, 0xbbe0,  31 , 16 , 0x0000);
    Serdes_set(unit, 0xb018,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb118,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb818,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb918,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb3fc,  31 , 16 , 0x3f);
    Serdes_set(unit, 0xbbfc,  31 , 16 , 0x3f);
    Serdes_set(unit, 0xb00c,  30 , 30 , 1);
    Serdes_set(unit, 0xb10c,  30 , 30 , 1);
    Serdes_set(unit, 0xb40c,  30 , 30 , 1);
    Serdes_set(unit, 0xb50c,  30 , 30 , 1);
    Serdes_set(unit, 0xb80c,  30 , 30 , 1);
    Serdes_set(unit, 0xb90c,  30 , 30 , 1);

    for (i = 0; i < sizeof(ofst_list)/sizeof(uint32); ++i)
    {
        ofst = ofst_list[i];
        Serdes_set(unit, 0xb350 + ofst,  31 , 16 , 0x417f);
        Serdes_set(unit, 0xb338 + ofst,  9  , 9  , 0);
        Serdes_set(unit, 0xb338 + ofst,  12 , 10 , 0x0);
        Serdes_set(unit, 0xb338 + ofst,  5  , 3  , 0x5);
        Serdes_set(unit, 0xb338 + ofst,  8  , 6  , 0x0);
        Serdes_set(unit, 0xb338 + ofst,  2  , 0  , 0x2);
        Serdes_set(unit, 0xb340 + ofst,  31 , 16 , 0xc440);
        Serdes_set(unit, 0xb34c + ofst,  3  , 3  , 0);
        Serdes_set(unit, 0xb308 + ofst,  31 , 16 , 0x8000);
        Serdes_set(unit, 0xb30c + ofst,  15 , 0  , 0x8000);
        Serdes_set(unit, 0xb314 + ofst,  15 , 0  , 0x0);
        Serdes_set(unit, 0xb33c + ofst,  15 , 0  , 0x2);
        Serdes_set(unit, 0xb33c + ofst,  31 , 16 , 0xbe00);
        Serdes_set(unit, 0xb35c + ofst,  10 , 10 , 0);
        Serdes_set(unit, 0xb35c + ofst,  26 , 26 , 0);
        Serdes_set(unit, 0xb35c + ofst,  14 , 14 , 0);
        Serdes_set(unit, 0xb35c + ofst,  30 , 30 , 0);
        Serdes_set(unit, 0xb320 + ofst,  5  , 5  , 0);
        Serdes_set(unit, 0xb350 + ofst,  24 , 24 , 0);
        Serdes_set(unit, 0xb304 + ofst,  31 , 28 , 0xf);
        Serdes_set(unit, 0xb33c + ofst,  29 , 28 , 0x3);
        Serdes_set(unit, 0xb33c + ofst,  27 , 25 , 0x7);
        Serdes_set(unit, 0xb340 + ofst,  31 , 31 , 1);
        Serdes_set(unit, 0xb340 + ofst,  30 , 30 , 1);
        Serdes_set(unit, 0xb340 + ofst,  29 , 29 , 0);
        Serdes_set(unit, 0xb340 + ofst,  28 , 28 , 0);
        Serdes_set(unit, 0xb340 + ofst,  27 , 25 , 0x2);
        Serdes_set(unit, 0xb340 + ofst,  24 , 22 , 0x2);
        Serdes_set(unit, 0xb340 + ofst,  21 , 19 , 0x0);
        Serdes_set(unit, 0xb340 + ofst,  18 , 16 , 0x0);
        Serdes_set(unit, 0xb358 + ofst,  9  , 9  , 1);
        Serdes_set(unit, 0xb358 + ofst,  25 , 25 , 1);
        Serdes_set(unit, 0xb350 + ofst,  5  , 5  , 1);
        Serdes_set(unit, 0xb350 + ofst,  6  , 6  , 0);
        Serdes_set(unit, 0xb338 + ofst,  15 , 15 , 0);
        Serdes_set(unit, 0xb320 + ofst,  15 , 12 , 0x0);
        Serdes_set(unit, 0xb324 + ofst,  20 , 20 , 0);
        Serdes_set(unit, 0xb324 + ofst,  25 , 25 , 0);
        Serdes_set(unit, 0xb324 + ofst,  19 , 16 , 0x8);
        Serdes_set(unit, 0xb324 + ofst,  24 , 21 , 0x8);
    }

    for (i = 0; i < sizeof(ofst_list1)/sizeof(uint32); ++i)
    {
        ofst = ofst_list1[i];

        Serdes_set(unit, 0xb350 + ofst,  31 , 16 , 0x417f);
        Serdes_set(unit, 0xb338 + ofst,  9  , 9  , 0);
        Serdes_set(unit, 0xb338 + ofst,  12 , 10 , 0x0);
        Serdes_set(unit, 0xb338 + ofst,  5  , 3  , 0x5);
        Serdes_set(unit, 0xb338 + ofst,  8  , 6  , 0x0);
        Serdes_set(unit, 0xb338 + ofst,  2  , 0  , 0x2);
        Serdes_set(unit, 0xb340 + ofst,  31 , 16 , 0xc440);
        Serdes_set(unit, 0xb308 + ofst,  31 , 16 , 0x8000);
        Serdes_set(unit, 0xb30c + ofst,  15 , 0  , 0x8000);
        Serdes_set(unit, 0xb314 + ofst,  15 , 0  , 0x0);
        Serdes_set(unit, 0xb33c + ofst,  15 , 0  , 0x2);
        Serdes_set(unit, 0xb33c + ofst,  31 , 16 , 0xbe00);
        Serdes_set(unit, 0xb320 + ofst,  5  , 5  , 0);
        Serdes_set(unit, 0xb350 + ofst,  24 , 24 , 0);
        Serdes_set(unit, 0xb304 + ofst,  31 , 28 , 0xf);
        Serdes_set(unit, 0xb33c + ofst,  29 , 28 , 0x3);
        Serdes_set(unit, 0xb33c + ofst,  27 , 25 , 0x7);
        Serdes_set(unit, 0xb340 + ofst,  31 , 31 , 1);
        Serdes_set(unit, 0xb340 + ofst,  30 , 30 , 1);
        Serdes_set(unit, 0xb340 + ofst,  29 , 29 , 0);
        Serdes_set(unit, 0xb340 + ofst,  28 , 28 , 0);
        Serdes_set(unit, 0xb340 + ofst,  27 , 25 , 0x2);
        Serdes_set(unit, 0xb340 + ofst,  24 , 22 , 0x2);
        Serdes_set(unit, 0xb340 + ofst,  21 , 19 , 0x0);
        Serdes_set(unit, 0xb340 + ofst,  18 , 16 , 0x0);
        Serdes_set(unit, 0xb358 + ofst,  9  , 9  , 1);
        Serdes_set(unit, 0xb358 + ofst,  25 , 25 , 1);
        Serdes_set(unit, 0xb350 + ofst,  5  , 5  , 1);
        Serdes_set(unit, 0xb350 + ofst,  6  , 6  , 0);
        Serdes_set(unit, 0xb338 + ofst,  15 , 15 , 0);
        Serdes_set(unit, 0xb320 + ofst,  15 , 12 , 0x0);
        Serdes_set(unit, 0xb324 + ofst,  20 , 20 , 0);
        Serdes_set(unit, 0xb324 + ofst,  25 , 25 , 0);
        Serdes_set(unit, 0xb324 + ofst,  19 , 16 , 0x8);
        Serdes_set(unit, 0xb324 + ofst,  24 , 21 , 0x8);
    }
    Serdes_set(unit, 0xab10,  15 , 0  , 0x8c6a);
    Serdes_set(unit, 0xb710,  15 , 0  , 0x8c6a);

    for (i = 0; i < sizeof(ofst_list2)/sizeof(uint32); ++i)
    {
        ofst = ofst_list2[i];

        Serdes_set(unit, 0xa320 + ofst,  31 , 31 , 0);
        Serdes_set(unit, 0xa320 + ofst,  30 , 28 , 0x1);
        Serdes_set(unit, 0xa320 + ofst,  27 , 25 , 0x2);
        Serdes_set(unit, 0xa320 + ofst,  24 , 22 , 0x3);
        Serdes_set(unit, 0xa32c + ofst,  15 , 15 , 0);
        Serdes_set(unit, 0xa310 + ofst,  3  , 3  , 0);
        Serdes_set(unit, 0xa30c + ofst,  25 , 25 , 0);
        Serdes_set(unit, 0xa30c + ofst,  24 , 24 , 0);
        Serdes_set(unit, 0xa328 + ofst,  1  , 1  , 1);
        Serdes_set(unit, 0xa328 + ofst,  31 , 28 , 0xc);
        Serdes_set(unit, 0xa32c + ofst,  12 , 12 , 0);
        Serdes_set(unit, 0xa330 + ofst,  5  , 0  , 0x6);
        Serdes_set(unit, 0xa310 + ofst,  6  , 6  , 0);
        Serdes_set(unit, 0xa310 + ofst,  11 , 11 , 0);
        Serdes_set(unit, 0xa310 + ofst,  15 , 12 , 0x8);
        Serdes_set(unit, 0xa310 + ofst,  10 , 7  , 0x8);
    }
}   /* end of rtl839x_serdes_patch_init */

void rtl839x_5x_serdes_patch_init(uint32 unit)
{
    Serdes_set(unit, 0xb018,  15 , 0  , 0x08e4);
    Serdes_set(unit, 0xb118,  15 , 0  , 0x08e4);

    Serdes_set(unit, 0xb3fc,  31 , 16 , 0x2b);
    Serdes_set(unit, 0xb338,  15 , 0  , 0x0722);
    Serdes_set(unit, 0xb3b8,  15 , 0  , 0x0722);

    Serdes_set(unit, 0xb340,  15 , 0  , 0x18f5);
    Serdes_set(unit, 0xb3c0,  15 , 0  , 0x18f5);

    Serdes_set(unit, 0xa340, 31, 0, 0xc400043f);
    Serdes_set(unit, 0xa3c0, 31, 0, 0xc40043f);
    Serdes_set(unit, 0xa740, 31, 0, 0xc400043f);
    Serdes_set(unit, 0xa7c0, 31, 0, 0xc40043f);
    Serdes_set(unit, 0xab40, 31, 0, 0xc400043f);
    Serdes_set(unit, 0xabc0, 31, 0, 0xc40043f);
    Serdes_set(unit, 0xaf40, 31, 0, 0xc400043f);
    Serdes_set(unit, 0xafc0, 31, 0, 0xc40043f);
    Serdes_set(unit, 0xb740, 31, 0, 0xc400043f);
    Serdes_set(unit, 0xb7c0, 31, 0, 0xc40043f);
    Serdes_set(unit, 0xb3f8, 31, 0, 0x4c00000);
}   /* end of rtl839x_5x_serdes_patch_init */

/* Function Name:
 *      dal_cypress_construct_phy_reset
 * Description:
 *      Reset PHY.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_cypress_construct_phy_reset(uint32 unit)
{
    uint32  data;
    int32       ret;
    rtk_port_t  basePort = 0;
    uint32      phyIdx = 0;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);

    data = 0x0;
    reg_field_write(unit, CYPRESS_SMI_GLB_CTRLr, CYPRESS_MDX_POLLING_ENf, &data);

    HWP_PHY_TRAVS(unit, phyIdx)
    {
        basePort = HWP_PHY_BASE_MACID_BY_IDX(unit, phyIdx);
        if (!HWP_PORT_EXIST(unit, basePort))
            continue;

        ret = ioal_phy_reset(unit, basePort);
        if (ret != RT_ERR_OK)
            break;
    }
    if (ret == RT_ERR_CHIP_NOT_SUPPORTED)
    {
        uint32          mac_id;
        unsigned int    reg_val;

        /*reset all ports */
        HWP_PORT_TRAVS(unit, mac_id)
        {
            if (!(HWP_PORT_EXIST(unit, mac_id) && (HWP_PHY_EXIST(unit, mac_id) || HWP_SERDES_PORT(unit, mac_id))))
            {
                continue;
            }

            switch (HWP_PHY_MODEL_BY_PORT(unit, mac_id))
            {
                case RTK_PHYTYPE_RTL8218E:
                    basePort = HWP_PHY_BASE_MACID(unit, mac_id);
                    if (mac_id == basePort)
                    {
                        RTK_MII_WRITE(unit, mac_id, 0x0, 30, 0x8);
                        RTK_MII_WRITE(unit, mac_id, 0x262, 16, 0x1);
                        RTK_MII_WRITE(unit, mac_id, 0x0, 30, 0x0);
                        osal_time_mdelay(200);
                    }
                    break;
                default:
                    RTK_MII_READ(unit, mac_id, 0, 0, &reg_val);
                    reg_val |= (0x1 << 15);
                    RTK_MII_WRITE(unit, mac_id, 0, 0, reg_val);
                    break;
            }
        }/* HWP_PORT_TRAVS */
    }
    return;
} /* end of dal_cypress_construct_phy_reset */

/* Function Name:
 *      rtl8390_drv_macPhyPatch1
 * Description:
 *      Rx forcerun reset
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtl8390_drv_macPhyPatch1(uint32 unit)
{
    uint32  chiptype;
    #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208L))
    uint32  val;
    #endif
    int     i;
    uint8   macId=0;

    if (macId);

    HWP_PHY_TRAVS(unit, i)
    {
        macId = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
        chiptype = HWP_PHY_MODEL_BY_PHY(unit, i);

        switch (chiptype)
        {
            #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208L))
            case RTK_PHYTYPE_RTL8208D:
            case RTK_PHYTYPE_RTL8208L:
                if (macId == 0 || macId == 8 || macId == 16 || macId == 24 || macId == 40)
                {
                    RTK_MII_READ(unit,macId, 64, 16, &val);
                    val &= ~(0x1 << 13);
                    val |= (0x1 << 13);
                    RTK_MII_WRITE(unit,macId, 64, 16, val);
                }
                break;
            #endif
            #if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB))
            case RTK_PHYTYPE_RTL8218B:
            case RTK_PHYTYPE_RTL8214FC:
            case RTK_PHYTYPE_RTL8218FB:
                if (macId == 0 || macId == 8 || macId == 16 || macId == 24 || macId == 40)
                {
                    RTK_MII_WRITE(unit,macId, RTK_MII_MAXPAGE8390, 29, 8);
                    RTK_MII_WRITE(unit,macId, 0x467, 0x14, 0x3c);
                }
                break;
            #endif
        }
    }
    return;
} /* end of rtl8390_drv_macPhyPatch1 */

/* Function Name:
 *      rtl8390_drv_macPhyPatch2
 * Description:
 *      Rx forcerun reset
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void rtl8390_drv_macPhyPatch2(uint32 unit)
{
    uint32  chiptype;
    #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208L))
    uint32  val;
    #endif
    int     i;
    uint8   macId=0;

    if (macId);

    HWP_PHY_TRAVS(unit, i)
    {
        macId = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
        chiptype = HWP_PHY_MODEL_BY_PHY(unit, i);

        switch (chiptype)
        {
            #if (defined(CONFIG_SDK_RTL8208D) || defined(CONFIG_SDK_RTL8208L))
            case RTK_PHYTYPE_RTL8208D:
            case RTK_PHYTYPE_RTL8208L:
                if (macId == 0 || macId == 8 || macId == 16 || macId == 24 || macId == 40)
                {
                    RTK_MII_READ(unit,macId, 64, 16, &val);
                    val &= ~(0x1 << 13);
                    RTK_MII_WRITE(unit,macId, 64, 16, val);
                }
                break;
            #endif
            #if (defined(CONFIG_SDK_RTL8218B) || defined(CONFIG_SDK_RTL8214FC) || defined(CONFIG_SDK_RTL8218FB))
            case RTK_PHYTYPE_RTL8218B:
            case RTK_PHYTYPE_RTL8214FC:
            case RTK_PHYTYPE_RTL8218FB:
                if (macId == 0 || macId == 8 || macId == 16 || macId == 24 || macId == 40)
                {
                    RTK_MII_WRITE(unit,macId, 0x467, 0x14, 0);
                    RTK_MII_WRITE(unit,macId, RTK_MII_MAXPAGE8390, 29, 0);
                }
                break;
            #endif
        }
    }
    return;
} /* end of rtl8390_drv_macPhyPatch2 */

void rtl839x_serdes_cmu(uint32 unit, uint32 enable, uint32 sds_num)
{
    uint32 addr5g;
    uint32 addr10g;
    uint32 val;
    uint32 addr_ofst = 0x400;
    uint32 ofst;

    ofst = addr_ofst*(sds_num/2);
    if (sds_num % 2 == 0) {
        addr5g = 20;
        addr10g = 16;
    } else {
        addr5g = 22;
        addr10g = 18;
    }

    if (enable == 1) {
        val = 1;
    } else {
        val = 0;
    }

    if (sds_num < 8 || sds_num == 10 || sds_num == 11) {
        Serdes_set(unit, 0xa3c0 + ofst,  addr5g, addr5g, 1);
        Serdes_set(unit, 0xa3c0 + ofst,  (addr5g + 1), (addr5g + 1), val);
    } else if (sds_num == 8 || sds_num == 9) {
        Serdes_set(unit, 0xb3f8,  addr10g, addr10g, 1);
        Serdes_set(unit, 0xb3f8,  (addr10g + 1), (addr10g + 1), val);
    } else if (sds_num == 12 || sds_num == 13) {
        Serdes_set(unit, 0xbbf8,  addr10g, addr10g, 1);
        Serdes_set(unit, 0xbbf8,  (addr10g + 1), (addr10g + 1), val);
    } else {
        osal_printf( "sds number doesn't exist");
        return;
    }
}

void rtl839x_93m_rst_sys(uint32 unit)
{
    rtl8390_drv_macPhyPatch1(unit);

    Serdes_set(unit, 0x0014,  4  , 4  , 1);
    Serdes_set(unit, 0xb880,  11  , 11  , 0x1);
    Serdes_set(unit, 0xb980,  11  , 11  , 0x1);

    osal_time_mdelay(500);
    Serdes_set(unit, 0xb018,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb118,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb818,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb918,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb3fc,  31 , 16 , 0x3f);
    Serdes_set(unit, 0xbbfc,  31 , 16 , 0x3f);
    Serdes_set(unit, 0xb00c,  30 , 30 , 1);
    Serdes_set(unit, 0xb10c,  30 , 30 , 1);
    Serdes_set(unit, 0xb40c,  30 , 30 , 1);
    Serdes_set(unit, 0xb50c,  30 , 30 , 1);
    Serdes_set(unit, 0xb80c,  30 , 30 , 1);
    Serdes_set(unit, 0xb90c,  30 , 30 , 1);

    rtl8390_drv_macPhyPatch2(unit);
}

void rtl839x_53m_rst_sys (uint32 unit)
{
    uint32 sdsList[] = {1, 3, 5, 7, 11};
    uint32 i;

    rtl8390_drv_macPhyPatch1(unit);

    Serdes_set(unit, 0x0014,  4  , 4  , 1);

    osal_time_mdelay(500);
    Serdes_set(unit, 0xb018,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb118,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb818,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb918,  15 , 0  , 0x08ec);
    Serdes_set(unit, 0xb3fc,  31 , 16 , 0x3f);
    Serdes_set(unit, 0xbbfc,  31 , 16 , 0x3f);
    Serdes_set(unit, 0xb00c,  30 , 30 , 1);
    Serdes_set(unit, 0xb10c,  30 , 30 , 1);
    Serdes_set(unit, 0xb40c,  30 , 30 , 1);
    Serdes_set(unit, 0xb50c,  30 , 30 , 1);
    Serdes_set(unit, 0xb80c,  30 , 30 , 1);
    Serdes_set(unit, 0xb90c,  30 , 30 , 1);

    Serdes_set(unit, 0xb018,  15 , 0  , 0x08e4);
    Serdes_set(unit, 0xb118,  15 , 0  , 0x08e4);
    Serdes_set(unit, 0xb3fc,  21 , 16 , 0x2b);

    for (i = 0; i < sizeof(sdsList)/sizeof(uint32); ++i)
        rtl839x_serdes_cmu(unit, 0, sdsList[i]);

    Serdes_set(unit, 0xb3f8,  23 , 23 , 1);
    Serdes_set(unit, 0xb3f8,  22 , 22 , 1);
    Serdes_set(unit, 0xb3f8,  26 , 26 , 1);
    Serdes_set(unit, 0xb3f8,  27 , 27 , 0);
    Serdes_set(unit, 0xbbf8,  23 , 23 , 1);
    Serdes_set(unit, 0xbbf8,  22 , 22 , 1);
    Serdes_set(unit, 0xbbf8,  26 , 26 , 1);
    Serdes_set(unit, 0xbbf8,  27 , 27 , 0);

    rtl8390_drv_macPhyPatch2(unit);
}

confcode_serdes_patch_t rtl839x_eee_enable0[] = {
    { 0, 0, 14, 10, 0x0},
    { 0, 0,  9,  5, 0x10},
    { 0, 0,  4,  0, 0x10},
};

confcode_serdes_patch_t rtl839x_eee_enable1[] = {
    //#qsgmii_lpi_tx_en=1, qsgmii_lpi_rx_en=1
    { 0, 0,  9,  8, 0x3},
};

void rtl8390_serdes_eee_enable(uint32 unit, const int sds_num)
{
    int eee_sds_addr0 = 0xa0e0;
    int eee_sds_addr1 = 0xa014;
    int eee_sds_addr_ofst = 0x400;
    int addr_ofst;
    int idx;

    addr_ofst = (eee_sds_addr_ofst * (sds_num / 2)) + (0x100 * (sds_num % 2));

    for (idx = 0; idx < (sizeof(rtl839x_eee_enable0)/sizeof(confcode_serdes_patch_t)); ++idx)
    {
        rtl839x_eee_enable0[idx].reg = eee_sds_addr0 + addr_ofst;
        Serdes_patch_set_chk(unit, rtl839x_eee_enable0[idx]);
    }

    for (idx = 0; idx < (sizeof(rtl839x_eee_enable1)/sizeof(confcode_serdes_patch_t)); ++idx)
    {
        rtl839x_eee_enable1[idx].reg = eee_sds_addr1 + addr_ofst;
        Serdes_patch_set_chk(unit, rtl839x_eee_enable1[idx]);
    }

    return ;
}   /* end of rtl8390_serdes_eee_enable */

int rtl8390_serdes_chk(uint32 unit, const int sdsId)
{
    uint32  base = 0xa078, chkPos, val;
    int     id;
    int32   ret;

    chkPos = base + (0x400 * (sdsId / 2)) + (0x100 * (sdsId % 2));

    if (HWP_8350_FAMILY_ID(unit))
    {
        id = sdsId / 2;
    }
    else
        id = sdsId;

    ret = ioal_mem32_read(unit, chkPos, &val);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }

    if (val != 0x1ff0000)
    {
        osal_printf("[WARN] Serdes %d initial fail %x %x\n", id, chkPos, val);
        return 1;
    }

    return 0;
}

confcode_mac_regval_t rtl835x_serdes_powerOff_conf[] = {
    { 0xa340, 0xc400043f},
    { 0xa3c0, 0xc40043f },
    { 0xa740, 0xc400043f},
    { 0xa7c0, 0xc40043f },
    { 0xab40, 0xc400043f},
    { 0xabc0, 0xc40043f },
    { 0xaf40, 0xc400043f},
    { 0xafc0, 0xc40043f },
    { 0xb740, 0xc400043f},
    { 0xb7c0, 0xc40043f },
    { 0xb3f8, 0x4c00000 },
};


/* Function Name:
 *      dal_cypress_construct_default_init
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
void dal_cypress_construct_default_init(uint32 unit)
{

    return;
}


/* Function Name:
 *      dal_cypress_construct_phyConfig_init
 * Description:
 *      PHY Configuration code that connect with RTL8390
 * Input:
 *      pModel - pointer to switch model of platform
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_cypress_construct_phyConfig_init(uint32 unit)
{
    uint32  i;
    uint32  data;
    uint8   macId, baseMacId;
    int32   phyPort;
    int32   ret;
    phy_type_t phyType;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);
    debug_regShow_set(1);

    /* Disable MAC polling PHY setting */
    data = 0x0;
    ret = reg_field_write(unit, CYPRESS_SMI_GLB_CTRLr,CYPRESS_MDX_POLLING_ENf,&data);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }

    phy_construct_config_init(unit);

    HWP_PHY_TRAVS(unit, i)
    {
        phyType = HWP_PHY_MODEL_BY_PHY(unit, i);
        baseMacId = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
        for (phyPort = 0; phyPort < HWP_PHY_MAX_PORT_NUM_BY_IDX(unit, i); ++phyPort)
        {
            macId = baseMacId + phyPort;
            /* Park Page to 0 */
            if(HWP_PORT_SMI(unit,macId) != 2) /*This is 10G SMI interface with Clause 45 is return index is 2, part to page 0 is unnecesaary*/
            {
                RTK_MII_WRITE(unit, macId, RTK_MII_MAXPAGE8390, 31, 0);
            }

            if (phy_construct_enable_set(phyType, unit, macId, ENABLED) != RT_ERR_OK)
            {
                CNSTRT_ERR("%s,%d: %d.%d phy enable fail.\n", __FUNCTION__, __LINE__, unit, macId);
            }
        } /* end for */
    } /* end HWP_PHY_TRAVS */

    osal_time_mdelay(500);

    HWP_PHY_TRAVS(unit, i)
    {
        phyType = HWP_PHY_MODEL_BY_PHY(unit, i);
        baseMacId = HWP_PHY_BASE_MACID_BY_IDX(unit, i);
        for (phyPort = 0; phyPort < HWP_PHY_MAX_PORT_NUM_BY_IDX(unit, i); ++phyPort)
        {
            macId = baseMacId + phyPort;
            if (phy_construct_enable_set(phyType, unit, macId, DISABLED) != RT_ERR_OK)
            {
                CNSTRT_ERR("%s,%d: %d.%d phy enable fail.\n", __FUNCTION__, __LINE__, unit, macId);
            }
        } /* end for */
    } /* end HWP_PHY_TRAVS */

    #if (defined(CONFIG_SDK_RTL8214FC))
    rtl8214fc_rtl8390_post_config(unit);
    #endif  /* defined(CONFIG_SDK_RTL8214FC) */

    /* Restore MAC polling PHY setting */
    data = 0x1;
    ret = reg_field_write(unit, CYPRESS_SMI_GLB_CTRLr,CYPRESS_MDX_POLLING_ENf,&data);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }

    debug_regShow_set(0);
    return;
} /* end of dal_cypress_construct_phyConfig_init */

void _dal_cypress_construct_serdes_10g_rx_rst(uint32 unit, int sds_num)
{
    int ofst;

    switch (sds_num)
    {
        case 8:
            ofst = 0x0;
            break;
        case 12:
            ofst = 0x800;
            break;
        default:
            return;
    }

    Serdes_set(unit, 0xb318 + ofst, 26, 26, 0);
    Serdes_set(unit, 0xb320 + ofst,  3 , 3 , 0x0);
    Serdes_set(unit, 0xb340 + ofst, 15, 15, 0x1);
    osal_time_mdelay(10);
    Serdes_set(unit, 0xb340 + ofst, 15, 15, 0x0);

    Serdes_set(unit, 0xb004 + ofst, 31, 16, 0x7146);
    osal_time_mdelay(10);
    Serdes_set(unit, 0xb004 + ofst, 31, 16, 0x7106);

    Serdes_set(unit, 0xb284 + ofst, 12, 12, 1);
    osal_time_mdelay(10);
    Serdes_set(unit, 0xb284 + ofst, 12, 12, 0);

    osal_time_mdelay(500);
    Serdes_set(unit, 0xb318 + ofst, 26, 26, 1);
}   /* end of _dal_cypress_construct_serdes_10g_rx_rst */

void _dal_cypress_construct_serdes_init_96m (uint32 unit)
{
    if (HWP_CHIP_REV(unit) < CHIP_REV_ID_C)
    {
        Serdes_set(unit, 0x8,  31 , 0  , 0x666666);
        Serdes_set(unit, 0xc,  31 , 0  , 0x10001);
        Serdes_set(unit, 0xab10,  15 , 0  , 0x8c6a);
        Serdes_set(unit, 0xb710,  15 , 0  , 0x8c6a);
        Serdes_set(unit, 0xb3fc,  16 , 16 , 1);
        Serdes_set(unit, 0xb3fc,  17 , 17 , 1);
        Serdes_set(unit, 0xb3fc,  19 , 18 , 0x3);
        Serdes_set(unit, 0xb3fc,  21 , 20 , 0x3);
        Serdes_set(unit, 0xbbfc,  16 , 16 , 1);
        Serdes_set(unit, 0xbbfc,  17 , 17 , 1);
        Serdes_set(unit, 0xbbfc,  19 , 18 , 0x3);
        Serdes_set(unit, 0xbbfc,  21 , 20 , 0x3);
        Serdes_set(unit, 0xb3fc,  8  , 8  , 1);
        Serdes_set(unit, 0xb3fc,  9  , 9  , 0);
        Serdes_set(unit, 0xb3fc,  10 , 10 , 1);
        Serdes_set(unit, 0xb3fc,  11 , 11 , 0);
        Serdes_set(unit, 0xb3fc,  7  , 4  , 0xf);
        Serdes_set(unit, 0xbbfc,  8  , 8  , 1);
        Serdes_set(unit, 0xbbfc,  9  , 9  , 0);
        Serdes_set(unit, 0xbbfc,  10 , 10 , 1);
        Serdes_set(unit, 0xbbfc,  11 , 11 , 0);
        Serdes_set(unit, 0xbbfc,  7  , 4  , 0xf);
        Serdes_set(unit, 0xb3f8,  22 , 22 , 1);
        Serdes_set(unit, 0xb3f8,  23 , 23 , 1);
        Serdes_set(unit, 0xbbf8,  22 , 22 , 1);
        Serdes_set(unit, 0xbbf8,  23 , 23 , 1);
        Serdes_set(unit, 0xba84,  10 , 9  , 0x1);
        Serdes_set(unit, 0xb284,  10 , 9  , 0x1);
        Serdes_set(unit, 0xb280,  25 , 24 , 0x3);
        Serdes_set(unit, 0xba80,  25 , 24 , 0x3);
        Serdes_set(unit, 0xb37c,  0  , 0  , 1);
        Serdes_set(unit, 0xbb7c,  0  , 0  , 1);
        Serdes_set(unit, 0xaf40,  27 , 24 , 0xf);

        _dal_cypress_construct_serdes_10g_rx_rst(unit, 8);
        _dal_cypress_construct_serdes_10g_rx_rst(unit, 12);

        Serdes_set(unit, 0xb284,  14 , 14 , 1);
        Serdes_set(unit, 0xba84,  14 , 14 , 1);
        Serdes_set(unit, 0x3fc,  31 , 0  , 0xffffff);
        Serdes_set(unit, 0x400,  31 , 0  , 0x0);
        Serdes_set(unit, 0x031c,  7  , 0  , 0x9f);
        Serdes_set(unit, 0x034c,  7  , 0  , 0x9f);
        Serdes_set(unit, 0x71bc,  20 , 20 , 1);
        Serdes_set(unit, 0x71ec,  20 , 20 , 1);
        Serdes_set(unit, 0x71bc,  19 , 4  , 0xfff);
        Serdes_set(unit, 0x71bc,  3  , 0  , 0xf);
        Serdes_set(unit, 0x71ec,  19 , 4  , 0xfff);
        Serdes_set(unit, 0x71ec,  3  , 0  , 0xf);

        Serdes_set(unit, 0xb0b0,  3  , 3  , 1);
        Serdes_set(unit, 0xb8b0,  3  , 3  , 1);
    }
    else
    {
        Serdes_set(unit, 0x8,  31 , 0  , 0x666666);
        Serdes_set(unit, 0xc,  31 , 0  , 0x10001);
        Serdes_set(unit, 0xab10,  15 , 0  , 0x8c6a);
        Serdes_set(unit, 0xb710,  15 , 0  , 0x8c6a);
        Serdes_set(unit, 0xb3fc,  16 , 16 , 1);
        Serdes_set(unit, 0xb3fc,  17 , 17 , 1);
        Serdes_set(unit, 0xb3fc,  19 , 18 , 0x3);
        Serdes_set(unit, 0xb3fc,  21 , 20 , 0x3);
        Serdes_set(unit, 0xbbfc,  16 , 16 , 1);
        Serdes_set(unit, 0xbbfc,  17 , 17 , 1);
        Serdes_set(unit, 0xbbfc,  19 , 18 , 0x3);
        Serdes_set(unit, 0xbbfc,  21 , 20 , 0x3);
        Serdes_set(unit, 0xb3fc,  8  , 8  , 1);
        Serdes_set(unit, 0xb3fc,  9  , 9  , 0);
        Serdes_set(unit, 0xb3fc,  10 , 10 , 1);
        Serdes_set(unit, 0xb3fc,  11 , 11 , 0);
        Serdes_set(unit, 0xb3fc,  7  , 4  , 0xf);
        Serdes_set(unit, 0xbbfc,  8  , 8  , 1);
        Serdes_set(unit, 0xbbfc,  9  , 9  , 0);
        Serdes_set(unit, 0xbbfc,  10 , 10 , 1);
        Serdes_set(unit, 0xbbfc,  11 , 11 , 0);
        Serdes_set(unit, 0xbbfc,  7  , 4  , 0xf);
        Serdes_set(unit, 0xb3f8,  22 , 22 , 1);
        Serdes_set(unit, 0xb3f8,  23 , 23 , 1);
        Serdes_set(unit, 0xbbf8,  22 , 22 , 1);
        Serdes_set(unit, 0xbbf8,  23 , 23 , 1);
        Serdes_set(unit, 0xba84,  10 , 9  , 0x1);
        Serdes_set(unit, 0xb284,  10 , 9  , 0x1);
        Serdes_set(unit, 0xb280,  25 , 24 , 0x3);
        Serdes_set(unit, 0xba80,  25 , 24 , 0x3);
        Serdes_set(unit, 0xb37c,  0  , 0  , 1);
        Serdes_set(unit, 0xbb7c,  0  , 0  , 1);
        Serdes_set(unit, 0xaf40,  27 , 24 , 0xf);

        _dal_cypress_construct_serdes_10g_rx_rst(unit, 8);
        _dal_cypress_construct_serdes_10g_rx_rst(unit, 12);

        Serdes_set(unit, 0xb284,  14 , 14 , 1);
        Serdes_set(unit, 0xba84,  14 , 14 , 1);
        //port polling will handled by mac init
        //Serdes_set(unit, 0x3fc,  31 , 0  , 0xffffff);
        //Serdes_set(unit, 0x400,  31 , 0  , 0x0);
        Serdes_set(unit, 0x031c,  0  , 0  , 0x0);
        Serdes_set(unit, 0x034c,  0  , 0  , 0x0);
        Serdes_set(unit, 0x71bc,  20 , 20 , 1);
        Serdes_set(unit, 0x71ec,  20 , 20 , 1);
        Serdes_set(unit, 0x71bc,  19 , 4  , 0xfff);
        Serdes_set(unit, 0x71bc,  3  , 0  , 0xf);
        Serdes_set(unit, 0x71ec,  19 , 4  , 0xfff);
        Serdes_set(unit, 0x71ec,  3  , 0  , 0xf);

        Serdes_set(unit, 0xb0b0,  3  , 3  , 1);
        Serdes_set(unit, 0xb8b0,  3  , 3  , 1);

        Serdes_set(unit, 0xb3a0, 31  , 16 , 0xFDAB);
        Serdes_set(unit, 0xbba0, 31  , 16 , 0xFDAB);
        Serdes_set(unit, 0xb3cc, 7   , 7  , 0);
        Serdes_set(unit, 0xbbcc, 7   , 7  , 0);
    }
}   /* end of _dal_cypress_construct_serdes_init_96m */

/* Function Name:
 *      dal_cypress_construct_serdesConfig_init
 * Description:
 *      Serdes Configuration code that connect with RTL8390
 * Input:
 *      pModel - pointer to switch model of platform
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void dal_cypress_construct_serdesConfig_init(uint32 unit)
{
    uint32  sdsId, idx, val;
    int32   ret;
    int i, macId;

    CNSTRT_PRINT("%s()\n",__FUNCTION__);
    debug_regShow_set(1);

    rtl839x_serdes_patch_init(unit);

    if (HWP_8350_FAMILY_ID(unit))
    {
        rtl839x_5x_serdes_patch_init(unit);
    }

    if (HWP_CHIP_ID(unit) == RTL8351M_CHIP_ID)
    {
        Serdes_set(unit, 0x0008,  31 , 0  , 0x6444444);
    }

    HWP_PHY_TRAVS(unit, idx)
    {
        if (RTK_PHYTYPE_RTL8214C == HWP_PHY_MODEL_BY_PHY(unit, idx))
        {
            Serdes_set(unit, 0xaf28,  4  , 3  , 0x1);
            Serdes_set(unit, 0xaf28,  1  , 1  , 0x0);
            break;
        }
    }

    for (sdsId = 0; sdsId < 14; ++sdsId)
    {
        if (HWP_SDS_EXIST(unit, sdsId) && (HWP_SDS_MODE(unit, sdsId) != RTK_MII_DISABLE))
            continue;

        val = 0;
        ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                REG_ARRAY_INDEX_NONE, sdsId, CYPRESS_SERDES_SPD_SELf, &val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
    }

    if (HWP_SDS_MODE(unit, 12) == RTK_MII_1000BX_FIBER)
    {
        Serdes_set(unit, 0xb880,  11  , 11  , 0x1);
        ret = phy_speed_set(unit, 48, PORT_SPEED_1000M);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
    }

    if (HWP_SDS_MODE(unit, 13) == RTK_MII_1000BX_FIBER)
    {
        Serdes_set(unit, 0xb980,  11  , 11  , 0x1);
        ret = phy_speed_set(unit, 49, PORT_SPEED_1000M);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
    }

    HWP_SDS_TRAVS(unit, sdsId)
    {
        rtl839x_serdes_reset(unit, sdsId);
    }

    if (HWP_8390_FAMILY_ID(unit))
    {
        rtl839x_93m_rst_sys(unit);
    }
    else
    {
        rtl839x_53m_rst_sys(unit);
    }

    /* digit serdes config must be after MAC serdes reset */
    if (HWP_SDS_MODE(unit, 12) == RTK_MII_1000BX_FIBER)
    {
        ret = phy_speed_set(unit, 48, PORT_SPEED_1000M);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
    }

    if (HWP_SDS_MODE(unit, 13) == RTK_MII_1000BX_FIBER)
    {
        ret = phy_speed_set(unit, 49, PORT_SPEED_1000M);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
    }

    HWP_SDS_TRAVS(unit, sdsId)
    {
        rtl8390_serdes_eee_enable(unit, sdsId);
    }

    /* power off */
    if (HWP_8350_FAMILY_ID(unit))
    {
        for (idx = 0; idx < (sizeof(rtl835x_serdes_powerOff_conf)/sizeof(confcode_mac_regval_t)); ++idx)
        {
            MacRegSetCheck(unit, rtl835x_serdes_powerOff_conf[idx].reg, rtl835x_serdes_powerOff_conf[idx].val);
        }
    }

    if (HWP_CHIP_ID(unit) == RTL8391M_CHIP_ID ||
            HWP_CHIP_ID(unit) == RTL8392M_CHIP_ID ||
            HWP_CHIP_ID(unit) == RTL8351M_CHIP_ID)
    {
        ret = ioal_mem32_read(unit, 0xAF40, &val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }

        val |= (0xF << 24);
        ret = ioal_mem32_write(unit, 0xAF40, val);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
        }
    }

    if (HWP_CHIP_ID(unit) == RTL8396M_CHIP_ID)
    {
        _dal_cypress_construct_serdes_init_96m(unit);

        /* power off non-used serdes */
        for (sdsId = 0; sdsId < 14; ++sdsId)
        {
            if (HWP_SDS_EXIST(unit, sdsId) && (HWP_SDS_MODE(unit, sdsId) != RTK_MII_DISABLE))
                continue;
            val = 0;
            ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                    REG_ARRAY_INDEX_NONE, sdsId, CYPRESS_SERDES_SPD_SELf, &val);
            if (ret != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
            }
        }

        if (HWP_SDS_MODE(unit, 8) == RTK_MII_10GR)
        {
            ret = phy_8390_10gMedia_set(unit, 24, PORT_10GMEDIA_FIBER_10G);
            if (ret != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
            }
        }

        if (HWP_SDS_MODE(unit, 12) == RTK_MII_10GR)
        {
            ret = phy_8390_10gMedia_set(unit, 36, PORT_10GMEDIA_FIBER_10G);
            if (ret != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
            }
        }
    }

    HWP_PHY_TRAVS(unit, i)
    {
        macId = HWP_PHY_BASE_MACID_BY_IDX(unit, i);

        switch (HWP_PHY_MODEL_BY_PHY(unit, i))
        {
            #if (defined(CONFIG_SDK_RTL8218D))
            case RTK_PHYTYPE_RTL8218D:
                Serdes_set(unit, 0, 31, 0, 0);

                if (0 == macId)
                {
                    Serdes_set(unit, 0xA330, 31, 28, 0xA);
                    Serdes_set(unit, 0xA3B0, 31, 28, 0xA);
                }
                else if (8 == macId)
                {
                    Serdes_set(unit, 0xA730, 31, 28, 0xA);
                    Serdes_set(unit, 0xA7B0, 31, 28, 0xA);
                }
                else if (16 == macId)
                {
                    Serdes_set(unit, 0xAB30, 31, 28, 0xA);
                    Serdes_set(unit, 0xABB0, 31, 28, 0xA);
                }
                else if (24 == macId)
                {
                    Serdes_set(unit, 0xAF30, 31, 28, 0xA);
                    Serdes_set(unit, 0xAFB0, 31, 28, 0xA);
                }
                else if (32 == macId)
                {
                    Serdes_set(unit, 0xB350, 9, 5, 0x8);
                    Serdes_set(unit, 0xB3D0, 9, 5, 0x8);
                }
                else if (40 == macId)
                {
                    Serdes_set(unit, 0xB730, 31, 28, 0xA);
                    Serdes_set(unit, 0xB7B0, 31, 28, 0xA);
                }

                Serdes_set(unit, 0, 31, 0, 0xc00);
                break;
            #endif/* CONFIG_SDK_RTL8218D */
            default:
                /* for compiler warning */
                if (0 == macId) {}
        }
    }
    debug_regShow_set(0);
    return;
} /* end of dal_cypress_construct_serdesConfig_init */

void rtl8390_sfp_speed_set(uint32 unit, int port, int speed)
{
    int macId, portSpeed;
    int ret;

    if (0 == HWP_SERDES_PORT_COUNT(unit))
        return;

    if (0 == port)
        macId = 48;
    else
        macId = 49;

    if (1000 == speed)
        portSpeed = PORT_SPEED_1000M;
    else
        portSpeed = PORT_SPEED_100M;

    ret = phy_speed_set(unit, macId, portSpeed);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GENERAL), "");
    }

    return;
}


