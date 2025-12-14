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
 * aal_l2_cls.c: Hardware Abstraction Layer for vlan driver to access hardware regsiters
 *
 */
#include <generated/ca_ne_autoconf.h>
#include "aal_l2_cls.h"
#include "aal_common.h"
#include "osal_common.h"

#if defined(CONFIG_ARCH_CORTINA_G3LITE)
#define __AAL_L2_CLS_MAX_KEY_ENTRY_ID 63
#define __AAL_L2_CLS_MAX_FIB_ENTRY_ID 63

#define __AAL_L2_CLS_MAX_KEY_EGR_ENTRY_ID 63
#define __AAL_L2_CLS_MAX_FIB_EGR_ENTRY_ID 63
#endif

extern APB0_t *g_g3_dev_ptr;

#define AAL_L2FE_CLE_REG_ADDR(reg) L2FE_CLE_##reg

#if 0//yocto
#define __L2_CLS_LOCK()    \
    do{ \
        __l2_cls_lock();\
    }while(0)

#define __L2_CLS_UNLOCK()    \
    do{ \
        __l2_cls_unlock();\
    }while(0)
ca_uint32 g_l2_cls_lock;
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

extern ca_boolean_t __g_l2_cls_debug;

#if 0//yocto
void  __l2_cls_lock(void)
{
    ca_spin_lock(g_l2_cls_lock);
}

void  __l2_cls_unlock(void)
{
    ca_spin_unlock(g_l2_cls_lock);
}
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

typedef struct{
    L2FE_CLE_IGR_KEY_DATA0_t key_data0;
    L2FE_CLE_IGR_KEY_DATA1_t key_data1;
    L2FE_CLE_IGR_KEY_DATA2_t key_data2;
    L2FE_CLE_IGR_KEY_DATA3_t key_data3;
    L2FE_CLE_IGR_KEY_DATA4_t key_data4;
    L2FE_CLE_IGR_KEY_DATA5_t key_data5;
    L2FE_CLE_IGR_KEY_DATA6_t key_data6;
    L2FE_CLE_IGR_KEY_DATA7_t key_data7;
    L2FE_CLE_IGR_KEY_DATA8_t key_data8;
    L2FE_CLE_IGR_KEY_DATA9_t key_data9;
    L2FE_CLE_IGR_KEY_DATA10_t key_data10;
    L2FE_CLE_IGR_KEY_DATA11_t key_data11;
    L2FE_CLE_IGR_KEY_DATA12_t key_data12;
}__aal_cls_key_data_t;

static ca_status_t __aal_l2_cls_ingr_key_data_read(
    ca_device_id_t             device_id,
    ca_uint16               entry_id,
    __aal_cls_key_data_t    *cls_key_data
)
{
    if(0 == device_id){
        READ_INDIRCT_REG(0, entry_id, L2FE_CLE_IGR_KEY_ACCESS);
        cls_key_data->key_data0.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA0);
        cls_key_data->key_data1.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA1);
        cls_key_data->key_data2.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA2);
        cls_key_data->key_data3.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA3);
        cls_key_data->key_data4.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA4);
        cls_key_data->key_data5.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA5);
        cls_key_data->key_data6.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA6);
        cls_key_data->key_data7.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA7);
        cls_key_data->key_data8.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA8);
        cls_key_data->key_data9.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA9);
        cls_key_data->key_data10.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA10);
        cls_key_data->key_data11.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA11);
        cls_key_data->key_data12.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA12);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(0, entry_id, L2FE_CLE_IGR_KEY_ACCESS);
        cls_key_data->key_data0.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA0);
        cls_key_data->key_data1.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA1);
        cls_key_data->key_data2.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA2);
        cls_key_data->key_data3.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA3);
        cls_key_data->key_data4.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA4);
        cls_key_data->key_data5.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA5);
        cls_key_data->key_data6.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA6);
        cls_key_data->key_data7.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA7);
        cls_key_data->key_data8.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA8);
        cls_key_data->key_data9.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA9);
        cls_key_data->key_data10.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA10);
        cls_key_data->key_data11.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA11);
        cls_key_data->key_data12.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_KEY_DATA12);
#endif
    }
    return CA_E_OK;

}
static ca_status_t __aal_l2_cls_ingr_key_data_write(
    ca_device_id_t             device_id,
    ca_uint16               entry_id,
    __aal_cls_key_data_t    *cls_key_data
)
{
    if(0 == device_id){
        CA_NE_REG_WRITE(cls_key_data->key_data12.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA12));
        CA_NE_REG_WRITE(cls_key_data->key_data11.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA11));
        CA_NE_REG_WRITE(cls_key_data->key_data10.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA10));
        CA_NE_REG_WRITE(cls_key_data->key_data9.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA9));
        CA_NE_REG_WRITE(cls_key_data->key_data8.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA8));
        CA_NE_REG_WRITE(cls_key_data->key_data7.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA7));
        CA_NE_REG_WRITE(cls_key_data->key_data6.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA6));
        CA_NE_REG_WRITE(cls_key_data->key_data5.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA5));
        CA_NE_REG_WRITE(cls_key_data->key_data4.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA4));
        CA_NE_REG_WRITE(cls_key_data->key_data3.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA3));
        CA_NE_REG_WRITE(cls_key_data->key_data2.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA2));
        CA_NE_REG_WRITE(cls_key_data->key_data1.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA1));
        CA_NE_REG_WRITE(cls_key_data->key_data0.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_KEY_DATA0));

        WRITE_INDIRCT_REG(0, entry_id, L2FE_CLE_IGR_KEY_ACCESS);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(cls_key_data->key_data12.wrd, L2FE_CLE_IGR_KEY_DATA12);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data11.wrd, L2FE_CLE_IGR_KEY_DATA11);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data10.wrd, L2FE_CLE_IGR_KEY_DATA10);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data9.wrd, L2FE_CLE_IGR_KEY_DATA9);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data8.wrd, L2FE_CLE_IGR_KEY_DATA8);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data7.wrd, L2FE_CLE_IGR_KEY_DATA7);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data6.wrd, L2FE_CLE_IGR_KEY_DATA6);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data5.wrd, L2FE_CLE_IGR_KEY_DATA5);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data4.wrd, L2FE_CLE_IGR_KEY_DATA4);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data3.wrd, L2FE_CLE_IGR_KEY_DATA3);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data2.wrd, L2FE_CLE_IGR_KEY_DATA2);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data1.wrd, L2FE_CLE_IGR_KEY_DATA1);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data0.wrd, L2FE_CLE_IGR_KEY_DATA0);

        CA_8279_WRITE_INDIRCT_REG(0, entry_id, L2FE_CLE_IGR_KEY_ACCESS);
#endif
    }

    return CA_E_OK;

}

static ca_status_t __aal_l2_cls_egr_key_data_read(
    ca_device_id_t             device_id,
    ca_uint16               entry_id,
    __aal_cls_key_data_t    *cls_key_data
)
{
    if(0 == device_id){
        READ_INDIRCT_REG(0, entry_id, L2FE_CLE_EGR_KEY_ACCESS);
        cls_key_data->key_data0.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA0);
        cls_key_data->key_data1.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA1);
        cls_key_data->key_data2.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA2);
        cls_key_data->key_data3.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA3);
        cls_key_data->key_data4.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA4);
        cls_key_data->key_data5.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA5);
        cls_key_data->key_data6.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA6);
        cls_key_data->key_data7.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA7);
        cls_key_data->key_data8.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA8);
        cls_key_data->key_data9.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA9);
        cls_key_data->key_data10.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA10);
        cls_key_data->key_data11.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA11);
        cls_key_data->key_data12.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA12);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(0, entry_id, L2FE_CLE_EGR_KEY_ACCESS);
        cls_key_data->key_data0.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA0);
        cls_key_data->key_data1.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA1);
        cls_key_data->key_data2.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA2);
        cls_key_data->key_data3.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA3);
        cls_key_data->key_data4.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA4);
        cls_key_data->key_data5.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA5);
        cls_key_data->key_data6.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA6);
        cls_key_data->key_data7.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA7);
        cls_key_data->key_data8.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA8);
        cls_key_data->key_data9.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA9);
        cls_key_data->key_data10.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA10);
        cls_key_data->key_data11.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA11);
        cls_key_data->key_data12.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_KEY_DATA12);
#endif
    }
    return CA_E_OK;

}
static ca_status_t __aal_l2_cls_egr_key_data_write(
    ca_device_id_t             device_id,
    ca_uint16               entry_id,
    __aal_cls_key_data_t    *cls_key_data
)
{
    if(0 == device_id){
        CA_NE_REG_WRITE(cls_key_data->key_data12.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA12));
        CA_NE_REG_WRITE(cls_key_data->key_data11.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA11));
        CA_NE_REG_WRITE(cls_key_data->key_data10.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA10));
        CA_NE_REG_WRITE(cls_key_data->key_data9.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA9));
        CA_NE_REG_WRITE(cls_key_data->key_data8.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA8));
        CA_NE_REG_WRITE(cls_key_data->key_data7.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA7));
        CA_NE_REG_WRITE(cls_key_data->key_data6.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA6));
        CA_NE_REG_WRITE(cls_key_data->key_data5.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA5));
        CA_NE_REG_WRITE(cls_key_data->key_data4.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA4));
        CA_NE_REG_WRITE(cls_key_data->key_data3.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA3));
        CA_NE_REG_WRITE(cls_key_data->key_data2.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA2));
        CA_NE_REG_WRITE(cls_key_data->key_data1.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA1));
        CA_NE_REG_WRITE(cls_key_data->key_data0.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_KEY_DATA0));

        WRITE_INDIRCT_REG(0, entry_id, L2FE_CLE_EGR_KEY_ACCESS);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(cls_key_data->key_data12.wrd, L2FE_CLE_EGR_KEY_DATA12);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data11.wrd, L2FE_CLE_EGR_KEY_DATA11);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data10.wrd, L2FE_CLE_EGR_KEY_DATA10);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data9.wrd, L2FE_CLE_EGR_KEY_DATA9);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data8.wrd, L2FE_CLE_EGR_KEY_DATA8);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data7.wrd, L2FE_CLE_EGR_KEY_DATA7);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data6.wrd, L2FE_CLE_EGR_KEY_DATA6);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data5.wrd, L2FE_CLE_EGR_KEY_DATA5);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data4.wrd, L2FE_CLE_EGR_KEY_DATA4);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data3.wrd, L2FE_CLE_EGR_KEY_DATA3);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data2.wrd, L2FE_CLE_EGR_KEY_DATA2);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data1.wrd, L2FE_CLE_EGR_KEY_DATA1);
        CA_8279_NE_REG_WRITE(cls_key_data->key_data0.wrd, L2FE_CLE_EGR_KEY_DATA0);

        CA_8279_WRITE_INDIRCT_REG(0, entry_id, L2FE_CLE_EGR_KEY_ACCESS);
#endif
    }
    return CA_E_OK;

}


//ca_status_t __aal_l2_cls_ingr_key_entry_type_


ca_status_t aal_l2_cls_ipv4_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv4_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv4_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    if (mask.s.valid) {
      cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   if(mask.s.is_ipv4_ipv6_key){
        if(cfg->is_ipv4_ipv6_key){
            cls_key_data.key_data12.bf.cl_key_entry_type = 1;
            cls_key_data.key_data11.bf0.cl_key_entry_type = 1;
            }else{
                cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY;
                cls_key_data.key_data12.bf.cl_key_entry_type = 0;
            }
   }
   cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid = FALSE;

    if(mask.s.ipv4_ecn){
        cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn = cfg->ipv4_ecn;
    }
    if(mask.s.ipv4_ecn_vld){
        cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn_mask = (cfg->ipv4_ecn_vld == 0);
    }

    if (mask.s.mac_da) {
      cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_mac_da =
           cfg->mac_da[0];
       cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da =
           cfg->mac_da[1]<<24 | cfg->mac_da[2]<<16 | cfg->mac_da[3]<< 8 | cfg->mac_da[4]  ;
       cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_da =
           cfg->mac_da[5];
    }

    if (mask.s.mac_sa) {
      cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa =
        cfg->mac_sa[0] <<16 | cfg->mac_sa[1] << 8 | cfg->mac_sa[2];

      cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa =
        cfg->mac_sa[3] <<16 | cfg->mac_sa[4] << 8 | cfg->mac_sa[5];
    }

    if (mask.s.mac_field_ctrl) {
      cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_mac_ctrl = cfg->mac_field_ctrl;
    }

    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_top_vid = (cfg->top_vlan_id >> 4) &0xff;
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_vid = (cfg->top_vlan_id  & 0x0f);
    }

    if (mask.s.top_is_svlan) {
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_sc_ind = cfg->top_is_svlan;
    }

    if (mask.s.inner_vlan_id) {
     cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_vid = cfg->inner_vlan_id;
    }

    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_sc_ind = cfg->inner_is_svlan;
    }
    if (mask.s.vlan_field_ctrl) {
        cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_vid_ctrl =
           cfg->vlan_field_ctrl;
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_802_1p = cfg->top_dot1p;
    }
    if (mask.s.inner_dot1p) {
      cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_802_1p = cfg->inner_dot1p;
    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_dot1p_ctrl =  cfg->dot1p_ctrl;
    }

    if(mask.s.vlan_number){
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_tag_num = cfg->vlan_number;

    }

    if (mask.s.vlan_number_vld) {
       if (cfg->vlan_number_vld) {
          cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x2FFFFE;
       } else {
          cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x000001;
       }
    }
    if(mask.s.ethernet_type){
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ev2_pt = cfg->ethernet_type;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FFFFD;
        } else {
           cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x000002;
        }
    }
    if(mask.s.ip_da){
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ipv4_da = (cfg->ip_da >> 31) & 1;
       cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_da = cfg->ip_da & 0x7FFFFFFF;
    }
    if(mask.s.ip_sa){
       cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_sa = (cfg->ip_sa >> 31) & 1;
       cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ipv4_sa = cfg->ip_sa & 0x7FFFFFFF;
    }
    if (mask.s.ip_da_mask) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &= 0x3FFF83;
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= (cfg->ip_da_mask << 2);
    }
    if (mask.s.ip_sa_mask) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &= 0x3FF07F;
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= (cfg->ip_sa_mask << 7) ;
    }
    if (mask.s.ip_ctrl) {
        cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_ip_ctrl =  cfg->ip_ctrl;
     }

    if (mask.s.ip_pt) {
       cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ip_pt =  (cfg->ip_pt >> 7) & 1;
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_pt =  (cfg->ip_pt  & 0x7f);
     }

    if (mask.s.ip_pt_valid) {
         if (cfg->ip_pt_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FEFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0001000;
         }
     }

    if (mask.s.dscp) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_tos =  cfg->dscp;
     }


    if (mask.s.dscp_valid) {
         if (cfg->dscp_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FDFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0002000;
         }
     }

    if (mask.s.ip_option) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_option =  cfg->ip_option;
     }


    if (mask.s.ip_option_valid) {
         if (cfg->ip_option_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FBFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0004000;
         }
     }

    if (mask.s.ip_fragment) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_fragment =  cfg->ip_fragment;
     }

    if (mask.s.ip_fragment_valid) {
         if (cfg->ip_fragment_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3F7FFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0008000;
         }
     }

    if (mask.s.l4_dst_port_hi) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_high =  cfg->l4_dst_port_hi;
     }
    if (mask.s.l4_dst_port_lo) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_low =  (cfg->l4_dst_port_lo >> 15) & 1;
       cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_dp_low =  (cfg->l4_dst_port_lo  & 0x7FFF);
     }
    if (mask.s.l4_dst_ctrl) {
        cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_dp_ctrl =  cfg->l4_dst_ctrl;
     }
    if (mask.s.l4_src_port_hi) {
       cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_high =  cfg->l4_src_port_hi;
     }
    if (mask.s.l4_dst_port_lo) {
       cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_low =  (cfg->l4_src_port_lo >> 15) & 1;
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_sp_low =  (cfg->l4_src_port_lo  & 0x7FFF);
     }
    if (mask.s.l4_src_ctrl) {
        cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_sp_ctrl =  cfg->l4_src_ctrl;
     }

    if (mask.s.l4_flag) {
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_flags =  cfg->l4_flag;
     }

    if (mask.s.l4_flag_valid) {
         if (cfg->l4_flag_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3EFFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0010000;
         }
     }

    if (mask.s.special_pkt_code) {
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_special_pkt_code =  cfg->special_pkt_code;
     }

    if (mask.s.special_pkt_code_valid) {
         if (cfg->special_pkt_code_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3DFFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x020000;
         }
    }
    if (mask.s.global_key) {
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_global_key =  (cfg->global_key>>23)&0x1FF;
       cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_global_key =  cfg->global_key & 0x7FFFFF;
    }

    if (mask.s.global_key_mask) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FFFF;
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |=  (cfg->global_key_mask << 18);
    }
    if (mask.s.l7_ctrl) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l7_ctrl =  cfg->l7_ctrl;
    }
    if (mask.s.fields_is_or) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_fields_operator =  cfg->fields_is_or;
    }

    if (mask.s.precedence) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_prcd =  cfg->precedence;
    }
    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_ipv4_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv4_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
    return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    if((cls_key_data.key_data12.bf.cl_key_entry_type == 1) && (cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        cfg->is_ipv4_ipv6_key = 1;
    }else{
        cfg->is_ipv4_ipv6_key = 0;
    }

    cfg->ipv4_ecn = cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn;
    cfg->ipv4_ecn_vld = (cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn_mask == 0);
    cfg->mac_da[0]   =  cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_mac_da;
    cfg->mac_da[1] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 24 & 0xff;
    cfg->mac_da[2] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 16 & 0xff;
    cfg->mac_da[3] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 8 & 0xff;
    cfg->mac_da[4] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 0 & 0xff;
    cfg->mac_da[5] = cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_da ;
    cfg->mac_sa[0]  =cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa >>16 & 0xff;
    cfg->mac_sa[1]  =cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa >>8 & 0xff;
    cfg->mac_sa[2]  =cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa >>0 & 0xff;
    cfg->mac_sa[3]  =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa >>16 & 0xff;
    cfg->mac_sa[4]  =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa >>8 & 0xff;
    cfg->mac_sa[5]  =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa >>0 & 0xff;
    cfg->mac_field_ctrl =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_mac_ctrl;
    cfg->top_vlan_id =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_top_vid<<4 |cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_vid;
    cfg->top_is_svlan =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_sc_ind;
    cfg->inner_vlan_id =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_vid;
    cfg->inner_is_svlan  =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_sc_ind;
    cfg->vlan_field_ctrl =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_vid_ctrl;
    cfg->top_dot1p = cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_802_1p;
    cfg->inner_dot1p =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_802_1p;
    cfg->dot1p_ctrl  =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_dot1p_ctrl ;
    cfg->vlan_number  =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_tag_num;
    cfg->vlan_number_vld =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x000001) == 0);
    cfg->ethernet_type  =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ev2_pt;
    cfg->ethernet_type_vld =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x000002) == 0);
    cfg->ip_da =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ipv4_da << 31 |cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_da;
    cfg->ip_sa  =cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_sa <<31 |cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ipv4_sa;
    cfg->ip_da_mask = ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask) >> 2) & 0x1F;
    cfg->ip_sa_mask = (cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask >> 7) & 0x1F;
    cfg->ip_ctrl  =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_ip_ctrl ;
    cfg->ip_pt  = cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ip_pt<<7 |cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_pt;
    cfg->ip_pt_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0001000) == 0);
    cfg->dscp =  cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_tos;
    cfg->dscp_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0002000) == 0);
    cfg->ip_option  =  cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_option ;
    cfg->ip_option_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0004000) == 0);
    cfg->ip_fragment   = cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_fragment ;
    cfg->ip_fragment_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0008000) == 0);
    cfg->l4_dst_port_hi  =  cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_high;
    cfg->l4_dst_port_lo =cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_low <<15 | cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_dp_low;
    cfg->l4_dst_ctrl   = cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_dp_ctrl;
    cfg->l4_src_port_hi    = cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_high;
    cfg->l4_src_port_lo  = cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_low << 15 |cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_sp_low;
    cfg->l4_src_ctrl   =cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_sp_ctrl ;
    cfg->l4_flag  =  cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_flags;
    cfg->l4_flag_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0010000) == 0);
    cfg->special_pkt_code = cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_special_pkt_code ;
    cfg->special_pkt_code_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0020000) == 0);
    cfg->global_key  =  cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_global_key<<23 | cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_global_key;
    cfg->global_key_mask =(cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask >> 18) & 0x0f;
    cfg->l7_ctrl=     cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l7_ctrl  ;
    cfg->fields_is_or = cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_fields_operator;
    cfg->precedence= cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_prcd;
#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_ipv6_short_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv6_short_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv6_short_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
      cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   if(mask.s.is_ipv4_ipv6_key){
         if(cfg->is_ipv4_ipv6_key){
             cls_key_data.key_data12.bf.cl_key_entry_type = 1;
             cls_key_data.key_data11.bf0.cl_key_entry_type = 1;
             }else{
                 cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY;
                 cls_key_data.key_data12.bf.cl_key_entry_type = 0;
             }
    }

   cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid = TRUE;
   if(mask.s.ipv6_ecn){
       cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn = cfg->ipv6_ecn;
   }
   if(mask.s.ipv6_ecn_valid){
       cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn_mask = (cfg->ipv6_ecn_valid == 0);
   }

    if (mask.s.ipv6_da) {
      cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[0] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data10.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[0] & 0x1F) << 27 | (cfg->ipv6_da[1] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data9.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[1] & 0x1F) << 27 | (cfg->ipv6_da[2] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data8.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[2] & 0x1F) << 27 | (cfg->ipv6_da[3] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[3] & 0x1F);
    }

    if (mask.s.ipv6_sa) {
       cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[0] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data6.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[0] & 0x1F) << 27 | (cfg->ipv6_sa[1] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data5.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[1] & 0x1F) << 27 | (cfg->ipv6_sa[2] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data4.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[2] & 0x1F) << 27 | (cfg->ipv6_sa[3] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[3] & 0x1F);
     }

    if (mask.s.ipv6_ctrl) {
      cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_ip_ctrl = cfg->ipv6_ctrl;
    }

    if (mask.s.ipv6_addr_mask) {
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_mask = cfg->ipv6_addr_mask;
    }

    if (mask.s.ipv6_pt) {
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_pt = cfg->ipv6_pt;
    }

    if (mask.s.ipv6_pt_valid) {
        if(cfg->ipv6_pt_valid){
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &= 0xFD;
        }else{
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x02;
        }
    }

    if (mask.s.dscp) {
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_tos = (cfg->dscp >> 1) & 0x1f;
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_ip_tos = cfg->dscp  & 0x01;
    }

    if (mask.s.dscp_valid) {
        if(cfg->dscp_valid){
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &= 0xFB;
        }else{
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x04;
        }
    }

    if (mask.s.l4_flag) {
        cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_flags =  cfg->l4_flag;
      }

     if (mask.s.l4_flag_valid) {
          if (cfg->l4_flag_valid) {
             cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xF7;
          } else {
             cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x08;
          }
      }

    if (mask.s.special_pkt_code) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_special_pkt_code =  cfg->special_pkt_code;
     }

    if (mask.s.special_pkt_code_valid) {
         if (cfg->special_pkt_code_valid) {
            cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xEF;
         } else {
            cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x10;
         }
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_sc_ind = cfg->top_is_svlan;
    }
    if (mask.s.top_vlan_id) {
        cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_vid =
           cfg->top_vlan_id;
    }

    if (mask.s.top_vlan_valid) {
        if (cfg->top_vlan_valid) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xDF;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x20;
        }
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_802_1p = cfg->top_dot1p;
    }
    if (mask.s.top_dot1p_valid) {
        if (cfg->top_dot1p_valid) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xBF;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x40;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_tag_num = cfg->vlan_number;
    }
    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xFE;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x01;
        }
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_dp = (cfg->l4_dst_port >> 11) & 0x1F;
       cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_dp = (cfg->l4_dst_port  & 0x7FF);
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_sp = cfg->l4_src_port  ;
    }
    if (mask.s.l4_ctrl) {
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_l4_ctrl = cfg->l4_ctrl  ;
    }
    if (mask.s.flow_lab) {
       cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_flow_label = (cfg->flow_lab >> 15 & 0x1F);
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_flow_label = cfg->flow_lab & 0x7FFF;
    }
    if(mask.s.flow_lab_valid){
        if (cfg->flow_lab_valid) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0x7F;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x80;
        }
    }
    if (mask.s.fields_is_or) {
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_fields_operator =  cfg->fields_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_prcd =  cfg->precedence;
    }
    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_ipv6_short_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv6_short_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid=cls_key_data.key_data12.bf.cl_key_valid ;
    if((cls_key_data.key_data12.bf.cl_key_entry_type == 1) && (cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        cfg->is_ipv4_ipv6_key = 1;
    }else{
        cfg->is_ipv4_ipv6_key = 0;
    }

    cfg->ipv6_ecn = cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn;
    cfg->ipv6_ecn_valid = (cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn_mask == 0);
    cfg->ipv6_da[0] = (cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | ((cls_key_data.key_data10.bf1.cl_key_orhr_ipv6e_ipv6_da >> 27) & 0x1F) ;
    cfg->ipv6_da[1] = (cls_key_data.key_data10.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | ((cls_key_data.key_data9.bf1.cl_key_orhr_ipv6e_ipv6_da >> 27) & 0x1F) ;
    cfg->ipv6_da[2] = (cls_key_data.key_data9.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | ((cls_key_data.key_data8.bf1.cl_key_orhr_ipv6e_ipv6_da >> 27) & 0x1F)  ;
    cfg->ipv6_da[3] = (cls_key_data.key_data8.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | (cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_da );
    cfg->ipv6_sa[0] = (cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | ((cls_key_data.key_data6.bf1.cl_key_orhr_ipv6e_ipv6_sa >> 27) & 0x1F);
    cfg->ipv6_sa[1] = (cls_key_data.key_data6.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | ((cls_key_data.key_data5.bf1.cl_key_orhr_ipv6e_ipv6_sa >> 27) & 0x1F);
    cfg->ipv6_sa[2] = (cls_key_data.key_data5.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | ((cls_key_data.key_data4.bf1.cl_key_orhr_ipv6e_ipv6_sa >> 27) & 0x1F);
    cfg->ipv6_sa[3] = (cls_key_data.key_data4.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | (cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_sa);
    cfg->ipv6_ctrl  = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_ip_ctrl;
    cfg->ipv6_addr_mask   =cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_mask;
    cfg->ipv6_pt    =cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_pt;
    cfg->ipv6_pt_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x02) == 0);
    cfg->dscp = cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_tos << 1 |cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_ip_tos;
    cfg->dscp_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x04) == 0);
    cfg->l4_flag   = cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_flags;
    cfg->l4_flag_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x08) == 0);
    cfg->special_pkt_code  =  cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_special_pkt_code ;
    cfg->special_pkt_code_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x10) == 0);
    cfg->top_is_svlan     =cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_sc_ind;
    cfg->top_vlan_id     = cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_vid;
    cfg->top_vlan_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x20) == 0);
    cfg->top_dot1p  = cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_802_1p;
    cfg->top_dot1p_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x40) == 0);
    cfg->vlan_number   =cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_tag_num;
    cfg->vlan_number_vld = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x01) == 0);
    cfg->l4_dst_port  =cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_dp  << 11 | cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_dp;
    cfg->l4_src_port   = cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_sp ;
    cfg->l4_ctrl  = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_l4_ctrl  ;
    cfg->flow_lab = cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_flow_label << 15  |cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_flow_label;
    cfg->flow_lab_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x80) == 0);
    cfg->fields_is_or = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_fields_operator ;
    cfg->precedence = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_prcd;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t
aal_l2_cls_l2r_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l2r_key_t        * cfg)
{
    ca_status_t ret = CA_E_OK;

    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id >> 6) & 0x3f;
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id) & 0x3f;
    }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.mac_field_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl =
            cfg->mac_field_ctrl;
    }
    if (mask.s.mac_da) {
       cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da =
            ((cfg->mac_da[0] << 11) | (cfg->mac_da[1] << 3) | ((cfg->mac_da[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da =
            (((cfg->mac_da[2] & 0x1f) << 24) | (cfg->mac_da[3] << 16) | (cfg->mac_da[4] << 8) | cfg->mac_da[5]);
    }
    if (mask.s.mac_sa) {
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (cfg->mac_sa[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (((cfg->mac_sa[0] & 0x1f) << 27) | (cfg->mac_sa[1] << 19) |
             (cfg->mac_sa[2] << 11) | (cfg->mac_sa[3] << 3) | ((cfg->mac_sa[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            ((cfg->mac_sa[4] & 0x1f) << 8) | (cfg->mac_sa[5]);
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p = cfg->inner_dot1p;
    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl;
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp =  cfg->l4_src_port;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port & 0xf);
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &=   0x2;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd = cfg->precedence;
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t
aal_l2_cls_l2r_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg)
{
    __aal_cls_key_data_t cls_key_data;
     ca_status_t ret = CA_E_OK;
     if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
         return CA_E_PARAM;
     }

     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
     __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


     cfg->top_dot1p =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p;
     cfg->inner_dot1p =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p;

     cfg->vlan_field_ctrl =
        cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl;
     cfg->top_vlan_id =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid;
     cfg->top_is_svlan =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind;
     cfg->inner_vlan_id =
         (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid << 6) | (cls_key_data.key_data7.bf2.
                 cl_key_tre_l2rl2r_1_inner_vid);
     cfg->inner_is_svlan =
        cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind;

     cfg->dot1p_ctrl = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl;

     cfg->vlan_number_vld = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x1) == 0);
     cfg->vlan_number     = cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num;

     cfg->ethernet_type_vld = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x2) == 0);
     cfg->ethernet_type = cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt;
     cfg->field_is_or = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator;

     cfg->l4_src_port = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp;
     cfg->l4_dst_port = (cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp | ((cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp << 4) & 0xfff0));

     cfg->mac_field_ctrl =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl;

     cfg->mac_da[0] =
         (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 11) & 0xff;
     cfg->mac_da[1] =
         (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 3) & 0xff;
     cfg->mac_da[2] =
         ((cls_key_data.key_data11.bf2.
           cl_key_tre_l2rl2r_1_mac_da & 7) << 5 & 0xE0) | ((cls_key_data.key_data10.bf2.
                   cl_key_tre_l2rl2r_1_mac_da
                   >> 24) & 0x1f);
     cfg->mac_da[3] =
         (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 16) & 0xff;
     cfg->mac_da[4] =
         (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 8) & 0xff;
     cfg->mac_da[5] =
         (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 0) & 0xff;
     cfg->mac_sa[0] =
         ((cls_key_data.key_data10.bf2.
           cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data9.bf2.
                   cl_key_tre_l2rl2r_1_mac_sa
                   >> 27) & 0x1f);
     cfg->mac_sa[1] =
         (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 19) & 0xff;
     cfg->mac_sa[2] =
         (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 11) & 0xff;
     cfg->mac_sa[3] =
         (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 3) & 0xff;
     cfg->mac_sa[4] =
         ((cls_key_data.key_data9.bf2.
           cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data8.bf2.
                   cl_key_tre_l2rl2r_1_mac_sa
                   >> 8) & 0x1f);
     cfg->mac_sa[5] =
         (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 0) & 0xff;

     cfg->l4_port_field_ctrl =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl;
     cfg->precedence =cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd;
#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

     return ret;

}


ca_status_t
aal_l2_cls_l2r_2nd_key_set(CA_IN ca_device_id_t device_id,
                          CA_IN ca_uint32_t entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t mask,
                          CA_IN aal_l2_cls_l2r_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;

    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id >> 3) & 0x1ff;
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id) & 0x7;
    }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.mac_field_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl =
            cfg->mac_field_ctrl;
    }
    if (mask.s.mac_da) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da =
            ((cfg->mac_da[0] << 14) | (cfg->mac_da[1] << 6) | ((cfg->
                    mac_da[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da =
            (((cfg->mac_da[2] & 0x3) << 24) | (cfg->mac_da[3] << 16) |
             (cfg->mac_da[4] << 8) | cfg->mac_da[5]);
    }
    if (mask.s.mac_sa) {
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (cfg->mac_sa[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (((cfg->mac_sa[0] & 0x3) << 30) | (cfg->mac_sa[1] << 22) |
             (cfg->mac_sa[2] << 14) | (cfg->mac_sa[3] << 6) | ((cfg->
                     mac_sa[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            ((cfg->mac_sa[4] & 0x3) << 8) | (cfg->mac_sa[5]);

    }

    if (mask.s.top_dot1p) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p = cfg->inner_dot1p;

    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl = cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl = cfg->l4_port_field_ctrl ;
    }
    if (mask.s.l4_src_port) {

       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp =  cfg->l4_src_port;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port & 1);
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port >> 1) & 0x7fff;
    }


    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd = cfg->precedence;
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t
aal_l2_cls_l2r_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg)
{
    __aal_cls_key_data_t cls_key_data;
     ca_status_t ret = CA_E_OK;
     if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
         return CA_E_PARAM;
     }

     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
     __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);
    cfg->precedence =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd;

    cfg->field_is_or = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator;

    cfg->l4_src_port = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp;
    cfg->l4_dst_port = (cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp | ((cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp << 1) & 0xfffe));

    cfg->dot1p_ctrl = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl;
    cfg->top_dot1p =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p;
    cfg->inner_dot1p =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p;
    cfg->vlan_number_vld = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x1) == 0);
    cfg->vlan_number =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num;

    cfg->vlan_field_ctrl =
     cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl;
    cfg->top_vlan_id =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid;
    cfg->top_is_svlan =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind;
    cfg->inner_vlan_id =
      (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf2.
              cl_key_tre_l2rl2r_2_inner_vid);
    cfg->inner_is_svlan =
     cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind;
    cfg->ethernet_type = cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt;
    cfg->mac_field_ctrl =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl;
    cfg->mac_da[0] =
       (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da[1] =
       (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da[2] =
       ((cls_key_data.key_data5.bf2.
         cl_key_tre_l2rl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf2.
                 cl_key_tre_l2rl2r_2_mac_da
                 >> 24) & 0x3);
    cfg->mac_da[3] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da[4] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da[5] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa[0] =
       ((cls_key_data.key_data4.bf2.
         cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf2.
                 cl_key_tre_l2rl2r_2_mac_sa
                 >> 30) & 0x3);
    cfg->mac_sa[1] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa[2] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa[3] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa[4] =
       ((cls_key_data.key_data3.bf2.
         cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf2.
                 cl_key_tre_l2rl2r_2_mac_sa
                 >> 8) & 0x3);
    cfg->mac_sa[5] =
       (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl;


    cfg->ethernet_type_vld = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x2) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;
}

ca_status_t aal_l2_cls_l3r_1st_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;

  __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if(mask.s.ecn){
        cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn_mask = (cfg->ecn_valid == 0);
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_inner_vid = cfg->inner_vlan_id ;
    }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.ip_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_ip_ctrl =
            cfg->ip_ctrl;
    }
    if (mask.s.ip_da) {
       cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ipv4_da = (cfg->ip_da >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_da = cfg->ip_da & 0x7FFF;
    }
    if (mask.s.ip_sa) {
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_sa =
            (cfg->ip_sa >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_sa = cfg->ip_sa & 0x7FFF;
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_802_1p = cfg->inner_dot1p;
    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl;
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_sp =  cfg->l4_src_port & 0x3FFF;
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_sp  = (cfg->l4_src_port >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_dp  = cfg->l4_dst_port ;
    }

    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_option = cfg->has_ipop;
    }
    if (mask.s.has_ipop_valid) {
        if (cfg->has_ipop_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_tos = cfg->dscp;
    }
    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_pt = cfg->ip_pt;
    }
    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_fragment = cfg->ipv4_fragment;
    }
    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag) {
        cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_l4_flags = ((cfg->l4_flag >> 2) & 0x01);
        cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_l4_flags = (cfg->l4_flag & 0x03);
    }
    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_special_pkt_code = cfg->special_pkt_code;
    }
    if (mask.s.special_pkt_code_valid) {
        if (cfg->special_pkt_code_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_tag_num = cfg->vlan_number;
    }
    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_1_prcd = cfg->precedence;
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_l3r_1st_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3r_key_t *cfg)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);
    cfg->ecn = cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn;
    cfg->ecn_valid = (cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn_mask == 0);
    cfg->vlan_field_ctrl =
      cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_vid_ctrl;
    cfg->top_vlan_id      =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_vid;
    cfg->top_is_svlan     =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_sc_ind;
    cfg->inner_vlan_id    = (cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_inner_vid);
    cfg->inner_is_svlan   =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_sc_ind;
    cfg->top_dot1p        =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_top_802_1p;
    cfg->inner_dot1p      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_802_1p;
    cfg->field_is_or      =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_fields_operator;
    cfg->l4_src_port      = cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_sp | (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_sp <<14);
    cfg->l4_dst_port      = (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_dp);
    cfg->vlan_number      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_tag_num;
    cfg->precedence       =cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_1_prcd;
    cfg->ethernet_type    =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_ev2_pt;
    cfg->ip_ctrl          =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_ip_ctrl;
    cfg->dot1p_ctrl       =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_dot1p_ctrl;
    cfg->ip_da            =cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ipv4_da <<15 |cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_da;
    cfg->ip_sa            =cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_sa <<15 |cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_sa;
    cfg->l4_port_field_ctrl =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_ctrl;
    cfg->ipv4_fragment    =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_fragment;
    cfg->l4_flag          =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_l4_flags<<2 | cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_l4_flags;
    cfg->has_ipop         =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_option;
    cfg->dscp             =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_tos;
    cfg->ip_pt            =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_pt;
    cfg->special_pkt_code =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_special_pkt_code;
    cfg->ipv4_fragment_valid   = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x20) == 0x00);
    cfg->l4_flag_valid         = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x40) == 0x00);
    cfg->vlan_number_vld       = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x01) == 0x00);
    cfg->ip_pt_valid           = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x04) == 0x00);
    cfg->has_ipop_valid        = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x10) == 0x00);
    cfg->dscp_valid            = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld     = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid =((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x80) == 0x00);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}



ca_status_t aal_l2_cls_l3r_2nd_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3r_key_t *cfg)


{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if(mask.s.ecn){
        cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn_mask = (cfg->ecn_valid == 0);
    }

    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid = cfg->inner_vlan_id;
            }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.ip_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl =
            cfg->ip_ctrl;
    }
    if (mask.s.ip_da) {
       cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            (cfg->ip_da >> 12) & 0xfffff;
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            cfg->ip_da & 0xfff;
    }
    if (mask.s.ip_sa) {
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            (cfg->ip_sa >> 12) & 0xfffff;
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            cfg->ip_sa & 0xfff;
    }

    if (mask.s.top_dot1p) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p = (cfg->top_dot1p & 1);
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p = ((cfg->top_dot1p >> 1) & 3);
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p = cfg->inner_dot1p;

    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl = cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl = cfg->l4_port_field_ctrl ;
    }
    if (mask.s.l4_src_port) {

       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp = (cfg->l4_src_port & 0x7ff);
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp = ((cfg->l4_src_port >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp = cfg->l4_dst_port;
    }


    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd = cfg->precedence;
    }

    if (mask.s.ip_pt) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt = cfg->ip_pt;
    }

    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos = cfg->dscp;
    }

    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option = cfg->has_ipop;
    }

    if (mask.s.has_ipop_valid) {
        if (cfg->has_ipop_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags = cfg->l4_flag;
    }

    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment = cfg->ipv4_fragment;
    }

    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code >> 4) & 1;
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid) {
        if (cfg->special_pkt_code_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x80;
        }
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_l3r_2nd_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3r_key_t *cfg)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);
    cfg->ecn                = cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn ;
    cfg->ecn_valid          = (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn_mask == 0);
    cfg->vlan_field_ctrl    =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl           ;
    cfg->top_vlan_id        =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid            ;
    cfg->top_is_svlan       =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id      =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid          ;
    cfg->inner_is_svlan     =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl            =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl            ;
    cfg->ip_da              = (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da)          ;
    cfg->ip_sa              = (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa)         ;
    cfg->top_dot1p          =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p    | (cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl            ;
    cfg->l4_src_port        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp | (cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp              ;
    cfg->vlan_number        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num            ;
    cfg->ethernet_type      =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt             ;
    cfg->field_is_or        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator    ;
    cfg->precedence         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd               ;
    cfg->ip_pt              =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt              ;
    cfg->special_pkt_code   = (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code    ;
    cfg->dscp               =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos             ;
    cfg->has_ipop           =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option        ;
    cfg->l4_flag            =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags           ;
    cfg->ipv4_fragment      =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment      ;
    cfg->ipv4_fragment_valid  = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid        = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld      = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid          = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid       = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid           = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld    = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid=((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x80) == 0x00);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}




ca_status_t aal_l2_cls_cmr_1st_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;



    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
     return CA_E_PARAM;
    }
    printk("%s %d \n",__FUNCTION__,__LINE__ );

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if(mask.s.ecn){
        cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn_mask = (cfg->ecn_valid==0);
    }

    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid = (cfg->top_vlan_id >> 3) & 0x1ff;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid = cfg->top_vlan_id & 0x07;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.vlan_field_valid) {
     if (cfg->vlan_field_valid) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xBFF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x400;
     }
    }

    if (mask.s.ip_addr_is_sa) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel =
            cfg->ip_addr_is_sa;
    }
    if (mask.s.ip_addr) {
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            (cfg->ip_addr >> 31) & 0x01;
       cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            cfg->ip_addr & 0x7fffffff;
    }
    if (mask.s.ip_addr_valid) {
        if (cfg->ip_addr_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xDFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel =
            cfg->mac_addr_is_sa;
    }

    if (mask.s.mac_addr) {
       cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            (cfg->mac_addr[0] << 9) | (cfg->mac_addr[1] << 1) | ((cfg->mac_addr[2] >> 7) & 0x01);
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            ((cfg->mac_addr[2] & 0x7f) << 24) | (cfg->mac_addr[3] << 16) | (cfg->mac_addr[4] << 8) | (cfg->mac_addr[5]);
    }

    if (mask.s.mac_addr_valid) {
        if (cfg->mac_addr_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xEFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x100;
        }
    }

    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.dot1p_valid) {
        if (cfg->dot1p_valid ) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0x7FF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x800;
        }
    }

    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl;
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp =  cfg->l4_src_port & 0x3FFF;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp  = (cfg->l4_src_port >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp  = cfg->l4_dst_port ;
    }

    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &=   0xFFE;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x001;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option = cfg->has_ipop;
    }
    if (mask.s.has_ipop_valid) {
     if (cfg->has_ipop_valid) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFEF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x010;
     }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos = cfg->dscp;
    }
    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFF7;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x008;
        }
    }
    if (mask.s.ip_pt) {
        cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt >> 7) &0x1;
        cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt&0x7f);
    }
    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFB;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x004;
        }
    }
    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment = cfg->ipv4_fragment;
    }
    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFDF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x020;
        }
    }
    if (mask.s.l4_flag) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags = cfg->l4_flag;
    }
    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFBF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x040;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code = cfg->special_pkt_code;
    }
    if (mask.s.special_pkt_code_valid) {
        if (cfg->special_pkt_code_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xF7F;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x080;
     }
    }
    if (mask.s.vlan_number) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num = cfg->vlan_number;
    }
    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFD;
        } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x002;
     }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd = cfg->precedence;
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t aal_l2_cls_cmr_1st_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmr_key_t *cfg)
{

    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->ecn                  = cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn;
    cfg->ecn_valid           =  (cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn_mask == 0);
    cfg->top_vlan_id          =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid <<3 |cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid;
    cfg->top_is_svlan         =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind;
    cfg->ip_addr_is_sa        =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel;
    cfg->ip_addr              =cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr << 31 |cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr;
    cfg->mac_addr_is_sa       =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel;
    cfg->mac_addr[0]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr[1]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr[2]          = (((cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr[3]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr[4]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr[5]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p            =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p;
    cfg->l4_port_field_ctrl   =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl;
    cfg->l4_src_port          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp | (cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp << 14);
    cfg->l4_dst_port          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp;
    cfg->vlan_number          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num;
    cfg->ethernet_type        =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt;
    cfg->field_is_or          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator;
    cfg->precedence           =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd;
    cfg->ip_pt                =cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt << 7 |cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt;
    cfg->dscp                 =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos;
    cfg->has_ipop             =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option;
    cfg->l4_flag              =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags;
    cfg->ipv4_fragment        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment;
    cfg->special_pkt_code        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code;
    cfg->dot1p_valid             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x800) == 0);
    cfg->vlan_field_valid        = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x400) == 0);
    cfg->ip_addr_valid           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x200) == 0);
    cfg->mac_addr_valid          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid  = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x080) == 0);
    cfg->l4_flag_valid           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid     = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x020) == 0);
    cfg->has_ipop_valid          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x010) == 0);
    cfg->dscp_valid              = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x008) == 0);
    cfg->ip_pt_valid             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld       = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x002) == 0);
    cfg->vlan_number_vld         = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x001) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t aal_l2_cls_cmr_2nd_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
     return CA_E_PARAM;
    }
    printk("%s %d \n",__FUNCTION__,__LINE__ );

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if(mask.s.ecn){
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn_mask = (cfg->ecn_valid == 0);
    }

    if (mask.s.top_vlan_id) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.vlan_field_valid) {
        if (cfg->vlan_field_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa;
    }
    if (mask.s.ip_addr) {
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            (cfg->ip_addr >> 28) & 0xf;
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            cfg->ip_addr & 0xfffffff;
    }
    if (mask.s.ip_addr_valid) {
        if (cfg->ip_addr_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa;
    }

    if (mask.s.mac_addr) {
       cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            (cfg->mac_addr[0] << 12) | (cfg->mac_addr[1] << 4) | ((cfg->mac_addr[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            ((cfg->mac_addr[2] & 0xf) << 24) | (cfg->mac_addr[3] << 16) | (cfg->mac_addr[4] << 8) | (cfg->mac_addr[5]);
    }

    if (mask.s.mac_addr_valid) {
        if (cfg->mac_addr_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.dot1p_valid) {
        if (cfg->dot1p_valid)
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl = cfg->l4_port_field_ctrl ;
    }
    if (mask.s.l4_src_port) {

       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp = (cfg->l4_src_port & 0x7ff);
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp = ((cfg->l4_src_port >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp = cfg->l4_dst_port;
    }


    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd = cfg->precedence;
    }

    if (mask.s.ip_pt) {
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt = ((cfg->ip_pt >> 4) & 0x0f);
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt = (cfg->ip_pt & 0x0f);
    }

    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos = cfg->dscp;
    }

    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option = cfg->has_ipop;
    }

    if (mask.s.has_ipop_valid) {
        if (cfg->has_ipop_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags = cfg->l4_flag;
    }

    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment = cfg->ipv4_fragment;
    }

    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code = cfg->special_pkt_code;
    }

    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x080;
        }
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


 ca_status_t aal_l2_cls_cmr_2nd_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->ecn                  = cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn;
    cfg->ecn_valid           =  (cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn_mask == 0);
    cfg->top_vlan_id          =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid;
    cfg->top_is_svlan         =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind;
    cfg->ip_addr_is_sa        =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel;
    cfg->ip_addr              =cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr;
    cfg->mac_addr_is_sa       =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel;
    cfg->mac_addr[0]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 12 & 0xff;
    cfg->mac_addr[1]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 4 & 0xff;
    cfg->mac_addr[2]          = (((cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 24) & 0xf));
    cfg->mac_addr[3]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 16 & 0xff);
    cfg->mac_addr[4]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 8 & 0xff);
    cfg->mac_addr[5]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 0 & 0xff);
    cfg->top_dot1p            =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p;
    cfg->l4_port_field_ctrl   =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl;
    cfg->l4_src_port          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp | (cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp << 11);
    cfg->l4_dst_port          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp;
    cfg->vlan_number          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num;
    cfg->ethernet_type        =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt;
    cfg->field_is_or          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator;
    cfg->precedence           =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd;
    cfg->ip_pt                =cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt;
    cfg->dscp                 =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos;
    cfg->has_ipop             =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option;
    cfg->l4_flag              =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags;
    cfg->ipv4_fragment        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment;
    cfg->special_pkt_code        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code;
    cfg->dot1p_valid             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid        = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid  = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid     = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid              = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld       = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld         = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x001) == 0);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t
aal_l2_cls_l2rl2r_key_set(CA_IN ca_device_id_t device_id,
                          CA_IN ca_uint32_t entry_id,
                          CA_IN aal_l2_cls_l2rl2r_key_mask_t mask,
                          CA_IN aal_l2_cls_l2rl2r_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;

    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

    cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
    cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;
    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id_0 >> 6) & 0x3f;
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id_0) & 0x3f;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.mac_field_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl =
            cfg->mac_field_ctrl_0;
    }
    if (mask.s.mac_da_0) {
       cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da =
            ((cfg->mac_da_0[0] << 11) | (cfg->mac_da_0[1] << 3) | ((cfg->mac_da_0[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da =
            (((cfg->mac_da_0[2] & 0x1f) << 24) | (cfg->mac_da_0[3] << 16) | (cfg->mac_da_0[4] << 8) | cfg->mac_da_0[5]);
    }
    if (mask.s.mac_sa_0) {
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (cfg->mac_sa_0[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (((cfg->mac_sa_0[0] & 0x1f) << 27) | (cfg->mac_sa_0[1] << 19) |
             (cfg->mac_sa_0[2] << 11) | (cfg->mac_sa_0[3] << 3) | ((cfg->mac_sa_0[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            ((cfg->mac_sa_0[4] & 0x1f) << 8) | (cfg->mac_sa_0[5]);
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp =  cfg->l4_src_port_0;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port_0 & 0xf);
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port_0 >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &=	0x2;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num = cfg->vlan_number_0;
    }


    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd = cfg->precedence_0;
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1 >> 3) & 0x1ff;
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1) & 0x7;
    }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.mac_field_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl =
            cfg->mac_field_ctrl_1;
    }
    if (mask.s.mac_da_1) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da =
            ((cfg->mac_da_1[0] << 14) | (cfg->mac_da_1[1] << 6) | ((cfg->
                    mac_da_1[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da =
            (((cfg->mac_da_1[2] & 0x3) << 24) | (cfg->mac_da_1[3] << 16) |
             (cfg->mac_da_1[4] << 8) | cfg->mac_da_1[5]);
    }
    if (mask.s.mac_sa_1) {
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (cfg->mac_sa_1[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (((cfg->mac_sa_1[0] & 0x3) << 30) | (cfg->mac_sa_1[1] << 22) |
             (cfg->mac_sa_1[2] << 14) | (cfg->mac_sa_1[3] << 6) | ((cfg->
                     mac_sa_1[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            ((cfg->mac_sa_1[4] & 0x3) << 8) | (cfg->mac_sa_1[5]);

    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp =  cfg->l4_src_port_1;
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port_1 & 1);
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port_1 >> 1) & 0x7fff;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd = cfg->precedence_1;
    }
    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t
aal_l2_cls_l2rl2r_key_get(CA_IN ca_device_id_t device_id,
                          CA_IN ca_uint32_t entry_id,
                          CA_OUT aal_l2_cls_l2rl2r_key_t * cfg)
{
    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;
    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;


    cfg->top_dot1p_0 =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p;
    cfg->inner_dot1p_0 =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p;

    cfg->vlan_field_ctrl_0 =
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl;
    cfg->top_vlan_id_0 =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid;
    cfg->top_is_svlan_0 =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind;
    cfg->inner_vlan_id_0 =
        (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid << 6) | (cls_key_data.key_data7.bf2.
                cl_key_tre_l2rl2r_1_inner_vid);
    cfg->inner_is_svlan_0 =
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind;

    cfg->dot1p_ctrl_0 = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl;

    cfg->vlan_number_vld_0 = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x1) == 0);
    cfg->vlan_number_0 =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num;


    cfg->ethernet_type_vld_0 = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x2) == 0);
    cfg->ethernet_type_0 = cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt;
    cfg->field_is_or_0 = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator;

    cfg->l4_src_port_0 = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp;
    cfg->l4_dst_port_0 = (cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp | ((cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp << 4) & 0xfff0));

    cfg->mac_field_ctrl_0 =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl;

    cfg->mac_da_0[0] =
        (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 11) & 0xff;
    cfg->mac_da_0[1] =
        (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 3) & 0xff;
    cfg->mac_da_0[2] =
        ((cls_key_data.key_data11.bf2.
          cl_key_tre_l2rl2r_1_mac_da & 7) << 5 & 0xE0) | ((cls_key_data.key_data10.bf2.
                  cl_key_tre_l2rl2r_1_mac_da
                  >> 24) & 0x1f);
    cfg->mac_da_0[3] =
        (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 16) & 0xff;
    cfg->mac_da_0[4] =
        (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 8) & 0xff;
    cfg->mac_da_0[5] =
        (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 0) & 0xff;
    cfg->mac_sa_0[0] =
        ((cls_key_data.key_data10.bf2.
          cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data9.bf2.
                  cl_key_tre_l2rl2r_1_mac_sa
                  >> 27) & 0x1f);
    cfg->mac_sa_0[1] =
        (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 19) & 0xff;
    cfg->mac_sa_0[2] =
        (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 11) & 0xff;
    cfg->mac_sa_0[3] =
        (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 3) & 0xff;
    cfg->mac_sa_0[4] =
        ((cls_key_data.key_data9.bf2.
          cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data8.bf2.
                  cl_key_tre_l2rl2r_1_mac_sa
                  >> 8) & 0x1f);
    cfg->mac_sa_0[5] =
        (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl;
    cfg->precedence_0 =cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd;

    cfg->precedence_1 =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd;

    cfg->field_is_or_1 = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator;

    cfg->l4_src_port_1 = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp;
    cfg->l4_dst_port_1 = (cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp | ((cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp << 1) & 0xfffe));

    cfg->dot1p_ctrl_1 = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl;
    cfg->top_dot1p_1 =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p;
    cfg->inner_dot1p_1 =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p;
    cfg->vlan_number_vld_1 = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x1) == 0);
    cfg->vlan_number_1 =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num;

    cfg->vlan_field_ctrl_1 =
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl;
    cfg->top_vlan_id_1 =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid;
    cfg->top_is_svlan_1 =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind;
    cfg->inner_vlan_id_1 =
        (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf2.
                cl_key_tre_l2rl2r_2_inner_vid);
    cfg->inner_is_svlan_1 =
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind;
    cfg->ethernet_type_1 = cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt;
    cfg->mac_field_ctrl_1 =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl;
     cfg->mac_da_1[0] =
         (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 14) & 0xff;
     cfg->mac_da_1[1] =
         (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 6) & 0xff;
     cfg->mac_da_1[2] =
         ((cls_key_data.key_data5.bf2.
           cl_key_tre_l2rl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf2.
                   cl_key_tre_l2rl2r_2_mac_da
                   >> 24) & 0x3);
     cfg->mac_da_1[3] =
         (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 16) & 0xff;
     cfg->mac_da_1[4] =
         (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 8) & 0xff;
     cfg->mac_da_1[5] =
         (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 0) & 0xff;
     cfg->mac_sa_1[0] =
         ((cls_key_data.key_data4.bf2.
           cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf2.
                   cl_key_tre_l2rl2r_2_mac_sa
                   >> 30) & 0x3);
     cfg->mac_sa_1[1] =
         (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 22) & 0xff;
     cfg->mac_sa_1[2] =
         (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 14) & 0xff;
     cfg->mac_sa_1[3] =
         (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 6) & 0xff;
     cfg->mac_sa_1[4] =
         ((cls_key_data.key_data3.bf2.
           cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf2.
                   cl_key_tre_l2rl2r_2_mac_sa
                   >> 8) & 0x3);
     cfg->mac_sa_1[5] =
         (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 0) & 0xff;

     cfg->l4_port_field_ctrl_1 =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl;


   cfg->ethernet_type_vld_1 = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x2) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_l2rl3r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l2rl3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l2rl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }


   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;


    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_inner_vid =
            (cfg->inner_vlan_id_0 >> 6) & 0x3f;
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_vid =
            (cfg->inner_vlan_id_0) & 0x3f;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.mac_field_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mac_ctrl =
            cfg->mac_field_ctrl_0;
    }
    if (mask.s.mac_da_0) {
       cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da =
            ((cfg->mac_da_0[0] << 11) | (cfg->mac_da_0[1] << 3) | ((cfg->mac_da_0[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da =
            (((cfg->mac_da_0[2] & 0x1f) << 24) | (cfg->mac_da_0[3] << 16) | (cfg->mac_da_0[4] << 8) | cfg->mac_da_0[5]);
    }
    if (mask.s.mac_sa_0) {
       cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_sa =
            (cfg->mac_sa_0[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa =
            (((cfg->mac_sa_0[0] & 0x1f) << 27) | (cfg->mac_sa_0[1] << 19) |
             (cfg->mac_sa_0[2] << 11) | (cfg->mac_sa_0[3] << 3) | ((cfg->mac_sa_0[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_mac_sa =
            ((cfg->mac_sa_0[4] & 0x1f) << 8) | (cfg->mac_sa_0[5]);
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_sp =  cfg->l4_src_port_0;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_dp  = (cfg->l4_dst_port_0 & 0xf);
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_l4_dp  = (cfg->l4_dst_port_0 >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask &=	0x2;
        } else {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_tag_num = cfg->vlan_number_0;
    }


    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_1_prcd = cfg->precedence_0;
    }

    if(mask.s.ecn_1){
        cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ecn = cfg->ecn_1;
    }
    if(mask.s.ecn_valid_1){
        cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ecn_mask = (cfg->ecn_valid_1== 0);
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_vid = cfg->inner_vlan_id_1;
            }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.ip_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_ip_ctrl =
            cfg->ip_ctrl_1;
    }
    if (mask.s.ip_da_1) {
       cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ipv4_da =
            (cfg->ip_da_1 >> 12) & 0xfffff;
       cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_da =
            cfg->ip_da_1 & 0xfff;
    }
    if (mask.s.ip_sa_1) {
       cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_sa =
            (cfg->ip_sa_1 >> 12) & 0xfffff;
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_sa =
            cfg->ip_sa_1 & 0xfff;
    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_top_802_1p = (cfg->top_dot1p_1 & 1);
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_802_1p = ((cfg->top_dot1p_1 >> 1) & 3);
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_pt = cfg->ip_pt_1;
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 >> 4) & 1;
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid_1) {
        if (cfg->special_pkt_code_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x80;
        }
    }
    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_l2rl3r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l2rl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }


     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid                =           cls_key_data.key_data12.bf.cl_key_valid                           ;
    cfg->vlan_field_ctrl_0    =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_vid_ctrl           ;
    cfg->top_vlan_id_0        =           cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_vid            ;
    cfg->top_is_svlan_0       =           cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_sc_ind         ;
    cfg->inner_vlan_id_0      = (cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_inner_vid << 6) |cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_vid      ;
    cfg->inner_is_svlan_0     =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_sc_ind       ;
    cfg->mac_field_ctrl_0     =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mac_ctrl           ;
    cfg->mac_da_0[0]          = (cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da) >> 11 & 0xff  ;
    cfg->mac_da_0[1]          = (cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da) >> 3 & 0xff  ;
    cfg->mac_da_0[2]          = ((cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da & 0x07) << 5) | ((cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 24) & 0x1f);
    cfg->mac_da_0[3]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 16) & 0xff    ;
    cfg->mac_da_0[4]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 8) & 0xff     ;
    cfg->mac_da_0[5]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 0) & 0xff     ;
    cfg->mac_sa_0[0]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_sa << 5) | ((cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 27) & 0x1f);
    cfg->mac_sa_0[1]          = (cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 19) & 0xff;
    cfg->mac_sa_0[2]          = (cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 11) & 0xff;
    cfg->mac_sa_0[3]          = (cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >>  3) & 0xff;
    cfg->mac_sa_0[4]          = (((cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa & 0x7) << 5) | ((cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 8) & 0xff));
    cfg->mac_sa_0[5]          = (cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_mac_sa & 0xff);
    cfg->top_dot1p_0          =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_top_802_1p         ;
    cfg->inner_dot1p_0        =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_802_1p       ;
    cfg->dot1p_ctrl_0         =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_0 =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_ctrl            ;
    cfg->l4_src_port_0        =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_sp              ;
    cfg->l4_dst_port_0        = (cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_l4_dp << 4) |cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_dp              ;
    cfg->vlan_number_0        =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_tag_num            ;
    cfg->ethernet_type_0      =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_ev2_pt             ;
    cfg->field_is_or_0        =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_fields_operator    ;
    cfg->precedence_0         =           cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_1_prcd               ;
    cfg->vlan_number_vld_0    = ((cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask & 0x2)  == 0x00);
    cfg->ethernet_type_vld_0  = ((cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask & 0x1)  == 0x00);

    cfg->ecn_1                     =  cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ecn;
    cfg->ecn_valid_1               =  (cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ecn_mask== 0);
    cfg->vlan_field_ctrl_1    =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_vid_ctrl           ;
    cfg->top_vlan_id_1        =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_vid            ;
    cfg->top_is_svlan_1       =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id_1      =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_vid          ;
    cfg->inner_is_svlan_1     =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl_1            =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_ip_ctrl            ;
    cfg->ip_da_1              = (cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_da)          ;
    cfg->ip_sa_1              = (cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_sa)         ;
    cfg->top_dot1p_1          =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_top_802_1p    | (cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p_1        =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl_1         =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_1 =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_ctrl            ;
    cfg->l4_src_port_1        =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_sp | (cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port_1        =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_dp              ;
    cfg->vlan_number_1        =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_tag_num            ;
    cfg->ethernet_type_1      =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_ev2_pt             ;
    cfg->field_is_or_1        =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_fields_operator    ;
    cfg->precedence_1         =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_prcd               ;
    cfg->ip_pt_1              =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_pt              ;
    cfg->special_pkt_code_1   = (cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_special_pkt_code    ;
    cfg->dscp_1               =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_tos             ;
    cfg->has_ipop_1           =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_option        ;
    cfg->l4_flag_1            =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_l4_flags           ;
    cfg->ipv4_fragment_1      =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_fragment      ;


    cfg->ipv4_fragment_valid_1 = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_1      = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_1    = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_1        = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_1     = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid_1         = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_1  = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_1 = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x80) == 0x00);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}

ca_status_t aal_l2_cls_l2rcmr_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l2rcmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_l2rcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;


    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_inner_vid =
            (cfg->inner_vlan_id_0 >> 6) & 0x3f;
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_vid =
            (cfg->inner_vlan_id_0) & 0x3f;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.mac_field_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mac_ctrl =
            cfg->mac_field_ctrl_0;
    }
    if (mask.s.mac_da_0) {
       cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da =
            ((cfg->mac_da_0[0] << 11) | (cfg->mac_da_0[1] << 3) | ((cfg->mac_da_0[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da =
            (((cfg->mac_da_0[2] & 0x1f) << 24) | (cfg->mac_da_0[3] << 16) | (cfg->mac_da_0[4] << 8) | cfg->mac_da_0[5]);
    }
    if (mask.s.mac_sa_0) {
       cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_sa =
            (cfg->mac_sa_0[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa =
            (((cfg->mac_sa_0[0] & 0x1f) << 27) | (cfg->mac_sa_0[1] << 19) |
             (cfg->mac_sa_0[2] << 11) | (cfg->mac_sa_0[3] << 3) | ((cfg->mac_sa_0[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_mac_sa =
            ((cfg->mac_sa_0[4] & 0x1f) << 8) | (cfg->mac_sa_0[5]);
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_sp =  cfg->l4_src_port_0;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_dp  = (cfg->l4_dst_port_0 & 0xf);
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_l4_dp  = (cfg->l4_dst_port_0 >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask &=   0x2;
        } else {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_tag_num = cfg->vlan_number_0;
    }


    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_1_prcd = cfg->precedence_0;
    }
    if(mask.s.ecn_1){
        cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_ecn = cfg->ecn_1;
    }
    if(mask.s.ecn_valid_1){
        cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_ecn_mask = (cfg->ecn_valid_1== 0);
    }

    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.vlan_field_valid_1) {
        if (cfg->vlan_field_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa_1;
    }
    if (mask.s.ip_addr_1) {
       cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_ipv4_addr =
            (cfg->ip_addr_1 >> 28) & 0xf;
       cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ipv4_addr =
            cfg->ip_addr_1 & 0xfffffff;
    }
    if (mask.s.ip_addr_valid_1) {
        if (cfg->ip_addr_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa_1;
    }

    if (mask.s.mac_addr_1) {
       cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr =
            (cfg->mac_addr_1[0] << 12) | (cfg->mac_addr_1[1] << 4) | ((cfg->mac_addr_1[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr =
            ((cfg->mac_addr_1[2] & 0xf) << 24) | (cfg->mac_addr_1[3] << 16) | (cfg->mac_addr_1[4] << 8) | (cfg->mac_addr_1[5]);
    }

    if (mask.s.mac_addr_valid_1) {
        if (cfg->mac_addr_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.dot1p_valid_1) {
        if (cfg->dot1p_valid_1)
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ip_pt = ((cfg->ip_pt_1 >> 4) & 0x0f);
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_pt = (cfg->ip_pt_1 & 0x0f);
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_special_pkt_code = cfg->special_pkt_code_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x080;
        }
    }


    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;


}


ca_status_t aal_l2_cls_l2rcmr_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l2rcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid                =   cls_key_data.key_data12.bf.cl_key_valid                           ;
    cfg->vlan_field_ctrl_0    =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_vid_ctrl           ;
    cfg->top_vlan_id_0        =   cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_vid            ;
    cfg->top_is_svlan_0       =   cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_sc_ind         ;
    cfg->inner_vlan_id_0      = (cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_inner_vid << 6) |cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_vid      ;
    cfg->inner_is_svlan_0     =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_sc_ind       ;
    cfg->mac_field_ctrl_0     =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mac_ctrl           ;
    cfg->mac_da_0[0]          = (cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da) >> 11 & 0xff  ;
    cfg->mac_da_0[1]          = (cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da) >> 3 & 0xff  ;
    cfg->mac_da_0[2]          = ((cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da & 0x07) << 5) | ((cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 24) & 0x1f);
    cfg->mac_da_0[3]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 16) & 0xff    ;
    cfg->mac_da_0[4]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 8) & 0xff     ;
    cfg->mac_da_0[5]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 0) & 0xff     ;
    cfg->mac_sa_0[0]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_sa << 5) | ((cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 27) & 0x1f);
    cfg->mac_sa_0[1]          = (cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 19) & 0xff;
    cfg->mac_sa_0[2]          = (cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 11) & 0xff;
    cfg->mac_sa_0[3]          = (cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >>  3) & 0xff;
    cfg->mac_sa_0[4]          = (((cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa & 0x7) << 5) | ((cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 8) & 0xff));
    cfg->mac_sa_0[5]          = (cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_mac_sa & 0xff);
    cfg->top_dot1p_0          =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_top_802_1p         ;
    cfg->inner_dot1p_0        =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_802_1p       ;
    cfg->dot1p_ctrl_0         =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_0 =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_ctrl            ;
    cfg->l4_src_port_0        =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_sp              ;
    cfg->l4_dst_port_0        = (cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_l4_dp << 4) |cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_dp              ;
    cfg->vlan_number_0        =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_tag_num            ;
    cfg->ethernet_type_0      =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_ev2_pt             ;
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask & 0x2)  == 0x00);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask & 0x1)  == 0x00);
    cfg->field_is_or_0        =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_fields_operator    ;
    cfg->precedence_0         =   cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_1_prcd               ;

    cfg->ecn_1                     =  cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_ecn;
    cfg->ecn_valid_1               =  (cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_ecn_mask== 0);
    cfg->top_vlan_id_1          =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_top_vid;
    cfg->top_is_svlan_1         =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_sc_ind;
    cfg->ip_addr_is_sa_1        =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_ipv4_addr_sel;
    cfg->ip_addr_1              =cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ipv4_addr;
    cfg->mac_addr_is_sa_1       =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mac_addr_sel;
    cfg->mac_addr_1[0]          =cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 12 & 0xff;
    cfg->mac_addr_1[1]          =cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 4 & 0xff;
    cfg->mac_addr_1[2]          = (((cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 24) & 0xf));
    cfg->mac_addr_1[3]          = (cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 16 & 0xff);
    cfg->mac_addr_1[4]          = (cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 8 & 0xff);
    cfg->mac_addr_1[5]          = (cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_1            =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_802_1p;
    cfg->l4_port_field_ctrl_1   =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_ctrl;
    cfg->l4_src_port_1          =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_sp | (cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_sp << 11);
    cfg->l4_dst_port_1          =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_dp;
    cfg->vlan_number_1          =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_tag_num;
    cfg->ethernet_type_1        =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_ev2_pt;
    cfg->field_is_or_1          =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_fields_operator;
    cfg->precedence_1           =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_prcd;
    cfg->ip_pt_1                =cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_pt;
    cfg->dscp_1                 =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_tos;
    cfg->has_ipop_1             =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_option;
    cfg->l4_flag_1              =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_l4_flags;
    cfg->ipv4_fragment_1        =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_fragment;
    cfg->special_pkt_code_1        =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_special_pkt_code;
    cfg->dot1p_valid_1             = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid_1        = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid_1           = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid_1          = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_1  = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid_1           = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_1     = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid_1          = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid_1              = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid_1             = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld_1       = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld_1         = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x001) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;
}




ca_status_t aal_l2_cls_l3rl2r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3rl2r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3rl2r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;

  __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;


   if(mask.s.ecn_0){
      cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn = cfg->ecn_0;
   }
   if(mask.s.ecn_valid_0){
      cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn_mask = (cfg->ecn_valid_0== 0);
   }

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_inner_vid = cfg->inner_vlan_id_0 ;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.ip_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_ip_ctrl =
            cfg->ip_ctrl_0;
    }
    if (mask.s.ip_da_0) {
       cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ipv4_da = (cfg->ip_da_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_da = cfg->ip_da_0 & 0x7FFF;
    }
    if (mask.s.ip_sa_0) {
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_sa =
            (cfg->ip_sa_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_sa = cfg->ip_sa_0 & 0x7FFF;
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_pt = cfg->ip_pt_0;
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_l4_flags = ((cfg->l4_flag_0>> 2) & 0x01);
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_l4_flags = (cfg->l4_flag_0 & 0x03);
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_1_prcd = cfg->precedence_0;
    }
    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1 >> 3) & 0x1ff;
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1) & 0x7;
    }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.mac_field_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mac_ctrl =
            cfg->mac_field_ctrl_1;
    }
    if (mask.s.mac_da_1) {
       cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da =
            ((cfg->mac_da_1[0] << 14) | (cfg->mac_da_1[1] << 6) | ((cfg->
                    mac_da_1[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da =
            (((cfg->mac_da_1[2] & 0x3) << 24) | (cfg->mac_da_1[3] << 16) |
             (cfg->mac_da_1[4] << 8) | cfg->mac_da_1[5]);
    }
    if (mask.s.mac_sa_1) {
       cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_sa =
            (cfg->mac_sa_1[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa =
            (((cfg->mac_sa_1[0] & 0x3) << 30) | (cfg->mac_sa_1[1] << 22) |
             (cfg->mac_sa_1[2] << 14) | (cfg->mac_sa_1[3] << 6) | ((cfg->
                     mac_sa_1[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_mac_sa =
            ((cfg->mac_sa_1[4] & 0x3) << 8) | (cfg->mac_sa_1[5]);

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_sp =  cfg->l4_src_port_1;
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_dp = (cfg->l4_dst_port_1 & 1);
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_l4_dp = (cfg->l4_dst_port_1 >> 1) & 0x7fff;
    }
    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask |= 0x01;
        }
    }
    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_prcd = cfg->precedence_1;
    }
    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_l3rl2r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3rl2r_key_t *cfg)

{

    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->vlan_field_ctrl_0 =
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_vid_ctrl;
    cfg->top_vlan_id_0      =cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_vid;
    cfg->top_is_svlan_0     =cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_sc_ind;
    cfg->inner_vlan_id_0    = (cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_inner_vid);
    cfg->inner_is_svlan_0   =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_sc_ind;
    cfg->top_dot1p_0        =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_top_802_1p;
    cfg->inner_dot1p_0      =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_802_1p;
    cfg->field_is_or_0      =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_fields_operator;
    cfg->l4_src_port_0      = cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_sp | (cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_sp <<14);
    cfg->l4_dst_port_0      = (cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_dp);
    cfg->vlan_number_0      =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_tag_num;
    cfg->precedence_0       =cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_1_prcd;
    cfg->ethernet_type_0    =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_ev2_pt;
    cfg->ip_ctrl_0          =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_ip_ctrl;
    cfg->dot1p_ctrl_0       =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_dot1p_ctrl;
    cfg->ip_da_0            =cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ipv4_da <<15 |cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_da;
    cfg->ip_sa_0            =cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_sa <<15 |cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_sa;
    cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_ctrl;
    cfg->ipv4_fragment_0    =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_fragment;
    cfg->l4_flag_0          =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_l4_flags<<2 | cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_l4_flags;
    cfg->has_ipop_0         =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_option;
    cfg->dscp_0             =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_tos;
    cfg->ip_pt_0            =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_pt;
    cfg->special_pkt_code_0 =cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_special_pkt_code;
    cfg->ipv4_fragment_valid_0   = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_0         = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_0       = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_0           = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_0        = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x10) == 0x00);
    cfg->dscp_valid_0            = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_0     = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_0 =((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x80) == 0x00);

    cfg->ecn_0                     =  cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn;
    cfg->ecn_valid_0               =  (cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn_mask== 0);

    cfg->mac_field_ctrl_1 =cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mac_ctrl;
    cfg->mac_da_1[0] =
        (cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da_1[1] =
        (cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da_1[2] =
        ((cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da
                  >> 24) & 0x3);
    cfg->mac_da_1[3] =
        (cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da_1[4] =
        (cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da_1[5] =
        (cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa_1[0] =
        ((cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa
                  >> 30) & 0x3);
    cfg->mac_sa_1[1] =
        (cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa_1[2] =
        (cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa_1[3] =
        (cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa_1[4] =
        ((cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_mac_sa
                  >> 8) & 0x3);
    cfg->mac_sa_1[5] =
        (cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_1 =cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_ctrl;
    cfg->precedence_1 =cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_prcd;
    cfg->field_is_or_1 = cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_fields_operator;
    cfg->l4_src_port_1 = cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_sp;
    cfg->l4_dst_port_1 = (cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_dp | ((cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_l4_dp << 1) & 0xfffe));
    cfg->dot1p_ctrl_1 = cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_dot1p_ctrl;
    cfg->top_dot1p_1 =cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_top_802_1p;
    cfg->inner_dot1p_1 =cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_802_1p;
    cfg->ethernet_type_vld_1 = ((cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask & 0x2) == 0);
    cfg->ethernet_type_1 = cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_ev2_pt;
    cfg->vlan_field_ctrl_1 =
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_vid_ctrl;
    cfg->top_vlan_id_1 =cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_vid;
    cfg->top_is_svlan_1 =cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_sc_ind;
    cfg->inner_vlan_id_1 =
        (cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_vid);
    cfg->inner_is_svlan_1 =
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_sc_ind;
    cfg->vlan_number_vld_1 = ((cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask & 0x1) == 0);
    cfg->vlan_number_1 =cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_tag_num;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_l3rl3r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3rl3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3rl3r_key_t *cfg)


{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;

    if(mask.s.ecn_0){
       cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ecn = cfg->ecn_0;
    }
    if(mask.s.ecn_valid_0){
       cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ecn_mask = (cfg->ecn_valid_0== 0);
    }

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_inner_vid = cfg->inner_vlan_id_0 ;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.ip_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_ip_ctrl =
            cfg->ip_ctrl_0;
    }
    if (mask.s.ip_da_0) {
       cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ipv4_da = (cfg->ip_da_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_da = cfg->ip_da_0 & 0x7FFF;
    }
    if (mask.s.ip_sa_0) {
       cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_sa =
            (cfg->ip_sa_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_sa = cfg->ip_sa_0 & 0x7FFF;
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_pt = cfg->ip_pt_0;
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag_0) {
        cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_l4_flags = ((cfg->l4_flag_0>> 2) & 0x01);
        cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_l4_flags = (cfg->l4_flag_0 & 0x03);
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_1_prcd = cfg->precedence_0;
    }

    if(mask.s.ecn_1){
        cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn = cfg->ecn_1;
    }
    if(mask.s.ecn_valid_1){
        cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn_mask = (cfg->ecn_valid_1== 0);
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid = cfg->inner_vlan_id_1;
            }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.ip_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl =
            cfg->ip_ctrl_1;
    }
    if (mask.s.ip_da_1) {
       cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            (cfg->ip_da_1 >> 12) & 0xfffff;
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            cfg->ip_da_1 & 0xfff;
    }
    if (mask.s.ip_sa_1) {
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            (cfg->ip_sa_1 >> 12) & 0xfffff;
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            cfg->ip_sa_1 & 0xfff;
    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p = (cfg->top_dot1p_1 & 1);
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p = ((cfg->top_dot1p_1 >> 1) & 3);
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt = cfg->ip_pt_1;
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 >> 4) & 1;
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid_1) {
        if (cfg->special_pkt_code_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x80;
        }
    }


    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_l3rl3r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3rl3r_key_t *cfg)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->vlan_field_ctrl_0 =
      cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_vid_ctrl;
    cfg->top_vlan_id_0      =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_vid;
    cfg->top_is_svlan_0     =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_sc_ind;
    cfg->inner_vlan_id_0    = (cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_inner_vid);
    cfg->inner_is_svlan_0   =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_sc_ind;
    cfg->top_dot1p_0        =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_top_802_1p;
    cfg->inner_dot1p_0      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_802_1p;
    cfg->field_is_or_0      =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_fields_operator;
    cfg->l4_src_port_0      = cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_sp | (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_sp <<14);
    cfg->l4_dst_port_0      = (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_dp);
    cfg->vlan_number_0      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_tag_num;
    cfg->precedence_0       =cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_1_prcd;
    cfg->ethernet_type_0    =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_ev2_pt;
    cfg->ip_ctrl_0          =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_ip_ctrl;
    cfg->dot1p_ctrl_0       =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_dot1p_ctrl;
    cfg->ip_da_0            =cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ipv4_da <<15 |cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_da;
    cfg->ip_sa_0            =cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_sa <<15 |cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_sa;
    cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_ctrl;
    cfg->ipv4_fragment_0    =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_fragment;
    cfg->l4_flag_0          =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_l4_flags<<2 | cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_l4_flags;
    cfg->has_ipop_0         =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_option;
    cfg->dscp_0             =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_tos;
    cfg->ip_pt_0            =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_pt;
    cfg->special_pkt_code_0 =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_special_pkt_code;
    cfg->ipv4_fragment_valid_0   = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_0         = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_0       = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_0           = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_0        = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x10) == 0x00);
    cfg->dscp_valid_0            = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_0     = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_0 =((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x80) == 0x00);

    cfg->ecn_0                     =  cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ecn;
    cfg->ecn_valid_0               =  (cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ecn_mask== 0);

    cfg->ecn_1                     =  cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn;
    cfg->ecn_valid_1               =  (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn_mask== 0);

    cfg->vlan_field_ctrl_1    =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl           ;
    cfg->top_vlan_id_1        =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid            ;
    cfg->top_is_svlan_1       =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id_1      =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid          ;
    cfg->inner_is_svlan_1     =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl_1            =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl            ;
    cfg->ip_da_1              = (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da)          ;
    cfg->ip_sa_1              = (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa)         ;
    cfg->top_dot1p_1          =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p    | (cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p_1        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl_1         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_1 =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl            ;
    cfg->l4_src_port_1        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp | (cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port_1        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp              ;
    cfg->vlan_number_1        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num            ;
    cfg->ethernet_type_1      =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt             ;
    cfg->field_is_or_1        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator    ;
    cfg->precedence_1         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd               ;
    cfg->ip_pt_1              =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt              ;
    cfg->special_pkt_code_1   = (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code    ;
    cfg->dscp_1               =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos             ;
    cfg->has_ipop_1           =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option        ;
    cfg->l4_flag_1            =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags           ;
    cfg->ipv4_fragment_1      =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment      ;
    cfg->ipv4_fragment_valid_1  = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_1        = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_1      = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_1          = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_1       = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid_1           = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_1    = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_1=((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x80) == 0x00);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}


ca_status_t aal_l2_cls_l3rcmr_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3rcmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3rcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;

    if(mask.s.ecn_0){
       cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ecn = cfg->ecn_0;
    }
    if(mask.s.ecn_valid_0){
       cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ecn_mask = (cfg->ecn_valid_0== 0);
    }

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_inner_vid = cfg->inner_vlan_id_0 ;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.ip_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_ip_ctrl =
            cfg->ip_ctrl_0;
    }
    if (mask.s.ip_da_0) {
       cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ipv4_da = (cfg->ip_da_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_da = cfg->ip_da_0 & 0x7FFF;
    }
    if (mask.s.ip_sa_0) {
       cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_sa =
            (cfg->ip_sa_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_sa = cfg->ip_sa_0 & 0x7FFF;
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_pt = cfg->ip_pt_0;
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag_0) {
        cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_l4_flags = ((cfg->l4_flag_0>> 2) & 0x01);
        cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_l4_flags = (cfg->l4_flag_0 & 0x03);
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_1_prcd = cfg->precedence_0;
    }
    if(mask.s.ecn_1){
        cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_ecn = cfg->ecn_1;
    }
    if(mask.s.ecn_valid_1){
        cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_ecn_mask = (cfg->ecn_valid_1== 0);
    }

    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.vlan_field_valid_1) {
        if (cfg->vlan_field_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa_1;
    }
    if (mask.s.ip_addr_1) {
       cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_ipv4_addr =
            (cfg->ip_addr_1 >> 28) & 0xf;
       cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ipv4_addr =
            cfg->ip_addr_1 & 0xfffffff;
    }
    if (mask.s.ip_addr_valid_1) {
        if (cfg->ip_addr_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa_1;
    }

    if (mask.s.mac_addr_1) {
       cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr =
            (cfg->mac_addr_1[0] << 12) | (cfg->mac_addr_1[1] << 4) | ((cfg->mac_addr_1[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr =
            ((cfg->mac_addr_1[2] & 0xf) << 24) | (cfg->mac_addr_1[3] << 16) | (cfg->mac_addr_1[4] << 8) | (cfg->mac_addr_1[5]);
    }

    if (mask.s.mac_addr_valid_1) {
        if (cfg->mac_addr_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.dot1p_valid_1) {
        if (cfg->dot1p_valid_1)
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ip_pt = ((cfg->ip_pt_1 >> 4) & 0x0f);
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_pt = (cfg->ip_pt_1 & 0x0f);
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_special_pkt_code = cfg->special_pkt_code_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x080;
        }
    }


    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_l3rcmr_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3rcmr_key_t *cfg)

{

   ca_status_t ret = CA_E_OK;
   __aal_cls_key_data_t cls_key_data;


   if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
   }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

   cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
   cfg->vlan_field_ctrl_0 =
     cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_vid_ctrl;
   cfg->top_vlan_id_0      =cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_vid;
   cfg->top_is_svlan_0     =cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_sc_ind;
   cfg->inner_vlan_id_0    = (cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_inner_vid);
   cfg->inner_is_svlan_0   =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_sc_ind;
   cfg->top_dot1p_0        =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_top_802_1p;
   cfg->inner_dot1p_0      =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_802_1p;
   cfg->field_is_or_0      =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_fields_operator;
   cfg->l4_src_port_0      = cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_sp | (cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_sp <<14);
   cfg->l4_dst_port_0      = (cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_dp);
   cfg->vlan_number_0      =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_tag_num;
   cfg->precedence_0       =cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_1_prcd;
   cfg->ethernet_type_0    =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_ev2_pt;
   cfg->ip_ctrl_0          =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_ip_ctrl;
   cfg->dot1p_ctrl_0       =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_dot1p_ctrl;
   cfg->ip_da_0            =cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ipv4_da <<15 |cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_da;
   cfg->ip_sa_0            =cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_sa <<15 |cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_sa;
   cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_ctrl;
   cfg->ipv4_fragment_0    =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_fragment;
   cfg->l4_flag_0          =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_l4_flags<<2 | cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_l4_flags;
   cfg->has_ipop_0         =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_option;
   cfg->dscp_0             =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_tos;
   cfg->ip_pt_0            =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_pt;
   cfg->special_pkt_code_0 =cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_special_pkt_code;
   cfg->ipv4_fragment_valid_0   = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x20) == 0x00);
   cfg->l4_flag_valid_0         = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x40) == 0x00);
   cfg->vlan_number_vld_0       = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x01) == 0x00);
   cfg->ip_pt_valid_0           = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x04) == 0x00);
   cfg->has_ipop_valid_0        = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x10) == 0x00);
   cfg->dscp_valid_0            = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x08) == 0x00);
   cfg->ethernet_type_vld_0     = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x02) == 0x00);
   cfg->special_pkt_code_valid_0 =((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x80) == 0x00);
   cfg->ecn_0                     =  cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ecn;
   cfg->ecn_valid_0               =  (cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ecn_mask== 0);

   cfg->ecn_1                     =  cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_ecn;
   cfg->ecn_valid_1               =  (cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_ecn_mask== 0);
   cfg->top_vlan_id_1          =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_top_vid;
   cfg->top_is_svlan_1         =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_sc_ind;
   cfg->ip_addr_is_sa_1        =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_ipv4_addr_sel;
   cfg->ip_addr_1              =cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ipv4_addr;
   cfg->mac_addr_is_sa_1       =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mac_addr_sel;
   cfg->mac_addr_1[0]          =cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 12 & 0xff;
   cfg->mac_addr_1[1]          =cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 4 & 0xff;
   cfg->mac_addr_1[2]          = (((cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 24) & 0xf));
   cfg->mac_addr_1[3]          = (cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 16 & 0xff);
   cfg->mac_addr_1[4]          = (cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 8 & 0xff);
   cfg->mac_addr_1[5]          = (cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 0 & 0xff);
   cfg->top_dot1p_1            =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_802_1p;
   cfg->l4_port_field_ctrl_1   =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_ctrl;
   cfg->l4_src_port_1          =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_sp | (cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_sp << 11);
   cfg->l4_dst_port_1          =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_dp;
   cfg->vlan_number_1          =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_tag_num;
   cfg->ethernet_type_1        =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_ev2_pt;
   cfg->field_is_or_1          =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_fields_operator;
   cfg->precedence_1           =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_prcd;
   cfg->ip_pt_1                =cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_pt;
   cfg->dscp_1                 =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_tos;
   cfg->has_ipop_1             =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_option;
   cfg->l4_flag_1              =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_l4_flags;
   cfg->ipv4_fragment_1        =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_fragment;
   cfg->special_pkt_code_1        =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_special_pkt_code;
    cfg->dot1p_valid_1             = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid_1        = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid_1           = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid_1          = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_1  = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid_1           = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_1     = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid_1          = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid_1              = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid_1             = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld_1       = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld_1         = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x001) == 0);



#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

   return ret;
}



ca_status_t aal_l2_cls_cmrl2r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmrl2r_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmrl2r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;



    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;

    if(mask.s.ecn_0){
       cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_ecn = cfg->ecn_0;
    }
    if(mask.s.ecn_valid_0){
       cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_ecn_mask = (cfg->ecn_valid_0== 0);
    }


    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_top_vid = (cfg->top_vlan_id_0 >> 3) & 0x1ff;
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_vid = cfg->top_vlan_id_0 & 0x07;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.vlan_field_valid_0) {
        if (cfg->vlan_field_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xBFF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x400;
        }
    }

   if (mask.s.ip_addr_is_sa_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_ipv4_addr_sel =
            cfg->ip_addr_is_sa_0;
    }
    if (mask.s.ip_addr_0) {
       cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_ipv4_addr =
            (cfg->ip_addr_0 >> 31) & 0x01;
       cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ipv4_addr =
            cfg->ip_addr_0 & 0x7fffffff;
    }
    if (mask.s.ip_addr_valid_0) {
        if (cfg->ip_addr_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xDFF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mac_addr_sel =
            cfg->mac_addr_is_sa_0;
    }

    if (mask.s.mac_addr_0) {
       cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr =
            (cfg->mac_addr_0[0] << 9) | (cfg->mac_addr_0[1] << 1) | ((cfg->mac_addr_0[2] >> 7) & 0x01);
       cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr =
            ((cfg->mac_addr_0[2] & 0x7f) << 24) | (cfg->mac_addr_0[3] << 16) | (cfg->mac_addr_0[4] << 8) | (cfg->mac_addr_0[5]);
    }

    if (mask.s.mac_addr_valid_0) {
        if (cfg->mac_addr_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xEFF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x100;
        }

    }

    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.dot1p_valid_0) {
        if (cfg->dot1p_valid_0 ) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0x7FF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x800;
        }
    }

    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &=   0xFFE;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x001;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFEF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x010;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFF7;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x008;
        }
    }
    if (mask.s.ip_pt_0) {
        cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ip_pt = ((cfg->ip_pt_0 >>7) & 1);
        cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_pt = (cfg->ip_pt_0 &0x7f);
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFFB;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x004;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFDF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x020;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFBF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x040;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xF7F;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x080;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFFD;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_1_prcd = cfg->precedence_0;
    }
    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_inner_vid =
            (cfg->inner_vlan_id_1 >> 3) & 0x1ff;
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_vid =
            (cfg->inner_vlan_id_1) & 0x7;
    }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.mac_field_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mac_ctrl =
            cfg->mac_field_ctrl_1;
    }
    if (mask.s.mac_da_1) {
       cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da =
            ((cfg->mac_da_1[0] << 14) | (cfg->mac_da_1[1] << 6) | ((cfg->
                    mac_da_1[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da =
            (((cfg->mac_da_1[2] & 0x3) << 24) | (cfg->mac_da_1[3] << 16) |
             (cfg->mac_da_1[4] << 8) | cfg->mac_da_1[5]);
    }
    if (mask.s.mac_sa_1) {
       cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_sa =
            (cfg->mac_sa_1[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa =
            (((cfg->mac_sa_1[0] & 0x3) << 30) | (cfg->mac_sa_1[1] << 22) |
             (cfg->mac_sa_1[2] << 14) | (cfg->mac_sa_1[3] << 6) | ((cfg->
                     mac_sa_1[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_mac_sa =
            ((cfg->mac_sa_1[4] & 0x3) << 8) | (cfg->mac_sa_1[5]);

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_sp =  cfg->l4_src_port_1;
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_dp = (cfg->l4_dst_port_1 & 1);
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_l4_dp = (cfg->l4_dst_port_1 >> 1) & 0x7fff;
    }
    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask |= 0x01;
        }
    }
    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_prcd = cfg->precedence_1;
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;



}




ca_status_t aal_l2_cls_cmrl2r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmrl2r_key_t *cfg)

{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }
     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;

    cfg->top_vlan_id_0          =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_top_vid <<3 |cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_vid;
    cfg->top_is_svlan_0         =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_sc_ind;
    cfg->ip_addr_is_sa_0        =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_ipv4_addr_sel;
    cfg->ip_addr_0              =cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_ipv4_addr << 31 |cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ipv4_addr;
    cfg->mac_addr_is_sa_0       =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mac_addr_sel;
    cfg->mac_addr_0[0]          =cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr_0[1]          =cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr_0[2]          = (((cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr_0[3]          = (cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr_0[4]          = (cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr_0[5]          = (cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_0            =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_802_1p;
    cfg->l4_port_field_ctrl_0   =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_ctrl;
    cfg->l4_src_port_0          =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_sp | (cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_sp << 14);
    cfg->l4_dst_port_0          =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_dp;
    cfg->vlan_number_0          =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_tag_num;
    cfg->ethernet_type_0        =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_ev2_pt;
    cfg->field_is_or_0          =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_fields_operator;
    cfg->precedence_0           =cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_1_prcd;
    cfg->ip_pt_0                =cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ip_pt << 7 |cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_pt;
    cfg->dscp_0                 =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_tos;
    cfg->has_ipop_0             =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_option;
    cfg->l4_flag_0              =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_l4_flags;
    cfg->ipv4_fragment_0        =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_fragment;
    cfg->special_pkt_code_0        =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_special_pkt_code;
    cfg->dot1p_valid_0             = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x800) == 0);
    cfg->vlan_field_valid_0        = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x400) == 0);
    cfg->ip_addr_valid_0           = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x200) == 0);
    cfg->mac_addr_valid_0          = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_0  = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x080) == 0);
    cfg->l4_flag_valid_0           = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_0     = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x020) == 0);
    cfg->has_ipop_valid_0          = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x010) == 0);
    cfg->dscp_valid_0              = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x008) == 0);
    cfg->ip_pt_valid_0             = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x002) == 0);
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x001) == 0);
    cfg->ecn_0                     =  cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_ecn;
    cfg->ecn_valid_0               =  (cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_ecn_mask== 0);

    cfg->mac_field_ctrl_1 =cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mac_ctrl;
    cfg->mac_da_1[0] =
        (cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da_1[1] =
        (cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da_1[2] =
        ((cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da
                  >> 24) & 0x3);
    cfg->mac_da_1[3] =
        (cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da_1[4] =
        (cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da_1[5] =
        (cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa_1[0] =
        ((cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa
                  >> 30) & 0x3);
    cfg->mac_sa_1[1] =
        (cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa_1[2] =
        (cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa_1[3] =
        (cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa_1[4] =
        ((cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_mac_sa
                  >> 8) & 0x3);
    cfg->mac_sa_1[5] =
        (cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_1 =cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_ctrl;
    cfg->precedence_1 =cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_prcd;
    cfg->field_is_or_1 = cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_fields_operator;
    cfg->l4_src_port_1 = cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_sp;
    cfg->l4_dst_port_1 = (cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_dp | ((cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_l4_dp << 1) & 0xfffe));
    cfg->dot1p_ctrl_1 = cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_dot1p_ctrl;
    cfg->top_dot1p_1 =cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_top_802_1p;
    cfg->inner_dot1p_1 =cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_802_1p;
    cfg->ethernet_type_vld_1 = ((cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask & 0x2) == 0);
    cfg->ethernet_type_1 = cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_ev2_pt;
    cfg->vlan_field_ctrl_1 =
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_vid_ctrl;
    cfg->top_vlan_id_1 =cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_vid;
    cfg->top_is_svlan_1 =cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_sc_ind;
    cfg->inner_vlan_id_1 =
        (cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_vid);
    cfg->inner_is_svlan_1 =
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_sc_ind;
    cfg->vlan_number_vld_1 = ((cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask & 0x1) == 0);
    cfg->vlan_number_1 =cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_tag_num;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;


}

ca_status_t aal_l2_cls_cmrl3r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmrl3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmrl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;



    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

     if (mask.s.valid) {
        cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
     }

    cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

    cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;
    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;

    if(mask.s.ecn_0){
       cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_ecn = cfg->ecn_0;
    }
    if(mask.s.ecn_valid_0){
       cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_ecn_mask = (cfg->ecn_valid_0== 0);
    }

     if (mask.s.top_vlan_id_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_top_vid = (cfg->top_vlan_id_0 >> 3) & 0x1ff;
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_vid = cfg->top_vlan_id_0 & 0x07;
     }
     if (mask.s.top_is_svlan_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_sc_ind =
             cfg->top_is_svlan_0;
     }
     if (mask.s.vlan_field_valid_0) {
         if (cfg->vlan_field_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xBFF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x400;
         }
     }

    if (mask.s.ip_addr_is_sa_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_ipv4_addr_sel =
             cfg->ip_addr_is_sa_0;
     }
     if (mask.s.ip_addr_0) {
        cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_ipv4_addr =
             (cfg->ip_addr_0 >> 31) & 0x01;
        cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ipv4_addr =
             cfg->ip_addr_0 & 0x7fffffff;
     }
     if (mask.s.ip_addr_valid_0) {
         if (cfg->ip_addr_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xDFF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x200;
         }
     }
     if (mask.s.mac_addr_is_sa_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mac_addr_sel =
             cfg->mac_addr_is_sa_0;
     }

     if (mask.s.mac_addr_0) {
        cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr =
             (cfg->mac_addr_0[0] << 9) | (cfg->mac_addr_0[1] << 1) | ((cfg->mac_addr_0[2] >> 7) & 0x01);
        cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr =
             ((cfg->mac_addr_0[2] & 0x7f) << 24) | (cfg->mac_addr_0[3] << 16) | (cfg->mac_addr_0[4] << 8) | (cfg->mac_addr_0[5]);
     }

     if (mask.s.mac_addr_valid_0) {
         if (cfg->mac_addr_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xEFF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x100;
         }

     }

     if (mask.s.top_dot1p_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_802_1p = cfg->top_dot1p_0;
     }

     if (mask.s.dot1p_valid_0) {
         if (cfg->dot1p_valid_0 ) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0x7FF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x800;
         }
     }

     if (mask.s.l4_port_field_ctrl_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
     }
     if (mask.s.l4_src_port_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
     }
     if (mask.s.l4_dst_port_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_dp  = cfg->l4_dst_port_0 ;
     }

     if (mask.s.vlan_number_vld_0) {
         if (cfg->vlan_number_vld_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &=   0xFFE;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x001;
         }
     }

     if (mask.s.has_ipop_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_option = cfg->has_ipop_0;
     }
     if (mask.s.has_ipop_valid_0) {
         if (cfg->has_ipop_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFEF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x010;
         }
     }
     if (mask.s.dscp_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_tos = cfg->dscp_0;
     }
     if (mask.s.dscp_valid_0) {
         if (cfg->dscp_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFF7;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x008;
         }
     }
     if (mask.s.ip_pt_0) {
         cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ip_pt = ((cfg->ip_pt_0 >>7) &1);
         cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_pt = (cfg->ip_pt_0 &0x7f);
     }
     if (mask.s.ip_pt_valid_0) {
         if (cfg->ip_pt_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFFB;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x004;
         }
     }
     if (mask.s.ipv4_fragment_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_fragment = cfg->ipv4_fragment_0;
     }
     if (mask.s.ipv4_fragment_valid_0) {
         if (cfg->ipv4_fragment_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFDF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x020;
         }
     }
     if (mask.s.l4_flag_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_l4_flags = cfg->l4_flag_0;
     }
     if (mask.s.l4_flag_valid_0) {
         if (cfg->l4_flag_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFBF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x040;
         }
     }
     if (mask.s.special_pkt_code_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_special_pkt_code = cfg->special_pkt_code_0;
     }
     if (mask.s.special_pkt_code_valid_0) {
         if (cfg->special_pkt_code_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xF7F;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x080;
         }
     }
     if (mask.s.vlan_number_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_tag_num = cfg->vlan_number_0;
     }
     if (mask.s.ethernet_type_vld_0) {
         if (cfg->ethernet_type_vld_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFFD;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x002;
         }
     }
     if (mask.s.ethernet_type_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_ev2_pt = cfg->ethernet_type_0;
     }
     if (mask.s.field_is_or_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_fields_operator = cfg->field_is_or_0;
     }
     if (mask.s.precedence_0) {
        cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_1_prcd = cfg->precedence_0;
     }

    if(mask.s.ecn_1){
       cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ecn = cfg->ecn_1;
    }
    if(mask.s.ecn_valid_1){
       cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ecn_mask = (cfg->ecn_valid_1== 0);
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_vid = cfg->inner_vlan_id_1;
            }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.ip_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_ip_ctrl =
            cfg->ip_ctrl_1;
    }
    if (mask.s.ip_da_1) {
       cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ipv4_da =
            (cfg->ip_da_1 >> 12) & 0xfffff;
       cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_da =
            cfg->ip_da_1 & 0xfff;
    }
    if (mask.s.ip_sa_1) {
       cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_sa =
            (cfg->ip_sa_1 >> 12) & 0xfffff;
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_sa =
            cfg->ip_sa_1 & 0xfff;
    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_top_802_1p = (cfg->top_dot1p_1 & 1);
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_802_1p = ((cfg->top_dot1p_1 >> 1) & 3);
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_pt = cfg->ip_pt_1;
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_special_pkt_code = (cfg->special_pkt_code_1 >> 4) & 1;
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_special_pkt_code = (cfg->special_pkt_code_1 & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid_1) {
        if (cfg->special_pkt_code_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x80;
        }
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     return ret;

}

ca_status_t aal_l2_cls_cmrl3r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmrl3r_key_t *cfg)

{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->top_vlan_id_0          =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_top_vid <<3 |cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_vid;
    cfg->top_is_svlan_0         =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_sc_ind;
    cfg->ip_addr_is_sa_0        =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_ipv4_addr_sel;
    cfg->ip_addr_0              =cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_ipv4_addr << 31 |cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ipv4_addr;
    cfg->mac_addr_is_sa_0       =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mac_addr_sel;
    cfg->mac_addr_0[0]          =cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr_0[1]          =cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr_0[2]          = (((cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr_0[3]          = (cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr_0[4]          = (cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr_0[5]          = (cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_0            =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_802_1p;
    cfg->l4_port_field_ctrl_0   =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_ctrl;
    cfg->l4_src_port_0          =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_sp | (cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_sp << 14);
    cfg->l4_dst_port_0          =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_dp;
    cfg->vlan_number_0          =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_tag_num;
    cfg->ethernet_type_0        =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_ev2_pt;
    cfg->field_is_or_0          =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_fields_operator;
    cfg->precedence_0           =cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_1_prcd;
    cfg->ip_pt_0                =cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ip_pt << 7 |cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_pt;
    cfg->dscp_0                 =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_tos;
    cfg->has_ipop_0             =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_option;
    cfg->l4_flag_0              =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_l4_flags;
    cfg->ipv4_fragment_0        =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_fragment;
    cfg->special_pkt_code_0        =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_special_pkt_code;
    cfg->dot1p_valid_0             = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x800) == 0);
    cfg->vlan_field_valid_0        = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x400) == 0);
    cfg->ip_addr_valid_0           = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x200) == 0);
    cfg->mac_addr_valid_0          = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_0  = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x080) == 0);
    cfg->l4_flag_valid_0           = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_0     = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x020) == 0);
    cfg->has_ipop_valid_0          = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x010) == 0);
    cfg->dscp_valid_0              = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x008) == 0);
    cfg->ip_pt_valid_0             = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x002) == 0);
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x001) == 0);
    cfg->ecn_0                     =  cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_ecn;
    cfg->ecn_valid_0               =  (cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_ecn_mask== 0);

    cfg->ecn_1                     =  cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ecn;
    cfg->ecn_valid_1               =  (cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ecn_mask== 0);
    cfg->vlan_field_ctrl_1    =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_vid_ctrl           ;
    cfg->top_vlan_id_1        =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_vid            ;
    cfg->top_is_svlan_1       =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id_1      =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_vid          ;
    cfg->inner_is_svlan_1     =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl_1            =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_ip_ctrl            ;
    cfg->ip_da_1              = (cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_da)          ;
    cfg->ip_sa_1              = (cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_sa)         ;
    cfg->top_dot1p_1          =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_top_802_1p    | (cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p_1        =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl_1         =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_1 =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_ctrl            ;
    cfg->l4_src_port_1        =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_sp | (cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port_1        =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_dp              ;
    cfg->vlan_number_1        =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_tag_num            ;
    cfg->ethernet_type_1      =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_ev2_pt             ;
    cfg->field_is_or_1        =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_fields_operator    ;
    cfg->precedence_1         =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_prcd               ;
    cfg->ip_pt_1              =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_pt              ;
    cfg->special_pkt_code_1   = (cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_special_pkt_code    ;
    cfg->dscp_1               =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_tos             ;
    cfg->has_ipop_1           =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_option        ;
    cfg->l4_flag_1            =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_l4_flags           ;
    cfg->ipv4_fragment_1      =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_fragment      ;
    cfg->ipv4_fragment_valid_1    = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_1          = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_1        = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_1            = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_1         = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid_1             = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_1      = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_1 = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x80) == 0x00);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_cmrcmr_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmrcmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmrcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
     return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;

    if(mask.s.ecn_0){
       cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn = cfg->ecn_0;
    }
    if(mask.s.ecn_valid_0){
       cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn_mask = (cfg->ecn_valid_0== 0);
    }

    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid = (cfg->top_vlan_id_0 >> 3) & 0x1ff;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid = cfg->top_vlan_id_0 & 0x07;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.vlan_field_valid_0) {
     if (cfg->vlan_field_valid_0) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xBFF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x400;
     }
    }

    if (mask.s.ip_addr_is_sa_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel =
            cfg->ip_addr_is_sa_0;
    }
    if (mask.s.ip_addr_0) {
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            (cfg->ip_addr_0 >> 31) & 0x01;
       cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            cfg->ip_addr_0 & 0x7fffffff;
    }
    if (mask.s.ip_addr_valid_0) {
        if (cfg->ip_addr_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xDFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel =
            cfg->mac_addr_is_sa_0;
    }

    if (mask.s.mac_addr_0) {
       cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            (cfg->mac_addr_0[0] << 9) | (cfg->mac_addr_0[1] << 1) | ((cfg->mac_addr_0[2] >> 7) & 0x01);
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            ((cfg->mac_addr_0[2] & 0x7f) << 24) | (cfg->mac_addr_0[3] << 16) | (cfg->mac_addr_0[4] << 8) | (cfg->mac_addr_0[5]);
    }

    if (mask.s.mac_addr_valid_0) {
        if (cfg->mac_addr_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xEFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x100;
        }
    }

    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.dot1p_valid_0) {
        if (cfg->dot1p_valid_0 ) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0x7FF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x800;
        }
    }

    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &=   0xFFE;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x001;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
     if (cfg->has_ipop_valid_0) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFEF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x010;
     }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFF7;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x008;
        }
    }
    if (mask.s.ip_pt_0) {
        cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt_0 >> 7) &0x1;
        cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt_0&0x7f);
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFB;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x004;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFDF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x020;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFBF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x040;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xF7F;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x080;
     }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFD;
        } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x002;
     }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd = cfg->precedence_0;
    }

    if(mask.s.ecn_1){
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn = cfg->ecn_1;
    }
    if(mask.s.ecn_valid_1){
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn_mask = (cfg->ecn_valid_1== 0);
    }

    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.vlan_field_valid_1) {
        if (cfg->vlan_field_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa_1;
    }
    if (mask.s.ip_addr_1) {
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            (cfg->ip_addr_1 >> 28) & 0xf;
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            cfg->ip_addr_1 & 0xfffffff;
    }
    if (mask.s.ip_addr_valid_1) {
        if (cfg->ip_addr_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa_1;
    }

    if (mask.s.mac_addr_1) {
       cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            (cfg->mac_addr_1[0] << 12) | (cfg->mac_addr_1[1] << 4) | ((cfg->mac_addr_1[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            ((cfg->mac_addr_1[2] & 0xf) << 24) | (cfg->mac_addr_1[3] << 16) | (cfg->mac_addr_1[4] << 8) | (cfg->mac_addr_1[5]);
    }

    if (mask.s.mac_addr_valid_1) {
        if (cfg->mac_addr_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.dot1p_valid_1) {
        if (cfg->dot1p_valid_1)
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt = ((cfg->ip_pt_1 >> 4) & 0x0f);
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt = (cfg->ip_pt_1 & 0x0f);
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code = cfg->special_pkt_code_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x080;
        }
    }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}



ca_status_t aal_l2_cls_cmrcmr_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmrcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->top_vlan_id_0          =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid <<3 |cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid;
    cfg->top_is_svlan_0         =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind;
    cfg->ip_addr_is_sa_0        =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel;
    cfg->ip_addr_0              =cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr << 31 |cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr;
    cfg->mac_addr_is_sa_0       =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel;
    cfg->mac_addr_0[0]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr_0[1]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr_0[2]          = (((cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr_0[3]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr_0[4]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr_0[5]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_0            =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p;
    cfg->l4_port_field_ctrl_0   =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl;
    cfg->l4_src_port_0          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp | (cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp << 14);
    cfg->l4_dst_port_0          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp;
    cfg->vlan_number_0          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num;
    cfg->ethernet_type_0        =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt;
    cfg->field_is_or_0          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator;
    cfg->precedence_0           =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd;
    cfg->ip_pt_0                =cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt << 7 |cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt;
    cfg->dscp_0                 =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos;
    cfg->has_ipop_0             =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option;
    cfg->l4_flag_0              =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags;
    cfg->ipv4_fragment_0        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment;
    cfg->special_pkt_code_0        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code;
    cfg->dot1p_valid_0             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x800) == 0);
    cfg->vlan_field_valid_0        = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x400) == 0);
    cfg->ip_addr_valid_0           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x200) == 0);
    cfg->mac_addr_valid_0          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_0  = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x080) == 0);
    cfg->l4_flag_valid_0           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_0     = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x020) == 0);
    cfg->has_ipop_valid_0          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x010) == 0);
    cfg->dscp_valid_0              = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x008) == 0);
    cfg->ip_pt_valid_0             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x002) == 0);
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x001) == 0);
    cfg->ecn_0                     =  cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn;
    cfg->ecn_valid_0               =  (cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn_mask== 0);

    cfg->ecn_1                     =  cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn;
    cfg->ecn_valid_1               =  (cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn_mask== 0);
    cfg->top_vlan_id_1          =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid;
    cfg->top_is_svlan_1         =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind;
    cfg->ip_addr_is_sa_1        =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel;
    cfg->ip_addr_1              =cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr;
    cfg->mac_addr_is_sa_1       =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel;
    cfg->mac_addr_1[0]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 12 & 0xff;
    cfg->mac_addr_1[1]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 4 & 0xff;
    cfg->mac_addr_1[2]          = (((cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 24) & 0xf));
    cfg->mac_addr_1[3]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 16 & 0xff);
    cfg->mac_addr_1[4]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 8 & 0xff);
    cfg->mac_addr_1[5]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_1            =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p;
    cfg->l4_port_field_ctrl_1   =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl;
    cfg->l4_src_port_1          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp | (cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp << 11);
    cfg->l4_dst_port_1          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp;
    cfg->vlan_number_1          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num;
    cfg->ethernet_type_1        =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt;
    cfg->field_is_or_1          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator;
    cfg->precedence_1           =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd;
    cfg->ip_pt_1                =cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt;
    cfg->dscp_1                 =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos;
    cfg->has_ipop_1             =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option;
    cfg->l4_flag_1              =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags;
    cfg->ipv4_fragment_1        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment;
    cfg->special_pkt_code_1        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code;
    cfg->dot1p_valid_1             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid_1        = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid_1           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid_1          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_1  = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid_1           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_1     = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid_1          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid_1              = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid_1             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld_1       = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld_1         = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x001) == 0);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_ingress_key_set(
               CA_IN ca_device_id_t device_id,
               CA_IN ca_uint32_t    entry_id,
               CA_IN aal_l2_cls_rule_type_t rule_type,
			   CA_IN aal_l2_cls_key_mask_t key_mask,
               CA_IN aal_l2_cls_key_t       *key_cfg
            )
{

    switch(rule_type){
        case   AAL_L2_CLS_RULE_TYPE_IPV4:
            key_mask.ipv4_key.s.is_ipv4_ipv6_key = 1;
            key_cfg->ipv4_key.is_ipv4_ipv6_key = FALSE;
            return aal_l2_cls_ipv4_key_set(device_id,entry_id, key_mask.ipv4_key, &key_cfg->ipv4_key);
        case   AAL_L2_CLS_RULE_TYPE_IPV6_SHORT:
            return aal_l2_cls_ipv6_short_key_set(device_id,entry_id, key_mask.ipv6_short_key, &key_cfg->ipv6_short_key);
        case   AAL_L2_CLS_RULE_TYPE_IPV6_LONG:

            key_mask.ipv6_long_key.entry_0.s.is_ipv4_ipv6_key = 1;
            key_cfg->ipv6_long_key.entry_0.is_ipv4_ipv6_key = TRUE;
            aal_l2_cls_ipv4_key_set(device_id,entry_id, key_mask.ipv6_long_key.entry_0, &key_cfg->ipv6_long_key.entry_0);

            return aal_l2_cls_ipv6_short_key_set(device_id,entry_id, key_mask.ipv6_long_key.entry_1, &key_cfg->ipv6_long_key.entry_1);
        case   AAL_L2_CLS_RULE_TYPE_L2RL2R:
            return aal_l2_cls_l2rl2r_key_set(device_id,entry_id, key_mask.l2rl2r_key, &key_cfg->l2rl2r_key);
        case   AAL_L2_CLS_RULE_TYPE_L2RL3R:
            return aal_l2_cls_l2rl3r_key_set(device_id,entry_id, key_mask.l2rl3r_key, &key_cfg->l2rl3r_key);
        case   AAL_L2_CLS_RULE_TYPE_L2RCMR:
            return aal_l2_cls_l2rcmr_key_set(device_id,entry_id, key_mask.l2rcmr_key, &key_cfg->l2rcmr_key);
        case   AAL_L2_CLS_RULE_TYPE_L3RL2R:
            return aal_l2_cls_l3rl2r_key_set(device_id,entry_id, key_mask.l3rl2r_key, &key_cfg->l3rl2r_key);
        case   AAL_L2_CLS_RULE_TYPE_L3RL3R:
            return aal_l2_cls_l3rl3r_key_set(device_id,entry_id, key_mask.l3rl3r_key, &key_cfg->l3rl3r_key);
        case   AAL_L2_CLS_RULE_TYPE_L3RCMR:
            return aal_l2_cls_l3rcmr_key_set(device_id,entry_id, key_mask.l3rcmr_key, &key_cfg->l3rcmr_key);
        case   AAL_L2_CLS_RULE_TYPE_CMRL2R:
            return aal_l2_cls_cmrl2r_key_set(device_id,entry_id, key_mask.cmrl2r_key, &key_cfg->cmrl2r_key);
        case   AAL_L2_CLS_RULE_TYPE_CMRL3R:
            return aal_l2_cls_cmrl3r_key_set(device_id,entry_id, key_mask.cmrl3r_key, &key_cfg->cmrl3r_key);
        case   AAL_L2_CLS_RULE_TYPE_CMRCMR:
            return aal_l2_cls_cmrcmr_key_set(device_id,entry_id, key_mask.cmrcmr_key, &key_cfg->cmrcmr_key);

        default:
                return CA_E_PARAM;
    }
    return CA_E_OK;
}

ca_status_t  aal_l2_cls_ingress_key_get(
               CA_IN ca_device_id_t device_id,
               CA_IN ca_uint32_t    entry_id,
               CA_IN aal_l2_cls_rule_type_t rule_type,
               CA_OUT aal_l2_cls_key_t       *key_cfg
)
{
    return CA_E_OK;
}

ca_status_t
aal_l2_cls_fib_set(CA_IN ca_device_id_t device_id,
                   CA_IN ca_uint32_t fib_id,
                   CA_IN aal_l2_cls_fib_mask_t mask,
                   CA_IN aal_l2_cls_fib_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    L2FE_CLE_IGR_FIB_DATA0_t fib_data0;
    L2FE_CLE_IGR_FIB_DATA1_t fib_data1;
    L2FE_CLE_IGR_FIB_DATA2_t fib_data2;
    L2FE_CLE_IGR_FIB_DATA3_t fib_data3;
    L2FE_CLE_IGR_FIB_DATA4_t fib_data4;
    fib_data0.wrd = 0;
    fib_data1.wrd = 0;
    fib_data2.wrd = 0;
    fib_data3.wrd = 0;
    fib_data4.wrd = 0;

    if ((fib_id > __AAL_L2_CLS_MAX_FIB_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_IGR_FIB_ACCESS);

        fib_data4.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA4));
        fib_data3.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA3));
        fib_data2.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA2));
        fib_data1.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA1));
        fib_data0.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA0));
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_IGR_FIB_ACCESS);

        fib_data4.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA4);
        fib_data3.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA3);
        fib_data2.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA2);
        fib_data1.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA1);
        fib_data0.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA0);
#endif
    }

    if (mask.s.permit_0) {
        fib_data2.bf.cl_fib_xr_permit_0 = cfg->permit_0;
    }
    if (mask.s.permit_valid_0) {
        fib_data2.bf.cl_fib_xr_permit_valid_0 = cfg->permit_valid_0;
    }
    if (mask.s.top_vlan_cmd_0) {
        fib_data4.bf.cl_fib_xr_top_vlan_cmd_0 = cfg->top_vlan_cmd_0;
    }
    if (mask.s.top_cmd_valid_0) {
        fib_data4.bf.cl_fib_xr_top_cmd_valid_0 = cfg->top_cmd_valid_0;
    }
    if (mask.s.top_vlanid_0) {
        fib_data4.bf.cl_fib_xr_top_vid_0 = cfg->top_vlanid_0;
    }
    if (mask.s.top_sc_ind_0) {
        fib_data4.bf.cl_fib_xr_top_sc_ind_0 = cfg->top_sc_ind_0;
    }

    if (mask.s.inner_vlan_cmd_0) {
        fib_data3.bf.cl_fib_xr_inner_vlan_cmd_0 = cfg->inner_vlan_cmd_0;
    }
    if (mask.s.inner_cmd_valid_0) {
        fib_data3.bf.cl_fib_xr_inner_cmd_valid_0 = cfg->inner_cmd_valid_0;
    }
    if (mask.s.inner_vlanid_0) {
        fib_data4.bf.cl_fib_xr_inner_vid_0 = (cfg->inner_vlanid_0 >> 9) & 0x07;
        fib_data3.bf.cl_fib_xr_inner_vid_0 = cfg->inner_vlanid_0 & 0x1ff;
    }
    if (mask.s.inner_sc_ind_0) {
        fib_data3.bf.cl_fib_xr_inner_sc_ind_0 = cfg->inner_sc_ind_0;
    }

    if (mask.s.dot1p_valid_0) {
        fib_data3.bf.cl_fib_xr_802_1p_valid_0 = cfg->dot1p_valid_0;
    }
    if (mask.s.dot1p_0) {
        fib_data3.bf.cl_fib_xr_802_1p_0 = cfg->dot1p_0;
    }

    if (mask.s.dscp_valid_0) {
        fib_data3.bf.cl_fib_xr_dscp_valid_0 = cfg->dscp_valid_0;
    }

    if (mask.s.dscp_0) {
        fib_data3.bf.cl_fib_xr_dscp_0 = cfg->dscp_0;
    }

    if (mask.s.cos_valid_0) {
        fib_data3.bf.cl_fib_xr_cos_valid_0 = cfg->cos_valid_0;
    }
    if (mask.s.cos_0) {
        fib_data3.bf.cl_fib_xr_cos_0 = cfg->cos_0;
    }

    if (mask.s.mark_ena_0) {
        fib_data2.bf.cl_fib_xr_mark_en_0 = cfg->mark_ena_0;
    }
    if (mask.s.flowid_0) {
        fib_data2.bf.cl_fib_xr_flowid_0 = cfg->flowid_0;
    }

    if (mask.s.dp_valid_0) {
        fib_data2.bf.cl_fib_xr_dp_valid_0 = cfg->dp_valid_0;
    }

    if (mask.s.dp_value_0) {
        fib_data2.bf.cl_fib_xr_ldpid_0 = cfg->dp_value_0 & 0x03;
        fib_data3.bf.cl_fib_xr_ldpid_0 = (cfg->dp_value_0 >> 2) & 0x0f;
    }

    if (mask.s.premarked_0) {
        fib_data2.bf.cl_fib_xr_premarked_0 = cfg->premarked_0;
    }
    if (mask.s.dscp_mark_down_0) {
        fib_data2.bf.cl_fib_xr_dscp_markdown_0 = cfg->dscp_mark_down_0;
    }

    if (mask.s.spt_mode_0) {
        fib_data2.bf.cl_fib_xr_stp_mode_0 = cfg->spt_mode_0;
    }

    if (mask.s.no_drop_0) {
        fib_data4.bf.cl_fib_xr_pkt_no_drop_0 = cfg->no_drop_0;
    }

    if (mask.s.permit_1) {
        fib_data0.bf.cl_fib_xr_permit_1 = cfg->permit_1;
    }
    if (mask.s.permit_valid_1) {
        fib_data0.bf.cl_fib_xr_permit_valid_1 = cfg->permit_valid_1;
    }

    if (mask.s.top_vlan_cmd_1) {
        fib_data1.bf.cl_fib_xr_top_vlan_cmd_1 = cfg->top_vlan_cmd_1;
    }
    if (mask.s.top_cmd_valid_1) {
        fib_data1.bf.cl_fib_xr_top_cmd_valid_1 = cfg->top_cmd_valid_1;
    }
    if (mask.s.top_vlanid_1) {
        fib_data1.bf.cl_fib_xr_top_vid_1 = cfg->top_vlanid_1 & 7;
        fib_data2.bf.cl_fib_xr_top_vid_1 = (cfg->top_vlanid_1 >> 3) & 0x1ff;
    }
    if (mask.s.top_sc_ind_1) {
        fib_data1.bf.cl_fib_xr_top_sc_ind_1 = cfg->top_sc_ind_1;
    }

    if (mask.s.inner_vlan_cmd_1) {
        fib_data1.bf.cl_fib_xr_inner_vlan_cmd_1 = cfg->inner_vlan_cmd_1;
    }
    if (mask.s.inner_cmd_valid_1) {
        fib_data1.bf.cl_fib_xr_inner_cmd_valid_1 =
            cfg->inner_cmd_valid_1;
    }
    if (mask.s.inner_vlanid_1) {
        fib_data1.bf.cl_fib_xr_inner_vid_1 = cfg->inner_vlanid_1;
    }
    if (mask.s.inner_sc_ind_1) {
        fib_data1.bf.cl_fib_xr_inner_sc_ind_1 = cfg->inner_sc_ind_1;
    }

    if (mask.s.dot1p_valid_1) {
        fib_data0.bf.cl_fib_xr_802_1p_valid_1 = cfg->dot1p_valid_1;
    }
    if (mask.s.dot1p_1) {
        fib_data1.bf.cl_fib_xr_802_1p_1 = cfg->dot1p_1;
    }

    if (mask.s.dscp_valid_1) {
        fib_data0.bf.cl_fib_xr_dscp_valid_1 = cfg->dscp_valid_1;
    }
    if (mask.s.dscp_1) {
        fib_data1.bf.cl_fib_xr_dscp_1 = cfg->dscp_1;
    }

    if (mask.s.cos_valid_1) {
        fib_data0.bf.cl_fib_xr_cos_valid_1 = cfg->cos_valid_1;
    }
    if (mask.s.cos_1) {
        fib_data0.bf.cl_fib_xr_cos_1 = cfg->cos_1;
    }
    if (mask.s.mark_ena_1) {
        fib_data0.bf.cl_fib_xr_mark_en_1 = cfg->mark_ena_1;
    }
    if (mask.s.flowid_1) {
        fib_data0.bf.cl_fib_xr_flowid_1 = cfg->flowid_1;
    }
    if (mask.s.dp_valid_1) {
        fib_data0.bf.cl_fib_xr_dp_valid_1 = cfg->dp_valid_1;
    }
    if (mask.s.dp_value_1) {
        fib_data0.bf.cl_fib_xr_ldpid_1 = cfg->dp_value_1;
    }

    if (mask.s.premarked_1) {
        fib_data0.bf.cl_fib_xr_premarked_1 = cfg->premarked_1;
    }

    if (mask.s.dscp_mark_down_1) {
        fib_data0.bf.cl_fib_xr_dscp_markdown_1 = cfg->dscp_mark_down_1;
    }

    if (mask.s.spt_mode_1) {
        fib_data0.bf.cl_fib_xr_stp_mode_1       = cfg->spt_mode_1;
    }
    if (mask.s.no_drop_1) {
        fib_data2.bf.cl_fib_xr_pkt_no_drop_1 = cfg->no_drop_1;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE(fib_data4.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA4));
        CA_NE_REG_WRITE(fib_data3.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA3));
        CA_NE_REG_WRITE(fib_data2.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA2));
        CA_NE_REG_WRITE(fib_data1.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA1));
        CA_NE_REG_WRITE(fib_data0.wrd, AAL_L2FE_CLE_REG_ADDR(IGR_FIB_DATA0));
        WRITE_INDIRCT_REG(device_id, fib_id, L2FE_CLE_IGR_FIB_ACCESS);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(fib_data4.wrd, L2FE_CLE_IGR_FIB_DATA4);
        CA_8279_NE_REG_WRITE(fib_data3.wrd, L2FE_CLE_IGR_FIB_DATA3);
        CA_8279_NE_REG_WRITE(fib_data2.wrd, L2FE_CLE_IGR_FIB_DATA2);
        CA_8279_NE_REG_WRITE(fib_data1.wrd, L2FE_CLE_IGR_FIB_DATA1);
        CA_8279_NE_REG_WRITE(fib_data0.wrd, L2FE_CLE_IGR_FIB_DATA0);
        CA_8279_WRITE_INDIRCT_REG(device_id, fib_id, L2FE_CLE_IGR_FIB_ACCESS);
#endif
    }
#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;
}

ca_status_t
aal_l2_cls_fib_get(CA_IN ca_device_id_t device_id,
                   CA_IN ca_uint32_t fib_id, CA_OUT aal_l2_cls_fib_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    L2FE_CLE_IGR_FIB_DATA0_t fib_data0;
    L2FE_CLE_IGR_FIB_DATA1_t fib_data1;
    L2FE_CLE_IGR_FIB_DATA2_t fib_data2;
    L2FE_CLE_IGR_FIB_DATA3_t fib_data3;
    L2FE_CLE_IGR_FIB_DATA4_t fib_data4;

    if ((fib_id > __AAL_L2_CLS_MAX_FIB_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_IGR_FIB_ACCESS);

        fib_data0.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA0);
        fib_data1.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA1);
        fib_data2.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA2);
        fib_data3.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA3);
        fib_data4.wrd = CA_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA4);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_IGR_FIB_ACCESS);

        fib_data0.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA0);
        fib_data1.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA1);
        fib_data2.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA2);
        fib_data3.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA3);
        fib_data4.wrd = CA_8279_NE_REG_READ(L2FE_CLE_IGR_FIB_DATA4);

#endif
    }
    cfg->permit_0           = fib_data2.bf.cl_fib_xr_permit_0;
    cfg->permit_valid_0     = fib_data2.bf.cl_fib_xr_permit_valid_0;
    cfg->premarked_0        = fib_data2.bf.cl_fib_xr_premarked_0;
    cfg->top_vlan_cmd_0     = fib_data4.bf.cl_fib_xr_top_vlan_cmd_0;
    cfg->top_cmd_valid_0    = fib_data4.bf.cl_fib_xr_top_cmd_valid_0;
    cfg->top_vlanid_0       = fib_data4.bf.cl_fib_xr_top_vid_0;
    cfg->top_sc_ind_0       = fib_data4.bf.cl_fib_xr_top_sc_ind_0;
    cfg->inner_vlan_cmd_0   = fib_data3.bf.cl_fib_xr_inner_vlan_cmd_0;
    cfg->inner_cmd_valid_0  = fib_data3.bf.cl_fib_xr_inner_cmd_valid_0;
    cfg->inner_vlanid_0     = fib_data3.bf.cl_fib_xr_inner_vid_0 | (fib_data4.bf.cl_fib_xr_inner_vid_0 << 9);
    cfg->inner_sc_ind_0     = fib_data3.bf.cl_fib_xr_inner_sc_ind_0 ;
    cfg->dot1p_valid_0      = fib_data3.bf.cl_fib_xr_802_1p_valid_0;
    cfg->dot1p_0            = fib_data3.bf.cl_fib_xr_802_1p_0;
    cfg->dscp_valid_0       = fib_data3.bf.cl_fib_xr_dscp_valid_0;
    cfg->dscp_0             = fib_data3.bf.cl_fib_xr_dscp_0;
    cfg->cos_valid_0        = fib_data3.bf.cl_fib_xr_cos_valid_0;
    cfg->cos_0              = fib_data3.bf.cl_fib_xr_cos_0;
    cfg->mark_ena_0         = fib_data2.bf.cl_fib_xr_mark_en_0;
    cfg->flowid_0           = fib_data2.bf.cl_fib_xr_flowid_0;
    cfg->dp_valid_0         = fib_data2.bf.cl_fib_xr_dp_valid_0;
    cfg->dp_value_0         = fib_data2.bf.cl_fib_xr_ldpid_0 | (fib_data3.bf.cl_fib_xr_ldpid_0 << 2);
    cfg->dscp_mark_down_0   = fib_data2.bf.cl_fib_xr_dscp_markdown_0;
    cfg->spt_mode_0         = fib_data2.bf.cl_fib_xr_stp_mode_0;
    cfg->no_drop_0          = fib_data4.bf.cl_fib_xr_pkt_no_drop_0;

    cfg->permit_1           = fib_data0.bf.cl_fib_xr_permit_1;
    cfg->permit_valid_1     = fib_data0.bf.cl_fib_xr_permit_valid_1;
    cfg->premarked_1        = fib_data0.bf.cl_fib_xr_premarked_1;
    cfg->top_vlan_cmd_1     = fib_data1.bf.cl_fib_xr_top_vlan_cmd_1;
    cfg->top_cmd_valid_1    = fib_data1.bf.cl_fib_xr_top_cmd_valid_1;
    cfg->top_vlanid_1       = fib_data1.bf.cl_fib_xr_top_vid_1 | (fib_data2.bf.cl_fib_xr_top_vid_1 << 3);
    cfg->top_sc_ind_1       = fib_data1.bf.cl_fib_xr_top_sc_ind_1;
    cfg->inner_vlan_cmd_1   = fib_data1.bf.cl_fib_xr_inner_vlan_cmd_1;
    cfg->inner_cmd_valid_1  = fib_data1.bf.cl_fib_xr_inner_cmd_valid_1;
    cfg->inner_vlanid_1     = fib_data1.bf.cl_fib_xr_inner_vid_1;
    cfg->inner_sc_ind_1     = fib_data1.bf.cl_fib_xr_inner_sc_ind_1;
    cfg->dot1p_valid_1      = fib_data0.bf.cl_fib_xr_802_1p_valid_1;
    cfg->dot1p_1            = fib_data1.bf.cl_fib_xr_802_1p_1;
    cfg->dscp_valid_1       = fib_data0.bf.cl_fib_xr_dscp_valid_1;
    cfg->dscp_1             = fib_data1.bf.cl_fib_xr_dscp_1;
    cfg->cos_valid_1        = fib_data0.bf.cl_fib_xr_cos_valid_1;
    cfg->cos_1              = fib_data0.bf.cl_fib_xr_cos_1;
    cfg->mark_ena_1         = fib_data0.bf.cl_fib_xr_mark_en_1;
    cfg->flowid_1           = fib_data0.bf.cl_fib_xr_flowid_1;
    cfg->dp_valid_1         = fib_data0.bf.cl_fib_xr_dp_valid_1;
    cfg->dp_value_1         = fib_data0.bf.cl_fib_xr_ldpid_1;
    cfg->dscp_mark_down_1   = fib_data0.bf.cl_fib_xr_dscp_markdown_1;
    cfg->spt_mode_1         = fib_data0.bf.cl_fib_xr_stp_mode_1;
    cfg->no_drop_1          = fib_data2.bf.cl_fib_xr_pkt_no_drop_1;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_egr_ipv4_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv4_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv4_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    if (mask.s.valid) {
      cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   if(mask.s.is_ipv4_ipv6_key){
        if(cfg->is_ipv4_ipv6_key){
            cls_key_data.key_data12.bf.cl_key_entry_type = 1;
            cls_key_data.key_data11.bf0.cl_key_entry_type = 1;
            }else{
                cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY;
                cls_key_data.key_data12.bf.cl_key_entry_type = 0;
            }
   }
   cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid = FALSE;

    if(mask.s.ipv4_ecn){
        cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn = cfg->ipv4_ecn;
    }
    if(mask.s.ipv4_ecn_vld){
        cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn_mask = (cfg->ipv4_ecn_vld == 0);
    }

    if (mask.s.mac_da) {
      cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_mac_da =
           cfg->mac_da[0];
       cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da =
           cfg->mac_da[1]<<24 | cfg->mac_da[2]<<16 | cfg->mac_da[3]<< 8 | cfg->mac_da[4]  ;
       cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_da =
           cfg->mac_da[5];
    }

    if (mask.s.mac_sa) {
      cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa =
        cfg->mac_sa[0] <<16 | cfg->mac_sa[1] << 8 | cfg->mac_sa[2];

      cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa =
        cfg->mac_sa[3] <<16 | cfg->mac_sa[4] << 8 | cfg->mac_sa[5];
    }

    if (mask.s.mac_field_ctrl) {
      cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_mac_ctrl = cfg->mac_field_ctrl;
    }

    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_top_vid = (cfg->top_vlan_id >> 4) &0xff;
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_vid = (cfg->top_vlan_id  & 0x0f);
    }

    if (mask.s.top_is_svlan) {
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_sc_ind = cfg->top_is_svlan;
    }

    if (mask.s.inner_vlan_id) {
     cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_vid = cfg->inner_vlan_id;
    }

    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_sc_ind = cfg->inner_is_svlan;
    }
    if (mask.s.vlan_field_ctrl) {
        cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_vid_ctrl =
           cfg->vlan_field_ctrl;
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_802_1p = cfg->top_dot1p;
    }
    if (mask.s.inner_dot1p) {
      cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_802_1p = cfg->inner_dot1p;
    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_dot1p_ctrl =  cfg->dot1p_ctrl;
    }

    if(mask.s.vlan_number){
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_tag_num = cfg->vlan_number;

    }

    if (mask.s.vlan_number_vld) {
       if (cfg->vlan_number_vld) {
          cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x2FFFFE;
       } else {
          cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x000001;
       }
    }
    if(mask.s.ethernet_type){
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ev2_pt = cfg->ethernet_type;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FFFFD;
        } else {
           cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x000002;
        }
    }
    if(mask.s.ip_da){
       cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ipv4_da = (cfg->ip_da >> 31) & 1;
       cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_da = cfg->ip_da & 0x7FFFFFFF;
    }
    if(mask.s.ip_sa){
       cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_sa = (cfg->ip_sa >> 31) & 1;
       cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ipv4_sa = cfg->ip_sa & 0x7FFFFFFF;
    }
    if (mask.s.ip_da_mask) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &= 0x3FFF83;
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= (cfg->ip_da_mask << 2);
    }
    if (mask.s.ip_sa_mask) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &= 0x3FF07F;
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= (cfg->ip_sa_mask << 7) ;
    }
    if (mask.s.ip_ctrl) {
        cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_ip_ctrl =  cfg->ip_ctrl;
     }

    if (mask.s.ip_pt) {
       cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ip_pt =  (cfg->ip_pt >> 7) & 1;
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_pt =  (cfg->ip_pt  & 0x7f);
     }

    if (mask.s.ip_pt_valid) {
         if (cfg->ip_pt_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FEFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0001000;
         }
     }

    if (mask.s.dscp) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_tos =  cfg->dscp;
     }


    if (mask.s.dscp_valid) {
         if (cfg->dscp_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FDFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0002000;
         }
     }

    if (mask.s.ip_option) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_option =  cfg->ip_option;
     }


    if (mask.s.ip_option_valid) {
         if (cfg->ip_option_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FBFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0004000;
         }
     }

    if (mask.s.ip_fragment) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_fragment =  cfg->ip_fragment;
     }

    if (mask.s.ip_fragment_valid) {
         if (cfg->ip_fragment_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3F7FFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0008000;
         }
     }

    if (mask.s.l4_dst_port_hi) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_high =  cfg->l4_dst_port_hi;
     }
    if (mask.s.l4_dst_port_lo) {
       cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_low =  (cfg->l4_dst_port_lo >> 15) & 1;
       cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_dp_low =  (cfg->l4_dst_port_lo  & 0x7FFF);
     }
    if (mask.s.l4_dst_ctrl) {
        cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_dp_ctrl =  cfg->l4_dst_ctrl;
     }
    if (mask.s.l4_src_port_hi) {
       cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_high =  cfg->l4_src_port_hi;
     }
    if (mask.s.l4_dst_port_lo) {
       cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_low =  (cfg->l4_src_port_lo >> 15) & 1;
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_sp_low =  (cfg->l4_src_port_lo  & 0x7FFF);
     }
    if (mask.s.l4_src_ctrl) {
        cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_sp_ctrl =  cfg->l4_src_ctrl;
     }

    if (mask.s.l4_flag) {
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_flags =  cfg->l4_flag;
     }

    if (mask.s.l4_flag_valid) {
         if (cfg->l4_flag_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3EFFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x0010000;
         }
     }

    if (mask.s.special_pkt_code) {
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_special_pkt_code =  cfg->special_pkt_code;
     }

    if (mask.s.special_pkt_code_valid) {
         if (cfg->special_pkt_code_valid) {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3DFFFF;
         } else {
            cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |= 0x020000;
         }
    }
    if (mask.s.global_key) {
       cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_global_key =  (cfg->global_key>>23)&0x1FF;
       cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_global_key =  cfg->global_key & 0x7FFFFF;
    }

    if (mask.s.global_key_mask) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask &=   0x3FFFF;
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask |=  (cfg->global_key_mask << 18);
    }
    if (mask.s.l7_ctrl) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l7_ctrl =  cfg->l7_ctrl;
    }
    if (mask.s.fields_is_or) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_fields_operator =  cfg->fields_is_or;
    }

    if (mask.s.precedence) {
       cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_prcd =  cfg->precedence;
    }
    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_egr_ipv4_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv4_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
    return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    if((cls_key_data.key_data12.bf.cl_key_entry_type == 1) && (cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        cfg->is_ipv4_ipv6_key = 1;
    }else{
        cfg->is_ipv4_ipv6_key = 0;
    }

    cfg->ipv4_ecn = cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn;
    cfg->ipv4_ecn_vld = (cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_ipv4_ecn_mask == 0);
    cfg->mac_da[0]   =  cls_key_data.key_data11.bf0.cl_key_orhr_ipv4e_mac_da;
    cfg->mac_da[1] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 24 & 0xff;
    cfg->mac_da[2] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 16 & 0xff;
    cfg->mac_da[3] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 8 & 0xff;
    cfg->mac_da[4] =cls_key_data.key_data10.bf0.cl_key_orhr_ipv4e_mac_da >> 0 & 0xff;
    cfg->mac_da[5] = cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_da ;
    cfg->mac_sa[0]  =cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa >>16 & 0xff;
    cfg->mac_sa[1]  =cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa >>8 & 0xff;
    cfg->mac_sa[2]  =cls_key_data.key_data9.bf0.cl_key_orhr_ipv4e_mac_sa >>0 & 0xff;
    cfg->mac_sa[3]  =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa >>16 & 0xff;
    cfg->mac_sa[4]  =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa >>8 & 0xff;
    cfg->mac_sa[5]  =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_mac_sa >>0 & 0xff;
    cfg->mac_field_ctrl =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_mac_ctrl;
    cfg->top_vlan_id =cls_key_data.key_data8.bf0.cl_key_orhr_ipv4e_top_vid<<4 |cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_vid;
    cfg->top_is_svlan =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_sc_ind;
    cfg->inner_vlan_id =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_vid;
    cfg->inner_is_svlan  =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_sc_ind;
    cfg->vlan_field_ctrl =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_vid_ctrl;
    cfg->top_dot1p = cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_top_802_1p;
    cfg->inner_dot1p =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_inner_802_1p;
    cfg->dot1p_ctrl  =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_dot1p_ctrl ;
    cfg->vlan_number  =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_tag_num;
    cfg->vlan_number_vld =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x000001) == 0);
    cfg->ethernet_type  =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ev2_pt;
    cfg->ethernet_type_vld =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x000002) == 0);
    cfg->ip_da =cls_key_data.key_data7.bf0.cl_key_orhr_ipv4e_ipv4_da << 31 |cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_da;
    cfg->ip_sa  =cls_key_data.key_data6.bf0.cl_key_orhr_ipv4e_ipv4_sa <<31 |cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ipv4_sa;
    cfg->ip_da_mask = ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask) >> 2) & 0x1F;
    cfg->ip_sa_mask = (cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask >> 7) & 0x1F;
    cfg->ip_ctrl  =cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_ip_ctrl ;
    cfg->ip_pt  = cls_key_data.key_data5.bf0.cl_key_orhr_ipv4e_ip_pt<<7 |cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_pt;
    cfg->ip_pt_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0001000) == 0);
    cfg->dscp =  cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ip_tos;
    cfg->dscp_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0002000) == 0);
    cfg->ip_option  =  cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_option ;
    cfg->ip_option_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0004000) == 0);
    cfg->ip_fragment   = cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_ipv4_fragment ;
    cfg->ip_fragment_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0008000) == 0);
    cfg->l4_dst_port_hi  =  cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_high;
    cfg->l4_dst_port_lo =cls_key_data.key_data4.bf0.cl_key_orhr_ipv4e_l4_dp_low <<15 | cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_dp_low;
    cfg->l4_dst_ctrl   = cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_dp_ctrl;
    cfg->l4_src_port_hi    = cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_high;
    cfg->l4_src_port_lo  = cls_key_data.key_data3.bf0.cl_key_orhr_ipv4e_l4_sp_low << 15 |cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_sp_low;
    cfg->l4_src_ctrl   =cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l4_sp_ctrl ;
    cfg->l4_flag  =  cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_l4_flags;
    cfg->l4_flag_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0010000) == 0);
    cfg->special_pkt_code = cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_special_pkt_code ;
    cfg->special_pkt_code_valid =
        ((cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask & 0x0020000) == 0);
    cfg->global_key  =  cls_key_data.key_data2.bf0.cl_key_orhr_ipv4e_global_key<<23 | cls_key_data.key_data1.bf0.cl_key_orhr_ipv4e_global_key;
    cfg->global_key_mask =(cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_mask >> 18) & 0x0f;
    cfg->l7_ctrl=     cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_l7_ctrl  ;
    cfg->fields_is_or = cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_fields_operator;
    cfg->precedence= cls_key_data.key_data0.bf0.cl_key_orhr_ipv4e_prcd;
#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t aal_l2_cls_egr_ipv6_short_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_ipv6_short_key_mask_t mask,
                                      CA_IN aal_l2_cls_ipv6_short_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
      cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   if(mask.s.is_ipv4_ipv6_key){
         if(cfg->is_ipv4_ipv6_key){
             cls_key_data.key_data12.bf.cl_key_entry_type = 1;
             cls_key_data.key_data11.bf0.cl_key_entry_type = 1;
             }else{
                 cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY;
                 cls_key_data.key_data12.bf.cl_key_entry_type = 0;
             }
    }

   cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid = TRUE;
   if(mask.s.ipv6_ecn){
       cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn = cfg->ipv6_ecn;
   }
   if(mask.s.ipv6_ecn_valid){
       cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn_mask = (cfg->ipv6_ecn_valid == 0);
   }

    if (mask.s.ipv6_da) {
      cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[0] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data10.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[0] & 0x1F) << 27 | (cfg->ipv6_da[1] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data9.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[1] & 0x1F) << 27 | (cfg->ipv6_da[2] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data8.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[2] & 0x1F) << 27 | (cfg->ipv6_da[3] >> 5 & 0x7FFFFFF);
      cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_da =
           (cfg->ipv6_da[3] & 0x1F);
    }

    if (mask.s.ipv6_sa) {
       cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[0] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data6.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[0] & 0x1F) << 27 | (cfg->ipv6_sa[1] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data5.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[1] & 0x1F) << 27 | (cfg->ipv6_sa[2] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data4.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[2] & 0x1F) << 27 | (cfg->ipv6_sa[3] >> 5 & 0x7FFFFFF);
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_sa =
            (cfg->ipv6_sa[3] & 0x1F);
     }

    if (mask.s.ipv6_ctrl) {
      cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_ip_ctrl = cfg->ipv6_ctrl;
    }

    if (mask.s.ipv6_addr_mask) {
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_mask = cfg->ipv6_addr_mask;
    }

    if (mask.s.ipv6_pt) {
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_pt = cfg->ipv6_pt;
    }

    if (mask.s.ipv6_pt_valid) {
        if(cfg->ipv6_pt_valid){
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &= 0xFD;
        }else{
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x02;
        }
    }

    if (mask.s.dscp) {
       cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_tos = (cfg->dscp >> 1) & 0x1f;
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_ip_tos = cfg->dscp  & 0x01;
    }

    if (mask.s.dscp_valid) {
        if(cfg->dscp_valid){
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &= 0xFB;
        }else{
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x04;
        }
    }

    if (mask.s.l4_flag) {
        cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_flags =  cfg->l4_flag;
      }

     if (mask.s.l4_flag_valid) {
          if (cfg->l4_flag_valid) {
             cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xF7;
          } else {
             cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x08;
          }
      }

    if (mask.s.special_pkt_code) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_special_pkt_code =  cfg->special_pkt_code;
     }

    if (mask.s.special_pkt_code_valid) {
         if (cfg->special_pkt_code_valid) {
            cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xEF;
         } else {
            cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x10;
         }
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_sc_ind = cfg->top_is_svlan;
    }
    if (mask.s.top_vlan_id) {
        cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_vid =
           cfg->top_vlan_id;
    }

    if (mask.s.top_vlan_valid) {
        if (cfg->top_vlan_valid) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xDF;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x20;
        }
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_802_1p = cfg->top_dot1p;
    }
    if (mask.s.top_dot1p_valid) {
        if (cfg->top_dot1p_valid) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xBF;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x40;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_tag_num = cfg->vlan_number;
    }
    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0xFE;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x01;
        }
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_dp = (cfg->l4_dst_port >> 11) & 0x1F;
       cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_dp = (cfg->l4_dst_port  & 0x7FF);
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_sp = cfg->l4_src_port  ;
    }
    if (mask.s.l4_ctrl) {
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_l4_ctrl = cfg->l4_ctrl  ;
    }
    if (mask.s.flow_lab) {
       cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_flow_label = (cfg->flow_lab >> 15 & 0x1F);
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_flow_label = cfg->flow_lab & 0x7FFF;
    }
    if(mask.s.flow_lab_valid){
        if (cfg->flow_lab_valid) {
           cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask &=   0x7F;
        } else {
              cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask |= 0x80;
        }
    }
    if (mask.s.fields_is_or) {
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_fields_operator =  cfg->fields_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_prcd =  cfg->precedence;
    }
    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_egr_ipv6_short_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_ipv6_short_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid=cls_key_data.key_data12.bf.cl_key_valid ;
    if((cls_key_data.key_data12.bf.cl_key_entry_type == 1) && (cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        cfg->is_ipv4_ipv6_key = 1;
    }else{
        cfg->is_ipv4_ipv6_key = 0;
    }

    cfg->ipv6_ecn = cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn;
    cfg->ipv6_ecn_valid = (cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_ecn_mask == 0);
    cfg->ipv6_da[0] = (cls_key_data.key_data11.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | ((cls_key_data.key_data10.bf1.cl_key_orhr_ipv6e_ipv6_da >> 27) & 0x1F) ;
    cfg->ipv6_da[1] = (cls_key_data.key_data10.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | ((cls_key_data.key_data9.bf1.cl_key_orhr_ipv6e_ipv6_da >> 27) & 0x1F) ;
    cfg->ipv6_da[2] = (cls_key_data.key_data9.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | ((cls_key_data.key_data8.bf1.cl_key_orhr_ipv6e_ipv6_da >> 27) & 0x1F)  ;
    cfg->ipv6_da[3] = (cls_key_data.key_data8.bf1.cl_key_orhr_ipv6e_ipv6_da << 5) | (cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_da );
    cfg->ipv6_sa[0] = (cls_key_data.key_data7.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | ((cls_key_data.key_data6.bf1.cl_key_orhr_ipv6e_ipv6_sa >> 27) & 0x1F);
    cfg->ipv6_sa[1] = (cls_key_data.key_data6.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | ((cls_key_data.key_data5.bf1.cl_key_orhr_ipv6e_ipv6_sa >> 27) & 0x1F);
    cfg->ipv6_sa[2] = (cls_key_data.key_data5.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | ((cls_key_data.key_data4.bf1.cl_key_orhr_ipv6e_ipv6_sa >> 27) & 0x1F);
    cfg->ipv6_sa[3] = (cls_key_data.key_data4.bf1.cl_key_orhr_ipv6e_ipv6_sa << 5) | (cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_sa);
    cfg->ipv6_ctrl  = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_ip_ctrl;
    cfg->ipv6_addr_mask   =cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ipv6_mask;
    cfg->ipv6_pt    =cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_pt;
    cfg->ipv6_pt_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x02) == 0);
    cfg->dscp = cls_key_data.key_data3.bf1.cl_key_orhr_ipv6e_ip_tos << 1 |cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_ip_tos;
    cfg->dscp_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x04) == 0);
    cfg->l4_flag   = cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_flags;
    cfg->l4_flag_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x08) == 0);
    cfg->special_pkt_code  =  cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_special_pkt_code ;
    cfg->special_pkt_code_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x10) == 0);
    cfg->top_is_svlan     =cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_sc_ind;
    cfg->top_vlan_id     = cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_vid;
    cfg->top_vlan_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x20) == 0);
    cfg->top_dot1p  = cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_top_802_1p;
    cfg->top_dot1p_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x40) == 0);
    cfg->vlan_number   =cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_tag_num;
    cfg->vlan_number_vld = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x01) == 0);
    cfg->l4_dst_port  =cls_key_data.key_data2.bf1.cl_key_orhr_ipv6e_l4_dp  << 11 | cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_dp;
    cfg->l4_src_port   = cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_l4_sp ;
    cfg->l4_ctrl  = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_l4_ctrl  ;
    cfg->flow_lab = cls_key_data.key_data1.bf1.cl_key_orhr_ipv6e_flow_label << 15  |cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_flow_label;
    cfg->flow_lab_valid = ((cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_mask & 0x80) == 0);
    cfg->fields_is_or = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_fields_operator ;
    cfg->precedence = cls_key_data.key_data0.bf1.cl_key_orhr_ipv6e_prcd;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t
aal_l2_cls_egr_l2r_1st_key_set(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t   mask,
                          CA_IN aal_l2_cls_l2r_key_t        * cfg)
{
    ca_status_t ret = CA_E_OK;

    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id >> 6) & 0x3f;
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id) & 0x3f;
    }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.mac_field_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl =
            cfg->mac_field_ctrl;
    }
    if (mask.s.mac_da) {
       cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da =
            ((cfg->mac_da[0] << 11) | (cfg->mac_da[1] << 3) | ((cfg->mac_da[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da =
            (((cfg->mac_da[2] & 0x1f) << 24) | (cfg->mac_da[3] << 16) | (cfg->mac_da[4] << 8) | cfg->mac_da[5]);
    }
    if (mask.s.mac_sa) {
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (cfg->mac_sa[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (((cfg->mac_sa[0] & 0x1f) << 27) | (cfg->mac_sa[1] << 19) |
             (cfg->mac_sa[2] << 11) | (cfg->mac_sa[3] << 3) | ((cfg->mac_sa[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            ((cfg->mac_sa[4] & 0x1f) << 8) | (cfg->mac_sa[5]);
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p = cfg->inner_dot1p;
    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl;
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp =  cfg->l4_src_port;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port & 0xf);
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &=   0x2;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd = cfg->precedence;
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t
aal_l2_cls_egr_l2r_1st_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg)
{
    __aal_cls_key_data_t cls_key_data;
     ca_status_t ret = CA_E_OK;
     if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
         return CA_E_PARAM;
     }

     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


     cfg->top_dot1p =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p;
     cfg->inner_dot1p =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p;

     cfg->vlan_field_ctrl =
        cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl;
     cfg->top_vlan_id =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid;
     cfg->top_is_svlan =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind;
     cfg->inner_vlan_id =
         (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid << 6) | (cls_key_data.key_data7.bf2.
                 cl_key_tre_l2rl2r_1_inner_vid);
     cfg->inner_is_svlan =
        cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind;

     cfg->dot1p_ctrl = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl;

     cfg->vlan_number_vld = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x1) == 0);
     cfg->vlan_number     = cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num;

     cfg->ethernet_type_vld = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x2) == 0);
     cfg->ethernet_type = cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt;
     cfg->field_is_or = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator;

     cfg->l4_src_port = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp;
     cfg->l4_dst_port = (cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp | ((cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp << 4) & 0xfff0));

     cfg->mac_field_ctrl =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl;

     cfg->mac_da[0] =
         (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 11) & 0xff;
     cfg->mac_da[1] =
         (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 3) & 0xff;
     cfg->mac_da[2] =
         ((cls_key_data.key_data11.bf2.
           cl_key_tre_l2rl2r_1_mac_da & 7) << 5 & 0xE0) | ((cls_key_data.key_data10.bf2.
                   cl_key_tre_l2rl2r_1_mac_da
                   >> 24) & 0x1f);
     cfg->mac_da[3] =
         (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 16) & 0xff;
     cfg->mac_da[4] =
         (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 8) & 0xff;
     cfg->mac_da[5] =
         (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 0) & 0xff;
     cfg->mac_sa[0] =
         ((cls_key_data.key_data10.bf2.
           cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data9.bf2.
                   cl_key_tre_l2rl2r_1_mac_sa
                   >> 27) & 0x1f);
     cfg->mac_sa[1] =
         (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 19) & 0xff;
     cfg->mac_sa[2] =
         (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 11) & 0xff;
     cfg->mac_sa[3] =
         (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 3) & 0xff;
     cfg->mac_sa[4] =
         ((cls_key_data.key_data9.bf2.
           cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data8.bf2.
                   cl_key_tre_l2rl2r_1_mac_sa
                   >> 8) & 0x1f);
     cfg->mac_sa[5] =
         (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 0) & 0xff;

     cfg->l4_port_field_ctrl =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl;
     cfg->precedence =cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd;
#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

     return ret;

}


ca_status_t
aal_l2_cls_egr_l2r_2nd_key_set(CA_IN ca_device_id_t device_id,
                          CA_IN ca_uint32_t entry_id,
                          CA_IN aal_l2_cls_l2r_key_mask_t mask,
                          CA_IN aal_l2_cls_l2r_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;

    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id >> 3) & 0x1ff;
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id) & 0x7;
    }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.mac_field_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl =
            cfg->mac_field_ctrl;
    }
    if (mask.s.mac_da) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da =
            ((cfg->mac_da[0] << 14) | (cfg->mac_da[1] << 6) | ((cfg->
                    mac_da[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da =
            (((cfg->mac_da[2] & 0x3) << 24) | (cfg->mac_da[3] << 16) |
             (cfg->mac_da[4] << 8) | cfg->mac_da[5]);
    }
    if (mask.s.mac_sa) {
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (cfg->mac_sa[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (((cfg->mac_sa[0] & 0x3) << 30) | (cfg->mac_sa[1] << 22) |
             (cfg->mac_sa[2] << 14) | (cfg->mac_sa[3] << 6) | ((cfg->
                     mac_sa[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            ((cfg->mac_sa[4] & 0x3) << 8) | (cfg->mac_sa[5]);

    }

    if (mask.s.top_dot1p) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p = cfg->inner_dot1p;

    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl = cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl = cfg->l4_port_field_ctrl ;
    }
    if (mask.s.l4_src_port) {

       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp =  cfg->l4_src_port;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port & 1);
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port >> 1) & 0x7fff;
    }


    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd = cfg->precedence;
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t
aal_l2_cls_egr_l2r_2nd_key_get(CA_IN ca_device_id_t        device_id,
                          CA_IN ca_uint32_t                 entry_id,
                          CA_OUT aal_l2_cls_l2r_key_t        * cfg)
{
    __aal_cls_key_data_t cls_key_data;
     ca_status_t ret = CA_E_OK;
     if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
         return CA_E_PARAM;
     }

     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);
    cfg->precedence =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd;

    cfg->field_is_or = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator;

    cfg->l4_src_port = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp;
    cfg->l4_dst_port = (cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp | ((cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp << 1) & 0xfffe));

    cfg->dot1p_ctrl = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl;
    cfg->top_dot1p =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p;
    cfg->inner_dot1p =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p;
    cfg->vlan_number_vld = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x1) == 0);
    cfg->vlan_number =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num;

    cfg->vlan_field_ctrl =
     cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl;
    cfg->top_vlan_id =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid;
    cfg->top_is_svlan =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind;
    cfg->inner_vlan_id =
      (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf2.
              cl_key_tre_l2rl2r_2_inner_vid);
    cfg->inner_is_svlan =
     cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind;
    cfg->ethernet_type = cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt;
    cfg->mac_field_ctrl =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl;
    cfg->mac_da[0] =
       (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da[1] =
       (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da[2] =
       ((cls_key_data.key_data5.bf2.
         cl_key_tre_l2rl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf2.
                 cl_key_tre_l2rl2r_2_mac_da
                 >> 24) & 0x3);
    cfg->mac_da[3] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da[4] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da[5] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa[0] =
       ((cls_key_data.key_data4.bf2.
         cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf2.
                 cl_key_tre_l2rl2r_2_mac_sa
                 >> 30) & 0x3);
    cfg->mac_sa[1] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa[2] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa[3] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa[4] =
       ((cls_key_data.key_data3.bf2.
         cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf2.
                 cl_key_tre_l2rl2r_2_mac_sa
                 >> 8) & 0x3);
    cfg->mac_sa[5] =
       (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl;


    cfg->ethernet_type_vld = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x2) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;
}

ca_status_t aal_l2_cls_egr_l3r_1st_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;

  __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if(mask.s.ecn){
        cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn_mask = (cfg->ecn_valid == 0);
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_inner_vid = cfg->inner_vlan_id ;
    }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.ip_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_ip_ctrl =
            cfg->ip_ctrl;
    }
    if (mask.s.ip_da) {
       cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ipv4_da = (cfg->ip_da >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_da = cfg->ip_da & 0x7FFF;
    }
    if (mask.s.ip_sa) {
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_sa =
            (cfg->ip_sa >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_sa = cfg->ip_sa & 0x7FFF;
    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_802_1p = cfg->inner_dot1p;
    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl;
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_sp =  cfg->l4_src_port & 0x3FFF;
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_sp  = (cfg->l4_src_port >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_dp  = cfg->l4_dst_port ;
    }

    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_option = cfg->has_ipop;
    }
    if (mask.s.has_ipop_valid) {
        if (cfg->has_ipop_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_tos = cfg->dscp;
    }
    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_pt = cfg->ip_pt;
    }
    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_fragment = cfg->ipv4_fragment;
    }
    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag) {
        cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_l4_flags = ((cfg->l4_flag >> 2) & 0x01);
        cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_l4_flags = (cfg->l4_flag & 0x03);
    }
    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_special_pkt_code = cfg->special_pkt_code;
    }
    if (mask.s.special_pkt_code_valid) {
        if (cfg->special_pkt_code_valid) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_tag_num = cfg->vlan_number;
    }
    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_1_prcd = cfg->precedence;
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_egr_l3r_1st_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3r_key_t *cfg)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);
    cfg->ecn = cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn;
    cfg->ecn_valid = (cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ecn_mask == 0);
    cfg->vlan_field_ctrl =
      cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_vid_ctrl;
    cfg->top_vlan_id      =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_vid;
    cfg->top_is_svlan     =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_sc_ind;
    cfg->inner_vlan_id    = (cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_inner_vid);
    cfg->inner_is_svlan   =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_sc_ind;
    cfg->top_dot1p        =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_top_802_1p;
    cfg->inner_dot1p      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_802_1p;
    cfg->field_is_or      =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_fields_operator;
    cfg->l4_src_port      = cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_sp | (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_sp <<14);
    cfg->l4_dst_port      = (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_dp);
    cfg->vlan_number      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_tag_num;
    cfg->precedence       =cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_1_prcd;
    cfg->ethernet_type    =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_ev2_pt;
    cfg->ip_ctrl          =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_ip_ctrl;
    cfg->dot1p_ctrl       =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_dot1p_ctrl;
    cfg->ip_da            =cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ipv4_da <<15 |cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_da;
    cfg->ip_sa            =cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_sa <<15 |cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_sa;
    cfg->l4_port_field_ctrl =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_ctrl;
    cfg->ipv4_fragment    =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_fragment;
    cfg->l4_flag          =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_l4_flags<<2 | cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_l4_flags;
    cfg->has_ipop         =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_option;
    cfg->dscp             =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_tos;
    cfg->ip_pt            =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_pt;
    cfg->special_pkt_code =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_special_pkt_code;
    cfg->ipv4_fragment_valid   = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x20) == 0x00);
    cfg->l4_flag_valid         = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x40) == 0x00);
    cfg->vlan_number_vld       = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x01) == 0x00);
    cfg->ip_pt_valid           = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x04) == 0x00);
    cfg->has_ipop_valid        = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x10) == 0x00);
    cfg->dscp_valid            = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld     = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid =((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x80) == 0x00);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}



ca_status_t aal_l2_cls_egr_l3r_2nd_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3r_key_t *cfg)


{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if(mask.s.ecn){
        cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn_mask = (cfg->ecn_valid == 0);
    }

    if (mask.s.vlan_field_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl;
    }
    if (mask.s.top_vlan_id) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.inner_vlan_id) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid = cfg->inner_vlan_id;
            }
    if (mask.s.inner_is_svlan) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind =
            cfg->inner_is_svlan;
    }
    if (mask.s.ip_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl =
            cfg->ip_ctrl;
    }
    if (mask.s.ip_da) {
       cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            (cfg->ip_da >> 12) & 0xfffff;
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            cfg->ip_da & 0xfff;
    }
    if (mask.s.ip_sa) {
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            (cfg->ip_sa >> 12) & 0xfffff;
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            cfg->ip_sa & 0xfff;
    }

    if (mask.s.top_dot1p) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p = (cfg->top_dot1p & 1);
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p = ((cfg->top_dot1p >> 1) & 3);
    }

    if (mask.s.inner_dot1p) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p = cfg->inner_dot1p;

    }
    if (mask.s.dot1p_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl = cfg->dot1p_ctrl;
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl = cfg->l4_port_field_ctrl ;
    }
    if (mask.s.l4_src_port) {

       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp = (cfg->l4_src_port & 0x7ff);
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp = ((cfg->l4_src_port >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp = cfg->l4_dst_port;
    }


    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd = cfg->precedence;
    }

    if (mask.s.ip_pt) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt = cfg->ip_pt;
    }

    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos = cfg->dscp;
    }

    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option = cfg->has_ipop;
    }

    if (mask.s.has_ipop_valid) {
        if (cfg->has_ipop_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags = cfg->l4_flag;
    }

    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment = cfg->ipv4_fragment;
    }

    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code >> 4) & 1;
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid) {
        if (cfg->special_pkt_code_valid) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x80;
        }
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_egr_l3r_2nd_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3r_key_t *cfg)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);
    cfg->ecn                = cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn ;
    cfg->ecn_valid          = (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ecn_mask == 0);
    cfg->vlan_field_ctrl    =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl           ;
    cfg->top_vlan_id        =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid            ;
    cfg->top_is_svlan       =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id      =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid          ;
    cfg->inner_is_svlan     =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl            =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl            ;
    cfg->ip_da              = (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da)          ;
    cfg->ip_sa              = (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa)         ;
    cfg->top_dot1p          =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p    | (cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl            ;
    cfg->l4_src_port        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp | (cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp              ;
    cfg->vlan_number        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num            ;
    cfg->ethernet_type      =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt             ;
    cfg->field_is_or        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator    ;
    cfg->precedence         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd               ;
    cfg->ip_pt              =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt              ;
    cfg->special_pkt_code   = (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code    ;
    cfg->dscp               =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos             ;
    cfg->has_ipop           =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option        ;
    cfg->l4_flag            =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags           ;
    cfg->ipv4_fragment      =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment      ;
    cfg->ipv4_fragment_valid  = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid        = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld      = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid          = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid       = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid           = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld    = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid=((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x80) == 0x00);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}




ca_status_t aal_l2_cls_egr_cmr_1st_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;



    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
     return CA_E_PARAM;
    }
    printk("%s %d \n",__FUNCTION__,__LINE__ );

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if(mask.s.ecn){
        cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn_mask = (cfg->ecn_valid==0);
    }

    if (mask.s.top_vlan_id) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid = (cfg->top_vlan_id >> 3) & 0x1ff;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid = cfg->top_vlan_id & 0x07;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.vlan_field_valid) {
     if (cfg->vlan_field_valid) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xBFF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x400;
     }
    }

    if (mask.s.ip_addr_is_sa) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel =
            cfg->ip_addr_is_sa;
    }
    if (mask.s.ip_addr) {
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            (cfg->ip_addr >> 31) & 0x01;
       cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            cfg->ip_addr & 0x7fffffff;
    }
    if (mask.s.ip_addr_valid) {
        if (cfg->ip_addr_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xDFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel =
            cfg->mac_addr_is_sa;
    }

    if (mask.s.mac_addr) {
       cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            (cfg->mac_addr[0] << 9) | (cfg->mac_addr[1] << 1) | ((cfg->mac_addr[2] >> 7) & 0x01);
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            ((cfg->mac_addr[2] & 0x7f) << 24) | (cfg->mac_addr[3] << 16) | (cfg->mac_addr[4] << 8) | (cfg->mac_addr[5]);
    }

    if (mask.s.mac_addr_valid) {
        if (cfg->mac_addr_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xEFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x100;
        }
    }

    if (mask.s.top_dot1p) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.dot1p_valid) {
        if (cfg->dot1p_valid ) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0x7FF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x800;
        }
    }

    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl;
    }
    if (mask.s.l4_src_port) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp =  cfg->l4_src_port & 0x3FFF;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp  = (cfg->l4_src_port >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp  = cfg->l4_dst_port ;
    }

    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &=   0xFFE;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x001;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option = cfg->has_ipop;
    }
    if (mask.s.has_ipop_valid) {
     if (cfg->has_ipop_valid) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFEF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x010;
     }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos = cfg->dscp;
    }
    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFF7;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x008;
        }
    }
    if (mask.s.ip_pt) {
        cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt >> 7) &0x1;
        cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt&0x7f);
    }
    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFB;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x004;
        }
    }
    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment = cfg->ipv4_fragment;
    }
    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFDF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x020;
        }
    }
    if (mask.s.l4_flag) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags = cfg->l4_flag;
    }
    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFBF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x040;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code = cfg->special_pkt_code;
    }
    if (mask.s.special_pkt_code_valid) {
        if (cfg->special_pkt_code_valid) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xF7F;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x080;
     }
    }
    if (mask.s.vlan_number) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num = cfg->vlan_number;
    }
    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFD;
        } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x002;
     }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd = cfg->precedence;
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t aal_l2_cls_egr_cmr_1st_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmr_key_t *cfg)
{

    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->ecn                  = cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn;
    cfg->ecn_valid           =  (cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_ecn_mask == 0);
    cfg->top_vlan_id          =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid <<3 |cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid;
    cfg->top_is_svlan         =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind;
    cfg->ip_addr_is_sa        =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel;
    cfg->ip_addr              =cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr << 31 |cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr;
    cfg->mac_addr_is_sa       =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel;
    cfg->mac_addr[0]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr[1]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr[2]          = (((cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr[3]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr[4]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr[5]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p            =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p;
    cfg->l4_port_field_ctrl   =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl;
    cfg->l4_src_port          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp | (cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp << 14);
    cfg->l4_dst_port          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp;
    cfg->vlan_number          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num;
    cfg->ethernet_type        =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt;
    cfg->field_is_or          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator;
    cfg->precedence           =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd;
    cfg->ip_pt                =cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt << 7 |cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt;
    cfg->dscp                 =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos;
    cfg->has_ipop             =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option;
    cfg->l4_flag              =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags;
    cfg->ipv4_fragment        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment;
    cfg->special_pkt_code        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code;
    cfg->dot1p_valid             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x800) == 0);
    cfg->vlan_field_valid        = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x400) == 0);
    cfg->ip_addr_valid           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x200) == 0);
    cfg->mac_addr_valid          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid  = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x080) == 0);
    cfg->l4_flag_valid           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid     = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x020) == 0);
    cfg->has_ipop_valid          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x010) == 0);
    cfg->dscp_valid              = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x008) == 0);
    cfg->ip_pt_valid             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld       = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x002) == 0);
    cfg->vlan_number_vld         = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x001) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t aal_l2_cls_egr_cmr_2nd_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
     return CA_E_PARAM;
    }
    printk("%s %d \n",__FUNCTION__,__LINE__ );

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if(mask.s.ecn){
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn = cfg->ecn;
    }
    if(mask.s.ecn_valid){
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn_mask = (cfg->ecn_valid == 0);
    }

    if (mask.s.top_vlan_id) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid = cfg->top_vlan_id;
    }
    if (mask.s.top_is_svlan) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind =
            cfg->top_is_svlan;
    }
    if (mask.s.vlan_field_valid) {
        if (cfg->vlan_field_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa;
    }
    if (mask.s.ip_addr) {
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            (cfg->ip_addr >> 28) & 0xf;
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            cfg->ip_addr & 0xfffffff;
    }
    if (mask.s.ip_addr_valid) {
        if (cfg->ip_addr_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa;
    }

    if (mask.s.mac_addr) {
       cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            (cfg->mac_addr[0] << 12) | (cfg->mac_addr[1] << 4) | ((cfg->mac_addr[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            ((cfg->mac_addr[2] & 0xf) << 24) | (cfg->mac_addr[3] << 16) | (cfg->mac_addr[4] << 8) | (cfg->mac_addr[5]);
    }

    if (mask.s.mac_addr_valid) {
        if (cfg->mac_addr_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p = cfg->top_dot1p;
    }

    if (mask.s.dot1p_valid) {
        if (cfg->dot1p_valid)
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl = cfg->l4_port_field_ctrl ;
    }
    if (mask.s.l4_src_port) {

       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp = (cfg->l4_src_port & 0x7ff);
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp = ((cfg->l4_src_port >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp = cfg->l4_dst_port;
    }


    if (mask.s.vlan_number_vld) {
        if (cfg->vlan_number_vld) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num = cfg->vlan_number;
    }

    if (mask.s.ethernet_type_vld) {
        if (cfg->ethernet_type_vld) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt = cfg->ethernet_type;
    }
    if (mask.s.field_is_or) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator = cfg->field_is_or;
    }
    if (mask.s.precedence) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd = cfg->precedence;
    }

    if (mask.s.ip_pt) {
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt = ((cfg->ip_pt >> 4) & 0x0f);
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt = (cfg->ip_pt & 0x0f);
    }

    if (mask.s.ip_pt_valid) {
        if (cfg->ip_pt_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos = cfg->dscp;
    }

    if (mask.s.dscp_valid) {
        if (cfg->dscp_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option = cfg->has_ipop;
    }

    if (mask.s.has_ipop_valid) {
        if (cfg->has_ipop_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags = cfg->l4_flag;
    }

    if (mask.s.l4_flag_valid) {
        if (cfg->l4_flag_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment = cfg->ipv4_fragment;
    }

    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code = cfg->special_pkt_code;
    }

    if (mask.s.ipv4_fragment_valid) {
        if (cfg->ipv4_fragment_valid) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x080;
        }
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


 ca_status_t aal_l2_cls_egr_cmr_2nd_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->ecn                  = cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn;
    cfg->ecn_valid           =  (cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_ecn_mask == 0);
    cfg->top_vlan_id          =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid;
    cfg->top_is_svlan         =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind;
    cfg->ip_addr_is_sa        =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel;
    cfg->ip_addr              =cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr;
    cfg->mac_addr_is_sa       =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel;
    cfg->mac_addr[0]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 12 & 0xff;
    cfg->mac_addr[1]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 4 & 0xff;
    cfg->mac_addr[2]          = (((cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 24) & 0xf));
    cfg->mac_addr[3]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 16 & 0xff);
    cfg->mac_addr[4]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 8 & 0xff);
    cfg->mac_addr[5]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 0 & 0xff);
    cfg->top_dot1p            =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p;
    cfg->l4_port_field_ctrl   =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl;
    cfg->l4_src_port          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp | (cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp << 11);
    cfg->l4_dst_port          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp;
    cfg->vlan_number          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num;
    cfg->ethernet_type        =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt;
    cfg->field_is_or          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator;
    cfg->precedence           =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd;
    cfg->ip_pt                =cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt;
    cfg->dscp                 =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos;
    cfg->has_ipop             =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option;
    cfg->l4_flag              =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags;
    cfg->ipv4_fragment        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment;
    cfg->special_pkt_code        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code;
    cfg->dot1p_valid             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid        = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid  = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid     = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid              = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld       = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld         = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x001) == 0);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t
aal_l2_cls_egr_l2rl2r_key_set(CA_IN ca_device_id_t device_id,
                              CA_IN ca_uint32_t entry_id,
                              CA_IN aal_l2_cls_l2rl2r_key_mask_t mask,
                              CA_IN aal_l2_cls_l2rl2r_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_EGR_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

    cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
    cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;
    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id_0 >> 6) & 0x3f;
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_vid =
            (cfg->inner_vlan_id_0) & 0x3f;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.mac_field_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl =
            cfg->mac_field_ctrl_0;
    }
    if (mask.s.mac_da_0) {
       cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da =
            ((cfg->mac_da_0[0] << 11) | (cfg->mac_da_0[1] << 3) | ((cfg->mac_da_0[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da =
            (((cfg->mac_da_0[2] & 0x1f) << 24) | (cfg->mac_da_0[3] << 16) | (cfg->mac_da_0[4] << 8) | cfg->mac_da_0[5]);
    }
    if (mask.s.mac_sa_0) {
       cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (cfg->mac_sa_0[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            (((cfg->mac_sa_0[0] & 0x1f) << 27) | (cfg->mac_sa_0[1] << 19) |
             (cfg->mac_sa_0[2] << 11) | (cfg->mac_sa_0[3] << 3) | ((cfg->mac_sa_0[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa =
            ((cfg->mac_sa_0[4] & 0x1f) << 8) | (cfg->mac_sa_0[5]);
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp =  cfg->l4_src_port_0;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port_0 & 0xf);
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp  = (cfg->l4_dst_port_0 >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &=	0x2;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num = cfg->vlan_number_0;
    }


    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd = cfg->precedence_0;
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1 >> 3) & 0x1ff;
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1) & 0x7;
    }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.mac_field_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl =
            cfg->mac_field_ctrl_1;
    }
    if (mask.s.mac_da_1) {
       cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da =
            ((cfg->mac_da_1[0] << 14) | (cfg->mac_da_1[1] << 6) | ((cfg->
                    mac_da_1[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da =
            (((cfg->mac_da_1[2] & 0x3) << 24) | (cfg->mac_da_1[3] << 16) |
             (cfg->mac_da_1[4] << 8) | cfg->mac_da_1[5]);
    }
    if (mask.s.mac_sa_1) {
       cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (cfg->mac_sa_1[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            (((cfg->mac_sa_1[0] & 0x3) << 30) | (cfg->mac_sa_1[1] << 22) |
             (cfg->mac_sa_1[2] << 14) | (cfg->mac_sa_1[3] << 6) | ((cfg->
                     mac_sa_1[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa =
            ((cfg->mac_sa_1[4] & 0x3) << 8) | (cfg->mac_sa_1[5]);

    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp =  cfg->l4_src_port_1;
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port_1 & 1);
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp = (cfg->l4_dst_port_1 >> 1) & 0x7fff;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd = cfg->precedence_1;
    }
    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t
aal_l2_cls_egr_l2rl2r_key_get(CA_IN ca_device_id_t device_id,
                              CA_IN ca_uint32_t entry_id,
                              CA_OUT aal_l2_cls_l2rl2r_key_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;


    cfg->top_dot1p_0 =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_top_802_1p;
    cfg->inner_dot1p_0 =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_802_1p;

    cfg->vlan_field_ctrl_0 =
     cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_vid_ctrl;
    cfg->top_vlan_id_0 =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_vid;
    cfg->top_is_svlan_0 =cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_top_sc_ind;
    cfg->inner_vlan_id_0 =
      (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_inner_vid << 6) | (cls_key_data.key_data7.bf2.
              cl_key_tre_l2rl2r_1_inner_vid);
    cfg->inner_is_svlan_0 =
     cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_inner_sc_ind;

    cfg->dot1p_ctrl_0 = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_dot1p_ctrl;

    cfg->vlan_number_vld_0 = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x1) == 0);
    cfg->vlan_number_0 =cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_tag_num;


    cfg->ethernet_type_vld_0 = ((cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mask & 0x2) == 0);
    cfg->ethernet_type_0 = cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_ev2_pt;
    cfg->field_is_or_0 = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_fields_operator;

    cfg->l4_src_port_0 = cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_sp;
    cfg->l4_dst_port_0 = (cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_dp | ((cls_key_data.key_data7.bf2.cl_key_tre_l2rl2r_1_l4_dp << 4) & 0xfff0));

    cfg->mac_field_ctrl_0 =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_mac_ctrl;

    cfg->mac_da_0[0] =
      (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 11) & 0xff;
    cfg->mac_da_0[1] =
      (cls_key_data.key_data11.bf2.cl_key_tre_l2rl2r_1_mac_da >> 3) & 0xff;
    cfg->mac_da_0[2] =
      ((cls_key_data.key_data11.bf2.
        cl_key_tre_l2rl2r_1_mac_da & 7) << 5 & 0xE0) | ((cls_key_data.key_data10.bf2.
                cl_key_tre_l2rl2r_1_mac_da
                >> 24) & 0x1f);
    cfg->mac_da_0[3] =
      (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 16) & 0xff;
    cfg->mac_da_0[4] =
      (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 8) & 0xff;
    cfg->mac_da_0[5] =
      (cls_key_data.key_data10.bf2.cl_key_tre_l2rl2r_1_mac_da >> 0) & 0xff;
    cfg->mac_sa_0[0] =
      ((cls_key_data.key_data10.bf2.
        cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data9.bf2.
                cl_key_tre_l2rl2r_1_mac_sa
                >> 27) & 0x1f);
    cfg->mac_sa_0[1] =
      (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 19) & 0xff;
    cfg->mac_sa_0[2] =
      (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 11) & 0xff;
    cfg->mac_sa_0[3] =
      (cls_key_data.key_data9.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 3) & 0xff;
    cfg->mac_sa_0[4] =
      ((cls_key_data.key_data9.bf2.
        cl_key_tre_l2rl2r_1_mac_sa & 0x7) << 5 & 0xE0) | ((cls_key_data.key_data8.bf2.
                cl_key_tre_l2rl2r_1_mac_sa
                >> 8) & 0x1f);
    cfg->mac_sa_0[5] =
      (cls_key_data.key_data8.bf2.cl_key_tre_l2rl2r_1_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf2.cl_key_tre_l2rl2r_1_l4_ctrl;
    cfg->precedence_0 =cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_1_prcd;

    cfg->precedence_1 =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_prcd;

    cfg->field_is_or_1 = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_fields_operator;

    cfg->l4_src_port_1 = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_sp;
    cfg->l4_dst_port_1 = (cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_dp | ((cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_l4_dp << 1) & 0xfffe));

    cfg->dot1p_ctrl_1 = cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_dot1p_ctrl;
    cfg->top_dot1p_1 =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_top_802_1p;
    cfg->inner_dot1p_1 =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_802_1p;
    cfg->vlan_number_vld_1 = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x1) == 0);
    cfg->vlan_number_1 =cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_tag_num;

    cfg->vlan_field_ctrl_1 =
     cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_vid_ctrl;
    cfg->top_vlan_id_1 =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_vid;
    cfg->top_is_svlan_1 =cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_top_sc_ind;
    cfg->inner_vlan_id_1 =
      (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf2.
              cl_key_tre_l2rl2r_2_inner_vid);
    cfg->inner_is_svlan_1 =
     cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_inner_sc_ind;
    cfg->ethernet_type_1 = cls_key_data.key_data1.bf2.cl_key_tre_l2rl2r_2_ev2_pt;
    cfg->mac_field_ctrl_1 =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mac_ctrl;
    cfg->mac_da_1[0] =
       (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da_1[1] =
       (cls_key_data.key_data5.bf2.cl_key_tre_l2rl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da_1[2] =
       ((cls_key_data.key_data5.bf2.
         cl_key_tre_l2rl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf2.
                 cl_key_tre_l2rl2r_2_mac_da
                 >> 24) & 0x3);
    cfg->mac_da_1[3] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da_1[4] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da_1[5] =
       (cls_key_data.key_data4.bf2.cl_key_tre_l2rl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa_1[0] =
       ((cls_key_data.key_data4.bf2.
         cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf2.
                 cl_key_tre_l2rl2r_2_mac_sa
                 >> 30) & 0x3);
    cfg->mac_sa_1[1] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa_1[2] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa_1[3] =
       (cls_key_data.key_data3.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa_1[4] =
       ((cls_key_data.key_data3.bf2.
         cl_key_tre_l2rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf2.
                 cl_key_tre_l2rl2r_2_mac_sa
                 >> 8) & 0x3);
    cfg->mac_sa_1[5] =
       (cls_key_data.key_data2.bf2.cl_key_tre_l2rl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_1 =cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_l4_ctrl;
    cfg->ethernet_type_vld_1 = ((cls_key_data.key_data0.bf2.cl_key_tre_l2rl2r_2_mask & 0x2) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}

ca_status_t aal_l2_cls_egr_l2rl3r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l2rl3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l2rl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }


   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;


    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_inner_vid =
            (cfg->inner_vlan_id_0 >> 6) & 0x3f;
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_vid =
            (cfg->inner_vlan_id_0) & 0x3f;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.mac_field_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mac_ctrl =
            cfg->mac_field_ctrl_0;
    }
    if (mask.s.mac_da_0) {
       cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da =
            ((cfg->mac_da_0[0] << 11) | (cfg->mac_da_0[1] << 3) | ((cfg->mac_da_0[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da =
            (((cfg->mac_da_0[2] & 0x1f) << 24) | (cfg->mac_da_0[3] << 16) | (cfg->mac_da_0[4] << 8) | cfg->mac_da_0[5]);
    }
    if (mask.s.mac_sa_0) {
       cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_sa =
            (cfg->mac_sa_0[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa =
            (((cfg->mac_sa_0[0] & 0x1f) << 27) | (cfg->mac_sa_0[1] << 19) |
             (cfg->mac_sa_0[2] << 11) | (cfg->mac_sa_0[3] << 3) | ((cfg->mac_sa_0[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_mac_sa =
            ((cfg->mac_sa_0[4] & 0x1f) << 8) | (cfg->mac_sa_0[5]);
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_sp =  cfg->l4_src_port_0;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_dp  = (cfg->l4_dst_port_0 & 0xf);
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_l4_dp  = (cfg->l4_dst_port_0 >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask &=	0x2;
        } else {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_tag_num = cfg->vlan_number_0;
    }


    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_1_prcd = cfg->precedence_0;
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_vid = cfg->inner_vlan_id_1;
            }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.ip_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_ip_ctrl =
            cfg->ip_ctrl_1;
    }
    if (mask.s.ip_da_1) {
       cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ipv4_da =
            (cfg->ip_da_1 >> 12) & 0xfffff;
       cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_da =
            cfg->ip_da_1 & 0xfff;
    }
    if (mask.s.ip_sa_1) {
       cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_sa =
            (cfg->ip_sa_1 >> 12) & 0xfffff;
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_sa =
            cfg->ip_sa_1 & 0xfff;
    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_top_802_1p = (cfg->top_dot1p_1 & 1);
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_802_1p = ((cfg->top_dot1p_1 >> 1) & 3);
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_pt = cfg->ip_pt_1;
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 >> 4) & 1;
       cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid_1) {
        if (cfg->special_pkt_code_valid_1) {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask |= 0x80;
        }
    }
    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_egr_l2rl3r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l2rl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }


     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid                =           cls_key_data.key_data12.bf.cl_key_valid                           ;
    cfg->vlan_field_ctrl_0    =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_vid_ctrl           ;
    cfg->top_vlan_id_0        =           cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_vid            ;
    cfg->top_is_svlan_0       =           cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_top_sc_ind         ;
    cfg->inner_vlan_id_0      = (cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_inner_vid << 6) |cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_vid      ;
    cfg->inner_is_svlan_0     =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_sc_ind       ;
    cfg->mac_field_ctrl_0     =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mac_ctrl           ;
    cfg->mac_da_0[0]          = (cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da) >> 11 & 0xff  ;
    cfg->mac_da_0[1]          = (cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da) >> 3 & 0xff  ;
    cfg->mac_da_0[2]          = ((cls_key_data.key_data11.bf3.cl_key_tre_l2rl3r_1_mac_da & 0x07) << 5) | ((cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 24) & 0x1f);
    cfg->mac_da_0[3]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 16) & 0xff    ;
    cfg->mac_da_0[4]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 8) & 0xff     ;
    cfg->mac_da_0[5]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_da >> 0) & 0xff     ;
    cfg->mac_sa_0[0]          = (cls_key_data.key_data10.bf3.cl_key_tre_l2rl3r_1_mac_sa << 5) | ((cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 27) & 0x1f);
    cfg->mac_sa_0[1]          = (cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 19) & 0xff;
    cfg->mac_sa_0[2]          = (cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 11) & 0xff;
    cfg->mac_sa_0[3]          = (cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa >>  3) & 0xff;
    cfg->mac_sa_0[4]          = (((cls_key_data.key_data9.bf3.cl_key_tre_l2rl3r_1_mac_sa & 0x7) << 5) | ((cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_mac_sa >> 8) & 0xff));
    cfg->mac_sa_0[5]          = (cls_key_data.key_data8.bf3.cl_key_tre_l2rl3r_1_mac_sa & 0xff);
    cfg->top_dot1p_0          =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_top_802_1p         ;
    cfg->inner_dot1p_0        =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_inner_802_1p       ;
    cfg->dot1p_ctrl_0         =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_0 =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_ctrl            ;
    cfg->l4_src_port_0        =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_sp              ;
    cfg->l4_dst_port_0        = (cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_l4_dp << 4) |cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_l4_dp              ;
    cfg->vlan_number_0        =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_tag_num            ;
    cfg->ethernet_type_0      =           cls_key_data.key_data7.bf3.cl_key_tre_l2rl3r_1_ev2_pt             ;
    cfg->field_is_or_0        =           cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_fields_operator    ;
    cfg->precedence_0         =           cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_1_prcd               ;
    cfg->vlan_number_vld_0    = ((cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask & 0x2)  == 0x00);
    cfg->ethernet_type_vld_0  = ((cls_key_data.key_data6.bf3.cl_key_tre_l2rl3r_1_mask & 0x1)  == 0x00);

    cfg->vlan_field_ctrl_1    =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_vid_ctrl           ;
    cfg->top_vlan_id_1        =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_vid            ;
    cfg->top_is_svlan_1       =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id_1      =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_vid          ;
    cfg->inner_is_svlan_1     =           cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl_1            =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_ip_ctrl            ;
    cfg->ip_da_1              = (cls_key_data.key_data5.bf3.cl_key_tre_l2rl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_da)          ;
    cfg->ip_sa_1              = (cls_key_data.key_data4.bf3.cl_key_tre_l2rl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_sa)         ;
    cfg->top_dot1p_1          =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_top_802_1p    | (cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p_1        =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl_1         =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_1 =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_ctrl            ;
    cfg->l4_src_port_1        =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_l4_sp | (cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port_1        =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_l4_dp              ;
    cfg->vlan_number_1        =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_tag_num            ;
    cfg->ethernet_type_1      =           cls_key_data.key_data1.bf3.cl_key_tre_l2rl3r_2_ev2_pt             ;
    cfg->field_is_or_1        =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_fields_operator    ;
    cfg->precedence_1         =           cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_prcd               ;
    cfg->ip_pt_1              =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_pt              ;
    cfg->special_pkt_code_1   = (cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf3.cl_key_tre_l2rl3r_2_special_pkt_code    ;
    cfg->dscp_1               =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ip_tos             ;
    cfg->has_ipop_1           =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_option        ;
    cfg->l4_flag_1            =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_l4_flags           ;
    cfg->ipv4_fragment_1      =           cls_key_data.key_data3.bf3.cl_key_tre_l2rl3r_2_ipv4_fragment      ;


    cfg->ipv4_fragment_valid_1 = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_1      = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_1    = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_1        = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_1     = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid_1         = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_1  = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_1 = ((cls_key_data.key_data0.bf3.cl_key_tre_l2rl3r_2_mask & 0x80) == 0x00);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}

ca_status_t aal_l2_cls_egr_l2rcmr_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l2rcmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_l2rcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;


    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_inner_vid =
            (cfg->inner_vlan_id_0 >> 6) & 0x3f;
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_vid =
            (cfg->inner_vlan_id_0) & 0x3f;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.mac_field_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mac_ctrl =
            cfg->mac_field_ctrl_0;
    }
    if (mask.s.mac_da_0) {
       cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da =
            ((cfg->mac_da_0[0] << 11) | (cfg->mac_da_0[1] << 3) | ((cfg->mac_da_0[2]
                    >> 5) & 0x7));
       cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da =
            (((cfg->mac_da_0[2] & 0x1f) << 24) | (cfg->mac_da_0[3] << 16) | (cfg->mac_da_0[4] << 8) | cfg->mac_da_0[5]);
    }
    if (mask.s.mac_sa_0) {
       cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_sa =
            (cfg->mac_sa_0[0] >> 5) & 0x7;
       cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa =
            (((cfg->mac_sa_0[0] & 0x1f) << 27) | (cfg->mac_sa_0[1] << 19) |
             (cfg->mac_sa_0[2] << 11) | (cfg->mac_sa_0[3] << 3) | ((cfg->mac_sa_0[4] >> 5) & 0x7));
       cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_mac_sa =
            ((cfg->mac_sa_0[4] & 0x1f) << 8) | (cfg->mac_sa_0[5]);
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_sp =  cfg->l4_src_port_0;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_dp  = (cfg->l4_dst_port_0 & 0xf);
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_l4_dp  = (cfg->l4_dst_port_0 >> 4) & 0xfff ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask &=   0x2;
        } else {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask |= 0x1;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_tag_num = cfg->vlan_number_0;
    }


    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask &= 0x1;
        } else {
           cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_1_prcd = cfg->precedence_0;
    }

    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.vlan_field_valid_1) {
        if (cfg->vlan_field_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa_1;
    }
    if (mask.s.ip_addr_1) {
       cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_ipv4_addr =
            (cfg->ip_addr_1 >> 28) & 0xf;
       cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ipv4_addr =
            cfg->ip_addr_1 & 0xfffffff;
    }
    if (mask.s.ip_addr_valid_1) {
        if (cfg->ip_addr_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa_1;
    }

    if (mask.s.mac_addr_1) {
       cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr =
            (cfg->mac_addr_1[0] << 12) | (cfg->mac_addr_1[1] << 4) | ((cfg->mac_addr_1[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr =
            ((cfg->mac_addr_1[2] & 0xf) << 24) | (cfg->mac_addr_1[3] << 16) | (cfg->mac_addr_1[4] << 8) | (cfg->mac_addr_1[5]);
    }

    if (mask.s.mac_addr_valid_1) {
        if (cfg->mac_addr_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.dot1p_valid_1) {
        if (cfg->dot1p_valid_1)
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ip_pt = ((cfg->ip_pt_1 >> 4) & 0x0f);
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_pt = (cfg->ip_pt_1 & 0x0f);
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_special_pkt_code = cfg->special_pkt_code_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask |= 0x080;
        }
    }


    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;


}


ca_status_t aal_l2_cls_egr_l2rcmr_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l2rcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid                =   cls_key_data.key_data12.bf.cl_key_valid                           ;
    cfg->vlan_field_ctrl_0    =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_vid_ctrl           ;
    cfg->top_vlan_id_0        =   cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_vid            ;
    cfg->top_is_svlan_0       =   cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_top_sc_ind         ;
    cfg->inner_vlan_id_0      = (cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_inner_vid << 6) |cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_vid      ;
    cfg->inner_is_svlan_0     =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_sc_ind       ;
    cfg->mac_field_ctrl_0     =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mac_ctrl           ;
    cfg->mac_da_0[0]          = (cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da) >> 11 & 0xff  ;
    cfg->mac_da_0[1]          = (cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da) >> 3 & 0xff  ;
    cfg->mac_da_0[2]          = ((cls_key_data.key_data11.bf4.cl_key_tre_l2rcmr_1_mac_da & 0x07) << 5) | ((cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 24) & 0x1f);
    cfg->mac_da_0[3]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 16) & 0xff    ;
    cfg->mac_da_0[4]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 8) & 0xff     ;
    cfg->mac_da_0[5]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_da >> 0) & 0xff     ;
    cfg->mac_sa_0[0]          = (cls_key_data.key_data10.bf4.cl_key_tre_l2rcmr_1_mac_sa << 5) | ((cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 27) & 0x1f);
    cfg->mac_sa_0[1]          = (cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 19) & 0xff;
    cfg->mac_sa_0[2]          = (cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 11) & 0xff;
    cfg->mac_sa_0[3]          = (cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa >>  3) & 0xff;
    cfg->mac_sa_0[4]          = (((cls_key_data.key_data9.bf4.cl_key_tre_l2rcmr_1_mac_sa & 0x7) << 5) | ((cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_mac_sa >> 8) & 0xff));
    cfg->mac_sa_0[5]          = (cls_key_data.key_data8.bf4.cl_key_tre_l2rcmr_1_mac_sa & 0xff);
    cfg->top_dot1p_0          =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_top_802_1p         ;
    cfg->inner_dot1p_0        =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_inner_802_1p       ;
    cfg->dot1p_ctrl_0         =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_0 =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_ctrl            ;
    cfg->l4_src_port_0        =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_sp              ;
    cfg->l4_dst_port_0        = (cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_l4_dp << 4) |cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_l4_dp              ;
    cfg->vlan_number_0        =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_tag_num            ;
    cfg->ethernet_type_0      =   cls_key_data.key_data7.bf4.cl_key_tre_l2rcmr_1_ev2_pt             ;
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask & 0x2)  == 0x00);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_mask & 0x1)  == 0x00);
    cfg->field_is_or_0        =   cls_key_data.key_data6.bf4.cl_key_tre_l2rcmr_1_fields_operator    ;
    cfg->precedence_0         =   cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_1_prcd               ;
    cfg->top_vlan_id_1          =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_top_vid;
    cfg->top_is_svlan_1         =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_sc_ind;
    cfg->ip_addr_is_sa_1        =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_ipv4_addr_sel;
    cfg->ip_addr_1              =cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ipv4_addr;
    cfg->mac_addr_is_sa_1       =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mac_addr_sel;
    cfg->mac_addr_1[0]          =cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 12 & 0xff;
    cfg->mac_addr_1[1]          =cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 4 & 0xff;
    cfg->mac_addr_1[2]          = (((cls_key_data.key_data5.bf4.cl_key_tre_l2rcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 24) & 0xf));
    cfg->mac_addr_1[3]          = (cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 16 & 0xff);
    cfg->mac_addr_1[4]          = (cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 8 & 0xff);
    cfg->mac_addr_1[5]          = (cls_key_data.key_data4.bf4.cl_key_tre_l2rcmr_2_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_1            =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_top_802_1p;
    cfg->l4_port_field_ctrl_1   =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_ctrl;
    cfg->l4_src_port_1          =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_l4_sp | (cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_sp << 11);
    cfg->l4_dst_port_1          =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_l4_dp;
    cfg->vlan_number_1          =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_tag_num;
    cfg->ethernet_type_1        =cls_key_data.key_data1.bf4.cl_key_tre_l2rcmr_2_ev2_pt;
    cfg->field_is_or_1          =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_fields_operator;
    cfg->precedence_1           =cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_prcd;
    cfg->ip_pt_1                =cls_key_data.key_data3.bf4.cl_key_tre_l2rcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_pt;
    cfg->dscp_1                 =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ip_tos;
    cfg->has_ipop_1             =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_option;
    cfg->l4_flag_1              =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_l4_flags;
    cfg->ipv4_fragment_1        =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_ipv4_fragment;
    cfg->special_pkt_code_1        =cls_key_data.key_data2.bf4.cl_key_tre_l2rcmr_2_special_pkt_code;
    cfg->dot1p_valid_1             = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid_1        = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid_1           = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid_1          = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_1  = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid_1           = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_1     = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid_1          = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid_1              = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid_1             = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld_1       = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld_1         = ((cls_key_data.key_data0.bf4.cl_key_tre_l2rcmr_2_mask & 0x001) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;
}

ca_status_t aal_l2_cls_egr_l3rl2r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3rl2r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3rl2r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;

  __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_inner_vid = cfg->inner_vlan_id_0 ;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.ip_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_ip_ctrl =
            cfg->ip_ctrl_0;
    }
    if (mask.s.ip_da_0) {
       cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ipv4_da = (cfg->ip_da_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_da = cfg->ip_da_0 & 0x7FFF;
    }
    if (mask.s.ip_sa_0) {
       cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_sa =
            (cfg->ip_sa_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_sa = cfg->ip_sa_0 & 0x7FFF;
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_pt = cfg->ip_pt_0;
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_1_prcd = cfg->precedence_0;
    }
    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1 >> 3) & 0x1ff;
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_vid =
            (cfg->inner_vlan_id_1) & 0x7;
    }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.mac_field_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mac_ctrl =
            cfg->mac_field_ctrl_1;
    }
    if (mask.s.mac_da_1) {
       cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da =
            ((cfg->mac_da_1[0] << 14) | (cfg->mac_da_1[1] << 6) | ((cfg->
                    mac_da_1[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da =
            (((cfg->mac_da_1[2] & 0x3) << 24) | (cfg->mac_da_1[3] << 16) |
             (cfg->mac_da_1[4] << 8) | cfg->mac_da_1[5]);
    }
    if (mask.s.mac_sa_1) {
       cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_sa =
            (cfg->mac_sa_1[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa =
            (((cfg->mac_sa_1[0] & 0x3) << 30) | (cfg->mac_sa_1[1] << 22) |
             (cfg->mac_sa_1[2] << 14) | (cfg->mac_sa_1[3] << 6) | ((cfg->
                     mac_sa_1[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_mac_sa =
            ((cfg->mac_sa_1[4] & 0x3) << 8) | (cfg->mac_sa_1[5]);

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_sp =  cfg->l4_src_port_1;
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_dp = (cfg->l4_dst_port_1 & 1);
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_l4_dp = (cfg->l4_dst_port_1 >> 1) & 0x7fff;
    }
    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask |= 0x01;
        }
    }
    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_prcd = cfg->precedence_1;
    }
    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_egr_l3rl2r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3rl2r_key_t *cfg)

{

    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->vlan_field_ctrl_0 =
       cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_vid_ctrl;
    cfg->top_vlan_id_0      =cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_vid;
    cfg->top_is_svlan_0     =cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_top_sc_ind;
    cfg->inner_vlan_id_0    = (cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_inner_vid);
    cfg->inner_is_svlan_0   =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_sc_ind;
    cfg->top_dot1p_0        =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_top_802_1p;
    cfg->inner_dot1p_0      =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_inner_802_1p;
    cfg->field_is_or_0      =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_fields_operator;
    cfg->l4_src_port_0      = cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_sp | (cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_sp <<14);
    cfg->l4_dst_port_0      = (cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_l4_dp);
    cfg->vlan_number_0      =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_tag_num;
    cfg->precedence_0       =cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_1_prcd;
    cfg->ethernet_type_0    =cls_key_data.key_data7.bf5.cl_key_tre_l3rl2r_1_ev2_pt;
    cfg->ip_ctrl_0          =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_ip_ctrl;
    cfg->dot1p_ctrl_0       =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_dot1p_ctrl;
    cfg->ip_da_0            =cls_key_data.key_data11.bf5.cl_key_tre_l3rl2r_1_ipv4_da <<15 |cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_da;
    cfg->ip_sa_0            =cls_key_data.key_data10.bf5.cl_key_tre_l3rl2r_1_ipv4_sa <<15 |cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_sa;
    cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_l4_ctrl;
    cfg->ipv4_fragment_0    =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_fragment;
    cfg->l4_flag_0          =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_l4_flags;
    cfg->has_ipop_0         =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ipv4_option;
    cfg->dscp_0             =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_tos;
    cfg->ip_pt_0            =cls_key_data.key_data9.bf5.cl_key_tre_l3rl2r_1_ip_pt;
    cfg->special_pkt_code_0 =cls_key_data.key_data8.bf5.cl_key_tre_l3rl2r_1_special_pkt_code;
    cfg->ipv4_fragment_valid_0   = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_0         = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_0       = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_0           = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_0        = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x10) == 0x00);
    cfg->dscp_valid_0            = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_0     = ((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_0 =((cls_key_data.key_data6.bf5.cl_key_tre_l3rl2r_1_mask & 0x80) == 0x00);

    cfg->mac_field_ctrl_1 =cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mac_ctrl;
    cfg->mac_da_1[0] =
        (cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da_1[1] =
        (cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da_1[2] =
        ((cls_key_data.key_data5.bf5.cl_key_tre_l3rl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da
                  >> 24) & 0x3);
    cfg->mac_da_1[3] =
        (cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da_1[4] =
        (cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da_1[5] =
        (cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa_1[0] =
        ((cls_key_data.key_data4.bf5.cl_key_tre_l3rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa
                  >> 30) & 0x3);
    cfg->mac_sa_1[1] =
        (cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa_1[2] =
        (cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa_1[3] =
        (cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa_1[4] =
        ((cls_key_data.key_data3.bf5.cl_key_tre_l3rl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_mac_sa
                  >> 8) & 0x3);
    cfg->mac_sa_1[5] =
        (cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_1 =cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_ctrl;
    cfg->precedence_1 =cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_prcd;
    cfg->field_is_or_1 = cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_fields_operator;
    cfg->l4_src_port_1 = cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_sp;
    cfg->l4_dst_port_1 = (cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_l4_dp | ((cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_l4_dp << 1) & 0xfffe));
    cfg->dot1p_ctrl_1 = cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_dot1p_ctrl;
    cfg->top_dot1p_1 =cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_top_802_1p;
    cfg->inner_dot1p_1 =cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_802_1p;
    cfg->ethernet_type_vld_1 = ((cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask & 0x2) == 0);
    cfg->ethernet_type_1 = cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_ev2_pt;
    cfg->vlan_field_ctrl_1 =
       cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_vid_ctrl;
    cfg->top_vlan_id_1 =cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_vid;
    cfg->top_is_svlan_1 =cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_top_sc_ind;
    cfg->inner_vlan_id_1 =
        (cls_key_data.key_data2.bf5.cl_key_tre_l3rl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_vid);
    cfg->inner_is_svlan_1 =
       cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_inner_sc_ind;
    cfg->vlan_number_vld_1 = ((cls_key_data.key_data0.bf5.cl_key_tre_l3rl2r_2_mask & 0x1) == 0);
    cfg->vlan_number_1 =cls_key_data.key_data1.bf5.cl_key_tre_l3rl2r_2_tag_num;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_egr_l3rl3r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3rl3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3rl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_inner_vid = cfg->inner_vlan_id_0 ;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.ip_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_ip_ctrl =
            cfg->ip_ctrl_0;
    }
    if (mask.s.ip_da_0) {
       cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ipv4_da = (cfg->ip_da_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_da = cfg->ip_da_0 & 0x7FFF;
    }
    if (mask.s.ip_sa_0) {
       cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_sa =
            (cfg->ip_sa_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_sa = cfg->ip_sa_0 & 0x7FFF;
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_pt = cfg->ip_pt_0;
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_1_prcd = cfg->precedence_0;
    }

    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid = cfg->inner_vlan_id_1;
            }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.ip_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl =
            cfg->ip_ctrl_1;
    }
    if (mask.s.ip_da_1) {
       cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            (cfg->ip_da_1 >> 12) & 0xfffff;
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da =
            cfg->ip_da_1 & 0xfff;
    }
    if (mask.s.ip_sa_1) {
       cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            (cfg->ip_sa_1 >> 12) & 0xfffff;
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa =
            cfg->ip_sa_1 & 0xfff;
    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p = (cfg->top_dot1p_1 & 1);
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p = ((cfg->top_dot1p_1 >> 1) & 3);
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt = cfg->ip_pt_1;
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 >> 4) & 1;
       cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code = (cfg->special_pkt_code_1 & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid_1) {
        if (cfg->special_pkt_code_valid_1) {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask |= 0x80;
        }
    }


    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_egr_l3rl3r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3rl3r_key_t *cfg)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
       return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->vlan_field_ctrl_0 =
      cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_vid_ctrl;
    cfg->top_vlan_id_0      =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_vid;
    cfg->top_is_svlan_0     =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_top_sc_ind;
    cfg->inner_vlan_id_0    = (cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_inner_vid);
    cfg->inner_is_svlan_0   =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_sc_ind;
    cfg->top_dot1p_0        =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_top_802_1p;
    cfg->inner_dot1p_0      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_inner_802_1p;
    cfg->field_is_or_0      =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_fields_operator;
    cfg->l4_src_port_0      = cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_sp | (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_sp <<14);
    cfg->l4_dst_port_0      = (cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_l4_dp);
    cfg->vlan_number_0      =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_tag_num;
    cfg->precedence_0       =cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_1_prcd;
    cfg->ethernet_type_0    =cls_key_data.key_data7.bf6.cl_key_tre_l3rl3r_1_ev2_pt;
    cfg->ip_ctrl_0          =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_ip_ctrl;
    cfg->dot1p_ctrl_0       =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_dot1p_ctrl;
    cfg->ip_da_0            =cls_key_data.key_data11.bf6.cl_key_tre_l3rl3r_1_ipv4_da <<15 |cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_da;
    cfg->ip_sa_0            =cls_key_data.key_data10.bf6.cl_key_tre_l3rl3r_1_ipv4_sa <<15 |cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_sa;
    cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_l4_ctrl;
    cfg->ipv4_fragment_0    =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_fragment;
    cfg->l4_flag_0          =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_l4_flags;
    cfg->has_ipop_0         =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ipv4_option;
    cfg->dscp_0             =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_tos;
    cfg->ip_pt_0            =cls_key_data.key_data9.bf6.cl_key_tre_l3rl3r_1_ip_pt;
    cfg->special_pkt_code_0 =cls_key_data.key_data8.bf6.cl_key_tre_l3rl3r_1_special_pkt_code;
    cfg->ipv4_fragment_valid_0   = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_0         = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_0       = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_0           = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_0        = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x10) == 0x00);
    cfg->dscp_valid_0            = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_0     = ((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_0 =((cls_key_data.key_data6.bf6.cl_key_tre_l3rl3r_1_mask & 0x80) == 0x00);


    cfg->vlan_field_ctrl_1    =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_vid_ctrl           ;
    cfg->top_vlan_id_1        =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_vid            ;
    cfg->top_is_svlan_1       =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id_1      =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_vid          ;
    cfg->inner_is_svlan_1     =           cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl_1            =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_ip_ctrl            ;
    cfg->ip_da_1              = (cls_key_data.key_data5.bf6.cl_key_tre_l3rl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_da)          ;
    cfg->ip_sa_1              = (cls_key_data.key_data4.bf6.cl_key_tre_l3rl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_sa)         ;
    cfg->top_dot1p_1          =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_top_802_1p    | (cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p_1        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl_1         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_1 =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_ctrl            ;
    cfg->l4_src_port_1        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_l4_sp | (cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port_1        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_l4_dp              ;
    cfg->vlan_number_1        =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_tag_num            ;
    cfg->ethernet_type_1      =           cls_key_data.key_data1.bf6.cl_key_tre_l3rl3r_2_ev2_pt             ;
    cfg->field_is_or_1        =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_fields_operator    ;
    cfg->precedence_1         =           cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_prcd               ;
    cfg->ip_pt_1              =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_pt              ;
    cfg->special_pkt_code_1   = (cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf6.cl_key_tre_l3rl3r_2_special_pkt_code    ;
    cfg->dscp_1               =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ip_tos             ;
    cfg->has_ipop_1           =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_option        ;
    cfg->l4_flag_1            =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_l4_flags           ;
    cfg->ipv4_fragment_1      =           cls_key_data.key_data3.bf6.cl_key_tre_l3rl3r_2_ipv4_fragment      ;
    cfg->ipv4_fragment_valid_1  = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_1        = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_1      = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_1          = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_1       = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid_1           = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_1    = ((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_1=((cls_key_data.key_data0.bf6.cl_key_tre_l3rl3r_2_mask & 0x80) == 0x00);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    return ret;

}


ca_status_t aal_l2_cls_egr_l3rcmr_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_l3rcmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_l3rcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_L3RULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;

    if (mask.s.vlan_field_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_vid_ctrl =
            cfg->vlan_field_ctrl_0;
    }
    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_vid = cfg->top_vlan_id_0;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.inner_vlan_id_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_inner_vid = cfg->inner_vlan_id_0 ;
    }
    if (mask.s.inner_is_svlan_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_sc_ind =
            cfg->inner_is_svlan_0;
    }
    if (mask.s.ip_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_ip_ctrl =
            cfg->ip_ctrl_0;
    }
    if (mask.s.ip_da_0) {
       cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ipv4_da = (cfg->ip_da_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_da = cfg->ip_da_0 & 0x7FFF;
    }
    if (mask.s.ip_sa_0) {
       cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_sa =
            (cfg->ip_sa_0 >> 15) & 0x1FFFF;
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_sa = cfg->ip_sa_0 & 0x7FFF;
    }
    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.inner_dot1p_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_802_1p = cfg->inner_dot1p_0;
    }
    if (mask.s.dot1p_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_dot1p_ctrl =  cfg->dot1p_ctrl_0;
    }
    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &=   0xFE;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x1;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xEF;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x10;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xF7;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x08;
        }
    }
    if (mask.s.ip_pt_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_pt = cfg->ip_pt_0;
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xFB;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x04;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xDF;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x20;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xBF;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x40;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0x7F;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x80;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask &= 0xFD;
        } else {
           cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_1_prcd = cfg->precedence_0;
    }

    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.vlan_field_valid_1) {
        if (cfg->vlan_field_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa_1;
    }
    if (mask.s.ip_addr_1) {
       cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_ipv4_addr =
            (cfg->ip_addr_1 >> 28) & 0xf;
       cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ipv4_addr =
            cfg->ip_addr_1 & 0xfffffff;
    }
    if (mask.s.ip_addr_valid_1) {
        if (cfg->ip_addr_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa_1;
    }

    if (mask.s.mac_addr_1) {
       cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr =
            (cfg->mac_addr_1[0] << 12) | (cfg->mac_addr_1[1] << 4) | ((cfg->mac_addr_1[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr =
            ((cfg->mac_addr_1[2] & 0xf) << 24) | (cfg->mac_addr_1[3] << 16) | (cfg->mac_addr_1[4] << 8) | (cfg->mac_addr_1[5]);
    }

    if (mask.s.mac_addr_valid_1) {
        if (cfg->mac_addr_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.dot1p_valid_1) {
        if (cfg->dot1p_valid_1)
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ip_pt = ((cfg->ip_pt_1 >> 4) & 0x0f);
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_pt = (cfg->ip_pt_1 & 0x0f);
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_special_pkt_code = cfg->special_pkt_code_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask |= 0x080;
        }
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}


ca_status_t aal_l2_cls_egr_l3rcmr_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_l3rcmr_key_t *cfg)

{

   ca_status_t ret = CA_E_OK;
   __aal_cls_key_data_t cls_key_data;


   if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
   }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

   cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
   cfg->vlan_field_ctrl_0 =
     cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_vid_ctrl;
   cfg->top_vlan_id_0      =cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_vid;
   cfg->top_is_svlan_0     =cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_top_sc_ind;
   cfg->inner_vlan_id_0    = (cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_inner_vid);
   cfg->inner_is_svlan_0   =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_sc_ind;
   cfg->top_dot1p_0        =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_top_802_1p;
   cfg->inner_dot1p_0      =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_inner_802_1p;
   cfg->field_is_or_0      =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_fields_operator;
   cfg->l4_src_port_0      = cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_sp | (cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_sp <<14);
   cfg->l4_dst_port_0      = (cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_l4_dp);
   cfg->vlan_number_0      =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_tag_num;
   cfg->precedence_0       =cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_1_prcd;
   cfg->ethernet_type_0    =cls_key_data.key_data7.bf7.cl_key_tre_l3rcmr_1_ev2_pt;
   cfg->ip_ctrl_0          =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_ip_ctrl;
   cfg->dot1p_ctrl_0       =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_dot1p_ctrl;
   cfg->ip_da_0            =cls_key_data.key_data11.bf7.cl_key_tre_l3rcmr_1_ipv4_da <<15 |cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_da;
   cfg->ip_sa_0            =cls_key_data.key_data10.bf7.cl_key_tre_l3rcmr_1_ipv4_sa <<15 |cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_sa;
   cfg->l4_port_field_ctrl_0 =cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_l4_ctrl;
   cfg->ipv4_fragment_0    =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_fragment;
   cfg->l4_flag_0          =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_l4_flags;
   cfg->has_ipop_0         =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ipv4_option;
   cfg->dscp_0             =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_tos;
   cfg->ip_pt_0            =cls_key_data.key_data9.bf7.cl_key_tre_l3rcmr_1_ip_pt;
   cfg->special_pkt_code_0 =cls_key_data.key_data8.bf7.cl_key_tre_l3rcmr_1_special_pkt_code;
   cfg->ipv4_fragment_valid_0   = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x20) == 0x00);
   cfg->l4_flag_valid_0         = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x40) == 0x00);
   cfg->vlan_number_vld_0       = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x01) == 0x00);
   cfg->ip_pt_valid_0           = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x04) == 0x00);
   cfg->has_ipop_valid_0        = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x10) == 0x00);
   cfg->dscp_valid_0            = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x08) == 0x00);
   cfg->ethernet_type_vld_0     = ((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x02) == 0x00);
   cfg->special_pkt_code_valid_0 =((cls_key_data.key_data6.bf7.cl_key_tre_l3rcmr_1_mask & 0x80) == 0x00);
   cfg->top_vlan_id_1          =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_top_vid;
   cfg->top_is_svlan_1         =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_sc_ind;
   cfg->ip_addr_is_sa_1        =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_ipv4_addr_sel;
   cfg->ip_addr_1              =cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ipv4_addr;
   cfg->mac_addr_is_sa_1       =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mac_addr_sel;
   cfg->mac_addr_1[0]          =cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 12 & 0xff;
   cfg->mac_addr_1[1]          =cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 4 & 0xff;
   cfg->mac_addr_1[2]          = (((cls_key_data.key_data5.bf7.cl_key_tre_l3rcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 24) & 0xf));
   cfg->mac_addr_1[3]          = (cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 16 & 0xff);
   cfg->mac_addr_1[4]          = (cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 8 & 0xff);
   cfg->mac_addr_1[5]          = (cls_key_data.key_data4.bf7.cl_key_tre_l3rcmr_2_mac_addr >> 0 & 0xff);
   cfg->top_dot1p_1            =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_top_802_1p;
   cfg->l4_port_field_ctrl_1   =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_ctrl;
   cfg->l4_src_port_1          =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_l4_sp | (cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_sp << 11);
   cfg->l4_dst_port_1          =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_l4_dp;
   cfg->vlan_number_1          =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_tag_num;
   cfg->ethernet_type_1        =cls_key_data.key_data1.bf7.cl_key_tre_l3rcmr_2_ev2_pt;
   cfg->field_is_or_1          =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_fields_operator;
   cfg->precedence_1           =cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_prcd;
   cfg->ip_pt_1                =cls_key_data.key_data3.bf7.cl_key_tre_l3rcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_pt;
   cfg->dscp_1                 =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ip_tos;
   cfg->has_ipop_1             =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_option;
   cfg->l4_flag_1              =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_l4_flags;
   cfg->ipv4_fragment_1        =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_ipv4_fragment;
   cfg->special_pkt_code_1        =cls_key_data.key_data2.bf7.cl_key_tre_l3rcmr_2_special_pkt_code;
    cfg->dot1p_valid_1             = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid_1        = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid_1           = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid_1          = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_1  = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid_1           = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_1     = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid_1          = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid_1              = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid_1             = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld_1       = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld_1         = ((cls_key_data.key_data0.bf7.cl_key_tre_l3rcmr_2_mask & 0x001) == 0);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

   return ret;
}

ca_status_t aal_l2_cls_egr_cmrl2r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmrl2r_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmrl2r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;



    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_top_vid = (cfg->top_vlan_id_0 >> 3) & 0x1ff;
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_vid = cfg->top_vlan_id_0 & 0x07;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.vlan_field_valid_0) {
        if (cfg->vlan_field_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xBFF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x400;
        }
    }

   if (mask.s.ip_addr_is_sa_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_ipv4_addr_sel =
            cfg->ip_addr_is_sa_0;
    }
    if (mask.s.ip_addr_0) {
       cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_ipv4_addr =
            (cfg->ip_addr_0 >> 31) & 0x01;
       cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ipv4_addr =
            cfg->ip_addr_0 & 0x7fffffff;
    }
    if (mask.s.ip_addr_valid_0) {
        if (cfg->ip_addr_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xDFF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mac_addr_sel =
            cfg->mac_addr_is_sa_0;
    }

    if (mask.s.mac_addr_0) {
       cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr =
            (cfg->mac_addr_0[0] << 9) | (cfg->mac_addr_0[1] << 1) | ((cfg->mac_addr_0[2] >> 7) & 0x01);
       cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr =
            ((cfg->mac_addr_0[2] & 0x7f) << 24) | (cfg->mac_addr_0[3] << 16) | (cfg->mac_addr_0[4] << 8) | (cfg->mac_addr_0[5]);
    }

    if (mask.s.mac_addr_valid_0) {
        if (cfg->mac_addr_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xEFF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x100;
        }

    }

    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.dot1p_valid_0) {
        if (cfg->dot1p_valid_0 ) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0x7FF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x800;
        }
    }

    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &=   0xFFE;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x001;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
        if (cfg->has_ipop_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFEF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x010;
        }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFF7;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x008;
        }
    }
    if (mask.s.ip_pt_0) {
        cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ip_pt = ((cfg->ip_pt_0 >>7) & 1);
        cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_pt = (cfg->ip_pt_0 &0x7f);
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFFB;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x004;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFDF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x020;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFBF;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x040;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xF7F;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x080;
        }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask &= 0xFFD;
        } else {
           cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
       cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_1_prcd = cfg->precedence_0;
    }
    if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_inner_vid =
            (cfg->inner_vlan_id_1 >> 3) & 0x1ff;
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_vid =
            (cfg->inner_vlan_id_1) & 0x7;
    }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.mac_field_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mac_ctrl =
            cfg->mac_field_ctrl_1;
    }
    if (mask.s.mac_da_1) {
       cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da =
            ((cfg->mac_da_1[0] << 14) | (cfg->mac_da_1[1] << 6) | ((cfg->
                    mac_da_1[2] >> 2) & 0x3f));
       cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da =
            (((cfg->mac_da_1[2] & 0x3) << 24) | (cfg->mac_da_1[3] << 16) |
             (cfg->mac_da_1[4] << 8) | cfg->mac_da_1[5]);
    }
    if (mask.s.mac_sa_1) {
       cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_sa =
            (cfg->mac_sa_1[0] >> 2) & 0x3f;
       cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa =
            (((cfg->mac_sa_1[0] & 0x3) << 30) | (cfg->mac_sa_1[1] << 22) |
             (cfg->mac_sa_1[2] << 14) | (cfg->mac_sa_1[3] << 6) | ((cfg->
                     mac_sa_1[4] >> 2) & 0x3f));
       cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_mac_sa =
            ((cfg->mac_sa_1[4] & 0x3) << 8) | (cfg->mac_sa_1[5]);

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_sp =  cfg->l4_src_port_1;
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_dp = (cfg->l4_dst_port_1 & 1);
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_l4_dp = (cfg->l4_dst_port_1 >> 1) & 0x7fff;
    }
    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask &= 0x02;
        } else {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask |= 0x01;
        }
    }
    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask &= 0x1;
        } else {
           cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_prcd = cfg->precedence_1;
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;



}




ca_status_t aal_l2_cls_egr_cmrl2r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmrl2r_key_t *cfg)

{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }
     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;

    cfg->top_vlan_id_0          =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_top_vid <<3 |cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_vid;
    cfg->top_is_svlan_0         =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_sc_ind;
    cfg->ip_addr_is_sa_0        =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_ipv4_addr_sel;
    cfg->ip_addr_0              =cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_ipv4_addr << 31 |cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ipv4_addr;
    cfg->mac_addr_is_sa_0       =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mac_addr_sel;
    cfg->mac_addr_0[0]          =cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr_0[1]          =cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr_0[2]          = (((cls_key_data.key_data11.bf8.cl_key_tre_cmrl2r_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr_0[3]          = (cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr_0[4]          = (cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr_0[5]          = (cls_key_data.key_data10.bf8.cl_key_tre_cmrl2r_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_0            =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_top_802_1p;
    cfg->l4_port_field_ctrl_0   =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_ctrl;
    cfg->l4_src_port_0          =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_l4_sp | (cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_sp << 14);
    cfg->l4_dst_port_0          =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_l4_dp;
    cfg->vlan_number_0          =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_tag_num;
    cfg->ethernet_type_0        =cls_key_data.key_data7.bf8.cl_key_tre_cmrl2r_1_ev2_pt;
    cfg->field_is_or_0          =cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_fields_operator;
    cfg->precedence_0           =cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_1_prcd;
    cfg->ip_pt_0                =cls_key_data.key_data9.bf8.cl_key_tre_cmrl2r_1_ip_pt << 7 |cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_pt;
    cfg->dscp_0                 =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ip_tos;
    cfg->has_ipop_0             =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_option;
    cfg->l4_flag_0              =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_l4_flags;
    cfg->ipv4_fragment_0        =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_ipv4_fragment;
    cfg->special_pkt_code_0        =cls_key_data.key_data8.bf8.cl_key_tre_cmrl2r_1_special_pkt_code;
    cfg->dot1p_valid_0             = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x800) == 0);
    cfg->vlan_field_valid_0        = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x400) == 0);
    cfg->ip_addr_valid_0           = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x200) == 0);
    cfg->mac_addr_valid_0          = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_0  = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x080) == 0);
    cfg->l4_flag_valid_0           = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_0     = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x020) == 0);
    cfg->has_ipop_valid_0          = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x010) == 0);
    cfg->dscp_valid_0              = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x008) == 0);
    cfg->ip_pt_valid_0             = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x002) == 0);
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf8.cl_key_tre_cmrl2r_1_mask & 0x001) == 0);

    cfg->mac_field_ctrl_1 =cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mac_ctrl;
    cfg->mac_da_1[0] =
        (cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da >> 14) & 0xff;
    cfg->mac_da_1[1] =
        (cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da >> 6) & 0xff;
    cfg->mac_da_1[2] =
        ((cls_key_data.key_data5.bf8.cl_key_tre_cmrl2r_2_mac_da & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da
                  >> 24) & 0x3);
    cfg->mac_da_1[3] =
        (cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da >> 16) & 0xff;
    cfg->mac_da_1[4] =
        (cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da >> 8) & 0xff;
    cfg->mac_da_1[5] =
        (cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_da >> 0) & 0xff;
    cfg->mac_sa_1[0] =
        ((cls_key_data.key_data4.bf8.cl_key_tre_cmrl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa
                  >> 30) & 0x3);
    cfg->mac_sa_1[1] =
        (cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 22) & 0xff;
    cfg->mac_sa_1[2] =
        (cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 14) & 0xff;
    cfg->mac_sa_1[3] =
        (cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 6) & 0xff;
    cfg->mac_sa_1[4] =
        ((cls_key_data.key_data3.bf8.cl_key_tre_cmrl2r_2_mac_sa & 0x3f) << 2 & 0xFC) | ((cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_mac_sa
                  >> 8) & 0x3);
    cfg->mac_sa_1[5] =
        (cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_mac_sa >> 0) & 0xff;

    cfg->l4_port_field_ctrl_1 =cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_ctrl;
    cfg->precedence_1 =cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_prcd;
    cfg->field_is_or_1 = cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_fields_operator;
    cfg->l4_src_port_1 = cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_sp;
    cfg->l4_dst_port_1 = (cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_l4_dp | ((cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_l4_dp << 1) & 0xfffe));
    cfg->dot1p_ctrl_1 = cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_dot1p_ctrl;
    cfg->top_dot1p_1 =cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_top_802_1p;
    cfg->inner_dot1p_1 =cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_802_1p;
    cfg->ethernet_type_vld_1 = ((cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask & 0x2) == 0);
    cfg->ethernet_type_1 = cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_ev2_pt;
    cfg->vlan_field_ctrl_1 =
       cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_vid_ctrl;
    cfg->top_vlan_id_1 =cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_vid;
    cfg->top_is_svlan_1 =cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_top_sc_ind;
    cfg->inner_vlan_id_1 =
        (cls_key_data.key_data2.bf8.cl_key_tre_cmrl2r_2_inner_vid << 3) | (cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_vid);
    cfg->inner_is_svlan_1 =
       cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_inner_sc_ind;
    cfg->vlan_number_vld_1 = ((cls_key_data.key_data0.bf8.cl_key_tre_cmrl2r_2_mask & 0x1) == 0);
    cfg->vlan_number_1 =cls_key_data.key_data1.bf8.cl_key_tre_cmrl2r_2_tag_num;

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;


}

ca_status_t aal_l2_cls_egr_cmrl3r_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmrl3r_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmrl3r_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;



    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

     if (mask.s.valid) {
        cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
     }

    cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

    cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;
    cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;



     if (mask.s.top_vlan_id_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_top_vid = (cfg->top_vlan_id_0 >> 3) & 0x1ff;
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_vid = cfg->top_vlan_id_0 & 0x07;
     }
     if (mask.s.top_is_svlan_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_sc_ind =
             cfg->top_is_svlan_0;
     }
     if (mask.s.vlan_field_valid_0) {
         if (cfg->vlan_field_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xBFF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x400;
         }
     }

    if (mask.s.ip_addr_is_sa_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_ipv4_addr_sel =
             cfg->ip_addr_is_sa_0;
     }
     if (mask.s.ip_addr_0) {
        cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_ipv4_addr =
             (cfg->ip_addr_0 >> 31) & 0x01;
        cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ipv4_addr =
             cfg->ip_addr_0 & 0x7fffffff;
     }
     if (mask.s.ip_addr_valid_0) {
         if (cfg->ip_addr_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xDFF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x200;
         }
     }
     if (mask.s.mac_addr_is_sa_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mac_addr_sel =
             cfg->mac_addr_is_sa_0;
     }

     if (mask.s.mac_addr_0) {
        cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr =
             (cfg->mac_addr_0[0] << 9) | (cfg->mac_addr_0[1] << 1) | ((cfg->mac_addr_0[2] >> 7) & 0x01);
        cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr =
             ((cfg->mac_addr_0[2] & 0x7f) << 24) | (cfg->mac_addr_0[3] << 16) | (cfg->mac_addr_0[4] << 8) | (cfg->mac_addr_0[5]);
     }

     if (mask.s.mac_addr_valid_0) {
         if (cfg->mac_addr_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xEFF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x100;
         }

     }

     if (mask.s.top_dot1p_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_802_1p = cfg->top_dot1p_0;
     }

     if (mask.s.dot1p_valid_0) {
         if (cfg->dot1p_valid_0 ) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0x7FF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x800;
         }
     }

     if (mask.s.l4_port_field_ctrl_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
     }
     if (mask.s.l4_src_port_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
     }
     if (mask.s.l4_dst_port_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_dp  = cfg->l4_dst_port_0 ;
     }

     if (mask.s.vlan_number_vld_0) {
         if (cfg->vlan_number_vld_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &=   0xFFE;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x001;
         }
     }

     if (mask.s.has_ipop_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_option = cfg->has_ipop_0;
     }
     if (mask.s.has_ipop_valid_0) {
         if (cfg->has_ipop_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFEF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x010;
         }
     }
     if (mask.s.dscp_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_tos = cfg->dscp_0;
     }
     if (mask.s.dscp_valid_0) {
         if (cfg->dscp_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFF7;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x008;
         }
     }
     if (mask.s.ip_pt_0) {
         cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ip_pt = ((cfg->ip_pt_0 >>7) &1);
         cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_pt = (cfg->ip_pt_0 &0x7f);
     }
     if (mask.s.ip_pt_valid_0) {
         if (cfg->ip_pt_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFFB;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x004;
         }
     }
     if (mask.s.ipv4_fragment_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_fragment = cfg->ipv4_fragment_0;
     }
     if (mask.s.ipv4_fragment_valid_0) {
         if (cfg->ipv4_fragment_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFDF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x020;
         }
     }
     if (mask.s.l4_flag_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_l4_flags = cfg->l4_flag_0;
     }
     if (mask.s.l4_flag_valid_0) {
         if (cfg->l4_flag_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFBF;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x040;
         }
     }
     if (mask.s.special_pkt_code_0) {
        cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_special_pkt_code = cfg->special_pkt_code_0;
     }
     if (mask.s.special_pkt_code_valid_0) {
         if (cfg->special_pkt_code_valid_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xF7F;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x080;
         }
     }
     if (mask.s.vlan_number_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_tag_num = cfg->vlan_number_0;
     }
     if (mask.s.ethernet_type_vld_0) {
         if (cfg->ethernet_type_vld_0) {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask &= 0xFFD;
         } else {
            cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask |= 0x002;
         }
     }
     if (mask.s.ethernet_type_0) {
        cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_ev2_pt = cfg->ethernet_type_0;
     }
     if (mask.s.field_is_or_0) {
        cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_fields_operator = cfg->field_is_or_0;
     }
     if (mask.s.precedence_0) {
        cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_1_prcd = cfg->precedence_0;
     }


       if (mask.s.vlan_field_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_vid_ctrl =
            cfg->vlan_field_ctrl_1;
    }
    if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.inner_vlan_id_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_vid = cfg->inner_vlan_id_1;
            }
    if (mask.s.inner_is_svlan_1) {
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_sc_ind =
            cfg->inner_is_svlan_1;
    }
    if (mask.s.ip_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_ip_ctrl =
            cfg->ip_ctrl_1;
    }
    if (mask.s.ip_da_1) {
       cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ipv4_da =
            (cfg->ip_da_1 >> 12) & 0xfffff;
       cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_da =
            cfg->ip_da_1 & 0xfff;
    }
    if (mask.s.ip_sa_1) {
       cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_sa =
            (cfg->ip_sa_1 >> 12) & 0xfffff;
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_sa =
            cfg->ip_sa_1 & 0xfff;
    }

    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_top_802_1p = (cfg->top_dot1p_1 & 1);
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_802_1p = ((cfg->top_dot1p_1 >> 1) & 3);
    }

    if (mask.s.inner_dot1p_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_inner_802_1p = cfg->inner_dot1p_1;

    }
    if (mask.s.dot1p_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_dot1p_ctrl = cfg->dot1p_ctrl_1;
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xFE;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x01;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xFD;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x2;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_pt = cfg->ip_pt_1;
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xFB;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x4;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xF7;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x8;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xEF;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x10;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xBF;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x40;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0xDF;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x20;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_special_pkt_code = (cfg->special_pkt_code_1 >> 4) & 1;
       cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_special_pkt_code = (cfg->special_pkt_code_1 & 0x0f) ;
    }

    if (mask.s.special_pkt_code_valid_1) {
        if (cfg->special_pkt_code_valid_1) {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask &= 0x7F;
        } else {
           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask |= 0x80;
        }
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     return ret;

}

ca_status_t aal_l2_cls_egr_cmrl3r_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmrl3r_key_t *cfg)

{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->top_vlan_id_0          =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_top_vid <<3 |cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_vid;
    cfg->top_is_svlan_0         =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_sc_ind;
    cfg->ip_addr_is_sa_0        =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_ipv4_addr_sel;
    cfg->ip_addr_0              =cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_ipv4_addr << 31 |cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ipv4_addr;
    cfg->mac_addr_is_sa_0       =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mac_addr_sel;
    cfg->mac_addr_0[0]          =cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr_0[1]          =cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr_0[2]          = (((cls_key_data.key_data11.bf9.cl_key_tre_cmrl3r_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr_0[3]          = (cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr_0[4]          = (cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr_0[5]          = (cls_key_data.key_data10.bf9.cl_key_tre_cmrl3r_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_0            =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_top_802_1p;
    cfg->l4_port_field_ctrl_0   =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_ctrl;
    cfg->l4_src_port_0          =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_l4_sp | (cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_sp << 14);
    cfg->l4_dst_port_0          =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_l4_dp;
    cfg->vlan_number_0          =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_tag_num;
    cfg->ethernet_type_0        =cls_key_data.key_data7.bf9.cl_key_tre_cmrl3r_1_ev2_pt;
    cfg->field_is_or_0          =cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_fields_operator;
    cfg->precedence_0           =cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_1_prcd;
    cfg->ip_pt_0                =cls_key_data.key_data9.bf9.cl_key_tre_cmrl3r_1_ip_pt << 7 |cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_pt;
    cfg->dscp_0                 =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ip_tos;
    cfg->has_ipop_0             =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_option;
    cfg->l4_flag_0              =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_l4_flags;
    cfg->ipv4_fragment_0        =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_ipv4_fragment;
    cfg->special_pkt_code_0        =cls_key_data.key_data8.bf9.cl_key_tre_cmrl3r_1_special_pkt_code;
    cfg->dot1p_valid_0             = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x800) == 0);
    cfg->vlan_field_valid_0        = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x400) == 0);
    cfg->ip_addr_valid_0           = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x200) == 0);
    cfg->mac_addr_valid_0          = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_0  = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x080) == 0);
    cfg->l4_flag_valid_0           = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_0     = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x020) == 0);
    cfg->has_ipop_valid_0          = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x010) == 0);
    cfg->dscp_valid_0              = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x008) == 0);
    cfg->ip_pt_valid_0             = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x002) == 0);
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf9.cl_key_tre_cmrl3r_1_mask & 0x001) == 0);

    cfg->vlan_field_ctrl_1    =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_vid_ctrl           ;
    cfg->top_vlan_id_1        =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_vid            ;
    cfg->top_is_svlan_1       =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_sc_ind         ;
    cfg->inner_vlan_id_1      =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_vid          ;
    cfg->inner_is_svlan_1     =           cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_inner_sc_ind       ;
    cfg->ip_ctrl_1            =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_ip_ctrl            ;
    cfg->ip_da_1              = (cls_key_data.key_data5.bf9.cl_key_tre_cmrl3r_2_ipv4_da << 12) | (cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_da)          ;
    cfg->ip_sa_1              = (cls_key_data.key_data4.bf9.cl_key_tre_cmrl3r_2_ipv4_sa  << 12) | (cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_sa)         ;
    cfg->top_dot1p_1          =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_top_802_1p    | (cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_top_802_1p << 1)     ;
    cfg->inner_dot1p_1        =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_inner_802_1p       ;
    cfg->dot1p_ctrl_1         =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_dot1p_ctrl         ;
    cfg->l4_port_field_ctrl_1 =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_ctrl            ;
    cfg->l4_src_port_1        =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_l4_sp | (cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_sp << 5)            ;
    cfg->l4_dst_port_1        =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_l4_dp              ;
    cfg->vlan_number_1        =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_tag_num            ;
    cfg->ethernet_type_1      =           cls_key_data.key_data1.bf9.cl_key_tre_cmrl3r_2_ev2_pt             ;
    cfg->field_is_or_1        =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_fields_operator    ;
    cfg->precedence_1         =           cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_prcd               ;
    cfg->ip_pt_1              =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_pt              ;
    cfg->special_pkt_code_1   = (cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_special_pkt_code  << 4) | cls_key_data.key_data2.bf9.cl_key_tre_cmrl3r_2_special_pkt_code    ;
    cfg->dscp_1               =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ip_tos             ;
    cfg->has_ipop_1           =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_option        ;
    cfg->l4_flag_1            =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_l4_flags           ;
    cfg->ipv4_fragment_1      =           cls_key_data.key_data3.bf9.cl_key_tre_cmrl3r_2_ipv4_fragment      ;
    cfg->ipv4_fragment_valid_1    = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x20) == 0x00);
    cfg->l4_flag_valid_1          = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x40) == 0x00);
    cfg->vlan_number_vld_1        = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x01) == 0x00);
    cfg->ip_pt_valid_1            = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x04) == 0x00);
    cfg->has_ipop_valid_1         = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x10) == 0x00);
    cfg->dscp_valid_1             = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x08) == 0x00);
    cfg->ethernet_type_vld_1      = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x02) == 0x00);
    cfg->special_pkt_code_valid_1 = ((cls_key_data.key_data0.bf9.cl_key_tre_cmrl3r_2_mask & 0x80) == 0x00);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_egr_cmrcmr_key_set(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_IN aal_l2_cls_cmrcmr_key_mask_t mask,
                                      CA_IN aal_l2_cls_cmrcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;



    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
     return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    if (mask.s.valid) {
       cls_key_data.key_data12.bf.cl_key_valid = cfg->valid;
    }

   cls_key_data.key_data11.bf0.cl_key_entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;

   cls_key_data.key_data11.bf1.cl_key_tre_rule_hinge = FALSE;

   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = AAL_L2_CLS_KEY_2RULE_TYPE_COMBRULE;
   cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = AAL_L2_CLS_KEY_2RULE_TYPE_L2RULE;



    if (mask.s.top_vlan_id_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid = (cfg->top_vlan_id_0 >> 3) & 0x1ff;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid = cfg->top_vlan_id_0 & 0x07;
    }
    if (mask.s.top_is_svlan_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind =
            cfg->top_is_svlan_0;
    }
    if (mask.s.vlan_field_valid_0) {
     if (cfg->vlan_field_valid_0) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xBFF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x400;
     }
    }

    if (mask.s.ip_addr_is_sa_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel =
            cfg->ip_addr_is_sa_0;
    }
    if (mask.s.ip_addr_0) {
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            (cfg->ip_addr_0 >> 31) & 0x01;
       cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr =
            cfg->ip_addr_0 & 0x7fffffff;
    }
    if (mask.s.ip_addr_valid_0) {
        if (cfg->ip_addr_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xDFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel =
            cfg->mac_addr_is_sa_0;
    }

    if (mask.s.mac_addr_0) {
       cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            (cfg->mac_addr_0[0] << 9) | (cfg->mac_addr_0[1] << 1) | ((cfg->mac_addr_0[2] >> 7) & 0x01);
       cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr =
            ((cfg->mac_addr_0[2] & 0x7f) << 24) | (cfg->mac_addr_0[3] << 16) | (cfg->mac_addr_0[4] << 8) | (cfg->mac_addr_0[5]);
    }

    if (mask.s.mac_addr_valid_0) {
        if (cfg->mac_addr_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xEFF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x100;
        }
    }

    if (mask.s.top_dot1p_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p = cfg->top_dot1p_0;
    }

    if (mask.s.dot1p_valid_0) {
        if (cfg->dot1p_valid_0 ) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0x7FF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x800;
        }
    }

    if (mask.s.l4_port_field_ctrl_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl  = cfg->l4_port_field_ctrl_0;
    }
    if (mask.s.l4_src_port_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp =  cfg->l4_src_port_0 & 0x3FFF;
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp  = (cfg->l4_src_port_0 >> 14) & 0x03;
    }
    if (mask.s.l4_dst_port_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp  = cfg->l4_dst_port_0 ;
    }

    if (mask.s.vlan_number_vld_0) {
        if (cfg->vlan_number_vld_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &=   0xFFE;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x001;
        }
    }

    if (mask.s.has_ipop_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option = cfg->has_ipop_0;
    }
    if (mask.s.has_ipop_valid_0) {
     if (cfg->has_ipop_valid_0) {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFEF;
     } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x010;
     }
    }
    if (mask.s.dscp_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos = cfg->dscp_0;
    }
    if (mask.s.dscp_valid_0) {
        if (cfg->dscp_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFF7;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x008;
        }
    }
    if (mask.s.ip_pt_0) {
        cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt_0 >> 7) &0x1;
        cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt = (cfg->ip_pt_0&0x7f);
    }
    if (mask.s.ip_pt_valid_0) {
        if (cfg->ip_pt_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFB;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x004;
        }
    }
    if (mask.s.ipv4_fragment_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment = cfg->ipv4_fragment_0;
    }
    if (mask.s.ipv4_fragment_valid_0) {
        if (cfg->ipv4_fragment_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFDF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x020;
        }
    }
    if (mask.s.l4_flag_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags = cfg->l4_flag_0;
    }
    if (mask.s.l4_flag_valid_0) {
        if (cfg->l4_flag_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFBF;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x040;
        }
    }
    if (mask.s.special_pkt_code_0) {
       cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code = cfg->special_pkt_code_0;
    }
    if (mask.s.special_pkt_code_valid_0) {
        if (cfg->special_pkt_code_valid_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xF7F;
        } else {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x080;
     }
    }
    if (mask.s.vlan_number_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num = cfg->vlan_number_0;
    }
    if (mask.s.ethernet_type_vld_0) {
        if (cfg->ethernet_type_vld_0) {
           cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask &= 0xFFD;
        } else {
        cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask |= 0x002;
     }
    }
    if (mask.s.ethernet_type_0) {
       cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt = cfg->ethernet_type_0;
    }
    if (mask.s.field_is_or_0) {
       cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator = cfg->field_is_or_0;
    }
    if (mask.s.precedence_0) {
        cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd = cfg->precedence_0;
    }


     if (mask.s.top_vlan_id_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid = cfg->top_vlan_id_1;
    }
    if (mask.s.top_is_svlan_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind =
            cfg->top_is_svlan_1;
    }
    if (mask.s.vlan_field_valid_1) {
        if (cfg->vlan_field_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xBFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x400;
        }
    }
    if (mask.s.ip_addr_is_sa_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel =
            cfg->ip_addr_is_sa_1;
    }
    if (mask.s.ip_addr_1) {
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            (cfg->ip_addr_1 >> 28) & 0xf;
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr =
            cfg->ip_addr_1 & 0xfffffff;
    }
    if (mask.s.ip_addr_valid_1) {
        if (cfg->ip_addr_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xDFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x200;
        }
    }
    if (mask.s.mac_addr_is_sa_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel =
            cfg->mac_addr_is_sa_1;
    }

    if (mask.s.mac_addr_1) {
       cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            (cfg->mac_addr_1[0] << 12) | (cfg->mac_addr_1[1] << 4) | ((cfg->mac_addr_1[2] >> 4) & 0x0f);
       cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr =
            ((cfg->mac_addr_1[2] & 0xf) << 24) | (cfg->mac_addr_1[3] << 16) | (cfg->mac_addr_1[4] << 8) | (cfg->mac_addr_1[5]);
    }

    if (mask.s.mac_addr_valid_1) {
        if (cfg->mac_addr_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xEFF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x100;
        }

    }
    if (mask.s.top_dot1p_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p = cfg->top_dot1p_1;
    }

    if (mask.s.dot1p_valid_1) {
        if (cfg->dot1p_valid_1)
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0x7ff;
        else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x800;
        }
    }
    if (mask.s.l4_port_field_ctrl_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl = cfg->l4_port_field_ctrl_1 ;
    }
    if (mask.s.l4_src_port_1) {

       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp = (cfg->l4_src_port_1 & 0x7ff);
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp = ((cfg->l4_src_port_1 >> 11) & 0x1f);
    }
    if (mask.s.l4_dst_port_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp = cfg->l4_dst_port_1;
    }


    if (mask.s.vlan_number_vld_1) {
        if (cfg->vlan_number_vld_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFE;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x001;
        }
    }

    if (mask.s.vlan_number_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num = cfg->vlan_number_1;
    }

    if (mask.s.ethernet_type_vld_1) {
        if (cfg->ethernet_type_vld_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFD;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x002;
        }
    }
    if (mask.s.ethernet_type_1) {
       cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt = cfg->ethernet_type_1;
    }
    if (mask.s.field_is_or_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator = cfg->field_is_or_1;
    }
    if (mask.s.precedence_1) {
       cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd = cfg->precedence_1;
    }

    if (mask.s.ip_pt_1) {
       cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt = ((cfg->ip_pt_1 >> 4) & 0x0f);
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt = (cfg->ip_pt_1 & 0x0f);
    }

    if (mask.s.ip_pt_valid_1) {
        if (cfg->ip_pt_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFFB;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x004;
        }
    }
    if (mask.s.dscp_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos = cfg->dscp_1;
    }

    if (mask.s.dscp_valid_1) {
        if (cfg->dscp_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFF7;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x008;
        }
    }

    if (mask.s.has_ipop_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option = cfg->has_ipop_1;
    }

    if (mask.s.has_ipop_valid_1) {
        if (cfg->has_ipop_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFEF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x010;
        }
    }

    if (mask.s.l4_flag_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags = cfg->l4_flag_1;
    }

    if (mask.s.l4_flag_valid_1) {
        if (cfg->l4_flag_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFBF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x040;
        }
    }

    if (mask.s.ipv4_fragment_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment = cfg->ipv4_fragment_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xFDF;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x020;
        }
    }
    if (mask.s.special_pkt_code_1) {
       cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code = cfg->special_pkt_code_1;
    }

    if (mask.s.ipv4_fragment_valid_1) {
        if (cfg->ipv4_fragment_valid_1) {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask &= 0xF7F;
        } else {
           cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask |= 0x080;
        }
    }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}



ca_status_t aal_l2_cls_egr_cmrcmr_key_get(CA_IN ca_device_id_t device_id,
                                      CA_IN ca_uint32_t entry_id,
                                      CA_OUT aal_l2_cls_cmrcmr_key_t *cfg)
{
    ca_status_t ret = CA_E_OK;
    __aal_cls_key_data_t cls_key_data;


    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == cfg)) {
      return CA_E_PARAM;
    }

    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);


    cfg->valid =cls_key_data.key_data12.bf.cl_key_valid;
    cfg->top_vlan_id_0          =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_top_vid <<3 |cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_vid;
    cfg->top_is_svlan_0         =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_sc_ind;
    cfg->ip_addr_is_sa_0        =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_ipv4_addr_sel;
    cfg->ip_addr_0              =cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_ipv4_addr << 31 |cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ipv4_addr;
    cfg->mac_addr_is_sa_0       =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mac_addr_sel;
    cfg->mac_addr_0[0]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 9 & 0xff;
    cfg->mac_addr_0[1]          =cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 1 & 0xff;
    cfg->mac_addr_0[2]          = (((cls_key_data.key_data11.bf10.cl_key_tre_cmrcmr_1_mac_addr & 0x1) << 7) | ((cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 24) & 0x7f));
    cfg->mac_addr_0[3]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 16 & 0xff);
    cfg->mac_addr_0[4]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 8 & 0xff);
    cfg->mac_addr_0[5]          = (cls_key_data.key_data10.bf10.cl_key_tre_cmrcmr_1_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_0            =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_top_802_1p;
    cfg->l4_port_field_ctrl_0   =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_ctrl;
    cfg->l4_src_port_0          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_l4_sp | (cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_sp << 14);
    cfg->l4_dst_port_0          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_l4_dp;
    cfg->vlan_number_0          =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_tag_num;
    cfg->ethernet_type_0        =cls_key_data.key_data7.bf10.cl_key_tre_cmrcmr_1_ev2_pt;
    cfg->field_is_or_0          =cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_fields_operator;
    cfg->precedence_0           =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_1_prcd;
    cfg->ip_pt_0                =cls_key_data.key_data9.bf10.cl_key_tre_cmrcmr_1_ip_pt << 7 |cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_pt;
    cfg->dscp_0                 =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ip_tos;
    cfg->has_ipop_0             =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_option;
    cfg->l4_flag_0              =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_l4_flags;
    cfg->ipv4_fragment_0        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_ipv4_fragment;
    cfg->special_pkt_code_0        =cls_key_data.key_data8.bf10.cl_key_tre_cmrcmr_1_special_pkt_code;
    cfg->dot1p_valid_0             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x800) == 0);
    cfg->vlan_field_valid_0        = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x400) == 0);
    cfg->ip_addr_valid_0           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x200) == 0);
    cfg->mac_addr_valid_0          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_0  = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x080) == 0);
    cfg->l4_flag_valid_0           = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_0     = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x020) == 0);
    cfg->has_ipop_valid_0          = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x010) == 0);
    cfg->dscp_valid_0              = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x008) == 0);
    cfg->ip_pt_valid_0             = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x004) == 0);
    cfg->ethernet_type_vld_0       = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x002) == 0);
    cfg->vlan_number_vld_0         = ((cls_key_data.key_data6.bf10.cl_key_tre_cmrcmr_1_mask & 0x001) == 0);


    cfg->top_vlan_id_1          =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_top_vid;
    cfg->top_is_svlan_1         =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_sc_ind;
    cfg->ip_addr_is_sa_1        =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_ipv4_addr_sel;
    cfg->ip_addr_1              =cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_ipv4_addr << 28 |cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ipv4_addr;
    cfg->mac_addr_is_sa_1       =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mac_addr_sel;
    cfg->mac_addr_1[0]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 12 & 0xff;
    cfg->mac_addr_1[1]          =cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 4 & 0xff;
    cfg->mac_addr_1[2]          = (((cls_key_data.key_data5.bf10.cl_key_tre_cmrcmr_2_mac_addr & 0xf) << 4) | ((cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 24) & 0xf));
    cfg->mac_addr_1[3]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 16 & 0xff);
    cfg->mac_addr_1[4]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 8 & 0xff);
    cfg->mac_addr_1[5]          = (cls_key_data.key_data4.bf10.cl_key_tre_cmrcmr_2_mac_addr >> 0 & 0xff);
    cfg->top_dot1p_1            =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_top_802_1p;
    cfg->l4_port_field_ctrl_1   =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_ctrl;
    cfg->l4_src_port_1          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_l4_sp | (cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_sp << 11);
    cfg->l4_dst_port_1          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_l4_dp;
    cfg->vlan_number_1          =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_tag_num;
    cfg->ethernet_type_1        =cls_key_data.key_data1.bf10.cl_key_tre_cmrcmr_2_ev2_pt;
    cfg->field_is_or_1          =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_fields_operator;
    cfg->precedence_1           =cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_prcd;
    cfg->ip_pt_1                =cls_key_data.key_data3.bf10.cl_key_tre_cmrcmr_2_ip_pt << 4 |cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_pt;
    cfg->dscp_1                 =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ip_tos;
    cfg->has_ipop_1             =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_option;
    cfg->l4_flag_1              =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_l4_flags;
    cfg->ipv4_fragment_1        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_ipv4_fragment;
    cfg->special_pkt_code_1        =cls_key_data.key_data2.bf10.cl_key_tre_cmrcmr_2_special_pkt_code;
    cfg->dot1p_valid_1             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x800) == 0);
    cfg->vlan_field_valid_1        = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x400) == 0);
    cfg->ip_addr_valid_1           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x200) == 0);
    cfg->mac_addr_valid_1          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x100) == 0);
    cfg->special_pkt_code_valid_1  = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x080) == 0);
    cfg->l4_flag_valid_1           = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x040) == 0);
    cfg->ipv4_fragment_valid_1     = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x020) == 0);
    cfg->has_ipop_valid_1          = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x010) == 0);
    cfg->dscp_valid_1              = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x008) == 0);
    cfg->ip_pt_valid_1             = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x004) == 0);
    cfg->ethernet_type_vld_1       = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x002) == 0);
    cfg->vlan_number_vld_1         = ((cls_key_data.key_data0.bf10.cl_key_tre_cmrcmr_2_mask & 0x001) == 0);


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t
aal_l2_cls_egr_fib_set(CA_IN ca_device_id_t device_id,
                       CA_IN ca_uint32_t fib_id,
                       CA_IN aal_l2_cls_fib_mask_t mask,
                       CA_IN aal_l2_cls_fib_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    L2FE_CLE_EGR_FIB_DATA0_t fib_data0;
    L2FE_CLE_EGR_FIB_DATA1_t fib_data1;
    L2FE_CLE_EGR_FIB_DATA2_t fib_data2;
    L2FE_CLE_EGR_FIB_DATA3_t fib_data3;
    L2FE_CLE_EGR_FIB_DATA4_t fib_data4;

    fib_data0.wrd = 0;
    fib_data1.wrd = 0;
    fib_data2.wrd = 0;
    fib_data3.wrd = 0;
    fib_data4.wrd = 0;

    if ((fib_id > __AAL_L2_CLS_MAX_FIB_EGR_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_EGR_FIB_ACCESS);
        fib_data4.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA4));
        fib_data3.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA3));
        fib_data2.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA2));
        fib_data1.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA1));
        fib_data0.wrd = CA_NE_REG_READ(AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA0));
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_EGR_FIB_ACCESS);
        fib_data4.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA4);
        fib_data3.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA3);
        fib_data2.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA2);
        fib_data1.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA1);
        fib_data0.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA0);
#endif
    }

    if (mask.s.permit_0) {
        fib_data2.bf.cl_fib_xr_permit_0 = cfg->permit_0;
    }
    if (mask.s.permit_valid_0) {
        fib_data2.bf.cl_fib_xr_permit_valid_0 = cfg->permit_valid_0;
    }
    if (mask.s.top_vlan_cmd_0) {
        fib_data4.bf.cl_fib_xr_top_vlan_cmd_0 = cfg->top_vlan_cmd_0;
    }
    if (mask.s.top_cmd_valid_0) {
        fib_data4.bf.cl_fib_xr_top_cmd_valid_0 = cfg->top_cmd_valid_0;
    }
    if (mask.s.top_vlanid_0) {
        fib_data4.bf.cl_fib_xr_top_vid_0 = cfg->top_vlanid_0;
    }
    if (mask.s.top_sc_ind_0) {
        fib_data4.bf.cl_fib_xr_top_sc_ind_0 = cfg->top_sc_ind_0;
    }

    if (mask.s.inner_vlan_cmd_0) {
        fib_data3.bf.cl_fib_xr_inner_vlan_cmd_0 = cfg->inner_vlan_cmd_0;
    }
    if (mask.s.inner_cmd_valid_0) {
        fib_data3.bf.cl_fib_xr_inner_cmd_valid_0 = cfg->inner_cmd_valid_0;
    }
    if (mask.s.inner_vlanid_0) {
        fib_data4.bf.cl_fib_xr_inner_vid_0 = (cfg->inner_vlanid_0 >> 9) & 0x07;
        fib_data3.bf.cl_fib_xr_inner_vid_0 = cfg->inner_vlanid_0 & 0x1ff;
    }
    if (mask.s.inner_sc_ind_0) {
        fib_data3.bf.cl_fib_xr_inner_sc_ind_0 = cfg->inner_sc_ind_0;
    }

    if (mask.s.dot1p_valid_0) {
        fib_data3.bf.cl_fib_xr_802_1p_valid_0 = cfg->dot1p_valid_0;
    }
    if (mask.s.dot1p_0) {
        fib_data3.bf.cl_fib_xr_802_1p_0 = cfg->dot1p_0;
    }

    if (mask.s.dscp_valid_0) {
        fib_data3.bf.cl_fib_xr_dscp_valid_0 = cfg->dscp_valid_0;
    }

    if (mask.s.dscp_0) {
        fib_data3.bf.cl_fib_xr_dscp_0 = cfg->dscp_0;
    }

    if (mask.s.cos_valid_0) {
        fib_data3.bf.cl_fib_xr_cos_valid_0 = cfg->cos_valid_0;
    }
    if (mask.s.cos_0) {
        fib_data3.bf.cl_fib_xr_cos_0 = cfg->cos_0;
    }

    if (mask.s.mark_ena_0) {
        fib_data2.bf.cl_fib_xr_mark_en_0 = cfg->mark_ena_0;
    }
    if (mask.s.flowid_0) {
        fib_data2.bf.cl_fib_xr_flowid_0 = cfg->flowid_0;
    }

    if (mask.s.dp_valid_0) {
        fib_data2.bf.cl_fib_xr_dp_valid_0 = cfg->dp_valid_0;
    }

    if (mask.s.dp_value_0) {
        fib_data2.bf.cl_fib_xr_ldpid_0 = cfg->dp_value_0 & 0x03;
        fib_data3.bf.cl_fib_xr_ldpid_0 = (cfg->dp_value_0 >> 2) & 0x0f;
    }

    if (mask.s.premarked_0) {
        fib_data2.bf.cl_fib_xr_premarked_0 = cfg->premarked_0;
    }
    if (mask.s.dscp_mark_down_0) {
        fib_data2.bf.cl_fib_xr_dscp_markdown_0 = cfg->dscp_mark_down_0;
    }

    if (mask.s.spt_mode_0) {
        fib_data2.bf.cl_fib_xr_stp_mode_0 = cfg->spt_mode_0;
    }

    if (mask.s.no_drop_0) {
        fib_data4.bf.cl_fib_xr_pkt_no_drop_0 = cfg->no_drop_0;
    }

    if (mask.s.permit_1) {
        fib_data0.bf.cl_fib_xr_permit_1 = cfg->permit_1;
    }
    if (mask.s.permit_valid_1) {
        fib_data0.bf.cl_fib_xr_permit_valid_1 = cfg->permit_valid_1;
    }

    if (mask.s.top_vlan_cmd_1) {
        fib_data1.bf.cl_fib_xr_top_vlan_cmd_1 = cfg->top_vlan_cmd_1;
    }
    if (mask.s.top_cmd_valid_1) {
        fib_data1.bf.cl_fib_xr_top_cmd_valid_1 = cfg->top_cmd_valid_1;
    }
    if (mask.s.top_vlanid_1) {
        fib_data1.bf.cl_fib_xr_top_vid_1 = cfg->top_vlanid_1 & 7;
        fib_data2.bf.cl_fib_xr_top_vid_1 = (cfg->top_vlanid_1 >> 3) & 0x1ff;
    }
    if (mask.s.top_sc_ind_1) {
        fib_data1.bf.cl_fib_xr_top_sc_ind_1 = cfg->top_sc_ind_1;
    }

    if (mask.s.inner_vlan_cmd_1) {
        fib_data1.bf.cl_fib_xr_inner_vlan_cmd_1 = cfg->inner_vlan_cmd_1;
    }
    if (mask.s.inner_cmd_valid_1) {
        fib_data1.bf.cl_fib_xr_inner_cmd_valid_1 =
            cfg->inner_cmd_valid_1;
    }
    if (mask.s.inner_vlanid_1) {
        fib_data1.bf.cl_fib_xr_inner_vid_1 = cfg->inner_vlanid_1;
    }
    if (mask.s.inner_sc_ind_1) {
        fib_data1.bf.cl_fib_xr_inner_sc_ind_1 = cfg->inner_sc_ind_1;
    }

    if (mask.s.dot1p_valid_1) {
        fib_data0.bf.cl_fib_xr_802_1p_valid_1 = cfg->dot1p_valid_1;
    }
    if (mask.s.dot1p_1) {
        fib_data1.bf.cl_fib_xr_802_1p_1 = cfg->dot1p_1;
    }

    if (mask.s.dscp_valid_1) {
        fib_data0.bf.cl_fib_xr_dscp_valid_1 = cfg->dscp_valid_1;
    }
    if (mask.s.dscp_1) {
        fib_data1.bf.cl_fib_xr_dscp_1 = cfg->dscp_1;
    }

    if (mask.s.cos_valid_1) {
        fib_data0.bf.cl_fib_xr_cos_valid_1 = cfg->cos_valid_1;
    }
    if (mask.s.cos_1) {
        fib_data0.bf.cl_fib_xr_cos_1 = cfg->cos_1;
    }
    if (mask.s.mark_ena_1) {
        fib_data0.bf.cl_fib_xr_mark_en_1 = cfg->mark_ena_1;
    }
    if (mask.s.flowid_1) {
        fib_data0.bf.cl_fib_xr_flowid_1 = cfg->flowid_1;
    }
    if (mask.s.dp_valid_1) {
        fib_data0.bf.cl_fib_xr_dp_valid_1 = cfg->dp_valid_1;
    }
    if (mask.s.dp_value_1) {
        fib_data0.bf.cl_fib_xr_ldpid_1 = cfg->dp_value_1;
    }

    if (mask.s.premarked_1) {
        fib_data0.bf.cl_fib_xr_premarked_1 = cfg->premarked_1;
    }

    if (mask.s.dscp_mark_down_1) {
        fib_data0.bf.cl_fib_xr_dscp_markdown_1 = cfg->dscp_mark_down_1;
    }

    if (mask.s.spt_mode_1) {
        fib_data0.bf.cl_fib_xr_stp_mode_1       = cfg->spt_mode_1;
    }

    if (mask.s.no_drop_1) {
        fib_data2.bf.cl_fib_xr_pkt_no_drop_1 = cfg->no_drop_1;
    }

    if(0 == device_id){
        CA_NE_REG_WRITE(fib_data4.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA4));

        CA_NE_REG_WRITE(fib_data3.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA3));
        CA_NE_REG_WRITE(fib_data2.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA2));
        CA_NE_REG_WRITE(fib_data1.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA1));
        CA_NE_REG_WRITE(fib_data0.wrd, AAL_L2FE_CLE_REG_ADDR(EGR_FIB_DATA0));
        WRITE_INDIRCT_REG(device_id, fib_id, L2FE_CLE_EGR_FIB_ACCESS);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_NE_REG_WRITE(fib_data4.wrd, L2FE_CLE_EGR_FIB_DATA4);
        CA_8279_NE_REG_WRITE(fib_data3.wrd, L2FE_CLE_EGR_FIB_DATA3);
        CA_8279_NE_REG_WRITE(fib_data2.wrd, L2FE_CLE_EGR_FIB_DATA2);
        CA_8279_NE_REG_WRITE(fib_data1.wrd, L2FE_CLE_EGR_FIB_DATA1);
        CA_8279_NE_REG_WRITE(fib_data0.wrd, L2FE_CLE_EGR_FIB_DATA0);
        CA_8279_WRITE_INDIRCT_REG(device_id, fib_id, L2FE_CLE_EGR_FIB_ACCESS);
#endif
    }
#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t
aal_l2_cls_egr_fib_get(CA_IN ca_device_id_t device_id,
                       CA_IN ca_uint32_t fib_id,
                       CA_OUT aal_l2_cls_fib_t * cfg)
{
    ca_status_t ret = CA_E_OK;
    L2FE_CLE_EGR_FIB_DATA0_t fib_data0;
    L2FE_CLE_EGR_FIB_DATA1_t fib_data1;
    L2FE_CLE_EGR_FIB_DATA2_t fib_data2;
    L2FE_CLE_EGR_FIB_DATA3_t fib_data3;
    L2FE_CLE_EGR_FIB_DATA4_t fib_data4;

    if ((fib_id > __AAL_L2_CLS_MAX_FIB_EGR_ENTRY_ID) || (NULL == cfg)) {
        return CA_E_PARAM;
    }

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp

    if(0 == device_id){
        READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_EGR_FIB_ACCESS);

        fib_data0.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA0);
        fib_data1.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA1);
        fib_data2.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA2);
        fib_data3.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA3);
        fib_data4.wrd = CA_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA4);
    }else if(1 == device_id){
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
        CA_8279_READ_INDIRCT_REG(device_id, fib_id, L2FE_CLE_EGR_FIB_ACCESS);

        fib_data0.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA0);
        fib_data1.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA1);
        fib_data2.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA2);
        fib_data3.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA3);
        fib_data4.wrd = CA_8279_NE_REG_READ(L2FE_CLE_EGR_FIB_DATA4);
#endif
    }
    cfg->permit_0           = fib_data2.bf.cl_fib_xr_permit_0;
    cfg->permit_valid_0     = fib_data2.bf.cl_fib_xr_permit_valid_0;
    cfg->premarked_0        = fib_data2.bf.cl_fib_xr_premarked_0;
    cfg->top_vlan_cmd_0     = fib_data4.bf.cl_fib_xr_top_vlan_cmd_0;
    cfg->top_cmd_valid_0    = fib_data4.bf.cl_fib_xr_top_cmd_valid_0;
    cfg->top_vlanid_0       = fib_data4.bf.cl_fib_xr_top_vid_0;
    cfg->top_sc_ind_0       = fib_data4.bf.cl_fib_xr_top_sc_ind_0;
    cfg->inner_vlan_cmd_0   = fib_data3.bf.cl_fib_xr_inner_vlan_cmd_0;
    cfg->inner_cmd_valid_0  = fib_data3.bf.cl_fib_xr_inner_cmd_valid_0;
    cfg->inner_vlanid_0     = fib_data3.bf.cl_fib_xr_inner_vid_0 | (fib_data4.bf.cl_fib_xr_inner_vid_0 << 9);
    cfg->inner_sc_ind_0     = fib_data3.bf.cl_fib_xr_inner_sc_ind_0 ;
    cfg->dot1p_valid_0      = fib_data3.bf.cl_fib_xr_802_1p_valid_0;
    cfg->dot1p_0            = fib_data3.bf.cl_fib_xr_802_1p_0;
    cfg->dscp_valid_0       = fib_data3.bf.cl_fib_xr_dscp_valid_0;
    cfg->dscp_0             = fib_data3.bf.cl_fib_xr_dscp_0;
    cfg->cos_valid_0        = fib_data3.bf.cl_fib_xr_cos_valid_0;
    cfg->cos_0              = fib_data3.bf.cl_fib_xr_cos_0;
    cfg->mark_ena_0         = fib_data2.bf.cl_fib_xr_mark_en_0;
    cfg->flowid_0           = fib_data2.bf.cl_fib_xr_flowid_0;
    cfg->dp_valid_0         = fib_data2.bf.cl_fib_xr_dp_valid_0;
    cfg->dp_value_0         = fib_data2.bf.cl_fib_xr_ldpid_0 | (fib_data3.bf.cl_fib_xr_ldpid_0 << 2);
    cfg->dscp_mark_down_0   = fib_data2.bf.cl_fib_xr_dscp_markdown_0;
    cfg->spt_mode_0         = fib_data2.bf.cl_fib_xr_stp_mode_0;
    cfg->no_drop_0          = fib_data4.bf.cl_fib_xr_pkt_no_drop_0;

    cfg->permit_1           = fib_data0.bf.cl_fib_xr_permit_1;
    cfg->permit_valid_1     = fib_data0.bf.cl_fib_xr_permit_valid_1;
    cfg->premarked_1        = fib_data0.bf.cl_fib_xr_premarked_1;
    cfg->top_vlan_cmd_1     = fib_data1.bf.cl_fib_xr_top_vlan_cmd_1;
    cfg->top_cmd_valid_1    = fib_data1.bf.cl_fib_xr_top_cmd_valid_1;
    cfg->top_vlanid_1       = fib_data1.bf.cl_fib_xr_top_vid_1 | (fib_data2.bf.cl_fib_xr_top_vid_1 << 3);
    cfg->top_sc_ind_1       = fib_data1.bf.cl_fib_xr_top_sc_ind_1;
    cfg->inner_vlan_cmd_1   = fib_data1.bf.cl_fib_xr_inner_vlan_cmd_1;
    cfg->inner_cmd_valid_1  = fib_data1.bf.cl_fib_xr_inner_cmd_valid_1;
    cfg->inner_vlanid_1     = fib_data1.bf.cl_fib_xr_inner_vid_1;
    cfg->inner_sc_ind_1     = fib_data1.bf.cl_fib_xr_inner_sc_ind_1;
    cfg->dot1p_valid_1      = fib_data0.bf.cl_fib_xr_802_1p_valid_1;
    cfg->dot1p_1            = fib_data1.bf.cl_fib_xr_802_1p_1;
    cfg->dscp_valid_1       = fib_data0.bf.cl_fib_xr_dscp_valid_1;
    cfg->dscp_1             = fib_data1.bf.cl_fib_xr_dscp_1;
    cfg->cos_valid_1        = fib_data0.bf.cl_fib_xr_cos_valid_1;
    cfg->cos_1              = fib_data0.bf.cl_fib_xr_cos_1;
    cfg->mark_ena_1         = fib_data0.bf.cl_fib_xr_mark_en_1;
    cfg->flowid_1           = fib_data0.bf.cl_fib_xr_flowid_1;
    cfg->dp_valid_1         = fib_data0.bf.cl_fib_xr_dp_valid_1;
    cfg->dp_value_1         = fib_data0.bf.cl_fib_xr_ldpid_1;
    cfg->dscp_mark_down_1   = fib_data0.bf.cl_fib_xr_dscp_markdown_1;
    cfg->spt_mode_1         = fib_data0.bf.cl_fib_xr_stp_mode_1;
    cfg->no_drop_1          = fib_data2.bf.cl_fib_xr_pkt_no_drop_1;


#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;
}

ca_status_t aal_l2_cls_init(ca_device_id_t device_id)
{

    ca_uint32_t entry_id = 0;
    aal_l2_cls_l2rl2r_key_mask_t key_mask;
    aal_l2_cls_l2rl2r_key_t      key;
    ca_status_t   ret = CA_E_OK;

    key_mask.u64 = 0;
    memset(&key, 0,sizeof(aal_l2_cls_ipv4_key_t));

    key.dot1p_ctrl_0        = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    key.dot1p_ctrl_1        = AAL_L2_CLS_DOT1P_FIELD_CTRL_MASK_DOT1P;
    key.mac_field_ctrl_0    = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    key.mac_field_ctrl_1    = AAL_L2_CLS_MAC_FIELD_CTRL_MASK_MAC;
    key.vlan_field_ctrl_0   = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    key.vlan_field_ctrl_1   = AAL_L2_CLS_VLAN_FIELD_CTRL_MASK_VLAN;
    key.vlan_number_vld_0   = FALSE;
    key.vlan_number_vld_1   = FALSE;
    key.ethernet_type_vld_1 = FALSE;
    key.ethernet_type_vld_0 = FALSE;
    key.l4_port_field_ctrl_0= AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;
    key.l4_port_field_ctrl_1= AAL_L2_CLS_L4_PORT_FIELD_CTRL_MASK_L4PORT;

    key_mask.s.dot1p_ctrl_0        = 1 ;
    key_mask.s.dot1p_ctrl_1        = 1 ;
    key_mask.s.mac_field_ctrl_0    = 1 ;
    key_mask.s.mac_field_ctrl_1    = 1 ;
    key_mask.s.vlan_field_ctrl_0   = 1 ;
    key_mask.s.vlan_field_ctrl_1   = 1 ;
    key_mask.s.vlan_number_vld_0   = 1 ;
    key_mask.s.vlan_number_vld_1   = 1 ;
    key_mask.s.ethernet_type_vld_1 = 1 ;
    key_mask.s.ethernet_type_vld_0 = 1 ;
    key_mask.s.l4_port_field_ctrl_0= 1 ;
    key_mask.s.l4_port_field_ctrl_1= 1 ;

#if 0//yocto
#else//sd1 uboot for 98f mp test - disable all igr_cls by default
    key.valid = DISABLE;
    key_mask.s.valid = ENABLE;
#endif//sd1 uboot for 98f mp test

    for(entry_id = 0; entry_id <= __AAL_L2_CLS_MAX_KEY_ENTRY_ID; entry_id ++){

        //
        ret = aal_l2_cls_l2rl2r_key_set(device_id,entry_id,key_mask,&key);
        if(ret != CA_E_OK){
            printf("\naal_l2_cls_l2rl2r_key_set error  entry_id\n %d", entry_id);
        }
    }
    printf("\naal_l2_cls_l2rl2r_key_set  entry_id %d\n", entry_id);

    return ret;

}

ca_status_t aal_l2_cls_entry_status_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t entry_id,
    CA_IN aal_l2_cls_entry_status_mask_t mask,
    CA_IN aal_l2_cls_entry_status_t *key_type)
{
    __aal_cls_key_data_t cls_key_data;
     ca_status_t ret = CA_E_OK;

     if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == key_type)) {
         return CA_E_PARAM;
     }
     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

     if (mask.s.valid) {
         cls_key_data.key_data12.bf.cl_key_valid = key_type->valid;
     }

     if(mask.s.entry_type){
        cls_key_data.key_data12.bf.cl_key_entry_type = ((key_type->entry_type >> 1)&1);
        cls_key_data.key_data11.bf0.cl_key_entry_type = (key_type->entry_type&1);
     }
     if(mask.s.is_ipv6_rule){
        cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid = key_type->is_ipv6_rule;
     }
     if(mask.s.rule_0){
        cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = key_type->rule_0;
     }
     if(mask.s.rule_1){
        cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = key_type->rule_1;
     }

    __aal_l2_cls_ingr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_entry_status_get(ca_device_id_t device_id, ca_uint32_t entry_id, aal_l2_cls_entry_status_t *key_type)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == key_type)) {
        return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_ingr_key_data_read(device_id,entry_id, &cls_key_data);

    key_type->valid = cls_key_data.key_data12.bf.cl_key_valid;

    if((cls_key_data.key_data12.bf.cl_key_entry_type == 1)&&(cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        key_type->entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE2ENTRY;
    }else if((cls_key_data.key_data12.bf.cl_key_entry_type == 0)&&(cls_key_data.key_data11.bf0.cl_key_entry_type == 0)){
        key_type->entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY;
        if(cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid == 1){
            key_type->is_ipv6_rule = TRUE;
        }else{
            key_type->is_ipv6_rule = FALSE;
        }
    }else if((cls_key_data.key_data12.bf.cl_key_entry_type == 0)&&(cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        key_type->entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
        key_type->is_ipv6_rule = FALSE;

        key_type->rule_0 = cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0;
        key_type->rule_1 = cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1;

    }    else{
        ret = CA_E_NOT_SUPPORT;
    }

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}

ca_status_t aal_l2_cls_egr_entry_status_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t entry_id,
    CA_IN aal_l2_cls_entry_status_mask_t mask,
    CA_IN aal_l2_cls_entry_status_t *key_type)
{
    __aal_cls_key_data_t cls_key_data;
     ca_status_t ret = CA_E_OK;

     if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == key_type)) {
         return CA_E_PARAM;
     }
     memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));
#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
     __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

     if (mask.s.valid) {
         cls_key_data.key_data12.bf.cl_key_valid = key_type->valid;
     }

     if(mask.s.entry_type){
        cls_key_data.key_data12.bf.cl_key_entry_type = ((key_type->entry_type >> 1)&1);
        cls_key_data.key_data11.bf0.cl_key_entry_type = (key_type->entry_type&1);
     }
     if(mask.s.is_ipv6_rule){
        cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid = key_type->is_ipv6_rule;
     }
     if(mask.s.rule_0){
        cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0 = key_type->rule_0;
     }
     if(mask.s.rule_1){
        cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1 = key_type->rule_1;
     }

    __aal_l2_cls_egr_key_data_write(device_id,entry_id, &cls_key_data);

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_egr_entry_status_get(ca_device_id_t device_id, ca_uint32_t entry_id, aal_l2_cls_entry_status_t *key_type)
{

    __aal_cls_key_data_t cls_key_data;
    ca_status_t ret = CA_E_OK;

    if ((entry_id > __AAL_L2_CLS_MAX_KEY_ENTRY_ID) || (NULL == key_type)) {
        return CA_E_PARAM;
    }
    memset(&cls_key_data, 0, sizeof(__aal_cls_key_data_t));

#if 0//yocto
    __L2_CLS_LOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    __aal_l2_cls_egr_key_data_read(device_id,entry_id, &cls_key_data);

    key_type->valid = cls_key_data.key_data12.bf.cl_key_valid;

    if((cls_key_data.key_data12.bf.cl_key_entry_type == 1)&&(cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        key_type->entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE2ENTRY;
    }else if((cls_key_data.key_data12.bf.cl_key_entry_type == 0)&&(cls_key_data.key_data11.bf0.cl_key_entry_type == 0)){
        key_type->entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_1RULE1ENTRY;
        if(cls_key_data.key_data11.bf0.cl_key_orhr_ipv6_valid == 1){
            key_type->is_ipv6_rule = TRUE;
        }else{
            key_type->is_ipv6_rule = FALSE;
        }
    }else if((cls_key_data.key_data12.bf.cl_key_entry_type == 0)&&(cls_key_data.key_data11.bf0.cl_key_entry_type == 1)){
        key_type->entry_type = AAL_L2_CLS_KEY_ENTRY_TYPE_2RULE1ENTRY;
        key_type->is_ipv6_rule = FALSE;

        key_type->rule_0 = cls_key_data.key_data11.bf2.cl_key_tre_rule_type_0;
        key_type->rule_1 = cls_key_data.key_data11.bf2.cl_key_tre_rule_type_1;

    }    else{
        ret = CA_E_NOT_SUPPORT;
    }

#if 0//yocto
    __L2_CLS_UNLOCK();
#else//sd1 uboot for 98f mp
#endif//sd1 uboot for 98f mp
    return ret;

}


ca_status_t aal_l2_cls_debug_set(
    CA_IN ca_device_id_t               device_id,
    CA_IN aal_l2_cls_debug_flag_t   *debug_flag)
{
    if(NULL == debug_flag){
        return CA_E_PARAM;
    }
    __g_l2_cls_debug = debug_flag->debug_flag;
    return CA_E_OK;
}

ca_status_t aal_l2_cls_debug_get(
    CA_IN ca_device_id_t               device_id,
    CA_OUT aal_l2_cls_debug_flag_t   *debug_flag)
{
    if(NULL == debug_flag){
        return CA_E_PARAM;
    }

    debug_flag->debug_flag = __g_l2_cls_debug;

    return CA_E_OK;
}


EXPORT_SYMBOL(aal_l2_cls_cmrcmr_key_get);
EXPORT_SYMBOL(aal_l2_cls_l2rl3r_key_get);
EXPORT_SYMBOL(aal_l2_cls_l2rcmr_key_get);
EXPORT_SYMBOL(aal_l2_cls_cmrl3r_key_set);
EXPORT_SYMBOL(aal_l2_cls_l3rl2r_key_get);
EXPORT_SYMBOL(aal_l2_cls_ipv6_short_key_set);
EXPORT_SYMBOL(aal_l2_cls_cmrl3r_key_get);
EXPORT_SYMBOL(aal_l2_cls_l3rcmr_key_get);
EXPORT_SYMBOL(aal_l2_cls_l2rl2r_key_set);
EXPORT_SYMBOL(aal_l2_cls_cmrcmr_key_set);
EXPORT_SYMBOL(aal_l2_cls_l2rl3r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_fib_set);
EXPORT_SYMBOL(aal_l2_cls_cmrl2r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_fib_get);
EXPORT_SYMBOL(aal_l2_cls_ipv4_key_get);
EXPORT_SYMBOL(aal_l2_cls_fib_get);
EXPORT_SYMBOL(aal_l2_cls_l2rcmr_key_set);
EXPORT_SYMBOL(aal_l2_cls_cmrl2r_key_set);
EXPORT_SYMBOL(aal_l2_cls_l3rcmr_key_set);
EXPORT_SYMBOL(aal_l2_cls_l3rl3r_key_set);
EXPORT_SYMBOL(aal_l2_cls_ipv6_short_key_get);
EXPORT_SYMBOL(aal_l2_cls_l2rl2r_key_get);
EXPORT_SYMBOL(aal_l2_cls_ipv4_key_set);
EXPORT_SYMBOL(aal_l2_cls_l3rl2r_key_set);
EXPORT_SYMBOL(aal_l2_cls_fib_set);
EXPORT_SYMBOL(aal_l2_cls_l3rl3r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_cmrcmr_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2rl3r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2rcmr_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_cmrl3r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3rl2r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_ipv6_short_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_cmrl3r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l3rcmr_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2rl2r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_cmrcmr_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l2rl3r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_cmrl2r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_ipv4_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2rcmr_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_cmrl2r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3rcmr_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3rl3r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_ipv6_short_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2rl2r_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_ipv4_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3rl2r_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3rl3r_key_get);
EXPORT_SYMBOL(aal_l2_cls_entry_status_get);
EXPORT_SYMBOL(aal_l2_cls_entry_status_set);
EXPORT_SYMBOL(aal_l2_cls_l2r_1st_key_get);
EXPORT_SYMBOL(aal_l2_cls_l2r_1st_key_set);
EXPORT_SYMBOL(aal_l2_cls_l2r_2nd_key_get);
EXPORT_SYMBOL(aal_l2_cls_l2r_2nd_key_set);
EXPORT_SYMBOL(aal_l2_cls_l3r_1st_key_get);
EXPORT_SYMBOL(aal_l2_cls_l3r_1st_key_set);
EXPORT_SYMBOL(aal_l2_cls_l3r_2nd_key_get);
EXPORT_SYMBOL(aal_l2_cls_l3r_2nd_key_set);
EXPORT_SYMBOL(aal_l2_cls_cmr_1st_key_get);
EXPORT_SYMBOL(aal_l2_cls_cmr_1st_key_set);
EXPORT_SYMBOL(aal_l2_cls_cmr_2nd_key_get);
EXPORT_SYMBOL(aal_l2_cls_cmr_2nd_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l2r_1st_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2r_1st_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l2r_2nd_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l2r_2nd_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3r_1st_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l3r_1st_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_l3r_2nd_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_l3r_2nd_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_cmr_1st_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_cmr_1st_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_cmr_2nd_key_get);
EXPORT_SYMBOL(aal_l2_cls_egr_cmr_2nd_key_set);
EXPORT_SYMBOL(aal_l2_cls_egr_entry_status_get);
EXPORT_SYMBOL(aal_l2_cls_egr_entry_status_set);
EXPORT_SYMBOL(aal_l2_cls_debug_set);
EXPORT_SYMBOL(aal_l2_cls_debug_get);

