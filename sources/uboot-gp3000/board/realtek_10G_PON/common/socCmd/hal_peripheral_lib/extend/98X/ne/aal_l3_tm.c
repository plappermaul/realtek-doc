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
#include "aal_l2_qm.h"
#include "aal_l2_tm.h"
#include "aal_l3_tm.h"

#define CA_L3TM_REG_READ(addr)        CA_REG_READ(AAL_NE_REG_ADDR(addr))
#define CA_L3TM_REG_WRITE(addr, data) CA_REG_WRITE(data, AAL_NE_REG_ADDR(addr))



#ifdef __L3_ES
#endif

ca_status_t aal_l3_tm_es_ena_set
(
    ca_device_id_t  device_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = QM_QM_ES_CTRL;
    QM_QM_ES_CTRL_t ctrl;

    ctrl.wrd = CA_NE_REG_READ(addr);

    ctrl.bf.tx_en = ena;

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_ena_get
(
    ca_device_id_t  device_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = QM_QM_ES_CTRL;
    QM_QM_ES_CTRL_t ctrl;

    if (!ena)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    *ena = ctrl.bf.tx_en ? TRUE : FALSE;

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_cpu_port_ena_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = QM_QM_ES_CTRL;
    QM_QM_ES_CTRL_t ctrl;

    if (port_id > 7)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    if (ena)
        ctrl.wrd |= (1 << port_id);
    else
        ctrl.wrd &= ~(1 << port_id);

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_cpu_port_ena_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = QM_QM_ES_CTRL;
    QM_QM_ES_CTRL_t ctrl;

    if (!ena)
        return CA_E_PARAM;

    if (port_id > 7)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    *ena = ((ctrl.wrd >> port_id) & 0x1) ? TRUE : FALSE;

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_ni_port_ena_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = QM_QM_ES_CTRL;
    QM_QM_ES_CTRL_t ctrl;

    if (port_id > 7)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    if (ena)
        ctrl.wrd |= (1 << (port_id + 8));
    else
        ctrl.wrd &= ~(1 << (port_id + 8));

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_ni_port_ena_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = QM_QM_ES_CTRL;
    QM_QM_ES_CTRL_t ctrl;

    if (!ena)
        return CA_E_PARAM;

    if (port_id > 7)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    *ena = ((ctrl.wrd >> (port_id + 8)) & 0x1) ? TRUE : FALSE;

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_voq_ena_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = QM_QM_SCH_CFG0;
    QM_QM_SCH_CFG0_t ctrl;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;

    /* instances:  48 instances, base + [0..47] * 4, CPU(0-7), NI(8-15), CPUEPP256(16-47) */
    ctrl.wrd = CA_NE_REG_READ(addr + 4 * port_id);

    if (ena)
        ctrl.wrd |= (1 << voq_id);
    else
        ctrl.wrd &= ~(1 << voq_id);

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_voq_ena_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = QM_QM_SCH_CFG0;
    QM_QM_SCH_CFG0_t ctrl;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (!ena)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;
    /* instances:  48 instances, base + [0..47] * 4, CPU(0-7), NI(8-15), CPUEPP256(16-47) */

    ctrl.wrd = CA_NE_REG_READ(addr + 4 * port_id);

    *ena = ((ctrl.wrd >> voq_id) & 0x1) ? TRUE : FALSE;

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_voq_flush_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id
)
{
    ca_uint32_t addr = QM_QM_SCH_CFG0;
    QM_QM_SCH_CFG0_t ctrl;
    ca_boolean_t empty = FALSE;
    ca_uint32_t i = 0;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;

    /* instances:  48 instances, base + [0..47] * 4, CPU(0-7), NI(8-15), CPUEPP256(16-47) */
    addr += port_id * 4;

    ctrl.wrd = CA_NE_REG_READ(addr);

    ctrl.bf.flush_voqid = voq_id;
    ctrl.bf.flush_en     = 1;

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    do
    {
        (void)aal_l3_tm_es_voq_status_get(device_id, port_id, voq_id, &empty);
        if (i++ > 1000)     /* 1000 times timeout */
            empty = TRUE;
    } while (!empty);


    ctrl.wrd = CA_NE_REG_READ(addr);

    ctrl.bf.flush_en = 0;   /* disable flush_en filed */

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_voq_flush_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *voq_id
)
{
    ca_uint32_t addr = QM_QM_SCH_CFG0;
    QM_QM_SCH_CFG0_t ctrl;

    if (!voq_id)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;

    /* instances:  48 instances, base + [0..47] * 4, CPU(0-7), NI(8-15), CPUEPP256(16-47) */
    ctrl.wrd = CA_NE_REG_READ(addr + port_id * 4);

    *voq_id = ((ctrl.wrd >> 8) & 0x7);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_voq_status_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t *empty
)
{
    ca_uint32_t addr = QM_QM_VOQ_STATUS0;
    QM_QM_VOQ_STATUS0_t value;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;

    if (!empty)
        return CA_E_PARAM;

    /* instances:  48 instances, base + [0..47] * 4, CPU(0-7), NI(8-15), CPUEPP256(16-47) */
    value.wrd = CA_NE_REG_READ(addr + (port_id / 4) * 4);

    *empty = ((value.wrd >> (((port_id % 4) * 8) + voq_id)) & 0x1) ? FALSE : TRUE;

    return CA_E_OK;
}


ca_status_t aal_l3_tm_voq_drop_ena_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = QM_QM_RMU0_CTRL;
    QM_QM_RMU0_CTRL_t ctrl;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    if (ena)
    {
        ctrl.bf.voq_drop_en    = 1;
        ctrl.bf.voq_drop_voqid = port_id * 8 + voq_id;
    }
    else
    {
        ctrl.bf.voq_drop_en    = 0;
    }

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_voq_drop_ena_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = QM_QM_RMU0_CTRL;
    QM_QM_RMU0_CTRL_t ctrl;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (port_id > CA_AAL_L3_QM_MAX_PORT_ID)
        return CA_E_PARAM;

    if (!ena)
        return CA_E_PARAM;

    ctrl.wrd = CA_NE_REG_READ(addr);

    *ena = ctrl.bf.voq_drop_en ? TRUE : FALSE;

    return CA_E_OK;
}


ca_status_t aal_l3_tm_es_weight_ratio_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint8_t   cos,
    ca_uint32_t  weight
)
{
    /*
    ** 16 instances: base + ([0..15] * 8)
    ** VOQ7 is the highest priority and VOQ0 is the lowest priority voq
    ** 0 ~ 7 CPU, 8 ~ 15 NI
    */
    ca_uint32_t addr = 0;
    QM_QM_VOQ3_0_WEIGHT_RATIO_CFG0_t ctrl;

    if (port_id > 15)
        return CA_E_PARAM;

    if (cos > 7)
        return CA_E_PARAM;

    addr = QM_QM_VOQ3_0_WEIGHT_RATIO_CFG0 + port_id * 8 + ((cos >= 4) ? 4 : 0);

    ctrl.wrd = CA_NE_REG_READ(addr);

    // ctrl.wrd &= ~(0xff << (voq_id % 4));
    // ctrl.wrd |= weight << (voq_id  % 4);

    switch (cos % 4)
    {
    case 0 :
        ctrl.bf.voq0 = weight;
        break;
    case 1 :
        ctrl.bf.voq1 = weight;
        break;
    case 2 :
        ctrl.bf.voq2 = weight;
        break;
    case 3 :
        ctrl.bf.voq3 = weight;
            break;
    default:
        break;
    }

    CA_NE_REG_WRITE(ctrl.wrd, addr);

    return CA_E_OK;
}

ca_status_t aal_l3_tm_es_weight_ratio_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint8_t   cos,
    ca_uint32_t  *weight
)
{
    /*
    ** 16 instances: base + ([0..15] * 8)
    ** VOQ7 is the highest priority and VOQ0 is the lowest priority voq
    ** 0 ~ 7 CPU, 8 ~ 15 NI
    */

    ca_uint32_t addr = 0;
    QM_QM_VOQ3_0_WEIGHT_RATIO_CFG0_t ctrl;

    if (port_id > 15)
        return CA_E_PARAM;

    if (!weight)
        return CA_E_PARAM;

    if (cos > 7)
        return CA_E_PARAM;

    addr = QM_QM_VOQ3_0_WEIGHT_RATIO_CFG0 + port_id * 8 + ((cos >= 4) ? 4 : 0);

    ctrl.wrd = CA_NE_REG_READ(addr);

    // ctrl.wrd &= ~(0xff << (voq_id % 4));
    // ctrl.wrd |= weight << (voq_id  % 4);

    switch (cos % 4)
    {
    case 0 :
        *weight = ctrl.bf.voq0;
        break;
    case 1 :
        *weight = ctrl.bf.voq1;
        break;
    case 2 :
        *weight = ctrl.bf.voq2;
        break;
    case 3 :
        *weight = ctrl.bf.voq3;
        break;
    default:
        break;
    }

    return CA_E_OK;
}


/*
** L3 TM module initialization function
*/
ca_status_t aal_l3_tm_init(ca_device_id_t device_id)
{

    return CA_E_OK;
}


#ifdef __KERNEL__
EXPORT_SYMBOL(aal_l3_tm_es_ena_set);
EXPORT_SYMBOL(aal_l3_tm_es_ena_get);
EXPORT_SYMBOL(aal_l3_tm_es_cpu_port_ena_set);
EXPORT_SYMBOL(aal_l3_tm_es_cpu_port_ena_get);
EXPORT_SYMBOL(aal_l3_tm_es_ni_port_ena_set);
EXPORT_SYMBOL(aal_l3_tm_es_ni_port_ena_get);
EXPORT_SYMBOL(aal_l3_tm_es_voq_ena_set);
EXPORT_SYMBOL(aal_l3_tm_es_voq_ena_get);
EXPORT_SYMBOL(aal_l3_tm_es_voq_flush_set);
EXPORT_SYMBOL(aal_l3_tm_es_voq_flush_get);
EXPORT_SYMBOL(aal_l3_tm_es_voq_status_get);
EXPORT_SYMBOL(aal_l3_tm_voq_drop_ena_set);
EXPORT_SYMBOL(aal_l3_tm_voq_drop_ena_get);
EXPORT_SYMBOL(aal_l3_tm_es_weight_ratio_set);
EXPORT_SYMBOL(aal_l3_tm_es_weight_ratio_get);
EXPORT_SYMBOL(aal_l3_tm_init);
#endif

