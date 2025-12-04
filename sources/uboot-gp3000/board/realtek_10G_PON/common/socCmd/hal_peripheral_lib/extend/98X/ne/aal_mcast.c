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
 * aal_mcast.c: Hardware Abstraction Layer for Multicast to access hardware regsiters
 *
 */

#include "ca_ne_autoconf.h"
//#include <linux/sched.h>

#include "aal_common.h"
#include "aal_table.h"
#include "aal_mcast.h"
#include "aal_port.h"
#include "scfg.h"
#include "vlan.h"

SIZE_CHECK_ASSERT(sizeof(ni_mce_indx_tbl_entry_t) == 8);
SIZE_CHECK_ASSERT(sizeof(l2fe_arb_fib_tbl_entry_t) == 16);
SIZE_CHECK_ASSERT(sizeof(im3e_mldda_entry_t) == 24);
SIZE_CHECK_ASSERT(sizeof(im3e_mldsa_entry_t) == 20);

#define IM3E_CMD_ADD	1
#define IM3E_CMD_DEL	2
#define IM3E_CMD_SRCH	3
#define IM3E_CMD_SRCH_N	4
#define TABLE_TRY_TIMEOUT	1000

struct repl_bits_s {
	ca_uint16_t ref_cnt;
	repl_key_t key;
};

struct mcgid_entry_s {
	ca_uint8_t used; //0: free 1: used
	ca_uint8_t *l2ptr; //point back to mcg or src node
	ca_uint8_t *l3ptr; //point back to mcg or src node
};

struct fake_vlan_entry_s {
	ca_uint8_t valid; //has fake_vid
	ca_uint16_t ref_cnt; //has wan_vid
	ca_uint16_t wan_vid;
	ca_uint16_t fake_vid;
};

struct repl_bits_s free_repl_bits_pool[ARB_FIB_TBL_ENTRY_MAX];
struct mcgid_entry_s free_mcgid_pool[MCE_INDX_TBL_ENTRY_MAX];
struct mcgid_entry_s fake_mcgid_pool[MCE_INDX_TBL_ENTRY_MAX];
struct fake_vlan_entry_s fake_vlan_pool[CFG_ID_FAKE_WAN_MC_VID_LEN];
ca_uint8_t im3e_cmd_mode = 1; /* 0: false 1: true */

ca_uint16_t aal_mc_fake_vid_get(ca_uint16_t wan_vid)
{
	int i;

	for (i = 0; i < CFG_ID_FAKE_WAN_MC_VID_LEN; i++) {
		if (fake_vlan_pool[i].valid && fake_vlan_pool[i].ref_cnt &&
			fake_vlan_pool[i].wan_vid == wan_vid) {
			return fake_vlan_pool[i].fake_vid;
		}
	}

	return CA_UINT16_INVALID;
}

/* Assume wan_vid is not allocated before. */
ca_uint16_t aal_mc_fake_vid_alloc(ca_uint16_t wan_vid)
{
	int wan_port_type_is_l3, i, free_idx = CFG_ID_FAKE_WAN_MC_VID_LEN;
	ca_port_id_t port_id;

	if (AAL_LPORT_ETH_WAN == AAL_LPORT_ETH_NI7)
		port_id = CA_PORT_ID(CA_PORT_TYPE_ETHERNET, AAL_LPORT_ETH_WAN);
	else
		port_id = CA_PORT_ID(CA_PORT_TYPE_L2RP, AAL_LPORT_L3_WAN);

	for (i = 0; i < CFG_ID_FAKE_WAN_MC_VID_LEN; i++) {
		if (fake_vlan_pool[i].valid && !fake_vlan_pool[i].ref_cnt) {
			if (free_idx == CFG_ID_FAKE_WAN_MC_VID_LEN)
				free_idx = i;
		} else if (fake_vlan_pool[i].valid && fake_vlan_pool[i].ref_cnt &&
			fake_vlan_pool[i].wan_vid == wan_vid) {
			fake_vlan_pool[i].ref_cnt++;
			return fake_vlan_pool[i].fake_vid;
		}
	}

	/* Full */
	if (free_idx == CFG_ID_FAKE_WAN_MC_VID_LEN)
		return CA_UINT16_INVALID;

	fake_vlan_pool[free_idx].wan_vid = wan_vid;
	fake_vlan_pool[free_idx].ref_cnt = 1;
	/* Make a hole */
	ca_l2_vlan_create(0, fake_vlan_pool[free_idx].fake_vid);
	ca_l2_vlan_port_set(0, fake_vlan_pool[free_idx].fake_vid, 1, &port_id, 0, &port_id);

	return fake_vlan_pool[free_idx].fake_vid;
}

void aal_mc_fake_vid_free(ca_uint16_t wan_vid)
{
	int i;

	for (i = 0; i < CFG_ID_FAKE_WAN_MC_VID_LEN; i++) {
		if (fake_vlan_pool[i].valid && fake_vlan_pool[i].ref_cnt &&
			fake_vlan_pool[i].wan_vid == wan_vid) {
			fake_vlan_pool[i].ref_cnt--;
			if (fake_vlan_pool[i].ref_cnt == 0)
				ca_l2_vlan_delete(0, fake_vlan_pool[i].fake_vid);
			return;
		}
	}

	return;
}

/***************************************************************************
 * NI, MCE (Multicast Engine)
 ***************************************************************************/

/* Table NI_HV.MCE_INDX */
static aal_table_t ni_mce_indx_table;

int aal_mce_indx_init(void)
{
	spin_lock_init(&ni_mce_indx_table.lock);

	return __aal_table_register(AAL_TABLE_MCE_INDX_TBL, &ni_mce_indx_table);
}

void aal_mce_indx_exit(void)
{
	__aal_table_unregister(AAL_TABLE_MCE_INDX_TBL);
}

static int aal_mce_indx_entry_add_by_idx(void *entry, unsigned int idx)
{
	return generic_entry_add_by_idx(&ni_mce_indx_table, entry, idx);
}

static int aal_mce_indx_entry_del(unsigned int idx)
{
	return generic_entry_del(&ni_mce_indx_table, idx);
}

static int aal_mce_indx_entry_set(unsigned int idx, void *entry)
{
	return generic_entry_set(&ni_mce_indx_table, idx, entry);
}

static int aal_mce_indx_entry_get_by_idx(unsigned int idx, void *entry)
{
	return generic_entry_get_by_idx(&ni_mce_indx_table, idx, entry);
}

static void aal_mce_indx_entry_print(unsigned int start_idx, int count)
{
	aal_table_t *table = &ni_mce_indx_table;
	ni_mce_indx_tbl_entry_t entry;
	int i, ret;
	unsigned int refcnt;

	if (start_idx >= table->max_entry) {
//		printk(KERN_ERR "%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		printf("%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		return;
	}

//	printk(KERN_INFO "|===== NI MCE INDX Table ==================================================|\n");
	printf("|===== NI MCE INDX Table ==================================================|\n");

	for (i = 0; i < count; i++) {
		if (start_idx + i >= table->max_entry)
			break;
		if (ca_aal_table_sw_cache) {
			ret = aal_mce_indx_entry_get_by_idx(start_idx + i, (void *)&entry);
		} else {
			ret = hw_entry_get(&ni_mce_indx_table, start_idx + i, &entry);
		}
		if ((ret == AAL_E_ENTRYNOTRSVD) || (ret == AAL_E_ENTRYNOTFOUND) || (ret == AAL_E_NULLPTR)) {
//			printk(KERN_INFO "| index %04d | NOT USED\n", start_idx + i);
			printf("| index %04d | NOT USED\n", start_idx + i);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
		} else {
			refcnt = atomic_read(&table->entry[start_idx + i].refcnt);

//			printk(KERN_INFO "| index %04d | ref count %d\n", start_idx + i, refcnt);
			printf("| index %04d | ref count %d\n", start_idx + i, refcnt);
//			printk(KERN_INFO "  |- replication bits: 0x%016llx\n", entry.mc_vec);
			printf("  |- replication bits: 0x%016llx\n", entry.mc_vec);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
#if 0//yocto
			cond_resched();
#else//sd1 uboot for 98f
#endif
		}
	}
}

static aal_table_t ni_mce_indx_table = {
	.table_id = AAL_TABLE_MCE_INDX_TBL,
	.table_name = "Multicast Engine (MCE)",
	.max_entry = MCE_INDX_TBL_ENTRY_MAX,
	.used_entry = 0,
	.curr_ptr = 0,
	.entry_size = sizeof(ni_mce_indx_tbl_entry_t),
	.access_reg = NI_HV_MCE_INDX_LKUP_ACCESS,
	.data_reg_num = 2,
	.op = {
		.entry_add = NULL,
		.entry_add_by_idx = aal_mce_indx_entry_add_by_idx,
		.entry_del = aal_mce_indx_entry_del,
		.entry_set = aal_mce_indx_entry_set,
		.entry_get = NULL,
		.entry_get_by_idx = aal_mce_indx_entry_get_by_idx,
		.table_flush = NULL,
		.get_avail_count = NULL,
		.entry_print = aal_mce_indx_entry_print,
	},
};

/***************************************************************************
 * L2FE, ARB
 ***************************************************************************/

/* Table L2FE.ARB.MC_FIB */
static aal_table_t l2fe_arb_fib_table;

int aal_arb_fib_init(void)
{
	spin_lock_init(&l2fe_arb_fib_table.lock);

	return __aal_table_register(AAL_TABLE_ARB_FIB_TBL, &l2fe_arb_fib_table);
}

void aal_arb_fib_exit(void)
{
	__aal_table_unregister(AAL_TABLE_ARB_FIB_TBL);
}

static int aal_arb_fib_entry_add_by_idx(void *entry, unsigned int idx)
{
	return generic_entry_add_by_idx(&l2fe_arb_fib_table, entry, idx);
}

static int aal_arb_fib_entry_del(unsigned int idx)
{
	return generic_entry_del(&l2fe_arb_fib_table, idx);
}

static int aal_arb_fib_entry_set(unsigned int idx, void *entry)
{
	return generic_entry_set(&l2fe_arb_fib_table, idx, entry);
}

static int aal_arb_fib_entry_get_by_idx(unsigned int idx, void *entry)
{
	return generic_entry_get_by_idx(&l2fe_arb_fib_table, idx, entry);
}

static void aal_arb_fib_entry_print(unsigned int start_idx, int count)
{
	aal_table_t *table = &l2fe_arb_fib_table;
	l2fe_arb_fib_tbl_entry_t entry;
	int i, ret;
	unsigned int refcnt;

	if (start_idx >= table->max_entry) {
//		printk(KERN_ERR "%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		printf("%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		return;
	}

//	printk(KERN_INFO "|===== L2FE ARB MC FIB Table ==================================================|\n");
	printf("|===== L2FE ARB MC FIB Table ==================================================|\n");

	for (i = 0; i < count; i++) {
		if (start_idx + i >= table->max_entry)
			break;
		if (ca_aal_table_sw_cache) {
			ret = aal_arb_fib_entry_get_by_idx(start_idx + i, (void *)&entry);
		} else {
			ret = hw_entry_get(&l2fe_arb_fib_table, start_idx + i, (void *)&entry);
		}
		if ((ret == AAL_E_ENTRYNOTRSVD) || (ret == AAL_E_ENTRYNOTFOUND) || (ret == AAL_E_NULLPTR)) {
//			printk(KERN_INFO "| index %04d | NOT USED\n", start_idx + i);
			printf("| index %04d | NOT USED\n", start_idx + i);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
		} else {
			refcnt = atomic_read(&table->entry[start_idx + i].refcnt);

//			printk(KERN_INFO "| index %04d | ref count %d\n", start_idx + i, refcnt);
			printf("| index %04d | ref count %d\n", start_idx + i, refcnt);
//			printk(KERN_INFO "  |- wan_dst: %d (distined to %s)\n", entry.wan_dst,
			printf("  |- wan_dst: %d (distined to %s)\n", entry.wan_dst,
					(entry.wan_dst)? "WAN": "LAN");
//			printk(KERN_INFO "  |- permit_spid_en: %d (%s)\n", entry.permit_spid_en,
			printf("  |- permit_spid_en: %d (%s)\n", entry.permit_spid_en,
					(entry.permit_spid_en)? "NOP": "Drop source port loopback");
//			printk(KERN_INFO "  |- mac_sa_cmd: %d (%s)\n", entry.mac_sa_cmd,
			printf("  |- mac_sa_cmd: %d (%s)\n", entry.mac_sa_cmd,
					(entry.mac_sa_cmd)? "swap":"old");
//			printk(KERN_INFO "  |- mac_sa_sel: %d\n", entry.mac_sa_sel);
			printf("  |- mac_sa_sel: %d\n", entry.mac_sa_sel);
//			printk(KERN_INFO "  |- ldpid: 0x%03x\n", entry.ldpid);
			printf("  |- ldpid: 0x%03x\n", entry.ldpid);

//			printk(KERN_INFO "  |- cos: 0x%03x\n", entry.cos);
			printf("  |- cos: 0x%03x\n", entry.cos);
//			printk(KERN_INFO "  |- pol_en: %d\n", entry.pol_en);
			printf("  |- pol_en: %d\n", entry.pol_en);

//			printk(KERN_INFO "  |- pol_grp_id: 0x%03x\n", entry.pol_grp_id);
			printf("  |- pol_grp_id: 0x%03x\n", entry.pol_grp_id);
//			printk(KERN_INFO "  |- pol_id: 0x%03x\n", entry.pol_id);
			printf("  |- pol_id: 0x%03x\n", entry.pol_id);
                        
//			printk(KERN_INFO "  |- mcgid_en: %d\n", entry.mcgid_en);
			printf("  |- mcgid_en: %d\n", entry.mcgid_en);
//			printk(KERN_INFO "  |- mcgid: 0x%03x\n", entry.mcgid);
			printf("  |- mcgid: 0x%03x\n", entry.mcgid);

//			printk(KERN_INFO "  |- vlan_fltr_en: %d (%s)\n", entry.vlan_fltr_en,
			printf("  |- vlan_fltr_en: %d (%s)\n", entry.vlan_fltr_en,
					(entry.vlan_fltr_en)? "drop":"forward");
//			printk(KERN_INFO "  |- vlan_cmd: 0x%x (%s)\n", entry.vlan_cmd,
			printf("  |- vlan_cmd: 0x%x (%s)\n", entry.vlan_cmd,
					(entry.vlan_cmd == 1)? "push":
					(entry.vlan_cmd == 2)? "pop all":
					(entry.vlan_cmd == 3)? "swap":
					(entry.vlan_cmd == 4)? "pop": "NOP");
//			printk(KERN_INFO "  |- vid: 0x%03x\n", entry.vid);
			printf("  |- vid: 0x%03x\n", entry.vid);
//			printk(KERN_INFO "  |- sc_ind: %d (%s)\n", entry.sc_ind,
			printf("  |- sc_ind: %d (%s)\n", entry.sc_ind,
					(entry.sc_ind)? "SVLAN":"CVLAN");
//			printk(KERN_INFO "  |- dot1p_cmd: 0x%x (%s)\n", entry.dot1p_cmd,
			printf("  |- dot1p_cmd: 0x%x (%s)\n", entry.dot1p_cmd,
					(entry.dot1p_cmd)? "swap":"old");
//			printk(KERN_INFO "  |- dot1p: 0x%x\n", entry.dot1p);
			printf("  |- dot1p: 0x%x\n", entry.dot1p);
//			printk(KERN_INFO "  |- dei_cmd: 0x%x (%s)\n", entry.dei_cmd,
			printf("  |- dei_cmd: 0x%x (%s)\n", entry.dei_cmd,
					(entry.dei_cmd)? "swap":"old");
//			printk(KERN_INFO "  |- dei: 0x%x\n", entry.dei);
			printf("  |- dei: 0x%x\n", entry.dei);
//			printk(KERN_INFO "  |- mac_cmd: %d (%s)\n", entry.mac_cmd,
			printf("  |- mac_cmd: %d (%s)\n", entry.mac_cmd,
					(entry.mac_cmd)? "swap":"old");
//			printk(KERN_INFO "  |- mac_da: %02x:%02x:%02x:%02x:%02x:%02x\n",
			printf("  |- mac_da: %02x:%02x:%02x:%02x:%02x:%02x\n",
					entry.mac_da[5], entry.mac_da[4],
					entry.mac_da[3], entry.mac_da[2],
					entry.mac_da[1], entry.mac_da[0]);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
#if 0//yocto
			cond_resched();
#else//sd1 uboot for 98f
#endif
		}
	}
}

static aal_table_t l2fe_arb_fib_table= {
	.table_id = AAL_TABLE_ARB_FIB_TBL,
	.table_name = "ARB FIB",
	.max_entry = ARB_FIB_TBL_ENTRY_MAX,
	.used_entry = 0,
	.curr_ptr = 0,
	.entry_size = sizeof(l2fe_arb_fib_tbl_entry_t),
	.access_reg = L2FE_ARB_MC_FIB_TBL_ACCESS,
	.data_reg_num = 4,
	.op = {
		.entry_add = NULL,
		.entry_add_by_idx = aal_arb_fib_entry_add_by_idx,
		.entry_del = aal_arb_fib_entry_del,
		.entry_set = aal_arb_fib_entry_set,
		.entry_get = NULL,
		.entry_get_by_idx = aal_arb_fib_entry_get_by_idx,
		.table_flush = NULL,
		.get_avail_count = NULL,
		.entry_print = aal_arb_fib_entry_print,
	},
};

/***************************************************************************
 * L2FE, IM3E (IGMP Engine)
 ***************************************************************************/

/* IM3E Control */
int aal_im3e_control_set(L2FE_IM3E_CTRL_t *ctrl)
{
	if (ctrl == NULL) return AAL_E_NULLPTR;
	CA_L2FE_REG_WRITE(ctrl->wrd, L2FE_IM3E_CTRL);

	return AAL_E_OK;
}

int aal_im3e_control_get(L2FE_IM3E_CTRL_t *ctrl)
{
	if (ctrl == NULL) return AAL_E_NULLPTR;
	ctrl->wrd = CA_L2FE_REG_READ(L2FE_IM3E_CTRL);

	return AAL_E_OK;
}

/* Because the index of HW entry is jumping by sorting, so we let user to
 * operate the table by the index of SW entry.
 * WTFing HW design!
 */
static int aal_im3e_hw_entry_cmd_set(struct aal_table_s *table, unsigned int idx)
{
	/* No matter what idx is, use ADD cmd instead */
	int i;
	uint32_t val = 0;
	unsigned int *ptr;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;
	if (idx >= table->max_entry) return AAL_E_OUTRANGE;
	if (table->entry[idx].data == NULL) return AAL_E_NULLPTR;

	ptr = table->entry[idx].data;
	/* write the value to the specific data registers */
	for (i = 0; i < table->data_reg_num ; i++)
		CA_NE_REG_WRITE(ptr[i], table->access_reg + ((table->data_reg_num - i) << 2));

	/* start the write access (prepare access register) */
	val |= 1 << 30;		/* set rbw to write */
	val |= 1 << 31;		/* set access to start */
	val |= IM3E_CMD_ADD;
	CA_NE_REG_WRITE(val, table->access_reg);

	/* wait write complete */
	i = 0;
	do {
		val = CA_NE_REG_READ(table->access_reg);
	} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
	if (i >= TABLE_TRY_TIMEOUT)
		return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

	/* Check CMD result */
	if (table->table_id == AAL_TABLE_IM3E_MLDDA)
		val = CA_NE_REG_READ(L2FE_IM3E_STTS1);
	else
		val = CA_NE_REG_READ(L2FE_IM3E_STTS2);
	if (val == 0) {
		printf("CMD Add OK\n");
		return AAL_E_OK;
	} else{
		printf("CMD Add failed\n");
		return AAL_E_ACCESSINCOMP;
	}
}

static int aal_im3e_hw_entry_cmd_clear(aal_table_t *table, unsigned int idx)
{
	int i;
	uint32_t val = 0;
	unsigned int *ptr;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;
	if (idx >= table->max_entry) return AAL_E_OUTRANGE;
	printf("del idx %d, ptr %lx\n", idx, table->entry[idx].data);
	if (table->entry[idx].data == NULL) return AAL_E_NULLPTR;

	ptr = table->entry[idx].data;
	/* write the value to the specific data registers */
	for (i = 0; i < table->data_reg_num ; i++)
		CA_NE_REG_WRITE(ptr[i], table->access_reg + ((table->data_reg_num - i) << 2));

	/* start the write access (prepare access register) */
	val |= 1 << 30;		/* set rbw to write */
	val |= 1 << 31;		/* set access to start */
	val |= IM3E_CMD_DEL;
	CA_NE_REG_WRITE(val, table->access_reg);

	/* wait write complete */
	i = 0;
	do {
		val = CA_NE_REG_READ(table->access_reg);
	} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
	if (i >= TABLE_TRY_TIMEOUT)
		return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

	/* Check CMD result */
	if (table->table_id == AAL_TABLE_IM3E_MLDDA)
		val = CA_NE_REG_READ(L2FE_IM3E_STTS1);
	else
		val = CA_NE_REG_READ(L2FE_IM3E_STTS2);
	if (val == 0) {
		printf("CMD Del OK\n");
		return AAL_E_OK;
	} else {
		printf("CMD Del failed\n");
		return AAL_E_ACCESSINCOMP;
	}
}

/* IMPORTANT: This function is ONLY for debug print!!!!!!!!!!!!!!!!!!!!!!!!
 * DO NOT use on cli or API!!!!!!!!!
 */
static int aal_im3e_hw_entry_cmd_get_dbg(struct aal_table_s *table, unsigned int HW_idx, void *data)
{
	int i, j;
	uint32_t val = 0;
	unsigned int *ptr;

	if (table == NULL || table->entry == NULL) return AAL_E_TBLNOTEXIST;
	if (HW_idx >= table->max_entry) return AAL_E_OUTRANGE;
	if (data == NULL) return AAL_E_NULLPTR;

	/* Get the first entry */
	if (table->curr_ptr == NULL || ((aal_entry_t *)table->curr_ptr)->data == NULL)
		return AAL_E_NULLPTR;

	ptr = ((aal_entry_t *)table->curr_ptr)->data;
	/* write the value to the specific data registers */
	for (i = 0; i < table->data_reg_num ; i++)
		CA_NE_REG_WRITE(ptr[i], table->access_reg + ((table->data_reg_num - i) << 2));

	/* start the read access (prepare access register) */
	val |= 0 << 30;		/* set rbw to read */
	val |= 1 << 31;		/* set access to start */
	val |= IM3E_CMD_SRCH;
	CA_NE_REG_WRITE(val, table->access_reg);

	/* wait write complete */
	i = 0;
	do {
		val = CA_NE_REG_READ(table->access_reg);
	} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
	if (i >= TABLE_TRY_TIMEOUT)
		return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

	/* Check CMD result */
	if (table->table_id == AAL_TABLE_IM3E_MLDDA)
		val = CA_NE_REG_READ(L2FE_IM3E_STTS1);
	else
		val = CA_NE_REG_READ(L2FE_IM3E_STTS2);
	if (val == 0) {
		printf("CMD SRCH OK\n");
	} else{
		printf("CMD SRCH failed\n");
		return AAL_E_ENTRYNOTFOUND;
	}

	/* Move to the entry */
	for (j = 0; j < HW_idx; j++) {
		/* start the read access (prepare access register) */
		val |= 0 << 30;		/* set rbw to read */
		val |= 1 << 31;		/* set access to start */
		val |= IM3E_CMD_SRCH_N;
		CA_NE_REG_WRITE(val, table->access_reg);

		/* wait write complete */
		i = 0;
		do {
			val = CA_NE_REG_READ(table->access_reg);
		} while (((1 << 31) & val) && (++i < TABLE_TRY_TIMEOUT));
		if (i >= TABLE_TRY_TIMEOUT)
			return AAL_E_ACCESSINCOMP;	/* ACCESS can't complete */

		/* Check CMD result */
		if (table->table_id == AAL_TABLE_IM3E_MLDDA)
			val = CA_NE_REG_READ(L2FE_IM3E_STTS1);
		else
			val = CA_NE_REG_READ(L2FE_IM3E_STTS2);
		if (val == 0) {
			printf("CMD SRCH NEXT OK\n");
		} else{
			printf("CMD SRCH NEXT failed\n");
			return AAL_E_ENTRYNOTFOUND;
		}
	}

	/* Return data */
	ptr = data;
	for (i = 0; i < table->data_reg_num; i++)
		ptr[i] = CA_NE_REG_READ(table->access_reg + ((table->data_reg_num - i) << 2));

	return AAL_E_OK;
}

/* Table L2FE.IM3E.MLDDA */
static aal_table_t im3e_mldda_table;

int aal_im3e_mldda_init(void)
{
        L2FE_IM3E_CTRL_t ctrl;
        ca_uint32_t wtfake_vlan[CFG_ID_FAKE_WAN_MC_VID_LEN];
        int i, cnt = 0;
#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        spin_lock_init(&im3e_mldda_table.lock);

        if (im3e_cmd_mode == 1) {
                ctrl.wrd = L2FE_IM3E_CTRL_dft;
                ctrl.bf.mcu_oprt_mode = 0; /* 0:cmd 1:indirect */
                aal_im3e_control_set(&ctrl);
                im3e_mldda_table.hw_entry_set = aal_im3e_hw_entry_cmd_set;
                im3e_mldda_table.hw_entry_clear = aal_im3e_hw_entry_cmd_clear;
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }

#if 0//yocto
	/* Init Fake VLAN here. */
	if (CA_E_OK != scfg_read(0, CFG_ID_FAKE_WAN_MC_VID,
		CFG_ID_FAKE_WAN_MC_VID_LEN * sizeof(ca_uint32_t),
		(ca_uint8_t *) wtfake_vlan)) {
		printf("SCFG read %s failed!!!\n", CFG_ID_FAKE_WAN_MC_VID);
	}
#else//sd1 uboot for 98f
        wtfake_vlan[0] = 0xFEF;
        wtfake_vlan[1] = 0xFF0;
        wtfake_vlan[2] = 0xFF1;
        wtfake_vlan[3] = 0xFF2;
        wtfake_vlan[4] = 0xFF3;
        wtfake_vlan[5] = 0xFF4;
        wtfake_vlan[6] = 0xFF5;
        wtfake_vlan[7] = 0xFF6;
        wtfake_vlan[8] = 0xFF7;
        wtfake_vlan[9] = 0xFF8;
        wtfake_vlan[10] = 0xFF9;
        wtfake_vlan[11] = 0xFFA;
        wtfake_vlan[12] = 0xFFB;
        wtfake_vlan[13] = 0xFFC;
        wtfake_vlan[14] = 0xFFD;
        wtfake_vlan[15] = 0xFFE;
#endif

        for (i = 0; i < CFG_ID_FAKE_WAN_MC_VID_LEN; i++){
                if (wtfake_vlan[i] != CA_UINT16_INVALID){
                        fake_vlan_pool[cnt].valid = 1;
                        fake_vlan_pool[cnt].fake_vid = wtfake_vlan[i];

                        cnt++;
                }
        }

        return __aal_table_register(AAL_TABLE_IM3E_MLDDA, &im3e_mldda_table);
}

void aal_im3e_mldda_exit(void)
{
	__aal_table_unregister(AAL_TABLE_IM3E_MLDDA);
}

int aal_im3e_mldda_set_valid_count(void)
{
	aal_table_t *table = &im3e_mldda_table;

	CA_L2FE_REG_WRITE(table->used_entry, L2FE_IM3E_MLDDA_TBL_INDRCT_ACCESS);

	return AAL_E_OK;
}

int aal_im3e_mldda_get_valid_count(ca_uint32_t *count)
{
	if (count == NULL) return AAL_E_NULLPTR;

	*count = CA_L2FE_REG_READ(L2FE_IM3E_MLDDA_TBL_INDRCT_ACCESS);

	return AAL_E_OK;
}

int aal_im3e_mldda_entry_add(void *entry, unsigned int *rslt_idx)
{
	aal_table_t *table = &im3e_mldda_table;
	unsigned long flags;
	im3e_mldda_entry_t *orig_data, *data = entry;
	int i, ret;

	if ((entry == NULL) || (rslt_idx == NULL) || (table->entry == NULL)) return AAL_E_NULLPTR;

	spin_lock_irqsave(&table->lock, flags);

	for (i = 0; i < table->max_entry; i++) {
		orig_data = table->entry[i].data;
		if (orig_data == NULL || orig_data->entry_valid == 0) continue;

		if (   (orig_data->ipv6_da_0 == data->ipv6_da_0)
		    && (orig_data->ipv6_da_1 == data->ipv6_da_1)
		    && (orig_data->ipv6_da_2 == data->ipv6_da_2)
		    && (orig_data->ipv6_da_3 == data->ipv6_da_3)
		    && (orig_data->v3_ind == data->v3_ind)
		    && (orig_data->vid == data->vid)
		    && (orig_data->vlan_sc_ind == data->vlan_sc_ind)) {
			return AAL_E_EXIST;
		}
	}

	spin_unlock_irqrestore(&table->lock, flags);

	ret = generic_entry_add(&im3e_mldda_table, entry, rslt_idx);

	/* For indirect access mode, update the valid entry number. */
	if (ret == AAL_E_OK && im3e_cmd_mode == 0)
		aal_im3e_mldda_set_valid_count();
	/* TODO: error handle -> free data */

	return ret;
}

int aal_im3e_mldda_entry_add_by_idx(void *entry, unsigned int idx)
{
	int ret;

	ret = generic_entry_add_by_idx(&im3e_mldda_table, entry, idx);

	/* For indirect access mode, update the valid entry number. */
	if (ret == AAL_E_OK && im3e_cmd_mode == 0)
		aal_im3e_mldda_set_valid_count();

	return ret;
}

int aal_im3e_mldda_entry_del(unsigned int idx)
{
	int ret;

	ret = generic_entry_del(&im3e_mldda_table, idx);

	/* For indirect access mode, update the valid entry number. */
	if (ret == AAL_E_OK && im3e_cmd_mode == 0)
		aal_im3e_mldda_set_valid_count();

	return ret;
}

int aal_im3e_mldda_entry_set(unsigned int idx, void *entry)
{
	int ret;

	if (im3e_cmd_mode == 0) {
		return generic_entry_set(&im3e_mldda_table, idx, entry);
	} else {
		/* Delete then add */
		ret = aal_im3e_mldda_entry_del(idx);
		if (ret == AAL_E_OK)
			ret = aal_im3e_mldda_entry_add_by_idx(entry, idx);
		return ret;
	}
}

int aal_im3e_mldda_entry_get(void *entry, unsigned int *rslt_idx)
{
	return generic_entry_get(&im3e_mldda_table, entry, rslt_idx);
}

int aal_im3e_mldda_entry_get_by_idx(unsigned int idx, void *entry)
{
	return generic_entry_get_by_idx(&im3e_mldda_table, idx, entry);
}

int aal_im3e_mldda_table_flush(void)
{
	return generic_table_flush(&im3e_mldda_table);
}

int aal_im3e_mldda_get_avail_count(void)
{
	return generic_get_avail_count(&im3e_mldda_table);
}

void aal_im3e_mldda_entry_print(unsigned int start_idx, int count)
{
	aal_table_t *table = &im3e_mldda_table;
	im3e_mldda_entry_t entry;
	int end_idx;
	int i, ret;
	unsigned int refcnt;

	if (start_idx >= table->max_entry) {
//		printk(KERN_ERR "%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		printf("%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		return;
	}

	if ((count < 0) || (start_idx + count > table->max_entry)) {
		end_idx = table->max_entry;
	} else {
		end_idx = start_idx + count;
	}

//	printk(KERN_INFO "|===== MLD DA Table ==================================================|\n");
	printf("|===== MLD DA Table ==================================================|\n");

	for (i = start_idx; i < end_idx; i++) {
		if (ca_aal_table_sw_cache) {
			ret = aal_im3e_mldda_entry_get_by_idx(i, &entry);
		} else {
			if (im3e_cmd_mode == 1) {
				//ret = aal_im3e_hw_entry_cmd_get_dbg(&im3e_mldda_table, i, &entry);
				ret = aal_im3e_mldda_entry_get_by_idx(i, &entry);
			} else
				ret = hw_entry_get(&im3e_mldda_table, i, &entry);
		}
		if ((ret == AAL_E_ENTRYNOTRSVD) || (ret == AAL_E_ENTRYNOTFOUND) || (ret == AAL_E_NULLPTR)) {
//			printk(KERN_INFO "| index %04d | NOT USED\n", i);
			printf("| index %04d | NOT USED\n", i);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
		} else {
			refcnt = atomic_read(&table->entry[i].refcnt);

//			printk(KERN_INFO "| index %04d | ref count %d\n", i, refcnt);
			printf("| index %04d | ref count %d\n", i, refcnt);
//			printk(KERN_INFO "  |- entry_valid %01x, mc_group_id(10) 0x%03x, connection_id(8) 0x%02x,v3_ind %01x\n",
			printf("  |- entry_valid %01x, mc_group_id(10) 0x%03x, connection_id(8) 0x%02x,v3_ind %01x\n",
			    entry.entry_valid, entry.mc_group_id, entry.connection_id, entry.v3_ind);
//			printk(KERN_INFO "  |- ipv6_da %08x %08x %08x %08x\n", entry.ipv6_da_3, entry.ipv6_da_2, entry.ipv6_da_1, entry.ipv6_da_0);
			printf("  |- ipv6_da %08x %08x %08x %08x\n", entry.ipv6_da_3, entry.ipv6_da_2, entry.ipv6_da_1, entry.ipv6_da_0);
//			printk(KERN_INFO "  |- vid(12) 0x%03x, vlan_sc_ind %01x\n", entry.vid, entry.vlan_sc_ind);
			printf("  |- vid(12) 0x%03x, vlan_sc_ind %01x\n", entry.vid, entry.vlan_sc_ind);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
#if 0//yocto
			cond_resched();
#else//sd1 uboot for 98f
#endif
		}
	}
}

static aal_table_t im3e_mldda_table= {
	.table_id = AAL_TABLE_IM3E_MLDDA,
	.table_name = "IM3E MLD_DA",
	.max_entry = IM3E_MLDDA_ENTRY_MAX,
	.used_entry = 0,
	.curr_ptr = 0,
	.entry_size = sizeof(im3e_mldda_entry_t),
	.access_reg = L2FE_IM3E_MLDDA_ACCESS,
	.data_reg_num = 6,
	.op = {
		.entry_add = aal_im3e_mldda_entry_add,
		.entry_add_by_idx = aal_im3e_mldda_entry_add_by_idx,
		.entry_del = aal_im3e_mldda_entry_del,
		.entry_set = aal_im3e_mldda_entry_set,
		.entry_get = aal_im3e_mldda_entry_get,
		.entry_get_by_idx = aal_im3e_mldda_entry_get_by_idx,
		.table_flush = aal_im3e_mldda_table_flush,
		.get_avail_count = aal_im3e_mldda_get_avail_count,
		.entry_print = aal_im3e_mldda_entry_print,
	},
};

/* Table L2FE.IM3E.MLDSA */
static aal_table_t im3e_mldsa_table;

int aal_im3e_mldsa_init(void)
{
        spin_lock_init(&im3e_mldsa_table.lock);

#if CONFIG_98F_UBOOT_NE_DBG
        ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

        if (im3e_cmd_mode == 1) {
                im3e_mldsa_table.hw_entry_set = aal_im3e_hw_entry_cmd_set;
                im3e_mldsa_table.hw_entry_clear = aal_im3e_hw_entry_cmd_clear;
#if CONFIG_98F_UBOOT_NE_DBG
                ca_printf("\t%s(%d)\n", __FUNCTION__, __LINE__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
        }

        return __aal_table_register(AAL_TABLE_IM3E_MLDSA, &im3e_mldsa_table);
}

void aal_im3e_mldsa_exit(void)
{
	__aal_table_unregister(AAL_TABLE_IM3E_MLDSA);
}

int aal_im3e_mldsa_set_valid_count(void)
{
	aal_table_t *table = &im3e_mldsa_table;

	CA_L2FE_REG_WRITE(table->used_entry, L2FE_IM3E_MLDSA_TBL_INDRCT_ACCESS);

	return AAL_E_OK;
}

int aal_im3e_mldsa_get_valid_count(ca_uint32_t *count)
{
	if (count == NULL) return AAL_E_NULLPTR;

	*count = CA_L2FE_REG_READ(L2FE_IM3E_MLDSA_TBL_INDRCT_ACCESS);

	return AAL_E_OK;
}

int aal_im3e_mldsa_entry_add(void *entry, unsigned int *rslt_idx)
{
	aal_table_t *table = &im3e_mldsa_table;
	unsigned long flags;
	im3e_mldsa_entry_t *orig_data, *data = entry;
	int i, ret;

	if ((entry == NULL) || (rslt_idx == NULL) || (table->entry == NULL)) return AAL_E_NULLPTR;

	spin_lock_irqsave(&table->lock, flags);

	for (i = 0; i < table->max_entry; i++) {
		orig_data = table->entry[i].data;
		if (orig_data == NULL || orig_data->entry_valid == 0) continue;

		if (   (orig_data->connection_id == data->connection_id)
		    && (orig_data->ipv6_sa_0 == data->ipv6_sa_0)
		    && (orig_data->ipv6_sa_1 == data->ipv6_sa_1)
		    && (orig_data->ipv6_sa_2 == data->ipv6_sa_2)
		    && (orig_data->ipv6_sa_3 == data->ipv6_sa_3)) {
			return AAL_E_EXIST;	// TODO: only compare these?
		}
	}

	spin_unlock_irqrestore(&table->lock, flags);

	ret = generic_entry_add(&im3e_mldsa_table, entry, rslt_idx);

	/* For indirect access mode, update the valid entry number. */
	if (ret == AAL_E_OK && im3e_cmd_mode == 0)
		aal_im3e_mldsa_set_valid_count();

	return ret;
}

int aal_im3e_mldsa_entry_add_by_idx(void *entry, unsigned int idx)
{
	int ret;

	ret = generic_entry_add_by_idx(&im3e_mldsa_table, entry, idx);

	/* For indirect access mode, update the valid entry number. */
	if (ret == AAL_E_OK && im3e_cmd_mode == 0)
		aal_im3e_mldsa_set_valid_count();

	return ret;
}

int aal_im3e_mldsa_entry_del(unsigned int idx)
{
	int ret;

	ret = generic_entry_del(&im3e_mldsa_table, idx);

	/* For indirect access mode, update the valid entry number. */
	if (ret == AAL_E_OK && im3e_cmd_mode == 0)
		aal_im3e_mldsa_set_valid_count();

	return ret;
}

int aal_im3e_mldsa_entry_set(unsigned int idx, void *entry)
{
	int ret;

	if (im3e_cmd_mode == 0) {
		return generic_entry_set(&im3e_mldsa_table, idx, entry);
	} else {
		/* Delete then add */
		ret = aal_im3e_mldsa_entry_del(idx);
		if (ret == AAL_E_OK)
			ret = aal_im3e_mldsa_entry_add_by_idx(entry, idx);
		return ret;
	}
}

int aal_im3e_mldsa_entry_get(void *entry, unsigned int *rslt_idx)
{
	return generic_entry_get(&im3e_mldsa_table, entry, rslt_idx);
}

int aal_im3e_mldsa_entry_get_by_idx(unsigned int idx, void *entry)
{
	return generic_entry_get_by_idx(&im3e_mldsa_table, idx, entry);
}

int aal_im3e_mldsa_table_flush(void)
{
	return generic_table_flush(&im3e_mldsa_table);
}

int aal_im3e_mldsa_get_avail_count(void)
{
	return generic_get_avail_count(&im3e_mldsa_table);
}

void aal_im3e_mldsa_entry_print(unsigned int start_idx, int count)
{
	aal_table_t *table = &im3e_mldsa_table;
	im3e_mldsa_entry_t entry;
	int end_idx;
	int i, ret;
	unsigned int refcnt;

	if (start_idx >= table->max_entry) {
//		printk(KERN_ERR "%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		printf("%s: Out of range! Max is %u, but start_idx is %u.\n", __func__, table->max_entry, start_idx);
		return;
	}

	if ((count < 0) || (start_idx + count > table->max_entry)) {
		end_idx = table->max_entry;
	} else {
		end_idx = start_idx + count;
	}

//	printk(KERN_INFO "|===== MLD SA Table ==================================================|\n");
	printf("|===== MLD SA Table ==================================================|\n");

	for (i = start_idx; i < end_idx; i++) {
		if (ca_aal_table_sw_cache) {
			ret = aal_im3e_mldsa_entry_get_by_idx(i, &entry);
		} else {
			if (im3e_cmd_mode == 1) {
				//ret = aal_im3e_hw_entry_cmd_get_dbg(&im3e_mldsa_table, i, &entry);
				ret = aal_im3e_mldsa_entry_get_by_idx(i, &entry);
			} else
				ret = hw_entry_get(&im3e_mldsa_table, i, &entry);
		}
		if ((ret == AAL_E_ENTRYNOTRSVD) || (ret == AAL_E_ENTRYNOTFOUND) || (ret == AAL_E_NULLPTR)) {
//			printk(KERN_INFO "| index %04d | NOT USED\n", i);
			printf("| index %04d | NOT USED\n", i);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
		} else {
			refcnt = atomic_read(&table->entry[i].refcnt);

//			printk(KERN_INFO "| index %04d | ref count %d\n", i, refcnt);
			printf("| index %04d | ref count %d\n", i, refcnt);
//			printk(KERN_INFO "  |- entry_valid %01x, mc_group_id(10) 0x%02x, connection_id(8) 0x%02x\n",
			printf("  |- entry_valid %01x, mc_group_id(10) 0x%02x, connection_id(8) 0x%02x\n",
			    entry.entry_valid, entry.mc_group_id, entry.connection_id);
//			printk(KERN_INFO "  |- ipv6_sa %08x %08x %08x %08x\n", entry.ipv6_sa_3, entry.ipv6_sa_2, entry.ipv6_sa_1, entry.ipv6_sa_0);
			printf("  |- ipv6_sa %08x %08x %08x %08x\n", entry.ipv6_sa_3, entry.ipv6_sa_2, entry.ipv6_sa_1, entry.ipv6_sa_0);
//			printk(KERN_INFO "|------------------------------------------------------------------|\n");
			printf("|------------------------------------------------------------------|\n");
#if 0//yocto
			cond_resched();
#else//sd1 uboot for 98f
#endif
		}
	}
}

static aal_table_t im3e_mldsa_table= {
	.table_id = AAL_TABLE_IM3E_MLDSA,
	.table_name = "IM3E MLD_SA",
	.max_entry = IM3E_MLDSA_ENTRY_MAX,
	.used_entry = 0,
	.curr_ptr = 0,
	.entry_size = sizeof(im3e_mldsa_entry_t),
	.access_reg = L2FE_IM3E_MLDSA_ACCESS,
	.data_reg_num = 5,
	.op = {
		.entry_add = aal_im3e_mldsa_entry_add,
		.entry_add_by_idx = aal_im3e_mldsa_entry_add_by_idx,
		.entry_del = aal_im3e_mldsa_entry_del,
		.entry_set = aal_im3e_mldsa_entry_set,
		.entry_get = aal_im3e_mldsa_entry_get,
		.entry_get_by_idx = aal_im3e_mldsa_entry_get_by_idx,
		.table_flush = aal_im3e_mldsa_table_flush,
		.get_avail_count = aal_im3e_mldsa_get_avail_count,
		.entry_print = aal_im3e_mldsa_entry_print,
	},
};

/* AAL helper funcitons */
/**
 * Return an availabe MCE entry index.
 * Return -1 when MCE is full.
 */
ca_int16_t aal_mc_mcgid_alloc()
{
	ca_int16_t i;

	for (i = 0; i < MCE_INDX_TBL_ENTRY_MAX; i++) {
		if (free_mcgid_pool[i].used == 0) {
			free_mcgid_pool[i].used = 1;
			free_mcgid_pool[i].l2ptr = free_mcgid_pool[i].l3ptr = NULL;
			return (ca_int16_t)(i);
		}
	}
	return -1;
}

void aal_mc_mcgid_set_callback(ca_uint16_t mcgid, mcgid_type_t type, ca_int8_t *ptr)
{
	if (free_mcgid_pool[mcgid].used) {
		if (type == MC_L2)
			free_mcgid_pool[mcgid].l2ptr = ptr;
		if (type == MC_L3)
			free_mcgid_pool[mcgid].l3ptr = ptr;
	}
}

ca_int8_t* aal_mc_mcgid_ptr_get(ca_uint16_t mcgid, mcgid_type_t type)
{
	if (mcgid >= MCE_INDX_TBL_ENTRY_MAX)
		return NULL;

	if (free_mcgid_pool[mcgid].used) {
		if (type == MC_L2)
			return free_mcgid_pool[mcgid].l2ptr;
		else if (type == MC_L3)
			return free_mcgid_pool[mcgid].l3ptr;
	}
	return NULL;
}

/**
 * Return a FAKE MCE entry index.
 * Return -1 when MCE is full. Yes! Even it's fake.
 */
ca_int16_t aal_mc_fake_mcgid_alloc(void)
{
	ca_int16_t i;

	for (i = 0; i < MCE_INDX_TBL_ENTRY_MAX; i++) {
		if (fake_mcgid_pool[i].used == 0) {
			fake_mcgid_pool[i].used = 1;
			fake_mcgid_pool[i].l2ptr = fake_mcgid_pool[i].l3ptr = NULL;
			return (ca_int16_t)(i);
		}
	}
	return -1;
}

void aal_mc_fake_mcgid_set_callback(ca_uint16_t mcgid, mcgid_type_t type, ca_int8_t *ptr)
{
	if (fake_mcgid_pool[mcgid].used) {
		if (type == MC_L2)
			fake_mcgid_pool[mcgid].l2ptr = ptr;
		if (type == MC_L3)
			fake_mcgid_pool[mcgid].l3ptr = ptr;
	}
}

ca_int8_t* aal_mc_fake_mcgid_ptr_get(ca_uint16_t mcgid, mcgid_type_t type)
{
	if (mcgid >= MCE_INDX_TBL_ENTRY_MAX)
		return NULL;

	if (fake_mcgid_pool[mcgid].used) {
		if (type == MC_L2)
			return fake_mcgid_pool[mcgid].l2ptr;
		else if (type == MC_L3)
			return fake_mcgid_pool[mcgid].l3ptr;
	}
	return NULL;
}

ca_status_t aal_mc_mcgid_free(ca_int16_t mcgid)
{
	if (mcgid < 0 || mcgid >= MCE_INDX_TBL_ENTRY_MAX || free_mcgid_pool[mcgid].used == 0)
		return CA_E_ERROR;

	free_mcgid_pool[mcgid].used = 0;
	free_mcgid_pool[mcgid].l2ptr = NULL;
	free_mcgid_pool[mcgid].l3ptr = NULL;
	return CA_E_OK;
}

ca_status_t aal_mc_fake_mcgid_free(ca_int16_t mcgid)
{
	if (mcgid < 0 || mcgid >= MCE_INDX_TBL_ENTRY_MAX || fake_mcgid_pool[mcgid].used == 0)
		return CA_E_ERROR;

	fake_mcgid_pool[mcgid].used = 0;
	fake_mcgid_pool[mcgid].l2ptr = NULL;
	fake_mcgid_pool[mcgid].l3ptr = NULL;
	return CA_E_OK;
}

/**
 * Return an available replication bit (ARB MC FIB entry).
 * The replication bit is unique to the interface(port and vlan).
 * Return -1 when the ARB MC FIB is full.
 */
ca_int16_t aal_mc_repl_bit_alloc(repl_key_t *key)
{
	ca_int16_t i, free_idx = ARB_FIB_TBL_ENTRY_MAX;
	l2fe_arb_fib_tbl_entry_t arb_fib_entry = {0};

	for (i = 0; i < ARB_FIB_TBL_ENTRY_MAX; i++) {
		if (!free_repl_bits_pool[i].ref_cnt) {
			if (free_idx == ARB_FIB_TBL_ENTRY_MAX)
				free_idx = i; /* The first free bit */
		} else if (memcmp(&free_repl_bits_pool[i].key, key, sizeof(repl_key_t)) == 0) {
			free_repl_bits_pool[i].ref_cnt++;
			return i;
		}
	}

	/* Full */
	if (free_idx == ARB_FIB_TBL_ENTRY_MAX)
		return -1;

	/* Not found => Allocate */
	free_repl_bits_pool[free_idx].ref_cnt = 1;
	memcpy(&free_repl_bits_pool[free_idx].key, key, sizeof(repl_key_t));

	/* Set to ARB FIB directly */
	arb_fib_entry.mcgid = 0;
	arb_fib_entry.mcgid_en = 0;
	arb_fib_entry.pol_id = 0;
	arb_fib_entry.pol_grp_id = 0;
	arb_fib_entry.pol_en = 0;
	arb_fib_entry.cos = 0;
	arb_fib_entry.ldpid = key->ldpid;
	arb_fib_entry.mac_sa_sel = L2_PE_MAC_SA_LAN_IDX;
	arb_fib_entry.mac_sa_cmd = key->mac_sa_cmd;
	arb_fib_entry.permit_spid_en = 0;
	arb_fib_entry.wan_dst = (arb_fib_entry.ldpid == AAL_LPORT_ETH_WAN);
	arb_fib_entry.cos_cmd = 1;
	arb_fib_entry.rsvd = 0;

	arb_fib_entry.mac_cmd = key->mac_da_cmd;
	arb_fib_entry.mac_da[0] = key->mac_da[5];
	arb_fib_entry.mac_da[1] = key->mac_da[4];
	arb_fib_entry.mac_da[2] = key->mac_da[3];
	arb_fib_entry.mac_da[3] = key->mac_da[2];
	arb_fib_entry.mac_da[4] = key->mac_da[1];
	arb_fib_entry.mac_da[5] = key->mac_da[0];

	if (key->vlan_cmd) {
		arb_fib_entry.dei = (key->vlan & 0x1000) >> 12;
		arb_fib_entry.dei_cmd = 1;
		arb_fib_entry.dot1p = (key->vlan & 0xE000) >> 13;
		arb_fib_entry.dot1p_cmd = 1;
		arb_fib_entry.sc_ind = 0;
		arb_fib_entry.vid = (key->vlan & 0xFFF);
		arb_fib_entry.vlan_cmd = key->vlan_cmd;
	} else {
		arb_fib_entry.dei = 0;
		arb_fib_entry.dei_cmd = 0;
		arb_fib_entry.dot1p = 0;
		arb_fib_entry.dot1p_cmd = 0;
		arb_fib_entry.sc_ind = 0;
		arb_fib_entry.vid = 0;
		arb_fib_entry.vlan_cmd = 0;
	}
	arb_fib_entry.vlan_fltr_en = 0;
	aal_arb_fib_entry_add_by_idx(&arb_fib_entry, free_idx);

	return free_idx;
}

ca_int16_t aal_mc_repl_bit_get(repl_key_t *key)
{
	ca_int16_t i;

	for (i = 0; i < ARB_FIB_TBL_ENTRY_MAX; i++) {
		if (free_repl_bits_pool[i].ref_cnt &&
			memcmp(&free_repl_bits_pool[i].key, key, sizeof(repl_key_t)) == 0) {
			return i;
		}
	}

	return -1;
}

ca_status_t aal_mc_repl_bit_free(ca_int16_t port_bit)
{
	if (port_bit < 0 || port_bit >= ARB_FIB_TBL_ENTRY_MAX)
		return CA_E_ERROR;

	if (free_repl_bits_pool[port_bit].ref_cnt > 0) {
		free_repl_bits_pool[port_bit].ref_cnt--;
	}
	if (free_repl_bits_pool[port_bit].ref_cnt == 0) {
		aal_arb_fib_entry_del(port_bit);
	}

	return CA_E_OK;
}

ca_int16_t aal_flooding_group_create()
{
	ca_int16_t mcgid;
	ni_mce_indx_tbl_entry_t repl_bits = {0};

	mcgid = aal_mc_mcgid_alloc();
	if (mcgid < 0)
		return mcgid;

	/* Add an empty MCE entry */
	aal_mce_indx_entry_add_by_idx(&repl_bits, mcgid);

	return mcgid;
}

ca_status_t aal_flooding_group_delete(ca_int16_t mcgid)
{
	ca_status_t ret;

	ret = aal_mc_mcgid_free(mcgid);

	if (ret == CA_E_OK)
		aal_mce_indx_entry_del(mcgid);

	return ret;
}

ca_status_t aal_flooding_port_add(ca_int16_t mcgid, ca_int16_t ldpid)
{
	ca_int16_t port_bit;
	repl_key_t key = {0};
	ni_mce_indx_tbl_entry_t repl_bits = {0};

	if (AAL_E_OK != aal_mce_indx_entry_get_by_idx(mcgid, &repl_bits))
		return CA_E_NOT_FOUND;

	key.ldpid = ldpid;
	port_bit = aal_mc_repl_bit_alloc(&key);
	if (port_bit < 0)
		return CA_E_FULL;
	if (repl_bits.mc_vec & ((ca_uint64_t)1 << port_bit))
		return CA_E_EXISTS;

	/* Update the port bit for mcgid */
	repl_bits.mc_vec |= ((ca_uint64_t)1 << port_bit);
	aal_mce_indx_entry_set(mcgid, &repl_bits);

	return CA_E_OK;
}

ca_status_t aal_flooding_port_delete(ca_int16_t mcgid, ca_int16_t ldpid)
{
	ca_int16_t port_bit;
	repl_key_t key = {0};
	ni_mce_indx_tbl_entry_t repl_bits = {0};

	if (AAL_E_OK != aal_mce_indx_entry_get_by_idx(mcgid, &repl_bits))
		return CA_E_NOT_FOUND;

	key.ldpid = ldpid;
	port_bit = aal_mc_repl_bit_get(&key);
	if (port_bit < 0)
		return CA_E_NOT_FOUND;
	if (!(repl_bits.mc_vec & ((ca_uint64_t)1 << port_bit)))
		return CA_E_NOT_FOUND;

	aal_mc_repl_bit_free(port_bit);

	/* Update the port bit for mcgid */
	repl_bits.mc_vec ^= ((ca_uint64_t)1 << port_bit);
	aal_mce_indx_entry_set(mcgid, &repl_bits);

	return CA_E_OK;
}

ca_int16_t aal_flooding_fib_idx_alloc(void)
{
	ca_int16_t i;
	l2fe_arb_fib_tbl_entry_t arb_fib_entry = {0};

	for (i = 0; i < ARB_FIB_TBL_ENTRY_MAX; i++) {
		if (!free_repl_bits_pool[i].ref_cnt) { /* The first free bit */
			free_repl_bits_pool[i].ref_cnt = 1;
			memset(&free_repl_bits_pool[i].key, 0, sizeof(repl_key_t));
			/* Set to ARB FIB directly */
			aal_arb_fib_entry_add_by_idx(&arb_fib_entry, i);
			return i;
		}
	}

	return -1;
}

ca_status_t aal_flooding_fib_idx_free(ca_int16_t mc_fib_idx)
{
	if (mc_fib_idx < 0 || mc_fib_idx >= ARB_FIB_TBL_ENTRY_MAX)
		return CA_E_NOT_FOUND;

	if (free_repl_bits_pool[mc_fib_idx].ref_cnt > 0) {
		free_repl_bits_pool[mc_fib_idx].ref_cnt = 0;
		/* delete arb fib */
		aal_arb_fib_entry_del(mc_fib_idx);
		return CA_E_OK;
	}

	return CA_E_NOT_FOUND;
}

ca_status_t aal_flooding_mc_fib_add(ca_int16_t mcgid, ca_int16_t mc_fib_idx)
{
	ni_mce_indx_tbl_entry_t repl_bits = {0};

	if (AAL_E_OK != aal_mce_indx_entry_get_by_idx(mcgid, &repl_bits))
		return CA_E_NOT_FOUND;

	if (mc_fib_idx < 0)
		return CA_E_NOT_FOUND;

	if (repl_bits.mc_vec & ((ca_uint64_t)1 << mc_fib_idx))
		return CA_E_EXISTS;

	/* Update the port bit for mcgid */
	repl_bits.mc_vec |= ((ca_uint64_t)1 << mc_fib_idx);
	aal_mce_indx_entry_set(mcgid, &repl_bits);

	return CA_E_OK;
}

ca_status_t aal_flooding_mc_fib_delete(ca_int16_t mcgid, ca_int16_t mc_fib_idx)
{
	ni_mce_indx_tbl_entry_t repl_bits = {0};

	if (AAL_E_OK != aal_mce_indx_entry_get_by_idx(mcgid, &repl_bits))
		return CA_E_NOT_FOUND;

	if (mc_fib_idx < 0)
		return CA_E_NOT_FOUND;

	if (!(repl_bits.mc_vec & ((ca_uint64_t)1 << mc_fib_idx)))
		return CA_E_NOT_FOUND;

	/* Update the port bit for mcgid */
	repl_bits.mc_vec ^= ((ca_uint64_t)1 << mc_fib_idx);
	aal_mce_indx_entry_set(mcgid, &repl_bits);

	return CA_E_OK;
}

aal_direct_table_t l2pe_my_mac_table = {
	.max_entry = 2,
	.first_reg = L2FE_PE_MY_MAC_LOW_0,
	.reg_num = 2,
	.reg_stride = 8
};

ca_status_t aal_l2fe_pe_my_mac_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l2fe_pe_my_mac_tbl_entry_t *entry
)
{
	return aal_direct_table_set(&l2pe_my_mac_table, entry_idx, entry, AAL_TABLE_DIR_LOW_TO_HI);
}

/* AAL CLI debug functions */
ca_status_t aal_mc_ni_mce_indx_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT ni_mce_indx_tbl_entry_t *entry
)
{
	return hw_entry_get(&ni_mce_indx_table, entry_idx, entry);
};

ca_status_t aal_mc_ni_mce_indx_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN ni_mce_indx_tbl_entry_t *entry
)
{
	ni_mce_indx_tbl_entry_t tmp_entry;

	if (AAL_E_OK != aal_mce_indx_entry_get_by_idx(entry_idx, &tmp_entry))
		return aal_mce_indx_entry_add_by_idx(entry, entry_idx);
	else
		return aal_mce_indx_entry_set(entry_idx, entry);
};

ca_status_t aal_mc_l2fe_arb_fib_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT l2fe_arb_fib_tbl_entry_t *entry
)
{
	return hw_entry_get(&l2fe_arb_fib_table, entry_idx, entry);
};

ca_status_t aal_mc_l2fe_arb_fib_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN l2fe_arb_fib_tbl_entry_t *entry
)
{
	l2fe_arb_fib_tbl_entry_t tmp_entry;

	if (AAL_E_OK != aal_arb_fib_entry_get_by_idx(entry_idx, &tmp_entry))
		return aal_arb_fib_entry_add_by_idx(entry, entry_idx);
	else
		return aal_arb_fib_entry_set(entry_idx, entry);
};

ca_status_t aal_mc_im3e_mldda_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT im3e_mldda_entry_t *entry
)
{
	int ret;

	ret = aal_im3e_mldda_entry_get_by_idx(entry_idx, entry);
	if (ret != AAL_E_OK)
		memset(entry, 0, sizeof(im3e_mldda_entry_t));

	return AAL_E_OK;
};

ca_status_t aal_mc_im3e_mldda_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN im3e_mldda_entry_t *entry
)
{
	im3e_mldda_entry_t tmp_entry;

	if (AAL_E_OK != aal_im3e_mldda_entry_get_by_idx(entry_idx, &tmp_entry)) {
		return aal_im3e_mldda_entry_add_by_idx(entry, entry_idx);
	} else {
		if (entry->entry_valid)
			return aal_im3e_mldda_entry_set(entry_idx, entry);
		else
			return aal_im3e_mldda_entry_del(entry_idx);
	}
};

ca_status_t aal_mc_im3e_mldsa_get(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_OUT im3e_mldsa_entry_t *entry
)
{
	int ret;

	ret = aal_im3e_mldsa_entry_get_by_idx(entry_idx, entry);
	if (ret != AAL_E_OK)
		memset(entry, 0, sizeof(im3e_mldsa_entry_t));

	return AAL_E_OK;
};

ca_status_t aal_mc_im3e_mldsa_set(
	CA_IN ca_device_id_t dev,
	CA_IN ca_uint16_t entry_idx,
	CA_IN im3e_mldsa_entry_t *entry
)
{
	im3e_mldsa_entry_t tmp_entry;

	if (AAL_E_OK != aal_im3e_mldsa_entry_get_by_idx(entry_idx, &tmp_entry)) {
		return aal_im3e_mldsa_entry_add_by_idx(entry, entry_idx);
	} else {
		if (entry->entry_valid)
			return aal_im3e_mldsa_entry_set(entry_idx, entry);
		else
			return aal_im3e_mldsa_entry_del(entry_idx);
	}
};

EXPORT_SYMBOL(aal_mc_ni_mce_indx_get);
EXPORT_SYMBOL(aal_mc_ni_mce_indx_set);
EXPORT_SYMBOL(aal_mc_l2fe_arb_fib_get);
EXPORT_SYMBOL(aal_mc_l2fe_arb_fib_set);
EXPORT_SYMBOL(aal_mc_im3e_mldda_get);
EXPORT_SYMBOL(aal_mc_im3e_mldda_set);
EXPORT_SYMBOL(aal_mc_im3e_mldsa_get);
EXPORT_SYMBOL(aal_mc_im3e_mldsa_set);
