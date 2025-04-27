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
 * Purpose : Definition those public diagnostic routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Remote/Local Loopback
 *           (2) RTCT
 *           (3) Dump whole registers and tables
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/type.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/diag.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/chip.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <hal/mac/drv/drv.h>
#include <hal/mac/reg.h>
#include <osal/print.h>
#include <osal/memory.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#if defined(CONFIG_SDK_RTL9310)
#include <dal/mango/dal_mango_l2.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#endif
#if defined(CONFIG_SDK_RTL9300)
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#endif

/*
 * Symbol Definition
 */
typedef enum rtk_diag_rtRsvd_type_e {
    DIAG_RSVD = 0x1,
    DIAG_SIZE = 0x2,
    DIAG_TRANSLATE = 0x4,
    DIAG_END
} rtk_diag_rtRsvd_type_t;

typedef void (*rtk_diag_rtRsvd_act_func)(uint32 unit, uint32 *pVal);
typedef struct rtk_diag_rtRsvd_tbl_s
{
    rtk_diag_rtRsvd_act_func func;
    uint32                   index;
    rtk_diag_rtRsvd_type_t   type;
} rtk_diag_rtRsvd_tbl_t;

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

#if defined(CONFIG_SDK_RTL9310)
void _rtk_rtL2Tbl_translate(uint32 unit, uint32 *pVal)
{
    uint32 index_l = *pVal;
    uint32 index_p = 0;

    _dal_mango_l2_tablePhyIndex_get(index_l, &index_p);
    *pVal = index_p;
}

void _rtk_rtLpmTbl_size(uint32 unit, uint32 *pVal)
{
    if ((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID))
        *pVal = 768;
}

void _rtk_rtAclTbl_size(uint32 unit, uint32 *pVal)
{
    if ((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID))
        *pVal = 2048;
}

void _rtk_rtL2Tbl_size(uint32 unit, uint32 *pVal)
{
    if ((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID))
        *pVal = 16384;
}
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
void _rtk_rtRsvdTbl_show(uint32 unit, uint32 *pVal)
{
    uint32 index = *pVal;
    osal_printf("\nTable Index               = %u\n", index);
    osal_printf("Table Reserved\n");
}
#endif

/*************************************************************************************************
 *
 * Reserved Table Define
 *
 *************************************************************************************************/

#if defined(CONFIG_SDK_RTL9300)
const rtk_diag_rtRsvd_tbl_t rtk9300RsvdTbl[] =
{
    {_rtk_rtRsvdTbl_show,             LONGAN_L2_CAM_MCt,            DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             LONGAN_L2_CAM_UCt,            DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             LONGAN_L3_PREFIX_ROUTE_IP6MCt, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             LONGAN_L3_PREFIX_ROUTE_IPMCt, DIAG_RSVD},
    {NULL, -1, DIAG_END}
};
#endif

#if defined(CONFIG_SDK_RTL9310)
const rtk_diag_rtRsvd_tbl_t rtk9310RsvdTbl[] =
{
    {_rtk_rtL2Tbl_translate,          MANGO_L2_UCt,                 DIAG_TRANSLATE},
    {_rtk_rtL2Tbl_translate,          MANGO_L2_MCt,                 DIAG_TRANSLATE},
    {_rtk_rtL2Tbl_size,               MANGO_L2_UCt,                 DIAG_SIZE},
    {_rtk_rtL2Tbl_size,               MANGO_L2_MCt,                 DIAG_SIZE},
    {_rtk_rtRsvdTbl_show,             MANGO_L2_CAM_CB_MCt,          DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_L2_CAM_CB_UCt,          DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_L2_CAM_MCt,             DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_L2_CAM_UCt,             DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IP6MC_0t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IP6MC_1t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IP6MC_2t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IP6MC_3t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IP6MC_4t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IP6MC_5t, DIAG_RSVD},
    {_rtk_rtLpmTbl_size,              MANGO_L3_PREFIX_ROUTE_IP6UC_0t, DIAG_SIZE},
    {_rtk_rtLpmTbl_size,              MANGO_L3_PREFIX_ROUTE_IP6UC_1t, DIAG_SIZE},
    {_rtk_rtLpmTbl_size,              MANGO_L3_PREFIX_ROUTE_IP6UC_2t, DIAG_SIZE},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IPMC_0t,  DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,              MANGO_L3_PREFIX_ROUTE_IPMC_1t,  DIAG_RSVD},
    {_rtk_rtLpmTbl_size,              MANGO_L3_PREFIX_ROUTE_IPUCt,    DIAG_SIZE},
    {_rtk_rtAclTbl_size,              MANGO_EACLt,                  DIAG_SIZE},
    {_rtk_rtAclTbl_size,              MANGO_IACLt,                  DIAG_SIZE},
    {_rtk_rtAclTbl_size,              MANGO_VACLt,                  DIAG_SIZE},
    {_rtk_rtRsvdTbl_show,             MANGO_FT_L2_CAM_FMT0_0t,      DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_FT_L2_CAM_FMT0_1t,      DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_FT_L2_CAM_FMT1_0t,      DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_FT_L2_CAM_FMT1_1t,      DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_FT_L2_CAM_FMT2_0t,      DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_BSSIDt,                 DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_BSSID_CAMt,             DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_BSSID_LISTt,            DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_BSSID_TUNNEL_STARTt,    DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_CAPWAP_TUNNEL_START_0t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_CAPWAP_TUNNEL_START_1t, DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_WLC_MCASTt,             DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_WLC_MCAST_CAMt,         DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_WLC_UCASTt,             DIAG_RSVD},
    {_rtk_rtRsvdTbl_show,             MANGO_WLC_UCAST_CAMt,         DIAG_RSVD},
    {NULL, -1, DIAG_END}
};
#endif

typedef struct rtk_family_rsvdTbl_map_s
{
    uint32 family_id;
    const rtk_diag_rtRsvd_tbl_t *rsvd_tbl;
}rtk_family_rsvdTbl_map_t;

rtk_family_rsvdTbl_map_t rtk_mac_rsvdTbl[]=
{
    #if defined(CONFIG_SDK_RTL9300)
    {RTL9300_FAMILY_ID, rtk9300RsvdTbl},
    #endif
    #if defined(CONFIG_SDK_RTL9310)
    {RTL9310_FAMILY_ID, rtk9310RsvdTbl},
    #endif
};



/* Module Name    : diagnostic */
/* Sub-module Name: Global     */

/* Function Name:
 *      rtk_diag_init
 * Description:
 *      Initialize diagnostic module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Module must be initialized before using all of APIs in this module
 * Changes:
 *      None
 */
int32
rtk_diag_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->diag_init(unit);
} /* end of rtk_diag_init */

/* Module Name    : Diag */
/* Sub-module Name: RTCT */

/* Function Name:
 *      rtk_diag_table_whole_read
 * Description:
 *      Dump whole table content in console for debugging
 * Input:
 *      unit        - unit id
 *      table_index - dumped table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - dumped table index is out of range
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_table_whole_read(uint32 unit, uint32 table_index)
{
    rtk_table_t *pDump_table = NULL;
    uint32 *pDump_data_buf = NULL;
    uint32 dump_table_loop_index, dump_entry_loop_index;
    uint32 dump_entry_size,total_table_num;
    uint32 reverse_valid;
    uint32 reverse_data;
    uint32 ret;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32              rsvIdx = 0;
#endif
    uint32              rsvVal = 0;
    #if defined(CONFIG_SDK_RTL9300)
    uint32 mulHsaEn = DISABLED, mulHsbEn = DISABLED;
    #endif
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    char   table_name[TBL_NAME_LEN];
#endif
    total_table_num = HAL_GET_MAX_TABLE_IDX(unit);
    osal_printf("\nTotal Table Number = %u",total_table_num);

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    if(table_index != 0xff)    /*Just dump specific table*/
        RT_PARAM_CHK((table_index >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    for (dump_table_loop_index = 0; dump_table_loop_index < total_table_num; dump_table_loop_index++)
    {
        if(table_index != 0xff) /*Just dump specific table*/
        {
            dump_table_loop_index = table_index;
            total_table_num = dump_table_loop_index;
        }

        #if defined(CONFIG_SDK_RTL9300)
        if (HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)
        {
            if ((total_table_num-1) == dump_table_loop_index)   //THE last table --HSA table
            {
                RT_ERR_CHK(reg_array_field_read(unit, LONGAN_TEST_MODE_ALE_HSB_MULTI_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_ENf, &mulHsbEn), ret);
                RT_ERR_CHK(reg_array_field_read(unit, LONGAN_TEST_MODE_ALE_HSA_MULTI_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_ENf, &mulHsaEn), ret);
                if ((ENABLED != mulHsbEn) ||  (ENABLED != mulHsaEn))   //this table should multiple mode enable
                    continue;
            }
        }
        #endif

        pDump_table = table_find(unit, dump_table_loop_index);
        rsvVal = pDump_table->size;

#if defined(CONFIG_SDK_RTL9300)
        rsvIdx = 0;
        if (HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)
        {
            while (NULL != rtk9300RsvdTbl[rsvIdx].func)
            {
                if (table_index != rtk9300RsvdTbl[rsvIdx].index ||
                        DIAG_TRANSLATE == rtk9300RsvdTbl[rsvIdx].type)
                {
                    rsvIdx++;
                    continue;
                }

                if (DIAG_SIZE == rtk9300RsvdTbl[rsvIdx].type)
                {
                    rtk9300RsvdTbl[rsvIdx].func(unit, &rsvVal);
                    break;
                }
                else if (DIAG_RSVD == rtk9300RsvdTbl[rsvIdx].type)
                {
                    rtk9300RsvdTbl[rsvIdx].func(unit, &table_index);
                    return RT_ERR_OK;
                }
            }
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        rsvIdx = 0;
        if (HWP_CHIP_FAMILY_ID(unit)==RTL9310_FAMILY_ID)
        {
            while (NULL != rtk9310RsvdTbl[rsvIdx].func)
            {
                if (table_index != rtk9310RsvdTbl[rsvIdx].index ||
                        DIAG_TRANSLATE == rtk9310RsvdTbl[rsvIdx].type)
                {
                    rsvIdx++;
                    continue;
                }

                if (DIAG_SIZE == rtk9310RsvdTbl[rsvIdx].type)
                {
                    rtk9310RsvdTbl[rsvIdx].func(unit, &rsvVal);
                    break;
                }
                else if (DIAG_RSVD == rtk9310RsvdTbl[rsvIdx].type)
                {
                    rtk9310RsvdTbl[rsvIdx].func(unit, &table_index);
                    return RT_ERR_OK;
                }
            }
        }
#endif

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        osal_memset(table_name, 0, TBL_NAME_LEN);
        ret = table_name_get(unit, dump_table_loop_index, table_name);
        if(ret == RT_ERR_OK)
        {
            osal_printf("\nTable Type(Phyical ID)  = %d, [%s]",pDump_table->type,table_name);
        }else{
            osal_printf("\nTable Type(Phyical ID)  = %d",pDump_table->type);
        }
#else
        osal_printf("\nTable Type(Phyical ID)  = %u",pDump_table->type);
#endif
        osal_printf("\nTable size              = %u",rsvVal);
        osal_printf("\nTable datareg_num       = %u",pDump_table->datareg_num);
        osal_printf("\nTable field_num         = %u\n",pDump_table->field_num);
        osal_printf("\nTable Index               = %u\n",dump_table_loop_index);

        if(table_index == 0xff) /*Dump whole tables*/
        {
            if(HWP_8390_50_FAMILY(unit))
                if((dump_table_loop_index > 4)&&(dump_table_loop_index < 10))
                    continue;
            if(HWP_8380_30_FAMILY(unit))
                if((dump_table_loop_index > 5)&&(dump_table_loop_index < 11))
                    continue;
        }

        pDump_data_buf = (uint32 *)osal_alloc((uint32)(sizeof(uint32)*pDump_table->datareg_num));
        if(pDump_data_buf == NULL)
        {
            return RT_ERR_FAILED;
        }

        for (dump_entry_loop_index = 0; dump_entry_loop_index < (pDump_table->size); dump_entry_loop_index++)
        {
            osal_memset(pDump_data_buf, 0, (sizeof(uint32)*pDump_table->datareg_num));
            reverse_valid = REVERSE_BIT_INVALID;
            reverse_data = REVERSE_BIT_INVALID;
            if ((ret = RT_MAPPER(unit)->diag_table_read(unit, dump_table_loop_index, dump_entry_loop_index, pDump_data_buf, &reverse_valid, &reverse_data)) != RT_ERR_OK)
            {
                osal_printf("\nRead table failed., ERROR Code = %x\n", ret);
                osal_free((void *)pDump_data_buf);
                return RT_ERR_FAILED;
            }
            osal_printf("\nEntry(%05u) ",dump_entry_loop_index);

            /*Display by Data Reg*/
            for (dump_entry_size = 0; dump_entry_size < pDump_table->datareg_num; dump_entry_size++)
            {
                if ((dump_entry_size != 0)&&((dump_entry_size%8) == 0))
                    osal_printf("\n            ");

                osal_printf("-%08x",*(pDump_data_buf+(dump_entry_size)));
            }

        }
        osal_printf("\n");
        osal_free((void *)pDump_data_buf);
    }
    return RT_ERR_OK;
} /* end of rtk_diag_table_whole_read */


/* Function Name:
 *      rtk_diag_tableEntry_read
 * Description:
 *      Dump table entry of a specific range
 * Input:
 *      unit - unit id
 *      table_index - table index number
 *      start_index - entry's start index for dump
 *      end_index - entry's end index for dump
 *      detail - TRUE: display field information; False: summary information.
 * Output:
 *      Display out the information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_diag_tableEntry_read(uint32 unit, uint32 table_index, uint32 start_index, uint32 end_index, uint32 detail)
{
    rtk_table_t         *pDump_table;
    uint32              *pDump_data_buf;
    uint32              *pField_buf = NULL, field_val = 0;
    uint32              ent_idx, field_idx, ent_phy_idx = 0;
    uint32              dump_entry_size, field_len = 0;
    uint32              reverse_valid;
    uint32              reverse_data;
    uint32              rsvVal = 0;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32              rsvIdx = 0;
#endif
    int32               ret;
    #if defined(CONFIG_SDK_RTL9300)
    uint32              mulHsbEn =0, mulHsaEn = 0;
    #endif
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    char                table_name[TBL_NAME_LEN];
#endif

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    RT_PARAM_CHK((table_index >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    #if defined(CONFIG_SDK_RTL9300)
    if (HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)
    {
        if ((HAL_GET_MAX_TABLE_IDX(unit)-1) == table_index)   //THE last table --HSA table
        {
            RT_ERR_CHK(reg_array_field_read(unit, LONGAN_TEST_MODE_ALE_HSB_MULTI_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_ENf, &mulHsbEn), ret);
            RT_ERR_CHK(reg_array_field_read(unit, LONGAN_TEST_MODE_ALE_HSA_MULTI_CTRLr, REG_ARRAY_INDEX_NONE, 0, LONGAN_ENf, &mulHsaEn), ret);
            if ((ENABLED != mulHsbEn) ||  (ENABLED != mulHsaEn))   //this table should multiple mode enable
            {
                osal_printf("Table can read only when the multiple ALE test mode\n");
                return RT_ERR_OK;
            }
        }
    }
    #endif

    pDump_table = table_find(unit, table_index);
    if (pDump_table == NULL)
    {
        osal_printf("\nTable not found\n");
        return RT_ERR_FAILED;
    }

    rsvVal = pDump_table->size;

#if defined(CONFIG_SDK_RTL9300)
    rsvIdx = 0;
    if (HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)
    {
        while (NULL != rtk9300RsvdTbl[rsvIdx].func)
        {
            if (table_index != rtk9300RsvdTbl[rsvIdx].index ||
                    DIAG_TRANSLATE == rtk9300RsvdTbl[rsvIdx].type)
            {
                rsvIdx++;
                continue;
            }

            if (DIAG_SIZE == rtk9300RsvdTbl[rsvIdx].type)
            {
                rtk9300RsvdTbl[rsvIdx].func(unit, &rsvVal);
                break;
            }
            else if (DIAG_RSVD == rtk9300RsvdTbl[rsvIdx].type)
            {
                rtk9300RsvdTbl[rsvIdx].func(unit, &table_index);
                return RT_ERR_OK;
            }
        }
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    rsvIdx = 0;
    if (HWP_CHIP_FAMILY_ID(unit)==RTL9310_FAMILY_ID)
    {
        while (NULL != rtk9310RsvdTbl[rsvIdx].func)
        {
            if (table_index != rtk9310RsvdTbl[rsvIdx].index ||
                    DIAG_TRANSLATE == rtk9310RsvdTbl[rsvIdx].type)
            {
                rsvIdx++;
                continue;
            }

            if (DIAG_SIZE == rtk9310RsvdTbl[rsvIdx].type)
            {
                rtk9310RsvdTbl[rsvIdx].func(unit, &rsvVal);
                break;
            }
            else if (DIAG_RSVD == rtk9310RsvdTbl[rsvIdx].type)
            {
                rtk9310RsvdTbl[rsvIdx].func(unit, &table_index);
                return RT_ERR_OK;
            }
        }
    }
#endif


    osal_printf("\n");
    osal_printf("Table Type(Phyical ID)  = %u", pDump_table->type);
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    osal_memset(&table_name, 0, TBL_NAME_LEN);
    ret = table_name_get(unit, table_index, table_name);
    if(ret == RT_ERR_OK)
    {
        osal_printf(" [%s]", table_name);
    }
    else
    {
        osal_printf(" [N/A]");
    }
#endif
    osal_printf("\n");
    osal_printf("Table size              = %u\n", rsvVal);
    osal_printf("Table datareg_num       = %u\n", pDump_table->datareg_num);
    osal_printf("Table field_num         = %u\n", pDump_table->field_num);
    osal_printf("Table Index             = %u\n", table_index);


    if (start_index >= pDump_table->size)
    {
        start_index = pDump_table->size - 1;
    }

    if (end_index >= pDump_table->size)
    {
        end_index = pDump_table->size - 1;
    }

    /* print title */
    osal_printf(" Index|");
    if (detail)
    {
        for (field_idx = 0; field_idx < pDump_table->field_num; field_idx++)
        {
            if (pDump_table->pFields[field_idx].len > 1)
                osal_printf(" %d:%d|", (pDump_table->pFields[field_idx].lsp + pDump_table->pFields[field_idx].len - 1),
                                   pDump_table->pFields[field_idx].lsp);
            else
                osal_printf(" %d|", pDump_table->pFields[field_idx].lsp);
        }
    }
    osal_printf(" 32bit-format\n");

    pDump_data_buf = (uint32 *)osal_alloc((uint32)(sizeof(uint32)*pDump_table->datareg_num));
    if (pDump_data_buf == NULL)
    {
        osal_printf("\nOut of memory resources for table data\n");
        return RT_ERR_FAILED;
    }

    for (ent_idx = start_index; ent_idx <= end_index; ent_idx++)
    {
        osal_memset(pDump_data_buf, 0, (sizeof(uint32)*pDump_table->datareg_num));

        reverse_valid = REVERSE_BIT_INVALID;
        reverse_data = REVERSE_BIT_INVALID;

        ent_phy_idx = ent_idx;

#if defined(CONFIG_SDK_RTL9310)
        rsvIdx = 0;
        if (HWP_CHIP_FAMILY_ID(unit)==RTL9310_FAMILY_ID)
        {
            while (NULL != rtk9310RsvdTbl[rsvIdx].func)
            {
                if (table_index != rtk9310RsvdTbl[rsvIdx].index ||
                        DIAG_TRANSLATE != rtk9310RsvdTbl[rsvIdx].type)
                {
                    rsvIdx++;
                    continue;
                }

                rtk9310RsvdTbl[rsvIdx].func(unit, &ent_phy_idx);
                break;
            }
        }
#endif

        if ((ret = RT_MAPPER(unit)->diag_table_read(unit, table_index, ent_phy_idx, pDump_data_buf, &reverse_valid, &reverse_data)) != RT_ERR_OK)
        {
            osal_printf("\nRead table failed on entry %u., ERROR Code = %x\n", ent_idx, ret);
            goto out;
        }

        osal_printf(" %u|", ent_idx);
        if (detail)
        {
            for (field_idx = 0; field_idx < pDump_table->field_num; field_idx++)
            {
                if (pDump_table->pFields[field_idx].len <= 32)
                {
                    field_val = 0;
                    ret = table_field_get(unit, table_index, field_idx, &field_val, (uint32 *)pDump_data_buf);
                    if (ret != RT_ERR_OK)
                        osal_printf(" (N/A)|");
                    else
                        osal_printf(" %x|", field_val);
                }
                else
                {
                    if (field_len < pDump_table->pFields[field_idx].len)
                    {
                        /* allocate memory for the field value */
                        if (pField_buf != NULL)
                            osal_free((void *)pField_buf);
                        field_len = ((pDump_table->pFields[field_idx].len + 31) / 32) * 32;
                        pField_buf = (uint32 *)osal_alloc(field_len/8);
                        if ( pField_buf == NULL )
                        {
                            //osal_printf("\nOut of memory resources for field %d\n", field_idx);
                            field_len = 0;
                            osal_printf("(OOM)|");
                            continue;
                        }
                    }

                    osal_memset(pField_buf, 0, (field_len/8));
                    ret = table_field_get(unit, table_index, field_idx, (uint32 *)pField_buf, (uint32 *)pDump_data_buf);
                    if (ret != RT_ERR_OK)
                        osal_printf(" (N/A)|");
                    else
                    {
                        int32       i;
                        for (i = 0; i < (field_len/32); i++)
                            osal_printf(" %08x", pField_buf[i]);
                        osal_printf("|");
                    }
                }
            }/* end for (field_idx ... */
        }/* end if (detail) */

        osal_printf(" ");

        /* Display Data in 32bit format */
        for (dump_entry_size = 0; dump_entry_size < pDump_table->datareg_num; dump_entry_size++)
        {
            osal_printf("-%08x",*(pDump_data_buf+(dump_entry_size)));
        }
        osal_printf("\n");
    }

  out:
    osal_free((void *)pDump_data_buf);
    if (pField_buf)
    {
        osal_free((void *)pField_buf);
    }
    osal_printf("\n");

    return RT_ERR_OK;
}


/* Function Name:
*      _rtk_rsvdTable_change
* Description:
*      Perform actions:
*      DIAG_RSVD - reserve table check
*      DIAG_SIZE - change table size
*      DIAG_TRANSLATE - logical entry index to physical
* Input:
*      unit - unit id
*      rsvdTbl - reserved table
*      tbl - table index
*      pTbl - table info
*      entry - entry index
*      action - action needs to be performed
* Output:
*      pTbl - updated table info
*      entry - translated physical entry index
* Return:
*      RT_ERR_OK - not reserved table or entry index has been translated
* Applicable:
*      8380, 8390, 9300, 9310
* Note:
*      None
* Changes:
*      [SDK_3.0.0]
*          New added function.
*/
int32 _rtk_rsvdTable_change(
    uint32 unit,
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl,
    uint32 tbl,
    uint32 *tbl_size,
    uint32 *entry,
    rtk_diag_rtRsvd_type_t action)
{
    uint32 rsvIdx;
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == rsvdTbl), RT_ERR_NULL_POINTER);

    rsvIdx = 0;

    while (NULL != rsvdTbl[rsvIdx].func)
    {
      if (tbl == rsvdTbl[rsvIdx].index)
      {
          if((action & DIAG_SIZE) && (rsvdTbl[rsvIdx].type == DIAG_SIZE))
          {    //change table size
               rsvdTbl[rsvIdx].func(unit, tbl_size);
          }
          if((action & DIAG_TRANSLATE) && (rsvdTbl[rsvIdx].type == DIAG_TRANSLATE))
          {   //change logical index to physical index
              rsvdTbl[rsvIdx].func(unit, entry);
          }
          if((action & DIAG_RSVD) && (rsvdTbl[rsvIdx].type == DIAG_RSVD))
          {
              return RT_ERR_INPUT;
          }
      }
      rsvIdx++;
    }

    return RT_ERR_OK;
}

#define DIAG_RSVD_TBL_MODIFY(tbl_size, tbl, entry, entry_phyidx) \
 /* Find reserved table */\
 for(i=0; i < sizeof(rtk_mac_rsvdTbl)/sizeof(rtk_family_rsvdTbl_map_t); i++)\
 {\
     if(HWP_CHIP_FAMILY_ID(unit) == rtk_mac_rsvdTbl[i].family_id)\
     {\
         rsvdTbl = rtk_mac_rsvdTbl[i].rsvd_tbl;\
     }\
 }\
\
 if (rsvdTbl != NULL)\
 {\
     \
     tbl_size = pTbl->size;       \
     ret = _rtk_rsvdTable_change(unit, rsvdTbl, tbl, &tbl_size,\
                                 &entry_phyidx, DIAG_RSVD|DIAG_SIZE|DIAG_TRANSLATE);\
     if(ret != RT_ERR_OK)\
     {\
         return ret;\
     }\
     if(entry >= tbl_size)\
     {\
         osal_printf("Error entry %d is over %d\n", entry, tbl_size); \
         return RT_ERR_OUT_OF_RANGE;\
     }\
 }


 /* Function Name:
 *      rtk_diag_tableEntryDatareg_write
 * Description:
 *      Write a data reg of a table entry
 * Input:
 *      unit - unit id
 *      table_index - table index number
 *      entry_index - entry's start index for dump
 *      datareg_index - data register index
 *      pData - 32-bit data to be written
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableEntryDatareg_write(
    uint32 unit,
    uint32 table_index,
    uint32 entry_index,
    uint32 datareg_index,
    uint32* pData)
{
    uint32 *value = NULL;
    int ret;
    uint32 i,tbl_size,entry_phyidx = entry_index;
    rtk_table_t *pTbl;
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl = NULL;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    /* Param check */
    pTbl = table_find(unit, table_index);
    if (pTbl == NULL)
    {
        return RT_ERR_FAILED;
    }

    DIAG_RSVD_TBL_MODIFY(tbl_size, table_index, entry_index, entry_phyidx);

    if (datareg_index >= pTbl->datareg_num)
    {
        return RT_ERR_FAILED;
    }

    /* Malloc buffer for value storage */
    value = (uint32 *)osal_alloc(pTbl->datareg_num * (sizeof(uint32)));
    if (value == NULL)
    {
        osal_printf("\nOut of memory resources for table data\n");
        return RT_ERR_FAILED;
    }

    /* Read the table entry */
    osal_memset(value, 0, pTbl->datareg_num * (sizeof(uint32)));
    ret = table_read(unit, table_index, entry_phyidx, value);
    if (ret != RT_ERR_OK)
    {
        goto out;
    }

    /* Write the data register */
    value[pTbl->datareg_num - datareg_index - 1] = *pData;

    ret = table_write(unit, table_index, entry_phyidx, value);
out:
    osal_free(value);
    return ret;
}


/* Function Name:
 *      rtk_diag_tableEntry_write
 * Description:
 *      Write a specific table entry
 * Input:
 *      unit - unit id
 *      table_index - table index number
 *      entry_index - entry's start index for dump
 *      pData       - table entry data
 *      datareg_num - table entry data reg number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableEntry_write(
    uint32 unit,
    uint32 table_index,
    uint32 entry_index,
    uint32 *pData,
    uint32 datareg_num)
{
    rtk_table_t *pTbl = NULL;
    int ret,i;
    uint32 entry_phyidx = entry_index,tbl_size;
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl = NULL;
    RT_PARAM_CHK((pData == NULL), RT_ERR_NULL_POINTER);

    /* Check the data register number */
    pTbl = table_find(unit, table_index);

    DIAG_RSVD_TBL_MODIFY(tbl_size, table_index, entry_index, entry_phyidx);

    if (datareg_num != pTbl->datareg_num)
    {
        return RT_ERR_INPUT;
    }

    /* Write to table */
    ret = table_write(unit, table_index, entry_phyidx, pData);
    return ret;
}

/* Function Name:
 *      rtk_diag_phy_reg_whole_read
 * Description:
 *      Dump all standard registers of PHY of all ports
 * Input:
 *      unit - unit id
 * Output:
 *      all registers value in console.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_phy_reg_whole_read(uint32 unit)
{
    rtk_switch_devInfo_t    *devInfo = NULL;
    uint32                  port,page=0,pData,i;
    rtk_port_phy_reg_t      reg[]={ PHY_REG_CONTROL,
                                    PHY_REG_STATUS,
                                    PHY_REG_IDENTIFIER_1,
                                    PHY_REG_IDENTIFIER_2,
                                    PHY_REG_AN_ADVERTISEMENT,
                                    PHY_REG_AN_LINKPARTNER,
                                    PHY_REG_1000_BASET_CONTROL,
                                    PHY_REG_1000_BASET_STATUS,
                                    PHY_REG_END};
    devInfo = osal_alloc(sizeof(rtk_switch_devInfo_t));
    if (devInfo == NULL)
        return RT_ERR_FAILED;

    osal_memset(devInfo, 0, sizeof(rtk_switch_devInfo_t));
    if (rtk_switch_deviceInfo_get(unit, devInfo) != RT_ERR_OK)
    {
        osal_free(devInfo);
        return RT_ERR_FAILED;
    }

    osal_printf("PHY registers: (format: reg:value,...)\n");

    for (port = 0; port < RTK_MAX_NUM_OF_PORTS; port++)
    {
        if (!RTK_PORTMASK_IS_PORT_SET(devInfo->ether.portmask, port))
            continue;

        osal_printf("Port%2u",port);
        osal_printf("    ");
        i=0;
        while(reg[i]!=PHY_REG_END){
            if( RT_MAPPER(unit)->port_phyReg_get(unit, port, page, reg[i], &pData) != RT_ERR_OK)
            {
                osal_free(devInfo);
                return RT_ERR_FAILED;
            }
            osal_printf("%d:0x%04x,",reg[i],pData);
            i++;
        }
        osal_printf("\n");
    }

    if(devInfo)
        osal_free(devInfo);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_diag_reg_whole_read
 * Description:
 *      Dump all registers' value in console for debugging
 * Input:
 *      unit - unit id
 * Output:
 *      all registers value in console.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_reg_whole_read(uint32 unit)
{
    uint32  dump_reg_loop_index;
    uint32  addr = 0;
    uint32  value = 0;
    uint32  dim1, dim2, field_no_of_word, dim2_max, array_dim;
    int32   ret = RT_ERR_FAILED;
    rtk_reg_info_t  reg_data;
    rtk_reg_info_t  dump_reg_info;
    uint32    bit_offset_size;
    uint32    bit_offset_32unit, dump_loop;
#ifdef CONFIG_SDK_DUMP_REG_WITH_NAME
    char    reg_name[64];
#endif

    osal_memset(&reg_data, 0, sizeof(rtk_reg_info_t));

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    osal_printf("\nMAX_REG_IDX = %u\n",HAL_GET_MAX_REG_IDX(unit));

    /* dump registers of MAC */
    for (dump_reg_loop_index = 0; dump_reg_loop_index < HAL_GET_MAX_REG_IDX(unit); dump_reg_loop_index++)
    {
        if (RT_ERR_OK != reg_info_get(unit, dump_reg_loop_index, &dump_reg_info))
        {
            osal_printf("\nCANNOT get reg info\n");
            return RT_ERR_FAILED;
        }

        if (dump_reg_info.offset >= SWCORE_MEM_SIZE)
            return RT_ERR_FAILED;

#ifdef CONFIG_SDK_DUMP_REG_WITH_NAME
        osal_memset(&reg_name, 0, 64);
        ret = reg_name_get(unit, dump_reg_loop_index, reg_name);
        if (RT_ERR_OK == ret)
        {
            osal_printf("\nREG-Idx(%04d)[%s] bit_offset = %d: ",dump_reg_loop_index, reg_name, dump_reg_info.bit_offset);
        }else{
            osal_printf("\nREG-Idx(%04d) bit_offset = %d: ",dump_reg_loop_index,dump_reg_info.bit_offset);
        }
#else
        osal_printf("\nREG-Idx(%04u) bit_offset = %u: ",dump_reg_loop_index,dump_reg_info.bit_offset);
#endif
        if (dump_reg_info.bit_offset == 0)
        {   /* non register array case */
            RT_PARAM_CHK(ioal_mem32_read(unit, dump_reg_info.offset, &value), ret);
            osal_printf("\nRegister 0x%x : 0x%08x", dump_reg_info.offset, value);
        }
        else if ((dump_reg_info.bit_offset % 32) == 0)
        {   /* register array case (unit: word) */
            if (dump_reg_info.bit_offset == 32)
            {
                for (dim1 = dump_reg_info.lport; dim1 <= dump_reg_info.hport; dim1++)
                {
                    for (dim2 = dump_reg_info.larray; dim2 <= dump_reg_info.harray; dim2++)
                    {
                        if (dump_reg_info.is_PpBlock)
                            addr = dump_reg_info.offset + (dim1 - dump_reg_info.lport) * 0x100 + (dim2 - dump_reg_info.larray) * 0x4;
                        else
                            addr = dump_reg_info.offset + (dim1 - dump_reg_info.lport) * (dump_reg_info.harray - dump_reg_info.larray + 1) * 0x4 + (dim2 - dump_reg_info.larray) * 0x4;
                        RT_PARAM_CHK(ioal_mem32_read(unit, addr, &value), ret);
                        osal_printf("\nRegister 0x%x : 0x%08x", addr, value);
                    }
                }
            }
            else
            {
                /*vv============*/
                bit_offset_size = dump_reg_info.bit_offset;

                for (dim1 = dump_reg_info.lport; dim1 <= dump_reg_info.hport; dim1++)
                {
                    for (dim2 = dump_reg_info.larray; dim2 <= dump_reg_info.harray; dim2++)
                    {
                        bit_offset_32unit = (bit_offset_size / 32);
                        for(dump_loop = 0; dump_loop < bit_offset_32unit; dump_loop++)
                        {
                            if (dump_reg_info.is_PpBlock)
                                addr = dump_reg_info.offset + (dim1 - dump_reg_info.lport) * 0x100 + (dim2 - dump_reg_info.larray) * 0x4;
                            else
                                addr = dump_reg_info.offset + (dim1 - dump_reg_info.lport) * (dump_reg_info.harray - dump_reg_info.larray + 1) * ( bit_offset_32unit * 0x4) + (dim2 - dump_reg_info.larray) * ( bit_offset_32unit * 0x4) + (dump_loop * 4);
                            RT_PARAM_CHK(ioal_mem32_read(unit, addr, &value), ret);
                            osal_printf("\nRegister 0x%x : 0x%08x", addr, value);
                        }
                    }
                }
                /*^^============*/

            }
        }
        else
        {
            /* register array case (unit: bit) */
            field_no_of_word = 32/dump_reg_info.bit_offset;
            if (dump_reg_info.lport != dump_reg_info.hport)
            {
                if (dump_reg_info.larray != dump_reg_info.harray)
                    array_dim = 2;
                else
                    array_dim = 1;
            }
            else
            {
                array_dim = 0;
            }

            osal_printf("port = (%u~%u), array = (%u~%u)",dump_reg_info.lport, dump_reg_info.hport,dump_reg_info.larray,dump_reg_info.harray);
            if (array_dim == 2)
            {
                if ((dump_reg_info.harray-dump_reg_info.larray+1) % field_no_of_word)
                    dim2_max = (dump_reg_info.harray-dump_reg_info.larray+1)/field_no_of_word;
                else
                    dim2_max = (dump_reg_info.harray-dump_reg_info.larray+1)/field_no_of_word -1;
                for (dim1 = dump_reg_info.lport; dim1 <= dump_reg_info.hport; dim1++)
                {
                    osal_printf("\nPort/Index(%u)",dim1);
                    for (dim2 = 0; dim2 <= dim2_max; dim2++)
                    {
                        if (dump_reg_info.is_PpBlock)
                            addr = dump_reg_info.offset + (dim1- dump_reg_info.lport) * 0x100 + (dim2) * 0x4;
                        else
                            addr = dump_reg_info.offset + (dim1 - dump_reg_info.lport) * (dim2_max + 1) * 0x4 + (dim2) * 0x4;
                        RT_PARAM_CHK(ioal_mem32_read(unit, addr, &value), ret);
                        osal_printf("\nRegister 0x%x : 0x%08x", addr, value);
                    }
                }
            }
            else if (array_dim == 1) /*One Dim in Port Index*/
            {
                if ((dump_reg_info.hport-dump_reg_info.lport+1) % field_no_of_word)
                    dim2_max = (dump_reg_info.hport-dump_reg_info.lport+1)/field_no_of_word;
                else
                    dim2_max = (dump_reg_info.hport-dump_reg_info.lport+1)/field_no_of_word -1;

                for (dim2 = 0; dim2 <= dim2_max; dim2++)
                {
                    addr = dump_reg_info.offset + (dim2) * 0x4;
                    RT_PARAM_CHK(ioal_mem32_read(unit, addr, &value), ret);
                    osal_printf("\nRegister 0x%x : 0x%08x", addr, value);
                }
            }
            else
            {
                /*VV============*/
                if (dump_reg_info.larray != dump_reg_info.harray) /*One Dim in Array Index*/
                {
                    if ((dump_reg_info.harray-dump_reg_info.larray+1) % field_no_of_word)
                        dim2_max = (dump_reg_info.harray-dump_reg_info.larray+1)/field_no_of_word;
                    else
                        dim2_max = (dump_reg_info.harray-dump_reg_info.larray+1)/field_no_of_word -1;

                    for (dim2 = 0; dim2 <= dim2_max; dim2++)
                    {
                        addr = dump_reg_info.offset + (dim2) * 0x4;
                        RT_PARAM_CHK(ioal_mem32_read(unit, addr, &value), ret);
                        osal_printf("\nRegister 0x%x : 0x%08x", addr, value);
                    }
                }else
                {
                /*^^============*/
                    osal_printf("\nRegister 0x%x : Unexpected Case\n", dump_reg_info.offset);
                }
            }
        }
        osal_printf("\n");
    }
    return RT_ERR_OK;
} /* end of rtk_diag_reg_whole_read */



/* Function Name:
 *      rtk_diag_peripheral_register_dump
 * Description:
 *      Dump Chip peripheral registers
 * Input:
 *      unit           - unit id
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK        - OK
 *      RT_ERR_FAILED   - Failed
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_peripheral_register_dump(uint32 unit)
{
    int ret;

    ret = drv_swcore_register_dump(unit);

    return ret;
} /* end of rtk_diag_peripheral_register_dump */

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
/* Function Name:
 *      rtk_diag_table_reg_field_get
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
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_diag_table_reg_field_get(uint32 unit, rtk_diag_debug_t *pCfg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pCfg, RT_ERR_NULL_POINTER);

    return RT_MAPPER(unit)->diag_table_reg_field_get(unit, pCfg);
}

/* Function Name:
 *      rtk_diag_table_reg_field_set
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
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_diag_table_reg_field_set(uint32 unit, rtk_diag_debug_t *pCfg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pCfg, RT_ERR_NULL_POINTER);

    return RT_MAPPER(unit)->diag_table_reg_field_set(unit, pCfg);
}
#endif

/* Function Name:
 *      rtk_diag_regArray_get
 * Description:
 *      Get the specified register value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      word_num   - size of pValue
 * Output:
 *      pValue  - pointer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArray_get(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_array_read(unit, reg, index_1, index_2, pValue);
}

/* Function Name:
 *      rtk_diag_regArray_set
 * Description:
 *      Set the specified register value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      pValue  - pointer of value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArray_set(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_array_write(unit, reg, index_1, index_2, pValue);
}

/* Function Name:
 *      rtk_diag_regArrayField_get
 * Description:
 *      Get the specified register-field value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      field   - field id
 * Output:
 *      pValue  - pointer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArrayField_get(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 field, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_array_field_read(unit, reg, index_1, index_2, field, pValue);
}

/* Function Name:
 *      rtk_diag_regArrayField_set
 * Description:
 *      Set the specified register-field value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      field   - field id
 *      pValue  - pointer of value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArrayField_set(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 field, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_array_field_write(unit, reg, index_1, index_2, field, pValue);
}

/* Function Name:
 *      rtk_diag_tableEntry_get
 * Description:
 *      Get the specified table entry in console for debugging
 * Input:
 *      unit  - unit id
 *      table - table id
 *      entry  - entry index
 * Output:
 *      pData - pointer of data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_tableEntry_get(uint32 unit, uint32 table, uint32 addr, uint32 *pData)
{
    uint32 i,entry_phyidx = addr,tbl_size;
    int32 ret;
    rtk_table_t *pTbl = NULL;
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl = NULL;
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    pTbl = table_find(unit, table);
    if (pTbl == NULL)
    {
        return RT_ERR_FAILED;
    }

    DIAG_RSVD_TBL_MODIFY(tbl_size, table, addr, entry_phyidx);

    return table_read(unit, table , entry_phyidx, pData);
}

/* Function Name:
 *      rtk_diag_tableEntry_set
 * Description:
 *      Set the specified table entry in console for debugging
 * Input:
 *      unit  - unit id
 *      table - table id
 *      addr  - address of entry
 *      pData - pointer of data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_tableEntry_set(uint32 unit, uint32 table, uint32 addr, uint32 *pData)
{
    uint32 i,entry_phyidx = addr,tbl_size;
    int32 ret;
    rtk_table_t *pTbl;
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl = NULL;
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    pTbl = table_find(unit, table);
    if (pTbl == NULL)
    {
        return RT_ERR_FAILED;
    }

    DIAG_RSVD_TBL_MODIFY(tbl_size, table, addr, entry_phyidx);

    return table_write(unit, table, entry_phyidx, pData);
}

/* Function Name:
 *      rtk_diag_table_index_name
 * Description:
 *      Dump whole table index mapping name
 * Input:
 *      unit        - unit id
 *      table_index - dumped table index name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - dumped table index is out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_table_index_name(uint32 unit, uint32 table_index)
{
    uint32 dump_table_loop_index;
    uint32 total_table_num;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32 rsvIdx = 0, isRsv = 0;
#endif
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    uint32 ret;
    char   table_name[TBL_NAME_LEN];
#endif
    total_table_num = HAL_GET_MAX_TABLE_IDX(unit);
    osal_printf("\nTotal Table Number = %u",total_table_num);

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    if(table_index != 0xff)    /*Just dump specific table*/
        RT_PARAM_CHK((table_index >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    for (dump_table_loop_index = 0; dump_table_loop_index < total_table_num; dump_table_loop_index++)
    {
        if(table_index != 0xff) /*Just dump specific table*/
        {
            dump_table_loop_index = table_index;
            total_table_num = dump_table_loop_index;
        }

#if defined(CONFIG_SDK_RTL9300)
        rsvIdx = 0;
        isRsv = 0;
        if (HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)
        {
            while (NULL != rtk9300RsvdTbl[rsvIdx].func)
            {
                if (dump_table_loop_index != rtk9300RsvdTbl[rsvIdx].index ||
                        DIAG_TRANSLATE == rtk9300RsvdTbl[rsvIdx].type)
                {
                    rsvIdx++;
                    continue;
                }

                if (DIAG_RSVD == rtk9300RsvdTbl[rsvIdx].type)
                {
                    osal_printf("\nTable : Index(%d) [Table Reserved]", dump_table_loop_index);
                    isRsv = 1;
                    break;
                }

                rsvIdx++;
            }
            if (isRsv)
                continue;
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        rsvIdx = 0;
        isRsv = 0;
        if (HWP_CHIP_FAMILY_ID(unit)==RTL9310_FAMILY_ID)
        {
            while (NULL != rtk9310RsvdTbl[rsvIdx].func)
            {
                if (dump_table_loop_index != rtk9310RsvdTbl[rsvIdx].index ||
                        DIAG_TRANSLATE == rtk9310RsvdTbl[rsvIdx].type)
                {
                    rsvIdx++;
                    continue;
                }

                if (DIAG_RSVD == rtk9310RsvdTbl[rsvIdx].type)
                {
                    osal_printf("\nTable : Index(%d) [Table Reserved]", dump_table_loop_index);
                    isRsv = 1;
                    break;
                }

                rsvIdx++;
            }
            if (isRsv)
                continue;
        }
#endif

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
        osal_memset(table_name, 0, TBL_NAME_LEN);
        ret = table_name_get(unit, dump_table_loop_index, table_name);
        if(ret == RT_ERR_OK)
        {
            osal_printf("\nTable : Index(%d) [%s]",dump_table_loop_index,table_name);
        }else{
            osal_printf("\nTable : Index(%d)",dump_table_loop_index);
        }
#else
        osal_printf("\nTable : Index(%d)",dump_table_loop_index);
#endif
    }
    osal_printf("\n");

    return RT_ERR_OK;
} /* end of rtk_diag_table_index_name */

/* Function Name:
 *      rtk_diag_reg_get
 * Description:
 *      Get the specified register value in for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      word_num - size of pValue
 * Output:
 *      pValue  - pointer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_reg_get(uint32 unit, uint32 reg, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_read(unit, reg, pValue);
}

/* Function Name:
 *      rtk_diag_reg_set
 * Description:
 *      Set the specified register value for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      pValue  - pointer of value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_reg_set(uint32 unit, uint32 reg, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_write(unit, reg, pValue);
}

/* Function Name:
 *      rtk_diag_regField_get
 * Description:
 *      Get the specified register-field value for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      field   - field id
 * Output:
 *      pValue  - pointer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regField_get(uint32 unit, uint32 reg, uint32 field, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_field_read(unit, reg, field, pValue);
}

/* Function Name:
 *      rtk_diag_regField_set
 * Description:
 *      Set the specified register-field value for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      field   - field id
 *      pValue  - pointer of value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regField_set(uint32 unit, uint32 reg, uint32 field, uint32 *pValue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return reg_field_write(unit, reg, field, pValue);
}


#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)

#define RTK_REG_INFO_CP2_DIAG(pReg, pReg_info, reg_idx)      \
    pReg_info->index = reg_idx;                              \
    pReg_info->reg_words = HAL_GET_REG_WORD_NUM(unit, reg_idx);  \
    osal_memcpy(pReg_info->name, pReg->name, REG_NAME_LEN);  \
    pReg_info->offset = pReg->offset;                        \
    pReg_info->field_num = pReg->field_num;                  \
    pReg_info->lport = pReg->lport;                          \
    pReg_info->hport = pReg->hport;                          \
    pReg_info->larray = pReg->larray;                        \
    pReg_info->harray = pReg->harray;


/* Function Name:
 *      _rtk_diag_regIndex_find
 * Description:
 *      Find the RTK reg index by the given name.
 * Input:
 *      unit      - unit id
 *      reg_name  - REG name
 * Output:
 *      pReg_idx  - REG enum index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must fully match
 * Changes:
 *      None
 */
int32
_rtk_diag_regIndex_find(uint32 unit, char *reg_name, uint32 *pReg_idx)
{
    uint32 reg_idx, reg_num_max;
    rtk_reg_t **reg_list;

    /* Get HAL Reg/Table Name list */
    reg_list = HAL_GET_MAC_DRIVER(unit)->pReg_list;
    reg_num_max = HAL_GET_MAX_REG_IDX(unit);

    if (NULL == reg_list)
    {
       return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

     /* If fully match, return REG index */
    for(reg_idx = 0; reg_idx < reg_num_max; reg_idx++)
    {
        if(osal_strcmp(reg_list[reg_idx]->name, reg_name) == 0)
        {
            *pReg_idx = reg_idx;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_INPUT;
}


/* Diag shell/input array index to RTK API array index */
#define DIAG_RTK_REG_ARRAY_IDX(array_idx)   \
    ((array_idx>=0)? array_idx : REG_ARRAY_INDEX_NONE)
#define DIAG_REG_ARRAY_IDX1_SIZE(pReg) \
    (pReg->harray - pReg->larray)
#define DIAG_REG_ARRAY_IDX2_SIZE(pReg) \
    (pReg->hport - pReg->lport)
/* REG ARRAY 1-dimension indexed by index2 */
#define DIAG_IS_REG_ARRAY_1D_IDX2(pReg)  \
    ((DIAG_REG_ARRAY_IDX1_SIZE(pReg) > 0)&&(DIAG_REG_ARRAY_IDX2_SIZE(pReg) == 0))
/* REG ARRAY 1-dimension indexed by index1 */
#define DIAG_IS_REG_ARRAY_1D_IDX1(pReg)  \
    ((DIAG_REG_ARRAY_IDX1_SIZE(pReg) == 0)&&(DIAG_REG_ARRAY_IDX2_SIZE(pReg) > 0) )
/* REG ARRAY 2-dimension */
#define DIAG_IS_REG_ARRAY_2D(pReg)  \
    ((DIAG_REG_ARRAY_IDX1_SIZE(pReg) > 0)&&(DIAG_REG_ARRAY_IDX2_SIZE(pReg) > 0) )
/* Check if a REG ARRAY */
#define DIAG_IS_REG_ARRAY(pReg)  \
    ((DIAG_REG_ARRAY_IDX1_SIZE(pReg) > 0) || (DIAG_REG_ARRAY_IDX2_SIZE(pReg) > 0))
/* Iteration of REG array index 1 */
#define DIAG_REG_ARRAY_ITER_IDX1(pReg, i) \
    for (i= pReg->lport; i<= pReg->hport; i++)
/* Iteration of REG array index 2 */
#define DIAG_REG_ARRAY_ITER_IDX2(pReg, i) \
    for (i= pReg->larray; i<= pReg->harray; i++)


/* pField array index to RTK API field index */
#define DIAG_RTK_REG_FIELD_IDX(pReg, field_idx) \
    pReg->pFields[field_idx].name

#define DIAG_RTK_REG_FIELD_NAME(pReg, field_idx) \
    pReg->pFields[field_idx].field_name


/* Function Name:
 *      rtk_diag_regInfoByStr_get
 * Description:
 *      Find the RTK reg info by the given name.
 * Input:
 *      unit      - unit id
 *      reg_name  - REG name
 * Output:
 *      pReg_info - REG info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must fully match
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regInfoByStr_get(uint32 unit, char *reg_name, rtk_diag_regInfo_t *pReg_info)
{
    int32 reg_idx;
    uint32 reg_num_max;
    rtk_reg_t **reg_list;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == reg_name) || (NULL == pReg_info), RT_ERR_NULL_POINTER);

    reg_list = HAL_GET_MAC_DRIVER(unit)->pReg_list;
    reg_num_max = HAL_GET_MAX_REG_IDX(unit);

    if (NULL == reg_list)
    {
        return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    for(reg_idx = 0; reg_idx < reg_num_max; reg_idx++)
    {
        /* If fully match, return REG information */
        if(osal_strcmp(reg_list[reg_idx]->name, reg_name) == 0)
        {
            RTK_REG_INFO_CP2_DIAG(reg_list[reg_idx], pReg_info, reg_idx);
            return RT_ERR_OK;
        }
    }
    return RT_ERR_INPUT;
}


/* Function Name:
 *      rtk_diag_regFieldInfo_get
 * Description:
 *      Find the RTK reg field info
 * Input:
 *      unit      - unit id
 *      reg       - REG enum index
 *      field     - REG field index
 * Output:
 *      pField_info - REG field info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must fully match
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regFieldInfo_get(uint32 unit, uint32 reg, uint32 field, rtk_diag_regFieldInfo_t *pField_info)
{
    rtk_reg_t **reg_list;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == pField_info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    reg_list = HAL_GET_MAC_DRIVER(unit)->pReg_list;

    if(NULL == reg_list)
    {
        return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    if(field >= reg_list[reg]->field_num)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    pField_info->index = reg_list[reg]->pFields[field].name;
    pField_info->lsp = reg_list[reg]->pFields[field].lsp;
    pField_info->len = reg_list[reg]->pFields[field].len;
    osal_memcpy(pField_info->field_name, reg_list[reg]->pFields[field].field_name, REG_NAME_LEN);
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_diag_regInfoByStr_match
 * Description:
 *      Find the next RTK REG info by matching keyword
 * Input:
 *      unit      - unit id
 *      keyword   - keyword for matching REG name
 *      reg       - starting REG index
 * Output:
 *      pReg_info - REG info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      It performs partially match with the REG name.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regInfoByStr_match(uint32 unit, char *keyword, int32 reg, rtk_diag_regInfo_t *pReg_info)
{
    int32 reg_idx;
    uint32 reg_num_max;
    rtk_reg_t **reg_list;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == keyword) || (NULL == pReg_info), RT_ERR_NULL_POINTER);

    reg_list = HAL_GET_MAC_DRIVER(unit)->pReg_list;
    reg_num_max = HAL_GET_MAX_REG_IDX(unit);

    if (NULL == reg_list)
    {
        return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    for(reg_idx = 0; reg_idx < reg_num_max; reg_idx++)
    {
        /* If partially match, return REG info */
        if ((reg_idx > reg) && (osal_strstr(reg_list[reg_idx]->name, keyword) != NULL))
        {
            RTK_REG_INFO_CP2_DIAG(reg_list[reg_idx], pReg_info, reg_idx);
            return RT_ERR_OK;
        }
    }
    return RT_ERR_INPUT;
}
#endif //#if defined (CONFIG_SDK_DUMP_REG_WITH_NAME)

#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)

#define RTK_TBL_INFO_CP2_DIAG(pTbl_entry, pTbl_info, tbl_idx) \
    pTbl_info->index = tbl_idx;                                  \
    osal_memcpy(pTbl_info->name, pTbl_entry->name, TBL_NAME_LEN);  \
    pTbl_info->type = pTbl_entry->type;                            \
    pTbl_info->size = pTbl_entry->size;                            \
    pTbl_info->datareg_num = pTbl_entry->datareg_num;              \
    pTbl_info->field_num = pTbl_entry->field_num;

/* Function Name:
 *      rtk_diag_tableInfoByStr_get
 * Description:
 *      Find the TBL info by the given name.
 * Input:
 *      unit      - unit id
 *      tbl_name  - TBL name
 * Output:
 *      pTbl_info - TBL info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The given name must be the same with table name otherwise not found.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableInfoByStr_get(uint32 unit, char *tbl_name, rtk_diag_tblInfo_t *pTbl_info)
{
    int32 tbl_idx,i, ret;
    uint32 tbl_num_max, tbl_size;
    rtk_table_t **tbl_list;
    rtk_table_t tbl_entry;
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl = NULL;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == tbl_name) || (NULL == pTbl_info), RT_ERR_NULL_POINTER);

    tbl_list = HAL_GET_MAC_DRIVER(unit)->pTable_list;
    tbl_num_max = HAL_GET_MAX_TABLE_IDX(unit);

    if (NULL == tbl_list)
    {
        return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    /* Find reserved table */
    for(i=0; i < sizeof(rtk_mac_rsvdTbl)/sizeof(rtk_family_rsvdTbl_map_t); i++)
    {
        if(HWP_CHIP_FAMILY_ID(unit) == rtk_mac_rsvdTbl[i].family_id)
        {
            rsvdTbl = rtk_mac_rsvdTbl[i].rsvd_tbl;
        }
    }

    for(tbl_idx = 0; tbl_idx < tbl_num_max; tbl_idx++)
    {
        /* If fully match, return TBL information */
        if(osal_strcmp(tbl_list[tbl_idx]->name, tbl_name) == 0)
        {
            osal_memcpy(&tbl_entry, tbl_list[tbl_idx], sizeof(rtk_table_t));

            /* reserved table check */
            if (rsvdTbl != NULL)
            {
                tbl_size = tbl_entry.size;
                ret = _rtk_rsvdTable_change(unit, rsvdTbl, tbl_idx, &tbl_size,
                                            NULL, DIAG_SIZE|DIAG_RSVD);
                if(ret != RT_ERR_OK)
                {
                    return ret;
                }
                tbl_entry.size = tbl_size;
            }
            RTK_TBL_INFO_CP2_DIAG((&tbl_entry), pTbl_info, tbl_idx);

            return RT_ERR_OK;
        }
    }

    return RT_ERR_INPUT;
}

/* Function Name:
 *      rtk_diag_tableFieldInfo_get
 * Description:
 *      Find the RTK TBL field info
 * Input:
 *      unit      - unit id
 *      tbl       - TBL index
 *      field     - field index
 * Output:
 *      pField_info - TBL field info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *       The given name must be the same with table name otherwise not found.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableFieldInfo_get(uint32 unit, uint32 tbl, uint32 field, rtk_diag_tblFieldInfo_t *pField_info)
{

    rtk_table_t **tbl_list;
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == pField_info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tbl >= HAL_GET_MAX_TABLE_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    tbl_list = HAL_GET_MAC_DRIVER(unit)->pTable_list;

    if(NULL == tbl_list)
    {
        return RT_ERR_DRIVER_NOT_SUPPORTED;
    }

    if(field >= tbl_list[tbl]->field_num)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    pField_info->lsp = tbl_list[tbl]->pFields[field].lsp;
    pField_info->len = tbl_list[tbl]->pFields[field].len;
    osal_memcpy(pField_info->name, tbl_list[tbl]->pFields[field].name, TBL_NAME_LEN);
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_diag_tableInfoByStr_match
 * Description:
 *      Find the next RTK TBL info by matching keyword
 * Input:
 *      unit      - unit id
 *      keyword   - keyword for matching REG name
 *      tbl       - starting TBL index
 * Output:
 *      pReg_info - TBL info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      It performs partially match with the TBL name.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableInfoByStr_match(uint32 unit, char *keyword, int32 tbl, rtk_diag_tblInfo_t *pTbl_info)
{
    int32 tbl_idx, i, ret;
    uint32 tbl_num_max, tbl_size;
    rtk_table_t **tbl_list;
    rtk_table_t tbl_entry;
    const rtk_diag_rtRsvd_tbl_t *rsvdTbl = NULL;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == keyword) || (NULL == pTbl_info), RT_ERR_NULL_POINTER);

    tbl_list = HAL_GET_MAC_DRIVER(unit)->pTable_list;
    tbl_num_max = HAL_GET_MAX_TABLE_IDX(unit);


    /* Find reserved table */
    for(i=0; i < sizeof(rtk_mac_rsvdTbl)/sizeof(rtk_family_rsvdTbl_map_t); i++)
    {
        if(HWP_CHIP_FAMILY_ID(unit) == rtk_mac_rsvdTbl[i].family_id)
        {
            rsvdTbl = rtk_mac_rsvdTbl[i].rsvd_tbl;
        }
    }

    for(tbl_idx = 0; tbl_idx < tbl_num_max; tbl_idx++)
    {
        /* If partially match, return TBL info */
        if ((tbl_idx > tbl) && (osal_strstr(tbl_list[tbl_idx]->name, keyword) != NULL))
        {
            osal_memcpy(&tbl_entry, tbl_list[tbl_idx], sizeof(rtk_table_t));
            /* Reserved table check */
            if (rsvdTbl != NULL)
            {
                tbl_size = tbl_entry.size;
                ret = _rtk_rsvdTable_change(unit, rsvdTbl, tbl_idx, &tbl_size,
                                            NULL, DIAG_SIZE|DIAG_RSVD);
                if(ret != RT_ERR_OK)
                {
                    continue;
                }
                tbl_entry.size = tbl_size;
            }
            RTK_TBL_INFO_CP2_DIAG((&tbl_entry), pTbl_info, tbl_idx);

            return RT_ERR_OK;
        }
    }

    return RT_ERR_INPUT;
}


#endif //#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)

int32
rtk_diag_reg_dump(uint32 unit)
{
#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
      uint32 val;
      int32 ret;
      uint32 reg_idx;
      uint32 field_idx;
      uint32 cpu_port;
      uint32 rtk_reg_num;
      rtk_regField_t *p_fields;
      rtk_reg_t **pReg_list;
    osal_printf("enter rtk_diag_reg_dump\n");
      //unit = buf.sdk_cfg.unit;
      rtk_reg_num = HAL_GET_MAX_REG_IDX(unit);
      pReg_list = HAL_GET_MAC_DRIVER(unit)->pReg_list;
      cpu_port = HWP_CPU_MACID(unit);

      for(reg_idx = 0; reg_idx < rtk_reg_num; reg_idx++)
      {
          p_fields = pReg_list[reg_idx]->pFields;
          osal_printf("%s\n", pReg_list[reg_idx]->name);

          for(field_idx = 0; field_idx < pReg_list[reg_idx]->field_num; field_idx++)
          {
              ret = reg_field_read(unit, reg_idx, p_fields[field_idx].name, &val);
              if(RT_ERR_OK == ret)
                  osal_printf("  %s - %s = 0x%08x\n", pReg_list[reg_idx]->name, \
                          p_fields[field_idx].field_name, val);
              else
                  osal_printf("  %s - %s = fail\n", pReg_list[reg_idx]->name, \
                          p_fields[field_idx].field_name);
          }

          if(pReg_list[reg_idx]->hport == cpu_port)
          {
              for(field_idx = 0; field_idx < pReg_list[reg_idx]->field_num; field_idx++)
              {
                  ret = reg_array_field_read(unit, reg_idx, cpu_port, REG_ARRAY_INDEX_NONE, \
                          p_fields[field_idx].name, &val);
                  if(RT_ERR_OK == ret)
                      osal_printf("  %s - %s[cpuport] = 0x%08x\n", pReg_list[reg_idx]->name, \
                              p_fields[field_idx].field_name, val);
                  else
                      osal_printf("  %s - %s[cpuport] = fail\n", pReg_list[reg_idx]->name, \
                              p_fields[field_idx].field_name);
              }
          }
      }
      return RT_ERR_OK;
#else
      return RT_ERR_OK;
#endif
}

