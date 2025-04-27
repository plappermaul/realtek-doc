#ifndef __RTK_MAPLE_TABLE_STRUCT_H__
#define __RTK_MAPLE_TABLE_STRUCT_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#ifdef __BOOTLOADER__
#include <hal/chipdef/maple/rtk_maple_uboot_feature_def.h>
#else   /* __BOOTLOADER__ */
#include <hal/chipdef/maple/rtk_maple_feature_def.h>
#endif  /* __BOOTLOADER__ */

/* Superset Table Enum */
typedef enum rtk_maple_table_list_e
{
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACLt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MCt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIPt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MCt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UCt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MCt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIPt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MCt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOPt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACYt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UCt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_LOGt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MC_PMSKt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTIt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_ROUTINGt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_UNTAGt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLANt,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVTt,
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */

    MAPLE_TABLE_LIST_END,
} rtk_maple_table_list_t;


/* Internal Table Enum */
typedef enum rtk_int_maple_table_list_e
{
#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_IACL_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_CAM_IP_MC_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_CAM_IP_MC_SIP_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_CAM_MC_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_CAM_UC_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_IP_MC_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_IP_MC_SIP_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_MC_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_NEXT_HOP_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_NEXT_HOP_LEGACY_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_L2_UC_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_LOG_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_MC_PMSK_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_MSTI_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_ROUTING_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_UNTAG_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_VLAN_RTL8380,
#endif
#if defined(CONFIG_SDK_RTL8380)
    INT_MAPLE_VLAN_EGR_CNVT_RTL8380,
#endif
#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */

    INT_MAPLE_TABLE_LIST_END,
} rtk_int_maple_table_list_t;

#if defined(CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS)

typedef enum rtk_maple_iaclField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_11tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_10tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_9tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_8tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_7tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_6tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_5tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_4tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_3tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FIELD_0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_SPMMASK_FIXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_SPNtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_MGNT_VLANtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_DMAC_HIT_SWtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_NOT_FIRST_FRAGtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FRAME_TYPE_L4tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FRAME_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_OTAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_ITAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_OTAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_ITAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FRAME_TYPE_L2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_TIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_11tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_10tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_9tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_8tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_7tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_6tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_5tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_4tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_3tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FIELD_0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_SPMMASK_FIXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_SPNtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_MGNT_VLANtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_DMAC_HIT_SWtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_NOT_FIRST_FRAGtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FRAME_TYPE_L4tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FRAME_TYPEtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_OTAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_ITAG_FMTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_OTAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_ITAG_EXISTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_FRAME_TYPE_L2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_BMSK_TIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_VALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_NOTtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AND1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AND2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_IVALIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AIF4tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AIF3tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AIF2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AIF1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_AIF0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_DROPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FWD_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_OVID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_IVID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_FLT_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_LOG_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_RMK_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_METER_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_TAGST_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_MIR_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_NORPRI_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_CPUPRI_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_OTPID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_ITPID_SELtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_IACL_SHAPER_SELtf,
#endif

    MAPLE_IACLFIELD_LIST_END,
} rtk_maple_iaclField_list_t;


typedef enum rtk_maple_l2_cam_ip_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_MC_PMSK_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_FID_RVIDtf,
#endif

    MAPLE_L2_CAM_IP_MCFIELD_LIST_END,
} rtk_maple_l2_cam_ip_mcField_list_t;


typedef enum rtk_maple_l2_cam_ip_mc_sipField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIP_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIP_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIP_MC_PMSK_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIP_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIP_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_IP_MC_SIP_SIPtf,
#endif

    MAPLE_L2_CAM_IP_MC_SIPFIELD_LIST_END,
} rtk_maple_l2_cam_ip_mc_sipField_list_t;


typedef enum rtk_maple_l2_cam_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MC_MC_PMSK_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MC_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_MC_FID_RVIDtf,
#endif

    MAPLE_L2_CAM_MCFIELD_LIST_END,
} rtk_maple_l2_cam_mcField_list_t;


typedef enum rtk_maple_l2_cam_ucField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_STATICtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_AGEtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_SLPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_SA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_DA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_SUSPENDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_CAM_UC_FID_RVIDtf,
#endif

    MAPLE_L2_CAM_UCFIELD_LIST_END,
} rtk_maple_l2_cam_ucField_list_t;


typedef enum rtk_maple_l2_ip_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_MC_PMSK_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_FID_RVIDtf,
#endif

    MAPLE_L2_IP_MCFIELD_LIST_END,
} rtk_maple_l2_ip_mcField_list_t;


typedef enum rtk_maple_l2_ip_mc_sipField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIP_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIP_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIP_MC_PMSK_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIP_FID_RVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIP_GIPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_IP_MC_SIP_SIPtf,
#endif

    MAPLE_L2_IP_MC_SIPFIELD_LIST_END,
} rtk_maple_l2_ip_mc_sipField_list_t;


typedef enum rtk_maple_l2_mcField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MC_MC_PMSK_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MC_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_MC_FID_RVIDtf,
#endif

    MAPLE_L2_MCFIELD_LIST_END,
} rtk_maple_l2_mcField_list_t;


typedef enum rtk_maple_l2_next_hopField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_STATICtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_AGEtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_SLPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_VLAN_TARGETtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_ROUTE_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_SA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_DA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_SUSPENDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_FID_RVIDtf,
#endif

    MAPLE_L2_NEXT_HOPFIELD_LIST_END,
} rtk_maple_l2_next_hopField_list_t;


typedef enum rtk_maple_l2_next_hop_legacyField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_DPAtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_GWMAC_IDXtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_DVIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_NEXT_HOP_LEGACY_VID_SELtf,
#endif

    MAPLE_L2_NEXT_HOP_LEGACYFIELD_LIST_END,
} rtk_maple_l2_next_hop_legacyField_list_t;


typedef enum rtk_maple_l2_ucField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_IP_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_IP6_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_STATICtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_AGEtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_SLPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_VIDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_SA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_DA_BLKtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_SUSPENDtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_NEXT_HOPtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_MACtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_L2_UC_FID_RVIDtf,
#endif

    MAPLE_L2_UCFIELD_LIST_END,
} rtk_maple_l2_ucField_list_t;


typedef enum rtk_maple_logField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_LOG_CNTRtf,
#endif

    MAPLE_LOGFIELD_LIST_END,
} rtk_maple_logField_list_t;


typedef enum rtk_maple_mc_pmskField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MC_PMSK_PMSKtf,
#endif

    MAPLE_MC_PMSKFIELD_LIST_END,
} rtk_maple_mc_pmskField_list_t;


typedef enum rtk_maple_mstiField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT28tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT27tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT26tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT25tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT24tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT23tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT22tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT21tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT20tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT19tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT18tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT17tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT16tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT15tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT14tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT13tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT12tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT11tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT10tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT9tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT8tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT7tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT6tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT5tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT4tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT3tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT2tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_MSTI_STATE_PORT0tf,
#endif

    MAPLE_MSTIFIELD_LIST_END,
} rtk_maple_mstiField_list_t;


typedef enum rtk_maple_routingField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_ROUTING_GATEWAY_MACtf,
#endif

    MAPLE_ROUTINGFIELD_LIST_END,
} rtk_maple_routingField_list_t;


typedef enum rtk_maple_untagField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_UNTAG_PMSKtf,
#endif

    MAPLE_UNTAGFIELD_LIST_END,
} rtk_maple_untagField_list_t;


typedef enum rtk_maple_vlanField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_MBRtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_FID_MSTItf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_L2_HASH_KEY_UCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_L2_HASH_KEY_MCtf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_VLAN_PROFILEtf,
#endif

    MAPLE_VLANFIELD_LIST_END,
} rtk_maple_vlanField_list_t;


typedef enum rtk_maple_vlan_egr_cnvtField_list_e
{
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_VALID1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_ORGPRI1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_ORG_VID1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_ORG_VID_RNG_CHK1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_ORGPRI1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHK1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_PORT_ID1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_PORT_ID1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_VID_SHIFT1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_NEW_VID1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_PRI_ASSIGN1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_NEW_PRI1tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_VALID0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_ORGPRI0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_ORG_VID0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_ORG_VID_RNG_CHK0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_ORGPRI0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHK0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_PORT_ID0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_BMSK_PORT_ID0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_VID_SHIFT0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_NEW_VID0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_PRI_ASSIGN0tf,
#endif
#if defined(CONFIG_SDK_RTL8380)
    MAPLE_VLAN_EGR_CNVT_NEW_PRI0tf,
#endif

    MAPLE_VLAN_EGR_CNVTFIELD_LIST_END,
} rtk_maple_vlan_egr_cnvtField_list_t;

#endif   /* CONFIG_SDK_CHIP_FEATURE_TABLE_ACCESS */
extern rtk_table_t rtk_maple_table_list[];


#endif    /* __RTK_MAPLE_TABLE_STRUCT_H__ */
