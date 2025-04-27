/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : register service APIs in the SDK.
 *
 * Feature : register service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <osal/memory.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#endif
#if defined(CONFIG_SDK_RTL9310)
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#endif
#include <hal/common/halctrl.h>
#include <hal/common/miim.h>
#include <hal/mac/reg.h>
#include <hal/mac/mac_debug.h>
#include <hal/mac/mem.h>
#include <hal/mac/serdes.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#if defined(CONFIG_SDK_RTL9310)
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <dal/mango/dal_mango_sds.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#endif

/*
 * Data Declaration
 */

#define RTL838x_PAGE_CNT_MASK 0x7ff

#define MAX_STRING_LEN 256
#define MAX_BUF_LEN   16
#if defined(CONFIG_SDK_RTL8390)
const static uint16 outQ_usedPage_fieldidx[] = {CYPRESS_OUT_Q_Q0_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q1_USED_PAGE_CNTtf,
                                                CYPRESS_OUT_Q_Q2_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q3_USED_PAGE_CNTtf,
                                                CYPRESS_OUT_Q_Q4_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q5_USED_PAGE_CNTtf,
                                                CYPRESS_OUT_Q_Q6_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q7_USED_PAGE_CNTtf};
const static uint16 outQ_maxUsedPage_fieldidx[] = {CYPRESS_OUT_Q_Q0_MAX_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q1_MAX_USED_PAGE_CNTtf,
                                                CYPRESS_OUT_Q_Q2_MAX_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q3_MAX_USED_PAGE_CNTtf,
                                                CYPRESS_OUT_Q_Q4_MAX_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q5_MAX_USED_PAGE_CNTtf,
                                                CYPRESS_OUT_Q_Q6_MAX_USED_PAGE_CNTtf, CYPRESS_OUT_Q_Q7_MAX_USED_PAGE_CNTtf};
#endif
#if defined(CONFIG_SDK_RTL9310)
const static uint16 outQ_used_page_fieldidx[] = {MANGO_OUT_Q_USED_PAGE_CNT_Q0tf, MANGO_OUT_Q_USED_PAGE_CNT_Q1tf,
                                                MANGO_OUT_Q_USED_PAGE_CNT_Q2tf, MANGO_OUT_Q_USED_PAGE_CNT_Q3tf,
                                                MANGO_OUT_Q_USED_PAGE_CNT_Q4tf, MANGO_OUT_Q_USED_PAGE_CNT_Q5tf,
                                                MANGO_OUT_Q_USED_PAGE_CNT_Q6tf, MANGO_OUT_Q_USED_PAGE_CNT_Q7tf,
                                                MANGO_OUT_Q_USED_PAGE_CNT_Q8tf, MANGO_OUT_Q_USED_PAGE_CNT_Q9tf,
                                                MANGO_OUT_Q_USED_PAGE_CNT_Q10tf, MANGO_OUT_Q_USED_PAGE_CNT_Q11tf};
const static uint16 outQ_max_used_page_fieldidx[] = {MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q0tf, MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q1tf,
                                                MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q2tf, MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q3tf,
                                                MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q4tf, MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q5tf,
                                                MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q6tf, MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q7tf,
                                                MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q8tf, MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q9tf,
                                                MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q10tf, MANGO_OUT_Q_MAX_USED_PAGE_CNT_Q11tf};
#endif

enum LAYER34FMT
{
    LAYER34_UKONWN = 0,
    IPPRO_ARP,
    IPV4_ICMP,
    IPV4_IGMP,
    IPV4_TCP,
    IPV4_UDP,
    IPV4_UNKNOWN, /*6*/
    IPV6_ICMPV6,
    IPV6_TCP,
    IPV6_UDP,
    IPV6_UNKNOWN,  /*10*/
    L34FMT_MAX
};

extern uint32 waMon_enable;
extern uint32 pktBuf_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 macSerdes_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 phy_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 fiber_rx_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 phySerdes_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 phy95rClkRecov_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 phy95rClkRecovOK_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 drainTx_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 drainTxPathHasPkt_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 drainTxQFailed_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];
extern uint32 syncPhyToMacFailed_watchdog_cnt[RTK_MAX_NUM_OF_UNIT];

/*
 * Function Declaration
 */
int32 _rtk_getHsb(uint32 unit, hsb_param_t * hsb)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)  || defined(CONFIG_SDK_RTL9300)
    uint32 value;
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)  || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32 tmpVal;
#endif
    hal_control_t *pInfo;

    if(NULL == hsb)
    {
        return RT_ERR_FAILED;
    }

    osal_memset(hsb, 0, sizeof(hsb_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        /* HSB_DATA0 */
        reg_read(unit, CYPRESS_HSB_DATA0r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA0r, CYPRESS_PAGE_CNTf, &tmpVal, &value);
        hsb->r8390.page_cnt = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA0r, CYPRESS_ENDSCf, &tmpVal, &value);
        hsb->r8390.endsc = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA0r, CYPRESS_BGDSCf, &tmpVal, &value);
        hsb->r8390.bgsc = tmpVal;

        /* HSB_DATA1 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA1r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA1r, CYPRESS_FIELD_SELTOR11f, &tmpVal, &value);
        hsb->r8390.field_seltor11 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA1r, CYPRESS_FIELD_SELTOR10f, &tmpVal, &value);
        hsb->r8390.field_seltor10 = tmpVal;

        /* HSB_DATA2 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA2r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA2r, CYPRESS_FIELD_SELTOR9f, &tmpVal, &value);
        hsb->r8390.field_seltor9 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA2r, CYPRESS_FIELD_SELTOR8f, &tmpVal, &value);
        hsb->r8390.field_seltor8 = tmpVal;

        /* HSB_DATA3 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA3r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA3r, CYPRESS_FIELD_SELTOR7f, &tmpVal, &value);
        hsb->r8390.field_seltor7 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA3r, CYPRESS_FIELD_SELTOR6f, &tmpVal, &value);
        hsb->r8390.field_seltor6 = tmpVal;

        /* HSB_DATA4 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA4r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA4r, CYPRESS_FIELD_SELTOR5f, &tmpVal, &value);
        hsb->r8390.field_seltor5 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA4r, CYPRESS_FIELD_SELTOR4f, &tmpVal, &value);
        hsb->r8390.field_seltor4 = tmpVal;

        /* HSB_DATA5 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA5r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA5r, CYPRESS_FIELD_SELTOR3f, &tmpVal, &value);
        hsb->r8390.field_seltor3 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA5r, CYPRESS_FIELD_SELTOR2f, &tmpVal, &value);
        hsb->r8390.field_seltor2 = tmpVal;

        /* HSB_DATA6 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA6r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA6r, CYPRESS_FIELD_SELTOR1f, &tmpVal, &value);
        hsb->r8390.field_seltor1 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA6r, CYPRESS_FIELD_SELTOR0f, &tmpVal, &value);
        hsb->r8390.field_seltor0 = tmpVal;

        /* HSB_DATA7 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA7r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA7r, CYPRESS_FIELD_SEL_VLDf, &tmpVal, &value);
        hsb->r8390.field_sel_vld = tmpVal;

        /* HSB_DATA8 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA8r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA8r, CYPRESS_IMPLS_EXPf, &tmpVal, &value);
        hsb->r8390.impls_exp = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA8r, CYPRESS_IMPLS_LABELf, &tmpVal, &value);
        hsb->r8390.impls_label = tmpVal;

        /* HSB_DATA9 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA9r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA9r, CYPRESS_OMPLS_EXPf, &tmpVal, &value);
        hsb->r8390.ompls_exp = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA9r, CYPRESS_OMPLS_LABELf, &tmpVal, &value);
        hsb->r8390.ompls_label = tmpVal;

        /* HSB_DATA10 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA10r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA10r, CYPRESS_ARPOPCODEf, &tmpVal, &value);
        hsb->r8390.arpopcode = tmpVal;

        /* HSB_DATA11 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA11r, CYPRESS_TGT_MAC_47_32f, &value);
        hsb->r8390.target_mac[0] = (uint8)(value >> 8) & 0xff;
        hsb->r8390.target_mac[1] = (uint8)(value >> 0) & 0xff;

        /* HSB_DATA12 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA12r, CYPRESS_TGT_MAC_31_0f, &value);
        hsb->r8390.target_mac[2] = (uint8)(value >> 24) & 0xff;
        hsb->r8390.target_mac[3] = (uint8)(value >> 16) & 0xff;
        hsb->r8390.target_mac[4] = (uint8)(value >> 8) & 0xff;
        hsb->r8390.target_mac[5] = (uint8)(value >> 0) & 0xff;

        /* HSB_DATA13 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA13r, CYPRESS_SENDER_MAC_47_32f, &value);
        hsb->r8390.sender_mac[0] = (value >> 8) & 0xff;
        hsb->r8390.sender_mac[1] = (value >> 0) & 0xff;

        /* HSB_DATA14 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA14r, CYPRESS_SENDER_MAC_31_0f, &value);
        hsb->r8390.sender_mac[2] = (value >> 24) & 0xff;
        hsb->r8390.sender_mac[3] = (value >> 16) & 0xff;
        hsb->r8390.sender_mac[4] = (value >> 8) & 0xff;
        hsb->r8390.sender_mac[5] = (value >> 0) & 0xff;

        /* HSB_DATA15 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA15r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA15r, CYPRESS_DPORTf, &tmpVal, &value);
        hsb->r8390.dport = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA15r, CYPRESS_SPORTf, &tmpVal, &value);
        hsb->r8390.sport = tmpVal;

        /* HSB_DATA16 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA16r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA16r, CYPRESS_TCP_FLAGf, &tmpVal, &value);
        hsb->r8390.tcp_flag = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA16r, CYPRESS_IP_VERf, &tmpVal, &value);
        hsb->r8390.ipver = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA16r, CYPRESS_IP_TTLf, &tmpVal, &value);
        hsb->r8390.ipttl = tmpVal;

        /* HSB_DATA17 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA17r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA17r, CYPRESS_IP_TOSf, &tmpVal, &value);
        hsb->r8390.ip_tos = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA17r, CYPRESS_IP_PROTOCOLf, &tmpVal, &value);
        hsb->r8390.ip_protocol = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA17r, CYPRESS_IP_FLAGf, &tmpVal, &value);
        hsb->r8390.ip_flag = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA17r, CYPRESS_IP_OFFSETf, &tmpVal, &value);
        hsb->r8390.ip_offset = tmpVal;

        /* HSB_DATA18 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA18r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA18r, CYPRESS_DIPf, &tmpVal, &value);
        hsb->r8390.dip = tmpVal;

        /* HSB_DATA19 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA19r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA19r, CYPRESS_SIPf, &tmpVal, &value);
        hsb->r8390.sip = tmpVal;

        /* HSB_DATA20 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA20r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA20r, CYPRESS_OTAGf, &tmpVal, &value);
        hsb->r8390.otag = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA20r, CYPRESS_ITAGf, &tmpVal, &value);
        hsb->r8390.itag = tmpVal;

        /* HSB_DATA21 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA21r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA21r, CYPRESS_OTPID_IDXf, &tmpVal, &value);
        hsb->r8390.otpid_idx = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA21r, CYPRESS_ITPID_IDXf, &tmpVal, &value);
        hsb->r8390.itpid_idx = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA21r, CYPRESS_ETPf, &tmpVal, &value);
        hsb->r8390.etp = tmpVal;

        /* HSB_DATA22 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA22r, CYPRESS_SMAC_47_32f, &value);
        hsb->r8390.smac[0] = (value >> 8) & 0xff;
        hsb->r8390.smac[1] = (value >> 0) & 0xff;

        /* HSB_DATA23 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA23r, CYPRESS_SMAC_31_0f, &value);
        hsb->r8390.smac[2] = (value >> 24) & 0xff;
        hsb->r8390.smac[3] = (value >> 16) & 0xff;
        hsb->r8390.smac[4] = (value >> 8) & 0xff;
        hsb->r8390.smac[5] = (value >> 0) & 0xff;

        /* HSB_DATA24 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA24r, CYPRESS_DMAC_47_32f, &value);
        hsb->r8390.dmac[0] = (value >> 8) & 0xff;
        hsb->r8390.dmac[1] = (value >> 0) & 0xff;

        /* HSB_DATA25 */
        value = 0;
        reg_field_read(unit, CYPRESS_HSB_DATA25r, CYPRESS_DMAC_31_0f, &value);
        hsb->r8390.dmac[2] = (value >> 24) & 0xff;
        hsb->r8390.dmac[3] = (value >> 16) & 0xff;
        hsb->r8390.dmac[4] = (value >> 8) & 0xff;
        hsb->r8390.dmac[5] = (value >> 0) & 0xff;

        /* HSB_DATA26 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA26r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPLENf, &tmpVal, &value);
        hsb->r8390.iplen = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV4HDLENf, &tmpVal, &value);
        hsb->r8390.ipv4hdlen = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_TCPSEQZEROf, &tmpVal, &value);
        hsb->r8390.tcpseqzero = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6HOPf, &tmpVal, &value);
        hsb->r8390.ipv6hop = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6ROUTf, &tmpVal, &value);
        hsb->r8390.ipv6rout = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6FRAGf, &tmpVal, &value);
        hsb->r8390.ipv6frag = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6DESTf, &tmpVal, &value);
        hsb->r8390.ipv6dest = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6AUTHf, &tmpVal, &value);
        hsb->r8390.ipv6auth = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPCSOKf, &tmpVal, &value);
        hsb->r8390.ipcsok = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6EXT_LONGf, &tmpVal, &value);
        hsb->r8390.ipv6ext_long = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV6f, &tmpVal, &value);
        hsb->r8390.ipv6 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA26r, CYPRESS_IPV4f, &tmpVal, &value);
        hsb->r8390.ipv4 = tmpVal;

        /* HSB_DATA27 */
        value = 0;
        reg_read(unit, CYPRESS_HSB_DATA27r, &value);
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_OMPLS_IFf, &tmpVal, &value);
        hsb->r8390.ompls_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_IMPLS_IFf, &tmpVal, &value);
        hsb->r8390.impls_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_ETAG_IFf, &tmpVal, &value);
        hsb->r8390.etag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_CPUTAG_IFf, &tmpVal, &value);
        hsb->r8390.cputag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_OTAG_IFf, &tmpVal, &value);
        hsb->r8390.otag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_ITAG_IFf, &tmpVal, &value);
        hsb->r8390.itag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_OAMPDUf, &tmpVal, &value);
        hsb->r8390.oampdu = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_LLC_OTHERf, &tmpVal, &value);
        hsb->r8390.llc_other = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_PPPOE_IFf, &tmpVal, &value);
        hsb->r8390.pppoe_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_RFC_1042f, &tmpVal, &value);
        hsb->r8390.rfc1042 = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_SPNf, &tmpVal, &value);
        hsb->r8390.spa = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_ERRPKTf, &tmpVal, &value);
        hsb->r8390.errpkt = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_L4HDCHKf, &tmpVal, &value);
        hsb->r8390.l4hdchk = tmpVal;
        reg_field_get(unit, CYPRESS_HSB_DATA27r, CYPRESS_PKTLENf, &tmpVal, &value);
        hsb->r8390.pktlen = tmpVal;
    }
    else
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        /* HSB_DATA0 */
        reg_read(unit, MAPLE_HSB_DATA0r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA0r, MAPLE_IP_FRG_OFFSETf, &tmpVal, &value);
        hsb->r8380.ip_offset = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA0r, MAPLE_IP_PROTOf, &tmpVal, &value);
        hsb->r8380.ip_protocol = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA0r, MAPLE_IP_TOSf, &tmpVal, &value);
        hsb->r8380.ip_dstype = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA0r, MAPLE_IP_MORE_FLAGf, &tmpVal, &value);
        hsb->r8380.ip_mf = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA0r, MAPLE_RTK_PPf, &tmpVal, &value);
        hsb->r8380.rtkpp = tmpVal;

        /* HSB_DATA1 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA1r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_CPU_TAG_EXISTf, &tmpVal, &value);
        hsb->r8380.ctag_exist = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_ETAG_EXISTf, &tmpVal, &value);
        hsb->r8380.etag_exist = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_RTAG_EXISTf, &tmpVal, &value);
        hsb->r8380.rtag_exist = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_OTAG_EXISTf, &tmpVal, &value);
        hsb->r8380.otag_exist = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_ITAG_EXISTf, &tmpVal, &value);
        hsb->r8380.itag_exist = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_IP_TTLf, &tmpVal, &value);
        hsb->r8380.ip_ttl = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA1r, MAPLE_IP_LENGTHf, &tmpVal, &value);
        hsb->r8380.ip_length = tmpVal;

        /* HSB_DATA2 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA2r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA2r, MAPLE_SIP_31_0f, &tmpVal, &value);
        hsb->r8380.sip[15] = tmpVal & 0xff;
        hsb->r8380.sip[14] = (tmpVal >> 8) & 0xff;
        hsb->r8380.sip[13] = (tmpVal >> 16) & 0xff;
        hsb->r8380.sip[12] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA3 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA3r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA3r, MAPLE_SIP_63_32f, &tmpVal, &value);
        hsb->r8380.sip[11] = tmpVal & 0xff;
        hsb->r8380.sip[10] = (tmpVal >> 8) & 0xff;
        hsb->r8380.sip[9] = (tmpVal >> 16) & 0xff;
        hsb->r8380.sip[8] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA4 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA4r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA4r, MAPLE_SIP_95_64f, &tmpVal, &value);
        hsb->r8380.sip[7] = tmpVal & 0xff;
        hsb->r8380.sip[6] = (tmpVal >> 8) & 0xff;
        hsb->r8380.sip[5] = (tmpVal >> 16) & 0xff;
        hsb->r8380.sip[4] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA5 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA5r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA5r, MAPLE_SIP_127_96f, &tmpVal, &value);
        hsb->r8380.sip[3] = tmpVal & 0xff;
        hsb->r8380.sip[2] = (tmpVal >> 8) & 0xff;
        hsb->r8380.sip[1] = (tmpVal >> 16) & 0xff;
        hsb->r8380.sip[0] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA6 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA6r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA6r, MAPLE_DIP_31_0f, &tmpVal, &value);
        hsb->r8380.dip[15] = tmpVal & 0xff;
        hsb->r8380.dip[14] = (tmpVal >> 8) & 0xff;
        hsb->r8380.dip[13] = (tmpVal >> 16) & 0xff;
        hsb->r8380.dip[12] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA7 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA7r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA7r, MAPLE_DIP_63_32f, &tmpVal, &value);
        hsb->r8380.dip[11] = tmpVal & 0xff;
        hsb->r8380.dip[10] = (tmpVal >> 8) & 0xff;
        hsb->r8380.dip[9] = (tmpVal >> 16) & 0xff;
        hsb->r8380.dip[8] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA8 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA8r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA8r, MAPLE_DIP_95_64f, &tmpVal, &value);
        hsb->r8380.dip[7] = tmpVal & 0xff;
        hsb->r8380.dip[6] = (tmpVal >> 8) & 0xff;
        hsb->r8380.dip[5] = (tmpVal >> 16) & 0xff;
        hsb->r8380.dip[4] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA9 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA9r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA9r, MAPLE_DIP_127_96f, &tmpVal, &value);
        hsb->r8380.dip[3] = tmpVal & 0xff;
        hsb->r8380.dip[2] = (tmpVal >> 8) & 0xff;
        hsb->r8380.dip[1] = (tmpVal >> 16) & 0xff;
        hsb->r8380.dip[0] = (tmpVal >> 24) & 0xff;

        /* HSB_DATA10 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA10r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_FRAME_TYPEf, &tmpVal, &value);
        hsb->r8380.frame_type = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_IPV4_PKTf, &tmpVal, &value);
        hsb->r8380.ipv4_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_IPV6_PKTf, &tmpVal, &value);
        hsb->r8380.ipv6_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_ARP_PKTf, &tmpVal, &value);
        hsb->r8380.arp_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_PPPOE_PKTf, &tmpVal, &value);
        hsb->r8380.pppoe_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_TCP_PKTf, &tmpVal, &value);
        hsb->r8380.tcp_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_UDP_PKTf, &tmpVal, &value);
        hsb->r8380.udp_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_IGMP_PKTf, &tmpVal, &value);
        hsb->r8380.igmp_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_ICMP_PKTf, &tmpVal, &value);
        hsb->r8380.icmp_pkt = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_RSPAN_RMf, &tmpVal, &value);
        hsb->r8380.rx_rm_rtag = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_TCP_SN_EQ_0f, &tmpVal, &value);
        hsb->r8380.tcp_sn_eq_0 = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_TCP_FLAGf, &tmpVal, &value);
        hsb->r8380.tcp_flag = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_TCP_HDR_LENf, &tmpVal, &value);
        hsb->r8380.tcp_hdrlen = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_IPV4_DFf, &tmpVal, &value);
        hsb->r8380.ipv4_df = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA10r, MAPLE_IPV4_HDR_LENf, &tmpVal, &value);
        hsb->r8380.ipv4_hdrlen = tmpVal;

        /* HSB_DATA11 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA11r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA11r, MAPLE_L4_CONTENTf, &tmpVal, &value);
        hsb->r8380.l4_content = tmpVal;

        /* HSB_DATA12 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA12r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA12r, MAPLE_OTAG_CONTENTf, &tmpVal, &value);
        hsb->r8380.otag_content = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA12r, MAPLE_ITAG_CONTENTf, &tmpVal, &value);
        hsb->r8380.itag_content = tmpVal;

        /* HSB_DATA13 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA13r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA13r, MAPLE_CPU_TAG_CONTENT_31_0f, &tmpVal, &value);
        hsb->r8380.ctag_dpm = tmpVal & 0x1fffffff;
        hsb->r8380.ctag_as_dpm = (tmpVal >> 30) & 0x1;
        hsb->r8380.ctag_physical_dpm = (tmpVal >> 29) & 0x1;

        /* HSB_DATA14 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA14r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA14r, MAPLE_FS_VALIDf, &tmpVal, &value);
        hsb->r8380.fs3_valid = (tmpVal & 0x8) >> 3;
        hsb->r8380.fs2_valid = (tmpVal & 0x4) >> 2;
        hsb->r8380.fs1_valid = (tmpVal & 0x2) >> 1;
        hsb->r8380.fs0_valid = tmpVal & 0x1;
        reg_field_get(unit, MAPLE_HSB_DATA14r, MAPLE_TYPELENf, &tmpVal, &value);
        hsb->r8380.typelen = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA14r, MAPLE_CRC_EQf, &tmpVal, &value);
        hsb->r8380.crceq = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA14r, MAPLE_CPU_TAG_CONTENT_42_32f, &tmpVal, &value);
        hsb->r8380.ctag_bp_fltr_1 = (tmpVal >> 10) & 0x1;
        hsb->r8380.ctag_bp_fltr_2 = (tmpVal >> 9) & 0x1;
        hsb->r8380.ctag_as_tagsts = (tmpVal >> 8) & 0x1;
        hsb->r8380.ctag_acl_act = (tmpVal >> 7) & 0x1;
        hsb->r8380.ctag_rvid_sel = (tmpVal >> 6) & 0x1;
        hsb->r8380.ctag_l2learning = (tmpVal >> 5) & 0x1;
        hsb->r8380.ctag_as_pri = (tmpVal >> 4) & 0x1;
        hsb->r8380.ctag_pri = (tmpVal >> 1) & 0x7;

        /* HSB_DATA15 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA15r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA15r, MAPLE_FS1_DATAf, &tmpVal, &value);
        hsb->r8380.fs1_data = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA15r, MAPLE_FS0_DATAf, &tmpVal, &value);
        hsb->r8380.fs0_data = tmpVal;

        /* HSB_DATA16 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA16r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA16r, MAPLE_FS3_DATAf, &tmpVal, &value);
        hsb->r8380.fs3_data = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA16r, MAPLE_FS2_DATAf, &tmpVal, &value);
        hsb->r8380.fs2_data = tmpVal;

        /* HSB_DATA17 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA17r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA17r, MAPLE_SA_31_0f, &tmpVal, &value);
        hsb->r8380.smac[2] = (tmpVal >> 24) & 0xff;
        hsb->r8380.smac[3] = (tmpVal >> 16) & 0xff;
        hsb->r8380.smac[4] = (tmpVal >> 8) & 0xff;
        hsb->r8380.smac[5] = tmpVal & 0xff;

        /* HSB_DATA18 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA18r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA18r, MAPLE_PKT_LENf, &tmpVal, &value);
        hsb->r8380.pktlen = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA18r, MAPLE_SA_47_32f, &tmpVal, &value);
        hsb->r8380.smac[0] = (tmpVal >> 8) & 0xff;
        hsb->r8380.smac[1] = tmpVal & 0xff;
        reg_field_get(unit, MAPLE_HSB_DATA18r, MAPLE_IPV6_MOB_EXT_HDRf, &tmpVal, &value);
        hsb->r8380.ipv6_mob_ext_hdr = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA18r, MAPLE_IPV6_HBH_EXT_HDR_ERRf, &tmpVal, &value);
        hsb->r8380.ipv6_hbh_ext_hdr_err = tmpVal;

        /* HSB_DATA19 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA19r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA19r, MAPLE_DA_31_0f, &tmpVal, &value);
        hsb->r8380.dmac[2] = (tmpVal >> 24) & 0xff;
        hsb->r8380.dmac[3] = (tmpVal >> 16) & 0xff;
        hsb->r8380.dmac[4] = (tmpVal >> 8) & 0xff;
        hsb->r8380.dmac[5] = tmpVal & 0xff;

        /* HSB_DATA20 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA20r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA20r, MAPLE_DA_47_32f, &tmpVal, &value);
        hsb->r8380.dmac[0] = (tmpVal >> 8) & 0xff;
        hsb->r8380.dmac[1] = tmpVal & 0xff;

        /* HSB_DATA21 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA21r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA21r, MAPLE_IPV6_EXT_HDR_LENf, &tmpVal, &value);
        hsb->r8380.ipv6_extension_hdrlen = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA21r, MAPLE_OTAG_IDXf, &tmpVal, &value);
        hsb->r8380.otag_index = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA21r, MAPLE_ITAG_IDXf, &tmpVal, &value);
        hsb->r8380.itag_index = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA21r, MAPLE_RX_DROPf, &tmpVal, &value);
        hsb->r8380.rxdrop = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA21r, MAPLE_OAM_PKTf, &tmpVal, &value);
        hsb->r8380.oampdu = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA21r, MAPLE_SRC_PHYf, &tmpVal, &value);
        hsb->r8380.sphy = tmpVal;

        /* HSB_DATA22 */
        value = 0;
        reg_read(unit, MAPLE_HSB_DATA22r, &value);
        reg_field_get(unit, MAPLE_HSB_DATA22r, MAPLE_HSB_RSVDf, &tmpVal, &value);

        reg_field_get(unit, MAPLE_HSB_DATA22r, MAPLE_IPV6_FLOW_LABELf, &tmpVal, &value);
        hsb->r8380.ipv6_flow_label = tmpVal;
        reg_field_get(unit, MAPLE_HSB_DATA22r, MAPLE_IPV6_UNKNOWN_HDRf, &tmpVal, &value);
        hsb->r8380.ipv6_unknown_hdr = tmpVal;
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        /* HSB_DATA0 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA0r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA0r, LONGAN_STHDR_VLDf, &tmpVal, &value);
        hsb->r9300.sthdr_vld = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA0r, LONGAN_LBHDR_VLDf, &tmpVal, &value);
        hsb->r9300.lbhdr_vld = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA0r, LONGAN_FST_DSCf, &tmpVal, &value);
        hsb->r9300.fst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA0r, LONGAN_LST_DSCf, &tmpVal, &value);
        hsb->r9300.lst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA25r, LONGAN_PAGE_CNTf, &tmpVal, &value);
        hsb->r9300.page_cnt = tmpVal;

        /* HSB_DATA1 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA1r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA1r, LONGAN_STHDR_0_3f, &tmpVal, &value);
        hsb->r9300.sthdr_txCtag_fwd_type = (tmpVal>>28) & 0xf;
        hsb->r9300.sthdr_txCtag_acl_act = (tmpVal>>27) & 0x1;
        hsb->r9300.sthdr_txCtag_cngst_drop = (tmpVal>>26) & 0x1;
        hsb->r9300.sthdr_txCtag_dm_pkt = (tmpVal>>25) & 0x1;
        hsb->r9300.sthdr_txCtag_dg_pkt = (tmpVal>>24) & 0x1;
        hsb->r9300.sthdr_txCtag_bp_filter = (tmpVal>>23) & 0x1;
        hsb->r9300.sthdr_txCtag_bp_stp = (tmpVal>>22) & 0x1;
        hsb->r9300.sthdr_txCtag_bp_vlan_egr = (tmpVal>>21) & 0x1;
        hsb->r9300.sthdr_txCtag_ale_as_tagsts = (tmpVal>>20) & 0x1;
        hsb->r9300.sthdr_txCtag_l3_act = (tmpVal>>19) & 0x1;
        hsb->r9300.sthdr_txCtag_as_qid = (tmpVal>>18) & 0x1;
        hsb->r9300.sthdr_txCtag_qid = (tmpVal>>13) & 0x1f;
        hsb->r9300.sthdr_txCtag_src_filter_en = (tmpVal>>12) & 0x1;
        hsb->r9300.sthdr_txCtag_sp_is_trk = (tmpVal>>11) & 0x1;
        hsb->r9300.sthdr_txCtag_spn = (tmpVal>>1) & 0x3ff;
        hsb->r9300.sthdr_txCtag_sw_unit = (((tmpVal>>0) & 0x1)<<3) & 0x8;

        hsb->r9300.sthdr_rxCtag_of_lu_mis_tbl_id = (tmpVal>>30) & 0x3;
        hsb->r9300.sthdr_rxCtag_acl_of_hit = (tmpVal>>28) & 0x3;
        hsb->r9300.sthdr_rxCtag_of_tbl_id = (tmpVal>>26) & 0x3;
        hsb->r9300.sthdr_rxCtag_l2_err_pkt = (tmpVal>>25) & 0x1;
        hsb->r9300.sthdr_rxCtag_l3_err_pkt = (tmpVal>>24) & 0x1;
        hsb->r9300.sthdr_rxCtag_atk_type = (tmpVal>>19) & 0x1f;
        hsb->r9300.sthdr_rxCtag_qid = (tmpVal>>14) & 0x1f;
        hsb->r9300.sthdr_rxCtag_idx = (((tmpVal>>0) & 0x3fff)<<1) & 0x7ffe;


        /* HSB_DATA2 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA2r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA2r, LONGAN_STHDR_4_7f, &tmpVal, &value);
        hsb->r9300.sthdr_txCtag_sw_unit |= ((tmpVal>>29) & 0x7);
        hsb->r9300.sthdr_txCtag_dpm.bits[1] = (((tmpVal>>0) & 0x1fffffe0)>>5) & 0xffffff;
        hsb->r9300.sthdr_txCtag_dpm.bits[0] = (((tmpVal>>0) & 0x1f)<<27) & 0xf8000000;

        hsb->r9300.sthdr_rxCtag_idx |= ((tmpVal>>31) & 0x1);
        hsb->r9300.sthdr_rxCtag_sflow = (tmpVal>>29) & 0x3;
        hsb->r9300.sthdr_rxCtag_tt_hit = (tmpVal>>28) & 0x1;
        hsb->r9300.sthdr_rxCtag_tt_idx = (tmpVal>>19) & 0x1ff;
        hsb->r9300.sthdr_rxCtag_mac_cst = (tmpVal>>18) & 0x1;
        hsb->r9300.sthdr_rxCtag_dm_rxidx_of_lb_times = (tmpVal>>12) & 0x3f;
        hsb->r9300.sthdr_rxCtag_new_sa = (tmpVal>>11) & 0x1;
        hsb->r9300.sthdr_rxCtag_pmv_forbid = (tmpVal>>10) & 0x1;
        hsb->r9300.sthdr_rxCtag_l2_sttc_pmv = (tmpVal>>9) & 0x1;
        hsb->r9300.sthdr_rxCtag_l2_dyn_pmv = (tmpVal>>8) & 0x1;
        hsb->r9300.sthdr_rxCtag_port_data_is_trk = (tmpVal>>7) & 0x1;
        hsb->r9300.sthdr_rxCtag_port_data = (((tmpVal>>0) & 0x7f)<<3) & 0x3f8;

        /* HSB_DATA3 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA3r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA3r, LONGAN_STHDR_8_11f, &tmpVal, &value);
        hsb->r9300.sthdr_txCtag_dpm.bits[0] |= ((tmpVal>>5) & 0x7ffffff);

        hsb->r9300.sthdr_rxCtag_port_data |= ((tmpVal>>29) & 0x7);
        hsb->r9300.sthdr_rxCtag_hash_full = ((tmpVal>>28) & 0x1);
        hsb->r9300.sthdr_rxCtag_invalid_sa = ((tmpVal>>27) & 0x1);
        hsb->r9300.sthdr_rxCtag_rsn = ((tmpVal>>21) & 0x3f);
        hsb->r9300.sthdr_acl_fwd_cpu_pkt_hit = ((tmpVal>>20) & 0x1);
        hsb->r9300.sthdr_acl_fwd_cpu_pkt_fmt = ((tmpVal>>19) & 0x1);

        /* HSB_DATA4 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA4r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA4r, LONGAN_STHDR_12_15f, &tmpVal, &value);
        hsb->r9300.ctag_fwd_type = (tmpVal) & 0xf;

        hsb->r9300.sthdr_comm_ver = (tmpVal>>30) & 0x3;
        hsb->r9300.sthdr_comm_info_hdr_type = (tmpVal>>29) & 0x1;
        hsb->r9300.sthdr_comm_sp_info = (tmpVal>>19) & 0x3ff;
        hsb->r9300.sthdr_comm_sp_filter = (tmpVal>>18) & 0x1;
        hsb->r9300.sthdr_comm_fwd_type = (tmpVal>>16) & 0x3;
        hsb->r9300.sthdr_comm_dp_fmt = (tmpVal>>15) & 0x1;
        hsb->r9300.sthdr_comm_dp_info = (tmpVal>>3) & 0xfff;
        hsb->r9300.sthdr_comm_sa_lrn = (tmpVal>>2) & 0x1;
        hsb->r9300.sthdr_comm_vlan_tag = (tmpVal>>0) & 0x3;

        /* HSB_DATA5 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA5r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA5r, LONGAN_STHDR_16f, &tmpVal, &value);

        hsb->r9300.sthdr_vlan_ori_opri = (tmpVal>>5) & 0x7;
        hsb->r9300.sthdr_vlan_ori_ocfi = (tmpVal>>4) & 0x1;
        hsb->r9300.sthdr_vlan_ori_ipri = (tmpVal>>1) & 0x7;
        hsb->r9300.sthdr_vlan_ori_icfi = (tmpVal>>0) & 0x1;

        hsb->r9300.ctag_ale_as_tagsts = (tmpVal >> 0) & 0x1;
        hsb->r9300.ctag_bp_vlan_egr   = (tmpVal >> 1) & 0x1;
        hsb->r9300.ctag_bp_stp        = (tmpVal >> 2) & 0x1;
        hsb->r9300.ctag_bp_fltr       = (tmpVal >> 3) & 0x1;
        hsb->r9300.ctag_dg_pkt        = (tmpVal >> 4) & 0x1;
        hsb->r9300.ctag_cngst_drop    = (tmpVal >> 6) & 0x1;
        hsb->r9300.ctag_acl_act       = (tmpVal >> 7) & 0x1;

        /* HSB_DATA6 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA6r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA6r, LONGAN_FS10_DATAf, &tmpVal, &value);
        hsb->r9300.fs10_data = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA6r, LONGAN_FS11_DATAf, &tmpVal, &value);
        hsb->r9300.fs11_data = tmpVal;

        /* HSB_DATA7 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA7r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA7r, LONGAN_FS8_DATAf, &tmpVal, &value);
        hsb->r9300.fs8_data = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA7r, LONGAN_FS9_DATAf, &tmpVal, &value);
        hsb->r9300.fs9_data = tmpVal;

        /* HSB_DATA8 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA8r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA8r, LONGAN_FS6_DATAf, &tmpVal, &value);
        hsb->r9300.fs6_data = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA8r, LONGAN_FS7_DATAf, &tmpVal, &value);
        hsb->r9300.fs7_data = tmpVal;

        /* HSB_DATA9 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA9r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA9r, LONGAN_FS5_DATAf, &tmpVal, &value);

        hsb->r9300.fs5_data = tmpVal;
        hsb->r9300.ctag_l3_act       = (tmpVal >> 15) & 0x1;
        hsb->r9300.ctag_ori_tagif_en = (tmpVal >> 14) & 0x1;
        hsb->r9300.ctag_as_qid       = (tmpVal >> 13) & 0x1;
        hsb->r9300.ctag_qid          = (tmpVal >> 8) & 0xf;
        hsb->r9300.ctag_ori_itagif   = (tmpVal >> 7) & 0x1;
        hsb->r9300.ctag_ori_otagif   = (tmpVal >> 6) & 0x1;
        hsb->r9300.ctag_fwd_vid_sel  = (tmpVal >> 5) & 0x1;
        hsb->r9300.ctag_fwd_vid_en   = (tmpVal >> 4) & 0x1;

        hsb->r9300.lbhdr_org_srcport = (tmpVal >> 11) & 0x1f;
        hsb->r9300.lbhdr_lbttl       = (tmpVal >> 8) & 0x7;
        hsb->r9300.lbhdr_metadata    = (tmpVal >> 0) & 0xff;

        reg_field_get(unit, LONGAN_HSB_DATA9r, LONGAN_FS4_DATAf, &tmpVal, &value);

        hsb->r9300.fs4_data = tmpVal;
        hsb->r9300.sthdr_info_cpu_tag_if = (tmpVal>>14) &0x3;
        hsb->r9300.sthdr_info_int_pri = (tmpVal>>11) & 0x7;
        hsb->r9300.sthdr_info_trk_hash = (tmpVal>>5) & 0x3f;
        hsb->r9300.sthdr_info_class = (tmpVal>>3) & 0x3;
        hsb->r9300.sthdr_info_da_hit = (tmpVal>>2) & 0x1;
        hsb->r9300.sthdr_info_vb_iso_mbr = (tmpVal>>1) & 0x1;
        hsb->r9300.sthdr_info_int_otag_hit = (tmpVal>>0) & 0x1;

        hsb->r9300.ctag_sp_is_trk     = (tmpVal >> 6) & 0x1;
        hsb->r9300.ctag_src_filter_en = (tmpVal >> 7) & 0x1;
        hsb->r9300.ctag_fwd_vid |= (((hsb->r9300.fs5_data & 0xF) << 8) & 0xf00) | ((hsb->r9300.fs4_data >> 8) & 0xFF);

        /* HSB_DATA10 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA10r, &value);

        reg_field_get(unit, LONGAN_HSB_DATA10r, LONGAN_FS3_DATAf, &tmpVal, &value);
        hsb->r9300.sthdr_info_int_otag_if = (tmpVal>>15) & 0x1;
        hsb->r9300.sthdr_info_int_otag_tpid_hit = (tmpVal>>14) & 0x1;
        hsb->r9300.sthdr_info_int_otag_tpid_idx = (tmpVal>>12) & 0x3;
        hsb->r9300.sthdr_info_int_otag_pri_hit = (tmpVal>>11) & 0x1;
        hsb->r9300.sthdr_info_int_otag_pri = (tmpVal>>8) & 0x7;
        hsb->r9300.sthdr_info_int_ocfi = (tmpVal>>7) & 0x1;
        hsb->r9300.sthdr_info_int_ovid = (((tmpVal>>0) & 0x7f)<<5) & 0xfe0;

        hsb->r9300.fs3_data = tmpVal;
        hsb->r9300.ctag_dev_id = (hsb->r9300.fs3_data >>8) & 0xf;
        hsb->r9300.ctag_spn =  (((hsb->r9300.fs4_data  & 0x3F) << 4) & 0x3f0) | ((hsb->r9300.fs3_data >> 12) & 0xF);

        reg_field_get(unit, LONGAN_HSB_DATA10r, LONGAN_FS2_DATAf, &tmpVal, &value);
        hsb->r9300.sthdr_info_int_ovid |= ((tmpVal>>11) & 0x1f);
        hsb->r9300.sthdr_info_int_itag_hit = (tmpVal>>10) & 0x1;
        hsb->r9300.sthdr_info_int_itag_if = (tmpVal>>9) & 0x1;
        hsb->r9300.sthdr_info_int_itag_tpid_hit = (tmpVal>>8) & 0x1;
        hsb->r9300.sthdr_info_int_itag_tpid_idx = (tmpVal>>6) & 0x3;
        hsb->r9300.sthdr_info_int_itag_pri_hit = (tmpVal>>5) & 0x1;
        hsb->r9300.sthdr_info_int_itag_pri = (tmpVal>>2) & 0x7;
        hsb->r9300.sthdr_info_int_icfi = (tmpVal>>1) & 0x1;
        hsb->r9300.sthdr_info_int_ivid = (((tmpVal>>0) & 0x1) <<11) & 0x800;

        hsb->r9300.fs2_data = tmpVal;


        /* HSB_DATA11 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA11r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA11r, LONGAN_FS1_DATAf, &tmpVal, &value);
        hsb->r9300.sthdr_info_int_ivid = (tmpVal>>5) & 0x7ff;
        hsb->r9300.sthdr_info_fwd_base = (tmpVal>>4) & 0x1;
        hsb->r9300.sthdr_info_dp = (tmpVal>>2) & 0x3;
        hsb->r9300.sthdr_info_int_dscp_hit = (tmpVal>>1) & 0x1;
        hsb->r9300.sthdr_info_mir_id = ((tmpVal>>0) & 0x1)<<1;

        hsb->r9300.fs1_data = tmpVal;

        reg_field_get(unit, LONGAN_HSB_DATA11r, LONGAN_FS0_DATAf, &tmpVal, &value);
        hsb->r9300.sthdr_info_mir_id |= ((tmpVal>>15) & 0x1);
        hsb->r9300.sthdr_info_rout_tunnel_if = (tmpVal>>14) & 0x1;
        hsb->r9300.sthdr_info_igr_l3_intf_type = (tmpVal>>13) & 0x1;
        hsb->r9300.sthdr_info_igr_l3_intf_id = (tmpVal>>1) & 0xfff;

        hsb->r9300.fs0_data = tmpVal;

        hsb->r9300.ctag_dpm.bits[0] = ((hsb->r9300.fs1_data <<16) & 0xffff0000) | (hsb->r9300.fs0_data);
        hsb->r9300.ctag_dpm.bits[1] = ((hsb->r9300.fs3_data << 16) & 0xff0000) | (hsb->r9300.fs2_data);
        /* HSB_DATA12 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA12r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA12r, LONGAN_FLOW_LABELf, &tmpVal, &value);
        hsb->r9300.ipv6_flow_label = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA12r, LONGAN_FS_VALIDf, &tmpVal, &value);
        hsb->r9300.fs_valid = tmpVal;

        /* HSB_DATA13 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA13r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA13r, LONGAN_DSAP_SSAPf, &tmpVal, &value);
        hsb->r9300.dsap_ssap = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA13r, LONGAN_L2_PTPf, &tmpVal, &value);
        hsb->r9300.l2_ptp= tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA13r, LONGAN_UDP_PTPf, &tmpVal, &value);
        hsb->r9300.udp_ptp = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA13r, LONGAN_IGR_ERRf, &tmpVal, &value);
        hsb->r9300.igr_err = tmpVal;

        /* HSB_DATA14 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA14r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA14r, LONGAN_ARP_OPCODEf, &tmpVal, &value);
        hsb->r9300.arp_opcode = tmpVal;


        /* HSB_DATA15/16/17 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA17r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA17r, LONGAN_SENDER_MAC_31_0f, &tmpVal, &value);
        osal_memcpy(&hsb->r9300.sender_mac[2], &tmpVal, 4);
        hsb->r9300.sender_mac[2] = (tmpVal >> 24)& 0xff;
        hsb->r9300.sender_mac[3] = (tmpVal >> 16)& 0xff;
        hsb->r9300.sender_mac[4] = (tmpVal >> 8)& 0xff;
        hsb->r9300.sender_mac[5] = (tmpVal)& 0xff;
        reg_read(unit, LONGAN_HSB_DATA16r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA16r, LONGAN_SENDER_MAC_47_32f, &tmpVal, &value);
        hsb->r9300.sender_mac[0] = (tmpVal>>8) & 0xff;
        hsb->r9300.sender_mac[1] = (tmpVal) & 0xff;
        reg_field_get(unit, LONGAN_HSB_DATA16r, LONGAN_TARGET_MAC_15_0f, &tmpVal, &value);
        hsb->r9300.target_mac[4] = (tmpVal>>8) & 0xff;
        hsb->r9300.target_mac[5] = (tmpVal) & 0xff;
        reg_read(unit, LONGAN_HSB_DATA15r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA15r, LONGAN_TARGET_MAC_47_16f, &tmpVal, &value);
        osal_memcpy(&hsb->r9300.target_mac[0], &tmpVal, 4);
        hsb->r9300.target_mac[0] = (tmpVal >> 24)& 0xff;
        hsb->r9300.target_mac[1] = (tmpVal >> 16)& 0xff;
        hsb->r9300.target_mac[2] = (tmpVal >> 8)& 0xff;
        hsb->r9300.target_mac[3] = (tmpVal)& 0xff;

        /* HSB_DATA18 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA18r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA18r, LONGAN_SPORTf, &tmpVal, &value);
        hsb->r9300.sport = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA18r, LONGAN_DPORTf, &tmpVal, &value);
        hsb->r9300.dport = tmpVal;

        /* HSB_DATA19 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA19r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA19r, LONGAN_IP_PROTOCOLf, &tmpVal, &value);
        hsb->r9300.ip_protocol = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA19r, LONGAN_IP_TOSf, &tmpVal, &value);
        hsb->r9300.ip_tos = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA19r, LONGAN_IP_TTLf, &tmpVal, &value);
        hsb->r9300.ip_ttl = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA19r, LONGAN_TCP_FLAGf, &tmpVal, &value);
        hsb->r9300.tcp_flag = tmpVal;

        /* HSB_DATA20 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA20r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA20r, LONGAN_IP_OFFSETf, &tmpVal, &value);
        hsb->r9300.ip_offset = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA20r, LONGAN_IP_FLAGf, &tmpVal, &value);
        hsb->r9300.ip_flag = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA20r, LONGAN_IP_VERf, &tmpVal, &value);
        hsb->r9300.ip_ver= tmpVal;

        /* HSB_DATA21 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA21r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA21r, LONGAN_DIPf, &tmpVal, &value);
        hsb->r9300.dip = tmpVal;

        /* HSB_DATA22 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA22r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA22r, LONGAN_SIPf, &tmpVal, &value);
        hsb->r9300.sip = tmpVal;

        /* HSB_DATA23 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA23r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA23r, LONGAN_ITAG_CONTENTf, &tmpVal, &value);
        hsb->r9300.ipri = (tmpVal >> 13) & 0x7;
        hsb->r9300.icfi = (tmpVal >> 12) & 0x1;
        hsb->r9300.ivid = tmpVal & 0xFFF;
        reg_field_get(unit, LONGAN_HSB_DATA23r, LONGAN_OTAG_CONTENTf, &tmpVal, &value);
        hsb->r9300.opri = (tmpVal >> 13) & 0x7;
        hsb->r9300.ocfi = (tmpVal >> 12) & 0x1;
        hsb->r9300.ovid = tmpVal & 0xFFF;

        /* HSB_DATA24 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA24r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA24r, LONGAN_TYPELENf, &tmpVal, &value);
        hsb->r9300.typelen = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA24r, LONGAN_IP_LENGTHf, &tmpVal, &value);
        hsb->r9300.ip_length = tmpVal;

        /* HSB_DATA25/26/27 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA27r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA27r, LONGAN_DMAC_31_0f, &tmpVal, &value);
        osal_memcpy(&hsb->r9300.dmac[2], &tmpVal, 4);
        hsb->r9300.dmac[2] = (tmpVal >> 24)& 0xff;
        hsb->r9300.dmac[3] = (tmpVal >> 16)& 0xff;
        hsb->r9300.dmac[4] = (tmpVal >> 8)& 0xff;
        hsb->r9300.dmac[5] = (tmpVal)& 0xff;
        reg_read(unit, LONGAN_HSB_DATA26r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA26r, LONGAN_DMAC_47_32f, &tmpVal, &value);
        hsb->r9300.dmac[0] = (tmpVal>>8) & 0xff;
        hsb->r9300.dmac[1] = (tmpVal) & 0xff;
        reg_field_get(unit, LONGAN_HSB_DATA26r, LONGAN_SMAC_15_0f, &tmpVal, &value);
        hsb->r9300.smac[4] = (tmpVal>>8) & 0xff;
        hsb->r9300.smac[5] = (tmpVal) & 0xff;
        reg_read(unit, LONGAN_HSB_DATA25r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA25r, LONGAN_SMAC_47_16f, &tmpVal, &value);
        osal_memcpy(&hsb->r9300.smac[0], &tmpVal, 4);
        hsb->r9300.smac[0] = (tmpVal >> 24)& 0xff;
        hsb->r9300.smac[1] = (tmpVal >> 16)& 0xff;
        hsb->r9300.smac[2] = (tmpVal >> 8)& 0xff;
        hsb->r9300.smac[3] = (tmpVal)& 0xff;

       /* HSB_DATA28 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA28r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_RFC_1042f, &tmpVal, &value);
        hsb->r9300.rfc1042 = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_PPPOE_PKTf, &tmpVal, &value);
        hsb->r9300.pppoe_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_LLC_OTHERf, &tmpVal, &value);
        hsb->r9300.llc_other = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_OAM_PDUf, &tmpVal, &value);
        hsb->r9300.oam_pdu = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_ITAG_EXISTf, &tmpVal, &value);
        hsb->r9300.itag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_OTAG_EXISTf, &tmpVal, &value);
        hsb->r9300.otag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_CTAG_EXISTf, &tmpVal, &value);
        hsb->r9300.ctag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_ETAG_EXISTf, &tmpVal, &value);
        hsb->r9300.etag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_RTAG_EXISTf, &tmpVal, &value);
        hsb->r9300.rtag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_RTAG_TYPEf, &tmpVal, &value);
        hsb->r9300.rtag_type = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV4_PKTf, &tmpVal, &value);
        hsb->r9300.ipv4_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_PKTf, &tmpVal, &value);
        hsb->r9300.ipv6_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_EXT_LONGf, &tmpVal, &value);
        hsb->r9300.ipv6_ext_long = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV4_CHKSUM_OKf, &tmpVal, &value);
        hsb->r9300.ipv4_chksum_ok = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_AUTHf, &tmpVal, &value);
        hsb->r9300.ipv6_auth = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_DESTf, &tmpVal, &value);
        hsb->r9300.ipv6_dest = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_FRAGf, &tmpVal, &value);
        hsb->r9300.ipv6_frag = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_ROUTf, &tmpVal, &value);
        hsb->r9300.ipv6_rout = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV6_HOPf, &tmpVal, &value);
        hsb->r9300.ipv6_hop = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_TCP_SN_EQ_0f, &tmpVal, &value);
        hsb->r9300.tcp_sn_eq_0 = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_IPV4_HDRLENf, &tmpVal, &value);
        hsb->r9300.ipv4_hdrlen = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_ITPID_IDXf, &tmpVal, &value);
        hsb->r9300.itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA28r, LONGAN_OTPID_IDXf, &tmpVal, &value);
        hsb->r9300.otpid_idx = tmpVal;

        /* HSB_DATA29 */
        value = 0;
        reg_read(unit, LONGAN_HSB_DATA29r, &value);
        reg_field_get(unit, LONGAN_HSB_DATA29r, LONGAN_PKT_LENf, &tmpVal, &value);
        hsb->r9300.pkt_len = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA29r, LONGAN_L4_HDR_CHKf, &tmpVal, &value);
        hsb->r9300.l4_hdr_chk = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA29r, LONGAN_L2_ERR_PKTf, &tmpVal, &value);
        hsb->r9300.l2_err_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSB_DATA29r, LONGAN_SPAf, &tmpVal, &value);
        hsb->r9300.sphy = tmpVal;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        uint32 *val;

        val = osal_alloc(256*sizeof(uint32));
        if (NULL == val)
        {
            osal_printf("osal_alloc  failed!!\n");
            return RT_ERR_FAILED;
        }

        /* HSB_DATA0 */
        reg_read(unit, MANGO_HSB_DATAr, val);
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_ERR_PKTf, &tmpVal, val);
        hsb->r9310.OHSB_ERR_PKT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L2_CRC_ERRf, &tmpVal, val);
        hsb->r9310.OHSB_L2_CRC_ERR = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_PKT_LENf, &tmpVal, val);
        hsb->r9310.OHSB_PKT_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_PARSER_CANT_HANDLEf, &tmpVal, val);
        hsb->r9310.OHSB_PARSER_CANT_HANDLE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_MALFORMED_PKTf, &tmpVal, val);
        hsb->r9310.OHSB_MALFORMED_PKT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_SPPf, &tmpVal, val);
        hsb->r9310.OHSB_SPP = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_RTAG_IFf, &tmpVal, val);
        hsb->r9310.OHSB_RTAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_RTAG_239_208f, &tmpVal, val);
        hsb->r9310.OHSB_RTAG_239_208 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_RTAG_207_176f, &tmpVal, val);
        hsb->r9310.OHSB_RTAG_207_176 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_RTAG_175_144f, &tmpVal, val);
        hsb->r9310.OHSB_RTAG_175_144 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_RTAG_143_128f, &tmpVal, val);
        hsb->r9310.OHSB_RTAG_143_128 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_DMAC_47_32f, &tmpVal, val);
        hsb->r9310.OHSB_DMAC_47_32 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_DMAC_31_0f, &tmpVal, val);
        hsb->r9310.OHSB_DMAC_31_0 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_SMAC_47_16f, &tmpVal, val);
        hsb->r9310.OHSB_SMAC_47_16 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_SMAC_15_0f, &tmpVal, val);
        hsb->r9310.OHSB_SMAC_15_0 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_OTAGf, &tmpVal, val);
        hsb->r9310.OHSB_OTAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_ITAGf, &tmpVal, val);
        hsb->r9310.OHSB_ITAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_OTAG_IFf, &tmpVal, val);
        hsb->r9310.OHSB_OTAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_OTAG_TPID_IDXf, &tmpVal, val);
        hsb->r9310.OHSB_OTAG_TPID_IDX = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_ITAG_IFf, &tmpVal, val);
        hsb->r9310.OHSB_ITAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_ITAG_TPID_IDXf, &tmpVal, val);
        hsb->r9310.OHSB_ITAG_TPID_IDX = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_ETAG_IFf, &tmpVal, val);
        hsb->r9310.OHSB_ETAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_OMPLS_IFf, &tmpVal, val);
        hsb->r9310.OHSB_OMPLS_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IMPLS_IFf, &tmpVal, val);
        hsb->r9310.OHSB_IMPLS_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_OMPLS_HDRf, &tmpVal, val);
        hsb->r9310.OHSB_OMPLS_HDR = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IMPLS_HDRf, &tmpVal, val);
        hsb->r9310.OHSB_IMPLS_HDR = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_ETHTYPEf, &tmpVal, val);
        hsb->r9310.OHSB_ETHTYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_FRAME_TYPEf, &tmpVal, val);
        hsb->r9310.OHSB_FRAME_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_OAMPDUf, &tmpVal, val);
        hsb->r9310.OHSB_OAMPDU = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_PPPOE_IF_MPLS_TYPEf, &tmpVal, val);
        hsb->r9310.OHSB_PPPOE_IF_MPLS_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IPV4_TYPEf, &tmpVal, val);
        hsb->r9310.OHSB_IPV4_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IPV6_TYPEf, &tmpVal, val);
        hsb->r9310.OHSB_IPV6_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_VERf, &tmpVal, val);
        hsb->r9310.OHSB_IP_VER = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_HDR_LENf, &tmpVal, val);
        hsb->r9310.OHSB_IP_HDR_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_TOSf, &tmpVal, val);
        hsb->r9310.OHSB_IP_TOS = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_FLOW_LBLf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_FLOW_LBL = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_LENf, &tmpVal, val);
        hsb->r9310.OHSB_IP_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_FLAGf, &tmpVal, val);
        hsb->r9310.OHSB_IP_FLAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_OFFSETf, &tmpVal, val);
        hsb->r9310.OHSB_IP_OFFSET = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_TTLf, &tmpVal, val);
        hsb->r9310.OHSB_IP_TTL = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_PROTOf, &tmpVal, val);
        hsb->r9310.OHSB_IP_PROTO = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L4_OFFSETf, &tmpVal, val);
        hsb->r9310.OHSB_L4_OFFSET = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_SIP3_FIELD_SEL11_10f, &tmpVal, val);
        hsb->r9310.OHSB_IP_SIP3_FIELD_SEL11_10 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_SIP2_FIELD_SEL9_8f, &tmpVal, val);
        hsb->r9310.OHSB_IP_SIP2_FIELD_SEL9_8 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_SIP1_FIELD_SEL7_6f, &tmpVal, val);
        hsb->r9310.OHSB_IP_SIP1_FIELD_SEL7_6 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_SIPf, &tmpVal, val);
        hsb->r9310.OHSB_IP_SIP = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_DIP3f, &tmpVal, val);
        hsb->r9310.OHSB_IP_DIP3 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_DIP2f, &tmpVal, val);
        hsb->r9310.OHSB_IP_DIP2 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_DIP1f, &tmpVal, val);
        hsb->r9310.OHSB_IP_DIP1 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP_DIPf, &tmpVal, val);
        hsb->r9310.OHSB_IP_DIP = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_HOPBYHOP_ERRf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_HOPBYHOP_ERR = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_HOPBYHOP_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_HOPBYHOP_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_ROUTE_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_ROUTE_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_FRAGMENT_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_FRAGMENT_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_ESP_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_ESP_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_AUTH_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_AUTH_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_DESTINATION_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_DESTINATION_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_MOBILITY_EXISTf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_MOBILITY_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_REPEATf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_REPEAT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_IP6_EH_TOTAL_LENf, &tmpVal, val);
        hsb->r9310.OHSB_IP6_EH_TOTAL_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L4_HDR_RDYf, &tmpVal, val);
        hsb->r9310.OHSB_L4_HDR_RDY = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L4_SPORTf, &tmpVal, val);
        hsb->r9310.OHSB_L4_SPORT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L4_DPORTf, &tmpVal, val);
        hsb->r9310.OHSB_L4_DPORT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L4_OPCODEf, &tmpVal, val);
        hsb->r9310.OHSB_L4_OPCODE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_L4_DATAf, &tmpVal, val);
        hsb->r9310.OHSB_L4_DATA = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_PREf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_PRE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_HLENf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_HLEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_RIDf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_RID = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_WBIDf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_WBID = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_FLAGf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_FLAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_RMAC_LENf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_RMAC_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_HDR_RDYf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_HDR_RDY = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_BSSID_VALIDf, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_BSSID_VALID = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_BSSID_47_16f, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_BSSID_47_16 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_OHSB_CAPWAP_BSSID_15_0f, &tmpVal, val);
        hsb->r9310.OHSB_CAPWAP_BSSID_15_0 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L3_OFFSETf, &tmpVal, val);
        hsb->r9310.IHSB_L3_OFFSET = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L4_OFFSETf, &tmpVal, val);
        hsb->r9310.IHSB_L4_OFFSET = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_PARSER_CANT_HANDLEf, &tmpVal, val);
        hsb->r9310.IHSB_PARSER_CANT_HANDLE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_MALFORMED_PKTf, &tmpVal, val);
        hsb->r9310.IHSB_MALFORMED_PKT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L2_RDYf, &tmpVal, val);
        hsb->r9310.IHSB_L2_RDY = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_FRAME_CTRLf, &tmpVal, val);
        hsb->r9310.IHSB_FRAME_CTRL = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_QOS_CTRLf, &tmpVal, val);
        hsb->r9310.IHSB_QOS_CTRL = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_DMAC_47_16f, &tmpVal, val);
        hsb->r9310.IHSB_DMAC_47_16 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_DMAC_15_0f, &tmpVal, val);
        hsb->r9310.IHSB_DMAC_15_0 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_SMAC_47_32f, &tmpVal, val);
        hsb->r9310.IHSB_SMAC_47_32 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_SMAC_31_0f, &tmpVal, val);
        hsb->r9310.IHSB_SMAC_31_0 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_OTAGf, &tmpVal, val);
        hsb->r9310.IHSB_OTAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_ITAGf, &tmpVal, val);
        hsb->r9310.IHSB_ITAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_ETHTYPEf, &tmpVal, val);
        hsb->r9310.IHSB_ETHTYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_OTAG_IFf, &tmpVal, val);
        hsb->r9310.IHSB_OTAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_OTAG_TPID_IDXf, &tmpVal, val);
        hsb->r9310.IHSB_OTAG_TPID_IDX = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_ITAG_IFf, &tmpVal, val);
        hsb->r9310.IHSB_ITAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_ITAG_TPID_IDXf, &tmpVal, val);
        hsb->r9310.IHSB_ITAG_TPID_IDX = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_FRAME_TYPEf, &tmpVal, val);
        hsb->r9310.IHSB_FRAME_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IPV4_TYPEf, &tmpVal, val);
        hsb->r9310.IHSB_IPV4_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IPV6_TYPEf, &tmpVal, val);
        hsb->r9310.IHSB_IPV6_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_VERf, &tmpVal, val);
        hsb->r9310.IHSB_IP_VER = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_HDR_LENf, &tmpVal, val);
        hsb->r9310.IHSB_IP_HDR_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_TOSf, &tmpVal, val);
        hsb->r9310.IHSB_IP_TOS = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_FLOW_LBLf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_FLOW_LBL = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_LENf, &tmpVal, val);
        hsb->r9310.IHSB_IP_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_FLAGf, &tmpVal, val);
        hsb->r9310.IHSB_IP_FLAG = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_OFFSETf, &tmpVal, val);
        hsb->r9310.IHSB_IP_OFFSET = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_TTLf, &tmpVal, val);
        hsb->r9310.IHSB_IP_TTL = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_PROTOf, &tmpVal, val);
        hsb->r9310.IHSB_IP_PROTO = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_SIP3_FIELD_SEL5_4f, &tmpVal, val);
        hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_SIP2_FIELD_SEL3_2f, &tmpVal, val);
        hsb->r9310.IHSB_IP_SIP2_FIELD_SEL3_2 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_SIP1_FIELD_SEL1_0f, &tmpVal, val);
        hsb->r9310.IHSB_IP_SIP1_FIELD_SEL1_0 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_SIPf, &tmpVal, val);
        hsb->r9310.IHSB_IP_SIP = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_DIP3f, &tmpVal, val);
        hsb->r9310.IHSB_IP_DIP3 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_DIP2f, &tmpVal, val);
        hsb->r9310.IHSB_IP_DIP2 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_DIP1f, &tmpVal, val);
        hsb->r9310.IHSB_IP_DIP1 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP_DIPf, &tmpVal, val);
        hsb->r9310.IHSB_IP_DIP = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_HOPBYHOP_ERRf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_HOPBYHOP_ERR = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_HOPBYHOP_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_HOPBYHOP_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_ROUTE_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_ROUTE_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_FRAGMENT_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_FRAGMENT_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_ESP_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_ESP_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_AUTH_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_AUTH_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_DESTINATION_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_DESTINATION_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_MOBILITY_EXISTf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_MOBILITY_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_REPEATf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_REPEAT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L4_HDR_RDYf, &tmpVal, val);
        hsb->r9310.IHSB_L4_HDR_RDY = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_IP6_EH_TOTAL_LENf, &tmpVal, val);
        hsb->r9310.IHSB_IP6_EH_TOTAL_LEN = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L4_SPORTf, &tmpVal, val);
        hsb->r9310.IHSB_L4_SPORT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L4_DPORTf, &tmpVal, val);
        hsb->r9310.IHSB_L4_DPORT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L4_OPCODEf, &tmpVal, val);
        hsb->r9310.IHSB_L4_OPCODE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_L4_DATAf, &tmpVal, val);
        hsb->r9310.IHSB_L4_DATA = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_FIELD_SEL12f, &tmpVal, val);
        hsb->r9310.HSB_FIELD_SEL12 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_FIELD_SEL13f, &tmpVal, val);
        hsb->r9310.HSB_FIELD_SEL13 = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_IHSB_FIELD_VALIDf, &tmpVal, val);
        hsb->r9310.HSB_FIELD_VALID = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PKT_TYPEf, &tmpVal, val);
        hsb->r9310.HSB_PKT_TYPE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_EST_INVALIDf, &tmpVal, val);
        hsb->r9310.HSB_EST_INVALID = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ETAG_IFf, &tmpVal, val);
        hsb->r9310.HSB_PE_ETAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ETAG_PRIf, &tmpVal, val);
        hsb->r9310.HSB_PE_ETAG_PRI = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ETAG_DEIf, &tmpVal, val);
        hsb->r9310.HSB_PE_ETAG_DEI = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_IGR_ECID_BASEf, &tmpVal, val);
        hsb->r9310.HSB_PE_IGR_ECID_BASE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ETAG_RSVDf, &tmpVal, val);
        hsb->r9310.HSB_PE_ETAG_RSVD = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ECID_GRPf, &tmpVal, val);
        hsb->r9310.HSB_PE_ECID_GRP = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ECID_BASEf, &tmpVal, val);
        hsb->r9310.HSB_PE_ECID_BASE = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_IGR_ECID_EXTf, &tmpVal, val);
        hsb->r9310.HSB_PE_IGR_ECID_EXT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PE_ECID_EXTf, &tmpVal, val);
        hsb->r9310.HSB_PE_ECID_EXT = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_BEGIN_DESCf, &tmpVal, val);
        hsb->r9310.HSB_BEGIN_DESC = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_END_DESCf, &tmpVal, val);
        hsb->r9310.HSB_END_DESC = tmpVal;
        reg_field_get(unit, MANGO_HSB_DATAr, MANGO_HSB_PAGEf, &tmpVal, val);
        hsb->r9310.HSB_PAGE = tmpVal;

        osal_free(val);
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

void _dumpHsbPayloadInfo(uint32 unit, hsb_param_t * hsb)
{
}

/* Function Name:
 *      hal_dumpHsb
 * Description:
 *      Dump hsb paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsb(uint32 unit)
{
#if defined(CONFIG_SDK_RTL8380)
    uint32  index;
#endif
    hsb_param_t  *hsb;
    hal_control_t *pInfo;

    hsb = osal_alloc(sizeof(hsb_param_t));
    if (NULL == hsb)
    {
        osal_printf("osal_alloc hsb_param_t failed!!\n");
        return RT_ERR_FAILED;
    }


    if(RT_ERR_OK != _rtk_getHsb(unit, hsb))
    {
        osal_free(hsb);
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        osal_free(hsb);
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        osal_printf("\n===========================HSB===============================\n");

        osal_printf("page_cnt : 0x%x\t\t", hsb->r8390.page_cnt);
        osal_printf("endsc : 0x%x\t\t", hsb->r8390.endsc);
        osal_printf("bgsc : 0x%x\n", hsb->r8390.bgsc);

        osal_printf("field_seltor11 : 0x%x\t", hsb->r8390.field_seltor11);
        osal_printf("field_seltor10 : 0x%x\t", hsb->r8390.field_seltor10);
        osal_printf("field_seltor9 : 0x%x\n", hsb->r8390.field_seltor9);
        osal_printf("field_seltor8 : 0x%x\t", hsb->r8390.field_seltor8);
        osal_printf("field_seltor7 : 0x%x\t", hsb->r8390.field_seltor7);
        osal_printf("field_seltor6 : 0x%x\n", hsb->r8390.field_seltor6);
        osal_printf("field_seltor5 : 0x%x\t", hsb->r8390.field_seltor5);
        osal_printf("field_seltor4 : 0x%x\t", hsb->r8390.field_seltor4);
        osal_printf("field_seltor3 : 0x%x\n", hsb->r8390.field_seltor3);
        osal_printf("field_seltor2 : 0x%x\t", hsb->r8390.field_seltor2);
        osal_printf("field_seltor1 : 0x%x\t", hsb->r8390.field_seltor1);
        osal_printf("field_seltor0 : 0x%x\n", hsb->r8390.field_seltor0);
        osal_printf("field_sel_vld : 0x%x\t", hsb->r8390.field_sel_vld);
        osal_printf("impls_exp : 0x%x\t\t", hsb->r8390.impls_exp);
        osal_printf("impls_label : 0x%x\n", hsb->r8390.impls_label);

        osal_printf("ompls_exp : 0x%x\t\t", hsb->r8390.ompls_exp);
        osal_printf("ompls_label : 0x%x\t", hsb->r8390.ompls_label);
        osal_printf("arpopcode : 0x%x\n", hsb->r8390.arpopcode);

        osal_printf("target_mac : %02x:%02x:%02x:%02x:%02x:%02x\n", hsb->r8390.target_mac[0], hsb->r8390.target_mac[1], hsb->r8390.target_mac[2], hsb->r8390.target_mac[3], hsb->r8390.target_mac[4], hsb->r8390.target_mac[5]);
        osal_printf("sender_mac : %02x:%02x:%02x:%02x:%02x:%02x\n", hsb->r8390.sender_mac[0], hsb->r8390.sender_mac[1], hsb->r8390.sender_mac[2], hsb->r8390.sender_mac[3], hsb->r8390.sender_mac[4], hsb->r8390.sender_mac[5]);
        osal_printf("dport : 0x%x\t\t", hsb->r8390.dport);
        osal_printf("sport : 0x%x\t\t", hsb->r8390.sport);
        osal_printf("tcp_flag : 0x%x\n", hsb->r8390.tcp_flag);

        osal_printf("ipver : 0x%x\t\t", hsb->r8390.ipver);
        osal_printf("ipttl : 0x%x\t\t", hsb->r8390.ipttl);
        osal_printf("ip_tos : 0x%x\n", hsb->r8390.ip_tos);

        osal_printf("ip_protocol : 0x%x\t", hsb->r8390.ip_protocol);
        osal_printf("ip_flag : 0x%x\t\t", hsb->r8390.ip_flag);
        osal_printf("ip_offset : 0x%x\n", hsb->r8390.ip_offset);

        osal_printf("dip : 0x%x\t\t", hsb->r8390.dip);
        osal_printf("sip : 0x%x\t\t", hsb->r8390.sip);
        osal_printf("otag : 0x%x\n", hsb->r8390.otag);

        osal_printf("itag : 0x%x\t\t", hsb->r8390.itag);
        osal_printf("otpid_idx : 0x%x\t\t", hsb->r8390.otpid_idx);
        osal_printf("itpid_idx : 0x%x\n", hsb->r8390.itpid_idx);

        osal_printf("smac : %02x:%02x:%02x:%02x:%02x:%02x\n", hsb->r8390.smac[0], hsb->r8390.smac[1], hsb->r8390.smac[2], hsb->r8390.smac[3], hsb->r8390.smac[4], hsb->r8390.smac[5]);
        osal_printf("dmac : %02x:%02x:%02x:%02x:%02x:%02x\n", hsb->r8390.dmac[0], hsb->r8390.dmac[1], hsb->r8390.dmac[2], hsb->r8390.dmac[3], hsb->r8390.dmac[4], hsb->r8390.dmac[5]);
        osal_printf("etp : 0x%x\t\t", hsb->r8390.etp);
        osal_printf("iplen : 0x%x\t\t", hsb->r8390.iplen);
        osal_printf("ipv4hdlen : 0x%x\n", hsb->r8390.ipv4hdlen);

        osal_printf("tcpseqzero : 0x%x\t", hsb->r8390.tcpseqzero);
        osal_printf("ipv6hop : 0x%x\t\t", hsb->r8390.ipv6hop);
        osal_printf("ipv6rout : 0x%x\n", hsb->r8390.ipv6rout);

        osal_printf("ipv6frag : 0x%x\t\t", hsb->r8390.ipv6frag);
        osal_printf("ipv6dest : 0x%x\t\t", hsb->r8390.ipv6dest);
        osal_printf("ipv6auth : 0x%x\n", hsb->r8390.ipv6auth);

        osal_printf("ipcsok : 0x%x\t\t", hsb->r8390.ipcsok);
        osal_printf("ipv6ext_long : 0x%x\t", hsb->r8390.ipv6ext_long);
        osal_printf("ipv6 : 0x%x\n", hsb->r8390.ipv6);

        osal_printf("ipv4 : 0x%x\t\t", hsb->r8390.ipv4);
        osal_printf("ompls_if : 0x%x\t\t", hsb->r8390.ompls_if);
        osal_printf("impls_if : 0x%x\n", hsb->r8390.impls_if);

        osal_printf("etag_if : 0x%x\t\t", hsb->r8390.etag_if);
        osal_printf("cputag_if : 0x%x\t\t", hsb->r8390.cputag_if);
        osal_printf("otag_if : 0x%x\n", hsb->r8390.otag_if);

        osal_printf("itag_if : 0x%x\t\t", hsb->r8390.itag_if);
        osal_printf("oampdu : 0x%x\t\t", hsb->r8390.oampdu);
        osal_printf("llc_other : 0x%x\n", hsb->r8390.llc_other);

        osal_printf("pppoe_if : 0x%x\t\t", hsb->r8390.pppoe_if);
        osal_printf("rfc1042 : 0x%x\t\t", hsb->r8390.rfc1042);
        osal_printf("spa : 0x%x\n", hsb->r8390.spa);

        osal_printf("errpkt : 0x%x\t\t", hsb->r8390.errpkt);
        osal_printf("l4hdchk : 0x%x\t\t", hsb->r8390.l4hdchk);
        osal_printf("pktlen : 0x%x\n", hsb->r8390.pktlen);
        osal_printf("=============================================================\n");
    }
    else
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))

    {
        osal_printf("\n=======================================   HSB   =======================================\n");

        /*CPU TAG*/
        osal_printf("--------------------------------------cpu tag-----------------------------------------\n");
        osal_printf("%-14s = 0x%-6x\t", "ctag_exist", hsb->r8380.ctag_exist);
        osal_printf("%-14s = 0x%-6x\t", "bp_fltr_1", hsb->r8380.ctag_bp_fltr_1);
        osal_printf("%-14s = 0x%-6x\t\n", "bp_fltr_2", hsb->r8380.ctag_bp_fltr_2);

        osal_printf("%-14s = 0x%-6x\t", "as_tagsts", hsb->r8380.ctag_as_tagsts);
        osal_printf("%-14s = 0x%-6x\t", "acl_act", hsb->r8380.ctag_acl_act);
        osal_printf("%-14s = 0x%-6x\t\n", "rvid_sel", hsb->r8380.ctag_rvid_sel);

        osal_printf("%-14s = 0x%-6x\t", "l2learning", hsb->r8380.ctag_l2learning);
        osal_printf("%-14s = 0x%-6x\t", "as_pri", hsb->r8380.ctag_as_pri);
        osal_printf("%-14s = 0x%-6x\t\n", "tag_pri", hsb->r8380.ctag_pri);

        osal_printf("%-14s = 0x%-6x\t", "as_dpm", hsb->r8380.ctag_as_dpm);
        osal_printf("%-14s = 0x%-6x\t", "physical_dpm", hsb->r8380.ctag_physical_dpm);
        osal_printf("%-14s = 0x%-6x\t\n", "dpm", hsb->r8380.ctag_dpm);
        /*tag status*/
        osal_printf("-----------------------------------otag/itag/extra tag-----------------------------------\n");
        osal_printf("%-14s = 0x%-6x\t", "rtag_exist", hsb->r8380.rtag_exist);
        osal_printf("%-14s = 0x%-6x\t", "otag_exist", hsb->r8380.otag_exist);
        osal_printf("%-14s = 0x%-6x\t\n", "otag_content", hsb->r8380.otag_content);

        osal_printf("%-14s = 0x%-6x\t", "otag_index", hsb->r8380.otag_index);
        osal_printf("%-14s = 0x%-6x\t", "itag_exist", hsb->r8380.itag_exist);
        osal_printf("%-14s = 0x%-6x\t\n", "itag_content", hsb->r8380.itag_content);

        osal_printf("%-14s = 0x%-6x\t", "itag_index", hsb->r8380.itag_index);
        osal_printf("%-14s = 0x%-6x\t\n", "etag_exist", hsb->r8380.etag_exist);
        osal_printf("----------------------------------------pkt type----------------------------------------\n");
        osal_printf("%-14s = 0x%-6x\t\n", "pppoe_pkt", hsb->r8380.pppoe_pkt);

        osal_printf("%-14s = 0x%-6x\t", "rtkpp", hsb->r8380.rtkpp);
        osal_printf("%-14s = 0x%-6x\t", "frame_type", hsb->r8380.frame_type);
        osal_printf("%-14s = 0x%-6x\t\n", "typelen", hsb->r8380.typelen);

        osal_printf("%-14s = 0x%-6x\t", "ipv4_pkt", hsb->r8380.ipv4_pkt);
        osal_printf("%-14s = 0x%-6x\t", "ipv6_pkt", hsb->r8380.ipv6_pkt);
        osal_printf("%-14s = 0x%-6x\t\n", "arp_pkt", hsb->r8380.arp_pkt);

        osal_printf("%-14s = 0x%-6x\t", "icmp_pkt", hsb->r8380.icmp_pkt);
        osal_printf("%-14s = 0x%-6x\t", "igmp_pkt", hsb->r8380.igmp_pkt);
        osal_printf("%-14s = 0x%-6x\t\n", "udp_pkt", hsb->r8380.udp_pkt);

        osal_printf("%-14s = 0x%-6x\t", "tcp_pkt", hsb->r8380.tcp_pkt);
        osal_printf("%-14s = 0x%-6x\t", "ip_mf", hsb->r8380.ip_mf);
        osal_printf("%-14s = 0x%-6x\t\n", "ip_dstype", hsb->r8380.ip_dstype);

        osal_printf("%-14s = 0x%-6x\t", "ip_protocol", hsb->r8380.ip_protocol);
        osal_printf("%-14s = 0x%-6x\t", "ip_offset", hsb->r8380.ip_offset);
        osal_printf("%-14s = 0x%-6x\t\n", "ip_length", hsb->r8380.ip_length);

        osal_printf("%-14s = 0x%-6x\t", "ip_ttl", hsb->r8380.ip_ttl);
        osal_printf("%-14s = 0x%-6x\t", "ipv4_hdrlen", hsb->r8380.ipv4_hdrlen);
        osal_printf("%-14s = 0x%-6x\t\n", "ipv4_df", hsb->r8380.ipv4_df);

        osal_printf("%-14s = 0x%-6x\t", "exten_hdrlen", hsb->r8380.ipv6_extension_hdrlen);
        osal_printf("%-14s = 0x%-6x\t", "tcp_hdrlen", hsb->r8380.tcp_hdrlen);
        osal_printf("%-14s = 0x%-6x\t\n", "tcp_flag", hsb->r8380.tcp_flag);

        osal_printf("%-14s = 0x%-6x\t", "tcp_sn_eq_0", hsb->r8380.tcp_sn_eq_0);
        osal_printf("%-14s = 0x%-6x\t", "l4_content", hsb->r8380.l4_content);
        osal_printf("%-14s = 0x%-6x\t\n", "flow_label", hsb->r8380.ipv6_flow_label);

        osal_printf("%-14s = 0x%-6x\t", "mob_ext_hdr", hsb->r8380.ipv6_mob_ext_hdr);
        osal_printf("%-14s = 0x%-6x\t", "hbh_ext_err", hsb->r8380.ipv6_hbh_ext_hdr_err);
        osal_printf("%-14s = 0x%-6x\t\n\n", "unknown_hdr", hsb->r8380.ipv6_unknown_hdr);

        /*MAC Address*/
        osal_printf("%-14s = 0x", "DMAC:");
        for(index=0; index<6; index++)
            osal_printf("%02x ", hsb->r8380.dmac[index]);
        osal_printf("\n");
        osal_printf("%-14s = 0x", "SMAC:");
        for(index=0; index<6; index++)
            osal_printf("%02x ", hsb->r8380.smac[index]);
        osal_printf("\n\n");

        /*dip address*/
        osal_printf("%-14s = 0x", "dip");
        for(index=0; index<16; index++)
            osal_printf("%02x ", hsb->r8380.dip[index]);
        osal_printf("\n");
        /*sip address*/
        osal_printf("%-14s = 0x", "sip");
        for(index=0; index<16; index++)
            osal_printf("%02x ", hsb->r8380.sip[index]);
        osal_printf("\n");

        //osal_printf("\n");
        osal_printf("--------------------------------------field selectors-----------------------------------\n");
        osal_printf("%-14s = 0x%-6x\t", "fs3_valid", hsb->r8380.fs3_valid);
        osal_printf("%-14s = 0x%-6x\t", "fs2_valid", hsb->r8380.fs2_valid);
        osal_printf("%-14s = 0x%-6x\t\n", "fs1_valid", hsb->r8380.fs1_valid);

        osal_printf("%-14s = 0x%-6x\t", "fs0_valid", hsb->r8380.fs0_valid);
        osal_printf("%-14s = 0x%-6x\t", "fs3_data", hsb->r8380.fs3_data);
        osal_printf("%-14s = 0x%-6x\t\n", "fs2_data", hsb->r8380.fs2_data);

        osal_printf("%-14s = 0x%-6x\t", "fs1_data", hsb->r8380.fs1_data);
        osal_printf("%-14s = 0x%-6x\t\n", "fs0_data", hsb->r8380.fs0_data);
        osal_printf("-------------------------------------------misc-----------------------------------------\n");
        osal_printf("%-14s = 0x%-6x\t", "sphy", hsb->r8380.sphy);
        osal_printf("%-14s = 0x%-6x\t", "pktlen", hsb->r8380.pktlen);
        osal_printf("%-14s = 0x%-6x\t\n", "crceq", hsb->r8380.crceq);
        osal_printf("%-14s = 0x%-6x\t", "rxdrop", hsb->r8380.rxdrop);
        osal_printf("%-14s = 0x%-6x\t", "oampdu", hsb->r8380.oampdu);
        osal_printf("%-14s = 0x%-6x\t", "rx_rm_rtag", hsb->r8380.rx_rm_rtag);
        osal_printf("\n====================================================================================\n");
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        osal_printf("\n=======================================   HSB   =======================================\n");

        /*Stacking Header*/
        osal_printf("--------------------------------------Stacking header Valid = %d-----------------------------------------\n", hsb->r9300.sthdr_vld );
        osal_printf("********************Common header********************\n");
        osal_printf("%-16s = 0x%-6x\t", "ver", hsb->r9300.sthdr_comm_ver);
        osal_printf("%-16s = 0x%-6x\t", "info_hdr_type", hsb->r9300.sthdr_comm_info_hdr_type);
        osal_printf("%-16s = 0x%-6x\t\n", "sp_info", hsb->r9300.sthdr_comm_sp_info);

        osal_printf("%-16s = 0x%-6x\t", "sp_filter", hsb->r9300.sthdr_comm_sp_filter);
        osal_printf("%-16s = 0x%-6x\t", "dp_fmt", hsb->r9300.sthdr_comm_dp_fmt);
        osal_printf("%-16s = 0x%-6x\t\n", "dp_info", hsb->r9300.sthdr_comm_dp_info);

        osal_printf("%-16s = 0x%-6x\t", "fwd_type", hsb->r9300.sthdr_comm_fwd_type);
        osal_printf("%-16s = 0x%-6x\t", "sa_lrn", hsb->r9300.sthdr_comm_sa_lrn);
        osal_printf("%-16s = 0x%-6x\t\n", "sthdr_comm_vlan_tag", hsb->r9300.sthdr_comm_vlan_tag);

        osal_printf("********************Info header********************\n");
        osal_printf("%-16s = 0x%-6x\t", "cpu_tag_if", hsb->r9300.sthdr_info_cpu_tag_if);
        osal_printf("%-16s = 0x%-6x\t", "int_pri", hsb->r9300.sthdr_info_int_pri);
        osal_printf("%-16s = 0x%-6x\t\n", "trk_hash", hsb->r9300.sthdr_info_trk_hash);

        osal_printf("%-16s = 0x%-6x\t", "class", hsb->r9300.sthdr_info_class);
        osal_printf("%-16s = 0x%-6x\t", "da_hit", hsb->r9300.sthdr_info_da_hit);
        osal_printf("%-16s = 0x%-6x\t\n", "vb_iso_mbr", hsb->r9300.sthdr_info_vb_iso_mbr);

        osal_printf("%-16s = 0x%-6x\t", "int_otag_hit", hsb->r9300.sthdr_info_int_otag_hit);
        osal_printf("%-16s = 0x%-6x\t", "int_otag_if", hsb->r9300.sthdr_info_int_otag_if);
        osal_printf("%-16s = 0x%-6x\t\n", "int_otag_tpid_hit", hsb->r9300.sthdr_info_int_otag_tpid_hit);

        osal_printf("%-16s = 0x%-6x\t", "int_otag_tpid_idx", hsb->r9300.sthdr_info_int_otag_tpid_idx);
        osal_printf("%-16s = 0x%-6x\t", "int_otag_pri_hit", hsb->r9300.sthdr_info_int_otag_pri_hit);
        osal_printf("%-16s = 0x%-6x\t\n", "int_otag_pri", hsb->r9300.sthdr_info_int_otag_pri);

        osal_printf("%-16s = 0x%-6x\t", "int_ocfi", hsb->r9300.sthdr_info_int_ocfi);
        osal_printf("%-16s = 0x%-6x\t", "int_ovid", hsb->r9300.sthdr_info_int_ovid);
        osal_printf("%-16s = 0x%-6x\t\n", "int_itag_hit", hsb->r9300.sthdr_info_int_itag_hit);

        osal_printf("%-16s = 0x%-6x\t", "int_itag_if", hsb->r9300.sthdr_info_int_itag_if);
        osal_printf("%-16s = 0x%-6x\t", "int_itag_tpid_hit", hsb->r9300.sthdr_info_int_itag_tpid_hit);
        osal_printf("%-16s = 0x%-6x\t\n", "int_itag_tpid_idx", hsb->r9300.sthdr_info_int_itag_tpid_idx);

        osal_printf("%-16s = 0x%-6x\t", "int_itag_pri_hit", hsb->r9300.sthdr_info_int_itag_pri_hit);
        osal_printf("%-16s = 0x%-6x\t", "int_itag_pri", hsb->r9300.sthdr_info_int_itag_pri);
        osal_printf("%-16s = 0x%-6x\t\n", "int_icfi", hsb->r9300.sthdr_info_int_icfi);

        osal_printf("%-16s = 0x%-6x\t", "int_ivid", hsb->r9300.sthdr_info_int_ivid);
        osal_printf("%-16s = 0x%-6x\t", "fwd_base", hsb->r9300.sthdr_info_fwd_base);
        osal_printf("%-16s = 0x%-6x\t\n", "dp", hsb->r9300.sthdr_info_dp);

        osal_printf("%-16s = 0x%-6x\t", "int_dscp_hit", hsb->r9300.sthdr_info_int_dscp_hit);
        osal_printf("%-16s = 0x%-6x\t", "mir_id", hsb->r9300.sthdr_info_mir_id);
        osal_printf("%-16s = 0x%-6x\t\n", "rout_tunnel_if", hsb->r9300.sthdr_info_rout_tunnel_if);

        osal_printf("%-16s = 0x%-6x\t", "igr_l3_intf_type", hsb->r9300.sthdr_info_igr_l3_intf_type);
        osal_printf("%-16s = 0x%-6x\t\n", "igr_l3_intf_id", hsb->r9300.sthdr_info_igr_l3_intf_id);

        osal_printf("********************Vlan header Valid= %d********************\n", hsb->r9300.sthdr_comm_vlan_tag);
        osal_printf("%-16s = 0x%-6x\t", "ori_opri", hsb->r9300.sthdr_vlan_ori_opri);
        osal_printf("%-16s = 0x%-6x\t\n", "ori_ocfi", hsb->r9300.sthdr_vlan_ori_ocfi);

        osal_printf("%-16s = 0x%-6x\t", "ori_ipri", hsb->r9300.sthdr_vlan_ori_ipri);
        osal_printf("%-16s = 0x%-6x\t\n", "ori_icfi", hsb->r9300.sthdr_vlan_ori_icfi);

        osal_printf("********************Stack Tx CTag Valid= %d********************\n", (hsb->r9300.sthdr_info_cpu_tag_if==2 ? 1:0));
        osal_printf("%-16s = 0x%-6x\t", "fwd_type", hsb->r9300.sthdr_txCtag_fwd_type);
        osal_printf("%-16s = 0x%-6x\t", "acl_act", hsb->r9300.sthdr_txCtag_acl_act);
        osal_printf("%-16s = 0x%-6x\t\n", "cngst_drop", hsb->r9300.sthdr_txCtag_cngst_drop);

        osal_printf("%-16s = 0x%-6x\t", "dm_pkt", hsb->r9300.sthdr_txCtag_dm_pkt);
        osal_printf("%-16s = 0x%-6x\t", "dg_pkt", hsb->r9300.sthdr_txCtag_dg_pkt);
        osal_printf("%-16s = 0x%-6x\t\n", "bp_filter", hsb->r9300.sthdr_txCtag_bp_filter);

        osal_printf("%-16s = 0x%-6x\t", "bp_stp", hsb->r9300.sthdr_txCtag_bp_stp);
        osal_printf("%-16s = 0x%-6x\t", "bp_vlan_egr", hsb->r9300.sthdr_txCtag_bp_vlan_egr);
        osal_printf("%-16s = 0x%-6x\t\n", "ale_as_tagsts", hsb->r9300.sthdr_txCtag_ale_as_tagsts);

        osal_printf("%-16s = 0x%-6x\t", "l3_act", hsb->r9300.sthdr_txCtag_l3_act);
        osal_printf("%-16s = 0x%-6x\t", "as_qid", hsb->r9300.sthdr_txCtag_as_qid);
        osal_printf("%-16s = 0x%-6x\t\n", "qid", hsb->r9300.sthdr_txCtag_qid);

        osal_printf("%-16s = 0x%-6x\t", "src_filter_en", hsb->r9300.sthdr_txCtag_src_filter_en);
        osal_printf("%-16s = 0x%-6x\t", "sp_is_trk", hsb->r9300.sthdr_txCtag_sp_is_trk);
        osal_printf("%-16s = 0x%-6x\t\n", "spn", hsb->r9300.sthdr_txCtag_spn);

        osal_printf("%-16s = 0x%-6x\t", "sw_unit", hsb->r9300.sthdr_txCtag_sw_unit);
        osal_printf("%-16s = 0x%-6x\t", "dpm[1]", hsb->r9300.sthdr_txCtag_dpm.bits[1]);
        osal_printf("%-16s = 0x%-6x\t\n", "dpm[0]", hsb->r9300.sthdr_txCtag_dpm.bits[0]);

        osal_printf("********************Stack Rx CTag Valid= %d********************\n", (hsb->r9300.sthdr_info_cpu_tag_if==1 ? 1:0));
        osal_printf("%-16s = 0x%-6x\t", "of_lu_mis_tbl_id", hsb->r9300.sthdr_rxCtag_of_lu_mis_tbl_id);
        osal_printf("%-16s = 0x%-6x\t", "acl_of_hit", hsb->r9300.sthdr_rxCtag_acl_of_hit);
        osal_printf("%-16s = 0x%-6x\t\n", "of_tbl_id", hsb->r9300.sthdr_rxCtag_of_tbl_id);

        osal_printf("%-16s = 0x%-6x\t", "l2_err_pkt", hsb->r9300.sthdr_rxCtag_l2_err_pkt);
        osal_printf("%-16s = 0x%-6x\t", "l3_err_pkt", hsb->r9300.sthdr_rxCtag_l3_err_pkt);
        osal_printf("%-16s = 0x%-6x\t\n", "atk_type", hsb->r9300.sthdr_rxCtag_atk_type);

        osal_printf("%-16s = 0x%-6x\t", "qid", hsb->r9300.sthdr_rxCtag_qid);
        osal_printf("%-16s = 0x%-6x\t", "sthdr_rxCtag_idx", hsb->r9300.sthdr_rxCtag_idx);
        osal_printf("%-16s = 0x%-6x\t\n", "sflow", hsb->r9300.sthdr_rxCtag_sflow);

        osal_printf("%-16s = 0x%-6x\t", "tt_hit", hsb->r9300.sthdr_rxCtag_tt_hit);
        osal_printf("%-16s = 0x%-6x\t", "sthdr_rxCtag_tt_idx", hsb->r9300.sthdr_rxCtag_tt_idx);
        osal_printf("%-16s = 0x%-6x\t\n", "mac_cst", hsb->r9300.sthdr_rxCtag_mac_cst);

        osal_printf("%-16s = 0x%-6x\t", "dm_rxidx_of_lb_times", hsb->r9300.sthdr_rxCtag_dm_rxidx_of_lb_times);
        osal_printf("%-16s = 0x%-6x\t", "new_sa", hsb->r9300.sthdr_rxCtag_new_sa);
        osal_printf("%-16s = 0x%-6x\t\n", "pmv_forbid", hsb->r9300.sthdr_rxCtag_pmv_forbid);

        osal_printf("%-16s = 0x%-6x\t", "l2_sttc_pmv", hsb->r9300.sthdr_rxCtag_l2_sttc_pmv);
        osal_printf("%-16s = 0x%-6x\t", "l2_dyn_pmv", hsb->r9300.sthdr_rxCtag_l2_dyn_pmv);
        osal_printf("%-16s = 0x%-6x\t\n", "port_data_is_trk", hsb->r9300.sthdr_rxCtag_port_data_is_trk);

        osal_printf("%-16s = 0x%-6x\t", "port_data", hsb->r9300.sthdr_rxCtag_port_data);
        osal_printf("%-16s = 0x%-6x\t", "hash_full", hsb->r9300.sthdr_rxCtag_hash_full);
        osal_printf("%-16s = 0x%-6x\t\n", "invalid_sa", hsb->r9300.sthdr_rxCtag_invalid_sa);
        osal_printf("%-16s = 0x%-6x\t\n", "trap rsn", hsb->r9300.sthdr_rxCtag_rsn);

        osal_printf("*************************ACL************************************\n");
        osal_printf("%-16s = 0x%-6x\t", "fwd cpu pkt hit", hsb->r9300.sthdr_acl_fwd_cpu_pkt_hit);
        osal_printf("%-16s = 0x%-6x\t\n", "fwd cpu pkt fmt", hsb->r9300.sthdr_acl_fwd_cpu_pkt_fmt);

        /*Stacking Header*/
        osal_printf("--------------------------------------Packet Info-----------------------------------------\n");
        osal_printf("********************L2 Info********************\n");
        osal_printf("%-16s = %x:%x:%x:%x:%x:%x\n", "dmac[6]", hsb->r9300.dmac[0],hsb->r9300.dmac[1], hsb->r9300.dmac[2], hsb->r9300.dmac[3], hsb->r9300.dmac[4], hsb->r9300.dmac[5]);
        osal_printf("%-16s = %x:%x:%x:%x:%x:%x\n", "smac[6]", hsb->r9300.smac[0],hsb->r9300.smac[1], hsb->r9300.smac[2], hsb->r9300.smac[3], hsb->r9300.smac[4], hsb->r9300.smac[5]);

        osal_printf("%-16s = 0x%-6x\t", "sphy", hsb->r9300.sphy);
        osal_printf("%-16s = 0x%-6x\t", "l2_err_pkt", hsb->r9300.l2_err_pkt);
        osal_printf("%-16s = 0x%-6x\t\n", "typelen", hsb->r9300.typelen);

        osal_printf("%-16s = 0x%-6x\t", "llc_other", hsb->r9300.llc_other);
        osal_printf("%-16s = 0x%-6x\t", "dsap_ssap", hsb->r9300.dsap_ssap);
        osal_printf("%-16s = 0x%-6x\t\n", "pppoe_pkt", hsb->r9300.pppoe_pkt);

        osal_printf("%-16s = 0x%-6x\t", "rfc1042", hsb->r9300.rfc1042);
        osal_printf("%-16s = 0x%-6x\t", "pkt_len", hsb->r9300.pkt_len);
        osal_printf("%-16s = 0x%-6x\t\n", "igr_err", hsb->r9300.igr_err);

        osal_printf("%-16s = 0x%-6x\t", "page_cnt", hsb->r9300.page_cnt);
        osal_printf("%-16s = 0x%-6x\t", "fst_dsc", hsb->r9300.fst_dsc);
        osal_printf("%-16s = 0x%-6x\t\n", "lst_dsc", hsb->r9300.lst_dsc);
        osal_printf("********************VLAN Tag Info********************\n");
        osal_printf("%-16s = 0x%-6x\t", "otag_exist", hsb->r9300.otag_exist);
        osal_printf("%-16s = 0x%-6x\t", "otpid_idx", hsb->r9300.otpid_idx);
        osal_printf("%-16s = 0x%-6x\t\n", "opri", hsb->r9300.opri);

        osal_printf("%-16s = 0x%-6x\t", "ocfi", hsb->r9300.ocfi);
        osal_printf("%-16s = 0x%-6x\t\n", "ovid", hsb->r9300.ovid);

        osal_printf("%-16s = 0x%-6x\t", "itag_exist", hsb->r9300.itag_exist);
        osal_printf("%-16s = 0x%-6x\t", "itpid_idx", hsb->r9300.itpid_idx);
        osal_printf("%-16s = 0x%-6x\t\n", "ipri", hsb->r9300.ipri);

        osal_printf("%-16s = 0x%-6x\t", "icfi", hsb->r9300.icfi);
        osal_printf("%-16s = 0x%-6x\t\n", "ivid", hsb->r9300.ivid);

        osal_printf("%-16s = 0x%-6x\t", "rtag_exist", hsb->r9300.rtag_exist);
        osal_printf("%-16s = 0x%-6x\t", "rtag_type", hsb->r9300.rtag_type);
        osal_printf("%-16s = 0x%-6x\t\n", "etag_exist", hsb->r9300.etag_exist);
        osal_printf("********************L3/L4 Info********************\n");
        osal_printf("%-16s = 0x%-6x\t", "ipv4_pkt", hsb->r9300.ipv4_pkt);
        osal_printf("%-16s = 0x%-6x\t", "ipv6_pkt", hsb->r9300.ipv6_pkt);
        osal_printf("%-16s = 0x%-6x\t\n", "ip_ver", hsb->r9300.ip_ver);

        osal_printf("%-16s = 0x%-6x\t", "dip", hsb->r9300.dip);
        osal_printf("%-16s = 0x%-6x\t", "sip", hsb->r9300.sip);
        osal_printf("%-16s = 0x%-6x\t\n", "ip_ttl", hsb->r9300.ip_ttl);

        osal_printf("%-16s = 0x%-6x\t", "ip_tos", hsb->r9300.ip_tos);
        osal_printf("%-16s = 0x%-6x\t", "ip_protocol", hsb->r9300.ip_protocol);
        osal_printf("%-16s = 0x%-6x\t\n", "ip_flag", hsb->r9300.ip_flag);

        osal_printf("%-16s = 0x%-6x\t", "ip_offset", hsb->r9300.ip_offset);
        osal_printf("%-16s = 0x%-6x\t", "ip_length", hsb->r9300.ip_length);
        osal_printf("%-16s = 0x%-6x\t\n", "ipv4_hdrlen", hsb->r9300.ipv4_hdrlen);

        osal_printf("%-16s = 0x%-6x\t", "tcp_sn_eq_0", hsb->r9300.tcp_sn_eq_0);
        osal_printf("%-16s = 0x%-6x\t", "ipv6_hop", hsb->r9300.ipv6_hop);
        osal_printf("%-16s = 0x%-6x\t\n", "ipv6_rout", hsb->r9300.ipv6_rout);

        osal_printf("%-16s = 0x%-6x\t", "ipv6_frag", hsb->r9300.ipv6_frag);
        osal_printf("%-16s = 0x%-6x\t", "ipv6_dest", hsb->r9300.ipv6_dest);
        osal_printf("%-16s = 0x%-6x\t\n", "ipv6_auth", hsb->r9300.ipv6_auth);

        osal_printf("%-16s = 0x%-6x\t", "ipv4_chksum_ok", hsb->r9300.ipv4_chksum_ok);
        osal_printf("%-16s = 0x%-6x\t", "ipv6_ext_long", hsb->r9300.ipv6_ext_long);
        osal_printf("%-16s = 0x%-6x\t\n", "ipv6_flow_label", hsb->r9300.ipv6_flow_label);

        osal_printf("%-16s = 0x%-6x\t\n", "arp_opcode", hsb->r9300.arp_opcode);
        osal_printf("%-16s = %x:%x:%x:%x:%x:%x\n", "target_mac[6]", hsb->r9300.target_mac[0],hsb->r9300.target_mac[1], hsb->r9300.target_mac[2], hsb->r9300.target_mac[3], hsb->r9300.target_mac[4], hsb->r9300.target_mac[5]);
        osal_printf("%-16s = %x:%x:%x:%x:%x:%x\n", "sender_mac[6]", hsb->r9300.sender_mac[0],hsb->r9300.sender_mac[1], hsb->r9300.sender_mac[2], hsb->r9300.sender_mac[3], hsb->r9300.sender_mac[4], hsb->r9300.sender_mac[5]);

        osal_printf("%-16s = 0x%-6x\t", "dport", hsb->r9300.dport);
        osal_printf("%-16s = 0x%-6x\t", "sport", hsb->r9300.sport);
        osal_printf("%-16s = 0x%-6x\t\n", "tcp_flag", hsb->r9300.tcp_flag);

        osal_printf("%-16s = 0x%-6x\t\n", "l4_hdr_chk", hsb->r9300.l4_hdr_chk);

        osal_printf("********************CPU Tag Info********************\n");
        osal_printf("%-16s = 0x%-6x\t", "ctag_exist", hsb->r9300.ctag_exist);
        osal_printf("%-16s = 0x%-6x\t", "ctag_acl_act", hsb->r9300.ctag_acl_act);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_cngst_drop", hsb->r9300.ctag_cngst_drop);

        osal_printf("%-16s = 0x%-6x\t", "ctag_dg_pkt", hsb->r9300.ctag_dg_pkt);
        osal_printf("%-16s = 0x%-6x\t", "ctag_bp_fltr", hsb->r9300.ctag_bp_fltr);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_bp_stp", hsb->r9300.ctag_bp_stp);

        osal_printf("%-16s = 0x%-6x\t", "ctag_bp_vlan_egr", hsb->r9300.ctag_bp_vlan_egr);
        osal_printf("%-16s = 0x%-6x\t", "ctag_ale_as_tagsts", hsb->r9300.ctag_ale_as_tagsts);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_as_qid", hsb->r9300.ctag_as_qid);

        osal_printf("%-16s = 0x%-6x\t", "ctag_qid", hsb->r9300.ctag_qid);
        osal_printf("%-16s = 0x%-6x\t", "ctag_ori_tagif_en", hsb->r9300.ctag_ori_tagif_en);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_ori_itagif", hsb->r9300.ctag_ori_itagif);

        osal_printf("%-16s = 0x%-6x\t", "ctag_ori_otagif", hsb->r9300.ctag_ori_otagif);
        osal_printf("%-16s = 0x%-6x\t", "ctag_fwd_vid_sel", hsb->r9300.ctag_fwd_vid_sel);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_fwd_vid_en", hsb->r9300.ctag_fwd_vid_en);

        osal_printf("%-16s = 0x%-6x\t", "ctag_fwd_vid", hsb->r9300.ctag_fwd_vid);
        osal_printf("%-16s = 0x%-6x\t", "ctag_fwd_type", hsb->r9300.ctag_fwd_type);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_src_filter_en", hsb->r9300.ctag_src_filter_en);

        osal_printf("%-16s = 0x%-6x\t", "ctag_sp_is_trk", hsb->r9300.ctag_sp_is_trk);
        osal_printf("%-16s = 0x%-6x\t", "ctag_spn", hsb->r9300.ctag_spn);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_dev_id", hsb->r9300.ctag_dev_id);

        osal_printf("%-16s = 0x%-6x\t", "ctag_dpm_0", hsb->r9300.ctag_dpm.bits[0]);
        osal_printf("%-16s = 0x%-6x\t\n", "ctag_dpm_1", hsb->r9300.ctag_dpm.bits[1]);
        osal_printf("********************ACL related Info********************\n");
        osal_printf("%-16s = 0x%-6x\t", "fs11_data", hsb->r9300.fs11_data);
        osal_printf("%-16s = 0x%-6x\t", "fs10_data", hsb->r9300.fs10_data);
        osal_printf("%-16s = 0x%-6x\t\n", "fs9_data", hsb->r9300.fs9_data);

        osal_printf("%-16s = 0x%-6x\t", "fs8_data", hsb->r9300.fs8_data);
        osal_printf("%-16s = 0x%-6x\t", "fs7_data", hsb->r9300.fs7_data);
        osal_printf("%-16s = 0x%-6x\t\n", "fs6_data", hsb->r9300.fs6_data);

        osal_printf("%-16s = 0x%-6x\t", "fs5_data", hsb->r9300.fs5_data);
        osal_printf("%-16s = 0x%-6x\t", "fs4_data", hsb->r9300.fs4_data);
        osal_printf("%-16s = 0x%-6x\t\n", "fs3_data", hsb->r9300.fs3_data);

        osal_printf("%-16s = 0x%-6x\t", "fs2_data", hsb->r9300.fs2_data);
        osal_printf("%-16s = 0x%-6x\t", "fs1_data", hsb->r9300.fs1_data);
        osal_printf("%-16s = 0x%-6x\t\n", "fs0_data", hsb->r9300.fs0_data);

        osal_printf("----------------------------Loopback header Valid = %d-------------------------------\n", hsb->r9300.lbhdr_vld );
        osal_printf("%-16s = 0x%-6x\t", "lbhdr_org_srcport", hsb->r9300.lbhdr_org_srcport);
        osal_printf("%-16s = 0x%-6x\t", "lbhdr_lbttl", hsb->r9300.lbhdr_lbttl);
        osal_printf("%-16s = 0x%-6x\t\n", "lbhdr_metadata", hsb->r9300.lbhdr_metadata);

        osal_printf("--------------------------------------Others-----------------------------------------\n");
        osal_printf("%-16s = 0x%-6x\t", "udp_ptp", hsb->r9300.udp_ptp);
        osal_printf("%-16s = 0x%-6x\t", "l2_ptp", hsb->r9300.l2_ptp);
        osal_printf("%-16s = 0x%-6x\t\n", "oam_pdu", hsb->r9300.oam_pdu);

        osal_printf("\n====================================================================================\n");
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        osal_printf("\n========================================HSB============================================\n");
        osal_printf("---------- << HSB >> ------------------------------------------------------------------\n");
        osal_printf("PKTBUF_DESC : 0x%04X,0x%04X\t", hsb->r9310.HSB_BEGIN_DESC, hsb->r9310.HSB_END_DESC );
        osal_printf("PAGE : 0x%02X\t", hsb->r9310.HSB_PAGE );
        osal_printf("PKT_LEN     : 0x%04X (%d)\n", hsb->r9310.OHSB_PKT_LEN, hsb->r9310.OHSB_PKT_LEN );
        osal_printf("ERR_PKT     : 0x%X\t\t", hsb->r9310.OHSB_ERR_PKT );
        osal_printf("L2_CRC_ERR  : 0x%X\t", hsb->r9310.OHSB_L2_CRC_ERR );
        osal_printf("PKT_TYPE    : 0x%X\n", hsb->r9310.HSB_PKT_TYPE );
        osal_printf("FIELD_SEL12 : 0x%04X\t\t", hsb->r9310.HSB_FIELD_SEL12 );
        osal_printf("FIELD_SEL13 : 0x%04X\t", hsb->r9310.HSB_FIELD_SEL13 );
        osal_printf("FIELD_VALID : 0x%04X\n", hsb->r9310.HSB_FIELD_VALID );
        osal_printf("---------- << OHSB >> -----------------------------------------------------------------\n");
        osal_printf("PARSER_CANT_HANDLE : 0x%X\t", hsb->r9310.OHSB_PARSER_CANT_HANDLE );
        osal_printf("MALFORMED_PKT : 0x%X\t", hsb->r9310.OHSB_MALFORMED_PKT );
        osal_printf("SPP : 0x%X\n", hsb->r9310.OHSB_SPP );
        osal_printf("RTAG_IF : 0x%X\t\t", hsb->r9310.OHSB_RTAG_IF );
        osal_printf("RTAG_239_208 : 0x%08X\t", hsb->r9310.OHSB_RTAG_239_208 );
        osal_printf("RTAG_207_176 : 0x%08X\n\t\t\t", hsb->r9310.OHSB_RTAG_207_176 );
        osal_printf("RTAG_175_144 : 0x%08X\t", hsb->r9310.OHSB_RTAG_175_144 );
        osal_printf("RTAG_143_128 : 0x%04X\n", hsb->r9310.OHSB_RTAG_143_128 );
        osal_printf("DMAC_47_32 : 0x%04X\t", hsb->r9310.OHSB_DMAC_47_32 );
        osal_printf("DMAC_31_0 : 0x%08X\n", hsb->r9310.OHSB_DMAC_31_0 );
        osal_printf("SMAC_47_16 : 0x%08X\t", hsb->r9310.OHSB_SMAC_47_16 );
        osal_printf("SMAC_15_0 : 0x%04X\n", hsb->r9310.OHSB_SMAC_15_0 );
        osal_printf("EST_INVALID : 0x%X\t", hsb->r9310.HSB_EST_INVALID );
        osal_printf("PE_ETAG_IF : 0x%X\n", hsb->r9310.HSB_PE_ETAG_IF );
        osal_printf("PE_ETAG          : PRI 0x%X DEI 0x%X RSVD 0x%X\n", hsb->r9310.HSB_PE_ETAG_PRI, hsb->r9310.HSB_PE_ETAG_DEI, hsb->r9310.HSB_PE_ETAG_RSVD );
        osal_printf("PE_ETAG_IGR_ECID :         EXT 0x%02X BASE 0x%04X\n", hsb->r9310.HSB_PE_IGR_ECID_EXT, hsb->r9310.HSB_PE_IGR_ECID_BASE );
        osal_printf("PE_ETAG_ECID     : GRP 0x%X EXT 0x%02X BASE 0x%04X\n", hsb->r9310.HSB_PE_ECID_GRP, hsb->r9310.HSB_PE_ECID_EXT, hsb->r9310.HSB_PE_ECID_BASE );
        osal_printf("OTAG_IF    : 0x%X\t", hsb->r9310.OHSB_OTAG_IF );
        osal_printf("OTAG_TPID_IDX : 0x%X\t", hsb->r9310.OHSB_OTAG_TPID_IDX );
        osal_printf("OTAG:16 : 0x%04X\n", hsb->r9310.OHSB_OTAG );
        osal_printf("ITAG_IF    : 0x%X\t", hsb->r9310.OHSB_ITAG_IF );
        osal_printf("ITAG_TPID_IDX : 0x%X\t", hsb->r9310.OHSB_ITAG_TPID_IDX );
        osal_printf("ITAG:16 : 0x%04X\n", hsb->r9310.OHSB_ITAG );
        osal_printf("ETAG_IF    : 0x%X\t", hsb->r9310.OHSB_ETAG_IF );
        osal_printf("FRAME_TYPE    : 0x%X\t", hsb->r9310.OHSB_FRAME_TYPE );
        osal_printf("ETHTYPE : 0x%04X\n", hsb->r9310.OHSB_ETHTYPE );
        osal_printf("OMPLS_IF   : 0x%X\t", hsb->r9310.OHSB_OMPLS_IF );
        osal_printf("OMPLS_HDR     : 0x%X\n", hsb->r9310.OHSB_OMPLS_HDR );
        osal_printf("IMPLS_IF   : 0x%X\t", hsb->r9310.OHSB_IMPLS_IF );
        osal_printf("IMPLS_HDR     : 0x%X\n", hsb->r9310.OHSB_IMPLS_HDR );
        osal_printf("OAMPDU     : 0x%X\t", hsb->r9310.OHSB_OAMPDU );
        osal_printf("PPPOE_IF_MPLS_TYPE : 0x%X\n", hsb->r9310.OHSB_PPPOE_IF_MPLS_TYPE );
        osal_printf("IPV4_TYPE  : 0x%X\t", hsb->r9310.OHSB_IPV4_TYPE );
        osal_printf("IPV6_TYPE : 0x%X\t\t", hsb->r9310.OHSB_IPV6_TYPE );
        osal_printf("IP_VER       : 0x%X\n", hsb->r9310.OHSB_IP_VER );
        osal_printf("IP_HDR_LEN : 0x%X\t", hsb->r9310.OHSB_IP_HDR_LEN );
        osal_printf("IP_TOS    : 0x%02X\t", hsb->r9310.OHSB_IP_TOS );
        osal_printf("IP6_FLOW_LBL : 0x%05X\n", hsb->r9310.OHSB_IP6_FLOW_LBL );
        osal_printf("IP_LEN     : 0x%04X\t", hsb->r9310.OHSB_IP_LEN );
        osal_printf("IP_FLAG   : 0x%X\t\t", hsb->r9310.OHSB_IP_FLAG );
        osal_printf("IP_OFFSET    : 0x%04X\n", hsb->r9310.OHSB_IP_OFFSET );
        osal_printf("IP_TTL     : 0x%02X\t", hsb->r9310.OHSB_IP_TTL );
        osal_printf("IP_PROTO  : 0x%02X\t", hsb->r9310.OHSB_IP_PROTO );
        osal_printf("L4_OFFSET    : 0x%02X\n", hsb->r9310.OHSB_L4_OFFSET );
        osal_printf("IP_SIP3_FIELD_SEL11_10 : 0x%08X\n", hsb->r9310.OHSB_IP_SIP3_FIELD_SEL11_10 );
        osal_printf("IP_SIP2_FIELD_SEL9_8   : 0x%08X\n", hsb->r9310.OHSB_IP_SIP2_FIELD_SEL9_8 );
        osal_printf("IP_SIP1_FIELD_SEL7_6   : 0x%08X\n", hsb->r9310.OHSB_IP_SIP1_FIELD_SEL7_6 );
        osal_printf("IP_SIP  : 0x%08X\t", hsb->r9310.OHSB_IP_SIP );
        osal_printf("IP_DIP  : 0x%08X\n", hsb->r9310.OHSB_IP_DIP );
        osal_printf("IP_DIP3 : 0x%08X\t", hsb->r9310.OHSB_IP_DIP3 );
        osal_printf("IP_DIP2 : 0x%08X\t", hsb->r9310.OHSB_IP_DIP2 );
        osal_printf("IP_DIP1 : 0x%08X\n", hsb->r9310.OHSB_IP_DIP1 );
        osal_printf("L4_HDR_RDY : 0x%X\t", hsb->r9310.OHSB_L4_HDR_RDY );
        osal_printf("L4_SPORT    : 0x%04X\t", hsb->r9310.OHSB_L4_SPORT );
        osal_printf("L4_DPORT   : 0x%04X\n\t\t\t", hsb->r9310.OHSB_L4_DPORT );
        osal_printf("L4_OPCODE   : 0x%04X\t", hsb->r9310.OHSB_L4_OPCODE );
        osal_printf("L4_DATA    : 0x%04X\n", hsb->r9310.OHSB_L4_DATA );
        osal_printf("IP6_EH_HOPBYHOP_EXIST : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_HOPBYHOP_EXIST );
        osal_printf("IP6_EH_HOPBYHOP_ERR      : 0x%X\n", hsb->r9310.OHSB_IP6_EH_HOPBYHOP_ERR );
        osal_printf("IP6_EH_ROUTE_EXIST    : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_ROUTE_EXIST );
        osal_printf("IP6_EH_FRAGMENT_EXIST    : 0x%X\n", hsb->r9310.OHSB_IP6_EH_FRAGMENT_EXIST );
        osal_printf("IP6_EH_ESP_EXIST      : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_ESP_EXIST );
        osal_printf("IP6_EH_AUTH_EXIST        : 0x%X\n", hsb->r9310.OHSB_IP6_EH_AUTH_EXIST );
        osal_printf("IP6_EH_MOBILITY_EXIST : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_MOBILITY_EXIST );
        osal_printf("IP6_EH_DESTINATION_EXIST : 0x%X\n", hsb->r9310.OHSB_IP6_EH_DESTINATION_EXIST );
        osal_printf("IP6_EH_REPEAT         : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_REPEAT );
        osal_printf("IP6_EH_TOTAL_LEN         : 0x%02X\n", hsb->r9310.OHSB_IP6_EH_TOTAL_LEN );
        osal_printf("CAPWAP_PRE     : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_PRE );
        osal_printf("CAPWAP_HLEN : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_HLEN );
        osal_printf("CAPWAP_RID : 0x%02X\n", hsb->r9310.OHSB_CAPWAP_RID );
        osal_printf("CAPWAP_WBID    : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_WBID );
        osal_printf("CAPWAP_FLAG : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_FLAG );
        osal_printf("CAPWAP_RMAC_LEN : 0x%02X\n", hsb->r9310.OHSB_CAPWAP_RMAC_LEN );
        osal_printf("CAPWAP_HDR_RDY : 0x%X\t", hsb->r9310.OHSB_CAPWAP_HDR_RDY );
        osal_printf("CAPWAP_BSSID_VALID : 0x%X\t\n", hsb->r9310.OHSB_CAPWAP_BSSID_VALID );
        osal_printf("CAPWAP_BSSID_47_16 : 0x%08X\t\t", hsb->r9310.OHSB_CAPWAP_BSSID_47_16 );
        osal_printf("CAPWAP_BSSID_15_0 : 0x%04X\t\n", hsb->r9310.OHSB_CAPWAP_BSSID_15_0 );
        osal_printf("---------- << IHSB >> -----------------------------------------------------------------\n");
        osal_printf("PARSER_CANT_HANDLE : 0x%X\t", hsb->r9310.IHSB_PARSER_CANT_HANDLE );
        osal_printf("MALFORMED_PKT : 0x%X\t", hsb->r9310.IHSB_MALFORMED_PKT );
        osal_printf("L2_RDY : 0x%X\n", hsb->r9310.IHSB_L2_RDY );
        osal_printf("FRAME_CTRL : 0x%X\t\t", hsb->r9310.IHSB_FRAME_CTRL );
        osal_printf("QOS_CTRL : 0x%X\t\t\n", hsb->r9310.IHSB_QOS_CTRL );
        osal_printf("DMAC_47_16 : 0x%08X\t", hsb->r9310.IHSB_DMAC_47_16 );
        osal_printf("DMAC_15_0 : 0x%04X\n", hsb->r9310.IHSB_DMAC_15_0 );
        osal_printf("SMAC_47_32 : 0x%04X\t", hsb->r9310.IHSB_SMAC_47_32 );
        osal_printf("SMAC_31_0 : 0x%08X\n", hsb->r9310.IHSB_SMAC_31_0 );
        osal_printf("OTAG_IF    : 0x%X\t", hsb->r9310.IHSB_OTAG_IF );
        osal_printf("OTAG_TPID_IDX : 0x%X\t", hsb->r9310.IHSB_OTAG_TPID_IDX );
        osal_printf("OTAG:16 : 0x%04X\n", hsb->r9310.IHSB_OTAG );
        osal_printf("ITAG_IF    : 0x%X\t", hsb->r9310.IHSB_ITAG_IF );
        osal_printf("ITAG_TPID_IDX : 0x%X\t", hsb->r9310.IHSB_ITAG_TPID_IDX );
        osal_printf("ITAG:16 : 0x%04X\n", hsb->r9310.IHSB_ITAG );
        osal_printf("L3_OFFSET  : 0x%02X\t", hsb->r9310.IHSB_L3_OFFSET );
        osal_printf("FRAME_TYPE    : 0x%X\t", hsb->r9310.IHSB_FRAME_TYPE );
        osal_printf("ETHTYPE : 0x%04X\n", hsb->r9310.IHSB_ETHTYPE );
        osal_printf("IPV4_TYPE  : 0x%X\t", hsb->r9310.IHSB_IPV4_TYPE );
        osal_printf("IPV6_TYPE : 0x%X\t\t", hsb->r9310.IHSB_IPV6_TYPE );
        osal_printf("IP_VER       : 0x%X\n", hsb->r9310.IHSB_IP_VER );
        osal_printf("IP_HDR_LEN : 0x%X\t", hsb->r9310.IHSB_IP_HDR_LEN );
        osal_printf("IP_TOS    : 0x%02X\t", hsb->r9310.IHSB_IP_TOS );
        osal_printf("IP6_FLOW_LBL : 0x%05X\n", hsb->r9310.IHSB_IP6_FLOW_LBL );
        osal_printf("IP_LEN     : 0x%04X\t", hsb->r9310.IHSB_IP_LEN );
        osal_printf("IP_FLAG   : 0x%X\t\t", hsb->r9310.IHSB_IP_FLAG );
        osal_printf("IP_OFFSET    : 0x%04X\n", hsb->r9310.IHSB_IP_OFFSET );
        osal_printf("IP_TTL     : 0x%02X\t", hsb->r9310.IHSB_IP_TTL );
        osal_printf("IP_PROTO  : 0x%02X\t", hsb->r9310.IHSB_IP_PROTO );
        osal_printf("L4_OFFSET    : 0x%02X\n", hsb->r9310.IHSB_L4_OFFSET );
        osal_printf("IP_SIP3_FIELD_SEL5_4 : 0x%08X\n", hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 );
        osal_printf("IP_SIP2_FIELD_SEL3_2 : 0x%08X\n", hsb->r9310.IHSB_IP_SIP2_FIELD_SEL3_2 );
        osal_printf("IP_SIP1_FIELD_SEL1_0 : 0x%08X\n", hsb->r9310.IHSB_IP_SIP1_FIELD_SEL1_0 );
        osal_printf("IP_SIP  : 0x%08X\t", hsb->r9310.IHSB_IP_SIP );
        osal_printf("IP_DIP  : 0x%08X\n", hsb->r9310.IHSB_IP_DIP );
        osal_printf("IP_DIP3 : 0x%08X\t", hsb->r9310.IHSB_IP_DIP3 );
        osal_printf("IP_DIP2 : 0x%08X\t", hsb->r9310.IHSB_IP_DIP2 );
        osal_printf("IP_DIP1 : 0x%08X\n", hsb->r9310.IHSB_IP_DIP1 );
        osal_printf("L4_HDR_RDY : 0x%X\t", hsb->r9310.IHSB_L4_HDR_RDY );
        osal_printf("L4_SPORT    : 0x%04X\t", hsb->r9310.IHSB_L4_SPORT );
        osal_printf("L4_DPORT   : 0x%04X\n\t\t\t", hsb->r9310.IHSB_L4_DPORT );
        osal_printf("L4_OPCODE   : 0x%04X\t", hsb->r9310.IHSB_L4_OPCODE );
        osal_printf("L4_DATA    : 0x%04X\n", hsb->r9310.IHSB_L4_DATA );
        osal_printf("IP6_EH_HOPBYHOP_EXIST : 0x%X\t", hsb->r9310.IHSB_IP6_EH_HOPBYHOP_EXIST );
        osal_printf("IP6_EH_HOPBYHOP_ERR      : 0x%X\n", hsb->r9310.IHSB_IP6_EH_HOPBYHOP_ERR );
        osal_printf("IP6_EH_ROUTE_EXIST    : 0x%X\t", hsb->r9310.IHSB_IP6_EH_ROUTE_EXIST );
        osal_printf("IP6_EH_FRAGMENT_EXIST    : 0x%X\n", hsb->r9310.IHSB_IP6_EH_FRAGMENT_EXIST );
        osal_printf("IP6_EH_ESP_EXIST      : 0x%X\t", hsb->r9310.IHSB_IP6_EH_ESP_EXIST );
        osal_printf("IP6_EH_AUTH_EXIST        : 0x%X\n", hsb->r9310.IHSB_IP6_EH_AUTH_EXIST );
        osal_printf("IP6_EH_MOBILITY_EXIST : 0x%X\t", hsb->r9310.IHSB_IP6_EH_MOBILITY_EXIST );
        osal_printf("IP6_EH_DESTINATION_EXIST : 0x%X\n", hsb->r9310.IHSB_IP6_EH_DESTINATION_EXIST );
        osal_printf("IP6_EH_REPEAT         : 0x%X\t", hsb->r9310.IHSB_IP6_EH_REPEAT );
        osal_printf("IP6_EH_TOTAL_LEN         : 0x%02X\n", hsb->r9310.IHSB_IP6_EH_TOTAL_LEN );
        osal_printf("=======================================================================================\n");
    }
    else
#endif
    {
        osal_free(hsb);
        return RT_ERR_FAILED;
    }

    osal_free(hsb);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsb_openflow
 * Description:
 *      Dump hsb paramter for OpenFlow mode of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsb_openflow(uint32 unit)
{
    uint32 value = 0;
    hsb_param_t  *hsb;
    hal_control_t *pInfo;

    hsb = osal_alloc(sizeof(hsb_param_t));
    if (NULL == hsb)
    {
        osal_printf("osal_alloc hsb_param_t failed!!\n");
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK != _rtk_getHsb(unit, hsb))
    {
        osal_free(hsb);
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        osal_free(hsb);
        return RT_ERR_FAILED;
    }

    if (HWP_9310_FAMILY_ID(unit))
    {
        osal_printf("\n==================================HSB OpenFlow Loopback Mode==================================\n");
        osal_printf("---------- << HSB >> ------------------------------------------------------------------\n");
        osal_printf("PKTBUF_DESC : 0x%04X,0x%04X\t", hsb->r9310.HSB_BEGIN_DESC, hsb->r9310.HSB_END_DESC );
        osal_printf("PAGE : 0x%02X\t", hsb->r9310.HSB_PAGE );
        osal_printf("PKT_LEN     : 0x%04X (%d)\n", hsb->r9310.OHSB_PKT_LEN, hsb->r9310.OHSB_PKT_LEN );
        osal_printf("ERR_PKT     : 0x%X\t\t", hsb->r9310.OHSB_ERR_PKT );
        osal_printf("L2_CRC_ERR  : 0x%X\t", hsb->r9310.OHSB_L2_CRC_ERR );
        osal_printf("PKT_TYPE    : 0x%X\n", hsb->r9310.HSB_PKT_TYPE );
        osal_printf("FIELD_VALID : 0x%04X\n", hsb->r9310.HSB_FIELD_VALID );
        osal_printf("---------- << OHSB >> -----------------------------------------------------------------\n");
        osal_printf("PARSER_CANT_HANDLE : 0x%X\t", hsb->r9310.OHSB_PARSER_CANT_HANDLE );
        osal_printf("MALFORMED_PKT : 0x%X\t", hsb->r9310.OHSB_MALFORMED_PKT );
        osal_printf("SPP : 0x%X\n", hsb->r9310.OHSB_SPP );
        osal_printf("RTAG_IF : 0x%X\t\n", hsb->r9310.OHSB_RTAG_IF );

#if 0
        osal_printf("RTAG_239_208 : 0x%08X\t", hsb->r9310.OHSB_RTAG_239_208 );
        osal_printf("RTAG_207_176 : 0x%08X\n\t\t\t", hsb->r9310.OHSB_RTAG_207_176 );
        osal_printf("RTAG_175_144 : 0x%08X\t", hsb->r9310.OHSB_RTAG_175_144 );
        osal_printf("RTAG_143_128 : 0x%04X\n", hsb->r9310.OHSB_RTAG_143_128 );
        osal_printf("FIELD_SEL12 : 0x%04X\t\t", hsb->r9310.HSB_FIELD_SEL12 );
        osal_printf("FIELD_SEL13 : 0x%04X\t", hsb->r9310.HSB_FIELD_SEL13 );
        osal_printf("IP_SIP3_FIELD_SEL5_4 : 0x%08X\n", hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 );
        osal_printf("IP_SIP2_FIELD_SEL3_2 : 0x%08X\n", hsb->r9310.IHSB_IP_SIP2_FIELD_SEL3_2 );
        osal_printf("IP_SIP1_FIELD_SEL1_0 : 0x%08X\n", hsb->r9310.IHSB_IP_SIP1_FIELD_SEL1_0 );
        osal_printf("DMAC_47_16 : 0x%08X\t", hsb->r9310.IHSB_DMAC_47_16 );
        osal_printf("DMAC_15_0 : 0x%04X\n", hsb->r9310.IHSB_DMAC_15_0 );
        osal_printf("SMAC_47_32 : 0x%04X\t", hsb->r9310.IHSB_SMAC_47_32 );
        osal_printf("SMAC_31_0 : 0x%08X\n", hsb->r9310.IHSB_SMAC_31_0 );
#endif

        osal_printf("\n---------- << OpenFlow Loopback Header Start>> ---------------------------------------\n");
        value = (hsb->r9310.OHSB_RTAG_239_208 >> 26);
        osal_printf("SPORT : 0x%02X\t", value);
        value = (hsb->r9310.OHSB_RTAG_239_208 >> 10) & 0xffff;
        osal_printf("ORG_ITAG : 0x%04X\t", value);
        value = (hsb->r9310.OHSB_RTAG_239_208 >> 8) & 0x3;
        osal_printf("ORG_ITPID_IDX : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_239_208 >> 7) & 0x1;
        osal_printf("ORG_ITAG_IF : 0x%X\n", value);
        value = ((hsb->r9310.OHSB_RTAG_239_208 & 0x7f) << 9) | (hsb->r9310.OHSB_RTAG_207_176 >> 23);
        osal_printf("ORG_OTAG : 0x%04X\t", value);
        value = (hsb->r9310.OHSB_RTAG_207_176 >> 21) & 0x3;
        osal_printf("ORG_OTPID_IDX : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_207_176 >> 20) & 0x1;
        osal_printf("ORG_OTAG_IF : 0x%X\n", value);
        value = (hsb->r9310.OHSB_RTAG_207_176 >> 19) & 0x1;
        osal_printf("TO_OF : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_207_176 >> 13) & 0x3f;
        osal_printf("CURRENT_LB_TIME : 0x%02X\t", value);
        value = (hsb->r9310.OHSB_RTAG_207_176 >> 7) & 0x3f;
        osal_printf("TARGET_LB_TIME : 0x%02X\n", value);
        value = (hsb->r9310.OHSB_RTAG_207_176 >> 5) & 0x3;
        osal_printf("NEXT_TLB_ID : 0x%X\t", value);
        value = ((hsb->r9310.OHSB_RTAG_207_176 & 0x1f) << 7) | (hsb->r9310.OHSB_RTAG_175_144 >> 25);
        osal_printf("METADATA : 0x%03X\t", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 24) & 0x1;
        osal_printf("VALID_ACT_SET : 0x%X\n", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 23) & 0x1;
        osal_printf("CP_TTL_INWARD : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 22) & 0x1;
        osal_printf("POP_VLAN : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 20) & 0x3;
        osal_printf("POP_MPLS : 0x%X\t", (value & 0x2)>>1);
        osal_printf("POP_MPLS_TYPE : 0x%X\n", (value & 0x1));
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 18) & 0x3;
        osal_printf("PUSH_MPLS : 0x%X\t", (value & 0x2)>>1);
        osal_printf("PUSH_MPLS_MODE : 0x%X\t", (value & 0x1));
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 17) & 0x1;
        osal_printf("PUSH_MPLS_VPN_TYPE : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 6) & 0x7ff;
        osal_printf("PUSH_MPLS_LIB_IDX : 0x%X\n", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 5) & 0x1;
        osal_printf("PUSH_MPLS_ETHTYPE : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 4) & 0x1;
        osal_printf("PUSH_VLAN : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 2) & 0x3;
        osal_printf("PUSH_VLAN_ETHTYPE : 0x%X\n", value);
        value = (hsb->r9310.OHSB_RTAG_175_144 >> 1) & 0x1;
        osal_printf("CP_TTL_OUTWARD : 0x%X\t", value);
        value = hsb->r9310.OHSB_RTAG_175_144 & 0x1;
        osal_printf("DEC_MPLS_TTL : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_143_128 >> 15) & 0x1;
        osal_printf("DEC_IP_TTL : 0x%X\n", value);
        value = (hsb->r9310.OHSB_RTAG_143_128 >> 14) & 0x1;
        osal_printf("SET_FIELD_SA : 0x%X\t", value);
        value = (hsb->r9310.OHSB_RTAG_143_128 >> 4) & 0x3ff;
        osal_printf("SET_FIELD_SA_DATA : 0x%03X\t", value);
        value = (hsb->r9310.OHSB_RTAG_143_128 >> 3) & 0x1;
        osal_printf("SET_FIELD_DA : 0x%X\n", value);
        value = ((hsb->r9310.OHSB_RTAG_143_128 & 0x7) << 13) | (hsb->r9310.HSB_FIELD_SEL13 >> 3);
        osal_printf("SET_FIELD_DA_DATA : 0x%04X\t", value);
        value = (hsb->r9310.HSB_FIELD_SEL13 >> 2) & 0x1;
        osal_printf("SET_FIELD_VID : 0x%X\t", value);
        value = ((hsb->r9310.HSB_FIELD_SEL13  & 0x3) << 10) | (hsb->r9310.HSB_FIELD_SEL12 >> 6);
        osal_printf("SET_FIELD_VID_DATA : 0x%03X\n", value);
        value = (hsb->r9310.HSB_FIELD_SEL12 >> 5) & 0x1;
        osal_printf("SET_FIELD_PCP : 0x%X\t", value);
        value = (hsb->r9310.HSB_FIELD_SEL12 >> 2) & 0x7;
        osal_printf("SET_FIELD_PCP_DATA : 0x%X\t", value);
        value = (hsb->r9310.HSB_FIELD_SEL12 >> 1) & 0x1;
        osal_printf("SET_FIELD_MPLS_LIB : 0x%X\n", value);
        value = ((hsb->r9310.HSB_FIELD_SEL12 & 0x1) << 10) | (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 22);
        osal_printf("SET_FIELD_MPLS_LIB_DATA : 0x%03X\t", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 21) & 0x1;
        osal_printf("SET_FIELD_MPLS_TC : 0x%X\t", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 18) & 0x7;
        osal_printf("SET_FIELD_MPLS_TC_DATA : 0x%X\n", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 17) & 0x1;
        osal_printf("SET_FIELD_MPLS_TTL : 0x%X\t", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 9) & 0xff;
        osal_printf("SET_FIELD_MPLS_TTL_DATA : 0x%02X\t", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 8) & 0x1;
        osal_printf("SET_FIELD_IP_DSCP : 0x%X\n", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 2) & 0x3f;
        osal_printf("SET_FIELD_IP_DSCP_DATA : 0x%02X\t", value);
        value = (hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 >> 1) & 0x1;
        osal_printf("SET_FIELD_IP_TTL : 0x%X\t", value);
        value = ((hsb->r9310.IHSB_IP_SIP3_FIELD_SEL5_4 & 0x1) << 7) | (hsb->r9310.IHSB_IP_SIP2_FIELD_SEL3_2 >> 25);
        osal_printf("SET_FIELD_IP_TTL_DATA : 0x%02X\n", value);
        value = (hsb->r9310.IHSB_IP_SIP2_FIELD_SEL3_2 >> 24) & 0x1;
        osal_printf("SET_FIELD_IP_SIP : 0x%X\t", value);
        value = ((hsb->r9310.IHSB_IP_SIP2_FIELD_SEL3_2 & 0xffffff) << 8) | (hsb->r9310.IHSB_IP_SIP1_FIELD_SEL1_0 >> 24);
        osal_printf("SET_FIELD_IP_SIP_DATA : 0x%08X\t", value);
        value = (hsb->r9310.IHSB_IP_SIP1_FIELD_SEL1_0 >> 23) & 0x1;
        osal_printf("SET_FIELD_IP_DIP : 0x%X\n", value);
        value = ((hsb->r9310.IHSB_IP_SIP1_FIELD_SEL1_0 & 0x7fffff) << 9) | (hsb->r9310.IHSB_DMAC_47_16 >> 23);
        osal_printf("SET_FIELD_IP_DIP_DATA : 0x%08X\t", value);
        value = (hsb->r9310.IHSB_DMAC_47_16 >> 22) & 0x1;
        osal_printf("SET_FIELD_L4_SPORT : 0x%X\t", value);
        value = (hsb->r9310.IHSB_DMAC_47_16 >> 6) & 0xffff;
        osal_printf("SET_FIELD_L4_SPORT_DATA : 0x%04X\n", value);
        value = (hsb->r9310.IHSB_DMAC_47_16 >> 5) & 0x1;
        osal_printf("SET_FIELD_L4_DPORT : 0x%X\t", value);
        value = ((hsb->r9310.IHSB_DMAC_47_16 & 0x1f) << 11) | (hsb->r9310.IHSB_DMAC_15_0 >> 5);
        osal_printf("SET_FIELD_L4_DPORT_DATA : 0x%04X\t", value);
        value = (hsb->r9310.IHSB_DMAC_15_0 >> 4) & 0x1;
        osal_printf("SET_FIELD_IP_RSVD : 0x%X\n", value);
        value = (hsb->r9310.IHSB_DMAC_15_0 >> 3) & 0x1;
        osal_printf("SET_QUEUE : 0x%X\t", value);
        value = hsb->r9310.IHSB_DMAC_15_0 & 0x7;
        osal_printf("SET_QUEUE_DATA : 0x%X\t", value);
        value = (hsb->r9310.IHSB_SMAC_47_32 >> 15) & 0x1;
        osal_printf("GROUP : 0x%X\n", value);
        value = (hsb->r9310.IHSB_SMAC_47_32 >> 4) & 0x7ff;
        osal_printf("GROUP_IDX : 0x%03X\t", value);
        value = hsb->r9310.IHSB_SMAC_47_32 & 0xf;
        osal_printf("OUTPUT_TYPE : 0x%X\t", value);
        value = (hsb->r9310.IHSB_SMAC_31_0 >> 20) & 0xffff;
        osal_printf("OUTPUT_DATA : 0x%03X\n", value);
        osal_printf("---------- << OpenFlow Loopback Header End>> ---------------------------------------\n\n");

        osal_printf("DMAC_47_32 : 0x%04X\t", hsb->r9310.OHSB_DMAC_47_32 );
        osal_printf("DMAC_31_0 : 0x%08X\n", hsb->r9310.OHSB_DMAC_31_0 );
        osal_printf("SMAC_47_16 : 0x%08X\t", hsb->r9310.OHSB_SMAC_47_16 );
        osal_printf("SMAC_15_0 : 0x%04X\n", hsb->r9310.OHSB_SMAC_15_0 );
        osal_printf("EST_INVALID : 0x%X\t", hsb->r9310.HSB_EST_INVALID );
        osal_printf("PE_ETAG_IF : 0x%X\n", hsb->r9310.HSB_PE_ETAG_IF );
        osal_printf("PE_ETAG          : PRI 0x%X DEI 0x%X RSVD 0x%X\n", hsb->r9310.HSB_PE_ETAG_PRI, hsb->r9310.HSB_PE_ETAG_DEI, hsb->r9310.HSB_PE_ETAG_RSVD );
        osal_printf("PE_ETAG_IGR_ECID :         EXT 0x%02X BASE 0x%04X\n", hsb->r9310.HSB_PE_IGR_ECID_EXT, hsb->r9310.HSB_PE_IGR_ECID_BASE );
        osal_printf("PE_ETAG_ECID     : GRP 0x%X EXT 0x%02X BASE 0x%04X\n", hsb->r9310.HSB_PE_ECID_GRP, hsb->r9310.HSB_PE_ECID_EXT, hsb->r9310.HSB_PE_ECID_BASE );
        osal_printf("OTAG_IF    : 0x%X\t", hsb->r9310.OHSB_OTAG_IF );
        osal_printf("OTAG_TPID_IDX : 0x%X\t", hsb->r9310.OHSB_OTAG_TPID_IDX );
        osal_printf("OTAG:16 : 0x%04X\n", hsb->r9310.OHSB_OTAG );
        osal_printf("ITAG_IF    : 0x%X\t", hsb->r9310.OHSB_ITAG_IF );
        osal_printf("ITAG_TPID_IDX : 0x%X\t", hsb->r9310.OHSB_ITAG_TPID_IDX );
        osal_printf("ITAG:16 : 0x%04X\n", hsb->r9310.OHSB_ITAG );
        osal_printf("ETAG_IF    : 0x%X\t", hsb->r9310.OHSB_ETAG_IF );
        osal_printf("FRAME_TYPE    : 0x%X\t", hsb->r9310.OHSB_FRAME_TYPE );
        osal_printf("ETHTYPE : 0x%04X\n", hsb->r9310.OHSB_ETHTYPE );
        osal_printf("OMPLS_IF   : 0x%X\t", hsb->r9310.OHSB_OMPLS_IF );
        osal_printf("OMPLS_HDR     : 0x%X\n", hsb->r9310.OHSB_OMPLS_HDR );
        osal_printf("IMPLS_IF   : 0x%X\t", hsb->r9310.OHSB_IMPLS_IF );
        osal_printf("IMPLS_HDR     : 0x%X\n", hsb->r9310.OHSB_IMPLS_HDR );
        osal_printf("OAMPDU     : 0x%X\t", hsb->r9310.OHSB_OAMPDU );
        osal_printf("PPPOE_IF_MPLS_TYPE : 0x%X\n", hsb->r9310.OHSB_PPPOE_IF_MPLS_TYPE );
        osal_printf("IPV4_TYPE  : 0x%X\t", hsb->r9310.OHSB_IPV4_TYPE );
        osal_printf("IPV6_TYPE : 0x%X\t\t", hsb->r9310.OHSB_IPV6_TYPE );
        osal_printf("IP_VER       : 0x%X\n", hsb->r9310.OHSB_IP_VER );
        osal_printf("IP_HDR_LEN : 0x%X\t", hsb->r9310.OHSB_IP_HDR_LEN );
        osal_printf("IP_TOS    : 0x%02X\t", hsb->r9310.OHSB_IP_TOS );
        osal_printf("IP6_FLOW_LBL : 0x%05X\n", hsb->r9310.OHSB_IP6_FLOW_LBL );
        osal_printf("IP_LEN     : 0x%04X\t", hsb->r9310.OHSB_IP_LEN );
        osal_printf("IP_FLAG   : 0x%X\t\t", hsb->r9310.OHSB_IP_FLAG );
        osal_printf("IP_OFFSET    : 0x%04X\n", hsb->r9310.OHSB_IP_OFFSET );
        osal_printf("IP_TTL     : 0x%02X\t", hsb->r9310.OHSB_IP_TTL );
        osal_printf("IP_PROTO  : 0x%02X\t", hsb->r9310.OHSB_IP_PROTO );
        osal_printf("L4_OFFSET    : 0x%02X\n", hsb->r9310.OHSB_L4_OFFSET );
        osal_printf("IP_SIP3_FIELD_SEL11_10 : 0x%08X\n", hsb->r9310.OHSB_IP_SIP3_FIELD_SEL11_10 );
        osal_printf("IP_SIP2_FIELD_SEL9_8   : 0x%08X\n", hsb->r9310.OHSB_IP_SIP2_FIELD_SEL9_8 );
        osal_printf("IP_SIP1_FIELD_SEL7_6   : 0x%08X\n", hsb->r9310.OHSB_IP_SIP1_FIELD_SEL7_6 );
        osal_printf("IP_SIP  : 0x%08X\t", hsb->r9310.OHSB_IP_SIP );
        osal_printf("IP_DIP  : 0x%08X\n", hsb->r9310.OHSB_IP_DIP );
        osal_printf("IP_DIP3 : 0x%08X\t", hsb->r9310.OHSB_IP_DIP3 );
        osal_printf("IP_DIP2 : 0x%08X\t", hsb->r9310.OHSB_IP_DIP2 );
        osal_printf("IP_DIP1 : 0x%08X\n", hsb->r9310.OHSB_IP_DIP1 );
        osal_printf("L4_HDR_RDY : 0x%X\t", hsb->r9310.OHSB_L4_HDR_RDY );
        osal_printf("L4_SPORT    : 0x%04X\t", hsb->r9310.OHSB_L4_SPORT );
        osal_printf("L4_DPORT   : 0x%04X\n\t\t\t", hsb->r9310.OHSB_L4_DPORT );
        osal_printf("L4_OPCODE   : 0x%04X\t", hsb->r9310.OHSB_L4_OPCODE );
        osal_printf("L4_DATA    : 0x%04X\n", hsb->r9310.OHSB_L4_DATA );
        osal_printf("IP6_EH_HOPBYHOP_EXIST : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_HOPBYHOP_EXIST );
        osal_printf("IP6_EH_HOPBYHOP_ERR      : 0x%X\n", hsb->r9310.OHSB_IP6_EH_HOPBYHOP_ERR );
        osal_printf("IP6_EH_ROUTE_EXIST    : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_ROUTE_EXIST );
        osal_printf("IP6_EH_FRAGMENT_EXIST    : 0x%X\n", hsb->r9310.OHSB_IP6_EH_FRAGMENT_EXIST );
        osal_printf("IP6_EH_ESP_EXIST      : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_ESP_EXIST );
        osal_printf("IP6_EH_AUTH_EXIST        : 0x%X\n", hsb->r9310.OHSB_IP6_EH_AUTH_EXIST );
        osal_printf("IP6_EH_MOBILITY_EXIST : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_MOBILITY_EXIST );
        osal_printf("IP6_EH_DESTINATION_EXIST : 0x%X\n", hsb->r9310.OHSB_IP6_EH_DESTINATION_EXIST );
        osal_printf("IP6_EH_REPEAT         : 0x%X\t\t", hsb->r9310.OHSB_IP6_EH_REPEAT );
        osal_printf("IP6_EH_TOTAL_LEN         : 0x%02X\n", hsb->r9310.OHSB_IP6_EH_TOTAL_LEN );
        osal_printf("CAPWAP_PRE     : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_PRE );
        osal_printf("CAPWAP_HLEN : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_HLEN );
        osal_printf("CAPWAP_RID : 0x%02X\n", hsb->r9310.OHSB_CAPWAP_RID );
        osal_printf("CAPWAP_WBID    : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_WBID );
        osal_printf("CAPWAP_FLAG : 0x%02X\t", hsb->r9310.OHSB_CAPWAP_FLAG );
        osal_printf("CAPWAP_RMAC_LEN : 0x%02X\n", hsb->r9310.OHSB_CAPWAP_RMAC_LEN );
        osal_printf("CAPWAP_HDR_RDY : 0x%X\t", hsb->r9310.OHSB_CAPWAP_HDR_RDY );
        osal_printf("CAPWAP_BSSID_VALID : 0x%X\t\n", hsb->r9310.OHSB_CAPWAP_BSSID_VALID );
        osal_printf("CAPWAP_BSSID_47_16 : 0x%08X\t\t", hsb->r9310.OHSB_CAPWAP_BSSID_47_16 );
        osal_printf("CAPWAP_BSSID_15_0 : 0x%04X\t\n", hsb->r9310.OHSB_CAPWAP_BSSID_15_0 );
        osal_printf("---------- << IHSB >> -----------------------------------------------------------------\n");
        osal_printf("PARSER_CANT_HANDLE : 0x%X\t", hsb->r9310.IHSB_PARSER_CANT_HANDLE );
        osal_printf("MALFORMED_PKT : 0x%X\t", hsb->r9310.IHSB_MALFORMED_PKT );
        osal_printf("L2_RDY : 0x%X\n", hsb->r9310.IHSB_L2_RDY );
        osal_printf("FRAME_CTRL : 0x%X\t\t", hsb->r9310.IHSB_FRAME_CTRL );
        osal_printf("QOS_CTRL : 0x%X\t\t\n", hsb->r9310.IHSB_QOS_CTRL );
        osal_printf("OTAG_IF    : 0x%X\t", hsb->r9310.IHSB_OTAG_IF );
        osal_printf("OTAG_TPID_IDX : 0x%X\t", hsb->r9310.IHSB_OTAG_TPID_IDX );
        osal_printf("OTAG:16 : 0x%04X\n", hsb->r9310.IHSB_OTAG );
        osal_printf("ITAG_IF    : 0x%X\t", hsb->r9310.IHSB_ITAG_IF );
        osal_printf("ITAG_TPID_IDX : 0x%X\t", hsb->r9310.IHSB_ITAG_TPID_IDX );
        osal_printf("ITAG:16 : 0x%04X\n", hsb->r9310.IHSB_ITAG );
        osal_printf("L3_OFFSET  : 0x%02X\t", hsb->r9310.IHSB_L3_OFFSET );
        osal_printf("FRAME_TYPE    : 0x%X\t", hsb->r9310.IHSB_FRAME_TYPE );
        osal_printf("ETHTYPE : 0x%04X\n", hsb->r9310.IHSB_ETHTYPE );
        osal_printf("IPV4_TYPE  : 0x%X\t", hsb->r9310.IHSB_IPV4_TYPE );
        osal_printf("IPV6_TYPE : 0x%X\t\t", hsb->r9310.IHSB_IPV6_TYPE );
        osal_printf("IP_VER       : 0x%X\n", hsb->r9310.IHSB_IP_VER );
        osal_printf("IP_HDR_LEN : 0x%X\t", hsb->r9310.IHSB_IP_HDR_LEN );
        osal_printf("IP_TOS    : 0x%02X\t", hsb->r9310.IHSB_IP_TOS );
        osal_printf("IP6_FLOW_LBL : 0x%05X\n", hsb->r9310.IHSB_IP6_FLOW_LBL );
        osal_printf("IP_LEN     : 0x%04X\t", hsb->r9310.IHSB_IP_LEN );
        osal_printf("IP_FLAG   : 0x%X\t\t", hsb->r9310.IHSB_IP_FLAG );
        osal_printf("IP_OFFSET    : 0x%04X\n", hsb->r9310.IHSB_IP_OFFSET );
        osal_printf("IP_TTL     : 0x%02X\t", hsb->r9310.IHSB_IP_TTL );
        osal_printf("IP_PROTO  : 0x%02X\t", hsb->r9310.IHSB_IP_PROTO );
        osal_printf("L4_OFFSET    : 0x%02X\n", hsb->r9310.IHSB_L4_OFFSET );
        osal_printf("IP_SIP  : 0x%08X\t", hsb->r9310.IHSB_IP_SIP );
        osal_printf("IP_DIP  : 0x%08X\n", hsb->r9310.IHSB_IP_DIP );
        osal_printf("IP_DIP3 : 0x%08X\t", hsb->r9310.IHSB_IP_DIP3 );
        osal_printf("IP_DIP2 : 0x%08X\t", hsb->r9310.IHSB_IP_DIP2 );
        osal_printf("IP_DIP1 : 0x%08X\n", hsb->r9310.IHSB_IP_DIP1 );
        osal_printf("L4_HDR_RDY : 0x%X\t", hsb->r9310.IHSB_L4_HDR_RDY );
        osal_printf("L4_SPORT    : 0x%04X\t", hsb->r9310.IHSB_L4_SPORT );
        osal_printf("L4_DPORT   : 0x%04X\n\t\t\t", hsb->r9310.IHSB_L4_DPORT );
        osal_printf("L4_OPCODE   : 0x%04X\t", hsb->r9310.IHSB_L4_OPCODE );
        osal_printf("L4_DATA    : 0x%04X\n", hsb->r9310.IHSB_L4_DATA );
        osal_printf("IP6_EH_HOPBYHOP_EXIST : 0x%X\t", hsb->r9310.IHSB_IP6_EH_HOPBYHOP_EXIST );
        osal_printf("IP6_EH_HOPBYHOP_ERR      : 0x%X\n", hsb->r9310.IHSB_IP6_EH_HOPBYHOP_ERR );
        osal_printf("IP6_EH_ROUTE_EXIST    : 0x%X\t", hsb->r9310.IHSB_IP6_EH_ROUTE_EXIST );
        osal_printf("IP6_EH_FRAGMENT_EXIST    : 0x%X\n", hsb->r9310.IHSB_IP6_EH_FRAGMENT_EXIST );
        osal_printf("IP6_EH_ESP_EXIST      : 0x%X\t", hsb->r9310.IHSB_IP6_EH_ESP_EXIST );
        osal_printf("IP6_EH_AUTH_EXIST        : 0x%X\n", hsb->r9310.IHSB_IP6_EH_AUTH_EXIST );
        osal_printf("IP6_EH_MOBILITY_EXIST : 0x%X\t", hsb->r9310.IHSB_IP6_EH_MOBILITY_EXIST );
        osal_printf("IP6_EH_DESTINATION_EXIST : 0x%X\n", hsb->r9310.IHSB_IP6_EH_DESTINATION_EXIST );
        osal_printf("IP6_EH_REPEAT         : 0x%X\t", hsb->r9310.IHSB_IP6_EH_REPEAT );
        osal_printf("IP6_EH_TOTAL_LEN         : 0x%02X\n", hsb->r9310.IHSB_IP6_EH_TOTAL_LEN );
        osal_printf("=======================================================================================\n");
    }
    else
    {
        osal_free(hsb);
        return RT_ERR_FAILED;
    }

    osal_free(hsb);
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
int32 _rtk_getHsa(uint32 unit, hsa_param_t * hsa)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
    uint32 value;
#endif
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32 tmpVal = 0;
#endif
#if defined(CONFIG_SDK_RTL8390)
    uint32 port, reg, field;
    uint32 tmpVal1 = 0, tmpVal2 = 0;
#endif
#if defined(CONFIG_SDK_RTL9310)
    uint32 i;
    uint32 shift_bit = 0;
#endif
    hal_control_t *pInfo;

    if(NULL == hsa)
    {
        return RT_ERR_FAILED;
    }

    osal_memset(hsa, 0, sizeof(hsa_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }


#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        /* HSA_DATA0 */
        reg_read(unit, CYPRESS_HSA_DATA0r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_PAGE_CNTf, &tmpVal, &value);
        hsa->r8390.page_cnt = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_ACL_MPLS_HITf, &tmpVal, &value);
        hsa->r8390.acl_mpls_hit = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_ACL_MPLS_ACTf, &tmpVal, &value);
        hsa->r8390.acl_mpls_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_MPLS_INDEXf, &tmpVal, &value);
        hsa->r8390.mpls_index = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_ASTAGSTSf, &tmpVal, &value);
        hsa->r8390.astagsts = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_CTAG_DMf, &tmpVal, &value);
        hsa->r8390.ctag_dm = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_CTAG_DGPKTf, &tmpVal, &value);
        hsa->r8390.ctag_dgpkt = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_TX_PTP_LOGf, &tmpVal, &value);
        hsa->r8390.tx_ptp_log = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_TX_PTP_OFFLOADf, &tmpVal, &value);
        hsa->r8390.tx_ptp_offload = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_NEW_SAf, &tmpVal, &value);
        hsa->r8390.new_sa = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_MAC_CSTf, &tmpVal, &value);
        hsa->r8390.mac_cst = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA0r, CYPRESS_STC_L2_PMVf, &tmpVal, &value);
        hsa->r8390.l2_pmv = tmpVal;

        /* HSA_DATA1 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA1r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_ATK_TYPEf, &tmpVal, &value);
        hsa->r8390.atk_type = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_DM_RXIDXf, &tmpVal, &value);
        hsa->r8390.dm_rxidx = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_ACL_HITf, &tmpVal, &value);
        hsa->r8390.acl_hit = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_ACL_IDf, &tmpVal, &value);
        hsa->r8390.acl_id = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_EAV_CLASS_Bf, &tmpVal, &value);
        hsa->r8390.eav_class_b = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_EAV_CLASS_Af, &tmpVal, &value);
        hsa->r8390.eav_class_a = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA1r, CYPRESS_REASONf, &tmpVal, &value);
        hsa->r8390.reason = tmpVal;

        /* HSA_DATA2 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA2r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_ACL_TOSf, &tmpVal, &value);
        hsa->r8390.acl_tos = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_NEW_TOSf, &tmpVal, &value);
        hsa->r8390.new_tos = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_INTERNAL_PRIf, &tmpVal, &value);
        hsa->r8390.internal_pri = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_OPRI_ACTf, &tmpVal, &value);
        hsa->r8390.opri_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_IPRI_ACTf, &tmpVal, &value);
        hsa->r8390.ipri_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_C2SC_PRIf, &tmpVal, &value);
        hsa->r8390.c2sc_pri = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_EACL_ITAGf, &tmpVal, &value);
        hsa->r8390.eacl_itag = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_EACL_OTAGf, &tmpVal, &value);
        hsa->r8390.eacl_otag = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_C2SC_ITAGf, &tmpVal, &value);
        hsa->r8390.c2sc_itag = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_C2SC_OTAGf, &tmpVal, &value);
        hsa->r8390.c2sc_otag = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_ITAG_STATUSf, &tmpVal, &value);
        hsa->r8390.itag_status = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_OTAG_STATUSf, &tmpVal, &value);
        hsa->r8390.otag_status = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_EACL_IVIDf, &tmpVal, &value);
        hsa->r8390.eacl_ivid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_C2SC_IVIDf, &tmpVal, &value);
        hsa->r8390.c2sc_ivid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_EACL_OTPIDf, &tmpVal, &value);
        hsa->r8390.eacl_otpid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_OTPID_IDXf, &tmpVal, &value);
        hsa->r8390.otpid_idx = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_EACL_ITPIDf, &tmpVal, &value);
        hsa->r8390.eacl_itpid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_C2SC_ITPIDf, &tmpVal, &value);
        hsa->r8390.c2sc_itpid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA2r, CYPRESS_ITPID_IDXf, &tmpVal, &value);
        hsa->r8390.itpid_idx = tmpVal;

        /* HSA_DATA3 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA3r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA3r, CYPRESS_NEW_OTAGf, &tmpVal, &value);
        hsa->r8390.new_otag = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA3r, CYPRESS_NEW_ITAGf, &tmpVal, &value);
        hsa->r8390.new_itag = tmpVal;

        /* HSA_DATA4 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA4r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA4r, CYPRESS_ALE_OVIDf, &tmpVal, &value);
        hsa->r8390.ale_ovid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA4r, CYPRESS_L3_ROUTEf, &tmpVal, &value);
        hsa->r8390.l3_route = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA4r, CYPRESS_DA_PIDf, &tmpVal, &value);
        hsa->r8390.da_pid = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA4r, CYPRESS_NORMAL_FWDf, &tmpVal, &value);
        hsa->r8390.normal_fwd = tmpVal;

        /* HSA_DATA5 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA5r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_SWAP_MACf, &tmpVal, &value);
        hsa->r8390.swap_mac = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_CPU_FWDf, &tmpVal, &value);
        hsa->r8390.cpu_fwd = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_SFLOW_INFOf, &tmpVal, &value);
        hsa->r8390.sflow_info = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_MIRROR_1_INFOf, &tmpVal, &value);
        hsa->r8390.mirror_1_info = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_MIRROR_2_INFOf, &tmpVal, &value);
        hsa->r8390.mirror_2_info = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_MIRROR_3_INFOf, &tmpVal, &value);
        hsa->r8390.mirror_3_info = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA5r, CYPRESS_MIRROR_4_INFOf, &tmpVal, &value);
        hsa->r8390.mirror_4_info = tmpVal;

        /* HSA_DATA6 ~ HSA_DATA11*/
        for (port = 0; port < 53; port++)
        {
            reg = CYPRESS_HSA_DATA6r + (port / 10);
            if (0 == ((53 - port) / 10))
                field = CYPRESS_PORT50_QIDf - (port % 10);
            else
                field = CYPRESS_PORT9_QIDf + ((port / 10) * 10) + (9 - (port % 10));

            tmpVal1 = 0;
            reg_field_read(unit, reg, field, &tmpVal1);
            hsa->r8390.qid[port].qid = tmpVal1;
        }

        /* HSA_DATA12 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA12r, &value);  /* FIX ME */
        reg_field_get(unit, CYPRESS_HSA_DATA12r, CYPRESS_COLORf, &tmpVal, &value);
        hsa->r8390.color = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA12r, CYPRESS_RVID_SELf, &tmpVal, &value);
        hsa->r8390.rvid_sel = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA12r, CYPRESS_RVIDf, &tmpVal, &value);
        hsa->r8390.rvid = tmpVal;

        /* HSA_DATA13, 14 */
        tmpVal1 = 0;
        tmpVal2 = 0;
        reg_field_read(unit, CYPRESS_HSA_DATA13r, CYPRESS_DPM_52_32f, &tmpVal2);
        reg_field_read(unit, CYPRESS_HSA_DATA14r, CYPRESS_DPM_31_0f, &tmpVal1);
        hsa->r8390.dpm.bits[1] = tmpVal2;
        hsa->r8390.dpm.bits[0] = tmpVal1;

        tmpVal = 0;
        reg_field_read(unit, CYPRESS_HSA_DATA13r, CYPRESS_DPCf, &tmpVal);
        hsa->r8390.dpc = tmpVal;

        /* HSA_DATA15 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA15r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA15r, CYPRESS_ENDSCf, &tmpVal, &value);
        hsa->r8390.endsc = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA15r, CYPRESS_BGDSCf, &tmpVal, &value);
        hsa->r8390.bgdsc = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA15r, CYPRESS_STPID_IDXf, &tmpVal, &value);
        hsa->r8390.stpid_idx = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA15r, CYPRESS_CTPID_IDXf, &tmpVal2, &value);
        hsa->r8390.ctpid_idx = tmpVal2;

        /* HSA_DATA16 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA16r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA16r, CYPRESS_ORG_OTAGf, &tmpVal, &value);
        hsa->r8390.org_otag = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA16r, CYPRESS_ORG_ITAGf, &tmpVal, &value);
        hsa->r8390.org_itag = tmpVal;

        /* HSA_DATA17 */
        value = 0;
        reg_read(unit, CYPRESS_HSA_DATA17r, &value);
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_OMPLS_IFf, &tmpVal, &value);
        hsa->r8390.ompls_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_IMPLS_IFf, &tmpVal, &value);
        hsa->r8390.impls_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_ORG_ETAG_IFf, &tmpVal, &value);
        hsa->r8390.org_etag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_ORG_CPUTAG_IFf, &tmpVal, &value);
        hsa->r8390.org_cputag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_ORG_OTAG_IFf, &tmpVal, &value);
        hsa->r8390.org_otag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_ORG_ITAG_IFf, &tmpVal, &value);
        hsa->r8390.org_itag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_IPV6f, &tmpVal, &value);
        hsa->r8390.ipv6 = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_LLC_OTHERf, &tmpVal, &value);
        hsa->r8390.llc_other = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_PPPOE_IFf, &tmpVal, &value);
        hsa->r8390.pppoe_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_RFC1042f, &tmpVal, &value);
        hsa->r8390.rfc1042 = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_SPNf, &tmpVal, &value);
        hsa->r8390.spa = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_ERRPKTf, &tmpVal, &value);
        hsa->r8390.errpkt = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_IPV4f, &tmpVal, &value);
        hsa->r8390.ipv4 = tmpVal;
        reg_field_get(unit, CYPRESS_HSA_DATA17r, CYPRESS_PKTLENf, &tmpVal, &value);
        hsa->r8390.pktlen = tmpVal;
    }
    else
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        /* HSA_DATA0 */
        reg_read(unit, MAPLE_HSA_DATA0r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ORG_ITPID_IDXf, &tmpVal, &value);
        hsa->r8380.org_itpid_idx = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ORG_OTPID_IDXf, &tmpVal, &value);
        hsa->r8380.org_otpid_idx = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_HSA_ORG_OCFIf, &tmpVal, &value);
        hsa->r8380.org_ocfi = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ORG_OPRIf, &tmpVal, &value);
        hsa->r8380.org_opri = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ORG_OVIDf, &tmpVal, &value);
        hsa->r8380.org_ovid = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ORG_ITAG_IFf, &tmpVal, &value);
        hsa->r8380.org_itag_if = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ORG_OTAG_IFf, &tmpVal, &value);
        hsa->r8380.org_otag_if = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ALE_ITAG_STSf, &tmpVal, &value);
        hsa->r8380.ale_itag_sts = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ALE_OTAG_STSf, &tmpVal, &value);
        hsa->r8380.ale_otag_sts = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ALE_ITAG_HITf, &tmpVal, &value);
        hsa->r8380.ale_itag_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ALE_OTAG_HITf, &tmpVal, &value);
        hsa->r8380.ale_otag_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_ASTAGSTSf, &tmpVal, &value);
        hsa->r8380.astagsts = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA0r, MAPLE_CPU_TAG_IFf, &tmpVal, &value);
        hsa->r8380.cpu_tag_if = tmpVal;

        /* HSA_DATA1 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA1r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA1r, MAPLE_ALE_INT_PRIf, &tmpVal, &value);
        hsa->r8380.ale_int_pri = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA1r, MAPLE_FWD_VIDf, &tmpVal, &value);
        hsa->r8380.fwd_vid = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA1r, MAPLE_FWD_BASEf, &tmpVal, &value);
        hsa->r8380.fwd_base = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA1r, MAPLE_ORG_ICFIf, &tmpVal, &value);
        hsa->r8380.org_icfi = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA1r, MAPLE_ORG_IPRIf, &tmpVal, &value);
        hsa->r8380.org_ipri = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA1r, MAPLE_ORG_IVIDf, &tmpVal, &value);
        hsa->r8380.org_ivid = tmpVal;

        /* HSA_DATA2 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA2r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ALE_IVID_HITf, &tmpVal, &value);
        hsa->r8380.ale_ivid_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ALE_OVID_HITf, &tmpVal, &value);
        hsa->r8380.ale_ovid_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ACL_ITPID_IDXf, &tmpVal, &value);
        hsa->r8380.acl_itpid_idx = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ACL_ITPID_HITf, &tmpVal, &value);
        hsa->r8380.acl_itpid_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ACL_OTPID_IDXf, &tmpVal, &value);
        hsa->r8380.acl_otpid_idx = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ACL_OTPID_HITf, &tmpVal, &value);
        hsa->r8380.acl_otpid_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ALE_IVIDf, &tmpVal, &value);
        hsa->r8380.ale_ivid = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA2r, MAPLE_ALE_OVIDf, &tmpVal, &value);
        hsa->r8380.ale_ovid = tmpVal;

        /* HSA_DATA3 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA3r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_IPV6f, &tmpVal, &value);
        hsa->r8380.ipv6 = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_IPV4f, &tmpVal, &value);
        hsa->r8380.ipv4 = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_PPPOEf, &tmpVal, &value);
        hsa->r8380.pppoe = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_RFC1042f, &tmpVal, &value);
        hsa->r8380.rfc1042 = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_ETAGf, &tmpVal, &value);
        hsa->r8380.etag = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_ACL_HITf, &tmpVal, &value);
        hsa->r8380.acl_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_ACL_IDX_9_0f, &tmpVal, &value);
        hsa->r8380.acl_idx_9_0 = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_ACL_IDX_10f, &tmpVal, &value);
        hsa->r8380.acl_idx_10 = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_MAC_CSTf, &tmpVal, &value);
        hsa->r8380.mac_cst = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_ATK_HITf, &tmpVal, &value);
        hsa->r8380.atk_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_ATK_TYPEf, &tmpVal, &value);
        hsa->r8380.atk_type = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_NEW_SAf, &tmpVal, &value);
        hsa->r8380.new_sa = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_STC_L2_PMVf, &tmpVal, &value);
        hsa->r8380.l2_pmv = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA3r, MAPLE_REASONf, &tmpVal, &value);
        hsa->r8380.reason = tmpVal;

        /* HSA_DATA4 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA4r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_CPU_AS_PRIf, &tmpVal, &value);
        hsa->r8380.cpu_as_pri = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_ACL_AS_PRIf, &tmpVal, &value);
        hsa->r8380.acl_as_pri = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_DA_LK_HITf, &tmpVal, &value);
        hsa->r8380.da_lk_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_IS_EAV_Bf, &tmpVal, &value);
        hsa->r8380.is_eav_b = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_IS_EAV_Af, &tmpVal, &value);
        hsa->r8380.is_eav_a = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_DPCf, &tmpVal, &value);
        hsa->r8380.dpc = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_SPf, &tmpVal, &value);
        hsa->r8380.sp = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_L3_ROUTINGf, &tmpVal, &value);
        hsa->r8380.l3_routing = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_L3_ROUTING_IDXf, &tmpVal, &value);
        hsa->r8380.l3_routing_idx = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA4r, MAPLE_ACL_RMKf, &tmpVal, &value);
        hsa->r8380.acl_rmk = tmpVal;

        /* HSA_DATA5 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA5r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA5r, MAPLE_RSPAN_RMf, &tmpVal, &value);
        hsa->r8380.rspan_rm = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA5r, MAPLE_MIR_NOR_FWDf, &tmpVal, &value);
        hsa->r8380.mir_nor_fwd = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA5r, MAPLE_MIR4_INFOf, &tmpVal, &value);
        hsa->r8380.mir4_info = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA5r, MAPLE_MIR3_INFOf, &tmpVal, &value);
        hsa->r8380.mir3_info = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA5r, MAPLE_MIR2_INFOf, &tmpVal, &value);
        hsa->r8380.mir2_info = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA5r, MAPLE_MIR1_INFOf, &tmpVal, &value);
        hsa->r8380.mir1_info = tmpVal;

        /* HSA_DATA6 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA6r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA6r, MAPLE_INBW_PKTLENf, &tmpVal, &value);
        hsa->r8380.inbw_pktlen = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA6r, MAPLE_PKTLENf, &tmpVal, &value);
        hsa->r8380.pktlen = tmpVal;


        /* HSA_DATA7 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA7r, &value);  /* FIX ME */
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_CTAG_HITf, &tmpVal, &value);
        hsa->r8380.ale_ctag_hit = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_RXQIDf, &tmpVal, &value);
        hsa->r8380.rxqid = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_TXQID_CPUf, &tmpVal, &value);
        hsa->r8380.txqid_cpu = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_TXQID_NRMf, &tmpVal, &value);
        hsa->r8380.txqid_nrm = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_DP_BF_RXQf, &tmpVal, &value);
        hsa->r8380.dp_bf_rxq = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_LST_DSCf, &tmpVal, &value);
        hsa->r8380.lst_dsc = tmpVal;
        reg_field_get(unit, MAPLE_HSA_DATA7r, MAPLE_FST_DSCf, &tmpVal, &value);
        hsa->r8380.fst_dsc = tmpVal;

        /* HSA_DATA8 */
        value = 0;
        reg_read(unit, MAPLE_HSA_DATA8r, &value);
        reg_field_get(unit, MAPLE_HSA_DATA8r, MAPLE_DPMf, &tmpVal, &value);
        hsa->r8380.dpm = tmpVal;
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        /* HSA_DATA0 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA0r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_IPV4_PKTf, &tmpVal, &value);
        hsa->r9300.ipv4_pkt= tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_IPV6_PKTf, &tmpVal, &value);
        hsa->r9300.ipv6_pkt= tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_L3_ERR_PKTf, &tmpVal, &value);
        hsa->r9300.l3_err_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_L2_ERR_PKTf, &tmpVal, &value);
        hsa->r9300.l2_err_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_SPHYf, &tmpVal, &value);
        hsa->r9300.sphy= tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_STK_SUNITf, &tmpVal, &value);
        hsa->r9300.stk_src_unit = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_STK_SPAf, &tmpVal, &value);
        hsa->r9300.stk_spa = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_STK_SPA_IS_TRKf, &tmpVal, &value);
        hsa->r9300.stk_spa_is_trk = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_STK_SPA_TRK_IDf, &tmpVal, &value);
        hsa->r9300.stk_spa_trk_id = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_RFC1042f, &tmpVal, &value);
        hsa->r9300.rfc1042 = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_PPPOE_IFf, &tmpVal, &value);
        hsa->r9300.pppoe_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_LLC_OTHERf, &tmpVal, &value);
        hsa->r9300.llc_other = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_ORG_ITAG_IFf, &tmpVal, &value);
        hsa->r9300.org_itag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_ORG_OTAG_IFf, &tmpVal, &value);
        hsa->r9300.org_otag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA0r, LONGAN_CTAG_IFf, &tmpVal, &value);
        hsa->r9300.ctag_if = tmpVal;

        /* HSA_DATA1 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA1r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA1r, LONGAN_ETAG_IFf, &tmpVal, &value);
        hsa->r9300.etag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA1r, LONGAN_ORG_ITAG_CONTENTf, &tmpVal, &value);
        hsa->r9300.org_itag_pri = (tmpVal >> 13) & 0x7;
        hsa->r9300.org_itag_cfi = (tmpVal >> 12) & 0x1;
        hsa->r9300.org_itag_vid = tmpVal & 0xFFF;

        /* HSA_DATA2 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA2r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA2r, LONGAN_ORG_OTAG_CONTENTf, &tmpVal, &value);
        hsa->r9300.org_otag_pri = (tmpVal >> 13) & 0x7;
        hsa->r9300.org_otag_cfi = (tmpVal >> 12) & 0x1;
        hsa->r9300.org_otag_vid = tmpVal & 0xFFF;
        reg_field_get(unit, LONGAN_HSA_DATA2r, LONGAN_ORG_ITPID_IDXf, &tmpVal, &value);
        hsa->r9300.org_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA2r, LONGAN_ORG_OTPID_IDXf, &tmpVal, &value);
        hsa->r9300.org_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA2r, LONGAN_FWD_VIDf, &tmpVal, &value);
        hsa->r9300.fwd_vid = tmpVal;

        /* HSA_DATA3 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA3r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_FWD_BASEf, &tmpVal, &value);
        hsa->r9300.fwd_base = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_DPf, &tmpVal, &value);
        hsa->r9300.dp = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_OAM_SWAP_MACf, &tmpVal, &value);
        hsa->r9300.oam_swap_mac = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_L3_DA_IFf, &tmpVal, &value);
        hsa->r9300.l3_da_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_L3_SA_IFf, &tmpVal, &value);
        hsa->r9300.l3_sa_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_L3_DA_IDXf, &tmpVal, &value);
        hsa->r9300.l3_da_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_L3_SA_IDXf, &tmpVal, &value);
        hsa->r9300.l3_sa_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_CPU_FMTf, &tmpVal, &value);
        hsa->r9300.cpu_fmt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_CPU_FMT_ENf, &tmpVal, &value);
        hsa->r9300.cpu_fmt_en = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_ROUTED_PKTf, &tmpVal, &value);
        hsa->r9300.routed_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_INVALID_SAf, &tmpVal, &value);
        hsa->r9300.invalid_sa = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_L2_HASH_FULLf, &tmpVal, &value);
        hsa->r9300.l2_hash_full = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_TTL_DECf, &tmpVal, &value);
        hsa->r9300.ttl_dec = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_N_1_OVID_ASf, &tmpVal, &value);
        hsa->r9300.n_1_ovid_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_N_1_IVID_ASf, &tmpVal, &value);
        hsa->r9300.n_1_ivid_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA3r, LONGAN_EACL_OVID_ASf, &tmpVal, &value);
        hsa->r9300.eacl_ovid_as = tmpVal;

        /* HSA_DATA4 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA4r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA4r, LONGAN_EACL_IVID_ASf, &tmpVal, &value);
        hsa->r9300.eacl_ivid_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA4r, LONGAN_ALE_ITAG_CONTENTf, &tmpVal, &value);
        hsa->r9300.ale_itag_pri = (tmpVal >> 13) & 0x7;
        hsa->r9300.ale_itag_cfi = (tmpVal >> 12) & 0x1;
        hsa->r9300.ale_itag_vid = tmpVal & 0xFFF;

        /* HSA_DATA5 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA5r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_ALE_OTAG_CONTENTf, &tmpVal, &value);
        hsa->r9300.ale_otag_pri = (tmpVal >> 13) & 0x7;
        hsa->r9300.ale_otag_cfi = (tmpVal >> 12) & 0x1;
        hsa->r9300.ale_otag_vid = tmpVal & 0xFFF;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_ITPID_IDXf, &tmpVal, &value);
        hsa->r9300.itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_OTPID_IDXf, &tmpVal, &value);
        hsa->r9300.otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_IACL_ITPID_ASf, &tmpVal, &value);
        hsa->r9300.iacl_itpid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_IACL_OTPID_ASf, &tmpVal, &value);
        hsa->r9300.iacl_otpid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_EACL_ITPID_ASf, &tmpVal, &value);
        hsa->r9300.eacl_itpid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_EACL_OTPID_ASf, &tmpVal, &value);
        hsa->r9300.eacl_otpid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_ALE_OTAG_STATUSf, &tmpVal, &value);
        hsa->r9300.ale_otag_status = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_ALE_ITAG_STATUSf, &tmpVal, &value);
        hsa->r9300.ale_itag_status = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_IACL_OTAG_STATUS_ASf, &tmpVal, &value);
        hsa->r9300.iacl_otag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_IACL_ITAG_STATUS_ASf, &tmpVal, &value);
        hsa->r9300.iacl_itag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_EACL_OTAG_STATUS_ASf, &tmpVal, &value);
        hsa->r9300.eacl_otag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA5r, LONGAN_EACL_ITAG_STATUS_ASf, &tmpVal, &value);
        hsa->r9300.eacl_itag_status_as = tmpVal;

        /* HSA_DATA6 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA6r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_CPU_TAGSTS_ASf, &tmpVal, &value);
        hsa->r9300.cpu_tagsts_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_IACL_IPRI_ASf, &tmpVal, &value);
        hsa->r9300.iacl_ipri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_IACL_OPRI_ASf, &tmpVal, &value);
        hsa->r9300.iacl_opri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_EACL_IPRI_ASf, &tmpVal, &value);
        hsa->r9300.eacl_ipri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_EACL_OPRI_ASf, &tmpVal, &value);
        hsa->r9300.eacl_opri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_INT_PRIf, &tmpVal, &value);
        hsa->r9300.int_pri = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_INT_DSCPf, &tmpVal, &value);
        hsa->r9300.new_dscp = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_INT_DSCP_HITf, &tmpVal, &value);
        hsa->r9300.acl_rmk_dscp = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_REASONf, &tmpVal, &value);
        hsa->r9300.reason = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_IS_EAV_Bf, &tmpVal, &value);
        hsa->r9300.is_eav_b = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA6r, LONGAN_IS_EAV_Af, &tmpVal, &value);
        hsa->r9300.is_eav_a = tmpVal;

        /* HSA_DATA7 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA7r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA7r, LONGAN_ACL_IDXf, &tmpVal, &value);
        hsa->r9300.acl_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA7r, LONGAN_ACL_HITf, &tmpVal, &value);
        hsa->r9300.acl_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA7r, LONGAN_ATK_TYPEf, &tmpVal, &value);
        hsa->r9300.atk_type = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA7r, LONGAN_L2_ORG_TRK_IFf, &tmpVal, &value);
        hsa->r9300.l2_org_trk_if = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA7r, LONGAN_L2_ORG_PORTf, &tmpVal, &value);
        hsa->r9300.l2_org_port = tmpVal;

        /* HSA_DATA8 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA8r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_L2_STTC_PMVf, &tmpVal, &value);
        hsa->r9300.l2_sttc_pmv = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_L2_DYN_PMVf, &tmpVal, &value);
        hsa->r9300.l2_dyn_pmv = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_PMV_FORBIDf, &tmpVal, &value);
        hsa->r9300.pmv_forbid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_L2_MAC_CSTf, &tmpVal, &value);
        hsa->r9300.l2_mac_cst = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_NEW_SAf, &tmpVal, &value);
        hsa->r9300.new_sa = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_TRK_HASHf, &tmpVal, &value);
        hsa->r9300.trk_hash = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_VB_ISO_MBRf, &tmpVal, &value);
        hsa->r9300.vb_iso_mbr = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_SA_LRNf, &tmpVal, &value);
        hsa->r9300.sa_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_FWD_TYPEf, &tmpVal, &value);
        hsa->r9300.fwd_type = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_DP_FMTf, &tmpVal, &value);
        hsa->r9300.dp_fmt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_DP_INFOf, &tmpVal, &value);
        hsa->r9300.dp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA8r, LONGAN_CTX_FWD_TYPEf, &tmpVal, &value);
        hsa->r9300.ctx_fwd_type = tmpVal;

        /* HSA_DATA9 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA9r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA9r, LONGAN_CTX_DEV_IDf, &tmpVal, &value);
        hsa->r9300.ctx_unit_id = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA9r, LONGAN_CTX_DPM_0_27f, &tmpVal, &value);
        hsa->r9300.ctx_dpm.bits[0] = tmpVal;

        /* HSA_DATA10 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA10r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA10r, LONGAN_CTX_DPM_28_55f, &tmpVal, &value);
        hsa->r9300.ctx_dpm.bits[0] = hsa->r9300.ctx_dpm.bits[0] | (tmpVal & 0xf) << 28;
        hsa->r9300.ctx_dpm.bits[1] = (tmpVal >> 4) & 0xffffff;

        /* HSA_DATA11 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA11r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_SP_FLTR_ENf, &tmpVal, &value);
        hsa->r9300.ctx_sp_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_SPN_IS_TRKf, &tmpVal, &value);
        hsa->r9300.ctx_sp_is_trk = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_SPNf, &tmpVal, &value);
        hsa->r9300.ctx_spn = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_FWD_VID_ENf, &tmpVal, &value);
        hsa->r9300.ctx_fvid_en = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_SRC_FLTR_ENf, &tmpVal, &value);
        hsa->r9300.sp_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_AS_QIDf, &tmpVal, &value);
        hsa->r9300.ctx_qid_as = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_QIDf, &tmpVal, &value);
        hsa->r9300.ctx_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_BP_VLAN_EGRf, &tmpVal, &value);
        hsa->r9300.ctx_bp_vlan_egr = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_BP_STP_EGRf, &tmpVal, &value);
        hsa->r9300.ctx_bp_stp_egr = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_BP_FLTR_EGRf, &tmpVal, &value);
        hsa->r9300.ctx_bp_fltr_egr = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_CNGST_DROPf, &tmpVal, &value);
        hsa->r9300.ctx_cngst_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_ACL_ACTf, &tmpVal, &value);
        hsa->r9300.ctx_acl_act = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA11r, LONGAN_CTX_L3_ACTf, &tmpVal, &value);
        hsa->r9300.ctx_l3_act = tmpVal;

        /* HSA_DATA12 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA12r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_META_DATAf, &tmpVal, &value);
        hsa->r9300.meta_data = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_LB_TTLf, &tmpVal, &value);
        hsa->r9300.lb_ttl = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_DA_HITf, &tmpVal, &value);
        hsa->r9300.da_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_STK_MIRf, &tmpVal, &value);
        hsa->r9300.stk_mir = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_IP_RSVD_INVERTf, &tmpVal, &value);
        hsa->r9300.ip_rsvd_invert = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_LB_PKTf, &tmpVal, &value);
        hsa->r9300.lb_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_TXQID_CPUf, &tmpVal, &value);
        hsa->r9300.txqid_cpu.qid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA12r, LONGAN_NOR_CPU_SELf, &tmpVal, &value);
        hsa->r9300.nor_cpu_sel = tmpVal;

        /* HSA_DATA13 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA13r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA13r, LONGAN_IPMC_PKTf, &tmpVal, &value);
        hsa->r9300.ipmc_pkt= tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA13r, LONGAN_IPMC_QIDf, &tmpVal, &value);
        hsa->r9300.ipmc_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA13r, LONGAN_LST_DSCf, &tmpVal, &value);
        hsa->r9300.lst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA13r, LONGAN_PKT_LENf, &tmpVal, &value);
        hsa->r9300.pkt_len = tmpVal;

        /* HSA_DATA14 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA14r, &value);

        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_PAGE_CNTf, &tmpVal, &value);
        hsa->r9300.page_cnt = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_DPCf, &tmpVal, &value);
        hsa->r9300.dpc = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_DY_GASPf, &tmpVal, &value);
        hsa->r9300.dy_gasp= tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_MIR_NORMAL_FWDf, &tmpVal, &value);
        hsa->r9300.mir_normal_fwd = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_MIR0_INFOf, &tmpVal, &value);
        hsa->r9300.mir0_info = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_MIR1_INFOf, &tmpVal, &value);
        hsa->r9300.mir1_info = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA14r, LONGAN_MIR2_INFOf, &tmpVal, &value);
        hsa->r9300.mir2_info = tmpVal;

        /* HSA_DATA15 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA15r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA15r, LONGAN_MIR3_INFOf, &tmpVal, &value);
        hsa->r9300.mir3_info = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA15r, LONGAN_STK_CPU_FWDf, &tmpVal, &value);
        hsa->r9300.cpu_fwd_type = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA15r, LONGAN_MIRING_PORT_0f, &tmpVal, &value);
        hsa->r9300.miring_port_0 = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA15r, LONGAN_MIRING_PORT_1f, &tmpVal, &value);
        hsa->r9300.miring_port_1 = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA15r, LONGAN_MIRING_PORT_2f, &tmpVal, &value);
        hsa->r9300.miring_port_2 = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA15r, LONGAN_MIRING_PORT_3f, &tmpVal, &value);
        hsa->r9300.miring_port_3 = tmpVal;

       /* HSA_DATA16 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA16r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_STK_NOR_FWDf, &tmpVal, &value);
        hsa->r9300.stk_nor_fwd = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_SFLOW_INFOf, &tmpVal, &value);
        hsa->r9300.sflow_info = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_SFLOW_SMPL_PORTf, &tmpVal, &value);
        hsa->r9300.sflow_smpl_port = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_REPLICATE_TYPEf, &tmpVal, &value);
        hsa->r9300.replicate_type = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_REPLICATE_NONEf, &tmpVal, &value);
        hsa->r9300.replicate_noe = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_REPLICATE_FST_CPf, &tmpVal, &value);
        hsa->r9300.replicate_fst_cp = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA16r, LONGAN_REPLICATE_LST_CPf, &tmpVal, &value);
        hsa->r9300.replicate_lst_cp = tmpVal;

        /* HSA_DATA17 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA17r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA17r, LONGAN_DPMf, &tmpVal, &value);
        hsa->r9300.dpm.bits[0] = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA17r, LONGAN_DPM_LBf, &tmpVal, &value);
        hsa->r9300.dpm_lb = tmpVal;

        /* HSA_DATA18/19/20 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA18r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA18r, LONGAN_TXQID_NORMAL_31_0f, &tmpVal, &value);
        hsa->r9300.txqid_normal[0].qid = tmpVal & 0x7;
        hsa->r9300.txqid_normal[1].qid = (tmpVal >> 3 )& 0x7;
        hsa->r9300.txqid_normal[2].qid= (tmpVal >> 6 )& 0x7;
        hsa->r9300.txqid_normal[3].qid = (tmpVal >> 9 )& 0x7;
        hsa->r9300.txqid_normal[4].qid = (tmpVal >> 12 )& 0x7;
        hsa->r9300.txqid_normal[5].qid = (tmpVal >> 15 )& 0x7;
        hsa->r9300.txqid_normal[6].qid = (tmpVal >> 18 )& 0x7;
        hsa->r9300.txqid_normal[7].qid = (tmpVal >> 21 )& 0x7;
        hsa->r9300.txqid_normal[8].qid = (tmpVal >> 24 )& 0x7;
        hsa->r9300.txqid_normal[9].qid = (tmpVal >> 27 )& 0x7;
        hsa->r9300.txqid_normal[10].qid = (tmpVal >> 30 )& 0x3;
        reg_read(unit, LONGAN_HSA_DATA19r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA19r, LONGAN_TXQID_NORMAL_63_32f, &tmpVal, &value);
        hsa->r9300.txqid_normal[10].qid = hsa->r9300.txqid_normal[10].qid | ((tmpVal & 0x1) << 2);
        hsa->r9300.txqid_normal[11].qid = (tmpVal >> 1 )& 0x7;
        hsa->r9300.txqid_normal[12].qid = (tmpVal >> 4 )& 0x7;
        hsa->r9300.txqid_normal[13].qid = (tmpVal >> 7 )& 0x7;
        hsa->r9300.txqid_normal[14].qid = (tmpVal >> 10 )& 0x7;
        hsa->r9300.txqid_normal[15].qid = (tmpVal >> 13 )& 0x7;
        hsa->r9300.txqid_normal[16].qid = (tmpVal >> 16 )& 0x7;
        hsa->r9300.txqid_normal[17].qid = (tmpVal >> 19 )& 0x7;
        hsa->r9300.txqid_normal[18].qid = (tmpVal >> 22 )& 0x7;
        hsa->r9300.txqid_normal[19].qid = (tmpVal >> 25 )& 0x7;
        hsa->r9300.txqid_normal[20].qid = (tmpVal >> 28 )& 0x7;
        hsa->r9300.txqid_normal[21].qid = (tmpVal >> 31 )& 0x1;
        reg_read(unit, LONGAN_HSA_DATA20r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA20r, LONGAN_TXQID_NORMAL_71_64f, &tmpVal, &value);
        hsa->r9300.txqid_normal[21].qid = hsa->r9300.txqid_normal[21].qid | ((tmpVal & 0x3) << 1);
        hsa->r9300.txqid_normal[22].qid = (tmpVal >> 2 )& 0x7;
        hsa->r9300.txqid_normal[23].qid = (tmpVal >> 5 )& 0x7;
        reg_field_get(unit, LONGAN_HSA_DATA20r, LONGAN_TXQID_STK_PORTf, &tmpVal, &value);
        hsa->r9300.txqid_stk[0].qid = tmpVal & 0xf;
        hsa->r9300.txqid_stk[1].qid = (tmpVal >> 4) & 0xf;
        hsa->r9300.txqid_stk[2].qid = (tmpVal >> 8) & 0xf;
        hsa->r9300.txqid_stk[3].qid = (tmpVal >> 12) & 0xf;

        /* HSA_DATA21 */
        value = 0;
        reg_read(unit, LONGAN_HSA_DATA21r, &value);
        reg_field_get(unit, LONGAN_HSA_DATA21r, LONGAN_FST_DSCf, &tmpVal, &value);
        hsa->r9300.fst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSA_DATA21r, LONGAN_DROP_RSNf, &tmpVal, &value);
        hsa->r9300.drop_rsn = tmpVal;
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        uint32 *val;

        val = osal_alloc(256*sizeof(uint32));
        if (NULL == val)
        {
            osal_printf("osal_alloc  failed!!\n");
            return RT_ERR_FAILED;
        }

        reg_read(unit, MANGO_HSA_DATAr, val);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_HSB_EADRf, &tmpVal, val);
        hsa->r9310.hsb_eadr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PKT_LENf, &tmpVal, val);
        hsa->r9310.pkt_len = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PG_CNTf, &tmpVal, val);
        hsa->r9310.pg_cnt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STACK_IFf, &tmpVal, val);
        hsa->r9310.stack_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_TX_IFf, &tmpVal, val);
        hsa->r9310.cpu_tx_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_IFf, &tmpVal, val);
        hsa->r9310.of_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_PIPELINEf, &tmpVal, val);
        hsa->r9310.of_pipeline = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STACK_FMTf, &tmpVal, val);
        hsa->r9310.Stack_fmt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_IFf, &tmpVal, val);
        hsa->r9310.ori_itag_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_IFf, &tmpVal, val);
        hsa->r9310.ori_otag_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_VIDf, &tmpVal, val);
        hsa->r9310.ori_itag_vid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_CFIf, &tmpVal, val);
        hsa->r9310.ori_itag_cfi = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_PRIf, &tmpVal, val);
        hsa->r9310.ori_itag_pri = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_TPIDf, &tmpVal, val);
        hsa->r9310.ori_itag_tpid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_VIDf, &tmpVal, val);
        hsa->r9310.ori_otag_vid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_CFIf, &tmpVal, val);
        hsa->r9310.ori_otag_cfi = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_PRIf, &tmpVal, val);
        hsa->r9310.ori_otag_pri = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_TPIDf, &tmpVal, val);
        hsa->r9310.ori_otag_tpid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CLASS_Af, &tmpVal, val);
        hsa->r9310.class_a = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CLASS_Bf, &tmpVal, val);
        hsa->r9310.class_b = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PPPOE_IFf, &tmpVal, val);
        hsa->r9310.pppoe_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FRAME_TYPEf, &tmpVal, val);
        hsa->r9310.frame_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EVLAN_IFf, &tmpVal, val);
        hsa->r9310.evlan_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IPV4_IFf, &tmpVal, val);
        hsa->r9310.ipv4_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IPV6_IFf, &tmpVal, val);
        hsa->r9310.ipv6_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORG_OMPLS_IFf, &tmpVal, val);
        hsa->r9310.org_ompls_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORG_IMPLS_IFf, &tmpVal, val);
        hsa->r9310.org_impls_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PARSER_CANT_HANDLEf, &tmpVal, val);
        hsa->r9310.parser_cant_handle = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_HOPBYHOP_ERRf, &tmpVal, val);
        hsa->r9310.IP6_EH_HOPBYHOP_ERR = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_HOPBYHOP_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_HOPBYHOP_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_ROUTE_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_ROUTE_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_FRAGMENT_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_FRAGMENT_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_ESP_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_ESP_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_AUTH_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_AUTH_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_DESTINATION_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_DESTINATION_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_MOBILITY_EXISTf, &tmpVal, val);
        hsa->r9310.IP6_EH_MOBILITY_EXIST = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_REPEATf, &tmpVal, val);
        hsa->r9310.IP6_EH_REPEAT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_PROTf, &tmpVal, val);
        hsa->r9310.ip_prot = tmpVal;
#if 0
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_OFFSET_ZEROf, &tmpVal, val);
        hsa->r9310.ip_offset_zero = tmpVal;
#endif
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_FLAG_SETf, &tmpVal, val);
        hsa->r9310.ip_flag_set = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L4_HDR_RDYf, &tmpVal, val);
        hsa->r9310.l4_hdr_rdy = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L4_OFFSETf, &tmpVal, val);
        hsa->r9310.l4_offset = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_ERR_PKTf, &tmpVal, val);
        hsa->r9310.l2_err_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L3_ERR_PKTf, &tmpVal, val);
        hsa->r9310.l3_err_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L3_OFFSETf, &tmpVal, val);
        hsa->r9310.L3_offset = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IN_SPNf, &tmpVal, val);
        hsa->r9310.in_spn = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SPNf, &tmpVal, val);
        hsa->r9310.spn = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SP_TRKf, &tmpVal, val);
        hsa->r9310.sp_trk = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SP_IS_TRKf, &tmpVal, val);
        hsa->r9310.sp_is_trk = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_HITf, &tmpVal, val);
        hsa->r9310.int_itag_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_HITf, &tmpVal, val);
        hsa->r9310.int_otag_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_CRCf, &tmpVal, val);
        hsa->r9310.l2_crc = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DA_HITf, &tmpVal, val);
        hsa->r9310.da_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_VID_ENf, &tmpVal, val);
        hsa->r9310.fwd_vid_en = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_RMA_PKTf, &tmpVal, val);
        hsa->r9310.rma_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_META_DATAf, &tmpVal, val);
        hsa->r9310.meta_data = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TRK_HASHf, &tmpVal, val);
        hsa->r9310.trk_hash = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DPf, &tmpVal, val);
        hsa->r9310.dp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INTERNAL_PRIORITYf, &tmpVal, val);
        hsa->r9310.internal_Priority = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IGR_L3_INTF_IDf, &tmpVal, val);
        hsa->r9310.l3_intf_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TT_IDXf, &tmpVal, val);
        hsa->r9310.tt_idx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TT_HITf, &tmpVal, val);
        hsa->r9310.tt_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SELf, &tmpVal, val);
        hsa->r9310.hsa_sel = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SADRf, &tmpVal, val);
        hsa->r9310.hsa_sadr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_HSB_SADRf, &tmpVal, val);
        hsa->r9310.hsb_sadr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DPM_57_32f, &tmpVal, val);
        hsa->r9310.dpm.bits[1] = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DPM_31_0f, &tmpVal, val);
        hsa->r9310.dpm.bits[0] = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CP_CNTf, &tmpVal, val);
        hsa->r9310.cp_cnt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STK_CPU_FWDf, &tmpVal, val);
        hsa->r9310.stk_cpu_fwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STK_NM_FWDf, &tmpVal, val);
        hsa->r9310.stk_nm_fwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MIRRORING_INFOf, &tmpVal, val);
        hsa->r9310.mirroring_info = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MIRRORED_INFOf, &tmpVal, val);
        hsa->r9310.mirrored_info = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MIRR_NFWDf, &tmpVal, val);
        hsa->r9310.mirr_nfwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_LOCAL_CPU_FWDf, &tmpVal, val);
        hsa->r9310.local_cpu_fwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SFLOW_MIRRORINGf, &tmpVal, val);
        hsa->r9310.sflow_mirroring = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SFLOW_MIRROREDf, &tmpVal, val);
        hsa->r9310.sflow_mirrored = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DM_PKTf, &tmpVal, val);
        hsa->r9310.dm_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DG_PKTf, &tmpVal, val);
        hsa->r9310.dg_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TX_MIR_CANCELf, &tmpVal, val);
        hsa->r9310.tx_mir_cancel = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_VB_ISO_MBRf, &tmpVal, val);
        hsa->r9310.vb_iso_mbr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_LEARN_ENf, &tmpVal, val);
        hsa->r9310.learn_en = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_VID_SELf, &tmpVal, val);
        hsa->r9310.fwd_vid_sel = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_TYPEf, &tmpVal, val);
        hsa->r9310.fwd_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_RVIDf, &tmpVal, val);
        hsa->r9310.RVID = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_LOCAL_REASONf, &tmpVal, val);
        hsa->r9310.local_reason = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_REMOTE_REASONf, &tmpVal, val);
        hsa->r9310.remote_reason = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_PORTf, &tmpVal, val);
        hsa->r9310.fwd_port = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_TX_FWD_TYPEf, &tmpVal, val);
        hsa->r9310.cpu_tx_fwd_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CTX_DPM_55_32f, &tmpVal, val);
        hsa->r9310.ctx_dpm_55_32 = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CTX_DPM_31_0f, &tmpVal, val);
        hsa->r9310.ctx_dpm_31_0 = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SW_DEVf, &tmpVal, val);
        hsa->r9310.sw_unit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SPF_SPNf, &tmpVal, val);
        hsa->r9310.spf_spn = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SPF_IS_TRKf, &tmpVal, val);
        hsa->r9310.spf_is_trk = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SRC_FILTER_ENf, &tmpVal, val);
        hsa->r9310.src_filter_en = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_AS_QIDf, &tmpVal, val);
        hsa->r9310.as_qid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ALE_AS_TAGSTSf, &tmpVal, val);
        hsa->r9310.ale_as_tagsts = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BP_VLAN_EGRf, &tmpVal, val);
        hsa->r9310.bp_vlan_egr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BP_STPf, &tmpVal, val);
        hsa->r9310.bp_stp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BP_FLTRf, &tmpVal, val);
        hsa->r9310.bp_fltr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CNGST_DROPf, &tmpVal, val);
        hsa->r9310.cngst_drop = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_ACTf, &tmpVal, val);
        hsa->r9310.acl_act = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STK_MIR_HITf, &tmpVal, val);
        hsa->r9310.stk_mir_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TS_REQf, &tmpVal, val);
        hsa->r9310.ts_req = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_ENCAP_REQf, &tmpVal, val);
        hsa->r9310.mpls_encap_req = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TS_INDEXf, &tmpVal, val);
        hsa->r9310.ts_index = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BSSID_IDXf, &tmpVal, val);
        hsa->r9310.bssid_idx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_ENCAP_IDXf, &tmpVal, val);
        hsa->r9310.mpls_encap_idx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_DECAP_REQf, &tmpVal, val);
        hsa->r9310.mpls_decap_req = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_HDR_TTLf, &tmpVal, val);
        hsa->r9310.ip_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_IPRI_HITf, &tmpVal, val);
        hsa->r9310.HSA_N21_IPRI_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_DSCPf, &tmpVal, val);
        hsa->r9310.HSA_INT_DSCP = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_DSCP_HITf, &tmpVal, val);
        hsa->r9310.HSA_INT_DSCP_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_DECAP_STAf, &tmpVal, val);
        hsa->r9310.mpls_decap_sta = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_IPRIf, &tmpVal, val);
        hsa->r9310.HSA_N21_IPRI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_OPRIf, &tmpVal, val);
        hsa->r9310.HSA_N21_OPRI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_OPRI_HITf, &tmpVal, val);
        hsa->r9310.HSA_N21_OPRI_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_TCf, &tmpVal, val);
        hsa->r9310.mpls_tc = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_IFf, &tmpVal, val);
        hsa->r9310.HSA_INT_ITAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SURF_OVIDf, &tmpVal, val);
        hsa->r9310.SURF_OVID = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ITAG_PRIf, &tmpVal, val);
        hsa->r9310.HSA_ITAG_PRI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_IPRI_HITf, &tmpVal, val);
        hsa->r9310.HSA_EACL_IPRI_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_VIDf, &tmpVal, val);
        hsa->r9310.HSA_INT_OTAG_VID = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_IPRI_HITf, &tmpVal, val);
        hsa->r9310.HSA_IACL_IPRI_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SURF_IVIDf, &tmpVal, val);
        hsa->r9310.SURF_IVID = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OTAG_PRIf, &tmpVal, val);
        hsa->r9310.HSA_OTAG_PRI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_IVID_HITf, &tmpVal, val);
        hsa->r9310.EACL_IVID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_VIDf, &tmpVal, val);
        hsa->r9310.HSA_INT_ITAG_VID = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OPRI_HITf, &tmpVal, val);
        hsa->r9310.HSA_EACL_OPRI_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_OPRI_HITf, &tmpVal, val);
        hsa->r9310.HSA_IACL_OPRI_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_OTPID_IDXf, &tmpVal, val);
        hsa->r9310.ACL_OTPID_IDX = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_CFIf, &tmpVal, val);
        hsa->r9310.HSA_INT_OTAG_CFI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_CFIf, &tmpVal, val);
        hsa->r9310.HSA_INT_ITAG_CFI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_VRF_IDf, &tmpVal, val);
        hsa->r9310.vrf_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L3_MDF_REQf, &tmpVal, val);
        hsa->r9310.l3_mdf_req = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OAM_LBKf, &tmpVal, val);
        hsa->r9310.oam_lbk = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_IVID_HITf, &tmpVal, val);
        hsa->r9310.N21_IVID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_P_TYPEf, &tmpVal, val);
        hsa->r9310.fwd_p_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OTAG_HITf, &tmpVal, val);
        hsa->r9310.EACL_OTAG_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OTAG_IFf, &tmpVal, val);
        hsa->r9310.EACL_OTAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_IFf, &tmpVal, val);
        hsa->r9310.HSA_INT_OTAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_ITAG_HITf, &tmpVal, val);
        hsa->r9310.EACL_ITAG_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_ITAG_IFf, &tmpVal, val);
        hsa->r9310.EACL_ITAG_IF = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OVID_HITf, &tmpVal, val);
        hsa->r9310.EACL_OVID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_OVID_HITf, &tmpVal, val);
        hsa->r9310.N21_OVID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_OTPID_HITf, &tmpVal, val);
        hsa->r9310.IACL_OTPID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_ITPID_HITf, &tmpVal, val);
        hsa->r9310.IACL_ITPID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OTPID_HITf, &tmpVal, val);
        hsa->r9310.EACL_OTPID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_ITPID_HITf, &tmpVal, val);
        hsa->r9310.EACL_ITPID_HIT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_ITPID_IDXf, &tmpVal, val);
        hsa->r9310.ACL_ITPID_IDX = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PORT_DATAf, &tmpVal, val);
        hsa->r9310.port_data = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PORT_IS_TRKf, &tmpVal, val);
        hsa->r9310.port_is_trk = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INVALID_SAf, &tmpVal, val);
        hsa->r9310.invalid_sa = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_HASH_FULLf, &tmpVal, val);
        hsa->r9310.hash_full = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_DYN_PMVf, &tmpVal, val);
        hsa->r9310.l2_dyn_pmv = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_STTC_PMVf, &tmpVal, val);
        hsa->r9310.l2_sttc_pmv = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PMV_FORBIDf, &tmpVal, val);
        hsa->r9310.pmv_forbid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_NEW_SAf, &tmpVal, val);
        hsa->r9310.new_sa = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DM_RXIDXf, &tmpVal, val);
        hsa->r9310.dm_rxidx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MAC_CSTf, &tmpVal, val);
        hsa->r9310.mac_cst = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_IDXf, &tmpVal, val);
        hsa->r9310.acl_idx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ATK_TYPEf, &tmpVal, val);
        hsa->r9310.atk_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_RX_L2_ERRf, &tmpVal, val);
        hsa->r9310.cpu_rx_l2_err = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_RX_L3_ERRf, &tmpVal, val);
        hsa->r9310.cpu_rx_l3_err = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_RX_SFLOWf, &tmpVal, val);
        hsa->r9310.cpu_rx_sflow = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_TBL_IDf, &tmpVal, val);
        hsa->r9310.of_tbl_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_OF_HITf, &tmpVal, val);
        hsa->r9310.acl_of_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_MISS_TBL_IDf, &tmpVal, val);
        hsa->r9310.of_miss_tbl_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DMAC_47_32f, &tmpVal, val);
        hsa->r9310.dmac[0] = (tmpVal >> 8) & 0xFF;
        hsa->r9310.dmac[1] = (tmpVal >> 0) & 0xFF;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DMAC_31_0f, &tmpVal, val);
        hsa->r9310.dmac[2] = (tmpVal >> 24) & 0xFF;
        hsa->r9310.dmac[3] = (tmpVal >> 16) & 0xFF;
        hsa->r9310.dmac[4] = (tmpVal >> 8) & 0xFF;
        hsa->r9310.dmac[5] = (tmpVal >> 0) & 0xFF;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SMAC_47_16f, &tmpVal, val);
        hsa->r9310.smac[0] = (tmpVal >> 24) & 0xFF;
        hsa->r9310.smac[1] = (tmpVal >> 16) & 0xFF;
        hsa->r9310.smac[2] = (tmpVal >> 8) & 0xFF;
        hsa->r9310.smac[3] = (tmpVal >> 0) & 0xFF;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SMAC_15_0f, &tmpVal, val);
        hsa->r9310.smac[4] = (tmpVal >> 8) & 0xFF;
        hsa->r9310.smac[5] = (tmpVal >> 0) & 0xFF;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_9_0f, &tmpVal, val);
        hsa->r9310.qid[9].qid = (tmpVal >> 27) & 0x7;
        hsa->r9310.qid[8].qid = (tmpVal >> 24) & 0x7;
        hsa->r9310.qid[7].qid = (tmpVal >> 21) & 0x7;
        hsa->r9310.qid[6].qid = (tmpVal >> 18) & 0x7;
        hsa->r9310.qid[5].qid = (tmpVal >> 15) & 0x7;
        hsa->r9310.qid[4].qid = (tmpVal >> 12) & 0x7;
        hsa->r9310.qid[3].qid = (tmpVal >> 9) & 0x7;
        hsa->r9310.qid[2].qid = (tmpVal >> 6) & 0x7;
        hsa->r9310.qid[1].qid = (tmpVal >> 3) & 0x7;
        hsa->r9310.qid[0].qid = (tmpVal >> 0) & 0x7;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_19_10f, &tmpVal, val);
        hsa->r9310.qid[19].qid = (tmpVal >> 27) & 0x7;
        hsa->r9310.qid[18].qid = (tmpVal >> 24) & 0x7;
        hsa->r9310.qid[17].qid = (tmpVal >> 21) & 0x7;
        hsa->r9310.qid[16].qid = (tmpVal >> 18) & 0x7;
        hsa->r9310.qid[15].qid = (tmpVal >> 15) & 0x7;
        hsa->r9310.qid[14].qid = (tmpVal >> 12) & 0x7;
        hsa->r9310.qid[13].qid = (tmpVal >> 9) & 0x7;
        hsa->r9310.qid[12].qid = (tmpVal >> 6) & 0x7;
        hsa->r9310.qid[11].qid = (tmpVal >> 3) & 0x7;
        hsa->r9310.qid[10].qid = (tmpVal >> 0) & 0x7;

        for (i = 0; i < 57; i++)
        {
            if (i == 56)
            {
                reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_CPUf, &tmpVal, val);
                hsa->r9310.cpu_qid.qid = tmpVal;
            }
            else if ((52 <= i) && (i < 56))
            {
                reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_55_52f, &tmpVal, val);
                shift_bit = (i - 52) * 4;
                hsa->r9310.stack_qid[i - 52].qid = (tmpVal >> shift_bit) & 0xf;
            }
            else
            {
                if (i < 10)
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_9_0f, &tmpVal, val);
                else if ((10 <= i) && (i < 20))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_19_10f, &tmpVal, val);
                else if ((20 <= i) && (i < 30))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_29_20f, &tmpVal, val);
                else if ((30 <= i) && (i < 40))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_39_30f, &tmpVal, val);
                else if ((40 <= i) && (i < 50))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_49_40f, &tmpVal, val);
                else if ((50 <= i) && (i < 52))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_51_50f, &tmpVal, val);
                shift_bit = (i % 10) * 3;
                hsa->r9310.qid[i].qid = (tmpVal >> shift_bit) & 0x7;
            }
        }

        /* BPE */
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_EST_MATCHf, &tmpVal, val);
        hsa->r9310.EST_MATCH = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_DST_PE_ECIDf, &tmpVal, val);
        hsa->r9310.DST_PE_ECID = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_PE_LU_MODEf, &tmpVal, val);
        hsa->r9310.PE_LU_MODE = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_PE_NSGf, &tmpVal, val);
        hsa->r9310.PE_NSG = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_INT_PE_ECID_EXTf, &tmpVal, val);
        hsa->r9310.INT_PE_ECID_EXT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ECID_EXTf, &tmpVal, val);
        hsa->r9310.ORI_PE_ECID_EXT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_IGR_ECID_EXTf, &tmpVal, val);
        hsa->r9310.ORI_PE_IGR_ECID_EXT = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ECID_BASEf, &tmpVal, val);
        hsa->r9310.ORI_PE_ECID_BASE = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ECID_GRPf, &tmpVal, val);
        hsa->r9310.ORI_PE_ECID_GRP = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ETAG_RSVDf, &tmpVal, val);
        hsa->r9310.ORI_PE_ETAG_RSVD = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_IGR_ECID_BASEf, &tmpVal, val);
        hsa->r9310.ORI_PE_IGR_ECID_BASE = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ETAG_DEIf, &tmpVal, val);
        hsa->r9310.ORI_PE_ETAG_DEI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ETAG_PRIf, &tmpVal, val);
        hsa->r9310.ORI_PE_ETAG_PRI = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_ORI_PE_ETAG_IFf, &tmpVal, val);
        hsa->r9310.ORI_PE_ETAG_IF = tmpVal;

        osal_free(val);
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _rtk_getHsa */

/* Function Name:
 *      hal_dumpHsa
 * Description:
 *      Dump hsa paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsa(uint32 unit)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
    uint32  i;
#endif
    hsa_param_t  hsa;
    hal_control_t *pInfo;

    if(RT_ERR_OK != _rtk_getHsa(unit, &hsa))
    {
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        osal_printf("\n===========================HSA===============================\n");

        osal_printf("page_cnt : 0x%x\t\t", hsa.r8390.page_cnt);
        osal_printf("acl_mpls_hit : 0x%x\t", hsa.r8390.acl_mpls_hit);
        osal_printf("acl_mpls_act : 0x%x\n", hsa.r8390.acl_mpls_act);

        osal_printf("mpls_index : 0x%x\t", hsa.r8390.mpls_index);
        osal_printf("astagsts : 0x%x\t\t", hsa.r8390.astagsts);
        osal_printf("ctag_dm : 0x%x\n", hsa.r8390.ctag_dm);

        osal_printf("ctag_dgpkt : 0x%x\t", hsa.r8390.ctag_dgpkt);
        osal_printf("tx_ptp_log : 0x%x\t", hsa.r8390.tx_ptp_log);
        osal_printf("tx_ptp_offload : 0x%x\n", hsa.r8390.tx_ptp_offload);

        osal_printf("new_sa : 0x%x\t\t", hsa.r8390.new_sa);
        osal_printf("mac_cst : 0x%x\t\t", hsa.r8390.mac_cst);
        osal_printf("l2_pmv : 0x%x\n", hsa.r8390.l2_pmv);

        osal_printf("atk_type : 0x%x\t\t", hsa.r8390.atk_type);
        osal_printf("dm_rxidx : 0x%x\t\t", hsa.r8390.dm_rxidx);
        osal_printf("acl_hit : 0x%x\n", hsa.r8390.acl_hit);

        osal_printf("acl_id : 0x%x\t\t", hsa.r8390.acl_id);
        osal_printf("eav_class_b : 0x%x\t", hsa.r8390.eav_class_b);
        osal_printf("eav_class_a : 0x%x\n", hsa.r8390.eav_class_a);

        osal_printf("reason : 0x%x\t\t", hsa.r8390.reason);
        osal_printf("acl_tos : 0x%x\t\t", hsa.r8390.acl_tos);
        osal_printf("new_tos : 0x%x\n", hsa.r8390.new_tos);

        osal_printf("internal_pri : 0x%x\t", hsa.r8390.internal_pri);
        osal_printf("opri_act : 0x%x\t\t", hsa.r8390.opri_act);
        osal_printf("ipri_act : 0x%x\n", hsa.r8390.ipri_act);

        osal_printf("c2sc_pri : 0x%x\t\t", hsa.r8390.c2sc_pri);
        osal_printf("eacl_itag : 0x%x\t\t", hsa.r8390.eacl_itag);
        osal_printf("eacl_otag : 0x%x\n", hsa.r8390.eacl_otag);

        osal_printf("c2sc_itag : 0x%x\t\t", hsa.r8390.c2sc_itag);
        osal_printf("c2sc_otag : 0x%x\t\t", hsa.r8390.c2sc_otag);
        osal_printf("itag_status : 0x%x\n", hsa.r8390.itag_status);

        osal_printf("otag_status : 0x%x\t", hsa.r8390.otag_status);
        osal_printf("eacl_ivid : 0x%x\t\t", hsa.r8390.eacl_ivid);
        osal_printf("c2sc_ivid : 0x%x\n", hsa.r8390.c2sc_ivid);

        osal_printf("eacl_otpid : 0x%x\t", hsa.r8390.eacl_otpid);
        osal_printf("otpid_idx : 0x%x\t\t", hsa.r8390.otpid_idx);
        osal_printf("eacl_itpid : 0x%x\n", hsa.r8390.eacl_itpid);

        osal_printf("c2sc_itpid : 0x%x\t", hsa.r8390.c2sc_itpid);
        osal_printf("itpid_idx : 0x%x\t\t", hsa.r8390.itpid_idx);
        osal_printf("new_otag : 0x%x\n", hsa.r8390.new_otag);

        osal_printf("new_itag : 0x%x\t\t", hsa.r8390.new_itag);
        osal_printf("ale_ovid : 0x%x\t\t", hsa.r8390.ale_ovid);
        osal_printf("l3_route : 0x%x\n", hsa.r8390.l3_route);

        osal_printf("da_pid : 0x%x\t\t", hsa.r8390.da_pid);
        osal_printf("normal_fwd : 0x%x\t", hsa.r8390.normal_fwd);
        osal_printf("swap_mac : 0x%x\n", hsa.r8390.swap_mac);

        osal_printf("cpu_fwd : 0x%x\t\t", hsa.r8390.cpu_fwd);
        osal_printf("sflow_info : 0x%x\t", hsa.r8390.sflow_info);
        osal_printf("mirror_1_info : 0x%x\n", hsa.r8390.mirror_1_info);

        osal_printf("mirror_2_info : 0x%x\t", hsa.r8390.mirror_2_info);
        osal_printf("mirror_3_info : 0x%x\t", hsa.r8390.mirror_3_info);
        osal_printf("mirror_4_info : 0x%x\n", hsa.r8390.mirror_4_info);


        for (i = 0; i < 53; i++)
        {
            if (0 != ((i+1) % 3))
                osal_printf("qid[%2u] : 0x%x\t\t", i, hsa.r8390.qid[i].qid);
            else
                osal_printf("qid[%2u] : 0x%x\n", i, hsa.r8390.qid[i].qid);
        }
        osal_printf("\n");  /* end of QID */

        osal_printf("color : 0x%x\t\t", hsa.r8390.color);
        osal_printf("rvid_sel : 0x%x\t\t", hsa.r8390.rvid_sel);
        osal_printf("rvid : 0x%x\n", hsa.r8390.rvid);
        osal_printf("dpc : 0x%x\t\t", hsa.r8390.dpc);
        osal_printf("dpm[52:32] : 0x%x\t", hsa.r8390.dpm.bits[1]);
        osal_printf("dpm[31: 0] : 0x%x\n", hsa.r8390.dpm.bits[0]);
        osal_printf("endsc : 0x%x\t\t", hsa.r8390.endsc);
        osal_printf("bgdsc : 0x%x\t\t", hsa.r8390.bgdsc);
        osal_printf("stpid_idx : 0x%x\n", hsa.r8390.stpid_idx);

        osal_printf("ctpid_idx : 0x%x\t\t", hsa.r8390.ctpid_idx);
        osal_printf("org_otag : 0x%x\t\t", hsa.r8390.org_otag);
        osal_printf("org_itag : 0x%x\n", hsa.r8390.org_itag);

        osal_printf("ompls_if : 0x%x\t\t", hsa.r8390.ompls_if);
        osal_printf("impls_if : 0x%x\t\t", hsa.r8390.impls_if);
        osal_printf("org_etag_if : 0x%x\n", hsa.r8390.org_etag_if);

        osal_printf("org_cputag_if : 0x%x\t", hsa.r8390.org_cputag_if);
        osal_printf("org_otag_if : 0x%x\t", hsa.r8390.org_otag_if);
        osal_printf("org_itag_if : 0x%x\n", hsa.r8390.org_itag_if);

        osal_printf("ipv6 : 0x%x\t\t", hsa.r8390.ipv6);
        osal_printf("llc_other : 0x%x\t\t", hsa.r8390.llc_other);
        osal_printf("pppoe_if : 0x%x\n", hsa.r8390.pppoe_if);
        osal_printf("rfc1042 : 0x%x\t\t", hsa.r8390.rfc1042);
        osal_printf("spa : 0x%x\t\t", hsa.r8390.spa);
        osal_printf("errpkt : 0x%x\n", hsa.r8390.errpkt);
        osal_printf("ipv4 : 0x%x\t\t", hsa.r8390.ipv4);
        osal_printf("pktlen : 0x%x\n", hsa.r8390.pktlen);
        osal_printf("=============================================================\n");
    }
    else
#endif

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        osal_printf("\n===============================================  HSA  ==================================================\n");

        osal_printf("%-13s = 0x%-9x", "cpu_tag_if", hsa.r8380.cpu_tag_if);
        osal_printf("%-13s = 0x%-9x\n", "astagsts", hsa.r8380.astagsts);

        osal_printf("---------------------------------------------- ALE DECISION --------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "ale_otag_hit", hsa.r8380.ale_otag_hit);
        osal_printf("%-13s = 0x%-9x", "ale_otag_sts", hsa.r8380.ale_otag_sts);
        osal_printf("%-13s = 0x%-9x", "ale_ovid_hit", hsa.r8380.ale_ovid_hit);
        osal_printf("%-13s = 0x%-9x\n", "ale_ovid", hsa.r8380.ale_ovid);

        osal_printf("%-13s = 0x%-9x", "ale_itag_hit", hsa.r8380.ale_itag_hit);
        osal_printf("%-13s = 0x%-9x", "ale_itag_sts", hsa.r8380.ale_itag_sts);
        osal_printf("%-13s = 0x%-9x", "ale_ivid_hit", hsa.r8380.ale_ivid_hit);
        osal_printf("%-13s = 0x%-9x\n", "ale_ivid", hsa.r8380.ale_ivid);

        osal_printf("%-13s = 0x%-9x\n", "ale_ctag_hit", hsa.r8380.ale_ctag_hit);

        osal_printf("---------------------------------------------- ACL DECISION --------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "acl_otpid_hit", hsa.r8380.acl_otpid_hit);
        osal_printf("%-13s = 0x%-9x", "acl_otpid_idx", hsa.r8380.acl_otpid_idx);
        osal_printf("%-13s = 0x%-9x", "acl_itpid_hit", hsa.r8380.acl_itpid_hit);
        osal_printf("%-13s = 0x%-9x\n", "acl_itpid_idx", hsa.r8380.acl_itpid_idx);
        osal_printf("%-13s = 0x%-9x", "acl_rmk", hsa.r8380.acl_rmk);
        osal_printf("%-13s = 0x%-9x", "l3_routing", hsa.r8380.l3_routing);
        osal_printf("%-13s = 0x%-9x\n", "l3_routing_idx", hsa.r8380.l3_routing_idx);

        osal_printf("---------------------------------------------- ORG TAG INFO --------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "org_otag_if", hsa.r8380.org_otag_if);
        osal_printf("%-13s = 0x%-9x", "org_ovid", hsa.r8380.org_ovid);
        osal_printf("%-13s = 0x%-9x", "org_opri", hsa.r8380.org_opri);
        osal_printf("%-13s = 0x%-9x\n", "org_ocfi", hsa.r8380.org_ocfi);

        osal_printf("%-13s = 0x%-9x", "org_itag_if", hsa.r8380.org_itag_if);
        osal_printf("%-13s = 0x%-9x", "org_ivid", hsa.r8380.org_ivid);
        osal_printf("%-13s = 0x%-9x", "org_ipri", hsa.r8380.org_ipri);
        osal_printf("%-13s = 0x%-9x\n", "org_icfi", hsa.r8380.org_icfi);

        osal_printf("%-13s = 0x%-9x", "org_otpid_idx", hsa.r8380.org_otpid_idx);
        osal_printf("%-13s = 0x%-9x\n", "org_itpid_idx", hsa.r8380.org_itpid_idx);

        osal_printf("---------------------------------------------- PACKET INFO ---------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "rfc1042", hsa.r8380.rfc1042);
        osal_printf("%-13s = 0x%-9x", "pppoe", hsa.r8380.pppoe);
        osal_printf("%-13s = 0x%-9x", "ipv4", hsa.r8380.ipv4);
        osal_printf("%-13s = 0x%-9x\n", "ipv6", hsa.r8380.ipv6);

        osal_printf("%-13s = 0x%-9x", "etag", hsa.r8380.etag);
        osal_printf("%-13s = 0x%-9x", "sp", hsa.r8380.sp);
        osal_printf("%-13s = 0x%-9x", "dpm", hsa.r8380.dpm);
        osal_printf("%-13s = 0x%-9x\n", "pktlen", hsa.r8380.pktlen);

        osal_printf("%-13s = 0x%-9x", "dp_bf_rxq", hsa.r8380.dp_bf_rxq);
        osal_printf("%-13s = 0x%-9x", "reason", hsa.r8380.reason);
        osal_printf("%-13s = 0x%-9x", "fwd_base", hsa.r8380.fwd_base);
        osal_printf("%-13s = 0x%-9x\n", "dpc", hsa.r8380.dpc);

        osal_printf("--------------------------------------------- MIRROR INFO ----------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "mir1_info", hsa.r8380.mir1_info);
        osal_printf("%-13s = 0x%-9x", "mir2_info", hsa.r8380.mir2_info);
        osal_printf("%-13s = 0x%-9x", "mir3_info", hsa.r8380.mir3_info);
        osal_printf("%-13s = 0x%-9x\n", "mir4_info", hsa.r8380.mir4_info);
        osal_printf("%-13s = 0x%-9x", "rspan_rm", hsa.r8380.rspan_rm);
        osal_printf("%-13s = 0x%-9x\n", "mir_nor_fwd", hsa.r8380.mir_nor_fwd);

        osal_printf("-------------------------------------------- FOR RX CPU TAG --------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "fwd_vid", hsa.r8380.fwd_vid);
        osal_printf("%-13s = 0x%-9x", "ale_int_pri", hsa.r8380.ale_int_pri);
        osal_printf("%-13s = 0x%-9x", "l2_pmv", hsa.r8380.l2_pmv);
        osal_printf("%-13s = 0x%-9x\n", "new_sa", hsa.r8380.new_sa);
        osal_printf("%-13s = 0x%-9x", "atk_type", hsa.r8380.atk_type);
        osal_printf("%-13s = 0x%-9x", "atk_hit", hsa.r8380.atk_hit);
        osal_printf("%-13s = 0x%-9x\n", "mac_cst", hsa.r8380.mac_cst);
        osal_printf("%-13s = 0x%-9x", "acl_hit", hsa.r8380.acl_hit);
        osal_printf("%-13s = 0x%-11x\n", "acl_idx", (hsa.r8380.acl_idx_10 << 10) | hsa.r8380.acl_idx_9_0);

        osal_printf("------------------------------------------------- MISC -------------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "is_eav_a", hsa.r8380.is_eav_a);
        osal_printf("%-13s = 0x%-9x\n", "is_eav_b", hsa.r8380.is_eav_b);
        osal_printf("%-13s = 0x%-9x", "da_lk_hit", hsa.r8380.da_lk_hit);
        osal_printf("%-13s = 0x%-9x", "acl_as_pri", hsa.r8380.acl_as_pri);
        osal_printf("%-13s = 0x%-9x\n", "cpu_as_pri", hsa.r8380.cpu_as_pri);

        osal_printf("---------------------------------------------- TX/RX QUEUE ----------------------------------------------\n");
        osal_printf("%-13s = 0x%-9x", "fst_dsc", hsa.r8380.fst_dsc);
        osal_printf("%-13s = 0x%-9x", "lst_dsc", hsa.r8380.lst_dsc);
        osal_printf("%-13s = 0x%-9x\n", "inbw_pktlen", hsa.r8380.inbw_pktlen);
        osal_printf("%-13s = 0x%-9x", "txqid_nrm", hsa.r8380.txqid_nrm);
        osal_printf("%-13s = 0x%-9x", "txqid_cpu", hsa.r8380.txqid_cpu);
        osal_printf("%-13s = 0x%-9x\n", "rxqid", hsa.r8380.rxqid);

        osal_printf("\n========================================================================================================\n");
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
       uint32 port;
       osal_printf("\n===============================================  HSA  ==================================================\n");
       osal_printf("drop_rsn:7=%2d\t\t\t", hsa.r9300.drop_rsn );          /* Drop reason */
       osal_printf("fst_dsc:12=0x%3x\t\t", hsa.r9300.fst_dsc );          /* start page */
       osal_printf("txqid_cpu:5=%2d\n", hsa.r9300.txqid_cpu.qid );         /* CPU QID */
       for(port=0; port<24; port++)
       {
            osal_printf("txqid_normal[%u]=%2d\t\t", port, hsa.r9300.txqid_normal[port].qid ); /* Queue ID per-port */
            if((port+1)%3 == 0)
                osal_printf("\n");
       }
       for(port=24; port<28; port++)
       {
            osal_printf("txqid_stk[%u]=%2d\t\t", port, hsa.r9300.txqid_stk[port-24].qid ); /* Queue ID per stk port */
            if((port+1)%3 == 0)
                osal_printf("\n");
       }

       osal_printf("dpm=0x%7x\t\t\t", hsa.r9300.dpm.bits[0] );          /* destination port mask */
       osal_printf("dpm_lp=0x%7x\t\t", hsa.r9300.dpm_lb );            /* send to loopback port */
       osal_printf("lb_ttl=0x%7x\n", hsa.r9300.lb_ttl );                /* loopback times */
       osal_printf("meta_data=0x%7x\t\t", hsa.r9300.meta_data );      /* metadata */
       osal_printf("lb_pkt:1=0x%d\t\t\t", hsa.r9300.lb_pkt );            /* loopback packet */
       osal_printf("replicate_lst_cp:1=%d\n", hsa.r9300.replicate_lst_cp );  /* IPMC last copy */
       osal_printf("replicate_fst_cp:1=%d\t\t", hsa.r9300.replicate_fst_cp );  /* IPMC first copy */
       osal_printf("replicate_noe:1=%d\t\t", hsa.r9300.replicate_noe );     /* Pkt from non-replication queue */
       osal_printf("replicate_type:2=%d\n", hsa.r9300.replicate_type );    /* IPMC packet type*/
       osal_printf("mir3_info:5=%2d\t\t\t", hsa.r9300.mir3_info );         /* mirroring information for set 3 */
       osal_printf("mir2_info:5=%2d\t\t\t", hsa.r9300.mir2_info );         /* mirroring information for set 2 */
       osal_printf("mir1_info:5=%2d\n", hsa.r9300.mir1_info );         /* mirroring information for set 1 */
       osal_printf("mir0_info:5=%2d\t\t\t", hsa.r9300.mir0_info );         /* mirroring information for set 0
                                    0~28: TX mirror
                                    29: RX mirror
                                    30: RSPAN mirror
                                    31: not mirror
                                */
       osal_printf("mir_normal_fwd:4=0x%x\t\t", hsa.r9300.mir_normal_fwd );    /* Normal forward to mirroring port */
       osal_printf("dy_gasp:1=%d\n", hsa.r9300.dy_gasp );           /* Is dying gasp packet */
       osal_printf("dpc:5=%2d\t\t\t", hsa.r9300.dpc );               /* How many destination port this packet has */
       osal_printf("page_cnt:6=%2d\t\t\t", hsa.r9300.page_cnt );          /* Page count */
       osal_printf("pkt_len:14=0x%4x\n", hsa.r9300.pkt_len );          /* Packet length (Include CRC) */
       osal_printf("lst_dsc:12=0x%3x\t\t", hsa.r9300.lst_dsc );          /* end page */
       osal_printf("ipmc_qid:1=%d\t\t\t", hsa.r9300.ipmc_qid );          /* IPMC Queue UD */
       osal_printf("ipmc_pkt:1=%d\n", hsa.r9300.ipmc_pkt );          /* Is IPMC packet */
       osal_printf("ip-rsvd flag invert:1=%d\t\t", hsa.r9300.ip_rsvd_invert);        /* invert ip rsvd flag bit in ipv4 header */
       osal_printf("pmv_forbid:1=%d\t\t\t", hsa.r9300.pmv_forbid );        /* Pmove is forbade */
       osal_printf("new_sa:1=%d\n", hsa.r9300.new_sa );            /* New SA */
       osal_printf("l2_dyn_pmv:1=%d\t\t\t", hsa.r9300.l2_dyn_pmv );        /* Dynamic L2 entry port move */
       osal_printf("l2_sttc_pmv:1=%d\t\t\t", hsa.r9300.l2_sttc_pmv );       /* Static L2 entry port move */
       osal_printf("l2_org_port:5=%2d\n", hsa.r9300.l2_org_port );       /* Pmove origin port in L2 table */
       osal_printf("l2_org_trk_if:1=%d\t\t", hsa.r9300.l2_org_trk_if );     /* Pmove orign port is trunk port */
       osal_printf("atk_type:5=%2d\t\t\t", hsa.r9300.atk_type );          /* Attack packet type */
       osal_printf("l3_sa_if:1=%d\n", hsa.r9300.l3_sa_if );          /* Is L3 route SA enabled */
       osal_printf("l3_da_if:1=%d\t\t\t", hsa.r9300.l3_da_if );          /* Is L3 route DA enabled */
       osal_printf("acl_hit:1=%d\t\t\t", hsa.r9300.acl_hit );           /* Hit ACL rule */
       osal_printf("acl_idx:11=0x%3x\n", hsa.r9300.acl_idx );          /* ACL index */
       osal_printf("is_eav_a:1=%d\t\t\t", hsa.r9300.is_eav_a );          /* Incoming is EAV packet with class A */
       osal_printf("is_eav_b:1=%d\t\t\t", hsa.r9300.is_eav_b );          /* Incoming is EAV packet with class B */
       osal_printf("reason:6=%2d\n", hsa.r9300.reason );            /* Reason for CPU */
       osal_printf("acl_rmk_dscp:1=%d\t\t", hsa.r9300.acl_rmk_dscp );       /* Modify dscp field */
       osal_printf("new_dscp:8=0x%2x\t\t\t", hsa.r9300.new_dscp );           /* New dscp field */
       osal_printf("int_pri:3=%d\n", hsa.r9300.int_pri );           /* Internal priority, for egress remarking */
       osal_printf("cpu_tagsts_as:1=%d\t\t", hsa.r9300.cpu_tagsts_as );          /* CPU Assign tag status,0=don't touch,1=ALE */
       osal_printf("l2_mac_cst:1=%d\t\t\t", hsa.r9300.l2_mac_cst );        /* MAC constraint (learn limit) */
       osal_printf("iacl_otpid:1=%d\n", hsa.r9300.iacl_otpid );                 /* ivc , stacking,assign itpid */
       osal_printf("eacl_otpid:1=%d\t\t\t", hsa.r9300.eacl_otpid );         /* is ALE outer TPID valid */
       osal_printf("iacl_ipri_as:1=%d\t\t", hsa.r9300.iacl_ipri_as );        /* Is C2SC outer priority valid */
       osal_printf("iacl_opri_as:1=%d\n", hsa.r9300.iacl_opri_as );        /* Is C2SC outer priority valid */
       osal_printf("eacl_opri_as:1=%d\t\t", hsa.r9300.eacl_opri_as );        /* Is EACL outer priority valid */
       osal_printf("eacl_ipri_as:1=%d\t\t", hsa.r9300.eacl_ipri_as );        /* Is EACL inner priority valid */
       osal_printf("ivc_ipri_as:1=%d\n", hsa.r9300.iacl_ipri_as );        /* Is C2SC inner priority valid */
       osal_printf("eacl_itag_status_as:1=%d\t\t", hsa.r9300.eacl_itag_status_as );         /* Egress ACL force inner tag status */
       osal_printf("eacl_otag_status_as:1=%d\t\t", hsa.r9300.eacl_otag_status_as );         /* Egress ACL force outer tag status */
       osal_printf("ivc_itag_status_as:1=%d\n", hsa.r9300.iacl_itag_status_as );         /* C2SC force inner tag status */
       osal_printf("ivc_otag_status_as:1=%d\t\t", hsa.r9300.iacl_otag_status_as );         /* C2SC force outer tag status */
       osal_printf("ale_itag_status:2=%d\t\t", hsa.r9300.ale_itag_status );       /* Tag status for inner VLAN. 0=untag, 1=tag,2=Keep format */
       osal_printf("ale_otag_status:2=%d\n", hsa.r9300.ale_otag_status );       /* Tag status for outer VLAN. 0=untag, 1=tag,2=Keep format */
       osal_printf("otpid_idx:2=%d\t\t\t", hsa.r9300.otpid_idx );               /* Index of Egress outer TPID */
       osal_printf("iacl_itpid:1=%d\t\t\t", hsa.r9300.iacl_itpid );                 /* ivc , stacking,assign itpid */
       osal_printf("eacl_itpid:1=%d\n", hsa.r9300.eacl_itpid );                 /* eacl assign otpid */
       osal_printf("itpid_idx:2=%d\t\t\t", hsa.r9300.itpid_idx );               /* Index of Egress Inner TPID */
       osal_printf("ale_otag_pri:3=%d\t\t", hsa.r9300.ale_otag_pri );           /* New Outer tag (VLAN ID is combination result of Egressa ACL, C2SC, IACL, port-based) */
       osal_printf("ale_otag_cfi:1=%d\n", hsa.r9300.ale_otag_cfi );
       osal_printf("ale_otag_vid:12=0x%3x\t\t", hsa.r9300.ale_otag_vid );
       osal_printf("ale_itag_pri:3=%d\t\t", hsa.r9300.ale_itag_pri );         /* New inner tag(VLAN ID is combinational result of ACL, C2SC, port-protocol) */
       osal_printf("ale_itag_cfi:1=%d\n", hsa.r9300.ale_itag_cfi );
       osal_printf("ale_itag_vid:12=0x%3x\t\t", hsa.r9300.ale_itag_vid );
       osal_printf("eacl_ivid_as:1=%d\t\t", hsa.r9300.eacl_ivid_as );         /* Is EACL inner VID valid */
       osal_printf("eacl_ovid_as:1=%d\n", hsa.r9300.eacl_ovid_as );      /* ACL force outer VLAN ID */
       osal_printf("N_1_ivid_as:1=%d\t\t\t", hsa.r9300.n_1_ivid_as );         /* Is EACL inner VID valid */
       osal_printf("N_1_ovid_as:1=%d\t\t\t", hsa.r9300.n_1_ovid_as );      /* ACL force outer VLAN ID */
       osal_printf("ttl_dec:1=%d\n", hsa.r9300.ttl_dec );           /* L3 route ttl */
       osal_printf("l2_hash_full:1=%d\t\t", hsa.r9300.l2_hash_full );      /* Is LUT hash bucket full */
       osal_printf("invalid_sa:1=%d\t\t\t", hsa.r9300.invalid_sa );        /* Invalid SA */
       osal_printf("routed_pkt:1=%d\n", hsa.r9300.routed_pkt );  /* Router change mac */
       osal_printf("cpu_fmt_en:1=%d\t\t\t", hsa.r9300.cpu_fmt_en );        /* ACL trap/copy to cpu pkt format(original or modify) enable */
       osal_printf("cpu_fmt:1=%d\t\t\t", hsa.r9300.cpu_fmt );           /* ACL trap/copy to cpu pkt format(original or modify) */
       osal_printf("l3_sa_idx:6=%2d\n", hsa.r9300.l3_sa_idx );         /* Pointer ID for DA, L3 routing function */
       osal_printf("l3_da_idx:11=0x%3x\t\t", hsa.r9300.l3_da_idx );        /* Pointer ID for DA, L3 routing function */
       osal_printf("oam_swap_mac:1=%d\t\t", hsa.r9300.oam_swap_mac );      /* Swap DA and SA for oam loopback */
       osal_printf("dp:2=%d\n", hsa.r9300.dp );                /* Color of packet */
       osal_printf("fwd_base:1=%d\t\t\t", hsa.r9300.fwd_base );          /* The source of RVID, 1=RVID get from outer tag, 0= RVID get from inner tag */
       osal_printf("fwd_vid:12=0x%3x\t\t", hsa.r9300.fwd_vid );          /* Relayed VID */
       osal_printf("org_otpid_idx:2=%d\n", hsa.r9300.org_otpid_idx );     /* original TPID index for outer tag */
       osal_printf("org_itpid_idx:2=%d\t\t", hsa.r9300.org_itpid_idx );     /* original TPID index for inner tag */
       osal_printf("org_otag_pri:3=%d\t\t", hsa.r9300.org_otag_pri );      /* Outer VLAN Tag B[15:13]=User-priority, B[12]=CFI, B[11:0]=VID */
       osal_printf("org_otag_cfi:1=%d\n", hsa.r9300.org_otag_cfi );
       osal_printf("org_otag_vid:12=0x%3x\t\t", hsa.r9300.org_otag_vid );
       osal_printf("org_itag_pri:3=%d\t\t", hsa.r9300.org_itag_pri );      /* Inner VLAN Tag B[15:13]=User-priority, B[12]=CFI, B[11:0]=VID */
       osal_printf("org_itag_cfi:1=%d\n", hsa.r9300.org_itag_cfi );
       osal_printf("org_itag_vid:12=0x%3x\t\t", hsa.r9300.org_itag_vid );
       osal_printf("etag_if:1=%d\t\t\t", hsa.r9300.etag_if );           /* Has Extra tag */
       osal_printf("ctag_if:1=%d\n", hsa.r9300.ctag_if );           /* Has CPU Tag */
       osal_printf("org_otag_if:1=%d\t\t\t", hsa.r9300.org_otag_if );       /* Has outer VLAN Tag(Tag or priority tag) */
       osal_printf("org_itag_if:1=%d\t\t\t", hsa.r9300.org_itag_if );       /* Has inner VLAN Tag(Tag or priority tag) */
       osal_printf("llc_other:1=%d\n", hsa.r9300.llc_other );         /* LEN < 0x0600, {DSAP, SSAP, CTL, OUI} != 0xaaaa03_000000 */
       osal_printf("pppoe_if:1=%d\t\t\t", hsa.r9300.pppoe_if );          /* PPPoE packet */
       osal_printf("rfc1042:1=%d\t\t\t", hsa.r9300.rfc1042 );           /* LLC SNAP */
       osal_printf("sphy:5=%2d\n", hsa.r9300.sphy );              /* Source Port ID */
       osal_printf("l2_err_pkt:1=%d\t\t\t", hsa.r9300.l2_err_pkt );        /* Error packet(CRC,Symbol) */
       osal_printf("l3_err_pkt:1=%d\t\t\t", hsa.r9300.l3_err_pkt );        /* Is l3 checksum error packet */
       osal_printf("ipv6_pkt:1=%d\n", hsa.r9300.ipv6_pkt );          /* IPv6 Type */
       osal_printf("ipv4_pkt:1=%d\t\t\t", hsa.r9300.ipv4_pkt );          /* IPv4 Type */

       osal_printf("stk_src_unit:4=%d\t\t", hsa.r9300.stk_src_unit);
       osal_printf("stk_spa:6=%d\n", hsa.r9300.stk_spa);
       osal_printf("stk_spa_is_trk:1=%d\t\t", hsa.r9300.stk_spa_is_trk);
       osal_printf("stk_spa_trk_id:6=%d\t\t", hsa.r9300.stk_spa_trk_id);
       osal_printf("igr_intf_id:12=%2d\t\n", hsa.r9300.igr_intf_id );
       osal_printf("trk_hash:6=%d\t\t\t", hsa.r9300.trk_hash);
       osal_printf("vb_iso_mbr:1=%d\t\t\t", hsa.r9300.vb_iso_mbr );
       osal_printf("sa_lrn:1=%d\n", hsa.r9300.sa_lrn);
       osal_printf("fwd_type:2=%d\t\t\t", hsa.r9300.fwd_type);
       osal_printf("dp_fmt:1=%d\t\t\t", hsa.r9300.dp_fmt);
       osal_printf("dp_info:12=%d\n", hsa.r9300.dp_info);
       osal_printf("da_hit:1=%d\t\t\t", hsa.r9300.da_hit);
       osal_printf("sp_fltr_en:1=%d\t\t\t", hsa.r9300.sp_fltr_en);

       osal_printf("ctx_dpm:56=0x%8x - 0x%8x\n", hsa.r9300.ctx_dpm.bits[0],hsa.r9300.ctx_dpm.bits[1]);
       osal_printf("ctx_fwd_type:4=%d\t\t", hsa.r9300.ctx_fwd_type );
       osal_printf("ctx_unit_id:4=%d\t\t\t", hsa.r9300.ctx_unit_id );
       osal_printf("ctx_sp_fltr_en:1=%d\n", hsa.r9300.ctx_sp_fltr_en);
       osal_printf("ctx_sp_is_trk:1=%d\t\t", hsa.r9300.ctx_sp_is_trk);
       osal_printf("ctx_spn:10=%d\t\t\t", hsa.r9300.ctx_spn);
       osal_printf("ctx_fvid_en:1=%d\n", hsa.r9300.ctx_fvid_en );
       osal_printf("ctx_qid_as:1=%d\t\t\t", hsa.r9300.ctx_qid_as );
       osal_printf("ctx_qid:5=%2d\t\t\t", hsa.r9300.ctx_qid );
       osal_printf("ctx_bp_vlan_egr:1=%d\n", hsa.r9300.ctx_bp_vlan_egr );
       osal_printf("ctx_bp_stp_egr:1=%d\t\t", hsa.r9300.ctx_bp_stp_egr );
       osal_printf("ctx_bp_fltr_egr:1=%d\t\t", hsa.r9300.ctx_bp_fltr_egr );
       osal_printf("ctx_cngst_drop:1=%d\n", hsa.r9300.ctx_cngst_drop );

       osal_printf("ctx_acl_act:1=%d\t\t\t", hsa.r9300.ctx_acl_act);
       osal_printf("ctx_l3_act:1=%2d\t\t\t", hsa.r9300.ctx_l3_act);
       osal_printf("stk_mir:1=%d\n", hsa.r9300.stk_mir);
       osal_printf("miring_port_0:5=%d\t\t", hsa.r9300.miring_port_0 );
       osal_printf("miring_port_1:5=%d\t\t", hsa.r9300.miring_port_1);
       osal_printf("miring_port_2:5=%d\n", hsa.r9300.miring_port_2);
       osal_printf("miring_port_3:5=%d\t\t", hsa.r9300.miring_port_3);
       osal_printf("stk_nor_fwd:4=%d\t\t\t", hsa.r9300.stk_nor_fwd);
       osal_printf("cpu_fwd_type:4=%d\n", hsa.r9300.cpu_fwd_type);

       osal_printf("sflow_info:5=%d\t\t\t", hsa.r9300.sflow_info);
       osal_printf("sflow_smpl_port:3=%d\t\t", hsa.r9300.sflow_smpl_port);
       osal_printf("nor_cpu_sel:1=%d\n", hsa.r9300.nor_cpu_sel);
       osal_printf("\n========================================================================================================\n");
    }
    else
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        osal_printf("\n========================================HSA============================================\n");

        osal_printf("hsb_eadr : 0x%x\t\t\t", hsa.r9310.hsb_eadr );
        osal_printf("pkt_len : 0x%x\t\t\t", hsa.r9310.pkt_len );
        osal_printf("pg_cnt : 0x%x\n", hsa.r9310.pg_cnt );
        osal_printf("stack_if : 0x%x\t\t\t", hsa.r9310.stack_if );
        osal_printf("cpu_tx_if : 0x%x\t\t\t", hsa.r9310.cpu_tx_if );
        osal_printf("of_if : 0x%x\n", hsa.r9310.of_if );
        osal_printf("of_pipeline : 0x%x\t\t", hsa.r9310.of_pipeline );
        osal_printf("Stack_fmt : 0x%x\t\t\t", hsa.r9310.Stack_fmt );
        osal_printf("ori_itag_if : 0x%x\n", hsa.r9310.ori_itag_if );
        osal_printf("ori_otag_if : 0x%x\t\t", hsa.r9310.ori_otag_if );
        osal_printf("ori_itag_vid : 0x%x\t\t", hsa.r9310.ori_itag_vid );
        osal_printf("ori_itag_cfi : 0x%x\n", hsa.r9310.ori_itag_cfi );
        osal_printf("ori_itag_pri : 0x%x\t\t", hsa.r9310.ori_itag_pri );
        osal_printf("ori_itag_tpid : 0x%x\t\t", hsa.r9310.ori_itag_tpid );
        osal_printf("ori_otag_vid : 0x%x\n", hsa.r9310.ori_otag_vid );
        osal_printf("ori_otag_cfi : 0x%x\t\t", hsa.r9310.ori_otag_cfi );
        osal_printf("ori_otag_pri : 0x%x\t\t", hsa.r9310.ori_otag_pri );
        osal_printf("ori_otag_tpid : 0x%x\n", hsa.r9310.ori_otag_tpid );
        osal_printf("class_a : 0x%x\t\t\t", hsa.r9310.class_a );
        osal_printf("class_b : 0x%x\n", hsa.r9310.class_b );
        osal_printf("pppoe_if : 0x%x\t\t\t", hsa.r9310.pppoe_if );
        osal_printf("frame_type : 0x%x\n", hsa.r9310.frame_type );
        osal_printf("evlan_if : 0x%x\t\t\t", hsa.r9310.evlan_if );
        osal_printf("ipv4_if : 0x%x\t\t\t", hsa.r9310.ipv4_if );
        osal_printf("ipv6_if : 0x%x\n", hsa.r9310.ipv6_if );
        osal_printf("org_ompls_if : 0x%x\t\t", hsa.r9310.org_ompls_if );
        osal_printf("org_impls_if : 0x%x\t\t", hsa.r9310.org_impls_if );
        osal_printf("parser_cant_handle : 0x%x\n", hsa.r9310.parser_cant_handle );
        osal_printf("IP6_EH_HOPBYHOP_ERR : 0x%x\t", hsa.r9310.IP6_EH_HOPBYHOP_ERR );
        osal_printf("IP6_EH_HOPBYHOP_EXIST : 0x%x\t", hsa.r9310.IP6_EH_HOPBYHOP_EXIST );
        osal_printf("IP6_EH_ROUTE_EXIST : 0x%x\n", hsa.r9310.IP6_EH_ROUTE_EXIST );
        osal_printf("IP6_EH_FRAGMENT_EXIST : 0x%x\t", hsa.r9310.IP6_EH_FRAGMENT_EXIST );
        osal_printf("IP6_EH_ESP_EXIST : 0x%x\t\t", hsa.r9310.IP6_EH_ESP_EXIST );
        osal_printf("IP6_EH_AUTH_EXIST : 0x%x\n", hsa.r9310.IP6_EH_AUTH_EXIST );
        osal_printf("IP6_EH_DESTINATION_EXIST : 0x%x\t", hsa.r9310.IP6_EH_DESTINATION_EXIST );
        osal_printf("IP6_EH_MOBILITY_EXIST : 0x%x\t", hsa.r9310.IP6_EH_MOBILITY_EXIST );
        osal_printf("IP6_EH_REPEAT : 0x%x\n", hsa.r9310.IP6_EH_REPEAT );
        osal_printf("ip_prot : 0x%x\t\t\t", hsa.r9310.ip_prot );
//        osal_printf("ip_offset_zero : 0x%x\t\t\t", hsa.r9310.ip_offset_zero );
        osal_printf("ip_flag_set : 0x%x\n", hsa.r9310.ip_flag_set );
        osal_printf("l4_hdr_rdy : 0x%x\t\t", hsa.r9310.l4_hdr_rdy );
        osal_printf("l4_offset : 0x%x\n", hsa.r9310.l4_offset );
        osal_printf("l2_err_pkt : 0x%x\t\t", hsa.r9310.l2_err_pkt );
        osal_printf("l3_err_pkt : 0x%x\n", hsa.r9310.l3_err_pkt );
        osal_printf("L3_offset : 0x%x\t\t\t", hsa.r9310.L3_offset );
        osal_printf("in_spn : 0x%x\t\t\t", hsa.r9310.in_spn );
        osal_printf("spn : 0x%x\n", hsa.r9310.spn );
        osal_printf("sp_trk : 0x%x\t\t\t", hsa.r9310.sp_trk );
        osal_printf("sp_is_trk : 0x%x\t\t\t", hsa.r9310.sp_is_trk );
        osal_printf("int_itag_hit : 0x%x\n", hsa.r9310.int_itag_hit );
        osal_printf("int_otag_hit : 0x%x\t\t", hsa.r9310.int_otag_hit );
        osal_printf("l2_crc : 0x%x\t\t\t\n", hsa.r9310.l2_crc );
        osal_printf("da_hit : 0x%x\t\t\t", hsa.r9310.da_hit );
        osal_printf("fwd_vid_en : 0x%x\t\t", hsa.r9310.fwd_vid_en );
        osal_printf("rma_pkt : 0x%x\n", hsa.r9310.rma_pkt );
        osal_printf("meta_data : 0x%x\t\t\t", hsa.r9310.meta_data );
        osal_printf("trk_hash : 0x%x\n", hsa.r9310.trk_hash );
        osal_printf("dp : 0x%x\t\t\t", hsa.r9310.dp );
        osal_printf("internal_Priority : 0x%x\n", hsa.r9310.internal_Priority );
        osal_printf("l3_intf_id : 0x%x\t\t", hsa.r9310.l3_intf_id );
        osal_printf("tt_idx : 0x%x\t\t\t", hsa.r9310.tt_idx );
        osal_printf("tt_hit : 0x%x\n", hsa.r9310.tt_hit );
        osal_printf("hsa_sel : 0x%x\t\t\t", hsa.r9310.hsa_sel );
        osal_printf("hsa_sadr : 0x%x\t\t\t", hsa.r9310.hsa_sadr );
        osal_printf("hsb_sadr : 0x%x\n", hsa.r9310.hsb_sadr );
        osal_printf("dpm[56:32] : 0x%08x\t\t", hsa.r9310.dpm.bits[1]);
        osal_printf("dpm[31:0] : 0x%08x\n", hsa.r9310.dpm.bits[0]);
        osal_printf("cp_cnt : 0x%x\t\t\t", hsa.r9310.cp_cnt );
        osal_printf("stk_cpu_fwd : 0x%x\t\t", hsa.r9310.stk_cpu_fwd );
        osal_printf("stk_nm_fwd : 0x%x\n", hsa.r9310.stk_nm_fwd );
        osal_printf("mirroring_info : 0x%x\t\t", hsa.r9310.mirroring_info );
        osal_printf("mirrored_info : 0x%x\t", hsa.r9310.mirrored_info );
        osal_printf("mirr_nfwd : 0x%x\n", hsa.r9310.mirr_nfwd );
        osal_printf("local_cpu_fwd : 0x%x\t\t", hsa.r9310.local_cpu_fwd );
        osal_printf("sflow_mirroring : 0x%x\t\t", hsa.r9310.sflow_mirroring );
        osal_printf("sflow_mirrored : 0x%x\n", hsa.r9310.sflow_mirrored );
        osal_printf("dm_pkt : 0x%x\t\t\t", hsa.r9310.dm_pkt );
        osal_printf("dg_pkt : 0x%x\t\t\t", hsa.r9310.dg_pkt );
        osal_printf("tx_mir_cancel : 0x%x\n", hsa.r9310.tx_mir_cancel );
        osal_printf("vb_iso_mbr : 0x%x\t\t", hsa.r9310.vb_iso_mbr );
        osal_printf("learn_en : 0x%x\t\t\t", hsa.r9310.learn_en );
        osal_printf("fwd_vid_sel : 0x%x\n", hsa.r9310.fwd_vid_sel );
        osal_printf("fwd_type : 0x%x\t\t\t", hsa.r9310.fwd_type );
        osal_printf("RVID : 0x%x\t\t\t", hsa.r9310.RVID );
        osal_printf("local_reason : 0x%x\n", hsa.r9310.local_reason );
        osal_printf("remote_reason : 0x%x\t\t", hsa.r9310.remote_reason );
        osal_printf("fwd_port : 0x%x\t\t\t", hsa.r9310.fwd_port );
        osal_printf("cpu_tx_fwd_type : 0x%x\n", hsa.r9310.cpu_tx_fwd_type );
        osal_printf("ctx_dpm_55_32 : 0x%x\t\t", hsa.r9310.ctx_dpm_55_32 );
        osal_printf("ctx_dpm_31_0  : 0x%x\n", hsa.r9310.ctx_dpm_31_0 );
        osal_printf("sw_unit : 0x%x\t\t\t", hsa.r9310.sw_unit );
        osal_printf("spf_spn : 0x%x\t\t\t", hsa.r9310.spf_spn );
        osal_printf("spf_is_trk : 0x%x\n", hsa.r9310.spf_is_trk );
        osal_printf("src_filter_en : 0x%x\t\t", hsa.r9310.src_filter_en );
        osal_printf("as_qid : 0x%x\t\t\t", hsa.r9310.as_qid );
        osal_printf("ale_as_tagsts : 0x%x\n", hsa.r9310.ale_as_tagsts );
        osal_printf("bp_vlan_egr : 0x%x\t\t", hsa.r9310.bp_vlan_egr );
        osal_printf("bp_stp : 0x%x\t\t\t", hsa.r9310.bp_stp );
        osal_printf("bp_fltr : 0x%x\n", hsa.r9310.bp_fltr );
        osal_printf("cngst_drop : 0x%x\t\t", hsa.r9310.cngst_drop );
        osal_printf("acl_act : 0x%x\t\t\t", hsa.r9310.acl_act );
        osal_printf("stk_mir_hit : 0x%x\n", hsa.r9310.stk_mir_hit );
        osal_printf("ts_req : 0x%x\t\t\t", hsa.r9310.ts_req );
        osal_printf("mpls_encap_req : 0x%x\t\t", hsa.r9310.mpls_encap_req );
        osal_printf("ts_index : 0x%x\n", hsa.r9310.ts_index );
        osal_printf("bssid_idx : 0x%x\t\t\t", hsa.r9310.bssid_idx );
        osal_printf("mpls_encap_idx : 0x%x\t\t", hsa.r9310.mpls_encap_idx );
        osal_printf("mpls_decap_req : 0x%x\n", hsa.r9310.mpls_decap_req );
        osal_printf("ip_ttl : 0x%x\t\t\t", hsa.r9310.ip_ttl );
        osal_printf("HSA_N21_IPRI_HIT : 0x%x\t\t", hsa.r9310.HSA_N21_IPRI_HIT );
        osal_printf("HSA_INT_DSCP : 0x%x\n", hsa.r9310.HSA_INT_DSCP );
        osal_printf("HSA_INT_DSCP_HIT : 0x%x\t\t", hsa.r9310.HSA_INT_DSCP_HIT );
        osal_printf("mpls_decap_sta : 0x%x\t\t", hsa.r9310.mpls_decap_sta );
        osal_printf("HSA_N21_IPRI : 0x%x\n", hsa.r9310.HSA_N21_IPRI );
        osal_printf("HSA_N21_OPRI : 0x%x\t\t", hsa.r9310.HSA_N21_OPRI );
        osal_printf("HSA_N21_OPRI_HIT : 0x%x\t\t", hsa.r9310.HSA_N21_OPRI_HIT );
        osal_printf("mpls_tc : 0x%x\n", hsa.r9310.mpls_tc );
        osal_printf("HSA_INT_ITAG_IF : 0x%x\t\t", hsa.r9310.HSA_INT_ITAG_IF );
        osal_printf("SURF_OVID : 0x%x\t\t\t", hsa.r9310.SURF_OVID );
        osal_printf("HSA_ITAG_PRI : 0x%x\n", hsa.r9310.HSA_ITAG_PRI );
        osal_printf("HSA_EACL_IPRI_HIT : 0x%x\t\t", hsa.r9310.HSA_EACL_IPRI_HIT );
        osal_printf("HSA_INT_OTAG_VID : 0x%x\t\t", hsa.r9310.HSA_INT_OTAG_VID );
        osal_printf("HSA_IACL_IPRI_HIT : 0x%x\n", hsa.r9310.HSA_IACL_IPRI_HIT );
        osal_printf("SURF_IVID : 0x%x\t\t\t", hsa.r9310.SURF_IVID );
        osal_printf("HSA_OTAG_PRI : 0x%x\t\t", hsa.r9310.HSA_OTAG_PRI );
        osal_printf("EACL_IVID_HIT : 0x%x\n", hsa.r9310.EACL_IVID_HIT );
        osal_printf("HSA_INT_ITAG_VID : 0x%x\t\t", hsa.r9310.HSA_INT_ITAG_VID );
        osal_printf("HSA_EACL_OPRI_HIT : 0x%x\t\t", hsa.r9310.HSA_EACL_OPRI_HIT );
        osal_printf("HSA_IACL_OPRI_HIT : 0x%x\n", hsa.r9310.HSA_IACL_OPRI_HIT );
        osal_printf("ACL_OTPID_IDX : 0x%x\t\t", hsa.r9310.ACL_OTPID_IDX );
        osal_printf("HSA_INT_OTAG_CFI : 0x%x\t\t", hsa.r9310.HSA_INT_OTAG_CFI );
        osal_printf("HSA_INT_ITAG_CFI : 0x%x\n", hsa.r9310.HSA_INT_ITAG_CFI );
        osal_printf("vrf_id : 0x%x\t\t\t", hsa.r9310.vrf_id );
        osal_printf("l3_mdf_req : 0x%x\t\t", hsa.r9310.l3_mdf_req );
        osal_printf("oam_lbk : 0x%x\n", hsa.r9310.oam_lbk );
        osal_printf("N21_IVID_HIT : 0x%x\t\t", hsa.r9310.N21_IVID_HIT );
        osal_printf("fwd_p_type : 0x%x\t\t", hsa.r9310.fwd_p_type );
        osal_printf("EACL_OTAG_HIT : 0x%x\n", hsa.r9310.EACL_OTAG_HIT );
        osal_printf("EACL_OTAG_IF : 0x%x\t\t", hsa.r9310.EACL_OTAG_IF );
        osal_printf("HSA_INT_OTAG_IF : 0x%x\t\t", hsa.r9310.HSA_INT_OTAG_IF );
        osal_printf("EACL_ITAG_HIT : 0x%x\n", hsa.r9310.EACL_ITAG_HIT );
        osal_printf("EACL_ITAG_IF : 0x%x\t\t", hsa.r9310.EACL_ITAG_IF );
        osal_printf("EACL_OVID_HIT : 0x%x\t\t", hsa.r9310.EACL_OVID_HIT );
        osal_printf("N21_OVID_HIT : 0x%x\n", hsa.r9310.N21_OVID_HIT );
        osal_printf("IACL_OTPID_HIT : 0x%x\t\t", hsa.r9310.IACL_OTPID_HIT );
        osal_printf("IACL_ITPID_HIT : 0x%x\t\t", hsa.r9310.IACL_ITPID_HIT );
        osal_printf("EACL_OTPID_HIT : 0x%x\n", hsa.r9310.EACL_OTPID_HIT );
        osal_printf("EACL_ITPID_HIT : 0x%x\t\t", hsa.r9310.EACL_ITPID_HIT );
        osal_printf("ACL_ITPID_IDX : 0x%x\t\t", hsa.r9310.ACL_ITPID_IDX );
        osal_printf("port_data : 0x%x\n", hsa.r9310.port_data );
        osal_printf("port_is_trk : 0x%x\t\t", hsa.r9310.port_is_trk );
        osal_printf("invalid_sa : 0x%x\t\t", hsa.r9310.invalid_sa );
        osal_printf("hash_full : 0x%x\n", hsa.r9310.hash_full );
        osal_printf("l2_dyn_pmv : 0x%x\t\t", hsa.r9310.l2_dyn_pmv );
        osal_printf("l2_sttc_pmv : 0x%x\t\t", hsa.r9310.l2_sttc_pmv );
        osal_printf("pmv_forbid : 0x%x\n", hsa.r9310.pmv_forbid );
        osal_printf("new_sa : 0x%x\t\t\t", hsa.r9310.new_sa );
        osal_printf("dm_rxidx : 0x%x\t\t\t", hsa.r9310.dm_rxidx );
        osal_printf("mac_cst : 0x%x\n", hsa.r9310.mac_cst );
        osal_printf("acl_idx : 0x%x\t\t\t", hsa.r9310.acl_idx );
        osal_printf("atk_type : 0x%x\t\t\t", hsa.r9310.atk_type );
        osal_printf("cpu_rx_l2_err : 0x%x\n", hsa.r9310.cpu_rx_l2_err );
        osal_printf("cpu_rx_l3_err : 0x%x\t\t", hsa.r9310.cpu_rx_l3_err );
        osal_printf("cpu_rx_sflow : 0x%x\t\t", hsa.r9310.cpu_rx_sflow );
        osal_printf("of_tbl_id : 0x%x\n", hsa.r9310.of_tbl_id );
        osal_printf("acl_of_hit : 0x%x\t\t", hsa.r9310.acl_of_hit );
        osal_printf("of_miss_tbl_id : 0x%x\n", hsa.r9310.of_miss_tbl_id );
        osal_printf("dmac[6]=%02x:%02x:%02x:%02x:%02x:%02x\n", hsa.r9310.dmac[0],hsa.r9310.dmac[1], hsa.r9310.dmac[2], hsa.r9310.dmac[3], hsa.r9310.dmac[4], hsa.r9310.dmac[5]);
        osal_printf("smac[6]=%02x:%02x:%02x:%02x:%02x:%02x\n", hsa.r9310.smac[0],hsa.r9310.smac[1], hsa.r9310.smac[2], hsa.r9310.smac[3], hsa.r9310.smac[4], hsa.r9310.smac[5]);

        for (i = 0; i < 52; i++)
        {
            if (0 != ((i+1) % 3))
                osal_printf("qid[%02u] : %d\t\t\t", i, hsa.r9310.qid[i].qid);
            else
                osal_printf("qid[%02u] : %d\n", i, hsa.r9310.qid[i].qid);
        }
        osal_printf("\n");  /* end of QID */

        osal_printf("stack_qid[52] : %02d\t\t", hsa.r9310.stack_qid[0].qid);
        osal_printf("stack_qid[53] : %02d\t\t", hsa.r9310.stack_qid[1].qid);
        osal_printf("stack_qid[54] : %02d\n", hsa.r9310.stack_qid[2].qid);
        osal_printf("stack_qid[55] : %02d\t\t", hsa.r9310.stack_qid[3].qid);
        osal_printf("cpu_qid[56] : %02d\t\t\n", hsa.r9310.cpu_qid.qid);

        osal_printf("EST_MATCH : 0x%x\t\t\tDST_PE_ECID : 0x%06x\n", hsa.r9310.EST_MATCH, hsa.r9310.DST_PE_ECID);
        osal_printf("PE_LU_MODE : 0x%x\t\tPE_NSG : 0x%02x\n", hsa.r9310.PE_LU_MODE, hsa.r9310.PE_NSG);
        osal_printf("INT_PE_ECID_EXT : 0x%02x\t\tORI_PE_ETAG_IF : 0x%x\n", hsa.r9310.INT_PE_ECID_EXT, hsa.r9310.ORI_PE_ETAG_IF);
        osal_printf("ORI_PE_IGR_ECID  :                  EXT 0x%02x BASE 0x%03x\n", hsa.r9310.ORI_PE_IGR_ECID_EXT, hsa.r9310.ORI_PE_IGR_ECID_BASE);
        osal_printf("ORI_PE_ETAG_ECID : RSVD 0x%x GRP 0x%x EXT 0x%02x BASE 0x%03x\n", hsa.r9310.ORI_PE_ETAG_RSVD, hsa.r9310.ORI_PE_ECID_GRP, hsa.r9310.ORI_PE_ECID_EXT, hsa.r9310.ORI_PE_ECID_BASE);

        osal_printf("=======================================================================================\n");
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of hal_dumpHsa */
#endif

#if defined(CONFIG_SDK_RTL9310)
int32 _rtk_getHsa_openflow(uint32 unit, hsa_param_t * hsa)
{
    uint32 tmpVal = 0;
    uint32 i;
    uint32 shift_bit = 0;
    hal_control_t *pInfo;

    if(NULL == hsa)
    {
        return RT_ERR_FAILED;
    }

    osal_memset(hsa, 0, sizeof(hsa_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    if (HWP_9310_FAMILY_ID(unit))
    {
        uint32 *val;

        val = osal_alloc(256*sizeof(uint32));
        if (NULL == val)
        {
            osal_printf("osal_alloc  failed!!\n");
            return RT_ERR_FAILED;
        }

        reg_read(unit, MANGO_HSA_DATAr, val);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_HSB_EADRf, &tmpVal, val);
        hsa->r9310_hsa_of.hsb_eadr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PKT_LENf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_len = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PG_CNTf, &tmpVal, val);
        hsa->r9310_hsa_of.pg_cnt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STACK_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.stack_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_TX_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.cpu_tx_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.of_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_PIPELINEf, &tmpVal, val);
        hsa->r9310_hsa_of.of_pipeline = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STACK_FMTf, &tmpVal, val);
        hsa->r9310_hsa_of.stack_fmt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_itag_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_otag_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_VIDf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_itag_vid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_CFIf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_itag_cfi = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_PRIf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_itag_pri = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_ITAG_TPIDf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_itag_tpid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_VIDf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_otag_vid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_CFIf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_otag_cfi = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_PRIf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_otag_pri = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORI_OTAG_TPIDf, &tmpVal, val);
        hsa->r9310_hsa_of.ori_otag_tpid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CLASS_Af, &tmpVal, val);
        hsa->r9310_hsa_of.class_a = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CLASS_Bf, &tmpVal, val);
        hsa->r9310_hsa_of.class_b = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PPPOE_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.pppoe_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FRAME_TYPEf, &tmpVal, val);
        hsa->r9310_hsa_of.frame_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EVLAN_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.evlan_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IPV4_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.ipv4_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IPV6_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.ipv6_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORG_OMPLS_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.org_ompls_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ORG_IMPLS_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.org_impls_if = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PARSER_CANT_HANDLEf, &tmpVal, val);
        hsa->r9310_hsa_of.parser_cant_handle = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_HOPBYHOP_ERRf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_hopbyhop_err = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_HOPBYHOP_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_hopbyhop_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_ROUTE_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_route_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_FRAGMENT_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_fragment_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_ESP_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_esp_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_AUTH_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_auth_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_DESTINATION_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_destination_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_MOBILITY_EXISTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_mobility_exist = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP6_EH_REPEATf, &tmpVal, val);
        hsa->r9310_hsa_of.ip6_eh_repeat = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_PROTf, &tmpVal, val);
        hsa->r9310_hsa_of.ip_prot = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_FLAG_SETf, &tmpVal, val);
        hsa->r9310_hsa_of.ip_flag_set = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L4_HDR_RDYf, &tmpVal, val);
        hsa->r9310_hsa_of.l4_hdr_rdy = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L4_OFFSETf, &tmpVal, val);
        hsa->r9310_hsa_of.l4_offset = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_ERR_PKTf, &tmpVal, val);
        hsa->r9310_hsa_of.l2_err_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L3_ERR_PKTf, &tmpVal, val);
        hsa->r9310_hsa_of.l3_err_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L3_OFFSETf, &tmpVal, val);
        hsa->r9310_hsa_of.l3_offset = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IN_SPNf, &tmpVal, val);
        hsa->r9310_hsa_of.in_spn = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SPNf, &tmpVal, val);
        hsa->r9310_hsa_of.spn = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SP_TRKf, &tmpVal, val);
        hsa->r9310_hsa_of.sp_trk = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SP_IS_TRKf, &tmpVal, val);
        hsa->r9310_hsa_of.sp_is_trk = tmpVal;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.loopback_time = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.loopback_time |= ((tmpVal & 0x1) << 1);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_CRCf, &tmpVal, val);
        hsa->r9310_hsa_of.loopback_time |= ((tmpVal & 0x1) << 2);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DA_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.loopback_time |= ((tmpVal & 0x1) << 3);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_VID_ENf, &tmpVal, val);
        hsa->r9310_hsa_of.loopback_time |= ((tmpVal & 0x1) << 4);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_RMA_PKTf, &tmpVal, val);
        hsa->r9310_hsa_of.loopback_time |= ((tmpVal & 0x1) << 5);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_META_DATAf, &tmpVal, val);
        hsa->r9310_hsa_of.meta_data = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TRK_HASHf, &tmpVal, val);
        hsa->r9310_hsa_of.trk_hash = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DPf, &tmpVal, val);
        hsa->r9310_hsa_of.dp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INTERNAL_PRIORITYf, &tmpVal, val);
        hsa->r9310_hsa_of.internal_priority = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IGR_L3_INTF_IDf, &tmpVal, val);
        hsa->r9310_hsa_of.l3_intf_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TT_IDXf, &tmpVal, val);
        hsa->r9310_hsa_of.tt_idx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TT_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.tt_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SELf, &tmpVal, val);
        hsa->r9310_hsa_of.hsa_sel = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SADRf, &tmpVal, val);
        hsa->r9310_hsa_of.hsa_sadr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_HSB_SADRf, &tmpVal, val);
        hsa->r9310_hsa_of.hsb_sadr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DPM_57_32f, &tmpVal, val);
        hsa->r9310_hsa_of.dpm.bits[1] = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DPM_31_0f, &tmpVal, val);
        hsa->r9310_hsa_of.dpm.bits[0] = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CP_CNTf, &tmpVal, val);
        hsa->r9310_hsa_of.cp_cnt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STK_CPU_FWDf, &tmpVal, val);
        hsa->r9310_hsa_of.stk_cpu_fwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STK_NM_FWDf, &tmpVal, val);
        hsa->r9310_hsa_of.stk_nm_fwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MIRRORING_INFOf, &tmpVal, val);
        hsa->r9310_hsa_of.mirroring_info = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MIRRORED_INFOf, &tmpVal, val);
        hsa->r9310_hsa_of.mirrored_info = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MIRR_NFWDf, &tmpVal, val);
        hsa->r9310_hsa_of.mirr_nfwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_LOCAL_CPU_FWDf, &tmpVal, val);
        hsa->r9310_hsa_of.local_cpu_fwd = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SFLOW_MIRRORINGf, &tmpVal, val);
        hsa->r9310_hsa_of.sflow_mirroring = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SFLOW_MIRROREDf, &tmpVal, val);
        hsa->r9310_hsa_of.sflow_mirrored = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DM_PKTf, &tmpVal, val);
        hsa->r9310_hsa_of.dm_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DG_PKTf, &tmpVal, val);
        hsa->r9310_hsa_of.dg_pkt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TX_MIR_CANCELf, &tmpVal, val);
        hsa->r9310_hsa_of.tx_mir_cancel = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_VB_ISO_MBRf, &tmpVal, val);
        hsa->r9310_hsa_of.vb_iso_mbr = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_LEARN_ENf, &tmpVal, val);
        hsa->r9310_hsa_of.learn_en = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_VID_SELf, &tmpVal, val);
        hsa->r9310_hsa_of.fwd_vid_sel = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_TYPEf, &tmpVal, val);
        hsa->r9310_hsa_of.fwd_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_RVIDf, &tmpVal, val);
        hsa->r9310_hsa_of.rvid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_LOCAL_REASONf, &tmpVal, val);
        hsa->r9310_hsa_of.local_reason = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_REMOTE_REASONf, &tmpVal, val);
        hsa->r9310_hsa_of.remote_reason = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_PORTf, &tmpVal, val);
        hsa->r9310_hsa_of.output_data = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_TX_FWD_TYPEf, &tmpVal, val);
        hsa->r9310_hsa_of.output_type = tmpVal;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CTX_DPM_31_0f, &tmpVal, val);
        hsa->r9310_hsa_of.group_idx = (tmpVal & 0x7FF);
        hsa->r9310_hsa_of.group = ((tmpVal >> 11) & 0x1);
        hsa->r9310_hsa_of.queue_data = ((tmpVal >> 12) & 0x7);
        hsa->r9310_hsa_of.set_queue_data = ((tmpVal >> 15) & 0x1);
        hsa->r9310_hsa_of.field_l4_dport = ((tmpVal >> 16) & 0xFFFF);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CTX_DPM_55_32f, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_l4_dport = (tmpVal & 0x1);
        hsa->r9310_hsa_of.field_l4_sport = ((tmpVal >> 1) & 0xFFFF);
        hsa->r9310_hsa_of.set_field_l4_sport = ((tmpVal >> 17) & 0x1);
        hsa->r9310_hsa_of.field_ip_dip = ((tmpVal >> 18) & 0x3F);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SW_DEVf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SPF_SPNf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x3FF) << 4);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SPF_IS_TRKf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 14);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SRC_FILTER_ENf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 15);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_AS_QIDf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 16);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ALE_AS_TAGSTSf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 17);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BP_VLAN_EGRf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 18);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BP_STPf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 19);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BP_FLTRf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 20);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CNGST_DROPf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 21);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_ACTf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 22);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_STK_MIR_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x3) << 23);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SET_IP_RSVf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dip |= ((tmpVal & 0x1) << 25);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IGR_L3_INTF_TYPEf, &tmpVal, val);
        hsa->r9310_hsa_of.set_ip_rsv = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TS_REQf, &tmpVal, val);
        hsa->r9310_hsa_of.l2mpls_tt = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_ENCAP_REQf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_ip_dip = tmpVal;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_TS_INDEXf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_sip = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_BSSID_IDXf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_sip |= ((tmpVal & 0xFFF) << 9);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_ENCAP_IDXf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_sip |= ((tmpVal & 0x7FF) << 21);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_DECAP_REQf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_ip_sip = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IP_HDR_TTLf, &tmpVal, val);
        hsa->r9310_hsa_of.l3_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_IPRI_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_ip_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_DSCPf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_dscp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_DSCP_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_ip_dscp = tmpVal;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_DECAP_STAf, &tmpVal, val);
        hsa->r9310_hsa_of.field_mpls_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_IPRIf, &tmpVal, val);
        hsa->r9310_hsa_of.field_mpls_ttl |= ((tmpVal & 0x7) << 2);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_OPRIf, &tmpVal, val);
        hsa->r9310_hsa_of.field_mpls_ttl |= ((tmpVal & 0x7) << 5);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_OPRI_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_mpls_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MPLS_TCf, &tmpVal, val);
        hsa->r9310_hsa_of.field_mpls_tc = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_mpls_tc = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SURF_OVIDf, &tmpVal, val);
        hsa->r9310_hsa_of.field_mpls_label = (tmpVal & 0x7FF);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SURF_OVIDf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_mpls_label = ((tmpVal >> 11) & 0x1);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ITAG_PRIf, &tmpVal, val);
        hsa->r9310_hsa_of.field_pcp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_IPRI_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_pcp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_VIDf, &tmpVal, val);
        hsa->r9310_hsa_of.field_vid = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_IPRI_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_vid = tmpVal;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SURF_IVIDf, &tmpVal, val);
        hsa->r9310_hsa_of.field_da = (tmpVal & 0xFFF);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OTAG_PRIf, &tmpVal, val);
        hsa->r9310_hsa_of.field_da |= ((tmpVal & 0x7) << 12);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_IVID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.field_da |= ((tmpVal & 0x7) << 15);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_VIDf, &tmpVal, val);
        hsa->r9310_hsa_of.set_field_da = (tmpVal & 0x1);
        hsa->r9310_hsa_of.field_sa = ((tmpVal >> 1) & 0x3FF);
        hsa->r9310_hsa_of.set_field_sa = ((tmpVal >> 11) & 0x1);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OPRI_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.dec_ip_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_OPRI_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.dec_mpls_ttl = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_OTPID_IDXf, &tmpVal, val);
        hsa->r9310_hsa_of.copy_ttl_outward = (tmpVal & 0x1);
        hsa->r9310_hsa_of.push_vlan_ethtype = ((tmpVal >> 1) & 0x1);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_CFIf, &tmpVal, val);
        hsa->r9310_hsa_of.push_vlan_ethtype |= ((tmpVal & 0x1) << 1);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_ITAG_CFIf, &tmpVal, val);
        hsa->r9310_hsa_of.push_vlan = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_VRF_IDf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_ethtype = (tmpVal & 0x1);
        hsa->r9310_hsa_of.push_mpls_lib_idx = ((tmpVal >> 1) & 0x7F);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L3_MDF_REQf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 7);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OAM_LBKf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 8);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_IVID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 9);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_FWD_P_TYPEf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 10);


        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OTAG_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_vpn_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OTAG_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls_mode = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INT_OTAG_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.push_mpls = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_ITAG_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.pop_mpls_type = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_ITAG_IFf, &tmpVal, val);
        hsa->r9310_hsa_of.pop_mpls = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OVID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.pop_vlan = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_N21_OVID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.copy_ttl_inward = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_OTPID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.to_of = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_IACL_ITPID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.apply_action = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_OTPID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.of_lookup_req = tmpVal;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_EACL_ITPID_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.next_tab = (tmpVal & 0x1);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_ITPID_IDXf, &tmpVal, val);
        hsa->r9310_hsa_of.next_tab |= ((tmpVal & 0x1) << 1);
        hsa->r9310_hsa_of.field_ip_ttl = ((tmpVal >> 1) & 0x1);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PORT_DATAf, &tmpVal, val);
        hsa->r9310_hsa_of.field_ip_ttl |= ((tmpVal & 0x7F) << 1);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_INVALID_SAf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_dscp = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_HASH_FULLf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_dscp |= ((tmpVal & 0x1) << 1);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_DYN_PMVf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_dscp |= ((tmpVal & 0x1) << 2);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_L2_STTC_PMVf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_dscp |= ((tmpVal & 0x1) << 3);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_PMV_FORBIDf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_dscp |= ((tmpVal & 0x1) << 4);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_NEW_SAf, &tmpVal, val);
        hsa->r9310_hsa_of.pkt_dscp |= ((tmpVal & 0x1) << 5);

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DM_RXIDXf, &tmpVal, val);
        hsa->r9310_hsa_of.lb_des_time = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_MAC_CSTf, &tmpVal, val);
        hsa->r9310_hsa_of.mac_cst = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_IDXf, &tmpVal, val);
        hsa->r9310_hsa_of.acl_idx = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ATK_TYPEf, &tmpVal, val);
        hsa->r9310_hsa_of.group_hash = (tmpVal & 0x7);
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_RX_L2_ERRf, &tmpVal, val);
        hsa->r9310_hsa_of.cpu_rx_l2_err = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_RX_L3_ERRf, &tmpVal, val);
        hsa->r9310_hsa_of.cpu_rx_l3_err = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_CPU_RX_SFLOWf, &tmpVal, val);
        hsa->r9310_hsa_of.cpu_rx_sflow = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_TBL_IDf, &tmpVal, val);
        hsa->r9310_hsa_of.of_tbl_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_ACL_OF_HITf, &tmpVal, val);
        hsa->r9310_hsa_of.acl_of_hit = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_OF_MISS_TBL_IDf, &tmpVal, val);
        hsa->r9310_hsa_of.of_miss_tbl_id = tmpVal;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DMAC_47_32f, &tmpVal, val);
        hsa->r9310_hsa_of.dmac[0] = (tmpVal >> 8) & 0xFF;
        hsa->r9310_hsa_of.dmac[1] = (tmpVal >> 0) & 0xFF;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_DMAC_31_0f, &tmpVal, val);
        hsa->r9310_hsa_of.dmac[2] = (tmpVal >> 24) & 0xFF;
        hsa->r9310_hsa_of.dmac[3] = (tmpVal >> 16) & 0xFF;
        hsa->r9310_hsa_of.dmac[4] = (tmpVal >> 8) & 0xFF;
        hsa->r9310_hsa_of.dmac[5] = (tmpVal >> 0) & 0xFF;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SMAC_47_16f, &tmpVal, val);
        hsa->r9310_hsa_of.smac[0] = (tmpVal >> 24) & 0xFF;
        hsa->r9310_hsa_of.smac[1] = (tmpVal >> 16) & 0xFF;
        hsa->r9310_hsa_of.smac[2] = (tmpVal >> 8) & 0xFF;
        hsa->r9310_hsa_of.smac[3] = (tmpVal >> 0) & 0xFF;
        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_SMAC_15_0f, &tmpVal, val);
        hsa->r9310_hsa_of.smac[4] = (tmpVal >> 8) & 0xFF;
        hsa->r9310_hsa_of.smac[5] = (tmpVal >> 0) & 0xFF;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_9_0f, &tmpVal, val);
        hsa->r9310_hsa_of.qid[9].qid = (tmpVal >> 27) & 0x7;
        hsa->r9310_hsa_of.qid[8].qid = (tmpVal >> 24) & 0x7;
        hsa->r9310_hsa_of.qid[7].qid = (tmpVal >> 21) & 0x7;
        hsa->r9310_hsa_of.qid[6].qid = (tmpVal >> 18) & 0x7;
        hsa->r9310_hsa_of.qid[5].qid = (tmpVal >> 15) & 0x7;
        hsa->r9310_hsa_of.qid[4].qid = (tmpVal >> 12) & 0x7;
        hsa->r9310_hsa_of.qid[3].qid = (tmpVal >> 9) & 0x7;
        hsa->r9310_hsa_of.qid[2].qid = (tmpVal >> 6) & 0x7;
        hsa->r9310_hsa_of.qid[1].qid = (tmpVal >> 3) & 0x7;
        hsa->r9310_hsa_of.qid[0].qid = (tmpVal >> 0) & 0x7;

        reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_19_10f, &tmpVal, val);
        hsa->r9310_hsa_of.qid[19].qid = (tmpVal >> 27) & 0x7;
        hsa->r9310_hsa_of.qid[18].qid = (tmpVal >> 24) & 0x7;
        hsa->r9310_hsa_of.qid[17].qid = (tmpVal >> 21) & 0x7;
        hsa->r9310_hsa_of.qid[16].qid = (tmpVal >> 18) & 0x7;
        hsa->r9310_hsa_of.qid[15].qid = (tmpVal >> 15) & 0x7;
        hsa->r9310_hsa_of.qid[14].qid = (tmpVal >> 12) & 0x7;
        hsa->r9310_hsa_of.qid[13].qid = (tmpVal >> 9) & 0x7;
        hsa->r9310_hsa_of.qid[12].qid = (tmpVal >> 6) & 0x7;
        hsa->r9310_hsa_of.qid[11].qid = (tmpVal >> 3) & 0x7;
        hsa->r9310_hsa_of.qid[10].qid = (tmpVal >> 0) & 0x7;

        for (i = 0; i < 57; i++)
        {
            if (i == 56)
            {
                reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_CPUf, &tmpVal, val);
                hsa->r9310_hsa_of.cpu_qid.qid = tmpVal;
            }
            else if ((52 <= i) && (i < 56))
            {
                reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_55_52f, &tmpVal, val);
                shift_bit = (i - 52) * 4;
                hsa->r9310_hsa_of.stack_qid[i - 52].qid = (tmpVal >> shift_bit) & 0xf;
            }
            else
            {
                if (i < 10)
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_9_0f, &tmpVal, val);
                else if ((10 <= i) && (i < 20))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_19_10f, &tmpVal, val);
                else if ((20 <= i) && (i < 30))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_29_20f, &tmpVal, val);
                else if ((30 <= i) && (i < 40))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_39_30f, &tmpVal, val);
                else if ((40 <= i) && (i < 50))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_49_40f, &tmpVal, val);
                else if ((50 <= i) && (i < 52))
                    reg_field_get(unit, MANGO_HSA_DATAr, MANGO_HSA_QID_51_50f, &tmpVal, val);
                shift_bit = (i % 10) * 3;
                hsa->r9310_hsa_of.qid[i].qid = (tmpVal >> shift_bit) & 0x7;
            }
        }
        osal_free(val);
    }
    else
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _rtk_getHsa_openflow */

/* Function Name:
 *      hal_dumpHsa_openflow
 * Description:
 *      Dump OpenFlow mode hsa paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsa_openflow(uint32 unit)
{
    uint32  i;
    hsa_param_t  hsa;
    hal_control_t *pInfo;

    if(RT_ERR_OK != _rtk_getHsa_openflow(unit, &hsa))
    {
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        osal_printf("\n===========================  HSA OpenFlow Mode ========================================\n");

        osal_printf("hsb_eadr: 0x%x\t\t\t", hsa.r9310_hsa_of.hsb_eadr);
        osal_printf("pkt_len: 0x%x\t\t\t", hsa.r9310_hsa_of.pkt_len);
        osal_printf("pg_cnt: 0x%x\n", hsa.r9310_hsa_of.pg_cnt);
        osal_printf("stack_if: 0x%x\t\t\t", hsa.r9310_hsa_of.stack_if);
        osal_printf("cpu_tx_if: 0x%x\t\t\t", hsa.r9310_hsa_of.cpu_tx_if);
        osal_printf("of_if: 0x%x\n", hsa.r9310_hsa_of.of_if);
        osal_printf("of_pipeline: 0x%x\t\t", hsa.r9310_hsa_of.of_pipeline);
        osal_printf("Stack_fmt: 0x%x\t\t\t", hsa.r9310_hsa_of.stack_fmt);
        osal_printf("ori_itag_if: 0x%x\n", hsa.r9310_hsa_of.ori_itag_if);
        osal_printf("ori_otag_if: 0x%x\t\t", hsa.r9310_hsa_of.ori_otag_if);
        osal_printf("ori_itag_vid: 0x%x\t\t", hsa.r9310_hsa_of.ori_itag_vid);
        osal_printf("ori_itag_cfi: 0x%x\n", hsa.r9310_hsa_of.ori_itag_cfi);
        osal_printf("ori_itag_pri: 0x%x\t\t", hsa.r9310_hsa_of.ori_itag_pri);
        osal_printf("ori_itag_tpid: 0x%x\t\t", hsa.r9310_hsa_of.ori_itag_tpid);
        osal_printf("ori_otag_vid: 0x%x\n", hsa.r9310_hsa_of.ori_otag_vid);
        osal_printf("ori_otag_cfi: 0x%x\t\t", hsa.r9310_hsa_of.ori_otag_cfi);
        osal_printf("ori_otag_pri: 0x%x\n", hsa.r9310_hsa_of.ori_otag_pri);
        osal_printf("ori_otag_tpid: 0x%x\t\t", hsa.r9310_hsa_of.ori_otag_tpid);
        osal_printf("class_a: 0x%x\t\t\t", hsa.r9310_hsa_of.class_a);
        osal_printf("class_b: 0x%x\n", hsa.r9310_hsa_of.class_b);
        osal_printf("pppoe_if: 0x%x\t\t\t", hsa.r9310_hsa_of.pppoe_if);
        osal_printf("frame_type: 0x%x\t\t\t", hsa.r9310_hsa_of.frame_type);
        osal_printf("evlan_if: 0x%x\n", hsa.r9310_hsa_of.evlan_if);
        osal_printf("ipv4_if: 0x%x\t\t\t", hsa.r9310_hsa_of.ipv4_if);
        osal_printf("ipv6_if: 0x%x\t\t\t", hsa.r9310_hsa_of.ipv6_if);
        osal_printf("org_ompls_if: 0x%x\n", hsa.r9310_hsa_of.org_ompls_if);
        osal_printf("org_impls_if: 0x%x\t\t", hsa.r9310_hsa_of.org_impls_if);
        osal_printf("parser_cant_handle: 0x%x\t\t", hsa.r9310_hsa_of.parser_cant_handle);
        osal_printf("IP6_EH_HOPBYHOP_ERR: 0x%x\n", hsa.r9310_hsa_of.ip6_eh_hopbyhop_err);
        osal_printf("IP6_EH_HOPBYHOP_EXIST: 0x%x\t", hsa.r9310_hsa_of.ip6_eh_hopbyhop_exist);
        osal_printf("IP6_EH_ROUTE_EXIST: 0x%x\t\t", hsa.r9310_hsa_of.ip6_eh_route_exist);
        osal_printf("IP6_EH_FRAGMENT_EXIST: 0x%x\n", hsa.r9310_hsa_of.ip6_eh_fragment_exist);
        osal_printf("IP6_EH_ESP_EXIST: 0x%x\t\t", hsa.r9310_hsa_of.ip6_eh_esp_exist);
        osal_printf("IP6_EH_AUTH_EXIST: 0x%x\t\t", hsa.r9310_hsa_of.ip6_eh_auth_exist);
        osal_printf("IP6_EH_DESTINATION_EXIST: 0x%x\n", hsa.r9310_hsa_of.ip6_eh_destination_exist);
        osal_printf("IP6_EH_MOBILITY_EXIST: 0x%x\t", hsa.r9310_hsa_of.ip6_eh_mobility_exist);
        osal_printf("IP6_EH_REPEAT: 0x%x\n", hsa.r9310_hsa_of.ip6_eh_repeat);
        osal_printf("ip_prot: 0x%x\t\t\t", hsa.r9310_hsa_of.ip_prot);
        osal_printf("ip_offset_zero: 0x%x\t\t", hsa.r9310_hsa_of.ip_offset_zero);
        osal_printf("ip_flag_set: 0x%x\n", hsa.r9310_hsa_of.ip_flag_set);
        osal_printf("l4_hdr_rdy: 0x%x\t\t\t", hsa.r9310_hsa_of.l4_hdr_rdy);
        osal_printf("l4_offset: 0x%x\t\t\t", hsa.r9310_hsa_of.l4_offset);
        osal_printf("l2_err_pkt: 0x%x\n", hsa.r9310_hsa_of.l2_err_pkt);
        osal_printf("l3_err_pkt: 0x%x\t\t\t", hsa.r9310_hsa_of.l3_err_pkt);
        osal_printf("L3_offset: 0x%x\t\t\t", hsa.r9310_hsa_of.l3_offset);
        osal_printf("in_spn: 0x%x\n", hsa.r9310_hsa_of.in_spn);
        osal_printf("spn: 0x%x\t\t\t", hsa.r9310_hsa_of.spn);
        osal_printf("sp_trk: 0x%x\t\t\t", hsa.r9310_hsa_of.sp_trk);
        osal_printf("sp_is_trk: 0x%x\n", hsa.r9310_hsa_of.sp_is_trk);
        osal_printf("loopback_time: 0x%x\t\t", hsa.r9310_hsa_of.loopback_time);
        osal_printf("meta_data: 0x%x\t\t\t", hsa.r9310_hsa_of.meta_data);
        osal_printf("trk_hash: 0x%x\n", hsa.r9310_hsa_of.trk_hash);
        osal_printf("dp: 0x%x\t\t\t\t", hsa.r9310_hsa_of.dp);
        osal_printf("internal_Priority: 0x%x\t\t", hsa.r9310_hsa_of.internal_priority);
        osal_printf("l3_intf_id: 0x%x\n", hsa.r9310_hsa_of.l3_intf_id);
        osal_printf("tt_idx: 0x%x\t\t\t", hsa.r9310_hsa_of.tt_idx);
        osal_printf("tt_hit: 0x%x\t\t\t", hsa.r9310_hsa_of.tt_hit);
        osal_printf("hsa_sel: 0x%x\n", hsa.r9310_hsa_of.hsa_sel);
        osal_printf("hsa_sadr: 0x%x\t\t\t", hsa.r9310_hsa_of.hsa_sadr);
        osal_printf("hsb_sadr: 0x%x\n", hsa.r9310_hsa_of.hsb_sadr);
        osal_printf("dpm[56:32] : 0x%08x\t\t", hsa.r9310_hsa_of.dpm.bits[1]);
        osal_printf("dpm[31:0] : 0x%08x\t\t", hsa.r9310_hsa_of.dpm.bits[0]);
        osal_printf("L3_TTL: 0x%x\n", hsa.r9310_hsa_of.l3_ttl);
        osal_printf("cp_cnt: 0x%x\t\t\t", hsa.r9310_hsa_of.cp_cnt);
        osal_printf("stk_cpu_fwd: 0x%x\t\t", hsa.r9310_hsa_of.stk_cpu_fwd);
        osal_printf("stk_nm_fwd: 0x%x\n", hsa.r9310_hsa_of.stk_nm_fwd);
        osal_printf("mirroring_info: 0x%x\t\t", hsa.r9310_hsa_of.mirroring_info);
        osal_printf("mirrored_info: 0x%x\t\t", hsa.r9310_hsa_of.mirrored_info);
        osal_printf("mirr_nfwd: 0x%x\n", hsa.r9310_hsa_of.mirr_nfwd);
        osal_printf("local_cpu_fwd: 0x%x\t\t", hsa.r9310_hsa_of.local_cpu_fwd);
        osal_printf("sflow_mirroring: 0x%x\t\t", hsa.r9310_hsa_of.sflow_mirroring);
        osal_printf("sflow_mirrored: 0x%x\n", hsa.r9310_hsa_of.sflow_mirrored);
        osal_printf("dm_pkt: 0x%x\t\t\t", hsa.r9310_hsa_of.dm_pkt);
        osal_printf("dg_pkt: 0x%x\t\t\t", hsa.r9310_hsa_of.dg_pkt);
        osal_printf("tx_mir_cancel: 0x%x\n", hsa.r9310_hsa_of.tx_mir_cancel);
        osal_printf("vb_iso_mbr: 0x%x\t\t\t", hsa.r9310_hsa_of.vb_iso_mbr);
        osal_printf("learn_en: 0x%x\t\t\t", hsa.r9310_hsa_of.learn_en);
        osal_printf("fwd_vid_sel: 0x%x\n", hsa.r9310_hsa_of.fwd_vid_sel);
        osal_printf("fwd_type: 0x%x\t\t\t", hsa.r9310_hsa_of.fwd_type);
        osal_printf("RVID: 0x%x\t\t\t", hsa.r9310_hsa_of.rvid);
        osal_printf("local_reason: 0x%x\n", hsa.r9310_hsa_of.local_reason);
        osal_printf("remote_reason: 0x%x\t\t", hsa.r9310_hsa_of.remote_reason);
        osal_printf("output_data: 0x%x\t\t", hsa.r9310_hsa_of.output_data);
        osal_printf("output_type: 0x%x\n", hsa.r9310_hsa_of.output_type);
        osal_printf("group_idx: 0x%x\t\t\t", hsa.r9310_hsa_of.group_idx);
        osal_printf("group: 0x%x\t\t\t", hsa.r9310_hsa_of.group);
        osal_printf("queue_data: 0x%x\n", hsa.r9310_hsa_of.queue_data);
        osal_printf("set_queue_data: 0x%x\t\t", hsa.r9310_hsa_of.set_queue_data);
        osal_printf("field_l4_dport: 0x%x\t\t", hsa.r9310_hsa_of.field_l4_dport);
        osal_printf("set_field_l4_dport: 0x%x\n", hsa.r9310_hsa_of.set_field_l4_dport);
        osal_printf("field_l4_sport: 0x%x\t\t", hsa.r9310_hsa_of.field_l4_sport);
        osal_printf("set_field_l4_sport: 0x%x\t\t", hsa.r9310_hsa_of.set_field_l4_sport);
        osal_printf("field_ip_dip: 0x%x\t\t", hsa.r9310_hsa_of.field_ip_dip);
        osal_printf("set_ip_rsv: 0x%x\t\t\t", hsa.r9310_hsa_of.set_ip_rsv);
        osal_printf("set_field_ip_dip: 0x%x\n", hsa.r9310_hsa_of.set_field_ip_dip);
        osal_printf("field_ip_sip: 0x%x\t\t", hsa.r9310_hsa_of.field_ip_sip);
        osal_printf("set_field_ip_sip: 0x%x\t\t", hsa.r9310_hsa_of.set_field_ip_sip);
        osal_printf("field_ip_ttl: 0x%x\n", hsa.r9310_hsa_of.field_ip_ttl);
        osal_printf("set_field_ip_ttl: 0x%x\t\t", hsa.r9310_hsa_of.set_field_ip_ttl);
        osal_printf("field_ip_dscp: 0x%x\t\t", hsa.r9310_hsa_of.field_ip_dscp);
        osal_printf("set_field_ip_dscp: 0x%x\n", hsa.r9310_hsa_of.set_field_ip_dscp);
        osal_printf("field_mpls_ttl: 0x%x\t\t", hsa.r9310_hsa_of.field_mpls_ttl);
        osal_printf("set_field_mpls_ttl: 0x%x\t\t", hsa.r9310_hsa_of.set_field_mpls_ttl);
        osal_printf("field_mpls_tc: 0x%x\n", hsa.r9310_hsa_of.field_mpls_tc);
        osal_printf("set_field_mpls_tc: 0x%x\t\t", hsa.r9310_hsa_of.set_field_mpls_tc);
        osal_printf("field_mpls_label_idx: 0x%x\t", hsa.r9310_hsa_of.field_mpls_label);
        osal_printf("set_field_mpls_label: 0x%x\n", hsa.r9310_hsa_of.set_field_mpls_label);
        osal_printf("field_pcp: 0x%x\t\t\t", hsa.r9310_hsa_of.field_pcp);
        osal_printf("set_field_pcp: 0x%x\t\t", hsa.r9310_hsa_of.set_field_pcp);
        osal_printf("field_vid: 0x%x\n", hsa.r9310_hsa_of.field_vid);
        osal_printf("set_field_vid: 0x%x\t\t", hsa.r9310_hsa_of.set_field_vid);
        osal_printf("field_da_index: %d\t\t", hsa.r9310_hsa_of.field_da);
        osal_printf("set_field_da: 0x%x\n", hsa.r9310_hsa_of.set_field_da);
        osal_printf("field_sa_index: %d\t\t", hsa.r9310_hsa_of.field_sa);
        osal_printf("set_field_sa: 0x%x\t", hsa.r9310_hsa_of.set_field_sa);
        osal_printf("dec_ip_ttl: 0x%x\n", hsa.r9310_hsa_of.dec_ip_ttl);
        osal_printf("dec_mpls_ttl: 0x%x\t\t", hsa.r9310_hsa_of.dec_mpls_ttl);
        osal_printf("copy_ttl_outward: 0x%x\t\t", hsa.r9310_hsa_of.copy_ttl_outward);
        osal_printf("push_vlan_ethtype: 0x%x\n", hsa.r9310_hsa_of.push_vlan_ethtype);
        osal_printf("push_vlan: 0x%x\t\t\t", hsa.r9310_hsa_of.push_vlan);
        osal_printf("push_mpls_ethtype: 0x%x\t\t", hsa.r9310_hsa_of.push_mpls_ethtype);
        osal_printf("push_mpls_lib_idx: 0x%x\n", hsa.r9310_hsa_of.push_mpls_lib_idx);
        osal_printf("push_mpls_lib_idx: 0x%x\t\t", hsa.r9310_hsa_of.push_mpls_lib_idx);
        osal_printf("push_mpls_vpn_type: 0x%x\t\t", hsa.r9310_hsa_of.push_mpls_vpn_type);
        osal_printf("push_mpls_mode: 0x%x\n", hsa.r9310_hsa_of.push_mpls_mode);
        osal_printf("push_mpls: 0x%x\t\t\t", hsa.r9310_hsa_of.push_mpls);
        osal_printf("pop_mpls_type: 0x%x\t\t", hsa.r9310_hsa_of.pop_mpls_type);
        osal_printf("pop_mpls: 0x%x\n", hsa.r9310_hsa_of.pop_mpls);
        osal_printf("pop_vlan: 0x%x\t\t\t", hsa.r9310_hsa_of.pop_vlan);
        osal_printf("copy_ttl_inward: 0x%x\t\t", hsa.r9310_hsa_of.copy_ttl_inward);
        osal_printf("to_OF: 0x%x\n", hsa.r9310_hsa_of.to_of);
        osal_printf("apply_action: 0x%x\t\t", hsa.r9310_hsa_of.apply_action);
        osal_printf("of_lookup_req: 0x%x\t\t", hsa.r9310_hsa_of.of_lookup_req);
        osal_printf("next_tab: 0x%x\n", hsa.r9310_hsa_of.next_tab);
        osal_printf("pkt_dscp: 0x%x\t\t", hsa.r9310_hsa_of.pkt_dscp);
        osal_printf("lb_des_time: 0x%x\n", hsa.r9310_hsa_of.lb_des_time);
        osal_printf("mac_cst: 0x%x\t\t\t", hsa.r9310_hsa_of.mac_cst);
        osal_printf("acl_idx: 0x%x\t\t\t", hsa.r9310_hsa_of.acl_idx);
        osal_printf("group_hash: 0x%x\n", hsa.r9310_hsa_of.group_hash);
        osal_printf("cpu_rx_l2_err: 0x%x\t\t", hsa.r9310_hsa_of.cpu_rx_l2_err);
        osal_printf("cpu_rx_l3_err: 0x%x\t\t", hsa.r9310_hsa_of.cpu_rx_l3_err);
        osal_printf("cpu_rx_sflow: 0x%x\n", hsa.r9310_hsa_of.cpu_rx_sflow);
        osal_printf("of_tbl_id: 0x%x\t\t\t", hsa.r9310_hsa_of.of_tbl_id);
        osal_printf("acl_of_hit: 0x%x\n", hsa.r9310_hsa_of.acl_of_hit);
        osal_printf("of_miss_tbl_id: 0x%x\n", hsa.r9310_hsa_of.of_miss_tbl_id);
        osal_printf("dmac[6]=%02x:%02x:%02x:%02x:%02x:%02x\n", hsa.r9310_hsa_of.dmac[0],hsa.r9310_hsa_of.dmac[1], hsa.r9310_hsa_of.dmac[2], hsa.r9310_hsa_of.dmac[3], hsa.r9310_hsa_of.dmac[4], hsa.r9310_hsa_of.dmac[5]);
        osal_printf("smac[6]=%02x:%02x:%02x:%02x:%02x:%02x\n", hsa.r9310_hsa_of.smac[0],hsa.r9310_hsa_of.smac[1], hsa.r9310_hsa_of.smac[2], hsa.r9310_hsa_of.smac[3], hsa.r9310_hsa_of.smac[4], hsa.r9310_hsa_of.smac[5]);

        for (i = 0; i < 52; i++)
        {
            if (0 != ((i+1) % 3))
                osal_printf("qid[%02u] : %d\t\t\t", i, hsa.r9310_hsa_of.qid[i].qid);
            else
                osal_printf("qid[%02u] : %d\n", i, hsa.r9310_hsa_of.qid[i].qid);
        }
        osal_printf("\n");  /* end of QID */

        osal_printf("stack_qid[52] : %02d\t\t", hsa.r9310_hsa_of.stack_qid[0].qid);
        osal_printf("stack_qid[53] : %02d\t\t", hsa.r9310_hsa_of.stack_qid[1].qid);
        osal_printf("stack_qid[54] : %02d\n", hsa.r9310_hsa_of.stack_qid[2].qid);
        osal_printf("stack_qid[55] : %02d\t\t", hsa.r9310_hsa_of.stack_qid[3].qid);
        osal_printf("cpu_qid[56] : %02d\t\t", hsa.r9310_hsa_of.cpu_qid.qid);
        osal_printf("L2mpls_tt: 0x%x\t\t\n", hsa.r9310_hsa_of.l2mpls_tt);

        osal_printf("=======================================================================================\n");
    }
    else
#endif

    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of hal_dumpHsa */
#endif


#if defined(CONFIG_SDK_RTL8390)
int32 _hal_getHsm(uint32 unit, hsm_param_t *hsm)
{
    uint32 val = 0, tmpVal = 0;
    hal_control_t *pInfo;

    if(NULL == hsm)
    {
        return RT_ERR_FAILED;
    }

    osal_memset(hsm, 0, sizeof(hsm_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        /* HSM PRE */
        /* HSM_PRE_DATA0 */
        reg_read(unit, CYPRESS_HSM_PRE_DATA0r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_PPPOEf, &tmpVal, &val);
        hsm->r8390.pre_data.pppoe_ef = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_CMACf, &tmpVal, &val);
        hsm->r8390.pre_data.gmac = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_GMACf, &tmpVal, &val);
        hsm->r8390.pre_data.cmac = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_IPV4_HDR_ERRf, &tmpVal, &val);
        hsm->r8390.pre_data.ipv4_hdr_err = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_IPV4_TTL_ZEROf, &tmpVal, &val);
        hsm->r8390.pre_data.ipv4_ttl_zero = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_IPV4_OPTf, &tmpVal, &val);
        hsm->r8390.pre_data.ipv4_opt = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_IPV6_HDR_ERRf, &tmpVal, &val);
        hsm->r8390.pre_data.ipv4_hdr_err = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_IPV6_HL_ZEROf, &tmpVal, &val);
        hsm->r8390.pre_data.ipv6_hl_zero = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_IPV6_HOPf, &tmpVal, &val);
        hsm->r8390.pre_data.ipv6_hop = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_OAM_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.oam_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_STPf, &tmpVal, &val);
        hsm->r8390.pre_data.stp = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_VLAN_ZEROf, &tmpVal, &val);
        hsm->r8390.pre_data.vlan_zero = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_VLAN_SPMf, &tmpVal, &val);
        hsm->r8390.pre_data.vlan_spm = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_UNKEYf, &tmpVal, &val);
        hsm->r8390.pre_data.unkey = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_MCKEYf, &tmpVal, &val);
        hsm->r8390.pre_data.mckey = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_VLAN_PROFILEf, &tmpVal, &val);
        hsm->r8390.pre_data.vlan_profile = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_FIDf, &tmpVal, &val);
        hsm->r8390.pre_data.fid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_METER_DROPf, &tmpVal, &val);
        hsm->r8390.pre_data.meter_drop = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA0r, CYPRESS_CFM_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.cfm_act = tmpVal;

        /* HSM_PRE_DATA1 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA1r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA1r, CYPRESS_CFM_IDXf, &tmpVal, &val);
        hsm->r8390.pre_data.cfm_idx = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA1r, CYPRESS_CFI_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.cfi_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA1r, CYPRESS_TAG_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.tag_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA1r, CYPRESS_NIVIDf, &tmpVal, &val);
        hsm->r8390.pre_data.nivid = tmpVal;

        /* HSM_PRE_DATA2 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA2r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA2r, CYPRESS_ACL_BYPASSf, &tmpVal, &val);
        hsm->r8390.pre_data.acl_bypass = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA2r, CYPRESS_NOVIDf, &tmpVal, &val);
        hsm->r8390.pre_data.novid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA2r, CYPRESS_RVIDf, &tmpVal, &val);
        hsm->r8390.pre_data.rvid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA2r, CYPRESS_ACL_MEETf, &tmpVal, &val);
        hsm->r8390.pre_data.acl_meet = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA2r, CYPRESS_ACLC2SCf, &tmpVal, &val);
        hsm->r8390.pre_data.aclc2sc = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA2r, CYPRESS_ACL_HITf, &tmpVal, &val);
        hsm->r8390.pre_data.acl_hit = tmpVal;

        /* HSM_PRE_DATA3 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA3r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA3r, CYPRESS_ACL_IDXf, &tmpVal, &val);
        hsm->r8390.pre_data.acl_idx = tmpVal;

        /* HSM_PRE_DATA4 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA4r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA4r, CYPRESS_ACL_DATAf, &tmpVal, &val);
        hsm->r8390.pre_data.acl_data = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA4r, CYPRESS_ORG_IVIF_IFf, &tmpVal, &val);
        hsm->r8390.pre_data.org_ivif_if = tmpVal;

        /* HSM_PRE_DATA5 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA5r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_ORG_IVIDf, &tmpVal, &val);
        hsm->r8390.pre_data.org_ivid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_SPE_TRAP_RSNf, &tmpVal, &val);
        hsm->r8390.pre_data.spe_trap_rsn = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_SPE_TRAP_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.spe_trap_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_EAV_CLASSAf, &tmpVal, &val);
        hsm->r8390.pre_data.eav_classa = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_EAV_CLASSBf, &tmpVal, &val);
        hsm->r8390.pre_data.eav_classb = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_HWATT_RSNf, &tmpVal, &val);
        hsm->r8390.pre_data.hwatt_rsn = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_HWATT_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.hwatt_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_RMA_PKTf, &tmpVal, &val);
        hsm->r8390.pre_data.rma_pkt= tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_RMA_ACTf, &tmpVal, &val);
        hsm->r8390.pre_data.rma_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_RMA_LEARNf, &tmpVal, &val);
        hsm->r8390.pre_data.rma_learn = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_RMA_BYPASSf, &tmpVal, &val);
        hsm->r8390.pre_data.rma_bypass = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_C2SC_HIT_MODEf, &tmpVal, &val);
        hsm->r8390.pre_data.c2sc_hit_mode = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA5r, CYPRESS_C2SC_HITf, &tmpVal, &val);
        hsm->r8390.pre_data.c2sc_hit = tmpVal;

        /* HSM_PRE_DATA6 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA6r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA6r, CYPRESS_C2SC_DATAf, &tmpVal, &val);
        hsm->r8390.pre_data.c2sc_data = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA6r, CYPRESS_INTERNAL_PRIOf, &tmpVal, &val);
        hsm->r8390.pre_data.internal_prio = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA6r, CYPRESS_DP_PREf, &tmpVal, &val);
        hsm->r8390.pre_data.dp_pre = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA6r, CYPRESS_PROTOCOL_STORMf, &tmpVal, &val);
        hsm->r8390.pre_data.protocol_storm = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA6r, CYPRESS_IBW_PASSf, &tmpVal, &val);
        hsm->r8390.pre_data.ibw_pass = tmpVal;

        /* HSM_PRE_DATA7 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA7r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA7r, CYPRESS_SLIDf, &tmpVal, &val);
        hsm->r8390.pre_data.slid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA7r, CYPRESS_TRUNK_GRPf, &tmpVal, &val);
        hsm->r8390.pre_data.trunk_grp = tmpVal;

        /* HSM_PRE_DATA8 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA8r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA8r, CYPRESS_SIPf, &tmpVal, &val);
        hsm->r8390.pre_data.sip = tmpVal;

        /* HSM_PRE_DATA9 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA9r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA9r, CYPRESS_DIPf, &tmpVal, &val);
        hsm->r8390.pre_data.dip = tmpVal;

        /* HSM_PRE_DATA10 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA10r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA10r, CYPRESS_IPV6f, &tmpVal, &val);
        hsm->r8390.pre_data.ipv6 = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA10r, CYPRESS_IPV4f, &tmpVal, &val);
        hsm->r8390.pre_data.ipv4 = tmpVal;

        /* HSM_PRE_DATA11 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_PRE_DATA11r, CYPRESS_SMAC_47_32f, &val);
        hsm->r8390.pre_data.smac[0] = (uint8)(val >> 8) & 0xff;
        hsm->r8390.pre_data.smac[1] = (uint8)(val >> 0) & 0xff;

        /* HSM_PRE_DATA12 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_PRE_DATA12r, CYPRESS_SMAC_31_0f, &val);
        hsm->r8390.pre_data.smac[2] = (uint8)(val >> 24) & 0xff;
        hsm->r8390.pre_data.smac[3] = (uint8)(val >> 16) & 0xff;
        hsm->r8390.pre_data.smac[4] = (uint8)(val >> 8) & 0xff;
        hsm->r8390.pre_data.smac[5] = (uint8)(val >> 0) & 0xff;

        /* HSM_PRE_DATA13 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_PRE_DATA13r, CYPRESS_DMAC_47_32f, &val);
        hsm->r8390.pre_data.dmac[0] = (val >> 8) & 0xff;
        hsm->r8390.pre_data.dmac[1] = (val >> 0) & 0xff;

        /* HSM_PRE_DATA14 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_PRE_DATA14r, CYPRESS_DMAC_31_0f, &val);
        hsm->r8390.pre_data.dmac[2] = (val >> 24) & 0xff;
        hsm->r8390.pre_data.dmac[3] = (val >> 16) & 0xff;
        hsm->r8390.pre_data.dmac[4] = (val >> 8) & 0xff;
        hsm->r8390.pre_data.dmac[5] = (val >> 0) & 0xff;

        /* HSM_PRE_DATA15 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_PRE_DATA15r, &val);
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA15r, CYPRESS_CTAG_LNf, &tmpVal, &val);
        hsm->r8390.pre_data.ctag_ln = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA15r, CYPRESS_CTAG_IFf, &tmpVal, &val);
        hsm->r8390.pre_data.ctag_if = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_PRE_DATA15r, CYPRESS_PKTLENf, &tmpVal, &val);
        hsm->r8390.pre_data.pktlen = tmpVal;

        /* HSM LU */
        /* HSM_LU_DATA0 */
        reg_read(unit, CYPRESS_HSM_LU_DATA0r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA0r, CYPRESS_ACL_BYPASSf, &tmpVal, &val);
        hsm->r8390.lu_data.acl_bypass = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA0r, CYPRESS_DPORT_MOVEf, &tmpVal, &val);
        hsm->r8390.lu_data.dpmove_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA0r, CYPRESS_SPORT_FILTER_ENf, &tmpVal, &val);
        hsm->r8390.lu_data.sport_filter_en = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA0r, CYPRESS_GMAC_ERRf, &tmpVal, &val);
        hsm->r8390.lu_data.gmac_err = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA0r, CYPRESS_MC_SAf, &tmpVal, &val);
        hsm->r8390.lu_data.mc_sa = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA0r, CYPRESS_VLAN_SPFILTERf, &tmpVal, &val);
        hsm->r8390.lu_data.vlan_spfilter = tmpVal;

        /* HSM_LU_DATA1 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA1r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA1r, CYPRESS_DPM_52_32f, &tmpVal, &val);
        hsm->r8390.lu_data.dpm.bits[1] = tmpVal;

        /* HSM_LU_DATA2 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA2r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA2r, CYPRESS_DPM_31_0f, &tmpVal, &val);
        hsm->r8390.lu_data.dpm.bits[0] = tmpVal;

        /* HSM_LU_DATA3 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA3r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA3r, CYPRESS_LOOKUP_TYPEf, &tmpVal, &val);
        hsm->r8390.lu_data.lookup_type = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA3r, CYPRESS_FWD_TYPEf, &tmpVal, &val);
        hsm->r8390.lu_data.fwd_type = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA3r, CYPRESS_NEWSA_ACf, &tmpVal, &val);
        hsm->r8390.lu_data.newsa_ac = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA3r, CYPRESS_PMOVE_ACTf, &tmpVal, &val);
        hsm->r8390.lu_data.pmove_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA3r, CYPRESS_MACLIMIT_ACTf, &tmpVal, &val);
        hsm->r8390.lu_data.maclimit_act = tmpVal;

        /* HSM_LU_DATA4 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA4r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA4r, CYPRESS_ROUTE_DATAf, &tmpVal, &val);
        hsm->r8390.lu_data.route_data = tmpVal;

        /* HSM_LU_DATA5 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA5r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA5r, CYPRESS_DA_HITf, &tmpVal, &val);
        hsm->r8390.lu_data.da_hit = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA5r, CYPRESS_DA_DATAf, &tmpVal, &val);
        hsm->r8390.lu_data.da_data = tmpVal;

        /* HSM_LU_DATA6 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA6r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA6r, CYPRESS_SA_HITf, &tmpVal, &val);
        hsm->r8390.lu_data.sa_hit = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA6r, CYPRESS_SA_DATAf, &tmpVal, &val);
        hsm->r8390.lu_data.sa_data = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA6r, CYPRESS_CMACf, &tmpVal, &val);
        hsm->r8390.lu_data.cmac = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA6r, CYPRESS_GMACf, &tmpVal, &val);
        hsm->r8390.lu_data.gmac = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA6r, CYPRESS_IPV4_HDR_ERRf, &tmpVal, &val);
        hsm->r8390.lu_data.ipv4_hdr_err = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA6r, CYPRESS_IPV4_OPTf, &tmpVal, &val);
        hsm->r8390.lu_data.ipv4_opt = tmpVal;

        /* HSM_LU_DATA7 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA7r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_IPV4_TTL_ZEROf, &tmpVal, &val);
        hsm->r8390.lu_data.ipv4_ttl_zero = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_IPV6_HDR_ERRf, &tmpVal, &val);
        hsm->r8390.lu_data.ipv6_hdr_err = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_IPV6_HL_ZEROf, &tmpVal, &val);
        hsm->r8390.lu_data.ipv6_hl_zero = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_IPV6_HOPf, &tmpVal, &val);
        hsm->r8390.lu_data.ipv6_hop = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_OAM_ACTf, &tmpVal, &val);
        hsm->r8390.lu_data.oam_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_STPf, &tmpVal, &val);
        hsm->r8390.lu_data.stp = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_VLAN_ZEROf, &tmpVal, &val);
        hsm->r8390.lu_data.vlan_zero = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_VLAN_SPMf, &tmpVal, &val);
        hsm->r8390.lu_data.vlan_spm = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_FIDf, &tmpVal, &val);
        hsm->r8390.lu_data.fid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_METER_DROPf, &tmpVal, &val);
        hsm->r8390.lu_data.meter_drop = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_CFM_ACTf, &tmpVal, &val);
        hsm->r8390.lu_data.cfm_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_CFM_IDXf, &tmpVal, &val);
        hsm->r8390.lu_data.cfm_idx = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_CFI_ACTf, &tmpVal, &val);
        hsm->r8390.lu_data.cfi_act = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA7r, CYPRESS_TAG_ACTf, &tmpVal, &val);
        hsm->r8390.lu_data.tag_act = tmpVal;

        /* HSM_LU_DATA8 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA8r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA8r, CYPRESS_NIVIDf, &tmpVal, &val);
        hsm->r8390.lu_data.nivid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA8r, CYPRESS_NOVIDf, &tmpVal, &val);
        hsm->r8390.lu_data.novid = tmpVal;

        /* HSM_LU_DATA9 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA9r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA9r, CYPRESS_RVIDf, &tmpVal, &val);
        hsm->r8390.lu_data.rvid = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA9r, CYPRESS_ACL_MEETf, &tmpVal, &val);
        hsm->r8390.lu_data.acl_meet = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA9r, CYPRESS_ACLC2SCf, &tmpVal, &val);
        hsm->r8390.lu_data.aclc2sc = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA9r, CYPRESS_ACL_HITf, &tmpVal, &val);
        hsm->r8390.lu_data.acl_hit = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA9r, CYPRESS_ACL_IDXf, &tmpVal, &val);
        hsm->r8390.lu_data.acl_idx = tmpVal;

        /* HSM_LU_DATA10 */
        val = 0;
        reg_read(unit, CYPRESS_HSM_LU_DATA10r, &val);
        reg_field_get(unit, CYPRESS_HSM_LU_DATA10r, CYPRESS_ACL_DATAf, &tmpVal, &val);
        hsm->r8390.lu_data.acl_data = tmpVal;
        reg_field_get(unit, CYPRESS_HSM_LU_DATA10r, CYPRESS_SPE_TRAP_RSNf, &tmpVal, &val);
        hsm->r8390.lu_data.spe_trap_rsn = tmpVal;

        /* HSM_LU_DATA11 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_RMA_BYP_VLANf, &val);
        hsm->r8390.lu_data.rma_byp_vlan = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_SPE_TRAP_ACTf, &val);
        hsm->r8390.lu_data.spe_trap_act = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_EAV_CLASSAf, &val);
        hsm->r8390.lu_data.eav_classa = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_EAV_CLASSBf, &val);
        hsm->r8390.lu_data.eav_classb = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_HWATT_RSNf, &val);
        hsm->r8390.lu_data.hwatt_rsn = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_HWATT_ACTf, &val);
        hsm->r8390.lu_data.hwatt_act = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_RMA_PKTf, &val);
        hsm->r8390.lu_data.rma_pkt = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_RMA_ACTf, &val);
        hsm->r8390.lu_data.rma_act = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_RMA_BYPASSf, &val);
        hsm->r8390.lu_data.rma_bypass = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_C2SC_HIT_MODEf, &val);
        hsm->r8390.lu_data.c2sc_hit_mode = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA11r, CYPRESS_C2SC_HITf, &val);
        hsm->r8390.lu_data.c2sc_hit = tmpVal;

        /* HSM_LU_DATA12 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA12r, CYPRESS_C2SC_DATAf, &val);
        hsm->r8390.lu_data.c2sc_data = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA12r, CYPRESS_INTERNAL_PRIOf, &val);
        hsm->r8390.lu_data.internal_prio = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA12r, CYPRESS_DP_PREf, &val);
        hsm->r8390.lu_data.dp_pre = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA12r, CYPRESS_PROTOCOL_STORMf, &val);
        hsm->r8390.lu_data.protocol_storm = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA12r, CYPRESS_IBW_PASSf, &val);
        hsm->r8390.lu_data.ibw_pass = tmpVal;

        /* HSM_LU_DATA13 */
        val = 0;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA13r, CYPRESS_SLIDf, &val);
        hsm->r8390.lu_data.slid = tmpVal;
        reg_field_read(unit, CYPRESS_HSM_LU_DATA13r, CYPRESS_TRUNK_GRPf, &val);
        hsm->r8390.lu_data.trunk_grp = tmpVal;
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _hal_getHsm */
#endif

#if defined(CONFIG_SDK_RTL8390)
int32 _hal_dumpHsm_8390(uint32 unit)
{
    hsm_param_t hsm;

    if (RT_ERR_OK != _hal_getHsm(unit, &hsm))
    {
        return RT_ERR_FAILED;
    }

    osal_printf("\n=========================HSM PRE=============================\n");
    osal_printf("PPPOE : 0x%x\t\t", hsm.r8390.pre_data.pppoe_ef);
    osal_printf("CMAC : 0x%x\t\t", hsm.r8390.pre_data.cmac);
    osal_printf("GMAC : 0x%x\n", hsm.r8390.pre_data.gmac);

    osal_printf("IPV4_HDR_ERR : 0x%x\t", hsm.r8390.pre_data.ipv4_hdr_err);
    osal_printf("IPV4_TTL_ZERO : 0x%x\t", hsm.r8390.pre_data.ipv4_ttl_zero);
    osal_printf("IPV4_OPT : 0x%x\n", hsm.r8390.pre_data.ipv4_opt);

    osal_printf("IPV6_HDR_ERR : 0x%x\t", hsm.r8390.pre_data.ipv6_hdr_err);
    osal_printf("IPV6_HL_ZERO : 0x%x\t", hsm.r8390.pre_data.ipv6_hl_zero);
    osal_printf("IPV6_HOP : 0x%x\n", hsm.r8390.pre_data.ipv6_hop);

    osal_printf("OAM_ACT : 0x%x\t\t", hsm.r8390.pre_data.oam_act);
    osal_printf("STP : 0x%x\t\t", hsm.r8390.pre_data.stp);
    osal_printf("VLAN_ZERO : 0x%x\n", hsm.r8390.pre_data.vlan_zero);

    osal_printf("VLAN_SPM : 0x%x\t\t", hsm.r8390.pre_data.vlan_spm);
    osal_printf("UNKEY : 0x%x\t\t", hsm.r8390.pre_data.unkey);
    osal_printf("MCKEY : 0x%x\n", hsm.r8390.pre_data.mckey);

    osal_printf("VLAN_PROFILE : 0x%x\t", hsm.r8390.pre_data.vlan_profile);
    osal_printf("FID : 0x%x\t\t", hsm.r8390.pre_data.fid);
    osal_printf("METER_DROP : 0x%x\n", hsm.r8390.pre_data.meter_drop);

    osal_printf("CFM_ACT : 0x%x\t\t", hsm.r8390.pre_data.cfm_act);
    osal_printf("CFM_IDX : 0x%x\t\t", hsm.r8390.pre_data.cfm_idx);
    osal_printf("CFI_ACT : 0x%x\n", hsm.r8390.pre_data.cfi_act);

    osal_printf("TAG_ACT : 0x%x\t\t", hsm.r8390.pre_data.tag_act);
    osal_printf("NIVID : 0x%x\t\t", hsm.r8390.pre_data.nivid);
    osal_printf("ACL_BYPASS : 0x%x\n", hsm.r8390.pre_data.acl_bypass);

    osal_printf("NOVID : 0x%x\t\t", hsm.r8390.pre_data.novid);
    osal_printf("RVID : 0x%x\t\t", hsm.r8390.pre_data.rvid);
    osal_printf("ACL_MEET : 0x%x\n", hsm.r8390.pre_data.acl_meet);

    osal_printf("ACLC2SC : 0x%x\t\t", hsm.r8390.pre_data.aclc2sc);
    osal_printf("ACL_HIT : 0x%x\t\t", hsm.r8390.pre_data.acl_hit);
    osal_printf("ACL_IDX : 0x%x\n", hsm.r8390.pre_data.acl_idx);

    osal_printf("ACL_DATA : 0x%x\t\t", hsm.r8390.pre_data.acl_data);
    osal_printf("ORG_IVIF_IF : 0x%x\t", hsm.r8390.pre_data.org_ivif_if);
    osal_printf("ORG_IVID : 0x%x\n", hsm.r8390.pre_data.org_ivid);

    osal_printf("SPE_TRAP_RSN : 0x%x\t", hsm.r8390.pre_data.spe_trap_rsn);
    osal_printf("SPE_TRAP_ACT : 0x%x\t", hsm.r8390.pre_data.spe_trap_act);
    osal_printf("EAV_CLASSA : 0x%x\n", hsm.r8390.pre_data.eav_classa);

    osal_printf("EAV_CLASSB : 0x%x\t", hsm.r8390.pre_data.eav_classb);
    osal_printf("HWATT_RSN : 0x%x\t\t", hsm.r8390.pre_data.hwatt_rsn);
    osal_printf("HWATT_ACT : 0x%x\n", hsm.r8390.pre_data.hwatt_act);

    osal_printf("RMA_PKT : 0x%x\t\t", hsm.r8390.pre_data.rma_pkt);
    osal_printf("RMA_ACT : 0x%x\t\t", hsm.r8390.pre_data.rma_act);
    osal_printf("RMA_LEARN : 0x%x\n", hsm.r8390.pre_data.rma_learn);

    osal_printf("RMA_BYPASS : 0x%x\t", hsm.r8390.pre_data.rma_bypass);
    osal_printf("C2SC_HIT_MODE : 0x%x\t", hsm.r8390.pre_data.c2sc_hit_mode);
    osal_printf("C2SC_HIT : 0x%x\n", hsm.r8390.pre_data.c2sc_hit);

    osal_printf("C2SC_DATA : 0x%x\t\t", hsm.r8390.pre_data.c2sc_data);
    osal_printf("INTERNAL_PRIO : 0x%x\t", hsm.r8390.pre_data.internal_prio);
    osal_printf("DP_PRE : 0x%x\n", hsm.r8390.pre_data.dp_pre);

    osal_printf("PROTOCOL_STORM : 0x%x\t", hsm.r8390.pre_data.protocol_storm);
    osal_printf("IBW_PASS : 0x%x\t\t", hsm.r8390.pre_data.ibw_pass);
    osal_printf("SLID : 0x%x\n", hsm.r8390.pre_data.slid);

    osal_printf("TRUNK_GRP : 0x%x\t\t", hsm.r8390.pre_data.trunk_grp);
    osal_printf("SIP : 0x%x\t\t", hsm.r8390.pre_data.sip);
    osal_printf("DIP : 0x%x\n", hsm.r8390.pre_data.dip);

    osal_printf("IPv6 : 0x%x\t\t", hsm.r8390.pre_data.ipv6);
    osal_printf("IPv4 : 0x%x\t\t", hsm.r8390.pre_data.ipv4);
    osal_printf("CTAG_LN : 0x%x\n", hsm.r8390.pre_data.ctag_ln);
    osal_printf("CTAG_IF : 0x%x\t\t", hsm.r8390.pre_data.ctag_if);
    osal_printf("PKTLEN : 0x%x\n", hsm.r8390.pre_data.pktlen);

    osal_printf("SMAC : %02x:%02x:%02x:%02x:%02x:%02x\n", hsm.r8390.pre_data.smac[0], hsm.r8390.pre_data.smac[1], hsm.r8390.pre_data.smac[2], hsm.r8390.pre_data.smac[3], hsm.r8390.pre_data.smac[4], hsm.r8390.pre_data.smac[5]);
    osal_printf("DMAC : %02x:%02x:%02x:%02x:%02x:%02x\n", hsm.r8390.pre_data.dmac[0], hsm.r8390.pre_data.dmac[1], hsm.r8390.pre_data.dmac[2], hsm.r8390.pre_data.dmac[3], hsm.r8390.pre_data.dmac[4], hsm.r8390.pre_data.dmac[5]);
    osal_printf("=============================================================\n");

    osal_printf("\n=========================HSM LU==============================\n");
    osal_printf("ACL_BYPASS : 0x%x\t", hsm.r8390.lu_data.acl_bypass);
    osal_printf("DPORT_MOVE : 0x%x\t", hsm.r8390.lu_data.dpmove_act);
    osal_printf("SPORT_FILTER_EN : 0x%x\n", hsm.r8390.lu_data.sport_filter_en);

    osal_printf("GMAC_ERR : 0x%x\t\t", hsm.r8390.lu_data.gmac_err);
    osal_printf("MC_SA : 0x%x\t\t", hsm.r8390.lu_data.mc_sa);
    osal_printf("VLAN_SPFILTER : 0x%x\n", hsm.r8390.lu_data.vlan_spfilter);

    osal_printf("DPM[52:32] : 0x%x\t", hsm.r8390.lu_data.dpm.bits[1]);
    osal_printf("DPM[31: 0] : 0x%x\t", hsm.r8390.lu_data.dpm.bits[0]);
    osal_printf("LOOKUP_TYPE : 0x%x\n", hsm.r8390.lu_data.lookup_type);

    osal_printf("FWD_TYPE : 0x%x\t\t", hsm.r8390.lu_data.fwd_type);
    osal_printf("NEWSA_AC : 0x%x\t\t", hsm.r8390.lu_data.newsa_ac);
    osal_printf("PMOVE_ACT : 0x%x\n", hsm.r8390.lu_data.pmove_act);

    osal_printf("MACLIMIT_ACT : 0x%x\t", hsm.r8390.lu_data.maclimit_act);
    osal_printf("ROUTE_DATA : 0x%x\t", hsm.r8390.lu_data.route_data);
    osal_printf("DA_HIT : 0x%x\n", hsm.r8390.lu_data.da_hit);

    osal_printf("DA_DATA : 0x%x\t\t", hsm.r8390.lu_data.da_data);
    osal_printf("SA_HIT : 0x%x\t\t", hsm.r8390.lu_data.sa_hit);
    osal_printf("SA_DATA : 0x%x\n", hsm.r8390.lu_data.sa_data);

    osal_printf("CMAC : 0x%x\t\t", hsm.r8390.lu_data.cmac);
    osal_printf("GMAC : 0x%x\t\t", hsm.r8390.lu_data.gmac);
    osal_printf("IPV4_HDR_ERR : 0x%x\n", hsm.r8390.lu_data.ipv4_hdr_err);

    osal_printf("IPV4_OPT : 0x%x\t\t", hsm.r8390.lu_data.ipv4_opt);
    osal_printf("IPV4_TTL_ZERO : 0x%x\t", hsm.r8390.lu_data.ipv4_ttl_zero);
    osal_printf("IPV6_HDR_ERR : 0x%x\n", hsm.r8390.lu_data.ipv6_hdr_err);

    osal_printf("IPV6_HL_ZERO : 0x%x\t", hsm.r8390.lu_data.ipv6_hl_zero);
    osal_printf("IPV6_HOP : 0x%x\t\t", hsm.r8390.lu_data.ipv6_hop);
    osal_printf("OAM_ACT : 0x%x\n", hsm.r8390.lu_data.oam_act);

    osal_printf("STP : 0x%x\t\t", hsm.r8390.lu_data.stp);
    osal_printf("VLAN_ZERO : 0x%x\t\t", hsm.r8390.lu_data.vlan_zero);
    osal_printf("VLAN_SPM : 0x%x\n", hsm.r8390.lu_data.vlan_spm);

    osal_printf("FID : 0x%x\t\t", hsm.r8390.lu_data.fid);
    osal_printf("METER_DROP : 0x%x\t", hsm.r8390.lu_data.meter_drop);
    osal_printf("CFM_ACT : 0x%x\n", hsm.r8390.lu_data.cfm_act);

    osal_printf("CFM_IDX : 0x%x\t\t", hsm.r8390.lu_data.cfm_idx);
    osal_printf("CFI_ACT : 0x%x\t\t", hsm.r8390.lu_data.cfi_act);
    osal_printf("TAG_ACT : 0x%x\n", hsm.r8390.lu_data.tag_act);

    osal_printf("NIVID : 0x%x\t\t", hsm.r8390.lu_data.nivid);
    osal_printf("NOVID : 0x%x\t\t", hsm.r8390.lu_data.novid);
    osal_printf("RVID : 0x%x\n", hsm.r8390.lu_data.rvid);

    osal_printf("ACL_MEET : 0x%x\t\t", hsm.r8390.lu_data.acl_meet);
    osal_printf("ACLC2SC : 0x%x\t\t", hsm.r8390.lu_data.aclc2sc);
    osal_printf("ACL_HIT : 0x%x\n", hsm.r8390.lu_data.acl_hit);

    osal_printf("ACL_IDX : 0x%x\t\t", hsm.r8390.lu_data.acl_idx);
    osal_printf("ACL_DATA : 0x%x\t\t", hsm.r8390.lu_data.acl_data);
    osal_printf("SPE_TRAP_RSN : 0x%x\n", hsm.r8390.lu_data.spe_trap_rsn);

    osal_printf("RMA_BYP_VLAN : 0x%x\t", hsm.r8390.lu_data.rma_byp_vlan);
    osal_printf("SPE_TRAP_ACT : 0x%x\t", hsm.r8390.lu_data.spe_trap_act);
    osal_printf("EAV_CLASSA : 0x%x\n", hsm.r8390.lu_data.eav_classa);

    osal_printf("EAV_CLASSB : 0x%x\t", hsm.r8390.lu_data.eav_classb);
    osal_printf("HWATT_RSN : 0x%x\t\t", hsm.r8390.lu_data.hwatt_rsn);
    osal_printf("HWATT_ACT : 0x%x\n", hsm.r8390.lu_data.hwatt_act);

    osal_printf("RMA_PKT : 0x%x\t\t", hsm.r8390.lu_data.rma_pkt);
    osal_printf("RMA_ACT : 0x%x\t\t", hsm.r8390.lu_data.rma_act);
    osal_printf("RMA_BYPASS : 0x%x\n", hsm.r8390.lu_data.rma_bypass);

    osal_printf("C2SC_HIT_MODE : 0x%x\t", hsm.r8390.lu_data.c2sc_hit_mode);
    osal_printf("C2SC_HIT : 0x%x\t\t", hsm.r8390.lu_data.c2sc_hit);
    osal_printf("C2SC_DATA : 0x%x\n", hsm.r8390.lu_data.c2sc_data);

    osal_printf("INTERNAL_PRIO : 0x%x\t", hsm.r8390.lu_data.internal_prio);
    osal_printf("DP_PRE : 0x%x\t\t", hsm.r8390.lu_data.dp_pre);
    osal_printf("PROTOCOL_STORM : 0x%x\n", hsm.r8390.lu_data.protocol_storm);

    osal_printf("IBW_PASS : 0x%x\t\t", hsm.r8390.lu_data.ibw_pass);
    osal_printf("SLID : 0x%x\t\t", hsm.r8390.lu_data.slid);
    osal_printf("TRUNK_GRP : 0x%x\n", hsm.r8390.lu_data.trunk_grp);
    osal_printf("=============================================================\n");

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
int32 _hal_getHsm_9310(uint32 unit, hsm_param_t *hsm)
{
    uint32 *val;
    uint32 tmpVal = 0;
    hal_control_t *pInfo;

    val = osal_alloc(256*sizeof(uint32));
    if (NULL == val)
    {
        osal_printf("osal_alloc  failed!!\n");
        return RT_ERR_FAILED;
    }


    osal_memset(hsm, 0, sizeof(hsa_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        osal_free(val);
        return RT_ERR_FAILED;
    }

    /* HSM_DATA */
    reg_read(unit, MANGO_HSM_DATAr, val);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EST_MATCHf, &tmpVal, val);
    hsm->r9310.est_match = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DST_PE_ECIDf, &tmpVal, val);
    hsm->r9310.dst_pe_ecid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PE_LU_MODEf, &tmpVal, val);
    hsm->r9310.pe_lu_mode = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PE_NSGf, &tmpVal, val);
    hsm->r9310.pe_nsg = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INT_PE_ECID_EXTf, &tmpVal, val);
    hsm->r9310.int_pe_ecid_ext = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ECID_EXTf, &tmpVal, val);
    hsm->r9310.ori_pe_ecid_ext = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_IGR_ECID_EXTf, &tmpVal, val);
    hsm->r9310.ori_pe_igr_ecid_ext = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ECID_BASEf, &tmpVal, val);
    hsm->r9310.ori_pe_ecid_base = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ECID_GRPf, &tmpVal, val);
    hsm->r9310.ori_pe_ecid_grp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ETAG_RSVDf, &tmpVal, val);
    hsm->r9310.ori_pe_etag_rsvd = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_IGR_ECID_BASEf, &tmpVal, val);
    hsm->r9310.ori_pe_igr_ecid_base = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ETAG_DEIf, &tmpVal, val);
    hsm->r9310.ori_pe_etag_dei = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ETAG_PRIf, &tmpVal, val);
    hsm->r9310.ori_pe_etag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_PE_ETAG_IFf, &tmpVal, val);
    hsm->r9310.ori_pe_etag_if = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSB_SADRf, &tmpVal, val);
    hsm->r9310.hsb_sadr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSB_EADRf, &tmpVal, val);
    hsm->r9310.hsb_eadr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PKT_LENf, &tmpVal, val);
    hsm->r9310.pkt_len = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PG_CNTf, &tmpVal, val);
    hsm->r9310.pg_cnt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_STACK_IFf, &tmpVal, val);
    hsm->r9310.stack_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_IFf, &tmpVal, val);
    hsm->r9310.cpu_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_IFf, &tmpVal, val);
    hsm->r9310.of_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_PIPELINEf, &tmpVal, val);
    hsm->r9310.of_pipeline = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_STACK_FMTf, &tmpVal, val);
    hsm->r9310.stack_fmt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_IFf, &tmpVal, val);
    hsm->r9310.ori_itag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_IFf, &tmpVal, val);
    hsm->r9310.ori_otag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_VIDf, &tmpVal, val);
    hsm->r9310.ori_itag_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_CFIf, &tmpVal, val);
    hsm->r9310.ori_itag_cfi = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_PRIf, &tmpVal, val);
    hsm->r9310.ori_itag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_TPIDf, &tmpVal, val);
    hsm->r9310.ori_itag_tpid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_VIDf, &tmpVal, val);
    hsm->r9310.ori_otag_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_CFIf, &tmpVal, val);
    hsm->r9310.ori_otag_cfi = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_PRIf, &tmpVal, val);
    hsm->r9310.ori_otag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_TPIDf, &tmpVal, val);
    hsm->r9310.ori_otag_tpid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CLASS_Af, &tmpVal, val);
    hsm->r9310.class_a = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CLASS_Bf, &tmpVal, val);
    hsm->r9310.class_b = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PPPOE_IFf, &tmpVal, val);
    hsm->r9310.pppoe_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FRAME_TYPEf, &tmpVal, val);
    hsm->r9310.frame_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EVLAN_IFf, &tmpVal, val);
    hsm->r9310.evlan_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IPV4_IFf, &tmpVal, val);
    hsm->r9310.ipv4_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IPV6_IFf, &tmpVal, val);
    hsm->r9310.ipv6_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORG_OMPLS_IFf, &tmpVal, val);
    hsm->r9310.org_ompls_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORG_IMPLS_IFf, &tmpVal, val);
    hsm->r9310.org_impls_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PARSER_CANT_HANDLEf, &tmpVal, val);
    hsm->r9310.parser_cant_handle = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_HOPBYHOP_ERRf, &tmpVal, val);
    hsm->r9310.ip6_eh_hopbyhop_err = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_HOPBYHOP_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_hopbyhop_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_ROUTE_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_route_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_FRAGMENT_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_fragment_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_ESP_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_esp_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_AUTH_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_auth_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_DESTINATION_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_destination_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_MOBILITY_EXISTf, &tmpVal, val);
    hsm->r9310.ip6_eh_mobility_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_REPEATf, &tmpVal, val);
    hsm->r9310.ip6_eh_repeat = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_PROTf, &tmpVal, val);
    hsm->r9310.ip_prot = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_OFFSET_ZEROf, &tmpVal, val);
    hsm->r9310.ip_offset_zero = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_FLAG_SETf, &tmpVal, val);
    hsm->r9310.ip_flag_set = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L4_HDR_RDYf, &tmpVal, val);
    hsm->r9310.l4_hdr_rdy = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L4_OFFSETf, &tmpVal, val);
    hsm->r9310.l4_offset = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L2_ERR_PKTf, &tmpVal, val);
    hsm->r9310.l2_err_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_ERR_PKTf, &tmpVal, val);
    hsm->r9310.l3_err_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_OFFSETf, &tmpVal, val);
    hsm->r9310.l3_offset = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IN_SPNf, &tmpVal, val);
    hsm->r9310.in_spn = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SPNf, &tmpVal, val);
    hsm->r9310.spn = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SP_TRKf, &tmpVal, val);
    hsm->r9310.sp_trk = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SP_IS_TRKf, &tmpVal, val);
    hsm->r9310.sp_is_trk = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INT_ITAG_HITf, &tmpVal, val);
    hsm->r9310.int_itag_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INT_OTAG_HITf, &tmpVal, val);
    hsm->r9310.int_otag_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_L2_CRCf, &tmpVal, val);
    hsm->r9310.l2_crc = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_DA_HITf, &tmpVal, val);
    hsm->r9310.da_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_VID_ENf, &tmpVal, val);
    hsm->r9310.fwd_vid_en = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_RMA_PKTf, &tmpVal, val);
    hsm->r9310.rma_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_META_DATAf, &tmpVal, val);
    hsm->r9310.meta_data = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRK_HASHf, &tmpVal, val);
    hsm->r9310.trk_hash = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DPf, &tmpVal, val);
    hsm->r9310.dp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INTERNAL_PRIORITYf, &tmpVal, val);
    hsm->r9310.internal_priority = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_ENf, &tmpVal, val);
    hsm->r9310.l3_en = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IGR_L3_INTF_IDf, &tmpVal, val);
    hsm->r9310.igr_l3_intf_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TT_IDXf, &tmpVal, val);
    hsm->r9310.tt_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TT_HITf, &tmpVal, val);
    hsm->r9310.tt_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_QIDf, &tmpVal, val);
    hsm->r9310.cpu_qid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DM_PKTf, &tmpVal, val);
    hsm->r9310.dm_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DG_PKTf, &tmpVal, val);
    hsm->r9310.dg_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TX_MIR_CANCELf, &tmpVal, val);
    hsm->r9310.tx_mir_cancel = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_VB_ISO_MBRf, &tmpVal, val);
    hsm->r9310.vb_iso_mbr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_LEARN_ENf, &tmpVal, val);
    hsm->r9310.learn_en = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_VID_SELf, &tmpVal, val);
    hsm->r9310.fwd_vid_sel = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_TYPEf, &tmpVal, val);
    hsm->r9310.fwd_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_RVIDf, &tmpVal, val);
    hsm->r9310.rvid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FIDf, &tmpVal, val);
    hsm->r9310.fid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_LOCAL_REASONf, &tmpVal, val);
    hsm->r9310.trap_to_local_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_LOCAL_CPUf, &tmpVal, val);
    hsm->r9310.trap_to_local_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_LOCAL_REASONf, &tmpVal, val);
    hsm->r9310.copy_to_local_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_LOCAL_CPUf, &tmpVal, val);
    hsm->r9310.copy_to_local_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_REMOTE_REASONf, &tmpVal, val);
    hsm->r9310.trap_to_remote_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_REMOTE_CPUf, &tmpVal, val);
    hsm->r9310.trap_to_remote_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_REMOTE_REASONf, &tmpVal, val);
    hsm->r9310.copy_to_remote_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_REMOTE_CPUf, &tmpVal, val);
    hsm->r9310.copy_to_remote_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DROP_REASONf, &tmpVal, val);
    hsm->r9310.drop_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DROP_PKTf, &tmpVal, val);
    hsm->r9310.drop_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_LOCAL_REASONf, &tmpVal, val);
    hsm->r9310.local_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_REMOTE_REASON_RX_TAG_REASONf, &tmpVal, val);
    hsm->r9310.remote_reason_rx_tag_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_PORTf, &tmpVal, val);
    hsm->r9310.fwd_port = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_TX_FWD_TYPEf, &tmpVal, val);
    hsm->r9310.cpu_tx_fwd_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CTX_DPM_31_0f, &tmpVal, val);
    hsm->r9310.ctx_dpm_31_0 = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CTX_DPM_55_32f, &tmpVal, val);
    hsm->r9310.ctx_dpm_55_32 = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SW_DEVf, &tmpVal, val);
    hsm->r9310.sw_unit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SPF_SPNf, &tmpVal, val);
    hsm->r9310.spf_spn = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SPF_IS_TRKf, &tmpVal, val);
    hsm->r9310.spf_is_trk = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SRC_FILTER_ENf, &tmpVal, val);
    hsm->r9310.src_filter_en = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_AS_QIDf, &tmpVal, val);
    hsm->r9310.as_qid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ALE_AS_TAGSTSf, &tmpVal, val);
    hsm->r9310.ale_as_tagsts = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BP_VLAN_EGRf, &tmpVal, val);
    hsm->r9310.bp_vlan_egr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BP_STPf, &tmpVal, val);
    hsm->r9310.bp_stp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BP_FLTRf, &tmpVal, val);
    hsm->r9310.bp_fltr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CNGST_DROPf, &tmpVal, val);
    hsm->r9310.cngst_drop = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_ACTf, &tmpVal, val);
    hsm->r9310.acl_act = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_STK_MIR_HITf, &tmpVal, val);
    hsm->r9310.stk_mir_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TS_REQf, &tmpVal, val);
    hsm->r9310.ts_req = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_ENCAP_REQf, &tmpVal, val);
    hsm->r9310.mpls_encap_req = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TS_INDEXf, &tmpVal, val);
    hsm->r9310.ts_index = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BSSID_IDXf, &tmpVal, val);
    hsm->r9310.bssid_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_ENCAP_IDXf, &tmpVal, val);
    hsm->r9310.mpls_encap_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_DECAP_REQf, &tmpVal, val);
    hsm->r9310.mpls_decap_req = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_HDR_TTLf, &tmpVal, val);
    hsm->r9310.ip_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_IPRI_HITf, &tmpVal, val);
    hsm->r9310.hsa_n21_ipri_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_DSCPf, &tmpVal, val);
    hsm->r9310.hsa_int_dscp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_DSCP_HITf, &tmpVal, val);
    hsm->r9310.hsa_int_dscp_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_DECAP_STAf, &tmpVal, val);
    hsm->r9310.mpls_decap_sta = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_IPRIf, &tmpVal, val);
    hsm->r9310.hsa_n21_ipri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_OPRIf, &tmpVal, val);
    hsm->r9310.hsa_n21_opri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_OPRI_HITf, &tmpVal, val);
    hsm->r9310.hsa_n21_opri_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_TCf, &tmpVal, val);
    hsm->r9310.mpls_tc = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_ITAG_IFf, &tmpVal, val);
    hsm->r9310.hsa_int_itag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SURF_OVIDf, &tmpVal, val);
    hsm->r9310.surf_ovid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_ITAG_PRIf, &tmpVal, val);
    hsm->r9310.hsa_itag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_EACL_IPRI_HITf, &tmpVal, val);
    hsm->r9310.hsa_eacl_ipri_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_OTAG_VIDf, &tmpVal, val);
    hsm->r9310.hsa_int_otag_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_IACL_IPRI_HITf, &tmpVal, val);
    hsm->r9310.hsa_iacl_ipri_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SURF_IVIDf, &tmpVal, val);
    hsm->r9310.surf_ivid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_OTAG_PRIf, &tmpVal, val);
    hsm->r9310.hsa_otag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_IVID_HITf, &tmpVal, val);
    hsm->r9310.eacl_ivid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_ITAG_VIDf, &tmpVal, val);
    hsm->r9310.hsa_int_itag_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_EACL_OPRI_HITf, &tmpVal, val);
    hsm->r9310.hsa_eacl_opri_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_IACL_OPRI_HITf, &tmpVal, val);
    hsm->r9310.hsa_iacl_opri_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_OTPID_IDXf, &tmpVal, val);
    hsm->r9310.acl_otpid_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_OTAG_CFIf, &tmpVal, val);
    hsm->r9310.hsa_int_otag_cfi = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_ITAG_CFIf, &tmpVal, val);
    hsm->r9310.hsa_int_itag_cfi = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_VRF_IDf, &tmpVal, val);
    hsm->r9310.vrf_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_MDF_REQf, &tmpVal, val);
    hsm->r9310.l3_mdf_req = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OAM_LBKf, &tmpVal, val);
    hsm->r9310.oam_lbk = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_N21_IVID_HITf, &tmpVal, val);
    hsm->r9310.n21_ivid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_P_TYPEf, &tmpVal, val);
    hsm->r9310.fwd_p_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OTAG_HITf, &tmpVal, val);
    hsm->r9310.eacl_otag_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OTAG_IFf, &tmpVal, val);
    hsm->r9310.eacl_otag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_OTAG_IFf, &tmpVal, val);
    hsm->r9310.hsa_int_otag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_ITAG_HITf, &tmpVal, val);
    hsm->r9310.eacl_itag_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_ITAG_IFf, &tmpVal, val);
    hsm->r9310.eacl_itag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OVID_HITf, &tmpVal, val);
    hsm->r9310.eacl_ovid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_N21_OVID_HITf, &tmpVal, val);
    hsm->r9310.n21_ovid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IACL_OTPID_HITf, &tmpVal, val);
    hsm->r9310.iacl_otpid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IACL_ITPID_HITf, &tmpVal, val);
    hsm->r9310.iacl_itpid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OTPID_HITf, &tmpVal, val);
    hsm->r9310.eacl_otpid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_ITPID_HITf, &tmpVal, val);
    hsm->r9310.eacl_itpid_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_ITPID_IDXf, &tmpVal, val);
    hsm->r9310.acl_itpid_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PORT_DATAf, &tmpVal, val);
    hsm->r9310.port_data = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PORT_IS_TRKf, &tmpVal, val);
    hsm->r9310.port_is_trk = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INVALID_SAf, &tmpVal, val);
    hsm->r9310.invalid_sa = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HASH_FULLf, &tmpVal, val);
    hsm->r9310.hash_full = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L2_DYN_PMVf, &tmpVal, val);
    hsm->r9310.l2_dyn_pmv = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L2_STTC_PMVf, &tmpVal, val);
    hsm->r9310.l2_sttc_pmv = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PMV_FORBIDf, &tmpVal, val);
    hsm->r9310.pmv_forbid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_NEW_SAf, &tmpVal, val);
    hsm->r9310.new_sa = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DM_RXIDXf, &tmpVal, val);
    hsm->r9310.dm_rxidx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MAC_CSTf, &tmpVal, val);
    hsm->r9310.mac_cst = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_IDXf, &tmpVal, val);
    hsm->r9310.acl_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ATK_TYPEf, &tmpVal, val);
    hsm->r9310.atk_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_RX_L2_ERRf, &tmpVal, val);
    hsm->r9310.cpu_rx_l2_err = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_RX_L3_ERRf, &tmpVal, val);
    hsm->r9310.cpu_rx_l3_err = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_RX_SFLOWf, &tmpVal, val);
    hsm->r9310.cpu_rx_sflow = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_TBL_IDf, &tmpVal, val);
    hsm->r9310.of_tbl_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_OF_HITf, &tmpVal, val);
    hsm->r9310.acl_of_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_AS_QIDf, &tmpVal, val);
    hsm->r9310.acl_as_qid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_RMA_FLOOD_IDf, &tmpVal, val);
    hsm->r9310.rma_flood_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_MISS_TBL_IDf, &tmpVal, val);
    hsm->r9310.of_miss_tbl_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_APP_TYPEf, &tmpVal, val);
    hsm->r9310.app_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DMAC_47_32f, &tmpVal, val);
    hsm->r9310.dmac[0] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310.dmac[1] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DMAC_31_0f, &tmpVal, val);
    hsm->r9310.dmac[2] = (tmpVal >> 24);
    hsm->r9310.dmac[3] = (tmpVal & 0xFF0000) >> 16;
    hsm->r9310.dmac[4] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310.dmac[5] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SMAC_47_16f, &tmpVal, val);
    hsm->r9310.smac[0] = (tmpVal >> 24);
    hsm->r9310.smac[1] = (tmpVal & 0xFF0000) >> 16;
    hsm->r9310.smac[2] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310.smac[3] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SMAC_15_0f, &tmpVal, val);
    hsm->r9310.smac[4] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310.smac[5] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRK_UC_HASH_0f, &tmpVal, val);
    hsm->r9310.trk_uc_hash_0 = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRK_UC_HASH_1f, &tmpVal, val);
    hsm->r9310.trk_uc_hash_1 = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OAMPDUf, &tmpVal, val);
    hsm->r9310.oampdu = tmpVal;

    /* HSM_FWD_DATA */
    reg_read(unit, MANGO_HSM_FWD_DATAr, val);
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_IP_LENf, &tmpVal, val);
    hsm->r9310.ip_len = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_DROP_TRAPf, &tmpVal, val);
    hsm->r9310.drop_trap = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_PKT_TYPEf, &tmpVal, val);
    hsm->r9310.pkt_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_ACL_INFOf, &tmpVal, val);
    hsm->r9310.acl_info = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_ACL_CMDf, &tmpVal, val);
    hsm->r9310.acl_cmd = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_MIR_ACL_RXf, &tmpVal, val);
    hsm->r9310.mir_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_MIR_ACL_CANCELf, &tmpVal, val);
    hsm->r9310.mir_act = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_MIR_HITf, &tmpVal, val);
    hsm->r9310.mir_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_STK_PMSKf, &tmpVal, val);
    hsm->r9310.stk_pmsk = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_RPF_FLAGf, &tmpVal, val);
    hsm->r9310.rpf_flag = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_RPF_ASSERTf, &tmpVal, val);
    hsm->r9310.rpf_assert = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_TS_IDXf, &tmpVal, val);
    hsm->r9310.ts_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_TS_HITf, &tmpVal, val);
    hsm->r9310.ts_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_OIL_IDXf, &tmpVal, val);
    hsm->r9310.oil_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_L3VLDf, &tmpVal, val);
    hsm->r9310.l3vld = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_BSSID_IDXf, &tmpVal, val);
    hsm->r9310.fwd_bssid_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_BSSID_TYPEf, &tmpVal, val);
    hsm->r9310.bssid_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_WLAN_RDYf, &tmpVal, val);
    hsm->r9310.wlan_rdy = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_PORT_INFOf, &tmpVal, val);
    hsm->r9310.port_info = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_MC_TYPEf, &tmpVal, val);
    hsm->r9310.mc_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_PORT_TYPEf, &tmpVal, val);
    hsm->r9310.port_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_LAN_RDYf, &tmpVal, val);
    hsm->r9310.lan_rdy = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_DA_BLOCKf, &tmpVal, val);
    hsm->r9310.da_block = tmpVal;
    reg_field_get(unit, MANGO_HSM_FWD_DATAr, MANGO_L2VLDf, &tmpVal, val);
    hsm->r9310.l2vld = tmpVal;

    /* HSM_POST_DATA */
    reg_read(unit, MANGO_HSM_POST_DATAr, val);
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_LDPM_57_32f, &tmpVal, val);
    hsm->r9310.ldpm_57_32 = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_LDPM_31_0f, &tmpVal, val);
    hsm->r9310.ldpm_31_0 = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_ACL_MIRR_RXf, &tmpVal, val);
    hsm->r9310.acl_mirr = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_STK_CDPMf, &tmpVal, val);
    hsm->r9310.stk_cdpm = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_STK_NDPMf, &tmpVal, val);
    hsm->r9310.stk_ndpm = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_PKT_TYPEf, &tmpVal, val);
    hsm->r9310.pkt_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_HSA_SELf, &tmpVal, val);
    hsm->r9310.hsa_sel = tmpVal;
    reg_field_get(unit, MANGO_HSM_POST_DATAr, MANGO_HSA_ADDRf, &tmpVal, val);
    hsm->r9310.hsa_addr = tmpVal;

    osal_free(val);

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
int32 _hal_dumpHsm_9310(uint32 unit)
{
    hsm_param_t   hsm;
    //uint32        tmp;
    hal_control_t *pInfo;

    if(RT_ERR_OK != _hal_getHsm_9310(unit, &hsm))
    {
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    osal_printf("\n========================================  HSM ========================================\n");
    osal_printf("OAMPDU : 0x%x\t\t\t", hsm.r9310.oampdu);
    osal_printf("TRK_UC_HASH_1 : 0x%x\t\t", hsm.r9310.trk_uc_hash_1);
    osal_printf("TRK_UC_HASH_0 : 0x%x\n", hsm.r9310.trk_uc_hash_0);
    osal_printf("dmac: 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", hsm.r9310.dmac[0], hsm.r9310.dmac[1],
                                                         hsm.r9310.dmac[2], hsm.r9310.dmac[3],
                                                         hsm.r9310.dmac[4], hsm.r9310.dmac[5]);
    osal_printf("smac: 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", hsm.r9310.smac[0], hsm.r9310.smac[1],
                                                         hsm.r9310.smac[2], hsm.r9310.smac[3],
                                                         hsm.r9310.smac[4], hsm.r9310.smac[5]);
    osal_printf("APP_TYPE : 0x%x\t\t\t", hsm.r9310.app_type);
    osal_printf("OF_MISS_TBL_ID : 0x%x\t\t", hsm.r9310.of_miss_tbl_id);
    osal_printf("RMA_FLOOD_ID : 0x%x\n" , hsm.r9310.rma_flood_id);
    osal_printf("ACL_AS_QID : 0x%x\t\t", hsm.r9310.acl_as_qid);
    osal_printf("ACL_OF_HIT : 0x%x\t\t", hsm.r9310.acl_of_hit);
    osal_printf("OF_TBL_ID : 0x%x\n", hsm.r9310.of_tbl_id);
    osal_printf("CPU_RX_SFLOW : 0x%x\t\t" , hsm.r9310.cpu_rx_sflow);
    osal_printf("CPU_RX_L3_ERR : 0x%x\t\t", hsm.r9310.cpu_rx_l3_err);
    osal_printf("CPU_RX_L2_ERR : 0x%x\n", hsm.r9310.cpu_rx_l2_err);
    osal_printf("ATK_TYPE : 0x%x\t\t\t" , hsm.r9310.atk_type);
    osal_printf("ACL_IDX : 0x%x\t\t\t", hsm.r9310.acl_idx);
    osal_printf("MAC_CST : 0x%x\n", hsm.r9310.mac_cst);
    osal_printf("DM_RXIDX : 0x%x\t\t\t", hsm.r9310.dm_rxidx);
    osal_printf("NEW_SA : 0x%x\t\t\t", hsm.r9310.new_sa);
    osal_printf("PMV_FORBID : 0x%x\n", hsm.r9310.pmv_forbid);
    osal_printf("L2_STTC_PMV : 0x%x\t\t", hsm.r9310.l2_sttc_pmv);
    osal_printf("L2_DYN_PMV : 0x%x\t\t", hsm.r9310.l2_dyn_pmv);
    osal_printf("HASH_FULL : 0x%x\n", hsm.r9310.hash_full);
    osal_printf("INVALID_SA : 0x%x\t\t", hsm.r9310.invalid_sa);
    osal_printf("PORT_IS_TRK : 0x%x\t\t", hsm.r9310.port_is_trk);
    osal_printf("PORT_DATA : 0x%x\n", hsm.r9310.port_data);
    osal_printf("ACL_ITPID_IDX : 0x%x\t\t", hsm.r9310.acl_itpid_idx);
    osal_printf("EACL_ITPID_HIT : 0x%x\t\t", hsm.r9310.eacl_itpid_hit);
    osal_printf("EACL_OTPID_HIT : 0x%x\n", hsm.r9310.eacl_otpid_hit);
    osal_printf("IACL_ITPID_HIT : 0x%x\t\t", hsm.r9310.iacl_itpid_hit);
    osal_printf("IACL_OTPID_HIT : 0x%x\t\t", hsm.r9310.iacl_otpid_hit);
    osal_printf("N21_OVID_HIT : 0x%x\n" , hsm.r9310.n21_ovid_hit);
    osal_printf("EACL_OVID_HIT : 0x%x\t\t", hsm.r9310.eacl_ovid_hit);
    osal_printf("EACL_ITAG_IF : 0x%x\t\t" , hsm.r9310.eacl_itag_if);
    osal_printf("EACL_ITAG_HIT : 0x%x\n", hsm.r9310.eacl_itag_hit);
    osal_printf("HSA_INT_OTAG_IF : 0x%x\t\t", hsm.r9310.hsa_int_otag_if);
    osal_printf("EACL_OTAG_IF : 0x%x\t\t" , hsm.r9310.eacl_otag_if);
    osal_printf("EACL_OTAG_HIT : 0x%x\n", hsm.r9310.eacl_otag_hit);
    osal_printf("FWD_P_TYPE : 0x%x\t\t", hsm.r9310.fwd_p_type);
    osal_printf("N21_IVID_HIT : 0x%x\t\t", hsm.r9310.n21_ivid_hit);
    osal_printf("OAM_LBK : 0x%x\n", hsm.r9310.oam_lbk);
    osal_printf("L3_MDF_REQ : 0x%x\t\t", hsm.r9310.l3_mdf_req);
    osal_printf("VRF_ID : 0x%x\t\t\t", hsm.r9310.vrf_id);
    osal_printf("HSA_INT_ITAG_CFI : 0x%x\n" , hsm.r9310.hsa_int_itag_cfi);
    osal_printf("HSA_INT_OTAG_CFI : 0x%x\t\t" , hsm.r9310.hsa_int_otag_cfi);
    osal_printf("ACL_OTPID_IDX : 0x%x\t\t", hsm.r9310.acl_otpid_idx);
    osal_printf("HSA_IACL_OPRI_HIT : 0x%x\n", hsm.r9310.hsa_iacl_opri_hit);
    osal_printf("HSA_EACL_OPRI_HIT : 0x%x\t\t", hsm.r9310.hsa_eacl_opri_hit);
    osal_printf("HSA_INT_ITAG_VID : 0x%x\t\t", hsm.r9310.hsa_int_itag_vid);
    osal_printf("EACL_IVID_HIT : 0x%x\n", hsm.r9310.eacl_ivid_hit);
    osal_printf("HSA_OTAG_PRI : 0x%x\t\t" , hsm.r9310.hsa_otag_pri);
    osal_printf("SURF_IVID : 0x%x\t\t\t", hsm.r9310.surf_ivid);
    osal_printf("HSA_IACL_IPRI_HIT : 0x%x\n", hsm.r9310.hsa_iacl_ipri_hit);
    osal_printf("HSA_INT_OTAG_VID : 0x%x\t\t" , hsm.r9310.hsa_int_otag_vid);
    osal_printf("HSA_EACL_IPRI_HIT : 0x%x\t\t", hsm.r9310.hsa_eacl_ipri_hit);
    osal_printf("HSA_ITAG_PRI : 0x%x\n" , hsm.r9310.hsa_itag_pri);
    osal_printf("SURF_OVID : 0x%x\t\t\t" , hsm.r9310.surf_ovid);
    osal_printf("HSA_INT_ITAG_IF : 0x%x\t\t", hsm.r9310.hsa_int_itag_if);
    osal_printf("MPLS_TC : 0x%x\n", hsm.r9310.mpls_tc);
    osal_printf("HSA_N21_OPRI_HIT : 0x%x\t\t", hsm.r9310.hsa_n21_opri_hit);
    osal_printf("HSA_N21_OPRI : 0x%x\t\t", hsm.r9310.hsa_n21_opri);
    osal_printf("HSA_N21_IPRI : 0x%x\n", hsm.r9310.hsa_n21_ipri);
    osal_printf("MPLS_DECAP_STA : 0x%x\t\t", hsm.r9310.mpls_decap_sta);
    osal_printf("HSA_INT_DSCP_HIT : 0x%x\t\t", hsm.r9310.hsa_int_dscp_hit);
    osal_printf("HSA_INT_DSCP : 0x%x\n", hsm.r9310.hsa_int_dscp);
    osal_printf("HSA_N21_IPRI_HIT : 0x%x\t\t", hsm.r9310.hsa_n21_ipri_hit);
    osal_printf("IP_TTL : 0x%x\t\t\t", hsm.r9310.ip_ttl);
    osal_printf("MPLS_DECAP_REQ : 0x%x\n", hsm.r9310.mpls_decap_req);
    osal_printf("MPLS_ENCAP_IDX : 0x%x\t\t", hsm.r9310.mpls_encap_idx);
    osal_printf("BSSID_IDX : 0x%x\t\t\t", hsm.r9310.bssid_idx);
    osal_printf("TS_INDEX : 0x%x\n" , hsm.r9310.ts_index);
    osal_printf("MPLS_ENCAP_REQ : 0x%x\t\t", hsm.r9310.mpls_encap_req);
    osal_printf("TS_REQ : 0x%x\t\t\t", hsm.r9310.ts_req);
    osal_printf("STK_MIR_HIT : 0x%x\n", hsm.r9310.stk_mir_hit);
    osal_printf("ACL_ACT : 0x%x\t\t\t", hsm.r9310.acl_act);
    osal_printf("CNGST_DROP : 0x%x\t\t", hsm.r9310.cngst_drop);
    osal_printf("BP_FLTR : 0x%x\n", hsm.r9310.bp_fltr);
    osal_printf("BP_STP : 0x%x\t\t\t", hsm.r9310.bp_stp);
    osal_printf("BP_VLAN_EGR : 0x%x\t\t", hsm.r9310.bp_vlan_egr);
    osal_printf("ALE_AS_TAGSTS : 0x%x\n", hsm.r9310.ale_as_tagsts);
    osal_printf("AS_QID : 0x%x\t\t\t", hsm.r9310.as_qid);
    osal_printf("SRC_FILTER_EN : 0x%x\t\t", hsm.r9310.src_filter_en);
    osal_printf("SPF_IS_TRK : 0x%x\n", hsm.r9310.spf_is_trk);
    osal_printf("SPF_SPN : 0x%x\t\t\t", hsm.r9310.spf_spn);
    osal_printf("SW_DEV : 0x%x\t\t\t", hsm.r9310.sw_unit);
    osal_printf("CTX_DPM_55_32 : 0x%x\n", hsm.r9310.ctx_dpm_55_32);
    osal_printf("CTX_DPM_31_0 : 0x%x\t\t" , hsm.r9310.ctx_dpm_31_0);
    osal_printf("CPU_TX_FWD_TYPE : 0x%x\n", hsm.r9310.cpu_tx_fwd_type);
    osal_printf("FWD_PORT : 0x%x\t\t" , hsm.r9310.fwd_port);
    osal_printf("REMOTE_REASON_RX_TAG_REASON: 0x%x\t", hsm.r9310.remote_reason_rx_tag_reason);
    osal_printf("LOCAL_REASON : 0x%x\n", hsm.r9310.local_reason);
    osal_printf("DROP_PKT : 0x%x\t\t\t" , hsm.r9310.drop_pkt);
    osal_printf("DROP_REASON : 0x%x\t\t"  , hsm.r9310.drop_reason);
    osal_printf("COPY_TO_REMOTE_CPU : 0x%x\n" , hsm.r9310.copy_to_remote_cpu);
    osal_printf("COPY_TO_REMOTE_REASON : 0x%x\t", hsm.r9310.copy_to_remote_reason);
    osal_printf("TRAP_TO_REMOTE_CPU : 0x%x\t" , hsm.r9310.trap_to_remote_cpu);
    osal_printf("TRAP_TO_REMOTE_REASON : 0x%x\n", hsm.r9310.trap_to_remote_reason);
    osal_printf("COPY_TO_LOCAL_CPU : 0x%x\t\t", hsm.r9310.copy_to_local_cpu);
    osal_printf("COPY_TO_LOCAL_REASON : 0x%x\t", hsm.r9310.copy_to_local_reason);
    osal_printf("TRAP_TO_LOCAL_CPU : 0x%x\n", hsm.r9310.trap_to_local_cpu);
    osal_printf("TRAP_TO_LOCAL_REASON : 0x%x\t", hsm.r9310.trap_to_local_reason);
    osal_printf("FID : 0x%x\t\t\t", hsm.r9310.fid);
    osal_printf("RVID : 0x%x\n"  , hsm.r9310.rvid);
    osal_printf("FWD_TYPE : 0x%x\t\t\t" , hsm.r9310.fwd_type);
    osal_printf("FWD_VID_SEL : 0x%x\t\t"  , hsm.r9310.fwd_vid_sel);
    osal_printf("LEARN_EN : 0x%x\n" , hsm.r9310.learn_en);
    osal_printf("VB_ISO_MBR : 0x%x\t\t", hsm.r9310.vb_iso_mbr);
    osal_printf("TX_MIR_CANCEL : 0x%x\t\t", hsm.r9310.tx_mir_cancel);
    osal_printf("DG_PKT : 0x%x\n", hsm.r9310.dg_pkt);
    osal_printf("DM_PKT : 0x%x\t\t\t", hsm.r9310.dm_pkt);
    osal_printf("CPU_QID : 0x%x\t\t\t", hsm.r9310.cpu_qid);
    osal_printf("TT_HIT : 0x%x\n", hsm.r9310.tt_hit);
    osal_printf("TT_IDX : 0x%x\t\t\t", hsm.r9310.tt_idx);
    osal_printf("IGR_L3_INTF_ID : 0x%x\t\t", hsm.r9310.igr_l3_intf_id);
    osal_printf("L3_EN : 0x%x\n" , hsm.r9310.l3_en);
    osal_printf("INTERNAL_PRIORITY : 0x%x\t\t", hsm.r9310.internal_priority);
    osal_printf("DP : 0x%x\t\t\t", hsm.r9310.dp);
    osal_printf("TRK_HASH : 0x%x\n" , hsm.r9310.trk_hash);
    osal_printf("META_DATA : 0x%x\t\t\t", hsm.r9310.meta_data);
    osal_printf("RMA_PKT : 0x%x\t\t\t", hsm.r9310.rma_pkt);
    osal_printf("FWD_VID_EN : 0x%x\n", hsm.r9310.fwd_vid_en);
    osal_printf("DA_HIT : 0x%x\n", hsm.r9310.da_hit);
    osal_printf("L2_CRC : 0x%x\t\t\t", hsm.r9310.l2_crc);
    osal_printf("INT_OTAG_HIT : 0x%x\n", hsm.r9310.int_otag_hit);
    osal_printf("INT_ITAG_HIT : 0x%x\t\t", hsm.r9310.int_itag_hit);
    osal_printf("SP_IS_TRK : 0x%x\t\t\t", hsm.r9310.sp_is_trk);
    osal_printf("SP_TRK : 0x%x\n", hsm.r9310.sp_trk);
    osal_printf("SPN : 0x%x\t\t\t", hsm.r9310.spn);
    osal_printf("IN_SPN : 0x%x\t\t\t", hsm.r9310.in_spn);
    osal_printf("L3_OFFSET : 0x%x\n", hsm.r9310.l3_offset);
    osal_printf("L3_ERR_PKT : 0x%x\t\t", hsm.r9310.l3_err_pkt);
    osal_printf("L2_ERR_PKT : 0x%x\t\t", hsm.r9310.l2_err_pkt);
    osal_printf("L4_OFFSET : 0x%x\n", hsm.r9310.l4_offset);
    osal_printf("L4_HDR_RDY : 0x%x\t\t", hsm.r9310.l4_hdr_rdy);
    osal_printf("IP_FLAG_SET : 0x%x\t\t"  , hsm.r9310.ip_flag_set);
    osal_printf("IP_OFFSET_ZERO : 0x%x\n", hsm.r9310.ip_offset_zero);
    osal_printf("IP_PROT : 0x%x\t\t\t", hsm.r9310.ip_prot);
    osal_printf("IP6_EH_REPEAT : 0x%x\t\t", hsm.r9310.ip6_eh_repeat);
    osal_printf("IP6_EH_MOBILITY_EXIST : 0x%x\n", hsm.r9310.ip6_eh_mobility_exist);
    osal_printf("IP6_EH_DESTINATION_EXIST: 0x%x\t", hsm.r9310.ip6_eh_destination_exist);
    osal_printf("IP6_EH_AUTH_EXIST : 0x%x\t\t", hsm.r9310.ip6_eh_auth_exist);
    osal_printf("IP6_EH_ESP_EXIST : 0x%x\n" , hsm.r9310.ip6_eh_esp_exist);
    osal_printf("IP6_EH_FRAGMENT_EXIST : 0x%x\t", hsm.r9310.ip6_eh_fragment_exist);
    osal_printf("IP6_EH_ROUTE_EXIST : 0x%x\t" , hsm.r9310.ip6_eh_route_exist);
    osal_printf("IP6_EH_HOPBYHOP_EXIST : 0x%x\n", hsm.r9310.ip6_eh_hopbyhop_exist);
    osal_printf("IP6_EH_HOPBYHOP_ERR : 0x%x\t", hsm.r9310.ip6_eh_hopbyhop_err);
    osal_printf("PARSER_CANT_HANDLE : 0x%x\t" , hsm.r9310.parser_cant_handle);
    osal_printf("ORG_IMPLS_IF : 0x%x\n" , hsm.r9310.org_impls_if);
    osal_printf("ORG_OMPLS_IF : 0x%x\t\t" , hsm.r9310.org_ompls_if);
    osal_printf("IPV6_IF : 0x%x\t\t\t", hsm.r9310.ipv6_if);
    osal_printf("IPV4_IF : 0x%x\n", hsm.r9310.ipv4_if);
    osal_printf("EVLAN_IF : 0x%x\t\t\t" , hsm.r9310.evlan_if);
    osal_printf("FRAME_TYPE : 0x%x\t\t", hsm.r9310.frame_type);
    osal_printf("PPPOE_IF : 0x%x\n" , hsm.r9310.pppoe_if);
    osal_printf("CLASS_B : 0x%x\t\t\t", hsm.r9310.class_b);
    osal_printf("CLASS_A : 0x%x\t\t\t", hsm.r9310.class_a);
    osal_printf("ORI_OTAG_TPID : 0x%x\n", hsm.r9310.ori_otag_tpid);
    osal_printf("ORI_OTAG_PRI : 0x%x\t\t" , hsm.r9310.ori_otag_pri);
    osal_printf("ORI_OTAG_CFI : 0x%x\t\t" , hsm.r9310.ori_otag_cfi);
    osal_printf("ORI_OTAG_VID : 0x%x\n" , hsm.r9310.ori_otag_vid);
    osal_printf("ORI_ITAG_TPID : 0x%x\t\t", hsm.r9310.ori_itag_tpid);
    osal_printf("ORI_ITAG_PRI : 0x%x\t\t" , hsm.r9310.ori_itag_pri);
    osal_printf("ORI_ITAG_CFI : 0x%x\n" , hsm.r9310.ori_itag_cfi);
    osal_printf("ORI_ITAG_VID : 0x%x\t\t" , hsm.r9310.ori_itag_vid);
    osal_printf("ORI_OTAG_IF : 0x%x\t\t"  , hsm.r9310.ori_otag_if);
    osal_printf("ORI_ITAG_IF : 0x%x\n"  , hsm.r9310.ori_itag_if);
    osal_printf("STACK_FMT : 0x%x\t\t\t", hsm.r9310.stack_fmt);
    osal_printf("OF_PIPELINE : 0x%x\t\t"  , hsm.r9310.of_pipeline);
    osal_printf("OF_IF : 0x%x\n" , hsm.r9310.of_if);
    osal_printf("CPU_IF : 0x%x\t\t\t", hsm.r9310.cpu_if);
    osal_printf("STACK_IF : 0x%x\t\t\t" , hsm.r9310.stack_if);
    osal_printf("PG_CNT : 0x%x\n", hsm.r9310.pg_cnt);
    osal_printf("PKT_LEN : 0x%x\t\t\t", hsm.r9310.pkt_len);
    osal_printf("HSB_EADR : 0x%x\t\t\t" , hsm.r9310.hsb_eadr);
    osal_printf("HSB_SADR : 0x%x\n" , hsm.r9310.hsb_sadr);
    osal_printf("EST_MATCH : 0x%x\t\t\tDST_PE_ECID : 0x%06x\n", hsm.r9310.est_match, hsm.r9310.dst_pe_ecid);
    osal_printf("PE_LU_MODE : 0x%x\t\tPE_NSG : 0x%02x\n", hsm.r9310.pe_lu_mode, hsm.r9310.pe_nsg);
    osal_printf("INT_PE_ECID_EXT : 0x%02x\t\tORI_PE_ETAG_IF : 0x%x\n", hsm.r9310.int_pe_ecid_ext, hsm.r9310.ori_pe_etag_if);
    osal_printf("ORI_PE_IGR_ECID  :                  EXT 0x%02x BASE 0x%03x\n", hsm.r9310.ori_pe_igr_ecid_ext, hsm.r9310.ori_pe_igr_ecid_base);
    osal_printf("ORI_PE_ETAG_ECID : RSVD 0x%x GRP 0x%x EXT 0x%02x BASE 0x%03x\n", hsm.r9310.ori_pe_etag_rsvd, hsm.r9310.ori_pe_ecid_grp, hsm.r9310.ori_pe_ecid_ext, hsm.r9310.ori_pe_ecid_base);
    osal_printf("\n================================ HSM_FWD_DATA ========================================\n");
    osal_printf("IP_LEN : 0x%x\t\t" , hsm.r9310.ip_len);
    osal_printf("DROP_TRAP : 0x%x\t\t", hsm.r9310.drop_trap);
    osal_printf("PKT_TYPE : 0x%x\t\t", hsm.r9310.pkt_type);
    osal_printf("ACL_INFO : 0x%x\n", hsm.r9310.acl_info);
    osal_printf("ACL_CMD : 0x%x\t\t", hsm.r9310.acl_cmd);
    osal_printf("MIR_IDX : 0x%x\t\t", hsm.r9310.mir_idx);
    osal_printf("MIR_ACT : 0x%x\t\t", hsm.r9310.mir_act);
    osal_printf("MIR_HIT : 0x%x\n", hsm.r9310.mir_hit);
    osal_printf("STK_PMSK : 0x%x\t\t", hsm.r9310.stk_pmsk);
    osal_printf("RPF_FLAG : 0x%x\t\t", hsm.r9310.rpf_flag);
    osal_printf("RPF_ASSERT : 0x%x\t", hsm.r9310.rpf_assert);
    osal_printf("TS_IDX : 0x%x\n", hsm.r9310.ts_idx);
    osal_printf("TS_HIT : 0x%x\t\t", hsm.r9310.ts_hit);
    osal_printf("OIL_IDX : 0x%x\t\t", hsm.r9310.oil_idx);
    osal_printf("L3VLD : 0x%x\t\t", hsm.r9310.l3vld);
    osal_printf("BSSID_IDX : 0x%x\n", hsm.r9310.fwd_bssid_idx);
    osal_printf("BSSID_TYPE : 0x%x\t", hsm.r9310.bssid_type);
    osal_printf("WLAN_RDY : 0x%x\t\t", hsm.r9310.wlan_rdy);
    osal_printf("PORT_INFO : 0x%x\t\t", hsm.r9310.port_info);
    osal_printf("MC_TYPE : 0x%x\n", hsm.r9310.mc_type);
    osal_printf("PORT_TYPE : 0x%x\t\t", hsm.r9310.port_type);
    osal_printf("LAN_RDY : 0x%x\t\t", hsm.r9310.lan_rdy);
    osal_printf("DA_BLOCK : 0x%x\t\t", hsm.r9310.da_block);
    osal_printf("L2VLD : 0x%x\n", hsm.r9310.l2vld);
    osal_printf("\n================================ HSM_POST_DATA ========================================\n");
    osal_printf("LDPM_57_32 : 0x%x\t" , hsm.r9310.ldpm_57_32);
    osal_printf("LDPM_31_0 : 0x%x\t\t" , hsm.r9310.ldpm_31_0);
    osal_printf("ACL_MIRR : 0x%x\t\t" , hsm.r9310.acl_mirr);
    osal_printf("STK_CDPM : 0x%x\n" , hsm.r9310.stk_cdpm);
    osal_printf("STK_NDPM : 0x%x\t\t" , hsm.r9310.stk_ndpm);
    osal_printf("PKT_TYPE : 0x%x\t\t" , hsm.r9310.pkt_type);
    osal_printf("HSA_SEL : 0x%x\t\t" , hsm.r9310.hsa_sel);
    osal_printf("HSA_ADDR : 0x%x\n" , hsm.r9310.hsa_addr);
    osal_printf("=======================================================================================\n");

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsm
 * Description:
 *      Dump hsm paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsm(uint32 unit)
{
    hal_control_t *pHalCtrl = NULL;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        if (RT_ERR_OK != _hal_dumpHsm_8390(unit))
        {
            return RT_ERR_FAILED;
        }
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if (RT_ERR_OK != _hal_dumpHsm_9310(unit))
        {
            return RT_ERR_FAILED;
        }
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of hal_dumpHsm */
#endif

#if defined(CONFIG_SDK_RTL9310)
int32 _hal_getHsm_openflow_9310(uint32 unit, hsm_param_t *hsm)
{

    uint32 *val;
    uint32 tmpVal = 0;
    hal_control_t *pInfo;

    val = osal_alloc(256*sizeof(uint32));
    if (NULL == val)
    {
        osal_printf("osal_alloc  failed!!\n");
        return RT_ERR_FAILED;
    }


    osal_memset(hsm, 0, sizeof(hsm_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        osal_free(val);
        return RT_ERR_FAILED;
    }

    /* HSM_DATA */
    reg_read(unit, MANGO_HSM_DATAr, val);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSB_SADRf, &tmpVal, val);
    hsm->r9310_hsm_of.hsb_sadr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSB_EADRf, &tmpVal, val);
    hsm->r9310_hsm_of.hsb_eadr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PKT_LENf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_len = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PG_CNTf, &tmpVal, val);
    hsm->r9310_hsm_of.pg_cnt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_STACK_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.stack_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.cpu_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.of_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_PIPELINEf, &tmpVal, val);
    hsm->r9310_hsm_of.of_pipeline = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_STACK_FMTf, &tmpVal, val);
    hsm->r9310_hsm_of.stack_fmt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_itag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_otag_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_VIDf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_itag_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_CFIf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_itag_cfi = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_PRIf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_itag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_ITAG_TPIDf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_itag_tpid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_VIDf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_otag_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_CFIf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_otag_cfi = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_PRIf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_otag_pri = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORI_OTAG_TPIDf, &tmpVal, val);
    hsm->r9310_hsm_of.ori_otag_tpid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CLASS_Af, &tmpVal, val);
    hsm->r9310_hsm_of.class_a = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CLASS_Bf, &tmpVal, val);
    hsm->r9310_hsm_of.class_b = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PPPOE_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.pppoe_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FRAME_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.frame_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EVLAN_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.evlan_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IPV4_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.ipv4_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IPV6_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.ipv6_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORG_OMPLS_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.org_ompls_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ORG_IMPLS_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.org_impls_if = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PARSER_CANT_HANDLEf, &tmpVal, val);
    hsm->r9310_hsm_of.parser_cant_handle = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_HOPBYHOP_ERRf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_hopbyhop_err = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_HOPBYHOP_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_hopbyhop_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_ROUTE_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_route_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_FRAGMENT_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_fragment_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_ESP_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_esp_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_AUTH_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_auth_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_DESTINATION_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_destination_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_MOBILITY_EXISTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_mobility_exist = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP6_EH_REPEATf, &tmpVal, val);
    hsm->r9310_hsm_of.ip6_eh_repeat = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_PROTf, &tmpVal, val);
    hsm->r9310_hsm_of.ip_prot = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_OFFSET_ZEROf, &tmpVal, val);
    hsm->r9310_hsm_of.ip_offset_zero = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_FLAG_SETf, &tmpVal, val);
    hsm->r9310_hsm_of.ip_flag_set = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L4_HDR_RDYf, &tmpVal, val);
    hsm->r9310_hsm_of.l4_hdr_rdy = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L4_OFFSETf, &tmpVal, val);
    hsm->r9310_hsm_of.l4_offset = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L2_ERR_PKTf, &tmpVal, val);
    hsm->r9310_hsm_of.l2_err_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_ERR_PKTf, &tmpVal, val);
    hsm->r9310_hsm_of.l3_err_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_OFFSETf, &tmpVal, val);
    hsm->r9310_hsm_of.l3_offset = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IN_SPNf, &tmpVal, val);
    hsm->r9310_hsm_of.in_spn = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SPNf, &tmpVal, val);
    hsm->r9310_hsm_of.spn = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SP_TRKf, &tmpVal, val);
    hsm->r9310_hsm_of.sp_trk = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SP_IS_TRKf, &tmpVal, val);
    hsm->r9310_hsm_of.sp_is_trk = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INT_ITAG_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.loopback_time = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INT_OTAG_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.loopback_time |= ((tmpVal & 0x1) << 1);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_L2_CRCf, &tmpVal, val);
    hsm->r9310_hsm_of.loopback_time |= ((tmpVal & 0x1) << 2);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_DA_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.loopback_time |= ((tmpVal & 0x1) << 3);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_FWD_VID_ENf, &tmpVal, val);
    hsm->r9310_hsm_of.loopback_time |= ((tmpVal & 0x1) << 4);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_RMA_PKTf, &tmpVal, val);
    hsm->r9310_hsm_of.loopback_time |= ((tmpVal & 0x1) << 5);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_META_DATAf, &tmpVal, val);
    hsm->r9310_hsm_of.meta_data = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRK_HASHf, &tmpVal, val);
    hsm->r9310_hsm_of.trk_hash = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DPf, &tmpVal, val);
    hsm->r9310_hsm_of.dp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INTERNAL_PRIORITYf, &tmpVal, val);
    hsm->r9310_hsm_of.internal_priority = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_ENf, &tmpVal, val);
    hsm->r9310_hsm_of.l3_en = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IGR_L3_INTF_IDf, &tmpVal, val);
    hsm->r9310_hsm_of.igr_l3_intf_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TT_IDXf, &tmpVal, val);
    hsm->r9310_hsm_of.tt_idx = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TT_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.tt_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_QIDf, &tmpVal, val);
    hsm->r9310_hsm_of.cpu_qid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DM_PKTf, &tmpVal, val);
    hsm->r9310_hsm_of.dm_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DG_PKTf, &tmpVal, val);
    hsm->r9310_hsm_of.dg_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TX_MIR_CANCELf, &tmpVal, val);
    hsm->r9310_hsm_of.tx_mir_cancel = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_VB_ISO_MBRf, &tmpVal, val);
    hsm->r9310_hsm_of.vb_iso_mbr = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_LEARN_ENf, &tmpVal, val);
    hsm->r9310_hsm_of.learn_en = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_VID_SELf, &tmpVal, val);
    hsm->r9310_hsm_of.fwd_vid_sel = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.fwd_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_RVIDf, &tmpVal, val);
    hsm->r9310_hsm_of.rvid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FIDf, &tmpVal, val);
    hsm->r9310_hsm_of.fid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_LOCAL_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.trap_to_local_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_LOCAL_CPUf, &tmpVal, val);
    hsm->r9310_hsm_of.trap_to_local_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_LOCAL_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.copy_to_local_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_LOCAL_CPUf, &tmpVal, val);
    hsm->r9310_hsm_of.copy_to_local_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_REMOTE_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.trap_to_remote_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRAP_TO_REMOTE_CPUf, &tmpVal, val);
    hsm->r9310_hsm_of.trap_to_remote_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_REMOTE_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.copy_to_remote_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_COPY_TO_REMOTE_CPUf, &tmpVal, val);
    hsm->r9310_hsm_of.copy_to_remote_cpu = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DROP_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.drop_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DROP_PKTf, &tmpVal, val);
    hsm->r9310_hsm_of.drop_pkt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_LOCAL_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.local_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_REMOTE_REASON_RX_TAG_REASONf, &tmpVal, val);
    hsm->r9310_hsm_of.remote_reason = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_PORTf, &tmpVal, val);
    hsm->r9310_hsm_of.output_data = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_TX_FWD_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.output_type = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CTX_DPM_31_0f, &tmpVal, val);
    hsm->r9310_hsm_of.group_idx = (tmpVal & 0x7FF);
    hsm->r9310_hsm_of.group = ((tmpVal >> 11) & 0x1);
    hsm->r9310_hsm_of.queue_data = ((tmpVal >> 12) & 0x7);
    hsm->r9310_hsm_of.set_queue_data = ((tmpVal >> 15) & 0x1);
    hsm->r9310_hsm_of.field_l4_dport = ((tmpVal >> 16) & 0xFFFF);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CTX_DPM_55_32f, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_l4_dport = (tmpVal & 0x1);
    hsm->r9310_hsm_of.field_l4_sport = ((tmpVal >> 1) & 0xFFFF);
    hsm->r9310_hsm_of.set_field_l4_sport = ((tmpVal >> 17) & 0x1);
    hsm->r9310_hsm_of.field_ip_dip = ((tmpVal >> 18) & 0x3F);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SW_DEVf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SPF_SPNf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x3FF) << 4);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SPF_IS_TRKf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 14);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SRC_FILTER_ENf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 15);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_AS_QIDf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 16);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ALE_AS_TAGSTSf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 17);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BP_VLAN_EGRf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 18);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BP_STPf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 19);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BP_FLTRf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 20);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CNGST_DROPf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 21);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_ACTf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 22);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_STK_MIR_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x3) << 23);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SET_IP_RSVf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dip |= ((tmpVal & 0x1) << 25);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IGR_L3_INTF_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.set_ip_rsv = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TS_REQf, &tmpVal, val);
    hsm->r9310_hsm_of.l2mpls_tt = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_ENCAP_REQf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_ip_dip = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TS_INDEXf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_sip = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_BSSID_IDXf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_sip |= ((tmpVal & 0xFFF) << 9);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_ENCAP_IDXf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_sip |= ((tmpVal & 0x7FF) << 21);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_DECAP_REQf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_ip_sip = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IP_HDR_TTLf, &tmpVal, val);
    hsm->r9310_hsm_of.ip_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_IPRI_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_ip_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_DSCPf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_dscp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_DSCP_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_ip_dscp = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_DECAP_STAf, &tmpVal, val);
    hsm->r9310_hsm_of.field_mpls_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_IPRIf, &tmpVal, val);
    hsm->r9310_hsm_of.field_mpls_ttl |= ((tmpVal & 0x7) << 2);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_OPRIf, &tmpVal, val);
    hsm->r9310_hsm_of.field_mpls_ttl |= ((tmpVal & 0x7) << 5);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_N21_OPRI_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_mpls_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MPLS_TCf, &tmpVal, val);
    hsm->r9310_hsm_of.field_mpls_tc = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_ITAG_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_mpls_tc = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SURF_OVIDf, &tmpVal, val);
    hsm->r9310_hsm_of.field_mpls_label = (tmpVal & 0x7FF);
    hsm->r9310_hsm_of.set_field_mpls_label = ((tmpVal >> 11) & 0x1);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_ITAG_PRIf, &tmpVal, val);
    hsm->r9310_hsm_of.field_pcp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_EACL_IPRI_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_pcp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_OTAG_VIDf, &tmpVal, val);
    hsm->r9310_hsm_of.field_vid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_IACL_IPRI_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_vid = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SURF_IVIDf, &tmpVal, val);
    hsm->r9310_hsm_of.field_da = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_OTAG_PRIf, &tmpVal, val);
    hsm->r9310_hsm_of.field_da |= ((tmpVal & 0x7) << 12);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_IVID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.field_da |= ((tmpVal & 0x1) << 15);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_ITAG_VIDf, &tmpVal, val);
    hsm->r9310_hsm_of.set_field_da = (tmpVal & 0x1);
    hsm->r9310_hsm_of.field_sa = ((tmpVal >> 1) & 0x3FF);
    hsm->r9310_hsm_of.set_field_sa = ((tmpVal >> 11) & 0x1);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_EACL_OPRI_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.dec_ip_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_IACL_OPRI_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.dec_mpls_ttl = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_OTPID_IDXf, &tmpVal, val);
    hsm->r9310_hsm_of.copy_ttl_outward = (tmpVal & 0x1);
    hsm->r9310_hsm_of.push_vlan_ethtype = ((tmpVal >> 1) & 0x1);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_OTAG_CFIf, &tmpVal, val);
    hsm->r9310_hsm_of.push_vlan_ethtype |= ((tmpVal & 0x1) << 1);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_ITAG_CFIf, &tmpVal, val);
    hsm->r9310_hsm_of.push_vlan = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_VRF_IDf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_ethtype = tmpVal;
    hsm->r9310_hsm_of.push_mpls_lib_idx = ((tmpVal >> 1) & 0x7F);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L3_MDF_REQf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 7);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OAM_LBKf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 8);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_N21_IVID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 9);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_FWD_P_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_lib_idx |= ((tmpVal & 0x1) << 10);


    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OTAG_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_vpn_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OTAG_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls_mode = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HSA_INT_OTAG_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.push_mpls = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_ITAG_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.pop_mpls_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_ITAG_IFf, &tmpVal, val);
    hsm->r9310_hsm_of.pop_mpls = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OVID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.pop_vlan = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_N21_OVID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.copy_ttl_inward = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IACL_OTPID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.to_of = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_IACL_ITPID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.apply_action = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_OTPID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.of_lookup_req = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_EACL_ITPID_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.next_tab = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_ITPID_IDXf, &tmpVal, val);
    hsm->r9310_hsm_of.next_tab |= ((tmpVal & 0x1) << 1);
    hsm->r9310_hsm_of.field_ip_ttl = ((tmpVal >> 1) & 0x1);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PORT_DATAf, &tmpVal, val);
    hsm->r9310_hsm_of.field_ip_ttl |= ((tmpVal & 0x7F) << 1);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_INVALID_SAf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_dscp = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_HASH_FULLf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_dscp |= ((tmpVal & 0x1) << 1);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L2_DYN_PMVf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_dscp |= ((tmpVal & 0x1) << 2);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_L2_STTC_PMVf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_dscp |= ((tmpVal & 0x1) << 3);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_PMV_FORBIDf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_dscp |= ((tmpVal & 0x1) << 4);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_NEW_SAf, &tmpVal, val);
    hsm->r9310_hsm_of.pkt_dscp |= ((tmpVal & 0x1) << 5);

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DM_RXIDXf, &tmpVal, val);
    hsm->r9310_hsm_of.lb_des_time = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_MAC_CSTf, &tmpVal, val);
    hsm->r9310_hsm_of.mac_cst = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_IDXf, &tmpVal, val);
    hsm->r9310_hsm_of.acl_idx = tmpVal;

    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ATK_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.group_hash = (tmpVal & 0x7);
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_RX_L3_ERRf, &tmpVal, val);
    hsm->r9310_hsm_of.cpu_rx_l3_err = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_CPU_RX_SFLOWf, &tmpVal, val);
    hsm->r9310_hsm_of.cpu_rx_sflow = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_TBL_IDf, &tmpVal, val);
    hsm->r9310_hsm_of.of_tbl_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_OF_HITf, &tmpVal, val);
    hsm->r9310_hsm_of.acl_of_hit = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_ACL_AS_QIDf, &tmpVal, val);
    hsm->r9310_hsm_of.acl_as_qid = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_RMA_FLOOD_IDf, &tmpVal, val);
    hsm->r9310_hsm_of.rma_flood_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OF_MISS_TBL_IDf, &tmpVal, val);
    hsm->r9310_hsm_of.of_miss_tbl_id = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_APP_TYPEf, &tmpVal, val);
    hsm->r9310_hsm_of.app_type = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DMAC_47_32f, &tmpVal, val);
    hsm->r9310_hsm_of.dmac[0] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310_hsm_of.dmac[1] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_DMAC_31_0f, &tmpVal, val);
    hsm->r9310_hsm_of.dmac[2] = (tmpVal >> 24);
    hsm->r9310_hsm_of.dmac[3] = (tmpVal & 0xFF0000) >> 16;
    hsm->r9310_hsm_of.dmac[4] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310_hsm_of.dmac[5] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SMAC_47_16f, &tmpVal, val);
    hsm->r9310_hsm_of.smac[0] = (tmpVal >> 24);
    hsm->r9310_hsm_of.smac[1] = (tmpVal & 0xFF0000) >> 16;
    hsm->r9310_hsm_of.smac[2] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310_hsm_of.smac[3] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_SMAC_15_0f, &tmpVal, val);
    hsm->r9310_hsm_of.smac[4] = (tmpVal & 0xFF00) >> 8;
    hsm->r9310_hsm_of.smac[5] = tmpVal & 0xFF;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRK_UC_HASH_0f, &tmpVal, val);
    hsm->r9310_hsm_of.trk_uc_hash_0 = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_TRK_UC_HASH_1f, &tmpVal, val);
    hsm->r9310_hsm_of.trk_uc_hash_1 = tmpVal;
    reg_field_get(unit, MANGO_HSM_DATAr, MANGO_OAMPDUf, &tmpVal, val);
    hsm->r9310_hsm_of.oampdu = tmpVal;

    osal_free(val);

    return RT_ERR_OK;
}

int32 _hal_dumpHsm_openflow_9310(uint32 unit)
{
    hsm_param_t   hsm;
    hal_control_t *pInfo;

    if(RT_ERR_OK != _hal_getHsm_openflow_9310(unit, &hsm))
    {
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    osal_printf("\n===========================  HSM OpenFlow Mode =======================================\n");
    osal_printf("OAMPDU: 0x%x\t\t\t", hsm.r9310_hsm_of.oampdu);
    osal_printf("TRK_UC_HASH_1: 0x%x\t\t", hsm.r9310_hsm_of.trk_uc_hash_1);
    osal_printf("TRK_UC_HASH_0: 0x%x\n", hsm.r9310_hsm_of.trk_uc_hash_0);
    osal_printf("DMAC: 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", hsm.r9310_hsm_of.dmac[0], hsm.r9310_hsm_of.dmac[2],
                                                         hsm.r9310_hsm_of.dmac[2], hsm.r9310_hsm_of.dmac[3],
                                                         hsm.r9310_hsm_of.dmac[4], hsm.r9310_hsm_of.dmac[5]);
    osal_printf("SMAC: 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", hsm.r9310_hsm_of.smac[0], hsm.r9310_hsm_of.smac[1],
                                                         hsm.r9310_hsm_of.smac[2], hsm.r9310_hsm_of.smac[3],
                                                         hsm.r9310_hsm_of.smac[4], hsm.r9310_hsm_of.smac[5]);
    osal_printf("APP_TYPE: 0x%x\t\t\t", hsm.r9310_hsm_of.app_type);
    osal_printf("OF_MISS_TBL_ID: 0x%x\t\t", hsm.r9310_hsm_of.of_miss_tbl_id);
    osal_printf("RMA_FLOOD_ID: 0x%x\n", hsm.r9310_hsm_of.rma_flood_id);
    osal_printf("ACL_AS_QID: 0x%x\t\t\t", hsm.r9310_hsm_of.acl_as_qid);
    osal_printf("ACL_OF_HIT: 0x%x\t\t\t", hsm.r9310_hsm_of.acl_of_hit);
    osal_printf("OF_TBL_ID: 0x%x\n", hsm.r9310_hsm_of.of_tbl_id);
    osal_printf("CPU_RX_SFLOW: 0x%x\t\t", hsm.r9310_hsm_of.cpu_rx_sflow);
    osal_printf("CPU_RX_L3_ERR: 0x%x\t\t", hsm.r9310_hsm_of.cpu_rx_l3_err);
    osal_printf("CPU_RX_L2_ERR: 0x%x\n", hsm.r9310_hsm_of.cpu_rx_l2_err);
    osal_printf("GROUP_HASH: 0x%x\t\t\t", hsm.r9310_hsm_of.group_hash);
    osal_printf("ACL_IDX: 0x%x\t\t\t", hsm.r9310_hsm_of.acl_idx);
    osal_printf("MAC_CST: 0x%x\n", hsm.r9310_hsm_of.mac_cst);
    osal_printf("LB_DES_TIME: 0x%x\t\t", hsm.r9310_hsm_of.lb_des_time);
    osal_printf("PKT_DSCP: 0x%x\t\t", hsm.r9310_hsm_of.pkt_dscp);
    osal_printf("FIELD_IP_TTL: 0x%x\n", hsm.r9310_hsm_of.field_ip_ttl);
    osal_printf("NEXT_TAB: 0x%x\t\t\t", hsm.r9310_hsm_of.next_tab);
    osal_printf("OF_LOOKUP_REQ: 0x%x\t\t", hsm.r9310_hsm_of.of_lookup_req);
    osal_printf("APPLY_ACTION: 0x%x\n", hsm.r9310_hsm_of.apply_action);
    osal_printf("TO_OF: 0x%x\t\t\t", hsm.r9310_hsm_of.to_of);
    osal_printf("COPY_TTL_INWARD: 0x%x\t\t", hsm.r9310_hsm_of.copy_ttl_inward);
    osal_printf("POP_VLAN: 0x%x\n", hsm.r9310_hsm_of.pop_vlan);
    osal_printf("POP_MPLS: 0x%x\t\t\t", hsm.r9310_hsm_of.pop_mpls);
    osal_printf("POP_MPLS_TYPE: 0x%x\t\t", hsm.r9310_hsm_of.pop_mpls_type);
    osal_printf("PUSH_MPLS: 0x%x\n", hsm.r9310_hsm_of.push_mpls);
    osal_printf("PUSH_MPLS_MODE: 0x%x\t\t", hsm.r9310_hsm_of.push_mpls_mode);
    osal_printf("PUSH_MPLS_VPN_TYPE: 0x%x\t\t", hsm.r9310_hsm_of.push_mpls_vpn_type);
    osal_printf("PUSH_MPLS_LIB_IDX: 0x%x\n", hsm.r9310_hsm_of.push_mpls_lib_idx);
    osal_printf("PUSH_MPLS_ETHTYPE: 0x%x\t\t", hsm.r9310_hsm_of.push_mpls_ethtype);
    osal_printf("PUSH_VLAN: 0x%x\t\t\t", hsm.r9310_hsm_of.push_vlan);
    osal_printf("PUSH_VLAN_ETHTYPE: 0x%x\n", hsm.r9310_hsm_of.push_vlan_ethtype);
    osal_printf("COPY_TTL_OUTWARD: 0x%x\t\t", hsm.r9310_hsm_of.copy_ttl_outward);
    osal_printf("DEC_MPLS_TTL: 0x%x\t\t", hsm.r9310_hsm_of.dec_mpls_ttl);
    osal_printf("DEC_IP_TTL: 0x%x\n", hsm.r9310_hsm_of.dec_ip_ttl);
    osal_printf("SET_FIELD_SA: 0x%x\t\t", hsm.r9310_hsm_of.set_field_sa);
    osal_printf("FIELD_SA: 0x%x\t\t\t", hsm.r9310_hsm_of.field_sa);
    osal_printf("SET_FIELD_DA: 0x%x\n", hsm.r9310_hsm_of.set_field_da);
    osal_printf("FIELD_DA: 0x%x\t\t\t", hsm.r9310_hsm_of.field_da);
    osal_printf("SET_FIELD_VID: 0x%x\t\t", hsm.r9310_hsm_of.set_field_vid);
    osal_printf("FIELD_VID: 0x%x\n", hsm.r9310_hsm_of.field_vid);
    osal_printf("SET_FIELD_PCP: 0x%x\t\t", hsm.r9310_hsm_of.set_field_pcp);
    osal_printf("FIELD_PCP: 0x%x\t\t\t", hsm.r9310_hsm_of.field_pcp);
    osal_printf("SET_FIELD_MPLS_LABEL: 0x%x\n", hsm.r9310_hsm_of.set_field_mpls_label);
    osal_printf("FIELD_MPLS_LABEL: 0x%x\t\t", hsm.r9310_hsm_of.field_mpls_label);
    osal_printf("SET_FIELD_MPLS_TC: 0x%x\t\t", hsm.r9310_hsm_of.set_field_mpls_tc);
    osal_printf("FIELD_MPLS_TC: 0x%x\n", hsm.r9310_hsm_of.field_mpls_tc);
    osal_printf("SET_FIELD_MPLS_TTL: 0x%x\t\t", hsm.r9310_hsm_of.set_field_mpls_ttl);
    osal_printf("FIELD_MPLS_TTL: 0x%x\t\t", hsm.r9310_hsm_of.field_mpls_ttl);
    osal_printf("SET_FIELD_IP_DSCP: 0x%x\n", hsm.r9310_hsm_of.set_field_ip_dscp);
    osal_printf("FIELD_IP_DSCP: 0x%x\t\t", hsm.r9310_hsm_of.field_ip_dscp);
    osal_printf("SET_FIELD_IP_TTL: 0x%x\t\t", hsm.r9310_hsm_of.set_field_ip_ttl);
    osal_printf("IP_TTL: 0x%x\n", hsm.r9310_hsm_of.ip_ttl);
    osal_printf("SET_FIELD_IP_SIP: 0x%x\t\t", hsm.r9310_hsm_of.set_field_ip_sip);
    osal_printf("FIELD_IP_SIP: 0x%x\t\t", hsm.r9310_hsm_of.field_ip_sip);
    osal_printf("SET_FIELD_IP_DIP: 0x%x\n", hsm.r9310_hsm_of.set_field_ip_dip);
    osal_printf("SET_IP_RSV: 0x%x\t\t\t", hsm.r9310_hsm_of.set_ip_rsv);
    osal_printf("FIELD_IP_DIP: 0x%x\t\t", hsm.r9310_hsm_of.field_ip_dip);
    osal_printf("SET_FIELD_L4_SPORT: 0x%x\n", hsm.r9310_hsm_of.set_field_l4_sport);
    osal_printf("FIELD_L4_SPORT: 0x%x\t\t", hsm.r9310_hsm_of.field_l4_sport);
    osal_printf("SET_FIELD_L4_DPORT: 0x%x\t\t", hsm.r9310_hsm_of.set_field_l4_dport);
    osal_printf("FIELD_L4_DPORT: 0x%x\n", hsm.r9310_hsm_of.field_l4_dport);
    osal_printf("SET_QUEUE_DATA: 0x%x\t\t", hsm.r9310_hsm_of.set_queue_data);
    osal_printf("QUEUE_DATA: 0x%x\t\t\t", hsm.r9310_hsm_of.queue_data);
    osal_printf("GROUP: 0x%x\n", hsm.r9310_hsm_of.group);
    osal_printf("GROUP_IDX: 0x%x\t\t\t", hsm.r9310_hsm_of.group_idx);
    osal_printf("OUTPUT_TYPE: 0x%x\t\t", hsm.r9310_hsm_of.output_type);
    osal_printf("OUTPUT_DATA: 0x%x\n", hsm.r9310_hsm_of.output_data);
    osal_printf("REMOTE_REASON: 0x%x\t\t", hsm.r9310_hsm_of.remote_reason);
    osal_printf("LOCAL_REASON: 0x%x\t\t", hsm.r9310_hsm_of.local_reason);
    osal_printf("DROP_PKT: 0x%x\n", hsm.r9310_hsm_of.drop_pkt);
    osal_printf("DROP_REASON: 0x%x\t\t", hsm.r9310_hsm_of.drop_reason);
    osal_printf("COPY_TO_REMOTE_CPU: 0x%x\t\t", hsm.r9310_hsm_of.copy_to_remote_cpu);
    osal_printf("COPY_TO_REMOTE_REASON: 0x%x\n", hsm.r9310_hsm_of.copy_to_remote_reason);
    osal_printf("TRAP_TO_REMOTE_CPU: 0x%x\t\t", hsm.r9310_hsm_of.trap_to_remote_cpu);
    osal_printf("TRAP_TO_REMOTE_REASON: 0x%x\t", hsm.r9310_hsm_of.trap_to_remote_reason);
    osal_printf("COPY_TO_LOCAL_CPU: 0x%x\n", hsm.r9310_hsm_of.copy_to_local_cpu);
    osal_printf("COPY_TO_LOCAL_REASON: 0x%x\t", hsm.r9310_hsm_of.copy_to_local_reason);
    osal_printf("TRAP_TO_LOCAL_CPU: 0x%x\t\t", hsm.r9310_hsm_of.trap_to_local_cpu);
    osal_printf("TRAP_TO_LOCAL_REASON: 0x%x\n", hsm.r9310_hsm_of.trap_to_local_reason);
    osal_printf("FID: 0x%x\t\t\t", hsm.r9310_hsm_of.fid);
    osal_printf("RVID: 0x%x\t\t\t", hsm.r9310_hsm_of.rvid);
    osal_printf("FWD_TYPE: 0x%x\n", hsm.r9310_hsm_of.fwd_type);
    osal_printf("FWD_VID_SEL: 0x%x\t\t", hsm.r9310_hsm_of.fwd_vid_sel);
    osal_printf("LEARN_EN: 0x%x\t\t\t", hsm.r9310_hsm_of.learn_en);
    osal_printf("VB_ISO_MBR: 0x%x\n", hsm.r9310_hsm_of.vb_iso_mbr);
    osal_printf("TX_MIR_CANCEL: 0x%x\t\t", hsm.r9310_hsm_of.tx_mir_cancel);
    osal_printf("DG_PKT: 0x%x\t\t\t", hsm.r9310_hsm_of.dg_pkt);
    osal_printf("DM_PKT: 0x%x\n", hsm.r9310_hsm_of.dm_pkt);
    osal_printf("CPU_QID: 0x%x\t\t\t", hsm.r9310_hsm_of.cpu_qid);
    osal_printf("TT_HIT: 0x%x\t\t\t", hsm.r9310_hsm_of.tt_hit);
    osal_printf("TT_IDX: 0x%x\n", hsm.r9310_hsm_of.tt_idx);
    osal_printf("IGR_L3_INTF_ID: 0x%x\t\t", hsm.r9310_hsm_of.igr_l3_intf_id);
    osal_printf("L3_EN: 0x%x\t\t\t", hsm.r9310_hsm_of.l3_en);
    osal_printf("INTERNAL_PRIORITY: 0x%x\n", hsm.r9310_hsm_of.internal_priority);
    osal_printf("DP: 0x%x\t\t\t\t", hsm.r9310_hsm_of.dp);
    osal_printf("TRK_HASH: 0x%x\t\t\t", hsm.r9310_hsm_of.trk_hash);
    osal_printf("META_DATA: 0x%x\n", hsm.r9310_hsm_of.meta_data);
    osal_printf("LOOPBACK_TIME: 0x%x\t\t", hsm.r9310_hsm_of.loopback_time);
    osal_printf("SP_IS_TRK: 0x%x\t\t\t", hsm.r9310_hsm_of.sp_is_trk);
    osal_printf("SP_TRK: 0x%x\n", hsm.r9310_hsm_of.sp_trk);
    osal_printf("SPN: 0x%x\t\t\t", hsm.r9310_hsm_of.spn);
    osal_printf("IN_SPN: 0x%x\t\t\t", hsm.r9310_hsm_of.in_spn);
    osal_printf("L3_OFFSET: 0x%x\n", hsm.r9310_hsm_of.l3_offset);
    osal_printf("L3_ERR_PKT: 0x%x\t\t\t", hsm.r9310_hsm_of.l3_err_pkt);
    osal_printf("L2_ERR_PKT: 0x%x\t\t\t", hsm.r9310_hsm_of.l2_err_pkt);
    osal_printf("L4_OFFSET: 0x%x\n", hsm.r9310_hsm_of.l4_offset);
    osal_printf("L4_HDR_RDY: 0x%x\t\t\t", hsm.r9310_hsm_of.l4_hdr_rdy);
    osal_printf("IP_FLAG_SET: 0x%x\t\t", hsm.r9310_hsm_of.ip_flag_set);
    osal_printf("IP_OFFSET_ZERO: 0x%x\n", hsm.r9310_hsm_of.ip_offset_zero);
    osal_printf("IP_PROT: 0x%x\t\t\t", hsm.r9310_hsm_of.ip_prot);
    osal_printf("IP6_EH_REPEAT: 0x%x\t\t", hsm.r9310_hsm_of.ip6_eh_repeat);
    osal_printf("IP6_EH_MOBILITY_EXIST: 0x%x\n", hsm.r9310_hsm_of.ip6_eh_mobility_exist);
    osal_printf("IP6_EH_DESTINATION_EXIST: 0x%x\t", hsm.r9310_hsm_of.ip6_eh_destination_exist);
    osal_printf("IP6_EH_AUTH_EXIST: 0x%x\t\t", hsm.r9310_hsm_of.ip6_eh_auth_exist);
    osal_printf("IP6_EH_ESP_EXIST: 0x%x\n", hsm.r9310_hsm_of.ip6_eh_esp_exist);
    osal_printf("IP6_EH_FRAGMENT_EXIST: 0x%x\t", hsm.r9310_hsm_of.ip6_eh_fragment_exist);
    osal_printf("IP6_EH_ROUTE_EXIST: 0x%x\t\t", hsm.r9310_hsm_of.ip6_eh_route_exist);
    osal_printf("IP6_EH_HOPBYHOP_EXIST: 0x%x\n", hsm.r9310_hsm_of.ip6_eh_hopbyhop_exist);
    osal_printf("IP6_EH_HOPBYHOP_ERR: 0x%x\t", hsm.r9310_hsm_of.ip6_eh_hopbyhop_err);
    osal_printf("PARSER_CANT_HANDLE: 0x%x\t\t", hsm.r9310_hsm_of.parser_cant_handle);
    osal_printf("ORG_IMPLS_IF: 0x%x\n", hsm.r9310_hsm_of.org_impls_if);
    osal_printf("ORG_OMPLS_IF: 0x%x\t\t", hsm.r9310_hsm_of.org_ompls_if);
    osal_printf("IPV6_IF: 0x%x\t\t\t", hsm.r9310_hsm_of.ipv6_if);
    osal_printf("IPV4_IF: 0x%x\n", hsm.r9310_hsm_of.ipv4_if);
    osal_printf("EVLAN_IF: 0x%x\t\t\t", hsm.r9310_hsm_of.evlan_if);
    osal_printf("FRAME_TYPE: 0x%x\t\t\t", hsm.r9310_hsm_of.frame_type);
    osal_printf("PPPOE_IF: 0x%x\n", hsm.r9310_hsm_of.pppoe_if);
    osal_printf("CLASS_B: 0x%x\t\t\t", hsm.r9310_hsm_of.class_b);
    osal_printf("CLASS_A: 0x%x\t\t\t", hsm.r9310_hsm_of.class_a);
    osal_printf("ORI_OTAG_TPID: 0x%x\n", hsm.r9310_hsm_of.ori_otag_tpid);
    osal_printf("ORI_OTAG_PRI: 0x%x\t\t", hsm.r9310_hsm_of.ori_otag_pri);
    osal_printf("ORI_OTAG_CFI: 0x%x\t\t", hsm.r9310_hsm_of.ori_otag_cfi);
    osal_printf("ORI_OTAG_VID: 0x%x\n", hsm.r9310_hsm_of.ori_otag_vid);
    osal_printf("ORI_ITAG_TPID: 0x%x\t\t", hsm.r9310_hsm_of.ori_itag_tpid);
    osal_printf("ORI_ITAG_PRI: 0x%x\t\t", hsm.r9310_hsm_of.ori_itag_pri);
    osal_printf("ORI_ITAG_CFI: 0x%x\n", hsm.r9310_hsm_of.ori_itag_cfi);
    osal_printf("ORI_ITAG_VID: 0x%x\t\t", hsm.r9310_hsm_of.ori_itag_vid);
    osal_printf("ORI_OTAG_IF: 0x%x\t\t", hsm.r9310_hsm_of.ori_otag_if);
    osal_printf("ORI_ITAG_IF: 0x%x\n", hsm.r9310_hsm_of.ori_itag_if);
    osal_printf("STACK_FMT: 0x%x\t\t\t", hsm.r9310_hsm_of.stack_fmt);
    osal_printf("OF_PIPELINE: 0x%x\t\t", hsm.r9310_hsm_of.of_pipeline);
    osal_printf("OF_IF: 0x%x\n", hsm.r9310_hsm_of.of_if);
    osal_printf("CPU_IF: 0x%x\t\t\t", hsm.r9310_hsm_of.cpu_if);
    osal_printf("STACK_IF: 0x%x\t\t\t", hsm.r9310_hsm_of.stack_if);
    osal_printf("PG_CNT: 0x%x\n", hsm.r9310_hsm_of.pg_cnt);
    osal_printf("PKT_LEN: 0x%x\t\t\t", hsm.r9310_hsm_of.pkt_len);
    osal_printf("HSB_EADR: 0x%x\t\t\t", hsm.r9310_hsm_of.hsb_eadr);
    osal_printf("HSB_SADR: 0x%x\n", hsm.r9310_hsm_of.hsb_sadr);
    osal_printf("L2MPLS_TT: 0x%x\t\t\t\n", hsm.r9310_hsm_of.l2mpls_tt);

    osal_printf("=======================================================================================\n");

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_dumpHsm_openflow
 * Description:
 *      Dump OpenFlow mode hsm paramter of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsm_openflow(uint32 unit)
{
    hal_control_t *pHalCtrl = NULL;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if (RT_ERR_OK != _hal_dumpHsm_openflow_9310(unit))
        {
            return RT_ERR_FAILED;
        }
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of hal_dumpHsm_openflow */
#endif


#if defined(CONFIG_SDK_RTL9300)
int32 _hal_getHsm_9300(uint32 unit, uint32 index, hsm_param_t *hsm)
{
    uint32 val = 0;
    uint32 tmpVal = 0;
    hal_control_t *pInfo;

    osal_memset(hsm, 0, sizeof(hsm_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    if (index == 0) {
        // HSM0_DATA0
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA0r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA0r, LONGAN_PKT_LENf, &tmpVal, &val);
        hsm->r9300.pre_data.pkt_len = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA0r, LONGAN_CTAG_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.ctag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA0r, LONGAN_DMAC_15_0f, &tmpVal, &val);
        hsm->r9300.pre_data.dmac[4] = (tmpVal & 0xFF00) >> 8;
        hsm->r9300.pre_data.dmac[5] = tmpVal & 0xFF;

        // HSM0_DATA1
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA1r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA1r, LONGAN_DMAC_47_16f, &tmpVal, &val);
        hsm->r9300.pre_data.dmac[0] = (tmpVal >> 24);
        hsm->r9300.pre_data.dmac[1] = (tmpVal & 0xFF0000) >> 16;
        hsm->r9300.pre_data.dmac[2] = (tmpVal & 0xFF00) >> 8;
        hsm->r9300.pre_data.dmac[3] = tmpVal & 0xFF;

        // HSM0_DATA2
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA2r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA2r, LONGAN_SMAC_31_0f, &tmpVal, &val);
        hsm->r9300.pre_data.smac[2] = (tmpVal >> 24);
        hsm->r9300.pre_data.smac[3] = (tmpVal & 0xFF0000) >> 16;
        hsm->r9300.pre_data.smac[4] = (tmpVal & 0xFF00) >> 8;
        hsm->r9300.pre_data.smac[5] = tmpVal & 0xFF;

        // HSM0_DATA3
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA3r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA3r, LONGAN_SMAC_47_32f, &tmpVal, &val);
        hsm->r9300.pre_data.smac[0] = (tmpVal & 0xFF00) >> 8;
        hsm->r9300.pre_data.smac[1] = tmpVal & 0xFF;
        reg_field_get(unit, LONGAN_HSM0_DATA3r, LONGAN_TRK_IDf, &tmpVal, &val);
        hsm->r9300.pre_data.trk_id = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA3r, LONGAN_SPHYf, &tmpVal, &val);
        hsm->r9300.pre_data.sphy = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA3r, LONGAN_STH_VLDf, &tmpVal, &val);
        hsm->r9300.pre_data.sth_vld = tmpVal;

        // HSM0_DATA4
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA4r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_SP_INFOf, &tmpVal, &val);
        hsm->r9300.pre_data.sp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_FWD_TYPEf, &tmpVal, &val);
        hsm->r9300.pre_data.fwd_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_DP_FMTf, &tmpVal, &val);
        hsm->r9300.pre_data.dp_fmt = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_DP_INFOf, &tmpVal, &val);
        hsm->r9300.pre_data.dp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_ST_SA_LRNf, &tmpVal, &val);
        hsm->r9300.pre_data.st_sa_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_ST_CTAG_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.st_ctag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_BYP_IGR_BWCTRLf, &tmpVal, &val);
        hsm->r9300.pre_data.byp_igr_bwctrl = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_PROTO_STORM_DROPf, &tmpVal, &val);
        hsm->r9300.pre_data.proto_storm_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA4r, LONGAN_DPf, &tmpVal, &val);
        hsm->r9300.pre_data.dp = tmpVal;

        // HSM0_DATA5
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA5r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_INT_PRIf, &tmpVal, &val);
        hsm->r9300.pre_data.int_pri = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_INT_PRI_ROUTEf, &tmpVal, &val);
        hsm->r9300.pre_data.int_pri_route = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_IVC_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.c2scact_ipri = tmpVal & 0x7;
        hsm->r9300.pre_data.c2scact_aipri = (tmpVal >> 3) & 0x1;
        hsm->r9300.pre_data.c2scact_itag = (tmpVal >> 4) & 0x1;
        hsm->r9300.pre_data.c2scact_aitag = (tmpVal >> 5) & 0x1;
        hsm->r9300.pre_data.c2scact_itpid = (tmpVal >> 6) & 0x3;
        hsm->r9300.pre_data.c2scact_aitpid = (tmpVal >> 8) & 0x1;
        hsm->r9300.pre_data.c2scact_opri = (tmpVal >> 9) & 0x7;
        hsm->r9300.pre_data.c2scact_aopri = (tmpVal >> 12) & 0x3;
        hsm->r9300.pre_data.c2scact_otag = (tmpVal >> 14) & 0x1;
        hsm->r9300.pre_data.c2scact_aotag = (tmpVal >> 15) & 0x1;
        hsm->r9300.pre_data.c2scact_otpid = (tmpVal >> 16) & 0x3;
        hsm->r9300.pre_data.c2scact_aotpid = (tmpVal >> 18) & 0x1;
        hsm->r9300.pre_data.macipact_pri = tmpVal & 0x7;
        hsm->r9300.pre_data.macipact_apri = (tmpVal >> 3) & 0x1;
        hsm->r9300.pre_data.macipact_tag = (tmpVal >> 4) & 0x1;
        hsm->r9300.pre_data.macipact_atag = (tmpVal >> 5) & 0x1;
        hsm->r9300.pre_data.macipact_tpid = (tmpVal >> 6) & 0x3;
        hsm->r9300.pre_data.macipact_atpid = (tmpVal >> 8) & 0x1;
        hsm->r9300.pre_data.macipact_type = (tmpVal >> 9) & 0x1;
        hsm->r9300.pre_data.macipact_ignore = (tmpVal >> 10) & 0x1;
        hsm->r9300.pre_data.macipact_fwdact = (tmpVal >> 11) & 0x3;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_IVC_HITf, &tmpVal, &val);
        hsm->r9300.pre_data.ivc_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_IVC_MODEf, &tmpVal, &val);
        hsm->r9300.pre_data.ivc_mode = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_RMA_BYP_STPf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_byp_stp = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_RMA_LRNf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA5r, LONGAN_RMA_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_act = tmpVal;


        // HSM0_DATA6
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA6r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_RMA_PKTf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_RMA_BYP_NEW_SA_DROPf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_byp_new_sa_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_GRAP_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.grap_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_ATK_HITf, &tmpVal, &val);
        hsm->r9300.pre_data.atk_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_ATK_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.atk_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_ATK_TYPEf, &tmpVal, &val);
        hsm->r9300.pre_data.atk_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_EAV_CLASS_Bf, &tmpVal, &val);
        hsm->r9300.pre_data.eav_class_b = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_EAV_CLASS_Af, &tmpVal, &val);
        hsm->r9300.pre_data.eav_class_a = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_CAPWAP_TRAPf, &tmpVal, &val);
        hsm->r9300.pre_data.capwap_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_ORG_ITAGf, &tmpVal, &val);
        hsm->r9300.pre_data.org_ivid = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_ORG_ITAG_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.org_itag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA6r, LONGAN_ORG_OTAG_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.org_otag_if = tmpVal;

        // HSM0_DATA7
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA7r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA7r, LONGAN_ORG_OTAGf, &tmpVal, &val);
        hsm->r9300.pre_data.org_ovid = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA7r, LONGAN_ACL_ACT_19_0f, &tmpVal, &val);
        hsm->r9300.pre_data.acl_act = tmpVal & 0xFFFFF;

        // HSM0_DATA8
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA8r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA8r, LONGAN_ACL_ACT_51_20f, &tmpVal, &val);
        hsm->r9300.pre_data.acl_act |= (tmpVal << 19);

        // HSM0_DATA9
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA9r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA9r, LONGAN_ACL_IDXf, &tmpVal, &val);
        hsm->r9300.pre_data.acl_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA9r, LONGAN_ACL_HITf, &tmpVal, &val);
        hsm->r9300.pre_data.acl_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA9r, LONGAN_IVC_DROPf, &tmpVal, &val);
        hsm->r9300.pre_data.ivc_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA9r, LONGAN_FWD_BASEf, &tmpVal, &val);
        hsm->r9300.pre_data.fwd_base = tmpVal;

        // HSM0_DATA10
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA10r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA10r, LONGAN_FWD_VIDf, &tmpVal, &val);
        hsm->r9300.pre_data.fwd_vid = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA10r, LONGAN_IGR_OVIDf, &tmpVal, &val);
        hsm->r9300.pre_data.igr_ovid = tmpVal;

        // HSM0_DATA11
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA11r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA11r, LONGAN_IGR_IVIDf, &tmpVal, &val);
        hsm->r9300.pre_data.igr_ivid = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA11r, LONGAN_VLAN_AFT_DROPf, &tmpVal, &val);
        hsm->r9300.pre_data.vlan_aft_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA11r, LONGAN_VLAN_CFI_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.vlan_cfi_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA11r, LONGAN_IACL_METER_DROPf, &tmpVal, &val);
        hsm->r9300.pre_data.iacl_meter_drop= tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA11r, LONGAN_VLAN_GROUP_MASKf, &tmpVal, &val);
        hsm->r9300.pre_data.vlan_group_mask = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA11r, LONGAN_MSTIf, &tmpVal, &val);
        hsm->r9300.pre_data.msti = tmpVal;

        // HSM0_DATA12
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA12r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_VLAN_PROFf, &tmpVal, &val);
        hsm->r9300.pre_data.vlan_prof = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_MCAST_HKEYf, &tmpVal, &val);
        hsm->r9300.pre_data.mcast_hkey = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_UCAST_HKEYf, &tmpVal, &val);
        hsm->r9300.pre_data.ucast_hkey = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_RX_PORT_IN_VLANf, &tmpVal, &val);
        hsm->r9300.pre_data.rx_port_in_vlan = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_IGR_STP_STATUSf, &tmpVal, &val);
        hsm->r9300.pre_data.igr_stp_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_OAM_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.oam_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_OAM_DIS_LRNf, &tmpVal, &val);
        hsm->r9300.pre_data.oam_dis_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_IACL_BYP_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.iacl_byp_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_IACL_METADATAf, &tmpVal, &val);
        hsm->r9300.pre_data.iacl_metadata = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_RMA_BYP_VLAN_FLTRf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_byp_vlan_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA12r, LONGAN_ERR_PKTf, &tmpVal, &val);
        hsm->r9300.pre_data.err_pkt = tmpVal;

        // HSM0_DATA13
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA13r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_RMA_LLDP_PKTf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_lldp_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_RMA_USR_PKTf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_usr_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_RMA_RSNf, &tmpVal, &val);
        hsm->r9300.pre_data.rma_rsn = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_RTAG_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.rtag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_DHCP_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.dhcp_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_RIP_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.rip_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_IP_RSV_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.ip_rsv_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA13r, LONGAN_MCMIS_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.macMis_act = tmpVal;

        // HSM0_DATA14
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA14r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA14r, LONGAN_IGR_VLAN_MBRf, &tmpVal, &val);
        hsm->r9300.pre_data.igr_vlan_mbr = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA14r, LONGAN_IPV4_PKTf, &tmpVal, &val);
        hsm->r9300.pre_data.ipv4_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA14r, LONGAN_IPV6_PKTf, &tmpVal, &val);
        hsm->r9300.pre_data.ipv6_pkt = tmpVal;

        // HSM0_DATA15
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA15r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA15r, LONGAN_ROUTE_ACT_31_0f, &tmpVal, &val);
        hsm->r9300.pre_data.route_act = tmpVal;

        // HSM0_DATA16
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA16r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA16r, LONGAN_ROUTE_ACT_61_32f, &tmpVal, &val);
        hsm->r9300.pre_data.route_act |=  (uint64)tmpVal<< 32;
        // HSM0_DATA17
        val = 0;
        reg_read(unit, LONGAN_HSM0_DATA17r, &val);
        reg_field_get(unit, LONGAN_HSM0_DATA17r, LONGAN_SPCL_ACTf, &tmpVal, &val);
        hsm->r9300.pre_data.sptrsn = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA17r, LONGAN_LUT_HASH_ADDR0f, &tmpVal, &val);
        hsm->r9300.pre_data.lut_haddr0 = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA17r, LONGAN_LUT_HASH_ADDR1f, &tmpVal, &val);
        hsm->r9300.pre_data.lut_haddr1 = tmpVal;
        reg_field_get(unit, LONGAN_HSM0_DATA17r, LONGAN_ST_RT_IFf, &tmpVal, &val);
        hsm->r9300.pre_data.st_rt_if = tmpVal;
    } else if (index == 1) {
        // HSM1_DATA0
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA0r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_TRK_IDf, &tmpVal, &val);
        hsm->r9300.lu_data.trk_id = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_SPHYf, &tmpVal, &val);
        hsm->r9300.lu_data.sphy = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_BYP_IGR_BWCTRLf, &tmpVal, &val);
        hsm->r9300.lu_data.byp_igr_bwctrl = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_PROTO_STORM_DROPf, &tmpVal, &val);
        hsm->r9300.lu_data.proto_storm_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_DPf, &tmpVal, &val);
        hsm->r9300.lu_data.dp = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_INT_PRIf, &tmpVal, &val);
        hsm->r9300.lu_data.int_pri = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA0r, LONGAN_INT_PRI_ROUTEf, &tmpVal, &val);
        hsm->r9300.lu_data.int_pri_route = tmpVal;

        // HSM1_DATA1
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA1r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_IVC_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.c2scact_ipri = tmpVal & 0x7;
        hsm->r9300.lu_data.c2scact_aipri = (tmpVal >> 3) & 0x1;
        hsm->r9300.lu_data.c2scact_itag = (tmpVal >> 4) & 0x1;
        hsm->r9300.lu_data.c2scact_aitag = (tmpVal >> 5) & 0x1;
        hsm->r9300.lu_data.c2scact_itpid = (tmpVal >> 6) & 0x3;
        hsm->r9300.lu_data.c2scact_aitpid = (tmpVal >> 8) & 0x1;
        hsm->r9300.lu_data.c2scact_opri = (tmpVal >> 9) & 0x7;
        hsm->r9300.lu_data.c2scact_aopri = (tmpVal >> 12) & 0x3;
        hsm->r9300.lu_data.c2scact_otag = (tmpVal >> 14) & 0x1;
        hsm->r9300.lu_data.c2scact_aotag = (tmpVal >> 15) & 0x1;
        hsm->r9300.lu_data.c2scact_otpid = (tmpVal >> 16) & 0x3;
        hsm->r9300.lu_data.c2scact_aotpid = (tmpVal >> 18) & 0x1;
        hsm->r9300.lu_data.macipact_pri = tmpVal & 0x7;
        hsm->r9300.lu_data.macipact_apri = (tmpVal >> 3) & 0x1;
        hsm->r9300.lu_data.macipact_tag = (tmpVal >> 4) & 0x1;
        hsm->r9300.lu_data.macipact_atag = (tmpVal >> 5) & 0x1;
        hsm->r9300.lu_data.macipact_tpid = (tmpVal >> 6) & 0x3;
        hsm->r9300.lu_data.macipact_atpid = (tmpVal >> 8) & 0x1;
        hsm->r9300.lu_data.macipact_type = (tmpVal >> 9) & 0x1;
        hsm->r9300.lu_data.macipact_ignore = (tmpVal >> 10) & 0x1;
        hsm->r9300.lu_data.macipact_fwdact = (tmpVal >> 11) & 0x3;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_IVC_HITf, &tmpVal, &val);
        hsm->r9300.lu_data.ivc_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_IVC_MODEf, &tmpVal, &val);
        hsm->r9300.lu_data.ivc_mode = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_RMA_BYP_STPf, &tmpVal, &val);
        hsm->r9300.lu_data.rma_byp_stp = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_RMA_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.rma_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_RMA_PKTf, &tmpVal, &val);
        hsm->r9300.lu_data.rma_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_ATK_HITf, &tmpVal, &val);
        hsm->r9300.lu_data.atk_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA1r, LONGAN_ATK_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.atk_act = tmpVal;

        // HSM1_DATA2
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA2r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA2r, LONGAN_ATK_TYPEf, &tmpVal, &val);
        hsm->r9300.lu_data.atk_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA2r, LONGAN_EAV_CLASS_Bf, &tmpVal, &val);
        hsm->r9300.lu_data.eav_class_b = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA2r, LONGAN_EAV_CLASS_Af, &tmpVal, &val);
        hsm->r9300.lu_data.eav_class_a = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA2r, LONGAN_CAPWAP_TRAPf, &tmpVal, &val);
        hsm->r9300.lu_data.capwap_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA2r, LONGAN_ACL_ACT_20_0f, &tmpVal, &val);
        hsm->r9300.lu_data.acl_act = tmpVal & 0x1FFFFF;

        // HSM1_DATA3
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA3r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA3r, LONGAN_ACL_ACT_51_21f, &tmpVal, &val);
        hsm->r9300.lu_data.acl_act |= (tmpVal << 21);

        // HSM1_DATA4
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA4r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA4r, LONGAN_ACL_IDXf, &tmpVal, &val);
        hsm->r9300.lu_data.acl_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA4r, LONGAN_ACL_HITf, &tmpVal, &val);
        hsm->r9300.lu_data.acl_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA4r, LONGAN_IVC_DROPf, &tmpVal, &val);
        hsm->r9300.lu_data.ivc_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA4r, LONGAN_FWD_BASEf, &tmpVal, &val);
        hsm->r9300.lu_data.fwd_base = tmpVal;

        // HSM1_DATA5
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA5r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA5r, LONGAN_FWD_VIDf, &tmpVal, &val);
        hsm->r9300.lu_data.fwd_vid = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA5r, LONGAN_IGR_OVIDf, &tmpVal, &val);
        hsm->r9300.lu_data.igr_ovid = tmpVal;

        // HSM1_DATA6
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA6r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA6r, LONGAN_IGR_IVIDf, &tmpVal, &val);
        hsm->r9300.lu_data.igr_ivid = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA6r, LONGAN_VLAN_AFT_DROPf, &tmpVal, &val);
        hsm->r9300.lu_data.vlan_aft_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA6r, LONGAN_VLAN_CFI_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.vlan_cfi_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA6r, LONGAN_IACL_METER_DROPf, &tmpVal, &val);
        hsm->r9300.lu_data.iacl_meter_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA6r, LONGAN_VLAN_GROUP_MASKf, &tmpVal, &val);
        hsm->r9300.lu_data.vlan_group_mask = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA6r, LONGAN_MSTIf, &tmpVal, &val);
        hsm->r9300.lu_data.msti = tmpVal;

        // HSM1_DATA7
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA7r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA7r, LONGAN_RX_PORT_IN_VLANf, &tmpVal, &val);
        hsm->r9300.lu_data.rx_port_in_vlan = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA7r, LONGAN_IGR_STP_STATUSf, &tmpVal, &val);
        hsm->r9300.lu_data.igr_stp_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA7r, LONGAN_OAM_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.oam_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA7r, LONGAN_L2_SA_ENTRYf, &tmpVal, &val);
        hsm->r9300.lu_data.l2_sa_trunk = (tmpVal >> 12) & 0x1;
        hsm->r9300.lu_data.l2_sa_slp = (tmpVal >> 7) & 0x1;
        hsm->r9300.lu_data.l2_sa_age = (tmpVal >> 4) & 0x7;
        hsm->r9300.lu_data.l2_sa_sab = (tmpVal >> 3) & 0x1;
        hsm->r9300.lu_data.l2_sa_dab = (tmpVal >> 2) & 0x1;
        hsm->r9300.lu_data.l2_sa_st = (tmpVal >> 1) & 0x1;
        hsm->r9300.lu_data.l2_sa_suspend = tmpVal & 0x1;
        reg_field_get(unit, LONGAN_HSM1_DATA7r, LONGAN_L2_SA_HITf, &tmpVal, &val);
        hsm->r9300.lu_data.l2_sa_hit = tmpVal;

        // HSM1_DATA8
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA8r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA8r, LONGAN_L2_DA_ENTRYf, &tmpVal, &val);
        hsm->r9300.lu_data.l2_da_trunk = (tmpVal >> 25) & 0x1;
        hsm->r9300.lu_data.l2_da_slp = (tmpVal >> 20) & 0x1F;
        hsm->r9300.lu_data.l2_da_age = (tmpVal >> 17) & 0x7;
        hsm->r9300.lu_data.l2_da_sab = (tmpVal >> 16) & 0x1;
        hsm->r9300.lu_data.l2_da_dab = (tmpVal >> 15) & 0x1;
        hsm->r9300.lu_data.l2_da_st = (tmpVal >> 14) & 0x1;
        hsm->r9300.lu_data.l2_da_suspend = (tmpVal >> 13) & 0x1;
        hsm->r9300.lu_data.l2_da_nh = (tmpVal >> 12) & 0x1;
        hsm->r9300.lu_data.l2_da_vid = tmpVal & 0xFFF;
        reg_field_get(unit, LONGAN_HSM1_DATA8r, LONGAN_L2_DA_HITf, &tmpVal, &val);
        hsm->r9300.lu_data.l2_da_hit = tmpVal;

        // HSM1_DATA9
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA9r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA9r, LONGAN_L3_DA_IDXf, &tmpVal, &val);
        hsm->r9300.lu_data.l3_da_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA9r, LONGAN_MAC_LIMIT_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.mac_limit_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA9r, LONGAN_MAC_LIMIT_VLAN_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.mac_limit_vlan_act = tmpVal;

        // HSM1_DATA10
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA10r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA10r, LONGAN_STTC_PM_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.sttc_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA10r, LONGAN_NEW_SA_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.new_sa_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA10r, LONGAN_DA_TYPEf, &tmpVal, &val);
        hsm->r9300.lu_data.da_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA10r, LONGAN_HASH_TYPEf, &tmpVal, &val);
        hsm->r9300.lu_data.hash_type = tmpVal;

        // HSM1_DATA11
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA11r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA11r, LONGAN_DPMf, &tmpVal, &val);
        hsm->r9300.lu_data.dpm = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA11r, LONGAN_IGR_VLAN_FLTR_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.igr_vlan_fltr_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA11r, LONGAN_IS_MCAST_SAf, &tmpVal, &val);
        hsm->r9300.lu_data.is_mcast_sa = tmpVal;

        // HSM1_DATA12
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA12r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_SRC_PORT_FLTR_ENf, &tmpVal, &val);
        hsm->r9300.lu_data.src_port_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_DYN_PM_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.dyn_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_FORBID_PM_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.forbid_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_ORG_SPAf, &tmpVal, &val);
        hsm->r9300.lu_data.org_spa = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_CTAG_FLAGf, &tmpVal, &val);
        hsm->r9300.lu_data.ctag_flag = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_IACL_BYP_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.iacl_byp_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_RMA_BYP_VLAN_FLTRf, &tmpVal, &val);
        hsm->r9300.lu_data.rma_byp_vlan_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_RMA_BYP_NEW_SA_DROPf, &tmpVal, &val);
        hsm->r9300.lu_data.rma_byp_new_sa_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_NEXTHOP_AGEOUTf, &tmpVal, &val);
        hsm->r9300.lu_data.nexthop_ageout = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA12r, LONGAN_NEXTHOP_IFf, &tmpVal, &val);
        hsm->r9300.lu_data.nexthop_if = tmpVal;

        // HSM1_DATA13
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA13r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_IACL_METADATAf, &tmpVal, &val);
        hsm->r9300.lu_data.iacl_metadata = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_L2_HASH_FULL_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.l2_hash_full = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_RMA_RSNf, &tmpVal, &val);
        hsm->r9300.lu_data.rma_rsn = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_DHCP_IFf, &tmpVal, &val);
        hsm->r9300.lu_data.dhcp_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_RIP_IFf, &tmpVal, &val);
        hsm->r9300.lu_data.rip_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_IP_RSV_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.ip_rsv_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_SPCL_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.sptrsn = tmpVal;

        // HSM1_DATA14
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA14r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA13r, LONGAN_ROUTE_ACT_31_0f, &tmpVal, &val);
        hsm->r9300.lu_data.route_act = tmpVal;

        // HSM1_DATA15
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA15r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA15r, LONGAN_ROUTE_ACT_41_32f, &tmpVal, &val);
        hsm->r9300.lu_data.route_act |= (uint64)tmpVal << 32;
        reg_field_get(unit, LONGAN_HSM1_DATA14r, LONGAN_PORT_MVf, &tmpVal, &val);
        hsm->r9300.lu_data.port_mv = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA14r, LONGAN_MCMIS_ACTf, &tmpVal, &val);
        hsm->r9300.lu_data.macMis_act = tmpVal;
        // HSM0_DATA16
        val = 0;
        reg_read(unit, LONGAN_HSM1_DATA16r, &val);
        reg_field_get(unit, LONGAN_HSM1_DATA16r, LONGAN_FWD_TYPEf, &tmpVal, &val);
        hsm->r9300.lu_data.fwd_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA16r, LONGAN_DP_FMTf, &tmpVal, &val);
        hsm->r9300.lu_data.dp_fmt = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA16r, LONGAN_DP_INFOf, &tmpVal, &val);
        hsm->r9300.lu_data.dp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA16r, LONGAN_ST_SA_LRNf, &tmpVal, &val);
        hsm->r9300.lu_data.sa_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA16r, LONGAN_TRK_UCf, &tmpVal, &val);
        hsm->r9300.lu_data.trk_uc = tmpVal;
        reg_field_get(unit, LONGAN_HSM1_DATA16r, LONGAN_TRK_HIT_IDXf, &tmpVal, &val);
        hsm->r9300.lu_data.trk_hit_idx = tmpVal;
    } else if (index == 2) {
        // HSM2_DATA0
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA0r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_OAM_PDUf, &tmpVal, &val);
        hsm->r9300.post0_data.oam_pdu = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_IPV4_IGMPf, &tmpVal, &val);
        hsm->r9300.post0_data.ipv4_igmp = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_IPV6_MLDf, &tmpVal, &val);
        hsm->r9300.post0_data.ipv6_mld = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_HOL_PRVNTf, &tmpVal, &val);
        hsm->r9300.post0_data.hol_prvnt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_IPV4_CHKSUM_OKf, &tmpVal, &val);
        hsm->r9300.post0_data.ipv4_chksum_ok = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_ERR_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.err_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_IP_TTLf, &tmpVal, &val);
        hsm->r9300.post0_data.ip_ttl = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA0r, LONGAN_IP_LENGTHf, &tmpVal, &val);
        hsm->r9300.post0_data.ip_length = tmpVal;

        // HSM2_DATA1
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA1r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA1r, LONGAN_PKT_LENf, &tmpVal, &val);
        hsm->r9300.post0_data.pkt_len = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA1r, LONGAN_LLC_OTHERf, &tmpVal, &val);
        hsm->r9300.post0_data.llc_other = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA1r, LONGAN_PPPOE_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.pppoe_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA1r, LONGAN_RFC1042f, &tmpVal, &val);
        hsm->r9300.post0_data.rfc1042 = tmpVal;

        // HSM2_DATA2
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA2r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA3r, LONGAN_ORG_OTPID_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.org_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA3r, LONGAN_ORG_ITPID_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.org_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_SPHYf, &tmpVal, &val);
        hsm->r9300.post0_data.sphy = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_IGR_ERRf, &tmpVal, &val);
        hsm->r9300.post0_data.igr_err = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_RTAG_TYPEf, &tmpVal, &val);
        hsm->r9300.post0_data.rtag_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_RTAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post0_data.rtag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_ETAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post0_data.etag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_CTAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post0_data.ctag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_ITAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post0_data.itag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA2r, LONGAN_OTAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post0_data.otag_exist = tmpVal;

        // HSM2_DATA3
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA3r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA3r, LONGAN_ITAG_CONTENTf, &tmpVal, &val);
        hsm->r9300.post0_data.ivid = tmpVal & 0xFFF;
        hsm->r9300.post0_data.icfi = (tmpVal >> 12) & 0x1;
        hsm->r9300.post0_data.ipri = (tmpVal >> 13) & 0x7;
        reg_field_get(unit, LONGAN_HSM2_DATA3r, LONGAN_OTAG_CONTENTf, &tmpVal, &val);
        hsm->r9300.post0_data.ovid = tmpVal & 0xFFF;
        hsm->r9300.post0_data.ocfi = (tmpVal >> 12) & 0x1;
        hsm->r9300.post0_data.opri = (tmpVal >> 13) & 0x7;

        // HSM2_DATA4
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA4r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_IPV6_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.ipv6_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_IPV4_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.ipv4_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_IS_MCAST_SAf, &tmpVal, &val);
        hsm->r9300.post0_data.is_mcast_sa = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_SA_ZEROf, &tmpVal, &val);
        hsm->r9300.post0_data.sa_zero = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_DA_MCASTf, &tmpVal, &val);
        hsm->r9300.post0_data.da_mcast = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_DA_BCASTf, &tmpVal, &val);
        hsm->r9300.post0_data.da_bcast = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_RMA_BPDU_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_bpdu_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_ETHf, &tmpVal, &val);
        hsm->r9300.post0_data.eth = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_RLDP_RLPP_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.rldp_rlpp_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_ARP_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.arp_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA4r, LONGAN_RMA_EAPOL_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_eapol_pkt = tmpVal;

        // HSM2_DATA5
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA5r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA5r, LONGAN_LST_DSCf, &tmpVal, &val);
        hsm->r9300.post0_data.lst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA5r, LONGAN_FST_DSCf, &tmpVal, &val);
        hsm->r9300.post0_data.fst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA5r, LONGAN_PAGE_CNTf, &tmpVal, &val);
        hsm->r9300.post0_data.page_cnt= tmpVal;

        // HSM2_DATA6
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA6r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_RMA_RSNf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_rsn = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_RMA_BYP_STPf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_byp_stp = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_RMA_BYP_VLAN_FLTRf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_byp_vlan_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_RMA_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_RMA_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_EAV_CLASS_Bf, &tmpVal, &val);
        hsm->r9300.post0_data.eav_class_b = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_EAV_CLASS_Af, &tmpVal, &val);
        hsm->r9300.post0_data.eav_class_a = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_PROTO_STORM_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.proto_storm_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_CAPWAP_TRAPf, &tmpVal, &val);
        hsm->r9300.post0_data.capwap_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_VLAN_AFT_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.vlan_aft_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_VLAN_CFI_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.vlan_cfi_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_IGR_STP_STATUSf, &tmpVal, &val);
        hsm->r9300.post0_data.igr_stp_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_L2_SA_BLOCKf, &tmpVal, &val);
        hsm->r9300.post0_data.l2_sa_sab = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_L2_SA_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.l2_sa_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA6r, LONGAN_DA_TYPEf, &tmpVal, &val);
        hsm->r9300.post0_data.da_type = tmpVal;

        // HSM2_DATA7
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA7r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA7r, LONGAN_DPMf, &tmpVal, &val);
        hsm->r9300.post0_data.dpm = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA7r, LONGAN_RX_PORT_IN_VLANf, &tmpVal, &val);
        hsm->r9300.post0_data.rx_port_in_vlan = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA7r, LONGAN_VLAN_SP_FLTR_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.vlan_src_port_fltr_act = tmpVal;

        // HSM2_DATA8
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA8r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_IACL_BYP_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_byp_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_RMA_BYP_NEW_SA_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.rma_byp_new_sa_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_TRK_IDf, &tmpVal, &val);
        hsm->r9300.post0_data.trk_id = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_SRC_PORT_FLTR_ENf, &tmpVal, &val);
        hsm->r9300.post0_data.src_port_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_HASH_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.hash_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_HASH_UCf, &tmpVal, &val);
        hsm->r9300.post0_data.hash_uc = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_UNKN_TRK_HASH_1f, &tmpVal, &val);
        hsm->r9300.post0_data.unkn_trk_hash_1 = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_UNKN_TRK_HASH_0f, &tmpVal, &val);
        hsm->r9300.post0_data.unkn_trk_hash_0 = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA8r, LONGAN_TRK_HASH_1f, &tmpVal, &val);
        hsm->r9300.post0_data.trk_hash_1 = tmpVal;

        // HSM2_DATA9
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA9r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_TRK_HASH_0f, &tmpVal, &val);
        hsm->r9300.post0_data.trk_hash_0 = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_EGR_OTPID_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.egr_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_EGR_ITPID_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.egr_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_EGR_OTPID_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.egr_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_EGR_ITPID_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.egr_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_INT_OCFIf, &tmpVal, &val);
        hsm->r9300.post0_data.int_ocfi = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_INT_ICFIf, &tmpVal, &val);
        hsm->r9300.post0_data.int_icfi = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_EGR_OTAG_STATUSf, &tmpVal, &val);
        hsm->r9300.post0_data.egr_otag_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_EGR_ITAG_STATUSf, &tmpVal, &val);
        hsm->r9300.post0_data.egr_itag_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_IVC_IACL_OPRI_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_iacl_opri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_IVC_IACL_IPRI_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_iacl_ipri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_IVC_IACL_OTAG_STATUS_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_iacl_otag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_IVC_IACL_ITAG_STATUS_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_iacl_itag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_ACL_REDIf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_redi = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA9r, LONGAN_ACL_CPU_FMTf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_cpu_fmt = tmpVal;

        // HSM2_DATA10
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA10r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_QIDf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_PRI_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_pri_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_MIR_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_mir_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_MIR_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_mir_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_OTPID_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_otpid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_ITPID_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_itpid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_OPRI_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_opri_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_IPRI_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_ipri_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_OTAG_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_otag_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_ITAG_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.n_1_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_N_1_OVID_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_EACL_OVID_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA10r, LONGAN_IGR_OVIDf, &tmpVal, &val);
        hsm->r9300.post0_data.igr_ovid = tmpVal;

        // HSM2_DATA11
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA11r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA11r, LONGAN_N_1_IVID_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA11r, LONGAN_EACL_IVID_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_ivid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA11r, LONGAN_IGR_IVIDf, &tmpVal, &val);
        hsm->r9300.post0_data.igr_ivid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA11r, LONGAN_ACL_RMK_VALf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_rmk_val = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA11r, LONGAN_EACL_RMK_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_rmk_hit = tmpVal;

        // HSM2_DATA12
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA12r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_IACL_MIR_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_mir_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_IACL_MIR_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_mir_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_ACL_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_ACL_TRAP_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_IACL_QIDf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_INT_PRI_ROUTEf, &tmpVal, &val);
        hsm->r9300.post0_data.int_pri_route = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_INT_PRIf, &tmpVal, &val);
        hsm->r9300.post0_data.int_pri = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA12r, LONGAN_BYP_IGR_BWCTRLf, &tmpVal, &val);
        hsm->r9300.post0_data.byp_igr_bwctrl = tmpVal;

        // HSM2_DATA13
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA13r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA13r, LONGAN_ATK_TYPEf, &tmpVal, &val);
        hsm->r9300.post0_data.atk_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA13r, LONGAN_ATK_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.atk_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA13r, LONGAN_OAM_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.oam_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA13r, LONGAN_MCRT_OIL_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.mcrt_oil_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA13r, LONGAN_MCRT_RPF_ASSTf, &tmpVal, &val);
        hsm->r9300.post0_data.mcrt_rpf_asst = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA13r, LONGAN_ORG_SPAf, &tmpVal, &val);
        hsm->r9300.post0_data.org_spa = tmpVal;

        // HSM2_DATA14
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA14r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA14r, LONGAN_L2_DA_BLOCKf, &tmpVal, &val);
        hsm->r9300.post0_data.l2_da_block = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA14r, LONGAN_L2_DA_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.l2_da_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA14r, LONGAN_FWD_VIDf, &tmpVal, &val);
        hsm->r9300.post0_data.fwd_vid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA14r, LONGAN_ORIG_FWD_VIDf, &tmpVal, &val);
        hsm->r9300.post0_data.orig_fwd_vid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA14r, LONGAN_FWD_BASEf, &tmpVal, &val);
        hsm->r9300.post0_data.fwd_base = tmpVal;

        // HSM2_DATA15
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA15r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_L3_DA_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.l3_da_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_HASH_TYPEf, &tmpVal, &val);
        hsm->r9300.post0_data.hash_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_DPf, &tmpVal, &val);
        hsm->r9300.post0_data.dp = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_TRK_MC_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.trk_mc_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_STORM_LKMISf, &tmpVal, &val);
        hsm->r9300.post0_data.storm_lkmis = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_NEXTHOP_AGEOUTf, &tmpVal, &val);
        hsm->r9300.post0_data.nexthop_ageout = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_NEXTHOP_IFf, &tmpVal, &val);
        hsm->r9300.post0_data.nexthop_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_IACL_METER_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_meter_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA15r, LONGAN_EACL_METER_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_meter_drop = tmpVal;

        // HSM2_DATA16
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA16r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IVC_BYP_IGR_VLAN_FLTRf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_byp_igr_vlan_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IVC_COPYf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_copy = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IVC_TRAP_MASTERf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_trap_master = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IVC_TRAPf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IVC_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.ivc_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_COPYf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_copy = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_TRAP_MASTERf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_trap_master = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_TRAPf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_REDIR_ZEROf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_redir_zero = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_DROP_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_drop_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IACL_COPYf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_copy = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IACL_TRAP_MASTERf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_trap_master = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IACL_TRAPf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IACL_REDIR_ZEROf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_redir_zero = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IACL_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_IACL_DROP_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.iacl_drop_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_EACL_DISABLEf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_disable = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_ACL_REDIR_COPYf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_rediCopy = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_GARP_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.eacl_disable = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_DHCP_IFf, &tmpVal, &val);
        hsm->r9300.post0_data.dhcp_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA16r, LONGAN_RIP_IFf, &tmpVal, &val);
        hsm->r9300.post0_data.rip_if = tmpVal;

        // HSM2_DATA17
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA17r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_ST_SA_LRNf, &tmpVal, &val);
        hsm->r9300.post0_data.sa_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_ST_DA_HITf, &tmpVal, &val);
        hsm->r9300.post0_data.st_da_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_TRK_HASHf, &tmpVal, &val);
        hsm->r9300.post0_data.trk_hash = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_MIR_IDf, &tmpVal, &val);
        hsm->r9300.post0_data.mir_id = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_ST_CTAG_IFf, &tmpVal, &val);
        hsm->r9300.post0_data.st_ctag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_FWD_TYPEf, &tmpVal, &val);
        hsm->r9300.post0_data.fwd_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_DP_INFOf, &tmpVal, &val);
        hsm->r9300.post0_data.dp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA17r, LONGAN_DP_FMTf, &tmpVal, &val);
        hsm->r9300.post0_data.dp_fmt = tmpVal;


        // HSM2_DATA18
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA18r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_SP_FLTR_ENf, &tmpVal, &val);
        hsm->r9300.post0_data.sp_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_SP_INFOf, &tmpVal, &val);
        hsm->r9300.post0_data.sp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_STK_IFf, &tmpVal, &val);
        hsm->r9300.post0_data.stk_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_LB_PKTf, &tmpVal, &val);
        hsm->r9300.post0_data.lb_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_LB_TTLf, &tmpVal, &val);
        hsm->r9300.post0_data.lb_ttl = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_ACL_LB_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.acl_lb_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_META_DATAf, &tmpVal, &val);
        hsm->r9300.post0_data.metadata = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_CTAG_FLAGf, &tmpVal, &val);
        hsm->r9300.post0_data.ctag_flag = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA18r, LONGAN_VB_ISO_MBRf, &tmpVal, &val);
        hsm->r9300.post0_data.vb_iso_mbr = tmpVal;

        // HSM2_DATA19
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA19r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_IP_RSVD_INVERTf, &tmpVal, &val);
        hsm->r9300.post0_data.ip_rsvd_inv = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_L2_HASH_FULL_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.l2_hash_full = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_FORCE_PMOVEf, &tmpVal, &val);
        hsm->r9300.post0_data.force_pmove = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_DYN_PM_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.dyn_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_STTC_PM_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.sttc_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_MAC_LIMIT_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.mac_limit_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_MAC_LIMIT_VLAN_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.mac_limit_vlan_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r , LONGAN_NEW_SA_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.new_sa_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA19r, LONGAN_SPCL_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.sptrsn = tmpVal;

        // HSM2_DATA20
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA20r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA20r, LONGAN_IP_RSV_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.ip_rsv_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA20r, LONGAN_MCMIS_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.macMiss_act = tmpVal;

        // HSM2_DATA21
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA21r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA21r, LONGAN_CTX_DEV_IDf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_unit = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA21r, LONGAN_CTX_DPM_27_0f, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_dpm = tmpVal;

        // HSM2_DATA22
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA22r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA22r, LONGAN_CTX_DPM_55_28f, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_dpm |=  (uint64)tmpVal << 28;
        reg_field_get(unit, LONGAN_HSM2_DATA22r, LONGAN_CTX_DPM_TYPEf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_dpm_type = tmpVal;

        // HSM2_DATA23
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_SP_FLTRf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_sp_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_FWD_VID_ENf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_fwd_vid_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_QIDf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_AS_QIDf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_as_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_ORI_TAGf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_org_tag = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_L3_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_l3_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_TAG_ASf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_as_tag = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_BP_VLAN_EGRf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_bp_egr_vlan = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_BP_STP_EGRf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_bp_stp = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_BP_FLTR_EGRf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_bp_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_DGf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_dg_gasp = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_CNGST_DROPf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_cngst_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA23r, LONGAN_CTX_ACL_ACTf, &tmpVal, &val);
        hsm->r9300.post0_data.ctx_acl_act = tmpVal;

        // HSM2_DATA24
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA24r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA24r, LONGAN_CRX_DATA_31_0f, &tmpVal, &val);
        hsm->r9300.post0_data.crx_data[0] = tmpVal;

        // HSM2_DATA24
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA25r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA25r, LONGAN_CRX_DATA_63_32f, &tmpVal, &val);
        hsm->r9300.post0_data.crx_data[1] = tmpVal;

        // HSM2_DATA26
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA26r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA26r, LONGAN_CRX_DATA_79_64f, &tmpVal, &val);
        hsm->r9300.post0_data.crx_data[2] = tmpVal & 0xffff;
        reg_field_get(unit, LONGAN_HSM2_DATA26r, LONGAN_L3_UC_KNf, &tmpVal, &val);
        hsm->r9300.post0_data.l3_uc_kn = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA26r, LONGAN_ST_PMSKf, &tmpVal, &val);
        hsm->r9300.post0_data.st_pmsk = tmpVal;

        // HSM2_DATA27
        val = 0;
        reg_read(unit, LONGAN_HSM2_DATA27r, &val);
        reg_field_get(unit, LONGAN_HSM2_DATA27r, LONGAN_RT_EVENTf, &tmpVal, &val);
        hsm->r9300.post0_data.rt_event = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA27r, LONGAN_L3_SA_IDXf, &tmpVal, &val);
        hsm->r9300.post0_data.l3_sa_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA27r, LONGAN_L3_SA_REPf, &tmpVal, &val);
        hsm->r9300.post0_data.l3_sa_rep = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA27r, LONGAN_L3_DA_REPf, &tmpVal, &val);
        hsm->r9300.post0_data.l3_da_rep = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA27r, LONGAN_L3_TTL_DECf, &tmpVal, &val);
        hsm->r9300.post0_data.l3_ttl_dec = tmpVal;
        reg_field_get(unit, LONGAN_HSM2_DATA27r, LONGAN_IP_ROUTE_ENf, &tmpVal, &val);
        hsm->r9300.post0_data.ip_route_en = tmpVal;
    } else {
        // HSM3_DATA0
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA0r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_OAM_PDUf, &tmpVal, &val);
        hsm->r9300.post_replication_data.oam_pdu = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_IPV4_IGMPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ipv4_igmp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_IPV6_MLDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ipv6_mld = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_HOL_PRVNTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.hol_prvnt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_IPV4_CHKSUM_OKf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ipv4_chksum_ok = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_ERR_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.err_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_IP_TTLf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ip_ttl = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA0r, LONGAN_IP_LENGTHf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ip_length = tmpVal;

        // HSM3_DATA1
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA1r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA1r, LONGAN_PKT_LENf, &tmpVal, &val);
        hsm->r9300.post_replication_data.pkt_len = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA1r, LONGAN_LLC_OTHERf, &tmpVal, &val);
        hsm->r9300.post_replication_data.llc_other = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA1r, LONGAN_PPPOE_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.pppoe_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA1r, LONGAN_RFC1042f, &tmpVal, &val);
        hsm->r9300.post_replication_data.rfc1042 = tmpVal;

        // HSM3_DATA2
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA2r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA3r, LONGAN_ORG_OTPID_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.org_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA3r, LONGAN_ORG_ITPID_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.org_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_SPHYf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sphy = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_IGR_ERRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.igr_err = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_RTAG_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rtag_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_RTAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rtag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_ETAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.etag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_CTAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_ITAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.itag_exist = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA2r, LONGAN_OTAG_EXISTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.otag_exist = tmpVal;

        // HSM3_DATA3
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA3r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA3r, LONGAN_ITAG_CONTENTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivid = tmpVal & 0xFFF;
        hsm->r9300.post_replication_data.icfi = (tmpVal >> 12) & 0x1;
        hsm->r9300.post_replication_data.ipri = (tmpVal >> 13) & 0x7;
        reg_field_get(unit, LONGAN_HSM3_DATA3r, LONGAN_OTAG_CONTENTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ovid = tmpVal & 0xFFF;
        hsm->r9300.post_replication_data.ocfi = (tmpVal >> 12) & 0x1;
        hsm->r9300.post_replication_data.opri = (tmpVal >> 13) & 0x7;

        // HSM3_DATA4
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA4r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_IPV6_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ipv6_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_IPV4_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ipv4_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_IS_MCAST_SAf, &tmpVal, &val);
        hsm->r9300.post_replication_data.is_mcast_sa = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_SA_ZEROf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sa_zero = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_DA_MCASTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.da_mcast = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_DA_BCASTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.da_bcast = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_RMA_BPDU_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_bpdu_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_ETHf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eth = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_RLDP_RLPP_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rldp_rlpp_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_ARP_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.arp_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA4r, LONGAN_RMA_EAPOL_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_eapol_pkt = tmpVal;

        // HSM3_DATA5
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA5r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA5r, LONGAN_LST_DSCf, &tmpVal, &val);
        hsm->r9300.post_replication_data.lst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA5r, LONGAN_FST_DSCf, &tmpVal, &val);
        hsm->r9300.post_replication_data.fst_dsc = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA5r, LONGAN_PAGE_CNTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.page_cnt= tmpVal;

        // HSM3_DATA6
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA6r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_RMA_RSNf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_rsn = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_RMA_BYP_STPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_byp_stp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_RMA_BYP_VLAN_FLTRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_byp_vlan_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_RMA_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_RMA_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_EAV_CLASS_Bf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eav_class_b = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_EAV_CLASS_Af, &tmpVal, &val);
        hsm->r9300.post_replication_data.eav_class_a = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_PROTO_STORM_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.proto_storm_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_CAPWAP_TRAPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.capwap_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_VLAN_AFT_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.vlan_aft_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_VLAN_CFI_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.vlan_cfi_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_IGR_STP_STATUSf, &tmpVal, &val);
        hsm->r9300.post_replication_data.igr_stp_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_L2_SA_BLOCKf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l2_sa_sab = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_L2_SA_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l2_sa_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA6r, LONGAN_DA_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.da_type = tmpVal;

        // HSM3_DATA7
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA7r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA7r, LONGAN_DPMf, &tmpVal, &val);
        hsm->r9300.post_replication_data.dpm = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA7r, LONGAN_RX_PORT_IN_VLANf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rx_port_in_vlan = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA7r, LONGAN_VLAN_SP_FLTR_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.vlan_src_port_fltr_act = tmpVal;

        // HSM3_DATA8
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA8r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_IACL_BYP_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_byp_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_RMA_BYP_NEW_SA_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rma_byp_new_sa_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_TRK_IDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.trk_id = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_SRC_PORT_FLTR_ENf, &tmpVal, &val);
        hsm->r9300.post_replication_data.src_port_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_HASH_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.hash_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_HASH_UCf, &tmpVal, &val);
        hsm->r9300.post_replication_data.hash_uc = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_UNKN_TRK_HASH_1f, &tmpVal, &val);
        hsm->r9300.post_replication_data.unkn_trk_hash_1 = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_UNKN_TRK_HASH_0f, &tmpVal, &val);
        hsm->r9300.post_replication_data.unkn_trk_hash_0 = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA8r, LONGAN_TRK_HASH_1f, &tmpVal, &val);
        hsm->r9300.post_replication_data.trk_hash_1 = tmpVal;

        // HSM3_DATA9
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA9r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_TRK_HASH_0f, &tmpVal, &val);
        hsm->r9300.post_replication_data.trk_hash_0 = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_EGR_OTPID_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.egr_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_EGR_ITPID_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.egr_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_EGR_OTPID_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.egr_otpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_EGR_ITPID_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.egr_itpid_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_INT_OCFIf, &tmpVal, &val);
        hsm->r9300.post_replication_data.int_ocfi = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_INT_ICFIf, &tmpVal, &val);
        hsm->r9300.post_replication_data.int_icfi = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_EGR_OTAG_STATUSf, &tmpVal, &val);
        hsm->r9300.post_replication_data.egr_otag_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_EGR_ITAG_STATUSf, &tmpVal, &val);
        hsm->r9300.post_replication_data.egr_itag_status = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_IVC_IACL_OPRI_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_iacl_opri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_IVC_IACL_IPRI_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_iacl_ipri_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_IVC_IACL_OTAG_STATUS_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_iacl_otag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_IVC_IACL_ITAG_STATUS_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_iacl_itag_status_as = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_ACL_REDIf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_redi = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA9r, LONGAN_ACL_CPU_FMTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_cpu_fmt = tmpVal;

        // HSM3_DATA10
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA10r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_QIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_PRI_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_pri_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_MIR_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_mir_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_MIR_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_mir_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_OTPID_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_otpid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_ITPID_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_itpid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_OPRI_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_opri_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_IPRI_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_ipri_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_OTAG_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_otag_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_ITAG_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.n_1_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_N_1_OVID_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_EACL_OVID_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA10r, LONGAN_IGR_OVIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.igr_ovid = tmpVal;

        // HSM3_DATA11
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA11r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA11r, LONGAN_N_1_IVID_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_ovid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA11r, LONGAN_EACL_IVID_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_ivid_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA11r, LONGAN_IGR_IVIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.igr_ivid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA11r, LONGAN_ACL_RMK_VALf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_rmk_val = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA11r, LONGAN_EACL_RMK_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_rmk_hit = tmpVal;

        // HSM3_DATA12
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA12r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_IACL_MIR_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_mir_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_IACL_MIR_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_mir_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_ACL_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_ACL_TRAP_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_IACL_QIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_INT_PRI_ROUTEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.int_pri_route = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_INT_PRIf, &tmpVal, &val);
        hsm->r9300.post_replication_data.int_pri = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA12r, LONGAN_BYP_IGR_BWCTRLf, &tmpVal, &val);
        hsm->r9300.post_replication_data.byp_igr_bwctrl = tmpVal;

        // HSM3_DATA13
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA13r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA13r, LONGAN_ATK_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.atk_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA13r, LONGAN_ATK_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.atk_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA13r, LONGAN_OAM_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.oam_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA13r, LONGAN_MCRT_OIL_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.mcrt_oil_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA13r, LONGAN_MCRT_RPF_ASSTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.mcrt_rpf_asst = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA13r, LONGAN_ORG_SPAf, &tmpVal, &val);
        hsm->r9300.post_replication_data.org_spa = tmpVal;

        // HSM3_DATA14
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA14r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA14r, LONGAN_L2_DA_BLOCKf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l2_da_block = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA14r, LONGAN_L2_DA_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l2_da_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA14r, LONGAN_FWD_VIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.fwd_vid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA14r, LONGAN_ORIG_FWD_VIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.orig_fwd_vid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA14r, LONGAN_FWD_BASEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.fwd_base = tmpVal;

        // HSM3_DATA15
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA15r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_L3_DA_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l3_da_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_HASH_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.hash_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_DPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.dp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_TRK_MC_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.trk_mc_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_STORM_LKMISf, &tmpVal, &val);
        hsm->r9300.post_replication_data.storm_lkmis = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_NEXTHOP_AGEOUTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.nexthop_ageout = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_NEXTHOP_IFf, &tmpVal, &val);
        hsm->r9300.post_replication_data.nexthop_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_IACL_METER_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_meter_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA15r, LONGAN_EACL_METER_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_meter_drop = tmpVal;

        // HSM3_DATA16
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA16r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IVC_BYP_IGR_VLAN_FLTRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_byp_igr_vlan_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IVC_COPYf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_copy = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IVC_TRAP_MASTERf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_trap_master = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IVC_TRAPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IVC_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ivc_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_COPYf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_copy = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_TRAP_MASTERf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_trap_master = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_TRAPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_REDIR_ZEROf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_redir_zero = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_DROP_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_drop_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IACL_COPYf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_copy = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IACL_TRAP_MASTERf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_trap_master = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IACL_TRAPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_trap = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IACL_REDIR_ZEROf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_redir_zero = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IACL_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_IACL_DROP_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.iacl_drop_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_EACL_DISABLEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_disable = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_ACL_REDIR_COPYf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_rediCopy = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_GARP_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.eacl_disable = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_DHCP_IFf, &tmpVal, &val);
        hsm->r9300.post_replication_data.dhcp_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA16r, LONGAN_RIP_IFf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rip_if = tmpVal;

        // HSM3_DATA17
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA17r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_ST_SA_LRNf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sa_lrn = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_ST_DA_HITf, &tmpVal, &val);
        hsm->r9300.post_replication_data.st_da_hit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_TRK_HASHf, &tmpVal, &val);
        hsm->r9300.post_replication_data.trk_hash = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_MIR_IDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.mir_id = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_ST_CTAG_IFf, &tmpVal, &val);
        hsm->r9300.post_replication_data.st_ctag_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_FWD_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.fwd_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_DP_INFOf, &tmpVal, &val);
        hsm->r9300.post_replication_data.dp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA17r, LONGAN_DP_FMTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.dp_fmt = tmpVal;


        // HSM3_DATA18
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA18r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_SP_FLTR_ENf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sp_fltr_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_SP_INFOf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sp_info = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_STK_IFf, &tmpVal, &val);
        hsm->r9300.post_replication_data.stk_if = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_LB_PKTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.lb_pkt = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_LB_TTLf, &tmpVal, &val);
        hsm->r9300.post_replication_data.lb_ttl = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_ACL_LB_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.acl_lb_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_META_DATAf, &tmpVal, &val);
        hsm->r9300.post_replication_data.metadata = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_CTAG_FLAGf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctag_flag = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA18r, LONGAN_VB_ISO_MBRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.vb_iso_mbr = tmpVal;

        // HSM3_DATA19
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA19r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_IP_RSVD_INVERTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ip_rsvd_inv = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_L2_HASH_FULL_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l2_hash_full = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_FORCE_PMOVEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.force_pmove = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_DYN_PM_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.dyn_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_STTC_PM_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sttc_pm_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_MAC_LIMIT_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.mac_limit_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_MAC_LIMIT_VLAN_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.mac_limit_vlan_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r , LONGAN_NEW_SA_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.new_sa_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA19r, LONGAN_SPCL_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.sptrsn = tmpVal;

        // HSM3_DATA20
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA20r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA20r, LONGAN_IP_RSV_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ip_rsv_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA20r, LONGAN_MCMIS_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.macMiss_act = tmpVal;

        // HSM3_DATA21
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA21r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA21r, LONGAN_CTX_DEV_IDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_unit = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA21r, LONGAN_CTX_DPM_27_0f, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_dpm = tmpVal;

        // HSM3_DATA22
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA22r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA22r, LONGAN_CTX_DPM_55_28f, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_dpm |=  (uint64)tmpVal << 28;
        reg_field_get(unit, LONGAN_HSM3_DATA22r, LONGAN_CTX_DPM_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_dpm_type = tmpVal;

        // HSM3_DATA23
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_SP_FLTRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_sp_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_FWD_VID_ENf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_fwd_vid_en = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_QIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_AS_QIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_as_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_ORI_TAGf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_org_tag = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_L3_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_l3_act = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_TAG_ASf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_as_tag = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_BP_VLAN_EGRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_bp_egr_vlan = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_BP_STP_EGRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_bp_stp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_BP_FLTR_EGRf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_bp_fltr = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_DGf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_dg_gasp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_CNGST_DROPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_cngst_drop = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA23r, LONGAN_CTX_ACL_ACTf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ctx_acl_act = tmpVal;

        // HSM3_DATA24
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA24r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA24r, LONGAN_CRX_DATA_31_0f, &tmpVal, &val);
        hsm->r9300.post_replication_data.crx_data[0] = tmpVal;

        // HSM3_DATA25
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA25r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA25r, LONGAN_CRX_DATA_63_32f, &tmpVal, &val);
        hsm->r9300.post_replication_data.crx_data[1] = tmpVal;

        // HSM3_DATA26
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA26r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA26r, LONGAN_CRX_DATA_79_64f, &tmpVal, &val);
        hsm->r9300.post_replication_data.crx_data[2] = tmpVal & 0xffff;
        reg_field_get(unit, LONGAN_HSM3_DATA26r, LONGAN_L3_UC_KNf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l3_uc_kn = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA26r, LONGAN_ST_PMSKf, &tmpVal, &val);
        hsm->r9300.post_replication_data.st_pmsk = tmpVal;

        // HSM3_DATA27
        val = 0;
        reg_read(unit, LONGAN_HSM3_DATA27r, &val);
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_REPLICATE_LST_CPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rt_ipmc_lstcp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_REPLICATE_FST_CPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rt_ipmc_fstcp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_REPLICATE_NONEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rt_ipmc_uccp = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_IPMC_QIDf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rt_ipmc_qid = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_REPLICATE_TYPEf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rt_ipmc_type = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_REPLICATE_TTL_DECf, &tmpVal, &val);
        hsm->r9300.post_replication_data.rt_ipmc_ttl_dec = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_L3_SA_IDXf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l3_sa_idx = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_L3_SA_REPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l3_sa_rep = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_L3_DA_REPf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l3_da_rep = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_L3_TTL_DECf, &tmpVal, &val);
        hsm->r9300.post_replication_data.l3_ttl_dec = tmpVal;
        reg_field_get(unit, LONGAN_HSM3_DATA27r, LONGAN_IP_ROUTE_ENf, &tmpVal, &val);
        hsm->r9300.post_replication_data.ip_route_en = tmpVal;
    }

    return RT_ERR_OK;
}
#endif


#if defined(CONFIG_SDK_RTL8380)
int32 _hal_getHsm_8380(uint32 unit, uint32 index, hsm_param_t *hsm)
{
    uint32 val = 0;
    uint32 tmpVal = 0;
#if defined(CONFIG_SDK_RTL8380)
    uint32 MAPLE_HSM_DATA[17];
#endif
    hal_control_t *pInfo;

    osal_memset(hsm, 0, sizeof(hsm_param_t));
    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8380)
    switch(index)
    {
    case 0:
        for(val = 0; val < 17; val++)
            MAPLE_HSM_DATA[val] = MAPLE_HSM0_DATA0r + val;
        break;

    case 1:
        for(val = 0; val < 17; val++)
            MAPLE_HSM_DATA[val] = MAPLE_HSM1_DATA0r + val;
        break;

    case 2:
        for(val = 0; val < 17; val++)
            MAPLE_HSM_DATA[val] = MAPLE_HSM2_DATA0r + val;
        break;

    default:
        break;
    }

    /* HSM_DATA0 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[0], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[0], MAPLE_ACL_IDX0f, &tmpVal, &val);
    hsm->r8380.acl_idx0 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[0], MAPLE_ACL_IDX1f, &tmpVal, &val);
    hsm->r8380.acl_idx1 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[0], MAPLE_ACL_IDX2f, &tmpVal, &val);
    hsm->r8380.acl_idx2 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[0], MAPLE_ACL_IDX3f, &tmpVal, &val);
    hsm->r8380.acl_idx3 = tmpVal;

    /* HSM_DATA1 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[1], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[1], MAPLE_ACL_IDX4f, &tmpVal, &val);
    hsm->r8380.acl_idx4 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[1], MAPLE_ACL_IDX5f, &tmpVal, &val);
    hsm->r8380.acl_idx5 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[1], MAPLE_ACL_IDX6f, &tmpVal, &val);
    hsm->r8380.acl_idx6 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[1], MAPLE_ACL_IDX7f, &tmpVal, &val);
    hsm->r8380.acl_idx7 = tmpVal;

    /* HSM_DATA2 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[2], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[2], MAPLE_ACL_ROUTf, &tmpVal, &val);
    hsm->r8380.acl_rout = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[2], MAPLE_ACL_REDIRf, &tmpVal, &val);
    hsm->r8380.acl_redir = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[2], MAPLE_ACL_COPY_HITf, &tmpVal, &val);
    hsm->r8380.acl_copy_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[2], MAPLE_ACL_DPMf, &tmpVal, &val);
    hsm->r8380.acl_dpm = tmpVal;

    /* HSM_DATA3 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[3], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_MIR_ACTf, &tmpVal, &val);
    hsm->r8380.acl_mir_act = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_MIR_HITf, &tmpVal, &val);
    hsm->r8380.acl_mir_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_CPU_PRIf, &tmpVal, &val);
    hsm->r8380.acl_cpuPri = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_CPUPRI_HITf, &tmpVal, &val);
    hsm->r8380.acl_cpuPri_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_NORMALPRI_HITf, &tmpVal, &val);
    hsm->r8380.acl_normalPri_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_EGRTAGSTS_HITf, &tmpVal, &val);
    hsm->r8380.acl_egr_tagSts_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_RMK_VALf, &tmpVal, &val);
    hsm->r8380.acl_rmk_val = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_RMK_ACTf, &tmpVal, &val);
    hsm->r8380.acl_rmk_act = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_RMK_HITf, &tmpVal, &val);
    hsm->r8380.acl_rmk_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_LOG_HITf, &tmpVal, &val);
    hsm->r8380.acl_log_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[3], MAPLE_ACL_FWD_INFOf, &tmpVal, &val);
    hsm->r8380.acl_fwd_info = tmpVal;

    /* HSM_DATA4 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[4], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[4], MAPLE_ACL_OVID_HITf, &tmpVal, &val);
    hsm->r8380.acl_ovid_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[4], MAPLE_ACL_IVID_HITf, &tmpVal, &val);
    hsm->r8380.acl_ivid_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[4], MAPLE_ACL_FLT_HITf, &tmpVal, &val);
    hsm->r8380.acl_fltr_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[4], MAPLE_ACL_FLT_DPMf, &tmpVal, &val);
    hsm->r8380.acl_fltr_dpm = tmpVal;

    /* HSM_DATA5 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[5], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_COPY_TO_CPUf, &tmpVal, &val);
    hsm->r8380.copyToCpu = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ALE_TRAPf, &tmpVal, &val);
    hsm->r8380.ale_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ALE_DROPf, &tmpVal, &val);
    hsm->r8380.ale_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_RNG_CHK_IPf, &tmpVal, &val);
    hsm->r8380.rng_chk_ip = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_METER_HITf, &tmpVal, &val);
    hsm->r8380.acl_meter = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_ITPID_IDXf, &tmpVal, &val);
    hsm->r8380.acl_itpid_idx = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_ITPID_HITf, &tmpVal, &val);
    hsm->r8380.acl_itpid_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_OTPID_IDXf, &tmpVal, &val);
    hsm->r8380.acl_otpid_idx = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_OTPID_HITf, &tmpVal, &val);
    hsm->r8380.acl_otpid_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_MIR_IDXf, &tmpVal, &val);
    hsm->r8380.acl_mir_idx = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_CPUTAGf, &tmpVal, &val);
    hsm->r8380.acl_cpuTag = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_FORCEf, &tmpVal, &val);
    hsm->r8380.acl_force = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[5], MAPLE_ACL_IDX_9_0f, &tmpVal, &val);
    hsm->r8380.acl_idx_9_0 = tmpVal;

    /* HSM_DATA6 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[6], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[6], MAPLE_RNG_CHK_SPMf, &tmpVal, &val);
    hsm->r8380.rng_chk_spm = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[6], MAPLE_RNG_CHK_COMMf, &tmpVal, &val);
    hsm->r8380.rng_chk_comm = tmpVal;

    /* HSM_DATA7 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[7], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[7], MAPLE_RMA_FLOODf, &tmpVal, &val);
    hsm->r8380.rma_fld = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[7], MAPLE_L2_DA_IDXf, &tmpVal, &val);
    hsm->r8380.l2_da_idx = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[7], MAPLE_L2_DA_HITf, &tmpVal, &val);
    hsm->r8380.l2_da_hit = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[7], MAPLE_L2_DA_LKMISSf, &tmpVal, &val);
    hsm->r8380.l2_da_lm = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[7], MAPLE_L2_SA_IDXf, &tmpVal, &val);
    hsm->r8380.l2_sa_idx = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[7], MAPLE_L2_SA_HITf, &tmpVal, &val);
    hsm->r8380.l2_sa_hit = tmpVal;

    /* HSM_DATA8 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[8], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[8], MAPLE_INT_PRIf, &tmpVal, &val);
    hsm->r8380.int_pri = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[8], MAPLE_DPMf, &tmpVal, &val);
    hsm->r8380.dpm = tmpVal;

    /* HSM_DATA9 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[9], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_FIDf, &tmpVal, &val);
    hsm->r8380.fid = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_VLAN_FWD_BASEf, &tmpVal, &val);
    hsm->r8380.vlan_fwd_base = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_COPY_BYP_VLAN_IFILTERf, &tmpVal, &val);
    hsm->r8380.copy_igrVlan_lky = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_BYP_STPf, &tmpVal, &val);
    hsm->r8380.byp_igr_stp = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_BYP_STORMf, &tmpVal, &val);
    hsm->r8380.byp_storm = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_CPU_PRIf, &tmpVal, &val);
    hsm->r8380.cpu_pri = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_SRC_LOGIC_PORTf, &tmpVal, &val);
    hsm->r8380.slp = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_EGR_VLAN_LKYf, &tmpVal, &val);
    hsm->r8380.egr_vlan_lky = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_EAV_CLASS_Bf, &tmpVal, &val);
    hsm->r8380.eav_class_b = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_EAV_CLASS_Af, &tmpVal, &val);
    hsm->r8380.eav_class_a = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[9], MAPLE_ATK_TYPEf, &tmpVal, &val);
    hsm->r8380.atk_prvnt_rsn = tmpVal;

    /* HSM_DATA10 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[10], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_L2_DIS_SA_LRNf, &tmpVal, &val);
    hsm->r8380.dis_sa_lrn = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_IGR_VLAN_LKYf, &tmpVal, &val);
    hsm->r8380.igr_vlan_lky = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_INPUT_QIDf, &tmpVal, &val);
    hsm->r8380.input_qid = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_OTAG_STSf, &tmpVal, &val);
    hsm->r8380.otag_sts = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_ITAG_STSf, &tmpVal, &val);
    hsm->r8380.itag_sts = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_ALE_IVIDf, &tmpVal, &val);
    hsm->r8380.ale_ivid = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[10], MAPLE_ALE_OVIDf, &tmpVal, &val);
    hsm->r8380.ale_ovid = tmpVal;

    /* HSM_DATA11 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[11], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[11], MAPLE_COPY_BYP_STPf, &tmpVal, &val);
    hsm->r8380.copy_igrStp_lky = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[11], MAPLE_BYP_EGR_STPf, &tmpVal, &val);
    hsm->r8380.byp_egr_stp_pmsk = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[11], MAPLE_BYP_IGR_BWCTRLf, &tmpVal, &val);
    hsm->r8380.byp_igr_bwctrl = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[11], MAPLE_KNMC_BYP_VLAN_EFILTERf, &tmpVal, &val);
    hsm->r8380.knmc_byp_vlan_efilter = tmpVal;

    /* HSM_DATA12 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[12], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_MIR_FLTR_DROPf, &tmpVal, &val);
    hsm->r8380.mir_filter_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_TRUNK_DROPf, &tmpVal, &val);
    hsm->r8380.trunk_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ISOLATION_DROPf, &tmpVal, &val);
    hsm->r8380.isolation_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_SELF_FLTR_DROPf, &tmpVal, &val);
    hsm->r8380.src_port_filter_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_STORM_DROPf, &tmpVal, &val);
    hsm->r8380.storm_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_STP_EFILTER_DROPf, &tmpVal, &val);
    hsm->r8380.mstp_egr_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_VLAN_EFILTER_DROPf, &tmpVal, &val);
    hsm->r8380.vlan_egr_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_INVLD_MCASTDPM_DROPf, &tmpVal, &val);
    hsm->r8380.invld_mcast_dpm_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_INVLD_DPM_DROPf, &tmpVal, &val);
    hsm->r8380.invld_dpm_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_DA_LKMISS_DROPf, &tmpVal, &val);
    hsm->r8380.l2_lm_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_DA_BLK_DROPf, &tmpVal, &val);
    hsm->r8380.l2_da_blk = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_VLAN_MAC_CONSTRT_DROPf, &tmpVal, &val);
    hsm->r8380.l2_vlan_constrt_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_PORT_MAC_CONSTRT_DROPf, &tmpVal, &val);
    hsm->r8380.l2_port_constrt_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_SYS_MAC_CONSTRT_DROPf, &tmpVal, &val);
    hsm->r8380.l2_sys_constrt_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_NEW_SA_DROPf, &tmpVal, &val);
    hsm->r8380.l2_newSA_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_PORT_MV_DROPf, &tmpVal, &val);
    hsm->r8380.l2_portMove_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_SA_BLK_DROPf, &tmpVal, &val);
    hsm->r8380.l2_sa_blk = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_SELFMAC_DROPf, &tmpVal, &val);
    hsm->r8380.sTrap_selfMac_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_L2_INVLD_SA_DROPf, &tmpVal, &val);
    hsm->r8380.l2_invldSA_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_STP_IFILTER_DROPf, &tmpVal, &val);
    hsm->r8380.mstp_igr_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_VLAN_IFILTER_DROPf, &tmpVal, &val);
    hsm->r8380.vlan_igr_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_VLAN_CFI_DROPf, &tmpVal, &val);
    hsm->r8380.vlan_cfi_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_VLAN_AFT_DROPf, &tmpVal, &val);
    hsm->r8380.vlan_aft_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ACL_METER_DROPf, &tmpVal, &val);
    hsm->r8380.acl_meter_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ACL_REDIR_DROPf, &tmpVal, &val);
    hsm->r8380.acl_redir_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_INPUT_Q_DROPf, &tmpVal, &val);
    hsm->r8380.input_q_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ROUT_DROPf, &tmpVal, &val);
    hsm->r8380.l3_rout_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ACL_DROPf, &tmpVal, &val);
    hsm->r8380.acl_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ACL_LKMISS_DROPf, &tmpVal, &val);
    hsm->r8380.acl_lm_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_ATK_DROPf, &tmpVal, &val);
    hsm->r8380.atk_prvnt_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_RMA_DROPf, &tmpVal, &val);
    hsm->r8380.rma_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[12], MAPLE_RSPAN_DROPf, &tmpVal, &val);
    hsm->r8380.rspan_drop = tmpVal;

    /* HSM_DATA13 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[13], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_REASON_COPYf, &tmpVal, &val);
    hsm->r8380.reason_n_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_ACL_COPYf, &tmpVal, &val);
    hsm->r8380.acl_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_L2_DA_LKMISS_COPYf, &tmpVal, &val);
    hsm->r8380.l2_lm_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_L2_PORT_MV_COPYf, &tmpVal, &val);
    hsm->r8380.l2_portMove_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_L2_NEW_SA_COPYf, &tmpVal, &val);
    hsm->r8380.l2_newSA_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_L2_VLAN_MAC_CONSTRT_COPYf, &tmpVal, &val);
    hsm->r8380.l2_vlan_constrt_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_L2_PORT_MAC_CONSTRT_COPYf, &tmpVal, &val);
    hsm->r8380.l2_port_constrt_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_L2_SYS_MAC_CONSTRT_COPYf, &tmpVal, &val);
    hsm->r8380.l2_sys_constrt_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_SPECIAL_COPYf, &tmpVal, &val);
    hsm->r8380.sTrap_copy = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_REASON_DROPf, &tmpVal, &val);
    hsm->r8380.reason_n_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_MAX_LEN_DROPf, &tmpVal, &val);
    hsm->r8380.tx_maxLen_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_FLOWCTRL_DROPf, &tmpVal, &val);
    hsm->r8380.flowCtrl_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_LINK_DOWN_DROPf, &tmpVal, &val);
    hsm->r8380.link_down_drop = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[13], MAPLE_ACL_FLTR_DROPf, &tmpVal, &val);
    hsm->r8380.acl_filter_drop = tmpVal;

    /* HSM_DATA14 */
    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[14], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_REASON_TRAPf, &tmpVal, &val);
    hsm->r8380.reason_n_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_L2_DA_LKMISS_TRAPf, &tmpVal, &val);
    hsm->r8380.l2_lm_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_L2_VLAN_MAC_CONSTRT_TRAPf, &tmpVal, &val);
    hsm->r8380.l2_vlan_constrt_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_L2_PORT_MAC_CONSTRT_TRAPf, &tmpVal, &val);
    hsm->r8380.l2_port_constrt_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_L2_SYS_MAC_CONSTRT_TRAPf, &tmpVal, &val);
    hsm->r8380.l2_sys_constrt_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_L2_NEW_SA_TRAPf, &tmpVal, &val);
    hsm->r8380.l2_newSA_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_L2_PORT_MV_TRAPf, &tmpVal, &val);
    hsm->r8380.l2_portMove_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_SELFMAC_TRAPf, &tmpVal, &val);
    hsm->r8380.sTrap_selfMac_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_VLAN_IFILTER_TRAPf, &tmpVal, &val);
    hsm->r8380.vlan_igr_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_VLAN_CFI_TRAPf, &tmpVal, &val);
    hsm->r8380.vlan_cfi_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_ROUT_TRAPf, &tmpVal, &val);
    hsm->r8380.l3_rout_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_ACL_TRAPf, &tmpVal, &val);
    hsm->r8380.acl_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_SPECIAL_TRAPf, &tmpVal, &val);
    hsm->r8380.sTrap_comm_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_RLDP_RLPP_TRAPf, &tmpVal, &val);
    hsm->r8380.rldp_rlpp_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_ATK_TRAPf, &tmpVal, &val);
    hsm->r8380.atk_prvnt_trap = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[14], MAPLE_RMA_TRAPf, &tmpVal, &val);
    hsm->r8380.rma_trap = tmpVal;

    /* HSM_DATA15 */

    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[15], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[15], MAPLE_ACL_IDX8f, &tmpVal, &val);
    hsm->r8380.acl_idx8 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[15], MAPLE_ACL_IDX9f, &tmpVal, &val);
    hsm->r8380.acl_idx9 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[15], MAPLE_ACL_IDX10f, &tmpVal, &val);
    hsm->r8380.acl_idx10 = tmpVal;
    reg_field_get(unit, MAPLE_HSM_DATA[15], MAPLE_ACL_IDX11f, &tmpVal, &val);
    hsm->r8380.acl_idx11 = tmpVal;

    /* HSM_DATA16 */

    val = 0;
    reg_read(unit, MAPLE_HSM_DATA[16], &val);
    reg_field_get(unit, MAPLE_HSM_DATA[16], MAPLE_ACL_IDX_10f, &tmpVal, &val);
    hsm->r8380.acl_idx_10 = tmpVal;
#endif


    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9300)
int32 _hal_dumpHsm_9300(uint32 unit, uint32 index)
{

    hsm_param_t   hsm;
    hal_control_t *pInfo;

    if(RT_ERR_OK != _hal_getHsm_9300(unit, index, &hsm))
    {
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    osal_printf("\n========================================  HSM %u  ========================================\n", index);
    if (index == 0) {
        // HSM0_DATA0
        osal_printf("dmac: 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\t", hsm.r9300.pre_data.dmac[0], hsm.r9300.pre_data.dmac[1],
                                                             hsm.r9300.pre_data.dmac[2], hsm.r9300.pre_data.dmac[3],
                                                             hsm.r9300.pre_data.dmac[4], hsm.r9300.pre_data.dmac[5]);
        osal_printf("smac: 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\t", hsm.r9300.pre_data.smac[0], hsm.r9300.pre_data.smac[1],
                                                             hsm.r9300.pre_data.smac[2], hsm.r9300.pre_data.smac[3],
                                                             hsm.r9300.pre_data.smac[4], hsm.r9300.pre_data.smac[5]);
        osal_printf("rtag_if: %d\n", hsm.r9300.pre_data.rtag_if);
        osal_printf("pkt_len: 0x%4x\t\t\t", hsm.r9300.pre_data.pkt_len);
        osal_printf("ctag_if: 0x%x\t\t\t", hsm.r9300.pre_data.ctag_if);
        osal_printf("trk_id: %2d\n", hsm.r9300.pre_data.trk_id);
        osal_printf("sphy: %2d\t\t\t", hsm.r9300.pre_data.sphy);
        osal_printf("byp_igr_bwctrl: %d\t\t", hsm.r9300.pre_data.byp_igr_bwctrl);
        osal_printf("proto_storm_drop: %d\n", hsm.r9300.pre_data.proto_storm_drop);

        osal_printf("sth_vld: %2d\t\t\t", hsm.r9300.pre_data.sth_vld);
        osal_printf("sp_info: %d\t\t\t", hsm.r9300.pre_data.sp_info);
        osal_printf("fwd_type: %d\n", hsm.r9300.pre_data.fwd_type);
        osal_printf("dp_info: %2d\t\t\t", hsm.r9300.pre_data.dp_info);
        osal_printf("dp_fmt: %d\t\t\t", hsm.r9300.pre_data.dp_fmt);
        osal_printf("st_sa_lrn: %d\n", hsm.r9300.pre_data.st_sa_lrn);
        osal_printf("st_ctag_if: %d\t\t\t", hsm.r9300.pre_data.st_ctag_if);
        osal_printf("dp: %d\t\t\t\t", hsm.r9300.pre_data.dp);

        // HSM0_DATA5
        osal_printf("int_pri: %d\t\t\n", hsm.r9300.pre_data.int_pri);
        if (!hsm.r9300.pre_data.ivc_mode) {
            osal_printf("c2scact_ipri: %d\t\t\t", hsm.r9300.pre_data.c2scact_ipri);
            osal_printf("c2scact_aipri: %d\t\t", hsm.r9300.pre_data.c2scact_aipri);
            osal_printf("c2scact_itag: %d\n", hsm.r9300.pre_data.c2scact_itag);
            osal_printf("c2scact_aitag: %d\t\t", hsm.r9300.pre_data.c2scact_aitag);
            osal_printf("c2scact_itpid: %d\t\t", hsm.r9300.pre_data.c2scact_itpid);
            osal_printf("c2scact_aitpid: %d\n", hsm.r9300.pre_data.c2scact_aitpid);
            osal_printf("c2scact_opri: %d\t\t\t", hsm.r9300.pre_data.c2scact_opri);
            osal_printf("c2scact_aopri: %d\t\t", hsm.r9300.pre_data.c2scact_aopri);
            osal_printf("c2scact_otag: %d\n", hsm.r9300.pre_data.c2scact_otag);
            osal_printf("c2scact_aotag: %d\t\t", hsm.r9300.pre_data.c2scact_aotag);
            osal_printf("c2scact_otpid: %d\t\t", hsm.r9300.pre_data.c2scact_otpid);
            osal_printf("c2scact_aotpid: %d\n", hsm.r9300.pre_data.c2scact_aotpid);
        } else {
            osal_printf("macipact_pri: %d\t\t", hsm.r9300.pre_data.macipact_pri);
            osal_printf("macipact_apri: %d\t\t", hsm.r9300.pre_data.macipact_apri);
            osal_printf("macipact_tag: %d\n", hsm.r9300.pre_data.macipact_tag);
            osal_printf("macipact_atag: %d\t\t", hsm.r9300.pre_data.macipact_atag);
            osal_printf("macipact_tpid: %d\t\t", hsm.r9300.pre_data.macipact_tpid);
            osal_printf("macipact_atpid: %d\n", hsm.r9300.pre_data.macipact_atpid);
            osal_printf("macipact_type: %d\t\t", hsm.r9300.pre_data.macipact_type);
            osal_printf("macipact_ignore: %d\t\t", hsm.r9300.pre_data.macipact_ignore);
            osal_printf("macipact_fwdact: %d\n", hsm.r9300.pre_data.macipact_fwdact);
        }
        osal_printf("ivc_hit: %d\t\t\t", hsm.r9300.pre_data.ivc_hit);
        osal_printf("ivc_mode: %d\t\t\t", hsm.r9300.pre_data.ivc_mode);
        osal_printf("rma_byp_stp: %d\n", hsm.r9300.pre_data.rma_byp_stp);
        osal_printf("rma_lrn: %d\t\t\t", hsm.r9300.pre_data.rma_lrn);
        osal_printf("rma_act: %d\t\t\t", hsm.r9300.pre_data.rma_act);

        // HSM0_DATA6
        osal_printf("rma_pkt: %d\n", hsm.r9300.pre_data.rma_pkt);
        osal_printf("rma_byp_new_sa_drop: %d\t\t", hsm.r9300.pre_data.rma_byp_new_sa_drop);
        osal_printf("grap_act: %d\t\t\t", hsm.r9300.pre_data.grap_act);
        osal_printf("atk_hit: %d\n", hsm.r9300.pre_data.atk_hit);
        osal_printf("atk_act: %d\t\t\t", hsm.r9300.pre_data.atk_act);
        osal_printf("atk_type: %2d\t\t\t", hsm.r9300.pre_data.atk_type);
        osal_printf("eav_class_b: %d\n", hsm.r9300.pre_data.eav_class_b);
        osal_printf("eav_class_a: %d\t\t\t", hsm.r9300.pre_data.eav_class_a);
        osal_printf("capwap_trap: %d\t\t\t", hsm.r9300.pre_data.capwap_trap);
        osal_printf("org_ivid: 0x%3x\n", hsm.r9300.pre_data.org_ivid);
        osal_printf("org_itag_if: %d\t\t\t", hsm.r9300.pre_data.org_itag_if);

        // HSM0_DATA7
        osal_printf("org_ovid: 0x%3x\t\t\t", hsm.r9300.pre_data.org_ovid);
        osal_printf("org_otag_if: %d\n", hsm.r9300.pre_data.org_otag_if);

        // HSM0_DATA9
        osal_printf("acl_idx: 0x%3x\t\t\t", hsm.r9300.pre_data.acl_idx);
        osal_printf("acl_hit: 0x%2x\t\t\t", hsm.r9300.pre_data.acl_hit);
        osal_printf("sptrsn: %2d\n", hsm.r9300.pre_data.sptrsn);
        osal_printf("ivc_drop: %d\t\t\t", hsm.r9300.pre_data.ivc_drop);
        osal_printf("fwd_base: %d\t\t\t", hsm.r9300.pre_data.fwd_base);

        // HSM0_DATA10
        osal_printf("fwd_vid: 0x%3x\n", hsm.r9300.pre_data.fwd_vid);
        osal_printf("igr_ovid: 0x%3x\t\t\t", hsm.r9300.pre_data.igr_ovid);

        // HSM0_DATA11
        osal_printf("igr_ivid: 0x%3x\t\t\t", hsm.r9300.pre_data.igr_ivid);
        osal_printf("vlan_aft_drop: %d\n", hsm.r9300.pre_data.vlan_aft_drop);
        osal_printf("vlan_cfi_act: %d\t\t\t", hsm.r9300.pre_data.vlan_cfi_act);
        osal_printf("iacl_meter_drop: %d\t\t", hsm.r9300.pre_data.iacl_meter_drop);
        osal_printf("vlan_group_mask: 0x%2x\n", hsm.r9300.pre_data.vlan_group_mask);
        osal_printf("msti: 0x%2x\t\t\t", hsm.r9300.pre_data.msti);

        // HSM0_DATA12
        osal_printf("vlan_prof: %d\t\t\t", hsm.r9300.pre_data.vlan_prof);
        osal_printf("mcast_hkey: %d\n", hsm.r9300.pre_data.mcast_hkey);
        osal_printf("ucast_hkey: %d\t\t\t", hsm.r9300.pre_data.ucast_hkey);
        osal_printf("rx_port_in_vlan: %d\t\t", hsm.r9300.pre_data.rx_port_in_vlan);
        osal_printf("igr_stp_status: %d\n", hsm.r9300.pre_data.igr_stp_status);
        osal_printf("oam_act: %d\t\t\t", hsm.r9300.pre_data.oam_act);
        osal_printf("oam_dis_lrn: %d\t\t\t", hsm.r9300.pre_data.oam_dis_lrn);
        osal_printf("iacl_byp_act: 0x%x\n", hsm.r9300.pre_data.iacl_byp_act);
        osal_printf("iacl_metadata: 0x%2x\t\t", hsm.r9300.pre_data.iacl_metadata);
        osal_printf("rma_byp_vlan_fltr: %d\t\t", hsm.r9300.pre_data.rma_byp_vlan_fltr);
        osal_printf("err_pkt: %d\n", hsm.r9300.pre_data.err_pkt);
        osal_printf("rma_eapol_pkt: %d\t\t", hsm.r9300.pre_data.rma_eapol_pkt);

        // HSM0_DATA13
        osal_printf("rma_lldp_pkt: %d\t\t\t", hsm.r9300.pre_data.rma_lldp_pkt);
        osal_printf("rma_usr_pkt: %d\n", hsm.r9300.pre_data.rma_usr_pkt);
        osal_printf("rma_rsn: 0x%2x\t\t\t", hsm.r9300.pre_data.rma_rsn);

        osal_printf("dhcp_if: %d\t\t\t", hsm.r9300.pre_data.dhcp_if);
        osal_printf("rip_if: %d\n", hsm.r9300.pre_data.rip_if);
        osal_printf("ip_rsv_act: %d\t\t\t", hsm.r9300.pre_data.ip_rsv_act);
        osal_printf("macMisAct: %d\t\t\t", hsm.r9300.pre_data.macMis_act);

        // HSM0_DATA14
        osal_printf("igr_vlan_mbr: 0x%8x\n", hsm.r9300.pre_data.igr_vlan_mbr);
        osal_printf("ipv4_pkt: %d\t\t\t", hsm.r9300.pre_data.ipv4_pkt);
        osal_printf("ipv6_pkt: %d\t\t\t", hsm.r9300.pre_data.ipv6_pkt);

        // HSM0_DATA15 ~ 17
        osal_printf("route_act: 0x");
        if((hsm.r9300.pre_data.route_act >> 32) != 0)
            osal_printf("%6x", (uint32)(hsm.r9300.pre_data.route_act >> 32));
        osal_printf("%08x\n", (uint32)(hsm.r9300.pre_data.route_act & 0xffffffff));

        osal_printf("lut_haddr0: %d\t\t\t", hsm.r9300.pre_data.lut_haddr0);
        osal_printf("lut_haddr1: %d\t\t\t", hsm.r9300.pre_data.lut_haddr1);
        osal_printf("int_pri_route: %d\n", hsm.r9300.pre_data.int_pri_route);
        osal_printf("st_rt_if: %d\t\t\t", hsm.r9300.pre_data.st_rt_if);

        osal_printf("acl_act: 0x");
        if((hsm.r9300.pre_data.acl_act >> 32) != 0)
            osal_printf("%5x", (uint32)(hsm.r9300.pre_data.acl_act >> 32));
        osal_printf("%08x\n", (uint32)(hsm.r9300.pre_data.acl_act & 0xffffffff));
    } else if (index == 1) {
        // HSM1_DATA0
        osal_printf("trk_id: %2d\t\t\t", hsm.r9300.lu_data.trk_id);
        osal_printf("sphy: %2d\t\t\t", hsm.r9300.lu_data.sphy);
        osal_printf("byp_igr_bwctrl: %d\n", hsm.r9300.lu_data.byp_igr_bwctrl);
        osal_printf("proto_storm_drop: %d\t\t", hsm.r9300.lu_data.proto_storm_drop);
        osal_printf("dp: %d\t\t\t\t", hsm.r9300.lu_data.dp);
        osal_printf("int_pri: %d\n", hsm.r9300.lu_data.int_pri);
        osal_printf("int_pri_route: %d\t\t", hsm.r9300.lu_data.int_pri_route);

        // HSM1_DATA1
        if (!hsm.r9300.pre_data.ivc_mode) {
            osal_printf("c2scact_ipri: %d\t\t\t", hsm.r9300.pre_data.c2scact_ipri);
            osal_printf("c2scact_aipri: %d\n", hsm.r9300.pre_data.c2scact_aipri);
            osal_printf("c2scact_itag: %d\t\t\t", hsm.r9300.pre_data.c2scact_itag);
            osal_printf("c2scact_aitag: %d\t\t", hsm.r9300.pre_data.c2scact_aitag);
            osal_printf("c2scact_itpid: %d\n", hsm.r9300.pre_data.c2scact_itpid);
            osal_printf("c2scact_aitpid: %d\t\t", hsm.r9300.pre_data.c2scact_aitpid);
            osal_printf("c2scact_opri: %d\t\t\t", hsm.r9300.pre_data.c2scact_opri);
            osal_printf("c2scact_aopri: %d\n", hsm.r9300.pre_data.c2scact_aopri);
            osal_printf("c2scact_otag: %d\t\t\t", hsm.r9300.pre_data.c2scact_otag);
            osal_printf("c2scact_aotag: %d\t\t", hsm.r9300.pre_data.c2scact_aotag);
            osal_printf("c2scact_otpid: %d\n", hsm.r9300.pre_data.c2scact_otpid);
            osal_printf("c2scact_aotpid: %d\t\t", hsm.r9300.pre_data.c2scact_aotpid);
        } else {
            osal_printf("macipact_pri: %d\t\t\t", hsm.r9300.pre_data.macipact_pri);
            osal_printf("macipact_apri: %d\n", hsm.r9300.pre_data.macipact_apri);
            osal_printf("macipact_tag: %d\t\t\t", hsm.r9300.pre_data.macipact_tag);
            osal_printf("macipact_atag: %d\t\t\t", hsm.r9300.pre_data.macipact_atag);
            osal_printf("macipact_tpid: %d\n", hsm.r9300.pre_data.macipact_tpid);
            osal_printf("macipact_atpid: %d\t\t\t", hsm.r9300.pre_data.macipact_atpid);
            osal_printf("macipact_type: %d\t\t\t", hsm.r9300.pre_data.macipact_type);
            osal_printf("macipact_ignore: %d\n", hsm.r9300.pre_data.macipact_ignore);
            osal_printf("macipact_fwdact: %d\t\t\t", hsm.r9300.pre_data.macipact_fwdact);
        }
        osal_printf("ivc_hit: %d\t\t\t", hsm.r9300.lu_data.ivc_hit);
        osal_printf("ivc_mode: %d\n", hsm.r9300.lu_data.ivc_mode);
        osal_printf("rma_byp_stp: %d\t\t\t", hsm.r9300.lu_data.rma_byp_stp);
        osal_printf("rma_act: %d\t\t\t", hsm.r9300.lu_data.rma_act);
        osal_printf("rma_pkt: %d\n", hsm.r9300.lu_data.rma_pkt);
        osal_printf("atk_hit: %d\t\t\t", hsm.r9300.lu_data.atk_hit);
        osal_printf("atk_act: %d\t\t\t", hsm.r9300.lu_data.atk_act);

        // HSM1_DATA2
        osal_printf("atk_type: %2d\n", hsm.r9300.lu_data.atk_type);
        osal_printf("eav_class_b: %d\t\t\t", hsm.r9300.lu_data.eav_class_b);
        osal_printf("eav_class_a: %d\t\t\t", hsm.r9300.lu_data.eav_class_a);
        osal_printf("capwap_trap: %d\n", hsm.r9300.lu_data.capwap_trap);

        // HSM1_DATA4
        osal_printf("acl_idx: 0x%3x\t\t\t", hsm.r9300.lu_data.acl_idx);
        osal_printf("acl_hit: 0x%2x\t\t\t", hsm.r9300.lu_data.acl_hit);
        osal_printf("ivc_drop: %d\n", hsm.r9300.lu_data.ivc_drop);
        osal_printf("fwd_base: %d\t\t\t", hsm.r9300.lu_data.fwd_base);

        // HSM1_DATA5
        osal_printf("fwd_vid: 0x%3x\t\t\t", hsm.r9300.lu_data.fwd_vid);
        osal_printf("igr_ovid: 0x%3x\n", hsm.r9300.lu_data.igr_ovid);

        // HSM1_DATA6
        osal_printf("igr_ivid: 0x%3x\t\t\t", hsm.r9300.lu_data.igr_ivid);
        osal_printf("vlan_aft_drop: %d\t\t", hsm.r9300.lu_data.vlan_aft_drop);
        osal_printf("vlan_cfi_act: %d\n", hsm.r9300.lu_data.vlan_cfi_act);
        osal_printf("iacl_meter_drop: %d\t\t", hsm.r9300.lu_data.iacl_meter_drop);
        osal_printf("vlan_group_mask: 0x%2x\t\t", hsm.r9300.lu_data.vlan_group_mask);
        osal_printf("msti: 0x%2x\n", hsm.r9300.lu_data.msti);

        // HSM1_DATA7
        osal_printf("rx_port_in_vlan: %d\t\t", hsm.r9300.lu_data.rx_port_in_vlan);
        osal_printf("igr_stp_status: %d\t\t", hsm.r9300.lu_data.igr_stp_status);
        osal_printf("oam_act: %d\n", hsm.r9300.lu_data.oam_act);
        osal_printf("l2_sa_trunk: %d\t\t\t", hsm.r9300.lu_data.l2_sa_trunk);
        osal_printf("l2_sa_slp: %d\t\t\t", hsm.r9300.lu_data.l2_sa_slp);
        osal_printf("l2_sa_age: %d\n", hsm.r9300.lu_data.l2_sa_age);
        osal_printf("l2_sa_sab: %d\t\t\t", hsm.r9300.lu_data.l2_sa_sab);
        osal_printf("l2_sa_dab: %d\t\t\t", hsm.r9300.lu_data.l2_sa_dab);
        osal_printf("l2_sa_st: %d\n", hsm.r9300.lu_data.l2_sa_st);
        osal_printf("l2_sa_suspend: %d\t\t", hsm.r9300.lu_data.l2_sa_suspend);
        osal_printf("l2_sa_hit: %d\t\t\t", hsm.r9300.lu_data.l2_sa_hit);

        // HSM1_DATA8
        osal_printf("l2_da_trunk: %d\n", hsm.r9300.lu_data.l2_da_trunk);
        osal_printf("l2_da_slp: %d\t\t\t", hsm.r9300.lu_data.l2_da_slp);
        osal_printf("l2_da_age: %d\t\t\t", hsm.r9300.lu_data.l2_da_age);
        osal_printf("l2_da_sab: %d\n", hsm.r9300.lu_data.l2_da_sab);
        osal_printf("l2_da_dab: %d\t\t\t", hsm.r9300.lu_data.l2_da_dab);
        osal_printf("l2_da_st: %d\t\t\t", hsm.r9300.lu_data.l2_da_st);
        osal_printf("l2_da_suspend: %d\n", hsm.r9300.lu_data.l2_da_suspend);
        osal_printf("l2_da_nh: %d\t\t\t", hsm.r9300.lu_data.l2_da_nh);
        osal_printf("l2_da_vid: %d\t\t\t", hsm.r9300.lu_data.l2_da_vid);
        osal_printf("l2_da_hit: %d\n", hsm.r9300.lu_data.l2_da_hit);

        // HSM1_DATA9
        osal_printf("l3_da_idx: 0x%3x\t\t", hsm.r9300.lu_data.l3_da_idx);
        osal_printf("mac_limit_act: %d\t\t", hsm.r9300.lu_data.mac_limit_act);
        osal_printf("mac_limit_vlan_act: %d\n", hsm.r9300.lu_data.mac_limit_vlan_act);

        // HSM1_DATA10
        osal_printf("sttc_pm_act: %d\t\t\t", hsm.r9300.lu_data.sttc_pm_act);
        osal_printf("new_sa_act: %d\t\t\t", hsm.r9300.lu_data.new_sa_act);
        osal_printf("da_type: %d\n", hsm.r9300.lu_data.da_type);
        osal_printf("hash_type: %d\t\t\t", hsm.r9300.lu_data.hash_type);

        // HSM1_DATA11
        osal_printf("dpm: 0x%8x\t\t\t", hsm.r9300.lu_data.dpm);
        osal_printf("igr_vlan_fltr_act: %d\n", hsm.r9300.lu_data.igr_vlan_fltr_act);
        osal_printf("is_mcast_sa: %d\t\t\t", hsm.r9300.lu_data.is_mcast_sa);

        // HSM1_DATA12
        osal_printf("src_port_fltr_en: %d\t\t", hsm.r9300.lu_data.src_port_fltr_en);
        osal_printf("dyn_pm_act: %d\n", hsm.r9300.lu_data.dyn_pm_act);
        osal_printf("forbid_pm_act: %d\t\t", hsm.r9300.lu_data.forbid_pm_act);
        osal_printf("org_spa: %2d\t\t\t", hsm.r9300.lu_data.org_spa);
        osal_printf("ctag_flag: %d\n", hsm.r9300.lu_data.ctag_flag);
        osal_printf("iacl_byp_act: %d\t\t\t", hsm.r9300.lu_data.iacl_byp_act);
        osal_printf("rma_byp_vlan_fltr: %d\t\t", hsm.r9300.lu_data.rma_byp_vlan_fltr);
        osal_printf("rma_byp_new_sa_drop: %d\n", hsm.r9300.lu_data.rma_byp_new_sa_drop);
        osal_printf("nexthop_ageout: %d\t\t", hsm.r9300.lu_data.nexthop_ageout);
        osal_printf("nexthop_if: %d\t\t\t", hsm.r9300.lu_data.nexthop_if);

        // HSM1_DATA13
        osal_printf("iacl_metadata: 0x%2x\n", hsm.r9300.lu_data.iacl_metadata);
        osal_printf("l2_hash_full: %d\t\t\t", hsm.r9300.lu_data.l2_hash_full);
        osal_printf("rma_rsn: 0x%2x\t\t\t", hsm.r9300.lu_data.rma_rsn);
        osal_printf("dhcp_if: %d\n", hsm.r9300.lu_data.dhcp_if);
        osal_printf("rip_if: %d\t\t\t", hsm.r9300.lu_data.rip_if);
        osal_printf("ip_rsv_act: %d\t\t\t", hsm.r9300.lu_data.ip_rsv_act);
        osal_printf("sptrsn: %2d\n", hsm.r9300.lu_data.sptrsn);

        // HSM1_DATA15
        osal_printf("port_mv: %d\t\t\t", hsm.r9300.lu_data.port_mv);
        osal_printf("macMisAct: %d\t\t\t", hsm.r9300.lu_data.macMis_act);

        // HSM1_DATA16
        osal_printf("fwd_type: %d\n", hsm.r9300.lu_data.fwd_type);
        osal_printf("dp_info: %2d\t\t\t", hsm.r9300.lu_data.dp_info);
        osal_printf("dp_fmt: %d\t\t\t", hsm.r9300.lu_data.dp_fmt);
        osal_printf("st_sa_lrn: %d\nt", hsm.r9300.lu_data.sa_lrn);
        osal_printf("trk_uc: %d\t\t\t", hsm.r9300.lu_data.trk_uc);
        osal_printf("trk_hit_idx: %d\nt", hsm.r9300.lu_data.trk_hit_idx);

        osal_printf("route_act: 0x");
        if((hsm.r9300.lu_data.route_act >> 32) != 0)
            osal_printf("%6x", (uint32)(hsm.r9300.lu_data.route_act >> 32));
        osal_printf("%08x\n", (uint32)(hsm.r9300.lu_data.route_act & 0xffffffff));

        osal_printf("acl_act: 0x");
        if((hsm.r9300.lu_data.acl_act >> 32) != 0)
            osal_printf("%5x", (uint32)(hsm.r9300.lu_data.acl_act >> 32));
        osal_printf("%08x\n", (uint32)(hsm.r9300.lu_data.acl_act & 0xffffffff));

    } else if (index == 2) {
        // HSM2_DATA0
        osal_printf("oam_pdu: %d\t\t\t", hsm.r9300.post0_data.oam_pdu);
        osal_printf("ipv4_igmp: %d\t\t\t", hsm.r9300.post0_data.ipv4_igmp);
        osal_printf("ipv6_mld: %d\n", hsm.r9300.post0_data.ipv6_mld);
        osal_printf("hol_prvnt: %d\t\t\t", hsm.r9300.post0_data.hol_prvnt);
        osal_printf("ipv4_chksum_ok: %d\t\t", hsm.r9300.post0_data.ipv4_chksum_ok);
        osal_printf("err_pkt: %d\n", hsm.r9300.post0_data.err_pkt);
        osal_printf("ip_ttl: 0x%2x\t\t\t", hsm.r9300.post0_data.ip_ttl);
        osal_printf("ip_length: 0x%4x\t\t", hsm.r9300.post0_data.ip_length);

        // HSM2_DATA1
        osal_printf("pkt_len: 0x%4x\n", hsm.r9300.post0_data.pkt_len);
        osal_printf("llc_other: %d\t\t\t", hsm.r9300.post0_data.llc_other);
        osal_printf("pppoe_pkt: %d\t\t\t", hsm.r9300.post0_data.pppoe_pkt);
        osal_printf("rfc1042: %d\n", hsm.r9300.post0_data.rfc1042);

        // HSM2_DATA2
        osal_printf("org_otpid_idx: %2d\t\t", hsm.r9300.post0_data.org_otpid_idx);
        osal_printf("org_itpid_idx: %2d\t\t", hsm.r9300.post0_data.org_itpid_idx);
        osal_printf("sphy: %2d\n", hsm.r9300.post0_data.sphy);
        osal_printf("igr_err: %d\t\t\t", hsm.r9300.post0_data.igr_err);
        osal_printf("rtag_type: %d\t\t\t", hsm.r9300.post0_data.rtag_type);
        osal_printf("rtag_exist: %d\n", hsm.r9300.post0_data.rtag_exist);
        osal_printf("etag_exist: %d\t\t\t", hsm.r9300.post0_data.etag_exist);
        osal_printf("ctag_exist: %d\t\t\t", hsm.r9300.post0_data.ctag_exist);
        osal_printf("itag_exist: %d\n", hsm.r9300.post0_data.itag_exist);
        osal_printf("otag_exist: %d\t\t\t", hsm.r9300.post0_data.otag_exist);

        // HSM2_DATA3
        osal_printf("ivid: 0x%3x\t\t\t", hsm.r9300.post0_data.ivid);
        osal_printf("icfi: %d\n", hsm.r9300.post0_data.icfi);
        osal_printf("ipri: %d\t\t\t\t", hsm.r9300.post0_data.ipri);
        osal_printf("acl_rmk_val: 0x%2x\n", hsm.r9300.post0_data.acl_rmk_val); // for type setting
        osal_printf("ovid: 0x%3x\t\t\t", hsm.r9300.post0_data.ovid);
        osal_printf("ocfi: %d\t\t\t\t", hsm.r9300.post0_data.ocfi);
        osal_printf("opri: %d\n", hsm.r9300.post0_data.opri);

        // HSM2_DATA4
        osal_printf("ipv6_pkt: %d\t\t\t", hsm.r9300.post0_data.ipv6_pkt);
        osal_printf("ipv4_pkt: %d\t\t\t", hsm.r9300.post0_data.ipv4_pkt);
        osal_printf("is_mcast_sa: %d\n", hsm.r9300.post0_data.is_mcast_sa);
        osal_printf("sa_zero: %d\t\t\t", hsm.r9300.post0_data.sa_zero);
        osal_printf("da_mcast: %d\t\t\t", hsm.r9300.post0_data.da_mcast);
        osal_printf("da_bcast: %d\n", hsm.r9300.post0_data.da_bcast);
        osal_printf("da_bcast: %d\t\t\t", hsm.r9300.post0_data.da_bcast);
        osal_printf("eth: %d\t\t\t\t", hsm.r9300.post0_data.eth);
        osal_printf("rldp_rlpp_pkt: %d\n", hsm.r9300.post0_data.rldp_rlpp_pkt);
        osal_printf("arp_pkt: %d\t\t\t", hsm.r9300.post0_data.arp_pkt);
        osal_printf("rma_eapol_pkt: %d\t\t", hsm.r9300.post0_data.rma_eapol_pkt);

        // HSM2_DATA5
        osal_printf("lst_dsc: 0x%3x\n", hsm.r9300.post0_data.lst_dsc);
        osal_printf("fst_dsc: 0x%3x\t\t\t", hsm.r9300.post0_data.fst_dsc);
        osal_printf("page_cnt: 0x%2x\t\t\t", hsm.r9300.post0_data.page_cnt);

        // HSM2_DATA6
        osal_printf("rma_rsn: %2d\n", hsm.r9300.post0_data.rma_rsn);
        osal_printf("rma_byp_stp: %d\t\t\t", hsm.r9300.post0_data.rma_byp_stp);
        osal_printf("rma_byp_vlan_fltr: %d\t\t", hsm.r9300.post0_data.rma_byp_vlan_fltr);
        osal_printf("rma_act: %d\n", hsm.r9300.post0_data.rma_act);
        osal_printf("rma_pkt: %d\t\t\t", hsm.r9300.post0_data.rma_pkt);
        osal_printf("eav_class_b: %d\t\t\t", hsm.r9300.post0_data.eav_class_b);
        osal_printf("eav_class_a: %d\n", hsm.r9300.post0_data.eav_class_a);
        osal_printf("proto_storm_drop: %d\t\t", hsm.r9300.post0_data.proto_storm_drop);
        osal_printf("capwap_trap: %d\t\t\t", hsm.r9300.post0_data.capwap_trap);
        osal_printf("vlan_aft_drop: %d\n", hsm.r9300.post0_data.vlan_aft_drop);
        osal_printf("vlan_cfi_act: %d\t\t\t", hsm.r9300.post0_data.vlan_cfi_act);
        osal_printf("igr_stp_status: %d\t\t", hsm.r9300.post0_data.igr_stp_status);
        osal_printf("l2_sa_sab: %d\n", hsm.r9300.post0_data.l2_sa_sab);
        osal_printf("l2_sa_hit: %d\t\t\t", hsm.r9300.post0_data.l2_sa_hit);
        osal_printf("da_type: %d\t\t\t", hsm.r9300.post0_data.da_type);

        // HSM2_DATA7
        osal_printf("dpm: 0x%8x\n", hsm.r9300.post0_data.dpm);
        osal_printf("rx_port_in_vlan: %d\t\t", hsm.r9300.post0_data.rx_port_in_vlan);
        osal_printf("vlan_src_port_fltr_act: %d\t", hsm.r9300.post0_data.vlan_src_port_fltr_act);

        // HSM2_DATA8
        osal_printf("iacl_byp_act: %d\n", hsm.r9300.post0_data.iacl_byp_act);
        osal_printf("rma_byp_new_sa_drop: %d\t\t", hsm.r9300.post0_data.rma_byp_new_sa_drop);
        osal_printf("trk_id: 0x%2x\t\t\t", hsm.r9300.post0_data.trk_id);
        osal_printf("src_port_fltr_en: %d\n", hsm.r9300.post0_data.src_port_fltr_en);
        osal_printf("hash_idx: 0x%2x\t\t\t", hsm.r9300.post0_data.hash_idx);
        osal_printf("hash_uc: %d\t\t\t", hsm.r9300.post0_data.hash_uc);
        osal_printf("unkn_trk_hash_1: 0x%2x\n", hsm.r9300.post0_data.unkn_trk_hash_1);
        osal_printf("unkn_trk_hash_0: 0x%2x\t\t", hsm.r9300.post0_data.unkn_trk_hash_0);
        osal_printf("trk_hash_1: 0x%2x\t\t", hsm.r9300.post0_data.trk_hash_1);

        // HSM2_DATA9
        osal_printf("trk_hash_0: 0x%2x\n", hsm.r9300.post0_data.trk_hash_0);
        osal_printf("egr_otpid_idx: %d\t\t", hsm.r9300.post0_data.egr_otpid_idx);
        osal_printf("egr_itpid_idx: %d\t\t", hsm.r9300.post0_data.egr_itpid_idx);
        osal_printf("int_ocfi: %d\n", hsm.r9300.post0_data.int_ocfi);
        osal_printf("int_icfi: %d\t\t\t", hsm.r9300.post0_data.int_icfi);
        osal_printf("egr_opri: %d\t\t\t", hsm.r9300.post0_data.egr_opri);
        osal_printf("egr_ipri: %d\n", hsm.r9300.post0_data.egr_ipri);
        osal_printf("egr_otag_status: %d\t\t", hsm.r9300.post0_data.egr_otag_status);
        osal_printf("egr_itag_status: %d\t\t", hsm.r9300.post0_data.egr_itag_status);
        osal_printf("ivc_otpid_as: %d\n", hsm.r9300.post0_data.ivc_otpid_as);
        osal_printf("ivc_itpid_as: %d\t\t\t", hsm.r9300.post0_data.ivc_itpid_as);
        osal_printf("ivc_iacl_opri_as: %d\t\t", hsm.r9300.post0_data.ivc_iacl_opri_as);
        osal_printf("ivc_iacl_ipri_as: %d\n", hsm.r9300.post0_data.ivc_iacl_ipri_as);
        osal_printf("ivc_iacl_otag_status_as: %d\t", hsm.r9300.post0_data.ivc_iacl_otag_status_as);
        osal_printf("ivc_iacl_itag_status_as: %d\t", hsm.r9300.post0_data.ivc_iacl_itag_status_as);
        osal_printf("acl_redi: %d\n", hsm.r9300.post0_data.acl_redi);
        osal_printf("acl_cpu_fmt: %d\t\t\t", hsm.r9300.post0_data.acl_cpu_fmt);

        // HSM2_DATA10
        osal_printf("eacl_qid: %2d\t\t\t", hsm.r9300.post0_data.eacl_qid);
        osal_printf("eacl_pri_hit: %d\n", hsm.r9300.post0_data.eacl_pri_hit);
        osal_printf("eacl_mir_hit: %d\t\t\t", hsm.r9300.post0_data.eacl_mir_hit);
        osal_printf("eacl_mir_act: %d\t\t\t", hsm.r9300.post0_data.eacl_mir_act);
        osal_printf("eacl_otpid_hit: %d\n", hsm.r9300.post0_data.eacl_otpid_hit);
        osal_printf("eacl_itpid_hit: %d\t\t", hsm.r9300.post0_data.eacl_itpid_hit);
        osal_printf("eacl_opri_hit: %d\t\t", hsm.r9300.post0_data.eacl_opri_hit);
        osal_printf("eacl_ipri_hit: %d\n", hsm.r9300.post0_data.eacl_ipri_hit);
        osal_printf("eacl_otag_hit: %d\t\t", hsm.r9300.post0_data.eacl_otag_hit);
        osal_printf("eacl_itag_hit: %d\t\t", hsm.r9300.post0_data.eacl_itag_hit);
        osal_printf("n_1_ovid_hit: %d\n", hsm.r9300.post0_data.n_1_ovid_hit);
        osal_printf("eacl_ovid_hit: %d\t\t", hsm.r9300.post0_data.eacl_ovid_hit);
        osal_printf("igr_ovid: 0x%3x\t\t\t", hsm.r9300.post0_data.igr_ovid);

        // HSM2_DATA11
        osal_printf("n_1_ivid_hit: %d\n", hsm.r9300.post0_data.n_1_ivid_hit);
        osal_printf("eacl_ivid_hit: %d\t\t", hsm.r9300.post0_data.eacl_ivid_hit);
        osal_printf("igr_ivid: 0x%3x\t\t\t", hsm.r9300.post0_data.igr_ivid);
        osal_printf("eacl_rmk_hit: %d\n", hsm.r9300.post0_data.eacl_rmk_hit);

        // HSM2_DATA12
        osal_printf("iacl_mir_act: %d\t\t\t", hsm.r9300.post0_data.iacl_mir_act);
        osal_printf("iacl_mir_hit: %d\t\t\t", hsm.r9300.post0_data.iacl_mir_hit);
        osal_printf("acl_idx: 0x%3x\n", hsm.r9300.post0_data.acl_idx);
        osal_printf("acl_hit: %d\t\t\t", hsm.r9300.post0_data.acl_hit);
        osal_printf("iacl_qid: %2d\t\t\t", hsm.r9300.post0_data.iacl_qid);
        osal_printf("int_pri_route: %d\n", hsm.r9300.post0_data.int_pri_route);
        osal_printf("int_pri: %d\t\t\t", hsm.r9300.post0_data.int_pri);
        osal_printf("byp_igr_bwctrl: %d\t\t", hsm.r9300.post0_data.byp_igr_bwctrl);

        // HSM2_DATA13
        osal_printf("atk_type: %2d\n", hsm.r9300.post0_data.atk_type);
        osal_printf("atk_act: %d\t\t\t", hsm.r9300.post0_data.atk_act);
        osal_printf("oam_act: %d\t\t\t", hsm.r9300.post0_data.oam_act);
        osal_printf("mcrt_oil_idx: 0x%3x\n", hsm.r9300.post0_data.mcrt_oil_idx);
        osal_printf("mcrt_rpf_asst: %d\t\t", hsm.r9300.post0_data.mcrt_rpf_asst);
        osal_printf("org_spa: %2d\t\t\t", hsm.r9300.post0_data.org_spa);


        // HSM2_DATA14
        osal_printf("l2_da_block: %d\n", hsm.r9300.post0_data.l2_da_block);
        osal_printf("l2_da_hit: %d\t\t\t", hsm.r9300.post0_data.l2_da_hit);
        osal_printf("fwd_vid: 0x%3x\t\t\t", hsm.r9300.post0_data.fwd_vid);
        osal_printf("orig_fwd_vid: 0x%3x\n", hsm.r9300.post0_data.orig_fwd_vid);
        osal_printf("fwd_base: %d\t\t\t", hsm.r9300.post0_data.fwd_base);

        // HSM2_DATA15
        osal_printf("l3_da_idx: 0x%3x\t\t", hsm.r9300.post0_data.l3_da_idx);
        osal_printf("hash_type: %d\n", hsm.r9300.post0_data.hash_type);
        osal_printf("dp: %d\t\t\t\t", hsm.r9300.post0_data.dp);
        osal_printf("trk_mc_hit: %d\t\t\t", hsm.r9300.post0_data.trk_mc_hit);
        osal_printf("storm_lkmis: %d\n", hsm.r9300.post0_data.storm_lkmis);
        osal_printf("nexthop_ageout: %d\t\t", hsm.r9300.post0_data.nexthop_ageout);
        osal_printf("nexthop_if: %d\t\t\t", hsm.r9300.post0_data.nexthop_if);
        osal_printf("iacl_meter_drop: %d\n", hsm.r9300.post0_data.iacl_meter_drop);
        osal_printf("eacl_meter_drop: %d\t\t", hsm.r9300.post0_data.eacl_meter_drop);

        // HSM2_DATA16
        osal_printf("ivc_byp_igr_vlan_fltr: %d\t", hsm.r9300.post0_data.ivc_byp_igr_vlan_fltr);
        osal_printf("ivc_copy: %d\n", hsm.r9300.post0_data.ivc_copy);
        osal_printf("ivc_trap_master: %d\t\t", hsm.r9300.post0_data.ivc_trap_master);
        osal_printf("ivc_trap: %d\t\t\t", hsm.r9300.post0_data.ivc_trap);
        osal_printf("ivc_drop: %d\n", hsm.r9300.post0_data.ivc_drop);
        osal_printf("eacl_copy: %d\t\t\t", hsm.r9300.post0_data.eacl_copy);
        osal_printf("eacl_trap_master: %d\t\t", hsm.r9300.post0_data.eacl_trap_master);
        osal_printf("eacl_trap: %d\n", hsm.r9300.post0_data.eacl_trap);
        osal_printf("eacl_redir_zero: %d\t\t", hsm.r9300.post0_data.eacl_redir_zero);
        osal_printf("eacl_drop_act: %d\t\t", hsm.r9300.post0_data.eacl_drop_act);
        osal_printf("eacl_drop: %d\n", hsm.r9300.post0_data.eacl_drop);
        osal_printf("iacl_copy: %d\t\t\t", hsm.r9300.post0_data.iacl_copy);
        osal_printf("iacl_trap_master: %d\t\t", hsm.r9300.post0_data.iacl_trap_master);
        osal_printf("iacl_trap: %d\n", hsm.r9300.post0_data.iacl_trap);
        osal_printf("iacl_redir_zero: %d\t\t", hsm.r9300.post0_data.iacl_redir_zero);
        osal_printf("iacl_drop: %d\t\t\t", hsm.r9300.post0_data.iacl_drop);
        osal_printf("iacl_drop_act: %d\n", hsm.r9300.post0_data.iacl_drop_act);
        osal_printf("eacl_disable: %d\t\t\t", hsm.r9300.post0_data.eacl_disable);
        osal_printf("garp_act: %d\t\t\t", hsm.r9300.post0_data.garp_act);
        osal_printf("dhcp_if: %d\n", hsm.r9300.post0_data.dhcp_if);
        osal_printf("rip_if: %d\t\t\t", hsm.r9300.post0_data.rip_if);

        // HSM2_DATA17
        osal_printf("sa_lrn: %d\t\t\t",hsm.r9300.post0_data.sa_lrn);
        osal_printf("st_da_hit: %d\n",hsm.r9300.post0_data.st_da_hit);
        osal_printf("trk_hash: %d\t\t\t",hsm.r9300.post0_data.trk_hash);
        osal_printf("mir_id: %d\t\t\t",hsm.r9300.post0_data.mir_id);
        osal_printf("st_ctag_if: %d\n",hsm.r9300.post0_data.st_ctag_if);
        osal_printf("fwd_type: %d\t\t\t",hsm.r9300.post0_data.fwd_type);
        osal_printf("dp_info: %d\t\t\t",hsm.r9300.post0_data.dp_info);
        osal_printf("dp_fmt: %d\n",hsm.r9300.post0_data.dp_fmt);

        // HSM2_DATA18
        osal_printf("sp_fltr_en: %d\t\t\t",hsm.r9300.post0_data.sp_fltr_en);
        osal_printf("sp_info: %d\t\t\t",hsm.r9300.post0_data.sp_info);
        osal_printf("stk_if: %d\n",hsm.r9300.post0_data.stk_if);
        osal_printf("lb_pkt: %d\t\t\t",hsm.r9300.post0_data.lb_pkt);
        osal_printf("lb_ttl: %d\t\t\t",hsm.r9300.post0_data.lb_ttl);
        osal_printf("acl_lb_act: %d\n",hsm.r9300.post0_data.acl_lb_act);
        osal_printf("metadata: %d\t\t\t",hsm.r9300.post0_data.metadata);
        osal_printf("ctag_flag: %d\t\t\t",hsm.r9300.post0_data.ctag_flag);
        osal_printf("vb_iso_mbr: %d\n",hsm.r9300.post0_data.vb_iso_mbr);

        // HSM2_DATA19
        osal_printf("ip_rsvd_inv: %d\t\t\t",hsm.r9300.post0_data.ip_rsvd_inv);
        osal_printf("l2_hash_full: %d\t\t\t",hsm.r9300.post0_data.l2_hash_full);
        osal_printf("force_pmove: %d\n",hsm.r9300.post0_data.force_pmove);
        osal_printf("dyn_pm_act: %d\t\t\t",hsm.r9300.post0_data.dyn_pm_act);
        osal_printf("sttc_pm_act: %d\t\t\t",hsm.r9300.post0_data.sttc_pm_act);
        osal_printf("mac_limit_act: %d\n",hsm.r9300.post0_data.mac_limit_act);
        osal_printf("mac_limit_vlan_act: %d\t\t",hsm.r9300.post0_data.mac_limit_vlan_act);
        osal_printf("new_sa_act: %d\t\t\t",hsm.r9300.post0_data.new_sa_act);
        osal_printf("sptrsn: %d\n",hsm.r9300.post0_data.sptrsn);

        // HSM2_DATA20
        osal_printf("ip_rsv_act: %d\t\t\t",hsm.r9300.post0_data.ip_rsv_act);
        osal_printf("macMiss_act: %d\t\t\t",hsm.r9300.post0_data.macMiss_act);

        // HSM2_DATA21-22
        osal_printf("ctx_unit: %d\n",hsm.r9300.post0_data.ctx_unit);
        osal_printf("ctx_dpm: %8x-%8x\t",(uint32)(hsm.r9300.post0_data.ctx_dpm & 0xffffffff),(uint32)(hsm.r9300.post0_data.ctx_dpm >> 32) & 0xffffff);
        osal_printf("ctx_dpm_type: %d\n",hsm.r9300.post0_data.ctx_dpm_type);

        // HSM2_DATA23
        osal_printf("ctx_sp_fltr: %d\t\t\t",hsm.r9300.post0_data.ctx_sp_fltr);
        osal_printf("ctx_fwd_vid_en: %d\t\t",hsm.r9300.post0_data.ctx_fwd_vid_en);
        osal_printf("ctx_as_qid: %d\n",hsm.r9300.post0_data.ctx_as_qid);
        osal_printf("ctx_qid: %d\t\t\t",hsm.r9300.post0_data.ctx_qid);
        osal_printf("ctx_org_tag: %d\t\t\t",hsm.r9300.post0_data.ctx_org_tag);
        osal_printf("ctx_l3_act: %d\n",hsm.r9300.post0_data.ctx_l3_act);
        osal_printf("ctx_as_tag: %d\t\t\t",hsm.r9300.post0_data.ctx_as_tag);
        osal_printf("ctx_bp_egr_vlan: %d\t\t",hsm.r9300.post0_data.ctx_bp_egr_vlan);
        osal_printf("ctx_bp_stp: %d\n",hsm.r9300.post0_data.ctx_bp_stp);
        osal_printf("ctx_bp_fltr: %d\t\t\t",hsm.r9300.post0_data.ctx_bp_fltr);
        osal_printf("ctx_dg_gasp: %d\t\t\t",hsm.r9300.post0_data.ctx_dg_gasp);
        osal_printf("ctx_cngst_drop: %d\n",hsm.r9300.post0_data.ctx_cngst_drop);
        osal_printf("ctx_acl_act: %d\t\t\t",hsm.r9300.post0_data.ctx_acl_act);

        // HSM2_DATA24 -26
        osal_printf("crx_data[0]: %d\t\t\t",hsm.r9300.post0_data.crx_data[0]);
        osal_printf("crx_data[1]: %d\n",hsm.r9300.post0_data.crx_data[1]);
        osal_printf("crx_data[2]: %d\t\t\t",hsm.r9300.post0_data.crx_data[2] & 0xffff);

        // HSM2_DATA27
        osal_printf("rt_event: 0x%3x\t\t\t", hsm.r9300.post0_data.rt_event);
        osal_printf("l3_sa_idx: %2d\n", hsm.r9300.post0_data.l3_sa_idx);
        osal_printf("l3_sa_rep: %d\t\t\t", hsm.r9300.post0_data.l3_sa_rep);
        osal_printf("l3_da_rep: %d\t\t\t", hsm.r9300.post0_data.l3_da_rep);
        osal_printf("l3_ttl_dec: %d\n", hsm.r9300.post0_data.l3_ttl_dec);
        osal_printf("ip_route_en: %d\n", hsm.r9300.post0_data.ip_route_en);

        osal_printf("l3_uc_kn: %d\t\t\t", hsm.r9300.post0_data.l3_uc_kn);
        osal_printf("st_pmsk: %d\t\t\t", hsm.r9300.post0_data.st_pmsk);
        osal_printf("acl_rediCopy: %d\n", hsm.r9300.post0_data.acl_rediCopy);
    } else {
        // HSM3_DATA0
        osal_printf("oam_pdu: %d\t\t\t", hsm.r9300.post_replication_data.oam_pdu);
        osal_printf("ipv4_igmp: %d\t\t\t", hsm.r9300.post_replication_data.ipv4_igmp);
        osal_printf("ipv6_mld: %d\n", hsm.r9300.post_replication_data.ipv6_mld);
        osal_printf("hol_prvnt: %d\t\t\t", hsm.r9300.post_replication_data.hol_prvnt);
        osal_printf("ipv4_chksum_ok: %d\t\t", hsm.r9300.post_replication_data.ipv4_chksum_ok);
        osal_printf("err_pkt: %d\n", hsm.r9300.post_replication_data.err_pkt);
        osal_printf("ip_ttl: 0x%2x\t\t\t", hsm.r9300.post_replication_data.ip_ttl);
        osal_printf("ip_length: 0x%4x\t\t", hsm.r9300.post_replication_data.ip_length);

        // HSM3_DATA1
        osal_printf("pkt_len: 0x%4x\n", hsm.r9300.post_replication_data.pkt_len);
        osal_printf("llc_other: %d\t\t\t", hsm.r9300.post_replication_data.llc_other);
        osal_printf("pppoe_pkt: %d\t\t\t", hsm.r9300.post_replication_data.pppoe_pkt);
        osal_printf("rfc1042: %d\n", hsm.r9300.post_replication_data.rfc1042);

        // HSM3_DATA2
        osal_printf("org_otpid_idx: %2d\t\t", hsm.r9300.post_replication_data.org_otpid_idx);
        osal_printf("org_itpid_idx: %2d\t\t", hsm.r9300.post_replication_data.org_itpid_idx);
        osal_printf("sphy: %2d\n", hsm.r9300.post_replication_data.sphy);
        osal_printf("igr_err: %d\t\t\t", hsm.r9300.post_replication_data.igr_err);
        osal_printf("rtag_type: %d\t\t\t", hsm.r9300.post_replication_data.rtag_type);
        osal_printf("rtag_exist: %d\n", hsm.r9300.post_replication_data.rtag_exist);
        osal_printf("etag_exist: %d\t\t\t", hsm.r9300.post_replication_data.etag_exist);
        osal_printf("ctag_exist: %d\t\t\t", hsm.r9300.post_replication_data.ctag_exist);
        osal_printf("itag_exist: %d\n", hsm.r9300.post_replication_data.itag_exist);
        osal_printf("otag_exist: %d\t\t\t", hsm.r9300.post_replication_data.otag_exist);

        // HSM3_DATA3
        osal_printf("ivid: 0x%3x\t\t\t", hsm.r9300.post_replication_data.ivid);
        osal_printf("icfi: %d\n", hsm.r9300.post_replication_data.icfi);
        osal_printf("ipri: %d\t\t\t\t", hsm.r9300.post_replication_data.ipri);
        osal_printf("acl_rmk_val: 0x%2x\n", hsm.r9300.post_replication_data.acl_rmk_val); // for type setting
        osal_printf("ovid: 0x%3x\t\t\t", hsm.r9300.post_replication_data.ovid);
        osal_printf("ocfi: %d\t\t\t\t", hsm.r9300.post_replication_data.ocfi);
        osal_printf("opri: %d\n", hsm.r9300.post_replication_data.opri);

        // HSM3_DATA4
        osal_printf("ipv6_pkt: %d\t\t\t", hsm.r9300.post_replication_data.ipv6_pkt);
        osal_printf("ipv4_pkt: %d\t\t\t", hsm.r9300.post_replication_data.ipv4_pkt);
        osal_printf("is_mcast_sa: %d\n", hsm.r9300.post_replication_data.is_mcast_sa);
        osal_printf("sa_zero: %d\t\t\t", hsm.r9300.post_replication_data.sa_zero);
        osal_printf("da_mcast: %d\t\t\t", hsm.r9300.post_replication_data.da_mcast);
        osal_printf("da_bcast: %d\n", hsm.r9300.post_replication_data.da_bcast);
        osal_printf("da_bcast: %d\t\t\t", hsm.r9300.post_replication_data.da_bcast);
        osal_printf("eth: %d\t\t\t\t", hsm.r9300.post_replication_data.eth);
        osal_printf("rldp_rlpp_pkt: %d\n", hsm.r9300.post_replication_data.rldp_rlpp_pkt);
        osal_printf("arp_pkt: %d\t\t\t", hsm.r9300.post_replication_data.arp_pkt);
        osal_printf("rma_eapol_pkt: %d\t\t", hsm.r9300.post_replication_data.rma_eapol_pkt);

        // HSM3_DATA5
        osal_printf("lst_dsc: 0x%3x\n", hsm.r9300.post_replication_data.lst_dsc);
        osal_printf("fst_dsc: 0x%3x\t\t\t", hsm.r9300.post_replication_data.fst_dsc);
        osal_printf("page_cnt: 0x%2x\t\t\t", hsm.r9300.post_replication_data.page_cnt);

        // HSM3_DATA6
        osal_printf("rma_rsn: %2d\n", hsm.r9300.post_replication_data.rma_rsn);
        osal_printf("rma_byp_stp: %d\t\t\t", hsm.r9300.post_replication_data.rma_byp_stp);
        osal_printf("rma_byp_vlan_fltr: %d\t\t", hsm.r9300.post_replication_data.rma_byp_vlan_fltr);
        osal_printf("rma_act: %d\n", hsm.r9300.post_replication_data.rma_act);
        osal_printf("rma_pkt: %d\t\t\t", hsm.r9300.post_replication_data.rma_pkt);
        osal_printf("eav_class_b: %d\t\t\t", hsm.r9300.post_replication_data.eav_class_b);
        osal_printf("eav_class_a: %d\n", hsm.r9300.post_replication_data.eav_class_a);
        osal_printf("proto_storm_drop: %d\t\t", hsm.r9300.post_replication_data.proto_storm_drop);
        osal_printf("capwap_trap: %d\t\t\t", hsm.r9300.post_replication_data.capwap_trap);
        osal_printf("vlan_aft_drop: %d\n", hsm.r9300.post_replication_data.vlan_aft_drop);
        osal_printf("vlan_cfi_act: %d\t\t\t", hsm.r9300.post_replication_data.vlan_cfi_act);
        osal_printf("igr_stp_status: %d\t\t", hsm.r9300.post_replication_data.igr_stp_status);
        osal_printf("l2_sa_sab: %d\n", hsm.r9300.post_replication_data.l2_sa_sab);
        osal_printf("l2_sa_hit: %d\t\t\t", hsm.r9300.post_replication_data.l2_sa_hit);
        osal_printf("da_type: %d\t\t\t", hsm.r9300.post_replication_data.da_type);

        // HSM3_DATA7
        osal_printf("dpm: 0x%8x\n", hsm.r9300.post_replication_data.dpm);
        osal_printf("rx_port_in_vlan: %d\t\t", hsm.r9300.post_replication_data.rx_port_in_vlan);
        osal_printf("vlan_src_port_fltr_act: %d\t", hsm.r9300.post_replication_data.vlan_src_port_fltr_act);

        // HSM3_DATA8
        osal_printf("iacl_byp_act: %d\n", hsm.r9300.post_replication_data.iacl_byp_act);
        osal_printf("rma_byp_new_sa_drop: %d\t\t", hsm.r9300.post_replication_data.rma_byp_new_sa_drop);
        osal_printf("trk_id: 0x%2x\t\t\t", hsm.r9300.post_replication_data.trk_id);
        osal_printf("src_port_fltr_en: %d\n", hsm.r9300.post_replication_data.src_port_fltr_en);
        osal_printf("hash_idx: 0x%2x\t\t\t", hsm.r9300.post_replication_data.hash_idx);
        osal_printf("hash_uc: %d\t\t\t", hsm.r9300.post_replication_data.hash_uc);
        osal_printf("unkn_trk_hash_1: 0x%2x\n", hsm.r9300.post_replication_data.unkn_trk_hash_1);
        osal_printf("unkn_trk_hash_0: 0x%2x\t\t", hsm.r9300.post_replication_data.unkn_trk_hash_0);
        osal_printf("trk_hash_1: 0x%2x\t\t", hsm.r9300.post_replication_data.trk_hash_1);

        // HSM3_DATA9
        osal_printf("trk_hash_0: 0x%2x\n", hsm.r9300.post_replication_data.trk_hash_0);
        osal_printf("egr_otpid_idx: %d\t\t", hsm.r9300.post_replication_data.egr_otpid_idx);
        osal_printf("egr_itpid_idx: %d\t\t", hsm.r9300.post_replication_data.egr_itpid_idx);
        osal_printf("int_ocfi: %d\n", hsm.r9300.post_replication_data.int_ocfi);
        osal_printf("int_icfi: %d\t\t\t", hsm.r9300.post_replication_data.int_icfi);
        osal_printf("egr_opri: %d\t\t\t", hsm.r9300.post_replication_data.egr_opri);
        osal_printf("egr_ipri: %d\n", hsm.r9300.post_replication_data.egr_ipri);
        osal_printf("egr_otag_status: %d\t\t", hsm.r9300.post_replication_data.egr_otag_status);
        osal_printf("egr_itag_status: %d\t\t", hsm.r9300.post_replication_data.egr_itag_status);
        osal_printf("ivc_otpid_as: %d\n", hsm.r9300.post_replication_data.ivc_otpid_as);
        osal_printf("ivc_itpid_as: %d\t\t\t", hsm.r9300.post_replication_data.ivc_itpid_as);
        osal_printf("ivc_iacl_opri_as: %d\t\t", hsm.r9300.post_replication_data.ivc_iacl_opri_as);
        osal_printf("ivc_iacl_ipri_as: %d\n", hsm.r9300.post_replication_data.ivc_iacl_ipri_as);
        osal_printf("ivc_iacl_otag_status_as: %d\t", hsm.r9300.post_replication_data.ivc_iacl_otag_status_as);
        osal_printf("ivc_iacl_itag_status_as: %d\t", hsm.r9300.post_replication_data.ivc_iacl_itag_status_as);
        osal_printf("acl_redi: %d\n", hsm.r9300.post_replication_data.acl_redi);
        osal_printf("acl_cpu_fmt: %d\t\t\t", hsm.r9300.post_replication_data.acl_cpu_fmt);

        // HSM3_DATA10
        osal_printf("eacl_qid: %2d\t\t\t", hsm.r9300.post_replication_data.eacl_qid);
        osal_printf("eacl_pri_hit: %d\n", hsm.r9300.post_replication_data.eacl_pri_hit);
        osal_printf("eacl_mir_hit: %d\t\t\t", hsm.r9300.post_replication_data.eacl_mir_hit);
        osal_printf("eacl_mir_act: %d\t\t\t", hsm.r9300.post_replication_data.eacl_mir_act);
        osal_printf("eacl_otpid_hit: %d\n", hsm.r9300.post_replication_data.eacl_otpid_hit);
        osal_printf("eacl_itpid_hit: %d\t\t", hsm.r9300.post_replication_data.eacl_itpid_hit);
        osal_printf("eacl_opri_hit: %d\t\t", hsm.r9300.post_replication_data.eacl_opri_hit);
        osal_printf("eacl_ipri_hit: %d\n", hsm.r9300.post_replication_data.eacl_ipri_hit);
        osal_printf("eacl_otag_hit: %d\t\t", hsm.r9300.post_replication_data.eacl_otag_hit);
        osal_printf("eacl_itag_hit: %d\t\t", hsm.r9300.post_replication_data.eacl_itag_hit);
        osal_printf("n_1_ovid_hit: %d\n", hsm.r9300.post_replication_data.n_1_ovid_hit);
        osal_printf("eacl_ovid_hit: %d\t\t", hsm.r9300.post_replication_data.eacl_ovid_hit);
        osal_printf("igr_ovid: 0x%3x\t\t\t", hsm.r9300.post_replication_data.igr_ovid);

        // HSM3_DATA11
        osal_printf("n_1_ivid_hit: %d\n", hsm.r9300.post_replication_data.n_1_ivid_hit);
        osal_printf("eacl_ivid_hit: %d\t\t", hsm.r9300.post_replication_data.eacl_ivid_hit);
        osal_printf("igr_ivid: 0x%3x\t\t\t", hsm.r9300.post_replication_data.igr_ivid);
        osal_printf("eacl_rmk_hit: %d\n", hsm.r9300.post_replication_data.eacl_rmk_hit);

        // HSM3_DATA12
        osal_printf("iacl_mir_act: %d\t\t\t", hsm.r9300.post_replication_data.iacl_mir_act);
        osal_printf("iacl_mir_hit: %d\t\t\t", hsm.r9300.post_replication_data.iacl_mir_hit);
        osal_printf("acl_idx: 0x%3x\n", hsm.r9300.post_replication_data.acl_idx);
        osal_printf("acl_hit: %d\t\t\t", hsm.r9300.post_replication_data.acl_hit);
        osal_printf("iacl_qid: %2d\t\t\t", hsm.r9300.post_replication_data.iacl_qid);
        osal_printf("int_pri_route: %d\n", hsm.r9300.post_replication_data.int_pri_route);
        osal_printf("int_pri: %d\t\t\t", hsm.r9300.post_replication_data.int_pri);
        osal_printf("byp_igr_bwctrl: %d\t\t", hsm.r9300.post_replication_data.byp_igr_bwctrl);

        // HSM3_DATA13
        osal_printf("atk_type: %2d\n", hsm.r9300.post_replication_data.atk_type);
        osal_printf("atk_act: %d\t\t\t", hsm.r9300.post_replication_data.atk_act);
        osal_printf("oam_act: %d\t\t\t", hsm.r9300.post_replication_data.oam_act);
        osal_printf("mcrt_oil_idx: 0x%3x\n", hsm.r9300.post_replication_data.mcrt_oil_idx);
        osal_printf("mcrt_rpf_asst: %d\t\t", hsm.r9300.post_replication_data.mcrt_rpf_asst);
        osal_printf("org_spa: %2d\t\t\t", hsm.r9300.post_replication_data.org_spa);


        // HSM3_DATA14
        osal_printf("l2_da_block: %d\n", hsm.r9300.post_replication_data.l2_da_block);
        osal_printf("l2_da_hit: %d\t\t\t", hsm.r9300.post_replication_data.l2_da_hit);
        osal_printf("fwd_vid: 0x%3x\t\t\t", hsm.r9300.post_replication_data.fwd_vid);
        osal_printf("orig_fwd_vid: 0x%3x\n", hsm.r9300.post_replication_data.orig_fwd_vid);
        osal_printf("fwd_base: %d\t\t\t", hsm.r9300.post_replication_data.fwd_base);

        // HSM3_DATA15
        osal_printf("l3_da_idx: 0x%3x\t\t", hsm.r9300.post_replication_data.l3_da_idx);
        osal_printf("hash_type: %d\n", hsm.r9300.post_replication_data.hash_type);
        osal_printf("dp: %d\t\t\t\t", hsm.r9300.post_replication_data.dp);
        osal_printf("trk_mc_hit: %d\t\t\t", hsm.r9300.post_replication_data.trk_mc_hit);
        osal_printf("storm_lkmis: %d\n", hsm.r9300.post_replication_data.storm_lkmis);
        osal_printf("nexthop_ageout: %d\t\t", hsm.r9300.post_replication_data.nexthop_ageout);
        osal_printf("nexthop_if: %d\t\t\t", hsm.r9300.post_replication_data.nexthop_if);
        osal_printf("iacl_meter_drop: %d\n", hsm.r9300.post_replication_data.iacl_meter_drop);
        osal_printf("eacl_meter_drop: %d\t\t", hsm.r9300.post_replication_data.eacl_meter_drop);

        // HSM3_DATA16
        osal_printf("ivc_byp_igr_vlan_fltr: %d\t", hsm.r9300.post_replication_data.ivc_byp_igr_vlan_fltr);
        osal_printf("ivc_copy: %d\n", hsm.r9300.post_replication_data.ivc_copy);
        osal_printf("ivc_trap_master: %d\t\t", hsm.r9300.post_replication_data.ivc_trap_master);
        osal_printf("ivc_trap: %d\t\t\t", hsm.r9300.post_replication_data.ivc_trap);
        osal_printf("ivc_drop: %d\n", hsm.r9300.post_replication_data.ivc_drop);
        osal_printf("eacl_copy: %d\t\t\t", hsm.r9300.post_replication_data.eacl_copy);
        osal_printf("eacl_trap_master: %d\t\t", hsm.r9300.post_replication_data.eacl_trap_master);
        osal_printf("eacl_trap: %d\n", hsm.r9300.post_replication_data.eacl_trap);
        osal_printf("eacl_redir_zero: %d\t\t", hsm.r9300.post_replication_data.eacl_redir_zero);
        osal_printf("eacl_drop_act: %d\t\t", hsm.r9300.post_replication_data.eacl_drop_act);
        osal_printf("eacl_drop: %d\n", hsm.r9300.post_replication_data.eacl_drop);
        osal_printf("iacl_copy: %d\t\t\t", hsm.r9300.post_replication_data.iacl_copy);
        osal_printf("iacl_trap_master: %d\t\t", hsm.r9300.post_replication_data.iacl_trap_master);
        osal_printf("iacl_trap: %d\n", hsm.r9300.post_replication_data.iacl_trap);
        osal_printf("iacl_redir_zero: %d\t\t", hsm.r9300.post_replication_data.iacl_redir_zero);
        osal_printf("iacl_drop: %d\t\t\t", hsm.r9300.post_replication_data.iacl_drop);
        osal_printf("iacl_drop_act: %d\n", hsm.r9300.post_replication_data.iacl_drop_act);
        osal_printf("eacl_disable: %d\t\t\t", hsm.r9300.post_replication_data.eacl_disable);
        osal_printf("garp_act: %d\t\t\t", hsm.r9300.post_replication_data.garp_act);
        osal_printf("dhcp_if: %d\n", hsm.r9300.post_replication_data.dhcp_if);
        osal_printf("rip_if: %d\t\t\t", hsm.r9300.post_replication_data.rip_if);

        // HSM3_DATA17
        osal_printf("sa_lrn: %d\t\t\t",hsm.r9300.post_replication_data.sa_lrn);
        osal_printf("st_da_hit: %d\n",hsm.r9300.post_replication_data.st_da_hit);
        osal_printf("trk_hash: %d\t\t\t",hsm.r9300.post_replication_data.trk_hash);
        osal_printf("mir_id: %d\t\t\t",hsm.r9300.post_replication_data.mir_id);
        osal_printf("st_ctag_if: %d\n",hsm.r9300.post_replication_data.st_ctag_if);
        osal_printf("fwd_type: %d\t\t\t",hsm.r9300.post_replication_data.fwd_type);
        osal_printf("dp_info: %d\t\t\t",hsm.r9300.post_replication_data.dp_info);
        osal_printf("dp_fmt: %d\n",hsm.r9300.post_replication_data.dp_fmt);

        // HSM3_DATA18
        osal_printf("sp_fltr_en: %d\t\t\t",hsm.r9300.post_replication_data.sp_fltr_en);
        osal_printf("sp_info: %d\t\t\t",hsm.r9300.post_replication_data.sp_info);
        osal_printf("stk_if: %d\n",hsm.r9300.post_replication_data.stk_if);
        osal_printf("lb_pkt: %d\t\t\t",hsm.r9300.post_replication_data.lb_pkt);
        osal_printf("lb_ttl: %d\t\t\t",hsm.r9300.post_replication_data.lb_ttl);
        osal_printf("acl_lb_act: %d\n",hsm.r9300.post_replication_data.acl_lb_act);
        osal_printf("metadata: %d\t\t\t",hsm.r9300.post_replication_data.metadata);
        osal_printf("ctag_flag: %d\t\t\t",hsm.r9300.post_replication_data.ctag_flag);
        osal_printf("vb_iso_mbr: %d\n",hsm.r9300.post_replication_data.vb_iso_mbr);

        // HSM3_DATA19
        osal_printf("ip_rsvd_inv: %d\t\t\t",hsm.r9300.post_replication_data.ip_rsvd_inv);
        osal_printf("l2_hash_full: %d\t\t\t",hsm.r9300.post_replication_data.l2_hash_full);
        osal_printf("force_pmove: %d\n",hsm.r9300.post_replication_data.force_pmove);
        osal_printf("dyn_pm_act: %d\t\t\t",hsm.r9300.post_replication_data.dyn_pm_act);
        osal_printf("sttc_pm_act: %d\t\t\t",hsm.r9300.post_replication_data.sttc_pm_act);
        osal_printf("mac_limit_act: %d\n",hsm.r9300.post_replication_data.mac_limit_act);
        osal_printf("mac_limit_vlan_act: %d\t\t",hsm.r9300.post_replication_data.mac_limit_vlan_act);
        osal_printf("new_sa_act: %d\t\t\t",hsm.r9300.post_replication_data.new_sa_act);
        osal_printf("sptrsn: %d\n",hsm.r9300.post_replication_data.sptrsn);

        // HSM3_DATA20
        osal_printf("ip_rsv_act: %d\t\t\t",hsm.r9300.post_replication_data.ip_rsv_act);
        osal_printf("macMiss_act: %d\t\t\t",hsm.r9300.post_replication_data.macMiss_act);

        // HSM3_DATA21-22
        osal_printf("ctx_unit: %d\n",hsm.r9300.post_replication_data.ctx_unit);
        osal_printf("ctx_dpm: %8x-%8x\t",(uint32)(hsm.r9300.post_replication_data.ctx_dpm & 0xffffffff),(uint32)(hsm.r9300.post_replication_data.ctx_dpm >> 32) & 0xffffff);
        osal_printf("ctx_dpm_type: %d\n",hsm.r9300.post_replication_data.ctx_dpm_type);

        // HSM3_DATA23
        osal_printf("ctx_sp_fltr: %d\t\t\t",hsm.r9300.post_replication_data.ctx_sp_fltr);
        osal_printf("ctx_fwd_vid_en: %d\t\t",hsm.r9300.post_replication_data.ctx_fwd_vid_en);
        osal_printf("ctx_as_qid: %d\n",hsm.r9300.post_replication_data.ctx_as_qid);
        osal_printf("ctx_qid: %d\t\t\t",hsm.r9300.post_replication_data.ctx_qid);
        osal_printf("ctx_org_tag: %d\t\t\t",hsm.r9300.post_replication_data.ctx_org_tag);
        osal_printf("ctx_l3_act: %d\n",hsm.r9300.post_replication_data.ctx_l3_act);
        osal_printf("ctx_as_tag: %d\t\t\t",hsm.r9300.post_replication_data.ctx_as_tag);
        osal_printf("ctx_bp_egr_vlan: %d\t\t",hsm.r9300.post_replication_data.ctx_bp_egr_vlan);
        osal_printf("ctx_bp_stp: %d\n",hsm.r9300.post_replication_data.ctx_bp_stp);
        osal_printf("ctx_bp_fltr: %d\t\t\t",hsm.r9300.post_replication_data.ctx_bp_fltr);
        osal_printf("ctx_dg_gasp: %d\t\t\t",hsm.r9300.post_replication_data.ctx_dg_gasp);
        osal_printf("ctx_cngst_drop: %d\n",hsm.r9300.post_replication_data.ctx_cngst_drop);
        osal_printf("ctx_acl_act: %d\t\t\t",hsm.r9300.post_replication_data.ctx_acl_act);

        // HSM3_DATA24 -26
        osal_printf("crx_data[0]: %d\t\t\t",hsm.r9300.post_replication_data.crx_data[0]);
        osal_printf("crx_data[1]: %d\n",hsm.r9300.post_replication_data.crx_data[1]);
        osal_printf("crx_data[2]: %d\t\t\t",hsm.r9300.post_replication_data.crx_data[2] & 0xffff);

        // HSM3_DATA27
        osal_printf("rt_ipmc_lstcp: 0x%3x\t\t", hsm.r9300.post_replication_data.rt_ipmc_lstcp);
        osal_printf("rt_ipmc_fstcp: 0x%3x\n", hsm.r9300.post_replication_data.rt_ipmc_fstcp);
        osal_printf("rt_ipmc_uccp: 0x%3x\t\t", hsm.r9300.post_replication_data.rt_ipmc_uccp);
        osal_printf("rt_ipmc_qid: 0x%3x\t", hsm.r9300.post_replication_data.rt_ipmc_qid);
        osal_printf("rt_ipmc_type: 0x%3x\n", hsm.r9300.post_replication_data.rt_ipmc_type);
        osal_printf("rt_ipmc_ttl_dec: 0x%3x\t\t", hsm.r9300.post_replication_data.rt_ipmc_ttl_dec);
        osal_printf("ipmc_pkt: 0x%3x\t\t\t", hsm.r9300.post_replication_data.ipmc_pkt);
        osal_printf("route_en: 0x%3x\n", hsm.r9300.post_replication_data.route_en);
        osal_printf("l3_sa_idx: %2d\t\t\t", hsm.r9300.post_replication_data.l3_sa_idx);
        osal_printf("l3_sa_rep: %d\t\t\t", hsm.r9300.post_replication_data.l3_sa_rep);
        osal_printf("l3_da_rep: %d\n", hsm.r9300.post_replication_data.l3_da_rep);
        osal_printf("l3_ttl_dec: %d\t\t\t", hsm.r9300.post_replication_data.l3_ttl_dec);
        osal_printf("ip_route_en: %d\n", hsm.r9300.post_replication_data.ip_route_en);
        osal_printf("l3_uc_kn: %d\t\t\t", hsm.r9300.post_replication_data.l3_uc_kn);
        osal_printf("st_pmsk: %d\t\t\t", hsm.r9300.post_replication_data.st_pmsk);
        osal_printf("acl_rediCopy: %d\n", hsm.r9300.post_replication_data.acl_rediCopy);
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8380)
int32 _hal_dumpHsm_8380(uint32 unit, uint32 index)
{
    hsm_param_t   hsm;
    uint32        tmp;
    hal_control_t *pInfo;

    if(RT_ERR_OK != _hal_getHsm_8380(unit, index, &hsm))
    {
        return RT_ERR_FAILED;
    }

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    osal_printf("\n========================================  HSM %u  ========================================\n", index);
    tmp = hsm.r8380.acl_idx0;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx0", tmp);
    tmp = hsm.r8380.acl_idx1;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx1", tmp);
    tmp = hsm.r8380.acl_idx2;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x\n", "acl_idx2", tmp);

    tmp = hsm.r8380.acl_idx3;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx3", tmp);
    tmp = hsm.r8380.acl_idx4;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx4", tmp);
    tmp = hsm.r8380.acl_idx5;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x\n", "acl_idx5", tmp);

    tmp = hsm.r8380.acl_idx6;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx6", tmp);
    tmp = hsm.r8380.acl_idx7;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx7", tmp);
    tmp = hsm.r8380.acl_idx8;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x\n", "acl_idx8", tmp);

    tmp = hsm.r8380.acl_idx9;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx9", tmp);
    tmp = hsm.r8380.acl_idx10;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x", "acl_idx10", tmp);
    tmp = hsm.r8380.acl_idx11;
    tmp = ((tmp << 1) & 0x100) | (tmp & 0x7f);
    osal_printf("%-18s = 0x%-8x\n", "acl_idx11", tmp);

    osal_printf("%-18s = 0x%-8x", "acl_dpm", hsm.r8380.acl_dpm);
    osal_printf("%-18s = 0x%-8x", "acl_idx", (hsm.r8380.acl_idx_10 << 10) | hsm.r8380.acl_idx_9_0);
    osal_printf("%-18s = 0x%-8x\n", "acl_copy_hit", hsm.r8380.acl_copy_hit);

    osal_printf("%-18s = 0x%-8x", "acl_redir", hsm.r8380.acl_redir);
    osal_printf("%-18s = 0x%-8x", "acl_fwd_info", hsm.r8380.acl_fwd_info);
    osal_printf("%-18s = 0x%-8x\n", "acl_log_hit", hsm.r8380.acl_log_hit);

    osal_printf("%-18s = 0x%-8x", "acl_rout", hsm.r8380.acl_rout);
    osal_printf("%-18s = 0x%-8x", "acl_rmk_hit", hsm.r8380.acl_rmk_hit);
    osal_printf("%-18s = 0x%-8x\n", "acl_rmk_act", hsm.r8380.acl_rmk_act);

    osal_printf("%-18s = 0x%-8x", "acl_rmk_val", hsm.r8380.acl_rmk_val);
    osal_printf("%-18s = 0x%-8x", "acl_cpuPri_hit", hsm.r8380.acl_cpuPri_hit);
    osal_printf("%-18s = 0x%-8x\n", "acl_cpuPri", hsm.r8380.acl_cpuPri);

    osal_printf("%-18s = 0x%-8x", "acl_normalPri_hit", hsm.r8380.acl_normalPri_hit);
    osal_printf("%-18s = 0x%-8x", "acl_mir_hit", hsm.r8380.acl_mir_hit);
    osal_printf("%-18s = 0x%-8x\n", "acl_mir_act", hsm.r8380.acl_mir_act);

    osal_printf("%-18s = 0x%-8x", "acl_mir_idx", hsm.r8380.acl_mir_idx);
    osal_printf("%-18s = 0x%-8x", "acl_fltr_dpm", hsm.r8380.acl_fltr_dpm);
    osal_printf("%-18s = 0x%-8x\n", "acl_fltr_hit", hsm.r8380.acl_fltr_hit);

    osal_printf("%-18s = 0x%-8x", "acl_ivid_hit", hsm.r8380.acl_ivid_hit);
    osal_printf("%-18s = 0x%-8x", "acl_ovid_hit", hsm.r8380.acl_ovid_hit);
    osal_printf("%-18s = 0x%-8x\n", "acl_egr_tagSts_hit", hsm.r8380.acl_egr_tagSts_hit);

    osal_printf("%-18s = 0x%-8x", "acl_meter_hit", hsm.r8380.acl_meter);
    osal_printf("%-18s = 0x%-8x", "acl_force", hsm.r8380.acl_force);
    osal_printf("%-18s = 0x%-8x\n", "acl_cpuTag", hsm.r8380.acl_cpuTag);

    osal_printf("%-18s = 0x%-8x", "acl_otpid_hit", hsm.r8380.acl_otpid_hit);
    osal_printf("%-18s = 0x%-8x", "acl_otpid_idx", hsm.r8380.acl_otpid_idx);
    osal_printf("%-18s = 0x%-8x\n", "acl_itpid_hit", hsm.r8380.acl_itpid_hit);

    osal_printf("%-18s = 0x%-8x", "acl_itpid_idx", hsm.r8380.acl_itpid_idx);
    osal_printf("%-18s = 0x%-8x", "rng_chk_comm", hsm.r8380.rng_chk_comm);
    osal_printf("%-18s = 0x%-8x\n", "rng_chk_spm", hsm.r8380.rng_chk_spm);

    osal_printf("%-18s = 0x%-8x", "rng_chk_ip", hsm.r8380.rng_chk_ip);
    osal_printf("%-18s = 0x%-8x", "l2_sa_hit", hsm.r8380.l2_sa_hit);
    osal_printf("%-18s = 0x%-8x\n", "l2_sa_idx", hsm.r8380.l2_sa_idx);

    osal_printf("%-18s = 0x%-8x", "l2_da_lm", hsm.r8380.l2_da_lm);
    osal_printf("%-18s = 0x%-8x", "l2_da_hit", hsm.r8380.l2_da_hit);
    osal_printf("%-18s = 0x%-8x\n", "l2_da_idx", hsm.r8380.l2_da_idx);

    osal_printf("%-18s = 0x%-8x", "dis_sa_lrn", hsm.r8380.dis_sa_lrn);
    osal_printf("%-18s = 0x%-8x", "rma_fld", hsm.r8380.rma_fld);
    osal_printf("%-18s = 0x%-8x\n", "ale_ovid", hsm.r8380.ale_ovid);

    osal_printf("%-18s = 0x%-8x", "ale_ivid", hsm.r8380.ale_ivid);
    osal_printf("%-18s = 0x%-8x", "itag_sts", hsm.r8380.itag_sts);
    osal_printf("%-18s = 0x%-8x\n", "otag_sts", hsm.r8380.otag_sts);

    osal_printf("%-18s = 0x%-8x", "ale_drop", hsm.r8380.ale_drop);
    osal_printf("%-18s = 0x%-8x", "ale_trap", hsm.r8380.ale_trap);
    osal_printf("%-18s = 0x%-8x\n", "copyToCpu", hsm.r8380.copyToCpu);

    osal_printf("%-18s = 0x%-8x", "int_pri", hsm.r8380.int_pri);
    osal_printf("%-18s = 0x%-8x", "cpu_pri", hsm.r8380.cpu_pri);
    osal_printf("%-18s = 0x%-8x\n", "dpm", hsm.r8380.dpm);

    osal_printf("%-18s = 0x%-8x", "slp", hsm.r8380.slp);
    osal_printf("%-18s = 0x%-8x", "fid", hsm.r8380.fid);
    osal_printf("%-18s = 0x%-8x\n", "input_qid", hsm.r8380.input_qid);

    osal_printf("%-18s = 0x%-8x", "vlan_fwd_base", hsm.r8380.vlan_fwd_base);
    osal_printf("%-18s = 0x%-8x", "atk_prvnt_rsn", hsm.r8380.atk_prvnt_rsn);
    osal_printf("%-18s = 0x%-8x\n", "eav_class_a", hsm.r8380.eav_class_a);

    osal_printf("%-18s = 0x%-8x", "eav_class_b", hsm.r8380.eav_class_b);
    osal_printf("%-18s = 0x%-8x", "egr_vlan_lky", hsm.r8380.egr_vlan_lky);
    osal_printf("%-18s = 0x%-8x\n", "byp_igr_bwctrl", hsm.r8380.byp_igr_bwctrl);

    osal_printf("%-18s = 0x%-8x", "byp_storm", hsm.r8380.byp_storm);
    osal_printf("%-18s = 0x%-8x", "byp_igr_stp", hsm.r8380.byp_igr_stp);
    osal_printf("%-18s = 0x%-8x\n", "igr_vlan_lky", hsm.r8380.igr_vlan_lky);

    osal_printf("%-18s = 0x%-8x", "copy_igrVlan_lky", hsm.r8380.copy_igrVlan_lky);
    osal_printf("%-18s = 0x%-8x", "copy_igrStp_lky", hsm.r8380.copy_igrStp_lky);
    osal_printf("%-18s = 0x%-8x\n", "byp_egr_stp_pmsk", hsm.r8380.byp_egr_stp_pmsk);

    osal_printf("%-18s = 0x%-8x\n", "knmc_byp_vlan_efilter", hsm.r8380.knmc_byp_vlan_efilter);

    osal_printf("-----------------------------------------drop-----------------------------------------\n");
    osal_printf("%-21s = 0x%-3x", "rspan_drop", hsm.r8380.rspan_drop);
    osal_printf("%-21s = 0x%-3x", "rma_drop", hsm.r8380.rma_drop);
    osal_printf("%-21s = 0x%-3x\n", "atk_prvnt_drop", hsm.r8380.atk_prvnt_drop);

    osal_printf("%-21s = 0x%-3x", "acl_lm_drop", hsm.r8380.acl_lm_drop);
    osal_printf("%-21s = 0x%-3x", "acl_drop", hsm.r8380.acl_drop);
    osal_printf("%-21s = 0x%-3x\n", "acl_redir_drop", hsm.r8380.acl_redir_drop);

    osal_printf("%-21s = 0x%-3x", "acl_meter_drop", hsm.r8380.acl_meter_drop);
    osal_printf("%-21s = 0x%-3x", "acl_filter_drop", hsm.r8380.acl_filter_drop);
    osal_printf("%-21s = 0x%-3x\n", "vlan_aft_drop", hsm.r8380.vlan_aft_drop);

    osal_printf("%-21s = 0x%-3x", "vlan_cfi_drop", hsm.r8380.vlan_cfi_drop);
    osal_printf("%-21s = 0x%-3x", "vlan_igr_drop", hsm.r8380.vlan_igr_drop);
    osal_printf("%-21s = 0x%-3x\n", "vlan_egr_drop", hsm.r8380.vlan_egr_drop);

    osal_printf("%-21s = 0x%-3x", "mstp_igr_drop", hsm.r8380.mstp_igr_drop);
    osal_printf("%-21s = 0x%-3x", "l2_invldSA_drop", hsm.r8380.l2_invldSA_drop);
    osal_printf("%-21s = 0x%-3x\n", "l2_sa_blk", hsm.r8380.l2_sa_blk);

    osal_printf("%-21s = 0x%-3x", "l2_portMove_drop", hsm.r8380.l2_portMove_drop);
    osal_printf("%-21s = 0x%-3x", "l2_newSA_drop", hsm.r8380.l2_newSA_drop);
    osal_printf("%-21s = 0x%-3x\n", "l2_sys_constrt_drop", hsm.r8380.l2_sys_constrt_drop);

    osal_printf("%-21s = 0x%-3x", "l2_port_constrt_drop", hsm.r8380.l2_port_constrt_drop);
    osal_printf("%-21s = 0x%-3x", "l2_vlan_constrt_drop", hsm.r8380.l2_vlan_constrt_drop);
    osal_printf("%-21s = 0x%-3x\n", "l2_da_blk", hsm.r8380.l2_da_blk);

    osal_printf("%-21s = 0x%-3x", "l2_lm_drop", hsm.r8380.l2_lm_drop);
    osal_printf("%-21s = 0x%-3x", "l3_rout_drop", hsm.r8380.l3_rout_drop);
    osal_printf("%-21s = 0x%-3x\n", "input_q_drop", hsm.r8380.input_q_drop);

    osal_printf("%-21s = 0x%-3x", "invld_dpm_drop", hsm.r8380.invld_dpm_drop);
    osal_printf("%-21s = 0x%-3x", "invld_mcast_dpm_drop", hsm.r8380.invld_mcast_dpm_drop);
    osal_printf("%-21s = 0x%-3x\n", "mstp_egr_drop", hsm.r8380.mstp_egr_drop);

    osal_printf("%-21s = 0x%-3x", "storm_drop", hsm.r8380.storm_drop);
    osal_printf("%-21s = 0x%-3x", "src_port_filter_drop", hsm.r8380.src_port_filter_drop);
    osal_printf("%-21s = 0x%-3x\n", "isolation_drop", hsm.r8380.isolation_drop);

    osal_printf("%-21s = 0x%-3x", "trunk_drop", hsm.r8380.trunk_drop);
    osal_printf("%-21s = 0x%-3x", "mir_filter_drop", hsm.r8380.mir_filter_drop);
    osal_printf("%-21s = 0x%-3x\n", "sTrap_selfMac_drop", hsm.r8380.sTrap_selfMac_drop);

    osal_printf("%-21s = 0x%-3x", "link_down_drop", hsm.r8380.link_down_drop);
    osal_printf("%-21s = 0x%-3x", "flowCtrl_drop", hsm.r8380.flowCtrl_drop);
    osal_printf("%-21s = 0x%-3x\n", "tx_maxLen_drop", hsm.r8380.tx_maxLen_drop);

    osal_printf("%-21s = 0x%-3x\n", "reason_n_drop", hsm.r8380.reason_n_drop);
    osal_printf("-----------------------------------------copy-----------------------------------------\n");
    osal_printf("%-21s = 0x%-3x", "sTrap_copy", hsm.r8380.sTrap_copy);
    osal_printf("%-21s = 0x%-3x", "l2_sys_constrt_copy", hsm.r8380.l2_sys_constrt_copy);
    osal_printf("%-21s = 0x%-3x\n", "l2_port_constrt_copy", hsm.r8380.l2_port_constrt_copy);

    osal_printf("%-21s = 0x%-3x", "l2_vlan_constrt_copy", hsm.r8380.l2_vlan_constrt_copy);
    osal_printf("%-21s = 0x%-3x", "l2_newSA_copy", hsm.r8380.l2_newSA_copy);
    osal_printf("%-21s = 0x%-3x\n", "l2_portMove_copy", hsm.r8380.l2_portMove_copy);

    osal_printf("%-21s = 0x%-3x", "l2_lm_copy", hsm.r8380.l2_lm_copy);
    osal_printf("%-21s = 0x%-3x", "acl_copy", hsm.r8380.acl_copy);
    osal_printf("%-21s = 0x%-3x\n", "reason_n_copy", hsm.r8380.reason_n_copy);
    osal_printf("-----------------------------------------trap-----------------------------------------\n");
    osal_printf("%-21s = 0x%-3x", "rma_trap", hsm.r8380.rma_trap);
    osal_printf("%-21s = 0x%-3x", "atk_prvnt_trap", hsm.r8380.atk_prvnt_trap);
    osal_printf("%-21s = 0x%-3x\n", "rldp_rlpp_trap", hsm.r8380.rldp_rlpp_trap);

    osal_printf("%-21s = 0x%-3x", "sTrap_comm_trap", hsm.r8380.sTrap_comm_trap);
    osal_printf("%-21s = 0x%-3x", "acl_trap", hsm.r8380.acl_trap);
    osal_printf("%-21s = 0x%-3x\n", "l3_rout_trap", hsm.r8380.l3_rout_trap);

    osal_printf("%-21s = 0x%-3x", "vlan_cfi_trap", hsm.r8380.vlan_cfi_trap);
    osal_printf("%-21s = 0x%-3x", "vlan_igr_trap", hsm.r8380.vlan_igr_trap);
    osal_printf("%-21s = 0x%-3x\n", "sTrap_selfMac_trap", hsm.r8380.sTrap_selfMac_trap);

    osal_printf("%-21s = 0x%-3x", "l2_portMove_trap", hsm.r8380.l2_portMove_trap);
    osal_printf("%-21s = 0x%-3x", "l2_newSA_trap", hsm.r8380.l2_newSA_trap);
    osal_printf("%-21s = 0x%-3x\n", "l2_sys_constrt_trap", hsm.r8380.l2_sys_constrt_trap);

    osal_printf("%-21s = 0x%-3x", "l2_port_constrt_trap", hsm.r8380.l2_port_constrt_trap);
    osal_printf("%-21s = 0x%-3x", "l2_vlan_constrt_trap", hsm.r8380.l2_vlan_constrt_trap);
    osal_printf("%-21s = 0x%-3x\n", "l2_lm_trap", hsm.r8380.l2_lm_trap);

    osal_printf("%-21s = 0x%-3x\n", "reason_n_trap", hsm.r8380.reason_n_trap);
    osal_printf("====================================================================================\n");

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_dumpHsmIdx
 * Description:
 *      Dump hsm paramter of the specified device.
 * Input:
 *      unit - unit id
 *      index -hsm index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_dumpHsmIdx(uint32 unit, uint32 index)
{
    hal_control_t *pHalCtrl = NULL;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        if(RT_ERR_OK != _hal_dumpHsm_8380(unit, index))
        {
            return RT_ERR_FAILED;
        }
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        if(RT_ERR_OK != _hal_dumpHsm_9300(unit, index))
        {
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
} /* end of hal_dumpHsmIdx */
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getDbgCntr
 * Description:
 *      Get debug counter of the specified device.
 * Input:
 *      unit  - unit id
 *      type  - debug counter type
 * Output:
 *      pCntr - value of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getDbgCntr(uint32 unit, rtk_dbg_mib_dbgType_t type, uint32 *pCntr)
{
    int32 ret = RT_ERR_FAILED;

    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }


#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        switch (type)
        {
            case RTL8390_DBG_MIB_ALE_TX_GOOD_PKTS:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER0r,
                        CYPRESS_REASON_0f, pCntr);
                break;
            case RTL8390_DBG_MIB_ERROR_PKTS:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER1r,
                        CYPRESS_REASON_1f, pCntr);
                break;
            case RTL8390_DBG_MIB_EGR_ACL_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER2r,
                        CYPRESS_REASON_2f, pCntr);
                break;
            case RTL8390_DBG_MIB_EGR_METER_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER3r,
                        CYPRESS_REASON_3f, pCntr);
                break;
            case RTL8390_DBG_MIB_OAM:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER4r,
                        CYPRESS_REASON_4f, pCntr);
                break;
            case RTL8390_DBG_MIB_CFM:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER5r,
                        CYPRESS_REASON_5f, pCntr);
                break;
            case RTL8390_DBG_MIB_VLAN_IGR_FLTR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER6r,
                        CYPRESS_REASON_6f, pCntr);
                break;
            case RTL8390_DBG_MIB_VLAN_ERR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER7r,
                        CYPRESS_REASON_7f, pCntr);
                break;
            case RTL8390_DBG_MIB_INNER_OUTER_CFI_EQUAL_1:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER8r,
                        CYPRESS_REASON_8f, pCntr);
                break;
            case RTL8390_DBG_MIB_VLAN_TAG_FORMAT:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER9r,
                        CYPRESS_REASON_9f, pCntr);
                break;
            case RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER10r,
                        CYPRESS_REASON_10f, pCntr);
                break;
            case RTL8390_DBG_MIB_INBW:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER11r,
                        CYPRESS_REASON_11f, pCntr);
                break;
            case RTL8390_DBG_MIB_RMA:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER12r,
                        CYPRESS_REASON_12f, pCntr);
                break;
            case RTL8390_DBG_MIB_HW_ATTACK_PREVENTION:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER13r,
                        CYPRESS_REASON_13f, pCntr);
                break;
            case RTL8390_DBG_MIB_PROTO_STORM:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER14r,
                        CYPRESS_REASON_14f, pCntr);
                break;
            case RTL8390_DBG_MIB_MCAST_SA:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER15r,
                        CYPRESS_REASON_15f, pCntr);
                break;
            case RTL8390_DBG_MIB_IGR_ACL_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER16r,
                        CYPRESS_REASON_16f, pCntr);
                break;
            case RTL8390_DBG_MIB_IGR_METER_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER17r,
                        CYPRESS_REASON_17f, pCntr);
                break;
            case RTL8390_DBG_MIB_DFLT_ACTION_FOR_MISS_ACL_AND_C2SC:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER18r,
                        CYPRESS_REASON_18f, pCntr);
                break;
            case RTL8390_DBG_MIB_NEW_SA:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER19r,
                        CYPRESS_REASON_19f, pCntr);
                break;
            case RTL8390_DBG_MIB_PORT_MOVE:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER20r,
                        CYPRESS_REASON_20f, pCntr);
                break;
            case RTL8390_DBG_MIB_SA_BLOCKING:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER21r,
                        CYPRESS_REASON_21f, pCntr);
                break;
            case RTL8390_DBG_MIB_ROUTING_EXCEPTION:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER22r,
                        CYPRESS_REASON_22f, pCntr);
                break;
            case RTL8390_DBG_MIB_SRC_PORT_SPENDING_TREE_NON_FWDING:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER23r,
                        CYPRESS_REASON_23f, pCntr);
                break;
            case RTL8390_DBG_MIB_MAC_LIMIT:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER24r,
                        CYPRESS_REASON_24f, pCntr);
                break;
            case RTL8390_DBG_MIB_UNKNOW_STORM:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER25r,
                        CYPRESS_REASON_25f, pCntr);
                break;
            case RTL8390_DBG_MIB_MISS_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER26r,
                        CYPRESS_REASON_26f, pCntr);
                break;
            case RTL8390_DBG_MIB_CPU_MAC_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER27r,
                        CYPRESS_REASON_27f, pCntr);
                break;
            case RTL8390_DBG_MIB_DA_BLOCKING:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER28r,
                        CYPRESS_REASON_28f, pCntr);
                break;
            case RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_EGR_ACL:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER29r,
                        CYPRESS_REASON_29f, pCntr);
                break;
            case RTL8390_DBG_MIB_VLAN_EGR_FILTER:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER30r,
                        CYPRESS_REASON_30f, pCntr);
                break;
            case RTL8390_DBG_MIB_SPANNING_TRE:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER31r,
                        CYPRESS_REASON_31f, pCntr);
                break;
            case RTL8390_DBG_MIB_PORT_ISOLATION:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER32r,
                        CYPRESS_REASON_32f, pCntr);
                break;
            case RTL8390_DBG_MIB_OAM_EGRESS_DROP:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER33r,
                        CYPRESS_REASON_33f, pCntr);
                break;
            case RTL8390_DBG_MIB_MIRROR_ISOLATION:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER34r,
                        CYPRESS_REASON_34f, pCntr);
                break;
            case RTL8390_DBG_MIB_MAX_LEN_BEFORE_EGR_ACL:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER35r,
                        CYPRESS_REASON_35f, pCntr);
                break;
            case RTL8390_DBG_MIB_SRC_PORT_FILTER_BEFORE_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER36r,
                        CYPRESS_REASON_36f, pCntr);
                break;
            case RTL8390_DBG_MIB_MAX_LEN_BEFORE_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER37r,
                        CYPRESS_REASON_37f, pCntr);
                break;
            case RTL8390_DBG_MIB_SPECIAL_CONGEST_BEFORE_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER38r,
                        CYPRESS_REASON_38f, pCntr);
                break;
            case RTL8390_DBG_MIB_LINK_STATUS_BEFORE_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER39r,
                        CYPRESS_REASON_39f, pCntr);
                break;
            case RTL8390_DBG_MIB_WRED_BEFORE_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER40r,
                        CYPRESS_REASON_40f, pCntr);
                break;
            case RTL8390_DBG_MIB_MAX_LEN_AFTER_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER41r,
                        CYPRESS_REASON_41f, pCntr);
                break;
            case RTL8390_DBG_MIB_SPECIAL_CONGEST_AFTER_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER42r,
                        CYPRESS_REASON_42f, pCntr);
                break;
            case RTL8390_DBG_MIB_LINK_STATUS_AFTER_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER43r,
                        CYPRESS_REASON_43f, pCntr);
                break;
            case RTL8390_DBG_MIB_WRED_AFTER_MIRROR:
                ret = reg_field_read(unit, CYPRESS_STAT_PRVTE_DROP_COUNTER44r,
                        CYPRESS_REASON_44f, pCntr);
                break;
            default:
                break;
        }
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        switch (type)
        {
            case RTL8380_DBG_MIB_ALE_TX_GOOD_PKTS:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER0r,
                        MAPLE_REASON_0f, pCntr);
                break;
            case RTL8380_DBG_MIB_MAC_RX_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER1r,
                        MAPLE_REASON_1f, pCntr);
                break;
            case RTL8380_DBG_MIB_ACL_FWD_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER2r,
                        MAPLE_REASON_2f, pCntr);
                break;
            case RTL8380_DBG_MIB_HW_ATTACK_PREVENTION_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER3r,
                        MAPLE_REASON_3f, pCntr);
                break;
            case RTL8380_DBG_MIB_RMA_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER4r,
                        MAPLE_REASON_4f, pCntr);
                break;
            case RTL8380_DBG_MIB_VLAN_IGR_FLTR_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER5r,
                        MAPLE_REASON_5f, pCntr);
                break;
            case RTL8380_DBG_MIB_INNER_OUTER_CFI_EQUAL_1_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER6r,
                        MAPLE_REASON_6f, pCntr);
                break;
            case RTL8380_DBG_MIB_PORT_MOVE_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER7r,
                        MAPLE_REASON_7f, pCntr);
                break;
            case RTL8380_DBG_MIB_NEW_SA_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER8r,
                        MAPLE_REASON_8f, pCntr);
                break;
            case RTL8380_DBG_MIB_MAC_LIMIT_SYS_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER9r,
                        MAPLE_REASON_9f, pCntr);
                break;
            case RTL8380_DBG_MIB_MAC_LIMIT_VLAN_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER10r,
                        MAPLE_REASON_10f, pCntr);
                break;
            case RTL8380_DBG_MIB_MAC_LIMIT_PORT_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER11r,
                        MAPLE_REASON_11f, pCntr);
                break;
            case RTL8380_DBG_MIB_SWITCH_MAC_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER12r,
                        MAPLE_REASON_12f, pCntr);
                break;
            case RTL8380_DBG_MIB_ROUTING_EXCEPTION_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER13r,
                        MAPLE_REASON_13f, pCntr);
                break;
            case RTL8380_DBG_MIB_DA_LKMISS_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER14r,
                        MAPLE_REASON_14f, pCntr);
                break;
            case RTL8380_DBG_MIB_RSPAN_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER15r,
                        MAPLE_REASON_15f, pCntr);
                break;
            case RTL8380_DBG_MIB_ACL_LKMISS_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER16r,
                        MAPLE_REASON_16f, pCntr);
                break;
            case RTL8380_DBG_MIB_ACL_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER17r,
                        MAPLE_REASON_17f, pCntr);
                break;
            case RTL8380_DBG_MIB_INBW_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER18r,
                        MAPLE_REASON_18f, pCntr);
                break;
            case RTL8380_DBG_MIB_IGR_METER_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER19r,
                        MAPLE_REASON_19f, pCntr);
                break;

            case RTL8380_DBG_MIB_ACCEPT_FRAME_TYPE_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER20r,
                        MAPLE_REASON_20f, pCntr);
                break;
            case RTL8380_DBG_MIB_STP_IGR_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER21r,
                        MAPLE_REASON_21f, pCntr);
                break;
            case RTL8380_DBG_MIB_INVALID_SA_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER22r,
                        MAPLE_REASON_22f, pCntr);
                break;
            case RTL8380_DBG_MIB_SA_BLOCKING_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER23r,
                        MAPLE_REASON_23f, pCntr);
                break;
            case RTL8380_DBG_MIB_DA_BLOCKING_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER24r,
                        MAPLE_REASON_24f, pCntr);
                break;
            case RTL8380_DBG_MIB_L2_INVALID_DPM_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER25r,
                        MAPLE_REASON_25f, pCntr);
                break;
            case RTL8380_DBG_MIB_MCST_INVALID_DPM_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER26r,
                        MAPLE_REASON_26f, pCntr);
                break;
            case RTL8380_DBG_MIB_RX_FLOW_CONTROL_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER27r,
                        MAPLE_REASON_27f, pCntr);
                break;
            case RTL8380_DBG_MIB_STORM_SPPRS_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER28r,
                        MAPLE_REASON_28f, pCntr);
                break;
            case RTL8380_DBG_MIB_LALS_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER29r,
                        MAPLE_REASON_29f, pCntr);
                break;
            case RTL8380_DBG_MIB_VLAN_EGR_FILTER_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER30r,
                        MAPLE_REASON_30f, pCntr);
                break;
            case RTL8380_DBG_MIB_STP_EGR_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER31r,
                        MAPLE_REASON_31f, pCntr);
                break;
            case RTL8380_DBG_MIB_SRC_PORT_FILTER_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER32r,
                        MAPLE_REASON_32f, pCntr);
                break;
            case RTL8380_DBG_MIB_PORT_ISOLATION_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER33r,
                        MAPLE_REASON_33f, pCntr);
                break;
            case RTL8380_DBG_MIB_ACL_FLTR_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER34r,
                        MAPLE_REASON_34f, pCntr);
                break;

            case RTL8380_DBG_MIB_MIRROR_FLTR_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER35r,
                        MAPLE_REASON_35f, pCntr);
                break;
            case RTL8380_DBG_MIB_TX_MAX_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER36r,
                        MAPLE_REASON_36f, pCntr);
                break;
            case RTL8380_DBG_MIB_LINK_DOWN_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER37r,
                        MAPLE_REASON_37f, pCntr);
                break;
            case RTL8380_DBG_MIB_FLOW_CONTROL_DROP:
                ret = reg_field_read(unit, MAPLE_STAT_PRVTE_DROP_COUNTER38r,
                        MAPLE_REASON_38f, pCntr);
                break;

            default:
                break;
        }
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        switch (type)
        {
            case RTL9310_DBG_CNTR_ALE_RX_GOOD_PKTS:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR0r, MANGO_REASON_0f, pCntr);
                break;
            case RTL9310_DBG_CNTR_RX_MAX_FRAME_SIZE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR1r, MANGO_REASON_1f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MAC_RX_DROP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR2r, MANGO_REASON_2f, pCntr);
                break;
            case RTL9310_DBG_CNTR_OPENFLOW_IP_MPLS_TTL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR3r, MANGO_REASON_3f, pCntr);
                break;
            case RTL9310_DBG_CNTR_OPENFLOW_TBL_MISS:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR4r, MANGO_REASON_4f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IGR_BW:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR5r, MANGO_REASON_5f, pCntr);
                break;
            case RTL9310_DBG_CNTR_SPECIAL_CONGEST:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR6r, MANGO_REASON_6f, pCntr);
                break;
            case RTL9310_DBG_CNTR_EGR_QUEUE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR7r, MANGO_REASON_7f, pCntr);
                break;
            case RTL9310_DBG_CNTR_RESERVED:
                *pCntr = 0;
                ret = RT_ERR_OK;
                break;
            case RTL9310_DBG_CNTR_EGR_LINK_STATUS:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR9r, MANGO_REASON_9f, pCntr);
                break;
            case RTL9310_DBG_CNTR_STACK_UCAST_NONUCAST_TTL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR10r, MANGO_REASON_10f, pCntr);
                break;
            case RTL9310_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR11r, MANGO_REASON_11f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L2_CRC:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR12r, MANGO_REASON_12f, pCntr);
                break;
            case RTL9310_DBG_CNTR_SRC_PORT_FILTER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR13r, MANGO_REASON_13f, pCntr);
                break;
            case RTL9310_DBG_CNTR_PARSER_PACKET_TOO_LONG:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR14r, MANGO_REASON_14f, pCntr);
                break;
            case RTL9310_DBG_CNTR_PARSER_MALFORM_PACKET:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR15r, MANGO_REASON_15f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MPLS_OVER_2_LBL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR16r, MANGO_REASON_16f, pCntr);
                break;
            case RTL9310_DBG_CNTR_EACL_METER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR17r, MANGO_REASON_17f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IACL_METER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR18r, MANGO_REASON_18f, pCntr);
                break;
            case RTL9310_DBG_CNTR_PROTO_STORM:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR19r, MANGO_REASON_19f, pCntr);
                break;
            case RTL9310_DBG_CNTR_INVALID_CAPWAP_HEADER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR20r, MANGO_REASON_20f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR21r, MANGO_REASON_21f, pCntr);
                break;
            case RTL9310_DBG_CNTR_OAM_PARSER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR22r, MANGO_REASON_22f, pCntr);
                break;
            case RTL9310_DBG_CNTR_UC_MC_RPF:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR23r, MANGO_REASON_23f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP_MAC_BINDING_MATCH_MISMATCH:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR24r, MANGO_REASON_24f, pCntr);
                break;
            case RTL9310_DBG_CNTR_SA_BLOCK:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR25r, MANGO_REASON_25f, pCntr);
                break;
            case RTL9310_DBG_CNTR_TUNNEL_IP_ADDRESS_CHECK:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR26r, MANGO_REASON_26f, pCntr);
                break;
            case RTL9310_DBG_CNTR_EACL_DROP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR27r, MANGO_REASON_27f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IACL_DROP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR28r, MANGO_REASON_28f, pCntr);
                break;
            case RTL9310_DBG_CNTR_ATTACK_PREVENT:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR29r, MANGO_REASON_29f, pCntr);
                break;
            case RTL9310_DBG_CNTR_SYSTEM_PORT_LIMIT_LEARN:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR30r, MANGO_REASON_30f, pCntr);
                break;
            case RTL9310_DBG_CNTR_OAMPDU:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR31r, MANGO_REASON_31f, pCntr);
                break;
            case RTL9310_DBG_CNTR_CCM_RX:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR32r, MANGO_REASON_32f, pCntr);
                break;
            case RTL9310_DBG_CNTR_CFM_UNKNOWN_TYPE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR33r, MANGO_REASON_33f, pCntr);
                break;
            case RTL9310_DBG_CNTR_LBM_LBR_LTM_LTR:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR34r, MANGO_REASON_34f, pCntr);
                break;
            case RTL9310_DBG_CNTR_Y_1731:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR35r, MANGO_REASON_35f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_LIMIT_LEARN:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR36r, MANGO_REASON_36f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR37r, MANGO_REASON_37f, pCntr);
                break;
            case RTL9310_DBG_CNTR_CFI_1:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR38r, MANGO_REASON_38f, pCntr);
                break;
            case RTL9310_DBG_CNTR_STATIC_DYNAMIC_PORT_MOVING:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR39r, MANGO_REASON_39f, pCntr);
                break;
            case RTL9310_DBG_CNTR_PORT_MOVE_FORBID:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR40r, MANGO_REASON_40f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L3_CRC:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR41r, MANGO_REASON_41f, pCntr);
                break;
            case RTL9310_DBG_CNTR_BPDU_PTP_LLDP_EAPOL_RMA:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR42r, MANGO_REASON_42f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR43r, MANGO_REASON_43f, pCntr);
                break;
            case RTL9310_DBG_CNTR_INVALID_SA:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR44r, MANGO_REASON_44f, pCntr);
                break;
            case RTL9310_DBG_CNTR_NEW_SA:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR45r, MANGO_REASON_45f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_IGR_FILTER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR46r, MANGO_REASON_46f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IGR_VLAN_CONVERT:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR47r, MANGO_REASON_47f, pCntr);
                break;
            case RTL9310_DBG_CNTR_GRATUITOUS_ARP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR48r, MANGO_REASON_48f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MSTP_SRC_DROP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR49r, MANGO_REASON_49f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L2_HASH_FULL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR50r, MANGO_REASON_50f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MPLS_UNKNOWN_LBL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR51r, MANGO_REASON_51f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L3_IPUC_NON_IP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR52r, MANGO_REASON_52f, pCntr);
                break;
            case RTL9310_DBG_CNTR_TTL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR53r, MANGO_REASON_53f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MTU:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR54r, MANGO_REASON_54f, pCntr);
                break;
            case RTL9310_DBG_CNTR_ICMP_REDIRECT:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR55r, MANGO_REASON_55f, pCntr);
                break;
            case RTL9310_DBG_CNTR_STORM_CONTROL:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR56r, MANGO_REASON_56f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L3_DIP_DMAC_MISMATCH:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR57r, MANGO_REASON_57f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP4_IP_OPTION:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR58r, MANGO_REASON_58f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP6_HBH_EXT_HEADER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR59r, MANGO_REASON_59f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP4_IP6_HEADER_ERROR:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR60r, MANGO_REASON_60f, pCntr);
                break;
            case RTL9310_DBG_CNTR_ROUTING_IP_ADDR_CHECK:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR61r, MANGO_REASON_61f, pCntr);
                break;
            case RTL9310_DBG_CNTR_ROUTING_EXCEPTION:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR62r, MANGO_REASON_62f, pCntr);
                break;
            case RTL9310_DBG_CNTR_DA_BLOCK:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR63r, MANGO_REASON_63f, pCntr);
                break;
            case RTL9310_DBG_CNTR_OAM_MUX:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR64r, MANGO_REASON_64f, pCntr);
                break;
            case RTL9310_DBG_CNTR_PORT_ISOLATION:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR65r, MANGO_REASON_65f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_EGR_FILTER:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR66r, MANGO_REASON_66f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MIRROR_ISOLATE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR67r, MANGO_REASON_67f, pCntr);
                break;
            case RTL9310_DBG_CNTR_MSTP_DESTINATION_DROP:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR68r, MANGO_REASON_68f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L2_MC_BRIDGE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR69r, MANGO_REASON_69f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR70r, MANGO_REASON_70f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L2_UC:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR71r, MANGO_REASON_71f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L2_MC:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR72r, MANGO_REASON_72f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP4_MC:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR73r, MANGO_REASON_73f, pCntr);
                break;
            case RTL9310_DBG_CNTR_IP6_MC:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR74r, MANGO_REASON_74f, pCntr);
                break;
            case RTL9310_DBG_CNTR_L3_UC_MC_ROUTE:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR75r, MANGO_REASON_75f, pCntr);
                break;
            case RTL9310_DBG_CNTR_UNKNOWN_L2_UC_FLPM:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR76r, MANGO_REASON_76f, pCntr);
                break;
            case RTL9310_DBG_CNTR_BC_FLPM:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR77r, MANGO_REASON_77f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_L2_MC_FLPM:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR78r, MANGO_REASON_78f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_PRO_UNKNOWN_IP4_MC_FLPM:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR79r, MANGO_REASON_79f, pCntr);
                break;
            case RTL9310_DBG_CNTR_VLAN_PROFILE_UNKNOWN_IP6_MC_FLPM:
                ret = reg_field_read(unit, MANGO_STAT_PRVTE_DBG_CNTR80r, MANGO_REASON_80f, pCntr);
                break;
            default:
                break;
        }
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        switch (type)
        {
            case RTL9300_DBG_CNTR_OAM_PARSER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER0r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_UC_RPF:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER1r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_DEI_CFI:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER2r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MAC_IP_SUBNET_BASED_VLAN:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER3r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_VLAN_IGR_FILTER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER4r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L2_UC_MC_BRIDGE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER5r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP4_IP6_MC_BRIDGE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER6r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_PTP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER7r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_USER_DEF_0_3:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER8r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_RESERVED:
                *pCntr = 0;
                ret = RT_ERR_OK;
                break;
            case RTL9300_DBG_CNTR_RESERVED1:
                *pCntr = 0;
                ret = RT_ERR_OK;
                break;
            case RTL9300_DBG_CNTR_RESERVED2:
                *pCntr = 0;
                ret = RT_ERR_OK;
                break;
            case RTL9300_DBG_CNTR_BPDU_RMA:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER12r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_LACP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER13r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_LLDP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER14r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_EAPOL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER15r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_XX_RMA:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER16r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_IPUC_NON_IP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER17r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP4_IP6_HEADER_ERROR:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER18r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_BAD_IP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER19r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_DIP_DMAC_MISMATCH:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER20r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP4_IP_OPTION:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER21r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP_UC_MC_ROUTING_LOOK_UP_MISS:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER22r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_DST_NULL_INTF:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER23r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_PBR_NULL_INTF:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER24r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_HOST_NULL_INTF:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER25r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ROUTE_NULL_INTF:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER26r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_BRIDGING_ACTION:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER27r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ROUTING_ACTION:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER28r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IPMC_RPF:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER29r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L2_NEXTHOP_AGE_OUT:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER30r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_UC_TTL_FAIL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER31r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_MC_TTL_FAIL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER32r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_UC_MTU_FAIL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER33r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_MC_MTU_FAIL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER34r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_UC_ICMP_REDIR:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER35r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP6_MLD_OTHER_ACT:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER36r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ND:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER37r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP_MC_RESERVED:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER38r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP6_HBH:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER39r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_INVALID_SA:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER40r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L2_HASH_FULL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER41r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_NEW_SA:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER42r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_PORT_MOVE_FORBID:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER43r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_STATIC_PORT_MOVING:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER44r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_DYNMIC_PORT_MOVING:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER45r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_CRC:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER46r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MAC_LIMIT:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER47r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ATTACK_PREVENT:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER48r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ACL_FWD_ACTION:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER49r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_OAMPDU:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER50r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_OAM_MUX:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER51r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_TRUNK_FILTER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER52r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ACL_DROP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER53r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IGR_BW:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER54r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ACL_METER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER55r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_VLAN_ACCEPT_FRAME_TYPE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER56r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MSTP_SRC_DROP_DISABLED_BLOCKING:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER57r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_SA_BLOCK:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER58r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_DA_BLOCK:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER59r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_STORM_CONTROL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER60r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_VLAN_EGR_FILTER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER61r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MSTP_DESTINATION_DROP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER62r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_SRC_PORT_FILTER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER63r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_PORT_ISOLATION:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER64r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_TX_MAX_FRAME_SIZE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER65r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_EGR_LINK_STATUS:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER66r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MAC_TX_DISABLE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER67r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MAC_PAUSE_FRAME:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER68r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MAC_RX_DROP:                     /*LLC check failure, 48pass1,dscrunout/l2-CRC/oversize/undersize/symbol-error*/
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER69r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MIRROR_ISOLATE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER70r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_RX_FC:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER71r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_EGR_QUEUE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER72r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_HSM_RUNOUT:            /*Replication*/
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER73r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ROUTING_DISABLE:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER74r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_INVALID_L2_NEXTHOP_ENTRY:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER75r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_L3_MC_SRC_FLT:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER76r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_CPUTAG_FLT:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER77r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_FWD_PMSK_NULL:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER78r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IPUC_ROUTING_LOOKUP_MISS:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER79r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_MY_DEV_DROP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER80r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_STACK_NONUC_BLOCKING_PMSK:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER81r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_STACK_PORT_NOT_FOUND:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER82r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_ACL_LOOPBACK_DROP:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER83r, LONGAN_REASONf, pCntr);
                break;
            case RTL9300_DBG_CNTR_IP6_ROUTING_EXT_HEADER:
                ret = reg_field_read(unit, LONGAN_STAT_PRVTE_DROP_COUNTER84r, LONGAN_REASONf, pCntr);
                break;
            default:
                break;
        }
    }
    else
#endif
    {

        return ret;
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getDbgEncapCntr
 * Description:
 *      Get encap debug counter of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      pEncapCntr  - value of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getDbgEncapCntr(uint32 unit, rtk_dbg_encap_cntr_t *pEncapCntr)
{
#if defined(CONFIG_SDK_RTL9310)
    uint32 value = 0;
    int32 ret = RT_ERR_FAILED;
#endif
    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        ret = reg_read(unit, MANGO_STAT_PRVTE_DBG_ENCAP_CNTRr, &value);
        pEncapCntr->encapOpenFlow = (value >> 24) & 0xFF;
        pEncapCntr->encapEACL     = (value >> 16) & 0xFF;
        pEncapCntr->encapEVC      = (value >> 8) & 0xFF;
        pEncapCntr->encapTTL      = (value) & 0xFF;

        return ret;
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlIgrPortUsedPageCnt
 * Description:
 *      Get flow control ingress used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 * pCntr, uint32 * pMaxCntr)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
    int32 ret = RT_ERR_FAILED;
    uint32 value = 0;
#endif

    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        ret = reg_array_read(unit, CYPRESS_FC_P_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16);

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
            ret = reg_array_read(unit, MAPLE_FC_P_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);
            if(ret != RT_ERR_OK)
                return ret;
            *pCntr = (value & RTL838x_PAGE_CNT_MASK);


            ret = reg_array_read(unit, MAPLE_FC_P_PAGE_PEAKCNTr, port, REG_ARRAY_INDEX_NONE, &value);
            if(ret != RT_ERR_OK)
                return ret;
            *pMaxCntr = (value & RTL838x_PAGE_CNT_MASK);

            return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        ret = reg_array_read(unit, MANGO_FC_PORT_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16) & 0xFFFF;

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        ret = reg_array_read(unit, LONGAN_FC_PORT_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);
        *pCntr = (value & 0xFFFF);

        if(ret != RT_ERR_OK)
            return ret;

        ret = reg_array_read(unit, LONGAN_FC_PORT_PEAK_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);
        *pMaxCntr = (value & 0xFFFF);

        return ret;
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlEgrPortUsedPageCnt
 * Description:
 *      Get flow control egress used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
    int32 ret = RT_ERR_FAILED;
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
    out_q_entry_t outQ;
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    uint32 value = 0;
#endif

    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        osal_memset(&outQ, 0, sizeof(outQ));

        if ((ret = table_read(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;
        if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_PE_USED_PAGE_CNTtf,
                    pCntr, (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;
        if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_PE_MAX_USED_PAGE_CNTtf,
                    pMaxCntr, (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        ret = reg_array_read(unit, MAPLE_FC_P_EGR_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);
        *pCntr = (value & RTL838x_PAGE_CNT_MASK);
        *pMaxCntr = (value >> 16) & RTL838x_PAGE_CNT_MASK;

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if (HWP_IS_CPU_PORT(unit, port))
        {
            ret = reg_read(unit, MANGO_FC_CPU_EGR_PAGE_CNTr, &value);

            *pCntr = (value & 0xFFFF);
            *pMaxCntr = (value >> 16) & 0xFFFF;
        }
        else
        {
            osal_memset(&outQ, 0, sizeof(outQ));

            if ((ret = table_read(unit, MANGO_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
                return ret;

            if ((ret = table_field_get(unit, MANGO_OUT_Qt, MANGO_OUT_Q_PORT_USED_PAGE_CNTtf,
                        pCntr, (uint32 *) &outQ)) != RT_ERR_OK)
                return ret;
            if ((ret = table_field_get(unit, MANGO_OUT_Qt, MANGO_OUT_Q_PORT_MAX_USED_PAGE_CNTtf,
                        pMaxCntr, (uint32 *) &outQ)) != RT_ERR_OK)
                return ret;
        }

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        ret = reg_array_read(unit, LONGAN_FC_PORT_EGR_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value);

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16) & 0xFFFF;

        return ret;
    }
    else
#endif
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlSystemUsedPageCnt
 * Description:
 *      Get flow control system used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlSystemUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310)  || defined(CONFIG_SDK_RTL9300)
    int32 ret = RT_ERR_FAILED;
    uint32 value = 0;
#endif

    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        ret = reg_read(unit, CYPRESS_FC_TL_USED_PAGE_CNTr, &value);

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16);

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
        ret = reg_read(unit, MAPLE_FC_GLB_PAGE_CNTr, &value);
        if(ret != RT_ERR_OK)
            return ret;
        *pCntr = (value & RTL838x_PAGE_CNT_MASK);


        ret = reg_read(unit, MAPLE_FC_GLB_PAGE_PEAKCNTr, &value);
        if(ret != RT_ERR_OK)
            return ret;
        *pMaxCntr = (value & RTL838x_PAGE_CNT_MASK);

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        ret = reg_read(unit, MANGO_FC_GLB_PAGE_CNTr, &value);

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16) & 0xFFFF;

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        ret = reg_read(unit, LONGAN_FC_GLB_PAGE_CNTr, &value);
        *pCntr = (value & 0xFFFF);
        if(ret != RT_ERR_OK)
            return ret;

        ret = reg_read(unit, LONGAN_FC_GLB_PAGE_PEAKCNTr, &value);
        *pMaxCntr = (value & 0xFFFF);

        return ret;
    }
    else
#endif

    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlSystemPublicUsedPageCnt
 * Description:
 *      Get flow control system public used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlSystemPublicUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value = 0;
    hal_control_t *pInfo;
#if defined(CONFIG_SDK_RTL9300)
    rtk_port_t  port;
    uint32  idx;
    uint32  pub_cnt;
    uint32  port_page_cnt[29];
    rtk_flowctrl_thresh_t thresh[4];
#endif

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if ((ret = reg_read(unit, MANGO_FC_PB_PAGE_CNTr, &value)) != RT_ERR_OK)
        {
            return ret;
        }

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16) & 0xFFFF;
    }
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        for (idx = 0; idx <= HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit); idx++)
        {
            RT_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, idx, &thresh[idx]), ret);
        }

        HWP_PORT_TRAVS(unit, port)
        {
            RT_ERR_CHK(reg_array_field_read(unit, LONGAN_FC_PORT_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, LONGAN_P_PAGE_CNTf, &port_page_cnt[port]), ret);
        }
        RT_ERR_CHK(reg_field_read(unit, LONGAN_ETE_FC_L2NTFY_PORT_PAGE_CNTr, LONGAN_P_PAGE_CNTf, &value), ret);

        pub_cnt = 0;
        HWP_PORT_TRAVS(unit, port)
        {
            RT_ERR_CHK(reg_array_field_read(unit, LONGAN_FC_PORT_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE, LONGAN_IDXf, &idx), ret);
            pub_cnt += ((port_page_cnt[port] > thresh[idx].guar) ? (port_page_cnt[port] - thresh[idx].guar) : 0);
        }
        *pCntr = pub_cnt + value;
        *pMaxCntr = 0;
    }
#endif
    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlSystemPublicResource_get
 * Description:
 *      Get flow control system total public resource of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of total public resource
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlSystemPublicResource_get(uint32 unit, uint32 *pCntr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 guarCnt;
    uint32 grp_idx, idx;
    rtk_port_t port;
#if defined(CONFIG_SDK_RTL9310)
    rtk_port_t dummy_port;
#endif
    rtk_flowctrl_thresh_t thresh[4];
    rtk_switch_devInfo_t devInfo;
    uint32  total_page_cnt;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    RT_ERR_CHK(rtk_switch_deviceInfo_get(unit, &devInfo), ret);

    for (idx = 0; idx < 4; idx++)
    {
        RT_ERR_CHK(rtk_flowctrl_igrPauseThreshGroup_get(unit, idx, &thresh[idx]), ret);
    }

    guarCnt = 0;
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        for (port = 0; port <= devInfo.cpuPort; port++)
        {
            RT_ERR_CHK(reg_array_field_read(unit, MANGO_FC_PORT_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE, MANGO_IDXf, &grp_idx), ret);
            guarCnt += thresh[grp_idx].guar;
        }
        for (dummy_port = 0; dummy_port < 2; dummy_port++)
        {
            RT_ERR_CHK(reg_array_field_read(unit, MANGO_FC_DUMMY_PORT_THR_SET_SELr, REG_ARRAY_INDEX_NONE, dummy_port, MANGO_IDXf, &grp_idx), ret);
            guarCnt += thresh[grp_idx].guar;
        }
        total_page_cnt = 5840;
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        for (port = 0; port <= devInfo.cpuPort; port++)
        {
            RT_ERR_CHK(reg_array_field_read(unit, LONGAN_FC_PORT_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE, LONGAN_IDXf, &grp_idx), ret);
            guarCnt += thresh[grp_idx].guar;
        }
        RT_ERR_CHK(reg_field_read(unit, LONGAN_FC_GLB_DROP_THRr, LONGAN_DROP_ALLf, &total_page_cnt), ret);
    }
#endif

    *pCntr = (guarCnt < total_page_cnt) ? (total_page_cnt - guarCnt) : 0 ;

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getFlowCtrlSystemIgrQueueUsedPageCnt
 * Description:
 *      Get flow control system ingress queue used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pCntr    - value of used page count
 *      pMaxCntr - value of maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlSystemIgrQueueUsedPageCnt(uint32 unit, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr)
{
    int32 ret = RT_ERR_FAILED;

    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    /* Admit queue */
    if ((ret = reg_field_read(unit, MANGO_IGBW_ADMIT_Q_PAGE_CNTr, MANGO_CNTf, &pQCntr->cntr[0])) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_read(unit, MANGO_IGBW_ADMIT_Q_PAGE_CNTr, MANGO_MAX_CNTf, &pQMaxCntr->cntr[0])) != RT_ERR_OK)
    {
        return ret;
    }

    /* Drop queue */
    if ((ret = reg_field_read(unit, MANGO_IGBW_DROP_Q_PAGE_CNTr, MANGO_CNTf, &pQCntr->cntr[1])) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = reg_field_read(unit, MANGO_IGBW_DROP_Q_PAGE_CNTr, MANGO_MAX_CNTf, &pQMaxCntr->cntr[1])) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getFlowCtrlPortIgrQueueUsedPageCnt
 * Description:
 *      Get flow control ingress queue used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pQCntr    - value of queue used page count
 *      pQMaxCntr - value of queue maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlPortIgrQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_qid_t qid;
    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    if ((ret = reg_field_write(unit, MANGO_IGBW_PAGE_CNT_CTRLr, MANGO_PORT_IDXf, &port)) != RT_ERR_OK)
    {
        return ret;
    }

    for (qid = 0; qid < HAL_MAX_NUM_OF_IGR_QUEUE(unit); qid++)
    {
        if ((ret = reg_array_field_read(unit, MANGO_IGBW_Q_PAGE_CNTr, REG_ARRAY_INDEX_NONE, qid,
                    MANGO_CNTf, &pQCntr->cntr[qid])) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_array_field_read(unit, MANGO_IGBW_Q_PAGE_CNTr, REG_ARRAY_INDEX_NONE, qid,
                    MANGO_MAX_CNTf, &pQMaxCntr->cntr[qid])) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
/* Function Name:
 *      hal_getFlowCtrlPortQueueUsedPageCnt
 * Description:
 *      Get flow control egress queue used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pQCntr    - value of queue used page count
 *      pQMaxCntr - value of queue maximum used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlPortQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr)
{
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    int32 ret = RT_ERR_FAILED;
    rtk_qid_t qid;
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
    out_q_entry_t outQ;
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    uint32 value = 0;
#endif

    hal_control_t *pInfo;

    if ((pInfo = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }


#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit)   ||
        HWP_8390_FAMILY_ID(unit))
    {
        osal_memset(&outQ, 0, sizeof(outQ));

        if ((ret = table_read(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;

        for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
        {
            if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, (uint32)outQ_usedPage_fieldidx[qid],
                        &pQCntr->cntr[qid], (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;
            if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, (uint32)outQ_maxUsedPage_fieldidx[qid],
                        &pQMaxCntr->cntr[qid], (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;
        }

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8330_FAMILY_ID(unit) ||
        HWP_8380_FAMILY_ID(unit))
    {
            for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
            {
                ret = reg_array_read(unit, MAPLE_FC_PQ_EGR_PAGE_CNT0r+port/8, port, qid, &value);
                if(ret != RT_ERR_OK)
                    return ret;

                pQCntr->cntr[qid] = value & RTL838x_PAGE_CNT_MASK;
                pQMaxCntr->cntr[qid] = (value>>16) & RTL838x_PAGE_CNT_MASK;
            }
            return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if (HWP_IS_CPU_PORT(unit, port))
        {
            for (qid = 0; qid <= (HAL_MAX_NUM_OF_CPU_QUEUE(unit) - 1); qid++)
            {
                if ((ret = reg_array_read(unit, MANGO_FC_CPU_Q_EGR_PAGE_CNTr, REG_ARRAY_INDEX_NONE,
                            qid, &value)) != RT_ERR_OK)
                {
                    return ret;
                }

                pQCntr->cntr[qid] = (value & 0xFFFF);
                pQMaxCntr->cntr[qid] = (value >> 16) & 0xFFFF;
            }
        }
        else
        {
            osal_memset(&outQ, 0, sizeof(outQ));

            if ((ret = table_read(unit, MANGO_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
                return ret;

            for (qid = 0; qid < HAL_MAX_NUM_OF_QUEUE(unit); qid++)
            {
                if ((ret = table_field_get(unit, MANGO_OUT_Qt, (uint32)outQ_used_page_fieldidx[qid],
                            &pQCntr->cntr[qid], (uint32 *) &outQ)) != RT_ERR_OK)
                {
                    return ret;
                }
                if ((ret = table_field_get(unit, MANGO_OUT_Qt, (uint32)outQ_max_used_page_fieldidx[qid],
                            &pQMaxCntr->cntr[qid], (uint32 *) &outQ)) != RT_ERR_OK)
                {
                    return ret;
                }
            }
        }

        return ret;
    }
    else
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        if (HWP_IS_CPU_PORT(unit, port))
        {
            for (qid = 0; qid <= (HAL_MAX_NUM_OF_CPU_QUEUE(unit) - 1); qid++)
            {
                if ((ret = reg_array_read(unit, LONGAN_FC_CPU_Q_EGR_PAGE_CNTr, REG_ARRAY_INDEX_NONE, qid, &value)) != RT_ERR_OK)
                {
                    return ret;
                }

                pQCntr->cntr[qid] = (value & 0xFFFF);
                pQMaxCntr->cntr[qid] = (value >> 16) & 0xFFFF;
            }
        }
        else if(HWP_UPLINK_PORT(unit, port))
        {
            for (qid = 0; qid <= (HAL_MAX_NUM_OF_STACK_QUEUE(unit) - 1); qid++)
            {
                if ((ret = reg_array_read(unit, LONGAN_FC_PORT_Q_EGR_PAGE_CNT_SET1r, port, qid, &value)) != RT_ERR_OK)
                {
                    return ret;
                }

                pQCntr->cntr[qid] = (value & 0xFFFF);
                pQMaxCntr->cntr[qid] = (value >> 16) & 0xFFFF;
            }
        }
        else
        {
            for (qid = 0; qid <= (HAL_MAX_NUM_OF_QUEUE(unit) - 1); qid++)
            {
                if ((ret = reg_array_read(unit, LONGAN_FC_PORT_Q_EGR_PAGE_CNT_SET0r, port, qid, &value)) != RT_ERR_OK)
                {
                    return ret;
                }

                pQCntr->cntr[qid] = (value & 0xFFFF);
                pQMaxCntr->cntr[qid] = (value >> 16) & 0xFFFF;
            }
        }

        return ret;
    }
    else
#endif

    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlIgrPortUsedPageCnt
 * Description:
 *      Reset flow control ingress used page count of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_resetFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port)
{
    int32 ret = RT_ERR_OK;
    uint32 value = 1;

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        if ((ret = reg_array_write(unit, CYPRESS_FC_P_USED_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, &value)) != RT_ERR_OK)
            return ret;
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_PAGE_CNTr, port, REG_ARRAY_INDEX_NONE, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;

        /* Ingress queue used page count */
        if ((ret = reg_field_write(unit, MANGO_IGBW_PAGE_CNT_CTRLr, MANGO_PORT_IDXf, &port)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_IGBW_PAGE_CNT_CTRLr, MANGO_PORT_RSTf, &value)) != RT_ERR_OK)
            return ret;
    }
#endif

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlEgrPortUsedPageCnt
 * Description:
 *      Reset flow control egress used page count including port and queue of the specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_resetFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value = 1;
    out_q_entry_t outQ;
#if defined(CONFIG_SDK_RTL9310)
    rtk_qid_t qid;
#endif

    osal_memset(&outQ, 0, sizeof(outQ));

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        if ((ret = table_read(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;
        ret = table_field_set(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_PE_USED_PAGE_CNTtf,
                    &value, (uint32 *) &outQ);
        if ((ret = table_write(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
            return ret;
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if (HWP_IS_CPU_PORT(unit, port))
        {
            if ((ret = reg_field_write(unit, MANGO_FC_CPU_EGR_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            {
                return ret;
            }
            for (qid = 0; qid <= (HAL_MAX_NUM_OF_CPU_QUEUE(unit) - 1); qid++)
            {
                if ((ret = reg_array_field_write(unit, MANGO_FC_CPU_Q_EGR_PAGE_CNTr, REG_ARRAY_INDEX_NONE,
                            qid, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
                {
                    return ret;
                }
            }
        }
        else
        {
            if ((ret = table_read(unit, MANGO_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
                return ret;
            ret = table_field_set(unit, MANGO_OUT_Qt, MANGO_OUT_Q_PORT_MAX_USED_PAGE_CNTtf,
                        &value, (uint32 *) &outQ);
            if ((ret = table_write(unit, MANGO_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
                return ret;
        }
    }

#endif

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlSystemUsedPageCnt
 * Description:
 *      Reset flow control system used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_resetFlowCtrlSystemUsedPageCnt(uint32 unit)
{
    int32 ret = RT_ERR_OK;
    uint32 value = 1;

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8390_50_FAMILY(unit))
    {
        if ((ret = reg_write(unit, CYPRESS_FC_TL_USED_PAGE_CNTr, &value)) != RT_ERR_OK)
            return ret;
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        if ((ret = reg_field_write(unit, MANGO_FC_GLB_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_FC_PB_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_IGBW_PAGE_CNT_CTRLr, MANGO_RSTf, &value)) != RT_ERR_OK)
            return ret;
    }
#endif

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_resetFlowCtrlRepctQueueUsedPageCnt
 * Description:
 *      Reset flow control replication queue used page count of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_resetFlowCtrlRepctQueueUsedPageCnt(uint32 unit)
{
    int32 ret = RT_ERR_OK;
    uint32 value = 0xffff;

    if (RTL9310_FAMILY_ID == HWP_CHIP_FAMILY_ID(unit))
    {
        if ((ret = reg_field_write(unit, MANGO_FC_ALE_NON_REPCT_Q_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_FC_ALE_REPCT_Q_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_FC_REPCT_FCON_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_FC_REPCT_FCOFF_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_FC_REPCT_FCOFF_DROP_CNTr, MANGO_CNTf, &value)) != RT_ERR_OK)
            return ret;
        if ((ret = reg_field_write(unit, MANGO_FC_HSA_PAGE_CNTr, MANGO_MAX_CNTf, &value)) != RT_ERR_OK)
            return ret;
    }

    return ret;
}
#endif

#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL9300)|| defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getWatchdogCnt
 * Description:
 *      Display phyWatchdog, serdesWatchdog and PktbufWatchdog.
 * Input:
 *      unit     - unit id
 *      type    - Which Watchdog value
 * Output:
 *      count   - counter's value
 * Return:
 *      RT_ERR_OK
 */
int32 hal_getWatchdogCnt(uint32 unit, uint32 type, uint32 * count)

{
    int32 ret = RT_ERR_OK;

    switch(type)
    {
        case 0:
            *count = pktBuf_watchdog_cnt[unit];
            break;
        case 1:
            *count = macSerdes_watchdog_cnt[unit];
            break;
        case 2:
            *count = phy_watchdog_cnt[unit];
            break;
         case 3:
            *count = fiber_rx_watchdog_cnt[unit];
            break;
         case 4:
            *count = phySerdes_watchdog_cnt[unit];
            break;
         case 5:
            *count = phy95rClkRecov_watchdog_cnt[unit];
            break;
         case 6:
            *count = phy95rClkRecovOK_watchdog_cnt[unit];
            break;
         case 7:
            *count = drainTx_watchdog_cnt[unit];
            break;
         case 8:
            *count = drainTxPathHasPkt_watchdog_cnt[unit];
            break;
         case 9:
            *count = drainTxQFailed_watchdog_cnt[unit];
            break;
         case 10:
            *count = syncPhyToMacFailed_watchdog_cnt[unit];
            break;
        default:
            osal_printf("\nUnknown Watchdog type(%u)!!!\n", type);
    }

    return ret;
}

/* Function Name:
 *      hal_setWatchdogMonitorEnable
 * Description:
 *      Eanble/Disable watchdog monitor
 * Input:
 *      enable - enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 */
int32 hal_setWatchdogMonitorEnable(uint32 enable)
{
    waMon_enable     = enable;
    osal_printf("\nWatchdog Monitor %s\n", (enable==DISABLED)?"off":"on");
    return RT_ERR_OK;
}


#endif

#if defined(CONFIG_SDK_RTL9310)
/* Function Name:
 *      hal_getFlowCtrlRepctQueueCntrInfo
 * Description:
 *      Get flow control replication queue used page count and relative counter info of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pRepctCntr    - the pointer of counter values
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_getFlowCtrlRepctQueueCntrInfo(uint32 unit, rtk_dbg_repctQ_CntrInfo_t *pRepctCntr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value = 0;

    if ((ret = reg_read(unit, MANGO_FC_ALE_NON_REPCT_Q_PAGE_CNTr, &value)) != RT_ERR_OK)
        return ret;
    pRepctCntr->nonRepctCntr = (value & 0xFFFF);
    pRepctCntr->nonRepctCntrMax = (value >> 16);

    if ((ret = reg_read(unit, MANGO_FC_ALE_REPCT_Q_PAGE_CNTr, &value)) != RT_ERR_OK)
        return ret;
    pRepctCntr->repctCntr = (value & 0xFFFF);
    pRepctCntr->repctCntrMax = (value >> 16);

    if ((ret = reg_read(unit, MANGO_FC_REPCT_FCON_PAGE_CNTr, &value)) != RT_ERR_OK)
        return ret;
    pRepctCntr->repctFCOnCntr = (value & 0xFFFF);
    pRepctCntr->repctFCOnCntrMax = (value >> 16);

    if ((ret = reg_read(unit, MANGO_FC_REPCT_FCOFF_PAGE_CNTr, &value)) != RT_ERR_OK)
        return ret;
    pRepctCntr->repctFCOffCntr = (value & 0xFFFF);
    pRepctCntr->repctFCOffCntrMax = (value >> 16);

    if ((ret = reg_read(unit, MANGO_FC_REPCT_FCOFF_DROP_CNTr, &value)) != RT_ERR_OK)
        return ret;
    pRepctCntr->repctFCOffDropPktCntr = (value & 0xFFFF);

    if ((ret = reg_read(unit, MANGO_FC_HSA_PAGE_CNTr, &value)) != RT_ERR_OK)
        return ret;
    pRepctCntr->extraHsaPktCntr = (value & 0xFFFF);
    pRepctCntr->extraHsaPktCntrMax = (value >> 16);

    return RT_ERR_OK;
}

/* Function Name:
 *      hal_repctQueueEmptyStatus_get
 * Description:
 *      Get ALE replication queue status.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable - pointer to empty status of replication queue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_repctQueueEmptyStatus_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 value;

    if ((ret = reg_field_read(unit, MANGO_HSA_DBG_CTRLr, MANGO_REP_QUEUE_EMPTYf, &value)) != RT_ERR_OK)
        return ret;

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
}

/* Function Name:
 *      hal_repctQueueStickEnable_get
 * Description:
 *      Get ALE replication queue stuck function status.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable - pointer to enable status of replication queue stuck function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_repctQueueStickEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 value;

    if ((ret = reg_field_read(unit, MANGO_HSA_DBG_CTRLr, MANGO_REP_QUEUE_STICKf, &value)) != RT_ERR_OK)
        return ret;

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
}

/* Function Name:
 *      hal_repctQueueStickEnable_set
 * Description:
 *      Set ALE replication queue stuck function status.
 * Input:
 *      unit     - unit id
 *      enable - status of replication queue stuck function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_repctQueueStickEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    uint32 value;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    if ((ret = reg_field_write(unit, MANGO_HSA_DBG_CTRLr, MANGO_REP_QUEUE_STICKf, &value)) != RT_ERR_OK)
        return ret;

    return ret;
}

/* Function Name:
 *      hal_repctQueueStickEnable_set
 * Description:
 *      Get ALE replication queue fetch function status.
 * Input:
 *      unit      - unit id
 * Output:
 *      pEnable - pointer to enable status of replication queue fetch function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_repctQueueFetchEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_OK;
    uint32 value;

    if ((ret = reg_field_read(unit, MANGO_HSA_DBG_CTRLr, MANGO_REP_QUEUE_FETCHf, &value)) != RT_ERR_OK)
        return ret;

    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
}

/* Function Name:
 *      hal_repctQueueStickEnable_set
 * Description:
 *      Set ALE replication queue fetch function status.
 * Input:
 *      unit     - unit id
 *      enable - status of replication queue fetch function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 */
int32 hal_repctQueueFetchEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret = RT_ERR_OK;
    uint32 value;

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    if ((ret = reg_field_write(unit, MANGO_HSA_DBG_CTRLr, MANGO_REP_QUEUE_FETCHf, &value)) != RT_ERR_OK)
        return ret;

    return ret;
}
#endif

/* Function Name:
 *      mac_debug_sds_rxCali
 * Description:
 *      execute SerDes rx calibration.
 * Input:
 *      unit          - unit id
 *      sds          - SerDes id
 *      retryCnt   - retry count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
mac_debug_sds_rxCali(uint32 unit, uint32 sds, uint32 retryCnt)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    int32   ret;
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        RT_ERR_CHK(dal_longan_sds_rxCali(unit, sds, retryCnt), ret);
    }
#endif  /* CONFIG_SDK_RTL9300 */
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        RT_ERR_CHK(dal_mango_sds_rxCali(unit, sds, retryCnt), ret);
    }
#endif  /* CONFIG_SDK_RTL9310 */

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCali */

/* Function Name:
 *      mac_debug_sds_rxCaliEnable_set
 * Description:
 *      Enable the SerDes rx calibration
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 *      enable      - rx calibration enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
mac_debug_sds_rxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    int32   ret;
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        RT_ERR_CHK(dal_longan_sds_rxCaliEnable_set(unit, sds, enable), ret);
    }
#endif  /* CONFIG_SDK_RTL9300 */
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        RT_ERR_CHK(dal_mango_sds_rxCaliEnable_set(unit, sds, enable), ret);
    }
#endif  /* CONFIG_SDK_RTL9310 */

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCaliEnable_set */

/* Function Name:
 *      mac_debug_sds_rxCaliEnable_get
 * Description:
 *      Get the SerDes rx calibration enable status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pEnable      - pointer to x calibration enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
mac_debug_sds_rxCaliEnable_get(uint32 unit, uint32 sds, uint32 *pEnable)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    int32   ret;
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        RT_ERR_CHK(dal_longan_sds_rxCaliEnable_get(unit, sds, pEnable), ret);
    }
#endif  /* CONFIG_SDK_RTL9300 */
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        RT_ERR_CHK(dal_mango_sds_rxCaliEnable_get(unit, sds, pEnable), ret);
    }
#endif  /* CONFIG_SDK_RTL9310 */

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCaliEnable_get */

/* Function Name:
 *      mac_debug_sds_rxCaliStatus_get
 * Description:
 *      Get the SerDes rx calibration status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      pStatus      - pointer to  status of rx calibration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
mac_debug_sds_rxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus)
{

#if defined(CONFIG_SDK_RTL9300)
    if (HWP_9300_FAMILY_ID(unit))
    {
        dal_longan_sds_rxCaliStatus_get(unit, sds, pStatus);
    }
#endif  /* CONFIG_SDK_RTL9300 */
#if defined(CONFIG_SDK_RTL9310)
    if (HWP_9310_FAMILY_ID(unit))
    {
        dal_mango_sds_rxCaliStatus_get(unit, sds, pStatus);
    }
#endif  /* CONFIG_SDK_RTL9310 */

    return RT_ERR_OK;
}   /* end of mac_debug_sds_rxCaliEnable_get */

/* Function Name:
 *      mac_debug_sds_rxCali_debugEnable_set
 * Description:
 *      Enable debug msg for SerDes rx calibration
 * Input:
 *      enable      - enable print debug msg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
mac_debug_sds_rxCali_debugEnable_set(rtk_enable_t enable)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    int32   ret;
#endif
#if defined(CONFIG_SDK_RTL9300)
    RT_ERR_CHK(dal_longan_sds_rxCali_debugEnable_set(enable), ret);
#endif  /* CONFIG_SDK_RTL9300 */
#if defined(CONFIG_SDK_RTL9310)
    RT_ERR_CHK(dal_mango_sds_rxCaliDbgEnable_set(enable), ret);
#endif  /* CONFIG_SDK_RTL9310 */

    return RT_ERR_OK;
}


