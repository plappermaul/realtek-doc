/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 81667 $
 * $Date: 2017-08-24 20:27:09 +0800 (Thu, 24 Aug 2017) $
 *
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */

/*
 * Include Files
 */
#include <dal/dal_mapper.h>
#include <dal/rtrpc/rtrpc_diag.h>
#include <dal/rtrpc/rtrpc_vlan.h>
#include <dal/rtrpc/rtrpc_l2.h>
#include <dal/rtrpc/rtrpc_l3.h>
#include <dal/rtrpc/rtrpc_tunnel.h>
#include <dal/rtrpc/rtrpc_vxlan.h>
#include <dal/rtrpc/rtrpc_mcast.h>
#include <dal/rtrpc/rtrpc_ipmcast.h>
#include <dal/rtrpc/rtrpc_port.h>
#include <dal/rtrpc/rtrpc_trunk.h>
#include <dal/rtrpc/rtrpc_sec.h>
#include <dal/rtrpc/rtrpc_stp.h>
#include <dal/rtrpc/rtrpc_rate.h>
#include <dal/rtrpc/rtrpc_qos.h>
#include <dal/rtrpc/rtrpc_trap.h>
#include <dal/rtrpc/rtrpc_time.h>
#include <dal/rtrpc/rtrpc_counter.h>
#include <dal/rtrpc/rtrpc_switch.h>
#include <dal/rtrpc/rtrpc_mirror.h>
#include <dal/rtrpc/rtrpc_flowctrl.h>
#include <dal/rtrpc/rtrpc_eee.h>
#include <dal/rtrpc/rtrpc_oam.h>
#include <dal/rtrpc/rtrpc_pie.h>
#include <dal/rtrpc/rtrpc_acl.h>
#include <dal/rtrpc/rtrpc_led.h>
#include <dal/rtrpc/rtrpc_mpls.h>
#include <dal/rtrpc/rtrpc_stack.h>
#include <dal/rtrpc/rtrpc_stp.h>
#include <dal/rtrpc/rtrpc_openflow.h>
#include <dal/rtrpc/rtrpc_bpe.h>
#include <dal/rtrpc/rtrpc_sds.h>

#ifdef CONFIG_RISE

/* Function Name:
 *      dal_rtrpc_init
 * Description:
 *      Initilize DAL RTRPC
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - initialize success
 *      RT_ERR_FAILED - initialize fail
 * Note:
 *      None
 */
int dal_rtrpc_init(uint32 unit)
{
    /* nothing needs to be init here, so simply return OK */
    return RT_ERR_OK;
}
#endif

#ifdef CONFIG_RISE
void
rtrpc_Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->_init = dal_rtrpc_init;

   rtrpc_mirrorMapper_init(pMapper);
   rtrpc_ipmcMapper_init(pMapper);
   //rtrpc_l2_ntfyMapper_init(pMapper);
   rtrpc_bpeMapper_init(pMapper);
   rtrpc_qosMapper_init(pMapper);
   rtrpc_mplsMapper_init(pMapper);
   rtrpc_switchMapper_init(pMapper);
   //rtrpc_uartMapper_init(pMapper);
   //rtrpc_hwpMapper_init(pMapper);
   rtrpc_timeMapper_init(pMapper);
   rtrpc_vlanMapper_init(pMapper);
   rtrpc_oamMapper_init(pMapper);
   //rtrpc_serdesMapper_init(pMapper);
   //rtrpc_debugMapper_init(pMapper);
   rtrpc_trapMapper_init(pMapper);
   rtrpc_secMapper_init(pMapper);
   rtrpc_stackMapper_init(pMapper);
   //rtrpc_nicMapper_init(pMapper);
   rtrpc_aclMapper_init(pMapper);
   rtrpc_sdsMapper_init(pMapper);
   rtrpc_mcastMapper_init(pMapper);
   //rtrpc_counterMapper_init(pMapper);
   rtrpc_l3Mapper_init(pMapper);
   rtrpc_vxlanMapper_init(pMapper);
   rtrpc_flowctrlMapper_init(pMapper);
   rtrpc_portMapper_init(pMapper);
   rtrpc_trunkMapper_init(pMapper);
   rtrpc_ledMapper_init(pMapper);
   rtrpc_stpMapper_init(pMapper);
   rtrpc_pieMapper_init(pMapper);
   rtrpc_ofMapper_init(pMapper);
   rtrpc_rateMapper_init(pMapper);
   rtrpc_diagMapper_init(pMapper);
   rtrpc_l2Mapper_init(pMapper);
   rtrpc_tunnelMapper_init(pMapper);
   //rtrpc_sysMapper_init(pMapper);
   rtrpc_eeeMapper_init(pMapper);
}
#endif

