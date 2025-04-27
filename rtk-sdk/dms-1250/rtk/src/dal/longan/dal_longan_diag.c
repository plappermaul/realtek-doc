/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : Definition those public Port APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Port
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>

#include <rtk/port.h>
#include <rtk/default.h>

#include <rtk/diag.h>
#include <dal/longan/dal_longan_diag.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>

#if (defined CONFIG_SDK_APP_DIAG_EXT)
#include <dal/longan/diag_table_reg.c>

#include <dal/longan/dal_longan_l2.h>
#include <dal/longan/dal_longan_l3.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>

#include <hal/chipdef/allreg.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <dal/longan/dal_longan_vlan.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               diag_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         diag_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* vlan semaphore handling */
#define DIAG_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(diag_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_DIAG), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define DIAG_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(diag_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_DIAG), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


#if (defined CONFIG_SDK_APP_DIAG_EXT)

#define DAL_LONGAN_HOST_ADDR_TO_IDX(_addr)       ((((_addr)/8)*6) + ((_addr)%8))
#define HASH_BIT_EXTRACT(_val, _lsb, _len)   (((_val) & (((1 << (_len)) - 1) << (_lsb))) >> (_lsb))

#define MAX_DEBUG_FIELD_LEN 4    // Maximum 128 bits for IPv6 address
#define MAX_DEBUG_ENTRY_LEN (256-62)    // Maximum 8192 bits
#define MAX_TABLE_FIELD_NAME_LEN 128
#define MAX_VALUE_STRING 128

static void _ipv6_swap(uint32 *da_swap_ptr, uint32 *da_ptr)
{
    da_swap_ptr[0] = da_ptr[3];
    da_swap_ptr[1] = da_ptr[2];
    da_swap_ptr[2] = da_ptr[1];
    da_swap_ptr[3] = da_ptr[0];
}

static int32 _diag_table_reg_name2int(uint32 unit, char *table_name, char *field_name, diag_table_reg_field_name_type_t type, int32 *value)
{
    diag_table_reg_name_value_pair_t *pArray = NULL;
    int32 found = 0;
    char *name = NULL;
    char string[MAX_TABLE_FIELD_NAME_LEN];

    osal_memset(string, 0, sizeof(string));

        switch (type) {
            case DIAG_TABLE_NAME: {
                pArray = longan_diag_table_array;
                name = table_name;
                break;
            }

        case DIAG_TABLE_FIELD_NAME: {
            pArray = longan_diag_table_field_array;
            osal_strcat(string, table_name);
            osal_strcat(string, "_");
            osal_strcat(string, field_name);
            name = string;
            break;
        }

        case DIAG_REG_NAME: {
            pArray = longan_diag_reg_array;
            name = table_name;
            break;
        }

        default: {
            pArray = longan_diag_reg_field_array;
            name = field_name;
            break;
        }
    }

    while('\0' != pArray->name[0])
    {
        if (0 == osal_strcmp(pArray->name, name)) {
            found = 1;
            break;
        } else {
            pArray += 1;
        }
    }


    if (!found) {
        return (RT_ERR_FAILED);
    } else {
        *value = pArray->value;
        return (RT_ERR_OK);
    }
}

static int32 _diag_table_name2int(uint32 unit, char *name, int32 *value)
{
    diag_table_reg_field_name_type_t type = DIAG_TABLE_NAME;

    if (!name || !value)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
        return (RT_ERR_FAILED);
    }

    return (_diag_table_reg_name2int(unit, name, "", type, value));
}

static int32 _diag_table_field_name2int(uint32 unit, char *table_name, char *field_name, int32 *value)
{
    diag_table_reg_field_name_type_t type = DIAG_TABLE_FIELD_NAME;

    if (!table_name || !field_name || !value)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
        return (RT_ERR_FAILED);
    }

    return (_diag_table_reg_name2int(unit, table_name, field_name, type, value));
}

static int32 _diag_reg_name2int(uint32 unit, char *name, int32 *value)
{
    diag_table_reg_field_name_type_t type = DIAG_REG_NAME;

    if (!name || !value)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
        return (RT_ERR_FAILED);
    }

    return (_diag_table_reg_name2int(unit, name, "", type, value));
}

static int32 _diag_reg_field_name2int(uint32 unit, char *name, int32 *value)
{
    diag_table_reg_field_name_type_t type = DIAG_REG_FIELD_NAME;

    if (!name || !value)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
        return (RT_ERR_FAILED);
    }

    return (_diag_table_reg_name2int(unit, "", name, type, value));
}

static int32 _diag_l3_hash_algrithom_9300(uint32 unit, rtk_diag_l3_entry_type_t type, uint32 hash_id, uint32 *sa_ptr, uint32 *da_ptr, uint32 vid, uint32 *bucket_ptr)
{
    uint32 hash_seed[32];
    uint32 vlan_key_exist = 0;
    uint32 sa_key_exist = 0;
    uint32 is_ipv4 = 0;
    uint32 is_ucast = 0;
    uint32 sa[IP6_ADDR_LEN>>2];
    uint32 da[IP6_ADDR_LEN>>2];
    uint32 vlan_id = 0;
    uint32 sum,sum1;
    uint32 vidHashSel;
    int32 ret;


    switch (type) {
        case L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN: {
            vlan_key_exist = 1;
            is_ipv4 = 1;
            break;
        }

        case L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN: {
            vlan_key_exist = 1;
            sa_key_exist = 1;
            is_ipv4 = 1;
            break;
        }

        case L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN: {
            vlan_key_exist = 1;
            break;
        }

        case L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN: {
            vlan_key_exist = 1;
            sa_key_exist = 1;
            break;
        }

        case L3_ENTRY_L3_UCAST_IPV4: {
            is_ipv4 = 1;
            is_ucast = 1;
            break;
        }

        case L3_ENTRY_L3_UCAST_IPV6: {
            is_ucast = 1;
            break;
        }

        case L3_ENTRY_L3_MCAST_IPV4_SINGLE: {
            is_ipv4 = 1;
            break;
        }

        case L3_ENTRY_L3_MCAST_IPV4_DOUBLE: {
            sa_key_exist = 1;
            is_ipv4 = 1;
            break;
        }

        case L3_ENTRY_L3_MCAST_IPV6_SINGLE: {
            break;
        }

        default: {    //  L3_ENTRY_L3_MCAST_IPV6_DOUBLE
            sa_key_exist = 1;
            break;
        }
    }

    osal_memset(hash_seed, 0, sizeof(hash_seed));
    osal_memset(sa, 0, IP6_ADDR_LEN);
    osal_memset(da, 0, IP6_ADDR_LEN);
    if (is_ipv4){
        da[3] = da_ptr[0];
        if (sa_key_exist){
            sa[3] = sa_ptr[0];
        } else {
            if (!is_ucast) {
                sa[3] = 0xFFFFFFFF;
            }
        }
    } else {
        osal_memcpy(da, da_ptr, IP6_ADDR_LEN);
        if (sa_key_exist){
            osal_memcpy(sa, sa_ptr, IP6_ADDR_LEN);
        } else {
            if (!is_ucast) {
                osal_memset(sa, 0xFF, IP6_ADDR_LEN);
            }
        }
    }
    if (vlan_key_exist) {
        vlan_id = vid & 0xFFF;
    } else {
        if ( RT_ERR_OK != (ret = reg_field_read(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_LU_FORCE_MODE_HASH_KEY_SELf, &vidHashSel)))
        {
            RT_ERR(ret, (MOD_DAL|MOD_DIAG), "");
            return (RT_ERR_FAILED);
        }

        if (1 == vidHashSel)
            vlan_id = vid & 0xFFF;
    }

    //remove compile waring
    sum = 0;
    sum1 = 0;
    sum = sum + sum1;

    if ((hash_id != 0) && (hash_id != 1)) {
        return (RT_ERR_INPUT);
    } else {
       if (hash_id == 0)
         {
            /*sip[127~96]*/
            hash_seed[0] = HASH_BIT_EXTRACT(sa[0], 27, 5);
            hash_seed[1] = HASH_BIT_EXTRACT(sa[0], 18, 9);
            hash_seed[2] = HASH_BIT_EXTRACT(sa[0], 9, 9);
            hash_seed[3] = HASH_BIT_EXTRACT(sa[0], 0, 9);

            /*sip[95~64]*/
            hash_seed[4] = HASH_BIT_EXTRACT(sa[1], 23, 9);
            hash_seed[5] = HASH_BIT_EXTRACT(sa[1], 14, 9);
            hash_seed[6] = HASH_BIT_EXTRACT(sa[1], 5, 9);
            hash_seed[7] = (HASH_BIT_EXTRACT(sa[1], 0, 5) << 4) | (HASH_BIT_EXTRACT(sa[2], 28, 4));

            /*sip[63~32]*/
            hash_seed[8] = HASH_BIT_EXTRACT(sa[2], 19, 9);
            hash_seed[9] = HASH_BIT_EXTRACT(sa[2], 10, 9);
            hash_seed[10] = HASH_BIT_EXTRACT(sa[2], 1, 9);
            hash_seed[11] = (HASH_BIT_EXTRACT(sa[2], 0, 1) << 8) |(HASH_BIT_EXTRACT(sa[3], 24,8)) ;

            /*sip[31:0]*/
            hash_seed[12] = HASH_BIT_EXTRACT(sa[3], 15, 9);
            hash_seed[13] = HASH_BIT_EXTRACT(sa[3], 6, 9);
            hash_seed[14] = HASH_BIT_EXTRACT(sa[3], 0, 6) << 2;

            /*dip[127~96]*/
            hash_seed[15] = HASH_BIT_EXTRACT(da[0], 30, 2);

            hash_seed[16] = HASH_BIT_EXTRACT(da[0], 21, 9);
            hash_seed[17] = HASH_BIT_EXTRACT(da[0], 12, 9);
            hash_seed[18] = HASH_BIT_EXTRACT(da[0], 3, 9);
            hash_seed[19] = (HASH_BIT_EXTRACT(da[0], 0, 3) << 6) | (HASH_BIT_EXTRACT(da[1], 26, 6));

            /*dip[95~64]*/
            hash_seed[20] = HASH_BIT_EXTRACT(da[1], 17, 9);
            hash_seed[21] = HASH_BIT_EXTRACT(da[1], 8, 9);
            hash_seed[22] = (HASH_BIT_EXTRACT(da[1], 0, 8) << 1) | (HASH_BIT_EXTRACT(da[2], 31, 1));

            /*dip[63~32]*/
            hash_seed[23] = HASH_BIT_EXTRACT(da[2], 22, 9);
            hash_seed[24] = HASH_BIT_EXTRACT(da[2], 13, 9);
            hash_seed[25] = HASH_BIT_EXTRACT(da[2], 4, 9);
            hash_seed[26] = (HASH_BIT_EXTRACT(da[2], 0, 4) << 5) | (HASH_BIT_EXTRACT(da[3], 27, 5));

            /*dip[31:0]*/
            hash_seed[27] = HASH_BIT_EXTRACT(da[3], 18, 9);
            hash_seed[28] = HASH_BIT_EXTRACT(da[3], 9, 9);
            hash_seed[29] = HASH_BIT_EXTRACT(da[3], 0, 9);

            hash_seed[30] = HASH_BIT_EXTRACT(vlan_id, 3, 9);
            hash_seed[31] = HASH_BIT_EXTRACT(vlan_id, 0, 3) << 5;

            *bucket_ptr = hash_seed[0] ^ hash_seed[1] ^ hash_seed[2] ^ hash_seed[3] ^ \
                                    hash_seed[4] ^ hash_seed[5] ^ hash_seed[6] ^ hash_seed[7] ^ \
                                    hash_seed[8] ^ hash_seed[9] ^ hash_seed[10] ^ hash_seed[11] ^ \
                                    hash_seed[12] ^ hash_seed[13] ^ hash_seed[14] ^ hash_seed[15] ^ \
                                    hash_seed[16] ^ hash_seed[17] ^ hash_seed[18] ^ hash_seed[19] ^ \
                                    hash_seed[20] ^ hash_seed[21] ^ hash_seed[22] ^ hash_seed[23] ^ \
                                    hash_seed[24] ^ hash_seed[25] ^ hash_seed[26] ^ hash_seed[27] ^ \
                                    hash_seed[28] ^ hash_seed[29] ^ hash_seed[30] ^ hash_seed[31] ;
         }
         else
         {
            hash_seed[0] = HASH_BIT_EXTRACT(sa[3], 26, 6);
            hash_seed[1] = HASH_BIT_EXTRACT(sa[3], 17, 9);
            hash_seed[2] = HASH_BIT_EXTRACT(sa[3], 8, 9);
            hash_seed[3] = HASH_BIT_EXTRACT(sa[3], 0, 8) << 1;

            hash_seed[4] = HASH_BIT_EXTRACT(da[3], 27, 5);
            hash_seed[5] = HASH_BIT_EXTRACT(da[3], 18, 9);
            hash_seed[6] = HASH_BIT_EXTRACT(da[3], 9, 9);

            //pre_hash
            sum = hash_seed[0] + hash_seed[1] + hash_seed[2] + hash_seed[3] + \
                        hash_seed[4] + hash_seed[5] + hash_seed[6];
            sum1 = (sum & 0x1FF) + ((sum & (0x1FF << 9)) >> 9);
            hash_seed[7] = (sum1 & 0x1FF) + ((sum1 & (0x1FF << 9)) >> 9);

            hash_seed[8] = HASH_BIT_EXTRACT(sa[0], 24, 8);
            hash_seed[9] = HASH_BIT_EXTRACT(sa[0], 15, 9);
            hash_seed[10] = HASH_BIT_EXTRACT(sa[0], 6, 9);
            hash_seed[11] = (HASH_BIT_EXTRACT(sa[0], 0, 6) << 3) | (HASH_BIT_EXTRACT(sa[1], 29, 3));

            hash_seed[12] = HASH_BIT_EXTRACT(sa[1], 20, 9);
            hash_seed[13] = HASH_BIT_EXTRACT(sa[1], 11, 9);
            hash_seed[14] = HASH_BIT_EXTRACT(sa[1], 2, 9);
            hash_seed[15] = (HASH_BIT_EXTRACT(sa[1], 0, 2) << 7) |(HASH_BIT_EXTRACT(sa[2], 25, 7));

            hash_seed[16] = HASH_BIT_EXTRACT(sa[2], 16, 9);
            hash_seed[17] = HASH_BIT_EXTRACT(sa[2], 7, 9);
            hash_seed[18] = (HASH_BIT_EXTRACT(sa[2], 0, 7) << 2) |(HASH_BIT_EXTRACT(da[0], 30, 2));

            hash_seed[19] = HASH_BIT_EXTRACT(da[0], 21, 9);
            hash_seed[20] = HASH_BIT_EXTRACT(da[0], 12, 9);
            hash_seed[21] = HASH_BIT_EXTRACT(da[0], 3, 9);
            hash_seed[22] = (HASH_BIT_EXTRACT(da[0], 0, 3) << 6) | (HASH_BIT_EXTRACT(da[1], 26, 6));

            hash_seed[23] = HASH_BIT_EXTRACT(da[1], 17, 9);
            hash_seed[24] = HASH_BIT_EXTRACT(da[1], 8, 9);
            hash_seed[25] = HASH_BIT_EXTRACT(da[1], 0, 8) << 1 |(HASH_BIT_EXTRACT(da[2], 31, 1));

            hash_seed[26] = HASH_BIT_EXTRACT(da[2], 22, 9);
            hash_seed[27] = HASH_BIT_EXTRACT(da[2], 13, 9);
            hash_seed[28] = HASH_BIT_EXTRACT(da[2], 4, 9);
            hash_seed[29] = (HASH_BIT_EXTRACT(da[2], 0, 4) << 5) | (HASH_BIT_EXTRACT(vlan_id, 7, 5));

            hash_seed[30] = HASH_BIT_EXTRACT(vlan_id, 0, 7) << 2;
            hash_seed[31] = HASH_BIT_EXTRACT(da[3], 0, 9);

            *bucket_ptr =
                    hash_seed[7] ^ \
                    hash_seed[8] ^ hash_seed[9] ^ hash_seed[10] ^ hash_seed[11] ^ \
                    hash_seed[12] ^ hash_seed[13] ^ hash_seed[14] ^ hash_seed[15] ^ \
                    hash_seed[16] ^ hash_seed[17] ^ hash_seed[18] ^ hash_seed[19] ^ \
                    hash_seed[20] ^ hash_seed[21] ^ hash_seed[22] ^ hash_seed[23] ^ \
                    hash_seed[24] ^ hash_seed[25] ^ hash_seed[26] ^ hash_seed[27] ^ \
                    hash_seed[28] ^ hash_seed[29] ^ hash_seed[30] ^ hash_seed[31] ;
         }
    }

    return (RT_ERR_OK);
}

int32 _diag_l3_hash_search(uint32 unit, rtk_diag_l3_entry_type_t type, uint32 *sa_ptr, uint32 *da_ptr, uint32 vid,
    uint32 *index, uint32 *hit, uint32 *free, uint32 *vid_cmp, int32 mcast_exact_match, int32 vid_cmp_src, void *p_l3_entry)
{
    int32 ret = RT_ERR_FAILED;
    uint32 bucket_0 = 0;
    uint32 bucket_1 = 0;
    host_routing_entry_t l3_entry;
    host_routing_entry_t l3_entry_free_0;
    host_routing_entry_t l3_entry_free_1;
    host_routing_entry_t l3_entry_hit_0;
    host_routing_entry_t l3_entry_hit_1;
    uint32 l3_entry_hit;
    uint32 hit_0 = 0;
    uint32 hit_1 = 0;
    uint32 l3_entry_hit_vid_cmp = 0;
    uint32 l3_entry_free;
    uint32 free_0 = 0;
    uint32 free_1 = 0;
    uint32 l3_entry_free_addr_0 = 0;
    uint32 l3_entry_free_addr_1 = 0;
    uint32 l3_entry_hit_addr_0 = 0;
    uint32 l3_entry_hit_addr_1 = 0;
    uint32 l3_hash_free_total_0 = 0;
    uint32 l3_hash_free_total_1 = 0;

    uint32 field_vid;
    uint32 field_vid_cmp;
    uint32 field_sa[IP6_ADDR_LEN>>2] = {0, 0, 0, 0};
    uint32 field_da[IP6_ADDR_LEN>>2] = {0, 0, 0, 0};
    uint32 field_sa_all_one[IP6_ADDR_LEN>>2] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    uint32 field_valid;
    uint32 field_entry_type;
    uint32 field_ipmc_type;
    uint32 l3_hash_algo_blk0 = 0;
    uint32 l3_hash_algo_blk1 = 1;
    uint32 i;
    uint32 addr;
    uint32 step = 0;
    uint32 j;
    uint32 free_count_continuous;
    uint32 align;
    uint32 sip_swap[IP6_ADDR_LEN>>2];
    uint32 dip_swap[IP6_ADDR_LEN>>2];
    uint32 isL3mc = 0;

    int32 l3_uc = (type == L3_ENTRY_L3_UCAST_IPV4) || (type == L3_ENTRY_L3_UCAST_IPV6);

    switch (type) {
        case L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN:
        case L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN:
        case L3_ENTRY_L3_MCAST_IPV4_SINGLE:
        case L3_ENTRY_L3_MCAST_IPV4_DOUBLE:
            align = 2;
            break;

        case L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN:
        case L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN:
        case L3_ENTRY_L3_MCAST_IPV6_SINGLE:
        case L3_ENTRY_L3_MCAST_IPV6_DOUBLE:
            align = 6;
            break;

        case L3_ENTRY_L3_UCAST_IPV4:
            align = 1;
            break;

        default:    // L3_ENTRY_L3_UCAST_IPV6
            align = 3;
            break;
    }

    if (l3_uc)
    {
        RT_ERR_CHK(reg_array_field_read(unit, LONGAN_L3_HOST_TBL_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_UC_HASH_ALG_SEL_0f, &l3_hash_algo_blk0), ret);
    }
    else
    {
        RT_ERR_CHK(reg_array_field_read(unit, LONGAN_L3_HOST_TBL_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_MC_HASH_ALG_SEL_0f, &l3_hash_algo_blk0), ret);
    }
    RT_ERR_CHK(_diag_l3_hash_algrithom_9300(unit, type, l3_hash_algo_blk0, sa_ptr, da_ptr, vid, &bucket_0), ret);

    if (l3_uc)
    {
        RT_ERR_CHK(reg_array_field_read(unit, LONGAN_L3_HOST_TBL_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_UC_HASH_ALG_SEL_1f, &l3_hash_algo_blk1), ret);
    }
    else
    {
        RT_ERR_CHK(reg_array_field_read(unit, LONGAN_L3_HOST_TBL_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_MC_HASH_ALG_SEL_1f, &l3_hash_algo_blk1), ret);
    }
    RT_ERR_CHK(_diag_l3_hash_algrithom_9300(unit, type, l3_hash_algo_blk1, sa_ptr, da_ptr, vid, &bucket_1), ret);

    for (j = 0; j < 2; j++)
    {
        free_count_continuous = 0;
        for (i = 0; i < 6; )
        {
            l3_entry_hit = 0;
            l3_entry_free = 0;
            l3_entry_hit_vid_cmp = 0;

        addr = (j == 0) ? ((bucket_0 << 3) | i) : ((1 << 12) | (bucket_1 << 3) | i);
        addr = DAL_LONGAN_HOST_ADDR_TO_IDX(addr);
        osal_memset(l3_entry.entry_data, 0, sizeof(l3_entry));
        RT_ERR_CHK(table_read(unit, LONGAN_L3_HOST_ROUTE_IPUCt, addr, (uint32 *)&l3_entry), ret);
        RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPUCt, LONGAN_L3_HOST_ROUTE_IPUC_VALIDtf, &field_valid, (uint32 *)&l3_entry), ret);
        RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPUCt, LONGAN_L3_HOST_ROUTE_IPUC_ENTRY_TYPEtf, &field_entry_type, (uint32 *)&l3_entry), ret);

        switch (type) {
            case L3_ENTRY_L3_UCAST_IPV4: {
                if (!field_valid) {    // free entry
                    step = 1;
                    l3_entry_free = 1;
                } else if (field_entry_type == 0) {    // entry type match
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPUCt, LONGAN_L3_HOST_ROUTE_IPUC_IPtf, field_da, (uint32 *)&l3_entry), ret);
                    l3_entry_hit = (field_da[0] == *da_ptr);    // match DIP
                    step = 1;
                } else if (field_entry_type == 1) {
                    step = 2;
                    RT_ERR_CHK((i % 2), ret);
                } else if (field_entry_type == 2) {
                    step = 3;
                    RT_ERR_CHK((i % 3), ret);
                } else {
                    step = 6;
                    RT_ERR_CHK((i % 6), ret);
                }

                break;
            }

            case L3_ENTRY_L3_UCAST_IPV6: {
                if (!field_valid) {
                    step = 1;
                    if (free_count_continuous == 2) {
                        l3_entry_free = 1;
                        free_count_continuous = 0;
                    } else if ((free_count_continuous == 0) && ((i % align) != 0)) { // Only start counting when align
                        free_count_continuous = 0;
                    } else {
                        free_count_continuous += 1;
                    }
                } else if (field_entry_type == 2) {
                    osal_memset(l3_entry.entry_data, 0, sizeof(l3_entry));
                    RT_ERR_CHK(table_read(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, addr, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, LONGAN_L3_HOST_ROUTE_IP6UC_VALIDtf, &field_valid, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, LONGAN_L3_HOST_ROUTE_IP6UC_ENTRY_TYPEtf, &field_entry_type, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, LONGAN_L3_HOST_ROUTE_IP6UC_IPtf, field_da, (uint32 *)&l3_entry), ret);

                    _ipv6_swap(dip_swap, field_da);
                    l3_entry_hit = (dip_swap[0] == *da_ptr) &&
                                   (dip_swap[1] == *(da_ptr+1)) &&
                                   (dip_swap[2] == *(da_ptr+2)) &&
                                   (dip_swap[3] == *(da_ptr+3));    // match DIP
                    step = 3;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 3), ret);
                } else if (field_entry_type == 0) {
                    step = 1;
                    free_count_continuous = 0;
                } else if (field_entry_type == 1) {
                    step = 2;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 2), ret);
                } else {
                    step = 6;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 6), ret);
                }

                break;
            }

            case L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN:
            case L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN:
            case L3_ENTRY_L3_MCAST_IPV4_SINGLE:
            case L3_ENTRY_L3_MCAST_IPV4_DOUBLE: {
                if (!field_valid) {
                    step = 1;
                    if (free_count_continuous == 1) {
                        l3_entry_free = 1;
                        free_count_continuous = 0;
                    } else if ((free_count_continuous == 0) && ((i % align) != 0)) { // Only start counting when align
                        free_count_continuous = 0;
                    } else {
                        free_count_continuous += 1;
                    }
                } else if (field_entry_type == 1) {
                    uint32 ipmc_type = ((type == L3_ENTRY_L3_MCAST_IPV4_SINGLE) || (type == L3_ENTRY_L3_MCAST_IPV4_DOUBLE)) ? 1 : 0;
                    uint32 sa_exist = (type == L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN) ||
                                      (type == L3_ENTRY_L3_MCAST_IPV4_DOUBLE);
                    uint32 vlan_exist = (type == L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN) || (type == L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN);
                    isL3mc = (L3_ENTRY_L3_MCAST_IPV4_DOUBLE == type) ||(L3_ENTRY_L3_MCAST_IPV4_SINGLE == type) ;
                    osal_memset(l3_entry.entry_data, 0, sizeof(l3_entry));
                    RT_ERR_CHK(table_read(unit, LONGAN_L3_HOST_ROUTE_IPMCt, addr, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, &field_ipmc_type, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, field_sa, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, field_da, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, &field_vid, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_VID_CMPtf, &field_vid_cmp, (uint32 *)&l3_entry), ret);

                    l3_entry_hit = (field_ipmc_type == ipmc_type) &&
                                   (field_da[0] == *da_ptr) &&
                                   (sa_exist ? (field_sa[0] == *sa_ptr) : (field_sa[0] == field_sa_all_one[0])) &&
                                   (!vlan_exist || (field_vid == vid));
                        if (isL3mc)
                        {
                            l3_entry_hit &= (vid_cmp_src == field_vid_cmp);
                        }
                    if (field_vid_cmp) {
                        l3_entry_hit &= ((vid_cmp_src == 1) && (field_vid == vid));
                    } else {    // vid_cmp not to be compared
                        if (mcast_exact_match) {
                            l3_entry_hit &= (!vid_cmp_src || ((field_vid == vid)));
                        }
                    }
                    l3_entry_hit_vid_cmp = l3_entry_hit && (field_vid_cmp && (field_vid == vid));
                    step = 2;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 2), ret);
                } else if (field_entry_type == 0) {
                    step = 1;
                    free_count_continuous = 0;
                } else if (field_entry_type == 2) {
                    step = 3;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 3), ret);
                } else {
                    step = 6;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 6), ret);
                }

                break;
            }

            case L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN:
            case L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN:
            case L3_ENTRY_L3_MCAST_IPV6_SINGLE:
            case L3_ENTRY_L3_MCAST_IPV6_DOUBLE: {
                if (!field_valid) {
                    step = 1;
                    if (free_count_continuous == 5) {
                        l3_entry_free = 1;
                        free_count_continuous = 0;
                    } else if ((free_count_continuous == 0) && ((i % align) != 0)) { // Only start counting when align
                        free_count_continuous = 0;
                    } else {
                        free_count_continuous += 1;
                    }
                } else if (field_entry_type == 3) {
                    uint32 ipmc_type = ((type == L3_ENTRY_L3_MCAST_IPV6_SINGLE) || (type == L3_ENTRY_L3_MCAST_IPV6_DOUBLE)) ? 1 : 0;
                    uint32 sa_exist = (type == L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN) ||
                                      (type == L3_ENTRY_L3_MCAST_IPV6_DOUBLE);
                    uint32 vlan_exist = (type == L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN) || (type == L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN);
                    isL3mc = (L3_ENTRY_L3_MCAST_IPV6_DOUBLE == type) ||(L3_ENTRY_L3_MCAST_IPV6_SINGLE == type) ;
                    osal_memset(l3_entry.entry_data, 0, sizeof(l3_entry));
                    RT_ERR_CHK(table_read(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, addr, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, &field_ipmc_type, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, field_sa, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, field_da, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, &field_vid, (uint32 *)&l3_entry), ret);
                    RT_ERR_CHK(table_field_get(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, LONGAN_L3_HOST_ROUTE_IP6MC_VID_CMPtf, &field_vid_cmp, (uint32 *)&l3_entry), ret);

                    _ipv6_swap(dip_swap, field_da);
                    _ipv6_swap(sip_swap, field_sa);
                    l3_entry_hit = (field_ipmc_type == ipmc_type) &&
                                   ((dip_swap[0] == *da_ptr) && (dip_swap[1] == *(da_ptr+1)) && (dip_swap[2] == *(da_ptr+2)) && (dip_swap[3] == *(da_ptr+3))) &&
                                   (sa_exist ? ((sip_swap[0] == *sa_ptr) && (sip_swap[1] == *(sa_ptr+1)) && (sip_swap[2] == *(sa_ptr+2)) && (sip_swap[3] == *(sa_ptr+3))) :
                                               ((sip_swap[0] == field_sa_all_one[0]) && (sip_swap[1] == field_sa_all_one[1]) && (sip_swap[2] == field_sa_all_one[2]) && (sip_swap[3] == field_sa_all_one[3]))) &&
                                   (!vlan_exist || (field_vid == vid));
                     if (isL3mc)
                     {
                          l3_entry_hit &= (vid_cmp_src == field_vid_cmp);
                     }

                    if (field_vid_cmp) {
                        l3_entry_hit &= ((vid_cmp_src == 1) && (field_vid == vid));
                    } else {    // vid_cmp not to be compared
                        if (mcast_exact_match) {
                            l3_entry_hit &= (!vid_cmp_src || ((field_vid == vid)));
                        }
                    }
                    l3_entry_hit_vid_cmp = l3_entry_hit && (field_vid_cmp && (field_vid == vid));
                    step = 6;
                    RT_ERR_CHK((i % 6), ret);
                } else if (field_entry_type == 0) {
                    step = 1;
                    free_count_continuous = 0;
                } else if (field_entry_type == 1) {
                    step = 2;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 2), ret);
                } else {
                    step = 3;
                    free_count_continuous = 0;
                    RT_ERR_CHK((i % 3), ret);
                }

                break;
            }

           default:
                break;

        }

        if (l3_entry_hit_vid_cmp) { // first select matched entry with vid_cmp
            *index = addr;
            *hit = 1;
            *free = 0;
            *vid_cmp = 1;
            osal_memcpy(p_l3_entry, &l3_entry, sizeof(l3_entry));
            return (RT_ERR_OK);
        } else if (l3_entry_hit) {
            if (j == 0) {
                if (!hit_0) {   // record the first left hit entry
                    hit_0 = 1;
                    l3_entry_hit_addr_0 = addr;
                    osal_memcpy(&l3_entry_hit_0, &l3_entry, sizeof(l3_entry));
                }
            } else {
                if (!hit_1) {    // record the first right hit entry
                    hit_1 = 1;
                    l3_entry_hit_addr_1 = addr;
                    osal_memcpy(&l3_entry_hit_1, &l3_entry, sizeof(l3_entry));
                }
            }
        } else if (l3_entry_free) {
            if (j == 0) {
                l3_hash_free_total_0++; // count left free entry
                if (!free_0) {
                    free_0 = 1;
                    l3_entry_free_addr_0 = addr - (align - 1);
                    osal_memcpy(&l3_entry_free_0, &l3_entry, sizeof(l3_entry)); // use l3_entry with address = l3_entry_free_addr_0 ???
                }
            } else {
                l3_hash_free_total_1++; // count right free entry
                if (!free_1) {
                    free_1 = 1;
                    l3_entry_free_addr_1 = addr - (align - 1);
                    osal_memcpy(&l3_entry_free_1, &l3_entry, sizeof(l3_entry));
                }
            }
        }

        i += step;
    }
}

        if (hit_0) {    // select left hit entry
            *index = l3_entry_hit_addr_0;
            *hit = 1;
            *free = 0;
            *vid_cmp = 0;
            osal_memcpy(p_l3_entry, &l3_entry_hit_0, sizeof(l3_entry_hit_0));
        } else if (hit_1) {    // select right hit entry
            *index = l3_entry_hit_addr_1;
            *hit = 1;
            *free = 0;
            *vid_cmp = 0;
            osal_memcpy(p_l3_entry, &l3_entry_hit_1, sizeof(l3_entry_hit_1));
        } else if (l3_hash_free_total_0 >= l3_hash_free_total_1) {    // select left or right with more free entry
            *index = l3_entry_free_addr_0;
            *hit = 0;
            *free = free_0;
            osal_memcpy(p_l3_entry, &l3_entry_free_0, sizeof(l3_entry_free_0));
        } else {
            *index = l3_entry_free_addr_1;
            *hit = 0;
            *free = free_1;
            osal_memcpy(p_l3_entry, &l3_entry_free_1, sizeof(l3_entry_free_1));
        }


    return (RT_ERR_OK);
}

static int32 _diag_debug_l3_hash_field_get(uint32 *chip_index_ptr, uint32 *sa_ptr, uint32 *da_ptr, uint32 cmp_vid,
    uint32 *vid_ptr, char *field_name_ptr, rtk_diag_l3_entry_type_t type, uint32 *pIdx)
{
    uint32 table_index = 0;
    uint32 field_index = 0;
    int32 ret = RT_ERR_FAILED;
    host_routing_entry_t l3_entry;
    uint32 value[MAX_DEBUG_FIELD_LEN];
    uint32 index = 0;
    char table_name[MAX_TABLE_FIELD_NAME_LEN];
    uint32 hit = 0;
    uint32 free = 0;
    int32 is_vid_cmp = 0;
    int32 vid_cmp_src;
    int32 mcast_exact_match = 1;   // config read/write using extra match

    *pIdx = 0;
    osal_memset(&l3_entry, 0, sizeof(l3_entry));
    osal_memset(value, 0, sizeof(value));
    osal_memset(table_name, 0, sizeof(table_name));

    switch (type) {
        case L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN:
        case L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN:
        case L3_ENTRY_L3_MCAST_IPV4_SINGLE:
        case L3_ENTRY_L3_MCAST_IPV4_DOUBLE: {
            osal_strcpy(table_name, "l3_host_route_ipmc");
            break;
        }

        case L3_ENTRY_L3_UCAST_IPV4: {
            osal_strcpy(table_name, "l3_host_route_ipuc");
            break;
        }

        case L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN:
        case L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN:
        case L3_ENTRY_L3_MCAST_IPV6_SINGLE:
        case L3_ENTRY_L3_MCAST_IPV6_DOUBLE: {
            osal_strcpy(table_name, "l3_host_route_ip6mc");
            break;
        }

        default: {
            osal_strcpy(table_name, "l3_host_route_ip6uc");
            break;
        }
    }

    if ((ret = _diag_table_name2int(*chip_index_ptr, table_name, (int32*)&table_index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_DIAG), "");
        return (RT_ERR_FAILED);
    }

    if ((ret = _diag_table_field_name2int(*chip_index_ptr, table_name, field_name_ptr, (int32*)&field_index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_DIAG), "Not supported table field: %s\n\n", field_name_ptr);
        return (RT_ERR_FAILED);
    }

    vid_cmp_src = cmp_vid;

    ret = _diag_l3_hash_search(*chip_index_ptr, type, sa_ptr, da_ptr, *vid_ptr, &index, &hit, &free, (uint32*)&is_vid_cmp, mcast_exact_match, vid_cmp_src, &l3_entry);

    if (RT_ERR_OK != ret)
    {
        osal_printf("_diag_l3_hash_search error.\n");
        return (ret);
    }
    else if (!hit)
    {
        osal_printf("Not found the specified L3 entry!\n");
        return (RT_ERR_OK);
    }

    if ((ret = table_field_get(*chip_index_ptr, table_index, field_index, &value[0], (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        osal_printf("table_field_get error.\n");
        return (RT_ERR_FAILED);
    }
    *pIdx = index;

    return (RT_ERR_OK);
}


static int32 _diag_debug_l3_hash_field_set(uint32 *chip_index_ptr, void *sa_ptr, void *da_ptr, uint32 vid_cmp,
    uint32 *vid_ptr, char *field_name_ptr, rtk_diag_l3_entry_type_t type, uint32 *value_ptr, uint32 *pIdx)
{
    uint32 table_index = 0;
    uint32 field_index = 0;
    int32 ret = RT_ERR_FAILED;
    host_routing_entry_t l3_entry;
    uint32 index = 0;
    char table_name[MAX_TABLE_FIELD_NAME_LEN];
    uint32 hit = 0;
    uint32 free = 0;
    uint32 sip_all_one[IPV6_ADDR_LEN >> 2] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    uint32 sip_swap[IPV6_ADDR_LEN >> 2];
    uint32 dip_swap[IPV6_ADDR_LEN >> 2];
    uint32 entry_type;
    uint32 ipmc_type;
    uint32 valid = 1;
    int32 is_vid_cmp = 0;
    int32 vid_cmp_src = vid_cmp;
    int32 mcast_exact_match = 1;   // config read/write using extra match

    *pIdx = 0;

    osal_memset(&l3_entry, 0, sizeof(l3_entry));
    osal_memset(table_name, 0, sizeof(table_name));

    switch (type) {
        case L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN:
        case L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN:
        case L3_ENTRY_L3_MCAST_IPV4_SINGLE:
        case L3_ENTRY_L3_MCAST_IPV4_DOUBLE: {
            osal_strcpy(table_name, "l3_host_route_ipmc");
            break;
        }

        case L3_ENTRY_L3_UCAST_IPV4: {
            osal_strcpy(table_name, "l3_host_route_ipuc");
            break;
        }

        case L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN:
        case L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN:
        case L3_ENTRY_L3_MCAST_IPV6_SINGLE:
        case L3_ENTRY_L3_MCAST_IPV6_DOUBLE: {
            osal_strcpy(table_name, "l3_host_route_ip6mc");
            break;
        }

        default: {
            osal_strcpy(table_name, "l3_host_route_ip6uc");
            break;
        }
    }

    if ((ret = _diag_table_name2int(*chip_index_ptr, table_name, (int32*)&table_index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_DIAG), "");
        return (RT_ERR_FAILED);
    }

    if ((ret = _diag_table_field_name2int(*chip_index_ptr, table_name, field_name_ptr, (int32*)&field_index)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_DIAG), "Not supported table field: %s\n\n", field_name_ptr);
        return (RT_ERR_FAILED);
    }

    ret = _diag_l3_hash_search(*chip_index_ptr, type, sa_ptr, da_ptr, *vid_ptr, &index, &hit, &free, (uint32*)&is_vid_cmp, mcast_exact_match, vid_cmp_src, &l3_entry);
    if (RT_ERR_OK != ret)
    {
        osal_printf("_diag_l3_hash_search error.\n");
        return (ret);
    }
    else if (!hit && !free)
    {
        osal_printf("Not found the specified L3 entry and no free entry!\n");
        return (RT_ERR_FAILED);
    }

    switch (type) {
        case L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN: {
            entry_type = 1;
            ipmc_type = 0;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, da_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, sip_all_one, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);

            break;
        }

        case L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN: {
            entry_type = 1;
            ipmc_type = 0;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, da_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, sa_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);
            break;
        }

        case L3_ENTRY_L3_MCAST_IPV4_SINGLE: {
            entry_type = 1;
            ipmc_type = 1;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, da_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, sip_all_one, (uint32 *)&l3_entry), ret);

            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VID_CMPtf, (uint32*)&vid_cmp_src, (uint32 *)&l3_entry), ret);
            break;
        }

        case L3_ENTRY_L3_MCAST_IPV4_DOUBLE: {
            entry_type = 1;
            ipmc_type = 1;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, da_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, sa_ptr, (uint32 *)&l3_entry), ret);

            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPMC_VID_CMPtf, (uint32*)&vid_cmp_src, (uint32 *)&l3_entry), ret);
            break;
        }

        case L3_ENTRY_L3_UCAST_IPV4: {
            entry_type = 0;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPUC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPUC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IPUC_IPtf, da_ptr, (uint32 *)&l3_entry), ret);
            break;
        }

        case L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN: {
            entry_type = 3;
            ipmc_type = 0;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            _ipv6_swap(dip_swap, da_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, dip_swap, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, sip_all_one, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);

            break;
        }

        case L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN: {
            entry_type = 3;
            ipmc_type = 0;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            _ipv6_swap(dip_swap, da_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, dip_swap, (uint32 *)&l3_entry), ret);
            _ipv6_swap(sip_swap, sa_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, sip_swap, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);

            break;
        }

        case L3_ENTRY_L3_MCAST_IPV6_SINGLE: {
            entry_type = 3;
            ipmc_type = 1;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            _ipv6_swap(dip_swap, da_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, dip_swap, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, sip_all_one, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VID_CMPtf, (uint32*)&vid_cmp_src, (uint32 *)&l3_entry), ret);
            break;
        }

        case L3_ENTRY_L3_MCAST_IPV6_DOUBLE: {
            entry_type = 3;
            ipmc_type = 1;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, &ipmc_type, (uint32 *)&l3_entry), ret);
            _ipv6_swap(dip_swap, da_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, dip_swap, (uint32 *)&l3_entry), ret);
            _ipv6_swap(sip_swap, sa_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, sip_swap, (uint32 *)&l3_entry), ret);

            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, vid_ptr, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6MC_VID_CMPtf, (uint32*)&vid_cmp_src, (uint32 *)&l3_entry), ret);
            break;
        }

        default: {
            entry_type = 2;
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6UC_VALIDtf, &valid, (uint32 *)&l3_entry), ret);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6UC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l3_entry), ret);
            _ipv6_swap(dip_swap, da_ptr);
            RT_ERR_CHK(table_field_set(*chip_index_ptr, table_index, LONGAN_L3_HOST_ROUTE_IP6UC_IPtf, dip_swap, (uint32 *)&l3_entry), ret);
            break;
        }
    }
    if ((ret = table_field_set(*chip_index_ptr, table_index, field_index, value_ptr, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        osal_printf("table_field_set");
        return (RT_ERR_FAILED);
    }

    if ((ret = table_write(*chip_index_ptr, table_index, index, (uint32 *)&l3_entry)) != RT_ERR_OK)
    {
        osal_printf("table_write");
        return (RT_ERR_FAILED);
    }

    *pIdx = index;

    return (RT_ERR_OK);
}

/* Module Name    : Diag */

/* Function Name:
 *      dal_longan_diag_table_reg_field_get
 * Description:
 *      Dump the specified registers/table field value in console for debugging
 * Input:
 *      unit - unit id
 *      pCfg - diag config
 * Output:
 *      the specified registers/table field value.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300
 * Note:
 */
int32
dal_longan_diag_table_reg_field_get(uint32 unit, rtk_diag_debug_t *pCfg)
{
    int32 ret = RT_ERR_FAILED;
    uint32 table_reg_index = 0;
    uint32 field_index = 0;
    uint32 data[MAX_DEBUG_ENTRY_LEN];
    uint32 index1;
    uint32 index2;

    /* check Init status */
    RT_INIT_CHK(diag_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    osal_memset(data, 0, sizeof(data));

    DIAG_SEM_LOCK(unit);

    if (pCfg->type == ENTRY_REG) {
        if ((ret = _diag_reg_name2int(unit, pCfg->table_reg_name, (int32*)&table_reg_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported register: %s\n\n", pCfg->table_reg_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((ret = _diag_reg_field_name2int(unit, pCfg->field_name, (int32*)&field_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported register field: %s\n\n", pCfg->field_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((rtk_longan_reg_list[table_reg_index].lport == 0) &&
            (rtk_longan_reg_list[table_reg_index].hport == 0) &&
            (pCfg->table_reg_offset == 0)) {
            index1 = REG_ARRAY_INDEX_NONE;
        } else {
            index1 = pCfg->table_reg_offset;
        }

        if ((rtk_longan_reg_list[table_reg_index].larray == 0) &&
            (rtk_longan_reg_list[table_reg_index].harray == 0) &&
            (pCfg->field_offset == 0)) {
            index2 = REG_ARRAY_INDEX_NONE;
        } else {
            index2 = pCfg->field_offset;
        }

        if ((ret = reg_array_field_read(unit, table_reg_index, index1, index2, field_index, pCfg->value)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }
    } else if (pCfg->type == ENTRY_TABLE) {
        if ((ret = _diag_table_name2int(unit, pCfg->table_reg_name, (int32*)&table_reg_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported table: %s\n\n", pCfg->table_reg_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((ret = _diag_table_field_name2int(unit, pCfg->table_reg_name, pCfg->field_name, (int32*)&field_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported table field: %s\n\n", pCfg->field_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((ret = table_read(unit, table_reg_index, pCfg->table_reg_offset, data)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }
        if (pCfg->value_type == VALUE_MAC) {
            if ((ret = table_field_mac_get(unit, table_reg_index, field_index, (uint8 *)pCfg->value, data)) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
                DIAG_SEM_UNLOCK(unit);
                return (RT_ERR_FAILED);
            }
        }
        else
        {
            if ((ret = table_field_get(unit, table_reg_index, field_index, pCfg->value, data)) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
                DIAG_SEM_UNLOCK(unit);
                return (RT_ERR_FAILED);
            }
        }
    }
    else
    {
        if ((ret = _diag_debug_l3_hash_field_get(&unit, pCfg->sip, pCfg->dip, pCfg->cmp_vid, &pCfg->vid, pCfg->field_name, pCfg->hash_type, &pCfg->table_index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (ret);
        }
    }

    DIAG_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      diag_longan_diag_table_reg_field_set
 * Description:
 *      Dump the specified registers/table field value in console for debugging
 * Input:
 *      unit - unit id
 *      pCfg - diag config
 * Output:
 *      N/A.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300
 * Note:
 */
int32
dal_longan_diag_table_reg_field_set(uint32 unit, rtk_diag_debug_t *pCfg)
{
    int32 ret = RT_ERR_FAILED;
    uint32 table_reg_index = 0;
    uint32 field_index = 0;
    uint32 data[MAX_DEBUG_ENTRY_LEN];
    uint32 index1;
    uint32 index2;

    /* check Init status */
    RT_INIT_CHK(diag_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    DIAG_SEM_LOCK(unit);

    if (pCfg->type == ENTRY_REG) {
        if ((ret = _diag_reg_name2int(unit, pCfg->table_reg_name, (int32*)&table_reg_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported register: %s\n\n", pCfg->table_reg_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((ret = _diag_reg_field_name2int(unit, pCfg->field_name, (int32*)&field_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported register field: %s\n\n", pCfg->field_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((rtk_longan_reg_list[table_reg_index].lport == 0) &&
            (rtk_longan_reg_list[table_reg_index].hport == 0) &&
            (pCfg->table_reg_offset == 0)) {
            index1 = REG_ARRAY_INDEX_NONE;
        } else {
            index1 = pCfg->table_reg_offset;
        }

        if ((rtk_longan_reg_list[table_reg_index].larray == 0) &&
            (rtk_longan_reg_list[table_reg_index].harray == 0) &&
            (pCfg->field_offset == 0)) {
            index2 = REG_ARRAY_INDEX_NONE;
        } else {
            index2 = pCfg->field_offset;
        }

        if ((ret = reg_array_field_write(unit, table_reg_index, index1, index2, field_index, pCfg->value)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }
    } else if (pCfg->type == ENTRY_TABLE) {
        if ((ret = _diag_table_name2int(unit, pCfg->table_reg_name, (int32*)&table_reg_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported table: %s\n\n", pCfg->table_reg_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((ret = _diag_table_field_name2int(unit, pCfg->table_reg_name, pCfg->field_name, (int32*)&field_index)) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "Not supported table field: %s\n\n", pCfg->field_name);
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if ((ret = table_read(unit, table_reg_index, pCfg->table_reg_offset, &data[0])) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }

        if (pCfg->value_type == VALUE_MAC) {
            if ((ret = table_field_mac_set(unit, table_reg_index, field_index, (uint8 *)pCfg->value, &data[0])) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
                DIAG_SEM_UNLOCK(unit);
                return (RT_ERR_FAILED);
            }
        } else if (pCfg->value_type == VALUE_IPV6) {
            uint32 dip_swap[IPV6_ADDR_LEN >> 2];

            _ipv6_swap(dip_swap, pCfg->value);
            if ((ret = table_field_set(unit, table_reg_index, field_index, dip_swap, &data[0])) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
                DIAG_SEM_UNLOCK(unit);
                return (RT_ERR_FAILED);
            }
        } else {
            if ((ret = table_field_set(unit, table_reg_index, field_index, pCfg->value, &data[0])) != RT_ERR_OK)
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
                DIAG_SEM_UNLOCK(unit);
                return (RT_ERR_FAILED);
            }
        }

        if ((ret = table_write(unit, table_reg_index, pCfg->table_reg_offset, &data[0])) != RT_ERR_OK)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (RT_ERR_FAILED);
        }
    }
    else
    {
        if ((ret = _diag_debug_l3_hash_field_set(&unit, pCfg->sip, pCfg->dip, pCfg->cmp_vid, &pCfg->vid, pCfg->field_name, pCfg->hash_type, pCfg->value, &pCfg->table_index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_DIAG), "");
            DIAG_SEM_UNLOCK(unit);
            return (ret);
        }
    }

    DIAG_SEM_UNLOCK(unit);

    return (RT_ERR_OK);
}
#endif

/* Function Name:
 *      dal_longan_diagMapper_init
 * Description:
 *      Hook diag module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook diag module before calling any diag APIs.
 */
int32
dal_longan_diagMapper_init(dal_mapper_t *pMapper)
{
    pMapper->diag_init = dal_longan_diag_init;
    pMapper->diag_table_read = dal_longan_diag_table_read;

#if (defined CONFIG_SDK_APP_DIAG_EXT)
    pMapper->diag_table_reg_field_get = dal_longan_diag_table_reg_field_get;
    pMapper->diag_table_reg_field_set = dal_longan_diag_table_reg_field_set;
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_diag_init
 * Description:
 *      Initialize diagnostic module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_longan_diag_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(diag_init[unit]);
    diag_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    diag_sem[unit] = osal_sem_mutex_create();
    if (0 == diag_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_PORT), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    diag_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}/* end of dal_cypress_port_init */


/* Function Name:
 *      dal_longan_diag_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * 	  pData - pointer buffer for read back table entry
 *      pRev_vaild - used to sure the revbits is vaild.
 * 	  pRevbits - pointer buffer for read reverse bits which are not contain in entry.
 * Output:
 * 	  pData - pointer buffer for read back table entry
 *      pRev_vaild - used to sure the revbits is vaild.
 * 	  pRevbits - pointer buffer for read reverse bits which are not contain in entry.
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 * Note:
 *      1. Basically, this is a transparent API for table read.
 *      2. For L2 hash table, this API will converse the hiding 12-bits,
 *          and provide to upper layer by pRevbits parameter.
 *      3. addr format :
 *          From RTK and realchip view : hash_key[13:2]location[1:0]
 */
int32
dal_longan_diag_table_read(uint32  unit, uint32  table, uint32  addr, uint32  *pData, uint32 *pRev_vaild, uint32 *pRevbits)
{
    int32 ret = RT_ERR_OK;

       /* check Init status */
    RT_INIT_CHK(diag_init[unit]);

    DIAG_SEM_LOCK(unit);

    *pRev_vaild = 0;
    ret = table_read(unit, table, addr, pData);

    DIAG_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_longan_diag_table_read */

