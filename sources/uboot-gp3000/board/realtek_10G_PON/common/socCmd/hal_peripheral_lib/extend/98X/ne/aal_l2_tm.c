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
#include "aal_l2_te.h"
#include "aal_l3_tm.h"
#include "aal_l3_te.h"

#if 0//yocto
#define CA_L2TM_REG_READ(addr)        CA_REG_READ(AAL_NE_REG_ADDR(addr))
#define CA_L2TM_REG_WRITE(addr, data) CA_REG_WRITE(data, AAL_NE_REG_ADDR(addr))
#else//sd1 uboot for 98f
#define CA_L2TM_REG_READ(addr)        CA_REG_READ((volatile uintptr_t)(addr))
#define CA_L2TM_REG_WRITE(addr, data) CA_REG_WRITE(data, (volatile uintptr_t)(addr))
#endif


#define CA_AAL_ES_PORT_CHECK(x)                             \
    do                                                      \
    {                                                       \
        if (((x) == 14) || ((x) > CA_AAL_ES_MAX_PORT_ID))   \
            return CA_E_PARAM;                              \
    }while(0)


#ifdef __L2_TM_ES
#endif

ca_status_t aal_l2_tm_read_clr_set
(
    ca_device_id_t  device_id,
    ca_boolean_t read_clr
)
{
    L2TM_L2TM_GLB_CTRL_t value;

    value.wrd = CA_L2TM_REG_READ(L2TM_L2TM_GLB_CTRL);

    value.bf.cntrcfg_clear_on_read = read_clr;

    CA_L2TM_REG_WRITE(L2TM_L2TM_GLB_CTRL, value.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_read_clr_get
(
    ca_device_id_t  device_id,
    ca_boolean_t *read_clr
)
{
    L2TM_L2TM_GLB_CTRL_t value;

    if (!read_clr)
        return CA_E_PARAM;

    value.wrd = CA_L2TM_REG_READ(L2TM_L2TM_GLB_CTRL);

    *read_clr = value.bf.cntrcfg_clear_on_read ? TRUE :FALSE ;

    return CA_E_OK;
}


ca_status_t aal_l2_tm_es_ena_set
(
    ca_device_id_t  device_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_CTRL;
    L2TM_L2TM_ES_CTRL_t ctrl;

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    ctrl.bf.tx_en = ena;

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_es_ena_get
(
    ca_device_id_t  device_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_CTRL;
    L2TM_L2TM_ES_CTRL_t ctrl;

    if (!ena)
        return CA_E_PARAM;

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    *ena = ctrl.bf.tx_en ? TRUE : FALSE;

    return CA_E_OK;
}



ca_status_t aal_l2_tm_es_port_ena_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_CTRL;
    L2TM_L2TM_ES_CTRL_t ctrl;

    CA_AAL_ES_PORT_CHECK(port_id);

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    if (ena)
        ctrl.wrd |= (1 << port_id);
    else
        ctrl.wrd &= ~(1 << port_id);

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_es_port_ena_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_CTRL;
    L2TM_L2TM_ES_CTRL_t ctrl;

    if (!ena)
        return CA_E_PARAM;

    CA_AAL_ES_PORT_CHECK(port_id);

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    *ena = ((ctrl.wrd >> port_id) & 0x1) ? TRUE : FALSE;

    return CA_E_OK;
}


ca_status_t aal_l2_tm_es_voq_status_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t *empty
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_VOQ_STS0;
    L2TM_L2TM_ES_VOQ_STS0_t value;

    CA_AAL_ES_PORT_CHECK(port_id);

    if (voq_id > 7)
        return CA_E_PARAM;

    if (!empty)
        return CA_E_PARAM;

    value.wrd = CA_L2TM_REG_READ(addr + (port_id / 4) * 4);

    *empty = ((value.wrd >> (((port_id % 4) * 8) + voq_id)) & 0x1) ? FALSE : TRUE;

    return CA_E_OK;
}


ca_status_t aal_l2_tm_es_voq_flush_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_SCH_CFG0;
    L2TM_L2TM_ES_SCH_CFG0_t ctrl;
    ca_boolean_t empty = FALSE;
    ca_uint32_t i = 0;
    ca_port_id_t es_port = port_id;

    CA_AAL_ES_PORT_CHECK(port_id);

    if (voq_id > 7)
        return CA_E_PARAM;

    /*
    Instances:  15 instances
    Names: %d=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,15]
    Addresses: base + [0..14] * 12
    */
    if (es_port == CA_AAL_ES_MAX_PORT_ID)
        es_port -= 1;

    addr += 12 * es_port;

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    ctrl.wrd &= ~(0x7 << 8);    /* clear flush_voqid fields */
    ctrl.wrd |= (voq_id << 8);  /* set flush_voqid fields   */
    ctrl.wrd |= (1 << 15);      /* set flush_en fields      */

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    do
    {
        (void)aal_l2_tm_es_voq_status_get(device_id, port_id, voq_id, &empty);
        if (i++ > 1000)     /* 1000 times timeout */
            empty = TRUE;
    } while (!empty);


    ctrl.wrd = CA_L2TM_REG_READ(addr);
    ctrl.wrd &= ~(1 << 15);     /* disable flush_en filed */

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_es_voq_flush_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *voq_id
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_SCH_CFG0;
    L2TM_L2TM_ES_SCH_CFG0_t ctrl;

    if (!voq_id)
        return CA_E_PARAM;

    CA_AAL_ES_PORT_CHECK(port_id);

    /*
    Instances:  15 instances
    Names: %d=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,15]
    Addresses: base + [0..14] * 12
    */

    if (port_id == CA_AAL_ES_MAX_PORT_ID)
        port_id -= 1;

    ctrl.wrd = CA_L2TM_REG_READ(addr + 12 * port_id);

    *voq_id = ((ctrl.wrd >> 8) & 0x7);

    return CA_E_OK;
}


ca_status_t aal_l2_tm_es_voq_ena_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t ena
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_SCH_CFG0;
    L2TM_L2TM_ES_SCH_CFG0_t ctrl;

    CA_AAL_ES_PORT_CHECK(port_id);

    if (voq_id > 7)
        return CA_E_PARAM;

    /*
    Instances:  15 instances
    Names: %d=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,15]
    Addresses: ( base=[0x2204] ) + ( 12 * ( %d=[0..14] ) )
    */
    if (port_id == CA_AAL_ES_MAX_PORT_ID)
        port_id -= 1;

    ctrl.wrd = CA_L2TM_REG_READ(addr + 12 * port_id);

    if (ena)
        ctrl.wrd |= (1 << voq_id);
    else
        ctrl.wrd &= ~(1 << voq_id);

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_es_voq_ena_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  voq_id,
    ca_boolean_t *ena
)
{
    ca_uint32_t addr = L2TM_L2TM_ES_SCH_CFG0;
    L2TM_L2TM_ES_SCH_CFG0_t ctrl;

    if (voq_id > 7)
        return CA_E_PARAM;

    if (!ena)
        return CA_E_PARAM;

    CA_AAL_ES_PORT_CHECK(port_id);

    /*
    Instances:  15 instances
    Names: %d=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,15]
    Addresses: ( base=[0x2204] ) + ( 12 * ( %d=[0..14] ) )
    */
    if (port_id == CA_AAL_ES_MAX_PORT_ID)
        port_id -= 1;

    ctrl.wrd = CA_L2TM_REG_READ(addr + 12 * port_id);

    *ena = ((ctrl.wrd >> voq_id) & 0x1) ? TRUE : FALSE;

    return CA_E_OK;
}


ca_status_t aal_l2_tm_es_weight_ratio_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint8_t   cos,
    ca_uint32_t  weight
)
{
    ca_uint32_t addr = 0;
    L2TM_L2TM_ES_VOQ3_0_WEIGHT_RATIO_CFG0_t ctrl;

    CA_AAL_ES_PORT_CHECK(port_id);

    if (cos > 7)
        return CA_E_PARAM;

    if (weight > 0xff)
        return CA_E_PARAM;

    /*
    Instances:  15 instances
    Names: %d=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,15]
    Addresses: ( base=[0x2208] ) + ( 12 * ( %d=[0..14] ) )
    */
    /* NI 0 ~ 6, L3QM 7, L3LAN 8, L3WAN 9, MC 10 */
    if (port_id == CA_AAL_ES_MAX_PORT_ID)
        port_id -= 1;

    addr = L2TM_L2TM_ES_VOQ3_0_WEIGHT_RATIO_CFG0 + port_id * 12 + ((cos >= 4) ? 4 : 0);

    ctrl.wrd = CA_L2TM_REG_READ(addr);

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

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_es_weight_ratio_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint8_t   cos,
    ca_uint32_t  *weight
)
{

    /*  ( base=[0x188] ) + ( 12 * ( %d=[0..10] ) ) */
    ca_uint32_t addr = 0;
    L2TM_L2TM_ES_VOQ3_0_WEIGHT_RATIO_CFG0_t ctrl;

    CA_AAL_ES_PORT_CHECK(port_id);

    if (!weight)
        return CA_E_PARAM;

    if (cos > 7)
        return CA_E_PARAM;

    /*
    Instances:  15 instances
    Names: %d=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,15]
    Addresses: ( base=[0x2208] ) + ( 12 * ( %d=[0..14] ) )
    */
    /* NI 0 ~ 6, L3QM 7, L3LAN 8, L3WAN 9, MC 10 */
    if (port_id == CA_AAL_ES_MAX_PORT_ID)
        port_id -= 1;

    addr = L2TM_L2TM_ES_VOQ3_0_WEIGHT_RATIO_CFG0 + port_id * 12 + ((cos >= 4) ? 4 : 0);

    ctrl.wrd = CA_L2TM_REG_READ(addr);

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


ca_status_t aal_l2_tm_es_port_weight_ratio_set
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  weight
)
{
    L2TM_L2TM_ES_PORT3_0_WEIGHT_RATIO_CFG_t ctrl;
    ca_uint32_t addr = 0;

    /* NI 0 ~ 6, L3QM 7, L3LAN 8, L3WAN 9, MC 10 */
    CA_AAL_ES_PORT_CHECK(port_id);

    addr = L2TM_L2TM_ES_PORT3_0_WEIGHT_RATIO_CFG + (port_id / 4) * 4;

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    // ctrl.wrd &= ~(0xff << (port_id % 4));
    // ctrl.wrd |= weight << (port_id  % 4);

    switch (port_id % 4)
    {
    case 0 :
        ctrl.bf.port0 = weight;
        break;
    case 1 :
        ctrl.bf.port1 = weight;
        break;
    case 2 :
        ctrl.bf.port2 = weight;
        break;
    case 3 :
        ctrl.bf.port3 = weight;
            break;
    default:
        break;
    }

    CA_L2TM_REG_WRITE(addr, ctrl.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_tm_es_port_weight_ratio_get
(
    ca_device_id_t  device_id,
    ca_port_id_t port_id,
    ca_uint32_t  *weight
)
{
    L2TM_L2TM_ES_PORT3_0_WEIGHT_RATIO_CFG_t ctrl;
    ca_uint32_t addr = 0;

    /* NI 0 ~ 6, L3QM 7, L3LAN 8, L3WAN 9, MC 10 */
    CA_AAL_ES_PORT_CHECK(port_id);

    if (!weight)
        return CA_E_PARAM;

    addr = L2TM_L2TM_ES_PORT3_0_WEIGHT_RATIO_CFG + (port_id / 4) * 4;

    ctrl.wrd = CA_L2TM_REG_READ(addr);

    // ctrl.wrd &= ~(0xff << (port_id % 4));
    // ctrl.wrd |= weight << (port_id  % 4);

    switch (port_id % 4)
    {
    case 0 :
        *weight = ctrl.bf.port0;
        break;
    case 1 :
        *weight = ctrl.bf.port1;
        break;
    case 2 :
        *weight = ctrl.bf.port2;
        break;
    case 3 :
        *weight = ctrl.bf.port3;
        break;
    default:
        break;
    }

    return CA_E_OK;
}

#ifdef __L2_TM_BM
#endif
static inline ca_status_t __l2_tm_bm_pkt_mem_access
(
    ca_device_id_t          device_id,
    ca_uint32_t          addr,
    volatile ca_uint32_t *data
)
{
    L2TM_L2TM_BM_PKT_MEM_ACCESS_t access;
    ca_uint32_t i = 0;

    access.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_ACCESS);

    while (access.bf.access && (i < AAL_TBL_OP_TIMEOUT))
    {
        access.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    memset((void *)&access, 0, sizeof(access));

    access.bf.address  = addr;
    access.bf.rbw      = 0;     /* read */
    access.bf.access   = 1;

    CA_L2TM_REG_WRITE(L2TM_L2TM_BM_PKT_MEM_ACCESS, access.wrd);

    /* waiting untill read-operation is completed */

    i = 0;

    access.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_ACCESS);
    while (access.bf.access)
    {
        access.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_ACCESS);
        if (i++ >= AAL_TBL_OP_TIMEOUT)
        {
            return CA_E_TIMEOUT;
        }
    }

    for (i=0; i<8; i++)
        *data++ = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_DATA7 + 4 * i);


    return CA_E_OK;

}


ca_status_t aal_l2_tm_bm_buff_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint16_t  buff_id,
    CA_OUT aal_l2_tm_bm_buff_pkt_t *pkt_buff
)
{
    ca_uint32_t pkt_data[8];
    ca_uint32_t index = buff_id;

    if (index > 0xfff)  /* 4k 64 bytes buffer */
        return CA_E_PARAM;

    memset((void *)&(pkt_data), 0, sizeof(pkt_data));

    /* read packet mem */
    if (__l2_tm_bm_pkt_mem_access(device_id, index, (volatile ca_uint32_t *)&pkt_data[0]) != CA_E_OK)
        return CA_E_ERROR;

    pkt_buff->cos              =  (pkt_data[1] >> 0)  & 0x7;
    pkt_buff->ldpid            =  (pkt_data[1] >> 3)  & 0x3f;
    pkt_buff->lspid            =  (pkt_data[1] >> 9)  & 0x3f;
    pkt_buff->pkt_size         =  (pkt_data[1] >> 15) & 0x3fff;
    pkt_buff->fe_bypass        =  (pkt_data[1] >> 29) & 0x1;
    pkt_buff->hdr_type         =  (pkt_data[1] >> 30) & 0x3;

    pkt_buff->mc_group_id      =  (pkt_data[0] >> 0)  & 0xff;
    pkt_buff->drop_code        =  (pkt_data[0] >> 8)  & 0x7;
    pkt_buff->rx_pkt_type      =  (pkt_data[0] >> 11) & 0x3;
    pkt_buff->no_drop          =  (pkt_data[0] >> 13) & 0x1;
    pkt_buff->mirror           =  (pkt_data[0] >> 14) & 0x1;
    pkt_buff->mark             =  (pkt_data[0] >> 15) & 0x1;
    pkt_buff->policer_en       =  (pkt_data[0] >> 16) & 0x3;
    pkt_buff->policing_id      =  (pkt_data[0] >> 18) & 0x1ff;
    pkt_buff->policing_group_id=  (pkt_data[0] >> 27) & 0x7;
    pkt_buff->deep_q           =  (pkt_data[0] >> 30) & 0x1;
    pkt_buff->cpu_flag         =  (pkt_data[0] >> 31) & 0x1;

    pkt_buff->mac_da[0]        =  (pkt_data[4] >> 24) & 0xff;
    pkt_buff->mac_da[1]        =  (pkt_data[4] >> 16) & 0xff;
    pkt_buff->mac_da[2]        =  (pkt_data[4] >> 8)  & 0xff;
    pkt_buff->mac_da[3]        =  (pkt_data[4] >> 0)  & 0xff;
    pkt_buff->mac_da[4]        =  (pkt_data[5] >> 24) & 0xff;
    pkt_buff->mac_da[5]        =  (pkt_data[5] >> 16) & 0xff;
    pkt_buff->mac_sa[0]        =  (pkt_data[5] >> 8)  & 0xff;
    pkt_buff->mac_sa[1]        =  (pkt_data[5] >> 0)  & 0xff;
    pkt_buff->mac_sa[2]        =  (pkt_data[6] >> 24) & 0xff;
    pkt_buff->mac_sa[3]        =  (pkt_data[6] >> 16) & 0xff;
    pkt_buff->mac_sa[4]        =  (pkt_data[6] >> 8)  & 0xff;
    pkt_buff->mac_sa[5]        =  (pkt_data[6] >> 0)  & 0xff;
    pkt_buff->eth_type         =  (pkt_data[7] >> 16) & 0xffff;
    pkt_buff->vlan_tag         =  (pkt_data[7] >> 0)  & 0xffff;


    return CA_E_OK;
}

ca_status_t aal_l2_tm_bm_buff_dump_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint16_t  buff_id
)
{
    #define __BM_PRINT_BUFF_SIZE 256
    ca_uint8_t  str_buff[__BM_PRINT_BUFF_SIZE], str_tmp[16];
    ca_uint32_t pkt_data[8];
    ca_uint32_t index = buff_id;
    ca_uint32_t i = 0;

    if (index > 0xfff)  /* 4k 64 bytes buffer */
        return CA_E_PARAM;

    memset((void *)&(pkt_data[0]), 0, sizeof(pkt_data)); /* each access to buffer shall read 32 bytes */
    memset((void *)&(str_buff[0]), 0, sizeof(str_buff));

    /* read packet mem */
    if (__l2_tm_bm_pkt_mem_access(device_id, index, (volatile ca_uint32_t *)&pkt_data[0]) != CA_E_OK)
        return CA_E_ERROR;

    for (i=0; i<32; i++)
    {
        sprintf(str_tmp, "%02x ", (pkt_data[i / 4] >> ((i % 4) * 8)) & 0xff);

        strcat(str_buff, str_tmp);

        if ((i & 0xf) == 0xf)
            strcat(str_buff, "\n");
    }

    printf("%s", str_buff);

    return CA_E_OK;
}



ca_status_t aal_l2_tm_bm_stats_get
(
    CA_IN  ca_device_id_t device_id,
    CA_OUT aal_l2_tm_bm_mib_t *mib
)
{
    if (!mib)
        return CA_E_PARAM;

    mib->rx_cnt           = CA_L2TM_REG_READ(L2TM_L2TM_BM_RX_PCNT);
    mib->tx_cnt           = CA_L2TM_REG_READ(L2TM_L2TM_BM_TX_PCNT);
    mib->sb_drop_cnt      = CA_L2TM_REG_READ(L2TM_L2TM_BM_SB_DPCNT);
    mib->hdr_drop_cnt     = CA_L2TM_REG_READ(L2TM_L2TM_BM_HDR_DPCNT);
    mib->te_drop_cnt      = CA_L2TM_REG_READ(L2TM_L2TM_BM_TE_DPCNT);
    mib->err_drop_cnt     = CA_L2TM_REG_READ(L2TM_L2TM_BM_ERR_DPCNT);
    mib->no_buff_drop_cnt = CA_L2TM_REG_READ(L2TM_L2TM_BM_NOBUF_DPCNT);

    return CA_E_OK;
}

ca_status_t aal_l2_tm_bm_status_get
(
    CA_IN  ca_device_id_t     device_id,
    CA_OUT aal_l2_tm_bm_status_t *status
)
{
    L2TM_L2TM_BM_STS_t value;

    if (!status)
        return CA_E_PARAM;

    value.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_STS);

    status->fe_tm_valid           = value.bf.fe_tm_vld;
    status->tm_fe_ready           = value.bf.tm_fe_rdy;
    status->ni_tm_valid           = value.bf.ni_tm_vld;
    status->tm_ni_ready           = value.bf.tm_ni_rdy;
    status->tm_ni_valid           = value.bf.tm_ni_vld;
    status->deep_q_if_fifo_empty  = value.bf.dq_ff_empty;
    status->deep_q_pkt_fifo_empty = value.bf.dq_pack_ff_empty;
    status->fe_if_fifo_empty      = value.bf.fe_ff_empty;
    status->pkt_fifo_empty        = value.bf.pack_ff_empty;
    status->wr_data_fifo_empty    = value.bf.wdat_ff_empty;
    status->en_q_buff_fifo_empty  = value.bf.wcmd_ff_empty;
    status->rd_data_fifo_empty    = value.bf.rdat_ff_empty;
    status->de_q_buff_fifo_empty  = value.bf.rcmd_ff_empty;
    status->buff_recycling_fifo_empty = value.bf.rcyc_ff_empty;

    return CA_E_OK;
}



ca_status_t aal_l2_tm_bm_cfg_set
(
    CA_IN ca_device_id_t      device_id,
    CA_IN aal_l2_tm_bm_cfg_msk_t msk,
    CA_IN aal_l2_tm_bm_cfg_t     *cfg
)
{
    L2TM_L2TM_BM_CTRL_t value;

    if (!cfg)
        return CA_E_PARAM;

    if (msk.u32 == 0)
        return CA_E_OK;


    value.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_CTRL);

    if (msk.s.rx_drop_cos)
    {
        if (cfg->rx_drop_cos > 0x7)
            return CA_E_PARAM;

        value.bf.rx_drop_cos = cfg->rx_drop_cos;
    }

    if (msk.s.rx_drop_port)
    {
        if (cfg->rx_drop_port > 0xf)
            return CA_E_PARAM;

        value.bf.rx_drop_pid = cfg->rx_drop_port;
    }

    if (msk.s.rx_drop_ena)
    {
        value.bf.rx_drop_en = cfg->rx_drop_ena;
    }

    if (msk.s.sb_drop_ena)
    {
        value.bf.sb_drop_dis = cfg->sb_drop_ena ? 0 : 1;
    }

    if (msk.s.min_pkt_len)
    {
        if (cfg->min_pkt_len > 0x7f)
            return CA_E_PARAM;

        value.bf.min_pktlen = cfg->min_pkt_len;
    }

    if (msk.s.rx_ena)
    {
        value.bf.rx_en = cfg->rx_ena ? 1 : 0;
    }

    CA_L2TM_REG_WRITE(L2TM_L2TM_BM_CTRL, value.wrd);

    return CA_E_OK;
}


ca_status_t aal_l2_tm_bm_cfg_get
(
    CA_IN ca_device_id_t   device_id,
    CA_OUT aal_l2_tm_bm_cfg_t *cfg
)
{
    L2TM_L2TM_BM_CTRL_t value;

    if (!cfg)
        return CA_E_PARAM;

    value.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_CTRL);

    cfg->rx_drop_cos  = value.bf.rx_drop_cos;
    cfg->rx_drop_port = value.bf.rx_drop_pid;
    cfg->rx_drop_ena  = value.bf.rx_drop_en;
    cfg->sb_drop_ena  = (value.bf.sb_drop_dis == 1) ? FALSE : TRUE;
    cfg->min_pkt_len  = value.bf.min_pktlen;
    cfg->rx_ena       = value.bf.rx_en;

    return CA_E_OK;
}

/*
ca_status_t aal_l2_tm_bm_pkt_buff_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint16_t  buff_id,
    CA_OUT aal_l2_tm_bm_buff_pkt_t *buf_pkt
)
{
    L2TM_L2TM_BM_PKT_MEM_ACCESS_t bm_pkt_access;
    ca_uint32_t                   bm_pkt_data[8];
    ca_uint32_t                   i = 0;

    if (NULL == buf_pkt)
    {
        return CA_E_PARAM;
    }

    bm_pkt_access.bf.address  = buff_id;
    bm_pkt_access.bf.rbw      = 0;
    bm_pkt_access.bf.access   = 1;

    CA_L2TM_REG_WRITE(L2TM_L2TM_BM_PKT_MEM_ACCESS, bm_pkt_access.wrd);

    for (i = 0; i<AAL_TBL_OP_TIMEOUT; i++)
    {
        bm_pkt_access.wrd = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_ACCESS);
        if (bm_pkt_access.bf.access == 0)
        {
            break;
        }
    }

    if (i == AAL_TBL_OP_TIMEOUT)
    {
        return CA_E_TIMEOUT;
    }

    for (i=0; i<8; i++)
    {
        bm_pkt_data[i] = CA_L2TM_REG_READ(L2TM_L2TM_BM_PKT_MEM_DATA7 + 4 * i);
    }


    buf_pkt->cpu_flag         =  (bm_pkt_data[0] >> 31) & 1;
    buf_pkt->deep_q           =  (bm_pkt_data[0] >> 30) & 1;
    buf_pkt->policy_ena       =  (bm_pkt_data[0] >> 16) & 3;
    buf_pkt->policy_group_id  =  (bm_pkt_data[0] >> 27) & 7;
    buf_pkt->policy_id        =  (bm_pkt_data[0] >> 18) & 0x1ff;
    buf_pkt->marked           =  (bm_pkt_data[0] >> 15) & 1;
    buf_pkt->mirror           =  (bm_pkt_data[0] >> 14) & 1;
    buf_pkt->no_drop          =  (bm_pkt_data[0] >> 13) & 1;
    buf_pkt->rx_packet_type   =  (bm_pkt_data[0] >> 11) & 3;
    buf_pkt->drop_code        =  (bm_pkt_data[0] >> 8)  & 7;
    buf_pkt->mc_group_id      =  (bm_pkt_data[0] >> 0)  & 0xff;
    buf_pkt->header_type      =  (bm_pkt_data[1] >> 30) & 3;
    buf_pkt->fe_bypass        =  (bm_pkt_data[1] >> 29) & 1;
    buf_pkt->packet_size      =  (bm_pkt_data[1] >> 15) & 0x3fff;
    buf_pkt->cos              =  (bm_pkt_data[1] >> 0)  & 0x7;
    buf_pkt->logic_spid       =  (bm_pkt_data[1] >> 9)  & 0x3f;
    buf_pkt->logic_dpid       =  (bm_pkt_data[1] >> 3)  & 0x3f;

    buf_pkt->mac_da[0]        =  (bm_pkt_data[4] >> 24) & 0xff;
    buf_pkt->mac_da[1]        =  (bm_pkt_data[4] >> 16) & 0xff;
    buf_pkt->mac_da[2]        =  (bm_pkt_data[4] >> 8)  & 0xff;
    buf_pkt->mac_da[3]        =  (bm_pkt_data[4] >> 0)  & 0xff;
    buf_pkt->mac_da[4]        =  (bm_pkt_data[5] >> 24) & 0xff;
    buf_pkt->mac_da[5]        =  (bm_pkt_data[5] >> 16) & 0xff;
    buf_pkt->mac_sa[0]        =  (bm_pkt_data[5] >> 8)  & 0xff;
    buf_pkt->mac_sa[1]        =  (bm_pkt_data[5] >> 0)  & 0xff;
    buf_pkt->mac_sa[2]        =  (bm_pkt_data[6] >> 24) & 0xff;
    buf_pkt->mac_sa[3]        =  (bm_pkt_data[6] >> 16) & 0xff;
    buf_pkt->mac_sa[4]        =  (bm_pkt_data[6] >> 8)  & 0xff;
    buf_pkt->mac_sa[5]        =  (bm_pkt_data[6] >> 0)  & 0xff;
    buf_pkt->tpid_eth_type    =  (bm_pkt_data[7] >> 16) & 0xffff;
    buf_pkt->dot_1p           =  (bm_pkt_data[7] >> 13) & 0x7;
    buf_pkt->cfi              =  (bm_pkt_data[7] >> 12) & 0x1;
    buf_pkt->vlan_id          =  (bm_pkt_data[7])       & 0xfff;

    return CA_E_OK;
}

*/



/*
** L2 TM module initialization function
*/
ca_status_t aal_l2_tm_init(ca_device_id_t device_id)
{
    ca_int8 sz_info[256], sz_tmp[16];
    ca_uint32_t i = 0, voq_id = 0;

    (void)aal_l2_qm_init(device_id);

    /* TM counter will be cleared on reading */
    (void)aal_l2_tm_read_clr_set(device_id, CA_TRUE);

#if CONFIG_98F_UBOOT_NE_DBG
    /* ES initialization */
    sprintf(sz_info, "%s", "L2_TM : enable ES for port ");
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    /* enable global ES control */
    (void)aal_l2_tm_es_ena_set(device_id, TRUE);

    /* enable port level ES control */
    for (i=CA_AAL_ES_PORT_NI0; i<=CA_AAL_ES_MAX_PORT_ID; i++)
    {
        if (i != 14)
        {
#if CONFIG_98F_UBOOT_NE_DBG
            sprintf(sz_tmp, (i == CA_AAL_ES_MAX_PORT_ID) ? "%02d\n" : "%02d, ", i);
            strcat(sz_info, sz_tmp);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }

        (void)aal_l2_tm_es_port_ena_set(device_id, i, TRUE);
    }

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("%s", sz_info);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if 0//yocto
    #if 0
    for (i=CA_AAL_ES_PORT_NI0; i<=CA_AAL_ES_PORT_NI6; i++)
        (void)aal_l2_tm_es_port_ena_set(device_id, i, TRUE);

    (void)aal_l2_tm_es_port_ena_set(device_id, CA_AAL_ES_PORT_L3QM, TRUE);
    (void)aal_l2_tm_es_port_ena_set(device_id, CA_AAL_ES_PORT_L3LAN, TRUE);
    (void)aal_l2_tm_es_port_ena_set(device_id, CA_AAL_ES_PORT_L3WAN, TRUE);
    (void)aal_l2_tm_es_port_ena_set(device_id, CA_AAL_ES_PORT_MC, TRUE);
    #endif
#endif

    /* enable voq level ES control */
    for (i=0; i<=CA_AAL_MAX_PORT_ID; i++)
        for (voq_id=0; voq_id<=7; voq_id++)
            (void)aal_l2_tm_es_voq_ena_set(device_id, i, voq_id, TRUE);

    /* init L2 TE */
    (void)aal_l2_te_init(device_id);

    /* init l3 tm and te */
    (void)aal_l3_tm_init(device_id);
    (void)aal_l3_te_init(device_id);

     return CA_E_OK;
}

#ifdef __KERNEL__
EXPORT_SYMBOL(aal_l2_tm_read_clr_set);
EXPORT_SYMBOL(aal_l2_tm_read_clr_get);
EXPORT_SYMBOL(aal_l2_tm_es_ena_set);
EXPORT_SYMBOL(aal_l2_tm_es_ena_get);
EXPORT_SYMBOL(aal_l2_tm_es_port_ena_set);
EXPORT_SYMBOL(aal_l2_tm_es_port_ena_get);
EXPORT_SYMBOL(aal_l2_tm_es_voq_status_get);
EXPORT_SYMBOL(aal_l2_tm_es_voq_flush_set);
EXPORT_SYMBOL(aal_l2_tm_es_voq_flush_get);
EXPORT_SYMBOL(aal_l2_tm_es_voq_ena_set);
EXPORT_SYMBOL(aal_l2_tm_es_voq_ena_get);
EXPORT_SYMBOL(aal_l2_tm_es_weight_ratio_set);
EXPORT_SYMBOL(aal_l2_tm_es_weight_ratio_get);
EXPORT_SYMBOL(aal_l2_tm_es_port_weight_ratio_set);
EXPORT_SYMBOL(aal_l2_tm_es_port_weight_ratio_get);
EXPORT_SYMBOL(aal_l2_tm_bm_buff_get);
EXPORT_SYMBOL(aal_l2_tm_bm_buff_dump_get);
EXPORT_SYMBOL(aal_l2_tm_bm_stats_get);
EXPORT_SYMBOL(aal_l2_tm_bm_status_get);
EXPORT_SYMBOL(aal_l2_tm_bm_cfg_set);
EXPORT_SYMBOL(aal_l2_tm_bm_cfg_get);
EXPORT_SYMBOL(aal_l2_tm_init);
#endif


