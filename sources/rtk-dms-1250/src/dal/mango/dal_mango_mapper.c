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
#include <dal/mango/dal_mango_mapper.h>
#include <dal/mango/dal_mango_common.h>
#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_tunnel.h>
#include <dal/mango/dal_mango_vxlan.h>
#include <dal/mango/dal_mango_mcast.h>
#include <dal/mango/dal_mango_ipmcast.h>
#include <dal/mango/dal_mango_port.h>
#include <dal/mango/dal_mango_trunk.h>
#include <dal/mango/dal_mango_sec.h>
#include <dal/mango/dal_mango_stp.h>
#include <dal/mango/dal_mango_rate.h>
#include <dal/mango/dal_mango_qos.h>
#include <dal/mango/dal_mango_trap.h>
#include <dal/mango/dal_mango_stat.h>
#include <dal/mango/dal_mango_switch.h>
#include <dal/mango/dal_mango_mirror.h>
#include <dal/mango/dal_mango_flowctrl.h>
#include <dal/mango/dal_mango_eee.h>
#include <dal/mango/dal_mango_oam.h>
#include <dal/mango/dal_mango_pie.h>
#include <dal/mango/dal_mango_acl.h>
#include <dal/mango/dal_mango_led.h>
#include <dal/mango/dal_mango_mpls.h>
#include <dal/mango/dal_mango_stack.h>
#include <dal/mango/dal_mango_openflow.h>
#include <dal/mango/dal_mango_bpe.h>
#include <dal/mango/dal_mango_diag.h>
#include <dal/mango/dal_mango_sds.h>

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
 *      dal_mangoMapper_init
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
void dal_mangoMapper_init(dal_mapper_t *pMapper)
{
    pMapper->_init = dal_mango_init;
#ifndef __BOOTLOADER__
    dal_mango_vlanMapper_init(pMapper);
    dal_mango_l2Mapper_init(pMapper);
    dal_mango_trunkMapper_init(pMapper);
    dal_mango_qosMapper_init(pMapper);
    dal_mango_stpMapper_init(pMapper);
    dal_mango_rateMapper_init(pMapper);
    dal_mango_trapMapper_init(pMapper);
    dal_mango_statMapper_init(pMapper);
    dal_mango_switchMapper_init(pMapper);
    dal_mango_mirrorMapper_init(pMapper);
    dal_mango_flowctrlMapper_init(pMapper);
    dal_mango_eeeMapper_init(pMapper);
    dal_mango_secMapper_init(pMapper);
    dal_mango_l3Mapper_init(pMapper);
    dal_mango_tunnelMapper_init(pMapper);
    dal_mango_vxlanMapper_init(pMapper);
    dal_mango_ipmcMapper_init(pMapper);
    dal_mango_mcastMapper_init(pMapper);
    dal_mango_oamMapper_init(pMapper);
    dal_mango_pieMapper_init(pMapper);
    dal_mango_aclMapper_init(pMapper);
    dal_mango_ledMapper_init(pMapper);
    dal_mango_ofMapper_init(pMapper);
    dal_mango_mplsMapper_init(pMapper);
    dal_mango_stackMapper_init(pMapper);
    dal_mango_bpeMapper_init(pMapper);
    dal_mango_diagMapper_init(pMapper);
    dal_mango_sdsMapper_init(pMapper);
#endif
    dal_mango_portMapper_init(pMapper);
}

/* Function Name:
 *      dal_mango_init
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
int dal_mango_init(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;

#ifndef __BOOTLOADER__
    /* Same initialize sequence as original rtk_init function */
    if ((ret = dal_mango_common_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_common_init Failed!");
        return ret;
    }


    if ((ret = dal_mango_switch_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_switch_init Failed!");
        return ret;
    }
#endif  /* __BOOTLOADER__ */
    if ((ret = dal_phy_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_phy_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_port_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_port_init Failed!");
        return ret;
    }

#ifndef __BOOTLOADER__
    if ((ret = dal_mango_stack_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_stack_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_led_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_led_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_eee_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_eee_init Failed!");
        return ret;
    }


    if ((ret = dal_mango_sec_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_sec_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_trunk_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_trunk_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_qos_init(unit, RTK_DEFAULT_QOS_QUEUE_NUMBER)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_qos_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_rate_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_rate_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_flowctrl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_flowctrl_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_vlan_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_vlan_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_stp_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_stp_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_l2_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_l2_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_l3_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_l3_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_tunnel_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_tunnel_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_mcast_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_mcast_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_ipmc_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_ipmc_init Failed!");
        return ret;
    }

#if CODE_TBC
    if ((ret = dal_mango_time_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_time_init Failed!");
        return ret;
    }
#endif

    if ((ret = dal_mango_trap_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_trap_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_mirror_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_mirror_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_stat_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_stat_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_oam_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_oam_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_pie_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_pie_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_acl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_acl_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_mpls_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_mpls_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_of_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_openflow_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_vxlan_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_vxlan_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_bpe_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_bpe_init Failed!");
        return ret;
    }

    if ((ret = dal_mango_diag_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_mango_diag_init Failed!");
        return ret;
    }
#endif  /* __BOOTLOADER__ */

    return RT_ERR_OK;
} /* end of dal_mango_init */

