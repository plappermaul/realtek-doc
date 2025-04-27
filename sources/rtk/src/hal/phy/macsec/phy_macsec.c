/*
 * Copyright (C) 2022 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PHY MACSec Driver APIs.
 *
 * Feature : PHY MACSec Driver APIs
 *
 */

/*
 * Include Files
 */
#include <hal/phy/macsec/phy_macsec.h>
#include <hal/common/halctrl.h>
#include <osal/time.h>
#include <osal/memory.h>
#include <hal/phy/macsec/aes.h>


/*
 * Symbol Definition
 */
#define MACSEC_DBG_PRINT   0

/*
 * Data Declaration
 */
phy_macsec_info_t    *phy_macsec_info[RTK_MAX_NUM_OF_UNIT] = { NULL };

/*
 * Macro Declaration
 */
#if MACSEC_DBG_PRINT
    #define MACSEC_DBG(fmt, args...)    osal_printf("[%s:%d]"fmt, __FUNCTION__, __LINE__, ##args)
#else
    #define MACSEC_DBG(fmt, args...)    do {} while(0)
#endif

#define MACSEC_RET_CHK( _call) \
do\
{\
    if((ret = _call) != RT_ERR_OK)\
    {\
        return ret;\
    }\
} while(0)

#define MACSEC_REG_GET(unit, port, dir, reg, pData) \
do\
{\
    if((ret = phy_macsec_reg_get(unit, port, dir, reg, pData)) != RT_ERR_OK)\
    {\
        return ret;\
    }\
} while (0)

#define MACSEC_REG_SET(unit, port, dir, reg, data) \
do\
{\
    if((ret = phy_macsec_reg_set(unit, port, dir, reg, data)) != RT_ERR_OK)\
    {\
        return ret;\
    }\
} while (0)

#define MACSEC_REG_ARRAY_GET(unit, port, dir, reg, pArray, cnt) \
do\
{\
    if((ret = phy_macsec_reg_array_get(unit, port, dir, reg, pArray, cnt)) != RT_ERR_OK)\
    {\
        return ret;\
    }\
} while(0)

#define MACSEC_REG_ARRAY_SET(unit, port, dir, reg, pArray, cnt) \
do\
{\
    if((ret = phy_macsec_reg_array_set(unit, port, dir, reg, pArray, cnt)) != RT_ERR_OK)\
    {\
        return ret;\
    }\
} while(0)

#ifndef WAIT_COMPLETE_VAR
#define WAIT_COMPLETE_VAR() \
    osal_usecs_t    _t, _now, _t_wait=0, _timeout;  \
    int32           _chkCnt=0;

#define WAIT_COMPLETE(_timeout_us)     \
    _timeout = _timeout_us;  \
    for(osal_time_usecs_get(&_t),osal_time_usecs_get(&_now),_t_wait=0,_chkCnt=0 ; \
        (_t_wait <= _timeout); \
        osal_time_usecs_get(&_now), _chkCnt++, _t_wait += ((_now >= _t) ? (_now - _t) : (0xFFFFFFFF - _t + _now)),_t = _now \
       )

#define WAIT_COMPLETE_IS_TIMEOUT()   (_t_wait > _timeout)
#endif

/*
 * Function Declaration
 */

static int32
phy_macsec_reg_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 *pData)
{
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_reg_get(unit, port, dir, reg, pData);
}

static int32
phy_macsec_reg_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 reg, uint32 data)
{
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    return pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_macsec_reg_set(unit, port, dir, reg, data);
}

static int32
phy_macsec_reg_array_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
                         const uint32 reg, uint32* pArray, const uint32 cnt)
{
    int32  ret = RT_ERR_OK;
    uint32 data = 0, i = 0;
    uint32 offset = reg;

    for (i = 0; i < cnt; i++)
    {
        MACSEC_REG_GET(unit, port, dir, offset, &data);
        pArray[i] = data;
        offset += MACSEC_REG_OFFS;
    }
    return ret;
}

static int32
phy_macsec_reg_array_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
                         const uint32 reg, uint32* pArray, const uint32 cnt)
{
    int32  ret = RT_ERR_OK;
    uint32 data = 0, i = 0;
    uint32 offset = reg;

    for (i = 0; i < cnt; i++)
    {
        data = pArray[i];
        MACSEC_REG_SET(unit, port, dir, offset, data);
        offset += MACSEC_REG_OFFS;
    }
    return ret;
}

static int32
phy_macsec_hw_flow_action_write(
        uint32 unit, rtk_port_t port,
        rtk_macsec_dir_t dir, uint32 flow_index,
        const uint32 SAIndex,
        const uint8 FlowType,
        const uint8 DestPort,
        const uint8 fDropNonReserved,
        const uint8 fFlowCryptAuth,
        const uint8 DropAction,
        const uint8 fProtect,
        const uint8 fSAInUse,
        const uint8 fIncludeSCI,
        const uint8 ValidateFrames,
        const uint8 TagBypassSize,
        const uint8 fSaIndexUpdate,
        const uint8 ConfOffset,
        const uint8 fConfProtect)
{
    int32 ret = RT_ERR_OK;
    uint32 data = 0;

    if(fDropNonReserved)
        data |= BIT_4_IN32;
    else
        data &= ~BIT_4_IN32;

    if(fFlowCryptAuth)
        data |= BIT_5_IN32;
    else
        data &= ~BIT_5_IN32;

    if(fProtect)
        data |= BIT_16_IN32;
    else
        data &= ~BIT_16_IN32;

    if(fSAInUse)
        data |= BIT_17_IN32;
    else
        data &= ~BIT_17_IN32;

    if(fIncludeSCI)
        data |= BIT_18_IN32;
    else
        data &= ~BIT_18_IN32;

    if(fSaIndexUpdate)
        data |= BIT_23_IN32;
    else
        data &= ~BIT_23_IN32;

    if(fConfProtect)
        data |= BIT_31_IN32;
    else
        data &= ~BIT_31_IN32;

    data |= (uint32)((((uint32)ConfOffset)     & MASK_7_BITS) << 24);
    data |= (uint32)((((uint32)TagBypassSize)  & MASK_2_BITS) << 21);
    data |= (uint32)((((uint32)ValidateFrames) & MASK_2_BITS) << 19);
    data |= (uint32)((((uint32)SAIndex)        & MASK_8_BITS) << 8);
    data |= (uint32)((((uint32)DropAction)     & MASK_2_BITS) << 6);
    data |= (uint32)((((uint32)DestPort)       & MASK_2_BITS) << 2);
    data |= (uint32)((((uint32)FlowType)       & MASK_2_BITS));

    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_FLOW_CTRL(flow_index), data);

    return ret;
}

static void
phy_macsec_copy_key_to_raw(uint32 *pRaw, uint32 offset, uint8 *pKey, uint32 key_bytes)
{
    uint32_t *dst = pRaw + offset;
    const uint8_t *src = pKey;
    unsigned int i,j;
    uint32_t w;
    if (pRaw == NULL)
        return;
    for(i=0; i<(key_bytes+3)/4; i++)
    {
        w=0;
        for(j=0; j<4; j++)
            w=(w>>8)|(*src++ << 24);
        *dst++ = w;
    }
}

static void
phy_macsec_copy_raw_to_key(uint32 *pRaw, uint32 offset ,uint8 *pKey, uint32 raw_words)
{
    uint32_t *src = pRaw + offset;
    uint8_t *dst = pKey;
    unsigned int i;
    if (pRaw == NULL)
        return;

    for (i = 0; i < raw_words; i++)
    {
        *dst++ = (uint8)((src[i] & 0xff));
        *dst++ = (uint8)((src[i] & 0xff00) >> 8) ;
        *dst++ = (uint8)((src[i] & 0xff0000) >> 16) ;
        *dst++ = (uint8)((src[i] & 0xff000000) >> 24) ;
    }
}

static void
phy_macsec_hw_sa_offset_parse(phy_macsec_sa_params_t *pSa, phy_macsec_sa_offset_t *pOffs)
{
    unsigned int long_key;
    memset(pOffs, 0, sizeof(phy_macsec_sa_offset_t));

    pOffs->key_offs = 2;
    if (pSa->key_bytes == 16)
    {
        long_key = 0;
    }
    else
    {
        long_key = 4;
    }
    pOffs->hkey_offs = long_key + 6;
    pOffs->seq_offs = long_key + 10;
    if (pSa->direction == RTK_MACSEC_DIR_EGRESS)
    {
        if ((pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN) != 0)
        {
            pOffs->ctx_salt_offs = long_key + 13;
            pOffs->iv_offs = long_key + 16;
            if (long_key)
                pOffs->upd_ctrl_offs = 16;
            else
                pOffs->upd_ctrl_offs = 19;
        }
        else
        {
            pOffs->iv_offs = long_key + 11;
            pOffs->upd_ctrl_offs = long_key + 15;
        }
    }
    else
    {
        if ((pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN) != 0)
        {
            pOffs->mask_offs = long_key + 12;
            pOffs->ctx_salt_offs = long_key + 13;
            pOffs->upd_ctrl_offs = 0;
        }
        else
        {
            pOffs->mask_offs = long_key + 11;
            pOffs->iv_offs = long_key + 12;
            pOffs->upd_ctrl_offs = 0;
        }
    }
}

static uint32
phy_macsec_hw_context_id_gen(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sa_index)
{
    uint32 context_id = 0;
    context_id = 0x10000 * (phy_macsec_info[unit]->port[port]->sa_gen_seq & (0xFFFF)) + 0x1000 * (dir)+ sa_index;
    phy_macsec_info[unit]->port[port]->sa_gen_seq++;
    return context_id;
}

static int32
phy_macsec_hw_sa_parse(uint32 unit, rtk_port_t port, uint32 sa_index, uint32 *pSa_raw, phy_macsec_sa_params_t *pSa)
{
    int32 ret = RT_ERR_OK;
    uint32 next_offs = 0, words = 0;
    uint8 tmp8 = 0;
    if (pSa == NULL || pSa_raw == NULL)
        return RT_ERR_INPUT;

    osal_memset(pSa, 0, sizeof(phy_macsec_sa_params_t));

    if (pSa_raw[0] & BIT_29_IN32)
    {
        pSa->flags |= RTK_PHY_MACSEC_SA_FLAG_XPN;
    }

    if ((pSa_raw[0] & MASK_4_BITS) == 0b0110)
    {
        pSa->direction = RTK_MACSEC_DIR_EGRESS;

        pSa->an = (uint8)((pSa_raw[0] >> 26) & 0x3);
    }
    else
    {
        pSa->direction = RTK_MACSEC_DIR_INGRESS;
    }

    if (((pSa_raw[0] >> 17) & MASK_3_BITS) == 0b101)
    {
        pSa->key_bytes = 16;
    }
    else
    {
        pSa->key_bytes = 32;
    }
    next_offs = 2;

    pSa->context_id = pSa_raw[1];

    /* key */
    words = (pSa->key_bytes * 8) / 32;
    phy_macsec_copy_raw_to_key(pSa_raw, next_offs, pSa->key, words);
    next_offs += words + 4;

    /* seq */
    pSa->seq = pSa_raw[next_offs];
    words = 1;

    if (pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN)
    {
        pSa->seq_h = pSa_raw[next_offs+1];
        words = 2;
    }
    next_offs += words;

    /* replay_window(ingress) */
    if (pSa->direction == RTK_MACSEC_DIR_EGRESS)
    {
        words = (pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN) ? 1 : 0;
    }
    else
    {
        pSa->replay_window = pSa_raw[next_offs];
        words = 1;
    }
    next_offs += words;

    /* CtxSalt */
    if (pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN)
    {
        words = 3;

        pSa->ssci[0] = phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[0];
        pSa->ssci[1] = phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[1];
        pSa->ssci[2] = phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[2];
        pSa->ssci[3] = phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[3];

        tmp8 = (uint8)(pSa_raw[next_offs] & 0xFF);
        pSa->salt[0] = (tmp8 ^ (pSa->ssci[0]));
        tmp8 = (uint8)((pSa_raw[next_offs] & 0xFF00) >> 8 );
        pSa->salt[1] = (tmp8 ^ (pSa->ssci[1]));
        tmp8 = (uint8)((pSa_raw[next_offs] & 0xFF0000) >> 16 );
        pSa->salt[2] = (tmp8 ^ (pSa->ssci[2]));
        tmp8 = (uint8)((pSa_raw[next_offs] & 0xFF000000) >> 24 );
        pSa->salt[3] = (tmp8 ^ (pSa->ssci[3]));

        phy_macsec_copy_raw_to_key(pSa_raw, next_offs + 1, &pSa->salt[4], 2);

    }
    else
    {
        words = 0;
    }
    next_offs += words;

    /* IV(SCI) */
    if (pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN)
    {
        if (pSa->direction == RTK_MACSEC_DIR_EGRESS)
        {
            words = 3;
        }
        else
        {
            words = 0;
        }
    }
    else
    {
        words = 4;
    }

    if (words != 0)
    {
        phy_macsec_copy_raw_to_key(pSa_raw, next_offs, pSa->sci, 2);
    }
    next_offs += words;

    /* Update Control */
    if (pSa->direction == RTK_MACSEC_DIR_EGRESS)
    {

        pSa->flow_index = (uint32)((pSa_raw[next_offs] >> 16) & MASK_15_BITS);
        pSa->next_sa_index = (uint32)(pSa_raw[next_offs] & MASK_14_BITS);
        pSa->update_en = (pSa_raw[next_offs] & BIT_31_IN32) ? 1 : 0;
        pSa->next_sa_valid = (pSa_raw[next_offs] & BIT_15_IN32) ? 1 : 0;
        pSa->sa_expired_irq = (pSa_raw[next_offs] & BIT_14_IN32) ? 1 : 0;
        MACSEC_DBG("flow_index: %u\n", pSa->flow_index);
        MACSEC_DBG("next_sa_index: %u\n", pSa->next_sa_index);
        MACSEC_DBG("update_en: %u, next_sa_valid: %u, sa_expired_irq:%u\n", pSa->update_en, pSa->next_sa_valid,  pSa->sa_expired_irq);
    }

    return ret;
}

static int32
phy_macsec_hw_sa_build(uint32 unit, rtk_port_t port, uint32 sa_index, phy_macsec_sa_params_t *pSa, phy_macsec_aes_cb aes_cb, uint32 *pSa_raw)
{
    int32 ret = RT_ERR_OK;
    phy_macsec_sa_offset_t offs;

    uint8 hkey[16] = { 0 };
    uint32 tmp = 0;
    uint32_t seq = 0; // sequence number.
    uint32_t seq_h = 0; // High part of sequence number (64-bit sequence numbers)
    uint32 gen_id = 0;

    if (pSa == NULL || pSa_raw == NULL || aes_cb == NULL)
        return RT_ERR_INPUT;
    if (pSa->an > 3)
        return RT_ERR_INPUT;

    if ((pSa->direction != RTK_MACSEC_DIR_INGRESS) &&
        (pSa->direction != RTK_MACSEC_DIR_EGRESS))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "unknown direction:%d", pSa->direction);
        return RT_ERR_INPUT;
    }

    if (pSa->context_id == 0)
    {
        gen_id = phy_macsec_hw_context_id_gen(unit, port, pSa->direction, sa_index);
    }
    else
    {
        gen_id = pSa->context_id;
    }

    // Compute offsets for various fields.
    phy_macsec_hw_sa_offset_parse(pSa, &offs);

    // Fill the entire SA record with zeros.
    osal_memset(pSa_raw, 0, PHY_MACSEC_MAX_SA_SIZE * sizeof(uint32_t));

    if (pSa->direction == RTK_MACSEC_DIR_EGRESS)
    {
        if((pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN) != 0)
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_EG64;
        }
        else
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_EG32;
        }
        seq = pSa->seq;
        seq_h = pSa->seq_h;
        pSa_raw[0] |= (pSa->an & 0x3) << 26;
    }
    else //RTK_MACSEC_DIR_INGRESS
    {
        if((pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN) != 0)
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_IG64;
        }
        else
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_IG32;
        }
        seq = (pSa->seq == 0 && pSa->seq_h == 0) ? 1 : pSa->seq;
        seq_h = pSa->seq_h;
    }

    switch (pSa->key_bytes)
    {
        case 16:
            pSa_raw[0] |= MACSEC_SAB_CW0_AES128;
            break;
        case 32:
            pSa_raw[0] |= MACSEC_SAB_CW0_AES256;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "unsupported AES key size:%d", pSa->key_bytes);
            return RT_ERR_INPUT;
    }

    // Fill in ID
    pSa_raw[1] = gen_id;

    // Fill Key and HKey
    phy_macsec_copy_key_to_raw(pSa_raw, offs.key_offs, pSa->key, pSa->key_bytes);
    /* generate hkey from key, encrypt a single all-zero block */
    aes_cb((uint8_t *)(pSa_raw + offs.hkey_offs),  hkey, pSa->key, pSa->key_bytes);
    phy_macsec_copy_key_to_raw(pSa_raw, offs.hkey_offs, hkey, 16);

    // Fill in sequence number/seqmask.
    pSa_raw[offs.seq_offs] = seq;
    if ((pSa->flags & RTK_PHY_MACSEC_SA_FLAG_XPN) != 0)
        pSa_raw[offs.seq_offs + 1] = seq_h;

    if (pSa->direction == RTK_MACSEC_DIR_INGRESS)
        pSa_raw[offs.mask_offs] = pSa->replay_window;

    // Fill in CtxSalt field.
    if (offs.ctx_salt_offs > 0)
    {
        //phy_macsec_copy_key_to_raw(&phy_macsec_info[unit]->ssci[port], 0, pSa->ssci, 8);
        phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[0] = pSa->ssci[0];
        phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[1] = pSa->ssci[1];
        phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[2] = pSa->ssci[2];
        phy_macsec_info[unit]->port[port]->sa_info[sa_index].ssci[3] = pSa->ssci[3];

        //[0] = most significant 32-bits Salt XOR-ed with SSCI
        tmp =  (pSa->salt[0] ^ pSa->ssci[0]) |
              ((pSa->salt[1] ^ pSa->ssci[1]) << 8)  |
              ((pSa->salt[2] ^ pSa->ssci[2]) << 16) |
              ((pSa->salt[3] ^ pSa->ssci[3]) << 24);
        pSa_raw[offs.ctx_salt_offs] = tmp;
        //[1:2] = lower 64-bits Salt
        phy_macsec_copy_key_to_raw(pSa_raw, offs.ctx_salt_offs + 1, pSa->salt + 4, 8);
    }

    // Fill in IV(SCI) fields.
    if (offs.iv_offs > 0)
    {
        phy_macsec_copy_key_to_raw(pSa_raw, offs.iv_offs, pSa->sci, 8);
    }

    // Fill in update control fields.
    if(offs.upd_ctrl_offs > 0)
    {
        tmp = (pSa->next_sa_index & MASK_14_BITS) |
            ((pSa->flow_index & MASK_15_BITS) << 16);

        if (pSa->update_en)
            tmp |= BIT_31_IN32;
        if (pSa->next_sa_valid)
            tmp |= BIT_15_IN32;
        if (pSa->sa_expired_irq)
            tmp |= BIT_14_IN32;

        pSa_raw[offs.upd_ctrl_offs] = tmp;
    }

    return ret;
}

int32
phy_macsec_hw_sa_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sa_index)
{
    int32 ret = RT_ERR_OK;
    uint32 sa_raw[PHY_MACSEC_MAX_SA_SIZE] = {0};

    MACSEC_REG_ARRAY_SET(unit, port, dir, MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0), sa_raw, MACSEC_XFORM_REC_SIZE(dir));

    return ret;
}

static int32
phy_macsec_hw_sa_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sa_index, phy_macsec_sa_params_t *pSa)
{
    int32 ret = RT_ERR_OK;
    uint32 sa_raw[PHY_MACSEC_MAX_SA_SIZE] = {0};

    if (pSa == NULL)
        return RT_ERR_INPUT;

    if (pSa->direction == RTK_MACSEC_DIR_EGRESS && dir == RTK_MACSEC_DIR_INGRESS)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "direction mismatch!(set egress entry on ingress)");
        return RT_ERR_INPUT;
    }
    if (pSa->direction == RTK_MACSEC_DIR_INGRESS && dir == RTK_MACSEC_DIR_EGRESS)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "direction mismatch!(set ingress entry on egress)");
        return RT_ERR_INPUT;
    }

    if ((ret = phy_macsec_hw_sa_build(unit, port, sa_index, pSa, (phy_macsec_aes_cb) AES_Encrypt, sa_raw)) != RT_ERR_OK)
        return ret;

    MACSEC_REG_ARRAY_SET(unit, port, dir, MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0), sa_raw, MACSEC_XFORM_REC_SIZE(dir));

    return ret;
}

int32
phy_macsec_hw_sa_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 sa_index, phy_macsec_sa_params_t *pSa)
{
    int32 ret = RT_ERR_OK;
    uint32 sa_raw[PHY_MACSEC_MAX_SA_SIZE] = {0};

    if (pSa == NULL)
        return RT_ERR_INPUT;

    MACSEC_REG_ARRAY_GET(unit, port, dir, MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0), sa_raw, MACSEC_XFORM_REC_SIZE(dir));

    #if MACSEC_DBG_PRINT
    MACSEC_DBG("SA offset = 0x%04X\n", MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0));
    for (ret = 0; ret < MACSEC_XFORM_REC_SIZE(dir); ret++)
    {
        MACSEC_DBG("SA[%02u] = 0x%08X\n", ret, sa_raw[ret]);
    }
    ret = RT_ERR_OK;
    #endif

    phy_macsec_hw_sa_parse(unit, port, sa_index, sa_raw, pSa);

    return ret;
}

/* Function Name:
 *      phy_macsec_flow_action_rule_set
 * Description:
 *      setup a flow control entry
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      flow_index    - table entry index
 *      pAct          - pointer to the struct for describe the rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_INPUT              - invalid parameter
 * Note:
 *      None
 */
int32
phy_macsec_hw_flow_action_rule_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index, phy_macsec_flow_action_t *pAct)
{
    int32  ret = RT_ERR_OK;
    uint32 sa_index = pAct->sa_index;
    uint8  flow_type = 0;
    uint8  dest_port = 0;
    uint8  drop_non_reserved = 0; //do not drop
    uint8  flow_crypt_auth = 0;
    uint8  drop_action = 0; // CRC
    uint8  protect = 0;
    uint8  sa_in_use = 0;
    uint8  include_sci = 0;
    uint8  validate_frames = 0;
    uint8  tag_bypass_size = 0;
    uint8  sa_index_update = 0;
    uint8  conf_offset = 0;
    uint8  conf_protect = 0;

    if (flow_index >= MACSEC_SA_MAX(unit, port))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "flow_index out of range!");
        return RT_ERR_INPUT;
    }
    if (pAct->flow_type == RTK_MACSEC_FLOW_EGRESS && dir == RTK_MACSEC_DIR_INGRESS)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "flow_type mismatch!(set egress entry on ingress)");
        return RT_ERR_INPUT;
    }
    if (pAct->flow_type == RTK_MACSEC_FLOW_INGRESS && dir == RTK_MACSEC_DIR_EGRESS)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "flow_type mismatch!(set ingress entry on egress)");
        return RT_ERR_INPUT;
    }

    dest_port = pAct->dest_port;
    switch (pAct->flow_type)
    {
        case RTK_MACSEC_FLOW_EGRESS:
            flow_type = 0b11;
            dest_port = RTK_MACSEC_PORT_COMMON;
            flow_crypt_auth = 0b0;
            protect = pAct->params.egress.protect_frame;
            sa_in_use = pAct->params.egress.sa_in_use;
            include_sci = pAct->params.egress.include_sci;
            validate_frames = pAct->params.egress.use_es | ( pAct->params.egress.use_scb << 1);
            tag_bypass_size = pAct->params.egress.tag_bypass_size;
            conf_offset = pAct->params.egress.confidentiality_offset;
            conf_protect = pAct->params.egress.conf_protect;
            break;

        case RTK_MACSEC_FLOW_INGRESS:
            flow_type = 0b10;
            dest_port = RTK_MACSEC_PORT_CONTROLLED;
            flow_crypt_auth = 0b0;
            protect = pAct->params.ingress.replay_protect;
            sa_in_use = pAct->params.ingress.sa_in_use;

            switch (pAct->params.ingress.validate_frames)
            {
               case RTK_MACSEC_VALIDATE_DISABLE:
                   validate_frames = 0b00;
                   break;
               case RTK_MACSEC_VALIDATE_CHECK:
                   validate_frames = 0b01;
                   break;
               case RTK_MACSEC_VALIDATE_STRICT:
                   validate_frames = 0b10;
                   break;
               default:
                   RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "unknown type of validate_frames!");
                   return RT_ERR_INPUT;
            }
            conf_offset = pAct->params.ingress.confidentiality_offset;
            break;

        case RTK_MACSEC_FLOW_BYPASS:
            flow_type = 0b00;
            flow_crypt_auth = 0b0;
            //sa_in_use = 1;
            sa_in_use = pAct->params.bypass_drop.sa_in_use;
            break;

        case RTK_MACSEC_FLOW_DROP:
            flow_type = 0b01;
            flow_crypt_auth = 0b0;
            //sa_in_use = 1;
            sa_in_use = pAct->params.bypass_drop.sa_in_use;
            break;

        default:
            return RT_ERR_INPUT;
    }

    ret = phy_macsec_hw_flow_action_write(
        unit, port,
        dir, flow_index,
        sa_index,
        flow_type,
        dest_port,
        drop_non_reserved,
        flow_crypt_auth,
        drop_action,
        protect,
        sa_in_use,
        include_sci,
        validate_frames,
        tag_bypass_size,
        sa_index_update,
        conf_offset,
        conf_protect);

    return ret;
}

int32
phy_macsec_hw_flow_action_rule_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index, phy_macsec_flow_action_t *pAct)
{
    int32 ret = RT_ERR_OK;
    uint32 reg = 0, reg_data = 0;
    uint8 flow_type = 0;

    if(pAct == NULL)
        return RT_ERR_INPUT;

    osal_memset(pAct, 0, sizeof(phy_macsec_flow_action_t));

    reg = MACSEC_REG_SAM_FLOW_CTRL(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);

    pAct->dest_port = (reg_data >> 2) & MASK_2_BITS;

    pAct->sa_index  = (reg_data >> 8) & MASK_8_BITS;

    flow_type = reg_data & MASK_2_BITS;
    switch (flow_type)
    {
        case 0b11:
            pAct->flow_type = RTK_MACSEC_FLOW_EGRESS;
            pAct->params.egress.protect_frame          = (reg_data >> 16) & MASK_1_BITS;
            pAct->params.egress.sa_in_use              = (reg_data >> 17) & MASK_1_BITS;
            pAct->params.egress.include_sci            = (reg_data >> 18) & MASK_1_BITS;
            pAct->params.egress.use_es                 = (reg_data >> 19) & MASK_1_BITS;
            pAct->params.egress.use_scb                = (reg_data >> 20) & MASK_1_BITS;
            pAct->params.egress.tag_bypass_size        = (reg_data >> 21) & MASK_2_BITS;
            pAct->params.egress.sa_index_update_by_hw  = (reg_data >> 23) & MASK_2_BITS;
            pAct->params.egress.confidentiality_offset = (reg_data >> 24) & MASK_7_BITS;
            pAct->params.egress.conf_protect           = (reg_data >> 31) & MASK_1_BITS;
            break;
        case 0b10:
            pAct->flow_type = RTK_MACSEC_FLOW_INGRESS;
            pAct->params.ingress.replay_protect         = (reg_data >> 16) & MASK_1_BITS;
            switch ((reg_data >> 19) & MASK_2_BITS)
            {
                case 0b00:
                    pAct->params.ingress.validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
                    break;
                case 0b01:
                    pAct->params.ingress.validate_frames = RTK_MACSEC_VALIDATE_CHECK;
                    break;
                case 0b10:
                    pAct->params.ingress.validate_frames = RTK_MACSEC_VALIDATE_STRICT;
                    break;
                default:
                    return RT_ERR_FAILED;
            }

            pAct->params.ingress.confidentiality_offset = (reg_data >> 24) & MASK_7_BITS;
            break;
        case 0b01:
        case 0b00:
        default:
            pAct->flow_type = (flow_type == 0b01) ? RTK_MACSEC_FLOW_DROP : RTK_MACSEC_FLOW_BYPASS;
            pAct->params.bypass_drop.sa_in_use = (reg_data >> 17) & MASK_1_BITS;
            break;
    }

    return ret;
}

static int32
phy_macsec_hw_flow_action_rule_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index)
{
    int32 ret = RT_ERR_OK;

    ret = phy_macsec_hw_flow_action_write(
        unit, port,
        dir, flow_index,
        0,
        0, 0, 0, 0, 0, 0,
        0, //SA not in use
        0, 0, 0, 0, 0, 0);
    return ret;
}

/* Function Name:
 *      phy_macsec_match_rule_set
 * Description:
 *      setup a match-rule entry that specify how to classify a packet
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      flow_index     - table entry index
 *      data          - pointer to the struct for describe the rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_INPUT              - invalid parameter
 * Note:
 *      None
 */
int32
phy_macsec_hw_flow_match_rule_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index, phy_macsec_flow_match_t *pMatch)
{
    int32  ret = RT_ERR_OK;
    uint32 reg_data = 0;
    uint32 srcPort = 0;
    uint16 tmp = 0;

    if (pMatch == NULL)
        return RT_ERR_INPUT;

    if (flow_index >= MACSEC_SA_MAX(unit, port))
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "flow_index out of range!");
        return RT_ERR_INPUT;
    }

    {
        reg_data = 0;
        reg_data |= (uint32)((((uint32)pMatch->mac_sa[3]) & MASK_8_BITS) << 24);
        reg_data |= (uint32)((((uint32)pMatch->mac_sa[2]) & MASK_8_BITS) << 16);
        reg_data |= (uint32)((((uint32)pMatch->mac_sa[1]) & MASK_8_BITS) << 8);
        reg_data |= (uint32)((((uint32)pMatch->mac_sa[0]) & MASK_8_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_SA_MATCH_LO(flow_index), reg_data);

        reg_data = 0;
        tmp = ((pMatch->etherType & 0xFF) << 8) | (pMatch->etherType >> 8);
        reg_data |= (uint32)((((uint32)tmp)  & MASK_16_BITS) << 16);

        reg_data |= (uint32)((((uint32)pMatch->mac_sa[5]) & MASK_8_BITS) << 8);
        reg_data |= (uint32)((((uint32)pMatch->mac_sa[4]) & MASK_8_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_SA_MATCH_HI(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= (uint32)((((uint32)pMatch->mac_da[3]) & MASK_8_BITS) << 24);
        reg_data |= (uint32)((((uint32)pMatch->mac_da[2]) & MASK_8_BITS) << 16);
        reg_data |= (uint32)((((uint32)pMatch->mac_da[1]) & MASK_8_BITS) << 8);
        reg_data |= (uint32)((((uint32)pMatch->mac_da[0]) & MASK_8_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_DA_MATCH_LO(flow_index), reg_data);

        reg_data = 0;
        reg_data |= (uint32)((((uint32)pMatch->vlan_id)   & MASK_12_BITS) << 16);
        reg_data |= (uint32)((((uint32)pMatch->mac_da[5]) & MASK_8_BITS) << 8);
        reg_data |= (uint32)((((uint32)pMatch->mac_da[4]) & MASK_8_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_DA_MATCH_HI(flow_index), reg_data);
    }

    {
        srcPort = pMatch->sourcePort;

        reg_data = 0;
        reg_data |= (pMatch->fVLANValid) ? (BIT_0_IN32) : 0;
        reg_data |= (pMatch->fQinQFound) ? (BIT_1_IN32) : 0;
        reg_data |= (pMatch->fSTagValid) ? (BIT_2_IN32) : 0;
        reg_data |= (pMatch->fQTagFound) ? (BIT_3_IN32) : 0;

        reg_data |= (pMatch->fControlPacket) ? (BIT_7_IN32)  : 0;
        reg_data |= (pMatch->fUntagged) ?      (BIT_8_IN32)  : 0;
        reg_data |= (pMatch->fTagged) ?        (BIT_9_IN32)  : 0;
        reg_data |= (pMatch->fBadTag) ?        (BIT_10_IN32) : 0;
        reg_data |= (pMatch->fKayTag) ?        (BIT_11_IN32) : 0;

        reg_data |= (uint32)((((uint32)pMatch->macsec_TCI_AN)    & MASK_8_BITS) << 24);
        reg_data |= (uint32)((((uint32)pMatch->matchPriority)    & MASK_4_BITS) << 16);
        reg_data |= (uint32)((((uint32)srcPort)                & MASK_2_BITS) << 12);
        reg_data |= (uint32)((((uint32)pMatch->vlanUserPriority) & MASK_3_BITS) << 4);
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MISC_MATCH(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= (uint32)(((uint32)(pMatch->sci[3]) & MASK_8_BITS) << 24);
        reg_data |= (uint32)(((uint32)(pMatch->sci[2]) & MASK_8_BITS) << 16);
        reg_data |= (uint32)(((uint32)(pMatch->sci[1]) & MASK_8_BITS) << 8);
        reg_data |= (uint32)(((uint32)(pMatch->sci[0]) & MASK_8_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_SCI_MATCH_LO(flow_index), reg_data);

        reg_data = 0;
        reg_data |= (uint32)(((uint32)(pMatch->sci[7]) & MASK_8_BITS) << 24);
        reg_data |= (uint32)(((uint32)(pMatch->sci[6]) & MASK_8_BITS) << 16);
        reg_data |= (uint32)(((uint32)(pMatch->sci[5]) & MASK_8_BITS) << 8);
        reg_data |= (uint32)(((uint32)(pMatch->sci[4]) & MASK_8_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_SCI_MATCH_HI(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= pMatch->matchMask;
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MASK(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= (uint32)((((uint32)pMatch->flow_index) & MASK_8_BITS) << 16);
        reg_data |= (uint32)((((uint32)pMatch->vlanUpInner) & MASK_3_BITS) << 12);
        reg_data |= (uint32)((((uint32)pMatch->vlanIdInner) & MASK_12_BITS));
        MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_EXT_MATCH(flow_index), reg_data);
    }

    return ret;
}

int32
phy_macsec_hw_flow_match_rule_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index, phy_macsec_flow_match_t *pMatch)
{
    int32 ret = RT_ERR_OK;
    uint32 reg = 0, reg_data = 0;
    uint16 tmp = 0;

    if (pMatch == NULL)
        return RT_ERR_INPUT;

    osal_memset(pMatch, 0, sizeof(phy_macsec_flow_match_t));

    reg = MACSEC_REG_SAM_MAC_SA_MATCH_LO(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->mac_sa[3] = (uint8)((reg_data >> 24) & MASK_8_BITS);
    pMatch->mac_sa[2] = (uint8)((reg_data >> 16) & MASK_8_BITS);
    pMatch->mac_sa[1] = (uint8)((reg_data >> 8)  & MASK_8_BITS);
    pMatch->mac_sa[0] = (uint8)((reg_data)       & MASK_8_BITS);

    reg = MACSEC_REG_SAM_MAC_SA_MATCH_HI(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    tmp = (uint16)((reg_data >> 16) & MASK_16_BITS);
    pMatch->etherType = ((tmp & 0xFF) << 8) | ((tmp >> 8) & 0xFF);
    pMatch->mac_sa[5] = (uint8)((reg_data >> 8)  & MASK_8_BITS);
    pMatch->mac_sa[4] = (uint8)((reg_data)       & MASK_8_BITS);

    reg = MACSEC_REG_SAM_MAC_DA_MATCH_LO(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->mac_da[3] = (uint8)((reg_data >> 24) & MASK_8_BITS);
    pMatch->mac_da[2] = (uint8)((reg_data >> 16) & MASK_8_BITS);
    pMatch->mac_da[1] = (uint8)((reg_data >> 8)  & MASK_8_BITS);
    pMatch->mac_da[0] = (uint8)((reg_data)       & MASK_8_BITS);

    reg = MACSEC_REG_SAM_MAC_DA_MATCH_HI(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->vlan_id   = (uint16)((reg_data >> 16) & MASK_12_BITS);
    pMatch->mac_da[5] = (uint8)((reg_data >> 8)   & MASK_8_BITS);
    pMatch->mac_da[4] = (uint8)((reg_data)        & MASK_8_BITS);

    reg = MACSEC_REG_SAM_MISC_MATCH(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->fVLANValid = (reg_data & BIT_0_IN32) ? 1 : 0;
    pMatch->fQinQFound = (reg_data & BIT_1_IN32) ? 1 : 0;
    pMatch->fSTagValid = (reg_data & BIT_2_IN32) ? 1 : 0;
    pMatch->fQTagFound = (reg_data & BIT_3_IN32) ? 1 : 0;

    pMatch->fControlPacket = (reg_data & BIT_7_IN32) ? 1 : 0;
    pMatch->fUntagged      = (reg_data & BIT_8_IN32) ? 1 : 0;
    pMatch->fTagged        = (reg_data & BIT_9_IN32) ? 1 : 0;
    pMatch->fBadTag        = (reg_data & BIT_10_IN32) ? 1 : 0;
    pMatch->fKayTag        = (reg_data & BIT_11_IN32) ? 1 : 0;

    pMatch->macsec_TCI_AN = (uint8)((reg_data >> 24) & MASK_8_BITS);
    pMatch->matchPriority = (uint8)((reg_data >> 16) & MASK_4_BITS);
    pMatch->sourcePort =    (uint8)((reg_data >> 12) & MASK_2_BITS);

    pMatch->vlanUserPriority = (uint8)((reg_data >> 4) & MASK_3_BITS);

    reg = MACSEC_REG_SAM_SCI_MATCH_LO(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->sci[3] = (uint8)((reg_data >> 24) & MASK_8_BITS);
    pMatch->sci[2] = (uint8)((reg_data >> 16) & MASK_8_BITS);
    pMatch->sci[1] = (uint8)((reg_data >> 8)  & MASK_8_BITS);
    pMatch->sci[0] = (uint8)((reg_data)       & MASK_8_BITS);

    reg = MACSEC_REG_SAM_SCI_MATCH_HI(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->sci[7] = (uint8)((reg_data >> 24) & MASK_8_BITS);
    pMatch->sci[6] = (uint8)((reg_data >> 16) & MASK_8_BITS);
    pMatch->sci[5] = (uint8)((reg_data >> 8)  & MASK_8_BITS);
    pMatch->sci[4] = (uint8)((reg_data)       & MASK_8_BITS);

    reg = MACSEC_REG_SAM_MASK(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->matchMask = reg_data;

    reg = MACSEC_REG_SAM_EXT_MATCH(flow_index);
    MACSEC_REG_GET(unit, port, dir, reg, &reg_data);
    MACSEC_DBG("[0x%04X] = 0x%08X\n", reg, reg_data);
    pMatch->flow_index  = (uint32) ((reg_data >> 16) & MASK_8_BITS);
    pMatch->vlanUpInner = (uint8)  ((reg_data >> 12) & MASK_3_BITS);
    pMatch->vlanIdInner = (uint16) ((reg_data) & MASK_12_BITS);

    return ret;
}

static int32
phy_macsec_hw_flow_match_rule_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index)
{
    int32  ret = RT_ERR_OK;
    uint32 reg_data = 0;

    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_SA_MATCH_LO(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_SA_MATCH_HI(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_DA_MATCH_LO(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MAC_DA_MATCH_HI(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MISC_MATCH(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_SCI_MATCH_LO(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_SCI_MATCH_HI(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_MASK(flow_index), reg_data);
    MACSEC_REG_SET(unit, port, dir, MACSEC_REG_SAM_EXT_MATCH(flow_index), reg_data);

    return ret;
}

int32
phy_macsec_hw_flow_enable_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index, rtk_enable_t *pEnable)
{
    int32  ret = RT_ERR_OK;
    uint32 reg_data;

    MACSEC_REG_GET(unit, port, dir, MACSEC_REG_SAM_ENTRY_ENABLE(flow_index/32), &reg_data);
    *pEnable = (reg_data & (BIT_0_IN32 << (flow_index % 32))) ? ENABLED : DISABLED;

    return ret;
}

static int32
phy_macsec_hw_flow_enable_set(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir, uint32 flow_index, rtk_enable_t enable)
{
    int32        ret = RT_ERR_OK;
    uint32       reg_data = 0;
    rtk_enable_t cur_ena;
    WAIT_COMPLETE_VAR();

    if ((ret = phy_macsec_hw_flow_enable_get(unit, port, dir, flow_index, &cur_ena)) != RT_ERR_OK)
    {
        return ret;
    }

    if (enable != cur_ena)
    {
        MACSEC_REG_SET(unit, port, dir, (enable == ENABLED)?
                   MACSEC_REG_SAM_ENTRY_SET(flow_index/32) : MACSEC_REG_SAM_ENTRY_CLEAR(flow_index/32),
                   BIT_0_IN32 << (flow_index % 32));

        if (enable == DISABLED)
        {
            WAIT_COMPLETE(10000000)
            {
                MACSEC_REG_GET(unit, port, dir, MACSEC_REG_SAM_IN_FLIGHT, &reg_data);
                if ((reg_data & 0x3F) == 0)
                    break;
            }
            if (WAIT_COMPLETE_IS_TIMEOUT())
            {
                ret = RT_ERR_BUSYWAIT_TIMEOUT;
                RT_ERR(ret, (MOD_HAL), "flow delete timeout. u%u, p%u, val=0x%x", unit, port, (reg_data & 0x3F));
                return ret;
            }
        }

    }

    return ret;
}

/* Function Name:
 *      phy_macsec_port_cfg_set
 * Description:
 *      Set per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      pPortcfg - pointer to macsec port configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_port_cfg_set(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg)
{
    int32 ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl;
    uint32 data = 0;
    uint32 xpn_thr_h = 0, xpn_thr_l = 0;
    uint8 drop_kay = 0;

    if (pPortcfg == NULL)
        return RT_ERR_INPUT;

    if(pPortcfg->pn_intr_threshold > 0xFFFFFFFF)
        return RT_ERR_INPUT;

    if(pPortcfg->xpn_intr_threshold > 0xFFFFFFFFFFFFFFFF)
        return RT_ERR_INPUT;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ctrl_set)
    {
        MACSEC_RET_CHK(pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ctrl_set(unit, port,
                   RTK_PHY_CTRL_MACSEC_BYPASS, (pPortcfg->flags & RTK_MACSEC_PORT_F_ENABLE) ? 0 : 1));
    }

    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS,  0x797C, &data);
    data &= (~(BIT_11_IN32 | BIT_10_IN32));
    switch (pPortcfg->nm_validate_frames)
    {
        case RTK_MACSEC_VALIDATE_STRICT:
            data |= BIT_11_IN32;
            break;
        case RTK_MACSEC_VALIDATE_CHECK:
            data |= BIT_10_IN32;
            break;
        case RTK_MACSEC_VALIDATE_DISABLE:
        default:
            break;
    }
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS,  0x797C, data);

    drop_kay = (pPortcfg->flags & RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY) ? (1) : (0);
    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_SAM_NM_FLOW_NCP, &data);
    data &= (~BIT_24_IN32);
    data |= (drop_kay) ? (BIT_24_IN32) : (0);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_SAM_NM_FLOW_NCP, data);

    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS,  MACSEC_REG_SAM_NM_FLOW_NCP, &data);
    data &= (~BIT_24_IN32);
    data |= (drop_kay) ? (BIT_24_IN32) : (0);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS,  MACSEC_REG_SAM_NM_FLOW_NCP, data);

    xpn_thr_h = (uint32)(pPortcfg->xpn_intr_threshold >> 32);
    xpn_thr_l = (uint32)(pPortcfg->xpn_intr_threshold & 0xFFFFFFFF);

    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF420, pPortcfg->pn_intr_threshold);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF424, xpn_thr_l);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF428, xpn_thr_h);

    return ret;
}

/* Function Name:
 *      phy_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_port_cfg_get(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg)
{
    int32 ret = RT_ERR_OK;
    hal_control_t   *pHalCtrl;
    uint32 data = 0;
    uint64 xpn_thr = 0;

    if (pPortcfg == NULL)
        return RT_ERR_INPUT;
    osal_memset(pPortcfg, 0x0, sizeof(rtk_macsec_port_cfg_t));

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ctrl_set)
    {
        if((ret = pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_ctrl_get(unit, port,
                   RTK_PHY_CTRL_MACSEC_BYPASS, &data)) != RT_ERR_OK)
            return ret;
    }
    if (data == 0)
        pPortcfg->flags |= RTK_MACSEC_PORT_F_ENABLE;

    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS,  MACSEC_REG_SAM_NM_FLOW_NCP, &data);
    if (data & BIT_24_IN32)
        pPortcfg->flags |= RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY;

    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS,  0x797C, &data);
    switch ((data & 0xC00) >> 10)
    {
        case 0b10:
            pPortcfg->nm_validate_frames = RTK_MACSEC_VALIDATE_STRICT;
            break;
        case 0b01:
            pPortcfg->nm_validate_frames = RTK_MACSEC_VALIDATE_CHECK;
            break;
        case 0b00:
        default:
            pPortcfg->nm_validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
            break;
    }


    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF420, &data);
    pPortcfg->pn_intr_threshold = data;

    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF424, &data);
    xpn_thr = (uint64) data;
    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF428, &data);
    xpn_thr |= ((uint64) data) << 32;
    pPortcfg->xpn_intr_threshold =xpn_thr;

    return ret;
}

/* Function Name:
 *      phy_macsec_sc_create
 * Description:
 *      Create a MACsec Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      pSc      - pointer to macsec sc configuration structure
 * Output:
 *      pSc_id   - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_EXCEEDS_CAPACITY
 * Note:
 *      None
 */
int32
phy_macsec_sc_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    rtk_macsec_sc_t *pSc, uint32 *pSc_id)
{
    int32 ret = RT_ERR_OK;
    uint8 an = 0, tci_an = 0;
    uint32 i = 0, sc_id = 0xFFFFFFFF;
    uint32 flow_base = 0;
    rtk_macsec_cipher_t cs = RTK_MACSEC_CIPHER_GCM_ASE_128;
    phy_macsec_flow_action_t flow;
    phy_macsec_flow_match_t match;
    phy_macsec_sa_params_t hwsa;

    if (pSc == NULL || pSc_id == NULL)
        return RT_ERR_INPUT;

    for (i = 0; i < MACSEC_SC_MAX(unit, port); i++)
    {
        if(MACSEC_SC_IS_CLEAR(unit, port, dir, i))
        {
            sc_id = i;
            break;
        }
    }
    if ( sc_id == 0xFFFFFFFF )
    {
        RT_ERR(RT_ERR_EXCEEDS_CAPACITY, (MOD_HAL|MOD_PHY), "no empty SC entry!");
        return RT_ERR_EXCEEDS_CAPACITY;
    }

    flow_base = PHY_MACSEC_HW_FLOW_ID(sc_id);

    osal_memset(&flow, 0, sizeof(phy_macsec_flow_action_t));
    osal_memset(&match, 0, sizeof(phy_macsec_flow_match_t));
    osal_memset(&hwsa, 0, sizeof(phy_macsec_sa_params_t));

    cs = (dir == RTK_MACSEC_DIR_EGRESS) ? (pSc->tx.cipher_suite) : (pSc->rx.cipher_suite);
    switch (cs)
    {
        case RTK_MACSEC_CIPHER_GCM_ASE_128:
            hwsa.flags = 0;
            hwsa.key_bytes = 16;
            if(pSc->rx.replay_window > 0xFFFFFFFF)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "PN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0xFFFFFFFF));
                return RT_ERR_INPUT;
            }
            break;
        case RTK_MACSEC_CIPHER_GCM_ASE_256:
            hwsa.flags = 0;
            hwsa.key_bytes = 32;
            if(pSc->rx.replay_window > 0xFFFFFFFF)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "PN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0xFFFFFFFF));
                return RT_ERR_INPUT;
            }
            break;
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
            hwsa.flags = RTK_PHY_MACSEC_SA_FLAG_XPN;
            hwsa.key_bytes = 16;
            if(pSc->rx.replay_window > 0x40000000)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "XPN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0x40000000));
                return RT_ERR_INPUT;
            }
            break;
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
            hwsa.flags = RTK_PHY_MACSEC_SA_FLAG_XPN;
            hwsa.key_bytes = 32;
            if(pSc->rx.replay_window > 0x40000000)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "XPN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0x40000000));
                return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        /* match rule */
        match.fUntagged = 1;
        match.fControlPacket = 0;
        switch (pSc->tx.flow_match)
        {
            case RTK_MACSEC_MATCH_NON_CTRL:
                match.matchMask = MACSEC_SA_MATCH_MASK_CTRL_PKT;
                break;

            case RTK_MACSEC_MATCH_MAC_DA:
                osal_memcpy(match.mac_da, pSc->tx.mac_da.octet, 6 * sizeof(uint8));
                match.matchMask = (MACSEC_SA_MATCH_MASK_MAC_DA_FULL
                    | MACSEC_SA_MATCH_MASK_CTRL_PKT);
                break;

            default:
                return RT_ERR_INPUT;
        }
        match.flow_index = flow_base;
        MACSEC_RET_CHK(phy_macsec_hw_flow_match_rule_set(unit, port, dir, flow_base, &match));
        MACSEC_SC_MATCH(unit, port, dir, sc_id) = pSc->tx.flow_match;

        /* flow ctrl */
        flow.flow_type = RTK_MACSEC_FLOW_EGRESS;
        flow.dest_port = RTK_MACSEC_PORT_COMMON;
        flow.sa_index = PHY_MACSEC_HW_SA_ID(sc_id, 0);
        flow.params.egress.protect_frame = pSc->tx.protect_frame;
        flow.params.egress.sa_in_use = 0;
        flow.params.egress.include_sci = pSc->tx.include_sci;
        flow.params.egress.use_es = pSc->tx.use_es;
        flow.params.egress.use_scb = pSc->tx.use_scb;
        flow.params.egress.confidentiality_offset = 0;
        flow.params.egress.conf_protect = pSc->tx.conf_protect;

        MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_set(unit, port, dir, flow_base, &flow));

        /* TC */
        hwsa.direction = dir;
        hwsa.flow_index = flow_base;
        hwsa.sa_expired_irq = 1;
        hwsa.update_en = 1;
        hwsa.next_sa_valid = 0;
        osal_memcpy(hwsa.sci, pSc->tx.sci, 8 * sizeof(uint8));
        for (an = 0; an < 4; an++)
        {
            hwsa.an = an;
            hwsa.next_sa_index = PHY_MACSEC_HW_SA_ID(sc_id, ((an + 1) % 4));
            MACSEC_RET_CHK(phy_macsec_hw_sa_set(unit, port, dir, PHY_MACSEC_HW_SA_ID(sc_id, an), &hwsa));
        }
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        /* match rule */
        match.fTagged = 1;
        match.fControlPacket = 0;
        osal_memcpy(match.sci, pSc->rx.sci, 8 * sizeof(uint8));
        switch (pSc->rx.flow_match)
        {
            case RTK_MACSEC_MATCH_SCI:
                tci_an = 0x20;
                match.matchMask = (MACSEC_SA_MATCH_MASK_MACSEC_SCI
                    | MACSEC_SA_MATCH_MASK_MACSEC_TCI_AN_SC
                    | MACSEC_SA_MATCH_MASK_CTRL_PKT);
                break;

            case RTK_MACSEC_MATCH_MAC_SA:
                tci_an = 0x0;
                osal_memcpy(match.mac_sa, pSc->rx.mac_sa.octet, 6 * sizeof(uint8));
                match.matchMask = (MACSEC_SA_MATCH_MASK_MAC_SA_FULL
                    | MACSEC_SA_MATCH_MASK_MACSEC_TCI_AN_SC
                    | MACSEC_SA_MATCH_MASK_CTRL_PKT);
                break;

            default:
                return RT_ERR_INPUT;
        }
        for (an = 0; an < 4; an++)
        {
            match.macsec_TCI_AN = an | tci_an;
            match.flow_index = flow_base + an;
            MACSEC_RET_CHK(phy_macsec_hw_flow_match_rule_set(unit, port, dir, (flow_base + an), &match));
        }
        MACSEC_SC_MATCH(unit, port, dir, sc_id) = pSc->rx.flow_match;

        /* flow ctrl */
        flow.flow_type = RTK_MACSEC_FLOW_INGRESS;
        flow.dest_port = RTK_MACSEC_PORT_CONTROLLED;

        flow.params.ingress.replay_protect = pSc->rx.replay_protect;
        flow.params.ingress.sa_in_use = 0;
        flow.params.ingress.validate_frames = pSc->rx.validate_frames;
        flow.params.ingress.confidentiality_offset = 0;

        for (an = 0; an < 4; an++)
        {
            flow.sa_index = PHY_MACSEC_HW_SA_ID(sc_id, an);
            MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_set(unit, port, dir, (flow_base + an), &flow));
        }

        /* TC */
        hwsa.direction = dir;
        hwsa.replay_window = pSc->rx.replay_window;
        osal_memcpy(hwsa.sci, pSc->rx.sci, 8 * sizeof(uint8));
        for (an = 0; an < 4; an++)
        {
            MACSEC_RET_CHK(phy_macsec_hw_sa_set(unit, port, dir, PHY_MACSEC_HW_SA_ID(sc_id, an), &hwsa));
        }
    }
    MACSEC_SC_CS(unit, port, dir, sc_id) = cs;
    MACSEC_SC_SET_USED(unit, port, dir, sc_id);
    *pSc_id = sc_id;
    return ret;
}

/* Function Name:
 *      phy_macsec_sc_get
 * Description:
 *      Get configuration info for a created Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_index - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND
 * Note:
 *      None
 */
int32
phy_macsec_sc_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_t *pSc)
{
    int32 ret = RT_ERR_OK;
    uint32 flow_base = 0, sa_base = 0;
    phy_macsec_flow_action_t flow;
    phy_macsec_flow_match_t match;
    phy_macsec_sa_params_t hwsa;

    if (pSc == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    osal_memset(pSc, 0, sizeof(rtk_macsec_sc_t));

    flow_base = PHY_MACSEC_HW_FLOW_ID(sc_id);
    sa_base = PHY_MACSEC_HW_SA_ID(sc_id, 0);

    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_base, &flow));
    MACSEC_RET_CHK(phy_macsec_hw_sa_get(unit, port, dir, sa_base, &hwsa));

    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        pSc->tx.cipher_suite = MACSEC_SC_CS(unit, port, dir, sc_id);
        pSc->tx.flow_match = MACSEC_SC_MATCH(unit, port, dir, sc_id);
        if (RTK_MACSEC_MATCH_MAC_DA == pSc->tx.flow_match)
        {
            MACSEC_RET_CHK(phy_macsec_hw_flow_match_rule_get(unit, port, dir, flow_base, &match));
            osal_memcpy(pSc->tx.mac_da.octet, match.mac_da, 6 * sizeof(uint8));
        }

        pSc->tx.protect_frame = flow.params.egress.protect_frame;
        pSc->tx.include_sci = flow.params.egress.include_sci;
        pSc->tx.use_es = flow.params.egress.use_es;
        pSc->tx.use_scb = flow.params.egress.use_scb;
        pSc->tx.conf_protect = flow.params.egress.conf_protect;

        osal_memcpy(pSc->tx.sci, hwsa.sci, 8 * sizeof(uint8));
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        pSc->rx.cipher_suite = MACSEC_SC_CS(unit, port, dir, sc_id);
        pSc->rx.flow_match = MACSEC_SC_MATCH(unit, port, dir, sc_id);
        MACSEC_RET_CHK(phy_macsec_hw_flow_match_rule_get(unit, port, dir, flow_base, &match));
        if (RTK_MACSEC_MATCH_MAC_SA == pSc->rx.flow_match)
        {
            osal_memcpy(pSc->rx.mac_sa.octet, match.mac_sa, 6 * sizeof(uint8));
        }
        osal_memcpy(pSc->rx.sci, match.sci, 8 * sizeof(uint8));

        pSc->rx.replay_protect = flow.params.ingress.replay_protect;
        pSc->rx.validate_frames = flow.params.ingress.validate_frames;

        pSc->rx.replay_window = hwsa.replay_window;
    }
    return ret;
}

/* Function Name:
 *      phy_macsec_sc_del
 * Description:
 *      Delete a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_sc_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id)
{
    int32 ret = RT_ERR_OK;
    uint8 an = 0;
    uint32 flow_base = 0, flow_id = 0;

    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
        return RT_ERR_OK;

    flow_base = PHY_MACSEC_HW_FLOW_ID(sc_id);

    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        flow_id = flow_base;

        MACSEC_RET_CHK(phy_macsec_hw_flow_enable_set(unit, port, dir, flow_id, DISABLED));
        MACSEC_RET_CHK(phy_macsec_hw_flow_match_rule_del(unit, port, dir, flow_id));
        MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_del(unit, port, dir, flow_id));

        for (an = 0; an < 4; an++)
        {
            MACSEC_RET_CHK(phy_macsec_hw_sa_del(unit,port,dir,PHY_MACSEC_HW_SA_ID(sc_id, an)));
            MACSEC_SA_UNSET_USED(unit, port, dir, PHY_MACSEC_HW_SA_ID(sc_id, an));
        }
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        for (an = 0; an < 4; an++)
        {
            flow_id = flow_base + an;
            MACSEC_RET_CHK(phy_macsec_hw_flow_enable_set(unit, port, dir, flow_id, DISABLED));
            MACSEC_RET_CHK(phy_macsec_hw_flow_match_rule_del(unit, port, dir, flow_id));
            MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_del(unit, port, dir, flow_id));
            MACSEC_RET_CHK(phy_macsec_hw_sa_del(unit, port, dir, PHY_MACSEC_HW_SA_ID(sc_id, an)));
            MACSEC_SA_UNSET_USED(unit, port, dir, PHY_MACSEC_HW_SA_ID(sc_id, an));
        }
    }

    MACSEC_SC_UNSET_USED(unit, port, dir, sc_id);
    return ret;
}

/* Function Name:
 *      phy_macsec_sc_status_get
 * Description:
 *      Get hardware status for a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_status - pointer to macsec SC status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_sc_status_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_status_t *pSc_status)
{
    int32 ret = RT_ERR_OK;
    phy_macsec_flow_action_t flow;
    uint32 flow_base = 0, flow_id = 0;
    uint32 flow_reg = 0, flow_data = 0;
    rtk_enable_t ena = DISABLED;
    rtk_macsec_an_t an;

    if (pSc_status == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    osal_memset(pSc_status, 0, sizeof(rtk_macsec_sc_status_t));

    flow_base = PHY_MACSEC_HW_FLOW_ID(sc_id);
    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_base, &flow));

    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_base, &flow));
        MACSEC_RET_CHK(phy_macsec_hw_flow_enable_get(unit, port, dir, flow_base, &ena));
        pSc_status->tx.hw_flow_index = flow_base;
        pSc_status->tx.hw_sa_index = flow.sa_index;
        pSc_status->tx.sa_inUse = flow.params.egress.sa_in_use;
        pSc_status->tx.hw_sc_flow_status = (ena == ENABLED) ? 1 : 0;
        pSc_status->tx.running_an = PHY_MACSEC_HW_SA_TO_AN(flow.sa_index);

        flow_reg = MACSEC_REG_SAM_FLOW_CTRL(flow_base);
        MACSEC_REG_GET(unit, port, dir, flow_reg, &flow_data);
        pSc_status->tx.hw_flow_data = flow_data;
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        pSc_status->rx.hw_flow_base = flow_base;
        for (an = RTK_MACSEC_AN0; an < RTK_MACSEC_AN_MAX; an++)
        {
            flow_id = flow_base + an;
            MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_id, &flow));
            MACSEC_RET_CHK(phy_macsec_hw_flow_enable_get(unit, port, dir, flow_id, &ena));
            pSc_status->rx.hw_sa_index[an] = flow.sa_index;
            pSc_status->rx.sa_inUse[an] = flow.params.ingress.sa_in_use;
            pSc_status->rx.hw_sc_flow_status[an] = (ena == ENABLED) ? 1 : 0;

            flow_reg = MACSEC_REG_SAM_FLOW_CTRL(flow_id);
            MACSEC_REG_GET(unit, port, dir, flow_reg, &flow_data);
            pSc_status->rx.hw_flow_data[an] = flow_data;
        }
    }

    return ret;
}


/* Function Name:
 *      phy_macsec_sa_activate
 * Description:
 *      Activate a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - Secure Channel id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      For egress, this function will change running SA.
 */
int32
phy_macsec_sa_activate(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an)
{
    int32 ret = RT_ERR_OK;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(sc_id, an);
    uint32 flow_id = 0;
    phy_macsec_flow_action_t flow;

    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(unit, port, dir, sa_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SA(SC %u, AN %u) is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        flow_id = PHY_MACSEC_HW_FLOW_ID(sc_id);
        MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_id, &flow));
        flow.sa_index = sa_id;
        flow.params.egress.sa_in_use = 1;
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        flow_id = PHY_MACSEC_HW_FLOW_ID(sc_id) + an;
        MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_id, &flow));
        flow.sa_index = sa_id;
        flow.params.ingress.sa_in_use = 1;
    }

    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_set(unit, port, dir, flow_id, &flow));
    MACSEC_RET_CHK(phy_macsec_hw_flow_enable_set(unit, port, dir, flow_id, ENABLED));
    return ret;
}

/* Function Name:
 *      phy_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association (inUse = 0)
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_rxsa_disable(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an)
{
    int32 ret = RT_ERR_OK;
    rtk_macsec_dir_t dir = RTK_MACSEC_DIR_INGRESS;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(rxsc_id, an);
    uint32 flow_id = PHY_MACSEC_HW_FLOW_ID(rxsc_id) + an;
    phy_macsec_flow_action_t flow;

    if (rxsc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, rxsc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX" ,rxsc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(unit, port, dir, rxsc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SA(SC %u, AN %u) is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX", rxsc_id, an);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_id, &flow));
    flow.params.ingress.sa_in_use = 0;
    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_set(unit, port, dir, flow_id, &flow));

    return ret;
}

/* Function Name:
 *      phy_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association (inUse = 0)
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_txsa_disable(uint32 unit, rtk_port_t port, uint32 txsc_id)
{
    int32 ret = RT_ERR_OK;
    rtk_macsec_dir_t dir = RTK_MACSEC_DIR_EGRESS;
    uint32 flow_id = PHY_MACSEC_HW_FLOW_ID(txsc_id);
    phy_macsec_flow_action_t flow;

    if (txsc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(unit, port, dir, txsc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",txsc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_get(unit, port, dir, flow_id, &flow));
    flow.params.egress.sa_in_use = 0;
    MACSEC_RET_CHK(phy_macsec_hw_flow_action_rule_set(unit, port, dir, flow_id, &flow));

    return ret;
}

/* Function Name:
 *      phy_macsec_sa_create
 * Description:
 *      Create a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 *      pSa      - pointer to macsec SA configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_sa_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa)
{
    int32 ret = RT_ERR_OK;
    phy_macsec_sa_params_t hwsa;
    rtk_macsec_cipher_t cs;
    rtk_macsec_sc_status_t sc_status;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(sc_id, an);
    uint32 post_sa_id = PHY_MACSEC_HW_SA_ID(sc_id, ((an + 3) % 4));
    uint8 flow_state_recover = 0;
    uint32 flow_id = 0;

    if (pSa == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    cs = MACSEC_SC_CS(unit, port, dir, sc_id);
    switch (cs)
    {
        case RTK_MACSEC_CIPHER_GCM_ASE_128:
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
            if (pSa->key_bytes != 16)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "Bad key_bytes:%u for AES-128.", pSa->key_bytes);
                return RT_ERR_INPUT;
            }
            break;

        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
        case RTK_MACSEC_CIPHER_GCM_ASE_256:
            if (pSa->key_bytes != 32)
            {
                RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "Bad key_bytes:%u for AES-256.", pSa->key_bytes);
                return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    MACSEC_RET_CHK(phy_macsec_sc_status_get(unit, port, dir, sc_id, &sc_status));
    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        /* disable flow for running AN */
        if ((sc_status.tx.hw_sc_flow_status == 1) && (sc_status.tx.running_an == an))
        {
            MACSEC_RET_CHK(phy_macsec_txsa_disable(unit, port, sc_id));
            flow_state_recover = 1;
        }

    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        if (sc_status.rx.hw_sc_flow_status[an] == 1)
        {
            MACSEC_RET_CHK(phy_macsec_rxsa_disable(unit, port, sc_id, an));
            flow_state_recover = 1;
        }
    }

    MACSEC_RET_CHK(phy_macsec_hw_sa_get(unit, port, dir, sa_id, &hwsa));
    switch (cs)
    {
        case RTK_MACSEC_CIPHER_GCM_ASE_128:
        case RTK_MACSEC_CIPHER_GCM_ASE_256:
            hwsa.key_bytes = pSa->key_bytes;
            hwsa.seq = pSa->pn;
            hwsa.seq_h = 0;
            osal_memset(hwsa.salt, 0x0, sizeof(uint8) * 12);
            osal_memset(hwsa.ssci, 0x0, sizeof(uint8) * 4);
            break;

        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
            hwsa.flags = RTK_PHY_MACSEC_SA_FLAG_XPN;
            hwsa.key_bytes = pSa->key_bytes;
            hwsa.seq = pSa->pn;
            hwsa.seq_h = pSa->pn_h;
            osal_memcpy(hwsa.salt, pSa->salt, sizeof(uint8) * 12);
            osal_memcpy(hwsa.ssci, pSa->ssci, sizeof(uint8) * 4);
            break;
        default:
            return RT_ERR_FAILED;
    }

    osal_memcpy(hwsa.key, pSa->key, sizeof(uint8) * hwsa.key_bytes);

    MACSEC_RET_CHK(phy_macsec_hw_sa_set(unit, port, dir, sa_id, &hwsa));

    if ((dir == RTK_MACSEC_DIR_EGRESS) && MACSEC_SA_IS_USED(unit, port, dir, post_sa_id))
    {
        MACSEC_RET_CHK(phy_macsec_hw_sa_get(unit, port, dir, post_sa_id, &hwsa));
        hwsa.next_sa_valid = 1;
        MACSEC_RET_CHK(phy_macsec_hw_sa_set(unit, port, dir, post_sa_id, &hwsa));
    }

    if (flow_state_recover == 1)
    {
        MACSEC_RET_CHK(phy_macsec_hw_flow_enable_set(unit, port, dir, flow_id, ENABLED));
    }

    MACSEC_SA_SET_USED(unit, port, dir, sa_id);
    return ret;
}

/* Function Name:
 *      phy_macsec_sa_get
 * Description:
 *      Get configuration info for a Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      pSa      - pointer to macsec SA configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_sa_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa)
{
    int32 ret = RT_ERR_OK;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(sc_id, an);
    phy_macsec_sa_params_t hwsa;

    if (pSa == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(unit, port, dir, sa_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SA(SC %u, AN %u) is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    osal_memset(pSa, 0x0, sizeof(rtk_macsec_sa_t));

    MACSEC_RET_CHK(phy_macsec_hw_sa_get(unit, port, dir, sa_id, &hwsa));

    pSa->key_bytes = hwsa.key_bytes;
    osal_memcpy(pSa->key, hwsa.key, sizeof(uint8) * hwsa.key_bytes);
    if (hwsa.flags & RTK_PHY_MACSEC_SA_FLAG_XPN)
    {
        pSa->pn = hwsa.seq;
        pSa->pn_h = hwsa.seq_h;
        osal_memcpy(pSa->salt, hwsa.salt, sizeof(uint8) * 12);
        osal_memcpy(pSa->ssci, hwsa.ssci, sizeof(uint8) * 4);
    }
    else /* PN */
    {
        pSa->pn = hwsa.seq;
        pSa->pn_h = hwsa.seq_h;
        osal_memcpy(pSa->salt, hwsa.salt, sizeof(uint8) * 12);
        osal_memcpy(pSa->ssci, hwsa.ssci, sizeof(uint8) * 4);
    }

    return ret;
}

/* Function Name:
 *      phy_macsec_sa_del
 * Description:
 *      Delete a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_sa_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an)
{
    int32 ret = RT_ERR_OK;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(sc_id, an);
    phy_macsec_sa_params_t hwsa;
    rtk_macsec_sc_status_t sc_status;

    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    MACSEC_RET_CHK(phy_macsec_sc_status_get(unit, port, dir, sc_id, &sc_status));
    if (dir == RTK_MACSEC_DIR_EGRESS)
    {
        /* disable flow for running AN */
        if ((sc_status.tx.hw_sc_flow_status == 1) && (sc_status.tx.running_an == an))
        {
            MACSEC_RET_CHK(phy_macsec_txsa_disable(unit, port, sc_id));
        }
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        if (sc_status.rx.hw_sc_flow_status[an] == 1)
        {
            MACSEC_RET_CHK(phy_macsec_rxsa_disable(unit, port, sc_id, an));
        }
    }

    MACSEC_RET_CHK(phy_macsec_hw_sa_get(unit, port, dir, sa_id, &hwsa));
    osal_memset(hwsa.key, 0, sizeof(uint8) * RTK_MACSEC_MAX_KEY_LEN);
    osal_memset(hwsa.salt, 0x0, sizeof(uint8) * 12);
    osal_memset(hwsa.ssci, 0x0, sizeof(uint8) * 4);
    hwsa.seq = 0;
    hwsa.seq_h = 0;
    MACSEC_RET_CHK(phy_macsec_hw_sa_set(unit, port, dir, sa_id, &hwsa));

    MACSEC_SA_UNSET_USED(unit, port, dir, sa_id);
    return ret;
}

/* Function Name:
 *      phy_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_stat_clear(uint32 unit, rtk_port_t port)
{
    int32 ret = RT_ERR_OK;
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_COUNT_CTRL, 0x00000001);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_COUNT_CTRL, 0x00000001);
    return ret;
}

/* Function Name:
 *      phy_macsec_stat_port_get
 * Description:
 *      get per-port statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
int32
phy_macsec_stat_port_get(uint32 unit, rtk_port_t port, rtk_macsec_stat_t stat,
    uint64 *pCnt)
{
    int32 ret = RT_ERR_OK;
    uint64 cnt_h = 0, cnt_l = 0;
    uint32 data= 0;

    if (pCnt == NULL)
        return RT_ERR_INPUT;

    switch (stat)
    {
        case RTK_MACSEC_STAT_InPktsUntagged:
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC418, &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC41C, &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsNoTag:
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC410, &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC414, &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsBadTag:
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC428, &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC42C, &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsUnknownSCI:
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC440, &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC444, &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsNoSCI:
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC438, &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_INGRESS, 0xC43C, &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_OutPktsUntagged:
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS, 0xC418, &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS, 0xC41C, &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}

/* Function Name:
 *      phy_macsec_stat_txsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
int32
phy_macsec_stat_txsa_get(uint32 unit, rtk_port_t port, uint32 sc_id,
    rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, uint64 *pCnt)
{
    int32 ret = RT_ERR_OK;
    uint64 cnt_h = 0, cnt_l = 0;
    uint32 data= 0, base = 0;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(sc_id, an);
    rtk_macsec_dir_t dir = RTK_MACSEC_DIR_EGRESS;

    if (pCnt == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(unit, port, dir, sa_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SA(SC %u, AN %u) is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    base = (sa_id * 0x80);
    switch (stat)
    {
        case RTK_MACSEC_TXSA_STAT_OutPktsTooLong:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8018), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x801C), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_TXSA_STAT_OutOctetsProtectedEncrypted:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8000), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8004), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_TXSA_STAT_OutPktsProtectedEncrypted:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8010), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8014), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return ret;
}

/* Function Name:
 *      phy_macsec_stat_rxsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
int32
phy_macsec_stat_rxsa_get(uint32 unit, rtk_port_t port, uint32 sc_id,
    rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, uint64 *pCnt)
{
    int32 ret = RT_ERR_OK;
    uint64 cnt_h = 0, cnt_l = 0;
    uint32 data= 0, base = 0;
    uint32 sa_id = PHY_MACSEC_HW_SA_ID(sc_id, an);
    rtk_macsec_dir_t dir = RTK_MACSEC_DIR_INGRESS;

    if (pCnt == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(unit, port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(unit, port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(unit, port, dir, sc_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SC %u is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(unit, port, dir, sa_id))
    {
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_HAL|MOD_PHY), "%s SA(SC %u, AN %u) is not existed!",
                (RTK_MACSEC_DIR_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    base = (sa_id * 0x80);
    switch (stat)
    {
        case RTK_MACSEC_RXSA_STAT_InPktsUnusedSA:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8048), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x804C), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsNotUsingSA:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8040), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8044), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsUnchecked:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8010), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8014), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsDelayed:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8018), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x801C), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsLate:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8020), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8024), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsOK:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8028), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x802C), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsInvalid:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8030), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8034), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsNotValid:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8038), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x803C), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InOctetsDecryptedValidated:
            MACSEC_REG_GET(unit, port, dir, (base + 0x8000), &data);
            cnt_l = (uint64)data;
            MACSEC_REG_GET(unit, port, dir, (base + 0x8004), &data);
            cnt_h = (uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}

/* Function Name:
 *      phy_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_macsec_intr_status_get(uint32 unit, rtk_port_t port,
    rtk_macsec_intr_status_t *pIntr_status)
{
    int32 ret = RT_ERR_OK;
    uint32 st_data = 0, exp_data = 0, thr_data = 0;
    uint32 i = 0, j = 0, sum_cnt = 0, sa_base = 0, sc_base = 0;;
    if (pIntr_status == NULL)
        return RT_ERR_INPUT;
    osal_memset(pIntr_status, 0x0, sizeof(rtk_macsec_intr_status_t));

    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS, 0xF80C, &st_data);

    if (st_data & 0x0200)
    {
        pIntr_status->status |= RTK_MACSEC_INTR_EGRESS_PN_ROLLOVER;
    }
    if (st_data & 0x0100)
    {
        pIntr_status->status |= RTK_MACSEC_INTR_EGRESS_PN_THRESHOLD;
    }

    sum_cnt = MACSEC_SA_MAX(unit, port)/ 32;

    for (i = 0; i < sum_cnt; i++) //0,1
    {
        MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS, (0xF000 + (4 * i)), &thr_data);
        MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS, (0x7A00 + (4 * i)), &exp_data);
        sa_base = i * 32; //0, 32
        sc_base = sa_base/4; //0, 8

        for (j = 0; j < 8; j++)
        {
            pIntr_status->egress_pn_thr_an_bmap[sc_base + j] = (thr_data >> (j * 4)) & 0xF;
            pIntr_status->egress_pn_exp_an_bmap[sc_base + j] = (exp_data >> (j * 4)) & 0xF;
        }

        MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS, (0xF000 + (4 * i)), 0xFFFF);
        MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS, (0x7A00 + (4 * i)), 0xFFFF);
    }

    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS, 0xF810, 0x03FF);
    return ret;
}

/* Function Name:
 *      phy_macsec_init
 * Description:
 *      PHY MACSec driver initialize
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
int32
phy_macsec_init(uint32 unit, rtk_port_t port)
{
    int32  ret = RT_ERR_OK;
    uint32 data = 0;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if (phy_macsec_info[unit] == NULL)
    {
        if ((phy_macsec_info[unit] = osal_alloc(sizeof(phy_macsec_info_t))) == NULL)
        {
            RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
        osal_memset(phy_macsec_info[unit], 0, sizeof(phy_macsec_info_t));
    }

    if (phy_macsec_info[unit]->port[port] == NULL)
    {
        if ((phy_macsec_info[unit]->port[port] = osal_alloc(sizeof(phy_macsec_port_info_t))) == NULL)
        {
            RT_ERR(RT_ERR_MEM_ALLOC, (MOD_HAL), "unit=%u,port=%u", unit, port);
            return RT_ERR_MEM_ALLOC;
        }
    }
    osal_memset(phy_macsec_info[unit]->port[port], 0, sizeof(phy_macsec_port_info_t));

    //fetch size info form PHY driver
    MACSEC_SA_MAX(unit, port) = pHalCtrl->pPhy_ctrl[port]->pPhyInfo->macsec_sa_num;

    //HW version check
    MACSEC_REG_GET(unit, port, RTK_MACSEC_DIR_EGRESS, 0xFFFC, &data);
    if ((data & MASK_8_BITS) != 160)
    {
        RT_ERR(RT_ERR_INPUT, (MOD_HAL|MOD_PHY), "u:%u p:%u bad MACSec HW ver 0x%08X", unit, port, data);
        return RT_ERR_FAILED;
    }

    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_COUNT_CTRL,     0x00000001);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_COUNT_CTRL,     0x0000000c);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_COUNT_SECFAIL1, 0x80fe0000);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_MISC_CONTROL,   0x02000046);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_CTX_CTRL,       0xe5880618);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_CTX_UPD_CTRL,   0x00000003);

    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_COUNT_CTRL,     0x00000001);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_COUNT_CTRL,     0x0000000c);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_COUNT_SECFAIL1, 0x80fe0000);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_MISC_CONTROL,   0x01001046);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_CTX_CTRL,       0xe5880614);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_CTX_UPD_CTRL,   0x00000003);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_SAM_CP_TAG,     0xe0fac688);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_IG_CC_CONTROL,  0x0000C000);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_SAM_NM_PARAMS,  0xe588003f);

    /* default Egress non-match pkt action:
       bypass: NCP - KaY tag; CP - Untagged
       drop: others
       */
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_SAM_NM_FLOW_NCP,    0x00010101);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  MACSEC_REG_SAM_NM_FLOW_CP,     0x01010100);
    /* default Ingress non-match pkt action:
       bypass: NCP - KaY tag,Tagged; CP - Untagged
       drop: others
       */
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_SAM_NM_FLOW_NCP,    0x08090809);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_INGRESS, MACSEC_REG_SAM_NM_FLOW_CP,     0x09090908);

    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF810,  0x000003ff);
    MACSEC_REG_SET(unit, port, RTK_MACSEC_DIR_EGRESS,  0xF808,  0x00000300);

    return ret;
}


