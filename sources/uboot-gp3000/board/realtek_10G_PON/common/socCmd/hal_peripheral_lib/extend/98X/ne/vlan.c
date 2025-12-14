#include <generated/ca_ne_autoconf.h>
#include "vlan.h"
#include "aal_l2_vlan.h"
#include "aal_port.h"
#include "aal_l2_cls.h"
#include "aal_fdb.h"
#include "aal_l3_cam.h"
#include "osal_common.h"
#include "sys.h"
#include "port.h"
#include "l2.h"
#include "aal_mcast.h"
#include "scfg.h"


#define __VLAN_PORT_MMSHP_NUM            16
#define __VAN_ACTION_PORT_NUM           8
#define __VLAN_NUM_PER_PORT             8
#define __VLAN_DEVID_FOR_SATURN         1

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define __VLAN_UTAG_STAG_ENTRY_ID       8
#define __VLAN_DTAG_ENTRY_ID            9
#else
#define __VLAN_UTAG_STAG_ENTRY_ID       14
#define __VLAN_DTAG_ENTRY_ID            15
#endif

#define __L2_VLAN_FIB_NUM               128
#define __L2_VLAN_LOGIC_PORT_NUM        64
#define __L2_VLAN_MC_FIB_NUM            64

#define __MAX_VLAN_ID                   4095
#define __MAX_PBITS_VALUE               0x7

/* totally 5 tpid can be added, three of them are predefined by ASIC,
   {0x8100, 0x9100, 0x88a8, user_defined_S_TPID, user_defined_C_TPID} */
#define __VLAN_TPID_NUM                 5

#define __IF_L2_VLAN_TPID_USER_DEFINE(tpid_value)\
    if((tpid_value != 0x8100) && (tpid_value != 0x9100) && (tpid_value != 0x88a8) && (tpid_value != 0))
extern ca_uint8_t g_l2_vlan_unknown_port_fib_map[64];

ca_uint16_t mcgid_ref_cnt[MCE_INDX_TBL_ENTRY_MAX];

typedef struct __l2_vlan_action {
    struct __l2_vlan_action *next;
    ca_vlan_key_type_t      key_type;
    ca_uint16_t             outer_vid;
    ca_uint16_t             inner_vid;
    ca_uint16_t              entry_id;
    ca_uint16_t              entry_offset;
    /*new_outer_vlan_src and new_outer_pri_src
    only for store the set value ,not support in G3 */
    ca_vlan_new_vlan_source_t     new_outer_vlan_src;
    ca_vlan_tag_priority_source_t new_outer_pri_src;
    ca_uint8_t                 inner_pbits;
    ca_uint8_t                 outer_pbits;
}__l2_vlan_action_t;

typedef struct {
    ca_uint8_t          counter;
    __l2_vlan_action_t  *next;
}__l2_vlan_action_header_t;


typedef struct __l2_vlan_id_item{
    ca_uint32_t             vlan_id;
    ca_uint32_t             fib_id;
    ca_uint32_t             memship_bmp;
    ca_uint32_t             untag_bmp;
    ca_uint64_t             mem_port_bmp;
    ca_uint64_t             untag_port_bmp;
    ca_uint64_t             flood_bmp_wan;
    ca_uint64_t             flood_bmp_lan;
    ca_uint64_t             mc_fib_id_bmp_wan;
    ca_uint64_t             mc_fib_id_bmp_lan;
    ca_uint32_t             mc_group_id_wan;
    ca_uint32_t             mc_group_id_lan;
    ca_boolean_t            mc_group_wan_valid;
    ca_boolean_t            mc_group_lan_valid;
    struct __l2_vlan_id_item *next;
}__l2_vlan_id_item_t;

typedef struct{
    ca_uint8_t counter;
    __l2_vlan_id_item_t *next;
}__l2_vlan_id_item_head_t;


static ca_uint32_t __g_l2_vlan_cls_entry_occupy[__VAN_ACTION_PORT_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

static ca_uint32_t __g_l2_vlan_egr_cls_entry_occupy[__VAN_ACTION_PORT_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

static ca_uint8_t __g_l2_vlan_mapped_port[__VLAN_PORT_MMSHP_NUM] = {
                            0,1,2,3,4,5,6,7,16,17,19,20,21,22,23,24
                            };
static ca_uint8_t __g_l2_vlan_mapped_mem_id[__L2_VLAN_LOGIC_PORT_NUM] = {
                            0,  1,  2,  3,  4,  5,  6,  7,
                            17, 17, 17, 17, 17, 17, 17, 17,
                            8,  9,  17, 10, 11, 12, 13, 14,
                            15, 17, 17, 17, 17, 17, 17, 17,
                            17, 17, 17, 17, 17, 17, 17, 17,
                            17, 17, 17, 17, 17, 17, 17, 17,
                            17, 17, 17, 17, 17, 17, 17, 17,
                            17, 17, 17, 17, 17, 17, 17, 17
                            };

static ca_uint32_t __g_l2_vlan_fib_wan_occupy[4] = {0,0,0,0};
static ca_uint32_t __g_l2_vlan_fib_lan_occupy[4] = {0,0,0,0};

static __l2_vlan_action_header_t __port_vlan_ingress_act_head[__VAN_ACTION_PORT_NUM];

static __l2_vlan_action_header_t __port_vlan_egress_act_head[__VAN_ACTION_PORT_NUM];

/* tpid poll set by ca_vlan_tpid_set() */
static ca_uint32_t __vlan_inner_tpid[__VLAN_TPID_NUM];
static ca_uint32_t __vlan_outer_tpid[__VLAN_TPID_NUM];

/* CSR_TPID_CMP_S & CSR_TPID_CMP_C are shared by all ports */
static ca_uint64_t __vlan_tpid_cmp_c_used_bmp = 0; /* each bit for one port */
static ca_uint64_t __vlan_tpid_cmp_s_used_bmp = 0;

/* 1. S and C can't indicate at egress, so CSR_VLAN_TPID_S/C set the same value
   2. CSR_VLAN_TPID_A & CSR_VLAN_TPID_C are init to be 0x8100, and can't be changed as untag packets will use it
   3. CSR_VLAN_TPID_B & CSR_VLAN_TPID_D are shared by all ports, need a bitmap to record which port used */
static ca_uint64_t __vlan_tpid_b_d_used_bmp = 0;

static __l2_vlan_id_item_head_t  __g_l2_vlan_id_item;

#define __VLAN_COMPBIT(value, bit_index)    (value & (1<< (bit_index)))



static ca_status_t __l2_vlan_fib_id_wan_reserve(ca_uint32_t fib_id)
{
    if(fib_id > 127){
        return CA_E_PARAM;
    }
    __g_l2_vlan_fib_wan_occupy[fib_id/32] |= (1<<(fib_id%32));

    return CA_E_OK;
}

static ca_status_t __l2_vlan_fib_id_lan_reserve(ca_uint32_t fib_id)
{
    if(fib_id > 127){
        return CA_E_PARAM;
    }
    __g_l2_vlan_fib_lan_occupy[fib_id/32] |= (1<<(fib_id%32));

    return CA_E_OK;
}




static ca_uint8_t __l2_vlan_fib_id_wan_search(void)
{
    ca_uint8 fib_id = 0;
    for(fib_id= 0; fib_id<__L2_VLAN_FIB_NUM; fib_id ++){
        if((__g_l2_vlan_fib_wan_occupy[fib_id/32] & (1<<(fib_id%32))) == 0){
            __l2_vlan_fib_id_wan_reserve(fib_id);
            break;
        }
    }
    return fib_id;
}
static ca_status_t __l2_vlan_fib_id_wan_release(ca_uint32_t fib_id)
{
    if(fib_id > 127){
        return CA_E_PARAM;
    }
    __g_l2_vlan_fib_wan_occupy[fib_id/32] &= ~(1<<(fib_id%32));

    return CA_E_OK;
}

static ca_uint8_t __l2_vlan_fib_id_lan_search(void)
{
    ca_uint8 fib_id = 0;
    for(fib_id= 0; fib_id<__L2_VLAN_FIB_NUM; fib_id ++){
        if((__g_l2_vlan_fib_lan_occupy[fib_id/32] & (1<<(fib_id%32))) == 0){
            __l2_vlan_fib_id_lan_reserve(fib_id);
            break;
        }
    }
    return fib_id;
}

static ca_status_t __l2_vlan_fib_id_lan_release(ca_uint32_t fib_id)
{
    if(fib_id > 127){
        return CA_E_PARAM;
    }
    __g_l2_vlan_fib_lan_occupy[fib_id/32] &= ~(1<<(fib_id%32));

    return CA_E_OK;
}

static ca_int32_t __l2_vlan_action_record_find(ca_uint16_t port_id, ca_vlan_key_type_t key_type,
        ca_uint16_t outer_vid, ca_uint16_t inner_vid, __l2_vlan_action_t **pentry)
{
    __l2_vlan_action_t   *ptemp, *pre;

    pre = NULL;
    ptemp   = __port_vlan_ingress_act_head[port_id].next;

    while (ptemp) {

        if ((0 == memcmp(&key_type, &(ptemp->key_type),sizeof(ca_vlan_key_type_t))) &&
                (outer_vid == ptemp->outer_vid) &&
                (inner_vid == ptemp->inner_vid)) {
            if (pentry) {
                *pentry = ptemp;
            }
            return 1;
        }
        pre = ptemp;
        ptemp = ptemp->next;
    }

    return 0;
}

static ca_status_t __l2_vlan_action_record_delete(ca_uint16_t port_id, ca_vlan_key_type_t key_type,
        ca_uint16_t outer_vid, ca_uint16_t inner_vid)
{
    __l2_vlan_action_t   *ptemp, *pre;
    pre = NULL;
    ptemp = __port_vlan_ingress_act_head[port_id].next;

    while (ptemp) {
        if ((0 == memcmp(&key_type, &(ptemp->key_type),sizeof(ca_vlan_key_type_t))) &&
            (outer_vid == ptemp->outer_vid) && (inner_vid == ptemp->inner_vid)) {
            if (pre) {
                pre->next = ptemp->next;
            } else {

                __port_vlan_ingress_act_head[port_id].next = ptemp->next;
            }
            pre = ptemp;
            ptemp = ptemp->next;

           // ca_free(pre);

            __port_vlan_ingress_act_head[port_id].counter--;

            while (ptemp) {
                pre = ptemp;
                ptemp = ptemp->next;
            }
            pre->next = NULL;

            return CA_E_OK;
        }
        pre = ptemp;
        ptemp = ptemp->next;
    }
    return CA_E_OK;
}

static ca_status_t __l2_vlan_action_record_add(ca_uint16_t port_id, __l2_vlan_action_t *pentry)
{
    __l2_vlan_action_t   *ptemp, *pre;
    ca_status_t ret = CA_E_OK;

    pre = NULL;

    if (__l2_vlan_action_record_find(port_id, pentry->key_type, pentry->outer_vid, pentry->inner_vid, &ptemp)) {

        ret = __l2_vlan_action_record_delete(port_id, pentry->key_type, pentry->outer_vid, pentry->inner_vid);
        if (CA_E_OK != ret) {
            goto end;
        }
    }

    pentry->next = NULL;

    ptemp   = __port_vlan_ingress_act_head[port_id].next;

    if (ptemp == NULL) {
        __port_vlan_ingress_act_head[port_id].counter = 1;
        __port_vlan_ingress_act_head[port_id].next = pentry;
        goto end;
    }
    while (ptemp) {
        pre = ptemp;
        ptemp = ptemp->next;
    }
    pre->next = pentry;
    __port_vlan_ingress_act_head[port_id].counter++;

end:
    return ret;
}



static ca_int32_t __l2_vlan_egress_action_record_find(ca_uint16_t port_id, ca_vlan_key_type_t key_type,
        ca_uint16_t outer_vid, ca_uint16_t inner_vid, __l2_vlan_action_t **pentry)
{
    __l2_vlan_action_t   *ptemp, *pre;

    pre = NULL;
    ptemp   = __port_vlan_egress_act_head[port_id].next;

    while (ptemp) {
        if ((0 == memcmp(&key_type, &(ptemp->key_type),sizeof(ca_vlan_key_type_t))) &&
                (outer_vid == ptemp->outer_vid) && (inner_vid == ptemp->inner_vid)) {
            if (pentry) {
                *pentry = ptemp;
            }
            return 1;
        }
        pre = ptemp;
        ptemp = ptemp->next;
    }
    return 0;
}

static ca_status_t __l2_vlan_egress_action_record_delete(ca_uint16_t port_id, ca_vlan_key_type_t key_type,
        ca_uint16_t outer_vid, ca_uint16_t inner_vid)
{
    __l2_vlan_action_t   *ptemp, *pre;
    pre = NULL;
    ptemp = __port_vlan_egress_act_head[port_id].next;

    while (ptemp) {
        if ((0 == memcmp(&key_type, &(ptemp->key_type),sizeof(ca_vlan_key_type_t))) &&
            (outer_vid == ptemp->outer_vid) && (inner_vid == ptemp->inner_vid)) {
            if (pre) {
                pre->next = ptemp->next;
            } else {
                __port_vlan_egress_act_head[port_id].next = ptemp->next;
            }
            pre = ptemp;
            ptemp = ptemp->next;
            //ca_free(pre);
            __port_vlan_egress_act_head[port_id].counter--;
            while (ptemp) {
                ptemp = ptemp->next;
            }
            return CA_E_OK;
        }
        pre = ptemp;
        ptemp = ptemp->next;
    }
    return CA_E_OK;
}


static ca_status_t __l2_vlan_egress_action_record_add(ca_uint16_t port_id, __l2_vlan_action_t *pentry)
{
    __l2_vlan_action_t   *ptemp, *pre;
    ca_status_t ret = CA_E_OK;

    pre = NULL;

    if (__l2_vlan_egress_action_record_find(port_id, pentry->key_type, pentry->outer_vid, pentry->inner_vid, &ptemp)) {
        ret = __l2_vlan_egress_action_record_delete(port_id, pentry->key_type, pentry->outer_vid, pentry->inner_vid);
        if (CA_E_OK != ret) {
            goto end;
        }
    }

    pentry->next = NULL;

    ptemp   = __port_vlan_egress_act_head[port_id].next;

    if (ptemp == NULL) {
        __port_vlan_egress_act_head[port_id].counter = 1;
        __port_vlan_egress_act_head[port_id].next = pentry;
        goto end;
    }
    while (ptemp) {
        pre = ptemp;
        ptemp = ptemp->next;
    }
    pre->next = pentry;
    __port_vlan_egress_act_head[port_id].counter++;

end:
    return ret;
}



#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define __L2_VLAN_CLS_ENTRY_GET_START 12
#define __L2_VLAN_CLS_ENTRY_GET_END   15
#else
#define __L2_VLAN_CLS_ENTRY_GET_START 20
#define __L2_VLAN_CLS_ENTRY_GET_END   27
#endif

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define __L2_VLAN_EGR_CLS_ENTRY_GET_START 0
#define __L2_VLAN_EGR_CLS_ENTRY_GET_END   15
#else
#define __L2_VLAN_EGR_CLS_ENTRY_GET_START 0
#define __L2_VLAN_EGR_CLS_ENTRY_GET_END   27
#endif

ca_status_t __l2_vlan_cls_entry_get(ca_uint32_t log_port, ca_uint8_t *entry_id, ca_uint8 *entry_offset)
{
    ca_uint8 i;
    for (i = __L2_VLAN_CLS_ENTRY_GET_START; i < __L2_VLAN_CLS_ENTRY_GET_END; i++) {
        if (((__g_l2_vlan_cls_entry_occupy[log_port] >> i)&0x1) == 0) {
            break;
        }
    }
    if (i == __L2_VLAN_CLS_ENTRY_GET_END) {
        return CA_E_RESOURCE;
    }
    *entry_id = i / 2 + log_port*__INGRESS_CLS_ENTRY_NUM_PER_PORT;
    *entry_offset = i % 2;
    return CA_E_OK;
}

ca_status_t __l2_vlan_egr_cls_entry_get(ca_uint32_t log_port, ca_uint8_t *entry_id, ca_uint8 *entry_offset)
{
    ca_uint8 i;
    for (i = __L2_VLAN_EGR_CLS_ENTRY_GET_START; i < __L2_VLAN_EGR_CLS_ENTRY_GET_END; i++) {
        if (((__g_l2_vlan_egr_cls_entry_occupy[log_port] >> i)&0x1) == 0) {
            break;
        }
    }
    if (i == __L2_VLAN_CLS_ENTRY_GET_END) {
        return CA_E_RESOURCE;
    }

    *entry_id = i / 2 + log_port * __EGRESS_CLS_ENTRY_NUM_PER_PORT;
    *entry_offset = i % 2;

    return CA_E_OK;
}

ca_status_t __l2_vlan_mc_group_port_delete_all(ca_uint32_t mc_group_id)
{
    ca_uint32_t logic_port_id = 0;

    for(logic_port_id = 0; logic_port_id < __L2_VLAN_LOGIC_PORT_NUM; logic_port_id++){
        aal_flooding_port_delete(mc_group_id, logic_port_id);
    }
    return CA_E_OK;

}
extern void aal_l2_vlan_init(ca_device_id_t device_id);
static int __l2_vlan_action_table_init(void)
{

    ca_uint32_t i;
    for(i = 0; i<__VAN_ACTION_PORT_NUM; i++){
        __port_vlan_ingress_act_head[i].counter = 0;
        __port_vlan_ingress_act_head[i].next = NULL;
        __port_vlan_egress_act_head[i].counter = 0;
        __port_vlan_egress_act_head[i].next = NULL;
    }
    __g_l2_vlan_id_item.counter      = 0;
    __g_l2_vlan_id_item.next         = NULL;

    for(i = 0; i<MCE_INDX_TBL_ENTRY_MAX; i++){
        mcgid_ref_cnt[i] = 0;
    }
    return 0;
}

ca_status_t ca_l2_vlan_learning_shared_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_vlan_learning_mode_t mode
)
{
    aal_l2_vlan_default_cfg_mask_t  vlan_def_cfg_mask;
    aal_l2_vlan_default_cfg_t       vlan_def_cfg;

    aal_fdb_ctrl_mask_t             fdb_ctrl_mask;
    aal_fdb_ctrl_t                  fdb_ctrl;
    ca_status_t                     ret = CA_E_OK;

    /* turn off fdb learn */
    fdb_ctrl_mask.wrd = 0;
    fdb_ctrl_mask.s.lrn_dis = 1;
    fdb_ctrl.lrn_dis = 1;
    ret = aal_fdb_ctrl_set(0, fdb_ctrl_mask, &fdb_ctrl);
    CA_RET_VALUE_CHECK(ret);

    /* flush fdb */
    ret = aal_fdb_addr_delete_all(0, CA_L2_ADDR_OP_BOTH);
    CA_RET_VALUE_CHECK(ret);

    vlan_def_cfg_mask.u32 = 0;
    memset(&vlan_def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    vlan_def_cfg_mask.s.vlan_mode = 1;
    vlan_def_cfg_mask.s.unknown_vlan_fwd_ena = 1;
    if(mode == CA_L2_VLAN_LEARNING_IVL){
        vlan_def_cfg.vlan_mode = AAL_L2_VLAN_MODE_IVL;
        vlan_def_cfg.unknown_vlan_fwd_ena = TRUE;
    }else if(mode == CA_L2_VLAN_LEARNING_SVL){
        vlan_def_cfg.vlan_mode = AAL_L2_VLAN_MODE_SVL;
        vlan_def_cfg.unknown_vlan_fwd_ena = TRUE;
    }else {
        ca_printf("not support CA_L2_VLAN_LEARNING_DIS(2) mode\r\n");
        fdb_ctrl.lrn_dis = 0;
        ret = aal_fdb_ctrl_set(0, fdb_ctrl_mask, &fdb_ctrl);
        return CA_E_NOT_SUPPORT;
    }
    ret = aal_l2_vlan_default_cfg_set(device_id, vlan_def_cfg_mask, &vlan_def_cfg);
    CA_RET_VALUE_CHECK(ret);

    /* turn on fdb learn */
    fdb_ctrl.lrn_dis = 0;
    ret = aal_fdb_ctrl_set(0, fdb_ctrl_mask, &fdb_ctrl);
    CA_RET_VALUE_CHECK(ret);

    return ret;
}

ca_status_t ca_l2_vlan_learning_shared_get(
    CA_IN  ca_device_id_t  device_id,
    CA_OUT ca_vlan_learning_mode_t *mode
)
{
    aal_l2_vlan_default_cfg_t   vlan_def_cfg;
    ca_status_t                 ret = CA_E_OK;

    if (mode == NULL) {
        return CA_E_PARAM;
    }
    memset(&vlan_def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));

    ret = aal_l2_vlan_default_cfg_get(device_id , &vlan_def_cfg);
    if (ret != CA_E_OK) {
        return ret;
    }
    if(vlan_def_cfg.vlan_mode == AAL_L2_VLAN_MODE_IVL){
        *mode = CA_L2_VLAN_LEARNING_IVL;
    }else if(vlan_def_cfg.vlan_mode == AAL_L2_VLAN_MODE_SVL){
        *mode = CA_L2_VLAN_LEARNING_SVL;
    }else{
        *mode = CA_L2_VLAN_LEARNING_IVL;
    }

    return CA_E_OK;
}
extern     ca_uint32_t g_vlan_mcgid_0 ;

extern     ca_uint32_t g_vlan_flooding_domain_1[CFG_ID_FLOODING_DOMAIN_LEN];
ca_status_t ca_l2_vlan_port_control_set(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_port_id_t           port_id,
    CA_IN ca_vlan_port_control_t *config
)
{
    aal_l2_vlan_default_cfg_mask_t  vlan_def_cfg_mask;
    aal_l2_vlan_default_cfg_t       vlan_def_cfg;
    aal_ilpb_cfg_msk_t              ilpb_cfg_mask;
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_arb_mc_fib_mask_t           mc_fib_mask;
    aal_arb_mc_fib_t                mc_fib_cfg;
    ca_int16_t                      mc_fib_id = 0;

    aal_elpb_cfg_msk_t              elpb_cfg_mask;
    aal_elpb_cfg_t                  elpb_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint32_t                     mapped_port_id;

    ca_status_t                     ret = CA_E_OK;

    if (NULL == config ) {
        return CA_E_PARAM;
    }
    if(config->default_tag >__MAX_VLAN_ID){
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);

    logic_port_id = PORT_ID(port_id);

    ret = aal_l2_vlan_port_memship_map_get(device_id,logic_port_id,&mapped_port_id);
    if (ret != CA_E_OK) {
        goto end;
    }

    vlan_def_cfg_mask.u32 = 0;
    memset(&ilpb_cfg_mask, 0, sizeof(aal_ilpb_cfg_msk_t));
    elpb_cfg_mask.u32[0] = 0;
    memset(&vlan_def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&elpb_cfg, 0, sizeof(aal_elpb_cfg_t));

    vlan_def_cfg_mask.s.rx_unknown_vlan_memship_check_bmp = 1;
    vlan_def_cfg_mask.s.tx_unknown_vlan_memship_check_bmp = 1;
    ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_def_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    if (config->drop_unknown_vlan) {
        vlan_def_cfg.rx_unknown_vlan_memship_check_bmp &= ~(1 << mapped_port_id);
        vlan_def_cfg.tx_unknown_vlan_memship_check_bmp &= ~(1 << mapped_port_id);

        mc_fib_id = g_l2_vlan_unknown_port_fib_map[logic_port_id];
        if(mc_fib_id != 0xff){
            ret = aal_flooding_mc_fib_delete(g_vlan_mcgid_0, mc_fib_id);
            ca_printf("%s:%d mc_fib_id = %d,g_vlan_mcgid_0 = %d in aal_flooding_mc_fib_delete() \n", __FUNCTION__,__LINE__,mc_fib_id,g_vlan_mcgid_0);
            if (ret != CA_E_OK) {
                ca_printf("%s:%d aal_flooding_mc_fib_delete() failed\n", __FUNCTION__,__LINE__);
                goto end;
            }
            ret = aal_flooding_fib_idx_free(mc_fib_id);
            if (ret != CA_E_OK) {
                ca_printf("%s:%d aal_flooding_fib_idx_free() failed\n", __FUNCTION__,__LINE__);
                goto end;
            }
        }
        g_l2_vlan_unknown_port_fib_map[logic_port_id] = 0xff;
    } else {
        vlan_def_cfg.rx_unknown_vlan_memship_check_bmp |= (1 << mapped_port_id);
        vlan_def_cfg.tx_unknown_vlan_memship_check_bmp |= (1 << mapped_port_id);
        if(g_l2_vlan_unknown_port_fib_map[logic_port_id] == 0xff){
            mc_fib_id = aal_flooding_fib_idx_alloc();
            if(mc_fib_id<0){
                ret = CA_E_RESOURCE;
                goto end;
            }

         ca_printf("mc_fib_id = %d,g_vlan_mcgid_0 = %d in aal_flooding_mc_fib_add()\n",mc_fib_id,g_vlan_mcgid_0);
         /*   ret = aal_l2_unknown_vlan_mc_fib_bmp_get(&mc_fib_idx_bmp);
            if(ret != CA_E_OK){
                goto end;
            }
            mc_fib_idx_bmp |= (1<<mc_fib_id);*/

            ret = aal_flooding_mc_fib_add(g_vlan_mcgid_0, mc_fib_id);
            if (ret != CA_E_OK) {
                ca_printf("%s:%d aal_flooding_mc_fib_add() failed\n", __FUNCTION__,__LINE__);
                goto end;
            }
            g_l2_vlan_unknown_port_fib_map[logic_port_id] = mc_fib_id;
            mc_fib_mask.u32 = 0;
            mc_fib_cfg.ldpid = logic_port_id;
            mc_fib_mask.s.ldpid = 1;
#ifdef      MC_FIB_COS_CMD
            mc_fib_mask.s.cos_cmd = 1;
            mc_fib_cfg.cos_cmd = 1;
#endif
            aal_arb_mc_fib_set(device_id,(ca_uint32_t)mc_fib_id,mc_fib_mask,&mc_fib_cfg);
        }
    }

    ilpb_cfg_mask.s.ing_fltr_ena = 1;
    ilpb_cfg.ing_fltr_ena = config->ingr_membership_check_enable;
    elpb_cfg_mask.s.egr_ve_srch_en = 1;
    elpb_cfg.egr_ve_srch_en = config->egr_membership_check_enable;



    elpb_cfg_mask.s.egr_cls_lookup_en = 1;
    elpb_cfg.egr_cls_lookup_en = config->egress_vlan_action_enable;

    ilpb_cfg_mask.s.vlan_cmd_sel_bm = 1;

    if (config->ingress_vlan_action_enable == FALSE) {
        ilpb_cfg.vlan_cmd_sel_bm = 0;
    } else {
        ilpb_cfg.vlan_cmd_sel_bm = 3;
    }
    ilpb_cfg_mask.s.default_top_vlan_cmd = 1;
    ilpb_cfg_mask.s.default_untag_cmd = 1;
    ilpb_cfg_mask.s.default_vlanid = 1;
    ilpb_cfg.default_vlanid = config->default_tag;
    if(config->default_tag_add){
       ilpb_cfg.default_top_vlan_cmd = AAL_VLAN_CMD_PUSH;
       ilpb_cfg.default_untag_cmd = AAL_VLAN_CMD_PUSH;
    }else{
        ilpb_cfg.default_top_vlan_cmd = AAL_VLAN_CMD_NOP;
        ilpb_cfg.default_untag_cmd = AAL_VLAN_CMD_NOP;
    }

#if 0
    if(config->outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN){
        ilpb_cfg_mask.s.top_s_tpid_enable = 1;
        ilpb_cfg_mask.s.top_c_tpid_enable = 1;
        ilpb_cfg.top_s_tpid_enable = 1;
        ilpb_cfg.top_c_tpid_enable = 0;

    }else if(config->outer_tpid == CA_L2_VLAN_TAG_TYPE_CVLAN){
        ilpb_cfg_mask.s.top_s_tpid_enable = 1;
        ilpb_cfg_mask.s.top_c_tpid_enable = 1;
        ilpb_cfg.top_s_tpid_enable = 0;
        ilpb_cfg.top_c_tpid_enable = 1;

    }else{
        return CA_E_PARAM;
    }


    if(config->inner_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN){
        ilpb_cfg_mask.s.inner_s_tpid_enable = 1;
        ilpb_cfg_mask.s.inner_c_tpid_enable = 1;
        ilpb_cfg.inner_s_tpid_enable = 1;
        ilpb_cfg.inner_c_tpid_enable = 0;

    }else if(config->inner_tpid == CA_L2_VLAN_TAG_TYPE_CVLAN){
        ilpb_cfg_mask.s.inner_s_tpid_enable = 1;
        ilpb_cfg_mask.s.inner_c_tpid_enable = 1;
        ilpb_cfg.inner_s_tpid_enable = 0;
        ilpb_cfg.inner_c_tpid_enable = 1;

    }else{
        return CA_E_PARAM;
    }
#endif

    ret = aal_l2_vlan_default_cfg_set(device_id, vlan_def_cfg_mask, &vlan_def_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    ret = aal_port_ilpb_cfg_set(device_id, logic_port_id, ilpb_cfg_mask, &ilpb_cfg);

    if (ret != CA_E_OK) {
        goto end;
    }
    ret = aal_port_elpb_cfg_set(device_id, logic_port_id, elpb_cfg_mask, &elpb_cfg);
end:
    return ret;
}

ca_status_t ca_l2_vlan_port_control_get(
    CA_IN  ca_device_id_t            device_id,
    CA_IN  ca_port_id_t           port_id,
    CA_OUT ca_vlan_port_control_t *config
)
{
    aal_l2_vlan_default_cfg_t       vlan_def_cfg;
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_elpb_cfg_t                  elpb_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint32_t                     mapped_port_id;
    ca_status_t                     ret = CA_E_OK;

    if (NULL == config) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);

    logic_port_id = PORT_ID(port_id);
    ret = aal_l2_vlan_port_memship_map_get(device_id,logic_port_id,&mapped_port_id);
    if (ret != CA_E_OK) {
        goto end;
    }

    memset(&vlan_def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    memset(&elpb_cfg, 0, sizeof(aal_elpb_cfg_t));

    ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_def_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    ret = aal_port_elpb_cfg_get(device_id, logic_port_id, &elpb_cfg);

    if (ret != CA_E_OK) {
        goto end;
    }

    config->drop_unknown_vlan  = (((vlan_def_cfg.rx_unknown_vlan_memship_check_bmp>>mapped_port_id)&0x01) == 0);
    config->ingr_membership_check_enable = ilpb_cfg.ing_fltr_ena;
    config->egr_membership_check_enable = elpb_cfg.egr_ve_srch_en;
    config->egress_vlan_action_enable = elpb_cfg.egr_cls_lookup_en;
    config->ingress_vlan_action_enable = (ilpb_cfg.vlan_cmd_sel_bm > 0);
    config->default_tag_add   = (ilpb_cfg.default_top_vlan_cmd == AAL_VLAN_CMD_PUSH);
    config->default_tag       = ilpb_cfg.default_vlanid;

    if(ilpb_cfg.top_s_tpid_enable == 1 && ilpb_cfg.top_c_tpid_enable == 0){
        config->outer_tpid = CA_L2_VLAN_TAG_TYPE_SVLAN;

    }else if(ilpb_cfg.top_s_tpid_enable == 0 && ilpb_cfg.top_c_tpid_enable == 1){
        config->outer_tpid = CA_L2_VLAN_TAG_TYPE_CVLAN;

    }else{
        config->outer_tpid = CA_L2_VLAN_TAG_TYPE_SVLAN;
    }

    if(ilpb_cfg.inner_s_tpid_enable == 1 && ilpb_cfg.inner_c_tpid_enable == 0){
        config->inner_tpid = CA_L2_VLAN_TAG_TYPE_SVLAN;

    }else if(ilpb_cfg.inner_s_tpid_enable == 0 && ilpb_cfg.inner_c_tpid_enable == 1){
        config->inner_tpid = CA_L2_VLAN_TAG_TYPE_CVLAN;

    }else{
        config->inner_tpid = CA_L2_VLAN_TAG_TYPE_SVLAN;
    }

end:
    return ret;
}

ca_status_t ca_l2_vlan_tpid_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t *inner_tpid,
    CA_IN ca_uint32_t inner_nums,
    CA_IN ca_uint32_t *outer_tpid,
    CA_IN ca_uint32_t outer_nums
)
{
    ca_uint32_t i = 0;
    ca_uint32_t user_def_inner_num = 0;
    ca_uint32_t user_def_outer_num = 0;

    if ((NULL == inner_tpid) || (NULL == outer_tpid)) {
        return CA_E_PARAM;
    }

    /* check inpput parameters */
    if(inner_nums > __VLAN_TPID_NUM || inner_nums == 0 ||
        outer_nums > __VLAN_TPID_NUM || outer_nums == 0){
        return CA_E_PARAM;
    }
    if(inner_tpid[0] != 0x8100 || outer_tpid[0] != 0x8100){
        ca_printf("index 0 should be 0x8100, and cannot be removed\r\n");
        return CA_E_PARAM;
    }

    for(i = 0; i < inner_nums; i++){
        if(inner_tpid[i] != 0x8100 && inner_tpid[i] != 0x9100 && inner_tpid[i] != 0x88a8){
            user_def_inner_num++;
            if(user_def_inner_num > 2){
                ca_printf("only support two user defined inner tpid\r\n");
                return CA_E_PARAM;
            }
        }
    }
    for(i = 0; i < outer_nums; i++){
        if(outer_tpid[i] != 0x8100 && outer_tpid[i] != 0x9100 && outer_tpid[i] != 0x88a8){
            user_def_outer_num++;
            if(user_def_outer_num > 2){
                ca_printf("only support two user defined outer tpid\r\n");
                return CA_E_PARAM;
            }
        }
    }

    /* store the tpid setting */
    memset(&__vlan_inner_tpid[0], 0 , __VLAN_TPID_NUM * sizeof(__vlan_inner_tpid[0]));
    for(i = 0; i < inner_nums; i++){
        __vlan_inner_tpid[i] = inner_tpid[i];
    }

    memset(&__vlan_outer_tpid[0], 0 , __VLAN_TPID_NUM * sizeof(__vlan_outer_tpid[0]));
    for(i = 0; i < outer_nums; i++){
        __vlan_outer_tpid[i] = outer_tpid[i];
    }

#if 0//yocto
    /* set l3fe tpid entries */
    aal_l3fe_pp_tpid_set(outer_nums, outer_tpid, inner_nums, inner_tpid);
#else//sd1 uboot for 98f - not to config l3fe yet
#endif//sd1 uboot for 98f - not to config l3fe yet

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
    aal_l3fe_pp_tpid_saturn_set(outer_nums, outer_tpid, inner_nums, inner_tpid);
#endif
    return CA_E_OK;

}

ca_status_t ca_l2_vlan_tpid_get(
    CA_IN  ca_device_id_t device_id,
    CA_OUT ca_uint32_t *inner_tpid,
    CA_OUT ca_uint32_t *inner_nums,
    CA_OUT ca_uint32_t *outer_tpid,
    CA_OUT ca_uint32_t *outer_nums
)
{
    ca_uint32_t i = 0;
    ca_uint32_t inner_num = 0;
    ca_uint32_t outer_num = 0;

    if ((NULL == inner_tpid) || (NULL == outer_tpid) || (NULL == inner_nums) || (NULL == outer_nums)) {
        return CA_E_PARAM;
    }

    for(i = 0; i < __VLAN_TPID_NUM; i++){
        if(__vlan_inner_tpid[i] != 0){
            inner_tpid[inner_num] = __vlan_inner_tpid[i];
            inner_num++;
        }else{
            break;
        }
    }
    *inner_nums = inner_num;

    for(i = 0; i < __VLAN_TPID_NUM; i++){
        if(__vlan_outer_tpid[i] != 0){
            outer_tpid[outer_num] = __vlan_outer_tpid[i];
            (outer_num)++;
        }else{
            break;
        }
    }
    *outer_nums = outer_num;

    return CA_E_OK;
}

ca_status_t __l2_vlan_outer_tpid_value2index(ca_uint32_t tpid_value, ca_uint32_t *tpid_index)
{
    ca_uint32_t i = 0;

    for(i = 0; i < __VLAN_TPID_NUM; i++){
        if(__vlan_outer_tpid[i] != 0){
            if(__vlan_outer_tpid[i] == tpid_value){
                *tpid_index = i;
                return CA_E_OK;
            }
        }else{
            break;
        }
    }

    return CA_E_ERROR;
}
ca_status_t __l2_vlan_inner_tpid_value2index(ca_uint32_t tpid_value, ca_uint32_t *tpid_index)
{
    ca_uint32_t i = 0;

    for(i = 0; i < __VLAN_TPID_NUM; i++){
        if(__vlan_inner_tpid[i] != 0){
            if(__vlan_inner_tpid[i] == tpid_value){
                *tpid_index = i;
                return CA_E_OK;
            }
        }else{
            break;
        }
    }

    return CA_E_ERROR;
}

void __l2_vlan_tpid_value2bmp_add(ca_uint32_t tpid_value, ca_uint8_t tpid_bmp_src, ca_uint8_t *tpid_bmp_dest)
{
    ca_uint8_t tpid_match_bmp = tpid_bmp_src;

    if(tpid_value == 0x8100){
        tpid_match_bmp |= (1 << AAL_VLAN_TPID_INDEX_8100);

    }else if(tpid_value == 0x9100){
        tpid_match_bmp |= (1 << AAL_VLAN_TPID_INDEX_9100);

    }else if(tpid_value == 0x88a8){
        tpid_match_bmp |= (1 << AAL_VLAN_TPID_INDEX_88A8);

    }else if(tpid_value != 0){
        tpid_match_bmp |= (1 << AAL_VLAN_TPID_INDEX_USER_DEFINE);
    }
    *tpid_bmp_dest = tpid_match_bmp;
}

void __l2_vlan_tpid_value2bmp_delete(ca_uint32_t tpid_value, ca_uint8_t tpid_bmp_src, ca_uint8_t *tpid_bmp_dest)
{
    ca_uint8_t tpid_match_bmp = tpid_bmp_src;

    if(tpid_value == 0x8100){
        tpid_match_bmp &= ~(1 << AAL_VLAN_TPID_INDEX_8100);

    }else if(tpid_value == 0x9100){
        tpid_match_bmp &= ~(1 << AAL_VLAN_TPID_INDEX_9100);

    }else if(tpid_value == 0x88a8){
        tpid_match_bmp &= ~(1 << AAL_VLAN_TPID_INDEX_88A8);

    }else if(tpid_value != 0){
        tpid_match_bmp &= ~(1 << AAL_VLAN_TPID_INDEX_USER_DEFINE);
    }
    *tpid_bmp_dest = tpid_match_bmp;
}

void __l2_vlan_tpid_bmp2list(ca_uint8_t tpid_bmp, ca_uint32_t *tpid, ca_uint32_t *tpid_num)
{
    ca_uint32_t i = 0;
    ca_uint32_t *temp_tpid = NULL;
    ca_uint32_t temp_num = 0;

    temp_tpid = tpid;
    for (i = 1; i < 4; i++) { /* index 0 is user defined, the tpid value should be got from VLAN_TPID_S/C */
        if (tpid_bmp & (1 << i)) {
            temp_num++;
            if(i == AAL_VLAN_TPID_INDEX_8100){
                *temp_tpid = 0x8100;

            }else if(i == AAL_VLAN_TPID_INDEX_9100){
                *temp_tpid = 0x9100;

            }else if(i == AAL_VLAN_TPID_INDEX_88A8){
                *temp_tpid = 0x88a8;

            }
            temp_tpid++;
        }
    }
    *tpid_num = temp_num;
}

ca_status_t __l2_vlan_tpid_match_bmp_set(ca_device_id_t device_id, ca_uint32_t lpid, ca_uint32_t tpid_bmp, ca_vlan_tag_type_t type)
{
    aal_ilpb_cfg_msk_t              ilpb_cfg_mask;
    aal_ilpb_cfg_t                  ilpb_cfg;

    memset(&ilpb_cfg_mask, 0, sizeof(aal_ilpb_cfg_msk_t));
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));

    if(type == CA_L2_VLAN_TAG_TYPE_SVLAN){
        ilpb_cfg_mask.s.s_tpid_match = 1;
        ilpb_cfg.s_tpid_match = tpid_bmp;
    }else{
        ilpb_cfg_mask.s.c_tpid_match = 1;
        ilpb_cfg.c_tpid_match = tpid_bmp;
    }

    return aal_port_ilpb_cfg_set(device_id, lpid, ilpb_cfg_mask, &ilpb_cfg);
}

ca_status_t __l2_vlan_tpid_cmp_value_set(ca_device_id_t device_id, ca_uint32_t tpid, ca_vlan_tag_type_t type)
{
    aal_l2_vlan_default_cfg_mask_t  vlan_def_cfg_mask;
    aal_l2_vlan_default_cfg_t       vlan_def_cfg;

    memset(&vlan_def_cfg_mask, 0, sizeof(aal_l2_vlan_default_cfg_mask_t));
    memset(&vlan_def_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));

    if(type == CA_L2_VLAN_TAG_TYPE_SVLAN){
        vlan_def_cfg_mask.s.cmp_tpid_svlan  = 1;
        vlan_def_cfg.cmp_tpid_svlan         = tpid;
    }else{
        vlan_def_cfg_mask.s.cmp_tpid_cvlan  = 1;
        vlan_def_cfg.cmp_tpid_cvlan         = tpid;
    }
    return aal_l2_vlan_default_cfg_set(device_id, vlan_def_cfg_mask, &vlan_def_cfg);
}

ca_status_t __l2_vlan_non_uc_dpid_fib_index_search(ca_device_id_t device_id,
                                                                ca_uint32_t port_id,
                                                                ca_uint64_t *fib_bmp)
{
    ca_uint32_t fib_idx = 0;
    ca_status_t ret = CA_E_OK;
    aal_arb_mc_fib_t mc_fib;
    if((port_id>63) ||(NULL == fib_bmp)){
            return CA_E_PARAM;
    }
    for (fib_idx = 0; fib_idx<64; fib_idx++){
        ret = aal_arb_mc_fib_get(device_id, fib_idx,&mc_fib);
        if(ret != CA_E_OK){
            return ret;
        }
        if(mc_fib.ldpid == port_id){
            *fib_bmp |= ((ca_uint64_t)1<<fib_idx);
        }
    }

    return CA_E_OK;
}

static ca_status_t __l2_vlan_id_item_add(__l2_vlan_id_item_t *pentry){

    __l2_vlan_id_item_t   *ptemp, *pre;
    ca_status_t ret = CA_E_OK;

    pre = NULL;
    pentry->next = NULL;

    ptemp = __g_l2_vlan_id_item.next;

    if (ptemp == NULL) {
        __g_l2_vlan_id_item.counter = 1;
        __g_l2_vlan_id_item.next = pentry;
        goto end;
    }
    while (ptemp) {
        pre = ptemp;
        ptemp = ptemp->next;
    }
    pre->next = pentry;
    __g_l2_vlan_id_item.counter++;

end:
    return ret;
}

static ca_status_t __l2_vlan_id_item_delete(ca_uint16_t vlan_id){

     __l2_vlan_id_item_t   *ptemp, *pre;
    ca_status_t ret = CA_E_OK;

    pre = NULL;
    ptemp = __g_l2_vlan_id_item.next;

     while (ptemp) {
         if (vlan_id == ptemp->vlan_id) {
             if (pre) {
                 pre->next = ptemp->next;
             } else {
                 __g_l2_vlan_id_item.next = ptemp->next;
             }
             pre = ptemp;
             ptemp = ptemp->next;
             //ca_free(pre);
             __g_l2_vlan_id_item.counter--;
             while (ptemp) {
                 ptemp = ptemp->next;
             }
             L2_DEBUG("__l2_vlan_id_item_delete() successfully \n");
             return ret;
         }
         pre = ptemp;
         ptemp = ptemp->next;
    }

    return ret;
}

static ca_boolean_t __l2_vlan_id_item_find(ca_uint16_t vlan_id, __l2_vlan_id_item_t **pentry){

    __l2_vlan_id_item_t   *ptemp, *pre;

    pre = NULL;
    ptemp   = __g_l2_vlan_id_item.next;
    while (ptemp) {
        if (vlan_id == ptemp->vlan_id) {
           *pentry = ptemp;
            return TRUE;
        }
        pre = ptemp;
        ptemp = ptemp->next;
    }

    return FALSE;
}


ca_status_t ca_l2_vlan_outer_tpid_add(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
)
{
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_l2_vlan_default_cfg_t       vlan_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint8_t                      tpid_bmp = 0;
    ca_status_t                     ret = CA_E_OK;

    CA_PORT_ID_CHECK(port_id);
	logic_port_id = PORT_ID(port_id);

    if(tpid_sel > __VLAN_TPID_NUM-1) {
        return CA_E_PARAM;
    }

    /* check if the tpid is set by ca_l2_vlan_tpid_set() */
    if(__vlan_outer_tpid[tpid_sel] == 0){
        ca_printf("this tpid index %d is not set by ca_l2_vlan_tpid_set() before\r\n", tpid_sel);
        return CA_E_PARAM;
    }

    /* get the vlan tag type of this port: S or C */
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    __IF_L2_VLAN_TPID_USER_DEFINE(__vlan_outer_tpid[tpid_sel]){        /* check tpid cmp s/c resource */
        memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
        ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);
        if (ret != CA_E_OK) {
            goto end;
        }

        if((ilpb_cfg.top_s_tpid_enable == 1) && (__vlan_tpid_cmp_s_used_bmp != 0) &&
            (__vlan_outer_tpid[tpid_sel] != vlan_cfg.cmp_tpid_svlan)){ /* double tag: S+S */
            /* some ports is using tpid_cmp_s, and tpid value is not the same as this port's tpid */
            ca_printf("no stpid resource: __vlan_tpid_cmp_s_used_bmp = 0x%x\r\n", __vlan_tpid_cmp_s_used_bmp);
            return CA_E_RESOURCE;

        }else if((ilpb_cfg.top_c_tpid_enable == 1) && (__vlan_tpid_cmp_c_used_bmp != 0) &&
            (__vlan_outer_tpid[tpid_sel] != vlan_cfg.cmp_tpid_cvlan)){
            ca_printf("no ctpid resource: __vlan_tpid_cmp_c_used_bmp = 0x%x\r\n", __vlan_tpid_cmp_c_used_bmp);
            return CA_E_RESOURCE;
        }

        /* configure TPID_CMP_S/C */
        if((ilpb_cfg.top_s_tpid_enable == 1) && (__vlan_tpid_cmp_s_used_bmp == 0)){ /* tpid_cmp_s is not used before */
            ret = __l2_vlan_tpid_cmp_value_set(device_id, __vlan_outer_tpid[tpid_sel], CA_L2_VLAN_TAG_TYPE_SVLAN);
            if (ret != CA_E_OK) {
                goto end;
            }
            __vlan_tpid_cmp_s_used_bmp |= ((ca_uint64_t)1<<logic_port_id);

        }else if((ilpb_cfg.top_c_tpid_enable == 1) && (__vlan_tpid_cmp_c_used_bmp == 0)){
            ret = __l2_vlan_tpid_cmp_value_set(device_id, __vlan_outer_tpid[tpid_sel], CA_L2_VLAN_TAG_TYPE_CVLAN);
            if (ret != CA_E_OK) {
                goto end;
            }
            __vlan_tpid_cmp_c_used_bmp |= ((ca_uint64_t)1<<logic_port_id);
        }
    }

    if(ilpb_cfg.top_s_tpid_enable == 1){ /* outer tpid is svlan tpid */
        __l2_vlan_tpid_value2bmp_add(__vlan_outer_tpid[tpid_sel], ilpb_cfg.s_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_SVLAN);

    }else if(ilpb_cfg.top_c_tpid_enable == 1){  /* outer tpid is cvlan tpid */
        __l2_vlan_tpid_value2bmp_add(__vlan_outer_tpid[tpid_sel], ilpb_cfg.c_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_CVLAN);
    }

end:
    return ret;
}

ca_status_t ca_l2_vlan_outer_tpid_delete(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
)
{
    aal_ilpb_cfg_t                  ilpb_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint8_t                      tpid_bmp = 0;
    ca_status_t                     ret = CA_E_OK;

    CA_PORT_ID_CHECK(port_id);
	logic_port_id = PORT_ID(port_id);

    if(tpid_sel > __VLAN_TPID_NUM-1) {
        return CA_E_PARAM;
    }

    /* check if the tpid is set by ca_l2_vlan_tpid_set() */
    if(__vlan_outer_tpid[tpid_sel] == 0){
        ca_printf("this tpid index %d is not set by ca_l2_vlan_tpid_set() before\r\n", tpid_sel);
        return CA_E_PARAM;
    }

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    if(ilpb_cfg.top_s_tpid_enable == 1){ /* outer tpid is svlan tpid */
        __l2_vlan_tpid_value2bmp_delete(__vlan_outer_tpid[tpid_sel], ilpb_cfg.s_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_SVLAN);
        if (ret != CA_E_OK) {
            goto end;
        }

    }else if(ilpb_cfg.top_c_tpid_enable == 1){  /* outer tpid is cvlan tpid */
        __l2_vlan_tpid_value2bmp_delete(__vlan_outer_tpid[tpid_sel], ilpb_cfg.c_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_CVLAN);
        if (ret != CA_E_OK) {
            goto end;
        }
    }

    __IF_L2_VLAN_TPID_USER_DEFINE(__vlan_outer_tpid[tpid_sel]){

        if(ilpb_cfg.top_s_tpid_enable == 1){
            if(__vlan_tpid_cmp_s_used_bmp == ((ca_uint64_t)1<<logic_port_id)){  /* only this use tpid_cmp_s */
                ret = __l2_vlan_tpid_cmp_value_set(device_id, 0, CA_L2_VLAN_TAG_TYPE_SVLAN);
                if (ret != CA_E_OK) {
                    goto end;
                }
                __vlan_tpid_cmp_s_used_bmp &= ~((ca_uint64_t)1<<logic_port_id); /* remove this port */
            }

        }else if(ilpb_cfg.top_c_tpid_enable == 1){
            if(__vlan_tpid_cmp_c_used_bmp == ((ca_uint64_t)1<<logic_port_id)){
                ret = __l2_vlan_tpid_cmp_value_set(device_id, 0, CA_L2_VLAN_TAG_TYPE_CVLAN);
                if (ret != CA_E_OK) {
                    goto end;
                }
                __vlan_tpid_cmp_c_used_bmp &= ~((ca_uint64_t)1<<logic_port_id);
            }
        }
    }

end:
    return ret;
}

ca_status_t ca_l2_vlan_inner_tpid_add(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
)
{
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_l2_vlan_default_cfg_t       vlan_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint8_t                      tpid_bmp = 0;
    ca_status_t                     ret = CA_E_OK;

    CA_PORT_ID_CHECK(port_id);
	logic_port_id = PORT_ID(port_id);

    if(tpid_sel > __VLAN_TPID_NUM-1) {
        return CA_E_PARAM;
    }

    /* check if the tpid is set by ca_l2_vlan_tpid_set() */
    if(__vlan_inner_tpid[tpid_sel] == 0){
        ca_printf("this tpid index %d is not set by ca_l2_vlan_tpid_set() before\r\n", tpid_sel);
        return CA_E_PARAM;
    }

    /* get the vlan tag type of this port: S or C */
    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    __IF_L2_VLAN_TPID_USER_DEFINE(__vlan_inner_tpid[tpid_sel]){
        /* check tpid cmp s/c resource */
        memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
        ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);
        if (ret != CA_E_OK) {
            goto end;
        }
        if(ilpb_cfg.inner_s_tpid_enable == 1 && __vlan_tpid_cmp_s_used_bmp != 0 &&
            __vlan_inner_tpid[tpid_sel] != vlan_cfg.cmp_tpid_svlan){
            /* some ports is using tpid_cmp_s, and tpid value is not the same as this port's tpid */
            ca_printf("no stpid resource: __vlan_tpid_cmp_s_used_bmp = 0x%x\r\n", __vlan_tpid_cmp_s_used_bmp);
            return CA_E_RESOURCE;

        }else if(ilpb_cfg.inner_c_tpid_enable == 1 && __vlan_tpid_cmp_c_used_bmp != 0 &&
            __vlan_inner_tpid[tpid_sel] != vlan_cfg.cmp_tpid_cvlan){
            ca_printf("no ctpid resource: __vlan_tpid_cmp_c_used_bmp = 0x%x\r\n", __vlan_tpid_cmp_c_used_bmp);
            return CA_E_RESOURCE;
        }

        /* configure TPID_CMP_S/C */
        if(ilpb_cfg.inner_s_tpid_enable == 1 && __vlan_tpid_cmp_s_used_bmp == 0){ /* tpid_cmp_s is not used before */
            ret = __l2_vlan_tpid_cmp_value_set(device_id, __vlan_inner_tpid[tpid_sel], CA_L2_VLAN_TAG_TYPE_SVLAN);
            if (ret != CA_E_OK) {
                goto end;
            }
            __vlan_tpid_cmp_s_used_bmp |= ((ca_uint64_t)1<<logic_port_id);

        }else if(ilpb_cfg.inner_c_tpid_enable == 1 && __vlan_tpid_cmp_c_used_bmp == 0){
            ret = __l2_vlan_tpid_cmp_value_set(device_id, __vlan_inner_tpid[tpid_sel], CA_L2_VLAN_TAG_TYPE_CVLAN);
            if (ret != CA_E_OK) {
                goto end;
            }
            __vlan_tpid_cmp_c_used_bmp |= ((ca_uint64_t)1<<logic_port_id);
        }
    }

    if(ilpb_cfg.inner_s_tpid_enable == 1){ /* inner tpid is svlan tpid */
        __l2_vlan_tpid_value2bmp_add(__vlan_inner_tpid[tpid_sel], ilpb_cfg.s_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_SVLAN);

    }else if(ilpb_cfg.inner_c_tpid_enable == 1){  /* inner tpid is cvlan tpid */
        __l2_vlan_tpid_value2bmp_add(__vlan_inner_tpid[tpid_sel], ilpb_cfg.c_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_CVLAN);
    }

end:
    return ret;
}

ca_status_t ca_l2_vlan_inner_tpid_delete(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
)
{
    aal_ilpb_cfg_t                  ilpb_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint8_t                      tpid_bmp = 0;
    ca_status_t                     ret = CA_E_OK;

    CA_PORT_ID_CHECK(port_id);
	logic_port_id = PORT_ID(port_id);

    if(tpid_sel > __VLAN_TPID_NUM-1) {
        return CA_E_PARAM;
    }

    /* check if the tpid is set by ca_l2_vlan_tpid_set() */
    if(__vlan_inner_tpid[tpid_sel] == 0){
        ca_printf("this tpid index %d is not set by ca_l2_vlan_tpid_set() before\r\n", tpid_sel);
        return CA_E_PARAM;
    }

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    if(ilpb_cfg.inner_s_tpid_enable == 1){ /* inner tpid is svlan tpid */
        __l2_vlan_tpid_value2bmp_delete(__vlan_inner_tpid[tpid_sel], ilpb_cfg.s_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_SVLAN);
        if (ret != CA_E_OK) {
            goto end;
        }

    }else if(ilpb_cfg.inner_c_tpid_enable == 1){  /* inner tpid is cvlan tpid */
        __l2_vlan_tpid_value2bmp_delete(__vlan_inner_tpid[tpid_sel], ilpb_cfg.c_tpid_match, &tpid_bmp);
        ret = __l2_vlan_tpid_match_bmp_set(device_id, logic_port_id, tpid_bmp, CA_L2_VLAN_TAG_TYPE_CVLAN);
        if (ret != CA_E_OK) {
            goto end;
        }
    }

    __IF_L2_VLAN_TPID_USER_DEFINE(__vlan_inner_tpid[tpid_sel]){

        if(ilpb_cfg.inner_s_tpid_enable == 1){
            if(__vlan_tpid_cmp_s_used_bmp == ((ca_uint64_t)1<<logic_port_id)){  /* only this use tpid_cmp_s */
                ret = __l2_vlan_tpid_cmp_value_set(device_id, 0, CA_L2_VLAN_TAG_TYPE_SVLAN);
                if (ret != CA_E_OK) {
                    goto end;
                }
                __vlan_tpid_cmp_s_used_bmp &= ~((ca_uint64_t)1<<logic_port_id); /* remove this port */
            }

        }else if(ilpb_cfg.inner_c_tpid_enable == 1){
            if(__vlan_tpid_cmp_c_used_bmp == (1<<logic_port_id)){
                ret = __l2_vlan_tpid_cmp_value_set(device_id, 0, CA_L2_VLAN_TAG_TYPE_CVLAN);
                if (ret != CA_E_OK) {
                    goto end;
                }
                __vlan_tpid_cmp_c_used_bmp &= ~((ca_uint64_t)1<<logic_port_id);
            }
        }
    }

end:
    return ret;
}

ca_status_t ca_l2_vlan_outer_tpid_list(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *tpid,
    CA_OUT ca_uint32_t  *tpid_nums
)
{
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_l2_vlan_default_cfg_t       vlan_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint8_t                      tpid_match_bmp = 0;
    ca_status_t                     ret = CA_E_OK;

    CA_PORT_ID_CHECK(port_id);

     if  ((tpid == NULL) || (tpid_nums == NULL)) {
         return CA_E_PARAM;
     }

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    logic_port_id = PORT_ID(port_id);

    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        return CA_E_ERROR;
    }

    if(ilpb_cfg.top_s_tpid_enable == 1){
        tpid_match_bmp = ilpb_cfg.s_tpid_match;
    }else{
        tpid_match_bmp = ilpb_cfg.c_tpid_match;
    }

    __l2_vlan_tpid_bmp2list(tpid_match_bmp, tpid, tpid_nums);

    if(tpid_match_bmp & (1 << AAL_VLAN_TPID_INDEX_USER_DEFINE)){
        memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
        ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);
        if (ret != CA_E_OK) {
            return ret;
        }
        if(ilpb_cfg.top_s_tpid_enable == 1){
            *(tpid + *tpid_nums) = vlan_cfg.cmp_tpid_svlan;
        }else{
            *(tpid + *tpid_nums) = vlan_cfg.cmp_tpid_cvlan;
        }
        *tpid_nums = *tpid_nums+1;
    }

    return CA_E_OK;
}

ca_status_t ca_l2_vlan_inner_tpid_list(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t *tpid,
    CA_OUT ca_uint32_t *tpid_nums
)
{
    aal_ilpb_cfg_t                  ilpb_cfg;
    aal_l2_vlan_default_cfg_t       vlan_cfg;
    ca_uint32_t                     logic_port_id;
    ca_uint8_t                      tpid_match_bmp = 0;
    ca_status_t                     ret = CA_E_OK;

    CA_PORT_ID_CHECK(port_id);

    if  ((tpid == NULL) || (tpid_nums == NULL)) {
        return CA_E_PARAM;
    }

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));
    logic_port_id = PORT_ID(port_id);

    ret = aal_port_ilpb_cfg_get(device_id, logic_port_id, &ilpb_cfg);
    if (ret != CA_E_OK) {
        return CA_E_ERROR;
    }

    if(ilpb_cfg.inner_s_tpid_enable == 1){
        tpid_match_bmp = ilpb_cfg.s_tpid_match;
    }else{
        tpid_match_bmp = ilpb_cfg.c_tpid_match;
    }

    __l2_vlan_tpid_bmp2list(tpid_match_bmp, tpid, tpid_nums);

    if(tpid_match_bmp & (1 << AAL_VLAN_TPID_INDEX_USER_DEFINE)){
        memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
        ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);
        if (ret != CA_E_OK) {
            return ret;
        }
        if(ilpb_cfg.inner_s_tpid_enable == 1){
            *(tpid + *tpid_nums) = vlan_cfg.cmp_tpid_svlan;
        }else{
            *(tpid + *tpid_nums) = vlan_cfg.cmp_tpid_cvlan;
        }
        *tpid_nums = *tpid_nums+1;
    }

    return CA_E_OK;
}

ca_status_t ca_l2_vlan_create(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t vid
)
{
    aal_l2_vlan_fib_map_t       vlan_fib_map;
    aal_l2_vlan_fib_map_mask_t  vlan_fib_map_mask;
    ca_status_t ret = CA_E_OK;



    if (vid > __MAX_VLAN_ID) {
        return CA_E_PARAM;
    }
    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));
    ret = aal_l2_vlan_fib_map_get(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, vid, &vlan_fib_map);

    if (ret != CA_E_OK) {
        goto end;
    }
    if (vlan_fib_map.valid) {
        ret = CA_E_EXISTS;
        goto end;
    }

    vlan_fib_map_mask.u32 = 0;
    vlan_fib_map_mask.s.valid = 1;
    vlan_fib_map_mask.s.fib_id = 1;
    vlan_fib_map.valid = 1;
    vlan_fib_map.fib_id = __l2_vlan_fib_id_wan_search();
    if(vlan_fib_map.fib_id >= __L2_VLAN_FIB_NUM){
        ret = CA_E_RESOURCE;
        goto end;
    }

    ret = aal_l2_vlan_fib_map_set(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, vid, vlan_fib_map_mask, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }
    ret = aal_l2_vlan_fib_map_set(device_id, AAL_L2_VLAN_TYPE_WAN_CVLAN, vid, vlan_fib_map_mask, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }

    vlan_fib_map.fib_id = __l2_vlan_fib_id_lan_search();
    if(vlan_fib_map.fib_id >= __L2_VLAN_FIB_NUM){
        ret = CA_E_RESOURCE;
        goto end;
    }

    ret = aal_l2_vlan_fib_map_set(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, vid, vlan_fib_map_mask, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }

end:
    return ret;
}

ca_status_t ca_l2_vlan_delete(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t vid
)
{
    aal_l2_vlan_fib_map_t       vlan_fib_map;
    aal_l2_vlan_fib_map_mask_t  vlan_fib_map_mask;
    aal_l2_vlan_fib_t           vlan_fib;
    aal_l2_vlan_fib_mask_t  vlan_fib_mask;
    ca_status_t ret = CA_E_OK;
    __l2_vlan_id_item_t         *vlan_id_item = NULL;
    ca_uint32_t temp_fib_id = 0;
    ca_uint8    i = 0, mc_group_id = 0;

    if (vid > __MAX_VLAN_ID) {
        return CA_E_PARAM;
    }

    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));

    ret = aal_l2_vlan_fib_map_get(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, vid, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }

    if (vlan_fib_map.valid == 0) {
       // printk("\n%s %d\n",__FILE__,__LINE__);
        ret = CA_E_NOT_FOUND;
        goto end;
    }

    temp_fib_id = vlan_fib_map.fib_id;

    ret = aal_l2_vlan_action_cfg_get(device_id,AAL_L2_VLAN_TYPE_LAN_VLAN,temp_fib_id,&vlan_fib);
    if (ret != CA_E_OK) {
        goto end;
    }


    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));
    memset(&vlan_fib, 0, sizeof(aal_l2_vlan_fib_t));
    vlan_fib_map_mask.u32 = 0;
    vlan_fib_map_mask.s.valid = 1;
    vlan_fib_map_mask.s.fib_id = 1;
    ret = aal_l2_vlan_fib_map_set(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, vid, vlan_fib_map_mask, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }
    ret = aal_l2_vlan_fib_map_set(device_id, AAL_L2_VLAN_TYPE_WAN_CVLAN, vid, vlan_fib_map_mask, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }
    ret = aal_l2_vlan_fib_map_set(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, vid, vlan_fib_map_mask, &vlan_fib_map);
    if (ret != CA_E_OK) {
        goto end;
    }

    vlan_fib_mask.u32 = CA_UINT32_INVALID;
    ret = aal_l2_vlan_action_cfg_set(device_id,AAL_L2_VLAN_TYPE_WAN_SVLAN,temp_fib_id,vlan_fib_mask,&vlan_fib);
    if (ret != CA_E_OK) {
        goto end;
    }
    ret = aal_l2_vlan_action_cfg_set(device_id,AAL_L2_VLAN_TYPE_WAN_CVLAN,temp_fib_id,vlan_fib_mask,&vlan_fib);
    if (ret != CA_E_OK) {
        goto end;
    }
    ret = aal_l2_vlan_action_cfg_set(device_id,AAL_L2_VLAN_TYPE_LAN_VLAN,temp_fib_id,vlan_fib_mask,&vlan_fib);
    if (ret != CA_E_OK) {
        goto end;
    }

    ret = __l2_vlan_fib_id_lan_release(temp_fib_id);
    if (ret != CA_E_OK) {
        goto end;
    }
    ret = __l2_vlan_fib_id_wan_release(temp_fib_id);
    if (ret != CA_E_OK) {
        goto end;
    }

    if(__l2_vlan_id_item_find(vid,&vlan_id_item) != TRUE){
        printk("\n%s %d\n",__FILE__,__LINE__);
        goto end;
    }
  //  printk("\n%s %d mc_fib_bmp 0x%llx\n",__FILE__,__LINE__,vlan_id_item->mc_fib_id_bmp);
    if(vlan_id_item->mc_group_wan_valid){
        mc_group_id = vlan_id_item->mc_group_id_wan;
        for(i = 0; i<__L2_VLAN_MC_FIB_NUM; i++){
            if((vlan_id_item->mc_fib_id_bmp_wan >> i) & 1){
               // printk("\n%s %d i %d\n",__FILE__,__LINE__, i);
                ret = aal_flooding_mc_fib_delete(mc_group_id,i);
                if(CA_E_OK != ret){
                   printk("\n%s %d, mc group id %d, i %d\n",__FILE__,__LINE__,mc_group_id, i);
                    goto end;
                }
                ret =  aal_flooding_fib_idx_free(i);
                if(CA_E_OK != ret){
                   printk("\n%s %d i %d\n",__FILE__,__LINE__, i);
                    goto end;
                }
            }
        }
        ret = aal_flooding_group_delete(mc_group_id);
        if (ret != CA_E_OK) {
            //  printk("\n%s %d mc group id %d\n",__FILE__,__LINE__, mc_group_id);
            goto end;
        }
    }
    if(vlan_id_item->mc_group_lan_valid){
        mc_group_id = vlan_id_item->mc_group_id_lan;
        for(i = 0; i<__L2_VLAN_MC_FIB_NUM; i++){
            if((vlan_id_item->mc_fib_id_bmp_lan >> i) & 1){
               // printk("\n%s %d i %d\n",__FILE__,__LINE__, i);
                ret = aal_flooding_mc_fib_delete(mc_group_id,i);
                if(CA_E_OK != ret){
                   printk("\n%s %d, mc group id %d, i %d\n",__FILE__,__LINE__,mc_group_id, i);
                    goto end;
                }
                ret =  aal_flooding_fib_idx_free(i);
                if(CA_E_OK != ret){
                   printk("\n%s %d i %d\n",__FILE__,__LINE__, i);
                    goto end;
                }
            }
        }
        ret = aal_flooding_group_delete(mc_group_id);
        if (ret != CA_E_OK) {
            //  printk("\n%s %d mc group id %d\n",__FILE__,__LINE__, mc_group_id);
            goto end;
        }
    }

    ret = __l2_vlan_id_item_delete(vid);
    if (ret != CA_E_OK) {
        goto end;
    }

    ca_free(vlan_id_item);
    vlan_id_item = NULL;

end:
    return ret;
}

ca_status_t ca_l2_vlan_delete_all(
    CA_IN ca_device_id_t device_id
)
{
    ca_uint16_t vlan;

    ca_status_t ret = CA_E_OK;

    for(vlan = 0; vlan<=__MAX_VLAN_ID; vlan++){
        ret = ca_l2_vlan_delete(device_id, vlan);
        if(CA_E_NOT_FOUND == ret){
            continue;
        }
        if(CA_E_OK!= ret){
            return ret;
        }
    }

    return CA_E_OK;
}

ca_status_t ca_l2_vlan_port_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_uint32_t  vid,
    CA_IN ca_uint8_t   member_count,
    CA_IN ca_port_id_t *member_ports,
    CA_IN ca_uint8_t   untagged_count,
    CA_IN ca_port_id_t *untagged_ports
)
{
    aal_l2_vlan_fib_mask_t vlan_fib_mask;
    aal_l2_vlan_fib_t     vlan_fib;
    aal_l2_vlan_fib_map_t       vlan_fib_map;
    aal_arb_mc_fib_mask_t           mc_fib_mask;
    aal_arb_mc_fib_t                mc_fib;

    ca_uint32_t           logic_port_id;
    ca_uint32_t             mapped_port_id;
    ca_uint32_t            fib_id           = 0;
    ca_int16_t              mc_fib_id        = 0;
    ca_uint16_t            temp_mem         = 0;
    ca_uint16_t            temp_untag       = 0;
    ca_uint16_t           memship_bitmap    = 0;
    ca_uint16_t           untag_bitmap      = 0;
    ca_uint64_t           mc_fib_idx_bmp_wan      = 0;
    ca_uint64_t           mc_fib_idx_bmp_lan      = 0;
    ca_int16_t            mc_group_id_wan       = 0;
    ca_int16_t            mc_group_id_lan       = 0;
    ca_uint16_t            i = 0;
    ca_boolean_t           find_flag = FALSE;

    ca_status_t             ret             = CA_E_OK;
    ca_uint8_t              wan_flag        = 0;
    ca_uint8_t              lan_flag        = 0;
    __l2_vlan_id_item_t    *vlan_id_item    = NULL ,*vlan_id_item_add = NULL;
    ca_uint64_t             flood_bmp_wan       = 0;
    ca_uint64_t             flood_bmp_lan       = 0;
    ca_uint64_t             temp_memship_bmp  = 0;
    ca_uint64_t             temp_untag_bmp    = 0;

    if ((vid > __MAX_VLAN_ID) || (NULL == member_ports) || (NULL == untagged_ports)) {
        return CA_E_PARAM;
    }

    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));
    memset(&vlan_fib, 0, sizeof(aal_l2_vlan_fib_t));
    memset(&mc_fib, 0, sizeof(aal_arb_mc_fib_t));

    for(i=0;i<member_count;i++){
        CA_PORT_ID_CHECK(*(member_ports + i));
        if(PORT_ID(*(member_ports+i)) == AAL_LPORT_ETH_WAN || PORT_ID(*(member_ports+i)) == AAL_LPORT_L3_WAN ){
            wan_flag = 1;
        }else{
            lan_flag = 1;
        }
    }

    L2_DEBUG("vlan_id is %d\n",vid);
        ret = __l2_vlan_id_item_find(vid,&vlan_id_item);
        if(ret == FALSE){
            L2_DEBUG("__l2_vlan_id_item_find nothing ,so create a new item\n");
            find_flag = FALSE;
        }else{
            printk("__l2_vlan_id_item_find one vlan_id item,so free it\n");
            printk("mc_fib_id_bmp_wan = 0x%llx\n",vlan_id_item->mc_fib_id_bmp_wan);
            printk("mc_fib_id_bmp_lan = 0x%llx\n",vlan_id_item->mc_fib_id_bmp_lan);
            for(i = 0; i<__L2_VLAN_MC_FIB_NUM; i++){
                if((vlan_id_item->mc_fib_id_bmp_wan >> i) & 0x1){

                    ret = aal_flooding_mc_fib_delete(vlan_id_item->mc_group_id_wan,i);
                    if(CA_E_OK != ret){
                        printk("%s %d i = %d\n",__FILE__,__LINE__,i);
                        goto end;
                    }
                    ret =  aal_flooding_fib_idx_free(i);
                    if(CA_E_OK != ret){
                        printk("%s %d i = %d\n",__FILE__,__LINE__,i);
                        goto end;
                    }
                }

                if((vlan_id_item->mc_fib_id_bmp_lan >> i) & 0x1){

                    ret = aal_flooding_mc_fib_delete(vlan_id_item->mc_group_id_lan,i);
                    if(CA_E_OK != ret){
                        printk("%s %d i = %d\n",__FILE__,__LINE__,i);
                        goto end;
                    }
                    ret =  aal_flooding_fib_idx_free(i);
                    if(CA_E_OK != ret){
                        printk("%s %d i = %d\n",__FILE__,__LINE__,i);
                        goto end;
                    }
                }
            }
            find_flag = TRUE;
        }

    if(1 == wan_flag){
        //WAN
        ret = aal_l2_vlan_fib_map_get(device_id,AAL_L2_VLAN_TYPE_WAN_SVLAN,vid, &vlan_fib_map);
        if(CA_E_OK != ret){
            L2_DEBUG("aal_l2_vlan_fib_map_get() failed\n");
            return ret;
        }
        if(vlan_fib_map.valid == FALSE){
            ret = CA_E_NOT_FOUND;
            L2_DEBUG("vlan_fib_map is invalid\n");
            return ret;
        }
        ret = aal_l2_vlan_action_cfg_get(device_id,AAL_L2_VLAN_TYPE_WAN_SVLAN,vlan_fib_map.fib_id,&vlan_fib);

        if(CA_E_OK != ret){
            return ret;
        }
        if(find_flag == TRUE){
            if(vlan_id_item->mc_group_wan_valid){
                mc_group_id_wan = vlan_id_item->mc_group_id_wan;
            }else{
                mc_group_id_wan = aal_flooding_group_create();
            }
        }else{
            mc_group_id_wan = aal_flooding_group_create();
        }

        for (temp_mem = 0; temp_mem < member_count; temp_mem++) {
            logic_port_id =  PORT_ID(*(member_ports + temp_mem));
            ret = aal_l2_vlan_port_memship_map_get(device_id,logic_port_id,&mapped_port_id);
            if (ret != CA_E_OK) {
                return ret;
            }
            memship_bitmap |= (1 << mapped_port_id);
            flood_bmp_wan |= (1 << logic_port_id);
            temp_memship_bmp |= (1 << logic_port_id);
        }

        for (temp_untag = 0; temp_untag < untagged_count; temp_untag++) {
            logic_port_id = PORT_ID(*(untagged_ports + temp_untag));
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
            ret = aal_l2_vlan_port_memship_map_get(__VLAN_DEVID_FOR_SATURN,logic_port_id,&mapped_port_id);
#else
            ret = aal_l2_vlan_port_memship_map_get(device_id,logic_port_id,&mapped_port_id);
#endif
            if (ret != CA_E_OK) {
                printk("%s %d ret %d\n",__FILE__,__LINE__,ret);
                return ret;
            }
            untag_bitmap |= (1 << mapped_port_id);
            temp_untag_bmp |= 1 << logic_port_id;
        }

        for(i = 0; i<__L2_VLAN_LOGIC_PORT_NUM; i++){
            if((flood_bmp_wan>> i)&1){
                mc_fib_id = aal_flooding_fib_idx_alloc();
                if(mc_fib_id<0){
                    ret = CA_E_RESOURCE;
                    printk("%s %d mc_fib_id = %d\n",__FILE__,__LINE__,mc_fib_id);
                    goto end;
                }
                mc_fib_idx_bmp_wan |= (ca_uint64_t)(((ca_uint64_t)1)<<mc_fib_id);
                ret = aal_flooding_mc_fib_add(mc_group_id_wan, mc_fib_id);
                if (ret != CA_E_OK) {
                    printk("%s %d mc_fib_id = %d\n",__FILE__,__LINE__,mc_fib_id);
                    goto end;
                }

                mc_fib_mask.u32 = 0;
                mc_fib.ldpid = i;
                mc_fib_mask.s.ldpid = 1;
                if((temp_untag_bmp>>i)&1){
                      mc_fib_mask.s.vlan_cmd = 1;
                      mc_fib.vlan_cmd = AAL_ARB_MC_VLAN_CMD_POP;
                }
#ifdef      MC_FIB_COS_CMD
                mc_fib_mask.s.cos_cmd = 1;
                mc_fib.cos_cmd = 1;
#endif
                L2_DEBUG("\t wan mc_group_id %d mc_fib_id %d\n", mc_group_id_wan, mc_fib_id);

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
                aal_arb_mc_fib_set(__VLAN_DEVID_FOR_SATURN,(ca_uint32_t)mc_fib_id,mc_fib_mask,&mc_fib);
#else
                aal_arb_mc_fib_set(device_id,(ca_uint32_t)mc_fib_id,mc_fib_mask,&mc_fib);
#endif
            }
        }


        vlan_fib_mask.u32 = 0;
        memset(&vlan_fib, 0, sizeof(aal_l2_vlan_fib_t));
        vlan_fib_mask.s.memship_bm      = 1;
        vlan_fib_mask.s.untag_bm        = 1;
        vlan_fib_mask.s.permit          = 1;
        vlan_fib_mask.s.stp_stat        = 1;
        vlan_fib_mask.s.vlan_group_id   = 1;

        vlan_fib.memship_bm             = memship_bitmap;
        vlan_fib.untag_bm               = untag_bitmap;
        vlan_fib.permit                 = TRUE;
        vlan_fib.stp_stat               = AAL_L2_VLAN_STP_STAT_FORWARD_AND_LEARN;
        vlan_fib.vlan_group_id          = mc_group_id_wan;

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        ret = aal_l2_vlan_action_cfg_set(__VLAN_DEVID_FOR_SATURN, AAL_L2_VLAN_TYPE_WAN_SVLAN, vlan_fib_map.fib_id, vlan_fib_mask, &vlan_fib);
#else
        ret = aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, vlan_fib_map.fib_id, vlan_fib_mask, &vlan_fib);
#endif
        if (ret != CA_E_OK) {
            printk("%s %d ret = %d \n",__FILE__,__LINE__,ret);
            goto end;
        }

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        ret = aal_l2_vlan_action_cfg_set(__VLAN_DEVID_FOR_SATURN, AAL_L2_VLAN_TYPE_WAN_CVLAN, vlan_fib_map.fib_id, vlan_fib_mask, &vlan_fib);
#else
        ret = aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_CVLAN, vlan_fib_map.fib_id, vlan_fib_mask, &vlan_fib);
#endif
        if (ret != CA_E_OK) {
            printk("%s %d ret = %d \n",__FILE__,__LINE__,ret);
            goto end;
        }

        L2_DEBUG("wan fib_id is %d,flood_bmp is 0x%x,memship_bmp is 0x%x,mem_port_bmp is 0x%x\n",
            fib_id,
            flood_bmp_wan,
            memship_bitmap,
            temp_memship_bmp);
        L2_DEBUG ("wan untag_bmp is 0x%x,untag_port_bmp is 0x%x,vlan_id is %d\n",
            untag_bitmap,
            temp_untag_bmp,
            vid);

    }

       //LAN
   if(1 == lan_flag){
       ret = aal_l2_vlan_fib_map_get(device_id,AAL_L2_VLAN_TYPE_LAN_VLAN,vid, &vlan_fib_map);
       if(CA_E_OK != ret){
           goto end;
       }
       if(vlan_fib_map.valid == FALSE){
           ret = CA_E_NOT_FOUND;
           printk("%s %d ret = %d \n",__FILE__,__LINE__,ret);
           goto end;
       }
       ret = aal_l2_vlan_action_cfg_get(device_id,AAL_L2_VLAN_TYPE_LAN_VLAN,vlan_fib_map.fib_id,&vlan_fib);
       if(CA_E_OK != ret){
           printk("%s %d ret = %d \n",__FILE__,__LINE__,ret);
            goto end;
       }

       if(find_flag == TRUE){
            if(vlan_id_item->mc_group_lan_valid){
                mc_group_id_lan = vlan_id_item->mc_group_id_lan;
            }else{
                mc_group_id_lan = aal_flooding_group_create();
            }
        }else{
                mc_group_id_lan = aal_flooding_group_create();
        }
        if(mc_group_id_lan < 0){
           ret = CA_E_NOT_FOUND;
           printk("%s %d ret = %d \n",__FILE__,__LINE__,ret);
           goto end;
       }
       for (temp_mem = 0; temp_mem < member_count; temp_mem++) {

           logic_port_id =  PORT_ID(*(member_ports + temp_mem));
           ret = aal_l2_vlan_port_memship_map_get(device_id,logic_port_id,&mapped_port_id);
           if (ret != CA_E_OK) {
               goto end;
           }
           memship_bitmap |= 1 << mapped_port_id;
           flood_bmp_lan  |= 1 << logic_port_id;
           temp_memship_bmp |= 1 << logic_port_id;
       }

        for (temp_untag = 0; temp_untag < untagged_count; temp_untag++) {
           CA_PORT_ID_CHECK(*(untagged_ports + temp_untag));
           logic_port_id = PORT_ID(*(untagged_ports + temp_untag));
           ret = aal_l2_vlan_port_memship_map_get(device_id,logic_port_id,&mapped_port_id);
           if (ret != CA_E_OK) {
               goto end;
           }

           untag_bitmap |= (1 << mapped_port_id);
           temp_untag_bmp |= 1 << logic_port_id;
       }

    // if(find_flag == 1){
       for(i = 0; i<__L2_VLAN_LOGIC_PORT_NUM; i++){
           if((flood_bmp_lan >> i)&1){
               mc_fib_id = aal_flooding_fib_idx_alloc();
               if(mc_fib_id<0){
                   ret = CA_E_RESOURCE;
                   goto end;
               }
               mc_fib_idx_bmp_lan |= (ca_uint64_t)(((ca_uint64_t)1)<<mc_fib_id);
               ret = aal_flooding_mc_fib_add(mc_group_id_lan, mc_fib_id);
               if (ret != CA_E_OK) {
                   goto end;
               }

               mc_fib_mask.u32 = 0;
               mc_fib.ldpid = i;
               mc_fib_mask.s.ldpid = 1;
               if((temp_untag_bmp>>i)&1){
                     mc_fib_mask.s.vlan_cmd = 1;
                     mc_fib.vlan_cmd = AAL_ARB_MC_VLAN_CMD_POP;
               }
#ifdef      MC_FIB_COS_CMD
            mc_fib_mask.s.cos_cmd = 1;
            mc_fib.cos_cmd = 1;
#endif
                L2_DEBUG("\t lan mc_group_id %d mc_fib_id %d\n", mc_group_id_lan, mc_fib_id);

               aal_arb_mc_fib_set(device_id,(ca_uint32_t)mc_fib_id,mc_fib_mask,&mc_fib);
           }
       }


        vlan_fib_mask.u32 = 0;
        memset(&vlan_fib, 0, sizeof(aal_l2_vlan_fib_t));
        vlan_fib_mask.s.memship_bm      = 1;
        vlan_fib_mask.s.untag_bm        = 1;
        vlan_fib_mask.s.permit          = 1;
        vlan_fib_mask.s.stp_stat        = 1;
        vlan_fib_mask.s.vlan_group_id   = 1;

        vlan_fib.memship_bm             = memship_bitmap;
        vlan_fib.untag_bm               = untag_bitmap;
        vlan_fib.permit                 = TRUE;
        vlan_fib.stp_stat               = AAL_L2_VLAN_STP_STAT_FORWARD_AND_LEARN;
        vlan_fib.vlan_group_id          = mc_group_id_lan;

        ret = aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, vlan_fib_map.fib_id, vlan_fib_mask, &vlan_fib);
        if (ret != CA_E_OK) {
              goto end;
        }
    }
   if(find_flag == TRUE){
        __l2_vlan_id_item_delete(vid);
        ca_free(vlan_id_item);
        vlan_id_item = NULL;
    }
    vlan_id_item_add = (__l2_vlan_id_item_t *)ca_malloc(sizeof(__l2_vlan_id_item_t));
    if(NULL == vlan_id_item_add){
        ret  = CA_E_RESOURCE;
        L2_DEBUG("ca_malloc() failed\n");
        goto end;
    }


    vlan_id_item_add->flood_bmp_wan = flood_bmp_wan;
    vlan_id_item_add->flood_bmp_lan = flood_bmp_lan;
    vlan_id_item_add->mc_fib_id_bmp_wan = mc_fib_idx_bmp_wan;
    vlan_id_item_add->mc_fib_id_bmp_lan = mc_fib_idx_bmp_lan;

    vlan_id_item_add->memship_bmp = memship_bitmap;
    vlan_id_item_add->mem_port_bmp = temp_memship_bmp;
    vlan_id_item_add->untag_bmp = untag_bitmap;
    vlan_id_item_add->untag_port_bmp =temp_untag_bmp;
    vlan_id_item_add->vlan_id = vid;
    vlan_id_item_add->fib_id  = fib_id;
    if(wan_flag == 1){
        vlan_id_item_add->mc_group_id_wan = mc_group_id_wan;
        vlan_id_item_add->mc_group_wan_valid = TRUE;
    }
    if(lan_flag == 1){
        vlan_id_item_add->mc_group_id_lan = mc_group_id_lan;
        vlan_id_item_add->mc_group_lan_valid = TRUE;
    }

    ret = __l2_vlan_id_item_add(vlan_id_item_add);
    if(CA_E_OK != ret){
        L2_DEBUG("__l2_vlan_id_item_add failed\n");
        goto end;
    }

    L2_DEBUG("wan fib_id is %d,flood_bmp_wan is 0x%llx,memship_bmp is 0x%x,mem_port_bmp is 0x%x mc_fib_bmp is 0x%llx\n",
        vlan_id_item_add->fib_id,
        vlan_id_item_add->flood_bmp_wan,
        vlan_id_item_add->memship_bmp,
        vlan_id_item_add->mem_port_bmp,
        vlan_id_item_add->mc_fib_id_bmp_wan);
    L2_DEBUG("lan fib_id is %d,flood_bmp_lan is 0x%llx,memship_bmp is 0x%x,mem_port_bmp is 0x%x mc_fib_bmp is 0x%llx\n",
        vlan_id_item_add->fib_id,
        vlan_id_item_add->flood_bmp_lan,
        vlan_id_item_add->memship_bmp,
        vlan_id_item_add->mem_port_bmp,
        vlan_id_item_add->mc_fib_id_bmp_lan);

    L2_DEBUG ("lan untag_bmp is 0x%x,untag_port_bmp is 0x%x,vlan_id is %d\n",
        vlan_id_item_add->untag_bmp,
        vlan_id_item_add->untag_port_bmp,
        vlan_id_item_add->vlan_id);

end:

    return ret;
}

ca_status_t ca_l2_vlan_port_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint32_t  vid,
    CA_OUT ca_uint8_t   *member_count,
    CA_OUT ca_port_id_t *member_ports,
    CA_OUT ca_uint8_t   *untagged_count,
    CA_OUT ca_port_id_t * untagged_ports
)
{
    ca_status_t                 ret = CA_E_OK;
    ca_uint8_t                 temp_mem_count = 0;
    ca_uint8_t                 temp_untag_count = 0;
    ca_port_id_t               *temp_mem_port = member_ports;
    ca_port_id_t               *temp_untag_port = untagged_ports;
    ca_uint8_t                  i = 0;
    __l2_vlan_id_item_t        *vlan_id_item = NULL;
    ca_boolean_t                find_flag = FALSE;

    if ((vid > __MAX_VLAN_ID) || (NULL == member_count) || (NULL == member_ports) ||
            (NULL == untagged_count) || (NULL == untagged_ports)) {
        return CA_E_PARAM;
    }

    find_flag = __l2_vlan_id_item_find(vid,&vlan_id_item);
    if(TRUE != find_flag){
        *member_count = 0;
        *member_ports = 0;
        *untagged_count = 0;
        *untagged_ports = 0;
        L2_DEBUG("__l2_vlan_id_item_find for this VLAN id failed ,so return with 0 count and 0 ports \n");
        return CA_E_OK;
    }

    L2_DEBUG("fib_id is %d,flood_bmp_wan is 0x%llx,flood_bmp_lan is 0x%llx,memship_bmp is 0x%x,mem_port_bmp is 0x%x\n",
        vlan_id_item->fib_id,
        vlan_id_item->flood_bmp_wan,
        vlan_id_item->flood_bmp_lan,
        vlan_id_item->memship_bmp,
        vlan_id_item->mem_port_bmp);
    L2_DEBUG ("untag_bmp is 0x%x,untag_port_bmp is 0x%x,vlan_id is %d,vid is %d\n",
        vlan_id_item->untag_bmp,
        vlan_id_item->untag_port_bmp,
        vlan_id_item->vlan_id,
        vid);

    for (i = 0; i <__L2_VLAN_LOGIC_PORT_NUM; i++) {
        if (((vlan_id_item->mem_port_bmp)>>i)&1) {
            temp_mem_count ++;
            if((i >= CA_PORT_ID_NI0) && (i <= 7)){
                L2_DEBUG("From PORT_ID_NI to PORT_ID_NI7 \n ");
                *temp_mem_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,i);
            }else if((i == CA_PORT_ID_CPU0) || (i == CA_PORT_ID_CPU1 )||((i >= CA_PORT_ID_CPU3) && (i <= CA_PORT_ID_CPU7))){
                L2_DEBUG("From PORT_ID_CPU0 to PORT_ID_CPU7\n");
                *temp_mem_port = CA_PORT_ID(CA_PORT_TYPE_CPU,i);
            }else if((i == CA_PORT_ID_L3_WAN ) || (i == CA_PORT_ID_L3_LAN)){
                  L2_DEBUG("PORT_ID_L3_WAN/LAN\n");
                 *temp_mem_port = CA_PORT_ID(CA_PORT_TYPE_L2RP,i);
            }else{
                L2_DEBUG("Default return port id");
               *temp_mem_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,i);
            }
            temp_mem_port++;
        }
        if (((vlan_id_item->untag_port_bmp)>>i)&1) {
            temp_untag_count ++;
            if(i >= CA_PORT_ID_NI0 && i <= CA_PORT_ID_NI7){
                L2_DEBUG("From PORT_ID_NI to PORT_ID_NI7 \n ");
                *temp_untag_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,i);
            }else if(i == CA_PORT_ID_CPU0 || i == CA_PORT_ID_CPU1 ||(i >= CA_PORT_ID_CPU3 && i <= CA_PORT_ID_CPU7)){
                L2_DEBUG("From PORT_ID_CPU0 to PORT_ID_CPU7\n");
                *temp_untag_port = CA_PORT_ID(CA_PORT_TYPE_CPU,i);
            }else if(i == CA_PORT_ID_L3_WAN || i == CA_PORT_ID_L3_LAN){
                L2_DEBUG("PORT_ID_L3_WAN/LAN\n");
                 *temp_untag_port = CA_PORT_ID(CA_PORT_TYPE_L2RP,i);
            }else{
                L2_DEBUG("Default return port id");
               *temp_untag_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,i);
            }
            temp_untag_port++;
        }
    }
    *member_count = temp_mem_count;
    *untagged_count = temp_untag_count;
    L2_DEBUG("member_count is %d, untagge_count is %d\n",temp_mem_count,temp_untag_count);

    return ret;
}

ca_status_t ca_l2_vlan_iterate (
    CA_IN 		ca_device_id_t					device_id,
    CA_IN_OUT 	ca_iterator_t				*p_return_entry)
{
    ca_uint32_t  i = 0,j = 0;
    ca_uint8_t   member_count = 0;
    ca_uint8_t   untagged_count = 0;
    ca_vlan_iterate_entry_t local_vlan_entry;
    __l2_vlan_id_item_t *ptemp = NULL;

    CA_ASSERT_RET(p_return_entry != NULL && p_return_entry->p_entry_data != NULL, CA_E_PARAM);
    CA_ASSERT_RET(p_return_entry->entry_count, CA_E_PARAM);
    CA_ASSERT_RET(p_return_entry->entry_count * sizeof(ca_vlan_iterate_entry_t) <= p_return_entry->user_buffer_size, CA_E_PARAM);

    memset(&local_vlan_entry, 0, sizeof(ca_vlan_iterate_entry_t));

     if(NULL == p_return_entry->p_prev_handle){
        ptemp = __g_l2_vlan_id_item.next;
     }else{
        ptemp =  p_return_entry->p_prev_handle;
        if(NULL == ptemp->next)
            return CA_E_UNAVAIL;
     }

    while(ptemp&&(i < p_return_entry->entry_count)){

        for(j=0;j<__L2_VLAN_LOGIC_PORT_NUM;j++){
            if((ptemp->mem_port_bmp>>j)&0x1){
                if((j >= CA_PORT_ID_NI0)&& (j <= 7)){
                    local_vlan_entry.member_ports[member_count] = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,j);
                }else if((i == CA_PORT_ID_CPU0) || (i == CA_PORT_ID_CPU1 )||((i >= CA_PORT_ID_CPU3) && (i <= CA_PORT_ID_CPU7))){
                    local_vlan_entry.member_ports[member_count] = CA_PORT_ID(CA_PORT_TYPE_CPU,j);
                }else if((i == CA_PORT_ID_L3_WAN ) || (i == CA_PORT_ID_L3_LAN)){
                    local_vlan_entry.member_ports[member_count] = CA_PORT_ID(CA_PORT_TYPE_L2RP,j);
                }else{
                    local_vlan_entry.member_ports[member_count] = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,j);
                }
                member_count++;
            }
        }

        for(j=0;j<__L2_VLAN_LOGIC_PORT_NUM;j++){
            if((ptemp->untag_port_bmp>>j)&0x1){
                if((j >= CA_PORT_ID_NI0)&& (j <= 7)){
                    local_vlan_entry.untagged_ports[untagged_count] = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,j);
                }else if((i == CA_PORT_ID_CPU0) || (i == CA_PORT_ID_CPU1 )||((i >= CA_PORT_ID_CPU3) && (i <= CA_PORT_ID_CPU7))){
                    local_vlan_entry.untagged_ports[untagged_count] = CA_PORT_ID(CA_PORT_TYPE_CPU,j);
                }else if((i == CA_PORT_ID_L3_WAN ) || (i == CA_PORT_ID_L3_LAN)){
                    local_vlan_entry.untagged_ports[untagged_count] = CA_PORT_ID(CA_PORT_TYPE_L2RP,j);
                }else{
                    local_vlan_entry.untagged_ports[untagged_count] = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,j);
                }
                untagged_count++;
            }
        }
        local_vlan_entry.vid = ptemp->vlan_id;
        local_vlan_entry.member_count = member_count;
        local_vlan_entry.untagged_count = untagged_count;
        memcpy(p_return_entry->p_entry_data + i*sizeof(ca_vlan_iterate_entry_t),\
            &local_vlan_entry,sizeof(ca_vlan_iterate_entry_t));
        p_return_entry->p_prev_handle = ptemp;
        p_return_entry->entry_index[i] = ptemp->fib_id;
        ptemp = ptemp->next;
        i++;
    }

    p_return_entry->entry_count = i;

    if(0 == i){
        p_return_entry->p_prev_handle = NULL;
        return CA_E_UNAVAIL;
    }else{
        return CA_E_OK;
    }
}

ca_status_t ca_l2_vlan_ingress_default_action_set(
    CA_IN ca_device_id_t       device_id,
    CA_IN ca_port_id_t      port_id,
    CA_IN ca_vlan_action_t *untag_action,
    CA_IN ca_vlan_action_t *single_tag_action,
    CA_IN ca_vlan_action_t *double_tag_action
)
{
    aal_l2_cls_l2rl2r_key_t cls_key;
    aal_l2_cls_fib_t        cls_fib;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_fib_mask_t        fib_mask;
    ca_uint16_t             logic_port_id;
    ca_status_t             ret = CA_E_OK;
    ca_uint16_t             entry_id;
    ca_vlan_port_control_t   port_control;
    if ((NULL == untag_action) || (NULL == single_tag_action) || (NULL == double_tag_action)) {
        return CA_E_PARAM;
    }

    CA_PORT_ID_CHECK(port_id);

    ret = ca_l2_vlan_port_control_get(device_id,port_id,&port_control);
    if(CA_E_OK != ret){
        return ret;
    }

	logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    if (untag_action->outer_vlan_cmd > CA_L2_VLAN_TAG_ACTION_PUSH) {
        return CA_E_PARAM;
    }
    if (untag_action->inner_vlan_cmd > CA_L2_VLAN_TAG_ACTION_PUSH) {
        return CA_E_PARAM;
    }
    if (single_tag_action->inner_vlan_cmd > CA_L2_VLAN_TAG_ACTION_PUSH) {
        return CA_E_PARAM;
    }


    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    cls_mask.u64 = 0;

    cls_key.valid = 1;
   // cls_key.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
   // cls_key.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    //cls_key.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    cls_mask.s.valid       = 1;
    //cls_mask.s.entry_type  = 1;
    //cls_mask.s.rule_type_0 = 1;
    //cls_mask.s.rule_type_1 = 1;

    cls_key.vlan_number_vld_0 = 1;
    cls_key.vlan_number_0 = 0;

    cls_key.vlan_number_vld_1= 1;
    cls_key.vlan_number_1 = 1;
    cls_mask.s.vlan_number_vld_0   = 1;
    cls_mask.s.vlan_number_0       = 1;
    cls_mask.s.vlan_number_vld_1   = 1;
    cls_mask.s.vlan_number_1       = 1;

    cls_key.ethernet_type_vld_0     = 0;
    cls_key.ethernet_type_vld_1     = 0;
    cls_key.l4_port_field_ctrl_0    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.l4_port_field_ctrl_1    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.mac_field_ctrl_0        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.mac_field_ctrl_1        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.vlan_field_ctrl_0       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.vlan_field_ctrl_1       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.dot1p_ctrl_0            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_key.dot1p_ctrl_1            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_mask.s.ethernet_type_vld_0     = 1 ;
    cls_mask.s.ethernet_type_vld_1     = 1 ;
    cls_mask.s.l4_port_field_ctrl_0    = 1 ;
    cls_mask.s.l4_port_field_ctrl_1    = 1 ;
    cls_mask.s.mac_field_ctrl_0        = 1 ;
    cls_mask.s.mac_field_ctrl_1        = 1 ;
    cls_mask.s.vlan_field_ctrl_0       = 1 ;
    cls_mask.s.vlan_field_ctrl_1       = 1 ;
    cls_mask.s.dot1p_ctrl_0            = 1 ;
    cls_mask.s.dot1p_ctrl_1            = 1 ;

    entry_id = logic_port_id  * __INGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_UTAG_STAG_ENTRY_ID;
    ret = aal_l2_cls_l2rl2r_key_set(device_id, entry_id, cls_mask, &cls_key);
    if (ret != CA_E_OK) {
        goto end;
    }
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    fib_mask.u64 = 0;

    fib_mask.s.top_cmd_valid_0 = 1;
    cls_fib.top_cmd_valid_0 = TRUE;
    fib_mask.s.top_vlan_cmd_0 = 1;
    cls_fib.top_vlan_cmd_0 = untag_action->outer_vlan_cmd;
    if(untag_action->flow_id != CA_UINT16_INVALID){
        fib_mask.s.flowid_0 = 1;
        cls_fib.flowid_0 = untag_action->flow_id;
        fib_mask.s.mark_ena_0 = 1;
        cls_fib.mark_ena_0 = TRUE;
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
        if(untag_action->flow_id < 128){
            cls_fib.dp_value_0 = 7;
            cls_fib.dp_valid_0 = TRUE;
            cls_fib.cos_valid_0 = TRUE;
            cls_fib.cos_0 = ((untag_action->flow_id)&7);
            fib_mask.s.dp_value_0 = 1;
            fib_mask.s.dp_valid_0 = 1;
            fib_mask.s.cos_valid_0 = 1;
            fib_mask.s.cos_0 = 1;
        }
#endif
    }else{
        fib_mask.s.flowid_0 = 1;
         cls_fib.flowid_0 = 0;
         fib_mask.s.mark_ena_0 = 1;
         cls_fib.mark_ena_0 = FALSE;
    }

    fib_mask.s.top_sc_ind_0 = 1;
    cls_fib.top_sc_ind_0 = port_control.outer_tpid;
    fib_mask.s.inner_sc_ind_0 = 1;
    cls_fib.inner_sc_ind_0 = port_control.inner_tpid;

    if (untag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) {
        fib_mask.s.top_vlanid_0 = 1;
        cls_fib.top_vlanid_0 = untag_action->new_outer_vlan;
        fib_mask.s.dot1p_valid_0 = 1;
        cls_fib.dot1p_valid_0 = TRUE;
        fib_mask.s.dot1p_0 = 1;
        cls_fib.dot1p_0       = untag_action->new_outer_pri;
    }else
    {
        fib_mask.s.top_vlanid_0 = 1;
        cls_fib.top_vlanid_0 = 0;
        fib_mask.s.dot1p_0 = 1;
        cls_fib.dot1p_0  = 0;

	}
    fib_mask.s.inner_cmd_valid_0 = 1;
    cls_fib.inner_cmd_valid_0 = TRUE;
    fib_mask.s.inner_vlan_cmd_0 = 1;
    cls_fib.inner_vlan_cmd_0 = untag_action->inner_vlan_cmd;
    if (untag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) {
        fib_mask.s.inner_vlanid_0 = 1;
        cls_fib.inner_vlanid_0 = untag_action->new_inner_vlan;
    }else{
        fib_mask.s.inner_vlanid_0 = 1;
        cls_fib.inner_vlanid_0 = 0;

	}
    fib_mask.s.top_cmd_valid_1 = 1;
    cls_fib.top_cmd_valid_1 = TRUE;
    fib_mask.s.top_vlan_cmd_1 = 1;
    cls_fib.top_vlan_cmd_1 = single_tag_action->outer_vlan_cmd;
    if(single_tag_action->flow_id != CA_UINT16_INVALID){
        fib_mask.s.flowid_1 = 1;
        cls_fib.flowid_1 = single_tag_action->flow_id;
        fib_mask.s.mark_ena_1 = 1;
        cls_fib.mark_ena_1 = TRUE;
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
        if(single_tag_action->flow_id < 128){
            cls_fib.dp_value_1 = 7;
            cls_fib.dp_valid_1 = TRUE;
            cls_fib.cos_valid_1 = TRUE;
            cls_fib.cos_1 = ((single_tag_action->flow_id)&7);
            fib_mask.s.dp_value_1 = 1;
            fib_mask.s.dp_valid_1 = 1;
            fib_mask.s.cos_valid_1 = 1;
            fib_mask.s.cos_1 = 1;
        }
#endif
    }else{
        fib_mask.s.flowid_1 = 1;
         cls_fib.flowid_1 = 0;
         fib_mask.s.mark_ena_1 = 1;
         cls_fib.mark_ena_1 = FALSE;
    }

    if ((single_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) || (single_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        fib_mask.s.top_vlanid_1 = 1;
        cls_fib.top_vlanid_1 = single_tag_action->new_outer_vlan;
        fib_mask.s.dot1p_valid_1 = 1;
        cls_fib.dot1p_valid_1 = TRUE;
        fib_mask.s.dot1p_1 = 1;
        cls_fib.dot1p_1       = single_tag_action->new_outer_pri;
    }else{
		fib_mask.s.top_vlanid_1 = 1;
		cls_fib.top_vlanid_1 = 0;
		fib_mask.s.dot1p_1 = 1;
		cls_fib.dot1p_1  = 0;
	}

    fib_mask.s.inner_cmd_valid_1 = 1;
    cls_fib.inner_cmd_valid_1 = TRUE;
    fib_mask.s.inner_vlan_cmd_1 = 1;
    cls_fib.inner_vlan_cmd_1 = single_tag_action->inner_vlan_cmd;
    if (single_tag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) {
        fib_mask.s.inner_vlanid_1 = 1;
        cls_fib.inner_vlanid_1 = single_tag_action->new_inner_vlan;
    }else{
        fib_mask.s.inner_vlanid_1 = 1;
        cls_fib.inner_vlanid_1 = 0;

	}
     fib_mask.s.spt_mode_0 =1 ;
     fib_mask.s.spt_mode_1 =1 ;
     cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
     cls_fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
     fib_mask.s.top_sc_ind_1      = 1;
     cls_fib.top_sc_ind_1         = port_control.outer_tpid;
     fib_mask.s.inner_sc_ind_1    = 1;
     cls_fib.inner_sc_ind_1       = port_control.inner_tpid;

    ret = aal_l2_cls_fib_set(device_id, entry_id,fib_mask,&cls_fib);
    if (ret != CA_E_OK) {
        goto end;
    }
    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    cls_mask.u64 = 0;
    fib_mask.u64 = 0;
    cls_key.valid = 1;
    //cls_key.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
    //cls_key.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    //cls_key.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    cls_mask.s.valid       = 1;
    //cls_mask.s.entry_type  = 1;
    //cls_mask.s.rule_type_0 = 1;
    //cls_mask.s.rule_type_1 = 1;
    cls_key.vlan_number_vld_0 = 1;
    cls_key.vlan_number_0 = 2;
    cls_mask.s.vlan_number_vld_0   = 1;
    cls_mask.s.vlan_number_0       = 1;


    cls_key.ethernet_type_vld_0     = 0;
    cls_key.ethernet_type_vld_1     = 0;
    cls_key.l4_port_field_ctrl_0    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.l4_port_field_ctrl_1    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.mac_field_ctrl_0        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.mac_field_ctrl_1        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.vlan_field_ctrl_0       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.vlan_field_ctrl_1       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.dot1p_ctrl_0            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_key.dot1p_ctrl_1            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_mask.s.ethernet_type_vld_0     = 1 ;
    cls_mask.s.ethernet_type_vld_1     = 1 ;
    cls_mask.s.l4_port_field_ctrl_0    = 1 ;
    cls_mask.s.l4_port_field_ctrl_1    = 1 ;
    cls_mask.s.mac_field_ctrl_0        = 1 ;
    cls_mask.s.mac_field_ctrl_1        = 1 ;
    cls_mask.s.vlan_field_ctrl_0       = 1 ;
    cls_mask.s.vlan_field_ctrl_1       = 1 ;
    cls_mask.s.dot1p_ctrl_0            = 1 ;
    cls_mask.s.dot1p_ctrl_1            = 1 ;
    entry_id = logic_port_id * __INGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_DTAG_ENTRY_ID;
    ret = aal_l2_cls_l2rl2r_key_set(device_id, entry_id, cls_mask,&cls_key);
    if (ret != CA_E_OK) {
        goto end;
    }

    fib_mask.s.top_cmd_valid_0 = 1;
    cls_fib.top_cmd_valid_0 = TRUE;
    fib_mask.s.top_vlan_cmd_0 = 1;
    cls_fib.top_vlan_cmd_0 = double_tag_action->outer_vlan_cmd;

    if ((double_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
            (double_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        fib_mask.s.top_vlanid_0 = 1;
        cls_fib.top_vlanid_0 = double_tag_action->new_outer_vlan;
        fib_mask.s.dot1p_valid_0 = 1;
        cls_fib.dot1p_valid_0 = TRUE;
        fib_mask.s.dot1p_0 = 1;
        cls_fib.dot1p_0       = double_tag_action->new_outer_pri;
    }else{
		fib_mask.s.top_vlanid_0 = 1;
		cls_fib.top_vlanid_0 = 0;
		fib_mask.s.dot1p_0 = 1;
		cls_fib.dot1p_0  = 0;
	}
    fib_mask.s.top_sc_ind_0 = 1;
    cls_fib.top_sc_ind_0 = port_control.outer_tpid;
    fib_mask.s.inner_sc_ind_0 = 1;
    cls_fib.inner_sc_ind_0 = port_control.inner_tpid;

    fib_mask.s.inner_cmd_valid_0 = 1;
    cls_fib.inner_cmd_valid_0 = TRUE;
    fib_mask.s.inner_vlan_cmd_0 = 1;
    cls_fib.inner_vlan_cmd_0 = double_tag_action->inner_vlan_cmd;
    if ((double_tag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
            (double_tag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        fib_mask.s.inner_vlanid_0 = 1;
        cls_fib.inner_vlanid_0 = double_tag_action->new_inner_vlan;
    }else{
		fib_mask.s.inner_vlanid_0 = 1;
        cls_fib.inner_vlanid_0 = 0;
	}

    fib_mask.s.spt_mode_0 =1 ;
    cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;

    if(double_tag_action->flow_id != CA_UINT16_INVALID){
        fib_mask.s.flowid_0 = 1;
        cls_fib.flowid_0 = double_tag_action->flow_id;
        fib_mask.s.mark_ena_0 = 1;
        cls_fib.mark_ena_0 = TRUE;
#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
        if(double_tag_action->flow_id < 128){
            cls_fib.dp_value_0 = 7;
            cls_fib.dp_valid_0 = TRUE;
            cls_fib.cos_valid_0 = TRUE;
            cls_fib.cos_0 = ((double_tag_action->flow_id)&7);
            fib_mask.s.dp_value_0 = 1;
            fib_mask.s.dp_valid_0 = 1;
            fib_mask.s.cos_valid_0 = 1;
            fib_mask.s.cos_0 = 1;
        }
#endif
        }else{
        fib_mask.s.flowid_0 = 1;
         cls_fib.flowid_0 = 0;
         fib_mask.s.mark_ena_0 = 1;
         cls_fib.mark_ena_0 = FALSE;
    }

    ret = aal_l2_cls_fib_set(device_id, entry_id, fib_mask,&cls_fib);
end:
    return ret;
}

ca_status_t ca_l2_vlan_ingress_default_action_get(
    CA_IN  ca_device_id_t      device_id,
    CA_IN  ca_port_id_t     port_id,
    CA_OUT ca_vlan_action_t *untag_action,
    CA_OUT ca_vlan_action_t *single_tag_action,
    CA_OUT ca_vlan_action_t *double_tag_action
)
{
    aal_l2_cls_fib_t        cls_fib;
    ca_status_t             ret = CA_E_OK;
    ca_uint16_t             entry_id;
    ca_uint16_t             logic_port_id;
    if ( (NULL == untag_action) || (NULL == single_tag_action) || (NULL == double_tag_action)) {
        return CA_E_PARAM;
    }

    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    entry_id = logic_port_id* __INGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_UTAG_STAG_ENTRY_ID;
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    ret = aal_l2_cls_fib_get(device_id, entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        goto end;
    }
    if (cls_fib.top_cmd_valid_0) {
        untag_action->outer_vlan_cmd = cls_fib.top_vlan_cmd_0;
        untag_action->new_outer_vlan = cls_fib.top_vlanid_0;
    }
    if (cls_fib.dot1p_valid_0) {
        untag_action->new_outer_pri = cls_fib.dot1p_0;
    }
    if (cls_fib.inner_cmd_valid_0) {
        untag_action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_0;
        untag_action->new_inner_vlan = cls_fib.inner_vlanid_0;
    }
    if(cls_fib.mark_ena_0){
        untag_action->flow_id = cls_fib.flowid_0;
    }
    if (cls_fib.top_cmd_valid_1) {
        single_tag_action->outer_vlan_cmd = cls_fib.top_vlan_cmd_1;
        single_tag_action->new_outer_vlan = cls_fib.top_vlanid_1;
    }
    if (cls_fib.dot1p_valid_1) {
        single_tag_action->new_outer_pri = cls_fib.dot1p_1;
    }
    if (cls_fib.inner_cmd_valid_1) {
        single_tag_action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_1;
        single_tag_action->new_inner_vlan = cls_fib.inner_vlanid_1;
    }
    if(cls_fib.mark_ena_1){
        single_tag_action->flow_id = cls_fib.flowid_1;
    }

    entry_id = logic_port_id * __INGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_DTAG_ENTRY_ID;
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    ret = aal_l2_cls_fib_get(device_id, entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        goto end;
    }

    if (cls_fib.top_cmd_valid_0) {
        double_tag_action->outer_vlan_cmd = cls_fib.top_vlan_cmd_0;
        double_tag_action->new_outer_vlan = cls_fib.top_vlanid_0;
    }
    if (cls_fib.dot1p_valid_0) {
        double_tag_action->new_outer_pri = cls_fib.dot1p_0;
    }
    if (cls_fib.inner_cmd_valid_0) {
        double_tag_action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_0;
        double_tag_action->new_inner_vlan = cls_fib.inner_vlanid_0;
    }
    if(cls_fib.mark_ena_0){
        double_tag_action->flow_id = cls_fib.flowid_0;
    }
end:
    return ret;
}
#define __L2_VLAN_KEY_TYPE_MASK_OUTER_VID 1
#define __L2_VLAN_KEY_TYPE_MASK_INNER_VID 2
#define __L2_VLAN_KEY_TYPE_MASK_OUTER_PRI 4
#define __L2_VLAN_KEY_TYPE_MASK_INNER_PRI 8

ca_status_t __l2_vlan_l2rl2r_key_get(
        ca_device_id_t                 device_id,
        ca_uint8_t                  cls_entry_offset,
        ca_boolean_t                top_is_svlan,
        ca_boolean_t                inner_is_svlan,
        ca_vlan_key_entry_t         *key,
        aal_l2_cls_l2rl2r_key_t     *l2_key,
        aal_l2_cls_l2rl2r_key_mask_t*l2_key_mask
)
{
    ca_uint32_t key_type_mask = 0;

    if(key == NULL || l2_key ==NULL || l2_key_mask == NULL){
        return CA_E_PARAM;
    }
    if(key->key_type.inner_dei || key->key_type.outer_dei ||
        key->key_type.inner_tpid || key->key_type.outer_tpid){
        return CA_E_NOT_SUPPORT;
    }
    if(key->key_type.outer_vid){
        if (key->outer_vid > __MAX_VLAN_ID) {
             return CA_E_PARAM;
         }
        key_type_mask |= __L2_VLAN_KEY_TYPE_MASK_OUTER_VID;
    }
    if(key->key_type.inner_vid){
        if (key->inner_vid > __MAX_VLAN_ID) {
             return CA_E_PARAM;
         }
         key_type_mask |= __L2_VLAN_KEY_TYPE_MASK_INNER_VID;
    }
    if(key->key_type.outer_pbits){
        if(key->outer_pbits > __MAX_PBITS_VALUE){
            return CA_E_PARAM;
        }
        key_type_mask |= __L2_VLAN_KEY_TYPE_MASK_OUTER_PRI;
    }

    if(key->key_type.inner_pbits){
        if(key->inner_pbits > __MAX_PBITS_VALUE){
            return CA_E_PARAM;
        }
        key_type_mask |= __L2_VLAN_KEY_TYPE_MASK_INNER_PRI;
    }

    l2_key->ethernet_type_vld_0     = 0;
    l2_key->ethernet_type_vld_1     = 0;
    l2_key->vlan_number_vld_0       = 0;
    l2_key->vlan_number_vld_1       = 0;
    l2_key->l4_port_field_ctrl_0    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    l2_key->l4_port_field_ctrl_1    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    l2_key->mac_field_ctrl_0        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    l2_key->mac_field_ctrl_1        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    l2_key->valid                   = TRUE;

    l2_key_mask->s.ethernet_type_vld_0     = 1 ;
    l2_key_mask->s.ethernet_type_vld_1     = 1 ;
    l2_key_mask->s.vlan_number_vld_0       = 1 ;
    l2_key_mask->s.vlan_number_vld_1       = 1 ;
    l2_key_mask->s.l4_port_field_ctrl_0    = 1 ;
    l2_key_mask->s.l4_port_field_ctrl_1    = 1 ;
    l2_key_mask->s.mac_field_ctrl_0        = 1 ;
    l2_key_mask->s.mac_field_ctrl_1        = 1 ;
    l2_key_mask->s.valid                   = 1 ;

    switch(key_type_mask){
        case 1:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0   = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_vlan_id_0       = key->outer_vid;
            l2_key->top_is_svlan_0      = (top_is_svlan>0);
            l2_key->inner_vlan_id_0     = key->outer_vid;
            l2_key->dot1p_ctrl_0            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.top_is_svlan_1      = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1           = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_vlan_id_1               = key->outer_vid;
            l2_key->top_is_svlan_1              = (top_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->outer_vid;
            l2_key->dot1p_ctrl_1                = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        }
        break;
        case 2:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;

            l2_key->vlan_field_ctrl_0     = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_vlan_id_0         = key->inner_vid;
            l2_key->inner_is_svlan_0      = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0       = key->inner_vid;
            l2_key->dot1p_ctrl_0          = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
          } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.inner_is_svlan_1      = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1           = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_vlan_id_1               = key->inner_vid;
            l2_key->inner_is_svlan_1              = (inner_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->inner_vid;
            l2_key->dot1p_ctrl_1                = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        }
        break;
        case 3:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0   = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
            l2_key->top_vlan_id_0       = key->outer_vid;
            l2_key->top_is_svlan_0      = (top_is_svlan>0);
            l2_key->inner_is_svlan_0    = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0     = key->inner_vid;
            l2_key->dot1p_ctrl_0            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.top_is_svlan_1      = 1;
            l2_key_mask->s.inner_is_svlan_1    = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
            l2_key->top_vlan_id_1              = key->outer_vid;
            l2_key->top_is_svlan_1             = (top_is_svlan>0);
            l2_key->inner_is_svlan_1           = (inner_is_svlan>0);
            l2_key->inner_vlan_id_1            = key->inner_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        }
        break;

        case 4:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->outer_pbits;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->outer_pbits;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;
        }
        break;

        case 5:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->outer_pbits;
            l2_key->top_vlan_id_0               = key->outer_vid;
            l2_key->top_is_svlan_0              = (top_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->outer_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;

        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.top_is_svlan_1      = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->outer_pbits;
            l2_key->top_vlan_id_1               = key->outer_vid;
            l2_key->top_is_svlan_1              = (top_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->outer_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;
        }
        break;
        case 6:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->outer_pbits;
            l2_key->top_vlan_id_0               = key->inner_vid;
            l2_key->inner_is_svlan_0              = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->inner_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;

        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.inner_is_svlan_1      = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->outer_pbits;
            l2_key->top_vlan_id_1               = key->inner_vid;
            l2_key->inner_is_svlan_1              = (inner_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->inner_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;
        }
        break;

        case 7:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->outer_pbits;
            l2_key->top_vlan_id_0               = key->outer_vid;
            l2_key->top_is_svlan_0             = (top_is_svlan>0);
            l2_key->inner_is_svlan_0             = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->inner_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;

        } else {
             l2_key_mask->s.vlan_field_ctrl_1   = 1;
             l2_key_mask->s.top_vlan_id_1       = 1;
             l2_key_mask->s.top_is_svlan_1      = 1;
             l2_key_mask->s.inner_is_svlan_1    = 1;
             l2_key_mask->s.top_dot1p_1         = 1;
             l2_key_mask->s.inner_dot1p_1       = 1;
             l2_key_mask->s.inner_vlan_id_1     = 1;
             l2_key_mask->s.dot1p_ctrl_1        = 1 ;
             l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
             l2_key->top_dot1p_1                = key->outer_pbits;
             l2_key->inner_dot1p_1              = key->outer_pbits;
             l2_key->top_vlan_id_1               = key->outer_vid;
             l2_key->top_is_svlan_1             = (top_is_svlan>0);
             l2_key->inner_is_svlan_1             = (inner_is_svlan>0);
             l2_key->inner_vlan_id_1             = key->inner_vid;
             l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_TOP_RANGE;

        }
        break;
        case 8:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            l2_key->top_dot1p_0                = key->inner_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            l2_key->top_dot1p_1                = key->inner_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;
        }
        break;
        case 9:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_dot1p_0                = key->inner_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->top_vlan_id_0               = key->outer_vid;
            l2_key->top_is_svlan_0              = (top_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->outer_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;

        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.top_is_svlan_1      = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_dot1p_1                = key->inner_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->top_vlan_id_1               = key->outer_vid;
            l2_key->top_is_svlan_1              = (top_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->outer_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;
        }
        break;
        case 10:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_dot1p_0                = key->inner_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->top_vlan_id_0               = key->inner_vid;
            l2_key->inner_is_svlan_0              = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->inner_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;

        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.inner_is_svlan_1      = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_dot1p_1                = key->inner_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->top_vlan_id_1              = key->inner_vid;
            l2_key->inner_is_svlan_1           = (inner_is_svlan>0);
            l2_key->inner_vlan_id_1            = key->inner_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;
        }
        break;
        case 11:
         if (cls_entry_offset == 0) {
             l2_key_mask->s.vlan_field_ctrl_0   = 1;
             l2_key_mask->s.top_vlan_id_0       = 1;
             l2_key_mask->s.top_is_svlan_0      = 1;
             l2_key_mask->s.inner_is_svlan_0    = 1;
             l2_key_mask->s.top_dot1p_0         = 1;
             l2_key_mask->s.inner_dot1p_0       = 1;
             l2_key_mask->s.inner_vlan_id_0     = 1;
             l2_key_mask->s.dot1p_ctrl_0        = 1 ;
             l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
             l2_key->top_dot1p_0                = key->inner_pbits;
             l2_key->inner_dot1p_0              = key->inner_pbits;
             l2_key->top_vlan_id_0               = key->outer_vid;
             l2_key->top_is_svlan_0             = (top_is_svlan>0);
             l2_key->inner_is_svlan_0             = (inner_is_svlan>0);
             l2_key->inner_vlan_id_0             = key->inner_vid;
             l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;

         } else {
              l2_key_mask->s.vlan_field_ctrl_1   = 1;
              l2_key_mask->s.top_vlan_id_1       = 1;
              l2_key_mask->s.top_is_svlan_1      = 1;
              l2_key_mask->s.inner_is_svlan_1    = 1;
              l2_key_mask->s.top_dot1p_1         = 1;
              l2_key_mask->s.inner_dot1p_1       = 1;
              l2_key_mask->s.inner_vlan_id_1     = 1;
              l2_key_mask->s.dot1p_ctrl_1        = 1 ;
              l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
              l2_key->top_dot1p_1                = key->inner_pbits;
              l2_key->inner_dot1p_1              = key->inner_pbits;
              l2_key->top_vlan_id_1               = key->outer_vid;
              l2_key->top_is_svlan_1             = (top_is_svlan>0);
              l2_key->inner_is_svlan_1             = (inner_is_svlan>0);
              l2_key->inner_vlan_id_1             = key->inner_vid;
              l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_INNER_RANGE;
         }
        break;
        case 12:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        }
        break;
        case 13:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_vlan_id_0               = key->outer_vid;
            l2_key->top_is_svlan_0              = (top_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->outer_vid;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.top_is_svlan_1      = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_TOP_RANGE;
            l2_key->top_vlan_id_1               = key->outer_vid;
            l2_key->top_is_svlan_1              = (top_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->outer_vid;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        }
        break;
        case 14:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->top_vlan_id_0               = key->inner_vid;
            l2_key->inner_is_svlan_0              = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->inner_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;

        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.inner_is_svlan_1      = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_INNER_RANGE;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->top_vlan_id_1              = key->inner_vid;
            l2_key->inner_is_svlan_1           = (inner_is_svlan>0);
            l2_key->inner_vlan_id_1            = key->inner_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        }
        break;
        case 15:
        if (cls_entry_offset == 0) {
            l2_key_mask->s.vlan_field_ctrl_0   = 1;
            l2_key_mask->s.top_vlan_id_0       = 1;
            l2_key_mask->s.top_is_svlan_0      = 1;
            l2_key_mask->s.inner_is_svlan_0    = 1;
            l2_key_mask->s.top_dot1p_0         = 1;
            l2_key_mask->s.inner_dot1p_0       = 1;
            l2_key_mask->s.inner_vlan_id_0     = 1;
            l2_key_mask->s.dot1p_ctrl_0        = 1 ;
            l2_key->vlan_field_ctrl_0          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
            l2_key->top_dot1p_0                = key->outer_pbits;
            l2_key->inner_dot1p_0              = key->inner_pbits;
            l2_key->top_vlan_id_0               = key->outer_vid;
            l2_key->top_is_svlan_0             = (top_is_svlan>0);
            l2_key->inner_is_svlan_0             = (inner_is_svlan>0);
            l2_key->inner_vlan_id_0             = key->inner_vid;
            l2_key->dot1p_ctrl_0               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        } else {
            l2_key_mask->s.vlan_field_ctrl_1   = 1;
            l2_key_mask->s.top_vlan_id_1       = 1;
            l2_key_mask->s.top_is_svlan_1      = 1;
            l2_key_mask->s.inner_is_svlan_1    = 1;
            l2_key_mask->s.top_dot1p_1         = 1;
            l2_key_mask->s.inner_dot1p_1       = 1;
            l2_key_mask->s.inner_vlan_id_1     = 1;
            l2_key_mask->s.dot1p_ctrl_1        = 1 ;
            l2_key->vlan_field_ctrl_1          = AAL_L2_CLS_VLAN_FIELD_CTRL_ALL_MATCH;
            l2_key->top_dot1p_1                = key->outer_pbits;
            l2_key->inner_dot1p_1              = key->inner_pbits;
            l2_key->top_vlan_id_1               = key->outer_vid;
            l2_key->top_is_svlan_1             = (top_is_svlan>0);
            l2_key->inner_is_svlan_1             = (inner_is_svlan>0);
            l2_key->inner_vlan_id_1             = key->inner_vid;
            l2_key->dot1p_ctrl_1               = AAL_L2_CLS_DOT1P_FIELD_CTRL_ALL_MATCH;
        }
        break;
        default:
            return CA_E_PARAM;
    }
      return CA_E_OK;
}

ca_status_t ca_l2_vlan_ingress_action_add(
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t  *key,
    CA_IN ca_vlan_action_t   *action
)
{
    aal_l2_cls_l2rl2r_key_t cls_key;
    aal_l2_cls_l2rl2r_key_t saturn_cls_key;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_l2rl2r_key_mask_t saturn_cls_mask;
    aal_l2_cls_fib_mask_t        fib_mask;
    aal_l2_cls_fib_mask_t        saturn_fib_mask;
    aal_l2_cls_fib_t        cls_fib;
    aal_l2_cls_fib_t        saturn_cls_fib;
    __l2_vlan_action_t      *temp_l2_vlan_action;
    //__l2_vlan_action_t       *vlan_action;
    // ca_uint64_t             key                 = 0;
    ca_uint8_t              cls_entry_id        = 0;
    ca_uint8_t              cls_entry_offset    = 0;
    ca_status_t              ret = CA_E_OK;
    ca_uint32_t              logic_port_id;
    ca_boolean_t             find_flag = FALSE;
    ca_vlan_port_control_t   port_control;
    ca_vlan_key_type_t       key_type;
    ca_vlan_port_control_t   saturn_port_control;
    ca_vlan_key_type_t       saturn_key_type;
    ca_uint32_t              inner_vid = 0;
    ca_uint32_t              outer_vid = 0;
    ca_uint8_t              inner_pbits = 0;
    ca_uint8_t              outer_pbits = 0;

    if ((NULL == action) ||(NULL == key)) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);

    logic_port_id = PORT_ID(port_id);
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&saturn_cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset(&saturn_cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset(&port_control, 0, sizeof(ca_vlan_port_control_t));
    memset(&key_type , 0,sizeof(ca_vlan_key_type_t));
    memset(&saturn_port_control, 0, sizeof(ca_vlan_port_control_t));
    memset(&saturn_key_type , 0,sizeof(ca_vlan_key_type_t));
    cls_mask.u64 = 0;
    saturn_cls_mask.u64 = 0;
    fib_mask.u64 = 0;
    saturn_fib_mask.u64 = 0;

    memcpy(&key_type,&(key->key_type),sizeof(ca_vlan_key_type_t));
    inner_vid = key->inner_vid;
    outer_vid = key->outer_vid;
    inner_pbits = key->inner_pbits;
    outer_pbits = key->outer_pbits;

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    if (__l2_vlan_action_record_find(logic_port_id, key_type, outer_vid, inner_vid, &temp_l2_vlan_action) == 0) {

        ret = __l2_vlan_cls_entry_get(logic_port_id, &cls_entry_id, &cls_entry_offset);
        if (ret != CA_E_OK) {
            return ret;
        }

        printk("\nfunction %s  %d entry_id = %d, entry_offset = %d\n", __FUNCTION__,__LINE__, cls_entry_id, cls_entry_offset);
    } else {
        find_flag = TRUE;
        cls_entry_id = temp_l2_vlan_action->entry_id;
        cls_entry_offset = temp_l2_vlan_action->entry_offset;

        printk("\nfunction %s  %d entry_id = %d, entry_offset = %d\n", __FUNCTION__,__LINE__, cls_entry_id, cls_entry_offset);
    }
    ret = aal_l2_cls_l2rl2r_key_get(device_id, cls_entry_id, &cls_key);
    if (ret != CA_E_OK) {
        L2_DEBUG("aal_l2_cls_l2rl2r_key_get() failed\n");
        return ret;
    }

    ret = ca_l2_vlan_port_control_get(device_id,port_id,&port_control);
    if (ret != CA_E_OK) {
        return ret;
    }
    ret = __l2_vlan_l2rl2r_key_get(device_id,cls_entry_offset,port_control.outer_tpid,port_control.inner_tpid,key,&cls_key,&cls_mask);
    if(ret != CA_E_OK){
        return ret;
    }
    ret = aal_l2_cls_l2rl2r_key_set(device_id, cls_entry_id, cls_mask, &cls_key);
    if (ret != CA_E_OK) {
        return ret;
    }

#if defined(CONFIG_ARCH_CORTINA_G3HGU)

    ret = aal_l2_cls_l2rl2r_key_get(__VLAN_DEVID_FOR_SATURN, cls_entry_id, &saturn_cls_key);
    if (ret != CA_E_OK) {
        L2_DEBUG("aal_l2_cls_l2rl2r_key_get() for saturn failed\n");
        return ret;
    }

    ret = ca_l2_vlan_port_control_get(__VLAN_DEVID_FOR_SATURN,port_id,&saturn_port_control);
    if (ret != CA_E_OK) {
        L2_DEBUG("ca_l2_vlan_port_control_get() for saturn failed\n");
        return ret;
    }
    ret = __l2_vlan_l2rl2r_key_get(__VLAN_DEVID_FOR_SATURN,cls_entry_offset,saturn_port_control.outer_tpid,saturn_port_control.inner_tpid,key,&saturn_cls_key,&saturn_cls_mask);
    if(ret != CA_E_OK){
        L2_DEBUG("__l2_vlan_l2rl2r_key_get() for saturn failed \n");
        return ret;
    }
    ret = aal_l2_cls_l2rl2r_key_set(__VLAN_DEVID_FOR_SATURN, cls_entry_id, saturn_cls_mask, &saturn_cls_key);
    if (ret != CA_E_OK) {
        L2_DEBUG("aal_l2_cls_l2rl2r_key_set() for saturn failed\n");
        return ret;
    }

#endif

    if (cls_entry_offset == 0) {
        fib_mask.s.top_cmd_valid_0 = 1;
        cls_fib.top_cmd_valid_0 = TRUE;
        fib_mask.s.top_vlan_cmd_0 = 1;
        cls_fib.top_vlan_cmd_0 = action->outer_vlan_cmd;
        fib_mask.s.inner_cmd_valid_0 = 1;
        cls_fib.inner_cmd_valid_0 = TRUE;
        fib_mask.s.inner_vlan_cmd_0 = 1;
        cls_fib.inner_vlan_cmd_0 = action->inner_vlan_cmd;
        fib_mask.s.spt_mode_0 =1 ;
        cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
        fib_mask.s.flowid_0 = 1;
        cls_fib.flowid_0 = action->flow_id;
#if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
        if(action->flow_id <= 127){
            fib_mask.s.dp_valid_0   = 1;
            cls_fib.dp_valid_0      = 1;
            fib_mask.s.dp_value_0   = 1;
            cls_fib.dp_value_0      = AAL_LPORT_ETH_WAN;
            fib_mask.s.cos_valid_0  = 1;
            cls_fib.cos_valid_0     = 1;
            fib_mask.s.cos_0        = 1;
            cls_fib.cos_0           = (action->flow_id)&7;
        }
#endif
        fib_mask.s.mark_ena_0 = 1;
        cls_fib.mark_ena_0 = TRUE;
        fib_mask.s.dot1p_valid_0 = 1;
        cls_fib.dot1p_valid_0 = TRUE;
        fib_mask.s.dot1p_0 = 1;
        cls_fib.dot1p_0       = action->new_outer_pri;
        fib_mask.s.top_sc_ind_0 = 1;
        cls_fib.top_sc_ind_0 = port_control.outer_tpid;
        fib_mask.s.inner_sc_ind_0 = 1;
        cls_fib.inner_sc_ind_0 = port_control.inner_tpid;

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        saturn_fib_mask.s.top_cmd_valid_0 = 1;
        saturn_cls_fib.top_cmd_valid_0 = TRUE;
        saturn_fib_mask.s.top_vlan_cmd_0 = 1;
        saturn_cls_fib.top_vlan_cmd_0 = action->outer_vlan_cmd;
        saturn_fib_mask.s.inner_cmd_valid_0 = 1;
        saturn_cls_fib.inner_cmd_valid_0 = TRUE;
        saturn_fib_mask.s.inner_vlan_cmd_0 = 1;
        saturn_cls_fib.inner_vlan_cmd_0 = action->inner_vlan_cmd;
        saturn_fib_mask.s.spt_mode_0 =1 ;
        saturn_cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
        saturn_fib_mask.s.flowid_0 = 1;
        saturn_cls_fib.flowid_0 = action->flow_id;
        saturn_fib_mask.s.dot1p_valid_0 = 1;
        saturn_cls_fib.dot1p_valid_0 = TRUE;
        saturn_fib_mask.s.dot1p_0 = 1;
        saturn_cls_fib.dot1p_0       = action->new_outer_pri;
        saturn_fib_mask.s.top_sc_ind_0 = 1;
        saturn_cls_fib.top_sc_ind_0 = port_control.outer_tpid;
        saturn_fib_mask.s.inner_sc_ind_0 = 1;
        saturn_cls_fib.inner_sc_ind_0 = port_control.inner_tpid;
#endif
    } else {
        fib_mask.s.top_cmd_valid_1 = 1;
        cls_fib.top_cmd_valid_1 = TRUE;
        fib_mask.s.top_vlan_cmd_1 = 1;
        cls_fib.top_vlan_cmd_1 = action->outer_vlan_cmd;
        fib_mask.s.inner_cmd_valid_1 = 1;
        cls_fib.inner_cmd_valid_1 = TRUE;
        fib_mask.s.inner_vlan_cmd_1 = 1;
        cls_fib.inner_vlan_cmd_1 = action->inner_vlan_cmd;
        fib_mask.s.spt_mode_1 =1 ;
        cls_fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
        fib_mask.s.flowid_1 = 1;
        cls_fib.flowid_1 = action->flow_id;
        fib_mask.s.mark_ena_1 = 1;
        cls_fib.mark_ena_1 = TRUE;
        fib_mask.s.dot1p_valid_1 = 1;
        cls_fib.dot1p_valid_1 = TRUE;
        fib_mask.s.dot1p_1 = 1;
        cls_fib.dot1p_1       = action->new_outer_pri;
        fib_mask.s.top_sc_ind_1 = 1;
        cls_fib.top_sc_ind_1 = port_control.outer_tpid;
        fib_mask.s.inner_sc_ind_1 = 1;
        cls_fib.inner_sc_ind_1 = port_control.inner_tpid;
#if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
        if(action->flow_id <= 127){
            fib_mask.s.dp_valid_1   = 1;
            cls_fib.dp_valid_1      = 1;
            fib_mask.s.dp_value_1   = 1;
            cls_fib.dp_value_1      = AAL_LPORT_ETH_WAN;
            fib_mask.s.cos_valid_1  = 1;
            cls_fib.cos_valid_1     = 1;
            fib_mask.s.cos_1        = 1;
            cls_fib.cos_1           = (action->flow_id)&7;
        }
#endif
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        saturn_fib_mask.s.top_cmd_valid_1 = 1;
        saturn_cls_fib.top_cmd_valid_1 = TRUE;
        saturn_fib_mask.s.top_vlan_cmd_1 = 1;
        saturn_cls_fib.top_vlan_cmd_1 = action->outer_vlan_cmd;
        saturn_fib_mask.s.inner_cmd_valid_1 = 1;
        saturn_cls_fib.inner_cmd_valid_1 = TRUE;
        saturn_fib_mask.s.inner_vlan_cmd_1 = 1;
        saturn_cls_fib.inner_vlan_cmd_1 = action->inner_vlan_cmd;
        saturn_fib_mask.s.spt_mode_1 =1 ;
        saturn_cls_fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
        saturn_fib_mask.s.flowid_1 = 1;
        saturn_cls_fib.flowid_1 = action->flow_id;
        saturn_fib_mask.s.dot1p_valid_1 = 1;
        saturn_cls_fib.dot1p_valid_1 = TRUE;
        saturn_fib_mask.s.dot1p_1 = 1;
        saturn_cls_fib.dot1p_1       = action->new_outer_pri;
        saturn_fib_mask.s.top_sc_ind_1 = 1;
        saturn_cls_fib.top_sc_ind_1 = port_control.outer_tpid;
        saturn_fib_mask.s.inner_sc_ind_1 = 1;
        saturn_cls_fib.inner_sc_ind_1 = port_control.inner_tpid;
#endif
    }

    if ((action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
            (action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            cls_fib.top_vlanid_0 = action->new_outer_vlan;
            fib_mask.s.dot1p_valid_0 = 1;
            cls_fib.dot1p_valid_0 = TRUE;
            fib_mask.s.dot1p_0 = 1;
            cls_fib.dot1p_0       = action->new_outer_pri;
        } else {
            fib_mask.s.top_vlanid_1 = 1;
            cls_fib.top_vlanid_1 = action->new_outer_vlan;
            fib_mask.s.dot1p_valid_1 = 1;
            cls_fib.dot1p_valid_1 = TRUE;
            fib_mask.s.dot1p_1 = 1;
            cls_fib.dot1p_1       = action->new_outer_pri;
        }
    }


    if ((action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
             (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
         if (cls_entry_offset == 0) {
             fib_mask.s.inner_vlanid_0 = 1;
             cls_fib.inner_vlanid_0 = action->new_inner_vlan;
         } else {
             fib_mask.s.inner_vlanid_1 = 1;
             cls_fib.inner_vlanid_1 = action->new_inner_vlan;
         }
     }

#if defined(CONFIG_ARCH_CORTINA_G3HGU)

    if((action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH)){
        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            fib_mask.s.inner_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            cls_fib.inner_vlanid_0 = action->new_inner_vlan;
            saturn_cls_fib.top_vlanid_0 = action->new_outer_vlan;
        }else{
            fib_mask.s.top_vlanid_1 = 1;
            fib_mask.s.inner_vlanid_1 = 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            cls_fib.inner_vlanid_1 = action->new_inner_vlan;
            saturn_cls_fib.top_vlanid_1 = action->new_outer_vlan;
        }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_PUSH) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)){
        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            fib_mask.s.inner_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            cls_fib.inner_vlanid_0 = action->new_inner_vlan;
            saturn_cls_fib.top_vlanid_0 = action->new_outer_vlan;
        }else {
            fib_mask.s.top_vlanid_1= 1;
            fib_mask.s.inner_vlanid_1 = 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            cls_fib.inner_vlanid_1 = action->new_inner_vlan;
            saturn_cls_fib.top_vlanid_1 = action->new_outer_vlan;
       }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_PUSH) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_POP)){
        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            fib_mask.s.inner_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            cls_fib.inner_vlanid_0 = 0;
            saturn_cls_fib.top_vlanid_0 = action->new_outer_vlan;
        }else {
            fib_mask.s.top_vlanid_1= 1;
            fib_mask.s.inner_vlanid_1 = 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            cls_fib.inner_vlanid_1 = 0;
            saturn_cls_fib.top_vlanid_1 = action->new_outer_vlan;
       }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_PUSH) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_NOP)){

        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_0 = action->new_outer_vlan;
        }else {
            fib_mask.s.top_vlanid_1= 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_1 = action->new_outer_vlan;
       }

    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_POP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_NOP)){

        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.inner_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_0 = 0;
            saturn_cls_fib.inner_vlanid_0 = 0;

        }else {
            fib_mask.s.top_vlanid_1= 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;
            saturn_fib_mask.s.inner_vlanid_0 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_1 = 0;
            saturn_cls_fib.inner_vlanid_1 = 0;
       }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_POP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_POP)){

        if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            fib_mask.s.inner_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.inner_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            cls_fib.inner_vlanid_0 = 0;
            saturn_cls_fib.top_vlanid_0 = 0;
            saturn_cls_fib.inner_vlanid_0 = 0;

        }else {
            fib_mask.s.top_vlanid_1= 1;
            fib_mask.s.inner_vlanid_1 = 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;
            saturn_fib_mask.s.inner_vlanid_0 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            cls_fib.inner_vlanid_1 = 0;
            saturn_cls_fib.top_vlanid_1 = 0;
            saturn_cls_fib.inner_vlanid_1 = 0;
       }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_POP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH)){

         if (cls_entry_offset == 0) {
            fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.inner_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_0 = 0;
            saturn_cls_fib.inner_vlanid_0 = action->new_inner_vlan;

        }else {
            fib_mask.s.top_vlanid_1= 1;
            saturn_fib_mask.s.top_vlanid_1 = 1;
            saturn_fib_mask.s.inner_vlanid_0 = 1;

            cls_fib.top_vlanid_1 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_1 = 0;
            saturn_cls_fib.inner_vlanid_1 = action->new_inner_vlan;
       }

    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_POP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)){
          if (cls_entry_offset == 0) {
             fib_mask.s.top_vlanid_0 = 1;
             saturn_fib_mask.s.top_vlanid_0 = 1;
             saturn_fib_mask.s.inner_vlanid_0 = 1;

             cls_fib.top_vlanid_0 = action->flow_id + 1;
             saturn_cls_fib.top_vlanid_0 = 0;
             saturn_cls_fib.inner_vlanid_0 = action->new_inner_vlan;

         }else {
             fib_mask.s.top_vlanid_1= 1;
             saturn_fib_mask.s.top_vlanid_1 = 1;
             saturn_fib_mask.s.inner_vlanid_0 = 1;

             cls_fib.top_vlanid_1 = action->flow_id + 1;
             saturn_cls_fib.top_vlanid_1 = 0;
             saturn_cls_fib.inner_vlanid_1 = action->new_inner_vlan;
        }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_NOP)){
          if (cls_entry_offset == 0) {
             fib_mask.s.top_vlanid_0 = 1;
             fib_mask.s.inner_vlanid_0 = 1;
             saturn_fib_mask.s.top_vlanid_0 = 1;

             cls_fib.top_vlanid_0 = action->flow_id + 1;
             cls_fib.inner_vlanid_0 = action->new_outer_vlan;
             saturn_cls_fib.top_vlanid_0 = 0;

         }else {
             fib_mask.s.top_vlanid_1= 1;
             fib_mask.s.inner_vlanid_1 = 1;
             saturn_fib_mask.s.top_vlanid_1 = 1;

             cls_fib.top_vlanid_1 = action->flow_id + 1;
             cls_fib.inner_vlanid_1 = action->new_outer_vlan;
             saturn_cls_fib.top_vlanid_1 = 0;
        }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_POP)){
          if (cls_entry_offset == 0) {
             fib_mask.s.top_vlanid_0 = 1;
             fib_mask.s.inner_vlanid_0 = 1;
             saturn_fib_mask.s.top_vlanid_0 = 1;

             cls_fib.top_vlanid_0 = action->flow_id + 1;
             cls_fib.inner_vlanid_0 = action->new_outer_vlan;
             saturn_cls_fib.top_vlanid_0 = 0;

         }else {
             fib_mask.s.top_vlanid_1= 1;
             fib_mask.s.inner_vlanid_1 = 1;
             saturn_fib_mask.s.top_vlanid_1 = 1;

             cls_fib.top_vlanid_1 = action->flow_id + 1;
             cls_fib.inner_vlanid_1 = action->new_outer_vlan;
             saturn_cls_fib.top_vlanid_1 = 0;
        }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH)){
          if (cls_entry_offset == 0) {
             fib_mask.s.top_vlanid_0 = 1;
             fib_mask.s.inner_vlanid_0 = 1;
             saturn_fib_mask.s.top_vlanid_0 = 1;

             cls_fib.top_vlanid_0 = action->flow_id + 1;
             cls_fib.inner_vlanid_0 = action->new_inner_vlan;
             saturn_cls_fib.top_vlanid_0 = action->new_outer_vlan;

         }else {
             fib_mask.s.top_vlanid_1= 1;
             fib_mask.s.inner_vlanid_1 = 1;
             saturn_fib_mask.s.top_vlanid_1 = 1;

             cls_fib.top_vlanid_1 = action->flow_id + 1;
             cls_fib.inner_vlanid_1 = action->new_inner_vlan;
             saturn_cls_fib.top_vlanid_1 = action->new_outer_vlan;
        }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_SWAP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)){
          if (cls_entry_offset == 0) {
             fib_mask.s.top_vlanid_0 = 1;
             fib_mask.s.inner_vlanid_0 = 1;
             saturn_fib_mask.s.top_vlanid_0 = 1;

             cls_fib.top_vlanid_0 = action->flow_id + 1;
             cls_fib.inner_vlanid_0 = action->new_inner_vlan;
             saturn_cls_fib.top_vlanid_0 = action->new_outer_vlan;

         }else {
             fib_mask.s.top_vlanid_1= 1;
             fib_mask.s.inner_vlanid_1 = 1;
             saturn_fib_mask.s.top_vlanid_1 = 1;

             cls_fib.top_vlanid_1 = action->flow_id + 1;
             cls_fib.inner_vlanid_1 = action->new_inner_vlan;
             saturn_cls_fib.top_vlanid_1 = action->new_outer_vlan;
        }
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_NOP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_NOP)){
         if(cls_entry_offset == 0){
            fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_0 = 0;
         }else{
            fib_mask.s.top_vlanid_0 = 1;
            saturn_fib_mask.s.top_vlanid_0 = 1;

            cls_fib.top_vlanid_0 = action->flow_id + 1;
            saturn_cls_fib.top_vlanid_0 = 0;
         }

    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_NOP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_POP)){
        return CA_E_NOT_SUPPORT;
    }

    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_NOP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)){
        return CA_E_NOT_SUPPORT;
    }


    if((action->outer_vlan_cmd = CA_L2_VLAN_TAG_ACTION_NOP) &&
        (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH)){
          if (cls_entry_offset == 0) {
             fib_mask.s.top_vlanid_0 = 1;
             fib_mask.s.inner_vlanid_0 = 1;
             saturn_fib_mask.s.top_vlanid_0 = 1;

             cls_fib.top_vlanid_0 = action->flow_id + 1;
             cls_fib.inner_vlanid_0 = action->new_outer_vlan;
             saturn_cls_fib.top_vlanid_0 = 0;
         }else {
             fib_mask.s.top_vlanid_1= 1;
             fib_mask.s.inner_vlanid_1 = 1;
             saturn_fib_mask.s.top_vlanid_1 = 1;

             cls_fib.top_vlanid_1 = action->flow_id + 1;
             cls_fib.inner_vlanid_1 = action->new_outer_vlan;
             saturn_cls_fib.top_vlanid_1 = 0;
        }
    }

#endif

    ret = aal_l2_cls_fib_set(device_id, cls_entry_id,fib_mask, &cls_fib);
    if (ret != CA_E_OK) {
        L2_DEBUG("aal_l2_cls_fib_set() failed\n");
        return ret;
    }

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
    ret = aal_l2_cls_fib_set(__VLAN_DEVID_FOR_SATURN, cls_entry_id,fib_mask, &cls_fib);
    if (ret != CA_E_OK) {
        L2_DEBUG("aal_l2_cls_fib_set() for saturn failed\n");
        return ret;
    }
#endif

    if(find_flag == FALSE){
        temp_l2_vlan_action = (__l2_vlan_action_t *)ca_malloc(sizeof(__l2_vlan_action_t));
        if(temp_l2_vlan_action == NULL){
            return CA_E_RESOURCE;
        }
        memset(temp_l2_vlan_action, 0, sizeof(__l2_vlan_action_t));
    }
    temp_l2_vlan_action->inner_vid = inner_vid;
    temp_l2_vlan_action->outer_vid = outer_vid;
    temp_l2_vlan_action->key_type = key_type;
    temp_l2_vlan_action->entry_id = cls_entry_id;
    temp_l2_vlan_action->entry_offset = cls_entry_offset;
    temp_l2_vlan_action->new_outer_pri_src = action->new_outer_pri_src;
    temp_l2_vlan_action->new_outer_vlan_src = action->new_outer_vlan_src ;
    temp_l2_vlan_action->inner_pbits = inner_pbits;
    temp_l2_vlan_action->outer_pbits = outer_pbits;

    __l2_vlan_action_record_add(logic_port_id, temp_l2_vlan_action);

    __g_l2_vlan_cls_entry_occupy[logic_port_id] |= 1<<(((cls_entry_id - logic_port_id*__INGRESS_CLS_ENTRY_NUM_PER_PORT)<<1)|cls_entry_offset);

    return ret;
}

ca_status_t ca_l2_vlan_ingress_action_delete(
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t key
)
{
    aal_l2_cls_l2rl2r_key_t cls_key;
    aal_l2_cls_fib_t        cls_fib;
    aal_l2_cls_fib_mask_t        fib_mask;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    __l2_vlan_action_t      *temp_l2_vlan_action;
    ca_uint8_t              cls_entry_id        = 0;
    ca_uint8_t              cls_entry_offset    = 0;
    ca_status_t              ret = CA_E_OK;
    ca_uint32_t                 logic_port_id;
    ca_vlan_key_type_t       key_type;
    ca_uint32_t              inner_vid = 0;
    ca_uint32_t              outer_vid = 0;
    ca_uint8_t              inner_pbits = 0;
    ca_uint8_t              outer_pbits = 0;


    memcpy(&key_type,&(key.key_type),sizeof(ca_vlan_key_type_t));
    inner_vid = key.inner_vid;
    outer_vid = key.outer_vid;
    inner_pbits = key.inner_pbits;
    outer_pbits = key.outer_pbits;

     printk("%s, port_id = 0x%x, inner_vid = %d, outer_vid = %d\n",
        __FUNCTION__, port_id, inner_vid,outer_vid);

    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

   if(key_type.outer_vid){
        if (outer_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }
   if(key_type.inner_vid){
        if (inner_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }
    if(key_type.outer_pbits){
        if (outer_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if(key_type.inner_pbits){
        if (inner_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if (__l2_vlan_action_record_find(logic_port_id, key_type, outer_vid, inner_vid, &temp_l2_vlan_action) == 0) {
        return CA_E_OK;
    } else {
        cls_entry_id = temp_l2_vlan_action->entry_id;
        cls_entry_offset = temp_l2_vlan_action->entry_offset;
    }

    ret = aal_l2_cls_l2rl2r_key_get(device_id, cls_entry_id, &cls_key);
    if (ret != CA_E_OK) {
        return ret;
    }
    /*
    ret = aal_l2_cls_l2rl2r_key_del(device_id,cls_entry_id);
    if (ret != CA_E_OK) {
        return ret;
    }
    */
    // memset(&cls_key.l2r_key[cls_entry_offset], 0, sizeof(aal_l2_cls_l2r_key_t));
    if (cls_entry_offset == 0) {
        cls_mask.u64 = 0;
        cls_mask.s.vlan_field_ctrl_0 = 1;
        cls_key.vlan_field_ctrl_0 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
        cls_mask.s.dot1p_ctrl_0 = 1;
        cls_key.dot1p_ctrl_0 = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        cls_mask.s.vlan_number_vld_0 = 1;
        cls_key.vlan_number_vld_0 = FALSE;
        if(0 == __VLAN_COMPBIT(__g_l2_vlan_cls_entry_occupy[logic_port_id],(((cls_entry_id - logic_port_id*__INGRESS_CLS_ENTRY_NUM_PER_PORT) << 1) | 1))){
           cls_mask.s.valid = 1;
           cls_key.valid = 0;
        }
    } else {
        cls_mask.u64 = 0;
        cls_mask.s.vlan_field_ctrl_1 = 1;
        cls_key.vlan_field_ctrl_1 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
        cls_mask.s.dot1p_ctrl_1 = 1;
        cls_key.dot1p_ctrl_1 = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        cls_mask.s.vlan_number_vld_1 = 1;
        cls_key.vlan_number_vld_1 = FALSE;
        if(0 == __VLAN_COMPBIT(__g_l2_vlan_cls_entry_occupy[logic_port_id],(((cls_entry_id - logic_port_id*__INGRESS_CLS_ENTRY_NUM_PER_PORT) << 1) | 0))){
           cls_mask.s.valid = 1;
           cls_key.valid = 0;
        }
    }

    ret = aal_l2_cls_l2rl2r_key_set(device_id, cls_entry_id, cls_mask, &cls_key);
    if (ret != CA_E_OK) {
        return ret;
    }

    ret = aal_l2_cls_fib_get(device_id, cls_entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        return ret;
    }
    fib_mask.u64 = 0;
    // memset(&cls_fib.cls_fib[cls_entry_offset], 0, sizeof(aal_l2_cls_one_fib_t));
    if (cls_entry_offset == 0) {
        fib_mask.s.top_cmd_valid_0 = 1;
        cls_fib.top_cmd_valid_0 = FALSE;
        fib_mask.s.inner_cmd_valid_0 = 1;
        cls_fib.inner_cmd_valid_0 = FALSE;

        fib_mask.s.top_vlan_cmd_0 = 1;
        fib_mask.s.top_vlanid_0 = 1 ;
        fib_mask.s.dot1p_0 = 1;
        fib_mask.s.inner_vlan_cmd_0 = 1;
        fib_mask.s.inner_vlanid_0 = 1;
        fib_mask.s.top_sc_ind_0 = 1;
        fib_mask.s.inner_sc_ind_0 = 1;

        cls_fib.top_vlan_cmd_0 = 0;
        cls_fib.top_vlanid_0 = 0;
        cls_fib.dot1p_0 = 0;
        cls_fib.inner_cmd_valid_0 = 0;
        cls_fib.inner_vlanid_0 = 0;
        cls_fib.top_sc_ind_0 = 0;
        cls_fib.inner_sc_ind_0 = 0;

    } else {
        fib_mask.s.top_cmd_valid_1 = 1;
        cls_fib.top_cmd_valid_1 = FALSE;
        fib_mask.s.inner_cmd_valid_1 = 1;
        cls_fib.inner_cmd_valid_1 = FALSE;

        fib_mask.s.top_vlan_cmd_1 = 1;
        fib_mask.s.top_vlanid_1 = 1 ;
        fib_mask.s.dot1p_1 = 1;
        fib_mask.s.inner_vlan_cmd_1 = 1;
        fib_mask.s.inner_vlanid_1 = 1;
        fib_mask.s.top_sc_ind_1 = 1;
        fib_mask.s.inner_sc_ind_1 = 1;

        cls_fib.top_vlan_cmd_1 = 0;
        cls_fib.top_vlanid_1 = 0;
        cls_fib.dot1p_1 = 0;
        cls_fib.inner_cmd_valid_1 = 0;
        cls_fib.inner_vlanid_1 = 0;
        cls_fib.top_sc_ind_1 = 0;
        cls_fib.inner_sc_ind_1 = 0;

    }

    ret = aal_l2_cls_fib_set(device_id, cls_entry_id,fib_mask, &cls_fib);
    if (ret != CA_E_OK) {
        return ret;
    }

    __l2_vlan_action_record_delete(logic_port_id, key_type, outer_vid, inner_vid);

    __g_l2_vlan_cls_entry_occupy[logic_port_id] &= ~(1 << (((cls_entry_id - logic_port_id*__INGRESS_CLS_ENTRY_NUM_PER_PORT) << 1) | cls_entry_offset));

    ca_free(temp_l2_vlan_action);

    temp_l2_vlan_action = NULL;

    return ret;
}


ca_status_t ca_l2_vlan_ingress_action_delete_all (
	CA_IN		ca_device_id_t				device_id,
	CA_IN		ca_port_id_t			port_id
)
{
	ca_status_t         ret = CA_E_OK;
	 __l2_vlan_action_t   *ptemp = NULL;
	ca_uint32_t         logic_port_id = 0;
    ca_vlan_key_entry_t  key_entry;

    memset(&key_entry,0,sizeof(ca_vlan_key_entry_t));
    memset(&key_entry,0,sizeof(ca_vlan_key_entry_t));

	CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

	ptemp = __port_vlan_ingress_act_head[logic_port_id].next;

	while(ptemp)
	{
   		memcpy(&key_entry.key_type, &(ptemp->key_type),sizeof(ca_vlan_key_type_t));
    	key_entry.inner_vid = ptemp->inner_vid;
    	key_entry.outer_vid = ptemp->outer_vid;

    	ptemp = ptemp->next;

		ret = ca_l2_vlan_ingress_action_delete(device_id ,port_id,key_entry);
		if (ret != CA_E_OK) {
        	return ret;
    	}
	}

	return CA_E_OK;
}

ca_status_t ca_l2_vlan_ingress_action_get(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_port_id_t       port_id,
    CA_IN  ca_vlan_key_entry_t key,
    CA_OUT ca_vlan_action_t   *action
)
{
    aal_l2_cls_fib_t        cls_fib;
    __l2_vlan_action_t      *temp_l2_vlan_action = NULL;
    ca_uint8_t              cls_entry_id        = 0;
    ca_uint8_t              cls_entry_offset    = 0;
    ca_status_t              ret = CA_E_OK;
    ca_uint32_t              logic_port_id;
    ca_vlan_key_type_t       key_type;
    ca_uint32_t              inner_vid = 0;
    ca_uint32_t              outer_vid = 0;
    ca_uint8_t              inner_pbits = 0;
    ca_uint8_t              outer_pbits = 0;

    if (NULL == action) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&key_type , 0,sizeof(ca_vlan_key_type_t));

    memcpy(&key_type,&key.key_type,sizeof(ca_vlan_key_type_t));
    inner_vid = key.inner_vid;
    outer_vid = key.outer_vid;
    inner_pbits = key.inner_pbits;
    outer_pbits = key.outer_pbits;

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    if(key_type.outer_vid){
        if (outer_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }

   if(key_type.inner_vid){
        if (inner_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }

    if(key_type.outer_pbits){
        if (outer_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if(key_type.inner_pbits){
        if (inner_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }


    if (__l2_vlan_action_record_find(logic_port_id, key_type, outer_vid, inner_vid, &temp_l2_vlan_action) == 0) {
        return CA_E_NOT_FOUND;
    } else {
        cls_entry_id = temp_l2_vlan_action->entry_id;
        cls_entry_offset = temp_l2_vlan_action->entry_offset;
    }

    ret = aal_l2_cls_fib_get(device_id, cls_entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        return ret;
    }
    if (cls_entry_offset == 0) {
        if (cls_fib.top_cmd_valid_0) {
            action->outer_vlan_cmd = cls_fib.top_vlan_cmd_0;
            action->new_outer_vlan = cls_fib.top_vlanid_0;
        }
        if (cls_fib.dot1p_valid_0) {
            //For ingress action ,not ssupport new_outer_pri & new_inner_pri from Configure guide doc
            //action->new_outer_pri = cls_fib.dot1p_0;
        }
        if (cls_fib.inner_cmd_valid_0) {
            action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_0;
            action->new_inner_vlan = cls_fib.inner_vlanid_0;
        }
        if(cls_fib.mark_ena_0){
            action->flow_id = cls_fib.flowid_0;
        }
    } else {
        if (cls_fib.top_cmd_valid_1) {
            action->outer_vlan_cmd = cls_fib.top_vlan_cmd_1;
            action->new_outer_vlan = cls_fib.top_vlanid_1;
        }
        if (cls_fib.dot1p_valid_1) {
            //For ingress action ,not ssupport new_outer_pri & new_inner_pri from Configure guide doc
            //action->new_outer_pri = cls_fib.dot1p_1;
        }
        if (cls_fib.inner_cmd_valid_1) {
            action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_1;
            action->new_inner_vlan = cls_fib.inner_vlanid_1;
        }
        if(cls_fib.mark_ena_1){
            action->flow_id = cls_fib.flowid_1;
        }
    }

    action->new_outer_pri_src = temp_l2_vlan_action->new_outer_pri_src;
    action->new_outer_vlan_src = temp_l2_vlan_action->new_outer_vlan_src;

    return CA_E_OK;
}

ca_status_t ca_l2_vlan_ingress_action_iterate (
    CA_IN		ca_device_id_t					device_id,
    CA_IN		ca_port_id_t			    port_id,
    CA_IN_OUT	ca_iterator_t				*p_return_entry)
{

    ca_uint32_t    logic_port_id = 0;
    ca_uint32_t    i = 0,j = 0;
    __l2_vlan_action_t   *ptemp = NULL;
    ca_vlan_action_iterate_entry_t ca_vlan_action_iterate_entry;
    ca_status_t   ret = CA_E_OK;
    ca_vlan_key_entry_t  key_entry;

    CA_ASSERT_RET(p_return_entry != NULL && p_return_entry->p_entry_data != NULL, CA_E_PARAM);
    CA_ASSERT_RET(p_return_entry->entry_count, CA_E_PARAM);
    CA_ASSERT_RET(p_return_entry->entry_count * sizeof(ca_vlan_action_iterate_entry_t) <= p_return_entry->user_buffer_size, CA_E_UNAVAIL);
    memset(&key_entry,0,sizeof(ca_vlan_key_entry_t));

    memset(&ca_vlan_action_iterate_entry,0,sizeof(ca_vlan_action_iterate_entry_t));
    memset(&key_entry,0,sizeof(ca_vlan_key_entry_t));

    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    if(NULL == p_return_entry->p_prev_handle){
        ptemp = __port_vlan_ingress_act_head[logic_port_id].next;
     }else{
        ptemp =  p_return_entry->p_prev_handle;
        if(NULL == ptemp->next)
            return CA_E_UNAVAIL;
     }

    while(ptemp &&(i < p_return_entry->entry_count)){

        ca_vlan_action_iterate_entry.key.key_type = ptemp->key_type;
        ca_vlan_action_iterate_entry.key.inner_vid = ptemp->inner_vid;
        ca_vlan_action_iterate_entry.key.outer_vid = ptemp->outer_vid;
        ca_vlan_action_iterate_entry.key.inner_pbits = ptemp->inner_pbits;
        ca_vlan_action_iterate_entry.key.outer_pbits = ptemp->outer_pbits;

        key_entry.key_type  = ptemp->key_type;
        key_entry.inner_vid = ptemp->inner_vid;
        key_entry.outer_vid = ptemp->outer_vid;
        key_entry.inner_pbits = ptemp->inner_pbits;
        key_entry.outer_pbits = ptemp->outer_pbits;

        ca_l2_vlan_ingress_action_get(device_id,port_id,key_entry,&ca_vlan_action_iterate_entry.action);
        memcpy(p_return_entry->p_entry_data + i*sizeof(ca_vlan_action_iterate_entry_t),\
                &ca_vlan_action_iterate_entry,sizeof(ca_vlan_action_iterate_entry_t));
        p_return_entry->entry_index[j++] = ptemp->entry_id;
        p_return_entry->p_prev_handle = ptemp;
        ptemp = ptemp->next;
        i++;
    }

    p_return_entry->entry_count = i;

    if( 0 == i){
        p_return_entry->p_prev_handle = NULL;
        return CA_E_UNAVAIL;
     }else{
        return CA_E_OK;
     }
    return ret;
}


/* Default configuration:
   CSR_VLAN_TPID_A = 0x8100
   CSR_VLAN_TPID_B = 0
   CSR_VLAN_TPID_C = 0x8100
   CSR_VLAN_TPID_D = 0

   L2FE_PE_TPID_SEL_CONFIG_TBL_DATA.stag = 10;
   L2FE_PE_TPID_SEL_CONFIG_TBL_DATA.ctag = 10;

   CSR_VLAN_TPID_BD_used_bmp = 0b111111;

    CSR_VLAN_TPID_A & CSR_VLAN_TPID_C is 0x8100 and cannot be removed. As they are also be used by untag packets.
    CSR_VLAN_TPID_B & CSR_VLAN_TPID_D should be the same as egress can't diffirentiate stag or ctag.
*/

/*  L2FE_PE_TPID_SEL_CONFIG_TBL_DATA.stag
    outgoing SVLAN TPID selection control for this port.
    When "00",use CSR_VLAN_TPID_A;
    when "01",use CSR_VLAN_TPID_B;
    when "10",use CSR_OLD_A. It means the SVLAN TPID carried by the incoming packet will be used. If no SVLAN,use CSR_VLAN_TPID_A;
    when "11",use CSR_OLD_B. It means the SVLAN TPID carried by the incoming packet will be used. If no SVLAN,use CSR_VLAN_TPID_B
    */
ca_status_t __l2_vlan_egress_tpid_check(ca_vlan_action_t *default_action)
{
    /* both local, or both keep */
    if(default_action->new_inner_tpid_src == CA_L2_VLAN_TPID_LOCAL && default_action->new_outer_tpid_src == CA_L2_VLAN_TPID_LOCAL){

        if((default_action->new_inner_tpid_index > __VLAN_TPID_NUM-1) ||
            (default_action->new_outer_tpid_index > __VLAN_TPID_NUM-1)){
            return CA_E_PARAM;
        }

        if((__vlan_inner_tpid[default_action->new_inner_tpid_index] == 0) ||
            (__vlan_outer_tpid[default_action->new_outer_tpid_index] == 0)){
            ca_printf("inner_tpid outer tpid has not been set in the tpid poll\r\n");
            return CA_E_PARAM;
        }
        if(__vlan_inner_tpid[default_action->new_inner_tpid_index] != __vlan_outer_tpid[default_action->new_outer_tpid_index]){
            ca_printf("new_inner_tpid index %d [0x%x] != new_outer_tpid index %d [0x%x]\r\n",
                default_action->new_inner_tpid_index, __vlan_inner_tpid[default_action->new_inner_tpid_index],
                default_action->new_outer_tpid_index, __vlan_outer_tpid[default_action->new_outer_tpid_index]);

            return CA_E_NOT_SUPPORT;
        }
        return CA_E_OK;
    }else if(default_action->new_inner_tpid_src == CA_L2_VLAN_TPID_INNER_TAG && default_action->new_outer_tpid_src == CA_L2_VLAN_TPID_OUTER_TAG){
        return CA_E_OK;
    }
    return CA_E_NOT_SUPPORT;
}

/* set the value of CSR_VLAN_TPID_B and CSR_VLAN_TPID_D */
ca_status_t __l2_vlan_egress_tpid_bd_set(
    ca_device_id_t                 device_id,
    ca_uint32_t                 tpid_value)
{
    aal_l2_vlan_default_cfg_mask_t  vlan_msk;
    aal_l2_vlan_default_cfg_t  vlan_cfg;

    memset(&vlan_msk, 0, sizeof(aal_l2_vlan_default_cfg_mask_t));
    memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    vlan_msk.s.tx_tpid1 = 1;
    vlan_msk.s.tx_tpid3 = 1;
    vlan_cfg.tx_tpid1 = tpid_value;
    vlan_cfg.tx_tpid3 = tpid_value;
    return aal_l2_vlan_default_cfg_set(device_id, vlan_msk, &vlan_cfg);
}

ca_status_t __l2_vlan_egress_tpid_sel_set(
    ca_device_id_t                 device_id,
    ca_uint32_t                 logical_port_id,
    aal_l2_vlan_tx_tpid_sel_t   tpid_sel)
{
    aal_l2_vlan_port_tx_tpid_sel_mask_t tpid_msk;
    aal_l2_vlan_port_tx_tpid_sel_t tpid_cfg;
    ca_status_t ret = CA_E_OK;

    memset(&tpid_msk, 0, sizeof(aal_l2_vlan_port_tx_tpid_sel_mask_t));
    memset(&tpid_cfg, 0, sizeof(aal_l2_vlan_port_tx_tpid_sel_t));
    tpid_msk.s.svlan = 1;
    tpid_msk.s.cvlan = 1;
    tpid_cfg.svlan = tpid_sel;
    tpid_cfg.cvlan = tpid_sel;
    ret = aal_l2_vlan_tx_tpid_sel_set(device_id, logical_port_id, tpid_msk, &tpid_cfg);
    if (ret != CA_E_OK) {
        ca_printf("aal_l2_vlan_tx_tpid_sel_set failed. ret = %d\r\n", ret);
        return ret;
    }
    if(tpid_sel == AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_A || tpid_sel == AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A){
        if((__vlan_tpid_b_d_used_bmp & ((ca_uint64_t)1<<logical_port_id)) != 0){ /* this port used CSR_XXX_B before */
           __vlan_tpid_b_d_used_bmp &= ~((ca_uint64_t)1<<logical_port_id);
           if(__vlan_tpid_b_d_used_bmp == 0){ /* No port use CSR_XXX_B now, clear CSR_XXX_B & CSR_XXX_D */
                ca_printf("No port use CSR_XXX_B now, clear CSR_XXX_B & CSR_XXX_D\r\n");
                ret = __l2_vlan_egress_tpid_bd_set(device_id, 0);
                if (ret != CA_E_OK) {
                    return ret;
                }
            }
        }
    }else if(tpid_sel == AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_B || tpid_sel == AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_B){
        if((__vlan_tpid_b_d_used_bmp & ((ca_uint64_t)1<<logical_port_id)) == 0){ /* this port not used CSR_XXX_B before */
           __vlan_tpid_b_d_used_bmp |= ((ca_uint64_t)1<<logical_port_id);
        }
    }

    ca_printf("__l2_vlan_egress_tpid_sel_set used_bmp 0x%x\r\n", __vlan_tpid_b_d_used_bmp);
    return CA_E_OK;
}

ca_status_t __l2_vlan_egress_tpid_set(
    ca_device_id_t             device_id,
    ca_uint32_t             logical_port_id,
    ca_vlan_tpid_source_t   tpid_src,
    ca_uint32_t             tpid_value)
{
    aal_l2_vlan_default_cfg_t  vlan_cfg;
    ca_status_t ret = CA_E_OK;

    memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
    ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);
    if (ret != CA_E_OK) {
        goto end;
    }

    if(tpid_src == CA_L2_VLAN_TPID_LOCAL){    /* case1: use new tpid set by API */
        ca_printf("new egress tpid set to 0x%x\r\n", tpid_value);
        if(tpid_value == 0x8100){
            ret = __l2_vlan_egress_tpid_sel_set(device_id, logical_port_id, AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_A);
            if (ret != CA_E_OK) {
                goto end;
            }

        }else{
            if(__vlan_tpid_b_d_used_bmp != 0 && (vlan_cfg.tx_tpid1 != tpid_value)){
                ca_printf("CSR_VLAN_TPID_B has been used, used_bmp 0x%x \r\n", __vlan_tpid_b_d_used_bmp);
                return CA_E_RESOURCE;
            }else{
                if(__vlan_tpid_b_d_used_bmp == 0){
                    ret = __l2_vlan_egress_tpid_bd_set(device_id, tpid_value);
                    if (ret != CA_E_OK) {
                        goto end;
                    }
                }
                ret = __l2_vlan_egress_tpid_sel_set(device_id, logical_port_id, AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_B);
                if (ret != CA_E_OK) {
                    goto end;
                }
            }
        }

    }else{  /* case2: keep tpid of incoming packets */
        ca_printf("keep tpid at egress\r\n");
        ret = __l2_vlan_egress_tpid_sel_set(device_id, logical_port_id, AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A);
        if (ret != CA_E_OK) {
            goto end;
        }
    }
end:
    return ret;
}

ca_status_t ca_l2_vlan_egress_default_action_set(
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_port_id_t        port_id,
    CA_IN       ca_vlan_action_t    *default_action,
    CA_IN       ca_vlan_action_t    *untag_action,
    CA_IN       ca_vlan_action_t    *single_tag_action,
    CA_IN       ca_vlan_action_t    *double_tag_action
)
{
    ca_vlan_tpid_source_t           tpid_src;
    ca_uint32_t                     tpid_value;
    aal_l2_cls_l2rl2r_key_t         cls_key;
    aal_l2_cls_fib_t                cls_fib;
    aal_l2_cls_fib_mask_t           fib_mask;
    aal_l2_cls_l2rl2r_key_mask_t    cls_mask;
    aal_arb_mc_fib_mask_t           mc_fib_mask;
    aal_arb_mc_fib_t                mc_fib;
    ca_vlan_port_control_t          port_control;

    ca_status_t             ret = CA_E_OK;
    ca_uint16_t             entry_id = 0;
    ca_uint32_t             logic_port_id = 0;
    ca_uint32_t             mc_fib_id = 0;
    ca_uint64_t             mc_fib_bmp = 0;

    memset(&mc_fib, 0, sizeof(aal_arb_mc_fib_t));
    memset(&port_control, 0, sizeof(aal_arb_mc_fib_t));

    if ((NULL == default_action) || (NULL == untag_action) || (NULL == single_tag_action) || (NULL == double_tag_action)) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);

    if (untag_action->outer_vlan_cmd > CA_L2_VLAN_TAG_ACTION_PUSH) {
        return CA_E_PARAM;
    }
    if (untag_action->inner_vlan_cmd > CA_L2_VLAN_TAG_ACTION_PUSH) {
        return CA_E_PARAM;
    }
    if (single_tag_action->inner_vlan_cmd > CA_L2_VLAN_TAG_ACTION_PUSH) {
        return CA_E_PARAM;
    }

    ca_printf("new_inner_tpid_src %d, new_inner_tpid_index 0x%x, new_outer_tpid_src %d, new_outer_tpid_index 0x%x\r\n",
        default_action->new_inner_tpid_src, default_action->new_inner_tpid_index,
        default_action->new_outer_tpid_src, default_action->new_outer_tpid_index);

    ret = __l2_vlan_egress_tpid_check(default_action);
    if(ret != CA_E_OK){
        return ret;
    }

    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    tpid_src = default_action->new_outer_tpid_src;
    tpid_value = __vlan_outer_tpid[default_action->new_outer_tpid_index];
    ret = __l2_vlan_egress_tpid_set(device_id, logic_port_id, tpid_src, tpid_value);
    if (ret != CA_E_OK) {
        goto end;
    }

    ret = ca_l2_vlan_port_control_get(device_id,port_id,&port_control);
    if (ret != CA_E_OK) {
        return ret;
    }

    mc_fib_mask.u32         = 0;
    mc_fib_mask.s.vlan_cmd  = 1;
    mc_fib_mask.s.sc_ind    = 1;
    mc_fib_mask.s.dot1p_cmd = 1;
    mc_fib_mask.s.dot1p     = 1;
    mc_fib_mask.s.vid       = 1;

    mc_fib.vlan_cmd = default_action->outer_vlan_cmd;
    if(port_control.outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN){
        mc_fib.sc_ind = TRUE;
    }else{
        mc_fib.sc_ind = FALSE;
    }

    mc_fib.vid   = default_action->new_outer_vlan;
    switch(default_action->new_outer_pri_src){
        case CA_L2_VLAN_TAG_PRI_SRC_NEW_PRI:
            mc_fib.dot1p_cmd = AAL_ARB_MC_DOT1P_CMD_USE_THIS_TABLE;
        break;
        case CA_L2_VLAN_TAG_PRI_SRC_OUTER_PRI:
            mc_fib.dot1p_cmd = AAL_ARB_MC_DOT1P_CMD_KEEP_OLD;
        break;
        case CA_L2_VLAN_TAG_PRI_SRC_DSCP_TO_8021P_TABLE:
            mc_fib.dot1p_cmd = AAL_ARB_MC_DOT1P_CMD_NO_IP_USE_EGRESS_TABLE;
        break;
        case CA_L2_VLAN_TAG_PRI_SRC_8021P_REMARK_TABLE:
            mc_fib.dot1p_cmd = AAL_ARB_MC_DOT1P_CMD_USE_EGRESS_TABLE;
        break;
        default:
            mc_fib.dot1p_cmd = AAL_ARB_MC_DOT1P_CMD_USE_THIS_TABLE;
        break;
    }
    mc_fib.dot1p = default_action->new_outer_pri;

    ret = __l2_vlan_non_uc_dpid_fib_index_search(device_id, logic_port_id,&mc_fib_bmp);
    if (ret != CA_E_OK) {
        goto end;
    }

    for(mc_fib_id = 0; mc_fib_id<64; mc_fib_id++){
        if((mc_fib_bmp >> mc_fib_id)&1){
#ifdef      MC_FIB_COS_CMD
            mc_fib_mask.s.cos_cmd = 1;
            mc_fib.cos_cmd = 1;
#endif
            ret = aal_arb_mc_fib_set(device_id,mc_fib_id,mc_fib_mask,&mc_fib);
            if(ret != CA_E_OK){
                goto end;
            }
        }
    }

    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    cls_mask.u64 = 0;
    cls_key.valid = 1;
   // cls_key.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
    //cls_key.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    //cls_key.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    cls_mask.s.valid       = 1;
    //cls_mask.s.entry_type  = 1;
    //cls_mask.s.rule_type_0 = 1;
    //cls_mask.s.rule_type_1 = 1;
    cls_key.vlan_number_vld_0 = 1;
    cls_key.vlan_number_0 = 0;

    cls_key.vlan_number_vld_1 = 1;
    cls_key.vlan_number_1 = 1;

    cls_mask.s.vlan_number_vld_0   = 1;
    cls_mask.s.vlan_number_0       = 1;
    cls_mask.s.vlan_number_vld_1   = 1;
    cls_mask.s.vlan_number_1       = 1;


    cls_key.ethernet_type_vld_0     = 0;
    cls_key.ethernet_type_vld_1     = 0;
    cls_key.l4_port_field_ctrl_0    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.l4_port_field_ctrl_1    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.mac_field_ctrl_0        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.mac_field_ctrl_1        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.vlan_field_ctrl_0       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.vlan_field_ctrl_1       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.dot1p_ctrl_0            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_key.dot1p_ctrl_1            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;

    cls_mask.s.ethernet_type_vld_0     = 1 ;
    cls_mask.s.ethernet_type_vld_1     = 1 ;
    cls_mask.s.l4_port_field_ctrl_0    = 1 ;
    cls_mask.s.l4_port_field_ctrl_1    = 1 ;
    cls_mask.s.mac_field_ctrl_0        = 1 ;
    cls_mask.s.mac_field_ctrl_1        = 1 ;
    cls_mask.s.vlan_field_ctrl_0       = 1 ;
    cls_mask.s.vlan_field_ctrl_1       = 1 ;
    cls_mask.s.dot1p_ctrl_0            = 1 ;
    cls_mask.s.dot1p_ctrl_1            = 1 ;

    entry_id = logic_port_id* __EGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_UTAG_STAG_ENTRY_ID;
    ret = aal_l2_cls_egr_l2rl2r_key_set(device_id, entry_id, cls_mask , &cls_key);
    if (ret != CA_E_OK) {
        goto end;

    }
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));

    fib_mask.u64 = 0;
    fib_mask.s.top_cmd_valid_0 = 1;
    cls_fib.top_cmd_valid_0 = TRUE;
    fib_mask.s.top_vlan_cmd_0 = 1;
    cls_fib.top_vlan_cmd_0 = untag_action->outer_vlan_cmd;

    if (untag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) {
        fib_mask.s.top_vlanid_0 = 1;
        cls_fib.top_vlanid_0 = untag_action->new_outer_vlan;
        fib_mask.s.dot1p_valid_0 = 1;
        cls_fib.dot1p_valid_0 = TRUE;
        fib_mask.s.dot1p_0 = 1;
        cls_fib.dot1p_0       = untag_action->new_outer_pri;
    }
    fib_mask.s.top_sc_ind_0 = 1;
    cls_fib.top_sc_ind_0 = (port_control.outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN);
    fib_mask.s.inner_cmd_valid_0 = 1;
    cls_fib.inner_cmd_valid_0 = TRUE;
    fib_mask.s.inner_vlan_cmd_0 = 1;
    cls_fib.inner_vlan_cmd_0 = untag_action->inner_vlan_cmd;
    if (untag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) {
        fib_mask.s.inner_vlanid_0 = 1;
        cls_fib.inner_vlanid_0 = untag_action->new_inner_vlan;
    }
    fib_mask.s.top_cmd_valid_1 = 1;
    cls_fib.top_cmd_valid_1 = TRUE;
    fib_mask.s.top_vlan_cmd_1 = 1;
    cls_fib.top_vlan_cmd_1 = single_tag_action->outer_vlan_cmd;

    if ((single_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) || (single_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        fib_mask.s.top_vlanid_1 = 1;
        cls_fib.top_vlanid_1 = single_tag_action->new_outer_vlan;
        fib_mask.s.dot1p_valid_1 = 1;
        cls_fib.dot1p_valid_1 = TRUE;
        fib_mask.s.dot1p_1 = 1;
        cls_fib.dot1p_1       = single_tag_action->new_outer_pri;
    }
    fib_mask.s.top_sc_ind_1 = 1;
    cls_fib.top_sc_ind_1 = (port_control.outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN);
    fib_mask.s.inner_cmd_valid_1 = 1;
    cls_fib.inner_cmd_valid_1 = TRUE;
    fib_mask.s.inner_vlan_cmd_1 = 1;
    cls_fib.inner_vlan_cmd_1 = single_tag_action->inner_vlan_cmd;
    if (single_tag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) {
        fib_mask.s.inner_vlanid_1 = 1;
        cls_fib.inner_vlanid_1 = single_tag_action->new_inner_vlan;
    }

     fib_mask.s.spt_mode_0 =1 ;
     fib_mask.s.spt_mode_1 =1 ;
     cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
     cls_fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;

    ret = aal_l2_cls_egr_fib_set(device_id, entry_id,fib_mask,&cls_fib);
    if (ret != CA_E_OK) {
        goto end;
    }
    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    cls_mask.u64 = 0;
    fib_mask.u64 = 0;
    cls_key.valid = 1;
   // cls_key.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
    //cls_key.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    //cls_key.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    cls_mask.s.valid       = 1;
   // cls_mask.s.entry_type  = 1;
    //cls_mask.s.rule_type_0 = 1;
    //cls_mask.s.rule_type_1 = 1;

    cls_key.vlan_number_vld_0 = 1;
    cls_key.vlan_number_0 = 2;
    cls_mask.s.vlan_number_vld_0   = 1;
    cls_mask.s.vlan_number_0       = 1;

    cls_key.ethernet_type_vld_0     = 0;
    cls_key.ethernet_type_vld_1     = 0;
    cls_key.l4_port_field_ctrl_0    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.l4_port_field_ctrl_1    = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_key.mac_field_ctrl_0        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.mac_field_ctrl_1        = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_key.vlan_field_ctrl_0       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.vlan_field_ctrl_1       = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_key.dot1p_ctrl_0            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_key.dot1p_ctrl_1            = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_mask.s.ethernet_type_vld_0     = 1 ;
    cls_mask.s.ethernet_type_vld_1     = 1 ;
    cls_mask.s.l4_port_field_ctrl_0    = 1 ;
    cls_mask.s.l4_port_field_ctrl_1    = 1 ;
    cls_mask.s.mac_field_ctrl_0        = 1 ;
    cls_mask.s.mac_field_ctrl_1        = 1 ;
    cls_mask.s.vlan_field_ctrl_0       = 1 ;
    cls_mask.s.vlan_field_ctrl_1       = 1 ;
    cls_mask.s.dot1p_ctrl_0            = 1 ;
    cls_mask.s.dot1p_ctrl_1            = 1 ;

    entry_id = logic_port_id* __INGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_DTAG_ENTRY_ID;
    ret = aal_l2_cls_egr_l2rl2r_key_set(device_id, entry_id, cls_mask, &cls_key);
    if (ret != CA_E_OK) {
        goto end;
    }

   fib_mask.s.top_cmd_valid_0 = 1;
    cls_fib.top_cmd_valid_0 = TRUE;
    fib_mask.s.top_vlan_cmd_0 = 1;
    cls_fib.top_vlan_cmd_0 = double_tag_action->outer_vlan_cmd;

    if ((double_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
            (double_tag_action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        fib_mask.s.top_vlanid_0 = 1;
        cls_fib.top_vlanid_0 = double_tag_action->new_outer_vlan;
        fib_mask.s.dot1p_valid_0 = 1;
        cls_fib.dot1p_valid_0 = TRUE;
        fib_mask.s.dot1p_0 = 1;
        cls_fib.dot1p_0       = double_tag_action->new_outer_pri;
    }
    fib_mask.s.top_sc_ind_0 = 1;
    cls_fib.top_sc_ind_0 = (port_control.outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN);
    fib_mask.s.inner_cmd_valid_0 = 1;
    cls_fib.inner_cmd_valid_0 = TRUE;
    fib_mask.s.inner_vlan_cmd_0 = 1;
    cls_fib.inner_vlan_cmd_0 = double_tag_action->inner_vlan_cmd;
    if ((double_tag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
            (double_tag_action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
        fib_mask.s.inner_vlanid_0 = 1;
        cls_fib.inner_vlanid_0 = double_tag_action->new_inner_vlan;
    }

    fib_mask.s.spt_mode_0 =1 ;
    cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;

    ret = aal_l2_cls_egr_fib_set(device_id, entry_id,fib_mask,&cls_fib);
end:
    return ret;

}

ca_status_t ca_l2_vlan_egress_default_action_get(
    CA_IN       ca_device_id_t             device_id,
    CA_IN       ca_port_id_t            port_id,
    CA_OUT      ca_vlan_action_t        *default_action,
    CA_OUT      ca_vlan_action_t        *untag_action,
    CA_OUT      ca_vlan_action_t        *single_tag_action,
    CA_OUT      ca_vlan_action_t        *double_tag_action
)
{
    aal_l2_vlan_port_tx_tpid_sel_t tpid_cfg;
    aal_l2_vlan_default_cfg_t  vlan_cfg;
    ca_uint32_t outer_tpid_index = 0;
    ca_uint32_t inner_tpid_index = 0;
    aal_l2_cls_fib_t        cls_fib;
    ca_status_t             ret = CA_E_OK;
    ca_uint16_t             entry_id;
    ca_uint16_t             logic_port_id;

    ca_uint64_t             mc_fib_bmp = 0;
    ca_uint32_t             mc_fib_id = 0;
    aal_arb_mc_fib_t                mc_fib;

    if ((NULL == default_action) || (NULL == untag_action) || (NULL == single_tag_action) || (NULL == double_tag_action)) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    memset(&tpid_cfg, 0, sizeof(aal_l2_vlan_port_tx_tpid_sel_t));
    memset(&mc_fib, 0, sizeof(aal_arb_mc_fib_t));
    ret = aal_l2_vlan_tx_tpid_sel_get(device_id, logic_port_id, &tpid_cfg);
    if (ret != CA_E_OK) {
        ca_printf("aal_l2_vlan_tx_tpid_sel_get failed. ret %d\r\n", ret);
        goto end;
    }
    ret = __l2_vlan_non_uc_dpid_fib_index_search(device_id, logic_port_id,&mc_fib_bmp);
    if (ret != CA_E_OK) {
        goto end;
    }

    if(mc_fib_bmp != 0){
        for(mc_fib_id = 0; mc_fib_id<64; mc_fib_id++){
            if((mc_fib_bmp>>mc_fib_id) & 1){
                ret = aal_arb_mc_fib_get(device_id,mc_fib_id,&mc_fib);
                if(ret != CA_E_OK){
                    goto end;
                }
                if(mc_fib.dot1p_cmd == AAL_ARB_MC_DOT1P_CMD_KEEP_OLD){
                    default_action->new_outer_pri_src = CA_L2_VLAN_TAG_PRI_SRC_OUTER_PRI;
                }else if(mc_fib.dot1p_cmd == AAL_ARB_MC_DOT1P_CMD_USE_THIS_TABLE){
                    default_action->new_outer_pri_src = CA_L2_VLAN_TAG_PRI_SRC_NEW_PRI;
                    default_action->new_outer_pri = mc_fib.dot1p;
                }else if(mc_fib.dot1p_cmd == AAL_ARB_MC_DOT1P_CMD_NO_IP_USE_EGRESS_TABLE){
                    default_action->new_outer_pri_src = CA_L2_VLAN_TAG_PRI_SRC_8021P_REMARK_TABLE;
                }else{
                    default_action->new_outer_pri_src = CA_L2_VLAN_TAG_PRI_SRC_DSCP_TO_8021P_TABLE;
                }
                default_action->outer_vlan_cmd = mc_fib.vlan_cmd;
                default_action->new_outer_vlan = mc_fib.vid;
                break;
            }
        }
    }

    if(tpid_cfg.svlan == AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_A){
        default_action->new_inner_tpid_src = CA_L2_VLAN_TPID_LOCAL;
        default_action->new_outer_tpid_src = CA_L2_VLAN_TPID_LOCAL;

        __l2_vlan_inner_tpid_value2index(0x8100, &inner_tpid_index);
        __l2_vlan_outer_tpid_value2index(0x8100, &outer_tpid_index);
        default_action->new_inner_tpid_index= inner_tpid_index;
        default_action->new_outer_tpid_index = outer_tpid_index;

    }else if(tpid_cfg.svlan == AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_B){
        default_action->new_inner_tpid_src = CA_L2_VLAN_TPID_LOCAL;
        default_action->new_outer_tpid_src = CA_L2_VLAN_TPID_LOCAL;

        memset(&vlan_cfg, 0, sizeof(aal_l2_vlan_default_cfg_t));
        ret = aal_l2_vlan_default_cfg_get(device_id, &vlan_cfg);
        if (ret != CA_E_OK) {
            ca_printf("aal_l2_vlan_default_cfg_get failed. ret %d\r\n", ret);
            goto end;
        }

        __l2_vlan_inner_tpid_value2index(vlan_cfg.tx_tpid1, &inner_tpid_index);
        __l2_vlan_outer_tpid_value2index(vlan_cfg.tx_tpid1, &outer_tpid_index);
        default_action->new_inner_tpid_index= inner_tpid_index;
        default_action->new_outer_tpid_index = outer_tpid_index;

    }else if(tpid_cfg.svlan == AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A){
        default_action->new_inner_tpid_src = CA_L2_VLAN_TPID_INNER_TAG;
        default_action->new_outer_tpid_src = CA_L2_VLAN_TPID_OUTER_TAG;
    }

    entry_id = logic_port_id * __EGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_UTAG_STAG_ENTRY_ID;
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    ret = aal_l2_cls_egr_fib_get(device_id, entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        goto end;
    }
    if (cls_fib.top_cmd_valid_0) {
        untag_action->outer_vlan_cmd = cls_fib.top_vlan_cmd_0;
        untag_action->new_outer_vlan = cls_fib.top_vlanid_0;
    }
    if (cls_fib.dot1p_valid_0) {
        untag_action->new_outer_pri = cls_fib.dot1p_0;
    }
    if (cls_fib.inner_cmd_valid_0) {
        untag_action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_0;
        untag_action->new_inner_vlan = cls_fib.inner_vlanid_0;
    }
    if (cls_fib.top_cmd_valid_1) {
        single_tag_action->outer_vlan_cmd = cls_fib.top_vlan_cmd_1;
        single_tag_action->new_outer_vlan = cls_fib.top_vlanid_1;
    }

    if (cls_fib.dot1p_valid_1) {
        single_tag_action->new_outer_pri = cls_fib.dot1p_1;
    }
    if (cls_fib.inner_cmd_valid_1) {
        single_tag_action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_1;
        single_tag_action->new_inner_vlan = cls_fib.inner_vlanid_1;
    }


    entry_id = logic_port_id* __EGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_DTAG_ENTRY_ID;
    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    ret = aal_l2_cls_egr_fib_get(device_id, entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        goto end;
    }

    if (cls_fib.top_cmd_valid_0) {
        double_tag_action->outer_vlan_cmd = cls_fib.top_vlan_cmd_0;
        double_tag_action->new_outer_vlan = cls_fib.top_vlanid_0;
    }
    if (cls_fib.dot1p_valid_0) {
        double_tag_action->new_outer_pri = cls_fib.dot1p_0;
    }
    if (cls_fib.inner_cmd_valid_0) {
        double_tag_action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_0;
        double_tag_action->new_inner_vlan = cls_fib.inner_vlanid_0;
    }
end:
    return ret;

}


ca_status_t ca_l2_vlan_egress_action_add(
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t *key,
    CA_IN ca_vlan_action_t   *action
)
{
    aal_l2_cls_l2rl2r_key_t cls_key;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_fib_t        cls_fib;
    aal_l2_cls_fib_mask_t        fib_mask;
    __l2_vlan_action_t      *temp_l2_vlan_action = NULL;
    //__l2_vlan_action_t       vlan_action;
    ca_uint8_t              cls_entry_id        = 0;
    ca_uint8_t              cls_entry_offset    = 0;
    ca_status_t              ret = CA_E_OK;
    ca_uint32_t                 logic_port_id;
    ca_vlan_port_control_t   port_control;
    ca_vlan_key_type_t       key_type;
    ca_uint32_t              inner_vid = 0;
    ca_uint32_t              outer_vid = 0;
    ca_uint8_t              inner_pbits = 0;
    ca_uint8_t              outer_pbits = 0;

    if ((NULL == action)||(NULL == key)) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset(&port_control, 0, sizeof(ca_vlan_port_control_t));
    memset(&key_type , 0,sizeof(ca_vlan_key_type_t));

    fib_mask.u64 = 0;
    cls_mask.u64 = 0;

    memcpy(&key_type,&(key->key_type),sizeof(ca_vlan_key_type_t));
    inner_vid = key->inner_vid;
    outer_vid = key->outer_vid;
    inner_pbits = key->inner_pbits;
    outer_pbits = key->outer_pbits;

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
    if(logic_port_id != wan_port_id){
#endif
        //LAN port
        if (__l2_vlan_egress_action_record_find(logic_port_id, key_type, outer_vid, inner_vid, &temp_l2_vlan_action) == 0) {
            ret = __l2_vlan_egr_cls_entry_get(logic_port_id, &cls_entry_id, &cls_entry_offset);
            if (ret != CA_E_OK) {
                L2_DEBUG("__l2_vlan_egr_cls_entry_get() failed\n");
                return ret;
            }
        } else {
            cls_entry_id = temp_l2_vlan_action->entry_id;
            cls_entry_offset = temp_l2_vlan_action->entry_offset;
        }
        printk("\nfunction %s entry_id = %d, entry_offset = %d\n", __FUNCTION__, cls_entry_id, cls_entry_offset);
        ret = aal_l2_cls_egr_l2rl2r_key_get(device_id, cls_entry_id, &cls_key);
        if (ret != CA_E_OK) {
            L2_DEBUG("aal_l2_cls_egr_l2rl2r_key_get() failed\n");
            return ret;
        }

        ret = ca_l2_vlan_port_control_get(device_id,port_id,&port_control);
        if (ret != CA_E_OK) {
            L2_DEBUG("ca_l2_vlan_port_control_get() failed\n");
            return ret;
        }

        ret = __l2_vlan_l2rl2r_key_get(device_id,cls_entry_offset,port_control.outer_tpid,port_control.inner_tpid,key,&cls_key,&cls_mask);
        if(ret != CA_E_OK){
            L2_DEBUG("__l2_vlan_l2rl2r_key_get() failed \n");
            return ret;
        }

        ret = aal_l2_cls_egr_l2rl2r_key_set(device_id, cls_entry_id, cls_mask, &cls_key);
        if (ret != CA_E_OK) {
            L2_DEBUG("aal_l2_cls_egr_l2rl2r_key_set() failed\n");
            return ret;
        }


       if (cls_entry_offset == 0) {
             fib_mask.s.top_cmd_valid_0 = 1;
             cls_fib.top_cmd_valid_0 = TRUE;
             fib_mask.s.top_vlan_cmd_0 = 1;
             fib_mask.s.top_sc_ind_0 = 1;
             cls_fib.top_sc_ind_0 = (port_control.outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN);
             cls_fib.top_vlan_cmd_0 = action->outer_vlan_cmd;
             fib_mask.s.inner_cmd_valid_0 = 1;
             cls_fib.inner_cmd_valid_0 = TRUE;
             fib_mask.s.inner_vlan_cmd_0 = 1;
             cls_fib.inner_vlan_cmd_0 = action->inner_vlan_cmd;
             fib_mask.s.spt_mode_0 =1 ;
             cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
            fib_mask.s.top_sc_ind_0 = 1;
            cls_fib.top_sc_ind_0 = port_control.outer_tpid;
            fib_mask.s.inner_sc_ind_0 = 1;
            cls_fib.inner_sc_ind_0 = port_control.inner_tpid;
         } else {
             fib_mask.s.top_cmd_valid_1 = 1;
             cls_fib.top_cmd_valid_1 = TRUE;
             fib_mask.s.top_vlan_cmd_1 = 1;
             fib_mask.s.top_sc_ind_1 = 1;
             cls_fib.top_sc_ind_1 = (port_control.outer_tpid == CA_L2_VLAN_TAG_TYPE_SVLAN);
             cls_fib.top_vlan_cmd_1 = action->outer_vlan_cmd;
             fib_mask.s.inner_cmd_valid_1 = 1;
             cls_fib.inner_cmd_valid_1 = TRUE;
             fib_mask.s.inner_vlan_cmd_1 = 1;
             cls_fib.inner_vlan_cmd_1 = action->inner_vlan_cmd;
             fib_mask.s.spt_mode_1 =1 ;
             cls_fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
             fib_mask.s.top_sc_ind_1 = 1;
             cls_fib.top_sc_ind_1 = port_control.outer_tpid;
             fib_mask.s.inner_sc_ind_1 = 1;
             cls_fib.inner_sc_ind_1 = port_control.inner_tpid;
         }

         if ((action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
                 (action->outer_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
             if (cls_entry_offset == 0) {
                 fib_mask.s.top_vlanid_0 = 1;
                 cls_fib.top_vlanid_0 = action->new_outer_vlan;
                 fib_mask.s.dot1p_valid_0 = 1;
                 cls_fib.dot1p_valid_0 = TRUE;
                 fib_mask.s.dot1p_0 = 1;
                 cls_fib.dot1p_0       = action->new_outer_pri;
             } else {
                 fib_mask.s.top_vlanid_1 = 1;
                 cls_fib.top_vlanid_1 = action->new_outer_vlan;
                 fib_mask.s.dot1p_valid_1 = 1;
                 cls_fib.dot1p_valid_1 = TRUE;
                 fib_mask.s.dot1p_1 = 1;
                 cls_fib.dot1p_1       = action->new_outer_pri;
             }
         }


         if ((action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_PUSH) ||
                  (action->inner_vlan_cmd == CA_L2_VLAN_TAG_ACTION_SWAP)) {
              if (cls_entry_offset == 0) {
                  fib_mask.s.inner_vlanid_0 = 1;
                  cls_fib.inner_vlanid_0 = action->new_inner_vlan;

              } else {
                  fib_mask.s.inner_vlanid_1 = 1;
                  cls_fib.inner_vlanid_1 = action->new_inner_vlan;
              }
          }

        ret = aal_l2_cls_egr_fib_set(device_id, cls_entry_id,fib_mask, &cls_fib);
        if (ret != CA_E_OK) {
            return ret;
        }
        temp_l2_vlan_action = (__l2_vlan_action_t *)ca_malloc(sizeof(__l2_vlan_action_t));
        if(temp_l2_vlan_action == NULL){
            return CA_E_RESOURCE;
        }
        memset(temp_l2_vlan_action, 0, sizeof(__l2_vlan_action_t));
        temp_l2_vlan_action->key_type = key_type;
        temp_l2_vlan_action->inner_vid = inner_vid;
        temp_l2_vlan_action->outer_vid = outer_vid;
        temp_l2_vlan_action->entry_id = cls_entry_id;
        temp_l2_vlan_action->entry_offset = cls_entry_offset;
        temp_l2_vlan_action->new_outer_pri_src = action->new_outer_pri_src;
        temp_l2_vlan_action->new_outer_vlan_src = action->new_outer_vlan_src ;
        temp_l2_vlan_action->inner_pbits = inner_pbits;
        temp_l2_vlan_action->outer_pbits = outer_pbits;

        if (__l2_vlan_egress_action_record_add(logic_port_id, temp_l2_vlan_action) != CA_E_OK) {
            L2_DEBUG("__l2_vlan_egress_action_record_add() failed\n");
            ret = CA_E_RESOURCE;
        }
        __g_l2_vlan_egr_cls_entry_occupy[logic_port_id] |= (1 << (((cls_entry_id - logic_port_id*__EGRESS_CLS_ENTRY_NUM_PER_PORT) << 1) | cls_entry_offset));

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
    }else {
        //WAN port
        ret = CA_E_NOT_SUPPORT;
        L2_DEBUG("Not Support for WAN Port until now, so return error\n");
    }
#endif

    return ret;
}


ca_status_t ca_l2_vlan_egress_action_delete(
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t key
)
{
    aal_l2_cls_l2rl2r_key_t cls_key;
    aal_l2_cls_fib_t        cls_fib;
    aal_l2_cls_fib_mask_t        fib_mask;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    __l2_vlan_action_t      *temp_l2_vlan_action = NULL;
    ca_uint8_t              cls_entry_id        = 0;
    ca_uint8_t              cls_entry_offset    = 0;
    ca_status_t              ret = CA_E_OK;
    ca_uint32_t                 logic_port_id;
    ca_vlan_key_type_t       key_type;
    ca_uint32_t              inner_vid = 0;
    ca_uint32_t              outer_vid = 0;
    ca_uint8_t              inner_pbits = 0;
    ca_uint8_t              outer_pbits = 0;

    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    memset(&key_type , 0,sizeof(ca_vlan_key_type_t));

    memcpy(&key_type,&key.key_type,sizeof(ca_vlan_key_type_t));
    inner_vid = key.inner_vid;
    outer_vid = key.outer_vid;
    inner_pbits = key.inner_pbits;
    outer_pbits = key.outer_pbits;

    if(key_type.outer_vid){
        if (outer_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }

    if(key_type.inner_vid){
        if (inner_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }

    if(key_type.outer_pbits){
        if (outer_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if(key_type.inner_pbits){
        if (inner_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if (__l2_vlan_egress_action_record_find(logic_port_id, key_type, outer_vid, inner_vid, &temp_l2_vlan_action) == 0) {
        return CA_E_NOT_FOUND;
    } else {
        cls_entry_id = temp_l2_vlan_action->entry_id;
        cls_entry_offset = temp_l2_vlan_action->entry_offset;
    }

    ret = aal_l2_cls_egr_l2rl2r_key_get(device_id, cls_entry_id, &cls_key);
    if (ret != CA_E_OK) {
        return ret;
    }
    /*
    ret = aal_l2_cls_l2rl2r_key_del(device_id,cls_entry_id);
    if (ret != CA_E_OK) {
        return ret;
    }
    */

    if (cls_entry_offset == 0) {
        cls_mask.u64 = 0;
        cls_mask.s.vlan_field_ctrl_0 = 1;
        cls_key.vlan_field_ctrl_0 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
        cls_mask.s.dot1p_ctrl_0 = 1;
        cls_key.dot1p_ctrl_0 = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        cls_mask.s.vlan_number_vld_0 = 1;
        cls_key.vlan_number_vld_0 = FALSE;

    } else {
        cls_mask.u64 = 0;
        cls_mask.s.vlan_field_ctrl_1 = 1;
        cls_key.vlan_field_ctrl_1 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
        cls_mask.s.dot1p_ctrl_1 = 1;
        cls_key.dot1p_ctrl_1 = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
        cls_mask.s.vlan_number_vld_1 = 1;
        cls_key.vlan_number_vld_1 = FALSE;
    }

    ret = aal_l2_cls_l2rl2r_key_set(device_id, cls_entry_id, cls_mask, &cls_key);
    if (ret != CA_E_OK) {
        return ret;
    }

    ret = aal_l2_cls_egr_fib_get(device_id, cls_entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        return ret;
    }

    fib_mask.u64 = 0;
    if (cls_entry_offset == 0) {
        fib_mask.s.top_cmd_valid_0 = 1;
        cls_fib.top_cmd_valid_0 = FALSE;
        fib_mask.s.inner_cmd_valid_0 = 1;
        cls_fib.inner_cmd_valid_0 = FALSE;
    } else {
        fib_mask.s.top_cmd_valid_1 = 1;
        cls_fib.top_cmd_valid_1 = FALSE;
        fib_mask.s.inner_cmd_valid_1 = 1;
        cls_fib.inner_cmd_valid_1 = FALSE;
    }

    ret = aal_l2_cls_egr_fib_set(device_id, cls_entry_id,fib_mask,&cls_fib);
    if (ret != CA_E_OK) {
        return ret;
    }
    __l2_vlan_egress_action_record_delete(logic_port_id, key_type, outer_vid, inner_vid);
    __g_l2_vlan_egr_cls_entry_occupy[logic_port_id] &= ~(1 << (((cls_entry_id - logic_port_id*__EGRESS_CLS_ENTRY_NUM_PER_PORT) << 1) | cls_entry_offset));

    return ret;
}

ca_status_t ca_l2_vlan_egress_action_delete_all (
	CA_IN		ca_device_id_t				device_id,
	CA_IN		ca_port_id_t			port_id
)
{

	ca_status_t         ret = CA_E_OK;
	 __l2_vlan_action_t   *ptemp = NULL;
	ca_uint32_t         logic_port_id = 0;
    ca_vlan_key_entry_t  key_entry;

    memset(&key_entry,0,sizeof(ca_vlan_key_entry_t));

	CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

	ptemp = __port_vlan_egress_act_head[logic_port_id].next;

	while(ptemp)
	{
        memcpy(&key_entry.key_type,&(ptemp->key_type),sizeof(ca_vlan_key_type_t));
        key_entry.inner_vid = ptemp->inner_vid;
        key_entry.outer_vid = ptemp->outer_vid;

        ptemp = ptemp->next;

		ret = ca_l2_vlan_egress_action_delete(device_id ,port_id,key_entry);
		if (ret != CA_E_OK) {
        	return ret;
    	}
	}

	return CA_E_OK;

}

ca_status_t ca_l2_vlan_egress_action_get(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_port_id_t       port_id,
    CA_IN  ca_vlan_key_entry_t key,
    CA_OUT ca_vlan_action_t   *action
)
{
    aal_l2_cls_fib_t        cls_fib;
    __l2_vlan_action_t      *temp_l2_vlan_action = NULL;
    ca_uint8_t              cls_entry_id        = 0;
    ca_uint8_t              cls_entry_offset    = 0;
    ca_status_t              ret = CA_E_OK;
    ca_uint32_t              logic_port_id;
    ca_vlan_key_type_t       key_type;
    ca_uint32_t              inner_vid = 0;
    ca_uint32_t              outer_vid = 0;
    ca_uint8_t              inner_pbits = 0;
    ca_uint8_t              outer_pbits = 0;

    if (NULL == action) {
        return CA_E_PARAM;
    }
    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    memset(&key_type , 0,sizeof(ca_vlan_key_type_t));

    memcpy(&key_type,&key.key_type,sizeof(ca_vlan_key_type_t));
    inner_vid = key.inner_vid;
    outer_vid = key.outer_vid;
    inner_pbits = key.inner_pbits;
    outer_pbits = key.outer_pbits;

     {
    if(key_type.outer_vid){
        if (outer_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }

    if(key_type.inner_vid)
        if (inner_vid > __MAX_VLAN_ID) {
            return CA_E_PARAM;
        }
    }

    if(key_type.outer_pbits){
        if (outer_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if(key_type.inner_pbits){
        if (inner_pbits > __MAX_PBITS_VALUE) {
            return CA_E_PARAM;
        }
    }

    if (__l2_vlan_egress_action_record_find(logic_port_id, key_type, outer_vid, inner_vid, &temp_l2_vlan_action) == 0) {
        return CA_E_NOT_FOUND;
    } else {
        cls_entry_id = temp_l2_vlan_action->entry_id;
        cls_entry_offset = temp_l2_vlan_action->entry_offset;
    }

    ret = aal_l2_cls_egr_fib_get(device_id, cls_entry_id, &cls_fib);
    if (ret != CA_E_OK) {
        return ret;
    }
    if (cls_entry_offset == 0) {
        if (cls_fib.top_cmd_valid_0) {
            action->outer_vlan_cmd = cls_fib.top_vlan_cmd_0;
            action->new_outer_vlan = cls_fib.top_vlanid_0;
        }
        if (cls_fib.dot1p_valid_0) {
            //For egress action ,not ssupport new_outer_pri & new_inner_pri from Configure guide doc
            //action->new_outer_pri = cls_fib.dot1p_0;
        }
        if (cls_fib.inner_cmd_valid_0) {
            action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_0;
            action->new_inner_vlan = cls_fib.inner_vlanid_0;
        }
        if(cls_fib.mark_ena_0){
            action->flow_id = cls_fib.flowid_0;
        }
    } else {
        if (cls_fib.top_cmd_valid_1) {
            action->outer_vlan_cmd = cls_fib.top_vlan_cmd_1;
            action->new_outer_vlan = cls_fib.top_vlanid_1;
        }
        if (cls_fib.dot1p_valid_1) {
            //For egress action ,not ssupport new_outer_pri & new_inner_pri from Configure guide doc
            //action->new_outer_pri = cls_fib.dot1p_1;
        }
        if (cls_fib.inner_cmd_valid_1) {
            action->inner_vlan_cmd = cls_fib.inner_vlan_cmd_1;
            action->new_inner_vlan = cls_fib.inner_vlanid_1;
        }
        if(cls_fib.mark_ena_1){
            action->flow_id = cls_fib.flowid_1;
        }

    }
    action->new_outer_pri_src = temp_l2_vlan_action->new_outer_pri_src;
    action->new_outer_vlan_src= temp_l2_vlan_action->new_outer_vlan_src;

    return CA_E_OK;
}

ca_status_t ca_l2_vlan_egress_action_iterate (
    CA_IN		ca_device_id_t					device_id,
    CA_IN		ca_port_id_t				port_id,
    CA_IN_OUT	ca_iterator_t				*p_return_entry)
{
    ca_uint32_t    logic_port_id = 0;
    ca_uint32_t    i = 0,j = 0;
    __l2_vlan_action_t   *ptemp = NULL;
    ca_vlan_action_iterate_entry_t ca_vlan_action_iterate_entry;
    ca_vlan_key_entry_t  key_entry;

    CA_ASSERT_RET(p_return_entry != NULL && p_return_entry->p_entry_data != NULL, CA_E_PARAM);
    CA_ASSERT_RET(p_return_entry->entry_count, CA_E_PARAM);
    CA_ASSERT_RET(p_return_entry->entry_count * sizeof(ca_vlan_action_iterate_entry_t) <= p_return_entry->user_buffer_size, CA_E_UNAVAIL);

    memset(&ca_vlan_action_iterate_entry,0,sizeof(ca_vlan_action_iterate_entry_t));
    memset(&key_entry,0,sizeof(ca_vlan_key_entry_t));

    CA_PORT_ID_CHECK(port_id);
    logic_port_id = PORT_ID(port_id);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
     if(logic_port_id < 3){
        L2_DEBUG("logic_port_id < 3");
        return CA_E_PARAM;
     }
     logic_port_id -= 3;
#endif

    if(NULL == p_return_entry->p_prev_handle){
        ptemp = __port_vlan_egress_act_head[logic_port_id].next;
     }else{
        ptemp =  p_return_entry->p_prev_handle;
        if(NULL == ptemp->next)
            return CA_E_UNAVAIL;
     }

    while(ptemp &&(i < p_return_entry->entry_count)){

        ca_vlan_action_iterate_entry.key.key_type= ptemp->key_type;
        ca_vlan_action_iterate_entry.key.inner_vid = ptemp->inner_vid;
        ca_vlan_action_iterate_entry.key.outer_vid = ptemp->outer_vid;
        ca_vlan_action_iterate_entry.key.inner_pbits = ptemp->inner_pbits;
        ca_vlan_action_iterate_entry.key.outer_pbits = ptemp->outer_pbits;

        key_entry.key_type  = ptemp->key_type;
        key_entry.inner_vid = ptemp->inner_vid;
        key_entry.outer_vid = ptemp->outer_vid;
        key_entry.inner_pbits = ptemp->inner_pbits;
        key_entry.outer_pbits = ptemp->outer_pbits;

        ca_l2_vlan_egress_action_get(device_id,port_id,key_entry,&ca_vlan_action_iterate_entry.action);
        memcpy(p_return_entry->p_entry_data + i*sizeof(ca_vlan_action_iterate_entry_t),\
                &ca_vlan_action_iterate_entry,sizeof(ca_vlan_action_iterate_entry_t));
        p_return_entry->entry_index[j++] = ptemp->entry_id;
        p_return_entry->p_prev_handle = ptemp;
        ptemp = ptemp->next;
        i++;
    }

    p_return_entry->entry_count = i;

    if( 0 == i){
        p_return_entry->p_prev_handle = NULL;
        return CA_E_UNAVAIL;
     }else{
        return CA_E_OK;
     }

}

ca_status_t ca_l2_vlan_traverse(
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_l2_vlan_traverse_cb trav_fn,
    CA_IN void                   *user_data
)
{
    return CA_E_OK;
}

ca_status_t __ca_l2_vlan_flooding_domain_fib_add(
    ca_device_id_t             device_id,
    ca_int16_t              mc_group_id,
    ca_vlan_id_t            ingress_vid,
    ca_flooding_port_t *    member,
    ca_vlan_id_t            dst_port_id,
    ca_uint64_t *           mc_fib_idx_bmp)
 {
    ca_status_t             ret = CA_E_OK;
    ca_uint32_t             mc_fib_id = 0;
    aal_arb_mc_fib_mask_t   mc_fib_mask;
    aal_arb_mc_fib_t        mc_fib;
    memset(&mc_fib_mask,0,sizeof(aal_arb_mc_fib_mask_t));
    memset(&mc_fib,0,sizeof( aal_arb_mc_fib_t));

    mc_fib_id = aal_flooding_fib_idx_alloc();
    if (mc_fib_id < 0){
        return CA_E_RESOURCE;
    }

    *mc_fib_idx_bmp |= (ca_uint64_t)(((ca_uint64_t)1)<<mc_fib_id);

    ret = aal_flooding_mc_fib_add(mc_group_id, mc_fib_id);
    L2_DEBUG("\t mc_group_id %d add mc_fib_id %d dst_port_id %d\n", mc_group_id, mc_fib_id, dst_port_id);
    if (ret != CA_E_OK) {
        aal_flooding_fib_idx_free(mc_fib_id);
    return ret;
    }

    mc_fib_mask.u32 = 0;
    mc_fib.ldpid = dst_port_id;
    mc_fib_mask.s.ldpid = 1;
#ifdef  MC_FIB_COS_CMD
    mc_fib_mask.s.cos_cmd = 1;
    mc_fib.cos_cmd = 1;
#endif
    /*
    * If egress_vid ==
    *    CA_UINT32_INVALID: keep the original vlan id
    *    0                              POP the vlan id
    *    others:                      SWAP
    */
    if (member->egress_vid != CA_UINT16_INVALID) {
        if(member->egress_vid != 0){
            mc_fib.vid = member->egress_vid;
            mc_fib_mask.s.vid = 1;
            mc_fib.vlan_cmd = AAL_ARB_MC_VLAN_CMD_SWAP;
        }else{
            mc_fib.vlan_cmd = AAL_ARB_MC_VLAN_CMD_POP;
        }
    }else{
        mc_fib.vlan_cmd = AAL_ARB_MC_VLAN_CMD_NOP;
    }

    mc_fib_mask.s.vlan_cmd = 1;
    ret = aal_arb_mc_fib_set(device_id,(ca_uint32_t)mc_fib_id,mc_fib_mask,&mc_fib);
    return ret;
}

ca_status_t ca_l2_vlan_flooding_domain_members_update (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_vlan_id_t        ingress_vid,
    CA_IN       ca_uint8_t          ingress_port_type,
    CA_IN       ca_uint8_t          member_count,
    CA_IN       ca_flooding_port_t  members[])
{
    ca_status_t   ret = CA_E_OK;

    aal_l2_vlan_fib_map_t           vlan_fib_map;
    ca_uint32_t                     logic_port_id;
    ca_uint8              i                = 0;
    ca_uint16_t           temp_mem         = 0;
    ca_uint64_t           mc_fib_idx_bmp      = 0;
    ca_int16_t            mc_group_id       = 0;

    __l2_vlan_id_item_t   *vlan_item_bmp   = NULL;
    aal_l2_vlan_type_t      vlan_type;

    memset(&vlan_fib_map,0,sizeof(aal_l2_vlan_fib_map_t));

    CA_ASSERT_RET((CA_L2_ING_PORT_TYPE_WAN == ingress_port_type || CA_L2_ING_PORT_TYPE_LAN == ingress_port_type),CA_E_PARAM);
    CA_ASSERT_RET(ingress_vid<=__MAX_VLAN_ID,CA_E_PARAM);

    if (CA_L2_ING_PORT_TYPE_WAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_WAN_SVLAN;
    } else	if (CA_L2_ING_PORT_TYPE_LAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_LAN_VLAN;
    } else
        return CA_E_PARAM;

    ret = aal_l2_vlan_fib_map_get(device_id,vlan_type,ingress_vid, &vlan_fib_map);
    if(CA_E_OK != ret){
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }

    if(vlan_fib_map.valid == FALSE){
        ret = CA_E_NOT_FOUND;
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }

    L2_DEBUG("Update %s ======= \n",
        (CA_L2_ING_PORT_TYPE_LAN == ingress_port_type)?"lan":"wan");

    if(__l2_vlan_id_item_find(ingress_vid, &vlan_item_bmp)!= TRUE){
        ret = CA_E_RESOURCE;
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }
    if(ingress_port_type == CA_L2_ING_PORT_TYPE_WAN){
        if(vlan_item_bmp->mc_group_wan_valid){
            mc_group_id =  vlan_item_bmp->mc_group_id_wan;
            mc_fib_idx_bmp = vlan_item_bmp->mc_fib_id_bmp_wan;
        }else{
            mc_group_id = aal_flooding_group_create();
        }
    }else{
        if(vlan_item_bmp->mc_group_lan_valid){
            mc_group_id =  vlan_item_bmp->mc_group_id_lan;
            mc_fib_idx_bmp = vlan_item_bmp->mc_fib_id_bmp_lan;
        }else{
            mc_group_id = aal_flooding_group_create();
        }
    }
    if(mc_group_id < 0){
        ret = CA_E_RESOURCE;
         printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
         goto end;
    }

    L2_DEBUG("\t create mc_group_id is %d\n", mc_group_id);


    for(i = 0; i < __L2_VLAN_MC_FIB_NUM;  i++) {
        if((mc_fib_idx_bmp >> i)&1){
            ret = aal_flooding_mc_fib_delete(mc_group_id,i);
            if(CA_E_OK != ret){
                goto end;
            }
            ret =  aal_flooding_fib_idx_free(i);
            if(CA_E_OK != ret){
                goto end;
            }
        }
    }
    mc_fib_idx_bmp = 0;
    for (temp_mem = 0; temp_mem < member_count; temp_mem++) {
        CA_PORT_ID_CHECK(members[temp_mem].egress_port);
        logic_port_id =  PORT_ID(members[temp_mem].egress_port);
        ret =__ca_l2_vlan_flooding_domain_fib_add(device_id, mc_group_id, ingress_vid,
        &members[temp_mem], logic_port_id, &mc_fib_idx_bmp);
        if (ret != CA_E_OK) {
            goto end;
        }
    }
    if(ingress_port_type == CA_L2_ING_PORT_TYPE_WAN){
        vlan_item_bmp->mc_fib_id_bmp_wan = mc_fib_idx_bmp;
        vlan_item_bmp->mc_group_id_wan = mc_group_id;
        vlan_item_bmp->mc_group_wan_valid = TRUE;
    }else{
        vlan_item_bmp->mc_fib_id_bmp_lan = mc_fib_idx_bmp;
        vlan_item_bmp->mc_group_id_lan = mc_group_id;
        vlan_item_bmp->mc_group_lan_valid = TRUE;
    }


end:
    return ret;

}

ca_status_t ca_l2_vlan_flooding_domain_members_add (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_vlan_id_t        ingress_vid,
    CA_IN       ca_uint8_t          ingress_port_type,
    CA_IN       ca_uint8_t          member_count,
    CA_IN       ca_flooding_port_t  members[])
{
    ca_status_t                 ret = CA_E_OK;
    aal_l2_vlan_fib_map_t       vlan_fib_map;
    aal_l2_vlan_fib_t           vlan_fib;
    ca_uint32_t                 logic_port_id, i;
    __l2_vlan_id_item_t         *vlan_id_item  = NULL;
    aal_l2_vlan_type_t          vlan_type;

    CA_ASSERT_RET((CA_L2_ING_PORT_TYPE_WAN == ingress_port_type || CA_L2_ING_PORT_TYPE_LAN == ingress_port_type),CA_E_PARAM);
    CA_ASSERT_RET(ingress_vid <= __MAX_VLAN_ID,CA_E_PARAM);

    /* don't need to update AAL_L2_VLAN_TYPE_WAN_CVLAN
     * because WAN_SVLAN / LAN_VLAN use the same fib
     */

    if (CA_L2_ING_PORT_TYPE_WAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_WAN_SVLAN;
    } else  if (CA_L2_ING_PORT_TYPE_LAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_LAN_VLAN;
    } else
        return CA_E_PARAM;

    ret = aal_l2_vlan_fib_map_get(device_id,vlan_type,ingress_vid, &vlan_fib_map);
    if(CA_E_OK != ret){
    	goto end;
    }

    if(vlan_fib_map.valid == FALSE){
        ret = CA_E_NOT_FOUND;
        goto end;
    }

    ret = aal_l2_vlan_action_cfg_get(device_id,vlan_type,vlan_fib_map.fib_id,&vlan_fib);
    if(CA_E_OK != ret){
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }
    if(__l2_vlan_id_item_find(ingress_vid, &vlan_id_item)== 0){
        ret = CA_E_NOT_FOUND;
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }

#if 1
    if(ingress_port_type == CA_L2_ING_PORT_TYPE_WAN){
        if (vlan_id_item->mc_group_wan_valid == 0) {
            ret = CA_E_NOT_FOUND;
            printk("%s please call ca_l2_vlan_flooding_domain_members_update() at first!! \n", __func__);
            goto end;
        }
    }else{
        if (vlan_id_item->mc_group_lan_valid == 0) {
            ret = CA_E_NOT_FOUND;
            printk("%s please call ca_l2_vlan_flooding_domain_members_update() at first!! \n", __func__);
            goto end;
        }

    }
#endif

    for (i = 0;  i < member_count; i++) {
        logic_port_id =  PORT_ID(members[i].egress_port);
        if(ingress_port_type == CA_L2_ING_PORT_TYPE_WAN){
            if(vlan_id_item->flood_bmp_wan & (1 << logic_port_id)){
                ret = CA_E_PARAM;
                printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
                goto end;
            }
            ret = __ca_l2_vlan_flooding_domain_fib_add(
                device_id,
                vlan_id_item->mc_group_id_wan,
                ingress_vid,
                &members[i],
                logic_port_id,
                &vlan_id_item->mc_fib_id_bmp_wan);
            if (ret != CA_E_OK){
                L2_DEBUG("__ca_l2_vlan_flooding_domain_fib_add() failed\n");
                goto end;
            }
        }else{
            if(vlan_id_item->flood_bmp_lan & (1 << logic_port_id)){
                ret = CA_E_PARAM;
                goto end;
            }
            ret = __ca_l2_vlan_flooding_domain_fib_add(
                device_id,
                vlan_id_item->mc_group_id_lan,
                ingress_vid,
                &members[i],
                logic_port_id,
                &vlan_id_item->mc_fib_id_bmp_lan);
            if (ret != CA_E_OK){
                L2_DEBUG("__ca_l2_vlan_flooding_domain_fib_add() failed\n");
                goto end;
            }
        }
    }
end:
    return ret;
}

ca_status_t ca_l2_vlan_flooding_domain_members_delete (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_vlan_id_t        ingress_vid,
    CA_IN       ca_uint8_t          ingress_port_type)
{
    ca_status_t             ret            = CA_E_OK;
    aal_l2_vlan_fib_map_t   vlan_fib_map            ;
    aal_l2_vlan_type_t      vlan_type               ;
    ca_uint64_t             mc_fib_bmp              ;
    __l2_vlan_id_item_t     *vlan_id_item  = NULL   ;
    ca_uint32_t             i                       ;
    ca_uint8_t              mc_group_id             ;


    CA_ASSERT_RET((CA_L2_ING_PORT_TYPE_WAN == ingress_port_type || CA_L2_ING_PORT_TYPE_LAN == ingress_port_type),CA_E_PARAM);
    CA_ASSERT_RET(ingress_vid <= __MAX_VLAN_ID,CA_E_PARAM);

    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));
    if (CA_L2_ING_PORT_TYPE_WAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_WAN_SVLAN;
    } else  if (CA_L2_ING_PORT_TYPE_LAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_LAN_VLAN;
    } else{
        return CA_E_PARAM;
    }
    ret = aal_l2_vlan_fib_map_get(device_id,vlan_type,ingress_vid, &vlan_fib_map);
    if(CA_E_OK != ret){
        goto end;
    }
    if(vlan_fib_map.valid == FALSE){
        ret = CA_E_NOT_FOUND;
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }

    if(__l2_vlan_id_item_find(ingress_vid, &vlan_id_item)!= TRUE){
        ret = CA_E_RESOURCE;
        printk("%s %d ret = %d\n",__FILE__,__LINE__,ret);
        goto end;
    }
    mc_group_id = (vlan_type == AAL_L2_VLAN_TYPE_LAN_VLAN) ? vlan_id_item->mc_group_id_lan:vlan_id_item->mc_group_id_wan;
    mc_fib_bmp = (vlan_type == AAL_L2_VLAN_TYPE_LAN_VLAN) ? vlan_id_item->mc_fib_id_bmp_lan: vlan_id_item->mc_fib_id_bmp_wan;

    L2_DEBUG("\t delete %s fib_id %d mc_group_id %d mc_fib_bmp 0x%x\n",
            (vlan_type == AAL_L2_VLAN_TYPE_LAN_VLAN)? "lan":"wan",
            vlan_fib_map.fib_id, mc_group_id,
            mc_fib_bmp);



    for(i = 0; i<64; i++){
        if((mc_fib_bmp >> i)&1){
            aal_flooding_mc_fib_delete(mc_group_id,i);
            ret = aal_flooding_fib_idx_free(i);
            L2_DEBUG("\t free %s mc_group_id %d mc_fib_id %d ret %d %s\n",
            (vlan_type == AAL_L2_VLAN_TYPE_LAN_VLAN)? "lan":"wan",
                mc_group_id, i, ret, (ret != CA_E_OK)? "???":"");
        }
    }

    if(vlan_type == AAL_L2_VLAN_TYPE_LAN_VLAN){
        vlan_id_item->mc_fib_id_bmp_lan = 0 ;
    }else{
        vlan_id_item->mc_fib_id_bmp_wan = 0;
    }
    return CA_E_OK;
 end:
    return ret;
}
ca_status_t ca_l2_vlan_flooding_domain_members_get (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_vlan_id_t        ingress_vid,
    CA_IN       ca_uint8_t          ingress_port_type,
    CA_OUT      ca_uint8_t          *p_member_count,
    CA_OUT      ca_flooding_port_t  members[])
{
    __l2_vlan_id_item_t   *vlan_item_bmp   = NULL;
    ca_status_t             ret             = CA_E_PARAM;
    aal_l2_vlan_fib_map_t   vlan_fib_map;
    ca_uint32_t             i=0;
    aal_l2_vlan_fib_t       vlan_fib;
    ca_uint8_t              p_member_count_tmp = 0;
    aal_arb_mc_fib_t        arb_mc_fib;
    aal_l2_vlan_type_t      vlan_type;

    CA_ASSERT_RET((CA_L2_ING_PORT_TYPE_WAN == ingress_port_type || CA_L2_ING_PORT_TYPE_LAN == ingress_port_type),CA_E_PARAM);
    CA_ASSERT_RET(p_member_count != NULL, CA_E_PARAM);
    CA_ASSERT_RET(ingress_vid <= __MAX_VLAN_ID,CA_E_PARAM);

    memset(&vlan_fib,0,sizeof(aal_l2_vlan_fib_t ));
    memset(&vlan_fib_map,0,sizeof(aal_l2_vlan_fib_map_t));
    memset(&arb_mc_fib,0,sizeof(aal_arb_mc_fib_t));

    if (CA_L2_ING_PORT_TYPE_WAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_WAN_SVLAN;
    } else  if (CA_L2_ING_PORT_TYPE_LAN == ingress_port_type) {
        vlan_type = AAL_L2_VLAN_TYPE_LAN_VLAN;
    } else{
        return CA_E_PARAM;
    }
    ret = aal_l2_vlan_fib_map_get(device_id,vlan_type,ingress_vid, &vlan_fib_map);
    if (CA_E_OK != ret){
        return ret;
    }
    if(vlan_fib_map.valid == FALSE){
        return CA_E_NOT_FOUND;
    }
    if(__l2_vlan_id_item_find(ingress_vid, &vlan_item_bmp)== 0){
        return CA_E_NOT_FOUND;
    }

    if (CA_L2_ING_PORT_TYPE_WAN == ingress_port_type){
        for(i=0;i<64;i++){
            if((vlan_item_bmp->mc_fib_id_bmp_wan>>i)&0x1){
                ret =  aal_arb_mc_fib_get(device_id,i,&arb_mc_fib);
                if(CA_E_OK != ret){
                    return ret;
                }
                if(arb_mc_fib.ldpid >= 0 && arb_mc_fib.ldpid <= 7){
                    members->egress_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,arb_mc_fib.ldpid);
                }else if(arb_mc_fib.ldpid == 16 || arb_mc_fib.ldpid == 17 ||(arb_mc_fib.ldpid >= 19 && arb_mc_fib.ldpid <= 23)){
                    members->egress_port = CA_PORT_ID(CA_PORT_TYPE_CPU,arb_mc_fib.ldpid);
                }else if(arb_mc_fib.ldpid == 24){
                    members->egress_port = CA_PORT_ID(CA_PORT_TYPE_L3,arb_mc_fib.ldpid);
                }else{
                    return CA_E_NOT_FOUND;
                }
                    /*
                        * If egress_vid ==
                        *    CA_UINT32_INVALID: 	keep the original vlan id
                        *    0: 					POP the vlan id
                        *    others: 				SWAP
                        */
                if (arb_mc_fib.vlan_cmd == AAL_ARB_MC_VLAN_CMD_NOP)
                    members->egress_vid = CA_UINT32_INVALID;
                else if (arb_mc_fib.vlan_cmd == AAL_ARB_MC_VLAN_CMD_POP)
                    members->egress_vid = 0;
                else
                    members->egress_vid = arb_mc_fib.vid;

                members++;
                p_member_count_tmp++;
            }
        }
    }else{
        for(i=0;i<64;i++){
            if((vlan_item_bmp->mc_fib_id_bmp_lan>>i)&0x1){
                ret =  aal_arb_mc_fib_get(device_id,i,&arb_mc_fib);
                if(CA_E_OK != ret){
                    return ret;
                }
                if(arb_mc_fib.ldpid >= 0 && arb_mc_fib.ldpid <= 7){
                    members->egress_port = CA_PORT_ID(CA_PORT_TYPE_ETHERNET,arb_mc_fib.ldpid);
                }else if(arb_mc_fib.ldpid == 16 || arb_mc_fib.ldpid == 17 ||(arb_mc_fib.ldpid >= 19 && arb_mc_fib.ldpid <= 23)){
                    members->egress_port = CA_PORT_ID(CA_PORT_TYPE_CPU,arb_mc_fib.ldpid);
                }else if(arb_mc_fib.ldpid == 24){
                    members->egress_port = CA_PORT_ID(CA_PORT_TYPE_L3,arb_mc_fib.ldpid);
                }else{
                    return CA_E_NOT_FOUND;
                }
                    /*
                        * If egress_vid ==
                        *    CA_UINT32_INVALID: 	keep the original vlan id
                        *    0: 					POP the vlan id
                        *    others: 				SWAP
                        */
                if (arb_mc_fib.vlan_cmd == AAL_ARB_MC_VLAN_CMD_NOP)
                    members->egress_vid = CA_UINT32_INVALID;
                else if (arb_mc_fib.vlan_cmd == AAL_ARB_MC_VLAN_CMD_POP)
                    members->egress_vid = 0;
                else
                    members->egress_vid = arb_mc_fib.vid;

                members++;
                p_member_count_tmp++;
            }
        }

    }
    *p_member_count = p_member_count_tmp;

    return CA_E_OK;
}

ca_status_t ca_l2_vlan_action_get (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_vlan_id_t        ingress_vid,
    CA_IN       ca_boolean_t        direction,
    CA_OUT      ca_vlan_action_t    *p_vlan_action)
{
    ca_status_t  ret            = CA_E_OK;
    aal_l2_vlan_fib_map_t       vlan_fib_map;
    aal_l2_vlan_fib_t           vlan_fib_cfg;

    CA_ASSERT_RET((CA_L2_VLAN_ACTION_DIR_LAN_TO_WAN == direction) ||(CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN == direction), CA_E_PARAM);
    CA_ASSERT_RET(p_vlan_action != NULL, CA_E_PARAM);
    CA_ASSERT_RET(ingress_vid <= __MAX_VLAN_ID,CA_E_PARAM);

    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));
    memset(&vlan_fib_cfg, 0, sizeof(aal_l2_vlan_fib_t));

    if(CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN == direction){

        ret = aal_l2_vlan_fib_map_get(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, ingress_vid, &vlan_fib_map);
        if (ret != CA_E_OK) {
            goto end;
        }

        if (vlan_fib_map.valid == 0) {
            ret = CA_E_NOT_FOUND;
            goto end;
        }

        ret = aal_l2_vlan_action_cfg_get(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN,vlan_fib_map.fib_id,&vlan_fib_cfg);
        if (ret != CA_E_OK) {
            goto end;
        }

        p_vlan_action->new_outer_vlan_src = vlan_fib_cfg.top_vlan_id_sel;
    }


    if(CA_L2_VLAN_ACTION_DIR_LAN_TO_WAN == direction){

        ret = aal_l2_vlan_fib_map_get(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, ingress_vid, &vlan_fib_map);
        if (ret != CA_E_OK) {
            goto end;
        }

        if (vlan_fib_map.valid == 0) {
            ret = CA_E_EXISTS;
            goto end;
        }

        ret = aal_l2_vlan_action_cfg_get(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN,vlan_fib_map.fib_id,&vlan_fib_cfg);
        if (ret != CA_E_OK) {
            goto end;
        }

    }
    p_vlan_action->new_inner_vlan = vlan_fib_cfg.vlan_id;
    p_vlan_action->new_outer_vlan = vlan_fib_cfg.vlan_id;

    p_vlan_action->new_inner_vlan_src = 0;
    p_vlan_action->new_inner_pri_src = 0;
    p_vlan_action->new_inner_pri = 0;
    p_vlan_action->new_inner_tpid_index = 0;
    p_vlan_action->new_inner_tpid_src = 0;

    p_vlan_action->inner_vlan_cmd= vlan_fib_cfg.inner_vlan_cmd;
    p_vlan_action->outer_vlan_cmd= vlan_fib_cfg.top_vlan_cmd;
    if (vlan_fib_cfg.dot1p_field_vld == 1){
        p_vlan_action->new_outer_pri = vlan_fib_cfg.dot1p;
    }
    else{
        p_vlan_action->new_outer_pri = CA_UINT32_INVALID;
    }

    p_vlan_action->new_outer_pri_src = 0 ;
    p_vlan_action->new_outer_tpid_index =  0 ;
    p_vlan_action->new_outer_tpid_src = 0;
    if (vlan_fib_cfg.flow_field_vld == 1){
        p_vlan_action->flow_id = vlan_fib_cfg.flow_id;
    }
    else{
        p_vlan_action->flow_id = CA_FLOW_ID_INVALID;
    }

    return CA_E_OK;

 end:
    return ret;
}

void __ca_l2_vlan_action_to_ve_fib(ca_vlan_action_t	*p_vlan_action,
    aal_l2_vlan_fib_t *p_vlan_fib_cfg) {

    if (p_vlan_action->new_outer_vlan != CA_UINT32_INVALID) {
        p_vlan_fib_cfg->vlan_id = p_vlan_action->new_outer_vlan;
        p_vlan_fib_cfg->inner_vlan_cmd = p_vlan_action->inner_vlan_cmd;
        p_vlan_fib_cfg->inner_vlan_cmd_field_vld = 1;
        p_vlan_fib_cfg->top_vlan_cmd = p_vlan_action->outer_vlan_cmd;
        p_vlan_fib_cfg->top_vlan_cmd_field_vld = 1;
    } else {
        p_vlan_fib_cfg->top_vlan_cmd_field_vld = 0;
        p_vlan_fib_cfg->inner_vlan_cmd_field_vld = 0;
    }
    if (CA_UINT32_INVALID != p_vlan_action->new_outer_pri) {
        p_vlan_fib_cfg->dot1p = p_vlan_action->new_outer_pri ;
        p_vlan_fib_cfg->dot1p_field_vld = 1;
    } else{
        p_vlan_fib_cfg->dot1p_field_vld = 0;
    }
    if (CA_FLOW_ID_INVALID != p_vlan_action->flow_id) {
        p_vlan_fib_cfg->flow_id = p_vlan_action->flow_id;
        p_vlan_fib_cfg->flow_field_vld = 1;
 #if  defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
        if(p_vlan_action->flow_id <= 127){
            p_vlan_fib_cfg->dpid_field_vld  = 1;
            p_vlan_fib_cfg->dpid            = 7;
            p_vlan_fib_cfg->cos_field_vld   = 1;
            p_vlan_fib_cfg->cos             = (p_vlan_action->flow_id&7);
        }
#endif
    } else
        p_vlan_fib_cfg->flow_field_vld = 0;

    p_vlan_fib_cfg->top_vlan_id_sel = (p_vlan_action->new_outer_vlan_src
        == CA_L2_VLAN_NEW_VLAN_SRC_FDB) ;
}

ca_status_t ca_l2_vlan_action_set (
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_vlan_id_t        ingress_vid,
    CA_IN       ca_boolean_t        direction,
    CA_IN       ca_vlan_action_t    *p_vlan_action)
{
    ca_status_t                 ret            = CA_E_OK;
    aal_l2_vlan_fib_map_t       vlan_fib_map;
    aal_l2_vlan_fib_t           vlan_fib_cfg;
    aal_l2_vlan_fib_mask_t      vlan_fib_mask;


    CA_ASSERT_RET((CA_L2_VLAN_ACTION_DIR_LAN_TO_WAN == direction) ||(CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN == direction), CA_E_PARAM);
    CA_ASSERT_RET(p_vlan_action != NULL, CA_E_PARAM);
    CA_ASSERT_RET(ingress_vid <= __MAX_VLAN_ID,CA_E_PARAM);

    memset(&vlan_fib_map, 0, sizeof(aal_l2_vlan_fib_map_t));
    memset(&vlan_fib_cfg, 0, sizeof(aal_l2_vlan_fib_t));
    memset(&vlan_fib_mask,0, sizeof(aal_l2_vlan_fib_mask_t));


    if(CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN == direction){

        ret = aal_l2_vlan_fib_map_get(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN, ingress_vid, &vlan_fib_map);
        if (ret != CA_E_OK) {
            L2_DEBUG("aal_l2_vlan_fib_map_get() failed\n");
            goto end;
        }

        if(FALSE == vlan_fib_map.valid){
            ret = CA_E_NOT_FOUND;
            L2_DEBUG("vlan_fib_map is invalid\n");
            goto end;
        }

        vlan_fib_mask.u32 = 0;
        vlan_fib_mask.s.vlan_id = 1;
        vlan_fib_mask.s.top_vlan_cmd_field_vld = 1;
        vlan_fib_mask.s.top_vlan_cmd = 1;
        vlan_fib_mask.s.inner_vlan_cmd_field_vld = 1;
        vlan_fib_mask.s.inner_vlan_cmd = 1;
        vlan_fib_mask.s.dot1p_field_vld = 1;
        vlan_fib_mask.s.dot1p = 1;
        vlan_fib_mask.s.flow_field_vld = 1;
        vlan_fib_mask.s.flow_id = 1;
        vlan_fib_mask.s.top_vlan_id_sel = 1;

        __ca_l2_vlan_action_to_ve_fib(p_vlan_action, &vlan_fib_cfg);

        ret = aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_WAN_SVLAN,vlan_fib_map.fib_id,vlan_fib_mask,&vlan_fib_cfg);
        if (ret != CA_E_OK) {
            goto end;
        }
    }

    if(CA_L2_VLAN_ACTION_DIR_LAN_TO_WAN == direction){

        ret = aal_l2_vlan_fib_map_get(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN, ingress_vid, &vlan_fib_map);
        if (ret != CA_E_OK) {
            goto end;
        }

        if(FALSE == vlan_fib_map.valid){
            ret = CA_E_NOT_FOUND;
            goto end;
        }

        vlan_fib_mask.u32 = 0;
        vlan_fib_mask.s.vlan_id = 1;
        vlan_fib_mask.s.top_vlan_cmd_field_vld = 1;
        vlan_fib_mask.s.top_vlan_cmd = 1;
        vlan_fib_mask.s.inner_vlan_cmd_field_vld = 1;
        vlan_fib_mask.s.inner_vlan_cmd = 1;
        vlan_fib_mask.s.dot1p_field_vld = 1;
        vlan_fib_mask.s.dot1p = 1;
        vlan_fib_mask.s.flow_field_vld = 1;
        vlan_fib_mask.s.flow_id = 1;
        __ca_l2_vlan_action_to_ve_fib(p_vlan_action, &vlan_fib_cfg);

        ret = aal_l2_vlan_action_cfg_set(device_id, AAL_L2_VLAN_TYPE_LAN_VLAN,vlan_fib_map.fib_id,vlan_fib_mask,&vlan_fib_cfg);
        if (ret != CA_E_OK) {
            goto end;
        }

    }

    return CA_E_OK;
end:
    return ret;

}

ca_status_t ca_l2_vlan_init(ca_device_id_t device_id)
{
    ca_status_t ret = CA_E_OK;

    memset(&__vlan_inner_tpid[0], 0 , __VLAN_TPID_NUM * sizeof(__vlan_inner_tpid[0]));
    memset(&__vlan_outer_tpid[0], 0 , __VLAN_TPID_NUM * sizeof(__vlan_outer_tpid[0]));

    /* set 0x8100 as default comparing tpid */
    __vlan_inner_tpid[0] = 0x8100;
    __vlan_outer_tpid[0] = 0x8100;


#if 0//yocto
    /* set l3fe tpid entries */
    aal_l3fe_pp_tpid_set(1, __vlan_outer_tpid, 1, __vlan_inner_tpid);
#else//sd1 uboot for 98f - not to config l3fe yet
#endif//sd1 uboot for 98f - not to config l3fe yet
    aal_l2_vlan_init(device_id);

    return (ca_status_t)__l2_vlan_action_table_init();
}

EXPORT_SYMBOL(ca_l2_vlan_learning_shared_get);
EXPORT_SYMBOL(ca_l2_vlan_egress_action_get);
EXPORT_SYMBOL(ca_l2_vlan_inner_tpid_add);
EXPORT_SYMBOL(ca_l2_vlan_tpid_get);
EXPORT_SYMBOL(ca_l2_vlan_delete);
EXPORT_SYMBOL(ca_l2_vlan_ingress_default_action_set);
EXPORT_SYMBOL(ca_l2_vlan_inner_tpid_delete);
EXPORT_SYMBOL(ca_l2_vlan_ingress_action_add);
EXPORT_SYMBOL(ca_l2_vlan_inner_tpid_list);
EXPORT_SYMBOL(ca_l2_vlan_outer_tpid_delete);
EXPORT_SYMBOL(ca_l2_vlan_egress_default_action_get);
EXPORT_SYMBOL(ca_l2_vlan_ingress_action_delete);
EXPORT_SYMBOL(ca_l2_vlan_ingress_action_delete_all);
EXPORT_SYMBOL(ca_l2_vlan_outer_tpid_list);
EXPORT_SYMBOL(ca_l2_vlan_egress_action_add);
EXPORT_SYMBOL(ca_l2_vlan_egress_action_delete);
EXPORT_SYMBOL(ca_l2_vlan_egress_action_delete_all);
EXPORT_SYMBOL(ca_l2_vlan_delete_all);
EXPORT_SYMBOL(ca_l2_vlan_create);
EXPORT_SYMBOL(ca_l2_vlan_port_control_set);
EXPORT_SYMBOL(ca_l2_vlan_port_set);
EXPORT_SYMBOL(ca_l2_vlan_learning_shared_set);
EXPORT_SYMBOL(ca_l2_vlan_ingress_action_get);
EXPORT_SYMBOL(ca_l2_vlan_outer_tpid_add);
EXPORT_SYMBOL(ca_l2_vlan_tpid_set);
EXPORT_SYMBOL(ca_l2_vlan_port_control_get);
EXPORT_SYMBOL(ca_l2_vlan_egress_default_action_set);
EXPORT_SYMBOL(ca_l2_vlan_ingress_default_action_get);
EXPORT_SYMBOL(ca_l2_vlan_traverse);
EXPORT_SYMBOL(ca_l2_vlan_port_get);
EXPORT_SYMBOL(ca_l2_vlan_egress_action_iterate);
EXPORT_SYMBOL(ca_l2_vlan_ingress_action_iterate);
EXPORT_SYMBOL(ca_l2_vlan_iterate);
EXPORT_SYMBOL(ca_l2_vlan_flooding_domain_members_add);
EXPORT_SYMBOL(ca_l2_vlan_flooding_domain_members_update);
EXPORT_SYMBOL(ca_l2_vlan_flooding_domain_members_delete);
EXPORT_SYMBOL(ca_l2_vlan_flooding_domain_members_get);
EXPORT_SYMBOL(ca_l2_vlan_action_get);
EXPORT_SYMBOL(ca_l2_vlan_action_set);



ca_status_t ca_set_l2fe_igr_cls(
    CA_IN ca_boolean_t enable, 
    CA_IN ca_port_id_t lspid, /* P0~P4 */
    CA_IN ca_port_id_t ldpid /* P0~P4 */
)
{

//printf("\t%s\tlspid: %#x\tldpid: %#x\n", __func__, lspid, ldpid);

    ca_device_id_t device_id = 0;
    aal_l2_cls_l2rl2r_key_t cls_key;
    aal_l2_cls_fib_t cls_fib;
    aal_l2_cls_l2rl2r_key_mask_t cls_mask;
    aal_l2_cls_fib_mask_t fib_mask;
    ca_uint16_t logic_port_id;
    ca_status_t ret = CA_E_OK;
    ca_uint16_t entry_id;
    ca_vlan_port_control_t port_control;

    ret = ca_l2_vlan_port_control_get(device_id, lspid, &port_control);

    logic_port_id = PORT_ID(lspid);

    memset(&cls_key, 0, sizeof(aal_l2_cls_l2rl2r_key_t));
    cls_mask.u64 = 0;

    cls_mask.s.valid = 1;
    cls_key.valid = enable;

#if 0
    cls_mask.s.entry_type = 1;
    cls_key.entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

    cls_mask.s.ethernet_type_0 = 1;
    cls_key.rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;

    cls_mask.s.rule_type_1 = 1;
    cls_key.rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
#endif

    cls_mask.s.vlan_number_vld_0 = 1;
    cls_key.vlan_number_vld_0 = FALSE;
    cls_mask.s.vlan_number_vld_1 = 1;
    cls_key.vlan_number_vld_1= FALSE;

    cls_mask.s.ethernet_type_vld_0 = 1;
    cls_key.ethernet_type_vld_0 = FALSE;
    cls_mask.s.ethernet_type_vld_1 = 1;
    cls_key.ethernet_type_vld_1 = FALSE;

    cls_mask.s.l4_port_field_ctrl_0 = 1;
    cls_key.l4_port_field_ctrl_0 = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    cls_mask.s.l4_port_field_ctrl_1 = 1;
    cls_key.l4_port_field_ctrl_1 = AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;

    cls_mask.s.mac_field_ctrl_0 = 1;
    cls_key.mac_field_ctrl_0 = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    cls_mask.s.mac_field_ctrl_1 = 1;
    cls_key.mac_field_ctrl_1 = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;

    cls_mask.s.vlan_field_ctrl_0 = 1;
    cls_key.vlan_field_ctrl_0 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    cls_mask.s.vlan_field_ctrl_1 = 1;
    cls_key.vlan_field_ctrl_1 = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;

    cls_mask.s.dot1p_ctrl_0 = 1;
    cls_key.dot1p_ctrl_0 = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    cls_mask.s.dot1p_ctrl_1 = 1;
    cls_key.dot1p_ctrl_1 = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;

    cls_mask.s.field_is_or_0 = 1;
    cls_key.field_is_or_0 = TRUE;
    cls_mask.s.field_is_or_1 = 1;
    cls_key.field_is_or_1 = FALSE;

    cls_mask.s.precedence_0 = 1;
    cls_key.precedence_0 = 7;
    cls_mask.s.precedence_1 = 1;
    cls_key.precedence_1 = 6;

//    entry_id = logic_port_id * __INGRESS_CLS_ENTRY_NUM_PER_PORT + __VLAN_UTAG_STAG_ENTRY_ID;
    entry_id = logic_port_id * __INGRESS_CLS_ENTRY_NUM_PER_PORT;
//printf("\t %s(%d)\tentry_id: %#x\n", __func__, __LINE__, entry_id);
//printf("\t %s(%d)\tlspid: %#x\n", __func__, __LINE__, lspid);
//printf("\t %s(%d)\tlogic_port_id: %#x\n", __func__, __LINE__, logic_port_id);
//printf("\t %s(%d)\tldpid: %#x\n", __func__, __LINE__, ldpid);

    ret = aal_l2_cls_l2rl2r_key_set(device_id, entry_id, cls_mask, &cls_key);
    if (ret != CA_E_OK) {
        printf("\t %s(%d)\taal_l2_cls_l2rl2r_key_set: ret = %#x\n", __func__, __LINE__, ret);
        goto end;
    }

    memset(&cls_fib, 0, sizeof(aal_l2_cls_fib_t));
    fib_mask.u64 = 0;

    fib_mask.s.permit_0 = 1;
    cls_fib.permit_0 = TRUE;
    fib_mask.s.permit_1 = 1;
    cls_fib.permit_1 = TRUE;

    fib_mask.s.permit_valid_0 = 1;
    cls_fib.permit_valid_0 = TRUE;
    fib_mask.s.permit_valid_1 = 1;
    cls_fib.permit_valid_1 = TRUE;

    fib_mask.s.premarked_0 = 1;
    cls_fib.premarked_0 = FALSE;
    fib_mask.s.premarked_1 = 1;
    cls_fib.premarked_1 = FALSE;

    fib_mask.s.top_cmd_valid_0 = 1;
    cls_fib.top_cmd_valid_0 = FALSE;
    fib_mask.s.top_cmd_valid_1 = 1;
    cls_fib.top_cmd_valid_1 = TRUE;

    fib_mask.s.inner_cmd_valid_0 = 1;
    cls_fib.inner_cmd_valid_0 = FALSE;
    fib_mask.s.inner_cmd_valid_1 = 1;
    cls_fib.inner_cmd_valid_1 = FALSE;

    fib_mask.s.dot1p_valid_0 = 1;
    cls_fib.dot1p_valid_0 = FALSE;
    fib_mask.s.dot1p_valid_1 = 1;
    cls_fib.dot1p_valid_1 = FALSE;

    fib_mask.s.dscp_valid_0 = 1;
    cls_fib.dscp_valid_0 = FALSE;
    fib_mask.s.dscp_valid_1 = 1;
    cls_fib.dscp_valid_1 = FALSE;

    fib_mask.s.cos_valid_0 = 1;
    cls_fib.cos_valid_0 = FALSE;
    fib_mask.s.cos_valid_1 = 1;
    cls_fib.cos_valid_1 = FALSE;

    fib_mask.s.dp_valid_0 = 1;
    cls_fib.dp_valid_0 = TRUE;
    fib_mask.s.dp_value_0 = 1;
    cls_fib.dp_value_0 = ldpid;

    fib_mask.s.dp_valid_1 = 1;
    cls_fib.dp_valid_1 = TRUE;
    fib_mask.s.dp_value_1 = 1;
    cls_fib.dp_value_1 = ldpid;

    fib_mask.s.flowid_0 = 1;
    cls_fib.flowid_0 = 0;
    fib_mask.s.flowid_1 = 1;
    cls_fib.flowid_1 = 0;

    fib_mask.s.mark_ena_0 = 1;
    cls_fib.mark_ena_0 = FALSE;
    fib_mask.s.mark_ena_1 = 1;
    cls_fib.mark_ena_1 = FALSE;

    fib_mask.s.dscp_mark_down_0 = 1;
    cls_fib.dscp_mark_down_0 = FALSE;
    fib_mask.s.dscp_mark_down_1 = 1;
    cls_fib.dscp_mark_down_1 = FALSE;

    fib_mask.s.spt_mode_0 =1 ;
    cls_fib.spt_mode_0 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;
    fib_mask.s.spt_mode_1 =1 ;
    cls_fib.spt_mode_1 = AAL_L2_CLS_SPT_MODE_FWD_LEARN ;

    fib_mask.s.no_drop_0 = 1;
    cls_fib.no_drop_0 = FALSE;
    fib_mask.s.no_drop_1 = 1;
    cls_fib.no_drop_1 = FALSE;

    ret = aal_l2_cls_fib_set(device_id, entry_id, fib_mask, &cls_fib);
    if (ret != CA_E_OK) {
        printf("\t %s(%d)\taal_l2_cls_fib_set: ret = %#x\n", __func__, __LINE__, ret);
        goto end;
    }

end:
    return ret;
}

void ca_get_l2fe_igr_cls_key(
    CA_IN ca_uint32_t _entry_id /* P0~P4 */
)
{
    ca_device_id_t  device_id = 0; 
    ca_status_t ret = CA_E_OK;
    aal_l2_cls_l2rl2r_key_t  cfg ; 

    memset(&cfg, 0x00, sizeof(cfg));

    ret = aal_l2_cls_l2rl2r_key_get(device_id,_entry_id,&cfg);
    if(ret != CA_E_OK){ 
        printf("\taal_l2_cls_l2rl2r_key_get failed, return = %d\n", ret ); 
        return CA_E_PARAM; 
    }

    printf("\t%-30s:  %08x \r\n","valid",cfg.valid);
    printf("\t%-30s:  %02x:%02x:%02x:%02x:%02x:%02x\r\n","mac_da_0",cfg.mac_da_0[0],cfg.mac_da_0[1],cfg.mac_da_0[2],cfg.mac_da_0[3],cfg.mac_da_0[4],cfg.mac_da_0[5]);
    printf("\t%-30s:  %02x:%02x:%02x:%02x:%02x:%02x\r\n","mac_sa_0",cfg.mac_sa_0[0],cfg.mac_sa_0[1],cfg.mac_sa_0[2],cfg.mac_sa_0[3],cfg.mac_sa_0[4],cfg.mac_sa_0[5]);
    printf("\t%-30s:  %08x \r\n","mac_field_ctrl_0",cfg.mac_field_ctrl_0);
    printf("\t%-30s:  %08x \r\n","top_vlan_id_0",cfg.top_vlan_id_0);
    printf("\t%-30s:  %08x \r\n","top_is_svlan_0",cfg.top_is_svlan_0);
    printf("\t%-30s:  %08x \r\n","inner_vlan_id_0",cfg.inner_vlan_id_0);
    printf("\t%-30s:  %08x \r\n","inner_is_svlan_0",cfg.inner_is_svlan_0);
    printf("\t%-30s:  %08x \r\n","vlan_field_ctrl_0",cfg.vlan_field_ctrl_0);
    printf("\t%-30s:  %08x \r\n","top_dot1p_0",cfg.top_dot1p_0);
    printf("\t%-30s:  %08x \r\n","inner_dot1p_0",cfg.inner_dot1p_0);
    printf("\t%-30s:  %08x \r\n","dot1p_ctrl_0",cfg.dot1p_ctrl_0);
    printf("\t%-30s:  %08x \r\n","vlan_number_0",cfg.vlan_number_0);
    printf("\t%-30s:  %08x \r\n","vlan_number_vld_0",cfg.vlan_number_vld_0);
    printf("\t%-30s:  %08x \r\n","ethernet_type_0",cfg.ethernet_type_0);
    printf("\t%-30s:  %08x \r\n","ethernet_type_vld_0",cfg.ethernet_type_vld_0);
    printf("\t%-30s:  %08x \r\n","l4_src_port_0",cfg.l4_src_port_0);
    printf("\t%-30s:  %08x \r\n","l4_dst_port_0",cfg.l4_dst_port_0);
    printf("\t%-30s:  %08x \r\n","l4_port_field_ctrl_0",cfg.l4_port_field_ctrl_0);
    printf("\t%-30s:  %08x \r\n","field_is_or_0",cfg.field_is_or_0);
    printf("\t%-30s:  %08x \r\n","precedence_0",cfg.precedence_0);
    printf("\t%-30s:  %02x:%02x:%02x:%02x:%02x:%02x\r\n","mac_da_1",cfg.mac_da_1[0],cfg.mac_da_1[1],cfg.mac_da_1[2],cfg.mac_da_1[3],cfg.mac_da_1[4],cfg.mac_da_1[5]);
    printf("\t%-30s:  %02x:%02x:%02x:%02x:%02x:%02x\r\n","mac_sa_1",cfg.mac_sa_1[0],cfg.mac_sa_1[1],cfg.mac_sa_1[2],cfg.mac_sa_1[3],cfg.mac_sa_1[4],cfg.mac_sa_1[5]);
    printf("\t%-30s:  %08x \r\n","mac_field_ctrl_1",cfg.mac_field_ctrl_1);
    printf("\t%-30s:  %08x \r\n","top_vlan_id_1",cfg.top_vlan_id_1);
    printf("\t%-30s:  %08x \r\n","top_is_svlan_1",cfg.top_is_svlan_1);
    printf("\t%-30s:  %08x \r\n","inner_vlan_id_1",cfg.inner_vlan_id_1);
    printf("\t%-30s:  %08x \r\n","inner_is_svlan_1",cfg.inner_is_svlan_1);
    printf("\t%-30s:  %08x \r\n","vlan_field_ctrl_1",cfg.vlan_field_ctrl_1);
    printf("\t%-30s:  %08x \r\n","top_dot1p_1",cfg.top_dot1p_1);
    printf("\t%-30s:  %08x \r\n","inner_dot1p_1",cfg.inner_dot1p_1);
    printf("\t%-30s:  %08x \r\n","dot1p_ctrl_1",cfg.dot1p_ctrl_1);
    printf("\t%-30s:  %08x \r\n","vlan_number_1",cfg.vlan_number_1);
    printf("\t%-30s:  %08x \r\n","vlan_number_vld_1",cfg.vlan_number_vld_1);
    printf("\t%-30s:  %08x \r\n","ethernet_type_1",cfg.ethernet_type_1);
    printf("\t%-30s:  %08x \r\n","ethernet_type_vld_1",cfg.ethernet_type_vld_1);
    printf("\t%-30s:  %08x \r\n","l4_src_port_1",cfg.l4_src_port_1);
    printf("\t%-30s:  %08x \r\n","l4_dst_port_1",cfg.l4_dst_port_1);
    printf("\t%-30s:  %08x \r\n","l4_port_field_ctrl_1",cfg.l4_port_field_ctrl_1);
    printf("\t%-30s:  %08x \r\n","field_is_or_1",cfg.field_is_or_1);
    printf("\t%-30s:  %08x \r\n","precedence_1",cfg.precedence_1);

    return; 
}

void ca_get_l2fe_igr_cls_fib(
    CA_IN ca_uint32_t _fib_id /* P0~P4 */
)
{
    ca_device_id_t device_id = 0; 
    ca_status_t ret = CA_E_OK;
    aal_l2_cls_fib_t cfg;

    memset(&cfg,0x00,sizeof(cfg));

    ret = aal_l2_cls_fib_get(device_id,_fib_id,&cfg);
    if(ret != CA_E_OK){ 
        printf("\taal_l2_cls_fib_get failed, return = %d\n", ret ); 
        return CA_E_PARAM; 
    }

    printf("\t%-30s:  %08x \r\n","permit_0",cfg.permit_0);
    printf("\t%-30s:  %08x \r\n","permit_valid_0",cfg.permit_valid_0);
    printf("\t%-30s:  %08x \r\n","premarked_0",cfg.premarked_0);
    printf("\t%-30s:  %08x \r\n","top_sc_ind_0",cfg.top_sc_ind_0);
    printf("\t%-30s:  %08x \r\n","top_vlan_cmd_0",cfg.top_vlan_cmd_0);
    printf("\t%-30s:  %08x \r\n","top_cmd_valid_0",cfg.top_cmd_valid_0);
    printf("\t%-30s:  %08x \r\n","top_vlanid_0",cfg.top_vlanid_0);
    printf("\t%-30s:  %08x \r\n","inner_sc_ind_0",cfg.inner_sc_ind_0);
    printf("\t%-30s:  %08x \r\n","inner_vlan_cmd_0",cfg.inner_vlan_cmd_0);
    printf("\t%-30s:  %08x \r\n","inner_cmd_valid_0",cfg.inner_cmd_valid_0);
    printf("\t%-30s:  %08x \r\n","inner_vlanid_0",cfg.inner_vlanid_0);
    printf("\t%-30s:  %08x \r\n","dot1p_0",cfg.dot1p_0);
    printf("\t%-30s:  %08x \r\n","dot1p_valid_0",cfg.dot1p_valid_0);
    printf("\t%-30s:  %08x \r\n","dscp_0",cfg.dscp_0);
    printf("\t%-30s:  %08x \r\n","dscp_valid_0",cfg.dscp_valid_0);
    printf("\t%-30s:  %08x \r\n","cos_0",cfg.cos_0);
    printf("\t%-30s:  %08x \r\n","cos_valid_0",cfg.cos_valid_0);
    printf("\t%-30s:  %08x \r\n","dp_value_0",cfg.dp_value_0);
    printf("\t%-30s:  %08x \r\n","dp_valid_0",cfg.dp_valid_0);
    printf("\t%-30s:  %08x \r\n","flowid_0",cfg.flowid_0);
    printf("\t%-30s:  %08x \r\n","mark_ena_0",cfg.mark_ena_0);
    printf("\t%-30s:  %08x \r\n","dscp_mark_down_0",cfg.dscp_mark_down_0);
    printf("\t%-30s:  %08x \r\n","spt_mode_0",cfg.spt_mode_0);
    printf("\t%-30s:  %08x \r\n","no_drop_0",cfg.no_drop_0);
    printf("\t%-30s:  %08x \r\n","permit_1",cfg.permit_1);
    printf("\t%-30s:  %08x \r\n","permit_valid_1",cfg.permit_valid_1);
    printf("\t%-30s:  %08x \r\n","premarked_1",cfg.premarked_1);
    printf("\t%-30s:  %08x \r\n","top_sc_ind_1",cfg.top_sc_ind_1);
    printf("\t%-30s:  %08x \r\n","top_vlan_cmd_1",cfg.top_vlan_cmd_1);
    printf("\t%-30s:  %08x \r\n","top_cmd_valid_1",cfg.top_cmd_valid_1);
    printf("\t%-30s:  %08x \r\n","top_vlanid_1",cfg.top_vlanid_1);
    printf("\t%-30s:  %08x \r\n","inner_sc_ind_1",cfg.inner_sc_ind_1);
    printf("\t%-30s:  %08x \r\n","inner_vlan_cmd_1",cfg.inner_vlan_cmd_1);
    printf("\t%-30s:  %08x \r\n","inner_cmd_valid_1",cfg.inner_cmd_valid_1);
    printf("\t%-30s:  %08x \r\n","inner_vlanid_1",cfg.inner_vlanid_1);
    printf("\t%-30s:  %08x \r\n","dot1p_1",cfg.dot1p_1);
    printf("\t%-30s:  %08x \r\n","dot1p_valid_1",cfg.dot1p_valid_1);
    printf("\t%-30s:  %08x \r\n","dscp_1",cfg.dscp_1);
    printf("\t%-30s:  %08x \r\n","dscp_valid_1",cfg.dscp_valid_1);
    printf("\t%-30s:  %08x \r\n","cos_1",cfg.cos_1);
    printf("\t%-30s:  %08x \r\n","cos_valid_1",cfg.cos_valid_1);
    printf("\t%-30s:  %08x \r\n","dp_value_1",cfg.dp_value_1);
    printf("\t%-30s:  %08x \r\n","dp_valid_1",cfg.dp_valid_1);
    printf("\t%-30s:  %08x \r\n","flowid_1",cfg.flowid_1);
    printf("\t%-30s:  %08x \r\n","mark_ena_1",cfg.mark_ena_1);
    printf("\t%-30s:  %08x \r\n","dscp_mark_down_1",cfg.dscp_mark_down_1);
    printf("\t%-30s:  %08x \r\n","spt_mode_1",cfg.spt_mode_1);
    printf("\t%-30s:  %08x \r\n","no_drop_1",cfg.no_drop_1);

    return; 
}

