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
 * $Revision: 72502 $
 * $Date: 2016-10-17 15:06:44 +0800 (Mon, 17 Oct 2016) $
 *
 * Purpose : Define diag shell commands for Open vSwitch
 *
 * Feature : The file includes the following module and sub-modules
 *           1) Open vSwitch commands.
 *
 */

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_version.h>
#include <ioal/mem32.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

/*
 * ovs-vsctl help
 */
cparser_result_t cparser_cmd_ovs_vsctl_help(
    cparser_context_t *context)
{
    int32   ret;
    DIAG_UTIL_PARAM_CHK();

    ret = system("ovs-vsctl --help");
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}


/*
 * ovs-vsctl set-controller <STRING:bridge> <STRING:target>
 */
cparser_result_t cparser_cmd_ovs_vsctl_set_controller_bridge_target(cparser_context_t *context,
    char **bridge_ptr,
    char **target_ptr)
{
    int32   ret;
    char   *pBuffer = NULL;
    int     str_length = 128;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK(((strlen(*bridge_ptr) + strlen(*target_ptr))>= str_length), CPARSER_ERR_INVALID_PARAMS);

    if ((pBuffer = (char*)malloc(str_length)) == NULL)
    {
        diag_util_printf("malloc() fail!\n");
        return CPARSER_NOT_OK;
    }

    osal_sprintf(pBuffer, "ovs-vsctl set-controller %s %s", *bridge_ptr, *target_ptr);
    ret = system(pBuffer);

    if (RT_ERR_OK != ret)
    {
        free(pBuffer);
        return CPARSER_NOT_OK;
    }

    free(pBuffer);
    return CPARSER_OK;

}
cparser_result_t cparser_cmd_ovs_db_init(cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    system("ovsdb-tool create /usr/etc/openvswitch/conf.db /usr/share/openvswitch/vswitch.ovsschema");
    system("ovsdb-server --remote=punix:/usr/var/run/openvswitch/db.sock --remote=db:Open_vSwitch,Open_vSwitch,manager_options --pidfile --detach --log-file ovs-vswitchd &");

    return CPARSER_OK;

}



