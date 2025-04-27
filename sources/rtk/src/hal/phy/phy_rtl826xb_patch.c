/*
 * Copyright (C) 2009-2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : PHY 826XB/8261N Driver APIs.
 *
 * Feature : PHY 826XB/8261N Driver APIs
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
#include <hal/phy/phy_construct.h>
#include <osal/time.h>
#include <hal/phy/construct/conftypes.h>
#if defined(CONFIG_SDK_RTL826XB)
#include <hal/phy/phy_rtl826xb.h>
#include <hal/phy/construct/conf_rtl8264b.c>
#include <hal/phy/construct/conf_rtl8261n_c.c>
#endif
/*
 * Symbol Definition
 */

#define PHY_PATCH_OP_NORMAL 0
#define PHY_PATCH_OP_BCAST  1
#define PHY_PATCH_WAIT_TIMEOUT     1000000

#define PHY_PATCH_LOG    LOG_INFO

/*
 * Data Declaration
 */
rtk_phy_hwpatch_t rtl826XB_patch_rtk_conf[] = {
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
_phy_rtl826xb_patch_mask(uint8 msb, uint8 lsb)
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
_phy_rtl826xb_patch_mask_get(uint8 msb, uint8 lsb, uint32 *mask)
{
    if ((msb > 15) || (lsb > 15) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }
    *mask = _phy_rtl826xb_patch_mask(msb, lsb);
    return RT_ERR_OK;
}

int32
_phy_rtl826xb_patch_wait(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data, uint32 mask, uint8 bcast_op)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 cnt = 0;

    rtk_port_t  p = 0;
    uint8  smiBus = HWP_PORT_SMI(unit, port);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(unit, port);
    uint8  bcast_phyad = UNITMAP(unit)->hwp_macID2PortDescp[port]->phy_addr;


    if (bcast_op == PHY_PATCH_OP_BCAST)
    {
        if ((ret = phy_826xb_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
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

                    if (++cnt >= PHY_PATCH_WAIT_TIMEOUT)
                    {
                        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                        return RT_ERR_TIMEOUT;
                    }
                }while ((rData & mask) != data);
            }
        }

        osal_time_mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                if ((ret = phy_826xb_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_826xb_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else
    {
        do
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            if (++cnt >= PHY_PATCH_WAIT_TIMEOUT)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, port, mmdAddr, mmdReg, data, mask, rData, cnt);
                return RT_ERR_TIMEOUT;
            }
            osal_time_mdelay(1);
        }while ((rData & mask) != data);
    }

    return RT_ERR_OK;
}

int32
_phy_rtl826xb_patch_wait_not_equal(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data, uint32 mask, uint8 bcast_op)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 cnt = 0;

    rtk_port_t  p = 0;
    uint8  smiBus = HWP_PORT_SMI(unit, port);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(unit, port);
    uint8  bcast_phyad = UNITMAP(unit)->hwp_macID2PortDescp[port]->phy_addr;

    if (bcast_op == PHY_PATCH_OP_BCAST)
    {
        if ((ret = phy_826xb_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
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

                    if (++cnt >= PHY_PATCH_WAIT_TIMEOUT)
                    {
                        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                        return RT_ERR_TIMEOUT;
                    }
                }while ((rData & mask) == data);
            }
        }

        osal_time_mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                if ((ret = phy_826xb_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_826xb_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else
    {
        do
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            if (++cnt >= PHY_PATCH_WAIT_TIMEOUT)
            {
                RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, port, mmdAddr, mmdReg, data, mask, rData, cnt);
                return RT_ERR_TIMEOUT;
            }
            osal_time_mdelay(1);
        }while ((rData & mask) == data);
    }

    return RT_ERR_OK;
}

int32
_phy_rtl826xb_patch_top_get(uint32 unit, rtk_port_t port, uint32 topPage, uint32 topReg, uint32 *pData)
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
_phy_rtl826xb_patch_top_set(uint32 unit, rtk_port_t port, uint32 topPage, uint32 topReg, uint32 wData)
{
    int32  ret = 0;
    uint32 topAddr = (topPage * 8) + (topReg - 16);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, topAddr, wData)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
}

int32
_phy_rtl826xb_patch_sds_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 sdsAddr = 0x8000 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826xb_patch_top_set(unit, port, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826xb_patch_top_get(unit, port, 40, 18, &rData)) != RT_ERR_OK)
        return ret;
    *pData = rData;
    return _phy_rtl826xb_patch_wait(unit, port, PHY_MMD_VEND1, 0x143, 0, BIT_15, PHY_PATCH_OP_NORMAL);
}

int32
_phy_rtl826xb_patch_sds_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 wData, uint8 bcast)
{
    int32  ret = 0;
    uint32 sdsAddr = 0x8800 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826xb_patch_top_set(unit, port, 40, 17, wData)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826xb_patch_top_set(unit, port, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    return _phy_rtl826xb_patch_wait(unit, port, PHY_MMD_VEND1, 0x143, 0, BIT_15, bcast);
}

int32 _phy_rtl826xb_patch_data_ram_write(uint32 unit, uint8 port, uint32 addr, uint32 data)
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
phy_rtl826xb_patch_process_op(uint32 unit, uint8 port, uint8 portOffset, rtk_phy_hwpatch_t *op, uint8 bcast)
{
    int32  ret = 0;
    uint32 mask = 0, rData = 0, wData = 0;
    if ((op->portmask & (1 << portOffset)) == 0)
    {
        return RT_ERR_ABORT;
    }

    ret = _phy_rtl826xb_patch_mask_get(op->msb, op->lsb, &mask);
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

            break;

        case RTK_HWPATCH_OP_TOP:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = _phy_rtl826xb_patch_top_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = _phy_rtl826xb_patch_top_set(unit, port, op->pagemmd, op->addr, wData)) != RT_ERR_OK)
                return ret;

            break;

        case RTK_HWPATCH_OP_SDS:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = _phy_rtl826xb_patch_sds_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);

            if ((ret = _phy_rtl826xb_patch_sds_set(unit, port, op->pagemmd, op->addr, wData, bcast)) != RT_ERR_OK)
                return ret;

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
            if ((ret = _phy_rtl826xb_patch_data_ram_write(unit, port, op->addr, op->data)) != RT_ERR_OK)
                return ret;
            break;

        case RTK_HWPATCH_OP_UNKNOWN:
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int32
phy_rtl826xb_patch_op(uint32 unit, uint8 port, uint8 portOffset, uint8 patch_op, uint8 portmask, uint16 pagemmd, uint16 addr, uint8 msb, uint8 lsb, uint16 data)
{
    rtk_phy_hwpatch_t op;

    op.patch_op = patch_op;
    op.portmask = portmask;
    op.pagemmd  = pagemmd;
    op.addr     = addr;
    op.msb      = msb;
    op.lsb      = lsb;
    op.data     = data;

    return phy_rtl826xb_patch_process_op(unit, port, portOffset, &op, PHY_PATCH_OP_NORMAL);
}

int32 _phy_rtl826xb_patch_process(uint32 unit, uint8 port, uint8 portOffset, rtk_phy_hwpatch_t *pPatch, int32 size, uint32 *cnt, uint8 bcast)
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
        ret = phy_rtl826xb_patch_process_op(unit, port, portOffset, &patch[i], bcast);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    *cnt = i;
    return RT_ERR_OK;
}

int32 _phy_rtl826xb_patch(uint32 unit, uint8 port, uint8 portOffset, uint8 bcast)
{
    int32 ret = RT_ERR_OK;
    uint32 cnt = 0;
    uint32 rData = 0;

    uint32 FW_VER    = 0;
    uint32 MAIN_VER  = 0;
    uint32 SW_VER    = 0;
    uint32 TOP_VER   = 0;
    uint32 AFEFW_VER = 0;
    rtk_phy_hwpatch_t *patch_fwpr_conf;
    rtk_phy_hwpatch_t *patch_fwlm_conf;
    rtk_phy_hwpatch_t *patch_afe_conf;
    rtk_phy_hwpatch_t *patch_sds_conf;
    rtk_phy_hwpatch_t *patch_top_conf;
    uint32 size_fwpr_conf = 0;
    uint32 size_fwlm_conf = 0;
    uint32 size_afe_conf  = 0;
    uint32 size_sds_conf  = 0;
    uint32 size_top_conf  = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, 30, 0x104, &rData)) != RT_ERR_OK)
        return ret;

    if ((rData & 0x7) == 0x0)
    {
        FW_VER    = RTL8264B_FW_VER;
        MAIN_VER  = RTL8264B_MAIN_VER;
        SW_VER    = RTL8264B_SW_VER;
        TOP_VER   = RTL8264B_TOP_VER;
        AFEFW_VER = RTL8264B_AFEFW_VER;
        patch_fwpr_conf = rtl8264B_patch_fwpr_conf;
        patch_fwlm_conf = rtl8264B_patch_fwlm_conf;
        patch_afe_conf  = rtl8264B_patch_afe_conf;
        patch_sds_conf  = rtl8264B_patch_sds_conf;
        patch_top_conf  = rtl8264B_patch_top_conf;
        size_fwpr_conf = sizeof(rtl8264B_patch_fwpr_conf);
        size_fwlm_conf = sizeof(rtl8264B_patch_fwlm_conf);
        size_afe_conf  = sizeof(rtl8264B_patch_afe_conf);
        size_sds_conf  = sizeof(rtl8264B_patch_sds_conf);
        size_top_conf  = sizeof(rtl8264B_patch_top_conf);
    }
    else
    {
        FW_VER    = RTL8261N_C_FW_VER;
        MAIN_VER  = RTL8261N_C_MAIN_VER;
        SW_VER    = RTL8261N_C_SW_VER;
        TOP_VER   = RTL8261N_C_TOP_VER;
        AFEFW_VER = RTL8261N_C_AFEFW_VER;
        patch_fwpr_conf = rtl8261N_C_patch_fwpr_conf;
        patch_fwlm_conf = rtl8261N_C_patch_fwlm_conf;
        patch_afe_conf  = rtl8261N_C_patch_afe_conf;
        patch_sds_conf  = rtl8261N_C_patch_sds_conf;
        patch_top_conf  = rtl8261N_C_patch_top_conf;
        size_fwpr_conf = sizeof(rtl8261N_C_patch_fwpr_conf);
        size_fwlm_conf = sizeof(rtl8261N_C_patch_fwlm_conf);
        size_afe_conf  = sizeof(rtl8261N_C_patch_afe_conf);
        size_sds_conf  = sizeof(rtl8261N_C_patch_sds_conf);
        size_top_conf  = sizeof(rtl8261N_C_patch_top_conf);

    }

    //PP_TOPReg w $PHYID 90 18 ,(0x2d2)
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 18, 15, 0, MAIN_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg w $PHYID 90 19 ,(0x2d3)
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 19, 15, 0, SW_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg w $PHYID 90 20 ,(0x2d4)
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 21, 15, 0, TOP_VER)) != RT_ERR_OK)
        return ret;
    //PP_TOPReg w $PHYID 90 21 ,(0x2d5)
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_TOP, 0xff, 90, 21, 15, 0, AFEFW_VER)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg w $PHYID 0xB820 0x0010
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 15, 0, 0x0010)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xB800 6 6 wait 1
    if ((ret = _phy_rtl826xb_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, BIT_6, BIT_6, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl826xb_patch_process(unit, port, portOffset, patch_fwpr_conf, size_fwpr_conf, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB fwpr patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826XB fwpr patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //PP_PHYReg w $PHYID 0xB820 0x0000
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 15, 0, 0x0000)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xB800 6 6  wait 0
    if ((ret = _phy_rtl826xb_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, 0, BIT_6, bcast)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $::broadcast_PHYID 0xA4a0 10 10 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA4A0, 10, 10, 0x1)) != RT_ERR_OK)
        return ret;

    //[PP_PHYReg_bit r $PHYID 0xa600 7 0]
    if ((ret = _phy_rtl826xb_patch_wait(unit, port, PHY_MMD_VEND2, 0xa600, 0x1, 0xFF, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl826xb_patch_process(unit, port, portOffset, patch_fwlm_conf, size_fwlm_conf, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB fwlm patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826XB fwlm patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //xg_patch_en_flag
    //PP_PHYReg_bit w $PHYID 0xbf86 9 9 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 9, 9, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 8 8 0x0
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 8, 8, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 7 7 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 7, 7, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 6 6 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 6, 6, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 5 5 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 5, 5, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 4 4 0x1;
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 4, 4, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 6 6 0x0
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 6, 6, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 9 9 0x0
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 9, 9, 0x0)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 7 7 0x0
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 7, 7, 0x0)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xbc62 12 8
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xbc62, &rData)) != RT_ERR_OK)
        return ret;
    rData = REG32_FIELD_GET(rData, 8, 0x1F00);
    for (cnt = 0; cnt <= rData; cnt++)
    {
        //PP_PHYReg_bit w $PHYID 0xbc62 12 8 $t
        if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbc62, 12, 8, cnt)) != RT_ERR_OK)
        return ret;
    }

    //PP_PHYReg_bit w $PHYID 0xbc02 2 2 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbc02, 2, 2, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbc02 3 3 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbc02, 3, 3, 0x1)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit w $PHYID 0xbf86 6 6 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 6, 6, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 9 9 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 9, 9, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbf86 7 7 0x1
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbf86, 7, 7, 0x1)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg_bit w $PHYID 0xbc04 9 2 0xff
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xbc04, 9, 2, 0xff)) != RT_ERR_OK)
        return ret;


    //PP_PHYReg w $::broadcast_PHYID 0xA4a0 0x0180
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA4A0, 15, 0, 0x0180)) != RT_ERR_OK)
        return ret;

    //[PP_PHYReg_bit r $PHYID 0xa600 7 0]
    if ((ret = _phy_rtl826xb_patch_wait_not_equal(unit, port, PHY_MMD_VEND2, 0xa600, 0x1, 0xFF, bcast)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg w $::broadcast_PHYID 0xA436 0x801E
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x801E)) != RT_ERR_OK)
        return ret;
    //PP_PHYReg w $::broadcast_PHYID 0xA438 $currentVersion
    if ((ret = phy_rtl826xb_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, FW_VER)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl826xb_patch_process(unit, port, portOffset, patch_afe_conf, size_afe_conf, &cnt, bcast)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB afe patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826XB afe patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if ((ret = _phy_rtl826xb_patch_process(unit, port, portOffset, patch_top_conf, size_top_conf, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB top patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826XB top patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if ((ret = _phy_rtl826xb_patch_process(unit, port, portOffset, patch_sds_conf, size_sds_conf, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB sds patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 826XB sds patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if(HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit))
    {
        if ((ret = _phy_rtl826xb_patch_process(unit, port, portOffset, rtl826XB_patch_rtk_conf, sizeof(rtl826XB_patch_rtk_conf), &cnt, bcast))!= RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 826XB rtk patch failed. ret:0x%X\n", unit, port, ret);
            return ret;
        }
    }

    return ret;
}

/* Function Name:
 *      phy_rtl826xb_patch
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
int32 phy_rtl826xb_patch(uint32 unit, uint8 port, uint8 portOffset)
{
    return _phy_rtl826xb_patch(unit, port, portOffset, PHY_PATCH_OP_NORMAL);
}

/* Function Name:
 *      phy_rtl826xb_broadcast_patch
 * Description:
 *      apply patch data to PHY
 * Input:
 *      unit       - unit id
 *      baseport   - base port id on the PHY chip
 *      portOffset - the index offset base on baseport for the port to patch
 *      perChip    - 1 for per-chip mode, 0 for per-bus mode
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
int32 phy_rtl826xb_broadcast_patch(uint32 unit, uint8 port, uint8 portOffset)
{
    return _phy_rtl826xb_patch(unit, port, portOffset, PHY_PATCH_OP_BCAST);
}

