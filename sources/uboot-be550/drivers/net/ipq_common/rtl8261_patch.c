#include <asm/io.h>
#include <malloc.h>
#include <phy.h>
#include "rtl8261_patch.h"
#include "ipq_phy.h"
#include "rtl8261_error.h"
#include "construct/conf_rtl8264b.c"
#include "construct/conf_rtl8261n_c.c"

u8 phy_patch_op_translate(u8 patch_mode, u8 patch_op, u8 compare_op)
{
    if (patch_mode != PHY_PATCH_MODE_CMP)
    {    
        return patch_op;
    }    
    else 
    {
        switch (compare_op)
        {
            case RTK_PATCH_CMP_WS:
                return RTK_PATCH_OP_SKIP;
            case RTK_PATCH_CMP_W:
            case RTK_PATCH_CMP_WC:
            case RTK_PATCH_CMP_SWC:
            default:
                return RTK_PATCH_OP_TO_CMP(patch_op, compare_op);
        }
    }
}

static uint16 _phy_rtl826xb_mmd_convert(uint16 page, uint16 addr)
{
    uint16 reg = 0;
    if (addr < 16)
    {
        reg = 0xA400 + (page * 2);
    }
    else if (addr < 24)
    {
        reg = (16*page) + ((addr - 16) * 2);
    }
    else
    {
        reg = 0xA430 + ((addr - 24) * 2);
    }
    return reg;
}

int32_t _phy_rtl826xb_patch_top_get(u32 phy_id, u32 topPage, u32 topReg, u32 *pData)
{
    int32_t  ret = 0; 
    u32 rData = 0; 
    u32 topAddr = (topPage * 8) + (topReg - 16); 

    if ((ret = phy_common_general_reg_mmd_get(phy_id, PHY_MMD_VEND1, topAddr, &rData)) != RT_ERR_OK)
        return ret; 
    *pData = rData;
    return RT_ERR_OK;
}

int32_t _phy_rtl826xb_patch_top_set(u32 phy_id, u32 topPage, u32 topReg, u32 wData)
{
    int32_t  ret = 0; 
    u32 topAddr = (topPage * 8) + (topReg - 16); 
    if ((ret = phy_common_general_reg_mmd_set(phy_id, PHY_MMD_VEND1, topAddr, wData)) != RT_ERR_OK)
        return ret; 
    return RT_ERR_OK;
}

int32_t _phy_rtl826xb_patch_wait(u32 phy_id, u32 mmdAddr, u32 mmdReg, u32 data, u32 mask, u8 patch_mode)
{
    int32_t  ret = 0;
    u32 rData = 0;
    u32 cnt = 0;
    int32_t count = 1000;
    int32_t i = 0;

    void*  p = NULL;
    uint8  smiBus = HWP_PORT_SMI(phy_id);
    uint32 phyChip = HWP_PHY_MODEL_BY_PORT(phy_id);
    uint8  bcast_phyad = HWP_PHY_ADDR(phy_id);

    if (patch_mode == PHY_PATCH_MODE_BCAST_BUS)
    {
        if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            printf("826XB patch wait disable broadcast failed! 0x%X\n", ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)
        {
            if ((HWP_PORT_SMI(phy_id) == smiBus) && (HWP_PHY_MODEL_BY_PORT(phy_id) == phyChip))
            {
                for (i = 0; i < count; i++)
                {
                    if ((ret = phy_common_general_reg_mmd_get(phy_id, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if ((rData & mask) == data)
                        break;

                    mdelay(3);
                }

                if (i == count)
                {
                    printf("826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)
        {
            if ((HWP_PORT_SMI(phy_id) == smiBus) && (HWP_PHY_MODEL_BY_PORT(phy_id) == phyChip))
            {
                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    printf("826XB patch wait set broadcast PHYAD failed! 0x%X\n", ret);
                    return ret;
                }

                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    printf("826XB patch wait enable broadcast failed! 0x%X\n", ret);
                    return ret;
                }
            }
        }
    }
    else if (patch_mode == PHY_PATCH_MODE_BCAST)
    {
        if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            printf("826x patch wait disable broadcast failed! 0x%X\n", ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)
        {
            if (HWP_PHY_BASE_MACID(phy_id) == HWP_PHY_BASE_MACID(phy_id))
            {
                for (i = 0; i < count; i++)
                {
                    if ((ret = phy_common_general_reg_mmd_get(phy_id, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if ((rData & mask) == data)
                        break;
                    mdelay(3);
                }

                if (i == count)
                {
                    printf("826x patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        mdelay(1);
        //for port in same PHY, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if (HWP_PHY_BASE_MACID(phy_id) == HWP_PHY_BASE_MACID(phy_id))
            {
                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    printf("826XB patch wait set broadcast PHYAD failed! 0x%X\n", ret);
                    return ret;
                }

                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    printf("826XB patch wait enable broadcast failed! 0x%X\n", ret);
                    return ret;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < count; i++)
        {
            if ((ret = phy_common_general_reg_mmd_get(phy_id, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            ++cnt;
            if ((rData & mask) == data)
                break;

            mdelay(1);
        }

        if (i == count)
        {
            printf("826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", mmdAddr, mmdReg, data, mask, rData, cnt);
            return RT_ERR_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}

int32_t _phy_rtl826xb_patch_wait_not_equal(u32 phy_id, u32 mmdAddr, u32 mmdReg, u32 data, u32 mask, u8 patch_mode)
{
    int32_t  ret = 0;
    u32 rData = 0;
    u32 cnt = 0;
    int count = 1000;
    int i = 0;

    void*  p = NULL;
    u8  smiBus = HWP_PORT_SMI(phy_id);
    u32 phyChip = HWP_PHY_MODEL_BY_PORT(phy_id);
    u8  bcast_phyad = HWP_PHY_ADDR(phy_id);

    if (patch_mode == PHY_PATCH_MODE_BCAST_BUS)
    {
        if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            printf("826XB patch wait disable broadcast failed! 0x%X\n", ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)
        {
            if ((HWP_PORT_SMI(phy_id) == smiBus) && (HWP_PHY_MODEL_BY_PORT(phy_id) == phyChip))
            {
                for (i = 0; i < count; i++)
                {
                    if ((ret = phy_common_general_reg_mmd_get(phy_id, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if ((rData & mask) != data)
                        break;

                    mdelay(3);
                }
                if (i == count)
                {
                    printf("826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        mdelay(1);
        //for port in same SMI bus, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(unit, p)
        {
            if ((HWP_PORT_SMI(phy_id) == smiBus) && (HWP_PHY_MODEL_BY_PORT(phy_id) == phyChip))
            {
                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    printf("826XB patch wait set broadcast PHYAD failed! 0x%X\n", ret);
                    return ret;
                }

                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    printf("826XB patch wait enable broadcast failed! 0x%X\n", ret);
                    return ret;
                }
            }
        }
    }
    else if (patch_mode == PHY_PATCH_MODE_BCAST)
    {
        if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 0)) != RT_ERR_OK)
        {
            printf("826x patch wait disable broadcast failed! 0x%X\n", ret);
            return ret;
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)
        {
            if (HWP_PHY_BASE_MACID(phy_id) == HWP_PHY_BASE_MACID(phy_id))
            {
                for (i = 0; i < count; i++)
                {
                    if ((ret = phy_common_general_reg_mmd_get(phy_id, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                    {
                        return ret;
                    }
                    ++cnt;

                    if (((rData & mask) != data))
                        break;

                    mdelay(3);
                }

                if (i == count)
                {
                    printf("826XB patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", mmdAddr, mmdReg, data, mask, rData, cnt);
                    return RT_ERR_TIMEOUT;
                }
            }
        }

        mdelay(1);
        //for port in same PHY, set mdio broadcast ENABLE
        HWP_PORT_TRAVS_EXCEPT_CPU(phy_id, p)
        {
            if (HWP_PHY_BASE_MACID(phy_id) == HWP_PHY_BASE_MACID(phy_id))
            {
                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST_PHYAD, (uint32)bcast_phyad)) != RT_ERR_OK)
                {
                    printf("826XB patch wait set broadcast PHYAD failed! 0x%X\n", ret);
                    return ret;
                }

                if ((ret = phy_826xb_ctrl_set(phy_id, RTK_PHY_CTRL_MIIM_BCAST, 1)) != RT_ERR_OK)
                {
                    printf("826XB patch wait enable broadcast failed! 0x%X\n", ret);
                    return ret;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < count; i++)
        {
            if ((ret = phy_common_general_reg_mmd_get(phy_id, mmdAddr, mmdReg, &rData)) != RT_ERR_OK)
                return ret;

            ++cnt;
            if ((rData & mask) != data)
                break;

            mdelay(1);
        }
        if (i == count)
        {
            printf("826xb patch wait[%u,0x%X,0x%X,0x%X]:0x%X cnt:%u\n", mmdAddr, mmdReg, data, mask, rData, cnt);
            return RT_ERR_TIMEOUT;
        }

    }

    return RT_ERR_OK;
}


int32_t _phy_rtl826xb_patch_sds_get(u32 phy_id, u32 sdsPage, u32 sdsReg, u32 *pData)
{
    int32_t  ret = 0;
    u32 rData = 0;
    u32 sdsAddr = 0x8000 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826xb_patch_top_set(phy_id, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826xb_patch_top_get(phy_id, 40, 18, &rData)) != RT_ERR_OK)
        return ret;
    *pData = rData;
    return _phy_rtl826xb_patch_wait(phy_id, PHY_MMD_VEND1, 0x143, 0, BIT_15, PHY_PATCH_MODE_NORMAL);
}

int32_t _phy_rtl826xb_patch_sds_set(u32 phy_id, u32 sdsPage, u32 sdsReg, u32 wData, u8 patch_mode)
{
    int32_t  ret = 0;
    u32 sdsAddr = 0x8800 + (sdsReg << 6) + sdsPage;

    if ((ret = _phy_rtl826xb_patch_top_set(phy_id, 40, 17, wData)) != RT_ERR_OK)
        return ret;
    if ((ret = _phy_rtl826xb_patch_top_set(phy_id, 40, 19, sdsAddr)) != RT_ERR_OK)
        return ret;
    return _phy_rtl826xb_patch_wait(phy_id, PHY_MMD_VEND1, 0x143, 0, BIT_15, patch_mode);
}

int32_t phy_rtl826xb_patch_op(u32 phy_id, u8 portOffset, rtk_hwpatch_t *pPatch_data, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    u32 rData = 0, wData = 0;
    u16 reg = 0;
    u8 patch_op = 0;
    u32 mask = 0;

    if ((pPatch_data->portmask & (1 << portOffset)) == 0)
    {
        return RT_ERR_ABORT;
    }
    mask = UINT32_BITS_MASK(pPatch_data->msb, pPatch_data->lsb);
    patch_op = phy_patch_op_translate(patch_mode, pPatch_data->patch_op, pPatch_data->compare_op);

    #if 0
    osal_printf("[%s,%d]u%up%u, patch_mode:%u/patch_op:%u/compare_op:%u => op: %u\n", __FUNCTION__, __LINE__, unit, port,
                           patch_mode, pPatch_data->patch_op, pPatch_data->compare_op,
                           patch_op);
    #endif

    switch (patch_op)
    {
        case RTK_PATCH_OP_PHY:
            reg = _phy_rtl826xb_mmd_convert(pPatch_data->pagemmd, pPatch_data->addr);
            if ((pPatch_data->msb != 15) || (pPatch_data->lsb != 0))
            {
                RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 31, reg, &rData), ret);
            }
            wData = REG32_FIELD_SET(rData, pPatch_data->data, pPatch_data->lsb, mask);
            RT_ERR_CHK(phy_common_general_reg_mmd_set(phy_id, 31, reg, wData), ret);
            break;
        case RTK_PATCH_OP_CMP_PHY:
            reg = _phy_rtl826xb_mmd_convert(pPatch_data->pagemmd, pPatch_data->addr);
            RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 31, reg, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;
        case RTK_PATCH_OP_CMP_SRAM_PHY:
            reg = _phy_rtl826xb_mmd_convert(pPatch_data->sram_p, pPatch_data->sram_rw);
            RT_ERR_CHK(phy_common_general_reg_mmd_set(phy_id, 31, reg, pPatch_data->sram_a), ret);
            reg = _phy_rtl826xb_mmd_convert(pPatch_data->sram_p, pPatch_data->sram_rr);
            RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 31, reg, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;

        case RTK_PATCH_OP_PHYOCP:
            if ((pPatch_data->msb != 15) || (pPatch_data->lsb != 0))
            {
                RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 31, pPatch_data->addr, &rData), ret);
            }
            wData = REG32_FIELD_SET(rData, pPatch_data->data, pPatch_data->lsb, mask);
            RT_ERR_CHK(phy_common_general_reg_mmd_set(phy_id, 31, pPatch_data->addr, wData), ret);
            break;
        case RTK_PATCH_OP_CMP_PHYOCP:
            RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 31, pPatch_data->addr, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;
        case RTK_PATCH_OP_CMP_SRAM_PHYOCP:
            RT_ERR_CHK(phy_common_general_reg_mmd_set(phy_id, 31, pPatch_data->sram_rw, pPatch_data->sram_a), ret);
            RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 31, pPatch_data->sram_rr, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;

        case RTK_PATCH_OP_TOP:
            if ((pPatch_data->msb != 15) || (pPatch_data->lsb != 0))
            {
                RT_ERR_CHK(_phy_rtl826xb_patch_top_get(phy_id, pPatch_data->pagemmd, pPatch_data->addr, &rData), ret);
            }
            wData = REG32_FIELD_SET(rData, pPatch_data->data, pPatch_data->lsb, mask);
            RT_ERR_CHK(_phy_rtl826xb_patch_top_set(phy_id, pPatch_data->pagemmd, pPatch_data->addr, wData), ret);
            break;
        case RTK_PATCH_OP_CMP_TOP:
            RT_ERR_CHK(_phy_rtl826xb_patch_top_get(phy_id, pPatch_data->pagemmd, pPatch_data->addr, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;
        case RTK_PATCH_OP_CMP_SRAM_TOP:
            RT_ERR_CHK(_phy_rtl826xb_patch_top_set(phy_id, pPatch_data->sram_p, pPatch_data->sram_rw, pPatch_data->sram_a), ret);
            RT_ERR_CHK(_phy_rtl826xb_patch_top_get(phy_id, pPatch_data->sram_p, pPatch_data->sram_rr, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;

        case RTK_PATCH_OP_PSDS0:
            if ((pPatch_data->msb != 15) || (pPatch_data->lsb != 0))
            {
                RT_ERR_CHK(_phy_rtl826xb_patch_sds_get(phy_id, pPatch_data->pagemmd, pPatch_data->addr, &rData), ret);
            }
            wData = REG32_FIELD_SET(rData, pPatch_data->data, pPatch_data->lsb, mask);
            RT_ERR_CHK(_phy_rtl826xb_patch_sds_set(phy_id, pPatch_data->pagemmd, pPatch_data->addr, wData, patch_mode), ret);
            break;
        case RTK_PATCH_OP_CMP_PSDS0:
            RT_ERR_CHK(_phy_rtl826xb_patch_sds_get(phy_id, pPatch_data->pagemmd, pPatch_data->addr, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;
        case RTK_PATCH_OP_CMP_SRAM_PSDS0:
            RT_ERR_CHK(_phy_rtl826xb_patch_sds_set(phy_id, pPatch_data->sram_p, pPatch_data->sram_rw, pPatch_data->sram_a, patch_mode), ret);
            RT_ERR_CHK(_phy_rtl826xb_patch_sds_get(phy_id, pPatch_data->sram_p, pPatch_data->sram_rr, &rData), ret);
            PHYPATCH_COMPARE(pPatch_data->pagemmd, pPatch_data->addr, pPatch_data->msb, pPatch_data->lsb, pPatch_data->data, rData, mask);
            break;

        case RTK_PATCH_OP_DELAY_MS:
            mdelay(pPatch_data->data);
            break;

        case RTK_PATCH_OP_SKIP:
            return RT_ERR_ABORT;

        default:
            printf("P%u patch_op:%u not implemented yet!\n", phy_id, pPatch_data->patch_op);
            return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    return ret;
}

int32_t phy_patch_op(rt_phy_patch_db_t *pPhy_patchDb, uint32 phy_id, uint8 portOffset, uint8 patch_op, uint16 portmask, uint16 pagemmd, uint16 addr, uint8 msb, uint8 lsb, uint16 data, uint8 patch_mode)
{   
    rtk_hwpatch_t op; 
        
    op.patch_op = patch_op;
    op.portmask = portmask;
    op.pagemmd  = pagemmd;
    op.addr     = addr;
    op.msb      = msb;
    op.lsb      = lsb;
    op.data     = data;
    op.compare_op = RTK_PATCH_CMP_W;

    return pPhy_patchDb->fPatch_op(phy_id, portOffset, &op, patch_mode);
}

static int32_t _phy_rtl826xb_flow_r1(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xb82 16 4 4 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xb82, 16, 4, 4, 0x1, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xb82 16 4 4 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xb82, 16, 4, 4, 0x1, patch_mode), ret);

    //set patch_rdy [PHYReg_bit r $PHYID 0xb80 16 6 6] ; Wait for patch ready = 1
    RT_ERR_CHK(_phy_rtl826xb_patch_wait(phy_id, 31, _phy_rtl826xb_mmd_convert(0xb80, 16), BIT_6, BIT_6, patch_mode), ret);

    //PHYReg w $PHYID 0xa43 27 $0x8023
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 27, 15, 0, 0x8023, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 28 $0x3802
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 28, 15, 0, 0x3802, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 27 0xB82E
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 27, 15, 0, 0xB82E, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 28 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 28, 15, 0, 0x1, patch_mode), ret);

     return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_r12(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xb82 16 4 4 0x1
    //RT_ERR_CHK(phy_patch_op(pPatchDb, unit, port, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xb82, 16, 4, 4, 0x1, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xb82 16 4 4 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xb82, 16, 4, 4, 0x1, patch_mode), ret);

    //set patch_rdy [PHYReg_bit r $PHYID 0xb80 16 6 6] ; Wait for patch ready = 1
    RT_ERR_CHK(_phy_rtl826xb_patch_wait(phy_id, 31, _phy_rtl826xb_mmd_convert(0xb80, 16), BIT_6, BIT_6, patch_mode), ret);

    //PHYReg w $PHYID 0xa43 27 $0x8023
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 27, 15, 0, 0x8023, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 28 $0x3800
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 28, 15, 0, 0x3800, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 27 0xB82E
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 27, 15, 0, 0xB82E, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 28 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 28, 15, 0, 0x1, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_r2(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg w $PHYID 0xa43 27 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 27, 15, 0, 0x0000, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 28 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 28, 15, 0, 0x0000, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xB82 23 0 0 0x0
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xB82, 23, 0, 0, 0x0, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 27 $0x8023
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 27, 15, 0, 0x8023, patch_mode), ret);
    //PHYReg w $PHYID 0xa43 28 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa43, 28, 15, 0, 0x0000, patch_mode), ret);

    //PHYReg_bit w $PHYID 0xb82 16 4 4 0x0
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xb82, 16, 4, 4, 0x0, patch_mode), ret);
    //set patch_rdy [PHYReg_bit r $PHYID 0xb80 16 6 6] ; Wait for patch ready != 1
    RT_ERR_CHK( _phy_rtl826xb_patch_wait_not_equal(phy_id, 31, _phy_rtl826xb_mmd_convert(0xb80, 16), BIT_6, BIT_6, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_l1(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa4a 16 10 10 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa4a, 16, 10, 10, 0x1, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa4a 16 10 10 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa4a, 16, 10, 10, 0x1, patch_mode), ret);

    //set pcs_state [PHYReg_bit r $PHYID 0xa60 16 7 0] ; Wait for pcs state = 1
    RT_ERR_CHK( _phy_rtl826xb_patch_wait(phy_id, 31, _phy_rtl826xb_mmd_convert(0xa60, 16), 0x1, 0xFF, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_l2(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa4a 16 10 10 0x0
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa4a, 16, 10, 10, 0x0, patch_mode), ret);

    //set pcs_state [PHYReg_bit r $PHYID 0xa60 16 7 0] ; Wait for pcs state != 1
    RT_ERR_CHK( _phy_rtl826xb_patch_wait_not_equal(phy_id, 31, _phy_rtl826xb_mmd_convert(0xa60, 16), 0x1, 0xFF, patch_mode), ret);

     return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_n01(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa01 21 15 0 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 21, 15, 0, 0x1, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 19 15 0 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 19, 15, 0, 0x0000, patch_mode), ret);
    //# PHYReg_bit w $PHYID 0xa01 17 15 0 0x0000
    //RT_ERR_CHK(phy_patch_op(pPatchDb, unit, port, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 17, 15, 0, 0x0000, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_n02(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa01 21 15 0 0x0
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 21, 15, 0, 0x0, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 19 15 0 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 19, 15, 0, 0x0000, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 17 15 0 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 17, 15, 0, 0x0000, patch_mode), ret);
    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_n11(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa01 21 15 0 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 21, 15, 0, 0x1, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 19 15 0 0x0010
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 19, 15, 0, 0x0010, patch_mode), ret);
    //# PHYReg_bit w $PHYID 0xa01 17 15 0 0x0000
    //RT_ERR_CHK(phy_patch_op(pPatchDb, unit, port, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 17, 15, 0, 0x0000, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_n12(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa01 21 15 0 0x0
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 21, 15, 0, 0x0, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 19 15 0 0x0010
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 19, 15, 0, 0x0010, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 17 15 0 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 17, 15, 0, 0x0000, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_n21(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa01 21 15 0 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 21, 15, 0, 0x1, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 19 15 0 0x0020
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 19, 15, 0, 0x0020, patch_mode), ret);
    //# PHYReg_bit w $PHYID 0xa01 17 15 0 0x0000
    //RT_ERR_CHK(phy_patch_op(pPatchDb, unit, port, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 17, 15, 0, 0x0000, patch_mode), ret);

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_n22(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    //PHYReg_bit w $PHYID 0xa01 21 15 0 0x0
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 21, 15, 0, 0x0, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 19 15 0 0x0020
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 19, 15, 0, 0x0020, patch_mode), ret);
    //PHYReg_bit w $PHYID 0xa01 17 15 0 0x0000
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHY, 0xFF, 0xa01, 17, 15, 0, 0x0000, patch_mode), ret);

    return RT_ERR_OK;
}

#define PHYPATCH_IS_RTKSDS(_unit)	0

static int32_t _phy_rtl826xb_flow_s(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    if (PHYPATCH_IS_RTKSDS(phy_id))
    {
        PHYPATCH_DB_GET(phy_id, pPatchDb);

        RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PSDS0, 0xff, 0x07, 0x10, 15, 0, 0x80aa, patch_mode), ret);
        RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PSDS0, 0xff, 0x06, 0x12, 15, 0, 0x5078, patch_mode), ret);
    }

    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_pi(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;
    uint32_t rData = 0, cnt = 0;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    _phy_rtl826xb_flow_l1(phy_id, portOffset, patch_mode);

    //  PP_PHYReg_bit w $PHYID 0xbf86 9 9 0x1; #SS_EN_XG = 1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 9, 9, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 8 8 0x0;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 8, 8, 0x0, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 7 7 0x1;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 7, 7, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 6 6 0x1;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 6, 6, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 5 5 0x1;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 5, 5, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 4 4 0x1;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 4, 4, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 6 6 0x0;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 6, 6, 0x0, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 9 9 0x0;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 9, 9, 0x0, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 7 7 0x0;
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 7, 7, 0x0, patch_mode), ret);

    //PP_PHYReg_bit r $PHYID 0xbc62 12 8
    if ((ret = phy_common_general_reg_mmd_get(phy_id, PHY_MMD_VEND2, 0xbc62, &rData)) != RT_ERR_OK)
        return ret;
    rData = REG32_FIELD_GET(rData, 8, 0x1F00);
    for (cnt = 0; cnt <= rData; cnt++)
    {
        //PP_PHYReg_bit w $PHYID 0xbc62 12 8 $t
        RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbc62, 12, 8, cnt, patch_mode), ret);
    }

    //   PP_PHYReg_bit w $PHYID 0xbc02 2 2 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbc02, 2, 2, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbc02 3 3 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbc02, 3, 3, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 6 6 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 6, 6, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 9 9 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 9, 9, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbf86 7 7 0x1
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbf86, 7, 7, 0x1, patch_mode), ret);
    //   PP_PHYReg_bit w $PHYID 0xbc04 9 2 0xff
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 31, 0xbc04, 9, 2, 0xff, patch_mode), ret);

    _phy_rtl826xb_flow_l2(phy_id, portOffset, patch_mode);
    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_cmpstart(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 1, 0, 11, 11, 0x0, patch_mode), ret);
    return RT_ERR_OK;
}

static int32_t _phy_rtl826xb_flow_cmpend(u32 phy_id, u8 portOffset, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);
    RT_ERR_CHK(phy_patch_op(pPatchDb, phy_id, portOffset, RTK_PATCH_OP_PHYOCP, 0xFF, 1, 0, 11, 11, 0x1, patch_mode), ret);
    return RT_ERR_OK;
}

int32_t phy_rtl826xb_patch_flow(u32 phy_id, u8 portOffset, u8 patch_flow, u8 patch_mode)
{
    int32_t ret = RT_ERR_OK;

    RT_LOG(LOG_INFO, (MOD_HAL | MOD_PHY), "flow%u\n", __FUNCTION__, (patch_flow - PHY_PATCH_TYPE_END));
    switch (patch_flow)
    {
        case RTK_PATCH_TYPE_FLOW(0):
            RT_ERR_CHK(_phy_rtl826xb_flow_r1(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(1):
            RT_ERR_CHK(_phy_rtl826xb_flow_r2(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(2):
            RT_ERR_CHK(_phy_rtl826xb_flow_l1(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(3):
            RT_ERR_CHK(_phy_rtl826xb_flow_l2(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(4):
            RT_ERR_CHK(_phy_rtl826xb_flow_n01(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(5):
            RT_ERR_CHK(_phy_rtl826xb_flow_n02(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(6):
            RT_ERR_CHK(_phy_rtl826xb_flow_n11(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(7):
            RT_ERR_CHK(_phy_rtl826xb_flow_n12(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(8):
            RT_ERR_CHK(_phy_rtl826xb_flow_n21(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(9):
            RT_ERR_CHK(_phy_rtl826xb_flow_n22(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(10):
            RT_ERR_CHK(_phy_rtl826xb_flow_s(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(11):
            RT_ERR_CHK(_phy_rtl826xb_flow_pi(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(12):
            RT_ERR_CHK(_phy_rtl826xb_flow_r12(phy_id, portOffset, patch_mode), ret);
            break;

        case RTK_PATCH_TYPE_FLOW(13):
            RT_ERR_CHK(_phy_rtl826xb_flow_cmpstart(phy_id, portOffset, patch_mode), ret);
            break;
        case RTK_PATCH_TYPE_FLOW(14):
            RT_ERR_CHK(_phy_rtl826xb_flow_cmpend(phy_id, portOffset, patch_mode), ret);
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}

int32_t phy_rtl826xb_patch_db_init(u32 phy_id, rt_phy_patch_db_t **pPhy_patchDb)
{
    int32_t ret = 0;
    rt_phy_patch_db_t *patch_db = NULL;
    u32 rData = 0;

    patch_db = (rt_phy_patch_db_t *)malloc(sizeof(rt_phy_patch_db_t));
    RT_PARAM_CHK(NULL == patch_db, RT_ERR_MEM_ALLOC);
    memset(patch_db, 0x0, sizeof(rt_phy_patch_db_t));

    /* patch callback */
    patch_db->fPatch_op = phy_rtl826xb_patch_op;
    patch_db->fPatch_flow = phy_rtl826xb_patch_flow;

    /* patch table */
    RT_ERR_CHK(phy_common_general_reg_mmd_get(phy_id, 30, 0x104, &rData), ret);
    if ((rData & 0xFFC0) == 0x1140) /* RTL8261BE */
    {
	printf("patch db RTL8261BE\n");
        /* patch */
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  0, RTK_PATCH_TYPE_FLOW(0), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  1, PHY_PATCH_TYPE_NCTL0, rtl8261n_c_nctl0_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  2, PHY_PATCH_TYPE_NCTL1, rtl8261n_c_nctl1_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  3, PHY_PATCH_TYPE_NCTL2, rtl8261n_c_nctl2_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  4, PHY_PATCH_TYPE_UC2, rtl8261n_c_uc2_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  5, PHY_PATCH_TYPE_UC, rtl8261n_c_uc_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  6, PHY_PATCH_TYPE_DATARAM, rtl8261n_c_dataram_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  7, RTK_PATCH_TYPE_FLOW(1), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  8, RTK_PATCH_TYPE_FLOW(2), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  9, PHY_PATCH_TYPE_ALGXG, rtl8261n_c_algxg_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 10, PHY_PATCH_TYPE_ALG1G, rtl8261n_c_alg_giga_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 11, PHY_PATCH_TYPE_NORMAL, rtl8261n_c_normal_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 12, PHY_PATCH_TYPE_TOP, rtl8261n_c_top_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 13, PHY_PATCH_TYPE_SDS, rtl8261n_c_sds_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 14, PHY_PATCH_TYPE_AFE, rtl8261n_c_afe_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 15, PHY_PATCH_TYPE_RTCT, rtl8261n_c_rtct_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 16, RTK_PATCH_TYPE_FLOW(3), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 17, RTK_PATCH_TYPE_FLOW(10), NULL);

        /* compare */
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  0, RTK_PATCH_TYPE_FLOW(13), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  1, PHY_PATCH_TYPE_TOP, rtl8261n_c_top_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  2, PHY_PATCH_TYPE_SDS, rtl8261n_c_sds_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  3, PHY_PATCH_TYPE_AFE, rtl8261n_c_afe_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  4, RTK_PATCH_TYPE_FLOW(4), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  5, PHY_PATCH_TYPE_NCTL0, rtl8261n_c_nctl0_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  6, RTK_PATCH_TYPE_FLOW(5), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  7, RTK_PATCH_TYPE_FLOW(6), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  8, PHY_PATCH_TYPE_NCTL1, rtl8261n_c_nctl1_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  9, RTK_PATCH_TYPE_FLOW(7), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 10, RTK_PATCH_TYPE_FLOW(8), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 11, PHY_PATCH_TYPE_NCTL2, rtl8261n_c_nctl2_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 12, RTK_PATCH_TYPE_FLOW(9), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 13, PHY_PATCH_TYPE_UC, rtl8261n_c_uc_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 14, PHY_PATCH_TYPE_UC2, rtl8261n_c_uc2_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 15, RTK_PATCH_TYPE_FLOW(0), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 16, PHY_PATCH_TYPE_DATARAM, rtl8261n_c_dataram_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 17, RTK_PATCH_TYPE_FLOW(1), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 18, PHY_PATCH_TYPE_ALGXG, rtl8261n_c_algxg_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 19, PHY_PATCH_TYPE_ALG1G, rtl8261n_c_alg_giga_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 20, PHY_PATCH_TYPE_NORMAL, rtl8261n_c_normal_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 21, PHY_PATCH_TYPE_RTCT, rtl8261n_c_rtct_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 22, RTK_PATCH_TYPE_FLOW(14), NULL);
    }
    else if ((rData & 0xF) == 0x0)
    {
        /* patch */
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  0, RTK_PATCH_TYPE_FLOW(12), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  1, PHY_PATCH_TYPE_NCTL0, rtl8264b_nctl0_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  2, PHY_PATCH_TYPE_NCTL1, rtl8264b_nctl1_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  3, PHY_PATCH_TYPE_NCTL2, rtl8264b_nctl2_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  4, PHY_PATCH_TYPE_UC2, rtl8264b_uc2_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  5, PHY_PATCH_TYPE_UC, rtl8264b_uc_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  6, PHY_PATCH_TYPE_DATARAM, rtl8264b_dataram_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  7, RTK_PATCH_TYPE_FLOW(1), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  8, RTK_PATCH_TYPE_FLOW(2), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  9, PHY_PATCH_TYPE_ALGXG, rtl8264b_algxg_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 10, PHY_PATCH_TYPE_ALG1G, rtl8264b_alg_giga_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 11, PHY_PATCH_TYPE_NORMAL, rtl8264b_normal_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 12, PHY_PATCH_TYPE_TOP, rtl8264b_top_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 13, PHY_PATCH_TYPE_SDS, rtl8264b_sds_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 14, PHY_PATCH_TYPE_AFE, rtl8264b_afe_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 15, PHY_PATCH_TYPE_RTCT, rtl8264b_rtct_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 16, RTK_PATCH_TYPE_FLOW(3), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 17, RTK_PATCH_TYPE_FLOW(11), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 18, RTK_PATCH_TYPE_FLOW(10), NULL);

        /* compare */
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  0, RTK_PATCH_TYPE_FLOW(13), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  1, PHY_PATCH_TYPE_TOP, rtl8264b_top_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  2, PHY_PATCH_TYPE_SDS, rtl8264b_sds_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  3, PHY_PATCH_TYPE_AFE, rtl8264b_afe_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  4, RTK_PATCH_TYPE_FLOW(4), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  5, PHY_PATCH_TYPE_NCTL0, rtl8264b_nctl0_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  6, RTK_PATCH_TYPE_FLOW(5), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  7, RTK_PATCH_TYPE_FLOW(6), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  8, PHY_PATCH_TYPE_NCTL1, rtl8264b_nctl1_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  9, RTK_PATCH_TYPE_FLOW(7), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 10, RTK_PATCH_TYPE_FLOW(8), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 11, PHY_PATCH_TYPE_NCTL2, rtl8264b_nctl2_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 12, RTK_PATCH_TYPE_FLOW(9), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 13, PHY_PATCH_TYPE_UC, rtl8264b_uc_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 14, PHY_PATCH_TYPE_UC2, rtl8264b_uc2_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 15, RTK_PATCH_TYPE_FLOW(12), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 16, PHY_PATCH_TYPE_DATARAM, rtl8264b_dataram_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 17, RTK_PATCH_TYPE_FLOW(1), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 18, PHY_PATCH_TYPE_ALGXG, rtl8264b_algxg_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 19, PHY_PATCH_TYPE_ALG1G, rtl8264b_alg_giga_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 20, PHY_PATCH_TYPE_NORMAL, rtl8264b_normal_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 21, PHY_PATCH_TYPE_RTCT, rtl8264b_rtct_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 22, RTK_PATCH_TYPE_FLOW(14), NULL);
    }
    else
    {
        /* patch */
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  0, RTK_PATCH_TYPE_FLOW(0), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  1, PHY_PATCH_TYPE_NCTL0, rtl8261n_c_nctl0_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  2, PHY_PATCH_TYPE_NCTL1, rtl8261n_c_nctl1_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  3, PHY_PATCH_TYPE_NCTL2, rtl8261n_c_nctl2_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  4, PHY_PATCH_TYPE_UC2, rtl8261n_c_uc2_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  5, PHY_PATCH_TYPE_UC, rtl8261n_c_uc_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  6, PHY_PATCH_TYPE_DATARAM, rtl8261n_c_dataram_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  7, RTK_PATCH_TYPE_FLOW(1), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  8, RTK_PATCH_TYPE_FLOW(2), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db,  9, PHY_PATCH_TYPE_ALGXG, rtl8261n_c_algxg_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 10, PHY_PATCH_TYPE_ALG1G, rtl8261n_c_alg_giga_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 11, PHY_PATCH_TYPE_NORMAL, rtl8261n_c_normal_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 12, PHY_PATCH_TYPE_TOP, rtl8261n_c_top_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 13, PHY_PATCH_TYPE_SDS, rtl8261n_c_sds_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 14, PHY_PATCH_TYPE_AFE, rtl8261n_c_afe_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 15, PHY_PATCH_TYPE_RTCT, rtl8261n_c_rtct_conf);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 16, RTK_PATCH_TYPE_FLOW(3), NULL);
        PHYPATCH_SEQ_TABLE_ASSIGN(patch_db, 17, RTK_PATCH_TYPE_FLOW(10), NULL);

        /* compare */
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  0, RTK_PATCH_TYPE_FLOW(13), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  1, PHY_PATCH_TYPE_TOP, rtl8261n_c_top_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  2, PHY_PATCH_TYPE_SDS, rtl8261n_c_sds_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  3, PHY_PATCH_TYPE_AFE, rtl8261n_c_afe_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  4, RTK_PATCH_TYPE_FLOW(4), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  5, PHY_PATCH_TYPE_NCTL0, rtl8261n_c_nctl0_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  6, RTK_PATCH_TYPE_FLOW(5), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  7, RTK_PATCH_TYPE_FLOW(6), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  8, PHY_PATCH_TYPE_NCTL1, rtl8261n_c_nctl1_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db,  9, RTK_PATCH_TYPE_FLOW(7), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 10, RTK_PATCH_TYPE_FLOW(8), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 11, PHY_PATCH_TYPE_NCTL2, rtl8261n_c_nctl2_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 12, RTK_PATCH_TYPE_FLOW(9), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 13, PHY_PATCH_TYPE_UC, rtl8261n_c_uc_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 14, PHY_PATCH_TYPE_UC2, rtl8261n_c_uc2_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 15, RTK_PATCH_TYPE_FLOW(0), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 16, PHY_PATCH_TYPE_DATARAM, rtl8261n_c_dataram_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 17, RTK_PATCH_TYPE_FLOW(1), NULL);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 18, PHY_PATCH_TYPE_ALGXG, rtl8261n_c_algxg_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 19, PHY_PATCH_TYPE_ALG1G, rtl8261n_c_alg_giga_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 20, PHY_PATCH_TYPE_NORMAL, rtl8261n_c_normal_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 21, PHY_PATCH_TYPE_RTCT, rtl8261n_c_rtct_conf);
        PHYPATCH_CMP_TABLE_ASSIGN(patch_db, 22, RTK_PATCH_TYPE_FLOW(14), NULL);
    }
    *pPhy_patchDb = patch_db;
    printf ("patch_db address: 0x%p\n", patch_db);
    return ret;
}

static int32 _phy_patch_process(uint32 phy_id, uint8 portOffset, rtk_hwpatch_t *pPatch, int32 size, uint8 patch_mode)
{
    int32 i = 0;
    int32 ret = 0;
    int32 chk_ret = RT_ERR_OK;
    int32 n;
    rtk_hwpatch_t *patch = pPatch;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    if (size <= 0)
    {
        return RT_ERR_OK;
    }
    n = size / sizeof(rtk_hwpatch_t);

    for (i = 0; i < n; i++)
    {
        ret = pPatchDb->fPatch_op(phy_id, portOffset, &patch[i], patch_mode);
        if ((ret != RT_ERR_ABORT) && (ret != RT_ERR_OK))
        {
            if ((ret == RT_ERR_CHECK_FAILED) && (patch_mode == PHY_PATCH_MODE_CMP))
            {
                printf("PATCH CHECK: Failed entry:%u|%u|0x%X|0x%X|%u|%u|0x%X\n",
                            i + 1, patch[i].patch_op, patch[i].pagemmd, patch[i].addr, patch[i].msb, patch[i].lsb, patch[i].data);
                chk_ret = RT_ERR_CHECK_FAILED;
                continue;
            }
            else
            {
                printf("P%u %s failed! %u[%u][0x%X][0x%X][0x%X] ret=0x%X\n", phy_id, __FUNCTION__,
                       i+1, patch[i].patch_op, patch[i].pagemmd, patch[i].addr, patch[i].data, ret);
                return ret;
            }
        }

    }
    return (chk_ret == RT_ERR_CHECK_FAILED) ? chk_ret : RT_ERR_OK;
}

int32 phy_patch(uint32 phy_id, uint8 portOffset, uint8 patch_mode)
{
    int32 ret = RT_ERR_OK;
    int32 chk_ret = RT_ERR_OK;
    uint32 i = 0;
    uint8 patch_type = 0;
    rt_phy_patch_db_t *pPatchDb = NULL;
    rtk_hwpatch_seq_t *table = NULL;

    PHYPATCH_DB_GET(phy_id, pPatchDb);

    if ((pPatchDb == NULL) || (pPatchDb->fPatch_op == NULL) || (pPatchDb->fPatch_flow == NULL))
    {
        printf("phy_patch, db is NULL\n");
        return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    if (patch_mode == PHY_PATCH_MODE_CMP)
    {
        table = pPatchDb->cmp_table;
    }
    else
    {
        table = pPatchDb->seq_table;
    }
    // printf("phy_patch: portOffset:%u  patch_mode:%u\n", portOffset, patch_mode);

    for (i = 0; i < RTK_PATCH_SEQ_MAX; i++)
    {
        patch_type = table[i].patch_type;
        //printf("phy_patch: table[%u] patch_type:%u\n", i, patch_type);

        if (RTK_PATCH_TYPE_IS_DATA(patch_type))
        {
            ret = _phy_patch_process(phy_id, portOffset, table[i].patch.data.conf, table[i].patch.data.size, patch_mode);

            if (ret == RT_ERR_CHECK_FAILED)
	    {
		printf("RT_ERR_CHECK_FAILED\n");
                chk_ret = ret;
	    }
            else if (ret  != RT_ERR_OK)
            {
                printf("patch_mode:%u id:%u patch-%u failed. ret:0x%X\n", patch_mode, i, patch_type, ret);
                return ret;
            }
        }
        else if (RTK_PATCH_TYPE_IS_FLOW(patch_type))
        {
            RT_ERR_CHK_EHDL(pPatchDb->fPatch_flow(phy_id, portOffset, table[i].patch.flow_id, patch_mode),
                            ret, printf("patch_mode:%u id:%u patch-%u failed. ret:0x%X\n", patch_mode, i, patch_type, ret););
        }
        else
        {
	    printf("patch type is not supported\n");
            break;
        }
    }

    return (chk_ret == RT_ERR_CHECK_FAILED) ? chk_ret : RT_ERR_OK;
}

