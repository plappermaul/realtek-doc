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
#include <dal/longan/dal_longan_mapper.h>
#include <dal/longan/dal_longan_common.h>
#include <dal/longan/dal_longan_vlan.h>
#include <dal/longan/dal_longan_l2.h>
#include <dal/longan/dal_longan_trunk.h>
#include <dal/longan/dal_longan_oam.h>
#include <dal/longan/dal_longan_mirror.h>
#include <dal/longan/dal_longan_stat.h>
#include <dal/longan/dal_longan_sec.h>
#include <dal/longan/dal_longan_l3.h>
#include <dal/longan/dal_longan_ipmcast.h>
#include <dal/longan/dal_longan_mcast.h>
#include <dal/longan/dal_longan_port.h>
#include <dal/longan/dal_longan_stp.h>
#include <dal/longan/dal_longan_trap.h>
#include <dal/longan/dal_longan_switch.h>
#include <dal/longan/dal_longan_diag.h>
#include <dal/longan/dal_longan_led.h>
#include <dal/longan/dal_longan_pie.h>
#include <dal/longan/dal_longan_acl.h>
#include <dal/longan/dal_longan_flowctrl.h>
#include <dal/longan/dal_longan_rate.h>
#include <dal/longan/dal_longan_qos.h>
#include <dal/longan/dal_longan_eee.h>
#include <dal/longan/dal_longan_stack.h>
#include <dal/longan/dal_longan_sds.h>
#if CODE_TBC
#include <dal/longan/dal_longan_eee.h>
#include <dal/longan/dal_longan_time.h>
#include <dal/longan/dal_longan_mpls.h>
#endif
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
 *      dal_longanMapper_init
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
void dal_longanMapper_init(dal_mapper_t *pMapper)
{
    pMapper->_init = dal_longan_init;
#ifndef __BOOTLOADER__
    dal_longan_vlanMapper_init(pMapper);
    dal_longan_qosMapper_init(pMapper);
    dal_longan_stpMapper_init(pMapper);
    dal_longan_rateMapper_init(pMapper);
    dal_longan_trapMapper_init(pMapper);
    dal_longan_statMapper_init(pMapper);
    dal_longan_switchMapper_init(pMapper);
    dal_longan_mirrorMapper_init(pMapper);
    dal_longan_flowctrlMapper_init(pMapper);
    dal_longan_eeeMapper_init(pMapper);
    dal_longan_secMapper_init(pMapper);
    dal_longan_l3Mapper_init(pMapper);
    dal_longan_ipmcMapper_init(pMapper);
    dal_longan_mcastMapper_init(pMapper);
    dal_longan_oamMapper_init(pMapper);
    dal_longan_pieMapper_init(pMapper);
    dal_longan_aclMapper_init(pMapper);
    dal_longan_ledMapper_init(pMapper);
    dal_longan_diagMapper_init(pMapper);
    dal_longan_sdsMapper_init(pMapper);
#endif
    dal_longan_l2Mapper_init(pMapper);
    dal_longan_trunkMapper_init(pMapper);
    dal_longan_stackMapper_init(pMapper);
    dal_longan_portMapper_init(pMapper);

}

/* Function Name:
 *      dal_longan_init
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
int dal_longan_init(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;

#ifndef __BOOTLOADER__

    /* Same initialize sequence as original rtk_init function */
    if ((ret = dal_longan_common_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_common_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_switch_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_switch_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_led_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_led_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_sec_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_sec_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_qos_init(unit, RTK_DEFAULT_QOS_QUEUE_NUMBER)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_qos_init Failed!");
        return ret;
    }
    if ((ret = dal_longan_rate_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_rate_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_vlan_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_vlan_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_stp_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_stp_init Failed!");
        return ret;
    }

  #if CODE_TBC
    if ((ret = dal_longan_time_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_time_init Failed!");
        return ret;
    }
  #endif

    if ((ret = dal_longan_trap_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_trap_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_mirror_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_mirror_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_stat_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_stat_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_oam_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_oam_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_acl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_acl_init Failed!");
        return ret;
    }
    if ((ret = dal_longan_pie_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_pie_init Failed!");
        return ret;
    }


  #if CODE_TBC
    if ((ret = dal_longan_mpls_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_mpls_init Failed!");
        return ret;
    }
  #endif

    if ((ret = dal_longan_diag_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_diag_init Failed!");
        return ret;
    }

#endif //__BOOTLOADER__

    if ((ret = dal_longan_sds_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_sds_init Failed!");
        return ret;
    }

    if ((ret = dal_phy_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_phy_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_flowctrl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_flowctrl_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_port_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_port_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_l2_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_l2_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_eee_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_eee_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_trunk_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_trunk_init Failed!");
        return ret;
    }

#ifndef __BOOTLOADER__
    if ((ret = dal_longan_l3_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_l3_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_mcast_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_mcast_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_ipmc_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_ipmc_init Failed!");
        return ret;
    }
#endif

    if ((ret = dal_longan_stack_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_stack_init Failed!");
        return ret;
    }

    if ((ret = dal_longan_stack_cascadeMode_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_longan_stack_cascadeMode_init Failed!");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_longan_init */


