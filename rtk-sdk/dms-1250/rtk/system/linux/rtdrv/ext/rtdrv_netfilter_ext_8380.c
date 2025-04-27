/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Module.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Module
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>

#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <common/rt_autoconf.h>
#include <common/rt_error.h>
#include <common/debug/mem.h>
#include <osal/print.h>
#include <hal/mac/mem.h>
#include <ioal/mem32.h>
#include <drv/nic/nic.h>
#include <private/drv/nic/nic_diag.h>
#include <drv/watchdog/watchdog.h>
#include <rtdrv/ext/rtdrv_netfilter_ext_8380.h>
#if (defined(CONFIG_SDK_DRIVER_TEST) || defined(CONFIG_SDK_DRIVER_TEST_MODULE))
#include <sdk/sdk_test.h>
#endif
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <rtk/eee.h>
#ifdef CONFIG_SDK_MODEL_MODE
#include <model_comm.h>
#include <tc.h>
#include <virtualmac/vmac_target.h>
#include <osal/time.h>
#endif

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
/* Function Name:
 *      do_rtdrv_ext_set_ctl
 * Description:
 *      This function is called whenever a process tries to do setsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_ext_set_ctl(struct sock *sk, int cmd, void *user_in, unsigned int len)
{
//    void                *user = (char *)user_in + sizeof(rtdrv_msgHdr_t);
    int32   ret = RT_ERR_FAILED;
#ifdef CONFIG_SDK_RTL8380
#ifdef CONFIG_SDK_MODEL_MODE
    rtdrv_ext_union_t   buf;
#endif

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
    /** PORT **/
    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
    /** TRUNK **/
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
    /** SVLAN **/
    /** SWITCH **/
    /** NIC **/
    /** MPLS **/
    /** EEE **/

    /** IOL **/
    /** MODEL TEST **/
    /** packet generation */

    /**testing cases**/
#ifdef CONFIG_SDK_MODEL_MODE
        case RTDRV_EXT_MODEL_TEST_SET:
            copy_from_user(&buf.model_cfg, user, sizeof(rtdrv_ext_modelCfg_t));
            vmac_setCaredICType(CARE_TYPE_REAL);
            ret = tc_exec(buf.model_cfg.startID, buf.model_cfg.endID);
            break;

       case RTDRV_EXT_MODEL_TARGET_SET:
           copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_ext_unitCfg_t));
           vmac_setCaredICType(CARE_TYPE_REAL);
           ret = vmac_setTarget(buf.unit_cfg.data);
           break;

      case RTDRV_EXT_MODEL_REG_ACCESS_SET:
          copy_from_user(&buf.unit_cfg, user, sizeof(rtdrv_ext_unitCfg_t));
          vmac_setCaredICType(CARE_TYPE_REAL);
          vmac_setRegAccessType(buf.unit_cfg.data);
          ret = RT_ERR_OK;
          break;
#endif
        default:
            break;
    }
#endif /*CONFIG_SDK_RTL8380*/

    copy_to_user(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    return 0;
}

/* Function Name:
 *      do_rtdrv_ext_get_ctl
 * Description:
 *      This function is called whenever a process tries to do getsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 * Output:
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_ext_get_ctl(struct sock *sk, int cmd, void *user_in, int *len)
{
    int32   ret = RT_ERR_FAILED;
#ifdef CONFIG_SDK_RTL8380

    switch(cmd)
    {
    /** INIT **/
    /** L2 **/
    /** PORT **/

    /** VLAN **/
    /** STP **/
    /** REG **/
    /** COUNTER **/
    /** TRAP **/
    /** FILTER **/
    /** PIE **/
    /** QOS **/
    /** TRUNK **/
    /** DOT1X **/
    /** FLOWCTRL **/
    /** RATE **/
    /** SVLAN **/
    /** SWITCH **/

    /** NIC **/
    /** MPLS **/
    /** EEE **/
    /** IOL **/
    /** MODEL TEST **/
#ifdef CONFIG_SDK_MODEL_MODE
        case RTDRV_EXT_MODEL_TARGET_GET:
            ret = vmac_getTarget(&value);
            buf.unit_cfg.data = value;
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_ext_unitCfg_t));
            break;
        case RTDRV_EXT_MODEL_REG_ACCESS_GET:
             vmac_getRegAccessType(&value);
            buf.unit_cfg.data = value;
            copy_to_user(user, &buf.unit_cfg, sizeof(rtdrv_ext_unitCfg_t));
            ret = RT_ERR_OK;
            break;
#endif

        /*** packet generation ***/
        default:
            break;
    }
#endif /*CONFIG_SDK_RTL8380*/

    copy_to_user(&((rtdrv_msgHdr_t *)user_in)->ret_code, &ret, sizeof(ret));

    return 0;
}

struct nf_sockopt_ops rtdrv_ext_sockopts = {
    .pf         = PF_INET,
    .set_optmin = RTDRV_EXT_BASE_CTL,
    .set_optmax = RTDRV_EXT_SET_MAX+1,
    .set        = do_rtdrv_ext_set_ctl,
    .get_optmin = RTDRV_EXT_BASE_CTL,
    .get_optmax = RTDRV_EXT_GET_MAX+1,
    .get        = do_rtdrv_ext_get_ctl,
};
