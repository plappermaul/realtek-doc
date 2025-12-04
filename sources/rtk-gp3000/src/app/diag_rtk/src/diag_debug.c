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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those debug command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) debug
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <ioal/io_mii.h>
#include <hal/common/halctrl.h>
#include <diag_str.h>
#include <rtk/acl.h>
#include <rtk/debug.h>
#include <rtk/gpio.h>
#include <rtk/ponmac.h>
#include <rtk/mdio.h>
#include <rtk/i2c.h>
#include <netinet/in.h>
#include <hal/chipdef/swdef.h>
#include <apollo_reg_struct.h>
#include <hal/mac/drv.h>
#ifdef CONFIG_SDK_APOLLO
#include <dal/apollo/raw/apollo_raw_hwmisc.h>
#include <dal/apollo/raw/apollo_raw_qos.h>
#include <dal/apollo/raw/apollo_raw_flowctrl.h>
#include <dal/apollo/raw/apollo_raw_vlan.h>
#endif




#include <ioal/mem32.h>
#include <osal/memory.h>

#include <hal/mac/mac_probe.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_qos.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <dal/apollomp/raw/apollomp_raw_hwmisc.h>
#endif

#ifdef CONFIG_SDK_RTL9601B
#include <dal/rtl9601b/dal_rtl9601b_hwmisc.h>
#include <dal/rtl9601b/dal_rtl9601b_ponmac.h>
#include <hal/chipdef/rtl9601b/rtk_rtl9601b_reg_struct.h>
#endif

#ifdef CONFIG_SDK_RTL9602C
#include <dal/rtl9602c/dal_rtl9602c_hwmisc.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <dal/rtl9602c/dal_rtl9602c_switch.h>
#endif


#ifdef CONFIG_SDK_RTL9607C
#include <dal/rtl9607c/dal_rtl9607c_hwmisc.h>
#include <hal/chipdef/rtl9607c/rtk_rtl9607c_reg_struct.h>
#include <dal/rtl9607c/dal_rtl9607c_switch.h>
#endif



#ifdef CONFIG_SDK_RTL9603CVD
#include <dal/rtl9603cvd/dal_rtl9603cvd_hwmisc.h>
#include <hal/chipdef/rtl9603cvd/rtk_rtl9603cvd_reg_struct.h>
#include <dal/rtl9603cvd/dal_rtl9603cvd_switch.h>
#endif

#ifdef CONFIG_SDK_RTL8277C
#include <dal/rtl8277c/dal_rtl8277c_switch.h>
#endif

#ifdef CONFIG_SDK_RTL9607F
#include <dal/rtl9607f/dal_rtl9607f_switch.h>
#endif

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
#include <rtk/rtusr/rtusr_pkt.h>
#endif /* defined(CONFIG_LINUX_USER_SHELL) */


#if defined(CONFIG_EUROPA_FEATURE)
#include <osal/time.h>
#include <rtk/ldd.h>
#include <module/europa/europa_reg_definition.h>
#include <module/europa/europa.h>
#endif


#include <rtk/svlan.h>
#include <rtk/stat.h>
#include <osal/time.h>

#if CONFIG_GPON_VERSION < 2
#include <rtk/gpon.h>
#else
#include <module/gpon/gpon.h>
#endif

#include <diag_display.h>

#define PATCH_W_OFFSET 0xC000
#define PATCH_R_OFFSET 0x8000

const char *diagStr_hsbLatchModeStr[] = {
    DIAG_STR_All,
    DIAG_STR_NONE,
    DIAG_STR_FIRSTDROP,
    DIAG_STR_FIRSTPASS,
    DIAG_STR_FIRSTTRAP2CPU,
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_ACL,
    DIAG_STR_REASON
};
const char *diagStr_hsbLatchModeStr2[] = {
    DIAG_STR_All,
    DIAG_STR_NONE,
    DIAG_STR_FIRSTDROP,
    DIAG_STR_FIRSTPASS,
    DIAG_STR_FIRSTTRAP2CPU,
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_REASON
};

typedef struct pkt_parser_debug_s
{
    uint32 time;
    uint32 spa_mask;
    rtk_mac_t smac;
    rtk_mac_t dmac;
    uint32 ethertype;
    uint32 length_start;
    uint32 length_end;
    uint32 cvid;
    uint32 svid;
    uint32 igmp_if;
    uint32 udp_if;
    uint32 tcp_if;
    uint32 oampdu;
    uint32 llc_other;
    uint32 snap_if;
    uint32 pppoe_if;
    uint32 protocol;
    rtk_ip_addr_t dip;
    rtk_ip_addr_t sip;
}pkt_parser_debug_t;


static rtk_hsb_t diagHsbUserData;


#if defined(CONFIG_EUROPA_FEATURE)

#define EUROPA_PARAMETER_LOCATION   "/var/config/europa.data"
#define EUROPA_PARAMETER_SIZE                                   1600
#define EUROPA_DDMI_A0_ADDR               0
#define EUROPA_DDMI_A2_ADDR               0x100
#define EUROPA_A4_ADDR                       0x200
#define EUROPA_A4_ADDR_BASE                                   (EUROPA_A4_ADDR+1)
#define EUROPA_APD_LUT_ADDR             0x300
#define EUROPA_LASER_LUT_ADDR         0x400
#define EUROPA_DCL_P0_ADDR                1346
#define EUROPA_DCL_P1_ADDR                1347
#define EUROPA_DCL_PAVG_ADDR            1348
#define EUROPA_W57_APCDIG_DAC_BIAS_ADDR           1349
#define EUROPA_RX_A_ADDR                                          1350
#define EUROPA_RX_B_ADDR                                          1354
#define EUROPA_RX_C_ADDR                                          1358
#define EUROPA_RSSI_V0_ADDR                                     1362
#define EUROPA_MPD0_ADDR                                          1366
#define EUROPA_CROSS_ADDR                                       1370
#define EUROPA_TX_A_ADDR                                          1372
#define EUROPA_TX_B_ADDR                                          1376
#define EUROPA_TX_C_ADDR                                          1380
#define EUROPA_TEMP_OFFSET_ADDR                                   1384
#define EUROPA_TEMP_INTR_SCALE_ADDR                               1385
#define EUROPA_RX_TH_ADDR                                         1386
#define EUROPA_RX_DETH_ADDR                                       1387

rtk_ldd_param_t ldd_param;


// ========== DDMI calibration ========== +
uint32 rx_pwr[3];
uint32 rx_rssi[3];

uint32 tx_pwr[2];
uint32 tx_mpd[2];

// shift bits for RX power coefficients
#define RX_A_SHIFT 30
#define RX_B_SHIFT 13
#define RX_C_SHIFT 12

// shift bits for TX power coefficients
#define TX_A_SHIFT 8
#define TX_B_SHIFT 5
// ========== DDMI calibration ========== -

#define RSSI_CURR_BREAKDOWN_TH  5000

// ========== LUT gen ========== +
typedef struct LASER_LUT_T
{
    int32_t temp;
    double bias;
    double mod;
} laser_lut_data_t;

#define MAX_LASER_LUT_DATA_NUM 20

laser_lut_data_t gLaserLutData[MAX_LASER_LUT_DATA_NUM];
laser_lut_data_t gLaserLut[151];  /* -40C ~ 110C LUT */


int my_round(double d)
{
    return (int) ( (0.0<d) ? (d+0.5) : (d-0.5) );
}


void rtk_ldd_flash_data_get(uint32 address, uint32 length, uint32 *value);
void rtk_ldd_flash_data_set(uint32 address, uint32 length, uint32 value);

#endif




static void _debug_pkt_parser_get(pkt_parser_debug_t *pDbg)
{
    FILE *fp_setting;
    char *delim = ":";
    char *pch;
    char buff[50];
    uint32 cnt;
    int tmp;
    rtk_ip_addr_t ip;
    char *file_name_setting = "/var/tmp/packet_parser_config.setting";

    //Initial value for skip compare
    pDbg->dmac.octet[5] = 0xff;
    pDbg->smac.octet[5] = 0xff;
    pDbg->time = 0xffffffff;
    pDbg->spa_mask = 0xffff;
    pDbg->ethertype = 0xffff;
    pDbg->length_start = 0xffff;
    pDbg->length_end = 0xffff;
    pDbg->cvid = 0xffff;
    pDbg->svid = 0xffff;
    pDbg->igmp_if = 0;
    pDbg->udp_if = 0;
    pDbg->tcp_if = 0;
    pDbg->oampdu = 0;
    pDbg->llc_other = 0;
    pDbg->snap_if = 0;
    pDbg->pppoe_if = 0;
    pDbg->protocol = 0;
    pDbg->sip = 0xffffffff;
    pDbg->dip = 0xffffffff;

    fp_setting = fopen(file_name_setting, "r");
    if(!fp_setting)
    {
        return;
    }

    while(NULL != fgets(buff, sizeof(buff), fp_setting))
    {
        pch = strtok(buff, "=");
        if(!strcmp(pch, "DA"))
        {
            cnt = 5;
            while(pch != NULL)
            {
                pch = strtok(NULL, delim);
                if(pch != NULL)
                {
                    tmp = atoi(pch);
                    pDbg->dmac.octet[cnt] = (uint8)(tmp & 0xff);
                    cnt--;
                }
            }
        }
        else if(!strcmp(pch, "SA"))
        {
            cnt = 5;
            while(pch != NULL)
            {
                pch = strtok(NULL, delim);
                if(pch != NULL)
                {
                    tmp = atoi(pch);
                    pDbg->smac.octet[cnt] = (uint8)(tmp & 0xff);
                    cnt--;
                }
            }
        }
        else if(!strcmp(pch, "ethertype"))
        {
            pch = strtok(NULL, delim);
            pDbg->ethertype = atoi(pch);
        }
        else if(!strcmp(pch, "length-start"))
        {
            pch = strtok(NULL, delim);
            pDbg->length_start = atoi(pch);
        }
        else if(!strcmp(pch, "length-end"))
        {
            pch = strtok(NULL, delim);
            pDbg->length_end = atoi(pch);
        }
        else if(!strcmp(pch, "igmp_if"))
        {
            pDbg->igmp_if = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "udp_if"))
        {
            pDbg->udp_if = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "tcp_if"))
        {
            pDbg->tcp_if = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "oampdu"))
        {
            pDbg->oampdu = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "llc_other"))
        {
            pDbg->llc_other = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "snap_if"))
        {
            pDbg->snap_if = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "pppoe_if"))
        {
            pDbg->pppoe_if = 1;
            pDbg->protocol = 1;
        }
        else if(!strcmp(pch, "source-port-mask"))
        {
            pch = strtok(NULL, delim);
            pDbg->spa_mask = atoi(pch);
        }
        else if(!strcmp(pch, "svid"))
        {
            pch = strtok(NULL, delim);
            pDbg->svid = atoi(pch);
        }
        else if(!strcmp(pch, "time"))
        {
            pch = strtok(NULL, delim);
            pDbg->time = atoi(pch);
        }
        else if(!strcmp(pch, "vid"))
        {
            pch = strtok(NULL, delim);
            pDbg->cvid = atoi(pch);
        }
        else if(!strcmp(pch, "sip"))
        {
            pch = strtok(NULL, delim);
            diag_util_str2ip(&ip, pch);
            pDbg->sip = ip;
        }
        else if(!strcmp(pch, "dip"))
        {
            pch = strtok(NULL, delim);
            diag_util_str2ip(&ip, pch);
            pDbg->dip = ip;
        }
        else
        {
            printf("WRONG data!!\n");
        }
    }


    fclose(fp_setting);

}

#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
static void _debug_hsb_write(FILE * fp, rtk_hsb_t *hsbData)
{
    uint32 chipId;
    uint32 ChipRevId;
    uint32 ret;
    fprintf(fp, "spa: %d pktLen: %d ponIdx: %d\n",
                                hsbData->spa,
                                hsbData->pkt_len,
                                hsbData->pon_idx);

    fprintf(fp, "da: %s ",
                        diag_util_inet_mactoa(&hsbData->da.octet[0]));
    fprintf(fp, "sa: %s etherType: 0x%4.4x\n",
                        diag_util_inet_mactoa(&hsbData->sa.octet[0]),
                        hsbData->ether_type);

    fprintf(fp, "ctag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->ctag_if,
                        (hsbData->ctag&0xE000)>>13,
                        (hsbData->ctag&0x1000)>>12,
                        hsbData->ctag&0xFFF);


    fprintf(fp, "stag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->stag_if,
                        (hsbData->stag&0xE000)>>13,
                        (hsbData->stag&0x1000)>>12,
                        hsbData->stag&0xFFF);
    fprintf(fp, "ip4if: %d dip: %s ",
                        hsbData->ip4_if,
                        diag_util_inet_ntoa(hsbData->dip));
    fprintf(fp, "sip: %s\n",
                        diag_util_inet_ntoa(hsbData->sip));
    fprintf(fp, "ip6if: %d ",hsbData->ip6_if);

    fprintf(fp, "dip6: %s ",diag_util_inet_n6toa(&hsbData->dip6.ipv6_addr[0]));
    fprintf(fp, "sip6: %s ",diag_util_inet_n6toa(&hsbData->sip6.ipv6_addr[0]));


    fprintf(fp, "userfield valid: 0x%4.4x\n",hsbData->user_valid);
    fprintf(fp, "00-07: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                        hsbData->user_field_0,
                        hsbData->user_field_1,
                        hsbData->user_field_2,
                        hsbData->user_field_3,
                        hsbData->user_field_4,
                        hsbData->user_field_5,
                        hsbData->user_field_6,
                        hsbData->user_field_7);
    
    if(hsbData->cputag_if)
    {
        fprintf(fp, "aware cpu-tag fields\n");
    }
    
}

#else

static void _debug_hsb_write(FILE * fp, rtk_hsb_t *hsbData)
{
    uint32 chipId;
    uint32 ChipRevId;
    uint32 ret;
    fprintf(fp, "spa: %d pktLen: %d ponIdx: %d\n",
                                hsbData->spa,
                                hsbData->pkt_len,
                                hsbData->pon_idx);

    fprintf(fp, "da: %s ",
                        diag_util_inet_mactoa(&hsbData->da.octet[0]));
    fprintf(fp, "sa: %s etherType: 0x%4.4x\n",
                        diag_util_inet_mactoa(&hsbData->sa.octet[0]),
                        hsbData->ether_type);

    fprintf(fp, "ctag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->ctag_if,
                        (hsbData->ctag&0xE000)>>13,
                        (hsbData->ctag&0x1000)>>12,
                        hsbData->ctag&0xFFF);

    if ((ret = drv_swcore_cid_get(&chipId,&ChipRevId)) != RT_ERR_OK)
    {
        return;
    }

    switch(chipId)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            fprintf(fp, "stag: %d pri: %d cfi: %d vid: %d tpid: %d\n",
                                hsbData->stag_if,
                                (hsbData->stag&0xE000)>>13,
                                (hsbData->stag&0x1000)>>12,
                                hsbData->stag&0xFFF,
                                hsbData->stag_tpid);
            break;
#endif
        default:
            fprintf(fp, "stag: %d pri: %d cfi: %d vid: %d\n",
                                hsbData->stag_if,
                                (hsbData->stag&0xE000)>>13,
                                (hsbData->stag&0x1000)>>12,
                                hsbData->stag&0xFFF);
            break;
    }

    switch(chipId)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:

            fprintf(fp, "ip4if: %d dip: %s ",
                                hsbData->ip4_if,
                                diag_util_inet_ntoa(hsbData->dip));
            fprintf(fp, "sip: %s tos_dscp: 0x%2.2x\n",
                                diag_util_inet_ntoa(hsbData->sip),
                                hsbData->tos_dscp);
            fprintf(fp, "ip6if: %d ",hsbData->ip6_if);

            fprintf(fp, "dip6: %s ",diag_util_inet_n6toa(&hsbData->dip6.ipv6_addr[0]));
            fprintf(fp, "sip6: %s ",diag_util_inet_n6toa(&hsbData->sip6.ipv6_addr[0]));
            fprintf(fp, "tc: %d\n",hsbData->tc);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            fprintf(fp, "ip4if: %d dip: %s ",
                                hsbData->ip4_if,
                                diag_util_inet_ntoa(hsbData->dip));
            fprintf(fp, "sip: %s\n",
                                diag_util_inet_ntoa(hsbData->sip));
            fprintf(fp, "ip6if: %d ",hsbData->ip6_if);

            fprintf(fp, "dip6: %s ",diag_util_inet_n6toa(&hsbData->dip6.ipv6_addr[0]));
            fprintf(fp, "sip6: %s ",diag_util_inet_n6toa(&hsbData->sip6.ipv6_addr[0]));
            break;
#else

        default:
            fprintf(fp, "dip: %s ",
                                diag_util_inet_ntoa(hsbData->dip));
            fprintf(fp, "sip: %s iptype: %d tos_dscp: 0x%2.2x\n",
                                diag_util_inet_ntoa(hsbData->sip),
                                hsbData->ip_type,
                                hsbData->tos_dscp);
            break;
#endif
    }

    switch(chipId)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            fprintf(fp, "ip6hnrg l4ok l3ok gt1 gt5 gre icmp udp tcp\n");
            fprintf(fp, "%-7d %-4d %-4d %-3d %-3d %-3d %-4d %-3d %-3d\n",
                                hsbData->ip6_nh_rg,
                                hsbData->cks_ok_l4,
                                hsbData->cks_ok_l3,
                                hsbData->ttl_gt1,
                                hsbData->ttl_gt5,
                                hsbData->gre_if,
                                hsbData->icmp_if,
                                hsbData->udp_if,
                                hsbData->tcp_if);

            fprintf(fp, "ptp oam rlpp rldp llc snap pppoe session\n");
            fprintf(fp, "%-3d %-3d %-4d %-4d %-3d %-4d %-5d 0x%4.4x\n",
                                hsbData->ptp_if,
                                hsbData->oampdu,
                                hsbData->rlpp_if,
                                hsbData->rldp_if,
                                hsbData->llc_other,
                                hsbData->snap_if,
                                hsbData->pppoe_if,
                                hsbData->pppoe_session);

            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            fprintf(fp, "igmp udp tcp oam llc snap pppoe\n");
            fprintf(fp, "%-4d %-3d %-3d %-3d %-3d %-4d %-5d\n",
                                hsbData->igmp_if,
                                hsbData->udp_if,
                                hsbData->tcp_if,
                                hsbData->oampdu,
                                hsbData->llc_other,
                                hsbData->snap_if,
                                hsbData->pppoe_if);

            break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                fprintf(fp, "nhlen protohn nhrg l4ok l3ok gt1 gt5 l4type\n");
                fprintf(fp, "%-5d %-7d %-4d %-4d %-4d %-3d %-3d %d\n",
                                    hsbData->len_of_nhs,
                                    hsbData->l3proto_nh,
                                    hsbData->ip6_nh_rg,
                                    hsbData->cks_ok_l4,
                                    hsbData->cks_ok_l3,
                                    hsbData->ttl_gt1,
                                    hsbData->ttl_gt5,
                                    hsbData->l4_type);

                fprintf(fp, "ptp oam rlpp rldp llc snap pppoe session\n");
                fprintf(fp, "%-3d %-3d %-4d %-4d %-3d %-4d %-5d 0x%4.4x\n",
                                    hsbData->ptp_if,
                                    hsbData->oampdu,
                                    hsbData->rlpp_if,
                                    hsbData->rldp_if,
                                    hsbData->llc_other,
                                    hsbData->snap_if,
                                    hsbData->pppoe_if,
                                    hsbData->pppoe_session);

                break;
#endif

    }

    switch(chipId)
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            fprintf(fp, "userfield valid: 0x%4.4x\n",hsbData->user_valid);
            fprintf(fp, "00-07: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                                hsbData->user_field_0,
                                hsbData->user_field_1,
                                hsbData->user_field_2,
                                hsbData->user_field_3,
                                hsbData->user_field_4,
                                hsbData->user_field_5,
                                hsbData->user_field_6,
                                hsbData->user_field_7);
            break;
#endif
        default:
            fprintf(fp, "userfield valid: 0x%4.4x\n",hsbData->user_valid);
            fprintf(fp, "00-07: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                                hsbData->user_field_0,
                                hsbData->user_field_1,
                                hsbData->user_field_2,
                                hsbData->user_field_3,
                                hsbData->user_field_4,
                                hsbData->user_field_5,
                                hsbData->user_field_6,
                                hsbData->user_field_7);
            
            fprintf(fp, "08-15: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                                hsbData->user_field_8,
                                hsbData->user_field_9,
                                hsbData->user_field_10,
                                hsbData->user_field_11,
                                hsbData->user_field_12,
                                hsbData->user_field_13,
                                hsbData->user_field_14,
                                hsbData->user_field_15);
            break;
    }


    if(hsbData->cputag_if)
    {
        fprintf(fp, "aware cpu-tag fields\n");
        switch(chipId)
        {
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                fprintf(fp, "l3c l4c txpmsk efidEn efid priSel pri keep vsel dislrn\n");
                fprintf(fp, "%-3d %-3d 0x%2.2x   %-6d %-4d %-6d %-3d %-4d %-4d %d\n",
                                hsbData->cputag_l3c,
                                hsbData->cputag_l4c,
                                hsbData->cputag_txpmsk,
                                hsbData->cputag_efid_en,
                                hsbData->cputag_efid,
                                hsbData->cputag_prisel,
                                hsbData->cputag_pri,
                                hsbData->cputag_keep,
                                hsbData->cputag_vsel,
                                hsbData->cputag_dislrn);

                fprintf(fp, "psel l34keep extspa pppoe_act pppoe_idx l2br pon_sid\n");
                fprintf(fp, "%-4d %-7d %-6d %-9d %-9d %-4d %-7d\n",
                                hsbData->cputag_psel,
                                hsbData->cputag_l34keep,
                                hsbData->cputag_extspa,
                                hsbData->cputag_pppoe_act,
                                hsbData->cputag_pppoe_idx,
                                hsbData->cputag_l2br,
                                hsbData->cputag_pon_sid);
                break;
#endif
#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
                fprintf(fp, "txpmsk priSel pri keep dislrn psel pon_sid\n");
                fprintf(fp, "0x%2.2x   %-6d %-3d %-4d %-6d %-4d %-7d\n",
                                hsbData->cputag_txpmsk,
                                hsbData->cputag_prisel,
                                hsbData->cputag_pri,
                                hsbData->cputag_keep,
                                hsbData->cputag_dislrn,
                                hsbData->cputag_psel,
                                hsbData->cputag_pon_sid);
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                fprintf(fp, "l3c l4c txpmsk priSel pri keep dislrn sb\n");
                fprintf(fp, "%-3d %-3d 0x%2.2x   %-6d %-3d %-4d %-6d %-2d\n",
                                hsbData->cputag_l3c,
                                hsbData->cputag_l4c,
                                hsbData->cputag_txpmsk,
                                hsbData->cputag_prisel,
                                hsbData->cputag_pri,
                                hsbData->cputag_keep,
                                hsbData->cputag_dislrn,
                                hsbData->cputag_sb);

                fprintf(fp, "psel l34keep pppoe_act pppoe_idx l2br pon_sid\n");
                fprintf(fp, "%-4d %-7d %-9d %-9d %-4d %-7d\n",
                                hsbData->cputag_psel,
                                hsbData->cputag_l34keep,
                                hsbData->cputag_pppoe_act,
                                hsbData->cputag_pppoe_idx,
                                hsbData->cputag_l2br,
                                hsbData->cputag_pon_sid);
                break;
#endif
        }
    }
}
#endif

static void _debug_hsa_write(FILE * fp, rtk_hsa_t *hsaData)
{
    uint32 chipId;
    uint32 ChipRevId;
    uint32 ret;

    if ((ret = drv_swcore_cid_get(&chipId,&ChipRevId)) != RT_ERR_OK)
    {
        return;
    }


#if defined(CONFIG_SDK_APOLLOMP)

    switch(chipId)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            fprintf(fp, "Port      CPU  5    PON  3    2    1    0\n");
            break;
#endif
    }



    fprintf(fp, "user_pri: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsam_user_pri>>18)&7,
                                (hsaData->rng_nhsam_user_pri>>15)&7,
                                (hsaData->rng_nhsam_user_pri>>12)&7,
                                (hsaData->rng_nhsam_user_pri>>9)&7,
                                (hsaData->rng_nhsam_user_pri>>6)&7,
                                (hsaData->rng_nhsam_user_pri>>3)&7,
                                hsaData->rng_nhsam_user_pri&7);

    switch(chipId)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            fprintf(fp, "qid:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_qid>>22)&7,
                                        (hsaData->rng_nhsab_qid>>19)&7,
                                        (hsaData->rng_nhsab_qid>>12)&0x7F,
                                        (hsaData->rng_nhsab_qid>>9)&7,
                                        (hsaData->rng_nhsab_qid>>6)&7,
                                        (hsaData->rng_nhsab_qid>>3)&7,
                                        hsaData->rng_nhsab_qid&7);
            break;
#endif
    }


    fprintf(fp, "dmp:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsab_dpm>>12)&3,
                                (hsaData->rng_nhsab_dpm>>10)&3,
                                (hsaData->rng_nhsab_dpm>>8)&3,
                                (hsaData->rng_nhsab_dpm>>6)&3,
                                (hsaData->rng_nhsab_dpm>>4)&3,
                                (hsaData->rng_nhsab_dpm>>2)&3,
                                hsaData->rng_nhsab_dpm&3);

    fprintf(fp, "untagset: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsac_untagset>>6)&1,
                                (hsaData->rng_nhsac_untagset>>5)&1,
                                (hsaData->rng_nhsac_untagset>>4)&1,
                                (hsaData->rng_nhsac_untagset>>3)&1,
                                (hsaData->rng_nhsac_untagset>>2)&1,
                                (hsaData->rng_nhsac_untagset>>1)&1,
                                hsaData->rng_nhsac_untagset&1);

    fprintf(fp, "spa ctag_act tag_if vid  cfi pri vidzero\n");
    fprintf(fp, "%-3d %-8d %-6d %-4d %-3d %-3d %-7d\n",
                                hsaData->rng_nhsab_spa,
                                hsaData->rng_nhsac_ctag_act,
                                hsaData->rng_nhsac_ctag_if,
                                hsaData->rng_nhsac_vid,
                                hsaData->rng_nhsac_cfi,
                                hsaData->rng_nhsac_pri,
                                hsaData->rng_nhsac_vidzero);


    fprintf(fp, "stag_type stag_if sp2s svid svidx dei spri pkt_spri vidsel frctag frctag_if\n");
    fprintf(fp, "%-9d %-7d %-4d %-4d %-5d %-3d %-4d %-8d %-6d %-6d %-9d\n",
                                hsaData->rng_nhsas_stag_type,
                                hsaData->rng_nhsas_stag_if,
                                hsaData->rng_nhsas_sp2s,
                                hsaData->rng_nhsas_svid,
                                hsaData->rng_nhsas_svidx,
                                hsaData->rng_nhsas_dei,
                                hsaData->rng_nhsas_spri,
                                hsaData->rng_nhsas_pkt_spri,
                                hsaData->rng_nhsas_vidsel,
                                hsaData->rng_nhsas_frctag,
                                hsaData->rng_nhsas_frctag_if);
    switch(chipId)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            fprintf(fp, "1prem 1premen dscprem dscpremen cpukeep keep ptp tcp udp ipv4 ipv6 1042 pppoe\n");
            fprintf(fp, "%-5d %-7d %-7d %-9d %-7d %-4d %-3d %-3d %-3d %-4d %-4d %-4d %-5d\n",
                                        hsaData->rng_nhsam_1p_rem,
                                        hsaData->rng_nhsam_1p_rem_en,
                                        hsaData->rng_nhsam_dscp_rem,
                                        hsaData->rng_nhsam_dscp_rem_en,
                                        hsaData->rng_nhsaf_cpukeep,
                                        hsaData->rng_nhsaf_keep,
                                        hsaData->rng_nhsaf_ptp,
                                        hsaData->rng_nhsaf_tcp,
                                        hsaData->rng_nhsaf_udp,
                                        hsaData->rng_nhsaf_ipv4,
                                        hsaData->rng_nhsaf_ipv6,
                                        hsaData->rng_nhsaf_rfc1042,
                                        hsaData->rng_nhsaf_pppoe_if);

            break;
#endif
    }



    fprintf(fp, "endsc  bgdsc  cpupri fwdrsn pon_sid pktlen regen_crc\n");
    fprintf(fp, "0x%-4.4x 0x%-4.4x %-6d %-6d %-7d %-6d %-d\n",
                                hsaData->rng_nhsab_endsc,
                                hsaData->rng_nhsab_bgdsc,
                                hsaData->rng_nhsab_cpupri,
                                hsaData->rng_nhsab_fwdrsn,
                                hsaData->rng_nhsab_pon_sid,
                                hsaData->rng_nhsab_pktlen,
                                hsaData->rng_nhsaf_regen_crc);


    switch(chipId)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            fprintf(fp, "ext_mask l3 org ipmc l2trans l34trans src_mode l3chsum l4schsum\n");
            fprintf(fp, "0x%4.4x   %-2d %-3d %-4d %-7d %-8d %-8d 0x%4.4x  0x%4.4x\n",
                                        hsaData->rng_nhsab_ext_mask,
                                        hsaData->rng_nhsan_l3,
                                        hsaData->rng_nhsan_org,
                                        hsaData->rng_nhsan_ipmc,
                                        hsaData->rng_nhsan_l2trans,
                                        hsaData->rng_nhsan_l34trans,
                                        hsaData->rng_nhsan_src_mode,
                                        hsaData->rng_nhsan_l3chsum,
                                        hsaData->rng_nhsan_l4chsum);

            break;
#endif
    }


    fprintf(fp, "pppoe_idx: %d pppoe_act: %d ttl_extmask: 0x%2.2x ttl_pmask: 0x%2.2x\n",
                                hsaData->rng_nhsan_pppoe_idx,
                                hsaData->rng_nhsan_pppoe_act,
                                hsaData->rng_nhsan_ttlm1_extmask,
                                hsaData->rng_nhsan_ttlm1_pmask);

    fprintf(fp, "newmac: %s smac_idx: %d ",
                        diag_util_inet_mactoa(&hsaData->rng_nhsan_newmac.octet[0]),
                        hsaData->rng_nhsan_smac_idx);
    fprintf(fp, "newip: %s newport: 0x%4.4x\n",
                        diag_util_inet_ntoa(hsaData->rng_nhsan_newip),
                        hsaData->rng_nhsan_newport);

    switch(chipId)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            fprintf(fp, "floodpkt: %d ctag_ponac: %d ponvid: %d ponpri: %d ponsvid: %d ponspir: %d\n",
                                        hsaData->rng_nhsab_floodpkt,
                                        hsaData->rng_nhsac_ctag_ponact,
                                        hsaData->rng_nhsac_ponvid,
                                        hsaData->rng_nhsac_ponpri,
                                        hsaData->rng_nhsas_ponsvid,
                                        hsaData->rng_nhsas_ponspri);

            fprintf(fp, "Port        CPU  5    PON  3    2    1    0\n");
            fprintf(fp, "un-Stagset: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsas_untagset>>6)&1,
                                (hsaData->rng_nhsas_untagset>>5)&1,
                                (hsaData->rng_nhsas_untagset>>4)&1,
                                (hsaData->rng_nhsas_untagset>>3)&1,
                                (hsaData->rng_nhsas_untagset>>2)&1,
                                (hsaData->rng_nhsas_untagset>>1)&1,
                                hsaData->rng_nhsas_untagset&1);

            break;
#endif
    }
#endif

    switch(chipId)
    {

#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            fprintf(fp, "Port      CPU  PON  0\n");
            fprintf(fp, "user_pri: %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsam_user_pri>>6)&7,
                                        (hsaData->rng_nhsam_user_pri>>3)&7,
                                        hsaData->rng_nhsam_user_pri&7);

            fprintf(fp, "qid:      %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_qid>>6)&7,
                                        (hsaData->rng_nhsab_qid>>3)&7,
                                        hsaData->rng_nhsab_qid&7);


            fprintf(fp, "dmp:      %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_dpm>>2)&1,
                                        (hsaData->rng_nhsab_dpm>>1)&1,
                                        hsaData->rng_nhsab_dpm&1);

            fprintf(fp, "untagset: %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsac_untagset>>2)&1,
                                        (hsaData->rng_nhsac_untagset>>1)&1,
                                        hsaData->rng_nhsac_untagset&1);
            fprintf(fp, "s-untag:  %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsas_untagset>>2)&1,
                                        (hsaData->rng_nhsas_untagset>>1)&1,
                                        hsaData->rng_nhsas_untagset&1);

            fprintf(fp, "orgcvid orgcpri orgcfi ctagponact ctagact vidzero\n");
            fprintf(fp, "%-7d %-7d %-6d %-10d %-7d %-7d\n",
                                        hsaData->rng_nhsac_org_cvid,
                                        hsaData->rng_nhsac_org_cpri,
                                        hsaData->rng_nhsac_org_cfi,
                                        hsaData->rng_nhsac_ctag_ponact,
                                        hsaData->rng_nhsac_ctag_act,
                                        hsaData->rng_nhsac_vidzero);

            fprintf(fp, "pritagif ctagif ponvid vid ponpri pri cactnop cacttag\n");
            fprintf(fp, "%-8d %-6d %-6d %-3d %-6d %-3d %-7d %-7d\n",
                                        hsaData->rng_nhsac_pritag_if,
                                        hsaData->rng_nhsac_ctag_if,
                                        hsaData->rng_nhsac_ponvid,
                                        hsaData->rng_nhsac_vid,
                                        hsaData->rng_nhsac_ponpri,
                                        hsaData->rng_nhsac_pri,
                                        hsaData->rng_nhsac_cact_nop,
                                        hsaData->rng_nhsac_cact_tag);

            fprintf(fp, "mdysvid mdysvidpon stagif dei stagtype frctag frctagif\n");
            fprintf(fp, "%-7d %-10d %-6d %-3d %-8d %-6d %-8d\n",
                                        hsaData->rng_nhsas_mdy_svid,
                                        hsaData->rng_nhsas_mdy_svid_pon,
                                        hsaData->rng_nhsas_stag_if,
                                        hsaData->rng_nhsas_dei,
                                        hsaData->rng_nhsas_stag_type,
                                        hsaData->rng_nhsas_frctag,
                                        hsaData->rng_nhsas_frctag_if);
            fprintf(fp, "ponsvid svid pktspri ponspri spri\n");
            fprintf(fp, "%-7d %-4d %-7d %-7d %-4d\n",
                                        hsaData->rng_nhsas_ponsvid,
                                        hsaData->rng_nhsas_svid,
                                        hsaData->rng_nhsas_pkt_spri,
                                        hsaData->rng_nhsas_ponspri,
                                        hsaData->rng_nhsas_spri);

            fprintf(fp, "cputagif 1prempon 1premenpon dscprempon dscpremenpon\n");
            fprintf(fp, "%-8d %-8d %-10d %-10d %-12d\n",
                                        hsaData->rng_nhsam_cputag_if,
                                        hsaData->rng_nhsam_1p_rem_pon,
                                        hsaData->rng_nhsam_1p_rem_en_pon,
                                        hsaData->rng_nhsam_dscp_rem_pon,
                                        hsaData->rng_nhsam_dscp_rem_en_pon);
            fprintf(fp, "1prem 1premen dscprem dscpremen\n");
            fprintf(fp, "%-5d %-7d %-7d %-9d\n",
                                        hsaData->rng_nhsam_1p_rem,
                                        hsaData->rng_nhsam_1p_rem_en,
                                        hsaData->rng_nhsam_dscp_rem,
                                        hsaData->rng_nhsam_dscp_rem_en);

            fprintf(fp, "regencrc cpukeep keep tcp udp ipv4 ipv6 1042 pppoe ipmc\n");
            fprintf(fp, "%-8d %-7d %-4d %-3d %-3d %-4d %-4d %-4d %-5d %-4d\n",
                                        hsaData->rng_nhsaf_regen_crc,
                                        hsaData->rng_nhsaf_cpukeep,
                                        hsaData->rng_nhsaf_keep,
                                        hsaData->rng_nhsaf_tcp,
                                        hsaData->rng_nhsaf_udp,
                                        hsaData->rng_nhsaf_ipv4,
                                        hsaData->rng_nhsaf_ipv6,
                                        hsaData->rng_nhsaf_rfc1042,
                                        hsaData->rng_nhsaf_pppoe_if,
                                        hsaData->rng_nhsab_ipmc);

            fprintf(fp, "endsc  bgdsc  floodpkt cpupri fwdrsn pon_sid spa pktlen omcipktlen\n");
            fprintf(fp, "0x%-4.4x 0x%-4.4x %-8d %-6d %-6d %-7d %-3d %-6d %-d\n",
                                        hsaData->rng_nhsab_endsc,
                                        hsaData->rng_nhsab_bgdsc,
                                        hsaData->rng_nhsab_floodpkt,
                                        hsaData->rng_nhsab_cpupri,
                                        hsaData->rng_nhsab_fwdrsn,
                                        hsaData->rng_nhsab_pon_sid,
                                        hsaData->rng_nhsab_spa,
                                        hsaData->rng_nhsab_pktlen,
                                        hsaData->rng_nhsab_omci_pktlen);

            break;
#endif
    }
}

static int32 _diag_debug_internal_phy_read(
    uint32      phyId,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    int32 ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            ret = apollo_interPhy_read(phyId, page, phyRegAddr, pData);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            ret = apollomp_interPhy_read(phyId, page, phyRegAddr, pData);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
    #if defined(FPGA_DEFINED)
            ret = rtl9601b_interPhy_read(phyId, page, phyRegAddr, pData);
    #else
            ret = rtl9601b_ocpInterPhy_read(phyId, phyRegAddr, pData);
    #endif
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
    #if defined(FPGA_DEFINED)
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return RT_ERR_FAILED;
    #else
            ret = rtl9602c_ocpInterPhy_read(phyId, phyRegAddr, pData);
    #endif
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return RT_ERR_FAILED;
            break;
    }

    return ret;
}



static int32 _diag_debug_internal_phy_write(
    uint32      phyId,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    int32 ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            ret = apollo_interPhy_write(phyId, page, phyRegAddr, data);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            ret = apollomp_interPhy_write(phyId, page, phyRegAddr, data);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
    #if defined(FPGA_DEFINED)
            ret = rtl9601b_interPhy_write(phyId, page, phyRegAddr, data);
    #else
            ret = rtl9601b_ocpInterPhy_write(phyId, phyRegAddr, data);
    #endif
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
    #if defined(FPGA_DEFINED)
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return RT_ERR_FAILED;
    #else
            ret = rtl9602c_ocpInterPhy_write(phyId, phyRegAddr, data);
    #endif
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return RT_ERR_FAILED;
            break;
    }

    return ret;
}



/*
 * debug gpio init
 */
cparser_result_t
cparser_cmd_debug_gpio_init(
    cparser_context_t *context)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_gpio_init(), ret);
    diag_util_printf("gpio init ok!\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_init */

/*
 * debug gpio set pin <UINT:gpioId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_gpio_set_pin_gpioId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr)
{
    int32 ret;
    uint32 gpioId;
    rtk_enable_t enable = DISABLED;
    DIAG_UTIL_PARAM_CHK();

    if(gpioId_ptr ==NULL)
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    gpioId  = *gpioId_ptr;
    if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    DIAG_UTIL_ERR_CHK(rtk_gpio_state_set(gpioId,enable), ret);

    if(enable){
        diag_util_printf("set gpio %d to enable!\n",gpioId);
    }else {
        diag_util_printf("set gpio %d to disable!\n",gpioId);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_set_pin_gpioid_state_enable_disable */

/*
 * debug gpio get pin <UINT:gpioId> state
 */
cparser_result_t
cparser_cmd_debug_gpio_get_pin_gpioId_state(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr)
{
    int32 ret;
    uint32 state,gpioId;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(gpioId_ptr ==NULL)
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    gpioId = *gpioId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_gpio_state_get(gpioId,&state), ret);

    if(state){
        diag_util_printf("gpio %d is enabled!\n",gpioId);
    }else {
        diag_util_printf("gpio %d is disabled!\n",gpioId);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_get_pin_gpioid_state */

/*
 * debug gpio set pin <UINT:gpioId> mode ( input | output )
 */
cparser_result_t
cparser_cmd_debug_gpio_set_pin_gpioId_mode_input_output(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr)
{

    int32 ret;
    uint32 mode = GPIO_INPUT,gpioId;
    DIAG_UTIL_PARAM_CHK();

    if(gpioId_ptr ==NULL)
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    gpioId = *gpioId_ptr;

    if ('i' == TOKEN_CHAR(6,0))
    {
        mode = GPIO_INPUT;
    }
    else if ('o' == TOKEN_CHAR(6,0))
    {
        mode = GPIO_OUTPUT;
    }

    DIAG_UTIL_ERR_CHK(rtk_gpio_mode_set(gpioId,mode), ret);

    if(mode==GPIO_INPUT)
    {
        diag_util_printf("set gpio %d to input mode!\n",gpioId);
    }
    else
    {
        diag_util_printf("set gpio %d to output mode!\n",gpioId);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_set_pin_gpioid_mode_input_output */

/*
 * debug gpio get pin <UINT:gpioId> mode
 */
cparser_result_t
cparser_cmd_debug_gpio_get_pin_gpioId_mode(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr)
{

    int32 ret;
    uint32 mode,gpioId;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(gpioId_ptr ==NULL)
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    gpioId = *gpioId_ptr;

    DIAG_UTIL_ERR_CHK(rtk_gpio_mode_get(gpioId,&mode), ret);

    if(mode==GPIO_INPUT){
        diag_util_printf("gpio %d is input mode!\n",gpioId);
    }else {
        diag_util_printf("gpio %d is output mode!\n",gpioId);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_get_pin_gpioid_mode */

/*
 * debug gpio set pin <UINT:gpioId> databit <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_gpio_set_pin_gpioId_databit_value(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr,
    uint32_t  *value_ptr)
{
    int32 ret;
    uint32 value,gpioId;
    DIAG_UTIL_PARAM_CHK();
    if(gpioId_ptr ==NULL)
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    gpioId = *gpioId_ptr;

    if(value_ptr ==NULL || (*value_ptr >= RTK_ENABLE_END))
    {
        diag_util_printf("input value out of range!\n");
    return CPARSER_ERR_INVALID_PARAMS;
    }
    value = *value_ptr;

    DIAG_UTIL_ERR_CHK(rtk_gpio_databit_set(gpioId,value), ret);
    diag_util_printf("set gpio %d value %d!\n",gpioId,value);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_set_pin_gpioid_databit_value */

/*
 * debug gpio get pin <UINT:gpioId> databit
 */
cparser_result_t
cparser_cmd_debug_gpio_get_pin_gpioId_databit(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr)
{
    int32 ret;
    uint32 gpioId,value;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(gpioId_ptr ==NULL)
    {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    gpioId = *gpioId_ptr;
    DIAG_UTIL_ERR_CHK(rtk_gpio_databit_get(gpioId,&value), ret);
    diag_util_printf("gpio %d value %d!\n",gpioId,value);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_get_pin_gpioid_databit */



/*
  * debug gpio set intr <UINT:gpioId> mode ( falling_edge | rising_edge | both_edge | disable )
  */
cparser_result_t
cparser_cmd_debug_gpio_set_intr_gpioId_mode_falling_edge_rising_edge_both_edge_disable(
 cparser_context_t *context,
 uint32_t  *gpioId_ptr)
{

    int32 ret;
    rtk_gpio_intrMode_t intrMode;
    DIAG_UTIL_PARAM_CHK();

     if ('f' == TOKEN_CHAR(6,0))
    {
        intrMode = GPIO_INTR_ENABLE_FALLING_EDGE;
    }
    else if ('r' == TOKEN_CHAR(6,0))
    {
        intrMode = GPIO_INTR_ENABLE_RISING_EDGE;
    }
    else if ('b' == TOKEN_CHAR(6,0))
    {
        intrMode = GPIO_INTR_ENABLE_BOTH_EDGE;
    }
    else
    {
        intrMode = GPIO_INTR_DISABLE;
    }


    DIAG_UTIL_ERR_CHK(rtk_gpio_intr_set(*gpioId_ptr,intrMode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_set_intr_gpioid_mode_falling_edge_rising_edge_both_edge_disable */

/*
* debug gpio get intr <UINT:gpioId> mode
*/
cparser_result_t
cparser_cmd_debug_gpio_get_intr_gpioId_mode(
 cparser_context_t *context,
 uint32_t  *gpioId_ptr)
{
    int32 ret;
    rtk_gpio_intrMode_t intrMode;
    char tmp[32]="";
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_gpio_intr_get(*gpioId_ptr,&intrMode), ret);

    switch(intrMode){
    case GPIO_INTR_ENABLE_FALLING_EDGE:
        strcpy(tmp,"FALLING EDGE");
    break;
    case GPIO_INTR_ENABLE_RISING_EDGE:
        strcpy(tmp,"RISING EDGE");
    break;
    case GPIO_INTR_ENABLE_BOTH_EDGE:
        strcpy(tmp,"BOTH EDGE");
    break;
    case GPIO_INTR_DISABLE:
        strcpy(tmp,"DISABLE");
    break;
    default:
        diag_util_printf("get gpio IMR failed!\n");
        return CPARSER_OK;
    break;
    }

    diag_util_printf("gpio %d intr mode  %s!\n",*gpioId_ptr,tmp);
    return CPARSER_OK;

}    /* end of cparser_cmd_debug_gpio_get_intr_gpioid_mode */

/*
* debug gpio get intr <UINT:gpioId> state
*/
cparser_result_t
cparser_cmd_debug_gpio_get_intr_gpioId_state(
 cparser_context_t *context,
 uint32_t  *gpioId_ptr)
{
    int32 ret;
    rtk_enable_t state;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_gpio_intrStatus_get(*gpioId_ptr,&state), ret);
    diag_util_printf("gpio %d intr status  %s!\n",*gpioId_ptr,state ? "ENABLED":"DISABLED");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_get_intr_gpioid_state */

/*
* debug gpio clear intr <UINT:gpioId> state
*/
cparser_result_t
cparser_cmd_debug_gpio_clear_intr_gpioId_state(
 cparser_context_t *context,
 uint32_t  *gpioId_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_gpio_intrStatus_clean(*gpioId_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_gpio_clear_intr_gpioid_state */




#define CF_HIT_FIELD_MASK       0x200
#define CF_HIT_VALUE_MASK       0x1ff
#define FIELD_SIZE 10

#if defined(CONFIG_SDK_RTL9602C)
const char *diagStr_cfActBit[] = {
           "CVlan",
           "SVlan",
           "DscpRemark",
           "Forward",
           "CfPri",
           "Sid"
};
#endif

cparser_result_t
cparser_cmd_debug_classf_get_hit_entry(
    cparser_context_t *context)
{
    int32 ret,i;
    uint32 value=0,reg=0,field=0;
    uint32 index,arraySize=0;
    int32 portIndex=REG_ARRAY_INDEX_NONE,arrayIndex=REG_ARRAY_INDEX_NONE ;

        DIAG_UTIL_PARAM_CHK();
        DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
    case APOLLO_CHIP_ID:
        reg = STAT_PRVTE_DROP_COUNTERr;
        field= PKT_INFOf;
        portIndex = 8;
        arraySize = 1;

        for(i=0; i<arraySize; i++)
        {
            arrayIndex+=i;
            DIAG_UTIL_ERR_CHK(reg_array_field_read(reg, portIndex,arrayIndex,field, &value), ret);
            diag_util_printf("Field: %d, Value: 0x%x\n",i,value);
            /*check field hit or not*/
            if(value & CF_HIT_FIELD_MASK)
            {
                index = value & CF_HIT_VALUE_MASK;
                diag_util_printf("classification hit entry: %d\n",index);

            }
            else
            {
                diag_util_printf("classification hit entry: none\n");
            }
        }
    break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    case APOLLOMP_CHIP_ID:
        /*This register is for CF hit check*/
        reg = APOLLOMP_STAT_CF_REASONr;
        field= APOLLOMP_CF_HIT_INFOf;
        arrayIndex = 0;
        arraySize = 2;

        for(i=0; i<arraySize; i++)
        {
            arrayIndex+=i;
            DIAG_UTIL_ERR_CHK(reg_array_field_read(reg, portIndex,arrayIndex,field, &value), ret);
            diag_util_printf("Field: %d, Value: 0x%x\n",i,value);
            /*check field hit or not*/
            if(value & CF_HIT_FIELD_MASK)
            {
                index = value & CF_HIT_VALUE_MASK;
                diag_util_printf("classification hit entry: %d\n",index);

            }
            else
            {
                diag_util_printf("classification hit entry: none\n");
            }
        }
    break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
    case RTL9601B_CHIP_ID:
        /*This register is for CF hit check*/
        reg = RTL9601B_STAT_CF_REASONr;
        field= RTL9601B_CF_HIT_INFOf;
        DIAG_UTIL_ERR_CHK(reg_field_read(reg,field, &value), ret);
        if(value & 0x100)
        {
            index = value & 0xff;
            diag_util_printf("classification hit entry: %d\n",index);
        }
        else
        {
            diag_util_printf("classification hit entry: none\n");
        }
    break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
    case RTL9602C_CHIP_ID:

        /*For CF pattern 0 hit check*/
        diag_util_printf("Pattern 0: \n");
        diag_util_printf(" Action      Hit Index\n");
        arraySize = 6;
        for(arrayIndex = 0; arrayIndex<arraySize; arrayIndex++)
        {
            DIAG_UTIL_ERR_CHK(reg_array_field_read(RTL9602C_STAT_CF_PTN0_REASONr, portIndex, arrayIndex, RTL9602C_CF_PTN0_HIT_INFOf, &value), ret);

            diag_util_printf(" %-11s %-3s %-d\n", diagStr_cfActBit[arrayIndex], (value & 0x100)? "yes":"no", value & 0xff);
        }

        /*For CF pattern 1 hit check*/
        diag_util_printf("Pattern 1: \n");
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_STAT_CF_REASONr, RTL9602C_CF_HIT_INFOf, &value), ret);
        if(value & 0x100)
        {
            index = value & 0xff;
            diag_util_printf(" Hit Entry: %d\n",index);
        }
        else
        {
            diag_util_printf(" Hit Entry: none\n");
        }
    break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
    case RTL9607C_CHIP_ID:     
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_STAT_CF_REASONr, RTL9607C_CF_HIT_INFOf, &value), ret);
        if(value & 0x100)
        {
            index = value & 0xff;
            diag_util_printf(" Hit Entry: %d\n",index);
        }
        else
        {
            diag_util_printf(" Hit Entry: none\n");
        }
    break;
#endif
    default:
        diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
    break;
    }



        return CPARSER_OK;
}    /* end of cparser_cmd_debug_classf_get_hit_entry */



/*
 * debug get log
 */
cparser_result_t cparser_cmd_debug_get_log(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  data = 0;
    uint64  data64 = 0;
    uint32  i = 0;
    uint32  log_type = LOG_TYPE_DEFAULT;
    int32   ret = RT_ERR_FAILED;

    char *pLevelName[] = {
        "fatal", "major", "minor", "warning", "event", "info",
        "func", "debug", "trace", ""
    };

    char *pModName[] = {
        STR_MOD_GENERAL, STR_MOD_DOT1X, STR_MOD_FILTER, STR_MOD_FLOWCTRL,
        STR_MOD_INIT, STR_MOD_L2, STR_MOD_MIRROR, STR_MOD_NIC, STR_MOD_PORT,
        STR_MOD_QOS, STR_MOD_RATE, STR_MOD_STAT, STR_MOD_STP, STR_MOD_SVLAN,
        STR_MOD_SWITCH, STR_MOD_TRAP, STR_MOD_TRUNK, STR_MOD_VLAN, STR_MOD_ACL,
        STR_MOD_HAL, STR_MOD_DAL, STR_MOD_RTDRV, STR_MOD_RTUSR, STR_MOD_DIAGSHELL,
        STR_MOD_UNITTEST, STR_MOD_OAM, STR_MOD_L3, STR_MOD_RTCORE, STR_MOD_EEE,
        STR_MOD_SEC, STR_MOD_LED, STR_MOD_RSVD_001, STR_MOD_RSVD_002, STR_MOD_RSVD_003,
        STR_MOD_L34, STR_MOD_GPON, STR_MOD_EPON, STR_MOD_HWMISC,STR_MOD_CPU,
        STR_MOD_INTR, STR_MOD_PONMAC, STR_MOD_RLDP, STR_MOD_CLASSIFY,STR_MOD_CLASSIFY,
        STR_MOD_PTP, STR_MOD_PPSTOD, STR_MOD_PBO, STR_MOD_MDIO,         
        STR_MOD_END
    };

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_log_enable_get(&data), ret);
    if (data < RTK_ENABLE_END)
        diag_util_printf("    status      : %s \n", data ? "ENABLE" : "DISABLE");
    else
        diag_util_printf("    status      : ERROR \n");

    DIAG_UTIL_ERR_CHK(rt_log_type_get(&log_type), ret);
    if (log_type < LOG_TYPE_END)
        diag_util_printf("    type        : %s \n", log_type ? "LEVEL-MASK" : "LEVEL");
    else
        diag_util_printf("    type        : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_level_get(&data), ret);
    if (data < LOG_LV_END)
    {
        if (LOG_MSG_OFF == data)
        {
            diag_util_printf("    level       : Message off ");
        }
        else
        {
            diag_util_printf("    level       : %d ", data);
        }
        if (LOG_TYPE_LEVEL == log_type)
            diag_util_printf("(*)");
        diag_util_printf("\n");
    }
    else
        diag_util_printf("    level       : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_mask_get(&data), ret);
    if (data <= LOG_MASK_ALL)
    {
        diag_util_printf("    level-mask  : ");
        if (data)
        {
            for (i = 0; i < LOG_MSG_OFF; i++)
            {
                if ((data >> i) & 0x1)
                    diag_util_printf("%s ", *(pLevelName + i));
            }
        }
        else
            diag_util_printf("ALL_MSG_OFF");

        if (LOG_TYPE_MASK == log_type)
            diag_util_printf("(*)");
        diag_util_printf("\n");
    }
    else
        diag_util_printf("    level-mask  : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_format_get(&data), ret);
    if (data < LOG_FORMAT_END)
        diag_util_printf("    format      : %s \n", data ? "DETAILED" : "NORMAL");
    else
        diag_util_printf("    format      : ERROR \n");

    data64 = 0;
    DIAG_UTIL_ERR_CHK(rt_log_moduleMask_get(&data64), ret);
    if (data64 <= MOD_ALL)
    {
        diag_util_printf("    module-mask : ");
        if (data64)
        {
            for (i = 0; i < SDK_MOD_END; i++)
            {
                if ((data64 >> i) & 0x1)
                    diag_util_printf("%s ", *(pModName + i));
            }
        }
        else
            diag_util_printf("ALL_MODULE_OFF");
        diag_util_printf("\n\n");
    }
    else
        diag_util_printf("    module-mask : ERROR \n");
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_get_log */

/*
 * debug get memory <UINT:address> { <UINT:words> }
 */
cparser_result_t cparser_cmd_debug_get_memory_address_words(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *words_ptr)
{
    uint32  mem = 0;
    uint32  value = 0;
    uint32  mem_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }


    if ('\0' == TOKEN_CHAR(4,0))
    {

        DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &value), ret);
        diag_util_mprintf("Memory 0x%x : 0x%08x\n", mem, value);
    }
    else
    {
        if(NULL != words_ptr)
        {
            mem_words = *words_ptr;
        }
        for (index = 0; index < mem_words; index++)
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &value), ret);
            if (0 == (index % 4))
            {
                diag_util_mprintf("\n");
                diag_util_printf("0x%08x ", mem);
            }
            diag_util_printf("0x%08x ", value);
            mem = mem + 4;
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_debug_get_memory_address_words */

/*
 * debug set log state ( disable | enable )
 */
cparser_result_t cparser_cmd_debug_set_log_state_disable_enable(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_enable_set(ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_enable_set(DISABLED), ret);
    }
    else {}
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_state_disable_enable */

/*
 * debug set log level <UINT:value>
 */
cparser_result_t cparser_cmd_debug_set_log_level_value(cparser_context_t *context,
    uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  log_level = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    log_level = *value_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_level_set(log_level), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_value */

/*
 * debug set log level-mask <UINT:bitmask>
 */
cparser_result_t cparser_cmd_debug_set_log_level_mask_bitmask(cparser_context_t *context,
    uint32_t *bitmask_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  log_level_mask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    log_level_mask = *bitmask_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_mask_set(log_level_mask), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_mask_bitmask */

/*
 * debug set log level-type ( level | level-mask )
 */
cparser_result_t cparser_cmd_debug_set_log_level_type_level_level_mask(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if (strlen(TOKEN_STR(4)) == strlen("level"))
    {
        DIAG_UTIL_ERR_CHK(rt_log_type_set(LOG_TYPE_LEVEL), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rt_log_type_set(LOG_TYPE_MASK), ret);
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_type_level_level_mask */

/*
 * debug set log format ( normal | detail )
 */
cparser_result_t cparser_cmd_debug_set_log_format_normal_detail(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_format_set(LOG_FORMAT_DETAILED), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rt_log_format_set(LOG_FORMAT_NORMAL), ret);
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_format_normal_detail */

/*
 * debug set log module <UINT64:bitmask>
 */
cparser_result_t cparser_cmd_debug_set_log_module_bitmask(cparser_context_t *context,
    uint64_t *bitmask_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint64  log_module_mask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    log_module_mask = *bitmask_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_moduleMask_set(log_module_mask), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_module_bitmask */

/*
 * debug set memory <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_debug_set_memory_address_value(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *value_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    mem = *address_ptr;
    value = *value_ptr;

    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(ioal_mem32_write(mem, value), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_memory_address_value */

/*
 * debug set soc-memory <UINT:address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_soc_memory_address_value(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *value_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    mem = *address_ptr;
    value = *value_ptr;

    if(((mem&0xff000000) != 0xB8000000) && ((mem&0xff000000) != 0xBB000000))
    {
        diag_util_printf("\n\rInput error! The address base must be 0xB8000000 or 0xBB000000.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

#if defined(RTL_CYGWIN_EMULATE) && !defined(CONFIG_SDK_MODEL_MODE_USER)
    {
        diag_util_printf("\n\rsoc memory access not support in this mode.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif

#if 0
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif
    DIAG_UTIL_ERR_CHK(ioal_socMem32_write(mem, value), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_soc_memory_address_value */

/*
 * debug get soc-memory <UINT:address> { <UINT:words> }
 */
cparser_result_t
cparser_cmd_debug_get_soc_memory_address_words(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *words_ptr)
{
    uint32  mem = 0;
    uint32  value = 0;
    uint32  mem_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;

    if(((mem&0xff000000) != 0xB8000000) && ((mem&0xff000000) != 0xBB000000))
    {
        diag_util_printf("\n\rInput error! The address base must be 0xB8000000 or 0xBB000000.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

#if defined(RTL_CYGWIN_EMULATE) && !defined(CONFIG_SDK_MODEL_MODE_USER)
    {
        diag_util_printf("\n\rsoc memory access not support in this mode.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#else


    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }


    if ('\0' == TOKEN_CHAR(4,0))
    {

        DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &value), ret);
        diag_util_mprintf("Memory 0x%x : 0x%08x\n", mem, value);
    }
    else
    {
        if(NULL != words_ptr)
        {
            mem_words = *words_ptr;
        }
        for (index = 0; index < mem_words; index++)
        {
            DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &value), ret);
            if (0 == (index % 4))
            {
                diag_util_mprintf("\n");
                diag_util_printf("0x%08x ", mem);
            }
            diag_util_printf("0x%08x ", value);
            mem = mem + 4;
        }
        diag_util_mprintf("\n");
    }
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_soc_memory_address_words */

/*
 * debug set pon-nic <UINT:address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_pon_nic_address_value(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *value_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    mem = *address_ptr;
    value = *value_ptr;

#if defined(RTL_CYGWIN_EMULATE) && !defined(CONFIG_SDK_MODEL_MODE_USER)
    {
        diag_util_printf("\n\rsystem memory access not support in this mode.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif

#if 0
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif
    DIAG_UTIL_ERR_CHK(ioal_ponNic_write(mem, value), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_pon_nic_address_value */

/*
 * debug get pon-nic <UINT:address> { <UINT:words> }
 */
cparser_result_t
cparser_cmd_debug_get_pon_nic_address_words(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *words_ptr)
{
    uint32  mem = 0;
    uint32  value = 0;
    uint32  mem_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;

#if defined(RTL_CYGWIN_EMULATE) && !defined(CONFIG_SDK_MODEL_MODE_USER)
    {
        diag_util_printf("\n\rsystem memory access not support in this mode.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#else


    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }


    if ('\0' == TOKEN_CHAR(4,0))
    {

        DIAG_UTIL_ERR_CHK(ioal_ponNic_read(mem, &value), ret);
        diag_util_mprintf("Memory 0x%x : 0x%08x\n", mem, value);
    }
    else
    {
        if(NULL != words_ptr) 
        {
            mem_words = *words_ptr;
        }
        for (index = 0; index < mem_words; index++)
        {
            DIAG_UTIL_ERR_CHK(ioal_ponNic_read(mem, &value), ret);
            if (0 == (index % 4))
            {
                diag_util_mprintf("\n");
                diag_util_printf("0x%08x ", mem);
            }
            diag_util_printf("0x%08x ", value);
            mem = mem + 4;
        }
        diag_util_mprintf("\n");
    }
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_pon_nic_address_words */

typedef union debug_batch_cmd_u
{
    /* A type operation - loop */
    struct
    {
        uint32 count;   /* loop count */
        uint16 pos;     /* Loop command apear pos */
    } loop;
    /* W type operation - write */
    struct
    {
        uint8 reg;      /* target register */
        uint8 msb;      /* msb of operation */
        uint8 lsb;      /* lsb of operation */
        uint16 page;    /* page to do the operation */
        uint16  data;   /* data for operation */
    } write;
    /* R type operation - read */
    struct
    {
        uint8 reg;      /* target register */
        uint8 msb;      /* msb of operation */
        uint8 lsb;      /* lsb of operation */
        uint16 page;    /* page to do the operation */
    } read;
} debug_batch_cmd_t;

typedef struct debug_batch_rec_s
{
    uint8 cmdType;
    debug_batch_cmd_t cmd;
    struct debug_batch_rec_s *pNext;
} debug_batch_rec_t;

#define DEBUG_BATCH_OP_LOOP     0
#define DEBUG_BATCH_OP_WRITE    1
#define DEBUG_BATCH_OP_READ     2
#define DEBUG_BATCH_OP_PHY      3
#define DEBUG_BATCH_OP_LIST     4
#define DEBUG_BATCH_OP_EXECUTE  5

static uint8 _diag_debug_batch_print = 0;
static uint16 phyId = 0;
static debug_batch_rec_t *pListHead = NULL;
static debug_batch_rec_t *pListTail = NULL;

static int32
_diag_debug_batch_parse_cmd(
    uint8 opType,
    uint32_t var1,
    uint32_t var2,
    uint32_t var3,
    uint32_t data)
{
    int32 ret;
    debug_batch_rec_t *pRec = NULL;

    if(_diag_debug_batch_print != 0) diag_util_printf("read cmds:\n");

    pRec = (debug_batch_rec_t *)osal_alloc(sizeof(debug_batch_rec_t));
    if(NULL == pRec)
    {
        diag_util_printf("%s:%d allocate fail\n", __FUNCTION__, __LINE__);
        goto parser_panic;
    }
    memset(pRec, 0, sizeof(debug_batch_rec_t));

    if(NULL == pListHead)
    {
        pListHead = pRec;
        pListTail = pRec;
    }
    else
    {
        pListTail->pNext = pRec;
        pListTail = pRec;
    }

    /* should be the cmd character */
    switch(opType)
    {
    case DEBUG_BATCH_OP_LOOP:
        pRec->cmdType = DEBUG_BATCH_OP_LOOP;
        pRec->cmd.loop.count = var1;

        if(_diag_debug_batch_print != 0) diag_util_printf("A %d\n", var1);
        break;

    case DEBUG_BATCH_OP_WRITE:
        pRec->cmdType = DEBUG_BATCH_OP_WRITE;

        if(var3 > var2)
        {
            diag_util_printf("lsb > msb\n");
            goto parser_panic;
        }
        pRec->cmd.write.reg = var1;
        pRec->cmd.write.msb = var2;
        pRec->cmd.write.lsb = var3;
        pRec->cmd.write.data = data;

        if(var1 <= 7)
            pRec->cmd.write.page = 0x0a40;
        else if(var1 <= 15)
            pRec->cmd.write.page = 0x0a41;
        else if(var1 <= 23)
            pRec->cmd.write.page = 0x0a42;
        else if(var1 <= 31)
            pRec->cmd.write.page = 0x0a43;

        if(_diag_debug_batch_print != 0)
        {
            diag_util_printf("W %2d(0x%04x) %2d %2d 0x%x\n",
            var1, pRec->cmd.write.page, var2, var3, data);
        }
        break;

    case DEBUG_BATCH_OP_READ:
        pRec->cmdType = DEBUG_BATCH_OP_READ;

        if(var3 > var2)
        {
            diag_util_printf("lsb > msb\n");
            goto parser_panic;
        }
        pRec->cmd.read.reg = var1;
        pRec->cmd.read.msb = var2;
        pRec->cmd.read.lsb = var3;

        if(var1 <= 7)
            pRec->cmd.read.page = 0x0a40;
        else if(var1 <= 15)
            pRec->cmd.read.page = 0x0a41;
        else if(var1 <= 23)
            pRec->cmd.read.page = 0x0a42;
        else if(var1 <= 31)
            pRec->cmd.read.page = 0x0a43;

        if(_diag_debug_batch_print != 0)
        {
            diag_util_printf("R %2d(0x%04x) %2d %2d\n",
            var1, pRec->cmd.read.page, var2, var3);
        }
        break;

    default:
        diag_util_printf("parser panic for unknown operation %d\n", opType);
        goto parser_panic;
    }

    return CPARSER_OK;

parser_panic:
    return CPARSER_NOT_OK;
} /* end of _diag_debug_batch_parse_cmd */

static int32
_diag_debug_batch_analysis()
{
    uint16 lastPage = 0x0a42;
    debug_batch_rec_t *pCurr = pListHead;

    /* Due to Apollo need additional page switch
     * This function analysis the list and set page to the command
     */
    if(_diag_debug_batch_print != 0) diag_util_printf("analyzed cmds:\n");
    while(pCurr != NULL)
    {
        switch(pCurr->cmdType)
        {
        case DEBUG_BATCH_OP_LOOP:
            /* Nothing to do with the loop operation */
            if(_diag_debug_batch_print != 0)
                diag_util_printf("A %d\n", pCurr->cmd.loop.count);
            break;
        case DEBUG_BATCH_OP_WRITE:
            /* Write to page switch register */
            if(31 == pCurr->cmd.write.reg)
            {
                /* Update last switched page */
                lastPage = pCurr->cmd.write.data;
            }
            else if(pCurr->cmd.write.reg >= 16 && pCurr->cmd.write.reg <= 23 )
            {
                /* Update write page */
                pCurr->cmd.write.page = lastPage;
            }
            if(_diag_debug_batch_print != 0)
            {
                diag_util_printf("W %2d(0x%04x) %2d %2d 0x%x\n",
                                 pCurr->cmd.write.reg,
                                 pCurr->cmd.write.page,
                                 pCurr->cmd.write.msb,
                                 pCurr->cmd.write.lsb,
                                 pCurr->cmd.write.data);
            }
            break;
        case DEBUG_BATCH_OP_READ:
            if(pCurr->cmd.read.reg >= 16 && pCurr->cmd.read.reg <= 23 )
            {
                /* Update read page */
                pCurr->cmd.read.page = lastPage;
            }
            if(_diag_debug_batch_print != 0)
            {
                diag_util_printf("R %2d(0x%04x) %2d %2d\n",
                                 pCurr->cmd.read.reg,
                                 pCurr->cmd.read.page,
                                 pCurr->cmd.read.msb,
                                 pCurr->cmd.read.lsb);
            }
            break;
        default:
            diag_util_printf("Unknown operation type %d!\n", pCurr->cmdType);
            return CPARSER_NOT_OK;
        }

        pCurr = pCurr->pNext;
    }

    return CPARSER_OK;
}/* _diag_debug_batch_analysis */

static int32
_diag_debug_batch_assemble_writedata(
    uint16 readData,
    uint16 msb,
    uint16 lsb,
    uint16 writeData,
    uint16 *outData)
{
    uint16 i;
    uint16 probe;
    uint16 temp = readData;

    for(i = lsb, probe = 0x01; i <= msb; i++, probe = probe << 1)
    {
        if((writeData & probe) == probe)
            temp = (temp | (0x01 << i));
        else
            temp = (temp & (~(0x01 << i)));
    }

    *outData = temp;

    return CPARSER_OK;
}/* _diag_debug_batch_assemble_writedata */

static int32
_diag_debug_batch_assemble_readdata(
    uint16 readData,
    uint16 msb,
    uint16 lsb,
    uint16 *outData)
{
    uint16 temp = readData;
    uint16 dataSize = sizeof(uint16) * 8 - 1;

    temp = temp << (dataSize - msb);
    temp = temp >> ((dataSize - msb) + lsb);
    /* Shift the result to lsb according to requirement
     */
    /* temp = temp << lsb; */

    *outData = temp;

    return CPARSER_OK;
} /* _diag_debug_batch_assemble_readdata */

static int32
_diag_debug_batch_execute()
{
    int32 ret;
    uint32 i = 1;
    uint16 readData;
    uint16 writeData;
    debug_batch_rec_t *pHead = pListHead;
    debug_batch_rec_t *pCurr = pListHead;

    if(NULL == pCurr)
        return CPARSER_NOT_OK;

    /* Set loop execution */
    if(_diag_debug_batch_print != 0) diag_util_printf("Execute cmds:\n");
    diag_util_printf("results:\n");
    if(pCurr->cmdType == DEBUG_BATCH_OP_LOOP)
    {
        i = pCurr->cmd.loop.count;
        pCurr = pCurr->pNext;
        pHead = pCurr;
    }

    for(; i > 0; i--)
    {
        pCurr = pHead;
        while(pCurr != NULL)
        {
            switch(pCurr->cmdType)
            {
            case DEBUG_BATCH_OP_WRITE:
                ret = _diag_debug_internal_phy_read(phyId, pCurr->cmd.write.page,
                                           pCurr->cmd.write.reg, &readData);
                if(ret != RT_ERR_OK)
                {
                    diag_util_printf("MDIO read failed!\n");
                    return CPARSER_NOT_OK;
                }
                if(_diag_debug_batch_print != 0)
                {
                    diag_util_printf("W %2d(0x%04x) %2d %2d 0x%04x, 0x%04x --> ",
                                     pCurr->cmd.write.reg,
                                     pCurr->cmd.write.page,
                                     pCurr->cmd.write.msb,
                                     pCurr->cmd.write.lsb,
                                     pCurr->cmd.write.data,
                                     readData);
                }

                _diag_debug_batch_assemble_writedata(readData,
                                                pCurr->cmd.write.msb,
                                                pCurr->cmd.write.lsb,
                                                pCurr->cmd.write.data,
                                                &writeData);
                ret = _diag_debug_internal_phy_write(phyId, pCurr->cmd.write.page,
                                            pCurr->cmd.write.reg,
                                            writeData);
                if(ret != RT_ERR_OK)
                {
                    diag_util_printf("MDIO write failed!\n");
                    return CPARSER_NOT_OK;
                }
                if(_diag_debug_batch_print != 0)
                    diag_util_printf("0x%04x\n", writeData);
                break;
            case DEBUG_BATCH_OP_READ:
                ret = _diag_debug_internal_phy_read(phyId, pCurr->cmd.read.page,
                                           pCurr->cmd.read.reg, &readData);
                if(ret != RT_ERR_OK)
                {
                    diag_util_printf("MDIO read failed!\n");
                    return CPARSER_NOT_OK;
                }
                if(_diag_debug_batch_print != 0)
                {
                    diag_util_printf("R %2d(0x%04x) %2d %2d = 0x%04x\n",
                                     pCurr->cmd.read.reg,
                                     pCurr->cmd.read.page,
                                     pCurr->cmd.read.msb,
                                     pCurr->cmd.read.lsb,
                                     readData);
                }
                _diag_debug_batch_assemble_readdata(readData,
                                                pCurr->cmd.read.msb,
                                                pCurr->cmd.read.lsb,
                                                &writeData);
                diag_util_printf("0x%04x\n", writeData);
                break;
            case DEBUG_BATCH_OP_LOOP:
            default:
                /* What happens? Skip the command */
                break;
            }

            pCurr = pCurr->pNext;
        }
    }

    return CPARSER_OK;
}/* _diag_debug_batch_execute */

cparser_result_t
_cparser_cmd_debug_batch_loop(
    cparser_context_t *context,
    uint32_t  *loop_ptr)
{
    int32 ret;

    ret = _diag_debug_batch_parse_cmd(DEBUG_BATCH_OP_LOOP,
                                      *loop_ptr,
                                      0,
                                      0,
                                      0);
    if(ret != CPARSER_OK)
    {
        diag_util_printf("parse cmd failed\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}/* end of _cparser_cmd_debug_batch_loop */

cparser_result_t
_cparser_cmd_debug_batch_write(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr,
    uint32_t  *data_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    ret = _diag_debug_batch_parse_cmd(DEBUG_BATCH_OP_WRITE,
                                      *reg_ptr,
                                      *msb_ptr,
                                      *lsb_ptr,
                                      *data_ptr);
    if(ret != CPARSER_OK)
    {
        diag_util_printf("parse cmd failed\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of _cparser_cmd_debug_batch_write */

cparser_result_t
_cparser_cmd_debug_batch_read(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    ret = _diag_debug_batch_parse_cmd(DEBUG_BATCH_OP_READ,
                                      *reg_ptr,
                                      *msb_ptr,
                                      *lsb_ptr,
                                      0);
    if(ret != CPARSER_OK)
    {
        diag_util_printf("parse cmd failed\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of _cparser_cmd_debug_batch_read */

/*
 * debug batch a <UINT:loop>
 */
cparser_result_t
cparser_cmd_debug_batch_a_loop(
    cparser_context_t *context,
    uint32_t  *loop_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return _cparser_cmd_debug_batch_loop(context, loop_ptr);
}    /* end of cparser_cmd_debug_batch_a_a_loop */

/*
 * debug batch A <UINT:loop>
 */
cparser_result_t
cparser_cmd_debug_batch_A_loop(
    cparser_context_t *context,
    uint32_t  *loop_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return _cparser_cmd_debug_batch_loop(context, loop_ptr);
}    /* end of cparser_cmd_debug_batch_a_a_loop */

/*
 * debug batch w <UINT:reg> <UINT:msb> <UINT:lsb> <HEX:data>
 */
cparser_result_t
cparser_cmd_debug_batch_w_reg_msb_lsb_data(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_write(context, reg_ptr, msb_ptr, lsb_ptr, data_ptr);
}    /* end of cparser_cmd_debug_batch_w_w_reg_msb_lsb_data */

/*
 * debug batch W <UINT:reg> <UINT:msb> <UINT:lsb> <HEX:data>
 */
cparser_result_t
cparser_cmd_debug_batch_W_reg_msb_lsb_data(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_write(context, reg_ptr, msb_ptr, lsb_ptr, data_ptr);
}    /* end of cparser_cmd_debug_batch_w_w_reg_msb_lsb_data */

/*
 * debug batch r <UINT:reg> <UINT:msb> <UINT:lsb>
 */
cparser_result_t
cparser_cmd_debug_batch_r_reg_msb_lsb(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_read(context, reg_ptr, msb_ptr, lsb_ptr);
}    /* end of cparser_cmd_debug_batch_r_r_reg_msb_lsb */

/*
 * debug batch R <UINT:reg> <UINT:msb> <UINT:lsb>
 */
cparser_result_t
cparser_cmd_debug_batch_R_reg_msb_lsb(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_read(context, reg_ptr, msb_ptr, lsb_ptr);
}    /* end of cparser_cmd_debug_batch_r_r_reg_msb_lsb */

/*
 * debug batch phy <UINT:phyid>
 */
cparser_result_t
cparser_cmd_debug_batch_phy_phyid(
    cparser_context_t *context,
    uint32_t  *phyid_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    phyId = *phyid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_batch_p_p_phyid */

/*
 * debug batch list
 */
cparser_result_t
cparser_cmd_debug_batch_list(
    cparser_context_t *context)
{
    debug_batch_rec_t *pPtr;

    DIAG_UTIL_PARAM_CHK();

    pPtr = pListHead;
    while(pPtr != NULL)
    {
        switch(pPtr->cmdType)
        {
        case DEBUG_BATCH_OP_LOOP:
            diag_util_printf("A %d\n", pPtr->cmd.loop.count);
            break;
        case DEBUG_BATCH_OP_WRITE:
            diag_util_printf("W %2d(0x%04x) %2d %2d 0x%04x\n",
                             pPtr->cmd.write.reg,
                             pPtr->cmd.write.page,
                             pPtr->cmd.write.msb,
                             pPtr->cmd.write.lsb,
                             pPtr->cmd.write.data);
            break;
        case DEBUG_BATCH_OP_READ:
            diag_util_printf("R %2d(0x%04x) %2d %2d\n",
                             pPtr->cmd.read.reg,
                             pPtr->cmd.read.page,
                             pPtr->cmd.read.msb,
                             pPtr->cmd.read.lsb);
            break;
        }

        pPtr = pPtr->pNext;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_batch_l_l */

/*
 * debug batch execute { debug }
 */
cparser_result_t
cparser_cmd_debug_batch_execute_debug(
    cparser_context_t *context)
{
    int32 ret;
    debug_batch_rec_t *pPtr;

    DIAG_UTIL_PARAM_CHK();

    if(4 == TOKEN_NUM() &&
       0 == strcmp(TOKEN_STR(3), "debug"))
    {
        _diag_debug_batch_print = 1;
    }

    ret = _diag_debug_batch_analysis();
    if(ret != CPARSER_OK)
    {
        diag_util_printf("analysis cmd failed\n");
        _diag_debug_batch_print = 0;
        return CPARSER_NOT_OK;
    }

    ret = _diag_debug_batch_execute();
    if(ret != CPARSER_OK)
    {
        diag_util_printf("execute cmd failed\n");
        _diag_debug_batch_print = 0;
        return CPARSER_NOT_OK;
    }

    /* Clean up the mess */
    while(pListHead != NULL)
    {
        pPtr = pListHead;
        pListHead = pListHead->pNext;
        pPtr->pNext = NULL;
        osal_free(pPtr);
    }
    pListHead = NULL;
    pListTail = NULL;

    _diag_debug_batch_print = 0;
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_batch_e_e_debug */

/*
 * debug dump ( hsa | hsb ) snapshot
 */
cparser_result_t
cparser_cmd_debug_dump_hsa_hsb_snapshot(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    rtk_hsb_t hsbData;
    rtk_hsa_t hsaData;
    uint32 mode;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            mode = 1; /*none*/
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &mode), ret);
            mode = 3; /*first-pass*/
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &mode), ret);
             
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            break;
#endif

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            mode = 1; /*none*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &mode), ret);
            mode = 3; /*first-pass*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &mode), ret);

            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            mode = 1; /*none*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_MODEf, &mode), ret);
            mode = 3; /*first-pass*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_MODEf, &mode), ret);

            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_hsbData_get(&hsbData), ret);

                DIAG_UTIL_ERR_CHK(rtl9602c_hsbPar_get(&hsbData), ret);

                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            mode = 1; /*none*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_MODEf, &mode), ret);
            mode = 3; /*first-pass*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_MODEf, &mode), ret);

            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_hsbData_get(&hsbData), ret);
				DIAG_UTIL_ERR_CHK(rtl9607c_hsbPar_get(&hsbData), ret);

                _diag_debug_hsb_display(&hsbData);

            }                   
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);


			}										
			break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            mode = 1; /*none*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_MODEf, &mode), ret);
            mode = 3; /*first-pass*/
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_MODEf, &mode), ret);

            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_hsbData_get(&hsbData), ret);
				DIAG_UTIL_ERR_CHK(rtl9603cvd_hsbPar_get(&hsbData), ret);

                _diag_debug_hsb_display(&hsbData);

            }                   
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);


			}										
			break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsa_hsb_snapshot */


/*
 * debug dump ( fb | hsa | hsb | hsd )
 */
cparser_result_t
cparser_cmd_debug_dump_fb_hsa_hsb_hsd(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    rtk_hsb_t hsbData;
    rtk_hsa_t hsaData;
    rtk_hsa_debug_t hsdData;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_hsdData_get(&hsdData), ret);
                _diag_debug_hsd_display(&hsdData);
            }
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsdData_get(&hsdData), ret);
                _diag_debug_hsd_display(&hsdData);
            }
            break;
#endif

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
            {
                DIAG_UTIL_ERR_CHK(rtl9601b_hsdData_get(&hsdData), ret);
                _diag_debug_hsd_display(&hsdData);
            }
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_hsbData_get(&hsbData), ret);

                DIAG_UTIL_ERR_CHK(rtl9602c_hsbPar_get(&hsbData), ret);

                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
            {
                DIAG_UTIL_ERR_CHK(rtl9602c_hsdData_get(&hsdData), ret);
                _diag_debug_hsd_display(&hsdData);
            }
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_hsbData_get(&hsbData), ret);
				DIAG_UTIL_ERR_CHK(rtl9607c_hsbPar_get(&hsbData), ret);

                _diag_debug_hsb_display(&hsbData);

            }                   
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9607c_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);


			}			
			else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
			{
				DIAG_UTIL_ERR_CHK(rtl9607c_hsdData_get(&hsdData), ret);
				_diag_debug_hsd_display(&hsdData);
			}
			else if(!osal_strcmp(TOKEN_STR(2),"fb"))
			{
				DIAG_UTIL_ERR_CHK(rtl9607c_fbData_get(&hsaData), ret);
				 _diag_debug_hsa_display(&hsaData);
			}							
			break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_hsbData_get(&hsbData), ret);
				DIAG_UTIL_ERR_CHK(rtl9603cvd_hsbPar_get(&hsbData), ret);

                _diag_debug_hsb_display(&hsbData);

            }                   
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(rtl9603cvd_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);


			}			
			else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
			{
				DIAG_UTIL_ERR_CHK(rtl9603cvd_hsdData_get(&hsdData), ret);
				_diag_debug_hsd_display(&hsdData);
			}
			else if(!osal_strcmp(TOKEN_STR(2),"fb"))
			{
				DIAG_UTIL_ERR_CHK(rtl9603cvd_fbData_get(&hsaData), ret);
				 _diag_debug_hsa_display(&hsaData);
			}							
			break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_fb_hsa_hsb_hsd */

/*
 * debug dump hsd port <UINT:port>
 */
cparser_result_t
cparser_cmd_debug_dump_hsd_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    uint32 readData;
    uint32 writeData;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*port_ptr >= 7), CPARSER_ERR_INVALID_PARAMS);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            reg_array_read(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &readData);

            writeData = (readData & 0xFFFFFF1F) | (*port_ptr<<5);

            reg_array_write(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &writeData);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            writeData = *port_ptr;
            reg_field_write(APOLLOMP_DBG_EP_CFGr,APOLLOMP_DBG_EPf, &writeData);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_dump_hsd_port_port */

/*
 * debug dump hsd latest
 */
cparser_result_t
cparser_cmd_debug_dump_hsd_latest(
    cparser_context_t *context)
{
    uint32 readData;
    uint32 writeData;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            reg_array_read(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &readData);

            writeData = (readData & 0xFFFFFF1F) | (7<<5);

            reg_array_write(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &writeData);

            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            writeData = 7;/*lasted is port 7*/
            reg_field_write(APOLLOMP_DBG_EP_CFGr,APOLLOMP_DBG_EPf, &writeData);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_dump_hsd_latest */


cparser_result_t cparser_cmd_debug_get_table_table_idx_address(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
#if defined(CONFIG_LUNA_G3_SERIES)
    //LUNA G3 not support table read
    return CPARSER_OK;
#else
    uint32      loop;
    int32       ret = RT_ERR_FAILED;
    uint32      value[20];

    DIAG_UTIL_PARAM_CHK();
    if ((ret = table_read(*table_idx_ptr, *address_ptr, value)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Table %u, address %u\n", *table_idx_ptr, *address_ptr);

    for (loop = 0; loop < 20; loop++)
    {
        diag_util_printf("%x-", value[loop]);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
#endif
}


/*
 * debug rtk-init
 */
cparser_result_t cparser_cmd_debug_rtk_init(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_all_module_init(), ret);

    return CPARSER_OK;
}




#ifdef CONFIG_SDK_APOLLOMP
void _apollomp_fpga_debug_phy_write(uint16 phy,uint16 reg,uint16 data)
{
    uint16 inputData;

    inputData = 0xC000 + (phy<<5) + reg;
    (void)io_mii_phy_reg_write(8,1,data);
    (void)io_mii_phy_reg_write(8,0,inputData);

}


static int32 _diag_debug_apollomp_fpga_init(void)
{
#if defined(FPGA_DEFINED)
    int32 ret;
    rtk_port_t port;
    uint32 data;

    HAL_SCAN_ALL_PORT(port)
    {
        /*PHY 0~6 RXC delay 6ns*/
        _apollomp_fpga_debug_phy_write(port,31,7);
        _apollomp_fpga_debug_phy_write(port,30,160);
        _apollomp_fpga_debug_phy_write(port,28,0x28);

        /*PHY 0~6 Disable PHY EEE*/
        _apollomp_fpga_debug_phy_write(port,0,0x1940);
        _apollomp_fpga_debug_phy_write(port,31,7);
        _apollomp_fpga_debug_phy_write(port,30,32);
        _apollomp_fpga_debug_phy_write(port,27,0x2f0a);
        _apollomp_fpga_debug_phy_write(port,31,0);
    }
    /*initial SerDes*/
    /*fine tune TX 16bits to 20 bits function block 622M sample 155M data position*/
    _apollomp_fpga_debug_phy_write(0x11,0,0x000a); /*force Tx/RX sample clk timing*/
    _apollomp_fpga_debug_phy_write(0x11,1,0x0100); /*fix Fiber TX parameter*/

    /*setting for jitter transfer---*/
    _apollomp_fpga_debug_phy_write(0x1d,0x1a,0x0000); /*RX_filter setting(7:0)*/
    _apollomp_fpga_debug_phy_write(0x1d,0x02,0x2d16); /*kp1==3,ki=1, TX CLK source =RX cdr,disable CMU_TX*/
    _apollomp_fpga_debug_phy_write(0x1d,0x16,0xa8b2); /*RX_KP1_2=3*/
    _apollomp_fpga_debug_phy_write(0x1d,0x03,0x6041); /*kp2=4*/
    _apollomp_fpga_debug_phy_write(0x1d,0x18,0xdde4); /*RX_KP2_2=4*/

    /*set best CMU-RX PLL parameter4*/
    _apollomp_fpga_debug_phy_write(0x1d,0x06,0xf4f0);
    _apollomp_fpga_debug_phy_write(0x1d,0x05,0x4003);
    _apollomp_fpga_debug_phy_write(0x1d,0x0f,0x4fe6); /*TX/RX Io=CML mode*/
    _apollomp_fpga_debug_phy_write(0x10,0x01,0x000c); /*rxd neg edge launch data*/

    HAL_SCAN_ALL_PORT(port)
    {
        _apollomp_fpga_debug_phy_write(port,4,0x05e1);
        _apollomp_fpga_debug_phy_write(port,0,0x1200);
    }

    /*ABLTY_FORCE_MODE to Enable Port 4,5,6*/
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x000120, 0x00000060), ret);
    /*FORCE_P_ABLTY port 5 ext  1000MF*/
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x0000A4, 0x00000076), ret);
    /*FORCE_P_ABLTY port 6 GMAC*/
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x0000A8, 0x00000076), ret);
#if 0
    /*FORCE_P_ABLTY port 4 ext (PON) to link down */
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x000098, 0x00000000), ret);
#endif

    /*set trap-insert-tag disable*/
    data = DISABLED;
    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_MAC_CPU_TAG_CTRLr, APOLLOMP_TRAP_TAGET_INSERT_ENf, &data), ret);

    /*WFQ_IFG = 0 WFQ_BURSTSIZE= 0x3FFF*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_qos_wfqBurstSize_set(0x3FFF), ret);
    DIAG_UTIL_ERR_CHK(apollomp_raw_qos_wfqCalIncIfg_set(APOLLOMP_QOS_IFG_EXCLUDE), ret);
#if 0
    /*QoS init*/
    DIAG_UTIL_ERR_CHK(rtk_qos_init(),ret);

    /*init vlan to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_vlan_init(),ret);

    /*init mib counter to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_stat_init(),ret);

    /*init port module to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_port_init(),ret);

#endif

    /*for fpga verify set forfard to CPU port packet do not keep original,
      set to follow asic decision*/
    data = 0;
    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_L34_GLB_CFGr, APOLLOMP_CFG_CPU_ORG_OPf, &data), ret);



    /*MOCIR_FRC_MD*/
    data = 0xFFFF;
    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_MOCIR_FRC_MDr, APOLLOMP_MOCIR_FRC_MDf, &data), ret);

    /*MOCIR_FRC_VAL*/
    data = 0xFFFF;
    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_MOCIR_FRC_VALr, APOLLOMP_MOCIR_FRC_VALf, &data), ret);


    /*CHIP_INF_SEL */
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x000170, 0x00000002), ret);

#if 0 /* no need to set tcont 1, ponmac_init or ponmac_mode_set will do it */
    /*PON_SCH_QMAP queue 0 map to t-cont 1*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacScheQmap_set(0,0x000000ff), ret);
    /*PON_TCONT_EN  for T-CONT 1*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacTcontEnable_set(0, ENABLED), ret);
    /*PON_QID_PIR_RATE for queue 0*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacPirRate_set(0, 0x1FFFF), ret);
#endif

#endif /*FPGA_DEFINED*/

    return RT_ERR_OK;
}

#endif /*CONFIG_SDK_APOLLOMP*/

#ifdef CONFIG_SDK_RTL9601B
void _rtl9601b_fpga_debug_phy_write(uint16 phy,uint16 reg,uint16 data)
{
    _diag_debug_internal_phy_write(phy, 0, reg, data);
}

static int32 _diag_debug_rtl9601b_fpga_init(void)
{
#if defined(FPGA_DEFINED)
    int32 ret;
    rtk_port_t port;
    uint32 data;

#if 1
    HAL_SCAN_ALL_PORT(port)
    {
        /* Init GPHY */
        _rtl9601b_fpga_debug_phy_write(port + 1, 0, 0x1940);
        _rtl9601b_fpga_debug_phy_write(port + 1, 4, 0x0de1);
        _rtl9601b_fpga_debug_phy_write(port + 1, 31, 7);
        _rtl9601b_fpga_debug_phy_write(port + 1, 30, 160);
        _rtl9601b_fpga_debug_phy_write(port + 1, 28, 0x28);
        _rtl9601b_fpga_debug_phy_write(port + 1, 30, 32);
        _rtl9601b_fpga_debug_phy_write(port + 1, 27, 0x2f0a);
        _rtl9601b_fpga_debug_phy_write(port + 1, 31, 0);
        _rtl9601b_fpga_debug_phy_write(port + 1, 0, 0x1140);

    }

    /*initial SerDes*/
    /*fine tune TX 16bits to 20 bits function block 622M sample 155M data position*/
    _rtl9601b_fpga_debug_phy_write(0x11,0,0x000a); /*force Tx/RX sample clk timing*/
    _rtl9601b_fpga_debug_phy_write(0x11,1,0x0100); /*fix Fiber TX parameter*/

    /*setting for jitter transfer---*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x1a,0x0000); /*RX_filter setting(7:0)*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x02,0x2d16); /*kp1==3,ki=1, TX CLK source =RX cdr,disable CMU_TX*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x16,0xa8b2); /*RX_KP1_2=3*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x03,0x6041); /*kp2=4*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x18,0xdde4); /*RX_KP2_2=4*/

    /*set best CMU-RX PLL parameter4*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x06,0xf4f0);
    _rtl9601b_fpga_debug_phy_write(0x1d,0x05,0x4003);
    _rtl9601b_fpga_debug_phy_write(0x1d,0x0f,0x4fe6); /*TX/RX Io=CML mode*/
    #if 0
    _rtl9601b_fpga_debug_phy_write(0x1d,0x0,0x5022); /*set GPON serdes TX internal looback to serdes  RX  (bit8=1)*/
    #endif
    _rtl9601b_fpga_debug_phy_write(0x10,0x01,0x000c); /*rxd neg edge launch data*/

    /*set trap-insert-tag disable*/
    //data = DISABLED;
    //DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_CPU_TAG_CTRLr, RTL9601B_CPU_TAG_INSERT_ENf, &data), ret);

    /*set aware disable*/
    //data = DISABLED;
    //DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_CPU_TAG_CTRLr, RTL9601B_CPU_TAG_AWAREf, &data), ret);

    /*cfg cf permit*/
    data = 1;
    DIAG_UTIL_ERR_CHK(reg_write(RTL9601B_CF_CFGr, &data), ret);

    /*release pon reset*/
    data = 0xe00;
    DIAG_UTIL_ERR_CHK(reg_write(RTL9601B_WSDS_DIG_13r, &data), ret);

    /*patch done*/
    data = 1;
    DIAG_UTIL_ERR_CHK(reg_write(RTL9601B_WRAP_GPHY_MISCr, &data), ret);

    /* MAC force */
    HAL_SCAN_ALL_PORT(port)
    {
        /* GIGA */
        data = 2;
        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9601B_SPEED_ABLTYf, &data), ret);
        /* Full duplex */
        data = 1;
        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9601B_DUPLEX_ABLTYf, &data), ret);
        /* Tx FC */
        data = 0;
        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9601B_TXPAUSE_ABLTYf, &data), ret);
        /* Tx FC */
        data = 0;
        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9601B_RXPAUSE_ABLTYf, &data), ret);
        /* Link status */
        data = 1;
        DIAG_UTIL_ERR_CHK(reg_array_field_write(RTL9601B_FORCE_P_ABLTYr, port, REG_ARRAY_INDEX_NONE, RTL9601B_LINK_ABLTYf, &data), ret);
        /* Enable MAC foce */
        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_ABLTY_FORCE_MODEr, RTL9601B_ABLTY_FORCE_MODEf, &data), ret);
        data |= (0x01 << port);
        DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_ABLTY_FORCE_MODEr, RTL9601B_ABLTY_FORCE_MODEf, &data), ret);
    }
#else
    HAL_SCAN_ALL_PORT(port)
    {
        /*PHY 0~2 RXC delay 6ns*/
        _rtl9601b_fpga_debug_phy_write(port,31,7);
        _rtl9601b_fpga_debug_phy_write(port,30,160);
        _rtl9601b_fpga_debug_phy_write(port,28,0x28);

        /*PHY 0~2 Disable PHY EEE*/
        _rtl9601b_fpga_debug_phy_write(port,0,0x1940);
        _rtl9601b_fpga_debug_phy_write(port,31,7);
        _rtl9601b_fpga_debug_phy_write(port,30,32);
        _rtl9601b_fpga_debug_phy_write(port,27,0x2f0a);
        _rtl9601b_fpga_debug_phy_write(port,31,0);
    }
    /*initial SerDes*/
    /*fine tune TX 16bits to 20 bits function block 622M sample 155M data position*/
    _rtl9601b_fpga_debug_phy_write(0x11,0,0x000a); /*force Tx/RX sample clk timing*/
    _rtl9601b_fpga_debug_phy_write(0x11,1,0x0100); /*fix Fiber TX parameter*/

    /*setting for jitter transfer---*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x1a,0x0000); /*RX_filter setting(7:0)*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x02,0x2d16); /*kp1==3,ki=1, TX CLK source =RX cdr,disable CMU_TX*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x16,0xa8b2); /*RX_KP1_2=3*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x03,0x6041); /*kp2=4*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x18,0xdde4); /*RX_KP2_2=4*/

    /*set best CMU-RX PLL parameter4*/
    _rtl9601b_fpga_debug_phy_write(0x1d,0x06,0xf4f0);
    _rtl9601b_fpga_debug_phy_write(0x1d,0x05,0x4003);
    _rtl9601b_fpga_debug_phy_write(0x1d,0x0f,0x4fe6); /*TX/RX Io=CML mode*/
    _rtl9601b_fpga_debug_phy_write(0x10,0x01,0x000c); /*rxd neg edge launch data*/

    HAL_SCAN_ALL_PORT(port)
    {
        _rtl9601b_fpga_debug_phy_write(port,4,0x05e1);
        _rtl9601b_fpga_debug_phy_write(port,0,0x1200);
    }
#endif

#endif /*FPGA_DEFINED*/

    return RT_ERR_OK;
}

#endif /*CONFIG_SDK_RTL9601B*/


#ifdef CONFIG_SDK_RTL9607C
void _rtl9607c_fpga_debug_phy_write(uint16 phy,uint16 reg,uint16 data)
{
    uint16 inputData;

    inputData = (PATCH_W_OFFSET | ((phy & 0x1F) << 5) | (reg & 0x1F));
    (void)io_mii_phy_reg_write(8,1,data);
    (void)io_mii_phy_reg_write(8,0,inputData);
}

static int32 _diag_debug_rtl9607c_fpga_init(void)
{
#if defined(FPGA_DEFINED)
    int32 ret;
    unsigned int value;
    rtk_port_macAbility_t portAbility;

    /* GPHY port is not equal to MAC port number
     * Actual mapping is
     * Port  0  1  2  3  4  5  8  9  10
     * MAC   0  1  2  3  4  5  8  9  10
     * PHY   0  1  2  3  4  8  6  5  7
     */

    /* Init GPHY */
    /* Remove Giga ability */
    _rtl9607c_fpga_debug_phy_write(0, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(0, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(0, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(1, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(1, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(1, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(2, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(2, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(2, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(3, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(3, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(3, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(4, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(4, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(4, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(8, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(8, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(8, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(6, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(6, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(6, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(5, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(5, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(5, 0, 0x1200);
    _rtl9607c_fpga_debug_phy_write(7, 4, 0x5e1);
    _rtl9607c_fpga_debug_phy_write(7, 9, 0x0);
    _rtl9607c_fpga_debug_phy_write(7, 0, 0x1200);

    /* Disable EEE */
    _rtl9607c_fpga_debug_phy_write(0, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(0, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(1, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(1, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(2, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(2, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(3, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(3, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(4, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(4, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(8, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(8, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(6, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(6, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(5, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(5, 25, 0x843);
    _rtl9607c_fpga_debug_phy_write(7, 31, 0xa43);
    _rtl9607c_fpga_debug_phy_write(7, 25, 0x843);


    /* MAC init */
    /*init port module to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_port_init(),ret);

    /* MAC force for port 5,8,9,10 */
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_get(0, &portAbility), ret);
    portAbility.duplex = PORT_FULL_DUPLEX;
    portAbility.speed = PORT_SPEED_1000M;
    portAbility.txFc = DISABLED;
    portAbility.rxFc = DISABLED;
    portAbility.linkStatus = PORT_LINKUP;
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(5,portAbility), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbilityState_set(5,ENABLED), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(8,portAbility), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbilityState_set(8,ENABLED), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(9,portAbility), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbilityState_set(9,ENABLED), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(10,portAbility), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbilityState_set(10,ENABLED), ret);

    /* Disable CPU tag aware for all GMACs*/
    value = 0;
    DIAG_UTIL_ERR_CHK(reg_write(RTL9607C_CPU_TAG_AWARE_CTRLr, &value), ret);

    //Enable IO_MODE_EN.INTRPT_EN (bit 12: enable interrupt(input) for P1LED1) on 6266
    DIAG_UTIL_ERR_CHK(ioal_socMem32_write(0xbb023018, 0x63020), ret);
#endif /*FPGA_DEFINED*/

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL9607C */

#ifdef CONFIG_SDK_RTL9603CVD
void _rtl9603cvd_fpga_debug_phy_write(uint16 phy,uint16 reg,uint16 data)
{
    uint16 inputData;

    inputData = (PATCH_W_OFFSET | ((phy & 0x1F) << 5) | (reg & 0x1F));
    (void)io_mii_phy_reg_write(8,1,data);
    (void)io_mii_phy_reg_write(8,0,inputData);
}

static int32 _diag_debug_rtl9603cvd_fpga_init(void)
{
#if defined(FPGA_DEFINED)
    int32 ret;
    unsigned int value;
    rtk_port_macAbility_t portAbility;

    /* GPHY port is not equal to MAC port number
     * Actual mapping is
     * Port  0  1  2  3  4  5
     * MAC   0  1  2  3  4  5
     * PHY   0  1  2  3  4  5
     */

    /* Init GPHY */
    /* For 100M ports */
    (void)io_mii_phy_reg_write(0, 15, 0x10);
    (void)io_mii_phy_reg_write(1, 15, 0x10);
    (void)io_mii_phy_reg_write(2, 15, 0x10);
    (void)io_mii_phy_reg_write(3, 15, 0x10);
    (void)io_mii_phy_reg_write(4, 15, 0x10);
    (void)io_mii_phy_reg_write(5, 15, 0x10);

    /* Remove Giga ability */
    _rtl9603cvd_fpga_debug_phy_write(0, 4, 0x5e1);
    _rtl9603cvd_fpga_debug_phy_write(0, 9, 0x0);
    _rtl9603cvd_fpga_debug_phy_write(0, 0, 0x1200);
    _rtl9603cvd_fpga_debug_phy_write(1, 4, 0x5e1);
    _rtl9603cvd_fpga_debug_phy_write(1, 9, 0x0);
    _rtl9603cvd_fpga_debug_phy_write(1, 0, 0x1200);
    _rtl9603cvd_fpga_debug_phy_write(2, 4, 0x5e1);
    _rtl9603cvd_fpga_debug_phy_write(2, 9, 0x0);
    _rtl9603cvd_fpga_debug_phy_write(2, 0, 0x1200);
    _rtl9603cvd_fpga_debug_phy_write(3, 4, 0x5e1);
    _rtl9603cvd_fpga_debug_phy_write(3, 9, 0x200);
    _rtl9603cvd_fpga_debug_phy_write(3, 0, 0x1240);
    _rtl9603cvd_fpga_debug_phy_write(4, 4, 0x5e1);
    _rtl9603cvd_fpga_debug_phy_write(4, 9, 0x200);
    _rtl9603cvd_fpga_debug_phy_write(4, 0, 0x1240);
    _rtl9603cvd_fpga_debug_phy_write(5, 4, 0x5e1);
    _rtl9603cvd_fpga_debug_phy_write(5, 9, 0x200);
    _rtl9603cvd_fpga_debug_phy_write(5, 0, 0x1240);

    /* Disable EEE */
    _rtl9603cvd_fpga_debug_phy_write(0, 31, 0xa43);
    _rtl9603cvd_fpga_debug_phy_write(0, 25, 0x843);
    _rtl9603cvd_fpga_debug_phy_write(1, 31, 0xa43);
    _rtl9603cvd_fpga_debug_phy_write(1, 25, 0x843);
    _rtl9603cvd_fpga_debug_phy_write(2, 31, 0xa43);
    _rtl9603cvd_fpga_debug_phy_write(2, 25, 0x843);
    _rtl9603cvd_fpga_debug_phy_write(3, 31, 0xa43);
    _rtl9603cvd_fpga_debug_phy_write(3, 25, 0x843);
    _rtl9603cvd_fpga_debug_phy_write(4, 31, 0xa43);
    _rtl9603cvd_fpga_debug_phy_write(4, 25, 0x843);
    _rtl9603cvd_fpga_debug_phy_write(5, 31, 0xa43);
    _rtl9603cvd_fpga_debug_phy_write(5, 25, 0x843);

    /* Giga reverset */
    _rtl9603cvd_fpga_debug_phy_write(3, 31, 0x0d08);
    _rtl9603cvd_fpga_debug_phy_write(3, 21, 0x18);
    _rtl9603cvd_fpga_debug_phy_write(3, 31, 0x0000);
    _rtl9603cvd_fpga_debug_phy_write(4, 31, 0x0d08);
    _rtl9603cvd_fpga_debug_phy_write(4, 21, 0x18);
    _rtl9603cvd_fpga_debug_phy_write(4, 31, 0x0000);
    _rtl9603cvd_fpga_debug_phy_write(5, 31, 0x0d08);
    _rtl9603cvd_fpga_debug_phy_write(5, 21, 0x18);
    _rtl9603cvd_fpga_debug_phy_write(5, 31, 0x0000);

    /* MAC init */
    /*init port module to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_port_init(),ret);

    /* MAC force for port 4,5 */
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_get(0, &portAbility), ret);
    portAbility.duplex = PORT_FULL_DUPLEX;
    portAbility.speed = PORT_SPEED_1000M;
    portAbility.txFc = DISABLED;
    portAbility.rxFc = DISABLED;
    portAbility.linkStatus = PORT_LINKUP;
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(4,portAbility), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbilityState_set(4,ENABLED), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbility_set(5,portAbility), ret);
    DIAG_UTIL_ERR_CHK(rtk_port_macForceAbilityState_set(5,ENABLED), ret);

    /* Disable CPU tag aware for all GMACs*/
    value = 0;
    DIAG_UTIL_ERR_CHK(reg_write(RTL9603CVD_CPU_TAG_AWARE_CTRLr, &value), ret);

    osal_time_mdelay(100);
    //Enable IO_MODE_EN.INTRPT_EN (bit 12: enable interrupt(input) for P1LED1) on 6266
    DIAG_UTIL_ERR_CHK(ioal_socMem32_write(0xbb023018, 0x63020), ret);
#endif /*FPGA_DEFINED*/

    return RT_ERR_OK;
}
#endif /* CONFIG_SDK_RTL9603CVD */

/*
 * debug fpga-init
 */
cparser_result_t cparser_cmd_debug_fpga_init(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
    int32 ret = RT_ERR_FAILED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("please use old version diagshell for test chip fpga verify!\n");
            break;
#endif

#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(_diag_debug_apollomp_fpga_init(), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(_diag_debug_rtl9601b_fpga_init(), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(_diag_debug_rtl9607c_fpga_init(), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(_diag_debug_rtl9603cvd_fpga_init(), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}


/*
 * debug set phy <UINT:phy_id> <UINT:reg_address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_phy_phy_id_reg_address_value(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr,
    uint32_t  *value_ptr)
{
    DIAG_UTIL_PARAM_CHK();

#if defined (FPGA_DEFINED)
    /* Note: For RTL9601B, this access is through SPI interface */
    (void)io_mii_phy_reg_write((uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr,(uint16_t)*value_ptr);
#else
    diag_util_mprintf("command not support in this mode!\n");
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_phy_phy_id_reg_address_value */

/*
 * debug get phy <UINT:phy_id> <UINT:reg_address>
 */
cparser_result_t
cparser_cmd_debug_get_phy_phy_id_reg_address(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr)
{
    uint16 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


#if defined (FPGA_DEFINED)
    /* Note: For RTL9601B, this access is through SPI interface */
    (void)io_mii_phy_reg_read((uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr,&data);
    diag_util_mprintf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr, data);
#else
    diag_util_mprintf("command not support in this mode!\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_phy_phy_id_reg_address_address */


/*
 * debug set register dump ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_set_register_dump_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if(TOKEN_STR(4)[0] == 'e')
        ioal_mem32_debugMode_set(ENABLED);
    else
        ioal_mem32_debugMode_set(DISABLED);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_register_dump_enable_disable */



/*display chip sub type*/
int32
cparser_cmd_debug_get_chip_sub_type(void)
{
    int32   ret = RT_ERR_FAILED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
        {
            uint32  value  = 0;

            value = 0xb;
            DIAG_UTIL_ERR_CHK(reg_field_write(BOND_INFOr,BOND_INFO_ENf, &value), ret);
            DIAG_UTIL_ERR_CHK(reg_field_read(BOND_INFOr,BOND_CHIP_MODEf, &value), ret);

            value = value>>2;

            switch(value)
            {
                case 0x4:
                    diag_util_mprintf("Apollo (TQFP176EP)\n");

                    break;
                case 0x1:
                    diag_util_mprintf("Apollo-Pro (LQFP216EP)\n");
                    break;
                case 0x6:
                    diag_util_mprintf("RTL8198B (LQFP256EP)\n");
                    break;
                case 0x7:
                    diag_util_mprintf("LQFP256EP Debug\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode\n");
                    break;
            }

            value = 0x0;
            DIAG_UTIL_ERR_CHK(reg_field_write(BOND_INFOr,BOND_INFO_ENf, &value), ret);
            break;
      }

#endif

#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType), ret); 

            switch(subType)
            {
                case APPOLOMP_CHIP_SUB_TYPE_RTL9601:
                    diag_util_mprintf("RTL9601 (LQFP128)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL9602B:
                    diag_util_mprintf("RTL9602B (LQFP128)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL86906:
                    diag_util_mprintf("RTL86906 (TQFP176EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL9606:
                    diag_util_mprintf("RTL9606 (LQFP216EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL9607:
                    diag_util_mprintf("RTL9607 (LQFP216EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL9602:
                    diag_util_mprintf("RTL9602 (LQFP216EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL9603:
                    diag_util_mprintf("RTL9603 (LQFP216EP)\n");
                    break;

                case APPOLOMP_CHIP_SUB_TYPE_RTL8696:
                    diag_util_mprintf("RTL8696 (LQFP216EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL9607P:
                    diag_util_mprintf("RTL9607P (LQFP256EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_RTL8198B:
                    diag_util_mprintf("RTL8198B (LQFP256EP)\n");
                    break;
                case APPOLOMP_CHIP_SUB_TYPE_2510:
                    diag_util_mprintf("(5VT): 5VT2510\n");
                    break;
                case 0x1F:
                    diag_util_mprintf("LQFP256EP Debug\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif

#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType), ret); 

            switch(subType)
            {
                case RTL9602C_CHIP_SUB_TYPE_RTL9602C_VA4:
                    diag_util_mprintf("RTL9602C-VA4\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9602C_VA5:
                    diag_util_mprintf("RTL9602C-VA5\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601C:
                    diag_util_mprintf("RTL9601C\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601CI_VB3:
                    diag_util_mprintf("RTL9601CI-VB3\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601CI_VB4:
                    diag_util_mprintf("RTL9601CI-VB4\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA2:
                    diag_util_mprintf("RTL9601D-VA2\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA3:
                    diag_util_mprintf("RTL9601D-VA3\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9601D_VA4:
                    diag_util_mprintf("RTL9601D-VA4\n");
                    break;
                case RTL9602C_CHIP_SUB_TYPE_RTL9602CP:
                    diag_util_mprintf("RTL9602CP\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif          

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType), ret); 
            switch(subType)
            {
                case RTL9607C_CHIP_SUB_TYPE_RTL9607CP:
                    diag_util_mprintf("RTL9607CP\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9607EP:
                    diag_util_mprintf("RTL9607EP\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9603CT:
                    diag_util_mprintf("RTL9603CT\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9607C_VA5:
                case RTL9607C_CHIP_SUB_TYPE_RTL9607C_VA6:
                case RTL9607C_CHIP_SUB_TYPE_RTL9607C_VA7:
                    diag_util_mprintf("RTL9607C\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9607E_VA5:
                    diag_util_mprintf("RTL9607E\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA4:
                case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA5:
                case RTL9607C_CHIP_SUB_TYPE_RTL9603C_VA6:
                    diag_util_mprintf("RTL9603C\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9603CW_VA6:
                case RTL9607C_CHIP_SUB_TYPE_RTL9603CW_VA7:
                    diag_util_mprintf("RTL9603CW\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9603CP:
                    diag_util_mprintf("RTL9603CP\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9603CE:
                    diag_util_mprintf("RTL9603CE\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL8198D_VE5:
                case RTL9607C_CHIP_SUB_TYPE_RTL8198D_VE6:
                case RTL9607C_CHIP_SUB_TYPE_RTL8198D_VE7:
                    diag_util_mprintf("RTL8198D\n");
                    break;
                case RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA5:
                case RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA6:
                case RTL9607C_CHIP_SUB_TYPE_RTL9606C_VA7:
                    diag_util_mprintf("RTL9606C\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif          

#ifdef CONFIG_SDK_LUNA_G3
        case LUNA_G3_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType), ret); 

            switch(subType)
            {
                case 1234:
                    diag_util_mprintf("LUNA G3 subtype\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif

#ifdef CONFIG_SDK_CA8279
        case CA8279_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType),ret);

            switch(subType)
            {
                case 1234:
                    diag_util_mprintf("CA8279 subtype\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif

#ifdef CONFIG_SDK_CA8277B
        case CA8277B_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType),ret);

            switch(subType)
            {
                case 1234:
                    diag_util_mprintf("CA8277B subtype\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif

#ifdef CONFIG_SDK_RTL8277C
        case RTL8277C_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType),ret);

            switch(subType)
            {
                case 1234:
                    diag_util_mprintf("RTL8277C subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9619:
                    diag_util_mprintf("RTL9619 subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9619C:
                    diag_util_mprintf("RTL9619C subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9617C:
                    diag_util_mprintf("RTL9617C subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9615C:
                    diag_util_mprintf("RTL9615C subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9607DQ:
                    diag_util_mprintf("RTL9607DQ subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9617CM:
                    diag_util_mprintf("RTL9617CM subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9607DM:
                    diag_util_mprintf("RTL9607DM subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL9607DS:
                    diag_util_mprintf("RTL9607DS subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL8198XB:
                    diag_util_mprintf("RTL8198BX subtype\n");
                    break;
                case RTL8277C_CHIP_SUB_TYPE_RTL8198XBL:
                    diag_util_mprintf("RTL8198XBL subtype\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType), ret); 

            switch(subType)
            {
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9603CE_VD:
                diag_util_mprintf("RTL9603CE-VD\n");
                break;
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9603C_VD4:
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9603C_VD5:
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9603C_VD6:
                diag_util_mprintf("RTL9603C-VDx\n");
                break;
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9602C_VD5:
                diag_util_mprintf("RTL9602C-VDx\n");
                break;
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9601D_VD3:
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9601D_VD4:
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9601D_VD5:
                diag_util_mprintf("RTL9601D-VDx\n");
                break;
            case RTL9603CVD_CHIP_SUB_TYPE_RTL9601DW_VD5:
                diag_util_mprintf("RTL9601DW-VDx\n");
                break;
            default:
                diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                break;
            }
            break;
        }
#endif          

#ifdef CONFIG_SDK_RTL9607F
        case RTL9607F_CHIP_ID:
        {
            uint32  chipId,rev,subType  = 0;
            DIAG_UTIL_ERR_CHK(rtk_switch_version_get(&chipId,&rev,&subType),ret);

            switch(subType)
            {
                case RTL9607F_CHIP_SUB_TYPE_RTL9607FQ:
                    diag_util_mprintf("RTL9607FQ subtype\n");
                    break;
                case RTL9607F_CHIP_SUB_TYPE_RTL9607FK:
                    diag_util_mprintf("RTL9607FK subtype\n");
                    break;
                case RTL9607F_CHIP_SUB_TYPE_RTL9607F:
                    diag_util_mprintf("RTL9607F subtype\n");
                    break;
                case RTL9607F_CHIP_SUB_TYPE_RTL9607FS:
                    diag_util_mprintf("RTL9607FS subtype\n");
                    break;
                case RTL9607F_CHIP_SUB_TYPE_RTL9607FD:
                    diag_util_mprintf("RTL9607FD subtype\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode 0x%x\n", subType);
                    break;
            }
            break;
        }
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}


/*
 * debug get version { detail }
 */
cparser_result_t
cparser_cmd_debug_get_version_detail(
    cparser_context_t *context)
{
    uint16 date, time, revision, type;
    int32 detail, i, total_entry, def_idx;
    uint32 rtl_version, sw_version;
    uint32 reg_addr;
    uint32 chip_id, value1, value2;
    uint32 regData;


#ifdef CONFIG_SDK_APOLLOMP
    uint32 rgf_ver_reg[]={
            APOLLOMP_RGF_VER_GLB_CTRLr,
            /*RGF_VER_CHIP_INFOr,*/
            APOLLOMP_RGF_VER_ALE_GLBr,
            APOLLOMP_RGF_VER_ALE_ACLr,
            APOLLOMP_RGF_VER_ALE_CVLANr,
            APOLLOMP_RGF_VER_ALE_DPMr,
            APOLLOMP_RGF_VER_ALE_L2r,
            APOLLOMP_RGF_VER_ALE_MLTVLANr,
            APOLLOMP_RGF_VER_ALE_SVLANr,
            APOLLOMP_RGF_VER_ALE_RLDPr,
            APOLLOMP_RGF_VER_ALE_EAV_AFBKr,
            APOLLOMP_RGF_VER_INTRr,
            /*RGF_VER_LEDr,*/
            APOLLOMP_RGF_VER_PER_PORT_MACr,
            /*RGF_VER_SDSREGr,*/
            APOLLOMP_RGF_VER_SWCOREr,
            /*RGF_VER_EPON_CTRLr,*/
            APOLLOMP_RGF_VER_ALE_RMA_ATTACKr,
            APOLLOMP_RGF_VER_BIST_CTRLr,
            APOLLOMP_RGF_VER_EGR_OUTQr,
            APOLLOMP_RGF_VER_EGR_SCHr,
            APOLLOMP_RGF_VER_ALE_HSAr,
            APOLLOMP_RGF_VER_ALE_METERr,
            /*RGF_VER_MAC_PONr,*/
            APOLLOMP_RGF_VER_MIB_CTRLr,
            APOLLOMP_RGF_VER_ALE_PISOr
            };

    int8 *rgf_ver_str[]={
            "RGF_VER_GLB_CTRL",
            /*"RGF_VER_CHIP_INFO",*/
            "RGF_VER_ALE_GLB",
            "RGF_VER_ALE_ACL",
            "RGF_VER_ALE_CVLAN",
            "RGF_VER_ALE_DPM",
            "RGF_VER_ALE_L2",
            "RGF_VER_ALE_MLTVLAN",
            "RGF_VER_ALE_SVLAN",
            "RGF_VER_ALE_RLDP",
            "RGF_VER_ALE_EAV_AFBK",
            "RGF_VER_INTR",
            /*"RGF_VER_LED",*/
            "RGF_VER_PER_PORT_MAC",
            /*"RGF_VER_SDSREG",*/
            "RGF_VER_SWCORE",
            /*"RGF_VER_EPON_CTRL",*/
            "RGF_VER_ALE_RMA_ATTACK",
            "RGF_VER_BIST_CTRL",
            "RGF_VER_EGR_OUTQ",
            "RGF_VER_EGR_SCH",
            "RGF_VER_ALE_HSA",
            "RGF_VER_ALE_METER",
            /*"RGF_VER_MAC_PON",*/
            "RGF_VER_MIB_CTRL",
            "RGF_VER_ALE_PISO"
            };
#endif


#ifdef CONFIG_SDK_RTL9607C
        uint32 rtl9607c_rgf_ver_reg[]={
                RTL9607C_RGF_VER_GLB_CTRLr,
                RTL9607C_RGF_VER_ALE_GLBr,
                RTL9607C_RGF_VER_ALE_ACLr,
                RTL9607C_RGF_VER_ALE_CVLANr,
                RTL9607C_RGF_VER_ALE_DPMr,
                RTL9607C_RGF_VER_ALE_L2r,
                RTL9607C_RGF_VER_ALE_MLTVLANr,
                RTL9607C_RGF_VER_ALE_SVLANr,
                RTL9607C_RGF_VER_ALE_RLDPr,
                RTL9607C_RGF_VER_ALE_EAV_AFBKr,
                RTL9607C_RGF_VER_INTRr,
                RTL9607C_RGF_VER_LEDr,
                RTL9607C_RGF_VER_PER_PORT_MACr,
                /*RTL9607C_RGF_VER_SDSREGr,*/
                RTL9607C_RGF_VER_SWCOREr,
                RTL9607C_RGF_VER_EPON_CTRLr,
                RTL9607C_RGF_VER_ALE_RMA_ATTACKr,
                RTL9607C_RGF_VER_BIST_CTRLr,
                RTL9607C_RGF_VER_EGR_OUTQr,
                RTL9607C_RGF_VER_EGR_SCHr,
                RTL9607C_RGF_VER_ALE_HSAr,
                RTL9607C_RGF_VER_ALE_METERr,
                RTL9607C_RGF_VER_MIB_CTRLr,
                RTL9607C_RGF_VER_ALE_PISOr,
                RTL9607C_RGF_VER_CHIP_INFOr,
                RTL9607C_RGF_VER_ALE_CFr,
                /*RTL9607C_RGF_VER_PHY_IPr,*/
                RTL9607C_RGF_VER_PONIP_GLBr,
                RTL9607C_RGF_VER_PONIP_USr,
                RTL9607C_RGF_VER_PONIP_DSr,

                };
    
        int8 *rtl9607c_rgf_ver_str[]={
                    "RGF_VER_GLB_CTRL",
                    "RGF_VER_ALE_GLB",
                    "RGF_VER_ALE_ACL",
                    "RGF_VER_ALE_CVLAN",
                    "RGF_VER_ALE_DPM",
                    "RGF_VER_ALE_L2",
                    "RGF_VER_ALE_MLTVLAN",
                    "RGF_VER_ALE_SVLAN",
                    "RGF_VER_ALE_RLDP",
                    "RGF_VER_ALE_EAV_AFBK",
                    "RGF_VER_INTR",
                    "RGF_VER_LED",
                    "RGF_VER_PER_PORT_MAC",
                    /*"RGF_VER_SDSREG",*/
                    "RGF_VER_SWCORE",
                    "RGF_VER_EPON_CTRL",
                    "RGF_VER_ALE_RMA_ATTACK",
                    "RGF_VER_BIST_CTRL",
                    "RGF_VER_EGR_OUTQ",
                    "RGF_VER_EGR_SCH",
                    "RGF_VER_ALE_HSA",
                    "RGF_VER_ALE_METER",
                    "RGF_VER_MIB_CTRL",
                    "RGF_VER_ALE_PISO",
                    "RGF_VER_CHIP_INFO",
                    "RGF_VER_ALE_CF",
                    /*"RGF_VER_PHY_IP",*/
                    "RGF_VER_PONIP_GLB",
                    "RGF_VER_PONIP_US",
                    "RGF_VER_PONIP_DS",
                };
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        uint32 rtl9603cvd_rgf_ver_reg[]={
                RTL9603CVD_RGF_VER_GLB_CTRLr,
                RTL9603CVD_RGF_VER_ALE_GLBr,
                RTL9603CVD_RGF_VER_ALE_ACLr,
                RTL9603CVD_RGF_VER_ALE_CVLANr,
                RTL9603CVD_RGF_VER_ALE_DPMr,
                RTL9603CVD_RGF_VER_ALE_L2r,
                RTL9603CVD_RGF_VER_ALE_MLTVLANr,
                RTL9603CVD_RGF_VER_ALE_SVLANr,
                RTL9603CVD_RGF_VER_ALE_EAV_AFBKr,
                RTL9603CVD_RGF_VER_INTRr,
                RTL9603CVD_RGF_VER_LEDr,
                RTL9603CVD_RGF_VER_PER_PORT_MACr,
                /*RTL9607C_RGF_VER_SDSREGr,*/
                RTL9603CVD_RGF_VER_SWCOREr,
                RTL9603CVD_RGF_VER_EPON_CTRLr,
                RTL9603CVD_RGF_VER_ALE_RMA_ATTACKr,
                RTL9603CVD_RGF_VER_BIST_CTRLr,
                RTL9603CVD_RGF_VER_EGR_OUTQr,
                RTL9603CVD_RGF_VER_EGR_SCHr,
                RTL9603CVD_RGF_VER_ALE_HSAr,
                RTL9603CVD_RGF_VER_ALE_METERr,
                RTL9603CVD_RGF_VER_MIB_CTRLr,
                RTL9603CVD_RGF_VER_ALE_PISOr,
                RTL9603CVD_RGF_VER_CHIP_INFOr,
                /*RTL9607C_RGF_VER_PHY_IPr,*/
                RTL9603CVD_RGF_VER_PONIP_GLBr,
                RTL9603CVD_RGF_VER_PONIP_USr,
                RTL9603CVD_RGF_VER_PONIP_DSr,

                };
    
        int8 *rtl9603cvd_rgf_ver_str[]={
                    "RGF_VER_GLB_CTRL",
                    "RGF_VER_ALE_GLB",
                    "RGF_VER_ALE_ACL",
                    "RGF_VER_ALE_CVLAN",
                    "RGF_VER_ALE_DPM",
                    "RGF_VER_ALE_L2",
                    "RGF_VER_ALE_MLTVLAN",
                    "RGF_VER_ALE_SVLAN",
                    "RGF_VER_ALE_EAV_AFBK",
                    "RGF_VER_INTR",
                    "RGF_VER_LED",
                    "RGF_VER_PER_PORT_MAC",
                    /*"RGF_VER_SDSREG",*/
                    "RGF_VER_SWCORE",
                    "RGF_VER_EPON_CTRL",
                    "RGF_VER_ALE_RMA_ATTACK",
                    "RGF_VER_BIST_CTRL",
                    "RGF_VER_EGR_OUTQ",
                    "RGF_VER_EGR_SCH",
                    "RGF_VER_ALE_HSA",
                    "RGF_VER_ALE_METER",
                    "RGF_VER_MIB_CTRL",
                    "RGF_VER_ALE_PISO",
                    "RGF_VER_CHIP_INFO",
                    /*"RGF_VER_PHY_IP",*/
                    "RGF_VER_PONIP_GLB",
                    "RGF_VER_PONIP_US",
                    "RGF_VER_PONIP_DS",
                };
#endif


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_NUM() == 4)
        detail = 1;
    else
        detail = 0;

    diag_util_mprintf("Diagshell Build Time: %s %s\n", __DATE__,__TIME__);

#ifdef CONFIG_SDK_USER_MODE
    diag_util_mprintf("Mode: User mode\n");
#else
    diag_util_mprintf("Mode: Kernel mode\n");
#endif

    diag_util_mprintf("Platform: ");
#ifdef CONFIG_PC_FPGA_MDIO_MODE
    diag_util_mprintf("PC -- MDIO Access mode\n");
#endif

#ifdef CONFIG_VIRTUAL_DRV_MODE
    diag_util_mprintf("PC -- virtual asic driver\n");
#endif

#ifdef CONFIG_RLX_MODE
    diag_util_mprintf("PC -- access asic by RLX interface\n");
#endif


#ifdef CONFIG_SOC_FPGA_MDIO_MODE
    diag_util_mprintf("SoC -- MDIO Access mode(for 0371 FPGA verify)\n");
#endif


#ifdef CONFIG_FPGA_SPI_MODE
    diag_util_mprintf("SoC -- run on 0371(for 6266 FPGA test)\n");
#endif

#ifdef CONFIG_FPGA_SPI_L34_MODE
    diag_util_mprintf("SoC -- run on 0371 for L34(for 6266 FPGA test)\n");
#endif

#ifdef CONFIG_SOC_MODE
    diag_util_mprintf("SoC -- Normal mode\n");
#endif

#ifdef CONFIG_SOC_VIRTUAL_DRV_MODE
    diag_util_mprintf("SoC -- virtual asic driver\n");
#endif


    /*for chip driver support*/
    diag_util_mprintf("Chip Support: \n");
#ifdef CONFIG_SDK_APOLLO
    diag_util_mprintf("             Apollo\n");
#endif

#ifdef CONFIG_SDK_APOLLOMP
    diag_util_mprintf("             Apollo-MP\n");
#endif

#ifdef CONFIG_SDK_RTL9601B
    diag_util_mprintf("             RTL9601B\n");
#endif


#ifdef FORCE_PROBE_APOLLO
    diag_util_mprintf("Chip probe setting: Force Apollo\n");
#elif FORCE_PROBE_APOLLO_REV_B
    diag_util_mprintf("Chip probe setting: Force Apollo Rev.B\n");
#elif FORCE_PROBE_APOLLOMP
    diag_util_mprintf("Chip probe setting: Force Apollo-MP\n");
#elif FORCE_PROBE_APOLLOMP_REV_B
    diag_util_mprintf("Chip probe setting: Force Apollo-MP Rev.B\n");
#elif FORCE_PROBE_RTL9601B
    diag_util_mprintf("Chip probe setting: Force RTL9601B\n");
#elif FORCE_PROBE_RTL9602C
    diag_util_mprintf("Chip probe setting: Force RTL9602C\n");
#elif FORCE_PROBE_RTL9602C_REV_B
    diag_util_mprintf("Chip probe setting: Force RTL9602C Rev.B\n");
#elif FORCE_PROBE_RTL9607C
    diag_util_mprintf("Chip probe setting: Force RTL9607C\n");
#elif FORCE_PROBE_RTL9607C_REV_B
    diag_util_mprintf("Chip probe setting: Force RTL9607C Rev.B\n");
#elif FORCE_PROBE_RTL9603CVD
    diag_util_mprintf("Chip probe setting: Force RTL9603CVD\n");
#else
    diag_util_mprintf("Chip probe setting: Auto detect\n");

    if (rtk_switch_version_get(&chip_id, &value1, &value2) == RT_ERR_OK)
    {
        if( (chip_id == APOLLO_CHIP_ID) )
        {
            diag_util_mprintf("Chip probe result : Apollo (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == APOLLOMP_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : Apollo-MP (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == RTL9601B_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : RTL9601B (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == RTL9602C_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : RTL9602C (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == RTL9607C_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : RTL9607C (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == LUNA_G3_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : LUNA G3 (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == CA8279_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : CA8279 (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == CA8277B_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result : CA8277B (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == RTL9603CVD_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result RTL9603CVD (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == RTL8277C_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result RTL8277C (ID = 0x%08X)\n", chip_id);
        }
        else if( (chip_id == RTL9607F_CHIP_ID))
        {
            diag_util_mprintf("Chip probe result RTL9607F (ID = 0x%08X)\n", chip_id);
        }
        else
        {
            diag_util_mprintf("Chip probe result : Unknown! (ID = 0x%08X)\n", chip_id);
        }
        cparser_cmd_debug_get_chip_sub_type();
    }
    else
    {
        diag_util_mprintf("Chip probe result : Unknown!\n");
    }
#endif

#if defined (FPGA_DEFINED)

#if defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)

    (void)io_mii_phy_reg_read(9, 0x0, &date);
    (void)io_mii_phy_reg_read(9, 0x1, &time);
    (void)io_mii_phy_reg_read(9, 0x2, &revision);
    (void)io_mii_phy_reg_read(9, 0x3, &type);
    diag_util_mprintf("FPGA data: 0x%04x, time 0x%04x, revision 0x%04x, tag 0x%04x\n", date, time, revision, type);

#endif

#ifdef CONFIG_SDK_RTL9601B
    reg_read(RTL9601B_FPGA_VER_MACr,&regData);

    diag_util_mprintf("FPGA svn: 0x%04x, day: %d, month: %d ",
                                regData&0xFFFF,
                                (regData>>19)&0x1F,
                                (regData>>24)&0xF);

    if(regData&0x40000)
        diag_util_mprintf("(clock 83.3Mhz) ");
    else
        diag_util_mprintf("(clock 66.6mhz) ");

    switch((regData>>16)&0x7)
    {
        case 1:
            diag_util_mprintf("type: switch\n");
            break;
        case 2:
            diag_util_mprintf("type: GPON\n");
            break;
        case 3:
            diag_util_mprintf("type: EPON\n");
            break;
        default:
            diag_util_mprintf("type: unknown\n");
            break;
    }

    (void)io_mii_phy_reg_read(9,0,&date);
    diag_util_mprintf("PBO FPGA %04x", date);
    (void)io_mii_phy_reg_read(9,1,&date);
    (void)io_mii_phy_reg_read(9,2,&time);
    (void)io_mii_phy_reg_read(9,3,&revision);
    diag_util_mprintf("-%04x-%04x svn(PBO): 0x%04x",
                                date,
                                time,
                                revision);
    (void)io_mii_phy_reg_read(9,4,&revision);
    diag_util_mprintf(" svn(PONNIC): 0x%04x\n", revision);
    (void)io_mii_phy_reg_read(9,5,&type);
    diag_util_mprintf(" type: 0x%04x\n", type);


#endif

#ifdef CONFIG_SDK_APOLLOMP
    reg_field_read(rgf_ver_reg[0], APOLLOMP_REGFILE_VERf, &rtl_version);
    diag_util_printf("%-22s: 0x%08x\n",rgf_ver_str[0], rtl_version);


    total_entry = sizeof(rgf_ver_reg)/sizeof(uint32);
    for(i = 0; i < total_entry; i++)
    {
        reg_field_read(rgf_ver_reg[i], APOLLOMP_REGFILE_VERf, &rtl_version);

        reg_addr = HAL_GET_REG_ADDR(rgf_ver_reg[i]);
        sw_version = 0;
        for(def_idx = 0; def_idx < APOLLOMP_REG_MAP_ENTRY_NUM; def_idx++)
        {
            if(apollomp_reg_map_def[def_idx].reg_addr == reg_addr)
                sw_version = apollomp_reg_map_def[def_idx].reset_val;
        }

        if((rtl_version != sw_version))
            diag_util_printf("Mismatch, %-22s: rtl_version=0x%08x, sw_version=0x%08x\n",rgf_ver_str[i],rtl_version,sw_version);

        if(detail)
            diag_util_printf("%-22s, version=0x%08x\n",rgf_ver_str[i],rtl_version);
    }
#endif

#if defined(CONFIG_SDK_RTL9607C)
        
        reg_field_read(rtl9607c_rgf_ver_reg[0], RTL9607C_REGFILE_VERf, &rtl_version);
        //diag_util_printf("%-22s: 0x%08x\n",rgf_ver_str[0], rtl_version);
        
        
        total_entry = sizeof(rgf_ver_reg)/sizeof(uint32);
        for(i = 0; i < total_entry; i++)
        {
            reg_field_read(rtl9607c_rgf_ver_reg[i], RTL9607C_REGFILE_VERf, &sw_version);
        
            if((rtl_version != sw_version))
                diag_util_printf("Mismatch, %-22s: 0x%08x (%-22s: 0x%08x)\n",rtl9607c_rgf_ver_str[i],sw_version,rtl9607c_rgf_ver_str[0], rtl_version);
        }
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
        
        reg_field_read(rtl9603cvd_rgf_ver_reg[0], RTL9603CVD_REGFILE_VERf, &rtl_version);
        //diag_util_printf("%-22s: 0x%08x\n",rgf_ver_str[0], rtl_version);
        
        
        total_entry = sizeof(rgf_ver_reg)/sizeof(uint32);
        for(i = 0; i < total_entry; i++)
        {
            reg_field_read(rtl9603cvd_rgf_ver_reg[i], RTL9603CVD_REGFILE_VERf, &sw_version);
        
            if((rtl_version != sw_version))
                diag_util_printf("Mismatch, %-22s: 0x%08x (%-22s: 0x%08x)\n",rtl9603cvd_rgf_ver_str[i],sw_version,rtl9603cvd_rgf_ver_str[0], rtl_version);
        }
#endif
#endif



    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_version_detail */

#define DEBUG_PKT_DUMPMODE_CHIP  0
#define DEBUG_PKT_DUMPMODE_FPGA  1

static uint8 pkt_rx_dumpMode = DEBUG_PKT_DUMPMODE_CHIP;

/* * debug packet rx dump { <UINT:byte> } */
cparser_result_t
cparser_cmd_debug_packet_rx_dump_byte(
    cparser_context_t *context,
    uint32_t  *byte_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int i, j;
    int len;
    uint16 enable;
    uint16 max_len;
    uint16 rx_len;
    uint8 buf[2048];
    struct pkt_dbg_rx_info rxInfo;
    struct pkt_dbg_rx_cputag cpuTag;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if(5 == TOKEN_NUM())
    {
        if(byte_ptr == NULL)
            return CPARSER_NOT_OK;
        
        max_len = (*byte_ptr > 2048) ? 2048 : *byte_ptr;
    }
    else
        max_len = 128;
    
    DIAG_UTIL_ERR_CHK(rtk_pkt_rxDump_get(&buf[0], max_len, &rx_len, &rxInfo, &enable), ret); 

    /* Check the packet for two dump mode */
    if(DEBUG_PKT_DUMPMODE_FPGA == pkt_rx_dumpMode && (rx_len != 0))
    {
        cpuTag = *((struct pkt_dbg_rx_cputag *)&buf[12]);
        if(cpuTag.etherType == 0x8899 &&
            cpuTag.protocol == 0x4)
        {
            /* CPU tag in the payload */
            if((cpuTag.reason >= 253) && (cpuTag.reason <= 254))
            {
                /* PTP CPU tag */
            }
            else
            {
                /* Normal CPU tag */
                for(i = 12; (i + sizeof(struct pkt_dbg_rx_cputag)) < rx_len; i++)
                {
                    buf[i] = buf[i+sizeof(struct pkt_dbg_rx_cputag)];
                }
                rx_len -= sizeof(struct pkt_dbg_rx_cputag);
                memset(&rxInfo, 0, sizeof(struct pkt_dbg_rx_info));
                /* Set fields manually */
                rxInfo.l3routing = cpuTag.l3r;
                rxInfo.origformat = cpuTag.org;
                rxInfo.pctrl = cpuTag.ptcl;
                rxInfo.data_length = 0xfff & rx_len;
                rxInfo.cputag = 1;
                rxInfo.pon_stream_id = cpuTag.pon_stream_id;
                rxInfo.src_port_num = cpuTag.spa;
                rxInfo.dst_port_mask = cpuTag.ext_port_mask;
                rxInfo.reason = cpuTag.reason;
                rxInfo.internal_priority = cpuTag.priority;
                rxInfo.ext_port_ttl_1 = cpuTag.ttl_1_ext_port_mask;
            }
        }
    }

    diag_util_printf("Packet dump: %s\n", enable == 0 ? "disabled" : "enabled");
    diag_util_printf("Actual packet length 0x%x\n", rx_len);
    diag_util_printf("Maximum dump length 0x%x\n", max_len);
#if defined(CONFIG_SDK_RTL9607C)
    {
        uint32 rx_gmac;
        int32 ret;
        
        DIAG_UTIL_ERR_CHK(rtk_pkt_rxGmacNum_get(&rx_gmac), ret); 

        if(rx_gmac==0xff)
            diag_util_printf("Gmac all\n");
        else
            diag_util_printf("Gmac 0x%x\n", rx_gmac);
    }
#endif
    len = (max_len > rx_len) ? rx_len : max_len;

    if(0 == len)
        return CPARSER_OK;

    diag_util_printf("Descriptor:\n");
    diag_util_printf("%8s %8s %8s %8s\n", "opts1", "addr", "opts2", "opts3");
    diag_util_printf("0x%08x 0x%08x 0x%08x 0x%08x\n\n", rxInfo.dw[0], rxInfo.addr, rxInfo.dw[1], rxInfo.dw[2]);
    diag_util_printf("own eor fs ls crc l3csf l4csf rcdf frag ppptag rwt pkttype rout orifmt pctrl len\n");
    diag_util_printf("%3d %3d %2d %2d %3d %5d %5d %4d %4d %6d %3d %7d %4d %6d %5d %3d\n",
        rxInfo.own,
        rxInfo.eor,
        rxInfo.fs,
        rxInfo.ls,
        rxInfo.crcerr,
        rxInfo.ipv4csf,
        rxInfo.l4csf,
        rxInfo.rcdf,
        rxInfo.ipfrag,
        rxInfo.pppoetag,
        rxInfo.rwt,
        rxInfo.pkttype,
        rxInfo.l3routing,
        rxInfo.origformat,
        rxInfo.pctrl,
        rxInfo.data_length);

    diag_util_printf("cputag ptp_in_cpu_tag_exist svlan_tag_exist pon_stream_id ctagva cvlan_tag\n");
    diag_util_printf("%6d %20d %15d %13d %6d %9d\n",
        rxInfo.cputag,
        rxInfo.ptp_in_cpu_tag_exist,
        rxInfo.svlan_tag_exist,
        rxInfo.pon_stream_id,
        rxInfo.ctagva,
        rxInfo.cvlan_tag);

    diag_util_printf("src_port_num dst_port_mask reason internal_pri ext_port_ttl\n");
    diag_util_printf("%12d %13d %6d %12d %12d\n\n",
        rxInfo.src_port_num,
        rxInfo.dst_port_mask,
        rxInfo.reason,
        rxInfo.internal_priority,
        rxInfo.ext_port_ttl_1);

    diag_util_printf("     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");

    for(i = 0, j = 0; i < len; i++) {
        if(0 == (i % 0x10)) {
            diag_util_printf("\n%04x ", j);
            j++;
        }
        diag_util_printf("%02x ", buf[i]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_dump_byte */

/*
 * debug packet rx dump-length <UINT:start> { <UINT:length> }
 */
cparser_result_t
cparser_cmd_debug_packet_rx_dump_length_start_length(
    cparser_context_t *context,
    uint32_t  *start_ptr,
    uint32_t  *length_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int ret;
    int i, j;
    int len, dump_len;
    uint16 enable;
    uint16 rx_len;
    uint8 buf[2048];
    struct pkt_dbg_rx_info rxInfo;
    struct pkt_dbg_rx_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    if(6 == TOKEN_NUM())
    {
        if(length_ptr == NULL)
            return CPARSER_NOT_OK;

        if(0 == *length_ptr)
        {
            diag_util_printf("Nothing to be dumped!\n");
            return CPARSER_OK;
        }
        dump_len = *length_ptr;
    }
    else
    {
        dump_len = 1;
    }
    DIAG_UTIL_ERR_CHK(rtk_pkt_rxDump_get(&buf[0], 2048, &rx_len, &rxInfo, &enable), ret); 

    /* Check the packet for two dump mode */
    if(DEBUG_PKT_DUMPMODE_FPGA == pkt_rx_dumpMode && (rx_len != 0))
    {
        cpuTag = *((struct pkt_dbg_rx_cputag *)&buf[12]);
        if(cpuTag.etherType == 0x8899 &&
            cpuTag.protocol == 0x4)
        {
            /* CPU tag in the payload */
            if((cpuTag.reason >= 253) && (cpuTag.reason <= 254))
            {
                /* PTP CPU tag */
            }
            else
            {
                /* Normal CPU tag */
                for(i = 12; (i + sizeof(struct pkt_dbg_rx_cputag)) < rx_len; i++)
                {
                    buf[i] = buf[i+sizeof(struct pkt_dbg_rx_cputag)];
                }
                rx_len -= sizeof(struct pkt_dbg_rx_cputag);
            }
        }
    }

    if(0 == rx_len)
    {
        diag_util_printf("Nothing to be dumped!\n");
        return CPARSER_OK;
    }

    len = (*start_ptr + dump_len < rx_len) ? (*start_ptr + dump_len) : rx_len;
    for(i = *start_ptr; i < len; i++)
    {
        diag_util_printf("%02x ", buf[i]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_dump_length_start_length */

/*
 * debug packet rx dump-fifo { <UINT:index> }
 */
cparser_result_t
cparser_cmd_debug_packet_rx_dump_fifo_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int ret;
    int loop_start, loop_end;
    int i, j;
    uint8 buf[64];

    DIAG_UTIL_PARAM_CHK();

    if(5 == TOKEN_NUM())
    {
        if(index_ptr == NULL)
            return CPARSER_NOT_OK;

        loop_start = *index_ptr;
        loop_end = *index_ptr;
    }
    else
    {
        loop_start = 0;
        loop_end = 15;
    }

    for(i = loop_start; i <= loop_end; i++)
    {
        ret = rtk_pkt_rxFifoDump_get(i, &buf[0]);
        if(RT_ERR_OK == ret)
        {
            diag_util_printf("packet%02d\n", i);
            for(j = 0; j < 64; j++)
            {
                diag_util_printf("%02x ", buf[j]);
                if(j != 0 && (((j+1) % 16) == 0))
                {
                    diag_util_printf("\n");
                }
            }
        }
        else
        {
            diag_util_printf("Nothing to be dumped for index %d (ret %d)\n", i, ret);
        }
    }

    return CPARSER_OK;
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_dump_fifo */

/*
 * debug packet rx mode ( chip | fpga )
 */
cparser_result_t
cparser_cmd_debug_packet_rx_mode_chip_fpga(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    diag_util_printf("dump mode: %s ->", pkt_rx_dumpMode ? "fpga" : "chip");
    if('c' == TOKEN_CHAR(4,0))
    {
        pkt_rx_dumpMode = DEBUG_PKT_DUMPMODE_CHIP;
        diag_util_printf(" %s\n", pkt_rx_dumpMode ? "fpga" : "chip");
    }
    else if('f' == TOKEN_CHAR(4,0))
    {
        pkt_rx_dumpMode = DEBUG_PKT_DUMPMODE_FPGA;
        diag_util_printf(" %s\n", pkt_rx_dumpMode ? "fpga" : "chip");
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_mode_chip_fpga */

/*
 * debug packet rx clear
 */
cparser_result_t
cparser_cmd_debug_packet_rx_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    {
        int32 ret;
        DIAG_UTIL_ERR_CHK(rtk_pkt_rxDump_clear(), ret); 
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_clear */

/* * debug packet rx { enable | disable } */
cparser_result_t
cparser_cmd_debug_packet_rx_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    {
        int ret;

        if('e' == TOKEN_CHAR(3, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_pkt_rxDumpEnable_set(1), ret); 
            diag_util_printf("Packet dump: enabled\n");
        }
        else if('d' == TOKEN_CHAR(3, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_pkt_rxDumpEnable_set(0), ret); 
            diag_util_printf("Packet dump: diabled\n");
        }
        else
            return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_enable_disable */

/*
 * debug packet rx gmac { <UINT:num> }
 */
cparser_result_t
cparser_cmd_debug_packet_rx_gmac_num(
    cparser_context_t *context,
    uint32_t  *num_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE) && defined(CONFIG_SDK_RTL9607C)
    uint32 rx_gmac=0;
    int32 ret;
    if(TOKEN_NUM() >= 5)
    {
        if(num_ptr == NULL)
            return CPARSER_NOT_OK;

        if(*num_ptr > 2)
        {
            diag_util_printf("Gmac num should be 0,1\n");
            return CPARSER_NOT_OK;
        }
        rx_gmac = *num_ptr;
    }
    else
        rx_gmac = 0xff; //0xff means rxceive all gmac

    
    DIAG_UTIL_ERR_CHK(rtk_pkt_rxGmacNum_set(rx_gmac), ret); 
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_gmac_num */

/*
 * debug packet tx set pkt <UINT:pos> <STRING:data>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_pkt_pos_data(
    cparser_context_t *context,
    uint32_t  *pos_ptr,
    char * *data_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 i;
    char tmp[3];
    char *pPtr;
    uint8 *pData = NULL;
    uint16 input_length;
    uint16 payload_length;

    DIAG_UTIL_PARAM_CHK();

    /* Syntax check of the input string */
    input_length = strlen(*data_ptr);
    if(input_length & 0x01)
    {
        diag_util_printf("payload syntax error\n");
        return CPARSER_NOT_OK;
    }
    for(i = 0, pPtr = *data_ptr; i < input_length; i++)
    {
        if((pPtr[i] >= 'a') && (pPtr[i] <= 'f'))
            continue;
        else if((pPtr[i] >= 'A') && (pPtr[i] <= 'F'))
            continue;
        else if((pPtr[i] >= '0') && (pPtr[i] <= '9'))
            continue;

        diag_util_printf("payload syntax error at %d\n", i);
        return CPARSER_NOT_OK;
    }

    /* Parse the string */
    payload_length = input_length >> 1;
    pData = (uint8 *) osal_alloc(payload_length * sizeof(uint8));
    if(NULL == pData)
    {
        diag_util_printf("allocate failed\n");
        return CPARSER_NOT_OK;
    }

    tmp[2] = '\0';
    for(i = 0, pPtr = *data_ptr; i < payload_length; i++, pPtr += 2)
    {
        tmp[0] = pPtr[0];
        tmp[1] = pPtr[1];
        pData[i] = strtoul(&tmp[0], NULL, 16);
    }

    rtk_pkt_txPkt_set(*pos_ptr, pData, payload_length);

    osal_free(pData);
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_pkt_pos_data */

/*
 * debug packet tx set addr <MACADDR:da> <MACADDR:sa>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_addr_da_sa(
    cparser_context_t *context,
    cparser_macaddr_t  *da_ptr,
    cparser_macaddr_t  *sa_ptr)
{
    DIAG_UTIL_PARAM_CHK();
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_txAddr_set((rtk_mac_t *)da_ptr, (rtk_mac_t *)sa_ptr);
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_addr_da_sa */

/*
 * debug packet tx set l2payload <UINT:pos> <STRING:payload>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_l2payload_pos_payload(
    cparser_context_t *context,
    uint32_t  *pos_ptr,
    char * *payload_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 i;
    char tmp[3];
    char *pPtr;
    uint8 *pPayload = NULL;
    uint16 input_length;
    uint16 payload_length;

    DIAG_UTIL_PARAM_CHK();

    /* Syntax check of the input string */
    input_length = strlen(*payload_ptr);
    if(input_length & 0x01)
    {
        diag_util_printf("payload syntax error\n");
        return CPARSER_NOT_OK;
    }
    for(i = 0, pPtr = *payload_ptr; i < input_length; i++)
    {
        if((pPtr[i] >= 'a') && (pPtr[i] <= 'f'))
            continue;
        else if((pPtr[i] >= 'A') && (pPtr[i] <= 'F'))
            continue;
        else if((pPtr[i] >= '0') && (pPtr[i] <= '9'))
            continue;

        diag_util_printf("payload syntax error at %d\n", i);
        return CPARSER_NOT_OK;
    }

    /* Parse the string */
    payload_length = input_length >> 1;
    pPayload = (uint8 *) osal_alloc(payload_length * sizeof(uint8));
    if(NULL == pPayload)
    {
        diag_util_printf("allocate failed\n");
        return CPARSER_NOT_OK;
    }

    tmp[2] = '\0';
    for(i = 0, pPtr = *payload_ptr; i < payload_length; i++, pPtr += 2)
    {
        tmp[0] = pPtr[0];
        tmp[1] = pPtr[1];
        pPayload[i] = strtoul(&tmp[0], NULL, 16);
    }

    rtk_pkt_txPkt_set(*pos_ptr + PKT_DEBUG_PKT_MACADDR_OFFSET, pPayload, payload_length);

    osal_free(pPayload);
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_payload_payload */

/*
 * debug packet tx set padding <UINT:start> <UINT:end> <UINT:pkt_len>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_padding_start_end_pkt_len(
    cparser_context_t *context,
    uint32_t  *start_ptr,
    uint32_t  *end_ptr,
    uint32_t  *pkt_len_ptr)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_txPadding_set(*start_ptr, *end_ptr, *pkt_len_ptr);
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_padding_data_pkt_len */


/*
 * debug packet tx get preview { <UINT:length> }
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_preview_length(
    cparser_context_t *context,
    uint32_t  *length_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 i,j;
    int32 ret;
    uint8 *pkt;
    uint16 pkt_length;
    uint16 dump_length;

    DIAG_UTIL_PARAM_CHK();

    if(TOKEN_NUM() >= 6)
    {
        if(length_ptr == NULL)
            return CPARSER_NOT_OK;

        dump_length = ((*length_ptr) > PKT_DEBUG_PKT_LENGTH_MAX) ? PKT_DEBUG_PKT_LENGTH_MAX : (*length_ptr);
    }
    else
    {
        dump_length = 128;
    }

    pkt = (uint8 *) osal_alloc(dump_length * sizeof(uint8));
    if(NULL == pkt)
    {
        diag_util_printf("%s:%d Allocate buffer failed!\n", __FUNCTION__, __LINE__);
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txBuffer_get(pkt, dump_length, &pkt_length);
    if(ret != 0)
    {
        diag_util_printf("%s:%d Get packet failed!\n", __FUNCTION__, __LINE__);
        osal_free(pkt);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("packet length: %d\n\n", pkt_length);
    if(pkt_length != 0)
    {
        diag_util_printf("     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");

        for(i = 0, j = 0; i < (dump_length < pkt_length ? dump_length : pkt_length); i++) {
            if(0 == (i % 0x10)) {
                diag_util_printf("\n%04x ", j);
                j++;
            }
            diag_util_printf("%02x ", pkt[i]);
        }
    }
    diag_util_printf("\n");

    osal_free(pkt);
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_get_preview_length */

/*
 * debug packet tx clear
 */
cparser_result_t
cparser_cmd_debug_packet_tx_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_txBuffer_clear();
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_clear */

/*
 * debug packet tx send { <UINT:count> }
 */
cparser_result_t
cparser_cmd_debug_packet_tx_send_count(
    cparser_context_t *context,
    uint32_t  *count_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int ret;
    uint32 tx_count;

    DIAG_UTIL_PARAM_CHK();

    if(TOKEN_NUM() >= 5)
    {
        if(count_ptr == NULL)
            return CPARSER_NOT_OK;

        tx_count = *count_ptr;
    }
    else
        tx_count = 1;

    ret = rtk_pkt_tx_send(tx_count);
    if(RT_ERR_NOT_INIT == ret)
    {
        diag_util_printf("no packet to send\n");
    }
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_send */

/*
 * debug packet tx send-continuously ( enable | disable ) { <UINT:count> }
 */
cparser_result_t
cparser_cmd_debug_packet_tx_send_continuously_enable_disable_count(
    cparser_context_t *context,
    uint32_t  *count_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    uint32 enable = ENABLED;
    int ret;
    uint32 count;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }

    if(TOKEN_NUM() == 6)
    {
        if(count_ptr == NULL)
            return CPARSER_NOT_OK;
            
        ret = rtk_pkt_continuouslyTxCnt_set(*count_ptr);
        if(RT_ERR_NOT_INIT == ret)
        {
            diag_util_printf("no packet to send\n");
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_pkt_continuouslyTxCnt_get(&count), ret); 

    ret = rtk_pkt_continuouslyTx_send(enable);
    if(RT_ERR_NOT_INIT == ret)
    {
        diag_util_printf("no packet to send\n");
    }
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_send_continus_enable_disable */

/*
 * debug packet tx set cputag l3cs ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l3cs_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.l3cs = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.l3cs = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l3cs_enable_disable */

/*
 * debug packet tx set cputag l4cs ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l4cs_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.l4cs = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.l4cs = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l4cs_enable_disable */

/*
 * debug packet tx set cputag keep ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_keep_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.keep = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.keep = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_keep_enable_disable */

/*
 * debug packet tx set cputag learning ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_learning_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.dislrn = 0;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.dislrn = 1;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_learning_enable_disable */

/*
 * debug packet tx set cputag l2br ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l2br_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.l2br = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.l2br = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l2br_enable_disable */

/*
 * debug packet tx set cputag sendback ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_sendback_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.sb = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.sb = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_sendback_enable_disable */

/*
 * debug packet tx set cputag l34keep ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l34keep_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.l34keep = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.l34keep = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l34keep_enable_disable */

/*
 * debug packet tx set cputag efid ( enable | disable ) <UINT:efid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_efid_enable_disable_efid(
    cparser_context_t *context,
    uint32_t  *efid_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    if(*efid_ptr > 0b111)
    {
        diag_util_printf("enhance fid out of range %d\n", *efid_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.efid = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.efid = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.efid_value = *efid_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_efid_enable_disable_efid */

/*
 * debug packet tx set cputag prisel ( enable | disable ) <UINT:priority>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_prisel_enable_disable_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    if(*priority_ptr > 0b111)
    {
        diag_util_printf("priority out of range %d\n", *priority_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.prisel = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.prisel = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.priority = *priority_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_prisel_enable_disable_priority */

/*
 * debug packet tx set cputag vsel ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_vsel_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.vsel = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.vsel = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_vsel_enable_disable */

/*
 * debug packet tx set cputag txmask_vidx <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_txmask_vidx_value(
    cparser_context_t *context,
    uint32_t *value_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();


    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    cpuTag.txmask_vidx = *value_ptr;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_txmask_vidx */

/*
 * debug packet tx set cputag psel ( enable | disable ) <UINT:streamid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_psel_enable_disable_streamid(
    cparser_context_t *context,
    uint32_t  *streamid_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    if(*streamid_ptr > 0b1111111)
    {
        diag_util_printf("stream id out of range %d\n", *streamid_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.psel = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.psel = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.pon_streamid = *streamid_ptr & 0b1111111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_psel_enable_disable_streamid */

/*
 * debug packet tx set cputag pppoeact ( intact | addhdr | removehdr | remarking ) <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_pppoeact_intact_addhdr_removehdr_remarking_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    if(*index_ptr > 0b111)
    {
        diag_util_printf("stream id out of range %d\n", *index_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    if('i' == TOKEN_CHAR(6,0))
    {
        cpuTag.pppoeact = 0b00;
    }
    else if('a' == TOKEN_CHAR(6,0))
    {
        cpuTag.pppoeact = 0b01;
    }
    else if(strlen(TOKEN_STR(6)) >= 4)
    {
        if('o' == TOKEN_CHAR(6,3))
        {
            cpuTag.pppoeact = 0b10;
        }
        else if('a' == TOKEN_CHAR(6,3))
        {
            cpuTag.pppoeact = 0b11;
        }
        else
        {
            diag_util_printf("invalid option %s\n", TOKEN_STR(6));
            return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.pppoeidx = *index_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__ ,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_pppoeact_intact_addhdr_removehdr_remarking_index */

/*
 * debug packet tx set cputag extspa <UINT:port>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_extspa_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    if(*port_ptr > 0b111)
    {
        diag_util_printf("port out of range %d\n", *port_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    cpuTag.extspa = *port_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_extspa_port */

/*
 * debug packet tx set cputag gmac <UINT:num>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_gmac_num(
    cparser_context_t *context,
    uint32_t  *num_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE) && (defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD))  
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    if(*num_ptr > 2)
    {
        diag_util_printf("Gmac num should be 0,1\n");
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
    cpuTag.tx_gmac = *num_ptr;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_gmac_num */

/*
 * debug packet tx set cputag clear
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_clear(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0 , sizeof(struct pkt_dbg_cputag));
 
    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n",__FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_clear */



/*
 * debug packet tx get cputag
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_cputag(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("l3cs l4cs txmask_vidx\n");
    diag_util_printf("%4u %4u %11u\n",
                     cpuTag.l3cs,
                     cpuTag.l4cs,
                     cpuTag.txmask_vidx);
    diag_util_printf("efid enhace_fid prisel priority keep vsel dislrn psel l34keep\n");
    diag_util_printf("%4u %10u %6u %8u %4u %4u %6u %4u %7u\n",
                     cpuTag.efid,
                     cpuTag.efid_value,
                     cpuTag.prisel,
                     cpuTag.priority,
                     cpuTag.keep,
                     cpuTag.vsel,
                     cpuTag.dislrn,
                     cpuTag.psel,
                     cpuTag.l34keep);
    diag_util_printf("extspa pppoeact pppoeidx l2br ponstream_id\n");
    diag_util_printf("%6u %8u %8u %4u %12u\n",
                     cpuTag.extspa,
                     cpuTag.pppoeact,
                     cpuTag.pppoeidx,
                     cpuTag.l2br,
                     cpuTag.pon_streamid);
    diag_util_printf("sb\n");
    diag_util_printf("%2u\n",
                     cpuTag.sb);
#if defined(CONFIG_SDK_RTL9607C)  || defined(CONFIG_SDK_RTL9603CVD)     
    diag_util_printf("tx_gmac\n");
    diag_util_printf("%2u\n",
                     cpuTag.tx_gmac);
#endif
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_get_cputag */

/*
 * debug packet tx set tx-cmd ipcs ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_tx_cmd_ipcs_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_tx_info txInfo;

    DIAG_UTIL_PARAM_CHK();

    memset(&txInfo, 0, sizeof(struct pkt_dbg_tx_info));
    ret = rtk_pkt_txCmd_get(&txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command get failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        txInfo.ipcs = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        txInfo.ipcs = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCmd_set(txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_tx_cmd_ipcs_enable_disable */

/*
 * debug packet tx set tx-cmd l4cs ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_tx_cmd_l4cs_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_tx_info txInfo;

    DIAG_UTIL_PARAM_CHK();

    memset(&txInfo, 0, sizeof(struct pkt_dbg_tx_info));
    ret = rtk_pkt_txCmd_get(&txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command get failed %d\n",__FUNCTION__ ,__LINE__,ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        txInfo.l4cs = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        txInfo.l4cs = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCmd_set(txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command set failed %d\n", __FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_tx_cmd_l4cs_enable_disable */

/*
 * debug packet tx set tx-cmd vlan <UINT:vid> <UINT:priority> <UINT:cfi>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_tx_cmd_vlan_vid_priority_cfi(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *priority_ptr,
    uint32_t  *cfi_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_tx_info txInfo;

    DIAG_UTIL_PARAM_CHK();

    if(*vid_ptr > 0xfff)
    {
        diag_util_printf("vid out of range %d\n", *vid_ptr);
        return CPARSER_NOT_OK;
    }

    if(*priority_ptr > 0b111)
    {
        diag_util_printf("priority out of range %d\n", *priority_ptr);
        return CPARSER_NOT_OK;
    }

    if(*cfi_ptr > 0b1)
    {
        diag_util_printf("cfi out of range %d\n", *cfi_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&txInfo, 0, sizeof(struct pkt_dbg_tx_info));
    ret = rtk_pkt_txCmd_get(&txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command get failed %d\n",__FUNCTION__ ,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
    txInfo.vidl = *vid_ptr & 0xff;
    txInfo.vidh = (*vid_ptr & 0xf00) >> 8;
    txInfo.prio = *priority_ptr & 0b111;
    txInfo.cfi = *cfi_ptr & 0b1;

    ret = rtk_pkt_txCmd_set(txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_tx_cmd_vlan_vid_priority_cfi */

/*
 * debug packet tx set tx-cmd clear
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_tx_cmd_clear(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_tx_info txInfo;

    DIAG_UTIL_PARAM_CHK();

    memset(&txInfo, 0, sizeof(txInfo));

    ret = rtk_pkt_txCmd_set(txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_tx_cmd_clear */

/*
 * debug packet tx get tx-cmd
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_tx_cmd(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    //rtk_enable_t enable;
    struct pkt_dbg_tx_info txInfo;

    DIAG_UTIL_PARAM_CHK();

    //diag_util_printf("tx command: %s\n", (enable == ENABLED) ? "enable" : "disable" );

    memset(&txInfo, 0, sizeof(struct pkt_dbg_tx_info));
    ret = rtk_pkt_txCmd_get(&txInfo);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx command get failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("ipcs l4cs vlanid pri cfi\n");
    diag_util_printf("%4u %4u %6u %3u %3u\n",
                     txInfo.ipcs,
                     txInfo.l4cs,
                     txInfo.vidl | (txInfo.vidh << 8),
                     txInfo.prio,
                     txInfo.cfi);
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_get_tx_cmd */

/*
 * debug packet tx set lso-param lenfix-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_lenfix_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.lenfix_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.lenfix_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.lenfix_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_lenfix_en_enable_disable */

/*
 * debug packet tx set lso-param bypass-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_bypass_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.bypass_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.bypass_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.bypass_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_bypass_en_enable_disable */

/*
 * debug packet tx set lso-param udp-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_udp_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.udp_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.udp_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.udp_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_udp_en_enable_disable */

/*
 * debug packet tx set lso-param tcp-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_tcp_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.tcp_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.tcp_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.tcp_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_tcp_en_enable_disable */

/*
 * debug packet tx set lso-param ipv6-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_ipv6_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.ipv6_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.ipv6_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.ipv6_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_ipv6_en_enable_disable */

/*
 * debug packet tx set lso-param ipv4-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_ipv4_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.ipv4_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.ipv4_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.ipv4_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_ipv4_en_enable_disable */

/*
 * debug packet tx set lso-param segment-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_segment_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.segment_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.segment_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.segment_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_segment_en_enable_disable */

/*
 * debug packet tx set lso-param unknow-tag-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_unknow_tag_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    if('e' == TOKEN_CHAR(6,0))
    {
        param.unknow_tag_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        param.unknow_tag_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    param_valid.unknow_tag_en = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n", __FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_unknow_tag_en_enable_disable */

/*
 * debug packet tx set lso-param segment-size <UINT:segment_size>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_segment_size_segment_size(
    cparser_context_t *context,
    uint32_t  *seegment_size_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    param.segment_size = *seegment_size_ptr;
    param_valid.segment_size = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_segment_size_segment_size */

/*
 * debug packet tx set lso-param packet_size <UINT:packet_size>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_lso_param_packet_size_packet_size(
    cparser_context_t *context,
    uint32_t  *packet_size_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&param, 0, sizeof(struct pkt_dbg_lso_param));
    memset(&param_valid, 0, sizeof(struct pkt_dbg_lso_param));

    param.packetSize = *packet_size_ptr;
    param_valid.packetSize = 1;

    ret = rtk_pkt_txLsoParam_set(param, param_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_lso_param_packet_size_packet_size */

/*
 * debug packet tx clear lso-param
 */
cparser_result_t
cparser_cmd_debug_packet_tx_clear_lso_param(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_lso_param param, param_valid;

    DIAG_UTIL_PARAM_CHK();

    ret = rtk_pkt_txLsoParam_clear();
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter clear failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_clear_lso_param */

/*
 * debug packet tx get lso-param
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_lso_param(
    cparser_context_t *context)
{
    return CPARSER_OK;
}

/*
 * debug packet tx set header-a cmd-asel <UINT:cmd_asel>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_cmd_asel_cmd_asel(
    cparser_context_t *context,
    uint32_t  *cmd_asel)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.cmd_asel = *cmd_asel;
    header_valid.cmd_asel = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_cmd_asel_cmd_asel */

/*
 * debug packet tx set header-a cmd-op <UINT:cmd_op>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_cmd_op_cmd_op(
    cparser_context_t *context,
    uint32_t  *cmd_op)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.cmd_op = *cmd_op;
    header_valid.cmd_op = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_cmd_op_cmd_op */

/*
 * debug packet tx set header-a cmd-mode <UINT:cmd_mode>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_cmd_mode_cmd_mode(
    cparser_context_t *context,
    uint32_t  *cmd_mode)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.cmd_mode = *cmd_mode;
    header_valid.cmd_mode = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_cmd_mode_cmd_mode */

/*
 * debug packet tx set header-a ptp-type <UINT:ptp_type>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_ptp_type_ptp_type(
    cparser_context_t *context,
    uint32_t  *ptp_type)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.ptp_type = *ptp_type;
    header_valid.ptp_type = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_ptp_type_ptp_type */

/*
 * debug packet tx set header-a cmd-offset <UINT:cmd_offset>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_cmd_offset_cmd_offset(
    cparser_context_t *context,
    uint32_t  *cmd_offset)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.cmd_offset = *cmd_offset;
    header_valid.cmd_offset = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_cmd_offset_cmd_offset */

/*
 * debug packet tx set header-a udp_cksum_offset <UINT:udp_cksum_offset>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_udp_cksum_offset_udp_cksum_offset(
    cparser_context_t *context,
    uint32_t  *udp_cksum_offset)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.udp_cksum_offset = *udp_cksum_offset;
    header_valid.udp_cksum_offset = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_udp_cksum_offset_udp_cksum_offset */

/*
 * debug packet tx set header-a cpu-flag <UINT:cpu_flag>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_cpu_flag_cpu_flag(
    cparser_context_t *context,
    uint32_t  *cpu_flag)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.cpu_flg = *cpu_flag;
    header_valid.cpu_flg = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_cpu_flag_cpu_flag */

/*
 * debug packet tx set header-a deep-q <UINT:deep_q>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_deep_q_deep_q(
    cparser_context_t *context,
    uint32_t  *deep_q)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.deep_q = *deep_q;
    header_valid.deep_q = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_deep_q_deep_q */

/*
 * debug packet tx set header-a pol-grp-id <UINT:pol_grp_id>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_pol_grp_id_pol_grp_id(
    cparser_context_t *context,
    uint32_t  *pol_grp_id)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.pol_grp_id = *pol_grp_id;
    header_valid.pol_grp_id = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_pol_grp_id_pol_grp_id */

/*
 * debug packet tx set header-a pol-id <UINT:pol_id>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_pol_id_pol_id(
    cparser_context_t *context,
    uint32_t  *pol_id)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.pol_id = *pol_id;
    header_valid.pol_id = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_pol_id_pol_id */

/*
 * debug packet tx set header-a pol-en ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_pol_en_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    if('e' == TOKEN_CHAR(6,0))
    {
        header.pol_en = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        header.pol_en = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    header_valid.pol_en = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_pol_en_enable_disable */

/*
 * debug packet tx set header-a mark <UINT:mark>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_mark_mark(
    cparser_context_t *context,
    uint32_t  *mark)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.mark = *mark;
    header_valid.mark = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_mark_mark */

/*
 * debug packet tx set header-a mirror <UINT:mirror>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_mirror_mirror(
    cparser_context_t *context,
    uint32_t  *mirror)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.mirror = *mirror;
    header_valid.mirror = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_mirror_mirror */

/*
 * debug packet tx set header-a no-drop ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_no_drop_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    if('e' == TOKEN_CHAR(6,0))
    {
        header.no_drop = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        header.no_drop = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    header_valid.no_drop = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n",__FUNCTION__,__LINE__, ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_no_drop_enable_disable */

/*
 * debug packet tx set header-a rx-pkt-type <UINT:rx_pkt_type>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_rx_pkt_type_rx_pkt_type(
    cparser_context_t *context,
    uint32_t  *rx_pkt_type)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.rx_pkt_type = *rx_pkt_type;
    header_valid.rx_pkt_type = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_rx_pkt_type_rx_pkt_type */

/*
 * debug packet tx set header-a drop-code <UINT:drop_code>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_drop_code_drop_code(
    cparser_context_t *context,
    uint32_t  *drop_code)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.drop_code = *drop_code;
    header_valid.drop_code = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_drop_code_drop_code */

/*
 * debug packet tx set header-a mcgid <UINT:mcgid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_mcgid_mcgid(
    cparser_context_t *context,
    uint32_t  *mcgid)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.mcgid = *mcgid;
    header_valid.mcgid = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_mcgid_mcgid */

/*
 * debug packet tx set header-a hdr-type <UINT:hdr_type>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_hdr_type_hdr_type(
    cparser_context_t *context,
    uint32_t  *hdr_type)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.hdr_type = *hdr_type;
    header_valid.hdr_type = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_hdr_type_hdr_type */

/*
 * debug packet tx set header-a fe-bypass <UINT:fe_bypass>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_fe_bypass_fe_bypass(
    cparser_context_t *context,
    uint32_t  *fe_bypass)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.fe_bypass = *fe_bypass;
    header_valid.fe_bypass = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_fe_bypass_fe_bypass */

/*
 * debug packet tx set header-a pkt-size <UINT:pkt_size>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_pkt_size_pkt_size(
    cparser_context_t *context,
    uint32_t  *pkt_size)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.pkt_size = *pkt_size;
    header_valid.pkt_size = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_pkt_size_pkt_size */

/*
 * debug packet tx set header-a lspid <UINT:lspid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_lspid_lspid(
    cparser_context_t *context,
    uint32_t  *lspid)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.lspid = *lspid;
    header_valid.lspid = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_lspid_lspid */

/*
 * debug packet tx set header-a ldpid <UINT:ldpid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_ldpid_ldpid(
    cparser_context_t *context,
    uint32_t  *ldpid)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.ldpid = *ldpid;
    header_valid.ldpid = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_ldpid_ldpid */

/*
 * debug packet tx set header-a cos <UINT:cos>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_header_a_cos_cos(
    cparser_context_t *context,
    uint32_t  *cos)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();

    memset(&header, 0, sizeof(struct pkt_dbg_header_a));
    memset(&header_valid, 0, sizeof(struct pkt_dbg_header_a));

    header.cos = *cos;
    header_valid.cos = 1;

    ret = rtk_pkt_txHeaderA_set(header, header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx header a set failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_header_a_cos_cos */

/*
 * debug packet tx clear header-a
 */
cparser_result_t
cparser_cmd_debug_packet_tx_clear_header_a(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    ret = rtk_pkt_txHeaderA_clear();
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter clear failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_clear_header_a */

/*
 * debug packet tx get header-a
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_header_a(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_header_a header, header_valid;

    DIAG_UTIL_PARAM_CHK();
    ret = rtk_pkt_txHeaderA_get(&header, &header_valid);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d tx lso parameter get failed %d\n", __FUNCTION__,__LINE__,ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("header-a\n");
    diag_util_printf("%-17s\n", "Fields");
    if(header_valid.cmd_asel)
    {
        diag_util_printf("%-17s: 0x%08x\n", "cmd_asel", header.cmd_asel);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "cmd_asel", "-");
    }
    if(header_valid.cmd_op)
    {
        diag_util_printf("%-17s: 0x%08x\n", "cmd_op", header.cmd_op);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "cmd_op", "-");
    }
    if(header_valid.cmd_mode)
    {
        diag_util_printf("%-17s: 0x%08x\n", "cmd_mode", header.cmd_mode);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "cmd_mode", "-");
    }
    if(header_valid.ptp_type)
    {
        diag_util_printf("%-17s: 0x%08x\n", "ptp_type", header.ptp_type);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "ptp_type", "-");
    }
    if(header_valid.cmd_offset)
    {
        diag_util_printf("%-17s: 0x%08x\n", "cmd_offset", header.cmd_offset);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "cmd_offset", "-");
    }
    if(header_valid.udp_cksum_offset)
    {
        diag_util_printf("%-17s: 0x%08x\n", "udp_cksum_offset", header.udp_cksum_offset);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "udp_cksum_offset", "-");
    }
    if(header_valid.cpu_flg)
    {
        diag_util_printf("%-17s: 0x%08x\n", "cpu_flg", header.cpu_flg);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "cpu_flg", "-");
    }
    if(header_valid.deep_q)
    {
        diag_util_printf("%-17s: 0x%08x\n", "deep_q", header.deep_q);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "deep_q", "-");
    }
    if(header_valid.pol_grp_id)
    {
        diag_util_printf("%-17s: 0x%08x\n", "pol_grp_id", header.pol_grp_id);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "pol_grp_id", "-");
    }
    if(header_valid.pol_id)
    {
        diag_util_printf("%-17s: 0x%08x\n", "pol_id", header.pol_id);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "pol_id", "-");
    }
    if(header_valid.pol_en)
    {
        diag_util_printf("%-17s: 0x%08x\n", "pol_en", header.pol_en);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "pol_en", "-");
    }
    if(header_valid.mark)
    {
        diag_util_printf("%-17s: 0x%08x\n", "mark", header.mark);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "mark", "-");
    }
    if(header_valid.mirror)
    {
        diag_util_printf("%-17s: 0x%08x\n", "mirror", header.mirror);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "mirror", "-");
    }
    if(header_valid.no_drop)
    {
        diag_util_printf("%-17s: 0x%08x\n", "no_drop", header.no_drop);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "no_drop", "-");
    }
    if(header_valid.rx_pkt_type)
    {
        diag_util_printf("%-17s: 0x%08x\n", "rx_pkt_type", header.rx_pkt_type);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "rx_pkt_type", "-");
    }
    if(header_valid.drop_code)
    {
        diag_util_printf("%-17s: 0x%08x\n", "drop_code", header.drop_code);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "drop_code", "-");
    }
    if(header_valid.mcgid)
    {
        diag_util_printf("%-17s: 0x%08x\n", "mcgid", header.mcgid);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "mcgid", "-");
    }
    if(header_valid.hdr_type)
    {
        diag_util_printf("%-17s: 0x%08x\n", "hdr_type", header.hdr_type);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "hdr_type", "-");
    }
    if(header_valid.fe_bypass)
    {
        diag_util_printf("%-17s: 0x%08x\n", "fe_bypass", header.fe_bypass);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "fe_bypass", "-");
    }
    if(header_valid.pkt_size)
    {
        diag_util_printf("%-17s: 0x%08x\n", "pkt_size", header.pkt_size);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "pkt_size", "-");
    }
    if(header_valid.lspid)
    {
        diag_util_printf("%-17s: 0x%08x\n", "lspid", header.lspid);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "lspid", "-");
    }
    if(header_valid.ldpid)
    {
        diag_util_printf("%-17s: 0x%08x\n", "ldpid", header.ldpid);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "ldpid", "-");
    }
    if(header_valid.cos)
    {
        diag_util_printf("%-17s: 0x%08x\n", "cos", header.cos);
    }
    else
    {
        diag_util_printf("%-17s: %6s\n", "cos", "-");
    }

#else
    diag_util_printf("command not support in this mode\n");
#endif
    return CPARSER_OK;
}

/*
 * debug register-rw-test register <UINT:address> data <UINT:value> count <UINT:count>
 */
cparser_result_t
cparser_cmd_debug_register_rw_test_register_address_data_value_count_count(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *value_ptr,
    uint32_t  *count_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    uint32  rValue  = 0;
    int32   ret = RT_ERR_FAILED;
    int32   i;
    int32   isSocMem = 0;


    DIAG_UTIL_PARAM_CHK();

    mem = *address_ptr;
    value = *value_ptr;

    if(mem >= 0x01000000)
        isSocMem = 1;/*soc memory*/
    else
        isSocMem = 0;

    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    for(i=0 ; i< *count_ptr ; i++)
    {
        if(isSocMem)
        {
            DIAG_UTIL_ERR_CHK(ioal_socMem32_write(mem, 0x0), ret);
            DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &rValue), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_write(mem, 0x0), ret);
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &rValue), ret);
        }
        if(rValue != 0x0)
        {
            diag_util_printf("\n\rError! data not match write:0x%8.8x read:0x%8.8x\n\r\n\r",0x0,rValue);
            return CPARSER_NOT_OK;
        }

        if(isSocMem)
        {
            DIAG_UTIL_ERR_CHK(ioal_socMem32_write(mem, value), ret);
            DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &rValue), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_write(mem, value), ret);
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &rValue), ret);
        }

        if(rValue != value)
        {
            diag_util_printf("\n\rError! fail at count:%d data not match write:0x%8.8x read:0x%8.8x\n\r\n\r",i,value,rValue);
            return CPARSER_NOT_OK;
        }


    }

    diag_util_printf("\n\rTest pass.\n\r\n\r");
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_register_rw_test_register_address_data_value_count_count */




#if defined(CMD_DEBUG_SET_PRINT_PORT_MUX_MUX)
/*
 * debug set print-port-mux <UINT:mux>
 */
cparser_result_t
cparser_cmd_debug_set_print_port_mux_mux(
    cparser_context_t *context,
    uint32_t  *mux_ptr)
{
    DIAG_UTIL_PARAM_CHK();


#if defined(CYGWIN_MDIO_IO)
    MDC_PIN_DEF(3,0,4,1,*mux_ptr);
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_print_port_mux_mux */
#endif

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * debug set ( dbgo_wrap_gphy | dbgo_sys_gen | dbgo_ctrlckt | dbgo_misc | dbg_rrcp_o | dbg_rldp_o )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_wrap_gphy_dbgo_sys_gen_dbgo_ctrlckt_dbgo_misc_dbg_rrcp_o_dbg_rldp_o(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_wrap_gphy"))
    {
        value = 0x0000;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_sys_gen"))
    {
        value = 0x0100;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_ctrlckt"))
    {
        value = 0x0300;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_misc"))
    {
        value = 0x0500;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_rrcp_o"))
    {
        value = 0x0500;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_rldp_o"))
    {
        value = 0x0501;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_wrap_gphy_dbgo_sys_gen_dbgo_ctrlckt_dbgo_misc_dbg_rrcp_o_dbg_rldp_o */

/*
 * debug set ( dbg_led_o | dbg_rtct_o | dbg_mib_o | dbg_phy_sts_o | dbg_intrpt_o | dbg_afbk_o | dbg_diag_o | dbgo_hwpkt | dbgo_efuse )
 */
cparser_result_t
cparser_cmd_debug_set_dbg_led_o_dbg_rtct_o_dbg_mib_o_dbg_phy_sts_o_dbg_intrpt_o_dbg_afbk_o_dbg_diag_o_dbgo_hwpkt_dbgo_efuse(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbg_led_o"))
    {
        value = 0x0502;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_rtct_o"))
    {
        value = 0x0503;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_mib_o"))
    {
        value = 0x0504;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_phy_sts_o"))
    {
        value = 0x0505;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_intrpt_o"))
    {
        value = 0x0506;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_afbk_o"))
    {
        value = 0x0507;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_diag_o"))
    {
        value = 0x0508;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_hwpkt"))
    {
        value = 0x0509;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_efuse"))
    {
        value = 0x050a;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbg_led_o_dbg_rtct_o_dbg_mib_o_dbg_phy_sts_o_dbg_intrpt_o_dbg_afbk_o_dbg_diag_o_dbgo_hwpkt_dbgo_efuse */

/*
 * debug set ( dbgo_wrap_sds | dbgo_chip_clk_gen_0 | dbgo_chip_clk_gen_1 | dbgo_chip_clk_gen_2 | dbgo_chip_rst_gen )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_wrap_sds_dbgo_chip_clk_gen_0_dbgo_chip_clk_gen_1_dbgo_chip_clk_gen_2_dbgo_chip_rst_gen(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_wrap_sds"))
    {
        value = 0x0800;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_clk_gen_0"))
    {
        value = 0x0900;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_clk_gen_1"))
    {
        value = 0x0a00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_clk_gen_2"))
    {
        value = 0x0b00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_rst_gen"))
    {
        value = 0x0c00;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_wrap_sds_dbgo_chip_clk_gen_0_dbgo_chip_clk_gen_1_dbgo_chip_clk_gen_2_dbgo_chip_rst_gen */

/*
 * debug set ( dbgo_chip_misc | dbgo_cfgif | dbgo_soc | dbgo_ssc | dbgo_pll_root | dbgo_ponctrl | dbgo_clkctrl )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_chip_misc_dbgo_cfgif_dbgo_soc_dbgo_ssc_dbgo_pll_root_dbgo_ponctrl_dbgo_clkctrl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_misc"))
    {
        value = 0x0d00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_cfgif"))
    {
        value = 0x0e00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_soc"))
    {
        value = 0x1000;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_ssc"))
    {
        value = 0x1100;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_pll_root"))
    {
        value = 0x1200;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_ponctrl"))
    {
        value = 0x1300;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_clkctrl"))
    {
        value = 0x1400;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_chip_misc_dbgo_cfgif_dbgo_soc_dbgo_ssc_dbgo_pll_root_dbgo_ponctrl_dbgo_clkctrl */

/*
 * debug set ( dbgo_rstgen | dbgo_clkgen | dbgo_sw_lxslv | PLLTOUT )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_rstgen_dbgo_clkgen_dbgo_sw_lxslv_PLLTOUT(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_rstgen"))
    {
        value = 0x1500;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_clkgen"))
    {
        value = 0x1600;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_sw_lxslv"))
    {
        value = 0x1700;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"PLLTOUT"))
    {
        value = 0x1800;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_rstgen_dbgo_clkgen_dbgo_sw_lxslv_plltout */

/*
 * debug set dbgo_regctrl
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_regctrl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0200;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_regctrl */

/*
 * debug set dbgo_regctrl ( dbgo_swarb | dbgo_gphyarb | dbgo_smimst | dbgo_iicmst | dbgo_iicmst_1 ) bits-3-0 <UINT:bits_3_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_regctrl_dbgo_swarb_dbgo_gphyarb_dbgo_smimst_dbgo_iicmst_dbgo_iicmst_1_bits_3_0_bits_3_0(
    cparser_context_t *context,
    uint32_t  *bits_3_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0200;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_3_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_swarb"))
            {
                value = 0x0050 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_gphyarb"))
            {
                value = 0x0060 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(2),"dbgo_smimst"))
            {
                value = 0x0070 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(2),"dbgo_iicmst"))
            {
                value = 0x00a0 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(2),"dbgo_iicmst_1"))
            {
                value = 0x00b0 | value;
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_3_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_swarb"))
            {
                value = 0x0050 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_gphyarb"))
            {
                value = 0x0060 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(2),"dbgo_smimst"))
            {
                value = 0x0070 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(2),"dbgo_iicmst"))
            {
                value = 0x00a0 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(2),"dbgo_iicmst_1"))
            {
                value = 0x00b0 | value;
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_regctrl_dbgo_swarb_dbgo_gphyarb_dbgo_smimst_dbgo_iicmst_dbgo_iicmst_1_bits_3_0_bits_3_0 */

/*
 * debug set dbgo_fctrl
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_fctrl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0400;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_fctrl */

/*
 * debug set dbgo_fctrl ( dbgo_ingress | dbgo_egress ) bits-3-0 <UINT:bits_3_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_fctrl_dbgo_ingress_dbgo_egress_bits_3_0_bits_3_0(
    cparser_context_t *context,
    uint32_t  *bits_3_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0400;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_3_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_ingress"))
            {
                value = 0x0000 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_egress"))
            {
                value = 0x0010 | value;
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_3_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_ingress"))
            {
                value = 0x0000 | value;
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_egress"))
            {
                value = 0x0010 | value;
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_fctrl_dbgo_ingress_dbgo_egress_bits_3_0_bits_3_0 */

/*
 * debug set dbgo_swcore_cen
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_swcore_cen(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0600;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_swcore_cen */

/*
 * debug set dbgo_swcore_cen ( dbgo_sel_sch | dbgo_sel_outq | dbgo_sel_mtr | dbgo_sel_hsactrl | dbgo_sel_inq | dbgo_sel_out_drp ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_dbgo_sel_outq_dbgo_sel_mtr_dbgo_sel_hsactrl_dbgo_sel_inq_dbgo_sel_out_drp_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0600;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_sch"))
            {
                value = 0x0100 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_outq"))
            {
                value = 0x0200 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_mtr"))
            {
                value = 0x0300 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_hsactrl"))
            {
                value = 0x0400 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_inq"))
            {
                value = 0x0500 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_out_drp"))
            {
                value = 0x0600 | (value&0xFF);
            }
            
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_sch"))
            {
                value = 0x0100 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_outq"))
            {
                value = 0x0200 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_mtr"))
            {
                value = 0x0300 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_hsactrl"))
            {
                value = 0x0400 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_inq"))
            {
                value = 0x0500 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_out_drp"))
            {
                value = 0x0600 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_dbgo_sel_outq_dbgo_sel_mtr_dbgo_sel_hsactrl_dbgo_sel_inq_dbgo_sel_out_drp_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_swcore_cen ( dbgo_sel_sch_pon | dbg_dpm_o | dbg_l2_o | dbg_acl_o | dbg_misc_o ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_pon_dbg_dpm_o_dbg_l2_o_dbg_acl_o_dbg_misc_o_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0600;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_sch_pon"))
            {
                value = 0x0700 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_dpm_o"))
            {
                value = 0x0000 | (value&0x3F);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_l2_o"))
            {
                value = 0x0080 | (value&0xF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_acl_o"))
            {
                value = 0x0090 | (value&0xF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_misc_o"))
            {
                value = 0x00a0 | (value&0xF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_sch_pon"))
            {
                value = 0x0700 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_dpm_o"))
            {
                value = 0x0000 | (value&0x3F);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_l2_o"))
            {
                value = 0x0080 | (value&0xF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_acl_o"))
            {
                value = 0x0090 | (value&0xF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"dbg_misc_o"))
            {
                value = 0x00a0 | (value&0xF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_pon_dbg_dpm_o_dbg_l2_o_dbg_acl_o_dbg_misc_o_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac */

/*
 * debug set dbgo_mac ( p0_dbgo_tx | p0_dbgo_rx | p0_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p0_dbgo_tx_p0_dbgo_rx_p0_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_tx"))
            {
                value = 0x0000 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_rx"))
            {
                value = 0x0100 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_eee"))
            {
                value = 0x0200 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_tx"))
            {
                value = 0x0000 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_rx"))
            {
                value = 0x0100 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_eee"))
            {
                value = 0x0200 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p0_dbgo_tx_p0_dbgo_rx_p0_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p1_dbgo_tx | p1_dbgo_rx | p1_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p1_dbgo_tx_p1_dbgo_rx_p1_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_tx"))
            {
                value = 0x0400 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_rx"))
            {
                value = 0x0500 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_eee"))
            {
                value = 0x0600 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_tx"))
            {
                value = 0x0400 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_rx"))
            {
                value = 0x0500 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_eee"))
            {
                value = 0x0600 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p1_dbgo_tx_p1_dbgo_rx_p1_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p2_dbgo_tx | p2_dbgo_rx | p2_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p2_dbgo_tx_p2_dbgo_rx_p2_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_tx"))
            {
                value = 0x0800 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_rx"))
            {
                value = 0x0900 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_eee"))
            {
                value = 0x0a00 | (value&0xFF);
            }
            
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_tx"))
            {
                value = 0x0800 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_rx"))
            {
                value = 0x0900 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_eee"))
            {
                value = 0x0a00 | (value&0xFF);
            }
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p2_dbgo_tx_p2_dbgo_rx_p2_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p3_dbgo_tx | p3_dbgo_rx | p3_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p3_dbgo_tx_p3_dbgo_rx_p3_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_tx"))
            {
                value = 0x0c00 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_rx"))
            {
                value = 0x0d00 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_eee"))
            {
                value = 0x0e00 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_tx"))
            {
                value = 0x0c00 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_rx"))
            {
                value = 0x0d00 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_eee"))
            {
                value = 0x0e00 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p3_dbgo_tx_p3_dbgo_rx_p3_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p4_dbgo_tx | p4_dbgo_rx | p4_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p4_dbgo_tx_p4_dbgo_rx_p4_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);

            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_tx"))
            {
                value = 0x1000 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_rx"))
            {
                value = 0x1100 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_eee"))
            {
                value = 0x1200 | (value&0xFF);
            }            
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_tx"))
            {
                value = 0x1000 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_rx"))
            {
                value = 0x1100 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_eee"))
            {
                value = 0x1200 | (value&0xFF);
            }            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p4_dbgo_tx_p4_dbgo_rx_p4_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p5_dbgo_tx | p5_dbgo_rx | p5_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p5_dbgo_tx_p5_dbgo_rx_p5_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_tx"))
            {
                value = 0x1400 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_rx"))
            {
                value = 0x1500 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_eee"))
            {
                value = 0x1600 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_tx"))
            {
                value = 0x1400 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_rx"))
            {
                value = 0x1500 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_eee"))
            {
                value = 0x1600 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p5_dbgo_tx_p5_dbgo_rx_p5_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p6_dbgo_tx | p6_dbgo_rx | p6_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p6_dbgo_tx_p6_dbgo_rx_p6_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_tx"))
            {
                value = 0x1800 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_rx"))
            {
                value = 0x1900 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_eee"))
            {
                value = 0x1a00 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            value = *bits_7_0_ptr;
            if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_tx"))
            {
                value = 0x1800 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_rx"))
            {
                value = 0x1900 | (value&0xFF);
            }
            else if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_eee"))
            {
                value = 0x1a00 | (value&0xFF);
            }
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p6_dbgo_tx_p6_dbgo_rx_p6_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set ( dbgo_m1_0 | dbgo_m1_1 | dbgo_m1_2 | dbgo_m1_3 | dbgo_m1_4 )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_m1_0_dbgo_m1_1_dbgo_m1_2_dbgo_m1_3_dbgo_m1_4(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32  val;

    DIAG_UTIL_PARAM_CHK();
/*
debug m1-0 -> 0xbb000158 bit27~bit20 write 0x25
debug m1-0 -> 0xbb000158 bit27~bit20 write 0x32
debug m1-0 -> 0xbb000158 bit27~bit20 write 0x26
debug m1-0 -> 0xbb000158 bit27~bit20 write 0x32
debug m1-0 -> 0xbb000158 bit27~bit20 write 0x38
*/
    if(!osal_strcmp(TOKEN_STR(2),"dbgo_m1_0"))
    {
        value = 0x25;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_m1_1"))
    {
        value = 0x32;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_m1_2"))
    {
        value = 0x26;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_m1_3"))
    {
        value = 0x32;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_m1_4"))
    {
        value = 0x38;
    }


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            val = value &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBG_BY_EXTf, &val), ret);
            val = (value >> 1) &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBG_BY_SLICf, &val), ret);
            val = (value >> 2) &0x3;
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBG_BY_OEMf, &val), ret);
            val = (value >> 4) &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBG_BY_SPIf, &val), ret);
            val = (value >> 5) &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGEN_BY_REGf, &val), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            val = value &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBG_BY_EXTf, &val), ret);
            val = (value >> 1) &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBG_BY_SLICf, &val), ret);
            val = (value >> 2) &0x3;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBG_BY_OEMf, &val), ret);
            val = (value >> 4) &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBG_BY_SPIf, &val), ret);
            val = (value >> 5) &0x1;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGEN_BY_REGf, &val), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_m1_0_dbgo_m1_1_dbgo_m1_2_dbgo_m1_3_dbgo_m1_4 */

#endif


/*
 * debug set hsb latch-mode ( all | none | first-drop | first-pass | first-trap | drop | trap | acl | reason)
 */
cparser_result_t
cparser_cmd_debug_set_hsb_latch_mode_all_none_first_drop_first_pass_first_trap_drop_trap_acl_reason(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    uint32  latchNoneValue  = 1;

    DIAG_UTIL_PARAM_CHK();


    if(!osal_strcmp(TOKEN_STR(4),"all"))
    {
        value = 0x0;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"none"))
    {
        value = 0x1;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-drop"))
    {
        value = 0x2;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-pass"))
    {
        value = 0x3;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-trap"))
    {
        value = 0x4;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)     
#else
    else if(!osal_strcmp(TOKEN_STR(4),"drop"))
    {
        value = 0x5;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"trap"))
    {
        value = 0x6;
    }
#endif  
#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP) || defined(CONFIG_SDK_RTL9601B)

    else if(!osal_strcmp(TOKEN_STR(4),"acl"))
    {
        value = 0x7;
    }
#else
    else if(!osal_strcmp(TOKEN_STR(4),"reason"))
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)   
        value = 0x5;
#else
        value = 0x7;
#endif
    }
#endif
    else
    {
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            /*change latch mode must set to  latch none first to clear valid bit*/
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &latchNoneValue), ret);

            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
            case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &latchNoneValue), ret);

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_MODEf, &latchNoneValue), ret);
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_MODEf, &latchNoneValue), ret);
           
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_MODEf, &latchNoneValue), ret);
           
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_MODEf, &value), ret);
            break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_latch_mode_all_none_first_drop_first_pass_first_trap_drop_trap_acl_reason */


/*
 * debug set hsb source ( user-data | mac-parser )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_source_user_data_mac_parser(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(4),"user-data"))
    {
        value = 0x1;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"mac-parser"))
    {
        value = 0x0;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_SELf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_SELf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_SELf, &value), ret);
            return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_source_user_data_mac_parser */


/*
 * debug get hsb valid
 */
cparser_result_t
cparser_cmd_debug_get_hsb_valid(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:

        DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_HSB_CTRLr,APOLLOMP_VALIDf, &value), ret);
        diag_util_mprintf("Valid: %d\n",value);
        break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:

        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9601B_HSB_CTRLr,RTL9601B_VALIDf, &value), ret);
        diag_util_mprintf("Valid: %d\n",value);
        break;
#endif

#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_HSB_CTRLr,RTL9602C_VALIDf, &value), ret);
        diag_util_mprintf("Valid: %d\n",value);

        break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_HSB_CTRLr,RTL9607C_VALIDf, &value), ret);
        diag_util_mprintf("Valid: %d\n",value);

        break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

        DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_HSB_CTRLr,RTL9603CVD_VALIDf, &value), ret);
        diag_util_mprintf("Valid: %d\n",value);

        break;
#endif
        default:
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_hsb_valid */

/*
 * debug set hsb user-data send-to-ale
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_send_to_ale(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();

    value = 1;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            apollomp_raw_hsbData_set(&diagHsbUserData);
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_VALIDf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            rtl9601b_hsbData_set(&diagHsbUserData);
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_VALIDf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            rtl9602c_hsbData_set(&diagHsbUserData);
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_WRITE_CMDf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_send_to_ale */


/*
 * debug show hsb user-data
 */
cparser_result_t
cparser_cmd_debug_show_hsb_user_data(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    rtk_hsb_t hsbData;;

    DIAG_UTIL_PARAM_CHK();

    _diag_debug_hsb_display(&diagHsbUserData);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_show_hsb_user_data */


/*
 * debug clear hsb user-data
 */
cparser_result_t
cparser_cmd_debug_clear_hsb_user_data(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    rtk_hsb_t hsbData;;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&diagHsbUserData,0x0,sizeof(rtk_hsb_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_clear_hsb_user_data */


/*
 * debug set hsb user-data ( spa | user-valid | tos | ip-type | ether-type | pkt-len | l4-type ) <UINT:data>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_spa_user_valid_tos_ip_type_ether_type_pkt_len_l4_type_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.spa = *data_ptr;
    }
    else if('u' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.user_valid = *data_ptr;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
    else if('t' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.tos_tc = *data_ptr;
    }
#else   
    
    else if('t' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.tos_dscp = *data_ptr;
    }
    else if('i' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ip_type = *data_ptr;
    }
#endif
    else if('e' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ether_type = *data_ptr;
    }
    else if('p' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.pkt_len = *data_ptr;
    }
    else if('l' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.l4_type = *data_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_spa_user_valid_tos_ip_type_ether_type_pkt_len_l4_type_data */

/*
 * debug set hsb user-data ( stag | ctag | pon-idx | len-of-nhs | ip-pro-nh | tc ) <UINT:data>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_stag_ctag_pon_idx_len_of_nhs_ip_pro_nh_tc_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.stag = *data_ptr;
    }
    else if('c' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ctag = *data_ptr;
    }
    else if('p' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.pon_idx = *data_ptr;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#else   
    
    else if('l' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.len_of_nhs = *data_ptr;
    }
    else if('i' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.l3proto_nh = *data_ptr;
    }
#endif
    else if('t' == TOKEN_CHAR(4,0))
    {
            switch(DIAG_UTIL_CHIP_TYPE)
            {
#ifdef CONFIG_SDK_APOLLOMP
                case APOLLOMP_CHIP_ID:
                    diagHsbUserData.tos_dscp = *data_ptr;
                    break;
#endif
#ifdef CONFIG_SDK_RTL9601B
                case RTL9601B_CHIP_ID:
                    diagHsbUserData.tos_dscp = *data_ptr;
                    break;
#endif
#ifdef CONFIG_SDK_RTL9602C
                case RTL9602C_CHIP_ID:
                    diagHsbUserData.tc = *data_ptr;
                    break;
#endif
#ifdef CONFIG_SDK_RTL9607C
                case RTL9607C_CHIP_ID:
                    diagHsbUserData.tos_tc = *data_ptr;
                    break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
                case RTL9603CVD_CHIP_ID:
                    diagHsbUserData.tos_tc = *data_ptr;
                    break;
#endif
                default:
                    break;
            }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_stag_ctag_pon_idx_len_of_nhs_ip_pro_nh_tc_data */


/*
 * debug set hsb user-data ( stag-if | ctag-if | igmp | udp | tcp | llc | pppoe )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_stag_if_ctag_if_igmp_udp_tcp_llc_pppoe(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.stag_if = ENABLED;
    }
    else if ('c' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ctag_if = ENABLED;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#else   
    else if ('i' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.igmp_if = ENABLED;
    }
    else if ('u' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.udp_if = ENABLED;
    }
    else if ('t' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.tcp_if = ENABLED;
    }
#endif
    else if ('l' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.llc_other = ENABLED;
    }
    else if ('p' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.pppoe_if = ENABLED;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_stag_if_ctag_if_igmp_udp_tcp_llc_pppoe */

/*
 * debug set hsb user-data ( snap | oam | ip-rsv-mc-addr | cpu-tag | nh-rg | l3-chk-ok | ttl-gt5 | rlpp | ip6 )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_snap_oam_ip_rsv_mc_addr_cpu_tag_nh_rg_l3_chk_ok_ttl_gt5_rlpp_ip6(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.snap_if = ENABLED;
    }
    else if ('o' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.oampdu = ENABLED;
    }
    else if ('c' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.cputag_if = ENABLED;
    }
    else if ('n' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ip6_nh_rg = ENABLED;
    }
    else if ('l' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.cks_ok_l3 = ENABLED;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)

	else if ('t' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ihl_gt5 = ENABLED;
    }
#else
	else if ('t' == TOKEN_CHAR(4,0))
	{
		diagHsbUserData.ttl_gt5 = ENABLED;
	}

#endif
	
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#else   
    else if ('i' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ip_rsv_mc_addr = ENABLED;
    }
    else if ('r' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.rlpp_if = ENABLED;
    }
#endif  
    else if ('i' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ip6_if = ENABLED;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_snap_oam_ip_rsv_mc_addr_cpu_tag_nh_rg_l3_chk_ok_ttl_gt5_rlpp_ip6 */

/*
 * debug set hsb user-data ( l4-chk-ok | ttl-gt1 | rldp | ptp | ip4 )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_l4_chk_ok_ttl_gt1_rldp_ptp_ip4(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if ('l' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.cks_ok_l4 = ENABLED;
    }
    else if ('t' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ttl_gt1 = ENABLED;
    }
    else if ('r' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.rldp_if= ENABLED;
    }
    else if ('p' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ptp_if = ENABLED;
    }
    else if ('i' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.ip4_if = ENABLED;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_l4_chk_ok_ttl_gt1_rldp_ptp_ip4 */

/*
 * debug set hsb cpu-tag ( psel | dislrn | prisel | keep | l3c | l4c | l2br | l34keep | vsel | efiden )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_cpu_tag_psel_dislrn_prisel_keep_l3c_l4c_l2br_l34keep_vsel_efiden(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(4),"psel"))
    {
        diagHsbUserData.cputag_psel = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"dislrn"))
    {
        diagHsbUserData.cputag_dislrn = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"prisel"))
    {
        diagHsbUserData.cputag_prisel = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"keep"))
    {
        diagHsbUserData.cputag_keep = ENABLED;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#else   
    else if(!osal_strcmp(TOKEN_STR(4),"l3c"))
    {
        diagHsbUserData.cputag_l3c = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"l4c"))
    {
        diagHsbUserData.cputag_l4c = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"l2br"))
    {
        diagHsbUserData.cputag_l2br = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"l34keep"))
    {
        diagHsbUserData.cputag_l34keep = ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"vsel"))
    {
        diagHsbUserData.cputag_vsel= ENABLED;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"efiden"))
    {
        diagHsbUserData.cputag_efid_en = ENABLED;
    }
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_cpu_tag_psel_dislrn_prisel_keep_l3c_l4c_l2br_l34keep_vsel_efiden */

/*
 * debug set hsb cpu-tag ( txportmask | priority | sb | extspa | pppoeact | pppoeidx | pon-sid | efid ) <UINT:data>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_cpu_tag_txportmask_priority_sb_extspa_pppoeact_pppoeidx_pon_sid_efid_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if(!osal_strcmp(TOKEN_STR(4),"txportmask"))
    {
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD) 
        diagHsbUserData.cputag_pmsk_7_0 = (*data_ptr) & 0xFF;
        diagHsbUserData.cputag_pmsk_10_8 = (*data_ptr) >> 8;

#else   
        diagHsbUserData.cputag_txpmsk = *data_ptr;
#endif
    }
    else if(!osal_strcmp(TOKEN_STR(4),"priority"))
    {
        diagHsbUserData.cputag_pri = *data_ptr;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"extspa"))
    {
        diagHsbUserData.cputag_extspa = *data_ptr;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"pppoeact"))
    {
        diagHsbUserData.cputag_pppoe_idx = *data_ptr;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"pppoeidx"))
    {
        diagHsbUserData.cputag_pppoe_act = *data_ptr;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"pon-sid"))
    {
        diagHsbUserData.cputag_pon_sid = *data_ptr;
    }
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#else   
    else if(!osal_strcmp(TOKEN_STR(4),"sb"))
    {
        diagHsbUserData.cputag_sb = *data_ptr;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"efid"))
    {
        diagHsbUserData.cputag_efid = *data_ptr;
    }
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_cpu_tag_txportmask_priority_sb_extspa_pppoeact_pppoeidx_pon_sid_efid_data */


/*
 * debug set hsb user-data user-field <UINT:index> <UINT:data>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_user_field_index_data(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *data_ptr)
{

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr) >= HAL_MAX_NUM_OF_FIELD_SELECTOR(), RT_ERR_OUT_OF_RANGE);

    if(*index_ptr == 0)
        diagHsbUserData.user_field_0 = *data_ptr;
    else if(*index_ptr == 1)
        diagHsbUserData.user_field_1 = *data_ptr;
    else if(*index_ptr == 2)
        diagHsbUserData.user_field_2 = *data_ptr;
    else if(*index_ptr == 3)
        diagHsbUserData.user_field_3 = *data_ptr;
    else if(*index_ptr == 4)
        diagHsbUserData.user_field_4 = *data_ptr;
    else if(*index_ptr == 5)
        diagHsbUserData.user_field_5 = *data_ptr;
    else if(*index_ptr == 6)
        diagHsbUserData.user_field_6 = *data_ptr;
    else if(*index_ptr == 7)
        diagHsbUserData.user_field_7 = *data_ptr;
#if defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD) 

#else
    else if(*index_ptr == 8)
        diagHsbUserData.user_field_8 = *data_ptr;
    else if(*index_ptr == 9)
        diagHsbUserData.user_field_9 = *data_ptr;
    else if(*index_ptr == 10)
        diagHsbUserData.user_field_10 = *data_ptr;
    else if(*index_ptr == 11)
        diagHsbUserData.user_field_11 = *data_ptr;
    else if(*index_ptr == 12)
        diagHsbUserData.user_field_12 = *data_ptr;
    else if(*index_ptr == 13)
        diagHsbUserData.user_field_13 = *data_ptr;
    else if(*index_ptr == 14)
        diagHsbUserData.user_field_14 = *data_ptr;
    else if(*index_ptr == 15)
        diagHsbUserData.user_field_15 = *data_ptr;
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_user_field_index_data */

/*
 * debug set hsb user-data ( dmac | smac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_dmac_smac_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4,0))
    {
        osal_memcpy(&diagHsbUserData.sa.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        osal_memcpy(&diagHsbUserData.da.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_dmac_smac_mac */

/*
 * debug set hsb user-data ( sip | dip ) <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_sip_dip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.sip = *ip_ptr;
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.dip = *ip_ptr;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_sip_dip_ip */

/*
 * debug set config
 */
cparser_result_t
cparser_cmd_debug_set_config(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_config_set(),ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_config */

/*
 * debug get config
 */
cparser_result_t
cparser_cmd_debug_get_config(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtl9602c_config_get(),ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_config */


#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * debug set hsb user-data ( pppoe-session | tpid ) <UINT:data>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_pppoe_session_tpid_data(
    cparser_context_t *context,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    if ('p' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.pppoe_session = *data_ptr;
    }
    else if('t' == TOKEN_CHAR(4,0))
    {
        diagHsbUserData.stag_tpid = *data_ptr;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_pppoe_session_tpid_data */

/*
 * debug set hsb user-data ( sip6 | dip6 ) <IPV6ADDR:ip6>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_user_data_sip6_dip6_ip6(
    cparser_context_t *context,
    char * *ip6_ptr)
{
    int32   ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if ('s' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&diagHsbUserData.sip6.ipv6_addr[0], TOKEN_STR(5)), ret);
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&diagHsbUserData.dip6.ipv6_addr[0], TOKEN_STR(5)), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_user_data_sip6_dip6_ip6 */

/*
 * debug set hsb egress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_egress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_EGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_EGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_EGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_egress_port_ports_all */

/*
 * debug set hsb ingress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_ingress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_IGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_IGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_IGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_ingress_port_ports_all */

/*
 * debug set hsb ingress port pbo ( exclude | include )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_ingress_port_pbo_exclude_include(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
	uint32 state = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    if ('i' == TOKEN_CHAR(6,0))
    {
        state = ENABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        state = DISABLED ;
    }


    switch(DIAG_UTIL_CHIP_TYPE)
    {

#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_IGR_PBOf, &state), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_IGR_PBOf, &state), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_ingress_port_pbo_exclude_include */


/*
 * debug set hsb reason <UINT:reason>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_reason_reason(
    cparser_context_t *context,
    uint32_t  *reason_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_HSA_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
        break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CTRLr,RTL9607C_LATCH_HSA_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
        break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CTRLr,RTL9603CVD_LATCH_HSA_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
        break;
#endif

        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_reason_reason */

/*
 * debug set fb egress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_debug_set_fb_egress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_FBHSA_CTRLr,RTL9607C_LATCH_FB_EGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_FBHSA_CTRLr,RTL9603CVD_LATCH_FB_EGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_fb_egress_port_ports_all */

/*
 * debug set fb ingress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_debug_set_fb_ingress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_FBHSA_CTRLr,RTL9607C_LATCH_FB_IGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_FBHSA_CTRLr,RTL9603CVD_LATCH_FB_IGR_PMSKf, &portlist.portmask.bits[0]), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_fb_ingress_port_ports_all */

/*
 * debug set fb reason <UINT:reason>
 */
cparser_result_t
cparser_cmd_debug_set_fb_reason_reason(
    cparser_context_t *context,
    uint32_t  *reason_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_FBHSA_CTRLr,RTL9607C_LATCH_FB_HSA_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
        break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_FBHSA_CTRLr,RTL9603CVD_LATCH_FB_HSA_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
        break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_fb_reason_reason */

/*
 * debug set fb latch-mode ( all | none | first-drop | first-pass | first-trap | reason)
 */
cparser_result_t
cparser_cmd_debug_set_fb_latch_mode_all_none_first_drop_first_pass_first_trap_reason(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();


    if(!osal_strcmp(TOKEN_STR(4),"all"))
    {
        value = 0x0;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"none"))
    {
        value = 0x1;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-drop"))
    {
        value = 0x2;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-pass"))
    {
        value = 0x3;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-trap"))
    {
        value = 0x4;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"reason"))
    {
        value = 0x5;
    }
    else
    {
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_FBHSA_CTRLr,RTL9607C_LATCH_FB_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_FBHSA_CTRLr,RTL9603CVD_LATCH_FB_MODEf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_fb_latch_mode_all_none_first_drop_first_pass_first_trap_reason */


/*
 * debug set hsd latch-mode ( all | none | first-pass | reason )
 */
cparser_result_t
cparser_cmd_debug_set_hsd_latch_mode_all_none_first_pass_reason(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(4),"all"))
    {
        value = 0x0;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"none"))
    {
        value = 0x1;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-pass"))
    {
        value = 0x2;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"reason"))
    {
        value = 0x3;
    }
    else
    {
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_NOT_OK;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
            case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSD_CFGr,RTL9602C_LATCH_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
            case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSD_CFGr,RTL9607C_LATCH_MODEf, &value), ret);
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
            case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSD_CFGr,RTL9603CVD_LATCH_MODEf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsd_latch_mode_all_none_first_pass_reason */

/*
 * debug set hsd egress port <UINT:port>
 */
cparser_result_t
cparser_cmd_debug_set_hsd_egress_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSD_CFGr,RTL9602C_LATCH_EGR_PORTf, port_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSD_CFGr,RTL9607C_LATCH_EGR_PORTf, port_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSD_CFGr,RTL9603CVD_LATCH_EGR_PORTf, port_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsd_egress_port_port */

/*
 * debug set hsd ingress port <UINT:port>
 */
cparser_result_t
cparser_cmd_debug_set_hsd_ingress_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSD_CFGr,RTL9602C_LATCH_IGR_PORTf, port_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSD_CFGr,RTL9607C_LATCH_IGR_PORTf, port_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSD_CFGr,RTL9603CVD_LATCH_IGR_PORTf, port_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsd_ingress_port_port */

/*
 * debug set hsd reason <UINT:reason>
 */
cparser_result_t
cparser_cmd_debug_set_hsd_reason_reason(
    cparser_context_t *context,
    uint32_t  *reason_ptr)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_HSD_CFGr,RTL9602C_LATCH_HSD_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSD_CFGr,RTL9607C_LATCH_HSD_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSD_CFGr,RTL9603CVD_LATCH_HSD_REASONf, reason_ptr), ret);
            return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsd_reason_reason */

/*
 * debug get hsd valid
 */
cparser_result_t
cparser_cmd_debug_get_hsd_valid(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {

#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9602C_HSD_CFGr,RTL9602C_VALIDf, &value), ret);
            diag_util_mprintf("Valid: %d\n",value);

            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_hsd_valid */

/*
 * debug set hsb invalid-ip-type ( forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_invalid_ip_type_forward_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();
    
    if ('f' == TOKEN_CHAR(4,0))
    {
        value = 0;
    }
    else if ('t' == TOKEN_CHAR(4,0))
    {
        value = 1;
    }
    else
        return CPARSER_NOT_OK;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_HSB_CFGr,RTL9607C_INVALID_IP_TYPE_ACTf, &value), ret);

            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_HSB_CFGr,RTL9603CVD_INVALID_IP_TYPE_ACTf, &value), ret);

            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_invalid_ip_type_forward_trap_to_cpu */

/*
 * debug get hsb invalid-ip-type
 */
cparser_result_t
cparser_cmd_debug_get_hsb_invalid_ip_type(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_HSB_CFGr,RTL9607C_INVALID_IP_TYPE_ACTf, &value), ret);
            diag_util_mprintf("%s\n",(0==value)? DIAG_STR_FORWARD : DIAG_STR_TRAP2CPU);

            break;
#endif
#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_HSB_CFGr,RTL9603CVD_INVALID_IP_TYPE_ACTf, &value), ret);
            diag_util_mprintf("%s\n",(0==value)? DIAG_STR_FORWARD : DIAG_STR_TRAP2CPU);

            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_hsb_invalid_ip_type */

/*
 * debug set hsb l2tp-udp-sport <UINT:sport>
 */
cparser_result_t
cparser_cmd_debug_set_hsb_l2tp_udp_sport_sport(
    cparser_context_t *context,
    uint32_t  *sport_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();
    
    value = *sport_ptr;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_CFG_L2TP_SPORTr,RTL9607C_CFG_L2TP_UDP_SPORTf, &value), ret);

            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_CFG_L2TP_SPORTr,RTL9603CVD_CFG_L2TP_UDP_SPORTf, &value), ret);

            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_l2tp_udp_sport_sport */

/*
 * debug get hsb l2tp-udp-sport
 */
cparser_result_t
cparser_cmd_debug_get_hsb_l2tp_udp_sport(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9607C_CFG_L2TP_SPORTr,RTL9607C_CFG_L2TP_UDP_SPORTf, &value), ret);
            diag_util_mprintf("UDP Source Port: %d\n", value);

            break;
#endif

#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:

            DIAG_UTIL_ERR_CHK(reg_field_read(RTL9603CVD_CFG_L2TP_SPORTr,RTL9603CVD_CFG_L2TP_UDP_SPORTf, &value), ret);
            diag_util_mprintf("UDP Source Port: %d\n", value);

            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_hsb_l2tp_udp_sport */

#endif

/*
 * debug reboot soc
 */
cparser_result_t
cparser_cmd_debug_reboot_soc(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    DIAG_UTIL_ERR_CHK(ioal_socMem32_write(0xbb00018c, 0x4), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_reboot_soc */

#ifdef CONFIG_SDK_DRIVER_UNITTEST
/*
 * debug test <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_test_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;

    rtk_acl_ingress_entry_t  aclRule;
    rtk_acl_field_t         aclField,aclField2;
    rtk_acl_template_t      aclTemplate;
    rtk_acl_field_entry_t   fieldSel;
    rtk_acl_rangeCheck_ip_t ipRange;

    rtk_acl_field_t         rawField08,rawField09,rawField10,rawField11,rawField12,rawField13;
    rtk_acl_field_t         rawField14,rawField15;
    rtk_enable_t state;
    rtk_port_t port;
    rtk_portmask_t memberPortmask, untagPortmask, portMask;
    ipaddr_t ipmc,ipmcMsk;
    rtk_switch_port_name_t portName;
    rtk_mac_t mac, macMask;

    rtk_acl_ingress_entry_t  entry;
    rtk_acl_field_t macfield;
    rtk_acl_field_entry_t field_selector;

    switch(*index_ptr)
    {
        /*add acl rule just after rtk_acl_init*/
        case 1:
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));

            aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
            aclField.fieldUnion.pattern.fieldIdx = 0x7;
            /*bit 0 TCP*/
            /*bit 1 UPD*/
            /*bit 2 ARP*/
            /* :       */
            /*bit 12 dhcp*/
            /*bit 13 dhcpv6*/
            aclField.fieldUnion.pattern.data.value = (0x1 << 2);/*care ARP*/
            aclField.fieldUnion.pattern.data.mask = (0x1 << 2);

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }
            aclRule.valid = ENABLED;
            aclRule.index = 0;/*rule index 0 */
            aclRule.templateIdx = 0x3;/*template index 3*/
            /*can be ingored*/
            /*
            aclRule.careTag.tags[ACL_CARE_TAG_IPV4].value = TRUE;
            aclRule.careTag.tags[ACL_CARE_TAG_IPV4].mask = TRUE;
            */
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

            /*enable for all ports actived*/
            aclRule.activePorts.bits[0] = 0x7F;


            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
        /*change acl template 2 to MAC_DA+MAC_SA+CTAG+STAG*/
        case 2:
            osal_memset(&aclTemplate, 0, sizeof(rtk_acl_template_t));

            aclTemplate.index = 2;
            aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
            aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
            aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
            aclTemplate.fieldType[3] = ACL_FIELD_SMAC0;
            aclTemplate.fieldType[4] = ACL_FIELD_SMAC1;
            aclTemplate.fieldType[5] = ACL_FIELD_SMAC2;
            aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
            aclTemplate.fieldType[7] = ACL_FIELD_STAG;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }
            break;

        /*add acl rule just after rtk_acl_init*/
        case 3:
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));

            aclField.fieldType = ACL_FIELD_STAG;
            aclField.fieldUnion.l2tag.vid.value = 100;
            aclField.fieldUnion.l2tag.vid.mask = 0xFFF;
            aclField.next = NULL;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            aclField2.fieldType = ACL_FIELD_ETHERTYPE;
            aclField2.fieldUnion.data.value = 0x0806;
            aclField2.fieldUnion.data.mask = 0xFFFF;
            aclField2.next = NULL;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField2))!= RT_ERR_OK)
            {
                return ret;
            }


            aclRule.valid = ENABLED;
            aclRule.index = 1;/*rule index 1*/
            aclRule.templateIdx = 0x0;/*template index 0*/
            /*care Ctag only*/
            aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask = 1;


            aclRule.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
            aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
            aclRule.act.priAct.aclPri = 7;
            /*enable for port 2 actived*/
            aclRule.activePorts.bits[0] = 0x4;


            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 4:

            fieldSel.format = ACL_FORMAT_IPPAYLOAD;
            fieldSel.index = 13;
            fieldSel.offset = 0x0;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            aclTemplate.index = 3;
            aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
            aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
            aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
            aclTemplate.fieldType[3] = ACL_FIELD_SMAC0;
            aclTemplate.fieldType[4] = ACL_FIELD_SMAC1;
            aclTemplate.fieldType[5] = ACL_FIELD_SMAC2;
            aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
            aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED13;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));

            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            aclField.fieldType = ACL_FIELD_USER_DEFINED13;
            aclField.fieldUnion.data.value = 0x1234;/*l4 port*/
            aclField.fieldUnion.data.mask = 0xFFFF;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }


            aclRule.valid = ENABLED;
            aclRule.index = 2;
            aclRule.templateIdx = 0x3;

            /*care tcp only*/
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].mask = 1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            rtk_switch_portMask_Clear(&aclRule.act.forwardAct.portMask);

            rtk_switch_allPortMask_set(&aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 5:

            fieldSel.format = ACL_FORMAT_IPPAYLOAD;
            fieldSel.index = 13;
            fieldSel.offset = 0x0;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            aclTemplate.index = 3;
            aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
            aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
            aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
            aclTemplate.fieldType[3] = ACL_FIELD_SMAC0;
            aclTemplate.fieldType[4] = ACL_FIELD_SMAC1;
            aclTemplate.fieldType[5] = ACL_FIELD_SMAC2;
            aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
            aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED13;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));

            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));

            aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
            aclField.fieldUnion.pattern.data.value = 0x1234;
            aclField.fieldUnion.pattern.data.mask = 0xFFFF;
            aclField.fieldUnion.pattern.fieldIdx = 7;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }


            aclRule.valid = ENABLED;
            aclRule.index = 2;
            aclRule.templateIdx = 0x3;

            /*care tcp only*/
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].mask = 1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            rtk_switch_portMask_Clear(&aclRule.act.forwardAct.portMask);

            rtk_switch_allPortMask_set(&aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 6:

            osal_memset(&ipRange, 0, sizeof(rtk_acl_rangeCheck_ip_t));
           
            ipRange.index = 2;
            ipRange.type = IPRANGE_IPV4_SIP;
            ipRange.lowerIp = 0x01020304;
            ipRange.upperIp = 0x05060708;

            if((ret = rtk_acl_ipRange_set(&ipRange))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));

            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));

            aclField.fieldType = ACL_FIELD_IP_RANGE;
            aclField.fieldUnion.data.value = 0x0004;
            aclField.fieldUnion.data.mask = 0x0004;


            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            aclRule.valid = ENABLED;
            aclRule.index = 3;
            aclRule.templateIdx = 0x1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            rtk_switch_portMask_Clear(&aclRule.act.forwardAct.portMask);

            rtk_switch_allPortMask_set(&aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 7:

            if ((ret = rtk_acl_init()) != RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_allPortMask_set(&portMask);
            port = RTK_SWITCH_GET_FIRST_PORT;
            while(rtk_switch_nextPortInMask_get(&portMask, &port) == RT_ERR_OK)
            {
                if ((ret = rtk_acl_igrState_set(port, ENABLED)) != RT_ERR_OK)
                {
                    return ret;
                }
            }

            aclTemplate.index = 3;
            aclTemplate.fieldType[0] = ACL_FIELD_IPV6_SIP0;
            aclTemplate.fieldType[1] = ACL_FIELD_IPV6_SIP1;
            aclTemplate.fieldType[2] = ACL_FIELD_USER_DEFINED08;
            aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED09;
            aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED10;
            aclTemplate.fieldType[5] = ACL_FIELD_USER_DEFINED11;
            aclTemplate.fieldType[6] = ACL_FIELD_USER_DEFINED12;
            aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED13;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 8;
            fieldSel.offset = 18;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 9;
            fieldSel.offset = 16;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 10;
            fieldSel.offset = 14;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 11;
            fieldSel.offset = 12;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 12;
            fieldSel.offset = 10;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 13;
            fieldSel.offset = 8;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }


            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));

            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            aclField.fieldType = ACL_FIELD_IPV6_SIP0;
            aclField.fieldUnion.ip6.value.ipv6_addr[14]= 0x00;
            aclField.fieldUnion.ip6.value.ipv6_addr[15]= 0x00;
            aclField.fieldUnion.ip6.mask.ipv6_addr[14]= 0xFF;
            aclField.fieldUnion.ip6.mask.ipv6_addr[15]= 0xFF;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
            aclField2.fieldType = ACL_FIELD_IPV6_SIP1;
            aclField2.fieldUnion.ip6.value.ipv6_addr[12]= 0x11;
            aclField2.fieldUnion.ip6.value.ipv6_addr[13]= 0x11;
            aclField2.fieldUnion.ip6.mask.ipv6_addr[12]= 0xFF;
            aclField2.fieldUnion.ip6.mask.ipv6_addr[13]= 0xFF;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField2))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField08, 0, sizeof(rtk_acl_field_t));
            rawField08.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField08.fieldUnion.pattern.data.value = 0x2222;
            rawField08.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField08.fieldUnion.pattern.fieldIdx = 2;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField08))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField09, 0, sizeof(rtk_acl_field_t));
            rawField09.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField09.fieldUnion.pattern.data.value = 0x3333;
            rawField09.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField09.fieldUnion.pattern.fieldIdx = 3;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField09))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField10, 0, sizeof(rtk_acl_field_t));
            rawField10.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField10.fieldUnion.pattern.data.value = 0x4444;
            rawField10.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField10.fieldUnion.pattern.fieldIdx = 4;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField10))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField11, 0, sizeof(rtk_acl_field_t));
            rawField11.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField11.fieldUnion.pattern.data.value = 0x5555;
            rawField11.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField11.fieldUnion.pattern.fieldIdx = 5;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField11))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField12, 0, sizeof(rtk_acl_field_t));
            rawField12.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField12.fieldUnion.pattern.data.value = 0x6666;
            rawField12.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField12.fieldUnion.pattern.fieldIdx = 6;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField12))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField13, 0, sizeof(rtk_acl_field_t));
            rawField13.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField13.fieldUnion.pattern.data.value = 0x7777;
            rawField13.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField13.fieldUnion.pattern.fieldIdx = 7;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField13))!= RT_ERR_OK)
            {
                return ret;
            }

            aclRule.valid = ENABLED;
            aclRule.index = 4;
            aclRule.templateIdx = 0x3;

            /*care IPv6 only*/
            aclRule.careTag.tags[ACL_CARE_TAG_IPV6].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_IPV6].mask = 1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            rtk_switch_portMask_Clear(&aclRule.act.forwardAct.portMask);

            rtk_switch_allPortMask_set(&aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
        /*init svlan with SVID 1000 default vlan in all ports and servce-port in PON port*/
        case 8:
            if((ret = rtk_svlan_init())!= RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_phyPortId_get(RTK_PORT_PON, &port);

            if((ret = rtk_svlan_servicePort_set(port, ENABLED))!= RT_ERR_OK)
            {
               return ret;
            }

            if((ret = rtk_svlan_create(1000))!= RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_allPortMask_set(&memberPortmask);
            rtk_switch_allPortMask_set(&untagPortmask);
            rtk_switch_port2PortMask_clear(&untagPortmask, RTK_PORT_PON);

            if((ret = rtk_svlan_memberPort_set(1000, &memberPortmask, &untagPortmask))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
        /*create SVID 2000 and tranparent SVLAN in PON and UTP0*/
        case 9:
            rtk_switch_phyPortId_get(RTK_PORT_PON, &port);

            if((ret = rtk_svlan_servicePort_set(port, ENABLED))!= RT_ERR_OK)
            {
               return ret;
            }

            if((ret = rtk_svlan_create(2000))!= RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_allPortMask_set(&memberPortmask);
            rtk_switch_allPortMask_set(&untagPortmask);
            rtk_switch_port2PortMask_clear(&untagPortmask, RTK_PORT_PON);
            rtk_switch_port2PortMask_clear(&untagPortmask, RTK_PORT_UTP0);

            if((ret = rtk_svlan_memberPort_set(2000, &memberPortmask, &untagPortmask))!= RT_ERR_OK)
            {
                return ret;
            }

            if((ret = rtk_svlan_untagAction_set(SVLAN_ACTION_DROP, 0))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
        /*239.1.1.1/16 to SVID 1000, CVID 200 from UTP0/1 to SVID 2000. ALL UTP default vlan in all ports and servce-port in PON port*/
        case 10:
            rtk_switch_phyPortId_get(RTK_PORT_PON, &port);

            if((ret = rtk_svlan_servicePort_set(port, ENABLED))!= RT_ERR_OK)
            {
               return ret;
            }

            if((ret = rtk_svlan_create(1000))!= RT_ERR_OK)
            {
                return ret;
            }

            if((ret = rtk_svlan_create(2000))!= RT_ERR_OK)
            {
                return ret;
            }

            if((ret = rtk_svlan_create(3000))!= RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_allPortMask_set(&memberPortmask);
            rtk_switch_allPortMask_set(&untagPortmask);
            rtk_switch_port2PortMask_clear(&untagPortmask, RTK_PORT_PON);

            if((ret = rtk_svlan_memberPort_set(1000, &memberPortmask, &untagPortmask))!= RT_ERR_OK)
            {
                return ret;
            }

            if((ret = rtk_svlan_memberPort_set(2000, &memberPortmask, &untagPortmask))!= RT_ERR_OK)
            {
                return ret;
            }

            if((ret = rtk_svlan_memberPort_set(3000, &memberPortmask, &untagPortmask))!= RT_ERR_OK)
            {
                return ret;
            }

            /*IPv4 multicast address*/
            ipmc = 0xFE010101;/*239.1.1.1*/
            ipmcMsk = 0xFFFFFFF0;/*255.255.255.240*/

            if((ret = rtk_svlan_ipmc2s_add(ipmc, ipmcMsk, 1000))!= RT_ERR_OK)
            {
                return ret;
            }

            mac.octet[2] = 0x01;
            mac.octet[3] = 0x02;
            mac.octet[4] = 0x03;
            mac.octet[5] = 0x01;

            macMask.octet[2] = 0xFF;
            macMask.octet[3] = 0xFF;
            macMask.octet[4] = 0xFF;
            macMask.octet[5] = 0x00;

            if((ret = rtk_svlan_l2mc2s_add(mac, macMask, 1000))!= RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_phyPortId_get(RTK_PORT_UTP0, &port);

            if((ret = rtk_svlan_c2s_add(200, port, 2000))!= RT_ERR_OK)
            {
                return ret;
            }

            rtk_switch_phyPortId_get(RTK_PORT_UTP1, &port);

            if((ret = rtk_svlan_c2s_add(200, port, 2000))!= RT_ERR_OK)
            {
                return ret;
            }

            for(portName = RTK_PORT_UTP0; portName <= RTK_PORT_UTP3; portName ++)
            {
                rtk_switch_phyPortId_get(portName, &port);
                if((ret = rtk_svlan_portSvid_set(port, 3000))!= RT_ERR_OK)
                {
                    return ret;
                }
            }

            break;
        case 11:
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&rawField15, 0, sizeof(rtk_acl_field_t));
            rawField15.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField15.fieldUnion.pattern.data.value = 0x2000;
            rawField15.fieldUnion.pattern.data.mask = 0x2000;
            rawField15.fieldUnion.pattern.fieldIdx = 7;/*template 3 index 7 = field select 15*/

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField15))!= RT_ERR_OK)
            {
                return ret;
            }

            aclRule.valid = ENABLED;
            aclRule.index = 0;/*rule index 0*/
            aclRule.templateIdx = 0x3;/*template index 3*/
            /*trap to cpu*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

            /*enable for all ports actived*/
            aclRule.activePorts.bits[0] = 0x7F;

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
        case 12:
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));

            aclField.fieldType = ACL_FIELD_DMAC;
            aclField.fieldUnion.mac.value.octet[0] = 0x00;
            aclField.fieldUnion.mac.value.octet[1] = 0x01;
            aclField.fieldUnion.mac.value.octet[2] = 0x02;
            aclField.fieldUnion.mac.value.octet[3] = 0x03;
            aclField.fieldUnion.mac.value.octet[4] = 0x04;
            aclField.fieldUnion.mac.value.octet[5] = 0x05;
            aclField.fieldUnion.mac.mask.octet[0] = 0xFF;
            aclField.fieldUnion.mac.mask.octet[1] = 0xFF;
            aclField.fieldUnion.mac.mask.octet[2] = 0xFF;
            aclField.fieldUnion.mac.mask.octet[3] = 0xFF;
            aclField.fieldUnion.mac.mask.octet[4] = 0xFF;
            aclField.fieldUnion.mac.mask.octet[5] = 0xFF;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            aclField2.fieldType = ACL_FIELD_SMAC;
            aclField2.fieldUnion.mac.value.octet[0] = 0x00;
            aclField2.fieldUnion.mac.value.octet[1] = 0x11;
            aclField2.fieldUnion.mac.value.octet[2] = 0x22;
            aclField2.fieldUnion.mac.value.octet[3] = 0x33;
            aclField2.fieldUnion.mac.value.octet[4] = 0x44;
            aclField2.fieldUnion.mac.value.octet[5] = 0x55;
            aclField2.fieldUnion.mac.mask.octet[0] = 0xFF;
            aclField2.fieldUnion.mac.mask.octet[1] = 0xFF;
            aclField2.fieldUnion.mac.mask.octet[2] = 0xFF;
            aclField2.fieldUnion.mac.mask.octet[3] = 0xFF;
            aclField2.fieldUnion.mac.mask.octet[4] = 0xFF;
            aclField2.fieldUnion.mac.mask.octet[5] = 0xFF;

            if((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField2))!= RT_ERR_OK)
            {
                return ret;
            }


            aclRule.valid = ENABLED;
            aclRule.index = 1;/*rule index 0*/
            aclRule.templateIdx = 0;/*template index 0*/
            /*trap to cpu*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

            /*enable for all ports actived*/
            aclRule.activePorts.bits[0] = 0x7F;

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 13:
            osal_memset(&entry, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&macfield, 0, sizeof(rtk_acl_field_t));

            entry.templateIdx = 0;

            entry.valid = ENABLED;

            entry.invert = ACL_INVERT_DISABLE;

            entry.activePorts.bits[0] = 0x3f;

            macfield.fieldType = ACL_FIELD_DMAC;
            macfield.fieldUnion.mac.value.octet[0] = 0x00;
            macfield.fieldUnion.mac.value.octet[1] = 0x11;
            macfield.fieldUnion.mac.value.octet[2] = 0x22;
            macfield.fieldUnion.mac.value.octet[3] = 0x33;
            macfield.fieldUnion.mac.value.octet[4] = 0x44;
            macfield.fieldUnion.mac.value.octet[5] = 0x55;
            macfield.fieldUnion.mac.mask.octet[0] = 0xFF;
            macfield.fieldUnion.mac.mask.octet[1] = 0xFF;
            macfield.fieldUnion.mac.mask.octet[2] = 0xFF;
            macfield.fieldUnion.mac.mask.octet[3] = 0xFF;
            macfield.fieldUnion.mac.mask.octet[4] = 0xFF;
            macfield.fieldUnion.mac.mask.octet[5] = 0xFF;
            if((ret = rtk_acl_igrRuleField_add(&entry, &macfield))!= RT_ERR_OK)
            {
                return ret;
            }


            entry.act.enableAct[ACL_IGR_FORWARD_ACT]= ENABLED;

            entry.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

            entry.act.forwardAct.portMask.bits[0]=0x40;

            ret = rtk_acl_igrRuleEntry_add(&entry);

            if(ret)

            {

                   osal_printf("!!Error: cfg cpu mac entry Failed, ret %d\r\n",ret);

            }

            break;
        case 14:

            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));

            osal_memset(&field_selector,0,sizeof(rtk_acl_field_entry_t));
            field_selector.format=ACL_FORMAT_RAW;
            field_selector.index = 8;
            field_selector.offset = 16;
            rtk_acl_fieldSelect_set(&field_selector);

            osal_memset(&field_selector,0,sizeof(rtk_acl_field_entry_t));
            field_selector.format=ACL_FORMAT_RAW;
            field_selector.index = 9;
            field_selector.offset = 20;
            rtk_acl_fieldSelect_set(&field_selector);

            osal_memset(&field_selector,0,sizeof(rtk_acl_field_entry_t));
            field_selector.format=ACL_FORMAT_RAW;
            field_selector.index = 10;
            field_selector.offset = 24;
            rtk_acl_fieldSelect_set(&field_selector);

            osal_memset(&field_selector,0,sizeof(rtk_acl_field_entry_t));
            field_selector.format=ACL_FORMAT_RAW;
            field_selector.index = 11;
            field_selector.offset = 35;
            rtk_acl_fieldSelect_set(&field_selector);

            osal_memset(&field_selector,0,sizeof(rtk_acl_field_entry_t));
            field_selector.format=ACL_FORMAT_RAW;
            field_selector.index = 12;
            field_selector.offset = 31;
            rtk_acl_fieldSelect_set(&field_selector);



            osal_memset(&rawField08, 0, sizeof(rtk_acl_field_t));

            rawField08.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField08.fieldUnion.pattern.data.value = 0x8100;
            rawField08.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField08.fieldUnion.pattern.fieldIdx = 0;
            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField08))!= RT_ERR_OK)
            {
                return ret;
            }
#if 1
            osal_memset(&rawField09, 0, sizeof(rtk_acl_field_t));
            rawField09.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField09.fieldUnion.pattern.data.value = 0x0800;
            rawField09.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField09.fieldUnion.pattern.fieldIdx = 1;
            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField09))!= RT_ERR_OK)
            {
                return ret;
            }
            osal_memset(&rawField12, 0, sizeof(rtk_acl_field_t));
            rawField12.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField12.fieldUnion.pattern.data.value = 0x0200;
            rawField12.fieldUnion.pattern.data.mask = 0xFF00;
            rawField12.fieldUnion.pattern.fieldIdx = 4;
            if((ret = rtk_acl_igrRuleField_add(&aclRule, &rawField12))!= RT_ERR_OK)
            {
                return ret;
            }
#endif
            //apollo_acl_index=5;
            aclRule.valid = ENABLED;
            aclRule.index = 5;/*rule index 0*/
            aclRule.templateIdx = 0x3;/*template index 3*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask = 1;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
            aclRule.activePorts.bits[0] = 0x3f;
            if((rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;


    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_test_index */
#endif

/*
 * debug set ind-phy <UINT:phy_id> <UINT:reg_address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_ind_phy_phy_id_reg_address_value(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr,
    uint32_t  *value_ptr)
{
    uint16 data;

    DIAG_UTIL_PARAM_CHK();

    /* This command is used to access the external sub-boards */
    switch(DIAG_UTIL_CHIP_TYPE)
    {
    #ifdef CONFIG_SDK_RTL9601B
    case RTL9601B_CHIP_ID:
        #if defined(FPGA_DEFINED)
        /* RTL9601B uses internal MII to access external PHYs */
        _diag_debug_internal_phy_write((uint8_t)*phy_id_ptr, 0, (uint8_t)*reg_address_ptr,(uint16_t)*value_ptr);
        #else
        /* ASIC uses OCP to access internal PHYs */
        _diag_debug_internal_phy_write((uint8_t)*phy_id_ptr, 0, (uint16_t)*reg_address_ptr,(uint16_t)*value_ptr);
        #endif
        break;
    #endif
    default:
        data = *value_ptr;
        (void)io_mii_phy_reg_write(8, 1, data);

        data = (PATCH_W_OFFSET | ((*phy_id_ptr & 0x1F) << 5) |(*reg_address_ptr & 0x1F));
        (void)io_mii_phy_reg_write(8, 0, data);
        break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_ind_phy_phy_id_reg_address_value */

/*
 * debug get ind-phy <UINT:phy_id> <UINT:reg_address>
 */
cparser_result_t
cparser_cmd_debug_get_ind_phy_phy_id_reg_address(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr)
{
    uint16 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /* This command is used to access the external sub-boards */
    switch(DIAG_UTIL_CHIP_TYPE)
    {
    #ifdef CONFIG_SDK_RTL9601B
    case RTL9601B_CHIP_ID:
        #if defined(FPGA_DEFINED)
        /* FPGA uses internal MII to access external PHYs */
        _diag_debug_internal_phy_read((uint8_t)*phy_id_ptr, 0, (uint8_t)*reg_address_ptr,&data);
        diag_util_printf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr, data);
        #else
        {
            uint16 page = ((uint16_t)*reg_address_ptr) >> 4;
            /* ASIC uses OCP to access internal PHYs */
            _diag_debug_internal_phy_read((uint8_t)*phy_id_ptr, 0, (uint16_t)*reg_address_ptr,&data);
            if((page == 0xa40) || (page == 0xa41) || (page == 0xa42) || (page == 0xa43))
            {
                diag_util_printf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,(*reg_address_ptr & 0x3f) >> 1, data);

            }
            else
            {
                diag_util_printf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,((*reg_address_ptr & 0xf) >> 1) | 0x10, data);

            }
        }
        #endif
        break;
    #endif
    default:
        data = (PATCH_R_OFFSET | ((*phy_id_ptr & 0x1F) << 5) |(*reg_address_ptr & 0x1F));
        (void)io_mii_phy_reg_write(8, 0, data);

        (void)io_mii_phy_reg_read(8, 2, &data);
        diag_util_printf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr, data);
        break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_ind_phy_phy_id_reg_address */

/*
 * debug set ind-serdes <UINT:idx> <UINT:page> <UINT:reg_addr> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_ind_serdes_idx_page_reg_addr_value(
    cparser_context_t *context,
    uint32_t  *idx_ptr,
    uint32_t  *page_ptr,
    uint32_t  *reg_addr_ptr,
    uint32_t  *value_ptr)
{
    DIAG_UTIL_PARAM_CHK();

#if defined (FPGA_DEFINED)
    diag_util_mprintf("command not support in this mode!\n");
#else
    #ifdef CONFIG_SDK_RTL9601B
    rtl9601b_sds_acc_t sds_acc;
    #endif

    switch(DIAG_UTIL_CHIP_TYPE)
    {
    #ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            memset(&sds_acc, 0, sizeof(sds_acc));
            sds_acc.index = *idx_ptr;
            sds_acc.page = *page_ptr;
            sds_acc.regaddr = *reg_addr_ptr;

            /* RTL9601B uses indirect access to access serdes */
            _rtl9601b_serdes_ind_write(sds_acc, *value_ptr);
            break;
    #endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_ind_serdes_idx_page_reg_addr_value */

/*
 * debug get ind-serdes <UINT:idx> <UINT:page> <UINT:reg_addr>
 */
cparser_result_t
cparser_cmd_debug_get_ind_serdes_idx_page_reg_addr(
    cparser_context_t *context,
    uint32_t  *idx_ptr,
    uint32_t  *page_ptr,
    uint32_t  *reg_addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

#if defined (FPGA_DEFINED)
    diag_util_mprintf("command not support in this mode!\n");
#else
    #ifdef CONFIG_SDK_RTL9601B
    rtl9601b_sds_acc_t sds_acc;
    uint16 value;
    #endif

    switch(DIAG_UTIL_CHIP_TYPE)
    {
    #ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            memset(&sds_acc, 0, sizeof(sds_acc));
            sds_acc.index = *idx_ptr;
            sds_acc.page = *page_ptr;
            sds_acc.regaddr = *reg_addr_ptr;

            /* RTL9601B uses indirect access to access serdes */
            _rtl9601b_serdes_ind_read(sds_acc, &value);
            diag_util_mprintf("Index: %u Page: %u Reg: %u Value: 0x%08x\n",
                sds_acc.index, sds_acc.page, sds_acc.regaddr, value);
            break;
    #endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_ind_serdes_idx_page_reg_addr */

/*
 * debug io dump
 */
cparser_result_t
cparser_cmd_debug_io_dump(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            diag_util_printf("\n[GPIO]\n");

            DIAG_UTIL_ERR_CHK(ioal_mem32_read(0x0000F0, &value), ret);
            diag_util_printf(" 0~31[0xBB0000F0]:%8.8x\n",value);
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(0x0000F4, &value), ret);
            diag_util_printf("32~63[0xBB0000F4]:%8.8x\n",value);
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(0x0000F8, &value), ret);
            diag_util_printf("64~71[0xBB0000F8]:%8.8x\n",value);

            diag_util_printf("\n[LED]\n");
            reg_read(APOLLOMP_IO_LED_ENr,&value);
            diag_util_printf("IO_LED_EN[0xBB023014]:%8.8x\n",value);
            diag_util_printf("  Parallel LED(0~16):%8.8x\n",value&0x1FFFF);
            diag_util_printf("  Serial LED:");
            if(value&0x20000)
                diag_util_printf("enable\n");
            else
                diag_util_printf("disable\n");

            diag_util_printf("\n[IO_MODE_EN]\n");
            reg_read(APOLLOMP_IO_MODE_ENr,&value);
            diag_util_printf("IO_MODE_EN[0xBB023018]:%8.8x\n",value);
            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_EXT_ENf,&value);
            diag_util_printf("  [UART_EXT_EN RTL9601_UART1]:%d\n",value);
            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SATA_ENf,&value);
            diag_util_printf("  [SATA_EN]:%d\n",value);
            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_DECT_ENf,&value);
            diag_util_printf("  [UART_DECT_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_PTP_IO_ENf,&value);
            diag_util_printf("  [PTP_IO_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_NFBI_ENf,&value);
            diag_util_printf("  [NFBI_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_EXT_INTRPT_ENf,&value);
            diag_util_printf("  [EXT_INTRPT_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_OEM_ENf,&value);
            diag_util_printf("  [OEM_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
            diag_util_printf("  [SLIC_PCM_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
            diag_util_printf("  [SLIC_SPI_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
            diag_util_printf("  [SLIC_ZSI_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
            diag_util_printf("  [SLIC_ISI_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_I2C_ENf,&value);
            diag_util_printf("  [I2C_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_INTRPT_ENf,&value);
            diag_util_printf("  [INTRPT_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_MDX_M_ENf,&value);
            diag_util_printf("  [MDX_M_EN]:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SATA_MDC_ENf,&value);
            diag_util_printf("  [SATA_MDC_EN:%d\n",value);

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&value);
            diag_util_printf("  [UART_EN:%d\n",value);


            reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
            diag_util_printf("\n[RGMII]:%d\n",value);

            switch(value)
            {
                case 1:
                    diag_util_printf("RGMII mode (share gpio 6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 2:
                case 4:
                    diag_util_printf("(T)MII MAC mode (share gpio 0/6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 3:
                case 5:
                    diag_util_printf("(T)MII PHY mode (share gpio 6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 7:
                    diag_util_printf("RMII MAC mode (share gpio 8/9/10/11/16/17/18)\n");
                    break;
                case 8:
                    diag_util_printf("RMII PHY mode (share gpio 8/9/10/11/16/17/18)\n");
                    break;
                default:
                    diag_util_printf("disabled\n");
                    break;

            }
            break;
    #endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_dump */

/*
 * debug io gpio-check <UINT:gpioId>
 */
cparser_result_t
cparser_cmd_debug_io_gpio_check_gpioId(
    cparser_context_t *context,
    uint32_t  *gpioId_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            int32   ret = RT_ERR_FAILED;
            uint32  value;

            reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, *gpioId_ptr, APOLLOMP_EN_GPIOf, (uint32 *)&value);
            diag_util_printf("gpio %d:",*gpioId_ptr);
            if(1==value)
            {
                diag_util_printf("enabled\n");
                switch(*gpioId_ptr)
                {
                    case 0:
                        /*share with TMII MAC mode*/
                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(4==value)
                        {
                            diag_util_printf("  warning~ TMII mac mode share gpio 0!!\n");
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_EXT_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  warning~ RTL9601 uart1 share gpio 0!!\n");
                        }

                        break;
                    case 6:
                    case 7:
                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(1==value || 4==value || 5==value)
                        {
                            diag_util_printf("  warning~ MII mode share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 8:
                    case 9:
                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(1==value || 2==value || 3==value || 4==value || 5==value || 7==value || 8==value)
                        {
                            diag_util_printf("  warning~ MII mode share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 10:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_EXT_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  warning~ RTL9601 uart1 share gpio 10!!\n");
                        }
                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(1==value || 2==value || 3==value || 4==value || 5==value || 7==value || 8==value)
                        {
                            diag_util_printf("  warning~ MII mode share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 11:
                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(1==value || 2==value || 3==value || 4==value || 5==value || 7==value || 8==value)
                        {
                            diag_util_printf("  warning~ MII mode share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 12:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED9_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 9 share gpio 12!!\n");
                        }
                        break;
                    case 13:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED8_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 8 share gpio 13!!\n");
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&value);
                        if(2&value)
                        {
                            diag_util_printf("  uart1 share gpio %d!!\n",*gpioId_ptr);
                        }

                        break;
                    case 15:
                    case 19:
                    case 20:

                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(1==value || 4==value || 5==value)
                        {
                            diag_util_printf("  warning~ MII mode share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 16:
                    case 17:
                    case 18:
                        reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
                        if(1==value || 2==value || 3==value || 4==value || 5==value || 7==value || 8==value)
                        {
                            diag_util_printf("  warning~ MII mode share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 21:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED15_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 15 share gpio 21!!\n");
                        }
                        break;
                    case 22:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED16_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 16 share gpio 22!!\n");
                        }
                        break;
                    case 23:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED7_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 7 share gpio 23!!\n");
                        }
                        break;
                    case 24:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED6_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 6 share gpio 24!!\n");
                        }
                        if(2&value)
                        {
                            diag_util_printf("  uart1 share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 25:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ISI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 26:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 27:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 28:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ISI share gpio %d!!\n",*gpioId_ptr);
                        }

                        break;
                    case 29:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ISI share gpio %d!!\n",*gpioId_ptr);
                        }

                        break;
                    case 30:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 31:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    case 32:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ISI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 33:
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_PCM share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_SPI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ZSI share gpio %d!!\n",*gpioId_ptr);
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  SLIC_ISI share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;


                    case 45:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED5_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 5 share gpio 45!!\n");
                        }
                        break;

                    case 46:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED4_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 4 share gpio 46!!\n");
                        }
                        break;
                    case 47:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED3_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 3 share gpio 47!!\n");
                        }
                        break;

                    case 48:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED2_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 2 share gpio 48!!\n");
                        }
                        break;
                    case 49:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED1_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 1 share gpio 49!!\n");
                        }

                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&value);
                        if(4&value)
                        {
                            diag_util_printf("  uart0 share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 52:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED0_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  Serlal LED gpio 52!!\n");
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&value);
                        if(4&value)
                        {
                            diag_util_printf("  uart0 share gpio %d!!\n",*gpioId_ptr);
                        }

                        break;
                    case 58:
                        /*share with serial led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_SERI_LED_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  Serlal LED share gpio 58/59!!\n");
                        }
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED13_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 13 share gpio 58!!\n");
                        }
                        break;

                    case 59:
                        /*share with serial led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_SERI_LED_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  Serlal LED share gpio 58/59!!\n");
                        }

                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED12_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 12 share gpio 59!!\n");
                        }
                        break;

                    case 60:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED11_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 11 share gpio 60!!\n");
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_MDX_M_ENf,&value);
                        if(2==value)
                        {
                            diag_util_printf("  mid/mdio share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;

                    case 61:
                        /*share with parallel led mode*/
                        reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED10_ENf,&value);
                        if(1==value)
                        {
                            diag_util_printf("  parallel LED 10 share gpio 61!!\n");
                        }
                        reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_MDX_M_ENf,&value);
                        if(2==value)
                        {
                            diag_util_printf("  mid/mdio share gpio %d!!\n",*gpioId_ptr);
                        }
                        break;
                    default:
                        break;

                }

            }
            else
            {
                diag_util_printf("disabled\n");
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_gpio_check_gpioid */

/*
 * debug io uart-check
 */
cparser_result_t
cparser_cmd_debug_io_uart_check(
    cparser_context_t *context)
{


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            int32   ret = RT_ERR_FAILED;
            uint32  value,check;

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&value);
            if(1&value)
            {
                diag_util_printf("uart 0 enable\n\n");
                /*gpio 49/52*/
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 49, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                if(1==check)
                    diag_util_printf(" warning!! gpio 49 enabled! \n");
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 52, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                if(1==check)
                    diag_util_printf(" warning!! gpio 52 enabled! \n");

                /*LED 0/1*/
                reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED0_ENf,&check);
                if(1==check)
                    diag_util_printf(" warning!! LED 0 enabled! \n");
                reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED1_ENf,&check);
                if(1==check)
                    diag_util_printf(" warning!! LED 1 enabled! \n");
            }
            if(2&value)
            {
                diag_util_printf("\nuart 1 enable\n\n");

                /*gpio 13/24*/
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 13, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                if(1==check)
                    diag_util_printf(" warning!! gpio 13 enabled! \n");
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 24, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                if(1==check)
                    diag_util_printf(" warning!! gpio 24 enabled! \n");

                /*LED 8/6*/
                reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED8_ENf,&check);
                if(1==check)
                    diag_util_printf(" warning!! LED 8 enabled! \n");
                reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED6_ENf,&check);
                if(1==check)
                    diag_util_printf(" warning!! LED 6 enabled! \n");
            }
            if(4&value)
            {
                diag_util_printf("warning IO_MODE_EN uart setting error\n");
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_uart_check */

/*
 * debug io mii-check
 */
cparser_result_t
cparser_cmd_debug_io_mii_check(
    cparser_context_t *context)
{

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            int32   ret = RT_ERR_FAILED;
            uint32  value,check;

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_MDX_M_ENf,&value);
            if(2==value)
            {
                diag_util_printf("mid/mdio enable!!\n");
                /*check GPIO 60/61*/
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 60, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1==value)
                    diag_util_printf(" warning gpio 60 enabled\n");
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 61, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1==value)
                    diag_util_printf(" warning gpio 61 enabled\n");

            }


            reg_field_read(APOLLOMP_MODE_EXTr,APOLLOMP_MODE_EXTf,&value);
            diag_util_printf("\n[RGMII] mode:%d\n",value);
            switch(value)
            {
                case 1:
                    diag_util_printf("RGMII mode (share gpio 6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 2:
                    diag_util_printf("MII MAC mode (share gpio 0/6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 4:
                    diag_util_printf("TMII MAC mode (share gpio 0/6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 3:
                    diag_util_printf("TMII PHY mode (share gpio 6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 5:
                    diag_util_printf("TMII PHY mode (share gpio 6/7/8/9/10/11/15/16/17/18/19/20)\n");
                    break;
                case 7:
                    diag_util_printf("RMII MAC mode (share gpio 8/9/10/11/16/17/18)\n");
                    break;
                case 8:
                    diag_util_printf("RMII PHY mode (share gpio 8/9/10/11/16/17/18)\n");
                    break;
                default:
                    diag_util_printf("not enable\n");
                    return CPARSER_OK;
                    break;

            }
            if(value == 2 || value == 4)
            {
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 0, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                if(1==check)
                    diag_util_printf(" warning gpio 0 enabled\n");
            }

            if(value == 2 || value == 4)
            {
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 0, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                if(1==check)
                    diag_util_printf(" warning gpio 0 enabled\n");
            }
            switch(value)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 7:
                case 8:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 8, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 8 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 9, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 9 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 10, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 10 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 11, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 11 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 16, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 16 enabled\n");
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 17, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 17 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 18, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 18 enabled\n");

                    break;
            }


            switch(value)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 15, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 15 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 19, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 19 enabled\n");

                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 20, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning gpio 20 enabled\n");
                    break;
                default:
                    break;
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_mii_check */

/*
 * debug io led-check serial
 */
cparser_result_t
cparser_cmd_debug_io_led_check_serial(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            int32   ret = RT_ERR_FAILED;
            uint32  value,check;

            reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_SERI_LED_ENf,&value);
            diag_util_printf("Serial led:");

            if(1==value)
            {
                diag_util_printf("enable\n");
            }
            else
            {
                diag_util_printf("disable\n");
                return CPARSER_OK;
            }

            reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 58, APOLLOMP_EN_GPIOf, (uint32 *)&check);
            if(1==check)
                diag_util_printf(" warning gpio 58 enabled\n");

            reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 59, APOLLOMP_EN_GPIOf, (uint32 *)&check);
            if(1==check)
                diag_util_printf(" warning gpio 59 enabled\n");
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_led_check_serial */

/*
 * debug io led-check parallel <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_io_led_check_parallel_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            int32   ret = RT_ERR_FAILED;
            uint32  value,check,ledIdx;;

            ledIdx = *index_ptr;

            switch(ledIdx)
            {
                case 0:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED0_ENf,&value);
                    break;
                case 1:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED1_ENf,&value);
                    break;
               case 2:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED2_ENf,&value);
                    break;
               case 3:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED3_ENf,&value);
                    break;
               case 4:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED4_ENf,&value);
                    break;
               case 5:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED5_ENf,&value);
                    break;
               case 6:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED6_ENf,&value);
                    break;
               case 7:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED7_ENf,&value);
                    break;
               case 8:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED8_ENf,&value);
                    break;
               case 9:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED9_ENf,&value);
                    break;
               case 10:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED10_ENf,&value);
                    break;

               case 11:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED11_ENf,&value);
                    break;

               case 12:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED12_ENf,&value);
                    break;

               case 13:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED13_ENf,&value);
                    break;

               case 14:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED14_ENf,&value);
                    break;

               case 15:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED15_ENf,&value);
                    break;
               case 16:
                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_LED16_ENf,&value);
                    break;
                default:
                    break;
            }

            diag_util_printf("led %d state:",ledIdx);

            if(1==value)
            {
                diag_util_printf("enable\n");
            }
            else
            {
                diag_util_printf("disable\n");
                return CPARSER_OK;
            }

            switch(ledIdx)
            {
                case 0:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 52, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 52 enabled! \n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&check);
                    if(1&check)
                        diag_util_printf(" warning!! uart0 enabled! \n");
                    break;

                case 1:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 49, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 49 enabled! \n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&check);
                    if(1&check)
                        diag_util_printf(" warning!! uart0 enabled! \n");
                    break;

               case 2:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 48, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 48 enabled! \n");
                    break;

               case 3:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 47, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 47 enabled! \n");
                    break;

               case 4:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 46, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 46 enabled! \n");
                    break;

               case 5:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 45, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 45 enabled! \n");
                    break;

               case 6:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 24, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 24 enabled! \n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&check);
                    if(2&check)
                    {
                        diag_util_printf(" warning uart 1 enable\n");
                    }
                    break;

               case 7:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 23, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 23 enabled! \n");
                    break;

               case 8:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 13, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 23 enabled! \n");
                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_UART_ENf,&check);
                    if(2&check)
                    {
                        diag_util_printf(" warning uart 1 enable\n");
                    }

                    break;
               case 9:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 12, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 12 enabled! \n");
                    break;
               case 10:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 61, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 61 enabled! \n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_I2C_ENf,&check);
                    if(2&check)
                        diag_util_printf(" warning I2C 1 enabled!\n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_MDX_M_ENf,&check);
                    if(2==check)
                        diag_util_printf(" warning mid/mdio enabled!\n");
                    break;

               case 11:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 60, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 60 enabled! \n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_I2C_ENf,&check);
                    if(2&check)
                        diag_util_printf("warning I2C 1 enabled!\n");

                    reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_MDX_M_ENf,&check);
                    if(2==check)
                        diag_util_printf("warning!! mdc/mdio enabled!\n");
                    break;

               case 12:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 59, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 59 enabled! \n");

                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_SERI_LED_ENf,&value);
                    if(1==check)
                        diag_util_printf(" warning!! Serial LED enabled! \n");
                    break;

               case 13:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 58, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 58 enabled! \n");

                    reg_field_read(APOLLOMP_IO_LED_ENr,APOLLOMP_SERI_LED_ENf,&value);
                    if(1==check)
                        diag_util_printf(" warning!! Serial LED enabled! \n");
                    break;
               case 15:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 21, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 21 enabled! \n");
                    break;
               case 16:
                    reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 22, APOLLOMP_EN_GPIOf, (uint32 *)&check);
                    if(1==check)
                        diag_util_printf(" warning!! gpio 2 enabled! \n");
                    break;
                default:
                    break;
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_led_check_parallel_index */

/*
 * debug io voip-check
 */

cparser_result_t
cparser_cmd_debug_io_voip_check(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            int32   ret = RT_ERR_FAILED;
            uint32  value,slicType,ledIdx;
            slicType = 0;

#define DIAG_SLIC_PCM 1
#define DIAG_SLIC_SPI 2
#define DIAG_SLIC_ZSI 4
#define DIAG_SLIC_ISI 8

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_PCM_ENf,&value);
            if(1==value)
            {
                diag_util_printf("SLIC_PCM :enabled\n");
                slicType = slicType | DIAG_SLIC_PCM;
            }
            else
            {
                diag_util_printf("SLIC_PCM :disabled\n");
            }

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_SPI_ENf,&value);
            if(1==value)
            {
                diag_util_printf("SLIC_SPI :enabled\n");
                slicType = slicType | DIAG_SLIC_SPI;
            }
            else
            {
                diag_util_printf("SLIC_SPI :disabled\n");
            }

            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ZSI_ENf,&value);
            if(1==value)
            {
                diag_util_printf("SLIC_ZSI :enabled\n");
                slicType = slicType | DIAG_SLIC_ZSI;
            }
            else
            {
                diag_util_printf("SLIC_ZSI :disabled\n");
            }


            reg_field_read(APOLLOMP_IO_MODE_ENr,APOLLOMP_SLIC_ISI_ENf,&value);
            if(1==value)
            {
                diag_util_printf("SLIC_ISI :enabled\n");
                slicType = slicType | DIAG_SLIC_ISI;
            }
            else
            {
                diag_util_printf("SLIC_ISI :disabled\n");
            }
            if(0 != slicType)
            {
                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 25, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ISI)
                    {
                        diag_util_printf(" warning!! gpio 25 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 26, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 26 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 27, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 27 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 28, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ISI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 28 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 29, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ISI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 29 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 30, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 30 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 31, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI)
                    {
                        diag_util_printf(" warning!! gpio 31 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 32, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ISI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 32 enabled! \n");
                    }
                }

                reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, 33, APOLLOMP_EN_GPIOf, (uint32 *)&value);
                if(1 == value)
                {
                    if(slicType&DIAG_SLIC_PCM || slicType&DIAG_SLIC_SPI || slicType&DIAG_SLIC_ISI || slicType&DIAG_SLIC_ZSI)
                    {
                        diag_util_printf(" warning!! gpio 33 enabled! \n");
                    }
                }
            }
        }
        break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_io_voip */

/*
 * debug pon mode set ( gpon | epon | fiber-1g | fiber-100m | fiber-auto | fiber-force-1g | fiber-force-100m )
 */
cparser_result_t
cparser_cmd_debug_pon_mode_set_gpon_epon_fiber_1g_fiber_100m_fiber_auto_fiber_force_1g_fiber_force_100m(
    cparser_context_t *context)
{
    int ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_CHAR(4,0) == 'g')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_GPON);
    }
    else if(TOKEN_CHAR(4,0) == 'e')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_EPON);
    }
    else if(TOKEN_CHAR(4,7) == 'g')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_FIBER_1G);
    }
    else if(TOKEN_CHAR(4,9) == 'm')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_FIBER_100M);
    }
    else if(TOKEN_CHAR(4,9) == 'o')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_FIBER_AUTO);
    }
    else if(TOKEN_CHAR(4,13) == 'g')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_FIBER_FORCE_1G);
    }
    else if(TOKEN_CHAR(4,15) == 'm')
    {
        ret = rtk_ponmac_mode_set(PONMAC_MODE_FIBER_FORCE_100M);
    }
    else
    {
        diag_util_mprintf("no such mode %s\n", TOKEN_STR(4));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    if(ret)
    {
        diag_util_mprintf("pon mode set failed (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pon_mode_set_gpon_epon_fiber_1g_fiber_100m_fiber_auto */

/*
 * debug pon mode get
 */
cparser_result_t
cparser_cmd_debug_pon_mode_get(
    cparser_context_t *context)
{
    int ret;
    rtk_ponmac_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_ponmac_mode_get(&mode);
    if(ret)
    {
        diag_util_mprintf("pon mode get failed (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("pon mode: %s\n", diagStr_ponModeName[mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pon_mode_get */
/*
* debug pkt-parser-dump <UINT:count> latch-mode ( all | none | first-drop | first-pass | first-trap | drop | trap | acl | reason ) <STRING:filename>
*/
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_count_latch_mode_all_none_first_drop_first_pass_first_trap_drop_trap_acl_reason_filename(
    cparser_context_t *context,
    uint32_t  *count_ptr,
    char * *filename_ptr)
{
    uint32 chipId;
    uint32 ChipRevId;
    uint32 value, value_0 = 0;
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    int32 dbg_counter, cnt=0;
    int32 flag = 0;
    uint64 cntr;
    char file_name[300];
    rtk_hsb_t hsbData, hsbData_old;
    rtk_hsa_t hsaData;
    rtk_hsa_debug_t hsdData;
    rtk_port_t port;
    rtk_stat_port_type_t type;
    pkt_parser_debug_t parser_debug;
    osal_usecs_t  startTime, endTime;
    FILE *fp;
    FILE *fp_setting;
    char *file_name_setting = "/var/tmp/packet_parser_config.setting";

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_CHAR(4,0) == 'n') //none
    {
        value = 0x1;
    }
    else if(TOKEN_CHAR(4,0) == 'd') //drop
    {
        value = 0x5;
    }
    else if(TOKEN_CHAR(4,0) == 't') //drop
    {
        value = 0x6;
    }
    else if(TOKEN_CHAR(4,0) == 'r') //drop
    {
        value = 0x7;
    }
    else if(TOKEN_CHAR(4,1) == 'c') //acl
    {
        value = 0x7;
    }
    else if(TOKEN_CHAR(4,1) == 'l') //all
    {
        value = 0x0;
    }
    else if(TOKEN_CHAR(4,6) == 'd') //first-drop
    {
        value = 0x2;
    }
    else if(TOKEN_CHAR(4,6) == 'p') //first-pass
    {
        value = 0x3;
    }
    else if(TOKEN_CHAR(4,6) == 't') //first-trap
    {
        value = 0x4;
    }
    else
    {
        diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
        return CPARSER_ERR_INVALID_PARAMS;
    }

    if (( ret = drv_swcore_cid_get(&chipId,&ChipRevId)) != 0 )
    {
        return -1;
    }

    switch(chipId)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            sprintf(file_name, "/var/config/%s",TOKEN_STR(5));
            fp = fopen(file_name, "w");
            if(!fp)
            {
                printf("open file error\n");
                return CPARSER_NOT_OK;
            }

            if((value == 2) || (value == 3) || (value == 4))
            {
                flag = 1;
            }

            for(cnt = 0; cnt < *count_ptr; cnt++)
            {
                fprintf(fp, "        counter = %d\n", cnt+1);
                fprintf(fp, "packet ingress\n");

                if(flag ==1 )
                {
                    if ( (ret = reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &value)) != 0)
                    {
                        printf("setting fail !\n");
                        return -1;
                    }
                }

                if (( ret = apollomp_raw_hsbData_get(&hsbData)) != 0 )
                {
                    printf("NOT support\n");
                    fclose(fp);
                    return -1;
                }

                _debug_hsb_write(fp, &hsbData);

                fprintf(fp, "\npacket egress\n");

                if (( ret = apollomp_raw_hsaData_get(&hsaData)) != 0 )
                {
                    printf("NOT support\n");
                    fclose(fp);
                    return -1;
                }
                _debug_hsa_write(fp, &hsaData);

                fprintf(fp, "\n");

                if(flag ==1 )
                {
                    if ( (ret = reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &value_0)) != 0)
                    {
                        printf("setting to latch None fail !\n");
                        return -1;
                    }
                }
            }

            fprintf(fp, "\n\nMIB Counter\n");

            for(port = 0; port <= 2; port++)
            {
                fprintf(fp, "Port: %d\n", port);
                for (type = 0; type < MIB_PORT_CNTR_END; type++)
                {
                    if ((ret = rtk_stat_port_get(port, type, &cntr)) == RT_ERR_OK)
                    {
                        fprintf(fp, "%-35s: ",diagStr_mibName[type]);
#if defined(NO_MIB_SEPARATE)
                        fprintf(fp, "%25llu\n", cntr);
#else
                        fprintf(fp, "%25llu\n", cntr);
#endif
                    }
                }
                fprintf(fp, "\n");
            }

            fclose(fp);

            break;
#endif

#ifdef CONFIG_SDK_RTL9601B
        case RTL9601B_CHIP_ID:
            sprintf(file_name, "/var/tmp/%s",TOKEN_STR(5));
            fp = fopen(file_name, "w");
            if(!fp)
            {
                printf("open file error\n");
                return CPARSER_NOT_OK;
            }

            memset(&hsbData, 0, sizeof(rtk_hsb_t));
            memset(&hsbData_old, 0, sizeof(rtk_hsb_t));

            if((value == 2) || (value == 3) || (value == 4))
            {
                flag = 1;
            }

            for(port = 0; port < 3; port++) //reset port 0,1,2
            {
                if ( (ret = rtk_stat_port_reset(port)) != 0)
                {
                    printf("reset port %d error !\n", port);
                    return -1;
                }
            }

            //CHECK IF CPU WILL HANG UP
            _debug_pkt_parser_get(&parser_debug);

            //for(cnt = 0; cnt < *count_ptr; cnt++)
            cnt = 1;
            osal_time_usecs_get(&startTime);

            if (( ret = rtl9601b_hsbData_get(&hsbData_old)) != 0 )
            {
                printf("NOT support\n");
                fclose(fp);
                return -1;
            }

            while(1)
            {
                //time is more than user defined seconds
                osal_time_usecs_get(&endTime);
                if(parser_debug.time != 0xffffffff)
                {
                    if(((endTime - startTime)/1000000) > parser_debug.time)
                    {
                        printf("pkt-parser-dump: %d seconds time out\n", parser_debug.time);
                        break;
                    }
                }
                else
                {
                    if(((endTime - startTime)/1000000) > 10) //default timeout is 10 seconds
                    {
                        printf("pkt-parser-dump: 10 seconds time out \n");
                        break;
                    }
                }

                //counter is more than user defined seconds
                if(cnt > *count_ptr)
                {
                    printf("pkt-parser-dump: Total captured packets reach %d \n",*count_ptr);
                    break;
                }

                if(flag ==1 )
                {
                    if ( (ret = reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &value)) != 0)
                    {
                        printf("setting fail !\n");
                        fclose(fp);
                        return -1;
                    }
                }

                if (( ret = rtl9601b_hsbData_get(&hsbData)) != 0 )
                {
                    printf("NOT support\n");
                    fclose(fp);
                    return -1;
                }

                //if old & new hsb is the same, than skip this recod.
                if(memcmp(&hsbData, &hsbData_old, sizeof(rtk_hsb_t)) == 0)
                {
                    if(flag ==1 ){
                        if ( (ret = reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &value_0)) != 0){
                            printf("setting to latch None fail !\n");
                            fclose(fp);
                            return -1;
                        }
                    }
                    continue;
                }

                if(((parser_debug.spa_mask != 0xffff)      && !(1<< hsbData.spa & parser_debug.spa_mask)) ||
                   ((parser_debug.smac.octet[5]!=0xff)    && (memcmp(&hsbData.sa, &parser_debug.smac, sizeof(rtk_mac_t))!= 0)) ||
                   ((parser_debug.dmac.octet[5]!=0xff)    && (memcmp(&hsbData.da, &parser_debug.dmac, sizeof(rtk_mac_t))!= 0)) ||
                   ((parser_debug.ethertype != 0xffff)    && (parser_debug.ethertype != hsbData.ether_type)) ||
                   ((parser_debug.length_start != 0xffff) && (parser_debug.length_start > hsbData.pkt_len || parser_debug.length_end < hsbData.pkt_len)) ||
                   ((parser_debug.cvid != 0xffff)         && (parser_debug.cvid != hsbData.ctag)) ||
                   ((parser_debug.svid != 0xffff)         && (parser_debug.svid != hsbData.stag)) ||
                   ((parser_debug.sip != 0xffffffff)         && (parser_debug.sip != hsbData.sip)) ||
                   ((parser_debug.dip != 0xffffffff)         && (parser_debug.dip != hsbData.dip)) ||
                   ((parser_debug.protocol == 1) &&
                   !(((parser_debug.igmp_if == 1) && (hsbData.igmp_if == 1)) ||
                   ((parser_debug.udp_if == 1) && (hsbData.udp_if == 1)) ||
                   ((parser_debug.tcp_if == 1) && (hsbData.tcp_if == 1)) ||
                   ((parser_debug.oampdu == 1) && (hsbData.oampdu == 1)) ||
                   ((parser_debug.llc_other == 1) && (hsbData.llc_other == 1)) ||
                   ((parser_debug.snap_if == 1) && (hsbData.snap_if == 1)) ||
                   ((parser_debug.pppoe_if == 1) && (hsbData.pppoe_if == 1))))
                )
                {
                    if(flag ==1 ){
                        if ( (ret = reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &value_0)) != 0){
                            printf("setting to latch None fail !\n");
                            fclose(fp);
                            return -1;
                        }
                    }
                    continue;
                }

                fprintf(fp, "        counter = %d\n", cnt);
                fprintf(fp, "packet ingress\n");


                _debug_hsb_write(fp, &hsbData);

                fprintf(fp, "\npacket egress\n");

                if (( ret = rtl9601b_hsaData_get(&hsaData)) != 0 )
                {
                    printf("NOT support\n");
                    fclose(fp);
                    return -1;
                }
                _debug_hsa_write(fp, &hsaData);

                fprintf(fp, "\n");

                memcpy(&hsbData_old, &hsbData, sizeof(rtk_hsb_t));

                if(flag ==1 ){
                    if ( (ret = reg_field_write(RTL9601B_HSB_CTRLr,RTL9601B_LATCH_MODEf, &value_0)) != 0){
                        printf("setting to latch None fail !\n");
                        fclose(fp);
                        return -1;
                    }
                }

                cnt++;
            }

            fprintf(fp, "\n\nMIB Counter\n");

            for(port = 0; port <= 2; port++)
            {
                fprintf(fp, "Port: %d\n", port);
                for (type = 0; type < MIB_PORT_CNTR_END; type++)
                {
                    if ((ret = rtk_stat_port_get(port, type, &cntr)) == RT_ERR_OK)
                    {
                        fprintf(fp, "%-35s: ",diagStr_mibName[type]);
#if defined(NO_MIB_SEPARATE)
                        fprintf(fp, "%25llu\n", cntr);
#else
                        fprintf(fp, "%25llu\n", cntr);
#endif
                    }
                }
                fprintf(fp, "\n");
            }

            fclose(fp);

            break;
#endif
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            sprintf(file_name, "/var/tmp/%s",TOKEN_STR(5));
            fp = fopen(file_name, "w");
            if(!fp)
            {
                printf("open file error\n");
                return CPARSER_NOT_OK;
            }

            if((value == 2) || (value == 3) || (value == 4))
            {
                flag = 1;
            }

            for(port = 0; port < 2; port++)
            {
                if ( (ret = rtk_stat_port_reset(port)) != 0)
                {
                    printf("reset port %d error !\n", port);
                    return -1;
                }
            }

            for(cnt = 0; cnt < *count_ptr; cnt++)
            {
                fprintf(fp, "        counter = %d\n", cnt+1);
                fprintf(fp, "packet ingress\n");

                if(flag ==1 )
                {
                    if ( (ret = reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_MODEf, &value)) != 0)
                    {
                        printf("setting fail !\n");
                        return -1;
                    }
                }

                if (( ret = rtl9602c_hsbData_get(&hsbData)) != 0 )
                {
                    printf("NOT support\n");
                    fclose(fp);
                    return -1;
                }

                _debug_hsb_write(fp, &hsbData);

                fprintf(fp, "\npacket egress\n");

                if (( ret = rtl9602c_hsaData_get(&hsaData)) != 0 )
                {
                    printf("NOT support\n");
                    fclose(fp);
                    return -1;
                }
                _debug_hsa_write(fp, &hsaData);

                fprintf(fp, "\n");

                if(flag ==1 )
                {
                    if ( (ret = reg_field_write(RTL9602C_HSB_CTRLr,RTL9602C_LATCH_MODEf, &value_0)) != 0)
                    {
                        printf("setting to latch None fail !\n");
                        return -1;
                    }
                }
            }

            fprintf(fp, "\n\nMIB Counter\n");

            for(port = 0; port <= 2; port++)
            {
                fprintf(fp, "Port: %d\n", port);
                for (type = 0; type < MIB_PORT_CNTR_END; type++)
                {
                    if ((ret = rtk_stat_port_get(port, type, &cntr)) == RT_ERR_OK)
                    {
                        fprintf(fp, "%-35s: ",diagStr_mibName[type]);
#if defined(NO_MIB_SEPARATE)
                        fprintf(fp, "%25llu\n", cntr);
#else
                        fprintf(fp, "%25llu\n", cntr);
#endif
                    }
                }
                fprintf(fp, "\n");
            }

            fclose(fp);

            break;
#endif

        default:
            printf("NOT support\n");
            return -1;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_count_latch_mode_all_none_first_drop_first_pass_first_trap_drop_trap_acl_reason_filename */

/*
 * debug pkt-parser-dump clear
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_clear(
    cparser_context_t *context)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "w");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_clear */

/*
 * debug pkt-parser-dump show
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_show(
    cparser_context_t *context)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buff[50];
    char *str;
    char *pch;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "r");
    if(!fp)
    {
        //file not exist(not set before)
        return CPARSER_OK;
    }

    while(NULL != fgets(buff, sizeof(buff), fp))
    {
        printf("%s",buff);
    }
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_show */

/*
 * debug pkt-parser-dump set  time <UINT:time>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_time_time(
    cparser_context_t *context,
    uint32_t  *time_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }
    sprintf(buf,"time=%d\n",*time_ptr);

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_time_time */

/*
 * debug pkt-parser-dump set source-port <PORT_LIST:ports>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_source_port_ports(
    cparser_context_t *context,
    char * *ports_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }

    ret = DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4);
    if( ret != RT_ERR_OK)
    {
        fclose(fp);
        return RT_ERR_FAILED;
    }
    

    sprintf(buf,"source-port-mask=%d\n", portlist.portmask.bits[0]);

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_source_port_port */

/*
 * debug pkt-parser-dump set da <MACADDR:da>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_da_da(
    cparser_context_t *context,
    cparser_macaddr_t  *da_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }
    sprintf(buf,"DA=%d:%d:%d:%d:%d:%d\n", da_ptr->octet[5],da_ptr->octet[4],da_ptr->octet[3],da_ptr->octet[2],da_ptr->octet[1],da_ptr->octet[0]);

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_da_da */

/*
 * debug pkt-parser-dump set sa <MACADDR:sa>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_sa_sa(
    cparser_context_t *context,
    cparser_macaddr_t  *sa_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }
    sprintf(buf,"SA=%d:%d:%d:%d:%d:%d\n", sa_ptr->octet[5],sa_ptr->octet[4],sa_ptr->octet[3],sa_ptr->octet[2],sa_ptr->octet[1],sa_ptr->octet[0]);

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_sa_sa */

/*
 * debug pkt-parser-dump set ethertype <UINT:ethertype>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_ethertype_ethertype(
    cparser_context_t *context,
    uint32_t  *ethertype_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }
    sprintf(buf,"ethertype=%d\n", *ethertype_ptr);

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_ethertype_ethertype */

/*
 * debug pkt-parser-dump set length <UINT:start> <UINT:end>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_length_start_end(
    cparser_context_t *context,
    uint32_t  *start_ptr,
    uint32_t  *end_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }

    sprintf(buf,"length-start=%d\n", *start_ptr);
    fputs(buf, fp);

    sprintf(buf,"length-end=%d\n", *end_ptr);
    fputs(buf, fp);

    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_length_start_end */

/*
 * debug pkt-parser-dump set vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }

    sprintf(buf,"vid=%d\n", *vid_ptr);
    fputs(buf, fp);

    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_vid_vid */

/*
 * debug pkt-parser-dump set svid <UINT:svid>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_svid_svid(
    cparser_context_t *context,
    uint32_t  *svid_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[50];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }
    sprintf(buf,"svid=%d\n", *svid_ptr);

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_svid_svid */

/*
 * debug pkt-parser-dump set protocol ( all | igmp | udp | tcp | oam | llc | snap | pppoe ) */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_protocol_all_igmp_udp_tcp_oam_llc_snap_pppoe(
    cparser_context_t *context)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[100];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }

    if(TOKEN_CHAR(4,0) == 'a')
    {
        sprintf(buf,"igmp_if=%d\n", 1);
        fputs(buf, fp);
        sprintf(buf,"udp_if=%d\n", 1);
        fputs(buf, fp);
        sprintf(buf,"tcp_if=%d\n", 1);
        fputs(buf, fp);
        sprintf(buf,"oampdu=%d\n", 1);
        fputs(buf, fp);
        sprintf(buf,"llc_other=%d\n", 1);
        fputs(buf, fp);
        sprintf(buf,"snap_if=%d\n", 1);
        fputs(buf, fp);
        sprintf(buf,"pppoe_if=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 'i')
    {
        sprintf(buf,"igmp_if=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 'u')
    {
        sprintf(buf,"udp_if=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 't')
    {
        sprintf(buf,"tcp_if=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 'o')
    {
        sprintf(buf,"oampdu=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 'l')
    {
        sprintf(buf,"llc_other=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 's')
    {
        sprintf(buf,"snap_if=%d\n", 1);
    }
    else if(TOKEN_CHAR(4,0) == 'p')
    {
        sprintf(buf,"pppoe_if=%d\n", 1);
    }
    else
    {
        diag_util_mprintf("no such mode %s\n", TOKEN_STR(4));
        
        fclose(fp);
        return CPARSER_ERR_INVALID_PARAMS;
    }

    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_protocol_all_igmp_udp_tcp_oam_llc_snap_pppoe */

/*
 * debug pkt-parser-dump set ip ( sip | dip ) <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_debug_pkt_parser_dump_set_ip_sip_dip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    FILE *fp;
    char *file_name = "/var/tmp/packet_parser_config.setting";
    char buf[100];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    sprintf(buf,"\n");

    fp = fopen(file_name, "a");
    if(!fp)
    {
        printf("open file error\n");
        return CPARSER_NOT_OK;
    }

    if ('s' == TOKEN_CHAR(4,0))
    {
        sprintf(buf,"sip=%s\n", diag_util_inet_ntoa(*ip_ptr));
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        sprintf(buf,"dip=%s\n", diag_util_inet_ntoa(*ip_ptr));
    }
    fputs(buf, fp);
    fclose(fp);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_pkt_parser_dump_set_ip_sip_dip_ip */

/*
 * debug set smart-card <UINT:address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_smart_card_address_value(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *value_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    mem = *address_ptr;
    value = *value_ptr;

    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x3c, value), ret);

    value = 0x30000 | mem;
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x40, value), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_smart_card_address_value */

/*
 * debug get smart-card <UINT:address>
 */
cparser_result_t
cparser_cmd_debug_get_smart_card_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    uint32  mem = 0;
    uint32  value = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;

    value = 0x10000 | mem;
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x40, value), ret);

    DIAG_UTIL_ERR_CHK(ioal_mem32_read(0x44, &value), ret);

   diag_util_printf("Address:%d data:%d\n",mem,value);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_smart_card_address */

/*
 * debug reset-wsds ( analog | gpon | epon | warp-all-sds )
 */
cparser_result_t
cparser_cmd_debug_reset_wsds_analog_gpon_epon_warp_all_sds(
    cparser_context_t *context)
{
    uint32 data;
    int32 ret = RT_ERR_FAILED;
    uint32 field;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_RTL9602C
        case RTL9602C_CHIP_ID:
            if('a'==TOKEN_CHAR(2,0))
            {
                field   = RTL9602C_CFG_SFT_RSB_ANAf;
            }
            else if ('g'==TOKEN_CHAR(2,0))
            {
                field   = RTL9602C_CFG_SFT_RSTB_GPONf;
            }
            else if ('e'==TOKEN_CHAR(2,0))
            {
                field   = RTL9602C_CFG_SFT_RSTB_EPONf;
            }
            else if ('w'==TOKEN_CHAR(2,0))
            {
                field   = RTL9602C_CFG_SFT_RSTBf;
            }
            else
                break;
            data=0;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_WSDS_DIG_00r,field,&data), ret);
            data=1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9602C_WSDS_DIG_00r,field,&data), ret);

        break;
#endif

#ifdef CONFIG_SDK_RTL9607C
        case RTL9607C_CHIP_ID:
            if('a'==TOKEN_CHAR(2,0))
            {
                field   = RTL9607C_CFG_SFT_RSB_ANAf;
            }
            else if ('g'==TOKEN_CHAR(2,0))
            {
                field   = RTL9607C_CFG_SFT_RSTB_GPONf;
            }
            else if ('e'==TOKEN_CHAR(2,0))
            {
                field   = RTL9607C_CFG_SFT_RSTB_EPONf;
            }
            else if ('w'==TOKEN_CHAR(2,0))
            {
                field   = RTL9607C_CFG_SFT_RSTBf;
            }
            else
                break;

            data=0;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_WSDS_DIG_00r,field,&data), ret);
            data=1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9607C_WSDS_DIG_00r,field,&data), ret);

        break;
#endif


#ifdef CONFIG_SDK_RTL9603CVD
        case RTL9603CVD_CHIP_ID:
            if('a'==TOKEN_CHAR(2,0))
            {
                field   = RTL9603CVD_CFG_SFT_RSB_ANAf;
            }
            else if ('g'==TOKEN_CHAR(2,0))
            {
                field   = RTL9603CVD_CFG_SFT_RSTB_GPONf;
            }
            else if ('e'==TOKEN_CHAR(2,0))
            {
                field   = RTL9603CVD_CFG_SFT_RSTB_EPONf;
            }
            else if ('w'==TOKEN_CHAR(2,0))
            {
                field   = RTL9603CVD_CFG_SFT_RSTBf;
            }
            else
                break;

            data=0;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_WSDS_DIG_00r,field,&data), ret);
            data=1;
            DIAG_UTIL_ERR_CHK(reg_field_write(RTL9603CVD_WSDS_DIG_00r,field,&data), ret);

        break;
#endif

        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_reset_wsds_analog_gpon_epon_warp_all_sds */

#if defined(CONFIG_EUROPA_8290)
//#if defined(CONFIG_EUROPA_FEATURE)

//// FIXME  booster API are chip-dependent, should move to RTL8290_API ++
/* Convert Vout voltage to DAC value
 *
 *  Vout: desired output voltage (mV)
 *  Rfb1: resistor (K ohms)
 *  Rfb2: resistor (K ohms)
 */
uint32 _ldd_booster_Vout2Dac(uint32 Vout, uint32 Rfb1, uint32 Rfb2)
{
   uint32 dac_code;
   uint32 Vref = 1200; /* 1.2V */
   uint32 Vdefault;

   Vdefault = Vref*(1+Rfb1/Rfb2) + Rfb1*12;

   /* avoid overflow if Vout is lower than Vdefault */
   if (Vout<Vdefault)
   {
    dac_code = 0;
   }
   else
   {
    //dac_code = (Vout*128)/(Rfb1*100) - (Vref*128)/(Rfb1*100) - (Vref*128)/(Rfb2*100) - 12*128/100;
    dac_code = (Vout-Vref)*128/(Rfb1*100) - (Vref*128)/(Rfb2*100) - 12*128/100;
   }

   return dac_code;
}

/* Convert DAC value to Vout voltage
 *
 *  Rfb1: resistor (K ohms)
 *  Rfb2: resistor (K ohms)
 */
uint32 _ldd_booster_Dac2Vout(uint32 dac_code, uint32 Rfb1, uint32 Rfb2)
{
   uint32 Vout;
   uint32 Vref = 1200; /* 1.2V */

   Vout = Vref + Vref*Rfb1/Rfb2 + Rfb1*12 + (Rfb1*dac_code*78125)/100000;

   return Vout;
}
//// FIXME  booster API are chip-dependent, should move to RTL8290_API --


//// FIXME  RSSI & MPD API are chip-dependent, should move to RTL8290_API ++
/*
 * Get MPD Current (nA)
 */
uint32 _ldd_get_mpd_current()
{
    int ret;
    uint32 mpd_v0, i_mpd, mpd1;
    uint32 loopcnt, i;
    uint64 sum, sum_mpd;


    rtk_ldd_flash_data_get(EUROPA_MPD0_ADDR, 4,  &ldd_param.mpd0);
    mpd_v0 =  ldd_param.mpd0;

    loopcnt = 10;

    sum = 0;
    sum_mpd = 0;

    for (i=0;i<loopcnt;i++)
    {
        ret = rtk_ldd_tx_power_get(mpd_v0, &mpd1, &i_mpd);
        if(ret)
        {
            diag_util_printf("Get MPD current fail!!! (%d)\n", ret);
            return CPARSER_NOT_OK;
        }
        sum += i_mpd;
        sum_mpd += mpd1;
    }

    i_mpd = sum/loopcnt;
    mpd1 = sum_mpd/loopcnt;

    return i_mpd*1000; /* uA to nA */
}

/*
 * Get RSSI Current (nA)
 */
uint32 _ldd_get_rssi_current()
{
    int ret;
    uint32 rssi_v0, i_rssi, v_rssi;
    uint32 loopcnt, i, cnt;
    uint64 sum_i, sum_v;
    uint32 i_rssi_2, i_rssi_3, i_rssi_4;

    loopcnt = 10;

    rtk_ldd_flash_data_get(EUROPA_RSSI_V0_ADDR, 4, &ldd_param.rssi_v0);
    rssi_v0 = ldd_param.rssi_v0;
    {
        if (0==rssi_v0)
        {
            diag_util_printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
            return CPARSER_NOT_OK;
        }
    }

    sum_i = 0;
    sum_v = 0;

    for (i=0;i<loopcnt;i++)
    {
        ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
        if(ret)
        {
            diag_util_printf("Get RSSI current fail!!! (%d)\n", ret);
            return CPARSER_NOT_OK;
        }
        sum_i += i_rssi;
        sum_v += v_rssi;
    }

    i_rssi = sum_i/loopcnt;
    v_rssi = sum_v/loopcnt;


    return i_rssi;

}
//// FIXME  RSSI & MPD API are chip-dependent, should move to RTL8290_API --


/*
 *  init RSSI-V0
 */
uint32 _ldd_cal_init(void)
{
    int ret;
    uint32 rssi_v0;
    uint32 mpd0;
    uint32 i;
    uint64 sum;
    uint32 index;

    uint8 reg;

    diag_util_printf("\n *** Initialize for self-calibration ***\n\n"
      " ==> Please keep dark condition, NO optical Tx/Rx power!\n\n");

    /* get RSSI-V0 - ensure dark condition */
    rssi_v0 = 0;
    sum = 0;

    for (i=0;i<100;i++)
    {
        ret = rtk_ldd_rssiV0_get(&rssi_v0);
        if(ret)
        {
            diag_util_printf("Get RSSI V0 fail!!! (%d)\n", ret);

            return CPARSER_NOT_OK;
        }
        sum += rssi_v0;
    }
    rssi_v0 = sum/100;

    diag_util_printf("RSSI V0 = %d\n", rssi_v0);

    ldd_param.rssi_v0 = rssi_v0;
    rtk_ldd_flash_data_set(EUROPA_RSSI_V0_ADDR, 4, ldd_param.rssi_v0);

    return CPARSER_OK;
}

/*
 *  load parameters which will not init by driver (i.e not registers)
 */
uint32 _ldd_cal_load(void)
{
   uint8 reg;
   uint32 parm;

   diag_util_printf("Loading parameters...\n");

   /* MPD0 */
   rtk_ldd_flash_data_get(EUROPA_MPD0_ADDR, 4, &ldd_param.mpd0);

   /* RSSI-V0*/
   rtk_ldd_flash_data_get(EUROPA_RSSI_V0_ADDR, 4, &ldd_param.rssi_v0);

#if 0
   // NOTE: store parameters in ldd_param.xx, but not reg

   /* TX - P0_target */
   rtk_ldd_flash_data_get(EUROPA_DCL_P0_ADDR, 1, &reg);
   rtk_ldd_parameter_set(1, EUROPA_REG_W58_ADDR, &reg);

   /* TX - P1_target */
   rtk_ldd_flash_data_get(EUROPA_DCL_P1_ADDR, 1, &reg);
   rtk_ldd_parameter_set(1, EUROPA_REG_W59_ADDR, &reg);

   /* TX - Pavg_target */
   rtk_ldd_flash_data_get(EUROPA_DCL_PAVG_ADDR, 1, &reg);
   rtk_ldd_parameter_set(1, EUROPA_REG_W61_ADDR, &reg);

   /* TX - DAC scale */
   rtk_ldd_flash_data_get(EUROPA_W57_APCDIG_DAC_BIAS_ADDR, 1, &reg);
   {
       uint8 w57;

       rtk_ldd_parameter_get(1, EUROPA_REG_W57_ADDR, &w57);

       if (reg)
       {
           /* full scale, clear the bit */
       reg = ((1<<EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET) ^ 0xff);
       w57 = w57 & reg;
       }
       else
       {
           /* half scale, set the bit */
       reg = 1<<EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET;
       w57 = w57 | reg;
       }

       rtk_ldd_parameter_set(1, EUROPA_REG_W57_ADDR, &w57);
   }

   /* TX - Crossing */
   rtk_ldd_flash_data_get(EUROPA_CROSS_ADDR, 1, &reg);
   {
        uint8 w48, cross_val, cross_mask;

    rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &w48);

    cross_mask = (EUROPA_REG_W48_TX_OFFSET_MOD_EN_MASK |
              EUROPA_REG_W48_TX_OFFSET_MOD_SIGN_MASK |
              EUROPA_REG_W48_TX_OFFSET_MOD_STR_MASK);

        cross_val = reg & cross_mask;

        w48 = (w48 & (0xff ^ cross_mask)) | cross_val;

        rtk_ldd_parameter_set(1, EUROPA_REG_W48_ADDR, &w48);
   }
#endif

   /* TX DDMI - slope & offset */
   {
     rtk_ldd_flash_data_get(EUROPA_TX_A_ADDR, 4, &parm);
     ldd_param.tx_a = (int32) parm;
     rtk_ldd_flash_data_get(EUROPA_TX_B_ADDR, 4, &parm);
     ldd_param.tx_b = (int32) parm;
     rtk_ldd_flash_data_get(EUROPA_TX_C_ADDR, 4, &parm);
     ldd_param.tx_c = (int32) parm;
   }


   /* RX DDMI - slope & offset */
   {
     rtk_ldd_flash_data_get(EUROPA_RX_A_ADDR, 4, &parm);
     ldd_param.rx_a = (int32) parm;
     rtk_ldd_flash_data_get(EUROPA_RX_B_ADDR, 4, &parm);
     ldd_param.rx_b = (int32) parm;
     rtk_ldd_flash_data_get(EUROPA_RX_C_ADDR, 4, &parm);
     ldd_param.rx_c = (int32) parm;
   }


   /* DDMI - Temperature Offset */
   {
      rtk_ldd_flash_data_get(EUROPA_TEMP_OFFSET_ADDR, 1, &parm);
      ldd_param.temp_offset = (int8) parm;
   }

#if 0
   /* Temperature interrupt scale */
   rtk_ldd_flash_data_get(EUROPA_TEMP_INTR_SCALE_ADDR, 1, &ldd_param.temp_intr_scale);


   /* RX SD - LOS assert/de-assert threshold */
   rtk_ldd_flash_data_get(EUROPA_RX_TH_ADDR, 1, &ldd_param.rx_th);
   rtk_ldd_flash_data_get(EUROPA_RX_DETH_ADDR, 1, &ldd_param.rx_deth);
#endif

   diag_util_printf("done.\n");

   return CPARSER_OK;
}


/*
 *  save parameters
 */
uint32 _ldd_cal_save(void)
{
   uint8 reg;

   diag_util_printf("Saving parameters...\n");

#if 0
   avoid MPD0 & RSSIV0 incindently overwritten

   /* MPD0 */
   rtk_ldd_flash_data_set(EUROPA_MPD0_ADDR, 4, ldd_param.mpd0);

   /* RSSI-V0*/
   rtk_ldd_flash_data_set(EUROPA_RSSI_V0_ADDR, 4, ldd_param.rssi_v0);
#endif

   /* TX - P0_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W58_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_DCL_P0_ADDR, 1, reg);

   /* TX - P1_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W59_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_DCL_P1_ADDR, 1, reg);

   /* TX - Pavg_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W61_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_DCL_PAVG_ADDR, 1, reg);

   /* TX - DAC scale */
   rtk_ldd_parameter_get(1, EUROPA_REG_W57_ADDR, &reg);
   reg = (reg>>EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET) & 0x01;
   reg = reg ^ 1; // in flash, 1=FULL SCALE, but in chip, 1=HALF SCALE
   rtk_ldd_flash_data_set(EUROPA_W57_APCDIG_DAC_BIAS_ADDR, 1, reg);

   /* TX - Crossing */
   rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_CROSS_ADDR, 1, reg);


   /* TX DDMI - slope & offset */
   rtk_ldd_flash_data_set(EUROPA_TX_A_ADDR, 4, (uint32)ldd_param.tx_a);
   rtk_ldd_flash_data_set(EUROPA_TX_B_ADDR, 4, (uint32)ldd_param.tx_b);
   rtk_ldd_flash_data_set(EUROPA_TX_C_ADDR, 4, (uint32)ldd_param.tx_c);


   /* RX DDMI - slope & offset */
   rtk_ldd_flash_data_set(EUROPA_RX_A_ADDR, 4, (uint32)ldd_param.rx_a);
   rtk_ldd_flash_data_set(EUROPA_RX_B_ADDR, 4, (uint32)ldd_param.rx_b);
   rtk_ldd_flash_data_set(EUROPA_RX_C_ADDR, 4, (uint32)ldd_param.rx_c);


   /* DDMI - Temperature Offset */
   rtk_ldd_flash_data_set(EUROPA_TEMP_OFFSET_ADDR, 1, (uint32)ldd_param.temp_offset);

   /* Temperature interrupt scale */
   rtk_ldd_parameter_get(1, EUROPA_REG_TEMP_INT_OFFSET_ADDR, &ldd_param.temp_intr_scale);
   rtk_ldd_flash_data_set(EUROPA_TEMP_INTR_SCALE_ADDR, 1, ldd_param.temp_intr_scale);


   /* RX SD - LOS assert/de-assert threshold */
   rtk_ldd_parameter_get(1, EUROPA_REG_RX_TH_ADDR, &ldd_param.rx_th);
   rtk_ldd_flash_data_set(EUROPA_RX_TH_ADDR, 1, ldd_param.rx_th);

   rtk_ldd_parameter_get(1, EUROPA_REG_RX_DE_TH_ADDR, &ldd_param.rx_deth);
   rtk_ldd_flash_data_set(EUROPA_RX_DETH_ADDR, 1, ldd_param.rx_deth);

   diag_util_printf("done.\n");

   return CPARSER_OK;
}

/*
 *  refresh parameters
 */
uint32 _ldd_cal_refresh(void)
{
   int ret;

   diag_util_printf("Refreshing parameters...\n");

   ret = rtk_ldd_config_refresh();
   if(ret)
   {
      diag_util_printf("Refresh failed!!! (%d)\n", ret);
      return CPARSER_NOT_OK;
   }
 
   diag_util_printf("done.\n");

   return CPARSER_OK;
}


 


/* given (X1,Y1) (X2,Y2) (X3,Y3), using polynomial fitting to get
 * coefficients a, b and c of the equation y = a x^2 + b x + c
 *    Xn are RSSI current in nA
 *    Yn are known Rx power in nW */
uint32 _ldd_quad_regression(
    uint32_t  X1,
    uint32_t  Y1,
    uint32_t  X2,
    uint32_t  Y2,
    uint32_t  X3,
    uint32_t  Y3,
    double    *A,
    double    *B,
    double    *C)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1/1000; /* nA to uA */
    y1 = (double)Y1/1000;
    x2 = (double)X2/1000;
    y2 = (double)Y2/1000; /* nW to uW */
    x3 = (double)X3/1000;
    y3 = (double)Y3/1000;

    //
    // TODO : validation the input value ?
    //

    a = (((y1*x2 - y2*x1)/(x1 - x2)) - ((y2*x3 - y3*x2)/(x2 - x3)) )/(x1*x2 - x2*x3);
    c = (((y1*x2 - y2*x1)/(x1 - x2)*x3) - ((y2*x3 - y3*x2)/(x2 - x3)*x1))/(x1 - x3);
    b = (y1 - a*x1*x1-c)/x1;


    *A = a;
    *B = b;
    *C = c;

    return 0;
}

/* given (X1,Y1) (X2,Y2), using linear fitting to get
 * coefficients a and b of the equation y = a x + b
 *
 * if (X2,Y2 = (0, 0), thne use y = a x only.
 *
 *    Xn are MPD current in nA
 *    Yn are known Rx power in nW */
uint32 _ldd_lin_regression(
    uint32_t  X1,
    uint32_t  Y1,
    uint32_t  X2,
    uint32_t  Y2,
    double    *A,
    double    *B)
{
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    x1 = (double)X1/1000; /* nA to uA */
    y1 = (double)Y1/1000;
    x2 = (double)X2/1000;
    y2 = (double)Y2/1000; /* nW to uW */

    //
    // TODO : validation the input value ?
    //

    if (0==x2 && 0==y2)   /* single point data */
    {
       // avoid divide by zero
       if (0==x1)
       {
          diag_util_printf("%s: (%f,%f) (%f, %f) fail.\n",  __FUNCTION__,
              x1, y2, x2, y2);
      *A = 0;
      *B = 0;
      return 1;
       }
       a = y1 / x1;
       b = 0;
    }
    else
    {
       // avoid divide by zero
       if (0==(x2-x1))
       {
          diag_util_printf("%s: (%f,%f) (%f, %f) fail.\n",  __FUNCTION__,
              x1, y2, x2, y2);
      *A = 0;
      *B = 0;
      return 1;
       }
       a = (y2 - y1) / (x2 - x1);
       b = y2 - a * x2;
    }

    *A = a;
    *B = b;

    return 0;
}


void _ldd_laserlut_lin_interpolation(laser_lut_data_t sdata[], int num, laser_lut_data_t output[])
{
   int i;
   int data_index;

   int start, end;
   double slope_b, slope_m;
   int offset;

   data_index = 0;

   for (data_index=0; data_index < (num-1); data_index++)
   {
      start = sdata[data_index].temp+40;
      end = sdata[data_index+1].temp+40;
      offset = end - start;

      /* fill sampled data */
      output[start].bias = sdata[data_index].bias;
      output[start].mod  = sdata[data_index].mod;
      output[end].bias = sdata[data_index+1].bias;
      output[end].mod  = sdata[data_index+1].mod;

      /* calculate slope */
      slope_b = (sdata[data_index+1].bias - sdata[data_index].bias)/offset;
      slope_m = (sdata[data_index+1].mod  - sdata[data_index].mod )/offset;

      diag_util_printf("%d~%d: offset=%d slopb_b=%f slope_m=%f\n",
              sdata[data_index].temp, sdata[data_index+1].temp,
              offset, slope_b, slope_m);

      /* interpolation */
      for (i=start; i<(end-1); i++)
      {
          output[i+1].bias = output[i].bias + slope_b;
          output[i+1].mod  = output[i].mod  + slope_m;
      printf("[%d]: %f %f\n", i+1-40, output[i+1].bias, output[i+1].mod);
      }
   }

   /* extra-polation for high temp */
   start = sdata[num-1].temp+40;
   end = 110+40;
   for (i=start; i<end; i++)
   {
      output[i+1].bias = output[i].bias + slope_b;
      output[i+1].mod  = output[i].mod  + slope_m;
      printf("[%d]: %f %f\n", i+1-40, output[i+1].bias, output[i+1].mod);
   }
}


void _ldd_laserlut_fixup(laser_lut_data_t data[], laser_lut_data_t output[], int chop)
{
   int start, end;
   int i;

   /* in case the lowest point is higher than 0 degC */
   if (0 < data[0].temp)
   {
       for (i=0; i<data[0].temp; i++)
       {
           output[i+40].bias = data[0].bias;
           output[i+40].mod  = data[0].mod;
       }
   }

   /* 0~-40, use data from 0C */
   start = -40;
   end = 0;
   for (i=(start+40); i<(end+40); i++)
   {
       output[i].bias = output[end+40].bias;
       output[i].mod  = output[end+40].mod;
   }

   /* 100~110, use data from 100C */
   start = chop;
   end = 110;
   for (i=(start+40); i<=(end+40); i++)
   {
       output[i].bias = output[start+40].bias;
       output[i].mod  = output[start+40].mod;
   }
}

void _ldd_laserlut_data_add(int index, int32_t temp, double bias, double mod)
{
    index = index - 1;

    if (index > 0 || index <= MAX_LASER_LUT_DATA_NUM)
    {
        gLaserLutData[index].temp = temp;
    gLaserLutData[index].bias = bias;
    gLaserLutData[index].mod = mod;
    }
}


void _ldd_laserlut_data_dump()
{
    int i;

    for (i=0; i<MAX_LASER_LUT_DATA_NUM; i++)
    {
       if (0.0==gLaserLutData[i].bias || 0.0==gLaserLutData[i].mod)
       {
           break;
       }
       else
       {
           diag_util_printf("  [%2d]  %3d  %2.2f  %2.2f\n",
        i+1, gLaserLutData[i].temp,
        gLaserLutData[i].bias, gLaserLutData[i].mod); 
       }
    }
}

int _ldd_laserlut_data_num_entry()
{
    int i;
    int num;

    for (i=0, num=0; i<MAX_LASER_LUT_DATA_NUM; i++)
    {
       if (0.0==gLaserLutData[i].bias || 0.0==gLaserLutData[i].mod)
       {
           break;
       }
       else
       {
       num++;
       }
    }

    diag_util_printf("Num Data entry : %d\n", num);

    return num;
}


void _ldd_laserlut_data_clear()
{
    int i;

    for (i=0; i<MAX_LASER_LUT_DATA_NUM; i++)
    {
    gLaserLutData[i].temp = 0;
    gLaserLutData[i].bias = 0.0;
    gLaserLutData[i].mod  = 0.0;
    }
}


void _ldd_bubble_sort(uint32 iarr[], uint8 num)
{
    uint8  i, j, k;
    uint32 temp;

    //printk(KERN_EMERG "\nUnsorted Data:");
    //for (k = 0; k < num; k++) {
    //  printk(KERN_EMERG " %d ", iarr[k]);
   //}

    for (i = 1; i < num; i++)
    {
        for (j = 0; j < num - 1; j++)
        {
            if (iarr[j] > iarr[j + 1])
            {
                temp = iarr[j];
                iarr[j] = iarr[j + 1];
                iarr[j + 1] = temp;
            }
        }
    }

    //printk(KERN_EMERG"\nAfter pass %d : ", i);
    //for (k = 0; k < num; k++)
    //{
    //     printk(KERN_EMERG" %d ", iarr[k]);
    //}

}

void _ldd_short_data_get(uint8 *ptr_data, uint32 length, uint32 *value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printf("Data Length Error!!!!!!!\n");
         return;
    }
    temp32 = 0;
    for(i=0;i<length;i++)
    {
        temp8 = *ptr_data;
        temp32 = temp32 | ((uint32)temp8<<(8*((length-1)-i)));
        ptr_data++;
    }

    //printf("%s = 0x%x!!!!!!!\n", __FUNCTION__, temp32);

    *value = temp32;
    return;
}

void _ldd_short_data_set(uint8 *ptr_data, uint32 length, uint32 value)
{
    uint8  i, temp8;
    uint32 temp32;

    if ((length==0)||(length>4))
    {
         printf("Data Length Error!!!!!!!\n");
         return;
    }
    temp8 = 0;
    temp32 = value;
    for(i=0;i<length;i++)
    {
        *ptr_data = (uint8)(temp32>>(8*((length-1)-i)));
       // printf("%s %d: 0x%x!!!!!!!\n", __FUNCTION__, i, *ptr_data);
        ptr_data++;
    }
    return;
}

#if 0
void _ldd_long_set(uint8 *ptr_data, uint32 address, uint32 length, uint8 *flash_data)
{
    if(flash_data == NULL||ptr_data == NULL)
    {
        printk(KERN_EMERG "%s NULL!!! \n", __FUNCTION__);
        return;
    }

    printk(KERN_EMERG "%s: address = %d, length = %d!!! \n", __FUNCTION__,address, length);

    memset(ptr_data, flash_data, sizeof(uint8)*length));

    return;

}
#endif

void rtk_ldd_flash_data_set(uint32 address, uint32 length, uint32 value)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);

    fp = fopen("/var/config/europa.data","rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    memset(init_data, 0x0, sizeof(uint8)*EUROPA_PARAMETER_SIZE);
    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
    fclose(fp);
#if 0
    ptr_data = init_data;
    printf("Before Write!!!\n");
    for(i=0;i<256;i++)
    {
        output = *ptr_data;
        ptr_data ++;
        printf("0x%02x ", output);
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n");
    }
#endif

    ptr_data = init_data;
    ptr_data = ptr_data + address;
    _ldd_short_data_set(ptr_data, length, value);

    fp = fopen("/var/config/europa.data","wb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }
    fseek(fp, 0, SEEK_SET);
#if 0
    printf("After Write!!!\n");
    for(i=0;i<256;i++)
    {
        output = *ptr_data;
        ptr_data ++;
        printf("0x%02x ", output);
        if(i%8 == 7)
            printf("   ");
        if(i%16 == 15)
            printf("\n");
    }
#endif
    fwrite(init_data,sizeof(char), EUROPA_PARAMETER_SIZE,fp);
    fclose(fp);
    free(init_data);
    return;
}

void rtk_ldd_flash_data_get(uint32 address, uint32 length, uint32 *value)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);

    fp = fopen("/var/config/europa.data","rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    memset(init_data, 0x0, sizeof(uint8)*EUROPA_PARAMETER_SIZE);
    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
    ptr_data = init_data;

    ptr_data = ptr_data + address;
    _ldd_short_data_get(ptr_data, length, value);

    free(init_data);
    fclose(fp);

    return;
}

void rtk_ldd_flash_array_set(uint32 address, uint32 length, uint8 *data)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);

    fp = fopen("/var/config/europa.data","rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    memset(init_data, 0x0, sizeof(uint8)*EUROPA_PARAMETER_SIZE);
    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;

    memcpy(ptr_data, data, length);

    fp = fopen("/var/config/europa.data","wb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }
    fseek(fp, 0, SEEK_SET);

    fwrite(init_data,sizeof(char), EUROPA_PARAMETER_SIZE,fp);
    fclose(fp);
    free(init_data);
    return;
}


void rtk_ldd_flash_array_get(uint32 address, uint32 length, uint8 *data)
{
    FILE *fp;
    uint8 *init_data, *ptr_data;
    uint32 i;
    uint8 output;

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);

    fp = fopen("/var/config/europa.data","rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    memset(init_data, 0x0, sizeof(uint8)*EUROPA_PARAMETER_SIZE);
    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
    fclose(fp);

    ptr_data = init_data;
    ptr_data = ptr_data + address;

    memcpy(data, ptr_data, sizeof(uint8)*length);

    free(init_data);
    return;
}

/*
 * debug europa init ( dol | dcl | scl-bias | scl-mod )
 */
cparser_result_t
cparser_cmd_debug_europa_init_dol_dcl_scl_bias_scl_mod(
    cparser_context_t *context)
{
    int ret;
    FILE *fp;
    uint8 *init_data, *ptr_data;
    uint32 i, length, loop_mode, status, mpd0, value, temp;
    uint8 bias_full;

    DIAG_UTIL_PARAM_CHK();
    fp = fopen(EUROPA_PARAMETER_LOCATION,"rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return CPARSER_OK;
    }

    if(TOKEN_CHAR(3,0) == 'd')
    {
        if(TOKEN_CHAR(3,1) == 'o')
        {
            loop_mode = LDD_LOOP_DOL;
        }
        else if(TOKEN_CHAR(3,1) == 'c')
        {
            loop_mode = LDD_LOOP_DCL;
        }
        else
        {
            diag_util_printf("no such mode %s\n", TOKEN_STR(3));
            return CPARSER_ERR_INVALID_PARAMS;
        }
    }
    else if(TOKEN_CHAR(3,0) == 's')
    {
        if(TOKEN_CHAR(3,4) == 'b')
        {
            loop_mode = LDD_LOOP_SCL_BIAS;
        }
        else if(TOKEN_CHAR(3,4) == 'm')
        {
            loop_mode = LDD_LOOP_SCL_MOD;
        }
        else
        {
            diag_util_printf("no such mode %s\n", TOKEN_STR(3));
            return CPARSER_ERR_INVALID_PARAMS;
        }
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(3));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);
    if (!init_data)
    {
        diag_util_printf("Memory Allocation Fail!!!!!!!\n");
        return CPARSER_NOT_OK;
    }

    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);

#if 0
    ptr_data = init_data;
    for(i=0;i<EUROPA_PARAMETER_SIZE;i++)
    {
        diag_util_printf("0x%02x ", *ptr_data);
        if(i%8 == 7)
            diag_util_printf("   ");
        if(i%16 == 15)
            diag_util_printf("\n");
        ptr_data++;
    }
#endif

    //memset(&ldd_param, 0, sizeof(rtk_ldd_param_t));

    rtk_ldd_i2c_init(I2C_PORT_0);
    osal_time_mdelay(100);
    rtk_ldd_gpio_init();
    /*Update DDMI A0*/
    diag_util_printf("Update DDMI A0!!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data;
    rtk_ldd_parameter_set(0x7F, 0, ptr_data);
    /*Update DDMI A2*/
   diag_util_printf("Update DDMI A2!!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data + EUROPA_DDMI_A2_ADDR;
    rtk_ldd_parameter_set(0x7F, 0x100, ptr_data);
    /*Update Patch Code A4*/
    diag_util_printf("Update Patch Code A4!!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data + EUROPA_A4_ADDR;
    length = *ptr_data;
    ptr_data++;
    rtk_ldd_parameter_set(length, 0x200, ptr_data);

    /*Update DCL P0/P1/Pav*/
    //diag_util_printf("Update DCL P0/P1/Pav!!!!!!!\n");
    diag_util_printf("Update configurations!!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data + EUROPA_DCL_P0_ADDR;
    rtk_ldd_parameter_set(1, EUROPA_REG_W58_ADDR, ptr_data);
    ptr_data++;
    rtk_ldd_parameter_set(1, EUROPA_REG_W59_ADDR, ptr_data);
    ptr_data++;
    rtk_ldd_parameter_set(1, EUROPA_REG_W61_ADDR, ptr_data);

    osal_time_mdelay(100);
    rtk_ldd_powerOnStatus_get(&status);
    if (status !=0)
    {
        diag_util_printf("rtk_ldd_powerOnStatus_get error!!!!!!!status = 0x%x \n", status);
        return CPARSER_NOT_OK;
    }

    /* APC Enable flow*/
    diag_util_printf("APC Enable flow!!!!!!!\n");
    osal_time_mdelay(100);
    rtk_ldd_apcEnableFlow_set(LDD_APC_MCU_READY, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_CHECK_READY, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_TX_SD_THRESHOLD, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_BIAS_POWER_ON, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_POWER_SET, loop_mode);
    //rtk_ldd_apcEnableFlow_set(LDD_APC_DIGITAL_POWER_ON, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_R_PROCESS_DETECT, loop_mode);
    /*Richard suggest to move after R Process Detect*/
    rtk_ldd_apcEnableFlow_set(LDD_APC_DIGITAL_POWER_ON, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_TX_RPD_RCODE_OUT, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_OFFSET_CALIBR, loop_mode);

    mpd0 = 0;
    ret = rtk_ldd_mpd0_get(10, &mpd0);
    if(ret)
    {
        diag_util_printf("Get MPD0 fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }
    diag_util_printf("MPD0 = 0x%x\n", mpd0);
    ldd_param.mpd0 = mpd0;

    if (mpd0 == 0)
    {
        diag_util_printf("MPD0 Error in APC Enable flow!!!!!!!\n");
        diag_util_printf("Read MPD0 again!!!\n");
        ret = rtk_ldd_mpd0_get(1, &mpd0);
        return CPARSER_NOT_OK;
    }


    //rtk_ldd_apcEnableFlow_set(LDD_APC_MEASURE_MPD0, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_WAIT_CALIBR, loop_mode);
    rtk_ldd_apcEnableFlow_set(LDD_APC_MOVE_CALIBR_RESULT, loop_mode);

    //Check EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_MASK status
    ptr_data = init_data + EUROPA_W57_APCDIG_DAC_BIAS_ADDR;
    bias_full = *ptr_data;
    if (1==bias_full)
    {
    /* default is HALF scale, clear the bit for full scale */
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 57, &value), ret);
        value = (value&0xF7);//EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_MASK
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 57, value), ret);
    diag_util_printf("DAC Full Scale!\n");
    }

    /* Tx Enable flow */
    diag_util_printf("Tx Enable flow !!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data + EUROPA_LASER_LUT_ADDR;
    rtk_ldd_txEnableFlow_set(LDD_TX_BIAS_MOD_PROTECT, loop_mode, ptr_data);
    rtk_ldd_txEnableFlow_set(LDD_TX_ENABLE_TX, loop_mode, ptr_data);
    rtk_ldd_txEnableFlow_set(LDD_TX_BIAS_MOD_LUT, loop_mode, ptr_data);
    rtk_ldd_txEnableFlow_set(LDD_TX_LOOP_MODE, loop_mode, ptr_data);
    rtk_ldd_txEnableFlow_set(LDD_TX_BURST_MODE_CTRL, loop_mode, ptr_data);
    rtk_ldd_txEnableFlow_set(LDD_TX_WAIT_500MS, loop_mode, ptr_data);

    /* Rx Enable flow */
    diag_util_printf("Rx Enable flow !!!!!!!\n");
    osal_time_mdelay(100);
    rtk_ldd_rxEnableFlow_set(LDD_RX_THRESHOLD);
    /* Enable Booster: set REG_W4<4> = 1 */
    rtk_ldd_rxEnableFlow_set(LDD_RX_BOOSTER);
    /* Enable RX: set REG_W41<4> = 0 */
    rtk_ldd_rxEnableFlow_set(LDD_RX_RX);
    /*Enable RSSI*/
    rtk_ldd_rxEnableFlow_set(LDD_RX_RSSI);
    /*Enable DAC*/
    rtk_ldd_rxEnableFlow_set(LDD_RX_DAC);

    /* Update APD lookup table */
    diag_util_printf("Update APD lookup table!!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data + EUROPA_APD_LUT_ADDR;
    rtk_ldd_apdLut_set(ptr_data);

    diag_util_printf("Enable TX SD!!!!!!!\n");
    osal_time_mdelay(100);
    ptr_data = init_data + EUROPA_LASER_LUT_ADDR;
    rtk_ldd_txEnableFlow_set(LDD_TX_ENABLE_TX_SD, loop_mode, ptr_data);
    rtk_ldd_txEnableFlow_set(LDD_TX_FAULT_ENABLE, loop_mode, ptr_data);

#if 0
    ptr_data = init_data + EUROPA_RX_A_ADDR;
    _ldd_short_data_get(ptr_data, 4, &value);
    /*temp =  *ptr_data;
    value = temp<<24;
    ptr_data++;
    temp =  *ptr_data;
    value = value | (temp<<16);
    ptr_data++;
    temp =  *ptr_data;
    value = value | (temp<<8);
    ptr_data++;
    temp =  *ptr_data;
    value = value | temp;*/
    ldd_param.rx_a = value;

    ptr_data = init_data + EUROPA_RX_B_ADDR;
    _ldd_short_data_get(ptr_data, 4, &value);
   /* temp =  *ptr_data;
    value = temp<<24;
    ptr_data++;
    temp =  *ptr_data;
    value = value | (temp<<16);
    ptr_data++;
    temp =  *ptr_data;
    value = value | (temp<<8);
    ptr_data++;
    temp =  *ptr_data;
    value = value | temp;*/
    ldd_param.rx_b = value;

    ptr_data = init_data + EUROPA_RX_C_ADDR;
    _ldd_short_data_get(ptr_data, 4, &value);
    /*temp =  *ptr_data;
    value = temp<<24;
    ptr_data++;
    temp =  *ptr_data;
    value = value | (temp<<16);
    ptr_data++;
    temp =  *ptr_data;
    value = value | (temp<<8);
    ptr_data++;
    temp =  *ptr_data;
    value = value | temp;*/
    ldd_param.rx_c = value;

    //rx_a = a*1048576
    //ldd_param.rx_a = 18;
    //rx_b = b*1048576
    //ldd_param.rx_b = 318798;
    //rx_c = c*1048576
    //ldd_param.rx_c = 20124;
#endif

    /*** ------------------ user_parameter_init() ----------------- ***/
    diag_util_printf("init user parameters ...\n");

    // FIXME  user_parameter_init();
    // FIXME  reg accessing should encap into a logical level API
    //
    // setup crossing point adjustment +
    //  W48[5]:  TX_OFFSET_MOD_EN, 1 enable
    //  W48[4]:  TX_OFFSET_MOD_SIGN, 0 for up (lift crossing point)
    //  W48[3:0]:  TX_OFFSET_MOD_STR: 0 ~ F, strength
    ptr_data = init_data + EUROPA_CROSS_ADDR;
    {
        uint8 w48, cross_val, cross_mask;

    rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &w48);

    cross_mask = (EUROPA_REG_W48_TX_OFFSET_MOD_EN_MASK |
              EUROPA_REG_W48_TX_OFFSET_MOD_SIGN_MASK |
              EUROPA_REG_W48_TX_OFFSET_MOD_STR_MASK);

        cross_val = (*ptr_data) & cross_mask;

        diag_util_printf("cross_val = 0x%x \n", cross_val);

        w48 = (w48 & (0xff ^ cross_mask)) | cross_val;

        diag_util_printf("w48 = 0x%x \n", w48);
        rtk_ldd_parameter_set(1, EUROPA_REG_W48_ADDR, &w48);
    }
    // setup crossing point adjustment -


    // temperature change interrupt scale
    ptr_data = init_data + EUROPA_TEMP_INTR_SCALE_ADDR;
    rtk_ldd_parameter_set(1, EUROPA_REG_TEMP_INT_OFFSET_ADDR, ptr_data);


    // LOS assert/de-assert threshold
    ptr_data = init_data + EUROPA_RX_TH_ADDR;
    rtk_ldd_parameter_set(1, EUROPA_REG_RX_TH_ADDR, ptr_data);
    ptr_data = init_data + EUROPA_RX_DETH_ADDR;
    rtk_ldd_parameter_set(1, EUROPA_REG_RX_DE_TH_ADDR, ptr_data);

    diag_util_printf("init user parameters done!\n");
    /*** ------------------ user_parameter_init() ----------------- ***/


    diag_util_printf("\n");
    free(init_data);
    fclose(fp);
    diag_util_printf("\nEuropa Initialization Success!!!\n");

    // FIXME save MPD0 on every init ?
    //rtk_ldd_flash_data_set(EUROPA_MPD0_ADDR, 4, ldd_param.mpd0);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_init_dol_dcl_scl_bias_scl_mod */

/*
 * debug europa ft-test <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_europa_ft_test_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int ret;
    uint32  mem = 0;
    uint32  value = 0;
#ifdef CONFIG_SDK_RTL9601B
    rtl9601b_sds_acc_t sds_acc;
#endif
    FILE *fp;
    uint8 *init_data, *ptr_data;
    uint32 vdd;
    uint32 loop_mode;
    int32  test_result;
    uint16 temp_k;
    uint32 i, length, status, mpd0;
    uint8 bias_full;
    rtk_gpon_prbs_t prbsCfg;
    uint32 i_mpd, mpd1;
    uint32 loopcnt;
    uint64 sum, sum_mpd;

    DIAG_UTIL_PARAM_CHK();

    if(*index_ptr == 0)
    {
        diag_util_printf("FT Test: DUT init\n");

       DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 100, 0x7d), ret);
        //PRBS
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x88, 0x8), ret);
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x1f4, 0x1), ret);
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22038, 0x1), ret);
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22030, 0x1000), ret);

        fp = fopen(EUROPA_PARAMETER_LOCATION,"rb");

        if (NULL ==fp)
        {
            diag_util_printf("Open file in /var/config/ error!!!!!!!\n");
            return CPARSER_NOT_OK;
        }

        init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);
        if (!init_data)
        {
            diag_util_printf("Memory Allocation Fail!!!!!!!\n");
            return CPARSER_NOT_OK;
        }
        fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
        /*Update W48*/
        ptr_data = init_data + EUROPA_A4_ADDR_BASE+ 48;
        rtk_ldd_parameter_set(1, EUROPA_REG_W48_ADDR, ptr_data);
        osal_time_mdelay(100);
        diag_util_printf("Update W48 from flash = 0x%x\n", *ptr_data);

        return CPARSER_OK;
    }
    else if(*index_ptr == 1)
    {
        diag_util_printf("FT Test: OLT init\n");

#ifdef CONFIG_SDK_RTL9601B
        memset(&sds_acc, 0, sizeof(sds_acc));
        sds_acc.index = 1;
        sds_acc.page = 0x20;
        sds_acc.regaddr = 1;

        /* RTL9601B uses indirect access to access serdes */
        _rtl9601b_serdes_ind_write(sds_acc, 0x80);
#endif

        //PRBS
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x88, 0x8), ret);
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x1f4, 0x1), ret);
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22038, 0x1), ret);
       DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22030, 0x1000), ret);

        DIAG_UTIL_ERR_CHK(rtk_i2c_enable_set(0, DISABLED), ret);
        DIAG_UTIL_ERR_CHK(rtk_gpio_mode_set(7,GPIO_OUTPUT), ret);
        DIAG_UTIL_ERR_CHK(rtk_gpio_state_set(7,ENABLED), ret);
        DIAG_UTIL_ERR_CHK(rtk_gpio_databit_set(7,0), ret);

        return CPARSER_OK;
    }
    else if(*index_ptr == 2)
    {
        diag_util_printf("FT Test: Reset DUT packet count and error rate\n");
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22034, 0xc0af), ret);
        osal_time_mdelay(100);
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22034, 0x40af), ret);
        return CPARSER_OK;
    }
    else if(*index_ptr == 3)
    {
        diag_util_printf("FT Test: Reset OLT packet count and error rate\n");
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22034, 0xc0fa), ret);
        osal_time_mdelay(100);
        DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x22034, 0x40fa), ret);
        return CPARSER_OK;
    }
    else if(*index_ptr == 4)
    {
        diag_util_printf("FT Test: Read packet count and error rate\n");
        mem = 0x2203C;
        DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &value), ret);
        diag_util_printf("Memory 0x%x : 0x%08x\n", mem, value);

        if (value != 0)
        {
             diag_util_printf("Error Numer = 0x%08x, Test Fail!!!!!!!\n", value);
        }

        mem = 0x22040;
        DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &value), ret);
        diag_util_printf("Memory 0x%x : 0x%08x\n", mem, value);

        if (value == 0)
        {
             diag_util_printf("Packet Numer = 0x%08x, Test Fail!!!!!!!\n", value);
        }

        return CPARSER_OK;
    }
    else if(*index_ptr == 5)
    {
        diag_util_printf("FT Test: Change to DOL mode\n");
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 69, &value), ret);
        value = (value&0xFC);//DOL mode
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 69, value), ret);

        /*REG_R33 -> REG_W54   for bias current*/
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x55, 33, &value), ret);
        diag_util_printf("Current Bias R33 = 0x%x, Set REG_R33 -> REG_W54\n", value);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 54, value), ret);
        /*REG_R32 -> REG_W55   for modulation current*/
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x55, 32, &value), ret);
        diag_util_printf("Current Modulation R32 = 0x%x, Set REG_R32 -> REG_W55\n", value);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 55, value), ret);

        /* Load BIAS/MOD initial code, Write REG_W70[7] to 1 and than write to 0 */
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0xbf), ret);
        osal_time_mdelay(100);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0x3f), ret);

        return CPARSER_OK;
    }
    else if(*index_ptr == 10)
    {
        /*Disable Interrupt */
        DIAG_UTIL_ERR_CHK(rtk_ldd_calibration_state_set(ENABLED), ret);

        /*Reset Test*/
        //printf("Reset Test\n");
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 102, 0x76), ret);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 103, 0xBF), ret);

        /*Set PIN_RESET = 0->1*/
        //printf("Set PIN_RESET (GPIO 15) 0->1\n");
        DIAG_UTIL_ERR_CHK(rtk_gpio_databit_set(15, 0), ret);
        osal_time_mdelay(100);
        DIAG_UTIL_ERR_CHK(rtk_gpio_databit_set(15, 1), ret);
        osal_time_mdelay(500);

        /*Check value*/
        //printf("Check value\n");
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 102, &value), ret);
        if (value != 0xA0)
        {
             diag_util_printf("Test Fail: {F002}\n");
            return CPARSER_OK;
        }
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 103, &value), ret);
        if (value!= 0xC0)
        {
             diag_util_printf("Test Fail: {F002}\n");
            return CPARSER_OK;
        }
        diag_util_printf("Test Pass: {P002}\n");

        osal_time_mdelay(100);

        /*Re-init*/
        diag_util_printf("\nStart Europa Initialization !!!\n");
        loop_mode = LDD_LOOP_DCL;
        fp = fopen(EUROPA_PARAMETER_LOCATION,"rb");
        if (NULL ==fp)
        {
            printf("Open file in /var/config/ error!!!!!!!\n");
            return CPARSER_OK;
        }
        init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);
        if (!init_data)
        {
            diag_util_printf("Memory Allocation Fail!!!!!!!\n");
            return CPARSER_NOT_OK;
        }

        fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
        //memset(&ldd_param, 0, sizeof(rtk_ldd_param_t));

        rtk_ldd_i2c_init(I2C_PORT_0);
        osal_time_mdelay(100);
        rtk_ldd_gpio_init();
        /*Update DDMI A0*/
        //diag_util_printf("Update DDMI A0!!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data;
        rtk_ldd_parameter_set(0x7F, 0, ptr_data);
        /*Update DDMI A2*/
       //diag_util_printf("Update DDMI A2!!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data + EUROPA_DDMI_A2_ADDR;
        rtk_ldd_parameter_set(0x7F, 0x100, ptr_data);
        /*Update Patch Code A4*/
        //diag_util_printf("Update Patch Code A4!!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data + EUROPA_A4_ADDR;
        length = *ptr_data;
        ptr_data++;
        rtk_ldd_parameter_set(length, 0x200, ptr_data);

        /*Update DCL P0/P1/Pav*/
        //diag_util_printf("Update configurations!!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data + EUROPA_DCL_P0_ADDR;
        rtk_ldd_parameter_set(1, EUROPA_REG_W58_ADDR, ptr_data);
        ptr_data++;
        rtk_ldd_parameter_set(1, EUROPA_REG_W59_ADDR, ptr_data);
        ptr_data++;
        rtk_ldd_parameter_set(1, EUROPA_REG_W61_ADDR, ptr_data);

        osal_time_mdelay(100);
        rtk_ldd_powerOnStatus_get(&status);
        if (status !=0)
        {
            diag_util_printf("rtk_ldd_powerOnStatus_get error!!!!!!!status = 0x%x \n", status);
            return CPARSER_NOT_OK;
        }

        /* APC Enable flow*/
        //diag_util_printf("APC Enable flow!!!!!!!\n");
        osal_time_mdelay(100);
        rtk_ldd_apcEnableFlow_set(LDD_APC_MCU_READY, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_CHECK_READY, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_TX_SD_THRESHOLD, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_BIAS_POWER_ON, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_POWER_SET, loop_mode);
        //rtk_ldd_apcEnableFlow_set(LDD_APC_DIGITAL_POWER_ON, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_R_PROCESS_DETECT, loop_mode);
        /*Richard suggest to move after R Process Detect*/
        rtk_ldd_apcEnableFlow_set(LDD_APC_DIGITAL_POWER_ON, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_TX_RPD_RCODE_OUT, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_OFFSET_CALIBR, loop_mode);

        mpd0 = 0;
        ret = rtk_ldd_mpd0_get(10, &mpd0);
        if(ret)
        {
            diag_util_printf("Get MPD0 fail!!! (%d)\n", ret);
            return CPARSER_NOT_OK;
        }
       // diag_util_printf("MPD0 = 0x%x\n", mpd0);
        ldd_param.mpd0 = mpd0;

        if (mpd0 == 0)
        {
            diag_util_printf("MPD0 Error in APC Enable flow!!!!!!!\n");
            diag_util_printf("Read MPD0 again!!!\n");
            ret = rtk_ldd_mpd0_get(1, &mpd0);
            return CPARSER_NOT_OK;
        }

        //rtk_ldd_apcEnableFlow_set(LDD_APC_MEASURE_MPD0, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_WAIT_CALIBR, loop_mode);
        rtk_ldd_apcEnableFlow_set(LDD_APC_MOVE_CALIBR_RESULT, loop_mode);

        //Check EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_MASK status
        ptr_data = init_data + EUROPA_W57_APCDIG_DAC_BIAS_ADDR;
        bias_full = *ptr_data;
        if (1==bias_full)
        {
        /* default is HALF scale, clear the bit for full scale */
            DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 57, &value), ret);
            value = (value&0xF7);//EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_MASK
            DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 57, value), ret);
             //diag_util_printf("DAC Full Scale!\n");
        }

        /* Tx Enable flow */
        //diag_util_printf("Tx Enable flow !!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data + EUROPA_LASER_LUT_ADDR;
        rtk_ldd_txEnableFlow_set(LDD_TX_BIAS_MOD_PROTECT, loop_mode, ptr_data);
        rtk_ldd_txEnableFlow_set(LDD_TX_ENABLE_TX, loop_mode, ptr_data);
        rtk_ldd_txEnableFlow_set(LDD_TX_BIAS_MOD_LUT, loop_mode, ptr_data);
        rtk_ldd_txEnableFlow_set(LDD_TX_LOOP_MODE, loop_mode, ptr_data);
        rtk_ldd_txEnableFlow_set(LDD_TX_BURST_MODE_CTRL, loop_mode, ptr_data);
        rtk_ldd_txEnableFlow_set(LDD_TX_WAIT_500MS, loop_mode, ptr_data);

        /* Rx Enable flow */
        //diag_util_printf("Rx Enable flow !!!!!!!\n");
        osal_time_mdelay(100);
        rtk_ldd_rxEnableFlow_set(LDD_RX_THRESHOLD);
        /* Enable Booster: set REG_W4<4> = 1 */
        rtk_ldd_rxEnableFlow_set(LDD_RX_BOOSTER);
        /* Enable RX: set REG_W41<4> = 0 */
        rtk_ldd_rxEnableFlow_set(LDD_RX_RX);
        /*Enable RSSI*/
        rtk_ldd_rxEnableFlow_set(LDD_RX_RSSI);
        /*Enable DAC*/
        rtk_ldd_rxEnableFlow_set(LDD_RX_DAC);

        /* Update APD lookup table */
        //diag_util_printf("Update APD lookup table!!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data + EUROPA_APD_LUT_ADDR;
        rtk_ldd_apdLut_set(ptr_data);

        //diag_util_printf("Enable TX SD!!!!!!!\n");
        osal_time_mdelay(100);
        ptr_data = init_data + EUROPA_LASER_LUT_ADDR;
        rtk_ldd_txEnableFlow_set(LDD_TX_ENABLE_TX_SD, loop_mode, ptr_data);
        rtk_ldd_txEnableFlow_set(LDD_TX_FAULT_ENABLE, loop_mode, ptr_data);


        /*** ------------------ user_parameter_init() ----------------- ***/
        //diag_util_printf("init user parameters ...\n");

        // FIXME  user_parameter_init();
        // FIXME  reg accessing should encap into a logical level API
        //
        // setup crossing point adjustment +
        //  W48[5]:  TX_OFFSET_MOD_EN, 1 enable
        //  W48[4]:  TX_OFFSET_MOD_SIGN, 0 for up (lift crossing point)
        //  W48[3:0]:  TX_OFFSET_MOD_STR: 0 ~ F, strength
        ptr_data = init_data + EUROPA_CROSS_ADDR;
        {
            uint8 w48, cross_val, cross_mask;

             rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &w48);

            cross_mask = (EUROPA_REG_W48_TX_OFFSET_MOD_EN_MASK |
                  EUROPA_REG_W48_TX_OFFSET_MOD_SIGN_MASK |
                  EUROPA_REG_W48_TX_OFFSET_MOD_STR_MASK);

            cross_val = (*ptr_data) & cross_mask;

            //diag_util_printf("cross_val = 0x%x \n", cross_val);

            w48 = (w48 & (0xff ^ cross_mask)) | cross_val;

            //diag_util_printf("w48 = 0x%x \n", w48);
            rtk_ldd_parameter_set(1, EUROPA_REG_W48_ADDR, &w48);
        }
        // setup crossing point adjustment -
        //diag_util_printf("init user parameters done!\n");
        /*** ------------------ user_parameter_init() ----------------- ***/
        diag_util_printf("\n");
        free(init_data);
        fclose(fp);
        diag_util_printf("\nEuropa Initialization Success!!!\n");

        /*Get 3.3V*/
        DIAG_UTIL_ERR_CHK(rtk_ldd_vdd_get(&vdd), ret);
        diag_util_printf("VDD = %duV\n", vdd);
        test_result = 3300000 - vdd;
    /*7% difference is allowed*/
        if (abs(test_result)>231000)
        {
             diag_util_printf("Test Fail: {F705}\n");
            return CPARSER_OK;
        }
        diag_util_printf("Test Pass: {P705}\n");

        /*Get TX Power*/
         prbsCfg = RTK_GPON_PRBS_31;
      DIAG_UTIL_ERR_CHK(rtk_gpon_txForcePRBS_set(prbsCfg), ret);

        loopcnt = 20;
        sum = 0;
        sum_mpd = 0;
        for (i=0;i<loopcnt;i++)
        {
            ret = rtk_ldd_tx_power_get(ldd_param.mpd0, &mpd1, &i_mpd);
            if(ret)
            {
                diag_util_printf("Get TX power fail!!! (%d)\n", ret);
                return CPARSER_NOT_OK;
            }
            sum += i_mpd;
            sum_mpd += mpd1;
        }

        i_mpd = sum/loopcnt;
        mpd1 = sum_mpd/loopcnt;

        diag_util_printf("Get TX power MPD0 = %d, MPD1 = %d\n", ldd_param.mpd0, mpd1);

         prbsCfg = RTK_GPON_PRBS_OFF;
      DIAG_UTIL_ERR_CHK(rtk_gpon_txForcePRBS_set(prbsCfg), ret);
        test_result = 445000 - mpd1;
    /*7% difference is allowed*/
        if (abs(test_result)>32000)
        {
             diag_util_printf("Test Fail: {F703}\n");
            return CPARSER_OK;
        }
        diag_util_printf("Test Pass: {P703}\n");


        /*Get Temperature*/
        DIAG_UTIL_ERR_CHK(rtk_ldd_temperature_get(&temp_k), ret);
        test_result = (temp_k*100-27315)/100;
        diag_util_printf("Temperature = %dK (%dC)\n", temp_k, test_result);

        if (abs(test_result-80)>3)
        {
             diag_util_printf("Test Fail: {F704}\n");
            return CPARSER_OK;
        }
        diag_util_printf("Test Pass: {P704}\n");

    }
    else if(*index_ptr == 11)
    {

        /*RX threshold Test 1*/
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 105, 0x40), ret);
        diag_util_printf("Set A4/105 = 0x40\n");
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 106, 0xA0), ret);
        diag_util_printf("Set A4/106 = 0xA0\n");
    }
    else if(*index_ptr == 12)
    {

        /*RX threshold Test 1*/
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 105, 0x10), ret);
        diag_util_printf("Set A4/105 = 0x10\n");
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 106, 0x40), ret);
        diag_util_printf("Set A4/106 = 0x40\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_ft_test_index */

/*
 * debug europa get tx-power ( original | statistic ) <UINT:loop_count>
 */
cparser_result_t
cparser_cmd_debug_europa_get_tx_power_original_statistic_loop_count(
    cparser_context_t *context,
    uint32_t  *loop_count_ptr)
{
    int ret;
    uint32 mpd_v0, i_mpd, mpd1;
    uint32 loopcnt, i;
    uint64 sum, sum_mpd;
    int32 tx_power;
    uint32 parm;

    double temp_f;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_flash_data_get(EUROPA_TX_A_ADDR, 4, &parm);
    ldd_param.tx_a = (int32) parm;
    rtk_ldd_flash_data_get(EUROPA_TX_B_ADDR, 4, &parm);
    ldd_param.tx_b = (int32) parm;
    rtk_ldd_flash_data_get(EUROPA_TX_C_ADDR, 4, &parm);
    ldd_param.tx_c = (int32) parm;

    if(TOKEN_CHAR(4,0) == 'o')
    {
        rtk_ldd_flash_data_get(EUROPA_MPD0_ADDR, 4,  &ldd_param.mpd0);
        diag_util_printf("Start to get MPD in original mode... \n", i);
        diag_util_printf("MPD0 = 0x%x \n", ldd_param.mpd0);
        mpd_v0 =  ldd_param.mpd0;
        loopcnt = *loop_count_ptr;
        sum = 0;
        sum_mpd = 0;
        for (i=0;i<loopcnt;i++)
        {
            diag_util_printf("Loop %d: \n", i);
            ret = rtk_ldd_tx_power_get(mpd_v0, &mpd1, &i_mpd);
            if(ret)
            {
                diag_util_printf("Get TX power fail!!! (%d)\n", ret);
                return CPARSER_NOT_OK;
            }
            sum += i_mpd;
            sum_mpd += mpd1;
        }

        i_mpd = sum/loopcnt;
        mpd1 = sum_mpd/loopcnt;

#if 0
        tx_power = (i_mpd*ldd_param.tx_a)>>TX_A_SHIFT +
           (ldd_param.tx_b)>>TX_B_SHIFT;
#endif
    if(0==i_mpd || 0==ldd_param.tx_a)
    {
       diag_util_printf("Invalid MPD current or DDMI coefficients!!!\n");
       temp_f = 0;
    }
    else if (0 == ldd_param.tx_b)
    {
       /* tx_b=0 means tx ddmi is calibrated at single point */
       temp_f = (double)i_mpd*(double)ldd_param.tx_a/(1<<TX_A_SHIFT);
    }
    else
    {
       temp_f = (double)i_mpd*(double)ldd_param.tx_a/(1<<TX_A_SHIFT) + \
            (double)(ldd_param.tx_b)/(1<<TX_B_SHIFT);
    }

    tx_power = (0.0<temp_f) ? (uint32)(temp_f*1000) : 0; /* nW */


        diag_util_printf("Get TX power MPD1 = %d, I_MPD = %d uA\n", mpd1, i_mpd);
        diag_util_printf("TX Power = %d nW\n", tx_power);
    }
    else if(TOKEN_CHAR(4,0) == 's')
    {
        diag_util_printf("Start to get MPD in statistic mode... \n", i);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_tx_power_original_statistic_loop_count */

/*
 * debug europa get rx-power ( original | statistic | window ) <UINT:loop_count>
 */
cparser_result_t
cparser_cmd_debug_europa_get_rx_power_original_statistic_window_loop_count(
    cparser_context_t *context,
    uint32_t  *loop_count_ptr)
{
    int ret;
    uint32 rssi_v0, i_rssi, v_rssi;
    uint32 loopcnt, i, cnt;
    uint64 sum_i, sum_v, sum_w;
    uint32 array[14];
    uint32 i_rssi_2, i_rssi_3, i_rssi_4;
    uint32 window[16];
    uint32 start, total_time;
    osal_usecs_t  startTime, endTime;
    uint64 temp64;
    uint32 temp_a, temp_b, temp_c, rx_power;

    double temp_f;

    uint32 parm;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    loopcnt = *loop_count_ptr;

    rtk_ldd_flash_data_get(EUROPA_RX_A_ADDR, 4, &parm);
    ldd_param.rx_a = (int32) parm;
    rtk_ldd_flash_data_get(EUROPA_RX_B_ADDR, 4, &parm);
    ldd_param.rx_b = (int32) parm;
    rtk_ldd_flash_data_get(EUROPA_RX_C_ADDR, 4, &parm);
    ldd_param.rx_c = (int32) parm;

    if(TOKEN_CHAR(4,0) == 'o')
    {
        rtk_ldd_flash_data_get(EUROPA_RSSI_V0_ADDR, 4, &ldd_param.rssi_v0);
        rssi_v0 = ldd_param.rssi_v0;
    // tempest - validate rssi_v0 first
        {
           if (0==rssi_v0)
           {
               diag_util_printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
               return CPARSER_NOT_OK;
           }
        }

        sum_i = 0;
        sum_v = 0;

        for (i=0;i<loopcnt;i++)
        {
            diag_util_printf("Loop %d: \n", i);
            ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
            if(ret)
            {
                diag_util_printf("Get RX power fail!!! (%d)\n", ret);
                return CPARSER_NOT_OK;
            }
            sum_i += i_rssi;
            sum_v += v_rssi;
        }

        i_rssi = sum_i/loopcnt;
        v_rssi = sum_v/loopcnt;

        diag_util_printf("Get RX power I_RSSI = %d nA, V_RSSI = %d uV RSSI_V0 = %d\n", i_rssi, v_rssi, rssi_v0);

        ldd_param.rssi_voltage= v_rssi;
        ldd_param.rssi_i= i_rssi;

        //diag_util_printf("RX coeff: %x %x %x\n", ldd_param.rx_a, ldd_param.rx_b, ldd_param.rx_c);

    /* note that i_rrsi is nA */
#if 0
    temp_a = (ldd_param.rx_a*(i_rssi*i_rssi)/1000000)>>RX_A_SHIFT;
    temp_b = (ldd_param.rx_b*i_rssi/1000)>>RX_B_SHIFT;
    temp_c = (ldd_param.rx_c)>>RX_C_SHIFT;
    rx_power = temp_a+temp_b+temp_c;
#endif
    if(0==i_rssi || 0==ldd_param.rx_a || 0==ldd_param.rx_b || 0==ldd_param.rx_c )
    {
       diag_util_printf("Invalid RSSI current or DDMI coefficients!!!\n");
       temp_f = 0;
    }
    else
    {
           temp_f = ((double)ldd_param.rx_a/(1<<RX_A_SHIFT))*((double)i_rssi*i_rssi/1000000) + \
                    ((double)ldd_param.rx_b/(1<<RX_B_SHIFT))*((double)i_rssi/1000) + \
                    ((double)ldd_param.rx_c/(1<<RX_C_SHIFT));
    }

        rx_power = (0.0<temp_f) ? (uint32)(temp_f*1000) : 0; /* nW */

        diag_util_printf("RX power = %d nW\n", rx_power);
    }
    else if(TOKEN_CHAR(4,0) == 's')
    {
        rtk_ldd_flash_data_get(EUROPA_RSSI_V0_ADDR, 4, &ldd_param.rssi_v0);
        rssi_v0 = ldd_param.rssi_v0;
    // tempest - validate rssi_v0 first
        {
           if (0==rssi_v0)
           {
               diag_util_printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
               return CPARSER_NOT_OK;
           }
        }

        sum_i = 0;

        diag_util_printf("Get RX power for %d times, RSSI V0-1 = %d, RSSI V0-2 = %d \n", loopcnt, ldd_param.rssi_v0, ldd_param.rssi_v0_2);

        diag_util_printf("Loop    Average1    Bubble1    Average2    Bubble2\n");

        for (cnt=0;cnt<loopcnt;cnt++)
        {
            //RSSI V0 from original state, not init
            sum_i = 0;
            rssi_v0 = ldd_param.rssi_v0;
            for (i=0;i<14;i++)
            {
                ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
                if(ret)
                {
                    diag_util_printf("Get RX power fail!!! (%d)\n", ret);
                    return CPARSER_NOT_OK;
                }
                array[i] = i_rssi;
                sum_i += i_rssi;
            }
            i_rssi = sum_i/14;

            _ldd_bubble_sort(array, 14);

            sum_i = 0;
            for(i=0;i<10;i++)
            {
                sum_i +=array[i+2];
            }
            i_rssi_2 = sum_i/10;

            //RSSI V0 from init state
            rssi_v0 = ldd_param.rssi_v0_2;
            sum_i    = 0;
            for (i=0;i<14;i++)
            {
                ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi_3);
                if(ret)
                {
                    diag_util_printf("Get RX power fail!!! (%d)\n", ret);
                    return CPARSER_NOT_OK;
                }
                array[i] = i_rssi_3;
                sum_i += i_rssi_3;
            }

            i_rssi_3 = sum_i/14;

            _ldd_bubble_sort(array, 14);

            sum_i = 0;
            for(i=0;i<10;i++)
            {
                sum_i +=array[i+2];
            }
            i_rssi_4 = sum_i/10;

            diag_util_printf("%3d    %6d    %6d    %6d    %6d\n", cnt, i_rssi, i_rssi_2, i_rssi_3, i_rssi_4);
        }

    }
    else if(TOKEN_CHAR(4,0) == 'w')
    {
        rssi_v0 = ldd_param.rssi_v0;
    // tempest - validate rssi_v0 first
        {
           if (0==rssi_v0)
           {
               diag_util_printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
               return CPARSER_NOT_OK;
           }
        }

        sum_i = 0;

        diag_util_printf("Get RX power for %d times, RSSI V0 = %d\n", loopcnt, ldd_param.rssi_v0);
        diag_util_printf("RX A = %d, RX B = %d, RX C = %d\n", (int32)ldd_param.rx_a, (int32)ldd_param.rx_b, (int32)ldd_param.rx_c);

        diag_util_printf("Loop    10times    Window    \n");

        sum_w = 0;
        for (cnt=0;cnt<loopcnt;cnt++)
        {
            //RSSI V0 from original state, not init
            sum_i = 0;
            rssi_v0 = ldd_param.rssi_v0;
            osal_time_usecs_get(&startTime);
            for (i=0;i<10;i++)
            {
                ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
                if(ret)
                {
                    diag_util_printf("Get RX power fail!!! (%d)\n", ret);
                    return CPARSER_NOT_OK;
                }
                sum_i += i_rssi;
            }
            i_rssi = sum_i/10;
            if (cnt<16)
            {
                window[cnt%16] = i_rssi;
                sum_w += i_rssi;
                i_rssi_3 = sum_w/(cnt+1);
            }
            else
            {
                sum_w = sum_w - window[cnt%16] + i_rssi;
                window[cnt%16] = i_rssi;
                i_rssi_3 = sum_w>>4;
            }
            osal_time_usecs_get(&endTime);

            if (cnt<16)
            {
                window[cnt%16] = i_rssi;
                //sum_w += i_rssi;
                sum_i = 0;
                for (i=0;i<=cnt;i++)
                {
                    sum_i += window[i];
                }
                i_rssi_2 = sum_i/(cnt+1);
                //i_rssi_3 = sum_w/(cnt+1);
            }
            else
            {
                //sum_w = sum_w - window[cnt%16] + i_rssi;
                window[cnt%16] = i_rssi;
                sum_i = 0;
                for (i=0;i<16;i++)
                {
                    sum_i += window[i];
                }
                i_rssi_2 = sum_i>>4;
                //i_rssi_3 = sum_w>>4;
            }

            temp64 = ldd_param.rx_a*i_rssi_2*i_rssi_2;
            if(temp64>=0x40000000)
                temp_a = (uint32)((temp64>>20)/1000);
            else
                temp_a = 0;

            //temp64 = (ldd_param.rx_b*i_rssi_2+ldd_param.rx_c*1000)>>20;
            temp64 = (ldd_param.rx_b*i_rssi_2)>>20;
            temp_b = (uint32)temp64;
            temp64 = (ldd_param.rx_c*1000)>>20;
            temp_c = (uint32)temp64;
            rx_power = temp_a+temp_b+temp_c;

            diag_util_printf("%3d    %6d    %6d    %6d    %6d    %6d    %6d    %6d    %6d\n", cnt, i_rssi, i_rssi_2, i_rssi_3, endTime-startTime, temp_a, temp_b, temp_c, rx_power);
        }

    }


    return CPARSER_OK;
}     /* end of cparser_cmd_debug_europa_get_rx_power_original_statistic_window_loop_count */

/*
 * debug europa get rssi-v0 <UINT:Index> <UINT:loop_count>
 */
cparser_result_t
cparser_cmd_debug_europa_get_rssi_v0_Index_loop_count(
    cparser_context_t *context,
    uint32_t  *Index_ptr,
    uint32_t  *loop_count_ptr)
{
    int ret;
    uint32 rssi_v0;
    uint32 loopcnt, i;
    uint64 sum;
    uint32 index;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rssi_v0 = 0;
    loopcnt = *loop_count_ptr;
    index = *Index_ptr;
    sum = 0;

    for (i=0;i<loopcnt;i++)
    {
        //diag_util_printf("Loop %d: \n", i);
        ret = rtk_ldd_rssiV0_get(&rssi_v0);
        if(ret)
        {
            diag_util_printf("Get RSSI V0 fail!!! (%d)\n", ret);
            return CPARSER_NOT_OK;
        }
        sum += rssi_v0;
        //diag_util_printf("Sum = 0x%x, rssi V0 = 0x%x \n", sum, rssi_v0);
    }

    rssi_v0 = sum/loopcnt;

    diag_util_printf("Get RSSI V0 = %d\n", rssi_v0);

    if(index == 0)
    {
        ldd_param.rssi_v0 = rssi_v0;
        rtk_ldd_flash_data_set(EUROPA_RSSI_V0_ADDR, 4, ldd_param.rssi_v0);
    }
    else if (index == 1)
    {
        ldd_param.rssi_v0_2 = rssi_v0;
    }

    diag_util_printf("Get RSSI V0-1 = %d\n", ldd_param.rssi_v0);
    diag_util_printf("Get RSSI V0-2 = %d\n", ldd_param.rssi_v0_2);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_rssi_v0_loop_count */

/*
 * debug europa reset ( chip | digital | 8051 | analog )
 */
cparser_result_t
cparser_cmd_debug_europa_reset_chip_digital_8051_analog(
    cparser_context_t *context)
{
    int ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_i2c_init(I2C_PORT_0);
    osal_time_mdelay(100);

    if(TOKEN_CHAR(3,0) == 'c')
    {
        /*Europa Reset Total Chip*/
        diag_util_printf("Reset Total Chip\n", ret);
        ret = rtk_ldd_reset(LDD_RESET_CHIP)  ;
    }
    else if(TOKEN_CHAR(3,1) == 'd')
    {
        /*Europa Reset Digital*/
        diag_util_printf("Reset Digital\n", ret);
        ret = rtk_ldd_reset(LDD_RESET_DIGITAL)  ;
    }
    else if(TOKEN_CHAR(3,1) == '8')
    {
        /*Europa Reset 8051*/
        diag_util_printf("Reset 8051l\n", ret);
        ret = rtk_ldd_reset(LDD_RESET_8051)  ;
    }
    else if(TOKEN_CHAR(3,1) == 'a')
    {
        /*Europa Reset Analog*/
        diag_util_printf("Reset Analog\n", ret);
        ret = rtk_ldd_reset(LDD_RESET_ANALOG)  ;
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(4));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_reset_chip_digital_8051_analog */

/*
 * debug europa get rssi-voltage <UINT:loop_count>
 */
cparser_result_t
cparser_cmd_debug_europa_get_rssi_voltage_loop_count(
    cparser_context_t *context,
    uint32_t  *loop_count_ptr)
{
    int ret;
    uint32 rssi_v0;
    uint32 loopcnt, i;
    uint32 sum;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

#if 0
    rssi_v0 = 0;
    loopcnt = *loop_count_ptr;
    sum = 0;

    for (i=0;i<loopcnt;i++)
    {
        diag_util_printf("Loop %d: \n", i);
        ret = rtk_ldd_rssiVoltage_get(&rssi_v0);
        if(ret)
        {
            diag_util_printf("Get RSSI Voltage fail!!! (%d)\n", ret);
            return CPARSER_NOT_OK;
        }
        sum += rssi_v0;
    }

    rssi_v0 = sum/loopcnt;

    diag_util_printf("Get RSSI Voltage = %d\n", rssi_v0);

    ldd_param.rssi_voltage= rssi_v0;
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_rssi_voltage_loop_count */


/*
 * debug europa dump ( all | a0 | a2 | a4 | a5 | ddmi )
 */
cparser_result_t
cparser_cmd_debug_europa_dump_all_a0_a2_a4_a5_ddmi(
    cparser_context_t *context)
{

    int ret;
    uint8 *init_data, *ptr_data;
    uint32 i, length, offset;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_i2c_init(I2C_PORT_0);
    osal_time_mdelay(100);

    if(TOKEN_CHAR(3,1) == 'l')
    {
        /*Dump All*/
        diag_util_printf("Dump All!!!!!!!\n", ret);
        osal_time_mdelay(100);
        length = 0x3FF;
        offset = 0;
    }
    else if(TOKEN_CHAR(3,1) == '0')
    {
        /*Dump DDMI A0*/
        diag_util_printf("Dump DDMI A0!!!!!!!\n", ret);
        osal_time_mdelay(100);
        length = 0xFF;
        offset = 0;
    }
    else if(TOKEN_CHAR(3,1) == '2')
    {
        /*Dump DDMI A2*/
        diag_util_printf("Dump DDMI A2!!!!!!!\n", ret);
        osal_time_mdelay(100);
        length = 0xFF;
        offset = 0x100;
    }
    else if(TOKEN_CHAR(3,1) == '4')
    {
        /*Dump A4*/
        diag_util_printf("Dump A4!!!!!!!\n", ret);
        osal_time_mdelay(100);
        length = 0xFF;
        offset = 0x200;
    }
    else if(TOKEN_CHAR(3,1) == '5')
    {
        /*Dump A4*/
        diag_util_printf("Dump A5!!!!!!!\n", ret);
        osal_time_mdelay(100);
        length = 0xFF;
        offset = 0x300;
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(4));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    init_data = (uint8 *)malloc(length);
    if (!init_data)
    {
        diag_util_printf("Memory Allocation Fail!!!!!!!\n", ret);
        return CPARSER_NOT_OK;
    }
    ptr_data = init_data;
    ret = rtk_ldd_parameter_get(length, offset, ptr_data);
    diag_util_printf("\n");
    free(init_data);
    if(ret)
    {
        diag_util_printf(" Dump registers failed (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_debug_europa_dump_all_a0_a2_a4_a5_ddmi */

/*
 * debug europa set mode ( dol | dcl | scl-bias | scl-mod )
 */
cparser_result_t
cparser_cmd_debug_europa_set_mode_dol_dcl_scl_bias_scl_mod(
    cparser_context_t *context)
{
    int ret;
    uint32 loop_mode, value, tx_sd_threshold;

    DIAG_UTIL_PARAM_CHK();

    if(TOKEN_CHAR(4,0) == 'd')
    {
        if(TOKEN_CHAR(4,1) == 'o')
        {
            loop_mode = LDD_LOOP_DOL;
            diag_util_printf("Change to DOL mode\n");
        }
        else if(TOKEN_CHAR(4,1) == 'c')
        {
            loop_mode = LDD_LOOP_DCL;
            diag_util_printf("Change to DCL mode\n");
        }
        else
        {
            diag_util_printf("no such mode %s\n", TOKEN_STR(3));
            return CPARSER_ERR_INVALID_PARAMS;
        }
    }
    else if(TOKEN_CHAR(4,0) == 's')
    {
        if(TOKEN_CHAR(4,4) == 'b')
        {
            loop_mode = LDD_LOOP_SCL_BIAS;
            diag_util_printf("Change to SCL Bias mode\n");
        }
        else if(TOKEN_CHAR(4,4) == 'm')
        {
            loop_mode = LDD_LOOP_SCL_MOD;
            diag_util_printf("Change to SCL Modulation mode\n");
        }
        else
        {
            diag_util_printf("no such mode %s\n", TOKEN_STR(4));
            return CPARSER_ERR_INVALID_PARAMS;
        }
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(3));
        return CPARSER_ERR_INVALID_PARAMS;
    }

     rtk_ldd_loopMode_set(loop_mode);

     //TX SD Threshold
    if (loop_mode == LDD_LOOP_DOL)
    {
        /*REG_R33 -> REG_W54   for bias current*/
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x55, 33, &value), ret);
        diag_util_printf("Current Bias R33 = 0x%x, Set REG_R33 -> REG_W54\n", value);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 54, value), ret);
        /*REG_R32 -> REG_W55   for modulation current*/
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x55, 32, &value), ret);
        diag_util_printf("Current Modulation R32 = 0x%x, Set REG_R32 -> REG_W55\n", value);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 55, value), ret);

        /* Load BIAS/MOD initial code, Write REG_W70[7] to 1 and than write to 0 */
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0xbf), ret);
        osal_time_mdelay(100);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0x3f), ret);

        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 57, &value), ret);
        if(value&0x08)
            tx_sd_threshold = 0x10;
        else
            tx_sd_threshold = 0x20;
    }
    else if (loop_mode == LDD_LOOP_DCL)
    {
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 58, &value), ret);
        tx_sd_threshold = value/2;
    }
    else
    {
        if (loop_mode == LDD_LOOP_SCL_BIAS)
        {
            /*REG_R32 -> REG_W55   for modulation current*/
            DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x55, 32, &value), ret);
            diag_util_printf("Current Modulation R32 = 0x%x, Set REG_R32 -> REG_W55\n", value);
            DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 55, value), ret);
        }
        else
        {
            /*REG_R33 -> REG_W54   for bias current*/
            DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x55, 33, &value), ret);
            diag_util_printf("Current Bias R33 = 0x%x, Set REG_R33 -> REG_W54\n", value);
            DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 54, value), ret);
        }
        /* Load BIAS/MOD initial code, Write REG_W70[7] to 1 and than write to 0 */
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0xbf), ret);
        osal_time_mdelay(100);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0x3f), ret);

        DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 61, &value), ret);
        tx_sd_threshold = value/10;
    }
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 62, tx_sd_threshold), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_mode_dol_dcl_scl_bias_scl_mod */

/*
 * debug europa get mode
 */
cparser_result_t
cparser_cmd_debug_europa_get_mode(
    cparser_context_t *context)
{
    int ret;
    uint32 loop_mode, value;

    DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 69, &value), ret);
    loop_mode = value &0x3;

    if(loop_mode == LDD_LOOP_DOL)
    {

        diag_util_printf("loop Mode is DOL mode\n");
    }
    else if(loop_mode == LDD_LOOP_DCL)
    {

        diag_util_printf("loop Mode is DCL mode\n");
    }
    else if(loop_mode == LDD_LOOP_SCL_BIAS)
    {

        diag_util_printf("loop Mode is SCL Bias mode\n");
    }
    else if(loop_mode == LDD_LOOP_SCL_MOD)
    {

        diag_util_printf("loop Mode is SCL Modulation mode\n");
    }
    else
    {
        diag_util_printf("no such mode\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_mode */

/*
 * debug europa get mpd0 <UINT:loop_count>
 */
cparser_result_t
cparser_cmd_debug_europa_get_mpd0_loop_count(
    cparser_context_t *context,
    uint32_t  *loop_count_ptr)
{
    int ret;
    uint32 mpd0;
    uint16 loopcnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mpd0 = 0;
    loopcnt = *loop_count_ptr;

    ret = rtk_ldd_mpd0_get(loopcnt, &mpd0);
    if(ret)
    {
        diag_util_printf("Get MPD0 fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("MPD0 = 0x%x\n", mpd0);

    ldd_param.mpd0 = mpd0;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_mpd0_loop_count */

/*
 * debug europa get vbr <UINT:init> <UINT:max> <UINT:loop_count> <UINT:threshold>
 */
cparser_result_t
cparser_cmd_debug_europa_get_vbr_init_max_loop_count_threshold(
    cparser_context_t *context,
    uint32_t  *init_ptr,
    uint32_t  *max_ptr,
    uint32_t  *loop_count_ptr,
    uint32_t  *threshold_ptr)
{
    int ret;
    uint32 rssi_v0, i_rssi, v_rssi;
    uint32 loopcnt, i, cnt1, cnt2;
    uint64 sum_i, sum_v, sum_w;
    uint32 init_value, max_value;
    uint32 start, total_time;
    osal_usecs_t  startTime, endTime;
    uint32 apd_value;

    uint8 orig_apd_value; // to restore orig val after test
    uint32 Vbr; // to calculate Vbr value in mV
    uint32 threshold;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    rtk_ldd_flash_data_get(EUROPA_RSSI_V0_ADDR, 4, &ldd_param.rssi_v0);
    rssi_v0 = ldd_param.rssi_v0;
    // tempest - validate rssi_v0 first
    {
        if (0==rssi_v0)
        {
            diag_util_printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
            return CPARSER_NOT_OK;
        }
    }

    init_value = *init_ptr;
    max_value = *max_ptr;
    loopcnt = *loop_count_ptr;
    threshold = *threshold_ptr;
    i_rssi = 0;
    apd_value = init_value;

    // get orig A4/100, restore the value after Vbr test
    //DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 100, &orig_apd_value), ret);
    rtk_ldd_parameter_get(1, EUROPA_REG_DAC_HB_ADDR, &orig_apd_value);

    diag_util_printf("Break Down Test Start with  APD value = 0x%x (orig=0x%x), loopcnt  = %d\n", apd_value, orig_apd_value, loopcnt);

    while((i_rssi<threshold)&&(apd_value<=max_value))
    {
        sum_i = 0;
        cnt1 = loopcnt;
        cnt2 = 0;

        DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 100, apd_value), ret);
        while ((cnt1>0) &&(cnt2<loopcnt))
        {
            ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
            if(ret)
            {
                diag_util_printf("Get RX power fail!!! (%d)\n", ret);
                return CPARSER_NOT_OK;
            }
            if (v_rssi>rssi_v0)
            {
                cnt1--;
                sum_i += i_rssi;
            }
         else
         {
                cnt2++;
         }
        }

        //diag_util_printf("Break Down Test: APD value = 0x%x, cnt1 = %d, cnt2  = %d\n", apd_value, cnt1, cnt2);
        if (cnt2>=loopcnt)
        {
            apd_value++;
            diag_util_printf("Break Down Test: APD value = 0x%x, RSSI Voltage < RSSI V0\n", apd_value);
        }
        else if (cnt1==0)
        {
            i_rssi = sum_i/loopcnt;
            diag_util_printf("Break Down Test: APD value = 0x%x, I RSSI  = %d\n", apd_value, i_rssi);
            if (i_rssi > threshold)
            {
                break;
            }
         else
         {
                apd_value++;
         }
        }
        else
        {
            diag_util_printf("Break Down Tes Error!!!!\n");
            break;
        }
    }

    diag_util_printf("restore orig APD value = 0x%x\n", orig_apd_value);
    //DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 100, orig_apd_value), ret);
    rtk_ldd_parameter_set(1, EUROPA_REG_DAC_HB_ADDR, &orig_apd_value);
    // calculate Vbr value in mV
    // tempest note
    //  Vout = Vref * ( 1 + Rfb1 / Rfb2) + Rfb1 * Idac
    //    Vref=1.2V  Rfb1=200k  Rfb2=10k
    //    Idac = 12uA + DAC_CODE * 0.78125uA
    Vbr = (2760000 + (15625 * apd_value)) / 100;

    diag_util_printf("Break Down Test Result: APD value = 0x%x, I RSSI = %d  Vbr=%d (mV)\n", apd_value, i_rssi, Vbr);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_vbr_init_max_loop_count_threshold */

/*
 * debug europa set ddmi ( temperature | vcc | tx-power | rx-power | bias | mod )
 */
cparser_result_t
cparser_cmd_debug_europa_set_ddmi_temperature_vcc_tx_power_rx_power_bias_mod(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_ddmi_temperature_vcc_tx_power_rx_power_bias_mod */

/*
 * debug europa get tx-bias
 */
cparser_result_t
cparser_cmd_debug_europa_get_tx_bias(
    cparser_context_t *context)
{
    int ret;
    uint32 bias;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    bias = 0;

    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        diag_util_printf("Get TX Bias Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("TX Bias = 0x%x(2uA), %duA\n", bias, bias*2);

    ldd_param.bias = bias;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_tx_bias */

/*
 * debug europa get tx-mod
 */
cparser_result_t
cparser_cmd_debug_europa_get_tx_mod(
    cparser_context_t *context)
{
    int ret;
    uint32 mod;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mod = 0;

    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        diag_util_printf("Get TX Modulation Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("TX Modulation = 0x%x(2uA), %duA\n", mod, mod*2);

    ldd_param.modulation = mod;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_tx_mod */

/*
 * debug europa get temperature
 */
cparser_result_t
cparser_cmd_debug_europa_get_temperature(
    cparser_context_t *context)
{
    int ret;
    uint16 tempK;
    uint32 parm;

    int32 tempC;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    tempK = 0;

    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        diag_util_printf("Get Temperature Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    //diag_util_printf("Temperature = %dK\n", temp);
    tempC = (tempK*100-27315)/100; /* degK to degC */
    printf("Temperature=%dK (%dC)\n", tempK, tempC);

    //rtk_ldd_flash_data_get(EUROPA_TEMP_OFFSET_ADDR, 1, &ldd_param.temp_offset);
    rtk_ldd_flash_data_get(EUROPA_TEMP_OFFSET_ADDR, 1, &parm);
    ldd_param.temp_offset = (int8) parm;

    tempC = tempC - ldd_param.temp_offset;
    diag_util_printf("BOSA Temperature = %dC (offset=%d)\n",
              tempC, ldd_param.temp_offset);

    ldd_param.temperature = tempK;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_temperature */

/*
 * debug europa get vdd
 */
cparser_result_t
cparser_cmd_debug_europa_get_vdd(
    cparser_context_t *context)
{
    int ret;
    uint32 vdd;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    vdd = 0;

    ret = rtk_ldd_vdd_get(&vdd);
    if(ret)
    {
        diag_util_printf("Get VDD Voltage fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("VDD Voltage = %d uV\n", vdd);

    ldd_param.vdd= vdd;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_vdd */

/*
 * debug europa set rx-param ( a | b | c ) <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_europa_set_rx_param_a_b_c_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    uint32 addr, para;

    DIAG_UTIL_PARAM_CHK();
    para = *value_ptr;

    if(TOKEN_CHAR(4,0) == 'a')
    {
        addr = EUROPA_RX_A_ADDR;
        ldd_param.rx_a = para;
    }
    else if(TOKEN_CHAR(4,0) == 'b')
    {
        addr = EUROPA_RX_B_ADDR;
        ldd_param.rx_b = para;
    }
    else if(TOKEN_CHAR(4,0) == 'c')
    {
        addr = EUROPA_RX_C_ADDR;
        ldd_param.rx_c = para;
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(4));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    rtk_ldd_flash_data_set(addr, 4, para);

    diag_util_printf("RX parameter a = %d\n",  ldd_param.rx_a);
    diag_util_printf("RX parameter b = %d\n",  ldd_param.rx_b);
    diag_util_printf("RX parameter c = %d\n",  ldd_param.rx_c);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_rx_param_a_b_c_value */

/*
 * debug europa get rx-param
 */
cparser_result_t
cparser_cmd_debug_europa_get_rx_param(
    cparser_context_t *context)
{
    uint32 addr, para1;

    addr = EUROPA_RX_A_ADDR;
    rtk_ldd_flash_data_get(addr, 4, &para1);
    diag_util_printf("RX parameter a: flash = %d, global = %d\n",  para1, ldd_param.rx_a);
    addr = EUROPA_RX_B_ADDR;
    rtk_ldd_flash_data_get(addr, 4, &para1);
    diag_util_printf("RX parameter b: flash = %d, global = %d\n", para1, ldd_param.rx_b);
    addr = EUROPA_RX_C_ADDR;
    rtk_ldd_flash_data_get(addr, 4, &para1);
    diag_util_printf("RX parameter c: flash = %d, global = %d\n", para1, ldd_param.rx_c);

        return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_rx_param */

/*
 * debug europa set tx-param ( a | b | c ) <INT:value>
 */
cparser_result_t
cparser_cmd_debug_europa_set_tx_param_a_b_c_value(
    cparser_context_t *context,
    int32_t  *value_ptr)
{
    uint32 addr;
    int32 para;

    DIAG_UTIL_PARAM_CHK();
    para = *value_ptr;

    if(TOKEN_CHAR(4,0) == 'a')
    {
        addr = EUROPA_TX_A_ADDR;
        ldd_param.tx_a = para;
    }
    else if(TOKEN_CHAR(4,0) == 'b')
    {
        addr = EUROPA_TX_B_ADDR;
        ldd_param.tx_b = para;
    }
    else if(TOKEN_CHAR(4,0) == 'c')
    {
        addr = EUROPA_TX_C_ADDR;
        ldd_param.tx_c = para;
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(4));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    rtk_ldd_flash_data_set(addr, 4, para);

    diag_util_printf("TX parameter a = %d\n",  ldd_param.tx_a);
    diag_util_printf("TX parameter b = %d\n",  ldd_param.tx_b);
    diag_util_printf("TX parameter c = %d\n",  ldd_param.tx_c);


    return CPARSER_OK;
}   /* end of cparser_cmd_debug_europa_set_tx_param_a_b_c_value */

/*
 * debug europa get tx-param
 */
cparser_result_t
cparser_cmd_debug_europa_get_tx_param(
    cparser_context_t *context)
{
    uint32 addr;
    int32 para1;

    addr = EUROPA_TX_A_ADDR;
    rtk_ldd_flash_data_get(addr, 4, &para1);
    diag_util_printf("Europa TX parameter a: flash = %d, global = %d\n", para1, ldd_param.tx_a);
    addr = EUROPA_TX_B_ADDR;
    rtk_ldd_flash_data_get(addr, 4, &para1);
    diag_util_printf("Europa TX parameter b: flash = %d, global = %d\n", para1, ldd_param.tx_b);
    addr = EUROPA_TX_C_ADDR;
    rtk_ldd_flash_data_get(addr, 4, &para1);
    diag_util_printf("Europa TX parameter c: flash = %d, global = %d\n", para1, ldd_param.tx_c);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_tx_param */

/*
 * debug europa set flash <UINT:addr> <UINT:len> <UINT:data>
 */
cparser_result_t
cparser_cmd_debug_europa_set_flash_addr_len_data(
    cparser_context_t *context,
    uint32_t  *addr_ptr,
    uint32_t  *len_ptr,
    uint32_t  *data_ptr)
{
    uint32 addr, len, data;
#if 0
    FILE *fp;
    uint8 *init_data, *ptr_data;

    DIAG_UTIL_PARAM_CHK();

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);

    fp = fopen("/var/config/europa.data","wb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return CPARSER_OK;
    }
    fseek(fp, 0, SEEK_SET);
    memset(init_data, 0x0, sizeof(uint8)*EUROPA_PARAMETER_SIZE);
    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
    ptr_data = init_data;

    addr = *addr_ptr;
    len   = *len_ptr;
    data = *data_ptr;

    ptr_data = ptr_data + addr;
    _ldd_short_data_set(ptr_data, len, data);

    fseek(fp, 0, SEEK_SET);
    fwrite(init_data,sizeof(char), EUROPA_PARAMETER_SIZE,fp);
    fclose(fp);
    free(init_data);
#endif
    DIAG_UTIL_PARAM_CHK();
    addr = *addr_ptr;
    len   = *len_ptr;
    data = *data_ptr;

    rtk_ldd_flash_data_set(addr, len, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_flash_addr_len_data */

/*
 * debug europa get flash <UINT:addr> <UINT:len>
 */
cparser_result_t
cparser_cmd_debug_europa_get_flash_addr_len(
    cparser_context_t *context,
    uint32_t  *addr_ptr,
    uint32_t  *len_ptr)
{
    uint32 addr, len, data;
#if 0
    FILE *fp;
    uint8 *init_data, *ptr_data;

    DIAG_UTIL_PARAM_CHK();

    init_data = (uint8 *)malloc(EUROPA_PARAMETER_SIZE);

    fp = fopen("/var/config/europa.data","rb");
    if (NULL ==fp)
    {
        printf("Open file in /var/config/ error!!!!!!!\n");
        return CPARSER_OK;
    }

    fseek(fp, 0, SEEK_SET);
    memset(init_data, 0x0, sizeof(uint8)*EUROPA_PARAMETER_SIZE);
    fread(init_data, 1, EUROPA_PARAMETER_SIZE, fp);
    ptr_data = init_data;
    addr = *addr_ptr;
    len   = *len_ptr;

    ptr_data = ptr_data + addr;
    _ldd_short_data_get(ptr_data, len, &data);

    free(init_data);
    fclose(fp);
    diag_util_printf("rtk_ldd_flash_data_get: addr = %d, len = %d, data = 0x%x\n", addr, len, data);
#endif

    DIAG_UTIL_PARAM_CHK();
    addr = *addr_ptr;
    len   = *len_ptr;
    rtk_ldd_flash_data_get(addr, len, &data);

    diag_util_printf("rtk_ldd_flash_data_get: addr = %d, len = %d, data = 0x%x\n", addr, len, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_flash_addr_len */

/*
 * p
 * debug europa set apdlut ( factor | offset ) <UINT:rfb1> <UINT:rfb2> <UINT:vbr> <UINT:temp> <UINT:value> <UINT:c1> <UINT:c2>
 */
cparser_result_t
cparser_cmd_debug_europa_set_apdlut_factor_offset_rfb1_rfb2_vbr_temp_value_c1_c2(
    cparser_context_t *context,
    uint32_t  *rfb1_ptr,
    uint32_t  *rfb2_ptr,
    uint32_t  *vbr_ptr,
    uint32_t  *temp_ptr,
    uint32_t  *value_ptr,
    uint32_t  *c1_ptr,
    uint32_t  *c2_ptr)
{
    uint8 lut[151];
    uint32 factor, offset;
    uint32 vbr;
    uint32 dac, dac2;
    uint32 temp;
    int32 pos, pos2, i, j, num;
    uint32 slope;
    FILE *fp;
    uint8 *init_data;
    uint32 rfb1, rfb2;
    uint64 vapd;

    uint32 v;

    DIAG_UTIL_PARAM_CHK();

    vbr = *vbr_ptr;
    temp = *temp_ptr;
    rfb1 = *rfb1_ptr;
    rfb2 = *rfb2_ptr;

    if(TOKEN_CHAR(4,0) == 'f')
    {
        factor = *value_ptr;
        vapd = vbr*factor/1000;
    }
    else if(TOKEN_CHAR(4,0) == 'o')
    {
        offset = *value_ptr;
        vapd = vbr-offset; /* offset is in mV also */
    }
    else
    {
        diag_util_printf("no such mode %s\n", TOKEN_STR(4));
        return CPARSER_ERR_INVALID_PARAMS;
    }

    //dac = (vapd -27600000)/156250;
    //Vout  = 1200 + 1200 * (Rfb1/Rfb2) + Rfb1 * 12 + (Rfb1 * DAC_CODE * 78125) / 100000
    dac = (vapd - 1200 - 1200*rfb1/rfb2 - rfb1*12)*100000/78125/rfb1;

    if (dac>255)
    {
        diag_util_printf("DAC over 255!!!\n");
        return CPARSER_ERR_INVALID_PARAMS;
    }

    pos = temp + 40;
    lut[pos] = dac;

    //For -40~temp
    pos2 = pos -1;
    slope= *c1_ptr;
    while(pos2>=0)
    {
        //dac2 = dac - (pos-pos2)*slope/1000;
        v = _ldd_booster_Dac2Vout(dac, rfb1, rfb2);
    v = v - (pos-pos2)*slope;
    dac2 = _ldd_booster_Vout2Dac(v, rfb1, rfb2);

        if (dac2>0)
        {
           lut[pos2] = dac2;
        }
        else
        {
           lut[pos2] = 0;
        }
     pos2--;
    }

    //For temp~110
    pos2 = pos + 1;
    slope= *c2_ptr;
    while(pos2<=151)
    {
        //dac2 = dac + (pos2-pos)*slope/1000;
        v = _ldd_booster_Dac2Vout(dac, rfb1, rfb2);
    v = v + (pos2-pos)*slope;
    dac2 = _ldd_booster_Vout2Dac(v, rfb1, rfb2);

        if (dac2>255)
        {
           lut[pos2] = 255;
        }
        else
        {
           lut[pos2] = dac2;
        }
     pos2++;
    }

    diag_util_printf("Temperature  &  DAC value\n ");

    for (i=0;i<10;i++)
    {
        if (i<9)
    {
            num =16;
        }
        else
        {
            num = 7;
        }

        printf("Temperature: ");
        for(j=0;j<num;j++)
        {
            printf("%4d ", j+i*16-40);
        }
        printf("\n ");
        printf("DAC:          ");
        for(j=0;j<num;j++)
        {
            printf("0x%02x ", lut[j+i*16]);
        }
        printf("\n\n ");
    }

    rtk_ldd_flash_array_set(EUROPA_APD_LUT_ADDR, 151, lut);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_apdlut_factor_offset_rfb1_rfb2_vbr_temp_value_c1_c2 */

/*
 * debug europa get apdlut
 */
cparser_result_t
cparser_cmd_debug_europa_get_apdlut(
    cparser_context_t *context)
{
    uint8 lut[151];
    int32  i, j, num;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_flash_array_get(EUROPA_APD_LUT_ADDR, 151, lut);

    diag_util_printf("Temperature  &  DAC value\n ");

    for (i=0;i<10;i++)
    {
        if (i<9)
        {
            num =16;
        }
        else
        {
            num = 7;
        }

        printf("Temperature: ");
        for(j=0;j<num;j++)
        {
            printf("%4d ", j+i*16-40);
        }
        printf("\n ");
        printf("DAC:          ");
        for(j=0;j<num;j++)
        {
            printf("0x%02x ", lut[j+i*16]);
        }
        printf("\n\n ");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_apdlut */

/*
 * debug europa set bias <UINT:current>
 */
cparser_result_t
cparser_cmd_debug_europa_set_bias_current(
    cparser_context_t *context,
    double *current_ptr)
{
    int ret;
    double curr;
    uint32_t Ibias;
#if 0
    uint32_t w54, w56;
#endif

    DIAG_UTIL_PARAM_CHK();

    curr = *current_ptr;
    if (80.0 < curr)
    {
        diag_util_printf("Invalid bias current %f\n", curr);
        return CPARSER_ERR_INVALID_PARAMS;
    }

    Ibias = (uint32_t)(curr * 1000 / 2); /* convert mA to 2uA unit */

#if 0
// FIXME : move detail to rtl8290_tx_bias_set
    Ibias = Ibias * 4096 / 80;

    DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 56, &w56), ret);

    w54 = (Ibias & 0xff0)>>4;
    w56 = ((w56 & 0xf0)) | (Ibias & 0x00f);

    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 54, w54), ret);
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 56, w56), ret);

    /* Load BIAS/MOD code, Write REG_W70[7] to 1 and than write to 0 */
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0xbf), ret);
    osal_time_mdelay(100);
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0x3f), ret);
#endif

    ret = rtk_ldd_tx_bias_set(Ibias);
    if(ret)
    {
        diag_util_printf("Set TX Bias Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_bias_current */


/*
 * debug europa set mod <UINT:current>
 */
cparser_result_t
cparser_cmd_debug_europa_set_mod_current(
    cparser_context_t *context,
    double *current_ptr)
{
    int ret;
    double curr;
    uint32_t Imod;
#if 0
    uint32_t w55, w56;
#endif

    DIAG_UTIL_PARAM_CHK();

    curr = *current_ptr;
    if (100.0 < curr)
    {
        diag_util_printf("Invalid mod current %f\n", curr);
        return CPARSER_ERR_INVALID_PARAMS;
    }

    Imod = (uint32_t)(curr * 1000 / 2); /* convert mA to 2uA unit */

#if 0
// FIXME : move detail to rtl8290_tx_mod_set
    Imod = Imod * 4096 / 100;

    DIAG_UTIL_ERR_CHK(rtk_i2c_read(EUROPA_I2C_PORT, 0x54, 56, &w56), ret);

    w55 = (Imod & 0xff0)>>4;
    w56 = ((w56 & 0x0f)) | ((Imod & 0x00f)<<4);

    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 55, w55), ret);
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 56, w56), ret);

    /* Load BIAS/MOD code, Write REG_W70[7] to 1 and than write to 0 */
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0xbf), ret);
    osal_time_mdelay(100);
    DIAG_UTIL_ERR_CHK(rtk_i2c_write(EUROPA_I2C_PORT, 0x54, 70, 0x3f), ret);
#endif

    ret = rtk_ldd_tx_mod_set(Imod);
    if(ret)
    {
        diag_util_printf("Set TX Mod Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_mod_current */


/*
 * debug europa save all
 */
cparser_result_t
cparser_cmd_debug_europa_save_all(
   cparser_context_t *context)
{
   DIAG_UTIL_PARAM_CHK();

   diag_util_printf("\n!! Command obsoleted !!\nUse 'europa cal save' in the future.\n");

   _ldd_cal_save();

   return CPARSER_OK;

#if 0
   uint8 reg;

   DIAG_UTIL_PARAM_CHK();

   diag_util_printf("Saving all parameters...\n");
   /* MPD0 */
   rtk_ldd_flash_data_set(EUROPA_MPD0_ADDR, 4, ldd_param.mpd0);

   /* RSSI-V0*/
   rtk_ldd_flash_data_set(EUROPA_RSSI_V0_ADDR, 4, ldd_param.rssi_v0);


   // NOTE: store parameters in ldd_param.xx, but not reg

   /* TX - P0_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W58_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_DCL_P0_ADDR, 1, reg);

   /* TX - P1_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W59_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_DCL_P1_ADDR, 1, reg);

   /* TX - Pavg_target */
   rtk_ldd_parameter_get(1, EUROPA_REG_W61_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_DCL_PAVG_ADDR, 1, reg);

   /* TX - DAC scale */
   rtk_ldd_parameter_get(1, EUROPA_REG_W57_ADDR, &reg);
   reg = (reg>>EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET) & 0x01;
   reg = reg ^ 1; // in flash, 1=FULL SCALE, but in chip, 1=HALF SCALE
   rtk_ldd_flash_data_set(EUROPA_W57_APCDIG_DAC_BIAS_ADDR, 1, reg);

   /* TX - Crossing */
   rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &reg);
   rtk_ldd_flash_data_set(EUROPA_CROSS_ADDR, 1, reg);


   /* TX DDMI - slope & offset */
   rtk_ldd_flash_data_set(EUROPA_TX_A_ADDR, 4, (uint32)ldd_param.tx_a);
   rtk_ldd_flash_data_set(EUROPA_TX_B_ADDR, 4, (uint32)ldd_param.tx_b);
   rtk_ldd_flash_data_set(EUROPA_TX_C_ADDR, 4, (uint32)ldd_param.tx_c);


   /* RX DDMI - slope & offset */
   rtk_ldd_flash_data_set(EUROPA_RX_A_ADDR, 4, (uint32)ldd_param.rx_a);
   rtk_ldd_flash_data_set(EUROPA_RX_B_ADDR, 4, (uint32)ldd_param.rx_b);
   rtk_ldd_flash_data_set(EUROPA_RX_C_ADDR, 4, (uint32)ldd_param.rx_c);


   /* DDMI - Temperature Offset */
   rtk_ldd_flash_data_set(EUROPA_TEMP_OFFSET_ADDR, 1, (uint32)ldd_param.temp_offset);

   /* Temperature interrupt scale */
   rtk_ldd_flash_data_set(EUROPA_TEMP_INTR_SCALE_ADDR, 1, ldd_param.temp_intr_scale);


   /* RX SD - LOS assert/de-assert threshold */
   rtk_ldd_flash_data_set(EUROPA_RX_TH_ADDR, 1, ldd_param.rx_th);
   rtk_ldd_flash_data_set(EUROPA_RX_DETH_ADDR, 1, ldd_param.rx_deth);

   diag_util_printf("done.\n");

   return CPARSER_OK;
#endif
}   /* end of cparser_cmd_debug_europa_save_all */


/*
 * debug europa set calibration state ( disable | enable )
 */
cparser_result_t
cparser_cmd_debug_europa_set_calibration_state_disable_enable(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(5,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_ldd_calibration_state_set(ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_ldd_calibration_state_set(DISABLED), ret);
    }
    else {}

    return CPARSER_OK;
} /* end of cparser_cmd_debug_europa_set_calibration_state_disable_enable */

/*
 * debug europa cal qreg <UINT:x1> <UINT:y1> <UINT:x2> <UINT:y2> <UINT:x3> <UINT :y3>
 */
cparser_result_t
cparser_cmd_debug_europa_cal_qreg_x1_y1_x2_y2_x3_y3(
    cparser_context_t *context,
    uint32_t  *x1_ptr,
    uint32_t  *y1_ptr,
    uint32_t  *x2_ptr,
    uint32_t  *y2_ptr,
    uint32_t  *x3_ptr,
    uint32_t  *y3_ptr)
{
#if 0
    uint32 temp;
    double x1, x2, x3, y1, y2, y3;
    double a, b, c;

    DIAG_UTIL_PARAM_CHK();

    temp = *x1_ptr;
    x1 = (double)temp;
    temp = *y1_ptr;
    y1 = (double)temp;
    temp = *x2_ptr;
    x2 = (double)temp;
    temp = *y2_ptr;
    y2 = (double)temp;
    temp = *x3_ptr;
    x3= (double)temp;
    temp = *y3_ptr;
    y3 = (double)temp;

    a = (((y1*x2 - y2*x1)/(x1 - x2)) - ((y2*x3 - y3*x2)/(x2 - x3)) )/(x1*x2 - x2*x3);
    c = (((y1*x2 - y2*x1)/(x1 - x2)*x3) - ((y2*x3 - y3*x2)/(x2 - x3)*x1))/(x1 - x3);
    b = (y1 - a*x1*x1-c)/x1;
#endif

    double a, b, c;

    DIAG_UTIL_PARAM_CHK();

    _ldd_quad_regression(*x1_ptr, *y1_ptr, *x2_ptr, *y2_ptr, *x3_ptr, *y3_ptr,
             &a, &b, &c);

    diag_util_printf("---- regression test ----\n");
    diag_util_printf("  parameter a = %lf\n", a);
    diag_util_printf("  parameter b = %lf\n", b);
    diag_util_printf("  parameter c = %lf\n", c);

    return CPARSER_OK;
}   /* end of cparser_cmd_debug_europa_cal_qreg_x1_y1_x2_y2_x3_y3 */

/*
 * debug europa cal ( p0 | p1 | pavg | dacfull ) <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_europa_cal_p0_p1_pavg_dacfull_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret;
    uint8 value;
    DIAG_UTIL_PARAM_CHK();


    /* the "value" parameter did not exist, get current setting */
    if(4 == TOKEN_NUM())
    {
       if(0 == strcmp(TOKEN_STR(3), "p0"))
       {
          rtk_ldd_parameter_get(1, EUROPA_REG_W58_ADDR, &value);
          diag_util_printf("MPD0 target=0x%02x\n", value);
       }
       else if(0 == strcmp(TOKEN_STR(3), "p1"))
       {
          rtk_ldd_parameter_get(1, EUROPA_REG_W59_ADDR, &value);
          diag_util_printf("MPD1 target=0x%02x\n", value);
       }
       else if(0 == strcmp(TOKEN_STR(3), "pavg"))
       {
          rtk_ldd_parameter_get(1, EUROPA_REG_W61_ADDR, &value);
          diag_util_printf("MPDavg target=0x%02x\n", value);
       }
       else if(0 == strcmp(TOKEN_STR(3), "dacfull"))
       {
          rtk_ldd_parameter_get(1, EUROPA_REG_W57_ADDR, &value);
          value = value & EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_MASK;

          diag_util_printf("DAC Scale=%s\n", value ? "Half" : "Full");
       }

       return CPARSER_OK;
    }


    /* config the setting by "value" parameter */

    value = (uint8)*value_ptr;

    if(0 == strcmp(TOKEN_STR(3), "p0"))
    {
       rtk_ldd_parameter_set(1, EUROPA_REG_W58_ADDR, &value);
    }
    else if(0 == strcmp(TOKEN_STR(3), "p1"))
    {
       rtk_ldd_parameter_set(1, EUROPA_REG_W59_ADDR, &value);
    }
    else if(0 == strcmp(TOKEN_STR(3), "pavg"))
    {
       rtk_ldd_parameter_set(1, EUROPA_REG_W61_ADDR, &value);
    }
    else if(0 == strcmp(TOKEN_STR(3), "dacfull"))
    {
       uint8 w57;

       rtk_ldd_parameter_get(1, EUROPA_REG_W57_ADDR, &w57);

       if (1==value)
       {
      /* full scale, clear DAC_BIAS_HALF bit */
          w57 = w57 & (0xff ^ (0x01<<EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET));
          diag_util_printf("Set Full Scale\n");
       }
       else if (0==value)
       {
      /* half scale, set DAC_BIAS_HALF bit */
          w57 = w57 | (0x01<<EUROPA_REG_W57_APCDIG_DAC_BIAS_HALF_OFFSET);
          diag_util_printf("Set Half Scale\n");
       }
       else
       {
          diag_util_printf("Invalid setting.\n");
       }

       rtk_ldd_parameter_set(1, EUROPA_REG_W57_ADDR, &w57);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_p0_p1_pavg_cross_dacfull_value */


/*
 * debug europa cal toffset <UINT:temp> { <INT:offset> }
 */
cparser_result_t
cparser_cmd_debug_europa_cal_toffset_temp_offset(
    cparser_context_t *context,
    int32_t  *temp_ptr,
    int32_t  *offset_ptr)
{
    int32 ret;
    int8 offset;

    DIAG_UTIL_PARAM_CHK();

    if(6 == TOKEN_NUM())
    {
       /* optional parameter means assign offset directly */
       diag_util_printf("Ignore temperature input.\n");
       offset = (int8) ((*offset_ptr) & 0xff);
    }
    else
    {
       uint16 tempK;
       int32 tempC;

       ret = rtk_ldd_temperature_get(&tempK);
       if(ret)
       {
           diag_util_printf("Get Temperature Fail!!! (%d)\n", ret);
           return CPARSER_NOT_OK;
       }

       tempC = (tempK*100-27315)/100; /* degK to degC */
       printf("Temperature=%dK (%dC)\n", tempK, tempC);

       offset = (int8)(tempC - *temp_ptr);
    }

    ldd_param.temp_offset = offset;
    diag_util_printf("Temperature offset=%d.\n", offset);

    // FIXME save to flash immediately
    rtk_ldd_flash_data_set(EUROPA_TEMP_OFFSET_ADDR, 1, (uint32)ldd_param.temp_offset);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_toffset_temp_offset */

/*
 * debug europa cal tscale <UINT:scale>
 */
cparser_result_t
cparser_cmd_debug_europa_cal_tscale_scale(
    cparser_context_t *context,
    uint32_t  *scale_ptr)
{
    int32 ret;
    uint8 temp_scale;
    DIAG_UTIL_PARAM_CHK();

    temp_scale = (uint8) (*scale_ptr & 0xff);

    if (0<temp_scale && 8>temp_scale)
    {
       rtk_ldd_parameter_set(1, EUROPA_REG_TEMP_INT_OFFSET_ADDR, &temp_scale);

       ldd_param.temp_intr_scale = temp_scale;

       diag_util_printf("Temperature scale=%d.\n", temp_scale);
       return CPARSER_OK;
    }
    else
    {
       diag_util_printf("Invalid temperature scale=%d.\n", temp_scale);
       return CPARSER_ERR_INVALID_PARAMS;
    }

}    /* end of cparser_cmd_debug_europa_cal_tscale_scale */


/*
 * debug europa cal cross  ( up | down | disable | show) <UINT:level>
   // setup crossing point adjustment +
   //    W48[5]:  TX_OFFSET_MOD_EN, 1 enable
   //    W48[4]:  TX_OFFSET_MOD_SIGN, 0 for up (lift crossing point)
   //    W48[3:0]:  TX_OFFSET_MOD_STR: 0 ~ F, strength
 */
cparser_result_t
cparser_cmd_debug_europa_cal_cross_up_down_disable_show_level(
            cparser_context_t *context,
                uint32_t  *level_ptr)
{
    uint8 w48, cross_val, cross_mask;
    uint8 level;

    DIAG_UTIL_PARAM_CHK();

    cross_val = 0;
    cross_mask = (EUROPA_REG_W48_TX_OFFSET_MOD_EN_MASK |
              EUROPA_REG_W48_TX_OFFSET_MOD_SIGN_MASK |
          EUROPA_REG_W48_TX_OFFSET_MOD_STR_MASK);

    rtk_ldd_parameter_get(1, EUROPA_REG_W48_ADDR, &w48);

    //if(4 == TOKEN_NUM())  /* dump current setting */
    if(5 == TOKEN_NUM() && 0 == strcmp(TOKEN_STR(4), "show"))
    {
       diag_util_printf("Cross finetune: %sable  %s  %d\n",
         (w48 & EUROPA_REG_W48_TX_OFFSET_MOD_EN_MASK) ? "En":"Dis",
     (w48 & EUROPA_REG_W48_TX_OFFSET_MOD_SIGN_MASK) ? "Down":"Up",
     (w48 & EUROPA_REG_W48_TX_OFFSET_MOD_STR_MASK));

       return CPARSER_OK;
    }


    w48 = w48 & (0xff ^ cross_mask); /* clear cross setting */

    if(5 == TOKEN_NUM() && 0 == strcmp(TOKEN_STR(4), "disable"))
    {
       rtk_ldd_parameter_set(1, EUROPA_REG_W48_ADDR, &w48);

       diag_util_printf("Disable cross finetune.\n");

       return CPARSER_OK;
    }

    if(6 == TOKEN_NUM())
    {
       /* W48[5] = 1 for enable */
       cross_val = cross_val | (0x01<<EUROPA_REG_W48_TX_OFFSET_MOD_EN_OFFSET);

       /* W48[4] = 1 for move down */
       if (0 == strcmp(TOKEN_STR(4), "down"))
       {
          cross_val = cross_val | (0x01<<EUROPA_REG_W48_TX_OFFSET_MOD_SIGN_OFFSET);
       }
       else if (0 == strcmp(TOKEN_STR(4), "up"))
       {
      // do nothing
       }

       /* W48[3:0] for strength */
       level = *level_ptr;

       if (1>level || 16<level)
       {
          diag_util_printf("Invalid level=%d.\n", level);

          return CPARSER_ERR_INVALID_PARAMS;
       }
       else
       {
      level = level - 1;
       }

       cross_val = cross_val | (level)<<EUROPA_REG_W48_TX_OFFSET_MOD_STR_OFFSET;

       /* apply cross setting */
       w48 = w48 | cross_val;

       //diag_util_printf("w48=0x%02x\n", w48);
       rtk_ldd_parameter_set(1, EUROPA_REG_W48_ADDR, &w48);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_cross_up_down_disable_show_level */


/*
 * debug europa cal ( init | load | save )
 */
cparser_result_t
cparser_cmd_debug_europa_cal_init_load_save_refresh(
            cparser_context_t *context)
{
    cparser_result_t ret;

    DIAG_UTIL_PARAM_CHK();


    if(0 == strcmp(TOKEN_STR(3), "init"))
    {
        ret = _ldd_cal_init();
    }
    else if(0 == strcmp(TOKEN_STR(3), "load"))
    {
        ret = _ldd_cal_load();
    }
    else if(0 == strcmp(TOKEN_STR(3), "save"))
    {
        ret = _ldd_cal_save();
    }
    else if(0 == strcmp(TOKEN_STR(3), "refresh"))
    {
        ret = _ldd_cal_refresh();
    }



    return ret;

#if 0
    int ret;
    uint32 rssi_v0;
    uint32 mpd0;
    uint32 i;
    uint64 sum;
    uint32 index;

    uint8 reg;

    diag_util_printf("\n *** Initialize for calibration mode ***\n\n"
      " ==> Please keep dark condition, NO optical Tx/Rx power!\n\n");

    DIAG_UTIL_PARAM_CHK();

    /* set driver to calibration mode */
    DIAG_UTIL_ERR_CHK(rtk_ldd_calibration_state_set(ENABLED), ret);

    /* set no validation mode (W77=0xA5) */
    reg=0xA5;
    rtk_ldd_parameter_set(1, EUROPA_REG_W77_ADDR, &reg);

    /* get RSSI-V0 - ensure dark condition */
    rssi_v0 = 0;
    sum = 0;

    for (i=0;i<100;i++)
    {
        ret = rtk_ldd_rssiV0_get(&rssi_v0);
        if(ret)
        {
            diag_util_printf("Get RSSI V0 fail!!! (%d)\n", ret);

            return CPARSER_NOT_OK;
        }
        sum += rssi_v0;
    }
    rssi_v0 = sum/100;

    diag_util_printf("RSSI V0 = %d\n", rssi_v0);

    ldd_param.rssi_v0 = rssi_v0;
    rtk_ldd_flash_data_set(EUROPA_RSSI_V0_ADDR, 4, ldd_param.rssi_v0);



    /* get MPD0 - ensure no tx ? */
    mpd0 = 0;
    ret = rtk_ldd_mpd0_get(100, &mpd0);
    if(ret)
    {
        diag_util_printf("Get MPD0 fail!!! (%d)\n", ret);

        return CPARSER_NOT_OK;
    }

    diag_util_printf("MPD0 = 0x%x\n", mpd0);

    ldd_param.mpd0 = mpd0;
    rtk_ldd_flash_data_set(EUROPA_MPD0_ADDR, 4, ldd_param.mpd0);


    return CPARSER_OK;
#endif
}    /* end of cparser_cmd_debug_europa_cal_init_load_save */

/*
 * debug europa set booster <UINT:Vout> <UINT:Rfb1> <UINT:Rfb2>
 */
cparser_result_t
cparser_cmd_debug_europa_set_booster_Vout_Rfb1_Rfb2(
    cparser_context_t *context,
    uint32_t  *Vout_ptr,
    uint32_t  *Rfb1_ptr,
    uint32_t  *Rfb2_ptr)
{
    uint32 Vout;
    uint32 Rfb1;
    uint32 Rfb2;
    uint32 dac_code;

    uint8 dac_reg;

    DIAG_UTIL_PARAM_CHK();

    if(7 == TOKEN_NUM())
    {
       if (NULL!=Rfb1_ptr && NULL!=Rfb2_ptr)
       {
          Rfb1 = *Rfb1_ptr;
          Rfb2 = *Rfb2_ptr;
       }
    }
    else
    {
          Rfb1 = 200;
          Rfb2 = 10;
    }

    Vout = *Vout_ptr ; /* mV */

    if (30000>Vout || 80000<Vout)
    {
        diag_util_printf("Not support voltage %d(mV)\n", Vout);

    return CPARSER_ERR_INVALID_PARAMS;
    }

    dac_code = _ldd_booster_Vout2Dac(Vout, Rfb1, Rfb2);

    if (0xff<dac_code)
    {
        diag_util_printf("Booster DAC out-of-range\n");

    return CPARSER_NOT_OK;
    }
    else
    {
        dac_reg = (uint8) dac_code;
    }


    rtk_ldd_parameter_set(1, EUROPA_REG_DAC_HB_ADDR, &dac_reg);

    diag_util_printf("Booster DAC set 0x%02x (Vout=%d mV)\n", dac_code, Vout);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_booster_vout_rfb1_rfb2 */


/*
 * debug europa get booster <UINT:Rfb1> <UINT:Rfb2>
 */
cparser_result_t
cparser_cmd_debug_europa_get_booster_Rfb1_Rfb2(
    cparser_context_t *context,
    uint32_t  *Rfb1_ptr,
    uint32_t  *Rfb2_ptr)
{
    uint32 Vout;
    uint32 Rfb1;
    uint32 Rfb2;

    uint8 dac_reg;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(6 == TOKEN_NUM())
    {
       if (NULL!=Rfb1_ptr && NULL!=Rfb2_ptr)
       {
          Rfb1 = *Rfb1_ptr;
          Rfb2 = *Rfb2_ptr;
       }
    }
    else
    {
          Rfb1 = 200;
          Rfb2 = 10;
    }

    rtk_ldd_parameter_get(1, EUROPA_REG_DAC_HB_ADDR, &dac_reg);

    Vout = _ldd_booster_Dac2Vout(dac_reg, Rfb1, Rfb2);

    diag_util_printf("Booster DAC is 0x%02x (Vout=%d mV)\n", dac_reg, Vout);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_booster_rfb1_rfb2 */



/*
 * debug europa cal rx-ddmi ( 0 | 1 | 2 | 3 | calc ) { <UINT:value> }
 */
cparser_result_t
cparser_cmd_debug_europa_cal_rx_ddmi_0_1_2_3_calc_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    double a, b, c;

    DIAG_UTIL_PARAM_CHK();

    if(5 == TOKEN_NUM())
    {
       if(0 == strcmp(TOKEN_STR(4), "0"))
       {
           diag_util_printf("reset cal data.\n");
           rx_pwr[0]  = rx_pwr[1]  = rx_pwr[2] = 0;
       rx_rssi[0] = rx_rssi[1] = rx_rssi[2] = 0;
       }
       else if(0 == strcmp(TOKEN_STR(4), "calc"))
       {
           diag_util_printf("calculate cal data.\n");

       diag_util_printf("---- regression ...\n");
           _ldd_quad_regression(rx_rssi[0], rx_pwr[0],
                    rx_rssi[1], rx_pwr[1],
                rx_rssi[2], rx_pwr[2], &a, &b, &c);
           diag_util_printf("  a = %f\n", a);
           diag_util_printf("  b = %f\n", b);
           diag_util_printf("  c = %f\n", c);

       diag_util_printf("---- shifting...\n");
       a = a*(1<<RX_A_SHIFT);
       b = b*(1<<RX_B_SHIFT);
       c = c*(1<<RX_C_SHIFT);
           diag_util_printf("  a = %f\n", a);
           diag_util_printf("  b = %f\n", b);
           diag_util_printf("  c = %f\n", c);

       diag_util_printf("---- casting...\n");
       ldd_param.rx_a = (int32) a;
       ldd_param.rx_b = (int32) b;
       ldd_param.rx_c = (int32) c;
           diag_util_printf("  a = 0x%08x\n", ldd_param.rx_a);
           diag_util_printf("  b = 0x%08x\n", ldd_param.rx_b);
           diag_util_printf("  c = 0x%08x\n", ldd_param.rx_c);

       // save immediately
       rtk_ldd_flash_data_set(EUROPA_RX_A_ADDR, 4, (uint32)ldd_param.rx_a);
       rtk_ldd_flash_data_set(EUROPA_RX_B_ADDR, 4, (uint32)ldd_param.rx_b);
       rtk_ldd_flash_data_set(EUROPA_RX_C_ADDR, 4, (uint32)ldd_param.rx_c);
       }
    }
    else if(6 == TOKEN_NUM())
    {
       if(0 == strcmp(TOKEN_STR(4), "1"))
       {
           rx_pwr[0]  = *value_ptr;
       rx_rssi[0] = _ldd_get_rssi_current();

           diag_util_printf("Data1: Power=%6dnW Irssi=%6dnA\n",
               rx_pwr[0], rx_rssi[0]);
       }
       else if(0 == strcmp(TOKEN_STR(4), "2"))
       {
           rx_pwr[1]  = *value_ptr;
       rx_rssi[1] = _ldd_get_rssi_current();

           diag_util_printf("Data1: Power=%6dnW Irssi=%6dnA\n",
               rx_pwr[0], rx_rssi[0]);
           diag_util_printf("Data2: Power=%6dnW Irssi=%6dnA\n",
               rx_pwr[1], rx_rssi[1]);
       }
       else if(0 == strcmp(TOKEN_STR(4), "3"))
       {
           rx_pwr[2]  = *value_ptr;
       rx_rssi[2] = _ldd_get_rssi_current();

           diag_util_printf("Data1: Power=%6dnW Irssi=%6dnA\n",
               rx_pwr[0], rx_rssi[0]);
           diag_util_printf("Data2: Power=%6dnW Irssi=%6dnA\n",
               rx_pwr[1], rx_rssi[1]);
           diag_util_printf("Data3: Power=%6dnW Irssi=%6dnA\n",
               rx_pwr[2], rx_rssi[2]);
       }

    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_rx_ddmi_0_1_2_3_calc_value */


/*
 * debug europa cal tx-ddmi ( 0 | 1 | 2 | calc ) { <UINT:value> }
 */
cparser_result_t
cparser_cmd_debug_europa_cal_tx_ddmi_0_1_2_calc_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    double a, b, c;

    DIAG_UTIL_PARAM_CHK();

    if(5 == TOKEN_NUM())
    {
       if(0 == strcmp(TOKEN_STR(4), "0"))
       {
           diag_util_printf("reset cal data.\n");
           tx_pwr[0] = tx_pwr[1] = 0;
       tx_mpd[0] = tx_mpd[1] = 0;
       }
       else if(0 == strcmp(TOKEN_STR(4), "calc"))
       {
           diag_util_printf("calculate cal data.\n");

       diag_util_printf("---- regression ...\n");
           _ldd_lin_regression(tx_mpd[0], tx_pwr[0], tx_mpd[1], tx_pwr[1],
                   &a, &b);
           diag_util_printf("  a = %f\n", a);
           diag_util_printf("  b = %f\n", b);

       diag_util_printf("---- shifting...\n");
       a = a*(1<<TX_A_SHIFT);
       b = b*(1<<TX_B_SHIFT);
           diag_util_printf("  a = %f\n", a);
           diag_util_printf("  b = %f\n", b);

       diag_util_printf("---- casting...\n");
       ldd_param.tx_a = (int32) a;
       ldd_param.tx_b = (int32) b;
           diag_util_printf("  a = 0x%08x\n", ldd_param.tx_a);
           diag_util_printf("  b = 0x%08x\n", ldd_param.tx_b);

       // save immediately
       rtk_ldd_flash_data_set(EUROPA_TX_A_ADDR, 4, (uint32)ldd_param.tx_a);
       rtk_ldd_flash_data_set(EUROPA_TX_B_ADDR, 4, (uint32)ldd_param.tx_b);
       rtk_ldd_flash_data_set(EUROPA_TX_C_ADDR, 4, (uint32)ldd_param.tx_c);
       }
    }
    else if(6 == TOKEN_NUM())
    {
       if(0 == strcmp(TOKEN_STR(4), "1"))
       {
           tx_pwr[0] = *value_ptr;
       tx_mpd[0] = _ldd_get_mpd_current();

           diag_util_printf("Data1: Power=%6dnW Impd=%6dnA\n",
               tx_pwr[0], tx_mpd[0]);
       }
       else if(0 == strcmp(TOKEN_STR(4), "2"))
       {
           tx_pwr[1] = *value_ptr;
       tx_mpd[1] = _ldd_get_mpd_current();

           diag_util_printf("Data1: Power=%6dnW Impd=%6dnA\n",
               tx_pwr[0], tx_mpd[0]);
           diag_util_printf("Data2: Power=%6dnW Impd=%6dnA\n",
               tx_pwr[1], tx_mpd[1]);
       }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_tx_ddmi_0_1_2_calc_value */


/*
 * debug europa cal los ( assert | deassert | show ) { <UINT:value> }
 */
cparser_result_t
cparser_cmd_debug_europa_cal_los_assert_deassert_show_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    uint8 fine;
    uint8 fine_max, fine_min;
    uint8 i, loopcount=100;

    uint32 sum=0;

    DIAG_UTIL_PARAM_CHK();

    if(6 == TOKEN_NUM())
    {
       if('a'==TOKEN_CHAR(4, 0)) /* "assert" */
       {
           diag_util_printf("manually set LOS assert threshold.\n");

       fine = (uint8)*value_ptr;
       rtk_ldd_parameter_set(1, EUROPA_REG_RX_TH_ADDR, &fine);

       ldd_param.rx_th = fine;
       }
       if('d'==TOKEN_CHAR(4, 0)) /* "de-assert" */
       {
           diag_util_printf("manually set LOS de-assert threshold.\n");

       fine = (uint8)*value_ptr;
       rtk_ldd_parameter_set(1, EUROPA_REG_RX_DE_TH_ADDR, &fine);

       ldd_param.rx_deth = fine;
       }
    }
    else if(5 == TOKEN_NUM())
    {
       if('a'==TOKEN_CHAR(4, 0)) /* "assert" */
       {
           diag_util_printf("Getting RX swing...\n");

	   sum = 0;
	   for (i=0; i<loopcount; i++)
       {
          rtk_ldd_parameter_get(1, EUROPA_REG_RX_RSSI_ADDR, &fine);

          if (0==i)
          {
             fine_min = fine;
          }
          else
          {
             // FIXME  Is it okay to user min val as RX_TH ??
             if (fine < fine_min)
                fine_min = fine;
          }
	      sum += fine;
       }
	   fine_min= sum/loopcount;

       rtk_ldd_parameter_set(1, EUROPA_REG_RX_TH_ADDR, &fine_min);
       ldd_param.rx_th = fine_min;

           diag_util_printf("\nSet RX_TH=0x%02x\n", fine_min);
       if (0==fine_min)
       {
           diag_util_printf("\n!!! Threshold is 0 !!!\n");
       }

       }
       else if('d'==TOKEN_CHAR(4, 0)) /* "de-assert" */
       {
           diag_util_printf("Getting RX swing...\n");

	   sum = 0;
	   for (i=0; i<loopcount; i++)
       {
          rtk_ldd_parameter_get(1, EUROPA_REG_RX_RSSI_ADDR, &fine);

          if (0==i)
          {
             fine_max = fine;
          }
          else
          {
             // FIXME  Is it okay to user max val as RX_DETH ??
             if (fine > fine_max)
                fine_max = fine;
          }
	      sum += fine;
       }
	   fine_max = sum/loopcount;

       rtk_ldd_parameter_set(1, EUROPA_REG_RX_DE_TH_ADDR, &fine_max);

       ldd_param.rx_deth = fine_max;

           diag_util_printf("\nSet RX_DETH=0x%02x\n", fine_max);
       if (0xff==fine_max)
       {
           diag_util_printf("\n!!! Threshold is 0xFF !!!\n");
       }

       }
       else if('s'==TOKEN_CHAR(4, 0)) /* "show" */
       {
       rtk_ldd_parameter_get(1, EUROPA_REG_RX_TH_ADDR,
                        &ldd_param.rx_th);
       rtk_ldd_parameter_get(1, EUROPA_REG_RX_DE_TH_ADDR,
                        &ldd_param.rx_deth);

           diag_util_printf("Current setting:\n"
                 "LOS Assert Threshold    = 0x%02x\n"
                 "LOS De-Assert Threshold = 0x%02x\n",
                 ldd_param.rx_th, ldd_param.rx_deth);
       }

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_los_assert_deassert_show_value */


/*
 * debug europa cal vbr <UINT:Rfb1> <UINT:Rfb2> <UINT:Vstart> <UINT:Vend>
 */
cparser_result_t
cparser_cmd_debug_europa_cal_vbr_Rfb1_Rfb2_Vstart_Vend(
    cparser_context_t *context,
    uint32_t  *Rfb1_ptr,
    uint32_t  *Rfb2_ptr,
    uint32_t  *Vstart_ptr,
    uint32_t  *Vend_ptr)
{

    int ret;
    uint32 Rfb1,Rfb2;

    uint16 tempK;
    int32 tempC;

    uint32 parm;

    uint32 rssi_v0, i_rssi, v_rssi;
    uint32 loopcnt, i, cnt1, cnt2;
    uint64 sum_i, sum_v, sum_w;

    uint8 init_dac, max_dac;
    uint8 orig_dac, dac;

    uint32 start_v, end_v, Vbr;



    DIAG_UTIL_PARAM_CHK();

    /* === get parameter */
    Rfb1 = *Rfb1_ptr;
    Rfb2 = *Rfb2_ptr;

    start_v = *Vstart_ptr * 1000;  /* V to mV */
    end_v = *Vend_ptr * 1000;

    rtk_ldd_flash_data_get(EUROPA_RSSI_V0_ADDR, 4, &ldd_param.rssi_v0);
    rssi_v0 = ldd_param.rssi_v0;
    {
        if (0==rssi_v0)
        {
            diag_util_printf("Invalid RSSI-V0, run get rssi-v0 first!\n");
            return CPARSER_NOT_OK;
        }
    }

    // get orig A4/100, restore the value after Vbr test
    rtk_ldd_parameter_get(1, EUROPA_REG_DAC_HB_ADDR, &orig_dac);


    /* === detect Vbr */

    init_dac = _ldd_booster_Vout2Dac(start_v, Rfb1, Rfb2);
    max_dac = _ldd_booster_Vout2Dac(end_v, Rfb1, Rfb2);

    loopcnt = 1;
    i_rssi = 0;
    dac = init_dac;

    diag_util_printf("Detecting Vbr from %dmV to %dmV...\n", start_v, end_v);


    while((i_rssi<RSSI_CURR_BREAKDOWN_TH)&&(dac<=max_dac))
    {
        sum_i = 0;
        cnt1 = loopcnt;
        cnt2 = 0;

    rtk_ldd_parameter_set(1, EUROPA_REG_DAC_HB_ADDR, &dac);

        while ((cnt1>0) &&(cnt2<loopcnt))
        {
            ret = rtk_ldd_rx_power_get(rssi_v0, &v_rssi, &i_rssi);
            if(ret)
            {
                diag_util_printf("Get RX power fail!!! (%d)\n", ret);
                return CPARSER_NOT_OK;
            }

            if (v_rssi>rssi_v0)
            {
                cnt1--;
                sum_i += i_rssi;
            }
            else
            {
                cnt2++;
            }
        }

        //diag_util_printf("Break Down Test: APD value = 0x%x, cnt1 = %d, cnt2  = %d\n", dac, cnt1, cnt2);
        if (cnt2>=loopcnt)
        {
            dac++;
        }
        else if (cnt1==0)
        {
            i_rssi = sum_i/loopcnt;
            diag_util_printf("   DAC=0x%02x, I_RSSI=%d\n", dac, i_rssi);
            if (i_rssi >RSSI_CURR_BREAKDOWN_TH)
            {
                break;
            }
            else
            {
                dac++;
            }
        }
        else
        {
            diag_util_printf("Break Down Test Error!!!!\n");
            break;
        }
    }

    diag_util_printf("restore orig DAC=0x%x\n", orig_dac);
    rtk_ldd_parameter_set(1, EUROPA_REG_DAC_HB_ADDR, &orig_dac);

    diag_util_printf("\nTest Result: DAC=0x%x, I_RSSI=%d\n", dac, i_rssi);

    /* === calculate Vout value */
    if (RSSI_CURR_BREAKDOWN_TH > i_rssi)
    {
     diag_util_printf("!!! FAILE TO DETECT BREAKDOWN VOLTAGE !!!\n");

         Vbr = 0;
    }
    else
    {
         Vbr = _ldd_booster_Dac2Vout(dac, Rfb1, Rfb2);
    }


    /* === get temperature */
    tempK = 0;

    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        diag_util_printf("Get Temperature Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    tempC = (tempK*100-27315)/100; /* degK to degC */
    //printf("Temperature=%dK (%dC)\n", tempK, tempC);

#if 0
    rtk_ldd_flash_data_get(EUROPA_TEMP_OFFSET_ADDR, 1, &parm);
    ldd_param.temp_offset = (int8) parm;

    tempC = tempC - ldd_param.temp_offset;
    diag_util_printf("BOSA Temperature = %dC (offset=%d)\n",
              tempC, ldd_param.temp_offset);
#endif

    printf("\nVbr=%d (mV) @ T=%dC (DAC=0x%02x)\n", Vbr, tempC, dac);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_vbr_rfb1_rfb2_vstart_vend */


/*
 * debug europa get driver-version
 */
cparser_result_t
cparser_cmd_debug_europa_get_driver_version(
    cparser_context_t *context)
{
    uint32 ver;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_driver_version_get(&ver);
    diag_util_printf("LDD Driver Version %d.%d.%d\n",
            LDD_DRIVER_VERSION_MAJOR(ver),
            LDD_DRIVER_VERSION_MINOR(ver),
            LDD_DRIVER_VERSION_PATCH(ver));

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_driver_version */


#define LASER_LUT_SZ 304
#define IBCONV(b) ((b)*1024/80)>>2
#define IMCONV(m) ((m)*1024/100)>>2
/*
 * debug europa generate laserlut <UINT:IB0> <UINT:IM0> <UINT:T0> <UINT:SB0> <UINT:SB1> <UINT:SM0> <UINT:SM1>
 */
cparser_result_t
cparser_cmd_debug_europa_generate_laserlut_IB0_IM0_T0_SB0_SB1_SM0_SM1(
    cparser_context_t *context,
    uint32_t  *IB0_ptr,
    uint32_t  *IM0_ptr,
    uint32_t  *T0_ptr,
    double  *SB0_ptr,
    double  *SB1_ptr,
    double  *SM0_ptr,
    double  *SM1_ptr)
{
    uint32  ib0, im0;
    double  sb0, sb1, sm0, sm1;
    double  ftmp;

    uint8 lut[LASER_LUT_SZ] = { 0 };
    int t, t0;

    DIAG_UTIL_PARAM_CHK();

    ib0 = *IB0_ptr;
    im0 = *IM0_ptr;
    t0  = *T0_ptr;

    sb0 = *SB0_ptr;
    sb1 = *SB1_ptr;
    sm0 = *SM0_ptr;
    sm1 = *SM1_ptr;

    diag_util_printf("Ib=%d Im=%d T0=%d SB0=%f SB1=%f SM0=%f SM1=%f\n",
            ib0, im0, t0, sb0, sb1, sm0, sm1);

    for (t=-40; t<=110; t++)
    {
       if (t == t0)
       {
           lut[(t+40)*2] = IBCONV(ib0);
       lut[(t+40)*2+1] = IMCONV(im0);
       }
       else if (t < t0)
       {
       ftmp = ib0 - (t0 - t)*sb0;
       if (3.0 > ftmp)
          ftmp=3.0;
           lut[(t+40)*2] =  IBCONV((int)ftmp);

       ftmp = im0 - (t0 - t)*sm0;
       if (12.0 > ftmp)
          ftmp=12.0;
       lut[(t+40)*2+1] = IMCONV((int)ftmp);
       }
       else if (t > t0)
       {
       ftmp = ib0 + (t - t0)*sb1;
       if (ftmp>50.0)
          ftmp=50.0;
           lut[(t+40)*2] =  IBCONV((int)ftmp);

       ftmp = im0 + (t - t0)*sm1;
       if (ftmp>80.0)
          ftmp=80.0;
       lut[(t+40)*2+1] = IMCONV((int)ftmp);
       }
    }

    rtk_ldd_flash_array_set(EUROPA_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    for (t=-40; t<=110; t++)
    {
       diag_util_printf("%2x %2x", lut[(t+40)*2], lut[(t+40)*2+1]);
       diag_util_printf(" ");

       if ((t+1)%8 == 0)
       {
          diag_util_printf("\n");
       }
    }
    diag_util_printf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_debug_europa_generate_laserlut_ib0_im0_t0_sb0_sb1_sm0_sm1 */

/*
 * debug europa set laserlut
 */
cparser_result_t
cparser_cmd_debug_europa_set_laserlut(
    cparser_context_t *context)
{
    int ret;
    uint32  bias, mod, temperature;
    double  sb0, sb1, sm0, sm1;

    uint16 tempK;


    DIAG_UTIL_PARAM_CHK();

    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        diag_util_printf("Get Bias Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }
    bias = (bias * 2)/1000;  /* 2uA to mA */

    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        diag_util_printf("Get Mod Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }
    mod = (mod * 2)/1000;  /* 2uA to mA */

#if 1
    tempK = 0;
    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        diag_util_printf("Get Temperature Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }
    temperature = (tempK*100-27315)/100; /* degK to degC */
#else
    temperature = 25;
#endif


    sb0 = 0.06;
    sb1 = 0.12;
    sm0 = 0.03;
    sm1 = 0.10;

    cparser_cmd_debug_europa_generate_laserlut_IB0_IM0_T0_SB0_SB1_SM0_SM1(
       context, &bias, &mod, &temperature, &sb0, &sb1, &sm0, &sm1);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_set_laserlut */

/*
 * debug europa dump laserlut
 */
cparser_result_t
cparser_cmd_debug_europa_dump_laserlut(
    cparser_context_t *context)
{
    uint8 lut[LASER_LUT_SZ];
    int32 t;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_flash_array_get(EUROPA_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    diag_util_printf("LaserLUT\n");

    for (t=-40; t<=110; t++)
    {
       diag_util_printf("%02x %02x", lut[(t+40)*2], lut[(t+40)*2+1]);
       diag_util_printf(" ");

       if ((t+1)%8 == 0)
       {
          diag_util_printf("\n");
       }
    }
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_laserlut */




/*
 * debug europa get laserlut
 */
cparser_result_t
cparser_cmd_debug_europa_get_laserlut(
    cparser_context_t *context)
{
    uint8 lut[LASER_LUT_SZ];
    int32  i, j, num;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rtk_ldd_flash_array_get(EUROPA_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    diag_util_printf("Temperature  &  DAC value\n ");

    for (i=0;i<10;i++)
    {
        if (i<9)
        {
            num =16;
        }
        else
        {
            num = 7;
        }

        printf("Temperature : ");
        for(j=0;j<num;j++)
        {
            printf("%4d  ", j+i*16-40);
        }
        printf("\n ");
        printf("BIAS/MOD    : ");
        for(j=0;j<num;j++)
        {
            printf("%02x/%02x ", lut[(j+i*16)*2], lut[(j+i*16)*2+1]);
        }
        printf("\n\n ");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_laserlut */




/*
 *  * debug europa get fine { <UINT:count> }
 *   */
cparser_result_t
cparser_cmd_debug_europa_get_fine_count(
    cparser_context_t *context,
    uint32_t  *count_ptr)
{
    uint8 fine;
    uint8 fine_max, fine_min;
    uint32_t i, loopcount=100;
    uint32_t sum;

 
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_printf("Getting RSSI_FINE...\n");

    if(5 == TOKEN_NUM())
    {
       loopcount = *count_ptr;
    }
    
    diag_util_printf("Getting RSSI_FINE for %d loop...\n", loopcount);

    for (i=0, sum=0; i<=loopcount; i++)
    {
       rtk_ldd_parameter_get(1, EUROPA_REG_RX_RSSI_ADDR, &fine);

       sum += fine;

       if (0==i)
       {
         fine_min = fine;
     fine_max = fine;
       }
       else
       {
     if (fine < fine_min)
       fine_min = fine;
          
     if (fine > fine_max)
       fine_max = fine;
       }
    }
    
    diag_util_printf("\nFINE max=0x%02x min=0x%01x avg=0x%02x\n", fine_max, fine_min, sum/loopcount);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_fine_count */


/*
* debug europa get laserinfo { <UINT:extra> }
*/
cparser_result_t
cparser_cmd_debug_europa_get_laserinfo_extra(
    cparser_context_t *context,
    uint32_t  *extra_ptr)
{
    int ret;
    uint32 bias;
    uint32 mod;
    uint16 tempK;
    int32 tempC;

    uint32 extra;

    double ib, im;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if (NULL == extra_ptr)
        extra = 0;
    else extra = *extra_ptr;



    bias = 0;
    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        diag_util_printf("Get TX Bias Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    ib = ((double)bias*2)/1000;  /* 2uA to mA */

    mod = 0;
    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        diag_util_printf("Get TX Mod Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    im = ((double)mod*2)/1000;  /* 2uA to mA */


    tempK = 0;
    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        diag_util_printf("Get Temperature Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    tempC = (tempK*100-27315)/100; /* degK to degC */

 
    if (0 == extra)
    {
        diag_util_printf("\n[%4d] Bias=%2.2f Modulation=%2.2f\n", 
            tempC, ib, im);
    }
    else
    {
        uint32 impd;
        uint8 loopmode;

        impd = _ldd_get_mpd_current();

    impd = impd / 1000;   /* nA to uA */

        rtk_ldd_parameter_get(1, EUROPA_REG_W69_ADDR, &loopmode);
        loopmode = loopmode & 0x3;

        diag_util_printf("\n[%4d] Bias=%2.2f Modulation=%2.2f Impd=%4d loopmode=%d\n", 
        tempC, ib, im, impd, loopmode);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_get_laserinfo_extra */


/* +++++++++++++++ LUT generation ++++++++++++++++++ */

/*
 * debug europa generate laserlut2 data <UINT:index> <INT:temp> <FLOAT:bias> <FLOAT:mod>
 */
cparser_result_t
cparser_cmd_debug_europa_generate_laserlut2_data_index_temp_bias_mod(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    int32_t  *temp_ptr,
    double  *bias_ptr,
    double  *mod_ptr)
{
    uint32_t index;
    int32_t  temp;
    double bias, mod;

    DIAG_UTIL_PARAM_CHK();

    if (0==*index_ptr)
    {
    _ldd_laserlut_data_clear();
        return CPARSER_OK;
    }
    else if (*index_ptr > 0)
        index = *index_ptr;
    else
    {
    diag_util_printf("Invalid index\n");
        return CPARSER_NOT_OK;
    }

    if ((-40 <= *temp_ptr) && (110 >= *temp_ptr))
    temp = *temp_ptr;
    else
    {
    diag_util_printf("Invalid temp\n");
        return CPARSER_NOT_OK;
    }

    if (0.0 != *bias_ptr)
    bias = *bias_ptr;
    else
    {
    diag_util_printf("Invalid bias\n");
        return CPARSER_NOT_OK;
    }

    if (0.0 != *mod_ptr)
    mod = *mod_ptr;
    else
    {
    diag_util_printf("Invalid mod\n");
        return CPARSER_NOT_OK;
    }

    _ldd_laserlut_data_add(index, temp, bias, mod);

    diag_util_printf("\nEntery added. [%d] Temp=%3d Bias=%2.2f Mod=%2.2f\n", 
            index, temp, bias, mod);

    _ldd_laserlut_data_dump();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_generate_laserlut2_data_index_temp_bias_mod */


/*
 * debug europa generate laserlut2 clear
 */
cparser_result_t
cparser_cmd_debug_europa_generate_laserlut2_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    _ldd_laserlut_data_clear();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_generate_laserlut2_clear */


/*
 * debug europa generate laserlut2 calc { <UINT:Tchop> }
 */
cparser_result_t
cparser_cmd_debug_europa_generate_laserlut2_calc_Tchop(
    cparser_context_t *context,
    uint32_t  *Tchop_ptr)
{
    uint32_t Tchop;
    int num;

    uint8 lut[LASER_LUT_SZ] = { 0 };
    int t;

    DIAG_UTIL_PARAM_CHK();

    if(6 == TOKEN_NUM() && NULL!=Tchop_ptr)
    {
        Tchop = *Tchop_ptr;
    }
    else
    {
        Tchop = 100;
    }

    num = _ldd_laserlut_data_num_entry();

    _ldd_laserlut_lin_interpolation(gLaserLutData, num, gLaserLut);

    _ldd_laserlut_fixup(gLaserLutData, gLaserLut, Tchop);


    for (t=-40; t<=110; t++)
    {
       lut[(t+40)*2] = IBCONV((uint32)my_round(gLaserLut[t+40].bias));
       lut[(t+40)*2+1] = IMCONV((uint32)my_round(gLaserLut[t+40].mod));
    }

    rtk_ldd_flash_array_set(EUROPA_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    for (t=-40; t<=110; t++)
    {
       diag_util_printf("%02x %02x", lut[(t+40)*2], lut[(t+40)*2+1]);
       diag_util_printf(" ");

       if ((t+1)%8 == 0)
       {
          diag_util_printf("\n");
       }
    }
    diag_util_printf("\n");



    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_generate_laserlut2_calc_tchop */


/*
 * debug europa cal laserlut data <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_europa_cal_laserlut_data_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int ret;
    uint32_t index;
    uint32 bias;
    uint32 mod;
    uint16 tempK;
    int32 tempC;

    double ib, im;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if (0==*index_ptr)
    {
    _ldd_laserlut_data_clear();
        return CPARSER_OK;
    }
    else if (*index_ptr > 0)
        index = *index_ptr;
    else
    {
    diag_util_printf("Invalid index\n");
        return CPARSER_NOT_OK;
    }

    bias = 0;
    ret = rtk_ldd_tx_bias_get(&bias);
    if(ret)
    {
        diag_util_printf("Get TX Bias Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    ib = ((double)bias*2)/1000;  /* 2uA to mA */

    mod = 0;
    ret = rtk_ldd_tx_mod_get(&mod);
    if(ret)
    {
        diag_util_printf("Get TX Mod Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    im = ((double)mod*2)/1000;  /* 2uA to mA */


    tempK = 0;
    ret = rtk_ldd_temperature_get(&tempK);
    if(ret)
    {
        diag_util_printf("Get Temperature Fail!!! (%d)\n", ret);
        return CPARSER_NOT_OK;
    }

    tempC = (tempK*100-27315)/100; /* degK to degC */


    _ldd_laserlut_data_add(index, tempC, ib, im);

    diag_util_printf("\nEntery added. [%d] Temp=%3d Bias=%2.2f Mod=%2.2f\n", 
            index, tempC, ib, im);

    _ldd_laserlut_data_dump();


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_laserlut_data_index */


/*
 * debug europa cal laserlut clear
 */
cparser_result_t
cparser_cmd_debug_europa_cal_laserlut_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    _ldd_laserlut_data_clear();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_laserlut_clear */


/*
 * debug europa cal laserlut calc { <UINT:Tchop> }
 */
cparser_result_t
cparser_cmd_debug_europa_cal_laserlut_calc_Tchop(
    cparser_context_t *context,
    uint32_t  *Tchop_ptr)
{
    uint32_t Tchop;
    int num;

    uint8 lut[LASER_LUT_SZ] = { 0 };
    int t;


    DIAG_UTIL_PARAM_CHK();

    if(6 == TOKEN_NUM() && NULL!=Tchop_ptr)
    {
        Tchop = *Tchop_ptr;
    }
    else
    {
        Tchop = 100;
    }

    num = _ldd_laserlut_data_num_entry();

    _ldd_laserlut_lin_interpolation(gLaserLutData, num, gLaserLut);

    _ldd_laserlut_fixup(gLaserLutData, gLaserLut, Tchop);


    for (t=-40; t<=110; t++)
    {
       lut[(t+40)*2] = IBCONV((uint32)my_round(gLaserLut[t+40].bias));
       lut[(t+40)*2+1] = IMCONV((uint32)my_round(gLaserLut[t+40].mod));
    }

    rtk_ldd_flash_array_set(EUROPA_LASER_LUT_ADDR, LASER_LUT_SZ, lut);

    for (t=-40; t<=110; t++)
    {
       diag_util_printf("%02x %02x", lut[(t+40)*2], lut[(t+40)*2+1]);
       diag_util_printf(" ");

       if ((t+1)%8 == 0)
       {
          diag_util_printf("\n");
       }
    }
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_europa_cal_laserlut_calc_tchop */
/* --------------- LUT generation ------------------ */

#endif

/*
 * debug ext-mdio c22 init <set> <port> <phyid>
 */
cparser_result_t
cparser_cmd_debug_ext_mdio_c22_init_set_port_phyid(
    cparser_context_t *context,
    uint32_t *set_ptr,
    uint32_t *port_ptr,
    uint32_t *phyid_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    int32 ret;
    if(*set_ptr>1)
    {
        diag_util_mprintf("mdio set number should be 0~1\n");
        return RT_ERR_FAILED;
    }

    if(*port_ptr>3)
    {
        diag_util_mprintf("mdio port number should be 0~3\n");
        return RT_ERR_FAILED;
    }

    if(*phyid_ptr>31)
    {
        diag_util_mprintf("mdio phy id should be 0~31\n");
        return RT_ERR_FAILED;
    }

    DIAG_UTIL_ERR_CHK(rtk_mdio_init(), ret); 
    DIAG_UTIL_ERR_CHK(rtk_mdio_cfg_set(*set_ptr,*port_ptr,*phyid_ptr,MDIO_FMT_C22), ret); 

#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}

int32 ext_mdio_c22_write(uint32_t *reg_ptr, uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    int32 ret;
    DIAG_UTIL_ERR_CHK(rtk_mdio_c22_write(*reg_ptr,*value_ptr), ret); 
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}

int32 ext_mdio_c22_read(uint32_t *reg_ptr, uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    uint16 data;
    int32 ret;
    
    DIAG_UTIL_ERR_CHK(rtk_mdio_c22_read(*reg_ptr,&data), ret); 
    *value_ptr = data;
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}

/*
 * debug ext-mdio c22 set <UINT:reg> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_ext_mdio_c22_set_reg_value(
    cparser_context_t *context,
    uint32_t *reg_ptr,
    uint32_t *value_ptr)
{
    if(ext_mdio_c22_write(reg_ptr,value_ptr) == RT_ERR_OK)
    {
        diag_util_mprintf("write reg=%d data=0x%04x\n",*reg_ptr,*value_ptr);
    }

    return CPARSER_OK;
}

/*
 * debug ext-mdio c22 get <UINT:reg>
 */
cparser_result_t
cparser_cmd_debug_ext_mdio_c22_get_reg(
    cparser_context_t *context,
    uint32_t *reg_ptr)
{
    uint32 value;

    if(ext_mdio_c22_read(reg_ptr, &value) == RT_ERR_OK)
    {
        diag_util_mprintf("read reg=%d data=0x%04x\n",*reg_ptr,value);
    }

    return CPARSER_OK;
}

/*
 * debug ext-67c set <UINT:reg> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_ext_67c_set_reg_value(
    cparser_context_t *context,
    uint32_t *reg_ptr,
    uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    uint32 reg,value;

    //debug ext-phy set 31 0x000E
    reg=31;
    value=0x000E;
    ext_mdio_c22_write(&reg,&value);

    osal_time_mdelay(10);

    //debug ext-phy set 23 register address
    reg=23;
    value=*reg_ptr;
    ext_mdio_c22_write(&reg,&value);

    osal_time_mdelay(10);

    //debug ext-phy set 24 value
    reg=24;
    value=*value_ptr;
    ext_mdio_c22_write(&reg,&value);

    osal_time_mdelay(10);

    //debug ext-phy set 21 0x0003
    reg=21;
    value=0x0003;
    ext_mdio_c22_write(&reg,&value);
    
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}

/*
 * debug ext-67c get <UINT:reg>
 */
cparser_result_t
cparser_cmd_debug_ext_67c_get_reg(
    cparser_context_t *context,
    uint32_t *reg_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    uint32 reg,value;

    //debug ext-phy set 31 0x000E
    reg=31;
    value=0x000E;
    ext_mdio_c22_write(&reg,&value);

    osal_time_mdelay(10);

    //debug ext-phy set 23 register address
    reg=23;
    value=*reg_ptr;
    ext_mdio_c22_write(&reg,&value);

    osal_time_mdelay(10);

    //debug ext-phy set 21 0x0001
    reg=21;
    value=0x0001;
    ext_mdio_c22_write(&reg,&value);

    osal_time_mdelay(10);

    //debug ext-phy get 25
    reg=25;
    value=0x0;
    ext_mdio_c22_read(&reg,&value);

    diag_util_mprintf("read reg=%d data=0x%04x\n",*reg_ptr,value);
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}

/*
 * debug ext-mdio c45 init <set> <port> <phyid>
 */
cparser_result_t
cparser_cmd_debug_ext_mdio_c45_init_set_port_phyid(
    cparser_context_t *context,
    uint32_t *set_ptr,
    uint32_t *port_ptr,
    uint32_t *phyid_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    uint32 value,data;
    int32 ret;
    if(*set_ptr>1)
    {
        diag_util_mprintf("mdio set number should be 0~1\n");
        return RT_ERR_FAILED;
    }

    if(*port_ptr>3)
    {
        diag_util_mprintf("mdio port number should be 0~3\n");
        return RT_ERR_FAILED;
    }

    if(*phyid_ptr>31)
    {
        diag_util_mprintf("mdio phy id should be 0~31\n");
        return RT_ERR_FAILED;
    }
    DIAG_UTIL_ERR_CHK(rtk_mdio_init(), ret); 
    DIAG_UTIL_ERR_CHK(rtk_mdio_cfg_set(*set_ptr,*port_ptr,*phyid_ptr,MDIO_FMT_C45), ret); 
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}

int32 ext_mdio_c45_write(uint32_t *dev_ptr, uint32_t *reg_ptr, uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    int32 ret;
    DIAG_UTIL_ERR_CHK(rtk_mdio_c45_write(*dev_ptr,*reg_ptr,*value_ptr), ret); 
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}
int32 ext_mdio_c45_read(uint32_t *dev_ptr, uint32_t *reg_ptr, uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_RTL9607C)
    uint16 data;
    int32 ret;

    DIAG_UTIL_ERR_CHK(rtk_mdio_c45_read(*dev_ptr,*reg_ptr,&data), ret); 
    *value_ptr = data;
#else
    diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
    return RT_ERR_FAILED;
#endif
    return CPARSER_OK;
}
/*
 * debug ext-mdio c45 set <UINT:dev> <UINT:reg> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_ext_mdio_c45_set_dev_reg_value(
    cparser_context_t *context,
    uint32_t *dev_ptr,
    uint32_t *reg_ptr,
    uint32_t *value_ptr)
{
    if(ext_mdio_c45_write(dev_ptr,reg_ptr,value_ptr) == RT_ERR_OK)
    {
        diag_util_mprintf("write dev=%d reg=%d data=0x%04x\n",*dev_ptr,*reg_ptr,*value_ptr);
    }

    return CPARSER_OK;
}

/*
 * debug ext-mdio c45 get <UINT:dev> <UINT:reg>
 */
cparser_result_t
cparser_cmd_debug_ext_mdio_c45_get_dev_reg(
    cparser_context_t *context,
    uint32_t *dev_ptr,
    uint32_t *reg_ptr)
{
    uint32 value;

    if(ext_mdio_c45_read(dev_ptr,reg_ptr,&value) == RT_ERR_OK)
    {
        diag_util_mprintf("read dev=%d reg=%d data=0x%04x\n",*dev_ptr,*reg_ptr,value);
    }

    return CPARSER_OK;
}

/*
 * debug i2c read-test port <UINT:port> dev <UINT:dev_id> reg <UINT:register> data <UINT:data> count <UINT:count>
 */
cparser_result_t
cparser_cmd_debug_i2c_read_test_port_port_dev_dev_id_reg_register_data_data_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr,
    uint32_t  *count_ptr)
{
    int32 ret;
    uint8 fail = 0;
    uint32 i, data;

    DIAG_UTIL_PARAM_CHK();

    for(i = 0 ; i < *count_ptr ; i ++ )
    {
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(*port_ptr, *dev_id_ptr, *register_ptr, &data), ret);
        if(data != *data_ptr)
        {
            diag_util_printf("[%d] read 0x%04x != expect 0x%04x\n", i, data, *data_ptr);
            fail = 1;
        }
    }
    diag_util_printf("%s\n", (fail == 0) ? "PASS" : "FAIL");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_i2c_read_test_port_port_dev_dev_id_reg_register_data_data_count_count */

/*
 * debug i2c read-write-test port <UINT:port> dev <UINT:dev_id> reg <UINT:register> data <UINT:data> count <UINT:count>
 */
cparser_result_t
cparser_cmd_debug_i2c_read_write_test_port_port_dev_dev_id_reg_register_data_data_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr,
    uint32_t  *count_ptr)
{
    int32 ret;
    uint8 fail = 0;
    uint32 i, data;

    DIAG_UTIL_PARAM_CHK();

    for(i = 0 ; i < *count_ptr ; i ++ )
    {
        /* Write to other value first */
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(*port_ptr, *dev_id_ptr, *register_ptr, *data_ptr + 1), ret);
        DIAG_UTIL_ERR_CHK(rtk_i2c_write(*port_ptr, *dev_id_ptr, *register_ptr, *data_ptr), ret);
        DIAG_UTIL_ERR_CHK(rtk_i2c_read(*port_ptr, *dev_id_ptr, *register_ptr, &data), ret);
        if(data != *data_ptr)
        {
            diag_util_printf("[%d] read 0x%04x != write 0x%04x\n", i, data, *data_ptr);
            fail = 1;
        }
    }

    diag_util_printf("%s\n", (fail == 0) ? "PASS" : "FAIL");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_i2c_read_write_test_port_port_dev_dev_id_reg_register_data_data_count_count */

/*
 * debug i2c read-write-test port <UINT:port> dev <UINT:dev_id> reg <UINT:register> data <UINT:data> len <UINT:len> count <UINT:count>
 */
cparser_result_t
cparser_cmd_debug_i2c_read_write_test_port_port_dev_dev_id_reg_register_data_data_len_len_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr,
    uint32_t  *len_ptr,
    uint32_t  *count_ptr)
{
    int32 ret;
    uint8 fail = 0;
    uint32 i, j, *pData = NULL;

    DIAG_UTIL_PARAM_CHK();

    pData = (uint32 *)malloc(*len_ptr *sizeof(uint32));
    if(NULL == pData)
    {
        diag_util_printf("malloc for read failed\n");
        return CPARSER_OK;
    }
    for(i = 0 ; i < *count_ptr ; i ++ )
    {
        /* Write to other value first */
        for(j = 0 ; j < *len_ptr ; j ++)
        {
            pData[j] = *data_ptr + 1;
        }
        DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_seq_write(*port_ptr, *dev_id_ptr, *register_ptr, pData, *len_ptr), ret);
        if(ret!=RT_ERR_OK){
            free(pData);
            return CPARSER_NOT_OK;
        }
        usleep(10000);
        for(j = 0 ; j < *len_ptr ; j ++)
        {
            pData[j] = *data_ptr;
        }
        DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_seq_write(*port_ptr, *dev_id_ptr, *register_ptr, pData, *len_ptr), ret);
        if(ret!=RT_ERR_OK){
            free(pData);
            return CPARSER_NOT_OK;
        }

        usleep(10000);
        memset(pData, 0, sizeof(uint32) * (*len_ptr));
        DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_seq_read(*port_ptr, *dev_id_ptr, *register_ptr, pData, *len_ptr), ret);
        if(ret!=RT_ERR_OK){
            free(pData);
            return CPARSER_NOT_OK;
        }

        for(j = 0 ; j < *len_ptr ; j ++)
        {
            if(pData[j] != *data_ptr)
            {
                diag_util_printf("[%d] read 0x%04x != write 0x%04x\n", i, pData[j], *data_ptr);
                fail = 1;
            }
        }
    }
    free(pData);

    diag_util_printf("%s\n", (fail == 0) ? "PASS" : "FAIL");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_i2c_read_write_test_port_port_dev_dev_id_reg_register_data_data_len_len_count_count */

/*
 * debug i2c curr-read-write-test port <UINT:port> dev <UINT:dev_id> reg <UINT:register> data <UINT:data> len <UINT:len> count <UINT:count>
 */
cparser_result_t
cparser_cmd_debug_i2c_curr_read_write_test_port_port_dev_dev_id_reg_register_data_data_len_len_count_count(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *dev_id_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr,
    uint32_t  *len_ptr,
    uint32_t  *count_ptr)
{
    int32 ret;
    uint8 fail = 0;
    uint32 i, j, *pData = NULL;

    DIAG_UTIL_PARAM_CHK();

    pData = (uint32 *)malloc(*len_ptr *sizeof(uint32));
    if(NULL == pData)
    {
        diag_util_printf("malloc for read failed\n");
        return CPARSER_OK;
    }
    for(i = 0 ; i < *count_ptr ; i ++ )
    {
        /* Write to other value first */
        for(j = 0 ; j < *len_ptr ; j ++)
        {
            pData[j] = *data_ptr + 1;
        }
        DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_seq_write(*port_ptr, *dev_id_ptr, *register_ptr, pData, *len_ptr), ret);
        if(ret!=RT_ERR_OK){
            free(pData);
            return CPARSER_NOT_OK;
        }

        usleep(10000);
        for(j = 0 ; j < *len_ptr ; j ++)
        {
            pData[j] = *data_ptr;
        }
        DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_seq_write(*port_ptr, *dev_id_ptr, *register_ptr, pData, *len_ptr), ret);
        if(ret!=RT_ERR_OK){
            free(pData);
            return CPARSER_NOT_OK;
        }
        
        usleep(10000);
        //memset(pData, 0, sizeof(pData) * (*len_ptr));
        memset(pData, 0, sizeof(*pData));
        DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_read(*port_ptr, *dev_id_ptr, *register_ptr, &pData[0]), ret);
        if(ret!=RT_ERR_OK){
            free(pData);
            return CPARSER_NOT_OK;
        }

        if(*len_ptr > 1)
        {
            DIAG_UTIL_ERR_CHK_NO_RET(rtk_i2c_seqCurr_read(*port_ptr, *dev_id_ptr, pData + 1, (*len_ptr) - 1), ret);
            if(ret!=RT_ERR_OK){
                free(pData);
                return CPARSER_NOT_OK;
            }
        }
        for(j = 0 ; j < *len_ptr ; j ++)
        {
            if(pData[j] != *data_ptr)
            {
                diag_util_printf("[%d] read 0x%04x != write 0x%04x\n", i, pData[j], *data_ptr);
                fail = 1;
            }
        }
    }
    free(pData);

    diag_util_printf("%s\n", (fail == 0) ? "PASS" : "FAIL");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_i2c_curr_read_write_test_port_port_dev_dev_id_reg_register_data_data_len_len_count_count */

