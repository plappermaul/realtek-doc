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
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <dal/dal_mapper.h>
#include <dal/dal_common.h>
#include <dal/dal_phy.h>
#include <dal/maple/dal_maple_mapper.h>
#include <dal/maple/dal_maple_vlan.h>
#include <dal/maple/dal_maple_l2.h>
#include <dal/maple/dal_maple_l3.h>
#include <dal/maple/dal_maple_port.h>
#include <dal/maple/dal_maple_trunk.h>
#include <dal/maple/dal_maple_sec.h>
#include <dal/maple/dal_maple_stp.h>
#include <dal/maple/dal_maple_rate.h>
#include <dal/maple/dal_maple_qos.h>
#include <dal/maple/dal_maple_trap.h>
#include <dal/maple/dal_maple_stat.h>
#include <dal/maple/dal_maple_switch.h>
#include <dal/maple/dal_maple_mirror.h>
#include <dal/maple/dal_maple_flowctrl.h>
#include <dal/maple/dal_maple_eee.h>
#include <dal/maple/dal_maple_led.h>
#include <dal/maple/dal_maple_time.h>
#include <dal/maple/dal_maple_acl.h>
#include <dal/maple/dal_maple_diag.h>
#include <rtk/default.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Module Name    :  */

/* Function Name:
 *      dal_mapleMapper_init
 * Description:
 *      Initilize DAL of smart switch
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      RTK must call this function before do other kind of action.
 */
void dal_mapleMapper_init(dal_mapper_t *pMapper)
{
    pMapper->_init = dal_maple_init;
#ifndef __BOOTLOADER__
    dal_maple_vlanMapper_init(pMapper);
    dal_maple_l2Mapper_init(pMapper);
    dal_maple_trunkMapper_init(pMapper);
    dal_maple_qosMapper_init(pMapper);
    dal_maple_stpMapper_init(pMapper);
    dal_maple_rateMapper_init(pMapper);
    dal_maple_trapMapper_init(pMapper);
    dal_maple_statMapper_init(pMapper);
    dal_maple_switchMapper_init(pMapper);
    dal_maple_mirrorMapper_init(pMapper);
    dal_maple_flowctrlMapper_init(pMapper);
    dal_maple_eeeMapper_init(pMapper);
    dal_maple_secMapper_init(pMapper);
    dal_maple_l3Mapper_init(pMapper);
    dal_maple_aclMapper_init(pMapper);
    dal_maple_ledMapper_init(pMapper);
    dal_maple_timeMapper_init(pMapper);
    dal_maple_diagMapper_init(pMapper);
#endif
    dal_maple_portMapper_init(pMapper);

}

/* Function Name:
 *      dal_maple_init
 * Description:
 *      Initilize DAL of smart switch
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - initialize success
 *      RT_ERR_FAILED - initialize fail
 * Note:
 *      RTK must call this function before do other kind of action.
 */
int dal_maple_init(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;


#ifndef __BOOTLOADER__
    /* Same initialize sequence as original rtk_init function */
    if ((ret = dal_maple_switch_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_switch_init Failed!");
        return ret;
    }
#endif
    if ((ret = dal_phy_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_phy_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_port_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_port_init Failed!");
        return ret;
    }

#ifndef __BOOTLOADER__
    if ((ret = dal_maple_led_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_led_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_eee_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_eee_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_sec_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_sec_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_trunk_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_trunk_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_qos_init(unit, RTK_DEFAULT_QOS_QUEUE_NUMBER)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_qos_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_rate_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_rate_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_flowctrl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_flowctrl_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_vlan_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_vlan_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_stp_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_stp_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_l2_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_l2_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_l3_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_l3_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_time_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_time_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_trap_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_trap_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_mirror_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_mirror_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_stat_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_stat_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_acl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_acl_init Failed!");
        return ret;
    }

    if ((ret = dal_maple_diag_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_maple_diag_init Failed!");
        return ret;
    }

#endif //__BOOTLOADER__

    return RT_ERR_OK;

} /* end of dal_maple_init */
