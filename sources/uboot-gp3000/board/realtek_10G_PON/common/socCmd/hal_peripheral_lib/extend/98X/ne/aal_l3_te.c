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
#include "aal_l3_te.h"
#include "aal_l3_tm.h"
#include "scfg.h"

#define CA_L3TE_REG_READ(addr)        CA_REG_READ(AAL_NE_REG_ADDR(addr))
#define CA_L3TE_REG_WRITE(addr, data) CA_REG_WRITE(data, AAL_NE_REG_ADDR(addr))

aal_l3_te_wred_info_t g_aal_l3_te_wred_profile_info[CA_AAL_L3_WRED_MAX_PROFILE_NUMS];
ca_uint8_t g_aal_l3_te_wred_sel_info[CA_AAL_L3_WRED_MAX_PORT_ID + 1][8];

ca_uint32_t l3_te_wred_mark_idx = 0;
ca_uint32_t l3_te_wred_unmark_idx = 0;

#ifdef __L3_TE_GLB
#endif

ca_status_t aal_l3_te_glb_cfg_set
(
    ca_device_id_t             device_id,
    aal_l3_te_glb_cfg_msk_t msk,
    aal_l3_te_glb_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    TE_TE_GLB_CTRL_t ctrl;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = TE_TE_GLB_CTRL;

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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

    if (msk.s.l2_tail_drop_ena)
    {
        ctrl.bf.l2taildrop_en = cfg->l2_tail_drop_ena;
    }

    CA_L3TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l3_te_glb_cfg_get
(
    ca_device_id_t         device_id,
    aal_l3_te_glb_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    TE_TE_GLB_CTRL_t ctrl;


    if (cfg == NULL)
        return CA_E_PARAM;

    addr = TE_TE_GLB_CTRL;

    ctrl.wrd = CA_L3TE_REG_READ(addr);

    cfg->policer_ena   = ctrl.bf.pol_en;
    cfg->shaper_ena    = ctrl.bf.shp_en;
    cfg->tail_drop_ena = ctrl.bf.taildrop_en;
    cfg->wred_ena      = ctrl.bf.wred_en;
    cfg->mark_ecn_ena  = ctrl.bf.mark_ecn_en;
    cfg->pol_ecn_ena   = ctrl.bf.pol_ecn_en;
    cfg->wred_ecn_ena  = ctrl.bf.wred_ecn_en;
    cfg->l2_tail_drop_ena = ctrl.bf.l2taildrop_en;

    return CA_E_OK;
}


ca_status_t aal_l3_te_status_get
(
    CA_IN  ca_device_id_t        device_id,
    CA_OUT aal_l3_te_status_t *status
)
{
    TE_TE_GLB_STS_t value;

    value.wrd = CA_L3TE_REG_READ(TE_TE_GLB_STS);

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


#ifdef __L3_SHAPER
#endif
static inline ca_status_t __l3_te_shaper_voq_tbc_access(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_SHP_VOQ_TBC_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L3TE_REG_READ(TE_SHP_VOQ_TBC_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L3TE_REG_READ(TE_SHP_VOQ_TBC_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_SHP_VOQ_TBC_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_SHP_VOQ_TBC_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_SHP_VOQ_TBC_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L3TE_REG_READ(TE_SHP_VOQ_TBC_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L3TE_REG_READ(TE_SHP_VOQ_TBC_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_SHP_VOQ_TBC_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_SHP_VOQ_TBC_MEM_DATA1);
    }

    return CA_E_OK;

}


static inline ca_status_t __l3_te_shaper_port_tbc_access(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_SHP_PORT_TBC_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L3TE_REG_READ(TE_SHP_PORT_TBC_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L3TE_REG_READ(TE_SHP_PORT_TBC_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_SHP_PORT_TBC_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_SHP_PORT_TBC_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_SHP_PORT_TBC_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L3TE_REG_READ(TE_SHP_PORT_TBC_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L3TE_REG_READ(TE_SHP_PORT_TBC_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_SHP_PORT_TBC_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_SHP_PORT_TBC_MEM_DATA1);
    }

    return CA_E_OK;
}

static inline ca_status_t __l3_te_shaper_group_tbc_access(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_SHP_GROUP_TBC_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L3TE_REG_READ(TE_SHP_GROUP_TBC_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L3TE_REG_READ(TE_SHP_GROUP_TBC_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_SHP_GROUP_TBC_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_SHP_GROUP_TBC_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_SHP_GROUP_TBC_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L3TE_REG_READ(TE_SHP_GROUP_TBC_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L3TE_REG_READ(TE_SHP_GROUP_TBC_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_SHP_GROUP_TBC_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_SHP_GROUP_TBC_MEM_DATA1);
    }

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_ni_port_group_set(
    ca_device_id_t  device_id,
    ca_boolean_t enable)
{
    TE_SHP_CTRL_t value;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_CTRL);

    value.bf.ni_port_grp_en = enable ? 1 : 0;

    CA_L3TE_REG_WRITE(TE_SHP_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_ni_port_group_get(
    ca_device_id_t  device_id,
    ca_boolean_t *enable)
{
    TE_SHP_CTRL_t value;

    if (!enable)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_CTRL);

    *enable = (ca_boolean_t)value.bf.ni_port_grp_en;

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_cpu_port_group_set
(
    ca_device_id_t  device_id,
    ca_boolean_t enable
)
{
    TE_SHP_CTRL_t value;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_CTRL);

    value.bf.cpu_port_grp_en = enable ? 1 : 0;

    CA_L3TE_REG_WRITE(TE_SHP_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_cpu_port_group_get
(
    ca_device_id_t  device_id,
    ca_boolean_t *enable
)
{
    TE_SHP_CTRL_t value;

    if (!enable)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_CTRL);

    *enable = (ca_boolean_t)value.bf.cpu_port_grp_en;

    return CA_E_OK;
}


ca_status_t aal_l3_te_shaper_voq_tbc_set(
    ca_device_id_t                 device_id,
    ca_uint32_t                 voq_id,
    aal_l3_te_shaper_tbc_cfg_msk_t msk,
    aal_l3_te_shaper_tbc_cfg_t     *cfg)
{
    TE_SHP_VOQ_TBC_MEM_DATA0_t value0;
    TE_SHP_VOQ_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = voq_id;

    if (!cfg)
        return CA_E_PARAM;

    if (voq_id > CA_AAL_MAX_VOQ_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_shaper_voq_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.rate_k)
    {
        if (cfg->rate_k > CA_AAL_L3_TE_SHP_MAX_RATE_K_VOQ)
            return CA_E_PARAM;

        value0.bf.rate_k = cfg->rate_k;
    }

    if (msk.s.rate_m)
    {
        if (cfg->rate_m > CA_AAL_L3_TE_SHP_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.rate_m = cfg->rate_m;
    }

    if (msk.s.bs)
    {
        if (cfg->bs > CA_AAL_L3_TE_SHP_MAX_BS || cfg->bs == 0)
            return CA_E_PARAM;

        value0.bf.bs = cfg->bs & 0x3f;
        value1.bf.bs = (cfg->bs >> 6) & 0x3f;
    }

    if (msk.s.state)
    {
        if (cfg->state >= CA_AAL_L3_SHAPER_STATE_END)
            return CA_E_PARAM;

        value1.bf.enb = cfg->state;
    }

    if (msk.s.mode)
    {
        if (cfg->mode >= CA_AAL_L3_SHAPER_MODE_END)
            return CA_E_PARAM;

        value1.bf.mode = cfg->mode;
    }

    if (msk.s.tbc_count)
    {
        value1.bf.tbc = cfg->tbc_count;
    }


    /* update packet type based policing table */
    return __l3_te_shaper_voq_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}



ca_status_t aal_l3_te_shaper_voq_tbc_get(
    ca_device_id_t             device_id,
    ca_uint32_t             voq_id,
    aal_l3_te_shaper_tbc_cfg_t *cfg)
{
    TE_SHP_VOQ_TBC_MEM_DATA0_t value0;
    TE_SHP_VOQ_TBC_MEM_DATA1_t value1;

    ca_uint32_t index = voq_id;

    if (!cfg)
        return CA_E_PARAM;

    if (voq_id > CA_AAL_MAX_VOQ_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_shaper_voq_tbc_access(
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
    cfg->state  = (aal_l3_te_shaper_admin_state_t)value1.bf.enb;
    cfg->mode   = (aal_l3_te_shaper_counting_mode_t)value1.bf.mode;
    cfg->tbc_count = value1.bf.tbc;

    return CA_E_OK;
}

/****************************************************************
** SHP_CTRL.cpu_port_grp_en is 0, entry 0 ~ 7 for 8 CPU ports
** SHP_CTRL.cpu_port_grp_en is 1, entry 0 for port group shaper
** SHP_CTRL.ni_port_grp_en  is 0, entry 8 ~ 15 for 8 NI ports
** SHP_CTRL.ni_port_grp_en  is 1, entry 0 for ni group shaper
****************************************************************/
ca_status_t aal_l3_te_shaper_port_tbc_set(
    ca_device_id_t                 device_id,
    ca_port_id_t                port_id,
    aal_l3_te_shaper_tbc_cfg_msk_t msk,
    aal_l3_te_shaper_tbc_cfg_t     *cfg)
{
    TE_SHP_PORT_TBC_MEM_DATA0_t value0;
    TE_SHP_PORT_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_shaper_port_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.rate_k)
    {
        if (cfg->rate_k > CA_AAL_L3_TE_SHP_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.rate_k = cfg->rate_k;
    }

    if (msk.s.rate_m)
    {
        if (cfg->rate_m > CA_AAL_L3_TE_SHP_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.rate_m = cfg->rate_m;
    }

    if (msk.s.bs)
    {
        if (cfg->bs > CA_AAL_L3_TE_SHP_MAX_BS || cfg->bs == 0)
            return CA_E_PARAM;

        value0.bf.bs = cfg->bs & 0x3f;
        value1.bf.bs = (cfg->bs >> 6) & 0x3f;
    }

    if (msk.s.state)
    {
        if (cfg->state >= CA_AAL_L3_SHAPER_STATE_END)
            return CA_E_PARAM;

        value1.bf.enb = cfg->state;
    }

    if (msk.s.mode)
    {
        if (cfg->mode >= CA_AAL_L3_SHAPER_MODE_END)
            return CA_E_PARAM;

        value1.bf.mode = cfg->mode;
    }

    if (msk.s.tbc_count)
    {
        value1.bf.tbc = cfg->tbc_count;
    }

    /* update packet type based policing table */
    return __l3_te_shaper_port_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}



ca_status_t aal_l3_te_shaper_port_tbc_get(
    ca_device_id_t             device_id,
    ca_port_id_t            port_id,
    aal_l3_te_shaper_tbc_cfg_t *cfg)
{
    TE_SHP_PORT_TBC_MEM_DATA0_t value0;
    TE_SHP_PORT_TBC_MEM_DATA1_t value1;

    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_shaper_port_tbc_access(
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
    cfg->state  = (aal_l3_te_shaper_admin_state_t)value1.bf.enb;
    cfg->mode   = (aal_l3_te_shaper_counting_mode_t)value1.bf.mode;
    cfg->tbc_count = value1.bf.tbc;

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_group_tbc_set
(
    ca_device_id_t                    device_id,
    ca_uint32_t                    group_id,
    aal_l3_te_shaper_tbc_cfg_msk_t msk,
    aal_l3_te_shaper_tbc_cfg_t     *cfg
)
{
    TE_SHP_GROUP_TBC_MEM_DATA0_t value0;
    TE_SHP_GROUP_TBC_MEM_DATA1_t value1;
    ca_uint32_t index = group_id;

    if (!cfg)
        return CA_E_PARAM;

    if (group_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_shaper_group_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.rate_k)
    {
        if (cfg->rate_k > CA_AAL_L3_TE_SHP_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.rate_k = cfg->rate_k;
    }

    if (msk.s.rate_m)
    {
        if (cfg->rate_m > CA_AAL_L3_TE_SHP_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.rate_m = cfg->rate_m;
    }

    if (msk.s.bs)
    {
        if (cfg->bs > CA_AAL_L3_TE_SHP_MAX_BS || cfg->bs == 0)
            return CA_E_PARAM;

        value0.bf.bs = cfg->bs & 0x3f;
        value1.bf.bs = (cfg->bs >> 6) & 0x3f;
    }

    if (msk.s.state)
    {
        if (cfg->state >= CA_AAL_L3_SHAPER_STATE_END)
            return CA_E_PARAM;

        value1.bf.enb = cfg->state;
    }

    if (msk.s.mode)
    {
        if (cfg->mode >= CA_AAL_L3_SHAPER_MODE_END)
            return CA_E_PARAM;

        value1.bf.mode = cfg->mode;
    }

    if (msk.s.tbc_count)
    {
        value1.bf.tbc = cfg->tbc_count;
    }

    /* update l3 te group shaper table */
    return __l3_te_shaper_group_tbc_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}



ca_status_t aal_l3_te_shaper_group_tbc_get
(
    ca_device_id_t                device_id,
    ca_uint32_t                group_id,
    aal_l3_te_shaper_tbc_cfg_t *cfg
)
{
    TE_SHP_GROUP_TBC_MEM_DATA0_t value0;
    TE_SHP_GROUP_TBC_MEM_DATA1_t value1;

    ca_uint32_t index = group_id;

    if (!cfg)
        return CA_E_PARAM;

    if (group_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_shaper_group_tbc_access(
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
    cfg->state  = (aal_l3_te_shaper_admin_state_t)value1.bf.enb;
    cfg->mode   = (aal_l3_te_shaper_counting_mode_t)value1.bf.mode;
    cfg->tbc_count = value1.bf.tbc;

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_ipq_set
(
    ca_device_id_t  device_id,
    ca_uint8_t      ipg_id,
    ca_uint16_t     ipg
)
{
    TE_SHP_IPG_PROFILE_t value;

    if (ipg_id > CA_AAL_MAX_IPG_ID)
        return CA_E_PARAM;

    if (ipg > CA_AAL_MAX_IPG)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_IPG_PROFILE);

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

    CA_L3TE_REG_WRITE(TE_SHP_IPG_PROFILE, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_ipq_get
(
    ca_device_id_t  device_id,
    ca_uint8_t      ipg_id,
    ca_uint16_t     *ipg
)
{
    TE_SHP_IPG_PROFILE_t value;

    if (!ipg)
        return CA_E_PARAM;

    if (ipg_id > CA_AAL_MAX_IPG_ID)
        return CA_E_PARAM;

    if (*ipg > CA_AAL_MAX_IPG)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_IPG_PROFILE);

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


ca_status_t aal_l3_te_shaper_port_ipq_set
(
    ca_device_id_t  device_id,
    ca_port_id_t    port_id,
    ca_uint8_t      ipg_id
)
{
    ca_uint32_t val32 = ipg_id;
    TE_SHP_PORT_IPG_SELECT0_t value;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (ipg_id > CA_AAL_MAX_IPG_ID)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_PORT_IPG_SELECT0);

    value.wrd &= ~(0x3 << (port_id * 2));
    value.wrd |= val32 << (port_id * 2);

    CA_L3TE_REG_WRITE(TE_SHP_PORT_IPG_SELECT0, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l3_te_shaper_port_ipq_get
(
    ca_device_id_t  device_id,
    ca_port_id_t    port_id,
    ca_uint8_t      *ipg_id
)
{
    TE_SHP_PORT_IPG_SELECT0_t value;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (!ipg_id)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_SHP_PORT_IPG_SELECT0);

    *ipg_id = (value.wrd >> (port_id * 2)) & 0x3;

    return CA_E_OK;
}



#ifdef __L3_POL
#endif
static inline ca_status_t __l3_te_policer_pkt_type_profile_access
(
    ca_device_id_t             device_id,
    aal_tbl_op_flag_t       op_flag,
    ca_uint32_t             addr,
    volatile ca_uint32_t    *data0,
    volatile ca_uint32_t    *data1,
    volatile ca_uint32_t    *data2
)
{
    TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_PROFILE_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_PROFILE_MEM_DATA1, *data1);
        CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_DATA1);
        *data2 = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;
}


static inline ca_status_t __l3_te_policer_pkt_type_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_COUNTER_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_COUNTER_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_PKT_TYPE_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;
}


static inline ca_status_t __l3_te_policer_port_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    TE_POL_PORT_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_PORT_PROFILE_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_PORT_PROFILE_MEM_DATA1, *data1);
        CA_L3TE_REG_WRITE(TE_POL_PORT_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_PORT_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_DATA1);
        *data2 = CA_L3TE_REG_READ(TE_POL_PORT_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}


static inline ca_status_t __l3_te_policer_port_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_POL_PORT_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_PORT_COUNTER_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_PORT_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_PORT_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L3TE_REG_READ(TE_POL_PORT_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_PORT_COUNTER_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_PORT_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;
}


static inline ca_status_t __l3_te_policer_flow_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    TE_POL_FLOW_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_FLOW_PROFILE_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_FLOW_PROFILE_MEM_DATA1, *data1);
        CA_L3TE_REG_WRITE(TE_POL_FLOW_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_FLOW_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_DATA1);
        *data2 = CA_L3TE_REG_READ(TE_POL_FLOW_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}


static inline ca_status_t __l3_te_policer_flow_counter_access(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_POL_FLOW_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_FLOW_COUNTER_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_FLOW_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_FLOW_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L3TE_REG_READ(TE_POL_FLOW_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_FLOW_COUNTER_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_FLOW_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;

}


static inline ca_status_t __l3_te_policer_flow_grp_profile_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1,
    volatile ca_uint32_t *data2
)
{
    TE_POL_AGRFLOW_PROFILE_MEM_ACCESS_t profile_access;
    ca_uint32_t i = 0;

    profile_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);

    while (profile_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_PROFILE_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_PROFILE_MEM_DATA1, *data1);
        CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_PROFILE_MEM_DATA2, *data2);
    }

    memset((void *)&profile_access, 0, sizeof(profile_access));

    profile_access.bf.address  = addr;
    profile_access.bf.rbw      = op_flag;
    profile_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_PROFILE_MEM_ACCESS, profile_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        profile_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);
        while (profile_access.bf.access)
        {
            profile_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_DATA1);
        *data2 = CA_L3TE_REG_READ(TE_POL_AGRFLOW_PROFILE_MEM_DATA2);
    }

    return CA_E_OK;

}



static inline ca_status_t __l3_te_policer_flow_grp_counter_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_POL_AGRFLOW_COUNTER_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_COUNTER_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_COUNTER_MEM_DATA1, *data1);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_POL_AGRFLOW_COUNTER_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L3TE_REG_READ(TE_POL_AGRFLOW_COUNTER_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_POL_AGRFLOW_COUNTER_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_POL_AGRFLOW_COUNTER_MEM_DATA1);
    }

    return CA_E_OK;

}

ca_status_t aal_l3_te_policer_pkt_type_cfg_set
(
    ca_device_id_t               device_id,
    aal_l3_te_policer_pkt_type_t pkt_type,
    aal_l3_te_policer_cfg_msk_t  msk,
    aal_l3_te_policer_cfg_t      *cfg
)
{
    ca_uint32_t addr = 0;
    TE_POL_PKT_TYPE_CTRL0_t ctrl;

    if (pkt_type >= CA_AAL_L3_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = TE_POL_PKT_TYPE_CTRL0 + (ca_uint32_t)pkt_type * 4;    /* 8 instances in total */

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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

    CA_L3TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l3_te_policer_pkt_type_cfg_get
(
    ca_device_id_t                   device_id,
    aal_l3_te_policer_pkt_type_t pkt_type,
    aal_l3_te_policer_cfg_t      *cfg
)
{
    ca_uint32_t addr = 0;
    TE_POL_PKT_TYPE_CTRL0_t ctrl;

    if (pkt_type >= CA_AAL_L3_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = TE_POL_PKT_TYPE_CTRL0 + pkt_type * 4;

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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

ca_status_t aal_l3_te_policer_pkt_type_profile_set
(
    ca_device_id_t                      device_id,
    aal_l3_te_policer_pkt_type_t    pkt_type,
    aal_l3_te_policer_profile_msk_t msk,
    aal_l3_te_policer_profile_t     *cfg
)
{
    TE_POL_PKT_TYPE_PROFILE_MEM_DATA0_t value0;
    TE_POL_PKT_TYPE_PROFILE_MEM_DATA1_t value1;
    TE_POL_PKT_TYPE_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = (ca_uint32_t)pkt_type;

    if (!cfg)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_L3_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_pkt_type_profile_access(
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
        if (cfg->cir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L3_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L3_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }


    /* update packet type based policing table */
    return __l3_te_policer_pkt_type_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}


ca_status_t aal_l3_te_policer_pkt_type_profile_get
(
    ca_device_id_t                  device_id,
    aal_l3_te_policer_pkt_type_t pkt_type,
    aal_l3_te_policer_profile_t  *cfg
)
{
    TE_POL_PKT_TYPE_PROFILE_MEM_DATA0_t value0;
    TE_POL_PKT_TYPE_PROFILE_MEM_DATA1_t value1;
    TE_POL_PKT_TYPE_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = pkt_type;

    if (!cfg)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_L3_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_pkt_type_profile_access(
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

ca_status_t aal_l3_te_policer_pkt_type_stats_get
(
    ca_device_id_t                   device_id,
    aal_l3_te_policer_pkt_type_t pkt_type,
    aal_l3_te_policer_stats_t    *stats
)
{
    TE_POL_PKT_TYPE_COUNTER_MEM_DATA0_t value0;
    TE_POL_PKT_TYPE_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = (ca_uint32_t)pkt_type;

    if (!stats)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_L3_POLICER_PKT_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_pkt_type_counter_access(
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


ca_status_t aal_l3_te_policer_port_cfg_set
(
    ca_device_id_t                  device_id,
    ca_flow_id_t                 port_id,
    aal_l3_te_policer_cfg_msk_t msk,
    aal_l3_te_policer_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    TE_POL_PORT_CTRL0_t ctrl;

    if (port_id > CA_AAL_L3_MAX_PORT_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = TE_POL_PORT_CTRL0 + port_id * 4;    /* 8 instances in total */

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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

    CA_L3TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l3_te_policer_port_cfg_get
(
    ca_device_id_t                device_id,
    ca_flow_id_t               port_id,
    aal_l3_te_policer_cfg_t   *cfg
)
{
    ca_uint32_t addr = 0;
    TE_POL_PORT_CTRL0_t ctrl;

    if (port_id > CA_AAL_L3_MAX_PORT_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = TE_POL_PORT_CTRL0 + port_id * 4;

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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

ca_status_t aal_l3_te_policer_port_profile_set
(
    ca_device_id_t                      device_id,
    ca_flow_id_t                     port_id,
    aal_l3_te_policer_profile_msk_t msk,
    aal_l3_te_policer_profile_t     *cfg
)
{
    TE_POL_PORT_PROFILE_MEM_DATA0_t value0;
    TE_POL_PORT_PROFILE_MEM_DATA1_t value1;
    TE_POL_PORT_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_port_profile_access(
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
        if (cfg->cir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L3_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L3_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }

    /* update packet type based policing table */
    return __l3_te_policer_port_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}


ca_status_t aal_l3_te_policer_port_profile_get
(
    ca_device_id_t                  device_id,
    ca_flow_id_t                 port_id,
    aal_l3_te_policer_profile_t *cfg
)
{
    TE_POL_PORT_PROFILE_MEM_DATA0_t value0;
    TE_POL_PORT_PROFILE_MEM_DATA1_t value1;
    TE_POL_PORT_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = port_id;

    if (!cfg)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_port_profile_access(
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

ca_status_t aal_l3_te_policer_port_stats_get
(
    ca_device_id_t                device_id,
    ca_flow_id_t               port_id,
    aal_l3_te_policer_stats_t *stats
)
{
    TE_POL_PORT_COUNTER_MEM_DATA0_t value0;
    TE_POL_PORT_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = port_id;

    if (!stats)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_port_counter_access(
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


ca_status_t aal_l3_te_policer_flow_cfg_set
(
    ca_device_id_t              device_id,
    ca_flow_id_t                flow_id,
    aal_l3_te_policer_cfg_msk_t msk,
    aal_l3_te_policer_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    TE_POL_FLOW_GRP_CTRL0_t ctrl;

    if (flow_id > CA_AAL_L3_MAX_FLOW_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = TE_POL_FLOW_GRP_CTRL0 + (flow_id / 32) * 4;    /* 32 flows share one configuration */

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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

    CA_L3TE_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l3_te_policer_flow_cfg_get
(
    ca_device_id_t              device_id,
    ca_flow_id_t             flow_id,
    aal_l3_te_policer_cfg_t *cfg)
{
    ca_uint32_t addr = 0;
    TE_POL_FLOW_GRP_CTRL0_t ctrl;

    if (flow_id > CA_AAL_L3_MAX_FLOW_ID)
        return CA_E_PARAM;

    if (cfg == NULL)
        return CA_E_PARAM;

    addr = TE_POL_FLOW_GRP_CTRL0 + (flow_id / 32) * 4;

    ctrl.wrd = CA_L3TE_REG_READ(addr);

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


ca_status_t aal_l3_te_policer_flow_profile_set(
    ca_device_id_t                   device_id,
    ca_flow_id_t                  flow_id,
    aal_l3_te_policer_profile_msk_t msk,
    aal_l3_te_policer_profile_t     *cfg)
{
    TE_POL_FLOW_PROFILE_MEM_DATA0_t value0;
    TE_POL_FLOW_PROFILE_MEM_DATA1_t value1;
    TE_POL_FLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = flow_id;

    if (!cfg)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_L3_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_flow_profile_access(
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
        if (cfg->cir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L3_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L3_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }

    /* update packet type based policing table */
    return __l3_te_policer_flow_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}



ca_status_t aal_l3_te_policer_flow_profile_get
(
    ca_device_id_t                  device_id,
    ca_flow_id_t                 flow_id,
    aal_l3_te_policer_profile_t *cfg)
{
    TE_POL_FLOW_PROFILE_MEM_DATA0_t value0;
    TE_POL_FLOW_PROFILE_MEM_DATA1_t value1;
    TE_POL_FLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = flow_id;

    if (!cfg)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_L3_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_flow_profile_access(
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



ca_status_t aal_l3_te_policer_flow_stats_get
(
    ca_device_id_t                device_id,
    ca_flow_id_t               flow_id,
    aal_l3_te_policer_stats_t *stats)
{
    TE_POL_FLOW_COUNTER_MEM_DATA0_t value0;
    TE_POL_FLOW_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = flow_id;

    if (!stats)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_L3_MAX_FLOW_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_flow_counter_access(
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


ca_status_t aal_l3_te_policer_agr_flow_profile_set
(
    ca_device_id_t                       device_id,
    ca_uint32_t                       agr_flow_id,
    aal_l3_te_policer_grp_map_type_t grp_map_type,
    aal_l3_te_policer_profile_msk_t  msk,
    aal_l3_te_policer_profile_t      *cfg)
{
    TE_POL_AGRFLOW_PROFILE_MEM_DATA0_t value0;
    TE_POL_AGRFLOW_PROFILE_MEM_DATA1_t value1;
    TE_POL_AGRFLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = agr_flow_id * 4 + (ca_uint32_t)grp_map_type;

    if (!cfg)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_L3_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (grp_map_type > CA_AAL_L3_POLICER_GRP_MAP_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_flow_grp_profile_access(
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
        if (cfg->cir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value0.bf.committed_rate_k = cfg->cir_k;
    }

    if (msk.s.cir_m)
    {
        if (cfg->cir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value0.bf.committed_rate_m = cfg->cir_m;
    }

    if (msk.s.cbs)
    {
        if (cfg->cbs > CA_AAL_L3_TE_POL_MAX_CBS || cfg->cbs == 0)
            return CA_E_PARAM;

        value0.bf.committed_bs = cfg->cbs & 0x3f;
        value1.bf.committed_bs = (cfg->cbs >> 6) & 0x3f;
    }

    if (msk.s.pir_k)
    {
        if (cfg->pir_k > CA_AAL_L3_TE_POL_MAX_RATE_K)
            return CA_E_PARAM;

        value1.bf.peak_rate_k = cfg->pir_k;
    }

    if (msk.s.pir_m)
    {
        if (cfg->pir_m > CA_AAL_L3_TE_POL_MAX_RATE_M)
            return CA_E_PARAM;

        value1.bf.peak_rate_m = cfg->pir_m;
    }

    if (msk.s.pbs)
    {
        if (cfg->pbs > CA_AAL_L3_TE_POL_MAX_PBS || cfg->pbs == 0)
            return CA_E_PARAM;

        value2.bf.peak_bs = cfg->pbs;
    }

    /* update packet type based policing table */
    return __l3_te_policer_flow_grp_profile_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd),
            (volatile ca_uint32_t *)&(value2.wrd));
}



ca_status_t aal_l3_te_policer_agr_flow_profile_get
(
    ca_device_id_t                       device_id,
    ca_uint32_t                       agr_flow_id,
    aal_l3_te_policer_grp_map_type_t grp_map_type,
    aal_l3_te_policer_profile_t      *cfg)
{
    TE_POL_AGRFLOW_PROFILE_MEM_DATA0_t value0;
    TE_POL_AGRFLOW_PROFILE_MEM_DATA1_t value1;
    TE_POL_AGRFLOW_PROFILE_MEM_DATA2_t value2;
    ca_uint32_t index = agr_flow_id * 4 + (ca_uint32_t)grp_map_type;

    if (!cfg)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_L3_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (grp_map_type > CA_AAL_L3_POLICER_GRP_MAP_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));
    memset((void *)&(value2), 0, sizeof(value2));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_flow_grp_profile_access(
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



ca_status_t aal_l3_te_policer_agr_flow_stats_get
(
    ca_device_id_t                       device_id,
    ca_uint32_t                       agr_flow_id,
    aal_l3_te_policer_grp_map_type_t grp_map_type,
    aal_l3_te_policer_stats_t        *stats)
{
    TE_POL_AGRFLOW_COUNTER_MEM_DATA0_t value0;
    TE_POL_AGRFLOW_COUNTER_MEM_DATA1_t value1;
    ca_uint32_t index = agr_flow_id * 4 + (ca_uint32_t)grp_map_type;

    if (!stats)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_L3_MAX_AGR_FLOW_ID)
        return CA_E_PARAM;

    if (grp_map_type > CA_AAL_L3_POLICER_GRP_MAP_TYPE_END)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_policer_flow_grp_counter_access(
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

#ifdef __L3_WRED
#endif
static inline ca_status_t __l3_te_wred_profile_access
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
    TE_WRED_PROFILE_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA1, *data1);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA2, *data2);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA3, *data3);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA4, *data4);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA5, *data5);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA6, *data6);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_DATA7, *data7);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_WRED_PROFILE_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA1);
        *data2 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA2);
        *data3 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA3);
        *data4 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA4);
        *data5 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA5);
        *data6 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA6);
        *data7 = CA_L3TE_REG_READ(TE_WRED_PROFILE_MEM_DATA7);
    }

    return CA_E_OK;
}


static inline ca_status_t __l3_te_wred_profile_sel_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    TE_WRED_PROFILE_SELECT_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_SELECT_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_SELECT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_SELECT_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_WRED_PROFILE_SELECT_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_WRED_PROFILE_SELECT_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_SELECT_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L3TE_REG_READ(TE_WRED_PROFILE_SELECT_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_WRED_PROFILE_SELECT_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_WRED_PROFILE_SELECT_MEM_DATA1);
    }

    return CA_E_OK;
}


ca_status_t aal_l3_te_wred_profile_set
(
    ca_device_id_t                  device_id,
    ca_uint32_t                  id,
    aal_l3_te_wred_profile_msk_t msk,
    aal_l3_te_wred_profile_t     *profile
)
{
    TE_WRED_PROFILE_MEM_DATA0_t value0;
    TE_WRED_PROFILE_MEM_DATA1_t value1;
    TE_WRED_PROFILE_MEM_DATA2_t value2;
    TE_WRED_PROFILE_MEM_DATA3_t value3;
    TE_WRED_PROFILE_MEM_DATA4_t value4;
    TE_WRED_PROFILE_MEM_DATA5_t value5;
    TE_WRED_PROFILE_MEM_DATA6_t value6;
    TE_WRED_PROFILE_MEM_DATA7_t value7;

    ca_uint32_t index = id;

    if (id > CA_AAL_L3_WRED_MAX_PROFILE_ID)
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
    if (__l3_te_wred_profile_access(
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
    return __l3_te_wred_profile_access(
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


ca_status_t aal_l3_te_wred_profile_get
(
    ca_device_id_t              device_id,
    ca_uint32_t              id,
    aal_l3_te_wred_profile_t *profile
)
{
    TE_WRED_PROFILE_MEM_DATA0_t value0;
    TE_WRED_PROFILE_MEM_DATA1_t value1;
    TE_WRED_PROFILE_MEM_DATA2_t value2;
    TE_WRED_PROFILE_MEM_DATA3_t value3;
    TE_WRED_PROFILE_MEM_DATA4_t value4;
    TE_WRED_PROFILE_MEM_DATA5_t value5;
    TE_WRED_PROFILE_MEM_DATA6_t value6;
    TE_WRED_PROFILE_MEM_DATA7_t value7;

    ca_uint32_t index = id;

    if (id > CA_AAL_L3_WRED_MAX_PROFILE_ID)
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
    if (__l3_te_wred_profile_access(
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

ca_status_t aal_l3_te_wred_profile_sel_set
(
    ca_device_id_t                   device_id,
    ca_uint32_t                   id,
    aal_l3_te_wred_profile_sel_msk_t msk,
    aal_l3_te_wred_profile_sel_t     *sel
)
{
    TE_WRED_PROFILE_SELECT_MEM_DATA0_t value0;
    TE_WRED_PROFILE_SELECT_MEM_DATA1_t value1;
    ca_uint32_t index = id;

    if (id > CA_AAL_L3_WRED_MAX_PROFILE_SEL_ID)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    if (!sel)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_wred_profile_sel_access(
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
    return __l3_te_wred_profile_sel_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd));
}


ca_status_t aal_l3_te_wred_profile_sel_get
(
    ca_device_id_t                  device_id,
    ca_uint32_t                  id,
    aal_l3_te_wred_profile_sel_t *sel
)
{
    TE_WRED_PROFILE_SELECT_MEM_DATA0_t value0;
    TE_WRED_PROFILE_SELECT_MEM_DATA1_t value1;
    ca_uint32_t index = id;

    if (id > CA_AAL_L3_WRED_MAX_PROFILE_SEL_ID)
        return CA_E_PARAM;

    if (!sel)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));


    /* get current configuration at first and then update specificed field */
    if (__l3_te_wred_profile_sel_access(
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

ca_boolean aal_l3_te_wred_info_lookup
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

    for (i=0; i<CA_AAL_L3_WRED_MAX_PROFILE_NUMS; i++)
    {
        if (g_aal_l3_te_wred_profile_info[i].ref_counter == 0)
            continue;

        if (
            memcmp(g_aal_l3_te_wred_profile_info[i].marked_dp, marked_dp, CA_AAL_L3_WRED_MAX_DROP_PROBABILTY * sizeof(ca_uint32_t)) == 0 &&
            memcmp(g_aal_l3_te_wred_profile_info[i].unmarked_dp, unmarked_dp, CA_AAL_L3_WRED_MAX_DROP_PROBABILTY * sizeof(ca_uint32_t)) == 0
            )
        {
            *index = i;     /* hit, this profile entry can be reused */
            return TRUE;
        }

    }

    return FALSE;

}

ca_boolean aal_l3_te_wred_info_empty(void)
{
    ca_uint32_t zero_dp[CA_AAL_L3_WRED_MAX_DROP_PROBABILTY];
    ca_uint32_t i = 0;

    memset((void *)&zero_dp[0], 0, sizeof(zero_dp));

    for (i=0; i<CA_AAL_L3_WRED_MAX_PROFILE_NUMS; i++)
    {
        if (memcmp(g_aal_l3_te_wred_profile_info[i].marked_dp, zero_dp, sizeof(zero_dp)) ||
            memcmp(g_aal_l3_te_wred_profile_info[i].unmarked_dp, zero_dp, sizeof(zero_dp)))
        {
            return FALSE;
        }
    }

    return TRUE;
}



ca_boolean aal_l3_te_wred_profile_info_allocate
(
    ca_uint32_t *index
)
{
    ca_uint32_t i = 0;

    if (!index)
        return FALSE;

    for (i=0; i<CA_AAL_L3_WRED_MAX_PROFILE_NUMS; i++)
    {
        if (g_aal_l3_te_wred_profile_info[i].ref_counter == 0)
        {
            *index = i;

            return TRUE;
        }
    }

    return FALSE;
}

ca_status_t aal_l3_te_wred_profile_info_set
(
    ca_uint32_t index,
    ca_uint32_t *marked_dp,
    ca_uint32_t *unmarked_dp
)
{
    if (index > CA_AAL_L3_WRED_MAX_PROFILE_ID)
        return CA_E_PARAM;

    if (!marked_dp)
        return CA_E_PARAM;

    if (!unmarked_dp)
        return CA_E_PARAM;

    memcpy(g_aal_l3_te_wred_profile_info[index].marked_dp, marked_dp, sizeof(g_aal_l3_te_wred_profile_info[index].marked_dp));
    memcpy(g_aal_l3_te_wred_profile_info[index].unmarked_dp, unmarked_dp, sizeof(g_aal_l3_te_wred_profile_info[index].unmarked_dp));

    return CA_E_OK;
}

ca_status_t aal_l3_te_wred_profile_info_get
(
    ca_uint32_t index,
    ca_uint32_t *marked_dp,
    ca_uint32_t *unmarked_dp
)
{
    if (index > CA_AAL_L3_WRED_MAX_PROFILE_ID)
        return CA_E_PARAM;

    if (!marked_dp)
        return CA_E_PARAM;

    if (!unmarked_dp)
        return CA_E_PARAM;

    memcpy(marked_dp, g_aal_l3_te_wred_profile_info[index].marked_dp, sizeof(g_aal_l3_te_wred_profile_info[index].marked_dp));
    memcpy(unmarked_dp, g_aal_l3_te_wred_profile_info[index].unmarked_dp, sizeof(g_aal_l3_te_wred_profile_info[index].unmarked_dp));

    return CA_E_OK;
}


ca_uint32_t aal_l3_te_wred_profile_info_ref_counter_get(ca_uint32_t index)
{
    return g_aal_l3_te_wred_profile_info[index].ref_counter;
}

void aal_l3_te_wred_profile_info_ref_inc(ca_uint32_t index)
{
    g_aal_l3_te_wred_profile_info[index].ref_counter++;
}

void aal_l3_te_wred_profile_info_ref_dec(ca_uint32_t index)
{
    if (g_aal_l3_te_wred_profile_info[index].ref_counter)
        g_aal_l3_te_wred_profile_info[index].ref_counter--;
}

void aal_l3_te_wred_sel_info_set
(
    ca_port_id_t port,
    ca_uint32_t  queue,
    ca_uint32    index
)
{
    if (port <= CA_AAL_L3_WRED_MAX_PORT_ID && queue <= 7)
        g_aal_l3_te_wred_sel_info[port][queue] = index;
}

ca_uint8_t aal_l3_te_wred_sel_info_get
(
    ca_port_id_t port,
    ca_uint32_t  queue
)
{
    if (port <= CA_AAL_L3_WRED_MAX_PORT_ID && queue <= 7)
        return g_aal_l3_te_wred_sel_info[port][queue];
    else
        return 0xff;
}

ca_status_t aal_l3_te_wred_debug_get(ca_device_id_t device_id)
{
    ca_uint32_t i = 0, j = 0;
    ca_int8_t str_marked[256] = {0};
    ca_int8_t str_unmarked[256] = {0};

    printk("============= S/W WRED profile table ============= \n");

    for (i=0; i<CA_AAL_L3_WRED_MAX_PROFILE_NUMS; i++)
    {
        printk("index %02d : ref_counter(%d)\n", i, g_aal_l3_te_wred_profile_info[i].ref_counter);
        sprintf(str_marked, "  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            g_aal_l3_te_wred_profile_info[i].marked_dp[0], g_aal_l3_te_wred_profile_info[i].marked_dp[1],
            g_aal_l3_te_wred_profile_info[i].marked_dp[2], g_aal_l3_te_wred_profile_info[i].marked_dp[3],
            g_aal_l3_te_wred_profile_info[i].marked_dp[4], g_aal_l3_te_wred_profile_info[i].marked_dp[5],
            g_aal_l3_te_wred_profile_info[i].marked_dp[6], g_aal_l3_te_wred_profile_info[i].marked_dp[7],
            g_aal_l3_te_wred_profile_info[i].marked_dp[8], g_aal_l3_te_wred_profile_info[i].marked_dp[9],
            g_aal_l3_te_wred_profile_info[i].marked_dp[10], g_aal_l3_te_wred_profile_info[i].marked_dp[11],
            g_aal_l3_te_wred_profile_info[i].marked_dp[12], g_aal_l3_te_wred_profile_info[i].marked_dp[13],
            g_aal_l3_te_wred_profile_info[i].marked_dp[14], g_aal_l3_te_wred_profile_info[i].marked_dp[15]);
        sprintf(str_unmarked, "  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[0], g_aal_l3_te_wred_profile_info[i].unmarked_dp[1],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[2], g_aal_l3_te_wred_profile_info[i].unmarked_dp[3],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[4], g_aal_l3_te_wred_profile_info[i].unmarked_dp[5],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[6], g_aal_l3_te_wred_profile_info[i].unmarked_dp[7],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[8], g_aal_l3_te_wred_profile_info[i].unmarked_dp[9],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[10], g_aal_l3_te_wred_profile_info[i].unmarked_dp[11],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[12], g_aal_l3_te_wred_profile_info[i].unmarked_dp[13],
            g_aal_l3_te_wred_profile_info[i].unmarked_dp[14], g_aal_l3_te_wred_profile_info[i].unmarked_dp[15]);

        printk("%s", str_marked);
        printk("%s", str_unmarked);
    }

    printk("\n");

    printk("===== S/W WRED Selection table ===== \n");
    printk("          Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7\n");
    printk("          -- -- -- -- -- -- -- --\n");

    for (i=0; i<=CA_AAL_L3_WRED_MAX_PORT_ID; i++)
    {
        printk("port %02d : ", i);
        for (j=0; j<8; j++)
        {
            printk("%02d ", g_aal_l3_te_wred_sel_info[i][j]);
        }
        printk("\n");
    }

    return CA_E_OK;
}


#ifdef __L3_PM
#endif
static inline ca_status_t __l3_te_pm_counter_access
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
    TE_PM_CNT_MEM_ACCESS_t counter_access;
    ca_uint32_t i = 0;

    counter_access.wrd = CA_L3TE_REG_READ(TE_PM_CNT_MEM_ACCESS);

    while (counter_access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        counter_access.wrd = CA_L3TE_REG_READ(TE_PM_CNT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA0, *data0);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA1, *data1);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA2, *data2);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA3, *data3);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA4, *data4);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA5, *data5);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA6, *data6);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA7, *data7);
        CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_DATA8, *data8);
    }

    memset((void *)&counter_access, 0, sizeof(counter_access));

    counter_access.bf.address  = addr;
    counter_access.bf.rbw      = op_flag;
    counter_access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_PM_CNT_MEM_ACCESS, counter_access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        counter_access.wrd = CA_L3TE_REG_READ(TE_PM_CNT_MEM_ACCESS);
        while (counter_access.bf.access)
        {
            counter_access.wrd = CA_L3TE_REG_READ(TE_PM_CNT_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA0);
        *data1 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA1);
        *data2 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA2);
        *data3 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA3);
        *data4 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA4);
        *data5 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA5);
        *data6 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA6);
        *data7 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA7);
        *data8 = CA_L3TE_REG_READ(TE_PM_CNT_MEM_DATA8);
    }

    return CA_E_OK;
}


ca_status_t aal_l3_te_pm_policer_flow_get
(
    ca_device_id_t            device_id,
    ca_flow_id_t           flow_id,
    aal_l3_te_pm_policer_t *pm
)
{
    TE_PM_CNT_MEM_DATA0_t value0;
    TE_PM_CNT_MEM_DATA1_t value1;
    TE_PM_CNT_MEM_DATA2_t value2;
    TE_PM_CNT_MEM_DATA3_t value3;
    TE_PM_CNT_MEM_DATA4_t value4;
    TE_PM_CNT_MEM_DATA5_t value5;
    TE_PM_CNT_MEM_DATA6_t value6;
    TE_PM_CNT_MEM_DATA7_t value7;
    TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = flow_id + CA_AAL_L3_TE_POL_PM_FLOW_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (flow_id > CA_AAL_L3_MAX_FLOW_ID)
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
    if (__l3_te_pm_counter_access(
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


ca_status_t aal_l3_te_pm_policer_agr_flow_get
(
    ca_device_id_t            device_id,
    ca_uint32_t            agr_flow_id,
    aal_l3_te_pm_policer_t *pm
)
{
    TE_PM_CNT_MEM_DATA0_t value0;
    TE_PM_CNT_MEM_DATA1_t value1;
    TE_PM_CNT_MEM_DATA2_t value2;
    TE_PM_CNT_MEM_DATA3_t value3;
    TE_PM_CNT_MEM_DATA4_t value4;
    TE_PM_CNT_MEM_DATA5_t value5;
    TE_PM_CNT_MEM_DATA6_t value6;
    TE_PM_CNT_MEM_DATA7_t value7;
    TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = agr_flow_id + CA_AAL_L3_TE_POL_PM_AGR_FLOW_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (agr_flow_id > CA_AAL_L3_MAX_AGR_FLOW_ID)
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
    if (__l3_te_pm_counter_access(
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

ca_status_t aal_l3_te_pm_policer_port_get
(
    ca_device_id_t            device_id,
    ca_port_id_t           port_id,
    aal_l3_te_pm_policer_t *pm
)
{
    TE_PM_CNT_MEM_DATA0_t value0;
    TE_PM_CNT_MEM_DATA1_t value1;
    TE_PM_CNT_MEM_DATA2_t value2;
    TE_PM_CNT_MEM_DATA3_t value3;
    TE_PM_CNT_MEM_DATA4_t value4;
    TE_PM_CNT_MEM_DATA5_t value5;
    TE_PM_CNT_MEM_DATA6_t value6;
    TE_PM_CNT_MEM_DATA7_t value7;
    TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = port_id + CA_AAL_L3_TE_POL_PM_SRC_PORT_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_MAX_PORT_ID)
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
    if (__l3_te_pm_counter_access(
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

ca_status_t aal_l3_te_pm_policer_pkt_type_get
(
    ca_device_id_t                  device_id,
    aal_l3_te_policer_pkt_type_t pkt_type,
    aal_l3_te_pm_policer_t       *pm
)
{
    TE_PM_CNT_MEM_DATA0_t value0;
    TE_PM_CNT_MEM_DATA1_t value1;
    TE_PM_CNT_MEM_DATA2_t value2;
    TE_PM_CNT_MEM_DATA3_t value3;
    TE_PM_CNT_MEM_DATA4_t value4;
    TE_PM_CNT_MEM_DATA5_t value5;
    TE_PM_CNT_MEM_DATA6_t value6;
    TE_PM_CNT_MEM_DATA7_t value7;
    TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = (ca_uint32_t)pkt_type + CA_AAL_L3_TE_POL_PM_PKT_TYPE_OFFSET;

    if (!pm)
        return CA_E_PARAM;

    if (pkt_type >= CA_AAL_L3_POLICER_PKT_TYPE_END)
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
    if (__l3_te_pm_counter_access(
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

ca_status_t aal_l3_te_pm_egress_voq_get
(
    ca_device_id_t           device_id,
    ca_uint32_t           voq_id,
    aal_l3_te_pm_egress_t *pm
)
{
    TE_PM_CNT_MEM_DATA0_t value0;
    TE_PM_CNT_MEM_DATA1_t value1;
    TE_PM_CNT_MEM_DATA2_t value2;
    TE_PM_CNT_MEM_DATA3_t value3;
    TE_PM_CNT_MEM_DATA4_t value4;
    TE_PM_CNT_MEM_DATA5_t value5;
    TE_PM_CNT_MEM_DATA6_t value6;
    TE_PM_CNT_MEM_DATA7_t value7;
    TE_PM_CNT_MEM_DATA8_t value8;
    ca_uint64_t tmp_val1 = 0;
    ca_uint64_t tmp_val2 = 0;
    ca_uint32_t index = voq_id + CA_AAL_L3_TE_PM_MEM_BLOCK_SIZE;

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
    if (__l3_te_pm_counter_access(
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


ca_status_t aal_l3_te_pm_enable_set
(
    ca_device_id_t device_id,
    ca_boolean_t enable
)
{
    TE_PM_CTRL_t value;

    value.wrd = CA_L3TE_REG_READ(TE_PM_CTRL);

    value.bf.mem_disable = enable ? 0 : 1;

    CA_L3TE_REG_WRITE(TE_PM_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l3_te_pm_enable_get
(
    ca_device_id_t device_id,
    ca_boolean_t *enable
)
{
    TE_PM_CTRL_t value;

    if (!enable)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PM_CTRL);

    *enable = value.bf.mem_disable ? FALSE : TRUE;

    return CA_E_OK;
}


ca_status_t aal_l3_te_pm_read_clr_set
(
    ca_device_id_t  device_id,
    ca_boolean_t read_clr
)
{
    TE_PM_CTRL_t value;

    value.wrd = CA_L3TE_REG_READ(TE_PM_CTRL);

    value.bf.mem_auto_clr_on_read = read_clr;

    CA_L3TE_REG_WRITE(TE_PM_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l3_te_pm_read_clr_get
(
    ca_device_id_t  device_id,
    ca_boolean_t *read_clr
)
{
    TE_PM_CTRL_t value;

    if (!read_clr)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PM_CTRL);

    *read_clr = value.bf.mem_auto_clr_on_read ? TRUE :FALSE ;

    return CA_E_OK;
}

ca_status_t aal_l3_te_pm_cnt_mem_set
(
    ca_device_id_t device_id,
    ca_uint32_t block_id,
    ca_uint32_t sel_id,
    ca_uint32_t sel_event
)
{
    TE_PM_CTRL_t value;

    if (block_id > 1)
        return CA_E_PARAM;

    if (sel_id > 1)
        return CA_E_PARAM;

    if (sel_event > 1)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PM_CTRL);

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

    CA_L3TE_REG_WRITE(TE_PM_CTRL, value.wrd);

    return CA_E_OK;
}


ca_status_t aal_l3_te_pm_cnt_mem_get
(
    ca_device_id_t device_id,
    ca_uint32_t block_id,
    ca_uint32_t *sel_id,
    ca_uint32_t *sel_event
)
{
    TE_PM_CTRL_t value;

    if (block_id > 1)
        return CA_E_PARAM;

    if (!sel_id)
        return CA_E_PARAM;

    if (!sel_event)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PM_CTRL);

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


ca_status_t aal_l3_te_pm_status_get
(
    ca_device_id_t           device_id,
    aal_l3_te_pm_status_t *status
)
{
    TE_PM_STS_t value;

    if (!status)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PM_STS);

    status->pkt_cnt0_init_done  = value.bf.pcnt0_init_done;
    status->pkt_cnt1_init_done  = value.bf.pcnt1_init_done;
    status->byte_cnt0_init_done = value.bf.bcnt0_init_done;
    status->byte_cnt1_init_done = value.bf.bcnt1_init_done;

    return CA_E_OK;
}

ca_status_t aal_l3_te_pm_stats_get
(
    ca_device_id_t device_id,
    aal_l3_te_pm_stats_t *stats
)
{
    if (!stats)
        return CA_E_ERROR;

    stats->total_cnt            = CA_L3TE_REG_READ(TE_PM_PCNT);
    stats->pre_mark_cnt         = CA_L3TE_REG_READ(TE_PM_PRE_MARK_PCNT);
    stats->policer_drop_cnt     = CA_L3TE_REG_READ(TE_PM_POL_DROP_PCNT);
    stats->policer_mark_cnt     = CA_L3TE_REG_READ(TE_PM_MARK_PCNT);
    stats->tail_drop_cnt        = CA_L3TE_REG_READ(TE_PM_TAIL_DROP_PCNT);
    stats->wred_yellow_drop_cnt = CA_L3TE_REG_READ(TE_PM_WRED_Y_DROP_PCNT);
    stats->wred_green_drop_cnt  = CA_L3TE_REG_READ(TE_PM_WRED_G_DROP_PCNT);
    stats->mark_cnt             = CA_L3TE_REG_READ(TE_PM_MARK_PCNT);
    stats->bypass_cnt           = CA_L3TE_REG_READ(TE_PM_BYPASS_PCNT);
    stats->bypass_flow_cnt      = CA_L3TE_REG_READ(TE_PM_BYPASS_FLOW_PCNT);
    stats->ce_cnt               = CA_L3TE_REG_READ(TE_PM_CE_PCNT);

    return CA_E_OK;
}

#ifdef __L3_PIE
#endif
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
static inline ca_status_t __l3_te_pie_lpid_qos_voq_access
(
    ca_device_id_t       device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data
)
{
    TE_PIE_LPID_QOS_VOQ_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L3TE_REG_READ(TE_PIE_LPID_QOS_VOQ_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L3TE_REG_READ(TE_PIE_LPID_QOS_VOQ_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L3TE_REG_WRITE(TE_PIE_LPID_QOS_VOQ_DATA, *data);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L3TE_REG_WRITE(TE_PIE_LPID_QOS_VOQ_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L3TE_REG_READ(TE_PIE_LPID_QOS_VOQ_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L3TE_REG_READ(TE_PIE_LPID_QOS_VOQ_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data = CA_L3TE_REG_READ(TE_PIE_LPID_QOS_VOQ_DATA);
    }

    return CA_E_OK;
}
#endif

ca_status_t aal_l3_te_pie_lpid_qos_voq_set
(
    ca_device_id_t                   device_id,
    ca_port_id_t                     lpid,
    ca_uint32_t                      cos,
    aal_l3_te_pie_lpid_qos_voq_msk_t msk,
    aal_l3_te_pie_lpid_qos_voq_t     *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_LPID_QOS_VOQ_DATA_t value0;
    ca_uint32_t index = (lpid << 3 | cos);

    if (!cfg)
        return CA_E_PARAM;

    if (cos > 7)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_pie_lpid_qos_voq_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd))
            != CA_E_OK)
        return CA_E_ERROR;


    if (msk.s.ena)
    {
        value0.bf.value = cfg->ena << 4;
    }

    if (msk.s.voq)
    {
        if (cfg->voq > CA_AAL_L3_PIE_MAX_VOQ)
            return CA_E_PARAM;

        value0.bf.value = (value0.bf.value & (~0xf)) | cfg->voq;
    }

    /* update packet type based policing table */
    return __l3_te_pie_lpid_qos_voq_access(
            device_id,
            AAL_TBL_OP_FLAG_WR,
            index,
            (volatile ca_uint32_t *)&(value0.wrd));
#else
    return CA_E_OK;
#endif
}



ca_status_t aal_l3_te_pie_lpid_qos_voq_get
(
    ca_device_id_t               device_id,
    ca_port_id_t                 lpid,
    ca_uint32_t                  cos,
    aal_l3_te_pie_lpid_qos_voq_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_LPID_QOS_VOQ_DATA_t value0;
    ca_uint32_t index = (lpid << 3 | cos);

    if (!cfg)
        return CA_E_PARAM;

    if (cos > 7)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));

    /* get current configuration at first and then update specificed field */
    if (__l3_te_pie_lpid_qos_voq_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd))
            != CA_E_OK)
        return CA_E_ERROR;

    cfg->ena = (value0.bf.value >> 4) & 0x1;
    cfg->voq = value0.bf.value & 0xf;

#endif
    return CA_E_OK;

}

/**********************************************************************
** set the buffer queue depth marked for the PIE 1/3 depth;
** the configuration is precalculated number of bytes
** that are in the queue that specifies when we reach 1/3 queue depth
**********************************************************************/
ca_status_t aal_l3_te_pie_buff_threshold_set
(
    ca_device_id_t device_id,
    ca_uint32_t    size
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_buf_t value;

    if (size > 0x1fffff)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_buf);

    value.bf.one_third = size;

    CA_L3TE_REG_WRITE(TE_PIE_buf, value.wrd);

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_buff_threshold_get
(
    ca_device_id_t device_id,
    ca_uint32_t    *size
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_buf_t value;

    if (!size)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_buf);

    *size = value.bf.one_third;

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_dp_scaling_set
(
    ca_device_id_t                 device_id,
    aal_l3_te_pie_dp_scaling_msk_t msk,
    aal_l3_te_pie_dp_scaling_t     *value
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_dp0_t val;

    if (!value)
        return CA_E_PARAM;

    val.wrd = CA_L3TE_REG_READ(TE_PIE_dp0);

    if (msk.s.value_a)
    {
        if (value->value_a > 0x3f)
            return CA_E_PARAM;

        val.bf.A = value->value_a;
    }

    if (msk.s.value_b)
    {
        if (value->value_b > 0x3ff)
            return CA_E_PARAM;

        val.bf.B = value->value_b;
    }

    CA_L3TE_REG_WRITE(TE_PIE_dp0, val.wrd);

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_dp_scaling_get
(
    ca_device_id_t             device_id,
    aal_l3_te_pie_dp_scaling_t *value
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_dp0_t val;

    if (!value)
        return CA_E_PARAM;

    val.wrd = CA_L3TE_REG_READ(TE_PIE_dp0);

    value->value_a = val.bf.A;
    value->value_b = val.bf.B;

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_dp_ceiling_set
(
    ca_device_id_t device_id,
    ca_uint32_t    ceiling
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_dp1_t value;

    if (ceiling > 0xfffffff)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_dp1);

    value.bf.ceiling = ceiling;

    CA_L3TE_REG_WRITE(TE_PIE_dp1, value.wrd);

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_dp_ceiling_get
(
    ca_device_id_t device_id,
    ca_uint32_t    *ceiling
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_dp1_t value;

    if (!ceiling)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_dp1);

    *ceiling = value.bf.ceiling;

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_dp_cfg_set
(
    ca_device_id_t         device_id,
    ca_uint32_t            pie_voq_id,
    aal_l3_te_pie_dp_msk_t msk,
    aal_l3_te_pie_dp_cfg_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_dp2_0_t val20;
    TE_PIE_dp3_0_t val30;
    TE_PIE_dp4_0_t val40;
    TE_PIE_dp5_0_t val50;
    TE_PIE_dp6_0_t val60;
    TE_PIE_dp8_0_t val80;
    ca_uint32_t addr = 0;

    if (!cfg)
        return CA_E_PARAM;

    if (pie_voq_id > CA_AAL_L3_PIE_MAX_VOQ)
        return CA_E_PARAM;

    if (msk.s.burst_reset_timeout)
    {
        if (cfg->burst_reset_timeout > 0x1fffff)
            return CA_E_PARAM;

        addr = TE_PIE_dp2_0 + pie_voq_id * 4;

        val20.wrd = CA_L3TE_REG_READ(addr);

        val20.bf.burst_reset_timeout = cfg->burst_reset_timeout;

        CA_L3TE_REG_WRITE(addr, val20.wrd);
    }

    if (msk.s.latency_threshold_hi)
    {
        if (cfg->latency_threshold_hi > 0x1fffff)
            return CA_E_PARAM;

        addr = TE_PIE_dp3_0 + pie_voq_id * 4;

        val30.wrd = CA_L3TE_REG_READ(addr);

        val30.bf.latency_high = cfg->latency_threshold_hi;

        CA_L3TE_REG_WRITE(addr, val30.wrd);
    }

    if (msk.s.latency_threshold_lo)
    {
        if (cfg->latency_threshold_lo > 0x1fffff)
            return CA_E_PARAM;

        addr = TE_PIE_dp4_0 + pie_voq_id * 4;

        val40.wrd = CA_L3TE_REG_READ(addr);

        val40.bf.latency_low = cfg->latency_threshold_lo;

        CA_L3TE_REG_WRITE(addr, val40.wrd);
    }

    if (msk.s.latency_target)
    {
        if (cfg->latency_target > 0x1fffff)
            return CA_E_PARAM;

        addr = TE_PIE_dp5_0 + pie_voq_id * 4;

        val50.wrd = CA_L3TE_REG_READ(addr);

        val50.bf.latency_target = cfg->latency_target;

        CA_L3TE_REG_WRITE(addr, val50.wrd);
    }

    if (msk.s.max_service_rate)
    {
        addr = TE_PIE_dp6_0 + pie_voq_id * 4;

        val60.wrd = CA_L3TE_REG_READ(addr);

        val60.bf.msr = cfg->max_service_rate;

        CA_L3TE_REG_WRITE(addr, val60.wrd);
    }

    if (msk.s.peak_rate)
    {
        addr = TE_PIE_dp8_0 + pie_voq_id * 4;

        val80.wrd = CA_L3TE_REG_READ(addr);

        val80.bf.peak_rate = cfg->peak_rate;

        CA_L3TE_REG_WRITE(addr, val80.wrd);
    }


#endif
    return CA_E_OK;

}


ca_status_t aal_l3_te_pie_dp_cfg_get
(
    ca_device_id_t         device_id,
    ca_uint32_t            pie_voq_id,
    aal_l3_te_pie_dp_cfg_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_dp2_0_t val20;
    TE_PIE_dp3_0_t val30;
    TE_PIE_dp4_0_t val40;
    TE_PIE_dp5_0_t val50;
    TE_PIE_dp6_0_t val60;
    TE_PIE_dp8_0_t val80;

    if (!cfg)
        return CA_E_PARAM;

    if (pie_voq_id > CA_AAL_L3_PIE_MAX_VOQ)
        return CA_E_PARAM;

    val20.wrd = CA_L3TE_REG_READ(TE_PIE_dp2_0 + pie_voq_id * 4);
    val30.wrd = CA_L3TE_REG_READ(TE_PIE_dp3_0 + pie_voq_id * 4);
    val40.wrd = CA_L3TE_REG_READ(TE_PIE_dp4_0 + pie_voq_id * 4);
    val50.wrd = CA_L3TE_REG_READ(TE_PIE_dp5_0 + pie_voq_id * 4);
    val60.wrd = CA_L3TE_REG_READ(TE_PIE_dp6_0 + pie_voq_id * 4);
    val80.wrd = CA_L3TE_REG_READ(TE_PIE_dp8_0 + pie_voq_id * 4);

    cfg->burst_reset_timeout  = val20.bf.burst_reset_timeout;
    cfg->latency_threshold_hi = val30.bf.latency_high;
    cfg->latency_threshold_lo = val40.bf.latency_low;
    cfg->latency_target       = val50.bf.latency_target;
    cfg->max_service_rate     = val60.bf.msr;
    cfg->peak_rate            = val80.bf.peak_rate;

#endif
    return CA_E_OK;

}


ca_status_t aal_l3_te_pie_timer_set
(
    ca_device_id_t            device_id,
    aal_l3_te_pie_timer_msk_t msk,
    aal_l3_te_pie_timer_cfg_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_timer0_t val0;
    TE_PIE_timer1_t val1;
    TE_PIE_timer2_t val2;

    if (!cfg)
        return CA_E_PARAM;

    if (msk.s.scalar)
    {
        if (cfg->scalar > 0x1ff)
            return CA_E_PARAM;

        val0.wrd = CA_L3TE_REG_READ(TE_PIE_timer0);

        val0.bf.scalar = cfg->scalar;

        CA_L3TE_REG_WRITE(TE_PIE_timer0, val0.wrd);
    }

    if (msk.s.interval)
    {
        if (cfg->interval > 0x1fffff)
            return CA_E_PARAM;

        val1.wrd = CA_L3TE_REG_READ(TE_PIE_timer1);

        val1.bf.interval = cfg->interval;

        CA_L3TE_REG_WRITE(TE_PIE_timer1, val1.wrd);
    }

    if (msk.s.burst_dec)
    {
        if (cfg->burst_dec > 0x1fffff)
            return CA_E_PARAM;

        val2.wrd = CA_L3TE_REG_READ(TE_PIE_timer2);

        val2.bf.burst_decrement = cfg->burst_dec;

        CA_L3TE_REG_WRITE(TE_PIE_timer2, val2.wrd);
    }

#endif

    return CA_E_OK;
}

ca_status_t aal_l3_te_pie_timer_get
(
    ca_device_id_t            device_id,
    aal_l3_te_pie_timer_cfg_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_timer0_t val0;
    TE_PIE_timer1_t val1;
    TE_PIE_timer2_t val2;

    if (!cfg)
        return CA_E_PARAM;

    val0.wrd = CA_L3TE_REG_READ(TE_PIE_timer0);
    val1.wrd = CA_L3TE_REG_READ(TE_PIE_timer1);
    val2.wrd = CA_L3TE_REG_READ(TE_PIE_timer2);

    cfg->scalar    = val0.bf.scalar;
    cfg->interval  = val1.bf.interval;
    cfg->burst_dec = val2.bf.burst_decrement;
#endif

    return CA_E_OK;
}

ca_status_t aal_l3_te_pie_dp_qfull_set
(
    ca_device_id_t device_id,
    ca_uint32_t    threshold
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_pie_t value;

    if (threshold > 0x1fffff)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_pie);

    value.bf.q_full = threshold;

    CA_L3TE_REG_WRITE(TE_PIE_pie, value.wrd);

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_dp_qfull_get
(
    ca_device_id_t device_id,
    ca_uint32_t    *threshold
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_pie_t value;

    if (!threshold)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_pie);

    *threshold = value.bf.q_full;

#endif
    return CA_E_OK;
}

ca_status_t aal_l3_te_pie_dp_glb_cfg_set
(
    ca_device_id_t             device_id,
    aal_l3_te_pie_dp_glb_msk_t msk,
    aal_l3_te_pie_dp_glb_cfg_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_en0_t val0;
    TE_PIE_en1_t val1;
    TE_PIE_en2_t val2;
    TE_PIE_en3_t val3;
    TE_PIE_en4_t val4;

    if (!cfg)
        return CA_E_PARAM;

    if (msk.s.high)
    {
        val0.wrd = CA_L3TE_REG_READ(TE_PIE_en0);

        val0.bf.high = cfg->high;

        CA_L3TE_REG_WRITE(TE_PIE_en0, val0.wrd);
    }

    if (msk.s.low)
    {
        val1.wrd = CA_L3TE_REG_READ(TE_PIE_en1);

        val1.bf.low = cfg->low;

        CA_L3TE_REG_WRITE(TE_PIE_en1, val0.wrd);
    }

    if (msk.s.random_disable)
    {
        val2.wrd = CA_L3TE_REG_READ(TE_PIE_en2);

        val2.bf.random_disable = cfg->random_disable;

        CA_L3TE_REG_WRITE(TE_PIE_en2, val2.wrd);
    }

if (msk.s.threshold)
{
    val3.wrd = CA_L3TE_REG_READ(TE_PIE_en3);

    val3.bf.threshold = cfg->threshold;

    CA_L3TE_REG_WRITE(TE_PIE_en3, val3.wrd);
}

if (msk.s.burst)
{
    val4.wrd = CA_L3TE_REG_READ(TE_PIE_en4);

    val4.bf.burst = cfg->burst;

    CA_L3TE_REG_WRITE(TE_PIE_en4, val4.wrd);
}



#endif
    return CA_E_OK;

}


ca_status_t aal_l3_te_pie_dp_glb_cfg_get
(
    ca_device_id_t             device_id,
    aal_l3_te_pie_dp_glb_cfg_t *cfg
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_en0_t val0;
    TE_PIE_en1_t val1;
    TE_PIE_en2_t val2;
    TE_PIE_en3_t val3;
    TE_PIE_en4_t val4;

    if (!cfg)
        return CA_E_PARAM;

    val0.wrd = CA_L3TE_REG_READ(TE_PIE_en0);
    val1.wrd = CA_L3TE_REG_READ(TE_PIE_en1);
    val2.wrd = CA_L3TE_REG_READ(TE_PIE_en2);
    val3.wrd = CA_L3TE_REG_READ(TE_PIE_en3);
    val4.wrd = CA_L3TE_REG_READ(TE_PIE_en4);

    cfg->high  = val0.bf.high;
    cfg->low   = val1.bf.low;
    cfg->random_disable = val2.bf.random_disable;
    cfg->threshold      = val3.bf.threshold;
    cfg->burst          = val4.bf.burst;
#endif
    return CA_E_OK;

}

/* size is in unit of 2^(x). for example, 128 bytes packet, its value should be 7 */
ca_status_t aal_l3_te_pie_mean_pkt_set
(
    ca_device_id_t device_id,
    ca_uint32_t    size
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_mean_t value;

    if (size > 0xf)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_mean);

    value.bf.size = size;

    CA_L3TE_REG_WRITE(TE_PIE_mean, value.wrd);

#endif
    return CA_E_OK;

}

ca_status_t aal_l3_te_pie_mean_pkt_get
(
    ca_device_id_t device_id,
    ca_uint32_t    *size
)
{
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    TE_PIE_mean_t value;

    if (!size)
        return CA_E_PARAM;

    value.wrd = CA_L3TE_REG_READ(TE_PIE_mean);

    *size = value.bf.size;

#endif
    return CA_E_OK;

}


/*
** L3 TE module initialization function
*/
ca_status_t aal_l3_te_init
(
    ca_device_id_t device_id
)
{
    aal_l3_te_glb_cfg_t         te_cfg;
    aal_l3_te_glb_cfg_msk_t     te_msk;
    aal_l3_te_policer_cfg_msk_t l3_policer_msk;
    aal_l3_te_policer_cfg_t     l3_policer_cfg;
    ca_uint32_t i = 0;

    /*
    ** disable l3 shaper port group function, each 8 cpu ports  and 8 NI ports
    ** shall have individual port shaper
    */
#if 0//yocto
    aal_scfg_read(CFG_ID_L3TE_WRED_MARK_IDX, 4, &l3_te_wred_mark_idx);
    aal_scfg_read(CFG_ID_L3TE_WRED_UNMARK_IDX, 4, &l3_te_wred_unmark_idx);
#else//sd1 uboot
        l3_te_wred_mark_idx = 8;
        l3_te_wred_unmark_idx = 8;
#endif
    (void)aal_l3_te_shaper_ni_port_group_set(device_id, FALSE);
    (void)aal_l3_te_shaper_cpu_port_group_set(device_id, FALSE);

    /* statistics counter settings */
    (void)aal_l3_te_pm_enable_set(device_id, TRUE);
    (void)aal_l3_te_pm_read_clr_set(device_id, TRUE);
    aal_l3_te_pm_cnt_mem_set(device_id, 0, 0, 0);
    aal_l3_te_pm_cnt_mem_set(device_id, 1, 1, 1);

    /* enable wred, policer, shaper, taildrop etc.*/
    memset((void *)&te_cfg, 0, sizeof(te_cfg));
    te_msk.u32 = 0;

    te_msk.s.policer_ena   = 1;
    te_msk.s.shaper_ena    = 1;
    te_msk.s.tail_drop_ena = 1;
    te_msk.s.wred_ena      = 1;
    te_msk.s.mark_ecn_ena  = 1;
    te_msk.s.pol_ecn_ena   = 1;
    te_msk.s.wred_ecn_ena  = 1;
    te_msk.s.l2_tail_drop_ena = 1;

    te_cfg.policer_ena   = CA_TRUE;
    te_cfg.shaper_ena    = CA_TRUE;
    te_cfg.tail_drop_ena = CA_TRUE;
    te_cfg.wred_ena      = CA_TRUE;
    te_cfg.mark_ecn_ena  = CA_FALSE;
    te_cfg.pol_ecn_ena   = CA_FALSE;
    te_cfg.wred_ecn_ena  = CA_FALSE;
    te_cfg.l2_tail_drop_ena = CA_TRUE;

    (void)aal_l3_te_glb_cfg_set(device_id, te_msk, &te_cfg);

    /* policer settings */
    memset((void *)&l3_policer_cfg, 0, sizeof(l3_policer_cfg));
    memset((void *)&l3_policer_msk, 0, sizeof(l3_policer_msk));

    l3_policer_msk.s.ipg = 1;
    l3_policer_cfg.ipg   = 24;  /* IPG 20 to 24, bug 50074 */
    for (i=0; i<=CA_AAL_L3_MAX_FLOW_ID; i++)
    {
        if (aal_l3_te_policer_flow_cfg_set(device_id, i, l3_policer_msk, &l3_policer_cfg) != CA_E_OK)
            CA_TM_LOG("aal_l3_te_policer_flow_cfg_set() : failed to set policer configurations for l3 flow %d, L%d\n", i, __LINE__);
    }

    for (i=0; i<=CA_AAL_L3_MAX_PORT_ID; i++)
    {
        if (aal_l3_te_policer_port_cfg_set(device_id, i, l3_policer_msk, &l3_policer_cfg) != CA_E_OK)
            CA_TM_LOG("aal_l3_te_policer_port_cfg_set() : failed to set policer configurations for l3 port %d, L%d\n", i, __LINE__);
    }

    for (i=0; i<CA_AAL_L3_POLICER_PKT_TYPE_END; i++)
    {
        if (aal_l3_te_policer_pkt_type_cfg_set(device_id, i, l3_policer_msk, &l3_policer_cfg) != CA_E_OK)
            CA_TM_LOG("aal_l3_te_policer_pkt_type_cfg_set() : failed to set policer configurations for l3 packet type %d, L%d\n", i, __LINE__);
    }

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
        TE_SHP_TB_CTRL_t ctrl;

        ctrl.wrd = CA_L3TE_REG_READ(TE_SHP_TB_CTRL);

        ctrl.bf.upd_cnt_lmt = 3;

        CA_L3TE_REG_WRITE(TE_SHP_TB_CTRL, ctrl.wrd);

    }while(0);

    do
    {   /* policer update */
        TE_POL_TB_CTRL_t ctrl;

        ctrl.wrd = CA_L3TE_REG_READ(TE_POL_TB_CTRL);

        ctrl.bf.upd_cnt_lmt = 3;

        CA_L3TE_REG_WRITE(TE_POL_TB_CTRL, ctrl.wrd);
    } while(0);

    #endif

    /* reset WRED software table */
    memset((void *)&g_aal_l3_te_wred_profile_info[0], 0x0, sizeof(g_aal_l3_te_wred_profile_info));
    memset((void *)&g_aal_l3_te_wred_sel_info[0][0], 0xff, sizeof(g_aal_l3_te_wred_sel_info));

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("L3_TE : initialize WRED profile & selection table ... ");
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    do
    {
        /* setup WRED profile and selection table */
        ca_uint32_t i = 0, j = 0, index = 0;

        if (aal_l3_te_wred_profile_info_allocate(&index))
        {
            for (i=0; i<=CA_AAL_L3_WRED_MAX_PORT_ID; i++)
            {
                for (j=0; j<8; j++)
                {
                    aal_l3_te_wred_profile_info_ref_inc(index); /* reference counter ++ */

                    /* update software selection table */
                    aal_l3_te_wred_sel_info_set(i, j, index);
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
EXPORT_SYMBOL(aal_l3_te_status_get);
EXPORT_SYMBOL(aal_l3_te_glb_cfg_set);
EXPORT_SYMBOL(aal_l3_te_glb_cfg_get);

EXPORT_SYMBOL(aal_l3_te_shaper_ni_port_group_set);
EXPORT_SYMBOL(aal_l3_te_shaper_ni_port_group_get);
EXPORT_SYMBOL(aal_l3_te_shaper_cpu_port_group_set);
EXPORT_SYMBOL(aal_l3_te_shaper_cpu_port_group_get);
EXPORT_SYMBOL(aal_l3_te_shaper_voq_tbc_set);
EXPORT_SYMBOL(aal_l3_te_shaper_voq_tbc_get);
EXPORT_SYMBOL(aal_l3_te_shaper_port_tbc_set);
EXPORT_SYMBOL(aal_l3_te_shaper_port_tbc_get);
EXPORT_SYMBOL(aal_l3_te_shaper_group_tbc_set);
EXPORT_SYMBOL(aal_l3_te_shaper_group_tbc_get);
EXPORT_SYMBOL(aal_l3_te_shaper_ipq_set);
EXPORT_SYMBOL(aal_l3_te_shaper_ipq_get);
EXPORT_SYMBOL(aal_l3_te_shaper_port_ipq_set);
EXPORT_SYMBOL(aal_l3_te_shaper_port_ipq_get);



EXPORT_SYMBOL(aal_l3_te_policer_pkt_type_cfg_set);
EXPORT_SYMBOL(aal_l3_te_policer_pkt_type_cfg_get);
EXPORT_SYMBOL(aal_l3_te_policer_pkt_type_profile_set);
EXPORT_SYMBOL(aal_l3_te_policer_pkt_type_profile_get);
EXPORT_SYMBOL(aal_l3_te_policer_pkt_type_stats_get);


EXPORT_SYMBOL(aal_l3_te_policer_port_cfg_set);
EXPORT_SYMBOL(aal_l3_te_policer_port_cfg_get);
EXPORT_SYMBOL(aal_l3_te_policer_port_profile_set);
EXPORT_SYMBOL(aal_l3_te_policer_port_profile_get);
EXPORT_SYMBOL(aal_l3_te_policer_port_stats_get);

EXPORT_SYMBOL(aal_l3_te_policer_flow_cfg_set);
EXPORT_SYMBOL(aal_l3_te_policer_flow_cfg_get);
EXPORT_SYMBOL(aal_l3_te_policer_flow_profile_set);
EXPORT_SYMBOL(aal_l3_te_policer_flow_profile_get);
EXPORT_SYMBOL(aal_l3_te_policer_flow_stats_get);

EXPORT_SYMBOL(aal_l3_te_policer_agr_flow_profile_set);
EXPORT_SYMBOL(aal_l3_te_policer_agr_flow_profile_get);
EXPORT_SYMBOL(aal_l3_te_policer_agr_flow_stats_get);

EXPORT_SYMBOL(aal_l3_te_wred_profile_set);
EXPORT_SYMBOL(aal_l3_te_wred_profile_get);
EXPORT_SYMBOL(aal_l3_te_wred_profile_sel_set);
EXPORT_SYMBOL(aal_l3_te_wred_profile_sel_get);
EXPORT_SYMBOL(aal_l3_te_wred_info_lookup);
EXPORT_SYMBOL(aal_l3_te_wred_profile_info_allocate);
EXPORT_SYMBOL(aal_l3_te_wred_profile_info_set);
EXPORT_SYMBOL(aal_l3_te_wred_profile_info_get);
EXPORT_SYMBOL(aal_l3_te_wred_profile_info_ref_counter_get);
EXPORT_SYMBOL(aal_l3_te_wred_profile_info_ref_inc);
EXPORT_SYMBOL(aal_l3_te_wred_profile_info_ref_dec);
EXPORT_SYMBOL(aal_l3_te_wred_sel_info_set);
EXPORT_SYMBOL(aal_l3_te_wred_sel_info_get);
EXPORT_SYMBOL(aal_l3_te_wred_debug_get);


EXPORT_SYMBOL(aal_l3_te_pm_policer_flow_get);
EXPORT_SYMBOL(aal_l3_te_pm_policer_agr_flow_get);
EXPORT_SYMBOL(aal_l3_te_pm_policer_port_get);
EXPORT_SYMBOL(aal_l3_te_pm_policer_pkt_type_get);
EXPORT_SYMBOL(aal_l3_te_pm_egress_voq_get);
EXPORT_SYMBOL(aal_l3_te_pm_enable_set);
EXPORT_SYMBOL(aal_l3_te_pm_enable_get);
EXPORT_SYMBOL(aal_l3_te_pm_read_clr_set);
EXPORT_SYMBOL(aal_l3_te_pm_read_clr_get);
EXPORT_SYMBOL(aal_l3_te_pm_cnt_mem_set);
EXPORT_SYMBOL(aal_l3_te_pm_cnt_mem_get);
EXPORT_SYMBOL(aal_l3_te_pm_stats_get);
EXPORT_SYMBOL(aal_l3_te_pm_status_get);
EXPORT_SYMBOL(aal_l3_te_pie_lpid_qos_voq_set);
EXPORT_SYMBOL(aal_l3_te_pie_lpid_qos_voq_get);
EXPORT_SYMBOL(aal_l3_te_pie_buff_threshold_set);
EXPORT_SYMBOL(aal_l3_te_pie_buff_threshold_get);
EXPORT_SYMBOL(aal_l3_te_pie_dp_scaling_set);
EXPORT_SYMBOL(aal_l3_te_pie_dp_scaling_get);
EXPORT_SYMBOL(aal_l3_te_pie_dp_ceiling_set);
EXPORT_SYMBOL(aal_l3_te_pie_dp_ceiling_get);
EXPORT_SYMBOL(aal_l3_te_pie_dp_cfg_set);
EXPORT_SYMBOL(aal_l3_te_pie_dp_cfg_get);
EXPORT_SYMBOL(aal_l3_te_pie_timer_set);
EXPORT_SYMBOL(aal_l3_te_pie_timer_get);
EXPORT_SYMBOL(aal_l3_te_pie_dp_qfull_set);
EXPORT_SYMBOL(aal_l3_te_pie_dp_qfull_get);
EXPORT_SYMBOL(aal_l3_te_pie_dp_glb_cfg_set);
EXPORT_SYMBOL(aal_l3_te_pie_dp_glb_cfg_get);
EXPORT_SYMBOL(aal_l3_te_pie_mean_pkt_set);
EXPORT_SYMBOL(aal_l3_te_pie_mean_pkt_get);


EXPORT_SYMBOL(aal_l3_te_init);
#endif
