#include <generated/ca_ne_autoconf.h>
#include "ca_types.h"
#include "osal_cmn.h"
#include <soc/cortina/registers.h>
#include "aal_common.h"
#include "aal_l2_qm.h"
#include "aal_l2_tm.h"

#if 0//yocto
#define CA_L2QM_REG_READ(addr)        CA_REG_READ(AAL_NE_REG_ADDR(addr))
#define CA_L2QM_REG_WRITE(addr, data) CA_REG_WRITE(data, AAL_NE_REG_ADDR(addr))
#else//sd1 uboot for 98f
#define CA_L2QM_REG_READ(addr)        CA_REG_READ((volatile uintptr_t)(addr))
#define CA_L2QM_REG_WRITE(addr, data) CA_REG_WRITE(data, (volatile uintptr_t)(addr))
#endif

#ifdef __L2_QM_LOCAL_FUNC
#endif


static inline ca_status_t __l2_qm_port_prvt_buff_cnt_mem_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data
)
{
    L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2QM_REG_WRITE(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_DATA, *data);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2QM_REG_WRITE(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data = CA_L2QM_REG_READ(L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_DATA);
    }

    return CA_E_OK;

}


static inline ca_status_t __l2_qm_voq_info_mem_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TM_QM_VOQINFO_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_VOQINFO_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_VOQINFO_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2QM_REG_WRITE(L2TM_L2TM_QM_VOQINFO_MEM_DATA0, *data0);
        CA_L2QM_REG_WRITE(L2TM_L2TM_QM_VOQINFO_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2QM_REG_WRITE(L2TM_L2TM_QM_VOQINFO_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_VOQINFO_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_VOQINFO_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2QM_REG_READ(L2TM_L2TM_QM_VOQINFO_MEM_DATA0);
        *data1 = CA_L2QM_REG_READ(L2TM_L2TM_QM_VOQINFO_MEM_DATA1);
    }

    return CA_E_OK;

}


static inline ca_status_t __l2_qm_freeq_info_mem_access
(
    ca_device_id_t          device_id,
    aal_tbl_op_flag_t    op_flag,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data0,
    volatile ca_uint32_t *data1
)
{
    L2TM_L2TM_QM_FREEQ_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_FREEQ_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_FREEQ_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    if (op_flag == AAL_TBL_OP_FLAG_WR)
    {
        /* the operation to write table */

        CA_L2QM_REG_WRITE(L2TM_L2TM_QM_FREEQ_MEM_DATA0, *data0);
        CA_L2QM_REG_WRITE(L2TM_L2TM_QM_FREEQ_MEM_DATA1, *data1);
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = op_flag;
    access.bf.access   = 1;

    CA_L2QM_REG_WRITE(L2TM_L2TM_QM_FREEQ_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */
    if (op_flag == AAL_TBL_OP_FLAG_RD)
    {
        i = 0;

        access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_FREEQ_MEM_ACCESS);
        while (access.bf.access)
        {
            access.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_FREEQ_MEM_ACCESS);
            if (i++ >= AAL_TBL_OP_TIMEOUT)
            {
                return CA_E_TIMEOUT;
            }
        }

        *data0 = CA_L2QM_REG_READ(L2TM_L2TM_QM_FREEQ_MEM_DATA0);
        *data1 = CA_L2QM_REG_READ(L2TM_L2TM_QM_FREEQ_MEM_DATA1);
    }

    return CA_E_OK;

}


#ifdef __L2_QM
#endif


ca_status_t aal_l2_qm_init(ca_device_id_t device_id)
{
    aal_l2_qm_eq_cfg_msk_t   qm_msk;
    aal_l2_qm_eq_cfg_t       qm_cfg;
    aal_l2_qm_buff_cfg_t     buff_cfg;
    aal_l2_qm_buff_cfg_msk_t buff_msk;

    /* empty queue initilization */
    memset((void *)&qm_cfg, 0, sizeof(qm_cfg));
    qm_msk.u32 = 0;

    qm_msk.s.eq_ena      = 1;
    qm_msk.s.buff_num    = 1;
    qm_msk.s.prvt_buff_num = 1;


    qm_cfg.eq_ena        = FALSE;
    qm_cfg.buff_num      = 0;     /* eq0 : 0 means (32K-1) * 32bytes packet buffer, 1M */
    qm_cfg.prvt_buff_num = 0;

#ifdef CONFIG_ARCH_CORTINA_G3LITE
    /* G3 lite buffer size is a half of G3 and performance requirement is lesser than 5Gbps; so EQ0(32 bytes) is used */

    (void)aal_l2_qm_eq_cfg_set(device_id, 1, qm_msk, &qm_cfg); /* configure eq1 */

    qm_cfg.eq_ena        = TRUE;
    qm_cfg.buff_num      = 4;   /* 4k * 32 = 128k buffer,  for lesser than 5Gbps performance case */
    qm_cfg.prvt_buff_num = CA_AAL_L2_QM_PORT_PRVT_BUFF_NUM;    /* in unit of 16 buffers */

    (void)aal_l2_qm_eq_cfg_set(device_id, 0, qm_msk, &qm_cfg); /* configure eq0 */
#elif defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
    (void)aal_l2_qm_eq_cfg_set(device_id, 0, qm_msk, &qm_cfg); /* configure eq0 */

    qm_cfg.eq_ena        = TRUE;
    qm_cfg.buff_num      = 8;     /* SFU : 8K * 64 buffer */
    qm_cfg.prvt_buff_num = CA_AAL_L2_QM_PORT_PRVT_BUFF_NUM;    /* in unit of 16 buffers */

    (void)aal_l2_qm_eq_cfg_set(device_id, 1, qm_msk, &qm_cfg); /* configure eq1 */
#else
    (void)aal_l2_qm_eq_cfg_set(device_id, 0, qm_msk, &qm_cfg); /* configure eq0 */

    qm_cfg.eq_ena        = TRUE;
    qm_cfg.buff_num      = 4;     /* eq1 : 0 means (16K-1) * 64 bytes packet buffer, 1M, better performance */
                                  /* for 5Gbps traffic, using 64 bytes buffer, 4K * 64 =256K */
    qm_cfg.prvt_buff_num = CA_AAL_L2_QM_PORT_PRVT_BUFF_NUM;    /* in unit of 16 buffers */

    (void)aal_l2_qm_eq_cfg_set(device_id, 1, qm_msk, &qm_cfg); /* configure eq1 */
#endif

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("L2_QM : EQ-0(0), EQ-1(4K) - system uses 4K * 64bytes buffer configuration.\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    /* port private buffer configuration, all port selects profile 0 by default */
    (void)aal_l2_qm_port_prvt_profile_set(device_id, 0, CA_AAL_L2_QM_PORT_PRVT_BUFF_NUM);

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("L2_QM : port private buffer number is %d\n", CA_AAL_L2_QM_PORT_PRVT_BUFF_NUM);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    /* gobal buffer configuration */
    memset((void *)&buff_cfg, 0x0, sizeof(buff_cfg));
    buff_msk.u32 = 0;

    buff_msk.s.fe_bp_ena          = 1;
    buff_msk.s.drop_ena           = 1;
    buff_msk.s.no_drop_threshold  = 1;
    buff_msk.s.non_cong_threshold = 1;

    buff_cfg.fe_bp_ena          = CA_FALSE;  /* back-pressure to FE */
    buff_cfg.drop_ena           = CA_TRUE;
    buff_cfg.no_drop_threshold  = 0x21;
    buff_cfg.non_cong_threshold = 0x4000;    /* BM is non-congestion state if aviable buffer is more than 2K */
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
    buff_cfg.non_cong_threshold = 0x1000;
#endif
    (void)aal_l2_qm_buff_cfg_set(device_id, buff_msk, &buff_cfg);

    return CA_E_OK;
}

ca_status_t aal_l2_qm_intr_ena_set
(
    ca_device_id_t           device_id,
    aal_l2_qm_intr_ena_msk_t msk,
    aal_l2_qm_intr_ena_t     *cfg
)
{
    ca_uint32_t addr = L2TM_L2TM_QM_INTE;
    L2TM_L2TM_QM_INTE_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    if (msk.s.freeq_ecc_correctederr_en)
    {
        ctrl.bf.freeq_ecc_correctederrE = cfg->freeq_ecc_correctederr_en;
    }

    if (msk.s.freeq_ecc_err_en)
    {
        ctrl.bf.freeq_ecc_errE = cfg->freeq_ecc_err_en;
    }

    if (msk.s.voqinfo_ecc_correctederr_en)
    {
        ctrl.bf.voqinfo_ecc_correctederrE = cfg->voqinfo_ecc_correctederr_en;
    }

    if (msk.s.voqinfo_ecc_err_en)
    {
        ctrl.bf.voqinfo_ecc_errE = cfg->voqinfo_ecc_err_en;
    }

    if (msk.s.no_buff_drop_en)
    {
        ctrl.bf.nobuf_dropE = cfg->no_buff_drop_en;
    }

    if (msk.s.bid_out_of_range_en)
    {
        ctrl.bf.bid_out_of_rangeE = cfg->bid_out_of_range_en;
    }

    if (msk.s.wr_race_err_en)
    {
        ctrl.bf.wr_race_errE = cfg->wr_race_err_en;
    }

    if (msk.s.eq0_stack_err_en)
    {
        ctrl.bf.eq0_stack_errE = cfg->eq0_stack_err_en;
    }

    if (msk.s.eq1_stack_err_en)
    {
        ctrl.bf.eq1_stack_errE = cfg->eq1_stack_err_en;
    }

    if (msk.s.enq_ff_undrflo_en)
    {
        ctrl.bf.enq_ff_undrfloE = cfg->enq_ff_undrflo_en;
    }

    if (msk.s.enq_ff_ovrflo_en)
    {
        ctrl.bf.enq_ff_ovrfloE = cfg->enq_ff_ovrflo_en;
    }

    if (msk.s.deq_ff_undrflo_en)
    {
        ctrl.bf.deq_ff_undrfloE = cfg->deq_ff_undrflo_en;
    }

    if (msk.s.deq_ff_ovrflo_en)
    {
        ctrl.bf.deq_ff_ovrfloE = cfg->deq_ff_ovrflo_en;
    }

    CA_L2QM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_qm_intr_ena_get
(
    ca_device_id_t       device_id,
    aal_l2_qm_intr_ena_t *cfg
)
{
    ca_uint32_t addr = L2TM_L2TM_QM_INTE;
    L2TM_L2TM_QM_INTE_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    cfg->freeq_ecc_correctederr_en    = ctrl.bf.freeq_ecc_correctederrE;
    cfg->freeq_ecc_err_en             = ctrl.bf.freeq_ecc_errE;
    cfg->voqinfo_ecc_correctederr_en  = ctrl.bf.voqinfo_ecc_correctederrE;
    cfg->voqinfo_ecc_err_en           = ctrl.bf.voqinfo_ecc_errE;
    cfg->no_buff_drop_en              = ctrl.bf.nobuf_dropE;
    cfg->bid_out_of_range_en          = ctrl.bf.bid_out_of_rangeE;
    cfg->wr_race_err_en    = ctrl.bf.wr_race_errE;
    cfg->eq0_stack_err_en  = ctrl.bf.eq0_stack_errE;
    cfg->eq1_stack_err_en  = ctrl.bf.eq1_stack_errE;
    cfg->enq_ff_undrflo_en = ctrl.bf.enq_ff_undrfloE;
    cfg->enq_ff_ovrflo_en  = ctrl.bf.enq_ff_ovrfloE;
    cfg->deq_ff_undrflo_en = ctrl.bf.deq_ff_undrfloE;
    cfg->deq_ff_ovrflo_en  = ctrl.bf.deq_ff_ovrfloE;

    return CA_E_OK;
}




ca_status_t aal_l2_qm_eq_stack_cfg_set
(
    ca_device_id_t               device_id,
    ca_uint8_t                eq_id,
    aal_l2_qm_eq_stack_cfg_msk_t msk,
    aal_l2_qm_eq_stack_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ0_STACK_CFG_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    if (eq_id > 1)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TM_QM_EQ0_STACK_CFG + eq_id * 4;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    if (msk.s.low_threshold)
    {
        ctrl.bf.low_ths = cfg->low_threshold;
    }

    if (msk.s.mid_threshold)
    {
        ctrl.bf.mid_ths = cfg->mid_threshold;
    }

    if (msk.s.high_threshold)
    {
        ctrl.bf.high_ths = cfg->high_threshold;
    }

    CA_L2QM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_qm_eq_stack_cfg_get
(
    ca_device_id_t           device_id,
    ca_uint8_t            eq_id,
    aal_l2_qm_eq_stack_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ0_STACK_CFG_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    if (eq_id > 1)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_EQ0_STACK_CFG + eq_id * 4;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    cfg->low_threshold = ctrl.bf.low_ths;
    cfg->mid_threshold = ctrl.bf.mid_ths;
    cfg->high_threshold = ctrl.bf.high_ths;

    return CA_E_OK;
}

ca_status_t aal_l2_qm_buff_cfg_set
(
    ca_device_id_t           device_id,
    aal_l2_qm_buff_cfg_msk_t msk,
    aal_l2_qm_buff_cfg_t     *cfg
)
{
    L2TM_L2TM_QM_GLOB_BUF_CFG_t value;

    if (!cfg)
        return CA_E_PARAM;

    value.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_GLOB_BUF_CFG);

    if (msk.s.no_drop_threshold)
    {
        if (cfg->no_drop_threshold > 0x7fff)
            return CA_E_PARAM;

        value.bf.nodrop  = cfg->no_drop_threshold;
    }

    if (msk.s.drop_ena)
    {
        value.bf.drop_en  = cfg->drop_ena ? 1 : 0;
    }

    if (msk.s.non_cong_threshold)
    {
        if (cfg->no_drop_threshold > 0x7fff)
            return CA_E_PARAM;

        value.bf.noncong  = cfg->non_cong_threshold;
    }

    if (msk.s.fe_bp_ena)
    {
        value.bf.fe_bp_en  = cfg->fe_bp_ena ? 1 : 0;
    }

    CA_L2QM_REG_WRITE(L2TM_L2TM_QM_GLOB_BUF_CFG, value.wrd);


    return CA_E_OK;
}

ca_status_t aal_l2_qm_buff_cfg_get
(
    ca_device_id_t           device_id,
    aal_l2_qm_buff_cfg_t     *cfg
)
{
    L2TM_L2TM_QM_GLOB_BUF_CFG_t value;

    if (!cfg)
        return CA_E_PARAM;

    value.wrd = CA_L2QM_REG_READ(L2TM_L2TM_QM_GLOB_BUF_CFG);

    cfg->no_drop_threshold  = value.bf.nodrop;
    cfg->drop_ena           = value.bf.drop_en ? TRUE : FALSE;
    cfg->non_cong_threshold = value.bf.noncong;
    cfg->fe_bp_ena          = value.bf.fe_bp_en ? TRUE : FALSE;

    return CA_E_OK;
}



ca_status_t aal_l2_qm_eq_cfg_set
(
    ca_device_id_t         device_id,
    ca_uint8_t          eq_id,
    aal_l2_qm_eq_cfg_msk_t msk,
    aal_l2_qm_eq_cfg_t     *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ_CFG_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    if (eq_id > 1)
        return CA_E_PARAM;

    if (!msk.u32)
        return CA_E_OK;

    addr = L2TM_L2TM_QM_EQ_CFG;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    if (msk.s.eq_ena)
    {
        if (eq_id)
            ctrl.bf.eq1_en = cfg->eq_ena;
        else
            ctrl.bf.eq0_en = cfg->eq_ena;
    }

    if (msk.s.buff_num)
    {
        if (eq_id)
        {
            if (cfg->buff_num > 0xf)
                return CA_E_PARAM;

            ctrl.bf.eq1_bufnum = cfg->buff_num;
        }
        else
        {
            if (cfg->buff_num > 0x1f)
                return CA_E_PARAM;

            ctrl.bf.eq0_bufnum = cfg->buff_num;
        }
    }

    if (msk.s.prvt_buff_num)
    {
        if (eq_id)
        {
            if (cfg->prvt_buff_num > 0x3ff)
                return CA_E_PARAM;

            ctrl.bf.eq1_prvt = cfg->prvt_buff_num;
        }
        else
        {
            if (cfg->prvt_buff_num > 0x7ff)
                return CA_E_PARAM;

            ctrl.bf.eq0_prvt = cfg->prvt_buff_num;
        }
    }



    CA_L2QM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}



ca_status_t aal_l2_qm_eq_cfg_get
(
    ca_device_id_t     device_id,
    ca_uint8_t      eq_id,
    aal_l2_qm_eq_cfg_t *cfg
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ_CFG_t ctrl;

    if (!cfg)
        return CA_E_PARAM;

    if (eq_id > 1)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_EQ_CFG;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    if (eq_id)
    {
        cfg->eq_ena        = ctrl.bf.eq1_en;
        cfg->buff_num      = ctrl.bf.eq1_bufnum;
        cfg->prvt_buff_num = ctrl.bf.eq1_prvt;
    }
    else
    {
        cfg->eq_ena        = ctrl.bf.eq0_en;
        cfg->buff_num      = ctrl.bf.eq0_bufnum;
        cfg->prvt_buff_num = ctrl.bf.eq0_prvt;
    }

    return CA_E_OK;
}

ca_status_t aal_l2_qm_port_prvt_sel_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  profile_id
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_PORT_PRVT_SELECT0_t ctrl;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (profile_id > CA_AAL_L2_QM_MAX_PORT_PRVT_PROFILE_ID)
        return CA_E_PARAM;

    addr = (port_id < 8) ? L2TM_L2TM_QM_PORT_PRVT_SELECT0 : L2TM_L2TM_QM_PORT_PRVT_SELECT1;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    ctrl.wrd &= ~(0xfff << (port_id % 8) * 4);
    ctrl.wrd |= (profile_id << (port_id % 8) * 4);

    CA_L2QM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_qm_port_prvt_sel_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *profile_id
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_PORT_PRVT_SELECT0_t ctrl;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    if (!profile_id)
        return CA_E_PARAM;

    addr = (port_id < 8) ? L2TM_L2TM_QM_PORT_PRVT_SELECT0 : L2TM_L2TM_QM_PORT_PRVT_SELECT1;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    *profile_id = (ctrl.wrd >> ((port_id % 8) * 4)) & 0x7;

    return CA_E_OK;
}


ca_status_t aal_l2_qm_port_prvt_profile_set
(
    ca_device_id_t device_id,
    ca_uint32_t profile_id,
    ca_uint32_t buff_nums
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_PORT_PRVT_PROFILE0_t ctrl;

    if (profile_id > CA_AAL_L2_QM_MAX_PORT_PRVT_PROFILE_ID) // up to 8 profiles in total
        return CA_E_PARAM;

    if (buff_nums > 0x7fff)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_PORT_PRVT_PROFILE0 + (profile_id / 2 * 4);

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    ctrl.wrd &= ~(0x7fff << (16 * (profile_id % 2)));
    ctrl.wrd |= (buff_nums << (16 * (profile_id % 2)));

    CA_L2QM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_qm_port_prvt_profile_get
(
    ca_device_id_t device_id,
    ca_uint32_t profile_id,
    ca_uint32_t *buff_nums
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_PORT_PRVT_PROFILE0_t ctrl;

    if (!buff_nums)
        return CA_E_PARAM;

    if (profile_id > CA_AAL_L2_QM_MAX_PORT_PRVT_PROFILE_ID) // up to 8 profiles in total
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_PORT_PRVT_PROFILE0 + (profile_id / 2 * 4);

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    *buff_nums = (ctrl.wrd >> (16 * (profile_id % 2))) & 0x7fff;

    return CA_E_OK;
}

ca_status_t aal_l2_qm_port_prvt_buff_status_get
(
    ca_device_id_t device_id,
    ca_uint32_t *status
)
{
    L2TM_L2TM_QM_PORT_NO_PRVT_STS_t ctrl;
    ca_uint32_t addr = 0;

    if (!status)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_PORT_NO_PRVT_STS;

    ctrl.wrd = CA_L2QM_REG_READ(addr);

    *status = ctrl.wrd;

    return CA_E_OK;
}

ca_status_t aal_l2_qm_port_prvt_buff_cnt_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *cnt
)
{
    L2TM_L2TM_QM_PORT_PRVT_BUFCNT_MEM_DATA_t value0;
    ca_uint32_t index = port_id;

    if (!cnt)
        return CA_E_PARAM;

    if (port_id > CA_AAL_MAX_PORT_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));

    /* read table */
    if (__l2_qm_port_prvt_buff_cnt_mem_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd))
            != CA_E_OK)
            return CA_E_ERROR;

    *cnt = value0.wrd;

    return CA_E_OK;
}



ca_status_t aal_l2_qm_eq_free_cnt_get
(
    ca_device_id_t device_id,
    ca_uint8_t  eq_id,
    ca_uint32_t *free_cnt
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ_FREECNT_t cnt;

    if (!free_cnt)
        return CA_E_PARAM;

    if (eq_id > 1)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_EQ_FREECNT;

    cnt.wrd = CA_L2QM_REG_READ(addr);

    if (eq_id)
        *free_cnt = cnt.bf.eq1;
    else
        *free_cnt = cnt.bf.eq0;

    return CA_E_OK;
}

ca_status_t aal_l2_qm_eq_glb_free_cnt_get
(
    ca_device_id_t device_id,
    ca_uint32_t *free_cnt
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ_GLB_FREECNT_t cnt;

    if (!free_cnt)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_EQ_GLB_FREECNT;

    cnt.wrd = CA_L2QM_REG_READ(addr);

    *free_cnt = cnt.bf.sum_of_eqcnt;

    return CA_E_OK;
}


ca_status_t aal_l2_qm_eq_cos_buf_cnt_get
(
    ca_device_id_t device_id,
    ca_uint8_t  cos,
    ca_uint32_t *buf_cnt
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ_COS0_BUFCNT_t cnt;

    if (!buf_cnt)
        return CA_E_PARAM;

    if (cos > 7)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_EQ_COS0_BUFCNT + cos * 4;

    cnt.wrd = CA_L2QM_REG_READ(addr);

    *buf_cnt = cnt.bf.sum_of_eqcnt;

    return CA_E_OK;
}

ca_status_t aal_l2_qm_eq_hdr_ptr_get
(
    ca_device_id_t device_id,
    ca_uint8_t  eq_id,
    ca_uint16_t *hdr_ptr
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ_HEAD_t vlaue;

    if (!hdr_ptr)
        return CA_E_PARAM;

    if (eq_id > 1)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_EQ_HEAD;

    vlaue.wrd = CA_L2QM_REG_READ(addr);

    if (eq_id)
        *hdr_ptr = vlaue.bf.eq1;
    else
        *hdr_ptr = vlaue.bf.eq0;

    return CA_E_OK;
}


ca_status_t aal_l2_qm_eq_stack_status_get
(
    ca_device_id_t device_id,
    ca_uint8_t  eq,
    aal_l2_qm_eq_stack_status_t *status
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_EQ0_STACK_STS_t value;

    if (eq > 1)
        return CA_E_PARAM;

    if (!status)
        return CA_E_PARAM;

    addr = eq ? L2TM_L2TM_QM_EQ1_STACK_STS : L2TM_L2TM_QM_EQ0_STACK_STS;

    value.wrd = CA_L2QM_REG_READ(addr);

    status->unfill_done = value.bf.stack_unfill_done;
    status->idle        = value.bf.stack_idle;
    status->empty       = value.bf.stack_empty;
    status->full        = value.bf.stack_full;
    status->cnts        = value.bf.stack_cntr;

    return CA_E_OK;

}

ca_status_t aal_l2_qm_status_get
(
    ca_device_id_t     device_id,
    aal_l2_qm_status_t *status
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_QM_STS_t value;

    if (!status)
        return CA_E_PARAM;

    addr = L2TM_L2TM_QM_STS;

    value.wrd = CA_L2QM_REG_READ(addr);

    status->en_q_fifo_empty = value.bf.enq_ff_empty;
    status->de_q_fifo_empty = value.bf.deq_ff_empty;
    status->plm_non_empty   = value.bf.plm_nempty;

    return CA_E_OK;
}


ca_status_t aal_l2_qm_voq_info_get
(
    ca_device_id_t          device_id,
    ca_uint32_t          voq_id,
    aal_l2_qm_voq_info_t *info
)
{
    L2TM_L2TM_QM_VOQINFO_MEM_DATA0_t value0;
    L2TM_L2TM_QM_VOQINFO_MEM_DATA1_t value1;
    ca_uint32_t index = voq_id;

    if (!info)
        return CA_E_PARAM;

    if (voq_id > CA_AAL_MAX_VOQ_ID)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* read table */
    if (__l2_qm_voq_info_mem_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
            return CA_E_ERROR;

    info->buff_len  = value0.bf.blen;
    info->eqid      = value0.bf.eqid;
    info->eop       = value0.bf.eop;
    info->sop       = value0.bf.sop;
    info->cpu_flag  = value0.bf.cpu;
    info->prvt_flag = value0.bf.prvt;
    info->head_ptr  = value0.bf.head_ptr;
    info->tail_ptr  = ((value1.bf.tail_ptr & 0x3ff) << 5)| (value0.bf.tail_ptr >> 27);
    info->valid     = value1.bf.vallid;
    info->ecc       = value1.bf.ecc;

    return CA_E_OK;
}


ca_status_t aal_l2_qm_freeq_get
(
    ca_device_id_t          device_id,
    ca_uint32_t          buff_id,
    aal_l2_qm_free_q_info_t *info
)
{
    L2TM_L2TM_QM_FREEQ_MEM_DATA0_t value0;
    L2TM_L2TM_QM_FREEQ_MEM_DATA1_t value1;
    ca_uint32_t index = buff_id;

    if (!info)
        return CA_E_PARAM;

    if (buff_id > CA_AAL_L2_BM_MAX_GLOBAL_BUFF_SIZE)
        return CA_E_PARAM;

    memset((void *)&(value0), 0, sizeof(value0));
    memset((void *)&(value1), 0, sizeof(value1));

    /* read table */
    if (__l2_qm_freeq_info_mem_access(
            device_id,
            AAL_TBL_OP_FLAG_RD,
            index,
            (volatile ca_uint32_t *)&(value0.wrd),
            (volatile ca_uint32_t *)&(value1.wrd))
            != CA_E_OK)
            return CA_E_ERROR;

    info->buff_len  = value0.bf.blen;
    info->eqid      = value0.bf.eqid;
    info->eop       = value0.bf.eop;
    info->sop       = value0.bf.sop;
    info->cpu_flag  = value0.bf.cpu;
    info->prvt_flag = value0.bf.prvt;
    info->next_ptr  = value0.bf.nxt_ptr;
    info->ecc       = ((value1.bf.ecc & 0x3) << 5)| (value0.bf.ecc >> 27);

    return CA_E_OK;
}


#ifdef __KERNEL__
EXPORT_SYMBOL(aal_l2_qm_init);
EXPORT_SYMBOL(aal_l2_qm_intr_ena_set);
EXPORT_SYMBOL(aal_l2_qm_intr_ena_get);
EXPORT_SYMBOL(aal_l2_qm_eq_stack_cfg_set);
EXPORT_SYMBOL(aal_l2_qm_eq_stack_cfg_get);
EXPORT_SYMBOL(aal_l2_qm_buff_cfg_set);
EXPORT_SYMBOL(aal_l2_qm_buff_cfg_get);
EXPORT_SYMBOL(aal_l2_qm_eq_cfg_set);
EXPORT_SYMBOL(aal_l2_qm_eq_cfg_get);
EXPORT_SYMBOL(aal_l2_qm_port_prvt_sel_set);
EXPORT_SYMBOL(aal_l2_qm_port_prvt_sel_get);
EXPORT_SYMBOL(aal_l2_qm_port_prvt_profile_set);
EXPORT_SYMBOL(aal_l2_qm_port_prvt_profile_get);
EXPORT_SYMBOL(aal_l2_qm_port_prvt_buff_status_get);
EXPORT_SYMBOL(aal_l2_qm_port_prvt_buff_cnt_get);
EXPORT_SYMBOL(aal_l2_qm_eq_free_cnt_get);
EXPORT_SYMBOL(aal_l2_qm_eq_glb_free_cnt_get);
EXPORT_SYMBOL(aal_l2_qm_eq_cos_buf_cnt_get);
EXPORT_SYMBOL(aal_l2_qm_eq_hdr_ptr_get);
EXPORT_SYMBOL(aal_l2_qm_eq_stack_status_get);
EXPORT_SYMBOL(aal_l2_qm_status_get);
EXPORT_SYMBOL(aal_l2_qm_voq_info_get);
EXPORT_SYMBOL(aal_l2_qm_freeq_get);
#endif

