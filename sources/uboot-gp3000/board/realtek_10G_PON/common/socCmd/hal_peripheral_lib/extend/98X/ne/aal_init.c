/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_init.c: AAL initialization
 */
#include "ca_ne_autoconf.h"
#include "ca_types.h"

//#include "aal_table.h"//TBD: clean bug

#include "aal_ni.h"
#include "aal_l3_if.h"
#include "aal_l3_cam.h"
#include "aal_hashlite.h"
#include "aal_l3_pe.h"
#include "aal_l3qm.h"
#include "aal_l3_cls.h"
#include "aal_l3fe.h"
#if defined(CONFIG_ARCH_CORTINA_G3LITE)
#include "aal_l3_te_cb.h"
#include "aal_hash.h"
#include "aal_lpm.h"
#endif
#include "aal_mcast.h"
#include "aal_arb.h"
#include "aal_mir.h"
#include "aal_l2_tm.h"
#include "aal_l2_tm_cb.h"
#include "aal_l2_cls.h"
#include "aal_sa_id.h"
#include "aal_ni_l2.h"
#include "aal_common.h"

#if 0
static int aal_system_setup(void)
{
	int ret = CA_E_OK;

	if (SOC_HAS_L3FE()) {
		/* Setup CAM Table
		 */
		if ((ret = aal_cam_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize CAM. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_G3HGU)
		/* Setup Hash Table (T2)
		 *  - all used hash masks
		 *  - all profiles including action group if hit, default action if miss
		 */
		if ((ret = aal_hash_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Engine. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		/* Setup LPM (T3)
		 *  - default route action
		 *  - memory range of each profile
		 *  - default t4ctrl
		 */
		if ((ret = aal_lpm_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize LPM. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}
#endif

		/* Setup HashLite (T4)
		 *  - init HW internal memory
		 *  - all used hash masks
		 *  - all profiles including action group if hit, default action if miss
		 *  - aging timer
		 */
		if ((ret = aal_hashlite_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Engine. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		/*
		 * Setup L3 special packets
		 */
		if ((ret = aal_l3_spcl_pkt_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3 spcl pkt. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}
	}

	return ret;
}
#endif

static int aal_driver_init(ca_device_id_t device_id)
{
        int ret = 0;

#if 1//sd1 uboot for 98f
        /* HW RST NI & NE */
        if ((ret = aal_glb_ni_ne_rst(device_id)) != AAL_E_OK) {
                ca_printf("%s:%d: Fail to HW rst NI & NE. (ret %d) (%s)\n", __func__, __LINE__, ret, __FILE__);
                return ret;
        }
#endif//sd1 uboot for 98f

        /* NI */
        if ((ret = aal_ni_init(device_id)) != AAL_E_OK) {
                ca_printf("%s:%d: Fail to initialize NI. (ret %d)\n", __func__, __LINE__, ret);
                return ret;
        }

        /* L2 Multicast */
        if ((ret = aal_im3e_mldda_init()) != AAL_E_OK) {
                ca_printf("%s:%d: Fail to initialize IM3E MLD DA Table. (ret %d)\n", __func__, __LINE__, ret);
                return ret;
        } else {
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\tIM3E MLD DA Table init done.\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }

        if ((ret = aal_im3e_mldsa_init()) != AAL_E_OK) {
                ca_printf("%s:%d: Fail to initialize IM3E MLD SA Table. (ret %d)\n", __func__, __LINE__, ret);
                return ret;
        } else {
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\tIM3E MLD SA Table init done.\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }

#if 0//TBD (+)
	/* L3 Multicast */
	if ((ret = aal_mce_indx_init()) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize MCE INDX Table. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}

	if ((ret = aal_arb_fib_init()) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize ARB FIB Table. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}
#endif//TBD (-)

	/* ARB */
	if ((ret = aal_arb_init(device_id)) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize L2 ARB. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}

#if 0//TBD (+)
	/* L2 Mirror Engine */
	if ((ret = aal_mire_init(device_id)) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize L2 Mirror Engine. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}
#endif//TBD (-)

        /* L2 Traffic Manager */
        if ((ret = aal_l2_tm_init(device_id)) != AAL_E_OK) {
                ca_printf("%s:%d: Fail to initialize L2 TM. (ret %d)\n", __func__, __LINE__, ret);
                return ret;
        }
        if ((ret = aal_l2_tm_cb_init(device_id)) != AAL_E_OK) {
                ca_printf("%s:%d: Fail to initialize L2 TM CB. (ret %d)\n", __func__, __LINE__, ret);
                return ret;
        }

#if 0//TBD (+)
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	/* L3 Traffic Engine */
	if ((ret = aal_l3_te_cb_init(device_id)) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize L3 TE CB. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}
#endif
#endif//TBD (-)

#if 1//yocto
#if 0
	/* L2 Classifier */
	if ((ret = aal_l2_cls_init(device_id)) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize L2 CLS. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}
#endif
#else//sd1 uboot for 98f mp test - init l2fe igr_cls, need for rgmii i/f loopback test
	/* L2 Classifier */
	if ((ret = aal_l2_cls_init(device_id)) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize L2 CLS. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}
#endif//sd1 uboot for 98f mp test - init l2fe igr_cls, need for rgmii i/f loopback test

#if 0//TBD (+)
	/* SA ID */
	if ((ret = __aal_sa_id_tbl_init()) != AAL_E_OK) {
		ca_printf("%s:%d: Fail to initialize SA ID Table. (ret %d)\n", __func__, __LINE__, ret);
		return ret;
	}

	if (SOC_HAS_L3FE()) {
		/* L3 Forwarding Engine */
		if ((ret = aal_l3fe_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3 FE. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		/* L3 PP CAM */
		if ((ret = aal_l3_cam_tbls_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3 PP CAM Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		/* L3 IF */
		if ((ret = aal_fe_l3e_if_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3 If Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_G3HGU)
		/* Hash */
		if ((ret = _hash_mask_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Mask Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _hash_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _cache_hash_table_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Cache Hash Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _cache_hash_action_table_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Cache Action Hash Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _hash_age_table_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Age Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _hash_overflow_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Overflow Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _hash_action_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Action Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _hash_overflow_action_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Overflow Action Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = _hash_default_action_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash Default Action Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}
#endif

		/* Hash-Lite */
		if ((ret = __hashlite_mask_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Mask Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = __hashlite_hash_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Hash Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = __hashlite_overflow_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Overflow Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = __hashlite_hash_action_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Hash Action Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = __hashlite_overflow_action_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Overflow Action Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = __hashlite_default_action_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize Hash-Lite Default Action Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_G3HGU)
		/* LPM */
		if ((ret = __aal_lpm_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize LPM Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}
#endif

		/* L3 PE */
		if ((ret = aal_l3pe_sixrd_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3FE PE 6RD Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		if ((ret = aal_l3pe_dsl_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3FE PE DS-Lite Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}

		/* L3 Classifier */
		if ((ret = aal_l3_cls_tbl_init()) != AAL_E_OK) {
			ca_printf("%s:%d: Fail to initialize L3 CLS Table. (ret %d)\n", __func__, __LINE__, ret);
			return ret;
		}
	}
#endif//TBD (-)

        return AAL_E_OK;
}

#if 0
static void aal_driver_exit(void)
{
	if (SOC_HAS_L3FE()) {
		/* NI */
		aal_l3_cam_tbls_exit();

		/* L3 IF */
		aal_fe_l3e_if_tbl_exit();

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_G3HGU)
		/* Hash */
		_hash_mask_tbl_exit();
		_hash_tbl_exit();
		_cache_hash_table_exit();
		_cache_hash_action_table_exit();
		_hash_age_table_exit();
		_hash_overflow_tbl_exit();
		_hash_action_exit();
		_hash_overflow_action_exit();
		_hash_default_action_exit();
#endif

		/* Hash-Lite */
		__hashlite_mask_tbl_exit();
		__hashlite_hash_tbl_exit();
		__hashlite_overflow_tbl_exit();
		__hashlite_hash_action_exit();
		__hashlite_overflow_action_exit();
		__hashlite_default_action_exit();

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_G3HGU)
		/* LPM */
		__aal_lpm_tbl_exit();
#endif

		/* L3FE PE */
		aal_l3pe_sixrd_tbl_exit();
		aal_l3pe_dsl_tbl_exit();
	}

	/* L2/3 Multicast */
	aal_mce_indx_exit();
	aal_arb_fib_exit();
	aal_im3e_mldda_exit();
	aal_im3e_mldsa_exit();

	/* SA ID */
	__aal_sa_id_tbl_exit();
}
#endif

ca_status_t aal_init(ca_device_id_t device_id)
{
	int ret;

	/* Initialize all AAL configuration and tables */
	ret = aal_driver_init(device_id);
	if (ret != AAL_E_OK)
		goto end_aal_init;

#if 0
	/* Setup system configuration through AAL functions; this must be behind AAL table initialization. */
	ret = aal_system_setup();
	if (ret != AAL_E_OK)
		goto end_aal_init;
#endif

end_aal_init:
	switch (ret) {
	case AAL_E_OK:
		return CA_E_OK;
	case AAL_E_TBLFULL:
		return CA_E_FULL;
	case AAL_E_ENTRY_USED:
		return CA_E_UNAVAIL;
	case AAL_E_OPNOTSUPP:
		return CA_E_NOT_SUPPORT;
	case AAL_E_NOMEM:
		return CA_E_MEMORY;
	case AAL_E_EXIST:
		return CA_E_EXISTS;
	default:
		return CA_E_INIT;
	}
}

#if 0
void aal_exit(void)
{
	/* Uninitialize all AAL tables */
	aal_driver_exit();
}
#endif

#define MAX_TX_MIB_CNT_NUM  26
char *tx_mib_tbl[MAX_TX_MIB_CNT_NUM] = {"TxUCPktCnt", "TxMCFrmCnt", "TxBCFrmCnt", "TxOAMFrmCnt", "TxJumboFrmCnt", \
                                "TxPauseFrmCnt", "TxPFCFrmCnt", "TxCrcErrFrmCnt", "TxOvSizeFrmCnt", "TxSingleColFrm", \
                                "TxMultiColFrm", "TxLateColFrm", "NONE", "TxExessColFrm", "TxStatsFrm64Oct", \
                                "TxStatsFrm65to127Oct", "TxStatsFrm128to255Oct", "TxStatsFrm256to511Oct", \
                                "TxStatsFrm512to1023Oct", "TxStatsFrm1024to1518Oct", "TxStatsFrm1519to2100Oct", \
                                "TxStatsFrm2101to9200Oct", "TxStatsFrm9201toMaxOct", "NONE", "TxByteCount_Lo", "TxByteCount_Hi"};

void dumpTxMIB(ca_uint32_t ca_ni_port)
{
        u16 i = 0x0;
        NI_HV_GLB_TXMIB_ACCESS_t ni_hv_glb_txmib_access;

        /* dump tx mib counter */
        for(i=0; i<MAX_TX_MIB_CNT_NUM; i++)
        {
                if("NONE" == tx_mib_tbl[i])
                        continue;

                ni_hv_glb_txmib_access.wrd = 0x0;
                ni_hv_glb_txmib_access.wrd = CA_REG_READ(NI_HV_GLB_TXMIB_ACCESS);
                ni_hv_glb_txmib_access.bf.access = 1;
                ni_hv_glb_txmib_access.bf.rbw = 0;

                /*
                  * 2'b00/11 = Plain read, counter value not changed
                  * 2'b01 = Read with MSB clear
                  * 2'b10 = Read all clear
                  */
                ni_hv_glb_txmib_access.bf.op_code = 0x0;
                ni_hv_glb_txmib_access.bf.port_id = ca_ni_port;
                ni_hv_glb_txmib_access.bf.counter_id = i;

                CA_REG_WRITE(ni_hv_glb_txmib_access.wrd, NI_HV_GLB_TXMIB_ACCESS);
                printf("%-30s(0x%x) of Port_%d:", tx_mib_tbl[i], i, ca_ni_port);
                printf("\t0x%08x", CA_REG_READ(NI_HV_GLB_TXMIB_DATA1));
                printf(" %08x\n", CA_REG_READ(NI_HV_GLB_TXMIB_DATA0));
        }

}

#define MAX_RX_MIB_CNT_NUM  26
char *rx_mib_tbl[MAX_RX_MIB_CNT_NUM] = {"RxUCPktCnt", "RxMCFrmCnt", "RxBCFrmCnt", "RxOAMFrmCnt", "RxJumboFrmCnt", \
                                "RxPauseFrmCnt", "RxPFCFrmCnt", "RxUnKnownOCFrmCnt", "RxCrcErrFrmCnt", "RxUndersizeFrmCnt", \
                                "RxRuntFrmCnt", "RxOvSizeFrmCnt", "RxJabberFrmCnt", "RxInvalidFrmCnt", "RxInvalidFlowFrmCnt", \
                                "RxStatsFrm64Oct", "RxStatsFrm65to127Oct", "RxStatsFrm128to255Oct", \
                                "RxStatsFrm256to511Oct", "RxStatsFrm512to1023Oct", "RxStatsFrm1024to1518Oct", \
                                "RxStatsFrm1519to2100Oct", "RxStatsFrm2101to9200Oct", "RxStatsFrm9201toMaxOct", "RxByteCount_Lo", "RxByteCount_Hi"};


void dumpRxMIB(ca_uint32_t ca_ni_port)
{
        u16 i = 0x0;
        NI_HV_GLB_RXMIB_ACCESS_t ni_hv_glb_rxmib_access;

        /* dump rx mib counter */
        for(i=0; i<MAX_RX_MIB_CNT_NUM; i++)
        {
                if("NONE" == rx_mib_tbl[i])
                        continue;

                ni_hv_glb_rxmib_access.wrd = 0x0;
                ni_hv_glb_rxmib_access.wrd = CA_REG_READ(NI_HV_GLB_RXMIB_ACCESS);
                ni_hv_glb_rxmib_access.bf.access = 1;
                ni_hv_glb_rxmib_access.bf.rbw = 0;

                /*
                  * 2'b00/11 = Plain read, counter value not changed
                  * 2'b01 = Read with MSB clear
                  * 2'b10 = Read all clear
                  */
                ni_hv_glb_rxmib_access.bf.op_code = 0x0;
                ni_hv_glb_rxmib_access.bf.port_id = ca_ni_port;
                ni_hv_glb_rxmib_access.bf.counter_id = i;

                CA_REG_WRITE(ni_hv_glb_rxmib_access.wrd, NI_HV_GLB_RXMIB_ACCESS);

                printf("%-30s(0x%x) of Port_%d:", rx_mib_tbl[i], i, ca_ni_port);
                printf("\t0x%08x", CA_REG_READ(NI_HV_GLB_RXMIB_DATA1));
                printf(" %08x\n", CA_REG_READ(NI_HV_GLB_RXMIB_DATA0));
        }

}

void dumpDropSrcCnt(void)
{
//        ca_device_id_t device_id = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
        ca_device_id_t device_id = 0;

        ca_status_t ret = CA_E_OK;
        aal_l2_fe_drop_source_t drop_source_cnt;

        memset(&drop_source_cnt,0x00,sizeof(drop_source_cnt));

        ret = aal_l2_fe_drop_source_cnt_get(device_id,&drop_source_cnt);
        if(ret != CA_E_OK){
                printf("%s failed, return = %d\n", __func__, ret );
                return CA_E_PARAM;
        }

        printf("%-30s: %08x \r\n","drop_source_cnt_00",drop_source_cnt.drop_source_cnt_00);
        printf("%-30s: %08x \r\n","ipv4_cs_chk_drp_01",drop_source_cnt.ipv4_cs_chk_drp_01);
        printf("%-30s: %08x \r\n","dpid_blackhole_drp_02",drop_source_cnt.dpid_blackhole_drp_02);
        printf("%-30s: %08x \r\n","ilpb_stp_chk_drp_03",drop_source_cnt.ilpb_stp_chk_drp_03);
        printf("%-30s: %08x \r\n","vlan_stp_chk_drp_04",drop_source_cnt.vlan_stp_chk_drp_04);
        printf("%-30s: %08x \r\n","ilpb_vlan_type_drp_05",drop_source_cnt.ilpb_vlan_type_drp_05);
        printf("%-30s: %08x \r\n","igr_cls_pmt_stp_drp_06",drop_source_cnt.igr_cls_pmt_stp_drp_06);
        printf("%-30s: %08x \r\n","vid4095_drp_07",drop_source_cnt.vid4095_drp_07);
        printf("%-30s: %08x \r\n","unknown_vlan_drp_08",drop_source_cnt.unknown_vlan_drp_08);
        printf("%-30s: %08x \r\n","ve_deny_mc_flag_drp_09",drop_source_cnt.ve_deny_mc_flag_drp_09);
        printf("%-30s: %08x \r\n","l2e_da_deny_drp_10",drop_source_cnt.l2e_da_deny_drp_10);
        printf("%-30s: %08x \r\n","non_std_sa_drp_11",drop_source_cnt.non_std_sa_drp_11);
        printf("%-30s: %08x \r\n","sw_learn_err_drp_12",drop_source_cnt.sw_learn_err_drp_12);
        printf("%-30s: %08x \r\n","l2e_sa_deny_drp_13",drop_source_cnt.l2e_sa_deny_drp_13);
        printf("%-30s: %08x \r\n","sa_mis_or_sa_limit_drp_14",drop_source_cnt.sa_mis_or_sa_limit_drp_14);
        printf("%-30s: %08x \r\n","uuc_umc_bc_drp_15",drop_source_cnt.uuc_umc_bc_drp_15);
        printf("%-30s: %08x \r\n","spid_dpid_not_in_rx_vlan_mem_drp_16",drop_source_cnt.spid_dpid_not_in_rx_vlan_mem_drp_16);
        printf("%-30s: %08x \r\n","dpid_not_in_port_mem_drp_17",drop_source_cnt.dpid_not_in_port_mem_drp_17);
        printf("%-30s: %08x \r\n","spid_dpid_not_in_tx_vlan_mem_drp_18",drop_source_cnt.spid_dpid_not_in_tx_vlan_mem_drp_18);
        printf("%-30s: %08x \r\n","tx_vlan_stp_drp_19",drop_source_cnt.tx_vlan_stp_drp_19);
        printf("%-30s: %08x \r\n","egr_cls_deny_drp_20",drop_source_cnt.egr_cls_deny_drp_20);
        printf("%-30s: %08x \r\n","loopback_drp_21",drop_source_cnt.loopback_drp_21);
        printf("%-30s: %08x \r\n","elpb_stp_drp_22",drop_source_cnt.elpb_stp_drp_22);
        printf("%-30s: %08x \r\n","drop_source_cnt_23",drop_source_cnt.drop_source_cnt_23);
        printf("%-30s: %08x \r\n","drop_source_cnt_24",drop_source_cnt.drop_source_cnt_24);
        printf("%-30s: %08x \r\n","mcfib_vlan_drp_25",drop_source_cnt.mcfib_vlan_drp_25);
        printf("%-30s: %08x \r\n","drop_source_cnt_26",drop_source_cnt.drop_source_cnt_26);
        printf("%-30s: %08x \r\n","mcfib_dpid_equal_spid_drp_27",drop_source_cnt.mcfib_dpid_equal_spid_drp_27);
        printf("%-30s: %08x \r\n","drop_source_cnt_28",drop_source_cnt.drop_source_cnt_28);
        printf("%-30s: %08x \r\n","drop_source_cnt_29",drop_source_cnt.drop_source_cnt_29);
        printf("%-30s: %08x \r\n","drop_source_cnt_30",drop_source_cnt.drop_source_cnt_30);
        printf("%-30s: %08x \r\n","drop_source_cnt_31",drop_source_cnt.drop_source_cnt_31);

        return CA_E_OK;
}

void dumpNiHdrA(ca_uint32_t ca_ni_port)
{
        ca_device_id_t device_id = 0;
        ca_status_t ret = CA_E_OK;
        aal_l2_fe_heada_t heada;

        memset(&heada,0x00,sizeof(heada));

        ret = aal_ni_heada_get(device_id, ca_ni_port, &heada);
        if(ret != CA_E_OK){
                printf("%s failed, return = %d\n", __func__, ret );
                return CA_E_PARAM;
        }

        printf("%-30s: %08x \r\n","heada_hi",heada.heada_hi);
        printf("%-30s: %08x \r\n","heada_mid",heada.heada_mid);
        printf("%-30s: %08x \r\n","heada_low",heada.heada_low);
        printf("%-30s: %08x \r\n","cpu_flag",heada.cpu_flag);
        printf("%-30s: %08x \r\n","deep_q",heada.deep_q);
        printf("%-30s: %08x \r\n","policy_group_id",heada.policy_group_id);
        printf("%-30s: %08x \r\n","policy_id",heada.policy_id);
        printf("%-30s: %08x \r\n","policy_ena",heada.policy_ena);
        printf("%-30s: %08x \r\n","marked",heada.marked);
        printf("%-30s: %08x \r\n","mirror",heada.mirror);
        printf("%-30s: %08x \r\n","no_drop",heada.no_drop);
        printf("%-30s: %08x \r\n","rx_packet_type",heada.rx_packet_type);
        printf("%-30s: %08x \r\n","drop_code",heada.drop_code);
        printf("%-30s: %08x \r\n","mc_group_id",heada.mc_group_id);
        printf("%-30s: %08x \r\n","header_type",heada.header_type);
        printf("%-30s: %08x \r\n","fe_bypass",heada.fe_bypass);
        printf("%-30s: %08x \r\n","packet_size",heada.packet_size);
        printf("%-30s: %08x \r\n","logic_spid",heada.logic_spid);
        printf("%-30s: %08x \r\n","logic_dpid",heada.logic_dpid);
        printf("%-30s:  %08x \r\n","cos",heada.cos);

        return CA_E_OK;
}

void dumpPpHdrA(void)
{
        ca_device_id_t device_id = 0;
        ca_status_t ret = CA_E_OK;
        aal_l2_fe_heada_t heada;

        memset(&heada,0x00,sizeof(heada));

        ret = aal_l2_fe_pp_heada_get(device_id,&heada);
        if(ret != CA_E_OK){
                printf("%s failed, return = %d\n", __func__, ret );
                return CA_E_PARAM;
        }

        printf("%-30s: %08x \r\n","heada_hi",heada.heada_hi);
        printf("%-30s: %08x \r\n","heada_mid",heada.heada_mid);
        printf("%-30s: %08x \r\n","heada_low",heada.heada_low);
        printf("%-30s: %08x \r\n","cpu_flag",heada.cpu_flag);
        printf("%-30s: %08x \r\n","deep_q",heada.deep_q);
        printf("%-30s: %08x \r\n","policy_group_id",heada.policy_group_id);
        printf("%-30s: %08x \r\n","policy_id",heada.policy_id);
        printf("%-30s: %08x \r\n","policy_ena",heada.policy_ena);
        printf("%-30s: %08x \r\n","marked",heada.marked);
        printf("%-30s: %08x \r\n","mirror",heada.mirror);
        printf("%-30s: %08x \r\n","no_drop",heada.no_drop);
        printf("%-30s: %08x \r\n","rx_packet_type",heada.rx_packet_type);
        printf("%-30s: %08x \r\n","drop_code",heada.drop_code);
        printf("%-30s: %08x \r\n","mc_group_id",heada.mc_group_id);
        printf("%-30s: %08x \r\n","header_type",heada.header_type);
        printf("%-30s: %08x \r\n","fe_bypass",heada.fe_bypass);
        printf("%-30s: %08x \r\n","packet_size",heada.packet_size);
        printf("%-30s: %08x \r\n","logic_spid",heada.logic_spid);
        printf("%-30s: %08x \r\n","logic_dpid",heada.logic_dpid);
        printf("%-30s: %08x \r\n","cos",heada.cos);

        return CA_E_OK;
}

void dumpPeHdrA(void)
{
        ca_device_id_t device_id = 0;
        ca_status_t ret = CA_E_OK;
        aal_l2_fe_heada_t heada;

        memset(&heada,0x00,sizeof(heada));

        ret = aal_l2_fe_pe_heada_get(device_id,&heada);
        if(ret != CA_E_OK){
                printf("%s failed, return = %d\n", __func__, ret );
                return CA_E_PARAM;
        }

        printf("%-30s: %08x \r\n","heada_hi",heada.heada_hi);
        printf("%-30s: %08x \r\n","heada_mid",heada.heada_mid);
        printf("%-30s: %08x \r\n","heada_low",heada.heada_low);
        printf("%-30s: %08x \r\n","cpu_flag",heada.cpu_flag);
        printf("%-30s: %08x \r\n","deep_q",heada.deep_q);
        printf("%-30s: %08x \r\n","policy_group_id",heada.policy_group_id);
        printf("%-30s: %08x \r\n","policy_id",heada.policy_id);
        printf("%-30s: %08x \r\n","policy_ena",heada.policy_ena);
        printf("%-30s: %08x \r\n","marked",heada.marked);
        printf("%-30s: %08x \r\n","mirror",heada.mirror);
        printf("%-30s: %08x \r\n","no_drop",heada.no_drop);
        printf("%-30s: %08x \r\n","rx_packet_type",heada.rx_packet_type);
        printf("%-30s: %08x \r\n","drop_code",heada.drop_code);
        printf("%-30s: %08x \r\n","mc_group_id",heada.mc_group_id);
        printf("%-30s: %08x \r\n","header_type",heada.header_type);
        printf("%-30s: %08x \r\n","fe_bypass",heada.fe_bypass);
        printf("%-30s: %08x \r\n","packet_size",heada.packet_size);
        printf("%-30s: %08x \r\n","logic_spid",heada.logic_spid);
        printf("%-30s: %08x \r\n","logic_dpid",heada.logic_dpid);
        printf("%-30s: %08x \r\n","cos",heada.cos);

        return CA_E_OK;
}

void dumpFdb(void)
{
    ca_device_id_t  device_id = 0; 
    ca_status_t ret = CA_E_OK;
    ca_uint32_t _flag;

    _flag = 2;//show {0: static; 1: dynamic; 2: both}

    ret = aal_fdb_table_get(device_id,_flag);
    if(ret != CA_E_OK){ 
        printf("%s failed, return = %d\n", __func__, ret );
        return CA_E_PARAM; 
    }

    return CA_E_OK; 
}

void dumpIlpb(ca_uint32_t ca_ni_port)
{
    ca_device_id_t  device_id = 0; 
    ca_status_t ret = CA_E_OK;
    ca_uint32_t _lpid;
    aal_ilpb_cfg_t  cfg ; 

    _lpid = ca_ni_port;

    memset(&cfg,0x00,sizeof(cfg));

    ret = aal_port_ilpb_cfg_get(device_id,_lpid,&cfg);
    if(ret != CA_E_OK){ 
        printf("%s failed, return = %d\n", __func__, ret );
        return CA_E_PARAM; 
    }

    printf("%-30s: %08x \r\n","wan_ind",cfg.wan_ind);
    printf("%-30s: %08x \r\n","igr_port_l2uc_redir_en",cfg.igr_port_l2uc_redir_en);
    printf("%-30s: %08x \r\n","igr_port_l2uc_ldpid",cfg.igr_port_l2uc_ldpid);
    printf("%-30s: %08x \r\n","igr_port_l2uc_cos",cfg.igr_port_l2uc_cos);
    printf("%-30s: %08x \r\n","igr_port_l2mc_redir_en",cfg.igr_port_l2mc_redir_en);
    printf("%-30s: %08x \r\n","igr_port_l2mc_ldpid",cfg.igr_port_l2mc_ldpid);
    printf("%-30s: %08x \r\n","igr_port_l2mc_cos",cfg.igr_port_l2mc_cos);
    printf("%-30s: %08x \r\n","igr_port_l2bc_redir_en",cfg.igr_port_l2bc_redir_en);
    printf("%-30s: %08x \r\n","igr_port_l2bc_ldpid",cfg.igr_port_l2bc_ldpid);
    printf("%-30s: %08x \r\n","igr_port_l2bc_cos",cfg.igr_port_l2bc_cos);
    printf("%-30s: %08x \r\n","default_vlanid",cfg.default_vlanid);
    printf("%-30s: %08x \r\n","sc_ind",cfg.sc_ind);
    printf("%-30s: %08x \r\n","unkwn_pol_idx",cfg.unkwn_pol_idx);
    printf("%-30s: %08x \r\n","vlan_cmd_sel_bm",cfg.vlan_cmd_sel_bm);
    printf("%-30s: %08x \r\n","default_top_vlan_cmd",cfg.default_top_vlan_cmd);
    printf("%-30s: %08x \r\n","default_untag_cmd",cfg.default_untag_cmd);
    printf("%-30s: %08x \r\n","inner_vid_sel_bm",cfg.inner_vid_sel_bm);
    printf("%-30s: %08x \r\n","inner_vlan_cmd",cfg.inner_vlan_cmd);
    printf("%-30s: %08x \r\n","default_noninnertag_cmd",cfg.default_noninnertag_cmd);
    printf("%-30s: %08x \r\n","drop_stag_packet",cfg.drop_stag_packet);
    printf("%-30s: %08x \r\n","drop_ctag_packet",cfg.drop_ctag_packet);
    printf("%-30s: %08x \r\n","drop_untagged_packet",cfg.drop_untagged_packet);
    printf("%-30s: %08x \r\n","drop_pritagged_packet",cfg.drop_pritagged_packet);
    printf("%-30s: %08x \r\n","drop_doubletagged_packet",cfg.drop_doubletagged_packet);
    printf("%-30s: %08x \r\n","drop_singletagged_packet",cfg.drop_singletagged_packet);
    printf("%-30s: %08x \r\n","s_tpid_match",cfg.s_tpid_match);
    printf("%-30s: %08x \r\n","c_tpid_match",cfg.c_tpid_match);
    printf("%-30s: %08x \r\n","top_s_tpid_enable",cfg.top_s_tpid_enable);
    printf("%-30s: %08x \r\n","top_c_tpid_enable",cfg.top_c_tpid_enable);
    printf("%-30s: %08x \r\n","inner_s_tpid_enable",cfg.inner_s_tpid_enable);
    printf("%-30s: %08x \r\n","inner_c_tpid_enable",cfg.inner_c_tpid_enable);
    printf("%-30s: %08x \r\n","other_tpid_match",cfg.other_tpid_match);
    printf("%-30s: %08x \r\n","stp_mode",cfg.stp_mode);
    printf("%-30s: %08x \r\n","station_move_en",cfg.station_move_en);
    printf("%-30s: %08x \r\n","sa_move_dis_fwd",cfg.sa_move_dis_fwd);
    printf("%-30s: %08x \r\n","loop_enable",cfg.loop_enable);
    printf("%-30s: %08x \r\n","da_sa_swap_en",cfg.da_sa_swap_en);
    printf("%-30s: %08x \r\n","spcl_pkt_idx",cfg.spcl_pkt_idx);
    printf("%-30s: %08x \r\n","igr_cls_lookup_en",cfg.igr_cls_lookup_en);
    printf("%-30s: %08x \r\n","cls_start",cfg.cls_start);
    printf("%-30s: %08x \r\n","cls_length",cfg.cls_length);
    printf("%-30s: %08x \r\n","flowid_sel_bm",cfg.flowid_sel_bm);
    printf("%-30s: %08x \r\n","top_802_1p_mode_sel",cfg.top_802_1p_mode_sel);
    printf("%-30s: %08x \r\n","change_1p_if_pop",cfg.change_1p_if_pop);
    printf("%-30s: %08x \r\n","dot1p_mark_control_bm",cfg.dot1p_mark_control_bm);
    printf("%-30s: %08x \r\n","inner_802_1p_mode_sel",cfg.inner_802_1p_mode_sel);
    printf("%-30s: %08x \r\n","default_802_1p",cfg.default_802_1p);
    printf("%-30s: %08x \r\n","dscp_markdown_en",cfg.dscp_markdown_en);
    printf("%-30s: %08x \r\n","dscp_mode_sel",cfg.dscp_mode_sel);
    printf("%-30s: %08x \r\n","dscp_mark_control_bm",cfg.dscp_mark_control_bm);
    printf("%-30s: %08x \r\n","default_dscp",cfg.default_dscp);
    printf("%-30s: %08x \r\n","cos_mode_sel",cfg.cos_mode_sel);
    printf("%-30s: %08x \r\n","cos_control_bm",cfg.cos_control_bm);
    printf("%-30s: %08x \r\n","default_cos",cfg.default_cos);
    printf("%-30s: %08x \r\n","ing_fltr_ena",cfg.ing_fltr_ena);
    printf("%-30s: %08x \r\n","color_src_sel",cfg.color_src_sel);
    printf("%-30s: %08x \r\n","dflt_premark",cfg.dflt_premark);

    return CA_E_OK ; 
}

void dumpNi2Pp(void)
{
        ca_device_id_t device_id = 0;
        ca_status_t ret = CA_E_OK;
        ca_ni2pp_cnt_t cnt;

        memset(&cnt,0x00,sizeof(cnt));

        ret = aal_l2_ni2pp_pkt_cnt_get(device_id,&cnt);
        if(ret != CA_E_OK){
                printf("%s failed, return = %d\n", __func__, ret );
                return CA_E_PARAM;
        }

        printf("%-30s: %08x \r\n","drp_cnt",cnt.drp_cnt);
        printf("%-30s: %08x \r\n","sop_cnt",cnt.sop_cnt);
        printf("%-30s: %08x \r\n","eop_cnt",cnt.eop_cnt);

        return CA_E_OK;
}

void dumpPe2Tm(void)
{
        ca_device_id_t device_id = 0;
        ca_status_t ret = CA_E_OK;
        ca_pe2tm_cnt_t cnt;

        memset(&cnt,0x00,sizeof(cnt));

        ret = aal_l2_pe2tm_pkt_cnt_get(device_id,&cnt);
        if(ret != CA_E_OK){
                printf("%s failed, return = %d\n", __func__, ret );
                return CA_E_PARAM;
        }

        printf("%-30s: %08x \r\n","tm2fe_ready",cnt.tm2fe_ready);
        printf("%-30s: %08x \r\n","sop_cnt",cnt.sop_cnt);
        printf("%-30s: %08x \r\n","eop_cnt",cnt.eop_cnt);

        return CA_E_OK;
}

void fephyPageRead(
    ca_uint16_t page,
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t *data
)
{
    fe_page_read(page, phy_ad, reg_ad, data);

#if 1
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") = " \
        LIGHT_BLUE  "%#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, *data);
#endif
}

void fephyPageWrite(
    ca_uint16_t page,
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t data
)
{
    fe_page_write(page, phy_ad, reg_ad, data);

#if 1
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") with " \
        LIGHT_BLUE  "data: %#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, data);
#endif
}

void mdioRead(
    ca_uint16_t page,
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t *data
)
{
    mdio_read(phy_ad, reg_ad, data);

#if 1
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") = " \
        LIGHT_BLUE  "%#x" NOCOLOR "\n", \
        __func__, phy_ad, reg_ad, *data);
#endif
}

void rt_mdioRead(
/*    ca_uint16_t page,*/
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t *data
)
{
    rt_mdio_read(/*page, */phy_ad, reg_ad, data);

#if 1
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
/*        RED "page %#x" NOCOLOR ", " \*/
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") = " \
        LIGHT_BLUE  "%#x" NOCOLOR "\n", \
        __func__, /*page, */phy_ad, reg_ad, *data);
#endif
}

void mdioPageRead(
    ca_uint16_t page,
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t *data
)
{
    mdio_page_read(page, phy_ad, reg_ad, data);

#if 1
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") = " \
        LIGHT_BLUE  "%#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, *data);
#endif
}

void rt_mdioWrite(
/*    ca_uint16_t page,*/
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t data
)
{
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
/*        RED "page %#x" NOCOLOR ", " \*/
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") with " \
        LIGHT_BLUE  "data: %#x" NOCOLOR "\n", \
        __func__, /*page, */phy_ad, reg_ad, data);

    rt_mdio_write(/*page, */phy_ad, reg_ad, data);
}

void mdioPageWrite(
    ca_uint16_t page,
    ca_uint32_t phy_ad,
    ca_uint32_t reg_ad,
    ca_uint16_t data
)
{
    /* show msg to user */
    printf("\t" LIGHT_GRAY "%s" NOCOLOR "(" \
        RED "page %#x" NOCOLOR ", " \
        YELLOW "phy_ad %#x" NOCOLOR ", " \
        GREEN "reg_ad %#x" NOCOLOR ") with " \
        LIGHT_BLUE  "data: %#x" NOCOLOR "\n", \
        __func__, page, phy_ad, reg_ad, data);

    mdio_page_write(page, phy_ad, reg_ad, data);
}

void fe_phy_AN(void)
{
    aal_fe_phy_auto_neg();

    return;
}

void fe_phy_force_mode(
    ca_port_id_t port_id,
    int fe_speed,
    bool is_full
){
    aal_fe_phy_force_mode(port_id, fe_speed, is_full);

    return;
}

void ge_nic_set(
    ca_uint8_t phy_ad,
    ca_uint8_t nic_mode,
    int ge_speed,
    bool is_full
){

#if 0
printf("%s  %#x  %#x  %#x  %#x\n", __func__, \
    phy_ad, nic_mode, ge_speed, is_full);
#endif
//printf("\t %s %d\n",__FUNCTION__,__LINE__);
    aal_set_8211f(phy_ad, nic_mode, ge_speed, is_full);
}

void ilpb_loopback_set(
    ca_port_id_t port_id, 
    ca_boolean_t en_loopback
)
{

#if 0
printf("%s\tport_id: %#x\ten_loopback: %#x\n", __func__, \
    port_id, en_loopback);
#endif

    ca_ilpb_loopback_set(port_id, en_loopback);
}

void mc_fib_loopback_set(
    ca_port_id_t port_id, 
    ca_boolean_t en_loopback
)
{

#if 0
printf("%s\tport_id: %#x\ten_loopback: %#x\n", __func__, \
    port_id, en_loopback);
#endif

    ca_mc_fib_loopback_set(port_id, en_loopback);
}

//+Andy
void fdb_sram_rw_test(
    ca_uint32       reg_value
)
{
	ca_device_id_t  device_id = 0; 
    ca_status_t ret = CA_E_OK;

    ret = aal_fdb_sram_rw_test(device_id, reg_value);
    if(ret != CA_E_OK){ 
        printf("%s failed, number of error fdb entries = %d\n", __func__, ret);
        return CA_E_PARAM; 
    }else
		printf("%s pass!!\n", __func__);

    return CA_E_OK; 
}
