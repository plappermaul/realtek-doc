#if 0//yocto
#include <generated/ca_ne_autoconf.h>
#include <soc/cortina/registers.h>
#else//sd1 uboot
#include "ca_ne_autoconf.h"
#include "g3lite_registers.h"
#endif
#include "ca_types.h"
#include "osal_cmn.h"
#include "aal_common.h"
#include "aal_l2_tm.h"
#include "aal_l2_te.h"
#include "aal_l2_qm.h"


#if 0//yocto
#define CA_L2TE_REG_READ(addr)        CA_REG_READ(AAL_NE_REG_ADDR(addr))
#define CA_L2TE_REG_WRITE(addr, data) CA_REG_WRITE(data, AAL_NE_REG_ADDR(addr))
#else//sd1 uboot for 98f
#define CA_L2TE_REG_READ(addr)        CA_REG_READ((volatile uintptr_t)(addr))
#define CA_L2TE_REG_WRITE(addr, data) CA_REG_WRITE(data, (volatile uintptr_t)(addr))
#endif

aal_l2_te_wred_info_t g_aal_l2_te_wred_profile_info[CA_AAL_WRED_MAX_PROFILE_NUMS];
ca_uint8_t g_aal_l2_te_wred_sel_info[CA_AAL_MAX_PORT_ID + 1][8];

static ca_uint32_t g_mark_dp[8][CA_AAL_WRED_MAX_DROP_PROBABILTY] =
{
    {63, 63, 63, 63, 63, 57, 52, 46, 40, 34, 29, 23, 17, 11, 6, 0},
    {63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5, 0, 0},
    {63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5, 0, 0},
    {63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5, 0, 0},
    {63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5,  0, 0, 0},
    {63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5,  0, 0, 0},
    {63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5,  0,  0, 0, 0},
    {63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5,  0,  0, 0, 0}
};

static ca_uint32_t g_unmark_dp[8][CA_AAL_WRED_MAX_DROP_PROBABILTY] =
{
    {63, 63, 63, 63, 63, 63, 57, 52, 46, 40, 34, 29, 23, 17, 11, 6},
    {63, 63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5, 0},
    {63, 63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5, 0},
    {63, 63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5, 0},
    {63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5,  0, 0},
    {63, 63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11,  5,  0, 0},
    {63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11, 5,   0,  0, 0},
    {63, 63, 58, 53, 47, 42, 37, 32, 26, 21, 16, 11, 5,   0,  0, 0}

};

#ifdef __L2_TE_GLB
#endif


ca_status_t aal_l2_te_glb_cfg_set
(
    ca_device_id_t             device_id,
    aal_l2_te_glb_cfg_msk_t msk,
    aal_l2_te_glb_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_GLB_CTRL_t ctrl;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TE_GLB_CTRL;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (msk.s.policer_ena)
    {
        ctrl.bf.pol_en = cfg->policer_ena;
    }

    if (msk.s.shaper_ena)
    {
        ctrl.bf.shp_en = cfg->shaper_ena;
    }

    if (msk.s.tail_drop_ena)
    {
        ctrl.bf.taildrop_en = cfg->tail_drop_ena;
    }

    if (msk.s.wred_ena)
    {
        ctrl.bf.wred_en = cfg->wred_ena;
    }

    if (msk.s.mark_ecn_ena)
    {
        ctrl.bf.mark_ecn_en = cfg->mark_ecn_ena;
    }

    if (msk.s.pol_ecn_ena)
    {
        ctrl.bf.pol_ecn_en = cfg->pol_ecn_ena;
    }

    if (msk.s.wred_ecn_ena)
    {
        ctrl.bf.wred_ecn_en = cfg->wred_ecn_ena;
    }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_glb_cfg_get
(
    ca_device_id_t         device_id,
    aal_l2_te_glb_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_GLB_CTRL_t ctrl;


    if (cfg == NULL)
        return CA_E_PARAM;

    addr = L2TM_L2TE_GLB_CTRL;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->policer_ena   = ctrl.bf.pol_en;
    cfg->shaper_ena    = ctrl.bf.shp_en;
    cfg->tail_drop_ena = ctrl.bf.taildrop_en;
    cfg->wred_ena      = ctrl.bf.wred_en;
    cfg->mark_ecn_ena  = ctrl.bf.mark_ecn_en;
    cfg->pol_ecn_ena   = ctrl.bf.pol_ecn_en;
    cfg->wred_ecn_ena  = ctrl.bf.wred_ecn_en;

    return CA_E_OK;
}

ca_status_t aal_l2_te_status_get
(
    CA_IN  ca_device_id_t        device_id,
    CA_OUT aal_l2_te_status_t *status
)
{
    TE_TE_GLB_STS_t value;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_GLB_STS);

    status->qmio_enq_ovrn   = value.bf.qmio_enq_ovrn;
    status->qmio_deq_ovrn   = value.bf.qmio_deq_ovrn;
    status->qmio_enp_ovrn   = value.bf.qmio_enp_ovrn;
    status->qmio_dep_ovrn   = value.bf.qmio_dep_ovrn;
    status->qmio_shp_ovrn   = value.bf.qmio_shp_ovrn;
    status->te_qm_nrdy      = value.bf.te_qm_nrdy;
    status->pol_init_done   = value.bf.pol_init_done;
    status->shp_init_done   = value.bf.shp_init_done;
    status->cb_init_done    = value.bf.cb_init_done;
    status->wred_init_done  = value.bf.wred_init_done;
    status->feio_sm         = value.bf.feio_sm;
    status->fe_te_req       = value.bf.fe_te_req;
    status->te_fe_ack       = value.bf.te_fe_ack;
    status->qmio_idle       = value.bf.qmio_idle;
    status->init_done       = value.bf.init_done;

    return CA_E_OK;
}



#ifdef __L2_TE_SHAPER
#endif
static inline ca_status_t __l2_te_shaper_voq_tbc_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_SHP_VOQ_TBC_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_VOQ_TBC_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_VOQ_TBC_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_VOQ_TBC_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_VOQ_TBC_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_VOQ_TBC_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA1);
    }

    return CA_E_OK;

}


static inline ca_status_t __l2_te_shaper_port_tbc_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_SHP_PORT_TBC_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_TBC_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_TBC_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_PORT_TBC_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_PORT_TBC_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_PORT_TBC_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_TBC_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_TBC_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_TBC_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_TBC_MEM_DATA1);
    }

    return CA_E_OK;

}



static inline ca_status_t __l2_te_shaper_group_tbc_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_SHP_GROUP_TBC_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_GROUP_TBC_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_GROUP_TBC_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_GROUP_TBC_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_GROUP_TBC_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_GROUP_TBC_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA1);
    }

    return CA_E_OK;

}


ca_status_t aal_l2_te_shaper_srp_bmp_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  bmp
)
{
    L2TM_L2TE_SHP_SRP_CONFIG_EN_t value;

    if (port_id > CA_AAL_MAX_NETWORK_PORT_ID)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_SRP_CONFIG_EN);

    value.wrd &= ~(0xf << (port_id * 4));
    value.wrd |= (bmp & 0xf) << (port_id * 4);

    CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_SRP_CONFIG_EN, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_shaper_srp_bmp_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *bmp
)
{
    L2TM_L2TE_SHP_SRP_CONFIG_EN_t value;

    if (port_id > CA_AAL_MAX_NETWORK_PORT_ID)
        return CA_E_PARAM;

    if (!bmp)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_SRP_CONFIG_EN);

    *bmp = (value.wrd >> (port_id * 4)) & 0xf;

    return CA_E_OK;
}

ca_status_t aal_l2_te_shaper_srp_cfg_set
(
    ca_device_id_t                 device_id,
    ca_uint8_t                  srp_id,
    aal_l2_te_shaper_srp_cfg_msk_t msk,
    aal_l2_te_shaper_srp_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_SHP_SRP_CONFIG0_t ctrl;

    if (srp_id > CA_AAL_MAX_SRP_ID)
        return CA_E_PARAM;

    if (!cfg)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TE_SHP_SRP_CONFIG0 + (srp_id / 2  * 4);

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (srp_id % 2)
     {
         if (msk.s.ena)
         {
             ctrl.bf.cfg1_enb = cfg->ena;
         }

         if (msk.s.cos)
         {
             ctrl.bf.cfg1_cos = cfg->cos;
         }

         if (msk.s.class_type)
         {
             ctrl.bf.cfg1_class_b = (ca_uint32_t)cfg->class_type;
         }
     }
     else
     {
         if (msk.s.ena)
         {
             ctrl.bf.cfg0_enb = cfg->ena;
         }

         if (msk.s.cos)
         {
             ctrl.bf.cfg0_cos = cfg->cos;
         }

         if (msk.s.class_type)
         {
             ctrl.bf.cfg0_class_b = (ca_uint32_t)cfg->class_type;
         }

     }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_shaper_srp_cfg_get
(
    ca_device_id_t                 device_id,
    ca_uint8_t                  srp_id,
    aal_l2_te_shaper_srp_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_SHP_SRP_CONFIG0_t ctrl;

    if (srp_id > CA_AAL_MAX_SRP_ID)
        return CA_E_PARAM;

    if (!cfg)
        return CA_E_PARAM;

    addr = L2TM_L2TE_SHP_SRP_CONFIG0 + (srp_id / 2  * 4);

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (srp_id % 2)
    {
        cfg->ena        = ctrl.bf.cfg1_enb;
        cfg->cos        = ctrl.bf.cfg1_cos;
        cfg->class_type = (aal_l2_te_shaper_srp_class_t)ctrl.bf.cfg1_class_b;
    }
    else
    {
        cfg->ena        = ctrl.bf.cfg0_enb;
        cfg->cos        = ctrl.bf.cfg0_cos;
        cfg->class_type = (aal_l2_te_shaper_srp_class_t)ctrl.bf.cfg0_class_b;
    }

    return CA_E_OK;
}


ca_status_t aal_l2_te_shaper_ipq_set
(
    ca_device_id_t     device_id,
    ca_uint8_t      ipg_id,
    ca_uint16_t     ipg
)
{
    L2TM_L2TE_SHP_IPG_PROFILE_t value;

    if (ipg_id > CA_AAL_MAX_IPG_ID)
        return CA_E_PARAM;

    if (ipg > CA_AAL_MAX_IPG)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_IPG_PROFILE);

    switch (ipg_id)
    {
    case 0 :
        value.bf.ipg0 = ipg;
        break;
    case 1 :
        value.bf.ipg1 = ipg;
        break;
    case 2 :
        value.bf.ipg2 = ipg;
        break;
    case 3 :
        value.bf.ipg3 = ipg;
        break;
    default:
        break;
    }

    CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_IPG_PROFILE, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_shaper_ipq_get
(
    ca_device_id_t     device_id,
    ca_uint8_t      ipg_id,
    ca_uint16_t     *ipg
)
{
    L2TM_L2TE_SHP_IPG_PROFILE_t value;

    if (!ipg)
        return CA_E_PARAM;

    if (ipg_id > CA_AAL_MAX_IPG_ID)
        return CA_E_PARAM;

    if (*ipg > CA_AAL_MAX_IPG)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_IPG_PROFILE);

    switch (ipg_id)
    {
    case 0 :
        *ipg = value.bf.ipg0;
        break;
    case 1 :
        *ipg = value.bf.ipg1;
        break;
    case 2 :
        *ipg = value.bf.ipg2;
        break;
    case 3 :
        *ipg = value.bf.ipg3;
        break;
    default:
        break;
    }

    return CA_E_OK;
}


ca_status_t aal_l2_te_shaper_port_ipq_set
(
    ca_device_id_t     device_id,
    ca_port_id_t    port_id,
    ca_uint8_t      ipg_id
)
{
    ca_uint32_t val32 = ipg_id;
    L2TM_L2TE_SHP_PORT_IPG_SELECT0_t value;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (ipg_id > CA_AAL_MAX_IPG_ID)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_IPG_SELECT0);

    value.wrd &= ~(0x3 << (port_id * 2));
    value.wrd |= val32 << (port_id * 2);

    CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_PORT_IPG_SELECT0, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_shaper_port_ipq_get
(
    ca_device_id_t     device_id,
    ca_port_id_t    port_id,
    ca_uint8_t      *ipg_id)
{
    L2TM_L2TE_SHP_PORT_IPG_SELECT0_t value;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (!ipg_id)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_PORT_IPG_SELECT0);

    *ipg_id = (value.wrd >> (port_id * 2)) & 0x3;

    return CA_E_OK;
}



ca_status_t aal_l2_te_shaper_voq_tbc_set
(
    ca_device_id_t                    device_id,
    ca_uint32_t                    voq_id,
    aal_l2_te_shaper_tbc_cfg_msk_t msk,
    aal_l2_te_shaper_tbc_cfg_t     *cfg
)
{
    L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA0_t value0;
    L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = voq_id;

    if (!cfg)
        return CA_E_PARAM;

    if (voq_id > CA_AAL_MAX_VOQ_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_shaper_voq_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.rate_k)
    {
        if (cfg->rate_k > CA_AAL_L2_TE_SHP_MAX_RATE_K_VOQ)
            return CA_E_PARAM;

        value0.bf.rate_k = cfg->rate_k;
    }

    if (msk.s.rate_m)
    {
        if (cfg->rate_m > CA_AAL_L2_TE_SHP_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.rate_m = cfg->rate_m;
    }

    if (msk.s.bs)
    {
        if (cfg->bs > CA_AAL_L2_TE_SHP_MAX_BS || cfg->bs == 0)
            return CA_E_PARAM;

        if (cfg->mode == CA_AAL_SHAPER_MODE_BPS)
        {
            if (cfg->bs < ((cfg->rate_m + 511) >> 9))
                return CA_E_PARAM;
        }

        if (cfg->mode == CA_AAL_SHAPER_MODE_PPS)
        {
            if (cfg->bs < ((cfg->rate_m + 1) >> 2))
                return CA_E_PARAM;
        }

        value0.bf.bs = cfg->bs & 0x3f;
        value1.bf.bs = (cfg->bs >> 6) & 0x3f;
    }

    if (msk.s.state)
    {
        if (cfg->state >= CA_AAL_SHAPER_STATE_END)
            return CA_E_PARAM;

        value1.bf.enb = cfg->state;
    }

    if (msk.s.mode)
    {
        if (cfg->mode >= CA_AAL_SHAPER_MODE_END)
            return CA_E_PARAM;

        value1.bf.mode = cfg->mode;
    }

    if (msk.s.tbc_count)
    {
        value1.bf.tbc = cfg->tbc_count;
    }

    if (msk.s.pkt_mode)
    {
        if (cfg->pkt_mode >= CA_AAL_SHAPER_TBC_PKT_MODE_END)
            return CA_E_PARAM;

        value1.bf.pkt_mode_class_sel= cfg->pkt_mode;
    }


    /* update packet type based policing table */
    return __l2_te_shaper_voq_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}



ca_status_t aal_l2_te_shaper_voq_tbc_get
(
    ca_device_id_t          device_id,
    ca_uint32_t          voq_id,
    aal_l2_te_shaper_tbc_cfg_t *cfg
)
{
    L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA0_t value0;
    L2TM_L2TE_SHP_VOQ_TBC_MEM_DATA1_t value1;

    ca_uint32_t index = voq_id;

    if (!cfg)
        return CA_E_PARAM;

    if (voq_id > CA_AAL_MAX_VOQ_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_shaper_voq_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->rate_k    = value0.bf.rate_k;
    cfg->rate_m    = value0.bf.rate_m;
    cfg->bs        = ((value1.bf.bs & 0x3f) << 6) | value0.bf.bs;
    cfg->state     = (aal_l2_te_shaper_admin_state_t)value1.bf.enb;
    cfg->mode      = (aal_l2_te_shaper_counting_mode_t)value1.bf.mode;
    cfg->tbc_count = value1.bf.tbc;
    cfg->pkt_mode = value1.bf.pkt_mode_class_sel;

    return CA_E_OK;
}



ca_status_t aal_l2_te_shaper_port_tbc_set
(
    ca_device_id_t                 device_id,
    ca_port_id_t                port_id,
    aal_l2_te_shaper_tbc_cfg_msk_t msk,
    aal_l2_te_shaper_tbc_cfg_t     *cfg
)
{
    L2TM_L2TE_SHP_PORT_TBC_MEM_DATA0_t value0;
    L2TM_L2TE_SHP_PORT_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_shaper_port_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.rate_k)
    {
        if (cfg->rate_k > CA_AAL_L2_TE_SHP_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.rate_k = cfg->rate_k;
    }

    if (msk.s.rate_m)
    {
        if (cfg->rate_m > CA_AAL_L2_TE_SHP_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.rate_m = cfg->rate_m;
    }

    if (msk.s.bs)
    {
        if (cfg->bs > CA_AAL_L2_TE_SHP_MAX_BS || cfg->bs == 0)
            return CA_E_PARAM;

        if (cfg->mode == CA_AAL_SHAPER_MODE_BPS)
        {
            if (cfg->bs < ((cfg->rate_m + 511) >> 9))
                return CA_E_PARAM;
        }

        if (cfg->mode == CA_AAL_SHAPER_MODE_PPS)
        {
            if (cfg->bs < ((cfg->rate_m + 1) >> 2))
                return CA_E_PARAM;
        }

        value0.bf.bs = cfg->bs & 0x3f;
        value1.bf.bs = (cfg->bs >> 6) & 0x3f;
    }

    if (msk.s.state)
    {
        if (cfg->state >= CA_AAL_SHAPER_STATE_END)
            return CA_E_PARAM;

        value1.bf.enb = cfg->state;
    }

    if (msk.s.mode)
    {
        if (cfg->mode >= CA_AAL_SHAPER_MODE_END)
            return CA_E_PARAM;

        value1.bf.mode = cfg->mode;
    }

    if (msk.s.tbc_count)
    {
        value1.bf.tbc = cfg->tbc_count;
    }

    if (msk.s.pkt_mode)
    {
        if (cfg->pkt_mode >= CA_AAL_SHAPER_TBC_PKT_MODE_END)
            return CA_E_PARAM;

        value1.bf.pkt_mode_class_sel= cfg->pkt_mode;
    }

    /* update packet type based policing table */
    return __l2_te_shaper_port_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}



ca_status_t aal_l2_te_shaper_port_tbc_get
(
    ca_device_id_t                device_id,
    ca_port_id_t               port_id,
    aal_l2_te_shaper_tbc_cfg_t *cfg
)
{
    L2TM_L2TE_SHP_PORT_TBC_MEM_DATA0_t value0;
    L2TM_L2TE_SHP_PORT_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_shaper_port_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->rate_k = value0.bf.rate_k;
    cfg->rate_m = value0.bf.rate_m;
    cfg->bs     = ((value1.bf.bs & 0x3f) << 6) | value0.bf.bs;
    cfg->state  = (aal_l2_te_shaper_admin_state_t)value1.bf.enb;
    cfg->mode   = (aal_l2_te_shaper_counting_mode_t)value1.bf.mode;
    cfg->tbc_count = value1.bf.tbc;
    cfg->pkt_mode = value1.bf.pkt_mode_class_sel;

    return CA_E_OK;
}

ca_status_t aal_l2_te_shaper_group_tbc_set
(
    ca_device_id_t                    device_id,
    ca_uint32_t                    group_id,
    aal_l2_te_shaper_tbc_cfg_msk_t msk,
    aal_l2_te_shaper_tbc_cfg_t     *cfg
)
{
    L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA0_t value0;
    L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = group_id;

    if (!cfg)
        return CA_E_PARAM;

    if (group_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_shaper_group_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.rate_k)
    {
        if (cfg->rate_k > CA_AAL_L2_TE_SHP_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.rate_k = cfg->rate_k;
    }

    if (msk.s.rate_m)
    {
        if (cfg->rate_m > CA_AAL_L2_TE_SHP_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.rate_m = cfg->rate_m;
    }

    if (msk.s.bs)
    {
        if (cfg->bs > CA_AAL_L2_TE_SHP_MAX_BS || cfg->bs == 0)
            return CA_E_PARAM;

        if (cfg->mode == CA_AAL_SHAPER_MODE_BPS)
        {
            if (cfg->bs < ((cfg->rate_m + 511) >> 9))
                return CA_E_PARAM;
        }

        if (cfg->mode == CA_AAL_SHAPER_MODE_PPS)
        {
            if (cfg->bs < ((cfg->rate_m + 1) >> 2))
                return CA_E_PARAM;
        }

        value0.bf.bs = cfg->bs & 0x3f;
        value1.bf.bs = (cfg->bs >> 6) & 0x3f;
    }

    if (msk.s.state)
    {
        if (cfg->state >= CA_AAL_SHAPER_STATE_END)
            return CA_E_PARAM;

        value1.bf.enb = cfg->state;
    }

    if (msk.s.mode)
    {
        if (cfg->mode >= CA_AAL_SHAPER_MODE_END)
            return CA_E_PARAM;

        value1.bf.mode = cfg->mode;
    }

    if (msk.s.tbc_count)
    {
        value1.bf.tbc = cfg->tbc_count;
    }

    if (msk.s.pkt_mode)
    {
        if (cfg->pkt_mode >= CA_AAL_SHAPER_TBC_PKT_MODE_END)
            return CA_E_PARAM;

        value1.bf.pkt_mode_class_sel= cfg->pkt_mode;
    }

    /* update packet type based policing table */
    return __l2_te_shaper_group_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}



ca_status_t aal_l2_te_shaper_group_tbc_get
(
    ca_device_id_t                 device_id,
    ca_uint32_t                 group_id,
    aal_l2_te_shaper_tbc_cfg_t  *cfg
)
{
    L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA0_t value0;
    L2TM_L2TE_SHP_GROUP_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = group_id;

    if (!cfg)
        return CA_E_PARAM;

    if (group_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_shaper_group_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->rate_k = value0.bf.rate_k;
    cfg->rate_m = value0.bf.rate_m;
    cfg->bs     = ((value1.bf.bs & 0x3f) << 6) | value0.bf.bs;
    cfg->state  = (aal_l2_te_shaper_admin_state_t)value1.bf.enb;
    cfg->mode   = (aal_l2_te_shaper_counting_mode_t)value1.bf.mode;
    cfg->tbc_count = value1.bf.tbc;
    cfg->pkt_mode = value1.bf.pkt_mode_class_sel;

    return CA_E_OK;
}


#ifdef __TE_POL
#endif

/* local function body */
#ifdef __TE_POL_LOCAL_FUNC
#endif
static inline ca_status_t __l2_te_policer_pkt_type_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA1, *data1);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA1);
        *data2 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}


static inline ca_status_t __l2_te_policer_pkt_type_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
         counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
             counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;

}




static inline ca_status_t __l2_te_policer_port_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    L2TM_L2TE_POL_PORT_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA1, *data1);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA1);
        *data2 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}



static inline ca_status_t __l2_te_policer_port_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_POL_PORT_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_COUNTER_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_PORT_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_COUNTER_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_PORT_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;

}


static inline ca_status_t __l2_te_policer_flow_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA1, *data1);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA1);
        *data2 = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}


static inline ca_status_t __l2_te_policer_flow_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_POL_FLOW_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_COUNTER_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_FLOW_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_COUNTER_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_FLOW_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;

}



static inline ca_status_t __l2_te_policer_agr_flow_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA1, *data1);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA1);
        *data2 = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}



static inline ca_status_t __l2_te_policer_agr_flow_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;

}


ca_status_t aal_l2_te_token_bucket_cfg_get
(
    ca_device_id_t                  device_id,
    aal_l2_te_token_bucket_cfg_t *cfg
)
{
    ca_uint32_t addr = L2TM_L2TE_POL_TB_CTRL;
    L2TM_L2TE_POL_TB_CTRL_t ctrl;

    if (cfg == NULL)
        return CA_E_PARAM;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->addition_token_ena = ctrl.bf.addtok_en;
    cfg->sub_token_ena      = ctrl.bf.subtok_en;
    cfg->update_cnt_lmt     = ctrl.bf.upd_cnt_lmt;
    cfg->cycle_cnt_lmt      = ctrl.bf.cyc_cnt_lmt;
    cfg->last_update_cycle  = ctrl.bf.upd_cnt_lmt;

    return CA_E_OK;
}


#ifdef __TE_POL_PKT_TYPE
#endif
ca_status_t aal_l2_te_policer_pkt_type_cfg_set
(
    ca_device_id_t                device_id,
    aal_l2_te_policer_pkt_type_t pkt_type,
    aal_l2_te_policer_cfg_msk_t  msk,
    aal_l2_te_policer_cfg_t      *cfg)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_PKT_TYPE_CTRL0_t ctrl;

    if (pkt_type >= CA_AAL_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TE_POL_PKT_TYPE_CTRL0 + (ca_uint32_t)pkt_type * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (msk.s.type)
    {
        ctrl.bf.type = cfg->type;
    }

    if (msk.s.color_blind)
    {
        ctrl.bf.color_blind = cfg->color_blind;
    }

    if (msk.s.pol_update_mode)
    {
        ctrl.bf.pkt_mode = cfg->pol_update_mode;
    }

    if (msk.s.pkt_len_sel)
    {
        ctrl.bf.egr_len_sel = cfg->pkt_len_sel;
    }

    if (msk.s.ipg)
    {
        ctrl.bf.ipg = cfg->ipg;
    }

    if (msk.s.bypass_yellow)
    {
        ctrl.bf.bypass_yellow = cfg->bypass_yellow;
    }

    if (msk.s.bypass_red)
    {
        ctrl.bf.bypass_red = cfg->bypass_red;
    }

    if (msk.s.result_update_mode)
    {
        ctrl.bf.update_mode = cfg->result_update_mode;
    }

    if (msk.s.nest_level)
    {
        ctrl.bf.nest_level = cfg->nest_level;
    }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_pkt_type_cfg_get
(
    ca_device_id_t                   device_id,
    aal_l2_te_policer_pkt_type_t pkt_type,
    aal_l2_te_policer_cfg_t      *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_PKT_TYPE_CTRL0_t ctrl;

    if (pkt_type >= CA_AAL_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = L2TM_L2TE_POL_PKT_TYPE_CTRL0 + (ca_uint32_t)pkt_type * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->type            = ctrl.bf.type;
    cfg->color_blind     = ctrl.bf.color_blind;
    cfg->pol_update_mode = ctrl.bf.pkt_mode;
    cfg->pkt_len_sel     = ctrl.bf.egr_len_sel;
    cfg->ipg             = ctrl.bf.ipg;
    cfg->bypass_yellow   = ctrl.bf.bypass_yellow;
    cfg->bypass_red      = ctrl.bf.bypass_red;
    cfg->result_update_mode = ctrl.bf.update_mode;
    cfg->nest_level      = ctrl.bf.nest_level;

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_pkt_type_profile_set
(
    ca_device_id_t                      device_id,
    aal_l2_te_policer_pkt_type_t    pkt_type,
    aal_l2_te_policer_profile_msk_t msk,
    aal_l2_te_policer_profile_t     *cfg)
{
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = (ca_uint32_t)pkt_type;

    if (!cfg)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_pkt_type_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.cir_k)
    {
        if (cfg->cir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L2_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L2_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }

    /* update packet type based policing table */
    return __l2_te_policer_pkt_type_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}

ca_status_t aal_l2_te_policer_pkt_type_profile_get
(
    ca_device_id_t                   device_id,
    aal_l2_te_policer_pkt_type_t pkt_type,
    aal_l2_te_policer_profile_t  *cfg
)
{
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_PKT_TYPE_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = pkt_type;

    if (!cfg)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_pkt_type_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->cir_k = value0.bf.committed_rate_k;
    cfg->cir_m = value0.bf.committed_rate_m;
    cfg->cbs   = ((value1.bf.committed_bs & 0x3f) << 6) | value0.bf.committed_bs;
    cfg->pir_k = value1.bf.peak_rate_k;
    cfg->pir_m = value1.bf.peak_rate_m;
    cfg->pbs   = value2.bf.peak_bs;

    return CA_E_OK;
}

ca_status_t aal_l2_te_policer_pkt_type_stats_get
(
    ca_device_id_t                   device_id,
    aal_l2_te_policer_pkt_type_t pkt_type,
    aal_l2_te_policer_stats_t    *stats)
{
    L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_DATA0_t value0;
    L2TM_L2TE_POL_PKT_TYPE_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = (ca_uint32_t)pkt_type;

    if (!stats)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;


    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_pkt_type_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    stats->committed_tb_nums = value0.bf.committed_tbc;
    stats->peak_tb_nums      = (value0.bf.peak_tbc >> 21) | ((value1.bf.peak_tbc & 0x3ff) << 11);

    return CA_E_OK;
}

#ifdef __TE_POL_PORT
#endif

ca_status_t aal_l2_te_policer_port_cfg_set
(
    ca_device_id_t                  device_id,
    ca_port_id_t                 port_id,
    aal_l2_te_policer_cfg_msk_t msk,
    aal_l2_te_policer_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_PORT_CTRL0_t ctrl;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TE_POL_PORT_CTRL0 + port_id * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (msk.s.type)
    {
        ctrl.bf.type = cfg->type;
    }

    if (msk.s.color_blind)
    {
        ctrl.bf.color_blind = cfg->color_blind;
    }

    if (msk.s.pol_update_mode)
    {
        ctrl.bf.pkt_mode = cfg->pol_update_mode;
    }

    if (msk.s.pkt_len_sel)
    {
        ctrl.bf.egr_len_sel = cfg->pkt_len_sel;
    }

    if (msk.s.ipg)
    {
        ctrl.bf.ipg = cfg->ipg;
    }

    if (msk.s.bypass_yellow)
    {
        ctrl.bf.bypass_yellow = cfg->bypass_yellow;
    }

    if (msk.s.bypass_red)
    {
        ctrl.bf.bypass_red = cfg->bypass_red;
    }

    if (msk.s.result_update_mode)
    {
        ctrl.bf.update_mode = cfg->result_update_mode;
    }

    if (msk.s.nest_level)
    {
        ctrl.bf.nest_level = cfg->nest_level;
    }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_port_cfg_get
(
    ca_device_id_t              device_id,
    ca_port_id_t             port_id,
    aal_l2_te_policer_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_PORT_CTRL0_t ctrl;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = L2TM_L2TE_POL_PORT_CTRL0 + port_id * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->type            = ctrl.bf.type;
    cfg->color_blind     = ctrl.bf.color_blind;
    cfg->pol_update_mode = ctrl.bf.pkt_mode;
    cfg->pkt_len_sel     = ctrl.bf.egr_len_sel;
    cfg->ipg             = ctrl.bf.ipg;
    cfg->bypass_yellow   = ctrl.bf.bypass_yellow;
    cfg->bypass_red      = ctrl.bf.bypass_red;
    cfg->result_update_mode = ctrl.bf.update_mode;
    cfg->nest_level      = ctrl.bf.nest_level;

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_port_profile_set
(
    ca_device_id_t                      device_id,
    ca_port_id_t                     port_id,
    aal_l2_te_policer_profile_msk_t msk,
    aal_l2_te_policer_profile_t     *cfg
)
{
    L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_port_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.cir_k)
    {
        if (cfg->cir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L2_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L2_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }

    /* update packet type based policing table */
    return __l2_te_policer_port_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}



ca_status_t aal_l2_te_policer_port_profile_get
(
    ca_device_id_t                  device_id,
    ca_port_id_t                 port_id,
    aal_l2_te_policer_profile_t *cfg
)
{
    L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_PORT_PROFILE_MEM_DATA2_t value2;

    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id >= CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_port_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->cir_k = value0.bf.committed_rate_k;
    cfg->cir_m = value0.bf.committed_rate_m;
    cfg->cbs   = ((value1.bf.committed_bs & 0x3f) << 6) | value0.bf.committed_bs;
    cfg->pir_k = value1.bf.peak_rate_k;
    cfg->pir_m = value1.bf.peak_rate_m;
    cfg->pbs   = value2.bf.peak_bs;

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_port_stats_get
(
    ca_device_id_t                device_id,
    ca_port_id_t               port_id,
    aal_l2_te_policer_stats_t *stats
)
{
    L2TM_L2TE_POL_PORT_COUNTER_MEM_DATA0_t value0;
    L2TM_L2TE_POL_PORT_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = port_id;

    if (!stats)
        return CA_E_PARAM;

    if (port_id >= CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_port_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    stats->committed_tb_nums = value0.bf.committed_tbc;
    stats->peak_tb_nums      = (value0.bf.peak_tbc >> 21) | ((value1.bf.peak_tbc & 0x3ff) << 11);

    return CA_E_OK;
}

#ifdef __TE_POL_FLOW
#endif

ca_status_t aal_l2_te_policer_flow_cfg_set
(
    ca_device_id_t                 device_id,
    ca_flow_id_t                flow_id,
    aal_l2_te_policer_cfg_msk_t msk,
    aal_l2_te_policer_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_FLOW_GRP_CTRL0_t ctrl;

    if (flow_id > CA_AAL_MAX_FLOW_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TE_POL_FLOW_GRP_CTRL0 + (flow_id / 32) * 4;    /* 32 flows share one configuration */

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (msk.s.type)
    {
        ctrl.bf.type = cfg->type;
    }

    if (msk.s.color_blind)
    {
        ctrl.bf.color_blind = cfg->color_blind;
    }

    if (msk.s.pol_update_mode)
    {
        ctrl.bf.pkt_mode = cfg->pol_update_mode;
    }

    if (msk.s.pkt_len_sel)
    {
        ctrl.bf.egr_len_sel = cfg->pkt_len_sel;
    }

    if (msk.s.ipg)
    {
        ctrl.bf.ipg = cfg->ipg;
    }

    if (msk.s.bypass_yellow)
    {
        ctrl.bf.bypass_yellow = cfg->bypass_yellow;
    }

    if (msk.s.bypass_red)
    {
        ctrl.bf.bypass_red = cfg->bypass_red;
    }

    if (msk.s.result_update_mode)
    {
        ctrl.bf.update_mode = cfg->result_update_mode;
    }

    if (msk.s.nest_level)
    {
        ctrl.bf.nest_level = cfg->nest_level;
    }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_flow_cfg_get
(
    ca_device_id_t              device_id,
    ca_flow_id_t             flow_id,
    aal_l2_te_policer_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_FLOW_GRP_CTRL0_t ctrl;

    if (flow_id > CA_AAL_MAX_FLOW_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = L2TM_L2TE_POL_FLOW_GRP_CTRL0 + (flow_id / 32) * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->type            = ctrl.bf.type;
    cfg->color_blind     = ctrl.bf.color_blind;
    cfg->pol_update_mode = ctrl.bf.pkt_mode;
    cfg->pkt_len_sel     = ctrl.bf.egr_len_sel;
    cfg->ipg             = ctrl.bf.ipg;
    cfg->bypass_yellow   = ctrl.bf.bypass_yellow;
    cfg->bypass_red      = ctrl.bf.bypass_red;
    cfg->result_update_mode = ctrl.bf.update_mode;
    cfg->nest_level      = ctrl.bf.nest_level;

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_flow_profile_set
(
    ca_device_id_t                      device_id,
    ca_flow_id_t                     flow_id,
    aal_l2_te_policer_profile_msk_t msk,
    aal_l2_te_policer_profile_t     *cfg
)
{
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = flow_id;

    if (!cfg)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.cir_k)
    {
        if (cfg->cir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L2_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L2_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }


    /* update packet type based policing table */
    return __l2_te_policer_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}


ca_status_t aal_l2_te_policer_flow_profile_get
(
    ca_device_id_t                  device_id,
    ca_flow_id_t                 flow_id,
    aal_l2_te_policer_profile_t *cfg
)
{
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_FLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = flow_id;

    if (!cfg)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->cir_k = value0.bf.committed_rate_k;
    cfg->cir_m = value0.bf.committed_rate_m;
    cfg->cbs   = ((value1.bf.committed_bs & 0x3f) << 6) | value0.bf.committed_bs;
    cfg->pir_k = value1.bf.peak_rate_k;
    cfg->pir_m = value1.bf.peak_rate_m;
    cfg->pbs   = value2.bf.peak_bs;

    return CA_E_OK;
}

ca_status_t aal_l2_te_policer_flow_stats_get
(
    ca_device_id_t                device_id,
    ca_flow_id_t               flow_id,
    aal_l2_te_policer_stats_t *stats
)
{
    L2TM_L2TE_POL_FLOW_COUNTER_MEM_DATA0_t value0;
    L2TM_L2TE_POL_FLOW_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = flow_id;

    if (!stats)
        return CA_E_PARAM;

    if (flow_id >= CA_AAL_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_flow_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    stats->committed_tb_nums = value0.bf.committed_tbc;
    stats->peak_tb_nums      = (value0.bf.peak_tbc >> 21) | ((value1.bf.peak_tbc & 0x3ff) << 11);

    return CA_E_OK;
}


#ifdef __TE_POL_FLOW_GRP
#endif

/*
** AGR FLOW_ID   MAPPING TABLE         RELATED FLOW ID to POL_FLOW_GRP%d_MAP%e
** 0             POL_FLOW_GRP0_MAPA    0~31
** 1             POL_FLOW_GRP0_MAPB    0~31
** 2             POL_FLOW_GRP0_MAPC    0~31
** 3             POL_FLOW_GRP0_MAPD    0~31
** 4             POL_FLOW_GRP1_MAPA    32~63
** 5             POL_FLOW_GRP1_MAPB    32~63
** 6             POL_FLOW_GRP1_MAPC    32~63
** 7             POL_FLOW_GRP1_MAPD    32~63
** 8             POL_FLOW_GRP2_MAPA    64~95
** 9             POL_FLOW_GRP2_MAPB    64~95
** 10            POL_FLOW_GRP2_MAPC    64~95
** 11            POL_FLOW_GRP2_MAPD    64~95
** 12            POL_FLOW_GRP3_MAPA    96~127
** 13            POL_FLOW_GRP3_MAPB    96~127
** 14            POL_FLOW_GRP3_MAPC    96~127
** 15            POL_FLOW_GRP3_MAPD    96~127
** 16            POL_FLOW_GRP4_MAPA    128~159
** 17            POL_FLOW_GRP4_MAPB    128~159
** 18            POL_FLOW_GRP4_MAPC    128~159
** 19            POL_FLOW_GRP4_MAPD    128~159
** 20            POL_FLOW_GRP5_MAPA    160~191
** 21            POL_FLOW_GRP5_MAPB    160~191
** 22            POL_FLOW_GRP5_MAPC    160~191
** 23            POL_FLOW_GRP5_MAPD    160~191
** 24            POL_FLOW_GRP6_MAPA    192~223
** 25            POL_FLOW_GRP6_MAPB    192~223
** 26            POL_FLOW_GRP6_MAPC    192~223
** 27            POL_FLOW_GRP6_MAPD    192~223
** 28            POL_FLOW_GRP7_MAPA    224~255
** 29            POL_FLOW_GRP7_MAPB    224~255
** 30            POL_FLOW_GRP7_MAPC    224~255
** 31            POL_FLOW_GRP7_MAPD    224~255
*/

/*
ca_status_t aal_l2_te_policer_flow_grp_map_set(
    ca_device_id_t                    device_id,
    ca_uint32_t                    agr_flow_id,
    aal_l2_te_policer_grp_map_type_t grp_map_type,
    ca_uint32_t                    bmp
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_FLOW_GRP0_MAPA_t ctrl;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (grp_map_type > CA_AAL_POLICER_GRP_MAP_TYPE_END)
        return CA_E_PARAM;

    addr = L2TM_L2TE_POL_FLOW_GRP0_MAPA + agr_flow_id * 16 + (ca_uint32_t)grp_map_type * 4;

    ctrl.wrd = bmp;

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_policer_flow_grp_map_get(
    ca_device_id_t                    device_id,
    ca_uint32_t                    agr_flow_id,
    aal_l2_te_policer_grp_map_type_t grp_map_type,
    ca_uint32_t                    *bmp
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_FLOW_GRP0_MAPA_t ctrl;

    if (!bmp)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (grp_map_type > CA_AAL_POLICER_GRP_MAP_TYPE_END)
        return CA_E_PARAM;

    addr = L2TM_L2TE_POL_FLOW_GRP0_MAPA + agr_flow_id * 16 + (ca_uint32_t)grp_map_type * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    *bmp = ctrl.wrd;

    return CA_E_OK;
}
*/


ca_status_t aal_l2_te_policer_agr_flow_cfg_set
(
    ca_device_id_t                  device_id,
    ca_uint32_t                  agr_flow_id,
    aal_l2_te_policer_cfg_msk_t msk,
    aal_l2_te_policer_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_AGRFLOW_CTRL0_t ctrl;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TE_POL_AGRFLOW_CTRL0 + agr_flow_id * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (msk.s.type)
    {
        ctrl.bf.type = cfg->type;
    }

    if (msk.s.color_blind)
    {
        ctrl.bf.color_blind = cfg->color_blind;
    }

    if (msk.s.pol_update_mode)
    {
        ctrl.bf.pkt_mode = cfg->pol_update_mode;
    }

    if (msk.s.pkt_len_sel)
    {
        ctrl.bf.egr_len_sel = cfg->pkt_len_sel;
    }

    if (msk.s.ipg)
    {
        ctrl.bf.ipg = cfg->ipg;
    }

    if (msk.s.bypass_yellow)
    {
        ctrl.bf.bypass_yellow = cfg->bypass_yellow;
    }

    if (msk.s.bypass_red)
    {
        ctrl.bf.bypass_red = cfg->bypass_red;
    }

    if (msk.s.result_update_mode)
    {
        ctrl.bf.update_mode = cfg->result_update_mode;
    }

    if (msk.s.nest_level)
    {
        ctrl.bf.nest_level = cfg->nest_level;
    }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_policer_agr_flow_cfg_get
(
    ca_device_id_t              device_id,
    ca_uint32_t              agr_flow_id,
    aal_l2_te_policer_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TE_POL_AGRFLOW_CTRL0_t ctrl;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = L2TM_L2TE_POL_AGRFLOW_CTRL0 + agr_flow_id * 4;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->type            = ctrl.bf.type;
    cfg->color_blind     = ctrl.bf.color_blind;
    cfg->pol_update_mode = ctrl.bf.pkt_mode;
    cfg->pkt_len_sel     = ctrl.bf.egr_len_sel;
    cfg->ipg             = ctrl.bf.ipg;
    cfg->bypass_yellow   = ctrl.bf.bypass_yellow;
    cfg->bypass_red      = ctrl.bf.bypass_red;
    cfg->result_update_mode = ctrl.bf.update_mode;
    cfg->nest_level      = ctrl.bf.nest_level;

    return CA_E_OK;
}



ca_status_t aal_l2_te_policer_agr_flow_profile_set
(
    ca_device_id_t                      device_id,
    ca_uint32_t                      agr_flow_id,
    aal_l2_te_policer_profile_msk_t msk,
    aal_l2_te_policer_profile_t     *cfg
)
{
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = agr_flow_id;

    if (!cfg)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_agr_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.cir_k)
    {
        if (cfg->cir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L2_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L2_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L2_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L2_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }

    /* update packet type based policing table */
    return __l2_te_policer_agr_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}



ca_status_t aal_l2_te_policer_agr_flow_profile_get
(
    ca_device_id_t                  device_id,
    ca_uint32_t                  agr_flow_id,
    aal_l2_te_policer_profile_t *cfg
)
{
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_POL_AGRFLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = agr_flow_id;

    if (!cfg)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_agr_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->cir_k = value0.bf.committed_rate_k;
    cfg->cir_m = value0.bf.committed_rate_m;
    cfg->cbs   = ((value1.bf.committed_bs & 0x3f) << 6) | value0.bf.committed_bs;
    cfg->pir_k = value1.bf.peak_rate_k;
    cfg->pir_m = value1.bf.peak_rate_m;
    cfg->pbs   = value2.bf.peak_bs;

    return CA_E_OK;
}

ca_status_t aal_l2_te_policer_agr_flow_stats_get
(
    ca_device_id_t                device_id,
    ca_uint32_t                agr_flow_id,
    aal_l2_te_policer_stats_t *stats
)
{
    L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_DATA0_t value0;
    L2TM_L2TE_POL_AGRFLOW_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = agr_flow_id;

    if (!stats)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_policer_agr_flow_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    stats->committed_tb_nums = value0.bf.committed_tbc;
    stats->peak_tb_nums      = (value0.bf.peak_tbc >> 21) | ((value1.bf.peak_tbc & 0x3ff) << 11);

    return CA_E_OK;
}


#ifdef __L2_TE_WRED
#endif

#ifdef __L2_TE_WREDLOCAL_FUNC
#endif

static inline ca_status_t __l2_te_wred_free_global_mem_size_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0
)
{
    L2TM_L2TE_WRED_AFGS_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_AFGS_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_AFGS_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_AFGS_MEM_DATA, *data0);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_AFGS_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_AFGS_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_AFGS_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_AFGS_MEM_DATA);
    }

    return CA_E_OK;

}

static inline ca_status_t __l2_te_wred_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2,
    volatile ca_uint32_t *data3,
    volatile ca_uint32_t *data4,
    volatile ca_uint32_t *data5,
    volatile ca_uint32_t *data6,
    volatile ca_uint32_t *data7
)
{
    L2TM_L2TE_WRED_PROFILE_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA1, *data1);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA2, *data2);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA3, *data3);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA4, *data4);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA5, *data5);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA6, *data6);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_DATA7, *data7);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA1);
        *data2 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA2);
        *data3 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA3);
        *data4 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA4);
        *data5 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA5);
        *data6 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA6);
        *data7 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_MEM_DATA7);
    }

    return CA_E_OK;
}


static inline ca_status_t __l2_te_wred_profile_sel_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TE_WRED_PROFILE_SELECT_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA1);
    }

    return CA_E_OK;
}



ca_status_t aal_l2_te_wred_cfg_set
(
    ca_device_id_t           device_id,
    aal_l2_te_wred_cfg_msk_t msk,
    aal_l2_te_wred_cfg_t     *cfg
)
{
    ca_uint32_t addr = L2TM_L2TE_WRED_CTRL;
    L2TM_L2TE_WRED_CTRL_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    if (msk.s.fast_decay)
    {
        ctrl.bf.fast_decay = cfg->fast_decay ? 1 : 0;
    }

    if (msk.s.global_weight)
    {
        ctrl.bf.afgs_weight = cfg->global_weight;
    }

    CA_L2TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_wred_cfg_get
(
    ca_device_id_t       device_id,
    aal_l2_te_wred_cfg_t *cfg
)
{
    ca_uint32_t addr = L2TM_L2TE_WRED_CTRL;
    L2TM_L2TE_WRED_CTRL_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    ctrl.wrd = CA_L2TE_REG_READ(addr);

    cfg->fast_decay = ctrl.bf.fast_decay ? TRUE : FALSE;
    cfg->global_weight = ctrl.bf.afgs_weight;

    return CA_E_OK;
}

ca_status_t aal_l2_te_wred_global_free_mem_size_set
(
    ca_device_id_t  device_id,
    ca_uint32_t  pool_id,
    ca_uint32_t  size
)
{
    L2TM_L2TE_WRED_AFGS_MEM_DATA_t value0;
    ca_uint32_t index = pool_id;

    if (pool_id > 0)
        return CA_E_PARAM;

    if (size > CA_AAL_L2_BM_MAX_GLOBAL_BUFF_SIZE)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));

    value0.wrd = size;

    /* update table */
    return __l2_te_wred_free_global_mem_size_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd));
}

ca_status_t aal_l2_te_wred_global_free_mem_size_get
(
    ca_device_id_t  device_id,
    ca_uint32_t  pool_id,
    ca_uint32_t  *size
)
{
    L2TM_L2TE_WRED_AFGS_MEM_DATA_t value0;
    ca_uint32_t index = pool_id;

    if (!size)
        return CA_E_PARAM;

    if (pool_id > 0)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));

    /* read table */
    if (__l2_te_wred_free_global_mem_size_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd))
            != CA_E_OK)
            return CA_E_ERROR;

    *size = value0.wrd;

    return CA_E_OK;
}


ca_status_t aal_l2_te_wred_voq_buff_size_set
(
    ca_device_id_t device_id,
    ca_uint32_t voq_id,
    ca_uint32_t buff_size
)
{
    return CA_E_OK;
}

ca_status_t aal_l2_te_wred_voq_buff_size_get
(
    ca_device_id_t device_id,
    ca_uint32_t voq_id,
    ca_uint32_t *buff_size
)
{
    return CA_E_OK;
}


ca_status_t aal_l2_te_wred_port_free_buff_size_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  buff_size
)
{
    return CA_E_OK;
}

ca_status_t aal_l2_te_wred_port_free_buff_size_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *buff_size
)
{
    return CA_E_OK;
}


ca_status_t aal_l2_te_wred_profile_sel_set
(
    ca_device_id_t                   device_id,
    ca_uint32_t                   id,
    aal_l2_te_wred_profile_sel_msk_t msk,
    aal_l2_te_wred_profile_sel_t     *sel
)
{
    L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA0_t value0;
    L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA1_t value1;
    ca_uint32_t index = id;

    if (id > CA_AAL_WRED_MAX_PROFILE_SEL_ID)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    if (!sel)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_wred_profile_sel_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    if (msk.s.profile_id_0)
    {
        value0.bf.sel0 = sel->profile_id[0];
    }

    if (msk.s.profile_id_1)
    {
        value0.bf.sel1 = sel->profile_id[1];
    }

    if (msk.s.profile_id_2)
    {
        value0.bf.sel2 = sel->profile_id[2];
    }

    if (msk.s.profile_id_3)
    {
        value0.bf.sel3 = sel->profile_id[3];
    }

    if (msk.s.profile_id_4)
    {
        value0.bf.sel4 = sel->profile_id[4];
    }

    if (msk.s.profile_id_5)
    {
        value0.bf.sel5 = sel->profile_id[5];
    }

    if (msk.s.profile_id_6)
    {
        /* cos6 low 2 bits */
        value0.bf.sel6 = sel->profile_id[6] & 0x3;

        /* cos6 high 3 bits */
        value1.bf.sel6 = (sel->profile_id[6] >> 2) & 0x7;
    }

    if (msk.s.profile_id_7)
    {
        value1.bf.sel7 = sel->profile_id[7];
    }


    /* update WRED profile selection table */
    return __l2_te_wred_profile_sel_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}


ca_status_t aal_l2_te_wred_profile_sel_get
(
    ca_device_id_t               device_id,
    ca_uint32_t               id,
    aal_l2_te_wred_profile_sel_t *sel
)
{
    L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA0_t value0;
    L2TM_L2TE_WRED_PROFILE_SELECT_MEM_DATA1_t value1;
    ca_uint32_t index = id;

    if (id > CA_AAL_WRED_MAX_PROFILE_SEL_ID)
        return CA_E_PARAM;

    if (!sel)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));


    /* get current configuration at first and then update specificed field */
    if (__l2_te_wred_profile_sel_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    sel->profile_id[0] = value0.bf.sel0;
    sel->profile_id[1] = value0.bf.sel1;
    sel->profile_id[2] = value0.bf.sel2;
    sel->profile_id[3] = value0.bf.sel3;
    sel->profile_id[4] = value0.bf.sel4;
    sel->profile_id[5] = value0.bf.sel5;
    sel->profile_id[6] = (value1.bf.sel6 << 2) | value0.bf.sel6;
    sel->profile_id[7] = value1.bf.sel7;


    return CA_E_OK;
}


ca_status_t aal_l2_te_wred_profile_set
(
    ca_device_id_t               device_id,
    ca_uint32_t               id,
    aal_l2_te_wred_profile_msk_t msk,
    aal_l2_te_wred_profile_t     *profile
)
{
    L2TM_L2TE_WRED_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA2_t value2;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA3_t value3;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA4_t value4;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA5_t value5;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA6_t value6;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA7_t value7;

    ca_uint32_t index = id;

    if (id > CA_AAL_WRED_MAX_PROFILE_ID)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    if (!profile)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));


    /* get current configuration at first and then update specified field */
    if (__l2_te_wred_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    if (msk.s.unmark_dp)
    {
        value0.wrd = 0;
        value1.wrd = 0;
        value2.wrd = 0;

        value0.wrd |= (profile->unmark_dp[0]);
        value0.wrd |= (profile->unmark_dp[1] << 6);
        value0.wrd |= (profile->unmark_dp[2] << 12);
        value0.wrd |= (profile->unmark_dp[3] << 18);
        value0.wrd |= (profile->unmark_dp[4] << 24);
        value0.wrd |= ((profile->unmark_dp[5] & 0x3) << 30);
        value1.wrd |= ((profile->unmark_dp[5] >> 2) & 0xf);
        value1.wrd |= (profile->unmark_dp[6] << 4);
        value1.wrd |= (profile->unmark_dp[7] << 10);
        value1.wrd |= (profile->unmark_dp[8] << 16);
        value1.wrd |= (profile->unmark_dp[9] << 22);
        value1.wrd |= ((profile->unmark_dp[10] & 0xf) << 28);
        value2.wrd |= ((profile->unmark_dp[10] >> 4) & 0x3);
        value2.wrd |= (profile->unmark_dp[11] << 2);
        value2.wrd |= (profile->unmark_dp[12] << 8);
        value2.wrd |= (profile->unmark_dp[13] << 14);
        value2.wrd |= (profile->unmark_dp[14] << 20);
        value2.wrd |= (profile->unmark_dp[15] << 26);
    }

    if (msk.s.mark_dp)
    {
        value3.wrd = 0;
        value4.wrd = 0;
        value5.wrd = 0;

        value3.wrd |= (profile->mark_dp[0]);
        value3.wrd |= (profile->mark_dp[1] << 6);
        value3.wrd |= (profile->mark_dp[2] << 12);
        value3.wrd |= (profile->mark_dp[3] << 18);
        value3.wrd |= (profile->mark_dp[4] << 24);
        value3.wrd |= ((profile->mark_dp[5] & 0x3) << 30);
        value4.wrd |= ((profile->mark_dp[5] >> 2) & 0xf);
        value4.wrd |= (profile->mark_dp[6] << 4);
        value4.wrd |= (profile->mark_dp[7] << 10);
        value4.wrd |= (profile->mark_dp[8] << 16);
        value4.wrd |= (profile->mark_dp[9] << 22);
        value4.wrd |= ((profile->mark_dp[10] & 0xf) << 28);
        value5.wrd |= ((profile->mark_dp[10] >> 4) & 0x3);
        value5.wrd |= (profile->mark_dp[11] << 2);
        value5.wrd |= (profile->mark_dp[12] << 8);
        value5.wrd |= (profile->mark_dp[13] << 14);
        value5.wrd |= (profile->mark_dp[14] << 20);
        value5.wrd |= (profile->mark_dp[15] << 26);
    }

    if (msk.s.unmark_idx)
    {
        value6.bf.unmark_idx = profile->unmark_idx;
    }

    if (msk.s.mark_idx)
    {
        value6.bf.mark_idx = profile->mark_idx;
    }

    /* update WRED profile selection table */
    return __l2_te_wred_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd));
}


ca_status_t aal_l2_te_wred_profile_get
(
    ca_device_id_t           device_id,
    ca_uint32_t           id,
    aal_l2_te_wred_profile_t *profile
)
{
    L2TM_L2TE_WRED_PROFILE_MEM_DATA0_t value0;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA1_t value1;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA2_t value2;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA3_t value3;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA4_t value4;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA5_t value5;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA6_t value6;
    L2TM_L2TE_WRED_PROFILE_MEM_DATA7_t value7;

    ca_uint32_t index = id;

    if (id > CA_AAL_WRED_MAX_PROFILE_ID)
        return CA_E_PARAM;

    if (!profile)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));


    /* get current configuration at first and then update specificed field */
    if (__l2_te_wred_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    profile->unmark_dp[0] = value0.wrd & 0x3f;
    profile->unmark_dp[1] = (value0.wrd >> 6) & 0x3f;
    profile->unmark_dp[2] = (value0.wrd >> 12) & 0x3f;
    profile->unmark_dp[3] = (value0.wrd >> 18) & 0x3f;
    profile->unmark_dp[4] = (value0.wrd >> 24) & 0x3f;
    profile->unmark_dp[5] = ((value0.wrd >> 30) & 0x3) | ((value1.wrd & 0xf) << 2);
    profile->unmark_dp[6] = (value1.wrd >> 4) & 0x3f;
    profile->unmark_dp[7] = (value1.wrd >> 10) & 0x3f;
    profile->unmark_dp[8] = (value1.wrd >> 16) & 0x3f;
    profile->unmark_dp[9] = (value1.wrd >> 22) & 0x3f;
    profile->unmark_dp[10] = ((value1.wrd >> 28) & 0xf) | ((value2.wrd & 0x3) << 4);
    profile->unmark_dp[11] = (value2.wrd >> 2) & 0x3f;
    profile->unmark_dp[12] = (value2.wrd >> 8) & 0x3f;
    profile->unmark_dp[13] = (value2.wrd >> 14) & 0x3f;
    profile->unmark_dp[14] = (value2.wrd >> 20) & 0x3f;
    profile->unmark_dp[15] = (value2.wrd >> 26) & 0x3f;

    profile->mark_dp[0] = value3.wrd & 0x3f;
    profile->mark_dp[1] = (value3.wrd >> 6) & 0x3f;
    profile->mark_dp[2] = (value3.wrd >> 12) & 0x3f;
    profile->mark_dp[3] = (value3.wrd >> 18) & 0x3f;
    profile->mark_dp[4] = (value3.wrd >> 24) & 0x3f;
    profile->mark_dp[5] = ((value3.wrd >> 30) & 0x3) | ((value4.wrd & 0xf) << 2);
    profile->mark_dp[6] = (value4.wrd >> 4) & 0x3f;
    profile->mark_dp[7] = (value4.wrd >> 10) & 0x3f;
    profile->mark_dp[8] = (value4.wrd >> 16) & 0x3f;
    profile->mark_dp[9] = (value4.wrd >> 22) & 0x3f;
    profile->mark_dp[10] = ((value4.wrd >> 28) & 0xf) | ((value5.wrd & 0x3) << 4);
    profile->mark_dp[11] = (value5.wrd >> 2) & 0x3f;
    profile->mark_dp[12] = (value5.wrd >> 8) & 0x3f;
    profile->mark_dp[13] = (value5.wrd >> 14) & 0x3f;
    profile->mark_dp[14] = (value5.wrd >> 20) & 0x3f;
    profile->mark_dp[15] = (value5.wrd >> 26) & 0x3f;

    profile->unmark_idx  = value6.bf.unmark_idx;
    profile->mark_idx    = value6.bf.mark_idx;

    return CA_E_OK;
}


ca_boolean aal_l2_te_wred_info_lookup
(
    ca_uint32_t *marked_dp,
    ca_uint32_t *unmarked_dp,
    ca_uint32_t *index
)
{
    ca_uint32_t i = 0;

    if (!marked_dp)
        return FALSE;

    if (!unmarked_dp)
        return FALSE;

    if (!index)
        return FALSE;

    for (i=0; i<CA_AAL_WRED_MAX_PROFILE_NUMS; i++)
    {
        if (g_aal_l2_te_wred_profile_info[i].ref_counter == 0)
            continue;

        if (
            memcmp(g_aal_l2_te_wred_profile_info[i].marked_dp, marked_dp, CA_AAL_WRED_MAX_DROP_PROBABILTY * sizeof(ca_uint32_t)) == 0 &&
            memcmp(g_aal_l2_te_wred_profile_info[i].unmarked_dp, unmarked_dp, CA_AAL_WRED_MAX_DROP_PROBABILTY * sizeof(ca_uint32_t)) == 0
            )
        {
            *index = i;     /* hit, this profile entry can be reused */
            return TRUE;
        }

    }

    return FALSE;

}

ca_boolean aal_l2_te_wred_info_empty(void)
{
    ca_uint32_t zero_dp[CA_AAL_WRED_MAX_DROP_PROBABILTY];
    ca_uint32_t i = 0;

    memset((void *)&zero_dp[0], 0, sizeof(zero_dp));

    for (i=0; i<CA_AAL_WRED_MAX_PROFILE_NUMS; i++)
    {
        if (memcmp(g_aal_l2_te_wred_profile_info[i].marked_dp, zero_dp, sizeof(zero_dp)) ||
            memcmp(g_aal_l2_te_wred_profile_info[i].unmarked_dp, zero_dp, sizeof(zero_dp)))
        {
            return FALSE;
        }
    }

    return TRUE;
}



ca_boolean aal_l2_te_wred_profile_info_allocate
(
    ca_uint32_t *index
)
{
    ca_uint32_t i = 0;

    if (!index)
        return FALSE;

    for (i=0; i<CA_AAL_WRED_MAX_PROFILE_NUMS; i++)
    {
        if (g_aal_l2_te_wred_profile_info[i].ref_counter == 0)
        {
            *index = i;

            return TRUE;
        }
    }

    return FALSE;
}

ca_status_t aal_l2_te_wred_profile_info_set
(
    ca_uint32_t index,
    ca_uint32_t *marked_dp,
    ca_uint32_t *unmarked_dp
)
{
    if (index > CA_AAL_WRED_MAX_PROFILE_ID)
        return CA_E_PARAM;

    if (!marked_dp)
        return CA_E_PARAM;

    if (!unmarked_dp)
        return CA_E_PARAM;

    memcpy(g_aal_l2_te_wred_profile_info[index].marked_dp, marked_dp, sizeof(g_aal_l2_te_wred_profile_info[index].marked_dp));
    memcpy(g_aal_l2_te_wred_profile_info[index].unmarked_dp, unmarked_dp, sizeof(g_aal_l2_te_wred_profile_info[index].unmarked_dp));

    return CA_E_OK;
}

ca_status_t aal_l2_te_wred_profile_info_get
(
    ca_uint32_t index,
    ca_uint32_t *marked_dp,
    ca_uint32_t *unmarked_dp
)
{
    if (index > CA_AAL_WRED_MAX_PROFILE_ID)
        return CA_E_PARAM;

    if (!marked_dp)
        return CA_E_PARAM;

    if (!unmarked_dp)
        return CA_E_PARAM;

    memcpy(marked_dp, g_aal_l2_te_wred_profile_info[index].marked_dp, sizeof(g_aal_l2_te_wred_profile_info[index].marked_dp));
    memcpy(unmarked_dp, g_aal_l2_te_wred_profile_info[index].unmarked_dp, sizeof(g_aal_l2_te_wred_profile_info[index].unmarked_dp));

    return CA_E_OK;
}


ca_uint32_t aal_l2_te_wred_profile_info_ref_counter_get(ca_uint32_t index)
{
    return g_aal_l2_te_wred_profile_info[index].ref_counter;
}

void aal_l2_te_wred_profile_info_ref_inc(ca_uint32_t index)
{
    g_aal_l2_te_wred_profile_info[index].ref_counter++;
}

void aal_l2_te_wred_profile_info_ref_dec(ca_uint32_t index)
{
    if (g_aal_l2_te_wred_profile_info[index].ref_counter)
        g_aal_l2_te_wred_profile_info[index].ref_counter--;
}

void aal_l2_te_wred_sel_info_set
(
    ca_port_id_t port,
    ca_uint32_t  queue,
    ca_uint32    index
)
{
    if (port <= CA_AAL_MAX_PORT_ID && queue <= 7)
        g_aal_l2_te_wred_sel_info[port][queue] = index;
}

ca_uint8_t aal_l2_te_wred_sel_info_get
(
    ca_port_id_t port,
    ca_uint32_t  queue
)
{
    if (port <= CA_AAL_MAX_PORT_ID && queue <= 7)
        return g_aal_l2_te_wred_sel_info[port][queue];
    else
        return 0xff;
}

ca_status_t aal_l2_te_wred_debug_get(ca_device_id_t device_id)
{
    ca_uint32_t i = 0, j = 0;
    ca_int8_t str_marked[256] = {0};
    ca_int8_t str_unmarked[256] = {0};

    printk("============= S/W WRED profile table ============= \n");

    for (i=0; i<CA_AAL_WRED_MAX_PROFILE_NUMS; i++)
    {
        printk("index %02d : ref_counter(%d)\n", i, g_aal_l2_te_wred_profile_info[i].ref_counter);
        sprintf(str_marked, "  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            g_aal_l2_te_wred_profile_info[i].marked_dp[0], g_aal_l2_te_wred_profile_info[i].marked_dp[1],
            g_aal_l2_te_wred_profile_info[i].marked_dp[2], g_aal_l2_te_wred_profile_info[i].marked_dp[3],
            g_aal_l2_te_wred_profile_info[i].marked_dp[4], g_aal_l2_te_wred_profile_info[i].marked_dp[5],
            g_aal_l2_te_wred_profile_info[i].marked_dp[6], g_aal_l2_te_wred_profile_info[i].marked_dp[7],
            g_aal_l2_te_wred_profile_info[i].marked_dp[8], g_aal_l2_te_wred_profile_info[i].marked_dp[9],
            g_aal_l2_te_wred_profile_info[i].marked_dp[10], g_aal_l2_te_wred_profile_info[i].marked_dp[11],
            g_aal_l2_te_wred_profile_info[i].marked_dp[12], g_aal_l2_te_wred_profile_info[i].marked_dp[13],
            g_aal_l2_te_wred_profile_info[i].marked_dp[14], g_aal_l2_te_wred_profile_info[i].marked_dp[15]);
        sprintf(str_unmarked, "  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[0], g_aal_l2_te_wred_profile_info[i].unmarked_dp[1],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[2], g_aal_l2_te_wred_profile_info[i].unmarked_dp[3],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[4], g_aal_l2_te_wred_profile_info[i].unmarked_dp[5],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[6], g_aal_l2_te_wred_profile_info[i].unmarked_dp[7],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[8], g_aal_l2_te_wred_profile_info[i].unmarked_dp[9],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[10], g_aal_l2_te_wred_profile_info[i].unmarked_dp[11],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[12], g_aal_l2_te_wred_profile_info[i].unmarked_dp[13],
            g_aal_l2_te_wred_profile_info[i].unmarked_dp[14], g_aal_l2_te_wred_profile_info[i].unmarked_dp[15]);

        printk("%s", str_marked);
        printk("%s", str_unmarked);
    }

    printk("\n");

    printk("===== S/W WRED Selection table ===== \n");
    printk("          Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7\n");
    printk("          -- -- -- -- -- -- -- --\n");

    for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
    {
        printk("port %02d : ", i);
        for (j=0; j<8; j++)
        {
            printk("%02d ", g_aal_l2_te_wred_sel_info[i][j]);
        }
        printk("\n");
    }

    return CA_E_OK;
}



#ifdef __TE_POL_PM
#endif

static inline ca_status_t __l2_te_pm_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2,
    volatile ca_uint32_t *data3,
    volatile ca_uint32_t *data4,
    volatile ca_uint32_t *data5,
    volatile ca_uint32_t *data6,
    volatile ca_uint32_t *data7,
    volatile ca_uint32_t *data8
)
{
    L2TM_L2TE_PM_CNT_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA0, *data0);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA1, *data1);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA2, *data2);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA3, *data3);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA4, *data4);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA5, *data5);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA6, *data6);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA7, *data7);
        CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_DATA8, *data8);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CNT_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA0);
        *data1 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA1);
        *data2 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA2);
        *data3 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA3);
        *data4 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA4);
        *data5 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA5);
        *data6 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA6);
        *data7 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA7);
        *data8 = CA_L2TE_REG_READ(L2TM_L2TE_PM_CNT_MEM_DATA8);
    }

    return CA_E_OK;
}

ca_status_t aal_l2_te_pm_enable_set
(
    ca_device_id_t device_id,
    ca_boolean_t enable
)
{
    L2TM_L2TE_PM_CTRL_t value;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CTRL);

    value.bf.mem_disable = enable ? 0 : 1;

    CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_pm_enable_get
(
    ca_device_id_t device_id,
    ca_boolean_t *enable
)
{
    L2TM_L2TE_PM_CTRL_t value;

    if (!enable)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CTRL);

    *enable = value.bf.mem_disable ? FALSE : TRUE;

    return CA_E_OK;
}


ca_status_t aal_l2_te_pm_read_clr_set
(
    ca_device_id_t device_id,
    ca_boolean_t read_clr
)
{
    L2TM_L2TE_PM_CTRL_t value;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CTRL);

    value.bf.mem_auto_clr_on_read = read_clr;

    CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_te_pm_read_clr_get
(
    ca_device_id_t device_id,
    ca_boolean_t *read_clr
)
{
    L2TM_L2TE_PM_CTRL_t value;

    if (!read_clr)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CTRL);

    *read_clr = value.bf.mem_auto_clr_on_read ? TRUE :FALSE ;

    return CA_E_OK;
}

ca_status_t aal_l2_te_pm_cnt_mem_set
(
    ca_device_id_t device_id,
    ca_uint32_t block_id,
    ca_uint32_t sel_id,
    ca_uint32_t sel_event
)
{
    L2TM_L2TE_PM_CTRL_t value;

    if (block_id > 1)
        return CA_E_PARAM;

    if (sel_id > 1)
        return CA_E_PARAM;

    if (sel_event > 1)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CTRL);

    if (block_id == 0)
    {
        value.bf.cnt0_mem_id_sel    = sel_id;
        value.bf.cnt0_mem_event_sel = sel_event;
    }
    else
    {
        value.bf.cnt1_mem_id_sel    = sel_id;
        value.bf.cnt1_mem_event_sel = sel_event;
    }

    CA_L2TE_REG_WRITE(L2TM_L2TE_PM_CTRL, value.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_te_pm_cnt_mem_get
(
    ca_device_id_t device_id,
    ca_uint32_t block_id,
    ca_uint32_t *sel_id,
    ca_uint32_t *sel_event
)
{
    L2TM_L2TE_PM_CTRL_t value;

    if (block_id > 1)
        return CA_E_PARAM;

    if (!sel_id)
        return CA_E_PARAM;

    if (!sel_event)
        return CA_E_PARAM;

    value.wrd = CA_L2TE_REG_READ(L2TM_L2TE_PM_CTRL);

    if (block_id == 0)
    {
        *sel_id    = value.bf.cnt0_mem_id_sel;
        *sel_event = value.bf.cnt0_mem_event_sel;
    }
    else
    {
        *sel_id    = value.bf.cnt1_mem_id_sel;
        *sel_event = value.bf.cnt1_mem_event_sel;
    }

    return CA_E_OK;
}


/*
** addr : 0 ~ 255 flow, 256 ~ 263 agr flow, 384 ~ 399 source port, 400 ~ 415 packet type
*/
ca_status_t aal_l2_te_pm_policer_pkt_type_get(
    ca_device_id_t                device_id,
    aal_l2_te_policer_pkt_type_t pkt_type,
    aal_l2_te_pm_policer_t       *pm)
{
    L2TM_L2TE_PM_CNT_MEM_DATA0_t value0;
    L2TM_L2TE_PM_CNT_MEM_DATA1_t value1;
    L2TM_L2TE_PM_CNT_MEM_DATA2_t value2;
    L2TM_L2TE_PM_CNT_MEM_DATA3_t value3;
    L2TM_L2TE_PM_CNT_MEM_DATA4_t value4;
    L2TM_L2TE_PM_CNT_MEM_DATA5_t value5;
    L2TM_L2TE_PM_CNT_MEM_DATA6_t value6;
    L2TM_L2TE_PM_CNT_MEM_DATA7_t value7;
    L2TM_L2TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = (ca_uint32_t)pkt_type + CA_AAL_L2_TE_POL_PM_PKT_TYPE_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));
    memset((void *)&(value8), 0, sizeof(value8));


    /* get current configuration at first and then update specificed field */
    if (__l2_te_pm_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd),
            (volatile ca_uint32_t *)&(value8.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    pm->total_pkt  = value0.wrd;
    pm->red_pkt    = value1.wrd;
    pm->yellow_pkt = value2.wrd;
    pm->green_pkt  = value3.wrd;

    tmp_val1 = value5.wrd;
    pm->total_bytes      = value4.wrd | (tmp_val1 & 0xf) << 32;  /* total 36 bits */

    tmp_val1 = value5.wrd;
    tmp_val2 = value6.wrd;

    pm->red_bytes = (tmp_val1 >> 4) | ((tmp_val2 & 0xff) << 28);

    tmp_val1 = value6.wrd;
    tmp_val2 = value7.wrd;

    pm->yellow_bytes = (tmp_val1 >> 8) | ((tmp_val2 & 0xfff) << 24);

    tmp_val1 = value7.wrd;
    tmp_val2 = value8.wrd;
    pm->green_bytes = (tmp_val1 >> 12) | ((tmp_val2 & 0xffff) << 20);

    return CA_E_OK;

}


/*
** addr : 0 ~ 255 flow, 256 ~ 263 agr flow, 384 ~ 399 source port, 400 ~ 415 packet type
*/
ca_status_t aal_l2_te_pm_policer_port_get(
    ca_device_id_t               device_id,
    ca_port_id_t              port_id,
    aal_l2_te_pm_policer_t      *pm)
{
    L2TM_L2TE_PM_CNT_MEM_DATA0_t value0;
    L2TM_L2TE_PM_CNT_MEM_DATA1_t value1;
    L2TM_L2TE_PM_CNT_MEM_DATA2_t value2;
    L2TM_L2TE_PM_CNT_MEM_DATA3_t value3;
    L2TM_L2TE_PM_CNT_MEM_DATA4_t value4;
    L2TM_L2TE_PM_CNT_MEM_DATA5_t value5;
    L2TM_L2TE_PM_CNT_MEM_DATA6_t value6;
    L2TM_L2TE_PM_CNT_MEM_DATA7_t value7;
    L2TM_L2TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = port_id + CA_AAL_L2_TE_POL_PM_SRC_PORT_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));
    memset((void *)&(value8), 0, sizeof(value8));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_pm_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd),
            (volatile ca_uint32_t *)&(value8.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    pm->total_pkt  = value0.wrd;
    pm->red_pkt    = value1.wrd;
    pm->yellow_pkt = value2.wrd;
    pm->green_pkt  = value3.wrd;

    tmp_val1 = value5.wrd;
    pm->total_bytes      = value4.wrd | (tmp_val1 & 0xf) << 32;  /* total 36 bits */

    tmp_val1 = value5.wrd;
    tmp_val2 = value6.wrd;

    pm->red_bytes = (tmp_val1 >> 4) | ((tmp_val2 & 0xff) << 28);

    tmp_val1 = value6.wrd;
    tmp_val2 = value7.wrd;

    pm->yellow_bytes = (tmp_val1 >> 8) | ((tmp_val2 & 0xfff) << 24);

    tmp_val1 = value7.wrd;
    tmp_val2 = value8.wrd;
    pm->green_bytes = (tmp_val1 >> 12) | ((tmp_val2 & 0xffff) << 20);

    return CA_E_OK;

}


/*
** addr : 0 ~ 255 flow, 256 ~ 263 agr flow, 384 ~ 399 source port, 400 ~ 415 packet type
*/
ca_status_t aal_l2_te_pm_policer_flow_get(
    ca_device_id_t               device_id,
    ca_flow_id_t              flow_id,
    aal_l2_te_pm_policer_t      *pm)
{
    L2TM_L2TE_PM_CNT_MEM_DATA0_t value0;
    L2TM_L2TE_PM_CNT_MEM_DATA1_t value1;
    L2TM_L2TE_PM_CNT_MEM_DATA2_t value2;
    L2TM_L2TE_PM_CNT_MEM_DATA3_t value3;
    L2TM_L2TE_PM_CNT_MEM_DATA4_t value4;
    L2TM_L2TE_PM_CNT_MEM_DATA5_t value5;
    L2TM_L2TE_PM_CNT_MEM_DATA6_t value6;
    L2TM_L2TE_PM_CNT_MEM_DATA7_t value7;
    L2TM_L2TE_PM_CNT_MEM_DATA8_t value8;

    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = flow_id + CA_AAL_L2_TE_POL_PM_FLOW_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));
    memset((void *)&(value8), 0, sizeof(value8));


    /* get current configuration at first and then update specificed field */
    if (__l2_te_pm_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd),
            (volatile ca_uint32_t *)&(value8.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    pm->total_pkt  = value0.wrd;
    pm->red_pkt    = value1.wrd;
    pm->yellow_pkt = value2.wrd;
    pm->green_pkt  = value3.wrd;

    tmp_val1 = value5.wrd;
    pm->total_bytes      = value4.wrd | (tmp_val1 & 0xf) << 32;  /* total 36 bits */

    tmp_val1 = value5.wrd;
    tmp_val2 = value6.wrd;

    pm->red_bytes = (tmp_val1 >> 4) | ((tmp_val2 & 0xff) << 28);

    tmp_val1 = value6.wrd;
    tmp_val2 = value7.wrd;

    pm->yellow_bytes = (tmp_val1 >> 8) | ((tmp_val2 & 0xfff) << 24);

    tmp_val1 = value7.wrd;
    tmp_val2 = value8.wrd;
    pm->green_bytes = (tmp_val1 >> 12) | ((tmp_val2 & 0xffff) << 20);


    return CA_E_OK;
}



/*
** addr : 0 ~ 255 flow, 256 ~ 263 agr flow, 384 ~ 399 source port, 400 ~ 415 packet type
*/
ca_status_t aal_l2_te_pm_policer_agr_flow_get(
    ca_device_id_t               device_id,
    ca_uint32_t               agr_flow_id,
    aal_l2_te_pm_policer_t      *pm)
{
    L2TM_L2TE_PM_CNT_MEM_DATA0_t value0;
    L2TM_L2TE_PM_CNT_MEM_DATA1_t value1;
    L2TM_L2TE_PM_CNT_MEM_DATA2_t value2;
    L2TM_L2TE_PM_CNT_MEM_DATA3_t value3;
    L2TM_L2TE_PM_CNT_MEM_DATA4_t value4;
    L2TM_L2TE_PM_CNT_MEM_DATA5_t value5;
    L2TM_L2TE_PM_CNT_MEM_DATA6_t value6;
    L2TM_L2TE_PM_CNT_MEM_DATA5_t value7;
    L2TM_L2TE_PM_CNT_MEM_DATA6_t value8;

    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = 0;

    if (!pm)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    index = agr_flow_id + CA_AAL_L2_TE_POL_PM_AGR_FLOW_OFFSET;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));
    memset((void *)&(value8), 0, sizeof(value8));


    /* get current configuration at first and then update specificed field */
    if (__l2_te_pm_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd),
            (volatile ca_uint32_t *)&(value8.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    pm->total_pkt  = value0.wrd;
    pm->red_pkt    = value1.wrd;
    pm->yellow_pkt = value2.wrd;
    pm->green_pkt  = value3.wrd;

    tmp_val1 = value5.wrd;
    pm->total_bytes      = value4.wrd | (tmp_val1 & 0xf) << 32;  /* total 36 bits */

    tmp_val1 = value5.wrd;
    tmp_val2 = value6.wrd;

    pm->red_bytes = (tmp_val1 >> 4) | ((tmp_val2 & 0xff) << 28);

    tmp_val1 = value6.wrd;
    tmp_val2 = value7.wrd;

    pm->yellow_bytes = (tmp_val1 >> 8) | ((tmp_val2 & 0xfff) << 24);

    tmp_val1 = value7.wrd;
    tmp_val2 = value8.wrd;
    pm->green_bytes = (tmp_val1 >> 12) | ((tmp_val2 & 0xffff) << 20);

    return CA_E_OK;
}




/*
** addr : 0 ~ 255 flow, 256 ~ 263 agr flow, 384 ~ 399 source port, 400 ~ 415 packet type
*/
ca_status_t aal_l2_te_pm_egress_voq_get
(
    ca_device_id_t           device_id,
    ca_uint32_t           voq_id,
    aal_l2_te_pm_egress_t *pm
)
{
    L2TM_L2TE_PM_CNT_MEM_DATA0_t value0;
    L2TM_L2TE_PM_CNT_MEM_DATA1_t value1;
    L2TM_L2TE_PM_CNT_MEM_DATA2_t value2;
    L2TM_L2TE_PM_CNT_MEM_DATA3_t value3;
    L2TM_L2TE_PM_CNT_MEM_DATA4_t value4;
    L2TM_L2TE_PM_CNT_MEM_DATA5_t value5;
    L2TM_L2TE_PM_CNT_MEM_DATA6_t value6;
    L2TM_L2TE_PM_CNT_MEM_DATA7_t value7;
    L2TM_L2TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = voq_id + 512;  /* using mem block 1 */

    if (!pm)
        return CA_E_PARAM;

    if (voq_id > CA_AAL_MAX_VOQ_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));
    memset((void *)&(value3), 0, sizeof(value3));
    memset((void *)&(value4), 0, sizeof(value4));
    memset((void *)&(value5), 0, sizeof(value5));
    memset((void *)&(value6), 0, sizeof(value6));
    memset((void *)&(value7), 0, sizeof(value7));
    memset((void *)&(value8), 0, sizeof(value8));

    /* get current configuration at first and then update specificed field */
    if (__l2_te_pm_counter_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd),
            (volatile ca_uint32_t *)&(value3.wrd),
            (volatile ca_uint32_t *)&(value4.wrd),
            (volatile ca_uint32_t *)&(value5.wrd),
            (volatile ca_uint32_t *)&(value6.wrd),
            (volatile ca_uint32_t *)&(value7.wrd),
            (volatile ca_uint32_t *)&(value8.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    pm->total_pkt       = value0.wrd;
    pm->td_drop_pkt     = value1.wrd;
    pm->wred_mark_pkt   = value2.wrd;
    pm->wred_unmark_pkt = value3.wrd;

    tmp_val1 = value5.wrd;
    pm->total_bytes      = value4.wrd | (tmp_val1 & 0xf) << 32;  /* total 36 bits */

    tmp_val1 = value5.wrd;
    tmp_val2 = value6.wrd;

    pm->td_drop_bytes = (tmp_val1 >> 4) | ((tmp_val2 & 0xff) << 28);

    tmp_val1 = value6.wrd;
    tmp_val2 = value7.wrd;

    pm->wred_mark_bytes = (tmp_val1 >> 8) | ((tmp_val2 & 0xfff) << 24);

    tmp_val1 = value7.wrd;
    tmp_val2 = value8.wrd;
    pm->wred_unmark_bytes = (tmp_val1 >> 12) | ((tmp_val2 & 0xffff) << 20);

    return CA_E_OK;

}

ca_status_t aal_l2_te_pm_stats_get
(
    ca_device_id_t device_id,
    aal_l2_te_pm_stats_t *stats
)
{
    if (!stats)
        return CA_E_ERROR;

    stats->total_cnt            = CA_L2TE_REG_READ(L2TM_L2TE_PM_PCNT);
    stats->pre_mark_cnt         = CA_L2TE_REG_READ(L2TM_L2TE_PM_PRE_MARK_PCNT);
    stats->policer_drop_cnt     = CA_L2TE_REG_READ(L2TM_L2TE_PM_POL_DROP_PCNT);
    stats->policer_mark_cnt     = CA_L2TE_REG_READ(L2TM_L2TE_PM_MARK_PCNT);
    stats->tail_drop_cnt        = CA_L2TE_REG_READ(L2TM_L2TE_PM_TAIL_DROP_PCNT);
    stats->wred_yellow_drop_cnt = CA_L2TE_REG_READ(L2TM_L2TE_PM_WRED_Y_DROP_PCNT);
    stats->wred_green_drop_cnt  = CA_L2TE_REG_READ(L2TM_L2TE_PM_WRED_G_DROP_PCNT);
    stats->mark_cnt             = CA_L2TE_REG_READ(L2TM_L2TE_PM_MARK_PCNT);
    stats->bypass_cnt           = CA_L2TE_REG_READ(L2TM_L2TE_PM_BYPASS_PCNT);
    stats->bypass_flow_cnt      = CA_L2TE_REG_READ(L2TM_L2TE_PM_BYPASS_FLOW_PCNT);
    stats->ce_cnt               = CA_L2TE_REG_READ(L2TM_L2TE_PM_CE_PCNT);

    return CA_E_OK;
}


/*
** L2 TE module initialization function
*/
ca_status_t aal_l2_te_init
(
    ca_device_id_t device_id
)
{
    aal_l2_te_wred_cfg_t     wred_cfg;
    aal_l2_te_wred_cfg_msk_t wred_msk;
    aal_l2_te_glb_cfg_t      te_cfg;
    aal_l2_te_glb_cfg_msk_t  te_cfg_msk;
    aal_l2_te_policer_cfg_msk_t policer_msk;
    aal_l2_te_policer_cfg_t     policer_cfg;
    ca_uint32_t i = 0;
    ca_uint32_t j = 0;

    /* enable wred, policer, shaper, taildrop etc.*/
    memset((void *)&te_cfg, 0, sizeof(te_cfg));
    te_cfg_msk.u32 = 0;

    te_cfg_msk.s.policer_ena   = 1;
    te_cfg_msk.s.shaper_ena    = 1;
    te_cfg_msk.s.tail_drop_ena = 1;
    te_cfg_msk.s.wred_ena      = 1;
    te_cfg_msk.s.mark_ecn_ena  = 1;
    te_cfg_msk.s.pol_ecn_ena   = 1;
    te_cfg_msk.s.wred_ecn_ena  = 1;

#if 1//yocto
    te_cfg.policer_ena   = CA_TRUE;
    te_cfg.shaper_ena    = CA_TRUE;
    te_cfg.tail_drop_ena = CA_TRUE;
    te_cfg.wred_ena      = CA_TRUE;
#else//sd1 uboot for 98f
    te_cfg.policer_ena   = CA_FALSE;
    te_cfg.shaper_ena    = CA_FALSE;
    te_cfg.tail_drop_ena = CA_TRUE;
    te_cfg.wred_ena      = CA_FALSE;
#endif//sd1 uboot for 98f
    te_cfg.mark_ecn_ena  = CA_FALSE;
    te_cfg.pol_ecn_ena   = CA_FALSE;
    te_cfg.wred_ecn_ena  = CA_FALSE;

    (void)aal_l2_te_glb_cfg_set(device_id, te_cfg_msk, &te_cfg);

    /* shaper settings */
    #ifdef CONFIG_CORTINA_BOARD_FPGA
    /*
    ** For ASIC: ethernet clock: 125MHz, core clock: 200MHz;
    ** For FPGA: ethernet clock: 25MHz,  core clock: 25MHz
    ** So, shaper and policer token bucket update time needs to be adjusted for G3 FPGA.
    */
    do
    {
        /* shaper update */
        L2TM_L2TE_SHP_TB_CTRL_t ctrl;

        ctrl.wrd = CA_L2TE_REG_READ(L2TM_L2TE_SHP_TB_CTRL);

        ctrl.bf.upd_cnt_lmt = 3;

        CA_L2TE_REG_WRITE(L2TM_L2TE_SHP_TB_CTRL, ctrl.wrd);

    }while(0);

    do
    {   /* policer update */
        L2TM_L2TE_POL_TB_CTRL_t ctrl;

        ctrl.wrd = CA_L2TE_REG_READ(L2TM_L2TE_POL_TB_CTRL);

        ctrl.bf.upd_cnt_lmt = 3;

        CA_L2TE_REG_WRITE(L2TM_L2TE_POL_TB_CTRL, ctrl.wrd);
    } while(0);


    #endif

    /* policer settings */
    memset((void *)&policer_cfg, 0, sizeof(policer_cfg));
    policer_msk.u32 = 0;

    policer_msk.s.type            = 1;
    policer_msk.s.pol_update_mode = 1;
    policer_cfg.type              = CA_AAL_POLICER_TYPE_RFC2698;     /* two rates three colors */
    policer_cfg.pol_update_mode   = CA_AAL_POLICER_UPDATE_MODE_BYTE; /* bytes mode            */

    /* port based policer settings */
    for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
        (void)aal_l2_te_policer_port_cfg_set(device_id, i, policer_msk, &policer_cfg);

    /* packet type based policer settings */
    for (i=CA_AAL_POLICER_PKT_TYPE_UC; i<=CA_AAL_POLICER_PKT_TYPE_UBC; i++)
        (void)aal_l2_te_policer_pkt_type_cfg_set(device_id, i, policer_msk, &policer_cfg);

    /* flow based policer settings, each 32 flows share one configuration */
    for (i=0; i<=(CA_AAL_MAX_FLOW_ID / 32); i++)
        (void)aal_l2_te_policer_flow_cfg_set(device_id, i * 32, policer_msk, &policer_cfg);


    /* statistics counter settings */
    (void)aal_l2_te_pm_enable_set(device_id, TRUE);    /* enable performance monitor  */
    (void)aal_l2_te_pm_read_clr_set(device_id, TRUE);  /* clear counter after reading */

    /*
    ** mem block 0
    ** 000 ~ 255 : flow
    ** 256 ~ 263 : agr flow
    ** 384 ~ 399 : packet type
    ** 400 ~ 415 : aggregation flow
    */
    (void)aal_l2_te_pm_cnt_mem_set(device_id, 0, 0, 0);      /* mem0 for flow, port, type, aggregation flow policer_drop, mark, unmark */
    (void)aal_l2_te_pm_cnt_mem_set(device_id, 1, 1, 1);      /* mem1 for VoQ WRED mark drop, unmark drop */


    /* WRED initialization */
    memset((void *)&wred_cfg, 0, sizeof(wred_cfg));
    wred_msk.u32 = 0;

    /* reset WRED software table */
    memset((void *)&g_aal_l2_te_wred_profile_info[0], 0x0, sizeof(g_aal_l2_te_wred_profile_info));
    memset((void *)&g_aal_l2_te_wred_sel_info[0][0], 0xff, sizeof(g_aal_l2_te_wred_sel_info));

    /* WRED initialization */
    (void)aal_l2_te_wred_global_free_mem_size_set(device_id, 0, 0x1000); /* CA_AAL_L2_BM_MAX_GLOBAL_BUFF_SIZE, 1K buffer here */

    #if 0
    aal_l2_te_wred_profile_msk_t wred_profile_msk;
    aal_l2_te_wred_profile_t wred_profile;

    for (i=0; i<8; i++)
    {
        wred_profile_msk.u32 = 0;
        memset((void *)&wred_profile, 0, sizeof(wred_profile));

        wred_profile_msk.s.mark_dp    = 1;
        wred_profile_msk.s.unmark_dp  = 1;
        wred_profile_msk.s.mark_idx   = 1;
        wred_profile_msk.s.unmark_idx = 1;

        memcpy((void *)&wred_profile.mark_dp, &g_mark_dp[i][0], sizeof(wred_profile.mark_dp));
        memset((void *)&wred_profile.unmark_dp, 0x00, sizeof(wred_profile.unmark_dp));
        wred_profile.mark_idx   = 8;    /* 16 DP * 2^8 = 4096 buffers, drop range is 0 ~ 4095  */

        (void)aal_l2_te_wred_profile_set(device_id, i, wred_profile_msk, &wred_profile);
    }

    for (i=8; i<16; i++)
    {
        wred_profile_msk.u32 = 0;
        memset((void *)&wred_profile, 0, sizeof(wred_profile));

        wred_profile_msk.s.mark_dp    = 1;
        wred_profile_msk.s.unmark_dp  = 1;
        wred_profile_msk.s.mark_idx   = 1;
        wred_profile_msk.s.unmark_idx = 1;

        memcpy((void *)&wred_profile.unmark_dp, &g_unmark_dp[i][0], sizeof(wred_profile.unmark_dp));
        memset((void *)&wred_profile.mark_dp, 0x00, sizeof(wred_profile.mark_dp));

        wred_profile.unmark_idx   = 8;

        (void)aal_l2_te_wred_profile_set(device_id, i, wred_profile_msk, &wred_profile);
    }
    #endif

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("L2_TE : initialize WRED profile & selection table ... ");
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    do
    {
        /* setup WRED profile and selection table */
        ca_uint32_t index = 0;

        if (aal_l2_te_wred_profile_info_allocate(&index))
        {
            for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
            {
                for (j=0; j<8; j++)
                {
                    aal_l2_te_wred_profile_info_ref_inc(index); /* reference counter ++ */

                    /* update software selection table */
                    aal_l2_te_wred_sel_info_set(i, j, index);
                }
            }

#if CONFIG_98F_UBOOT_NE_DBG
            ca_printf("done.\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }
        else
        {
            ca_printf("%s:\tfailed.\n", __func__);
        }
    }while(0);

    return CA_E_OK;
}

#ifdef __KERNEL__
EXPORT_SYMBOL(aal_l2_te_glb_cfg_set);
EXPORT_SYMBOL(aal_l2_te_glb_cfg_get);
EXPORT_SYMBOL(aal_l2_te_status_get);

EXPORT_SYMBOL(aal_l2_te_shaper_srp_bmp_set);
EXPORT_SYMBOL(aal_l2_te_shaper_srp_bmp_get);
EXPORT_SYMBOL(aal_l2_te_shaper_srp_cfg_set);
EXPORT_SYMBOL(aal_l2_te_shaper_srp_cfg_get);
EXPORT_SYMBOL(aal_l2_te_shaper_ipq_set);
EXPORT_SYMBOL(aal_l2_te_shaper_ipq_get);
EXPORT_SYMBOL(aal_l2_te_shaper_port_ipq_set);
EXPORT_SYMBOL(aal_l2_te_shaper_port_ipq_get);
EXPORT_SYMBOL(aal_l2_te_shaper_voq_tbc_set);
EXPORT_SYMBOL(aal_l2_te_shaper_voq_tbc_get);
EXPORT_SYMBOL(aal_l2_te_shaper_port_tbc_set);
EXPORT_SYMBOL(aal_l2_te_shaper_port_tbc_get);
EXPORT_SYMBOL(aal_l2_te_shaper_group_tbc_set);
EXPORT_SYMBOL(aal_l2_te_shaper_group_tbc_get);

EXPORT_SYMBOL(aal_l2_te_policer_pkt_type_cfg_set);
EXPORT_SYMBOL(aal_l2_te_policer_pkt_type_cfg_get);
EXPORT_SYMBOL(aal_l2_te_policer_pkt_type_profile_set);
EXPORT_SYMBOL(aal_l2_te_policer_pkt_type_profile_get);
EXPORT_SYMBOL(aal_l2_te_policer_pkt_type_stats_get);

EXPORT_SYMBOL(aal_l2_te_policer_port_cfg_set);
EXPORT_SYMBOL(aal_l2_te_policer_port_cfg_get);
EXPORT_SYMBOL(aal_l2_te_policer_port_profile_set);
EXPORT_SYMBOL(aal_l2_te_policer_port_profile_get);
EXPORT_SYMBOL(aal_l2_te_policer_port_stats_get);

EXPORT_SYMBOL(aal_l2_te_policer_flow_cfg_set);
EXPORT_SYMBOL(aal_l2_te_policer_flow_cfg_get);
EXPORT_SYMBOL(aal_l2_te_policer_flow_profile_set);
EXPORT_SYMBOL(aal_l2_te_policer_flow_profile_get);
EXPORT_SYMBOL(aal_l2_te_policer_flow_stats_get);

EXPORT_SYMBOL(aal_l2_te_policer_agr_flow_cfg_set);
EXPORT_SYMBOL(aal_l2_te_policer_agr_flow_cfg_get);
EXPORT_SYMBOL(aal_l2_te_policer_agr_flow_profile_set);
EXPORT_SYMBOL(aal_l2_te_policer_agr_flow_profile_get);
EXPORT_SYMBOL(aal_l2_te_policer_agr_flow_stats_get);

EXPORT_SYMBOL(aal_l2_te_wred_cfg_set);
EXPORT_SYMBOL(aal_l2_te_wred_cfg_get);
EXPORT_SYMBOL(aal_l2_te_wred_global_free_mem_size_set);
EXPORT_SYMBOL(aal_l2_te_wred_global_free_mem_size_get);
EXPORT_SYMBOL(aal_l2_te_wred_profile_sel_set);
EXPORT_SYMBOL(aal_l2_te_wred_profile_sel_get);
EXPORT_SYMBOL(aal_l2_te_wred_profile_set);
EXPORT_SYMBOL(aal_l2_te_wred_profile_get);

EXPORT_SYMBOL(aal_l2_te_wred_info_lookup);
EXPORT_SYMBOL(aal_l2_te_wred_profile_info_allocate);
EXPORT_SYMBOL(aal_l2_te_wred_profile_info_set);
EXPORT_SYMBOL(aal_l2_te_wred_profile_info_get);
EXPORT_SYMBOL(aal_l2_te_wred_profile_info_ref_counter_get);
EXPORT_SYMBOL(aal_l2_te_wred_profile_info_ref_inc);
EXPORT_SYMBOL(aal_l2_te_wred_profile_info_ref_dec);
EXPORT_SYMBOL(aal_l2_te_wred_sel_info_set);
EXPORT_SYMBOL(aal_l2_te_wred_sel_info_get);
EXPORT_SYMBOL(aal_l2_te_wred_debug_get);


EXPORT_SYMBOL(aal_l2_te_pm_enable_set);
EXPORT_SYMBOL(aal_l2_te_pm_enable_get);
EXPORT_SYMBOL(aal_l2_te_pm_read_clr_set);
EXPORT_SYMBOL(aal_l2_te_pm_read_clr_get);
EXPORT_SYMBOL(aal_l2_te_pm_cnt_mem_set);
EXPORT_SYMBOL(aal_l2_te_pm_cnt_mem_get);
EXPORT_SYMBOL(aal_l2_te_pm_policer_pkt_type_get);
EXPORT_SYMBOL(aal_l2_te_pm_policer_port_get);
EXPORT_SYMBOL(aal_l2_te_pm_policer_flow_get);
EXPORT_SYMBOL(aal_l2_te_pm_policer_agr_flow_get);
EXPORT_SYMBOL(aal_l2_te_pm_egress_voq_get);
EXPORT_SYMBOL(aal_l2_te_pm_stats_get);

EXPORT_SYMBOL(aal_l2_te_wred_voq_buff_size_set);
EXPORT_SYMBOL(aal_l2_te_wred_voq_buff_size_get);
EXPORT_SYMBOL(aal_l2_te_token_bucket_cfg_get);
EXPORT_SYMBOL(aal_l2_te_init);
#endif

