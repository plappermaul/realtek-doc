/*
 * Copyright (C) 2009-2020 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 74379 $
 * $Date: 2016-12-19 11:39:00 +0800 (Mon, 19 Dec 2016) $
 *
 * Purpose : PHY 8264 Driver APIs.
 *
 * Feature : PHY 8264 Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_rtl826x.h>
#include <hal/phy/phy_construct.h>
#include <osal/time.h>
#include <hal/phy/construct/conftypes.h>
#if (defined(CONFIG_SDK_RTL826X) || defined(CONFIG_SDK_RTL826XI) || defined(CONFIG_SDK_RTL825X) || defined(CONFIG_SDK_RTL825XI) || defined(CONFIG_SDK_RTL825XL) || defined(CONFIG_SDK_RTL8224))
#include <hal/phy/construct/conf_rtl8264.c>
#endif
/*
 * Symbol Definition
 */

#define PHY_RTL8264_OP_NORMAL 0
#define PHY_RTL8264_OP_BCAST  1

#define PHY_RTL8264_A_PATCH                  0
#if PHY_RTL8264_A_PATCH
#include <hal/phy/construct/conf_rtl8264_A.c>
#define PHY_RTL8264_A_PATCH_SDS_PATCH        rtl8264_A_patch_sds_conf
#define PHY_RTL8264_A_PATCH_AFE_PATCH        rtl8264_A_patch_afe_conf
#define PHY_RTL8264_A_PATCH_SRAM_PATCH       rtl8264_A_patch_sram_conf
#define PHY_RTL8264_A_PATCH_SRAM_UC_PATCH    rtl8264_A_patch_sram_uc_conf
#define PHY_RTL8264_A_PATCH_SRAM_ALGO_PATCH  rtl8264_A_patch_sram_algo_conf
#define PHY_RTL8264_A_PATCH_TOP_PATCH        rtl8264_A_patch_top_conf
#define PHY_RTL8264_A_PATCH_ALGO1G_PATCH     rtl8264_A_patch_algo1g_conf
#define PHY_RTL8264_A_PATCH_DATARAM_PATCH    rtl8264_A_patch_dataram_conf
#endif

#define PHY_RTL8264_PATCH_SRAM_VER_CHK     0
#define PHY_RTL8264_PATCH_WRITE_AND_CHECK  0
#define PHY_RTL8264_PATCH_WAIT_TIMEOUT     1000000

#define PHY_826X_LOG    LOG_INFO

/*
 * Data Declaration
 */

rtk_phy_hwpatch_t rtl826X_patch_rtk_conf[] = {
    {RTK_HWPATCH_OP_SDS, 0xff, 0x07, 0x10, 15, 0, 0x80aa},
    {RTK_HWPATCH_OP_SDS, 0xff, 0x06, 0x12, 15, 0, 0x5078},
};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

uint32
_phy_rtl826x_patch_mask(uint8 msb, uint8 lsb)
{
    uint32 val = 0;
    uint8    i = 0;

    for (i = lsb; i <= msb; i++)
    {
        val |= (1 << i);
    }
    return val;
}

int32
_phy_rtl826x_patch_mask_get(uint8 msb, uint8 lsb, uint32 *mask)
{
    if ((msb > 15) || (lsb > 15) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }
    *mask = _phy_rtl826x_patch_mask(msb, lsb);
    return RT_ERR_OK;
}

int32
_phy_rtl826x_patch_wait(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data, uint32 mask, uint8 bcast_op)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 cnt = 0;

#if PHY_826X_CONSTRUCT_BROADCAST
    rtk_port_t  p = 0;
    uint8  smiBus = HWP_PORT_SMI(unit, port);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(unit, port);
    uint8  bcast_phyad = UNITMAP(unit)->hwp_macID2PortDescp[port]->phy_addr;

    if (bcast_op == PHY_RTL8264_OP_BCAST)
    {
        if ((ret = phy_826x_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                do
                {
                    if ((ret = phy_common_general_reg_mmd_get(unit, p, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }

                    if (++cnt >= PHY_RTL8264_PATCH_WAIT_TIMEOUT)
                    {
                        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                        return RT_ERR_TIMEOUT;
                    }
                    //osal_time_udelay(10);
                }while ((rData & mask) != data);
            }
        }

        osal_time_mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                if ((ret = phy_826x_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_826x_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else
#endif
    {
        do
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            if (++cnt >= PHY_RTL8264_PATCH_WAIT_TIMEOUT)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, port, mmdAddr, mmdReg, data, mask, rData, cnt);
                return RT_ERR_TIMEOUT;
            }
            osal_time_mdelay(1);
        }while ((rData & mask) != data);
    }

    return RT_ERR_OK;
}

int32
_phy_rtl826x_patch_wait_not_equal(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data, uint32 mask, uint8 bcast_op)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 cnt = 0;

#if PHY_826X_CONSTRUCT_BROADCAST
    rtk_port_t  p = 0;
    uint8  smiBus = HWP_PORT_SMI(unit, port);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(unit, port);
    uint8  bcast_phyad = UNITMAP(unit)->hwp_macID2PortDescp[port]->phy_addr;

    if (bcast_op == PHY_RTL8264_OP_BCAST)
    {
        if ((ret = phy_826x_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
            return ret;
        }


        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                do
                {
                    if ((ret = phy_common_general_reg_mmd_get(unit, p, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }

                    if (++cnt >= PHY_RTL8264_PATCH_WAIT_TIMEOUT)
                    {
                        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                        return RT_ERR_TIMEOUT;
                    }
                    //osal_time_udelay(10);
                }while ((rData & mask) == data);
            }
        }

        osal_time_mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                if ((ret = phy_826x_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_826x_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else
#endif
    {
        do
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            if (++cnt >= PHY_RTL8264_PATCH_WAIT_TIMEOUT)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, port, mmdAddr, mmdReg, data, mask, rData, cnt);
                return RT_ERR_TIMEOUT;
            }
            osal_time_mdelay(1);
        }while ((rData & mask) == data);
    }

    return RT_ERR_OK;
}

int32
_phy_rtl826x_patch_top_get(uint32 unit, rtk_port_t port, uint32 topPage, uint32 topReg, uint32 *pData)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 topAddr = (topPage * 8) + (topReg - 16);

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, topAddr, &rData)) != RT_ERR_OK)
        return ret;
    *pData = rData;
    return RT_ERR_OK;
}

int32
_phy_rtl826x_patch_top_set(uint32 unit, rtk_port_t port, uint32 topPage, uint32 topReg, uint32 wData)
{
    int32  ret = 0;
    uint32 topAddr = (topPage * 8) + (topReg - 16);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, topAddr, wData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_rtl826x_patch_sds_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 sdsAddr = 0x8000 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826x_patch_top_set(unit, port, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826x_patch_top_get(unit, port, 40, 18, &rData)) != RT_ERR_OK)
        return ret;
    *pData = rData;
    return _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND1, 0x143, 0, BIT_15, PHY_RTL8264_OP_NORMAL);
}

int32
_phy_rtl826x_patch_sds_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 wData)
{
    int32  ret = 0;
    uint32 sdsAddr = 0x8800 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826x_patch_top_set(unit, port, 40, 17, wData)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826x_patch_top_set(unit, port, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    return _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND1, 0x143, 0, BIT_15, PHY_RTL8264_OP_NORMAL);
}

int32
_phy_rtl826x_patch_sds_bcast_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 wData)
{
    int32  ret = 0;
    uint32 sdsAddr = 0x8800 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826x_patch_top_set(unit, port, 40, 17, wData)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826x_patch_top_set(unit, port, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    return _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND1, 0x143, 0, BIT_15, PHY_RTL8264_OP_BCAST);
}

int32 _phy_rtl826x_patch_data_ram_write(uint32 unit, uint8 port, uint32 addr, uint32 data)
{
    int32 ret = 0;
    uint32 rData = 0, wData = 0;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB88E, addr)) != RT_ERR_OK)
        return ret;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB890, &rData)) != RT_ERR_OK)
        return ret;


    if ((addr % 2) == 0)
    {
        wData = REG32_FIELD_SET(rData, data, 8, 0xFF00);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB890, wData)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        wData = REG32_FIELD_SET(rData, data, 0, 0x00FF);
        if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB890, wData)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

int32
phy_rtl826x_patch_process_op(uint32 unit, uint8 port, uint8 portOffset, rtk_phy_hwpatch_t *op, uint8 bcast)
{
    int32  ret = 0;
    uint32 mask = 0, rData = 0, wData = 0;
    if ((op->portmask & (1 << portOffset)) == 0)
    {
        return RT_ERR_ABORT;
    }

    ret = _phy_rtl826x_patch_mask_get(op->msb, op->lsb, &mask);
    if (ret != RT_ERR_OK)
        return ret;

    switch (op->patch_op)
    {
        case RTK_HWPATCH_OP_PHY:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, op->addr, wData)) != RT_ERR_OK)
                return ret;
#if PHY_RTL8264_PATCH_WRITE_AND_CHECK
            if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, op->addr, &rData)) != RT_ERR_OK)
                return ret;
            if ((wData & mask) != (rData & mask))
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "[%s.%d]U%u P%u patch procss failed! wData:0x%04X rData:0x%04X mask:0x%04X\n", __FUNCTION__, __LINE__, unit, port, wData, rData, mask);
                return RT_ERR_NOT_FINISH;
            }
#endif
            break;

        case RTK_HWPATCH_OP_MMD:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, op->pagemmd, op->addr, wData)) != RT_ERR_OK)
                return ret;

#if PHY_RTL8264_PATCH_WRITE_AND_CHECK
            if ((ret = phy_common_general_reg_mmd_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                return ret;
            if ((wData & mask) != (rData & mask))
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "[%s.%d]U%u P%u patch procss failed! wData:0x%04X rData:0x%04X mask:0x%04X\n", __FUNCTION__, __LINE__, unit, port, wData, rData, mask);
                return RT_ERR_NOT_FINISH;
            }
#endif
            break;

        case RTK_HWPATCH_OP_TOP:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = _phy_rtl826x_patch_top_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = _phy_rtl826x_patch_top_set(unit, port, op->pagemmd, op->addr, wData)) != RT_ERR_OK)
                return ret;

#if PHY_RTL8264_PATCH_WRITE_AND_CHECK
            if ((ret = _phy_rtl826x_patch_top_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                return ret;
            if ((wData & mask) != (rData & mask))
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "[%s.%d]U%u P%u patch procss failed! wData:0x%04X rData:0x%04X mask:0x%04X\n", __FUNCTION__, __LINE__, unit, port, wData, rData, mask);
                return RT_ERR_NOT_FINISH;
            }
#endif
            break;

        case RTK_HWPATCH_OP_SDS:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = _phy_rtl826x_patch_sds_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if (bcast == PHY_RTL8264_OP_BCAST)
            {
                if ((ret = _phy_rtl826x_patch_sds_bcast_set(unit, port, op->pagemmd, op->addr, wData)) != RT_ERR_OK)
                    return ret;
            }
            else
            {
                if ((ret = _phy_rtl826x_patch_sds_set(unit, port, op->pagemmd, op->addr, wData)) != RT_ERR_OK)
                    return ret;
            }

#if PHY_RTL8264_PATCH_WRITE_AND_CHECK
            if ((ret = _phy_rtl826x_patch_sds_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                return ret;
            if ((wData & mask) != (rData & mask))
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "[%s.%d]U%u P%u patch procss failed! wData:0x%04X rData:0x%04X mask:0x%04X\n", __FUNCTION__, __LINE__, unit, port, wData, rData, mask);
                return RT_ERR_NOT_FINISH;
            }
#endif
            break;

        case RTK_HWPATCH_OP_PHYW:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, op->addr, wData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_HWPATCH_OP_ALGO:
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87C, op->addr)) != RT_ERR_OK)
                return ret;
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xB87E, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xB87E, wData)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_HWPATCH_OP_DATARAM:
            if ((ret = _phy_rtl826x_patch_data_ram_write(unit, port, op->addr, op->data)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_HWPATCH_OP_UNKNOWN:
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int32
phy_rtl826x_patch_op(uint32 unit, uint8 port, uint8 portOffset, uint8 patch_op, uint8 portmask, uint16 pagemmd, uint16 addr, uint8 msb, uint8 lsb, uint16 data)
{
    rtk_phy_hwpatch_t op;

    op.patch_op = patch_op;
    op.portmask = portmask;
    op.pagemmd  = pagemmd;
    op.addr     = addr;
    op.msb      = msb;
    op.lsb      = lsb;
    op.data     = data;

    return phy_rtl826x_patch_process_op(unit, port, portOffset, &op, PHY_RTL8264_OP_NORMAL);
}

int32 _phy_rtl826x_patch_process(uint32 unit, uint8 port, uint8 portOffset, rtk_phy_hwpatch_t *pPatch, int32 size, uint32 *cnt, uint8 bcast)
{
    int32 i = 0;
    int32 ret = 0;
    int32 n;
    rtk_phy_hwpatch_t *patch = pPatch;
    if (size <= 0)
    {
        *cnt = 0;
        return RT_ERR_OK;
    }
    n = size/sizeof(rtk_phy_hwpatch_t);

    for (i = 0; i < n; i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &patch[i], bcast);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    *cnt = i;
    return RT_ERR_OK;
}

int32 _phy_rtl826x_patch(uint32 unit, uint8 port, uint8 portOffset, uint8 bcast)
{
    int32 ret = RT_ERR_OK;
    uint32 cnt = 0;

    //PP_TOPReg w $PHYID 90 18 ,(0x2d2)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 18, 15, 0, RTL8264_MAIN_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg w $PHYID 90 19 ,(0x2d3)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 19, 15, 0, RTL8264_SW_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg w $PHYID 90 20 ,(0x2d4)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 21, 15, 0, RTL8264_TOP_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg w $PHYID 90 21 ,(0x2d5)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 21, 15, 0, RTL8264_AFEFW_VER)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg w $PHYID 0xB820 0x0010
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 15, 0, 0x0010)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xB800 6 6 wait 1
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, BIT_6, BIT_6, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl826x_patch_process(unit, port, portOffset, rtl8264_patch_fwpr_conf, sizeof(rtl8264_patch_fwpr_conf), &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x fwpr patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x fwpr patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //PP_PHYReg w $PHYID 0xB820 0x0000
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 15, 0, 0x0000)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xB800 6 6  wait 0
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, 0, BIT_6, bcast)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg w $PHYID 0xA46A 0x0302
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA46A, 15, 0, 0x0302)) != RT_ERR_OK)
        return ret;

    //[PP_PHYReg_bit r $PHYID 0xa600 7 0]
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xa600, 0x1, 0xFF, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl826x_patch_process(unit, port, portOffset, rtl8264_patch_fwlm_conf, sizeof(rtl8264_patch_fwlm_conf), &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x fwlm patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x fwlm patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //PP_PHYReg w $::broadcast_PHYID 0xA46A 0x0300
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA46A, 15, 0, 0x0300)) != RT_ERR_OK)
        return ret;

    //[PP_PHYReg_bit r $PHYID 0xa600 7 0]
    if ((ret = _phy_rtl826x_patch_wait_not_equal(unit, port, PHY_MMD_VEND2, 0xa600, 0x1, 0xFF, bcast)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg w $::broadcast_PHYID 0xA436 0x801E
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x801E)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg w $::broadcast_PHYID 0xA438 $currentVersion
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, RTL8264_FW_VER)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl826x_patch_process(unit, port, portOffset, rtl8264_patch_afe_conf, sizeof(rtl8264_patch_afe_conf), &cnt, bcast)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x afe patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x afe patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if ((ret = _phy_rtl826x_patch_process(unit, port, portOffset, rtl8264_patch_top_conf, sizeof(rtl8264_patch_top_conf), &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x top patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x top patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if ((ret = _phy_rtl826x_patch_process(unit, port, portOffset, rtl8264_patch_sds_conf, sizeof(rtl8264_patch_sds_conf), &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x sds patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x sds patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if(HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
    {
        if ((ret = _phy_rtl826x_patch_process(unit, port, portOffset, rtl826X_patch_rtk_conf, sizeof(rtl826X_patch_rtk_conf), &cnt, bcast))!= RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826x rtk patch failed. ret:0x%X\n", unit, port, ret);
            return ret;
        }
    }

    return ret;
}


#if PHY_RTL8264_A_PATCH
int32 _phy_rtl826x_A_patch_sds(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_SDS_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_SDS_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }

    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x sds patch done. cnt:%d\n", unit, port, i);

    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_afe(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_AFE_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_AFE_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x afe patch done. cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_algo1g(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_ALGO1G_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_ALGO1G_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x algo1g patch done. cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_sram(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_SRAM_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_SRAM_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x sram patch done. cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_sram_uc(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_SRAM_UC_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_SRAM_UC_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x sram_uc patch done cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_sram_algo(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_SRAM_ALGO_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_SRAM_ALGO_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x sram_algo patch done cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_data_ram(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_DATARAM_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_DATARAM_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x dataram patch done. cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_sram_start(uint32 unit, uint8 port, uint8 portOffset)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 rcal_p3_efuse = 0, rcal_p2_efuse = 0, rcal_p1_efuse = 0, rcal_p0_efuse = 0;

#if 1
    //PP_TOPReg_bit w $PHYID 90 18 7 0 ,(0x2d2)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 18, 7, 0, RTL8264_A_MAIN_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg_bit w $PHYID 90 19 7 0 ,(0x2d3)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 19, 7, 0, RTL8264_A_SW_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg_bit w $PHYID 90 21 7 0 ,(0x2d5)
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 21, 7, 0, RTL8264_A_SRAM_FW_VER)) != RT_ERR_OK)
        return ret;
#endif

    //PP_PHYReg_bit w $PHYID 0xbd38 12 12 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbd38, 12, 12, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbd38 13 13 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbd38, 13, 13, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbd40 13 2 0xFFF
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbd40, 13, 2, 0xFFF)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbd38 13 13 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbd38, 13, 13, 0x1)) != RT_ERR_OK)
        return ret;

#if 0
    //PP_PHYReg_bit r $PHYID 0xBD3E 4 0
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xBD3E, &rData)) != RT_ERR_OK)
        return ret;
    rData = REG32_FIELD_GET(rData, 0, 0x1F);
    RT_LOG(PHY_826X_LOG, (MOD_HAL|MOD_PHY), "U%u P%u dcocal_code:0x%x\n", unit, port, rData);
#endif

    //PP_PHYReg_bit r $PHYID 0xA420 2 0
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xA420, 0x3, 0x7, PHY_RTL8264_OP_NORMAL)) != RT_ERR_OK)
        return ret;

    //Start
#if PHY_RTL8264_PATCH_SRAM_VER_CHK
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xA436, 0x801E)) != RT_ERR_OK)
        return ret;
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xA438, &rData)) != RT_ERR_OK)
        return ret;

    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u FW ver in sram: %d, in patch: %d\n", unit, port, rData, RTL8264_A_SRAM_FW_VER);
    if (rData == RTL8264_A_SRAM_FW_VER)
    {
        return RT_ERR_ABORT;
    }
#endif

    //PP_PHYReg_bit w $PHYID 0xb820 4 4 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 4, 4, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit r $PHYID 0xB800 6 6
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, BIT_6, BIT_6, PHY_RTL8264_OP_NORMAL)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xa436 15 0 0x8023
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa436, 15, 0, 0x8023)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xa438 15 0 0x6900
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa438, 15, 0, 0x6900)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xa436 15 0 0xB82E
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa436, 15, 0, 0xB82E)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xa438 15 0 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa438, 15, 0, 0x1)) != RT_ERR_OK)
        return ret;

    //AFE patch
    if ((ret = _phy_rtl826x_A_patch_afe(unit, port, portOffset)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x patch afe failed! ret=0x%X\n", unit, port, ret);
        return ret;
    }

    //NCTRL & uC SRAM patch
    //PP_PHYReg_bit w $PHYID 0xb820 7 7 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 7, 7, 0x1)) != RT_ERR_OK)
        return ret;

    //sram
    if ((ret = _phy_rtl826x_A_patch_sram(unit, port, portOffset)) != RT_ERR_OK)
        return ret;

    //sram_patch_end
    //PP_PHYReg_bit w $PHYID 0xb820 7 7 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 7, 7, 0x0)) != RT_ERR_OK)
        return ret;

    //sram_uc
    //PP_PHYReg_bit w $PHYID 0xa466 1 1 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa466, 1, 1, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg w $PHYID 0xA436 0x8586
    //PP_PHYReg w $PHYID 0xA438 0xD00
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x8586)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0xD)) !=  RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xa47e 5 3 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa47e, 5, 3, 0x0)) != RT_ERR_OK)
        return ret;

    //UC
    if ((ret = _phy_rtl826x_A_patch_sram_uc(unit, port, portOffset)) != RT_ERR_OK)
        return ret;

    //data ram
    if ((ret = _phy_rtl826x_A_patch_data_ram(unit, port, portOffset)) != RT_ERR_OK)
        return ret;


    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xB82E 0 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xB82E, 0, 0, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x8023
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x8023)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xB820 4 4 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xB820, 4, 4, 0x0)) != RT_ERR_OK)
        return ret;

    //[PP_PHYReg_bit r $PHYID 0xB800 6 6]
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, 0, BIT_6, PHY_RTL8264_OP_NORMAL)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x801E
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x801E)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 15 0 RTL8264_A_SRAM_FW_VER
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, RTL8264_A_SRAM_FW_VER)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA46A 1 1 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA46A, 1, 1, 0x1)) != RT_ERR_OK)
        return ret;

    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x sram_start patch done.\n", unit, port);

    //ALGO
    if ((ret = _phy_rtl826x_A_patch_sram_algo(unit, port, portOffset)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x patch sram algo failed! ret=0x%X\n", unit, port, ret);
        return ret;
    }

    //ALGO 1G
    if ((ret = _phy_rtl826x_A_patch_algo1g(unit, port, portOffset)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x patch algo1g failed! ret=0x%X\n", unit, port, ret);
        return ret;
    }

    //normal patch
    //PP_SRAM w $PHYID 0x81A9 0x40
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x81A9)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x40)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x81A1 0x68
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x81A1)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x68)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x83F9 0x40
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83F9)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x40)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x83FB 0x40
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83FB)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x40)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x83F5 0x68
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83F5)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x68)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x83F7 0x68
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83F7)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x68)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x83E9 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83E9)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x0)) !=  RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x8015
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x8015)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 15 15 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 15, 0x1)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xD12A 15 0 0x8000
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xD12A, 15, 0, 0x8000)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xD12C 15 0 0x8000
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xD12C, 15, 0, 0x8000)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xD144 15 0 0x8000
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xD144, 15, 0, 0x8000)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xD146 15 0 0x8000
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xD146, 15, 0, 0x8000)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA664 7 0 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA664, 7, 0, 0x1)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA6F8 0 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA6F8, 0, 0, 0x0)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA45C 7 7 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA45C, 7, 7, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA45C 6 6 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA45C, 6, 6, 0x1)) != RT_ERR_OK)
        return ret;

    #if 0
    //PP_SRAM w $PHYID 0x83FA 0x2
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83FA)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x2)) !=  RT_ERR_OK)
        return ret;

    //PP_SRAM w $PHYID 0x83FC 0x2
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x83FC)) !=  RT_ERR_OK)
        return ret;
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 8, 0x2)) !=  RT_ERR_OK)
        return ret;
    #endif

    //PP_PHYReg_bit w $PHYID 0xA5EA 5 5 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA5EA, 5, 5, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xACB6 2 2 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xACB6, 2, 2, 0x1)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA5EA 14 14 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA5EA, 14, 14, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA654 11 11 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA654, 11, 11, 0x1)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xBCE2 15 12
    //PP_PHYReg_bit r $PHYID 0xBCE2 11 8
    //PP_PHYReg_bit r $PHYID 0xBCE2 7 4
    //PP_PHYReg_bit r $PHYID 0xBCE2 3 0
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xBCE2, &rData)) != RT_ERR_OK)
        return ret;
    rcal_p3_efuse = REG32_FIELD_GET(rData, 12, 0xF000);
    rcal_p2_efuse = REG32_FIELD_GET(rData, 8,  0x0F00);
    rcal_p1_efuse = REG32_FIELD_GET(rData, 4,  0x00F0);
    rcal_p0_efuse = REG32_FIELD_GET(rData, 0,  0x000F);

    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x84B4
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x84B4)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 15 0 [rcal_p0_efuse rcal_p1_efuse]
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, ((rcal_p0_efuse << 8) + rcal_p1_efuse))) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x84B6
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x84B6)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 15 0 [rcal_p2_efuse rcal_p3_efuse]
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, ((rcal_p2_efuse << 8) + rcal_p3_efuse))) != RT_ERR_OK)
        return ret;

#if 0//GIGA
    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x8011
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x8011)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 11 11 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 11, 11, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBCE0 15 0 0x9999
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBCE0, 15, 0, 0x9999)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xA46A 1 1 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA46A, 1, 1, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x8010
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x8010)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 11 11 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 11, 11, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA610 13 13 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA610, 13, 13, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA610 6 6 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA610, 6, 6, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA640 12 12 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA640, 12, 12, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA46A 1 1 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA46A, 1, 1, 0x0)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xBD92 15 0 0x2E
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD92, 15, 0, 0x2E)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD94 15 0 0x8002
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD94, 15, 0, 0x8002)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD96 15 0 0xF
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD96, 15, 0, 0xF)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD96 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD96, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD94 15 0 0x8003
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD94, 15, 0, 0x8003)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD96 15 0 0xF
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD96, 15, 0, 0xF)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD96 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD96, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;

    for (i = 0; i < 4; i++)
    {
        //PP_PHYReg_bit w $PHYID 0xBD92 15 0 0x29
        if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD92, 15, 0, 0x29)) != RT_ERR_OK)
            return ret;
        //PP_PHYReg_bit w $PHYID 0xBD94 15 0 0x2200
        if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD94, 15, 0, 0x2200)) != RT_ERR_OK)
            return ret;
        //PP_PHYReg_bit w $PHYID 0xBDA4 15 14 i
        if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBDA4, 15, 14, i)) != RT_ERR_OK)
            return ret;
        //PP_PHYReg_bit w $PHYID 0xBD96 15 0 0xF
        if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD96, 15, 0, 0xF)) != RT_ERR_OK)
            return ret;
        //PP_PHYReg_bit w $PHYID 0xBD96 15 0 0x0
        if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD96, 15, 0, 0x0)) != RT_ERR_OK)
            return ret;
    }

    //PP_PHYReg_bit w $PHYID 0xBCF2 5 2 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBCF2, 5, 2, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBCF2 5 2 0xF
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBCF2, 5, 2, 0xF)) != RT_ERR_OK)
        return ret;

    //adck_ofsk_done
    //PP_PHYReg_bit r $PHYID 0xBC74 11 8
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xBC74, 0x0F00, 0x0F00)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xBCF2 5 2 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBCF2, 5, 2, 0x0)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xBD5C 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD5C, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xBD5C 15 0 0xF
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD5C, 15, 0, 0xF)) != RT_ERR_OK)
        return ret;

    //adck_gaink_done
    //PP_PHYReg_bit r $PHYID 0xBC74 15 12
    if ((ret = _phy_rtl826x_patch_wait(unit, port, PHY_MMD_VEND2, 0xBC74, 0xF000, 0xF000)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xBD5C 15 0 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xBD5C, 15, 0, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA640 12 12 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA640, 12, 12, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA436 15 0 0x8010
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x8010)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xA438 11 11 0x1
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 11, 11, 0x1)) != RT_ERR_OK)
        return ret;
#endif//GIGA

    //PP_PHYReg_bit w $PHYID 0xA46A 1 1 0x0
    if ((ret = phy_rtl826x_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA46A, 1, 1, 0x0)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32 _phy_rtl826x_A_patch_top(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 i = 0;
    int32 ret = 0;

    for (i = 0; i < (sizeof(PHY_RTL8264_A_PATCH_TOP_PATCH) / sizeof(rtk_phy_hwpatch_t)); i++)
    {
        ret = phy_rtl826x_patch_process_op(unit, port, portOffset, &PHY_RTL8264_A_PATCH_TOP_PATCH[i], PHY_RTL8264_OP_NORMAL);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    RT_LOG(PHY_826X_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826x top patch done cnt:%d\n", unit, port, i);
    return RT_ERR_OK;
}

#endif

/* Function Name:
 *      phy_rtl826x_patch
 * Description:
 *      apply initial patch data to PHY
 * Input:
 *      unit       - unit id
 *      baseport   - base port id on the PHY chip
 *      portOffset - the index offset base on baseport for the port to patch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 *      RT_ERR_ABORT
 * Note:
 *      None
 */
int32 phy_rtl826x_patch(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 ret = 0;
    uint32 chipver = 0;

    if((ret = _phy_826x_chip_ver_get(unit, port, &chipver)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x get ver failed! ret=0x%X\n", unit, port, ret);
        return ret;
    }

    switch (chipver)
    {
#if PHY_RTL8264_A_PATCH
        case PHY_RTL8264_VER_A:
            if ((ret = _phy_rtl826x_A_patch_sds(unit, port, portOffset)) != RT_ERR_OK)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x patch sds failed! ret=0x%X\n", unit, port, ret);
                return ret;
            }

            if ((ret = _phy_rtl826x_A_patch_sram_start(unit, port, portOffset)) != RT_ERR_OK)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x patch sram failed! ret=0x%X\n", unit, port, ret);
                return ret;
            }

            if ((ret = _phy_rtl826x_A_patch_top(unit, port, portOffset)) != RT_ERR_OK)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u _phy_rtl826x_patch_top failed! ret=0x%X\n", unit, port, ret);
                return ret;
            }
            break;
#endif
        case PHY_RTL8264_VER_B:
        default:
            ret = _phy_rtl826x_patch(unit, port, portOffset, PHY_RTL8264_OP_NORMAL);
            break;
    }

    return ret;
}

int32 phy_rtl826x_broadcast_patch(uint32 unit, uint8 port, uint8 portOffset)
{
    int32 ret = 0;
    uint32 chipver = 0;

    if((ret = _phy_826x_chip_ver_get(unit, port, &chipver)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x get ver failed! ret=0x%X\n", unit, port, ret);
        return ret;
    }

    switch (chipver)
    {
        case PHY_RTL8264_VER_A:
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u rtl826x ver A not support broadcast!\n", unit, port);
            break;

        case PHY_RTL8264_VER_B:
        default:
            ret = _phy_rtl826x_patch(unit, port, portOffset, PHY_RTL8264_OP_BCAST);
            break;
    }

    return ret;
}

int32 phy_rtl826xI_broadcast_patch(uint32 unit, uint8 port, uint8 portOffset)
{
    return  _phy_rtl826x_patch(unit, port, portOffset, PHY_RTL8264_OP_BCAST);
}

