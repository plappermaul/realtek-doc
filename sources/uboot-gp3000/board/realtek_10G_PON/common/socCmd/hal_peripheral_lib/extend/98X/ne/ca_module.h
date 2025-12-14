#ifndef _CA_MODULE_H_
#define _CA_MODULE_H_

#define   CA_MODULES_DEFINES \
   xx(CA_MOD_DRVR_GEN,         "gen"    )       \
   xx(CA_MOD_DRVR_DEV,         "dev"    )       \
   xx(CA_MOD_PLAT_PERI,        "peri"  )        \
   xx(CA_MOD_LOG_KERN,         "klog" )         \
   xx(CA_MOD_OAM_APP,          "oam" )          \
   xx(CA_MOD_ORG_OAM_APP,      "oam-org" )      \
   xx(CA_MOD_ALARM_APP,        "alarm" )        \
   xx(CA_MOD_OMCI,             "omci")          \
   xx(CA_MOD_GPON_DRV,         "gpon")          \
   xx(CA_MOD_EPON_DRV,         "epon")          \
   xx(CA_MOD_CLI,              "cli")           \
   xx(CA_MOD_PLAT_SCFG,        "scfg")          \
   xx(CA_MOD_NE_DRV,           "ne")            \


typedef enum {
#undef xx
#define xx(mod_id,mod_desc)  mod_id,
    CA_MODULES_DEFINES
    CA_MOD_ID_MAX
} ca_mod_id_e;

#endif

