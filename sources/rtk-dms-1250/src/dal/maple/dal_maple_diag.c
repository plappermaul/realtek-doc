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
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/common/miim.h>
#include <hal/common/halctrl.h>
#include <dal/dal_common.h>
#include <dal/maple/dal_maple_diag.h>
#include <dal/maple/dal_maple_vlan.h>
#include <dal/dal_mgmt.h>
#include <dal/maple/dal_maple_l2.h>
#include <rtk/port.h>
#include <rtk/default.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32               diag_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name    : diagnostic     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_maple_diagMapper_init
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
dal_maple_diagMapper_init(dal_mapper_t *pMapper)
{
    pMapper->diag_init = dal_maple_diag_init;
    pMapper->diag_table_read = dal_maple_diag_table_read;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_diag_init
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
dal_maple_diag_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(diag_init[unit]);
    diag_init[unit] = INIT_NOT_COMPLETED;


    /* set init flag to complete init */
    diag_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}/* end of dal_cypress_port_init */


/* Module Name    : Diag */

/* Function Name:
 *      dal_maple_diag_table_read
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
dal_maple_diag_table_read(uint32  unit, uint32  table, uint32  addr, uint32  *pData, uint32 *pRev_vaild, uint32 *pRevbits)
{
    uint32 l2_entry_index;
    uint32 entry_vailed, ipmc_fmt, entry_vid;
    int32  ret = RT_ERR_OK;
    dal_maple_l2_entry_t l2_enrty;

    if((INT_MAPLE_L2_IP_MC_RTL8380 <= table)&&(INT_MAPLE_L2_UC_RTL8380 >= table))
    {
        ret = dal_maple_l2_getL2EntryfromHash_dump(unit, addr, &l2_enrty, &entry_vailed);
        if(TRUE == entry_vailed)
        {
            osal_printf("\n***Entry(%u) is Valid - ",addr);
            switch(l2_enrty.entry_type)
            {
            case L2_UNICAST:
                osal_printf("TYPE is L2_UNICAST,\n   VID = %d",l2_enrty.unicast.fid);
                break;
            case L2_MULTICAST:
                osal_printf("TYPE is L2_MULTICAST,\n   VID = %d",l2_enrty.l2mcast.rvid);
                break;
            case IP4_MULTICAST:
                ret = dal_maple_l2_ipmcMode_get(unit, &ipmc_fmt);
                if(ipmc_fmt == 0)
                    entry_vid = l2_enrty.ipmcast_mc_ip.rvid;
                else if(ipmc_fmt == 1)
                    entry_vid = l2_enrty.ipmcast_ip_mc_sip.rvid;
                else
                    entry_vid = l2_enrty.ipmcast_ip_mc.rvid;
                osal_printf("TYPE is IP4_MULTICAST,\n   VID = %u",entry_vid);
                break;
            case IP6_MULTICAST:
                ret = dal_maple_l2_ipmcMode_get(unit, &ipmc_fmt);
                if(ipmc_fmt == 0)
                    entry_vid = l2_enrty.ipmcast_mc_ip.rvid;
                else if(ipmc_fmt == 1)
                    entry_vid = l2_enrty.ipmcast_ip_mc_sip.rvid;
                else
                    entry_vid = l2_enrty.ipmcast_ip_mc.rvid;
                osal_printf("TYPE is IP6_MULTICAST,\n   VID = %u",entry_vid);
                break;
            default:
                osal_printf("TYPE Error\n");
            }
            l2_entry_index = addr;
            ret = table_read(unit, table, l2_entry_index, pData);
        }else{
            l2_entry_index = addr;
            ret = table_read(unit, table, l2_entry_index, pData);
        }
        return ret;
    }else{
        l2_entry_index = addr;
        ret = table_read(unit, table, l2_entry_index, pData);
        *pRev_vaild = L2ENTRY_REVERSE_INVALID;
    }

    return ret;
} /* end of dal_maple_diag_table_read */


