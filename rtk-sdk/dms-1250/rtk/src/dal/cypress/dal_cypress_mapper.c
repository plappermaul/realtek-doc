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
#include <dal/dal_mgmt.h>
#include <dal/dal_common.h>
#include <dal/dal_phy.h>
#include <dal/cypress/dal_cypress_mapper.h>
#include <dal/cypress/dal_cypress_common.h>
#include <dal/cypress/dal_cypress_vlan.h>
#include <dal/cypress/dal_cypress_l2.h>
#include <dal/cypress/dal_cypress_l3.h>
#include <dal/cypress/dal_cypress_port.h>
#include <dal/cypress/dal_cypress_trunk.h>
#include <dal/cypress/dal_cypress_sec.h>
#include <dal/cypress/dal_cypress_stp.h>
#include <dal/cypress/dal_cypress_rate.h>
#include <dal/cypress/dal_cypress_qos.h>
#include <dal/cypress/dal_cypress_trap.h>
#include <dal/cypress/dal_cypress_stat.h>
#include <dal/cypress/dal_cypress_switch.h>
#include <dal/cypress/dal_cypress_mirror.h>
#include <dal/cypress/dal_cypress_flowctrl.h>
#include <dal/cypress/dal_cypress_eee.h>
#include <dal/cypress/dal_cypress_time.h>
#include <dal/cypress/dal_cypress_oam.h>
#include <dal/cypress/dal_cypress_acl.h>
#include <dal/cypress/dal_cypress_mpls.h>
#include <dal/cypress/dal_cypress_diag.h>
#include <dal/cypress/dal_cypress_led.h>
#include <dal/cypress/dal_cypress_sds.h>
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
 *      dal_cypressMapper_init
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
void dal_cypressMapper_init(dal_mapper_t *pMapper)
{
    pMapper->_init = dal_cypress_init;
#ifndef __BOOTLOADER__
    dal_cypress_vlanMapper_init(pMapper);
    dal_cypress_l2Mapper_init(pMapper);
    dal_cypress_trunkMapper_init(pMapper);
    dal_cypress_qosMapper_init(pMapper);
    dal_cypress_stpMapper_init(pMapper);
    dal_cypress_rateMapper_init(pMapper);
    dal_cypress_trapMapper_init(pMapper);
    dal_cypress_statMapper_init(pMapper);
    dal_cypress_switchMapper_init(pMapper);
    dal_cypress_mirrorMapper_init(pMapper);
    dal_cypress_flowctrlMapper_init(pMapper);
    dal_cypress_eeeMapper_init(pMapper);
    dal_cypress_secMapper_init(pMapper);
    dal_cypress_l3Mapper_init(pMapper);
    dal_cypress_oamMapper_init(pMapper);
    dal_cypress_aclMapper_init(pMapper);
    dal_cypress_ledMapper_init(pMapper);
    dal_cypress_timeMapper_init(pMapper);
    dal_cypress_mplsMapper_init(pMapper);
    dal_cypress_diagMapper_init(pMapper);
#endif
    dal_cypress_portMapper_init(pMapper);

}

/* Function Name:
 *      dal_cypress_init
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
int dal_cypress_init(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;

#ifndef __BOOTLOADER__
    /* Same initialize sequence as original rtk_init function */
    if ((ret = dal_cypress_common_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_common_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_switch_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_switch_init Failed!");
        return ret;
    }
#endif /* __BOOTLOADER__ */

    if ((ret = dal_phy_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_phy_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_port_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_port_init Failed!");
        return ret;
    }

#ifndef __BOOTLOADER__
    if ((ret = dal_cypress_sds_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_sds_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_led_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_led_init Failed!");
        return ret;
    }
#endif/* __BOOTLOADER__ */

    if ((ret = dal_cypress_eee_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_eee_init Failed!");
        return ret;
    }

#ifndef __BOOTLOADER__
    if ((ret = dal_cypress_sec_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_sec_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_trunk_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_trunk_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_qos_init(unit, RTK_DEFAULT_QOS_QUEUE_NUMBER)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_qos_init Failed!");
        return ret;
    }
    if ((ret = dal_cypress_rate_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_rate_init Failed!");
        return ret;
    }
    if ((ret = dal_cypress_flowctrl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_flowctrl_init Failed!");
        return ret;
    }
    if ((ret = dal_cypress_vlan_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_vlan_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_stp_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_stp_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_l2_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_l2_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_l3_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_l3_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_time_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_time_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_trap_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_trap_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_mirror_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_mirror_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_stat_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_stat_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_oam_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_oam_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_acl_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_acl_init Failed!");
        return ret;
    }


    if ((ret = dal_cypress_mpls_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_mpls_init Failed!");
        return ret;
    }

    if ((ret = dal_cypress_diag_init(unit)) != RT_ERR_OK)
    {
        RT_LOG(LOG_MAJOR_ERR, MOD_DAL, "dal_cypress_diag_init Failed!");
        return ret;
    }

#endif //__BOOTLOADER__

    return RT_ERR_OK;

} /* end of dal_cypress_init */

