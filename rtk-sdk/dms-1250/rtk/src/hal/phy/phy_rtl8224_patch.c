/*
 * Copyright (C) 2009-2022 Realtek Semiconductor Corp.
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
 * Purpose : PHY 8224 HW patch APIs.
 *
 * Feature : PHY 8224 HW patch APIs
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
#if defined(CONFIG_SDK_RTL8224)
#include <hal/phy/phy_rtl8224.h>
#include <hal/phy/construct/conf_rtl8224.c>
#endif


/*
 * Symbol Definition
 */
#define PHY_PATCH_OP_NORMAL             0
#define PHY_PATCH_OP_BCAST              1
#define PHY_PATCH_OP_BCAST_SAME_CHIP    2
#define PHY_PATCH_WAIT_TIMEOUT          10000000

#define PHY_PATCH_LOG                   LOG_INFO


/*
 * Data Declaration
 */

rtk_phy_hwpatch_t *patch_fwpr_conf;
rtk_phy_hwpatch_t *patch_fwlm_conf;
rtk_phy_hwpatch_t *patch_afe_conf;
rtk_phy_hwpatch_t *patch_top_conf;
rtk_phy_hwpatch_t *patch_sds_conf;
int32 patch_fwpr_conf_size;
int32 patch_fwlm_conf_size;
int32 patch_afe_conf_size;
int32 patch_top_conf_size;
int32 patch_sds_conf_size;


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

uint32
_phy_rtl8224_patch_mask(uint8 msb, uint8 lsb)
{
    uint32  val = 0;
    uint8   i = 0;

    for (i = lsb; i <= msb; i++)
    {
        val |= (1 << i);
    }
    return val;
}

int32
_phy_rtl8224_patch_mask_get(uint8 msb, uint8 lsb, uint32 *mask)
{
    if ((msb > 15) || (lsb > 15) || (msb < lsb))
    {
        return RT_ERR_FAILED;
    }
    *mask = _phy_rtl8224_patch_mask(msb, lsb);
    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_wait(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data, uint32 mask, uint8 bcast_op)
{
    int32   ret = RT_ERR_OK;
    uint32  rData = 0;
    uint32  cnt = 0;
    WAIT_COMPLETE_VAR()

    rtk_port_t  p = 0;
    uint8  smiBus = HWP_PORT_SMI(unit, port);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(unit, port);
    uint8  bcast_phyad = UNITMAP(unit)->hwp_macID2PortDescp[port]->phy_addr;


    if (bcast_op == PHY_PATCH_OP_BCAST)
    {
        if ((ret = phy_8224_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                WAIT_COMPLETE(PHY_PATCH_WAIT_TIMEOUT)
                {
                    if ((ret = phy_common_general_reg_mmd_get(unit, p, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if ((rData & mask) == data)
                        break;

                }

                if(WAIT_COMPLETE_IS_TIMEOUT())
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        osal_time_mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else if (bcast_op == PHY_PATCH_OP_BCAST_SAME_CHIP)
    {
        if ((ret = phy_8224_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if (HWP_PORT_PHY_IDX(unit, p) == HWP_PORT_PHY_IDX(unit, port))
            {
                WAIT_COMPLETE(PHY_PATCH_WAIT_TIMEOUT)
                {
                    if ((ret = phy_common_general_reg_mmd_get(unit, p, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if ((rData & mask) == data)
                        break;
                }

                if(WAIT_COMPLETE_IS_TIMEOUT())
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        osal_time_mdelay(1);
        //for port in same PHY, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if (HWP_PORT_PHY_IDX(unit, p) == HWP_PORT_PHY_IDX(unit, port))
            {
                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else
    {
        WAIT_COMPLETE(PHY_PATCH_WAIT_TIMEOUT)
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            ++cnt;
            if ((rData & mask) == data)
                break;

            osal_time_mdelay(1);
        }

        if (WAIT_COMPLETE_IS_TIMEOUT())
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, port, mmdAddr, mmdReg, data, mask, rData, cnt);
            return RT_ERR_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_wait_not_equal(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data, uint32 mask, uint8 bcast_op)
{
    int32   ret = RT_ERR_OK;
    uint32  rData = 0;
    uint32  cnt = 0;
    WAIT_COMPLETE_VAR()

    rtk_port_t  p = 0;
    uint8  smiBus = HWP_PORT_SMI(unit, port);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(unit, port);
    uint8  bcast_phyad = UNITMAP(unit)->hwp_macID2PortDescp[port]->phy_addr;

    if (bcast_op == PHY_PATCH_OP_BCAST)
    {
        if ((ret = phy_8224_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
            return ret;
        }


        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                WAIT_COMPLETE(PHY_PATCH_WAIT_TIMEOUT)
                {
                    if ((ret = phy_common_general_reg_mmd_get(unit, p, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if ((rData & mask) != data)
                        break;

                }
                if(WAIT_COMPLETE_IS_TIMEOUT())
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        osal_time_mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(unit, p) == smiBus) && (HWP_PHY_MODEL_BY_PORT(unit, p) == phyChip))
            {
                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else if (bcast_op == PHY_PATCH_OP_BCAST_SAME_CHIP)
    {
        if ((ret = phy_8224_ctrl_set(unit, port, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait disable broadcast failed! 0x%X\n", unit, p, ret);
            return ret;
        }


        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if (HWP_PORT_PHY_IDX(unit, p) == HWP_PORT_PHY_IDX(unit, port))
            {
                WAIT_COMPLETE(PHY_PATCH_WAIT_TIMEOUT)
                {
                    if ((ret = phy_common_general_reg_mmd_get(unit, p, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if (((rData & mask) != data))
                        break;

                }

                if(WAIT_COMPLETE_IS_TIMEOUT())
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, p, mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        osal_time_mdelay(1);
        //for port in same PHY, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if (HWP_PORT_PHY_IDX(unit, p) == HWP_PORT_PHY_IDX(unit, port))
            {
                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait set broadcast PHYAD failed! 0x%X\n", unit, p, ret);
                    return ret;
                }

                if ((ret = phy_8224_ctrl_set(unit, p, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait enable broadcast failed! 0x%X\n", unit, p, ret);
                    return ret;
                }
            }
        }
    }
    else
    {
        WAIT_COMPLETE(PHY_PATCH_WAIT_TIMEOUT)
        {
            if ((ret = phy_common_general_reg_mmd_get(unit, port, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            ++cnt;
            if ((rData & mask) != data)
                break;

            osal_time_mdelay(1);
        }
        if(WAIT_COMPLETE_IS_TIMEOUT())
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", unit, port, mmdAddr, mmdReg, data, mask, rData, cnt);
            return RT_ERR_TIMEOUT;
        }

    }

    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_top_get(uint32 unit, rtk_port_t port, uint32 topPage, uint32 topReg, uint32 *pData)
{
    int32   ret = RT_ERR_OK;
    uint32  rData = 0;

    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, topReg, &rData)) != RT_ERR_OK)
        return ret;
    *pData = rData;

    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_top_set(uint32 unit, rtk_port_t port, uint32 topPage, uint32 topReg, uint32 wData)
{
    int32   ret = RT_ERR_OK;

    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, topReg, wData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_sds_get(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    int32   ret = RT_ERR_OK;
    uint32  rData = 0;
    uint32  sdsAddr = (0x1 << 15) | (0x0 << 14) | (sdsReg << 7) | (sdsPage << 1);

    if ((ret = _phy_rtl8224_patch_top_set(unit, port, 0, 0x3F8, sdsAddr)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl8224_patch_top_get(unit, port, 0, 0x3FC, &rData)) != RT_ERR_OK)
        return ret;
    *pData = rData;

    return _phy_rtl8224_patch_wait(unit, port, PHY_MMD_VEND1, 0x3F8, 0, BIT_15, PHY_PATCH_OP_NORMAL);
}

int32
_phy_rtl8224_patch_sds_set(uint32 unit, rtk_port_t port, uint32 sdsPage, uint32 sdsReg, uint32 wData, uint8 bcast)
{
    int32   ret = RT_ERR_OK;
    uint32  sdsAddr = (0x1 << 15) | (0x1 << 14) | (sdsReg << 7) | (sdsPage << 1);

    if ((ret = _phy_rtl8224_patch_top_set(unit, port, 0, 0x400, wData)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl8224_patch_top_set(unit, port, 0, 0x3F8, sdsAddr)) != RT_ERR_OK)
        return ret;

    return _phy_rtl8224_patch_wait(unit, port, PHY_MMD_VEND1, 0x3F8, 0, BIT_15, bcast);
}

int32
phy_rtl8224_patch_process_op(uint32 unit, uint8 port, uint8 portOffset, rtk_phy_hwpatch_t *op, uint8 bcast)
{
    int32   ret = RT_ERR_OK;
    uint32  mask = 0;
    uint32  rData = 0;
    uint32  wData = 0;

    if ((op->portmask & (1 << portOffset)) == 0)
    {
        return RT_ERR_ABORT;
    }

    ret = _phy_rtl8224_patch_mask_get(op->msb, op->lsb, &mask);

    if (ret != RT_ERR_OK)
        return ret;

    switch (op->patch_op)
    {
        case RTK_HWPATCH_OP_PHY:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, op->addr, &rData)) != RT_ERR_OK)
                {
                    osal_printf("\n[%s][port = %d] addr = 0x%08x ERROR!!!!!\n",__FUNCTION__,port, op->addr);
                    return ret;
                }
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, op->addr, wData)) != RT_ERR_OK)
                return ret;

            break;

        case RTK_HWPATCH_OP_MMD:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND1, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);
            if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND1, op->addr, wData)) != RT_ERR_OK)
                return ret;

            break;

        case RTK_HWPATCH_OP_TOP:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = _phy_rtl8224_patch_top_get(unit, port, PHY_MMD_VEND1, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);

            if ((ret = _phy_rtl8224_patch_top_set(unit, port, PHY_MMD_VEND1, op->addr, wData)) != RT_ERR_OK)
                return ret;

            break;

        case RTK_HWPATCH_OP_SDS:
            if ((op->msb != 15) || (op->lsb != 0))
            {
                if ((ret = _phy_rtl8224_patch_sds_get(unit, port, op->pagemmd, op->addr, &rData)) != RT_ERR_OK)
                    return ret;
            }
            wData = REG32_FIELD_SET(rData, op->data, op->lsb, mask);

            if ((ret = _phy_rtl8224_patch_sds_set(unit, port, op->pagemmd, op->addr, wData, bcast)) != RT_ERR_OK)
                return ret;

            break;

        case RTK_HWPATCH_OP_PHYW:
        case RTK_HWPATCH_OP_ALGO:
        case RTK_HWPATCH_OP_DATARAM:
        case RTK_HWPATCH_OP_UNKNOWN:
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

int32
phy_rtl8224_patch_op(uint32 unit, uint8 port, uint8 portOffset, uint8 patch_op, uint8 portmask, uint16 pagemmd, uint16 addr, uint8 msb, uint8 lsb, uint16 data)
{
    rtk_phy_hwpatch_t op;

    op.patch_op = patch_op;
    op.portmask = portmask;
    op.pagemmd  = pagemmd;
    op.addr     = addr;
    op.msb      = msb;
    op.lsb      = lsb;
    op.data     = data;

    return phy_rtl8224_patch_process_op(unit, port, portOffset, &op, PHY_PATCH_OP_NORMAL);
}

int32
_phy_rtl8224_patch_process(uint32 unit, uint8 port, uint8 portOffset, rtk_phy_hwpatch_t *pPatch, int32 size, uint32 *cnt, uint8 bcast)
{
    int32               ret = RT_ERR_OK;
    int32               i = 0;
    int32               n = 0;
    rtk_phy_hwpatch_t   *patch = pPatch;

    if (size <= 0)
    {
        *cnt = 0;
        return RT_ERR_OK;
    }
    n = size/sizeof(rtk_phy_hwpatch_t);

    for (i = 0; i < n; i++)
    {
        ret = phy_rtl8224_patch_process_op(unit, port, portOffset, &patch[i], bcast);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u %s failed! i=%u ret=0x%X\n", unit, port, __FUNCTION__, i, ret);
            return ret;
        }
    }
    *cnt = i;

    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_assign(uint32 unit, uint8 port, uint32 chipVer)
{
    if(chipVer == PHY_RTL8224_VER_A)
    {
        patch_fwpr_conf = rtl8224_a_patch_fwpr_conf;
        patch_fwlm_conf = rtl8224_a_patch_fwlm_conf;
        patch_afe_conf = rtl8224_a_patch_afe_conf;
        patch_top_conf = rtl8224_a_patch_top_conf;
        patch_sds_conf = rtl8224_a_patch_sds_conf;
        patch_fwpr_conf_size = sizeof(rtl8224_a_patch_fwpr_conf);
        patch_fwlm_conf_size= sizeof(rtl8224_a_patch_fwlm_conf);
        patch_afe_conf_size= sizeof(rtl8224_a_patch_afe_conf);
        patch_top_conf_size= sizeof(rtl8224_a_patch_top_conf);
        patch_sds_conf_size= sizeof(rtl8224_a_patch_sds_conf);
    }
    else if(chipVer == PHY_RTL8224_VER_B)
    {
        patch_fwpr_conf = rtl8224_b_patch_fwpr_conf;
        patch_fwlm_conf = rtl8224_b_patch_fwlm_conf;
        patch_afe_conf = rtl8224_b_patch_afe_conf;
        patch_top_conf = rtl8224_b_patch_top_conf;
        patch_sds_conf = rtl8224_b_patch_sds_conf;
        patch_fwpr_conf_size = sizeof(rtl8224_b_patch_fwpr_conf);
        patch_fwlm_conf_size= sizeof(rtl8224_b_patch_fwlm_conf);
        patch_afe_conf_size= sizeof(rtl8224_b_patch_afe_conf);
        patch_top_conf_size= sizeof(rtl8224_b_patch_top_conf);
        patch_sds_conf_size= sizeof(rtl8224_b_patch_sds_conf);
    }else{
        patch_fwpr_conf = rtl8224_patch_fwpr_conf;
        patch_fwlm_conf = rtl8224_patch_fwlm_conf;
        patch_afe_conf = rtl8224_patch_afe_conf;
        patch_top_conf = rtl8224_patch_top_conf;
        patch_sds_conf = rtl8224_patch_sds_conf;
        patch_fwpr_conf_size = sizeof(rtl8224_patch_fwpr_conf);
        patch_fwlm_conf_size= sizeof(rtl8224_patch_fwlm_conf);
        patch_afe_conf_size= sizeof(rtl8224_patch_afe_conf);
        patch_top_conf_size= sizeof(rtl8224_patch_top_conf);
        patch_sds_conf_size= sizeof(rtl8224_patch_sds_conf);
    }
    return RT_ERR_OK;
}

int32
_phy_rtl8224_patch_version_assign(uint32 unit, uint8 port, uint8 portOffset, uint32 chipVer)
{
    int32   ret = RT_ERR_OK;
    //writephy_ocp $phynum 0xA438 $currentVersion
    if((chipVer == PHY_RTL8224_VER_A) || (chipVer == PHY_RTL8224_VER_B))
    {
        ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, RTL8224_AB_FW_VER);
    }
    else
    {
        ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA438, 15, 0, RTL8224_FW_VER);
    }
    return ret;
}


int32
_phy_rtl8224_patch(uint32 unit, uint8 port, uint8 portOffset, uint8 bcast)
{
    int32   ret = RT_ERR_OK;
    uint32  cnt = 0;
    uint32  chipVer = 5;

    phy_8224_chipVer_get(unit, port, &chipVer);

    _phy_rtl8224_patch_assign(unit, port, chipVer);
    //Patch Request
    //PP_PHYReg w $PHYID 0xB820 bit.4 0x1 patch request
    if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 4, 4, 0x1)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xB800 6 6 wait 1
    if ((ret = _phy_rtl8224_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, BIT_6, BIT_6, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl8224_patch_process(unit, port, portOffset, patch_fwpr_conf, patch_fwpr_conf_size, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 fwpr patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 8224 fwpr patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //PP_PHYReg w $PHYID 0xB820 0x0000
    if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xb820, 4, 4, 0x0)) != RT_ERR_OK)
        return ret;

    //PP_PHYReg_bit r $PHYID 0xB800 6 6  wait 0 (Release patch request & wait patch_rdy = 0)
    if ((ret = _phy_rtl8224_patch_wait(unit, port, PHY_MMD_VEND2, 0xB800, 0, BIT_6, bcast)) != RT_ERR_OK)
        return ret;

    //Lock Main
    //writephybits_ocp $phynum 0xa4a0 10 10 0x1
    if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA4A0, 10, 10, 0x1)) != RT_ERR_OK)
        return ret;
    //[PP_PHYReg_bit r $PHYID 0xa600 7 0]
    if ((ret = _phy_rtl8224_patch_wait(unit, port, PHY_MMD_VEND2, 0xa600, 0x1, 0xFF, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl8224_patch_process(unit, port, portOffset, patch_fwlm_conf, patch_fwlm_conf_size, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 fwlm patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 8224 fwlm patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if ((ret = _phy_rtl8224_patch_process(unit, port, portOffset, patch_afe_conf, patch_afe_conf_size, &cnt, bcast)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 afe patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 8224 afe patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //Patch PHY
    if(chipVer == PHY_RTL8224_VER_A){
        //writephy_ocp $phynum 0xa5d0 0x0
        if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa5d0, 15, 0, 0x0)) != RT_ERR_OK)
            return ret;

        //writephybits_ocp $phynum 0xa430 2 0 0x0
        if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa430, 2, 0, 0x0)) != RT_ERR_OK)
            return ret;
    }

    //writephy_ocp $phynum 0xa47E 7 6 0x1
    if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xa47E, 7, 6, 0x1)) != RT_ERR_OK)
        return ret;

    //Release Lock Main
    //writephybits_ocp $phynum 0xa4a0 10 10 0x0
    if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA4A0, 10, 10, 0x0)) != RT_ERR_OK)
        return ret;

    //[PP_PHYReg_bit r $PHYID 0xa600 7 0]
    if ((ret = _phy_rtl8224_patch_wait_not_equal(unit, port, PHY_MMD_VEND2, 0xa600, 0x1, 0xFF, bcast)) != RT_ERR_OK)
        return ret;

    if ((ret = _phy_rtl8224_patch_process(unit, port, portOffset, patch_top_conf, patch_top_conf_size, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 top patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 8224 top patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    if ((ret = _phy_rtl8224_patch_process(unit, port, portOffset, patch_sds_conf, patch_sds_conf_size, &cnt, bcast))!= RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, (MOD_HAL | MOD_PHY), "U%u P%u 8224 sds patch failed. ret:0x%X\n", unit, port, ret);
        return ret;
    }
    RT_LOG(PHY_PATCH_LOG, (MOD_HAL | MOD_PHY), "U%u P%u 8224 sds patch done. ret:0x%X cnt:%d\n", unit, port, ret, cnt);

    //writephy_ocp $phynum 0xA436 0x801E
    if ((ret = phy_rtl8224_patch_op(unit, port, portOffset, RTK_HWPATCH_OP_PHY, 0xff, 0x00, 0xA436, 15, 0, 0x801E)) != RT_ERR_OK)
        return ret;

    ret = _phy_rtl8224_patch_version_assign(unit, port, portOffset, chipVer);

    return ret;
}

/* Function Name:
 *      phy_rtl8224_sdsReg_get
 * Description:
 *      Get SerDes Register
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      sdsPage     - the SerDes page
 *      sdsReg      - the SerDes register
 * Output:
 *      pData       - return register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_SUPPORTED
 *      RT_ERR_ABORT
 * Note:
 *      None
 */
int32
phy_rtl8224_sdsReg_get(uint32 unit, rtk_port_t baseport, uint32 sdsPage, uint32 sdsReg, uint32 *pData)
{
    _phy_rtl8224_patch_sds_get(unit, baseport, sdsPage, sdsReg, pData);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_rtl8224_sdsReg_set
 * Description:
 *      Set SerDes Register
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      sdsPage     - the SerDes page
 *      sdsReg      - the SerDes register
 *      wData       - the write data
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

int32
phy_rtl8224_sdsReg_set(uint32 unit, rtk_port_t baseport, uint32 sdsPage, uint32 sdsReg, uint32 wData)
{
    return _phy_rtl8224_patch_sds_set(unit, baseport, sdsPage, sdsReg, wData, 0);
}


/* Function Name:
 *      phy_rtl8224_patch
 * Description:
 *      Apply initial patch data to PHY
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      portOffset  - the index offset base on baseport for the port to patch
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
int32
phy_rtl8224_patch(uint32 unit, uint8 port, uint8 portOffset)
{
    int32   ret = RT_ERR_OK;
    uint32  phyData = 0;

    if ((ret = _phy_rtl8224_patch(unit, port, portOffset, PHY_PATCH_OP_NORMAL)) != RT_ERR_OK)
        return ret;

    /*Disable 100M/1G EEE advertise ability */
    phyData = 0;
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xa5d0, phyData)) != RT_ERR_OK)
        return ret;

    /*Disable 1G lite ability */
    if ((ret = phy_common_general_reg_mmd_get(unit, port, PHY_MMD_VEND2, 0xa428, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(BIT_9);
    if ((ret = phy_common_general_reg_mmd_set(unit, port, PHY_MMD_VEND2, 0xa428, phyData)) != RT_ERR_OK)
        return ret;

    if(!(HWP_9300_FAMILY_ID(unit) || HWP_9310_FAMILY_ID(unit)))
    {
        if ((ret = phy_8224_sdsOpCode_set(unit, port, 0x3)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_8224_sdsAmPeriod_set(unit, port, 0xa4)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = phy_8224_sdsOpCode_set(unit, port, 0xaa)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_8224_sdsAmPeriod_set(unit, port, 0x5078)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_8224_sdsAllAm_set(unit, port, 0x0)) != RT_ERR_OK)
            return ret;
        if ((ret = phy_8224_sdsExtra_set(unit, port)) != RT_ERR_OK)
            return ret;
    }
    return ret;
}

/* Function Name:
 *      phy_rtl8224_broadcast_patch
 * Description:
 *      Apply patch data to PHY
 * Input:
 *      unit        - unit id
 *      baseport    - base port id on the PHY chip
 *      portOffset  - the index offset base on baseport for the port to patch
 *      perChip     - 1 for per-chip mode, 0 for per-bus mode
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
int32
phy_rtl8224_broadcast_patch(uint32 unit, uint8 port, uint8 portOffset, uint8 perChip)
{
    int32   ret = RT_ERR_OK;

    if (0 == perChip)
    {
        ret = _phy_rtl8224_patch(unit, port, portOffset, PHY_PATCH_OP_BCAST);
    }
    else
    {
        ret = _phy_rtl8224_patch(unit, port, portOffset, PHY_PATCH_OP_BCAST_SAME_CHIP);
    }

    return ret;
}

