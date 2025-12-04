#ifndef __AAL_MIRE_H__
#define __AAL_MIRE_H__

#include "ca_types.h"


#define NUMOF_MIRE_SESSION  4

typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1 : 32 ;
#else 
    ca_uint32_t pkt_drop_src_mir_en  :  1 ; /* bits 0:0 */
    ca_uint32_t pkt_keep_old         :  1 ; /* bits 1:1 */
    ca_uint32_t lspid_remap          :  1 ; /* bits 2:2 */
    ca_uint32_t rsrvd1               : 29 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_mire_mir_ctrl_mask_t;

typedef struct {
    ca_boolean_t pkt_drop_src_mir_en;
    ca_boolean_t pkt_keep_old;
    ca_boolean_t lspid_remap;
} aal_mire_mir_ctrl_t;


typedef enum {
    MIR_DISABLE,
    MIR_INGRESS_PORT,
    MIR_EGRESS_PORT,
    MIR_PORT_CONN,
    MIR_FLOW_BASE,
    MIR_MODE_MAX
}aal_mir_mode_e;

typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1 : 32 ;
#else 
    ca_uint32_t flowid               :  1 ;
    ca_uint32_t ldpid                :  1 ;
    ca_uint32_t lspid                :  1 ;
    ca_uint32_t mode                 :  1 ;
    ca_uint32_t rsrvd2               :  28 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_mire_mir_rule_mask_t;


typedef struct {
    ca_uint8_t flowid;
    ca_uint8_t ldpid;
    ca_uint8_t lspid;
    aal_mir_mode_e mode;
} aal_mire_mir_rule_t;


typedef union {
  struct {
#ifdef CS_BIG_ENDIAN
    ca_uint32_t rsrvd1 : 32 ;
#else 
    ca_uint32_t lspid                :  1 ;
    ca_uint32_t pspid                :  1 ;
    ca_uint32_t ldpid                :  1 ;
    ca_uint32_t dot1p                :  1 ;
    ca_uint32_t sc_ind               :  1 ;
    ca_uint32_t vid                  :  1 ;
    ca_uint32_t vlan_cmd             :  1 ;
    ca_uint32_t cos_vld              :  1 ;
    ca_uint32_t cos                  :  1 ;
    ca_uint32_t sv_tpid_sel          :  1 ;
    ca_uint32_t cv_tpid_sel          :  1 ;
    ca_uint32_t rsrvd1               :  21 ;
#endif
  } bf ;
  ca_uint32_t     wrd ;
} aal_mire_mir_act_mask_t;



typedef struct {
    ca_uint8_t lspid;
    ca_uint8_t pspid;
    ca_uint8_t ldpid;
    ca_uint8_t dot1p;
    ca_uint16_t vid;
    ca_boolean_t vlan_cmd;
    ca_boolean_t sc_ind;
    ca_boolean_t cos_vld;
    ca_uint32_t cos;
    ca_uint32_t sv_tpid_sel;
    ca_uint32_t cv_tpid_sel;
} aal_mire_mir_act_t;


ca_status_t aal_mire_init(
    ca_device_id_t     device_id
);

ca_status_t aal_mire_ctrl_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_mire_mir_ctrl_mask_t  ctrl_mask,
    CA_IN aal_mire_mir_ctrl_t       *ctrl
);

ca_status_t aal_mire_ctrl_get(
    CA_IN ca_device_id_t       device_id,
    CA_OUT aal_mire_mir_ctrl_t  *ctrl
);

ca_status_t aal_mire_rule_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_int32_t                ssid,
    CA_IN aal_mire_mir_rule_mask_t  rule_mask,
    CA_IN aal_mire_mir_rule_t       *rule
);

ca_status_t aal_mire_rule_get(
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_int32_t            ssid,
    CA_OUT aal_mire_mir_rule_t  *rule
);

ca_status_t aal_mire_act_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN ca_int32_t                ssid,
    CA_IN aal_mire_mir_act_mask_t   act_mask,
    CA_IN aal_mire_mir_act_t        *act
);

ca_status_t aal_mire_act_get(
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_int32_t            ssid,
    CA_OUT aal_mire_mir_act_t   *act
);




#endif
