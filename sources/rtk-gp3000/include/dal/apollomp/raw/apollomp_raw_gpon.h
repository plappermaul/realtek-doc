/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision$
 * $Date$
 *
 * Purpose : GPON MAC register access APIs
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */
#ifndef _APOLLOMP_RAW_GPON_H_
#define _APOLLOMP_RAW_GPON_H_

#include <rtk/gpon.h>

typedef enum apollo_raw_gpon_dsGtc_pmMiscType_e{
    GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR = 0,
    GPON_REG_DSGTCPMMISC_BIP_BITS_ERR,
    GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BITS,
    GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BYTES,
    /* Rename from XXX_BLOCKS to XXX_CWS in GPON_MAC_SWIO_r1.1 */
    GPON_REG_DSGTCPMMISC_FEC_CORRETED_CWS,
    GPON_REG_DSGTCPMMISC_FEC_UNCOR_CWS,
    GPON_REG_DSGTCPMMISC_LOM,
    GPON_REG_DSGTCPMMISC_PLOAM_ACCEPT,
    GPON_REG_DSGTCPMMISC_PLOAM_FAIL,
    GPON_REG_DSGTCPMMISC_BWMAP_FAIL,
    GPON_REG_DSGTCPMMISC_BWMAP_INV,
    GPON_REG_DSGTCPMMISC_ACTIVE,
    GPON_REG_DSGTCPMMISC_BWMAP_ACPT,
    GPON_REG_DSGTCPMMISC_GEM_LOS,
    GPON_REG_DSGTCPMMISC_HEC_COR,
    GPON_REG_DSGTCPMMISC_GEM_IDLE,
    GPON_REG_DSGTCPMMISC_GEM_FAIL,
    GPON_REG_DSGTCPMMISC_RX_GEM_NON_IDLE,
    GPON_REG_DSGTCPMMISC_PLEND_CORRECTIONS,
    GPON_REG_DSGTCPMMISC_END,

}apollo_raw_gpon_dsGtc_pmMiscType_t;

typedef enum apollo_raw_gpon_dsGem_pmMiscType_e{
    GPON_REG_DSGEMPMMISC_MULTICAST_RX = 0,
    GPON_REG_DSGEMPMMISC_UNICAST_RX = 1,
    GPON_REG_DSGEMPMMISC_MULTICAST_FWD = 2,
    GPON_REG_DSGEMPMMISC_MULTICAST_LEAK = 3,
    GPON_REG_DSGEMPMMISC_ETH_CRC_ERR = 4,
    GPON_REG_DSGEMPMMISC_OVER_INTERLEAV = 5,
    GPON_REG_DSGEMPMMISC_OMCI_RX = 6,
    GPON_REG_DSGEMPMMISC_END,
}apollo_raw_gpon_dsGem_pmMiscType_t;

typedef enum apollo_raw_gpon_usGtc_pmMiscType_e{
    GPON_REG_USGTCPMMISC_PLOAM_BOH_TX = 0,
    GPON_REG_USGTCPMMISC_GEM_DBRU_TX = 1,
    GPON_REG_USGTCPMMISC_PLOAM_CPU_TX = 2,
    GPON_REG_USGTCPMMISC_PLOAM_AUTO_TX = 3,
    GPON_REG_USGTCPMMISC_GEM_BYTE_TX = 4,
    GPON_REG_USGTCPMMISC_END,
}apollo_raw_gpon_usGtc_pmMiscType_t;

typedef enum apollo_raw_gpon_usAutoPloamType_e{
    GPON_REG_USAUTOPLOAM_NOMSG = 0,
    GPON_REG_USAUTOPLOAM_SN = 1,
    GPON_REG_USAUTOPLOAM_DYINGGASP = 2,
}apollo_raw_gpon_usAutoPloamType_t;



void apollo_raw_gpon_intr_disableAll(void);                         /* Disable all INT */
int32 apollo_raw_gpon_top_gtcDsIntr_check(uint32 status);
int32 apollo_raw_gpon_top_gemDsIntr_check(uint32 status);
int32 apollo_raw_gpon_top_gtcUsIntr_check(uint32 status);
int32 apollo_raw_gpon_top_gemUsIntr_check(uint32 status);
/* page 0x00 */
void apollo_raw_gpon_reset_write(int32 reset);
int32 apollo_raw_gpon_restDone_wait(void);
uint32 apollo_raw_gpon_version_read(void);
uint32 apollo_raw_gpon_test_read(void);
void apollo_raw_gpon_test_write(uint32 value);
int32 apollo_raw_gpon_top_mask_read(uint32 *pValue);
void apollo_raw_gpon_top_mask_write(uint32 value);
int32 apollo_raw_gpon_top_intr_read(uint32 *pValue);

/* page 0x01 */
int32 apollo_raw_gpon_gtcDs_intr_read(uint32 *pValue);
int32 apollo_raw_gpon_gtcDsIntr_snReq_check(uint32 status);
int32 apollo_raw_gpon_gtcDsIntr_rangReq_check(uint32 status);
int32 apollo_raw_gpon_gtcDsIntr_ploam_check(uint32 status);
int32 apollo_raw_gpon_gtcDsIntr_los_check(uint32 status);
int32 apollo_raw_gpon_gtcDsIntr_lof_check(uint32 status);
int32 apollo_raw_gpon_gtcDsIntr_lom_check(uint32 status);
int32 apollo_raw_gpon_gtcDsIntr_fec_check(uint32 status);
int32 apollo_raw_gpon_gtcDs_mask_read(uint32 *pValue);
void apollo_raw_gpon_gtcDs_mask_write(uint32 value);
void apollo_raw_gpon_gtcDsMask_ploam_write(int32 value);      /* PLOAM INT */
void apollo_raw_gpon_gtcDsMask_rangReq_write(int32 value);    /* Ranging Req INT */
void apollo_raw_gpon_gtcDsMask_snReq_write(int32 value);      /* SN Req INT */
void apollo_raw_gpon_gtcDsMask_los_write(int32 value);        /* LOS INT */
void apollo_raw_gpon_gtcDsMask_lof_write(int32 value);        /* LOF INT */
void apollo_raw_gpon_gtcDsMask_lom_write(int32 value);      /* SF-LOS INT */
void apollo_raw_gpon_gtcDsMask_fec_write(int32 value);        /* FEC INT */
int32 apollo_raw_gpon_gtcDs_los_read(uint32 *pValue);
int32 apollo_raw_gpon_gtcDs_lof_read(uint32 *pValue);
int32 apollo_raw_gpon_gtcDs_lom_read(uint32 *pValue);
int32 apollo_raw_gpon_gtcDs_fec_read(uint32 *pValue);
void apollo_raw_gpon_onuId_write(uint8 value);  /* include d/s and u/s */
int apollo_raw_gpon_onuId_read(uint8 *pValue);
void apollo_raw_gpon_onuStatus_write(uint8 value);
int apollo_raw_gpon_onuStatus_read(uint8 *pValue);
void apollo_raw_gpon_dsBwmap_crcCheck_enable(int32 value);
int32 apollo_raw_gpon_dsBwmap_crcCheck_enable_read(int32 *value);

/* Added in GPON_MAC_SWIO_v1.1 */
void apollo_raw_gpon_dsBwmap_filterOnuId_write(int32 value);
int32 apollo_raw_gpon_dsBwmap_filterOnuId_read(int32 *value);

void apollo_raw_gpon_dsBwmap_plend_write(int32 value);
int32 apollo_raw_gpon_dsBwmap_plend_read(int32 *value);

#if 0 /* ploam_crc_check is removed in GPON_MAC_SWIO_v1.1 */
void apollo_raw_gpon_dsPloam_crcCheck_enable(int32 value);
int32 apollo_raw_gpon_dsPloam_crcCheck_enable_read(int32 *value);
#endif
void apollo_raw_gpon_dsScramble_enable(int32 value);
int32 apollo_raw_gpon_dsScramble_enable_read(int32 *value);
void apollo_raw_gpon_dsFec_bypass(int32 value);
int32 apollo_raw_gpon_dsFec_bypass_read(int32 *value);
void apollo_raw_gpon_dsFecThrd_write(uint8 value);
int32 apollo_raw_gpon_dsFecThrd_read(uint8 *value);
void apollo_raw_gpon_extraSN_write(uint8 value);

#if 0 /* gemloop is removed in GPON_MAC_SWIO_v1.1 */
int32 apollo_raw_gpon_gemLoop_read(uint32 *pValue);
void apollo_raw_gpon_gemLoop_write(int32 value);
#endif
void apollo_raw_gpon_dsPloam_nomsg_write(uint8 value);
void apollo_raw_gpon_dsPloam_onuIdFilter_write(int32 value);
int32 apollo_raw_gpon_dsPloam_onuIdFilter_read(int32 *value);
void apollo_raw_gpon_dsPloam_bcAccept_write(int32 value);
int32 apollo_raw_gpon_dsPloam_bcAccept_read(int32 *value);

/* Added in GPON_MAC_SWIO_v1.1 */
void apollo_raw_gpon_dsPloam_dropCrc_write(int32 value);
int32 apollo_raw_gpon_dsPloam_dropCrc_read(int32 *value);

int32 apollo_raw_gpon_cdrLos_read(uint32 *pValue);
int32 apollo_raw_gpon_optLos_read(uint32 *pValue);
void apollo_raw_gpon_losCfg_write(int32 opten, int32 optpolar, int32 cdren, int32 cdrpolar, int32 filter);
int32 apollo_raw_gpon_losCfg_read(int32 *opten, int32 *optpolar, int32 *cdren, int32 *cdrpolar, int32 *filter);
int32 apollo_raw_gpon_superframe_read(uint32 * value);
int32 apollo_raw_gpon_dsPloam_read(uint8 *value);
int32 apollo_raw_gpon_tcont_read(uint32 idx, uint32 *value);
int32 apollo_raw_gpon_tcont_write(uint32 idx, uint32 value);
int32 apollo_raw_gpon_tcont_clean(uint32 idx);
int32 apollo_raw_gpon_dsPort_read(uint32 idx, uint32 *value, uint32 *cfg);
int32 apollo_raw_gpon_dsPort_write(uint32 idx, uint32 value, uint32 cfg);
int32 apollo_raw_gpon_dsPort_clean(uint32 idx);
int32 apollo_raw_gpon_dsPort_pktCnt_read(uint32 idx, uint32 *value);
int32 apollo_raw_gpon_dsPort_byteCnt_read(uint32 idx, uint32 *value);
int32 apollo_raw_gpon_dsGtc_miscCnt_read(apollo_raw_gpon_dsGtc_pmMiscType_t idx, uint32 *pValue);
#if 0 /* TDM PTI is removed in GPON_MAC_SWIO_r1.1 */
void apollo_raw_gpon_dsTdm_pti_write(uint8 mask, uint8 end);
#endif
void apollo_raw_gpon_dsOmci_pti_write(uint8 mask, uint8 end);
int32 apollo_raw_gpon_dsOmci_pti_read(uint8 *mask, uint8 *end);
void apollo_raw_gpon_dsEth_pti_write(uint8 mask, uint8 end);
int32 apollo_raw_gpon_dsEth_pti_read(uint8 *mask, uint8 *end);

/* page 0x03 */
void apollo_raw_gpon_aes_keyswitch_write(uint32 value);
void apollo_raw_gpon_aes_keyswitch_read(uint32 *pValue);
int32 apollo_raw_gpon_aes_keyword_write(uint8 *value);

/* page 0x04 */
int32 apollo_raw_gpon_gemDs_intr_read(uint32 *pValue);
int32 apollo_raw_gpon_gemDsIntr_omci_check(uint32 status);
int32 apollo_raw_gpon_omci_read(uint8 *value);
int32 apollo_raw_gpon_dsPort_ethRxCnt_read(uint32 idx, uint32 *value);
int32 apollo_raw_gpon_dsPort_ethFwdCnt_read(uint32 idx, uint32 *value);
int32 apollo_raw_gpon_dsGem_miscCnt_read(apollo_raw_gpon_dsGem_pmMiscType_t idx, uint32 *pValue);
void apollo_raw_gpon_dsEth_fcsCheck_enable(int32 value);
int32 apollo_raw_gpon_dsEth_fcsCheck_enable_read(uint32 *value);
#if 0 /* remove H16b in GPON_MAC_SWIO_r1.2 */
void apollo_raw_gpon_dsEth_mcH16bChk_write(int32 value);
int32 apollo_raw_gpon_dsEth_mcH16bChk_read(int32 *value);
#endif
#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
void apollo_raw_gpon_dsEth_wellKnownAddrFwd_write(int32 value);
int32 apollo_raw_gpon_dsEth_wellKnownAddrFwd_read(int32 *value);
#endif
void apollo_raw_gpon_dsEth_bcPAss_write(int32 value);
int32 apollo_raw_gpon_dsEth_bcPAss_read(int32 *value);
void apollo_raw_gpon_dsEth_nonMcPass_write(int32 value);
int32 apollo_raw_gpon_dsEth_nonMcPass_read(int32 *value);
void apollo_raw_gpon_dsEth_macFilterMode_write(rtk_gpon_macTable_exclude_mode_t value);
int32 apollo_raw_gpon_dsEth_macFilterMode_read(rtk_gpon_macTable_exclude_mode_t *value);
void apollo_raw_gpon_dsEth_mcForceMode_write(rtk_gpon_mc_force_mode_t ipv4,
                                      rtk_gpon_mc_force_mode_t ipv6);
int32 apollo_raw_gpon_dsEth_mcForceMode_read(rtk_gpon_mc_force_mode_t *ipv4,
                                      rtk_gpon_mc_force_mode_t *ipv6);
int32 apollo_raw_gpon_macEntry_read(uint32 idx, uint32 *value);
int32 apollo_raw_gpon_macEntry_write(uint32 idx, uint32 value);
int32 apollo_raw_gpon_macEntry_clean(uint32 idx);
void apollo_raw_gpon_assembleTimeout_write(uint8 value);
int32 apollo_raw_gpon_assembleTimeout_read(uint8 *value);
void apollo_raw_gpon_ipv4McAddrPtn_write(uint32 value);
int32 apollo_raw_gpon_ipv4McAddrPtn_read(uint32 *value);
/* add IPV6_MC_MAC_PREFIX in GPON_MAC_SWIO_r1.2 */
void apollo_raw_gpon_ipv6McAddrPtn_write(uint32 value);
int32 apollo_raw_gpon_ipv6McAddrPtn_read(uint32 *value);
#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
void apollo_raw_gpon_wellKnownAddr_write(uint32 value);
int32 apollo_raw_gpon_wellKnownAddr_read(uint32 *value);
#endif

/* page 0x05 */
int32 apollo_raw_gpon_gtcUs_intr_read(uint32 *pValue);
int32 apollo_raw_gpon_gtcUsIntr_fec_check(uint32 status);
int32 apollo_raw_gpon_gtcUsIntr_urgPloamEmpty_check(uint32 status);
int32 apollo_raw_gpon_gtcUsIntr_nrmPloamEmpty_check(uint32 status);
int32 apollo_raw_gpon_gtcUs_mask_read(uint32 *pValue);
void apollo_raw_gpon_gtcUs_mask_write(uint32 value);
void apollo_raw_gpon_gtcUsMask_fec_write(int32 value);
void apollo_raw_gpon_gtcUsMask_urgPloamEmpty_write(int32 value);
void apollo_raw_gpon_gtcUsMask_nrmPloamEmpty_write(int32 value);
int32 apollo_raw_gpon_gtcUs_fec_read(uint32 *pValue);
int apollo_raw_gpon_usOnuId_read(uint8 *pValue);
void apollo_raw_gpon_forceLaser_write(rtk_gpon_laser_status_t value);
int32 apollo_raw_gpon_forceLaser_read(rtk_gpon_laser_status_t *pValue);
void apollo_raw_gpon_txPtnMode_write(rtk_gpon_tx_ptn_mode_t fg_mode, rtk_gpon_tx_ptn_mode_t bg_mode);
int32 apollo_raw_gpon_txPtnMode_read(rtk_gpon_tx_ptn_mode_t *pFg_mode, rtk_gpon_tx_ptn_mode_t *pBg_mode);
void apollo_raw_gpon_txPtn_write(uint32 fg_ptn, uint32 bg_ptn);
int32 apollo_raw_gpon_txPtn_read(uint32 *pFg_ptn, uint32 *pBg_ptn);
void apollo_raw_gpon_RogueOnt_write(int32 value);
void apollo_raw_gpon_forceUsPrbs_write(int32 value);
#if 0
void apollo_raw_gpon_forceUsGemIdle_write(int32 value);
int32 apollo_raw_gpon_forceUsGemIdle_read(int32 *pValue);
void apollo_raw_gpon_forceUsPrbs_write(int32 value);
int32 apollo_raw_gpon_forceUsPrbs_read(int32 *pValue);
#endif
void apollo_raw_gpon_ploam_enable(int32 value);
int32 apollo_raw_gpon_ploam_enable_read(int32 *pValue);
void apollo_raw_gpon_ind_nrm_ploam_enable(int32 value);
int32 apollo_raw_gpon_ind_nrm_ploam_enable_read(int32 *pValue);
void apollo_raw_gpon_dbru_enable(int32 value);
int32 apollo_raw_gpon_dbru_enable_read(int32 *pValue);
void apollo_raw_gpon_usScramble_enable(int32 value);
int32 apollo_raw_gpon_usScramble_enable_read(int32 *pValue);
void apollo_raw_gpon_usBurstPolarity_write(int32 value);
int32 apollo_raw_gpon_usBurstPolarity_read(int32 *value);
void apollo_raw_gpon_eqd_write(uint32 value);
int32 apollo_raw_gpon_eqd_read(uint32 *pMultiFrame, uint32 *pIntraFrame);
void apollo_raw_gpon_laserTime_write(uint8 on, uint8 off);
int32 apollo_raw_gpon_laserTime_read(uint8 *on, uint8 *off);
void apollo_raw_gpon_burstOverhead_write(uint8 rep, uint8 len, uint8 size, uint8 *oh);
int32 apollo_raw_gpon_usPloam_write(int32 urgent, uint8 *value);
void apollo_raw_gpon_usAutoPloam_write(apollo_raw_gpon_usAutoPloamType_t type, uint8 *value);
void apollo_raw_gpon_usPloam_crcGen_enable(int32 value);
int32 apollo_raw_gpon_usPloam_crcGen_read(int32 *value);
void apollo_raw_gpon_usPloam_onuIdFilter_write(int32 value);
int32 apollo_raw_gpon_usPloam_onuIdFilter_read(int32 *value);
void apollo_raw_gpon_usPloam_buf_flush_write(int32 value);
int32 apollo_raw_gpon_usPloam_buf_flush_read(int32 *value);
int32 apollo_raw_gpon_usGtc_miscCnt_read(apollo_raw_gpon_usGtc_pmMiscType_t idx, uint32 *pValue);
void apollo_raw_gpon_rdi_write(int32 value);
int32 apollo_raw_gpon_rdi_read(int32 *value);
#if 0 /* DBA TCONT is removed in GPON_MAC_SWIO_r1.1 */
void apollo_raw_gpon_dbaTcont_write(int32 enable, uint32 tcont);
#endif
void apollo_raw_gpon_usSmalSstartProc_enable(int32 value);
int32 apollo_raw_gpon_usSmalSstartProc_enable_read(int32 *value);
void apollo_raw_gpon_usSuppressLaser_enable(int32 value);
int32 apollo_raw_gpon_usSuppressLaser_enable_read(int32 *value);

/* page 0x06 */
/* Function Name:
 *      apollo_raw_gpon_gemUs_intr_read
 * Description:
 *      Read GPON GEM upstream interrupt indicator register
 * Input:
 *      reg_addr 	- void
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_intr_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_sdLong_check
 * Description:
 *      Indicate the Signal Detect valid more than 125us.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_sdLong_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_sdDiff_check
 * Description:
 *      Indicate the Signal Detect and the TX Burst enable has
 *      the huge difference for the Transmit Optical Module.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_sdDiff_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_reqDelay_check
 * Description:
 *      Indicate the GTC_US need request the next 125 us packets but
 *      the SWITCH also is busy on the last 125 us.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_reqDelay_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bcLess6_check
 * Description:
 *      Indicate the byte counter don't take less than 6 and
 *      as Empty TCONT by Switch
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bcLess6_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_errPLI_check
 * Description:
 *      Indicate the PLI mismatching with the input cycle from the Switch.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_errPLI_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_burstLarge_check
 * Description:
 *      Indicate the TM(SWITCH) has the larger burst bytes
 *      than the GTC indication. In this case, GEM module will
 *      insert IDLE until bank (125us) end.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_burstLarge_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankTooMuch_check
 * Description:
 *      Indicate the 1k bytes Bank has too much data at the time
 *      of leave 800 cycles (bytes). It will force the signal buffer
 *      full indication to un-valid in order to let the Switch can
 *      send the next bank data. It means the Switch gives too many
 *      traffic from request and full at the last 800 cycles of band read.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankTooMuch_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankRemain_check
 * Description:
 *      Indicate the Bank has the remained data after the 125us bank read.
 *      It means the Switch gives too many traffic from request.
 *      (Too many response times or too many bytes in one response, etc.)
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankRemain_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankOverflow_check
 * Description:
 *      Indicate Bank overflow. It means the Switch doesn¡¦t have the
 *      true back press machine.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankOverflow_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUsIntr_bankUnderflow_check
 * Description:
 *      Indicate Bank underflow. It is as timeout of request to response
 *      of interface. Insert IDLE in this case.
 * Input:
 *      status 	    - GEM upstream interrupt indicator value
 * Output:
 *      None
 * Return:
 *      1 		    - indicator is happened
 *      0 		    - indicator is not happened
 * Note:
 */
uint32 apollo_raw_gpon_gemUsIntr_bankUnderflow_check(uint32 status);

/* Function Name:
 *      apollo_raw_gpon_gemUs_mask_read
 * Description:
 *      Read the GEM upstream interrupt mask.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_mask_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_mask_write
 * Description:
 *      Write the GEM upstream interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUs_mask_write(uint32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_sdLong_write
 * Description:
 *      Turn on/off the SD long valid interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_sdLong_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_sdDiff_write
 * Description:
 *      Turn on/off the SD huge difference interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_sdDiff_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_reqDelay_write
 * Description:
 *      Turn on/off the request delay interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_reqDelay_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bcLess6_write
 * Description:
 *      Turn on/off the 'byte count less than 6' interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_bcLess6_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_errPLI_write
 * Description:
 *      Turn on/off the error PLT interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_errPLI_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_burstLarge_write
 * Description:
 *      Turn on/off the burst large interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_burstLarge_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankTooMuch_write
 * Description:
 *      Turn on/off the bank too much data interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_bankTooMuch_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankRemain_write
 * Description:
 *      Turn on/off the bank remain data interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_bankRemain_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankOverflow_write
 * Description:
 *      Turn on/off the bank overflow interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_bankOverflow_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUsrMask_bankUnderflow_write
 * Description:
 *      Turn on/off the bank underflow interrupt mask.
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUsrMask_bankUnderflow_write(int32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdLong_read
 * Description:
 *      None
 * Input:
 *      value       - interrupt mask value
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdLong_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdDiff_read
 * Description:
 *      Read the SD huge difference status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdDiff_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdLong_read
 * Description:
 *      Read the request delay status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_reqDelay_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_bcLess6_read
 * Description:
 *      Read the 'byte count less than 6' status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bcLess6_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_errPLI_read
 * Description:
 *      Read the error PLI status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_errPLI_read(uint32 *pValue);


/* Function Name:
 *      apollo_raw_gpon_gemUs_burstLarge_read
 * Description:
 *      Read the burst large status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_burstLarge_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankTooMuch_read
 * Description:
 *      Read the bank too much data status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankTooMuch_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankRemain_read
 * Description:
 *      Read the bank remain data status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankRemain_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankOverflow_read
 * Description:
 *      Read the bank overflow status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankOverflow_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_bankUnderflow_read
 * Description:
 *      Read the bank underflow status.
 * Input:
 *      None
 * Output:
 *      pValue              - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_bankUnderflow_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_forceIdle_write
 * Description:
 *      Turn on/off force to send IDLE GEM only.
 * Input:
 *      value       - on/off value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUs_forceIdle_write(uint32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUs_forceIdle_read
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_forceIdle_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_PTI_vector_write
 * Description:
 *      Set the PTI value vector mapping base on (OMCI,END_FRAG).
 * Input:
 *      pti_v0       - the PTI value base on (OMCI,END_FRAG)=(0,0)
 *      pti_v1       - the PTI value  base on (OMCI,END_FRAG)=(0,1)
 *      pti_v2       - the PTI value  base on (OMCI,END_FRAG)=(1,0)
 *      pti_v3       - the PTI value  base on (OMCI,END_FRAG)=(1,1)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32 apollo_raw_gpon_gemUs_PTI_vector_write(uint8 pti_v0, uint8 pti_v1, uint8 pti_v2, uint8 pti_v3);

/* Function Name:
 *      apollo_raw_gpon_gemUs_PTI_vector_read
 * Description:
 *      Read the setting of force IDLE GEM.
 * Input:
 *      None
 * Output:
 *      pPti_v0             - the PTI vector 0
 *      pPti_v1             - the PTI vector 1
 *      pPti_v2             - the PTI vector 2
 *      pPti_v3             - the PTI vector 3
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_PTI_vector_read(uint8 *pPti_v0, uint8 *pPti_v1, uint8 *pPti_v2, uint8 *pPti_v3);

#if 0 /* GPON_GEM_US_OPT_CFG is removed in GPON_MAC_SWIO_r1.1 */
/* Function Name:
 *      apollo_raw_gpon_gemUs_txDis_write
 * Description:
 *      Set the use of TX disable.
 * Input:
 *      use_tx_dis  - specify to use TX disable or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txDis_write(uint32 use_tx_dis);

/* Function Name:
 *      apollo_raw_gpon_gemUs_txDis_read
 * Description:
 *      Read the setting of TX disable for power saving.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txDis_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_optBehindCycle_write
 * Description:
 *      Set the optical behind cycle.
 * Input:
 *      cycle   - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optBehindCycle_write(uint32 cycle);

/* Function Name:
 *      apollo_raw_gpon_gemUs_optBehindCycle_read
 * Description:
 *      Read the setting of optical behind cycle.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optBehindCycle_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_optAheadCycle_write
 * Description:
 *      Set the optical ahead cycle.
 * Input:
 *      cycle   - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optAheadCycle_write(uint32 cycle);

/* Function Name:
 *      apollo_raw_gpon_gemUs_optAheadCycle_read
 * Description:
 *      Read the setting of optical ahead cycle.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_optAheadCycle_read(uint32 *pValue);
#endif

#if 0 /* GPON_GEM_US_OPT_SD will move to GTC_US */
/* Function Name:
 *      apollo_raw_gpon_gemUs_txSd_read
 * Description:
 *      Indicate the TX Signal Detect vaild.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of status
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txSd_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_txSdPolar_write
 * Description:
 *      Set the palarity setting of TX Signal Detect vaild.
 * Input:
 *      value       - specify the polarity value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txSdPolar_write(uint32 value);

/* Function Name:
 *      apollo_raw_gpon_gemUs_txSdPolar_read
 * Description:
 *      Read the palarity setting of TX Signal Detect vaild.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_txSdPolar_read(uint32 *pValue);

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdDiffCycle_write
 * Description:
 *      Set the setting of SD difference cycle.
 * Input:
 *      cycle       - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdDiffCycle_write(uint32 cycle);

/* Function Name:
 *      apollo_raw_gpon_gemUs_sdDiffCycle_read
 * Description:
 *      Read the setting of SD difference cycle.
 * Input:
 *      None
 * Output:
 *      pValue      - return value of setting
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_sdDiffCycle_read(uint32 *pValue);
#endif

/* Function Name:
 *      apollo_raw_gpon_gemUs_ethCnt_read
 * Description:
 *      Read the ether counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pEth_cntr           - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 apollo_raw_gpon_gemUs_ethCnt_read(uint32 local_idx, uint32 *pEth_cntr);

/* Function Name:
 *      apollo_raw_gpon_gemUs_gemCnt_read
 * Description:
 *      Read the ether counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pGem_cntr           - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 apollo_raw_gpon_gemUs_gemCnt_read(uint32 local_idx, uint32 *pGem_cntr);

/* Function Name:
 *      apollo_raw_gpon_gemUs_portCfg_write
 * Description:
 *      Set the mapping of local_idx and gem_port_id.
 * Input:
 *      cycle       - specify the cycle value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      Others              - Failed
 * Note:
 */
int32 apollo_raw_gpon_gemUs_portCfg_write(uint32 local_idx, uint32 gem_port_id);

/* Function Name:
 *      apollo_raw_gpon_gemUs_portCfg_read
 * Description:
 *      Read the mapping of local_idx and gem_port_id.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 apollo_raw_gpon_gemUs_portCfg_read(uint32 local_idx, uint32 *pGem_port_id);

/* Function Name:
 *      apollo_raw_gpon_gemUs_dataByteCnt_read
 * Description:
 *      Read the data byte counter for local port.
 * Input:
 *      local_idx           - local port index
 * Output:
 *      pByte_cntr          - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 apollo_raw_gpon_gemUs_dataByteCnt_read(uint32 local_idx, uint64 *pByte_cntr);

/* Function Name:
 *      apollo_raw_gpon_gemUs_idleByteCnt_read
 * Description:
 *      Read the IDLE byte counter for TCONT.
 * Input:
 *      local_idx           - local TCONT index
 * Output:
 *      pIdle_cntr          - counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 */
int32 apollo_raw_gpon_gemUs_idleByteCnt_read(uint32 local_idx, uint64 *pIdle_cntr);

#if defined(OLD_FPGA_DEFINED)
int32 apollo_raw_gpon_pktGen_cfg_write(uint32 item, uint32 tcont, uint32 len, uint32 gem, int32 omci);
int32 apollo_raw_gpon_pktGen_buf_write(uint32 item, uint8 *buf, uint32 len);
#if 0
void apollo_raw_gpon_fpga_patch(void);
#endif
#endif

#endif /*#ifndef _APOLLOMP_RAW_GPON_H_*/

